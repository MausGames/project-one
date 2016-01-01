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
: coreMenu (6u, SURFACE_INTRO)
{
    // create intro and main menu
    m_pIntroMenu = new cIntroMenu();
    m_pMainMenu  = new cMainMenu();

    // bind menu objects
    this->BindObject(SURFACE_INTRO,  m_pIntroMenu);
    this->BindObject(SURFACE_MAIN,   m_pMainMenu);
    this->BindObject(SURFACE_GAME,   &m_GameMenu);
    this->BindObject(SURFACE_CONFIG, &m_ConfigMenu);
    this->BindObject(SURFACE_EXTRA,  &m_ExtraMenu);
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
    m_MsgBox .Render();
    m_Tooltip.Render();
}


// ****************************************************************
// move the menu
void cMenu::Move()
{
    // 
    m_MsgBox.Move();

    // move the menu
    coreMenu::Move();

    // control mouse with joystick
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        Core::Input->ForwardDpadToStick(i);
        Core::Input->UseMouseWithJoystick(i, 0u, 1u, 0.4f);
    }

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EMPTY:
        break;

    case SURFACE_INTRO:
        {
            if(m_pIntroMenu->GetStatus())
            {
                // switch to main menu
                this->ChangeSurface(SURFACE_MAIN, 1.0f);
            }
        }
        break;

    case SURFACE_MAIN:
        {
            if(m_pMainMenu->GetStatus())
            {
                // switch to game menu
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
        break;

    case SURFACE_GAME:
        {
            if(m_GameMenu.GetStatus() == 1)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 1.0f);

                // 
                ASSERT(!g_pGame)
                g_pGame = new cGame(false);
                g_pGame->LoadMission(REF_ID(cViridoMission::ID));

                // 
                Core::Input->ShowCursor(false);
            }
            else if(m_GameMenu.GetStatus() == 2)
            {
                // switch to config menu
                this->ChangeSurface(SURFACE_CONFIG, 3.0f);

                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
            }
            else if(m_GameMenu.GetStatus() == 3)
            {
                // switch to extra menu
                this->ChangeSurface(SURFACE_EXTRA, 3.0f);
            }
        }
        break;

    case SURFACE_CONFIG:
        {
            if(m_ConfigMenu.GetStatus())
            {
                // return to game menu
                this->ChangeSurface(SURFACE_GAME, 3.0f);
            }
        }
        break;

    case SURFACE_EXTRA:
        {
            if(m_ExtraMenu.GetStatus())
            {
                // return to game menu
                this->ChangeSurface(SURFACE_GAME, 3.0f);
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    m_Tooltip.Move();
}


// ****************************************************************
// 
const coreLookup<std::string, std::string>& cMenu::GetLanguageList()
{
    // static language list (key = name, value = path)
    static coreLookup<std::string, std::string> asLanguage;

    if(asLanguage.empty())
    {
        // 
        std::vector<std::string> asFile;
        coreData::ScanFolder("data/languages", "*.lng", &asFile);
        if(asFile.empty()) Core::Log->Error("No language files found (data/languages/*.lng)");

        // 
        FOR_EACH(it, asFile) asLanguage[coreLanguage(it->c_str()).GetString("LANGUAGE")] = std::move(*it);
    }

    return asLanguage;
}


// ****************************************************************
// default button update routine
void cMenu::UpdateButton(coreButton* OUTPUT pButton, const coreBool bFocused, const coreVector3& vFocusColor)
{
    ASSERT(pButton)

    // select visible strength
    const coreFloat   fLight = bFocused ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE;
    const coreVector3 vColor = bFocused ? vFocusColor       : COLOR_MENU_WHITE;

    // set button and caption color
    pButton              ->SetColor3(vColor * (fLight / MENU_CONTRAST_WHITE));
    pButton->GetCaption()->SetColor3(vColor * (fLight));

    // 
    if(pButton->GetOverride() < 0) pButton->SetAlpha(pButton->GetAlpha() * 0.5f);
}

void cMenu::UpdateButton(coreButton* OUTPUT pButton, const coreBool bFocused)
{
    // 
    cMenu::UpdateButton(pButton, bFocused, COLOR_MENU_WHITE);
}


// ****************************************************************
// 
void cMenu::UpdateSwitchBox(coreSwitchBoxU8* OUTPUT pSwitchBox)
{
    ASSERT(pSwitchBox)

    auto UpdateArrowFunc = [&](coreButton* OUTPUT pArrow, const coreUintW iEntries)
    {
        const coreBool bEnd = (pSwitchBox->GetCurIndex() == iEntries);

        // 
        const coreFloat fAlpha = bEnd ? 0.25f           : (pArrow->IsFocused() ? 1.0f              : 0.75f);
        const coreFloat fLight = bEnd ? MENU_LIGHT_IDLE : (pArrow->IsFocused() ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE);

        // 
        pArrow              ->SetAlpha (pSwitchBox->GetAlpha() * fAlpha);
        pArrow->GetCaption()->SetColor3(COLOR_MENU_WHITE       * fLight);
    };

    // 
    UpdateArrowFunc(pSwitchBox->GetArrow(0u), 0u);
    UpdateArrowFunc(pSwitchBox->GetArrow(1u), pSwitchBox->GetNumEntries() - 1u);
}


// ****************************************************************
// 
coreVector3 FUNC_CONST cMenu::HealthColor(const coreFloat fValue)
{
    ASSERT(0.0f <= fValue && fValue <= 1.0f)

    // 
    if(fValue >= 0.5f) return LERP(COLOR_MENU_YELLOW, COLOR_MENU_GREEN,  fValue*2.0f - 1.0f);
                       return LERP(COLOR_MENU_RED,    COLOR_MENU_YELLOW, fValue*2.0f);
}


// ****************************************************************
// 
coreVector3 FUNC_CONST cMenu::ChainColor(const coreFloat fValue)
{
    ASSERT(0.0f <= fValue && fValue <= 1.0f)

    // 
    if(fValue >= 0.5f) return LERP(COLOR_MENU_PURPLE, COLOR_MENU_BLUE,   fValue*2.0f - 1.0f);
                       return LERP(COLOR_MENU_RED,    COLOR_MENU_PURPLE, fValue*2.0f);
}