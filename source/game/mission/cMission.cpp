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
coreUint32  cMission::s_aiSink[2]       = {};


// ****************************************************************
// constructor
cMission::cMission()noexcept
: m_apBoss              {}
, m_pCurBoss            (NULL)
, m_iCurBossIndex       (MISSION_NO_BOSS)
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
, m_iRecordBroken       (0u)
, m_iTakeFrom           (0u)
, m_iTakeTo             (TAKE_MISSION)
, m_bDelay              (false)
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
// 
void cMission::Open()
{
    // 
    g_pSave->EditLocalStatsMission()->iCountStart += 1u;
}


// ****************************************************************
// 
void cMission::Close()
{
    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

    // 
    g_pSave->EditGlobalStats()->iMissionsDone += 1u;

    // 
    coreUint32 iScoreFull = 0u;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        // 
        iScoreFull += pPlayer->GetScoreTable()->GetScoreMission(iMissionIndex);
    });

    // 
    g_pSave->EditLocalStatsMission()->iScoreBest   = MAX(g_pSave->EditLocalStatsMission()->iScoreBest,       iScoreFull);
    g_pSave->EditLocalStatsMission()->iScoreWorst  = MIN(g_pSave->EditLocalStatsMission()->iScoreWorst - 1u, iScoreFull - 1u) + 1u;
    g_pSave->EditLocalStatsMission()->iScoreTotal += iScoreFull;

    // 
    const coreUint32 iTimeUint = TABLE_TIME_TO_UINT(g_pGame->GetTimeTable()->GetTimeMission(iMissionIndex));
    g_pSave->EditLocalStatsMission()->iTimeBest   = MIN(g_pSave->EditLocalStatsMission()->iTimeBest - 1u, iTimeUint - 1u) + 1u;
    g_pSave->EditLocalStatsMission()->iTimeWorst  = MAX(g_pSave->EditLocalStatsMission()->iTimeWorst,     iTimeUint);
    g_pSave->EditLocalStatsMission()->iTimeTotal += iTimeUint;
    g_pSave->EditLocalStatsMission()->iCountEnd  += 1u;

    // 
    const coreUint32 iTimeShiftedUint = TABLE_TIME_TO_UINT(g_pGame->GetTimeTable()->GetTimeShiftedMission(iMissionIndex));
    if(iTimeShiftedUint - 1u < g_pSave->EditLocalStatsMission()->iTimeBestShifted - 1u)
    {
        g_pSave->EditLocalStatsMission()->iTimeBestShifted   = iTimeShiftedUint;
        g_pSave->EditLocalStatsMission()->iTimeBestShiftGood = g_pGame->GetTimeTable()->GetShiftGoodMission(iMissionIndex);
        g_pSave->EditLocalStatsMission()->iTimeBestShiftBad  = g_pGame->GetTimeTable()->GetShiftBadMission (iMissionIndex);
    }
    if(iTimeShiftedUint > g_pSave->EditLocalStatsMission()->iTimeWorstShifted)
    {
        g_pSave->EditLocalStatsMission()->iTimeWorstShifted   = iTimeShiftedUint;
        g_pSave->EditLocalStatsMission()->iTimeWorstShiftGood = g_pGame->GetTimeTable()->GetShiftGoodMission(iMissionIndex);
        g_pSave->EditLocalStatsMission()->iTimeWorstShiftBad  = g_pGame->GetTimeTable()->GetShiftBadMission (iMissionIndex);
    }

    // 
    if(g_CurConfig.Game.iGameSpeed >= 200u)
    {
        ADD_BIT(g_pSave->EditLocalStatsMission()->iFeat, FEAT_TWOHUNDRED)
    }

    // 
    g_pSave->SaveFile();
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
        
        // 
        std::memset(s_aiSink, 0, sizeof(s_aiSink));

        if(!m_anStage.empty())
        {
            // 
            m_fStageTimeBefore = m_fStageTime;
            m_fStageTime.Update(1.0f);

            // 
            m_fStageSubTimeBefore = m_fStageSubTime;
            m_fStageSubTime.Update(1.0f);
            
            // TODO 1: [MF] erste iteration startet mit m_fStageTime=0.016 und m_fStageSubTime=0.0, wegen erstem STAGE_SUB, is das problematisch ?

            // 
            m_anStage.back()();

            // 
            if(m_anStage.empty())
            {
                g_pGame->StartOutro((m_iTakeTo == TAKE_MISSION) ? GAME_OUTRO_MISSION : GAME_OUTRO_SEGMENT);
                if(m_iTakeTo == TAKE_MISSION) g_pGame->FadeMusic(0.3f);
            }
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
    STATIC_ASSERT(BADGES <= sizeof(m_iBadgeGiven)*8u)

    // 
    m_nCollPlayerEnemy  = NULL;
    m_nCollPlayerBullet = NULL;
    m_nCollEnemyBullet  = NULL;

    // 
    m_bDelay = false;

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
    g_pGame->GetInterface()->ShowAlert();

    // 
    g_pGame->FadeMusic(0.3f);
}

