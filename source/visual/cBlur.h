///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BLUR_H_
#define _P1_GUARD_BLUR_H_

// TODO 5: use shader subroutines
// TODO 3: attenuation should be affected by update-rate (actually only for moving objects)


// ****************************************************************
// blur aggregation class
class cBlur final : public coreResourceRelation
{
private:
    coreFrameBuffer m_aFrameBuffer[2];   // blur frame buffers (reduced resolution)

    coreProgramPtr m_apConvProgram[2];   // shader-programs for separate convolution (X and Y)
    coreFullscreen m_Transformer;        // object to draw into the next stage

    coreFloat m_fScale;                  // frame buffer resolution factor
    coreFloat m_fAttenuation;            // blur shader attenuation


public:
    cBlur(const coreTextureSpec& oTextureSpec, const coreFloat fScale, const coreFloat fAttenuation)noexcept;

    DISABLE_COPY(cBlur)

    // start and end blur aggregation
    void Start();
    void End();

    // clear frame buffer
    inline void Clear() {m_aFrameBuffer[0].Clear(CORE_FRAMEBUFFER_TARGET_COLOR);}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_aFrameBuffer[0];}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;
};


#endif // _P1_GUARD_BLUR_H_