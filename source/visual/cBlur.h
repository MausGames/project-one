//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BLUR_H_
#define _P1_GUARD_BLUR_H_

// TODO: check if uniform-branching is better then two separate shader-programs


// ****************************************************************
// blur aggregation class
class cBlur final : public coreResourceRelation
{
private:
    coreFrameBuffer  m_aFrameBuffer [3];   // blur frame buffers (only texture, reduced resolution)

    coreFrameBuffer* m_apBlurStage  [3];   // stage list (to change order dynamically)
    coreProgramPtr   m_apBlurProgram[2];   // shader-programs for separate convolution (X and Y)
    coreObject2D     m_Transformer;        // object to draw into the next stage

    coreFloat m_fScale;                    // resolution factor
    coreFloat m_fAttenuation;              // 


public:
    cBlur(const coreFloat& fScale, const coreFloat& fAttenuation)noexcept;

    DISABLE_COPY(cBlur)

    // start and end blur aggregation
    void Start();
    void End();

    // clear frame buffer
    inline void Clear() {m_apBlurStage[2]->Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return m_apBlurStage[2];}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_BLUR_H_