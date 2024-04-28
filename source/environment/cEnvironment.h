///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENVIRONMENT_H_
#define _P1_GUARD_ENVIRONMENT_H_

// TODO 5: separate culling between normal rendering and shadow, to improve efficiency (though this would update instancing buffer again ?)
// TODO 3: if m_fSideOffset will be used with rotation, make sure to use smooth lerp instead of the linear
// TODO 3: there is a 1-frame delay for TargetNow functions


// ****************************************************************
// environment definitions
#define ENVIRONMENT_MIX_FADE          (0u)     // 
#define ENVIRONMENT_MIX_WIPE          (1u)     // 
#define ENVIRONMENT_MIX_CURTAIN       (2u)     // 
#define ENVIRONMENT_MIX_CIRCLE        (3u)     // 
#define ENVIRONMENT_MIXES             (4u)     // 

#define ENVIRONMENT_TRANSITION_FACTOR (0.9f)   // 
#define ENVIRONMENT_SCALE_FACTOR      (!g_CurConfig.Graphics.iRender ? 0.7f : (g_bHandheld ? 0.8f : 1.0f))   // 

#define ENVIRONMENT_DEFAULT_DIRECTION (coreVector2(0.0f,1.0f))
#define ENVIRONMENT_DEFAULT_SIDE      (coreVector2(0.0f,0.0f))
#define ENVIRONMENT_DEFAULT_SPEED     (1.0f)
#define ENVIRONMENT_DEFAULT_HEIGHT    (0.0f)


// ****************************************************************
// environment class
class cEnvironment final : public coreResourceRelation
{
private:
    cBackground* m_pBackground;                          // current background instance (should never be NULL)
    cBackground* m_pOldBackground;                       // previous background instance (may be NULL)
    cBackground* m_pTempBackground;                      // 
    coreInt32    m_iLastID;                              // 

    coreFrameBuffer m_FrameBuffer;                       // environment frame buffer used for transition mixing
    coreFullscreen  m_MixObject;                         // fullscreen object for transition mixing
    coreProgramPtr  m_apMixProgram[ENVIRONMENT_MIXES];   // 

    coreTimer   m_TransitionTime;                        // background-transition timer
    coreVector2 m_vTransitionDir;                        // 

    coreVector2 m_avDirection[3];                        // background direction (0 = current value, 1 = target value, 2 = old value)
    coreVector2 m_avSide     [3];                        // background position offset
    coreFloat   m_afSpeed    [3];                        // movement speed
    coreFloat   m_afHeight   [3];                        // 
    coreFloat   m_afStrength [4];                        // 
    coreFlow    m_afLerp     [4];                        // 
    coreFloat   m_afFactor   [4];                        // 

    coreFloat   m_fFlyOffset;                            // global fly offset (directly accessed by background objects)
    coreFloat   m_fFlyShove;                             // 
    coreFloat   m_fSideOffset;                           // global side offset
    coreVector3 m_vCameraPos;                            // moved camera position
    coreVector3 m_vLightDir;                             // rotated light direction

    coreBool m_bActive;                                  // enables the environment (only for first background-transition on intro)


public:
    cEnvironment()noexcept;
    ~cEnvironment()final;

    DISABLE_COPY(cEnvironment)

    // render and move the environment
    void Render();
    void Move();

    // control active background
    void ChangeBackground(const coreInt32 iID, const coreUintW iTransitionType, const coreFloat fTransitionSpeed, const coreVector2 vTransitionDir = coreVector2(0.0f,-1.0f));
    inline cBackground*     GetBackground   ()const {ASSERT(m_pBackground) return m_pBackground;}
    inline cBackground*     GetOldBackground()const {return m_pOldBackground;}
    inline const coreInt32& GetLastID       ()const {return m_iLastID;}
    
    
    inline void UpdateTransitionSpeed(const coreFloat fTransitionSpeed) {m_TransitionTime.SetSpeed(ENVIRONMENT_TRANSITION_FACTOR * fTransitionSpeed);}

    // 
    FUNC_LOCAL coreFloat RetrieveTransitionBlend(const cBackground* pBackground)const;

    // retrieve safe geometric value
    FUNC_PURE coreFloat   RetrieveSafeHeight   (const coreVector2 vPosition,                                     const coreFloat fFallbackHeight = WATER_HEIGHT)const;
    FUNC_PURE coreVector3 RetrieveSafeIntersect(const coreVector3 vRayPosition, const coreVector3 vRayDirection, const coreFloat fFallbackHeight = WATER_HEIGHT)const;

