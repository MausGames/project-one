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
cWater::cWater(const coreHashString& sSkyTexture)noexcept
: m_fAnimation (0.0f)
, m_fFlyOffset (0.0f)
{
    const coreVector2 vWaterResolution = g_vGameResolution * WATER_SCALE_FACTOR;

    // create reflection frame buffer
    m_Reflection.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB8);
    m_Reflection.AttachTargetBuffer (CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16);
    m_Reflection.Create(vWaterResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create refraction frame buffer
    m_Refraction.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB8);
    m_Refraction.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create depth frame buffer
    m_Depth.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16);
    m_Depth.Create(vWaterResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create sky-plane object
    m_Sky.DefineTexture(0u, sSkyTexture);
    m_Sky.SetSize      (coreVector2(WATER_SCALE_FACTOR, WATER_SCALE_FACTOR) * SQRT2);
    m_Sky.SetTexSize   (coreVector2(WATER_SKY_SIZE,     WATER_SKY_SIZE));

    // 
    m_apSkyProgram[0] = Core::Manager::Resource->Get<coreProgram>("default_2d_program");
    m_apSkyProgram[1] = Core::Manager::Resource->Get<coreProgram>("environment_vignette_program");

    // load object resources
    this->DefineModel  (Core::Manager::Object->GetLowTriangle());
    this->DefineTexture(0u, "environment_water_norm.png");
    this->DefineTexture(1u, m_Reflection.GetColorTarget(0u).pTexture);
    this->DefineTexture(2u, m_Refraction.GetColorTarget(0u).pTexture);
    this->DefineTexture(3u, m_Depth     .GetDepthTarget()  .pTexture);
    this->DefineProgram("environment_water_program");

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

    // update all water uniforms
    this->GetProgram()->Enable();
    this->GetProgram()->SendUniform("u_v1Time",   m_fAnimation);
    this->GetProgram()->SendUniform("u_v1Offset", m_fFlyOffset * -0.0125f);

    // render the 3d-object
    this->coreObject3D::Render();

    // invalidate all frame buffer objects
    m_Reflection.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
    m_Refraction.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    m_Depth     .Invalidate(CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// move the water-surface
void cWater::Move()
{
    // update animation value
    m_fAnimation.Update(0.008f);

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
    {
        // flip projection left-right (also culling!, after StartDraw())
        c_cast<coreMatrix4&>(Core::Graphics->GetPerspective())._11 *= -1.0f;

        glDepthFunc(GL_ALWAYS);   // better performance than depth-clear
        glDisable  (GL_BLEND);
        {
            // move and render the sky-plane
            m_Sky.SetDirection(vOldCamOri.xy());
            m_Sky.SetTexOffset(coreVector2(vOldCamPos.x * 0.007f * WATER_SKY_SIZE, m_Sky.GetTexOffset().y));
            m_Sky.Move();
            m_Sky.Render(m_apSkyProgram[0]);
        }
        glDepthFunc(GL_LEQUAL);
        glEnable   (GL_BLEND);

        if(g_CurConfig.Graphics.iReflection && (g_pGame || g_pTheater->IsActive()))
        {
            glCullFace(GL_FRONT);
            {
                // render the theater
                g_pTheater->Render();

                // render the game
                if(g_pGame) g_pGame->Render();
            }
            glCullFace(GL_BACK);

            glDisable(GL_DEPTH_TEST);
            {
                // 
                m_Sky.Render(m_apSkyProgram[1]);
            }
            glEnable(GL_DEPTH_TEST);
        }
    }

    // reset camera, light and projection
    Core::Graphics->SetCamera(vOldCamPos, CAMERA_DIRECTION, vOldCamOri);
    Core::Graphics->SetLight (0u, coreVector4(0.0f,0.0f,0.0f,0.0f), vOldLight, coreVector4(0.0f,0.0f,0.0f,0.0f));
    c_cast<coreMatrix4&>(Core::Graphics->GetPerspective())._11 *= -1.0f;
}


// ****************************************************************
// update water depth map
void cWater::UpdateDepth(cOutdoor* pOutdoor, const std::vector<coreBatchList*>& apGroundObjectList)
{
    if(pOutdoor && pOutdoor->IsEnabled(CORE_OBJECT_ENABLE_ALL))
    {
        // fill depth frame buffer
        m_Depth.StartDraw();
        {
            glDepthFunc (GL_ALWAYS);   // better performance than depth-clear
            glDrawBuffer(GL_NONE);
            {
                // render the outdoor-surface
                pOutdoor->RenderDepth();
            }
            glDepthFunc (GL_LEQUAL);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
        }
    }
    else m_Depth.Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// set current fly offset
void cWater::SetFlyOffset(const coreFloat fFlyOffset)
{
    // set new value
    m_fFlyOffset = fFlyOffset;

    // move sky-plane texture
    m_Sky.SetTexOffset(coreVector2(0.0f, ((-8.0f * WATER_SKY_SIZE) / I_TO_F(OUTDOOR_HEIGHT)) * m_fFlyOffset));
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
    m_Ice = (*this);

    // 
    m_Ice.DefineTexture(0u, "environment_water_norm.png");
    m_Ice.DefineProgram("environment_ice_program");
}


// ****************************************************************
// 
void cIceWater::__RenderOwn()
{
    if(!m_Ice.GetProgram().IsUsable()) return;

    // 
    m_Ice.GetProgram()->Enable();
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
    const coreVector2 vRes = coreVector2(1.0f,1.0f) * (Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY) ? RAIN_WAVE_RES_HIGH : RAIN_WAVE_RES_LOW);

    // 
    m_WaveMap.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA16);
    m_WaveMap.Create(vRes, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    glBindTexture  (GL_TEXTURE_2D, m_WaveMap.GetColorTarget(0u).pTexture->GetIdentifier());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 
    m_WaveInjection.SetSize(m_WaveMap.GetResolution() / g_vGameResolution);
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
                    m_WaveInjection.Undefine();
                }
                glEnable(GL_BLEND);
            }

            // 
            glColorMask (true, true, false, false);
            glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
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
                    oDrop.SetAlpha(MAX(oDrop.GetAlpha() - RAIN_DROP_SPEED * Core::System->GetTime(), 0.0f));
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

    // update all lava uniforms
    this->GetProgram()->Enable();
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
    m_fAnimation.Update(0.008f);

    // move lava with camera
    this->SetPosition(coreVector3(0.0f, m_fFlyOffset * OUTDOOR_DETAIL, WATER_HEIGHT));

    // move the 3d-object
    this->coreObject3D::Move();
}