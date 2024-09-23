///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector3                              cMenu::s_vHighlightColor = COLOR_MENU_WHITE;
coreVector3                              cMenu::s_vButtonColor    = COLOR_MENU_WHITE;
coreMap<const void*, coreButton*>        cMenu::s_apCurButton     = {};
coreMap<const void*, coreButton*>        cMenu::s_apNewButton     = {};
cGuiButton*                              cMenu::s_pCurTab         = NULL;
cGuiButton*                              cMenu::s_pNewTab         = NULL;
cGuiObject*                              cMenu::s_pCurLine        = NULL;
cGuiObject*                              cMenu::s_pNewLine        = NULL;
coreMap<const void*, cMenu::sButtonData> cMenu::s_aButtonData     = {};


// ****************************************************************
// constructor
cMenu::cMenu()noexcept
: coreMenu             (SURFACE_MAX, SURFACE_INTRO)
, m_iPauseFrame        (0u)
, m_fNoticeSaveTime    (0.0f)
, m_bNoticeSavePrevent (false)


, iForceA (0xFFu)
, iForceB (0xFFu)


, m_TransitionTime   (coreTimer(1.3f, 0.0f, 1u))
, m_iTransitionState (0u)
, m_pTransitionMenu  (NULL)
, m_fVolume  (1.0f)
, m_Interface        (1u)
, m_fInterfaceAlpha  (0.0f)
//, m_vHighlightColor  (COLOR_MENU_WHITE)
{
    // 
    m_PauseLayer.DefineTexture(0u, "menu_background_black.png");
    m_PauseLayer.DefineProgram("menu_grey_vignette_program");
    m_PauseLayer.SetColor4    (coreVector4(0.6f,0.6f,0.6f,0.0f));
    m_PauseLayer.SetTexSize   (coreVector2(1.2f,1.2f));
    m_PauseLayer.SetStyle     (CORE_OBJECT2D_STYLE_VIEWDIR);

    // 
    m_NoticeSave.Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_NoticeSave.SetCenter      (coreVector2( 0.5f,-0.5f));
    m_NoticeSave.SetAlignment   (coreVector2(-1.0f, 1.0f));
    m_NoticeSave.SetColor3      (COLOR_MENU_INSIDE);
    m_NoticeSave.SetStyle       (m_NoticeSave.GetStyle() & ~CORE_OBJECT2D_STYLE_ALTCENTER);
    m_NoticeSave.SetTextLanguage("SAVING");

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
    this->BindObject(SURFACE_CREDITS, &m_CreditRoll);

    // 
    m_aFrameBuffer[0].AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_aFrameBuffer[0].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // 
    m_aFrameBuffer[1].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8, CORE_TEXTURE_MODE_NEAREST);
    m_aFrameBuffer[1].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    m_aFrameBuffer[2].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8, CORE_TEXTURE_MODE_NEAREST);
    m_aFrameBuffer[2].Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create mix object
    m_MixObject.DefineTexture(0u, m_aFrameBuffer[2].GetColorTarget(0u).pTexture);
    m_MixObject.DefineTexture(1u, m_aFrameBuffer[1].GetColorTarget(0u).pTexture);
    m_MixObject.DefineTexture(2u, "menu_background_black.png");
    m_MixObject.DefineProgram("full_transition_wipe_program");
    m_MixObject.SetSize      (coreVector2(1.0f,1.0f));
    m_MixObject.Move();

    // 
    m_TransitionTime.SetTimeID(0);

    // 
    m_Interface.ShowWave("1-1");
    m_Interface.UpdateLayout();
    m_Interface.UpdateEnabled();

    // 
    m_IntroMenu.StartIntro();

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
    cFigure       ::GlobalInit();
    cMenuNavigator::GlobalInit();

    // 
    s_apCurButton.clear();
    s_apNewButton.clear();
    s_pCurTab  = NULL;
    s_pNewTab  = NULL;
    s_pCurLine = NULL;
    s_pNewLine = NULL;
    s_aButtonData.clear();
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
    if(!STATIC_ISVALID(g_pGame))
    {
        // 
        m_Interface.SetAlphaAll(m_fInterfaceAlpha);
        m_Interface.Render();
    }
    
    // 
    m_PauseLayer.Render();

    if(m_TransitionTime.GetStatus())
    {
        ASSERT(m_pTransitionMenu)
        
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
            g_MenuInput = {};
            Core::Input->ClearMouseButton(CORE_INPUT_LEFT);
            if(cMenuNavigator::IsUsingAny()) Core::Input->SetMousePosition(MENUNAVIGATOR_IGNORE_MOUSE);
            
            Timeless([&]()
            {
                const coreUint8 iPrev = m_pTransitionMenu->GetCurSurface();
                const coreBool  bMove = (m_pTransitionMenu != this)           &&
                                        (m_pTransitionMenu != &m_IntroMenu)   &&
                                        (m_pTransitionMenu != &m_SummaryMenu) &&
                                        (m_pTransitionMenu != &m_DefeatMenu)  &&
                                        (m_pTransitionMenu != &m_FinishMenu)  &&
                                        (m_pTransitionMenu != &m_BridgeMenu);

                m_pTransitionMenu->ChangeSurface(iForceA, 0.0f);   // TODO 1: calls move for both old and new
                if(bMove) m_pTransitionMenu->Move();

                nRenderFunc(2u, m_pTransitionMenu);

                m_pTransitionMenu->ChangeSurface(iPrev, 0.0f);   // TODO 1: calls move for both old and new
                if(bMove) m_pTransitionMenu->Move();
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
            if(m_MixObject.GetProgram()->Enable())
            {
                // set transition uniforms
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
    }
    else this->coreMenu::Render();

    // 
    m_MsgBox .Render();
    m_Tooltip.Render();

    // 
    if(m_fNoticeSaveTime)
    {
        m_NoticeSave.Render();
    }
}


// ****************************************************************
// move the menu
void cMenu::Move()
{
    if(!STATIC_ISVALID(g_pGame))
    {
        const coreFloat fOldAlpha = m_fInterfaceAlpha;

        // 
        if((this->GetCurSurface() == SURFACE_CONFIG) && (m_ConfigMenu.GetCurSurface() ==  SURFACE_CONFIG_GAME))
             m_fInterfaceAlpha.UpdateMin( 5.0f, 1.0f);
        else m_fInterfaceAlpha.UpdateMax(-5.0f, 0.0f);

        // 
        if(fOldAlpha || m_fInterfaceAlpha)
        {
            m_Interface.Move();
        }
    }

    // 
    cFigure       ::GlobalUpdate();
    cMenuNavigator::GlobalUpdate();

    // 
    m_TransitionTime.Update(1.0f);

    //     to prevent things from being clicked (both mouse and controller) (e.g. changing switch boxes) while they are not yet visible
    if(m_TransitionTime.GetStatus() && (this->GetCurSurface() != SURFACE_INTRO))
        Core::Input->ClearMouseButtonAll();

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
                    switch(g_pGame->GetOutroType())
                    {
                    default: UNREACHABLE
                    case GAME_OUTRO_MISSION:       m_SummaryMenu.ShowMission();      break;
                    case GAME_OUTRO_SEGMENT:       m_SummaryMenu.ShowSegment();      break;
                    case GAME_OUTRO_BEGINNING:     m_SummaryMenu.ShowBeginning();    break;
                    case GAME_OUTRO_ENDING_NORMAL: m_SummaryMenu.ShowEndingNormal(); break;
                    case GAME_OUTRO_ENDING_SECRET: m_SummaryMenu.ShowEndingSecret(); break;
                    }

                    // 
                    this->ChangeSurface(SURFACE_SUMMARY, 0.0f);
                }
                else if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))
                {
                    if(g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK)
                    {
                        // 
                        if(g_pGame->GetContinuesCur() < g_pReplay->GetHeader().aiDataContinuesUsed[0]) m_DefeatMenu.ShowReplay();
                                                                                                  else m_DefeatMenu.ShowGameOver();
                    }
                    else
                    {
                        // 
                        if(g_pGame->GetContinuesLeft()) m_DefeatMenu.ShowContinue();
                                                   else m_DefeatMenu.ShowGameOver();
                    }

                    // 
                    this->ChangeSurface(SURFACE_DEFEAT, 0.0f);
                }
                else if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_FINISHED))
                {
                    if(g_pGame->GetKind() == GAME_KIND_ALL)
                    {
                        ASSERT(g_bDemoVersion)

                        // 
                        m_FinishMenu.ShowThankYou();

                        // 
                        this->ChangeSurface(SURFACE_FINISH, 0.0f);
                    }
                    else
                    {
                        // 
                        m_BridgeMenu.ReturnMenu(m_BridgeMenu.HasUnlocks() ? SURFACE_TITLE : SURFACE_GAME, false, true);

                        // 
                        m_GameMenu.ChangeSurface(SURFACE_GAME_MAIN, 0.0f);
                        m_GameMenu.LoadValues();

                        // 
                        this->ChangeSurface(SURFACE_BRIDGE, 0.0f);
                    }
                }
                else if((HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY) && g_MenuInput.bPause) || Core::System->GetWinFocusLost())
                {
                    // 
                    m_PauseMenu.ChangeSurface(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY) ? SURFACE_PAUSE_FULL : SURFACE_PAUSE_LIGHT, 0.0f);
                    m_PauseMenu.ResetNavigator();

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
                // 
                m_MainMenu.ResetNavigator();

                if(m_BridgeMenu.HasUnlocks())
                {
                    // 
                    m_BridgeMenu.ShowUnlocks();

                    // 
                    this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);
                }
                else
                {
                    // switch to main menu
                    this->ShiftSurface(this, SURFACE_MAIN, 3.0f, 0u);
                }
            }
        }
        break;

    case SURFACE_MAIN:
        {
            if(m_MainMenu.GetStatus() == 1)
            {
                // 
                m_GameMenu.ChangeSurface(g_bDemoVersion ? SURFACE_GAME_DEMO : (g_pSave->GetHeader().oProgress.bFirstPlay ? SURFACE_GAME_FIRST : SURFACE_GAME_MAIN), 0.0f);
                m_GameMenu.LoadValues();
                m_GameMenu.ResetNavigator();

                // switch to game menu
                this->ShiftSurface(this, SURFACE_GAME, 3.0f, 1u);
            }
            else if(m_MainMenu.GetStatus() == 2)
            {
                // 
                m_ScoreMenu.LoadMissions();
                m_ScoreMenu.ResetNavigator();

                // switch to score menu
                this->ShiftSurface(this, SURFACE_SCORE, 3.0f, 1u);
            }
            else if(m_MainMenu.GetStatus() == 3)
            {
                // 
                m_ReplayMenu.ChangeSurface(SURFACE_REPLAY_OVERVIEW, 0.0f);
                m_ReplayMenu.LoadOverview();
                m_ReplayMenu.ResetNavigator();

                // switch to replay menu
                this->ShiftSurface(this, SURFACE_REPLAY, 3.0f, 1u);
            }
            else if(m_MainMenu.GetStatus() == 4)
            {
                // 
                m_ExtraMenu.ChangeSurface(SURFACE_EXTRA_TROPHY, 0.0f);
                m_ExtraMenu.LoadTrophies();
                m_ExtraMenu.LoadMissions();
                m_ExtraMenu.ResetNavigator();

                // switch to extra menu
                this->ShiftSurface(this, SURFACE_EXTRA, 3.0f, 1u);
            }
            else if(m_MainMenu.GetStatus() == 5)
            {
                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
                m_ConfigMenu.ResetNavigator();

                // switch to config menu
                this->ShiftSurface(this, SURFACE_CONFIG, 3.0f, 1u);
            }
            else if(m_MainMenu.GetStatus() == 6)
            {
                // 
                m_CreditRoll.Start(CREDIT_TYPE_MENU);

                // switch to credits menu
                this->ShiftSurface(this, SURFACE_CREDITS, 3.0f, 1u);
            }
        }
        break;

    case SURFACE_GAME:
        {
            if(m_GameMenu.GetStatus() == 1)
            {
                // 
                m_BridgeMenu.EnterGame(!g_pSave->GetHeader().oProgress.bFirstPlay);

                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 1.0f, 0u);

                // 
                this->__StartGame();

                // 
                ASSERT(STATIC_ISVALID(g_pGame))
                if((g_pGame->GetKind() == GAME_KIND_SEGMENT) && (g_pSave->GetHeader().oProgress.aiAdvance[g_pGame->GetCurMissionIndex()] > 1))
                {
                    g_pGame->DisableMissionName();   // only when starting single/first segment from menu, when first segment of mission was already finished
                }

                // 
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_START);
            }
            else if(m_GameMenu.GetStatus() == 2)
            {
                // return to previous menu          
                this->ShiftSurface(this, SURFACE_MAIN, 3.0f, 2u);
            }
            else if(m_GameMenu.GetStatus() == 3)
            {
                // 
                coreUintW iMissionIndex, iSegmentIndex;
                m_GameMenu.RetrievePageData(&iMissionIndex, &iSegmentIndex);
                m_ExtraMenu.SetSelectedTrophy(iMissionIndex, iSegmentIndex);

                // return to previous menu            
                this->ShiftSurface(this, SURFACE_EXTRA, 3.0f, 2u);
            }
        }
        break;

    case SURFACE_SCORE:
        {
            if(m_ScoreMenu.GetStatus() == 1)
            {
                // 
                m_ReplayMenu.ChangeSurface(SURFACE_REPLAY_DETAILS, 0.0f);
                m_ReplayMenu.LoadDownload();
                m_ReplayMenu.ResetNavigator();

                // switch to replay menu
                this->ShiftSurface(this, SURFACE_REPLAY, 3.0f, 1u);
            }
            else if(m_ScoreMenu.GetStatus() == 2)
            {
                // return to previous menu          
                this->ShiftSurface(this, SURFACE_MAIN, 3.0f, 2u);
            }
        }
        break;

    case SURFACE_REPLAY:
        {
            if(m_ReplayMenu.GetStatus() == 1)
            {
                // 
                m_BridgeMenu.EnterGame(true);

                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 1.0f, 0u);

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
            else if(m_ReplayMenu.GetStatus() == 3)
            {
                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, false, true);

                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 2u);
            }
        }
        break;

    case SURFACE_EXTRA:
        {
            if(m_ExtraMenu.GetStatus() == 1)
            {
                // 
                m_GameMenu.ChangeSurface(SURFACE_GAME_ARMORY, 0.0f);
                m_GameMenu.LoadValues();
                m_GameMenu.ResetNavigator();

                // 
                coreUintW iMissionIndex, iSegmentIndex;
                m_ExtraMenu.GetSelectedTrophy(&iMissionIndex, &iSegmentIndex);
                m_GameMenu.SelectTrophy(iMissionIndex, iSegmentIndex);

                // switch to game menu
                this->ShiftSurface(this, SURFACE_GAME, 3.0f, 1u);
            }
            else if(m_ExtraMenu.GetStatus() == 2)
            {
                // return to previous menu          
                this->ShiftSurface(this, SURFACE_MAIN, 3.0f, 2u);
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
                m_BridgeMenu.UseRestart(true);

                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);

                // 
                Core::Audio->CancelSound(SOUND_EFFECT);
                Core::Audio->CancelSound(SOUND_AMBIENT);
            }
            else if(m_PauseMenu.GetStatus() == 3)
            {
                // 
                m_ConfigMenu.ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
                m_ConfigMenu.LoadValues();
                m_ConfigMenu.ResetNavigator();

                // switch to config menu
                this->ShiftSurface(this, SURFACE_CONFIG, 3.0f, 1u);
            }
            else if(m_PauseMenu.GetStatus() == 4)
            {
                if(g_pGame->GetKind() == GAME_KIND_ALL)
                {
                    // 
                    m_BridgeMenu.ReturnMenu(SURFACE_TITLE, true, true);
                }
                else
                {
                    // 
                    m_BridgeMenu.ReturnMenu(m_BridgeMenu.HasUnlocks() ? SURFACE_TITLE : SURFACE_GAME, true, true);

                    // 
                    m_GameMenu.ChangeSurface(SURFACE_GAME_MAIN, 0.0f);
                    m_GameMenu.LoadValues();
                }

                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);

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

                // 
                g_pGame->LoadNextMission();
            }
            else if(m_SummaryMenu.GetStatus() == 2)
            {
                // 
                m_CreditRoll.Start((m_SummaryMenu.GetCurSurface() == SURFACE_SUMMARY_ENDING_NORMAL) ? CREDIT_TYPE_NORMAL : CREDIT_TYPE_SECRET);

                // 
                this->ChangeSurface(SURFACE_CREDITS, 0.0f);

                // 
                g_pGame->CloseMission();
            }
            else if(m_SummaryMenu.GetStatus() == 3)
            {
                // 
                m_BridgeMenu.UseNext();

                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);
            }
            else if(m_SummaryMenu.GetStatus() == 4)
            {
                // 
                m_BridgeMenu.UseRestart(false);

                // 
                this->ShiftSurface(this, SURFACE_BRIDGE, 3.0f, 0u);
            }
            else if(m_SummaryMenu.GetStatus() == 5)
            {
                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, false, true);

                // 
                this->ChangeSurface(SURFACE_BRIDGE, 0.0f);
            }
            else if(m_SummaryMenu.GetStatus() == 6)
            {
                // 
                m_ReplayMenu.ChangeSurface(SURFACE_REPLAY_SLOTS, 0.0f);
                m_ReplayMenu.LoadOverview();
                m_ReplayMenu.ResetNavigator();

                // 
                this->ShiftSurface(this, SURFACE_REPLAY, 3.0f, 1u);
            }
        }
        break;

    case SURFACE_DEFEAT:
        {
            if(m_DefeatMenu.GetStatus() == 1)
            {
                // 
                m_BridgeMenu.UseContinue();

                // 
                this->ChangeSurface(SURFACE_BRIDGE, 0.0f);
            }
            else if(m_DefeatMenu.GetStatus() == 2)
            {
                if(g_pGame->GetKind() == GAME_KIND_ALL)
                {
                    if(g_bDemoVersion)
                    {
                        // 
                        m_FinishMenu.ShowThankYou();

                        // 
                        this->ChangeSurface(SURFACE_FINISH, 0.0f);
                    }
                    else
                    {
                        // 
                        m_SummaryMenu.ShowArcade();

                        // 
                        this->ChangeSurface(SURFACE_SUMMARY, 0.0f);
                    }
                }
                else
                {
                    // 
                    m_BridgeMenu.ReturnMenu(m_BridgeMenu.HasUnlocks() ? SURFACE_TITLE : SURFACE_GAME, false, true);

                    // 
                    m_GameMenu.ChangeSurface(SURFACE_GAME_MAIN, 0.0f);
                    m_GameMenu.LoadValues();

                    // 
                    this->ChangeSurface(SURFACE_BRIDGE, 0.0f);
                }
            }
        }
        break;

    case SURFACE_FINISH:
        {
            if(m_FinishMenu.GetStatus())
            {
                // 
                m_BridgeMenu.ReturnMenu(SURFACE_TITLE, false, true);

                // 
                this->ChangeSurface(SURFACE_BRIDGE, 0.0f);
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
                m_GameMenu.SelectNext();

                // 
                this->__EndGame();
                this->__StartGame();

                // 
                g_pGame->UseNext();
            }
            else if(m_BridgeMenu.GetStatus() == 5)
            {
                // 
                this->ChangeSurface(SURFACE_EMPTY, 0.0f);

                if(g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK)
                {
                    // 
                    g_pReplay->EndPlayback();
                    g_pReplay->RecreateGame();
                    g_pReplay->StartPlayback();
                }
                else
                {
                    // 
                    this->__EndGame();
                    this->__StartGame();
                }

                // 
                g_pGame->UseRestart();
            }
            else if(m_BridgeMenu.GetStatus() == 6)
            {
                // 
                this->ShiftSurface(this, SURFACE_MAIN, 3.0f, 0u);
            }
        }
        break;

    case SURFACE_CREDITS:
        {
            const coreBool bInMenu = (m_CreditRoll.GetType() == CREDIT_TYPE_MENU);

            if(m_CreditRoll.GetFinished() || (bInMenu && g_MenuInput.bCancel))
            {
                // 
                m_CreditRoll.End();

                if(bInMenu)
                {
                    // return to previous menu
                    this->ShiftSurface(this, this->GetOldSurface(), 3.0f, 2u);
                }
                else
                {
                    if(g_pGame->GetKind() == GAME_KIND_ALL)
                    {
                        // 
                        m_SummaryMenu.ShowArcade();

                        // 
                        this->ChangeSurface(SURFACE_SUMMARY, 0.0f);
                    }
                    else
                    {
                        // 
                        m_BridgeMenu.ReturnMenu(SURFACE_TITLE, false, true);

                        // 
                        this->ChangeSurface(SURFACE_BRIDGE, 0.0f);
                    }
                }
            }
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    coreFloat       fPauseAlpha = m_PauseLayer.GetAlpha();
    const coreFloat fPauseSpeed = STATIC_ISVALID(g_pGame) ? 1000.0f : 0.5f;

    // 
    if(((this->GetCurSurface() == SURFACE_CONFIG) || (this->GetCurSurface() == SURFACE_PAUSE)) && STATIC_ISVALID(g_pGame))
         fPauseAlpha = MIN(fPauseAlpha + TIME * fPauseSpeed, 0.25f);
    else fPauseAlpha = MAX(fPauseAlpha - TIME * fPauseSpeed, 0.0f);

    // 
    m_PauseLayer.SetSize     (coreVector2(1.0f,1.0f) * MaxAspectRatio(Core::System->GetResolution()));
    m_PauseLayer.SetAlpha    (fPauseAlpha);
    m_PauseLayer.SetTexOffset(coreVector2(0.0f, MENU_LAYER_TEXOFFSET));
    m_PauseLayer.SetEnabled  (fPauseAlpha ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);

    // 
    m_Tooltip   .Move();
    m_PauseLayer.Move();

    if(!STATIC_ISVALID(g_pGame)) m_bNoticeSavePrevent = false;

    // 
    if(g_pSave->GetActive()) {m_fNoticeSaveTime = m_bNoticeSavePrevent ? 0.0f : 1.0f; g_pSave->ResetActive();}
    else                     {m_fNoticeSaveTime.UpdateMax(-1.0f, 0.0f);}
    
    // 
    m_NoticeSave.SetPosition(coreVector2(-0.033f - 0.02f * BLENDB(1.0f - m_fNoticeSaveTime), 0.023f));
    m_NoticeSave.SetAlpha   (BLENDBR(m_fNoticeSaveTime));
    m_NoticeSave.SetEnabled (m_NoticeSave.GetAlpha() ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);
    m_NoticeSave.Move();

    if((this->GetCurSurface() == SURFACE_SUMMARY) && ((m_SummaryMenu.GetCurSurface() == SURFACE_SUMMARY_SEGMENT_SOLO) || (m_SummaryMenu.GetCurSurface() == SURFACE_SUMMARY_SEGMENT_COOP)) && (!STATIC_ISVALID(g_pGame) || (g_pGame->GetCurMission()->GetID() != cAterMission::ID)))
    {
        m_fVolume.UpdateMax(-0.5f, 0.0f);
    }
    else
    {
        m_fVolume.UpdateMin(0.5f, 1.0f);
    }
    
    
    const eSaveStatus eStatus = g_pSave->GetStatus();
    if((eStatus != SAVE_STATUS_OK) && !STATIC_ISVALID(g_pGame) && !m_MsgBox.IsVisible())
    {
        m_MsgBox.ShowQuestion(PRINT("%s (%s)", Core::Language->GetString("QUESTION_ERROR_SAVE"), Core::Language->GetString(PRINT("ERROR_SAVE_%02u", eStatus))), [](const coreInt32 iAnswer)
        {
            if(iAnswer == MSGBOX_ANSWER_YES)
                 g_pSave->SaveFile();
            else g_pSave->ResetStatus();
        });
    }
    
    
    FOR_EACH(it, s_apNewButton)
    {
        const void* pKey   = (*s_apNewButton.get_key(it));
        coreButton* pValue = (*it);

        if(s_apCurButton[pKey] != pValue)
        {
            s_apCurButton[pKey] = pValue;
            if(pValue && (!pValue->GetCaption() || pValue->GetCaption()->GetAlpha()) && pValue->GetAlpha())   // for summary buttons
            {
                if(TIME) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);
            }
        }
        s_apNewButton[pKey] = NULL;
    }
    
    if(s_pCurTab != s_pNewTab)
    {
        s_pCurTab = s_pNewTab;
        if(s_pCurTab)
        {
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);   // # same sound as button
        }
    }
    s_pNewTab = NULL;
    
    if(s_pCurLine != s_pNewLine)
    {
        s_pCurLine = s_pNewLine;
        if(s_pCurLine && cMenuNavigator::IsUsingAny())
        {
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 0.9f, 1.0f, SOUND_MENU_CHANGE_LINE);
        }
    }
    s_pNewLine = NULL;

    // TODO 1: tooltip has a delay
}


