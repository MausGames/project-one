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
cGlow::cGlow()noexcept
: m_Blur (CORE_GL_SUPPORT(EXT_packed_float) ? CORE_TEXTURE_SPEC_R11F_G11F_B10F : (CORE_GL_SUPPORT(ARB_texture_float) ? CORE_TEXTURE_SPEC_RGB16F : CORE_TEXTURE_SPEC_RGB10_A2), GLOW_SCALE_FACTOR, GLOW_ATTENUATION_FACTOR)
{
}


// ****************************************************************
// update the glow-effect
void cGlow::Update()
{
    if(!g_CurConfig.Graphics.iGlow) return;
    if(!Core::System->GetTime())    return;

    // create glow only with active foreground
    if(STATIC_ISVALID(g_pGame) || g_pTheater->IsActive())
    {
        m_Blur.Start();
        {
            // render single objects
            FOR_EACH(it, this->GetObjectSet())
                (*it)->Render((*it)->GetProgram());   // # use low-polygon models

            // render lists with objects
            FOR_EACH(it, this->GetListSet())
                (*it)->Render();

            // always render special-effects
            g_pSpecialEffects->Render(false);
        }
        m_Blur.End();
    }
    else m_Blur.Clear();
}