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
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetSize      (coreVector2(0.8f,0.55f));

    m_BackButton.Construct    (MENU_BUTTON, MENU_ICON_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f, 0.07f)); // * m_SaveButton.GetSize().y); 
    m_BackButton.SetAlignment (coreVector2(-1.0f, -1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    // bind menu objects
    for(coreUintW i = 0u; i < this->GetNumSurfaces(); ++i)
    {
        this->BindObject(i, &m_Background);
        this->BindObject(i, &m_BackButton);
    }
}


// ****************************************************************
// move the extra menu
void cExtraMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

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

    if(this->GetAlpha() >= 1.0f)
    {
        if(m_BackButton.IsClicked())
        {
            // 
            m_iStatus = 1;
        }
    }

    // 
    cMenu::UpdateButton(&m_BackButton, m_BackButton.IsFocused());

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}