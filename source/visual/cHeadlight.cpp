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
cHeadlight::cHeadlight()noexcept
: m_Flicker  (coreTimer(1.0f, 10.0f, 7u))
, m_iShatter (0u)
{
    const coreTextureSpec oSpec = CORE_GL_SUPPORT(ARB_texture_rg) ? CORE_TEXTURE_SPEC_R8 : CORE_TEXTURE_SPEC_RGB8;

    // 
    m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, oSpec);
    m_FrameBuffer.Create(g_vGameResolution * HEADLIGHT_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // load object resources
    this->DefineTexture(0u, m_FrameBuffer.GetColorTarget(0u).pTexture);
    this->DefineProgram("effect_headlight_program");

    // set object properties
    this->SetSize(coreVector2(1.0f,1.0f));
    this->Move();

    // 
    m_Spot .DefineTexture(0u, "effect_headlight_spot.png");
    m_Spot .DefineProgram("default_2d_program");
    m_Point.DefineTexture(0u, "effect_headlight_point.png");
    m_Point.DefineProgram("default_2d_program");

    // 
    m_pFlickerSound = Core::Manager::Resource->Get<coreSound>("effect_flicker.wav");
    m_pShatterSound = Core::Manager::Resource->Get<coreSound>("effect_shatter.wav");
}


// ****************************************************************
// destructor
cHeadlight::~cHeadlight()
{
    // explicitly undefine to detach textures
    this->Undefine();

    // 
    this->StopFlicker();
}


// ****************************************************************
// 
void cHeadlight::Render()
{
    // 
    this->coreFullscreen::Render();

    // 
    m_FrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// 
void cHeadlight::Update()
{
    if(m_Flicker.GetStatus())
    {
        // 
        m_Flicker.Update(1.0f);
        if(m_Flicker.GetStatus())
        {
            // 
            m_Spot.SetAlpha(g_CurConfig.Graphics.iFlash ? m_Flicker.GetValue(CORE_TIMER_GET_NORMAL) : 0.5f);
        }
        else
        {
            // 
            this->StopFlicker();
            if(m_iShatter == 2u) m_pShatterSound->PlayRelative(NULL, 0.7f, 1.0f, false, SOUND_EFFECT);

            // 
            m_Spot.SetAlpha(m_iShatter ? 0.0f : 1.0f);
        }
    }

    if(!m_aSpotCommand.empty() || !m_aPointCommand.empty())
    {
        // 
        m_Spot .SetColor3(coreVector3(1.0f,1.0f,1.0f) * HEADLIGHT_INTENSITY * m_Spot .GetAlpha());
        m_Point.SetColor3(coreVector3(1.0f,1.0f,1.0f) * HEADLIGHT_INTENSITY * m_Point.GetAlpha());

        // 
        m_FrameBuffer.StartDraw();
        m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
        {
            // 
            glBlendEquation(GL_MAX);
            {
                // 
                FOR_EACH(it, m_aSpotCommand)
                {
                    m_Spot.SetPosition (it->vPosition);
                    m_Spot.SetSize     (it->vSize);
                    m_Spot.SetDirection(it->vDirection);

                    m_Spot.Move();
                    m_Spot.Render();
                }

                // 
                FOR_EACH(it, m_aPointCommand)
                {
                    m_Point.SetPosition(it->vPosition);
                    m_Point.SetSize    (it->vSize);

                    m_Point.Move();
                    m_Point.Render();
                }
            }
            glBlendEquation(GL_FUNC_ADD);
        }

        // 
        m_aSpotCommand .clear();
        m_aPointCommand.clear();
    }
    else m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// 
void cHeadlight::UpdateDefault()
{
    if(STATIC_ISVALID(g_pGame))
    {
        // 
        g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
        {
            this->DrawSpot(pPlayer->GetPosition() + 49.0f * pPlayer->GetDirection(), coreVector2(60.0f,100.0f), pPlayer->GetDirection().xy());
        });
    }

    // 
    this->Update();
}


// ****************************************************************
// 
void cHeadlight::DrawSpot(const coreVector3 vPosition, const coreVector2 vSize, const coreVector2 vDirection)
{
    // 
    sSpotCommand oCommand;
    oCommand.vPosition  = g_pForeground->Project2D(vPosition)                * HEADLIGHT_SCALE_FACTOR;
    oCommand.vSize      = g_pForeground->Project2D(coreVector3(vSize, 0.0f)) * HEADLIGHT_SCALE_FACTOR;
    oCommand.vDirection = vDirection.InvertedX();

    // 
    m_aSpotCommand.push_back(oCommand);
}


// ****************************************************************
// 
void cHeadlight::DrawPoint(const coreVector3 vPosition, const coreVector2 vSize)
{
    // 
    sPointCommand oCommand;
    oCommand.vPosition = g_pForeground->Project2D(vPosition)                * HEADLIGHT_SCALE_FACTOR;
    oCommand.vSize     = g_pForeground->Project2D(coreVector3(vSize, 0.0f)) * HEADLIGHT_SCALE_FACTOR;

    // 
    m_aPointCommand.push_back(oCommand);
}

void cHeadlight::DrawPoint(const coreObject3D* pObject)
{
    ASSERT(pObject)

    // 
    this->DrawPoint(pObject->GetPosition(), coreVector2(3.0f,3.0f) * pObject->GetVisualRadius());
}


// ****************************************************************
// 
void cHeadlight::PlayFlicker(const coreUint8 iShatter)
{
    // 
    if(!m_Flicker.GetStatus()) m_pFlickerSound->PlayRelative(this, 3.0f, 1.0f, true, SOUND_EFFECT);
    m_Flicker.Play(CORE_TIMER_PLAY_RESET);

    // 
    ASSERT(m_iShatter != 2u)
    m_iShatter = iShatter;
}

void cHeadlight::StopFlicker()
{
    // 
    if(m_pFlickerSound->EnableRef(this)) m_pFlickerSound->Stop();
    m_Flicker.Stop();
}

void cHeadlight::ResetFlicker()
{
    // 
    this->StopFlicker();

    // 
    m_iShatter = 0u;

    // 
    m_Spot.SetAlpha(1.0f);
}


// ****************************************************************
// reset with the resource manager
void cHeadlight::__Reset(const coreResourceReset eInit)
{
    if(eInit) {m_FrameBuffer.Create(g_vGameResolution * HEADLIGHT_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL); this->Move();}
         else {m_FrameBuffer.Delete();}
}