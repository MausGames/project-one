//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cPostProcessing::cPostProcessing()noexcept
{
    const coreVector2& vResolution = Core::System->GetResolution();

    // load post-processing shader-program
    this->DefineProgram("full_post_program");
    this->Recompile();

    // set object properties
    this->SetSize     (coreVector2(1.0f, 1.0f));
    this->SetTexSize  (coreVector2(1.0f,-1.0f));
    this->SetTexOffset(coreVector2(0.0f, 1.0f));
    this->Move();

    // place objects left-right or top-down depending on window aspect ratio
    const coreVector2 vFlip = (vResolution.AspectRatio() < 1.0f) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
    for(int i = 0; i < 2; ++i)
    {
        const float fSide = (i ? 1.0f : -1.0f);

        // create area side-objects
        m_aSideArea[i].DefineProgram("default_2d_program");
        m_aSideArea[i].DefineTexture(0, "menu_background_black.png");

        m_aSideArea[i].SetSize     ((vResolution - g_vGameResolution) / vResolution.yx() * 0.5f + vFlip.yx());
        m_aSideArea[i].SetCenter   (vFlip * (fSide *  0.5f));
        m_aSideArea[i].SetAlignment(vFlip * (fSide * -1.0f));
        m_aSideArea[i].SetTexSize  (m_aSideArea[i].GetSize() * 3.0f);
        m_aSideArea[i].Move();

        // create highlight side-objects
        m_aSideLine[i].DefineProgram("default_2d_program");
        m_aSideLine[i].DefineTexture(0, "default_white.png");

        m_aSideLine[i].SetPosition (vFlip * (fSide * (-m_aSideArea[i].GetSize() + 0.011f)));
        m_aSideLine[i].SetSize     (vFlip * 0.006f + vFlip.yx());
        m_aSideLine[i].SetCenter   (m_aSideArea[i].GetCenter());
        m_aSideLine[i].SetAlignment(m_aSideArea[i].GetAlignment());
        m_aSideLine[i].SetColor3   (coreVector3(0.8f,0.8f,0.8f));
        m_aSideLine[i].Move();
    }
}


// ****************************************************************
// destructor
cPostProcessing::~cPostProcessing()
{
}


// ****************************************************************
// apply post-processing
void cPostProcessing::Apply()
{
    // switch back to default frame buffer
    coreFrameBuffer::EndDraw();

    // bind all required frame buffers
    this->DefineTexture(0, g_pEnvironment->GetFrameBuffer()->GetColorTarget(0).pTexture);
    this->DefineTexture(1, g_pForeground ->GetFrameBuffer()->GetColorTarget(0).pTexture);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    {
        // post-process
        this->Render();

        // render side-objects
        for(int i = 0; i < 2; ++i) m_aSideArea[i].Render();
        for(int i = 0; i < 2; ++i) m_aSideLine[i].Render();
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // invalidate all frame buffers
    g_pEnvironment->GetFrameBuffer()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    g_pForeground ->GetFrameBuffer()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    this->DefineTexture(0, NULL);
    this->DefineTexture(1, NULL);
}


// ****************************************************************
// recompile post-processing shader-program
void cPostProcessing::Recompile()
{
    // set fragment shader configuration
    ((coreShader*)Core::Manager::Resource->Get<coreShader>("full_post.frag")->GetResource())
        ->SetCustomCode(""); // TODO #

    // recompile and relink
    m_pProgram.GetHandle()->Reload();

    // finish now
    glFinish();
    Core::Manager::Resource->UpdateResources();
}