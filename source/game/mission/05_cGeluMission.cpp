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
cGeluMission::cGeluMission()noexcept
: m_Fang           (GELU_FANGS)
, m_iFangActive    (0u)
, m_Way            (GELU_WAYS)
, m_WayArrow       (GELU_WAYS)
, m_iWayActive     (0u)
, m_iWayVisible    (0u)
, m_iWayGhost      (0u)
, m_iWayFree       (0u)
, m_bWaySpooky     (false)
, m_Orb            (GELU_ORBS)
, m_afOrbTime      {}
, m_Line           (GELU_LINES)
, m_afLineTime     {}
, m_iLineMode      (0u)
, m_Coin           (GELU_COINS)
, m_CoinWave       (GELU_COINS)
, m_Gap            (GELU_GAPS)
, m_afGapTime      {}
, m_iGapActive     (0u)
, m_Shine          (GELU_SHINES)
, m_afShineTime    {}
, m_iShineActive   (0u)
, m_avOldPos       {}
, m_iTouchState    (0u)
, m_iTouchStateOld (0u)
, m_avFreezeMove   {}
, m_abCrushImmune  {}
, m_abCrushInside  {}
, m_iCrushState    (0u)
, m_fAnimation     (0.0f)
, m_bStory         (!HAS_BIT_EX(REPLAY_WRAP_PROGRESS_STATE, STATE_STORY_GELU) && (g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK))
{
    // 
    m_apBoss[0] = &m_Chol;

    // 
    m_Fang.DefineProgram("object_ship_detail_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_FANGS_RAWS; ++i)
        {
            // load object resources
            cLodObject* pFang = &m_aFangRaw[i];
            pFang->DefineModelHigh("object_fang.md3");
            pFang->DefineModelLow ("object_fang.md3");
            pFang->DefineVolume   ("object_fang_volume.md3");
            pFang->DefineTexture  (0u, "ship_enemy.png");
            pFang->DefineTexture  (1u, "menu_background_black.png");
            pFang->DefineProgram  ("object_ship_detail_program");

            // set object properties
            pFang->SetSize             (coreVector3(1.0f,1.0f,1.0f) * 9.0f);
            pFang->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.05f);
            pFang->SetTexOffset        (coreVector2(1.0f,1.0f) * (I_TO_F(i) * (1.0f/7.0f)));
            pFang->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Fang.BindObject(pFang);
        }
    }

    // 
    m_Way     .DefineProgram("effect_energy_flat_spheric_inst_program");
    m_WayArrow.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_WAYS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pWay = &m_aWayRaw[i];
            pWay->DefineModel  (iType ? "object_arrow_short.md3" : "object_cube_top.md3");
            pWay->DefineVolume ("object_cube_volume.md3");
            pWay->DefineTexture(0u, "effect_energy.png");
            pWay->DefineProgram(iType ? "effect_energy_flat_invert_program" : "effect_energy_flat_spheric_program");

            // set object properties
            pWay->SetSize             (coreVector3(1.0f,1.0f,1.0f) * (iType ? 2.0f : 7.0f));
            pWay->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.15f);
            pWay->SetColor3           (COLOR_ENERGY_MAGENTA * 0.8f);
            pWay->SetTexSize          (coreVector2(1.0f,1.0f) * (iType ? 0.4f : 0.7f));
            pWay->SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_WayArrow.BindObject(pWay);
                 else m_Way     .BindObject(pWay);
        }
    }

    // 
    m_Orb.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_ORBS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pOrb = &m_aOrbRaw[i];
            pOrb->DefineModel  ("object_sphere.md3");
            pOrb->DefineTexture(0u, "effect_energy.png");
            pOrb->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pOrb->SetColor3 (COLOR_ENERGY_CYAN * 0.7f);
            pOrb->SetTexSize(coreVector2(4.0f,4.0f) * 0.9f);
            pOrb->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Orb.BindObject(pOrb);
        }
    }

    // 
    m_Line.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_LINES_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pLine = &m_aLineRaw[i];
            pLine->DefineModel  ("object_tube.md3");
            pLine->DefineTexture(0u, "effect_energy.png");
            pLine->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pLine->SetColor3 (COLOR_ENERGY_CYAN * 0.5f);
            pLine->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Line.BindObject(pLine);
        }
    }

    // 
    m_Coin    .DefineProgram("effect_energy_flat_invert_inst_program");
    m_CoinWave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_COINS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pCoin = &m_aCoinRaw[i];
            pCoin->DefineModel  ("bullet_spear.md3");
            pCoin->DefineTexture(0u, "effect_energy.png");
            pCoin->DefineProgram(iType ? "effect_energy_flat_spheric_program" : "effect_energy_flat_invert_program");

            // set object properties
            pCoin->SetSize   (coreVector3(1.3f,1.0f,1.3f) * (iType ? 4.2f : 3.6f));
            pCoin->SetColor3 (COLOR_ENERGY_GREEN * 0.8f);
            pCoin->SetTexSize(coreVector2(0.4f,0.2f));
            pCoin->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_CoinWave.BindObject(pCoin);
                 else m_Coin    .BindObject(pCoin);
        }
    }

    // 
    m_Gap.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_GAPS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pGap = &m_aGapRaw[i];
            pGap->DefineModel  ("object_cube_top.md3");
            pGap->DefineTexture(0u, "effect_energy.png");
            pGap->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pGap->SetColor3 (COLOR_ENERGY_BLUE * 0.9f);
            pGap->SetTexSize(coreVector2(1.0f,1.0f) * 0.7f);
            pGap->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Gap.BindObject(pGap);
        }
    }

    // 
    m_Shine.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_SHINES_RAWS; ++i)
        {

            // load object resources
            coreObject3D* pShine = &m_aShineRaw[i];
            pShine->DefineModel  ("object_cube_top.md3");
            pShine->DefineTexture(0u, "effect_energy.png");
            pShine->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pShine->SetColor3 (COLOR_ENERGY_CYAN * 0.6f);
            pShine->SetTexSize(coreVector2(1.0f,1.0f) * 0.3f);
            pShine->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Shine.BindObject(pShine);
        }
    }

    // 
    m_Surfer.DefineModelHigh("object_cube_top.md3");
    m_Surfer.DefineModelLow ("object_cube_top.md3");
    m_Surfer.DefineVolume   ("object_cube_volume.md3");
    m_Surfer.DefineTexture  (0u, "effect_energy.png");
    m_Surfer.DefineProgram  ("effect_energy_flat_invert_program");
    m_Surfer.SetSize        (coreVector3(1.0f,1.0f,1.0f) * 2.5f);
    m_Surfer.SetTexSize     (coreVector2(1.0f,1.0f) * 0.4f);
    m_Surfer.Configure      (1000, COLOR_ENERGY_YELLOW * 0.8f);
    m_Surfer.AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_TOP | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_FLAT);

    // 
    m_SurferWave.DefineModel  ("object_cube_top.md3");
    m_SurferWave.DefineTexture(0u, "effect_energy.png");
    m_SurferWave.DefineProgram("effect_energy_flat_program");
    m_SurferWave.SetColor3    (COLOR_ENERGY_YELLOW * 0.4f);
    m_SurferWave.SetTexSize   (coreVector2(1.0f,1.0f) * 0.4f);
    m_SurferWave.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    g_pGlow->BindList(&m_Way);
    g_pGlow->BindList(&m_WayArrow);
    g_pGlow->BindList(&m_Orb);
    g_pGlow->BindList(&m_Line);
    g_pGlow->BindList(&m_Coin);
    g_pGlow->BindList(&m_CoinWave);
    g_pGlow->BindList(&m_Gap);
    g_pGlow->BindList(&m_Shine);
}


