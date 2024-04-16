///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 4: still old code left to clean up here


// ****************************************************************
// counter identifier
#define DUPLICATE_STATUS  (0u)
#define CURRENT_SIDE      (1u)
#define CURRENT_ITERATION (2u)
#define BOOMERANG_TARGET  (3u)
#define TELEPORT_COUNT    (4u)


// ****************************************************************
// vector identifier
#define STILL_ROTATION (0u)
#define STILL_MOVE     (1u)


// ****************************************************************
// constructor
cDharukBoss::cDharukBoss()noexcept
: m_DuplicateTrail   (DHARUK_DUPLICATE_RAWS)
, m_fDuplicateValue  (0.0f)
, m_Boomerang        (DHARUK_BOOMERANGS)
, m_BoomerangTrail   (DHARUK_BOOMERANGS * DHARUK_TRAILS)
, m_afBoomerangValue {}
, m_afVisibility     {}
, m_bHelperEvent     (false)
, m_iPackedDir       (0u)
, m_fAnimation       (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_dharuk_high.md3");
    this->DefineModelLow ("ship_boss_dharuk_low.md3");
    this->DefineVolume   ("ship_boss_dharuk_volume.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.4f);

    // configure the boss
    this->Configure(3100, COLOR_SHIP_RED);
    this->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_SECRET);

    // 
    PHASE_HEALTH_GOAL({3100, 2400, 1000, 0})

    // create duplicate object
    m_Duplicate.DefineModelHigh("ship_boss_dharuk_high.md3");
    m_Duplicate.DefineModelLow ("ship_boss_dharuk_low.md3");
    m_Duplicate.DefineVolume   ("ship_boss_dharuk_volume.md3");
    m_Duplicate.DefineTexture  (0u, "effect_energy.png");
    m_Duplicate.DefineProgram  ("effect_energy_blink_invert_program");
    m_Duplicate.SetSize        (this->GetSize());
    m_Duplicate.Configure      (1, COLOR_ENERGY_RED * 0.8f);
    m_Duplicate.AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_GHOST_BULLET | ENEMY_STATUS_SECRET);

    // create duplicate trail list
    m_DuplicateTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < DHARUK_DUPLICATE_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = (i % DHARUK_TRAILS) + 1u;

            // load object resources
            coreObject3D* pDuplicate = &m_aDuplicateRaw[i];
            pDuplicate->DefineModel  ("ship_boss_dharuk_low.md3");
            pDuplicate->DefineTexture(0u, "effect_energy.png");
            pDuplicate->DefineProgram("effect_energy_invert_program");

            // set object properties
            pDuplicate->SetSize   (this->GetSize());
            pDuplicate->SetColor3 (COLOR_ENERGY_RED * (0.15f + 0.15f * I_TO_F(iType)));
            pDuplicate->SetTexSize(coreVector2(1.0f,1.0f) * 0.5f);
            pDuplicate->SetEnabled((i < DHARUK_TRAILS) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_DuplicateTrail.BindObject(pDuplicate);
        }
    }

    // create boomerang lists
    m_Boomerang     .DefineProgram("effect_energy_invert_inst_program");
    m_BoomerangTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < DHARUK_BOOMERANGS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (DHARUK_TRAILS + 1u);

            // load object resources
            coreObject3D* pBoomerang = &m_aBoomerangRaw[i];
            pBoomerang->DefineModel  ("ship_boss_dharuk_boomerang.md3");
            pBoomerang->DefineVolume ("ship_boss_dharuk_boomerang_volume.md3");
            pBoomerang->DefineTexture(0u, "effect_energy.png");
            pBoomerang->DefineProgram("effect_energy_invert_program");

            // set object properties
            pBoomerang->SetSize   (coreVector3(1.0f,1.0f,1.0f) * 1.7f);
            pBoomerang->SetColor3 (COLOR_ENERGY_RED * (iType ? (0.15f + 0.15f * I_TO_F(iType)) : 0.8f));
            pBoomerang->SetTexSize(coreVector2(1.0f,1.0f) * 0.7f);
            pBoomerang->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BoomerangTrail.BindObject(pBoomerang);
                 else m_Boomerang     .BindObject(pBoomerang);
        }
    }

    // 
    m_Summon.DefineModel  ("object_sphere.md3");
    m_Summon.DefineTexture(0u, "effect_energy.png");
    m_Summon.DefineProgram("effect_energy_invert_program");
    m_Summon.SetColor3    (COLOR_ENERGY_RED * 0.8f);
    m_Summon.SetTexSize   (coreVector2(4.5f,4.5f));
    m_Summon.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// destructor
