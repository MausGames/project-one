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
// constructor
cViridoMission::cViridoMission()noexcept
: m_Ball           (VIRIDO_BALLS)
, m_BallTrail      (VIRIDO_BALLS * VIRIDO_TRAILS)
, m_apPaddleOwner  {}
, m_Barrier        (VIRIDO_BARRIERS)
, m_apBarrierOwner {}
, m_aiBarrierDir   {}
, m_Laser          (VIRIDO_LASERS)
, m_LaserWave      (VIRIDO_LASERS)
, m_apLaserOwner   {}
, m_Shadow         (VIRIDO_SHADOWS)
, m_apShadowOwner  {}
, m_iRealState     (0u)
, m_iStickyState   (0u)
, m_iBounceState   (0u)
, m_fAnimation     (0.0f)
{
    // 
    m_apBoss[0] = &m_Dharuk;
    m_apBoss[1] = &m_Torus;
    m_apBoss[2] = &m_Vaus;

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
            pBall->SetSize   (coreVector3(2.7f,2.7f,2.7f));
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
    g_pGlow->BindList(&m_Ball);
    g_pGlow->BindList(&m_BallTrail);
    g_pGlow->BindList(&m_Barrier);
    g_pGlow->BindList(&m_Laser);
    g_pGlow->BindList(&m_LaserWave);
}


// ****************************************************************
// destructor
cViridoMission::~cViridoMission()
{
    // 
    g_pGlow->UnbindList(&m_Ball);
    g_pGlow->UnbindList(&m_BallTrail);
    g_pGlow->UnbindList(&m_Barrier);
    g_pGlow->UnbindList(&m_Laser);
    g_pGlow->UnbindList(&m_LaserWave);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS;    ++i) this->DisableBall   (i, false);
    for(coreUintW i = 0u; i < VIRIDO_PADDLES;  ++i) this->DisablePaddle (i, false);
    for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i) this->DisableBarrier(i, false);
    for(coreUintW i = 0u; i < VIRIDO_LASERS;   ++i) this->DisableLaser  (i, false);
    for(coreUintW i = 0u; i < VIRIDO_SHADOWS;  ++i) this->DisableShadow (i, false);
}


