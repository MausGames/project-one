///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// - snow-laser must not move too fast (tested on 60 FPS), because it has no dynamic function and then leaves holes
// - attack during laser-phase is intended to keep the player away from the boss, so that they cannot do a super-short round
// - (old: as with the other bosses, the player is not allowed to receive any damage in the boss intro, which is why the damaging state is disabled)
// - in first phase, having multiple "attack all" phases did not work, as player could just fly back and forth between two limbs
// - in first phase, CW around, CCW around, 042513, the tail should pop out the head
// - in infinity phase, pattern shouldn't be too dense, otherwise the player won't be able to navigate through it ans then keeps the boss too far away
// - in infinity phase, since bullets now also move, global move should be slower so that the maximum speed it not too high, though bullets can then just stand still if both movements cancel each other (which is ok)
// - if all limbas ar outside for (regular) attacks, the shell invulnerability should also be removed
// - in infinity phase, the switch from 2-shot to 4-shot is not noticeable if there is no additional change in rotation
// - in ice-cube phase, the player should basically be prevented from getting to the boss with the cube
// - push-effect in the drag phase should not be too strong, otherwise the player has little room for movement
// - at the end of the drag phase, the player should be pulled towards the boss (background brake), which makes the transition to the swing phase more powerful
// - at the beginning and end of the drag phase, all bullets must be destroyed, otherwise the player will into the bullets too easily when accelerating and braking
// - (old: having the boss create the ice-cubes (e.g. with a beam) was too confusing, because it is at the same time as keeping the player away)
// - on the first ice-cube the boss is not allowed to make an attack, so the player can concentrate on understanding what needs to be done
// - boss should not swing too fast in the swing phase, so that you can attack him properly, and there should be a fixed pause between attacks (where boss is at the proper angle) in which you have time to turn
// - snow should only disappear after bounce transition, as an additional enhancement to this phase
// - transition in drag phase cannot happen at any time or sub-phase, because the sudden braking would seem strange (e.g. after the arc movement)
// - player can simply stand still during swing phase and all edge bullets will be cleanly destroyed, but the additional boss attack makes them move (the soeed if the edge bullets should not be increased futher, otherwise it will be too difficult in combination with the boss attack)
// ACHIEVEMENT: touch all the fresh ice-cubes
// COOP: only one player is chained to the boss
// TODO 1: hard mode: slipping movement (ice)
// TODO 1: in final phase, flip bullet and particles should be below enemies (also in stage), but cone bullets should be above, can that be split up in the bullet manager? or own bullet manager for boff (only for cones) [[but the whole thing is pretty bad, normal enemies cannot be made TOP (currently)]]
// TODO 1: chain should break on game over instead of just disappear (also in wave) (not when zero-length)


// ****************************************************************
// counter identifier
#define ICE_INDEX   (0u)
#define ICE_TOUCH   (1u)
#define EVADE_COUNT (2u)
#define STAR_HOLD   (3u)
#define STAR_COUNT  (4u)
#define DRAG_COUNT  (5u)
#define DRAG_SIDE   (6u)
#define CRASH_COUNT (7u)
#define SLAP_COUNT  (8u)
#define FLY_COUNT   (9u)
#define DIR_FIELD   (10u)


// ****************************************************************
// vector identifier
#define STAR_LENGTH   (0u)
#define LASER_MOVE    (1u)
#define INFINITY_ROTA (2u)
#define SHOT_TARGET   (3u)
#define ENV_ROTATION  (4u)
#define SLAP_ROTATION (5u)
#define ROTA_VALUE    (6u)


// ****************************************************************
// constructor
cZerothBoss::cZerothBoss()noexcept
: m_afLimbValue  {}
, m_vLaserDir    (coreVector2(0.0f,0.0f))
, m_fLaserTime   (0.0f)
, m_iLaserLimb   (0u)
, m_aiIceDir     {}
, m_abIcePreview {}
, m_vGlobalDir   (coreVector2(0.0f,1.0f))
, m_fAnimation   (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.0f);

    // configure the boss
    this->Configure(19000, COLOR_SHIP_BLUE);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    PHASE_HEALTH_GOAL({19000, 2100, 0})

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
    {
        coreHashString sModelHigh;
        coreHashString sModelLow;
        coreHashString sVolume;
             if(i == ZEROTH_LIMB_HEAD) {sModelHigh = "ship_boss_zeroth_head_high.md3"; sModelLow = "ship_boss_zeroth_head_low.md3"; sVolume = "ship_boss_zeroth_head_volume.md3";}
        else if(i == ZEROTH_LIMB_TAIL) {sModelHigh = "ship_boss_zeroth_tail.md3";      sModelLow = "ship_boss_zeroth_tail.md3";     sVolume = "ship_boss_zeroth_tail_volume.md3";}
        else                           {sModelHigh = "ship_boss_zeroth_leg.md3";       sModelLow = "ship_boss_zeroth_leg.md3";      sVolume = "ship_boss_zeroth_leg_volume.md3";}

        m_aLimb[i].DefineModelHigh(sModelHigh);
        m_aLimb[i].DefineModelLow (sModelLow);
        m_aLimb[i].DefineVolume   (sVolume);
        m_aLimb[i].SetSize        (this->GetSize());
        m_aLimb[i].Configure      (1, COLOR_SHIP_BLUE);
        m_aLimb[i].SetParent      (this);
    }

    // 
    m_Body.DefineModelHigh("ship_boss_zeroth_body_high.md3");
    m_Body.DefineModelLow ("ship_boss_zeroth_body_low.md3");
    m_Body.DefineVolume   ("ship_boss_zeroth_body_volume.md3");
    m_Body.SetSize        (this->GetSize());
    m_Body.Configure      (1, COLOR_SHIP_BLUE);
    m_Body.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_SECRET);
    m_Body.SetParent      (this);

    // 
    m_Laser.DefineModel  ("object_tube.md3");
    m_Laser.DefineTexture(0u, "effect_energy.png");
    m_Laser.DefineProgram("effect_energy_program");
    m_Laser.SetColor3    (COLOR_ENERGY_BLUE * 0.8f);
    m_Laser.SetTexSize   (ZEROTH_LASER_TEXSIZE);
    m_Laser.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_LaserWave.DefineModel  ("object_tube.md3");
    m_LaserWave.DefineTexture(0u, "effect_energy.png");
    m_LaserWave.DefineProgram("effect_energy_direct_program");
    m_LaserWave.SetColor3    (COLOR_ENERGY_BLUE * 1.5f);
    m_LaserWave.SetTexSize   (ZEROTH_LASERWAVE_TEXSIZE);
    m_LaserWave.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Indicator.DefineModel  ("object_sphere.md3");
    m_Indicator.DefineTexture(0u, "effect_energy.png");
    m_Indicator.DefineProgram("effect_energy_program");
    m_Indicator.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 3.0f);
    m_Indicator.SetTexSize   (coreVector2(4.5f,4.5f));
    m_Indicator.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
    {
        m_aIce[i].DefineModelHigh("object_cube_ice.md3");
        m_aIce[i].DefineModelLow ("object_cube_ice.md3");
        m_aIce[i].DefineVolume   ("object_cube_volume.md3");
        m_aIce[i].DefineTexture  (1u, "environment_water_norm.png");
        m_aIce[i].DefineProgram  ("object_ice_program");
        m_aIce[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 6.0f);
        m_aIce[i].SetTexSize     (coreVector2(0.25f,0.25f));
        m_aIce[i].Configure      (3500, coreVector3(1.0f,1.0f,1.0f));
        m_aIce[i].AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_GHOST_BULLET | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_SECRET | ENEMY_STATUS_NODELAY);

        ASSERT(coreMath::IsAligned(m_aIce[i].GetMaxHealth(), GAME_PLAYERS))
    }

    // 
    m_pLaserSound = Core::Manager::Resource->Get<coreSound>("effect_laser.wav");

    STATIC_ASSERT(offsetof(cZerothBoss, m_aLimb) < offsetof(cZerothBoss, m_Body))   // initialization order for collision detection
}


// ****************************************************************
// destructor
cZerothBoss::~cZerothBoss()
{
    // 
    this->Kill(false);

    // 
    this->__DisableLaser(false);
}


// ****************************************************************
// 
void cZerothBoss::ResurrectIntro()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

    // 
    for(coreUintW i = 0u, ie = g_pGame->IsMulti() ? CALOR_STARS : 1u; i < ie; ++i)
        pMission->EnableStar(i, this, coreVector2(0.0f,6.0f));

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) this->__SetLimbValue(i, 1.0f);
    this->__SetLimbValue(1u,               0.0f);
    this->__SetLimbValue(5u,               0.0f);
    this->__SetLimbValue(ZEROTH_LIMB_TAIL, 0.0f);

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) m_aLimb[i].ChangeToBottom();
    m_Body.ChangeToBottom();

    // 
    m_bForeshadow = true;

    // 
    this->AddStatus(ENEMY_STATUS_WORTHLESS);

    // 
    constexpr coreUint8 aiNewOrder[] = {cSpearBullet::ID};
    g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));

    // 
    m_iPhase = 200u;
    this->Resurrect();
}


// ****************************************************************
// 
void cZerothBoss::__ResurrectOwn()
{
    if(m_iPhase < 200u)
    {
        // 
        g_pGlow->BindObject(&m_Indicator);

        // 
        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) this->__SetLimbValue(i, 1.0f);

        // 
        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) m_aLimb[i].ChangeToNormal();
        m_Body.ChangeToNormal();

        // 
        constexpr coreUint8 aiNewOrder[] = {cConeBullet::ID};
        g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));

        // 
        this->_ResurrectBoss();
    }
}


