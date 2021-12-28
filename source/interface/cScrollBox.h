///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SCROLLBOX_H_
#define _P1_GUARD_SCROLLBOX_H_


// ****************************************************************
// 
#define SCROLL_WIDTH (0.05f)   // 
#define SCROLL_DELAY (-2.0f)   // 


// ****************************************************************
// 
class cScrollBox final : public coreViewBox
{
private:
    cGuiButton m_aArrow[2];   // 
    cGuiButton m_Cursor;      // 

    coreFloat m_fMaxOffset;   // 

    coreBool  m_bDrag;        // 
    coreFloat m_fDragValue;   // 

    coreTimer m_Automatic;    // 


public:
    cScrollBox()noexcept;

    DISABLE_COPY(cScrollBox)

    // 
    void Render()final;
    void Move  ()final;

    // 
    inline void SetMaxOffset(const coreFloat fMaxOffset) {m_fMaxOffset = fMaxOffset;}
};


#endif // _P1_GUARD_SCROLLBOX_H_