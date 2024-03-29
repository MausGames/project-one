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
cMuscusMission::cMuscusMission()noexcept
: m_Generate       (MUSCUS_GENERATES)
, m_GenerateWave   (MUSCUS_GENERATES)
, m_afGenerateTime {}
, m_afGenerateBang {}
, m_afGenerateView {}
, m_Pearl          (MUSCUS_PEARLS)
, m_PearlWave      (MUSCUS_PEARLS)
, m_iPearlActive   (0u)
, m_afStrikeTime   {}
, m_apStrikePlayer {}
, m_apStrikeTarget {}
, m_iStrikeState   (0u)
, m_fAnimation     (0.0f)
{
    // 
    m_apBoss[0] = &m_Geminga;

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
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
    {
        m_aStrikeSpline[i].Reserve(2u);
        m_aStrikeSpline[i].AddNode(coreVector2(0.0f,0.0f), coreVector2(0.0f,0.0f));
        m_aStrikeSpline[i].AddNode(coreVector2(0.0f,0.0f), coreVector2(0.0f,0.0f), 3.0f);
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
    m_afStrikeTime  [iIndex] = 0.0f;
    m_apStrikePlayer[iIndex] = NULL;
    m_apStrikeTarget[iIndex] = NULL;

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
    m_apStrikeTarget[iIndex] = NULL;   // only

    // 
    if(!bAnimated)
    {
        pPearl->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        pWave ->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMuscusMission::StrikeAttack(const coreUintW iIndex, cPlayer* pPlayer, const cShip* pTarget)
{
    ASSERT(iIndex < MUSCUS_PEARLS)
    coreObject3D* pPearl = (*m_Pearl.List())[iIndex];

    ASSERT(pPlayer && pTarget && HAS_BIT(m_iPearlActive, iIndex))

    // 
    const coreVector2 vDirIn  = (pTarget->GetPosition().xy() - pPearl->GetPosition().xy()).Normalized();
    const coreVector2 vDirOut = (-vDirIn + vDirIn.Rotated90() * Core::Rand->Float(1.0f) * ((iIndex & 0x01u) ? -1.0f : 1.0f)).Normalized() * Core::Rand->Float(2.5f,3.5f);

    // 
    m_aStrikeSpline[iIndex].EditNodePosition(0u, pPearl ->GetPosition().xy());
    m_aStrikeSpline[iIndex].EditNodePosition(1u, pTarget->GetPosition().xy());
    m_aStrikeSpline[iIndex].EditNodeTangent (0u, vDirOut);
    m_aStrikeSpline[iIndex].EditNodeTangent (1u, vDirIn);

    // 
    m_afStrikeTime  [iIndex] = 0.0f;
    m_apStrikePlayer[iIndex] = pPlayer;
    m_apStrikeTarget[iIndex] = pTarget;
}


// ****************************************************************
// 
void cMuscusMission::__RenderOwnUnder()
{
    DEPTH_PUSH

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
    m_iStrikeState = 0u;

    // 
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
    {
        coreObject3D* pPearl = (*m_Pearl    .List())[i];
        coreObject3D* pWave  = (*m_PearlWave.List())[i];
        if(!pPearl->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        if(m_apStrikeTarget[i])
        {
            // 
            m_aStrikeSpline[i].EditNodePosition(1u, m_apStrikeTarget[i]->GetPosition().xy());
            m_afStrikeTime [i].UpdateMin(1.0f, 1.0f);

            // 
            pPearl->SetPosition(coreVector3(m_aStrikeSpline[i].CalcPositionLerp(m_afStrikeTime[i]), 0.0f));

            if(m_afStrikeTime[i] >= 1.0f)
            {
                // 
                this->DisablePearl(i, true);
                g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_YELLOW);

                // 
                ADD_BIT(m_iStrikeState, i)
                STATIC_ASSERT(MUSCUS_PEARLS <= sizeof(m_iStrikeState)*8u)
            }
        }

        // 
        if(HAS_BIT(m_iPearlActive, i)) pPearl->SetAlpha(MIN(pPearl->GetAlpha() + 10.0f*TIME, 1.0f));
                                  else pPearl->SetAlpha(MAX(pPearl->GetAlpha() - 10.0f*TIME, 0.0f));

        // 
        if(!pPearl->GetAlpha()) this->DisablePearl(i, false);

        // 
        const coreFloat fOffset = I_TO_F(MUSCUS_PEARLS - i) * (1.0f/7.0f);
        const coreFloat fValue  = FRACT(7.0f * m_fAnimation + fOffset);
        STATIC_ASSERT(coreMath::IsAligned(MUSCUS_PEARLS, 7u))

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