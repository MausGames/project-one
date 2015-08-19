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
: m_ParticleColor    (512u)
, m_ParticleDark     (256u)
, m_ParticleSmoke    (512u)
, m_ParticleFire     (256u)
, m_apLightningOwner {}
, m_LightningList    (SPECIAL_LIGHTNINGS)
, m_iCurLightning    (0u)
, m_iCurBlast        (0u)
, m_iCurRing         (0u)
, m_iSoundGuard      (0u)
, m_ShakeTimer       (coreTimer(1.0f, 30.0f, 0u))
, m_fShakeStrength   (0.0f)
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
    for(coreUintW i = 0u; i < SPECIAL_LIGHTNINGS; ++i)
    {
        m_aLightning[i].DefineModel  (Core::Manager::Object->GetLowModel());
        m_aLightning[i].DefineTexture(0u, "effect_lightning.png");
        m_aLightning[i].DefineProgram("effect_lightning_program");
        m_aLightning[i].SetAlpha     (0.0f);
    }
    m_LightningList.DefineProgram("effect_lightning_inst_program");

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
        m_aRing[i].DefineProgram("effect_energy_ring_program");
        m_aRing[i].SetAlpha     (0.0f);
        m_aRing[i].SetTexSize   (coreVector2(4.0f,8.0f) * 0.4f);
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
    nLoadSoundFunc(m_apSound, SOUND_RUSH_SHORT,               "effect_rush.wav");
    nLoadSoundFunc(m_apSound, SOUND_RUSH_LONG,                "effect_rush.wav");

    // 
    m_ShakeTimer.Play(CORE_TIMER_PLAY_RESET);

    STATIC_ASSERT(SPECIAL_SOUNDS <= sizeof(m_iSoundGuard)*8u)
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

        // enable additive blending (keep alpha aggregation)
        glBlendFuncSeparate(FOREGROUND_BLEND_SUM, FOREGROUND_BLEND_ALPHA);
        {
            // render fire particle system
            m_ParticleFire.Render();

            // render lightning sprites
            m_LightningList.Render();
        }
        glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);

        // render all blast and ring objects
        auto nRenderFunc = [](coreObject3D* OUTPUT pArray, const coreUintW& iSize)
        {
            for(coreUintW i = 0u; i < iSize; ++i)
            {
                // check for visibility
                coreObject3D& oObject = pArray[i];
                if(!oObject.GetAlpha()) continue;

                // render the object
                oObject.Render();
            }
        };
        nRenderFunc(m_aBlast, SPECIAL_BLASTS);
        glCullFace(GL_FRONT);
        nRenderFunc(m_aRing,  SPECIAL_RINGS);   // # 1
        glCullFace(GL_BACK);
        nRenderFunc(m_aRing,  SPECIAL_RINGS);   // # 2
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
    FOR_EACH_DYN(it, *m_LightningList.List())
    {
        coreObject3D* pLightning = (*it);

        // 
        coreObject3D* pOwner = m_apLightningOwner[pLightning - m_aLightning];
        if(pOwner) pLightning->SetPosition(pOwner->GetPosition());

        // 
        pLightning->SetAlpha(pLightning->GetAlpha() - /*2.2f*/4.0f * Core::System->GetTime());

        // 
        if(pLightning->GetAlpha() > 0.0f) DYN_KEEP  (it)
                                     else DYN_REMOVE(it, *m_LightningList.List())
    }
    m_LightningList.MoveNormal();

    // loop through all blast objects
    for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
    {
        coreObject3D& oBlast = m_aBlast[i];
        if(!oBlast.GetAlpha()) continue;

        // 
        const coreFloat& fScale = oBlast.GetCollisionModifier().x;
        const coreFloat& fSpeed = oBlast.GetCollisionModifier().y;

        // 
        oBlast.SetAlpha    (MAX(oBlast.GetAlpha() - fSpeed * Core::System->GetTime(), 0.0f));
        oBlast.SetSize     (coreVector3(8.0f,8.0f,8.0f) * (fScale * (1.0f - oBlast.GetAlpha())));
        oBlast.SetTexOffset(coreVector2(0.0f,0.1f) * oBlast.GetAlpha());
        oBlast.Move();
    }

    // loop through all ring objects
    for(coreUintW i = 0u; i < SPECIAL_RINGS; ++i)
    {
        coreObject3D& oRing = m_aRing[i];
        if(!oRing.GetAlpha()) continue;

        // 
        const coreFloat& fScale = oRing.GetCollisionModifier().x;
        const coreFloat& fSpeed = oRing.GetCollisionModifier().y;
        const coreFloat& fTime  = oRing.GetCollisionModifier().z;

        // 
        c_cast<coreFloat&>(fTime) = MAX(fTime - fSpeed * Core::System->GetTime(), 0.0f);
        const coreFloat    fValue = 1.0f - std::pow(fTime, 6);

        // 
        oRing.SetAlpha    (MIN(fTime * fTime * 1.4f, 1.0f));
        oRing.SetSize     (coreVector3(8.0f,12.0f,8.0f) * (fScale * fValue));
        oRing.SetTexOffset(coreVector2(0.1f,0.2f) * fValue);
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
void cSpecialEffects::CreateTrailSmoke(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat& fScale, const coreUintW& iNum)
{
    ASSERT(vDirection.IsNormalized())

    // 
    m_ParticleSmoke.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        pParticle->SetPositionRel(vPosition, vDirection * fScale + coreVector3::Rand(-2.0f, 2.0f));
        pParticle->SetScaleAbs   (4.0f,                              8.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),        Core::Rand->Float(-PI*0.1f, PI*0.1f));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),  coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (0.7f);
    });
}


