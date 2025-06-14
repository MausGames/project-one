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
cWater::cWater(const coreHashString& sSkyTexture)noexcept
: m_fAnimation (0.0f)
, m_fFlyOffset (0.0f)
{
    const coreVector2 vWaterResolution = g_vGameResolution * WATER_SCALE_FACTOR * ENVIRONMENT_SCALE_FACTOR;

    if(sSkyTexture)
    {
        // create reflection frame buffer
        m_Reflection.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB8, CORE_TEXTURE_MODE_DEFAULT);
        m_Reflection.AttachTargetBuffer (CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16);
        m_Reflection.Create(vWaterResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }

    // create refraction frame buffer
    m_Refraction.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB8, CORE_TEXTURE_MODE_DEFAULT);
    m_Refraction.Create(g_vGameResolution * ENVIRONMENT_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    if(sSkyTexture)
    {
        if(CORE_GL_SUPPORT(ARB_depth_texture))
        {
            // create depth frame buffer
            m_Depth.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16, CORE_TEXTURE_MODE_DEFAULT);
            m_Depth.Create(vWaterResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
        }

        // create sky-plane object
        m_Sky.DefineTexture(0u, sSkyTexture);
        m_Sky.SetTexSize   (coreVector2(WATER_SKY_SIZE, WATER_SKY_SIZE));

        // 
        m_apSkyProgram[0] = Core::Manager::Resource->Get<coreProgram>("default_2d_program");
        m_apSkyProgram[1] = Core::Manager::Resource->Get<coreProgram>("environment_vignette_program");
    }

    // load object resources
    this->DefineModel  (Core::Manager::Object->GetLowTriangle());
    this->DefineTexture(0u, "environment_water_norm.png");
    this->DefineTexture(1u, m_Reflection.GetColorTarget(0u).pTexture);
    this->DefineTexture(2u, m_Refraction.GetColorTarget(0u).pTexture);
    this->DefineTexture(3u, m_Depth     .GetDepthTarget()  .pTexture);
    this->DefineProgram("environment_water_program");

    // 
    if(!m_Depth.GetIdentifier()) this->DefineTexture(3u, "fallback_depth.png");

    // set object properties
    this->SetSize(coreVector3(WATER_SIZE, WATER_SIZE, 1.0f));
}


// ****************************************************************
// destructor
cWater::~cWater()
{
    // explicitly undefine to detach textures
    this->Undefine();
}


