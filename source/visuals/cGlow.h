//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_GLOW_H_
#define _P1_GUARD_GLOW_H_


//****************************************************************
// glow definitions
#define GLOW_SCALE_FACTOR (0.5f)   // resolution factor of the glow frame buffer


// ****************************************************************
// glow-effect class
class cGlow final : public cBindContainer, public coreResourceRelation
{
private:
    coreFrameBuffer m_iFrameBuffer;       // glow frame buffer (only texture, reduced resolution)

    coreFrameBuffer m_aiBlurStage  [2];   // blur frame buffers (stages for separate convolution, X and Y)
    coreProgramPtr  m_apBlurProgram[2];   // shader-programs for each stage
    coreObject2D    m_Transformer;        // object to draw into the next stage


public:
    cGlow()noexcept;

    DISABLE_COPY(cGlow)

    // update the glow-effect
    void Update();

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_iFrameBuffer;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_GLOW_H_