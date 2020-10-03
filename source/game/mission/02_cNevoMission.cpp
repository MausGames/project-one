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
, m_Tile        (NEVO_TILES)
, m_afTileTime  {}
, m_vForce      (coreVector2(0.0f,0.0f))
, m_vImpact     (coreVector2(0.0f,0.0f))
, m_bClamp      (false)
, m_bOverdraw   (false)
{
    // 
    m_apBoss[0] = &m_Nautilus;
    m_apBoss[1] = &m_Amemasu;
    m_apBoss[2] = &m_Leviathan;

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
    m_Container.DefineModelHigh("object_container_high.md3");
    m_Container.DefineModelLow ("object_container_low.md3");
    m_Container.DefineTexture  (0u, "ship_enemy.png");
    m_Container.DefineProgram  ("object_ship_program");
    m_Container.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
    m_Container.SetColor3      (COLOR_SHIP_GREY);
    m_Container.SetEnabled     (CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// destructor
cNevoMission::~cNevoMission()
{
    // 
    for(coreUintW i = 0u; i < NEVO_TILES;  ++i) this->DisableTile (i, false);
    this->DisableContainer(false);
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
void cNevoMission::EnableContainer(const coreVector2& vPosition)
{
    // 
    if(m_Container.GetType()) return;
    m_Container.ChangeType(TYPE_NEVO_CONTAINER);

    // 
    m_Container.SetPosition(coreVector3(vPosition, 0.0f));

    // 
    m_Container.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    cShadow::GetGlobalContainer()->BindObject(&m_Container);

    // 
    m_bClamp    = false;
    m_bOverdraw = false;
}


// ****************************************************************
// 
void cNevoMission::DisableContainer(const coreBool bAnimated)
{
    // 
    if(!m_Container.GetType()) return;
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
    // 
    m_Tile.Render();
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnOver()
{
    // 
    if(m_bOverdraw) DEPTH_PUSH
               else DEPTH_PUSH_SHIP

    // 
    m_Container.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Container);
}


// ****************************************************************
// 
void cNevoMission::__MoveOwnAfter()
{
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

    if(m_Container.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        coreVector2 vNewPos = m_Container.GetPosition().xy();

        // 
        if(!m_vForce.IsNull())
        {
            vNewPos  += m_vForce * Core::System->GetTime();
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
        m_Container.ActivateModelLowOnly();
        {
            // 
            cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, &m_Container, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pContainer, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                if(!bFirstHit) return;

                // 
                pPlayer->TakeDamage(15, ELEMENT_NEUTRAL, vIntersection.xy());

                // 
                g_pSpecialEffects->MacroExplosionPhysicalDarkSmall(vIntersection);
            });

            // 
            Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &m_Container, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pContainer, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                if(!bFirstHit) return;

                // 
                pBullet->Reflect(pContainer, vIntersection.xy());
            });
        }
        m_Container.ActivateModelDefault();
    }
}