// ****************************************************************
// 
void cZerothBoss::__KillOwn(const coreBool bAnimated)
{
    if(m_iPhase < 200u)
    {
        cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

        // 
        for(coreUintW i = 0u; i < CALOR_STARS; ++i) pMission->DisableStar(i, bAnimated);
        pMission->DisableSnow(bAnimated);

        // 
        this->__DisableLaser(bAnimated);

        // 
        for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
            this->__DestroyCube(i, bAnimated);

        // 
        g_pGlow->UnbindObject(&m_Indicator);

        // 
        if(m_pLaserSound->EnableRef(this)) m_pLaserSound->Stop();
    }

    // 
    this->RemoveStatus(ENEMY_STATUS_WORTHLESS);

    // 
    g_pGame->GetBulletManagerEnemy()->ResetOrder();

    // 
    m_iPhase = 0u;
}


// ****************************************************************
// 
void cZerothBoss::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_Laser.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Laser);

    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_LaserWave.Render();
    }
    glDepthMask(true);
}


// ****************************************************************
// 
void cZerothBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    m_Indicator.Render();

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
    {
        m_aIce[i].DefineTexture(0u, m_aIce[i].HasStatus(ENEMY_STATUS_DEAD) ? NULL : g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture);
    }
}


// ****************************************************************
// 
void cZerothBoss::__MoveOwn()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());
    cSnow*         pSnow    = pMission->GetSnow();

    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);

    coreFloat fStarLength = 4.0f;

    const auto nGetIndicatorPos = [this]()
    {
        return this->GetPosition() + coreVector3(0.0f,0.0f,7.0f);
    };
    const auto nGetShootPos = [&]()
    {
        return g_pForeground->Project3D(nGetIndicatorPos());
    };

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_BREAK)
        {
            if(PHASE_TIME_POINT(0.75f))
            {
                this->_StartBoss();
            }

            coreInt16 iCrash = 0;

            coreVector2 vPos = LERP(coreVector2(-0.1f,1.5f), coreVector2(-4.0f,-6.7f), fTime);

                                              while(ABS(vPos.x) > 1.0f) {iCrash += 1; vPos.x = SIGN(vPos.x) * 2.0f - vPos.x;}
            if(m_aiCounter[CRASH_COUNT] >= 1) while(ABS(vPos.y) > 1.0f) {iCrash += 1; vPos.y = SIGN(vPos.y) * 2.0f - vPos.y;}

            vPos *= FOREGROUND_AREA;

            if(m_aiCounter[CRASH_COUNT] < iCrash)
            {
                m_aiCounter[CRASH_COUNT] = iCrash;

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }

            for(coreUintW i = 0u; i < CALOR_STARS; ++i)
            {
                if(pMission->GetStarLength(i))
                {
                    pMission->SetStarLength(i, LERPH3(CALOR_CHAIN_CONSTRAINT1, 0.0f, fTime));
                }
            }

            this->SetPosition      (coreVector3(vPos, 0.0f));
            this->DefaultRotateLerp(1.0f*PI, 5.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });

        g_pEnvironment->SetTargetDirectionNow(coreVector2(this->GetPosition().x * BLENDBR(MIN1(m_fPhaseTime * 0.3f)), FOREGROUND_AREA.y * 3.0f).Normalized());
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(60u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        if(PHASE_MAINTIME_BEFORE(0.5f) || PHASE_MAINTIME_POINT(0.5f))
        {
            this->__SetLimbValue(ZEROTH_LIMB_TAIL, 1.0f - BLENDB(MIN1(m_fPhaseTime * 2.0f)));

            if(PHASE_MAINTIME_BEFORE(0.5f)) m_aLimb[ZEROTH_LIMB_TAIL].AddStatus(ENEMY_STATUS_GHOST);
        }

        if(m_aiCounter[EVADE_COUNT] < 3) m_avVector[ROTA_VALUE].y = MIN(m_avVector[ROTA_VALUE].y + 0.5f * TIME, 1.0f);
                                    else m_avVector[ROTA_VALUE].y = MAX(m_avVector[ROTA_VALUE].y - 1.0f * TIME, 0.5f);

        m_avVector[ROTA_VALUE].x += -1.0f * TIME * m_avVector[ROTA_VALUE].y;

        const coreVector2 vRota = coreVector2::Direction(m_avVector[ROTA_VALUE].x);

        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
        {
            if(m_aLimb[i].WasDamaged())
            {
                m_aiCounter[EVADE_COUNT] += 1;

                const coreVector2 vDir = MapToAxisInv(this->GetDirection().xy(), coreVector2::Direction(I_TO_F(i) * (2.0f*PI) / I_TO_F(ZEROTH_LIMBS)));
                g_pSpecialEffects->MacroEruptionColorBig(this->GetPosition() + coreVector3(vDir * ZEROTH_RADIUS, 0.0f), vDir, COLOR_ENERGY_WHITE * 0.8f);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);

                if(m_aiCounter[EVADE_COUNT] < 4)
                {
                    PHASE_RESET(0u)

                    m_aLimb[ZEROTH_LIMB_TAIL].AddStatus(ENEMY_STATUS_GHOST);
                    ASSERT(i == ZEROTH_LIMB_TAIL)
                }
                else if(m_aiCounter[EVADE_COUNT] < 7)
                {
                    this->__SetLimbValue(i, 1.0f);

                    coreUintW iNextLimb;
                    switch(m_aiCounter[EVADE_COUNT])
                    {
                    default: UNREACHABLE
                    case 4: iNextLimb = 0u; break;
                    case 5: iNextLimb = 4u; break;
                    case 6: iNextLimb = 2u; break;
                    }

                    this->__SetLimbValue(iNextLimb, 0.0f);
                }
                else if(m_aiCounter[EVADE_COUNT] < 8)
                {
                    for(coreUintW j = 0u; j < ZEROTH_LIMBS; ++j)
                    {
                        this->__SetLimbValue(j, (i == j) ? 1.0f : 0.0f);
                    }
                }
                else
                {
                    this->__SetLimbValue(i, 1.0f);

                    if(m_aiCounter[EVADE_COUNT] >= 12)
                    {
                        PHASE_CHANGE_INC
                    }
                }

                break;
            }
        }

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            if(m_aiCounter[EVADE_COUNT] < 1)
            {
                this->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            }
            else if(m_aiCounter[EVADE_COUNT] < 4)
            {
                coreFloat fOldAngle, fNewAngle;
                switch(m_aiCounter[EVADE_COUNT])
                {
                default: UNREACHABLE
                case 1: fOldAngle =  (3.0f/3.0f)*PI; fNewAngle =  (1.0f/3.0f)*PI; break;
                case 2: fOldAngle =  (1.0f/3.0f)*PI; fNewAngle = (-1.0f/3.0f)*PI; break;
                case 3: fOldAngle = (-1.0f/3.0f)*PI; fNewAngle = (-8.0f/3.0f)*PI; break;
                }

                this->DefaultRotateLerp(fOldAngle, fNewAngle, fTime);

                if(PHASE_FINISHED)
                {
                    m_aLimb[ZEROTH_LIMB_TAIL].RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
            else
            {
                this->DefaultRotate((-8.0f/3.0f)*PI);
            }
        });

        this->SetDirection(MapToAxis(this->GetDirection(), vRota));

        if(m_aiCounter[EVADE_COUNT] == 7)
        {
            this->_ResurrectHelper(ELEMENT_GREEN, false);
        }

        m_avVector[ROTA_VALUE].z += 0.4f * TIME * I_TO_F(MIN(m_aiCounter[EVADE_COUNT], 7));

        PHASE_CONTROL_TICKER(1u, 0u, 5.0f, LERP_LINEAR)
        {
            if(g_pGame->IsEasy() && ((iTick % 10u) >= 5u)) return;

            const coreVector2 vBaseDir = coreVector2::Direction(m_avVector[ROTA_VALUE].z);
            const coreVector2 vBasePos = nGetShootPos();

            for(coreUintW j = 3u; j--; )
            {
                const coreVector2 vDir = MapToAxis(coreVector2::Direction(I_TO_F(j) * ((4.0f/6.0f)*PI)), vBaseDir);
                const coreVector2 vPos = vBasePos + vDir * ZEROTH_RADIUS_BULLET;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(2.0f);
            }

            g_pSpecialEffects->CreateSplashColor(coreVector3(vBasePos, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
            g_pSpecialEffects->PlaySound(coreVector3(vBasePos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        PHASE_CONTROL_TIMER(2u, 1.0f, LERP_LINEAR)
        {
            this->__SetIndicator(fTime, COLOR_ENERGY_GREEN * 0.8f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            this->__SetIndicator(1.0f - fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(61u)

                g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(0u));
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        const coreFloat fCurHealthPct = this->GetCurHealthPct();

        if(PHASE_MAINTIME_POINT(0.5f))
        {
            pMission->EnableSnow();

            this->SetDirection(coreVector3(0.0f,1.0f,0.0f));
            this->__EnableLaser(ZEROTH_LIMB_HEAD, true);
        }

        m_avVector[LASER_MOVE].x += 1.0f * TIME;

        const coreFloat fSpeed = 0.075f * LERP(1.0f, 0.5f, STEP(0.95f, 1.0f, fCurHealthPct)) * MIN1(m_fPhaseTime * 1.0f) * (g_pGame->IsEasy() ? 0.7f : 1.1f);

        PHASE_CONTROL_TIMER(0u, fSpeed, LERP_LINEAR)
        {
            const coreVector2 vPos = coreVector2::Direction(m_avVector[LASER_MOVE].x * (-2.0f*PI) * 0.0375f) * LERPS(0.0f, 0.3f, CLAMP01((m_avVector[LASER_MOVE].x - 7.0f) * 0.1f));

            this->SetPosition      (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            this->DefaultRotateLerp(0.0f*PI, 6.0f*PI, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_AGAIN(0u)
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 0.2f, LERP_LINEAR)
        {
            if(!iTick) return;

            this->__DisableLaser(true);
            this->__EnableLaser((iTick * 4u) % ZEROTH_LIMBS, true);
        });

        PHASE_CONTROL_TIMER(2u, 0.2f, LERP_LINEAR)
        {
            const coreFloat fValue = BLENDH3(CLAMP01(0.5f + 0.7f * COS(fTime * (2.0f*PI))));

            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                this->__SetLimbValue(i, (i == m_iLaserLimb) ? 1.0f : fValue);
            }

            if(m_aLimb[ZEROTH_LIMB_TAIL].HasStatus(ENEMY_STATUS_GHOST))
            {
                m_Body.AddStatus(ENEMY_STATUS_INVINCIBLE);
            }
            else
            {
                m_Body.RemoveStatus(ENEMY_STATUS_INVINCIBLE);
            }

            if(m_avVector[LASER_MOVE].x >= 5.0f)
            {
                this->__SetIndicator(PHASE_TIME_BEFORE(0.45f) ? STEP(0.0f, 0.45f, fTime) : 0.0f, COLOR_ENERGY_MAGENTA * 1.0f);

                if(PHASE_TIME_POINT(0.45f))
                {
                    const coreVector2 vBase = nGetShootPos();

                    for(coreUintW j = 12u; j--; )
                    {
                        if(g_pGame->IsEasy() && (j % 2u)) continue;

                        const coreVector2 vDir = MapToAxis(coreVector2::Direction(I_TO_F(j) * ((1.0f/6.0f)*PI)), this->GetDirection().xy());
                        const coreVector2 vPos = vBase + vDir * ZEROTH_RADIUS_BULLET;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.8f, this, vPos, vDir)->ChangeSize(1.7f);
                    }

                    if(m_avVector[LASER_MOVE].x >= 10.0f)
                    {
                        if(this->_ResurrectHelper(ELEMENT_MAGENTA, true))
                        {
                            g_pGame->GetHelper(ELEMENT_MAGENTA)->SetPosition(coreVector3(vBase, 0.0f));
                        }
                    }

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vBase, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->PlaySound(coreVector3(vBase, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(PHASE_FINISHED)
            {
                PHASE_AGAIN(2u)
            }
        });

        if((fCurHealthPct <= 0.95f) && m_aLimb[ZEROTH_LIMB_TAIL].HasStatus(ENEMY_STATUS_HIDDEN))
        {
            PHASE_CHANGE_INC

            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                this->__SetLimbValue(i, 1.0f);
            }

            m_Body.AddStatus(ENEMY_STATUS_INVINCIBLE);

            this->__DisableLaser(true);

            if(!HAS_BIT(m_aiCounter[ICE_TOUCH], 1u))
            {
                pMission->GiveBadge(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition());
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 22u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(62u)

            g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(1u));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        const coreBool bExit = (coreMath::PopCount(m_aiCounter[DIR_FIELD]) >= ZEROTH_LIMBS);

        if(!bExit)
        {
            const coreVector2 vBaseDir = g_pGame->FindPlayerDual(0u)->GetDirection().xy();

            coreBool bSameBase = true;
            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                if(!SameDirection(vBaseDir, pPlayer->GetDirection().xy()))
                    bSameBase = false;

                pPlayer->ShowArrow(0u);
            });

            if(bSameBase) m_vGlobalDir = vBaseDir;
        }

        const coreFloat   fGlobalSpeed = g_pGame->IsEasy() ? 13.0f : 20.0f;
        const coreVector2 vGlobalMove  = m_vGlobalDir * (-fGlobalSpeed * TIME);

        if(PHASE_BEGINNING2)
        {
            this->SetPosition(coreVector3(m_vGlobalDir * FOREGROUND_AREA * 1.44f, 0.0f));

            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                this->__SetLimbValue(i, 0.0f);
                m_aLimb[i].ResetDamageForwarded();
            }

            pMission->DisableSnow(true);

            this->__SetIndicator(1.0f, COLOR_ENERGY_BLUE * 1.0f);
        }

        coreUintW iFinished = 0u;

        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
        {
            if(m_aLimb[i].GetDamageForwarded() >= 30 * g_pGame->GetNumPlayers())
            {
                this->__SetLimbValue(i, 1.0f);
                iFinished += 1u;

                if(!HAS_BIT(m_aiCounter[DIR_FIELD], i))
                {
                    ADD_BIT(m_aiCounter[DIR_FIELD], i)

                    const coreVector2 vDir = MapToAxisInv(this->GetDirection().xy(), coreVector2::Direction(I_TO_F(i) * (2.0f*PI) / I_TO_F(ZEROTH_LIMBS)));
                    g_pSpecialEffects->MacroEruptionColorBig(this->GetPosition() + coreVector3(vDir * ZEROTH_RADIUS, 0.0f), vDir, COLOR_ENERGY_WHITE * 0.8f);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
                }
            }
        }

        const coreBool bAdvance = (iFinished < 3u);

        m_avVector[INFINITY_ROTA].x += 1.0f * TIME * (bAdvance ? 1.0f : -1.0f);
        this->DefaultRotate(m_avVector[INFINITY_ROTA].x);

        PHASE_CONTROL_TICKER(0u, 0u, 11.0f, LERP_LINEAR)
        {
            if((iTick % 8u) < 5u) return;

            const coreVector2 vBase = nGetShootPos();

            for(coreUintW i = 0u; i < 4u; ++i)
            {
                if((bAdvance || g_pGame->IsEasy()) && (i % 2u)) continue;

                const coreVector2 vDir = MapToAxis(StepRotated90(i), this->GetDirection().xy());
                const coreVector2 vPos = vBase + vDir * ZEROTH_RADIUS_BULLET;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.6f, this, vPos, vDir)->ChangeSize(2.1f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }

            g_pSpecialEffects->CreateSplashColor(coreVector3(vBase, 0.0f), 25.0f, 5u, COLOR_ENERGY_BLUE);
            g_pSpecialEffects->PlaySound(coreVector3(vBase, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        const auto nMoveFunc = [&](coreObject3D* OUTPUT pObject)
        {
            coreVector2 vNewPos = pObject->GetPosition().xy() + vGlobalMove;

            if(bExit)
            {
                pObject->SetPosition(coreVector3(vNewPos, 0.0f));
                return !g_pForeground->IsVisiblePoint(vNewPos, 1.45f);
            }
            else
            {
                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.45f) && (vGlobalMove.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.9f;
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.45f) && (vGlobalMove.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.9f;
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.45f) && (vGlobalMove.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.9f;
                else if((vNewPos.y >  FOREGROUND_AREA.y * 1.45f) && (vGlobalMove.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.9f;

                pObject->SetPosition(coreVector3(vNewPos, 0.0f));
                return false;
            }
        };

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
        {
            if(pBullet->GetFlyTime() >= 4.0f)
            {
                pBullet->Deactivate(true);
            }
            else if(nMoveFunc(pBullet))
            {
                pBullet->Deactivate(false);
            }
        });

        if(nMoveFunc(this))
        {
            PHASE_CHANGE_TO(40u)

            g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

            this->__SetIndicator(0.0f);

            g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(2u));
        }

        if(iFinished >= 4u)
        {
            if(this->_ResurrectHelper(ELEMENT_BLUE, false))
            {
                g_pGame->GetHelper(ELEMENT_BLUE)->SetPosition(coreVector3(m_vGlobalDir * FOREGROUND_AREA * 1.3f, 0.0f));
            }
            nMoveFunc(g_pGame->GetHelper(ELEMENT_BLUE));
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
                {
                    this->__SetLimbValue(i, 1.0f);
                }
            }

            this->DefaultMoveLerp  (coreVector2(0.0f,1.5f), coreVector2(0.0f,0.7f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                3.0f*PI,                fTime);

            for(coreUintW i = 0u; i < CALOR_STARS; ++i)
                pMission->SetStarLength(i, LERPH3(0.0f, CALOR_CHAIN_CONSTRAINT1, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        m_avVector[INFINITY_ROTA].y -= 1.0f * TIME * MIN1(m_fPhaseTime) * (1.0f + 0.3f * I_TO_F(m_aiCounter[SLAP_COUNT]));
        this->DefaultRotate(m_avVector[INFINITY_ROTA].y);

        PHASE_CONTROL_TICKER(0u, 0u, 0.15f, LERP_LINEAR)
        {
            this->__CreateCube(coreVector2((iTick % 2u) ? -0.9f : 0.9f, -1.3f) * FOREGROUND_AREA, coreVector2(0.0f,1.0f), false);
        });

        if(PHASE_BEGINNING2)   // after control-macro
        {
            m_aTimer[0].SetValue(m_aTimer[0].GetValue(CORE_TIMER_GET_NORMAL) + 0.9f);
        }

        if(this->WasDamaged())
        {
            for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
            {
                if(m_aIce[i].ReachedDeath())
                {
                    const coreVector3 vPos = m_aIce[i].GetPosition();

                    m_aiCounter[SLAP_COUNT] += 1;

                    m_avVector[SLAP_ROTATION].x = 1.0f;
                    m_avVector[SLAP_ROTATION].y = vPos.x;

                    if(m_aiCounter[SLAP_COUNT] >= 4)
                    {
                        PHASE_CHANGE_TO(70u)

                        m_aiCounter[SLAP_COUNT] = 0;   // also prevent shooting

                        g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                        const coreUintW iIndex = m_aIce[i].LastAttackerIndex();

                        pMission->StopSwing();
                        pMission->SetStarLength(iIndex, CALOR_CHAIN_CONSTRAINT2);

                        pMission->DisableStar(1u - iIndex, true);
                        STATIC_ASSERT(CALOR_STARS == 2u)

                        m_aiCounter[STAR_HOLD] = iIndex + 1;

                        for(coreUintW j = 0u; j < ZEROTH_LIMBS; ++j)
                        {
                            this->__SetLimbValue(j, 0.0f);
                        }

                        m_Body.RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                        for(coreUintW j = 0u; j < ZEROTH_ICES; ++j)
                        {
                            this->__DestroyCube(j, true);
                        }

                        this->__SetIndicator(1.0f, COLOR_ENERGY_YELLOW * 0.9f);

                        g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(3u));
                    }
                    else if(m_aiCounter[SLAP_COUNT] >= 3)
                    {
                        if(this->_ResurrectHelper(ELEMENT_RED, true))
                        {
                            g_pGame->GetHelper(ELEMENT_RED)->SetPosition(vPos);
                        }
                    }

                    g_pSpecialEffects->MacroExplosionDarkBig(vPos);
                    g_pSpecialEffects->PlaySound(vPos, 1.2f, 1.0f, SOUND_ENEMY_EXPLOSION_03);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);

                    break;
                }
            }
        }

        if(m_aiCounter[SLAP_COUNT])
        {
            const coreFloat fSpeed = I_TO_F(m_aiCounter[SLAP_COUNT]) * 0.8f;
            const coreFloat fEasy  = g_pGame->IsEasy() ? 0.5f : 1.0f;

            PHASE_CONTROL_TICKER(1u, 0u, fSpeed * fEasy, LERP_LINEAR)
            {
                const coreVector2 vBase = nGetShootPos();

                for(coreUintW j = 80u; j--; )
                {
                    if(((j + ((iTick * 2u) % 5u) * 2u) % 10u) < 5u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f));
                    const coreVector2 vPos = vBase + vDir * ZEROTH_RADIUS_BULLET;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f * fEasy, this, vPos, vDir)->ChangeSize(1.3f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vBase, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_RED);
                g_pSpecialEffects->PlaySound(coreVector3(vBase, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            });

            PHASE_CONTROL_TIMER(2u, 1.0f, LERP_LINEAR)
            {
                this->__SetIndicator(fTime, COLOR_ENERGY_RED * 0.9f);
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreUintW iIndex = m_aiCounter[STAR_HOLD] - 1;
            ASSERT(m_aiCounter[STAR_HOLD])

            const coreVector2 vDiff = this->GetPosition().xy() - g_pGame->GetPlayer(iIndex)->GetOldPos();   // # old position, due to chain constraints

            const coreFloat fFrom = AnglePos(m_vLastDirection.xy().Angle());
            const coreFloat fTo   = vDiff.Angle();

            this->DefaultRotateLerp(fFrom, fFrom + AngleDiff(fTo, fFrom) + (4.0f*PI), BLENDB(MIN1(fTime)));

            const coreFloat fSpeed = LERPH3(ZEROTH_SPEED_FAST, ZEROTH_SPEED_SLOW, fTime) * (m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f);

            g_pEnvironment->SetTargetSpeedNow(fSpeed);

            this->__SetIndicator(1.0f - fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_avVector[STAR_LENGTH].x = vDiff.Length();

                pMission->CatchObject(iIndex, this);
                pMission->StartSwing(0.3f);
                pMission->SetStarLength(iIndex, m_avVector[STAR_LENGTH].x);

                this->SetCollisionModifier(m_Body.GetCollisionRange() * 0.7f);   // low-quad collision

                for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
                {
                    m_aLimb[i].AddStatus(ENEMY_STATUS_GHOST_PLAYER);
                }
                m_Body.AddStatus(ENEMY_STATUS_GHOST_PLAYER);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fLength = LERPS(m_avVector[STAR_LENGTH].x, 30.0f, fTime);

            const coreUintW iIndex = m_aiCounter[STAR_HOLD] - 1;
            ASSERT(m_aiCounter[STAR_HOLD])

            pMission->SetStarLength(iIndex, fLength);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 52u)
    {
        const coreInt32 iCurHealth = this->GetCurHealth();

        const coreFloat fEasy = g_pGame->IsEasy() ? 0.7f : 1.0f;

        PHASE_CONTROL_TICKER(0u, 0u, 60.0f * fEasy, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);
            const coreVector2 vPos = vDir * FOREGROUND_AREA * -1.2f * SQRT2;

            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.25f * fEasy, this, vPos, vDir)->ChangeSize(1.5f)->AddStatus(BULLET_STATUS_IMMORTAL);
        });

        const coreFloat fSpeed = LERP(1.6f, CORE_MATH_PRECISION, STEP(0.0f, 1000.0f, I_TO_F(iCurHealth))) * fEasy;

        PHASE_CONTROL_TICKER(1u, 0u, fSpeed, LERP_LINEAR)
        {
            const coreVector2 vTarget = this->NearestPlayerDual(iTick % 2u)->GetPosition().xy();

            for(coreUintW j = 12u; j--; )
            {
                const coreVector2 vBase = MapToAxis(coreVector2::Direction(I_TO_F(j) * ((1.0f/6.0f)*PI)), this->GetDirection().xy());

                const coreVector2 vPos = this->GetPosition().xy() + vBase * ZEROTH_RADIUS;
                const coreVector2 vDir = (vTarget - vPos).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.8f, this, vPos, vDir)->ChangeSize(1.7f);
            }

            const coreVector2 vBase = nGetShootPos();

            g_pSpecialEffects->CreateSplashColor(coreVector3(vBase, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_ORANGE);
            g_pSpecialEffects->PlaySound(coreVector3(vBase, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);

            if(iTick == 2u)
            {
                if(this->_ResurrectHelper(ELEMENT_ORANGE, true))
                {
                    g_pGame->GetHelper(ELEMENT_ORANGE)->SetPosition(coreVector3(vBase, 0.0f));
                }
            }
        });

        this->__SetIndicator(STEP(0.55f, 1.0f, m_aTimer[1].GetValue(CORE_TIMER_GET_NORMAL)), COLOR_ENERGY_ORANGE * 0.9f);

        if(iCurHealth <= 1150)
        {
            this->_ResurrectHelper(ELEMENT_PURPLE, false);
        }

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([](cFlipBullet* OUTPUT pBullet)
        {
            if(pBullet->GetFlyTime() >= 8.0f) pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 60u) || (m_iPhase == 61u) || (m_iPhase == 62u))
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[CRASH_COUNT] = 0;
        }

        coreFloat   fSpeed;
        coreVector2 vTarget;
        coreUint8   iPhase;
        coreBool    bEnviron;
        switch(m_iPhase)
        {
        default: UNREACHABLE
        case 60u: fSpeed = 0.25f; vTarget = coreVector2(-4.0f, 0.0f); iPhase = 10u; bEnviron = true;  break;
        case 61u: fSpeed = 0.2f;  vTarget = coreVector2( 8.0f, 4.0f); iPhase = 20u; bEnviron = true;  break;
        case 62u: fSpeed = 0.2f;  vTarget = coreVector2(-6.0f,-3.9f); iPhase = 30u; bEnviron = false; break;
        }

        PHASE_CONTROL_TIMER(0u, fSpeed, LERP_SMOOTH)
        {
            coreInt16 iCrash = 0;

            coreVector2 vPos = LERP(m_vLastPosition, vTarget, fTime);

            while(ABS(vPos.x) > 1.0f) {iCrash += 1; if((m_iPhase == 62u) && (iCrash >= 3)) {iCrash = 2; break;} vPos.x = SIGN(vPos.x) * 2.0f - vPos.x;}
            while(ABS(vPos.y) > 1.0f) {iCrash += 1;                                                             vPos.y = SIGN(vPos.y) * 2.0f - vPos.y;}

            vPos *= FOREGROUND_AREA;

            if(m_aiCounter[CRASH_COUNT] < iCrash)
            {
                m_aiCounter[CRASH_COUNT] = iCrash;

                for(coreUintW j = 60u; j--; )
                {
                    if((j % 6u) < 4u) continue;
                    if(g_pGame->IsEasy() && ((j % 12u) < 6u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 3.0f));

                    const coreFloat fSpeed = (j % 2u) ? 0.7f : 0.73f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.4f);
                }

                if((m_iPhase == 62u) && (iCrash == 3))
                {
                    if(this->_ResurrectHelper(ELEMENT_CYAN, true))
                    {
                        g_pGame->GetHelper(ELEMENT_CYAN)->SetPosition(coreVector3(vPos, 0.0f));
                    }
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }

            this->SetPosition      (coreVector3(vPos, 0.0f));
            this->DefaultRotateLerp(AnglePos(m_vLastDirection.xy().Angle()), 5.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(iPhase)
        });

        if(bEnviron) g_pEnvironment->SetTargetDirectionNow(coreVector2(this->GetPosition().x, FOREGROUND_AREA.y * 3.0f).Normalized());
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                g_pSpecialEffects->PlaySound(this->GetPosition(), 2.0f, 1.0f, SOUND_EFFECT_FLY);
            }

            const coreFloat fFrom = AnglePos(m_vLastDirection.xy().Angle());
            const coreFloat fTo   = 0.0f*PI;

            this->DefaultMoveLerp  (m_vLastPosition, m_vLastPosition + coreVector2(0.0f,-0.3f), SIN(BLENDB(fTime) * (1.0f*PI)));
            this->DefaultRotateLerp(fFrom,           fFrom + AngleDiff(fTo, fFrom) + (4.0f*PI), BLENDB(fTime));

            const coreFloat fSpeed = LERPH3(ZEROTH_SPEED_SLOW, ZEROTH_SPEED_FAST, MIN1(fTime * 2.0f));
            g_pEnvironment->SetTargetSpeedNow(fSpeed);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 71u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            const coreFloat fSide = m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f;

            const coreVector2 vDir = (GetCurBackRotation() ? coreVector2::Direction((I_TO_F(m_aiCounter[DRAG_COUNT]) + fTime) * (0.5f*PI)) : ENVIRONMENT_DEFAULT_DIRECTION) * fSide;

            this->SetPosition (coreVector3(vDir * FOREGROUND_AREA * 0.7f, 0.0f));
            this->SetDirection(coreVector3(vDir, 0.0f));

            g_pEnvironment->SetTargetDirectionNow(vDir * SIGN(g_pEnvironment->GetSpeed()));

            if(!GetCurBackRotation())
            {
                const coreFloat fBase = m_aiCounter[DRAG_SIDE] ? (1.0f*PI) : (0.0f*PI);
                this->DefaultRotateLerp(fBase, fBase + (4.0f*PI) * fSide, fTime);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 72u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            const coreFloat fSide = m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f;

            const coreVector2 vEnvDir = g_pEnvironment->GetDirection() * SIGN(g_pEnvironment->GetSpeed());

            const coreFloat   fTime1 = BLENDH3(fTime);
            const coreFloat   fSin1  = SIN(fTime1 * (2.0f*PI)) * fSide;
            const coreVector2 vPos1  = (vEnvDir * 0.7f + vEnvDir.Rotated90() * fSin1 * 0.7f) * FOREGROUND_AREA;

            const coreFloat   fTime2 = BLENDH3(CLAMP01((fTime - 0.05f) / 0.95f));
            const coreFloat   fSin2  = SIN(fTime2 * (2.0f*PI)) * fSide;
            const coreVector2 vPos2  = (vEnvDir * 0.5f + vEnvDir.Rotated90() * fSin2 * 0.7f) * FOREGROUND_AREA;

            const coreVector2 vDir = (vPos1 - vPos2).Normalized();

            this->SetPosition (coreVector3(vPos1, 0.0f));
            this->SetDirection(coreVector3(vDir,  0.0f));

            if((m_aiCounter[DRAG_COUNT] == 1) && PHASE_TIME_POINT(0.5f))
            {
                this->_ResurrectHelper(ELEMENT_YELLOW, false);
            }

            if(PHASE_FINISHED)
            {
                if(this->GetCurHealth() <= 2100)
                {
                    PHASE_CHANGE_TO(50u)

                    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_05);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                    g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_BOSS_DEFAULT(4u));
                }
                else
                {
                    PHASE_CHANGE_TO(73u)

                    m_aiCounter[DRAG_COUNT] += 1;
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 73u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING)
            {
                g_pSpecialEffects->PlaySound(this->GetPosition(), 2.0f, 1.0f, SOUND_EFFECT_FLY);
            }

            const coreFloat fSide = (m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f);
            const coreFloat fLerp = LERP(1.0f, -1.0f, fTime);

            const coreVector2 vDir  = g_pEnvironment->GetDirection() * fSide;
            const coreVector2 vRota = coreVector2::Direction(LERP(0.0f*PI, 3.0f*PI, fTime));

            this->SetPosition (coreVector3(vDir * FOREGROUND_AREA * (0.7f * fLerp), 0.0f));
            this->SetDirection(coreVector3(MapToAxis(vDir, vRota), 0.0f));

            g_pEnvironment->SetTargetSpeedNow(ZEROTH_SPEED_FAST * fSide * fLerp);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(71u)
                PHASE_RESET(3u)

                m_aiCounter[DRAG_SIDE] = 1 - m_aiCounter[DRAG_SIDE];
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                m_aLimb[i].AddStatus(ENEMY_STATUS_GHOST);
            }
            m_Body.AddStatus(ENEMY_STATUS_GHOST);

            this->__SetIndicator(0.0f);

            this->_EndBoss();

            g_pSpecialEffects->MacroExplosionPhysicalDarkBig(this->GetPosition());
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);

            m_avVector[ENV_ROTATION].x = g_pEnvironment->GetDirection().Angle();
            m_avVector[ENV_ROTATION].y = g_pEnvironment->GetSpeed();
        }

        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            //g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(m_avVector[ENV_ROTATION].x, 0.0f*PI, fTime)));   end rotation
        });

        PHASE_CONTROL_TIMER(1u, 0.25f, LERP_SMOOTH)
        {
            g_pEnvironment->SetTargetSpeedNow(LERP(m_avVector[ENV_ROTATION].y, ZEROTH_SPEED_SLOW, fTime));
        });

        PHASE_CONTROL_TIMER(2u, 0.18f, LERP_LINEAR)
        {
            pMission->SetSwingSpeed(LERP(0.3f, 1.0f, fTime));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        const coreUintW iIndex = m_aiCounter[STAR_HOLD] - 1;
        ASSERT(m_aiCounter[STAR_HOLD])

        const cPlayer* pPlayer = g_pGame->GetPlayer(iIndex);

        const coreVector2 vDiff    = this->GetPosition().xy() - pPlayer->GetPosition().xy();
        const coreVector2 vDiffOld = this->GetOldPos()        - pPlayer->GetOldPos();

        const coreFloat fDot    = coreVector2::Dot(vDiff,    coreVector2(0.0f,1.0f));
        const coreFloat fDotOld = coreVector2::Dot(vDiffOld, coreVector2(0.0f,1.0f));

        if((fDot > 0.0f) && (fDotOld <= 0.0f))
        {
            PHASE_CHANGE_INC

            g_pGame->ForEachPlayerAll([&](const cPlayer* pPlayer, const coreUintW i)
            {
                pMission->DisableStar(i, true);
            });

            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        const coreUintW iIndex = m_aiCounter[STAR_HOLD] - 1;
        ASSERT(m_aiCounter[STAR_HOLD])

        const coreFloat fSide = iIndex ? -1.0f : 1.0f;

        coreVector2 vNewPos = this->GetPosition().xy() + coreVector2(-1.0f * fSide, 10.0f).Normalized() * (130.0f * TIME);

        if(m_aiCounter[FLY_COUNT] < 2)
        {
            if(vNewPos.y > FOREGROUND_AREA.y * 1.5f)
            {
                vNewPos.y -= FOREGROUND_AREA.y * 3.0f;

                m_aiCounter[FLY_COUNT] += 1;
                switch(m_aiCounter[FLY_COUNT])
                {
                default: UNREACHABLE
                case 1: vNewPos.x = -0.1f * fSide * FOREGROUND_AREA.x; break;
                case 2: vNewPos.x =  0.4f * fSide * FOREGROUND_AREA.x; break;
                }

                g_pSpecialEffects->PlaySound(this->GetPosition(), 2.0f, 1.0f, SOUND_EFFECT_FLY);
            }
        }
        else
        {
            if(vNewPos.y > FOREGROUND_AREA.y * 1.0f)
            {
                m_Body.Kill(true);

                this->Kill(true);

                if(this->HasAllHelpers())
                {
                    this->_CreateFragment(6u);
                }

                g_pSpecialEffects->CreateExplosion (this->GetPosition());
                g_pSpecialEffects->CreateSplashDark(this->GetPosition(), 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.2f, 0.6f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->SlowScreen(4.0f);
            }
        }

        this->SetPosition  (coreVector3(vNewPos, 0.0f));
        this->DefaultRotate((m_fPhaseTime * (2.0f*PI) - (0.5f*PI)) * fSide);
    }

    // ################################################################
    // 
    else if(m_iPhase == 200u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK)
        {
            this->DefaultMoveLerp  (coreVector2(0.0f,1.5f), coreVector2(0.0f,0.7f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                -2.0f*PI,               fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 201u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            this->DefaultRotateLerp(0.0f*PI, 4.0f*PI, BLENDS(fTime));

            fStarLength = LERP(fStarLength, 20.0f, SIN(fTime * (1.0f*PI)));

            PHASE_CONTROL_TICKER(1u, 0u, 1.0f + 29.0f * SIN(fTime * (1.0f*PI)), LERP_LINEAR)
            {
                for(coreUintW j = g_pGame->IsEasy() ? 2u : 3u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick * 3u + j) * GA);
                    const coreVector2 vPos = this->GetPosition().xy() + vDir * ZEROTH_RADIUS;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.3f);

                    g_pSpecialEffects->CreateBlowColor(coreVector3(vPos, 0.0f), coreVector3(vDir, 0.0f), 25.0f, 1u, COLOR_ENERGY_RED);
                }

                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            });

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 202u)
    {
        PHASE_CONTROL_TIMER(1u, 0.1f, LERP_SMOOTH)
        {
            //this->DefaultRotate(SIN(fTime * (2.0f*PI)) * (-0.1f*PI));
            this->DefaultRotate(fTime * (-4.0f*PI));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });

        PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
        {
            if((iTick % 10u) < (g_pGame->IsEasy() ? 7u : 3u))
            {
                m_avVector[SHOT_TARGET].xy(this->NearestPlayerDual(0u)->GetPosition().xy());
                m_avVector[SHOT_TARGET].zw(this->NearestPlayerDual(1u)->GetPosition().xy());
            }
            else
            {
                const coreVector2 vMap = this->GetDirection().xy().Rotated90();//coreVector2(1.0f,0.0f);//coreVector2::Direction(I_TO_F((iTick / 10u) % 3u) * ((2.0f/3.0f)*PI) + ((1.0f/6.0f)*PI));
                
                for(coreUintW j = 1u; j--; )
                {
                    const coreVector2 vPos = this->GetPosition().xy() + vMap * (ZEROTH_RADIUS * (((iTick / 10u) % 2u) ? -1.2f : 1.2f));
                    const coreVector2 vDir = ((((iTick / 10u) % 2u) ? m_avVector[SHOT_TARGET].xy() : m_avVector[SHOT_TARGET].zw()) - vPos).Normalized();
    
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.3f, this, vPos, vDir)->ChangeSize(1.8f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_YELLOW);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
        });

        //constexpr coreFloat fDuration = 10.0f;

        //const coreFloat fTime1 = STEPH3(fDuration * (1.0f/6.0f), fDuration * (2.0f/6.0f), m_fPhaseTime);
        //const coreFloat fTime2 = STEPH3(fDuration * (2.0f/6.0f), fDuration * (3.0f/6.0f), m_fPhaseTime);
        //const coreFloat fTime3 = STEPH3(fDuration * (3.0f/6.0f), fDuration * (4.0f/6.0f), m_fPhaseTime);
        //const coreFloat fTime4 = STEPH3(fDuration * (4.0f/6.0f), fDuration * (5.0f/6.0f), m_fPhaseTime);

        //this->__SetLimbValue(2u,        1.0f - fTime1 + fTime3);
        //this->__SetLimbValue(4u,        1.0f - fTime2 + fTime4);
        //this->__SetLimbValue(ZEROTH_LIMB_TAIL, fTime2 - fTime3);

        //if(PHASE_MAINTIME_AFTER(fDuration))
        //    PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 203u)
    {
        PHASE_CONTROL_TIMER(0u, 0.165f, LERP_LINEAR)
        {
            this->DefaultRotate(LERPBR(0.0f*PI, 6.0f*PI, fTime));
            
                
            const cPlayer* pPlayer = g_pGame->GetPlayer(0u);
                
            const coreVector2 vBase = this->GetPosition().xy() + this->GetDirection().xy() * ZEROTH_RADIUS;
            const coreVector2 vDiff = pPlayer->GetPosition().xy() - vBase;

            fStarLength = LERPBR(fStarLength, vDiff.Length(), STEP(0.2f, 1.0f, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 204u)
    {
        const coreFloat fSpeed = 4.0f;
        
        this->DefaultRotate(m_fPhaseTime * fSpeed);

        cPlayer* pPlayer = g_pGame->GetPlayer(g_pGame->IsMulti() ? m_aiCounter[STAR_COUNT] : 0u);

        const coreVector2 vBase = this->GetPosition().xy() + this->GetDirection().xy() * ZEROTH_RADIUS;
        const coreVector2 vDiff = pPlayer->GetPosition().xy() - vBase;

        fStarLength = vDiff.Length();

        coreBool bTurn = false;
        if(F_TO_SI(m_fPhaseTimeBefore * fSpeed / (2.0f*PI)) != F_TO_SI(m_fPhaseTime * fSpeed / (2.0f*PI)))
        {
            bTurn = true;
        }

        if((pMission->GetCatchObject(m_aiCounter[STAR_COUNT]) != pPlayer) && (m_aiCounter[STAR_COUNT] < g_pGame->GetNumPlayers()))
        {
            const coreVector2 vCatchDiff = pPlayer->GetPosition().xy() - pMission->GetStar(m_aiCounter[STAR_COUNT])->GetPosition().xy();
            if(vCatchDiff.LengthSq() < POW2(4.0f))
            {
                if(pPlayer->HasStatus(PLAYER_STATUS_DEAD)) g_pGame->RepairPlayer();

                pMission->CatchObject(m_aiCounter[STAR_COUNT], pPlayer);
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_GHOST);

                g_pSpecialEffects->PlaySound(pPlayer->GetPosition(), 1.0f, 1.5f, SOUND_EFFECT_DUST);
                g_pSpecialEffects->RumblePlayer(pPlayer, SPECIAL_RUMBLE_SMALL, 250u);
            }
        }
        else if(bTurn)
        {
            if(!HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))
            {
                m_aiCounter[STAR_COUNT] += 1;

                if(m_aiCounter[STAR_COUNT] >= coreInt32(GAME_PLAYERS))   // always the same amount
                {
                    m_avVector[STAR_LENGTH].x = fStarLength;
                    PHASE_CHANGE_INC
                }
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 205u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            this->DefaultRotate(LERP(0.0f*PI, 1.0f*PI, fTime));

            fStarLength = LERPB(m_avVector[STAR_LENGTH].x, fStarLength, fTime);

            if(PHASE_FINISHED)
            {
                for(coreUintW i = 0u; i < CALOR_STARS; ++i)
                    pMission->SetStarConnect(i, false);

                const coreVector2 vPos = pMission->GetStar(0u)->GetPosition().xy();

                m_avVector[STAR_LENGTH].zw(vPos);

                g_pSpecialEffects->MacroEruptionColorSmall(coreVector3(vPos, 0.0f), coreVector2(0.0f,-1.0f), COLOR_ENERGY_WHITE * 0.8f);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_05);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 206u)
    {
        PHASE_CONTROL_TIMER(0u, 1.5f, LERP_LINEAR)
        {
            this->DefaultMoveLerp  (coreVector2(0.0f,0.7f), coreVector2(0.0f,1.8f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                -2.0f*PI,               fTime);

            if(PHASE_FINISHED)
            {
                this->Kill(false);

                g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->SetMoveSpeed(1.0f);
                });
            }
        });

        if(!m_avVector[STAR_LENGTH].y)         
        PHASE_CONTROL_TIMER(1u, 3.5f, LERP_LINEAR)
        {
            const coreVector2 vPos = LERP(m_avVector[STAR_LENGTH].zw(), coreVector2(0.0f,-1.1f) * FOREGROUND_AREA, fTime);

            for(coreUintW i = 0u; i < CALOR_STARS; ++i)
                pMission->GetStar(i)->SetPosition(coreVector3(vPos, 0.0f));

            if(PHASE_FINISHED)
            {
                m_avVector[STAR_LENGTH].y = 1.0f;

                for(coreUintW i = 0u, ie = g_pGame->IsMulti() ? CALOR_STARS : 1u; i < ie; ++i)
                {
                    cPlayer* pPlayer = d_cast<cPlayer*>(pMission->GetCatchObject(i));

                    pMission->DisableStar(i, false);
                    pMission->EnableStar (i, pPlayer, coreVector2(0.0f,0.0f));

                    pMission->GetStar(i)->SetPosition(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3);

                    pMission->UncatchObject(i);

                    pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
                    pPlayer->ApplyForce  (coreVector2(0.0f,50.0f));
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_GHOST);
                    pPlayer->SetMoveSpeed(0.0f);
                }

                g_pGame->GetBulletManagerEnemy()->ClearBulletsTyped<cOrbBullet>(true);

                g_pSpecialEffects->MacroExplosionColorBig(pMission->GetStar(0u)->GetPosition(), COLOR_ENERGY_WHITE * 0.8f);
                g_pSpecialEffects->PlaySound(pMission->GetStar(0u)->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
            }
        });
    }

    // ################################################################
    // ################################################################

    if(((m_iPhase >= 70u) && (m_iPhase < 80u)) || (m_iPhase == 50u))
    {
        const coreFloat   fPower = STEP(ZEROTH_SPEED_SLOW, ZEROTH_SPEED_FAST, ABS(g_pEnvironment->GetSpeed())) * SIGN(g_pEnvironment->GetSpeed()) * ((m_iPhase == 50u) ? -0.7f : 0.9f);
        const coreVector2 vPush  = g_pEnvironment->GetDirection() * (fPower * 60.0f * TIME);

        g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->SetPosition(pPlayer->GetPosition() - coreVector3(vPush, 0.0f));
        });

        if((m_iPhase == 71u) || (m_iPhase == 72u))
        {
            PHASE_CONTROL_TICKER(3u, 80u, 10.0f, LERP_LINEAR)
            {
                if((iTick % 10u) < (g_pGame->IsEasy() ? 8u : 6u)) return;

                const coreVector2 vBase = nGetShootPos();

                for(coreUintW j = 3u; j--; )
                {
                    const coreVector2 vDir = MapToAxis(g_pEnvironment->GetDirection() * -SIGN(g_pEnvironment->GetSpeed()), coreVector2::Direction((I_TO_F(j) - 1.0f) * 0.5f));
                    const coreVector2 vPos = vBase + vDir * ZEROTH_RADIUS_BULLET;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.8f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vBase, 0.0f), 25.0f, 5u, COLOR_ENERGY_YELLOW);
                g_pSpecialEffects->PlaySound(coreVector3(vBase, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            });
        }
    }

    if((m_iPhase >= 200u) && (m_iPhase <= 205u))
    {
        if(fStarLength >= 0.0f)
        {
            const coreVector2 vOldPos = pMission->GetStar(0u)->GetPosition().xy();
            const coreVector2 vNewPos = this->GetPosition().xy() + this->GetDirection().xy() * (6.0f + fStarLength);

            for(coreUintW i = 0u; i < CALOR_STARS; ++i)
            {
                pMission->GetStar(i)->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            if(InBetweenExt(0.0f, vOldPos.x, vNewPos.x) == ((m_iPhase == 201u) ? -1 : 1))
            {
                g_pSpecialEffects->PlaySound(coreVector3(vOldPos, 0.0f), 1.0f, 0.7f, SOUND_EFFECT_WOOSH_01);
            }
        }
    }

    if((m_iPhase >= 202u) && (m_iPhase <= 204u))
    {
        PHASE_CONTROL_TICKER(2u, 0u, 1.0f, LERP_LINEAR)
        {
            for(coreUintW j = 120u; j--; )
            {
                if((j % 8u) < (g_pGame->IsEasy() ? 5u : 4u)) continue;

                const coreVector2 vDir = coreVector2::Direction((I_TO_F(j) + I_TO_F(iTick % 3u) * (8.0f/3.0f)) * (2.0f*PI) / 120.0f * ((iTick % 2u) ? -1.0f : 1.0f));
                const coreVector2 vPos = this->GetPosition().xy() + vDir * ZEROTH_RADIUS;

                const coreFloat fSpeed = 0.9f + 0.08f * I_TO_F(1u - (j % 2u));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(2.1f);

                if(j % 2u) g_pSpecialEffects->CreateBlowColor(coreVector3(vPos, 0.0f), coreVector3(vDir, 0.0f), 25.0f, 1u, COLOR_ENERGY_BLUE);
            }

            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });
    }

    if(m_avVector[SLAP_ROTATION].x)
    {
        m_avVector[SLAP_ROTATION].x = MAX0(m_avVector[SLAP_ROTATION].x - 2.0f * TIME);
        g_pEnvironment->SetTargetDirectionNow(coreVector2(SIN(BLENDBR(m_avVector[SLAP_ROTATION].x) * (1.0f*PI)) * SIGN(m_avVector[SLAP_ROTATION].y), 5.0f).Normalized());
    }

    if(m_Laser.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        const coreVector2 vOldDir = m_Laser.GetDirection().xy();

        // 
        const coreFloat fOldTime = m_fLaserTime;
        m_fLaserTime.Update(0.8f);
        const coreFloat fNewTime = m_fLaserTime;

        // 
        const coreVector3 vBasePos = this->GetPosition();
        const coreVector3 vBaseDir = MapToAxisInv(this->GetDirection(), m_vLaserDir);
        const coreFloat   fAlpha   = (fNewTime < 1.0f) ? (0.6f * (1.0f - fNewTime)) : 1.0f;//  MIN1(m_Laser.GetAlpha() + 2.0f * TIME);
        const coreFloat   fScale   = (fNewTime < 1.0f) ? 1.0f : MIN1((fNewTime - 1.0f) * 8.0f);//coreVector3(fAlpha, 1.0f, fAlpha);

        // 
        m_Laser.SetPosition (vBasePos + vBaseDir * ZEROTH_LASER_SIZE.y);
        m_Laser.SetSize     (coreVector3(fScale, 1.0f, fScale) * ZEROTH_LASER_SIZE);
        m_Laser.SetDirection(vBaseDir);
        m_Laser.SetAlpha    (fAlpha);
        m_Laser.SetTexOffset(coreVector2(-0.2f,-0.6f) * m_fAnimation);
        m_Laser.Move();

        // 
        m_LaserWave.SetPosition (vBasePos + vBaseDir * ZEROTH_LASERWAVE_SIZE.y);
        m_LaserWave.SetSize     (coreVector3(fScale, 1.0f, fScale) * ZEROTH_LASERWAVE_SIZE);
        m_LaserWave.SetDirection(vBaseDir * -1.0f);
        m_LaserWave.SetAlpha    (fAlpha);
        m_LaserWave.SetTexOffset(coreVector2(0.3f,1.3f) * m_fAnimation);
        m_LaserWave.Move();

        if((fOldTime < 1.0f) && (fNewTime >= 1.0f))
        {
            // 
            this->__BeginLaser();

            // 
            const coreFloat   fLen = g_pForeground->RayIntersection(this->GetPosition().xy(), vBaseDir.xy());
            const coreVector2 vHit = this->GetPosition().xy() + vBaseDir.xy() * fLen;

            // 
            //g_pSpecialEffects->CreateSplashColor(coreVector3(vHit, 0.0f), SPECIAL_SPLASH_BIG, COLOR_ENERGY_BLUE);
            //g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->MacroExplosionColorBig(coreVector3(vHit, 0.0f), COLOR_ENERGY_BLUE);

            // 
            g_pSpecialEffects->MacroEruptionColorBig(this->GetPosition() + vBaseDir * ZEROTH_RADIUS, vBaseDir.xy(), COLOR_ENERGY_BLUE);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

            // 
            this->__CreateCube(vHit, -AlongCrossNormal(vHit), true);
        }

        if(fNewTime >= 1.0f)
        {
            const coreVector2 vRayPos = vBasePos.xy();
            const coreVector2 vRayDir = vBaseDir.xy();

            cPlayer::TestCollision(PLAYER_TEST_NORMAL, vRayPos, vRayDir, &m_Laser, [&](cPlayer* OUTPUT pPlayer, const coreFloat* pfHitDistance, const coreUint8 iHitCount, const coreBool bFirstHit)
            {
                if(!bFirstHit) return;
                if(pfHitDistance[0] <= 0.0f) return;

                const coreVector2 vIntersection = vRayPos + vRayDir * pfHitDistance[0];

                // 
                pPlayer->TakeDamage(5, ELEMENT_BLUE, vIntersection);

                // 
                g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vIntersection, 0.0f), COLOR_ENERGY_BLUE);
            });

            if(pSnow->IsActive())
            {
                const coreVector2 vSide = vRayDir.Rotated90();

                const coreFloat fFrom = vOldDir.Angle();
                const coreFloat fTo   = vRayDir.Angle();
                const coreFloat fDiff = AngleDiff(fTo, fFrom);
                const coreUintW iNum  = F_TO_UI(fDiff * 200.0f) + 1u;

                for(coreUintW i = 0u; i < iNum; ++i)
                {
                    const coreVector2 vFinalDir = coreVector2::Direction(fFrom + fDiff * (I_TO_F(i) * RCP(I_TO_F(iNum))));   // # current direction will be used in the next iteration

                    for(coreUintW j = 0u; j < 2u; ++j)
                    {
                        const coreVector2 vFinalPos = vRayPos + vSide * ((I_TO_F(j) - 0.5f) * 0.6f);

                        pSnow->DrawRay(vFinalPos, vFinalDir, SNOW_TYPE_ADD);
                    }
                }
            }

            const coreFloat   fLen    = g_pForeground->RayIntersection(vRayPos, vRayDir);
            const coreVector3 vTarget = coreVector3(vRayPos + vRayDir * fLen, 0.0f);

            PHASE_CONTROL_TICKER(4u, 0u, 60.0f, LERP_LINEAR)
            {
                g_pSpecialEffects->CreateSplashFire (vTarget, 20.0f, 6u, COLOR_ENERGY_BLUE * 0.15f);
                g_pSpecialEffects->CreateSplashColor(vTarget, 70.0f, 4u, COLOR_ENERGY_BLUE);
            });

            if(m_pLaserSound->EnableRef(this))
            {
                g_pSpecialEffects->ExternSetSource(m_pLaserSound, vTarget);
                m_pLaserSound->SetVolume(0.8f);
            }
        }
    }

    // 
    m_Indicator.SetPosition (nGetIndicatorPos());
    m_Indicator.SetTexOffset(coreVector2(0.0f, -0.3f * m_fAnimation));
    m_Indicator.Move();

    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
    {
        cCustomEnemy& oIce = m_aIce[i];
        if(oIce.HasStatus(ENEMY_STATUS_DEAD)) continue;

        const coreFloat   fLerp = m_abIcePreview[i] ? 1.0f : LERPB(1.0f, 0.0f, MIN1(oIce.GetLifeTime() * 0.5f));
        const coreVector2 vMove = UnpackDirection(m_aiIceDir[i]);
        const coreVector2 vPos  = oIce.GetPosition ().xy() + vMove * (40.0f * TIME * fLerp);
        const coreVector2 vDir  = oIce.GetDirection().xy() * coreMatrix3::Rotation(10.0f * TIME * fLerp).m12();

        // 
        oIce.SetPosition (coreVector3(vPos, 0.0f));
        oIce.SetDirection(coreVector3(vDir, 0.0f));

        if(m_abIcePreview[i])
        {
            // 
            if(!g_pForeground->IsVisibleObject(&oIce)) this->__DestroyCube(i, false);

            // 
            cPlayer::TestCollision(PLAYER_TEST_ALL, &oIce, [this](cPlayer* OUTPUT pPlayer, const cCustomEnemy* pIce, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                if(pPlayer->IsNormal()) pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vIntersection.xy());   // # no first-hit check
                ADD_BIT(m_aiCounter[ICE_TOUCH], 0u)
            });
        }

        if(pSnow->IsActive())
        {
            pSnow->DrawPoint(vPos, 5.0f, SNOW_TYPE_REMOVE);
        }
    }

    if(pSnow->IsActive())
    {
        pSnow->DrawPoint(this->GetPosition().xy(), 7.0f, SNOW_TYPE_REMOVE);
    }

    // 
    m_Body.SetPosition   (this->GetPosition   ());
    m_Body.SetDirection  (this->GetDirection  ());
    m_Body.SetOrientation(this->GetOrientation());

    // 
    const coreFloat fRotaBase = this->GetDirection().xy().Angle();

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
    {
        const coreVector2 vDir = coreVector2::Direction(fRotaBase - I_TO_F(i) * (2.0f/6.0f * PI));

        m_aLimb[i].SetPosition   (this->GetPosition   () + coreVector3(vDir * m_afLimbValue[i], 0.0f));
        m_aLimb[i].SetDirection  (this->GetDirection  ());
        m_aLimb[i].SetOrientation(this->GetOrientation());
    }

    // 
    m_aLimb[1].SetDirection(coreVector3(m_aLimb[1].GetDirection().xy().Rotated120() * -1.0f, 0.0f));
    m_aLimb[4].SetDirection(coreVector3(m_aLimb[4].GetDirection().xy().Rotated120(),         0.0f));
    m_aLimb[5].SetDirection(coreVector3(m_aLimb[5].GetDirection().xy()              * -1.0f, 0.0f));

    if(m_aiCounter[STAR_HOLD])
    {
        pMission->GetStar(m_aiCounter[STAR_HOLD] - 1)->SetPosition(this->GetPosition());
    }

    const coreFloat   fEnvSpeed = g_pEnvironment->GetSpeed();
    const coreVector2 vEnvDir   = g_pEnvironment->GetDirection() * SIGN(fEnvSpeed);

    g_pSpecialEffects->CreateGust(STEP(ZEROTH_SPEED_SLOW, ZEROTH_SPEED_FAST, ABS(fEnvSpeed)), vEnvDir.Angle());

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(80u)
    }
    
    if((m_iPhase < 200u) && !this->HasStatus(ENEMY_STATUS_DEAD))
    {
    // 
    cHelper* pYellowHelper = g_pGame->GetHelper(ELEMENT_YELLOW);
    if(!pYellowHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pYellowHelper->GetLifeTime() * 0.5f;

        pYellowHelper->SetPosition(coreVector3(-0.8f, LERP(1.3f, -1.3f, fTime), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_YELLOW, false);
    }

    // 
    cHelper* pOrangeHelper = g_pGame->GetHelper(ELEMENT_ORANGE);
    if(!pOrangeHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pOrangeHelper->GetLifeTime() * 0.2f;

        pOrangeHelper->SetPosition(coreVector3(0.0f, -1.3f + 0.5f * SIN(fTime * (1.0f*PI)), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_ORANGE, false);
    }

    // 
    cHelper* pRedHelper = g_pGame->GetHelper(ELEMENT_RED);
    if(!pRedHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pRedHelper->GetPosition().xy() + coreVector2(SIGN(pRedHelper->GetPosition().y) * (0.0f + 20.0f * pRedHelper->GetLifeTime()) * TIME, 0.0f);

        pRedHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pRedHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_RED, false);
        }
    }

    // 
    cHelper* pMagentaHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);
    if(!pMagentaHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pMagentaHelper->GetPosition().xy() + coreVector2(-1.0f,-1.0f).Normalized() * (FOREGROUND_AREA.x * 0.4f * TIME);

        pMagentaHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pMagentaHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_MAGENTA, false);
        }
    }

    // 
    cHelper* pPurpleHelper = g_pGame->GetHelper(ELEMENT_PURPLE);
    if(!pPurpleHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreUintW iIndex = m_aiCounter[STAR_HOLD] - 1;
        ASSERT(m_aiCounter[STAR_HOLD])

        const cPlayer* pPlayer = g_pGame->GetPlayer(iIndex);

        const coreVector2 vDiff = this->GetPosition().xy() - pPlayer->GetPosition().xy();

        const coreFloat   fTime = pPurpleHelper->GetLifeTime() * 0.25f;
        const coreFloat   fLen  = LERP(2.3f, 0.5f, SIN(fTime * (1.0f*PI))) * FOREGROUND_AREA.x;
        const coreVector2 vDir  = -vDiff.Normalized();
        const coreVector2 vPos  = pPlayer->GetPosition().xy() + vDir * fLen;

        pPurpleHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_PURPLE, false);
    }

    // 
    cHelper* pBlueHelper = g_pGame->GetHelper(ELEMENT_BLUE);
    if(!pBlueHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        if((pBlueHelper->GetLifeTime() >= 3.0f) || (m_iPhase != 30u))
        {
            this->_KillHelper(ELEMENT_BLUE, true);
        }
    }

    // 
    cHelper* pCyanHelper = g_pGame->GetHelper(ELEMENT_CYAN);
    if(!pCyanHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pCyanHelper->GetPosition().xy() + AlongCrossNormal(pCyanHelper->GetPosition().xy()) * (pCyanHelper->GetLifeTime() * 2.0f * TIME);

        pCyanHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pCyanHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_CYAN, true);
        }
    }

    // 
    cHelper* pGreenHelper = g_pGame->GetHelper(ELEMENT_GREEN);
    if(!pGreenHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pGreenHelper->GetLifeTime() * 0.2f;

        pGreenHelper->SetPosition(coreVector3(0.0f, 1.3f - 0.5f * SIN(fTime * (1.0f*PI)), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_GREEN, false);
    }
    }
}


