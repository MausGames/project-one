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
#define GLOW_SCALE_FACTOR        (0.5f)   // resolution factor of the glow frame buffer
#define GLOW_ATTENUATION_FACTOR (15.0f)   // 


// ****************************************************************
// glow-effect class
class cGlow final : public cBindContainer
{
private:
    cBlur m_Blur;   // blur aggregation object (glow frame buffer)


public:
    cGlow()noexcept;

    DISABLE_COPY(cGlow)

    // update the glow-effect
    void Update();

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return m_Blur.GetFrameBuffer();}
};


#endif // _P1_GUARD_GLOW_H_