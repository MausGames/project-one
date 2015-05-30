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
, m_ParticleDark   (256u)
, m_ParticleSmoke  (512u)
, m_ParticleFire   (256u)
, m_iCurBlast      (0u)
, m_iCurRing       (0u)
, m_iSoundGuard    (0u)
, m_ShakeTimer     (coreTimer(1.0f, 30.0f, 0u))
, m_fShakeStrength (0.0f)
{
    // 
    m_ParticleColor.DefineProgram("effect_particle_color_program");
    m_ParticleColor.DefineTexture(0u, "effect_particle_32.png");
    m_ParticleDark .DefineProgram("effect_particle_dark_program");
    m_ParticleDark .DefineTexture(0u, "effect_particle_32.png");
    m_ParticleSmoke.DefineProgram("effect_particle_smoke_program");
    m_ParticleSmoke.DefineTexture(0u, "effect_particle_128.png");
    m_ParticleFire .DefineProgram("effect_particle_fire_program");
    m_ParticleFire .DefineTexture(0u, "effect_particle_128.png");

    // 
    for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
    {
        m_aBlast[i].DefineModel  ("object_sphere.md3");
        m_aBlast[i].DefineTexture(0u, "effect_energy.png");
        m_aBlast[i].DefineProgram("effect_energy_spheric_program");
        m_aBlast[i].SetAlpha     (0.0f);
        m_aBlast[i].SetTexSize   (coreVector2(12.0f,12.0f));
    }

    // 
    for(coreUintW i = 0u; i < SPECIAL_RINGS; ++i)
    {
        m_aRing[i].DefineModel  ("object_ring.md3");
        m_aRing[i].DefineTexture(0u, "effect_energy.png");
        m_aRing[i].DefineProgram("effect_energy_invert_program");
        m_aRing[i].SetAlpha     (0.0f);
        m_aRing[i].SetTexSize   (coreVector2(12.0f,12.0f));
    }

    // 
    auto nLoadSoundFunc = [](coreSoundPtr* ppSoundArray, const eSoundEffect& iSoundIndex, const coreChar* pcName)
    {
        coreSoundPtr& pSoundPtr = ppSoundArray[iSoundIndex & 0xFFu];

        if(!pSoundPtr) pSoundPtr = Core::Manager::Resource->Get<coreSound>(pcName);
        ASSERT(!std::strcmp(pSoundPtr.GetHandle()->GetName(), pcName))
    };
    nLoadSoundFunc(m_apSound, SOUND_EXPLOSION_ENERGY_SMALL,   "effect_explosion_energy.wav");
    nLoadSoundFunc(m_apSound, SOUND_EXPLOSION_ENERGY_BIG,     "effect_explosion_energy.wav");
    nLoadSoundFunc(m_apSound, SOUND_EXPLOSION_PHYSICAL_SMALL, "effect_explosion_physical.wav");
    nLoadSoundFunc(m_apSound, SOUND_EXPLOSION_PHYSICAL_BIG,   "effect_explosion_physical.wav");

    // 
    m_ShakeTimer.Play(CORE_TIMER_PLAY_RESET);

    STATIC_ASSERT(SPECIAL_SOUNDS <= sizeof(m_iSoundGuard)*8)
}


// ****************************************************************
// render special-effects
void cSpecialEffects::Render()
{
    glDepthMask(false);
    {
        // render particle systems
        m_ParticleColor.Render();
        m_ParticleDark .Render();
        m_ParticleSmoke.Render();

        // render fire particle system (with additive blending)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        m_ParticleFire.Render();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 
        for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
        {
            coreObject3D& oBlast = m_aBlast[i];
            if(!oBlast.GetAlpha()) continue;

            // 
            oBlast.Render();
        }

        // 
        for(coreUintW i = 0u; i < SPECIAL_RINGS; ++i)
        {
            coreObject3D& oRing = m_aRing[i];
            if(!oRing.GetAlpha()) continue;

            // 
            oRing.Render();
        }
    }
    glDepthMask(true);
}


