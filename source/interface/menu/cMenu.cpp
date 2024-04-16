///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector3 cMenu::m_vHighlightColor = COLOR_MENU_WHITE;
coreVector3 cMenu::m_vButtonColor    = COLOR_MENU_WHITE;
cGuiButton* cMenu::m_pCurButton      = NULL;
cGuiButton* cMenu::m_pNewButton      = NULL;
cGuiButton* cMenu::m_pCurTab         = NULL;
cGuiButton* cMenu::m_pNewTab         = NULL;


// ****************************************************************
// constructor
cMenu::cMenu()noexcept
: coreMenu           (SURFACE_MAX, SURFACE_INTRO)
, m_iPauseFrame      (0u)


, iForceA (0xFFu)
, iForceB (0xFFu)


, m_TransitionTime   (coreTimer(1.3f, 0.0f, 1u))
, m_iTransitionState (0u)
, m_pTransitionMenu  (NULL)
//, m_vHighlightColor  (COLOR_MENU_WHITE)
{
    // 
    m_PauseLayer.DefineTexture(0u, "menu_background_black.png");
    m_PauseLayer.DefineProgram("menu_grey_program");
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
    this->BindObject(SURFACE_CONFIG,  &m_ConfigMenu);
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
    //m_IntroMenu.StartIntro();

    // 
    if(g_pSave->GetHeader().oProgress.bFirstPlay)
    {
        m_IntroMenu.ActivateFirstPlay();
        m_TitleMenu.ActivateFirstPlay();
        m_MainMenu .ActivateFirstPlay();
        m_GameMenu .ActivateFirstPlay();
        m_PauseMenu.ActivateFirstPlay();
    }

    // 
    if(g_bDemoVersion)
    {
        m_MainMenu.ActivateDemoVersion();
    }

    // 
    m_TransitionTime.SetTimeID(0);

    // 
    cFigure::GlobalInit();
}


// ****************************************************************
// destructor
cMenu::~cMenu()
{
    // explicitly undefine to detach textures
    m_MixObject.Undefine();

    // 
    cFigure::GlobalExit();
}


