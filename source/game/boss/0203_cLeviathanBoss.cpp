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
cLeviathanBoss::cLeviathanBoss()noexcept
{
    // load models
    this->DefineModelHigh("ship_boss_leviathan_head_high.md3");
    this->DefineModelLow ("ship_boss_leviathan_head_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.5f,1.5f,1.5f)); 

    // configure the boss
    this->Configure(3000, COLOR_SHIP_BLUE); 
}


// ****************************************************************
// 
void cLeviathanBoss::__MoveOwn()
{

}