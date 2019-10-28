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
cPauseMenu::cPauseMenu()noexcept
: coreMenu    (1u, SURFACE_PAUSE_DEFAULT)
, m_iSelected (0u)
{
    // create menu objects
    m_ResumeButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ResumeButton.DefineProgram("menu_border_program");
    m_ResumeButton.SetPosition  (coreVector2(0.0f,-0.19f));
    m_ResumeButton.SetSize      (coreVector2(0.3f, 0.07f));
    m_ResumeButton.GetCaption()->SetTextLanguage("RESUME");

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetPosition  (m_ResumeButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ConfigButton.SetSize      (m_ResumeButton.GetSize());
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_RestartButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_RestartButton.DefineProgram("menu_border_program");
    m_RestartButton.SetPosition  (m_ResumeButton.GetPosition() + m_ResumeButton.GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.03f,-0.18f));
    m_RestartButton.SetSize      (coreVector2(1.0f,1.0f) * m_ResumeButton.GetSize().y);
    m_RestartButton.SetAlignment (coreVector2(1.0f,0.0f));
    m_RestartButton.GetCaption()->SetText(ICON_REFRESH_ALT);

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_ResumeButton.GetPosition() + m_ResumeButton.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(-0.03f,-0.18f));
    m_ExitButton.SetSize      (coreVector2( 1.0f,1.0f) * m_ResumeButton.GetSize().y);
    m_ExitButton.SetAlignment (coreVector2(-1.0f,0.0f));
    m_ExitButton.GetCaption()->SetText(ICON_POWER_OFF);

    // bind menu objects
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_ResumeButton);
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_ConfigButton);
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_RestartButton);
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_ExitButton);
}


// ****************************************************************
// move the pause menu
void cPauseMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_PAUSE_DEFAULT:
        {
            // 
                 if(m_ResumeButton .IsFocused()) m_iSelected = 0u;
            else if(m_ConfigButton .IsFocused()) m_iSelected = 1u;
            else if(m_RestartButton.IsFocused()) m_iSelected = 2u;
            else if(m_ExitButton   .IsFocused()) m_iSelected = 3u;

            // 
            cMenu::UpdateButton(&m_ResumeButton,  m_iSelected == 0u);
            cMenu::UpdateButton(&m_ConfigButton,  m_iSelected == 1u);
            cMenu::UpdateButton(&m_RestartButton, m_iSelected == 2u, COLOR_MENU_YELLOW);
            cMenu::UpdateButton(&m_ExitButton,    m_iSelected == 3u, COLOR_MENU_RED);

            // 
            if(m_RestartButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("RESTART"));
            if(m_ExitButton   .IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("EXIT_GAME"));

            if(this->GetAlpha() >= 1.0f)
            {
                if(m_ResumeButton.IsClicked() || g_MenuInput.bPause)
                {
                    // 
                    m_iStatus = 1;
                }
                else if(m_ConfigButton.IsClicked())
                {
                    // 
                    m_iStatus = 2;
                }
                else if(m_RestartButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_RESTART"), [this](const coreInt32 iAnswer)
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES)
                            m_iStatus = 103;
                    });
                }
                else if(m_ExitButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_EXIT_GAME"), [this](const coreInt32 iAnswer)
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES)
                            m_iStatus = 104;
                    });
                }
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }
}