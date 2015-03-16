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

    // 
    this->SetSize(coreVector3(3.0f,3.0f,3.0f));

    // configure the boss
    this->Configure(10000, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// 
void cCrossfieldBoss::__RenderOwnBefore()
{

}


// ****************************************************************
// 
void cCrossfieldBoss::__MoveOwn()
{
    // 
    if(m_iPhase == 0)
    {
        // 
        coreSpline2 oSpline;
        oSpline.AddNode(coreVector2(-140.0f,-10.0f), coreVector2( 1.0f, 0.0f));
        oSpline.AddNode(coreVector2(  10.0f, 10.0f), coreVector2(-1.0f,-1.0f).Normalize());
        oSpline.AddNode(coreVector2( -30.0f,  0.0f), coreVector2(-1.0f, 0.0f));
    }

    // 
    else if(m_iPhase == 1)
    {

    }

    // 
    else if(m_iPhase == 2)
    {

    }

}