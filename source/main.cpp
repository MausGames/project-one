///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector2     g_vGameResolution = coreVector2(0.0f,0.0f);
coreVector2     g_vMenuCenter     = coreVector2(0.0f,0.0f);
coreBool        g_bDebugOutput    = false;
coreMusicPlayer g_MusicPlayer     = {};

STATIC_MEMORY(cReplay,         g_pReplay)
STATIC_MEMORY(cOutline,        g_pOutline)
STATIC_MEMORY(cGlow,           g_pGlow)
STATIC_MEMORY(cDistortion,     g_pDistortion)
STATIC_MEMORY(cWindscreen,     g_pWindscreen)
STATIC_MEMORY(cSpecialEffects, g_pSpecialEffects)
STATIC_MEMORY(cPostProcessing, g_pPostProcessing)
STATIC_MEMORY(cForeground,     g_pForeground)
STATIC_MEMORY(cEnvironment,    g_pEnvironment)
STATIC_MEMORY(cMenu,           g_pMenu)
STATIC_MEMORY(cTheater,        g_pTheater)

cGame* g_pGame = NULL;

static coreUint64 m_iOldPerfTime = 0u;   // last measured high-precision time value
static void LockFramerate();             // lock frame rate and override frame time
static void ReshapeGame();               // reshape and resize game
static void DebugGame();                 // debug and test game


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // set camera to default values
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);

    // set view frustum to default values
    Core::Graphics->SetView(Core::System->GetResolution(), DEG_TO_RAD(45.0f), 50.0f, 500.0f);

    // set listener to default values
    Core::Audio->SetListener(coreVector3(0.0f,0.0f,10.0f), coreVector3(0.0f,0.0f,0.0f),
                             coreVector3(0.0f,0.0f,-1.0f), coreVector3(0.0f,1.0f,0.0f));

    // init system properties
    InitResolution(Core::System->GetResolution());
    InitFramerate();

    // load configuration
    LoadConfig();

    // load available music files
    g_MusicPlayer.AddMusicFolder("data/music", "*.ogg");

    // create and init main components
    cShadow::GlobalInit();
    STATIC_NEW(g_pReplay)
    STATIC_NEW(g_pOutline)
    STATIC_NEW(g_pGlow)
    STATIC_NEW(g_pDistortion)
    STATIC_NEW(g_pWindscreen)
    STATIC_NEW(g_pSpecialEffects)
    STATIC_NEW(g_pPostProcessing)
    STATIC_NEW(g_pForeground)
    STATIC_NEW(g_pEnvironment)
    STATIC_NEW(g_pMenu)
    STATIC_NEW(g_pTheater)
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // 
    SAFE_DELETE(g_pGame)

    // delete and exit main components
    STATIC_DELETE(g_pTheater)
    STATIC_DELETE(g_pMenu)
    STATIC_DELETE(g_pEnvironment)
    STATIC_DELETE(g_pForeground)
    STATIC_DELETE(g_pPostProcessing)
    STATIC_DELETE(g_pSpecialEffects)
    STATIC_DELETE(g_pWindscreen)
    STATIC_DELETE(g_pDistortion)
    STATIC_DELETE(g_pGlow)
    STATIC_DELETE(g_pOutline)
    STATIC_DELETE(g_pReplay)
    cShadow::GlobalExit();

    // unload music files
    g_MusicPlayer.ClearMusic();

    // save configuration
    SaveConfig();
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
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
            if(g_pGame || g_pTheater->IsActive() || g_pWindscreen->IsActive())
            {
                // create foreground frame buffer
                g_pForeground->Start();
                {
                    // render the theater
                    g_pTheater->Render();

                    // render the game
                    if(g_pGame) g_pGame->Render();

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
    Core::Debug->MeasureStart("Post Processing");
    {
        // apply post-processing
        g_pPostProcessing->Apply();
    }
    Core::Debug->MeasureEnd("Post Processing");
    Core::Debug->MeasureStart("Interface");
    {
        glDisable(GL_DEPTH_TEST);
        {
            // render the overlay separately
            if(g_pGame) g_pGame->RenderOverlay();

            // render the menu
            g_pMenu->Render();
        }
        glEnable(GL_DEPTH_TEST);
    }
    Core::Debug->MeasureEnd("Interface");
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    // reshape and resize game
    if(Core::System->GetWinSizeChanged()) ReshapeGame();

    // lock frame rate and override frame time
    if(g_pGame) LockFramerate();

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

            // move the theater
            g_pTheater->Move();

            // move the game
            if(g_pGame) g_pGame->Move();

            // 
            g_pWindscreen->Move();

            // move special-effects
            g_pSpecialEffects->Move();
        }

        // update the music-player
        g_MusicPlayer.Update();
    }
    Core::Debug->MeasureEnd("Move");

    // debug and test game
    if(Core::Debug->IsEnabled()) DebugGame();
}


