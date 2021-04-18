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
: m_Generate       (MUSCUS_GENERATES)
, m_GenerateWave   (MUSCUS_GENERATES)
, m_afGenerateTime {}
, m_afGenerateBang {}
, m_afGenerateView {}
, m_Pearl          (MUSCUS_PEARLS)
, m_PearlWave      (MUSCUS_PEARLS)
, m_iPearlActive   (0u)
, m_fAnimation     (0.0f)
{
    // 
    m_apBoss[0] = &m_Orlac;
    m_apBoss[1] = &m_Geminga;
    m_apBoss[2] = &m_Nagual;

    // 
    m_Generate    .DefineProgram("effect_energy_flat_invert_inst_program");
    m_GenerateWave.DefineProgram("effect_energy_flat_spheric_inst_program");
    {
        for(coreUintW i = 0u; i < MUSCUS_GENERATES_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % 2u;

            // load object resources
            coreObject3D* pGenerate = &m_aGenerateRaw[i];
            pGenerate->DefineModel  ("object_cube_top.md3");
            pGenerate->DefineTexture(0u, "effect_energy.png");
            pGenerate->DefineProgram(iType ? "effect_energy_flat_spheric_program" : "effect_energy_flat_invert_program");

            // set object properties
            pGenerate->SetColor3 (COLOR_ENERGY_GREEN * 0.7f);
            pGenerate->SetTexSize(coreVector2(1.0f,1.0f) * 0.5f);
            pGenerate->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_GenerateWave.BindObject(pGenerate);
                 else m_Generate    .BindObject(pGenerate);
        }
    }

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
    g_pGlow->BindList(&m_Generate);
    g_pGlow->BindList(&m_GenerateWave);
    g_pGlow->BindList(&m_Pearl);
    g_pGlow->BindList(&m_PearlWave);
}


// ****************************************************************
// destructor
cMuscusMission::~cMuscusMission()
{
    // 
    g_pGlow->UnbindList(&m_Generate);
    g_pGlow->UnbindList(&m_GenerateWave);
    g_pGlow->UnbindList(&m_Pearl);
    g_pGlow->UnbindList(&m_PearlWave);

    // 
    for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i) this->DisableGenerate(i, false);
    for(coreUintW i = 0u; i < MUSCUS_PEARLS;    ++i) this->DisablePearl   (i, false);
}


// ****************************************************************
// 
void cMuscusMission::EnableGenerate(const coreUintW iIndex)
{
    ASSERT(iIndex < MUSCUS_GENERATES)
    coreObject3D* pGenerate = (*m_Generate    .List())[iIndex];
    coreObject3D* pWave     = (*m_GenerateWave.List())[iIndex];

    // 
    WARN_IF(pGenerate->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afGenerateTime[iIndex] = 0.0f;
    m_afGenerateBang[iIndex] = 0.0f;
    m_afGenerateView[iIndex] = 0.0f;

    // 
    pGenerate->SetAlpha  (0.0f);
    pGenerate->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave    ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cMuscusMission::DisableGenerate(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < MUSCUS_GENERATES)
    coreObject3D* pGenerate = (*m_Generate    .List())[iIndex];
    coreObject3D* pWave     = (*m_GenerateWave.List())[iIndex];

    // 
    if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_afGenerateTime[iIndex] = -1.0f;

    // 
    if(!bAnimated)
    {
        pGenerate->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pWave    ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMuscusMission::EnablePearl(const coreUintW iIndex)
{
    ASSERT(iIndex < MUSCUS_PEARLS)
    coreObject3D* pPearl = (*m_Pearl    .List())[iIndex];
    coreObject3D* pWave  = (*m_PearlWave.List())[iIndex];

    // 
    WARN_IF(pPearl->IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ADD_BIT(m_iPearlActive, iIndex)
    STATIC_ASSERT(MUSCUS_PEARLS <= sizeof(m_iPearlActive)*8u)

    // 
    pPearl->SetAlpha  (0.0f);
    pPearl->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    pWave ->SetEnabled(CORE_OBJECT_ENABLE_ALL);
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
    REMOVE_BIT(m_iPearlActive, iIndex)

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
    DEPTH_PUSH   // TODO: precision artifacts

    glDepthMask(false);
    {
        // 
        m_GenerateWave.Render();

        // 
        m_PearlWave.Render();
    }
    glDepthMask(true);

    // 
    m_Generate.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Generate);

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
    for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
    {
        coreObject3D* pGenerate = (*m_Generate    .List())[i];
        coreObject3D* pWave     = (*m_GenerateWave.List())[i];
        if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        m_afGenerateBang[i].UpdateMax(-1.0f, 0.0f);

        if(m_afGenerateTime[i] >= 0.0f)
        {
            // 
            m_afGenerateTime[i].UpdateMax(-1.0f, 0.0f);

            // 
                 if(m_afGenerateBang[i]) m_afGenerateView[i] = 1.0f;
            else if(m_afGenerateTime[i]) m_afGenerateView[i].UpdateMin( 3.0f, 1.0f);
            else                         m_afGenerateView[i].UpdateMax(-3.0f, 0.0f);
        }
        else
        {
            // 
            m_afGenerateView[i].UpdateMax(-3.0f, 0.0f);

            // 
            if(!m_afGenerateView[i]) this->DisableGenerate(i, false);
        }

        // 
        const coreFloat fOffset = I_TO_F(i) * (1.0f/8.0f);
        const coreFloat fBang   = LERPB(0.0f, 1.0f, 1.0f - m_afGenerateBang[i]);

        // 
        pGenerate->SetSize     (coreVector3(5.0f,5.0f,5.0f) * LERP(1.2f, 1.0f, fBang));
        pGenerate->SetAlpha    (LERPH3(0.0f, 1.0f, m_afGenerateView[i]));
        pGenerate->SetTexOffset(coreVector2(0.0f, FRACT(-0.3f * m_fAnimation + fOffset)));

        // 
        pWave->SetPosition (pGenerate->GetPosition ());
        pWave->SetSize     (pGenerate->GetSize     () * LERP(1.0f, 1.6f, fBang));
        pWave->SetDirection(pGenerate->GetDirection());
        pWave->SetAlpha    (pGenerate->GetAlpha    () * LERP(1.0f, 0.0f, fBang));
        pWave->SetTexOffset(pGenerate->GetTexOffset());
    }

    // 
    m_Generate    .MoveNormal();
    m_GenerateWave.MoveNormal();

    // 
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
    {
        coreObject3D* pPearl = (*m_Pearl    .List())[i];
        coreObject3D* pWave  = (*m_PearlWave.List())[i];
        if(!pPearl->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(HAS_BIT(m_iPearlActive, i)) pPearl->SetAlpha(MIN(pPearl->GetAlpha() + 10.0f*TIME, 1.0f));
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