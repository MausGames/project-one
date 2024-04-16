﻿//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cMainMenu::cMainMenu()noexcept
: coreMenu (1u, SURFACE_MAIN_DEFAULT)
{
    // create menu objects
    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f, 0.09f * 2.5f));
    m_StartButton.SetSize      (coreVector2(0.34f,0.07f));
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_ScoreButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ScoreButton.DefineProgram("menu_border_program");
    m_ScoreButton.SetPosition  (m_StartButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ScoreButton.SetSize      (m_StartButton.GetSize());
    m_ScoreButton.GetCaption()->SetTextLanguage("LEADERBOARDS");

    m_ReplayButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ReplayButton.DefineProgram("menu_border_program");
    m_ReplayButton.SetPosition  (m_ScoreButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ReplayButton.SetSize      (m_StartButton.GetSize());
    m_ReplayButton.GetCaption()->SetTextLanguage("REPLAYS");

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetPosition  (m_ReplayButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ConfigButton.SetSize      (m_StartButton.GetSize());
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_ExtraButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExtraButton.DefineProgram("menu_border_program");
    m_ExtraButton.SetPosition  (m_ConfigButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExtraButton.SetSize      (m_StartButton.GetSize());
    m_ExtraButton.GetCaption()->SetTextLanguage("EXTRAS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_ExtraButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_StartButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    // bind menu objects
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_StartButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ScoreButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ReplayButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ConfigButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExtraButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExitButton);
}


// ****************************************************************
// move the main menu
void cMainMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_MAIN_DEFAULT:
        {
            // 
            cMenu::UpdateButton(&m_StartButton,  m_StartButton .IsFocused());
            cMenu::UpdateButton(&m_ScoreButton,  m_ScoreButton .IsFocused());
            cMenu::UpdateButton(&m_ReplayButton, m_ReplayButton.IsFocused());
            cMenu::UpdateButton(&m_ConfigButton, m_ConfigButton.IsFocused());
            cMenu::UpdateButton(&m_ExtraButton,  m_ExtraButton .IsFocused());
            cMenu::UpdateButton(&m_ExitButton,   m_ExitButton  .IsFocused());

            if(this->GetAlpha() >= 1.0f)
            {
                if(m_StartButton.IsClicked())
                {
                    // 
                    m_iStatus = 1;
                }
                else if(m_ScoreButton.IsClicked())
                {
                    // 
                    m_iStatus = 2;
                }
                else if(m_ReplayButton.IsClicked())
                {
                    // 
                    m_iStatus = 3;
                }
                else if(m_ConfigButton.IsClicked())
                {
                    // 
                    m_iStatus = 4;
                }
                else if(m_ExtraButton.IsClicked())
                {
                    // 
                    m_iStatus = 5;
                }
                else if(m_ExitButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_EXIT_GAME"), [](const coreInt32 iAnswer)
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES)
                            Core::System->Quit();
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