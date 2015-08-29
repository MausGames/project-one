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
cGlow::cGlow()noexcept
: m_Blur (CORE_GL_SUPPORT(ARB_texture_float) ? CORE_TEXTURE_SPEC_RGB_16F : CORE_TEXTURE_SPEC_RGB, GLOW_SCALE_FACTOR, GLOW_ATTENUATION_FACTOR)
{
}


// ****************************************************************
// update the glow-effect
void cGlow::Update()
{
    if(!g_CurConfig.Graphics.iGlow) return;
    if(!Core::System->GetTime())    return;

    // create glow only with active game
    if(g_pGame)
    {
        m_Blur.Start();
        {
            // draw single objects
            FOR_EACH(it, this->GetObjectSet())
                (*it)->Render();

            // draw lists with objects
            FOR_EACH(it, this->GetListSet())
                (*it)->Render();

            // always draw special-effects
            g_pSpecialEffects->Render();
        }
        m_Blur.End();
    }
    else m_Blur.Clear();
}