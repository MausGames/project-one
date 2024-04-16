///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// counter identifier
#define ROTATION_DIRECTION (0u)
#define EMIT_STATUS        (1u)
#define BALL_STATUS        (2u)
#define SKIP_SUMMON        (3u)
#define SUB_PHASE          (4u)


// ****************************************************************
// vector identifier
#define TUMBLE_DIRECTION (0u)




void __TorusCreateOverdrive(const coreUintW iIndex, const coreVector3& vIntersect, const coreFloat fTime, const coreBool bGround)
{
    
static coreVector3 vOldHit = coreVector3(0.0f,0.0f,0.0f);
static coreUint16 m_iDecalState = 0u;     


    ASSERT(iIndex < LEVIATHAN_RAYS)

    // 
    constexpr coreFloat fMin = 2.5f;
    constexpr coreFloat fMax = 5.0f;
    //coreVector3 vOldHit = m_avVector[OVERDRIVE_HIT + iIndex].xyz();

    // 
    if(vOldHit.IsNull()) vOldHit = vIntersect;
    else
    {
    gtScrewYou:

        // 
        const coreVector3 vDiff = vIntersect - vOldHit;
        const coreFloat   fLen  = vDiff.Length();

        // 
        if(fLen > fMin)
        {
            // 
            const coreVector3 vNewHit      = (fLen > fMax) ? LERP(vOldHit, vIntersect, fMax*RCP(fLen)) : vIntersect;
            const coreVector2 vOldOnScreen = g_pForeground->Project2D(vOldHit);
            const coreVector2 vNewOnScreen = g_pForeground->Project2D(vNewHit);

            // 
            if(((ABS(vOldOnScreen.x) < 0.55f) && (ABS(vOldOnScreen.y) < 0.55f)) ||
               ((ABS(vNewOnScreen.x) < 0.55f) && (ABS(vNewOnScreen.y) < 0.55f)))
            {
                STATIC_ASSERT(sizeof(m_iDecalState)*8u >= LEVIATHAN_RAYS*2u)

                // 
                if(CONTAINS_BIT(m_iDecalState, iIndex * 2u)) TOGGLE_BIT(m_iDecalState, iIndex * 2u + 1u)
                TOGGLE_BIT(m_iDecalState, iIndex * 2u)

                // 
                const coreBool    bRotated   = CONTAINS_BIT(m_iDecalState, iIndex * 2u);
                const coreBool    bFlipped   = CONTAINS_BIT(m_iDecalState, iIndex * 2u + 1u);
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

                // add object to background or windscreen
                if(bGround) g_pEnvironment->GetBackground()->AddDecal (pObject, vDecalPos,       128u, "effect_decal_single_inst_program", LIST_KEY);
                       else g_pWindscreen                  ->AddObject(pObject, vDecalPos, 3.0f, 128u, "effect_decal_single_inst_program", LIST_KEY);

                // 
                //g_pSpecialEffects->CreateSplashFire (vNewHit,  5.0f, bGround ? 3u : 6u, COLOR_FIRE_ORANGE);
                //g_pSpecialEffects->CreateSplashColor(vNewHit, 25.0f, bGround ? 2u : 4u, COLOR_FIRE_ORANGE);
            }

            // 
            //g_pSpecialEffects->ShakeScreen(0.1f + 0.55f * SIN(PI * fTime));

            // 
            vOldHit = vNewHit;
            goto gtScrewYou;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * Core::System->GetTime() * OUTDOOR_DETAIL;

    // 
    //m_avVector[OVERDRIVE_HIT + iIndex].xyz(vOldHit);
}


// ****************************************************************
// constructor
cTorusBoss::cTorusBoss()noexcept
: m_TurretHull      (TORUS_TURRETS)
, m_GunnerHull      (TORUS_GUNNERS)
, m_fAnimation      (0.0f)
, m_fRotationBoss   (-0.16f)   // # correction for aligning spear-bullets
, m_fRotationObject (0.0f)
, m_iTurretActive   (0u)
, m_iGunnerActive   (0u)
, m_iGunnerMove     (0u)
{
    // load models
    this->DefineModelHigh("ship_boss_torus_high.md3");
    this->DefineModelLow ("ship_boss_torus_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.7f,1.7f,1.7f));

    // configure the boss
    this->Configure(4000, COLOR_SHIP_GREY);

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
        m_aCircle[i].SetColor3    (COLOR_ENERGY_CYAN * 0.8f);
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
        m_aTurret[i].DefineModelHigh("object_cube.md3");
        m_aTurret[i].DefineModelLow ("object_cube.md3");
        m_aTurret[i].DefineTexture  (0u, "effect_energy.png");
        m_aTurret[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aTurret[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 2.85f);
        m_aTurret[i].SetTexSize     (coreVector2(0.8f,0.3f));
        m_aTurret[i].Configure      (100, COLOR_ENERGY_CYAN * 0.7f);
        m_aTurret[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_TurretHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            // load object resources
            coreObject3D* pTurret = &m_aTurretHullRaw[i];
            pTurret->DefineModel  ("object_cube.md3");
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
        m_aGunner[i].DefineModelHigh("object_tetra.md3");
        m_aGunner[i].DefineModelLow ("object_tetra.md3");
        m_aGunner[i].DefineTexture  (0u, "effect_energy.png");
        m_aGunner[i].DefineProgram  ("effect_energy_blink_invert_program");
        m_aGunner[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
        m_aGunner[i].SetTexSize     (coreVector2(0.8f,0.3f));
        m_aGunner[i].Configure      (100, COLOR_ENERGY_RED * 0.8f);
        m_aGunner[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_GunnerHull.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        {
            // load object resources
            coreObject3D* pGunner = &m_aGunnerHullRaw[i];
            pGunner->DefineModel  ("object_tetra.md3");
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

    STATIC_ASSERT(sizeof(m_iTurretActive)*8u >= TORUS_TURRETS)
    STATIC_ASSERT(sizeof(m_iGunnerActive)*8u >= TORUS_GUNNERS)
    STATIC_ASSERT(sizeof(m_iGunnerMove)  *8u >= TORUS_GUNNERS)
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

    // 
    constexpr coreUint8 aiNewOrder[] = {cSpearBullet::ID};
    g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
}


// ****************************************************************
// 
void cTorusBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    if(!m_aiCounter[BALL_STATUS])
    {
        // 
        pMission->EnableBall(0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized());
    }
    else
    {
        // 
        for(coreUintW i = 1u, ie = m_aiCounter[BALL_STATUS]; i < ie; ++i)
            pMission->DisableBall(i, bAnimated);
    }

    // 
    this->__DisableSummon();

    // 
    for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        this->__DisableTurret(i, bAnimated);

    // 
    for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
        this->__DisableGunner(i, bAnimated);

    // 
    g_pGlow->UnbindObject(&m_Emitter);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
        g_pGlow->UnbindObject(&m_aCircle[i]);

    // 
    g_pGlow->UnbindList(&m_TurretHull);
    g_pGlow->UnbindList(&m_GunnerHull);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnUnder()
{
    if(m_iTurretActive || m_iGunnerActive)
    {
        DEPTH_PUSH

        glDepthMask(false);
        {
            // 
            m_TurretHull.Render();
            m_GunnerHull.Render();
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
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    coreVector3 vNewOri = coreVector3(0.0f, 0.0f, m_aiCounter[ROTATION_DIRECTION] ? -1.0f : 1.0f);
    
    static coreVector3 vTestDir = coreVector3(0.0f,1.0f,0.0f);
    
    coreBool bFlip = false;
    
    static coreVector2 vOldEnvDir = coreVector2(0.0f,0.0f);

    // 
    m_fAnimation   .UpdateMod(-1.0f,               10.0f);
    m_fRotationBoss.UpdateMod(TORUS_BOSS_ROTATION, 2.0f*PI);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
#if 0
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.75f), fTime);

            if(PHASE_TIME_POINT(0.85f))
                this->_StartBoss();

            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                PHASE_CHANGE_TO(10u)
        });
#endif

        static coreFloat fCurHeight = 0.0f;
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) this->AddStatus(ENEMY_STATUS_GHOST);
            
            // TODO: spin faster at the end, like a real coin
            
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
            const coreVector2 vOffset = (ABS(vOri2.z) == 1.0f) ? coreVector2(0.0f,0.0f) : coreVector3::Cross(vOri2, coreVector3::Cross(vOri2, coreVector3(0.0f,0.0f,1.0f)).Normalized()).xy() * LERP(this->GetCollisionRange().x, 0.0f, ABS(vOri2.z));
            
            Core::Debug->InspectValue("dir", vOffset);
            const coreFloat fHeight = g_pEnvironment->RetrieveSafeHeight(vNewPos + vOffset) ;
            fCurHeight = LERP(fCurHeight, fHeight, 10.0f * Core::System->GetTime());
            this->SetPosition(coreVector3(vNewPos, fCurHeight + LERP(this->GetCollisionRange().x - 2.0f, this->GetCollisionRange().z + 1.0f*0.0f, ABS(vOri2.z))));
            
            if(fTime < 0.5f) __TorusCreateOverdrive(0u, coreVector3(vNewPos + vOffset, fCurHeight), fTime, true);
            
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


            //if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
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
                PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase >= 10u && m_iPhase <= 19u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            coreVector2 vPosFrom, vPosTo;
            coreFloat   fAngFrom, fAngTo;
            switch(m_iPhase)
            {
            default: ASSERT(false)
            case 10u: vPosFrom = m_vLastPosition;                               vPosTo = coreVector2( 1.0f, 1.0f).Normalized() * 0.65f; fAngFrom =  0.00f*PI; fAngTo = -0.25f*PI; break;
            case 11u: vPosFrom = coreVector2( 1.0f, 1.0f).Normalized() * 0.65f; vPosTo = coreVector2(-1.0f,-1.0f).Normalized() * 0.65f; fAngFrom = -0.25f*PI; fAngTo = -1.25f*PI; break;
            case 12u: vPosFrom = coreVector2(-1.0f,-1.0f).Normalized() * 0.65f; vPosTo = coreVector2(-1.0f, 1.0f).Normalized() * 0.65f; fAngFrom = -1.25f*PI; fAngTo = -1.75f*PI; break;
            case 13u: vPosFrom = coreVector2(-1.0f, 1.0f).Normalized() * 0.65f; vPosTo = coreVector2( 1.0f,-1.0f).Normalized() * 0.65f; fAngFrom = -1.75f*PI; fAngTo = -0.75f*PI; break;
            case 14u: vPosFrom = coreVector2( 1.0f,-1.0f).Normalized() * 0.65f; vPosTo = coreVector2( 1.0f, 1.0f).Normalized() * 0.65f; fAngFrom = -0.75f*PI; fAngTo = -0.25f*PI; break;
            }

            if(m_aiCounter[ROTATION_DIRECTION])
            {
                this->DefaultMoveLerp(vPosFrom.InvertedX(), vPosTo.InvertedX(), fTime);
                g_pEnvironment->SetTargetDirection(coreVector2::Direction(-LERP(fAngFrom, fAngTo, fTime)));
            }
            else
            {
                this->DefaultMoveLerp(vPosFrom, vPosTo, fTime);
                g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP(fAngFrom, fAngTo, fTime)));
            }

            if(m_aiCounter[SUB_PHASE])
            {
                vNewOri.yz(coreVector2::Direction(LERP(5.5f*PI, 5.0f*PI, fTime) + (m_aiCounter[ROTATION_DIRECTION] ? 0.0f : PI)));
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(m_iPhase + 10u)

                m_aiCounter[SKIP_SUMMON] = (CONTAINS_BIT(m_iGunnerActive, (m_iPhase - 20u) % TORUS_GUNNERS) ||
                                            CONTAINS_BIT(m_iTurretActive, (m_iPhase - 20u) % TORUS_TURRETS)) ? 1 : 0;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase >= 20u && m_iPhase <= 29u)
    {
        constexpr coreFloat  fDelay = 3.0f;
        constexpr coreUint16 iCount = 72u;
        constexpr coreUint16 iStop  = F_TO_UI(0.73f * I_TO_F(iCount));

        if(m_aiCounter[SKIP_SUMMON])
        {
            PHASE_CONTROL_PAUSE(0u, 1.0f/fDelay)
            {
                PHASE_CHANGE_TO((m_iPhase == 24u) ? 11u : (m_iPhase - 9u))
            });
        }
        else
        {
            PHASE_CONTROL_TIMER(1u, 1.0f/fDelay, LERP_LINEAR)
            {
                if(PHASE_BEGINNING) this->__EnableSummon(m_aCircle[0].GetPosition().xy(), m_aCircle[0].GetColor3());

                m_Summon.SetSize(coreVector3(2.5f,2.5f,2.5f) * LERPB(0.0f, 1.0f, MAX((fTime - 0.5f) / 0.5f, 0.0f)));
            });

            PHASE_CONTROL_TICKER(0u, iCount, I_TO_F(iCount)/fDelay, LERP_BREAK_REV)
            {
                if(iTick < iStop) g_pSpecialEffects->CreateChargeColor(this->GetPosition(), 50.0f, 2u, (m_aiCounter[SUB_PHASE] % 3) ? COLOR_ENERGY_RED : COLOR_ENERGY_CYAN);

                if(PHASE_FINISHED)
                {
                    PHASE_CHANGE_TO((m_iPhase == 24u) ? 11u : (m_iPhase - 9u))
                    PHASE_RESET(1u)

                    this->__DisableSummon();

                    if(m_aiCounter[SUB_PHASE] % 3) this->__EnableGunner((m_iPhase - 11u) % TORUS_GUNNERS, this->GetPosition().xy());
                                              else this->__EnableTurret((m_iPhase - 11u) % TORUS_TURRETS, this->GetPosition().xy());

                    //if(m_aiCounter[SUB_PHASE] >= 3)
                    //{
                    //    coreVector3 vNewColor;
                    //    switch(m_aiCounter[SUB_PHASE])
                    //    {
                    //    default: ASSERT(false)
                    //    case 7: m_aiCounter[SUB_PHASE] = 3; vNewColor = COLOR_ENERGY_CYAN * 0.8f; break;
                    //    case 3: m_aiCounter[SUB_PHASE] = 6; vNewColor = COLOR_ENERGY_CYAN * 0.8f; break;
                    //    case 6: m_aiCounter[SUB_PHASE] = 4; vNewColor = COLOR_ENERGY_RED  * 0.7f; break;
                    //    case 4: m_aiCounter[SUB_PHASE] = 7; vNewColor = COLOR_ENERGY_RED  * 0.7f; break;
                    //    }
                    //
                    //    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
                    //        m_aCircle[i].SetColor3(vNewColor);
                    //}
                }
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            vNewOri.yz(coreVector2::Direction(LERP(4.5f*PI, 4.0f*PI, fTime)));

            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.0f), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        constexpr coreFloat  fDelay = 4.0f;
        constexpr coreUint16 iCount = 100u;
        constexpr coreUint16 iStop  = F_TO_UI(0.73f * I_TO_F(iCount));

        PHASE_CONTROL_TIMER(1u, 1.0f/fDelay, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) this->__EnableSummon(m_aCircle[0].GetPosition().xy(), m_aCircle[0].GetColor3());

            m_Summon.SetSize(coreVector3(2.5f,2.5f,2.5f) * LERPB(0.0f, 1.0f, MAX((fTime - 0.5f) / 0.5f, 0.0f)));
        });

        PHASE_CONTROL_TICKER(0u, iCount, I_TO_F(iCount)/fDelay, LERP_BREAK_REV)
        {
            if(iTick < iStop) g_pSpecialEffects->CreateChargeColor(this->GetPosition(), 50.0f, 2u, COLOR_ENERGY_GREEN);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->__DisableSummon();

                for(coreUintW i = 0u, ie = /*((g_pGame->GetDifficulty() > 0) ? 2u : 1u)*/1u; i < ie; ++i)
                {
                    const coreVector2 vDir = (i ? coreVector2(1.0f,0.5f) : coreVector2(-0.5f,1.0f)).Normalized();
                    pMission->EnableBall(i, this->GetPosition().xy(), vDir);

                    m_aiCounter[BALL_STATUS] += 1;
                }
                
                bFlip = true;

                g_pSpecialEffects->MacroExplosionColorSmall(this->GetPosition(), COLOR_ENERGY_GREEN);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 52u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            //PHASE_CHANGE_INC
            PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 53u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.65f), fTime);

            if(PHASE_FINISHED)
                //PHASE_CHANGE_TO(60u)
                PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase >= 60u && m_iPhase <= 63u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            coreFloat fAngFrom, fAngTo;
            switch(m_iPhase)
            {
            default: ASSERT(false)
            case 60u: fAngFrom =  0.0f*PI; fAngTo = 1.5f*PI; break;
            case 61u: fAngFrom = -0.5f*PI; fAngTo = 1.0f*PI; break;
            case 62u: fAngFrom = -1.0f*PI; fAngTo = 0.5f*PI; break;
            case 63u: fAngFrom = -1.5f*PI; fAngTo = 0.0f*PI; break;
            }

            const coreVector2 vDirection = coreVector2::Direction(LERP(fAngFrom, fAngTo, fTime));

            this->SetPosition(coreVector3(vDirection * (0.65f * FOREGROUND_AREA), 0.0f));
            g_pEnvironment->SetTargetDirection(vDirection);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(m_iPhase + 4u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase >= 64u && m_iPhase <= 67u)
    {
        PHASE_CONTROL_TICKER(0u, 36u, 12.0f, LERP_LINEAR)
        {
            const coreFloat fAngle = I_TO_F(iTick) * 12.0f;

            for(coreUintW i = 4u; i--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 90.0f + fAngle));
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, this, this->GetPosition().xy(), vDir)->ChangeSize(1.3f);
            }

            g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 5.0f, 1u, COLOR_ENERGY_BLUE);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO((m_iPhase == 67u) ? 60u : (m_iPhase - 3u))
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fBreakTime  = LERPB(0.0f, 1.0f, fTime);
            const coreFloat fSmoothTime = LERPS(0.0f, 1.0f, fTime);

            vNewOri.yz(coreVector2::Direction((5.5f*PI) * fBreakTime + (m_aiCounter[ROTATION_DIRECTION] ? PI : 0.0f)));

            this->DefaultMoveLerp(m_vLastPosition, m_vLastPosition + m_avVector[TUMBLE_DIRECTION].xy(), fBreakTime);
            g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP(m_avVector[TUMBLE_DIRECTION].z, 0.0f*PI, fBreakTime)));

            coreVector3 vNewColor;
            switch(m_aiCounter[SUB_PHASE])
            {
            default: ASSERT(false)
            case 1: vNewColor = TernaryLerp(COLOR_ENERGY_CYAN  * 0.8f, COLOR_ENERGY_WHITE * 0.3f, COLOR_ENERGY_RED   * 0.7f, fSmoothTime); break;
            case 2: vNewColor = TernaryLerp(COLOR_ENERGY_RED   * 0.7f, COLOR_ENERGY_WHITE * 0.3f, COLOR_ENERGY_GREEN * 0.8f, fSmoothTime); break;
            case 3: vNewColor = TernaryLerp(COLOR_ENERGY_GREEN * 0.8f, COLOR_ENERGY_WHITE * 0.3f, COLOR_ENERGY_CYAN  * 0.8f, fSmoothTime); break;
            }

            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCircle); ++i)
                m_aCircle[i].SetColor3(vNewColor);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO((m_aiCounter[SUB_PHASE] == 2) ? 50u : 10u)

                m_aiCounter[ROTATION_DIRECTION] = 1 - m_aiCounter[ROTATION_DIRECTION];
                if(m_aiCounter[ROTATION_DIRECTION]) this->StorePosition(this->GetPosition().xy().InvertedX());
            }
        });
    }

    // ################################################################
    // ################################################################

    
    coreVector2 vNewEnvDir = coreVector2(0.0f,0.0f);
    
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
        
        
        //g_pEnvironment->SetTargetDirection(vNewDir.InvertedY());
        //vNewEnvDir = vNewDir.InvertedY();
    }
    vNewEnvDir = g_pEnvironment->GetDirection();
    
    

    if(m_iTurretActive || m_iGunnerActive)
    {
        // 
        m_fRotationObject.UpdateMod(TORUS_TURRET_SPEED, 10.0f);

        // 
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(m_fRotationObject * PI), 0.0f);
        const coreVector3 vOri  = coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y);
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
            pHull->SetPosition   (pTurret->GetPosition   ()); // ###   
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
            const coreFloat   fMoveSide = CONTAINS_BIT(m_iGunnerMove, i) ? -1.0f : 1.0f;
            const coreVector2 vMoveDir  = coreVector2(fMoveSide * FOREGROUND_AREA.x * TORUS_GUNNER_SPEED * Core::System->GetTime(), 0.0f);
            const coreVector2 vNewPos   = pGunner->GetPosition().xy() + ((i & 0x02u) ? vMoveDir.yx() : vMoveDir);

            // 
            const auto nBounceEffect = [](const coreVector2& vEffectPos)
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
            pGunner->SetPosition   (coreVector3(vNewPos, 0.0f));
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
        m_TurretHull.MoveNormal();
        m_GunnerHull.MoveNormal();

        // 
        PHASE_CONTROL_TICKER(2u, 0u, 10.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
            {
                cCustomEnemy* pTurret = &m_aTurret[i];
                if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pTurret->GetPosition ().xy();
                const coreVector2 vDir = pTurret->GetDirection().xy();

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.15f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_CYAN);
            }
        });

        // 
        PHASE_CONTROL_TICKER(3u, 0u, 10.0f, LERP_LINEAR)
        {
            // 
            for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
            {
                 #if 0
                cCustomEnemy* pGunner = &m_aGunner[i];
                if(!pGunner->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pGunner->GetPosition ().xy();
                const coreVector2 vDir = pGunner->GetDirection().xy();

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.15f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_RED);
                #endif
                cCustomEnemy* pGunner = &m_aGunner[i];
                if(!pGunner->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vPos = pGunner->GetPosition().xy();
                const coreVector2 vDir = (i & 0x02u) ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,1.0f);

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.15f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.15f);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_RED);
            }
        });
    }

    // constantly shoot into three directions
    PHASE_CONTROL_TICKER(4u, 0u, TORUS_BOSS_ROTATION * (2.0f / PI), LERP_LINEAR)
    {
        // 
        if(!m_aiCounter[EMIT_STATUS])
        {
            m_aiCounter[EMIT_STATUS] = 1;
            return;
        }

        // 
        const coreMatrix2 vMain = coreMatrix3::Rotation(DEG_TO_RAD(270.0f + I_TO_F(iTick & 0x03u) * (m_aiCounter[ROTATION_DIRECTION] ? -90.0f : 90.0f))).m12();
        const coreMatrix2 vSub  = coreMatrix3::Rotation(DEG_TO_RAD(120.0f)).m12();

        for(coreUintW i = 5u; i--; )
        {
            //if(i == 2u) continue;

            coreVector2 avDir[3];

            // 
            avDir[0] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i-2u) * 7.0f)) * vMain;
            avDir[1] = avDir[0] * vSub;
            avDir[2] = avDir[1] * vSub;

            for(coreUintW j = 0u; j < 3u; ++j)
            {
                const coreVector2 vDir = avDir[j];
                const coreVector2 vPos = this->GetPosition().xy() + vDir * 8.0f;

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.35f);

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
    m_Emitter.SetAlpha      (MAX(m_aTimer[4].GetValue(CORE_TIMER_GET_NORMAL) * 2.0f - 1.0f, 0.0f));
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

    // 
    if(this->ReachedHealthPct(0.7f) || this->ReachedHealthPct(0.4f) || bFlip)
    {
        // 
        m_aiCounter[SUB_PHASE] += 1;

        // 
        this->__DisableSummon();

        // 
        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
            this->__DisableTurret(i, true);

        // 
        for(coreUintW i = 0u; i < TORUS_GUNNERS; ++i)
            this->__DisableGunner(i, true);

        // 
        m_avVector[TUMBLE_DIRECTION].xy((bFlip ? coreVector2(0.5f,-1.0f).Normalized() : (this->GetMove()) * 1.5f));
        m_avVector[TUMBLE_DIRECTION].z = g_pEnvironment->GetDirection().Angle();

        // 
        PHASE_CHANGE_TO(70u)
        PHASE_RESET(0u)
        PHASE_RESET(1u)
        PHASE_RESET(2u)
    }

    
    
    coreObject3D* pBall = pMission->GetBall(0u);
    if(pBall->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
       // pBall->SetPosition(this->GetPosition());
        
        
        //m_fRotationObject.UpdateMod(TORUS_TURRET_SPEED, 10.0f);
        if(false) PHASE_CONTROL_TICKER(5u, 0u, 10.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = pBall->GetPosition ().xy();
            const coreVector2 vDir  = coreVector2::Direction(m_fRotationObject * PI);
    
            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.4f);
    
            // 
            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_GREEN);
        });
    }
    
        //m_fRotationObject.UpdateMod(TORUS_TURRET_SPEED, 10.0f);
    if(false) PHASE_CONTROL_TICKER(5u, 0u, 10.0f, LERP_LINEAR)
    {
        // 
        const coreObject3D* pBall = pMission->GetBall(0u);
        if(!pBall->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) return;

            const coreVector2 vPos = pBall->GetPosition ().xy();
            //const coreVector2 vDir = pBall->GetDirection().xy();
            const coreVector2 vDir  = coreVector2::Direction(m_fRotationObject * PI);

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.4f);

            // 
            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_GREEN);

    });

