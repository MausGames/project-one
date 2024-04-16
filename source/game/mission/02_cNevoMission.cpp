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
cNevoMission::cNevoMission()noexcept
: m_Bomb         (NEVO_BOMBS)
, m_abBombGone   {}
, m_Blast        (NEVO_BLASTS)
, m_BlastLine    (NEVO_BLASTS * NEVO_LINES)
, m_afBlastTime  {}
, m_Tile         (NEVO_TILES)
, m_afTileTime   {}
, m_Arrow        (NEVO_ARROWS)
, m_apArrowOwner {}
, m_aiArrowDir   {}
, m_Block        (NEVO_BLOCKS)
, m_BlockWave    (NEVO_BLOCKS)
, m_apBlockOwner {}
, m_afBlockScale {}
, m_vForce       (coreVector2(0.0f,0.0f))
, m_vImpact      (coreVector2(0.0f,0.0f))
, m_bClamp       (false)
, m_bOverdraw    (false)
, m_fAnimation   (0.0f)
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
            pTile->SetAlpha  (0.7f);
            pTile->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

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
            pArrow->DefineModel  ("bullet_cone.md3");
            pArrow->DefineTexture(0u, "effect_energy.png");
            pArrow->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pArrow->SetSize   (coreVector3(1.35f,1.55f,1.35f) * 1.3f);
            pArrow->SetColor3 (COLOR_ENERGY_GREEN * 0.6f);
            pArrow->SetTexSize(coreVector2(0.5f,0.2f) * 1.3f);
            pArrow->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Arrow.BindObject(pArrow);
        }
    }

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
            pBlock->DefineTexture(0u, "effect_energy.png");
            pBlock->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pBlock->SetColor3 (COLOR_ENERGY_ORANGE);
            pBlock->SetTexSize(coreVector2(3.0f,1.2f) * 0.55f);
            pBlock->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BlockWave.BindObject(pBlock);
                 else m_Block    .BindObject(pBlock);
        }
    }

    // 
    m_Container.DefineModelHigh("object_container_high.md3");
    m_Container.DefineModelLow ("object_container_low.md3");
    m_Container.DefineTexture  (0u, "ship_enemy.png");
    m_Container.DefineProgram  ("object_ship_program");
    m_Container.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
    m_Container.SetColor3      (COLOR_SHIP_GREY);
    m_Container.SetEnabled     (CORE_OBJECT_ENABLE_NOTHING);

    // 
    cShadow::GetGlobalContainer()->BindList(&m_Bomb);
    g_pGlow->BindList(&m_Blast);
    g_pGlow->BindList(&m_BlastLine);
    g_pGlow->BindList(&m_Arrow);
    g_pGlow->BindList(&m_Block);
    g_pGlow->BindList(&m_BlockWave);
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

    // 
    for(coreUintW i = 0u; i < NEVO_BOMBS;  ++i) this->DisableBomb (i, false);
    for(coreUintW i = 0u; i < NEVO_BLASTS; ++i) this->DisableBlast(i, false);
    for(coreUintW i = 0u; i < NEVO_TILES;  ++i) this->DisableTile (i, false);
    for(coreUintW i = 0u; i < NEVO_ARROWS; ++i) this->DisableArrow(i, false);
    for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i) this->DisableBlock(i, false);
    this->DisableContainer(false);
}


