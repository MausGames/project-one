///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector2     g_vGameResolution = coreVector2(0.0f,0.0f);
coreFloat       g_fGameRate       = 0.0f;
coreVector2     g_vHudDirection   = coreVector2(0.0f,1.0f);
coreBool        g_bTiltMode       = false;
coreFloat       g_fShiftMode      = 0.0f;
coreBool        g_bDemoVersion    = false;
coreBool        g_bPirateVersion  = false;
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

static coreProtect<coreUint64> s_iOldPerfTime  = 0u;     // last measured high-precision time value
static coreProtect<coreFloat>  s_fLogicalRate  = 0.0f;   // logical frame rate
static coreProtect<coreFloat>  s_fLogicalTime  = 0.0f;   // logical frame time (simulation rate)
static coreProtect<coreDouble> s_dPhysicalTime = 0.0;    // physical frame time (display rate)

static void LockFramerate();                             // lock frame rate
static void ForceFramerate(const coreBool bFull);        // override frame time
static void UpdateListener();                            // 
static void ReshapeGame();                               // reshape and resize game
static void DebugGame();                                 // debug and test game


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // set camera to default values
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);

    // set view frustum to default values
    Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f);

    // set listener to default values
    Core::Audio->SetListener(LISTENER_POSITION, LISTENER_VELOCITY, coreVector3(0.0f,0.0f,-1.0f), coreVector3(0.0f,1.0f,0.0f));

    // load configuration
    LoadConfig();

    // 
    InitAchievements();

    // load available music files
    //g_MusicPlayer.AddMusicFolder ("data/music",              "*.ogg");
    //g_MusicPlayer.AddMusicArchive("data/archives/pack1.cfa", "data/music/*.ogg");
    //g_MusicPlayer.AddMusicArchive("data/archives/pack2.cfa", "data/music/*.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_00.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_01.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_02.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_03.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_04_reverse.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_04.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_05.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_06.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_07.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_08_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_08_loop.ogg");
    g_MusicPlayer.AddMusicFile("data/music/boss_99.ogg");
    g_MusicPlayer.AddMusicFile("data/music/ending_normal.ogg");
    g_MusicPlayer.AddMusicFile("data/music/ending_secret.ogg");
    g_MusicPlayer.AddMusicFile("data/music/menu.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_00_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_00_loop.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_01_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_01_loop.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_02_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_02_loop.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_03_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_03_loop.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_04_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_04_loop.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_05.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_06_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_06_loop.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_07_intro.ogg");
    g_MusicPlayer.AddMusicFile("data/music/mission_07_loop.ogg");

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

    // check for demo version (game would crash otherwise anyway)
    g_bDemoVersion = !coreData::FileExists("data/archives/pack2.cfa");

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

    // override frame time (again)
    ForceFramerate(false);
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    // reshape and resize game
    if(Core::System->GetWinSizeChanged()) ReshapeGame();

    // lock frame rate and override frame time
    LockFramerate();
    ForceFramerate(true);

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
        if(!g_pMenu->IsPaused())
        {
            // 
            g_pReplay->Update();

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
        
        g_pSpecialEffects->MoveAlways();

        // 
        if(STATIC_ISVALID(g_pGame)) g_pGame->MoveAlways();

        // move post-processing
        g_pPostProcessing->Move();   // TODO 1: splitscreen value should not be updated outside of pause, but rest should
        
        
    if(!STATIC_ISVALID(g_pGame) && !g_pSave->GetHeader().oProgress.bFirstPlay)
    {
        g_MusicPlayer.SelectName("menu.ogg");
        
        g_MusicPlayer.SetVolume(1.0f * MUSIC_VOLUME);
        g_MusicPlayer.Play();
    }

        // update the music-player
        g_MusicPlayer.Update();
    }
    Core::Debug->MeasureEnd("Move");

    // 
    CheckAchievements();

    // 
    UpdateListener();

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
void InitFramerate()
{
    // get current display mode
    SDL_DisplayMode oMode = {};
    if(SDL_GetWindowDisplayMode(Core::System->GetWindow(), &oMode) || (oMode.refresh_rate <= 0))
    {
        SDL_GetCurrentDisplayMode(Core::System->GetDisplayIndex(), &oMode);
    }

    // 
    const coreUint32 iRefreshRate = (oMode.refresh_rate > 0) ? oMode.refresh_rate : 60u;

    // calculate logical and physical frame time
    if(!STATIC_ISVALID(g_pGame))
    {
        const coreDouble dUpdateFreq = coreDouble(g_CurConfig.Game.iUpdateFreq ? g_CurConfig.Game.iUpdateFreq : iRefreshRate);
        const coreDouble dGameSpeed  = coreDouble(g_CurConfig.Game.iGameSpeed  ? g_CurConfig.Game.iGameSpeed  : 100u) / 100.0;

        const coreDouble dRawRate   = dUpdateFreq / dGameSpeed;
        const coreDouble dFixedRate = MIN(CEIL(coreDouble(FRAMERATE_MIN) / dRawRate) * dRawRate, coreDouble(FRAMERATE_MAX));   // increase in multiples, if below minimum frame rate

        s_fLogicalRate  = coreFloat(dFixedRate);
        s_fLogicalTime  = coreFloat(1.0 / dFixedRate);
        s_dPhysicalTime = 1.0 / (dFixedRate * dGameSpeed);

        g_fGameRate = s_fLogicalRate;
    }

    // override vertical synchronization
    if(Core::Config->GetInt(CORE_CONFIG_SYSTEM_VSYNC))
    {
        ASSERT(s_dPhysicalTime)

        if(iRefreshRate == F_TO_UI(1.0 / s_dPhysicalTime))
        {
            SDL_GL_SetSwapInterval(1);
        }
        else
        {
            SDL_GL_SetSwapInterval(0);
        }
    }

    // TODO 1: coreProtect in variadic argument is not portable
    //Core::Log->Info("Frame rate properties configured (refresh rate %u, logical rate %.1f, physical rate %.1f)", iRefreshRate, s_fLogicalRate, 1.0 / s_dPhysicalTime);
}


