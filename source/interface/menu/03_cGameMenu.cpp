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
    m_Background.DefineProgram("menu_border_program");
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.SetPosition  (coreVector2(0.0f,0.05f));
    m_Background.SetSize      (coreVector2(0.8f,0.6f));

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f,0.0f));
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) - coreVector2(0.0f,0.02f));
    m_ConfigButton.SetSize      (m_StartButton.GetSize());
    m_ConfigButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_ExtraButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_ExtraButton.DefineProgram("menu_border_program");
    m_ExtraButton.SetPosition  (m_ConfigButton.GetPosition() + coreVector2(m_ConfigButton.GetSize().x + 0.02f,0.0f));
    m_ExtraButton.SetSize      (m_ConfigButton.GetSize());
    m_ExtraButton.SetAlignment (m_ConfigButton.GetAlignment());
    m_ExtraButton.GetCaption()->SetTextLanguage("EXTRAS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_ICON_MEDIUM_2, 0u);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) - coreVector2(0.0f,0.02f));
    m_ExitButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_ExitButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_ExitButton.GetCaption()->SetText(ICON_POWER_OFF);

    // bind menu objects
    this->BindObject(SURFACE_GAME_MISSION, &m_Background);
    this->BindObject(SURFACE_GAME_MISSION, &m_StartButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ConfigButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ExtraButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ExitButton);
}


// ****************************************************************
// move the game menu
void cGameMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;

    if(this->GetCurSurface() == SURFACE_GAME_MISSION)
    {
        // 
        cMenu::UpdateButton(&m_StartButton,  m_StartButton .IsFocused());
        cMenu::UpdateButton(&m_ConfigButton, m_ConfigButton.IsFocused());
        cMenu::UpdateButton(&m_ExtraButton,  m_ExtraButton .IsFocused());
        cMenu::UpdateButton(&m_ExitButton,   m_ExitButton  .IsFocused());

        // 
        if(m_ExtraButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(0.2f, TOOLTIP_PRINT("123 123 13 543 643 dfs%f fd", 0.2f));
        if(m_ExitButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(0.2f, "A fast hunter, who blocks the way on the beginning of your journey. He does not always attack from the front.");

        //g_pMenu->GetTooltip()->ShowText(NULL, Core::Language->GetString("EXIT_GAME"), TOOLTIP_ONELINER);

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
            else if(m_ExitButton.IsClicked())
            {
                // 
                Core::System->Quit();
            }
        }
    }
}