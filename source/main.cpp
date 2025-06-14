///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"
#include "additional/switch/main.cpp"

coreVector2     g_vGameResolution = coreVector2(0.0f,0.0f);
coreDouble      g_adGameTime[2]   = {};
coreVector2     g_vHudDirection   = coreVector2(0.0f,1.0f);
coreBool        g_bTiltMode       = false;
coreFloat       g_fShiftMode      = 0.0f;
coreBool        g_bDemoVersion    = false;
coreBool        g_bLeaderboards   = false;
coreBool        g_bSteamDeck      = false;
coreBool        g_bHandheld       = false;
coreBool        g_bDebugOutput    = false;
coreMusicPlayer g_MusicPlayer     = {};

STATIC_MEMORY(cReplay,         g_pReplay)
STATIC_MEMORY(cSave,           g_pSave)
STATIC_MEMORY(cOutline,        g_pOutline)
STATIC_MEMORY(cGlow,           g_pGlow)
STATIC_MEMORY(cDistortion,     g_pDistortion)
STATIC_MEMORY(cWindscreen,     g_pWindscreen)
STATIC_MEMORY(cSpecialEffects, g_pSpecialEffects)
STATIC_MEMORY(cPostProcessing, g_pPostProcessing)
STATIC_MEMORY(cForeground,     g_pForeground)
STATIC_MEMORY(cEnvironment,    g_pEnvironment)
STATIC_MEMORY(cMenu,           g_pMenu)
STATIC_MEMORY(cGame,           g_pGame)

static coreProtect<coreUint64> s_iOldPerfTime  = 0u;      // last measured high-precision time value
static coreProtect<coreFloat>  s_fLogicalRate  = 0.0f;    // logical frame rate
static coreProtect<coreFloat>  s_fLogicalTime  = 0.0f;    // logical frame time (simulation rate)
static coreProtect<coreDouble> s_dPhysicalTime = 0.0;     // physical frame time (display rate)
static coreBool                s_bUseLogical   = false;   // 
static coreBool                s_bMenuMusic    = false;   // 

