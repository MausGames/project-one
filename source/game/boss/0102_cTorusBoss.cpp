///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// nicht zu wenig erzeugen, aber trotzdem etwas "kräftigere" objekte als der gradius boss
// am anfang delay um die spear-bullet zu zeigen, bevor man einen anderen angriff damit kombiniert
// nach jump, wenn ein objekt erzeugt wird, begwegt sich der boss auf spieler zu, damit das erzeugte objekt nicht sofort/einfach zerstört werden kann
// laser sollten nicht unabhängig von der charge-attack erzeugt werden, weil sie sonst gleichzeitig passieren und den spieler verwirren und sein ausweich-manöver blockieren
// objekte sollten nicht zu schnell erzeugt werden, weil sie sonst überfordern, vor allem bei der wand-bounce phase
// schuss-geschwindigkeit der spear-bullets und dreh-geschwindigkeit sollten immer ein schönes muster haben
// charge-attack sollte nicht gemacht werden, wenn spieler zu nah am rand ist (perpendicular), weil sonst die beid-seitige ausweich-möglichkeit fehlt
// bei roter durchgehender attacke sollte man grad noch so durchschlüpfen können
// view-bullets am ende sollen durchgängig sein, um spieler im kreis bewegen zu lassen (wenn möglich), aber trotzdem leicht versetzt und ausgefranst, um gracing zu erschweren
// die schilde bringen spieler dazu sich um boss herum zu bewegen
// durchgängig aufrichtige bewegung is kompletter bullshit, animation schaut orsch aus wenn sie boss auch dabei dreht, und der boss wirkt nicht mehr so massiv
// ACHIEVEMENT: fly through the gate without taking damage
// TODO 4: clean up code (also in mission)
// TODO 1: hard mode: all object behaviors increase, gelber angriff is undurchdringbar (7 statt 3 geschosse)
// TODO 1: hard mode: neue beschwörung die nicht zerstört werden kann (vielleicht finaler ball kann nicht zerstört werden)
// TODO 3: purple helper shield wird unter laser gezeichnet
// TODO 1: [MF] [HIGH] MAIN: sound


// ****************************************************************
// counter identifier
#define EMIT_STATUS  (0u)
#define CUR_TARGET   (1u)
#define SLIDE_COUNT  (2u)
#define CRASH_COUNT  (3u)
#define TURRET_DIR   (4u)
#define TURRET_COUNT (5u)
#define DRIVER_COUNT (6u)


// ****************************************************************
// vector identifier
#define GRIND_VALUE      (0u)
#define OVERDRIVE_HIT    (1u)
#define JUMP_TARGET      (2u)
#define COLOR_CHANGE     (3u)
#define BARRIER_ANGLE    (4u)
#define ROTATE_SAVE      (5u)
#define HEIGHT_SAVE      (6u)
#define HELPER_DATA      (7u)
#define TUMBLE_DIRECTION (8u)
#define MIRROR_LERP      (9u)
#define GATE_DATA        (10u)


// ****************************************************************
// constructor
cTorusBoss::cTorusBoss()noexcept
: m_TurretHull      (TORUS_TURRETS)
, m_GunnerHull      (TORUS_GUNNERS)
, m_ChargerHull     (TORUS_CHARGERS)
, m_DriverHull      (TORUS_DRIVERS)
, m_WaverHull       (TORUS_WAVERS)
, m_fAnimation      (0.0f)
, m_fRotationBoss   (0.0f)
, m_fRotationObject (0.0f)
, m_iTurretActive   (0u)
, m_iTurretMove     (0u)
, m_iGunnerActive   (0u)
, m_iChargerActive  (0u)
, m_iDriverActive   (0u)
, m_iWaverActive    (0u)
, m_iDecalState     (0u)
{
    // load models
    this->DefineModelHigh("ship_boss_torus_high.md3");
    this->DefineModelLow ("ship_boss_torus_low.md3");
    this->DefineVolume   ("ship_boss_torus_volume.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 2.0f);

    // configure the boss
    this->Configure(7500, 0u, COLOR_SHIP_GREY);
    this->AddStatus(ENEMY_STATUS_SECRET);

    // 
    PHASE_HEALTH_GOAL({7500, 6300, 5600, 4200, 3500, 2800, 1400, 0})

    // 
    m_Emitter.DefineModel  ("ship_boss_torus_emitter.md3");
    m_Emitter.DefineTexture(0u, "effect_energy.png");
    m_Emitter.DefineProgram("effect_energy_invert_program");
    m_Emitter.SetSize      (this->GetSize());
    m_Emitter.SetColor3    (COLOR_ENERGY_YELLOW * 0.8f);
    m_Emitter.SetTexSize   (coreVector2(1.25f,1.25f));

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
    {
        m_aCircle[i].DefineModel  ("ship_boss_torus_circle.md3");
        m_aCircle[i].DefineTexture(0u, "effect_energy.png");
        m_aCircle[i].DefineProgram("effect_energy_invert_program");
        m_aCircle[i].SetSize      (this->GetSize());
        m_aCircle[i].SetColor3    (COLOR_ENERGY_WHITE * 0.3f);
        m_aCircle[i].SetTexSize   (coreVector2(1.25f,1.25f));
    }

    // 
    m_Summon.DefineModel  ("object_sphere.md3");
    m_Summon.DefineTexture(0u, "effect_energy.png");
    m_Summon.DefineProgram("effect_energy_invert_program");
    m_Summon.SetTexSize   (coreVector2(4.5f,4.5f));
    m_Summon.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
    {
        m_aTurret[i].DefineModelHigh("object_cube_top.md3");
        m_aTurret[i].DefineModelLow ("object_cube_top.md3");
        m_aTurret[i].DefineVolume   ("object_cube_volume.md3");
        m_aTurret[i].DefineTexture  (0u, "effect_energy.png");
        m_aTurret[i].DefineProgram  ("effect_energy_blink_flat_invert_program");
        m_aTurret[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 3.2f);
        m_aTurret[i].SetTexSize     (coreVector2(0.4f,0.4f));
        m_aTurret[i].Configure      (10, 0u, COLOR_ENERGY_CYAN * 0.7f);
        m_aTurret[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT | ENEMY_STATUS_SECRET);
    }

    // 
    m_TurretHull.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            // load object resources
            coreObject3D* pTurret = &m_aTurretHullRaw[i];
            pTurret->DefineModel  ("object_cube_top.md3");
            pTurret->DefineTexture(0u, "effect_energy.png");
            pTurret->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pTurret->SetSize   (m_aTurret[i].GetSize   ());
            pTurret->SetColor3 (m_aTurret[i].GetColor3 ());
            pTurret->SetTexSize(m_aTurret[i].GetTexSize());
            pTurret->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_TurretHull.BindObject(pTurret);
        }
    }

    // 
    for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
    {
        m_aGunner[i].DefineModelHigh("object_tetra_top.md3");
        m_aGunner[i].DefineModelLow ("object_tetra_top.md3");
        m_aGunner[i].DefineVolume   ("object_tetra_volume.md3");
        m_aGunner[i].DefineTexture  (0u, "effect_energy.png");
        m_aGunner[i].DefineProgram  ("effect_energy_blink_flat_invert_program");
        m_aGunner[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 3.0f);
        m_aGunner[i].SetTexSize     (coreVector2(0.4f,0.4f));
        m_aGunner[i].Configure      (10, 0u, COLOR_ENERGY_RED * 0.8f);
        m_aGunner[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT | ENEMY_STATUS_SECRET);
    }

    // 
    m_GunnerHull.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        {
            // load object resources
            coreObject3D* pGunner = &m_aGunnerHullRaw[i];
            pGunner->DefineModel  ("object_tetra_top.md3");
            pGunner->DefineTexture(0u, "effect_energy.png");
            pGunner->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pGunner->SetSize   (m_aGunner[i].GetSize   ());
            pGunner->SetColor3 (m_aGunner[i].GetColor3 ());
            pGunner->SetTexSize(m_aGunner[i].GetTexSize());
            pGunner->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_GunnerHull.BindObject(pGunner);
        }
    }

    // 
    for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i)
    {
        m_aCharger[i].DefineModelHigh("bullet_cone.md3");
        m_aCharger[i].DefineModelLow ("bullet_cone.md3");
        m_aCharger[i].DefineTexture  (0u, "effect_energy.png");
        m_aCharger[i].DefineProgram  ("effect_energy_blink_flat_invert_program");
        m_aCharger[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 2.7f);
        m_aCharger[i].SetTexSize     (coreVector2(0.5f,0.2f) * 2.0f);
        m_aCharger[i].Configure      (10, 0u, COLOR_ENERGY_ORANGE * 1.0f);
        m_aCharger[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_TOP | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST_BULLET | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT | ENEMY_STATUS_SECRET);
    }

    // 
    m_ChargerHull.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i)
        {
            // load object resources
            coreObject3D* pCharger = &m_aChargerHullRaw[i];
            pCharger->DefineModel  ("bullet_cone.md3");
            pCharger->DefineTexture(0u, "effect_energy.png");
            pCharger->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pCharger->SetSize   (m_aCharger[i].GetSize   ());
            pCharger->SetColor3 (m_aCharger[i].GetColor3 ());
            pCharger->SetTexSize(m_aCharger[i].GetTexSize());
            pCharger->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_ChargerHull.BindObject(pCharger);
        }
    }

    // 
    for(coreUintW i = 0u; i < TORUS_DRIVERS; ++i)
    {
        m_aDriver[i].DefineModelHigh("bullet_spear.md3");
        m_aDriver[i].DefineModelLow ("bullet_spear.md3");
        m_aDriver[i].DefineTexture  (0u, "effect_energy.png");
        m_aDriver[i].DefineProgram  ("effect_energy_blink_flat_invert_program");
        m_aDriver[i].SetSize        (coreVector3(2.6f,2.0f,2.6f) * 2.2f);
        m_aDriver[i].SetTexSize     (coreVector2(0.4f,0.2f) * 2.0f);
        m_aDriver[i].Configure      (10, 0u, COLOR_ENERGY_PURPLE * 1.0f);
        m_aDriver[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT | ENEMY_STATUS_SECRET);
    }

    // 
    m_DriverHull.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_DRIVERS; ++i)
        {
            // load object resources
            coreObject3D* pDriver = &m_aDriverHullRaw[i];
            pDriver->DefineModel  ("bullet_spear.md3");
            pDriver->DefineTexture(0u, "effect_energy.png");
            pDriver->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pDriver->SetSize   (m_aDriver[i].GetSize   ());
            pDriver->SetColor3 (m_aDriver[i].GetColor3 ());
            pDriver->SetTexSize(m_aDriver[i].GetTexSize());
            pDriver->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_DriverHull.BindObject(pDriver);
        }
    }

    // 
    for(coreUintW i = 0u; i < TORUS_WAVERS; ++i)
    {
        m_aWaver[i].DefineModelHigh("object_sphere.md3");
        m_aWaver[i].DefineModelLow ("object_sphere.md3");
        m_aWaver[i].DefineTexture  (0u, "effect_energy.png");
        m_aWaver[i].DefineProgram  ("effect_energy_blink_flat_invert_program");
        m_aWaver[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 3.1f);
        m_aWaver[i].SetTexSize     (coreVector2(4.5f,4.5f));
        m_aWaver[i].Configure      (10, 0u, COLOR_ENERGY_GREEN * 0.8f);
        m_aWaver[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT | ENEMY_STATUS_SECRET);
    }

    // 
    m_WaverHull.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_WAVERS; ++i)
        {
            // load object resources
            coreObject3D* pWaver = &m_aWaverHullRaw[i];
            pWaver->DefineModel  ("object_sphere.md3");
            pWaver->DefineTexture(0u, "effect_energy.png");
            pWaver->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pWaver->SetSize   (m_aWaver[i].GetSize   ());
            pWaver->SetColor3 (m_aWaver[i].GetColor3 ());
            pWaver->SetTexSize(m_aWaver[i].GetTexSize());
            pWaver->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_WaverHull.BindObject(pWaver);
        }
    }

    // 
    m_pRumbleSound = Core::Manager::Resource->Get<coreSound>("effect_rumble.wav");
    m_pWooshSound  = Core::Manager::Resource->Get<coreSound>("effect_woosh_01.wav");

    STATIC_ASSERT(sizeof(m_iTurretActive) *8u >= TORUS_TURRETS)
    STATIC_ASSERT(sizeof(m_iTurretMove)   *8u >= TORUS_TURRETS)
    STATIC_ASSERT(sizeof(m_iGunnerActive) *8u >= TORUS_GUNNERS)
    STATIC_ASSERT(sizeof(m_iChargerActive)*8u >= TORUS_CHARGERS)
    STATIC_ASSERT(sizeof(m_iDriverActive) *8u >= TORUS_DRIVERS)
    STATIC_ASSERT(sizeof(m_iWaverActive)  *8u >= TORUS_WAVERS)
}


