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
cMenu::cMenu()noexcept
: coreMenu (2, SURFACE_INTRO)
{
    // 
    this->BindObject(SURFACE_INTRO, &m_IntroMenu);
    this->BindObject(SURFACE_MAIN,  &m_MainMenu);
}


// ****************************************************************
// move the menu
void cMenu::Move()
{
    // move the menu
    coreMenu::Move();

    if(this->GetCurSurface() == SURFACE_INTRO)
    {
        // 
        if(m_IntroMenu.GetStatus()) this->ChangeSurface(SURFACE_MAIN, 1.0f);
    }
    else if(this->GetCurSurface() == SURFACE_MAIN)
    {

    }
}


// ****************************************************************
// 
void cMenu::UpdateButton(coreButton* OUTPUT pButton)
{
    // 
    const float fLight = pButton->IsFocused() ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE;

    // 
    pButton              ->SetColor3(coreVector3(1.0f,1.0f,1.0f) * (fLight));
    pButton->GetCaption()->SetColor3(coreVector3(1.0f,1.0f,1.0f) * (fLight * MENU_TEXT_CONTRAST));
}