static void LockFramerate();                                                // lock frame rate
static void ForceFramerate(const coreBool bFull, const coreBool bUpdate);   // override frame time
static void UpdateListener();                                               // 
static void ReshapeGame();                                                  // reshape and resize game
static void DebugGame();                                                    // debug and test game


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // set camera to default values
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);

    // set view frustum to default values
    Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f, 0.0f);

    // set listener to default values
    Core::Audio->SetListener(LISTENER_POSITION, LISTENER_VELOCITY, coreVector3(0.0f,0.0f,-1.0f), coreVector3(0.0f,1.0f,0.0f));

    // load configuration
    LoadConfig();

    // check for demo version (game would crash otherwise anyway)
    g_bDemoVersion = CoreApp::Settings::IsDemo();

    // 
    g_bLeaderboards = !std::strcmp(Core::Platform->GetIdentifier(), "Steam") || DEFINED(_CORE_SWITCH_);

    // 
    InitAchievements();
    InitLeaderboards();

    // load available music files (manually ordered)
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_00"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_01"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_02"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_03"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_04_reverse"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_04"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_05"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_06"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_07"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_08_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_08_loop"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/boss_99"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/ending_normal"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/ending_secret"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/menu"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_00_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_00_loop"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_01_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_01_loop"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_02_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_02_loop"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_03_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_03_loop"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_04_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_04_loop"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_05"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_06_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_06_loop"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_07_intro"));
    g_MusicPlayer.AddMusicFile(__MUSIC_FILE("data/music/mission_07_loop"));

    for(coreUintW i = 0u, ie = g_MusicPlayer.GetNumMusic(); i < ie; ++i)
    {
        coreMusic* pMusic = g_MusicPlayer.GetMusic(i);
        pMusic->SetLoop(std::strstr(pMusic->GetPath(), "_intro") == NULL);
    }

    g_MusicPlayer.StartThread();

    // init system properties
    InitResolution(Core::System->GetResolution());
    InitDirection();
    InitFramerate();

    // create and init main components
    cShadow::GlobalInit();
    STATIC_NEW(g_pReplay)
    STATIC_NEW(g_pSave)
    STATIC_NEW(g_pOutline)
    STATIC_NEW(g_pGlow)
    STATIC_NEW(g_pDistortion)
    STATIC_NEW(g_pWindscreen)
    STATIC_NEW(g_pSpecialEffects)
    STATIC_NEW(g_pPostProcessing)
    STATIC_NEW(g_pForeground)
    STATIC_NEW(g_pEnvironment)
    STATIC_NEW(g_pMenu)
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // 
    STATIC_DELETE(g_pGame)

    // delete and exit main components
    STATIC_DELETE(g_pMenu)
    STATIC_DELETE(g_pEnvironment)
    STATIC_DELETE(g_pForeground)
    STATIC_DELETE(g_pPostProcessing)
    STATIC_DELETE(g_pSpecialEffects)
    STATIC_DELETE(g_pWindscreen)
    STATIC_DELETE(g_pDistortion)
    STATIC_DELETE(g_pGlow)
    STATIC_DELETE(g_pOutline)
    STATIC_DELETE(g_pSave)
    STATIC_DELETE(g_pReplay)
    cShadow::GlobalExit();

    // unload music files
    g_MusicPlayer.KillThread();
    g_MusicPlayer.ClearMusic();

    // save configuration
    SaveConfig();
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    // prevent menu transition flickering   
    if(Core::Graphics->IsFrameSkipped())
    {
        glDisable(GL_DEPTH_TEST);
        {
            // always render to properly handle transitions
            g_pMenu->Render();   // TODO 1: handle mirror mode ? or remove everywhere
        }
        glEnable(GL_DEPTH_TEST);

        ForceFramerate(false, false);
        return;
    }
    
    Core::Debug->MeasureStart("Update Always");
    {
        const coreVector3 vOldCamPos = Core::Graphics->GetCamPosition();
        const coreVector3 vOldCamOri = Core::Graphics->GetCamOrientation();

        Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);   // for tilt-mode

        // 
        g_pSpecialEffects->Update();

        Core::Graphics->SetCamera(vOldCamPos, CAMERA_DIRECTION, vOldCamOri);
    }
    Core::Debug->MeasureEnd("Update Always");

    if(!g_pMenu->IsPausedWithStep())
    {
        Core::Debug->MeasureStart("Update");
        {
            if(!g_pMenu->IsPaused())
            {
                // update glow- and distortion-effect
                g_pGlow      ->Update();
                g_pDistortion->Update();

                // update the shadow map class
                cShadow::GlobalUpdate();
            }
        }
        Core::Debug->MeasureEnd("Update");
        Core::Debug->MeasureStart("Environment");
        {
            // render the environment
            g_pEnvironment->Render();
        }
        Core::Debug->MeasureEnd("Environment");
        Core::Debug->MeasureStart("Foreground");
        {
            if((STATIC_ISVALID(g_pGame) || g_pSpecialEffects->IsActive() || g_pWindscreen->IsActive()) && !g_bTiltMode)
            {
                // create foreground frame buffer
                g_pForeground->Start();
                {
                    if(STATIC_ISVALID(g_pGame))
                    {
                        // render the game
                        g_pGame->Render();
                    }
                    else
                    {
                        // render special-effects
                        g_pSpecialEffects->Render();
                    }

                    // 
                    g_pWindscreen->Render();
                }
                g_pForeground->End();
            }
            else
            {
                // clear the foreground
                g_pForeground->Clear();
            }
        }
        Core::Debug->MeasureEnd("Foreground");
    }

    glDisable(GL_DEPTH_TEST);
    {
        // 
        if(g_CurConfig.Game.iMirrorMode) glDisable(GL_CULL_FACE);

        Core::Debug->MeasureStart("Post Processing");
        {
            // render post-processing
            g_pPostProcessing->Render();

            if(g_bTiltMode)
            {
                if(STATIC_ISVALID(g_pGame))
                {
                    if(g_CurConfig.Game.iMirrorMode) glEnable(GL_CULL_FACE);

                    glEnable(GL_DEPTH_TEST);
                    {
                        g_pGame->Render();
                    }
                    glDisable(GL_DEPTH_TEST);

                    g_pPostProcessing->RenderTilt();

                    if(g_CurConfig.Game.iMirrorMode) glDisable(GL_CULL_FACE);
                }
            }
        }
        Core::Debug->MeasureEnd("Post Processing");
        Core::Debug->MeasureStart("Interface");
        {
            // 
            const coreMatrix4 mOldOrtho = Core::Graphics->GetOrtho();
            if(g_CurConfig.Game.iMirrorMode >= 2u) c_cast<coreMatrix4&>(Core::Graphics->GetOrtho()) = coreMatrix4::Scaling(IsHorizontal(g_vHudDirection) ? coreVector3(1.0f,-1.0f,1.0f) : coreVector3(-1.0f,1.0f,1.0f)) * mOldOrtho;

            // render the overlay separately
            if(STATIC_ISVALID(g_pGame)) g_pGame->RenderOverlay();

            // render the menu
            g_pMenu->Render();

            // 
            if(g_CurConfig.Game.iMirrorMode >= 2u) c_cast<coreMatrix4&>(Core::Graphics->GetOrtho()) = mOldOrtho;
        }
        Core::Debug->MeasureEnd("Interface");

        // 
        if(g_CurConfig.Game.iMirrorMode) glEnable(GL_CULL_FACE);
    }
    glEnable(GL_DEPTH_TEST);

    // override frame time (again, because of freeze)
    ForceFramerate(false, false);
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    // reshape and resize game
    if(Core::System->GetWinSizeChanged()) ReshapeGame();
    
    s_bUseLogical = STATIC_ISVALID(g_pGame) &&
                    !HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED) &&
                    (g_pGame->GetCurMission()->GetCurSegmentIndex() != MISSION_NO_SEGMENT) &&
                    (!g_pGame->GetCurMission()->GetCurBoss() || !g_pGame->GetCurMission()->GetCurBoss()->HasStartup()) &&
                    !g_pGame->GetInterface()->GetFakeEnd();

    // lock frame rate and override frame time
    LockFramerate();
    ForceFramerate(true, false);

    
    static coreFlow fBorder = 0.0f;
    if(STATIC_ISVALID(g_pGame)) fBorder.UpdateMin( 0.3f, 1.0f);
                           else fBorder.UpdateMax(-0.3f, 0.0f);
    g_pPostProcessing->SetBorderAll(LERPH3(POST_DEFAULT_BORDER_MIN, POST_DEFAULT_BORDER_MAX, fBorder));
    

    Core::Debug->MeasureStart("Move");
    {
        // update input interface
        UpdateInput();

        // move the menu
        g_pMenu->Move();

        // 
        Core::Manager::Object->SetUpdateCollision(!g_pMenu->IsPaused());

        if(!g_pMenu->IsPaused())
        {
            // 
            ForceFramerate(true, true);
            
                g_pDistortion->Move();

            // move the environment
            g_pEnvironment->Move();

            if(STATIC_ISVALID(g_pGame))
            {
                // move the game
                g_pGame->Move();

                // move the overlay separately
                g_pGame->MoveOverlay();
            }

            // 
            g_pWindscreen->Move();

            // move special-effects
            g_pSpecialEffects->Move();
        }

        // 
        g_pSpecialEffects->MoveAlways();

        // 
        if(STATIC_ISVALID(g_pGame)) g_pGame->MoveAlways();

        // move post-processing
        g_pPostProcessing->Move();   // TODO 1: splitscreen value should not be updated outside of pause, but rest should
        
        
        if(!STATIC_ISVALID(g_pGame) && !g_pSave->GetHeader().oProgress.bFirstPlay)
        {
            if(!s_bMenuMusic)
            {
                g_MusicPlayer.SelectName("menu");
                s_bMenuMusic = true;
            }
            
            g_MusicPlayer.SetVolume(1.0f * MUSIC_VOLUME);
            g_MusicPlayer.Play();
        }
        else s_bMenuMusic = false;

        // update the music-player
        g_MusicPlayer.Update();

        // 
        CheckAchievements();
        CheckLeaderboards();

        // 
        UpdateListener();
    }
    Core::Debug->MeasureEnd("Move");

    // 
    if(STATIC_ISVALID(g_pGame))
    {
        const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
        const coreUintW iSegmentIndex = g_pGame->GetCurMission()->GetCurSegmentIndex();

        if(iSegmentIndex != MISSION_NO_SEGMENT)
        {
            Core::Platform->SetRichValue("SEGMENT", cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex));
            Core::Platform->SetRichText ("Having fun!", "#game");
        }
    }
    else
    {
        Core::Platform->SetRichText("Getting ready!", "#menu");
    }

    // 
    Core::Platform->SetGameState(STATIC_ISVALID(g_pGame) ? ((HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY) && !g_pMenu->IsPaused()) ? CORE_PLATFORM_STATE_ACTIVE : CORE_PLATFORM_STATE_INACTIVE) : CORE_PLATFORM_STATE_MENU);
    
    
    cBackground* pCurBackground = g_pEnvironment->GetBackground();
    cBackground* pOldBackground = g_pEnvironment->GetOldBackground();

    const coreInt32 iCurID = pCurBackground->GetID();
    const coreInt32 iOldID = pOldBackground ? pOldBackground->GetID() : cNoBackground::ID;

    coreFloat fCurFactor = 0.0f;
    coreFloat fOldFactor = 0.0f;

         if(iCurID == cMossBackground   ::ID) fCurFactor = d_cast<cMossBackground*>   (pCurBackground)->GetHeadlight()->GetAlpha();
    else if(iCurID == cStomachBackground::ID) fCurFactor = d_cast<cStomachBackground*>(pCurBackground)->GetHeadlight()->GetAlpha();
    else if(iCurID == cDarkBackground   ::ID) fCurFactor = d_cast<cDarkBackground*>   (pCurBackground)->GetHeadlight()->GetAlpha();

         if(iOldID == cMossBackground   ::ID) fOldFactor = d_cast<cMossBackground*>   (pOldBackground)->GetHeadlight()->GetAlpha();
    else if(iOldID == cStomachBackground::ID) fOldFactor = d_cast<cStomachBackground*>(pOldBackground)->GetHeadlight()->GetAlpha();
    else if(iOldID == cDarkBackground   ::ID) fOldFactor = d_cast<cDarkBackground*>   (pOldBackground)->GetHeadlight()->GetAlpha();

    coreVector2 vTarget = coreVector2(0.0f,0.0f);
    if(STATIC_ISVALID(g_pGame))
    {
        g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
        {
            vTarget += pPlayer->GetPosition().xy();
        });
        vTarget *= RCP(I_TO_F(g_pGame->GetNumPlayers()));
    }

    g_pEnvironment->SetTargetSide(vTarget * (0.1f * LERP(fOldFactor, fCurFactor, g_pEnvironment->RetrieveTransitionBlend(pCurBackground))), 10.0f);
    
    

    // debug and test game
    if(Core::Debug->IsEnabled()) DebugGame();
}


