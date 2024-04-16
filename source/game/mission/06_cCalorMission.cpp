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
cCalorMission::cCalorMission()noexcept
{
    // 
    m_apBoss[0] = &m_Fenrir;
    m_apBoss[1] = &m_Shelob;
    m_apBoss[2] = &m_Zeroth;
}


// ****************************************************************
// destructor
cCalorMission::~cCalorMission()
{
    // 
    m_Snow.Disable(0.0f);
}


// ****************************************************************
// 
void cCalorMission::__RenderOwnUnder()
{
    // 
    m_Snow.Render();   // TODO: disable depth-test (full), but would overdraw ships then, maybe just write    
}


// ****************************************************************
// 
void cCalorMission::__MoveOwnAfter()
{
    // 
    m_Snow.Move();
}