///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cArcadeInput::cArcadeInput()noexcept
: m_iTextLength (0u)
, m_fFlashTime  (0.0f)
, m_iCurPage    (0u)
, m_iCurGlyph   (0u)
, m_bFinished   (false)
{
    // 
    this->SetAlpha(0.0f);   // due to late initialization

    // 
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_grey_vignette_program");
    m_Background.SetSize      (coreVector2(1.0f,1.0f));
    m_Background.SetTexSize   (coreVector2(1.2f,1.2f));

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        const coreUintW iCol = i % ARCADE_COLUMNS;
        const coreUintW iRow = i / ARCADE_COLUMNS;

        const coreFloat X = I_TO_F(iCol) - I_TO_F(ARCADE_COLUMNS - 1u) * 0.5f;
        const coreFloat Y = I_TO_F(iRow) * -1.0f;

        m_aButton[i].DefineProgram   ("menu_color_program");
        m_aButton[i].SetPosition     (coreVector2(X, Y)      * 0.08f);
        m_aButton[i].SetSize         (coreVector2(1.0f,1.0f) * 0.07f);
        m_aButton[i].SetColor3       (COLOR_MENU_BLACK);
        m_aButton[i].SetFocusModifier(coreVector2(1.0f,1.0f) * (0.08f/0.07f));
        m_aButton[i].SetFocusable    (true);

        m_Navigator.BindObject(&m_aButton[i], &m_aButton[((iRow + ARCADE_ROWS - 1u) % ARCADE_ROWS) * ARCADE_COLUMNS + iCol], &m_aButton[iRow * ARCADE_COLUMNS + ((iCol + ARCADE_COLUMNS - 1u) % ARCADE_COLUMNS)],
                                              &m_aButton[((iRow + 1u)               % ARCADE_ROWS) * ARCADE_COLUMNS + iCol], &m_aButton[iRow * ARCADE_COLUMNS + ((iCol + 1u)                  % ARCADE_COLUMNS)], MENU_TYPE_DEFAULT);
    }

    // 
    for(coreUintW j = 0u; j < ARCADE_PAGES; ++j)
    {
        for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
        {
            if(ARCADE_IS_COMMAND(j, i))
            {
                // 
                const coreChar* pcText;
                switch(g_aacArcadeGlyph[j][i])
                {
                default: UNREACHABLE
                case ARCADE_COMMAND_ALT: pcText = "ALT"; break;
                case ARCADE_COMMAND_DEL: pcText = "DEL"; break;
                case ARCADE_COMMAND_END: pcText = "END"; break;
                }

                m_aaGlyph[j][i].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
                m_aaGlyph[j][i].SetText  (pcText);
            }
            else
            {
                // 
                const coreChar acText[2] = {g_aacArcadeGlyph[j][i]};

                m_aaGlyph[j][i].Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
                m_aaGlyph[j][i].SetText  (acText);
            }

            m_aaGlyph[j][i].SetPosition(m_aButton[i].GetPosition());
            m_aaGlyph[j][i].SetColor3  (COLOR_MENU_WHITE);
        }
    }

    // 
    for(coreUintW i = 0u; i < ARCADE_TEXTS; ++i)
    {
        m_aText[i].Construct  (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
        m_aText[i].SetPosition(coreVector2(0.0f, 0.25f - 0.07f * I_TO_F(i)));
        m_aText[i].SetColor3  (COLOR_MENU_WHITE);
    }

    // 
    m_State.Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_State.SetPosition(coreVector2(0.0f,0.07f));
    m_State.SetColor3  (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);

    // 
    m_Navigator.SetIgnoreKeyboard(true);
}


