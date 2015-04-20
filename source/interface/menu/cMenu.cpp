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
: coreMenu (3u, SURFACE_INTRO)
{
    // create intro and main menu
    m_pIntroMenu = new cIntroMenu();
    m_pMainMenu  = new cMainMenu();

    // bind menu objects
    this->BindObject(SURFACE_INTRO, m_pIntroMenu);
    this->BindObject(SURFACE_MAIN,  m_pMainMenu);
}


// ****************************************************************
// destructor
cMenu::~cMenu()
{
    // delete intro and main menu
    SAFE_DELETE(m_pIntroMenu)
    SAFE_DELETE(m_pMainMenu)
}


// ****************************************************************
// move the menu
void cMenu::Move()
{
    // move the menu
    coreMenu::Move();

    // control mouse with joystick
    Core::Input->UseMouseWithJoystick(0u, 0u, 1u, 0.4f);
    Core::Input->UseMouseWithJoystick(1u, 0u, 1u, 0.4f);

    if(this->GetCurSurface() == SURFACE_INTRO)
    {
        // 
        if(m_pIntroMenu->GetStatus()) this->ChangeSurface(SURFACE_MAIN, 1.0f);
    }
    else if(this->GetCurSurface() == SURFACE_MAIN)
    {
        if(m_pMainMenu->GetStatus())
        {
            // 
            this->ChangeSurface(SURFACE_EMPTY, 1.0f);

            // 
            ASSERT(!g_pGame)
            g_pGame = new cGame(false);
            g_pGame->LoadMission(cViridoMission::ID);

            // TODO: menue-riss bei start oder seitlicher fade-out # 

            // unload expendable menu resources
            Core::Manager::Resource->AttachFunction([this]()
            {
                if(!this->GetTransition().GetStatus())
                {
                    // delete intro and main menu
                    SAFE_DELETE(m_pIntroMenu)
                    SAFE_DELETE(m_pMainMenu)
                    return CORE_OK;
                }
                return CORE_BUSY;
            });
        }
    }
}


// ****************************************************************
// default button update routine
void cMenu::UpdateButton(coreButton* OUTPUT pButton, const coreBool& bFocused)
{
    // select visible strength
    const coreFloat fLight = bFocused ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE;

    // set button and caption color
    pButton              ->SetColor3(coreVector3(1.0f,1.0f,1.0f) * (fLight));
    pButton->GetCaption()->SetColor3(coreVector3(1.0f,1.0f,1.0f) * (fLight * MENU_CONTRAST_WHITE));
}


// ****************************************************************
// 
coreVector3 cMenu::HealthColor(const coreFloat& fValue)
{
    ASSERT(0.0f <= fValue && fValue <= 1.0f)

    // 
    if(fValue >= 0.5f) return LERP(COLOR_YELLOW_F, COLOR_GREEN_F,  fValue*2.0f - 1.0f);
                       return LERP(COLOR_RED_F,    COLOR_YELLOW_F, fValue*2.0f);
}


// ****************************************************************
// 
coreVector3 cMenu::ChainColor(const coreFloat& fValue)
{
    ASSERT(0.0f <= fValue && fValue <= 1.0f)

    // 
    if(fValue >= 0.5f) return LERP(COLOR_PURPLE_F, COLOR_BLUE_F,   fValue*2.0f - 1.0f);
                       return LERP(COLOR_RED_F,    COLOR_PURPLE_F, fValue*2.0f);
}