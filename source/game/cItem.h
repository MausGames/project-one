///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ITEM_H_
#define _P1_GUARD_ITEM_H_

// TODO 4: lifetime should be handled in base class


// ****************************************************************
// item definitions
enum eItemStatus : coreUint8
{
    ITEM_STATUS_ACTIVE    = 0x01u,   // 
    ITEM_STATUS_COLLECTED = 0x02u,   // 
    ITEM_STATUS_LOST      = 0x04u,   // 
    ITEM_STATUS_FINISHED  = 0x08u    // 
};


// ****************************************************************
// item interface
class INTERFACE cItem : public coreObject3D
{
protected:
    coreFlow m_fLifeTime;    // 


public:
    cItem()noexcept;
    virtual ~cItem()override = default;

    FRIEND_CLASS(cItemManager)
    DISABLE_COPY(cItem)
    ENABLE_ID

    // 
    void Collect(cPlayer* OUTPUT pPlayer);
    void Lose   ();
    void Destroy(const coreBool bAnimated);

    // 
    inline const coreFloat& GetLifeTime()const {return m_fLifeTime;}


private:
    // own routines for derived classes
    virtual void __CollectOwn(cPlayer* OUTPUT pPlayer)  {}
    virtual void __LoseOwn   ()                         {}
    virtual void __DestroyOwn(const coreBool bAnimated) {}
};


// ****************************************************************
// item manager class
class cItemManager final
{
private:
    coreList<cItem*> m_apItemList;   // 


public:
    cItemManager() = default;
    ~cItemManager();

    DISABLE_COPY(cItemManager)

    // render and move the item manager
    void Render();
    void Move();

    // add and remove items
    template <typename T, typename... A> RETURN_RESTRICT T* AddItem(const coreVector2 vPosition, A&&... vArgs);
    template <typename T, typename... A> RETURN_RESTRICT T* AddItem(const coreVector3 vPosition, A&&... vArgs);
    void ClearItems(const coreBool bAnimated);

    // 
    void LoseItems();

    // 
    inline coreUintW GetNumItems      ()const {return m_apItemList.size();}
    inline coreUintW GetNumItemsActive()const {coreUintW iNum = 0u; FOR_EACH(it, m_apItemList) if(!HAS_FLAG((*it)->GetStatus(), ITEM_STATUS_FINISHED)) ++iNum; return iNum;}


private:
    // 
    static coreVector2 __Project3D(const coreVector3 vPosition);
};


// ****************************************************************
// fragment item class
class cFragmentItem final : public cItem
{
private:
    coreObject3D m_aHull[2];           // 
    coreObject3D m_aaElectron[3][2];   // 

    cPlayer*  m_pTarget;               // 

    coreUint8 m_iType;                 // 
    coreUint8 m_iMissionIndex;         // 
    coreUint8 m_iBossIndex;            // 

    coreFlow m_fAnimation;             // animation value
    coreFlow m_fChange;                // 


public:
    cFragmentItem(const coreUint8 iType, const coreUint8 iMissionIndex, const coreUint8 iBossIndex)noexcept;

    DISABLE_COPY(cFragmentItem)
    ASSIGN_ID(1, "Fragment")

    // 
    void Render()final;
    void Move  ()final;


private:
    // execute own routines
    void __CollectOwn(cPlayer* OUTPUT pPlayer)final;
    void __LoseOwn   ()final;
    void __DestroyOwn(const coreBool bAnimated)final;
};


// ****************************************************************
// 
template <typename T, typename... A> RETURN_RESTRICT T* cItemManager::AddItem(const coreVector2 vPosition, A&&... vArgs)
{
    // 
    const coreVector2 vClampPos = coreVector2(CLAMP(vPosition.x, -FOREGROUND_AREA.x, FOREGROUND_AREA.x),
                                              CLAMP(vPosition.y, -FOREGROUND_AREA.y, FOREGROUND_AREA.y));

    // 
    T* pNewItem = new T(std::forward<A>(vArgs)...);
    pNewItem->SetPosition(coreVector3(vClampPos, 0.0f));
    pNewItem->ChangeType (TYPE_ITEM);

    // 
    const auto it = std::find_if    (m_apItemList.begin(), m_apItemList.end(), [](const cItem* pItem) {return (pItem->GetID() == T::ID);});
    const auto et = std::find_if_not(it,                   m_apItemList.end(), [](const cItem* pItem) {return (pItem->GetID() == T::ID);});
    m_apItemList.insert(et, pNewItem);

    return pNewItem;
}

template <typename T, typename... A> RETURN_RESTRICT T* cItemManager::AddItem(const coreVector3 vPosition, A&&... vArgs)
{
    // 
    return this->AddItem<T, A...>(cItemManager::__Project3D(vPosition), std::forward<A>(vArgs)...);
}


#endif // _P1_GUARD_ITEM_H_