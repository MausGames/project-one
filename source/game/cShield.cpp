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
, m_fAnimation (0.0f)
{
    // 
    //g_pGlow->BindList(&m_ShieldList);
}


// ****************************************************************
// destructor
cShieldEffect::~cShieldEffect()
{
    // 
    //g_pGlow->UnbindList(&m_ShieldList);
}


// ****************************************************************
// 
void cShieldEffect::Construct(const coreHashString& sProgramSingleName, const coreHashString& sProgramInstancedName, const coreVector3 vColor)
{
    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        m_aShield[i].DefineModel  ("object_sphere.md3");
        m_aShield[i].DefineTexture(0u, "effect_energy.png");
        m_aShield[i].DefineProgram(sProgramSingleName);
        m_aShield[i].SetColor3    (vColor);
        m_aShield[i].SetAlpha     (0.0f);
        m_aShield[i].SetTexSize   (coreVector2(3.0f,3.0f));
    }

    // 
    m_ShieldList.DefineProgram(sProgramInstancedName);
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
    if(m_ShieldList.List()->empty()) return;

    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);

    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        if(!m_apOwner[i]) continue;

        coreObject3D& oShield = m_aShield[i];
        cEnemy*       pOwner  = m_apOwner[i];

        // 
        if(pOwner)
        {
            // 
            oShield.SetPosition(pOwner->GetPosition());
            oShield.SetSize    (coreVector3(1.1f,1.1f,1.1f) * pOwner->GetVisualRadius());
        }

        // 
        oShield.SetAlpha    (1.0f);
        oShield.SetTexOffset(coreVector2(FRACT(m_fAnimation * 0.1f), 0.0f));
    }

    // 
    m_ShieldList.MoveNormal();
}


// ****************************************************************
// 
void cShieldEffect::ClearShields(const coreBool bAnimated)
{
    if(!bAnimated)
    {
        // 
        std::memset(m_apOwner, 0, sizeof(m_apOwner));
        m_ShieldList.Clear();
    }
}


// ****************************************************************
// 
void cShieldEffect::BindEnemy(cEnemy* pEnemy)
{
    if(pEnemy->HasStatus(ENEMY_STATUS_SHIELDED)) return;

    // 
    coreUintW i = 0u;
    for(; i < SHIELD_SHIELDS; ++i)
    {
        if(!m_apOwner[i]) break;
    }
    ASSERT(i < SHIELD_SHIELDS)

    // 
    m_apOwner[i] = pEnemy;

    // 
    pEnemy->AddStatus(ENEMY_STATUS_SHIELDED);

    // 
    m_ShieldList.BindObject(&m_aShield[i]);
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

            // 
            m_ShieldList.UnbindObject(&m_aShield[i]);

            return;
        }
    }

    //ASSERT(false)
}


// ****************************************************************
// constructor
cShieldManager::cShieldManager()noexcept
{
    // 
    m_aShieldEffect[SHIELD_EFFECT_INVINCIBLE].Construct("effect_energy_spheric_program", "effect_energy_spheric_inst_program", COLOR_ENERGY_BLUE);
    m_aShieldEffect[SHIELD_EFFECT_DAMAGING]  .Construct("effect_energy_program",         "effect_energy_inst_program",         COLOR_ENERGY_RED);
}


// ****************************************************************
// render the shield manager
void cShieldManager::Render()
{
    //
    //for(coreUintW i = 0u; i < SHIELD_EFFECTS; ++i)
    //    m_aShieldEffect[i].Render();
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