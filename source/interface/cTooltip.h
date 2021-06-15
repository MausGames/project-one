///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TOOLTIP_H_
#define _P1_GUARD_TOOLTIP_H_

// TODO 1: transfer to Core Engine
// TODO 3: does not handle kerning (relevant ?)
// TODO 3: (this + __LINE__) may not handle implementation-split between cpp and h file
// TODO 3: pre-resolve coreFontPtr (GetResource)


// ****************************************************************
// tooltip definitions
#define TOOLTIP_LINES         (4u)                                  // number of available text lines
#define TOOLTIP_LINE_HEIGHT   (0.026f)                              // spacing between two text lines
#define TOOLTIP_LINE_BOTTOM   (0.022f)                              // 
#define TOOLTIP_OUTLINE_SIZE  (MENU_OUTLINE_SMALL)                  // default text outline size
#define TOOLTIP_BORDER_SIZE   (coreVector2(0.02f,0.016f))           // spacing between text and box-edge
#define TOOLTIP_TARGET_OFFSET (coreVector2(0.04f,0.0f))             // position offset from target (and for direction/alignment)

#define TOOLTIP_PRINT(f,...)  (this + __LINE__), f, ##__VA_ARGS__   // specialized print-function for ShowText()
#define TOOLTIP_ONELINER      (FLT_MAX)                             // create tooltip without wrapping (infinite width)

#define TOOLTIP_MOUSE         (Core::Input->GetMousePosition() * Core::System->GetResolution() * RCP(Core::System->GetResolution().Min()))
#define TOOLTIP_OBJECT(x)     (coreVector2((x).GetTransform()._31, (x).GetTransform()._32) * RCP(Core::System->GetResolution().Min()))


// ****************************************************************
// tooltip class
class cTooltip final : public cGuiObject
{
private:
    cGuiLabel m_aLine[TOOLTIP_LINES];   // text lines

    coreUintW m_iNumLines;              // currently active text lines
    coreBool  m_bDisplay;               // display the tooltip (once)

    const void* m_pLastRef;             // last string reference-pointer (dirty check)


public:
    cTooltip()noexcept;

    DISABLE_COPY(cTooltip)

    // render and move the tooltip
    void Render()final;
    void Move  ()final;

    // 
    template <typename... A> void ShowText(const coreVector2& vTarget, const coreFloat fWidth, const void* pRef, const coreChar* pcFormat, A&&... vArgs);
    inline void                   ShowText(const coreVector2& vTarget, const coreFloat fWidth, const coreChar* pcText);

    // 
    inline void UseColor(const coreUintW iIndex, const coreVector3& vColor) {ASSERT(iIndex < TOOLTIP_LINES) m_aLine[iIndex].SetColor3(vColor);}

    // force update on next display
    inline void Invalidate() {m_pLastRef = NULL;}


private:
    // 
    void __ShowText(const coreFloat fWidth, const coreChar* pcText);
};


// ****************************************************************
// 
template <typename... A> void cTooltip::ShowText(const coreVector2& vTarget, const coreFloat fWidth, const void* pRef, const coreChar* pcFormat, A&&... vArgs)
{
    // show tooltip at target position
    m_bDisplay = true;
    this->SetCenter(MapToAxis(vTarget, g_vHudDirection));

    // check and save last reference-pointer (to prevent redundant update)
    if(m_pLastRef == pRef) return;
    m_pLastRef = pRef;

    // forward copy of formatted string
    this->__ShowText(fWidth, PRINT(pcFormat, std::forward<A>(vArgs)...));
}

inline void cTooltip::ShowText(const coreVector2& vTarget, const coreFloat fWidth, const coreChar* pcText)
{
    // forward copy of trivial string
    this->ShowText(vTarget, fWidth, pcText, "%s", pcText);
}


#endif // _P1_GUARD_TOOLTIP_H_