// ****************************************************************
// render the water-surface
void cWater::Render(coreFrameBuffer* pBackground)
{
    if(!this->GetProgram().IsUsable()) return;

    // blit current background color into own refraction buffer
    pBackground->Blit(CORE_FRAMEBUFFER_TARGET_COLOR, &m_Refraction);

    // 
    this->__RenderOwn();

    if(this->GetProgram()->Enable())
    {
        // update all water uniforms
        this->GetProgram()->SendUniform("u_v1Time",   m_fAnimation);
        this->GetProgram()->SendUniform("u_v1Offset", m_fFlyOffset * -0.0125f);

        // render the 3d-object
        this->coreObject3D::Render();
    }

    // invalidate all frame buffer objects
    if(m_Reflection.GetIdentifier()) m_Reflection.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
    if(m_Refraction.GetIdentifier()) m_Refraction.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    if(m_Depth     .GetIdentifier()) m_Depth     .Invalidate(CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// move the water-surface
void cWater::Move()
{
    // update animation value
    m_fAnimation.UpdateMod(0.008f, 100.0f);

    // move water with camera (also water-level up and down)
    this->SetPosition(coreVector3(0.0f, m_fFlyOffset * OUTDOOR_DETAIL, WATER_HEIGHT + 0.4f * SIN(80.0f * m_fAnimation)));

    // 
    this->__MoveOwn();

    // move the 3d-object
    this->coreObject3D::Move();
}


// ****************************************************************
// update water reflection map
void cWater::UpdateReflection()
{
    if(!m_Reflection.GetIdentifier()) return;

    // 
    this->__UpdateOwn();

    // save current camera and light properties
    const coreVector3 vOldCamPos = Core::Graphics->GetCamPosition();
    const coreVector3 vOldCamOri = Core::Graphics->GetCamOrientation();
    const coreVector4 vOldLight  = Core::Graphics->GetLight(0u).vDirection;

    // flip camera upside-down and override light
    Core::Graphics->SetCamera(-CAMERA_POSITION + coreVector3(0.0f, 0.0f, WATER_HEIGHT*2.0f + g_pEnvironment->GetHeight()), -CAMERA_DIRECTION, CAMERA_ORIENTATION);
    Core::Graphics->SetLight (0u, coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(LIGHT_DIRECTION, 0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));

    // fill reflection frame buffer
    m_Reflection.StartDraw();
    if(DEFINED(_CORE_GLES_)) m_Reflection.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
    {
        // flip projection left-right (also culling!, after StartDraw())
        c_cast<coreMatrix4&>(Core::Graphics->GetPerspective())._11 *= -1.0f;

        glDepthFunc(GL_ALWAYS);   // better performance than clear
        glDisable  (GL_BLEND);
        {
            // move and render the sky-plane
            m_Sky.SetSize     (coreVector2(WATER_SCALE_FACTOR, WATER_SCALE_FACTOR) * SQRT2 * ENVIRONMENT_SCALE_FACTOR);
            m_Sky.SetDirection(vOldCamOri.xy());
            m_Sky.SetTexOffset(coreVector2(vOldCamPos.x * (0.008f * WATER_SKY_SIZE), m_fFlyOffset * (-0.05f * WATER_SKY_SIZE)));
            m_Sky.Move();
            m_Sky.Render(m_apSkyProgram[0]);
        }
        glDepthFunc(GL_LEQUAL);
        glEnable   (GL_BLEND);

        if(g_CurConfig.Graphics.iReflection && (STATIC_ISVALID(g_pGame) || g_pSpecialEffects->IsActive()))
        {
            // 
            cLodObject::AllowHigh(false);

            glCullFace(GL_FRONT);
            {
                if(STATIC_ISVALID(g_pGame))
                {
                    // render the game
                    g_pGame->Render();
                }
                else
                {
                    // render special-effects
                    g_pSpecialEffects->Render();
                }
            }
            glCullFace(GL_BACK);

            glDisable(GL_DEPTH_TEST);
            {
                // 
                m_Sky.Render(m_apSkyProgram[1]);
            }
            glEnable(GL_DEPTH_TEST);

            // 
            cLodObject::AllowHigh(true);
        }
    }

    // reset camera, light and projection
    Core::Graphics->SetCamera(vOldCamPos, CAMERA_DIRECTION, vOldCamOri);
    Core::Graphics->SetLight (0u, coreVector4(0.0f,0.0f,0.0f,0.0f), vOldLight, coreVector4(0.0f,0.0f,0.0f,0.0f));
    c_cast<coreMatrix4&>(Core::Graphics->GetPerspective())._11 *= -1.0f;
}


// ****************************************************************
// update water depth map
void cWater::UpdateDepth(cOutdoor* pOutdoor, const coreList<coreBatchList*>& apObjectList)
{
    if(!m_Depth.GetIdentifier()) return;

    if(pOutdoor && pOutdoor->IsEnabled(CORE_OBJECT_ENABLE_ALL))
    {
        // fill depth frame buffer
        m_Depth.StartDraw();
        if(DEFINED(_CORE_GLES_)) m_Depth.Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
        {
            glDepthFunc(GL_ALWAYS);   // better performance than clear
            {
                // render the outdoor-surface
                pOutdoor->RenderDepth();
            }
            glDepthFunc(GL_LEQUAL);

            // 
            FOR_EACH(it, apObjectList) (*it)->Render();
        }
    }
    else m_Depth.Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
}



void cWater::Reshape()
{
    const coreVector2 vWaterResolution = g_vGameResolution * WATER_SCALE_FACTOR * ENVIRONMENT_SCALE_FACTOR;

    if(m_Reflection.GetColorTarget(0u).IsValid())
    {
        // 
        m_Reflection.Delete();
        m_Reflection.Create(vWaterResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }

    // 
    m_Refraction.Delete();
    m_Refraction.Create(g_vGameResolution * ENVIRONMENT_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    if(m_Depth.GetDepthTarget().IsValid())
    {
        // create depth frame buffer
        m_Depth.Delete();
        m_Depth.Create(vWaterResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }

    // 
    this->DefineTexture(1u, m_Reflection.GetColorTarget(0u).pTexture);
    this->DefineTexture(2u, m_Refraction.GetColorTarget(0u).pTexture);
    this->DefineTexture(3u, m_Depth     .GetDepthTarget()  .pTexture);

    // 
    if(!m_Depth.GetIdentifier()) this->DefineTexture(3u, "fallback_depth.png");
}


// ****************************************************************
// constructor
cUnderWater::cUnderWater()noexcept
: cWater (NULL)
{
    // 
    this->DefineProgram("environment_under_program");
}


// ****************************************************************
// constructor
cIceWater::cIceWater(const coreHashString& sSkyTexture)noexcept
: cWater (sSkyTexture)
{
    // 
    m_Ice = (*s_cast<coreObject3D*>(this));

    // 
    m_Ice.DefineTexture(0u, "environment_water_norm.png");
    m_Ice.DefineProgram(m_Depth.GetIdentifier() ? "environment_ice_program" : NULL);
}


// ****************************************************************
// 
void cIceWater::__RenderOwn()
{
    if(!m_Ice.GetProgram().IsUsable()) return;
    if(!m_Ice.GetProgram()->Enable())  return;

    // 
    m_Ice.GetProgram()->SendUniform("u_v1Offset", m_fFlyOffset * -0.0125f);

    // 
    m_Ice.Render();
}


// ****************************************************************
// 
void cIceWater::__MoveOwn()
{
    // 
    m_Ice.SetPosition(coreVector3(0.0f, m_fFlyOffset * OUTDOOR_DETAIL, WATER_HEIGHT));
    m_Ice.Move();
}


// ****************************************************************
// constructor
cRainWater::cRainWater(const coreHashString& sSkyTexture)noexcept
: cWater       (sSkyTexture)
, m_DropList   (RAIN_DROPS)
, m_iCurDrop   (0u)
, m_fFallDelay (0.0f)
{
    // 
    m_WaveMap.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_GL_SUPPORT(EXT_texture_norm16) ? CORE_TEXTURE_SPEC_RGBA16 : CORE_TEXTURE_SPEC_RGBA8, CORE_TEXTURE_MODE_REPEAT);
    m_WaveMap.Create(g_vGameResolution * RAIN_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    m_WaveInjection.SetSize      (coreVector2(1.0f,1.0f) * RAIN_SCALE_FACTOR);
    m_WaveInjection.DefineTexture(0u, "environment_water_rain.png");
    m_WaveInjection.DefineProgram("default_2d_program");
    m_WaveInjection.Move();

    // 
    m_DropList.DefineProgram("effect_distortion_object3d_inst_program");
    for(coreUintW i = 0u; i < RAIN_DROPS; ++i)
    {
        coreObject3D* pDrop = &m_aDrop[i];

        // 
        pDrop->DefineModel  (Core::Manager::Object->GetLowQuad());
        pDrop->DefineTexture(0u, "effect_wave_norm.png");
        pDrop->DefineProgram("effect_distortion_object3d_program");
        pDrop->SetAlpha     (0.0f);

        // 
        m_DropList.BindObject(pDrop);
    }

    // 
    m_apSkyProgram[0] = Core::Manager::Resource->Get<coreProgram>("menu_grey_program");
    m_apSkyProgram[1] = Core::Manager::Resource->Get<coreProgram>("environment_vignette_grey_program");

    // 
    this->DefineTexture(0u, m_WaveMap.GetColorTarget(0u).pTexture);
    this->DefineProgram("environment_rain_program");
}


// ****************************************************************
// destructor
cRainWater::~cRainWater()
{
    // explicitly undefine to detach textures
    this->Undefine();
}



void cRainWater::Reshape()
{
    this->cWater::Reshape();

    // 
    m_WaveMap.Delete();
    m_WaveMap.Create(g_vGameResolution * RAIN_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    m_WaveInjection.DefineProgram("default_2d_program");
    m_WaveInjection.Move();

    // 
    this->DefineTexture(0u, m_WaveMap.GetColorTarget(0u).pTexture);
}


// ****************************************************************
// 
void cRainWater::__UpdateOwn()
{
    // 
    const coreFloat fMapSize = m_WaveMap.GetResolution().x;

    // 
    m_fFallDelay.Update(RAIN_DROP_SPEED * I_TO_F(RAIN_DROPS));
    if(m_fFallDelay >= 1.0f)
    {
        m_fFallDelay -= 1.0f;

        // 
        if(++m_iCurDrop >= RAIN_DROPS) m_iCurDrop = 0u;
        coreObject3D& oDrop = m_aDrop[m_iCurDrop];

        // 
        oDrop.SetPosition(coreVector3(coreVector2::Rand(-0.5f,0.5f, -0.5f,0.5f) * ((1.0f - RAIN_DROP_WIDTH) * fMapSize), 0.0f));
        oDrop.SetAlpha   (1.0f);
    }

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_WaveMap.StartDraw();
        {
            // 
            if(m_WaveInjection.GetTexture(0u).IsUsable() &&
               m_WaveInjection.GetProgram()  .IsUsable())
            {
                glDisable(GL_BLEND);
                {
                    // 
                    m_WaveInjection.Render();
                    m_WaveInjection.DefineProgram(NULL);   // # keep texture to make reshape faster
                }
                glEnable(GL_BLEND);
            }

            // 
            glColorMask (true, true, false, false);
            glClearColor(DISTORTION_CLEAR, DISTORTION_CLEAR, DISTORTION_CLEAR, DISTORTION_CLEAR);
            {
                // 
                m_WaveMap.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);

                // 
                for(coreUintW i = 0u; i < RAIN_DROPS; ++i)
                {
                    coreObject3D& oDrop = m_aDrop[i];
                    if(!oDrop.GetAlpha()) continue;

                    // 
                    oDrop.SetSize (coreVector3(1.0f,1.0f,1.0f) * (RAIN_DROP_WIDTH * fMapSize * (1.0f - oDrop.GetAlpha())));
                    oDrop.SetAlpha(MAX0(oDrop.GetAlpha() - RAIN_DROP_SPEED * TIME));
                }

                // 
                m_DropList.MoveNormal();
                m_DropList.Render();
            }
            glColorMask (true, true, true, true);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// constructor
cLava::cLava()noexcept
: m_fAnimation (0.0f)
, m_fFlyOffset (0.0f)
{
    // load object resources
    this->DefineModel  (Core::Manager::Object->GetLowTriangle());
    this->DefineTexture(0u, "environment_lava_diff.png");
    this->DefineTexture(1u, "environment_lava_norm.png");
    this->DefineTexture(2u, "environment_water_norm.png");
    this->DefineProgram("environment_lava_program");

    // set object properties
    this->SetSize(coreVector3(WATER_SIZE, WATER_SIZE, 1.0f));
}


// ****************************************************************
// render the lava-surface
void cLava::Render()
{
    if(!this->GetProgram().IsUsable()) return;
    if(!this->GetProgram()->Enable())  return;

    // update all lava uniforms
    this->GetProgram()->SendUniform("u_v1Time",   m_fAnimation);
    this->GetProgram()->SendUniform("u_v1Offset", m_fFlyOffset * -0.0125f);

    // render the 3d-object
    this->coreObject3D::Render();
}


// ****************************************************************
// move the lava-surface
void cLava::Move()
{
    // update animation value
    m_fAnimation.UpdateMod(0.008f, 100.0f);

    // move lava with camera
    this->SetPosition(coreVector3(0.0f, m_fFlyOffset * OUTDOOR_DETAIL, WATER_HEIGHT));

    // move the 3d-object
    this->coreObject3D::Move();
}