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

cOutline*        g_pOutlineFull    = NULL;
cOutline*        g_pOutlineDirect  = NULL;
cGlow*           g_pGlow           = NULL;
cDistortion*     g_pDistortion     = NULL;
cPostProcessing* g_pPostProcessing = NULL;

cForeground*     g_pForeground     = NULL;
cEnvironment*    g_pEnvironment    = NULL;
cMenu*           g_pMenu           = NULL;
cGame*           g_pGame           = NULL;


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // calculate biggest possible 1:1 resolution
    const float fMinRes = Core::System->GetResolution().Min();
    g_vGameResolution   = coreVector2(fMinRes, fMinRes);
    g_vMenuCenter       = g_vGameResolution / Core::System->GetResolution();

    // set camera to default values
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);

    // load configuration
    LoadConfig();

    // create and init main components
    cShadow::GlobalInit();
    g_pOutlineFull    = new cOutline(OUTLINE_SHADER_FULL);
    g_pOutlineDirect  = new cOutline(OUTLINE_SHADER_DIRECT);
    g_pGlow           = new cGlow();
    g_pDistortion     = new cDistortion();
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
    SAFE_DELETE(g_pDistortion)
    SAFE_DELETE(g_pGlow)
    SAFE_DELETE(g_pOutlineDirect)
    SAFE_DELETE(g_pOutlineFull)
    cShadow::GlobalExit();

    // save configuration
    SaveConfig();
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    Core::Debug->MeasureStart("Update");
    {
        // update the glow-effect
        g_pGlow->Update();

        // update the distortion-effect
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

                // apply outline-effect
                g_pOutlineFull  ->Apply();
                g_pOutlineDirect->Apply();
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
    // update input interface
    UpdateInput();

    // move the environment
    g_pEnvironment->Move();

    // move the menu
    g_pMenu->Move();

    // move the game
    if(g_pGame) g_pGame->Move();



    // TODO ### 
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(K), CORE_INPUT_PRESS))
    {
        SAFE_DELETE(g_pGame)
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(L), CORE_INPUT_PRESS))
    {
        if(!g_pGame)
        {
            g_pGame = new cGame(false);
            g_pGame->LoadMission(cMellanMission::ID);
        }
    }
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(1), CORE_INPUT_PRESS))
        g_pEnvironment->ChangeBackground(cGrassBackground::ID);
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(2), CORE_INPUT_PRESS))
        g_pEnvironment->ChangeBackground(cSeaBackground::ID);
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS))
        Core::System->Quit();

    g_pEnvironment->SetTargetSpeed(g_pGame ? 8.0f : 2.0f);
}