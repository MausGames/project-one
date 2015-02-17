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
cMellanMission::cMellanMission()noexcept
{
    // 
    m_apBoss[0] = &m_Crossfield;
    m_apBoss[1] = NULL;
    m_apBoss[2] = NULL;



    m_Crossfield.SetPosition(coreVector3(0.0f,15.0f,0.0f));

    for(int i = 0; i < ARRAY_SIZE(m_aScout); ++i)
        m_aScout[i].SetPosition(coreVector3((-27.0f+i*6.0f)*1.2f,0.0f,0.0f));
}