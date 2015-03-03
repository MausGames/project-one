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
{
    // create distortion frame buffer
    m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0, CORE_TEXTURE_SPEC_RGB);
    m_iFrameBuffer.Create(g_vGameResolution * DISTORTION_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);
}


// ****************************************************************
// update the distortion-effect
void cDistortion::Update()
{
    if(!g_CurConfig.Graphics.iDistortion) return;

    // 
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// reset with the resource manager
void cDistortion::__Reset(const coreResourceReset& bInit)
{
    if(bInit) m_iFrameBuffer.Create(g_vGameResolution * DISTORTION_SCALE_FACTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);
         else m_iFrameBuffer.Delete();
}