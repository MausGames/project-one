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
cGeluMission::cGeluMission()noexcept
: m_Fang        (GELU_FANGS)
, m_Way         (GELU_WAYS)
, m_WayArrow    (GELU_WAYS)
, m_iWayActive  (0u)
, m_iWayVisible (0u)
, m_Orb         (GELU_ORBS)
, m_afOrbTime   {}
, m_Line        (GELU_LINES)
, m_afLineTime  {}
, m_fAnimation  (0.0f)
{
    // 
    m_apBoss[0] = &m_Tartarus;
    m_apBoss[1] = &m_Phalaris;
    m_apBoss[2] = &m_Chol;

    // 
    m_Fang.DefineProgram("object_ship_glow_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_FANGS_RAWS; ++i)
        {
            // load object resources
            cLodObject* pFang = &m_aFangRaw[i];
            pFang->DefineModelHigh("object_fang.md3");
            pFang->DefineModelLow ("object_fang.md3");
            pFang->DefineTexture  (0u, "ship_enemy.png");
            pFang->DefineProgram  ("object_ship_glow_program");

            // set object properties
            pFang->SetSize   (coreVector3(1.0f,1.0f,1.0f) * 9.0f);
            pFang->SetColor3 (coreVector3(0.5f,0.5f,0.5f));
            pFang->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

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
            pWay->DefineModel  (iType ? "object_arrow.md3" : "object_cube_top.md3");
            pWay->DefineTexture(0u, "effect_energy.png");
            pWay->DefineProgram(iType ? "effect_energy_flat_invert_program" : "effect_energy_flat_spheric_program");

            // set object properties
            pWay->SetSize   (coreVector3(1.0f,1.0f,1.0f) * (iType ? 2.0f : 7.0f));
            pWay->SetColor3 (COLOR_ENERGY_MAGENTA * 0.8f);
            pWay->SetTexSize(coreVector2(1.0f,1.0f) * (iType ? 0.4f : 0.7f));
            pWay->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

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
            pOrb->SetColor3 (COLOR_ENERGY_CYAN);
            pOrb->SetTexSize(coreVector2(4.0f,4.0f));
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
            pLine->DefineModel  ("object_tube_open.md3");
            pLine->DefineTexture(0u, "effect_energy.png");
            pLine->DefineProgram("effect_energy_flat_invert_program");

            // set object properties
            pLine->SetColor3 (COLOR_ENERGY_CYAN * 0.5f);
            pLine->SetTexSize(coreVector2(0.5f,2.0f));
            pLine->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Line.BindObject(pLine);
        }
    }

    // 
    g_pGlow->BindList(&m_Way);
    g_pGlow->BindList(&m_WayArrow);
    g_pGlow->BindList(&m_Orb);
    g_pGlow->BindList(&m_Line);
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

    // 
    for(coreUintW i = 0u; i < GELU_FANGS; ++i) this->DisableFang(i, false);
    for(coreUintW i = 0u; i < GELU_WAYS;  ++i) this->DisableWay (i, false);
    for(coreUintW i = 0u; i < GELU_ORBS;  ++i) this->DisableOrb (i, false);
    for(coreUintW i = 0u; i < GELU_LINES; ++i) this->DisableLine(i, false);
}


// ****************************************************************
// 
void cGeluMission::EnableFang(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_FANGS)
    cLodObject& oFang = m_aFangRaw[iIndex];

    // 
    WARN_IF(oFang.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oFang.SetPosition(coreVector3(HIDDEN_POS, 0.0f));
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
    oFang.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->MacroDestructionDark(&oFang);
}


// ****************************************************************
// 
void cGeluMission::EnableWay(const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection)
{
    ASSERT(iIndex < GELU_WAYS)
    coreObject3D* pWay   = (*m_Way     .List())[iIndex];
    coreObject3D* pArrow = (*m_WayArrow.List())[iIndex];

    // 
    WARN_IF(pWay->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ADD_BIT(m_iWayActive,  iIndex)
    ADD_BIT(m_iWayVisible, iIndex)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayActive) *8u)
    STATIC_ASSERT(GELU_WAYS <= sizeof(m_iWayVisible)*8u)

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
    WARN_IF(oOrb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

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

    if(m_afOrbTime[iIndex] > 0.0f)
    {
        // 
        m_afOrbTime[iIndex] = -1.0f;

        // 
        if(bAnimated) g_pSpecialEffects->CreateSplashColor(oOrb.GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_CYAN);
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
    WARN_IF(oLine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afLineTime[iIndex] = 1.0f;

    // 
    oLine.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableLine(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_LINES)
    coreObject3D& oLine = m_aLineRaw[iIndex];

    // 
    if(!oLine.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_afLineTime[iIndex] > 0.0f) m_afLineTime[iIndex] = -1.0f;

    // 
    if(!bAnimated) oLine.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cGeluMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Line.Render();
        m_Orb .Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cGeluMission::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_Fang.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Fang);

    // 
    m_Way.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Way);

    DEPTH_PUSH

    // 
    m_WayArrow.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_WayArrow);
}