// ****************************************************************
// 
coreFloat cSpecialEffects::CreateLightning(const coreVector2& vPosFrom, const coreVector2& vPosTo, const coreFloat& fWidth, const coreVector3& vColor, const coreVector2& vTexSizeFactor, const coreFloat& fTexOffset)
{
    // 
    if(++m_iCurLightning >= SPECIAL_LIGHTNINGS) m_iCurLightning = 0u;
    coreObject3D& oLightning = m_aLightning[m_iCurLightning];

    // 
    m_apLightningOwner[m_iCurLightning] = NULL;




    const coreVector2 vDiff =  vPosTo - vPosFrom;


    //if(vDiff.LengthSq() < fWidth*fWidth*0.5f)
    //{
    //    const coreFloat fSide = (vDiff.x >= 0.0f) ? -1.0f : 1.0f;
    //
    //    const coreVector2 vEdgePos = vPosFrom + (fWidth * fSide * 0.8f) * vDiff.Rotated90().Normalize();
    //
    //    g_pSpecialEffects->CreateLightning(vPosFrom, vEdgePos, fWidth, vColor, coreVector2(-fSide,0.92f), 0.0f);
    //    g_pSpecialEffects->CreateLightning(vEdgePos, vPosTo,   fWidth, vColor, coreVector2( fSide,0.97f), 0.53f);
    //
    //    return 1.0f;
    //}



    const coreVector2 vPos  = (vPosTo + vPosFrom) * 0.5f;
    const coreVector2 vDir  = vDiff.Normalized();
    const coreFloat   fLen  = vDiff.Length();

    const coreVector2 vSize = coreVector2(fWidth, fLen);


    coreFloat fTexLen = vSize.yx().AspectRatio() * SPECIAL_LIGHTNING_CUTOUT * SPECIAL_LIGHTNING_RESIZE;
    if(fTexLen > 0.5f) fTexLen = std::round(fTexLen * 2.0f) * 0.5f;
    else fTexLen = 0.5f;


    // 
    oLightning.SetPosition (coreVector3(vPos, 0.0f));
    oLightning.SetSize     (coreVector3( vSize,      1.0f));
    oLightning.SetDirection(coreVector3(-vDir, 0.0f));
    oLightning.SetColor4   (coreVector4( vColor,     1.0f));

    if(vDiff.LengthSq() < fWidth*fWidth*0.5f)
    {
        oLightning.SetTexSize  (coreVector2( 0.5f, 0.25f) * vTexSizeFactor);
        oLightning.SetTexOffset(coreVector2(0.125f - oLightning.GetTexSize().x * 0.5f, (oLightning.GetTexSize().y - 0.25f) * -0.5f));
    }
    else
    {

    oLightning.SetTexSize  (coreVector2( SPECIAL_LIGHTNING_CUTOUT, fTexLen) * vTexSizeFactor);
    oLightning.SetTexOffset(coreVector2((1.0f - oLightning.GetTexSize().x) * 0.5f, fTexOffset));
    }

    // 
    ASSERT(m_LightningList.List()->size() <= SPECIAL_LIGHTNINGS)
    m_LightningList.BindObject(&oLightning);

    return fTexLen + fTexOffset;
}

