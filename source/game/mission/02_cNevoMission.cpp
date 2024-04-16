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
cNevoMission::cNevoMission()noexcept
: m_Bomb           (NEVO_BOMBS)
, m_afBombTime     {}
, m_iBombGone      (0u)
, m_Blast          (NEVO_BLASTS)
, m_BlastLine      (NEVO_BLASTS * NEVO_LINES)
, m_afBlastTime    {}
, m_Tile           (NEVO_TILES)
, m_afTileTime     {}
, m_Arrow          (NEVO_ARROWS)
, m_apArrowOwner   {}
, m_afArrowAlpha   {}
, m_aiArrowDir     {}
, m_iArrowActive   (0u)
, m_iArrowFake     (0u)
, m_bArrowEnabled  (true)
, m_Block          (NEVO_BLOCKS)
, m_BlockWave      (NEVO_BLOCKS)
, m_apBlockOwner   {}
, m_afBlockScale   {}
, m_afBlockRota    {}
, m_afScrapTime    {}
, m_Chip           (NEVO_CHIPS)
, m_ChipWave       (NEVO_CHIPS)
, m_vForce         (coreVector2(0.0f,0.0f))
, m_vImpact        (coreVector2(0.0f,0.0f))
, m_bClamp         (false)
, m_bOverdraw      (false)
, m_fDemoRangeAnim (0.0f)
, m_fAnimation     (0.0f)
{
    // 
    m_apBoss[0] = &m_Leviathan;

    // 
    m_Bomb.DefineProgram("object_ship_glow_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_BOMBS_RAWS; ++i)
        {
            // load object resources
            cLodObject* pBomb = &m_aBombRaw[i];
            pBomb->DefineModelHigh("object_bomb_high.md3");
            pBomb->DefineModelLow ("object_bomb_low.md3");
            pBomb->DefineTexture  (0u, "ship_enemy.png");
            pBomb->DefineProgram  ("object_ship_glow_program");

            // set object properties
            pBomb->SetColor3 (COLOR_ENERGY_YELLOW);
            pBomb->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Bomb.BindObject(pBomb);
        }
    }

    // 
    m_Blast    .DefineProgram("effect_energy_flat_inst_program");
    m_BlastLine.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_BLASTS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (NEVO_LINES + 1u);

            // load object resources
            coreObject3D* pBlast = &m_aBlastRaw[i];
            pBlast->DefineModel  (iType ? "object_tube_open.md3" : "object_sphere.md3");
            pBlast->DefineTexture(0u, "effect_energy.png");
            pBlast->DefineProgram("effect_energy_flat_program");

            // set object properties
            pBlast->SetSize   (iType ? coreVector3(0.0f,80.0f,0.0f) : coreVector3(5.0f,5.0f,5.0f));
            pBlast->SetColor3 (COLOR_ENERGY_ORANGE);   // # not fire
            pBlast->SetTexSize(iType ? coreVector2(0.6f,2.4f) : coreVector2(4.0f,4.0f));
            pBlast->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BlastLine.BindObject(pBlast);
                 else m_Blast    .BindObject(pBlast);
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
            pTile->SetAlpha    (0.7f);
            pTile->SetTexOffset((coreVector2(0.17f,0.31f) * I_TO_F(i)).Processed(FRACT));
            pTile->SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Tile.BindObject(pTile);
        }
    }

    // 
    m_Arrow.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_ARROWS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pArrow = &m_aArrowRaw[i];
            pArrow->DefineModel  ("object_arrow.md3");
            pArrow->DefineTexture(0u, "effect_energy.png");
            pArrow->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pArrow->SetSize   (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
            pArrow->SetColor3 (COLOR_ENERGY_GREEN * 0.5f);
            pArrow->SetTexSize(coreVector2(0.5f,0.2f) * 1.2f);
            pArrow->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Arrow.BindObject(pArrow);
        }
    }

#if defined(_P1_UNUSED_)

    // 
    m_Block    .DefineProgram("effect_energy_flat_spheric_inst_program");
    m_BlockWave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_BLOCKS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pBlock = &m_aBlockRaw[i];
            pBlock->DefineModel  ("object_tetra_top.md3");
            //pBlock->DefineModel  ("object_cube_top.md3");
            //pBlock->DefineModel  ("object_sphere.md3");
            pBlock->DefineTexture(0u, "effect_energy.png");
            pBlock->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pBlock->SetColor3 (COLOR_ENERGY_ORANGE);
            pBlock->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BlockWave.BindObject(pBlock);
                 else m_Block    .BindObject(pBlock);
        }
    }

