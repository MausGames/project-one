//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// counter identifier
#define CONTAINER_ATTACHED (0u)
#define DASH_SIDE          (1u)
#define SPIN_SIDE          (2u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cNautilusBoss::cNautilusBoss()noexcept
: m_fClawAngle (0.0f)
, m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_nautilus_body_high.md3");
    this->DefineModelLow ("ship_boss_nautilus_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(2.0f,2.0f,2.0f));

    // configure the boss
    this->Configure(8000, COLOR_SHIP_PURPLE);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].DefineModelHigh("ship_boss_nautilus_claw_high.md3");
        m_aClaw[i].DefineModelLow ("ship_boss_nautilus_claw_low.md3");
        m_aClaw[i].SetSize        (coreVector3(2.0f,2.0f,2.0f));
        m_aClaw[i].Configure      (1, COLOR_SHIP_PURPLE);
        m_aClaw[i].SetParent      (this);
    }
}


// ****************************************************************
// 
void cNautilusBoss::__ResurrectOwn()
{
    // 
    s_cast<cNevoMission*>(g_pGame->GetCurMission())->EnableContainer(coreVector2(0.0f,0.0f));

    // 
    m_aiCounter[CONTAINER_ATTACHED] = 1;
}


// ****************************************************************
// 
void cNautilusBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cNautilusBoss::__MoveOwn()
{
    cNevoMission* pMission   = s_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fTime);

            if(PHASE_TIME_POINT(0.85f))
                this->_StartBoss();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 1u)
    {
        constexpr coreFloat fHealth = 0.1f;

        PHASE_CONTROL_TICKER(0u, 0u, 20.0f)
        {
          //  if((iTick / 5u) & 0x01u) return;

            const coreVector2 vDir  = this->GetDirection().xy();
            const coreVector2 vPos  = this->GetPosition ().xy() + vDir * 5.0f;
            const coreVector2 vSide = vDir.Rotated90() * 5.0f;

            if((iTick / 5u) & 0x01u) g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos + vSide, vDir)->MakeYellow()->ChangeSize(1.2f);
                                else g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos - vSide, vDir)->MakeYellow()->ChangeSize(1.2f);
        });

        PHASE_CONTROL_TICKER(1u, 0u, 1.5f)
        {
            const coreVector2 vPos = this->GetPosition().xy();

            const coreUint16 iType = 1u << (CLAMP(F_TO_UI((1.0f - this->GetCurHealthPct()) * (4.0f / fHealth)), 1u, 3u) - 1u);

            const coreUintW iInc = MIN(iTick, 12u);
            for(coreUintW i = 12u - iInc, ie = 12u + iInc; i < ie; ++i)
            {
                if(((i / iType) + (iTick & 0x01u)) & 0x01u) continue;

                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 5.0f + 120.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.4f, this, vPos, vDir)->MakeCyan()->ChangeSize(1.1f);
            }
        });

        if((this->GetCurHealthPct() <= (1.0f - fHealth)) && coreMath::IsNear(COS(m_fAnimation), 0.0f, 0.01f))
        {
            PHASE_CHANGE_TO(10u)

            if(this->GetPosition().x < 0.0f) m_aiCounter[SPIN_SIDE] = 1;
        }
    }

    // ################################################################
    // 
    if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_LINEAR)
        {
            const coreFloat fSpinSign = m_aiCounter[SPIN_SIDE] ? -1.0f : 1.0f;

            if(PHASE_TIME_BEFORE(0.5f)) this->DefaultMoveLerp(m_vLastPosition,        coreVector2(0.0f,0.0f),  LERPS(0.0f, 1.0f, (fTime)      * 2.0f));
                                   else this->DefaultMoveLerp(coreVector2(0.0f,0.0f), coreVector2(0.0f,0.75f), LERPS(0.0f, 1.0f, (fTime-0.5f) * 2.0f));

            this->DefaultRotateLerp(0.0f*PI, fSpinSign * 31.0f*PI, LERPS(0.0f, 1.0f, fTime));

            if(PHASE_TIME_POINT(0.5f))
            {
                m_aiCounter[CONTAINER_ATTACHED] = 0;
                pMission->SetContainerForce(coreVector2(fSpinSign * 550.0f, 0.0f));
            }

            PHASE_CONTROL_TICKER(1u, 0u, 15.0f)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(LERPS(0.0f*PI, fSpinSign * 15.0f*PI, fTime));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, this, vPos,  vDir)->MakeRed()->ChangeSize(1.1f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, this, vPos, -vDir)->MakeRed()->ChangeSize(1.1f);
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(20u)
                PHASE_RESET(1u)
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            const cPlayer*  pPlayer = this->NearestPlayer();
            const coreFloat fAim    = (pPlayer->GetPosition().x - this->GetPosition().x) * Core::System->GetTime() * fTime * 3.0f;

            this->SetPosition         (coreVector3(this->GetPosition().x + fAim, this->GetPosition().y, 0.0f));
            this->DefaultOrientateLerp(0.0f*PI, 20.0f*PI, LERPB(0.0f, 1.0f, 1.0f - fTime));

            PHASE_CONTROL_TICKER(1u, 0u, 1.8f + 2.0f * fTime)
            {
                const coreVector2 vDir = this->GetDirection().xy();
                const coreVector2 vPos = this->GetPosition ().xy() + vDir * 5.0f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos,                                         vDir)->MakeOrange()->ChangeSize(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos - vDir * 3.0f + coreVector2( 3.0f,0.0f), vDir)->MakeOrange()->ChangeSize(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos - vDir * 3.0f + coreVector2(-3.0f,0.0f), vDir)->MakeOrange()->ChangeSize(1.4f);
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
                PHASE_RESET(1u)
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 3.0f, LERP_BREAK)
        {
            this->DefaultMoveLerp(m_vLastPosition, m_vLastPosition + coreVector2(0.0f, m_aiCounter[DASH_SIDE] ? -0.1f : 0.1f), fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 22u)
    {
        PHASE_CONTROL_TIMER(0u, 2.5f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(m_vLastPosition.x, m_aiCounter[DASH_SIDE] ? 1.0f : -1.0f), fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 23u)
    {
        PHASE_CONTROL_TICKER(1u, 80u, 60.0f)
        {
            const coreVector2 vPos    = m_vLastPosition * FOREGROUND_AREA;
            const coreVector2 vDir    = coreVector2(0.0f, m_aiCounter[DASH_SIDE] ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(I_TO_F(iTick + 3u) * 0.04f * FOREGROUND_AREA.x, 0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos + vOffset, vDir)->MakeGreen()->ChangeSize(1.1f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos - vOffset, vDir)->MakeGreen()->ChangeSize(1.1f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos + vOffset, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos + vOffset, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
        });

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            if(PHASE_TIME_AFTER(0.2f))
            {
                const coreFloat fDelayTime = LERPS(0.0f, 1.0f, (fTime - 0.2f) / 0.8f);

                this->DefaultMoveLerp    (m_vLastPosition,                         coreVector2(m_vLastPosition.x, m_aiCounter[DASH_SIDE] ? 0.75f : -0.75f), fDelayTime);
                if(!m_aiCounter[DASH_SIDE]) this->DefaultMultiateLerp(m_aiCounter[DASH_SIDE] ? (2.0f*PI) : (1.0f*PI), m_aiCounter[DASH_SIDE] ? (3.0f*PI) : (2.0f*PI),                                 fDelayTime);
            }

            if(PHASE_FINISHED && !m_aTimer[1].GetStatus())
            {
                PHASE_CHANGE_TO(m_aiCounter[DASH_SIDE] ? 30u : 20u)
                PHASE_RESET(1u)

                m_aiCounter[DASH_SIDE] = 1 - m_aiCounter[DASH_SIDE];
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_LINEAR)
        {
            this->DefaultMoveLerp    (m_vLastPosition, (pContainer->GetPosition().xy() - NAUTILUS_ATTACH_DIST * this->GetDirection().xy()) / FOREGROUND_AREA, LERPB(0.0f, 1.0f, fTime));
            //this->DefaultRotateLerp(1.0f*PI,               0.0f*PI, LERPS(0.0f, 1.0f, fTime));

            m_fClawAngle = SIN(PI * fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(10u)

                m_aiCounter[CONTAINER_ATTACHED] = 1;
                m_aiCounter[SPIN_SIDE]          = 1 - m_aiCounter[SPIN_SIDE];
            }
        });
    }

    // ################################################################
    // ################################################################

    if((m_iPhase < 10u) && Core::System->GetTime())
    {
        // 
        m_fAnimation.UpdateMod(1.0f, 2.0f*PI);

        // 
        const coreFloat fPrevX = 0.6f * SIN(m_fAnimation - Core::System->GetTime());
        const coreFloat fCurX  = 0.6f * SIN(m_fAnimation);

        // 
        this->SetPosition (coreVector3(fCurX * FOREGROUND_AREA.x, this->GetPosition().yz()));
        this->SetDirection(coreVector3(fCurX - fPrevX, -Core::System->GetTime(), 0.0f).Normalized());
    }

    if(m_aiCounter[CONTAINER_ATTACHED])
    {
        // 
        pContainer->SetPosition (this->GetPosition () + NAUTILUS_ATTACH_DIST * this->GetDirection());
        pContainer->SetDirection(this->GetDirection());
    }

    // 
    const coreVector2& vImpact = pMission->GetContainerImpact();
    if(!vImpact.IsNull())
    {
        for(coreUintW i = 20u; i--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact,  vDir)->MakeBlue()->ChangeSize(1.3f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact, -vDir)->MakeBlue()->ChangeSize(1.3f);
        }

        // 
        g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), SPECIAL_SPLASH_BIG, COLOR_ENERGY_BLUE);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
    }

    // 
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.11f * m_fClawAngle, this->GetOrientation()).m123();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].SetPosition   (this->GetPosition   ());
        m_aClaw[i].SetDirection  (this->GetDirection  () * (i ? mRota : mRota.Transposed()));
        m_aClaw[i].SetOrientation(this->GetOrientation() * (i ? -1.0f : 1.0f));
    }
}

// wirft ball von obne auf spieler
// stürtzt sich auf spieler von oben nach unten, wenn ball erwischt wird wird er weggestoßen
// greift kurz ball und schleudert ihn mit einer halen drehung auf die längere seite
// beginnt seiten angriff eins (// greift kurz ball und schleudert ihn mit einer halen drehung nach oben), dann zwei, dann rotiert er komisch weil er veriwrrt ist
// schießt tinte
// schnapt sich ball wieder und beginnt phase 1

// let the boss hit the ball is fragment