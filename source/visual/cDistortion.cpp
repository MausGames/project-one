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
cDistortion::cDistortion()noexcept
: m_iCurWave  (0u)
, m_iCurBurst (0u)
, m_bActive   (false)
{
    // create distortion frame buffer
    m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_GL_SUPPORT(ARB_texture_rg) ? CORE_TEXTURE_SPEC_RG : CORE_TEXTURE_SPEC_RGB);
    m_FrameBuffer.Create(g_vGameResolution * DISTORTION_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    for(coreUintW i = 0u; i < DISTORTION_WAVES; ++i)
    {
        m_aWave[i].DefineTexture(0u, "effect_wave_norm.png");
        m_aWave[i].DefineProgram("effect_distortion_program");
        m_aWave[i].SetAlpha     (0.0f);
    }

    // 
    for(coreUintW i = 0u; i < DISTORTION_BURSTS; ++i)
    {
        m_aBurst[i].DefineTexture(0u, "effect_wave_norm.png");
        m_aBurst[i].DefineProgram("effect_distortion_direct_program");
        m_aBurst[i].SetAlpha     (0.0f);
    }
}


// ****************************************************************
// update the distortion-effect
void cDistortion::Update()
{
    if(!g_CurConfig.Graphics.iDistortion)
    {
        // 
        m_bActive = false;
        return;
    }

    // 
    glClearColor(0.5f, 0.5f, 0.0f, 0.0f);

    // create distortion only with active objects
    m_bActive = std::any_of(m_aWave,  m_aWave  + DISTORTION_WAVES,  [](const coreObject2D& oWave)  {return oWave .GetAlpha() ? true : false;}) ||
                std::any_of(m_aBurst, m_aBurst + DISTORTION_BURSTS, [](const coreObject2D& oBurst) {return oBurst.GetAlpha() ? true : false;});
    if(m_bActive)
    {
        // 
        m_FrameBuffer.StartDraw();
        m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
        {
            glDisable(GL_DEPTH_TEST);
            {
                // 
                for(coreUintW i = 0u; i < DISTORTION_WAVES; ++i)
                {
                    coreObject2D& oWave = m_aWave[i];
                    if(!oWave.GetAlpha()) continue;

                    // 
                    const coreFloat& fScale = oWave.GetColor4().x;
                    const coreFloat& fSpeed = oWave.GetColor4().y;

                    // 
                    oWave.SetAlpha(MAX(oWave.GetAlpha() - fSpeed * Core::System->GetTime(), 0.0f));
                    oWave.SetSize (coreVector2(0.1f,0.1f) * (ABS(fScale) * ((fScale < 0.0f) ? oWave.GetAlpha() : (1.0f - oWave.GetAlpha()))));
                    oWave.Move();
                    oWave.Render();
                }

                // 
                for(coreUintW i = 0u; i < DISTORTION_BURSTS; ++i)
                {
                    coreObject2D& oBurst = m_aBurst[i];
                    if(!oBurst.GetAlpha()) continue;

                    // 
                    const coreFloat& fScale = oBurst.GetColor4().x;
                    const coreFloat  fSpeed = oBurst.GetColor4().yz().LengthSq();

                    // 
                    oBurst.SetAlpha(MAX(oBurst.GetAlpha() - fSpeed * Core::System->GetTime(), 0.0f));
                    oBurst.SetSize (coreVector2(0.1f,0.1f) * (fScale * (1.0f - oBurst.GetAlpha())));
                    oBurst.Move();
                    oBurst.Render();
                }
            }
            glEnable(GL_DEPTH_TEST);
        }
    }
    else m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);

    // 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


// ****************************************************************
// create animated distortion-wave
void cDistortion::CreateWave(const coreVector3& vPosition, const coreFloat& fScale, const coreFloat& fSpeed)
{
    // 
    if(++m_iCurWave >= DISTORTION_WAVES) m_iCurWave = 0u;
    coreObject2D& oWave = m_aWave[m_iCurWave];

    // 
    oWave.SetPosition(g_pForeground->Project(vPosition) * DISTORTION_SCALE_FACTOR);
    oWave.SetColor4  (coreVector4(fScale, fSpeed, 0.0f, 1.0f));
}


// ****************************************************************
// create animated distortion-burst
void cDistortion::CreateBurst(const coreVector3& vPosition, const coreVector2& vDirection, const coreFloat& fScale, const coreFloat& fSpeed)
{
    ASSERT(vDirection.IsNormalized() && (fScale > 0.0f))

    // 
    if(++m_iCurBurst >= DISTORTION_BURSTS) m_iCurBurst = 0u;
    coreObject2D& oBurst = m_aBurst[m_iCurBurst];

    // 
    oBurst.SetPosition(g_pForeground->Project(vPosition) * DISTORTION_SCALE_FACTOR);
    oBurst.SetColor4  (coreVector4(fScale, vDirection * SQRT(fSpeed), 1.0f));
}


// ****************************************************************
// reset with the resource manager
void cDistortion::__Reset(const coreResourceReset& bInit)
{
    if(bInit) m_FrameBuffer.Create(g_vGameResolution * DISTORTION_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);
         else m_FrameBuffer.Delete();
}