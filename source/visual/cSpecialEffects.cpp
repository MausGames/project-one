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
: m_ParticleColor  (512u)
, m_ParticleDark   (128u)
, m_ParticleSmoke  (256u)
, m_ParticleFire   (256u)
, m_iCurWave       (0u)
, m_ShakeTimer     (coreTimer(1.0f, 30.0f, 0u))
, m_fShakeStrength (0.0f)
{
    // 
    m_ParticleColor.DefineProgram("effect_particle_color_program");
    m_ParticleColor.DefineTexture(0u, "effect_particle_32.png");

    m_ParticleDark.DefineProgram("effect_particle_dark_program");
    m_ParticleDark.DefineTexture(0u, "effect_particle_32.png");

    m_ParticleSmoke.DefineProgram("effect_particle_smoke_program");
    m_ParticleSmoke.DefineTexture(0u, "effect_particle_128.png");

    m_ParticleFire.DefineProgram("effect_particle_fire_program");
    m_ParticleFire.DefineTexture(0u, "effect_particle_128.png");

    // 
    for(coreUintW i = 0u; i < SPECIAL_WAVES; ++i)
    {
        m_aWave[i].DefineModel  ("object_sphere.md3");
        m_aWave[i].DefineTexture(0u, "effect_energy.png");
        m_aWave[i].DefineProgram("effect_energy_spheric_program");
        m_aWave[i].SetAlpha     (0.0f);
        m_aWave[i].SetTexSize   (coreVector2(15.0f,15.0f));
    }

    // 
    auto nLoadSoundFunc = [](coreSoundPtr* ppSoundArray, const eSoundEffect& iSoundIndex, const coreChar* pcName)
    {
        coreSoundPtr& pSoundPtr = ppSoundArray[iSoundIndex & 0xFFu];

        if(!pSoundPtr) pSoundPtr = Core::Manager::Resource->Get<coreSound>(pcName);
        ASSERT(!std::strcmp(pSoundPtr.GetHandle()->GetName(), pcName))
    };
    nLoadSoundFunc(m_apSound, SOUND_EXPLOSION_ENERGY_SMALL, "explosion_energy.wav");
    nLoadSoundFunc(m_apSound, SOUND_EXPLOSION_ENERGY_BIG,   "explosion_energy.wav");

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
        m_ParticleColor.Render();
        m_ParticleDark .Render();
        m_ParticleSmoke.Render();

        // 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        m_ParticleFire.Render();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 
        for(coreUintW i = 0u; i < SPECIAL_WAVES; ++i)
        {
            coreObject3D& oWave = m_aWave[i];
            if(!oWave.GetAlpha()) continue;

            // 
            oWave.Render();
        }
    }
    glDepthMask(true);
}


// ****************************************************************
// move special-effects
void cSpecialEffects::Move()
{
    // 
    m_ParticleColor.Move();
    m_ParticleDark .Move();
    m_ParticleSmoke.Move();
    m_ParticleFire .Move();

    // 
    for(coreUintW i = 0u; i < SPECIAL_WAVES; ++i)
    {
        coreObject3D& oWave = m_aWave[i];
        if(!oWave.GetAlpha()) continue;

        // 
        const coreFloat& fScale = oWave.GetCollisionModifier().x;
        const coreFloat& fSpeed = oWave.GetCollisionModifier().y;

        // 
        oWave.SetAlpha(MAX(oWave.GetAlpha() - fSpeed * Core::System->GetTime(), 0.0f));
        oWave.SetSize (coreVector3(8.0f,8.0f,8.0f) * (fScale * (1.0f - oWave.GetAlpha())));
        oWave.Move();
    }

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
void cSpecialEffects::CreateSplashColor(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor)
{
    // 
    m_ParticleColor.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(1.0f), coreVector3::Rand(-fScale, fScale));
        pParticle->SetScaleAbs   (3.5f,                              1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),         coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateSplashDark(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum)
{
    // 
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(1.0f), coreVector3::Rand(-fScale, fScale));
        pParticle->SetScaleAbs   (3.5f,                              1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateSplashSmoke(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum)
{
    // 
    m_ParticleSmoke.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(1.0f), coreVector3::Rand(-fScale, fScale));
        pParticle->SetScaleAbs   (3.0f,                              12.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI*0.1f, PI*0.1f));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (0.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateSplashFire(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum)
{
    // 
    m_ParticleFire.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(3.0f), coreVector3::Rand(-fScale, fScale));
        pParticle->SetScaleAbs   (4.0f,                              10.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI*0.5f, PI*0.5f));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.4f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateDirectionColor(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor)
{
    // 
    m_ParticleColor.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(1.0f), coreVector3::Rand(-fScale, fScale) * 0.25f + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (3.5f,                              1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),         coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateWave(const coreVector3& vPosition, const coreFloat& fScale, const coreFloat& fSpeed, const coreVector3& vColor)
{
    // 
    if(++m_iCurWave >= SPECIAL_WAVES) m_iCurWave = 0u;
    coreObject3D& oWave = m_aWave[m_iCurWave];

    // 
    oWave.SetPosition         (vPosition);
    oWave.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oWave.SetDirection        (coreVector3(coreVector2::Rand(), 0.0f));
    oWave.SetColor4           (coreVector4(vColor, 1.0f));
    oWave.SetCollisionModifier(coreVector3(fScale, fSpeed, 0.0f));
}


// ****************************************************************
// 
void cSpecialEffects::PlaySound(const coreVector3& vPosition, const coreFloat& fVolume, const eSoundEffect& iSoundIndex)
{
    ASSERT(fVolume)

    coreFloat fBaseVolume = 0.0f;
    coreFloat fBasePitch  = 0.0f;

    // 
    switch(iSoundIndex)
    {
    default: ASSERT(false)
    case SOUND_EXPLOSION_ENERGY_SMALL: fBaseVolume = 1.0f; fBasePitch = 1.5f; break;
    case SOUND_EXPLOSION_ENERGY_BIG:   fBaseVolume = 3.0f; fBasePitch = 0.8f; break;
    }

    // 
    m_apSound[iSoundIndex & 0xFFu]->PlayPosition(NULL, fVolume * fBaseVolume, fBasePitch, 0.0f, false, vPosition);
}