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
cExtraMenu::cExtraMenu()noexcept
: coreMenu (1u, SURFACE_EXTRA_CREDITS)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.05f));
    m_Background.SetSize      (coreVector2(0.8f,0.6f));

    m_CancelButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_CancelButton.DefineProgram("menu_border_program");
    m_CancelButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_CancelButton.SetSize      (coreVector2(0.3f,0.07f));
    m_CancelButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_CancelButton.GetCaption()->SetTextLanguage("CANCEL");

    // bind menu objects
    for(coreUintW i = 0u; i < this->GetNumSurfaces(); ++i)
    {
        this->BindObject(i, &m_Background);
        this->BindObject(i, &m_CancelButton);
    }
}


// ****************************************************************
// move the extra menu
void cExtraMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EXTRA_CREDITS:
        {

        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    cMenu::UpdateButton(&m_CancelButton, m_CancelButton.IsFocused());

    if(this->GetAlpha() >= 1.0f)
    {
        if(m_CancelButton.IsClicked())
        {
            // 
            m_iStatus = 1;
        }
    }
}