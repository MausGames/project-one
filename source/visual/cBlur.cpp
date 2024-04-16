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
cBlur::cBlur(const coreTextureSpec& oTextureSpec, const coreFloat fScale, const coreFloat fAttenuation)noexcept
: m_fScale       (fScale)
, m_fAttenuation (fAttenuation)
{
    const coreVector2 vBlurResolution = g_vGameResolution * m_fScale;

    // create blur frame buffers
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFrameBuffer); ++i)
    {
        m_aFrameBuffer[i].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, oTextureSpec);
        m_aFrameBuffer[i].Create(vBlurResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }

    // load shader-programs for separate convolution
    m_apConvProgram[0] = Core::Manager::Resource->Get<coreProgram>("full_blur_1x_program");
    m_apConvProgram[1] = Core::Manager::Resource->Get<coreProgram>("full_blur_2y_program");

    // create transformer object
    m_Transformer.SetSize (coreVector2(1.0f,1.0f) * m_fScale);
    m_Transformer.SetAlpha(1.0f - m_fAttenuation * FRAMERATE_TIME);
    m_Transformer.Move();
}


// ****************************************************************
// start blur aggregation
void cBlur::Start()
{
    // draw into first blur stage
    m_aFrameBuffer[0].StartDraw();
}


// ****************************************************************
// end blur aggregation
void cBlur::End()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    {
        // forward to second blur stage (X convolution)
        m_aFrameBuffer[1].StartDraw();
        {
            m_Transformer.DefineTexture(0u, m_aFrameBuffer[0].GetColorTarget(0u).pTexture);
            m_Transformer.Render(m_apConvProgram[0]);
        }

        // invalidate first blur stage
        m_aFrameBuffer[0].Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);

        // forward to final blur stage (Y convolution)
        m_aFrameBuffer[0].StartDraw();
        {
            m_Transformer.DefineTexture(0u, m_aFrameBuffer[1].GetColorTarget(0u).pTexture);
            m_Transformer.Render(m_apConvProgram[1]);
        }

        // invalidate second blur stage
        m_aFrameBuffer[1].Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}


// ****************************************************************
// reset with the resource manager
void cBlur::__Reset(const coreResourceReset bInit)
{
    if(bInit)
    {
        const coreVector2 vBlurResolution = g_vGameResolution * m_fScale;

        // re-create all frame buffers
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFrameBuffer); ++i)
            m_aFrameBuffer[i].Create(vBlurResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }
    else
    {
        // delete all frame buffers
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFrameBuffer); ++i)
            m_aFrameBuffer[i].Delete();
    }
}