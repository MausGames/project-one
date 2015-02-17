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
cCrossfieldBoss::cCrossfieldBoss()noexcept
{
    // load models
    this->DefineModel   ("ship_boss_crossfield_high.md3");
    this->DefineModelLow("ship_boss_crossfield_low.md3");

    // set color value
    this->SetColor3(coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB());
    m_iBaseColor = this->GetColor4().PackUnorm4x8();

    // 
    this->SetSize(coreVector3(3.0f,3.0f,3.0f));

    // 
    m_iMaxHealth = m_iCurHealth = 1000;
}


// ****************************************************************
// 
void cCrossfieldBoss::__RenderOwn()
{

}


// ****************************************************************
// 
void cCrossfieldBoss::__MoveOwn()
{

}