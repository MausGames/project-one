///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cSpecialEffects::cSpecialEffects()noexcept
: m_aParticleColor   {coreParticleSystem(1024u), coreParticleSystem(1024u)}
, m_aParticleDark    {coreParticleSystem(512u),  coreParticleSystem(512u)}
, m_aParticleSmoke   {coreParticleSystem(512u),  coreParticleSystem(512u)}
, m_aParticleFire    {coreParticleSystem(512u),  coreParticleSystem(512u)}
, m_aParticleDot     {coreParticleSystem(256u),  coreParticleSystem(256u)}
, m_apLightningOwner {}
, m_LightningList    (SPECIAL_LIGHTNINGS)
, m_iCurLightning    (0u)
, m_afGustTime       {}
, m_afGustSide       {}
, m_fGustAngle       (0.0f)
, m_fGustSpawn       (0.0f)
, m_GustList         (SPECIAL_GUSTS)
, m_iCurGust         (0u)
, m_iCurBlast        (0u)
, m_iCurExplosion    (0u)
, m_aiSoundGuard     {}
, m_afRumbleTime     {}
, m_afRumbleStrength {}
, m_ShakeTimer       (coreTimer(1.0f, 30.0f, 0u))
, m_fShakeStrength   (0.0f)
, m_fShakeOverride   (1.0f)
, m_iShakeCount      (0u)
, m_iShakeType       (0u)
, m_fFreezeTime      (0.0f)
, m_fSlowTime        (0.0f)
, m_fSlowStrength    (0.0f)
, m_iEffectFrame     (0u)
, m_iEffectCount     (0u)
, m_iBreakupCount    (0u)
, m_aiParticleFract  {}
, m_bActive          (false)
{
    // 
    for(coreUintW i = 0u; i < 2u; ++i)
    {
        m_aParticleColor[i].DefineProgram("effect_particle_color_program");
        m_aParticleColor[i].DefineTexture(0u, "effect_particle.png");
        m_aParticleDark [i].DefineProgram("effect_particle_dark_program");
        m_aParticleDark [i].DefineTexture(0u, "effect_particle.png");
        m_aParticleSmoke[i].DefineProgram("effect_particle_smoke_program");
        m_aParticleSmoke[i].DefineTexture(0u, "effect_smoke.png");
        m_aParticleFire [i].DefineProgram("effect_particle_fire_program");
        m_aParticleFire [i].DefineTexture(0u, "effect_smoke.png");
        m_aParticleDot  [i].DefineProgram("effect_particle_smoke_program");                                      
        m_aParticleDot  [i].DefineTexture(0u, "environment_particle_02.png");
    }

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
    for(coreUintW i = 0u; i < SPECIAL_GUSTS; ++i)
    {
        m_aGust[i].DefineModel  (Core::Manager::Object->GetLowQuad());
        m_aGust[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aGust[i].DefineProgram("effect_gust_program");
    }
    m_GustList.DefineProgram("effect_gust_inst_program");

    // 
    for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
    {
        m_aBlast[i].DefineTexture(0u, "effect_energy.png");
        m_aBlast[i].DefineProgram("effect_energy_spheric_program");
        m_aBlast[i].SetAlpha     (0.0f);
    }
    m_apBlastModel[0] = Core::Manager::Resource->Get<coreModel>("object_sphere.md3");
    m_apBlastModel[1] = Core::Manager::Resource->Get<coreModel>("object_cube_top.md3");
    m_apBlastModel[2] = Core::Manager::Resource->Get<coreModel>("object_tetra_top.md3");

    // 
    for(coreUintW i = 0u; i < SPECIAL_EXPLOSION; ++i)
    {
        m_aExplosionBody[i].DefineModel  ("effect_explosion.md3");
        m_aExplosionBody[i].DefineProgram("effect_explosion_program");
        m_aExplosionBody[i].SetAlpha     (0.0f);
    }

    // 
    for(coreUintW i = 0u; i < SPECIAL_EXPLOSION; ++i)
    {
        //m_aExplosionWave[i].DefineModel  (Core::Manager::Resource->Load<coreModel>("effect_torus.md3", CORE_RESOURCE_UPDATE_AUTO, "data/models/effect_torus.md3"));//   "object_ring.md3");
        m_aExplosionWave[i].DefineModel  ("object_ring.md3");
        m_aExplosionWave[i].DefineProgram("effect_explosion_program");
        m_aExplosionWave[i].SetAlpha     (0.0f);
        
        //m_aExplosionWave[i].SetDirection  (coreVector3(0.0f,0.0f,1.0f));
        //m_aExplosionWave[i].SetOrientation(coreVector3(0.0f,1.0f,0.0f));
    }

    // 
    const auto nLoadSoundFunc = [this](const eSoundEffect eSoundIndex, const coreChar* pcName)
    {
        ASSERT(eSoundIndex < SOUND_MAX)
        coreSoundPtr& pSoundPtr = m_apSound[eSoundIndex];

        if(!pSoundPtr) pSoundPtr = Core::Manager::Resource->Get<coreSound>(pcName);
        ASSERT(!std::strcmp(pSoundPtr.GetHandle()->GetName(), pcName))
    };
    nLoadSoundFunc(SOUND_PLAYER_EXPLOSION,     "player_explosion.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_01,   "enemy_explosion_01.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_02,   "enemy_explosion_02.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_03,   "enemy_explosion_03.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_04,   "enemy_explosion_04.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_05,   "enemy_explosion_05.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_06,   "enemy_explosion_06.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_07,   "enemy_explosion_07.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_08,   "enemy_explosion_08.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_09,   "enemy_explosion_09.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_10,   "enemy_explosion_10.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_11,   "enemy_explosion_11.wav");
    nLoadSoundFunc(SOUND_ENEMY_EXPLOSION_12,   "enemy_explosion_12.wav");
    nLoadSoundFunc(SOUND_WEAPON_RAY,           "weapon_ray.wav");
    nLoadSoundFunc(SOUND_WEAPON_ENEMY,         "weapon_enemy.wav");
    nLoadSoundFunc(SOUND_BULLET_HIT,           "bullet_hit.wav");
    nLoadSoundFunc(SOUND_BULLET_REFLECT,       "bullet_reflect.wav");
    nLoadSoundFunc(SOUND_BULLET_VANISH,        "bullet_vanish.wav");
    nLoadSoundFunc(SOUND_SHIELD_HIT,           "shield_hit.wav");
    nLoadSoundFunc(SOUND_SHIELD_DESTROY,       "shield_destroy.wav");
    nLoadSoundFunc(SOUND_MEDAL_BRONZE,         "medal_bronze.wav");
    nLoadSoundFunc(SOUND_MEDAL_SILVER,         "medal_silver.wav");
    nLoadSoundFunc(SOUND_MEDAL_GOLD,           "medal_gold.wav");
    nLoadSoundFunc(SOUND_MEDAL_PLATINUM,       "medal_platinum.wav");
    nLoadSoundFunc(SOUND_MEDAL_DARK,           "medal_dark.wav");
    nLoadSoundFunc(SOUND_BADGE,                "badge.wav");
    nLoadSoundFunc(SOUND_HELPER,               "helper.wav");
    nLoadSoundFunc(SOUND_FRAGMENT_COLLECT,     "fragment_collect.wav");
    nLoadSoundFunc(SOUND_FRAGMENT_IMPACT,      "fragment_impact.wav");
    nLoadSoundFunc(SOUND_ITEM_01,              "item_01.wav");
    nLoadSoundFunc(SOUND_ITEM_02,              "item_02.wav");
    nLoadSoundFunc(SOUND_SUMMARY_TEXT,         "summary_text.wav");
    nLoadSoundFunc(SOUND_SUMMARY_SCORE,        "summary_score.wav");
    nLoadSoundFunc(SOUND_SUMMARY_MEDAL,        "summary_medal.wav");
    nLoadSoundFunc(SOUND_SUMMARY_PERFECT,      "summary_perfect.wav");
    nLoadSoundFunc(SOUND_SUMMARY_RECORD,       "summary_record.wav");
    nLoadSoundFunc(SOUND_UNLOCK,               "unlock.wav");
    nLoadSoundFunc(SOUND_CONTINUE_TICK,        "continue_tick.wav");
    nLoadSoundFunc(SOUND_CONTINUE_ACCEPT,      "continue_accept.wav");
    nLoadSoundFunc(SOUND_MENU_START,           "menu_start.wav");
    nLoadSoundFunc(SOUND_MENU_MSGBOX_SHOW,     "menu_msgbox_show.wav");
    nLoadSoundFunc(SOUND_MENU_MSGBOX_YES,      "menu_msgbox_yes.wav");
    nLoadSoundFunc(SOUND_MENU_MSGBOX_NO,       "menu_msgbox_no.wav");
    nLoadSoundFunc(SOUND_MENU_BUTTON_PRESS,    "menu_button_press.wav");
    nLoadSoundFunc(SOUND_MENU_SWITCH_ENABLED,  "menu_switch_enabled.wav");
    nLoadSoundFunc(SOUND_MENU_SWITCH_DISABLED, "menu_switch_disabled.wav");
    nLoadSoundFunc(SOUND_MENU_CHANGE_BUTTON,   "menu_change_button.wav");
    nLoadSoundFunc(SOUND_MENU_CHANGE_TAB,      "menu_change_tab.wav");
    nLoadSoundFunc(SOUND_MENU_CHANGE_LINE,     "menu_change_line.wav");
    nLoadSoundFunc(SOUND_MENU_SCROLL,          "menu_scroll.wav");
    nLoadSoundFunc(SOUND_MENU_SUB_IN,          "menu_sub_in.wav");
    nLoadSoundFunc(SOUND_MENU_SUB_OUT,         "menu_sub_out.wav");
    nLoadSoundFunc(SOUND_EFFECT_BEEP,          "effect_beep.wav");
    nLoadSoundFunc(SOUND_EFFECT_CLICK,         "effect_click.wav");
    nLoadSoundFunc(SOUND_EFFECT_DUST,          "effect_dust.wav");
    nLoadSoundFunc(SOUND_EFFECT_ERROR,         "effect_error.wav");
    nLoadSoundFunc(SOUND_EFFECT_FIRE,          "effect_fire.wav");
    nLoadSoundFunc(SOUND_EFFECT_FLY,           "effect_fly.wav");
    nLoadSoundFunc(SOUND_EFFECT_PEARL,         "effect_pearl.wav");
    nLoadSoundFunc(SOUND_EFFECT_SHAKE_01,      "effect_shake_01.wav");
    nLoadSoundFunc(SOUND_EFFECT_SHAKE_02,      "effect_shake_02.wav");
    nLoadSoundFunc(SOUND_EFFECT_SUCCESS,       "effect_success.wav");
    nLoadSoundFunc(SOUND_EFFECT_SWIPE_01,      "effect_swipe_01.wav");
    nLoadSoundFunc(SOUND_EFFECT_SWIPE_02,      "effect_swipe_02.wav");
    nLoadSoundFunc(SOUND_EFFECT_SWIPE_03,      "effect_swipe_03.wav");
    nLoadSoundFunc(SOUND_EFFECT_WOOSH_01,      "effect_woosh_01.wav");
    nLoadSoundFunc(SOUND_EFFECT_WOOSH_02,      "effect_woosh_02.wav");

    // 
    m_ShakeTimer.Play(CORE_TIMER_PLAY_RESET);

    // 
    m_ShakeTimer.SetTimeID(0);

    // 
    for(coreUintW i = 0u; i < SPECIAL_ICONS; ++i)
        m_aIcon[i].SetIndex(i);
    
    
    m_aSoundData.resize(SOUND_MAX);
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
            m_aParticleColor[0].Render();
            m_aParticleDark [0].Render();
            m_aParticleSmoke[0].Render();
            m_aParticleDot  [0].Render();

            // enable additive blending (keep alpha aggregation)
            if(bForeground) glBlendFuncSeparate(FOREGROUND_BLEND_SUM, FOREGROUND_BLEND_ALPHA);
                       else glBlendFunc        (FOREGROUND_BLEND_SUM);
            {
                // render fire particle system
                m_aParticleFire[0].Render();

                // render lightning sprites
                m_LightningList.Render();
            }
            if(bForeground) glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);
                       else glBlendFunc        (FOREGROUND_BLEND_DEFAULT);

            // 
            if(!g_bTiltMode) m_GustList.Render();

            // render all blast objects
            for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
            {
                if(m_aBlast[i].GetAlpha()) m_aBlast[i].Render();
            }

            // render all explosion objects
            for(coreUintW i = 0u; i < SPECIAL_EXPLOSION; ++i)
            {
                if(m_aExplosionBody[i].GetAlpha()) m_aExplosionBody[i].Render();
            }
            for(coreUintW i = 0u; i < SPECIAL_EXPLOSION; ++i)
            {
                //if(m_aExplosionWave[i].GetAlpha()) m_aExplosionWave[i].Render();
            }
        }
        glEnable(GL_DEPTH_TEST);
    }
}


