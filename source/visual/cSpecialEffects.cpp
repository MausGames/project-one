///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cSpecialEffects::cSpecialEffects()noexcept
: m_ParticleColor    (1024u)
, m_ParticleDark     (1024u)
, m_ParticleSmoke    (512u)
, m_ParticleFire     (1024u)
, m_apLightningOwner {}
, m_LightningList    (SPECIAL_LIGHTNINGS)
, m_iCurLightning    (0u)
, m_iCurBlast        (0u)
, m_eSoundGuard      (SOUND_FFFF)
, m_ShakeTimer       (coreTimer(1.0f, 30.0f, 0u))
, m_fShakeStrength   (0.0f)
, m_iShakeCount      (0u)
, m_bActive          (false)
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
        m_aLightning[i].DefineModel  (Core::Manager::Object->GetLowQuad());
        m_aLightning[i].DefineTexture(0u, "effect_lightning.png");
        m_aLightning[i].DefineProgram("effect_lightning_program");
        m_aLightning[i].SetAlpha     (0.0f);
    }
    m_LightningList.DefineProgram("effect_lightning_inst_program");

    // 
    for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
    {
        m_aBlast[i].DefineTexture(0u, "effect_energy.png");
        m_aBlast[i].DefineProgram("effect_energy_spheric_program");
        m_aBlast[i].SetAlpha     (0.0f);
    }
    m_apBlastModel[0] = Core::Manager::Resource->Get<coreModel>("object_sphere.md3");
    m_apBlastModel[1] = Core::Manager::Resource->Get<coreModel>("object_quad.md3");
    m_apBlastModel[2] = Core::Manager::Resource->Get<coreModel>("object_triangle.md3");

    // 
    const auto nLoadSoundFunc = [this](const eSoundEffect eSoundIndex, const coreChar* pcName)
    {
        coreSoundPtr& pSoundPtr = m_apSound[eSoundIndex & 0xFFu];

        if(!pSoundPtr) pSoundPtr = Core::Manager::Resource->Get<coreSound>(pcName);
        ASSERT(!std::strcmp(pSoundPtr.GetHandle()->GetName(), pcName))
    };
    nLoadSoundFunc(SOUND_EXPLOSION_ENERGY_SMALL,   "effect_explosion_energy.wav");
    nLoadSoundFunc(SOUND_EXPLOSION_ENERGY_BIG,     "effect_explosion_energy.wav");
    nLoadSoundFunc(SOUND_EXPLOSION_PHYSICAL_SMALL, "effect_explosion_physical.wav");
    nLoadSoundFunc(SOUND_EXPLOSION_PHYSICAL_BIG,   "effect_explosion_physical.wav");
    nLoadSoundFunc(SOUND_RUSH_SHORT,               "effect_rush.wav");
    nLoadSoundFunc(SOUND_RUSH_LONG,                "effect_rush.wav");

    // 
    m_ShakeTimer.Play(CORE_TIMER_PLAY_RESET);
}


// ****************************************************************
// render special-effects
void cSpecialEffects::Render()
{
    if(m_bActive)
    {
        glDisable(GL_DEPTH_TEST);
        {
            const coreBool bForeground = g_pForeground->IsTarget();

            // render particle systems
            m_ParticleColor.Render();
            m_ParticleDark .Render();
            m_ParticleSmoke.Render();

            // enable additive blending (keep alpha aggregation)
            if(bForeground) glBlendFuncSeparate(FOREGROUND_BLEND_SUM, FOREGROUND_BLEND_ALPHA);
                       else glBlendFunc        (FOREGROUND_BLEND_SUM);
            {
                // render fire particle system
                m_ParticleFire.Render();

                // render lightning sprites
                m_LightningList.Render();
            }
            if(bForeground) glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);
                       else glBlendFunc        (FOREGROUND_BLEND_DEFAULT);

            // render all blast objects
            const auto nRenderFunc = [](coreObject3D* OUTPUT pArray, const coreUintW iSize)
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
        }
        glEnable(GL_DEPTH_TEST);
    }
}


