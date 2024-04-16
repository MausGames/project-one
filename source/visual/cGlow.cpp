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
: m_Blur    (CORE_GL_SUPPORT(EXT_packed_float) ? CORE_TEXTURE_SPEC_R11F_G11F_B10F : (CORE_GL_SUPPORT(ARB_texture_float) ? CORE_TEXTURE_SPEC_RGB16F : CORE_TEXTURE_SPEC_RGB10_A2), GLOW_SCALE_FACTOR, GLOW_ATTENUATION_FACTOR)
, m_bActive (false)
{
}


// ****************************************************************
// update the glow-effect
void cGlow::Update()
{
    if(!g_CurConfig.Graphics.iGlow || (!TIME && Core::System->GetCurFrame()))
    {
        // 
        m_bActive = false;
        return;
    }

    // create glow only with active objects
    m_bActive = !this->IsEmpty() || g_pSpecialEffects->IsActive();
    if(m_bActive)
    {
        m_Blur.Start();
        {
            // render single objects
            FOR_EACH(it, this->GetObjectSet())
                (*it)->coreObject3D::Render();   // # use low-polygon models

            // render lists with objects
            FOR_EACH(it, this->GetListSet())
                (*it)->Render();

            // render special-effects
            g_pSpecialEffects->Render();
            g_pSpecialEffects->RenderBottom();
        }
        m_Blur.End();
    }
    else m_Blur.Clear();
}