cDharukBoss::~cDharukBoss()
{
    // 
    this->Kill(false);

    // 
    this->__DisableDuplicate(false);
    this->__DisableSummon   (false);
    for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i) this->__DisableBoomerang(i, false);
}


// ****************************************************************
// 
void cDharukBoss::__ResurrectOwn()
{
    // 
    g_pGlow->BindList(&m_DuplicateTrail);
    g_pGlow->BindList(&m_Boomerang);
    g_pGlow->BindList(&m_BoomerangTrail);

    // 
    m_bForeshadow = true;
}


// ****************************************************************
// 
void cDharukBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->__DisableDuplicate(bAnimated);
    this->__DisableSummon   (bAnimated);
    for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i) this->__DisableBoomerang(i, bAnimated);

    // 
    g_pGlow->UnbindList(&m_DuplicateTrail);
    g_pGlow->UnbindList(&m_Boomerang);
    g_pGlow->UnbindList(&m_BoomerangTrail);
}


// ****************************************************************
// 
void cDharukBoss::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_DuplicateTrail.Render();
        m_BoomerangTrail.Render();
    }
    glDepthMask(true);
}


// ****************************************************************
// 
void cDharukBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    m_Boomerang.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Boomerang);

    // 
    m_Summon.Render();
}


// ****************************************************************
// 
void cDharukBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    // 
    const coreVector2 avOldBasePos  [2] = {this->GetPosition   ().xy(),         m_Duplicate.GetPosition   ().xy()};
    const coreFloat   afOldBaseAngle[2] = {this->GetDirection  ().xy().Angle(), m_Duplicate.GetDirection  ().xy().Angle()};
    const coreFloat   afOldBaseOri  [2] = {this->GetOrientation().xz().Angle(), m_Duplicate.GetOrientation().xz().Angle()};   // # not y

    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    m_bHelperEvent = false;

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.32f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) this->AddStatus(ENEMY_STATUS_GHOST);

            const auto nEffectFunc = [](const coreVector2 vPosition, const coreVector2 vDirection)
            {
                g_pSpecialEffects->CreateBlowColor(coreVector3(vPosition, 0.0f), coreVector3(vDirection, 0.0f), SPECIAL_BLOW_BIG, COLOR_ENERGY_RED);
                g_pSpecialEffects->PlaySound(coreVector3(vPosition, 0.0f), 1.0f, 1.0f, SOUND_EFFECT_FLY);
            };

            constexpr coreVector2 vFrom1 = coreVector2(-1.5f, 0.7f);
            constexpr coreVector2 vFrom2 = coreVector2( 0.9f, 1.5f);
            constexpr coreVector2 vFrom3 = coreVector2(-0.4f, 1.5f);
            constexpr coreVector2 vTo1   = coreVector2( 1.5f,-0.2f);
            constexpr coreVector2 vTo2   = coreVector2(-1.5f,-0.6f);
            constexpr coreVector2 vTo3   = coreVector2( 0.7f,-1.5f);

            const coreFloat fLen1   = (vTo1 - vFrom1).Length();
            const coreFloat fLen2   = (vTo2 - vFrom2).Length();
            const coreFloat fLen3   = (vTo3 - vFrom3).Length();
            const coreFloat fLenRev = RCP(fLen1 + fLen2 + fLen3);

            const coreFloat fSpeed1  = fLen1 * fLenRev;
            const coreFloat fSpeed2  = fLen2 * fLenRev;
            const coreFloat fSpeed3  = fLen3 * fLenRev;
            const coreFloat fSpeed12 = fSpeed1 + fSpeed2;

                 if(PHASE_TIME_BEFORE(fSpeed1))  this->DefaultMoveLerp(vFrom1, vTo1, (fTime)            * RCP(fSpeed1));
            else if(PHASE_TIME_BEFORE(fSpeed12)) this->DefaultMoveLerp(vFrom2, vTo2, (fTime - fSpeed1)  * RCP(fSpeed2));
            else                                 this->DefaultMoveLerp(vFrom3, vTo3, (fTime - fSpeed12) * RCP(fSpeed3));

                 if(PHASE_TIME_POINT(0.0f))     nEffectFunc(this->GetPosition().xy(), (vTo1 - vFrom1).Normalized());
            else if(PHASE_TIME_POINT(fSpeed1))  nEffectFunc(this->GetPosition().xy(), (vTo2 - vFrom2).Normalized());
            else if(PHASE_TIME_POINT(fSpeed12)) nEffectFunc(this->GetPosition().xy(), (vTo3 - vFrom3).Normalized());

            this->DefaultRotateLerp(1.0f*PI, 19.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            if(PHASE_BEGINNING) this->RemoveStatus(ENEMY_STATUS_GHOST);

            this->DefaultMoveLerp  (coreVector2(0.0f, DHARUK_OUTSIDE), coreVector2(0.0f, DHARUK_HEIGHT), fTime);
            this->DefaultRotateLerp(0.0f*PI,                           3.0f*PI,                          fTime);

            if(PHASE_TIME_POINT(0.5f))
                g_pGame->GetInterface()->ShowBoss(this);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(0.0f,0.0f), fTime);
            this->DefaultRotateLerp(1.0f*PI,         3.0f*PI,                fTime);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 100u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp  (coreVector2(0.0f, DHARUK_OUTSIDE), coreVector2(0.0f, DHARUK_HEIGHT), fTime);
            this->DefaultRotateLerp(0.0f*PI,                           3.0f*PI,                          fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(110u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 110u)
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[TELEPORT_COUNT] = 0;
        }

        PHASE_CONTROL_TIMER(0u, 0.45f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(0.0f, -DHARUK_OUTSIDE), fTime);
            this->DefaultRotateLerp(1.0f*PI,         5.0f*PI,                            fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                g_pSpecialEffects->PlaySound(coreVector3(0.0f,0.0f,0.0f), 1.0f, 1.0f, SOUND_EFFECT_FLY);
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[TELEPORT_COUNT] = 1;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 111u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f * 0.65f, LERP_LINEAR)
        {
            coreFloat fSide;
            switch(m_aiCounter[TELEPORT_COUNT] % (g_pGame->IsEasy() ? 4 : 5))
            {
            default: UNREACHABLE
            case 0: fSide = -1.0f; break;
            case 1: fSide =  1.0f; break;
            case 2: fSide = -1.0f; break;
            case 3: fSide =  1.0f; break;
            case 4: fSide =  0.0f; break;
            }
            fSide *= 0.6f;

            if(PHASE_BEGINNING)
            {
                this->__BecomeInvisible1();
                this->__BecomeInvisible2();

                m_fDuplicateValue = 5.0f;

                const coreVector2 vSound = MapStepRotated90(coreVector2(fSide, 0.0f), m_aiCounter[TELEPORT_COUNT] % 4);
                g_pSpecialEffects->PlaySound(coreVector3(vSound, 0.0f), 1.0f, 1.0f, SOUND_EFFECT_FLY);
            }

            this->DefaultMoveLerp  (coreVector2(fSide, DHARUK_OUTSIDE), coreVector2(fSide, -DHARUK_OUTSIDE), fTime);
            this->DefaultRotateLerp(5.0f*PI,                            13.0f*PI,                            fTime);

            //if(m_aiCounter[CURRENT_ITERATION])
            //{
            //    constexpr coreBool abFlip[] = {false, false, true, false, true, true};
            //    if(abFlip[m_aiCounter[TELEPORT_COUNT] % ARRAY_SIZE(abFlip)]) this->InvertY();
            //}

            this->ToAxis(StepRotated90(m_aiCounter[TELEPORT_COUNT] % 4));

            if(PHASE_FINISHED)
            {
                if(!m_aiCounter[CURRENT_ITERATION] && (this->GetCurHealth() <= 2400))
                {
                    PHASE_CHANGE_INC
                }
                else
                {
                    PHASE_AGAIN(0u)

                    m_aiCounter[TELEPORT_COUNT] += 1;

                    if(m_aiCounter[TELEPORT_COUNT] == 2u)
                    {
                        m_bHelperEvent = true;
                    }
                }
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 12.0f, LERP_LINEAR)
        {
            if((iTick % 3u) < 1u) return;
            if(g_pGame->IsEasy() && ((iTick % 6u) < 3u)) return;

            const coreVector2 vPos = this->GetPosition().xy() * (m_aiCounter[CURRENT_ITERATION] ? -1.0f : 1.0f);
            const coreVector2 vDir = StepRotated90(m_aiCounter[TELEPORT_COUNT] % 4).Rotated90();

            const coreFloat fSpeed = ((iTick % 3u) == 2u) ? 1.4f : 1.3f;

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.6f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.6f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_MAGENTA);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        if(this->ReachedHealth(2400))
        {
            g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_MAGENTA);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 112u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp  (coreVector2(0.0f, DHARUK_OUTSIDE), coreVector2(0.0f, DHARUK_HEIGHT), fTime);
            this->DefaultRotateLerp(0.0f*PI,                           3.0f*PI,                          fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(120u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 120u)
    {
        PHASE_CONTROL_TICKER(0u, g_pGame->IsEasy() ? 2u : 4u, m_Boomerang.GetNumEnabled() ? (2.2f/3.0f) : 1.0f, LERP_LINEAR)
        {
            if(iTick < DHARUK_BOOMERANGS)
            {
                const coreFloat fSideSign = (m_aiCounter[CURRENT_SIDE] ? -1.0f : 1.0f) * ((iTick & 0x01u) ? 1.0f : -1.0f);

                this->__EnableBoomerang(iTick, this->GetPosition().xy(), coreVector2(fSideSign, 0.0f));
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 121u)
    {
        m_avVector[STILL_MOVE].y = MIN1(m_avVector[STILL_MOVE].y + 0.2f * TIME);

        if(this->GetCurHealth() <= 1000)
        {
            PHASE_CHANGE_INC

            m_aiCounter[BOOMERANG_TARGET] = 1;
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 122u)
    {
        if(PHASE_BEGINNING2 && !m_aiCounter[CURRENT_ITERATION])
        {
            this->__EnableSummon(coreVector2(0.0f, -this->GetPosition().y));
        }

        m_avVector[STILL_MOVE].y = MAX0(m_avVector[STILL_MOVE].y - 1.0f * TIME);

        coreBool bDisabled = true;

        for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i)
        {
            const coreObject3D* pBoomerang = (*m_Boomerang.List())[i];
            if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            bDisabled = false;

            const coreVector2 vBossPos = m_aiCounter[CURRENT_ITERATION] ? this->GetPosition().xy() : this->GetPosition().xy().InvertedY();
            const coreVector2 vDiff    = vBossPos - pBoomerang->GetPosition().xy();

            if(coreMath::IsNear(vDiff.x, 0.0f, 1.0f) && coreMath::IsNear(vDiff.y, 0.0f, CORE_MATH_PRECISION))
            {
                this->__DisableBoomerang(i, true);
                m_Summon.SetSize(m_Summon.GetSize() + coreVector3(1.3f,1.3f,1.3f) * (g_pGame->IsEasy() ? 3.0f : 1.0f));

                g_pSpecialEffects->PlaySound(m_Summon.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
            }
        }

        if(bDisabled)
        {
            PHASE_CHANGE_INC

            m_aiCounter[BOOMERANG_TARGET]  = 0;
            m_aiCounter[CURRENT_SIDE]      = 1 - m_aiCounter[CURRENT_SIDE];
            m_aiCounter[CURRENT_ITERATION] = 1 + m_aiCounter[CURRENT_ITERATION];
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 123u)
    {
        PHASE_CONTROL_TIMER(0u, 2.2f/2.0f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING)
            {
                this->__EnableDuplicate();
                this->__DisableSummon(true);

                g_pSpecialEffects->PlaySound(m_Summon.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }

            const coreFloat fAlpha = MIN1(fTime * 10.0f);

            m_Duplicate.SetSize (fAlpha * this->GetSize());
            m_Duplicate.SetAlpha(fAlpha);

            for(coreUintW i = DHARUK_TRAILS; i < DHARUK_DUPLICATE_RAWS; ++i)
            {
                m_aDuplicateRaw[i].SetSize (m_Duplicate.GetSize());
                m_aDuplicateRaw[i].SetAlpha(fAlpha * m_aDuplicateRaw[i - DHARUK_TRAILS].GetAlpha());
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 124u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(110u)
        });
    }

    // ################################################################
    // ################################################################

    if((m_iPhase >= 120u) && (m_iPhase < 130u))
    {
        coreVector3 vRota = m_avVector[STILL_ROTATION].xyz();

        if((m_iPhase == 120u) || (m_iPhase == 121u))
        {
            if((m_iPhase == 120u) && !vRota.y) vRota.y = 1.0f*PI;

            vRota.x  = MIN1(vRota.x + 0.2f * TIME);
            vRota.y += vRota.x * 6.0f * TIME;
        }
        else
        {
            if(vRota.x)
            {
                const coreFloat fOldAngle = vRota.y;
                vRota.y += vRota.x * 6.0f * TIME;

                if(InBetweenExt(1.0f*PI, AnglePos(fOldAngle), AnglePos(vRota.y)) == 1)
                {
                    vRota.x = 0.0f;
                    vRota.y = 1.0f*PI;
                }
            }
            else
            {
                vRota.z = MIN1(vRota.z + 0.65f * TIME);
                vRota.y = LERPB(1.0f*PI, 3.0f*PI, vRota.z);
            }
        }

        m_avVector[STILL_MOVE].x += 0.5f * TIME;

        const coreFloat fSide = SIN(m_avVector[STILL_MOVE].x) * BLENDS(m_avVector[STILL_MOVE].y) * 10.0f;

        this->SetPosition (coreVector3(fSide, this->GetPosition().yz()));
        this->SetDirection(coreVector3(coreVector2::Direction(-vRota.y), 0.0f));

        m_avVector[STILL_ROTATION].xyz(vRota);
    }

    // 
    coreBool bChange = false;
    this->SetPosition(coreVector3(this->__RepeatPosition(this->GetPosition().xy(), DHARUK_OUTSIDE, &bChange), 0.0f));

    // 
    m_Duplicate.SetPosition   (-this->GetPosition   ());
    m_Duplicate.SetDirection  (-this->GetDirection  ());
    m_Duplicate.SetOrientation( this->GetOrientation().InvertedX());
    m_Duplicate.SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation * -0.5f)));

    if(g_pForeground->IsVisiblePoint(this->GetPosition().xy(), 1.1f))
    {
        g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
        {
            const coreVector2 vPos = pPlayer->GetPosition ().xy();
            const coreVector2 vDir = pPlayer->GetDirection().xy();
            const coreVector2 vTan = vDir.Rotated90();

            if(m_afVisibility[0] < -0.5f)
            {
                const coreVector2 vDiff = this->GetPosition().xy() - (vPos + vDir * -16.0f);
                const coreFloat   fLen  = coreVector2::Dot(vDiff, vDir) * 0.29f;
                const coreFloat   fSide = coreVector2::Dot(vDiff, vTan);

                if((ABS(fSide) < fLen) || pPlayer->WasDamaged())
                {
                    m_afVisibility[0] = 0.0f;
                }
            }

            if(m_afVisibility[1] < -0.5f)
            {
                const coreVector2 vDiff = m_Duplicate.GetPosition().xy() - (vPos + vDir * -16.0f);
                const coreFloat   fLen  = coreVector2::Dot(vDiff, vDir) * 0.29f;
                const coreFloat   fSide = coreVector2::Dot(vDiff, vTan);

                if((ABS(fSide) < fLen) || pPlayer->WasDamaged())
                {
                    m_afVisibility[1] = 0.0f;
                }
            }
        });

        if(m_afVisibility[0] < -0.5f)
        {
            if(this->WasDamaged())
            {
                m_afVisibility[0] = 0.0f;
            }
        }
    }

    if(m_afVisibility[0] > -0.5f)
    {
        m_afVisibility[0] = MIN1(m_afVisibility[0] + 10.0f * TIME);

        this->SetAlpha(BLENDH3(m_afVisibility[0]));
        this->RemoveStatus(ENEMY_STATUS_HIDDEN);
    }

    if(m_afVisibility[1] > -0.5f)
    {
        m_afVisibility[1] = MIN1(m_afVisibility[1] + 10.0f * TIME);

        m_Duplicate.SetAlpha(BLENDH3(m_afVisibility[1]));
        m_Duplicate.RemoveStatus(ENEMY_STATUS_HIDDEN);
    }

    // 
    if(TIME)
    {
        const coreFloat fRcpTime = RCP(TIME);

        // 
        const coreVector2 avCurBasePos  [2] = {this->GetPosition   ().xy(),         m_Duplicate.GetPosition   ().xy()};
        const coreFloat   afCurBaseAngle[2] = {this->GetDirection  ().xy().Angle(), m_Duplicate.GetDirection  ().xy().Angle()};
        const coreFloat   afCurBaseOri  [2] = {this->GetOrientation().xz().Angle(), m_Duplicate.GetOrientation().xz().Angle()};   // # not y
        const coreVector2 avOffsetPos   [2] = {         (avCurBasePos  [0] - avOldBasePos  [0]) * fRcpTime * 0.05f,           (avCurBasePos  [1] - avOldBasePos  [1]) * fRcpTime * 0.05f};
        const coreFloat   afOffsetAngle [2] = {AngleDiff(afCurBaseAngle[0],  afOldBaseAngle[0]) * fRcpTime * 0.055f, AngleDiff(afCurBaseAngle[1],  afOldBaseAngle[1]) * fRcpTime * 0.055f};
        const coreFloat   afOffsetOri   [2] = {AngleDiff(afCurBaseOri  [0],  afOldBaseOri  [0]) * fRcpTime * 0.06f,  AngleDiff(afCurBaseOri  [1],  afOldBaseOri  [1]) * fRcpTime * 0.06f};

        for(coreUintW i = 0u, ie = (m_aiCounter[DUPLICATE_STATUS] ? DHARUK_DUPLICATE_RAWS : DHARUK_TRAILS); i < ie; ++i)
        {
            // 
            const coreUintW iIndex  = (i < DHARUK_TRAILS) ? 0u : 1u;
            const coreFloat fFactor = I_TO_F(DHARUK_TRAILS - (i % DHARUK_TRAILS));

            // 
            const coreVector2 vNewPos =                        avCurBasePos  [iIndex] - avOffsetPos  [iIndex] * fFactor;
            const coreVector2 vNewDir = coreVector2::Direction(afCurBaseAngle[iIndex] - afOffsetAngle[iIndex] * fFactor);
            const coreVector2 vNewOri = coreVector2::Direction(afCurBaseOri  [iIndex] - afOffsetOri  [iIndex] * fFactor);

            // 
            m_aDuplicateRaw[i].SetPosition   (coreVector3(vNewPos,   0.0f));
            m_aDuplicateRaw[i].SetDirection  (coreVector3(vNewDir,   0.0f));
            m_aDuplicateRaw[i].SetOrientation(coreVector3(vNewOri.x, 0.0f, vNewOri.y));
            m_aDuplicateRaw[i].SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation * -0.5f)));
            m_aDuplicateRaw[i].Move();

            const coreFloat fAlpha = (i < DHARUK_TRAILS) ? this->GetAlpha() : m_Duplicate.GetAlpha();
            const coreUintW iType = (i % DHARUK_TRAILS) + 1u;

            m_aDuplicateRaw[i].SetAlpha(fAlpha * (0.15f + 0.2f * I_TO_F(iType)));
        }
    }

    // 
    for(coreUintW i = 0u; i < DHARUK_BOOMERANGS; ++i)
    {
        coreObject3D* pBoomerang = (*m_Boomerang.List())[i];
        if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreVector2 vFly = this->__DecodeDirection(i);

        // 
        coreVector2       vNewPos = pBoomerang->GetPosition().xy() + vFly * FOREGROUND_AREA * (1.5f * TIME);
        const coreVector2 vNewDir = coreVector2::Direction((1.0f/16.0f) * PI * (vNewPos.x + vNewPos.y) * (vFly.x + vFly.y));

        // 
        vNewPos = this->__RepeatPosition(vNewPos, 1.35f, &bChange);
        if(bChange)
        {
            // 
            const coreVector2 vBossPos   = m_aiCounter[CURRENT_ITERATION] ? this->GetPosition().xy() : this->GetPosition().xy().InvertedY();
            const coreVector2 vPlayerPos = g_pGame->FindPlayerSide(vNewPos)->GetPosition().xy();

            // 
            const coreVector2 vTarget = m_aiCounter[BOOMERANG_TARGET] ? vBossPos : vPlayerPos;
            const coreVector2 vFlyAbs = vFly.Processed(ABS);
            vNewPos = vNewPos * vFlyAbs + vTarget * vFlyAbs.yx();

            // 
            m_afBoomerangValue[i] = m_aiCounter[BOOMERANG_TARGET] ? -1.0f : LERP(2.5f, 8.5f, STEP(1000.0f, 2400.0f, I_TO_F(this->GetCurHealth())));

            // 
            g_pSpecialEffects->CreateBlowColor(coreVector3(vNewPos, 0.0f), coreVector3(vFly, 0.0f), SPECIAL_BLOW_SMALL, COLOR_ENERGY_RED);
        }

        // 
        if(m_afBoomerangValue[i] > -0.5f) m_afBoomerangValue[i] = MAX0(m_afBoomerangValue[i] - 6.0f * TIME);

        // 
        pBoomerang->SetPosition (coreVector3(vNewPos, 0.0f));
        pBoomerang->SetDirection(coreVector3(vNewDir, 0.0f));
        pBoomerang->SetAlpha    ((m_afBoomerangValue[i] > -0.5f) ? MIN1(m_afBoomerangValue[i]) : MIN1(pBoomerang->GetAlpha() + 10.0f * TIME));
        pBoomerang->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * 0.5f)));

        for(coreUintW j = 0u; j < DHARUK_TRAILS; ++j)
        {
            coreObject3D* pTrail = (*m_BoomerangTrail.List())[i*DHARUK_TRAILS + j];

            // 
            const coreVector2 vNewPos2 = pBoomerang->GetPosition().xy() - vFly * FOREGROUND_AREA * (0.06f * I_TO_F(DHARUK_TRAILS - j));
            const coreVector2 vNewDir2 = coreVector2::Direction((1.0f/16.0f) * PI * (vNewPos2.x + vNewPos2.y) * (vFly.x + vFly.y));

            // 
            pTrail->SetPosition (coreVector3(vNewPos2, 0.0f));
            pTrail->SetDirection(coreVector3(vNewDir2, 0.0f));
            pTrail->SetAlpha    (pBoomerang->GetAlpha() * (0.15f + 0.2f * I_TO_F(j + 1u)));
            pTrail->SetTexOffset(coreVector2(0.0f, FRACT(m_fAnimation * 0.5f)));
        }
    }

    // 
    m_Boomerang     .MoveNormal();
    m_BoomerangTrail.MoveNormal();

    // 
    m_Summon.SetTexOffset(coreVector2(0.0f, -1.5f * m_fAnimation));
    m_Summon.Move();

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_DHARUK_BOOMERANG, [this](cPlayer* OUTPUT pPlayer, coreObject3D* pBoomerang, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        const coreUintW iIndex = m_Boomerang.List()->index(pBoomerang);
        m_afBoomerangValue[iIndex] = 5.0f;

        // 
        pPlayer->TakeDamage(5, ELEMENT_RED, vIntersection.xy());

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_RED);
    });

    if(this->ReachedDeath())
    {
        this->__DisableDuplicate(true);

        g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_RED);
        g_pSpecialEffects->MacroDestructionColor(this, COLOR_ENERGY_RED);
        g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);

        this->Kill(false);

        g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
    }
}


