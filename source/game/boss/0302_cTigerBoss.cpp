///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// dying enemies flying towards boss doesn't feel right when you yourself are flying above boss
// all together: wind, mines, enemies, haubitze ... is just too much
// it is very harder to find a perfect safe-spot, because of the tank-movement
// TODO 1: (mines need to be enemies to allow blinking, combo/chain)
// TODO 1: in die stacheln schießen erzeugt effekt (knusprig)
// TODO 1: implement high watermark in __AddEnemy
// TODO 1: make sure to disable wind on boss-death (hard if necessary)
// TODO 1: for bounce-bullets, change fly time or position, to make sure whole line disappears together (or switch to bounce count)
// TODO 1: sting calculations only for enabled stings
// TODO 1: raupen-spuren im sand bei einigen gegner-welle davor schon (foreshadowing)
// TODO 1: every weapon needs an own design, own number on top of it, own color?
// TODO 1: boss sollte immer im selben background offset starten (zm. für die sinus-welle) -> baue sync-punkt ein -> ich hab eh den sinus-wert allein wegen der bewegung
// TODO 1: arrow für schussrichtung ? (gelb)
// TODO 1: manche gegner im level sollen schon abstürzen
// TODO 1: fixup scoring (enemies or damaging boss makes score ? how to handle chain ?)
// TODO 1: MAIN: fragment, easy, hard (decision), coop, 3 badges, boss health, medal goal, intro, outro, foreshadow
// TODO 1: reifen unrealistisch animieren, damit sie bei hoher geschwindigkeit noch gut aussehen ?
// TODO 1: hard-mode: wind + minen
// TODO 1: make sure health is same in coop and non-coop
// TODO 1: improve the pattern for the spike phase


// ****************************************************************
// counter identifier
#define WEAPON_SHOT (0u)


// ****************************************************************
// vector identifier
#define TRAIL_HIT   (0u)   // # uses 0u - 1u
#define RECOIL_TIME (2u)
#define POS_OFFSET  (3u)


