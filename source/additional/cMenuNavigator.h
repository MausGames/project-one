///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MENUNAVIGATOR_H_
#define _P1_GUARD_MENUNAVIGATOR_H_

// TODO 4: cleanup, even header members and functions
// TODO 3: allow selecting the current tab when going below bottom, instead of skipping it
// TODO 1: allow hiding cursor (e.g. for world-map)
// TODO 1: correctly handle button-remapping feature
// TODO 3: when changing input for keyboard+mouse, you can use the joystick to map the left mouse-button


// ****************************************************************
// 
#define MENUNAVIGATOR_IGNORE_MOUSE (coreVector2(3.0f,3.0f))   // 
#define MENUNAVIGATOR_INVALID      (0xFFu)                    // 
#define MENUNAVIGATOR_PROMPTS      (3u)                       // 

enum eMenuType : coreUint8
{
    MENU_TYPE_DEFAULT      = 0x00u,
    MENU_TYPE_TAB_ROOT     = 0x04u,
    MENU_TYPE_TAB_NODE     = 0x08u,
    MENU_TYPE_SWITCH_MOVE  = 0x01u,
    MENU_TYPE_SWITCH_PRESS = 0x02u,
    MENU_TYPE_AUTO_CLICK   = 0x10u,
    MENU_TYPE_BIG          = 0x20u
};
ENABLE_BITWISE(eMenuType)


// ****************************************************************
// 
class cMenuNavigator final : public cGuiObject
{
private:
    // 
    struct sMenuEntry final
    {
        coreUint8 iMoveUp;      // 
        coreUint8 iMoveLeft;    // 
        coreUint8 iMoveDown;    // 
        coreUint8 iMoveRight;   // 
        eMenuType eType;        // 
        coreUint8 iSurface;     // 
    };

    // 
    struct sMenuTab final
    {
        coreUint8 iSurface;     // 
        coreUint8 iFallUp;      // 
        coreUint8 iFallLeft;    // 
        coreUint8 iFallDown;    // 
        coreUint8 iFallRight;   // 
        coreUint8 iLastEntry;   // 
    };

    // 
    using uShoulderType = std::function<void()>;


private:
    coreMap<coreObject2D*, sMenuEntry> m_aObject;    // 
    coreMap<coreObject2D*, sMenuTab>   m_aTab;       // 
    coreSet<coreObject2D*>             m_apScroll;   // 

    coreObject2D* m_pCurObject;                      // 
    coreUint8 m_iStore;                              // 
    coreUint8 m_iFirst;                              // 
    coreUint8 m_iBack;                               // 

    coreBool m_bPressed;                             // 
    coreBool m_bGrabbed;                             // 
    coreFlow m_fGrabTime;                            // 
    coreList<coreUint8> m_aiLock;                    // 
    coreList<coreUint8> m_aiLastPack;                // 

    coreList<coreTimer> m_aAutomatic;                // 

    coreVector2 m_vMouseOffset;                      // 

    coreVector2 m_vCurPos;                           // 
    coreVector2 m_vCurSize;                          // 
    coreBool    m_bShowIcon;                         // 

    cGuiObject m_aCursor[4];                         // 

    coreMenu* m_pMenu;                               // 

    uShoulderType m_nShoulderLeft;                   // 
    uShoulderType m_nShoulderRight;                  // 
    coreBool      m_bShoulder;                       // 

    cFigure m_aPrompt[MENUNAVIGATOR_PROMPTS];        // 

    static coreVector2 s_vMouseMove;                 // 
    static coreBool    s_bJoystick;                  // 
    static coreUint8   s_iJoystickType;              // 


public:
    cMenuNavigator()noexcept;

    DISABLE_COPY(cMenuNavigator)

    // 
    void Render()final;
    void Move()final;

    // 
    void Update();

    // 
    void BindObject (coreObject2D* pObject, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight, const eMenuType eType, const coreUint8 iSurface = 0u);
    void BindSurface(coreObject2D* pTab, const coreUint8 iSurface, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight);
    void BindScroll (coreObject2D* pScroll);

    // 
    inline void AssignMenu (coreMenu*     pMenu)   {ASSERT(pMenu)                    m_pMenu  = pMenu;}
    inline void AssignFirst(coreObject2D* pObject) {ASSERT(m_aObject.count(pObject)) m_iFirst = pObject ? this->__ToIndex(pObject) : MENUNAVIGATOR_INVALID;}
    inline void AssignBack (coreObject2D* pObject) {ASSERT(m_aObject.count(pObject)) m_iBack  = pObject ? this->__ToIndex(pObject) : MENUNAVIGATOR_INVALID;}
    inline void ResetFirst ()                      {m_pCurObject = NULL; m_iStore = MENUNAVIGATOR_INVALID; m_vCurPos = HIDDEN_POS; m_vCurSize = coreVector2(0.0f,0.0f);}

    // 
    template <typename F> inline void UseShoulderLeft (F&& nShoulderFunc) {ASSERT(!m_nShoulderLeft)  m_nShoulderLeft  = nShoulderFunc;}   // [](void) -> void
    template <typename F> inline void UseShoulderRight(F&& nShoulderFunc) {ASSERT(!m_nShoulderRight) m_nShoulderRight = nShoulderFunc;}   // [](void) -> void
    inline void SetShoulder(const coreBool bShoulder) {m_bShoulder = bShoulder;}

    // 
    inline void ShowIcon(const coreBool bStatus) {m_bShowIcon = bStatus;}

    // 
    inline void OverrideCurrent(coreObject2D* pObject) {ASSERT(m_aObject.count(pObject)) m_pCurObject = pObject;}
    inline void ForceCurrent   (coreObject2D* pObject) {this->OverrideCurrent(pObject); m_vCurPos = HIDDEN_POS; m_vCurSize = coreVector2(0.0f,0.0f);}

    // 
    static void GlobalInit();
    static void GlobalUpdate();

    // 
    static inline const coreBool& IsUsingJoystick() {return s_bJoystick;}

    // 
    static inline coreBool IsValid(const coreObject2D* pObject) {ASSERT(pObject) return (pObject->GetAlpha() && pObject->IsEnabled(CORE_OBJECT_ENABLE_ALL));}


private:
    // 
    inline coreUint8     __ToIndex (coreObject2D*   pObject)const {ASSERT(m_aObject.count(pObject))  return m_aObject.index(pObject);}
    inline coreObject2D* __ToObject(const coreUint8 iIndex)const  {ASSERT(iIndex < m_aObject.size()) return m_aObject.get_keylist().at(iIndex);}
};


#endif // _P1_GUARD_MENUNAVIGATOR_H_