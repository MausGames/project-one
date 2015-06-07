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
cConfigMenu::cConfigMenu()noexcept
: coreMenu (4u, SURFACE_CONFIG_VIDEO)
{
    // create menu objects
    m_Background.DefineProgram("menu_border_program");
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.SetPosition  (coreVector2(0.0f,0.05f));
    m_Background.SetSize      (coreVector2(0.8f,0.6f));

    m_SaveButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_SaveButton.DefineProgram("menu_border_program");
    m_SaveButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) - coreVector2(0.0f,0.02f));
    m_SaveButton.SetSize      (coreVector2(0.3f,0.07f));
    m_SaveButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_SaveButton.GetCaption()->SetTextLanguage("SAVE");

    m_CancelButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, 0u);
    m_CancelButton.DefineProgram("menu_border_program");
    m_CancelButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) - coreVector2(0.0f,0.02f));
    m_CancelButton.SetSize      (m_SaveButton.GetSize());
    m_CancelButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_CancelButton.GetCaption()->SetTextLanguage("CANCEL");

    // bind menu objects
    for(coreUintW i = 0u; i < this->GetNumSurfaces(); ++i)
    {
        this->BindObject(i, &m_Background);
        this->BindObject(i, &m_SaveButton);
        this->BindObject(i, &m_CancelButton);
    }
}


// ****************************************************************
// move the config menu
void cConfigMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;

    if(this->GetCurSurface() == SURFACE_CONFIG_VIDEO)
    {
        // 
        cMenu::UpdateButton(&m_SaveButton,   m_SaveButton  .IsFocused());
        cMenu::UpdateButton(&m_CancelButton, m_CancelButton.IsFocused());

        if(this->GetAlpha() >= 1.0f)
        {
            if(m_SaveButton.IsClicked())
            {
                // 
                SaveConfig();
            }
            else if(m_CancelButton.IsClicked())
            {
                // 
                LoadConfig();

                // 
                m_iStatus = 1;
            }
        }
    }
}