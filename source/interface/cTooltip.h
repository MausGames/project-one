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
#define TOOLTIP_LINES        (8u)                           // 
#define TOOLTIP_LINE_HEIGHT  (0.022f)                       // 
#define TOOLTIP_OUTLINE_SIZE (1u)                           // 
#define TOOLTIP_BORDER_SIZE  (coreVector2(0.02f, 0.016f))   // 
#define TOOLTIP_MOUSE_OFFSET (coreVector2(0.03f,-0.03f))    // 

#define TOOLTIP_PRINT(f,...) (this + __LINE__ * 100u), f, ##__VA_ARGS__   // 
#define TOOLTIP_ONELINER     (1000.0f)                                    // 


// ****************************************************************
// tooltip class
class cTooltip final : public coreObject2D
{
private:
    coreLabel m_aLine[TOOLTIP_LINES];   // 

    coreUintW m_iNumLines;              // 
    coreBool  m_bDisplay;               // 

    const void* m_pLastRef;             // 


public:
    cTooltip()noexcept;

    DISABLE_COPY(cTooltip)

    // render and move the tooltip
    void Render()override;
    void Move  ()override;

    // 
    template <typename... A> void ShowText(const coreFloat& fWidth, const void* pRef, const coreChar* pcFormat, A&&... vArgs);
    void                          ShowText(const coreFloat& fWidth, const coreChar* pcText);

    // 
    inline void Reset() {m_pLastRef = NULL;}


private:
    // 
    void __ShowText(const coreFloat& fWidth, const coreChar* pcText);
};


// ****************************************************************
// 
template <typename... A> void cTooltip::ShowText(const coreFloat& fWidth, const void* pRef, const coreChar* pcFormat, A&&... vArgs)
{
    // 
    m_bDisplay = true;
    this->SetCenter(Core::Input->GetMousePosition());

    // 
    if(m_pLastRef == pRef) return;
    m_pLastRef = pRef;

    // 
    this->__ShowText(fWidth, PRINT(pcFormat, std::forward<A>(vArgs)...));
}


#endif // _P1_GUARD_TOOLTIP_H_