#endif

    // 
    for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i)
    {
        m_aScrap[i].DefineModel  ("object_sphere.md3");
        m_aScrap[i].DefineTexture(0u, "effect_energy.png");
        m_aScrap[i].DefineProgram("effect_energy_flat_invert_program");
        m_aScrap[i].SetSize      (coreVector3(1.0f,1.0f,1.0f) * 2.5f);
        m_aScrap[i].SetColor3    (COLOR_ENERGY_YELLOW * 0.8f);
        m_aScrap[i].SetTexSize   (coreVector2(1.2f,0.25f) * 3.0f);
        m_aScrap[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Chip    .DefineProgram("effect_energy_flat_spheric_inst_program");
    m_ChipWave.DefineProgram("effect_energy_flat_inst_program");
    {
        for(coreUintW i = 0u; i < NEVO_CHIPS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pChip = &m_aChipRaw[i];
            pChip->DefineModel  ("object_cube_top.md3");
            pChip->DefineTexture(0u, "effect_energy.png");
            pChip->DefineProgram(iType ? "effect_energy_flat_program" : "effect_energy_flat_spheric_program");

            // set object properties
            pChip->SetColor3 (COLOR_ENERGY_BLUE * 0.9f);
            pChip->SetTexSize(coreVector2(1.0f,1.0f) * 0.4f);
            pChip->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_ChipWave.BindObject(pChip);
                 else m_Chip    .BindObject(pChip);
        }
    }

    // 
    m_Guide.DefineModel  ("bullet_cone.md3");
    m_Guide.DefineTexture(0u, "effect_energy.png");
    m_Guide.DefineProgram("effect_energy_flat_program");
    m_Guide.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 2.5f);
    m_Guide.SetColor3    (COLOR_ENERGY_PURPLE);
    m_Guide.SetTexSize   (coreVector2(1.0f,0.5f) * 0.7f);
    m_Guide.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Trend.DefineModel  ("object_tetra_top.md3");
    m_Trend.DefineTexture(0u, "effect_energy.png");
    m_Trend.DefineProgram("effect_energy_flat_program");
    m_Trend.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 6.0f);
    m_Trend.SetColor3    (COLOR_ENERGY_RED * 0.8f);
    m_Trend.SetTexSize   (coreVector2(1.0f,1.0f) * 0.5f);
    m_Trend.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

#if defined(_P1_UNUSED_)

    // 
    m_Container.DefineModelHigh("object_container_high.md3");
    m_Container.DefineModelLow ("object_container_low.md3");
    m_Container.DefineTexture  (0u, "ship_enemy.png");
    m_Container.DefineProgram  ("object_ship_program");
    m_Container.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
    m_Container.SetColor3      (COLOR_SHIP_GREY);
    m_Container.SetEnabled     (CORE_OBJECT_ENABLE_NOTHING);

#endif

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRange); ++i)
    {
        m_aDemoRange[i].DefineModel  (i ? "object_cube_top.md3" : "object_penta_top.md3");
        m_aDemoRange[i].DefineTexture(0u, "effect_energy.png");
        m_aDemoRange[i].DefineProgram("effect_energy_flat_invert_program");
        m_aDemoRange[i].SetPosition  (coreVector3(0.0f, i ? -0.2f : 0.2f, 0.0f) * FOREGROUND_AREA3);
        m_aDemoRange[i].SetColor3    (i ? COLOR_PLAYER_BLUE : COLOR_PLAYER_GREEN);
        m_aDemoRange[i].SetTexSize   (coreVector2(0.1f,0.1f));
        m_aDemoRange[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    cShadow::GetGlobalContainer()->BindList(&m_Bomb);
    g_pGlow->BindList(&m_Blast);
    g_pGlow->BindList(&m_BlastLine);
    g_pGlow->BindList(&m_Arrow);
    g_pGlow->BindList(&m_Block);
    g_pGlow->BindList(&m_BlockWave);
    g_pGlow->BindList(&m_Chip);
    g_pGlow->BindList(&m_ChipWave);

    if(g_bDemoVersion)
    {
        // 
        constexpr const coreChar* apcName[] =
        {
            "default_normal.png",
            "environment_animal_diff.png",
            "environment_animal_norm.png",
            "environment_block_diff.png",
            "environment_block_norm.png",
            "environment_dust_diff.png",
            "environment_earth_diff.png",
            "environment_particle_02.png",
            "environment_sea.png",

            "environment_animal_01.md3",
            "environment_animal_02.md3",
            "environment_sea_01.md3",
            "environment_sea_02.md3",
            "environment_sea_03.md3",
            "environment_sea_04.md3",
            "object_cube_ice.md3",

            "effect_decal_single_program",
            "effect_decal_single_inst_program",
            "environment_under_program",
            "object_ground_program",
            "object_ground_inst_program",
            "object_wave_program",
            "object_wave_inst_program"
        };
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_apResCache); ++i)
        {
            m_apResCache[i] = Core::Manager::Resource->Get<coreResourceDummy>(apcName[i]);
            STATIC_ASSERT(ARRAY_SIZE(m_apResCache) == ARRAY_SIZE(apcName))
        }

        // 
        m_pNightmareSound = Core::Manager::Resource->Get<coreSound>("effect_nightmare.wav");
    }
}


// ****************************************************************
// destructor
cNevoMission::~cNevoMission()
{
    // 
    cShadow::GetGlobalContainer()->UnbindList(&m_Bomb);
    g_pGlow->UnbindList(&m_Blast);
    g_pGlow->UnbindList(&m_BlastLine);
    g_pGlow->UnbindList(&m_Arrow);
    g_pGlow->UnbindList(&m_Block);
    g_pGlow->UnbindList(&m_BlockWave);
    g_pGlow->UnbindList(&m_Chip);
    g_pGlow->UnbindList(&m_ChipWave);

    // 
    for(coreUintW i = 0u; i < NEVO_BOMBS;  ++i) this->DisableBomb (i, false);
    for(coreUintW i = 0u; i < NEVO_BLASTS; ++i) this->DisableBlast(i, false);
    for(coreUintW i = 0u; i < NEVO_TILES;  ++i) this->DisableTile (i, false);
    for(coreUintW i = 0u; i < NEVO_ARROWS; ++i) this->DisableArrow(i, false);
    for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i) this->DisableBlock(i, false);
    for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i) this->DisableScrap(i, false);
    for(coreUintW i = 0u; i < NEVO_CHIPS;  ++i) this->DisableChip (i, false);
    this->DisableGuide    (false);
    this->DisableTrend    (false);
    this->DisableContainer(false);
    this->DisableRanges   (false);
}


