//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_FOREGROUND_H_
#define _P1_GUARD_FOREGROUND_H_


// ****************************************************************
// foreground definitions
#define FOREGROUND_AREA (coreVector2(41.5f,41.5f))   // default range of the foreground plane (-0.5 to +0.5, needs to be adapted after camera height change)


// ****************************************************************
// foreground aggregation class
class cForeground final : public coreResourceRelation
{
private:
    coreFrameBuffer m_FrameBuffer;   // foreground frame buffer (only texture, but in FBO for fast clear)
    coreMatrix4     m_mViewProj;     // foreground view-projection matrix


public:
    cForeground()noexcept;

    DISABLE_COPY(cForeground)

    // start and end foreground aggregation
    void Start();
    void End();

    // project world-position into screen-space
    inline FUNC_PURE coreVector2 Project(const coreVector3& vPosition) {const coreVector4 A = coreVector4(vPosition, 1.0f) * m_mViewProj; return A.xy() * (RCP(A.w) * 0.5f);}

    // clear the foreground
    inline void Clear() {m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_FrameBuffer;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_FOREGROUND_H_