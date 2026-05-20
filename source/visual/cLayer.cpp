///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cLayer::cLayer()noexcept
{
    // create layer frame buffer (with alpha)
    m_FrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_FrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16);
    m_FrameBuffer.Create(Core::System->GetResolution(), CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create resolved texture (with alpha)
    m_ResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8, CORE_TEXTURE_MODE_DEFAULT);
    m_ResolvedTexture.Create(Core::System->GetResolution(), CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    m_Fullscreen.DefineProgram("full_post_layer_program");
    m_Fullscreen.DefineTexture(POST_TEXTURE_UNIT_ENVIRONMENT, "default_black.webp");
    m_Fullscreen.DefineTexture(POST_TEXTURE_UNIT_GLOW,        "default_black.webp");
}


// ****************************************************************
// 
void cLayer::Start()
{
    // fill layer frame buffer
    m_FrameBuffer.StartDraw();
    m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

    // adjust blending function (to correctly aggregate alpha values)
    glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);

    // 
    const coreVector3 vCamOri = MapToAxisInv(CAMERA_ORIENTATION, g_pPostProcessing->GetDirection() * coreVector2(g_CurConfig.Game.iMirrorMode ? -1.0f : 1.0f, 1.0f));
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, vCamOri);   // do not reset at the end

    if(g_CurConfig.Game.iMirrorMode)
    {
        // 
        coreMatrix4 mPerspective = Core::Graphics->GetPerspective(); mPerspective._11 *= -1.0f;
        Core::Graphics->OverridePerspective(mPerspective);

        // 
        glCullFace(GL_FRONT);
    }
}


// ****************************************************************
// 
void cLayer::End()
{
    // 
    if(g_CurConfig.Game.iMirrorMode) glCullFace(GL_BACK);

    // reset blending function
    glBlendFunc(FOREGROUND_BLEND_DEFAULT);

    // resolve frame buffer to texture
    m_FrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_ResolvedTexture);
    m_FrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

    // 
    coreFrameBuffer::EndDraw();

    // 
    m_Fullscreen.DefineTexture(POST_TEXTURE_UNIT_FOREGROUND, m_ResolvedTexture.GetColorTarget(0u).pTexture);
    m_Fullscreen.DefineTexture(POST_TEXTURE_UNIT_DISTORTION, g_pDistortion->GetFrameBuffer()->GetColorTarget(0u).pTexture);
    m_Fullscreen.SetSize      (Core::System->GetCanonSize());
    m_Fullscreen.SetColor4    (g_pPostProcessing->GetInteriorColor());
    m_Fullscreen.Move();
    m_Fullscreen.Render();

    // 
    m_ResolvedTexture.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// reset with the resource manager
void cLayer::__Reset(const coreResourceReset eInit)
{
    if(eInit) {m_FrameBuffer.Create(Core::System->GetResolution(), CORE_FRAMEBUFFER_CREATE_MULTISAMPLED); m_ResolvedTexture.Create(Core::System->GetResolution(), CORE_FRAMEBUFFER_CREATE_NORMAL);}
         else {m_FrameBuffer.Delete();                                                                    m_ResolvedTexture.Delete();}
}