// ****************************************************************
// 
void cDharukBoss::__EnableDuplicate()
{
    // 
    if(m_aiCounter[DUPLICATE_STATUS] != 0) return;
    m_aiCounter[DUPLICATE_STATUS] = 1;

    // 
    m_fDuplicateValue = -1.0f;

    // 
    m_Duplicate.Resurrect();

    // 
    m_Duplicate.SetPosition   (-this->GetPosition   ());
    m_Duplicate.SetDirection  (-this->GetDirection  ());
    m_Duplicate.SetOrientation( this->GetOrientation().InvertedX());
    m_Duplicate.SetAlpha      (0.0f);

    // 
    for(coreUintW i = DHARUK_TRAILS; i < DHARUK_DUPLICATE_RAWS; ++i)
    {
        m_aDuplicateRaw[i].SetAlpha  (0.0f);
        m_aDuplicateRaw[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    }

    // 
    g_pSpecialEffects->MacroExplosionColorBig(m_Duplicate.GetPosition(), COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cDharukBoss::__DisableDuplicate(const coreBool bAnimated)
{
    // 
    if(m_aiCounter[DUPLICATE_STATUS] == 0) return;
    m_aiCounter[DUPLICATE_STATUS] = 0;

    // 
    m_Duplicate.Kill(false);

    // 
    for(coreUintW i = DHARUK_TRAILS; i < DHARUK_DUPLICATE_RAWS; ++i)
        m_aDuplicateRaw[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->MacroExplosionColorBig(m_Duplicate.GetPosition(), COLOR_ENERGY_RED);
        g_pSpecialEffects->MacroDestructionColor(&m_Duplicate, COLOR_ENERGY_RED);
        g_pSpecialEffects->PlaySound(m_Duplicate.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
    }
}


// ****************************************************************
// 
void cDharukBoss::__EnableBoomerang(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)
    coreObject3D* pBoomerang = (*m_Boomerang     .List())[iIndex];
    coreObject3D* pTrail     = (*m_BoomerangTrail.List())[iIndex*DHARUK_TRAILS];

    // 
    WARN_IF(pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableBoomerang(iIndex, false);
    pBoomerang->ChangeType(TYPE_DHARUK_BOOMERANG);

    // 
    m_afBoomerangValue[iIndex] = -1.0f;

    // 
    this->__EncodeDirection(iIndex, vDirection);

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(coreVector3(vPosition, 0.0f));
        pObject->SetAlpha   (0.0f);
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pBoomerang);
    for(coreUintW i = 0u; i < DHARUK_TRAILS; ++i) nInitFunc(pTrail + i);

    // 
    g_pSpecialEffects->MacroEruptionColorSmall(coreVector3(vPosition, 0.0f), vDirection, COLOR_ENERGY_RED);
    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
}


// ****************************************************************
// 
void cDharukBoss::__DisableBoomerang(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)
    coreObject3D* pBoomerang = (*m_Boomerang     .List())[iIndex];
    coreObject3D* pTrail     = (*m_BoomerangTrail.List())[iIndex*DHARUK_TRAILS];

    // 
    if(!pBoomerang->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    pBoomerang->ChangeType(0);

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitFunc(pBoomerang);
    for(coreUintW i = 0u; i < DHARUK_TRAILS; ++i) nExitFunc(pTrail + i);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pBoomerang->GetPosition(), COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cDharukBoss::__EnableSummon(const coreVector2 vPosition)
{
    WARN_IF(m_Summon.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableSummon(false);

    // 
    m_Summon.SetPosition(coreVector3(vPosition,0.0f));
    m_Summon.SetSize    (coreVector3(0.0f,0.0f,0.0f));

    // 
    m_Summon.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Summon);
}


// ****************************************************************
// 
void cDharukBoss::__DisableSummon(const coreBool bAnimated)
{
    if(!m_Summon.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Summon.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Summon);
}


// ****************************************************************
// 
coreVector2 cDharukBoss::__RepeatPosition(const coreVector2 vPosition, const coreFloat fThreshold, coreBool* OUTPUT pbChange)
{
    // 
    const coreVector2 vThreshVec = fThreshold * FOREGROUND_AREA;
    const coreVector2 vRepeatPos = coreVector2(FmodRange(vPosition.x, -vThreshVec.x, vThreshVec.x),
                                               FmodRange(vPosition.y, -vThreshVec.y, vThreshVec.y));

    // 
    ASSERT(pbChange)
    (*pbChange) = (vRepeatPos.x != vPosition.x) || (vRepeatPos.y != vPosition.y);

    return vRepeatPos;
}

coreVector2 cDharukBoss::__RepeatPosition(const coreVector2 vPosition, const coreFloat fThreshold)
{
    coreBool bChange;
    return this->__RepeatPosition(vPosition, fThreshold, &bChange);
}


// ****************************************************************
// 
void cDharukBoss::__EncodeDirection(const coreUintW iIndex, const coreVector2 vDirection)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)

    // 
    if(vDirection.x + vDirection.y > 0.0f)
         ADD_BIT   (m_iPackedDir, 0u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 0u + 2u*iIndex)

    // 
    if(coreMath::IsNear(vDirection.y, 0.0f))
         ADD_BIT   (m_iPackedDir, 1u + 2u*iIndex)
    else REMOVE_BIT(m_iPackedDir, 1u + 2u*iIndex)

    STATIC_ASSERT(DHARUK_BOOMERANGS*2u <= sizeof(m_iPackedDir)*8u)
}


// ****************************************************************
// 
coreVector2 cDharukBoss::__DecodeDirection(const coreUintW iIndex)
{
    ASSERT(iIndex < DHARUK_BOOMERANGS)

    // 
    const coreFloat P = HAS_BIT(m_iPackedDir, 0u + 2u*iIndex) ? 1.0f : -1.0f;
    const coreBool  X = HAS_BIT(m_iPackedDir, 1u + 2u*iIndex);

    // 
    return coreVector2((X) ? P : 0.0f, (!X) ? P : 0.0f);
}