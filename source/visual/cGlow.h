///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_GLOW_H_
#define _P1_GUARD_GLOW_H_

// TODO 2: glow is cleared abruptly when getting inactive, should afterglow instead
// TODO 4: remove exposed clear function (location where this is called should use a different way)


// ****************************************************************
// glow definitions
#define GLOW_SCALE_FACTOR       (SCALE_FACTOR)   // frame buffer resolution factor
#define GLOW_ATTENUATION_FACTOR (0.43f)          // blur shader attenuation (higher value = weaker glow)


// ****************************************************************
// glow-effect class
class cGlow final : public cBindContainerIn
{
private:
    cBlur m_Blur;         // blur aggregation object (glow frame buffer)

    coreBool m_bActive;   // 


public:
    cGlow()noexcept;

    DISABLE_COPY(cGlow)

    // update the glow-effect
    void Update();

    // clear frame buffer
    inline void Clear() {m_Blur.Clear();}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return m_Blur.GetFrameBuffer();}

    // 
    inline const coreBool& IsActive()const {return m_bActive;}
};


#endif // _P1_GUARD_GLOW_H_