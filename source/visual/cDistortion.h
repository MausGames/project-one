//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_DISTORTION_H_
#define _P1_GUARD_DISTORTION_H_


//****************************************************************
// distortion definitions
#define DISTORTION_SCALE_FACTOR (0.5f)   // resolution factor of the distortion frame buffer 


// ****************************************************************
// distortion-effect class
class cDistortion final : public coreResourceRelation
{
private:
    coreFrameBuffer m_iFrameBuffer;   // distortion frame buffer (only texture, reduced resolution) 


public:
    cDistortion()noexcept;

    DISABLE_COPY(cDistortion)

    // update the distortion-effect
    void Update();

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_iFrameBuffer;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_DISTORTION_H_