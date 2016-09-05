//////////////////////////////////////////////////////
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
cGameMenu::cGameMenu()noexcept
: coreMenu (2u, SURFACE_GAME_MISSION)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.05f));
    m_Background.SetSize      (coreVector2(0.8f,0.8f));

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f,-0.12f));
    m_StartButton.SetSize      (coreVector2(0.3f, 0.07f));
    m_StartButton.GetCaption()->SetTextLanguage("START");

    m_LeaderboardButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_LeaderboardButton.DefineProgram("menu_border_program");
    m_LeaderboardButton.SetPosition  (m_StartButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_LeaderboardButton.SetSize      (m_StartButton.GetSize());
    m_LeaderboardButton.GetCaption()->SetTextLanguage("LEADERBOARD");

    m_ReplayButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_ReplayButton.DefineProgram("menu_border_program");
    m_ReplayButton.SetPosition  (m_StartButton.GetPosition() + coreVector2(0.0f,-0.18f));
    m_ReplayButton.SetSize      (m_StartButton.GetSize());
    m_ReplayButton.GetCaption()->SetTextLanguage("REPLAY");

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_ConfigButton.SetSize      (m_StartButton.GetSize());
    m_ConfigButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_ExtraButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_ExtraButton.DefineProgram("menu_border_program");
    m_ExtraButton.SetPosition  (m_ConfigButton.GetPosition() + coreVector2(m_ConfigButton.GetSize().x + 0.02f, 0.0f));
    m_ExtraButton.SetSize      (m_ConfigButton.GetSize());
    m_ExtraButton.SetAlignment (m_ConfigButton.GetAlignment());
    m_ExtraButton.GetCaption()->SetTextLanguage("EXTRAS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_ICON_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_ExitButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_ConfigButton.GetSize().y);
    m_ExitButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_ExitButton.GetCaption()->SetText(ICON_POWER_OFF);

    m_Leaderboard.SetPosition(coreVector2(0.0f,0.4f));
    m_Leaderboard.SetSize    (coreVector2(m_Background.GetSize().x, 0.0f));

    // bind menu objects
    this->BindObject(SURFACE_GAME_MISSION, &m_Background);
    this->BindObject(SURFACE_GAME_MISSION, &m_StartButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_LeaderboardButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ReplayButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ConfigButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ExtraButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ExitButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_Leaderboard);
}


// ****************************************************************
// move the game menu
void cGameMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_GAME_MISSION:
        {
            // 
            cMenu::UpdateButton(&m_StartButton,       m_StartButton      .IsFocused());
            cMenu::UpdateButton(&m_LeaderboardButton, m_LeaderboardButton.IsFocused());
            cMenu::UpdateButton(&m_ReplayButton,      m_ReplayButton     .IsFocused());
            cMenu::UpdateButton(&m_ConfigButton,      m_ConfigButton     .IsFocused());
            cMenu::UpdateButton(&m_ExtraButton,       m_ExtraButton      .IsFocused());
            cMenu::UpdateButton(&m_ExitButton,        m_ExitButton       .IsFocused(), COLOR_MENU_RED);

            // 
            if(m_ExitButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("EXIT_GAME"));

            if(this->GetAlpha() >= 1.0f)
            {
                if(m_StartButton.IsClicked())
                {
                    // 
                    m_iStatus = 1;
                }
                else if(m_ConfigButton.IsClicked())
                {
                    // 
                    m_iStatus = 2;
                }
                else if(m_ExtraButton.IsClicked())
                {
                    // 
                    m_iStatus = 3;
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