// ****************************************************************
// destructor
cTorusBoss::~cTorusBoss()
{
    // 
    this->Kill(false);

    // 
    this->__DisableSummon(false);
    for(coreUintW i = 0u; i < TORUS_TURRETS;  ++i) this->__DisableTurret (i, false);
    for(coreUintW i = 0u; i < TORUS_GUNNERS;  ++i) this->__DisableGunner (i, false);
    for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i) this->__DisableCharger(i, false);
    for(coreUintW i = 0u; i < TORUS_DRIVERS;  ++i) this->__DisableDriver (i, false);
    for(coreUintW i = 0u; i < TORUS_WAVERS;   ++i) this->__DisableWaver  (i, false);
}


// ****************************************************************
// 
void cTorusBoss::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Emitter);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->BindObject(&m_aCircle[i]);

    // 
    g_pGlow->BindList(&m_TurretHull);
    g_pGlow->BindList(&m_GunnerHull);
    g_pGlow->BindList(&m_ChargerHull);
    g_pGlow->BindList(&m_DriverHull);
    g_pGlow->BindList(&m_WaverHull);

    // 
    constexpr coreUint8 aiNewOrder[] = {cSpearBullet::ID};
    g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));

    // 
    this->_ResurrectBoss();
}


// ****************************************************************
// 
void cTorusBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    for(coreUintW i = 0u; i < TORUS_BARRIERS; ++i) pMission->DisableBarrier(i, bAnimated);
    pMission->DisableShadow(0u, bAnimated);

    // 
    this->__DisableSummon(bAnimated);
    for(coreUintW i = 0u; i < TORUS_TURRETS;  ++i) this->__DisableTurret (i, bAnimated);
    for(coreUintW i = 0u; i < TORUS_GUNNERS;  ++i) this->__DisableGunner (i, bAnimated);
    for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i) this->__DisableCharger(i, bAnimated);
    for(coreUintW i = 0u; i < TORUS_DRIVERS;  ++i) this->__DisableDriver (i, bAnimated);
    for(coreUintW i = 0u; i < TORUS_WAVERS;   ++i) this->__DisableWaver  (i, bAnimated);

    // 
    g_pGlow->UnbindObject(&m_Emitter);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->UnbindObject(&m_aCircle[i]);

    // 
    g_pGlow->UnbindList(&m_TurretHull);
    g_pGlow->UnbindList(&m_GunnerHull);
    g_pGlow->UnbindList(&m_ChargerHull);
    g_pGlow->UnbindList(&m_DriverHull);
    g_pGlow->UnbindList(&m_WaverHull);

    // 
    if(m_pRumbleSound->EnableRef(this)) m_pRumbleSound->Stop();
    if(m_pWooshSound ->EnableRef(this)) m_pWooshSound ->Stop();

    // 
    g_pGame->GetBulletManagerEnemy()->ResetOrder();
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnBottom()
{
    if(this->HasStatus(ENEMY_STATUS_BOTTOM))
    {
        DEPTH_PUSH

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
            m_aCircle[i].Render();

        DEPTH_POP
    }
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnUnder()
{
    if(m_iTurretActive || m_iGunnerActive || m_iChargerActive || m_iDriverActive || m_iWaverActive)
    {
        DEPTH_PUSH

        glDepthMask(false);
        {
            // 
            m_TurretHull .Render();
            m_GunnerHull .Render();
            m_ChargerHull.Render();
            m_DriverHull .Render();
            m_WaverHull  .Render();
        }
        glDepthMask(true);
    }
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnOver()
{
    DEPTH_PUSH_SHIP

    // 
    m_Emitter.Render();

    if(!this->HasStatus(ENEMY_STATUS_BOTTOM))
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
            m_aCircle[i].Render();
    }

    // 
    m_Summon.Render();
}


// ****************************************************************
// 
void cTorusBoss::__MoveOwn()
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    // 
    coreVector3 vNewOri = coreVector3(0.0f, 0.0f, 1.0f);

    coreFloat fShootSpeed = 1.0f;

    // 
    m_fAnimation   .UpdateMod(-1.0f,               10.0f);
    m_fRotationBoss.UpdateMod(TORUS_BOSS_ROTATION, 2.0f*PI);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                g_pEnvironment->SetTargetSpeedNow(0.0f);

                this->AddStatus(ENEMY_STATUS_GHOST);
                this->ChangeToBottom();

                g_pSpecialEffects->ExternPlayPosition(m_pRumbleSound, this, 0.0f, 1.0f, true, SOUND_EFFECT, coreVector3(0.0f,100.0f,0.0f));
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_EFFECT_SHAKE_02);
            }

            if(PHASE_TIME_BEFORE(0.85f))
            {
                const coreFloat fNewTime = MIN(fTime, 0.7f);
                const coreVector2 vDir = coreVector2::Direction(LERPBR(0.0f*PI, 12.0f*PI, fNewTime));

                const coreFloat fLen  = LERPS(1.5f, 0.0f, MIN1(fNewTime * 1.5f)) * FOREGROUND_AREA.x;
                const coreFloat fLen2 = LERPS(5.0f, 0.0f, MIN1(fNewTime * 1.5f)) * FOREGROUND_AREA.x * LERPS(1.0f, 0.0f, CLAMP01((fNewTime - (2.0f/3.0f)) * 6.0f));

                const coreVector2 vNewPos = vDir * fLen;
                const coreVector3 vAnchor = coreVector3(vDir * fLen2, -2.0f);

                const coreVector3 vOri2 = (vAnchor - coreVector3(vNewPos, -20.0f)).Normalized();
                const coreVector2 vOffset = coreMath::IsNear(ABS(vOri2.z), 1.0f) ? coreVector2(0.0f,0.0f) : coreVector3::Cross(vOri2, coreVector3::Cross(vOri2, coreVector3(0.0f,0.0f,1.0f)).Normalized(coreVector3(0.0f,1.0f,0.0f))).xy() * LERP(this->GetVisualRange().x, 0.0f, ABS(vOri2.z));

                const coreFloat fHeight = g_pEnvironment->RetrieveSafeHeight(vNewPos + vOffset);
                m_avVector[HEIGHT_SAVE].x = LERP(m_avVector[HEIGHT_SAVE].x, fHeight, 10.0f * TIME);

                this->SetPosition(coreVector3(vNewPos, m_avVector[HEIGHT_SAVE].x + LERP(this->GetVisualRange().x - 2.0f, this->GetVisualRange().z, ABS(vOri2.z))));

                if(fNewTime < 0.57f) this->__CreateOverdrive(coreVector3(vNewPos + vOffset, m_avVector[HEIGHT_SAVE].x));

                m_fRotationBoss.UpdateMod(((fTime > 0.7f) ? 0.0f : -LERPB(5.0f, 8.3f, fNewTime)) - TORUS_BOSS_ROTATION, 2.0f*PI);
                vNewOri = -vOri2;
            }

            if(m_pRumbleSound->EnableRef(this))
            {
                g_pSpecialEffects->ExternSetSource(m_pRumbleSound, this->GetPosition());
                m_pRumbleSound->SetVolume(MAX0(SIN(STEP(0.0f, 0.57f, fTime) * (1.0f*PI))) * TORUS_RUMBLE_VOLUME);
            }

            if(PHASE_TIME_POINT(0.57f))
            {
                if(m_pRumbleSound->EnableRef(this)) m_pRumbleSound->Stop();
            }

            if(PHASE_TIME_POINT(0.85f))
            {
                this->_StartBoss();

                this->__ChangeColor(6u);

                m_fRotationBoss = this->GetDirection().xy().InvertedY().Angle();
                vNewOri = coreVector3(0.0f,0.0f,1.0f);

                m_avVector[HEIGHT_SAVE].x = this->GetPosition().z;

                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                g_pSpecialEffects->ExternPlayPosition(m_pWooshSound, this, 0.0f, 1.0f, true, SOUND_EFFECT, coreVector3(0.0f,0.0f,0.0f));
            }

            if(PHASE_TIME_POINT(0.5f))
            {
                this->RemoveStatus(ENEMY_STATUS_GHOST);
                this->ChangeToNormal();
            }

            this->SetPosition(coreVector3(this->GetPosition().xy(), LERPS(m_avVector[HEIGHT_SAVE].x, 0.0f, fTime)));

            m_fRotationBoss.UpdateMod(-TORUS_BOSS_ROTATION * (1.0f - fTime), 2.0f*PI);
            vNewOri = coreVector3(0.0f, coreVector2::Direction(LERPS(0.0f*PI, 4.0f*PI, fTime)));

            g_pEnvironment->SetTargetSpeedNow(LERPH3(0.0f, 6.0f, fTime));

            if(m_pWooshSound->EnableRef(this))
            {
                const coreFloat fValue = MAX0(SIN(fTime * (1.0f*PI)));

                m_pWooshSound->SetVolume(fValue * 0.5f);
                m_pWooshSound->SetPitch (fValue * 0.8f + 1.0f);
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                if(m_pWooshSound->EnableRef(this)) m_pWooshSound->Stop();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(110u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK_REV)
        {
            m_fRotationBoss.UpdateMod(-TORUS_BOSS_ROTATION, 2.0f*PI);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_avVector[GRIND_VALUE].x = cTorusBoss::__PositionToGrind(this->GetPosition().xy());
                m_avVector[GRIND_VALUE].y = m_avVector[GRIND_VALUE].x;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                g_pSpecialEffects->ExternPlayPosition(m_pRumbleSound, this, 0.0f, 1.0f, true, SOUND_EFFECT, this->GetPosition());
            }

            m_avVector[GRIND_VALUE].x += 0.5f * TIME * fTime;

            this->SetPosition(coreVector3(cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * FOREGROUND_AREA * 0.9f, 0.0f));

            m_fRotationBoss.UpdateMod(-3.0f * fTime - TORUS_BOSS_ROTATION, 2.0f*PI);

            if(m_pRumbleSound->EnableRef(this))
            {
                g_pSpecialEffects->ExternSetSource(m_pRumbleSound, this->GetPosition());
                m_pRumbleSound->SetVolume(fTime * TORUS_RUMBLE_VOLUME);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // rolls around the border
    else if(m_iPhase == 82u)
    {
        const coreFloat fPrevGrind = m_avVector[GRIND_VALUE].x;
        m_avVector[GRIND_VALUE].x += 0.5f * TIME;

        this->SetPosition(coreVector3(cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * FOREGROUND_AREA * 0.9f, 0.0f));

        m_fRotationBoss.UpdateMod(-3.0f - TORUS_BOSS_ROTATION, 2.0f*PI);   // -4.5f is realistic

        if(m_pRumbleSound->EnableRef(this))
        {
            g_pSpecialEffects->ExternSetSource(m_pRumbleSound, this->GetPosition());
        }

        if(this->GetCurHealth() >= 4200)
        {
            g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
            {
                if(m_iPhase != 82u) return;

                if(F_TO_UI(m_avVector[GRIND_VALUE].x) <= F_TO_UI(m_avVector[GRIND_VALUE].y)) return;

                const coreBool X = PHASE_FLYPAST(this, pPlayer, x) && InBetween(pPlayer->GetPosition().x, FOREGROUND_AREA.x * -0.7f, FOREGROUND_AREA.x * 0.7f);
                const coreBool Y = PHASE_FLYPAST(this, pPlayer, y) && InBetween(pPlayer->GetPosition().y, FOREGROUND_AREA.y * -0.7f, FOREGROUND_AREA.y * 0.7f);

                if(X || Y)
                {
                    PHASE_CHANGE_INC
                    PHASE_RESET(0u)

                    m_avVector[GRIND_VALUE].xy(coreVector2(1.0f,1.0f) * cTorusBoss::__PositionToGrind(this->GetPosition().xy() * (X ? coreVector2(1.0f,-1.0f) : coreVector2(-1.0f,1.0f))));

                    m_aiCounter[CRASH_COUNT] += 1;

                    g_pSpecialEffects->CreateSplashColor(this->GetPosition() + coreVector3(8.0f * AlongCrossNormal(this->GetPosition().xy()), 0.0f), 25.0f, 10u, COLOR_ENERGY_WHITE * 0.8f);
                    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.4f, 1.0f, SOUND_EFFECT_DUST);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                    if(m_pRumbleSound->EnableRef(this)) m_pRumbleSound->Stop();
                }
            });
        }
        else
        {
            if(InBetweenExt(0.5f, FRACT(fPrevGrind), FRACT(m_avVector[GRIND_VALUE].x)) == 1)
            {
                PHASE_CHANGE_TO(90u)

                this->__ChangeColor(7u);

                m_aiCounter[TURRET_DIR] = F_TO_SI(m_avVector[GRIND_VALUE].x) % 2;

                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.4f, 1.0f, SOUND_EFFECT_DUST);

                if(m_pRumbleSound->EnableRef(this)) m_pRumbleSound->Stop();
            }
        }
    }

    // ################################################################
    // jumps to the other side
    else if(m_iPhase == 83u)
    {
        PHASE_CONTROL_TIMER(0u, g_pGame->IsEasy() ? 0.8f : 1.0f, LERP_BREAK_REV)
        {
            const coreVector2 vTarget = cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * 0.9f;

            this->DefaultMoveLerp(m_vLastPosition, vTarget, fTime);

            if(PHASE_TIME_POINT(0.1f)) this->AddStatus(ENEMY_STATUS_DAMAGING);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(82u)

                this->RemoveStatus(ENEMY_STATUS_DAMAGING);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);

                g_pSpecialEffects->ExternPlayPosition(m_pRumbleSound, this, TORUS_RUMBLE_VOLUME, 1.0f, true, SOUND_EFFECT, this->GetPosition());

                this->__EnableDriver(MIN(coreMath::BitScanFwd(~m_iDriverActive), TORUS_DRIVERS - 1u), this->GetPosition().xy(), -this->GetPosition().xy().Normalized());
                m_aiCounter[DRIVER_COUNT] += 1;

                if(m_aiCounter[DRIVER_COUNT] == 3u)
                {
                    if(this->_ResurrectHelper(ELEMENT_PURPLE, true))
                    {
                        g_pGame->GetHelper(ELEMENT_PURPLE)->SetPosition(coreVector3(0.0f,0.0f,0.0f));

                        g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(0.0f,0.0f,0.0f), COLOR_ENERGY_PURPLE);
                        g_pSpecialEffects->PlaySound(coreVector3(0.0f,0.0f,0.0f), 1.0f, 1.2f, SOUND_ENEMY_EXPLOSION_05);
                    }
                }
            }
        });
    }

    // ################################################################
    // jumps in pattern 1
    else if(m_iPhase == 90u)
    {
        const coreFloat fPrevGrind = m_avVector[GRIND_VALUE].x;
        m_avVector[GRIND_VALUE].x += 0.55f * SQRT2 * TIME;

        this->SetPosition(coreVector3(cTorusBoss::__GrindToPosition2(m_avVector[GRIND_VALUE].x) * FOREGROUND_AREA * 0.9f, 0.0f));

        if(InBetweenExt(0.5f, FRACT(fPrevGrind), FRACT(m_avVector[GRIND_VALUE].x)) == 1)
        {
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

            if(F_TO_SI(m_avVector[GRIND_VALUE].x) % 2 == m_aiCounter[TURRET_DIR])
            {
                this->__EnableTurret((F_TO_UI(m_avVector[GRIND_VALUE].x * 0.5f) + m_aiCounter[TURRET_DIR]) % TORUS_TURRETS, this->GetPosition().xy());
                m_aiCounter[TURRET_COUNT] += 1;
            }
            else if(m_aiCounter[TURRET_COUNT] >= 2)
            {
                if(this->_ResurrectHelper(ELEMENT_CYAN, true))
                {
                    g_pGame->GetHelper(ELEMENT_CYAN)->SetPosition(this->GetPosition());
                    m_avVector[HELPER_DATA].xy(AlongCrossNormal(this->GetPosition().xy().Rotated90()));

                    g_pSpecialEffects->MacroExplosionColorSmall(this->GetPosition(), COLOR_ENERGY_CYAN);
                    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.2f, SOUND_ENEMY_EXPLOSION_05);
                }
            }

            if(this->GetCurHealth() < 3500)
            {
                PHASE_CHANGE_INC

                this->__ChangeColor(3u);

                m_avVector[GRIND_VALUE].z = FMOD (m_avVector[GRIND_VALUE].x, 4.0f);
                m_avVector[GRIND_VALUE].x = FRACT(m_avVector[GRIND_VALUE].x);
            }
        }
    }

    // ################################################################
    // jumps in pattern 2
    else if(m_iPhase == 91u)
    {
        const coreFloat fPrevGrind = m_avVector[GRIND_VALUE].x;
        m_avVector[GRIND_VALUE].x += 0.55f * (SQRT(2.0f) / SQRT(10.0f)) * SQRT2 * TIME;

        this->SetPosition(coreVector3(MapStepRotatedInv90(cTorusBoss::__GrindToPosition3(m_avVector[GRIND_VALUE].x), F_TO_UI(m_avVector[GRIND_VALUE].z)) * FOREGROUND_AREA * 0.9f, 0.0f));

        if((InBetweenExt(0.5f + (2.0f/3.0f), FMOD(fPrevGrind, 2.0f), FMOD(m_avVector[GRIND_VALUE].x, 2.0f)) == 1) ||
           (InBetweenExt(1.5f + (1.0f/3.0f), FMOD(fPrevGrind, 2.0f), FMOD(m_avVector[GRIND_VALUE].x, 2.0f)) == 1))
        {
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

            this->__EnableGunner(F_TO_UI(m_avVector[GRIND_VALUE].x + 0.5f) % TORUS_GUNNERS, this->GetPosition().xy());
        }
        else if(InBetweenExt(0.5f, FMOD(fPrevGrind, 2.0f), FMOD(m_avVector[GRIND_VALUE].x, 2.0f)) == 1)
        {
            this->_ResurrectHelper(ELEMENT_RED, false);
        }

        if(InBetweenExt(0.5f, FRACT(fPrevGrind), FRACT(m_avVector[GRIND_VALUE].x)) == 1)
        {
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

            if(this->GetCurHealth() < 2800)
            {
                PHASE_CHANGE_TO(100u)

                this->__ChangeColor(1u);

                m_avVector[GRIND_VALUE].x = cTorusBoss::__PositionToGrind(this->GetPosition().xy());
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 100u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                constexpr coreFloat fRound = ((2.0f*PI) / 3.0f);

                m_avVector[ROTATE_SAVE].x = m_fRotationBoss;
                m_avVector[ROTATE_SAVE].y = TRUNC(m_fRotationBoss / fRound) * fRound;

                m_aiCounter[EMIT_STATUS] = 0;
            }

            const coreVector2 vTarget = cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * 0.8f;

            this->DefaultMoveLerp(m_vLastPosition, vTarget, fTime);

            m_fRotationBoss = LERP(m_avVector[ROTATE_SAVE].x, m_avVector[ROTATE_SAVE].y, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 101u)
    {
        PHASE_CONTROL_TIMER(0u, 2.0f, LERP_LINEAR)
        {
            m_fRotationBoss.UpdateMod(LERP(-1.0f, 0.0f, fTime) * TORUS_BOSS_ROTATION, 2.0f*PI);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 102u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_BREAK_REV)
        {
            const coreVector2 vDir = coreVector2::Direction(fTime * (8.0f*PI) + m_vLastPosition.Angle());
            const coreFloat   fLen = LERP(0.8f, 0.0f, fTime);

            this->SetPosition(coreVector3(vDir * fLen * FOREGROUND_AREA, 0.0f));

            m_fRotationBoss.UpdateMod(LERP(0.0f, 5.0f, fTime), 2.0f*PI);

            fShootSpeed = LERP(1.0f, 2.8f, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                for(coreUintW i = 0u; i < TORUS_BARRIERS; ++i)
                {
                    pMission->EnableBarrier(i, VIRIDO_BARRIER_FREE, coreVector2(0.0f,1.0f), 0.0f);
                }

                pMission->SetBarrierSlow (true);
                pMission->SetBarrierClamp(false);
                pMission->SetBarrierRange(0.9f);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 103u)
    {
        m_fRotationBoss.UpdateMod(5.0f, 2.0f*PI);

        fShootSpeed = 2.8f;

        if(PHASE_MAINTIME_POINT(3.0f))
        {
            this->_ResurrectHelper(ELEMENT_YELLOW, false);
        }

        if(this->GetCurHealth() < 1400)
        {
            PHASE_CHANGE_TO(140u)

            this->__ChangeColor(4u);

            for(coreUintW i = 0u; i < TORUS_BARRIERS; ++i)
            {
                pMission->DisableBarrier(i, true);
            }

            pMission->SetBarrierSlow (false);
            pMission->SetBarrierClamp(true);
            pMission->SetBarrierRange(1.0f);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 110u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_SMOOTH)
        {
            coreVector2 vTarget;
            switch(m_aiCounter[CUR_TARGET] % 8)
            {
            default: UNREACHABLE
            case 0: vTarget = coreVector2( 0.0f, 1.0f); break;
            case 1: vTarget = coreVector2( 1.0f,-1.0f); break;
            case 2: vTarget = coreVector2(-1.0f, 0.0f); break;
            case 3: vTarget = coreVector2( 1.0f, 1.0f); break;
            case 4: vTarget = coreVector2( 0.0f,-1.0f); break;
            case 5: vTarget = coreVector2(-1.0f, 1.0f); break;
            case 6: vTarget = coreVector2( 1.0f, 0.0f); break;
            case 7: vTarget = coreVector2(-1.0f,-1.0f); break;
            }

            this->DefaultMoveLerp(m_vLastPosition, vTarget * 0.7f, fTime);

            if(!m_vLastPosition.IsNull()) g_pEnvironment->SetTargetDirectionNow(AngleLerpDir(m_vLastPosition, vTarget, fTime));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[CUR_TARGET] += 1;

                if(m_aiCounter[CUR_TARGET] == 2u) this->_ResurrectHelper(ELEMENT_BLUE, false);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 111u)
    {
        PHASE_CONTROL_TICKER(0u, 36u, 12.0f, LERP_LINEAR)
        {
            const coreFloat fAngle = I_TO_F(iTick) * 12.0f;

            for(coreUintW i = 4u; i--; )
            {
                if(g_pGame->IsEasy() && (i % 2u)) continue;

                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 90.0f + fAngle));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, this, vPos, vDir)->ChangeSize(1.7f);
            }

            g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 5.0f, 1u, COLOR_ENERGY_BLUE);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);

            if(PHASE_FINISHED)
            {
                if(this->GetCurHealth() < 6300)
                {
                    PHASE_CHANGE_TO(120u)

                    m_avVector[JUMP_TARGET].xy(coreVector2(0.0f,0.0f));

                    this->__ChangeColor(8u);
                }
                else
                {
                    PHASE_CHANGE_TO(110u)
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 120u)
    {
        if(PHASE_BEGINNING2)
        {
            m_avVector[TUMBLE_DIRECTION].z = g_pEnvironment->GetDirection().Angle();
        }

        PHASE_CONTROL_TIMER(1u, 0.5f, LERP_SMOOTH)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(m_avVector[TUMBLE_DIRECTION].z, 0.0f*PI, fTime)));
        });

        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                pMission->EnableShadow(0u, this, m_avVector[JUMP_TARGET].xy(), false);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_WOOSH_02);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                g_pSpecialEffects->ExternPlayPosition(m_pWooshSound, this, 0.0f, 1.5f, true, SOUND_EFFECT, this->GetPosition());
            }

            const coreFloat   fSin    = SIN(fTime * (1.0f*PI));
            const coreVector2 vPos    = LERP(m_vLastPosition * FOREGROUND_AREA, m_avVector[JUMP_TARGET].xy(), fTime) * (1.0f - 0.45f * fSin);
            const coreFloat   fHeight = 50.0f * fSin;

            this->SetPosition(coreVector3(vPos, fHeight));

            vNewOri.yz(coreVector2::Direction((6.0f*PI) * fTime));

            if(m_pWooshSound->EnableRef(this))
            {
                g_pSpecialEffects->ExternSetSource(m_pWooshSound, this->GetPosition());
                m_pWooshSound->SetVolume(MAX0(fSin) * 0.8f);
            }

            if(PHASE_FINISHED)
            {
                if(m_avVector[JUMP_TARGET].z < 0.0f)
                {
                    PHASE_CHANGE_TO(130u)

                    this->__ChangeColor(2u);
                }
                else
                {
                    PHASE_CHANGE_INC
                }

                pMission->DisableShadow(0u, true);

                this->__EnableWaver(MIN(coreMath::BitScanFwd(~m_iWaverActive), TORUS_WAVERS - 1u), this->GetPosition().xy());

                if(m_pWooshSound->EnableRef(this)) m_pWooshSound->Stop();

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 121u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            m_avVector[JUMP_TARGET].xy(this->NearestPlayerDual(1u)->GetPosition().xy());

            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 122u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, m_avVector[JUMP_TARGET].xy() / FOREGROUND_AREA, fTime);

            if(PHASE_FINISHED)
            {
                if(this->GetCurHealth() < 5600)
                {
                    m_avVector[JUMP_TARGET].xy(coreVector2(0.0f,0.0f));
                    m_avVector[JUMP_TARGET].z = -1.0f;
                }
                else
                {
                    m_avVector[JUMP_TARGET].xy(this->NearestPlayerDual(0u)->GetPosition().xy());
                    m_avVector[JUMP_TARGET].z = 1.0f - m_avVector[JUMP_TARGET].z;
                }

                if(this->_ResurrectHelper(ELEMENT_GREEN, true))
                {
                    g_pGame->GetHelper(ELEMENT_GREEN)->SetPosition(this->GetPosition());

                    g_pSpecialEffects->MacroExplosionColorSmall(this->GetPosition(), COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.2f, SOUND_ENEMY_EXPLOSION_05);
                }

                PHASE_CHANGE_TO(120u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 130u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,0.0f), 10.0f);
        });

        m_aiCounter[EMIT_STATUS] = 0;
    }

    // ################################################################
    // 
    else if(m_iPhase == 131u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(-1.5f, m_vLastPosition.y), fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->SetPosition(coreVector3(this->GetPosition().x, 0.0f * FOREGROUND_AREA.y, this->GetPosition().z));

                this->StorePosition();
            }
        });

        m_aiCounter[EMIT_STATUS] = 0;
    }

    // ################################################################
    // 
    else if(m_iPhase == 132u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 133u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            this->DefaultMoveLerp(coreVector2(1.5f, m_vLastPosition.y), coreVector2(-1.5f, m_vLastPosition.y), fTime);

            const coreFloat fRota = (this->GetPosition().x - -0.9f * FOREGROUND_AREA.x) * RCP(0.9f * FOREGROUND_AREA.x - -0.9f * FOREGROUND_AREA.x);

            vNewOri = coreVector3(0.0f,-1.0f,0.0f);
            vNewOri = coreVector3(0.0f, coreVector2::Direction(LERP(0.0f*PI, 1.0f*PI, fRota) + I_TO_F((m_aiCounter[SLIDE_COUNT] + 1) % 2) * (0.5f*PI)));

            if(!this->WasTeleporting())
            {
                if(PHASE_POSITION_POINT(this, 0.7f * FOREGROUND_AREA.x, x))
                {
                    this->__EnableCharger(MIN(coreMath::BitScanFwd(~m_iChargerActive), TORUS_CHARGERS - 1u), this->GetPosition().xy(), this->AimAtPlayerDual(m_aiCounter[SLIDE_COUNT] % 2).Normalized());
                }

                if(PHASE_POSITION_POINT(this, -0.7f * FOREGROUND_AREA.x, x))
                {
                    if(m_aiCounter[SLIDE_COUNT] == 2)
                    {
                        if(this->_ResurrectHelper(ELEMENT_ORANGE, true))
                        {
                            g_pGame->GetHelper(ELEMENT_ORANGE)->SetPosition(this->GetPosition());
                            m_avVector[HELPER_DATA].xy(this->AimAtPlayerDual(0u).Normalized());

                            g_pSpecialEffects->MacroExplosionColorSmall(this->GetPosition(), COLOR_ENERGY_ORANGE);
                            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.2f, SOUND_ENEMY_EXPLOSION_05);
                        }
                    }
                }
            }

            if(PHASE_FINISHED || ((m_aiCounter[SLIDE_COUNT] == 3) && (this->GetPosition().x <= -0.9f * FOREGROUND_AREA.x)))
            {
                m_aiCounter[SLIDE_COUNT] += 1;

                if(m_aiCounter[SLIDE_COUNT] >= 4)
                {
                    PHASE_CHANGE_INC

                    this->SetPosition(coreVector3(-0.9f * FOREGROUND_AREA.x, 0.0f, this->GetPosition().z));

                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
                }
                else
                {
                    PHASE_CHANGE_TO(132u)

                    coreFloat fSide;
                    switch(m_aiCounter[SLIDE_COUNT])
                    {
                    default: UNREACHABLE
                    case 1: fSide =  0.4f; break;
                    case 2: fSide = -0.4f; break;
                    case 3: fSide =  0.0f; break;
                    }

                    this->SetPosition(coreVector3(this->GetPosition().x, fSide * FOREGROUND_AREA.y, this->GetPosition().z));

                    this->StorePosition();
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 134u)
    {
        PHASE_CHANGE_TO(80u)

        this->__ChangeColor(5u);
    }

    // ################################################################
    // 
    else if(m_iPhase == 140u)
    {
        const cPlayer* pPlayer1 = g_pGame->GetPlayer(0u);
        const cPlayer* pPlayer2 = g_pGame->GetPlayer(1u);
        STATIC_ASSERT(GAME_PLAYERS == 2u)

             if(pPlayer1->HasStatus(PLAYER_STATUS_DEAD)) m_avVector[MIRROR_LERP].y = 1.0f;
        else if(pPlayer2->HasStatus(PLAYER_STATUS_DEAD)) m_avVector[MIRROR_LERP].y = 0.0f;

        if(m_avVector[MIRROR_LERP].y) m_avVector[MIRROR_LERP].x = MIN1(m_avVector[MIRROR_LERP].x + 1.0f*TIME);
                                 else m_avVector[MIRROR_LERP].x = MAX0(m_avVector[MIRROR_LERP].x - 1.0f*TIME);

        const coreVector2 vBasePos = LERPS(pPlayer1->GetPosition().xy(), pPlayer2->GetPosition().xy(), m_avVector[MIRROR_LERP].x);
        const coreVector2 vNewPos = vBasePos * -0.85f * BLENDH3(MIN1(m_fPhaseTime * 0.35f));

        this->SetPosition(coreVector3(vNewPos, 0.0f));

        fShootSpeed = LERP(4.0f, 2.8f, STEP(0.0f, 1400.0f, I_TO_F(this->GetCurHealth())));

        m_fRotationBoss.UpdateMod(5.0f * fShootSpeed / 2.8f, 2.0f*PI);

        if(!vNewPos.IsNull()) g_pEnvironment->SetTargetDirection(vNewPos.Normalized(), 4.0f * BLENDH3(MIN1(m_fPhaseTime * 0.1f)));

        if(PHASE_MAINTIME_POINT(3.0f))
        {
            this->_ResurrectHelper(ELEMENT_MAGENTA, true);
        }

        PHASE_CONTROL_TICKER(0u, 0u, 12.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = this->AimAtPlayerDual(1u - F_TO_UI(m_avVector[MIRROR_LERP].y)).Normalized();
            const coreVector2 vTan = vDir.Rotated90() * ((iTick % 2u) ? -1.0f : 1.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, this, vPos, (vDir + vTan * 0.01f).Normalized())->ChangeSize(1.8f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_MAGENTA);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 150u)
    {
        if(PHASE_BEGINNING2)
        {
            this->AddStatus(ENEMY_STATUS_GHOST);
            this->ChangeToBottom();

            this->_EndBoss();

            this->__ChangeColor(0u);

            for(coreUintW i = 0u; i < TORUS_TURRETS;  ++i) this->__DisableTurret (i, true);
            for(coreUintW i = 0u; i < TORUS_GUNNERS;  ++i) this->__DisableGunner (i, true);
            for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i) this->__DisableCharger(i, true);
            for(coreUintW i = 0u; i < TORUS_DRIVERS;  ++i) this->__DisableDriver (i, true);
            for(coreUintW i = 0u; i < TORUS_WAVERS;   ++i) this->__DisableWaver  (i, true);

            g_pSpecialEffects->MacroExplosionPhysicalDarkBig(this->GetPosition());
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);

            //const coreBool bFlip = true;
            //m_avVector[TUMBLE_DIRECTION].xy((bFlip ? coreVector2(0.5f,-1.0f).Normalized() : (this->GetMove()) * 1.5f));
            m_avVector[TUMBLE_DIRECTION].z = g_pEnvironment->GetDirection().Angle();
        }

        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            vNewOri.yz(coreVector2::Direction((6.0f*PI) * fTime));

            this->DefaultMoveLerp(m_vLastPosition, m_vLastPosition + m_avVector[TUMBLE_DIRECTION].xy(), fTime);
            //g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(m_avVector[TUMBLE_DIRECTION].z, 0.0f*PI, fTime)));   end rotation

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 151u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_BREAK_REV)
        {
            vNewOri.yz(coreVector2::Direction((-2.0f*PI) * fTime));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pSpecialEffects->MacroDestructionDark(this);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 152u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fHeight = LERPBR(0.0f, WATER_HEIGHT, fTime);

            this->SetPosition(coreVector3(m_vLastPosition * FOREGROUND_AREA * LERP(1.0f, 0.5f, fTime), fHeight));

            vNewOri.yz(coreVector2::Direction((-6.0f*PI) * fTime));

            if(PHASE_FINISHED)
            {
                this->Kill(true);

                if(this->HasAllHelpers())
                {
                    this->_CreateFragment(1u);
                }

                g_pSpecialEffects->CreateExplosion (this->GetPosition());
                g_pSpecialEffects->CreateSplashDark(this->GetPosition(), 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.2f, 0.6f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->SlowScreen(4.0f);

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "effect_soot.png");
                pObject->DefineProgram("effect_decal_single_program");

                // set object properties
                pObject->SetSize     (coreVector3(20.0f,20.0f,1.0f));
                pObject->SetDirection(coreVector3(0.0f,1.0f,0.0f));
                pObject->SetColor3   (coreVector3(0.0f,0.0f,0.0f));
                
                const coreVector3 vDecalPos = coreVector3(this->GetPosition().xy(), g_pEnvironment->RetrieveSafeHeight(this->GetPosition().xy()));

                // 
                g_pEnvironment->GetBackground()->AddDecal(pObject, vDecalPos, 1u, "effect_decal_single_inst_program", LIST_KEY);
            }
        });

        PHASE_CONTROL_TIMER(1u, 0.5f, LERP_LINEAR)
        {
            const coreVector3 vNewColor = COLOR_ENERGY_WHITE * LERP(0.3f, 0.05f, 0.5f + 0.5f * SIN(fTime * (10.0f*PI)));

            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
                m_aCircle[i].SetColor3(vNewColor);
        });
    }

    // ################################################################
    // ################################################################

    if(pMission->GetBarrier(0u)->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_avVector[BARRIER_ANGLE].x += 0.8f * TIME;

        for(coreUintW i = 0u; i < TORUS_BARRIERS; ++i)
        {
            coreObject3D* pBarrier = pMission->GetBarrier(i);

            const coreFloat   fTime  = FRACT(MAX0(m_avVector[BARRIER_ANGLE].x * 0.2f - I_TO_F(i / 2u) * 0.25f));
            const coreFloat   fLen   = fTime * 1.1f * SQRT2 * FOREGROUND_AREA.x;
            const coreFloat   fScale = fTime * 5.0f;
            const coreVector2 vDir   = coreVector2::Direction(m_avVector[BARRIER_ANGLE].x * (0.1f*PI) * (((i % 4u) < 2u) ? -1.0f : 1.0f)) * ((i % 2u) ? -1.0f : 1.0f);
            const coreVector2 vPos   = fScale ? (vDir * fLen) : HIDDEN_POS;

            pBarrier->SetPosition (coreVector3(vPos, 0.0f));
            pBarrier->SetDirection(coreVector3(vDir, 0.0f));

            pMission->SetBarrierScale(i, fScale);
        }
    }

    // 
    if(m_iPhase == 0u)
    {
        vNewOri = -vNewOri;

        if(!coreMath::IsNear(ABS(vNewOri.z), 1.0f))
        {
            const coreMatrix3 mRota   = coreMatrix4::RotationAxis(m_fRotationBoss, vNewOri).m123();
            const coreVector3 vNewDir = coreVector3::Cross(vNewOri, coreVector3::Cross(vNewOri, coreVector3(0.0f,0.0f,-1.0f)).Normalized(coreVector3(0.0f,1.0f,0.0f)));
            this->SetDirection((vNewDir * mRota).Normalized());
        }
        this->SetOrientation(vNewOri);
    }
    else
    {
        const coreVector2 vNewDir = coreVector2::Direction(m_fRotationBoss);
        this->SetDirection  (coreVector3(vNewDir.x, -vNewOri.z*vNewDir.y, vNewOri.y*vNewDir.y));
        this->SetOrientation(coreVector3(0.0f,       vNewOri.y,           vNewOri.z));
    }

    if(m_iTurretActive || m_iGunnerActive || m_iChargerActive || m_iDriverActive || m_iWaverActive)
    {
        // 
        m_fRotationObject.UpdateMod(-0.2f, 10.0f);

        // 
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(m_fRotationObject *   (2.0f*PI)), 0.0f);
        const coreVector3 vDir2 = coreVector3(coreVector2::Direction(m_fRotationObject * (-10.0f*PI)), 0.0f);
        const coreVector2 vTex  = (coreVector2(0.2f,1.0f) * m_fRotationObject).Processed(FRACT);
        const coreFloat   fTime = FMOD(m_fRotationObject * -10.0f, 2.0f);

        // 
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            cCustomEnemy* pTurret = &m_aTurret       [i];
            coreObject3D* pHull   = &m_aTurretHullRaw[i];
            if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreVector3 vSize = pTurret->GetSize();

            // 
            const coreFloat   fMoveSide = HAS_BIT(m_iTurretMove, i) ? -1.0f : 1.0f;
            const coreVector2 vMoveDir  = coreVector2(fMoveSide * FOREGROUND_AREA.x * 0.5f * TIME, 0.0f);
            coreVector2       vNewPos   = pTurret->GetPosition().xy() + (m_aiCounter[TURRET_DIR] ? vMoveDir.yx() : vMoveDir);

            // 
            const auto nBounceEffect = [](const coreVector2 vEffectPos)
            {
                g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
            };

            // 
                 if((vNewPos.x < -FOREGROUND_AREA.x) && (fMoveSide < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x); REMOVE_BIT(m_iTurretMove, i) nBounceEffect(coreVector2(-FOREGROUND_AREA.x - vSize.x, vNewPos.y));}
            else if((vNewPos.x >  FOREGROUND_AREA.x) && (fMoveSide > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x); ADD_BIT   (m_iTurretMove, i) nBounceEffect(coreVector2( FOREGROUND_AREA.x + vSize.x, vNewPos.y));}
                 if((vNewPos.y < -FOREGROUND_AREA.y) && (fMoveSide < 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y); REMOVE_BIT(m_iTurretMove, i) nBounceEffect(coreVector2(vNewPos.x, -FOREGROUND_AREA.x - vSize.y));}
            else if((vNewPos.y >  FOREGROUND_AREA.y) && (fMoveSide > 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y - FOREGROUND_AREA.y); ADD_BIT   (m_iTurretMove, i) nBounceEffect(coreVector2(vNewPos.x,  FOREGROUND_AREA.x + vSize.y));}

            // 
            pTurret->SetPosition (coreVector3(vNewPos, 0.0f));
            pTurret->SetDirection(vDir);
            pTurret->SetTexOffset(vTex);

            // 
            pHull->SetPosition   (pTurret->GetPosition   ());
            pHull->SetSize       (pTurret->GetSize       () * (1.0f + 0.3f*fTime));
            pHull->SetDirection  (pTurret->GetDirection  ());
            pHull->SetOrientation(pTurret->GetOrientation());
            pHull->SetAlpha      (pTurret->GetAlpha      () * (1.0f - 0.5f*fTime));
            pHull->SetTexOffset  (pTurret->GetTexOffset  ());

            // 
            if(pTurret->ReachedDeath()) this->__DisableTurret(i, true);
        }

        // 
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        {
            cCustomEnemy* pGunner = &m_aGunner       [i];
            coreObject3D* pHull   = &m_aGunnerHullRaw[i];
            if(!pGunner->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            // 
            pGunner->SetDirection(vDir);
            pGunner->SetTexOffset(vTex);

            // 
            pHull->SetPosition   (pGunner->GetPosition   ());
            pHull->SetSize       (pGunner->GetSize       () * (1.0f + 0.3f*fTime));
            pHull->SetDirection  (pGunner->GetDirection  ());
            pHull->SetOrientation(pGunner->GetOrientation());
            pHull->SetAlpha      (pGunner->GetAlpha      () * (1.0f - 0.5f*fTime));
            pHull->SetTexOffset  (pGunner->GetTexOffset  ());

            // 
            if(pGunner->ReachedDeath()) this->__DisableGunner(i, true);
        }

        // 
        for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i)
        {
            cCustomEnemy* pCharger = &m_aCharger       [i];
            coreObject3D* pHull    = &m_aChargerHullRaw[i];
            if(!pCharger->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            // 
            const coreFloat   fSpeed  = pCharger->GetLifeTime() * 80.0f;
            const coreVector2 vNewPos = pCharger->GetPosition().xy() + pCharger->GetDirection().xy() * ((20.0f + fSpeed) * TIME);

            // 
            pCharger->SetPosition (coreVector3(vNewPos, 0.0f));
            pCharger->SetTexOffset(vTex);

            // 
            pHull->SetPosition   (pCharger->GetPosition   ());
            pHull->SetSize       (pCharger->GetSize       () * (1.0f + 0.3f*fTime));
            pHull->SetDirection  (pCharger->GetDirection  ());
            pHull->SetOrientation(pCharger->GetOrientation());
            pHull->SetAlpha      (pCharger->GetAlpha      () * (1.0f - 0.5f*fTime));
            pHull->SetTexOffset  (pCharger->GetTexOffset  ());

            // 
            if(!g_pForeground->IsVisiblePoint(pCharger->GetPosition().xy()))
            {
                this->__DisableCharger(i, true);
            }
        }

        // 
        for(coreUintW i = 0u; i < TORUS_DRIVERS; ++i)
        {
            cCustomEnemy* pDriver = &m_aDriver       [i];
            coreObject3D* pHull   = &m_aDriverHullRaw[i];
            if(!pDriver->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            // 
            const coreVector2 vLaserDir = pMission->GetLaser(i)->GetDirection().xy();
            coreVector2       vNewPos   = pDriver->GetPosition().xy() - vLaserDir * (20.0f * TIME);

            // 
            if(((vNewPos.x < -FOREGROUND_AREA.x * 1.2f) && (-vLaserDir.x < 0.0f)) ||
               ((vNewPos.x >  FOREGROUND_AREA.x * 1.2f) && (-vLaserDir.x > 0.0f)) ||
               ((vNewPos.y < -FOREGROUND_AREA.y * 1.2f) && (-vLaserDir.y < 0.0f)) ||
               ((vNewPos.y >  FOREGROUND_AREA.y * 1.2f) && (-vLaserDir.y > 0.0f)))
            {
                vNewPos += vLaserDir * MAX0(g_pForeground->RayIntersection(vNewPos, vLaserDir, 1.2f));
            }

            // 
            pDriver->SetPosition (coreVector3(vNewPos, 0.0f));
            pDriver->SetDirection(vDir2);
            pDriver->SetTexOffset(vTex);

            // 
            pHull->SetPosition   (pDriver->GetPosition   ());
            pHull->SetSize       (pDriver->GetSize       () * (1.0f + 0.3f*fTime));
            pHull->SetDirection  (pDriver->GetDirection  ());
            pHull->SetOrientation(pDriver->GetOrientation());
            pHull->SetAlpha      (pDriver->GetAlpha      () * (1.0f - 0.5f*fTime));
            pHull->SetTexOffset  (pDriver->GetTexOffset  ());

            // 
            if(pDriver->ReachedDeath()) this->__DisableDriver(i, true);
        }

        // 
        for(coreUintW i = 0u; i < TORUS_WAVERS; ++i)
        {
            cCustomEnemy* pWaver = &m_aWaver       [i];
            coreObject3D* pHull  = &m_aWaverHullRaw[i];
            if(!pWaver->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            // 
            pWaver->SetTexOffset(vTex);

            // 
            pHull->SetPosition   (pWaver->GetPosition   ());
            pHull->SetSize       (pWaver->GetSize       () * (1.0f + 0.3f*fTime));
            pHull->SetDirection  (pWaver->GetDirection  ());
            pHull->SetOrientation(pWaver->GetOrientation());
            pHull->SetAlpha      (pWaver->GetAlpha      () * (1.0f - 0.5f*fTime));
            pHull->SetTexOffset  (pWaver->GetTexOffset  ());

            // 
            if(pWaver->ReachedDeath()) this->__DisableWaver(i, true);
        }

        // 
        m_TurretHull .MoveNormal();
        m_GunnerHull .MoveNormal();
        m_DriverHull .MoveNormal();
        m_ChargerHull.MoveNormal();
        m_WaverHull  .MoveNormal();

        // 
        if(m_iTurretActive) PHASE_CONTROL_TICKER(2u, 0u, g_pGame->IsEasy() ? 0.5f : 1.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
            {
                const cCustomEnemy* pTurret = &m_aTurret[i];
                if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pTurret->GetPosition ().xy();
                const coreVector2 vDir = pTurret->AimAtPlayerDual(i % 2u).Normalized();

                for(coreUintW j = 0u; j < 4u; ++j)
                {
                    const coreVector2 vTan = StepRotated90(j) * 3.0f;

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, this, vPos + vTan, vDir)->ChangeSize(1.2f);
                }

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });

        // 
        if(m_iGunnerActive) PHASE_CONTROL_TICKER(3u, 0u, 12.0f, LERP_LINEAR)
        {
            if(g_pGame->IsEasy() && ((iTick % 8u) < 4u)) return;

            // 
            for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
            {
                const cCustomEnemy* pGunner = &m_aGunner[i];
                if(!pGunner->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pGunner->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * (0.05f*PI));

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.2f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_RED);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });

        // 
        if(m_iChargerActive) PHASE_CONTROL_TICKER(4u, 0u, 30.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i)
            {
                const cCustomEnemy* pCharger = &m_aCharger[i];
                if(!pCharger->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                if(g_pGame->IsEasy() && (pCharger->GetLifeTime() > 0.6f)) return;

                const coreVector2 vPos = pCharger->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(pCharger->GetLifeTime() * 8.0f);

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.5f, this, vPos, vDir)->ChangeSize(1.6f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_ORANGE);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });

        // 
        if(m_iDriverActive) PHASE_CONTROL_TICKER(5u, 0u, 10.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_DRIVERS; ++i)
            {
                const cCustomEnemy* pDriver = &m_aDriver[i];
                if(!pDriver->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;


            }
        });

        // 
        if(m_iWaverActive) PHASE_CONTROL_TICKER(6u, 0u, 0.9f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_WAVERS; ++i)
            {
                const cCustomEnemy* pWaver = &m_aWaver[i];
                if(!pWaver->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                for(coreUintW j = 6u; j--; )
                {
                    if(g_pGame->IsEasy() && (j % 2u)) continue;

                    const coreVector2 vPos = pWaver->GetPosition().xy();
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 30.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f * 0.8f, this, vPos,  vDir)->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f * 0.8f, this, vPos,  vDir)->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f * 0.8f, this, vPos, -vDir)->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f * 0.8f, this, vPos, -vDir)->ChangeSize(1.5f);
                }

                // 
                g_pSpecialEffects->CreateSplashColor(pWaver->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->PlaySound(pWaver->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });
    }

    // constantly shoot into three directions
    PHASE_CONTROL_TICKER(7u, 0u, TORUS_BOSS_ROTATION * (2.0f / PI) * fShootSpeed * (g_pGame->IsEasy() ? 0.5f : 1.0f), LERP_LINEAR)
    {
        // 
        if(!m_aiCounter[EMIT_STATUS])
        {
            m_aiCounter[EMIT_STATUS] = 1;
            return;
        }

        // 
        const coreMatrix2 mRota = coreMatrix3::Rotation(this->GetDirection().xy().InvertedX().Normalized()).m12();

        for(coreUintW i = 5u; i--; )
        {
            if((i == 0u) || (i == 4u)) continue;

            coreVector2 avDir[3];

            // 
            avDir[0] = (coreVector2::Direction(DEG_TO_RAD(I_TO_F(i - 2u) * 9.0f)) * mRota).Normalized();
            avDir[1] = avDir[0].Rotated120();
            avDir[2] = avDir[1].Rotated120();

            for(coreUintW j = 0u; j < 3u; ++j)
            {
                const coreVector2 vDir = avDir[j];
                const coreVector2 vPos = this->GetPosition().xy() + vDir * (4.7f * this->GetSize().x);

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.6f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 2u, COLOR_ENERGY_YELLOW);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        }
    });

    // 
    if(!coreMath::IsNear(ABS(vNewOri.z), 1.0f) || (this->GetCurHealth() <= 0) || (g_pGame->IsEasy() && ((m_iPhase < 100u) || (m_iPhase >= 110u))))
        m_aiCounter[EMIT_STATUS] = 0;

    // 
    m_Emitter.SetPosition   (this->GetPosition   ());
    m_Emitter.SetDirection  (this->GetDirection  ());
    m_Emitter.SetOrientation(this->GetOrientation());
    m_Emitter.SetAlpha      (MAX0(m_aTimer[7].GetValue(CORE_TIMER_GET_NORMAL) * 2.0f - 1.0f));
    m_Emitter.SetTexOffset  (coreVector2(-0.1f,-0.4f) * m_fAnimation);
    m_Emitter.SetEnabled    (m_aiCounter[EMIT_STATUS] ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Emitter.Move();

    // 
    const coreFloat fFade  = 1.0f - FRACT(ABS(m_fAnimation));
    m_aCircle[1].SetSize(this->GetSize() * (0.5f + 0.5f*fFade));
    m_aCircle[1].SetAlpha(fFade);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
    {
        m_aCircle[i].SetPosition   (this->GetPosition   ());
        m_aCircle[i].SetDirection  (this->GetDirection  ());
        m_aCircle[i].SetOrientation(this->GetOrientation());
        m_aCircle[i].SetTexOffset  (coreVector2(0.1f * m_fAnimation, 0.0f));
        m_aCircle[i].Move();
    }

    // 
    m_Summon.SetTexOffset(coreVector2(0.0f, 0.3f * m_fAnimation));
    m_Summon.Move();
    
    
    if(m_avVector[COLOR_CHANGE].z)
    {
        m_avVector[COLOR_CHANGE].z = MAX0(m_avVector[COLOR_CHANGE].z - 0.5f * TIME);

        const auto nColorFunc = [](const coreUint8 iType)
        {
            switch(iType)
            {
            default: UNREACHABLE
            case 0u: return COLOR_ENERGY_WHITE   * 0.3f;
            case 1u: return COLOR_ENERGY_YELLOW  * 0.8f;
            case 2u: return COLOR_ENERGY_ORANGE  * 0.9f;
            case 3u: return COLOR_ENERGY_RED     * 0.7f;
            case 4u: return COLOR_ENERGY_MAGENTA * 0.9f;
            case 5u: return COLOR_ENERGY_PURPLE  * 1.0f;
            case 6u: return COLOR_ENERGY_BLUE    * 1.0f;
            case 7u: return COLOR_ENERGY_CYAN    * 0.8f;
            case 8u: return COLOR_ENERGY_GREEN   * 0.8f;
            }
        };

        const coreVector3 vFrom = nColorFunc(F_TO_UI(m_avVector[COLOR_CHANGE].x));
        const coreVector3 vTo   = nColorFunc(F_TO_UI(m_avVector[COLOR_CHANGE].y));

        const coreVector3 vNewColor = TernaryLerp(vTo, nColorFunc(0u), vFrom, BLENDS(m_avVector[COLOR_CHANGE].z));

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
            m_aCircle[i].SetColor3(vNewColor);
    }

    if(m_iPhase == 133u)
    {
        g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
        {
            const coreVector2 vDiff    = pPlayer->GetPosition().xy() - this->GetPosition().xy();
            const coreVector2 vDiffOld = pPlayer->GetOldPos()        - this->GetOldPos();

            const coreFloat fDot    = coreVector3::Dot(coreVector3(vDiff,    0.0f), this->GetOrientation());
            const coreFloat fDotOld = coreVector3::Dot(coreVector3(vDiffOld, 0.0f), this->GetOrientation());   // not old orientation

            if((SIGN(fDot) != SIGN(fDotOld)) && (vDiff.LengthSq() < POW2(8.0f)))
            {
                m_avVector[GATE_DATA].arr(i) = 0.3f;
            }
        });
    }
    g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
    {
        coreFloat& fValue = m_avVector[GATE_DATA].arr(i);
        STATIC_ASSERT(GAME_PLAYERS <= 4u)

        if(!pPlayer->IsNormal()) fValue = 0.0f;

        if(fValue)
        {
            fValue = MAX0(fValue - 1.0f * TIME);
            if(!fValue) pMission->GiveBadge(3u, BADGE_ACHIEVEMENT, this->GetPosition());
        }
    });

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(150u)
    }
    
    
    // 
    cHelper* pYellowHelper = g_pGame->GetHelper(ELEMENT_YELLOW);
    if(!pYellowHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat   fTime = pYellowHelper->GetLifeTime() * 0.3f;
        const coreFloat   fLen  = LERP(1.3f, 1.0f, SIN(fTime * (1.0f*PI))) * FOREGROUND_AREA.x;
        const coreVector2 vDir  = coreVector2::Direction(fTime * (-2.0f*PI) + (1.5f*PI));
        const coreVector2 vPos  = vDir * fLen;

        pYellowHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_YELLOW, false);
    }

    // 
    cHelper* pOrangeHelper = g_pGame->GetHelper(ELEMENT_ORANGE);
    if(!pOrangeHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vDir = m_avVector[HELPER_DATA].xy();
        const coreVector2 vPos = pOrangeHelper->GetPosition().xy() + vDir * ((20.0f + 80.0f * pOrangeHelper->GetLifeTime()) * TIME);

        pOrangeHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pOrangeHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_ORANGE, false);
        }
    }

    // 
    cHelper* pRedHelper = g_pGame->GetHelper(ELEMENT_RED);
    if(!pRedHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vFactor = (F_TO_UI(m_avVector[GRIND_VALUE].z) % 2u) ? coreVector2(-1.5f,1.0f) : coreVector2(1.0f,-1.5f);

        pRedHelper->SetPosition(this->GetPosition() * coreVector3(vFactor, 1.0f));

        if(pRedHelper->GetLifeTime() >= 1.8f)
        {
            this->_KillHelper(ELEMENT_RED, false);
        }
    }

    // 
    cHelper* pMagentaHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);
    if(!pMagentaHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pMagentaHelper->GetLifeTime() * 0.4f;

        pMagentaHelper->SetPosition(coreVector3(0.8f, -1.3f + 0.5f * SIN(fTime * (1.0f*PI)), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_MAGENTA, false);
    }

    // 
    cHelper* pPurpleHelper = g_pGame->GetHelper(ELEMENT_PURPLE);
    if(!pPurpleHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        coreUintW iCount = 0u;

        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
        {
            if(pMission->GetLaser(i)->IsEnabled(CORE_OBJECT_ENABLE_ALL))
                iCount += 1u;
        }

        if(iCount < 1u) this->_KillHelper(ELEMENT_PURPLE, true);
    }

    // 
    cHelper* pBlueHelper = g_pGame->GetHelper(ELEMENT_BLUE);
    if(!pBlueHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pBlueHelper->GetLifeTime() * 0.3f;

        pBlueHelper->SetPosition(coreVector3(LERPS(-1.3f, 1.3f, fTime), 0.8f, 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_BLUE, false);
    }

    // 
    cHelper* pCyanHelper = g_pGame->GetHelper(ELEMENT_CYAN);
    if(!pCyanHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vDir = m_avVector[HELPER_DATA].xy();
        const coreVector2 vPos = pCyanHelper->GetPosition().xy() + vDir * (FOREGROUND_AREA.x * 0.5f * TIME);

        pCyanHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pCyanHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_CYAN, false);
        }
    }

    // 
    cHelper* pGreenHelper = g_pGame->GetHelper(ELEMENT_GREEN);
    if(!pGreenHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        if(pGreenHelper->GetLifeTime() >= 2.0f)
        {
            this->_KillHelper(ELEMENT_GREEN, true);
        }
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableSummon(const coreVector2 vPosition, const coreVector3 vColor)
{
    WARN_IF(m_Summon.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableSummon(false);

    // 
    m_Summon.SetPosition(coreVector3(vPosition,0.0f));
    m_Summon.SetSize    (coreVector3(0.0f,0.0f,0.0f));
    m_Summon.SetColor3  (vColor);

    // 
    m_Summon.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Summon);
}


// ****************************************************************
// 
void cTorusBoss::__DisableSummon(const coreBool bAnimated)
{
    if(!m_Summon.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Summon.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Summon);
}


// ****************************************************************
// 
void cTorusBoss::__EnableTurret(const coreUintW iIndex, const coreVector2 vPosition)
{
    ASSERT(iIndex < TORUS_TURRETS)
    cCustomEnemy* pTurret = &m_aTurret       [iIndex];
    coreObject3D* pHull   = &m_aTurretHullRaw[iIndex];

    // 
    if(!pTurret->HasStatus(ENEMY_STATUS_DEAD)) return;
    pTurret->Resurrect();

    // 
    ASSERT(!HAS_BIT(m_iTurretActive, iIndex))
    ADD_BIT(m_iTurretActive, iIndex)

    // 
    SET_BIT(m_iTurretMove, iIndex, (iIndex % 2u))

    // 
    pTurret->SetPosition(coreVector3(vPosition, 0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_CYAN);
    g_pSpecialEffects->PlaySound(coreVector3(vPosition, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
}


// ****************************************************************
// 
void cTorusBoss::__DisableTurret(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TORUS_TURRETS)
    cCustomEnemy* pTurret = &m_aTurret       [iIndex];
    coreObject3D* pHull   = &m_aTurretHullRaw[iIndex];

    // 
    if(pTurret->HasStatus(ENEMY_STATUS_DEAD)) return;
    pTurret->Kill(false);

    // 
    ASSERT(HAS_BIT(m_iTurretActive, iIndex))
    REMOVE_BIT(m_iTurretActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->MacroExplosionColorSmall(pTurret->GetPosition(), COLOR_ENERGY_CYAN);
        g_pSpecialEffects->PlaySound(pTurret->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableGunner(const coreUintW iIndex, const coreVector2 vPosition)
{
    ASSERT(iIndex < TORUS_GUNNERS)
    cCustomEnemy* pGunner = &m_aGunner       [iIndex];
    coreObject3D* pHull   = &m_aGunnerHullRaw[iIndex];

    // 
    if(!pGunner->HasStatus(ENEMY_STATUS_DEAD)) return;
    pGunner->Resurrect();

    // 
    ASSERT(!HAS_BIT(m_iGunnerActive, iIndex))
    ADD_BIT(m_iGunnerActive, iIndex)

    // 
    pGunner->SetPosition(coreVector3(vPosition, 0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_RED);
    g_pSpecialEffects->PlaySound(coreVector3(vPosition, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
}


// ****************************************************************
// 
void cTorusBoss::__DisableGunner(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TORUS_GUNNERS)
    cCustomEnemy* pGunner = &m_aGunner       [iIndex];
    coreObject3D* pHull   = &m_aGunnerHullRaw[iIndex];

    // 
    if(pGunner->HasStatus(ENEMY_STATUS_DEAD)) return;
    pGunner->Kill(false);

    // 
    ASSERT(HAS_BIT(m_iGunnerActive, iIndex))
    REMOVE_BIT(m_iGunnerActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->MacroExplosionColorSmall(pGunner->GetPosition(), COLOR_ENERGY_RED);
        g_pSpecialEffects->PlaySound(pGunner->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableCharger(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < TORUS_CHARGERS)
    cCustomEnemy* pCharger = &m_aCharger       [iIndex];
    coreObject3D* pHull    = &m_aChargerHullRaw[iIndex];

    // 
    if(!pCharger->HasStatus(ENEMY_STATUS_DEAD)) return;
    pCharger->Resurrect();

    // 
    ASSERT(!HAS_BIT(m_iChargerActive, iIndex))
    ADD_BIT(m_iChargerActive, iIndex)

    // 
    pCharger->SetPosition (coreVector3(vPosition,  0.0f));
    pCharger->SetDirection(coreVector3(vDirection, 0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_ORANGE);
    g_pSpecialEffects->PlaySound(coreVector3(vPosition, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
}


// ****************************************************************
// 
void cTorusBoss::__DisableCharger(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TORUS_CHARGERS)
    cCustomEnemy* pCharger = &m_aCharger       [iIndex];
    coreObject3D* pHull    = &m_aChargerHullRaw[iIndex];

    // 
    if(pCharger->HasStatus(ENEMY_STATUS_DEAD)) return;
    pCharger->Kill(false);

    // 
    ASSERT(HAS_BIT(m_iChargerActive, iIndex))
    REMOVE_BIT(m_iChargerActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->MacroExplosionColorSmall(pCharger->GetPosition(), COLOR_ENERGY_ORANGE);
        g_pSpecialEffects->PlaySound(pCharger->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableDriver(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < TORUS_DRIVERS)
    cCustomEnemy* pDriver = &m_aDriver       [iIndex];
    coreObject3D* pHull   = &m_aDriverHullRaw[iIndex];

    // 
    if(!pDriver->HasStatus(ENEMY_STATUS_DEAD)) return;
    pDriver->Resurrect();

    // 
    ASSERT(!HAS_BIT(m_iDriverActive, iIndex))
    ADD_BIT(m_iDriverActive, iIndex)

    // 
    pDriver->SetPosition(coreVector3(vPosition, 0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());
    pMission->EnableLaser(iIndex, pDriver);
    pMission->GetLaser(iIndex)->SetPosition (coreVector3(vPosition, 0.0f));//coreVector3(0.0f,0.0f,0.0f));
    pMission->GetLaser(iIndex)->SetDirection(coreVector3(vDirection,0.0f));//coreVector3(vPosition,0.0f).Normalized());

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_PURPLE);
    g_pSpecialEffects->PlaySound(coreVector3(vPosition, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
}


// ****************************************************************
// 
void cTorusBoss::__DisableDriver(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TORUS_DRIVERS)
    cCustomEnemy* pDriver = &m_aDriver       [iIndex];
    coreObject3D* pHull   = &m_aDriverHullRaw[iIndex];

    // 
    if(pDriver->HasStatus(ENEMY_STATUS_DEAD)) return;
    pDriver->Kill(false);

    // 
    ASSERT(HAS_BIT(m_iDriverActive, iIndex))
    REMOVE_BIT(m_iDriverActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    d_cast<cViridoMission*>(g_pGame->GetCurMission())->DisableLaser(iIndex, bAnimated);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->MacroExplosionColorSmall(pDriver->GetPosition(), COLOR_ENERGY_PURPLE);
        g_pSpecialEffects->PlaySound(pDriver->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
    }
}


// ****************************************************************
// 
void cTorusBoss::__EnableWaver(const coreUintW iIndex, const coreVector2 vPosition)
{
    ASSERT(iIndex < TORUS_WAVERS)
    cCustomEnemy* pWaver = &m_aWaver       [iIndex];
    coreObject3D* pHull  = &m_aWaverHullRaw[iIndex];

    // 
    if(!pWaver->HasStatus(ENEMY_STATUS_DEAD)) return;
    pWaver->Resurrect();

    // 
    ASSERT(!HAS_BIT(m_iWaverActive, iIndex))
    ADD_BIT(m_iWaverActive, iIndex)

    // 
    pWaver->SetPosition(coreVector3(vPosition, 0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_GREEN);
    g_pSpecialEffects->PlaySound(coreVector3(vPosition, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
}


// ****************************************************************
// 
void cTorusBoss::__DisableWaver(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TORUS_WAVERS)
    cCustomEnemy* pWaver = &m_aWaver       [iIndex];
    coreObject3D* pHull  = &m_aWaverHullRaw[iIndex];

    // 
    if(pWaver->HasStatus(ENEMY_STATUS_DEAD)) return;
    pWaver->Kill(false);

    // 
    ASSERT(HAS_BIT(m_iWaverActive, iIndex))
    REMOVE_BIT(m_iWaverActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->MacroExplosionColorSmall(pWaver->GetPosition(), COLOR_ENERGY_GREEN);
        g_pSpecialEffects->PlaySound(pWaver->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
    }
}


// ****************************************************************
// 
void cTorusBoss::__CreateOverdrive(const coreVector3 vIntersect)
{
    // 
    constexpr coreFloat fMin = 2.5f;
    constexpr coreFloat fMax = 5.0f;
    coreVector3 vOldHit = m_avVector[OVERDRIVE_HIT].xyz();

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
            const coreVector3 vNewHit      = (fLen > fMax) ? LERP(vOldHit, vIntersect, fMax*RCP(fLen)) : vIntersect;
            const coreVector2 vOldOnScreen = g_pForeground->Project2D(vOldHit);
            const coreVector2 vNewOnScreen = g_pForeground->Project2D(vNewHit);

            // 
            if(((ABS(vOldOnScreen.x) < 0.55f) && (ABS(vOldOnScreen.y) < 0.55f)) ||
               ((ABS(vNewOnScreen.x) < 0.55f) && (ABS(vNewOnScreen.y) < 0.55f)))
            {
                // 
                if(HAS_BIT(m_iDecalState, 0u)) TOGGLE_BIT(m_iDecalState, 1u)
                TOGGLE_BIT(m_iDecalState, 0u)

                // 
                const coreBool    bRotated   = HAS_BIT(m_iDecalState, 0u);
                const coreBool    bFlipped   = HAS_BIT(m_iDecalState, 1u);
                const coreVector3 vDecalPos  = (vOldHit + vNewHit) * 0.5f;
                const coreVector2 vDecalSize = coreVector2(Core::Rand->Float(5.0f, 6.5f), MIN(fLen, fMax)*1.8f);
                const coreVector2 vDecalDir  = vDiff.xy().Normalized();

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "effect_soot.png");
                pObject->DefineProgram("effect_decal_single_program");

                // set object properties
                pObject->SetSize     (coreVector3((bRotated ? vDecalSize.yx()       : vDecalSize),                            1.0f));
                pObject->SetDirection(coreVector3((bRotated ? vDecalDir.Rotated90() : vDecalDir) * (bFlipped ? -1.0f : 1.0f), 0.0f));
                pObject->SetColor3   (coreVector3(0.0f,0.0f,0.0f));

                // 
                g_pEnvironment->GetBackground()->AddDecal(pObject, vDecalPos, 128u, "effect_decal_single_inst_program", LIST_KEY);

                // 
                g_pSpecialEffects->CreateSplashFire (vNewHit,  5.0f, 3u, COLOR_FIRE_ORANGE);
                g_pSpecialEffects->CreateSplashColor(vNewHit, 25.0f, 2u, COLOR_FIRE_ORANGE);
            }

            // 
            g_pSpecialEffects->ShakeScreen(0.3f);

            // 
            vOldHit = vNewHit;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * TIME * OUTDOOR_DETAIL;

    // 
    m_avVector[OVERDRIVE_HIT].xyz(vOldHit);
}


// ****************************************************************
// 
void cTorusBoss::__ChangeColor(const coreUint8 iType)
{
    m_avVector[COLOR_CHANGE].x = m_avVector[COLOR_CHANGE].y;
    m_avVector[COLOR_CHANGE].y = I_TO_F(iType);
    m_avVector[COLOR_CHANGE].z = 1.0f;
}


// ****************************************************************
// 
coreFloat cTorusBoss::__PositionToGrind(const coreVector2 vPosition)
{
    if(vPosition.IsNull()) return 0.5f;

    if(IsHorizontal(vPosition)) return (vPosition.y * RCP(ABS(vPosition.x))) * -SIGN(vPosition.x) * 0.5f + 0.5f + ((vPosition.x > 0.0f) ? 1.0f : 3.0f);
                           else return (vPosition.x * RCP(ABS(vPosition.y))) *  SIGN(vPosition.y) * 0.5f + 0.5f + ((vPosition.y > 0.0f) ? 0.0f : 2.0f);
}


// ****************************************************************
// 
coreVector2 cTorusBoss::__GrindToPosition(const coreFloat fGrind)
{
    ASSERT(fGrind >= 0.0f)

    const coreFloat fRealGrind = FMOD(fGrind, 4.0f);

    if(fRealGrind < 1.0f) return LERP(coreVector2(-1.0f, 1.0f), coreVector2( 1.0f, 1.0f), fRealGrind);
    if(fRealGrind < 2.0f) return LERP(coreVector2( 1.0f, 1.0f), coreVector2( 1.0f,-1.0f), fRealGrind - 1.0f);
    if(fRealGrind < 3.0f) return LERP(coreVector2( 1.0f,-1.0f), coreVector2(-1.0f,-1.0f), fRealGrind - 2.0f);
                          return LERP(coreVector2(-1.0f,-1.0f), coreVector2(-1.0f, 1.0f), fRealGrind - 3.0f);
}


// ****************************************************************
// 
coreVector2 cTorusBoss::__GrindToPosition2(const coreFloat fGrind)
{
    ASSERT(fGrind >= 0.0f)

    const coreFloat fRealGrind = FMOD(fGrind - 0.5f, 4.0f);

    if(fRealGrind < 1.0f) return LERP(coreVector2( 0.0f, 1.0f), coreVector2( 1.0f, 0.0f), fRealGrind);
    if(fRealGrind < 2.0f) return LERP(coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f), fRealGrind - 1.0f);
    if(fRealGrind < 3.0f) return LERP(coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f), fRealGrind - 2.0f);
                          return LERP(coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f), fRealGrind - 3.0f);

    //if(fRealGrind < 1.0f) return LERP(coreVector2( 0.0f, 1.0f), coreVector2( 1.0f, 0.0f), fRealGrind);
    //if(fRealGrind < 2.0f) return LERP(coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f), fRealGrind - 1.0f);
    //if(fRealGrind < 3.0f) return LERP(coreVector2( 0.0f,-1.0f), coreVector2( 0.0f, 1.0f), fRealGrind - 2.0f);
    //if(fRealGrind < 4.0f) return LERP(coreVector2( 0.0f, 1.0f), coreVector2(-1.0f, 0.0f), fRealGrind - 3.0f);
    //if(fRealGrind < 5.0f) return LERP(coreVector2(-1.0f, 0.0f), coreVector2( 0.0f,-1.0f), fRealGrind - 4.0f);
    //                      return LERP(coreVector2( 0.0f,-1.0f), coreVector2( 0.0f, 1.0f), fRealGrind - 5.0f);
}


// ****************************************************************
// 
coreVector2 cTorusBoss::__GrindToPosition3(const coreFloat fGrind)
{
    ASSERT(fGrind >= 0.0f)

    constexpr coreFloat C = (1.0f/3.0f);

    const coreFloat fRealGrind = FMOD(fGrind - 0.5f, 4.0f);

    if(fRealGrind <  2.0f*C) return LERP(coreVector2( 0.0f*C, 1.0f), coreVector2( 2.0f*C,-1.0f), (fRealGrind)             / (2.0f*C));
    if(fRealGrind <  3.0f*C) return LERP(coreVector2( 2.0f*C,-1.0f), coreVector2( 3.0f*C, 0.0f), (fRealGrind -  (2.0f*C)) / (1.0f*C));
    if(fRealGrind <  4.0f*C) return LERP(coreVector2( 3.0f*C, 0.0f), coreVector2( 2.0f*C, 1.0f), (fRealGrind -  (3.0f*C)) / (1.0f*C));
    if(fRealGrind <  6.0f*C) return LERP(coreVector2( 2.0f*C, 1.0f), coreVector2( 0.0f*C,-1.0f), (fRealGrind -  (4.0f*C)) / (2.0f*C));
    if(fRealGrind <  8.0f*C) return LERP(coreVector2( 0.0f*C,-1.0f), coreVector2(-2.0f*C, 1.0f), (fRealGrind -  (6.0f*C)) / (2.0f*C));
    if(fRealGrind <  9.0f*C) return LERP(coreVector2(-2.0f*C, 1.0f), coreVector2(-3.0f*C, 0.0f), (fRealGrind -  (8.0f*C)) / (1.0f*C));
    if(fRealGrind < 10.0f*C) return LERP(coreVector2(-3.0f*C, 0.0f), coreVector2(-2.0f*C,-1.0f), (fRealGrind -  (9.0f*C)) / (1.0f*C));
                             return LERP(coreVector2(-2.0f*C,-1.0f), coreVector2( 0.0f*C, 1.0f), (fRealGrind - (10.0f*C)) / (2.0f*C));
}