// ****************************************************************
// 
void cViridoMission::EnableBall(const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection)
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
void cViridoMission::EnableBarrier(const coreUintW iIndex, const cShip* pOwner, const coreVector2& vDirection, const coreFloat fSize)
{
    ASSERT(iIndex < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

    // 
    WARN_IF(oBarrier.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oBarrier.ChangeType(TYPE_VIRIDO_BARRIER);

    // 
    ASSERT(pOwner)
    m_apBarrierOwner[iIndex] = pOwner;
    m_aiBarrierDir  [iIndex] = PackDirection(vDirection);

    // 
    oBarrier.SetSize   (coreVector3(7.5f * fSize, 2.5f, 2.5f));
    oBarrier.SetTexSize(coreVector2(1.2f * fSize, 0.25f) * 0.5f);
    oBarrier.SetAlpha  (0.0f);
    oBarrier.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cViridoMission::DisableBarrier(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

    // 
    if(!oBarrier.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oBarrier.ChangeType(0);

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
    pLaser->ChangeType(TYPE_VIRIDO_LASER);

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
    pLaser->ChangeType(0);

    // 
    m_apLaserOwner[iIndex] = NULL;

    // 
    pLaser->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) for(coreUintW j = 80u; j--; ) g_pSpecialEffects->CreateSplashColor(pLaser->GetPosition() + pLaser->GetDirection() * (2.0f * I_TO_F(j - 40u)), 10.0f, 1u, COLOR_ENERGY_PURPLE);
}


// ****************************************************************
// 
void cViridoMission::EnableShadow(const coreUintW iIndex, const cShip* pOwner, const coreVector2& vPosition)
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

    DEPTH_PUSH

    // 
    m_Laser.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Laser);
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
        const coreVector2 vNewPos = pBall->GetPosition ().xy() + vNewDir * FOREGROUND_AREA * (CONTAINS_BIT(m_iStickyState, 1u) ? 0.0f : (VIRIDO_BALL_SPEED * Core::System->GetTime()));

        // restrict movement to the foreground area
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
            pTrail->SetAlpha(pTrail->GetAlpha() - 2.0f * Core::System->GetTime());
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
        if(pOwner) oPaddle.SetAlpha(MIN(oPaddle.GetAlpha() + 5.0f*Core::System->GetTime(), 1.0f));
              else oPaddle.SetAlpha(MAX(oPaddle.GetAlpha() - 5.0f*Core::System->GetTime(), 0.0f));

        // 
        if(!oPaddle.GetAlpha()) this->DisablePaddle(i, false);

        // 
        oPaddle.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
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
        const cShip* pOwner = m_apBarrierOwner[i];
        if(pOwner)
        {
            const coreVector2 vDir = UnpackDirection(m_aiBarrierDir[i]);

            oBarrier.SetPosition (coreVector3(pOwner->GetPosition().xy() + vDir * 7.0f, 0.0f));
            oBarrier.SetDirection(coreVector3(vDir, 0.0f));
        }

        // 
        if(pOwner) oBarrier.SetAlpha(MIN(oBarrier.GetAlpha() + 5.0f*Core::System->GetTime(), 1.0f));
              else oBarrier.SetAlpha(MAX(oBarrier.GetAlpha() - 5.0f*Core::System->GetTime(), 0.0f));

        // 
        if(!oBarrier.GetAlpha()) this->DisableBarrier(i, false);

        // 
        oBarrier.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
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
        const coreFloat fValue = FRACT(10.0f * m_fAnimation);

        // 
        pLaser->SetTexOffset(coreVector2(3.0f,4.0f) * FRACT(0.8f * m_fAnimation));

        // 
        pWave->SetPosition (pLaser->GetPosition ());
        pWave->SetSize     (pLaser->GetSize     () * (1.0f + 3.0f * fValue));
        pWave->SetDirection(pLaser->GetDirection());
        pWave->SetAlpha    (1.0f - fValue);
        pWave->SetTexOffset(pLaser->GetTexOffset());
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
            oShadow.SetSize(coreVector3(1.0f,1.0f,1.0f) * 0.25f * pOwner->GetPosition().z);
        }

        // 
        else oShadow.SetAlpha(MAX(oShadow.GetAlpha() - 5.0f*Core::System->GetTime(), 0.0f));

        // 
        if(!oShadow.GetAlpha()) this->DisableShadow(i, false);
    }

    // 
    m_Shadow.MoveNormal();

    // 
    m_iBounceState = 0u;
    STATIC_ASSERT(VIRIDO_PADDLES+1u <= sizeof(m_iBounceState)*8u)

    // 
    if(!CONTAINS_BIT(m_iStickyState, 1u))
    {
        // 
        Core::Manager::Object->TestCollision(TYPE_VIRIDO_PADDLE, TYPE_VIRIDO_BALL, [this](coreObject3D* OUTPUT pPaddle, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!bFirstHit || (coreVector2::Dot(pPaddle->GetDirection().xy(), pBall->GetDirection().xy()) >= 0.0f))
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

                if(CONTAINS_BIT(m_iRealState, iIndex))
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
    if(CONTAINS_BIT(m_iStickyState, 1u))
    {
        // only between first ball and first paddle
        coreObject3D& oBall   = m_aBallRaw[0];
        coreObject3D& oPaddle = m_aPaddle [0];

        // 
        oBall.SetPosition(oPaddle.GetPosition() + oPaddle.GetDirection() * (oPaddle.GetCollisionRange().y * 2.0f - 0.3f));
        oBall.Move();
    }

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_VIRIDO_BALL, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(10, ELEMENT_GREEN, vIntersection.xy());

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_GREEN);
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_VIRIDO_BALL, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pBullet->Reflect(pBall, vIntersection.xy());
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, TYPE_VIRIDO_BARRIER, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBarrier, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->ApplyForce(pBarrier->GetDirection().xy() * 100.0f);

        // 
        g_pSpecialEffects->CreateSplashColor(vIntersection, 5.0f, 3u, COLOR_ENERGY_BLUE);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_VIRIDO_BARRIER, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pBarrier, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pBullet->Reflect(pBarrier, vIntersection.xy(), pBarrier->GetDirection().xy());
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, TYPE_VIRIDO_LASER, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pLaser, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        const coreVector2 vDiff  = pPlayer->GetOldPos() - pLaser->GetPosition().xy();
        const coreVector2 vHit   = coreVector2::Dot(vDiff, pLaser->GetDirection().xy()) * pLaser->GetDirection().xy() + pLaser->GetPosition().xy();
        const coreVector2 vForce = pPlayer->GetOldPos() - vHit;

        // 
        pPlayer->ApplyForce(vForce.Normalized() * 100.0f);

        // 
        g_pSpecialEffects->CreateSplashColor(vIntersection, 5.0f, 3u, COLOR_ENERGY_PURPLE);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
    });

    // 
    if(!CONTAINS_FLAG(m_Vaus.GetStatus(), ENEMY_STATUS_DEAD))
    {
        cEnemy*     pCurEnemy  = NULL;
        coreFloat   fCurLenSq  = FLT_MAX;
        coreVector2 vCurImpact = coreVector2(FLT_MAX,FLT_MAX);

        // only first ball will be active in this mission stage
        coreObject3D& oBall = m_aBallRaw[0];

        // 
        const coreVector2 vBallPos    = oBall.GetPosition ().xy();
        const coreVector2 vBallDir    = oBall.GetDirection().xy();
        const coreVector2 vOldBallPos = vBallPos - vBallDir * FOREGROUND_AREA * (CONTAINS_BIT(m_iStickyState, 1u) ? 0.0f : (VIRIDO_BALL_SPEED * Core::System->GetTime()));

        // 
        Core::Manager::Object->TestCollision(TYPE_ENEMY, &oBall, [&](cEnemy* OUTPUT pEnemy, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
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
            oBall.SetPosition (coreVector3(vBallPos + vDiff * (3.0f * Core::System->GetTime()), 0.0f));
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
void cViridoMission::__BounceEffect(const coreVector2& vEffectPos)
{
    g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
    g_pSpecialEffects->PlaySound        (coreVector3(vEffectPos, 0.0f), 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
}