// ****************************************************************
// 
FUNC_PURE coreFloat RoundFreq(const coreFloat fFreq)
{
    ASSERT((fFreq > 0.0f) && (fFreq <= FRAMERATE_MIN))
    return RCP(ROUND(RCP(fFreq) * s_fLogicalRate)) * s_fLogicalRate;
}


// ****************************************************************
// lock frame rate
static void LockFramerate()
{
    if(!Core::Debug->IsEnabled())
    {
        coreUint64 iNewPerfTime;
        coreDouble dDifference;

        // measure and calculate current frame time
        const auto nMeasureFunc = [&]()
        {
            iNewPerfTime = SDL_GetPerformanceCounter();
            dDifference  = coreDouble(iNewPerfTime - s_iOldPerfTime) * Core::System->GetPerfFrequency();
        };

        // spin as long as frame time is too low
        for(nMeasureFunc(); dDifference < s_dPhysicalTime; nMeasureFunc())
        {
            // 
            constexpr coreUint32 iMargin = 1u;

            // sleep (once) to reduce overhead
            const coreUint32 iSleep = MAX(F_TO_UI((s_dPhysicalTime - dDifference) * 1000.0), iMargin) - iMargin;
            if(iSleep) SDL_Delay(iSleep);

            // 
            else CORE_SPINLOCK_YIELD
        }

        // save last high-precision time value
        s_iOldPerfTime = iNewPerfTime;
    }
}


