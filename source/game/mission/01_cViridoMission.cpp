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
: m_Ball           (VIRIDO_BALLS)
, m_BallTrail      (VIRIDO_BALLS * VIRIDO_TRAILS)
, m_apPaddleOwner  {}
, m_Barrier        (VIRIDO_BARRIERS)
, m_apBarrierOwner {}
, m_Laser          (VIRIDO_LASERS)
, m_LaserWave      (VIRIDO_LASERS)
, m_apLaserOwner   {}
, m_avLaserPos     {}
, m_avLaserDir     {}
, m_afLaserTick    {}
, m_Shadow         (VIRIDO_SHADOWS)
, m_apShadowOwner  {}
, m_Bean           (VIRIDO_BEANS)
, m_iPoleCount     (0u)
, m_iPoleIndex     (UINT8_MAX)
, m_iRealState     (0u)
, m_iStickyState   (0u)
, m_iBounceState   (0u)
, m_fAnimation     (0.0f)
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
    m_Barrier.DefineProgram("effect_energy_flat_direct_inst_program");
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
            m_Barrier.BindObject(pBarrier);
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
            pLaser->DefineModel  ("object_tube_open.md3");
            pLaser->DefineTexture(0u, "effect_energy.png");
            pLaser->DefineProgram("effect_energy_flat_program");

            // set object properties
            pLaser->SetSize   (coreVector3(0.55f,160.0f,0.55f));
            pLaser->SetColor3 (COLOR_ENERGY_PURPLE * (iType ? 0.7f : 1.0f));
            pLaser->SetTexSize(coreVector2(0.5f,40.0f));
            pLaser->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_LaserWave.BindObject(pLaser);
                 else m_Laser    .BindObject(pLaser);
        }
    }

    // 
    m_Shadow.DefineProgram("effect_decal_single_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_SHADOWS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pShadow = &m_aShadowRaw[i];
            pShadow->DefineModel  (Core::Manager::Object->GetLowQuad());
            pShadow->DefineTexture(0u, "effect_wave.png");
            pShadow->DefineProgram("effect_decal_single_program");

            // set object properties
            pShadow->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Shadow.BindObject(pShadow);
        }
    }

    // 
    m_Bean.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_BEANS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pBean = &m_aBeanRaw[i];
            pBean->DefineModel  ("object_cube_top.md3");
            pBean->DefineTexture(0u, "effect_energy.png");
            pBean->DefineProgram("effect_energy_flat_program");

            // set object properties
            pBean->SetSize             (coreVector3(1.0f,1.0f,1.0f) * 2.5f);
            pBean->SetColor3           (COLOR_ENERGY_MAGENTA);
            pBean->SetTexSize          (coreVector2(1.0f,1.0f) * 0.4f);
            pBean->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 3.0f);
            pBean->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Bean.BindObject(pBean);
        }
    }

    // 
    m_Globe.DefineModelHigh("object_sphere.md3");
    m_Globe.DefineModelLow ("object_sphere.md3");
    m_Globe.DefineTexture  (0u, "effect_energy.png");
    m_Globe.DefineProgram  ("effect_energy_invert_program");
    m_Globe.SetTexSize     (coreVector2(4.5f,4.5f));
    m_Globe.Configure      (1000, 0u, COLOR_ENERGY_PURPLE * 0.8f);
    m_Globe.AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS);

    // 
    g_pGlow->BindList(&m_Ball);
    g_pGlow->BindList(&m_BallTrail);
    g_pGlow->BindList(&m_Barrier);
    g_pGlow->BindList(&m_Laser);
    g_pGlow->BindList(&m_LaserWave);
    g_pGlow->BindList(&m_Bean);
}


