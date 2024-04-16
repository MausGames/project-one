///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// teaches: chain, fragment, boss structure, everything
// blaue und orangene geschosse dürfen nicht unten starten (brei dreh-angriff), weil man sonst nicht ausweichen kann
// bei rundum-schwung nach innen schießen war komplett fürn orsch
// spinning rotation on target attack needs to alternate, because rotation speed depends on player movement and so the difference can be hidden a bit
// general rotation between phases needs to alternate
// bei rundum-schwung, boss sollte nicht mit schwung nach außen fliegen, sonst erwischt es den überraschten spieler, erst schwung bremsen
// TODO 1: hard: shield, verfolgt spieler (vielleicht front), und wehrt angriffe ab
// TODO 1: vielleicht radiert das schwert einmal eine runde (oder 3/4) am rand bevor es sich rauszieht (transition), lebenspunkte müssen angepasst werden
// TODO 1: ansonsten normales rausziehen noch implementieren
// TODO 1: schwert sollte leicht kippen beim radieren
// TODO 1: das erste fragment bekommt man im intro (die mitte der tafel, das viereck), und saugt einen ein und zerstört das schiff, und erzeugt eine graue kopie (Eigengrau) -> erst dann bekommt man blauen schein und quad, auch health-icon anders
// TODO 1: improve intro and outro (including fragment collection)
// TODO 1: es sollten noch paar manuelle orientation-flips eingebaut werden (e.g. vorm runter-chargen in die wand)
// TODO 1: MAIN: fragment, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), intro, outro, foreshadow, overdrive, sound, attack size/count/speed, enemy/boss size, object size, background rota/speed


// ****************************************************************
// counter identifier
#define SWING_SIDE (0u)


// ****************************************************************
// vector identifier
#define CIRCLE_DATA (0u)
#define BURST_DATA  (1u)
#define LAST_TARGET (2u)


// ****************************************************************
// constructor
cIntroBoss::cIntroBoss()noexcept
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.5f);

    // configure the boss
    this->Configure(4400, 0u, COLOR_SHIP_PURPLE);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    m_Blade.DefineModelHigh("ship_boss_intro_sword_blade_high.md3");
    m_Blade.DefineModelLow ("ship_boss_intro_sword_blade_low.md3");
    m_Blade.SetSize        (this->GetSize());
    m_Blade.Configure      (1, 0u, COLOR_SHIP_PURPLE);
    m_Blade.AddStatus      (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_SECRET);
    m_Blade.SetParent      (this);

    // 
    m_Hilt.DefineModelHigh("ship_boss_intro_sword_hilt_high.md3");
    m_Hilt.DefineModelLow ("ship_boss_intro_sword_hilt_low.md3");
    m_Hilt.SetSize        (this->GetSize());
    m_Hilt.Configure      (1, 0u, COLOR_SHIP_PURPLE);
    m_Hilt.SetParent      (this);

    // 
    m_Shield.DefineModelHigh("ship_boss_intro_shield_high.md3");
    m_Shield.DefineModelLow ("ship_boss_intro_shield_low.md3");
    m_Shield.SetSize        (this->GetSize());
    m_Shield.Configure      (1, 0u, COLOR_SHIP_PURPLE);
    m_Shield.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_SECRET);
    m_Shield.SetParent      (this);
}


// ****************************************************************
// 
void cIntroBoss::__ResurrectOwn()
{
    // 
    this->_ResurrectBoss();
}


// ****************************************************************
// 
void cIntroBoss::__KillOwn(const coreBool bAnimated)
{
}


