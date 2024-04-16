///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MENUNAVIGATOR_H_
#define _P1_GUARD_MENUNAVIGATOR_H_

// TODO 4: cleanup, even header members and functions
// TODO 1: add button descriptions (at bottom) and for tabs
// TODO 1: improve cursor graphics
// TODO 3: allow selecting the current tab when going below bottom, instead of skipping it
// TODO 1: allow hiding cursor (e.g. for world-map)
// TODO 1: correctly handle button-remapping feature


// ****************************************************************
// 
enum eMenuType : coreUint8
{
    MENU_TYPE_DEFAULT      = 0x00u,
    MENU_TYPE_TAB_ROOT     = 0x04u,
    MENU_TYPE_TAB_NODE     = 0x08u,
    MENU_TYPE_SWITCH_MOVE  = 0x01u,
    MENU_TYPE_SWITCH_PRESS = 0x02u,
    MENU_TYPE_AUTO_CLICK   = 0x10u
};
ENABLE_BITWISE(eMenuType)


// ****************************************************************
// 
class cMenuNavigator final : public coreObject2D
{
private:
    // 
    struct sMenuEntry final
    {
        coreUint8 iMoveUp;         // 
        coreUint8 iMoveLeft;       // 
        coreUint8 iMoveDown;       // 
        coreUint8 iMoveRight;      // 
        coreUint8 iMoveFallback;   // 
        eMenuType eType;           // 
    };

    struct sMenuTab final
    {
        coreUint8 iSurface;   // 
    };


private:
    coreMap<coreObject2D*, sMenuEntry> m_aObject;   // 
    coreMap<coreObject2D*, sMenuTab> m_aTab;        // 

    coreObject2D* m_pCurObject;                     // 
    coreUint8 m_iStore;                             // 
    coreUint8 m_iFirst;                             // 

    coreBool m_bPressed;                            // 
    coreList<coreUint8> m_aiLock;                   // (0 = all | 1 = move) 

    coreVector2 m_vMouseOffset;                     // 

    coreMenu* m_pMenu;                              // 

    static coreVector2 s_vMouseMove;                // 
    static coreBool    s_bJoystick;                 // 


public:
    explicit cMenuNavigator(coreMenu* pMenu)noexcept;

    DISABLE_COPY(cMenuNavigator)

    // 
    void Move()final;

    // 
    void Update();

    // 
    void BindObject(coreObject2D* pObject, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight, coreObject2D* pFallback, const eMenuType eType);

    // 
    inline void AssignSurface(coreObject2D* pTab, const coreUint8 iSurface) {ASSERT(m_aTab   .count(pTab))    m_aTab.at(pTab).iSurface = iSurface;}
    inline void AssignFirst  (coreObject2D* pObject)                        {ASSERT(m_aObject.count(pObject)) m_iFirst = this->__ToIndex(pObject);}
    inline void ResetFirst   ()                                             {m_pCurObject = NULL; m_iStore = 0u;}

    // 
    inline void SetCurrent(coreObject2D* pObject) {ASSERT(m_aObject.count(pObject)) m_pCurObject = pObject;}

    // 
    static void GlobalUpdate();


private:
    // 
    inline coreUint8     __ToIndex (coreObject2D*   pObject)const {ASSERT(m_aObject.count(pObject))  return m_aObject.index(pObject);}
    inline coreObject2D* __ToObject(const coreUint8 iIndex)const  {ASSERT(iIndex < m_aObject.size()) return m_aObject.get_keylist().at(iIndex);}
};


#endif // _P1_GUARD_MENUNAVIGATOR_H_