// ****************************************************************
// destructor
cGeluMission::~cGeluMission()
{
    // 
    g_pGlow->UnbindList(&m_Way);
    g_pGlow->UnbindList(&m_WayArrow);
    g_pGlow->UnbindList(&m_Orb);
    g_pGlow->UnbindList(&m_Line);
    g_pGlow->UnbindList(&m_Coin);
    g_pGlow->UnbindList(&m_CoinWave);
    g_pGlow->UnbindList(&m_Gap);
    g_pGlow->UnbindList(&m_Shine);

    // 
    for(coreUintW i = 0u; i < GELU_FANGS;  ++i) this->DisableFang (i, false);
    for(coreUintW i = 0u; i < GELU_WAYS;   ++i) this->DisableWay  (i, false);
    for(coreUintW i = 0u; i < GELU_ORBS;   ++i) this->DisableOrb  (i, false);
    for(coreUintW i = 0u; i < GELU_LINES;  ++i) this->DisableLine (i, false);
    for(coreUintW i = 0u; i < GELU_COINS;  ++i) this->DisableCoin (i, false);
    for(coreUintW i = 0u; i < GELU_GAPS;   ++i) this->DisableGap  (i, false);
    for(coreUintW i = 0u; i < GELU_SHINES; ++i) this->DisableShine(i, false);
    this->DisableSurfer(false);
}


// ****************************************************************
// 
void cGeluMission::EnableFang(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_FANGS)
    cLodObject& oFang = m_aFangRaw[iIndex];

    // 
    WARN_IF(oFang.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableFang(iIndex, false);

    // 
    ADD_BIT(m_iFangActive, iIndex)
    STATIC_ASSERT(GELU_FANGS <= sizeof(m_iFangActive)*8u)

    // 
    m_avOldPos[iIndex] = coreVector2(0.0f,0.0f);
    STATIC_ASSERT(GELU_FANGS <= GELU_POSITIONS)

    // 
    oFang.SetPosition(coreVector3(HIDDEN_POS, 0.0f));
    oFang.SetColor3  (GELU_FANG_COLOR);
    oFang.SetEnabled (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableFang(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_FANGS)
    cLodObject& oFang = m_aFangRaw[iIndex];

    // 
    if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_iFangActive, iIndex)

    // 
    oFang.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->MacroExplosionDarkSmall(oFang.GetPosition());
        g_pSpecialEffects->CreateBreakupDark(&oFang, 80.0f, 14u);
    }
}


