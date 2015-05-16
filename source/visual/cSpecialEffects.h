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


// ****************************************************************
// special-effects definitions
#define SPECIAL_BLASTS          (4u)              // number of energy-blasts
#define SPECIAL_RINGS           (4u)              // number of energy-rings
#define SPECIAL_SOUNDS          (8u)              // number of sound-effect files

#define SPECIAL_SPLASH_TINY     (25.0f),  (13u)   // 
#define SPECIAL_SPLASH_SMALL    (50.0f),  (25u)   // 
#define SPECIAL_SPLASH_BIG      (100.0f), (50u)   // 
#define SPECIAL_BLOW_SMALL      (50.0f),  (25u)   // 
#define SPECIAL_BLOW_BIG        (100.0f), (50u)   // 
#define SPECIAL_CHARGE_BIG      (35.0f),  (40u)   // 

#define SPECIAL_EXPLOSION_SMALL (10.0f), (40u)    // 
#define SPECIAL_EXPLOSION_BIG   (20.0f), (80u)    // 

#define SPECIAL_BLAST_SMALL     (2.0f), (3.5f)    // 
#define SPECIAL_BLAST_BIG       (4.0f), (3.5f)    // 

#define SPECIAL_SHAKE_SMALL     (0.6f)            // 
#define SPECIAL_SHAKE_BIG       (1.2f)            // 

enum eSoundEffect : coreUint16   // 0xAABBu -> AA sub-index, BB file-index
{
    SOUND_EXPLOSION_ENERGY_SMALL   = 0x0000u,
    SOUND_EXPLOSION_ENERGY_BIG     = 0x0100u,
    SOUND_EXPLOSION_PHYSICAL_SMALL = 0x0001u,
    SOUND_EXPLOSION_PHYSICAL_BIG   = 0x0101u
};


// ****************************************************************
// special-effects class
class cSpecialEffects final
{
private:
    coreParticleSystem m_ParticleColor;       // color particle system
    coreParticleSystem m_ParticleDark;        // dark particle system
    coreParticleSystem m_ParticleSmoke;       // smoke particle system
    coreParticleSystem m_ParticleFire;        // fire particle system

    coreObject3D m_aBlast[SPECIAL_BLASTS];    // 
    coreUintW    m_iCurBlast;                 // 

    coreObject3D m_aRing[SPECIAL_RINGS];      // 
    coreUintW    m_iCurRing;                  // 

    coreSoundPtr m_apSound[SPECIAL_SOUNDS];   // 

    coreTimer m_ShakeTimer;                   // 
    coreFloat m_fShakeStrength;               // 


public:
    cSpecialEffects()noexcept;

    DISABLE_COPY(cSpecialEffects)

    // render and move special-effects
    void Render();
    void Move();

    // create centered particle splash
    void CreateSplashColor(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor);
    void CreateSplashDark (const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum);
    void CreateSplashSmoke(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum);
    void CreateSplashFire (const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum);

    // create directional particle blow
    void CreateBlowColor(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor);
    void CreateBlowDark (const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat& fScale, const coreUintW& iNum);

    // create whirling particle charge
    void CreateChargeColor(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor);
    void CreateChargeDark (const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum);

    // 
    void CreateBlast(const coreVector3& vPosition,                                                                 const coreFloat& fScale, const coreFloat& fSpeed, const coreVector3& vColor);
    void CreateRing (const coreVector3& vPosition, const coreVector3& vDirection, const coreVector3& vOrientation, const coreFloat& fScale, const coreFloat& fSpeed, const coreVector3& vColor);

    // 
    void PlaySound(const coreVector3& vPosition, const coreFloat& fVolume, const eSoundEffect& iSoundIndex);

    // 
    void ShakeScreen(const coreFloat& fStrength);
    inline const coreFloat& GetShakeStrength()const {return m_fShakeStrength;}

    // 
    void MacroExplosionColorSmall   (const coreVector3& vPosition, const coreVector3& vColor);
    void MacroExplosionColorBig     (const coreVector3& vPosition, const coreVector3& vColor);
    void MacroExplosionDarkSmall    (const coreVector3& vPosition);
    void MacroExplosionDarkBig      (const coreVector3& vPosition);
    void MacroExplosionPhysicalSmall(const coreVector3& vPosition);
    void MacroExplosionPhysicalBig  (const coreVector3& vPosition);
};


#endif // _P1_GUARD_SPECIALEFFECTS_H_