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
cViridoMission::cViridoMission()noexcept
{
    // 
    m_apBoss[0] = &m_Crossfield;
    m_apBoss[1] = &m_Torus;
    m_apBoss[2] = &m_Vaus;

    // 
    g_pEnvironment->SetTargetDirection(coreVector2(0.0f,1.0f));
    g_pEnvironment->SetTargetSide     (coreVector2(0.0f,0.0f));
    g_pEnvironment->SetTargetSpeed    (8.0f);
}


// ****************************************************************
// setup the Virido mission
void cViridoMission::__SetupOwn()
{
    STAGE_MAIN
    {
        // 
        if(STAGE_SUB(0.0f)) m_Crossfield.Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
        if(STAGE_SUB(0.0f)) m_Torus     .Resurrect(coreVector2(0.0f,0.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
        if(STAGE_SUB(0.0f)) m_Vaus      .Resurrect(coreVector2(0.0f,0.5f) * FOREGROUND_AREA, coreVector2(1.0f, 0.0f));

        // 
        if(g_pGame->GetEnemyList()->empty())
            STAGE_FINISH_NOW
    });
}


// ****************************************************************
// move the Virido mission
void cViridoMission::__MoveOwn()
{

}