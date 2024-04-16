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
, m_iCurGlyph   (0u)
, m_bFinished   (false)
{
    // 
    this->SetAlpha(0.0f);   // due to late initialization

    // 
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_grey_program");
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
        m_aButton[i].SetPosition     (coreVector2(X, Y)      *  0.08f);
        m_aButton[i].SetSize         (coreVector2(1.0f,1.0f) *  0.07f);
        m_aButton[i].SetColor3       (COLOR_MENU_BLACK);
        m_aButton[i].SetFocusModifier(coreVector2(1.0f,1.0f) * (0.08f/0.07f));
        m_aButton[i].SetFocusable    (true);

        m_Navigator.BindObject(&m_aButton[i], &m_aButton[((iRow + ARCADE_ROWS - 1u) % ARCADE_ROWS) * ARCADE_COLUMNS + iCol], &m_aButton[iRow * ARCADE_COLUMNS + ((iCol + ARCADE_COLUMNS - 1u) % ARCADE_COLUMNS)],
                                              &m_aButton[((iRow + 1u)               % ARCADE_ROWS) * ARCADE_COLUMNS + iCol], &m_aButton[iRow * ARCADE_COLUMNS + ((iCol + 1u)                  % ARCADE_COLUMNS)], MENU_TYPE_DEFAULT);
    }

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        if(ARCADE_IS_COMMAND(i))
        {
            // 
            const coreChar* pcText;
            switch(g_acArcadeGlyph[i])
            {
            default: ASSERT(false)
            case ARCADE_COMMAND_DEL: pcText = "DEL"; break;
            case ARCADE_COMMAND_END: pcText = "END"; break;
            }

            m_aGlyph[i].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
            m_aGlyph[i].SetText  (pcText);
        }
        else
        {
            // 
            const coreChar acText[2] = {g_acArcadeGlyph[i]};

            m_aGlyph[i].Construct(MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
            m_aGlyph[i].SetText  (acText);
        }

        m_aGlyph[i].SetPosition(m_aButton[i].GetPosition());
        m_aGlyph[i].SetColor3  (COLOR_MENU_WHITE);
    }

    // 
    m_Text.Construct  (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    m_Text.SetPosition(coreVector2(0.0f,0.2f));
    m_Text.SetColor3  (COLOR_MENU_WHITE);
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
        m_aGlyph[i].SetAlpha(this->GetAlpha());
        m_aGlyph[i].Render();
    }

    // 
    m_Text.SetAlpha(this->GetAlpha());
    m_Text.Render();

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
    const coreChar cChar = TO_UPPER(Core::Input->GetKeyboardChar());

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        // 
        m_aButton[i].Interact();

        // 
        if(m_aButton[i].IsClicked() || (g_MenuInput.bAccept && (m_iCurGlyph == i)) || (g_acArcadeGlyph[i] == cChar))
        {
            // 
            if(cChar) this->__MoveCursor(cArcadeInput::__RetrieveGlyphIndex(cChar));

            if(g_acArcadeGlyph[i] == ARCADE_COMMAND_DEL)
            {
                // 
                if(!m_sTextValue.empty()) m_sTextValue.pop_back();
                m_Text.SetText(m_sTextValue.c_str());
            }
            else if(g_acArcadeGlyph[i] == ARCADE_COMMAND_END)
            {
                // 
                m_bFinished = true;
            }
            else if(m_sTextValue.length() < m_iTextLength)
            {
                // 
                m_sTextValue += g_acArcadeGlyph[i];
                m_Text.SetText(m_sTextValue.c_str());

                // 
                if(g_MenuInput.bAccept && m_sTextTemplate.starts_with(m_sTextValue))
                {
                    const coreChar cGlyph = (m_sTextTemplate == m_sTextValue) ? ARCADE_COMMAND_END : m_sTextTemplate[m_sTextValue.length()];
                    this->__MoveCursor(cArcadeInput::__RetrieveGlyphIndex(cGlyph));
                }
            }

            // 
            if(!m_bFinished) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
            break;
        }

        // 
        if(m_aButton[i].IsFocused() && (m_iCurGlyph != i))
        {
            this->__MoveCursor(i);
            break;
        }
    }

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        const coreFloat   fLight = ((g_acArcadeGlyph[i] == ARCADE_COMMAND_DEL) ? m_sTextValue.empty() : (m_sTextValue.length() >= m_iTextLength)) ? MENU_LIGHT_IDLE : MENU_LIGHT_ACTIVE;
        const coreVector3 vColor = (m_iCurGlyph == i) ? g_pMenu->GetHighlightColor() : COLOR_MENU_WHITE;

        m_aGlyph[i].SetColor3(vColor * fLight);
    }

    // 
    if(!cMenuNavigator::IsUsingJoystick() && !g_pMenu->GetMsgBox()->IsVisible()) m_Navigator.OverrideCurrent(&m_aButton[m_iCurGlyph]);

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i) m_aButton[i].Move();
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i) m_aGlyph [i].Move();
    m_Text.Move();

    // 
    m_Background.SetTexOffset(coreVector2(0.0f, FRACT(coreFloat(-0.04 * Core::System->GetTotalTime()))));   // TODO 1: check if menu rotation is correct
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
    m_Text.SetColor3(g_pMenu->GetHighlightColor());
    m_Text.SetText  ("");

    // 
    m_sTextValue    = "";
    m_sTextTemplate = coreData::StrToUpper(pcTemplate);
    m_iTextLength   = iLength;

    // 
    m_iCurGlyph = m_sTextTemplate.empty() ? 0u : cArcadeInput::__RetrieveGlyphIndex(m_sTextTemplate[0]);

    // 
    m_Navigator.ForceCurrent(&m_aButton[m_iCurGlyph]);
}


// ****************************************************************
// 
void cArcadeInput::Clear()
{
    // 
    m_Text.SetText("");

    // 
    m_sTextValue = "";
}


// ****************************************************************
// 
void cArcadeInput::__MoveCursor(const coreUintW iNewGylph)
{
    ASSERT(iNewGylph < ARCADE_GLYPHS)

    // 
    m_iCurGlyph = iNewGylph;

    // 
    m_Navigator.OverrideCurrent(&m_aButton[m_iCurGlyph]);

    // 
    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);
}