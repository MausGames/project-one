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
    this->DefineModel  ("fragment_01.md3");
    this->DefineTexture(0u, "default_white.png");
    this->DefineProgram("effect_energy_program");
    this->SetColor3    (COLOR_ENERGY_WHITE * 0.05f);
    this->SetTexSize   (coreVector2(4.0f,4.0f));

    // 
    m_aHull[0].DefineModel  ("object_sphere.md3");
    m_aHull[0].DefineTexture(0u, "effect_energy.png");
    m_aHull[0].DefineProgram("effect_energy_flat_program");
    m_aHull[0].SetColor3    (COLOR_ENERGY_WHITE * 0.1f);
    m_aHull[0].SetTexSize   (coreVector2(4.0f,4.0f));

    // 
    m_aHull[1].DefineModel  ("object_sphere.md3");
    m_aHull[1].DefineTexture(0u, "effect_energy.png");
    m_aHull[1].DefineProgram("effect_energy_flat_spheric_program");
    m_aHull[1].SetColor3    (COLOR_ENERGY_WHITE * 0.5f);
    m_aHull[1].SetTexSize   (coreVector2(4.0f,4.0f));

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aaElectron); ++i)
    {
        m_aaElectron[i][0].DefineModel  ("object_sphere.md3");
        m_aaElectron[i][0].DefineTexture(0u, "effect_energy.png");
        m_aaElectron[i][0].DefineProgram("effect_energy_flat_program");
        m_aaElectron[i][0].SetColor3    (COLOR_ENERGY_WHITE * 0.1f);
        m_aaElectron[i][0].SetTexSize   (coreVector2(2.0f,2.0f));

        m_aaElectron[i][1].DefineModel  ("object_sphere.md3");
        m_aaElectron[i][1].DefineTexture(0u, "effect_energy.png");
        m_aaElectron[i][1].DefineProgram("effect_energy_flat_spheric_program");
        m_aaElectron[i][1].SetColor3    (COLOR_ENERGY_WHITE * 0.5f);
        m_aaElectron[i][1].SetTexSize   (coreVector2(2.0f,2.0f));
    }
}


// ****************************************************************
// 
void cFragmentItem::Render()
{
    glDisable(GL_DEPTH_TEST);
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aaElectron); ++i)
    {
        if(m_aaElectron[i][0].GetPosition().z < 0.0f) m_aaElectron[i][0].Render();
    }
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aaElectron); ++i)
    {
        if(m_aaElectron[i][1].GetPosition().z < 0.0f) m_aaElectron[i][1].Render();
    }

    // 
    m_aHull[0].Render();
    m_aHull[1].Render();
    
    
    glEnable(GL_DEPTH_TEST);

    // 
    this->coreObject3D::Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_LIGHT)->ApplyObject(this);
    
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aaElectron); ++i)
    {
        if(m_aaElectron[i][0].GetPosition().z >= 0.0f) m_aaElectron[i][0].Render();
    }
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aaElectron); ++i)
    {
        if(m_aaElectron[i][1].GetPosition().z >= 0.0f) m_aaElectron[i][1].Render();
    }
}


// ****************************************************************
// 
void cFragmentItem::Move()
{
    // 
    if(HAS_FLAG(m_iStatus, ITEM_STATUS_COLLECTED))
    {
        m_fChange.Update(1.0f);
        if(m_fChange >= 1.0f)
        {
            ASSERT(m_pTarget)

            // 
            g_pSpecialEffects->MacroExplosionDarkBig(m_pTarget->GetPosition());
            g_pSpecialEffects->PlaySound(m_pTarget->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);

            // 
            m_pTarget->SetDesaturate(PLAYER_DESATURATE_FRAGMENT);

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

    const coreFloat fScale = 3.0f;

    // 
    const coreVector3 vTargetPos = m_pTarget ? m_pTarget->GetPosition() : this->GetPosition();
    const coreVector2 vTexOffset = coreVector2(0.0f, m_fAnimation * -0.2f);
    
    
    const coreVector2 vDir = coreVector2::Direction(m_fAnimation);

    // 
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f) * fScale * MAX(1.0f - m_fChange * 4.0f, 0.0f));
    this->SetDirection  (coreVector3(vDir, 0.0f));
    this->SetOrientation(OriRoundDir(vDir, vDir));
    //this->SetTexOffset  (vTexOffset);
    this->SetEnabled    (this->GetSize().x ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    this->coreObject3D::Move();

    // 
    m_aHull[0].SetPosition (this->GetPosition());
    m_aHull[0].SetSize     (coreVector3(2.0f,2.0f,2.0f) * fScale * MAX(1.0f - m_fChange * 2.0f, 0.0f));
    m_aHull[0].SetTexOffset(-vTexOffset);
    m_aHull[0].SetEnabled  (m_aHull[0].GetSize().x ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_aHull[0].Move();

    // 
    m_aHull[1].SetPosition (this->GetPosition());
    m_aHull[1].SetSize     (coreVector3(1.6f,1.6f,1.6f) * fScale * MAX(1.0f - m_fChange * 2.0f, 0.0f));
    m_aHull[1].SetTexOffset(vTexOffset);
    m_aHull[1].SetEnabled  (m_aHull[1].GetSize().x ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_aHull[1].Move();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aaElectron); ++i)
    {
        const coreVector3 vPos = LERP(this->GetPosition() + avOrbit[i] * (coreVector3(3.5f,3.5f,3.5f) * fScale + coreVector3(coreVector2(35.0f,35.0f) * fScale * m_fChange, 0.0f)), vTargetPos, m_fChange);
        const coreVector2 vTex = vTexOffset + coreVector2(0.0f, I_TO_F(i + 1u) * 0.25f);

        m_aaElectron[i][0].SetPosition (vPos);
        m_aaElectron[i][0].SetSize     (coreVector3(0.65f,0.65f,0.65f) * fScale * (2.2f/1.6f));
        m_aaElectron[i][0].SetTexOffset(-vTex);
        m_aaElectron[i][0].Move();

        m_aaElectron[i][1].SetPosition (vPos);
        m_aaElectron[i][1].SetSize     (coreVector3(0.65f,0.65f,0.65f) * fScale);
        m_aaElectron[i][1].SetTexOffset(vTex);
        m_aaElectron[i][1].Move();
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