// ****************************************************************
// 
void cNevoMission::EnableBomb(const coreUintW iIndex, const coreBool bGrow)
{
    ASSERT(iIndex < NEVO_BOMBS)
    coreObject3D& oBomb = m_aBombRaw[iIndex];

    // 
    WARN_IF(oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    oBomb.ChangeType(TYPE_NEVO_BOMB);

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
    oBomb.ChangeType(0);

    // 
    m_abBombGone[iIndex] = true;

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
    WARN_IF(pBlast->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

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
    }
}


// ****************************************************************
// 
void cNevoMission::EnableTile(const coreUintW iIndex, const coreUintW iDimension)
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
void cNevoMission::DisableTile(const coreUintW iIndex, const coreBool bAnimated)
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
void cNevoMission::EnableArrow(const coreUintW iIndex, const cShip* pOwner, const coreVector2 vDirection)
{
    ASSERT(iIndex < NEVO_ARROWS)
    coreObject3D& oArrow = m_aArrowRaw[iIndex];

    // 
    WARN_IF(oArrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(pOwner)
    m_apArrowOwner[iIndex] = pOwner;
    m_aiArrowDir  [iIndex] = PackDirection(vDirection);

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
    ASSERT(iIndex < NEVO_BLOCKS)
    coreObject3D* pBlock = (*m_Block    .List())[iIndex];
    coreObject3D* pWave  = (*m_BlockWave.List())[iIndex];

    // 
    WARN_IF(pBlock->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    pBlock->ChangeType(TYPE_NEVO_BLOCK);

    // 
    ASSERT(pOwner)
    m_apBlockOwner[iIndex] = pOwner;
    m_afBlockScale[iIndex] = fScale;

    // 
    pBlock->SetSize   (coreVector3(0.0f,0.0f,0.0f));
    pBlock->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cNevoMission::DisableBlock(const coreUintW iIndex, const coreBool bAnimated)
{
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
}


// ****************************************************************
// 
void cNevoMission::EnableContainer(const coreVector2 vPosition)
{
    // 
    WARN_IF(m_Container.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    m_Container.ChangeType(TYPE_NEVO_CONTAINER);

    // 
    m_bClamp    = false;
    m_bOverdraw = false;

    // 
    m_Container.SetPosition(coreVector3(vPosition, 0.0f));
    m_Container.SetEnabled (CORE_OBJECT_ENABLE_ALL);
    cShadow::GetGlobalContainer()->BindObject(&m_Container);
}


// ****************************************************************
// 
void cNevoMission::DisableContainer(const coreBool bAnimated)
{
    // 
    if(!m_Container.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    m_Container.ChangeType(0);

    // 
    m_Container.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    cShadow::GetGlobalContainer()->UnbindObject(&m_Container);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalDarkBig(m_Container.GetPosition());
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Tile.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_BlockWave.Render();
    }
    glDepthMask(true);

    // 
    m_Block.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Block);
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnOver()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_BlastLine.Render();
        m_Blast    .Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    cLodObject::RenderHighList(&m_Bomb);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Bomb);

    // 
    m_Arrow.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyList(&m_Arrow);

    // 
    if(m_bOverdraw) DEPTH_PUSH
               else DEPTH_PUSH_SHIP

    // 
    cLodObject::RenderHighObject(&m_Container);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Container);
}


// ****************************************************************
// 
void cNevoMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    std::memset(m_abBombGone, 0, sizeof(m_abBombGone));

    // 
    for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
    {
        coreObject3D& oBomb = m_aBombRaw[i];
        if(!oBomb.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

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

        const coreVector2 vBaseDir = m_aBombRaw[i].GetDirection().xy();
        const coreVector2 vBasePos = m_aBombRaw[i].GetPosition ().xy();

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
        const coreFloat fBlend  = CLAMP((fTime - 1.0f) * 20.0f, 0.0f, 1.0f);
        const coreFloat fSize   = bWarn ? (2.0f)                  : (LERP(1.0f, 4.0f, fLerp) * fBlend);
        const coreFloat fAlpha  = bWarn ? (0.6f * (1.0f - fTime)) : (LERP(0.0f, 1.0f, fLerp) * fBlend);
        const coreFloat fOffset = FRACT(2.4f * m_fAnimation * (bWarn ? 0.35f : -1.0f));

        // 
        pBlast->SetPosition (coreVector3(vBasePos, 0.0f));
        pBlast->SetSize     (coreVector3(2.0f,2.0f,2.0f) * fSize);
        pBlast->SetAlpha    (bWarn ? 0.0f : fAlpha);
        pBlast->SetTexOffset(coreVector2(0.0f, FRACT(-2.5f * m_fAnimation)));

        for(coreUintW j = 0u; j < NEVO_LINES; ++j)
        {
            coreObject3D* pLine = (*m_BlastLine.List())[i*NEVO_LINES + j];

            // 
            const coreVector2 vDir = MapStepRotated90(vBaseDir, j);
            const coreVector2 vPos = vBasePos + vDir * pLine->GetSize().y;

            // 
            pLine->SetPosition (coreVector3(vPos, 0.0f));
            pLine->SetDirection(coreVector3(vDir, 0.0f));
            pLine->SetSize     (coreVector3(fSize, pLine->GetSize().y, fSize));
            pLine->SetAlpha    (fAlpha);
            pLine->SetTexOffset(coreVector2(1.0f,1.0f) * fOffset);
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
    for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
    {
        coreObject3D& oArrow = m_aArrowRaw[i];
        if(!oArrow.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apArrowOwner[i];
        if(pOwner)
        {
            const coreVector2 vDir = UnpackDirection(m_aiArrowDir[i]);

            oArrow.SetPosition (coreVector3(pOwner->GetPosition().xy() + vDir * 3.5f, 0.0f));
            oArrow.SetDirection(coreVector3(-vDir, 0.0f));
        }

        // 
        if(pOwner) oArrow.SetAlpha(MIN(oArrow.GetAlpha() + 5.0f*TIME, 1.0f));
              else oArrow.SetAlpha(MAX(oArrow.GetAlpha() - 5.0f*TIME, 0.0f));

        // 
        if(!oArrow.GetAlpha()) this->DisableArrow(i, false);

        // 
        oArrow.SetTexOffset(coreVector2(m_fAnimation * 0.6f, 0.0f));
    }

    // 
    m_Arrow.MoveNormal();

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
            pBlock->SetPosition(coreVector3(pOwner->GetPosition().xy(), 0.0f));
        }

        // 
        coreFloat fFade;
        if(pOwner) fFade = MIN(pBlock->GetSize().z + 1.0f * TIME, 1.0f);
              else fFade = MIN(pBlock->GetSize().z + 3.0f * TIME, 2.0f);

        // 
        if(fFade >= 2.0f) this->DisableBlock(i, false);

        // 
        const coreFloat   fValue = FRACT(5.0f * m_fAnimation);
        const coreFloat   fSize  = ABS(m_afBlockScale[i]) * (LERPB(0.0f, 1.0f, MIN(fFade, 1.0f)) + LERPB(0.0f, 1.0f, MAX(fFade - 1.0f, 0.0f)));
        const coreVector2 vDir   = coreVector2::Direction(m_fAnimation * ((m_afBlockScale[i] < 0.0f) ? (-2.0f*PI) : (1.0f*PI)));

        // 
        pBlock->SetSize     (coreVector3(fSize, fSize, fFade));
        pBlock->SetDirection(coreVector3(vDir, 0.0f));
        pBlock->SetAlpha    (MIN(2.0f - fFade, 1.0f));
        pBlock->SetTexOffset(coreVector2(0.0f, FRACT(0.8f * m_fAnimation + 0.15f * I_TO_F(i))));

        // 
        pWave->SetPosition (pBlock->GetPosition ());
        pWave->SetSize     (pBlock->GetSize     () * LERPB(0.0f, 1.0f, fValue));
        pWave->SetDirection(pBlock->GetDirection());
        pWave->SetAlpha    (pBlock->GetAlpha    () * LERPB(0.0f, 1.0f, 1.0f - fValue));
        pWave->SetTexOffset(pBlock->GetTexOffset());
    }

    // 
    m_Block    .MoveNormal();
    m_BlockWave.MoveNormal();

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_NEVO_BLOCK, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBlock, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(10, ELEMENT_ORANGE, vIntersection.xy());

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_ORANGE);
    });

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
        cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, &m_Container, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pContainer, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;

            // 
            const coreVector2 vDiff = pPlayer->GetOldPos() - pContainer->GetPosition().xy();
            pPlayer->ApplyForce(vDiff.Normalized() * 100.0f);

            // 
            g_pSpecialEffects->CreateSplashColor(vIntersection, 5.0f, 3u, COLOR_ENERGY_WHITE);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
        });

        // 
        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &m_Container, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pContainer, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;

            // 
            pBullet->Reflect(pContainer, vIntersection.xy(), 3.0f);
        });
    }
}