// ****************************************************************
// 
void cZerothBoss::__EnableLaser(const coreUintW iLimb, const coreBool bAnimated)
{
    WARN_IF(m_Laser.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableLaser(false);

    // 
    ASSERT(iLimb < ZEROTH_LIMBS)
    m_vLaserDir  = coreVector2::Direction(I_TO_F(iLimb) * (2.0f*PI) / I_TO_F(ZEROTH_LIMBS));
    m_fLaserTime = bAnimated ? 0.0f : 1.0f;
    m_iLaserLimb = iLimb;

    // 
    if(!bAnimated) this->__BeginLaser();

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    nInitFunc(&m_Laser);
    nInitFunc(&m_LaserWave);

    // 
    const coreVector3 vPos = this->GetPosition();
    const coreVector3 vDir = MapToAxisInv(this->GetDirection(), m_vLaserDir);

    // 
    g_pSpecialEffects->CreateSplashColor(vPos + vDir * ZEROTH_RADIUS, SPECIAL_SPLASH_TINY, COLOR_ENERGY_BLUE);
    g_pSpecialEffects->PlaySound(vPos, 1.2f, 1.0f, SOUND_EFFECT_DUST);
}


// ****************************************************************
// 
void cZerothBoss::__DisableLaser(const coreBool bAnimated)
{
    if(!m_Laser.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_pLaserSound->EnableRef(this)) m_pLaserSound->Stop();

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(pObject);
    };
    nExitFunc(&m_Laser);
    nExitFunc(&m_LaserWave);

    if(bAnimated)
    {
        // 
        const coreVector3 vPos = m_Laser.GetPosition();
        const coreVector3 vDir = m_Laser.GetDirection();

        // 
        for(coreUintW j = 100u; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (1.0f * (I_TO_F(j) - 49.5f) + ZEROTH_RADIUS), 10.0f, 1u, COLOR_ENERGY_BLUE);
    }
}


