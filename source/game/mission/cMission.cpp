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
cMission::cMission()noexcept
: m_pCurBoss    (NULL)
, m_iCurBossNum (MISSION_NO_BOSS)
{
    // 
    std::memset(m_apBoss, 0, sizeof(m_apBoss));
}


// ****************************************************************
// 
void cMission::_SetCurBoss(const coreByte& iIndex)
{
    // 
    m_pCurBoss    = (iIndex < MISSION_BOSSES) ? m_apBoss[iIndex] : NULL;
    m_iCurBossNum = (iIndex < MISSION_BOSSES) ? iIndex : MISSION_NO_BOSS;
}