// ****************************************************************
// init resolution properties
void InitResolution(const coreVector2 vResolution)
{
    // calculate biggest possible 1:1 resolution
    g_vGameResolution = coreVector2(1.0f,1.0f) * vResolution.Min();

    // 
    if(F_TO_UI(vResolution.x + vResolution.y) & 0x01u) g_vGameResolution.arr(IsHorizontal(vResolution) ? 0u : 1u) -= 1.0f;

    // 
    g_bSteamDeck = coreMath::IsNear(vResolution.AspectRatio(), coreVector2(1280.0f,800.0f).AspectRatio());
}


// ****************************************************************
// 
void InitDirection()
{
    // 
    switch(g_CurConfig.Game.iHudDirection)
    {
    default: g_vHudDirection = coreVector2( 0.0f, 1.0f); break;
    case 1u: g_vHudDirection = coreVector2(-1.0f, 0.0f); break;
    case 2u: g_vHudDirection = coreVector2( 0.0f,-1.0f); break;
    case 3u: g_vHudDirection = coreVector2( 1.0f, 0.0f); break;
    }

    // 
    Core::Manager::Object->SetSpriteViewDir  (g_vHudDirection);
    Core::Manager::Object->SetSpriteAltCenter(g_vGameResolution);
    Core::Manager::Object->RefreshSprites();
}


