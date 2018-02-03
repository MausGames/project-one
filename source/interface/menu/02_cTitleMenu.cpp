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
cTitleMenu::cTitleMenu()noexcept
: coreMenu    (1u, SURFACE_TITLE_DEFAULT)
, m_iSelected (0u)
{
    // create menu objects
    m_GameLogo.DefineTexture(0u, "game_logo.png");
    m_GameLogo.DefineProgram("menu_sharp_program");
    m_GameLogo.SetPosition  (coreVector2(0.0f,0.15f));
    m_GameLogo.SetSize      (coreVector2(1.0f,1.0f) * 0.52f);
    m_GameLogo.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 1.05f);

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f,-0.19f));
    m_StartButton.SetSize      (coreVector2(0.34f,0.07f));
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_StartButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_StartButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    m_aVersionInfo[0].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_aVersionInfo[0].SetPosition (coreVector2(0.0f, 0.03f));
    m_aVersionInfo[0].SetCenter   (coreVector2(0.0f,-0.5f) * g_vMenuCenter);
    m_aVersionInfo[0].SetAlignment(coreVector2(0.0f, 1.0f));
    m_aVersionInfo[0].SetText     ("(c) 2010-2018 Martin Mauersics (@MausGames)");

    m_aVersionInfo[1].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_aVersionInfo[1].SetPosition (m_aVersionInfo[0].GetPosition() + coreVector2(0.0f,-0.02f));
    m_aVersionInfo[1].SetCenter   (m_aVersionInfo[0].GetCenter());
    m_aVersionInfo[1].SetAlignment(m_aVersionInfo[0].GetAlignment());
    m_aVersionInfo[1].SetText     (PRINT("Project One - v0.1.0a third edition - %s %.5s", __DATE__, __TIME__));

    // bind menu objects
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_GameLogo);
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_StartButton);
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_ExitButton);
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_aVersionInfo[0]);
    this->BindObject(SURFACE_TITLE_DEFAULT, &m_aVersionInfo[1]);
}


// ****************************************************************
// move the title menu
void cTitleMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_TITLE_DEFAULT:
        {
            // 
                 if(m_StartButton.IsFocused()) m_iSelected = 0u;
            else if(m_ExitButton .IsFocused()) m_iSelected = 1u;

            // 
            cMenu::UpdateButton(&m_StartButton, m_iSelected == 0u);
            cMenu::UpdateButton(&m_ExitButton,  m_iSelected == 1u);

            if(this->GetAlpha() >= 1.0f)
            {
                if(m_StartButton.IsClicked())
                {
                    // 
                    m_iStatus = 1;
                }
                else if(m_ExitButton.IsClicked())
                {
                    // 
                    Core::System->Quit();
                }
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }
}