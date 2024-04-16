///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cViridoMission::cViridoMission()noexcept
: m_Ball            (VIRIDO_BALLS)
, m_BallTrail       (VIRIDO_BALLS * VIRIDO_TRAILS)
, m_apPaddleOwner   {}
, m_Barrier1        (VIRIDO_BARRIERS_FIRST)
, m_Barrier2        (VIRIDO_BARRIERS_SECOND)
, m_apBarrierOwner  {}
, m_avBarrierPos    {}
, m_avBarrierDir    {}
, m_fBarrierRange   (1.0f)
, m_bBarrierSlow    (false)
, m_bBarrierClamp   (true)
, m_bBarrierReflect (true)
, m_iBarrierBounce  (0u)
, m_Laser           (VIRIDO_LASERS)
, m_LaserWave       (VIRIDO_LASERS)
, m_apLaserOwner    {}
, m_avLaserPos      {}
, m_avLaserDir      {}
, m_afLaserTick     {}
, m_iLaserTouch     (0u)
, m_iLaserIgnore    (0u)
, m_bLaserCross     (true)
, m_Shadow          (VIRIDO_SHADOWS)
, m_apShadowOwner   {}
, m_iShadowType     (0u)
, m_Hint            (VIRIDO_HINTS)
, m_aiHintBarrier   {}
, m_iHintActive     (0u)
, m_Bean            (VIRIDO_BEANS)
, m_BeanWave        (VIRIDO_BEANS)
, m_aiDrumCount     {}
, m_aiDrumIndex     {}
, m_fPoleCount      (0.0f)
, m_iPoleIndex      (UINT8_MAX)
, m_iRealState      (0u)
, m_iStickyState    (0u)
, m_iBounceState    (0u)
, m_fAnimation      (0.0f)
, m_bStory          (!HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiState, STATE_STORY_VIRIDO))
{
    // 
    m_apBoss[0] = &m_Torus;

    // create ball lists
    m_Ball     .DefineProgram("effect_energy_invert_inst_program");
    m_BallTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_BALLS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (VIRIDO_TRAILS + 1u);

            // load object resources
            coreObject3D* pBall = &m_aBallRaw[i];
            pBall->DefineModel  ("object_sphere.md3");
            pBall->DefineTexture(0u, "effect_energy.png");
            pBall->DefineProgram("effect_energy_invert_program");

            // set object properties
            pBall->SetSize   (coreVector3(3.1f,3.1f,3.1f));
            pBall->SetColor3 (COLOR_ENERGY_GREEN * 0.8f);
            pBall->SetTexSize(coreVector2(4.5f,4.5f));
            pBall->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BallTrail.BindObject(pBall);
                 else m_Ball     .BindObject(pBall);
        }
    }

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
    {
        // 
        const coreBool bBoss = i ? false : true;

        // 
        m_aPaddle[i].DefineModel         ("object_paddle.md3");
        m_aPaddle[i].DefineTexture       (0u, "effect_energy.png");
        m_aPaddle[i].DefineProgram       ("effect_energy_flat_direct_program");
        m_aPaddle[i].SetSize             (bBoss ? coreVector3(3.5f,2.5f,2.5f) : coreVector3(2.5f,2.5f,2.5f));
        m_aPaddle[i].SetColor3           (bBoss ? COLOR_ENERGY_BLUE           : COLOR_ENERGY_RED);
        m_aPaddle[i].SetTexSize          (coreVector2(1.2f,0.25f) * 0.5f);
        m_aPaddle[i].SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);
        m_aPaddle[i].SetCollisionModifier(coreVector3(0.6f,1.0f,1.0f));

        // 
        m_aPaddleSphere[i].DefineModel("object_sphere.md3");
        m_aPaddleSphere[i].SetSize    (bBoss ? coreVector3(30.0f,30.0f,30.0f) : coreVector3(15.0f,15.0f,15.0f));
    }

    // 
    m_Barrier1.DefineProgram("effect_energy_flat_direct_inst_program");
    m_Barrier2.DefineProgram("effect_energy_flat_direct_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_BARRIERS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pBarrier = &m_aBarrierRaw[i];
            pBarrier->DefineModel  ("object_barrier.md3");
            pBarrier->DefineTexture(0u, "effect_energy.png");
            pBarrier->DefineProgram("effect_energy_flat_direct_program");

            // set object properties
            pBarrier->SetColor3 (COLOR_ENERGY_BLUE);
            pBarrier->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(i < VIRIDO_BARRIERS_FIRST) m_Barrier1.BindObject(pBarrier);
                                     else m_Barrier2.BindObject(pBarrier);
        }
    }

    // 
    m_Laser    .DefineProgram("effect_energy_flat_inst_program");
    m_LaserWave.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_LASERS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pLaser = &m_aLaserRaw[i];
            pLaser->DefineModel  ("object_tube.md3");
            pLaser->DefineTexture(0u, "effect_energy.png");
            pLaser->DefineProgram("effect_energy_flat_program");

            // set object properties
            pLaser->SetSize   (coreVector3(0.6f,160.0f,0.6f));
            pLaser->SetColor3 (COLOR_ENERGY_PURPLE * (iType ? 0.7f : 1.0f));
            pLaser->SetTexSize(coreVector2(0.5f,40.0f) * 0.25f);
            pLaser->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_LaserWave.BindObject(pLaser);
                 else m_Laser    .BindObject(pLaser);
        }
    }

    // 
    m_Shadow.DefineProgram("effect_decal_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_SHADOWS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pShadow = &m_aShadowRaw[i];
            pShadow->DefineModel  (Core::Manager::Object->GetLowQuad());
            pShadow->DefineTexture(0u, "effect_aim.png");
            pShadow->DefineProgram("effect_decal_program");

            // set object properties
            pShadow->SetTexSize(coreVector2(0.5f,1.0f));
            pShadow->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Shadow.BindObject(pShadow);
        }
    }

    // 
    m_Hint.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_HINTS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pHint = &m_aHintRaw[i];
            pHint->DefineModel  ("object_arrow_short.md3");
            pHint->DefineTexture(0u, "effect_energy.png");
            pHint->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pHint->SetColor3 (COLOR_ENERGY_BLUE);
            pHint->SetTexSize(coreVector2(0.5f,0.2f) * 1.2f);
            pHint->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Hint.BindObject(pHint);
        }
    }

    // 
    m_Bean    .DefineProgram("effect_energy_flat_inst_program");
    m_BeanWave.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_BEANS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pBean = &m_aBeanRaw[i];
            pBean->DefineModel  ("object_cube_top.md3");
            pBean->DefineTexture(0u, "effect_energy.png");
            pBean->DefineProgram("effect_energy_flat_program");

            // set object properties
            pBean->SetSize   (coreVector3(1.0f,1.0f,1.0f) * (iType ? 3.7f : 2.7f));
            pBean->SetColor3 (COLOR_ENERGY_MAGENTA * (iType ? 0.6f : 1.0f));
            pBean->SetTexSize(coreVector2(1.0f,1.0f) * 0.4f);
            pBean->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BeanWave.BindObject(pBean);
                 else m_Bean    .BindObject(pBean);
        }
    }

    // 
    m_Globe.DefineModelHigh("object_tetra_top.md3");
    m_Globe.DefineModelLow ("object_tetra_top.md3");
    m_Globe.DefineVolume   ("object_tetra_volume.md3");
    m_Globe.DefineTexture  (0u, "effect_energy.png");
    m_Globe.DefineProgram  ("effect_energy_flat_invert_program");
    m_Globe.SetTexSize     (coreVector2(1.0f,1.0f) * 0.4f);
    m_Globe.Configure      (1000, 0u, COLOR_ENERGY_PURPLE * 0.9f);
    m_Globe.AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT);

    // 
    m_GlobeWave.DefineModel  ("object_tetra_top.md3");
    m_GlobeWave.DefineTexture(0u, "effect_energy.png");
    m_GlobeWave.DefineProgram("effect_energy_flat_invert_program");
    m_GlobeWave.SetColor3    (COLOR_ENERGY_PURPLE * 0.6f);
    m_GlobeWave.SetTexSize   (coreVector2(1.0f,1.0f) * 0.4f);
    m_GlobeWave.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Target.DefineModel  ("object_sphere.md3");
    m_Target.DefineTexture(0u, "effect_energy.png");
    m_Target.DefineProgram("effect_energy_flat_invert_program");
    m_Target.SetColor3    (COLOR_ENERGY_CYAN * 0.6f);
    m_Target.SetTexSize   (coreVector2(1.0f,1.0f) * 3.5f);
    m_Target.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTargetWave); ++i)
    {
        m_aTargetWave[i].DefineModel  ("object_sphere.md3");
        m_aTargetWave[i].DefineTexture(0u, "effect_energy.png");
        m_aTargetWave[i].DefineProgram("effect_energy_flat_spheric_program");
        m_aTargetWave[i].SetColor3    (COLOR_ENERGY_CYAN * 0.6f);
        m_aTargetWave[i].SetTexSize   (coreVector2(1.0f,1.0f) * 3.5f);
        m_aTargetWave[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    g_pGlow->BindList(&m_Ball);
    g_pGlow->BindList(&m_BallTrail);
    g_pGlow->BindList(&m_Barrier1);
    g_pGlow->BindList(&m_Barrier2);
    g_pGlow->BindList(&m_Laser);
    g_pGlow->BindList(&m_LaserWave);
    g_pGlow->BindList(&m_Hint);
    g_pGlow->BindList(&m_Bean);
    g_pGlow->BindList(&m_BeanWave);

    // 
    for(coreUintW i = 0u; i < VIRIDO_DRUMS; ++i)
        m_aiDrumIndex[i] = UINT8_MAX;
}


// ****************************************************************
// destructor
cViridoMission::~cViridoMission()
{
    // 
    g_pGlow->UnbindList(&m_Ball);
    g_pGlow->UnbindList(&m_BallTrail);
    g_pGlow->UnbindList(&m_Barrier1);
    g_pGlow->UnbindList(&m_Barrier2);
    g_pGlow->UnbindList(&m_Laser);
    g_pGlow->UnbindList(&m_LaserWave);
    g_pGlow->UnbindList(&m_Hint);
    g_pGlow->UnbindList(&m_Bean);
    g_pGlow->UnbindList(&m_BeanWave);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS;    ++i) this->DisableBall   (i, false);
    for(coreUintW i = 0u; i < VIRIDO_PADDLES;  ++i) this->DisablePaddle (i, false);
    for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i) this->DisableBarrier(i, false);
    for(coreUintW i = 0u; i < VIRIDO_LASERS;   ++i) this->DisableLaser  (i, false);
    for(coreUintW i = 0u; i < VIRIDO_SHADOWS;  ++i) this->DisableShadow (i, false);
    for(coreUintW i = 0u; i < VIRIDO_HINTS;    ++i) this->DisableHint   (i, false);
    for(coreUintW i = 0u; i < VIRIDO_BEANS;    ++i) this->DisableBean   (i, false);
    this->DisableGlobe (false);
    this->DisableTarget(false);
}


