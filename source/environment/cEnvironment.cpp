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
cEnvironment::cEnvironment()noexcept
: m_pBackground    (NULL)
, m_pOldBackground (NULL)
, m_Transition     (coreTimer(1.3f, 0.9f, 1u))
, m_fFlyOffset     (0.0f)
, m_fSideOffset    (0.0f)
, m_vCameraPos     (CAMERA_POSITION)
, m_vLightDir      (LIGHT_DIRECTION)
, m_bActive        (false)
{
    // create environment frame buffer
    m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // create mix object
    m_MixObject.DefineProgram("full_transition_program");
    m_MixObject.DefineTexture(2u, "menu_background_black.png");
    m_MixObject.SetSize      (coreVector2(1.0f,1.0f));
    m_MixObject.Move();

    // reset transformation properties
    m_avDirection[1] = m_avDirection[0] = coreVector2(0.0f,1.0f);
    m_avSide     [1] = m_avSide     [0] = coreVector2(0.0f,0.0f);
    m_afSpeed    [1] = m_afSpeed    [0] = 2.0f;

    // load first background
    m_pBackground = new cNoBackground();
    this->ChangeBackground(MAX(Core::Config->GetInt("Game", "Background", 0), s_cast<coreInt32>(cGrassBackground::ID)));
}


// ****************************************************************
// destructor
cEnvironment::~cEnvironment()
{
    // save last background
    Core::Config->SetInt("Game", "Background", m_pBackground->GetID());

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

    if(m_Transition.GetStatus())
    {
        if(m_MixObject.GetProgram().IsUsable())
        {
            // render old background
            m_pOldBackground->Render();

            // set transition time
            m_MixObject.GetProgram()->Enable();
            m_MixObject.GetProgram()->SendUniform("u_v1Transition", m_Transition.GetValue(CORE_TIMER_GET_NORMAL));

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            {
                // mix both backgrounds together
                m_iFrameBuffer.StartDraw();
                m_MixObject.Render();
            }
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);

            // invalidate single backgrounds
            m_pOldBackground->GetResolvedTexture()->GetColorTarget(0u).pTexture->Invalidate(0u);
            m_pBackground   ->GetResolvedTexture()->GetColorTarget(0u).pTexture->Invalidate(0u);
        }
    }
}


// ****************************************************************
// move the environment
void cEnvironment::Move()
{
    // update all transformation properties
    const coreFloat fFactor = Core::System->GetTime() * 2.0f;
    m_avDirection[0] = (m_avDirection[0] + (m_avDirection[1] - m_avDirection[0]) * fFactor * 4.0f).Normalize();
    m_avSide     [0] =  m_avSide     [0] + (m_avSide     [1] - m_avSide     [0]) * fFactor * 8.0f;
    m_afSpeed    [0] =  m_afSpeed    [0] + (m_afSpeed    [1] - m_afSpeed    [0]) * fFactor * 0.8f;

    // calculate global fly offset
    m_fFlyOffset += Core::System->GetTime() * m_afSpeed[0];
    while(m_fFlyOffset <  0.0f)                   m_fFlyOffset += I_TO_F(OUTDOOR_HEIGHT);
    while(m_fFlyOffset >= I_TO_F(OUTDOOR_HEIGHT)) m_fFlyOffset -= I_TO_F(OUTDOOR_HEIGHT);

    // calculate global side offset (only perpendicular to flight direction, never on diagonal camera (smooth with max-min))
    const coreVector2 vAbsDir = m_avDirection[0].Abs();
    m_fSideOffset             = 0.0f;//coreVector2::Dot(m_avDirection[0].Rotated90(), m_avSide[0]) * (vAbsDir.Max() - vAbsDir.Min()); TODO # 

    // calculate camera and light values
    m_vCameraPos = coreVector3(m_fSideOffset, m_fFlyOffset * OUTDOOR_DETAIL, CAMERA_POSITION.z);
    m_vLightDir  = LIGHT_DIRECTION * coreMatrix4::RotationZ(m_avDirection[0]);

    // move current background
    m_pBackground->Move();

    if(m_Transition.GetStatus())
    {
        // update transition and move old background (do not update while new background is still loading)
        if(!Core::Manager::Resource->IsLoading() && m_bActive && m_Transition.Update(1.0f))
        {
            // delete old background
            m_MixObject.DefineTexture(0u, NULL);
            SAFE_DELETE(m_pOldBackground)
        }
        else m_pOldBackground->Move();
    }
}


