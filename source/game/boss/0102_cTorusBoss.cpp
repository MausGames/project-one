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
cTorusBoss::cTorusBoss()noexcept
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModel   ("ship_boss_torus_high.md3");
    this->DefineModelLow("ship_boss_torus_low.md3");

    // 
    this->SetSize             (coreVector3(1.7f,1.7f,1.7f));
    this->SetCollisionModifier(coreVector3(0.8f,0.8f,1.0f));

    // configure the boss
    this->Configure(10000, coreVector3(0.0f/360.0f, 0.0f/100.0f, 60.0f/100.0f).HSVtoRGB());




    for(coreUintW i = 0u; i < 2u; ++i)
    {
        m_aCircle[i].DefineModel  ("object_circle_02.md3");
        m_aCircle[i].DefineTexture(0u, "effect_energy.png");
        m_aCircle[i].DefineProgram("effect_energy_invert_program");
        m_aCircle[i].SetSize      (this->GetSize());
        m_aCircle[i].SetColor3    (coreVector3(0.3f,0.7f,0.3f) * 0.8f);
        m_aCircle[i].SetTexSize   (coreVector2(1.25f,1.25f));
    }

}


// ****************************************************************
// destructor
cTorusBoss::~cTorusBoss()
{
}


// ****************************************************************
// 
void cTorusBoss::__ResurrectOwn()
{
    for(coreUintW i = 0u; i < 2u; ++i)
        g_pGlow->BindObject(&m_aCircle[i]);
}


// ****************************************************************
// 
void cTorusBoss::__KillOwn()
{
    for(coreUintW i = 0u; i < 2u; ++i)
        g_pGlow->UnbindObject(&m_aCircle[i]);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnWeak()
{
    glDepthFunc(GL_ALWAYS);
    {
        // 
        for(coreUintW i = 0u; i < 2u; ++i)
            m_aCircle[i].Render();
    }
    glDepthFunc(GL_LEQUAL);
}


// ****************************************************************
// 
void cTorusBoss::__RenderOwnStrong()
{
}


// ****************************************************************
// 
void cTorusBoss::__MoveOwn()
{




    // 
    m_fAnimation.Update(-1.0f);

    // rotate around z-axis
    const coreVector2 vDir = coreVector2::Direction(m_fAnimation * 1.2f);
    this->SetDirection(coreVector3(vDir, 0.0f));



        //const coreVector2 vDir = coreVector2::Direction(m_fAnimation);
        //this->SetDirection  (-coreVector3(vDir, 0.0f));
        //this->SetOrientation(-coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));



    const coreFloat fFade = 1.0f - FRACT(ABS(m_fAnimation));
    m_aCircle[1].SetSize(this->GetSize() * (0.5f + 0.5f*fFade));
    m_aCircle[1].SetAlpha(1.0f * fFade);

    for(coreUintW i = 0u; i < 2u; ++i)
    {
        m_aCircle[i].SetPosition   (this->GetPosition   ());
        m_aCircle[i].SetDirection  (this->GetDirection  ());
        m_aCircle[i].SetOrientation(this->GetOrientation());
        m_aCircle[i].SetTexOffset  (coreVector2(m_fAnimation * 0.1f, I_TO_F(i)*0.5f));
        m_aCircle[i].Move();
    }
}