// ****************************************************************
// 
void cZerothBoss::__BeginLaser()
{
    // 
    g_pSpecialEffects->ExternPlayPosition(m_pLaserSound, this, 0.0f, 0.5f, true, SOUND_EFFECT, coreVector3(0.0f,0.0f,0.0f));
}


// ****************************************************************
// 
void cZerothBoss::__CreateCube(const coreVector2 vPosition, const coreVector2 vDirection, const coreBool bPreview)
{
    // 
    if(!m_aIce[m_aiCounter[ICE_INDEX]].HasStatus(ENEMY_STATUS_DEAD))
        m_aiCounter[ICE_INDEX] = 1 - m_aiCounter[ICE_INDEX];

    // 
    cCustomEnemy& oIce = m_aIce[m_aiCounter[ICE_INDEX]];

    // 
    m_aiIceDir    [m_aiCounter[ICE_INDEX]] = PackDirection(vDirection);
    m_abIcePreview[m_aiCounter[ICE_INDEX]] = bPreview;

    // 
    if(bPreview) REMOVE_BIT(m_aiCounter[ICE_TOUCH], 0u)

    // 
    if(bPreview) oIce.ChangeToNormal();
            else oIce.ChangeToBottom();

    // 
    this->__DestroyCube(m_aiCounter[ICE_INDEX], true);
    oIce.Resurrect();

    // 
    oIce.SetPosition (coreVector3(vPosition,  0.0f));
    oIce.SetDirection(coreVector3(vDirection, 0.0f));
}


