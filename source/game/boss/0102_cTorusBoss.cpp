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
// TODO 1: clean up code (also in mission)
// TODO 1: bottom state during intro
// TODO 1: boss steckt im boden (nur 1x) bei einer gruppe davor (foreshadowing)
// TODO 1: shields needs to start deterministic
// TODO 1: yellow phase needs to start deterministic, spin into correct angle on back-bounce (1/3), depending on current emitter-time
// TODO 1: all create-index-selections need to be correct (specific index on static positions, round-robin on moving/dynamic)
// TODO 1: badge: flieg durch den boss hindurch
// TODO 1: ending: herumrudern, überflippen und abstürzen
// TODO 1: vielleicht die schilde in schild-phase etwas interessanter anordnen, vielleicht fliegen sie auch nach außen ?
// TODO 1: schilde schön erzeugen und entfernen (nach außen fliegen ?)
// TODO 1: laser schön erzeugen ?
// TODO 1: 4th infinity move ? up middle down middle
// TODO 1: make sure to force-set orientation back to normal after orientation-changing phases
// TODO 1: (finale) phase bei der alle objekte nochmal erzeugt werden ?
// TODO 1: die turrets sollten vielleicht abhängig vom sprung-start erzeugt werden (entweder X oder Y seite)
// TODO 1: size got increased, adjust intro animation
// TODO 1: MAIN: fragment, easy, hard (decision), coop, 3 badges, boss health, medal goal, intro, outro, foreshadow


// ****************************************************************
// counter identifier
#define EMIT_STATUS (0u)
#define CUR_TARGET  (1u)
#define SLIDE_COUNT (2u)
#define CRASH_COUNT (3u)