// ****************************************************************
// destructor
cViridoMission::~cViridoMission()
{
    // 
    m_Globe.Kill(false);

    // 
    g_pGlow->UnbindList(&m_Ball);
    g_pGlow->UnbindList(&m_BallTrail);
    g_pGlow->UnbindList(&m_Barrier);
    g_pGlow->UnbindList(&m_Laser);
    g_pGlow->UnbindList(&m_LaserWave);
    g_pGlow->UnbindList(&m_Bean);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS;    ++i) this->DisableBall   (i, false);
    for(coreUintW i = 0u; i < VIRIDO_PADDLES;  ++i) this->DisablePaddle (i, false);
    for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i) this->DisableBarrier(i, false);
    for(coreUintW i = 0u; i < VIRIDO_LASERS;   ++i) this->DisableLaser  (i, false);
    for(coreUintW i = 0u; i < VIRIDO_SHADOWS;  ++i) this->DisableShadow (i, false);
    for(coreUintW i = 0u; i < VIRIDO_BEANS;    ++i) this->DisableBean   (i, false);
}


// ****************************************************************
// 
void cViridoMission::EnableBall(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < VIRIDO_BALLS)
    coreObject3D* pBall  = (*m_Ball     .List())[iIndex];
    coreObject3D* pTrail = (*m_BallTrail.List())[iIndex*VIRIDO_TRAILS];

    // 
    WARN_IF(pBall->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
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
    WARN_IF(oPaddle.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
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
    WARN_IF(oBarrier.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(pOwner)
    m_apBarrierOwner[iIndex] = pOwner;

    // 
    oBarrier.SetDirection(coreVector3(vDirection, 0.0f));
    oBarrier.SetSize     (coreVector3(7.5f * fScale, 2.5f, 2.5f));
    oBarrier.SetTexSize  (coreVector2(1.2f * fScale, 0.25f) * 0.5f);
    oBarrier.SetAlpha    (0.0f);
    oBarrier.SetEnabled  (CORE_OBJECT_ENABLE_ALL);
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
    if(!bAnimated) oBarrier.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cViridoMission::EnableLaser(const coreUintW iIndex, const cShip* pOwner)
{
    ASSERT(iIndex < VIRIDO_LASERS)
    coreObject3D* pLaser = (*m_Laser    .List())[iIndex];
    coreObject3D* pWave  = (*m_LaserWave.List())[iIndex];

    // 
    WARN_IF(pLaser->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(pOwner)
    m_apLaserOwner[iIndex] = pOwner;

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
void cViridoMission::EnableShadow(const coreUintW iIndex, const cShip* pOwner, const coreVector2 vPosition)
{
    ASSERT(iIndex < VIRIDO_SHADOWS)
    coreObject3D& oShadow = m_aShadowRaw[iIndex];

    // 
    WARN_IF(oShadow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(pOwner)
    m_apShadowOwner[iIndex] = pOwner;

    // 
    oShadow.SetPosition(coreVector3(vPosition,0.0f));
    oShadow.SetSize    (coreVector3(0.0f,0.0f,0.0f));
    oShadow.SetAlpha   (0.85f);
    oShadow.SetEnabled (CORE_OBJECT_ENABLE_ALL);
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
void cViridoMission::EnableBean(const coreUintW iIndex)
{
    ASSERT(iIndex < VIRIDO_BEANS)
    coreObject3D& oBean = m_aBeanRaw[iIndex];

    // 
    WARN_IF(oBean.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oBean.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cViridoMission::DisableBean(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_BEANS)
    coreObject3D& oBean = m_aBeanRaw[iIndex];

    // 
    if(!oBean.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oBean.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(oBean.GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_MAGENTA);
}


// ****************************************************************
// 
void cViridoMission::StartPoleDance(const coreUintW iIndex)
{
    WARN_IF(m_iPoleIndex != UINT8_MAX) return;

    ASSERT(iIndex < VIRIDO_LASERS)
    coreObject3D* pLaser = (*m_Laser.List())[iIndex];

    // 
    m_iPoleCount = 0u;
    m_iPoleIndex = iIndex;

    // 
    ASSERT(pLaser->IsEnabled(CORE_OBJECT_ENABLE_ALL))
    pLaser->SetColor3(pLaser->GetColor3() * 0.6f);
}


// ****************************************************************
// 
void cViridoMission::EndPoleDance(const coreBool bAnimated)
{
    if(m_iPoleIndex == UINT8_MAX) return;

    ASSERT(m_iPoleIndex < VIRIDO_LASERS)
    coreObject3D* pLaser = (*m_Laser.List())[m_iPoleIndex];

    // 
    m_iPoleCount = 0u;
    m_iPoleIndex = UINT8_MAX;

    // 
    pLaser->SetColor3(pLaser->GetColor3() / 0.6f);

    // 
    if(bAnimated) for(coreUintW j = 100u; j--; ) g_pSpecialEffects->CreateSplashColor(pLaser->GetPosition() + pLaser->GetDirection() * (2.0f * (I_TO_F(j) - 49.5f)), 10.0f, 1u, COLOR_ENERGY_PURPLE * 0.6f);
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_BallTrail.Render();

        // 
        m_LaserWave.Render();

        // 
        m_Shadow.Render();
    }
    glDepthMask(true);

    // 
    m_Laser.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Laser);

    // 
    m_Bean.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Bean);
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
    m_Barrier.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyList(&m_Barrier);
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnTop()
{
    DEPTH_PUSH

    // 
    m_Ball.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Ball);
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
            const coreVector2 vDir = pOwner->GetDirection().xy();

            oPaddle.SetPosition (coreVector3(pOwner->GetPosition().xy() + vDir * 3.0f, 0.0f));
            oPaddle.SetDirection(coreVector3(vDir, 0.0f));
        }

        // 
        if(pOwner) oPaddle.SetAlpha(MIN(oPaddle.GetAlpha() + 5.0f*TIME, 1.0f));
              else oPaddle.SetAlpha(MAX(oPaddle.GetAlpha() - 5.0f*TIME, 0.0f));

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
            const cEnemy* pOwner = d_cast<const cEnemy*>(m_apBarrierOwner[i]);
            if(pOwner)
            {
                oBarrier.SetPosition(coreVector3(pOwner->GetPosition().xy() + oBarrier.GetDirection().xy() * 7.0f, 0.0f));
            }
        }

        // 
        if(m_apBarrierOwner[i]) oBarrier.SetAlpha(MIN(oBarrier.GetAlpha() + 5.0f*TIME, 1.0f));
                           else oBarrier.SetAlpha(MAX(oBarrier.GetAlpha() - 5.0f*TIME, 0.0f));

        // 
        if(!oBarrier.GetAlpha()) this->DisableBarrier(i, false);

        // 
        const coreFloat fOffset = 0.0f;// I_TO_F(i) * (1.0f/7.0f);   TODO 1: eigentlich schaut es besser aus, wenn es symmetrisch ist, bei anderen auch ?

        // 
        oBarrier.SetTexOffset(coreVector2(0.0f, FRACT(0.5f * m_fAnimation + fOffset)));

        // 
        const coreVector2 vRayPos = oBarrier.GetPosition ().xy() + oBarrier.GetDirection().xy() * oBarrier.GetCollisionRange().y;
        const coreVector2 vRayDir = oBarrier.GetDirection().xy().Rotated90();

        // 
        cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, vRayPos, vRayDir, &oBarrier, [&](cPlayer* OUTPUT pPlayer, const coreFloat* pfHitDistance, const coreUint8 iHitCount, const coreBool bFirstHit)
        {
            if(ABS(pfHitDistance[0]) > (oBarrier.GetCollisionRange().x)) return;
            if(coreVector2::Dot(pPlayer->GetMove(), oBarrier.GetDirection().xy()) > 0.0f) return;

            // 
            const coreVector2 vNewDiff = MapToAxisInv(pPlayer->GetPosition().xy() - oBarrier.GetPosition().xy(), oBarrier.GetDirection().xy());
            const coreVector2 vNewPos  = oBarrier.GetPosition().xy() + MapToAxis(coreVector2(vNewDiff.x, oBarrier.GetCollisionRange().y + 0.1f), oBarrier.GetDirection().xy());

            // 
            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));
        });

        // 
        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &oBarrier, [](cBullet* OUTPUT pBullet, const coreObject3D* pBarrier, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;

            // 
            pBullet->Reflect(pBarrier, vIntersection.xy(), pBarrier->GetDirection().xy());
        });
    }

    // 
    m_Barrier.MoveNormal();

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
        pLaser->SetTexOffset(coreVector2(3.0f,4.0f) * FRACT(0.8f * m_fAnimation + fOffset));

        // 
        pWave->SetPosition (pLaser->GetPosition ());
        pWave->SetSize     (pLaser->GetSize     () * (1.0f + 3.0f * fValue));
        pWave->SetDirection(pLaser->GetDirection());
        pWave->SetAlpha    (1.0f - fValue);
        pWave->SetTexOffset(pLaser->GetTexOffset());

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
            
            
            for(coreUintW j = 0u; j < VIRIDO_LASERS; ++j)
            {
                if(i == j) continue;
                
                coreObject3D* pLaser2 = (*m_Laser.List())[j];
                if(!pLaser2->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
        
                const coreVector2 vRayDir2 = pLaser2->GetDirection().xy();
                const coreVector2 vRayPos2 = pLaser2->GetPosition ().xy() - vRayDir2 * pLaser2->GetSize().y;
        
                // (here due to ordering, to prevent player from flying through laser)
                cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, vRayPos2, vRayDir2, pLaser2, [&](cPlayer* OUTPUT pPlayer2, const coreFloat* pfHitDistance2, const coreUint8 iHitCount2, const coreBool bFirstHit2)
                {
                    // 
                    const coreVector2 vDiff2 = pPlayer->GetOldPos() - m_avLaserPos[j];
                    const coreVector2 vNorm2 = m_avLaserDir[j].Rotated90();
                    const coreFloat   fSide2 = coreVector2::Dot(vDiff2, vNorm2);
        
                    // 
                    const coreVector2 vIntersection2 = vRayPos2 + vRayDir2 * pfHitDistance2[0];
        
                    // 
                    coreVector2 vNewPos2 = vIntersection2 + vNorm2 * (fSide2 * 1.0f);

                    // 
                    pPlayer->SetPosition(coreVector3((vNewPos + vNewPos2) * 0.5f, 0.0f));
                    
                    //const coreVector2 vShift = 
                    pPlayer->SetPosition(coreVector3(pPlayer->GetOldPos() + (pLaser->GetPosition().xy() - m_avLaserPos[i]) + (pLaser2->GetPosition().xy() - m_avLaserPos[j]), 0.0f));
                });
            }
            

            // 
            const coreUintW iIndex = pPlayer - g_pGame->GetPlayer(0u);
            coreFlow&       fTick  = m_afLaserTick[iIndex];

            // 
            fTick.Update(30.0f);
            if(fTick >= 1.0f)
            {
                // 
                if(i == m_iPoleIndex) m_iPoleCount += F_TO_UI(fTick);
                fTick = FRACT(fTick);

                // 
                g_pSpecialEffects->CreateSplashColor(coreVector3(vIntersection, 0.0f), 5.0f, 1u, COLOR_ENERGY_PURPLE);
            }
        });

        // 
        //m_avLaserPos[i] = pLaser->GetPosition ().xy();
        //m_avLaserDir[i] = pLaser->GetDirection().xy();
    }
    
    
    // 
    for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
    {
        coreObject3D* pLaser = (*m_Laser    .List())[i];

        m_avLaserPos[i] = pLaser->GetPosition ().xy();
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
            oShadow.SetSize(coreVector3(1.0f,1.0f,1.0f) * 0.25f * pOwner->GetPosition().z * (pOwner->GetSize().z / 1.4f));
        }

        // 
        else oShadow.SetAlpha(MAX(oShadow.GetAlpha() - 5.0f*TIME, 0.0f));

        // 
        if(!oShadow.GetAlpha()) this->DisableShadow(i, false);
    }

    // 
    m_Shadow.MoveNormal();

    // 
    for(coreUintW i = 0u; i < VIRIDO_BEANS; ++i)
    {
        coreObject3D& oBean = m_aBeanRaw[i];
        if(!oBean.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/7.0f);
        const coreVector2 vDir    = coreVector2::Direction((8.0f*PI) * m_fAnimation * ((i % 2u) ? -1.0f : 1.0f));

        // 
        oBean.SetDirection(coreVector3(vDir, 0.0f));
        oBean.SetTexOffset(coreVector2(0.0f, FRACT(0.8f * m_fAnimation + fOffset)));
    }

    // 
    m_Bean.MoveNormal();

    // 
    if(!m_Globe.HasStatus(ENEMY_STATUS_DEAD))
    {
        m_Globe.SetTexOffset(coreVector2(0.0f, m_fAnimation));
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