//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

#define ASSERT_ALL {ASSERT(g_pReplay)     ASSERT(g_pOutline)        ASSERT(g_pGlow)           \
                    ASSERT(g_pDistortion) ASSERT(g_pSpecialEffects) ASSERT(g_pPostProcessing) \
                    ASSERT(g_pForeground) ASSERT(g_pEnvironment)    ASSERT(g_pMenu)           \
                    ASSERT(g_pTheater)}

coreVector2      g_vGameResolution = coreVector2(0.0f,0.0f);
coreVector2      g_vMenuCenter     = coreVector2(0.0f,0.0f);
coreMusicPlayer  g_MusicPlayer     = {};

cReplay*         g_pReplay         = NULL;
cOutline*        g_pOutline        = NULL;
cGlow*           g_pGlow           = NULL;
cDistortion*     g_pDistortion     = NULL;
cSpecialEffects* g_pSpecialEffects = NULL;
cPostProcessing* g_pPostProcessing = NULL;

cForeground*     g_pForeground     = NULL;
cEnvironment*    g_pEnvironment    = NULL;
cMenu*           g_pMenu           = NULL;
cTheater*        g_pTheater        = NULL;
cGame*           g_pGame           = NULL;

static coreUint64 m_iOldPerfTime = 0u;      // last measured high-precision time value
static coreBool   m_bHardLock    = false;   // status to force framerate with busy-waiting
static void LockFramerate();                // lock framerate and override elapsed time
static void DebugGame();                    // debug and test game separately


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // set camera to default values
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);

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
    g_pReplay         = new cReplay();
    g_pOutline        = new cOutline();
    g_pGlow           = new cGlow();
    g_pDistortion     = new cDistortion();
    g_pSpecialEffects = new cSpecialEffects();
    g_pPostProcessing = new cPostProcessing();
    g_pForeground     = new cForeground();
    g_pEnvironment    = new cEnvironment();
    g_pMenu           = new cMenu();
    g_pTheater        = new cTheater();
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // delete and exit main components
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pTheater)
    SAFE_DELETE(g_pMenu)
    SAFE_DELETE(g_pEnvironment)
    SAFE_DELETE(g_pForeground)
    SAFE_DELETE(g_pPostProcessing)
    SAFE_DELETE(g_pSpecialEffects)
    SAFE_DELETE(g_pDistortion)
    SAFE_DELETE(g_pGlow)
    SAFE_DELETE(g_pOutline)
    SAFE_DELETE(g_pReplay)
    cShadow::GlobalExit();

    // remove all music files
    g_MusicPlayer.ClearMusic();

    // save configuration
    SaveConfig();
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    ASSERT_ALL

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
            if(g_pGame || g_pTheater->IsActive())
            {
                // create foreground frame buffer
                g_pForeground->Start();
                {
                    // render the theater
                    g_pTheater->Render();

                    // render the game
                    if(g_pGame) g_pGame->Render();
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
    ASSERT_ALL

    // lock framerate and override elapsed time
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

            // move environment, theater and game
            g_pEnvironment->Move();
            g_pTheater    ->Move();
            if(g_pGame) g_pGame->Move();

            // move special-effects
            g_pSpecialEffects->Move();
        }

        // update the music-player
        g_MusicPlayer.Update();
    }
    Core::Debug->MeasureEnd("Move");

    // debug and test game separately
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
// init framerate properties
void InitFramerate()
{
    SDL_Window* pWindow = Core::System->GetWindow();

    // get current display mode
    SDL_DisplayMode oMode;
    SDL_GetWindowDisplayMode(pWindow, &oMode);

    if(oMode.refresh_rate != F_TO_SI(FRAMERATE_VALUE))
    {
        // try to override framerate manually
        oMode.refresh_rate = F_TO_SI(FRAMERATE_VALUE);
        SDL_SetWindowDisplayMode(pWindow, &oMode);
        SDL_GetWindowDisplayMode(pWindow, &oMode);

        // check again and enable hard-lock on error
        if(oMode.refresh_rate != F_TO_SI(FRAMERATE_VALUE))
            m_bHardLock = true;
    }
}


// ****************************************************************
// lock framerate and override elapsed time
static void LockFramerate()
{
#if (1)

    if(m_bHardLock)
    {
        coreUint64 iNewPerfTime;
        coreFloat  fDifference;

        // measure and calculate current frame time
        auto nMeasureFunc = [&]()
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
        }

        // save last high-precision time value
        m_iOldPerfTime = iNewPerfTime;
    }

    // override elapsed time
    if(Core::System->GetTime()) c_cast<coreFloat&>(Core::System->GetTime()) = FRAMERATE_TIME;

