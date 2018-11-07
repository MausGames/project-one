///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cPostProcessing::cPostProcessing()noexcept
{
    // load post-processing shader-programs
    m_pProgramSimple    = Core::Manager::Resource->Get<coreProgram>("full_post_program");
    m_pProgramDistorted = Core::Manager::Resource->Get<coreProgram>("full_post_distorted_program");
    m_pProgramDebug     = Core::Manager::Resource->Get<coreProgram>("full_post_debug_program");
    this->Recompile();

    // set object properties
    this->SetSize(coreVector2(1.0f,1.0f));
    this->Move();

    // create side-objects
    this->__Reset(CORE_RESOURCE_RESET_INIT);

    // create watermark
    m_Watermark.Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_Watermark.SetPosition (coreVector2(-0.01f, 0.015f));
    m_Watermark.SetDirection(coreVector2( 1.0f,  0.0f));
    m_Watermark.SetCenter   (coreVector2( 0.5f, -0.5f));
    m_Watermark.SetAlignment(coreVector2(-1.0f,  1.0f));
    m_Watermark.SetText     ("Project One (@MausGames - work in progress)");
    m_Watermark.Move();

    // reset side-object opacity
    this->SetSideOpacity(0.0f);

    // 
    this->SetSaturation(1.0f);
    this->SetValue     (1.0f);
}


// ****************************************************************
// apply post-processing
void cPostProcessing::Apply()
{
    // switch back to default frame buffer (again)
    coreFrameBuffer::EndDraw();

    // select between debug, distorted or simple shader-program
                    if(g_bDebugOutput) this->DefineProgram(m_pProgramDebug);
    else if(g_pDistortion->IsActive()) this->DefineProgram(m_pProgramDistorted);
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
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSideArea); ++i) m_aSideArea[i].Render();
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // render watermark
    if(g_MenuInput.bScreenshot)
        m_Watermark.Render();

    // invalidate all frame buffers (# not cGlow and cDistortion, because of incremental rendering and pause)
    if(!g_pMenu->IsPaused())
    {
        g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture->Invalidate(0u);
        g_pForeground ->GetFrameBuffer()->GetColorTarget(0u).pTexture->Invalidate(0u);
    }
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

    const coreChar* pcConfig1 =               g_CurConfig.Graphics.iGlow       ? SHADER_GLOW       : "";
    const coreChar* pcConfig2 = PRINT("%s%s", g_CurConfig.Graphics.iDistortion ? SHADER_DISTORTION : "", pcConfig1);
    const coreChar* pcConfig3 = PRINT("%s%s",                                    SHADER_DEBUG,           pcConfig2);

    // change configuration of post-processing shaders
    d_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post.frag")          ->GetRawResource())->SetCustomCode(pcConfig1);
    d_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post_distorted.frag")->GetRawResource())->SetCustomCode(pcConfig2);
    d_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post_debug.frag")    ->GetRawResource())->SetCustomCode(pcConfig3);

    // recompile and relink
    m_pProgramSimple   .GetHandle()->Reload();
    m_pProgramDistorted.GetHandle()->Reload();
    m_pProgramDebug    .GetHandle()->Reload();

    // finish now
    if(Core::System->GetCurFrame())
    {
        coreSync::Finish();
        Core::Manager::Resource->UpdateResources();
    }

#endif
}


// ****************************************************************
// set side-object opacity
void cPostProcessing::SetSideOpacity(const coreFloat fValue)
{
    // change color instead of transparency (blending is disabled)
    const coreVector3 vColor = coreVector3(1.0f,1.0f,1.0f) * CLAMP(fValue, 0.0f, 1.0f);

    // update all relevant side-objects
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSideArea); ++i) m_aSideArea[i].SetColor3(vColor);
    m_Watermark.SetColor3(vColor * MENU_CONTRAST_WHITE);
}


// ****************************************************************
// reset with the resource manager
void cPostProcessing::__Reset(const coreResourceReset bInit)
{
    if(bInit)
    {
        const coreVector2& vResolution = Core::System->GetResolution();

        // place objects left-right or top-down depending on window aspect ratio
        const coreVector2 vFlip = (vResolution.AspectRatio() < 1.0f) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSideArea); ++i)
        {
            const coreFloat fSide = (i ? 1.0f : -1.0f);

            // create side-objects
            m_aSideArea[i].DefineProgram("menu_border_direct_program");
            m_aSideArea[i].DefineTexture(0u, "menu_background_black.png");
            m_aSideArea[i].SetPosition  (vFlip * (fSide *  0.1f));
            m_aSideArea[i].SetSize      (coreVector2(1.0f, ((vResolution - g_vGameResolution) / vResolution.yx()).Max() * 0.5f) + 0.1f);
            m_aSideArea[i].SetDirection (vFlip * (fSide *  coreVector2(-1.0f,1.0f)));
            m_aSideArea[i].SetCenter    (vFlip * (fSide *  0.5f));
            m_aSideArea[i].SetAlignment (vFlip * (fSide * -1.0f));
            m_aSideArea[i].SetTexOffset (coreVector2(0.1f, 0.0f));
            m_aSideArea[i].Move();
        }
    }
}