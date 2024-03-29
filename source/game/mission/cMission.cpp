///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
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
: m_apBoss              {}
, m_pCurBoss            (NULL)
, m_iCurBossIndex       (MISSION_NO_BOSS)
, m_iCurWaveCount       (0u)
, m_iCurWaveIndex       (MISSION_NO_WAVE)
, m_iCurSegmentIndex    (MISSION_NO_SEGMENT)
, m_piData              (NULL)
, m_iDataSize           (0u)
, m_iStageNum           (0u)
, m_fStageTime          (0.0f)
, m_fStageTimeBefore    (0.0f)
, m_iStageSub           (0u)
, m_fStageSubTime       (0.0f)
, m_fStageSubTimeBefore (0.0f)
, m_fStageWait          (0.0f)
, m_pfMedalGoal         (NULL)
, m_iBadgeGiven         (0u)
, m_iTakeFrom           (0u)
, m_iTakeTo             (TAKE_MISSION)
, m_bRepeat             (false)
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
    ALIGNED_DELETE(m_piData)
}


// ****************************************************************
// setup the mission
void cMission::Setup(const coreUint8 iTakeFrom, const coreUint8 iTakeTo)
{
    // 
    ASSERT(iTakeFrom <= iTakeTo)
    m_iTakeFrom = iTakeFrom;
    m_iTakeTo   = iTakeTo;

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
void cMission::RenderBottom() {this->__RenderOwnBottom();}
void cMission::RenderUnder () {this->__RenderOwnUnder ();}
void cMission::RenderOver  () {this->__RenderOwnOver  ();}
void cMission::RenderTop   () {this->__RenderOwnTop   ();}


// ****************************************************************
// move the mission
void cMission::MoveBefore()
{
    // 
    this->__MoveOwnBefore();

    do
    {
        // 
        m_bRepeat = false;

        if(!m_anStage.empty())
        {
            // 
            m_fStageTimeBefore = m_fStageTime;
            m_fStageTime.Update(1.0f);

            // 
            m_fStageSubTimeBefore = m_fStageSubTime;
            m_fStageSubTime.Update(1.0f);

            // 
            m_anStage.back()();

            // 
            if(m_anStage.empty()) g_pGame->StartOutro((m_iTakeTo == TAKE_MISSION) ? 0u : 1u);
        }
    }
    while(m_bRepeat);
}

void cMission::MoveMiddle()
{
    // 
    this->__MoveOwnMiddle();
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
    FOR_EACH(it, m_apPath)  SAFE_DELETE(*it)
    FOR_EACH(it, m_apSquad) SAFE_DELETE(*it)

    // 
    m_apPath .clear();
    m_apSquad.clear();

    // 
    if(m_piData) std::memset(m_piData, 0, sizeof(coreUint32) * m_iDataSize);

    // 
    m_fStageTime          = 0.0f;
    m_fStageTimeBefore    = 0.0f;
    m_iStageSub           = 0u;
    m_fStageSubTime       = 0.0f;
    m_fStageSubTimeBefore = 0.0f;
    m_fStageWait          = 0.0f;

    // 
    m_pfMedalGoal = NULL;

    // 
    m_iBadgeGiven = 0u;

    // 
    m_nCollPlayerEnemy  = NULL;
    m_nCollPlayerBullet = NULL;
    m_nCollEnemyBullet  = NULL;

    // 
    m_bRepeat = true;
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
    this->__OpenSegment();

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

    // 
    g_pSave->EditGlobalStats()->iBossesDone += 1u;
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
    this->__OpenSegment();

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

    // 
    m_iStageSub  = 0xFFu;
    m_fStageWait = 4.0f;

    // 
    g_pSave->EditGlobalStats()->iWavesDone += 1u;
}


// ****************************************************************
// 
void cMission::GiveBadge(const coreUintW iIndex, const coreUint8 iBadge, const coreVector3 vPosition)
{
    ASSERT(iIndex < BADGES)

    if(HAS_BIT(m_iBadgeGiven, iIndex)) return;
    ADD_BIT(m_iBadgeGiven, iIndex)

    // 
    const coreUint32 iBonus = cGame::CalcBonusBadge(iBadge);

    // 
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->GetDataTable ()->GiveBadge(iIndex);
        pPlayer->GetScoreTable()->AddScore(iBonus, false);
    });

    // 
    g_pGame->GetCombatText()->DrawBadge(iBonus, vPosition);
}


