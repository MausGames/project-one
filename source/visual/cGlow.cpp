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
cGlow::cGlow()noexcept
{
    const coreVector2 vGlowResolution = g_vGameResolution * GLOW_SCALE_FACTOR;

    // create glow frame buffer
    m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_FrameBuffer.Create(vGlowResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create blur frame buffers
    m_aBlurStage[0].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_aBlurStage[0].Create(vGlowResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    m_aBlurStage[1].AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_aBlurStage[1].Create(vGlowResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // load shader-programs for separate convolution
    m_apBlurProgram[0] = Core::Manager::Resource->Get<coreProgram>("full_blur_1_program");
    m_apBlurProgram[1] = Core::Manager::Resource->Get<coreProgram>("full_blur_2_program");

    // create transformer object
    m_Transformer.SetSize(coreVector2(GLOW_SCALE_FACTOR, GLOW_SCALE_FACTOR));
    m_Transformer.Move();
}


// ****************************************************************
// update the glow-effect
void cGlow::Update()
{
    if(!g_CurConfig.Graphics.iGlow) return;

    // create glow only with active game
    if(g_pGame)
    {
        // draw into first blur stage
        m_aBlurStage[0].StartDraw();
        m_aBlurStage[0].Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
        {
            // draw single objects
            FOR_EACH(it, this->GetObjectSet())
                (*it)->Render();

            // draw lists with objects
            FOR_EACH(it, this->GetListSet())
                (*it)->Render();

            // always draw special-effects
            g_pSpecialEffects->Render();
        }

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        {
            // forward to second blur stage (X convolution)
            m_aBlurStage[1].StartDraw();
            {
                m_Transformer.DefineProgram(m_apBlurProgram[0]);
                m_Transformer.DefineTexture(0u, m_aBlurStage[0].GetColorTarget(0u).pTexture);
                m_Transformer.Render();
            }

            // invalidate first blur stage
            m_aBlurStage[0].Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);

            // forward to final glow frame buffer (Y convolution)
            m_FrameBuffer.StartDraw();
            {
                m_Transformer.DefineProgram(m_apBlurProgram[1]);
                m_Transformer.DefineTexture(0u, m_aBlurStage[1].GetColorTarget(0u).pTexture);
                m_Transformer.Render();
            }

            // invalidate second blur stage
            m_aBlurStage[1].Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
    }
    else m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// reset with the resource manager
void cGlow::__Reset(const coreResourceReset& bInit)
{
    if(bInit)
    {
        const coreVector2 vGlowResolution = g_vGameResolution * GLOW_SCALE_FACTOR;

        // re-create all frame buffers
        m_FrameBuffer  .Create(vGlowResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
        m_aBlurStage[0].Create(vGlowResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
        m_aBlurStage[1].Create(vGlowResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }
    else
    {
        // delete all frame buffers
        m_FrameBuffer  .Delete();
        m_aBlurStage[0].Delete();
        m_aBlurStage[1].Delete();
    }
}