// ****************************************************************
// move special-effects
void cSpecialEffects::Move()
{
    // 
    m_bActive = m_ParticleColor.GetNumActiveParticles() ||
                m_ParticleDark .GetNumActiveParticles() ||
                m_ParticleSmoke.GetNumActiveParticles() ||
                m_ParticleFire .GetNumActiveParticles() ||
                m_LightningList.List()->size()          ||
                std::any_of(m_aBlast, m_aBlast + SPECIAL_BLASTS, [](const coreObject3D& oBlast) {return oBlast.GetAlpha() ? true : false;});
    if(m_bActive)
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
            const coreObject3D* pOwner = m_apLightningOwner[pLightning - m_aLightning];
            if(pOwner) pLightning->SetPosition(pOwner->GetPosition());

            // 
            pLightning->SetAlpha(pLightning->GetAlpha() - 4.0f * Core::System->GetTime());

            // 
            if(pLightning->GetAlpha() > 0.0f)
                 DYN_KEEP  (it)
            else DYN_REMOVE(it, *m_LightningList.List())
        }
        m_LightningList.MoveNormal();

        // loop through all blast objects
        for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
        {
            coreObject3D& oBlast = m_aBlast[i];
            if(!oBlast.GetAlpha()) continue;

            // 
            const coreFloat fScale = oBlast.GetCollisionModifier().x;
            const coreFloat fSpeed = oBlast.GetCollisionModifier().y;

            // 
            oBlast.SetAlpha    (MAX(oBlast.GetAlpha() - fSpeed * Core::System->GetTime(), 0.0f));
            oBlast.SetSize     (coreVector3(8.0f,8.0f,8.0f) * (fScale * (1.0f - oBlast.GetAlpha())));
            oBlast.SetTexOffset(coreVector2(0.0f,0.1f) * oBlast.GetAlpha());
            oBlast.Move();
        }
    }

    // reset sound-guard
    m_eSoundGuard = SOUND_FFFF;

    // 
    if(m_fShakeStrength && m_ShakeTimer.Update(1.0f))
    {
        // update shake animation
        m_fShakeStrength = MAX(m_fShakeStrength - 0.07f, 0.0f);   // # without delta-time
        m_iShakeCount    = m_iShakeCount + 1u;

        // 
        g_pPostProcessing->SetPosition(g_vHudDirection.InvertedX() * (m_fShakeStrength * 0.01f * ((m_iShakeCount & 0x01u) ? 1.0f : -1.0f)));
    }
}


