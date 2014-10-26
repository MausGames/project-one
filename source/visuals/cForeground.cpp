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
cForeground::cForeground()
: m_mViewProj (Core::Graphics->GetCamera() * coreMatrix4::Perspective(g_vGameResolution, Core::Graphics->GetFOV(), Core::Graphics->GetNearClip(), Core::Graphics->GetFarClip()))
{
#if defined(_FOREGROUND_FRAMEBUFFER_)

    // create foreground frame buffer (with alpha)
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGBA);
    m_iFrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0, CORE_TEXTURE_SPEC_DEPTH);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

#endif

    // create resolved texture (with alpha)
    m_iResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGBA);
    m_iResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
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
    // set foreground camera and light
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);
    Core::Graphics->SetLight (0, coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(LIGHT_DIRECTION, 0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));

#if defined(_FOREGROUND_FRAMEBUFFER_)

    // fill foreground frame buffer
    m_iFrameBuffer.StartDraw();
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

#else

    // switch back to default frame buffer
    coreFrameBuffer::EndDraw();

    // clear color buffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

#endif
}


// ****************************************************************
// end foreground aggregation
void cForeground::End()
{
#if defined(_FOREGROUND_FRAMEBUFFER_)

    // resolve frame buffer to texture
    m_iFrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_iResolvedTexture);
    m_iFrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

#else

    // copy screen space to texture
    m_iResolvedTexture.GetColorTarget(0).pTexture->Enable(0);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
  	                    F_TO_SI(Core::Graphics->GetViewResolution().x - m_iResolvedTexture.GetResolution().x) / 2,
  	                    F_TO_SI(Core::Graphics->GetViewResolution().y - m_iResolvedTexture.GetResolution().y) / 2,
  	                    F_TO_SI(m_iResolvedTexture.GetResolution().x),
  	                    F_TO_SI(m_iResolvedTexture.GetResolution().y));

    // explicitly invalidate color and depth buffer 
    if(CORE_GL_SUPPORT(ARB_invalidate_subdata))
    {
        constexpr_var GLenum aiAttachment[2] = {GL_COLOR, GL_DEPTH};
        glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, aiAttachment);
    }

#endif
}


// ****************************************************************
// reset with the resource manager
void cForeground::__Reset(const coreResourceReset& bInit)
{
#if defined(_FOREGROUND_FRAMEBUFFER_)

    // handle foreground frame buffers
    if(bInit) m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);
         else m_iFrameBuffer.Delete();

#endif

    // handle resolved texture
    if(bInit) m_iResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
         else m_iResolvedTexture.Delete();
}