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
: coreMenu           (SURFACE_MAX, SURFACE_INTRO)
, m_iPauseFrame      (0u)
, m_TransitionTime   (coreTimer(1.3f, 0.0f, 1u))
, m_iTransitionState (0u)
, m_pTransitionMenu  (NULL)
, m_vHighlightColor  (coreVector3(0.0f,0.0f,0.0f))
{
    // 
    m_PauseLayer.DefineTexture(0u, "menu_background_black.png");
    m_PauseLayer.DefineProgram("default_2d_program");
    m_PauseLayer.SetSize      (coreVector2(1.0f,1.0f));
    m_PauseLayer.SetColor4    (coreVector4(0.6f,0.6f,0.6f,0.0f));
    m_PauseLayer.SetTexSize   (coreVector2(1.2f,1.2f));

    // bind menu objects
    this->BindObject(SURFACE_INTRO,   &m_IntroMenu);
    this->BindObject(SURFACE_TITLE,   &m_IntroMenu);
    this->BindObject(SURFACE_TITLE,   &m_TitleMenu);
    this->BindObject(SURFACE_MAIN,    &m_MainMenu);
    this->BindObject(SURFACE_GAME,    &m_GameMenu);
    this->BindObject(SURFACE_SCORE,   &m_ScoreMenu);
    this->BindObject(SURFACE_REPLAY,  &m_ReplayMenu);
    this->BindObject(SURFACE_EXTRA,   &m_ExtraMenu);
    this->BindObject(SURFACE_CONFIG,  &m_PauseLayer);
    this->BindObject(SURFACE_CONFIG,  &m_ConfigMenu);
    this->BindObject(SURFACE_PAUSE,   &m_PauseLayer);
    this->BindObject(SURFACE_PAUSE,   &m_PauseMenu);
    this->BindObject(SURFACE_SUMMARY, &m_SummaryMenu);
    this->BindObject(SURFACE_DEFEAT,  &m_DefeatMenu);
    this->BindObject(SURFACE_FINISH,  &m_FinishMenu);
    this->BindObject(SURFACE_BRIDGE,  &m_BridgeMenu);

    // 
    m_aFrameBuffer[0].AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_aFrameBuffer[0].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // 
    m_aFrameBuffer[1].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_aFrameBuffer[1].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    m_aFrameBuffer[2].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_aFrameBuffer[2].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create mix object
    m_MixObject.DefineTexture(0u, m_aFrameBuffer[2].GetColorTarget(0u).pTexture);
    m_MixObject.DefineTexture(1u, m_aFrameBuffer[1].GetColorTarget(0u).pTexture);
    m_MixObject.DefineTexture(2u, "menu_background_black.png");
    m_MixObject.DefineProgram("full_transition_wipe_program");
    m_MixObject.SetSize      (coreVector2(1.0f,1.0f));
    m_MixObject.Move();

    // 
    m_IntroMenu.StartIntro();

    // 
    if(g_pSave->GetHeader().oProgress.bFirstPlay)
    {
        m_IntroMenu.ActivateFirstPlay();
        m_TitleMenu.ActivateFirstPlay();
        m_MainMenu .ActivateFirstPlay();
        m_GameMenu .ActivateFirstPlay();
    }
}


// ****************************************************************
// destructor
cMenu::~cMenu()
{
    // explicitly undefine to detach textures
    m_MixObject.Undefine();
}


// ****************************************************************
// render the menu
void cMenu::Render()
{
    if(m_TransitionTime.GetStatus())
    {
        if((m_iTransitionState == 2u) || ((m_iTransitionState == 1u) && !m_pTransitionMenu->GetTransition().GetStatus()))
        {
            // 
            m_aFrameBuffer[0].StartDraw();
            m_aFrameBuffer[0].Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
            {
                glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);
                {
                    // 
                    this->coreMenu::Render();
                }
                glBlendFunc(FOREGROUND_BLEND_DEFAULT);
            }

            // resolve frame buffer to texture
            m_aFrameBuffer[0].Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_aFrameBuffer[m_iTransitionState]);
            m_aFrameBuffer[0].Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);

            // 
            coreFrameBuffer::EndDraw();

            // 
            m_iTransitionState -= 1u;
        }

        if(m_MixObject.GetProgram().IsUsable())
        {
            // set transition uniforms
            m_MixObject.GetProgram()->Enable();
            m_MixObject.GetProgram()->SendUniform("u_v1TransitionTime", m_TransitionTime.GetValue(CORE_TIMER_GET_NORMAL));
            m_MixObject.GetProgram()->SendUniform("u_v2TransitionDir",  g_vHudDirection.Rotated90());

            glBlendFunc(FOREGROUND_BLEND_ALPHA);
            {
                // 
                m_MixObject.Render();
            }
            glBlendFunc(FOREGROUND_BLEND_DEFAULT);
        }
    }
    else this->coreMenu::Render();

    // 
    m_MsgBox .Render();
    m_Tooltip.Render();
}


