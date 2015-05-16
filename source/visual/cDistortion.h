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
#define DISTORTION_SCALE_FACTOR (0.5f)            // resolution factor of the distortion frame buffer
#define DISTORTION_WAVES        (4u)              // number of distortion-waves
#define DISTORTION_BURSTS       (4u)              // number of distortion-bursts

#define DISTORTION_WAVE_SMALL   ( 2.5f), (3.0f)   // 
#define DISTORTION_WAVE_BIG     ( 5.0f), (3.0f)   // 
#define DISTORTION_WAVE_BIG_REV (-3.0f), (1.7f)   // 


// ****************************************************************
// distortion-effect class
class cDistortion final : public coreResourceRelation
{
private:
    coreFrameBuffer m_iFrameBuffer;             // distortion frame buffer (only 2-channel texture, reduced resolution)

    coreObject2D m_aWave[DISTORTION_WAVES];     // 
    coreUintW    m_iCurWave;                    // 

    coreObject2D m_aBurst[DISTORTION_BURSTS];   // 
    coreUintW    m_iCurBurst;                   // 


public:
    cDistortion()noexcept;

    DISABLE_COPY(cDistortion)

    // update the distortion-effect
    void Update();

    // create distortion-objects
    void CreateWave (const coreVector3& vPosition,                                const coreFloat& fScale, const coreFloat& fSpeed);
    void CreateBurst(const coreVector3& vPosition, const coreVector2& vDirection, const coreFloat& fScale, const coreFloat& fSpeed);

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_iFrameBuffer;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_DISTORTION_H_