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
, m_fStageTime       (0.0f)
, m_fStageTimeBefore (0.0f)
{
    // reset boss pointers
    std::memset(m_apBoss, 0, sizeof(m_apBoss));
}


// ****************************************************************
// setup the mission
void cMission::Setup()
{
    ASSERT(m_anStage.empty())
    m_anStage.clear();

    // 
    this->__SetupOwn();

    // 
    std::reverse(m_anStage.begin(), m_anStage.end());
    STAGE_MAIN
    {
        // 
        if(g_pGame->GetTimeMission() >= 0.0f)
            STAGE_FINISH_NOW
    });

#if defined(_CORE_DEBUG_)

    // 
    for(coreUintW i = 0u; i < MISSION_BOSSES; ++i)
        ASSERT(m_apBoss[i])

#endif
}


// ****************************************************************
// move the mission
void cMission::MoveBefore()
{
    if(!m_anStage.empty())
    {
        // 
        m_fStageTimeBefore = m_fStageTime;
        m_fStageTime.Update(1.0f);

        // 
        m_anStage.back()();
        if(m_anStage.empty())
        {
            // TODO # end mission? 
        }
    }

    // 
    this->__MoveOwnBefore();
}

void cMission::MoveAfter()
{
    // 
    this->__MoveOwnAfter();
}


// ****************************************************************
// set boss active
void cMission::SetCurBoss(const coreUintW& iIndex)
{
    // save pointer and index for direct access
    m_pCurBoss      = (iIndex < MISSION_BOSSES) ? m_apBoss[iIndex] : NULL;
    m_iCurBossIndex = (iIndex < MISSION_BOSSES) ? iIndex           : MISSION_NO_BOSS;
}

void cMission::SetCurBoss(const cBoss* pBoss)
{
    // 
    for(coreUintW i = 0u; i < MISSION_BOSSES; ++i)
    {
        if(pBoss == m_apBoss[i])
        {
            // 
            this->SetCurBoss(i);
            return;
        }
    }

    ASSERT(false)
}