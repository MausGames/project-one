///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 1: (mines need to be enemies to allow blinking, combo/chain)
// TODO 1: in die stacheln schießen erzeugt effekt (knusprig)
// TODO 1: implement high watermark in __AddEnemy
// TODO 1: make sure to disable wind on boss-death (hard if necessary)
// TODO 1: for bounce-bullets, change fly time or position, to make sure whole line disappears together (or switch to bounce count)
// TODO 1: sting calculations only for enabled stings
// TODO 1: raupen-spuren im sand bei einigen gegner-welle ndavor schon (foreshadowing)
// dying enemies flying towards boss doesn't feel right when you yourself are flying above boss
// all together: wind, mines, enemies, haubitze ... is just too much


// ****************************************************************
// counter identifier
#define WEAPON_SHOT (0u)


// ****************************************************************
// vector identifier
#define TRAIL_HIT (0u)   // # uses 0u - 1u


// ****************************************************************
// constructor
cTigerBoss::cTigerBoss()noexcept
: m_Sting         (TIGER_STINGS)
, m_afStingTime   {}
, m_avPushDir     {}
, m_fPushPower    (0.0f)
, m_bPushState    (false)
, m_iWeaponType   (0u)
, m_fWeaponChange (0.0f)
, m_aEnemyData    {}
, m_vGroundPos    (coreVector2(0.0f,0.0f))
, m_fAnimation    (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_tiger_body_high.md3");
    this->DefineModelLow ("ship_boss_tiger_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.5f);

    // configure the boss
    this->Configure(1000, COLOR_SHIP_YELLOW);
    this->AddStatus(ENEMY_STATUS_BOTTOM);

    // 
    m_Track.DefineModelHigh("ship_boss_tiger_track_high.md3");
    m_Track.DefineModelLow ("ship_boss_tiger_track_low.md3");
    m_Track.DefineTexture  (0u, "effect_track.png");
    m_Track.DefineProgram  ("effect_track_program");
    m_Track.SetSize        (this->GetSize());
    m_Track.SetTexSize     (coreVector2(1.0f,2.0f));
    m_Track.Configure      (1, coreVector3(1.0f,1.0f,1.0f));
    m_Track.AddStatus      (ENEMY_STATUS_BOTTOM);
    m_Track.SetParent      (this);

    // 
    m_Weapon.Configure(1, COLOR_SHIP_YELLOW);
    m_Weapon.AddStatus(ENEMY_STATUS_BOTTOM);
    m_Weapon.SetParent(this);

    // 
    m_WeaponOld.Configure(1, COLOR_SHIP_YELLOW);
    m_WeaponOld.AddStatus(ENEMY_STATUS_BOTTOM);
    m_WeaponOld.SetParent(this);

    // 
    m_Sting.DefineProgram("object_ship_glow_inst_program");
    {
        for(coreUintW i = 0u; i < TIGER_STINGS; ++i)
        {
            // load object resources
            coreObject3D* pSting = &m_aStingRaw[i];
            pSting->DefineModel  ("object_sting.md3");
            pSting->DefineTexture(0u, "ship_enemy.png");
            pSting->DefineProgram("object_ship_glow_program");

            // set object properties
            pSting->SetSize     (coreVector3(1.0f,1.0f,7.0f));
            pSting->SetDirection(coreVector3(0.0f,0.0f,1.0f));
            pSting->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.4f, 0.6f));
            pSting->SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Sting.BindObject(pSting);
        }
    }

    // 
    for(coreUintW i = 0u; i < TIGER_SIDES; ++i)
    {
        m_afStingTime[i] = -1.0f;
    }

    // 
    for(coreUintW i = 0u; i < TIGER_WEAPONS; ++i)
    {
        m_apModelHigh[i] = Core::Manager::Resource->Get<coreModel>(PRINT("ship_boss_tiger_weapon_%02zu_high.md3", i + 1u));
        m_apModelLow [i] = Core::Manager::Resource->Get<coreModel>(PRINT("ship_boss_tiger_weapon_%02zu_low.md3",  i + 1u));
    }

    // 
    m_aEnemyPath[0].Reserve(2u);
    m_aEnemyPath[0].AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
    m_aEnemyPath[0].AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
    m_aEnemyPath[0].Refine();

    m_aEnemyPath[1].Reserve(2u);
    m_aEnemyPath[1].AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
    m_aEnemyPath[1].AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
    m_aEnemyPath[1].Refine();

    // 
    this->__SwitchWeapon(0u);
}


