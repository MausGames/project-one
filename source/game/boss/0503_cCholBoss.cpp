///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 1: flügel sollten sehr dynamisch sein, wie ein vogel, gehen nach hinten bei sturzflug
// TODO 1: rotation von körper oder flügel signalisieren bestimmten angriff -> andere bosse auch
// TODO 1: show boss banner with each resurrection ?
// TODO 1: ram von boss und wings braucht flammen-wellen-effekt
// TODO 1: outline vom body is dünner als von flügeln (wegen shape)
// wave with heat areas und temperaturanzeige über spieler is hidden phase
// tod: gegner wird von seinen eigenen flügeln aufgespießt, tak-tak-tak-tak-boom
// 3 verschiedene death animations, die letzte is offensichtlicher
// intro: boss fliegt wie meteoriten auf den boden (bam-bam-bam-bam-BAM) (von selber richtung), und baut sich dann zusammen, glüht ab nachm einschlag (energy overlay), + trail
// death 1: fliegt in lava, kommt wieder raus
// feuer baut sich auf mit partikeln von vorne nach hinten, kann sich mit flügel-bewegung strecken


// ****************************************************************
// counter identifier
#define DEATH_COUNT (0u)


// ****************************************************************
// vector identifier
#define START_VALUES_POS (0u)   // # uses 0u - 4u
#define START_VALUES_DIR (5u)   // # uses 5u - 9u