// ****************************************************************
// render the arcade input
void cArcadeInput::Render()
{
    // 
    m_Background.SetAlpha(this->GetAlpha() * 0.5f);
    m_Background.Render();

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        m_aButton[i].SetAlpha(this->GetAlpha());
        m_aButton[i].Render();
    }

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        m_aaGlyph[m_iCurPage][i].SetAlpha(this->GetAlpha());
        m_aaGlyph[m_iCurPage][i].Render();
    }

    // 
    for(coreUintW i = 0u; i < ARCADE_TEXTS; ++i)
    {
        m_aText[i].SetAlpha(this->GetAlpha());
        m_aText[i].Render();
    }

    // 
    m_State.SetAlpha(this->GetAlpha());
    m_State.Render();

    // 
    m_Navigator.Render();
}


// ****************************************************************
// move the arcade input
void cArcadeInput::Move()
{
    // 
    m_bFinished = false;

    // 
    m_Navigator.Update();

    // 
    const coreChar cChar = g_MenuInput.bToggle ? ARCADE_COMMAND_ALT : g_MenuInput.bCancel ? ARCADE_COMMAND_DEL : Core::Input->GetKeyboardChar();

    // 
    for(coreUintW j = 0u; j < ARCADE_PAGES; ++j)
    {
        // 
        if(!cChar && (m_iCurPage != j)) continue;

        for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
        {
            // 
            m_aButton[i].Interact();

            // 
            if(TIME && (m_aButton[i].IsClicked(CORE_INPUT_LEFT, CORE_INPUT_RELEASE) || (g_aacArcadeGlyph[j][i] == cChar)))
            {
                if(g_aacArcadeGlyph[j][i] == ARCADE_COMMAND_ALT)
                {
                    // 
                    m_iCurPage = (m_iCurPage + 1u) % ARCADE_PAGES;
                }
                else if(g_aacArcadeGlyph[j][i] == ARCADE_COMMAND_DEL)
                {
                    // 
                    if(m_sTextValue.empty()) m_bFinished = true;
                    this->__PopCharacter();
                    this->__SetText(m_sTextValue.c_str());
                }
                else if(g_aacArcadeGlyph[j][i] == ARCADE_COMMAND_END)
                {
                    // 
                    m_bFinished = true;
                }
                else if(m_sTextValue.length() < m_iTextLength)
                {
                    // 
                    m_sTextValue += g_aacArcadeGlyph[j][i];
                    this->__SetText(m_sTextValue.c_str());

                    // 
                    if(cChar)
                    {
                        coreUint8 iNewPage, iNewGlyph;
                        cArcadeInput::__RetrieveIndex(cChar, &iNewPage, &iNewGlyph);

                        this->__MoveCursor(iNewPage, iNewGlyph);
                    }

                    // 
                    //if(g_MenuInput.bAccept && m_sTextTemplate.starts_with(m_sTextValue))
                    //{
                    //  coreUint8 iNewPage, iNewGlyph;
                    //  cArcadeInput::__RetrieveIndex((m_sTextTemplate == m_sTextValue) ? ARCADE_COMMAND_END : m_sTextTemplate[m_sTextValue.length()], &iNewPage, &iNewGlyph);
                    //
                    //  this->__MoveCursor(iNewPage, iNewGlyph);
                    //}
                }

                // 
                if(!m_bFinished) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);

                // 
                j = ARCADE_PAGES;
                break;
            }

            // 
            if(m_aButton[i].IsFocused() && (m_iCurPage == j) && (m_iCurGlyph != i))
            {
                this->__MoveCursor(j, i);
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);
                break;
            }
        }
    }

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        const coreFloat   fLight = (ARCADE_IS_COMMAND(m_iCurPage, i) || (m_sTextValue.length() < m_iTextLength)) ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE;
        const coreVector3 vColor = (m_iCurGlyph == i) ? g_pMenu->GetHighlightColor() : COLOR_MENU_WHITE;

        m_aaGlyph[m_iCurPage][i].SetColor3(vColor * fLight);
    }

    // 
    if(!cMenuNavigator::IsUsingAny() && !g_pMenu->GetMsgBox()->IsVisible()) m_Navigator.OverrideCurrent(&m_aButton[m_iCurGlyph]);

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i) m_aButton            [i].Move();
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i) m_aaGlyph[m_iCurPage][i].Move();
    for(coreUintW i = 0u; i < ARCADE_TEXTS;  ++i) m_aText              [i].Move();

    // 
    m_State.SetText(PRINT("%zu / %u", m_sTextValue.length(), m_iTextLength));
    m_State.Move();

    // 
    m_fFlashTime.UpdateMax(-1.0f, 0.0f);

    // 
    m_Background.SetColor3   (coreVector3(1.0f,1.0f,1.0f) * LERPH3(1.0f, 3.0f, m_fFlashTime));
    m_Background.SetTexOffset(MENU_LAYER_TEXOFFSET);
    m_Background.Move();

    // 
    m_Navigator.SetAlpha(this->GetAlpha());
    m_Navigator.Move();
}


