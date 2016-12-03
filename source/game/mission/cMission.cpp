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
, m_piInt            (NULL)
, m_pfFloat          (NULL)
, m_iIntSize         (0u)
, m_iFloatSize       (0u)
, m_iStageNum        (0u)
, m_fStageWait       (0.0f)
, m_fStageTime       (0.0f)
, m_fStageTimeBefore (0.0f)
{
}


// ****************************************************************
// destructor
cMission::~cMission()
{
    // 
    m_anStage.clear();
    m_apPath .clear();
    m_apSquad.clear();

    // 
    SAFE_DELETE_ARRAY(m_piInt)
    SAFE_DELETE_ARRAY(m_pfFloat)

    // 
    g_pEnvironment->SetTargetDirection(ENVIRONMENT_DEFAULT_DIRECTION);
    g_pEnvironment->SetTargetSide     (ENVIRONMENT_DEFAULT_SIDE);
    g_pEnvironment->SetTargetSpeed    (ENVIRONMENT_DEFAULT_SPEED);
}


// ****************************************************************
// setup the mission
void cMission::Setup()
{
    // 
    ASSERT(m_anStage.empty())
    m_anStage.clear();

    // 
    this->__SetupOwn();

    // 
    m_anStage.reverse();
    STAGE_MAIN
    {
        // begin mission
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
void cMission::RenderUnder () {this->__RenderOwnUnder ();}
void cMission::RenderAttack() {this->__RenderOwnAttack();}
void cMission::RenderOver  () {this->__RenderOwnOver  ();}


// ****************************************************************
// move the mission
void cMission::MoveBefore()
{
    if(!m_anStage.empty())
    {
        if(m_fStageWait > 0.0f)
        {
            // 
            m_fStageWait.Update(-1.0f);
        }
        else
        {
            // 
            m_fStageTimeBefore = m_fStageTime;
            m_fStageTime.Update(1.0f);

            // 
            m_anStage.back()();
            if(m_anStage.empty())
            {

            }
        }
    }

    // 
    this->__MoveOwnBefore();
}

void cMission::MoveAfter()
{
    if(!m_anStage.empty())
    {
        const coreUint16& iCurStageLine = m_anStage.get_keylist().back();

        // 
        FOR_EACH_DYN(it, m_apPath)
        {
            if((*m_apPath.get_key(it)) < iCurStageLine)
                 DYN_REMOVE(it, m_apPath)
            else DYN_KEEP  (it)
        }

        // 
        FOR_EACH_DYN(it, m_apSquad)
        {
            if((*m_apSquad.get_key(it)) < iCurStageLine)
                 DYN_REMOVE(it, m_apSquad)
            else DYN_KEEP  (it)
        }
    }

    // 
    this->__MoveOwnAfter();
}


// ****************************************************************
// 
void cMission::SkipStage()
{
    // 
    m_fStageTime = 0.0f;
    m_anStage.pop_back();

    // 
    std::memset(m_piInt,   0, sizeof(coreInt16) * m_iIntSize);
    std::memset(m_pfFloat, 0, sizeof(coreFloat) * m_iFloatSize);

    // 
    g_pGame->GetShieldManager()->ClearShields(true);
}


// ****************************************************************
// set boss active
void cMission::SetCurBoss(const coreUintW iIndex)
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


// ****************************************************************
// 
#include "01_cViridoMission.cpp"
#include "02_cNevoMission.cpp"
#include "03_cHarenaMission.cpp"
#include "04_cRutilusMission.cpp"
#include "05_cGeluMission.cpp"
#include "06_cCalorMission.cpp"
#include "07_cMuscusMission.cpp"
#include "08_cAterMission.cpp"
#include "99_cIntroMission.cpp"