// ****************************************************************
// 
void cNevoMission::EnableBomb(const coreUintW iIndex, const coreBool bGrow)
{
    ASSERT(iIndex < NEVO_BOMBS)
    coreObject3D& oBomb = m_aBombRaw[iIndex];

    // 
    WARN_IF(oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBomb(iIndex, false);

    // 
    m_afBombTime[iIndex] = 0.0f;

    // 
    oBomb.SetSize   (coreVector3(1.0f,1.0f,1.0f) * (bGrow ? 0.0f : NEVO_BOMB_SIZE));
    oBomb.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cNevoMission::DisableBomb(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_BOMBS)
    coreObject3D& oBomb = m_aBombRaw[iIndex];

    // 
    if(!oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ADD_BIT(m_iBombGone, iIndex)
    STATIC_ASSERT(NEVO_BOMBS <= sizeof(m_iBombGone)*8u)

    // 
    oBomb.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalColorSmall(oBomb.GetPosition(), COLOR_FIRE_ORANGE);
}


// ****************************************************************
// 
void cNevoMission::EnableBlast(const coreUintW iIndex)
{
    ASSERT(iIndex < NEVO_BLASTS)
    coreObject3D* pBlast = (*m_Blast    .List())[iIndex];
    coreObject3D* pLine  = (*m_BlastLine.List())[iIndex*NEVO_LINES];

    // 
    WARN_IF(pBlast->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBlast(iIndex, false);

    // 
    m_afBlastTime[iIndex] = 0.0f;

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pBlast);
    for(coreUintW i = 0u; i < NEVO_LINES; ++i) nInitFunc(pLine + i);
}


// ****************************************************************
// 
void cNevoMission::DisableBlast(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_BLASTS)
    coreObject3D* pBlast = (*m_Blast    .List())[iIndex];
    coreObject3D* pLine  = (*m_BlastLine.List())[iIndex*NEVO_LINES];

    // 
    if(!pBlast->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    if(!bAnimated || (m_afBlastTime[iIndex] < 1.0f))
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        };
        nExitFunc(pBlast);
        for(coreUintW i = 0u; i < NEVO_LINES; ++i) nExitFunc(pLine + i);

        if(bAnimated)
        {
            for(coreUintW i = 0u; i < NEVO_LINES; ++i)
            {
                // 
                const coreVector3 vPos = pBlast ->GetPosition();
                const coreVector3 vDir = pLine[i].GetDirection();
                const coreUintW   iNum = F_TO_UI(g_pForeground->RayIntersection(vPos.xy(), vDir.xy()) * 0.5f);

                // 
                for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (I_TO_F(j) / 0.5f), 10.0f, 1u, COLOR_ENERGY_ORANGE);
            }
        }
    }
}


