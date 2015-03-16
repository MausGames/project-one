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
: m_pCurBoss         (NULL)
, m_iCurBossIndex    (MISSION_NO_BOSS)
, m_fPhaseTime       (0.0f)
, m_fPhaseTimeBefore (0.0f)
{
    // reset boss pointers
    std::memset(m_apBoss, 0, sizeof(m_apBoss));
}


// ****************************************************************
// setup the mission
void cMission::Setup()
{
    ASSERT(m_anPhase.empty())
    m_anPhase.clear();

    // 
    this->__SetupOwn();

    // 
    std::reverse(m_anPhase.begin(), m_anPhase.end());
    PHASE_MAIN
    {
        // 
        if(g_pGame->GetTimeMission() >= 0.0f)
            PHASE_END_NOW
    });

#if defined(_CORE_DEBUG_)

    // 
    for(coreUintW i = 0; i < MISSION_BOSSES; ++i)
        ASSERT(m_apBoss[i])

#endif
}


// ****************************************************************
// move the mission
void cMission::Move()
{
    if(m_anPhase.empty()) return;

    // 
    m_fPhaseTimeBefore = m_fPhaseTime;
    m_fPhaseTime.Update(1.0f);

    // 
    m_anPhase.back()();
    if(m_anPhase.empty())
    {
        // TODO # end mission? 
    }

    // call individual move routine
    this->__MoveOwn();
}


// ****************************************************************
// set boss active
void cMission::_SetCurBoss(const coreUintW& iIndex)
{
    // save pointer and index for direct access
    m_pCurBoss      = (iIndex < MISSION_BOSSES) ? m_apBoss[iIndex] : NULL;
    m_iCurBossIndex = (iIndex < MISSION_BOSSES) ? iIndex           : MISSION_NO_BOSS;
}

void cMission::_SetCurBoss(const cBoss* pBoss)
{
    // 
    for(coreUintW i = 0; i < MISSION_BOSSES; ++i)
        if(pBoss == m_apBoss[i]) this->_SetCurBoss(i);
}