void cSpecialEffects::RenderBottom()
{
    if(m_bActive)
    {
        //glDisable(GL_DEPTH_TEST);   // cannot be used in tilt mode, particles will overlap player
        glDepthMask(false);
        {
            const coreBool bForeground = g_pForeground->IsTarget();

            // render particle systems
            m_aParticleColor[1].Render();
            m_aParticleDark [1].Render();
            m_aParticleSmoke[1].Render();
            m_aParticleDot  [1].Render();

            // enable additive blending (keep alpha aggregation)
            if(bForeground) glBlendFuncSeparate(FOREGROUND_BLEND_SUM, FOREGROUND_BLEND_ALPHA);
                       else glBlendFunc        (FOREGROUND_BLEND_SUM);
            {
                // render fire particle system
                m_aParticleFire[1].Render();
            }
            if(bForeground) glBlendFuncSeparate(FOREGROUND_BLEND_DEFAULT, FOREGROUND_BLEND_ALPHA);
                       else glBlendFunc        (FOREGROUND_BLEND_DEFAULT);
                       
                       
            if(g_bTiltMode) m_GustList.Render();
        }
        //glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
    }
}


// ****************************************************************
// move special-effects
void cSpecialEffects::Move()
{
    // 
    m_bActive = m_aParticleColor[0].GetNumActiveParticles() || m_aParticleColor[1].GetNumActiveParticles() ||
                m_aParticleDark [0].GetNumActiveParticles() || m_aParticleDark [1].GetNumActiveParticles() ||
                m_aParticleSmoke[0].GetNumActiveParticles() || m_aParticleSmoke[1].GetNumActiveParticles() ||
                m_aParticleFire [0].GetNumActiveParticles() || m_aParticleFire [1].GetNumActiveParticles() ||
                m_aParticleDot  [0].GetNumActiveParticles() || m_aParticleDot  [1].GetNumActiveParticles() ||
                !m_LightningList.List()->empty()            ||
                !m_GustList     .List()->empty()            ||
                std::any_of(m_aBlast, m_aBlast + SPECIAL_BLASTS, [](const coreObject3D& oBlast) {return oBlast.GetAlpha() ? true : false;}) ||
                std::any_of(m_aExplosionBody, m_aExplosionBody + SPECIAL_EXPLOSION, [](const coreObject3D& oExplosion) {return oExplosion.GetAlpha() ? true : false;});
    if(m_bActive)
    {
        // move particle systems
        for(coreUintW i = 0u; i < 2u; ++i)
        {
            m_aParticleColor[i].Move();
            m_aParticleDark [i].Move();
            m_aParticleSmoke[i].Move();
            m_aParticleFire [i].Move();
            m_aParticleDot  [i].Move();
        }

        // 
        FOR_EACH_DYN(it, *m_LightningList.List())
        {
            coreObject3D* pLightning = (*it);

            // 
            const coreObject3D* pOwner = m_apLightningOwner[pLightning - m_aLightning];
            if(pOwner) pLightning->SetPosition(pOwner->GetPosition());

            // 
            pLightning->SetAlpha(pLightning->GetAlpha() - 4.0f * TIME);

            // 
            if(pLightning->GetAlpha() > 0.0f)
                 DYN_KEEP  (it, *m_LightningList.List())
            else DYN_REMOVE(it, *m_LightningList.List())
        }
        m_LightningList.MoveNormal();
        
        const coreVector2 vGustDir = coreVector2::Direction(m_fGustAngle);

        // 
        FOR_EACH_DYN(it, *m_GustList.List())
        {
            coreObject3D* pGust = (*it);

            const coreUintW iIndex = pGust - m_aGust;
            coreFlow& fTime = m_afGustTime[iIndex];
            
            if(g_bTiltMode)
            {
                fTime.Update(1.5f);
            
                const coreVector2 vRadialDir = coreVector2::Direction(m_afGustSide[iIndex] * (2.0f*PI));
                
                const coreVector2 vPos   = Core::Graphics->GetCamPosition().xy() + MapToAxis(coreVector2(0.0f, LERP(-1.0f, -1.5f, 0.5f + 0.5f * SIN(m_afGustSide[iIndex] * (15.0f*PI)))) * FOREGROUND_AREA, vRadialDir);
                const coreFloat   fScale = 7.0f * (1.0f + 5.0f * SIN(fTime * (1.0f*PI)));
                
                pGust->SetPosition (coreVector3(vPos, LERPS(-1.2f, 1.2f, fTime) * 50.0f));
                pGust->SetSize     (coreVector3(1.0f, fScale, 1.0f));
                pGust->SetDirection(coreVector3(0.0f,0.0f, 1.0f));
                pGust->SetOrientation(coreVector3(vRadialDir, 0.0f));
                pGust->SetAlpha    (1.0f);
            }
            else
            {
                fTime.Update(2.5f);
                
                const coreVector2 vPos   = MapToAxis(coreVector2(m_afGustSide[iIndex], LERPS(1.2f, -1.2f, fTime) * FOREGROUND_AREA.y), vGustDir);
                const coreFloat   fScale = 7.0f * (1.0f + 5.0f * SIN(fTime * (1.0f*PI)));
                
                pGust->SetPosition (coreVector3(vPos, 0.0f));
                pGust->SetSize     (coreVector3(1.0f, fScale, 1.0f));
                pGust->SetDirection(coreVector3(vGustDir, 0.0f));
                pGust->SetAlpha    (1.0f);
            }

            // 
            if(fTime < 1.0f)
                 DYN_KEEP  (it, *m_GustList.List())
            else DYN_REMOVE(it, *m_GustList.List())
        }
        m_GustList.MoveNormal();

        // loop through all blast objects
        for(coreUintW i = 0u; i < SPECIAL_BLASTS; ++i)
        {
            coreObject3D& oBlast = m_aBlast[i];
            if(!oBlast.GetAlpha()) continue;

            // 
            const coreFloat fScale = oBlast.GetCollisionModifier().x;
            const coreFloat fSpeed = oBlast.GetCollisionModifier().y;

            // 
            oBlast.SetAlpha    (MAX0(oBlast.GetAlpha() - fSpeed * TIME));
            oBlast.SetSize     (coreVector3(8.0f,8.0f,8.0f) * (fScale * (1.0f - oBlast.GetAlpha())));
            oBlast.SetTexOffset(coreVector2(0.0f,0.1f) * oBlast.GetAlpha());
            oBlast.Move();
        }
        
        
        

        // 
        for(coreUintW i = 0u; i < SPECIAL_EXPLOSION; ++i)
        {
            coreObject3D& oExplosion = m_aExplosionBody[i];
            if(!oExplosion.GetAlpha()) continue;

            // 
            const coreFloat fScale = oExplosion.GetCollisionModifier().x;
            const coreFloat fSpeed = oExplosion.GetCollisionModifier().y;

            // 
            oExplosion.SetAlpha(MAX0(oExplosion.GetAlpha() - fSpeed * (2.3f) * TIME));
            oExplosion.SetSize (coreVector3(0.25f,0.25f,0.1f) * (fScale * 40.0f * 10.0f * SIN((0.5f*PI) * MAX0((1.0f - oExplosion.GetAlpha())))));
            oExplosion.Move();
        }
        

        // 
        for(coreUintW i = 0u; i < SPECIAL_EXPLOSION; ++i)
        {
            coreObject3D& oExplosion = m_aExplosionWave[i];
            if(!oExplosion.GetAlpha()) continue;

            // 
            const coreFloat fScale = oExplosion.GetCollisionModifier().x;
            const coreFloat fSpeed = oExplosion.GetCollisionModifier().y;

            // 
            oExplosion.SetAlpha(MAX0(oExplosion.GetAlpha() - fSpeed * (2.2f) * TIME));
            oExplosion.SetSize (coreVector3(0.2f,0.2f,0.2f) * (fScale * 50.0f * SIN((0.6f*PI) * (1.0f - oExplosion.GetAlpha()))));
            oExplosion.Move();
        }
    }

    // 
    if(m_fShakeStrength && m_ShakeTimer.Update(1.0f))
    {
        // update shake animation
        m_fShakeStrength = MAX0(m_fShakeStrength - 0.07f);   // # without delta-time
        m_iShakeCount    = m_iShakeCount + 1u;

        // 
        coreVector2 vBase;
        if(m_iShakeType)
        {
            // 
            vBase = MapToAxisInv(coreVector2::Direction(I_TO_F(m_iShakeCount) * GA), g_vHudDirection);
        }
        else
        {
            // 
            vBase = g_vHudDirection.InvertedX() * ((m_iShakeCount & 0x01u) ? 1.0f : -1.0f);
        }

        // 
        const coreFloat fPower = m_fShakeStrength * m_fShakeOverride * I_TO_F(g_CurConfig.Graphics.iShake) * 0.00007f;
        g_pPostProcessing->SetPosition((vBase * fPower * g_vGameResolution).Processed(ROUND) / g_vGameResolution);

        // 
        if(!m_fShakeStrength)
        {
            m_iShakeCount = 0u;
            m_iShakeType  = 0u;
        }
    }

    // 
    m_fShakeOverride = 1.0f;
}