// ****************************************************************
// render the menu
void cMenu::Render()
{
    // 
    m_PauseLayer.Render();

    if(m_TransitionTime.GetStatus())
    {
        const auto nRenderFunc = [this](const coreUintW iSurface, coreMenu* pMenu)
        {
            // 
            m_aFrameBuffer[0].StartDraw();
            m_aFrameBuffer[0].Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
            {
                glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);   // TODO 1: reduce those calls
                {
                    // 
                    pMenu->coreMenu::Render();
                }
                glBlendFunc(FOREGROUND_BLEND_DEFAULT);
            }

            // resolve frame buffer to texture
            m_aFrameBuffer[0].Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_aFrameBuffer[iSurface]);
            m_aFrameBuffer[0].Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
            
            
            coreFrameBuffer::EndDraw();   // TODO 1: reduce those calls
        };
        
        if(((m_iTransitionState <= 1u) && !m_pTransitionMenu->GetTransition().GetStatus()) && iForceA != 0xFFu)
        {
            Timeless([&]()
            {
                const coreUint8 iPrev = m_pTransitionMenu->GetCurSurface();

                m_pTransitionMenu->ChangeSurface(iForceA, 0.0f);   // TODO 1: calls move for both old and new
                //m_pTransitionMenu->coreMenu::Move();
                nRenderFunc(2u, m_pTransitionMenu);
                
                m_pTransitionMenu->ChangeSurface(iPrev, 0.0f);   // TODO 1: calls move for both old and new
            });
        }
        if(m_iTransitionState < 1u && iForceB != 0xFFu)
        {
            nRenderFunc(1u, this);
        }

        if((m_iTransitionState == 2u) || ((m_iTransitionState == 1u) && !m_pTransitionMenu->GetTransition().GetStatus()))
        {
            // 
            nRenderFunc(m_iTransitionState, this);

            // 
            //coreFrameBuffer::EndDraw();

            // 
            m_iTransitionState -= 1u;
        }

        if(m_MixObject.GetProgram().IsUsable())
        {
            // set transition uniforms
            m_MixObject.GetProgram()->Enable();
            m_MixObject.GetProgram()->SendUniform("u_v1TransitionTime", m_TransitionTime.GetValue(CORE_TIMER_GET_NORMAL));
            m_MixObject.GetProgram()->SendUniform("u_v2TransitionDir",  g_vHudDirection.InvertedX().Rotated90());

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
    cFigure       ::GlobalUpdate();
    cMenuNavigator::GlobalUpdate();

    // 
    if(!Core::Manager::Resource->IsLoading())   // TODO 1: hier wegen sync mit environment-change   
        m_TransitionTime.Update(1.0f);

    // 
    if(m_TransitionTime.GetStatus() && (this->GetCurSurface() != SURFACE_INTRO) && !DEFINED(_CORE_DEBUG_))
        Core::Input->ClearButtonAll();

    // 
    m_MsgBox.Move();   // # clears input

    // move the menu
    this->coreMenu::Move();
    
    
    static coreBool bStarted = false;
    if(!bStarted && !Core::Manager::Resource->IsLoading())   // after coreMenu::Move()
    {
        bStarted = true;
        m_IntroMenu.StartIntro();
    }
    

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
                    switch(g_pGame->GetOutroType())
                    {
                    default: ASSERT(false)
                    case GAME_OUTRO_MISSION:       m_SummaryMenu.ShowMission();      break;
                    case GAME_OUTRO_SEGMENT:       m_SummaryMenu.ShowSegment();      break;
                    case GAME_OUTRO_BEGINNING:     m_SummaryMenu.ShowBeginning();    break;
                    case GAME_OUTRO_ENDING_NORMAL: m_SummaryMenu.ShowEndingNormal(); break;
                    case GAME_OUTRO_ENDING_SECRET: m_SummaryMenu.ShowEndingSecret(); break;
                    }
                }
                else if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))
                {
                    // 
                    this->ChangeSurface(SURFACE_DEFEAT, 0.0f);

                    // 
                    if(g_pGame->GetContinues()) m_DefeatMenu.ShowContinue();
                                           else m_DefeatMenu.ShowGameOver();
                }
                else if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_FINISHED))
                {
                    if(g_pGame->GetKind() == GAME_KIND_ALL)
                    {
                        // 
                        this->ChangeSurface(SURFACE_FINISH, 0.0f);

                        // 
                        m_FinishMenu.ShowThankYou();
                    }
                    else
                    {
                        // 
                        this->ChangeSurface(SURFACE_BRIDGE, 0.0f);

                        // 
                        m_BridgeMenu.ReturnMenu(SURFACE_GAME, false, false);

                        // 
                        m_GameMenu.ChangeSurface((g_pGame->GetKind() == GAME_KIND_MISSION) ? SURFACE_GAME_STANDARD : SURFACE_GAME_TRAINING, 0.0f);
                        m_GameMenu.LoadValues();
                    }
                }
                else if((HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY) && g_MenuInput.bPause) || Core::System->GetWinFocusLost())
                {
                    // 
                    this->ChangeSurface(SURFACE_PAUSE, 0.0f);

                    // 
                    Core::Audio->PauseSound(SOUND_EFFECT);
                    Core::Audio->PauseSound(SOUND_AMBIENT);
                }
            }
        }
        break;

    case SURFACE_INTRO:
        {
            if(m_IntroMenu.GetStatus())
            {
                // switch to title menu
                this->ShiftSurface(this, SURFACE_TITLE, 0.75f, 0u);
            }
        }
        break;

    case SURFACE_TITLE:
        {
            if(m_TitleMenu.GetStatus())
            {
                // switch to main menu
                this->ShiftSurface(this, SURFACE_MAIN, 3.0f, 0u);

                // 
                m_MainMenu.ResetNavigator();
            }
        }
        break;

    case SURFACE_MAIN:
        {
            if(m_MainMenu.GetStatus() == 1)
            {
                // switch to game menu
                this->ShiftSurface(this, SURFACE_GAME, 3.0f, 1u, false, true);

                // 
                m_GameMenu.ChangeSurface(g_bDemoVersion ? SURFACE_GAME_DEMO : (g_pSave->GetHeader().oProgress.bFirstPlay ? SURFACE_GAME_ARMORY : SURFACE_GAME_STANDARD), 0.0f);
                m_GameMenu.LoadValues();
                m_GameMenu.LoadValuesDemo();
                m_GameMenu.ResetNavigator();
            }
            else if(m_MainMenu.GetStatus() == 2)
            {
                // switch to score menu
                this->ShiftSurface(this, SURFACE_SCORE, 3.0f, 1u);

                // 
                m_ScoreMenu.LoadMissions();
            }
            else if(m_MainMenu.GetStatus() == 3)
            {
                // switch to replay menu
                this->ShiftSurface(this, SURFACE_REPLAY, 3.0f, 1u);

                // 
                m_ReplayMenu.LoadOverview();
            }
            else if(m_MainMenu.GetStatus() == 4)
            {
                // switch to extra menu
                this->ShiftSurface(this, SURFACE_EXTRA, 3.0f, 1u);

                // 
                m_ExtraMenu.ChangeSurface(SURFACE_EXTRA_PROGRESS, 0.0f);
                m_ExtraMenu.LoadMissions();
            }
            else if(m_MainMenu.GetStatus() == 5)
            {
                // switch to config menu
                this->ShiftSurface(this, SURFACE_CONFIG, 3.0f, 1u);

                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
                m_ConfigMenu.ResetNavigator();
            }
        }
        break;

    case SURFACE_GAME:
        {
            if(m_GameMenu.GetStatus() == 1)
            {
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 1.0f, 0u);

                // 
                m_BridgeMenu.EnterGame(!g_pSave->GetHeader().oProgress.bFirstPlay);

                // 
                this->__StartGame();

                // 
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_START);
            }
            else if(m_GameMenu.GetStatus() == 2)
            {
                // return to previous menu          
                this->ShiftSurface(this, SURFACE_MAIN, 3.0f, 2u, true, false);
            }
        }
        break;

    case SURFACE_SCORE:
        {
            if(m_ScoreMenu.GetStatus())
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f, 2u);
            }
        }
        break;

    case SURFACE_REPLAY:
        {
            if(m_ReplayMenu.GetStatus() == 1)
            {
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 1.0f, 0u);

                // 
                m_BridgeMenu.EnterGame(true);

                // 
                g_pReplay->CreateGame();
                g_pReplay->StartPlayback();

                // 
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_START);
            }
            else if(m_ReplayMenu.GetStatus() == 2)
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f, 2u);
            }
        }
        break;

    case SURFACE_EXTRA:
        {
            if(m_ExtraMenu.GetStatus())
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f, 2u);
            }
        }
        break;

    case SURFACE_CONFIG:
        {
            if(m_ConfigMenu.GetStatus())
            {
                // return to previous menu
                this->ShiftSurface(this, this->GetOldSurface(), 3.0f, 2u);
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
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);

                // 
                m_BridgeMenu.UseRestart(true);

                // 
                Core::Audio->CancelSound(SOUND_EFFECT);
                Core::Audio->CancelSound(SOUND_AMBIENT);
            }
            else if(m_PauseMenu.GetStatus() == 3)
            {
                // switch to config menu
                this->ShiftSurface(this, SURFACE_CONFIG, 3.0f, 1u);

                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
                m_ConfigMenu.ResetNavigator();
            }
            else if(m_PauseMenu.GetStatus() == 4)
            {
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);

                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, true, true);

                // 
                Core::Audio->CancelSound(SOUND_EFFECT);
                Core::Audio->CancelSound(SOUND_AMBIENT);
            }
        }
        break;

    case SURFACE_SUMMARY:
        {
            if(m_SummaryMenu.GetStatus() == 1)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                #if defined(_CORE_DEBUG_)
                    if(STATIC_ISVALID(g_pGame))
                #endif

                // 
                g_pGame->LoadNextMission();
            }
            else if(m_SummaryMenu.GetStatus() == 2)
            {
                // 
                this->ChangeSurface(SURFACE_FINISH, 0.0f);

                // 
                m_FinishMenu.ShowThankYou();
            }
            else if(m_SummaryMenu.GetStatus() == 3)
            {
                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);

                // 
                m_BridgeMenu.UseRestart(false);
            }
        }
        break;

    case SURFACE_DEFEAT:
        {
            if(m_DefeatMenu.GetStatus() == 1)
            {
                // 
                this->ChangeSurface(SURFACE_BRIDGE, 0.0f);

                // 
                m_BridgeMenu.UseContinue();
            }
            else if(m_DefeatMenu.GetStatus() == 2)
            {
                // 
                this->ChangeSurface(SURFACE_BRIDGE, 0.0f);

                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, false, true);
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
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, false, true);
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
                this->ShiftSurface(this, m_BridgeMenu.GetTarget(), (m_BridgeMenu.GetTarget() == SURFACE_TITLE) ? 0.75f : 1.0f, 0u);

                // 
                this->__EndGame();
            }
            else if(m_BridgeMenu.GetStatus() == 3)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                // 
                g_pGame->UseContinue();
            }
            else if(m_BridgeMenu.GetStatus() == 4)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                // 
                this->__EndGame();
                this->__StartGame();

                // 
                g_pGame->UseRestart();
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    //Core::Input->ShowCursor((this->GetCurSurface() != SURFACE_EMPTY)   &&
    //                        (this->GetCurSurface() != SURFACE_SUMMARY) &&
    //                        (this->GetCurSurface() != SURFACE_DEFEAT)  &&
    //                        (this->GetCurSurface() != SURFACE_FINISH)  &&
    //                        (this->GetCurSurface() != SURFACE_BRIDGE));
    
    const coreFloat fSpeed = STATIC_ISVALID(g_pGame) ? 1000.0f : 0.5f;
    if(((this->GetCurSurface() == SURFACE_CONFIG) || (this->GetCurSurface() == SURFACE_PAUSE)) && STATIC_ISVALID(g_pGame))
         m_PauseLayer.SetAlpha(MIN(m_PauseLayer.GetAlpha() + fSpeed*TIME, 0.25f));
    else m_PauseLayer.SetAlpha(MAX(m_PauseLayer.GetAlpha() - fSpeed*TIME, 0.0f));

    // 
    if(((this->GetCurSurface() == SURFACE_CONFIG) || (this->GetCurSurface() == SURFACE_PAUSE)) && STATIC_ISVALID(g_pGame))
    {
        //m_PauseLayer.SetAlpha    (0.25f);
        m_PauseLayer.SetTexOffset(coreVector2(0.0f, FRACT(coreFloat(-0.04 * Core::System->GetTotalTime()))));
        //m_PauseLayer.SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    }
    else
    {
        //m_PauseLayer.SetEnabled(CORE_OBJECT_ENABLE_MOVE);
    }

    // 
    m_PauseLayer.SetSize(coreVector2(1.0f,1.0f) * MaxAspectRatio(Core::System->GetResolution()));

    // 
    m_Tooltip   .Move();
    m_PauseLayer.Move();
    
    
    if(m_pCurButton != m_pNewButton)
    {
        if(m_pCurButton)
        {
            //m_pCurButton->SetSize(m_pCurButton->GetSize() / 1.1f);
            //m_pCurButton->Move();
        }
        m_pCurButton = m_pNewButton;
        if(m_pCurButton)
        {
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);
            //m_pCurButton->SetSize(m_pCurButton->GetSize() * 1.1f);
            //m_pCurButton->Move();
        }
    }
    m_pNewButton = NULL;
    
    if(m_pCurTab != m_pNewTab)
    {
        if(m_pCurTab)
        {
        }
        m_pCurTab = m_pNewTab;
        if(m_pCurTab)
        {
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);   // # same sound as button
        }
    }
    m_pNewTab = NULL;
    // TODO 1: tooltip has a delay
}