#if 1
    if(pMission->GetBounceState())
    {
        //const coreVector2 vPos = pMission->GetBall(0u)->GetPosition().xy();

        /*
        for(coreUintW i = 40u; i--; )
        {
            if((i % 8u) < 4u) continue;

            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 4.5f));

            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vPos,  vDir)->ChangeSize(1.4f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vPos, -vDir)->ChangeSize(1.4f);
            // TODO: lower purple balls get overlapped   
        }
         */
        
        //const coreVector2 vDir = IsHorizontal(vPos) ? coreVector2(-SIGN(vPos.x), 0.0f) : coreVector2(0.0f, -SIGN(vPos.y));
        //const coreVector2 vTan = vDir.Rotated90();
        
        for(coreUintW i = 4u; i--; )
        {
        //    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vPos + (I_TO_F(i)-1.5f) * 3.0f * vTan, vDir)->ChangeSize(1.4f);
        }
    }
#endif


        const coreFloat A = vOldEnvDir.Angle();
        const coreFloat B = vNewEnvDir.Angle();
        const coreFloat fAngleDiff = B - A;

        for(coreUintW i = 0u; i < TORUS_TURRETS; ++i)
        {
            cCustomEnemy* pTurret = &m_aTurret       [i];
            //coreObject3D* pHull   = &m_aTurretHullRaw[i];
            if(!pTurret->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
            
            const coreVector2 vPos = pTurret->GetPosition().xy();
            const coreFloat   fLen = vPos.Length();
            
            //const coreVector2 A = MapToAxis(vOldEnvDir, vPos.Normalized());
            //const coreVector2 B = MapToAxis(vNewEnvDir, vPos.Normalized());

            // 
            //pTurret->SetPosition(coreVector3(vPos + (B - A) * fLen, 0.0f));
            
            const coreFloat C = vPos.Angle();
            
            pTurret->SetPosition(coreVector3(coreVector2::Direction(C + fAngleDiff) * fLen, 0.0f));
            
            if(!i)
            {
             Core::Debug->InspectValue("A", A);   
             Core::Debug->InspectValue("B", B);   
             Core::Debug->InspectValue("C", C);   
            }
        }
        
        
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
        {
            const coreVector2 vPos = pBullet->GetPosition().xy();
            const coreFloat   fLen = vPos.Length();
            
            const coreFloat C = vPos.Angle();
            
            pBullet->SetPosition(coreVector3(coreVector2::Direction(C + fAngleDiff) * fLen, 0.0f));
            pBullet->SetFlyDir(coreVector2::Direction(pBullet->GetFlyDir().Angle() + fAngleDiff));
        });

        
    vOldEnvDir = vNewEnvDir;
    
}


