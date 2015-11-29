//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

coreVector2      g_vGameResolution = coreVector2(0.0f,0.0f);
coreVector2      g_vMenuCenter     = coreVector2(0.0f,0.0f);
coreMusicPlayer  g_MusicPlayer     = {};

cOutline*        g_pOutline        = NULL;
cGlow*           g_pGlow           = NULL;
cDistortion*     g_pDistortion     = NULL;
cSpecialEffects* g_pSpecialEffects = NULL;
cPostProcessing* g_pPostProcessing = NULL;

cForeground*     g_pForeground     = NULL;
cEnvironment*    g_pEnvironment    = NULL;
cMenu*           g_pMenu           = NULL;
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
    g_pOutline        = new cOutline();
    g_pGlow           = new cGlow();
    g_pDistortion     = new cDistortion();
    g_pSpecialEffects = new cSpecialEffects();
    g_pPostProcessing = new cPostProcessing();
    g_pForeground     = new cForeground();
    g_pEnvironment    = new cEnvironment();
    g_pMenu           = new cMenu();
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // delete and exit main components
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pMenu)
    SAFE_DELETE(g_pEnvironment)
    SAFE_DELETE(g_pForeground)
    SAFE_DELETE(g_pPostProcessing)
    SAFE_DELETE(g_pSpecialEffects)
    SAFE_DELETE(g_pDistortion)
    SAFE_DELETE(g_pGlow)
    SAFE_DELETE(g_pOutline)
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
    Core::Debug->MeasureStart("Update");
    {
        // update glow- and distortion-effect
        g_pGlow      ->Update();
        g_pDistortion->Update();

        // update the shadow map class
        cShadow::GlobalUpdate();
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
        if(g_pGame)
        {
            // create foreground frame buffer
            g_pForeground->Start();
            {
                // render the game
                g_pGame->Render();

                // render special-effects
                g_pSpecialEffects->Render(true);
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
    // lock framerate and override elapsed time
    if(g_pGame) LockFramerate();

    Core::Debug->MeasureStart("Move");
    {
        // update input interface
        UpdateInput();

        // move environment, menu and game
        g_pEnvironment->Move();
        g_pMenu       ->Move();
        if(g_pGame) g_pGame->Move();

        // move special-effects
        g_pSpecialEffects->Move();

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

    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(K), CORE_INPUT_PRESS))
    {
        SAFE_DELETE(g_pGame)
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(L), CORE_INPUT_PRESS))
    {
        if(!g_pGame)
        {
            g_pGame = new cGame(true);
            g_pGame->LoadMission(REF_ID(cViridoMission::ID));
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(J), CORE_INPUT_PRESS))
    {
        g_pMenu->ChangeSurface(SURFACE_EMPTY, 0.0f);
    }
    if(!Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LSHIFT), CORE_INPUT_HOLD))
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cGrassBackground::ID));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cCloudBackground::ID));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cSeaBackground::ID));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cDesertBackground::ID));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(5), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cSpaceBackground::ID));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(6), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cVolcanoBackground::ID));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(7), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cSnowBackground::ID));
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(8), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(REF_ID(cMossBackground::ID));
    }

    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS))
        Core::System->Quit();

    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(H), CORE_INPUT_PRESS))
        g_pSpecialEffects->ShakeScreen(1.0f);

    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(G), CORE_INPUT_PRESS))
    {
        if(g_pGame)
        {
            const coreVector3 vPos = g_pGame->GetPlayer(0u)->GetPosition();

            g_pDistortion    ->CreateWave       (vPos, DISTORTION_WAVE_BIG);
            g_pSpecialEffects->CreateSplashColor(vPos, SPECIAL_SPLASH_BIG, COLOR_ENERGY_BLUE);
            g_pSpecialEffects->CreateBlast      (vPos, SPECIAL_BLAST_BIG,  LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_ENERGY_BLUE, 0.75f));

            g_pSpecialEffects->PlaySound(vPos, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(B), CORE_INPUT_PRESS))
    {
        if(g_pGame)
        {
            const coreVector3 vPos = g_pGame->GetPlayer(0u)->GetPosition();

            g_pDistortion    ->CreateWave      (vPos, DISTORTION_WAVE_SMALL);
            g_pSpecialEffects->CreateSplashDark(vPos, SPECIAL_SPLASH_SMALL);//, COLOR_RED_F);
            g_pSpecialEffects->CreateBlast     (vPos, SPECIAL_BLAST_SMALL, coreVector3(1.0f,1.0f,1.0f));
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(F), CORE_INPUT_PRESS))
    {
        if(g_pGame)
        {
            const coreVector3 vPos = g_pGame->GetPlayer(0u)->GetPosition();
            g_pDistortion    ->CreateBurst    (vPos, coreVector2(1.0f,0.0f),      DISTORTION_BURST_BIG);
            g_pSpecialEffects->CreateBlowColor(vPos, coreVector3(1.0f,0.0f,0.0f), SPECIAL_BLOW_BIG, COLOR_ENERGY_BLUE);
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(V), CORE_INPUT_PRESS))
    {
        if(g_pGame)
        {
            const coreVector3 vPos = g_pGame->GetPlayer(0u)->GetPosition();
            g_pDistortion    ->CreateBurst    (vPos, coreVector2(1.0f,0.0f),      DISTORTION_BURST_SMALL);
            g_pSpecialEffects->CreateBlowColor(vPos, coreVector3(1.0f,0.0f,0.0f), SPECIAL_BLOW_SMALL, COLOR_ENERGY_BLUE);
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(M), CORE_INPUT_PRESS))
    {
        if(g_pGame)
        {
            const coreVector3 vPos = g_pGame->GetPlayer(0u)->GetPosition();
            //g_pDistortion    ->CreateWave       (vPos, DISTORTION_WAVE_BIG);
            //g_pSpecialEffects->CreateSplashFire (vPos, SPECIAL_EXPLOSION_BIG);
            //g_pSpecialEffects->CreateSplashColor(vPos, SPECIAL_SPLASH_BIG, coreVector3(234.0f/255.0f, 72.0f/255.0f, 10.0f/255.0f) * 1.08f);
            //g_pSpecialEffects->PlaySound        (vPos, 1.0f, SOUND_EXPLOSION_PHYSICAL_BIG);
            //g_pSpecialEffects->ShakeScreen      (SPECIAL_SHAKE_BIG);

            //g_pSpecialEffects->MacroExplosionPhysicalBig(vPos);

            g_pSpecialEffects->CreateRing(vPos, coreVector3(0.0f,1.0f,0.0f), coreVector3(0.0f,0.0f,1.0f), SPECIAL_RING_BIG, COLOR_ENERGY_YELLOW);
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(O), CORE_INPUT_PRESS))
    {
        if(g_pGame && g_pGame->GetMission()->GetCurBoss())
        {
            g_pGame->GetMission()->GetCurBoss()->TakeDamage(1000, NULL);
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(P), CORE_INPUT_PRESS))
    {
        if(g_pGame)
        {
            cPlayer* pPlayer = g_pGame->GetPlayer(0u);
            pPlayer->SetForce(coreVector2(80.0f,40.0f));
        }
    }

    if(g_pGame)
    {
        if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LSHIFT), CORE_INPUT_HOLD))
        {
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, REF_ID(cRayWeapon  ::ID));
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, REF_ID(cPulseWeapon::ID));
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(3), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, REF_ID(cWaveWeapon ::ID));
            if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(4), CORE_INPUT_PRESS)) g_pGame->GetPlayer(0u)->EquipWeapon(0u, REF_ID(cTeslaWeapon::ID));
        }
    }

    static coreFloat TEST = 0.0f;
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(T), CORE_INPUT_HOLD))
    {
        TEST += 10.0f * Core::System->GetTime();
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(Y), CORE_INPUT_HOLD))
    {
        TEST -= 10.0f * Core::System->GetTime();
    }
    g_pEnvironment->SetTargetHeight(TEST);

    //std::function<void()> test = []()
    //{
    //
    //};

    coreUintW iSize = sizeof(cBullet); // 280
    iSize = iSize;

    coreVector2 A = coreVector2::Rand();
    coreVector2 B = A * 15.0f;
    coreVector2 C = A * SQRT(15.0f);

    coreFloat D = B.Length();
    coreFloat E = C.LengthSq();

    const coreChar* pcTest = SDL_GetDisplayName(0);


    coreVector3 vA = coreVector3::Rand();
    coreVector3 vB = coreVector3::Rand();

    coreVector3 vC = coreVector3::Cross(vA, vB);

    coreVector3 vD = (vA * vB.yzx() - vA.yzx() * vB).yzx();



    coreVector4 v1 = coreVector4(0.0f, -1.0f, coreVector2::Rand() * coreVector2::Rand());
    coreUint64 un = v1.PackFloat4x16();
    coreVector4 v2 = coreVector4::UnpackFloat4x16(un);



    coreSet<const coreChar*> apcList;
    apcList.insert(pcTest);

    coreSet<const coreChar*> apcList2 = std::move(apcList);

    coreVector2 aaa;
    coreVector2 bbb = coreVector2(std::numeric_limits<float>::signaling_NaN(), 1.0f);
    coreVector2 ccc = aaa + bbb;

    coreUintW iSize2 = sizeof(coreObject3D);
    coreUintW iSize3 = sizeof(coreObject2D);
    coreFloat t = (coreFloat)iSize2 / coreFloat(iSize2);
    iSize2 = iSize3;


    coreUint32 A_1 = v1.PackSnorm4x8();
    coreUint32 B_1 = 0;//PACK(v1);

    coreVector4 vUnpack1 = coreVector4::UnpackSnorm4x8(A_1);
    coreVector4 vUnpack2 = coreVector4::UnpackSnorm4x8(B_1);

    vUnpack1 = coreVector4(ABS(vUnpack1.x), ABS(vUnpack1.y), ABS(vUnpack1.z), ABS(vUnpack1.w)) - coreVector4(ABS(v1.x), ABS(v1.y), ABS(v1.z), ABS(v1.w));
    vUnpack2 = coreVector4(ABS(vUnpack2.x), ABS(vUnpack2.y), ABS(vUnpack2.z), ABS(vUnpack2.w)) - coreVector4(ABS(v1.x), ABS(v1.y), ABS(v1.z), ABS(v1.w));
    A_1 = B_1;

    coreUint16 f1 = coreMath::Float32to16(0.0f);
    coreFloat  f2 = coreMath::Float16to32(f1);
    f2 = f2;



    coreFloat f3 = Core::Rand->Float(100.0f);
    coreFloat f4 = std::sin(f3);
    coreFloat f6 = (std::fmod(f3 + PI*1.0f, 2.0f*PI) - PI*1.0f);
    coreFloat f5 = f6 - (f6*f6*f6)/6.0f + (f6*f6*f6*f6*f6)/120.0f - (f6*f6*f6*f6*f6*f6*f6)/5040.0f;
    f4 = Core::Rand->Float(10.0f);
    f5 = Core::Rand->Float(10.0f);
    f6 = MIN(f4, f5);
    f6 = (f4 != 0.0f) * (f4 * RSQRT(f4));
    //Core::Log->Info("%f", f6);

    f5 = coreMath::Log<1024>(1024.0f*1024.0f);

    coreUintW z1 = Core::Rand->Int(-1500, 1500);
    coreUintW z2 = Core::Rand->Int(-1500, 1500);
    coreUintW z3 = MAX(z1, z2);
              z3 = MIN(z2, z1);
              //z3 = 0;
    //SET_BIT(z3, 40, true)
    //z3 = z3;
    //z3 = z1;

    //Core::Log->Info("%d", z3);
    //Core::Debug->MeasureStart("snprintf");
    //
    //coreChar acString[256];
    //
    //for(coreUintW i = 0u; i < 10000u; ++i)
    //    snprintf(acString, 255, "hallo %d %.2f", (int)Core::Input->GetJoystickNum(), D);
    //
    //Core::Debug->MeasureEnd("snprintf");
    //
    //Core::Debug->MeasureStart("_snprintf");
    //
    //for(coreUintW i = 0u; i < 10000u; ++i)
    //    _snprintf(acString, 255, "hallo %d %.2f", (int)Core::Input->GetJoystickNum(), D);
    //
    //Core::Debug->MeasureEnd("_snprintf");
    //Core::Debug->MeasureStart("print");
    //
    //for(coreUintW i = 0u; i < 10000u; ++i)
    //    PRINT("hallo %d %.2f", (int)Core::Input->GetJoystickNum(), D);
    //
    //Core::Debug->MeasureEnd("print");

    //coreRand rand1(123);
    //coreRand rand2(321);
    //
    //std::swap(rand1, rand2);
    //
    //std::is_pod<coreVector3> test;
    //if(test == test) std::swap(rand1, rand2);



    //test.value;

    //g_pEnvironment->Activate();
    //g_pPostProcessing->SetSideOpacity(1.0f);


    //coreFloat f1 = 1.23f;
    //coreFloat f2 = 65.32f;
    //
    //coreUint16 u1 = float16(f1);
    //coreUint16 u2 = float16(f2);
    //
    //coreFloat f3 = float32(u1);
    //coreFloat f4 = float32(u2);
    //
    //f1 = f2;
    //f3 = f4;

    //Core::Debug->MeasureStart("Log");
    //for(coreUintW i = 0u; i < 1000u; ++i) coreData::FileExists("data/license.txt");
    //Core::Debug->MeasureEnd("Log");

    // ########################## DEBUG ##########################
}