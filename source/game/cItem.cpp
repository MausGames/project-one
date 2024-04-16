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
cItem::cItem()noexcept
: m_fAnimation (0.0f)
, m_fChange    (0.0f)
{
    // set initial status
    m_iStatus = ITEM_STATUS_ACTIVE;
}


// ****************************************************************
// 
void cItem::Collect(cPlayer* OUTPUT pPlayer)
{
    // 
    if(!HAS_FLAG(m_iStatus, ITEM_STATUS_ACTIVE)) return;
    REMOVE_FLAG(m_iStatus, ITEM_STATUS_ACTIVE)
    ADD_FLAG   (m_iStatus, ITEM_STATUS_COLLECTED)

    // 
    ASSERT(pPlayer)
    this->__CollectOwn(pPlayer);
}


// ****************************************************************
// 
void cItem::Lose()
{
    // 
    if(!HAS_FLAG(m_iStatus, ITEM_STATUS_ACTIVE)) return;
    REMOVE_FLAG(m_iStatus, ITEM_STATUS_ACTIVE)
    ADD_FLAG   (m_iStatus, ITEM_STATUS_LOST)

    // 
    this->__LoseOwn();
}


// ****************************************************************
// 
void cItem::Destroy(const coreBool bAnimated)
{
    // 
    if(HAS_FLAG(m_iStatus, ITEM_STATUS_FINISHED)) return;
    ADD_FLAG(m_iStatus, ITEM_STATUS_FINISHED)

    // 
    this->__DestroyOwn(bAnimated);
}


// ****************************************************************
// destructor
cItemManager::~cItemManager()
{
    // 
    this->ClearItems(false);
}


// ****************************************************************
// 
void cItemManager::Render()
{
    // 
    FOR_EACH(it, m_apItemList)
        (*it)->Render();
}


// ****************************************************************
// 
void cItemManager::Move()
{
    // 
    FOR_EACH_DYN(it, m_apItemList)
    {
        cItem* pItem = (*it);

        // 
        if(HAS_FLAG(pItem->GetStatus(), ITEM_STATUS_FINISHED))
        {
            SAFE_DELETE(pItem)
            DYN_REMOVE(it, m_apItemList)
        }
        else
        {
            pItem->Move();
            DYN_KEEP(it)
        }
    }
}


// ****************************************************************
// 
void cItemManager::ClearItems(const coreBool bAnimated)
{
    // 
    FOR_EACH(it, m_apItemList)
    {
        cItem* pItem = (*it);

        // 
        pItem->Destroy(bAnimated);
        SAFE_DELETE(pItem)
    }

    // 
    m_apItemList.clear();
}


// ****************************************************************
// 
void cItemManager::LoseItems()
{
    // 
    FOR_EACH(it, m_apItemList)
        (*it)->Lose();
}


// ****************************************************************
// constructor
cFragmentItem::cFragmentItem()noexcept
: m_pTarget (NULL)
{
    // 
    this->DefineModel  ("object_sphere.md3");
    this->DefineTexture(0u, "effect_energy.png");
    this->DefineProgram("effect_energy_program");
    this->SetColor3    (COLOR_ENERGY_WHITE * 0.5f);
    this->SetTexSize   (coreVector2(4.0f,4.0f));

    // 
    m_Hull.DefineModel  ("object_sphere.md3");
    m_Hull.DefineTexture(0u, "effect_energy.png");
    m_Hull.DefineProgram("effect_energy_flat_program");
    m_Hull.SetColor3    (COLOR_ENERGY_WHITE * 0.1f);
    m_Hull.SetTexSize   (coreVector2(4.0f,4.0f));

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aElectron); ++i)
    {
        m_aElectron[i].DefineModel  ("object_sphere.md3");
        m_aElectron[i].DefineTexture(0u, "effect_energy.png");
        m_aElectron[i].DefineProgram("effect_energy_spheric_program");
        m_aElectron[i].SetSize      (coreVector3(1.1f,1.1f,1.1f));
        m_aElectron[i].SetColor3    (COLOR_ENERGY_WHITE * 0.5f);
        m_aElectron[i].SetTexSize   (coreVector2(2.0f,2.0f));
    }
}


// ****************************************************************
// 
void cFragmentItem::Render()
{
    // 
    m_Hull.Render();

    // 
    this->coreObject3D::Render();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aElectron); ++i)
        m_aElectron[i].Render();
}


// ****************************************************************
// 
void cFragmentItem::Move()
{
    // 
    if(HAS_FLAG(m_iStatus, ITEM_STATUS_COLLECTED))
    {
        m_fChange.Update(2.0f);
        if(m_fChange >= 1.0f)
        {
            // 
            ASSERT(m_pTarget)
            g_pSpecialEffects->MacroExplosionDarkBig(m_pTarget->GetPosition());

            // 
            this->Destroy(false);
            return;
        }
    }

    // 
    m_fAnimation.Update(1.0f);

    // 
    const coreFloat afTime[3] =
    {
        (m_fAnimation)               * (2.0f*PI),
        (m_fAnimation + (1.0f/3.0f)) * (2.0f*PI),
        (m_fAnimation + (2.0f/3.0f)) * (2.0f*PI)
    };
    const coreVector3 avOrbit[3] =
    {
        coreVector3(coreVector2( (1.0f),       (0.0f))       * SIN(afTime[0]), COS(afTime[0])),
        coreVector3(coreVector2( (1.0f/SQRT2), (1.0f/SQRT2)) * SIN(afTime[1]), COS(afTime[1])),
        coreVector3(coreVector2(-(1.0f/SQRT2), (1.0f/SQRT2)) * SIN(afTime[2]), COS(afTime[2]))
    };

    // 
    const coreVector3 vTargetPos = m_pTarget ? m_pTarget->GetPosition() : this->GetPosition();
    const coreVector2 vTexOffset = coreVector2(0.0f, m_fAnimation * -0.2f);

    // 
    this->SetSize     (coreVector3(2.0f,2.0f,2.0f) * MAX(1.0f - m_fChange * 4.0f, 0.0f));
    this->SetTexOffset(vTexOffset);
    this->coreObject3D::Move();

    // 
    m_Hull.SetPosition (this->GetPosition());
    m_Hull.SetSize     (coreVector3(2.8f,2.8f,2.8f) * MAX(1.0f - m_fChange * 2.0f, 0.0f));
    m_Hull.SetTexOffset(-vTexOffset);
    m_Hull.Move();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aElectron); ++i)
    {
        m_aElectron[i].SetPosition (LERP(this->GetPosition() + avOrbit[i] * (coreVector3(7.0f,7.0f,7.0f) + coreVector3(coreVector2(70.0f,70.0f) * m_fChange, 0.0f)), vTargetPos, m_fChange));
        m_aElectron[i].SetTexOffset(vTexOffset);
        m_aElectron[i].Move();
    }
}


// ****************************************************************
// 
void cFragmentItem::__CollectOwn(cPlayer* OUTPUT pPlayer)
{
    // 
    m_pTarget = pPlayer;
}


// ****************************************************************
// 
void cFragmentItem::__LoseOwn()
{
    // 
    this->Destroy(true);
}


// ****************************************************************
// 
void cFragmentItem::__DestroyOwn(const coreBool bAnimated)
{
    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionDarkSmall(this->GetPosition());
}