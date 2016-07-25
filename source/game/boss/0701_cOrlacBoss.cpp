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
// constructor
cOrlacBoss::cOrlacBoss()noexcept
{
    // load models
    this->DefineModelHigh("ship_projectone.md3"); 
    this->DefineModelLow ("ship_projectone.md3"); 

    // set object properties
    this->SetSize             (coreVector3(1.0f,1.0f,1.0f)); 
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f)); 

    // configure the boss
    this->Configure(10000, COLOR_SHIP_GREY); 
}


// ****************************************************************
// 
void cOrlacBoss::__MoveOwn()
{

}