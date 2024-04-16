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
cShieldEffect::cShieldEffect()noexcept
: m_ShieldList (SHIELD_SHIELDS)
, m_apOwner    {}
, m_iCurShield (0u)
, m_fScale     (0.0f)
, m_fTexScale  (0.0f)
, m_fTexSpeed  (0.0f)
, m_fAnimation (0.0f)
{
    // 
    g_pGlow->BindList(&m_ShieldList);
}


// ****************************************************************
// destructor
cShieldEffect::~cShieldEffect()
{
    // 
    g_pGlow->UnbindList(&m_ShieldList);
}


// ****************************************************************
// 
void cShieldEffect::Construct(const coreHashString& sModelName, const coreHashString& sProgramSingleName, const coreHashString& sProgramInstancedName, const coreFloat fScale, const coreFloat fTexScale, const coreVector3 vColor)
{
    // 
    m_ShieldList.DefineProgram(sProgramInstancedName);
    {
        for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
        {
            // load object resources
            coreObject3D* pShield = &m_aShield[i];
            pShield->DefineModel  (sModelName);
            pShield->DefineTexture(0u, "effect_energy.png");
            pShield->DefineProgram(sProgramSingleName);

            // set object properties
            pShield->SetColor3 (vColor);
            pShield->SetAlpha  (0.0f);
            pShield->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_ShieldList.BindObject(pShield);
        }
    }

    // 
    m_fScale    = fScale;
    m_fTexScale = fTexScale;
}


// ****************************************************************
// render the shield effect
void cShieldEffect::Render()
{
    // 
    m_ShieldList.Render();
}


// ****************************************************************
// move the shield effect
void cShieldEffect::Move()
{
    // 
    m_fAnimation.UpdateMod(m_fTexSpeed, 10.0f);
    m_fTexSpeed = 0.0f;

    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        coreObject3D* pShield = &m_aShield[i];
        if(!pShield->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cEnemy* pOwner = m_apOwner[i];
        if(pOwner)
        {
            const coreFloat fRealScale    = m_fScale * pOwner->GetVisualRadius();
            const coreFloat fRealTexScale = m_fTexScale * fRealScale;

            // 
            pShield->SetPosition(pOwner->GetPosition());
            pShield->SetSize    (coreVector3(1.1f,1.1f,1.1f) * fRealScale);
            pShield->SetTexSize (coreVector2(1.0f,1.0f)      * fRealTexScale);

            // 
            m_fTexSpeed = 0.04f * fRealTexScale;
        }

        const coreFloat fOffset = I_TO_F(i) * (1.0f/8.0f);

        // 
        if(pOwner) pShield->SetAlpha(MIN1(pShield->GetAlpha() + 5.0f * TIME));
              else pShield->SetAlpha(MAX0(pShield->GetAlpha() - 5.0f * TIME));

        // 
        if(!pShield->GetAlpha()) pShield->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

        // 
        pShield->SetTexOffset(coreVector2(FRACT(m_fAnimation + fOffset), 0.0f));
    }

    // 
    m_ShieldList.MoveNormal();
}


// ****************************************************************
// 
void cShieldEffect::ClearShields(const coreBool bAnimated)
{
    // 
    std::memset(m_apOwner, 0, sizeof(m_apOwner));

    if(!bAnimated)
    {
        // 
        for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
        {
            m_aShield[i].SetAlpha  (0.0f);
            m_aShield[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    }
}


// ****************************************************************
// 
void cShieldEffect::BindEnemy(cEnemy* pEnemy)
{
    if(pEnemy->HasStatus(ENEMY_STATUS_SHIELDED)) return;

    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        // 
        if(++m_iCurShield >= SHIELD_SHIELDS) m_iCurShield = 0u;

        if(!m_apOwner[m_iCurShield])
        {
            // 
            m_aShield[m_iCurShield].SetEnabled(CORE_OBJECT_ENABLE_ALL);

            // 
            m_apOwner[m_iCurShield] = pEnemy;

            // 
            pEnemy->AddStatus(ENEMY_STATUS_SHIELDED);
            return;
        }
    }

    // 
    WARN_IF(true) {}
}


// ****************************************************************
// 
void cShieldEffect::UnbindEnemy(cEnemy* pEnemy)
{
    if(!pEnemy->HasStatus(ENEMY_STATUS_SHIELDED)) return;

    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        if(m_apOwner[i] == pEnemy)
        {
            // 
            m_apOwner[i] = NULL;

            // 
            pEnemy->RemoveStatus(ENEMY_STATUS_SHIELDED);
            return;
        }
    }

    // 
    WARN_IF(true) {}
}


// ****************************************************************
// constructor
cShieldManager::cShieldManager()noexcept
{
    // 
    m_aShieldEffect[SHIELD_EFFECT_INVINCIBLE].Construct("object_sphere.md3", "effect_energy_flat_spheric_program", "effect_energy_flat_spheric_inst_program", 1.05f, 0.7f, COLOR_ENERGY_BLUE);
    m_aShieldEffect[SHIELD_EFFECT_DAMAGING]  .Construct("object_star.md3",   "effect_energy_flat_spheric_program", "effect_energy_flat_spheric_inst_program", 0.8f, 0.1f, COLOR_ENERGY_RED * 0.6f);
    m_aShieldEffect[SHIELD_EFFECT_BASE]      .Construct("object_sphere.md3", "effect_energy_flat_program", "effect_energy_flat_inst_program", 1.0f, 1.0f, COLOR_ENERGY_WHITE * 0.6f);
}


// ****************************************************************
// render the shield manager
void cShieldManager::Render()
{
    //
    for(coreUintW i = 0u; i < SHIELD_EFFECTS; ++i)
        m_aShieldEffect[i].Render();
}


// ****************************************************************
// move the shield manager
void cShieldManager::Move()
{
    //
    for(coreUintW i = 0u; i < SHIELD_EFFECTS; ++i)
        m_aShieldEffect[i].Move();
}


// ****************************************************************
// 
void cShieldManager::ClearShields(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < SHIELD_EFFECTS; ++i)
        m_aShieldEffect[i].ClearShields(bAnimated);
}