// ****************************************************************
// 
void cTigerBoss::__ResurrectOwn()
{
    // 
    g_pGame->GetCrashManager()->SetImpactCallback([this](void* pData)
    {
        WARN_IF(!pData) return;

        const coreUint8 iPlayer = GET_BITVALUE(P_TO_UI(pData), 8u, 8u);
        const coreUint8 iDamage = GET_BITVALUE(P_TO_UI(pData), 8u, 0u);

        this->TakeDamage(iDamage, ELEMENT_NEUTRAL, HIDDEN_POS, g_pGame->GetPlayer(iPlayer));
    });
}


// ****************************************************************
// 
void cTigerBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < TIGER_SIDES; ++i)
        this->__DisableStings(i, bAnimated);

    // 
    g_pGame->GetCrashManager()->SetImpactCallback(NULL);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cTigerBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    glDepthFunc(GL_ALWAYS);
    {
        // 
        m_Sting.Render();
    }
    glDepthFunc(GL_LEQUAL);

    // 
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Sting);
}


// ****************************************************************
// 
void cTigerBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    if(this->ReachedDeath()) this->Kill(true);   

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);
    ASSERT(pSquad1->GetNumEnemies() == TIGER_ENEMIES)

    // 
    const coreVector2 vEnvDirection = g_pEnvironment->GetDirection();
    const coreFloat   fEnvSpeed     = g_pEnvironment->GetSpeed();
    const coreFloat   fEnvFlyOffset = g_pEnvironment->GetFlyOffset();

    // 
    if(fEnvFlyOffset * OUTDOOR_DETAIL < m_vGroundPos.y) m_vGroundPos.y -= I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL;

    // 
    const coreVector2 vPrevGroundPos = m_vGroundPos;
    m_vGroundPos = coreVector2(SIN(fEnvFlyOffset*0.075f*PI) * -12.5f, fEnvFlyOffset * OUTDOOR_DETAIL);

    // 
    const coreVector2 vBodyPos    = MapToAxis(coreVector2(m_vGroundPos.x, 0.0f), vEnvDirection);
    const coreFloat   fBodyHeight = LERP(this->GetPosition().z, g_pEnvironment->RetrieveSafeHeight(vBodyPos) + 2.0f, m_fLifeTimeBefore ? TIME : 1.0f);
    const coreVector2 vBodyDir    = TIME ? (MapToAxis(m_vGroundPos - vPrevGroundPos, vEnvDirection)).Normalized(this->GetDirection().xy()) : this->GetDirection().xy();

    // 
    this->SetPosition (coreVector3(vBodyPos, fBodyHeight));
    this->SetDirection(coreVector3(vBodyDir, 0.0f));

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
#if defined(_P1_VIDEO_)
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_LINEAR)
#else
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
#endif
        {
            if(PHASE_BEGINNING)
                this->_StartBoss();

#if defined(_P1_VIDEO_)
            if(PHASE_TIME_POINT(0.1f))
            {
                this->__EnableStings(0u);
                this->__EnableStings(1u);
                this->__EnableStings(2u);
                this->__EnableStings(3u);
            }
#endif

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->__EnableWind(coreVector2(-1.0f,0.0f));

                for(coreUintW i = 0u; i < 27u; ++i)
                {
                    const coreUintW x = i / 9u;
                    const coreUintW y = i % 9u;

                    const coreVector2 vPos = (coreVector2(I_TO_F(x) * 4.0f + ((y % 2u) ? -1.0f : 1.0f), (I_TO_F(y) - 4.0f) * 2.0f) * 0.16f + coreVector2(1.5f,0.0f)) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cMineBullet>(5, 0.0f, this, vPos, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->__DisableWind();

                for(coreUintW i = 0u; i < 5u; ++i)
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F(i) - 2.0f) * 0.15f - 0.6f, 0.0f);

                    this->__AddEnemy(1u, 0u, 1.0f, 0.2f * I_TO_F(i), vFactor, vOffset);
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        if(pSquad1->IsFinished())
        {
            PHASE_CHANGE_INC

            this->__SwitchWeapon(1u);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->__EnableWind(coreVector2(1.0f,0.0f));

                for(coreUintW i = 0u; i < 27u; ++i)
                {
                    const coreUintW x = i / 9u;
                    const coreUintW y = i % 9u;

                    const coreVector2 vPos = (coreVector2(I_TO_F(x) * 4.0f + ((y % 2u) ? -1.0f : 1.0f), (I_TO_F(y) - 4.0f) * 2.0f) * 0.16f + coreVector2(1.5f,0.0f)) * FOREGROUND_AREA.InvertedX();
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cMineBullet>(5, 0.0f, this, vPos, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->__DisableWind();

                for(coreUintW i = 0u; i < 10u; ++i)
                {
                    const coreVector2 vFactor = coreVector2(1.0f, (i < 5u) ? 1.0f : -1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F(i % 5u) - 2.0f) * 0.15f - 0.6f, 0.0f).InvertedX();

                    this->__AddEnemy(1u, 0u, 1.0f, 0.2f * I_TO_F(i % 5u), vFactor, vOffset);
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 13u)
    {
        if(pSquad1->IsFinished())
        {
            PHASE_CHANGE_INC

            this->__SwitchWeapon(2u);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 14u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->__EnableWind(coreVector2(0.0f,-1.0f));
            }

            if(PHASE_FINISHED)
            {
                //PHASE_CHANGE_INC

            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
        {
            if(iTick == 10u) this->__EnableStings(2u);            

            if(iTick >= 2u)
            {
                for(coreUintW i = 0u; i < 2u; ++i)
                {
                    const coreVector2 vPos = coreVector2(I_TO_F(i) * 1.0f + ((iTick % 2u) ? -0.75f : -0.25f), 1.3f) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cMineBullet>(5, 0.0f, this, vPos, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            }
        });

        PHASE_CONTROL_TICKER(2u, 0u, 4.0f, LERP_LINEAR)
        {
            if((iTick % 10u) < 5u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.9f,0.0f);

                this->__AddEnemy(2u, 3u, 1.0f, 0.0f, vFactor, vOffset);
            }
        });
    }

    // ################################################################
    // ################################################################

    if(m_iPhase >= 1u)
    {
        PHASE_CONTROL_TIMER(3u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                m_aiCounter[WEAPON_SHOT] += 1u;
            }

            const coreVector2 vAim    = this->AimAtPlayerDual((m_aiCounter[WEAPON_SHOT] / 2u) % 2u).Normalized();
            const coreVector2 vNewDir = SmoothAim(m_Weapon.GetDirection().xy(), vAim, 5.0f);

            m_Weapon.SetDirection(coreVector3(vNewDir, 0.0f));

            if(PHASE_FINISHED)
            {
                const coreVector2 vWeaponPos = m_Weapon.GetPosition ().xy();
                const coreVector2 vWeaponDir = m_Weapon.GetDirection().xy();
                const coreVector2 vHit       = vWeaponPos + vWeaponDir * g_pForeground->RayIntersection(vWeaponPos, vWeaponDir);

                if(m_iWeaponType == 0u)
                {
                    for(coreUintW i = 50u; i--; )
                    {
                        if((i % 10u) < 5u) continue;

                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) - 2.0f) * 3.6f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, this, vHit,  vDir)->ChangeSize(1.4f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, this, vHit, -vDir)->ChangeSize(1.4f);
                    }
                }
                else if(m_iWeaponType == 1u)
                {
                    const coreVector2 vDir = -AlongCrossNormal(vHit);

                    for(coreUintW i = 10u; i--; )
                    {
                        const coreVector2 vPos1 = vHit + vDir.Rotated90() * (I_TO_F(i) * 2.0f);
                        const coreVector2 vPos2 = vHit - vDir.Rotated90() * (I_TO_F(i) * 2.0f);

                              g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f - I_TO_F(i) * 0.035f, this, vPos1, vDir)->ChangeSize(1.3f);
                        if(i) g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f - I_TO_F(i) * 0.035f, this, vPos2, vDir)->ChangeSize(1.3f);
                    }
                }
                else if(m_iWeaponType == 2u)
                {
                    const coreVector2 vDir = IsHorizontal(vHit) ? vWeaponDir.InvertedX() : vWeaponDir.InvertedY();

                    for(coreUintW i = 10u; i--; )
                    {
                        const coreVector2 vPos = vHit - vDir * (I_TO_F(i) * 3.0f);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos, vDir)->ChangeSize(1.5f);
                    }
                }

                const coreVector3 vStart = m_Weapon.GetPosition() + m_Weapon.GetDirection() * 9.0f;
                const coreVector3 vDiff  = coreVector3(vHit, 0.0f) - vStart;
                const coreUintW   iNum   = F_TO_UI(vDiff.Length() / 1.9f);

                for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vStart + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum))), 10.0f, 1u, COLOR_ENERGY_WHITE);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vHit, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_WHITE);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                PHASE_RESET(3u)
            }
        });
    }

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
    {
        if(pBullet->GetFlyTime() < 8.0f)
        {
            coreVector2 vCurPos = pBullet->GetPosition().xy();
            coreVector2 vCurDir = pBullet->GetFlyDir();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.1f); vCurDir.x =  ABS(vCurDir.x);}
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.1f); vCurDir.x = -ABS(vCurDir.x);}
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.1f); vCurDir.y =  ABS(vCurDir.y);}
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.1f); vCurDir.y = -ABS(vCurDir.y);}

            pBullet->SetPosition(coreVector3(vCurPos, 0.0f));
            pBullet->SetFlyDir  (vCurDir);
        }
    });




    // TODO 1: move up, so new weapon-pos is used   
    // 
    m_fAnimation.Update(fEnvSpeed * -0.25f);

    // 
    m_Track.SetPosition   (this->GetPosition   ());
    m_Track.SetDirection  (this->GetDirection  ());
    m_Track.SetOrientation(this->GetOrientation());
    m_Track.SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation)));

    // 
    m_Weapon.SetPosition   (this->GetPosition   ());
    m_Weapon.SetOrientation(this->GetOrientation());

    if(!m_aiCounter[WEAPON_SHOT]) m_Weapon.SetDirection(this->GetDirection());          

    // 
    const coreVector2 vTrailPos1 = MapToAxisInv(vBodyPos - vBodyDir * 9.0f + vBodyDir.Rotated90() * 6.8f, vEnvDirection);
    const coreVector2 vTrailPos2 = MapToAxisInv(vBodyPos - vBodyDir * 9.0f - vBodyDir.Rotated90() * 6.8f, vEnvDirection);
    this->__CreateTrail(0u, coreVector3(vTrailPos1, g_pEnvironment->RetrieveSafeHeight(vTrailPos1) - 1.0f));
    this->__CreateTrail(1u, coreVector3(vTrailPos2, g_pEnvironment->RetrieveSafeHeight(vTrailPos2) - 1.0f));




    if(m_fWeaponChange < 3.0f)
    {
        m_fWeaponChange.Update(1.0f);

        m_Weapon.SetSize(this->GetSize() * LERPB(0.0f, 1.0f, MIN(m_fWeaponChange, 1.0f)));

        m_WeaponOld.SetPosition(m_WeaponOld.GetPosition() + coreVector3(vEnvDirection * (fEnvSpeed * TIME * -10.0f), 0.0f));
        m_WeaponOld.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    }
    else
    {
        m_WeaponOld.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

#if defined(_P1_VIDEO_)
    m_Weapon.SetAlpha(0.0f);
    m_WeaponOld.SetAlpha(0.0f);
#endif



    // 
    coreFloat afPrevStingTime[TIGER_SIDES];
    std::memcpy(afPrevStingTime, m_afStingTime, sizeof(m_afStingTime));

    for(coreUintW i = 0u; i < TIGER_SIDES; ++i)
    {
        if(m_afStingTime[i] >= 0.0f) m_afStingTime[i].UpdateMin(20.0f, 30.0f);
    }

    // 
    coreRand oStingRand(20u);
    for(coreUintW i = 0u; i < TIGER_STINGS; ++i)
    {
        const coreVector2 vSide  = (i < TIGER_STINGS_SIDE * 2u) ? coreVector2((i < TIGER_STINGS_SIDE * 1u) ? -1.0f : 1.0f, 0.0f) : coreVector2(0.0f, (i < TIGER_STINGS_SIDE * 3u) ? -1.0f : 1.0f);
        const coreFloat   fShift = I_TO_F(i % TIGER_STINGS_SIDE) - I_TO_F(TIGER_STINGS_SIDE - 1u) * 0.5f;

        const coreUintW iIndex      = i / TIGER_STINGS_SIDE;
        const coreFloat fPrevExtend = CLAMP((afPrevStingTime[iIndex] - ABS(fShift) - 1.0f) * 0.3f, 0.0f, 1.0f);
        const coreFloat fExtend     = CLAMP((m_afStingTime  [iIndex] - ABS(fShift) - 1.0f) * 0.3f, 0.0f, 1.0f);

        const coreVector2 vBase = (vSide.Rotated90() * (fShift * 0.062f) + vSide) * FOREGROUND_AREA * 1.15f;
        const coreVector2 vDir  = (-vBase.Normalized() + 0.5f * coreVector2(oStingRand.Float(-1.0f, 1.0f), oStingRand.Float(-1.0f, 1.0f)).Normalized()).Normalized();
        const coreVector2 vPos  = vBase + vDir * LERP(-12.0f, 0.0f, fExtend);

        m_aStingRaw[i].SetPosition   (coreVector3(vPos, 0.0f));
        m_aStingRaw[i].SetOrientation(coreVector3(vDir, 0.0f));
        m_aStingRaw[i].SetEnabled    (fExtend ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        if((fPrevExtend < 0.3f) && (fExtend >= 0.3f)) g_pSpecialEffects->CreateSplashColor(coreVector3(vBase + vDir * 2.0f, 0.0f), 5.0f, 3u, COLOR_ENERGY_WHITE);
        
        if((fPrevExtend < 0.3f) && (fExtend >= 0.3f)) g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
    }

    // 
    m_Sting.MoveNormal();

    // 
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        if(pPlayer->IsNormal())
        {
            const coreVector2 vPos = pPlayer->GetPosition().xy();

            if(((vPos.x <= -FOREGROUND_AREA.x + CORE_MATH_PRECISION) && (m_afStingTime[0] >= 30.0f)) ||
               ((vPos.x >=  FOREGROUND_AREA.x - CORE_MATH_PRECISION) && (m_afStingTime[1] >= 30.0f)) ||
               ((vPos.y <= -FOREGROUND_AREA.y + CORE_MATH_PRECISION) && (m_afStingTime[2] >= 30.0f)) ||
               ((vPos.y >=  FOREGROUND_AREA.y - CORE_MATH_PRECISION) && (m_afStingTime[3] >= 30.0f)))
            {
                pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vPos);
            }
        }
    });




    // 
    if(m_bPushState) m_fPushPower.UpdateMin( 1.0f, 1.0f);
                else m_fPushPower.UpdateMax(-1.0f, 0.0f);

    // 
    if(m_fPushPower)
    {
        // 
        const coreFloat fPower = LERPH3(0.0f, 1.0f, m_fPushPower);

        // 
        const coreVector2 vRealPushDir = MapToAxis(m_avPushDir[1], vEnvDirection);

        // 
        m_avPushDir[0] = LERP(m_avPushDir[0], m_avPushDir[1], TIME * 3.0f);
        d_cast<cDesertBackground*>(g_pEnvironment->GetBackground())->SetSandMove(m_avPushDir[0] * (1.0f + 3.0f * fPower));

        // 
        const coreVector2 vPlayerForce = vRealPushDir * (200.0f * fPower);
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ApplyForceTimed(vPlayerForce);
        });

        // 
        const coreVector2 vBulletForce = vRealPushDir * (TIME * 16.0f * fPower);
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cMineBullet>([&](cMineBullet* OUTPUT pBullet)
        {
            const coreVector2 vPos = pBullet->GetPosition().xy() + vBulletForce;

            if(((vPos.x < -FOREGROUND_AREA.x * 1.2f) && (vBulletForce.x < 0.0f)) ||
               ((vPos.x >  FOREGROUND_AREA.x * 1.2f) && (vBulletForce.x > 0.0f)) ||
               ((vPos.y < -FOREGROUND_AREA.y * 1.2f) && (vBulletForce.y < 0.0f)) ||
               ((vPos.y >  FOREGROUND_AREA.y * 1.2f) && (vBulletForce.y > 0.0f)))
            {
                pBullet->Deactivate(false);
            }

            pBullet->SetPosition(coreVector3(vPos, 0.0f));
        });
    }





    // (stage functions)   
    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
    {
        STAGE_LIFETIME(pEnemy, m_aEnemyData[i].fTimeFactor, m_aEnemyData[i].fTimeOffset)

        const coreUint8 iType = m_aEnemyData[i].iType - 1u;

        const coreSpline2* pPath = &m_aEnemyPath[iType];

        const coreVector2 vFactor = m_aEnemyData[i].vPosFactor;
        const coreVector2 vOffset = m_aEnemyData[i].vPosOffset;

        const coreBool bFinished = pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

        switch(m_aEnemyData[i].iRota)
        {
        default: ASSERT(false)
        case 0u:                      break;
        case 1u: pEnemy->Rotate90 (); break;
        case 2u: pEnemy->Rotate180(); break;
        case 3u: pEnemy->Rotate270(); break;
        }

        if(bFinished && (iType != 0u))
        {
            this->__DeleteEnemy(i, false);
        }
        else if(pEnemy->ReachedDeath())
        {
            this->__DeleteEnemy(i, true);
        }
    });
}