// ****************************************************************
// init frame rate properties
void InitFramerate(const coreUint16 iUpdateFreq, const coreUint8 iGameSpeed)
{
    // get current display mode
    const SDL_DisplayMode* pMode     = SDL_GetCurrentDisplayMode(Core::System->GetDisplayData(Core::System->GetDisplayIndex()).iDisplayID);
    coreUint16             iModeRate = pMode ? F_TO_UI(ROUND(pMode->refresh_rate)) : 0u;

#if defined(_CORE_EMSCRIPTEN_) || defined(_CORE_SWITCH_)
    if(!iModeRate) iModeRate = 60;
#endif

    // 
    const coreUint32 iRefreshRate = (iModeRate >= F_TO_UI(FRAMERATE_MIN)) ? iModeRate : SCORE_PURE_UPDATEFREQ;

    // calculate logical and physical frame time
    if(!STATIC_ISVALID(g_pGame))
    {
        const coreDouble dUpdateFreq = coreDouble(iUpdateFreq ? iUpdateFreq : iRefreshRate);
        const coreDouble dGameSpeed  = coreDouble(iGameSpeed  ? iGameSpeed  : 100u) / 100.0;

        const coreDouble dRawRate   = dUpdateFreq / dGameSpeed;
        const coreDouble dFixedRate = MIN(CEIL(coreDouble(FRAMERATE_MIN) / dRawRate) * dRawRate, coreDouble(FRAMERATE_MAX));   // increase in multiples, if below minimum frame rate

        s_fLogicalRate  = coreFloat(dFixedRate);
        s_fLogicalTime  = coreFloat(1.0 / dFixedRate);
        s_dPhysicalTime = 1.0 / ROUND(dFixedRate * dGameSpeed);

        const coreUint8 iSkip = iModeRate ? (F_TO_UI(ROUND(dFixedRate * dGameSpeed)) / iModeRate) : 0u;
        Core::Graphics->SetSkipFrame(iSkip ? (iSkip - 1u) : 0u);

        g_adGameTime[0] = 1.0 / dFixedRate;
        g_adGameTime[1] = s_dPhysicalTime;
    }

    // override vertical synchronization
    ASSERT(s_dPhysicalTime)
    if(Core::Config->GetInt(CORE_CONFIG_SYSTEM_VSYNC) && (iModeRate * (1u + Core::Graphics->GetSkipFrame()) == F_TO_UI(1.0 / s_dPhysicalTime)))
    {
        SDL_GL_SetSwapInterval(1);
    }
    else
    {
        SDL_GL_SetSwapInterval(0);
    }

    // 
    Core::Log->Info("Frame rate properties configured (refresh rate %u, logical rate %.1f, physical rate %.1f)", iRefreshRate, coreFloat(s_fLogicalRate), 1.0 / coreDouble(s_dPhysicalTime));
}