// ****************************************************************
// 
void cNevoMission::EnableTile(const coreUintW iIndex, const coreUintW iDimension)
{
    ASSERT(iIndex < NEVO_TILES)
    coreObject3D& oTile = m_aTileRaw[iIndex];

    // 
    WARN_IF(oTile.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableTile(iIndex, false);

    // 
    m_afTileTime[iIndex] = 1.0f;

    // 
    const coreFloat   fScale = RCP(I_TO_F(iDimension)) * FOREGROUND_AREA.x * 2.2f;
    const coreVector2 vPos   = (coreVector2(I_TO_F(iIndex % iDimension), I_TO_F(iIndex / iDimension)) - 0.5f * I_TO_F(iDimension - 1u)).InvertedY() * fScale;

    // 
    oTile.SetPosition (coreVector3(vPos, 0.0f));
    oTile.SetSize     (coreVector3(0.0f, 0.0f, fScale - 1.0f));
    oTile.SetDirection(coreVector3(0.0f, 1.0f, 0.0f));
    oTile.SetEnabled  (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cNevoMission::DisableTile(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_TILES)
    coreObject3D& oTile = m_aTileRaw[iIndex];

    // 
    if(!oTile.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_afTileTime[iIndex] > 0.0f)
    {
        m_afTileTime[iIndex] = -1.0f;
        oTile.SetSize(coreVector3(1.0f,1.0f,1.0f) * oTile.GetSize().x);
    }

    // 
    if(!bAnimated) oTile.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cNevoMission::EnableArrow(const coreUintW iIndex, const cShip* pOwner, const coreVector2 vDirection)
{
    ASSERT(iIndex < NEVO_ARROWS)
    coreObject3D& oArrow = m_aArrowRaw[iIndex];

    // 
    WARN_IF(oArrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableArrow(iIndex, false);

    // 
    ASSERT(pOwner)
    m_apArrowOwner[iIndex] = pOwner;
    m_afArrowAlpha[iIndex] = 0.0f;
    m_aiArrowDir  [iIndex] = PackDirection(vDirection);

    // 
    REMOVE_BIT(m_iArrowFake, iIndex)
    STATIC_ASSERT(NEVO_ARROWS <= sizeof(m_iArrowFake)*8u)

    // 
    oArrow.SetAlpha  (0.0f);
    oArrow.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cNevoMission::DisableArrow(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_ARROWS)
    coreObject3D& oArrow = m_aArrowRaw[iIndex];

    // 
    if(!oArrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_apArrowOwner[iIndex] = NULL;

    // 
    if(!bAnimated) oArrow.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cNevoMission::EnableBlock(const coreUintW iIndex, const cShip* pOwner, const coreFloat fScale)
{
#if defined(_P1_UNUSED_)

    ASSERT(iIndex < NEVO_BLOCKS)
    coreObject3D* pBlock = (*m_Block    .List())[iIndex];
    coreObject3D* pWave  = (*m_BlockWave.List())[iIndex];

    // 
    WARN_IF(pBlock->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBlock(iIndex, false);
    pBlock->ChangeType(TYPE_NEVO_BLOCK);

    // 
    ASSERT(pOwner)
    m_apBlockOwner[iIndex] = pOwner;
    m_afBlockScale[iIndex] = fScale;
    m_afBlockRota [iIndex] = 0.0f;

    // 
    pBlock->SetSize   (coreVector3(0.0f,0.0f,0.0f));
    pBlock->SetTexSize(coreVector2(3.0f,1.2f) * 0.055f * ABS(fScale));
    //pBlock->SetTexSize(coreVector2(3.0f,1.2f) * 0.55f * ABS(fScale));
    pBlock->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave ->SetTexSize(pBlock->GetTexSize());
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_ALL);

#endif
}


// ****************************************************************
// 
void cNevoMission::DisableBlock(const coreUintW iIndex, const coreBool bAnimated)
{
#if defined(_P1_UNUSED_)

    ASSERT(iIndex < NEVO_BLOCKS)
    coreObject3D* pBlock = (*m_Block    .List())[iIndex];
    coreObject3D* pWave  = (*m_BlockWave.List())[iIndex];

    // 
    if(!pBlock->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    pBlock->ChangeType(0);

    // 
    m_apBlockOwner[iIndex] = NULL;

    // 
    if(!bAnimated)
    {
        pBlock->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pWave ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

#endif
}


// ****************************************************************
// 
void cNevoMission::EnableScrap(const coreUintW iIndex, const coreVector2 vPosition)
{
    ASSERT(iIndex < NEVO_SCRAPS)
    coreObject3D& oScrap = m_aScrap[iIndex];

    // 
    WARN_IF(oScrap.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableScrap(iIndex, false);

    // 
    m_afScrapTime[iIndex] = 0.0f;

    // 
    oScrap.SetPosition(coreVector3(vPosition, 0.0f));
    oScrap.SetEnabled (CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&oScrap);
}


// ****************************************************************
// 
void cNevoMission::DisableScrap(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_SCRAPS)
    coreObject3D& oScrap = m_aScrap[iIndex];

    // 
    if(!oScrap.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afScrapTime[iIndex] = 0.0f;

    // 
    oScrap.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&oScrap);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(oScrap.GetPosition(), COLOR_ENERGY_YELLOW);
}


// ****************************************************************
// 
void cNevoMission::EnableChip(const coreUintW iIndex)
{
    ASSERT(iIndex < NEVO_CHIPS)
    coreObject3D* pChip = (*m_Chip    .List())[iIndex];
    coreObject3D* pWave = (*m_ChipWave.List())[iIndex];

    // 
    WARN_IF(pChip->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableChip(iIndex, false);

    // 
    m_afChipTime[iIndex] = 0.0f;

    // 
    pChip->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cNevoMission::DisableChip(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < NEVO_CHIPS)
    coreObject3D* pChip = (*m_Chip    .List())[iIndex];
    coreObject3D* pWave = (*m_ChipWave.List())[iIndex];

    // 
    if(!pChip->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afChipTime[iIndex] = 0.0f;

    // 
    pChip->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(pChip->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_BLUE);
}


// ****************************************************************
// 
void cNevoMission::EnableGuide()
{
    // 
    WARN_IF(m_Guide.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableGuide(false);

    // 
    m_Guide.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Guide);
}


// ****************************************************************
// 
void cNevoMission::DisableGuide(const coreBool bAnimated)
{
    // 
    if(!m_Guide.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Guide.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Guide);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(m_Guide.GetPosition(), 25.0f, 10u, COLOR_ENERGY_PURPLE);
}


// ****************************************************************
// 
void cNevoMission::EnableTrend()
{
    // 
    WARN_IF(m_Trend.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableTrend(false);

    // 
    m_Trend.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Trend);
}


// ****************************************************************
// 
void cNevoMission::DisableTrend(const coreBool bAnimated)
{
    // 
    if(!m_Trend.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Trend.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Trend);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(m_Trend.GetPosition(), 25.0f, 10u, COLOR_ENERGY_RED);
}


// ****************************************************************
// 
void cNevoMission::EnableContainer(const coreVector2 vPosition)
{
#if defined(_P1_UNUSED_)

    // 
    WARN_IF(m_Container.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableContainer(false);
    m_Container.ChangeType(TYPE_NEVO_CONTAINER);

    // 
    m_bClamp    = false;
    m_bOverdraw = false;

    // 
    m_Container.SetPosition(coreVector3(vPosition, 0.0f));
    m_Container.SetEnabled (CORE_OBJECT_ENABLE_ALL);
    cShadow::GetGlobalContainer()->BindObject(&m_Container);

#endif
}


// ****************************************************************
// 
void cNevoMission::DisableContainer(const coreBool bAnimated)
{
#if defined(_P1_UNUSED_)

    // 
    if(!m_Container.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    m_Container.ChangeType(0);

    // 
    m_Container.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    cShadow::GetGlobalContainer()->UnbindObject(&m_Container);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalDarkBig(m_Container.GetPosition());

#endif
}


// ****************************************************************
// 
void cNevoMission::EnableRanges()
{
    // 
    WARN_IF(m_aDemoRange[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableRanges(false);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRange); ++i)
    {
        m_aDemoRange[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(&m_aDemoRange[i]);
    }
}


// ****************************************************************
// 
void cNevoMission::DisableRanges(const coreBool bAnimated)
{
    // 
    if(!m_aDemoRange[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRange); ++i)
    {
        m_aDemoRange[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_aDemoRange[i]);
    }
}


// ****************************************************************
// 
void cNevoMission::SetTileStyle(const coreUintW iIndex, const coreUint8 iStyle)
{
    ASSERT(iIndex < NEVO_TILES)
    coreObject3D& oTile = m_aTileRaw[iIndex];

    switch(iStyle)
    {
    default: ASSERT(false)

    // 
    case 0u:
        oTile.SetColor3 (COLOR_ENERGY_BLUE);
        oTile.SetTexSize(coreVector2(1.0f,1.0f) * 0.0f);
        break;

    // 
    case 1u:
        oTile.SetColor3 (COLOR_ENERGY_YELLOW);
        oTile.SetTexSize(coreVector2(1.0f,1.0f) * 1.0f);
        break;

    // 
    case 2u:
        oTile.SetColor3 (COLOR_ENERGY_GREEN);
        oTile.SetTexSize(coreVector2(1.0f,1.0f) * 1.0f);
        break;
    }
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnBottom()
{
    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Tile.Render();

        // 
        m_Trend.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    //m_ChipWave.Render();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRange); ++i) m_aDemoRange[i].Render();
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRange); ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aDemoRange[i]);
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnOver()
{
    DEPTH_PUSH

    glDepthMask(false);   // TODO 1: glDisable(GL_DEPTH_TEST); in over ???
    {
        // 
        m_ChipWave.Render();
    }
    glDepthMask(true);

    // 
    m_Chip.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Chip);

    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_BlastLine.Render();
        m_Blast    .Render();

#if defined(_P1_UNUSED_)

        // 
        m_BlockWave.Render();

#endif
    }
    glEnable(GL_DEPTH_TEST);

    // 
    if(m_Bomb.GetCurEnabled()) cLodObject::RenderHighList(&m_Bomb);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Bomb);

    // 
    for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i) m_aScrap[i].Render();
    for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i) g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aScrap[i]);

#if defined(_P1_UNUSED_)

    // 
    m_Block.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Block);

    // 
    if(m_bOverdraw) DEPTH_PUSH
               else DEPTH_PUSH_SHIP

    // 
    cLodObject::RenderHighObject(&m_Container);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Container);

#endif
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnTop()
{
    DEPTH_PUSH

    // 
    m_Arrow.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Arrow);

    // 
    m_Guide.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Guide);
}


// ****************************************************************
// 
void cNevoMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    m_iBombGone = 0u;

    // 
    for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
    {
        coreObject3D& oBomb = m_aBombRaw[i];
        if(!oBomb.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat fPrevTime = m_afBombTime[i];
        m_afBombTime[i] += 1.0f * TIME;

        // 
        if(InBetween(2.0f, fPrevTime, m_afBombTime[i]))
        {
            this->EnableBlast(i);
            g_pSpecialEffects->PlaySound(oBomb.GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
        }

        // 
        oBomb.SetSize  (coreVector3(1.0f,1.0f,1.0f) * MIN(oBomb.GetSize().x + 20.0f * TIME, NEVO_BOMB_SIZE));
        oBomb.SetColor3(COLOR_ENERGY_YELLOW * (0.5f + 0.5f * COS(12.0f*PI * m_afBlastTime[i])));
    }

    // 
    m_Bomb.MoveNormal();

    // 
    for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
    {
        coreObject3D* pBlast = (*m_Blast.List())[i];
        if(!pBlast->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreVector2 vBasePos = m_aBombRaw[i].GetPosition ().xy();
        const coreVector2 vBaseDir = m_aBombRaw[i].GetDirection().xy();

        // 
        m_afBlastTime[i].Update(1.0f);
        const coreFloat fTime = m_afBlastTime[i];

        // 
        if(fTime >= 1.0f) this->DisableBomb (i, true);
        if(fTime >= 2.0f) this->DisableBlast(i, false);
        STATIC_ASSERT(NEVO_BOMBS == NEVO_BLASTS)

        // 
        const coreBool  bWarn   = (fTime < 1.0f);
        const coreFloat fLerp   = 1.0f - POW(fTime - 1.0f, 9.0f);
        const coreFloat fBlend  = CLAMP01((fTime - 1.0f) * 20.0f);
        const coreFloat fSize   = bWarn ? (2.0f)                  : (LERP(1.0f, 4.0f, fLerp) * fBlend);
        const coreFloat fAlpha  = bWarn ? (0.6f * (1.0f - fTime)) : (LERP(0.0f, 1.0f, fLerp) * fBlend);
        const coreFloat fTex    = FRACT(2.4f * m_fAnimation * (bWarn ? 0.35f : -1.0f));
        const coreFloat fOffset = I_TO_F(i) * (1.0f/8.0f);

        // 
        pBlast->SetPosition (coreVector3(vBasePos, 0.0f));
        pBlast->SetSize     (coreVector3(2.0f,2.0f,2.0f) * fSize);
        pBlast->SetAlpha    (bWarn ? 0.0f : fAlpha);
        pBlast->SetTexOffset(coreVector2(0.0f, FRACT(-2.5f * m_fAnimation + fOffset)));

        for(coreUintW j = 0u; j < NEVO_LINES; ++j)
        {
            coreObject3D* pLine = (*m_BlastLine.List())[i*NEVO_LINES + j];

            // 
            const coreVector2 vDir     = MapStepRotated90(vBaseDir, j);
            const coreVector2 vPos     = vBasePos + vDir * pLine->GetSize().y;
            const coreFloat   fOffset2 = I_TO_F(j) * (1.0f/32.0f) + fOffset;

            // 
            pLine->SetPosition (coreVector3(vPos, 0.0f));
            pLine->SetDirection(coreVector3(vDir, 0.0f));
            pLine->SetSize     (coreVector3(fSize, pLine->GetSize().y, fSize));
            pLine->SetAlpha    (fAlpha);
            pLine->SetTexOffset(coreVector2(1.0f,1.0f) * (fTex + fOffset2));
        }
    }

    // 
    m_Blast    .MoveNormal();
    m_BlastLine.MoveNormal();

    // 
    for(coreUintW i = 0u; i < NEVO_TILES; ++i)
    {
        coreObject3D& oTile = m_aTileRaw[i];
        if(!oTile.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        coreFloat fScale;
        if(m_afTileTime[i] > 0.0f)
        {
            // 
            m_afTileTime[i].UpdateMin(NEVO_TILE_SPEED, 2.0f);
            fScale = LERPB(0.0f, 1.0f, m_afTileTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afTileTime[i].UpdateMax(-NEVO_TILE_SPEED, -2.0f);
            fScale = LERPB(1.0f, 0.0f, -m_afTileTime[i] - 1.0f);

            // 
            if(m_afTileTime[i] <= -2.0f) this->DisableTile(i, false);
        }

        // 
        const coreFloat fSize     = oTile.GetSize().z;
        const coreFloat fModifier = ((fSize + 0.9f) * RCP(fSize));   // with leeway

        // 
        oTile.SetSize             (coreVector3(fScale, fScale, 1.0f) * fSize);
        oTile.SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * fModifier);
    }

    // 
    m_Tile.MoveNormal();

    // 
    const coreVector2 vArrowOri = coreVector2::Direction(m_fAnimation * (6.0f*PI));

    // 
    m_iArrowActive = m_bArrowEnabled ? BIT(PackDirection(g_pGame->FindPlayerDual(0u)->GetDirection().xy())) : 0u;
    g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
    {
        m_iArrowActive &= BIT(PackDirection(pPlayer->GetDirection().xy()));
    });

    // 
    for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
    {
        coreObject3D& oArrow = m_aArrowRaw[i];
        if(!oArrow.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apArrowOwner[i];
        if(pOwner)
        {
            const coreVector2 vDir = UnpackDirection(m_aiArrowDir[i]);

            oArrow.SetPosition   (pOwner->GetPosition());
            oArrow.SetDirection  (coreVector3(vDir, 0.0f));
            oArrow.SetOrientation(OriRoundDir(vArrowOri, vDir));
        }

        // 
        if(pOwner) m_afArrowAlpha[i] = MIN(m_afArrowAlpha[i] + 5.0f*TIME, 1.0f);
              else m_afArrowAlpha[i] = MAX(m_afArrowAlpha[i] - 5.0f*TIME, 0.0f);

        // 
        if(!m_afArrowAlpha[i]) this->DisableArrow(i, false);

        // 
        const coreFloat fActive = (HAS_BIT(m_iArrowActive, m_aiArrowDir[i]) || HAS_BIT(m_iArrowFake, i)) ? 1.0f : 0.5f;
        const coreFloat fOffset = I_TO_F(i) * (1.0f/8.0f);

        // 
        oArrow.SetAlpha    (m_afArrowAlpha[i] * fActive);
        oArrow.SetTexOffset(coreVector2(FRACT(0.6f * m_fAnimation + fOffset), 0.0f));
    }

    // 
    m_Arrow.MoveNormal();

#if defined(_P1_UNUSED_)

    // 
    for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
    {
        coreObject3D* pBlock = (*m_Block    .List())[i];
        coreObject3D* pWave  = (*m_BlockWave.List())[i];
        if(!pBlock->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apBlockOwner[i];
        if(pOwner)
        {
            
        }

        // 
        coreFloat fFade;
        if(pOwner) fFade = MIN(pBlock->GetSize().z + 1.0f * TIME, 1.0f);
              else fFade = MIN(pBlock->GetSize().z + 3.0f * TIME, 2.0f);

        // 
        if(fFade >= 2.0f) this->DisableBlock(i, false);

        // 
        const coreFloat   fValue = FRACT(2.0f * m_fAnimation);
        const coreFloat   fSize  = m_afBlockScale[i] * (LERPB(0.0f, 1.0f, MIN(fFade, 1.0f)) + LERPB(0.0f, 1.0f, MAX(fFade - 1.0f, 0.0f)));
        const coreVector2 vDir   = coreVector2::Direction(m_afBlockRota[i]);

        // 
        pBlock->SetSize     (coreVector3(fSize, fSize, fFade));
        pBlock->SetDirection(coreVector3(vDir, 0.0f));
        pBlock->SetAlpha    (MIN(2.0f - fFade, 1.0f));
        pBlock->SetTexOffset(coreVector2(0.0f, FRACT(0.8f * m_fAnimation + 0.15f * I_TO_F(i))));

        // 
        pWave->SetPosition (pBlock->GetPosition ());
        pWave->SetSize     (pBlock->GetSize     () * BLENDB(fValue));
        pWave->SetDirection(pBlock->GetDirection());
        pWave->SetAlpha    (pBlock->GetAlpha    () * BLENDB(1.0f - fValue));
        pWave->SetTexOffset(pBlock->GetTexOffset());
    }

    // 
    m_Block    .MoveNormal();
    m_BlockWave.MoveNormal();

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_NEVO_BLOCK, [](cPlayer* OUTPUT pPlayer, const coreObject3D* pBlock, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(10, ELEMENT_ORANGE, vIntersection.xy());

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_ORANGE);
    });

#endif

    // 
    for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i)
    {
        coreObject3D& oScrap = m_aScrap[i];
        if(!oScrap.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afScrapTime[i].Update(1.0f);

        // 
        oScrap.SetTexOffset(coreVector2(0.0f, -0.5f * m_fAnimation));
        oScrap.Move();
    }

    // 
    for(coreUintW i = 0u; i < NEVO_CHIPS; ++i)
    {
        coreObject3D* pChip = (*m_Chip    .List())[i];
        coreObject3D* pWave = (*m_ChipWave.List())[i];
        if(!pChip->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afChipTime[i].Update(0.25f * (i ? 1.0f : 0.5f));

        // 
        if(m_afChipTime[i] >= 1.0f) this->DisableChip(i, false);

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/7.0f);
        const coreVector2 vDir    = coreVector2::Direction(LERPBR(0.0f*PI, 4.0f*PI, m_afChipTime[i]));

        // 
        pChip->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 2.5f * LERPBR(1.0f, 0.0f, STEP(0.7f, 1.0f, m_afChipTime[i])));
        pChip->SetDirection(coreVector3(vDir, 0.0f));
        pChip->SetTexOffset(coreVector2(0.0f, FRACT(0.8f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pChip->GetPosition ());
        pWave->SetSize     (pChip->GetSize     () * 1.3f);
        pWave->SetDirection(pChip->GetDirection().InvertedX());
        pWave->SetTexOffset(pChip->GetTexOffset());
    }

    // 
    m_Chip    .MoveNormal();
    m_ChipWave.MoveNormal();

    // 
    if(m_Guide.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Guide.SetTexOffset(coreVector2(-0.5f * m_fAnimation, 0.0f));
        m_Guide.Move();
    }

    // 
    if(m_Trend.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Trend.SetTexOffset(coreVector2(1.0f,1.0f) * -0.5f * m_fAnimation);
        m_Trend.Move();
    }

#if defined(_P1_UNUSED_)

    if(m_Container.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        coreVector2 vNewPos = m_Container.GetPosition().xy();

        // 
        if(!m_vForce.IsNull())
        {
            vNewPos  += m_vForce * TIME;
            m_vForce *= FrictionFactor(2.5f);
        }

        // 
        m_vImpact = coreVector2(0.0f,0.0f);

        if(m_bClamp)
        {
            // TODO 1: better bounce correction (?)
            // 
                 if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; if(m_vForce.x < 0.0f) m_vImpact = coreVector2(-FOREGROUND_AREA.x * 1.1f, m_Container.GetPosition().y); m_vForce.x =  ABS(m_vForce.x);}
            else if(vNewPos.x >  FOREGROUND_AREA.x) {vNewPos.x =  FOREGROUND_AREA.x; if(m_vForce.x > 0.0f) m_vImpact = coreVector2( FOREGROUND_AREA.x * 1.1f, m_Container.GetPosition().y); m_vForce.x = -ABS(m_vForce.x);}
                 if(vNewPos.y < -FOREGROUND_AREA.y) {vNewPos.y = -FOREGROUND_AREA.y; if(m_vForce.y < 0.0f) m_vImpact = coreVector2(m_Container.GetPosition().x, -FOREGROUND_AREA.y * 1.1f); m_vForce.y =  ABS(m_vForce.y);}
            else if(vNewPos.y >  FOREGROUND_AREA.y) {vNewPos.y =  FOREGROUND_AREA.y; if(m_vForce.y > 0.0f) m_vImpact = coreVector2(m_Container.GetPosition().x,  FOREGROUND_AREA.y * 1.1f); m_vForce.y = -ABS(m_vForce.y);}
        }

        // 
        m_Container.SetPosition(coreVector3(vNewPos, m_Container.GetPosition().z));
        m_Container.Move();

        // 
        cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, &m_Container, [](cPlayer* OUTPUT pPlayer, const coreObject3D* pContainer, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;

            // 
            const coreVector2 vDiff = pPlayer->GetOldPos() - pContainer->GetPosition().xy();
            pPlayer->ApplyForce(vDiff.Normalized() * 100.0f);

            // 
            g_pSpecialEffects->CreateSplashColor(vIntersection, 5.0f, 3u, COLOR_ENERGY_WHITE * 0.8f);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
        });

        // 
        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &m_Container, [](cBullet* OUTPUT pBullet, const coreObject3D* pContainer, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;

            // 
            pBullet->Reflect(pContainer, vIntersection.xy());
        });
    }

#endif

    // 
    if(m_aDemoRange[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRange); ++i)
        {
            m_fDemoRangeAnim.Update(0.4f);

            const coreVector2 vDir = coreVector2::Direction(m_fDemoRangeAnim * (-1.6f*PI));

            m_aDemoRange[i].SetSize     (coreVector3(1.0f,1.0f,1.0f) * (i ? 0.03f : 0.032f) * 50.0f);              
            m_aDemoRange[i].SetDirection(coreVector3(vDir, 0.0f));
            m_aDemoRange[i].SetTexOffset(m_aDemoRange[i].GetTexOffset() - m_aDemoRange[i].GetDirection().xy() * (0.04f * TIME));
            m_aDemoRange[i].Move();
        }
    }

    // 
    if(g_pGame->GetBulletManagerEnemy()->GetNumBulletsTypedEst<cGrowBullet>())
    {
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cGrowBullet>([](cGrowBullet* OUTPUT pBullet)
        {
            const coreFloat fScale   = (pBullet->GetDamage() == 1) ? 5.0f : 1.0f;
            const coreFloat fNewSize = MIN(pBullet->GetSize().x + (7.0f * TIME * fScale), 15.0f * fScale);

            pBullet->SetSize(coreVector3(1.0f,1.0f,1.0f) * fNewSize);
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cGrowBullet>([](cGrowBullet* OUTPUT pBullet1)
        {
            if(pBullet1->GetDamage() == 2) return;
            if(coreMath::IsNear(pBullet1->GetSpeed(), 0.1f)) return;

            if(((pBullet1->GetPosition().x <  -FOREGROUND_AREA.x * 1.0f) && (pBullet1->GetFlyDir().x < 0.0f)) ||
               ((pBullet1->GetPosition().x >=  FOREGROUND_AREA.y * 1.0f) && (pBullet1->GetFlyDir().x > 0.0f)) ||
               ((pBullet1->GetPosition().y <  -FOREGROUND_AREA.y * 1.0f) && (pBullet1->GetFlyDir().y < 0.0f)) ||
               ((pBullet1->GetPosition().y >=  FOREGROUND_AREA.y * 1.0f) && (pBullet1->GetFlyDir().y > 0.0f)))
            {
                pBullet1->SetSpeed(0.1f);
                return;
            }

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cGrowBullet>([&](const cGrowBullet* pBullet2)
            {
                if(pBullet1 == pBullet2) return;

                const coreVector2 vDiff        = pBullet2->GetPosition().xy()   - pBullet1->GetPosition().xy();
                const coreFloat   fTotalRadius = pBullet2->GetCollisionRadius() + pBullet1->GetCollisionRadius();

                if(vDiff.LengthSq() > POW2(fTotalRadius)) return;

                 if(((pBullet2->GetPosition().x <  -FOREGROUND_AREA.x * 1.0f) && (pBullet1->GetFlyDir().x < 0.0f)) ||   // 2 1
                    ((pBullet2->GetPosition().x >=  FOREGROUND_AREA.y * 1.0f) && (pBullet1->GetFlyDir().x > 0.0f)) ||
                    ((pBullet2->GetPosition().y <  -FOREGROUND_AREA.y * 1.0f) && (pBullet1->GetFlyDir().y < 0.0f)) ||
                    ((pBullet2->GetPosition().y >=  FOREGROUND_AREA.y * 1.0f) && (pBullet1->GetFlyDir().y > 0.0f)))
                {
                    pBullet1->SetSpeed(0.1f);
                    return;
                }
            });
        });

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(pBulletEnemy->GetID() != cGrowBullet::ID) return;
            if(!bFirstHit || !g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            const coreBool  bBadge = (pBulletEnemy->GetDamage() > 5);
            const coreFloat fPower = I_TO_F(pBulletPlayer->GetDamage()) * RCP(I_TO_F(g_pGame->GetNumPlayers())) * (bBadge ? 0.5f : 1.0f);

            pBulletEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * (pBulletEnemy->GetSize().x * POW(0.9f, fPower)));

            if(bBadge)
            {
                pBulletEnemy->SetDamage(pBulletEnemy->GetDamage() + pBulletPlayer->GetDamage());
                g_pGame->PlayHitSound(vIntersection);

                d_cast<cPlayer*>(pBulletPlayer->GetOwner())->GetScoreTable()->RefreshCooldown();
            }

            pBulletPlayer->Deactivate(true);

            g_pSpecialEffects->PlaySound(vIntersection, 1.0f, 1.0f, SOUND_PLACEHOLDER);
        });
    }
}