void cSpecialEffects::MoveAlways()
{
    // reset sound-guard
    std::memset(m_aiSoundGuard, 0, sizeof(m_aiSoundGuard));
    STATIC_ASSERT(SOUND_MAX <= sizeof(m_aiSoundGuard)*8u)

    // 
    for(coreUintW i = 0u; i < SPECIAL_PLAYERS; ++i)
    {
        m_afRumbleTime[i].UpdateMax(-1.0f, 0.0f);
        if(!m_afRumbleTime[i]) m_afRumbleStrength[i] = 0.0f;
    }
}


// ****************************************************************
// 
void cSpecialEffects::Update()
{
    // 
    const coreUintW iNum = (STATIC_ISVALID(g_pGame) && g_pGame->IsMulti()) ? SPECIAL_ICONS : 1u;

    // 
    for(coreUintW i = 0u; i < iNum; ++i)
        m_aIcon[i].Update();
}


// ****************************************************************
// create centered particle splash
void cSpecialEffects::CreateSplashColor(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor, const coreBool bDeep, const coreBool bLock, const coreFloat fSize, const coreFloat fSpeed)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 0u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(bLock);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleColor[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), bDeep ? Core::Rand->Float(-1.0f,1.0f) : 0.0f).Normalized();

        pParticle->SetPositionRel(vPosition + vDir,           vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.5f * fSize,               1.0f * fSize);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f) * fSpeed);
    });
}

