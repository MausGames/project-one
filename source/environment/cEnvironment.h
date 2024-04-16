//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENVIRONMENT_H_
#define _P1_GUARD_ENVIRONMENT_H_

// TODO: render depth-quads or use scissor test (tested: works bad, no gain) on unused background areas (for transition!)
// TODO: make "side" parameterized
// TODO: use depth from water as own map for decals to reconstruct position for shadow-mapping, maybe add normal for lighting
// TODO: currently after changing the background there is no Background::Move before the first Render
// TODO: separate culling between normal rendering an shadow, to improve efficiency (though this would update instancing buffer again ?)
// TODO: transition is reset when resizing window


// ****************************************************************
// environment definitions
#define ENVIRONMENT_MIX_FADE    (0u)   // 
#define ENVIRONMENT_MIX_WIPE    (1u)   // 
#define ENVIRONMENT_MIX_CURTAIN (2u)   // 
#define ENVIRONMENT_MIX_CIRCLE  (3u)   // 
#define ENVIRONMENT_MIXES       (4u)   // 

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

    coreFrameBuffer m_FrameBuffer;                       // environment frame buffer used for transition mixing
    coreObject2D    m_MixObject;                         // fullscreen object for transition mixing
    coreProgramPtr  m_apMixProgram[ENVIRONMENT_MIXES];   // 

    coreTimer   m_TransitionTime;                        // background-transition timer
    coreVector2 m_vTransitionDir;                        // 

    coreVector2 m_avDirection[2];                        // background direction (0 = current value, 1 = target value)
    coreVector2 m_avSide     [2];                        // background position offset
    coreFloat   m_afSpeed    [2];                        // movement speed
    coreFloat   m_afHeight   [2];                        // 

    coreFloat   m_fFlyOffset;                            // global fly offset (directly accessed by background objects)
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
    void ChangeBackground(const coreInt32 iID, const coreUintW iTransitionType, const coreFloat fTransitionSpeed, const coreVector2& vTransitionDir = coreVector2(0.0f,-1.0f));
    inline cBackground* GetBackground   ()const {ASSERT(m_pBackground) return m_pBackground;}
    inline cBackground* GetOldBackground()const {return m_pOldBackground;}

    // 
    FUNC_LOCAL coreFloat RetrieveTransitionBlend(const cBackground* pBackground)const;

    // retrieve safe height value
    FUNC_PURE coreFloat RetrieveSafeHeight(const coreVector2& vPosition, const coreFloat fFallback = WATER_HEIGHT)const;

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return m_TransitionTime.GetStatus() ? &m_FrameBuffer : m_pBackground->GetResolvedTexture();}

    // set target transformation properties
    inline void SetTargetDirection(const coreVector2& vDirection) {m_avDirection[1] = vDirection; ASSERT(vDirection.IsNormalized())}
    inline void SetTargetSide     (const coreVector2& vSide)      {m_avSide     [1] = vSide;}
    inline void SetTargetSpeed    (const coreFloat    fSpeed)     {m_afSpeed    [1] = fSpeed;}
    inline void SetTargetHeight   (const coreFloat    fHeight)    {m_afHeight   [1] = fHeight;}

    // get current transformation properties
    inline const coreVector2& GetDirection()const {return m_avDirection[0];}
    inline const coreVector2& GetSide     ()const {return m_avSide     [0];}
    inline const coreFloat&   GetSpeed    ()const {return m_afSpeed    [0];}
    inline const coreFloat&   GetHeight   ()const {return m_afHeight   [0];}

    // get offset values
    inline const coreFloat&   GetFlyOffset ()const {return m_fFlyOffset;}
    inline const coreFloat&   GetSideOffset()const {return m_fSideOffset;}
    inline const coreVector3& GetCameraPos ()const {return m_vCameraPos;}
    inline const coreVector3& GetLightDir  ()const {return m_vLightDir;}

    // enable the environment
    inline void Activate() {m_bActive = true;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset bInit)final;
};


#endif // _P1_GUARD_ENVIRONMENT_H_