// ****************************************************************
// init resolution properties
void InitResolution(const coreVector2& vResolution)
{
    // calculate biggest possible 1:1 resolution
    g_vGameResolution = coreVector2(1.0f,1.0f) * vResolution.Min();
    g_vMenuCenter     = g_vGameResolution / vResolution;
}


// ****************************************************************
// init frame rate properties
void InitFramerate()
{
    if(!Core::Debug->IsEnabled())
    {
        SDL_Window* pWindow = Core::System->GetWindow();

        // get current display mode
        SDL_DisplayMode oMode = {};
        SDL_GetWindowDisplayMode(pWindow, &oMode);

        // check for valid refresh rate
        if(oMode.refresh_rate != F_TO_SI(FRAMERATE_VALUE))
        {
            // try to override refresh rate
            oMode.refresh_rate = F_TO_SI(FRAMERATE_VALUE);
            SDL_SetWindowDisplayMode(pWindow, &oMode);
            SDL_GetWindowDisplayMode(pWindow, &oMode);

            Core::Log->Warning("Refresh rate overridden (%d)", oMode.refresh_rate);
        }

        // force vertical synchronization
        if(SDL_GL_SetSwapInterval(-1)) SDL_GL_SetSwapInterval(1);
    }
}


// ****************************************************************
// lock frame rate and override frame time
static void LockFramerate()
{
    if(!Core::Debug->IsEnabled())
    {
        coreUint64 iNewPerfTime;
        coreFloat  fDifference;

        // measure and calculate current frame time
        const auto nMeasureFunc = [&]()
        {
            iNewPerfTime = SDL_GetPerformanceCounter();
            fDifference  = coreFloat(coreDouble(iNewPerfTime - m_iOldPerfTime) * Core::System->GetPerfFrequency());
        };

        // spin as long as frame time is too low
        for(nMeasureFunc(); fDifference < FRAMERATE_TIME; nMeasureFunc())
        {
            // sleep (once) to reduce overhead
            const coreUint32 iSleep = MAX(F_TO_UI((FRAMERATE_TIME - fDifference) * 1000.0f), 1u) - 1u;
            if(iSleep) SDL_Delay(iSleep);

        #if defined(_CORE_SSE_)

            // processor level spinning
            else _mm_pause();

        #endif
        }

        // save last high-precision time value
        m_iOldPerfTime = iNewPerfTime;
    }

    // override frame time
    if(Core::System->GetTime()) c_cast<coreFloat&>(Core::System->GetTime()) = FRAMERATE_TIME;
}


// ****************************************************************
// reshape and resize game
static void ReshapeGame()
{
    // update system properties
    InitResolution(Core::System->GetResolution());
    InitFramerate();

    // reshape engine
    Core::Reshape();
}


