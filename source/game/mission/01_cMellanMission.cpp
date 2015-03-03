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



    m_Crossfield.Resurrect(coreVector2(0.0f,0.65f) * FOREGROUND_AREA);
    //this->_SetCurBoss(0);
    for(int i = 0; i < ARRAY_SIZE(m_aScout); ++i)
    {
        m_aScout[i].Resurrect(coreVector2((-27.0f+i*6.0f)*1.2f,0.0f));
        //m_aScout[i].SetSize(m_aScout[i].GetSize() * 10.0f);
    }



    //m_aScout[2].Configure(200, coreVector3(118.0f/360.0f, 54.0f/100.0f, 66.0f/100.0f).HSVtoRGB());
    //m_aScout[2].SetSize(m_aScout[2].GetSize() * 1.1f);
}