// ****************************************************************
// 
void cTigerBoss::__EnableStings(const coreUintW iIndex)
{
    ASSERT(iIndex < TIGER_SIDES)

    //   3
    // 0   1
    //   2

    // 
    ASSERT(m_afStingTime[iIndex] < 0.0f)
    m_afStingTime[iIndex] = 0.0f;
}


// ****************************************************************
// 
void cTigerBoss::__DisableStings(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TIGER_SIDES)

    // 
    const coreUintW iFrom = TIGER_STINGS_SIDE * (iIndex);
    const coreUintW iTo   = TIGER_STINGS_SIDE * (iIndex + 1u);
    ASSERT(iTo <= TIGER_STINGS)

    // 
    for(coreUintW i = iFrom; i < iTo; ++i)
    {
        m_aStingRaw[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_afStingTime[iIndex] = -1.0f;

    // TODO 1: implement animation 
}


// ****************************************************************
// 
void cTigerBoss::__EnableWind(const coreVector2 vDir)
{
    ASSERT(vDir.IsNormalized())

    // 
    m_avPushDir[0] = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground())->GetSandMove().Normalized();
    m_avPushDir[1] = vDir;
    m_bPushState   = true;
}


// ****************************************************************
// 
void cTigerBoss::__DisableWind()
{
    // 
    m_bPushState = false;
}


