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
// ACHIEVEMENT: keep the extra enemy alive, until the boss gets rid of it
// TODO 1: hard: shield, verfolgt spieler (vielleicht front), und wehrt angriffe ab
// TODO 5: das erste fragment bekommt man im intro (die mitte der tafel, das viereck), und saugt einen ein und zerstört das schiff, und erzeugt eine graue kopie (Eigengrau) -> erst dann bekommt man blauen schein und quad, auch health-icon anders
// TODO 1: [MF] [HIGH] sounds verbessern


// ****************************************************************
// counter identifier
#define SWING_SIDE (0u)
#define RUSH_COUNT (1u)
#define ALT_HELPER (2u)


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
    this->Configure(4400, COLOR_SHIP_PURPLE);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    PHASE_HEALTH_GOAL({4400, 2100, 1500, 600, 0})

    // 
    m_Blade.DefineModelHigh("ship_boss_intro_sword_blade_high.md3");
    m_Blade.DefineModelLow ("ship_boss_intro_sword_blade_low.md3");
    m_Blade.DefineVolume   ("ship_boss_intro_sword_blade_volume.md3");
    m_Blade.SetSize        (this->GetSize());
    m_Blade.Configure      (1, COLOR_SHIP_PURPLE);
    m_Blade.AddStatus      (ENEMY_STATUS_BOTTOM | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_SECRET);
    m_Blade.SetParent      (this);

    // 
    m_Hilt.DefineModelHigh("ship_boss_intro_sword_hilt_high.md3");
    m_Hilt.DefineModelLow ("ship_boss_intro_sword_hilt_low.md3");
    m_Hilt.DefineVolume   ("ship_boss_intro_sword_hilt_volume.md3");
    m_Hilt.SetSize        (this->GetSize());
    m_Hilt.Configure      (1, COLOR_SHIP_PURPLE);
    m_Hilt.AddStatus      (ENEMY_STATUS_BOTTOM);
    m_Hilt.SetParent      (this);

    // 
    //m_Shield.DefineModelHigh("ship_boss_intro_shield_high.md3");
    //m_Shield.DefineModelLow ("ship_boss_intro_shield_low.md3");
    //m_Shield.SetSize        (this->GetSize());
    //m_Shield.Configure      (1, COLOR_SHIP_PURPLE);
    //m_Shield.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_SECRET);
    //m_Shield.SetParent      (this);

    // 
    m_Dummy.DefineModelHigh("ship_enemy_warrior_high.md3");
    m_Dummy.DefineModelLow ("ship_enemy_warrior_low.md3");
    m_Dummy.DefineVolume   ("ship_enemy_warrior_volume.md3");
    m_Dummy.AddStatus      (ENEMY_STATUS_BOTTOM | ENEMY_STATUS_GHOST);
}


// ****************************************************************
// destructor
cIntroBoss::~cIntroBoss()
{
    // 
    this->Kill(false);
}


// ****************************************************************
// 
void cIntroBoss::ResurrectIntro()
{
    // 
    m_Blade.AddStatus(ENEMY_STATUS_GHOST);
    m_Hilt .AddStatus(ENEMY_STATUS_GHOST);

    // 
    m_bForeshadow = true;

    // 
    m_iPhase = 200u;
    this->Resurrect();
}


// ****************************************************************
// 
void cIntroBoss::__ResurrectOwn()
{
    if(m_iPhase < 200u)
    {
        // 
        m_Blade.RemoveStatus(ENEMY_STATUS_GHOST);
        m_Hilt .RemoveStatus(ENEMY_STATUS_GHOST);

        // 
        this->_ResurrectBoss();
    }
}


// ****************************************************************
// 
void cIntroBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    m_Dummy.Kill(bAnimated);

    // 
    m_iPhase = 0u;
}