// ****************************************************************
// constructor
cCholBoss::cCholBoss()noexcept
: m_aiWingState   {}
, m_afWingThrow   {}
, m_avWingReturn  {}
, m_fTilt         (0.0f)
, m_fFlap         (0.0f)
, m_fAnimation    (0.0f)
, m_fResurrection (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_chol_body_high.md3");
    this->DefineModelLow ("ship_boss_chol_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.0f);

    // configure the boss
    this->Configure(100, COLOR_SHIP_ORANGE);

    // 
    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
    {
        m_aWing[i].DefineModelHigh("ship_boss_chol_wing_high.md3");
        m_aWing[i].DefineModelLow ("ship_boss_chol_wing_low.md3");
        m_aWing[i].SetSize        (this->GetSize());
        m_aWing[i].Configure      (1, COLOR_SHIP_ORANGE);
        m_aWing[i].SetParent      (this);
    }

    // 
    m_Fire.DefineModel  ("object_boss_chol_fire.md3");
    m_Fire.DefineTexture(0u, "effect_energy.png");
    m_Fire.DefineProgram("effect_energy_flat_direct_program");
    m_Fire.SetSize      (this->GetSize());
    m_Fire.SetColor3    (COLOR_ENERGY_ORANGE);
    m_Fire.SetAlpha     (0.0f);
    m_Fire.SetTexSize   (coreVector2(1.0f,0.3f) * 1.5f);
    m_Fire.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cCholBoss::__ResurrectOwn()
{
    // 
    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
        this->__ChangeWingIntro(i);
}


// ****************************************************************
// 
void cCholBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->__DisableFire(bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cCholBoss::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_Fire.Render();
}


// ****************************************************************
// 
void cCholBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 100.0f);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_LINEAR)
        {
            const auto nImpactFunc = [](coreObject3D* OUTPUT pObject, const coreVector2 vPos, const coreVector3 vDir, const coreFloat fTime, const coreFloat fTimeBefore, const coreBool bBig)
            {
                const coreVector3 vPoint   = coreVector3(vPos, g_pEnvironment->RetrieveSafeHeight(vPos) + 8.0f);
                const coreVector3 vPosFrom = vPoint - vDir * 150.0f;
                const coreVector3 vPosTo   = vPoint;

                pObject->SetPosition (LERP(vPosFrom, vPosTo, LERPBR(0.0f, 1.0f, CLAMP(fTime, 0.0f, 1.0f))));
                pObject->SetDirection(vDir);

                if(InBetween(1.0f, fTimeBefore, fTime))
                {
                    if(bBig) g_pSpecialEffects->MacroExplosionColorBig  (vPosTo, COLOR_ENERGY_ORANGE);
                        else g_pSpecialEffects->MacroExplosionColorSmall(vPosTo, COLOR_ENERGY_ORANGE);
                }
            };

            nImpactFunc(&m_aWing[0], coreVector2(-8.0f,-1.0f) * 2.0f, coreVector3(-0.5f,-1.0f,-2.5f).Normalized(), (fTime - 0.0f)  * 6.0f, (fTimeBefore - 0.0f)  * 6.0f, false);
            nImpactFunc(&m_aWing[1], coreVector2(-2.0f,-5.0f) * 2.0f, coreVector3(-1.0f,-0.5f,-2.5f).Normalized(), (fTime - 0.05f) * 6.0f, (fTimeBefore - 0.05f) * 6.0f, false);
            nImpactFunc(&m_aWing[2], coreVector2( 6.0f, 2.0f) * 2.0f, coreVector3(-1.5f,-1.0f,-2.5f).Normalized(), (fTime - 0.1f)  * 6.0f, (fTimeBefore - 0.1f)  * 6.0f, false);
            nImpactFunc(&m_aWing[3], coreVector2( 0.0f, 7.0f) * 2.0f, coreVector3(-1.0f,-1.5f,-2.5f).Normalized(), (fTime - 0.15f) * 6.0f, (fTimeBefore - 0.15f) * 6.0f, false);
            nImpactFunc(this,        coreVector2( 0.0f, 1.0f) * 2.0f, coreVector3(-1.0f,-1.0f,-2.5f).Normalized(), (fTime - 0.4f)  * 6.0f, (fTimeBefore - 0.4f)  * 6.0f, true);

            if(PHASE_TIME_AFTER(0.85f))
            {
                PHASE_CHANGE_INC

                this->_StartBoss();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
                {
                    const coreVector3 vWingDir = m_aWing[i].GetDirection();

                    m_avVector[START_VALUES_POS + i].xyz(m_aWing[i].GetPosition());
                    m_avVector[START_VALUES_DIR + i].xyz(coreVector3(MapToAxisInv(vWingDir.xy(), this->GetDirection().xy().Normalized()).Angle(), vWingDir.z, SQRT(1.0f - POW2(vWingDir.z))));

                    this->__ChangeWing(i, 22u);
                }

                m_avVector[START_VALUES_POS + 4u].xyz(this->GetPosition());
                m_avVector[START_VALUES_DIR + 4u].xyz(coreVector3(this->GetDirection().xy().Angle(), this->GetDirection().z, SQRT(1.0f - POW2(this->GetDirection().z))));
            }

            const coreVector3 vStartPos = m_avVector[START_VALUES_POS + 4u].xyz();
            const coreVector3 vStartDir = m_avVector[START_VALUES_DIR + 4u].xyz();

            const coreVector3 vNewPos = LERPH3(vStartPos, coreVector3(0.0f, 0.6f * FOREGROUND_AREA.y, 0.0f), fTime);
            const coreVector3 vNewDir = coreVector3(coreVector2::Direction(LERPH3(vStartDir.x, 1.0f*PI, fTime)) * vStartDir.z, LERPH3(vStartDir.y, 0.0f, fTime)).Normalized();

            this->SetPosition (vNewPos);
            this->SetDirection(vNewDir);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->__EnableFire();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TICKER(0u, 4u, 1.0f, LERP_LINEAR)
        {
            coreUintW iIndex;
            switch(iTick)
            {
            default: ASSERT(false)
            case 0u: iIndex = 0u; break;
            case 1u: iIndex = 2u; break;
            case 2u: iIndex = 1u; break;
            case 3u: iIndex = 3u; break;
            }

            this->__ChangeWingThrow(iIndex);
        });
    }

    // ################################################################
    // ################################################################

    // 
    if(this->ReachedDeath())
    {
        this->__KillFake();
    }
    if(m_fResurrection)
    {
        m_fResurrection.Update(-1.0f);
        if(m_fResurrection <= 0.0f)
        {
            this->__ResurrectFake();
        }
    }




    // 
    const coreVector3 vBodyDir  = this->GetDirection();
    const coreMatrix3 mBodyRota = coreMatrix4::RotationAxis(m_fTilt, vBodyDir).m123();
    const coreVector3 vBodyOri  = coreVector3::Cross(vBodyDir, coreVector3::Cross(vBodyDir, coreVector3(0.0f,0.0f,1.0f)).Normalized()) * mBodyRota;
    const coreVector3 vBodyTan  = coreVector3::Cross(vBodyOri, vBodyDir);

    // 
    m_fFlap = 0.0f + 0.05f * SIN(m_fLifeTime * 3.0f);

    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
    {
        cLodObject& oWing = m_aWing[i];

        const coreFloat fSide = (i < 2u) ? 1.0f : -1.0f;

        const coreVector2 vBodyDirNorm = vBodyDir.xy().Normalized();

        const coreFloat fAngle1 = DEG_TO_RAD( 25.0f) * I_TO_F((i % 2u) + 1u) * fSide;
        const coreFloat fAngle2 = DEG_TO_RAD(-10.0f) * I_TO_F((i % 2u) + 1u) * fSide;
        const coreFloat fAngle3 = LERP(fAngle1, fAngle2, m_fFlap);

        const coreVector3 vDir1 = coreVector3(MapToAxis(coreVector2::Direction(fAngle1), vBodyDirNorm), 0.0f) * mBodyRota;
        const coreVector3 vDir2 = coreVector3(MapToAxis(coreVector2::Direction(fAngle2), vBodyDirNorm), 0.0f) * mBodyRota;
        const coreVector3 vDir3 = coreVector3(MapToAxis(coreVector2::Direction(fAngle3), vBodyDirNorm), 0.0f) * mBodyRota;

        const coreVector3 vOff1 = vBodyDir *  15.0f - vDir1 *  15.0f;
        const coreVector3 vOff2 = vBodyDir * -16.0f - vDir2 * -15.0f;

        const coreVector3 vPos1 = this->GetPosition() + 1.2f * (vOff1 + vBodyTan * fSide * ((i % 2u) ? 2.0f : 1.7f));
        const coreVector3 vPos2 = this->GetPosition() + 1.2f * (vOff2 + vBodyTan * fSide * ((i % 2u) ? 5.5f : 3.0f) + vDir2 * ((i % 2u) ? -6.0f : -1.0f));
        const coreVector3 vPos3 = LERP(vPos1, vPos2, m_fFlap);

        if(m_aiWingState[i])
        {
            m_afWingThrow[i].Update(1.0f);

            switch(m_aiWingState[i])
            {
            case 1u:
                {
                    const coreVector2 vAim    = (this->NearestPlayerDual((i < 2u) ? 0u : 1u)->GetPosition().xy() - oWing.GetPosition().xy()).Normalized();
                    const coreVector2 vNewDir = SmoothAim(oWing.GetDirection().xy(), vAim, 5.0f);
                    const coreVector2 vNewOri = coreVector2::Direction(LERPBR(0.0f*PI, 10.0f*PI, MIN(m_afWingThrow[i] * (1.0f/3.0f), 1.0f)));

                    oWing.SetDirection  (coreVector3(vNewDir, 0.0f));
                    oWing.SetOrientation(OriRoundDir(vNewOri, vNewDir));

                    if(m_afWingThrow[i] >= 3.0f)
                    {
                        this->__ChangeWing(i, 2u);

                        g_pSpecialEffects->CreateBlowColor(oWing.GetPosition(), -oWing.GetDirection(), SPECIAL_BLOW_SMALL, COLOR_ENERGY_WHITE);

                        m_aWing[i].AddStatus(ENEMY_STATUS_DAMAGING);
                    }
                }
                break;

            case 2u:
                {
                    const coreVector2 vNewPos = oWing.GetPosition().xy() + oWing.GetDirection().xy() * ((120.0f + 300.0f * m_afWingThrow[i]) * TIME);

                    oWing.SetPosition(coreVector3(vNewPos, 0.0f));

                    if(!g_pForeground->IsVisiblePoint(vNewPos, 1.05f))
                    {
                        this->__ChangeWing(i, 3u);

                        g_pSpecialEffects->CreateSplashColor(oWing.GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_WHITE);
                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                        m_aWing[i].RemoveStatus(ENEMY_STATUS_DAMAGING);
                    }
                }
                break;

            case 3u:
                {
                    // nothing
                }
                break;

            case 11u:
                {
                    const coreVector2 vNewPos = oWing.GetPosition().xy() + oWing.GetDirection().xy() * (-20.0f * (1.0f - m_afWingThrow[i]) * TIME);

                    oWing.SetPosition(coreVector3(vNewPos, 0.0f));

                    if(m_afWingThrow[i] >= 1.0f)
                    {
                        this->__ChangeWing(i, 12u);

                        m_avWingReturn[i] = coreVector3(vNewPos, MapToAxisInv(oWing.GetDirection().xy(), vBodyDirNorm).Angle() + (2.0f*PI) * fSide);
                    }
                }
                break;

            case 12u:
                {
                    const coreVector3 vNewPos = LERPH3(coreVector3(m_avWingReturn[i].xy(), 0.0f), vPos3, MIN(m_afWingThrow[i], 1.0f));
                    const coreVector3 vNewDir = coreVector3(MapToAxis(coreVector2::Direction(LERPH3(m_avWingReturn[i].z, fAngle3, MIN(m_afWingThrow[i], 1.0f))), vBodyDirNorm), 0.0f);   // # no rota

                    oWing.SetPosition (vNewPos);
                    oWing.SetDirection(vNewDir);

                    if(m_afWingThrow[i] >= 1.0f)
                    {
                        this->__ChangeWing(i, 0u);
                    }
                }
                break;

            case 21u:
                {
                    // nothing
                }
                break;

            case 22u:
                {
                    const coreFloat fTime = MIN(m_afWingThrow[i] * 0.5f, 1.0f);

                    const coreVector3 vStartPos = m_avVector[START_VALUES_POS + i].xyz() * coreMatrix4::RotationZ(fTime * 1.0f*PI).m123();
                    const coreVector3 vStartDir = m_avVector[START_VALUES_DIR + i].xyz();

                    const coreVector3 vNewPos = LERPH3(vStartPos, vPos3, fTime);
                    const coreVector3 vNewDir = coreVector3(MapToAxis(coreVector2::Direction(LERPH3(vStartDir.x, fAngle3, fTime)), vBodyDirNorm) * vStartDir.z, LERPH3(vStartDir.y, 0.0f, fTime)).Normalized();   // # no rota

                    oWing.SetPosition (vNewPos);
                    oWing.SetDirection(vNewDir);

                    if(m_afWingThrow[i] >= 2.0f)
                    {
                        this->__ChangeWing(i, 0u);
                    }
                }
                break;

            default:
                ASSERT(false)
                break;
            }
        }
        else
        {
            oWing.SetPosition   (vPos3);
            oWing.SetDirection  (vDir3);
            oWing.SetOrientation(vBodyOri);
        }
    }




    // 
    if(m_Fire.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_Fire.SetPosition   (this->GetPosition ());
        m_Fire.SetDirection  (this->GetDirection());
        m_Fire.SetOrientation(vBodyOri);
        m_Fire.SetAlpha      (MIN(m_Fire.GetAlpha() + 5.0f * TIME, 1.0f));
        m_Fire.SetTexOffset  (coreVector2(0.0f, m_fAnimation * 0.16f));
        m_Fire.Move();
    }
}