void cSpecialEffects::CreateSplashDark(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreBool bDeep, const coreBool bLock)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 1u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(bLock);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleDark[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), bDeep ? Core::Rand->Float(-1.0f,1.0f) : 0.0f).Normalized();

        pParticle->SetPositionRel(vPosition + vDir,                 vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       PI);
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateSplashSmoke(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 2u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleSmoke[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir*0.0f,           vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.0f,                       12.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI*0.1f);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (0.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}

void cSpecialEffects::CreateSplashFire(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 3u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleFire[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir*0.0f,                 vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (4.5f,                             11.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       PI*0.5f);
        pParticle->SetColor4Abs  (coreVector4(vColor*0.926f, 1.0f), coreVector4(vColor*0.926f, 0.0f));
        pParticle->SetSpeed      (1.4f * Core::Rand->Float(0.9f, 1.1f));
    });
}

void cSpecialEffects::CreateSplashDot(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 4u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleDot[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir * (fScale ? 1.0f : 0.0f),           vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (4.5f,                       3.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// create directional particle blow
void cSpecialEffects::CreateBlowColor(const coreVector3 vPosition, const coreVector3 vDirection, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    ASSERT(vDirection.IsNormalized())

    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 0u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleColor[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,           vDir * (Core::Rand->Float(fScale) * 0.25f) + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (3.5f,                       1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateBlowDark(const coreVector3 vPosition, const coreVector3 vDirection, const coreFloat fScale, const coreUintW iNum)
{
    ASSERT(vDirection.IsNormalized())

    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 1u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleDark[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir,                 vDir * (Core::Rand->Float(fScale) * 0.25f) + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       PI);
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateBlowSmoke(const coreVector3 vPosition, const coreVector3 vDirection, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    ASSERT(vDirection.IsNormalized())

    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 2u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleSmoke[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir*0.0f,           vDir * (Core::Rand->Float(fScale) * 0.25f) + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (3.0f,                       12.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI*0.1f);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (0.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}

void cSpecialEffects::CreateBlowFire(const coreVector3 vPosition, const coreVector3 vDirection, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    ASSERT(vDirection.IsNormalized())

    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 3u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleFire[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionRel(vPosition + vDir*0.0f,           vDir * (Core::Rand->Float(fScale) * 0.25f) + vDirection * (Core::Rand->Float(fScale) * 0.75f));
        pParticle->SetScaleAbs   (4.5f,                        11.5f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI*0.5f);
        pParticle->SetColor4Abs  (coreVector4(vColor*0.926f, 1.0f),  coreVector4(vColor*0.926f, 0.0f));
        pParticle->SetSpeed      (1.4f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateChargeColor(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 0u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleColor[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionAbs(vPosition + vDir * Core::Rand->Float(fScale), vPosition + vDir);
        pParticle->SetScaleAbs   (3.5f,                                         1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),                   PI);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),                    coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateChargeDark(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 1u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleDark[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);

        pParticle->SetPositionAbs(vPosition + vDir * Core::Rand->Float(fScale), vPosition + vDir);
        pParticle->SetScaleAbs   (3.5f,                                         1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),                   PI);
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f),             coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateWhirlColor(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum, const coreVector3 vColor)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 0u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleColor[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);
        const coreFloat   fRand = Core::Rand->Float(fScale);

        pParticle->SetPositionAbs(vPosition + vDir * fRand,   vPosition + vDir * (fScale-fRand));
        pParticle->SetScaleAbs   (3.5f,                       1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}

void cSpecialEffects::CreateWhirlDark(const coreVector3 vPosition, const coreFloat fScale, const coreUintW iNum)
{
    // 
    const coreUintW iNum2 = this->__CalcParticleNum(iNum, 1u); if(!iNum2) return;
    const coreFloat fBase = this->__GetEffectBase(false);
    const coreFloat fStep = 2.0f*PI * RCP(I_TO_F(iNum2));

    // 
    coreUintW i = 0u;
    m_aParticleDark[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(iNum2, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vDir  = coreVector3(coreVector2::Direction(fBase + fStep * I_TO_F(i++)), 0.0f);
        const coreFloat   fRand = Core::Rand->Float(fScale);

        pParticle->SetPositionAbs(vPosition + vDir * fRand,         vPosition + vDir * (fScale-fRand));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       PI);
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.7f * Core::Rand->Float(0.9f, 1.1f));
    });
}


// ****************************************************************
// 
void cSpecialEffects::CreateBreakupColor(const coreObject3D* pObject, const coreFloat fScale, const coreUintW iStep, const coreVector3 vColor)
{
    ASSERT(pObject)

    if(!pObject->GetModel().IsUsable()) return;

    // 
    const coreModel*   pModel           = pObject->GetModel().GetResource();
    const coreVector3* pvVertexPosition = pModel->GetVertexPosition();
    //ASSERT(pModel->GetNumClusters())
    if(!pModel->GetNumClusters()) return;

    // 
    const coreVector3 vPosition = pObject->GetPosition();
    const coreVector3 vSize     = pObject->GetSize();
    const coreVector4 vRotation = pObject->GetRotation();
    const coreVector2 vCenter   = vRotation.QuatApply(pModel->GetWeightedCenter() * vSize).xy();
    const coreVector2 vSide     = this->__GetBreakupSide();
    
    const coreUintW iNum = this->__CalcParticleNum(MIN(500u / iStep, pModel->GetNumVertices()), 0u); if(!iNum) return;
    const coreFloat fStep = I_TO_F(pModel->GetNumVertices()) / I_TO_F(iNum);
    coreFloat fCurrent = 0.0f;
    
    ASSERT(fStep >= 1.0f)

    // 
    //coreUintW i = 0u;
    m_aParticleColor[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(/*pModel->GetNumVertices() / iStep*/iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vVertex = vRotation.QuatApply(pvVertexPosition[/*i*/F_TO_UI(fCurrent)] * vSize);
        const coreVector3 vDir    = coreVector3(vSide * SIGN(coreVector2::Dot(vVertex.xy() - vCenter, vSide)), 0.0f);
        //i += iStep;
        fCurrent += fStep;

        pParticle->SetPositionRel(vPosition + vVertex,        vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.5f,                       1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI), PI);
        pParticle->SetColor4Abs  (coreVector4(vColor, 1.0f),  coreVector4(vColor, 0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}

void cSpecialEffects::CreateBreakupDark(const coreObject3D* pObject, const coreFloat fScale, const coreUintW iStep)
{
    ASSERT(pObject)

    if(!pObject->GetModel().IsUsable()) return;

    // 
    const coreModel*   pModel           = pObject->GetModel().GetResource();
    const coreVector3* pvVertexPosition = pModel->GetVertexPosition();
    //ASSERT(pModel->GetNumClusters())
    if(!pModel->GetNumClusters()) return;

    // 
    const coreVector3 vPosition = pObject->GetPosition();
    const coreVector3 vSize     = pObject->GetSize();
    const coreVector4 vRotation = pObject->GetRotation();
    const coreVector2 vCenter   = vRotation.QuatApply(pModel->GetWeightedCenter() * vSize).xy();
    const coreVector2 vSide     = this->__GetBreakupSide();
    
    const coreUintW iNum = this->__CalcParticleNum(MIN(500u / iStep, pModel->GetNumVertices()), 1u); if(!iNum) return;
    const coreFloat fStep = I_TO_F(pModel->GetNumVertices()) / I_TO_F(iNum);
    coreFloat fCurrent = 0.0f;
    
    ASSERT(fStep >= 1.0f)

    // 
    //coreUintW i = 0u;
    m_aParticleDark[SPECIAL_DEPTH(vPosition)].GetDefaultEffect()->CreateParticle(/*pModel->GetNumVertices() / iStep*/iNum, [&](coreParticle* OUTPUT pParticle)
    {
        const coreVector3 vVertex = vRotation.QuatApply(pvVertexPosition[/*i*/F_TO_UI(fCurrent)] * vSize);
        const coreVector3 vDir    = coreVector3(vSide * SIGN(coreVector2::Dot(vVertex.xy() - vCenter, vSide)), 0.0f);
        //i += iStep;
        fCurrent += fStep;

        pParticle->SetPositionRel(vPosition + vVertex,              vDir * Core::Rand->Float(fScale));
        pParticle->SetScaleAbs   (3.5f,                             1.0f);
        pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),       PI);
        pParticle->SetColor4Abs  (coreVector4(0.0f,0.0f,0.0f,1.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
        pParticle->SetSpeed      (1.5f * Core::Rand->Float(0.7f, 1.3f));
    });
}


// ****************************************************************
// 
coreFloat cSpecialEffects::CreateLightning(const coreVector2 vPosFrom, const coreVector2 vPosTo, const coreFloat fWidth, const coreVector3 vColor, const coreVector2 vTexSizeFactor, const coreFloat fTexOffset)
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
        fTexLen = RoundFactor(MAX(fTexLen, 0.5f), 2.0f);

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

void cSpecialEffects::CreateLightning(coreObject3D* pOwner, const coreVector2 vDirection, const coreFloat fLength, const coreFloat fWidth, const coreVector3 vColor, const coreVector2 vTexSizeFactor, const coreFloat fTexOffset)
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
void cSpecialEffects::CreateGust(const coreFloat fFrequency, const coreFloat fAngle)
{
    // 
    m_fGustSpawn.Update(10.0f * fFrequency);
    if(m_fGustSpawn >= 1.0f)
    {
        ASSERT(m_fGustSpawn < 2.0f)
        m_fGustSpawn -= 1.0f;

        // 
        if(++m_iCurGust >= SPECIAL_GUSTS) m_iCurGust = 0u;
        coreObject3D& oGust = m_aGust[m_iCurGust];

        m_afGustTime[m_iCurGust] = 0.0f;

        // 
        const coreVector2 vPos = coreVector2(Core::Rand->Float(-1.0f, 1.0f), 100.0f) * FOREGROUND_AREA;

        m_afGustSide[m_iCurGust] = vPos.x;

        // 
        oGust.SetPosition(coreVector3(vPos, 0.0f));
        oGust.SetAlpha   (0.0f);

        // 
        WARN_IF(m_GustList.List()->count(&oGust)) {}
           else m_GustList.BindObject(&oGust);
    }

    // 
    m_fGustAngle = fAngle;
}


// ****************************************************************
// 
void cSpecialEffects::CreateBlastSphere(const coreVector3 vPosition, const coreFloat fScale, const coreFloat fSpeed, const coreVector3 vColor)
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
    oBlast.SetCollisionModifier(coreVector3(fScale, fSpeed, 0.0f));
    oBlast.SetColor4           (coreVector4(vColor * 0.65f, 1.0f));
    oBlast.SetTexSize          (coreVector2(12.0f,12.0f));
}

void cSpecialEffects::CreateBlastCube(const coreVector3 vPosition, const coreVector2 vDirection, const coreFloat fScale, const coreFloat fSpeed, const coreVector3 vColor)
{
    // 
    if(++m_iCurBlast >= SPECIAL_BLASTS) m_iCurBlast = 0u;
    coreObject3D& oBlast = m_aBlast[m_iCurBlast];

    ASSERT(!oBlast.GetAlpha())

    // 
    oBlast.DefineModel         (m_apBlastModel[1]);
    oBlast.SetPosition         (vPosition);
    oBlast.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oBlast.SetDirection        (coreVector3(vDirection, 0.0f));
    oBlast.SetCollisionModifier(coreVector3(fScale * 0.77f, fSpeed, 0.0f));
    oBlast.SetColor4           (coreVector4(vColor, 1.0f));
    oBlast.SetTexSize          (coreVector2(3.0f,1.2f));
}

void cSpecialEffects::CreateBlastTetra(const coreVector3 vPosition, const coreVector2 vDirection, const coreFloat fScale, const coreFloat fSpeed, const coreVector3 vColor)
{
    // 
    if(++m_iCurBlast >= SPECIAL_BLASTS) m_iCurBlast = 0u;
    coreObject3D& oBlast = m_aBlast[m_iCurBlast];

    ASSERT(!oBlast.GetAlpha())

    // 
    oBlast.DefineModel         (m_apBlastModel[2]);
    oBlast.SetPosition         (vPosition);
    oBlast.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oBlast.SetDirection        (coreVector3(vDirection, 0.0f));
    oBlast.SetCollisionModifier(coreVector3(fScale * 0.69f, fSpeed, 0.0f));
    oBlast.SetColor4           (coreVector4(vColor, 1.0f));
    oBlast.SetTexSize          (coreVector2(3.0f,1.2f));
}


// ****************************************************************
// 
void cSpecialEffects::CreateExplosion(const coreVector3 vPosition)
{
    // 
    if(++m_iCurExplosion >= SPECIAL_EXPLOSION) m_iCurExplosion = 0u;
    coreObject3D& oExplosion = m_aExplosionBody[m_iCurExplosion];

    ASSERT(!oExplosion.GetAlpha())
    
    const coreFloat fScale = 1.0f;
    const coreFloat fSpeed = 1.0f;
    const coreVector3 vColor = coreVector3(1.0f, 0.3f, 0.1f);

    // 
    //oExplosion.DefineModel         (m_apBlastModel[0]);
    oExplosion.SetPosition         (vPosition);
    oExplosion.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    //oExplosion.SetDirection        (coreVector3(coreVector2::Rand(), 0.0f));
    oExplosion.SetCollisionModifier(coreVector3(fScale, fSpeed, 0.0f));
    //oExplosion.SetColor4           (coreVector4(vColor, 1.4f));
    oExplosion.SetColor4           (coreVector4(vColor, 1.0f));
    //oExplosion.SetTexSize          (coreVector2(12.0f,12.0f));
    
    

    coreObject3D& oExplosion2 = m_aExplosionWave[m_iCurExplosion];

    // 
    oExplosion2.SetPosition         (vPosition);
    oExplosion2.SetSize             (coreVector3(0.0f,0.0f,0.0f));
    oExplosion2.SetCollisionModifier(coreVector3(fScale, fSpeed, 0.0f));
    oExplosion2.SetColor4           (coreVector4(vColor, 1.0f));
    
    
    
    this->ShakeScreen(SPECIAL_SHAKE_BIG);
                
    this->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 2000u);
}


// ****************************************************************
// 
void cSpecialEffects::PlaySound(const coreVector3 vPosition, const coreFloat fVolume, const coreFloat fPitch, const eSoundEffect eSoundIndex)
{
    ASSERT(fVolume > 0.0f)

    // 
    if(!m_apSound[eSoundIndex].IsUsable()) return;
    coreSound* pSound = m_apSound[eSoundIndex].GetResource();

    // 
    coreFloat fBaseVolume, fBasePitch, fBasePitchRnd;
    coreBool  bRelative;
    coreUint8 iType;
    switch(eSoundIndex)
    {
    default: UNREACHABLE
    case SOUND_PLAYER_EXPLOSION:     fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_01:   fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_02:   fBaseVolume = 1.3f; fBasePitch = 0.7f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_03:   fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_04:   fBaseVolume = 1.4f; fBasePitch = 1.2f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_05:   fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_06:   fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_07:   fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_08:   fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_09:   fBaseVolume = 1.4f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_10:   fBaseVolume = 1.5f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_11:   fBaseVolume = 3.0f; fBasePitch = 0.4f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ENEMY_EXPLOSION_12:   fBaseVolume = 0.6f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_WEAPON_RAY:           fBaseVolume = 0.8f; fBasePitch = 0.4f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_WEAPON_ENEMY:         fBaseVolume = 1.5f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_BULLET_HIT:           fBaseVolume = 1.5f; fBasePitch = 0.9f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_BULLET_REFLECT:       fBaseVolume = 1.1f; fBasePitch = 1.0f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_BULLET_VANISH:        fBaseVolume = 1.0f; fBasePitch = 0.8f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_SHIELD_HIT:           fBaseVolume = 1.5f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_SHIELD_DESTROY:       fBaseVolume = 1.5f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_MEDAL_BRONZE:         fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MEDAL_SILVER:         fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MEDAL_GOLD:           fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MEDAL_PLATINUM:       fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MEDAL_DARK:           fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_BADGE:                fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_HELPER:               fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_FRAGMENT_COLLECT:     fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_FRAGMENT_IMPACT:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_EFFECT; break;
    case SOUND_ITEM_01:              fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_ITEM_02:              fBaseVolume = 0.9f; fBasePitch = 0.9f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_SUMMARY_TEXT:         fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_SUMMARY_SCORE:        fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_SUMMARY_MEDAL:        fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_SUMMARY_PERFECT:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_SUMMARY_RECORD:       fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_UNLOCK:               fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_CONTINUE_TICK:        fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_CONTINUE_ACCEPT:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_START:           fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_MSGBOX_SHOW:     fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_MSGBOX_YES:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_MSGBOX_NO:       fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_BUTTON_PRESS:    fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_SWITCH_ENABLED:  fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_SWITCH_DISABLED: fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_CHANGE_BUTTON:   fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_CHANGE_TAB:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_CHANGE_LINE:     fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_SCROLL:          fBaseVolume = 0.8f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_SUB_IN:          fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_MENU_SUB_OUT:         fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = true;  iType = SOUND_MENU;   break;
    case SOUND_EFFECT_BEEP:          fBaseVolume = 2.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_CLICK:         fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.01f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_DUST:          fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_ERROR:         fBaseVolume = 1.0f; fBasePitch = 0.9f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_FIRE:          fBaseVolume = 1.0f; fBasePitch = 0.7f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_FLY:           fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_PEARL:         fBaseVolume = 1.1f; fBasePitch = 1.2f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_SHAKE_01:      fBaseVolume = 3.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_SHAKE_02:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_SUCCESS:       fBaseVolume = 3.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.0f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_SWIPE_01:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_SWIPE_02:      fBaseVolume = 1.0f; fBasePitch = 1.1f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_SWIPE_03:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_WOOSH_01:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.1f;  bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_EFFECT_WOOSH_02:      fBaseVolume = 1.0f; fBasePitch = 1.0f; fBasePitchRnd = 0.05f; bRelative = false; iType = SOUND_EFFECT; break;
    case SOUND_PLACEHOLDER:          return;   // #
    case SOUND_NONE:                 return;   // #
    }

    // 
    if(!g_CurConfig.Audio.i3DSound) bRelative = true;

    
    cSoundData& oData = m_aSoundData[eSoundIndex];

    // 
    if(HAS_BIT_EX(m_aiSoundGuard, eSoundIndex))
    {
        if(!bRelative)
        {
            // 
            oData.vPosition = (oData.vPosition * I_TO_F(oData.iCount) + vPosition) / I_TO_F(oData.iCount + 1u);
            oData.iCount    = (oData.iCount + 1u);

            // 
            if(pSound->EnableRef(this)) pSound->SetSource(oData.vPosition);
        }
        return;
    }
    ADD_BIT_EX(m_aiSoundGuard, eSoundIndex)

    // 
    oData.vPosition = vPosition;
    oData.iCount    = 1u;

    // 
    fBaseVolume *= (1.0f + Core::Rand->Float(-0.3f,          0.0f));
    fBasePitch  *= (1.0f + Core::Rand->Float(-fBasePitchRnd, fBasePitchRnd));

    // 
    if(pSound->EnableRef(this)) pSound->Stop();

    // 
    if(bRelative) pSound->PlayRelative(this, fVolume * fBaseVolume, fPitch * fBasePitch, false, iType);
             else pSound->PlayPosition(this, fVolume * fBaseVolume, fPitch * fBasePitch, false, iType, vPosition * 0.1f);
}


// ****************************************************************
// 
void cSpecialEffects::StopSound(const eSoundEffect eSoundIndex)
{
    // 
    if(!m_apSound[eSoundIndex].IsUsable()) return;
    coreSound* pSound = m_apSound[eSoundIndex].GetResource();

    // 
    if(pSound->EnableRef(this)) pSound->Stop();
}


// ****************************************************************
// 
void cSpecialEffects::ExternPlayPosition(const coreSoundPtr& pSound, const void* pRef, const coreFloat fVolume, const coreFloat fPitch, const coreBool bLoop, const coreUint8 iType, const coreVector3 vPosition)
{
    // 
    const coreVector3 vReal = g_CurConfig.Audio.i3DSound ? (vPosition * 0.1f) : Core::Audio->GetListenerPosition();

    // 
    pSound->PlayPosition(pRef, fVolume, fPitch, bLoop, iType, vReal);
}


// ****************************************************************
// 
void cSpecialEffects::ExternSetSource(const coreSoundPtr& pSound, const coreVector3 vPosition)
{
    // 
    const coreVector3 vReal = g_CurConfig.Audio.i3DSound ? (vPosition * 0.1f) : Core::Audio->GetListenerPosition();

    // 
    pSound->SetSource(vReal);
}


// ****************************************************************
// 
void cSpecialEffects::RumblePlayer(const cPlayer* pPlayer, const coreFloat fStrength, const coreUint32 iLengthMs)
{
    ASSERT(fStrength && iLengthMs)
    if(!STATIC_ISVALID(g_pGame)) return;

    // loop through all active players
    g_pGame->ForEachPlayerAll([&](const cPlayer* pCurPlayer, const coreUintW i)
    {
        if((pPlayer != pCurPlayer) && (pPlayer != NULL)) return;
        if(pCurPlayer->HasStatus(PLAYER_STATUS_DEAD) && !pCurPlayer->ReachedDeath()) return;
        
        if(fStrength < m_afRumbleStrength[i]) return;
        if(fStrength == m_afRumbleStrength[i] && I_TO_F(iLengthMs) * 0.001f < m_afRumbleTime[i]) return;

        const coreUint8   iRumble     = g_CurConfig.Input.aiRumble[i];
        const coreUint8   iType       = g_pGame->IsMulti() ? g_CurConfig.Input.aiType[i] : g_iTotalType;
        const coreUintW   iJoystickID = iType - INPUT_SETS_KEYBOARD;
        const sGameInput* pCurInput   = pCurPlayer->GetInput();

        // check for valid configuration
        if(iRumble && (iJoystickID < Core::Input->GetJoystickNum()))
        {
            // check for valid input set
            if((pCurInput == &g_TotalInput) || (P_TO_UI(pCurInput - g_aGameInput) < INPUT_SETS))   // # skip replay
            {
                // create rumble effect
                const coreFloat fFinal = CLAMP01(fStrength * I_TO_F(iRumble) * 0.1f * (DEFINED(_CORE_SWITCH_) ? 0.35f : 1.0f));
                Core::Input->JoystickRumble(iJoystickID, fFinal, fFinal, iLengthMs);

                // 
                m_afRumbleTime    [i] = I_TO_F(iLengthMs) * 0.001f;
                m_afRumbleStrength[i] = fStrength;
            }
        }
    });
}


// ****************************************************************
// 
void cSpecialEffects::ShakeScreen(const coreFloat fStrength)
{
    // 
    m_fShakeStrength = MAX(m_fShakeStrength, fStrength);

    // 
    if(m_fShakeStrength >= SPECIAL_SHAKE_BIG) m_iShakeType = 1u;
}


// ****************************************************************
// 
void cSpecialEffects::FreezeScreen(const coreFloat fTime)
{
    m_fFreezeTime = MAX(m_fFreezeTime, fTime);
}


// ****************************************************************
// 
void cSpecialEffects::SlowScreen(const coreFloat fTime)
{
    m_fSlowTime = MAX(m_fSlowTime, fTime);
}


// ****************************************************************
// 
void cSpecialEffects::ClearAll()
{
    // 
    for(coreUintW i = 0u; i < 2u; ++i)
    {
        m_aParticleColor[i].ClearAll();
        m_aParticleDark [i].ClearAll();
        m_aParticleSmoke[i].ClearAll();
        m_aParticleFire [i].ClearAll();
        m_aParticleDot  [i].ClearAll();
    }

    // 
    m_LightningList.Clear();
    m_GustList     .Clear();
    for(coreUintW i = 0u; i < SPECIAL_BLASTS;    ++i) m_aBlast        [i].SetAlpha(0.0f);
    for(coreUintW i = 0u; i < SPECIAL_EXPLOSION; ++i) m_aExplosionBody[i].SetAlpha(0.0f);
    m_fShakeStrength = 0.0f;
    m_iShakeCount    = 0u;
    m_iShakeType     = 0u;
    m_fFreezeTime    = 0.0f;
    m_fSlowTime      = 0.0f;

    // 
    g_pPostProcessing->SetPosition(coreVector2(0.0f,0.0f));
}


// ****************************************************************
// 
void cSpecialEffects::MacroExplosionColorSmall(const coreVector3 vPosition, const coreVector3 vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_SMALL, vColor);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_SMALL,  LERP(coreVector3(1.0f,1.0f,1.0f), vColor, 0.75f));
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionColorBig(const coreVector3 vPosition, const coreVector3 vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_BIG, vColor);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_BIG,  LERP(coreVector3(1.0f,1.0f,1.0f), vColor, 0.75f));
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionDarkSmall(const coreVector3 vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_SMALL);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_SMALL, COLOR_ENERGY_WHITE);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionDarkBig(const coreVector3 vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_BIG);
    this         ->CreateBlastSphere(vPosition, SPECIAL_BLAST_BIG, COLOR_ENERGY_WHITE);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionPhysicalColorSmall(const coreVector3 vPosition, const coreVector3 vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_SMALL,    vColor);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_SMALL, vColor);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionPhysicalColorBig(const coreVector3 vPosition, const coreVector3 vColor)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashColor(vPosition, SPECIAL_SPLASH_BIG,    vColor);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_BIG, vColor);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroExplosionPhysicalDarkSmall(const coreVector3 vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_SMALL);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_SMALL);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_SMALL, COLOR_FIRE_WHITE);
    this         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);
}

void cSpecialEffects::MacroExplosionPhysicalDarkBig(const coreVector3 vPosition)
{
    // 
    g_pDistortion->CreateWave       (vPosition, DISTORTION_WAVE_BIG);
    this         ->CreateSplashDark (vPosition, SPECIAL_SPLASH_BIG);
    this         ->CreateSplashFire (vPosition, SPECIAL_EXPLOSION_BIG, COLOR_FIRE_WHITE);
    this         ->ShakeScreen      (SPECIAL_SHAKE_BIG);
}

void cSpecialEffects::MacroEruptionColorSmall(const coreVector3 vPosition, const coreVector2 vDirection, const coreVector3 vColor)
{
    // 
    g_pDistortion->CreateBurst    (vPosition,             vDirection,        DISTORTION_BURST_SMALL);
    this         ->CreateBlowColor(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_SMALL, vColor);
}

void cSpecialEffects::MacroEruptionColorBig(const coreVector3 vPosition, const coreVector2 vDirection, const coreVector3 vColor)
{
    // 
    g_pDistortion->CreateBurst    (vPosition,             vDirection,        DISTORTION_BURST_BIG);
    this         ->CreateBlowColor(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_BIG, vColor);
}

void cSpecialEffects::MacroEruptionDarkSmall(const coreVector3 vPosition, const coreVector2 vDirection)
{
    // 
    g_pDistortion->CreateBurst   (vPosition,             vDirection,        DISTORTION_BURST_SMALL);
    this         ->CreateBlowDark(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_SMALL);
}

void cSpecialEffects::MacroEruptionDarkBig(const coreVector3 vPosition, const coreVector2 vDirection)
{
    // 
    g_pDistortion->CreateBurst   (vPosition,             vDirection,        DISTORTION_BURST_BIG);
    this         ->CreateBlowDark(vPosition, coreVector3(vDirection, 0.0f), SPECIAL_BLOW_BIG);
}


// ****************************************************************
// 
void cSpecialEffects::MacroDestructionColor(const coreObject3D* pObject, const coreVector3 vColor)
{
    ASSERT(pObject)

    // 
    const coreVector3 vPosition = pObject->GetPosition();
    const coreFloat   fPower    = SQRT(MAX(pObject->GetVisualRadius() / 2.5f - 0.47f, 1.0f));

    // 
    g_pDistortion->CreateWave        (vPosition, 2.5f * fPower, 3.0f);
    this         ->CreateBreakupColor(pObject,   52.0f,         F_TO_UI(32.0f * RCP(fPower)), vColor);
    this         ->CreateSplashFire  (vPosition, 5.0f * fPower, F_TO_UI(7.0f  *    (fPower)), vColor);
    this         ->ShakeScreen       (SPECIAL_SHAKE_TINY * fPower);
}

void cSpecialEffects::MacroDestructionDark(const coreObject3D* pObject)
{
    ASSERT(pObject)

    // 
    const coreVector3 vPosition = pObject->GetPosition();
    const coreFloat   fPower    = SQRT(MAX(pObject->GetVisualRadius() / 2.5f - 0.47f, 1.0f));

    // (# more particles than color) 
    g_pDistortion->CreateWave       (vPosition, 2.5f * fPower, 3.0f);
    this         ->CreateBreakupDark(pObject,   52.0f,         F_TO_UI(30.0f * RCP(fPower)));
    this         ->CreateSplashFire (vPosition, 5.0f * fPower, F_TO_UI(14.0f *    (fPower)), COLOR_FIRE_WHITE);
    this         ->ShakeScreen      (SPECIAL_SHAKE_TINY * fPower);
}


// ****************************************************************
// 
coreFloat cSpecialEffects::__GetEffectBase(const coreBool bLock)
{
    // 
    if(!bLock || (m_iEffectFrame != Core::System->GetCurFrame()))
    {
        m_iEffectFrame = Core::System->GetCurFrame();
        m_iEffectCount = (m_iEffectCount + 1u) % 144u;  // loop at (110.00621124003 * PI)
    }
    return GA * I_TO_F(m_iEffectCount);
}


// ****************************************************************
// 
coreVector2 cSpecialEffects::__GetBreakupSide()
{
    // 
    m_iBreakupCount = (m_iBreakupCount + 3u) % 8u;
    return coreVector2::Direction((0.25f*PI) * (I_TO_F(m_iBreakupCount) + 0.5f));
}


// ****************************************************************
// 
coreUintW cSpecialEffects::__CalcParticleNum(const coreUintW iValue, const coreUintW iIndex)
{
    ASSERT(iIndex < ARRAY_SIZE(m_aiParticleFract))
    coreUint8& iFract = m_aiParticleFract[iIndex];

    const coreUintW iTotal = iFract + iValue * g_CurConfig.Graphics.iParticle;

    iFract = iTotal % 100u;
    return iTotal / 100u;
}