// ****************************************************************
// move the menu
void cMenu::Move()
{
    // 
    m_TransitionTime.Update(1.0f);

    // 
    if(m_TransitionTime.GetStatus() && (this->GetCurSurface() != SURFACE_INTRO))
        Core::Input->ClearButtonAll();

    // 
    m_MsgBox.Move();   // # clears input

    // move the menu
    this->coreMenu::Move();

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EMPTY:
        {
            if(STATIC_ISVALID(g_pGame))
            {
                if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_OUTRO))
                {
                    // 
                    this->ChangeSurface(SURFACE_SUMMARY, 0.0f);

                    // 
                         if(g_pGame->GetOutroType() == 0u) m_SummaryMenu.ShowMission();
                    else if(g_pGame->GetOutroType() == 1u) m_SummaryMenu.ShowSegment();
                                                      else m_SummaryMenu.ShowBegin();
                }
                else if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))
                {
                    // 
                    this->ChangeSurface(SURFACE_DEFEAT, 0.0f);

                    // 
                    if(g_pGame->GetContinues()) m_DefeatMenu.ShowContinue();
                                           else m_DefeatMenu.ShowGameOver();

                    // 
                    Core::Audio->PauseSound();
                }
                else if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_FINISHED))
                {
                    // 
                    this->ChangeSurface(SURFACE_FINISH, 0.0f);

                    // 
                    m_FinishMenu.ShowThankYou();
                }
                else if(g_MenuInput.bPause || Core::System->GetWinFocusLost())
                {
                    // 
                    this->ChangeSurface(SURFACE_PAUSE, 0.0f);

                    // 
                    this->InvokePauseStep();

                    // 
                    Core::Audio->PauseSound();
                }
            }
        }
        break;

    case SURFACE_INTRO:
        {
            if(m_IntroMenu.GetStatus())
            {
                // switch to title menu
                this->ShiftSurface(this, SURFACE_TITLE, 0.75f);
            }
        }
        break;

    case SURFACE_TITLE:
        {
            if(m_TitleMenu.GetStatus())
            {
                // switch to main menu
                this->ShiftSurface(this, SURFACE_MAIN, 3.0f);
            }
        }
        break;

    case SURFACE_MAIN:
        {
            if(m_MainMenu.GetStatus() == 1)
            {
                // switch to game menu
                this->ShiftSurface(this, SURFACE_GAME, 3.0f);

                // 
                m_GameMenu.ChangeSurface(g_pSave->GetHeader().oProgress.bFirstPlay ? SURFACE_GAME_ARMORY : SURFACE_GAME_STANDARD, 0.0f);
                m_GameMenu.LoadValues();
            }
            else if(m_MainMenu.GetStatus() == 2)
            {
                // switch to score menu
                this->ShiftSurface(this, SURFACE_SCORE, 3.0f);
            }
            else if(m_MainMenu.GetStatus() == 3)
            {
                // switch to replay menu
                this->ShiftSurface(this, SURFACE_REPLAY, 3.0f);

                // 
                m_ReplayMenu.LoadReplays();
            }
            else if(m_MainMenu.GetStatus() == 4)
            {
                // switch to extra menu
                this->ShiftSurface(this, SURFACE_EXTRA, 3.0f);
            }
            else if(m_MainMenu.GetStatus() == 5)
            {
                // switch to config menu
                this->ShiftSurface(this, SURFACE_CONFIG, 3.0f);

                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
            }
        }
        break;

    case SURFACE_GAME:
        {
            if(m_GameMenu.GetStatus() == 1)
            {
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 1.0f);

                // 
                m_BridgeMenu.EnterGame();

                // 
                this->__StartGame();
            }
            else if(m_GameMenu.GetStatus() == 2)
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_SCORE:
        {
            if(m_ScoreMenu.GetStatus())
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_REPLAY:
        {
            if(m_ReplayMenu.GetStatus() == 1)
            {
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 1.0f);

                // 
                m_BridgeMenu.EnterGame();

                // 
                g_pReplay->CreateGame();
                g_pReplay->StartPlayback();
            }
            else if(m_ReplayMenu.GetStatus() == 2)
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_EXTRA:
        {
            if(m_ExtraMenu.GetStatus())
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_CONFIG:
        {
            if(m_ConfigMenu.GetStatus())
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f);
            }
        }
        break;

    case SURFACE_PAUSE:
        {
            if(m_PauseMenu.GetStatus() == 1)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                // 
                Core::Audio->ResumeSound();
            }
            else if(m_PauseMenu.GetStatus() == 2)
            {
                // switch to config menu
                this->ShiftSurface(this, SURFACE_CONFIG, 3.0f);

                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
            }
            else if(m_PauseMenu.GetStatus() == 3)
            {
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f);

                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, true);

                // 
                Core::Audio->CancelSound();
            }
        }
        break;

    case SURFACE_SUMMARY:
        {
            if(m_SummaryMenu.GetStatus())
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                // 
                g_pGame->LoadNextMission();
            }
        }
        break;

    case SURFACE_DEFEAT:
        {
            if(m_DefeatMenu.GetStatus() == 1)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                // 
                g_pGame->UseContinue();

                // 
                Core::Audio->ResumeSound();
            }
            else if(m_DefeatMenu.GetStatus() == 2)
            {
                // 
                this->ChangeSurface(SURFACE_BRIDGE, 0.0f);

                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, true);

                // 
                Core::Audio->CancelSound();
            }
        }
        break;

    case SURFACE_FINISH:
        {
            if(m_FinishMenu.GetStatus())
            {
                // 
                this->ChangeSurface(SURFACE_BRIDGE, 0.0f);

                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, false);
            }
        }
        break;

    case SURFACE_BRIDGE:
        {
            if(m_BridgeMenu.GetStatus() == 1)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);
            }
            else if(m_BridgeMenu.GetStatus() == 2)
            {
                // 
                this->ShiftSurface(this, m_BridgeMenu.GetTarget(), 0.75f);

                // 
                this->__EndGame();
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    Core::Input->ShowCursor((this->GetCurSurface() != SURFACE_EMPTY)   &&
                            (this->GetCurSurface() != SURFACE_SUMMARY) &&
                            (this->GetCurSurface() != SURFACE_DEFEAT)  &&
                            (this->GetCurSurface() != SURFACE_FINISH));

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
           (this->GetCurSurface() != SURFACE_SUMMARY) &&
           (this->GetCurSurface() != SURFACE_FINISH)  &&
           (this->GetCurSurface() != SURFACE_BRIDGE || m_BridgeMenu.GetPaused()) && STATIC_ISVALID(g_pGame);
}