// ****************************************************************
// change current background
void cEnvironment::ChangeBackground(const coreInt32& iID)
{
    if(m_pBackground) if(m_pBackground->GetID() == iID) return;

    // delete possible old background
    m_MixObject.DefineTexture(0u, NULL);
    SAFE_DELETE(m_pOldBackground)

    // make current to old
    m_pOldBackground = m_pBackground;

    // create new background
    switch(iID)
    {
    default: ASSERT(false)
    case cNoBackground     ::ID: m_pBackground = new cNoBackground     (); break;
    case cGrassBackground  ::ID: m_pBackground = new cGrassBackground  (); break;
    case cSeaBackground    ::ID: m_pBackground = new cSeaBackground    (); break;
    case cDesertBackground ::ID: m_pBackground = new cDesertBackground (); break;
    case cSpaceBackground  ::ID: m_pBackground = new cSpaceBackground  (); break;
    case cVolcanoBackground::ID: m_pBackground = new cVolcanoBackground(); break;
    case cSnowBackground   ::ID: m_pBackground = new cSnowBackground   (); break;
    case cMossBackground   ::ID: m_pBackground = new cMossBackground   (); break;
    }

    if(m_pOldBackground)
    {
        // start transition
        m_Transition.Play(CORE_TIMER_PLAY_RESET);
        m_Transition.SetValue(-0.15f);

        // set transition textures
        m_MixObject.DefineTexture(0u, m_pOldBackground->GetResolvedTexture()->GetColorTarget(0u).pTexture);
        m_MixObject.DefineTexture(1u, m_pBackground   ->GetResolvedTexture()->GetColorTarget(0u).pTexture);
    }
}


// ****************************************************************
// retrieve safe height value
coreFloat cEnvironment::RetrieveSafeHeight(const coreVector2& vPosition)
{
    // check for available outdoor-surface
    cOutdoor* pOutdoor = m_pBackground->GetOutdoor();
    if(!pOutdoor) return 0.0f;

    // retrieve height value
    const coreFloat fHeight = pOutdoor->RetrieveHeight(vPosition);

    // check for available water-surface
    cWater* pWater = m_pBackground->GetWater();
    if(pWater) return MAX(fHeight, WATER_HEIGHT);

    return fHeight;
}


// ****************************************************************
// reset with the resource manager
void cEnvironment::__Reset(const coreResourceReset& bInit)
{
    if(bInit)
    {
        const coreInt32 iID = P_TO_I(m_pBackground);

        // re-create background with saved ID
        m_pBackground = NULL;
        this->ChangeBackground(iID);

        // re-create environment frame buffer
        m_iFrameBuffer.Create(g_vGameResolution, CORE_FRAMEBUFFER_CREATE_NORMAL);
    }
    else
    {
        const coreInt32 iID = m_pBackground->GetID();

        // unbind textures and stop possible transition
        m_MixObject.DefineTexture(0u, NULL);
        m_MixObject.DefineTexture(1u, NULL);
        m_Transition.Stop();

        // delete both backgrounds
        SAFE_DELETE(m_pOldBackground)
        SAFE_DELETE(m_pBackground)

        // delete environment frame buffer
        m_iFrameBuffer.Delete();

        // save background ID
        m_pBackground = r_cast<cBackground*>(I_TO_P(iID));
    }
}