// ****************************************************************
// 
coreBool cMenu::IsPaused()const
{
    return (this->GetCurSurface() != SURFACE_EMPTY)   &&
           (this->GetCurSurface() != SURFACE_REPLAY)  &&
           (this->GetCurSurface() != SURFACE_SUMMARY) &&
           (this->GetCurSurface() != SURFACE_DEFEAT)  &&
           (this->GetCurSurface() != SURFACE_FINISH)  &&
           (this->GetCurSurface() != SURFACE_CREDITS) &&
           (this->GetCurSurface() != SURFACE_BRIDGE || m_BridgeMenu.GetPaused()) && STATIC_ISVALID(g_pGame);
}

coreBool cMenu::IsPausedWithStep()
{
    if(!this->IsPaused() || Core::Manager::Resource->IsLoading()) this->InvokePauseStep();
    return (m_iPauseFrame + 1u < Core::System->GetCurFrame());
}


// ****************************************************************
// 
coreBool cMenu::NeedsCursor()const
{
    return ((this->GetCurSurface() != SURFACE_EMPTY)  &&
            (this->GetCurSurface() != SURFACE_SUMMARY || (this->GetCurSurface() == SURFACE_SUMMARY && (m_SummaryMenu.GetCurSurface() == SURFACE_SUMMARY_SEGMENT_SOLO || m_SummaryMenu.GetCurSurface() == SURFACE_SUMMARY_SEGMENT_COOP))) &&
            (this->GetCurSurface() != SURFACE_DEFEAT) &&
            (this->GetCurSurface() != SURFACE_BRIDGE) &&
            (this->GetCurSurface() != SURFACE_CREDITS || (this->GetCurSurface() == SURFACE_CREDITS && !STATIC_ISVALID(g_pGame)))) ||
           (m_MsgBox.IsVisible());
}