// ****************************************************************
// move special-effects
void cSpecialEffects::Move()
{
    // move particle systems
    m_ParticleColor.Move();
    m_ParticleDark .Move();
    m_ParticleSmoke.Move();
    m_ParticleFire .Move();

    // 
    for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
    {
        coreObject3D& oBlast = m_aBlast[i];
        if(!oBlast.GetAlpha()) continue;

        // 
        const coreFloat& fScale = oBlast.GetCollisionModifier().x;
        const coreFloat& fSpeed = oBlast.GetCollisionModifier().y;

        // 
        oBlast.SetAlpha(MAX(oBlast.GetAlpha() - fSpeed * Core::System->GetTime(), 0.0f));
        oBlast.SetSize (coreVector3(8.0f,8.0f,8.0f) * (fScale * (1.0f - oBlast.GetAlpha())));
        oBlast.Move();
    }

    // 
    for(coreUintW i = 0u; i < SPECIAL_RINGS; ++i)
    {
        coreObject3D& oRing = m_aRing[i];
        if(!oRing.GetAlpha()) continue;

        // 
        const coreFloat& fScale = oRing.GetCollisionModifier().x;
        const coreFloat& fSpeed = oRing.GetCollisionModifier().y;

        // 
        oRing.SetAlpha(MAX(oRing.GetAlpha() - fSpeed * Core::System->GetTime(), 0.0f));
        oRing.SetSize (coreVector3(8.0f,8.0f,8.0f) * (fScale * (1.0f - oRing.GetAlpha())));
        oRing.Move();
    }

    // 
    m_iSoundGuard = 0u;

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
// create centered particle splash
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

void cSpecialEffects::CreateSplashFire(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum)
{
    // 
    m_ParticleFire.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(3.0f), coreVector3::Rand(-fScale, fScale));
        pParticle->SetScaleAbs   (4.0f,                              12.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI*0.5f, PI*0.5f));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.4f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// create directional particle blow