// ****************************************************************
// create centered particle splash
void cSpecialEffects::CreateSplashColor(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleColor.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,           vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.5f,                       1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateSplashDark(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,                 vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateSplashSmoke(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleSmoke.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,           vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.0f,                       12.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), Core::Rand->Float(-PI*0.1f, PI*0.1f));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (0.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}

void cSpecialEffects::CreateSplashFire(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleFire.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,                 vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (4.5f,                             11.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       Core::Rand->Float(-PI*0.5f, PI*0.5f));
        pParticle->SetColor4Abs  (coreVector4(vColor*0.926f, 1.0f), coreVector4(vColor*0.926f, 0.0f));
        pParticle->SetSpeed      (1.4f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// create directional particle blow
void cSpecialEffects::CreateBlowColor(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor)
{
    ASSERT(vDirection.IsNormalized())

    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleColor.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,           vDir * (Core::Rand->Float(fScale) * 0.25f) + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (3.5f,                       1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateBlowDark(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat fScale, const coreUintW iNum)
{
    ASSERT(vDirection.IsNormalized())

    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,                 vDir * (Core::Rand->Float(fScale) * 0.25f) + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateChargeColor(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleColor.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionAbs(vPosition + vDir * Core::Rand->Float(fScale), vPosition + vDir);
        pParticle->SetScaleAbs   (3.5f,                                         1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),                   Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),                    coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateChargeDark(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionAbs(vPosition + vDir * Core::Rand->Float(fScale), vPosition + vDir);
        pParticle->SetScaleAbs   (3.5f,                                         1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),                   Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),             coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateWhirlColor(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3& vColor)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleColor.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);
        const coreFloat   fRand = Core::Rand->Float(fScale);

        pParticle->SetPositionAbs(vPosition + vDir * fRand,         vPosition + vDir * (fScale-fRand));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),        coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}

void cSpecialEffects::CreateWhirlDark(const coreVector3& vPosition, const coreFloat fScale, const coreUintW iNum)
{
    // 
    const coreFloat fBase = Core::Rand->Float(-PI, PI);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum));

    // 
    coreUintW i = 0u;
    m_ParticleDark.GetDefaultEffect()->CreateParticle(iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);
        const coreFloat   fRand = Core::Rand->Float(fScale);

        pParticle->SetPositionAbs(vPosition + vDir * fRand,         vPosition + vDir * (fScale-fRand));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       Core::Rand->Float(-PI, PI));
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// 
coreFloat cSpecialEffects::CreateLightning(const coreVector2& vPosFrom, const coreVector2& vPosTo, const coreFloat fWidth, const coreVector3& vColor, const coreVector2& vTexSizeFactor, const coreFloat fTexOffset)
{
    // 
    if(++m_iCurLightning >= SPECIAL_LIGHTNINGS) m_iCurLightning = 0u;
    coreObject3D& oLightning = m_aLightning[m_iCurLightning];

    // 
    const coreVector2 vDiff      = (vPosTo - vPosFrom);
    const coreVector2 vPosition  = (vPosTo + vPosFrom) * 0.5f;
    const coreVector2 vDirection = vDiff.Normalized();
    const coreFloat   fLength    = vDiff.Length();
    const coreVector2 vSize      = coreVector2(fWidth, fLength);

    // 
    oLightning.SetPosition (coreVector3(vPosition,  0.0f));
    oLightning.SetSize     (coreVector3(vSize,      1.0f));
    oLightning.SetDirection(coreVector3(vDirection, 0.0f));
    oLightning.SetColor4   (coreVector4(vColor,     1.0f));

    coreFloat fTexLen = 0.0f;
    if(fLength < fWidth*0.9f)
    {
        // 
        oLightning.SetTexSize  ( coreVector2(SPECIAL_LIGHTNING_CUTOUT, 0.25f) * vTexSizeFactor);
        oLightning.SetTexOffset((coreVector2(0.25f,0.25f) - oLightning.GetTexSize()) * 0.5f);
    }
    else
    {
        // 
        fTexLen = vSize.yx().AspectRatio() * SPECIAL_LIGHTNING_CUTOUT * SPECIAL_LIGHTNING_RESIZE;
        fTexLen = ROUND(MAX(fTexLen, 0.5f) * 2.0f) * 0.5f;

        // 
        oLightning.SetTexSize  (coreVector2(SPECIAL_LIGHTNING_CUTOUT, fTexLen) * vTexSizeFactor);
        oLightning.SetTexOffset(coreVector2((1.0f - oLightning.GetTexSize().x) * 0.5f, fTexOffset));
    }

    // 
    WARN_IF(m_LightningList.List()->count(&oLightning)) {}
       else m_LightningList.BindObject(&oLightning);

    // 
    m_apLightningOwner[m_iCurLightning] = NULL;

    // 
    return fTexLen + fTexOffset;
}

void cSpecialEffects::CreateLightning(coreObject3D* pOwner, const coreVector2& vDirection, const coreFloat fLength, const coreFloat fWidth, const coreVector3& vColor, const coreVector2& vTexSizeFactor, const coreFloat fTexOffset)
{
    // 
    if(++m_iCurLightning >= SPECIAL_LIGHTNINGS) m_iCurLightning = 0u;
    coreObject3D& oLightning = m_aLightning[m_iCurLightning];

    // 
    const coreVector2 vSize   = coreVector2(fWidth, fLength);
    const coreFloat   fTexLen = vSize.yx().AspectRatio() * SPECIAL_LIGHTNING_CUTOUT * SPECIAL_LIGHTNING_RESIZE;

    // 
    oLightning.SetPosition (pOwner->GetPosition());
    oLightning.SetSize     (coreVector3(vSize,      1.0f));
    oLightning.SetDirection(coreVector3(vDirection, 0.0f));
    oLightning.SetColor4   (coreVector4(vColor,     1.0f));
    oLightning.SetTexSize  (coreVector2(SPECIAL_LIGHTNING_CUTOUT, fTexLen) * vTexSizeFactor);
    oLightning.SetTexOffset(coreVector2((1.0f - oLightning.GetTexSize().x) * 0.5f, fTexOffset));

    // 
    WARN_IF(m_LightningList.List()->count(&oLightning)) {}
       else m_LightningList.BindObject(&oLightning);

    // 
    m_apLightningOwner[m_iCurLightning] = pOwner;
}


// ****************************************************************
// 
void cSpecialEffects::CreateBlastSphere(const coreVector3& vPosition, const coreFloat fScale, const coreFloat fSpeed, const coreVector3& vColor)
{
    // 
    if(++m_iCurBlast >= SPECIAL_BLASTS) m_iCurBlast = 0u;
    coreObject3D& oBlast = m_aBlast[m_iCurBlast];

    ASSERT(!oBlast.GetAlpha())

    // 
    oBlast.DefineModel         (m_apBlastModel[0]);
    oBlast.SetPosition         (vPosition);
    oBlast.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oBlast.SetDirection        (coreVector3(coreVector2::Rand(), 0.0f));
    oBlast.SetColor4           (coreVector4(vColor * 0.65f, 1.0f));
    oBlast.SetTexSize          (coreVector2(12.0f,12.0f));
    oBlast.SetCollisionModifier(coreVector3(fScale, fSpeed, 0.0f));
}

void cSpecialEffects::CreateBlastQuad(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat fScale, const coreFloat fSpeed, const coreVector3& vColor)
{
    // 
    if(++m_iCurBlast >= SPECIAL_BLASTS) m_iCurBlast = 0u;
    coreObject3D& oBlast = m_aBlast[m_iCurBlast];

    ASSERT(!oBlast.GetAlpha())

    // 
    oBlast.DefineModel         (m_apBlastModel[1]);
    oBlast.SetPosition         (vPosition);
    oBlast.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oBlast.SetDirection        (vDirection);
    oBlast.SetColor4           (coreVector4(vColor, 1.0f));
    oBlast.SetTexSize          (coreVector2(3.0f,1.2f));
    oBlast.SetCollisionModifier(coreVector3(fScale * 0.77f, fSpeed, 0.0f));
}

void cSpecialEffects::CreateBlastTriangle(const coreVector3& vPosition, const coreVector3& vDirection, const coreFloat fScale, const coreFloat fSpeed, const coreVector3& vColor)
{
    // 
    if(++m_iCurBlast >= SPECIAL_BLASTS) m_iCurBlast = 0u;
    coreObject3D& oBlast = m_aBlast[m_iCurBlast];

    ASSERT(!oBlast.GetAlpha())

    // 
    oBlast.DefineModel         (m_apBlastModel[2]);
    oBlast.SetPosition         (vPosition);
    oBlast.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oBlast.SetDirection        (vDirection);
    oBlast.SetColor4           (coreVector4(vColor, 1.0f));
    oBlast.SetTexSize          (coreVector2(3.0f,1.2f));
    oBlast.SetCollisionModifier(coreVector3(fScale * 0.69f, fSpeed, 0.0f));
}


// ****************************************************************
// 
void cSpecialEffects::PlaySound(const coreVector3& vPosition, const coreFloat fVolume, const eSoundEffect eSoundIndex)
{
    ASSERT(fVolume > 0.0f)

    // 
    if(m_eSoundGuard == eSoundIndex) return;
    m_eSoundGuard = eSoundIndex;

    // 
    coreFloat fBaseVolume, fBasePitch, fBasePitchRnd;
    switch(eSoundIndex)
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
    m_apSound[eSoundIndex & 0xFFu]->PlayPosition(NULL, fVolume * fBaseVolume, fBasePitch, fBasePitchRnd, false, vPosition);
}


// ****************************************************************
// 
void cSpecialEffects::RumblePlayer(const cPlayer* pPlayer, const coreFloat fStrength, const coreUint32 iLengthMs)
{
    ASSERT(fStrength && iLengthMs)
    if(!STATIC_ISVALID(g_pGame)) return;

    // loop through all active players
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pCurPlayer, const coreUintW i)
    {
        if((pPlayer != pCurPlayer) && (pPlayer != NULL)) return;

        const coreUint8   iRumble     = g_CurConfig.Input.aiRumble[i];
        const coreUintW   iJoystickID = g_CurConfig.Input.aiType  [i] - INPUT_SETS_KEYBOARD;
        const sGameInput* pCurInput   = pCurPlayer->GetInput();

        // check for valid configuration
        if(iRumble && (iJoystickID < Core::Input->GetJoystickNum()))
        {
            // check for valid input set
            if((pCurInput == &g_TotalInput) || (P_TO_UI(pCurInput - g_aGameInput) < INPUT_SETS))
            {
                // create rumble effect
                Core::Input->RumbleJoystick(iJoystickID, CLAMP(fStrength * I_TO_F(iRumble) * 0.1f, 0.0f, 1.0f), iLengthMs);
            }
        }
    });
}