// ****************************************************************
// 
void cMenu::ShiftSurface(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface, const coreFloat fSpeed, const coreUint8 iSound)
{
    ASSERT(pMenu && fSpeed)
    
    const coreUint8 iCurSurface = pMenu->GetCurSurface();

    if(pMenu->ChangeSurface(iNewSurface, 1.0e06f))
    {
        const sMenuInput oSave = g_MenuInput;
        const coreBool   bSave = Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_PRESS);

        g_MenuInput = {};
        Core::Input->ClearMouseButton(CORE_INPUT_LEFT);

        Timeless([&]()
        {
            const coreSet<coreObject2D*>* pList = pMenu->List(iNewSurface);
            FOR_EACH(it, *pList) (*it)->Move();
        });

        g_MenuInput = oSave;
        Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, bSave);

        iForceA = ((pMenu != this) || iCurSurface != SURFACE_INTRO) ? iCurSurface : 0xFFu;
        iForceB = ((pMenu != this) || iCurSurface != SURFACE_INTRO) ? iNewSurface : 0xFFu;

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
    if(s_vHighlightColor == vColor) return;
    s_vHighlightColor = vColor;

    // 
    m_SummaryMenu.SetHighlightColor(vColor);
    m_FinishMenu .SetHighlightColor(vColor);
    m_BridgeMenu .SetHighlightColor(vColor);
}