// ****************************************************************
// 
coreBool cMenu::IsPaused()const
{
    return (this->GetCurSurface() != SURFACE_EMPTY)   &&
           (this->GetCurSurface() != SURFACE_SUMMARY) &&
           (this->GetCurSurface() != SURFACE_DEFEAT)  &&
           (this->GetCurSurface() != SURFACE_FINISH)  &&
           (this->GetCurSurface() != SURFACE_BRIDGE || m_BridgeMenu.GetPaused()) && STATIC_ISVALID(g_pGame);
}

coreBool cMenu::IsPausedWithStep()
{
    if(!this->IsPaused() || Core::Manager::Resource->IsLoading()) this->InvokePauseStep();
    return (m_iPauseFrame + 1u < Core::System->GetCurFrame());
}


// ****************************************************************
// 
void cMenu::ShiftSurface(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface, const coreFloat fSpeed, const coreUint8 iSound, const coreBool bUpdateFrom, const coreBool bUpdateTo)
{
    ASSERT(pMenu && fSpeed)
    
    const coreUint8 iCurSurface = pMenu->GetCurSurface();

    if(pMenu->ChangeSurface(iNewSurface, 1.0e06f))
    {
        //iForceA = bUpdateFrom ? pMenu->GetCurSurface() : 0xFFu;
        //iForceB = bUpdateTo   ? iNewSurface            : 0xFFu;
        iForceA = (iCurSurface != SURFACE_INTRO) ? iCurSurface : 0xFFu;
        iForceB = (iCurSurface != SURFACE_INTRO) ? iNewSurface : 0xFFu;

        // 
        m_TransitionTime.Play(CORE_TIMER_PLAY_RESET);
        m_TransitionTime.SetValue(-0.15f);
        m_TransitionTime.SetSpeed(0.9f * fSpeed);

        // 
        m_iTransitionState = 2u;
        m_pTransitionMenu  = pMenu;

             if(iSound == 1u) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SUB_IN);
        else if(iSound == 2u) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SUB_OUT);
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


