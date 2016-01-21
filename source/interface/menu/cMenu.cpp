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
: coreMenu      (7u, SURFACE_INTRO)
, m_iPauseFrame (0u)
{
    // create intro and main menu
    m_pIntroMenu = new cIntroMenu();
    m_pMainMenu  = new cMainMenu();

    // 
    m_PauseLayer.DefineTexture(0u, "menu_background_black.png");
    m_PauseLayer.DefineProgram("default_2d_program");
    m_PauseLayer.SetSize      (coreVector2(1.0f,1.0f));
    m_PauseLayer.SetColor4    (coreVector4(0.6f,0.6f,0.6f,0.0f));
    m_PauseLayer.SetTexSize   (coreVector2(1.2f,1.2f));

    // bind menu objects
    this->BindObject(SURFACE_INTRO,  m_pIntroMenu);
    this->BindObject(SURFACE_MAIN,   m_pMainMenu);
    this->BindObject(SURFACE_GAME,   &m_GameMenu);
    this->BindObject(SURFACE_CONFIG, &m_PauseLayer);
    this->BindObject(SURFACE_CONFIG, &m_ConfigMenu);
    this->BindObject(SURFACE_EXTRA,  &m_ExtraMenu);
    this->BindObject(SURFACE_PAUSE,  &m_PauseLayer);
    this->BindObject(SURFACE_PAUSE,  &m_PauseMenu);
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

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EMPTY:
        {
            if(g_pGame)
            {
                if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS) || Core::System->GetMinimized())
                {
                    // 
                    this->ChangeSurface(SURFACE_PAUSE, 0.0f);
                }
                else if(CONTAINS_VALUE(g_pGame->GetStatus(), GAME_STATUS_OUTRO))
                {
                    // 
                    this->ChangeSurface(SURFACE_GAME, 3.0f);

                    // 
                    ASSERT(g_pGame)
                    SAFE_DELETE(g_pGame)
                }
            }
        }
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
                // return to previous menu
                this->ChangeSurface(this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_EXTRA:
        {
            if(m_ExtraMenu.GetStatus())
            {
                // return to previous menu
                this->ChangeSurface(this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_PAUSE:
        {
            if((m_PauseMenu.GetStatus() == 1) || Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS))
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);
            }
            else if(m_PauseMenu.GetStatus() == 2)
            {
                // switch to config menu
                this->ChangeSurface(SURFACE_CONFIG, 3.0f);

                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
            }
            else if(m_PauseMenu.GetStatus() == 3)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                // 
                g_pGame->RestartMission();
            }
            else if(m_PauseMenu.GetStatus() == 4)
            {
                // 
                this->ChangeSurface(SURFACE_GAME, 1.0f);

                // 
                ASSERT(g_pGame)
                SAFE_DELETE(g_pGame)
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    Core::Input->ShowCursor(this->GetCurSurface() != SURFACE_EMPTY);

    // 
    if((this->GetCurSurface() == SURFACE_PAUSE) || (this->GetOldSurface() == SURFACE_PAUSE))
    {
        m_PauseLayer.SetAlpha    (m_PauseLayer.GetAlpha() * 0.25f);
        m_PauseLayer.SetTexOffset(coreVector2(0.0f, FRACT(coreFloat(-0.04 * Core::System->GetTotalTime()))));
        m_PauseLayer.SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    }
    else
    {
        m_PauseLayer.SetEnabled(CORE_OBJECT_ENABLE_MOVE);
    }

    // 
    m_Tooltip.Move();
}


// ****************************************************************
// 
coreBool cMenu::IsPaused()const
{
    return (this->GetCurSurface() != SURFACE_EMPTY) && g_pGame;
}

coreBool cMenu::IsPausedWithStep()
{
    if(!this->IsPaused()) this->InvokePauseStep();
    return (m_iPauseFrame != Core::System->GetCurFrame());
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