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
    this->SetSize(coreVector2(1.0f,1.0f));
    this->Move();

    // place objects left-right or top-down depending on window aspect ratio
    const coreVector2 vFlip = (vResolution.AspectRatio() < 1.0f) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
    for(coreUintW i = 0; i < 2; ++i)
    {
        const coreFloat fSide = (i ? 1.0f : -1.0f);

        // create side-objects
        m_aSideArea[i].DefineProgram("menu_border_program");
        m_aSideArea[i].DefineTexture(0, "menu_background_black.png");
        m_aSideArea[i].SetPosition  (vFlip * (fSide *  0.1f));
        m_aSideArea[i].SetSize      ((vResolution - g_vGameResolution) / vResolution.yx() * 0.5f + vFlip.yx() + 0.1f);
        m_aSideArea[i].SetCenter    (vFlip * (fSide *  0.5f));
        m_aSideArea[i].SetAlignment (vFlip * (fSide * -1.0f));
        m_aSideArea[i].Move();
    }

    // reset side-object opacity
    this->SetSideOpacity(0.0f);
}


// ****************************************************************
// apply post-processing
void cPostProcessing::Apply()
{

    //this->SetAlpha(g_pGame ? (1.0f - 0.2f*CLAMP(g_pGame->GetTimeMission()-2.0f, 0.0f, 1.0f)) : 1.0f);
    // shader!


    // switch back to default frame buffer (again)
    coreFrameBuffer::EndDraw();

    // bind all required frame buffers
    this->DefineTexture(POST_TEXTURE_UNIT_ENVIRONMENT, g_pEnvironment->GetFrameBuffer()->GetColorTarget(0).pTexture);
    this->DefineTexture(POST_TEXTURE_UNIT_FOREGROUND,  g_pForeground ->GetFrameBuffer()->GetColorTarget(0).pTexture);
    this->DefineTexture(POST_TEXTURE_UNIT_GLOW,        g_pGlow       ->GetFrameBuffer()->GetColorTarget(0).pTexture);
    this->DefineTexture(POST_TEXTURE_UNIT_DISTORTION,  g_pDistortion ->GetFrameBuffer()->GetColorTarget(0).pTexture);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    {
        // post-process
        this->Render();

        // render side-objects
        for(coreUintW i = 0; i < 2; ++i) m_aSideArea[i].Render();
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // invalidate all frame buffers
    g_pEnvironment->GetFrameBuffer()->GetColorTarget(0).pTexture->Invalidate(0);
    g_pForeground ->GetFrameBuffer()->GetColorTarget(0).pTexture->Invalidate(0);
    g_pGlow       ->GetFrameBuffer()->GetColorTarget(0).pTexture->Invalidate(0);
    g_pDistortion ->GetFrameBuffer()->GetColorTarget(0).pTexture->Invalidate(0);
    this->DefineTexture(POST_TEXTURE_UNIT_ENVIRONMENT, NULL);
    this->DefineTexture(POST_TEXTURE_UNIT_FOREGROUND,  NULL);
    this->DefineTexture(POST_TEXTURE_UNIT_GLOW,        NULL);
    this->DefineTexture(POST_TEXTURE_UNIT_DISTORTION,  NULL);
}


// ****************************************************************
// recompile post-processing shader-program
void cPostProcessing::Recompile()
{
    const coreChar* pcConfig = PRINT("%s", g_CurConfig.Graphics.iGlow ? SHADER_GLOW : "");

    // change configuration of post-processing shader
    ((coreShader*)Core::Manager::Resource->Get<coreShader>("full_post.frag")->GetResource())
        ->SetCustomCode(pcConfig);

    // recompile and relink
    m_pProgram.GetHandle()->Reload();

    // finish now
    glFinish();
    Core::Manager::Resource->UpdateResources();
}


// ****************************************************************
// set side-object opacity
void cPostProcessing::SetSideOpacity(const coreFloat& fValue)
{
    // change color instead of transparency (blending is disabled)
    const coreVector3 vColor = coreVector3(1.0f,1.0f,1.0f) * CLAMP(fValue, 0.0f, 1.0f);
    for(coreUintW i = 0; i < 2; ++i) m_aSideArea[i].SetColor3(vColor);
}