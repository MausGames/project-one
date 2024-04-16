///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ITEM_H_
#define _P1_GUARD_ITEM_H_


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
    coreFlow m_fAnimation;   // animation value
    coreFlow m_fChange;      // 


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
    std::vector<cItem*> m_apItemList;   // 


public:
    cItemManager() = default;
    ~cItemManager();

    DISABLE_COPY(cItemManager)

    // render and move the item manager
    void Render();
    void Move();

    // add and remove items
    template <typename T> RETURN_RESTRICT T* AddItem(const coreVector2& vPosition);
    void ClearItems(const coreBool bAnimated);

    // 
    void LoseItems();
};


// ****************************************************************
// fragment item class
class cFragmentItem final : public cItem
{
private:
    coreObject3D m_Hull;           // 
    coreObject3D m_aElectron[3];   // 

    cPlayer* m_pTarget;            // 


public:
    cFragmentItem()noexcept;

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
// TODO: ineinander verschachtelter rotierender wuerfel  
class cCubeItem final : public cItem
{
private:
    coreObject3D m_Hull;   // 


public:
    cCubeItem()noexcept;

    DISABLE_COPY(cCubeItem)
    ASSIGN_ID(2, "Cube")

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
template <typename T> RETURN_RESTRICT T* cItemManager::AddItem(const coreVector2& vPosition)
{
    // 
    const coreVector2 vClampPos = coreVector2(CLAMP(vPosition.x, -FOREGROUND_AREA.x, FOREGROUND_AREA.x),
                                              CLAMP(vPosition.y, -FOREGROUND_AREA.y, FOREGROUND_AREA.y));

    // 
    T* pNewItem = new T();
    pNewItem->SetPosition(coreVector3(vClampPos, 0.0f));
    pNewItem->ChangeType (TYPE_ITEM);

    // 
    const auto it = std::find_if    (m_apItemList.begin(), m_apItemList.end(), [](const cItem* pItem) {return (pItem->GetID() == T::ID);});
    const auto et = std::find_if_not(it,                   m_apItemList.end(), [](const cItem* pItem) {return (pItem->GetID() == T::ID);});
    m_apItemList.insert(et, pNewItem);

    return pNewItem;
}


#endif // _P1_GUARD_ITEM_H_