void cMenu::SetButtonColor(const coreVector3 vColor)
{
    // 
    m_vButtonColor = vColor;
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
    static const coreMap<coreString, coreString> s_asLanguage = []()
    {
        // 
        coreMap<coreString, coreString> asOutput;
        coreLanguage::GetAvailableLanguages(&asOutput);

        // 
        if(asOutput.empty()) asOutput.emplace("MISSING", "");

#if defined(_P1_DEBUG_RANDOM_)

        // 
        const coreString& sRandFile = asOutput.get_valuelist()[CORE_RAND_RUNTIME % asOutput.size()];
        Core::Language->Load(sRandFile.c_str());

        // 
        cMenu::UpdateLanguageFont();

#endif

        return asOutput;
    }();

    return s_asLanguage;
}


// ****************************************************************
// 
void cMenu::ChangeTab(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface)
{
    ASSERT(pMenu)

    // 
    pMenu->ChangeSurface(iNewSurface, 0.0f);

    // 
    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_TAB);
}


// ****************************************************************
// 
void cMenu::ClearScreen()
{
    // 
    g_pPostProcessing->SetSaturationAll(1.0f);
    g_pPostProcessing->SetValueAll     (1.0f);

    // prevent flickering (# render function)
    g_pGlow->Clear();
    g_pSpecialEffects->ClearAll();
}


