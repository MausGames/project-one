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
    // load post-processing shader-programs
    m_pProgramSimple    = Core::Manager::Resource->Get<coreProgram>("full_post_program");
    m_pProgramDistorted = Core::Manager::Resource->Get<coreProgram>("full_post_distorted_program");
    this->Recompile();

    // set object properties
    this->SetSize(coreVector2(1.0f,1.0f));
    this->Move();

    // create side-objects
    this->__Reset(CORE_RESOURCE_RESET_INIT);

    // create watermark
    m_Watermark.Construct   (MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_Watermark.SetPosition (coreVector2(-0.01f, 0.015f));
    m_Watermark.SetDirection(coreVector2( 1.0f,  0.0f));
    m_Watermark.SetCenter   (coreVector2( 0.5f, -0.5f));
    m_Watermark.SetAlignment(coreVector2(-1.0f,  1.0f));
    m_Watermark.SetText     ("Project One (@MausGames - work in progress)");
    m_Watermark.Move();

    // reset side-object opacity
    this->SetSideOpacity(0.0f);
}


// ****************************************************************
// apply post-processing
void cPostProcessing::Apply()
{
    // switch back to default frame buffer (again)
    coreFrameBuffer::EndDraw();

    // select between distorted or simple shader-program
    if(g_pDistortion->IsActive()) this->DefineProgram(m_pProgramDistorted);
                             else this->DefineProgram(m_pProgramSimple);

    // bind all required frame buffers
    this->DefineTexture(POST_TEXTURE_UNIT_ENVIRONMENT, g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture);
    this->DefineTexture(POST_TEXTURE_UNIT_FOREGROUND,  g_pForeground ->GetFrameBuffer()->GetColorTarget(0u).pTexture);
    this->DefineTexture(POST_TEXTURE_UNIT_GLOW,        g_pGlow       ->GetFrameBuffer()->GetColorTarget(0u).pTexture);
    this->DefineTexture(POST_TEXTURE_UNIT_DISTORTION,  g_pDistortion ->GetFrameBuffer()->GetColorTarget(0u).pTexture);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    {
        // post-process
        this->Render();

        // render side-objects
        for(coreUintW i = 0u; i < 2u; ++i) m_aSideArea[i].Render();
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // render watermark
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PRINTSCREEN), CORE_INPUT_PRESS))
        m_Watermark.Render();

    // invalidate all frame buffers
    g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture->Invalidate(0u);
    g_pForeground ->GetFrameBuffer()->GetColorTarget(0u).pTexture->Invalidate(0u);
    // # not cGlow, because of incremental rendering
    g_pDistortion ->GetFrameBuffer()->GetColorTarget(0u).pTexture->Invalidate(0u);
    this->DefineTexture(POST_TEXTURE_UNIT_ENVIRONMENT, NULL);
    this->DefineTexture(POST_TEXTURE_UNIT_FOREGROUND,  NULL);
    this->DefineTexture(POST_TEXTURE_UNIT_GLOW,        NULL);
    this->DefineTexture(POST_TEXTURE_UNIT_DISTORTION,  NULL);
}


// ****************************************************************
// recompile post-processing shader-programs
void cPostProcessing::Recompile()
{
#if !defined(CONFIG_FORCE)

    const coreChar* pcConfig1 =                g_CurConfig.Graphics.iGlow       ? SHADER_GLOW       : "";
    const coreChar* pcConfig2 = PRINT("%s %s", g_CurConfig.Graphics.iGlow       ? SHADER_GLOW       : "",
                                               g_CurConfig.Graphics.iDistortion ? SHADER_DISTORTION : "");

    // change configuration of post-processing shaders
    s_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post.frag")          ->GetResource())->SetCustomCode(pcConfig1);
    s_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post_distorted.frag")->GetResource())->SetCustomCode(pcConfig2);

    // recompile and relink
    m_pProgramSimple   .GetHandle()->Reload();
    m_pProgramDistorted.GetHandle()->Reload();

    // finish now
    coreSync::Finish();
    Core::Manager::Resource->UpdateResources();

#endif
}


// ****************************************************************
// set side-object opacity
void cPostProcessing::SetSideOpacity(const coreFloat& fValue)
{
    // change color instead of transparency (blending is disabled)
    const coreVector3 vColor = coreVector3(1.0f,1.0f,1.0f) * CLAMP(fValue, 0.0f, 1.0f);

    // update all relevant side-objects
    for(coreUintW i = 0u; i < 2u; ++i) m_aSideArea[i].SetColor3(vColor);
    m_Watermark.SetColor3(vColor * MENU_CONTRAST_WHITE);
}


// ****************************************************************
// reset with the resource manager
void cPostProcessing::__Reset(const coreResourceReset& bInit)
{
    if(bInit)
    {
        const coreVector2& vResolution = Core::System->GetResolution();

        // place objects left-right or top-down depending on window aspect ratio
        const coreVector2 vFlip = (vResolution.AspectRatio() < 1.0f) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
        for(coreUintW i = 0u; i < 2u; ++i)
        {
            const coreFloat fSide = (i ? 1.0f : -1.0f);

            // create side-objects
            m_aSideArea[i].DefineProgram("menu_border_program");
            m_aSideArea[i].DefineTexture(0u, "menu_background_black.png");
            m_aSideArea[i].SetPosition  (vFlip * (fSide *  0.1f));
            m_aSideArea[i].SetSize      ((vResolution - g_vGameResolution) / vResolution.yx() * 0.5f + vFlip.yx() + 0.1f);
            m_aSideArea[i].SetCenter    (vFlip * (fSide *  0.5f));
            m_aSideArea[i].SetAlignment (vFlip * (fSide * -1.0f));
            m_aSideArea[i].Move();
        }
    }
}