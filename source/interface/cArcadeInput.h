///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ARCADEINPUT_H_
#define _P1_GUARD_ARCADEINPUT_H_

// TODO 1: support for: mouse selection, controller selection, full keyboard input, (keyboard arrow selection (+enter? conflicts with other keyboard input))
// TODO 3: implement string sanitize function, and check function (for save and replay ?)
// TODO 3: require two enter presses to confirm with keyboard input
// TODO 3: cancel feature CORE_INPUT_CHAR(ESCAPE) (check coreTextBox issues) (with message box "do you really want to ...", but makes only sense in volatile situations)
// TODO 3: add blinking label-cursor (also to engine textbox)
// TODO 3: cursor might not be centered on character, because text is pixel-aligned and cursor not, maybe offset cursor instead


// ****************************************************************
// arcade input definitions
#define ARCADE_GLYPHS        (ARRAY_SIZE(g_acArcadeGlyph))                   // 
#define ARCADE_COLUMNS       (9u)                                            // 

#define ARCADE_IS_COMMAND(x) (std::iscntrl(coreUint8(g_acArcadeGlyph[x])))   // 
#define ARCADE_COMMAND_DEL   (CORE_INPUT_CHAR(BACKSPACE))                    // 
#define ARCADE_COMMAND_END   (CORE_INPUT_CHAR(RETURN))                       // 

// 
alignas(ALIGNMENT_CACHE) static constexpr coreChar g_acArcadeGlyph[] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
    'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0',
    '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '.', '!', '?', '+', '-', '=', '&',
    ARCADE_COMMAND_DEL,
    ARCADE_COMMAND_END
};


// ****************************************************************
// arcade input class
class cArcadeInput final : public cGuiObject
{
private:
    cGuiObject m_aButton[ARCADE_GLYPHS];   // 
    cGuiLabel  m_aGlyph [ARCADE_GLYPHS];   // 
    cGuiObject m_Cursor;                   // 

    cGuiLabel  m_Text;                     // 
    coreString m_sTextValue;               // 
    coreString m_sTextTemplate;            // 
    coreUint8  m_iTextLength;              // 

    coreFlow  m_fMove;                     // 
    coreUint8 m_iOldGlyph;                 // 
    coreUint8 m_iNewGlyph;                 // 

    coreFlow m_fCursorAngle;               // 

    coreBool m_bFinished;                  // 


public:
    cArcadeInput()noexcept;

    DISABLE_COPY(cArcadeInput)

    // render and move the arcade input
    void Render()final;
    void Move  ()final;

    // 
    void Start(const coreChar* pcTemplate, const coreUint8 iLength);

    // 
    inline const coreChar* GetText    ()const {return m_sTextValue.c_str();}
    inline const coreBool& GetFinished()const {return m_bFinished;}


private:
    // 
    void __MoveCursor(const coreUintW iNewGylph);

    // 
    static constexpr coreUintW __RetrieveGlyphIndex(const coreChar cGlyph);
};


// ****************************************************************
// 
constexpr coreUintW cArcadeInput::__RetrieveGlyphIndex(const coreChar cGlyph)
{
    return std::find(g_acArcadeGlyph, g_acArcadeGlyph + ARRAY_SIZE(g_acArcadeGlyph), cGlyph) - g_acArcadeGlyph;
}


#endif // _P1_GUARD_ARCADEINPUT_H_