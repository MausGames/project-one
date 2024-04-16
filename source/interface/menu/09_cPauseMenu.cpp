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
cPauseMenu::cPauseMenu()noexcept
: coreMenu (SURFACE_PAUSE_MAX, SURFACE_PAUSE_DEFAULT)
{
    // create menu objects
    m_ResumeButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ResumeButton.DefineProgram("menu_border_program");
    m_ResumeButton.SetPosition  (coreVector2(0.0f,0.135f));
    m_ResumeButton.SetSize      (coreVector2(0.3f,0.07f));
    m_ResumeButton.GetCaption()->SetTextLanguage("RESUME");

    m_RestartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_RestartButton.DefineProgram("menu_border_program");
    m_RestartButton.SetPosition  (m_ResumeButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_RestartButton.SetSize      (m_ResumeButton.GetSize());
    m_RestartButton.GetCaption()->SetTextLanguage("RESTART");

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetPosition  (m_RestartButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ConfigButton.SetSize      (m_ResumeButton.GetSize());
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_ConfigButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_ResumeButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    // 
    m_Navigator.BindObject(&m_ResumeButton,  &m_ExitButton,    NULL, &m_RestartButton, NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_RestartButton, &m_ResumeButton,  NULL, &m_ConfigButton,  NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ConfigButton,  &m_RestartButton, NULL, &m_ExitButton,    NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ExitButton,    &m_ConfigButton,  NULL, &m_ResumeButton,  NULL, NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignFirst(&m_ResumeButton);
    m_Navigator.AssignMenu(this);

    // bind menu objects
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_ResumeButton);
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_RestartButton);
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_ConfigButton);
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_ExitButton);
    this->BindObject(SURFACE_PAUSE_DEFAULT, &m_Navigator);
}


// ****************************************************************
// move the pause menu
void cPauseMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_PAUSE_DEFAULT:
        {
            if(m_ResumeButton.IsClicked() || g_MenuInput.bPause)
            {
                // 
                m_iStatus = 1;
            }
            else if(m_RestartButton.IsClicked())
            {
                // 
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_RESTART"), [this](const coreInt32 iAnswer)
                {
                    if(iAnswer == MSGBOX_ANSWER_YES)
                        m_iStatus = 102;
                });
            }
            else if(m_ConfigButton.IsClicked())
            {
                // 
                m_iStatus = 3;
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

            // 
            cMenu::UpdateButton(&m_ResumeButton,  m_ResumeButton .IsFocused());
            cMenu::UpdateButton(&m_RestartButton, m_RestartButton.IsFocused());
            cMenu::UpdateButton(&m_ConfigButton,  m_ConfigButton .IsFocused());
            cMenu::UpdateButton(&m_ExitButton,    m_ExitButton   .IsFocused());
        }
        break;

    default:
        ASSERT(false)
        break;
    }
}


// ****************************************************************
// 
void cPauseMenu::ActivateFirstPlay()
{
    // 
    m_RestartButton.SetOverride(-1);
}


// ****************************************************************
// 
void cPauseMenu::DeactivateFirstPlay()
{
    // 
    m_RestartButton.SetOverride(0);
}