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
cDemoMission::cDemoMission()noexcept
: m_Laser        (VIRIDO_LASERS)
, m_LaserWave    (VIRIDO_LASERS)
, m_apLaserOwner {}
, m_Tile         (NEVO_TILES)
, m_afTileTime   {}
, m_Plate        (RUTILUS_PLATES_RAWS)
, m_afPlateTime  {}
, m_avPlateData  {}
, m_fAreaTime    (0.0f)
, m_fAnimation   (0.0f)
{
    // 
    m_apBoss[0] = &m_ProjectOne;

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
    m_Tile.DefineProgram("object_tile_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_TILES_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pTile = &m_aTileRaw[i];
            pTile->DefineModel  (Core::Manager::Object->GetLowQuad());
            pTile->DefineTexture(0u, "menu_background_black.png");
            pTile->DefineProgram("object_tile_program");

            // set object properties
            pTile->SetAlpha  (0.7f);
            pTile->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Tile.BindObject(pTile);
        }
    }

    // 
    m_Plate.DefineProgram("object_plate_inst_program");
    {
        for(coreUintW i = 0u; i < RUTILUS_PLATES_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pPlate = &m_aPlateRaw[i];
            pPlate->DefineModel  (Core::Manager::Object->GetLowQuad());
            pPlate->DefineTexture(0u, "effect_arrow.png");
            pPlate->DefineProgram("object_plate_program");

            // set object properties
            pPlate->SetColor3 ((i % 2u) ? COLOR_ENERGY_BLUE : COLOR_ENERGY_RED);
            pPlate->SetAlpha  (0.7f);
            pPlate->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Plate.BindObject(pPlate);
        }
    }

    // 
    for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i)
    {
        // 
        m_aArea[i].DefineModel  ("object_sphere.md3");
        m_aArea[i].DefineTexture(0u, "effect_energy.png");
        m_aArea[i].DefineProgram(i ? "effect_energy_flat_spheric_program" : "effect_energy_flat_program");
        m_aArea[i].SetColor3    (COLOR_ENERGY_PURPLE * (i ? 1.0f : 0.6f));
        m_aArea[i].SetTexSize   (coreVector2(2.4f,0.4f) * 5.0f);
        m_aArea[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    g_pGlow->BindList(&m_Laser);
    g_pGlow->BindList(&m_LaserWave);
}


// ****************************************************************
// destructor
cDemoMission::~cDemoMission()
{
    // 
    g_pGlow->UnbindList(&m_Laser);
    g_pGlow->UnbindList(&m_LaserWave);

    // 
    for(coreUintW i = 0u; i < VIRIDO_LASERS;  ++i) this->DisableLaser(i, false);
    for(coreUintW i = 0u; i < NEVO_TILES;     ++i) this->DisableTile (i, false);
    for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i) this->DisablePlate(i, false);
    this->DisableArea(false);
}


// ****************************************************************
// 
void cDemoMission::EnableLaser(const coreUintW iIndex, const cShip* pOwner)
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
void cDemoMission::DisableLaser(const coreUintW iIndex, const coreBool bAnimated)
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
void cDemoMission::EnableTile(const coreUintW iIndex, const coreUintW iDimension)
{
    ASSERT(iIndex < NEVO_TILES)
    coreObject3D& oTile = m_aTileRaw[iIndex];

    // 
    WARN_IF(oTile.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afTileTime[iIndex] = 1.0f;

    // 
    const coreFloat   fScale = RCP(I_TO_F(iDimension)) * FOREGROUND_AREA.x * 2.2f;
    const coreVector2 vPos   = (coreVector2(I_TO_F(iIndex % iDimension), I_TO_F(iIndex / iDimension)) - 0.5f * I_TO_F(iDimension - 1u)).InvertedY() * fScale;

    // 
    oTile.SetPosition(coreVector3(vPos, 0.0f));
    oTile.SetSize    (coreVector3(0.0f, 0.0f, fScale - 1.0f));
    oTile.SetEnabled (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cDemoMission::DisableTile(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_TILES)
    coreObject3D& oTile = m_aTileRaw[iIndex];

    // 
    if(!oTile.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_afTileTime[iIndex] > 0.0f) m_afTileTime[iIndex] = -1.0f;

    // 
    if(!bAnimated) oTile.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cDemoMission::EnablePlate(const coreUintW iIndex, const coreFloat fFrom, const coreFloat fTo, const coreFloat fScale)
{
    ASSERT(iIndex < RUTILUS_PLATES)
    coreObject3D& oPlate = m_aPlateRaw[iIndex];

    // 
    WARN_IF(oPlate.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afPlateTime[iIndex] = 0.0f;
    m_avPlateData[iIndex] = coreVector4(fFrom, fTo, 0.0f, fScale);

    // 
    oPlate.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cDemoMission::DisablePlate(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < RUTILUS_PLATES)
    coreObject3D& oPlate = m_aPlateRaw[iIndex];

    // 
    if(!oPlate.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_avPlateData[iIndex].w != 0.0f) this->SetPlateScale(iIndex, 0.0f);

    // 
    if(!bAnimated) oPlate.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cDemoMission::EnableArea()
{
    coreObject3D& oArea = m_aArea[0];

    // 
    WARN_IF(oArea.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_fAreaTime = 1.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i) nInitFunc(&oArea + i);
}


// ****************************************************************
// 
void cDemoMission::DisableArea(const coreBool bAnimated)
{
    coreObject3D& oArea = m_aArea[0];

    // 
    if(!oArea.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_fAreaTime > 0.0f) m_fAreaTime = -1.0f;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            g_pGlow->UnbindObject(pObject);
        };
        for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i) nExitFunc(&oArea + i);
    }
}


// ****************************************************************
// 
void cDemoMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Tile.Render();

        // 
        m_Plate.Render();

        //
        for(coreUintW i = 0u; i < RUTILUS_AREAS; ++i) m_aArea[i].Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cDemoMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_LaserWave.Render();
    }
    glDepthMask(true);

    // 
    m_Laser.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Laser);
}


