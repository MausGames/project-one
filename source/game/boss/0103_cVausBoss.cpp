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



    m_Bump.DefineModel  ("object_paddle.md3");
    m_Bump.DefineTexture(0u, "effect_energy.png");
    m_Bump.DefineProgram("effect_energy_direct_program");
    m_Bump.SetSize      (coreVector3(3.5f,2.5f,2.5f));
    m_Bump.SetColor3    (coreVector3(0.1f,0.43f,1.0f));
    m_Bump.SetTexSize   (coreVector2(1.2f,0.25f) * 0.5f);

    //g_pOutlineDirect->BindObject(&m_Bump); ## 
    //g_pGlow->BindObject(&m_Bump);


}


// ****************************************************************
// destructor
cVausBoss::~cVausBoss()
{
    //g_pOutlineDirect->UnbindObject(&m_Bump); ## 
    //g_pGlow->UnbindObject(&m_Bump);
}


// ****************************************************************
// 
void cVausBoss::__RenderOwnBefore()
{
}


// ****************************************************************
// 
void cVausBoss::__RenderOwnAfter()
{
    m_Bump.Render();
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

    m_Bump.SetPosition   (coreVector3(this->GetPosition ().xy() + this->GetDirection().xy().Rotate90() * 3.0f, 0.0f));
    m_Bump.SetDirection  (coreVector3(this->GetDirection().xy().Rotate90(), 0.0f));
    m_Bump.SetOrientation(this->GetOrientation());
    m_Bump.SetTexOffset  (coreVector2(0.0f, m_fAnimation * -0.1f));
    m_Bump.Move();

}