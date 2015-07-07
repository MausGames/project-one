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
cWater::cWater()noexcept
: m_fAnimation (0.0f)
, m_fFlyOffset (0.0f)
{
    // create reflection and refraction buffers
    m_iAboveReflection.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_iAboveReflection.AttachTargetBuffer (CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH);
    m_iAboveReflection.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    m_iBelowRefraction.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_iBelowRefraction.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH);
    m_iBelowRefraction.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create sky-plane object
    m_Sky.DefineTexture(0u, "environment_clouds_blue.png");
    m_Sky.DefineProgram("default_2d_program");
    m_Sky.SetSize      (coreVector2(1.0f,1.0f) * SQRT(2.0f));
    m_Sky.SetTexSize   (coreVector2(WATER_SKY_SIZE, WATER_SKY_SIZE));

    // load object resources
    this->DefineModel  (Core::Manager::Object->GetLowModel());
    this->DefineTexture(0u, "environment_water_norm.png");
    this->DefineTexture(1u, m_iAboveReflection.GetColorTarget(0u).pTexture);
    this->DefineTexture(2u, m_iBelowRefraction.GetColorTarget(0u).pTexture);
    this->DefineTexture(3u, m_iBelowRefraction.GetDepthTarget()  .pTexture);
    this->DefineProgram("environment_water_program");

    // set object properties
    this->SetPosition(coreVector3(      0.0f,       0.0f, WATER_HEIGHT));
    this->SetSize    (coreVector3(WATER_SIZE, WATER_SIZE,         1.0f));

    // 
    m_apTexture[0].GetHandle()->OnLoadOnce([&]()
    {
        glBindTexture(GL_TEXTURE_2D, m_apTexture[0]->GetTexture());
        if(CORE_GL_SUPPORT(EXT_texture_filter_anisotropic)) glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);
    });
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
    pBackground->Blit(CORE_FRAMEBUFFER_TARGET_COLOR, &m_iBelowRefraction);

    // update all water uniforms
    this->GetProgram()->Enable();
    this->GetProgram()->SendUniform("u_v1Time",   m_fAnimation);
    this->GetProgram()->SendUniform("u_v1Offset", m_fFlyOffset * -0.0125f);

    // render the 3d-object
    coreObject3D::Render();

    // invalidate all frame buffer objects
    m_iAboveReflection.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
    m_iBelowRefraction.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// move the water-surface
void cWater::Move()
{
    // update animation value
    m_fAnimation.Update(0.008f);

    // move water level up and down
    this->SetPosition(coreVector3(0.0f, m_fFlyOffset * OUTDOOR_DETAIL, WATER_HEIGHT + 0.4f * SIN(40.0f * m_fAnimation)));

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// update water reflection map
void cWater::UpdateReflection()
{
    // save current camera and light properties
    const coreVector3 vOldCamPos = Core::Graphics->GetCamPosition();
    const coreVector3 vOldCamOri = Core::Graphics->GetCamOrientation();
    const coreVector4 vOldLight  = Core::Graphics->GetLight(0u).vDirection;

    // flip camera upside-down and override light
    Core::Graphics->SetCamera(-CAMERA_POSITION + coreVector3(0.0f, 0.0f, WATER_HEIGHT*2.0f), -CAMERA_DIRECTION, CAMERA_ORIENTATION);
    Core::Graphics->SetLight (0u, coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(LIGHT_DIRECTION, 0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));

    // fill reflection frame buffer
    m_iAboveReflection.StartDraw();
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
            m_Sky.Render();
        }
        glDepthFunc(GL_LEQUAL);
        glEnable   (GL_BLEND);

        if(g_CurConfig.Graphics.iReflection && g_pGame)
        {
            // render the game
            glCullFace(GL_FRONT);
            g_pGame->Render();
            glCullFace(GL_BACK);
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
    if(pOutdoor)
    {
        // fill only depth component of refraction frame buffer
        m_iBelowRefraction.StartDraw();
        {
            glDepthFunc (GL_ALWAYS);   // better performance than depth-clear
            glDrawBuffer(GL_NONE);
            {
                const coreMatrix4 mViewProj = Core::Graphics->GetCamera() * Core::Graphics->GetPerspective();

                // use shadow shaders for light-weight depth rendering
                cShadow::SendTransformInstanced(mViewProj);
                cShadow::SendTransformSingle   (mViewProj);

                // render the outdoor-surface
                pOutdoor->Render(cShadow::GetProgramSingle());

                // render all ground objects (after outdoor-surface, because of GL_ALWAYS)
                FOR_EACH(it, apGroundObjectList)
                    (*it)->Render(cShadow::GetProgramInstanced(), cShadow::GetProgramSingle());
            }
            glDepthFunc (GL_LEQUAL);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
        }
    }
    else m_iBelowRefraction.Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// set current fly offset
void cWater::SetFlyOffset(const coreFloat& fFlyOffset)
{
    // set new value
    m_fFlyOffset = fFlyOffset;

    // move sky-plane texture
    m_Sky.SetTexOffset(coreVector2(0.0f, ((-8.0f * WATER_SKY_SIZE) / I_TO_F(OUTDOOR_HEIGHT)) * m_fFlyOffset));
}