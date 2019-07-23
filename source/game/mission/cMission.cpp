///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreUint16  cMission::s_iTick           = 0u;
coreFloat   cMission::s_fLifeTimePoint  = 0.0f;
coreFloat   cMission::s_fHealthPctPoint = 0.0f;
coreVector2 cMission::s_vPositionPoint  = coreVector2(0.0f,0.0f);


// ****************************************************************
// constructor
cMission::cMission()noexcept
: m_apBoss           {}
, m_pCurBoss         (NULL)
, m_iCurBossIndex    (MISSION_NO_BOSS)
, m_iCurWaveCount    (0u)
, m_iCurWaveIndex    (MISSION_NO_WAVE)
, m_piData           (NULL)
, m_iDataSize        (0u)
, m_iStageNum        (0u)
, m_fStageTime       (0.0f)
, m_fStageTimeBefore (0.0f)
, m_iStageSub        (0u)
{
}


// ****************************************************************
// destructor
cMission::~cMission()
{
    // 
    FOR_EACH(it, m_apPath)  SAFE_DELETE(*it)
    FOR_EACH(it, m_apSquad) SAFE_DELETE(*it)

    // 
    m_anStage.clear();
    m_apPath .clear();
    m_apSquad.clear();

    // 
    ZERO_DELETE(m_piData)
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
    ASSERT(!m_anStage.empty())
    m_anStage.reverse();

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
        // 
        m_fStageTimeBefore = m_fStageTime;
        m_fStageTime.Update(1.0f);

        if(m_fStageTime > 0.0f)
        {
            // 
            m_anStage.back()();
            if(m_anStage.empty())
            {
                // 
                g_pGame->StartOutro(0u);
            }
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
// 
void cMission::SkipStage()
{
    // 
    m_fStageTime       = -MISSION_STAGE_DELAY;
    m_fStageTimeBefore = -MISSION_STAGE_DELAY;
    m_iStageSub        = 0u;
    m_anStage.pop_back();

    // 
    FOR_EACH(it, m_apPath)  SAFE_DELETE(*it)
    FOR_EACH(it, m_apSquad) SAFE_DELETE(*it)

    // 
    m_apPath .clear();
    m_apSquad.clear();

    // 
    std::memset(m_piData, 0, sizeof(coreUint32) * m_iDataSize);

    // 
    g_pGame->GetShieldManager()->ClearShields(true);
}


// ****************************************************************
// 
void cMission::ActivateBoss(const cBoss* pBoss)
{
    // 
    for(coreUintW i = 0u; i < MISSION_BOSSES; ++i)
    {
        if(pBoss == m_apBoss[i])
        {
            // save pointer and index for direct access
            m_pCurBoss      = m_apBoss[i];
            m_iCurBossIndex = i;
            return;
        }
    }

    ASSERT(false)
}

void cMission::DeactivateBoss()
{
    // 
    m_pCurBoss      = NULL;
    m_iCurBossIndex = MISSION_NO_BOSS;
}


// ****************************************************************
// 
void cMission::ActivateWave()
{
    // 
    ASSERT(m_iCurWaveCount < MISSION_WAVES)
    m_iCurWaveIndex = m_iCurWaveCount++;
}

void cMission::DeactivateWave()
{
    // 
    m_iCurWaveIndex = MISSION_NO_WAVE;
}


// ****************************************************************
// 
UNITY_BUILD
#include "01_cViridoMission.cpp"
#include "02_cNevoMission.cpp"
#include "03_cHarenaMission.cpp"
#include "04_cRutilusMission.cpp"
#include "05_cGeluMission.cpp"
#include "06_cCalorMission.cpp"
#include "07_cMuscusMission.cpp"
#include "08_cAterMission.cpp"
#include "99_cIntroMission.cpp"