// ****************************************************************
// 
const coreChar* cMenu::GetSegmentLetters(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    const coreChar* pcMissionLetter;
    switch(iMissionIndex)
    {
    case  9u: pcMissionLetter = "X1";                             break;
    case 10u: pcMissionLetter = "X2";                             break;
    default:  pcMissionLetter = coreData::ToChars(iMissionIndex); break;
    }

    // 
    const coreChar* pcSegmentLetter = MISSION_SEGMENT_IS_BOSS(iSegmentIndex) ? "B" : coreData::ToChars(iSegmentIndex + 1u);

    // 
    return PRINT("%s-%s", pcMissionLetter, pcSegmentLetter);
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
                              pButton              ->SetColor3(vColor * fLight);
    if(pButton->GetCaption()) pButton->GetCaption()->SetColor3(vColor * fLight);

    // 
    if(pButton->GetOverride() < 0) pButton->SetAlpha(pButton->GetAlpha() * 0.5f);
    
    if(bFocused)
    {
        m_pNewButton = pButton;
    }
    
    if(pButton->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_PRESS))
    {
        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
    }
}

void cMenu::UpdateButton(cGuiButton* OUTPUT pButton, const coreBool bFocused)
{
    // 
    cMenu::UpdateButton(pButton, bFocused, m_vButtonColor);
}