void cSpecialEffects::CreateLightning(coreObject3D* pOwner, const coreVector2& vDirection, const coreVector2& vSize, const coreVector3& vColor, const coreVector2& vTexSizeFactor, const coreFloat& fTexOffset)
{
    //// 
    //const coreFloat fNewOffset = this->CreateLightning(coreVector3(0.0f,0.0f,0.0f), vDirection, vSize, vColor, vTexSizeFactor, fTexOffset);
    //
    //// 
    //m_apLightningOwner[m_iCurLightning] = pOwner;
    //
    //return fNewOffset;




    // 
    if(++m_iCurLightning >= SPECIAL_LIGHTNINGS) m_iCurLightning = 0u;
    coreObject3D& oLightning = m_aLightning[m_iCurLightning];

    // 
    m_apLightningOwner[m_iCurLightning] = pOwner;



    coreFloat fTexLen = vSize.yx().AspectRatio() * SPECIAL_LIGHTNING_CUTOUT * SPECIAL_LIGHTNING_RESIZE;


    // 
    oLightning.SetPosition (pOwner->GetPosition());
    oLightning.SetSize     (coreVector3( vSize,      1.0f));
    oLightning.SetDirection(coreVector3(-vDirection, 0.0f));
    oLightning.SetColor4   (coreVector4( vColor,     1.0f));
    oLightning.SetTexSize  (coreVector2( SPECIAL_LIGHTNING_CUTOUT, fTexLen) * vTexSizeFactor);
    oLightning.SetTexOffset(coreVector2((1.0f - oLightning.GetTexSize().x) * 0.5f, fTexOffset));

    // 
    ASSERT(m_LightningList.List()->size() <= SPECIAL_LIGHTNINGS)
    m_LightningList.BindObject(&oLightning);
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
    if(++m_iCurRing >= SPECIAL_RINGS) m_iCurRing = 0u;
    coreObject3D& oRing = m_aRing[m_iCurRing];

    // 
    oRing.SetPosition         (vPosition);
    oRing.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oRing.SetDirection        (vDirection);
    oRing.SetOrientation      (vOrientation);
    oRing.SetColor4           (coreVector4(vColor * 0.65f, 1.0f));
    oRing.SetCollisionModifier(coreVector3(fScale, fSpeed, 1.0f));
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
    case SOUND_RUSH_SHORT:               fBaseVolume = 1.5f; fBasePitch = 1.5f; fBasePitchRnd = 0.1f; break;
    case SOUND_RUSH_LONG:                fBaseVolume = 1.5f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f; break;
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
    this         ->CreateBlast      (vPosition, SPECIAL_BLAST_SMALL,  LERP(coreVector3(1.0f,1.0f,1.0f), vColor, 0.75f));
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionColorBig(const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_BIG, vColor);
    this         ->CreateBlast      (vPosition, SPECIAL_BLAST_BIG,  LERP(coreVector3(1.0f,1.0f,1.0f), vColor, 0.75f));
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionDarkSmall(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave      (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashDark(vPosition, SPECIAL_SPLASH_SMALL);
    this         ->CreateBlast     (vPosition, SPECIAL_BLAST_SMALL, coreVector3(1.0f,1.0f,1.0f));
    this         ->PlaySound       (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    this         ->ShakeScreen     (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionDarkBig(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave      (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashDark(vPosition, SPECIAL_SPLASH_BIG);
    this         ->CreateBlast     (vPosition, SPECIAL_BLAST_BIG, coreVector3(1.0f,1.0f,1.0f));
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