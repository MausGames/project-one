///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_INK_H_
#define _P1_GUARD_INK_H_

// TODO 3: ships and objects moving in and out of ink should draw some clouds
// TODO 3: ink needs different color in point and line rendering (red?, blue?)
// TODO 3: ink flows away
// TODO 3: also create string-lists for u_av4Offset and u_av2Offset, like in background


// ****************************************************************
// ink definitions
#define INK_SAMPLES_POINT (3u)    // 
#define INK_SAMPLES_LINE  (10u)   // 


// ****************************************************************
// ink class
class cInk final : public coreFullscreen
{
private:
    coreVector4 m_vBlotPosition;   // 
    coreVector2 m_vBlotAlpha;      // 

    coreFlow  m_fAnimation;        // 
    coreFloat m_fDelay;            // 


public:
    cInk()noexcept;

    DISABLE_COPY(cInk)

    // render and move the ink
    void Render()final;
    void Move  ()final;

    // 
    void Enable (const coreBool  bLine);
    void Disable(const coreFloat fDelay);

    // 
    void SetBlotPosition(const coreUintW iIndex, const coreVector2 vPosition);
    void SetBlotAlpha   (const coreUintW iIndex, const coreFloat   fAlpha);

    // 
    inline coreBool IsActive()const {return !m_vBlotAlpha.IsNull();}
};


#endif // _P1_GUARD_INK_H_