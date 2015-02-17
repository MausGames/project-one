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
cMainMenu::cMainMenu()noexcept
: coreMenu (2, SURFACE_MAIN_DEFAULT)
{
    // 
    m_GameLogo.DefineProgram("default_2d_program");
    m_GameLogo.DefineTexture(0, "game_logo.png");
    m_GameLogo.SetPosition  (coreVector2(0.0f,0.17f));
    m_GameLogo.SetSize      (coreVector2(1.0f,1.0f) * 0.4f);

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM, 0);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f,-0.08f));
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_CreditsButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM, 0);
    m_CreditsButton.DefineProgram("menu_border_program");
    m_CreditsButton.SetPosition  (m_StartButton.GetPosition() - coreVector2(0.0f,0.09f));
    m_CreditsButton.SetSize      (m_StartButton.GetSize());
    m_CreditsButton.GetCaption()->SetTextLanguage("CREDITS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM, 0);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_StartButton.GetPosition() - coreVector2(0.0f,0.18f));
    m_ExitButton.SetSize      (m_StartButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    m_VersionInfo.Construct   (MENU_FONT_SMALL, 0);
    m_VersionInfo.SetPosition (coreVector2(0.0f, 0.01f));
    m_VersionInfo.SetCenter   (coreVector2(0.0f,-1.0f) * g_vMenuCenter);
    m_VersionInfo.SetAlignment(coreVector2(0.0f, 1.0f));
    m_VersionInfo.SetText     (PRINT("(c) 2010-2015 Maus Games - v0.1.0a - %s %.5s", __DATE__, __TIME__));

    // 
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_GameLogo);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_StartButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_CreditsButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExitButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_VersionInfo);
}


// ****************************************************************
// move the main menu
void cMainMenu::Move()
{
    // move the menu
    coreMenu::Move();

    if(this->GetCurSurface() == SURFACE_MAIN_DEFAULT)
    {
        // 
        cMenu::UpdateButton(&m_StartButton);
        cMenu::UpdateButton(&m_CreditsButton);
        cMenu::UpdateButton(&m_ExitButton);

        if(m_StartButton.IsClicked())
        {
            // 
            g_pGame = new cGame();
            g_pGame->LoadMission(cMellanMission::ID);
        }
        else if(m_CreditsButton.IsClicked())
        {

        }
        else if(m_ExitButton.IsClicked())
        {
            // 
            Core::System->Quit();
        }
    }
    else if(this->GetCurSurface() == SURFACE_MAIN_CREDITS)
    {

    }
}