// ****************************************************************
// 
void cZerothBoss::__DestroyCube(const coreUintW iIndex, const coreBool bAnimated)
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

    // 
    ASSERT(iIndex < ZEROTH_ICES)
    cCustomEnemy& oIce = m_aIce[iIndex];

    // 
    if(oIce.HasStatus(ENEMY_STATUS_DEAD)) return;

    // 
    if(m_abIcePreview[iIndex])
    {
        if(!HAS_BIT(m_aiCounter[ICE_TOUCH], 0u))
        {
            ADD_BIT(m_aiCounter[ICE_TOUCH], 1u)
            pMission->FailTrophy();
        }
    }

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        if(pMission->GetCatchObject(i) == &oIce)
        {
            pMission->UncatchObject(i);
            break;
        }
    }

    // 
    oIce.Kill(bAnimated);
}


// ****************************************************************
// 
void cZerothBoss::__SetLimbValue(const coreUintW iIndex, const coreFloat fValue)
{
    ASSERT(iIndex < ZEROTH_LIMBS)
    cCustomEnemy& oLimb = m_aLimb[iIndex];

    // 
    m_afLimbValue[iIndex] = CLAMP01(fValue) * -5.0f;

    // 
    if(fValue >= 0.8f) oLimb.AddStatus   (ENEMY_STATUS_GHOST);
                  else oLimb.RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    if(fValue >= 1.0f) oLimb.AddStatus   (ENEMY_STATUS_HIDDEN);
                  else oLimb.RemoveStatus(ENEMY_STATUS_HIDDEN);
}


// ****************************************************************
// 
void cZerothBoss::__SetIndicator(const coreFloat fValue, const coreVector3 vColor)
{
    ASSERT((fValue >= 0.0f) && (fValue <= 1.0f))

    // 
    m_Indicator.SetAlpha  (BLENDH3(fValue));
    m_Indicator.SetEnabled(fValue ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(!vColor.IsNull()) m_Indicator.SetColor3(vColor);
}