void cMission::DeactivateBoss()
{
    if(m_iCurSegmentIndex == MISSION_NO_SEGMENT) return;

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

    // 
    coreUint8& iAdvance = g_pSave->EditProgress()->aiAdvance[iMissionIndex + 1u];
    iAdvance = MAX(iAdvance, (iMissionIndex + 1u == MISSION_ATER) ? 6u : 1u);

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
void cMission::ActivateWave(const coreUintW iIndex, const coreChar* pcName)
{
    ASSERT(m_iCurSegmentIndex == MISSION_NO_SEGMENT)

    // 
    ASSERT(iIndex < MISSION_WAVES)
    m_iCurWaveIndex    = iIndex;
    m_iCurSegmentIndex = MISSION_WAVE_TO_SEGMENT(m_iCurWaveIndex);

    // 
    this->__OpenSegment();

    // 
    g_pGame->GetInterface()->ShowWave(pcName);
    
    
    if(!g_pGame->IsTask())
    {
        // 
        g_pGame->GetTimeTable()->AddShiftGood(10u);

        // 
        g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
           pPlayer->GetDataTable()->EditCounterTotal  ()->iShiftGoodAdded += 10u;
           pPlayer->GetDataTable()->EditCounterMission()->iShiftGoodAdded += 10u;
           pPlayer->GetDataTable()->EditCounterSegment()->iShiftGoodAdded += 10u;
        });
    }
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
    m_fStageWait = 6.0f;

    // 
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->HealShield(pPlayer->GetMaxShield() / 5);

        const coreUint32 iCombo = pPlayer->GetScoreTable()->GetCurCombo();
        if(iCombo)
        {
            const coreUint32 iScore = pPlayer->GetScoreTable()->AddScore(100u * iCombo, false);
            g_pGame->GetCombatText()->DrawExtra(iScore, pPlayer->GetPosition(), true);

            pPlayer->GetScoreTable()->CancelCooldown();
        }
    });

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
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->GetDataTable()->GiveBadge(iIndex);
    });

    // 
    const coreUint32 iBonus = cGame::CalcBonusBadge(iBadge);
    if(iBonus)
    {
        // 
        g_pGame->GetTimeTable()->AddShiftGood(iBonus);

        // 
        g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
           pPlayer->GetDataTable()->EditCounterTotal  ()->iShiftGoodAdded += iBonus;
           pPlayer->GetDataTable()->EditCounterMission()->iShiftGoodAdded += iBonus;
           pPlayer->GetDataTable()->EditCounterSegment()->iShiftGoodAdded += iBonus;
        });

        // 
        g_pSave->EditGlobalStats      ()->iShiftGoodAdded += iBonus;
        g_pSave->EditLocalStatsArcade ()->iShiftGoodAdded += iBonus;
        g_pSave->EditLocalStatsMission()->iShiftGoodAdded += iBonus;
        g_pSave->EditLocalStatsSegment()->iShiftGoodAdded += iBonus;

        // 
        g_pGame->GetCombatText()->DrawBadge(iBonus, vPosition);

        // 
        g_pSpecialEffects->PlaySound(vPosition, 1.0f, 1.0f, SOUND_BADGE);
    }

    // 
    if(iBadge == BADGE_ACHIEVEMENT)
    {
        // 
        const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

        // 
        ADD_BIT(g_pSave->EditProgress()->aaiBadge[iMissionIndex][m_iCurSegmentIndex], iIndex)
    }
}


// ****************************************************************
// 
void cMission::AddExtraScore(cPlayer* OUTPUT pPlayer, const coreUint32 iScore, const coreVector3 vPosition)
{
    // 
    const coreUint32 iFinalScore = pPlayer->GetScoreTable()->AddScore(iScore, true);
    pPlayer->GetScoreTable()->AddCombo(1u);

    // 
    g_pGame->GetCombatText()->DrawScore(iFinalScore, vPosition, false);
}


