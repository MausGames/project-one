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
coreVector2     g_vHudDirection   = coreVector2(0.0f,1.0f);
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

static coreUint64 s_iOldPerfTime  = 0u;    // last measured high-precision time value
static coreDouble s_dLogicalTime  = 0.0;   // logical frame time (simulation rate)
static coreDouble s_dPhysicalTime = 0.0;   // physical frame time (display rate)

static void LockFramerate();               // lock frame rate and override frame time
static void ReshapeGame();                 // reshape and resize game
static void DebugGame();                   // debug and test game


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

    // load configuration
    LoadConfig();

    // load available music files
    g_MusicPlayer.AddMusicFolder("data/music", "*.ogg");

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
            if(STATIC_ISVALID(g_pGame) || g_pSpecialEffects->IsActive() || g_pWindscreen->IsActive())
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

    glDisable(GL_CULL_FACE);   // for mirror mode
    glDisable(GL_DEPTH_TEST);
    {
        Core::Debug->MeasureStart("Post Processing");
        {
            // render post-processing
            g_pPostProcessing->Render();
        }
        Core::Debug->MeasureEnd("Post Processing");
        Core::Debug->MeasureStart("Interface");
        {
            // render the overlay separately
            if(STATIC_ISVALID(g_pGame)) g_pGame->RenderOverlay();

            // render the menu
            g_pMenu->Render();
        }
        Core::Debug->MeasureEnd("Interface");
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    // reshape and resize game
    if(Core::System->GetWinSizeChanged()) ReshapeGame();

    // lock frame rate and override frame time
    LockFramerate();

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

            // move post-processing
            g_pPostProcessing->Move();
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
void InitResolution(const coreVector2 vResolution)
{
    // calculate biggest possible 1:1 resolution
    g_vGameResolution = coreVector2(1.0f,1.0f) * vResolution.Min();
}


// ****************************************************************
// 
void InitDirection()
{
    // 
    switch(g_CurConfig.Game.iHudRotation)
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
    // calculate logical and physical frame time
    if(!STATIC_ISVALID(g_pGame)) s_dLogicalTime  = (1.0   / coreDouble(CLAMP(g_CurConfig.Game.iUpdateFreq, F_TO_UI(FRAMERATE_MIN), F_TO_UI(FRAMERATE_MAX))));
                                 s_dPhysicalTime = (100.0 / coreDouble(MAX  (g_CurConfig.Game.iGameSpeed,  1u))) * s_dLogicalTime;

    if(Core::Config->GetBool(CORE_CONFIG_SYSTEM_VSYNC))
    {
        // get current display mode
        SDL_DisplayMode oMode = {};
        SDL_GetWindowDisplayMode(Core::System->GetWindow(), &oMode);

        // override vertical synchronization
        if(oMode.refresh_rate == F_TO_SI(1.0 / s_dPhysicalTime))
        {
            if(SDL_GL_SetSwapInterval(-1)) SDL_GL_SetSwapInterval(1);
        }
        else
        {
            SDL_GL_SetSwapInterval(0);
        }
    }
}


// ****************************************************************
// lock frame rate and override frame time
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
            // sleep (once) to reduce overhead
            const coreUint32 iSleep = F_TO_UI((s_dPhysicalTime - dDifference) * 1000.0);
            if(iSleep) SDL_Delay(iSleep);

        #if defined(_CORE_SSE_)

            // processor level spinning
            else _mm_pause();

        #endif
        }

        // save last high-precision time value
        s_iOldPerfTime = iNewPerfTime;
    }

    // override frame time
    if(TIME) c_cast<coreFloat&>(TIME) = coreFloat(s_dLogicalTime);

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
            oOptions.iType       = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(X), CORE_INPUT_HOLD) ? GAME_TYPE_COOP : GAME_TYPE_SOLO;
            oOptions.iMode       = GAME_MODE_STANDARD;
            oOptions.iDifficulty = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(V), CORE_INPUT_HOLD) ? GAME_DIFFICULTY_EASY : GAME_DIFFICULTY_HARD;
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                oOptions.aaiWeapon [i][0] = cRayWeapon::ID;
                oOptions.aaiSupport[i][0] = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(C), CORE_INPUT_HOLD) ? 1u : 0u;
            }

            #define __LOAD_GAME(x) {STATIC_NEW(g_pGame, oOptions, GAME_MISSION_LIST_MAIN) g_pGame->LoadMissionID(x); g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f); g_pPostProcessing->SetWallOpacity(1.0f);}
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
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(0), CORE_INPUT_PRESS)) g_pEnvironment->ChangeBackground(cNoBackground     ::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
    }

    // set background movement
    if(!Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PERIOD), CORE_INPUT_HOLD))
    {
             if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_1),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f,-1.0f).Normalized());
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_2),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f,-1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_3),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f,-1.0f).Normalized());
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_4),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 0.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_6),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 0.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_7),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2(-1.0f, 1.0f).Normalized());
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_8),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 0.0f, 1.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_9),        CORE_INPUT_PRESS)) g_pEnvironment->SetTargetDirection(coreVector2( 1.0f, 1.0f).Normalized());
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_PLUS),     CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeed((&g_pEnvironment->GetSpeed())[1] + 1.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MINUS),    CORE_INPUT_PRESS)) g_pEnvironment->SetTargetSpeed((&g_pEnvironment->GetSpeed())[1] - 1.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_DIVIDE),   CORE_INPUT_HOLD))  g_pEnvironment->SetTargetSide ((&g_pEnvironment->GetSide ())[1] - coreVector2(30.0f * TIME, 0.0f));
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(KP_MULTIPLY), CORE_INPUT_HOLD))  g_pEnvironment->SetTargetSide ((&g_pEnvironment->GetSide ())[1] + coreVector2(30.0f * TIME, 0.0f));
    }

    // set background height
    static coreFloat s_fHeight = 0.0f;
         if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(R), CORE_INPUT_HOLD)) s_fHeight += 15.0f * TIME;
    else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(T), CORE_INPUT_HOLD)) s_fHeight -= 15.0f * TIME;
    g_pEnvironment->SetTargetHeight(s_fHeight);

    // set background interpolation
    if(g_pEnvironment->GetBackground()->GetOutdoor())
    {
             if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(COMMA), CORE_INPUT_PRESS)) g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(1.0f,   0.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(M),     CORE_INPUT_PRESS)) g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(0.0f, -15.0f);
        else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(N),     CORE_INPUT_PRESS)) g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeight(0.0f, -25.0f);
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
        static coreBool s_bInvincible = false;
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(Z), CORE_INPUT_PRESS))
        {
            s_bInvincible = !s_bInvincible;
            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetMaxHealth(s_bInvincible ? PLAYER_SHIELD : (PLAYER_LIVES - 1u));
                pPlayer->SetCurHealth(s_bInvincible ? PLAYER_SHIELD : (PLAYER_LIVES - 1u));
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
                pEnemy->TakeDamage(pEnemy->HasStatus(ENEMY_STATUS_BOSS) ? 500 : 10, ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f), NULL);
            });
        }
    }

    // damage boss
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(O), CORE_INPUT_PRESS))
    {
        if(STATIC_ISVALID(g_pGame) && g_pGame->GetCurMission()->GetCurBoss())
        {
            g_pGame->GetCurMission()->GetCurBoss()->TakeDamage(200, ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f), NULL);
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
}