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

cPostProcessing* g_pPostProcessing = NULL;
cOutline*        g_pOutline        = NULL;
cForeground*     g_pForeground     = NULL;
cEnvironment*    g_pEnvironment    = NULL;
cGame*           g_pGame           = NULL;

#define _DIRECT_DEBUG_ (1)


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // calculate biggest possible 1:1 resolution
    const float fMinRes = Core::System->GetResolution().Min();
    g_vGameResolution   = coreVector2(fMinRes, fMinRes);

    // set camera to default values
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);

    // load configuration
    LoadConfig();

    // create and init main components
    cShadow::GlobalInit();
    g_pPostProcessing = new cPostProcessing();
    g_pOutline        = new cOutline();
    g_pForeground     = new cForeground();
    g_pEnvironment    = new cEnvironment();
    g_pGame           = new cGame();
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // delete and exit main components
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pEnvironment)
    SAFE_DELETE(g_pForeground)
    SAFE_DELETE(g_pOutline)
    SAFE_DELETE(g_pPostProcessing)
    cShadow::GlobalExit();

    // save configuration
    SaveConfig();
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    // update the shadow map class
    cShadow::GlobalUpdate();

    Core::Debug->MeasureStart("Background");
    {
        // render the environment
        g_pEnvironment->Render();
    }
    Core::Debug->MeasureEnd  ("Background");
    Core::Debug->MeasureStart("Foreground");
    {
        // create foreground frame buffer
        g_pForeground->Start();
        {
            // render the game
            if(g_pGame) g_pGame->Render();

            // apply outline-effect
            g_pOutline->Apply();
        }
        g_pForeground->End();
    }
    Core::Debug->MeasureEnd  ("Foreground");
    Core::Debug->MeasureStart("Post Processing");
    {
        // apply post-processing
        g_pPostProcessing->Apply();
    }
    Core::Debug->MeasureEnd("Post Processing");
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    // update input interface
    UpdateInput();

    // move the environment
    g_pEnvironment->Move();

    // move the game
    if(g_pGame) g_pGame->Move();

#if defined(_DIRECT_DEBUG_)

    for(int i = 0; i < 8; ++i)
    {
        // override background
        if(Core::Input->GetKeyboardButton(coreInputKey(i + int(CORE_INPUT_KEY(1))), CORE_INPUT_PRESS))
            g_pEnvironment->ChangeBackground(i + 1);
    }

#endif
}