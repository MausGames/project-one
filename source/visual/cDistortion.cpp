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
: m_iCurWave (0u)
{
    // create distortion frame buffer
    m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RG);
    m_iFrameBuffer.Create(g_vGameResolution * DISTORTION_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    for(coreUintW i = 0u; i < DISTORTION_WAVES; ++i)
    {
        m_aWave[i].DefineTexture(0u, "effect_wave_norm.png");
        m_aWave[i].DefineProgram("effect_distortion_program");
        m_aWave[i].SetAlpha     (0.0f);
    }
}


// ****************************************************************
// update the distortion-effect
void cDistortion::Update()
{
    if(!g_CurConfig.Graphics.iDistortion) return;

    // 
    glClearColor(0.5f, 0.5f, 0.0f, 0.0f);

    // create distortion only with active waves
    if(std::any_of(m_aWave, m_aWave + DISTORTION_WAVES, [](const coreObject2D& oWave) {return oWave.GetAlpha() ? true : false;}))
    {
        // 
        m_iFrameBuffer.StartDraw();
        m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
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
            }
            glEnable(GL_DEPTH_TEST);
        }
    }
    else m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);

    // 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}


// ****************************************************************
// 
void cDistortion::CreateWave(const coreVector3& vPosition, const coreFloat& fScale, const coreFloat& fSpeed)
{
    // 
    if(++m_iCurWave >= DISTORTION_WAVES) m_iCurWave = 0u;
    coreObject2D& oWave = m_aWave[m_iCurWave];

    // 
    const coreVector4 vProjection = coreVector4(vPosition, 1.0f) * g_pForeground->GetViewProj();

    // 
    oWave.SetPosition(vProjection.xy() * (RCP(vProjection.w) * 0.5f * DISTORTION_SCALE_FACTOR));
    oWave.SetSize    (coreVector2(1.0f,1.0f) * MAX(fScale, 0.0f));
    oWave.SetColor4  (coreVector4(fScale, fSpeed, 0.0f, 1.0f));
}


// ****************************************************************
// reset with the resource manager
void cDistortion::__Reset(const coreResourceReset& bInit)
{
    if(bInit) m_iFrameBuffer.Create(g_vGameResolution * DISTORTION_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);
         else m_iFrameBuffer.Delete();
}