// ****************************************************************
// vector identifier
#define GRIND_VALUE   (0u)
#define OVERDRIVE_HIT (1u)
#define JUMP_TARGET   (2u)
#define COLOR_CHANGE  (3u)


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
, m_iGunnerActive   (0u)
, m_iGunnerMove     (0u)
, m_iChargerActive  (0u)
, m_iDriverActive   (0u)
, m_iWaverActive    (0u)
, m_iDecalState     (0u)
{
    // load models
    this->DefineModelHigh("ship_boss_torus_high.md3");
    this->DefineModelLow ("ship_boss_torus_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 2.0f);

    // configure the boss
    this->Configure(7500, COLOR_SHIP_GREY);

    // 
    m_Emitter.DefineModel  ("object_boss_torus_emitter.md3");
    m_Emitter.DefineTexture(0u, "effect_energy.png");
    m_Emitter.DefineProgram("effect_energy_invert_program");
    m_Emitter.SetSize      (this->GetSize());
    m_Emitter.SetColor3    (COLOR_ENERGY_YELLOW * 0.8f);
    m_Emitter.SetTexSize   (coreVector2(1.25f,1.25f));

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
    {
        m_aCircle[i].DefineModel  ("object_boss_torus_circle.md3");
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
        m_aTurret[i].DefineModelHigh("object_cube_rota.md3");
        m_aTurret[i].DefineModelLow ("object_cube_rota.md3");
        m_aTurret[i].DefineTexture  (0u, "effect_energy.png");
        m_aTurret[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aTurret[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 2.85f);
        m_aTurret[i].SetTexSize     (coreVector2(0.8f,0.3f));
        m_aTurret[i].Configure      (10, COLOR_ENERGY_CYAN * 0.7f);
        m_aTurret[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_TurretHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            // load object resources
            coreObject3D* pTurret = &m_aTurretHullRaw[i];
            pTurret->DefineModel  ("object_cube_rota.md3");
            pTurret->DefineTexture(0u, "effect_energy.png");
            pTurret->DefineProgram("effect_energy_invert_program");

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
        m_aGunner[i].DefineModelHigh("object_tetra_rota.md3");
        m_aGunner[i].DefineModelLow ("object_tetra_rota.md3");
        m_aGunner[i].DefineTexture  (0u, "effect_energy.png");
        m_aGunner[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aGunner[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
        m_aGunner[i].SetTexSize     (coreVector2(0.8f,0.3f));
        m_aGunner[i].Configure      (10, COLOR_ENERGY_RED * 0.8f);
        m_aGunner[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_GunnerHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        {
            // load object resources
            coreObject3D* pGunner = &m_aGunnerHullRaw[i];
            pGunner->DefineModel  ("object_tetra_rota.md3");
            pGunner->DefineTexture(0u, "effect_energy.png");
            pGunner->DefineProgram("effect_energy_invert_program");

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
        m_aCharger[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aCharger[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 2.5f);
        m_aCharger[i].SetTexSize     (coreVector2(0.5f,0.2f) * 2.0f);
        m_aCharger[i].Configure      (10, COLOR_ENERGY_ORANGE * 1.0f);
        m_aCharger[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST_BULLET);
    }

    // 
    m_ChargerHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i)
        {
            // load object resources
            coreObject3D* pCharger = &m_aChargerHullRaw[i];
            pCharger->DefineModel  ("bullet_cone.md3");
            pCharger->DefineTexture(0u, "effect_energy.png");
            pCharger->DefineProgram("effect_energy_invert_program");

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
        m_aDriver[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aDriver[i].SetSize        (coreVector3(2.6f,2.0f,2.6f) * 2.0f);
        m_aDriver[i].SetTexSize     (coreVector2(0.4f,0.2f) * 2.0f);
        m_aDriver[i].Configure      (10, COLOR_ENERGY_PURPLE * 1.0f);
        m_aDriver[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_DriverHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_DRIVERS; ++i)
        {
            // load object resources
            coreObject3D* pDriver = &m_aDriverHullRaw[i];
            pDriver->DefineModel  ("bullet_spear.md3");
            pDriver->DefineTexture(0u, "effect_energy.png");
            pDriver->DefineProgram("effect_energy_invert_program");

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
        m_aWaver[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aWaver[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 3.1f);
        m_aWaver[i].SetTexSize     (coreVector2(4.5f,4.5f));
        m_aWaver[i].Configure      (10, COLOR_ENERGY_GREEN * 0.8f);
        m_aWaver[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_WaverHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_WAVERS; ++i)
        {
            // load object resources
            coreObject3D* pWaver = &m_aWaverHullRaw[i];
            pWaver->DefineModel  ("object_sphere.md3");
            pWaver->DefineTexture(0u, "effect_energy.png");
            pWaver->DefineProgram("effect_energy_invert_program");

            // set object properties
            pWaver->SetSize   (m_aWaver[i].GetSize   ());
            pWaver->SetColor3 (m_aWaver[i].GetColor3 ());
            pWaver->SetTexSize(m_aWaver[i].GetTexSize());
            pWaver->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_WaverHull.BindObject(pWaver);
        }
    }

    STATIC_ASSERT(sizeof(m_iTurretActive) *8u >= TORUS_TURRETS)
    STATIC_ASSERT(sizeof(m_iGunnerActive) *8u >= TORUS_GUNNERS)
    STATIC_ASSERT(sizeof(m_iGunnerMove)   *8u >= TORUS_GUNNERS)
    STATIC_ASSERT(sizeof(m_iChargerActive)*8u >= TORUS_CHARGERS)
    STATIC_ASSERT(sizeof(m_iDriverActive) *8u >= TORUS_DRIVERS)
    STATIC_ASSERT(sizeof(m_iWaverActive)  *8u >= TORUS_WAVERS)
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
}


// ****************************************************************
// 
void cTorusBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    pMission->DisableBarrier(0u, bAnimated);
    pMission->DisableBarrier(1u, bAnimated);
    pMission->DisableBarrier(2u, bAnimated);
    pMission->DisableBarrier(3u, bAnimated);

    // 
    pMission->DisableShadow(0u, bAnimated);

    // 
    this->__DisableSummon();

    // 
    for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        this->__DisableTurret(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        this->__DisableGunner(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i)
        this->__DisableCharger(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < TORUS_DRIVERS; ++i)
        this->__DisableDriver(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < TORUS_WAVERS; ++i)
        this->__DisableWaver(i, bAnimated);

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
    g_pGame->GetBulletManagerEnemy()->ResetOrder();

    // 
    this->_EndBoss(bAnimated);
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

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        m_aCircle[i].Render();

    // 
    m_Summon.Render();
}


// ****************************************************************
// 
void cTorusBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    if(this->ReachedDeath()) this->Kill(true);   
    
    
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    coreVector3 vNewOri = coreVector3(0.0f, 0.0f, 1.0f);
    
    static coreVector3 vTestDir = coreVector3(0.0f,1.0f,0.0f);

    coreFloat fShootSpeed = 1.0f;

    // 
    m_fAnimation   .UpdateMod(-1.0f,               10.0f);
    m_fRotationBoss.UpdateMod(TORUS_BOSS_ROTATION, 2.0f*PI);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        static coreFloat fCurHeight = 0.0f;
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) this->AddStatus(ENEMY_STATUS_GHOST);
            
            if(PHASE_BEGINNING) g_pGame->GetInterface()->ShowAlert();

            // TODO 1: spin faster at the end, like a real coin
            
            if(fTime < 0.85f)
            {            
            const coreVector2 vDir = coreVector2::Direction(LERPB(12.0f*PI, 0.0f*PI, 1.0f - fTime));
            const coreFloat fLen = LERPS(0.0f, 1.5f, MAX(1.0f - fTime * 1.5f, 0.0f)) * FOREGROUND_AREA.x;
            
            const coreFloat fBonus = LERPS(0.0f, 1.0f, CLAMP((fTime - (2.0f/3.0f)) * 6.0f, 0.0f, 1.0f));
            const coreVector2 vDir2 = coreVector2::Direction(LERPB(12.0f*PI, 0.0f*PI, 1.0f - fTime)       + 0.0f*LERPB(-2.0f*PI, 0.0f*PI, 1.0f - fBonus));
            const coreFloat fLen2 = LERPS(0.3f, 5.0f, MAX(1.0f - fTime * 1.5f, 0.0f)) * FOREGROUND_AREA.x    * (1.0f - fBonus);
          
            const coreVector3 vAnchor = coreVector3(vDir2 * fLen2, -2.0f);
            
            const coreVector2 vNewPos = vDir * fLen;
            
            
            const coreVector3 vOri2 = (vAnchor - coreVector3(vNewPos, -20.0f)).Normalized();//LERP(coreVector3(-vDir, 0.0f), coreVector3(0.0f,0.0f,1.0f), fCurTime).Normalized();
            const coreVector2 vOffset = (ABS(vOri2.z) == 1.0f) ? coreVector2(0.0f,0.0f) : coreVector3::Cross(vOri2, coreVector3::Cross(vOri2, coreVector3(0.0f,0.0f,1.0f)).Normalized()).xy() * LERP(this->GetVisualRange().x, 0.0f, ABS(vOri2.z));
            
            const coreFloat fHeight = g_pEnvironment->RetrieveSafeHeight(vNewPos + vOffset) ;
            fCurHeight = LERP(fCurHeight, fHeight, 10.0f * TIME);
            this->SetPosition(coreVector3(vNewPos, fCurHeight + LERP(this->GetVisualRange().x - 2.0f, this->GetVisualRange().z + 1.0f*0.0f, ABS(vOri2.z))));
            
            if(fTime < 0.5f) this->__CreateOverdrive(coreVector3(vNewPos + vOffset, fCurHeight));
            
            m_fRotationBoss.UpdateMod(-LERPB(5.0f, 8.8f, fTime) - TORUS_BOSS_ROTATION, 2.0f*PI);
            vNewOri = -vOri2;
            
            
            vTestDir = coreVector3(-vDir2, 0.0f);
            }
            else
            {

                m_fRotationBoss = this->GetDirection().xy().InvertedY().Angle();
                vTestDir = -coreVector3(coreVector2::Direction(LERPB(12.0f*PI, 0.0f*PI, 0.15f)), 0.0f);
                vNewOri = coreVector3(0.0f,0.0f,1.0f);
            }
            

            if(PHASE_TIME_POINT(0.85f))
            {
                this->_StartBoss();

                this->__ChangeColor(6u);

                PHASE_CHANGE_INC
            }
        });
    }
    
    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        static coreFloat fTestHeight = this->GetPosition().z;
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_LINEAR)
        {
             if(PHASE_BEGINNING) this->RemoveStatus(ENEMY_STATUS_GHOST);
             
             m_fRotationBoss.UpdateMod(-TORUS_BOSS_ROTATION * (1.0f - fTime), 2.0f*PI);
             vTestDir = coreVector3(coreVector2::Direction(LERPB(12.0f*PI, 0.0f*PI, 0.15f)), 0.0f);
             
             const coreVector2 vOri = coreVector2::Direction(LERPS(0.0f*PI, 4.0f*PI, fTime));
             vNewOri = coreVector3(0.0f, vOri);
             
             this->SetPosition(coreVector3(this->GetPosition().xy(), LERPS(fTestHeight, 0.0f, fTime)));
             
             

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
            PHASE_CHANGE_TO(110u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        // fliegt auf rand
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
            m_avVector[GRIND_VALUE].x += 0.5f * TIME * fTime;

            this->SetPosition(coreVector3(cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * FOREGROUND_AREA * 0.9f, 0.0f));

            m_fRotationBoss.UpdateMod(-3.0f * fTime - TORUS_BOSS_ROTATION, 2.0f*PI);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        // rollt am rand entlang
        const coreFloat fPrevGrind = m_avVector[GRIND_VALUE].x;
        m_avVector[GRIND_VALUE].x += 0.5f * TIME;

        this->SetPosition(coreVector3(cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * FOREGROUND_AREA * 0.9f, 0.0f));

        m_fRotationBoss.UpdateMod(-3.0f - TORUS_BOSS_ROTATION, 2.0f*PI);   // -4.5f is realistic

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
                }
            });
        }
        else
        {
            if(InBetweenExt(0.5f, FRACT(fPrevGrind), FRACT(m_avVector[GRIND_VALUE].x)) == 1)
            {
                PHASE_CHANGE_TO(90u)

                this->__ChangeColor(7u);
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 83u)
    {
        // springt auf andere seite
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            const coreVector2 vTarget = cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * 0.9f;

            this->DefaultMoveLerp(m_vLastPosition, vTarget, fTime);

            if(PHASE_TIME_POINT(0.1f)) this->AddStatus(ENEMY_STATUS_DAMAGING);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(82u)

                this->RemoveStatus(ENEMY_STATUS_DAMAGING);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                
                this->__EnableDriver(F_TO_UI(m_avVector[GRIND_VALUE].x) % TORUS_DRIVERS, this->GetPosition().xy(), AlongCrossNormal(this->GetPosition().xy()));   // TODO 1: anderer index
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 90u)
    {
        // springt im muster
        const coreFloat fPrevGrind = m_avVector[GRIND_VALUE].x;
        m_avVector[GRIND_VALUE].x += 0.55f * SQRT2 * TIME;

        this->SetPosition(coreVector3(cTorusBoss::__GrindToPosition2(m_avVector[GRIND_VALUE].x) * FOREGROUND_AREA * 0.9f, 0.0f));

        if(InBetweenExt(0.5f, FRACT(fPrevGrind), FRACT(m_avVector[GRIND_VALUE].x)) == 1)
        {
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

            if(ABS(this->GetPosition().y) > 0.5f * FOREGROUND_AREA.y)
                this->__EnableTurret(F_TO_UI(m_avVector[GRIND_VALUE].x) % TORUS_TURRETS, this->GetPosition().xy());

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
    // 
    else if(m_iPhase == 91u)
    {
        // springt im muster 2
        const coreFloat fPrevGrind = m_avVector[GRIND_VALUE].x;
        m_avVector[GRIND_VALUE].x += 0.55f * (SQRT(2.0f) / SQRT(10.0f)) * SQRT2 * TIME;

        this->SetPosition(coreVector3(MapStepRotatedInv90(cTorusBoss::__GrindToPosition3(m_avVector[GRIND_VALUE].x), F_TO_UI(m_avVector[GRIND_VALUE].z)) * FOREGROUND_AREA * 0.9f, 0.0f));

        if(InBetweenExt(0.5f, FRACT(fPrevGrind), FRACT(m_avVector[GRIND_VALUE].x)) == 1)
        {
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

            if(this->GetCurHealth() < 2800)
            {
                PHASE_CHANGE_TO(100u)

                this->__ChangeColor(1u);

                m_avVector[GRIND_VALUE].x = cTorusBoss::__PositionToGrind(this->GetPosition().xy());
            }
        }

        if((InBetweenExt(0.5f + (2.0f/3.0f), FMOD(fPrevGrind, 2.0f), FMOD(m_avVector[GRIND_VALUE].x, 2.0f)) == 1) ||
           (InBetweenExt(1.5f + (1.0f/3.0f), FMOD(fPrevGrind, 2.0f), FMOD(m_avVector[GRIND_VALUE].x, 2.0f)) == 1))
        {
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                this->__EnableGunner(F_TO_UI(m_avVector[GRIND_VALUE].x) % TORUS_GUNNERS, this->GetPosition().xy());
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 100u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            const coreVector2 vTarget = cTorusBoss::__GrindToPosition(m_avVector[GRIND_VALUE].x) * 0.8f;

            this->DefaultMoveLerp(m_vLastPosition, vTarget, fTime);

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
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
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

                pMission->EnableBarrier(0u, this, coreVector2( 0.0f, 1.0f), -1, 3.0f);
                pMission->EnableBarrier(1u, this, coreVector2( 0.0f,-1.0f), -1, 3.0f);
                pMission->EnableBarrier(2u, this, coreVector2( 0.0f, 1.0f),  4, 2.0f);
                pMission->EnableBarrier(3u, this, coreVector2( 0.0f,-1.0f),  4, 2.0f);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 103u)
    {
        m_fRotationBoss.UpdateMod(5.0f, 2.0f*PI);

        fShootSpeed = 2.8f;

        if(this->GetCurHealth() < 1400)
        {
            PHASE_CHANGE_TO(140u)

            this->__ChangeColor(4u);

            pMission->DisableBarrier(0u, true);
            pMission->DisableBarrier(1u, true);
            pMission->DisableBarrier(2u, true);
            pMission->DisableBarrier(3u, true);
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
            default: ASSERT(false)
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

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[CUR_TARGET] += 1;
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
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 90.0f + fAngle));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, this, vPos, vDir)->ChangeSize(1.6f);
            }

            g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 5.0f, 1u, COLOR_ENERGY_BLUE);

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
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) pMission->EnableShadow(0u, this, m_avVector[JUMP_TARGET].xy());

            const coreFloat   fSin    = SIN(fTime * (1.0f*PI));
            const coreVector2 vPos    = LERP(m_vLastPosition * FOREGROUND_AREA, m_avVector[JUMP_TARGET].xy(), fTime) * (1.0f - 0.45f * fSin);
            const coreFloat   fHeight = 50.0f * fSin;

            this->SetPosition(coreVector3(vPos, fHeight));

            vNewOri.yz(coreVector2::Direction((6.0f*PI) * fTime));

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

                this->__EnableWaver(F_TO_UI(m_avVector[JUMP_TARGET].w) % TORUS_WAVERS, this->GetPosition().xy());

                m_avVector[JUMP_TARGET].w += 1.0f;   // TODO 1: sollte counter sein
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 121u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            m_avVector[JUMP_TARGET].xy(this->NearestPlayerDual(F_TO_UI(m_avVector[JUMP_TARGET].z))->GetPosition().xy());

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
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 123u)
    {
        PHASE_CONTROL_PAUSE(0u, 60.0f)
        {
            if(this->GetCurHealth() < 5600)
            {
                m_avVector[JUMP_TARGET].xy(coreVector2(0.0f,0.0f));
                m_avVector[JUMP_TARGET].z = -1.0f;
            }
            else
            {
                m_avVector[JUMP_TARGET].xy(this->NearestPlayerDual(F_TO_UI(m_avVector[JUMP_TARGET].z))->GetPosition().xy());
                m_avVector[JUMP_TARGET].z = 1.0f - m_avVector[JUMP_TARGET].z;
            }

            PHASE_CHANGE_TO(120u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 130u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 131u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(-1.4f, m_vLastPosition.y), fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->SetPosition(coreVector3(this->GetPosition().x, 0.7f * FOREGROUND_AREA.y, this->GetPosition().z));

                this->StorePosition();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 132u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            this->DefaultMoveLerp(coreVector2(1.4f, m_vLastPosition.y), coreVector2(-1.4f, m_vLastPosition.y), fTime);

            const coreFloat fRota = (this->GetPosition().x - -0.9f * FOREGROUND_AREA.x) * RCP(0.9f * FOREGROUND_AREA.x - -0.9f * FOREGROUND_AREA.x);

            vNewOri = coreVector3(0.0f,-1.0f,0.0f);
            vNewOri = coreVector3(0.0f, coreVector2::Direction(LERP(0.0f*PI, 1.0f*PI, fRota) + I_TO_F(m_aiCounter[SLIDE_COUNT] % 2) * (0.5f*PI)));

            if(!this->WasTeleporting())
            {
                if(PHASE_POSITION_POINT(this, 0.7f * FOREGROUND_AREA.x, x)) this->__EnableCharger(0u, this->GetPosition().xy(), this->AimAtPlayerSide().Normalized());
            }

            if(PHASE_FINISHED || ((m_aiCounter[SLIDE_COUNT] == 2) && (this->GetPosition().x <= -0.9f * FOREGROUND_AREA.x)))
            {
                m_aiCounter[SLIDE_COUNT] += 1;

                if(m_aiCounter[SLIDE_COUNT] >= 3)
                {
                    PHASE_CHANGE_INC

                    this->SetPosition(coreVector3(-0.9f * FOREGROUND_AREA.x, 0.0f, this->GetPosition().z));

                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                }
                else
                {
                    PHASE_RESET(0u)

                    coreFloat fSide;
                    switch(m_aiCounter[SLIDE_COUNT])
                    {
                    default: ASSERT(false)
                    case 1: fSide = -0.7f; break;
                    case 2: fSide =  0.0f; break;
                    }

                    this->SetPosition(coreVector3(this->GetPosition().x, fSide * FOREGROUND_AREA.y, this->GetPosition().z));

                    this->StorePosition();

                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 133u)
    {
        PHASE_CHANGE_TO(80u)

        this->__ChangeColor(5u);
    }

    // ################################################################
    // 
    else if(m_iPhase == 140u)
    {
        const coreVector2 vNewPos = this->NearestPlayerDual(0u)->GetPosition().xy() * -0.85f * BLENDH3(MIN1(m_fPhaseTime * 0.35f));

        this->SetPosition(coreVector3(vNewPos, 0.0f));
        
        const coreFloat fSpeed = STEP(0.0f, 1400.0f, I_TO_F(this->GetCurHealth()));
        fShootSpeed = LERP(4.0f, 2.8f, fSpeed);

        m_fRotationBoss.UpdateMod(5.0f * fShootSpeed / 2.8f, 2.0f*PI);


        PHASE_CONTROL_TICKER(0u, 0u, 12.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = this->AimAtPlayerDual(iTick % 2u).Normalized();
            const coreVector2 vTan = vDir.Rotated90() * ((iTick % 2u) ? -1.0f : 1.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, this, vPos, (vDir + vTan * 0.01f).Normalized())->ChangeSize(1.8f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_MAGENTA);
        });
    }

    // ################################################################
    // ################################################################

    
    
    // 
    if(m_iPhase == 0u)
    {
        vNewOri = -vNewOri;
        this->SetOrientation(coreVector3(vNewOri.x,  vNewOri.y,           vNewOri.z));
        
        
        const coreMatrix3 mRota = coreMatrix4::RotationAxis(m_fRotationBoss, vNewOri).m123();
        const coreVector3 vOri  = (ABS(vNewOri.z) == 1.0f) ? vTestDir : coreVector3::Cross(vNewOri, coreVector3::Cross(vNewOri, coreVector3(0.0f,0.0f,-1.0f)).Normalized());
        this->SetDirection(vOri * mRota);
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
        m_fRotationObject.UpdateMod(TORUS_TURRET_SPEED, 10.0f);

        // 
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(m_fRotationObject * PI), 0.0f);
        const coreVector3 vOri  = OriRoundDir(vDir.xy(), vDir.xy());
        const coreVector2 vTex  = coreVector2(0.2f,1.0f) * m_fRotationObject;
        const coreFloat   fTime = FMOD(m_fRotationObject * -5.0f, 2.0f);

        // 
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            cCustomEnemy* pTurret = &m_aTurret       [i];
            coreObject3D* pHull   = &m_aTurretHullRaw[i];
            if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            // 
            pTurret->SetDirection  (vDir);
            pTurret->SetOrientation(vOri);
            pTurret->SetTexOffset  (vTex);

            // 
            pHull->SetPosition   (pTurret->GetPosition   ());
            pHull->SetSize       (pTurret->GetSize       () * (1.0f + 0.25f*fTime));
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

            const coreVector3 vSize = pGunner->GetSize();

            // 
            const coreFloat   fMoveSide = HAS_BIT(m_iGunnerMove, i) ? -1.0f : 1.0f;
            const coreVector2 vMoveDir  = coreVector2(fMoveSide * FOREGROUND_AREA.x * TORUS_GUNNER_SPEED * TIME, 0.0f);
            const coreVector2 vNewPos   = pGunner->GetPosition().xy() + ((i & 0x02u) ? vMoveDir.yx() : vMoveDir);

            // 
            const auto nBounceEffect = [](const coreVector2 vEffectPos)
            {
                g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_RED);
                g_pSpecialEffects->PlaySound        (coreVector3(vEffectPos, 0.0f), 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
            };

            // 
                 if((vNewPos.x < -FOREGROUND_AREA.x) && (fMoveSide < 0.0f)) {nBounceEffect(vNewPos + coreVector2(-vSize.x, 0.0f)); REMOVE_BIT(m_iGunnerMove, i)}
            else if((vNewPos.x >  FOREGROUND_AREA.x) && (fMoveSide > 0.0f)) {nBounceEffect(vNewPos + coreVector2( vSize.x, 0.0f)); ADD_BIT   (m_iGunnerMove, i)}
                 if((vNewPos.y < -FOREGROUND_AREA.y) && (fMoveSide < 0.0f)) {nBounceEffect(vNewPos + coreVector2(0.0f, -vSize.y)); REMOVE_BIT(m_iGunnerMove, i)}
            else if((vNewPos.y >  FOREGROUND_AREA.y) && (fMoveSide > 0.0f)) {nBounceEffect(vNewPos + coreVector2(0.0f,  vSize.y)); ADD_BIT   (m_iGunnerMove, i)}

            // 
            //pGunner->SetPosition   (coreVector3(vNewPos, 0.0f));
            pGunner->SetDirection  (vDir);
            pGunner->SetOrientation(vOri);
            pGunner->SetTexOffset  (vTex);

            // 
            pHull->SetPosition   (pGunner->GetPosition   ());
            pHull->SetSize       (pGunner->GetSize       () * (1.0f + 0.25f*fTime));
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
            const coreFloat fSpeed = pCharger->GetLifeTime() * 80.0f;

            pCharger->SetPosition(pCharger->GetPosition() + pCharger->GetDirection() * ((20.0f + fSpeed) * TIME));

            // 
            //pCharger->SetDirection  (vDir);
            //pCharger->SetOrientation(vOri);
            pCharger->SetTexOffset  (vTex);

            // 
            pHull->SetPosition   (pCharger->GetPosition   ());
            pHull->SetSize       (pCharger->GetSize       () * (1.0f + 0.25f*fTime));
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

            const coreVector2 vLaserDir = -pMission->GetLaser(i)->GetDirection().xy();
            coreVector2 vNewPos = pDriver->GetPosition().xy() + vLaserDir * (20.0f * TIME);

                 if((vNewPos.x < -FOREGROUND_AREA.x * 1.2f) && (vLaserDir.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.4f;
            else if((vNewPos.x >  FOREGROUND_AREA.x * 1.2f) && (vLaserDir.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.4f;
                 if((vNewPos.y < -FOREGROUND_AREA.y * 1.2f) && (vLaserDir.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.4f;
            else if((vNewPos.y >  FOREGROUND_AREA.y * 1.2f) && (vLaserDir.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.4f;

            pDriver->SetPosition(coreVector3(vNewPos, 0.0f));

            // 
            pDriver->SetDirection  (coreVector3(coreVector2::Direction(pDriver->GetLifeTime() * (2.0f*PI)), 0.0f));
            //pDriver->SetOrientation(vOri);
            pDriver->SetTexOffset  (vTex);

            // 
            pHull->SetPosition   (pDriver->GetPosition   ());
            pHull->SetSize       (pDriver->GetSize       () * (1.0f + 0.25f*fTime));
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
            //pWaver->SetDirection  (vDir);
            //pWaver->SetOrientation(vOri);
            pWaver->SetTexOffset  (vTex);

            // 
            pHull->SetPosition   (pWaver->GetPosition   ());
            pHull->SetSize       (pWaver->GetSize       () * (1.0f + 0.25f*fTime));
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
        PHASE_CONTROL_TICKER(2u, 0u, 1.0f, LERP_LINEAR)
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
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, this, vPos + vTan, vDir)->ChangeSize(1.15f);
                }

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
            }
        });

        // 
        PHASE_CONTROL_TICKER(3u, 0u, 12.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
            {
                const cCustomEnemy* pGunner = &m_aGunner[i];
                if(!pGunner->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pGunner->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * (0.05f*PI));       

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.15f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_RED);
            }
        });

        // 
        PHASE_CONTROL_TICKER(4u, 0u, 30.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_CHARGERS; ++i)
            {
                const cCustomEnemy* pCharger = &m_aCharger[i];
                if(!pCharger->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pCharger->GetPosition().xy();
                const coreVector2 vDir = pCharger->AimAtPlayerDual(i % 2u).Normalized();

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.5f, this, vPos, vDir)->ChangeSize(1.6f);
            }
        });

        // 
        PHASE_CONTROL_TICKER(5u, 0u, 10.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_DRIVERS; ++i)
            {
                const cCustomEnemy* pDriver = &m_aDriver[i];
                if(!pDriver->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;


            }
        });

        // 
        PHASE_CONTROL_TICKER(6u, 0u, 0.9f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_WAVERS; ++i)
            {
                const cCustomEnemy* pWaver = &m_aWaver[i];
                if(!pWaver->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                for(coreUintW j = 6u; j--; )
                {
                    const coreVector2 vPos = pWaver->GetPosition().xy();
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 30.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, vPos,  vDir)->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, vPos, -vDir)->ChangeSize(1.5f);
                }

                // 
                g_pSpecialEffects->CreateSplashColor(pWaver->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
            }
        });
    }

    // constantly shoot into three directions
    PHASE_CONTROL_TICKER(7u, 0u, TORUS_BOSS_ROTATION * (2.0f / PI) * fShootSpeed, LERP_LINEAR)
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
            if(i == 0u || i == 4u) continue;

            coreVector2 avDir[3];

            // 
            avDir[0] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i - 2u) * 8.0f)) * mRota;
            avDir[1] = avDir[0].Rotated120();
            avDir[2] = avDir[1].Rotated120();

            for(coreUintW j = 0u; j < 3u; ++j)
            {
                const coreVector2 vDir = avDir[j];
                const coreVector2 vPos = this->GetPosition().xy() + vDir * (4.7f * this->GetSize().x);

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.5f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_YELLOW);
            }
        }
    });

    // 
    if(!coreMath::IsNear(ABS(vNewOri.z), 1.0f))
        m_aiCounter[EMIT_STATUS] = 0;

    // 
    m_Emitter.SetPosition   (this->GetPosition   ());
    m_Emitter.SetDirection  (this->GetDirection  ());
    m_Emitter.SetOrientation(this->GetOrientation());
    m_Emitter.SetAlpha      (MAX(m_aTimer[7].GetValue(CORE_TIMER_GET_NORMAL) * 2.0f - 1.0f, 0.0f));
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
            default: ASSERT(false)
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
}


// ****************************************************************
// 
void cTorusBoss::__EnableSummon(const coreVector2 vPosition, const coreVector3 vColor)
{
    WARN_IF(m_Summon.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

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
void cTorusBoss::__DisableSummon()
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
    pTurret->SetPosition(coreVector3(vPosition, 0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_CYAN);
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
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pTurret->GetPosition(), COLOR_ENERGY_CYAN);
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
    SET_BIT(m_iGunnerMove, iIndex, (iIndex < 2u))

    // 
    pGunner->SetPosition(coreVector3(vPosition, 0.0f));

    // 
    pHull->SetPosition(coreVector3(vPosition, 0.0f));
    pHull->SetEnabled (CORE_OBJECT_ENABLE_ALL);

    // 
    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPosition, 0.0f), COLOR_ENERGY_RED);
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
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pGunner->GetPosition(), COLOR_ENERGY_RED);
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
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pCharger->GetPosition(), COLOR_ENERGY_ORANGE);
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
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pDriver->GetPosition(), COLOR_ENERGY_PURPLE);
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
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pWaver->GetPosition(), COLOR_ENERGY_GREEN);
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