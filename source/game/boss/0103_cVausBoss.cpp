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
cVausBoss::cVausBoss()noexcept
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModel   ("ship_boss_vaus_high.md3");
    this->DefineModelLow("ship_boss_vaus_low.md3");

    // 
    this->SetSize             (coreVector3(2.5f,2.5f,2.5f));
    //this->SetCollisionModifier(coreVector3(0.8f,0.8f,1.0f));

    // configure the boss
    this->Configure(10000, coreVector3(51.0f/360.0f, 100.0f/100.0f, 85.0f/100.0f).HSVtoRGB());



    m_Paddle.DefineModel  ("object_paddle.md3");
    m_Paddle.DefineTexture(0u, "effect_energy.png");
    m_Paddle.DefineProgram("effect_energy_direct_program");
    m_Paddle.SetSize      (coreVector3(3.5f,2.5f,2.5f));
    m_Paddle.SetColor3    (coreVector3(0.1f,0.43f,1.0f));
    m_Paddle.SetTexSize   (coreVector2(1.2f,0.25f) * 0.5f);



}


// ****************************************************************
// destructor
cVausBoss::~cVausBoss()
{
}


// ****************************************************************
// 
void cVausBoss::__ResurrectOwn()
{
    g_aaOutline[PRIO_STRONG][STYLE_DIRECT].BindObject(&m_Paddle);
    g_pGlow->BindObject(&m_Paddle);
}


// ****************************************************************
// 
void cVausBoss::__KillOwn()
{
    g_aaOutline[PRIO_STRONG][STYLE_DIRECT].UnbindObject(&m_Paddle);
    g_pGlow->UnbindObject(&m_Paddle);
}


// ****************************************************************
// 
void cVausBoss::__RenderOwnWeak()
{
}


// ****************************************************************
// 
void cVausBoss::__RenderOwnStrong()
{
    // 
    glDepthFunc(GL_ALWAYS);
    m_Paddle.Render();
    glDepthFunc(GL_LEQUAL);
}


// ****************************************************************
// 
void cVausBoss::__MoveOwn()
{



    // 
    m_fAnimation.Update(-1.0f);

    //// rotate around z-axis
    //const coreVector2 vDir = coreVector2::Direction(m_fAnimation);
    //this->SetOrientation(coreVector3(0.0f, vDir));

    m_Paddle.SetPosition   (coreVector3(this->GetPosition ().xy() + this->GetDirection().xy().Rotate90() * 3.0f, 0.0f));
    m_Paddle.SetDirection  (coreVector3(this->GetDirection().xy().Rotate90(), 0.0f));
    m_Paddle.SetOrientation(this->GetOrientation());
    m_Paddle.SetTexOffset  (coreVector2(0.0f, m_fAnimation * -0.1f));
    m_Paddle.Move();

}