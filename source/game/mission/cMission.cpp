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
, m_iCurSegmentIndex (MISSION_NO_SEGMENT)
, m_piData           (NULL)
, m_iDataSize        (0u)
, m_iStageNum        (0u)
, m_fStageTime       (0.0f)
, m_fStageTimeBefore (0.0f)
, m_iStageSub        (0u)
, m_pfMedalGoal      (NULL)
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
}


// ****************************************************************
// render the mission
void cMission::RenderUnder() {this->__RenderOwnUnder();}
void cMission::RenderOver () {this->__RenderOwnOver ();}
void cMission::RenderTop  () {this->__RenderOwnTop  ();}


// ****************************************************************
// move the mission
void cMission::MoveBefore()
{
    // 
    this->__MoveOwnBefore();

    if(!m_anStage.empty())
    {
        // 
        m_fStageTimeBefore = m_fStageTime;
        m_fStageTime.Update(1.0f);

        // 
        m_anStage.back()();

        // 
        if(m_anStage.empty()) g_pGame->StartOutro(0u);
    }
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
    m_anStage.pop_back();

    // 
    m_fStageTime       = 0.0f;
    m_fStageTimeBefore = 0.0f;
    m_iStageSub        = 0u;
    m_pfMedalGoal      = NULL;

    // 
    FOR_EACH(it, m_apPath)  SAFE_DELETE(*it)
    FOR_EACH(it, m_apSquad) SAFE_DELETE(*it)

    // 
    m_apPath .clear();
    m_apSquad.clear();

    // 
    if(m_piData) std::memset(m_piData, 0, sizeof(coreUint32) * m_iDataSize);



    
    m_nCollPlayerEnemy  = NULL;
    m_nCollPlayerBullet = NULL;
    m_nCollEnemyBullet  = NULL;
}


// ****************************************************************
// 
void cMission::ActivateBoss(const cBoss* pBoss)
{
    ASSERT(m_iCurSegmentIndex == MISSION_NO_SEGMENT)

    // 
    const coreUintW iIndex = std::find(m_apBoss, m_apBoss + MISSION_BOSSES, pBoss) - m_apBoss;
    ASSERT(iIndex < MISSION_BOSSES)

    // 
    m_pCurBoss         = m_apBoss[iIndex];
    m_iCurBossIndex    = iIndex;
    m_iCurSegmentIndex = MISSION_BOSS_TO_SEGMENT(m_iCurBossIndex);

    // 
    g_pGame->GetInterface()->ShowBoss(m_pCurBoss);
}

void cMission::DeactivateBoss()
{
    if(m_iCurSegmentIndex == MISSION_NO_SEGMENT) return;

    // 
    this->__CloseSegment();

    // 
    m_pCurBoss         = NULL;
    m_iCurBossIndex    = MISSION_NO_BOSS;
    m_iCurSegmentIndex = MISSION_NO_SEGMENT;
}


// ****************************************************************
// 
void cMission::ActivateWave(const coreChar* pcName)
{
    ASSERT(m_iCurSegmentIndex == MISSION_NO_SEGMENT)

    // 
    ASSERT(m_iCurWaveCount < MISSION_WAVES)
    m_iCurWaveIndex    = m_iCurWaveCount++;
    m_iCurSegmentIndex = MISSION_WAVE_TO_SEGMENT(m_iCurWaveIndex);

    // 
    g_pGame->GetInterface()->ShowWave(pcName);
}

void cMission::DeactivateWave()
{
    if(m_iCurSegmentIndex == MISSION_NO_SEGMENT) return;

    // 
    this->__CloseSegment();

    // 
    m_iCurWaveIndex    = MISSION_NO_WAVE;
    m_iCurSegmentIndex = MISSION_NO_SEGMENT;
}


// ****************************************************************
// 
void cMission::__CloseSegment()
{
    ASSERT(m_iCurSegmentIndex != MISSION_NO_SEGMENT)

    // 
    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
    g_pGame->GetChromaManager     ()->ClearChromas(true);
    g_pGame->GetItemManager       ()->LoseItems();
    g_pGame->GetShieldManager     ()->ClearShields(true);

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

    // 
    const coreFloat  fTime  = g_pGame->GetTimeTable()->GetTimeSegmentSafe();
    const coreUint32 iBonus = cGame::CalcBonusTime(fTime);

    // 
    coreUint8  aiMedal[GAME_PLAYERS] = {};
    coreUint32 aiPower[GAME_PLAYERS] = {};
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint32 iDamageTaken = pPlayer->GetDataTable()->GetCounterSegment(iMissionIndex, m_iCurSegmentIndex).iDamageTaken;

        // 
        aiMedal[i] = cGame::CalcMedal(fTime, iDamageTaken, m_pfMedalGoal);
        aiPower[i] = pPlayer->GetScoreTable()->GetScoreSegment(iMissionIndex, m_iCurSegmentIndex) + aiMedal[i] * 1000000u;

        // 
        pPlayer->GetScoreTable()->AddScore(iBonus, false);
    });

    if(g_pGame->GetCoop())
    {
        // give medal to the better player (or both on draw)
        if(aiPower[0] >= aiPower[1]) g_pGame->GetPlayer(0u)->GetDataTable()->GiveMedalSegment(aiMedal[0]);
        if(aiPower[1] >= aiPower[0]) g_pGame->GetPlayer(1u)->GetDataTable()->GiveMedalSegment(aiMedal[1]);
        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }
    else
    {
        // give medal to the only player
        g_pGame->GetPlayer(0u)->GetDataTable()->GiveMedalSegment(aiMedal[0]);
    }

    // 
    const coreUint8 iShowMedal     = MAX(aiMedal[0], aiMedal[1]);
    const coreUint8 iShowMedalType = MISSION_SEGMENT_IS_BOSS(m_iCurSegmentIndex) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE;
    g_pGame->GetInterface()->ShowScore(iBonus, iShowMedal, iShowMedalType);
}


// ****************************************************************
// 
UNITY_BUILD
#include "01_cViridoMission.cpp"
#include "01_cViridoMission_setup.cpp"
#include "02_cNevoMission.cpp"
#include "03_cHarenaMission.cpp"
#include "04_cRutilusMission.cpp"
#include "05_cGeluMission.cpp"
#include "06_cCalorMission.cpp"
#include "07_cMuscusMission.cpp"
#include "08_cAterMission.cpp"
#include "99_cIntroMission.cpp"
#include "A1_cErrorMission.cpp"
#include "A2_cTinkerMission.cpp"