// ****************************************************************
// 
void cMenu::UpdateTab(cGuiButton* OUTPUT pTab, const coreBool bLocked, const coreBool bFocused, const coreVector3 vFocusColor)
{
    ASSERT(pTab)

    // select visible strength
    const coreFloat   fLight = (bLocked || bFocused) ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE;
    const coreVector3 vColor = (bLocked || bFocused) ? vFocusColor       : COLOR_MENU_WHITE;

    // set button and caption color
                           pTab              ->SetColor3(vColor * fLight);
    if(pTab->GetCaption()) pTab->GetCaption()->SetColor3(vColor * fLight);

    // 
    if(pTab->GetOverride() < 0) pTab->SetAlpha(pTab->GetAlpha() * 0.5f);
    
    if(!bLocked && bFocused)
    {
        m_pNewTab = pTab;
    }
}

void cMenu::UpdateTab(cGuiButton* OUTPUT pTab, const coreBool bLocked, const coreBool bFocused)
{
    // 
    cMenu::UpdateTab(pTab, bLocked, bFocused, m_vButtonColor);
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
    
    
    // 
    const coreBool bArrowLeft  = pSwitchBox->GetArrow(0u)->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_PRESS);
    const coreBool bArrowRight = pSwitchBox->GetArrow(1u)->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_PRESS);
    const coreInt8 iUserSwitch = pSwitchBox->GetUserSwitch();
    if(iUserSwitch)
    {
        // 
        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);
    }
    else if(bArrowLeft || bArrowRight)
    {
        // 
        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_DISABLED);
    }
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
    const coreFloat fSize = pObject->GetSize().y;
    const coreFloat fLerp = ((fSize - 0.2f) * RCP(fSize)) * 0.5f;

    // 
    pObject->GetProgram()->SendUniform("u_v4Scale", coreVector4(0.5f - fLerp, 0.5f + fLerp, 2.0f, 2.0f * fSize));
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
        m_MixObject.Move();
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
    coreInt32 iMissionID;
    coreUint8 iTakeFrom, iTakeTo, iKind;
    m_GameMenu.RetrieveStartData(&iMissionID, &iTakeFrom, &iTakeTo, &iKind);

    // 
    sGameOptions oOptions = {};
    oOptions.iKind        = iKind;
    oOptions.iType        = m_GameMenu.GetSelectedType      ();
    oOptions.iMode        = m_GameMenu.GetSelectedMode      ();
    oOptions.iDifficulty  = m_GameMenu.GetSelectedDifficulty();
    oOptions.iFlags       = GAME_FLAG_TASK;
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        oOptions.aiShield  [i]    = m_GameMenu.GetSelectedShield (i);
        oOptions.aaiWeapon [i][0] = m_GameMenu.GetSelectedWeapon (i);
        oOptions.aaiSupport[i][0] = m_GameMenu.GetSelectedSupport(i);
    }

    // 
    const coreInt32* piMissionList = g_bDemoVersion ?            GAME_MISSION_LIST_DEMO  :            GAME_MISSION_LIST_MAIN;
    const coreUintW  iNumMissions  = g_bDemoVersion ? ARRAY_SIZE(GAME_MISSION_LIST_DEMO) : ARRAY_SIZE(GAME_MISSION_LIST_MAIN);

    // 
    ASSERT(!STATIC_ISVALID(g_pGame))
    STATIC_NEW(g_pGame, oOptions, piMissionList, iNumMissions)
    g_pGame->LoadMissionID(iMissionID, iTakeFrom, iTakeTo);

    // 
    if(!g_bDemoVersion) g_pReplay->StartRecording();
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
    if(g_pSave->GetHeader().oProgress.bFirstPlay && g_pSave->GetHeader().oProgress.aiAdvance[1])
    {
        g_pSave->EditProgress()->bFirstPlay = false;

        m_TitleMenu.DeactivateFirstPlay();
        m_MainMenu .DeactivateFirstPlay();
        m_GameMenu .DeactivateFirstPlay();
        m_PauseMenu.DeactivateFirstPlay();
    }

    // 
    if(g_bDemoVersion)
    {
        m_MainMenu.ActivateDemoVersion();
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