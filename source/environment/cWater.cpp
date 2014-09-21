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
    m_iAboveReflection.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iAboveReflection.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    m_iBelowRefraction.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iBelowRefraction.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_DEPTH, 0, CORE_TEXTURE_SPEC_DEPTH);
    m_iBelowRefraction.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // create sky-plane object
    m_Sky.DefineTexture(0, "environment_clouds_blue.png");
    m_Sky.DefineProgram("default_2d_program");
    m_Sky.SetSize(coreVector2(1.0f,1.0f) * SQRT(2.0f));

    // load object resources
    this->DefineModel  ("default_square.md5mesh");
    this->DefineTexture(0, "environment_water_norm.png");
    this->DefineTexture(1, m_iAboveReflection.GetColorTarget(0).pTexture);
    this->DefineTexture(2, m_iBelowRefraction.GetColorTarget(0).pTexture);
    this->DefineTexture(3, m_iBelowRefraction.GetDepthTarget().pTexture);
    this->DefineProgram("environment_water_program");

    // set object properties
    this->SetPosition   (coreVector3(  0.0f,   0.0f,  WATER_HEIGHT));
    this->SetDirection  (coreVector3(  0.0f,   0.0f, -1.0f));
    this->SetOrientation(coreVector3(  0.0f,   1.0f,  0.0f));
    this->SetSize       (coreVector3(200.0f, 200.0f,  1.0f));
}


// ****************************************************************
// destructor
cWater::~cWater()
{
    // explicitely undefine to detach textures
    this->Undefine();
}


// ****************************************************************
// render the water-surface
void cWater::Render(coreFrameBuffer* pBackground)
{
    if(!this->GetProgram().IsUsable()) return;

    // blit current background into own refraction buffer
    pBackground->Blit(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH, &m_iBelowRefraction);

    // update all water uniforms
    this->GetProgram()->Enable();
    this->GetProgram()->SendUniform("u_fTime",   m_fAnimation);
    this->GetProgram()->SendUniform("u_fOffset", m_fFlyOffset * -0.0125f);
    this->GetProgram()->SendUniform("u_fSmooth", 0.5f - (this->GetPosition().z - WATER_HEIGHT));

    // render the 3d-object
    coreObject3D::Render();
    
    // invalidate all frame buffer objects
    m_iAboveReflection.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    m_iBelowRefraction.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
}


// ****************************************************************
// move the water-surface
void cWater::Move()
{
    // update animation value
    m_fAnimation.Update(0.02f);

    // move water level up and down
    this->SetPosition(coreVector3(0.0f, m_fFlyOffset * OUTDOOR_DETAIL, WATER_HEIGHT + 0.3f * SIN(40.0f * m_fAnimation)));

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
void cWater::UpdateReflection()
{
    // TODO #
    // if(!CONFIG_REFLECTION) return; (sky-plane?)

    // save current camera properties
    const coreVector3 vOldCamPos = Core::Graphics->GetCamPosition();
    const coreVector3 vOldCamOri = Core::Graphics->GetCamOrientation();

    // flip camera upside-down
    const coreVector3 vNewCamPos = vOldCamPos * coreVector3(1.0f,1.0f,-1.0f) + 2.0f * this->GetPosition();
    Core::Graphics->SetCamera(vNewCamPos, coreVector3(0.0f,0.0f,1.0f), coreVector3(0.0f,1.0f,0.0f));
    
    // create reflection buffer
    m_iAboveReflection.StartDraw();
    {
        // move and render the sky-plane
        m_Sky.SetDirection(vOldCamOri.xy());
        m_Sky.SetTexOffset(coreVector2(vOldCamPos.x * 0.006f, m_Sky.GetTexOffset().y));
        m_Sky.Move();
        m_Sky.Render();

        // TODO #
    }

    // reset camera
    Core::Graphics->SetCamera(vOldCamPos, coreVector3(0.0f,0.0f,-1.0f), vOldCamOri);
}


// ****************************************************************
// set current fly offset
void cWater::SetFlyOffset(const float& fFlyOffset)
{
    // set new value
    m_fFlyOffset = fFlyOffset;

    // move sky-plane texture
    m_Sky.SetTexOffset(coreVector2(0.0f, (-7.0f / float(OUTDOOR_HEIGHT)) * m_fFlyOffset));
}