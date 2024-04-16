///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_DISTORTION_H_
#define _P1_GUARD_DISTORTION_H_

// TODO 1: [MF] when resetting resource-manager, while distortion is active during pause, the whole screen is shifted


// ****************************************************************
// distortion definitions
#define DISTORTION_SCALE_FACTOR (SCALE_FACTOR)   // frame buffer resolution factor
#define DISTORTION_WAVES        (8u)             // number of distortion-waves
#define DISTORTION_BURSTS       (4u)             // number of distortion-bursts

#define DISTORTION_WAVE_TINY    ( 1.2f), (3.0f)
#define DISTORTION_WAVE_SMALL   ( 2.5f), (3.0f)
#define DISTORTION_WAVE_BIG     ( 5.0f), (3.0f)
#define DISTORTION_WAVE_BIG_REV (-3.0f), (1.7f)
#define DISTORTION_BURST_SMALL  ( 2.5f), (3.0f)
#define DISTORTION_BURST_BIG    ( 5.0f), (3.0f)


// ****************************************************************
// distortion-effect class
class cDistortion final : public coreResourceRelation
{
private:
    coreFrameBuffer m_FrameBuffer;              // distortion frame buffer (reduced resolution)

    coreObject2D m_aWave[DISTORTION_WAVES];     // 
    coreUintW    m_iCurWave;                    // 

    coreObject2D m_aBurst[DISTORTION_BURSTS];   // 
    coreUintW    m_iCurBurst;                   // 

    coreBool m_bActive;                         // 


public:
    cDistortion()noexcept;

    DISABLE_COPY(cDistortion)

    // update the distortion-effect
    void Update();

    // create distortion-objects
    void CreateWave (const coreVector3 vPosition,                               const coreFloat fScale, const coreFloat fSpeed);
    void CreateBurst(const coreVector3 vPosition, const coreVector2 vDirection, const coreFloat fScale, const coreFloat fSpeed);

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_FrameBuffer;}

    // 
    inline const coreBool& IsActive()const {return m_bActive;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;
};


#endif // _P1_GUARD_DISTORTION_H_