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
// counter identifier


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cProjectOneBoss::cProjectOneBoss()noexcept
{
    // load object resources
    this->DefineModelHigh("ship_projectone.md3");
    this->DefineModelLow ("ship_projectone.md3");
    this->DefineTexture  (0u, "ship_player.png");

    // set object properties
    this->SetSize(coreVector3(1.2f,1.2f,1.2f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_GREY);
}


// ****************************************************************
// 
void cProjectOneBoss::__ResurrectOwn()
{

}


// ****************************************************************
// 
void cProjectOneBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveOwn()
{
    // ################################################################
    // ################################################################
}