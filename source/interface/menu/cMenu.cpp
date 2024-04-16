///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cMenu::cMenu()noexcept
: coreMenu      (11u, SURFACE_INTRO)
, m_iPauseFrame (0u)
{
    // create intro and title menu
    m_pIntroMenu = new cIntroMenu();
    m_pTitleMenu = new cTitleMenu();

    // 
    m_PauseLayer.DefineTexture(0u, "menu_background_black.png");
    m_PauseLayer.DefineProgram("default_2d_program");
    m_PauseLayer.SetSize      (coreVector2(1.0f,1.0f));
    m_PauseLayer.SetColor4    (coreVector4(0.6f,0.6f,0.6f,0.0f));
    m_PauseLayer.SetTexSize   (coreVector2(1.2f,1.2f));

    // bind menu objects
    this->BindObject(SURFACE_INTRO,   m_pIntroMenu);
    this->BindObject(SURFACE_TITLE,   m_pTitleMenu);
    this->BindObject(SURFACE_MAIN,    &m_MainMenu);
    this->BindObject(SURFACE_GAME,    &m_GameMenu);
    this->BindObject(SURFACE_SCORE,   &m_ScoreMenu);
    this->BindObject(SURFACE_REPLAY,  &m_ReplayMenu);
    this->BindObject(SURFACE_CONFIG,  &m_PauseLayer);
    this->BindObject(SURFACE_CONFIG,  &m_ConfigMenu);
    this->BindObject(SURFACE_EXTRA,   &m_ExtraMenu);
    this->BindObject(SURFACE_PAUSE,   &m_PauseLayer);
    this->BindObject(SURFACE_PAUSE,   &m_PauseMenu);
    this->BindObject(SURFACE_SUMMARY, &m_SummaryMenu);
}


// ****************************************************************
// destructor
cMenu::~cMenu()
{
    // delete intro and title menu
    SAFE_DELETE(m_pIntroMenu)
    SAFE_DELETE(m_pTitleMenu)
}