void cMenu::SetButtonColor(const coreVector3 vColor)
{
    // 
    s_vButtonColor = vColor;
}


// ****************************************************************
// 
coreFloat cMenu::GetVolume()const
{
    return (this->IsPaused() ? 0.3f : 1.0f) * LERPH3(0.5f, 1.0f, m_fVolume);
}


// ****************************************************************
// 
void cMenu::UpdateLanguageFont()
{
    // 
    const coreChar* pcName = Core::Language->HasString("FONT") ? Core::Language->GetString("FONT") : MENU_FONT_STANDARD;
    Core::Manager::Resource->AssignProxy("dynamic_font", pcName);

    // 
    Core::Manager::Resource->UpdateWait();
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
        if(asOutput.empty()) asOutput.emplace("NO LANGUAGE FOUND", "");

        // 
        asOutput.sort_asc();

        // 
        const coreChar* pcKey = "English";
        if(asOutput.count(pcKey))
        {
            coreString sPath = std::move(asOutput.at(pcKey));

            asOutput.erase(pcKey);
            asOutput.emplace(asOutput.begin(), pcKey, std::move(sPath));
        }

        return asOutput;
    }();

    return s_asLanguage;
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
const coreChar* cMenu::GetMissionLetters(const coreUintW iMissionIndex)
{
    // 
    switch(iMissionIndex)
    {
    case  9u: return "X1";
    case 10u: return "X2";
    default:  return coreData::ToChars(iMissionIndex);
    }
}