// ****************************************************************
// override frame time
static void ForceFramerate(const coreBool bFull)
{
    if(STATIC_ISVALID(g_pGame) && !g_pMenu->IsPaused())
    {
        // 
        if(TIME) c_cast<coreFloat&>(TIME) = s_fLogicalTime;

        if(bFull)
        {
            // 
            coreFloat& fFreezeTime = c_cast<coreFloat&>(g_pSpecialEffects->GetFreezeTime());
            if(fFreezeTime)
            {
                fFreezeTime = MAX0(fFreezeTime - TIME);
                c_cast<coreFloat&>(TIME) *= 0.001f;
            }

            // 
            coreFloat& fSlowTime     = c_cast<coreFloat&>(g_pSpecialEffects->GetSlowTime());
            coreFloat& fSlowStrength = c_cast<coreFloat&>(g_pSpecialEffects->GetSlowStrength());
            if(fSlowTime || fSlowStrength)
            {
                fSlowTime     = MAX0(fSlowTime - TIME);
                fSlowStrength = fSlowTime ? 1.0f : MAX0(fSlowStrength - 0.3f * TIME);
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
    const coreFloat fSide = (g_CurConfig.Game.iMirrorMode == 1u) ? 1.0f : -1.0f;

    // 
    const coreVector2 vGame  = g_pPostProcessing->GetDirection();
    const coreVector2 vHud   = g_vHudDirection;
    const coreVector2 vFinal = MapToAxisInv(vGame, vHud);
    ASSERT(vFinal.IsNormalized())

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
    
    
    if(STATIC_ISVALID(g_pGame))   // TODO 1: sollte nur beim draggen passieren, passiert aber auch bei normaler resolution änderung durch options (GetWinSizeChanged ist in beiden fällen TRUE)
    {
        g_pGame->GetInterface ()->UpdateLayout();
        g_pGame->GetInterface ()->UpdateSpacing();
        g_pGame->GetInterface ()->MoveTimeless();
        g_pGame->GetCombatText()->UpdateLayout();

        if(STATIC_ISVALID(g_pGame) && (g_pGame->GetCurMission()->GetID() == cIntroMission::ID))
        {
            Timeless([]()
            {
                g_pGame->GetCurMission()->MoveAfter();
            });
            g_pMenu->InvokePauseStep();
        }
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
            oOptions.iKind       = GAME_KIND_ALL;
            oOptions.iType       = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(X), CORE_INPUT_HOLD) ? GAME_TYPE_COOP : GAME_TYPE_SOLO;
            oOptions.iMode       = GAME_MODE_STANDARD;
            oOptions.iDifficulty = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(V), CORE_INPUT_HOLD) ? GAME_DIFFICULTY_EASY : GAME_DIFFICULTY_NORMAL;
            oOptions.iFlags      = GAME_FLAG_TASK | GAME_FLAG_FRAGMENT;
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                oOptions.aiShield  [i]    = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(C), CORE_INPUT_HOLD) ? 20u : 0u;
                oOptions.aaiWeapon [i][0] = cRayWeapon::ID;
                oOptions.aaiSupport[i][0] = 0u;
            }

            const coreInt32* piMissionList = g_bDemoVersion ?            GAME_MISSION_LIST_DEMO  :            GAME_MISSION_LIST_MAIN;
            const coreUintW  iNumMissions  = g_bDemoVersion ? ARRAY_SIZE(GAME_MISSION_LIST_DEMO) : ARRAY_SIZE(GAME_MISSION_LIST_MAIN);

            #define __LOAD_GAME(x) {STATIC_NEW(g_pGame, oOptions, piMissionList, iNumMissions) g_pGame->LoadMissionID(x); g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f); g_pPostProcessing->SetWallOpacity(1.0f); g_pEnvironment->Activate(); g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);}
                 if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) __LOAD_GAME(cIntroMission  ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) __LOAD_GAME(cViridoMission ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS)) __LOAD_GAME(cNevoMission   ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS)) __LOAD_GAME(cHarenaMission ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(5), CORE_INPUT_PRESS)) __LOAD_GAME(cRutilusMission::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(6), CORE_INPUT_PRESS)) __LOAD_GAME(cGeluMission   ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(7), CORE_INPUT_PRESS)) __LOAD_GAME(cCalorMission  ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(8), CORE_INPUT_PRESS)) __LOAD_GAME(cMuscusMission ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(9), CORE_INPUT_PRESS)) __LOAD_GAME(cAterMission   ::ID)
            else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(0), CORE_INPUT_PRESS)) __LOAD_GAME(cErrorMission  ::ID)
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
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(0), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cStomachBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
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

#endif

    // keep noise low
    if(SDL_GL_GetSwapInterval())
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
        //if(g_pEnvironment->GetBackground()->GetID() == cDarkBackground::ID)
        //{
        //    d_cast<cDarkBackground*>(g_pEnvironment->GetBackground())->Dissolve();
        //}
        
        if(STATIC_ISVALID(g_pGame)) g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
        
        //if(STATIC_ISVALID(g_pGame) && g_pGame->GetCurMission()->GetCurBoss())
        //{
        //    c_cast<coreUint8&>(g_pGame->GetCurMission()->GetCurBoss()->GetHelperHit()) = 0xFFu;
        //}
        
        if(STATIC_ISVALID(g_pGame))
        {
            for(coreUintW i = 0u; i < FRAGMENTS; ++i)
                g_pGame->GetItemManager()->AddItem<cFragmentItem>(coreVector2((I_TO_F(i % 3u) - 1.0f) * 0.7f, (I_TO_F(i / 3u) - 1.0f) * 0.7f) * FOREGROUND_AREA     * 1.5f, i, 0u, 0u);
        }
    }
    
    Core::Debug->InspectValue("Fly Offset", g_pEnvironment->GetFlyOffset());
}