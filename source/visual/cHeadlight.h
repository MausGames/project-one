///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_HEADLIGHT_H_
#define _P1_GUARD_HEADLIGHT_H_

// TODO: make darkness configurable
// TODO: reduce black/transparent (0,0,0) color-areas in light-textures
// TODO: slow up and down glowing
// TODO: old flickering effect and sound
// TODO: activation-animation (e.g. for player or boss spot-lights)
// TODO: use simpler shader for light (single-channel only)


// ****************************************************************
// 
#define HEADLIGHT_SCALE_FACTOR (0.5f)   // frame buffer resolution factor
#define HEADLIGHT_INTENSITY    (1.5f)   // 


// ****************************************************************
// 
class cHeadlight final : public coreFullscreen, public coreResourceRelation
{
private:
    // 
    struct sSpotCommand final
    {
        coreVector2 vPosition;    // 
        coreVector2 vSize;        // 
        coreVector2 vDirection;   // 
    };

    // 
    struct sPointCommand final
    {
        coreVector2 vPosition;    // 
        coreVector2 vSize;        // 
    };


private:
    coreFrameBuffer m_FrameBuffer;             // 

    coreList<sSpotCommand>  m_aSpotCommand;    // 
    coreList<sPointCommand> m_aPointCommand;   // 

    coreObject2D m_Spot;                       // 
    coreObject2D m_Point;                      // 

    coreSoundPtr m_pFlickerSound;              // 
    coreSoundPtr m_pShatterSound;              // 

    coreTimer m_Flicker;                       // 
    coreUint8 m_iShatter;                      // 


public:
    cHeadlight()noexcept;
    ~cHeadlight()final;

    DISABLE_COPY(cHeadlight)

    // 
    void Render()final;

    // 
    void Update();
    void UpdateDefault();

    // 
    void DrawSpot (const coreVector3& vPosition, const coreVector2& vSize, const coreVector2& vDirection);
    void DrawPoint(const coreVector3& vPosition, const coreVector2& vSize);
    void DrawPoint(const coreObject3D* pObject);

    // 
    void PlayFlicker (const coreUint8 iShatter);
    void StopFlicker ();
    void ResetFlicker();

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_FrameBuffer;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;
};


#endif // _P1_GUARD_HEADLIGHT_H_