// ****************************************************************
// 
void cGeluMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
    {
        cLodObject& oFang = m_aFangRaw[i];
        if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // nothing
    }

    // 
    m_Fang.MoveNormal();

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
        const coreBool  bVisible = HAS_BIT(m_iWayVisible, i);

        // 
        pWay->SetAlpha    (bVisible ? 1.0f : 0.0f);
        pWay->SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));

        // 
        pArrow->SetPosition (pWay->GetPosition ());
        pArrow->SetDirection(pWay->GetDirection());
        pArrow->SetAlpha    (bVisible ? 1.0f : 0.4f);
        pArrow->SetTexOffset(pWay->GetTexOffset());
    }

    // 
    m_Way     .MoveNormal();
    m_WayArrow.MoveNormal();

    // 
    for(coreUintW i = 0u; i < GELU_ORBS; ++i)
    {
        coreObject3D& oOrb = m_aOrbRaw[i];
        if(!oOrb.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        coreFloat fScale;
        if(m_afOrbTime[i] > 0.0f)
        {
            // 
            m_afOrbTime[i].UpdateMin(2.0f, 2.0f);
            fScale = LERPH5(2.0f, 1.0f, m_afOrbTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afOrbTime[i].UpdateMax(-2.0f, -2.0f);
            fScale = LERPH5(1.0f, 2.0f, -m_afOrbTime[i] - 1.0f);

            // 
            if(m_afOrbTime[i] <= -2.0f) this->DisableOrb(i, false);
        }

        // 
        oOrb.SetSize     (coreVector3(2.5f,2.5f,2.5f) * fScale);
        oOrb.SetAlpha    (2.0f - fScale);
        oOrb.SetTexOffset(coreVector2(0.0f, FRACT(-2.4f * m_fAnimation)));
    }

    // 
    m_Orb.MoveNormal();

    // 
    for(coreUintW i = 0u; i < GELU_LINES; ++i)
    {
        coreObject3D& oLine = m_aLineRaw[i];
        if(!oLine.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreUintW     iIndex1 = (i < 12u) ? (i + (i/3u) + 1u) : (i - 12u);
        const coreUintW     iIndex2 = (i < 12u) ? (i + (i/3u))      : (i - 12u + 4u);
        const coreObject3D& oOrb1   = m_aOrbRaw[iIndex1];
        const coreObject3D& oOrb2   = m_aOrbRaw[iIndex2];
        STATIC_ASSERT((GELU_ORBS == 16u) && (GELU_LINES == 24u))

        // 
        if((m_afOrbTime[iIndex1] < 0.0f) || (m_afOrbTime[iIndex2] < 0.0f))
            this->DisableLine(i, true);

        coreFloat fScale;
        if(m_afLineTime[i] > 0.0f)
        {
            // 
            m_afLineTime[i].UpdateMin(2.0f, 2.0f);
            fScale = LERPH5(2.0f, 1.0f, m_afLineTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afLineTime[i].UpdateMax(-2.0f, -2.0f);
            fScale = LERPH5(1.0f, 2.0f, -m_afLineTime[i] - 1.0f);

            // 
            if(m_afLineTime[i] <= -2.0f) this->DisableLine(i, false);
        }

        // 
        const coreVector2 vDiff = (oOrb1.GetPosition().xy() - oOrb2.GetPosition().xy());
        const coreVector2 vPos  = (oOrb1.GetPosition().xy() + oOrb2.GetPosition().xy()) * 0.5f;
        const coreVector2 vDir  = vDiff.Normalized();
        const coreFloat   fLen  = vDiff.Length() * 0.5f;

        // 
        oLine.SetPosition (coreVector3(vPos, 0.0f));
        oLine.SetSize     (coreVector3(fScale, fLen, fScale));
        oLine.SetDirection(coreVector3(vDir, 0.0f));
        oLine.SetAlpha    (2.0f - fScale);
        oLine.SetTexOffset(coreVector2(1.0f,1.0f) * FRACT(-2.4f * m_fAnimation));
    }

    // 
    m_Line.MoveNormal();
}