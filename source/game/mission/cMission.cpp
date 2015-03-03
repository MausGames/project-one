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
: m_pCurBoss      (NULL)
, m_iCurBossIndex (MISSION_NO_BOSS)
{
    // reset boss pointers
    std::memset(m_apBoss, 0, sizeof(m_apBoss));
}


// ****************************************************************
// set boss active
void cMission::_SetCurBoss(const coreByte& iIndex)
{
    // save pointer and index for direct access
    m_pCurBoss      = (iIndex < MISSION_BOSSES) ? m_apBoss[iIndex] : NULL;
    m_iCurBossIndex = (iIndex < MISSION_BOSSES) ? iIndex           : MISSION_NO_BOSS;
}