coreBool cMenu::IsPausedWithStep()
{
    if(!this->IsPaused()) this->InvokePauseStep();
    return (m_iPauseFrame != Core::System->GetCurFrame());
}


// ****************************************************************
// 
void cMenu::ShiftSurface(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface, const coreFloat fSpeed)
{
    ASSERT(pMenu)

    if(!fSpeed)
    {
        // 
        pMenu->ChangeSurface(iNewSurface, 0.0f);
        return;
    }

    if(pMenu->ChangeSurface(iNewSurface, 1.0e06f))
    {
        // 
        m_TransitionTime.Play(CORE_TIMER_PLAY_RESET);
        m_TransitionTime.SetValue(-0.15f);
        m_TransitionTime.SetSpeed(0.9f * fSpeed);

        // 
        m_iTransitionState = 2u;
        m_pTransitionMenu  = pMenu;
    }
}

// ****************************************************************
// 
void cMenu::SetHighlightColor(const coreVector3 vColor)
{
    // 
    if(m_vHighlightColor == vColor) return;
    m_vHighlightColor = vColor;

    // 
    m_SummaryMenu.SetHighlightColor(vColor);
    m_FinishMenu .SetHighlightColor(vColor);
}


// ****************************************************************
// 
void cMenu::UpdateLanguageFont()
{
    // 
    const coreChar* pcName = Core::Language->HasString("FONT") ? Core::Language->GetString("FONT") : MENU_FONT_DEFAULT;
    Core::Manager::Resource->AssignProxy("dynamic_font", pcName);

    // 
    Core::Manager::Resource->UpdateResources();
    Core::Manager::Resource->UpdateFunctions();
}


// ****************************************************************
// 
const coreMap<coreString, coreString>& cMenu::GetLanguageList()
{
    // static language list <name, path>
    static coreMap<coreString, coreString> s_asLanguage;

    if(s_asLanguage.empty())
    {
        // 
        coreLanguage::GetAvailableLanguages(&s_asLanguage);

#if defined(_P1_DEBUG_RANDOM_)

        // 
        const coreString& sRandFile = s_asLanguage.get_valuelist()[CORE_RAND_RUNTIME % s_asLanguage.size()];
        Core::Language->Load(sRandFile.c_str());

        // 
        cMenu::UpdateLanguageFont();

#endif
    }

    return s_asLanguage;
}


