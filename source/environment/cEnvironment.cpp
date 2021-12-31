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
cEnvironment::cEnvironment()noexcept
: m_pBackground    (NULL)
, m_pOldBackground (NULL)
, m_iLastID        (0)
, m_TransitionTime (coreTimer(1.3f, 0.0f, 1u))
, m_vTransitionDir (coreVector2(0.0f,0.0f))
, m_afStrength     {}
, m_fFlyOffset     (0.0f)
, m_fFlyShove      (0.0f)
, m_fSideOffset    (0.0f)
, m_vCameraPos     (CAMERA_POSITION)
, m_vLightDir      (LIGHT_DIRECTION)
, m_bActive        (false)
{
    // create environment frame buffer
    m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB8);
    m_FrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create mix object
    m_MixObject.DefineTexture(2u, "menu_background_black.png");
    m_MixObject.SetSize      (coreVector2(1.0f,1.0f));
    m_MixObject.Move();

    // 
    m_apMixProgram[ENVIRONMENT_MIX_FADE]    = Core::Manager::Resource->Get<coreProgram>("full_transition_fade_program");
    m_apMixProgram[ENVIRONMENT_MIX_WIPE]    = Core::Manager::Resource->Get<coreProgram>("full_transition_wipe_program");
    m_apMixProgram[ENVIRONMENT_MIX_CURTAIN] = Core::Manager::Resource->Get<coreProgram>("full_transition_curtain_program");
    m_apMixProgram[ENVIRONMENT_MIX_CIRCLE]  = Core::Manager::Resource->Get<coreProgram>("full_transition_circle_program");

    // reset transformation properties
    m_avDirection[1] = m_avDirection[0] = ENVIRONMENT_DEFAULT_DIRECTION;
    m_avSide     [1] = m_avSide     [0] = ENVIRONMENT_DEFAULT_SIDE;
    m_afSpeed    [1] = m_afSpeed    [0] = ENVIRONMENT_DEFAULT_SPEED;
    m_afHeight   [1] = m_afHeight   [0] = ENVIRONMENT_DEFAULT_HEIGHT;

    // load first background
    m_pBackground = new cNoBackground();
    this->ChangeBackground(Core::Config->GetInt("Game", "Background", cCloudBackground::ID), ENVIRONMENT_MIX_CURTAIN, 0.75f, coreVector2(1.0f,0.0f));
}


// ****************************************************************
// destructor
cEnvironment::~cEnvironment()
{
    // save last background
    Core::Config->SetInt("Game", "Background", m_iLastID);

    // explicitly undefine to detach textures
    m_MixObject.Undefine();

    // delete background instances
    SAFE_DELETE(m_pBackground)
    SAFE_DELETE(m_pOldBackground)
}


// ****************************************************************
// render the environment
void cEnvironment::Render()
{
    // set environment camera and light
    Core::Graphics->SetCamera(m_vCameraPos, CAMERA_DIRECTION, coreVector3(m_avDirection[0].InvertedX(), 0.0f));
    Core::Graphics->SetLight (0u, coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(m_vLightDir, 0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));

    // render current background
    m_pBackground->Render();

    if(m_TransitionTime.GetStatus())
    {
        if(m_MixObject.GetProgram().IsUsable())
        {
            // render old background
            m_pOldBackground->Render();

            // set transition uniforms
            m_MixObject.GetProgram()->Enable();
            m_MixObject.GetProgram()->SendUniform("u_v1TransitionTime", m_TransitionTime.GetValue(CORE_TIMER_GET_NORMAL));
            m_MixObject.GetProgram()->SendUniform("u_v2TransitionDir",  m_vTransitionDir);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            {
                // mix both backgrounds together
                m_FrameBuffer.StartDraw();
                m_MixObject.Render();
            }
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);

            // invalidate single backgrounds
            m_pOldBackground->GetResolvedTexture()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
            m_pBackground   ->GetResolvedTexture()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
        }
        else m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
    }

    // 
    Core::Graphics->SetCamera(CAMERA_POSITION, CAMERA_DIRECTION, CAMERA_ORIENTATION);
    Core::Graphics->SetLight (0u, coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(LIGHT_DIRECTION, 0.0f), coreVector4(0.0f,0.0f,0.0f,0.0f));
}


