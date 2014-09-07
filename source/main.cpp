//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

coreVector2 g_vGameResolution = coreVector2(0.0f,0.0f);



coreFrameBuffer* pFrameBuffer = NULL;   // no pointer later

cOutdoor* pOutdoor = NULL;
cWater*   pWater   = NULL;

coreObject2D aSideArea[2];
coreObject2D aSideLine[2];

coreLabel* pFPS = NULL;


// uebergang erst starten wenn ressourcen geladen
// restart required rot anzeigen
// flimmerndes schiff
// reflexion streuen ?
// background wechsel mit oben-unten, links-rechts, schräg, oder ring-animation (gegen fluchrichtung ?)
// 4 interface alternativen (innen-außen, seitlich-unten)



// ****************************************************************
// init the application
void CoreApp::Init()
{
    const coreVector2& vResolution = Core::System->GetResolution();

    const float fMinRes = vResolution.Min();
    g_vGameResolution   = coreVector2(fMinRes, fMinRes);


    pFrameBuffer = new coreFrameBuffer();
    pFrameBuffer->AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    pFrameBuffer->AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0, CORE_TEXTURE_SPEC_DEPTH);
    pFrameBuffer->Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    pOutdoor = new cOutdoor("grass", "dust", 1, 4.0f);
    pWater   = new cWater();


    const coreVector2 vFlip = (vResolution.AspectRatio() < 1.0f) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
    for(int i = 0; i < 2; ++i)
    {
        const float fSide = (i ? 1.0f : -1.0f);

        aSideArea[i].DefineProgram("default_2d_program");
        aSideArea[i].DefineTexture(0, "menu_background_black.png");

        aSideArea[i].SetSize     ((vResolution - g_vGameResolution) / vResolution.yx() * 0.5f + vFlip.yx());
        aSideArea[i].SetCenter   (vFlip * (fSide *  0.5f));
        aSideArea[i].SetAlignment(vFlip * (fSide * -1.0f));
        aSideArea[i].SetTexSize  (aSideArea[i].GetSize() * 3.0f);
        aSideArea[i].Move();
        
        aSideLine[i].DefineProgram("default_2d_program");
        aSideLine[i].DefineTexture(0, "default_white.png");

        aSideLine[i].SetPosition (vFlip * (fSide * (-aSideArea[i].GetSize() + 0.011f)));
        aSideLine[i].SetSize     (vFlip * 0.006f + vFlip.yx());
        aSideLine[i].SetCenter   (aSideArea[i].GetCenter());
        aSideLine[i].SetAlignment(aSideArea[i].GetAlignment());
        aSideLine[i].SetColor3   (coreVector3(0.8f,0.8f,0.8f));
        aSideLine[i].Move();
    }


    pFPS = new coreLabel();
    pFPS->Construct("ethnocentric.ttf", 24, 8);
    pFPS->SetPosition (coreVector2(0.008f, 0.0f));
    pFPS->SetCenter   (coreVector2( -0.5f, 0.5f));
    pFPS->SetAlignment(coreVector2(  1.0f,-1.0f));


    Core::Graphics->SetLight(0, coreVector4(0.0f,0.0f,0.0f,0.0f),
                                coreVector4(coreVector3(1.0f,-1.1f,-0.85f).Normalize(), 0.0f),
                                coreVector4(0.0f,0.0f,0.0f,0.0f));

    Core::Graphics->SetCamera(coreVector3(0.0f,0.0f,110.0f), 
                              Core::Graphics->GetCamDirection(), 
                              Core::Graphics->GetCamOrientation());
}


// ****************************************************************
// exit the application
void CoreApp::Exit()
{
    SAFE_DELETE(pFrameBuffer)

    SAFE_DELETE(pOutdoor)
    SAFE_DELETE(pWater)

    for(int i = 0; i < 2; ++i)
    {
        aSideArea[i].Undefine();
        aSideLine[i].Undefine();
    }

    SAFE_DELETE(pFPS)
}


// ****************************************************************
// render the application
void CoreApp::Render()
{

    pWater->UpdateReflection();

    pFrameBuffer->StartDraw();
    {
        pFrameBuffer->Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
        glDisable(GL_BLEND); // TODO: put in environment class
        {
            pOutdoor->Render();
            pWater->Render(pFrameBuffer);
        }
        glEnable(GL_BLEND);

    }
    coreFrameBuffer::EndDraw();


    pFrameBuffer->Blit(CORE_FRAMEBUFFER_TARGET_COLOR, NULL, 0, 0, 
                       coreUint(Core::System->GetResolution().x - g_vGameResolution.x) / 2,
                       coreUint(Core::System->GetResolution().y - g_vGameResolution.y) / 2, 
                       coreUint(pFrameBuffer->GetResolution().x),
                       coreUint(pFrameBuffer->GetResolution().y));

    pFrameBuffer->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);


    glDisable(GL_DEPTH_TEST);
    {
        for(int i = 0; i < 2; ++i)
        {
            aSideArea[i].Render();
            aSideLine[i].Render();
        }
        pFPS->Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// move the application
void CoreApp::Move()
{
    if(Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_HOLD)) 
    {
        const float fMove = Core::Input->GetMouseRelative().y * -20.0f;
        pOutdoor->SetMoveOffset(pOutdoor->GetMoveOffset() + fMove);
        pWater->SetMoveOffset  (pWater->GetMoveOffset()   + fMove);
    }


    pOutdoor->Move();
    pWater->Move();


    pFPS->SetText(PRINT("%.1f", 1.0f / Core::System->GetTime()));
    pFPS->Move();
}