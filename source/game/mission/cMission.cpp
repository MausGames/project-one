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
: m_apBoss           {}
, m_pCurBoss         (NULL)
, m_iCurBossIndex    (MISSION_NO_BOSS)
, m_iStageNum        (0u)
, m_fStageTime       (0.0f)
, m_fStageTimeBefore (0.0f)
{
}


// ****************************************************************
// setup the mission
void cMission::Setup()
{
    // 
    ASSERT(m_anStage.empty())
    m_anStage    .clear();
    m_aiStageLine.clear();

    // 
    this->__SetupOwn();

    // 
    std::reverse(m_anStage.begin(), m_anStage.end());
    STAGE_MAIN
    {
        // begin mission
        if(g_pGame->GetTimeMission() >= 0.0f)
            STAGE_FINISH_NOW
    });

    // 
    m_iStageNum = m_anStage.size();

#if defined(_CORE_DEBUG_)

    // 
    for(coreUintW i = 0u; i < MISSION_BOSSES; ++i)
        ASSERT(m_apBoss[i])

#endif
}


// ****************************************************************
// render the mission
void cMission::RenderWeak()
{
    // 
    this->__RenderOwnWeak();
}

void cMission::RenderStrong()
{
    // 
    this->__RenderOwnStrong();
}

void cMission::RenderAfter()
{
    // 
    this->__RenderOwnAfter();
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
    ASSERT(m_anStage.size() == m_aiStageLine.size())

    // 
    this->__MoveOwnBefore();
}

void cMission::MoveAfter()
{
    FOR_EACH_DYN(it, m_apPath)
    {
        // 
        if(it->unique() && (*m_apPath.get_key(it) < m_aiStageLine.back()))
             DYN_REMOVE(it, m_apPath)
        else DYN_KEEP  (it)
    }

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