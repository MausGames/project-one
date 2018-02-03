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


// ****************************************************************
// vector identifier
#define STORED_POSITION (0u)


// ****************************************************************
// constructor
cNautilusBoss::cNautilusBoss()noexcept
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_nautilus_body_high.md3");
    this->DefineModelLow ("ship_boss_nautilus_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(2.0f,2.0f,2.0f));

    // configure the boss
    this->Configure(3000, COLOR_SHIP_PURPLE);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].DefineModelHigh("ship_boss_nautilus_claw_high.md3");
        m_aClaw[i].DefineModelLow ("ship_boss_nautilus_claw_low.md3");
        m_aClaw[i].SetSize        (coreVector3(2.0f,2.0f,2.0f));
        m_aClaw[i].Configure      (1, COLOR_SHIP_PURPLE);
    }
}


// ****************************************************************
// 
void cNautilusBoss::__ResurrectOwn()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
        m_aClaw[i].Resurrect();

    // 
    m_aiCounter[CONTAINER_ATTACHED] = 1;
}


// ****************************************************************
// 
void cNautilusBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
        m_aClaw[i].Kill(bAnimated);
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
            {
                g_pGame->GetCurMission()->ActivateBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 20.0f)
        {
            const coreVector2 vDir  = this->GetDirection().xy();
            const coreVector2 vPos  = this->GetPosition ().xy() - vDir * 5.0f;
            const coreVector2 vSide = vDir.Rotated90() * 5.0f;

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, this, vPos + vSide, -vDir)->MakeYellow()->MakeSmaller(1.1f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, this, vPos - vSide, -vDir)->MakeYellow()->MakeSmaller(1.1f);
        });

        if((this->GetCurHealthPct() <= 0.9f) && coreMath::IsNear(COS(m_fAnimation), 0.0f, 0.01f))
        {
            m_iPhase = 10u;

            this->__StorePosition();
        }
    }

    // ################################################################
    // 
    if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_LINEAR)
        {
            if(PHASE_TIME_BEFORE(0.5f)) this->DefaultMoveLerp(this->__LoadPosition(), coreVector2(0.0f,0.0f),  LERPS(0.0f, 1.0f, (fTime)      * 2.0f));
                                   else this->DefaultMoveLerp(coreVector2(0.0f,0.0f), coreVector2(0.0f,0.75f), LERPS(0.0f, 1.0f, (fTime-0.5f) * 2.0f));

            this->DefaultRotateLerp(0.0f*PI, 31.0f*PI, LERPS(0.0f, 1.0f, fTime));

            if(PHASE_TIME_POINT(0.5f))
            {
                m_aiCounter[CONTAINER_ATTACHED] = 0;
                pMission->SetContainerForce(coreVector2(550.0f,0.0f));
            }

            if(PHASE_FINISHED)
                m_iPhase = 20u;
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            const cPlayer*  pPlayer = g_pGame->FindPlayer(this->GetPosition().xy());
            const coreFloat fAim    = (pPlayer->GetPosition().x - this->GetPosition().x) * Core::System->GetTime() * fTime * 3.0f;

            this->SetPosition         (coreVector3(this->GetPosition().x + fAim, this->GetPosition().y, 0.0f));
            this->DefaultOrientateLerp(0.0f*PI, 20.0f*PI, LERPB(0.0f, 1.0f, 1.0f - fTime));

            PHASE_CONTROL_TICKER(1u, 0u, 2.0f + 2.0f * fTime)
            {
                const coreVector2 vDir = this->GetDirection().xy();
                const coreVector2 vPos = this->GetPosition ().xy() + vDir * 5.0f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.6f, this, vPos,                                         vDir)->MakeRed()->MakeSmaller(1.2f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.6f, this, vPos - vDir * 3.0f + coreVector2( 3.0f,0.0f), vDir)->MakeRed()->MakeSmaller(1.2f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.6f, this, vPos - vDir * 3.0f + coreVector2(-3.0f,0.0f), vDir)->MakeRed()->MakeSmaller(1.2f);
            });

            if(PHASE_FINISHED)
            {
                ++m_iPhase;
                PHASE_RESET(1u)

                this->__StorePosition();
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 2.5f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(this->__LoadPosition(), coreVector2(this->__LoadPosition().x, m_aiCounter[DASH_SIDE] ? 1.0f : -1.0f), fTime);

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                this->__StorePosition();

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 22u)
    {
        PHASE_CONTROL_TICKER(1u, 80u, 60.0f)
        {
            const coreVector2 vPos    = this->__LoadPosition() * FOREGROUND_AREA;
            const coreVector2 vDir    = coreVector2(0.0f, m_aiCounter[DASH_SIDE] ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(I_TO_F(iTick + 2u) * 0.04f * FOREGROUND_AREA.x, 0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos + vOffset, vDir)->MakeGreen()->MakeSmaller(1.1f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos - vOffset, vDir)->MakeGreen()->MakeSmaller(1.1f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos + vOffset, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos + vOffset, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
        });

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            if(PHASE_TIME_AFTER(0.2f))
            {
                const coreFloat fDelayTime = LERPS(0.0f, 1.0f, (fTime - 0.2f) / 0.8f);

                this->DefaultMoveLerp    (this->__LoadPosition(),                         coreVector2(this->__LoadPosition().x, m_aiCounter[DASH_SIDE] ? 0.75f : -0.75f), fDelayTime);
                this->DefaultMultiateLerp(m_aiCounter[DASH_SIDE] ? (2.0f*PI) : (1.0f*PI), m_aiCounter[DASH_SIDE] ? (3.0f*PI) : (2.0f*PI),                                 fDelayTime);
            }

            if(PHASE_FINISHED && !m_aTimer[1].GetStatus())
            {
                m_iPhase = 20u;
                PHASE_RESET(1u)

                m_aiCounter[DASH_SIDE] = 1 - m_aiCounter[DASH_SIDE];

                this->__StorePosition();
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
        this->SetDirection(coreVector3(fCurX - fPrevX, Core::System->GetTime(), 0.0f).Normalized());
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
        for(coreUintW i = 40u; i--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 4.5f));

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, vImpact,  vDir)->MakeBlue()->MakeSmaller(1.3f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, vImpact, -vDir)->MakeBlue()->MakeSmaller(1.3f);
        }

        // 
        g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), SPECIAL_SPLASH_BIG, COLOR_ENERGY_BLUE);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
    }

    // 
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.11f * SIN(4.0f * coreFloat(Core::System->GetTotalTime())), this->GetOrientation()).m123();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].SetPosition   (this->GetPosition   ());
        m_aClaw[i].SetDirection  (this->GetDirection  () * (i ? mRota : mRota.Transposed()));
        m_aClaw[i].SetOrientation(this->GetOrientation() * (i ? -1.0f : 1.0f));
    }
}


// ****************************************************************
// 
void cNautilusBoss::__StorePosition()
{
    m_avVector[STORED_POSITION].xy(this->GetPosition().xy() / FOREGROUND_AREA);
}


// ****************************************************************
// 
coreVector2 cNautilusBoss::__LoadPosition()const
{
    return m_avVector[STORED_POSITION].xy();
}

