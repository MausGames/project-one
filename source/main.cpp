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

#define SHOW_FPS
#if defined(SHOW_FPS)

    static coreLabel* m_pFPS = NULL;   // frame rate label for debugging purpose
    static float m_fFPSValue = 0.0f;   // current smooth frame rate value

#endif



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

#if defined(SHOW_FPS)

    // create frame rate label
    m_pFPS = new coreLabel("ethnocentric.ttf", 24, 8);
    m_pFPS->SetPosition (coreVector2(0.008f, 0.0f));
    m_pFPS->SetCenter   (coreVector2( -0.5f, 0.5f));
    m_pFPS->SetAlignment(coreVector2(  1.0f,-1.0f));

#endif
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // delete frame rate label
    SAFE_DELETE(m_pFPS)

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

    // render the environment
    g_pEnvironment->Render();

    // create foreground frame buffer
    g_pForeground->Start();
    {
        // render the game
        if(g_pGame) g_pGame->Render();

        // apply outline-effect
        g_pOutline->Apply();
    }
    g_pForeground->End();

    // apply post-processing
    g_pPostProcessing->Apply();

#if defined(SHOW_FPS)

    if(!Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PRINTSCREEN), CORE_INPUT_PRESS))
    {
        // update, move and render frame rate
        if(Core::System->GetTime()) m_fFPSValue = m_fFPSValue * 0.95f + RCP(Core::System->GetTime()) * 0.05f;
        m_pFPS->SetText(PRINT("%.1f", m_fFPSValue));
        m_pFPS->Move();
        m_pFPS->Render();
    }

#endif
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    // DEBUG #
    g_pEnvironment->SetTargetSide(Core::Input->GetMousePosition() * 60.0f);
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(A), CORE_INPUT_PRESS))
        g_pEnvironment->ChangeBackground(2);

    // move the environment
    g_pEnvironment->Move();

    // move the game
    if(g_pGame) g_pGame->Move();
}