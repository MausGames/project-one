///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MENUINPUT_H_
#define _P1_GUARD_MENUINPUT_H_

// TODO: cGui


// ****************************************************************
// 
class cMenuInput final : public coreObject2D
{
private:
    coreSet<coreObject2D*> m_apObject;   // 
    coreLookup<coreUint8, coreObject2D*> m_aiShoulder;   // 

    coreObject2D* m_pCurObject;          // 
    coreObject2D* m_pCurTab;

    coreBool m_bMoved;  

    coreMenu* m_pMenu;

    static coreBool s_bJoystick;


public:
    cMenuInput()noexcept;

    DISABLE_COPY(cMenuInput)

    // 
    void Move()final;

    inline void BindMenu(coreMenu* pMenu) {m_pMenu = pMenu;}

    // 
    inline void BindObject  (coreObject2D* pObject) {ASSERT(!m_apObject  .count(pObject)) m_apObject  .insert(pObject);}
    inline void BindShoulder(const coreUint8 iSurface, coreObject2D* pObject) {ASSERT(!m_aiShoulder.count(iSurface)) m_aiShoulder.emplace(iSurface, pObject);}
    // TODO: _bs ?   

    static inline const coreBool& GetJoystick() {return s_bJoystick;}
    static void OnlyWithJoystick(coreObject2D* OUTPUT pObject)
    {
        pObject->SetEnabled(s_bJoystick ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
};


#endif // _P1_GUARD_MENUINPUT_H_