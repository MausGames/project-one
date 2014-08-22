//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

cOutdoor* pOutdoor = NULL;
cWater*   pWater   = NULL;

// uebergang erst starten wenn ressourcen geladen


// ****************************************************************
// init the application
void CoreApp::Init()
{
    pOutdoor = new cOutdoor("grass", "dust", 1, 4.0f);
    pWater   = new cWater();

    Core::Graphics->SetLight(0, coreVector4(0.0f,0.0f,0.0f,0.0f),
                                coreVector4(coreVector3(1.0f,-1.1f,-0.85f).Normalize(), 0.0f),
                                coreVector4(0.0f,0.0f,0.0f,0.0f));

    Core::Graphics->SetCamera(coreVector3(0.0f,0.0f,200.0f), 
                              Core::Graphics->GetCamDirection(), 
                              Core::Graphics->GetCamOrientation());
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    SAFE_DELETE(pOutdoor)
    SAFE_DELETE(pWater)
}


// ****************************************************************
// render the application
void CoreApp::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND); // TODO: put in environment class
    {
        pOutdoor->Render();
        pWater->Render();
    }
    glEnable(GL_BLEND);
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    pOutdoor->Move();
    pWater->Move();
}