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

// TODO: render depth-quads or use scissor test on unused background areas (for transition!)
// TODO: make "side" parameterized


// ****************************************************************
// environment class
class cEnvironment final : public coreResourceRelation
{
private:
    cBackground* m_pBackground;       // current background instance (should never be NULL)
    cBackground* m_pOldBackground;    // previous background instance (may be NULL)

    coreFrameBuffer m_iFrameBuffer;   // environment frame buffer used for transition mixing (only texture)
    coreObject2D    m_MixObject;      // fullscreen object for transition mixing
    coreTimer       m_Transition;     // background-transition timer

    coreVector2 m_avDirection[2];     // background direction (0 = current value, 1 = target value)
    coreVector2 m_avSide     [2];     // background position offset
    coreFloat   m_afSpeed    [2];     // movement speed

    coreFloat   m_fFlyOffset;         // global fly offset (directly accessed by background objects)
    coreFloat   m_fSideOffset;        // global side offset
    coreVector3 m_vCameraPos;         // moved camera position
    coreVector3 m_vLightDir;          // rotated light direction

    coreBool m_bActive;               // enables the environment (only for first background-transition on intro)


public:
    cEnvironment()noexcept;
    ~cEnvironment();

    DISABLE_COPY(cEnvironment)

    // render and move the environment
    void Render();
    void Move();

    // control active background
    void ChangeBackground(const coreInt32& iID);
    inline cBackground*     GetBackground   ()const {ASSERT(m_pBackground) return m_pBackground;}
    inline cBackground*     GetOldBackground()const {return m_pOldBackground;}
    inline const coreTimer& GetTransition   ()const {return m_Transition;}

    // retrieve safe height value
    coreFloat RetrieveSafeHeight(const coreVector2& vPosition);

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return m_Transition.GetStatus() ? &m_iFrameBuffer : m_pBackground->GetResolvedTexture();}

    // set target transformation properties
    inline void SetTargetDirection(const coreVector2& vDirection) {m_avDirection[1] = vDirection; ASSERT(vDirection.IsNormalized())}
    inline void SetTargetSide     (const coreVector2& vSide)      {m_avSide     [1] = vSide;}
    inline void SetTargetSpeed    (const coreFloat&   fSpeed)     {m_afSpeed    [1] = fSpeed;}

    // get current transformation properties
    inline const coreVector2& GetDirection()const {return m_avDirection[0];}
    inline const coreVector2& GetSide     ()const {return m_avSide     [0];}
    inline const coreFloat&   GetSpeed    ()const {return m_afSpeed    [0];}

    // get offset values
    inline const coreFloat&   GetFlyOffset ()const {return m_fFlyOffset;}
    inline const coreFloat&   GetSideOffset()const {return m_fSideOffset;}
    inline const coreVector3& GetCameraPos ()const {return m_vCameraPos;}
    inline const coreVector3& GetLightDir  ()const {return m_vLightDir;}

    // enable the environment
    inline void Activate() {m_bActive = true;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_ENVIRONMENT_H_