// ****************************************************************
// move the environment
void cEnvironment::Move()
{
    // update all transformation properties
    if(m_afStrength[0] > 0.0f) m_avDirection[0] = SmoothAim(m_avDirection[0], m_avDirection[1], m_afStrength[0]);
    if(m_afStrength[1] > 0.0f) m_avSide     [0] = m_avSide  [0] + (m_avSide  [1] - m_avSide  [0]) * (TIME * m_afStrength[1]);
    if(m_afStrength[2] > 0.0f) m_afSpeed    [0] = m_afSpeed [0] + (m_afSpeed [1] - m_afSpeed [0]) * (TIME * m_afStrength[2]);
    if(m_afStrength[3] > 0.0f) m_afHeight   [0] = m_afHeight[0] + (m_afHeight[1] - m_afHeight[0]) * (TIME * m_afStrength[3]);

    // calculate global fly offset
    m_fFlyOffset += TIME * m_afSpeed[0];
    m_fFlyShove   = 0.0f;
         if(m_fFlyOffset <  0.0f)                   {m_fFlyOffset += I_TO_F(OUTDOOR_HEIGHT); m_fFlyShove =  I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL; m_pBackground->ShoveAdds(m_fFlyShove); if(m_pOldBackground) m_pOldBackground->ShoveAdds(m_fFlyShove);}
    else if(m_fFlyOffset >= I_TO_F(OUTDOOR_HEIGHT)) {m_fFlyOffset -= I_TO_F(OUTDOOR_HEIGHT); m_fFlyShove = -I_TO_F(OUTDOOR_HEIGHT) * OUTDOOR_DETAIL; m_pBackground->ShoveAdds(m_fFlyShove); if(m_pOldBackground) m_pOldBackground->ShoveAdds(m_fFlyShove);}

    // handle rare rounding errors
    m_fFlyOffset = CLAMP(m_fFlyOffset, 0.0f, I_TO_F(OUTDOOR_HEIGHT) - CORE_MATH_PRECISION);

    // calculate global side offset (only perpendicular to flight direction, never on diagonal camera (smooth with max-min))
    const coreVector2 vAbsDir = m_avDirection[0].Processed(ABS);
    m_fSideOffset             = 0.65f * coreVector2::Dot(m_avDirection[0].Rotated90(), m_avSide[0]) * (vAbsDir.Max() - vAbsDir.Min());

    // calculate camera and light values
    m_vCameraPos = coreVector3(m_fSideOffset, m_fFlyOffset * OUTDOOR_DETAIL, CAMERA_POSITION.z - m_afHeight[0]);
    m_vLightDir  = LIGHT_DIRECTION * coreMatrix3::Rotation(m_avDirection[0]);

    // move current background
    m_pBackground->Move();

    if(m_TransitionTime.GetStatus())
    {
        // update transition and move old background (do not update while new background is still loading)
        if(m_bActive && (!Core::Manager::Resource->IsLoading() || (m_TransitionTime.GetValue(CORE_TIMER_GET_NORMAL) > 0.0f)) && m_TransitionTime.Update(1.0f))
        {
            // delete old background
            m_MixObject.DefineTexture(0u, NULL);
            m_MixObject.DefineTexture(1u, NULL);
            SAFE_DELETE(m_pOldBackground)
        }
        else m_pOldBackground->Move();
    }

    // 
    const coreVector3 vColor = m_TransitionTime.GetStatus() ? LERPH3(m_pOldBackground->GetColor(), m_pBackground->GetColor(), m_TransitionTime.GetValuePct(CORE_TIMER_GET_NORMAL)) : m_pBackground->GetColor();
    g_pMenu->SetHighlightColor(vColor);
}


