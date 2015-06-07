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
: coreMenu (5u, SURFACE_INTRO)
{
    // create intro and main menu
    m_pIntroMenu = new cIntroMenu();
    m_pMainMenu  = new cMainMenu();

    // bind menu objects
    this->BindObject(SURFACE_INTRO,  m_pIntroMenu);
    this->BindObject(SURFACE_MAIN,   m_pMainMenu);
    this->BindObject(SURFACE_GAME,   &m_GameMenu);
    this->BindObject(SURFACE_CONFIG, &m_ConfigMenu);
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
// render the menu
void cMenu::Render()
{
    // 
    coreMenu::Render();

    // 
    m_Tooltip.Render();
}


// ****************************************************************
// move the menu
void cMenu::Move()
{
    // move the menu
    coreMenu::Move();

    // control mouse with joystick
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        Core::Input->ForwardDpadToStick(i);
        Core::Input->UseMouseWithJoystick(i, 0u, 1u, 0.4f);
    }

    if(this->GetCurSurface() == SURFACE_INTRO)
    {
        if(m_pIntroMenu->GetStatus())
        {
            // 
            this->ChangeSurface(SURFACE_MAIN, 1.0f);
        }
    }
    else if(this->GetCurSurface() == SURFACE_MAIN)
    {
        if(m_pMainMenu->GetStatus())
        {
            // 
            this->ChangeSurface(SURFACE_GAME, 1.0f);

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
    else if(this->GetCurSurface() == SURFACE_GAME)
    {
        if(m_GameMenu.GetStatus() == 1)
        {
            // 
            this->ChangeSurface(SURFACE_EMPTY, 1.0f);

            // 
            ASSERT(!g_pGame)
            g_pGame = new cGame(false);
            g_pGame->LoadMission(cViridoMission::ID);

            // 
            Core::Input->ShowCursor(false);

            // TODO: menue-riss bei start oder seitlicher fade-out # 
            // außerdem bei start im main-menue animation und explosion des logos oder riss in mitte
        }
        else if(m_GameMenu.GetStatus() == 2)
        {
            // 
            this->ChangeSurface(SURFACE_CONFIG, 3.0f);
        }
    }
    else if(this->GetCurSurface() == SURFACE_CONFIG)
    {
        if(m_ConfigMenu.GetStatus())
        {
            // 
            this->ChangeSurface(SURFACE_GAME, 3.0f);
        }
    }

    // 
    m_Tooltip.Move();
}


// ****************************************************************
// default button update routine
void cMenu::UpdateButton(coreButton* OUTPUT pButton, const coreBool& bFocused)
{
    ASSERT(pButton)

    // select visible strength
    const coreFloat fLight = bFocused ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE;

    // set button and caption color
    pButton              ->SetColor3(coreVector3(1.0f,1.0f,1.0f) * (fLight));
    pButton->GetCaption()->SetColor3(coreVector3(1.0f,1.0f,1.0f) * (fLight * MENU_CONTRAST_WHITE));
}


// ****************************************************************
// 
coreVector3 FUNC_CONST cMenu::HealthColor(const coreFloat& fValue)
{
    ASSERT(0.0f <= fValue && fValue <= 1.0f)

    // 
    if(fValue >= 0.5f) return LERP(COLOR_MENU_YELLOW, COLOR_MENU_GREEN,  fValue*2.0f - 1.0f);
                       return LERP(COLOR_MENU_RED,    COLOR_MENU_YELLOW, fValue*2.0f);
}


// ****************************************************************
// 
coreVector3 FUNC_CONST cMenu::ChainColor(const coreFloat& fValue)
{
    ASSERT(0.0f <= fValue && fValue <= 1.0f)

    // 
    if(fValue >= 0.5f) return LERP(COLOR_MENU_PURPLE, COLOR_MENU_BLUE,   fValue*2.0f - 1.0f);
                       return LERP(COLOR_MENU_RED,    COLOR_MENU_PURPLE, fValue*2.0f);
}