// ****************************************************************
// 
const coreChar* cMenu::GetSegmentLetters(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    const coreChar* pcMissionLetter = cMenu::GetMissionLetters(iMissionIndex);
    const coreChar* pcSegmentLetter = (iSegmentIndex == 6u) ? "X" : MISSION_SEGMENT_IS_BOSS(iSegmentIndex) ? "B" : coreData::ToChars(iSegmentIndex + 1u);

    // 
    return PRINT("%s-%s", pcMissionLetter, pcSegmentLetter);
}


// ****************************************************************
// 
const coreChar* cMenu::GetStoreText()
{
    static const coreBool bItch = coreData::FileExists("data/other/itch.txt");
    static const coreBool bEpic = coreData::FileExists("data/other/epic.txt");
    return bItch ? "TO_ITCH" : bEpic ? "TO_EPIC" : "TO_STEAM";
}


// ****************************************************************
// 
void cMenu::OpenStoreLink()
{
    static const coreBool bItch = coreData::FileExists("data/other/itch.txt");
    static const coreBool bEpic = coreData::FileExists("data/other/epic.txt");
    SDL_OpenURL(bItch ? "https://mausgames.itch.io/eigengrau" : bEpic ? "https://store.epicgames.com/p/eigengrau-62aef0" : "https://store.steampowered.com/app/1624320/Eigengrau/");
}