// ****************************************************************
// 
coreFloat RoundFreq(const coreFloat fFreq)
{
    ASSERT((fFreq > 0.0f) && (fFreq <= FRAMERATE_MIN))

    const coreFloat fLocal = s_fLogicalRate;
    return fLocal / ROUND(fLocal / fFreq);   // # normal division
}


// ****************************************************************
// 
coreVector2 CalcFinalDirection()
{
    const coreVector2 vGame  = g_pPostProcessing->GetDirection();
    const coreVector2 vHud   = g_vHudDirection;
    const coreVector2 vFinal = MapToAxisInv(vGame, vHud);
    ASSERT(vFinal.IsNormalized())

    return vFinal;
}


// ****************************************************************
// lock frame rate
static void LockFramerate()
{
#if !defined(_CORE_EMSCRIPTEN_) && !defined(_CORE_SWITCH_)

    if(!Core::Debug->IsEnabled() && !SDL_GL_GetSwapIntervalInline())
    {
        coreUint64 iNewPerfTime;
        coreDouble dDifference;

        // 
        const coreUint64 iLocalOldPerfTime  = s_iOldPerfTime;
        const coreDouble dLocalPhysicalTime = s_dPhysicalTime;

        // measure and calculate current frame time
        const auto nMeasureFunc = [&]()
        {
            iNewPerfTime = SDL_GetPerformanceCounter();
            dDifference  = coreDouble(iNewPerfTime - iLocalOldPerfTime) * Core::System->GetPerfFrequency();
        };

        // spin as long as frame time is too low
        for(nMeasureFunc(); dDifference < dLocalPhysicalTime; nMeasureFunc())
        {
            (void)dDifference;

            // 
            constexpr coreUint32 iMargin = 1u;

            // sleep (once) to reduce overhead
            const coreUint32 iSleep = MAX(F_TO_UI((dLocalPhysicalTime - dDifference) * 1000.0), iMargin) - iMargin;
            if(iSleep) SDL_Delay(iSleep);

            // 
            else CORE_SPINLOCK_YIELD
        }

        // save last high-precision time value
        s_iOldPerfTime = iNewPerfTime;
    }

#else

    (void)s_iOldPerfTime;

#endif
}


// ****************************************************************
// override frame time
static void ForceFramerate(const coreBool bFull, const coreBool bUpdate)
{
    if(STATIC_ISVALID(g_pGame) && !g_pMenu->IsPaused())
    {
        // 
        if(!DEFINED(_CORE_DEBUG_) || TIME) c_cast<coreFloat&>(TIME) = s_bUseLogical ? coreFloat(s_fLogicalTime) : coreFloat(s_dPhysicalTime);

        if(bFull)
        {
            // 
            coreFloat& fFreezeTime = c_cast<coreFloat&>(g_pSpecialEffects->GetFreezeTime());
            if(fFreezeTime)
            {
                if(bUpdate) fFreezeTime = MAX0(fFreezeTime - TIME);
                c_cast<coreFloat&>(TIME) *= 0.001f;
            }

            // 
            coreFloat& fSlowTime     = c_cast<coreFloat&>(g_pSpecialEffects->GetSlowTime());
            coreFloat& fSlowStrength = c_cast<coreFloat&>(g_pSpecialEffects->GetSlowStrength());
            if(fSlowTime || fSlowStrength)
            {
                if(bUpdate) fSlowTime     = MAX0(fSlowTime - TIME);
                if(bUpdate) fSlowStrength = fSlowTime ? 1.0f : MAX0(fSlowStrength - 0.3f * TIME);
                c_cast<coreFloat&>(TIME) *= LERPH3(1.0f, 0.1f, fSlowStrength);
            }
        }
    }
}


// ****************************************************************
// 
static void UpdateListener()
{
    // 
    const coreFloat   fSide  = (g_CurConfig.Game.iMirrorMode == 1u) ? 1.0f : -1.0f;
    const coreVector2 vFinal = CalcFinalDirection();

    // 
    Core::Audio->SetListener(LISTENER_POSITION, LISTENER_VELOCITY, coreVector3(0.0f, 0.0f, fSide), coreVector3(vFinal, 0.0f));
}