// ****************************************************************
// 
void cViridoMission::EnableBall(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < VIRIDO_BALLS)
    coreObject3D* pBall  = (*m_Ball     .List())[iIndex];
    coreObject3D* pTrail = (*m_BallTrail.List())[iIndex*VIRIDO_TRAILS];

    // 
    WARN_IF(pBall->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBall(iIndex, false);
    pBall->ChangeType(TYPE_VIRIDO_BALL);

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject, const coreFloat fAlpha)
    {
        pObject->SetPosition (coreVector3(vPosition,  0.0f));
        pObject->SetDirection(coreVector3(vDirection, 0.0f));
        pObject->SetAlpha    (fAlpha);
        pObject->SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pBall, 1.0f);
    for(coreUintW i = 0u; i < VIRIDO_TRAILS; ++i) nInitFunc(pTrail + i, 1.0f - I_TO_F(i+1u) / I_TO_F(VIRIDO_TRAILS));

    // 
    g_pSpecialEffects->MacroEruptionColorSmall(coreVector3(vPosition, 0.0f), vDirection, COLOR_ENERGY_GREEN);
}


// ****************************************************************
// 
void cViridoMission::DisableBall(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_BALLS)
    coreObject3D* pBall  = (*m_Ball     .List())[iIndex];
    coreObject3D* pTrail = (*m_BallTrail.List())[iIndex*VIRIDO_TRAILS];

    // 
    if(!pBall->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    pBall->ChangeType(0);

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitFunc(pBall);
    for(coreUintW i = 0u; i < VIRIDO_TRAILS; ++i) nExitFunc(pTrail + i);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pBall->GetPosition(), COLOR_ENERGY_GREEN);
}


// ****************************************************************
// 
void cViridoMission::EnablePaddle(const coreUintW iIndex, const cShip* pOwner)
{
    ASSERT(iIndex < VIRIDO_PADDLES)
    coreObject3D& oPaddle = m_aPaddle[iIndex];

    // 
    WARN_IF(oPaddle.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisablePaddle(iIndex, false);
    oPaddle.ChangeType(TYPE_VIRIDO_PADDLE);

    // 
    ASSERT(pOwner)
    m_apPaddleOwner[iIndex] = pOwner;

    // 
    oPaddle.SetAlpha  (0.0f);
    oPaddle.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&oPaddle);
}


// ****************************************************************
// 
void cViridoMission::DisablePaddle(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_PADDLES)
    coreObject3D& oPaddle = m_aPaddle[iIndex];

    // 
    if(!oPaddle.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oPaddle.ChangeType(0);

    // 
    m_apPaddleOwner[iIndex] = NULL;

    // 
    if(!bAnimated)
    {
        oPaddle.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&oPaddle);
    }
}


