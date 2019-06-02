///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_FOREGROUND_H_
#define _P1_GUARD_FOREGROUND_H_

// TODO: #define FOREGROUND_AREA_FULL (FOREGROUND_AREA * 1.1f)
// TODO: FOREGROUND_AREA_REV without RCP
// TODO: switch back to multisampled framebuffer, to improve quality on Nvidia GPUs


// ****************************************************************
// foreground definitions
#define FOREGROUND_AREA (coreVector2(41.5f,41.5f))   // area range for the foreground plane (+/-, needs to be adapted after camera height change)

#define FOREGROUND_BLEND_DEFAULT (GL_SRC_ALPHA), (GL_ONE_MINUS_SRC_ALPHA)   // default blending mode  (A*x + (1-A)*y)
#define FOREGROUND_BLEND_SUM     (GL_SRC_ALPHA), (GL_ONE)                   // additive blending mode (A*x +       y)
#define FOREGROUND_BLEND_ALPHA   (GL_ONE),       (GL_ONE_MINUS_SRC_ALPHA)   // alpha aggregation      (  x + (1-A)*y)


// ****************************************************************
// foreground aggregation class
class cForeground final : public coreResourceRelation
{
private:
    coreFrameBuffer m_FrameBuffer;   // foreground frame buffer (only texture, in FBO for fast clear)
    coreMatrix4     m_mViewProj;     // foreground view-projection matrix


public:
    cForeground()noexcept;

    DISABLE_COPY(cForeground)

    // start and end foreground aggregation
    void Start();
    void End();

    // clear frame buffer
    inline void Clear() {m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_FrameBuffer;}

    // project world-position into screen-space
    inline FUNC_LOCAL coreVector2 Project3D(const coreVector3& vPosition)const {return vPosition.xy() * (CAMERA_POSITION.z * RCP(CAMERA_POSITION.z - vPosition.z));}
    inline FUNC_LOCAL coreVector2 Project2D(const coreVector3& vPosition)const {const coreVector4 A = coreVector4(vPosition, 1.0f) * m_mViewProj; return A.xy() * (RCP(A.w) * 0.5f);}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __CalculateViewProj();
};


#endif // _P1_GUARD_FOREGROUND_H_