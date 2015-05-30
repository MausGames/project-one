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
: coreMenu     (2u, SURFACE_MAIN_DEFAULT)
, m_iCurButton (0u)
{
    // create menu objects
    m_GameLogo.DefineProgram("menu_sharp_program");
    m_GameLogo.DefineTexture(0, "game_logo.png");
    m_GameLogo.SetPosition  (coreVector2(0.0f,0.17f));
    m_GameLogo.SetSize      (coreVector2(1.0f,1.0f) * 0.42f);
    m_GameLogo.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 1.15f);

    m_Background.DefineProgram("menu_border_program");
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.SetPosition  (coreVector2(0.0f,0.05f));
    m_Background.SetSize      (coreVector2(0.6f,0.4f));

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f,-0.08f));
    m_StartButton.SetSize      (coreVector2(0.3f, 0.07f));
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_CreditsButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_CreditsButton.DefineProgram("menu_border_program");
    m_CreditsButton.SetPosition  (m_StartButton.GetPosition() - coreVector2(0.0f,0.09f));
    m_CreditsButton.SetSize      (m_StartButton.GetSize());
    m_CreditsButton.GetCaption()->SetTextLanguage("CREDITS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_StartButton.GetPosition() - coreVector2(0.0f,0.18f));
    m_ExitButton.SetSize      (m_StartButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_ExitButton.GetPosition());
    m_BackButton.SetSize      (m_ExitButton.GetSize());
    m_BackButton.GetCaption()->SetTextLanguage("BACK");

    m_aVersionInfo[0].Construct   (MENU_FONT_SMALL, 0u);
    m_aVersionInfo[0].SetPosition (coreVector2(0.0f, 0.03f));
    m_aVersionInfo[0].SetCenter   (coreVector2(0.0f,-0.5f) * g_vMenuCenter);
    m_aVersionInfo[0].SetAlignment(coreVector2(0.0f, 1.0f));
    m_aVersionInfo[0].SetText     ("(c) 2010-2015 Martin Mauersics (@MausGames)");

    m_aVersionInfo[1].Construct   (MENU_FONT_SMALL, 0u);
    m_aVersionInfo[1].SetPosition (coreVector2(0.0f, 0.01f));
    m_aVersionInfo[1].SetCenter   (coreVector2(0.0f,-0.5f) * g_vMenuCenter);
    m_aVersionInfo[1].SetAlignment(coreVector2(0.0f, 1.0f));
    m_aVersionInfo[1].SetText     (PRINT("Project One - v0.1.0a third edition - %s %.5s", __DATE__, __TIME__));

    // bind menu objects
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_GameLogo);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_StartButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_CreditsButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExitButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_aVersionInfo[0]);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_aVersionInfo[1]);

    this->BindObject(SURFACE_MAIN_CREDITS, &m_Background);
    this->BindObject(SURFACE_MAIN_CREDITS, &m_BackButton);
    this->BindObject(SURFACE_MAIN_CREDITS, &m_aVersionInfo[0]);
    this->BindObject(SURFACE_MAIN_CREDITS, &m_aVersionInfo[1]);
}


// ****************************************************************
// move the main menu
void cMainMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;

    if(this->GetCurSurface() == SURFACE_MAIN_DEFAULT)
    {
        // 
             if(m_StartButton  .IsFocused()) m_iCurButton = 0u;
        else if(m_CreditsButton.IsFocused()) m_iCurButton = 1u;
        else if(m_ExitButton   .IsFocused()) m_iCurButton = 2u;

        // 
        cMenu::UpdateButton(&m_StartButton,   m_iCurButton == 0u);
        cMenu::UpdateButton(&m_CreditsButton, m_iCurButton == 1u);
        cMenu::UpdateButton(&m_ExitButton,    m_iCurButton == 2u);

        if(this->GetAlpha() >= 1.0f)
        {
            if(m_StartButton.IsClicked())
            {
                // 
                m_iStatus = 1;
            }
            else if(m_CreditsButton.IsClicked())
            {
                // 
                this->ChangeSurface(SURFACE_MAIN_CREDITS, 3.0f);
            }
            else if(m_ExitButton.IsClicked())
            {
                // 
                Core::System->Quit();
            }
        }
    }
    else if(this->GetCurSurface() == SURFACE_MAIN_CREDITS)
    {
        // 
        cMenu::UpdateButton(&m_BackButton, m_BackButton.IsFocused());

        if(m_BackButton.IsClicked())
        {
            // 
            m_iCurButton = 0u;
            cMenu::UpdateButton(&m_StartButton,   true);
            cMenu::UpdateButton(&m_CreditsButton, false);

            // 
            this->ChangeSurface(SURFACE_MAIN_DEFAULT, 3.0f);
        }
    }
}