///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cArcadeInput::cArcadeInput()noexcept
: m_iTextLength  (0u)
, m_fMove        (0.0f)
, m_iOldGlyph    (0u)
, m_iNewGlyph    (0u)
, m_fCursorAngle (0.0f)
, m_bFinished    (false)
{
    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        const coreFloat X = I_TO_F(i % ARCADE_COLUMNS) - I_TO_F(ARCADE_COLUMNS - 1u) * 0.5f;
        const coreFloat Y = I_TO_F(i / ARCADE_COLUMNS) * -1.0f;

        m_aButton[i].DefineTexture   (0u, "default_black.png");
        m_aButton[i].DefineProgram   ("default_2d_program");
        m_aButton[i].SetPosition     (coreVector2(X, Y)      *  0.08f);
        m_aButton[i].SetSize         (coreVector2(1.0f,1.0f) *  0.07f);
        m_aButton[i].SetFocusModifier(coreVector2(1.0f,1.0f) * (0.08f/0.07f));
        m_aButton[i].SetFocusable    (true);
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
    m_Cursor.DefineTexture(0u, "menu_mission.png");
    m_Cursor.DefineProgram("default_2d_program");
    m_Cursor.SetSize      (coreVector2(1.0f,1.0f) * 0.08f);
    m_Cursor.SetTexSize   (coreVector2(0.5f,0.5f));

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
    m_Text.SetAlpha(this->GetAlpha());   // # swapped
    m_Text.Render();

    // 
    m_Cursor.SetAlpha(this->GetAlpha());
    m_Cursor.Render();
}


// ****************************************************************
// move the arcade input
void cArcadeInput::Move()
{
    // 
    m_bFinished = false;

    // 
    const coreChar cChar = TO_UPPER(Core::Input->GetKeyboardChar());

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        // 
        m_aButton[i].Interact();

        // 
        if(m_aButton[i].IsClicked() || (g_MenuInput.bAccept && (m_iNewGlyph == i)) || (g_acArcadeGlyph[i] == cChar))
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
            break;
        }

        // 
        if(m_aButton[i].IsFocused() && (m_iNewGlyph != i))
        {
            this->__MoveCursor(i);
            break;
        }
    }

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i)
    {
        const coreFloat   fLight = ((g_acArcadeGlyph[i] == ARCADE_COMMAND_DEL) ? m_sTextValue.empty() : (m_sTextValue.length() >= m_iTextLength)) ? MENU_LIGHT_IDLE : MENU_LIGHT_ACTIVE;
        const coreVector3 vColor = (m_iNewGlyph == i) ? g_pMenu->GetHighlightColor() : COLOR_MENU_WHITE;

        m_aGlyph[i].SetColor3(vColor * fLight);
    }

    // 
    if(m_fMove < 1.0f)
    {
        m_fMove.UpdateMin(10.0f, 1.0f);
        m_Cursor.SetPosition(LERPB(m_aGlyph[m_iOldGlyph].GetPosition(), m_aGlyph[m_iNewGlyph].GetPosition(), m_fMove));
    }

    // 
    m_fCursorAngle.UpdateMod(2.0f, 2.0f*PI);
    m_Cursor.SetDirection(coreVector2::Direction(m_fCursorAngle));

    // 
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i) m_aButton[i].Move();
    for(coreUintW i = 0u; i < ARCADE_GLYPHS; ++i) m_aGlyph [i].Move();
    m_Cursor.Move();
    m_Text  .Move();
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
    m_sTextTemplate = coreData::StrUpper(pcTemplate);
    m_iTextLength   = iLength;

    // 
    m_iNewGlyph = m_sTextTemplate.empty() ? 0u : cArcadeInput::__RetrieveGlyphIndex(m_sTextTemplate[0]);

    // 
    m_Cursor.SetPosition(m_aGlyph[m_iNewGlyph].GetPosition());
}


// ****************************************************************
// 
void cArcadeInput::__MoveCursor(const coreUintW iNewGylph)
{
    ASSERT(iNewGylph < ARCADE_GLYPHS)

    // 
    m_fMove     = 0.0f;
    m_iOldGlyph = m_iNewGlyph;
    m_iNewGlyph = iNewGylph;
}