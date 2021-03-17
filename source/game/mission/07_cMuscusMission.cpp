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
cMuscusMission::cMuscusMission()noexcept
: m_Pearl        (MUSCUS_PEARLS)
, m_PearlWave    (MUSCUS_PEARLS)
, m_aPearlActive (0u)
, m_fAnimation   (0.0f)
{
    // 
    m_apBoss[0] = &m_Orlac;
    m_apBoss[1] = &m_Geminga;
    m_apBoss[2] = &m_Nagual;

    // 
    m_Pearl    .DefineProgram("effect_energy_flat_inst_program");
    m_PearlWave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pPearl = &m_aPearlRaw[i];
            pPearl->DefineModel  ("object_sphere.md3");
            pPearl->DefineTexture(0u, "effect_energy.png");
            pPearl->DefineProgram(iType ? "effect_energy_flat_spheric_program" : "effect_energy_flat_program");

            // set object properties
            pPearl->SetColor3 (COLOR_ENERGY_YELLOW * 0.7f);
            pPearl->SetTexSize(coreVector2(1.5f,3.0f) * 0.6f);
            pPearl->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_PearlWave.BindObject(pPearl);
                 else m_Pearl    .BindObject(pPearl);
        }
    }

    // 
    g_pGlow->BindList(&m_Pearl);
    g_pGlow->BindList(&m_PearlWave);
}


// ****************************************************************
// destructor
cMuscusMission::~cMuscusMission()
{
    // 
    g_pGlow->UnbindList(&m_Pearl);
    g_pGlow->UnbindList(&m_PearlWave);

    // 
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i) this->DisablePearl(i, false);
}


// ****************************************************************
// 
void cMuscusMission::EnablePearl(const coreUintW iIndex, const coreVector2& vPosition)
{
    ASSERT(iIndex < MUSCUS_PEARLS)
    coreObject3D* pPearl = (*m_Pearl    .List())[iIndex];
    coreObject3D* pWave  = (*m_PearlWave.List())[iIndex];

    // 
    WARN_IF(pPearl->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ADD_BIT(m_aPearlActive, iIndex)
    STATIC_ASSERT(MUSCUS_PEARLS <= sizeof(m_aPearlActive)*8u)

    // 
    pPearl->SetPosition(coreVector3(vPosition, 0.0f));
    pPearl->SetAlpha   (0.0f);
    pPearl->SetEnabled (CORE_OBJECT_ENABLE_ALL);
    pWave ->SetEnabled (CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cMuscusMission::DisablePearl(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < MUSCUS_PEARLS)
    coreObject3D* pPearl = (*m_Pearl    .List())[iIndex];
    coreObject3D* pWave  = (*m_PearlWave.List())[iIndex];

    // 
    if(!pPearl->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    REMOVE_BIT(m_aPearlActive, iIndex)

    // 
    if(!bAnimated)
    {
        pPearl->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pWave ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMuscusMission::__RenderOwnUnder()
{
    DEPTH_PUSH
    DEPTH_PUSH   // TODO: first push causes outline-overdraw artifacts, precision too low on the first level, can it be handled on outline(-shader) ?

    glDepthMask(false);
    {
        // 
        m_PearlWave.Render();
    }
    glDepthMask(true);

    // 
    m_Pearl.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Pearl);
}


// ****************************************************************
// 
void cMuscusMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
    {
        coreObject3D* pPearl = (*m_Pearl    .List())[i];
        coreObject3D* pWave  = (*m_PearlWave.List())[i];
        if(!pPearl->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(CONTAINS_BIT(m_aPearlActive, i)) pPearl->SetAlpha(MIN(pPearl->GetAlpha() + 10.0f*TIME, 1.0f));
                                       else pPearl->SetAlpha(MAX(pPearl->GetAlpha() - 10.0f*TIME, 0.0f));

        // 
        if(!pPearl->GetAlpha()) this->DisablePearl(i, false);

        // 
        const coreFloat fOffset = I_TO_F(MUSCUS_PEARLS - i) * (1.0f/7.0f);
        const coreFloat fValue  = FRACT(7.0f * m_fAnimation + fOffset);
        STATIC_ASSERT((MUSCUS_PEARLS % 7u) == 0u)

        // 
        pPearl->SetSize     (coreVector3(2.0f,2.0f,2.0f) * pPearl->GetAlpha());
        pPearl->SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pPearl->GetPosition ());
        pWave->SetSize     (pPearl->GetSize     () * LERPH3(0.0f, 2.0f, fValue));
        pWave->SetDirection(pPearl->GetDirection());
        pWave->SetAlpha    (pPearl->GetAlpha    () * LERPH3(0.0f, 1.0f, 1.0f - fValue));
        pWave->SetTexOffset(pPearl->GetTexOffset());
    }

    // 
    m_Pearl    .MoveNormal();
    m_PearlWave.MoveNormal();
}