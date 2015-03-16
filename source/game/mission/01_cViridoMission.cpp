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
    m_apBoss[1] = &m_Crossfield;
    m_apBoss[2] = &m_Crossfield;




    m_Spline.AddNode(coreVector2(0.0f, 2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
    m_Spline.AddNode(coreVector2(0.0f,-2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
}


// ****************************************************************
// setup the Virido mission
void cViridoMission::__SetupOwn()
{
    PHASE_MAIN
    {
             if(PHASE_SUB(0.0f)) {m_aScout[0].SetupTrack(&m_Spline, coreVector2( 0.0f,0.0f));                   m_aScout[0].AddStatus(ENEMY_STATUS_SILENT); m_aScout[0].Resurrect();}
        else if(PHASE_SUB(0.3f)) {m_aScout[1].SetupTrack(&m_Spline, coreVector2(-0.2f,0.0f) * FOREGROUND_AREA); m_aScout[1].AddStatus(ENEMY_STATUS_SILENT); m_aScout[1].Resurrect();}
        else if(PHASE_SUB(0.6f)) {m_aScout[2].SetupTrack(&m_Spline, coreVector2(-0.4f,0.0f) * FOREGROUND_AREA); /*m_aScout[2].AddStatus(ENEMY_STATUS_SILENT);*/ m_aScout[2].Resurrect();}
        
        else if(PHASE_SUB(1.5f)) {m_aScout[3].SetupTrack(&m_Spline, coreVector2(-0.6f,0.0f) * FOREGROUND_AREA); m_aScout[3].AddStatus(ENEMY_STATUS_SILENT); m_aScout[3].Resurrect();}
        else if(PHASE_SUB(1.8f)) {m_aScout[4].SetupTrack(&m_Spline, coreVector2(-0.4f,0.0f) * FOREGROUND_AREA); m_aScout[4].AddStatus(ENEMY_STATUS_SILENT); m_aScout[4].Resurrect();}
        else if(PHASE_SUB(2.1f)) {m_aScout[5].SetupTrack(&m_Spline, coreVector2(-0.2f,0.0f) * FOREGROUND_AREA); /*m_aScout[5].AddStatus(ENEMY_STATUS_SILENT);*/ m_aScout[5].Resurrect();}
        
        else if(PHASE_SUB(3.0f)) {m_aScout[6].SetupTrack(&m_Spline, coreVector2( 0.0f,0.0f) * FOREGROUND_AREA); m_aScout[6].AddStatus(ENEMY_STATUS_SILENT); m_aScout[6].Resurrect();}
        else if(PHASE_SUB(3.3f)) {m_aScout[7].SetupTrack(&m_Spline, coreVector2( 0.2f,0.0f) * FOREGROUND_AREA); m_aScout[7].AddStatus(ENEMY_STATUS_SILENT); m_aScout[7].Resurrect();}
        else if(PHASE_SUB(3.6f)) {m_aScout[8].SetupTrack(&m_Spline, coreVector2( 0.4f,0.0f) * FOREGROUND_AREA); /*m_aScout[8].AddStatus(ENEMY_STATUS_SILENT);*/ m_aScout[8].Resurrect();}

        if(g_pGame->GetEnemyList()->empty())
            PHASE_END_NOW
    });

    PHASE_MAIN
    {
        if(PHASE_SUB(0.0f))
        {
            m_Crossfield.Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
            g_pGame->GetInterface()->ShowBoss(&m_Crossfield);
            this->_SetCurBoss(&m_Crossfield);
        }

        if(g_pGame->GetEnemyList()->empty())
            PHASE_END_NOW
    });
}


// ****************************************************************
// move the Virido mission
void cViridoMission::__MoveOwn()
{

}