// ****************************************************************
// 
void cTorusBoss::__EnableSummon(const coreVector2& vPosition, const coreVector3& vColor)
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
void cTorusBoss::__EnableTurret(const coreUintW iIndex, const coreVector2& vPosition)
{
    // 
    ASSERT(iIndex < TORUS_TURRETS)
    cCustomEnemy* pTurret = &m_aTurret       [iIndex];
    coreObject3D* pHull   = &m_aTurretHullRaw[iIndex];

    // 
    if(!CONTAINS_FLAG(pTurret->GetStatus(), ENEMY_STATUS_DEAD)) return;
    pTurret->Resurrect();

    // 
    ASSERT(!CONTAINS_BIT(m_iTurretActive, iIndex))
    ADD_BIT(m_iTurretActive, iIndex)

    // 
    pTurret->SetPosition (coreVector3(vPosition, 0.0f));
    pTurret->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

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
    // 
    ASSERT(iIndex < TORUS_TURRETS)
    cCustomEnemy* pTurret = &m_aTurret       [iIndex];
    coreObject3D* pHull   = &m_aTurretHullRaw[iIndex];

    // 
    if(CONTAINS_FLAG(pTurret->GetStatus(), ENEMY_STATUS_DEAD)) return;
    pTurret->Kill(false);

    // 
    ASSERT(CONTAINS_BIT(m_iTurretActive, iIndex))
    REMOVE_BIT(m_iTurretActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pTurret->GetPosition(), COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cTorusBoss::__EnableGunner(const coreUintW iIndex, const coreVector2& vPosition)
{
    // 
    ASSERT(iIndex < TORUS_GUNNERS)
    cCustomEnemy* pGunner = &m_aGunner       [iIndex];
    coreObject3D* pHull   = &m_aGunnerHullRaw[iIndex];

    // 
    if(!CONTAINS_FLAG(pGunner->GetStatus(), ENEMY_STATUS_DEAD)) return;
    pGunner->Resurrect();

    // 
    ASSERT(!CONTAINS_BIT(m_iGunnerActive, iIndex))
    ADD_BIT(m_iGunnerActive, iIndex)

    // 
    SET_BIT(m_iGunnerMove, iIndex, (iIndex < 2u))

    // 
    pGunner->SetPosition (coreVector3(vPosition, 0.0f));
    pGunner->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

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
    // 
    ASSERT(iIndex < TORUS_GUNNERS)
    cCustomEnemy* pGunner = &m_aGunner       [iIndex];
    coreObject3D* pHull   = &m_aGunnerHullRaw[iIndex];

    // 
    if(CONTAINS_FLAG(pGunner->GetStatus(), ENEMY_STATUS_DEAD)) return;
    pGunner->Kill(false);

    // 
    ASSERT(CONTAINS_BIT(m_iGunnerActive, iIndex))
    REMOVE_BIT(m_iGunnerActive, iIndex)

    // 
    pHull->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pGunner->GetPosition(), COLOR_ENERGY_RED);
}