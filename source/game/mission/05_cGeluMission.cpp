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
: m_Orb        (GELU_ORBS)
, m_afOrbTime  {}
, m_Web        (GELU_WEBS)
, m_afWebTime  {}
, m_fAnimation (0.0f)
{
    // 
    m_apBoss[0] = &m_Tartarus;
    m_apBoss[1] = &m_Phalaris;
    m_apBoss[2] = &m_Chol;

    // 
    m_Orb.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_ORBS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pOrb = &m_aOrbRaw[i];
            pOrb->DefineModel  ("object_sphere.md3");
            pOrb->DefineTexture(0u, "effect_energy.png");
            pOrb->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pOrb->SetColor3 (COLOR_ENERGY_CYAN);
            pOrb->SetTexSize(coreVector2(4.0f,4.0f));
            pOrb->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Orb.BindObject(pOrb);
        }
    }

    // 
    m_Web.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < GELU_WEBS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pWeb = &m_aWebRaw[i];
            pWeb->DefineModel  ("object_tube_open.md3");
            pWeb->DefineTexture(0u, "effect_energy.png");
            pWeb->DefineProgram("effect_energy_flat_spheric_program");

            // set object properties
            pWeb->SetColor3 (COLOR_ENERGY_CYAN * 0.5f);
            pWeb->SetTexSize(coreVector2(0.5f,2.0f));
            pWeb->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Web.BindObject(pWeb);
        }
    }

    // 
    g_pGlow->BindList(&m_Orb);
    g_pGlow->BindList(&m_Web);
}


// ****************************************************************
// destructor
cGeluMission::~cGeluMission()
{
    // 
    g_pGlow->UnbindList(&m_Orb);
    g_pGlow->UnbindList(&m_Web);

    // 
    for(coreUintW i = 0u; i < GELU_ORBS; ++i) this->DisableOrb(i, false);
    for(coreUintW i = 0u; i < GELU_WEBS; ++i) this->DisableWeb(i, false);
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

    // 
    if(m_afOrbTime[iIndex] > 0.0f) m_afOrbTime[iIndex] = -1.0f;

    // 
    if(!bAnimated) oOrb.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    if(bAnimated) g_pSpecialEffects->CreateSplashColor(oOrb.GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_CYAN);
}


// ****************************************************************
// 
void cGeluMission::EnableWeb(const coreUintW iIndex)
{
    ASSERT(iIndex < GELU_WEBS)
    coreObject3D& oWeb = m_aWebRaw[iIndex];

    // 
    WARN_IF(oWeb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afWebTime[iIndex] = 1.0f;

    // 
    oWeb.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGeluMission::DisableWeb(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < GELU_WEBS)
    coreObject3D& oWeb = m_aWebRaw[iIndex];

    // 
    if(!oWeb.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    if(m_afWebTime[iIndex] > 0.0f) m_afWebTime[iIndex] = -1.0f;

    // 
    if(!bAnimated) oWeb.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cGeluMission::__RenderOwnBottom()
{
    // 
    m_Web.Render();
    m_Orb.Render();
}


// ****************************************************************
// 
void cGeluMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

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
    for(coreUintW i = 0u; i < GELU_WEBS; ++i)
    {
        coreObject3D& oWeb = m_aWebRaw[i];
        if(!oWeb.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreUintW     iIndex1 = (i < 12u) ? (i + (i/3u) + 1u) : (i - 12u);
        const coreUintW     iIndex2 = (i < 12u) ? (i + (i/3u))      : (i - 12u + 4u);
        const coreObject3D& oOrb1   = m_aOrbRaw[iIndex1];
        const coreObject3D& oOrb2   = m_aOrbRaw[iIndex2];
        STATIC_ASSERT((GELU_ORBS == 16u) && (GELU_WEBS == 24u))

        // 
        if((m_afOrbTime[iIndex1] < 0.0f) || (m_afOrbTime[iIndex2] < 0.0f))
            this->DisableWeb(i, true);

        coreFloat fScale;
        if(m_afWebTime[i] > 0.0f)
        {
            // 
            m_afWebTime[i].UpdateMin(2.0f, 2.0f);
            fScale = LERPH5(2.0f, 1.0f, m_afWebTime[i] - 1.0f);
        }
        else
        {
            // 
            m_afWebTime[i].UpdateMax(-2.0f, -2.0f);
            fScale = LERPH5(1.0f, 2.0f, -m_afWebTime[i] - 1.0f);

            // 
            if(m_afWebTime[i] <= -2.0f) this->DisableWeb(i, false);
        }

        // 
        const coreVector2 vDiff = (oOrb1.GetPosition().xy() - oOrb2.GetPosition().xy());
        const coreVector2 vPos  = (oOrb1.GetPosition().xy() + oOrb2.GetPosition().xy()) * 0.5f;
        const coreVector2 vDir  = vDiff.Normalized();
        const coreFloat   fLen  = vDiff.Length() * 0.5f;

        // 
        oWeb.SetPosition (coreVector3(vPos, 0.0f));
        oWeb.SetSize     (coreVector3(fScale, fLen, fScale));
        oWeb.SetDirection(coreVector3(vDir, 0.0f));
        oWeb.SetAlpha    (2.0f - fScale);
        oWeb.SetTexOffset(coreVector2(1.0f,1.0f) * FRACT(-2.4f * m_fAnimation));
    }

    // 
    m_Web.MoveNormal();
}