// ****************************************************************
// 
void cViridoMission::EnableBarrier(const coreUintW iIndex, const cShip* pOwner, const coreVector2 vDirection, const coreFloat fScale)
{
    ASSERT(iIndex < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

    // 
    WARN_IF(oBarrier.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBarrier(iIndex, false);

    // 
    ASSERT(pOwner)
    m_apBarrierOwner[iIndex] = pOwner;
    m_avBarrierPos  [iIndex] = HIDDEN_POS;
    m_avBarrierDir  [iIndex] = vDirection;

    // 
    oBarrier.SetPosition (coreVector3(HIDDEN_POS, 0.0f));
    oBarrier.SetDirection(coreVector3(vDirection, 0.0f));
    oBarrier.SetAlpha    (0.0f);
    oBarrier.SetEnabled  (CORE_OBJECT_ENABLE_ALL);

    // 
    this->SetBarrierScale(iIndex, fScale);
}


// ****************************************************************
// 
void cViridoMission::DisableBarrier(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

    // 
    if(!oBarrier.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_apBarrierOwner[iIndex] = NULL;

    // 
    if(!bAnimated)
    {
        // 
        for(coreUintW i = 0u; i < VIRIDO_DRUMS; ++i)
        {
            if(iIndex == m_aiDrumIndex[i]) this->EndDrumBeat(i, false);
        }

        // 
        oBarrier.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cViridoMission::EnableLaser(const coreUintW iIndex, const cShip* pOwner)
{
    ASSERT(iIndex < VIRIDO_LASERS)
    coreObject3D* pLaser = (*m_Laser    .List())[iIndex];
    coreObject3D* pWave  = (*m_LaserWave.List())[iIndex];

    // 
    WARN_IF(pLaser->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableLaser(iIndex, false);

    // 
    ASSERT(pOwner)
    m_apLaserOwner[iIndex] = pOwner;
    m_avLaserPos  [iIndex] = HIDDEN_POS;
    m_avLaserDir  [iIndex] = coreVector2(0.0f,1.0f);

    // 
    pLaser->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cViridoMission::DisableLaser(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_LASERS)
    coreObject3D* pLaser = (*m_Laser    .List())[iIndex];
    coreObject3D* pWave  = (*m_LaserWave.List())[iIndex];

    // 
    if(!pLaser->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(iIndex == m_iPoleIndex) this->EndPoleDance(false);

    // 
    m_apLaserOwner[iIndex] = NULL;

    // 
    pLaser->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) for(coreUintW j = 100u; j--; ) g_pSpecialEffects->CreateSplashColor(pLaser->GetPosition() + pLaser->GetDirection() * (2.0f * (I_TO_F(j) - 49.5f)), 10.0f, 1u, COLOR_ENERGY_PURPLE);
}


// ****************************************************************
// 
void cViridoMission::EnableShadow(const coreUintW iIndex, const cShip* pOwner, const coreVector2 vPosition, const coreBool bQuad)
{
    ASSERT(iIndex < VIRIDO_SHADOWS)
    coreObject3D& oShadow = m_aShadowRaw[iIndex];

    // 
    WARN_IF(oShadow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableShadow(iIndex, false);

    // 
    ASSERT(pOwner)
    m_apShadowOwner[iIndex] = pOwner;

    // 
    SET_BIT(m_iShadowType, iIndex, bQuad)
    STATIC_ASSERT(VIRIDO_SHADOWS < sizeof(m_iShadowType)*8u)

    // 
    oShadow.SetPosition (coreVector3(vPosition,0.0f));
    oShadow.SetSize     (coreVector3(0.0f,0.0f,0.0f));
    oShadow.SetTexOffset(coreVector2(bQuad ? 0.5f : 0.0f, 0.0f));
    oShadow.SetColor3   (bQuad ? coreVector3(1.0f,0.85f,0.1f) : coreVector3(1.0f,1.0f,1.0f));
    oShadow.SetAlpha    (0.8f);
    oShadow.SetEnabled  (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cViridoMission::DisableShadow(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_SHADOWS)
    coreObject3D& oShadow = m_aShadowRaw[iIndex];

    // 
    if(!oShadow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_apShadowOwner[iIndex] = NULL;

    // 
    if(!bAnimated) oShadow.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cViridoMission::EnableHint(const coreUintW iIndex, const coreUintW iBarrier)
{
    ASSERT(iIndex < VIRIDO_HINTS)
    coreObject3D& oHint = m_aHintRaw[iIndex];

    // 
    WARN_IF(oHint.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableHint(iIndex, false);

    // 
    m_aiHintBarrier[iIndex] = iBarrier;

    // 
    REMOVE_BIT(m_iHintActive, iIndex)
    STATIC_ASSERT(VIRIDO_HINTS <= sizeof(m_iHintActive)*8u)

    // 
    oHint.SetAlpha  (0.0f);
    oHint.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cViridoMission::DisableHint(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_HINTS)
    coreObject3D& oHint = m_aHintRaw[iIndex];

    // 
    if(!oHint.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(!bAnimated) oHint.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cViridoMission::EnableBean(const coreUintW iIndex)
{
    ASSERT(iIndex < VIRIDO_BEANS)
    coreObject3D* pBean = (*m_Bean    .List())[iIndex];
    coreObject3D* pWave = (*m_BeanWave.List())[iIndex];

    // 
    WARN_IF(pBean->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBean(iIndex, false);

    // 
    pBean->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cViridoMission::DisableBean(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_BEANS)
    coreObject3D* pBean = (*m_Bean    .List())[iIndex];
    coreObject3D* pWave = (*m_BeanWave.List())[iIndex];

    // 
    if(!pBean->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    pBean->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(pBean->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_MAGENTA);
}


// ****************************************************************
// 
void cViridoMission::EnableGlobe()
{
    WARN_IF(m_GlobeWave.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableGlobe(false);

    // 
    m_Globe.Resurrect();

    // 
    m_GlobeWave.SetAlpha   (0.0f);
    m_GlobeWave.SetEnabled (CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_GlobeWave);
}


// ****************************************************************
// 
void cViridoMission::DisableGlobe(const coreBool bAnimated)
{
    if(!m_GlobeWave.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Globe.Kill(false);

    // 
    m_GlobeWave.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_GlobeWave);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(m_Globe.GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_PURPLE);
}


// ****************************************************************
// 
void cViridoMission::EnableTarget()
{
    // 
    WARN_IF(m_Target.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableTarget(false);

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
        pObject->SetSize    (coreVector3(0.0f,0.0f,0.0f));
        pObject->SetEnabled (CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    nInitFunc(&m_Target);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTargetWave); ++i) nInitFunc(&m_aTargetWave[i]);
}


// ****************************************************************
// 
void cViridoMission::DisableTarget(const coreBool bAnimated)
{
    // 
    if(!m_Target.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(pObject);
    };
    nExitFunc(&m_Target);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTargetWave); ++i) nExitFunc(&m_aTargetWave[i]);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(m_Target.GetPosition(), COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cViridoMission::StartDrumBeat(const coreUintW iDrum, const coreUintW iBarrier)
{
    ASSERT(iDrum < VIRIDO_DRUMS)
    WARN_IF(m_aiDrumIndex[iDrum] != UINT8_MAX) return;

    ASSERT(iBarrier < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iBarrier];

    // 
    m_aiDrumCount[iDrum] = 0u;
    m_aiDrumIndex[iDrum] = iBarrier;

    // 
    ASSERT(oBarrier.IsEnabled(CORE_OBJECT_ENABLE_ALL))
    oBarrier.SetColor3(COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cViridoMission::EndDrumBeat(const coreUintW iDrum, const coreBool bAnimated)
{
    ASSERT(iDrum < VIRIDO_DRUMS)
    if(m_aiDrumIndex[iDrum] == UINT8_MAX) return;

    ASSERT(m_aiDrumIndex[iDrum] < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[m_aiDrumIndex[iDrum]];

    // 
    m_aiDrumCount[iDrum] = 0u;
    m_aiDrumIndex[iDrum] = UINT8_MAX;

    // 
    oBarrier.SetColor3(COLOR_ENERGY_BLUE);

    // 
    if(bAnimated)
    {
        const coreVector2 vDir = oBarrier.GetDirection().xy();
        const coreVector2 vPos = oBarrier.GetPosition ().xy() + vDir * oBarrier.GetCollisionRange().y;

        for(coreUintW j = 20u; j--; ) g_pSpecialEffects->CreateBlowColor(coreVector3(vPos + vDir.Rotated90() * (1.0f * (I_TO_F(j) - 9.5f)), 0.0f), coreVector3(vDir, 0.0f), 100.0f, 1u, COLOR_ENERGY_RED);
    }
}


// ****************************************************************
// 
void cViridoMission::StartPoleDance(const coreUintW iIndex)
{
    WARN_IF(m_iPoleIndex != UINT8_MAX) return;

    ASSERT(iIndex < VIRIDO_LASERS)
    coreObject3D* pLaser = (*m_Laser    .List())[iIndex];
    coreObject3D* pWave  = (*m_LaserWave.List())[iIndex];

    // 
    m_fPoleCount = 0.0f;
    m_iPoleIndex = iIndex;

    // 
    ASSERT(pLaser->IsEnabled(CORE_OBJECT_ENABLE_ALL))
    pLaser->SetColor3(pLaser->GetColor3() * 0.6f);
    pWave ->SetColor3(pWave ->GetColor3() * 0.6f);
}


// ****************************************************************
// 
void cViridoMission::EndPoleDance(const coreBool bAnimated)
{
    if(m_iPoleIndex == UINT8_MAX) return;

    ASSERT(m_iPoleIndex < VIRIDO_LASERS)
    coreObject3D* pLaser = (*m_Laser    .List())[m_iPoleIndex];
    coreObject3D* pWave  = (*m_LaserWave.List())[m_iPoleIndex];

    // 
    m_fPoleCount = 0.0f;
    m_iPoleIndex = UINT8_MAX;

    // 
    pLaser->SetColor3(pLaser->GetColor3() / 0.6f);
    pWave ->SetColor3(pWave ->GetColor3() / 0.6f);

    // 
    if(bAnimated) for(coreUintW j = 100u; j--; ) g_pSpecialEffects->CreateSplashColor(pLaser->GetPosition() + pLaser->GetDirection() * (2.0f * (I_TO_F(j) - 49.5f)), 10.0f, 1u, COLOR_ENERGY_PURPLE * 0.6f);
}


// ****************************************************************
// 
void cViridoMission::SetBarrierScale(const coreUintW iIndex, const coreFloat fScale)
{
    ASSERT(iIndex < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

    // 
    ASSERT(oBarrier.IsEnabled(CORE_OBJECT_ENABLE_ALL))
    oBarrier.SetSize   (coreVector3(7.5f * fScale, 2.5f, 2.5f));
    oBarrier.SetTexSize(coreVector2(1.2f * fScale, 0.25f) * 0.5f);
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_Hint.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Hint);

    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_BallTrail.Render();

        // 
        m_LaserWave.Render();

        // 
        m_Shadow.Render();

        // 
        m_BeanWave.Render();
    }
    glDepthMask(true);

    // 
    m_Laser.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Laser);

    // 
    m_Bean.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Bean);

    // 
    m_GlobeWave.Render();
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i) m_aPaddle[i].Render();
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyObject(&m_aPaddle[i]);

    // 
    m_Barrier1.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyList(&m_Barrier1);

    DEPTH_PUSH

    // 
    m_Barrier2.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyList(&m_Barrier2);
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnTop()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTargetWave); ++i) m_aTargetWave[i].Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    m_Ball.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Ball);

    // 
    m_Target.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Target);
}


// ****************************************************************
// 
void cViridoMission::__MoveOwnBefore()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS; ++i)
    {
        coreObject3D* pBall = (*m_Ball.List())[i];
        if(!pBall->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreVector3 vSize = pBall->GetSize();

        // 
        coreVector2       vNewDir = pBall->GetDirection().xy();
        const coreVector2 vNewPos = pBall->GetPosition ().xy() + vNewDir * FOREGROUND_AREA * (HAS_BIT(m_iStickyState, 1u) ? 0.0f : (VIRIDO_BALL_SPEED * TIME));

        // restrict movement to the foreground area (# no position correction)
             if((vNewPos.x < -FOREGROUND_AREA.x) && (vNewDir.x < 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2(-vSize.x, 0.0f)); vNewDir.x =  ABS(vNewDir.x); if(!i) ADD_BIT(m_iBounceState, 7u)}
        else if((vNewPos.x >  FOREGROUND_AREA.x) && (vNewDir.x > 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2( vSize.x, 0.0f)); vNewDir.x = -ABS(vNewDir.x); if(!i) ADD_BIT(m_iBounceState, 7u)}
             if((vNewPos.y < -FOREGROUND_AREA.y) && (vNewDir.y < 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2( 0.0f,-vSize.y)); vNewDir.y =  ABS(vNewDir.y); if(!i) ADD_BIT(m_iBounceState, 7u)}
        else if((vNewPos.y >  FOREGROUND_AREA.y) && (vNewDir.y > 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2( 0.0f, vSize.y)); vNewDir.y = -ABS(vNewDir.y); if(!i) ADD_BIT(m_iBounceState, 7u)}

        // 
        pBall->SetPosition (coreVector3(vNewPos, 0.0f));
        pBall->SetDirection(coreVector3(vNewDir, 0.0f));
        pBall->SetTexOffset(coreVector2(0.0f, m_fAnimation));

        for(coreUintW j = 0u; j < VIRIDO_TRAILS; ++j)
        {
            coreObject3D* pTrail = (*m_BallTrail.List())[i*VIRIDO_TRAILS + j];

            // 
            pTrail->SetAlpha(pTrail->GetAlpha() - 2.0f * TIME);
            if(pTrail->GetAlpha() < 0.0f)
            {
                // 
                pTrail->SetPosition (pBall->GetPosition ());
                pTrail->SetDirection(pBall->GetDirection());
                pTrail->SetAlpha    (1.0f);
            }

            // 
            pTrail->SetSize     (vSize * pTrail->GetAlpha());
            pTrail->SetColor3   (COLOR_ENERGY_GREEN * LERP(0.15f, 0.8f, pTrail->GetAlpha()));
            pTrail->SetTexOffset(coreVector2(0.0f, m_fAnimation));
        }
    }

    // 
    m_Ball     .MoveNormal();
    m_BallTrail.MoveNormal();
}


// ****************************************************************
// 
void cViridoMission::__MoveOwnAfter()
{
    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
    {
        coreObject3D& oPaddle       = m_aPaddle      [i];
        coreObject3D& oPaddleSphere = m_aPaddleSphere[i];
        if(!oPaddle.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreBool bBoss = i ? false : true;

        // 
        const cShip* pOwner = m_apPaddleOwner[i];
        if(pOwner)
        {
            oPaddle.SetPosition (coreVector3(pOwner->GetPosition ().xy() + pOwner->GetDirection().xy() * 3.0f, 0.0f));
            oPaddle.SetDirection(coreVector3(pOwner->GetDirection().xy(), 0.0f));
        }

        // 
        if(pOwner) oPaddle.SetAlpha(MIN1(oPaddle.GetAlpha() + 5.0f*TIME));
              else oPaddle.SetAlpha(MAX0(oPaddle.GetAlpha() - 5.0f*TIME));

        // 
        if(!oPaddle.GetAlpha()) this->DisablePaddle(i, false);

        // 
        oPaddle.SetTexOffset(coreVector2(0.0f, 0.5f * m_fAnimation));
        oPaddle.Move();

        // 
        oPaddleSphere.SetPosition(coreVector3(oPaddle.GetPosition().xy() - oPaddle.GetDirection().xy() * (bBoss ? 27.0f : 12.0f), 0.0f));
        oPaddleSphere.Move();
    }

    // 
    for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
    {
        coreObject3D& oBarrier = m_aBarrierRaw[i];
        if(!oBarrier.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(m_apBarrierOwner[i] != VIRIDO_BARRIER_FREE)
        {
            // 
            const cShip* pOwner = m_apBarrierOwner[i];
            if(pOwner)
            {
                oBarrier.SetPosition(coreVector3(pOwner->GetPosition().xy() + oBarrier.GetDirection().xy() * 7.0f, 0.0f));
            }
        }

        // 
        if(m_apBarrierOwner[i]) oBarrier.SetAlpha(MIN1(oBarrier.GetAlpha() + 5.0f * TIME));
                           else oBarrier.SetAlpha(MAX0(oBarrier.GetAlpha() - (m_bBarrierSlow ? 1.0f : 5.0f) * TIME));

        // 
        if(!oBarrier.GetAlpha()) this->DisableBarrier(i, false);

        // 
        const coreFloat fOffset = I_TO_F(i) * (1.0f/7.0f);

        // 
        oBarrier.SetTexOffset(coreVector2(0.0f, FRACT(0.5f * m_fAnimation + fOffset)));

        if(m_apBarrierOwner[i])
        {
            // 
            const coreVector2 vRayPos = oBarrier.GetPosition ().xy() + oBarrier.GetDirection().xy() * oBarrier.GetCollisionRange().y;
            const coreVector2 vRayDir = oBarrier.GetDirection().xy().Rotated90();

            // 
            cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, vRayPos, vRayDir, &oBarrier, [&](cPlayer* OUTPUT pPlayer, const coreFloat* pfHitDistance, const coreUint8 iHitCount, const coreBool bFirstHit)
            {
                if(ABS(pfHitDistance[0]) > oBarrier.GetCollisionRange().x * m_fBarrierRange) return;

                // 
                const coreVector2 vDiff = pPlayer->GetPosition().xy() - oBarrier.GetPosition().xy();
                const coreFloat   fMax  = coreVector2::Dot(vDiff, oBarrier.GetDirection().xy());

                // 
                const coreVector2 vNewDiff = MapToAxisInv(vDiff, oBarrier.GetDirection().xy());
                coreVector2       vNewPos  = oBarrier.GetPosition().xy() + MapToAxis(coreVector2(vNewDiff.x, MAX(oBarrier.GetCollisionRange().y + 0.1f, fMax)), oBarrier.GetDirection().xy());

                // 
                const coreVector4 vArea = pPlayer->GetArea();

                if(m_bBarrierClamp)
                {
                         if(vRayDir.x && (vNewPos.x < vArea.x)) vNewPos -= vRayDir * ((vNewPos.x - vArea.x) * RCP(vRayDir.x));
                    else if(vRayDir.x && (vNewPos.x > vArea.z)) vNewPos -= vRayDir * ((vNewPos.x - vArea.z) * RCP(vRayDir.x));
                         if(vRayDir.y && (vNewPos.y < vArea.y)) vNewPos -= vRayDir * ((vNewPos.y - vArea.y) * RCP(vRayDir.y));
                    else if(vRayDir.y && (vNewPos.y > vArea.w)) vNewPos -= vRayDir * ((vNewPos.y - vArea.w) * RCP(vRayDir.y));
                }
                vNewPos.x = CLAMP(vNewPos.x, vArea.x, vArea.z);
                vNewPos.y = CLAMP(vNewPos.y, vArea.y, vArea.w);

                // 
                pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));
            });

            
            if((m_avBarrierPos[i] - oBarrier.GetPosition().xy()).LengthSq() >= POW2(20.0f)) continue;       
            
            g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                if(coreVector2::Dot(pBullet->GetFlyDir(), oBarrier.GetDirection().xy()) > 0.0f) return;
                
                const coreVector2 vOriginDiff = pBullet->GetOwner()->GetPosition().xy() - vRayPos;
                const coreBool bBehind = (coreVector2::Dot(vOriginDiff, oBarrier.GetDirection().xy()) < 0.0f);
                
                
                if(pBullet->GetFlyTime() < 0.1f)
                {
                    if(bBehind) return;
                }
                const coreBool bAlways = (pBullet->GetFlyTime() < 0.1f) && !bBehind;
                
                const coreFloat fSlant = (1.0f + coreVector2::Dot(pBullet->GetFlyDir(), oBarrier.GetDirection().xy())) * (bBehind ? 0.0f : 1.0f);      
                
                const coreFloat   fRadius = pBullet->GetCollisionRange().xy().Max();       
                const coreVector2 vNewPos = pBullet->GetPosition().xy() + pBullet->GetFlyDir() * fRadius;
                const coreVector2 vOldPos = pBullet->GetPosition().xy() - pBullet->GetFlyDir() * MAX(pBullet->GetCollisionRadius() * 2.0f, pBullet->GetSpeed() * BULLET_SPEED_FACTOR * TIME * (1.0f + 10.0f * fSlant));
                
                
                const coreVector2 vOldRayPos = m_avBarrierPos[i] + m_avBarrierDir[i] * oBarrier.GetCollisionRange().y;
                const coreVector2 vOldRayDir = m_avBarrierDir[i].Rotated90();
                
                // 
                const coreVector2 vDiff    = vNewPos - vRayPos;
                const coreVector2 vDiffOld = vOldPos - vOldRayPos;
            
                // 
                const coreFloat fDot    = coreVector2::Dot(vDiff,    vRayDir   .Rotated90());
                const coreFloat fDotOld = coreVector2::Dot(vDiffOld, vOldRayDir.Rotated90());
                
                coreVector2 vRealIntersection;
                if(!RayIntersection(vNewPos, pBullet->GetFlyDir(), vRayPos, vRayDir, &vRealIntersection)) return;
                
                const coreVector2 vRealDiff = vRayPos - vRealIntersection;
            
                // 
                if(((SIGN(fDot) != SIGN(fDotOld)) || (bAlways && (SIGN(fDot) > 0.0f)))/* && (ABS(fDot) < 5.0f) && (ABS(fDotOld) < 5.0f)*/    &&   // to handle teleportation
                    (/*ABS(coreVector2::Dot(vDiff,    vRayDir))*/vRealDiff.LengthSq()    < POW2(oBarrier.GetCollisionRange().x + pBullet->GetCollisionRange().x)))
                {
                    const coreVector2 vIntersection = vRayPos + vRayDir * coreVector2::Dot(vDiff, vRayDir);

                    // 
                    if(!g_pForeground->IsVisiblePoint(vIntersection))
                    {
                        pBullet->AddStatus(BULLET_STATUS_GHOST);
                        return;
                    }
    
                    // 
                    for(coreUintW j = 0u; j < VIRIDO_DRUMS; ++j)
                    {
                        if(i == m_aiDrumIndex[j])
                        {
                            // 
                            d_cast<cPlayer*>(pBullet->GetOwner())->GetScoreTable()->RefreshCooldown();

                            // 
                            m_aiDrumCount[j] += pBullet->GetDamage();

                            // 
                            pBullet->Deactivate(true);

                            // 
                            g_pGame->PlayHitSound(coreVector3(vIntersection, 0.0f));
                            return;
                        }
                    }

                    if(m_bBarrierReflect)
                    {
                        // 
                        coreVector2 vNormal = oBarrier.GetDirection().xy();
                        if(SameDirection(-pBullet->GetFlyDir(), vNormal))
                        {
                            m_iBarrierBounce = (m_iBarrierBounce + 1u) % 7u;
                            vNormal = (vNormal + vNormal.Rotated90() * LERP(-1.0f, 1.0f, I_TO_F(m_iBarrierBounce) / 6.0f) * 0.08f).Normalized();
                        }

                        // 
                        pBullet->Reflect(&oBarrier, vIntersection, vNormal);

                        // 
                        g_pGame->PlayReflectSound(coreVector3(vIntersection, 0.0f));
                    }
                    else
                    {
                        // 
                        pBullet->Deactivate(true);
                    }
                }
            });
        }
    }

    // 
    for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
    {
        const coreObject3D& oBarrier = m_aBarrierRaw[i];

        m_avBarrierPos[i] = oBarrier.GetPosition ().xy();   // old frozen
        m_avBarrierDir[i] = oBarrier.GetDirection().xy();
    }

    // 
    m_Barrier1.MoveNormal();
    m_Barrier2.MoveNormal();

    // 
    m_iLaserTouch = 0u;

    // 
    for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
    {
        coreObject3D* pLaser = (*m_Laser    .List())[i];
        coreObject3D* pWave  = (*m_LaserWave.List())[i];
        if(!pLaser->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat fValue  = FRACT(10.0f * m_fAnimation);
        const coreFloat fOffset = I_TO_F(i) * (1.0f/7.0f);

        // 
        pLaser->SetTexOffset(coreVector2(3.0f,4.0f) * FRACT(0.2f * m_fAnimation + fOffset));

        // 
        pWave->SetPosition (pLaser->GetPosition ());
        pWave->SetSize     (pLaser->GetSize     () * LERP(1.0f, 4.0f, fValue));
        pWave->SetDirection(pLaser->GetDirection());
        pWave->SetAlpha    (1.0f - BLENDH3(fValue));
        pWave->SetTexOffset(pLaser->GetTexOffset());
        
        STATIC_ASSERT(VIRIDO_LASERS <= sizeof(m_iLaserIgnore)*8u)
        
        if(HAS_BIT(m_iLaserIgnore, i)) continue;
        if((m_avLaserPos[i] - pLaser->GetPosition().xy()).LengthSq() >= POW2(20.0f)) continue;

        // 
        const coreVector2 vRayDir = pLaser->GetDirection().xy();
        const coreVector2 vRayPos = pLaser->GetPosition ().xy() - vRayDir * pLaser->GetSize().y;

        // (here due to ordering, to prevent player from flying through laser)
        cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, vRayPos, vRayDir, pLaser, [&](cPlayer* OUTPUT pPlayer, const coreFloat* pfHitDistance, const coreUint8 iHitCount, const coreBool bFirstHit)
        {
            // 
            const coreVector4 vArea = pPlayer->GetArea();
            const coreVector2 vDiff = pPlayer->GetOldPos() - m_avLaserPos[i];
            const coreVector2 vNorm = m_avLaserDir[i].Rotated90();
            const coreFloat   fSide = coreVector2::Dot(vDiff, vNorm);

            // 
            const coreVector2 vIntersection = vRayPos + vRayDir * pfHitDistance[0];

            // 
            coreVector2 vNewPos = vIntersection + vNorm * (fSide * 1.0f);

                 if(vRayDir.x && (vNewPos.x < vArea.x)) vNewPos -= vRayDir * ((vNewPos.x - vArea.x) * RCP(vRayDir.x));
            else if(vRayDir.x && (vNewPos.x > vArea.z)) vNewPos -= vRayDir * ((vNewPos.x - vArea.z) * RCP(vRayDir.x));
                 if(vRayDir.y && (vNewPos.y < vArea.y)) vNewPos -= vRayDir * ((vNewPos.y - vArea.y) * RCP(vRayDir.y));
            else if(vRayDir.y && (vNewPos.y > vArea.w)) vNewPos -= vRayDir * ((vNewPos.y - vArea.w) * RCP(vRayDir.y));
            vNewPos.x = CLAMP(vNewPos.x, vArea.x, vArea.z);
            vNewPos.y = CLAMP(vNewPos.y, vArea.y, vArea.w);

            // 
            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));
            
            

            if(m_bLaserCross)
            {
                coreUintW iCurLaser = i;
                //coreUintW iLimit = 5u;
                
                coreVector2 vOldPos = pPlayer->GetOldPos();
                
                for(coreUintW j = 0u; j < VIRIDO_LASERS; ++j)
                {
                    if(iCurLaser == j) continue;
                    
                    coreObject3D* pLaser2 = (*m_Laser.List())[j];
                    if(!pLaser2->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
                    
                    if((m_avLaserPos[j] - pLaser2->GetPosition().xy()).LengthSq() >= POW2(20.0f)) continue;       
                    
                    const coreVector2 vDiff2   = vNewPos - pLaser2->GetPosition().xy();
                    const coreVector2 vDiffOld = vOldPos - m_avLaserPos[j];
                
                    // 
                    const coreFloat fDot    = coreVector2::Dot(vDiff2,   pLaser2->GetDirection().xy().Rotated90());
                    const coreFloat fDotOld = coreVector2::Dot(vDiffOld, m_avLaserDir[j]             .Rotated90());
                
                    // 
                    if((SIGN(fDot) != SIGN(fDotOld)) && (ABS(fDot) < 5.0f) && (ABS(fDotOld) < 5.0f))   // to handle teleportation
                    {
                        const coreVector2 vRayDir2 = pLaser2->GetDirection().xy();
                        const coreVector2 vRayPos2 = pLaser2->GetPosition ().xy() - vRayDir2 * pLaser2->GetSize().y;
                        
                        coreVector2 vDot;
                        if(RayIntersection(vRayPos, vRayDir, vRayPos2, vRayDir2, &vDot))
                        {
                            const coreVector2 vDiffA = pPlayer->GetOldPos() - m_avLaserPos[j];
                            const coreVector2 vNormA = m_avLaserDir[j].Rotated90();
                            const coreFloat   fSideA = coreVector2::Dot(vDiffA, vNormA);
                        
                            const coreVector2 vMove = (vNorm * SIGN(fSide) + vNormA * SIGN(fSideA)).Normalized() * 0.1f;
                            vNewPos = vDot + vMove;
                        
                            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));
                        }
                    }
                }
            }
            
            // 
            ADD_BIT(m_iLaserTouch, i)
            STATIC_ASSERT(VIRIDO_LASERS <= sizeof(m_iLaserTouch)*8u)

            // 
            const coreUintW iIndex = g_pGame->GetPlayerIndex(pPlayer);
            coreFlow&       fTick  = m_afLaserTick[iIndex];

            // 
            fTick.Update(30.0f);
            if(fTick >= 1.0f)
            {
                // 
                if(i == m_iPoleIndex) g_pSpecialEffects->CreateSplashColor(coreVector3(vIntersection, 0.0f), 50.0f, 2u, COLOR_ENERGY_PURPLE * 0.6f);
                                 else g_pSpecialEffects->CreateSplashColor(coreVector3(vIntersection, 0.0f),  5.0f, 1u, COLOR_ENERGY_PURPLE);

                // 
                fTick = FRACT(fTick);
            }

            // 
            if(i == m_iPoleIndex)
            {
                m_fPoleCount.Update(1.0f);
            }
        });
    }

    // 
    for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
    {
        const coreObject3D* pLaser = (*m_Laser.List())[i];

        m_avLaserPos[i] = pLaser->GetPosition ().xy();   // old frozen
        m_avLaserDir[i] = pLaser->GetDirection().xy();
    }

    // 
    m_Laser    .MoveNormal();
    m_LaserWave.MoveNormal();

    // 
    for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
    {
        coreObject3D& oShadow = m_aShadowRaw[i];
        if(!oShadow.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apShadowOwner[i];
        if(pOwner)
        {
            oShadow.SetSize(coreVector3(1.0f,1.0f,1.0f) * 0.25f * pOwner->GetPosition().z * (pOwner->GetSize().z / 1.4f) * ((i < VIRIDO_SHADOWS_ENEMY) ? 1.0f : 2.0f));
        }

        // 
        else oShadow.SetAlpha(MAX(oShadow.GetAlpha() - 5.0f*TIME, 0.0f));

        // 
        oShadow.SetDirection(coreVector3(coreVector2::Direction((4.0f*PI) * m_fAnimation), 0.0f));

        // 
        if(!oShadow.GetAlpha()) this->DisableShadow(i, false);
    }

    // 
    m_Shadow.MoveNormal();

    // 
    for(coreUintW i = 0u; i < VIRIDO_HINTS; ++i)
    {
        coreObject3D& oHint = m_aHintRaw[i];
        if(!oHint.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreUintW iIndex = m_aiHintBarrier[i];
        ASSERT(iIndex < VIRIDO_BARRIERS)

        const coreVector2 vBasePos   = m_aBarrierRaw[iIndex].GetPosition ().xy();
        const coreVector2 vBaseDir   = m_aBarrierRaw[iIndex].GetDirection().xy();
        const coreFloat   fBaseAlpha = m_aBarrierRaw[iIndex].GetAlpha();

        // 
        if(!m_aBarrierRaw[iIndex].IsEnabled(CORE_OBJECT_ENABLE_MOVE)) this->DisableHint(i, false);

        // 
        const coreFloat fOffset = I_TO_F(i) * (1.0f/12.0f);
        const coreBool  bActive = HAS_BIT(m_iHintActive, i);

        // 
        oHint.SetPosition (coreVector3(vBasePos - vBaseDir * 7.0f, 0.0f));
        oHint.SetDirection(coreVector3(vBaseDir,                   0.0f));
        oHint.SetSize     (coreVector3(1.0f,1.0f,1.0f) * (bActive ? 2.5f : 1.9f));
        oHint.SetAlpha    (fBaseAlpha                  * (bActive ? 1.0f : 0.5f));
        oHint.SetTexOffset(coreVector2(FRACT(0.6f * m_fAnimation + fOffset), 0.0f));
    }

    // 
    m_Hint.MoveNormal();

    // 
    for(coreUintW i = 0u; i < VIRIDO_BEANS; ++i)
    {
        coreObject3D* pBean = (*m_Bean    .List())[i];
        coreObject3D* pWave = (*m_BeanWave.List())[i];
        if(!pBean->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/7.0f);
        const coreVector2 vDir    = coreVector2::Direction(((i % 2u) ? (-4.0f*PI) : (4.0f*PI)) * m_fAnimation);

        // 
        pBean->SetDirection(coreVector3(vDir, 0.0f));
        pBean->SetTexOffset(coreVector2(0.0f, FRACT(0.8f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pBean->GetPosition ());
        pWave->SetDirection(pBean->GetDirection().InvertedX());
        pWave->SetTexOffset(pBean->GetTexOffset());
        pWave->SetAlpha(0.6f);
    }

    // 
    m_Bean    .MoveNormal();
    m_BeanWave.MoveNormal();

    // 
    if(!m_Globe.HasStatus(ENEMY_STATUS_DEAD))
    {
        // 
        m_Globe.SetDirection(coreVector3(coreVector2::Direction((4.0f*PI) * m_fAnimation), 0.0f));
        m_Globe.SetTexOffset(coreVector2(0.0f, 0.5f * m_fAnimation));
        m_Globe.coreObject3D::Move();

        const coreFloat fTime = FRACT(7.0f * m_fAnimation);

        m_GlobeWave.SetPosition (m_Globe.GetPosition ());
        m_GlobeWave.SetSize     (m_Globe.GetSize     () * LERPB(1.0f, 1.5f, fTime));
        m_GlobeWave.SetDirection(m_Globe.GetDirection());
        m_GlobeWave.SetTexOffset(m_Globe.GetTexOffset());
        m_GlobeWave.SetAlpha    (1.0f - BLENDH3(fTime));
        m_GlobeWave.Move();
    }

    // 
    if(m_Target.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Target.SetTexOffset(coreVector2(0.0f, -m_fAnimation));
        m_Target.Move();

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTargetWave); ++i)
        {
            const coreFloat fTime = I_TO_F(i + 1u) / 3.0f;

            // 
            m_aTargetWave[i].SetPosition (m_Target.GetPosition());
            m_aTargetWave[i].SetSize     (m_Target.GetSize    () * LERP(2.0f, 1.0f, fTime));
            m_aTargetWave[i].SetTexOffset(coreVector2(0.0f, m_fAnimation * ((i % 2u) ? 1.0f : -1.0f)));
            m_aTargetWave[i].SetAlpha    (LERP(0.0f, 0.8f, fTime));
            m_aTargetWave[i].Move();
        }
    }

    // 
    m_iBounceState = 0u;
    STATIC_ASSERT(VIRIDO_PADDLES+1u <= sizeof(m_iBounceState)*8u)

    // 
    if(!HAS_BIT(m_iStickyState, 1u))
    {
        // 
        Core::Manager::Object->TestCollision(TYPE_VIRIDO_PADDLE, TYPE_VIRIDO_BALL, [this](const coreObject3D* pPaddle, coreObject3D* OUTPUT pBall, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            // 
            if(coreVector2::Dot(pPaddle->GetDirection().xy(), pBall->GetDirection().xy()) >= 0.0f)
                return;

            // 
            const coreUintW     iIndex        = pPaddle - m_aPaddle;
            const coreObject3D& oPaddleSphere = m_aPaddleSphere[iIndex];

            // 
            coreVector3 vImpact;
            if(Core::Manager::Object->TestCollision(&oPaddleSphere, pBall, &vImpact))
            {
                const coreVector2 vBallPos   = pBall  ->GetPosition ().xy();
                const coreVector2 vBallDir   = pBall  ->GetDirection().xy();
                const coreVector2 vPaddleDir = pPaddle->GetDirection().xy();

                if(HAS_BIT(m_iRealState, iIndex))
                {
                    // 
                    coreVector2 vNewDir = coreVector2::Reflect(vBallDir, (vBallPos - oPaddleSphere.GetPosition().xy()).Normalized(vBallDir));
                    if(IsHorizontal(vPaddleDir)) vNewDir.x = MAX(ABS(vNewDir.x), 0.75f) * vPaddleDir.x;
                                            else vNewDir.y = MAX(ABS(vNewDir.y), 0.75f) * vPaddleDir.y;

                    // 
                    pBall->SetDirection(coreVector3(vNewDir.Normalized(), 0.0f));
                }
                else
                {
                    // 
                    coreVector2 vNewDir = vBallDir;
                    if(IsHorizontal(vPaddleDir)) vNewDir.x = ABS(vNewDir.x) * vPaddleDir.x;
                                            else vNewDir.y = ABS(vNewDir.y) * vPaddleDir.y;

                    // 
                    pBall->SetDirection(coreVector3(vNewDir, 0.0f));
                }

                // 
                ADD_BIT(m_iBounceState, iIndex)
                if(m_iStickyState) ADD_BIT(m_iStickyState, 1u)

                // 
                cViridoMission::__BounceEffect(vImpact.xy());
            }
        });
    }

    // 
    if(HAS_BIT(m_iStickyState, 1u))
    {
        // only between first ball and first paddle
        coreObject3D& oBall   = m_aBallRaw[0];
        coreObject3D& oPaddle = m_aPaddle [0];

        // 
        oBall.SetPosition(oPaddle.GetPosition() + oPaddle.GetDirection() * (oPaddle.GetCollisionRange().y * 2.0f - 0.3f)); // TODO 1: adjust distance (ball size 2.7f -> 3.1f)
        oBall.Move();
    }

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_VIRIDO_BALL, [](cPlayer* OUTPUT pPlayer, const coreObject3D* pBall, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(10, ELEMENT_GREEN, vIntersection.xy());

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_GREEN);
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_VIRIDO_BALL, [](cBullet* OUTPUT pBullet, const coreObject3D* pBall, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pBullet->Reflect(pBall, vIntersection.xy());

        // 
        g_pGame->PlayReflectSound(vIntersection);
    });

    // 
    if(false)   // !m_Vaus.HasStatus(ENEMY_STATUS_DEAD))
    {
        cEnemy*     pCurEnemy  = NULL;
        coreFloat   fCurLenSq  = FLT_MAX;
        coreVector2 vCurImpact = coreVector2(FLT_MAX,FLT_MAX);

        // only first ball will be active in this mission stage
        coreObject3D& oBall = m_aBallRaw[0];

        // 
        const coreVector2 vBallPos    = oBall.GetPosition ().xy();
        const coreVector2 vBallDir    = oBall.GetDirection().xy();
        const coreVector2 vOldBallPos = vBallPos - vBallDir * FOREGROUND_AREA * (HAS_BIT(m_iStickyState, 1u) ? 0.0f : (VIRIDO_BALL_SPEED * TIME));

        // 
        Core::Manager::Object->TestCollision(TYPE_ENEMY, &oBall, [&](cEnemy* OUTPUT pEnemy, const coreObject3D* pBall, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!bFirstHit || (pEnemy->GetID() != cCinderEnemy::ID)) return;

            // 
            const coreFloat fNewLenSq = (pEnemy->GetPosition().xy() - vOldBallPos).LengthSq();
            if(fNewLenSq < fCurLenSq)
            {
                pCurEnemy  = pEnemy;
                fCurLenSq  = fNewLenSq;
                vCurImpact = vIntersection.xy();
            }
        });

        if(pCurEnemy)
        {
            const coreVector2 vDiff = vOldBallPos - pCurEnemy->GetPosition().xy();

            // 
            coreUintW iAxis;
            if(IsHorizontal(vDiff)) iAxis = ((vDiff.x * vBallDir.x) >= 0.0f) ? 1u : 0u;
                               else iAxis = ((vDiff.y * vBallDir.y) >= 0.0f) ? 0u : 1u;

            // 
            coreVector2 vNewDir = vBallDir;
            vNewDir.arr(iAxis) = ABS(vBallDir.arr(iAxis)) * SIGN(vDiff.arr(iAxis));

            // 
            oBall.SetPosition (coreVector3(vBallPos + vDiff * (3.0f * TIME), 0.0f));
            oBall.SetDirection(coreVector3(vNewDir, 0.0f));

            // 
            pCurEnemy->Kill(true);

            // 
            cViridoMission::__BounceEffect(vCurImpact);
        }
    }
}


// ****************************************************************
// 
void cViridoMission::__BounceEffect(const coreVector2 vEffectPos)
{
    g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
}