// ****************************************************************
// render the menu
void cMenu::Render()
{
    // 
    this->coreMenu::Render();

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
    this->coreMenu::Move();

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EMPTY:
        {
            if(g_pGame)
            {
                if(g_MenuInput.bPause || Core::System->GetWinFocusLost())
                {
                    // 
                    this->ChangeSurface(SURFACE_PAUSE, 0.0f);

                    // 
                    this->InvokePauseStep();
                }
                else if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_OUTRO))
                {
                    // 
                    this->ChangeSurface(SURFACE_SUMMARY, 3.0f);

                    // 
                    m_SummaryMenu.ShowSummary();
                }
            }
        }
        break;

    case SURFACE_INTRO:
        {
            if(m_pIntroMenu->GetStatus())
            {
                // switch to title menu
                this->ChangeSurface(SURFACE_TITLE, 1.0f);
            }
        }
        break;

    case SURFACE_TITLE:
        {
            if(m_pTitleMenu->GetStatus())
            {
                // switch to main menu
                this->ChangeSurface(SURFACE_MAIN, 3.0f);

                // unload expendable menu resources
                Core::Manager::Resource->AttachFunction([this]()
                {
                    if(!this->GetTransition().GetStatus())
                    {
                        // delete intro and title menu
                        SAFE_DELETE(m_pIntroMenu)
                        SAFE_DELETE(m_pTitleMenu)
                        return CORE_OK;
                    }
                    return CORE_BUSY;
                });
            }
        }
        break;

    case SURFACE_MAIN:
        {
            if(m_MainMenu.GetStatus() == 1)
            {
                // switch to game menu
                this->ChangeSurface(SURFACE_GAME, 3.0f);
            }
            else if(m_MainMenu.GetStatus() == 2)
            {
                // switch to score menu
                this->ChangeSurface(SURFACE_SCORE, 3.0f);
            }
            else if(m_MainMenu.GetStatus() == 3)
            {
                // switch to replay menu
                this->ChangeSurface(SURFACE_REPLAY, 3.0f);

                // 
                m_ReplayMenu.LoadReplays();
            }
            else if(m_MainMenu.GetStatus() == 4)
            {
                // switch to config menu
                this->ChangeSurface(SURFACE_CONFIG, 3.0f);

                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
            }
            else if(m_MainMenu.GetStatus() == 5)
            {
                // switch to extra menu
                this->ChangeSurface(SURFACE_EXTRA, 3.0f);
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
                g_pGame = new cGame(m_GameMenu.GetSelectedDifficulty(), (m_GameMenu.GetSelectedPlayers() > 1u) ? true : false, GAME_MISSION_LIST_DEFAULT);
                g_pGame->LoadNextMission();

                // 
                g_pReplay->StartRecording();
            }
            else if(m_GameMenu.GetStatus() == 2)
            {
                // return to previous menu
                this->ChangeSurface(this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_SCORE:
        {
            if(m_ScoreMenu.GetStatus())
            {
                // return to previous menu
                this->ChangeSurface(this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_REPLAY:
        {
            if(m_ReplayMenu.GetStatus() == 1)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 1.0f);

                // 
                g_pReplay->CreateGame();
                g_pReplay->StartPlayback();
            }
            else if(m_ReplayMenu.GetStatus() == 2)
            {
                // return to previous menu
                this->ChangeSurface(this->GetOldSurface(), 3.0f);
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
            if(m_PauseMenu.GetStatus() == 1)
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
                this->ChangeSurface(SURFACE_MAIN, 1.0f);

                // 
                if(g_pReplay->GetStatus() == REPLAY_STATUS_RECORDING)
                {
                    g_pReplay->EndRecording();
                    g_pReplay->SaveFile(PRINT("Debug Replay %s", coreData::DateTimePrint("%Y-%m-%d %H:%M:%S")));
                }
                else if(g_pReplay->GetStatus() == REPLAY_STATUS_PLAYBACK)
                {
                    g_pReplay->EndPlayback();
                    g_pReplay->Clear();
                }

                // 
                ASSERT(g_pGame)
                SAFE_DELETE(g_pGame)
            }
        }
        break;

    case SURFACE_SUMMARY:
        {
            if(m_SummaryMenu.GetStatus())
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 3.0f);

                // 
                g_pGame->LoadNextMission();
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    Core::Input->ShowCursor((this->GetCurSurface() != SURFACE_EMPTY) &&
                            (this->GetCurSurface() != SURFACE_SUMMARY));

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
    return (this->GetCurSurface() != SURFACE_EMPTY)   &&
           (this->GetCurSurface() != SURFACE_SUMMARY) && g_pGame;
}

coreBool cMenu::IsPausedWithStep()
{
    if(!this->IsPaused()) this->InvokePauseStep();
    return (m_iPauseFrame != Core::System->GetCurFrame());
}


// ****************************************************************
// 
void cMenu::UpdateLanguageFont()
{
    // 
    const coreChar* pcName = Core::Language->HasString("FONT") ? Core::Language->GetString("FONT") : "ethnocentric.ttf";
    Core::Manager::Resource->AssignProxy("dynamic_font", pcName);

    // 
    Core::Manager::Resource->UpdateResources();
    Core::Manager::Resource->UpdateFunctions();
}


// ****************************************************************
// 
const coreLookup<std::string, std::string>& cMenu::GetLanguageList()
{
    // static language list <name, path>
    static coreLookup<std::string, std::string> s_asLanguage;

    if(s_asLanguage.empty())
    {
        // 
        coreLanguage::GetAvailableLanguages(&s_asLanguage);

#if defined(_P1_DEBUG_RANDOM_)

        // 
        const std::string& sRandFile = s_asLanguage.get_valuelist()[CORE_RAND_RUNTIME % s_asLanguage.size()];
        Core::Language->Load(sRandFile.c_str());

        // 
        cMenu::UpdateLanguageFont();

#endif
    }

    return s_asLanguage;
}


// ****************************************************************
// default button update routine
void cMenu::UpdateButton(coreButton* OUTPUT pButton, const coreBool bFocused, const coreVector3& vFocusColor)
{
    ASSERT(pButton)

    const coreFloat A = LERP(MENU_LIGHT_IDLE, MENU_LIGHT_ACTIVE, 0.75f + 0.25f * SIN(10.0f * coreFloat(Core::System->GetTotalTime())));

    // select visible strength
    const coreFloat   fLight = bFocused ? A : MENU_LIGHT_IDLE;
    const coreVector3 vColor = bFocused ? vFocusColor       : COLOR_MENU_WHITE;

    // set button and caption color
    pButton              ->SetColor3(vColor * (fLight / MENU_CONTRAST_WHITE));
    pButton->GetCaption()->SetColor3(vColor * (fLight));

    // 
    if(pButton->GetOverride() < 0) pButton->SetAlpha(pButton->GetAlpha() * 0.5f);


    pButton->SetFocusable(pButton->GetOverride() >= 0);
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

    const auto UpdateArrowFunc = [&](coreButton* OUTPUT pArrow, const coreUintW iEndIndex)
    {
        const coreBool bEnd = (pSwitchBox->GetCurIndex() == iEndIndex);

        // 
        const coreFloat fAlpha = bEnd ? 0.25f           : (pArrow->IsFocused() ? 1.0f              : 0.75f);
        const coreFloat fLight = bEnd ? MENU_LIGHT_IDLE : (pArrow->IsFocused() ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE);

        // 
        pArrow              ->SetAlpha (pSwitchBox->GetAlpha() * fAlpha);
        pArrow->GetCaption()->SetColor3(COLOR_MENU_WHITE       * fLight);
    };

    // 
    if(pSwitchBox->GetOverride() < 0) pSwitchBox->SetAlpha(pSwitchBox->GetAlpha() * 0.5f);

    // 
    UpdateArrowFunc(pSwitchBox->GetArrow(0u), pSwitchBox->GetEndless() ? ~0u : 0u);
    UpdateArrowFunc(pSwitchBox->GetArrow(1u), pSwitchBox->GetEndless() ? ~0u : (pSwitchBox->GetNumEntries() - 1u));
}


// ****************************************************************
// 
void cMenu::UpdateAnimateProgram(coreObject2D* OUTPUT pObject)
{
    ASSERT(pObject)

    // 
    if(coreMath::IsNear(pObject->GetSize().y, 0.0f)) return;

    // 
    if(!pObject->GetProgram().IsUsable()) return;
    if(!pObject->GetProgram()->Enable())  return;

    // 
    const coreFloat fSize = 2.0f * pObject->GetSize().y * Core::System->GetResolution().yx().AspectRatio();
    const coreFloat fLerp = ((fSize - 0.4f) * RCP(fSize)) * 0.5f;

    // 
    pObject->GetProgram()->SendUniform("u_v4Scale", coreVector4(0.5f - fLerp, 0.5f + fLerp, 2.4f, fSize));
}


// ****************************************************************
// 
UNITY_BUILD
#include "01_cIntroMenu.cpp"
#include "02_cTitleMenu.cpp"
#include "03_cMainMenu.cpp"
#include "04_cGameMenu.cpp"
#include "05_cScoreMenu.cpp"
#include "06_cReplayMenu.cpp"
#include "07_cConfigMenu.cpp"
#include "08_cExtraMenu.cpp"
#include "09_cPauseMenu.cpp"
#include "10_cSummaryMenu.cpp"