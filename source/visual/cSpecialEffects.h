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
// special-effects class
class cSpecialEffects final
{
private:
    coreParticleSystem m_ParticleDark;    // 
    coreParticleSystem m_ParticleSmoke;   // 
    coreParticleSystem m_ParticleFire;    // 

    coreTimer m_ShakeTimer;               // 
    coreFloat m_fShakeStrength;           // 


public:
    cSpecialEffects()noexcept;

    DISABLE_COPY(cSpecialEffects)

    // render and move special-effects
    void Render();
    void Move();

    // 
    void CreateParticleDarkSplash (const coreUintW& iNum, const coreVector3& vPosition, const coreFloat& fSize);
    void CreateParticleSmokeSplash(const coreUintW& iNum, const coreVector3& vPosition, const coreFloat& fSize);
    void CreateParticleFireSplash (const coreUintW& iNum, const coreVector3& vPosition, const coreFloat& fSize);

    // 
    inline void ShakeScreen(const coreFloat& fStrength) {m_fShakeStrength = fStrength;}

    // 
    inline coreParticleSystem* GetParticleDark ()      {return &m_ParticleDark;}
    inline coreParticleSystem* GetParticleSmoke()      {return &m_ParticleSmoke;}
    inline coreParticleSystem* GetParticleFire ()      {return &m_ParticleFire;}
    inline const coreFloat&    GetShakeStrength()const {return m_fShakeStrength;}
};


#endif // _P1_GUARD_SPECIALEFFECTS_H_