// ****************************************************************
// 
void cCholBoss::__EnableFire()
{
    WARN_IF(m_Fire.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Fire.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Fire);
    
    
    
    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
    {
        g_pSpecialEffects->CreateBlowColor(m_aWing[i].GetPosition() + m_aWing[i].GetDirection() * 5.0f, -m_aWing[i].GetDirection(), SPECIAL_BLOW_BIG, COLOR_ENERGY_ORANGE);
    }
    
}


// ****************************************************************
// 
void cCholBoss::__DisableFire(const coreBool bAnimated)
{
    if(!m_Fire.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Fire.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Fire);
}


// ****************************************************************
// 
void cCholBoss::__ResurrectFake()
{
    // 
    this->SetCurHealth(this->GetMaxHealth());
    this->RefreshColor(1.0f);

    // 
    for(coreUintW i = 0u; i < CHOL_WINGS; ++i)
    {
        m_aWing[i].RefreshColor(1.0f);
    }

    // 
    m_fResurrection = 0.0f;

    // 
    g_pGame->GetInterface()->ShowBoss(this);

}


// ****************************************************************
// 
void cCholBoss::__KillFake()
{
    // 
    const coreFloat  fTime  = g_pGame->GetTimeTable()->GetTimeSegmentSafe();
    const coreUint32 iBonus = cGame::CalcBonusTime(fTime);

    // 
    g_pGame->GetInterface()->ShowScore(iBonus, 1u, MEDAL_TYPE_BOSS);

    // 
    m_fResurrection = INTERFACE_BANNER_DURATION_SCORE + 1.0f;
    m_aiCounter[DEATH_COUNT] += 1u;

    // 
    g_pSpecialEffects->MacroDestructionDark(this);

    // 
    this->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
}