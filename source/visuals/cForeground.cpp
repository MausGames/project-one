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
cForeground::cForeground()noexcept
: m_mViewProj (Core::Graphics->GetCamera() * coreMatrix4::Perspective(g_vGameResolution, Core::Graphics->GetFOV(), Core::Graphics->GetNearClip(), Core::Graphics->GetFarClip()))
{
    // create foreground frame buffer (texture with alpha)
    m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGBA);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
}


// ****************************************************************
// destructor
cForeground::~cForeground()
{
}


// ****************************************************************
// start foreground aggregation
void cForeground::Start()
{
    // switch back to default frame buffer
    coreFrameBuffer::EndDraw();

    // set foreground camera and light
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);
    Core::Graphics->SetLight (0, coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(LIGHT_DIRECTION, 0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));

    // adjust blending function (to correctly aggregate alpha values)
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // clear color buffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
}


// ****************************************************************
// end foreground aggregation
void cForeground::End()
{
    // copy screen space to texture (faster than dedicated multisampled FBO, but only once)
    m_iFrameBuffer.GetColorTarget(0).pTexture->Enable(0);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
  	                    F_TO_SI(Core::Graphics->GetViewResolution().x - m_iFrameBuffer.GetResolution().x) / 2,
  	                    F_TO_SI(Core::Graphics->GetViewResolution().y - m_iFrameBuffer.GetResolution().y) / 2,
  	                    F_TO_SI(m_iFrameBuffer.GetResolution().x),
  	                    F_TO_SI(m_iFrameBuffer.GetResolution().y));

    // explicitly invalidate color and depth buffer
    if(CORE_GL_SUPPORT(ARB_invalidate_subdata))
    {
        constexpr_var GLenum aiAttachment[2] = {GL_COLOR, GL_DEPTH};
        glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, aiAttachment);
    }

    // reset blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// ****************************************************************
// reset with the resource manager
void cForeground::__Reset(const coreResourceReset& bInit)
{
    if(bInit) m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
         else m_iFrameBuffer.Delete();
}