// ****************************************************************
// default button update routine
void cMenu::UpdateButton(cGuiButton* OUTPUT pButton, const coreBool bFocused, const coreVector3 vFocusColor)
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


// ****************************************************************
// 
void cMenu::UpdateSwitchBox(cGuiSwitchBox* OUTPUT pSwitchBox)
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
void cMenu::UpdateAnimateProgram(cGuiObject* OUTPUT pObject)
{
    ASSERT(pObject)

    // 
    if(coreMath::IsNear(pObject->GetSize().y, 0.0f)) return;

    // 
    if(!pObject->GetProgram().IsUsable()) return;
    if(!pObject->GetProgram()->Enable())  return;

    // 
    const coreFloat fSize = 2.0f * pObject->GetSize().y;
    const coreFloat fLerp = ((fSize - 0.4f) * RCP(fSize)) * 0.5f;

    // 
    pObject->GetProgram()->SendUniform("u_v4Scale", coreVector4(0.5f - fLerp, 0.5f + fLerp, 2.0f, fSize));
}


// ****************************************************************
// 
void cMenu::ApplyMedalTexture(cGuiObject* OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType)
{
    ASSERT(pObject && (iMedal < MEDAL_MAX) && (iMedalType < MEDAL_TYPE_MAX))

    // 
    const coreUint8 iIndex = (iMedal - MEDAL_BRONZE) + iMedalType * MEDAL_DARK;
    pObject->SetTexOffset(coreVector2(I_TO_F(iIndex % 4u), I_TO_F(iIndex / 4u)) * 0.25f);

    // 
    pObject->SetEnabled((iMedal != MEDAL_NONE) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// reset with the resource manager
void cMenu::__Reset(const coreResourceReset eInit)
{
    if(eInit)
    {
        // 
        m_aFrameBuffer[0].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);
        m_aFrameBuffer[1].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
        m_aFrameBuffer[2].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

        // 
        m_MixObject.DefineTexture(0u, m_aFrameBuffer[2].GetColorTarget(0u).pTexture);
        m_MixObject.DefineTexture(1u, m_aFrameBuffer[1].GetColorTarget(0u).pTexture);
    }
    else
    {
        // unbind textures and stop possible transition
        m_MixObject.DefineTexture(0u, NULL);
        m_MixObject.DefineTexture(1u, NULL);
        m_TransitionTime.Stop();

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFrameBuffer); ++i)
            m_aFrameBuffer[i].Delete();
    }
}


// ****************************************************************
// 
void cMenu::__StartGame()
{
    // 
    sGameOptions oOptions = {};
    oOptions.iPlayers     = m_GameMenu.GetSelectedPlayers   ();
    oOptions.iDifficulty  = m_GameMenu.GetSelectedDifficulty();
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        oOptions.aaiWeapon [i][0] = m_GameMenu.GetSelectedWeapon (i);
        oOptions.aaiSupport[i][0] = m_GameMenu.GetSelectedSupport(i);
    }

    // 
    ASSERT(!STATIC_ISVALID(g_pGame))
    STATIC_NEW(g_pGame, oOptions, GAME_MISSION_LIST_MAIN)
    g_pGame->LoadMissionID(m_GameMenu.GetMissionID());

    // 
    g_pReplay->StartRecording();
}


// ****************************************************************
// 
void cMenu::__EndGame()
{
    // 
    if(g_pReplay->GetStatus() == REPLAY_STATUS_RECORDING)
    {
        g_pReplay->EndRecording();
        g_pReplay->SaveFile(coreData::DateTimePrint("Debug Replay %Y-%m-%d %H:%M:%S"));
        g_pReplay->Clear();
    }
    else if(g_pReplay->GetStatus() == REPLAY_STATUS_PLAYBACK)
    {
        g_pReplay->EndPlayback();
        g_pReplay->Clear();
    }

    // 
    if(g_pSave->GetHeader().oProgress.bFirstPlay)
    {
        g_pSave->EditProgress()->bFirstPlay = false;

        m_TitleMenu.DeactivateFirstPlay();
        m_MainMenu .DeactivateFirstPlay();
        m_GameMenu .DeactivateFirstPlay();
    }

    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    STATIC_DELETE(g_pGame)
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
#include "07_cExtraMenu.cpp"
#include "08_cConfigMenu.cpp"
#include "09_cPauseMenu.cpp"
#include "10_cSummaryMenu.cpp"
#include "11_cDefeatMenu.cpp"
#include "12_cFinishMenu.cpp"
#include "13_cBridgeMenu.cpp"