// ****************************************************************
// 
void cMission::__OpenSegment()
{
    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

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
    g_pGame->GetShieldManager     ()->ClearShields(true);

    // 
    g_pGame->HideHelpers ();
    g_pGame->RepairPlayer();

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

    // 
    const coreFloat  fTime        = g_pGame->GetTimeTable()->GetTimeSegmentSafe();
    const coreFloat  fTimeShifted = g_pGame->GetTimeTable()->GetTimeShiftedSegmentSafe();
    const coreUint32 iBonus       = cGame::CalcBonusTime(fTimeShifted, m_pfMedalGoal);

    // 
    coreUint8  iMedal         = MEDAL_NONE;
    coreUint32 iScoreFull     = 0u;
    coreUint32 iMaxSeriesFull = 0u;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        // 
        pPlayer->GetScoreTable()->AddScore(iBonus, false);

        // 
        iMedal += cGame::CalcMedal(fTimeShifted, m_pfMedalGoal);

        // 
        iScoreFull     += pPlayer->GetScoreTable()->GetScoreSegment    (iMissionIndex, m_iCurSegmentIndex);
        iMaxSeriesFull += pPlayer->GetScoreTable()->GetMaxSeriesSegment(iMissionIndex, m_iCurSegmentIndex);
    });

    // 
    if(g_pGame->IsCoop()) iMedal /= GAME_PLAYERS;
    ASSERT(iMedal != MEDAL_NONE)

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
    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SPECIAL_SOUND_MEDAL(iMedal));

    // 
    g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->StartRolling(pPlayer->GetInput()->vMove);
    });

    // 
    g_pReplay->ApplySnapshot(REPLAY_SNAPSHOT_SEGMENT_END(iMissionIndex, m_iCurSegmentIndex));

    // 
    SET_BIT(m_iRecordBroken, 0u, (iScoreFull     > g_pSave->EditLocalStatsSegment()->iScoreBest))
    SET_BIT(m_iRecordBroken, 1u, (false))
    SET_BIT(m_iRecordBroken, 2u, (iMaxSeriesFull > g_pSave->EditLocalStatsSegment()->iMaxSeries))

    // 
    g_pSave->EditLocalStatsSegment()->iScoreBest   = MAX(g_pSave->EditLocalStatsSegment()->iScoreBest,       iScoreFull);
    g_pSave->EditLocalStatsSegment()->iScoreWorst  = MIN(g_pSave->EditLocalStatsSegment()->iScoreWorst - 1u, iScoreFull - 1u) + 1u;
    g_pSave->EditLocalStatsSegment()->iScoreTotal += iScoreFull;

    // 
    const coreUint32 iTimeUint = TABLE_TIME_TO_UINT(fTime);
    g_pSave->EditLocalStatsSegment()->iTimeBest   = MIN(g_pSave->EditLocalStatsSegment()->iTimeBest - 1u, iTimeUint - 1u) + 1u;
    g_pSave->EditLocalStatsSegment()->iTimeWorst  = MAX(g_pSave->EditLocalStatsSegment()->iTimeWorst,     iTimeUint);
    g_pSave->EditLocalStatsSegment()->iTimeTotal += iTimeUint;
    g_pSave->EditLocalStatsSegment()->iCountEnd  += 1u;

    // 
    g_pSave->EditLocalStatsSegment()->iMaxSeries = MAX(g_pSave->EditLocalStatsSegment()->iMaxSeries, iMaxSeriesFull);

    // 
    const coreUint32 iTimeShiftedUint = TABLE_TIME_TO_UINT(fTimeShifted);
    if(iTimeShiftedUint - 1u < g_pSave->EditLocalStatsSegment()->iTimeBestShifted - 1u)
    {
        ADD_BIT(m_iRecordBroken, 1u)

        g_pSave->EditLocalStatsSegment()->iTimeBestShifted   = iTimeShiftedUint;
        g_pSave->EditLocalStatsSegment()->iTimeBestShiftGood = g_pGame->GetTimeTable()->GetShiftGoodSegment(iMissionIndex, m_iCurSegmentIndex);
        g_pSave->EditLocalStatsSegment()->iTimeBestShiftBad  = g_pGame->GetTimeTable()->GetShiftBadSegment (iMissionIndex, m_iCurSegmentIndex);
    }
    if(iTimeShiftedUint > g_pSave->EditLocalStatsSegment()->iTimeWorstShifted)
    {
        g_pSave->EditLocalStatsSegment()->iTimeWorstShifted   = iTimeShiftedUint;
        g_pSave->EditLocalStatsSegment()->iTimeWorstShiftGood = g_pGame->GetTimeTable()->GetShiftGoodSegment(iMissionIndex, m_iCurSegmentIndex);
        g_pSave->EditLocalStatsSegment()->iTimeWorstShiftBad  = g_pGame->GetTimeTable()->GetShiftBadSegment (iMissionIndex, m_iCurSegmentIndex);
    }

    // 
    if(g_CurConfig.Game.iGameSpeed >= 200u)
    {
        ADD_BIT(g_pSave->EditLocalStatsSegment()->iFeat, FEAT_TWOHUNDRED)
    }

    // 
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        ADD_FLAG(g_pSave->EditProgress()->aaiBadge[iMissionIndex][m_iCurSegmentIndex], pPlayer->GetDataTable()->GetBadgeAll(iMissionIndex, m_iCurSegmentIndex))
    });

    // 
    coreUint8& iAdvance = g_pSave->EditProgress()->aiAdvance[iMissionIndex];
    iAdvance = MAX(iAdvance, m_iCurSegmentIndex + 2u);

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