void cSpecialEffects::CreateBlowColor(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor)
{
    ASSERT(vDirection.IsNormalized())

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

void cSpecialEffects::CreateBlowDark(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat& fScale, const coreUintW& iNum)
{
    ASSERT(vDirection.IsNormalized())

    // 
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition+coreVector3::Rand(1.0f), coreVector3::Rand(-fScale, fScale) * 0.25f + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (3.5f,                              1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// create whirling particle charge
void cSpecialEffects::CreateChargeColor(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum, const coreVector3& vColor)
{
    // 
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir  = coreVector3::Rand();
        const coreFloat   fRand = Core::Rand->Float(fScale);

        pParticle->SetPositionAbs(vPosition + vDir*fRand,           vPosition + vDir*(fScale-fRand));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),        coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}

void cSpecialEffects::CreateChargeDark(const coreVector3& vPosition, const coreFloat& fScale, const coreUintW& iNum)
{
    // 
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir  = coreVector3::Rand();
        const coreFloat   fRand = Core::Rand->Float(fScale);

        pParticle->SetPositionAbs(vPosition + vDir*fRand,           vPosition + vDir*(fScale-fRand));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateBlast(const coreVector3& vPosition, const coreFloat& fScale, const coreFloat& fSpeed, const coreVector3& vColor)
{
    // 
    if(++m_iCurBlast >= SPECIAL_BLASTS) m_iCurBlast = 0u;
    coreObject3D& oBlast = m_aBlast[m_iCurBlast];

    // 
    oBlast.SetPosition         (vPosition);
    oBlast.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oBlast.SetDirection        (coreVector3(coreVector2::Rand(), 0.0f));
    oBlast.SetColor4           (coreVector4(vColor * 0.65f, 1.0f));
    oBlast.SetCollisionModifier(coreVector3(fScale, fSpeed, 0.0f));
}


// ****************************************************************
// 
void cSpecialEffects::CreateRing(const coreVector3& vPosition, const coreVector3& vDirection, const coreVector3& vOrientation, const coreFloat& fScale, const coreFloat& fSpeed, const coreVector3& vColor)
{
    // 
    if(++m_iCurRing >= SPECIAL_BLASTS) m_iCurRing = 0u;
    coreObject3D& oRing = m_aRing[m_iCurRing];

    // 
    oRing.SetPosition         (vPosition);
    oRing.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oRing.SetDirection        (vDirection);
    oRing.SetOrientation      (vOrientation);
    oRing.SetColor4           (coreVector4(vColor * 0.65f, 1.0f));
    oRing.SetCollisionModifier(coreVector3(fScale, fSpeed, 0.0f));
}


// ****************************************************************
// 
void cSpecialEffects::PlaySound(const coreVector3& vPosition, const coreFloat& fVolume, const eSoundEffect& iSoundIndex)
{
    ASSERT(fVolume)

    // 
    const coreUintW iFileIndex = iSoundIndex & 0xFFu;

    // 
    if(CONTAINS_BIT(m_iSoundGuard, iFileIndex)) return;
    ADD_BIT(m_iSoundGuard, iFileIndex)

    // 
    coreFloat fBaseVolume, fBasePitch, fBasePitchRnd;
    switch(iSoundIndex)
    {
    default: ASSERT(false)
    case SOUND_EXPLOSION_ENERGY_SMALL:   fBaseVolume = 1.0f; fBasePitch = 1.5f; fBasePitchRnd = 0.1f; break;
    case SOUND_EXPLOSION_ENERGY_BIG:     fBaseVolume = 3.0f; fBasePitch = 0.8f; fBasePitchRnd = 0.1f; break;
    case SOUND_EXPLOSION_PHYSICAL_SMALL: fBaseVolume = 1.0f; fBasePitch = 0.9f; fBasePitchRnd = 0.1f; break;
    case SOUND_EXPLOSION_PHYSICAL_BIG:   fBaseVolume = 1.0f; fBasePitch = 0.6f; fBasePitchRnd = 0.1f; break;
    //case SOUND_RUSH_SHORT:             fBaseVolume = 1.5f; fBasePitch = 1.5f; fBasePitchRnd = 0.1f; break;
    //case SOUND_RUSH_LONG:              fBaseVolume = 1.5f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f; break;
    }

    // 
    m_apSound[iFileIndex]->PlayPosition(NULL, fVolume * fBaseVolume, fBasePitch, fBasePitchRnd, false, vPosition);
}


// ****************************************************************
// 
void cSpecialEffects::ShakeScreen(const coreFloat& fStrength)
{
    // 
    m_fShakeStrength = fStrength;

    // TODO # only with gamepads selected as active input device 
    // 
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
        Core::Input->RumbleJoystick(i, fStrength, 300u);
}


// ****************************************************************
// 
void cSpecialEffects::MacroExplosionColorSmall(const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_SMALL, vColor);
    this         ->CreateBlast      (vPosition, SPECIAL_BLAST_SMALL,  LERP(coreVector3(0.5f,0.5f,0.5f), vColor, 0.5f));
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionColorBig(const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_BIG, vColor);
    this         ->CreateBlast      (vPosition, SPECIAL_BLAST_BIG,  LERP(coreVector3(0.5f,0.5f,0.5f), vColor, 0.5f));
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionDarkSmall(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave      (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashDark(vPosition, SPECIAL_SPLASH_SMALL);
    this         ->CreateBlast     (vPosition, SPECIAL_BLAST_SMALL, coreVector3(0.5f,0.5f,0.5f));
    this         ->PlaySound       (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    this         ->ShakeScreen     (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionDarkBig(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave      (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashDark(vPosition, SPECIAL_SPLASH_BIG);
    this         ->CreateBlast     (vPosition, SPECIAL_BLAST_BIG, coreVector3(0.5f,0.5f,0.5f));
    this         ->PlaySound       (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
    this         ->ShakeScreen     (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionPhysicalSmall(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_SMALL, COLOR_FIRE_ORANGE);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_SMALL);
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_PHYSICAL_SMALL);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionPhysicalBig(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_BIG, COLOR_FIRE_ORANGE);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_BIG);
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_PHYSICAL_BIG);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroEruptionColorSmall(const coreVector3& vPosition, const coreVector2& vDirection, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateBurst    (vPosition,             vDirection,        DISTORTION_BURST_SMALL);
    this         ->CreateBlowColor(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_SMALL, vColor);
    this         ->PlaySound      (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
}

void cSpecialEffects::MacroEruptionColorBig(const coreVector3& vPosition, const coreVector2& vDirection, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateBurst    (vPosition,             vDirection,        DISTORTION_BURST_BIG);
    this         ->CreateBlowColor(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_BIG, vColor);
    this         ->PlaySound      (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
}

void cSpecialEffects::MacroEruptionDarkSmall(const coreVector3& vPosition, const coreVector2& vDirection)
{
    // 
    g_pDistortion->CreateBurst   (vPosition,             vDirection,        DISTORTION_BURST_SMALL);
    this         ->CreateBlowDark(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_SMALL);
    this         ->PlaySound     (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
}

void cSpecialEffects::MacroEruptionDarkBig(const coreVector3& vPosition, const coreVector2& vDirection)
{
    // 
    g_pDistortion->CreateBurst   (vPosition,             vDirection,        DISTORTION_BURST_BIG);
    this         ->CreateBlowDark(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_BIG);
    this         ->PlaySound     (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
}