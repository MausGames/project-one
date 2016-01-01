//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TOOLTIP_H_
#define _P1_GUARD_TOOLTIP_H_

// TODO: transfer to Core Engine
// TODO: does not handle Kanji & Kana correctly
// TODO: does not handle kerning (relevant ?)
// TODO: (this + __LINE__ * 100u) may not handle implementation-split between cpp and h file


// ****************************************************************
// tooltip definitions
#define TOOLTIP_LINES        (8u)                                         // number of available text lines
#define TOOLTIP_LINE_HEIGHT  (0.022f)                                     // spacing between to text lines
#define TOOLTIP_OUTLINE_SIZE (1u)                                         // default text outline size
#define TOOLTIP_BORDER_SIZE  (coreVector2(0.02f, 0.016f))                 // spacing between text and box-edge
#define TOOLTIP_MOUSE_OFFSET (coreVector2(0.03f,-0.03f))                  // position offset from mouse cursor (and for direction/alignment)

#define TOOLTIP_PRINT(f,...) (this + __LINE__ * 100u), f, ##__VA_ARGS__   // specialized print-function for ShowText()
#define TOOLTIP_ONELINER     (FLT_MAX)                                    // create tooltip without wrapping (infinite width)


// ****************************************************************
// tooltip class
class cTooltip final : public coreObject2D
{
private:
    coreLabel m_aLine[TOOLTIP_LINES];   // text lines

    coreUintW m_iNumLines;              // currently active text lines
    coreBool  m_bDisplay;               // display the tooltip (once)

    const void* m_pLastRef;             // last string reference-pointer (dirty check)


public:
    cTooltip()noexcept;

    DISABLE_COPY(cTooltip)

    // render and move the tooltip
    void Render()override;
    void Move  ()override;

    // 
    template <typename... A> void ShowText(const coreFloat fWidth, const void* pRef, const coreChar* pcFormat, A&&... vArgs);
    inline void                   ShowText(const coreFloat fWidth, const coreChar* pcText);

    // force update on next display
    inline void Invalidate() {m_pLastRef = NULL;}


private:
    // 
    void __ShowText(const coreFloat fWidth, const coreChar* pcText);
};


// ****************************************************************
// 
template <typename... A> void cTooltip::ShowText(const coreFloat fWidth, const void* pRef, const coreChar* pcFormat, A&&... vArgs)
{
    // show tooltip at current mouse position
    m_bDisplay = true;
    this->SetCenter(Core::Input->GetMousePosition());

    // check and save last reference-pointer (to prevent redundant update)
    if(m_pLastRef == pRef) return;
    m_pLastRef = pRef;

    // forward copy of formatted string
    this->__ShowText(fWidth, PRINT(pcFormat, std::forward<A>(vArgs)...));
}

inline void cTooltip::ShowText(const coreFloat fWidth, const coreChar* pcText)
{
    // forward copy of trivial string
    this->ShowText(fWidth, pcText, "%s", pcText);
}


#endif // _P1_GUARD_TOOLTIP_H_