// ****************************************************************
// constructor
cTigerBoss::cTigerBoss()noexcept
: m_Sting          (TIGER_STINGS)
, m_afStingTime    {}
, m_avPushDir      {}
, m_fPushPower     (0.0f)
, m_bPushState     (false)
, m_iWeaponType    (0u)
, m_iWeaponTypeOld (0u)
, m_fWeaponChange  (0.0f)
, m_vGroundPos     (coreVector2(0.0f,0.0f))
, m_fAnimation     (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_tiger_body_high.md3");
    this->DefineModelLow ("ship_boss_tiger_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.5f);

    // configure the boss
    this->Configure(TIGER_DAMAGE * 104, COLOR_SHIP_YELLOW);
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

    for(coreUintW i = 0u; i < TIGER_SUBS; ++i)
    {
        // 
        m_aWeapon[i].Configure(1, COLOR_SHIP_YELLOW);
        m_aWeapon[i].AddStatus(ENEMY_STATUS_BOTTOM);
        m_aWeapon[i].SetParent(this);
    }

    for(coreUintW i = 0u; i < TIGER_SUBS; ++i)
    {
        // 
        m_aWeaponOld[i].Configure(1, COLOR_SHIP_YELLOW);
        m_aWeaponOld[i].AddStatus(ENEMY_STATUS_BOTTOM);
        m_aWeaponOld[i].SetParent(this);
    }

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
    constexpr coreUintW aiSubs[] = {2u, 2u, 1u, 2u, 1u};
    STATIC_ASSERT((ARRAY_SIZE(aiSubs) == TIGER_WEAPONS) && std::any_of(aiSubs, aiSubs + ARRAY_SIZE(aiSubs), [](const coreUintW A) {return (A > 0u) && (A <= TIGER_SUBS);}))

    // 
    for(coreUintW i = 0u; i < TIGER_WEAPONS; ++i)
    {
        for(coreUintW j = 0u, je = aiSubs[i]; j < je; ++j)
        {
            m_aapModelHigh[i][j] = Core::Manager::Resource->Get<coreModel>(PRINT("ship_boss_tiger_weapon_%02zu_%02zu_high.md3", i + 1u, j + 1u));
            m_aapModelLow [i][j] = Core::Manager::Resource->Get<coreModel>(PRINT("ship_boss_tiger_weapon_%02zu_%02zu_low.md3",  i + 1u, j + 1u));
        }
    }
    m_aapModelHigh[1][2] = m_aapModelHigh[1][1];
    m_aapModelLow [1][2] = m_aapModelLow [1][1];

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
        const coreUint8 iDamage = GET_BITVALUE(P_TO_UI(pData), 8u, 0u) / 2u * g_pGame->GetNumPlayers();

        this->TakeDamage(iDamage, ELEMENT_NEUTRAL, HIDDEN_POS, g_pGame->GetPlayer(iPlayer));

        if((m_iPhase == 50u) && (this->GetCurHealth() <= TIGER_DAMAGE * 16)) this->AddStatus(ENEMY_STATUS_INVINCIBLE);
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

    cHarenaMission* pMission = d_cast<cHarenaMission*>(g_pGame->GetCurMission());

    if(this->ReachedDeath()) this->Kill(true);   

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(-80.0f, -40.0f, fTime);

            if(PHASE_TIME_POINT(0.7f))
                this->_StartBoss();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(1u);

            PHASE_CHANGE_TO(20u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        if(pMission->GetStageSub() == 11u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(-40.0f, 0.0f, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 22u)
    {
        if(pMission->GetStageSub() == 12u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 23u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(0.0f, 40.0f, fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__SwitchWeapon(2u);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 24u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(2u);

            PHASE_CHANGE_TO(30u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        if(pMission->GetStageSub() == 2u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_SMOOTH)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(fTime * (-1.5f*PI)));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        if(pMission->GetStageSub() == 7u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(40.0f, 0.0f, fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__SwitchWeapon(3u);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 34u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(3u);

            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        if(pMission->GetStageSub() == 7u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_SMOOTH)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(fTime * (-1.5f*PI) + (-1.5f*PI)));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 42u)
    {
        if(pMission->GetStageSub() == 17u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 43u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            this->__SwitchWeapon(1u);

            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 44u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(4u);

            PHASE_CHANGE_TO(50u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        if(this->GetCurHealth() <= TIGER_DAMAGE * 16)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.4f)   // longer
        {
            this->__SwitchWeapon(4u);

            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 52u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.2f)   // longer
        {
            pMission->ChangeInsanity(5u);

            this->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

            PHASE_CHANGE_TO(60u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        const coreFloat fRotaTime = BLENDBR(MIN1(m_fPhaseTime * 0.5f)) * 2.0f + MAX0(m_fPhaseTime - 2.0f);
        g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(fRotaTime * (-0.1f*PI) + (-3.0f*PI)));
    }

    // ################################################################
    // ################################################################

    // 
    const coreVector2 vEnvDirection = g_pEnvironment->GetDirection();
    const coreFloat   fEnvSpeed     = g_pEnvironment->GetSpeed();
    const coreFloat   fEnvFlyOffset = g_pEnvironment->GetFlyOffset();

    // 
    if(fEnvFlyOffset * OUTDOOR_DETAIL < m_vGroundPos.y) m_vGroundPos.y -= I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL;

    // 
    const coreVector2 vPrevGroundPos = m_vGroundPos;
    m_vGroundPos = coreVector2(SIN((fEnvFlyOffset + m_avVector[POS_OFFSET].y / OUTDOOR_DETAIL) * (0.075f*PI)) * -12.5f, fEnvFlyOffset * OUTDOOR_DETAIL);

    // 
    const coreVector2 vBodyPos    = MapToAxis(coreVector2(m_vGroundPos.x, m_avVector[POS_OFFSET].y), vEnvDirection);
    const coreFloat   fBodyHeight = LERP(this->GetPosition().z, g_pEnvironment->RetrieveSafeHeight(vBodyPos) + 2.0f, m_fLifeTimeBefore ? TIME : 1.0f);
    const coreVector2 vBodyDir    = TIME ? (MapToAxis(m_vGroundPos - vPrevGroundPos, vEnvDirection)).Normalized(this->GetDirection().xy()) : this->GetDirection().xy();

    // 
    this->SetPosition (coreVector3(vBodyPos, fBodyHeight));
    this->SetDirection(coreVector3(vBodyDir, 0.0f));

    if(m_iPhase >= 1u)
    {
        if(m_iWeaponType < 4u)
        {
            coreFloat fShootSpeed;
            switch(m_iWeaponType)
            {
            default: ASSERT(false)
            case 0u: fShootSpeed = 0.5f; break;
            case 1u: fShootSpeed = 2.0f; break;
            case 2u: fShootSpeed = 1.0f; break;
            case 3u: fShootSpeed = 0.5f; break;
            }

            PHASE_CONTROL_TIMER(3u, fShootSpeed, LERP_LINEAR)
            {
                if(PHASE_BEGINNING)
                {
                    m_aiCounter[WEAPON_SHOT] += 1;
                }

                const coreVector2 vAim    = this->AimAtPlayerDual((m_aiCounter[WEAPON_SHOT] / 2) % 2).Normalized();
                const coreVector2 vNewDir = SmoothAim(m_aWeapon[0].GetDirection().xy(), vAim, 5.0f);

                m_aWeapon[0].SetDirection(coreVector3(vNewDir, 0.0f));

                if(PHASE_FINISHED)
                {
                    if((m_iWeaponType != 1u) || ((m_aiCounter[WEAPON_SHOT] - 1) % 4 >= 2))
                    {
                        const coreVector2 vWeaponPos = m_aWeapon[0].GetPosition ().xy();
                        const coreVector2 vWeaponDir = m_aWeapon[0].GetDirection().xy();
                        const coreVector2 vHit       = vWeaponPos + vWeaponDir * g_pForeground->RayIntersection(vWeaponPos, vWeaponDir);

                        if(m_iWeaponType == 0u)
                        {
                            const coreVector2 vBase = -AlongCrossNormal(vHit).Rotated90();

                            for(coreUintW i = 37u; i--; )
                            {
                                const coreVector2 vDir = MapToAxis(coreVector2::Direction(I_TO_F(i) * (2.0f*PI) / 72.0f), vBase);

                                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 2.0f, this, vHit, vDir)->ChangeSize(1.6f);
                            }

                            m_avVector[RECOIL_TIME].x = 1.0f;
                        }
                        else if(m_iWeaponType == 1u)
                        {
                            const coreVector2 vDir = -AlongCrossNormal(vHit);

                            for(coreUintW i = 6u; i--; )
                            {
                                const coreVector2 vPos1 = vHit + vDir.Rotated90() * (I_TO_F(i) * 2.2f);
                                const coreVector2 vPos2 = vHit - vDir.Rotated90() * (I_TO_F(i) * 2.2f);

                                      g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f - I_TO_F(i) * 0.035f, this, vPos1, vDir)->ChangeSize(1.5f);
                                if(i) g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f - I_TO_F(i) * 0.035f, this, vPos2, vDir)->ChangeSize(1.5f);
                            }

                            m_avVector[RECOIL_TIME].arr(m_aiCounter[WEAPON_SHOT] % 2) = 1.0f;
                        }
                        else if(m_iWeaponType == 2u)
                        {
                            const coreVector2 vDir = IsHorizontal(vHit) ? vWeaponDir.InvertedX() : vWeaponDir.InvertedY();

                            for(coreUintW i = 8u; i--; )
                            {
                                const coreVector2 vPos = vHit - vDir * (I_TO_F(i) * 3.0f);

                                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.4f, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                            }
                        }
                        else if(m_iWeaponType == 3u)
                        {
                            for(coreUintW i = 50u; i--; )
                            {
                                if((i % 10u) < 2u) continue;

                                const coreVector2 vDir   = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) - 2.0f) * 3.6f));
                                const coreFloat   fSpeed = LERP(0.7f, 1.0f, I_TO_F(i % 10u) * 0.1f);

                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, fSpeed, this, vHit,  vDir)->ChangeSize(1.5f);
                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, fSpeed, this, vHit, -vDir)->ChangeSize(1.5f);
                            }

                            m_avVector[RECOIL_TIME].x = 1.0f;
                        }
                        else ASSERT(false)

                        const coreVector3 vStart = m_aWeapon[0].GetPosition() + m_aWeapon[0].GetDirection() * 9.0f;
                        const coreVector3 vDiff  = coreVector3(vHit, 0.0f) - vStart;
                        const coreUintW   iNum   = MAX(F_TO_UI(vDiff.Length() / 1.9f), 2u);

                        for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vStart + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, COLOR_ENERGY_WHITE);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vHit, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_WHITE);
                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    }

                    PHASE_RESET(3u)
                }
            });
        }
        else if(m_iWeaponType == 4u)
        {
            PHASE_CONTROL_TIMER(3u, 1.0f, LERP_LINEAR)
            {
                if(PHASE_BEGINNING)
                {
                    m_aiCounter[WEAPON_SHOT] += 1;
                }

                const coreVector2 vNewDir = coreVector2::Direction(m_fLifeTime);

                m_aWeapon[0].SetDirection(coreVector3(vNewDir, 0.0f));

                if(PHASE_FINISHED)
                {
                    if(m_aiCounter[WEAPON_SHOT] % 4 == 2)
                    {
                        for(coreUintW i = 4u; i--; )
                        {
                            const coreVector2 vDir = MapToAxis(vNewDir, coreVector2::Direction(I_TO_F(i) * (0.5f*PI)));
                            const coreVector2 vPos = m_aWeapon[0].GetPosition().xy() + vDir * 3.0f;

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cRocketBullet>(5, 1.0f, this, vPos, vDir)->SetTarget(this->NearestPlayerDual(m_aiCounter[WEAPON_SHOT] % 2))->ChangeSize(1.4f);
                        }
                    }

                    PHASE_RESET(3u)
                }
            });
        }
        else ASSERT(false)
    }

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
    {
        if(pBullet->GetDamage() > 2)
        {
            coreVector2 vCurPos = pBullet->GetPosition().xy();
            coreVector2 vCurDir = pBullet->GetFlyDir();
            coreInt32   iDamage = pBullet->GetDamage();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.1f); vCurDir.x =  ABS(vCurDir.x); iDamage -= 1;}
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.1f); vCurDir.x = -ABS(vCurDir.x); iDamage -= 1;}
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.1f); vCurDir.y =  ABS(vCurDir.y); iDamage -= 1;}
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.1f); vCurDir.y = -ABS(vCurDir.y); iDamage -= 1;}

            pBullet->SetPosition(coreVector3(vCurPos, 0.0f));
            pBullet->SetFlyDir  (vCurDir);
            pBullet->SetDamage  (iDamage);
        }
        else pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
    });

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([&](cConeBullet* OUTPUT pBullet)
    {
        const coreFloat fNewSpeed = pBullet->GetSpeed() - 2.0f * TIME;

        pBullet->SetSpeed(fNewSpeed);
        if(fNewSpeed <= 0.0f) pBullet->Deactivate(true);
    });




    // TODO 1: move up, so new weapon-pos is used   
    // 
    m_fAnimation.Update(fEnvSpeed * -0.25f);

    // 
    m_Track.SetPosition   (this->GetPosition   ());
    m_Track.SetDirection  (this->GetDirection  ());
    m_Track.SetOrientation(this->GetOrientation());
    m_Track.SetColor3     (coreVector3(1.0f,1.0f,1.0f));   // override brightness
    m_Track.SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation)));

    // 
    m_aWeapon[0].SetPosition   (this->GetPosition   ());
    m_aWeapon[0].SetOrientation(this->GetOrientation());

    if(!m_aiCounter[WEAPON_SHOT]) m_aWeapon[0].SetDirection(this->GetDirection());          

    // 
    const coreVector2 vTrailPos1 = vBodyPos - vBodyDir * 8.0f + vBodyDir.Rotated90() * 6.8f;
    const coreVector2 vTrailPos2 = vBodyPos - vBodyDir * 8.0f - vBodyDir.Rotated90() * 6.8f;
    this->__CreateTrail(0u, coreVector3(vTrailPos1, g_pEnvironment->RetrieveSafeHeight(vTrailPos1) - 1.0f));
    this->__CreateTrail(1u, coreVector3(vTrailPos2, g_pEnvironment->RetrieveSafeHeight(vTrailPos2) - 1.0f));



    if(m_fWeaponChange < 3.0f)
    {
        m_fWeaponChange.Update(1.0f);

        m_aWeapon[0].SetSize(this->GetSize() * LERPB(0.0f, 1.0f, MIN(m_fWeaponChange, 1.0f)) * 1.3f);

        const coreVector3 vPos  = m_aWeaponOld[0].GetPosition() + coreVector3(vEnvDirection * (fEnvSpeed * TIME * -10.0f), 0.0f);
        const coreMatrix3 mRota = coreMatrix4::RotationZ(-15.0f * TIME).m123();

        m_aWeaponOld[0].SetPosition   (vPos);
        m_aWeaponOld[0].SetDirection  (m_aWeaponOld[0].GetDirection  () * mRota);
        m_aWeaponOld[0].SetOrientation(m_aWeaponOld[0].GetOrientation() * mRota);

        m_aWeaponOld[0].RemoveStatus(ENEMY_STATUS_HIDDEN);
    }
    else
    {
        m_aWeaponOld[0].AddStatus(ENEMY_STATUS_HIDDEN);
    }


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

        if((fPrevExtend < 0.3f) && (fExtend >= 0.3f))
        {
            g_pSpecialEffects->CreateSplashColor(coreVector3(vBase + vDir * 2.0f, 0.0f), 5.0f, 3u, COLOR_ENERGY_WHITE);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
        }
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


    for(coreUintW i = 1u; i < TIGER_SUBS; ++i)
    {
        m_aWeapon[i].SetPosition   (m_aWeapon[0].GetPosition   ());
        m_aWeapon[i].SetSize       (m_aWeapon[0].GetSize       ());
        m_aWeapon[i].SetDirection  (m_aWeapon[0].GetDirection  ());
        m_aWeapon[i].SetOrientation(m_aWeapon[0].GetOrientation());

        m_aWeaponOld[i].SetPosition   (m_aWeaponOld[0].GetPosition   ());
        m_aWeaponOld[i].SetSize       (m_aWeaponOld[0].GetSize       ());
        m_aWeaponOld[i].SetDirection  (m_aWeaponOld[0].GetDirection  ());
        m_aWeaponOld[i].SetOrientation(m_aWeaponOld[0].GetOrientation());

        if(!m_aWeapon[i].GetModel() || m_aWeapon[0].HasStatus(ENEMY_STATUS_HIDDEN))
             m_aWeapon[i].AddStatus   (ENEMY_STATUS_HIDDEN);
        else m_aWeapon[i].RemoveStatus(ENEMY_STATUS_HIDDEN);

        if(!m_aWeaponOld[i].GetModel() || m_aWeaponOld[0].HasStatus(ENEMY_STATUS_HIDDEN))
             m_aWeaponOld[i].AddStatus   (ENEMY_STATUS_HIDDEN);
        else m_aWeaponOld[i].RemoveStatus(ENEMY_STATUS_HIDDEN);
    }
    
    if(m_iWeaponType == 1u)
    {
        const coreVector3 vOffset = m_aWeapon[0].GetDirection().RotatedZ90() * 1.55f;

        m_aWeapon[1].SetPosition(m_aWeapon[0].GetPosition() + vOffset);
        m_aWeapon[2].SetPosition(m_aWeapon[0].GetPosition() - vOffset);
    }
    if(m_iWeaponTypeOld == 1u)
    {
        const coreVector3 vOffset = m_aWeaponOld[0].GetDirection().RotatedZ90() * 1.55f;

        m_aWeaponOld[1].SetPosition(m_aWeaponOld[0].GetPosition() + vOffset);
        m_aWeaponOld[2].SetPosition(m_aWeaponOld[0].GetPosition() - vOffset);
    }

    m_avVector[RECOIL_TIME].xy((m_avVector[RECOIL_TIME].xy() - 1.0f * TIME).Processed(MAX, 0.0f));

    if(m_iWeaponType == 0u)
    {
        m_aWeapon[1].SetPosition(m_aWeapon[1].GetPosition() - m_aWeapon[1].GetDirection() * (2.8f * m_aWeapon[1].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x)));
    }
    else if(m_iWeaponType == 1u)
    {
        m_aWeapon[1].SetPosition(m_aWeapon[1].GetPosition() - m_aWeapon[1].GetDirection() * (2.5f * m_aWeapon[1].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x)));
        m_aWeapon[2].SetPosition(m_aWeapon[2].GetPosition() - m_aWeapon[2].GetDirection() * (2.5f * m_aWeapon[2].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].y)));
    }
    else if(m_iWeaponType == 3u)
    {
        m_aWeapon[1].SetPosition(m_aWeapon[1].GetPosition() - m_aWeapon[1].GetDirection() * (2.8f * m_aWeapon[1].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x)));
    }
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

    for(coreUintW i = 0u; i < TIGER_SUBS; ++i)
    {
        // 
        m_aWeaponOld[i].DefineModelHigh(m_aWeapon[i].GetModelHigh  ());
        m_aWeaponOld[i].DefineModelLow (m_aWeapon[i].GetModelLow   ());
        m_aWeaponOld[i].SetPosition    (m_aWeapon[i].GetPosition   ());
        m_aWeaponOld[i].SetSize        (m_aWeapon[i].GetSize       ());
        m_aWeaponOld[i].SetDirection   (m_aWeapon[i].GetDirection  ());
        m_aWeaponOld[i].SetOrientation (m_aWeapon[i].GetOrientation());

        // 
        m_aWeapon[i].DefineModelHigh(m_aapModelHigh[iType][i]);
        m_aWeapon[i].DefineModelLow (m_aapModelLow [iType][i]);
    }

    // 
    m_iWeaponTypeOld = m_iWeaponType;
    m_iWeaponType    = iType;
    m_fWeaponChange  = 0.0f;


    // 
    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(this->GetPosition(), COLOR_FIRE_ORANGE);

    m_aiCounter[WEAPON_SHOT] = 0;   // TODO 1: wave-weapon might be activated too early
    
    // 
    PHASE_RESET(3u)
}


