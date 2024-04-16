///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// - [chain, fragment, alles]
// - geschoss-welle mit loch von jeder seite nacheinander
// das erste fragment bekommt man im intro (die mitte der tafel, das viereck), und saugt einen ein und zerstört das schiff, und erzeugt eine graue kopie (Eigengrau) -> erst dann bekommt man blauen schein und quad, auch health-icon anders
// es muss einen mini-boss im intro geben (container! nicht P1), der einem die standard steuerung trainiert (slow/fast, turn, scoring), der einem das fragment gibt, und man kann nicht sterben


// ****************************************************************
// counter identifier
#define SWING_SIDE (0u)


// ****************************************************************
// vector identifier
#define CIRCLE_DATA (0u)
#define LAST_TARGET (1u)

// ****************************************************************
// constructor
cIntroBoss::cIntroBoss()noexcept
{
    // load models
    this->DefineModelHigh("ship_boss_intro_sword_high.md3");
    this->DefineModelLow ("ship_boss_intro_sword_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.5f);

    // configure the boss
    this->Configure(3000, COLOR_SHIP_PURPLE);

    // 
    m_Shield.DefineModelHigh("ship_boss_intro_shield_high.md3");
    m_Shield.DefineModelLow ("ship_boss_intro_shield_low.md3");
    m_Shield.SetSize        (this->GetSize());
    m_Shield.Configure      (1, COLOR_SHIP_PURPLE);
    m_Shield.AddStatus      (ENEMY_STATUS_INVINCIBLE);
    m_Shield.SetParent      (this);
}


// ****************************************************************
// 
void cIntroBoss::__ResurrectOwn()
{
    g_pEnvironment->ChangeBackground(cCloudBackground::ID, ENVIRONMENT_MIX_CURTAIN, 0.0f, coreVector2(1.0f,0.0f));
    g_pEnvironment->SetTargetSpeedNow(4.0f);
}


// ****************************************************************
// 
void cIntroBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cIntroBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    if(this->ReachedDeath()) this->Kill(true);   

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->_StartBoss();
            }

            this->SetPosition(coreVector3(0.0f,0.6f,0.0f) * FOREGROUND_AREA3);

            if(PHASE_TIME_AFTER(0.85f))
                PHASE_CHANGE_TO(70u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_SMOOTH)
        {
            this->DefaultRotateLerp(0.0f*PI, 10.0f*PI, fTime);

            //if(PHASE_FINISHED)
            //    PHASE_CHANGE_INC
        });


        PHASE_CONTROL_TICKER(1u, 0u, 30.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos,  vDir)->MakeWhite()->ChangeSize(1.7f);
            //if(m_fPhaseTime > 3.0f)
                //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos, -vDir)->MakeWhite()->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(coreVector2(-1.0f,1.0f), coreVector2(-1.0f,-1.0f), fTime);
            this->DefaultRotateLerp(1.55f*PI, 1.45f*PI, fTime);   // TODO 1: abhängig von Y position

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(coreVector2(-1.0f,-1.0f), coreVector2(1.0f,-1.0f), fTime);
            this->DefaultRotateLerp(1.45f*PI, -3.45f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(coreVector2(1.0f,-1.0f), coreVector2(1.0f,3.8f), fTime);
            this->DefaultRotateLerp(0.55f*PI, 0.45f*PI, fTime);   // TODO 1: abhängig von Y position

            const coreVector3 vPos = this->GetPosition();
            this->SetPosition(coreVector3(vPos.x, FmodRange(vPos.y, -1.4f * FOREGROUND_AREA.y, 1.4f * FOREGROUND_AREA.y), vPos.z));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(coreVector2(1.0f,1.0f), coreVector2(0.0f,0.8f), fTime);
            this->DefaultRotateLerp(0.45f*PI, 5.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 34u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 35u)
    {
        PHASE_CONTROL_TIMER(0u, 1.2f, LERP_LINEAR)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,0.8f), coreVector2(0.0f,-0.9f), ParaLerp(0.0f, -0.1f, 1.0f, fTime));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                for(coreUintW i = 0u; i < 26u; ++i)
                {
                    const coreVector2 vPos = (vDir * -1.1f) * FOREGROUND_AREA;
                    const coreVector2 vTan = vDir.Rotated90() * (I_TO_F(i) * 0.06f + 0.3f) * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos + vTan, vDir)->MakeWhite()->ChangeSize(1.7f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos - vTan, vDir)->MakeWhite()->ChangeSize(1.7f);
                }

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 36u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 37u)
    {
        const coreFloat fSide     = SIN(m_fPhaseTime       * 0.25f * (2.0f*PI)) * BLENDH3(MIN1(m_fPhaseTime       * 0.2f));
        const coreFloat fSidePrev = SIN(m_fPhaseTimeBefore * 0.25f * (2.0f*PI)) * BLENDH3(MIN1(m_fPhaseTimeBefore * 0.2f));

        this->SetPosition(coreVector3(fSide * 1.1f, -0.9f, 0.0f) * FOREGROUND_AREA3);

        const coreInt32 iTick     = F_TO_SI((fSide     + 1.0f) * 10.0f);
        const coreInt32 iTickPrev = F_TO_SI((fSidePrev + 1.0f) * 10.0f);

        if(iTick != iTickPrev)
        {
            const coreVector2 vPos = coreVector2(fSidePrev, -1.1f) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2((I_TO_F(iTick % 4u) - 1.5f) * 0.3f, 1.0f).Normalized();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.3f, this, vPos, vDir)->MakeWhite()->ChangeSize(1.7f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_WHITE);
            //g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        coreVector4& vCircleData = m_avVector[CIRCLE_DATA];

        vCircleData.x = MIN1(vCircleData.x + 1.0f * TIME);
        vCircleData.y += vCircleData.x * TIME;

        const coreFloat   fLen = LERP(0.25f, 0.8f, 0.5f + 0.5f * SIN(vCircleData.y * 0.5f));
        const coreVector2 vDir = coreVector2::Direction(vCircleData.y * (1.0f*PI));
        const coreVector2 vPos = vDir * fLen;

        this->SetPosition (coreVector3(vPos, 0.0f) * FOREGROUND_AREA3);
        this->SetDirection(coreVector3(vDir, 0.0f));
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            const coreVector2 vBase = StepRotated90(m_aiCounter[SWING_SIDE] % 4u);

            const coreFloat fVal = COS(fTime * (2.0f*PI));

            const coreVector2 vDir = MapToAxis(coreVector2(fVal, -0.5f * (1.0f - ABS(fVal))).Normalized(), vBase);
            const coreVector2 vPos = vBase * 1.25f + vDir * 0.3f;

            this->SetPosition (coreVector3(vPos, 0.0f) * FOREGROUND_AREA3);
            this->SetDirection(coreVector3(vDir, 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)

                m_aiCounter[SWING_SIDE] += 1u;
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 30.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, vDir)->MakeWhite()->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            m_avVector[LAST_TARGET].xy(this->NearestPlayerDual(F_TO_UI(m_avVector[LAST_TARGET].z))->GetPosition().xy());
            
            const coreVector2 vDiff = m_avVector[LAST_TARGET].xy() - this->GetPosition().xy();
            
            if(PHASE_BEGINNING)
            {
                m_avVector[LAST_TARGET].z = 1.0f - m_avVector[LAST_TARGET].z;
                m_avVector[LAST_TARGET].w = this->GetDirection().xy().Angle();
            }

            m_avVector[LAST_TARGET].w = SmoothAimAngle(m_avVector[LAST_TARGET].w, vDiff.Angle(), LERP(0.0f, 5.0f, fTime));
            const coreVector2 vDir = coreVector2::Direction(m_avVector[LAST_TARGET].w + LERP(0.0f*PI, m_avVector[LAST_TARGET].z ? (-4.0f*PI) : (4.0f*PI), fTime));

            this->SetDirection(coreVector3(vDir, 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                PHASE_RESET(1u)

                m_avVector[LAST_TARGET].xy(this->GetPosition().xy() + vDir * vDiff.Length());
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            this->DefaultMoveLerp(m_vLastPosition, m_avVector[LAST_TARGET].xy() / FOREGROUND_AREA, ParaLerp(0.0f, -0.1f, 1.0f, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(60u)
        });

        PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
        {
            if(iTick < 5u) return;

            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.1f, this, vPos,  vDir.Rotated90())->MakeWhite()->ChangeSize(1.7f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.1f, this, vPos, -vDir.Rotated90())->MakeWhite()->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 0.08f, LERP_HERMITE3)
        {
            const coreFloat fOffset = SIN(fTime * (2.0f*PI)) * 0.5f;
            this->SetPosition(coreVector3(fOffset, LERP(m_vLastPosition.y, 0.0f, fTime), 0.0f) * FOREGROUND_AREA3);
            
            this->DefaultRotateLerp(1.0f*PI, 8.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(20u)
        });


        PHASE_CONTROL_TICKER(1u, 0u, 1.5f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < 15u; ++i)
            {
                const coreVector2 vPos = this->GetPosition ().xy() + this->GetDirection().xy() * (I_TO_F(i) * 2.0f);
                const coreVector2 vDir = this->GetDirection().xy().Rotated90();
    
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos,  vDir)->MakeWhite()->ChangeSize(1.7f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos, -vDir)->MakeWhite()->ChangeSize(1.7f);
            }
        });
    }

    // ################################################################
    // ################################################################
    
    
    
    if((m_iPhase >= 30u) && (m_iPhase < 34u))
    {
        PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, vDir)->MakeWhite()->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos, -vDir)->MakeWhite()->ChangeSize(1.7f);
        });
    }

    
    
    
    // 
    m_Shield.SetPosition (coreVector3(10.0f,0.0f,0.0f) * FOREGROUND_AREA3);
    m_Shield.SetDirection(coreVector3( 0.0f,1.0f,0.0f));
}