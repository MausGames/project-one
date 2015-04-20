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


// ****************************************************************
// 
#define SPECIAL_WAVES        (4u)              // 
#define SPECIAL_SOUNDS       (8u)              // 

#define SPECIAL_SPLASH_SMALL (50.0f),  (25u)   // 
#define SPECIAL_SPLASH_BIG   (100.0f), (50u)   // 

enum eSoundEffect : coreUint16   // 0xAABBu -> AA sub-index, BB file-index
{
    SOUND_EXPLOSION_ENERGY_SMALL = 0x0100u,
    SOUND_EXPLOSION_ENERGY_BIG   = 0x0200u
};


// ****************************************************************
// special-effects class
class cSpecialEffects final
{
private:
    coreParticleSystem m_ParticleColor;       // 
    coreParticleSystem m_ParticleDark;        // 
    coreParticleSystem m_ParticleSmoke;       // 
    coreParticleSystem m_ParticleFire;        // 

    coreObject3D m_aWave[SPECIAL_WAVES];      // 
    coreUintW    m_iCurWave;                  // 

    coreSoundPtr m_apSound[SPECIAL_SOUNDS];   // 

    coreTimer m_ShakeTimer;                   // 
    coreFloat m_fShakeStrength;               // 


public:
    cSpecialEffects()noexcept;

    DISABLE_COPY(cSpecialEffects)

    // render and move special-effects
    void Render();
    void Move();

    // 
    void CreateSplashColor(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor);
    void CreateSplashDark (const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum);
    void CreateSplashSmoke(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum);
    void CreateSplashFire (const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum);

    void CreateDirectionColor(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor);

    // 
    void CreateWave(const coreVector3& vPosition, const coreFloat& fScale, const coreFloat& fSpeed, const coreVector3& vColor);

    // 
    void PlaySound(const coreVector3& vPosition, const coreFloat& fVolume, const eSoundEffect& iSoundIndex);

    // 
    inline void ShakeScreen(const coreFloat& fStrength) {m_fShakeStrength = fStrength;}

    // 
    inline coreParticleSystem* GetParticleColor()      {return &m_ParticleColor;}
    inline coreParticleSystem* GetParticleDark ()      {return &m_ParticleDark;}
    inline coreParticleSystem* GetParticleSmoke()      {return &m_ParticleSmoke;}
    inline coreParticleSystem* GetParticleFire ()      {return &m_ParticleFire;}
    inline const coreFloat&    GetShakeStrength()const {return m_fShakeStrength;}
};


#endif // _P1_GUARD_SPECIALEFFECTS_H_