#endif
}


// ****************************************************************
// debug and test game separately
static void DebugGame()
{
    // ########################## DEBUG ##########################

#if defined(_CORE_MSVC_)
    #pragma warning(disable : 4189 4702)
#endif

    // start game
    if(!g_pGame)
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LALT), CORE_INPUT_HOLD))
        {
            #define __LOAD_GAME(x) {g_pGame = new cGame(false); g_pGame->LoadMission(x); g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f); g_pEnvironment->SetTargetSpeed(6.0f);}
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
        g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f);
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
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_2),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f,-1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_3),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f,-1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_4),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 0.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_6),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 0.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_7),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_8),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f, 1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_9),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 1.0f).Normalized());
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PLUS),     CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeed((&g_pEnvironment->GetSpeed())[1] + 1.0f);
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MINUS),    CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeed((&g_pEnvironment->GetSpeed())[1] - 1.0f);
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_DIVIDE),   CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSide ((&g_pEnvironment->GetSide ())[1] - coreVector2(2.0f,0.0f));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MULTIPLY), CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSide ((&g_pEnvironment->GetSide ())[1] + coreVector2(2.0f,0.0f));
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

    // damage boss
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(O), CORE_INPUT_PRESS))
    {
        if(g_pGame && g_pGame->GetMission()->GetCurBoss())
        {
            g_pGame->GetMission()->GetCurBoss()->TakeDamage(1000, ELEMENT_NEUTRAL, NULL);
        }
    }

    // skip stage
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(P), CORE_INPUT_PRESS))
    {
        if(g_pGame && g_pGame->GetMission() &&
           CONTAINS_FLAG(g_pGame->GetMission()->GetBoss(0u)->GetStatus(), ENEMY_STATUS_DEAD) &&
           CONTAINS_FLAG(g_pGame->GetMission()->GetBoss(1u)->GetStatus(), ENEMY_STATUS_DEAD) &&
           CONTAINS_FLAG(g_pGame->GetMission()->GetBoss(2u)->GetStatus(), ENEMY_STATUS_DEAD))
        {
            g_pGame->GetMission()->SkipStage();
        }
    }

    // load boss
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PERIOD), CORE_INPUT_HOLD))
    {
        static std::string s_sCode;
        for(coreUintW i = 0u; i < 10u; ++i)
        {
            if(Core::Input->GetKeyboardButton(coreInputKey(CORE_INPUT_KEY(KP_1) + i), CORE_INPUT_PRESS))
                s_sCode += coreChar('0' + ((i+1u) % 10u));
        }

        if(s_sCode.size() >= 4u)
        {


            s_sCode.clear();
        }
    }

    static coreFloat s_fHeight = 0.0f;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(E), CORE_INPUT_HOLD))
    {
        s_fHeight += 15.0f * Core::System->GetTime();
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(R), CORE_INPUT_HOLD))
    {
        s_fHeight -= 15.0f * Core::System->GetTime();
    }
    g_pEnvironment->SetTargetHeight(s_fHeight);

    static coreBool s_bInvincible = true;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(Q), CORE_INPUT_PRESS))
    {
        s_bInvincible = !s_bInvincible;
    }
    if(s_bInvincible && g_pGame)
    {
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            if(CONTAINS_FLAG(pPlayer->GetStatus(), PLAYER_STATUS_DEAD))
                pPlayer->Resurrect(pPlayer->GetPosition().xy());

            const coreInt32 iHeal = pPlayer->GetCurHealth() - pPlayer->GetMaxHealth();
            if(iHeal) pPlayer->TakeDamage(iHeal, ELEMENT_NEUTRAL);
        });
    }

    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(F), CORE_INPUT_PRESS))
    {
        static coreBool s_bDebugMode = false;
        g_pPostProcessing->SetDebugMode(s_bDebugMode = !s_bDebugMode);
    }

    if(g_pEnvironment->GetBackground()->GetOutdoor())
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(M), CORE_INPUT_PRESS))
        {
            g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(1.0f, 0.0f);
        }
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(N), CORE_INPUT_PRESS))
        {
            g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(0.0f, -25.0f);
        }
    }

    // ########################## DEBUG ##########################
}