//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SPECIALEFFECTS_H_
#define _P1_GUARD_SPECIALEFFECTS_H_

// TODO: glowing waves
// TODO: particles should not overdraw outlines (at least color-particles)
// TODO: make lightning owner-sticky with position-offset
// TODO: implement pixel-fitting screen-shake ? (currently shaking creates a fullscreen-blur)
// TODO: don't invoke special-effects out of view (though consider effect-radius)


// ****************************************************************
// special-effects definitions
#define SPECIAL_LIGHTNINGS       (32u)     // number of lightning sprites
#define SPECIAL_BLASTS           (4u)      // number of energy-blasts
#define SPECIAL_RINGS            (8u)      // number of energy-rings
#define SPECIAL_SOUNDS           (8u)      // number of sound-effect files

#define SPECIAL_LIGHTNING_RESIZE (0.66f)   // 
#define SPECIAL_LIGHTNING_CUTOUT (0.5f)    // 

#define SPECIAL_SPLASH_TINY      (25.0f), (13u)
#define SPECIAL_SPLASH_SMALL     (50.0f), (25u)
#define SPECIAL_SPLASH_BIG      (100.0f), (50u)
#define SPECIAL_BLOW_SMALL       (50.0f), (25u)
#define SPECIAL_BLOW_BIG        (100.0f), (50u)
#define SPECIAL_CHARGE_BIG       (35.0f), (40u)
#define SPECIAL_EXPLOSION_SMALL  (10.0f), (40u)
#define SPECIAL_EXPLOSION_BIG    (20.0f), (80u)
#define SPECIAL_LIGHTNING_SMALL  (11.0f)
#define SPECIAL_LIGHTNING_BIG    (25.0f)
#define SPECIAL_BLAST_SMALL       (2.0f), (3.5f)
#define SPECIAL_BLAST_BIG         (4.0f), (3.5f)
#define SPECIAL_RING_SMALL        (1.5f), (1.8f)
#define SPECIAL_RING_BIG          (3.0f), (1.8f)
#define SPECIAL_SHAKE_SMALL       (0.6f)
#define SPECIAL_SHAKE_BIG         (1.2f)

enum eSoundEffect : coreUint16   // 0xAABBu -> AA sub-index, BB file-index
{
    SOUND_EXPLOSION_ENERGY_SMALL   = 0x0000u,
    SOUND_EXPLOSION_ENERGY_BIG     = 0x0100u,
    SOUND_EXPLOSION_PHYSICAL_SMALL = 0x0001u,
    SOUND_EXPLOSION_PHYSICAL_BIG   = 0x0101u,
    SOUND_RUSH_SHORT               = 0x0002u,
    SOUND_RUSH_LONG                = 0x0102u,
    SOUND_FFFF                     = 0xFFFFu
};


// ****************************************************************
// special-effects class
class cSpecialEffects final
{
private:
    coreParticleSystem m_ParticleColor;                     // color particle system
    coreParticleSystem m_ParticleDark;                      // dark particle system
    coreParticleSystem m_ParticleSmoke;                     // smoke particle system
    coreParticleSystem m_ParticleFire;                      // fire particle system

    coreObject3D  m_aLightning      [SPECIAL_LIGHTNINGS];   // 
    coreObject3D* m_apLightningOwner[SPECIAL_LIGHTNINGS];   // owner of a lightning sprite (to move together)
    coreBatchList m_LightningList;                          // 
    coreUintW     m_iCurLightning;                          // 

    coreObject3D m_aBlast[SPECIAL_BLASTS];                  // 
    coreUintW    m_iCurBlast;                               // 

    coreObject3D m_aRing[SPECIAL_RINGS];                    // 
    coreUintW    m_iCurRing;                                // 

    coreSoundPtr m_apSound[SPECIAL_SOUNDS];                 // 
    eSoundEffect m_iSoundGuard;                             // last played sound-effect (to reduce multiple same sound-effects within one frame)

    coreTimer m_ShakeTimer;                                 // 
    coreFloat m_fShakeStrength;                             // current shake strength (decreasing)

    coreBool m_bActive;                                     // 


public:
    cSpecialEffects()noexcept;

    DISABLE_COPY(cSpecialEffects)

    // render and move special-effects
    void Render(const coreBool bForeground);
    void Move();

    // 
    inline const coreBool& IsActive()const {return m_bActive;}

    // create centered particle splash
    void CreateSplashColor(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor);
    void CreateSplashDark (const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum);
    void CreateSplashSmoke(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum);
    void CreateSplashFire (const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum);

    // create directional particle blow
    void CreateBlowColor(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor);
    void CreateBlowDark (const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat fScale, const coreUintW iNum);

    // create whirling particle charge
    void CreateChargeColor(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor);
    void CreateChargeDark (const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum);

    // 
    coreFloat CreateLightning(const coreVector2& vPosFrom, const coreVector2& vPosTo,                              const coreFloat fWidth, const coreVector3& vColor, const coreVector2& vTexSizeFactor, const coreFloat fTexOffset);
    void      CreateLightning(coreObject3D*      pOwner,   const coreVector2& vDirection, const coreFloat fLength, const coreFloat fWidth, const coreVector3& vColor, const coreVector2& vTexSizeFactor, const coreFloat fTexOffset);

    // 
    void CreateBlast(const coreVector3& vPosition,                                                                 const coreFloat fScale, const coreFloat fSpeed, const coreVector3& vColor);
    void CreateRing (const coreVector3& vPosition, const coreVector3& vDirection, const coreVector3& vOrientation, const coreFloat fScale, const coreFloat fSpeed, const coreVector3& vColor);

    // 
    void PlaySound(const coreVector3& vPosition, const coreFloat fVolume, const eSoundEffect iSoundIndex);

    // 
    void ShakeScreen(const coreFloat fStrength);
    inline const coreFloat& GetShakeStrength()const {return m_fShakeStrength;}

    // 
    void MacroExplosionColorSmall   (const coreVector3& vPosition, const coreVector3& vColor);
    void MacroExplosionColorBig     (const coreVector3& vPosition, const coreVector3& vColor);
    void MacroExplosionDarkSmall    (const coreVector3& vPosition);
    void MacroExplosionDarkBig      (const coreVector3& vPosition);
    void MacroExplosionPhysicalSmall(const coreVector3& vPosition);
    void MacroExplosionPhysicalBig  (const coreVector3& vPosition);
    void MacroEruptionColorSmall    (const coreVector3& vPosition, const coreVector2& vDirection, const coreVector3& vColor);
    void MacroEruptionColorBig      (const coreVector3& vPosition, const coreVector2& vDirection, const coreVector3& vColor);
    void MacroEruptionDarkSmall     (const coreVector3& vPosition, const coreVector2& vDirection);
    void MacroEruptionDarkBig       (const coreVector3& vPosition, const coreVector2& vDirection);
    void MacroEruptionPhysicalSmall (const coreVector3& vPosition, const coreVector2& vDirection) {ASSERT(false)}
    void MacroEruptionPhysicalBig   (const coreVector3& vPosition, const coreVector2& vDirection) {ASSERT(false)}
};


#endif // _P1_GUARD_SPECIALEFFECTS_H_