    // 
    inline void OverrideFlyOffset(const coreFloat fFlyOffset) {m_fFlyOffset = fFlyOffset;}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return m_TransitionTime.GetStatus() ? &m_FrameBuffer : m_pBackground->GetResolvedTexture();}

    // set target transformation properties
    inline void SetTargetDirection    (const coreVector2 vDirection, const coreFloat fStrength) {m_avDirection[1] = vDirection; m_afStrength[0] = fStrength; m_afLerp[0] = 0.0f; ASSERT(vDirection.IsNormalized()) if(!GetCurBackRotation()) m_avDirection[1] = ENVIRONMENT_DEFAULT_DIRECTION;}
    inline void SetTargetSide         (const coreVector2 vSide,      const coreFloat fStrength) {m_avSide     [1] = vSide;      m_afStrength[1] = fStrength; m_afLerp[1] = 0.0f;}
    inline void SetTargetSpeed        (const coreFloat   fSpeed,     const coreFloat fStrength) {m_afSpeed    [1] = fSpeed;     m_afStrength[2] = fStrength; m_afLerp[2] = 0.0f; m_afSpeed[1] *= I_TO_F(GetCurBackSpeed()) / 100.0f;}
    inline void SetTargetHeight       (const coreFloat   fHeight,    const coreFloat fStrength) {m_afHeight   [1] = fHeight;    m_afStrength[3] = fStrength; m_afLerp[3] = 0.0f;}
    inline void SetTargetDirectionNow (const coreVector2 vDirection)                            {this->SetTargetDirection(vDirection, 0.0f); m_avDirection[0] = m_avDirection[1];}
    inline void SetTargetSideNow      (const coreVector2 vSide)                                 {this->SetTargetSide     (vSide,      0.0f); m_avSide     [0] = m_avSide     [1];}
    inline void SetTargetSpeedNow     (const coreFloat   fSpeed)                                {this->SetTargetSpeed    (fSpeed,     0.0f); m_afSpeed    [0] = m_afSpeed    [1];}
    inline void SetTargetHeightNow    (const coreFloat   fHeight)                               {this->SetTargetHeight   (fHeight,    0.0f); m_afHeight   [0] = m_afHeight   [1];}
    inline void SetTargetDirectionLerp(const coreVector2 vDirection, const coreFloat fTime)     {this->SetTargetDirection(vDirection, 0.0f); m_avDirection[2] = m_avDirection[0]; m_afLerp[0] = 1.0f; m_afFactor[0] = -RCP(fTime); m_avDirection[1].x = m_avDirection[1].Angle(); m_avDirection[2].x = m_avDirection[2].Angle();}
    inline void SetTargetSideLerp     (const coreVector2 vSide,      const coreFloat fTime)     {this->SetTargetSide     (vSide,      0.0f); m_avSide     [2] = m_avSide     [0]; m_afLerp[1] = 1.0f; m_afFactor[1] = -RCP(fTime);}
    inline void SetTargetSpeedLerp    (const coreFloat   fSpeed,     const coreFloat fTime)     {this->SetTargetSpeed    (fSpeed,     0.0f); m_afSpeed    [2] = m_afSpeed    [0]; m_afLerp[2] = 1.0f; m_afFactor[2] = -RCP(fTime);}
    inline void SetTargetHeightLerp   (const coreFloat   fHeight,    const coreFloat fTime)     {this->SetTargetHeight   (fHeight,    0.0f); m_afHeight   [2] = m_afHeight   [0]; m_afLerp[3] = 1.0f; m_afFactor[3] = -RCP(fTime);}

    // get current transformation properties
    inline const coreVector2& GetDirection()const {return m_avDirection[0];}
    inline const coreVector2& GetSide     ()const {return m_avSide     [0];}
    inline const coreFloat&   GetSpeed    ()const {return m_afSpeed    [0];}
    inline const coreFloat&   GetHeight   ()const {return m_afHeight   [0];}

    // get offset values
    inline const coreFloat&   GetFlyOffset ()const {return m_fFlyOffset;}
    inline const coreFloat&   GetFlyShove  ()const {return m_fFlyShove;}
    inline const coreFloat&   GetSideOffset()const {return m_fSideOffset;}
    inline const coreVector3& GetCameraPos ()const {return m_vCameraPos;}
    inline const coreVector3& GetLightDir  ()const {return m_vLightDir;}

    // enable the environment
    inline void Activate() {m_bActive = true;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __ClearTransition();
};


#endif // _P1_GUARD_ENVIRONMENT_H_