// ****************************************************************
// default button update routine
void cMenu::UpdateButton(coreButton* OUTPUT pButton, const void* pMenu, const coreBool bFocused, const coreVector3 vFocusColor, const eMenuUpdate eUpdate)
{
    ASSERT(pButton)

    // 
    if(!s_aButtonData.count_bs(pButton))
    {
        sButtonData oNewData;
        oNewData.vPosition  = pButton->GetPosition();
        oNewData.vSize      = pButton->GetSize();
        oNewData.vTexOffset = pButton->GetTexOffset();
        oNewData.fTime      = 0.0f;
        oNewData.bGrow      = false;

        s_aButtonData.emplace_bs(pButton, oNewData);
    }

    sButtonData& oData = s_aButtonData.at_bs(pButton);

    // 
         if(bFocused)            oData.bGrow = true;
    else if(oData.fTime >= 1.0f) oData.bGrow = false;

    // 
    if(oData.bGrow) oData.fTime.UpdateMin( 20.0f, 1.0f);
               else oData.fTime.UpdateMax(-20.0f, 0.0f);

    // select visible strength
    const coreFloat   fLight = LERPH3(MENU_LIGHT_IDLE,  MENU_LIGHT_ACTIVE, oData.fTime);
    const coreVector3 vColor = LERPH3(COLOR_MENU_WHITE, vFocusColor,       oData.fTime);

    // set button and caption color
                              pButton              ->SetColor3(vColor * fLight);
    if(pButton->GetCaption()) pButton->GetCaption()->SetColor3(vColor * fLight);

    if(!HAS_FLAG(eUpdate, MENU_UPDATE_NO_GROW))
    {
        // 
        const coreVector2 vBasePos = coreVector2(pButton->GetAlignment().x ? oData.vPosition.x : pButton->GetPosition().x, pButton->GetAlignment().y ? oData.vPosition.y : pButton->GetPosition().y);
        const coreFloat   fScale   = LERPH3(1.0f, 1.1f, oData.fTime);

        // 
        pButton->SetPosition     (vBasePos - oData.vSize * pButton->GetAlignment() * (0.5f * (fScale - 1.0f)));
        pButton->SetSize         (oData.vSize * fScale);
        pButton->SetTexOffset    (oData.vTexOffset + (pButton->GetSize() - oData.vSize) * -0.5f);
        pButton->SetFocusModifier(oData.vSize / pButton->GetSize());
        pButton->Move();
    }

    // 
    if(pButton->GetOverride() < 0) pButton->SetAlpha(pButton->GetAlpha() * 0.5f);

    // 
    if(bFocused) s_apNewButton[pMenu] = pButton;

    // 
    if(!HAS_FLAG(eUpdate, MENU_UPDATE_NO_SOUND))
    {
        if(pButton->IsClicked())
        {
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
        }
    }
}

