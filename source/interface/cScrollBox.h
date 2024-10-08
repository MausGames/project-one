///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SCROLLBOX_H_
#define _P1_GUARD_SCROLLBOX_H_

// TODO 1: add animations to show the player that scrolling is possible (pulsating arrows ?)


// ****************************************************************
// 
#define SCROLL_WIDTH      (0.05f)   // 
#define SCROLL_DELAY      (-2.0f)   // 
#define SCROLL_FOCUS_SIZE (0.35f)   // 


// ****************************************************************
// 
class cScrollBox final : public coreViewBox
{
private:
    cGuiButton m_aArrow[2];   // 
    cGuiButton m_Cursor;      // 

    coreFloat m_fCurOffset;   // 
    coreFloat m_fMaxOffset;   // 

    coreBool  m_bDrag;        // 
    coreFloat m_fDragValue;   // 

    coreFlow m_fSpeed;        // 
    coreBool m_bInside;       // 

    coreTimer m_Automatic;    // 


public:
    cScrollBox()noexcept;

    DISABLE_COPY(cScrollBox)

    // 
    void Render()final;
    void Move  ()final;

    // 
    void ScrollToObject(const coreObject2D* pObject, const coreBool bNow);

    // 
    inline coreBool IsBarFocused()const {return (m_aArrow[0].IsFocused() || m_aArrow[1].IsFocused() || m_Cursor.IsFocused());}

    // 
    inline void SetCurOffset(const coreFloat fCurOffset) {m_fCurOffset = fCurOffset; this->SetOffset(coreVector2(0.0f, m_fCurOffset));}
    inline void SetMaxOffset(const coreFloat fMaxOffset) {m_fMaxOffset = fMaxOffset;}
    inline void SetInside   (const coreBool  bInside)    {m_bInside    = bInside;}
};


#endif // _P1_GUARD_SCROLLBOX_H_