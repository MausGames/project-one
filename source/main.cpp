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

cEnvironment*    g_pEnvironment    = NULL;
cPostProcessing* g_pPostProcessing = NULL;

static coreModelPtr pSquare = NULL;   // square model handle


// ****************************************************************
// init the application
void CoreApp::Init()
{
    // 
    const float fMinRes = Core::System->GetResolution().Min();
    g_vGameResolution   = coreVector2(fMinRes, fMinRes);

    // 
    g_pEnvironment    = new cEnvironment();
    g_pPostProcessing = new cPostProcessing();

    // 
    Core::Graphics->SetCamera(coreVector3(0.0f,0.0f,110.0f), 
                              Core::Graphics->GetCamDirection(), 
                              Core::Graphics->GetCamOrientation());

    

 
    // hold square model active
    pSquare = Core::Manager::Resource->Get<coreModel>("default_square.md5mesh");
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    // 
    SAFE_DELETE(g_pEnvironment)
    SAFE_DELETE(g_pPostProcessing)

    // 
    pSquare = NULL;
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    // 
    Core::Graphics->SetLight(0, coreVector4(0.0f,0.0f,0.0f,0.0f),
                                coreVector4(LIGHT_DIRECTION * coreMatrix4::RotationZ(Core::Graphics->GetCamOrientation().xy() * coreVector2(-1.0f,1.0f)), 0.0f),
                                coreVector4(0.0f,0.0f,0.0f,0.0f));

    // 
    g_pEnvironment->Render();

    // 
    g_pPostProcessing->Apply();
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    // DEBUG #
    g_pEnvironment->SetTargetSide(Core::Input->GetMousePosition() * 60.0f);
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(A), CORE_INPUT_PRESS))
        g_pEnvironment->ChangeBackground(2);

    // 
    g_pEnvironment->Move();
}