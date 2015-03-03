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
: coreMenu     (2, SURFACE_MAIN_DEFAULT)
, m_iCurButton (0)
{
    // create menu objects
    m_aGameLogo[0].DefineProgram("menu_sharp_logo_program");
    m_aGameLogo[0].DefineTexture(0, "game_logo_back.png");
    m_aGameLogo[0].SetPosition  (coreVector2(0.0f,0.172f));
    m_aGameLogo[0].SetSize      (coreVector2(1.0f,1.0f) * 0.405f);
    m_aGameLogo[0].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 1.05f);

    m_aGameLogo[1].DefineProgram("menu_sharp_program");
    m_aGameLogo[1].DefineTexture(0, "game_logo_front.png");
    m_aGameLogo[1].SetPosition  (m_aGameLogo[0].GetPosition() + coreVector2(0.0f,0.028f));
    m_aGameLogo[1].SetSize      (coreVector2(1.0f,0.5f) * 0.43f);

    m_Background.DefineProgram("menu_border_program");
    m_Background.DefineTexture(0, "menu_background_black.png");
    m_Background.SetPosition  (coreVector2(0.0f,0.05f));
    m_Background.SetSize      (coreVector2(0.6f,0.4f));

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM, 0);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f,-0.08f));
    m_StartButton.SetSize      (coreVector2(0.3f, 0.07f));
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

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM, 0);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_ExitButton.GetPosition());
    m_BackButton.SetSize      (m_ExitButton.GetSize());
    m_BackButton.GetCaption()->SetTextLanguage("BACK");

    m_VersionInfo.Construct   (MENU_FONT_SMALL, 0);
    m_VersionInfo.SetPosition (coreVector2(0.0f, 0.01f));
    m_VersionInfo.SetCenter   (coreVector2(0.0f,-0.5f) * g_vMenuCenter);
    m_VersionInfo.SetAlignment(coreVector2(0.0f, 1.0f));
    m_VersionInfo.SetText     (PRINT("(c) 2010-2015 Maus Games - v0.1.0 new alpha - %s %.5s", __DATE__, __TIME__));

    // bind menu objects
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_aGameLogo[0]);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_aGameLogo[1]);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_StartButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_CreditsButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExitButton);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_VersionInfo);

    this->BindObject(SURFACE_MAIN_CREDITS, &m_Background);
    this->BindObject(SURFACE_MAIN_CREDITS, &m_BackButton);
    this->BindObject(SURFACE_MAIN_CREDITS, &m_VersionInfo);
}


// ****************************************************************
// move the main menu
void cMainMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;

    // 
    m_aGameLogo[0].SetDirection(coreVector2::Direction(float(Core::System->GetTotalTime()) * -0.05f));

    if(this->GetCurSurface() == SURFACE_MAIN_DEFAULT)
    {
        // 
             if(m_StartButton  .IsFocused()) m_iCurButton = 0;
        else if(m_CreditsButton.IsFocused()) m_iCurButton = 1;
        else if(m_ExitButton   .IsFocused()) m_iCurButton = 2;

        // 
        cMenu::UpdateButton(&m_StartButton,   m_iCurButton == 0);
        cMenu::UpdateButton(&m_CreditsButton, m_iCurButton == 1);
        cMenu::UpdateButton(&m_ExitButton,    m_iCurButton == 2);

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
            m_iCurButton = 0;
            cMenu::UpdateButton(&m_StartButton,   true);
            cMenu::UpdateButton(&m_CreditsButton, false);

            // 
            this->ChangeSurface(SURFACE_MAIN_DEFAULT, 3.0f);
        }
    }
}