// ****************************************************************
// change current background
void cEnvironment::ChangeBackground(const coreInt32 iID, const coreUintW iTransitionType, const coreFloat fTransitionSpeed, const coreVector2 vTransitionDir)
{
    if(m_pBackground) if(m_pBackground->GetID() == iID) return;

    // delete possible old background
    m_MixObject.DefineTexture(0u, NULL);
    m_MixObject.DefineTexture(1u, NULL);
    SAFE_DELETE(m_pOldBackground)

    // make current to old
    if(!fTransitionSpeed) SAFE_DELETE(m_pBackground)
    m_pOldBackground = m_pBackground;

    // create new background
    switch(iID)
    {
    default: WARN_IF(true) {}
    case cNoBackground     ::ID: m_pBackground = new cNoBackground     (); break;
    case cGrassBackground  ::ID: m_pBackground = new cGrassBackground  (); break;
    case cSeaBackground    ::ID: m_pBackground = new cSeaBackground    (); break;
    case cDesertBackground ::ID: m_pBackground = new cDesertBackground (); break;
    case cSpaceBackground  ::ID: m_pBackground = new cSpaceBackground  (); break;
    case cVolcanoBackground::ID: m_pBackground = new cVolcanoBackground(); break;
    case cSnowBackground   ::ID: m_pBackground = new cSnowBackground   (); break;
    case cMossBackground   ::ID: m_pBackground = new cMossBackground   (); break;
    case cDarkBackground   ::ID: m_pBackground = new cDarkBackground   (); break;
    case cStomachBackground::ID: m_pBackground = new cStomachBackground(); break;
    case cCloudBackground  ::ID: m_pBackground = new cCloudBackground  (); break;
    }

    // 
    if((iID != cNoBackground::ID) && (iID != cStomachBackground::ID))
        m_iLastID = iID;

    if(m_pOldBackground)
    {
        ASSERT((iTransitionType < ENVIRONMENT_MIXES) && fTransitionSpeed && vTransitionDir.IsNormalized())

        // start transition
        m_TransitionTime.Play(CORE_TIMER_PLAY_RESET);
        m_TransitionTime.SetValue(-0.15f);
        m_TransitionTime.SetSpeed(0.9f * fTransitionSpeed);
        m_vTransitionDir = vTransitionDir;

        // set transition resources
        m_MixObject.DefineProgram(m_apMixProgram[iTransitionType]);
        m_MixObject.DefineTexture(0u, m_pOldBackground->GetResolvedTexture()->GetColorTarget(0u).pTexture);
        m_MixObject.DefineTexture(1u, m_pBackground   ->GetResolvedTexture()->GetColorTarget(0u).pTexture);
    }

    Core::Log->Info("Background (%s) created", m_pBackground->GetName());
}


// ****************************************************************
// 
FUNC_LOCAL coreFloat cEnvironment::RetrieveTransitionBlend(const cBackground* pBackground)const
{
    return MAX(m_TransitionTime.GetValue((m_pBackground == pBackground) ? CORE_TIMER_GET_NORMAL : CORE_TIMER_GET_REVERSED), 0.0f);
}


// ****************************************************************
// retrieve safe height value
FUNC_PURE coreFloat cEnvironment::RetrieveSafeHeight(const coreVector2 vPosition, const coreFloat fFallbackHeight)const
{
    // check for available outdoor-surface
    const cOutdoor* pOutdoor = m_pBackground->GetOutdoor();
    if(!pOutdoor) return fFallbackHeight;

    // retrieve height value
    return pOutdoor->RetrieveHeight(vPosition);
}


// ****************************************************************
// retrieve safe ray intersection point
FUNC_PURE coreVector3 cEnvironment::RetrieveSafeIntersect(const coreVector3 vRayPosition, const coreVector3 vRayDirection, const coreFloat fFallbackHeight)const
{
    // check for available outdoor-surface
    const cOutdoor* pOutdoor = m_pBackground->GetOutdoor();
    if(!pOutdoor) return vRayPosition + vRayDirection * ((vRayPosition.z - fFallbackHeight) * RCP(vRayDirection.z));

    // retrieve ray intersection point
    return pOutdoor->RetrieveIntersect(vRayPosition, vRayDirection);
}


// ****************************************************************
// reset with the resource manager
void cEnvironment::__Reset(const coreResourceReset eInit)
{
    if(eInit)
    {
        // re-create environment frame buffer
        m_FrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

        // 
        m_MixObject.Move();
    }
    else
    {
        // delete environment frame buffer
        m_FrameBuffer.Delete();
    }
}