// ****************************************************************
// 
void cArcadeInput::Start(const coreChar* pcTemplate, const coreUint8 iLength)
{
    ASSERT(pcTemplate && iLength)

    // 
    for(coreUintW i = 0u; i < ARCADE_TEXTS;  ++i)
    {
        m_aText[i].SetColor3(g_pMenu->GetHighlightColor());
    }

    // 
    m_sTextValue    = "";
    m_sTextTemplate = pcTemplate;
    m_iTextLength   = iLength;

    // 
    this->__SetText("");

    //
    if(m_sTextTemplate.empty())
    {
        m_iCurPage  = 0u;
        m_iCurGlyph = 0u;
    }
    else
    {
        cArcadeInput::__RetrieveIndex(m_sTextTemplate[0], &m_iCurPage, &m_iCurGlyph);
    }

    // 
    m_Navigator.ForceCurrent(&m_aButton[m_iCurGlyph]);
}


// ****************************************************************
// 
void cArcadeInput::Clear()
{
    // 
    m_sTextValue = "";

    // 
    this->__SetText("");
}


// ****************************************************************
// 
void cArcadeInput::OverrideText(const coreChar* pcText)
{
    // 
    m_sTextValue = pcText;

    // 
    this->__SetText(pcText);
}


// ****************************************************************
// 
void cArcadeInput::__MoveCursor(const coreUint8 iNewPage, const coreUint8 iNewGylph)
{
    ASSERT(iNewGylph < ARCADE_GLYPHS)

    //
    m_iCurPage  = iNewPage;
    m_iCurGlyph = iNewGylph;

    // 
    m_Navigator.OverrideCurrent(&m_aButton[m_iCurGlyph]);
}


// ****************************************************************
// 
void cArcadeInput::__SetText(const coreChar* pcText)
{
    // 
    const coreUintW iLen = std::strlen(pcText);

    // 
    m_aText[0].SetText((iLen < m_iTextLength) ? PRINT("%s|", pcText) : pcText);
}


// ****************************************************************
// 
void cArcadeInput::__PopCharacter()
{
    // remove last character
    if(!m_sTextValue.empty())
    {
        if(HAS_FLAG(m_sTextValue.back(), 0x80u))
        {
            // handle UTF-8 encoding
            while(!HAS_FLAG(m_sTextValue.back(), 0xC0u)) m_sTextValue.pop_back();
        }
        m_sTextValue.pop_back();
    }
}


// ****************************************************************
// 
void cArcadeInput::__RetrieveIndex(const coreChar cChar, coreUint8* OUTPUT piPageIndex, coreUint8* OUTPUT piGylphIndex)
{
    ASSERT(piPageIndex && piGylphIndex)

    // 
    for(coreUintW j = 0u; j < ARCADE_PAGES; ++j)
    {
        const coreUintW iIndex = std::find(g_aacArcadeGlyph[j], g_aacArcadeGlyph[j] + ARCADE_GLYPHS, cChar) - g_aacArcadeGlyph[j];
        if(iIndex < ARCADE_GLYPHS)
        {
            // 
            (*piPageIndex)  = j;
            (*piGylphIndex) = iIndex;
            return;
        }
    }

    // 
    (*piPageIndex)  = 0u;
    (*piGylphIndex) = 0u;
}