// ****************************************************************
// debug and test game
static void DebugGame()
{
    // start game
    if(!g_pGame)
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LALT), CORE_INPUT_HOLD))
        {
            const coreUint8 iDifficulty = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(Y), CORE_INPUT_HOLD) ? 0u : 1u;
            const coreBool  bCoop       = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(X), CORE_INPUT_HOLD);

            #define __LOAD_GAME(x) {g_pGame = new cGame(iDifficulty, bCoop, GAME_MISSION_LIST_DEFAULT); g_pGame->LoadMissionID(x); g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f); g_pPostProcessing->SetSideOpacity(1.0f);}
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) __LOAD_GAME(cIntroMission  ::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) __LOAD_GAME(cViridoMission ::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS)) __LOAD_GAME(cNevoMission   ::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS)) __LOAD_GAME(cHarenaMission ::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(5), CORE_INPUT_PRESS)) __LOAD_GAME(cRutilusMission::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(6), CORE_INPUT_PRESS)) __LOAD_GAME(cGeluMission   ::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(7), CORE_INPUT_PRESS)) __LOAD_GAME(cCalorMission  ::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(8), CORE_INPUT_PRESS)) __LOAD_GAME(cMuscusMission ::ID)
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(9), CORE_INPUT_PRESS)) __LOAD_GAME(cAterMission   ::ID)
        }
    }

    // end game
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(K), CORE_INPUT_PRESS))
    {
        SAFE_DELETE(g_pGame)
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
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cCloudBackground  ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cGrassBackground  ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cSeaBackground    ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cDesertBackground ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(5), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cSpaceBackground  ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(6), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cVolcanoBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(7), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cSnowBackground   ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(8), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cMossBackground   ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(9), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cDarkBackground   ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(0), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(-cNoBackground     ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));

        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS))
            Core::System->Quit();
    }

    // set background movement
    if(!Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PERIOD), CORE_INPUT_HOLD))
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_1),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f,-1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_2),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f,-1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_3),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f,-1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_4),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 0.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_6),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 0.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_7),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_8),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f, 1.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_9),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PLUS),     CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeed((&g_pEnvironment->GetSpeed())[1] + 1.0f);
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MINUS),    CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeed((&g_pEnvironment->GetSpeed())[1] - 1.0f);
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_DIVIDE),   CORE_INPUT_HOLD))  g_pEnvironment->SetTargetSide ((&g_pEnvironment->GetSide ())[1] - coreVector2(30.0f * Core::System->GetTime(), 0.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MULTIPLY), CORE_INPUT_HOLD))  g_pEnvironment->SetTargetSide ((&g_pEnvironment->GetSide ())[1] + coreVector2(30.0f * Core::System->GetTime(), 0.0f));
    }

    // set background height
    static coreFloat s_fHeight = 0.0f;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(R), CORE_INPUT_HOLD)) s_fHeight += 15.0f * Core::System->GetTime();
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(T), CORE_INPUT_HOLD)) s_fHeight -= 15.0f * Core::System->GetTime();
    g_pEnvironment->SetTargetHeight(s_fHeight);

    // set background interpolation
    if(g_pEnvironment->GetBackground()->GetOutdoor())
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(COMMA), CORE_INPUT_PRESS)) g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(1.0f,   0.0f);
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(M),     CORE_INPUT_PRESS)) g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(0.0f, -15.0f);
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(N),     CORE_INPUT_PRESS)) g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(0.0f, -25.0f);
    }

    // equip weapon
    if(g_pGame)
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LCTRL), CORE_INPUT_HOLD))
        {
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cRayWeapon  ::ID);
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cPulseWeapon::ID);
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cWaveWeapon ::ID);
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, cTeslaWeapon::ID);
        }
    }

    // turn player
    if(g_pGame)
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(UP),    CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LEFT),  CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(DOWN),  CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RIGHT), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
    }

    // toggle invincibility
    if(g_pGame)
    {
        static coreBool s_bInvincible = true;
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(Z), CORE_INPUT_PRESS)) s_bInvincible = !s_bInvincible;

        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->SetMaxHealth(s_bInvincible ? 10000 : 1);
            pPlayer->SetCurHealth(s_bInvincible ? 10000 : 1);
        });
    }

    // damage boss
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(O), CORE_INPUT_PRESS))
    {
        if(g_pGame && g_pGame->GetCurMission()->GetCurBoss())
        {
            g_pGame->GetCurMission()->GetCurBoss()->TakeDamage(1000, ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f), NULL);
        }
    }

    // skip stage
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(P), CORE_INPUT_PRESS))
    {
        if(g_pGame &&
           CONTAINS_FLAG(g_pGame->GetCurMission()->GetBoss(0u)->GetStatus(), ENEMY_STATUS_DEAD) &&
           CONTAINS_FLAG(g_pGame->GetCurMission()->GetBoss(1u)->GetStatus(), ENEMY_STATUS_DEAD) &&
           CONTAINS_FLAG(g_pGame->GetCurMission()->GetBoss(2u)->GetStatus(), ENEMY_STATUS_DEAD))
        {
            g_pGame->GetCurMission()->SkipStage();
        }
    }

    // load boss
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PERIOD), CORE_INPUT_HOLD))
    {
        static std::string s_sCode;
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
            // TODO 
            s_sCode.clear();
        }
    }

    // show debug output
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(F), CORE_INPUT_PRESS))
    {
        g_bDebugOutput = !g_bDebugOutput;
    }

    // keep noise low
    if(SDL_GL_GetSwapInterval())
        SDL_Delay(1u);
}