// ****************************************************************
// reshape and resize game
static void ReshapeGame()
{
    // update system properties
    InitResolution(Core::System->GetResolution());
    InitDirection();
    InitFramerate();

    // reshape engine
    Core::Reshape();
    
    
    if(STATIC_ISVALID(g_pGame))   // TODO 1: should only happen when dragging, but also happens with normal resolution changes through the options menu (GetWinSizeChanged is TRUE in both cases)
    {
        g_pGame->GetInterface ()->UpdateLayout();
        g_pGame->GetInterface ()->UpdateSpacing();
        g_pGame->GetInterface ()->MoveTimeless();
        g_pGame->GetCombatText()->UpdateLayout();

        if(STATIC_ISVALID(g_pGame) && (g_pGame->GetCurMission()->GetID() == cIntroMission::ID))
        {
            Timeless([]()
            {
                g_pGame->GetCurMission()->MoveAfter();   // update manuals
            });
            g_pMenu->InvokePauseStep();
        }
    }
    
    if(STATIC_ISVALID(g_pGame))    // usually only during a game   // TODO 1: this is called during Move()
    {
    // refresh frame-buffers        
    g_pGlow->Update();
    Timeless([]()
    {
        g_pDistortion->Update();
    });
    coreFrameBuffer::EndDraw();
    }
    

    // 
    g_pMenu->InvokePauseStep();
}