// ****************************************************************
// 
void cTigerBoss::__CreateTrail(const coreUintW iIndex, const coreVector3 vIntersect)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)

    // always track intersection without rotation (to account for boss rotating with background)
    const coreVector2 vEnvDirection    = g_pEnvironment->GetDirection();
    const coreVector3 vRelIntersection = MapToAxisInv(vIntersect, vEnvDirection);

    // 
    constexpr coreFloat fMin = 1.7f;
    coreVector3 vOldHit = m_avVector[TRAIL_HIT + iIndex].xyz();

    // 
    if(vOldHit.IsNull()) vOldHit = vRelIntersection;
    else
    {
        while(true)
        {
            // 
            const coreVector3 vDiff = vRelIntersection - vOldHit;
            const coreFloat   fLen  = vDiff.Length();

            // 
            if(fLen < fMin) break;

            // 
            const coreVector3 vNewHit      = LERP(vOldHit, vRelIntersection, fMin*RCP(fLen));
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

                // add rotation back to final position
                const coreVector3 vAbsPos = MapToAxis(vDecalPos, vEnvDirection);

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "default_white.png");
                pObject->DefineProgram("effect_decal_single_program");   // TODO 1: eigenen decal_color shader (similar to menu_color)

                // set object properties
                pObject->SetSize     (coreVector3(vDecalSize, 1.0f));
                pObject->SetDirection(coreVector3(vDecalDir,  0.0f));
                pObject->SetColor4   (coreVector4(0.0f,0.0f,0.0f,0.5f));

                // add object to background
                g_pEnvironment->GetBackground()->AddDecal(pObject, vAbsPos, 256u, "effect_decal_single_inst_program", LIST_KEY);
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