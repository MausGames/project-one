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
cHeadlight::cHeadlight()noexcept
: m_Flicker  (coreTimer(1.0f, 10.0f, 7u))
, m_iShatter (0u)
, m_fBlend   (0.0f)
, m_iDefault (0xFFu)
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
    m_Spot .DefineProgram("menu_grey_program");
    m_Point.DefineTexture(0u, "effect_headlight_point.png");
    m_Point.DefineProgram("menu_grey_program");

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
            if(m_iShatter == HEADLIGHT_TYPE_SHATTER) m_pShatterSound->PlayRelative(NULL, 0.7f, 1.0f, false, SOUND_EFFECT);

            // 
            m_Spot.SetAlpha((m_iShatter == HEADLIGHT_TYPE_ON) ? 1.0f : 0.0f);
        }
    }

    if(m_fBlend)
    {
        // 
        m_fBlend.UpdateMin(1.0f, 2.0f);
        m_Spot.SetAlpha(BLENDH3(2.0f - m_fBlend));
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
void cHeadlight::UpdateDefault(const coreUint8 iType)
{
    if(STATIC_ISVALID(g_pGame))
    {
        if(iType == 0u)
        {
            // 
            g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
            {
                if(HAS_BIT(m_iDefault, i)) this->DrawSpot(pPlayer->GetPosition(), coreVector2(60.0f,100.0f) * 1.2f, 49.0f * pPlayer->GetDirection().xy() * 1.15f);
            });
        }
        else
        {
            // 
            g_pGame->ForEachPlayer([this](const cPlayer* pPlayer, const coreUintW i)
            {
                if(HAS_BIT(m_iDefault, i)) this->DrawPoint(pPlayer->GetPosition(), coreVector2(1.0f,1.0f) * 30.0f);
            });
        }

        STATIC_ASSERT(GAME_PLAYERS <= sizeof(m_iDefault)*8u)
    
    if(g_pGame->GetCurMission() && !g_pGame->GetCurMission()->GetCurBoss())
        g_pPostProcessing->SetBorderAll(POST_DEFAULT_BORDER_MIN);   // TODO 1: no effect during pause here
    }

    // 
    this->Update();
}


// ****************************************************************
// 
void cHeadlight::DrawSpot(const coreVector3 vPosition, const coreVector2 vSize, const coreVector2 vOffset)
{
    // 
    sSpotCommand oCommand;
    oCommand.vPosition  = (g_pForeground->Project2D(vPosition) + g_pForeground->Project2D(coreVector3(vOffset, 0.0f))) * HEADLIGHT_SCALE_FACTOR;   // separate
    oCommand.vSize      = (g_pForeground->Project2D(coreVector3(vSize, 0.0f)))                                         * HEADLIGHT_SCALE_FACTOR;
    oCommand.vDirection = vOffset.Normalized().InvertedX();

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
    ASSERT(m_iShatter != HEADLIGHT_TYPE_SHATTER)
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
    m_iShatter = HEADLIGHT_TYPE_ON;

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