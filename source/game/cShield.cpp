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
cShieldManager::cShieldManager()noexcept
: m_ShieldList (SHIELD_SHIELDS)
, m_apOwner    {}
, m_aiElement  {}
, m_aiHealth   {}
, m_afExtent   {}
, m_fAnimation (0.0f)
{
    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        m_aShield[i].DefineModel  ("effect_shield.md3");
        m_aShield[i].DefineTexture(0u, "effect_particle_128.png");
        m_aShield[i].DefineProgram("effect_shield_program");
    }

    // 
    m_ShieldList.DefineProgram("effect_shield_inst_program");

    // 
    g_pGlow->BindList(&m_ShieldList);
}


// ****************************************************************
// destructor
cShieldManager::~cShieldManager()
{
    // 
    g_pGlow->UnbindList(&m_ShieldList);
}


// ****************************************************************
// render the shield manager
void cShieldManager::Render()
{
    // 
    m_ShieldList.Render();
}


// ****************************************************************
// move the shield manager
void cShieldManager::Move()
{
    if(m_ShieldList.List()->empty()) return;

    // 
    m_fAnimation.Update(1.0f);
    const coreVector2 vDir  = coreVector2::Direction(m_fAnimation);
    const coreFloat   fWave = 0.25f + 0.05f * SIN(2.0f * m_fAnimation);

    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        if(!m_afExtent[i]) continue;

        coreObject3D& oShield = m_aShield [i];
        cEnemy*       pOwner  = m_apOwner [i];
        coreInt16&    iHealth = m_aiHealth[i];
        coreFlow&     fExtent = m_afExtent[i];

        // 
        if(iHealth <= 0)
        {
            // 
            fExtent.Update(-2.0f);
            if(fExtent < 0.0f)
            {
                fExtent = 0.0f;

                // 
                if(pOwner) this->UnbindEnemy(pOwner);
                m_ShieldList.UnbindObject(&oShield);
                continue;
            }
        }

        // 
        const coreFloat fExtent2 = fExtent * fExtent;
        const coreFloat fExtent3 = fExtent * fExtent2;
        coreFloat fBlink = 0.0f;

        // 
        if(pOwner)
        {
            // 
            oShield.SetPosition(pOwner->GetPosition());
            oShield.SetSize    (coreVector3(1.3f,1.3f,1.3f) * (pOwner->GetModel()->GetBoundingRadius() * pOwner->GetSize().Max()));

            // 
            if(iHealth > 0) fBlink = pOwner->GetBlink() * 0.8f;
        }

        // 
        oShield.SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
        oShield.SetTexOffset  (coreVector2((1.0f - fExtent3) * 1.5f + fWave, fBlink));
        oShield.SetAlpha      (MIN(fExtent2 * 1.4f, 1.0f));
    }

    // 
    m_ShieldList.MoveNormal();
}


// ****************************************************************
// 
void cShieldManager::AbsorbDamage(cEnemy* pEnemy, coreInt32* OUTPUT piDamage, const coreUint8 iElement)
{
    if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_SHIELDED)) return;
    if((*piDamage) <= 0) return;

    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        if(m_apOwner[i] == pEnemy)
        {
            // 
            if(m_aiHealth[i] > 0)
            {
                if(m_aiElement[i] == iElement)
                {
                    // 
                    m_aiHealth[i] -= (*piDamage);
                }
                else
                {
                    // 
                    (*piDamage) = 0;
                }
            }
            return;
        }
    }

    ASSERT(false)
}


// ****************************************************************
// 
void cShieldManager::ClearShields(const coreBool bAnimated)
{
    // 
    std::memset(m_aiHealth, 0, sizeof(m_aiHealth));

    if(!bAnimated)
    {
        // 
        std::memset(m_afExtent, 0, sizeof(m_afExtent));
        m_ShieldList.Clear();
    }
}


// ****************************************************************
// 
void cShieldManager::BindEnemy(cEnemy* pEnemy, const coreUint8 iElement, const coreInt16 iHealth)
{
    ASSERT(!pEnemy->HasStatus(ENEMY_STATUS_SHIELDED))

    // 
    coreUintW i = 0u;
    for(; i < SHIELD_SHIELDS; ++i)
    {
        if(!m_apOwner[i]) break;
    }
    ASSERT(i < SHIELD_SHIELDS)

    // 
    m_apOwner  [i] = pEnemy;
    m_aiElement[i] = iElement;
    m_aiHealth [i] = iHealth ? iHealth : (pEnemy->GetMaxHealth() >> 1);
    m_afExtent [i] = 1.0f;

    // 
    pEnemy->AddStatus(ENEMY_STATUS_SHIELDED);

    // 
    coreVector3 vColor;
    switch(iElement)
    {
    default: ASSERT(false)
    case ELEMENT_YELLOW: vColor = COLOR_ENERGY_YELLOW; break;
    case ELEMENT_PURPLE: vColor = COLOR_ENERGY_PURPLE; break;
    case ELEMENT_GREEN:  vColor = COLOR_ENERGY_GREEN;  break;
    case ELEMENT_BLUE:   vColor = COLOR_ENERGY_BLUE;   break;
    case ELEMENT_RED:    vColor = COLOR_ENERGY_RED;    break;
    }
    m_aShield[i].SetColor4(coreVector4(vColor, 0.0f));

    // 
    m_ShieldList.BindObject(&m_aShield[i]);
}


// ****************************************************************
// 
void cShieldManager::UnbindEnemy(cEnemy* pEnemy)
{
    if(!pEnemy->HasStatus(ENEMY_STATUS_SHIELDED)) return;

    // 
    for(coreUintW i = 0u; i < SHIELD_SHIELDS; ++i)
    {
        if(m_apOwner[i] == pEnemy)
        {
            // 
            m_apOwner[i] = NULL;
            pEnemy->RemoveStatus(ENEMY_STATUS_SHIELDED);
            return;
        }
    }

    ASSERT(false)
}