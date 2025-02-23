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
cDistortion::cDistortion()noexcept
: m_iCurWave   (0u)
, m_iCurBurst  (0u)
, m_iCurEraser (0u)
, m_bActive    (false)
{
    const coreTextureSpec oSpec = (CORE_GL_SUPPORT(EXT_texture_norm16) && CORE_GL_SUPPORT(ARB_texture_rg)) ? CORE_TEXTURE_SPEC_RG16 : (CORE_GL_SUPPORT(EXT_texture_type_2_10_10_10_rev) ? CORE_TEXTURE_SPEC_RGB10_A2 : CORE_TEXTURE_SPEC_RGB8);

    // create distortion frame buffer
    m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, oSpec, CORE_TEXTURE_MODE_DEFAULT);
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

    // 
    for(coreUintW i = 0u; i < DISTORTION_ERASERS; ++i)
    {
        m_aEraser[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aEraser[i].DefineProgram("menu_single_program");
        m_aEraser[i].SetColor3    (coreVector3(DISTORTION_CLEAR, DISTORTION_CLEAR, DISTORTION_CLEAR));
        m_aEraser[i].SetAlpha     (0.0f);
    }
}


// ****************************************************************
// update the distortion-effect
void cDistortion::Update()
{
    if(!g_CurConfig.Graphics.iDistortion)
    {
        // 
        glClearColor(DISTORTION_CLEAR, DISTORTION_CLEAR, DISTORTION_CLEAR, DISTORTION_CLEAR);
        m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // 
        m_bActive = false;
        return;
    }

    // 
    glClearColor(DISTORTION_CLEAR, DISTORTION_CLEAR, DISTORTION_CLEAR, DISTORTION_CLEAR);

    // create distortion only with active objects
    m_bActive = (std::any_of(m_aWave,  m_aWave  + DISTORTION_WAVES,  [](const coreObject2D& oWave)  {return oWave .GetAlpha() ? true : false;}) ||
                 std::any_of(m_aBurst, m_aBurst + DISTORTION_BURSTS, [](const coreObject2D& oBurst) {return oBurst.GetAlpha() ? true : false;})) && (g_fShiftMode < 1.0f);
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
                    oWave.Render();
                }

                // 
                for(coreUintW i = 0u; i < DISTORTION_BURSTS; ++i)
                {
                    coreObject2D& oBurst = m_aBurst[i];
                    if(!oBurst.GetAlpha()) continue;

                    // 
                    oBurst.Render();
                }

                // 
                for(coreUintW i = 0u; i < DISTORTION_ERASERS; ++i)
                {
                    coreObject2D& oEraser = m_aEraser[i];
                    if(!oEraser.GetAlpha()) continue;

                    // 
                    oEraser.Render();
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
// 
void cDistortion::Move()
{
    // 
    for(coreUintW i = 0u; i < DISTORTION_WAVES; ++i)
    {
        coreObject2D& oWave = m_aWave[i];
        if(!oWave.GetAlpha()) continue;

        // 
        const coreFloat fScale = oWave.GetColor4().x;
        const coreFloat fSpeed = oWave.GetColor4().y;

        // 
        oWave.SetAlpha(MAX0(oWave.GetAlpha() - fSpeed * TIME));
        oWave.SetSize (coreVector2(1.0f,1.0f) * (ABS(fScale) * ((fScale < 0.0f) ? oWave.GetAlpha() : (1.0f - oWave.GetAlpha()))));
        oWave.Move();
    }

    // 
    for(coreUintW i = 0u; i < DISTORTION_BURSTS; ++i)
    {
        coreObject2D& oBurst = m_aBurst[i];
        if(!oBurst.GetAlpha()) continue;

        // 
        const coreFloat fScale = oBurst.GetColor4().x;
        const coreFloat fSpeed = oBurst.GetColor4().yz().LengthSq();

        // 
        oBurst.SetAlpha(MAX0(oBurst.GetAlpha() - fSpeed * TIME));
        oBurst.SetSize (coreVector2(1.0f,1.0f) * (fScale * (1.0f - oBurst.GetAlpha())));
        oBurst.Move();
    }

    // 
    for(coreUintW i = 0u; i < DISTORTION_ERASERS; ++i)
    {
        coreObject2D& oEraser = m_aEraser[i];
        if(!oEraser.GetAlpha()) continue;

        // 
        oEraser.SetAlpha(0.0f);
    }
}


// ****************************************************************
// create animated distortion-wave
void cDistortion::CreateWave(const coreVector3 vPosition, const coreFloat fScale, const coreFloat fSpeed)
{
    // 
    if(++m_iCurWave >= DISTORTION_WAVES) m_iCurWave = 0u;
    coreObject2D& oWave = m_aWave[m_iCurWave];

    // 
    oWave.SetPosition(g_pForeground->Project2D(vPosition) * DISTORTION_SCALE_FACTOR);
    oWave.SetSize    (coreVector2(0.0f,0.0f));
    oWave.SetColor4  (coreVector4(0.2f * fScale * DISTORTION_SCALE_FACTOR, fSpeed, 0.0f, 1.0f));
    oWave.Move();
}


// ****************************************************************
// create animated distortion-burst
void cDistortion::CreateBurst(const coreVector3 vPosition, const coreVector2 vDirection, const coreFloat fScale, const coreFloat fSpeed)
{
    ASSERT(vDirection.IsNormalized() && (fScale > 0.0f))

    // 
    if(++m_iCurBurst >= DISTORTION_BURSTS) m_iCurBurst = 0u;
    coreObject2D& oBurst = m_aBurst[m_iCurBurst];

    // 
    oBurst.SetPosition(g_pForeground->Project2D(vPosition) * DISTORTION_SCALE_FACTOR);
    oBurst.SetSize    (coreVector2(0.0f,0.0f));
    oBurst.SetColor4  (coreVector4(0.2f * fScale * DISTORTION_SCALE_FACTOR, vDirection * SQRT(fSpeed), 1.0f));
    oBurst.Move();
}


// ****************************************************************
// 
void cDistortion::CreateEraser(const coreVector3 vPosition, const coreFloat fScale)
{
    // 
    if(++m_iCurEraser >= DISTORTION_ERASERS) m_iCurEraser = 0u;
    coreObject2D& oEraser = m_aEraser[m_iCurEraser];

    // 
    oEraser.SetPosition(g_pForeground->Project2D(vPosition) * DISTORTION_SCALE_FACTOR);
    oEraser.SetSize    (coreVector2(0.2f,0.2f) * fScale * DISTORTION_SCALE_FACTOR);
    oEraser.SetAlpha   (1.0f);
    oEraser.Move();
}


// ****************************************************************
// reset with the resource manager
void cDistortion::__Reset(const coreResourceReset eInit)
{
    if(eInit) m_FrameBuffer.Create(g_vGameResolution * DISTORTION_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);
         else m_FrameBuffer.Delete();
}