// ****************************************************************
// 
void cGeluMission::EnableWay(const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection)
{
    ASSERT(iIndex < GELU_WAYS)
    coreObject3D* pWay   = (*m_Way     .List())[iIndex];
    coreObject3D* pArrow = (*m_WayArrow.List())[iIndex];

    // 
    WARN_IF(pWay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableWay(iIndex, false);

    // 
    ADD_BIT   (m_iWayActive,  iIndex)
    ADD_BIT   (m_iWayVisible, iIndex)
    REMOVE_BIT(m_iWayGhost,   iIndex)
    REMOVE_BIT(m_iWayFree,    iIndex)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayActive) *8u)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayVisible)*8u)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayGhost)  *8u)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayFree)   *8u)

    // 
    m_avOldPos[iIndex] = coreVector2(0.0f,0.0f);
    STATIC_ASSERT(GELU_WAYS <= GELU_POSITIONS)

    // 
    pWay  ->SetPosition (coreVector3(vPosition,  0.0f));
    pWay  ->SetDirection(coreVector3(vDirection, 0.0f));
    pWay  ->SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    pArrow->SetEnabled  (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableWay(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_WAYS)
    coreObject3D* pWay   = (*m_Way     .List())[iIndex];
    coreObject3D* pArrow = (*m_WayArrow.List())[iIndex];

    // 
    if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_iWayActive, iIndex)
    if(!m_iWayActive)
    {
        g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
        });
    }

    // 
    if(!bAnimated)
    {
        pWay  ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pArrow->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cGeluMission::EnableOrb(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_ORBS)
    coreObject3D& oOrb = m_aOrbRaw[iIndex];

    // 
    WARN_IF(oOrb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableOrb(iIndex, false);

    // 
    m_afOrbTime[iIndex] = 1.0f;

    // 
    oOrb.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableOrb(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_ORBS)
    coreObject3D& oOrb = m_aOrbRaw[iIndex];

    // 
    if(!oOrb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(iIndex < GELU_SHINES) this->DisableShine(iIndex, bAnimated);

    if(m_afOrbTime[iIndex] > 0.0f)
    {
        // 
        m_afOrbTime[iIndex] = -1.0f;

        // 
        if(bAnimated)
        {
            g_pSpecialEffects->CreateSplashColor(oOrb.GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_CYAN);
            g_pSpecialEffects->PlaySound(oOrb.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
        }
    }

    // 
    if(!bAnimated) oOrb.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cGeluMission::EnableLine(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_LINES)
    coreObject3D& oLine = m_aLineRaw[iIndex];

    // 
    WARN_IF(oLine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableLine(iIndex, false);

    // 
    m_afLineTime[iIndex] = 1.0f;

    // 
    oLine.SetPosition(coreVector3(HIDDEN_POS, 0.0f));
    oLine.SetEnabled (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableLine(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_LINES)
    coreObject3D& oLine = m_aLineRaw[iIndex];

    // 
    if(!oLine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    if(m_afLineTime[iIndex] > 0.0f)
    {
        // 
        m_afLineTime[iIndex] = -1.0f;

        // 
        if(bAnimated) for(coreUintW j = 10u; j--; ) g_pSpecialEffects->CreateSplashColor(oLine.GetPosition() + oLine.GetDirection() * (2.0f * (I_TO_F(j) - 4.5f)), 10.0f, 1u, COLOR_ENERGY_CYAN);
    }

    // 
    if(!bAnimated) oLine.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cGeluMission::EnableCoin(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_COINS)
    coreObject3D* pCoin = (*m_Coin    .List())[iIndex];
    coreObject3D* pWave = (*m_CoinWave.List())[iIndex];

    // 
    WARN_IF(pCoin->IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableCoin(iIndex, false);

    // 
    pCoin->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableCoin(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_COINS)
    coreObject3D* pCoin = (*m_Coin    .List())[iIndex];
    coreObject3D* pWave = (*m_CoinWave.List())[iIndex];

    // 
    if(!pCoin->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    pCoin->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    pWave->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(pCoin->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
}


// ****************************************************************
// 
void cGeluMission::EnableGap(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_GAPS)
    coreObject3D& oGap = m_aGapRaw[iIndex];

    // 
    WARN_IF(oGap.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableGap(iIndex, false);

    // 
    ADD_BIT(m_iGapActive, iIndex)
    STATIC_ASSERT(GELU_GAPS <= sizeof(m_iGapActive)*8u)

    // 
    m_afGapTime[iIndex] = 0.0f;

    // 
    oGap.SetAlpha  (0.0f);
    oGap.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableGap(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_GAPS)
    coreObject3D& oGap = m_aGapRaw[iIndex];

    // 
    if(!oGap.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_iGapActive, iIndex)

    // 
    if(!bAnimated) oGap.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cGeluMission::EnableShine(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_SHINES)
    coreObject3D& oShine = m_aShineRaw[iIndex];

    // 
    WARN_IF(oShine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableShine(iIndex, false);

    // 
    ADD_BIT(m_iShineActive, iIndex)
    STATIC_ASSERT(GELU_SHINES <= sizeof(m_iShineActive)*8u)

    // 
    m_afShineTime[iIndex] = 0.0f;

    // 
    oShine.SetAlpha  (0.0f);
    oShine.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableShine(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_SHINES)
    coreObject3D& oShine = m_aShineRaw[iIndex];

    // 
    if(!oShine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_iShineActive, iIndex)

    // 
    if(!bAnimated) oShine.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cGeluMission::EnableSurfer()
{
    WARN_IF(m_SurferWave.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableSurfer(false);

    // 
    m_Surfer.Resurrect();

    // 
    m_SurferWave.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_SurferWave);
}


// ****************************************************************
// 
void cGeluMission::DisableSurfer(const coreBool bAnimated)
{
    if(!m_SurferWave.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Surfer.Kill(false);

    // 
    m_SurferWave.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_SurferWave);
}


// ****************************************************************
// 
void cGeluMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    // 
    m_Line.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Line);

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Shine.Render();
    }
    glEnable(GL_DEPTH_TEST);

    DEPTH_PUSH

    // 
    m_Orb.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Orb);

    // 
    m_Gap.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Gap);
}


// ****************************************************************
// 
void cGeluMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_CoinWave.Render();
    }
    glDepthMask(true);

    // 
    m_Coin.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Coin);

    // 
    if(m_Fang.GetNumEnabled()) cLodObject::RenderHighList(&m_Fang);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Fang);

    // 
    m_Way.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Way);

    DEPTH_PUSH

    // 
    m_WayArrow.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_WayArrow);

    // 
    m_SurferWave.Render();
}


