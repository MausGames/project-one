//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cSpecialEffects::cSpecialEffects()noexcept
: m_ParticleDark   (128u)
, m_ParticleSmoke  (256u)
, m_ParticleFire   (256u)
, m_ShakeTimer     (coreTimer(1.0f, 30.0f, 0u))
, m_fShakeStrength (0.0f)
{
    // 
    m_ParticleDark.DefineProgram("effect_particle_dark_program");
    m_ParticleDark.DefineTexture(0u, "effect_particle_32.png");

    m_ParticleSmoke.DefineProgram("effect_particle_smoke_program");
    m_ParticleSmoke.DefineTexture(0u, "effect_particle_128.png");

    m_ParticleFire.DefineProgram("effect_particle_fire_program");
    m_ParticleFire.DefineTexture(0u, "effect_particle_128.png");

    // 
    m_ShakeTimer.Play(CORE_TIMER_PLAY_RESET);
}


// ****************************************************************
// render special-effects
void cSpecialEffects::Render()
{
    glDepthMask(false);
    {
        // 
        m_ParticleDark .Render();
        m_ParticleSmoke.Render();

        // 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        m_ParticleFire.Render();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glDepthMask(true);
}


// ****************************************************************
// move special-effects
void cSpecialEffects::Move()
{
    // 
    m_ParticleDark .Move();
    m_ParticleSmoke.Move();
    m_ParticleFire .Move();

    // 
    if(m_fShakeStrength && m_ShakeTimer.Update(1.0f))
    {
        // 
        m_fShakeStrength = MAX(m_fShakeStrength - 0.07f, 0.0f);

        // 
        g_pPostProcessing->SetPosition(coreVector2::Rand() * m_fShakeStrength * 0.01f);
        g_pPostProcessing->Move();
    }
}


// ****************************************************************
// 
void cSpecialEffects::CreateParticleDarkSplash(const coreUintW& iNum, const coreVector3& vPosition, const coreFloat& fSize)
{
    // 
    m_ParticleDark.GetEmptyEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(1.0f), coreVector3::Rand(-fSize, fSize));
        pParticle->SetScaleAbs   (3.5f,                              1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f);
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateParticleSmokeSplash(const coreUintW& iNum, const coreVector3& vPosition, const coreFloat& fSize)
{
    // 
    m_ParticleSmoke.GetEmptyEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(1.0f), coreVector3::Rand(-fSize, fSize));
        pParticle->SetScaleAbs   (3.0f,                              12.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI*0.1f, PI*0.1f));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (0.7f);
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateParticleFireSplash(const coreUintW& iNum, const coreVector3& vPosition, const coreFloat& fSize)
{
    // 
    m_ParticleFire.GetEmptyEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(3.0f), coreVector3::Rand(-fSize, fSize));
        pParticle->SetScaleAbs   (4.0f,                              10.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI*0.5f, PI*0.5f));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.4f);
    });
}