// ****************************************************************
// 
void cSpecialEffects::ShakeScreen(const coreFloat fStrength)
{
    if(m_fShakeStrength < fStrength)
    {
        // 
        m_fShakeStrength = fStrength;
        m_iShakeCount    = 0u;

        // 
        this->RumblePlayer(NULL, fStrength * 0.5f, 250u);
    }
}


// ****************************************************************
// 
void cSpecialEffects::MacroExplosionColorSmall(const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_SMALL, vColor);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_SMALL,  LERP(coreVector3(1.0f,1.0f,1.0f), vColor, 0.75f));
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionColorBig(const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_BIG, vColor);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_BIG,  LERP(coreVector3(1.0f,1.0f,1.0f), vColor, 0.75f));
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionDarkSmall(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_SMALL);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_SMALL, COLOR_ENERGY_WHITE);
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionDarkBig(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_BIG);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_BIG, COLOR_ENERGY_WHITE);
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_ENERGY_BIG);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionPhysicalColorSmall(const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_SMALL,    vColor);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_SMALL, vColor);
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_PHYSICAL_SMALL);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionPhysicalColorBig(const coreVector3& vPosition, const coreVector3& vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_BIG,    vColor);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_BIG, vColor);
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_PHYSICAL_BIG);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionPhysicalDarkSmall(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_SMALL);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_SMALL, coreVector3(0.22f,0.22f,0.22f));
    this         ->PlaySound        (vPosition, 1.0f, SOUND_EXPLOSION_PHYSICAL_SMALL);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionPhysicalDarkBig(const coreVector3& vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_BIG);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_BIG, coreVector3(0.22f,0.22f,0.22f));
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