// ****************************************************************
// 
void cTigerBoss::__SwitchWeapon(const coreUintW iType)
{
    ASSERT(iType < TIGER_WEAPONS)

    // 
    m_WeaponOld.DefineModelHigh(m_Weapon.GetModelHigh  ());
    m_WeaponOld.DefineModelLow (m_Weapon.GetModelLow   ());
    m_WeaponOld.SetPosition    (m_Weapon.GetPosition   ());
    m_WeaponOld.SetSize        (m_Weapon.GetSize       ());
    m_WeaponOld.SetDirection   (m_Weapon.GetDirection  ());
    m_WeaponOld.SetOrientation (m_Weapon.GetOrientation());

    // 
    m_Weapon.DefineModelHigh(m_apModelHigh[iType]);
    m_Weapon.DefineModelLow (m_apModelLow [iType]);

    // 
    m_iWeaponType   = iType;
    m_fWeaponChange = 0.0f;

    // 
    PHASE_RESET(3u)
}


// ****************************************************************
// 
void cTigerBoss::__AddEnemy(const coreUint8 iType, const coreUint8 iRota, const coreFloat fTimeFactor, const coreFloat fTimeOffset, const coreVector2 vPosFactor, const coreVector2 vPosOffset)
{
    ASSERT(iType)

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);

    // 
    for(coreUintW i = 0u; i < TIGER_ENEMIES; ++i)
    {
        if(!m_aEnemyData[i].iType)
        {
            // 
            m_aEnemyData[i].iType       = iType;
            m_aEnemyData[i].iRota       = iRota;
            m_aEnemyData[i].fTimeFactor = fTimeFactor;
            m_aEnemyData[i].fTimeOffset = fTimeOffset;
            m_aEnemyData[i].vPosFactor  = vPosFactor;
            m_aEnemyData[i].vPosOffset  = vPosOffset;

            // 
            pSquad1->GetEnemy(i)->Resurrect();
            return;
        }
    }

    WARN_IF(true) {}
}