// ****************************************************************
// 
void cIntroBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    // 
    if(this->ReachedDeath())      
    {
        PHASE_CHANGE_TO(80u)
        
        // TODO 1: give last chain score
        // TODO 1: stop timer
        
        m_Blade.AddStatus(ENEMY_STATUS_GHOST);
        m_Hilt .AddStatus(ENEMY_STATUS_GHOST);
        
        g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
        
        g_pSpecialEffects->MacroExplosionDarkBig(this->GetPosition());
    }

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_BREAK)
        {
            if(PHASE_TIME_POINT(0.5f))
            {
                this->_StartBoss();
            }

            this->DefaultMoveLerp  (coreVector2(0.0f,1.4f), coreVector2(0.0f,0.6f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                3.0f*PI,                fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(70u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(0.0f,0.0f), fTime);
            this->DefaultRotateLerp(-1.0f*PI,        -12.0f*PI,              fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(40u)
        });

        PHASE_CONTROL_TICKER(1u, 0u, 30.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.8f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(-1.8f,0.9f), coreVector2(-1.0f,0.9f), fTime);
            this->SetDirection   (coreVector3(1.0f,0.0f,0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.85f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(coreVector2(-1.0f,0.9f), coreVector2(-1.0f,-1.0f), fTime);
            this->SetDirection   (coreVector3(1.0f,0.0f,0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp  (coreVector2(-1.0f,-1.0f), coreVector2(1.0f,-1.0f), fTime);
            this->DefaultRotateLerp(1.5f*PI,                  -3.5f*PI,                fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(coreVector2(1.0f,-1.0f), coreVector2(1.0f,3.8f), fTime);
            this->SetDirection   (coreVector3(-1.0f,0.0f,0.0f));

            const coreVector3 vPos = this->GetPosition();

            this->SetPosition(coreVector3(vPos.x, FmodRange(vPos.y, -1.4f * FOREGROUND_AREA.y, 1.4f * FOREGROUND_AREA.y), vPos.z));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 34u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp  (coreVector2(1.0f,1.0f), coreVector2(0.0f,0.8f), fTime);
            this->DefaultRotateLerp(0.5f*PI,                5.0f*PI,                fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 35u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.95f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 36u)
    {
        PHASE_CONTROL_TIMER(0u, 1.2f, LERP_LINEAR)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,0.8f), coreVector2(0.0f,-0.9f), ParaLerp(0.0f, -0.1f, 1.0f, fTime));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                for(coreUintW i = 0u; i < 30u; ++i)
                {
                    const coreVector2 vPos = (vDir * -1.1f) * FOREGROUND_AREA;
                    const coreVector2 vTan = vDir.Rotated90() * (I_TO_F(i) * 0.06f + 0.11f) * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, this, vPos + vTan, vDir)->ChangeSize(1.7f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, this, vPos - vTan, vDir)->ChangeSize(1.7f);
                }

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 37u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 38u)
    {
        const coreFloat fSide     = SIN(m_fPhaseTime       * (0.5f*PI)) * BLENDH3(MIN1(m_fPhaseTime       * 0.2f));
        const coreFloat fSidePrev = SIN(m_fPhaseTimeBefore * (0.5f*PI)) * BLENDH3(MIN1(m_fPhaseTimeBefore * 0.2f));

        this->SetPosition(coreVector3(fSide * 1.12f, -0.9f, 0.0f) * FOREGROUND_AREA3);

        const coreInt32 iTick     = F_TO_SI((fSide     + 1.0f) * 10.0f);
        const coreInt32 iTickPrev = F_TO_SI((fSidePrev + 1.0f) * 10.0f);

        if(iTick != iTickPrev)
        {
            const coreVector2 vPos = coreVector2(fSide, -1.1f) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2((I_TO_F(iTick % 4u) - 1.5f) * 0.3f, 1.0f).Normalized();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.3f, this, vPos, vDir)->ChangeSize(1.7f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_YELLOW);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY * 0.5f);
        }

        if(this->GetCurHealth() < 600)
        {
            PHASE_CHANGE_TO(60u)
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        coreVector4& vCircleData = m_avVector[CIRCLE_DATA];

        if(this->GetCurHealth() < 2100)
        {
            vCircleData.x = MAX0(vCircleData.x - 1.0f * TIME);
            vCircleData.z += 1.0f * TIME;
        }
        else
        {
            vCircleData.x = MIN1(vCircleData.x + 1.0f * TIME);
        }

        vCircleData.y += vCircleData.x * 0.95f * TIME;

        const coreFloat   fLen = LERP(0.0f, 0.8f, 0.5f + 0.5f * SIN(vCircleData.y * 0.4f - (0.5f*PI))) + LERPBR(0.0f, 1.4f * SQRT2, MAX0((vCircleData.z - 1.0f) * 1.0f));
        const coreVector2 vDir = coreVector2::Direction(vCircleData.y * (1.0f*PI));
        const coreVector2 vPos = vDir * fLen * FOREGROUND_AREA;

        this->SetPosition (coreVector3(vPos, 0.0f));
        this->SetDirection(coreVector3(vDir, 0.0f));

        if(!vCircleData.z)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 30.0f, LERP_LINEAR)
            {
                if(iTick % 4u < 2u) return;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.7f);
            });
        }

        if(vCircleData.z >= 2.0f)
            PHASE_CHANGE_TO(50u)
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            const coreFloat   fValue = COS(fTime * (2.0f*PI));
            const coreVector2 vBase  = StepRotated90(m_aiCounter[SWING_SIDE] % 4);
            const coreVector2 vDir   = MapToAxis(coreVector2(fValue, LERP(-0.5f, 0.0f, ABS(fValue))).Normalized(), vBase);
            const coreVector2 vPos   = vBase * 1.25f + vDir * 0.3f;

            this->SetPosition (coreVector3(vPos, 0.0f) * FOREGROUND_AREA3);
            this->SetDirection(coreVector3(vDir, 0.0f));

            if(PHASE_FINISHED)
            {
                if(this->GetCurHealth() < 1500)
                {
                    PHASE_CHANGE_TO(30u)
                }
                else
                {
                    PHASE_RESET(0u)

                    m_aiCounter[SWING_SIDE] += 1;
                }
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 30.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            coreVector4& vLastTarget = m_avVector[LAST_TARGET];

            const coreBool    bSide   = (vLastTarget.z != 0.0f);
            const coreVector2 vTarget = this->NearestPlayerDual(bSide ? 1u : 0u)->GetPosition().xy();
            const coreVector2 vDiff   = vTarget - this->GetPosition().xy();

            if(PHASE_BEGINNING)
            {
                vLastTarget.w = this->GetDirection().xy().Angle();
            }

            vLastTarget.w = SmoothAimAngle(vLastTarget.w, vDiff.Angle(), fTime * 5.0f);

            const coreVector2 vDir = coreVector2::Direction(vLastTarget.w + LERP(0.0f*PI, (4.0f*PI) * (bSide ? -1.0f : 1.0f), fTime));

            this->SetDirection(coreVector3(vDir, 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
                PHASE_RESET(1u)

                vLastTarget.xy((this->GetPosition().xy() + vDir * vDiff.Length()) / FOREGROUND_AREA);
                vLastTarget.z = 1.0f - vLastTarget.z;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            this->DefaultMoveLerp(m_vLastPosition, m_avVector[LAST_TARGET].xy(), ParaLerp(0.0f, -0.1f, 1.0f, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(60u)
        });

        PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
        {
            if(iTick < 5u) return;

            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.1f, this, vPos,  vDir.Rotated90())->ChangeSize(1.7f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.1f, this, vPos, -vDir.Rotated90())->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 0.08f, LERP_HERMITE3)
        {
            const coreFloat fOffset = SIN(fTime * (2.0f*PI)) * 0.5f;

            this->SetPosition      (coreVector3(fOffset, LERP(m_vLastPosition.y, 0.1f, fTime), 0.0f) * FOREGROUND_AREA3);
            this->DefaultRotateLerp(1.0f*PI, 9.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(20u)
        });

        PHASE_CONTROL_TICKER(1u, 0u, 1.5f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < 15u; ++i)
            {
                const coreVector2 vPos = this->GetPosition ().xy() + this->GetDirection().xy() * (I_TO_F(i) * 2.0f);
                const coreVector2 vDir = this->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.6f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.6f);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_BREAK)
        {
            const coreFloat fLastAngle = m_vLastDirection.xy().Angle();

            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(0.0f,0.3f), fTime);
            this->DefaultRotateLerp(fLastAngle,      fLastAngle - 2.0f*PI,   fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pGame->GetItemManager()->AddItem<cFragmentItem>(this->GetPosition().xy());

                g_pSpecialEffects->MacroExplosionDarkBig(this->GetPosition());
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        PHASE_CONTROL_TIMER(0u, 0.35f, LERP_BREAK)
        {
            m_avVector[BURST_DATA].x = fTime;

            if(PHASE_FINISHED)
            {
                this->Kill(true);
                this->_EndBoss();
            }
        });
    }

    // ################################################################
    // ################################################################

    // 
    if((m_iPhase >= 30u) && (m_iPhase <= 34u))
    {
        PHASE_CONTROL_TICKER(1u, 0u, 12.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.55f);
        });
    }

    // 
    const coreVector2 vDir    = this->GetDirection().xy();
    const coreVector2 vOffset = vDir * (m_avVector[BURST_DATA].x * FOREGROUND_AREA.x * 2.4f * SQRT2);
    const coreVector2 vRota   = MapToAxis(vDir, coreVector2::Direction(m_avVector[BURST_DATA].x * (7.0f*PI)));

    // 
    m_Blade.SetPosition   (coreVector3(this->GetPosition().xy() + vOffset - 14.0f * (vRota - vDir), 0.0f));
    m_Blade.SetDirection  (coreVector3(vRota, 0.0f));
    m_Blade.SetOrientation(this->GetOrientation());

    // 
    m_Hilt.SetPosition   (coreVector3(this->GetPosition().xy() - vOffset + 3.0f * (vRota - vDir), 0.0f));
    m_Hilt.SetDirection  (coreVector3(vRota, 0.0f));
    m_Hilt.SetOrientation(this->GetOrientation());

    // 
    m_Shield.SetPosition (coreVector3(HIDDEN_POS, 0.0f));
    m_Shield.SetDirection(coreVector3(0.0f, 1.0f, 0.0f));
}