// ****************************************************************
// 
void cGeluMission::__MoveOwnAlways()
{
    // 
    g_pGame->ForEachPlayerAll([this](const cPlayer* pPlayer, const coreUintW i)
    {
        if(!pPlayer->GetInput()->vMove.IsNull())
        {
            m_avFreezeMove[i] = pPlayer->GetInput()->vMove;
        }
        else if(pPlayer->HasStatus(PLAYER_STATUS_DEAD))
        {
            m_avFreezeMove[i] = coreVector2(0.0f,0.0f);
        }
    });
}


// ****************************************************************
// 
void cGeluMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    this->__UpdateCollisionFang();

    // 
    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
    {
        const cLodObject& oFang = m_aFangRaw[i];
        if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_avOldPos[i] = oFang.GetPosition().xy();   // old frozen

        // 
        if(oFang.GetAlpha() && g_pForeground->IsVisibleObject(&oFang))
        {
            const auto nBulletFangCollFunc = [&](cBullet* OUTPUT pBullet)
            {
                const coreVector2 vDiff = pBullet->GetPosition().xy() - oFang.GetPosition().xy();

                if(InBetween(vDiff, -oFang.GetCollisionRange().xy(), oFang.GetCollisionRange().xy()))
                {
                    pBullet->Deactivate(true);   // TODO 1: add correct impact parameter

                    if(m_iStageSub == 19u)   // TODO 1: HACK!!! this code-block should definitely be in the actual stage
                    {
                        this->DisableFang(i, true);
                        this->AddExtraScore(d_cast<cPlayer*>(pBullet->GetOwner()), 40u, oFang.GetPosition());

                        g_pSpecialEffects->PlaySound(oFang.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                        g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
                    }
                }
            };
            g_pGame->GetBulletManagerPlayer()->ForEachBullet(nBulletFangCollFunc);
        }
    }

    // 
    m_Fang.MoveNormal();

    // 
    this->__UpdateCollisionWay();

    // 
    const coreVector2 vBaseDir = g_pGame->FindPlayerDual(0u)->GetDirection().xy();

    // 
    coreBool bSameBase = true;
    g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
    {
        if(!SameDirection(vBaseDir, pPlayer->GetDirection().xy()))
            bSameBase = false;
    });

    // 
    for(coreUintW i = 0u; i < GELU_WAYS; ++i)
    {
        coreObject3D* pWay   = (*m_Way     .List())[i];
        coreObject3D* pArrow = (*m_WayArrow.List())[i];
        if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        if(!HAS_BIT(m_iWayActive, i))
        {
            // 
            const coreVector2 vPos    = pWay->GetPosition ().xy();
            const coreVector2 vDir    = pWay->GetDirection().xy();
            const coreVector2 vNewPos = vPos + vPos.Normalized() * coreVector2(1.0f,0.5f) * (TIME * 50.0f);
            const coreVector2 vNewDir = coreVector2::Direction(vDir.Angle() + (TIME * 4.0f));

            // 
            pWay->SetPosition (coreVector3(vNewPos, 0.0f));
            pWay->SetDirection(coreVector3(vNewDir, 0.0f));

            // 
            if(!g_pForeground->IsVisiblePoint(vNewPos, 1.3f))
                this->DisableWay(i, false);
        }

        // 
        const coreFloat fOffset  = I_TO_F(i) * (1.0f/7.0f);
        const coreBool  bVisible = HAS_BIT(m_iWayVisible, i) || (m_bWaySpooky &&  HAS_BIT(m_iWayActive,  i));
        const coreBool  bGhost   = HAS_BIT(m_iWayGhost,   i) || (m_bWaySpooky && !HAS_BIT(m_iWayVisible, i));
        const coreBool  bFree    = HAS_BIT(m_iWayFree,    i);

        // 
        pWay->SetAlpha    (bVisible ? (bGhost ? 0.5f : 1.0f) : 0.0f);
        pWay->SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));

        // 
        pArrow->SetPosition (pWay->GetPosition ());
        pArrow->SetAlpha    (bVisible ? (bGhost ? 0.5f : 1.0f) : 0.4f);
        pArrow->SetTexOffset(pWay->GetTexOffset());

        // 
        if(!bFree) pArrow->SetDirection(pWay->GetDirection());

        // 
        m_avOldPos[i] = pWay->GetPosition().xy();   // old frozen

        // 
        if(HAS_BIT(m_iWayActive, i))
        {
            if(bSameBase && SameDirection(vBaseDir, pWay->GetDirection().xy()) && !bFree)
            {
                REMOVE_BIT(m_iWayVisible, i)
            }
            else
            {
                ADD_BIT(m_iWayVisible, i)

                const auto nBulletWayCollFunc = [&](cBullet* OUTPUT pBullet)
                {
                    const coreVector2 vDiff = pBullet->GetPosition().xy() - pWay->GetPosition().xy();

                    if(InBetween(vDiff, -pWay->GetCollisionRange().xy(), pWay->GetCollisionRange().xy()))
                        pBullet->Deactivate(true);   // TODO 1: add correct impact parameter
                };
                g_pGame->GetBulletManagerPlayer()->ForEachBullet                 (nBulletWayCollFunc);
                g_pGame->GetBulletManagerEnemy ()->ForEachBulletTyped<cOrbBullet>(nBulletWayCollFunc);
            }
        }
    }

    // 
    m_Way     .MoveNormal();
    m_WayArrow.MoveNormal();

    if(m_iWayActive)
    {
        // 
        const coreUint32 iOldGhost = m_iWayGhost;
        m_iWayGhost = 0u;

        // 
        g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
        {
            if(!pPlayer->IsRolling())
            {
                const coreVector2 vPos = pPlayer->GetPosition().xy();

                for(coreUintW j = 0u; j < GELU_WAYS; ++j)
                {
                    const coreObject3D* pWay = (*m_Way.List())[j];
                    if(!HAS_BIT(iOldGhost, j) && (!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayActive, j) || pWay->GetAlpha())) continue;

                    const coreVector2 vSize = pWay->GetCollisionRange().xy() * 1.1f;
                    const coreVector2 vDiff = pWay->GetPosition().xy() - vPos;

                    if((ABS(vDiff.x) < vSize.x) && (ABS(vDiff.y) < vSize.y))
                    {
                        ADD_BIT(m_iWayGhost, j)
                        break;
                    }
                }
            }
        });
    }

    // 
    for(coreUintW i = 0u; i < GELU_ORBS; ++i)
    {
        coreObject3D& oOrb = m_aOrbRaw[i];
        if(!oOrb.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        coreFloat fScale;
        if(m_afOrbTime[i] > 0.0f)
        {
            // 
            m_afOrbTime[i].UpdateMin(3.0f, 2.0f);
            fScale = LERPH5(2.0f, 1.0f, m_afOrbTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afOrbTime[i].UpdateMax(-3.0f, -2.0f);
            fScale = LERPH5(1.0f, 2.0f, -m_afOrbTime[i] - 1.0f);

            // 
            if(m_afOrbTime[i] <= -2.0f) this->DisableOrb(i, false);
        }

        // 
        const coreFloat fOffset = I_TO_F(i) * (1.0f/8.0f);

        // 
        oOrb.SetSize     (coreVector3(2.9f,2.9f,2.9f) * fScale);
        oOrb.SetAlpha    (2.0f - fScale);
        oOrb.SetTexOffset(coreVector2(0.0f, FRACT(-2.0f * m_fAnimation + fOffset)));
    }

    // 
    m_Orb.MoveNormal();

    // 
    for(coreUintW i = 0u; i < GELU_LINES; ++i)
    {
        coreObject3D& oLine = m_aLineRaw[i];
        if(!oLine.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreUintW     iIndex1 = (m_iLineMode == 0u) ? ((i < 12u) ? (i + (i/3u) + 1u) : (i - 12u))      : ((i)      % GELU_ORBS);
        const coreUintW     iIndex2 = (m_iLineMode == 0u) ? ((i < 12u) ? (i + (i/3u))      : (i - 12u + 4u)) : ((i + 1u) % GELU_ORBS);
        const coreObject3D& oOrb1   = m_aOrbRaw[iIndex1];
        const coreObject3D& oOrb2   = m_aOrbRaw[iIndex2];
        ASSERT((iIndex1 < GELU_ORBS) && (iIndex2 < GELU_ORBS))

        // 
        if(!this->IsOrbEnabled(iIndex1) || !this->IsOrbEnabled(iIndex2))
            this->DisableLine(i, true);

        coreFloat fScale;
        if(m_afLineTime[i] > 0.0f)
        {
            // 
            m_afLineTime[i].UpdateMin(3.0f, 2.0f);
            fScale = LERPH5(2.0f, 1.0f, m_afLineTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afLineTime[i].UpdateMax(-3.0f, -2.0f);
            fScale = LERPH5(1.0f, 2.0f, -m_afLineTime[i] - 1.0f);

            // 
            if(m_afLineTime[i] <= -2.0f) this->DisableLine(i, false);
        }

        // 
        const coreVector2 vDiff   = (oOrb1.GetPosition().xy() - oOrb2.GetPosition().xy());
        const coreVector2 vPos    = (oOrb1.GetPosition().xy() + oOrb2.GetPosition().xy()) * 0.5f;
        const coreVector2 vDir    = vDiff.IsNull() ? coreVector2(0.0f,1.0f) : vDiff.Normalized();
        const coreFloat   fLen    = vDiff.Length() * 0.5f;
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/8.0f);

        // 
        oLine.SetPosition (coreVector3(vPos, 0.0f));
        oLine.SetSize     (coreVector3(fScale, fLen, fScale));
        oLine.SetDirection(coreVector3(vDir, 0.0f));
        oLine.SetAlpha    (2.0f - fScale);
        oLine.SetTexSize  (coreVector2(0.25f, fLen * (1.0f/12.8f)) * 0.9f);
        oLine.SetTexOffset(coreVector2(1.0f,1.0f) * FRACT(-1.0f * m_fAnimation + fOffset));
    }

    // 
    m_Line.MoveNormal();

    // 
    for(coreUintW i = 0u; i < GELU_COINS; ++i)
    {
        coreObject3D* pCoin = (*m_Coin    .List())[i];
        coreObject3D* pWave = (*m_CoinWave.List())[i];
        if(!pCoin->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/8.0f);
        const coreVector2 vDir    = coreVector2::Direction( (4.0f*PI) * m_fAnimation);
        const coreVector2 vDir2   = coreVector2::Direction(-(8.0f*PI) * m_fAnimation);

        // 
        pCoin->SetDirection(coreVector3(vDir, 0.0f));
        pCoin->SetTexOffset(coreVector2(0.0f, FRACT(0.4f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pCoin->GetPosition ());
        pWave->SetDirection(coreVector3(vDir2, 0.0f));
        pWave->SetAlpha    (pCoin->GetAlpha    ());
        pWave->SetTexOffset(pCoin->GetTexOffset());
    }

    // 
    m_Coin    .MoveNormal();
    m_CoinWave.MoveNormal();

    // 
    for(coreUintW i = 0u; i < GELU_GAPS; ++i)
    {
        coreObject3D& oGap = m_aGapRaw[i];
        if(!oGap.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(HAS_BIT(m_iGapActive, i)) m_afGapTime[i].UpdateMin( 3.0f, 1.0f);
                                else m_afGapTime[i].UpdateMax(-3.0f, 0.0f);

        // 
        if(!m_afGapTime[i]) this->DisableGap(i, false);

        // 
        const coreFloat fOffset = I_TO_F(i) * (1.0f/5.0f);

        // 
        oGap.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 6.0f * LERP(1.2f, 1.0f, m_afGapTime[i]));
        oGap.SetDirection(coreVector3(coreVector2::Direction(m_fAnimation * (-4.0f*PI) + fOffset * (2.0f*PI)), 0.0f));
        oGap.SetAlpha    (BLENDH3(m_afGapTime[i]));
        oGap.SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));
    }

    // 
    m_Gap.MoveNormal();

    // 
    for(coreUintW i = 0u; i < GELU_SHINES; ++i)
    {
        coreObject3D& oShine  = m_aShineRaw[i];
        if(!oShine.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(HAS_BIT(m_iShineActive, i)) m_afShineTime[i].UpdateMin( 3.0f, 1.0f);
                                  else m_afShineTime[i].UpdateMax(-3.0f, 0.0f);

        // 
        if(!m_afShineTime[i]) this->DisableShine(i, false);

        // 
        const coreFloat   fOffset = I_TO_F(i) * (1.0f/7.0f);
        const coreVector2 vDir    = coreVector2::Direction((6.0f*PI) * m_fAnimation + (((i % 2u) == ((i / 4u) % 2u)) ? (0.25f*PI) : (0.0f*PI)));

        // 
        oShine.SetPosition (m_aOrbRaw[i].GetPosition());
        oShine.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 3.5f * LERP(1.5f, 1.0f, m_afShineTime[i]));
        oShine.SetDirection(coreVector3(vDir, 0.0f));
        oShine.SetAlpha    (BLENDH3(m_afShineTime[i]));
        oShine.SetTexOffset(coreVector2(0.0f, FRACT(0.2f * m_fAnimation + fOffset)));

        STATIC_ASSERT(GELU_SHINES <= GELU_ORBS)
    }

    // 
    m_Shine.MoveNormal();

    // 
    if(!m_Surfer.HasStatus(ENEMY_STATUS_DEAD))
    {
        m_Surfer.SetDirection(coreVector3(coreVector2::Direction((8.0f*PI) * m_fAnimation), 0.0f));
        m_Surfer.SetTexOffset(coreVector2(0.0f, 0.5f * m_fAnimation));

        m_SurferWave.SetPosition (m_Surfer.GetPosition ());
        m_SurferWave.SetSize     (m_Surfer.GetSize     () * 1.5f);
        m_SurferWave.SetDirection(m_Surfer.GetDirection());
        m_SurferWave.SetTexOffset(m_Surfer.GetTexOffset());
        m_SurferWave.Move();
    }
}


// ****************************************************************
// 
void cGeluMission::__UpdateCollisionFang()
{
    if(!m_iFangActive) return;

    // 
    m_iTouchStateOld = m_iTouchState;
    m_iTouchState    = 0u;

    // 
    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
    {
        cLodObject& oFang = m_aFangRaw[i];
        if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        oFang.SetPosition(coreVector3(oFang.GetPosition().xy(), -0.1f));   // make sure player can be inside pyramid geometry
    }

    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        coreVector2 vNewPos = pPlayer->GetPosition().xy();

        const coreVector2 vFangRange = m_aFangRaw[0].GetCollisionRange().xy();
        if(vFangRange.IsNull()) return;

        coreFloat afDistance[4] = {};
        coreUint8 iHitIndex     = 255u;

        for(coreUintW j = 0u; j < GELU_FANGS; ++j)
        {
            const cLodObject& oFang = m_aFangRaw[j];
            if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreVector2 vFangPos = oFang.GetPosition().xy();
            const coreVector2 vShift   = (pPlayer->GetOldPos() - vFangPos) / vFangRange * 0.02f;

            coreVector2 vFangMove = vFangPos - m_avOldPos[j];
            if(vFangMove.LengthSq() >= POW2(vFangRange.x)) vFangMove = coreVector2(0.0f,0.0f);   // handle teleportation

            const coreVector2 vRayPos = pPlayer->GetOldPos() + vFangMove + vShift;

            for(coreUintW k = 0u; k < 4u; ++k)
            {
                const coreVector2 vRayDir = StepRotated90(k);

                coreFloat fHitDistance = 0.0f;
                coreUint8 iHitCount    = 1u;
                if(Core::Manager::Object->TestCollision(&oFang, coreVector3(vRayPos, 0.0f), coreVector3(vRayDir, 0.0f), &fHitDistance, &iHitCount))
                {
                    if(iHitCount & 0x01u)
                    {
                        iHitIndex = j;

                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - vFangPos;
                        if(IsHorizontal(vDiff)) vNewPos.x = vFangPos.x + SIGN(vDiff.x) * vFangRange.x;
                                           else vNewPos.y = vFangPos.y + SIGN(vDiff.y) * vFangRange.y;

                        //ASSERT(k == 0u)  triggered while moving within blocks, maybe edge case when moving along the tilted edge
                        break;
                    }
                    else
                    {
                        const coreFloat fNewDistance = fHitDistance + coreVector2::Dot(vFangMove + vShift - pPlayer->GetMove(), vRayDir) - CORE_MATH_PRECISION;
                        if(fNewDistance < afDistance[k])
                        {
                            afDistance[k] = fNewDistance;
                        }
                    }
                }
            }
        }

        coreBool bIntersect = false;
        coreBool bCrush     = false;
        for(coreUintW j = 0u; j < GELU_FANGS; ++j)
        {
            const cLodObject& oFang = m_aFangRaw[j];
            if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            if(InBetween(pPlayer->GetPosition().xy(), oFang.GetPosition().xy() - vFangRange, oFang.GetPosition().xy() + vFangRange))
            {
                bIntersect = true;
                if((iHitIndex != 255u) && (iHitIndex != j) && !HAS_BIT(m_iCrushState, 2u)) bCrush = true;

                ADD_BIT(m_iTouchState, j)
                STATIC_ASSERT(GELU_FANGS <= sizeof(m_iTouchState)*8u)
            }
        }

        if(pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE)) m_abCrushImmune[i] = true;

        if(bIntersect && !m_abCrushImmune[i])
        {
            vNewPos.y += afDistance[0];
            vNewPos.x -= afDistance[1];
            vNewPos.y -= afDistance[2];
            vNewPos.x += afDistance[3];

            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));

            if(bCrush || !InBetween(vNewPos, pPlayer->GetArea().xy() * 1.01f, pPlayer->GetArea().zw() * 1.01f))
            {
                if(pPlayer->IsNormal())
                {
                    pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vNewPos);
                }
                if(HAS_BIT(m_iCrushState, 0u)) m_abCrushImmune[i] = true;

                //if(!InBetween(vNewPos, pPlayer->GetArea().xy() * 1.01f, pPlayer->GetArea().zw() * 1.01f))   // prevent damaged player being pushed outside
                //    if(HAS_BIT(m_iCrushState, 0u)) m_abCrushImmune[i] = true;
            }
        }
        else if(!bIntersect && m_abCrushImmune[i])
        {
            if(pPlayer->IsNormal() || !HAS_BIT(m_iCrushState, 1u))
            {
                m_abCrushImmune[i] = false;
            }
        }

        m_abCrushInside[i] = bIntersect;
    });
    
    if(false)
    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
    {
        const cLodObject& oFang = m_aFangRaw[i];
        if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
        
        
        const coreVector2 vFangMove = oFang.GetPosition().xy() - m_avOldPos[i];
        const coreBool    bMoving   = (!vFangMove.IsNull() && (vFangMove.LengthSq() < POW2(5.0f)));
        
        
        if(bMoving)
        {
            for(coreUintW k = 0u; k < GELU_FANGS; ++k)
            {
                if(i == k) continue;
                
                const cLodObject& oFang2 = m_aFangRaw[k];
                if(!oFang2.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;
                
                const coreVector2 vDiff = oFang2.GetPosition().xy() - oFang.GetPosition().xy();
                
                
                const coreVector2 vFangMove2 = oFang2.GetPosition().xy() - m_avOldPos[k];
                //const coreBool    bMoving2   = (!vFangMove2.IsNull() && (vFangMove2.LengthSq() < POW2(5.0f)));
                
                if((ABS(vDiff.x) < oFang.GetVisualRange().x * 2.0f + 0.5f) &&
                   (ABS(vDiff.y) < oFang.GetVisualRange().y * 2.0f + 0.5f) && ((vFangMove - vFangMove2).Length() > (0.1f * TIME)))
                //for(coreUintW j = 0u; j < 4u; ++j)
                {
                    const coreVector2 vSide = AlongCrossNormal(vDiff);//StepRotated90(j);
                    const coreVector2 vPos  = oFang.GetPosition().xy() + (vSide + vSide.Rotated90()*0.0f) * oFang.GetVisualRange().xy();
                    
                    //g_pSpecialEffects->CreateSplashSmoke(coreVector3(vPos, 0.0f), 3.0f, 1u, coreVector3(1.0f,1.0f,1.0f));
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 1u, COLOR_ENERGY_WHITE * 0.8f);
                }
            }
        }
    }
    
    //m_iFangActive
    //if(m_aFangRaw[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        coreUintW aiMap[GELU_FANGS];
        for(coreUintW i = 0u; i < GELU_FANGS; ++i) aiMap[i] = i;

        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
        {
            if(!m_aFangRaw[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            for(coreUintW j = 0u; j < GELU_FANGS; ++j)
            {
                if(i == j) continue;
                if(!m_aFangRaw[j].IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vDiff = m_aFangRaw[i].GetPosition().xy() - m_avOldPos[j];
                if(vDiff.LengthSq() < POW2(3.0f))
                {
                    const coreUint16 iFirst = i;

                    const coreUintW iNew = j;
                    const coreUintW iOld = aiMap[iFirst];

                    // find second map entry
                    coreUintW iSecond = iNew;
                    while(aiMap[iSecond] != iNew) iSecond = aiMap[iSecond];

                    // swap indices (in map)
                    aiMap[iFirst]  = iNew;
                    aiMap[iSecond] = iOld;
                }
            }
        }

        coreVector2 avOffset[GELU_FANGS];
        for(coreUintW i = 0u; i < GELU_FANGS; ++i) avOffset[i] = m_aFangRaw[i].GetTexOffset();

        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            m_aFangRaw[i].SetTexOffset(avOffset[aiMap[i]]);
    }
}


// ****************************************************************
// 
void cGeluMission::__UpdateCollisionWay()
{
    if(!m_iWayActive) return;

    // 
    m_iTouchStateOld = m_iTouchState;
    m_iTouchState    = 0u;

    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        coreVector2 vNewPos = pPlayer->GetPosition().xy();

        const coreVector2 vWayRange = (*m_Way.List())[0]->GetCollisionRange().xy();
        if(vWayRange.IsNull()) return;

        coreFloat afDistance[4] = {};
        coreUint8 iHitIndex     = 255u;

        for(coreUintW j = 0u; j < GELU_WAYS; ++j)
        {
            const coreObject3D* pWay = (*m_Way.List())[j];
            if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayVisible, j) || HAS_BIT(m_iWayGhost, j)) continue;

            const coreVector2 vWayPos = pWay->GetPosition().xy();
            const coreVector2 vShift  = (pPlayer->GetOldPos() - vWayPos) / vWayRange * 0.02f;

            coreVector2 vWayMove = vWayPos - m_avOldPos[j];
            if(vWayMove.LengthSq() >= POW2(vWayRange.x)) vWayMove = coreVector2(0.0f,0.0f);   // handle teleportation

            const coreVector2 vRayPos = pPlayer->GetOldPos() + vWayMove + vShift;

            for(coreUintW k = 0u; k < 4u; ++k)
            {
                const coreVector2 vRayDir = StepRotated90(k);

                coreFloat fHitDistance = 0.0f;
                coreUint8 iHitCount    = 1u;
                if(Core::Manager::Object->TestCollision(pWay, coreVector3(vRayPos, 0.0f), coreVector3(vRayDir, 0.0f), &fHitDistance, &iHitCount))
                {
                    if(iHitCount & 0x01u)
                    {
                        iHitIndex = j;

                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - vWayPos;
                        if(IsHorizontal(vDiff)) vNewPos.x = vWayPos.x + SIGN(vDiff.x) * vWayRange.x;
                                           else vNewPos.y = vWayPos.y + SIGN(vDiff.y) * vWayRange.y;

                        // ASSERT(k == 0u) same as for fangs
                        break;
                    }
                    else
                    {
                        const coreFloat fNewDistance = fHitDistance + coreVector2::Dot(vWayMove + vShift - pPlayer->GetMove(), vRayDir) - CORE_MATH_PRECISION;
                        if(fNewDistance < afDistance[k])
                        {
                            afDistance[k] = fNewDistance;
                        }
                    }
                }
            }
        }

        coreBool bIntersect = false;
        coreBool bCrush     = false;
        for(coreUintW j = 0u; j < GELU_WAYS; ++j)
        {
            const coreObject3D* pWay = (*m_Way.List())[j];
            if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayVisible, j) || HAS_BIT(m_iWayGhost, j)) continue;

            if(InBetween(pPlayer->GetPosition().xy(), pWay->GetPosition().xy() - vWayRange, pWay->GetPosition().xy() + vWayRange))
            {
                bIntersect = true;
                if((iHitIndex != 255u) && (iHitIndex != j) && !HAS_BIT(m_iCrushState, 2u)) bCrush = true;

                ADD_BIT(m_iTouchState, j)
                STATIC_ASSERT(GELU_WAYS <= sizeof(m_iTouchState)*8u)
            }
        }

        if(pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE)) m_abCrushImmune[i] = true;

        if(bIntersect && !m_abCrushImmune[i])
        {
            vNewPos.y += afDistance[0];
            vNewPos.x -= afDistance[1];
            vNewPos.y -= afDistance[2];
            vNewPos.x += afDistance[3];

            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));

            if(bCrush || !InBetween(vNewPos, pPlayer->GetArea().xy() * 1.01f, pPlayer->GetArea().zw() * 1.01f))
            {
                if(pPlayer->IsNormal())
                {
                    pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vNewPos);
                }
                if(HAS_BIT(m_iCrushState, 0u)) m_abCrushImmune[i] = true;

                //if(!InBetween(vNewPos, pPlayer->GetArea().xy() * 1.01f, pPlayer->GetArea().zw() * 1.01f))
                //    if(HAS_BIT(m_iCrushState, 0u)) m_abCrushImmune[i] = true;
            }
        }
        else if(!bIntersect && m_abCrushImmune[i])
        {
            if(pPlayer->IsNormal() || !HAS_BIT(m_iCrushState, 1u))
            {
                m_abCrushImmune[i] = false;
            }
        }

        if(m_iTouchState && !m_iTouchStateOld)
        {
            g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), 25.0f, 5u, COLOR_ENERGY_MAGENTA);
        }

        m_abCrushInside[i] = bIntersect;
    });
}