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
cBlur::cBlur(const coreFloat& fScale, const coreFloat& fAttenuation)noexcept
: m_fScale       (fScale)
, m_fAttenuation (fAttenuation)
{
    const coreVector2 vBlurResolution = g_vGameResolution * m_fScale;

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFrameBuffer); ++i)
    {
        // create blur frame buffers
        m_aFrameBuffer[i].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
        m_aFrameBuffer[i].Create(vBlurResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

        // add to stage list
        m_apBlurStage[i] = &m_aFrameBuffer[i];
    }

    // load shader-programs for separate convolution
    m_apBlurProgram[0] = Core::Manager::Resource->Get<coreProgram>("full_blur_1_program");
    m_apBlurProgram[1] = Core::Manager::Resource->Get<coreProgram>("full_blur_2_program");

    // create transformer object
    m_Transformer.SetSize(coreVector2(1.0f,1.0f) * m_fScale);
    m_Transformer.Move();
}


// ****************************************************************
// start blur aggregation
void cBlur::Start()
{
    if(Core::System->GetTime())
    {
        // swap stages and re-use last frame
        std::swap(m_apBlurStage[0], m_apBlurStage[2]);
    }

    // draw into first blur stage
    m_apBlurStage[0]->StartDraw();
}


// ****************************************************************
// end blur aggregation
void cBlur::End()
{
    if(Core::System->GetTime())
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        {
            // set temporal attenuation as alpha
            m_Transformer.SetAlpha(1.0f - m_fAttenuation * Core::System->GetTime());

            // forward to second blur stage (X convolution)
            m_apBlurStage[1]->StartDraw();
            {
                m_Transformer.DefineProgram(m_apBlurProgram[0]);
                m_Transformer.DefineTexture(0u, m_apBlurStage[0]->GetColorTarget(0u).pTexture);
                m_Transformer.Render();
            }

            // invalidate first blur stage
            m_apBlurStage[0]->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);

            // forward to final blur stage (Y convolution)
            m_apBlurStage[2]->StartDraw();
            {
                m_Transformer.DefineProgram(m_apBlurProgram[1]);
                m_Transformer.DefineTexture(0u, m_apBlurStage[1]->GetColorTarget(0u).pTexture);
                m_Transformer.Render();
            }

            // invalidate second blur stage
            m_apBlurStage[1]->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
    }
}


// ****************************************************************
// reset with the resource manager
void cBlur::__Reset(const coreResourceReset& bInit)
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