void cMenu::UpdateButton(coreButton* OUTPUT pButton, const void* pMenu, const coreBool bFocused, const eMenuUpdate eUpdate)
{
    // 
    cMenu::UpdateButton(pButton, pMenu, bFocused, s_vButtonColor, eUpdate);
}


// ****************************************************************
// 
void cMenu::UpdateTab(cGuiButton* OUTPUT pTab, const coreBool bLocked, const coreBool bFocused, const coreVector3 vFocusColor, const eMenuUpdate eUpdate)
{
    ASSERT(pTab)

    // 
    if(!s_aButtonData.count_bs(pTab))
    {
        sButtonData oNewData;
        oNewData.vPosition  = pTab->GetPosition();
        oNewData.vSize      = pTab->GetSize();
        oNewData.vTexOffset = pTab->GetTexOffset();
        oNewData.fTime      = 0.0f;
        oNewData.bGrow      = false;

        s_aButtonData.emplace_bs(pTab, oNewData);
    }

    sButtonData& oData = s_aButtonData.at_bs(pTab);

    // 
         if(bLocked || bFocused) oData.bGrow = true;
    else if(oData.fTime >= 1.0f) oData.bGrow = false;

    // 
    if(oData.bGrow) oData.fTime.UpdateMin( 20.0f, 1.0f);
               else oData.fTime.UpdateMax(-20.0f, 0.0f);

    // select visible strength
    const coreFloat   fLight = LERPH3(MENU_LIGHT_IDLE,  MENU_LIGHT_ACTIVE, oData.fTime);
    const coreVector3 vColor = LERPH3(COLOR_MENU_WHITE, vFocusColor,       oData.fTime);

    // set button and caption color
                           pTab              ->SetColor3(vColor * fLight);
    if(pTab->GetCaption()) pTab->GetCaption()->SetColor3(vColor * fLight);

    // 
    const coreFloat fScale = LERPH3(1.0f, 1.2f, oData.fTime);

    // 
    pTab->SetSize     (oData.vSize * coreVector2(1.0f, fScale));
    pTab->SetPosition (oData.vPosition - oData.vSize * pTab->GetAlignment() * (0.5f * (fScale - 1.0f)) + coreVector2(0.0f, (pTab->GetSize().y - oData.vSize.y) * 0.5f));
    pTab->SetTexOffset(oData.vTexOffset + coreVector2(0.0f, (pTab->GetSize().y - oData.vSize.y) * -1.0f));
    pTab->Move();

    // 
    if(pTab->GetOverride() < 0) pTab->SetAlpha(pTab->GetAlpha() * 0.5f);

    // 
    if(!bLocked && bFocused) s_pNewTab = pTab;
}

void cMenu::UpdateTab(cGuiButton* OUTPUT pTab, const coreBool bLocked, const coreBool bFocused, const eMenuUpdate eUpdate)
{
    // 
    cMenu::UpdateTab(pTab, bLocked, bFocused, s_vButtonColor, eUpdate);
}