// ****************************************************************
// 
void cDemoMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);

    // 
    for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
    {
        coreObject3D* pLaser = (*m_Laser    .List())[i];
        coreObject3D* pWave  = (*m_LaserWave.List())[i];
        if(!pLaser->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat fValue = FRACT(2.0f * m_fAnimation);

        // 
        pLaser->SetTexOffset(coreVector2(3.0f,4.0f) * FRACT(0.8f * 0.2 * m_fAnimation));

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
    for(coreUintW i = 0u; i < NEVO_TILES; ++i)
    {
        coreObject3D& oTile = m_aTileRaw[i];
        if(!oTile.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        coreFloat fScale;
        if(m_afTileTime[i] > 0.0f)
        {
            // 
            m_afTileTime[i].UpdateMin(2.0f, 2.0f);
            fScale = LERPB(0.0f, 1.0f, m_afTileTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afTileTime[i].UpdateMax(-2.0f, -2.0f);
            fScale = LERPB(1.0f, 0.0f, -m_afTileTime[i] - 1.0f);

            // 
            if(m_afTileTime[i] <= -2.0f) this->DisableTile(i, false);
        }

        // 
        oTile.SetSize(coreVector3(fScale, fScale, 1.0f) * oTile.GetSize().z);
    }

    // 
    m_Tile.MoveNormal();

    // 
    for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
    {
        coreObject3D& oPlate = m_aPlateRaw[i];
        if(!oPlate.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afPlateTime[i].UpdateMin(1.0f, 1.0f);
        const coreFloat fOffset = LERPB(    m_avPlateData[i].x,      m_avPlateData[i].y,  m_afPlateTime[i]);
        const coreFloat fScale  = LERPB(ABS(m_avPlateData[i].z), ABS(m_avPlateData[i].w), m_afPlateTime[i]);

        // 
        if(fScale <= 0.0f) this->DisablePlate(i, false);

        // 
        const coreVector2 vSize = coreVector2(fScale, (m_avPlateData[i].w <= 0.0f) ? 1.0f : fScale);
        const coreVector2 vReal = IsHorizontal(oPlate.GetDirection().xy()) ? vSize : vSize.yx();

        // 
        oPlate.SetPosition (coreVector3(0.0f, fOffset, 0.0f) * (FOREGROUND_AREA.y * 2.2f));
        oPlate.SetSize     (coreVector3(vReal,         1.0f) * (FOREGROUND_AREA.y * 2.2f));
        oPlate.SetTexSize  (vReal * 4.0f);
        oPlate.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.4f) - vReal * 2.0f);
    }

    // 
    m_Plate.MoveNormal();

    // 
    if(m_aArea[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        coreFloat fScale;
        if(m_fAreaTime > 0.0f)
        {
            // 
            m_fAreaTime.UpdateMin(2.0f, 2.0f);
            fScale = LERPB(0.0f, 1.0f, m_fAreaTime - 1.0f);
        }
        else
        {
            // 
            m_fAreaTime.UpdateMax(-1.0f/0.6f, -2.0f);
            fScale = ParaLerp(1.0f, 1.5f, 0.0f, -m_fAreaTime - 1.0f);

            // 
            if(m_fAreaTime <= -2.0f) this->DisableArea(false);
        }

        // 
        m_aArea[0].SetSize     (coreVector3(20.0f,20.0f,20.0f) * fScale);
        m_aArea[0].SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.1f));
        m_aArea[0].Move();
    }

    // 
    if(m_aArea[1].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_aArea[1].SetPosition (m_aArea[0].GetPosition());
        m_aArea[1].SetSize     (m_aArea[0].GetSize    () * 0.8f);
        m_aArea[1].SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.1f));
        m_aArea[1].Move();
    }
    STATIC_ASSERT(RUTILUS_AREAS == 2u)

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, TYPE_VIRIDO_LASER, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pLaser, const coreVector3 vIntersection, const coreBool bFirstHit)
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
}