// ****************************************************************
// 
void cIntroBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    // 
    cEnemy* pSkewer = g_pGame->GetCurMission()->GetEnemySquad(0u)->GetEnemy(0u);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH_REV)
        {
            if(PHASE_BEGINNING)
            {
                pSkewer->Resurrect();

                m_Dummy.SetSize  (pSkewer->GetSize());
                m_Dummy.Configure(1, pSkewer->GetBaseColor());
                m_Dummy.Resurrect();
            }

            const coreFloat fHeight = LERP(-2.2f, 2.2f, fTime) * FOREGROUND_AREA.y;

            this->SetPosition (coreVector3(0.0f, fHeight, -20.0f));
            this->SetDirection(coreVector3(0.0f,1.0f,0.0f));

            this->DefaultOrientate(fTime * (4.0f*PI));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_Dummy.Kill(false);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
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
    else if(m_iPhase == 2u)
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

            if(PHASE_TIME_POINT(0.1f) || PHASE_TIME_POINT(0.3f) || PHASE_TIME_POINT(0.5f) || PHASE_TIME_POINT(0.7f) || PHASE_TIME_POINT(0.9f))
            {
                //g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_WOOSH_01);
            }

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__ResurrectHelperIntro(ELEMENT_BLUE, false);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(40u)
        });

        PHASE_CONTROL_TICKER(1u, 0u, g_pGame->IsEasy() ? 20.0f : 30.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            const coreFloat fSpeed = 1.1f * (1.0f - 0.1f * I_TO_F(iTick % 3u));

            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.9f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_BLUE);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,1.0f), 10.0f);
            }

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
            if(PHASE_TIME_POINT(0.2f))
            {
                //g_pSpecialEffects->PlaySound(coreVector3(-1.0f,0.0f,0.0f) * FOREGROUND_AREA3, 1.0f, 0.6f, SOUND_EFFECT_WOOSH_01);
            }

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

            if(PHASE_TIME_POINT(0.3f) || PHASE_TIME_POINT(0.7f))
            {
                //g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_WOOSH_01);
            }

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
            if(PHASE_TIME_POINT(0.1f) || PHASE_TIME_POINT(0.6f))
            {
                //g_pSpecialEffects->PlaySound(coreVector3(1.0f,0.0f,0.0f) * FOREGROUND_AREA3, 1.0f, 0.6f, SOUND_EFFECT_WOOSH_01);
            }

            this->DefaultMoveLerp(coreVector2(1.0f,-1.0f), coreVector2(1.0f,3.8f), fTime);
            this->SetDirection   (coreVector3(-1.0f,0.0f,0.0f));

            const coreVector3 vPos = this->GetPosition();

            this->SetPosition(coreVector3(vPos.x, FmodRange(vPos.y, -1.4f * FOREGROUND_AREA.y, 1.4f * FOREGROUND_AREA.y), vPos.z));

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__ResurrectHelperIntro(ELEMENT_CYAN, false);
            }

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

            if(PHASE_TIME_POINT(0.3f) || PHASE_TIME_POINT(0.7f))
            {
                //g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_WOOSH_01);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 35u)
    {
        PHASE_CONTROL_TIMER(0u, 0.95f, LERP_SMOOTH)
        {
            this->DefaultOrientate(fTime * (2.0f*PI));

            if(PHASE_FINISHED)
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

            if(PHASE_TIME_POINT(0.6f))
            {
                //g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.6f, SOUND_EFFECT_WOOSH_01);
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                for(coreUintW i = 0u; i < 30u; ++i)
                {
                    const coreVector2 vPos = (vDir * ((i % 2u) ? -1.1f : -1.12f)) * FOREGROUND_AREA;
                    const coreVector2 vTan = vDir.Rotated90() * (I_TO_F(i) * 0.06f + (g_pGame->IsEasy() ? 0.22f : 0.11f)) * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, this, vPos + vTan, vDir)->ChangeSize(1.7f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, this, vPos - vTan, vDir)->ChangeSize(1.7f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos + vTan, 0.0f), 25.0f, 2u, COLOR_ENERGY_PURPLE);
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos - vTan, 0.0f), 25.0f, 2u, COLOR_ENERGY_PURPLE);
                }

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);

                if(this->__ResurrectHelperIntro(ELEMENT_PURPLE, true))
                {
                    g_pGame->GetHelper(ELEMENT_PURPLE)->SetPosition(coreVector3(0.8f,-1.1f,0.0f) * FOREGROUND_AREA3);
                }
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
        const coreFloat fSide     = SIN(m_fPhaseTime       * (0.5f*PI)) * (0.7f + 0.3f * SIN(MIN(m_fPhaseTime,       15.0f) * (0.1f*PI))) * BLENDH3(MIN1(m_fPhaseTime       * 0.2f));
        const coreFloat fSidePrev = SIN(m_fPhaseTimeBefore * (0.5f*PI)) * (0.7f + 0.3f * SIN(MIN(m_fPhaseTimeBefore, 15.0f) * (0.1f*PI))) * BLENDH3(MIN1(m_fPhaseTimeBefore * 0.2f));

        const coreVector3 vTilt = coreVector3(TIME ? coreVector2(fSidePrev - fSide, -4.0f * TIME).Normalized() : coreVector2(0.0f,-1.0f), 0.0f);
        const coreVector3 vPos2 = coreVector3(fSide * 1.12f, -1.1f, 0.0f) + vTilt * -0.2f;

        this->SetPosition (vPos2 * FOREGROUND_AREA3);
        this->SetDirection(vTilt);

        const coreFloat fFactor   = g_pGame->IsEasy() ? 6.0f : 10.0f;
        const coreInt32 iTick     = F_TO_SI((fSide     * 0.99f + 1.0f) * fFactor);
        const coreInt32 iTickPrev = F_TO_SI((fSidePrev * 0.99f + 1.0f) * fFactor);

        if(iTick != iTickPrev)
        {
            const coreVector2 vPos = coreVector2(fSide, -1.1f) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2((I_TO_F(iTick % 4u) - 1.5f) * 0.3f, 1.0f).Normalized();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.3f, this, vPos, vDir)->ChangeSize(1.7f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_YELLOW);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY * 0.5f);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_EFFECT_DUST);
        }

        if(this->GetCurHealth() <= 600)
        {
            PHASE_CHANGE_TO(60u)

            g_pSpecialEffects->MacroExplosionDarkSmall(coreVector3(coreVector2(fSide, -1.1f) * FOREGROUND_AREA, 0.0f));
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

            this->__ResurrectHelperIntro(ELEMENT_YELLOW, false);

            g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(2u));
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        coreVector4& vCircleData = m_avVector[CIRCLE_DATA];

        if(this->GetCurHealth() <= 2100)
        {
            vCircleData.x = MAX0(vCircleData.x - 1.0f * TIME);
            vCircleData.z += 1.0f * TIME;

            this->DefaultOrientate(BLENDBR(CLAMP01(vCircleData.z - 1.0f)) * (4.0f*PI));
        }
        else
        {
            vCircleData.x = MIN1(vCircleData.x + 1.0f * TIME);
        }

        vCircleData.y += vCircleData.x * 0.8f * TIME;

        const coreVector2 vOldPos = this->GetPosition().xy();

        const coreFloat   fLen = LERP(0.0f, 0.8f, 0.5f + 0.5f * SIN(vCircleData.y * (0.125f*PI) - (0.5f*PI))) + LERPBR(0.0f, 1.4f * SQRT2, CLAMP01(vCircleData.z - 1.0f));
        const coreVector2 vDir = coreVector2::Direction(vCircleData.y * (1.0f*PI));
        const coreVector2 vPos = vDir * fLen * FOREGROUND_AREA;

        this->SetPosition (coreVector3(vPos, 0.0f));
        this->SetDirection(coreVector3(vDir, 0.0f));

        if(InBetweenExt(0.0f, vOldPos.x, vPos.x) == 1)
        {
            //g_pSpecialEffects->PlaySound(coreVector3(vOldPos, 0.0f), 1.0f, 0.6f, SOUND_EFFECT_WOOSH_01);
        }

        if(PHASE_MAINTIME_POINT(2.0f))
        {
            this->__ResurrectHelperIntro(ELEMENT_ORANGE, false);
        }

        if(!vCircleData.z)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 40.0f, LERP_LINEAR)
            {
                if((iTick < 16u) || (iTick % 8u < (g_pGame->IsEasy() ? 6u : 4u))) return;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.1f, this, vPos, vDir.InvertedY())->ChangeSize(1.7f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_ORANGE);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            });
        }

        if(vCircleData.z >= 2.0f)
        {
            PHASE_CHANGE_TO(50u)

            if(!pSkewer->HasStatus(ENEMY_STATUS_DEAD))
            {
                pSkewer->Kill(false);
                g_pGame->GetCurMission()->GiveBadge(3u, BADGE_ACHIEVEMENT, pSkewer->GetPosition());
            }

            g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(0u));
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            if(PHASE_TIME_POINT(0.15f) || PHASE_TIME_POINT(0.65f))
            {
                //g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_WOOSH_01);
            }

            const coreFloat   fValue = COS(fTime * (2.0f*PI));
            const coreVector2 vBase  = StepRotated90(m_aiCounter[SWING_SIDE] % 4);
            const coreVector2 vDir   = MapToAxis(coreVector2(fValue, LERP(-0.5f, 0.0f, ABS(fValue))).Normalized(), vBase);
            const coreVector2 vPos   = vBase * (1.25f - 0.15f * BLENDB(MIN(fTime, 1.0f - fTime, 0.2f) * 5.0f)) + vDir * 0.19f;

            this->SetPosition (coreVector3(vPos, 0.0f) * FOREGROUND_AREA3);
            this->SetDirection(coreVector3(vDir, 0.0f));

            if(PHASE_FINISHED)
            {
                if(this->GetCurHealth() <= 1500)
                {
                    PHASE_CHANGE_TO(30u)

                    g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(1u));
                }
                else
                {
                    PHASE_AGAIN(0u)

                    m_aiCounter[SWING_SIDE] += 1;

                    if(m_aiCounter[SWING_SIDE] == 2u)
                    {
                        this->__ResurrectHelperIntro(ELEMENT_GREEN, false);
                    }
                }
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, g_pGame->IsEasy() ? 20.0f : 30.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.7f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_GREEN);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        if(this->ReachedHealth(1500))
        {
            g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_GREEN);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->DefaultMoveLerp (m_vLastPosition, m_vLastPosition + m_vLastDirection.xy() * -0.6f, fTime);
            this->DefaultOrientate(fTime * (3.0f*PI));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
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

            if(PHASE_TIME_POINT(0.3f) || PHASE_TIME_POINT(0.7f))
            {
                //g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_WOOSH_01);
            }

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
    else if(m_iPhase == 62u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            this->DefaultMoveLerp(m_vLastPosition, m_avVector[LAST_TARGET].xy(), ParaLerp(0.0f, -0.1f, 1.0f, fTime));

            if(PHASE_TIME_POINT(0.6f))
            {
                //g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.6f, SOUND_EFFECT_WOOSH_01);
            }

            if(PHASE_TIME_POINT(0.5f) && (m_aiCounter[RUSH_COUNT] == 1))
            {
                if(this->__ResurrectHelperIntro(ELEMENT_MAGENTA, true))
                {
                    g_pGame->GetHelper(ELEMENT_MAGENTA)->SetPosition(coreVector3(m_vLastPosition * FOREGROUND_AREA, 0.0f));
                }
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(61u)

                m_aiCounter[RUSH_COUNT] += 1;
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
        {
            if((iTick < 5u) || (g_pGame->IsEasy() && (iTick >= 12u))) return;

            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.1f, this, vPos,  vDir.Rotated90())->ChangeSize(1.8f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.1f, this, vPos, -vDir.Rotated90())->ChangeSize(1.8f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 3u, COLOR_ENERGY_MAGENTA);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 0.08f, LERP_HERMITE3)
        {
            const coreFloat   fOffset = SIN(fTime * (2.0f*PI)) * 0.5f;
            const coreVector2 vNewPos = coreVector2(fOffset, LERP(m_vLastPosition.y, 0.1f, fTime));

            this->SetPosition      (coreVector3(vNewPos * FOREGROUND_AREA, 0.0f));
            this->DefaultRotateLerp(1.0f*PI, 9.0f*PI, fTime);

            g_pEnvironment->SetTargetDirectionNow(-coreVector2(fOffset, 0.6f).Normalized());

            if(PHASE_TIME_POINT(0.75f))
            {
                this->__ResurrectHelperIntro(ELEMENT_RED, false);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(20u)
        });

        PHASE_CONTROL_TICKER(1u, 0u, 1.5f, LERP_LINEAR)
        {
            for(coreUintW i = g_pGame->IsEasy() ? 6u : 0u; i < 15u; ++i)
            {
                const coreVector2 vPos = this->GetPosition ().xy() + this->GetDirection().xy() * (I_TO_F(i) * 2.0f);
                const coreVector2 vDir = this->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.6f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.6f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_RED);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_BREAK)
        {
            const coreFloat fLastAngle = m_vLastDirection.xy().Angle();

            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(0.0f,0.3f), fTime);
            this->DefaultRotateLerp(fLastAngle,      fLastAngle - (6.0f*PI), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        PHASE_CONTROL_TIMER(0u, 0.33f, LERP_BREAK_REV)
        {
            const coreFloat fLastAngle = m_vLastDirection.xy().Angle();

            this->DefaultRotateLerp(fLastAngle, fLastAngle + (0.5f*PI), fTime);

            if(PHASE_FINISHED)
            {
                this->_CreateFragment(0u);

                g_pSpecialEffects->CreateExplosion (this->GetPosition());
                g_pSpecialEffects->CreateSplashDark(this->GetPosition(), 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.2f, 0.6f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->SlowScreen(4.0f);

                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        PHASE_CONTROL_TIMER(0u, 0.35f, LERP_BREAK)
        {
            m_avVector[BURST_DATA].x = fTime;

            if(PHASE_TIME_AFTER(0.5f))
            {
                this->Kill(false);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 200u)
    {
    }

    // ################################################################
    // ################################################################

    // 
    if((m_iPhase >= 30u) && (m_iPhase <= 34u))
    {
        PHASE_CONTROL_TICKER(1u, 0u, g_pGame->IsEasy() ? 8.0f : 12.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f,  this, vPos, vDir)->ChangeSize(1.5f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.05f, this, vPos, vDir)->ChangeSize(1.5f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f,  this, vPos, vDir)->ChangeSize(1.5f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 3u, COLOR_ENERGY_CYAN);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });
    }

    // 
    if(m_iPhase < 200u)
    {
        // 
        if(!pSkewer->HasStatus(ENEMY_STATUS_DEAD))
        {
            cEnemy* pTarget = m_Dummy.HasStatus(ENEMY_STATUS_DEAD) ? pSkewer : &m_Dummy;

            const coreMatrix3 mRota = coreMatrix4::RotationAxis(this->GetOrientation().xz().InvertedX().Angle(), this->GetDirection()).m123();

            pTarget->SetPosition   ( this->GetPosition   () + this->GetDirection() * 12.0f);
            pTarget->SetDirection  ((this->GetDirection  ().RotatedZ45() * mRota).Normalized());
            pTarget->SetOrientation( this->GetOrientation());
        }
        else g_pGame->GetCurMission()->FailTrophy();
    }

    // 
    const coreVector2 vBossDir = this->GetDirection().xy();
    const coreVector2 vOffset  = vBossDir * (m_avVector[BURST_DATA].x * FOREGROUND_AREA.x * 2.4f * SQRT2);
    const coreVector2 vRota    = MapToAxis(vBossDir, coreVector2::Direction(m_avVector[BURST_DATA].x * (7.0f*PI)));

    // 
    m_Blade.SetPosition   (coreVector3(this->GetPosition().xy() + vOffset - 14.0f * (vRota - vBossDir), this->GetPosition().z));
    m_Blade.SetDirection  (coreVector3(vRota, 0.0f));
    m_Blade.SetOrientation(this->GetOrientation());

    // 
    m_Hilt.SetPosition   (coreVector3(this->GetPosition().xy() - vOffset + 3.0f * (vRota - vBossDir), this->GetPosition().z));
    m_Hilt.SetDirection  (coreVector3(vRota, 0.0f));
    m_Hilt.SetOrientation(this->GetOrientation());

    // 
    //m_Shield.SetPosition (coreVector3(HIDDEN_POS, 0.0f));
    //m_Shield.SetDirection(coreVector3(0.0f, 1.0f, 0.0f));

    // 
    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(80u)

        this->_EndBoss();

        m_Blade.AddStatus(ENEMY_STATUS_GHOST);
        m_Hilt .AddStatus(ENEMY_STATUS_GHOST);

        g_pSpecialEffects->MacroExplosionDarkBig(this->GetPosition());
        g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
    }
    

    // 
    cHelper* pYellowHelper = g_pGame->GetHelper(ELEMENT_YELLOW);
    if(!pYellowHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pYellowHelper->GetLifeTime() * 0.5f;

        pYellowHelper->SetPosition(coreVector3(0.0f, 1.3f - 0.5f * SIN(fTime * (1.0f*PI)), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_YELLOW, false);
    }

    // 
    cHelper* pOrangeHelper = g_pGame->GetHelper(ELEMENT_ORANGE);
    if(!pOrangeHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat   fTime = pOrangeHelper->GetLifeTime() * 0.3f;
        const coreFloat   fLen  = LERP(1.3f * SQRT2, 1.02f, SIN(fTime * (1.0f*PI))) * FOREGROUND_AREA.x;
        const coreVector2 vDir  = this->GetPosition().xy().Normalized();
        const coreVector2 vPos  = vDir * fLen;

        pOrangeHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(fTime >= 1.0f)   this->_KillHelper(ELEMENT_ORANGE, false);
        if(m_iPhase != 40u) this->_KillHelper(ELEMENT_ORANGE, true);
    }

    // 
    cHelper* pRedHelper = g_pGame->GetHelper(ELEMENT_RED);
    if(!pRedHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pRedHelper->GetLifeTime() * 0.4f;

        pRedHelper->SetPosition(coreVector3(0.9f, LERP(1.3f, -1.3f, fTime), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_RED, false);
    }

    // 
    cHelper* pMagentaHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);
    if(!pMagentaHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        if(pMagentaHelper->GetLifeTime() >= 3.0f)
        {
            this->_KillHelper(ELEMENT_MAGENTA, true);
        }
    }

    // 
    cHelper* pPurpleHelper = g_pGame->GetHelper(ELEMENT_PURPLE);
    if(!pPurpleHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pPurpleHelper->GetPosition().xy() + coreVector2(0.0f, 1.1f * BULLET_SPEED_FACTOR * TIME);

        pPurpleHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pPurpleHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_PURPLE, false);
        }
    }

    // 
    cHelper* pBlueHelper = g_pGame->GetHelper(ELEMENT_BLUE);
    if(!pBlueHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pBlueHelper->GetLifeTime() * 0.5f;

        pBlueHelper->SetPosition(coreVector3(LerpSmoothRev(-1.3f, 1.3f, fTime), 0.9f, 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_BLUE, false);
    }

    // 
    cHelper* pCyanHelper = g_pGame->GetHelper(ELEMENT_CYAN);
    if(!pCyanHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pCyanHelper->GetLifeTime() * 0.7f;

        pCyanHelper->SetPosition(coreVector3(-0.9f, LERP(1.3f, -1.3f, fTime), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_CYAN, false);
    }

    // 
    cHelper* pGreenHelper = g_pGame->GetHelper(ELEMENT_GREEN);
    if(!pGreenHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pGreenHelper->GetLifeTime() * 0.4f;

        pGreenHelper->SetPosition(coreVector3(1.3f - 0.5f * SIN(fTime * (1.0f*PI)), 0.0f, 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_GREEN, false);
    }
}


// ****************************************************************
// 
coreBool cIntroBoss::__ResurrectHelperIntro(const coreUint8 iElement, const coreBool bSmooth)
{
    if(d_cast<cIntroMission*>(g_pGame->GetCurMission())->GetFirstPlay()) return false;

    return this->_ResurrectHelper(iElement, bSmooth);
}