// ****************************************************************
// 
void cMenu::UpdateSwitchBox(cGuiSwitchBox* OUTPUT pSwitchBox, const eMenuUpdate eUpdate)
{
    ASSERT(pSwitchBox)

    const auto UpdateArrowFunc = [&](coreButton* OUTPUT pArrow, const coreUintW iEndIndex)
    {
        const coreBool bEnd = (pSwitchBox->GetCurIndex() == iEndIndex) || HAS_FLAG(eUpdate, MENU_UPDATE_STATIC);

        // 
        const coreFloat fAlpha = bEnd ? 0.25f           : (pArrow->IsFocused() ? 1.0f              : 0.75f);
        const coreFloat fLight = bEnd ? MENU_LIGHT_IDLE : (pArrow->IsFocused() ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE);

        // 
        pArrow              ->SetAlpha (pSwitchBox->GetAlpha() * fAlpha);
        pArrow              ->SetColor3(COLOR_MENU_WHITE       * fLight);
        pArrow->GetCaption()->SetColor3(COLOR_MENU_WHITE       * fLight);
    };

    // 
    if((pSwitchBox->GetOverride() < 0) && !HAS_FLAG(eUpdate, MENU_UPDATE_STATIC)) pSwitchBox->SetAlpha(pSwitchBox->GetAlpha() * 0.5f);

    // 
    UpdateArrowFunc(pSwitchBox->GetArrow(0u), pSwitchBox->GetEndless() ? ~0u : 0u);
    UpdateArrowFunc(pSwitchBox->GetArrow(1u), pSwitchBox->GetEndless() ? ~0u : (pSwitchBox->GetNumEntries() - 1u));

    // 
    if(!HAS_FLAG(eUpdate, MENU_UPDATE_NO_SOUND))
    {
        const coreInt8 iUserSwitch = pSwitchBox->GetUserSwitch();
        const coreBool bArrowLeft  = pSwitchBox->GetArrow(0u)->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_PRESS);
        const coreBool bArrowRight = pSwitchBox->GetArrow(1u)->IsClicked(CORE_INPUT_LEFT, CORE_INPUT_PRESS);

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
}


// ****************************************************************
// 
void cMenu::UpdateLine(cGuiObject* OUTPUT pLine, const coreBool bInteract, const coreVector3 vFocusColor, const eMenuUpdate eUpdate)
{
    if(!TIME) return;   // for transitions
    if(!pLine->GetAlpha()) return;

    // 
    if(bInteract) pLine->Interact();

    // 
    if(!HAS_FLAG(eUpdate, MENU_UPDATE_STATIC))
    {
        pLine->SetColor3(pLine->IsFocused() ? vFocusColor : coreVector3(1.0f,1.0f,1.0f));
    }

    // 
    if(!HAS_FLAG(eUpdate, MENU_UPDATE_NO_SOUND))
    {
        if(pLine->IsFocused()) s_pNewLine = pLine;
    }
}

void cMenu::UpdateLine(cGuiObject* OUTPUT pLine, const coreBool bInteract, const eMenuUpdate eUpdate)
{
    cMenu::UpdateLine(pLine, bInteract, s_vButtonColor, eUpdate);
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
void cMenu::ApplyMedalTexture(cGuiObject* OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType, const coreBool bHide)
{
    ASSERT(pObject && (iMedal < MEDAL_MAX) && (iMedalType < MEDAL_TYPE_MAX))

    const coreBool bValid = (iMedal != MEDAL_NONE);

    // 
    const coreUint8 iIndex = bValid ? ((iMedal - MEDAL_BRONZE) + iMedalType * MEDAL_DARK) : 15u;
    pObject->SetTexOffset(coreVector2(I_TO_F(iIndex % 4u), I_TO_F(iIndex / 4u)) * 0.25f);

    // 
    pObject->SetEnabled((bValid || !bHide /*|| DEFINED(_CORE_DEBUG_)*/) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    pObject->SetColor3 (coreVector3(1.0f,1.0f,1.0f) * (bValid ? 1.0f : 0.5f));
}


// ****************************************************************
// 
void cMenu::ChangeTab(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface)
{
    ASSERT(pMenu)

    // 
    if(pMenu->ChangeSurface(iNewSurface, 0.0f))
    {
        // 
        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_TAB);
    }
}


// ****************************************************************
// 
void cMenu::ClearButtonTime(cGuiButton* OUTPUT pButton)
{
    if(s_aButtonData.count_bs(pButton))
    {
        sButtonData& oData = s_aButtonData.at_bs(pButton);

        oData.fTime = 0.0f;
        oData.bGrow = false;

        pButton->SetSize(oData.vSize);

                                  pButton              ->SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        if(pButton->GetCaption()) pButton->GetCaption()->SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    }
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
        m_MixObject.Move();
    }
    else
    {
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
    InitFramerate(GetCurUpdateFreq(), GetCurGameSpeed());

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
    oOptions.iFlags       = GAME_FLAG_TASK | GAME_FLAG_FRAGMENT;
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

    if(iKind == GAME_KIND_ALL)
    {
        // 
        if(!g_bDemoVersion)
        {
            g_pReplay->StartRecording();
        }

        // 
        g_pSave->EditLocalStatsArcade()->iCountStart += 1u;
    }
}


// ****************************************************************
// 
void cMenu::__EndGame()
{
    // 
    if(!g_pSave->GetHeader().oProgress.bFirstPlay)
    {
        ADD_BIT_EX(g_pSave->EditProgress()->aiState, STATE_AFTER_FIRST)   // TODO 1: not immediately checked when returning from single stage (wrong order)
    }

    // 
    if(g_pSave->GetHeader().oProgress.bFirstPlay && g_pSave->GetHeader().oProgress.aiAdvance[1])
    {
        g_pSave->EditProgress()->bFirstPlay = false;
        if(!g_bDemoVersion)
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_MAIN_START)
            ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_MAIN_SCORE)
            ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_MAIN_REPLAY)
            ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_MAIN_EXTRA)
        }

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
    g_pReplay->Cancel();

    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    STATIC_DELETE(g_pGame)

    // 
    InitFramerate();
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