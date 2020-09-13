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
cInk::cInk()noexcept
: m_vBlotPosition (coreVector4(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX))
, m_vBlotAlpha    (coreVector2(0.0f,0.0f))
, m_fAnimation    (0.0f)
, m_fDelay        (0.0f)
{
    // 
    this->DefineTexture(0u, "menu_background_black.png");
    this->SetSize      (coreVector2(1.0f,1.0f));
    this->SetTexSize   (coreVector2(3.5f,3.5f));
}


// ****************************************************************
// render the ink
void cInk::Render()
{
    if(m_vBlotAlpha.IsNull()) return;

    // enable the shader-program
    if(!this->GetProgram().IsUsable()) return;
    if(!this->GetProgram()->Enable())  return;

    // update all ink uniforms
    coreProgram* pLocal = this->GetProgram().GetResource();
    pLocal->SendUniform("u_v4Position", m_vBlotPosition);
    pLocal->SendUniform("u_v2Alpha",    m_vBlotAlpha * 0.32f + 0.68f);

    // get current animation value
    const coreFloat fAnimation = m_fAnimation * 0.3f;

    if(this->GetStatus())
    {
        coreVector2 avOffset[INK_SAMPLES_LINE];

        // update line ink samples
        for(coreUintW i = 0u; i < INK_SAMPLES_LINE; ++i)
        {
            const coreFloat fPartOff  = I_TO_F(i) / I_TO_F(INK_SAMPLES_LINE);
            const coreFloat fPartTime = FRACT(fAnimation + fPartOff);

            avOffset[i] = coreVector2(0.675f * (0.5f - fPartTime), 0.05f * SIN(fPartTime * (4.0f*PI) + fPartOff * (2.0f*PI))) * 3.0f;
        }

        // merge two samples into one uniform
        for(coreUintW i = 0u; i < INK_SAMPLES_LINE/2u; ++i)
        {
            pLocal->SendUniform(PRINT("u_av4Offset[%zu]", i), r_cast<coreVector4*>(avOffset)[i]);
        }
    }
    else
    {
        // update point ink samples
        for(coreUintW i = 0u; i < INK_SAMPLES_POINT; ++i)
        {
            const coreFloat fPartOff  = I_TO_F(i) / I_TO_F(INK_SAMPLES_POINT);
            const coreFloat fPartTime = FRACT(fAnimation + fPartOff);

            pLocal->SendUniform(PRINT("u_av2Offset[%zu]", i), coreVector2::Direction(fPartTime * (4.0f*PI)) * 0.14f);
        }
    }

    // 
    this->coreFullscreen::Render();
}


// ****************************************************************
// move the ink
void cInk::Move()
{
    if(m_vBlotAlpha.IsNull()) return;

    // 
    m_fAnimation.UpdateMod(0.1f, 10.0f);

    // 
    if(m_fDelay) m_vBlotAlpha = (m_vBlotAlpha - m_fDelay * Core::System->GetTime()).Processed(MAX, 0.0f);

    // 
    this->SetTexOffset(coreVector2(m_fAnimation * -0.4, 0.0));
    this->coreFullscreen::Move();
}


// ****************************************************************
// 
void cInk::Enable(const coreBool bLine)
{
    // 
    this->DefineProgram(bLine ? "effect_ink_line_program" : "effect_ink_point_program");
    this->SetStatus    (bLine ? 1 : 0);

    // 
    WARN_IF(!m_vBlotAlpha.IsNull()) m_vBlotAlpha = coreVector2(0.0f,0.0f);

    // 
    m_fDelay = 0.0f;
}


// ****************************************************************
// 
void cInk::Disable(const coreFloat fDelay)
{
    // 
    if(!fDelay) m_vBlotAlpha = coreVector2(0.0f,0.0f);

    // 
    m_fDelay = fDelay;
}


// ****************************************************************
// 
void cInk::SetBlotPosition(const coreUintW iIndex, const coreVector2& vPosition)
{
    // 
    const coreVector2 vProjectedPos = g_pForeground->Project2D(coreVector3(vPosition, 0.0f));

    // 
    m_vBlotPosition.arr(iIndex * 2u)      = vProjectedPos.x;
    m_vBlotPosition.arr(iIndex * 2u + 1u) = vProjectedPos.y;
}


// ****************************************************************
// 
void cInk::SetBlotAlpha(const coreUintW iIndex, coreFloat fAlpha)
{
    // 
    m_vBlotAlpha.arr(iIndex) = fAlpha;
}