// ****************************************************************
// 
void cTigerBoss::__DeleteEnemy(const coreUintW iIndex, const coreBool bCrash)
{
    ASSERT(iIndex < TIGER_ENEMIES)

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);

    // 
    ASSERT(m_aEnemyData[iIndex].iType)
    m_aEnemyData[iIndex].iType = 0u;

    // 
    cEnemy* pEnemy = pSquad1->GetEnemy(iIndex);
    pEnemy->Kill(false);

    // 
    if(bCrash)
    {
        const coreUint8 iPlayer = pEnemy->LastAttacker() - g_pGame->GetPlayer(0u);
        const coreUint8 iDamage = 10u;

        g_pGame->GetCrashManager()->AddCrash(*pEnemy, this->GetPosition().xy(), I_TO_P(BITVALUE(8u, 8u, iPlayer) | BITVALUE(8u, 0u, iDamage)));
    }
}


// ****************************************************************
// 
void cTigerBoss::__CreateTrail(const coreUintW iIndex, const coreVector3 vIntersect)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)

    // 
    constexpr coreFloat fMin = 1.7f;
    coreVector3 vOldHit = m_avVector[TRAIL_HIT + iIndex].xyz();

    // 
    if(vOldHit.IsNull()) vOldHit = vIntersect;
    else
    {
        while(true)
        {
            // 
            const coreVector3 vDiff = vIntersect - vOldHit;
            const coreFloat   fLen  = vDiff.Length();

            // 
            if(fLen < fMin) break;

            // 
            const coreVector3 vNewHit      = LERP(vOldHit, vIntersect, fMin*RCP(fLen));
            const coreVector2 vOldOnScreen = g_pForeground->Project2D(vOldHit);
            const coreVector2 vNewOnScreen = g_pForeground->Project2D(vNewHit);

            // 
            if(((ABS(vOldOnScreen.x) < 0.55f) && (ABS(vOldOnScreen.y) < 0.55f)) ||
               ((ABS(vNewOnScreen.x) < 0.55f) && (ABS(vNewOnScreen.y) < 0.55f)))
            {
                // 
                const coreVector3 vDecalPos  = (vOldHit + vNewHit) * 0.5f;
                const coreVector2 vDecalSize = coreVector2(3.6f, fMin*0.7f);
                const coreVector2 vDecalDir  = vDiff.xy().Normalized();

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "default_white.png");
                pObject->DefineProgram("effect_decal_single_program");

                // set object properties
                pObject->SetSize     (coreVector3(vDecalSize, 1.0f));
                pObject->SetDirection(coreVector3(vDecalDir,  0.0f));
                pObject->SetColor4   (coreVector4(0.0f,0.0f,0.0f,0.5f));

                // add object to background
                g_pEnvironment->GetBackground()->AddDecal(pObject, vDecalPos, 256u, "effect_decal_single_inst_program", LIST_KEY);
            }

            // 
            vOldHit = vNewHit;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * TIME * OUTDOOR_DETAIL;

    // 
    m_avVector[TRAIL_HIT + iIndex].xyz(vOldHit);
}