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
cForeground::cForeground()noexcept
: m_mViewProj (coreMatrix4::Identity())
, m_bTarget   (false)
{
    // create foreground frame buffer (with alpha)
    m_FrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_FrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16);
    m_FrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create resolved texture (with alpha)
    m_ResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_ResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    this->__CalculateViewProj();
}


// ****************************************************************
// start foreground aggregation
void cForeground::Start()
{
    // fill foreground frame buffer
    m_FrameBuffer.StartDraw();
    m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);

    // adjust blending function (to correctly aggregate alpha values)
    glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);

    // 
    m_bTarget = true;
}


// ****************************************************************
// end foreground aggregation
void cForeground::End()
{
    // 
    m_bTarget = false;

    // reset blending function
    glBlendFunc(FOREGROUND_BLEND_DEFAULT);

    // resolve frame buffer to texture
    m_FrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_ResolvedTexture);
    m_FrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// 
coreBool cForeground::IsVisiblePoint(const coreVector2 vPosition, const coreFloat fFactor)const
{
    // 
    return ((ABS(vPosition.x) < FOREGROUND_AREA.x * fFactor) &&
            (ABS(vPosition.y) < FOREGROUND_AREA.y * fFactor));
}


// ****************************************************************
// 
coreBool cForeground::IsVisibleObject(const coreObject3D* pObject)const
{
    ASSERT(pObject)

    // 
    if(pObject->GetPosition().z >= CAMERA_POSITION.z - Core::Graphics->GetNearClip())
        return false;

    // 
    const coreVector2 vProjectedPos = this->Project3D(pObject->GetPosition());
    const coreFloat   fRadius       = pObject->GetVisualRadius();

    // 
    return ((ABS(vProjectedPos.x) < FOREGROUND_AREA.x * 1.1f + fRadius) &&
            (ABS(vProjectedPos.y) < FOREGROUND_AREA.y * 1.1f + fRadius));
}


// ****************************************************************
// 
coreFloat cForeground::RayIntersection(const coreVector2 vPosition, const coreVector2 vDirection, const coreFloat fFactor)const
{
    ASSERT(vDirection.IsNormalized())

    // 
    if(coreMath::IsNear(vDirection.x, 0.0f)) return (FOREGROUND_AREA.y * fFactor) - vPosition.y * vDirection.y;
    if(coreMath::IsNear(vDirection.y, 0.0f)) return (FOREGROUND_AREA.x * fFactor) - vPosition.x * vDirection.x;

    // 
    const coreVector2 vCorner = (FOREGROUND_AREA * fFactor) * vDirection.Processed(SIGN);
    const coreVector2 vDiff   = vCorner - vPosition;
    const coreVector2 vLength = vDiff / vDirection;

    return vLength.Min();
}


// ****************************************************************
// reset with the resource manager
void cForeground::__Reset(const coreResourceReset eInit)
{
    if(eInit) {m_FrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED); m_ResolvedTexture.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL); this->__CalculateViewProj();}
         else {m_FrameBuffer.Delete();                                                        m_ResolvedTexture.Delete();}
}


// ****************************************************************
// 
void cForeground::__CalculateViewProj()
{
    m_mViewProj = coreMatrix4::Camera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION) * coreMatrix4::Perspective(g_vGameResolution, Core::Graphics->GetFOV(), Core::Graphics->GetNearClip(), Core::Graphics->GetFarClip());
}