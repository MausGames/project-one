///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_FIGURE_H_
#define _P1_GUARD_FIGURE_H_

// TODO 3: arcade stick texture ?


// ****************************************************************
// 
#define FIGURE_BASE_KEYBOARD  (0xFFu)                                       // 
#define FIGURE_INVALID        (coreVector2(-1.0f,-1.0f))                    // 
#define FIGURE_SCALE_KEYBOARD (coreVector2(1.0f,1.0f) * (100.0f/1024.0f))   // 
#define FIGURE_SCALE_GAMEPAD  (coreVector2(0.125f,0.25f))                   // 
#define FIGURE_KEY_LEFTSTICK  (SDL_CONTROLLER_BUTTON_MAX + 10u)             // 
#define FIGURE_KEY_RIGHTSTICK (SDL_CONTROLLER_BUTTON_MAX + 11u)             // 


// ****************************************************************
// 
class cFigure final : public cGuiObject
{
private:
    coreUint8 m_iBase;                              // 
    coreUint8 m_iBaseRef;                           // 
    coreInt16 m_iKey;                               // 
    coreInt16 m_iKeyRef;                            // 

    cGuiLabel m_Fallback;                           // 

    static coreList<coreTexturePtr> s_apTexCache;   // 
    static coreList<coreUint8>      s_apTexBase;    // 


public:
    cFigure()noexcept;

    DISABLE_COPY(cFigure)

    // 
    void Render()final;
    void Move  ()final;

    // 
    inline void SetBase(const coreUint8 iBase) {m_iBase = iBase;}
    inline void SetKey (const coreInt16 iKey)  {m_iKey  = iKey;}

    // 
    void SetBaseAsType(const coreUint8 iType);

    // 
    static void GlobalInit();
    static void GlobalExit();
    static void GlobalUpdate();

    // 
    static coreHashString DetermineTexture  (const coreUint8 iBase);
    static coreVector2    DetermineTexOffset(const coreUint8 iBase, const coreInt16 iKey);
};


#endif // _P1_GUARD_FIGURE_H_