// ****************************************************************
// debug and test game
static void DebugGame()
{
#if defined(_P1_DEBUG_INPUT_)

    // start game
    if(!STATIC_ISVALID(g_pGame))
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LALT), CORE_INPUT_HOLD))
        {
            sGameOptions oOptions = {};
            oOptions.iKind        = GAME_KIND_ALL;
            oOptions.iType        = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(X), CORE_INPUT_HOLD) ? GAME_TYPE_COOP : GAME_TYPE_SOLO;
            oOptions.iMode        = GAME_MODE_STANDARD;
            oOptions.iDifficulty  = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(S), CORE_INPUT_HOLD) ? GAME_DIFFICULTY_HARD : Core::Input->GetKeyboardButton(CORE_INPUT_KEY(V), CORE_INPUT_HOLD) ? GAME_DIFFICULTY_EASY : GAME_DIFFICULTY_NORMAL;
            oOptions.iFlags       = GAME_FLAG_TASK | GAME_FLAG_FRAGMENT;
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                oOptions.aiShield  [i]    = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(C), CORE_INPUT_HOLD) ? 20u : 0u;
                oOptions.aaiWeapon [i][0] = cRayWeapon::ID;
                oOptions.aaiSupport[i][0] = 0u;
            }

            const coreInt32* piMissionList = g_bDemoVersion ?            GAME_MISSION_LIST_DEMO  :            GAME_MISSION_LIST_MAIN;
            const coreUintW  iNumMissions  = g_bDemoVersion ? ARRAY_SIZE(GAME_MISSION_LIST_DEMO) : ARRAY_SIZE(GAME_MISSION_LIST_MAIN);

            InitFramerate(GetCurUpdateFreq(), GetCurGameSpeed());

            #define __LOAD_GAME(x) {STATIC_NEW(g_pGame, oOptions, piMissionList, iNumMissions) g_pGame->LoadMissionID(x); g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f); g_pPostProcessing->SetWallOpacity(1.0f); g_pEnvironment->Activate(); g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);}
                 if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1),     CORE_INPUT_PRESS)) __LOAD_GAME(cIntroMission  ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2),     CORE_INPUT_PRESS)) __LOAD_GAME(cViridoMission ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3),     CORE_INPUT_PRESS)) __LOAD_GAME(cNevoMission   ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4),     CORE_INPUT_PRESS)) __LOAD_GAME(cHarenaMission ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(5),     CORE_INPUT_PRESS)) __LOAD_GAME(cRutilusMission::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(6),     CORE_INPUT_PRESS)) __LOAD_GAME(cGeluMission   ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(7),     CORE_INPUT_PRESS)) __LOAD_GAME(cCalorMission  ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(8),     CORE_INPUT_PRESS)) __LOAD_GAME(cMuscusMission ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(9),     CORE_INPUT_PRESS)) __LOAD_GAME(cAterMission   ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(0),     CORE_INPUT_PRESS)) __LOAD_GAME(cErrorMission  ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(MINUS), CORE_INPUT_PRESS)) __LOAD_GAME(cBonus1Mission ::ID)
            #undef __LOAD_GAME
        }
    }

    // end game
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(K), CORE_INPUT_PRESS))
    {
        STATIC_DELETE(g_pGame)
    }

    // hide menu
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(J), CORE_INPUT_PRESS))
    {
        static coreUint8 s_iOldSurface = SURFACE_EMPTY;
        if(g_pMenu->GetCurSurface() == SURFACE_EMPTY)
        {
            g_pMenu->ChangeSurface(s_iOldSurface, 0.0f);
        }
        else
        {
            s_iOldSurface = g_pMenu->GetCurSurface();
            g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f);
        }
    }

    // load background
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LSHIFT), CORE_INPUT_HOLD))
    {
             if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cCloudBackground  ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cGrassBackground  ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cSeaBackground    ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cDesertBackground ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(5), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cSpaceBackground  ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(6), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cVolcanoBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(7), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cSnowBackground   ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(8), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cMossBackground   ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(9), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cDarkBackground   ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(0), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cNoBackground     ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
    }

    // set background movement
    if(!Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PERIOD), CORE_INPUT_HOLD))
    {
             if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_1),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f,-1.0f).Normalized(), 10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_2),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f,-1.0f),              10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_3),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f,-1.0f).Normalized(), 10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_4),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 0.0f),              10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_6),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 0.0f),              10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_7),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 1.0f).Normalized(), 10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_8),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f, 1.0f),              10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_9),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 1.0f).Normalized(), 10.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PLUS),     CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeedNow((&g_pEnvironment->GetSpeed())[1] + 1.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MINUS),    CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeedNow((&g_pEnvironment->GetSpeed())[1] - 1.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_DIVIDE),   CORE_INPUT_HOLD))  g_pEnvironment->SetTargetSideNow ((&g_pEnvironment->GetSide ())[1] - coreVector2(30.0f * TIME, 0.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MULTIPLY), CORE_INPUT_HOLD))  g_pEnvironment->SetTargetSideNow ((&g_pEnvironment->GetSide ())[1] + coreVector2(30.0f * TIME, 0.0f));
    }

    // set background height
    static coreFloat s_fHeight = 0.0f;
         if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(R), CORE_INPUT_HOLD)) s_fHeight += 15.0f * TIME;
    else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(T), CORE_INPUT_HOLD)) s_fHeight -= 15.0f * TIME;
    g_pEnvironment->SetTargetHeightNow(s_fHeight);

    // set background interpolation
    if(g_pEnvironment->GetBackground()->GetOutdoor())
    {
        static coreFloat s_fMul = 1.0f;
        static coreFloat s_fAdd = 0.0f;

        coreBool bChange = false;

             if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(M),      CORE_INPUT_HOLD)) {s_fMul += 0.05f * TIME; bChange = true;}
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(N),      CORE_INPUT_HOLD)) {s_fMul -= 0.05f * TIME; bChange = true;}
             if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PERIOD), CORE_INPUT_HOLD)) {s_fAdd += 1.0f  * TIME; bChange = true;}
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(COMMA),  CORE_INPUT_HOLD)) {s_fAdd -= 1.0f  * TIME; bChange = true;}

        if(bChange)
        {
            g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeightNow(s_fMul, s_fAdd);

            Core::Debug->InspectValue("Env Mul", s_fMul);
            Core::Debug->InspectValue("Env Add", s_fAdd);
        }
    }

    // equip weapon
    if(STATIC_ISVALID(g_pGame))
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LCTRL), CORE_INPUT_HOLD))
        {
                 if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cRayWeapon  ::ID);
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cPulseWeapon::ID);
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cWaveWeapon ::ID);
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cTeslaWeapon::ID);
        }
    }

    // toggle invincibility
    if(STATIC_ISVALID(g_pGame))
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(Z), CORE_INPUT_PRESS))
        {
            static coreBool s_bInvincible = false;
            s_bInvincible = !s_bInvincible;
            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetMaxHealth(s_bInvincible ? 100 : PLAYER_LIVES);
                pPlayer->SetCurHealth(s_bInvincible ? 100 : PLAYER_LIVES);
            });
        }
    }

    // damage enemies
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(I), CORE_INPUT_PRESS))
    {
        if(STATIC_ISVALID(g_pGame))
        {
            g_pGame->GetEnemyManager()->ForEachEnemy([](cEnemy* OUTPUT pEnemy)
            {
                const coreBool bInvincible = pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE);
                pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->TakeDamage(pEnemy->HasStatus(ENEMY_STATUS_BOSS) ? 400 : 20, ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f), NULL, true);

                if(bInvincible) pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        }
    }

    // damage boss
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(O), CORE_INPUT_PRESS))
    {
        if(STATIC_ISVALID(g_pGame) && g_pGame->GetCurMission()->GetCurBoss())
        {
            g_pGame->GetCurMission()->GetCurBoss()->TakeDamage(400, ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f), NULL, true);
        }
    }

    // skip stage
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(P), CORE_INPUT_PRESS))
    {
        if(STATIC_ISVALID(g_pGame) &&
           (!g_pGame->GetCurMission()->GetBoss(0u) || g_pGame->GetCurMission()->GetBoss(0u)->HasStatus(ENEMY_STATUS_DEAD)) &&
           (!g_pGame->GetCurMission()->GetBoss(1u) || g_pGame->GetCurMission()->GetBoss(1u)->HasStatus(ENEMY_STATUS_DEAD)))
        {
            c_cast<coreUintW&>(g_pGame->GetCurMission()->GetCurSegmentIndex()) = MISSION_NO_SEGMENT;
            g_pGame->GetCurMission()->SkipStage();
            g_pGame->GetBulletManagerEnemy()->ClearBullets(false);
        }
    }

    // load boss
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PERIOD), CORE_INPUT_HOLD))
    {
        static coreString s_sCode;
        for(coreUintW i = 0u; i < 10u; ++i)
        {
            if(Core::Input->GetKeyboardButton(coreInputKey(CORE_INPUT_KEY(KP_1) + i), CORE_INPUT_PRESS))
            {
                s_sCode += coreChar('0' + ((i+1u) % 10u));
                Core::Debug->InspectValue("Boss Code", s_sCode.c_str());
            }
        }

        if(s_sCode.size() >= 4u)
        {
            // TODO 3 
            s_sCode.clear();
        }
    }

    // show debug output
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(F), CORE_INPUT_PRESS))
    {
        g_bDebugOutput = !g_bDebugOutput;
    }

    // keep noise low
    if(SDL_GL_GetSwapIntervalInline())
        SDL_Delay(1u);

    if(STATIC_ISVALID(g_pGame))
    {
        Core::Debug->InspectValue("Bullets", coreUint32(g_pGame->GetBulletManagerPlayer()->GetNumBullets() + g_pGame->GetBulletManagerEnemy()->GetNumBullets()));
        Core::Debug->InspectValue("Enemies", coreUint32(Core::Manager::Object->GetObjectList(TYPE_ENEMY).size()));
    }

    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(G), CORE_INPUT_PRESS))
    {
        //g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
        //g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
        g_pSpecialEffects->CreateExplosion(coreVector3(0.0f,0.0f,0.0f));
        g_pSpecialEffects->SlowScreen(4.0f);
    }

    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(B), CORE_INPUT_PRESS))
    {
        if(STATIC_ISVALID(g_pGame))
        {
            g_pGame->GetPlayer(0u)->SetMaxHealth(1);
            g_pGame->GetPlayer(0u)->TakeDamage(1, ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f));
        }
        
        //if(g_pEnvironment->GetBackground()->GetID() == cDarkBackground::ID)
        //{
        //    d_cast<cDarkBackground*>(g_pEnvironment->GetBackground())->Dissolve();
        //}
        
        //if(STATIC_ISVALID(g_pGame)) g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
        
        //static coreBool bDense = false;
        //if(g_pEnvironment->GetBackground()->GetID() == cSpaceBackground::ID)
        //{
        //    g_pEnvironment->GetBackground()->SetGroundDensity(0u, bDense ? 1.0f : 0.3f);
        //    bDense = !bDense;
        //}
        
        //if(STATIC_ISVALID(g_pGame))
        //{
        //    static coreUint32 iScore = 0u;
        //    iScore += 1u;
        //    
        //    static coreUint32 iTime = 100'000u;
        //    ASSERT(iTime)
        //    iTime -= 1'000u;
        //    
        //    //UploadLeaderboardsArcade(iScore, iTime);
        //    
        //    Core::Debug->InspectValue("Score", iScore);
        //    Core::Debug->InspectValue("Time",  iTime);
        //}
        
        //if(STATIC_ISVALID(g_pGame) && g_pGame->GetCurMission()->GetCurBoss())
        //{
        //    c_cast<coreUint8&>(g_pGame->GetCurMission()->GetCurBoss()->GetHelperHit()) = 0xFFu;
        //}
        
        //if(STATIC_ISVALID(g_pGame))
        //{
        //    for(coreUintW i = 0u; i < FRAGMENTS; ++i)
        //        g_pGame->GetItemManager()->AddItem<cFragmentItem>(coreVector2((I_TO_F(i % 3u) - 1.0f) * 0.7f, (I_TO_F(i / 3u) - 1.0f) * 0.7f) * FOREGROUND_AREA     * 1.5f, i, 0u, 0u);
        //}
    }

    Core::Debug->InspectValue("Score Queue",  coreUint32(g_pSave->GetScoreQueue ()->size()));
    Core::Debug->InspectValue("Replay Queue", coreUint32(g_pSave->GetReplayQueue()->size()));
    Core::Debug->InspectValue("Fly Offset",   g_pEnvironment->GetFlyOffset());

#endif
}