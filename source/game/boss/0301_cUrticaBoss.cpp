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
// counter identifier


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cUrticaBoss::cUrticaBoss()noexcept
{
    // load models
    this->DefineModelHigh("ship_projectone.md3"); 
    this->DefineModelLow ("ship_projectone.md3"); 

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f)); 

    // configure the boss
    this->Configure(10000, COLOR_SHIP_GREY); 
}


// ****************************************************************
// 
void cUrticaBoss::__MoveOwn()
{

    
    
    
    
    
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, this, [](cPlayer* OUTPUT pPlayer, cEnemy* OUTPUT pEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        pPlayer->TakeDamage(15, ELEMENT_NEUTRAL, vIntersection.xy());
    });
}