// ****************************************************************
// 
void cMission::__OpenSegment()
{
    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

    // 
    coreUint8& iAdvance = g_pSave->EditProgress()->aiAdvance[iMissionIndex];
    iAdvance = MAX(iAdvance, m_iCurSegmentIndex + 1u);

    // 
    g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_SEGMENT_START(iMissionIndex, m_iCurSegmentIndex));

    // 
    g_pSave->EditLocalStatsSegment()->iCountStart += 1u;
}


// ****************************************************************
// 
void cMission::__CloseSegment()
{
    ASSERT(m_iCurSegmentIndex != MISSION_NO_SEGMENT)

    // 
    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
    g_pGame->GetItemManager       ()->LoseItems();
    g_pGame->GetShieldManager     ()->ClearShields(true);

    // 
    g_pGame->HideHelpers    ();
    g_pGame->KillRepairEnemy();

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

    // 
    const coreFloat  fTime  = g_pGame->GetTimeTable()->GetTimeSegmentSafe();
    const coreUint32 iBonus = cGame::CalcBonusTime(fTime);

    // 
    coreUint8 iMedal = 0u;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint32 iDamageTaken  = pPlayer->GetDataTable ()->GetCounterSegment(iMissionIndex, m_iCurSegmentIndex).iDamageTaken;
        const coreUint32 iScoreSegment = pPlayer->GetScoreTable()->GetScoreSegment  (iMissionIndex, m_iCurSegmentIndex);

        // 
        iMedal += cGame::CalcMedal(fTime, iDamageTaken, m_pfMedalGoal);

        // 
        pPlayer->GetScoreTable()->AddScore(iBonus, false);

        // 
        const coreUint32 iScoreFull = iScoreSegment + iBonus;
        g_pSave->EditLocalStatsSegment()->iScoreBest   = MAX(g_pSave->EditLocalStatsSegment()->iScoreBest,       iScoreFull);
        g_pSave->EditLocalStatsSegment()->iScoreWorst  = MIN(g_pSave->EditLocalStatsSegment()->iScoreWorst - 1u, iScoreFull - 1u) + 1u;
        g_pSave->EditLocalStatsSegment()->iScoreTotal += iScoreFull;
    });

    // 
    if(g_pGame->IsCoop()) iMedal /= GAME_PLAYERS;

    // 
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->GetDataTable()->GiveMedalSegment(iMedal);
    });

    // 
    const coreUint8 iShowMedal     = iMedal;
    const coreUint8 iShowMedalType = MISSION_SEGMENT_IS_BOSS(m_iCurSegmentIndex) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE;
    g_pGame->GetInterface()->ShowScore(iBonus, iShowMedal, iShowMedalType);

    // 
    g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_SEGMENT_END(iMissionIndex, m_iCurSegmentIndex));

    // 
    const coreUint32 iTimeUint = TABLE_TIME_TO_UINT(fTime);
    g_pSave->EditLocalStatsSegment()->iTimeBest   = MAX(g_pSave->EditLocalStatsSegment()->iTimeBest,       iTimeUint);
    g_pSave->EditLocalStatsSegment()->iTimeWorst  = MIN(g_pSave->EditLocalStatsSegment()->iTimeWorst - 1u, iTimeUint - 1u) + 1u;
    g_pSave->EditLocalStatsSegment()->iTimeTotal += iTimeUint;
    g_pSave->EditLocalStatsSegment()->iCountEnd  += 1u;

    // 
    g_pSave->SaveFile();
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
#include "A1_cBonus1Mission.cpp"
#include "A2_cBonus2Mission.cpp"
#include "B1_cErrorMission.cpp"
#include "B2_cDemoMission.cpp"