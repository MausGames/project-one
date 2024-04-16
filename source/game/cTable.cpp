///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cDataTable::cDataTable()noexcept
: m_pOwner (NULL)
{
    // 
    this->Reset();
}


// ****************************************************************
// 
void cDataTable::Update()
{
}


// ****************************************************************
// 
void cDataTable::Reset()
{
    // 
    std::memset(&m_CounterTotal,     0, sizeof(m_CounterTotal));
    std::memset(&m_aCounterMission,  0, sizeof(m_aCounterMission));
    std::memset(&m_aaCounterSegment, 0, sizeof(m_aaCounterSegment));

    // 
    std::memset(&m_aiMedalMission,  0, sizeof(m_aiMedalMission));
    std::memset(&m_aaiMedalSegment, 0, sizeof(m_aaiMedalSegment));

    // 
    std::memset(&m_aiFragment, 0, sizeof(m_aiFragment));
    std::memset(&m_aaiBadge,   0, sizeof(m_aaiBadge));
    std::memset(&m_aiHelper,   0, sizeof(m_aiHelper));
}


// ****************************************************************
// 
void cDataTable::RevertSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)

    m_aiFragment[iMissionIndex]                = 0u;   // reset whole mission
    m_aaiBadge  [iMissionIndex][iSegmentIndex] = 0u;
    m_aiHelper  [iMissionIndex]                = 0u;   // reset whole mission
}

void cDataTable::RevertSegment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->RevertSegment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
RETURN_NONNULL cDataTable::sCounter* cDataTable::EditCounterTotal()
{
    // 
    return &m_CounterTotal;
}

RETURN_NONNULL cDataTable::sCounter* cDataTable::EditCounterMission(const coreUintW iMissionIndex)
{
    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    return &m_aCounterMission[iMissionIndex];
}

RETURN_NONNULL cDataTable::sCounter* cDataTable::EditCounterMission()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditCounterMission(g_pGame->GetCurMissionIndex());
}

RETURN_NONNULL cDataTable::sCounter* cDataTable::EditCounterSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    if(iSegmentIndex != MISSION_NO_SEGMENT)
    {
        ASSERT(iMissionIndex < TABLE_MISSIONS)
        ASSERT(iSegmentIndex < TABLE_SEGMENTS)
        return &m_aaCounterSegment[iMissionIndex][iSegmentIndex];
    }

    WARN_IF(false) {}

    // 
    static sCounter s_CounterDummy;
    return &s_CounterDummy;
}

RETURN_NONNULL cDataTable::sCounter* cDataTable::EditCounterSegment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditCounterSegment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cDataTable::GiveMedalMission(const coreUint8 iMedal, const coreUintW iMissionIndex)
{
    // 
    ASSERT(iMedal        < MEDAL_MAX)
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    m_aiMedalMission[iMissionIndex] = iMedal;

    if(STATIC_ISVALID(g_pGame))
    {
        if(g_pGame->GetPlayerIndex(m_pOwner) == 0u)
        {
            // 
            g_pSave->EditGlobalStats      ()->aiMedalsEarned[iMedal] += 1u;
            g_pSave->EditLocalStatsMission()->aiMedalsEarned[iMedal] += 1u;
        }

        // 
        coreUint8& iMedalMission = g_pSave->EditProgress()->aaaaiMedalMission[g_pGame->GetType()][g_pGame->GetMode()][g_pGame->GetDifficulty()][iMissionIndex];
        iMedalMission = MAX(iMedalMission, iMedal);
    }
}

void cDataTable::GiveMedalMission(const coreUint8 iMedal)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->GiveMedalMission(iMedal, g_pGame->GetCurMissionIndex());
}

void cDataTable::GiveMedalSegment(const coreUint8 iMedal, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    ASSERT(iMedal        < MEDAL_MAX)
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)
    m_aaiMedalSegment[iMissionIndex][iSegmentIndex] = iMedal;

    if(STATIC_ISVALID(g_pGame))
    {
        if(g_pGame->GetPlayerIndex(m_pOwner) == 0u)
        {
            // 
            g_pSave->EditGlobalStats      ()                            ->aiMedalsEarned[iMedal] += 1u;
            g_pSave->EditLocalStatsSegment(iMissionIndex, iSegmentIndex)->aiMedalsEarned[iMedal] += 1u;
        }

        // 
        coreUint8& iMedalSegment = g_pSave->EditProgress()->aaaaaiMedalSegment[g_pGame->GetType()][g_pGame->GetMode()][g_pGame->GetDifficulty()][iMissionIndex][iSegmentIndex];
        iMedalSegment = MAX(iMedalSegment, iMedal);
    }
}

void cDataTable::GiveMedalSegment(const coreUint8 iMedal)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->GiveMedalSegment(iMedal, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cDataTable::GiveFragment(const coreUintW iMissionIndex, const coreUintW iBossIndex)
{
    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iBossIndex    < TABLE_BOSSES)
    ADD_BIT(m_aiFragment[iMissionIndex], iBossIndex)

    if(STATIC_ISVALID(g_pGame))
    {
        //if(g_pGame->GetPlayerIndex(m_pOwner) == 0u)
        //{
        //    g_pSave->EditGlobalStats      ()                                                  ->iFragmentsEarned += 1u;
        //    g_pSave->EditLocalStatsMission(iMissionIndex)                                     ->iFragmentsEarned += 1u;
        //    g_pSave->EditLocalStatsSegment(iMissionIndex, MISSION_BOSS_TO_SEGMENT(iBossIndex))->iFragmentsEarned += 1u;
        //}

        // 
        ADD_BIT(g_pSave->EditProgress()->aiFragment[iMissionIndex], iBossIndex)
    }
}

void cDataTable::GiveFragment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->GiveFragment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurBossIndex());
}


// ****************************************************************
// 
void cDataTable::GiveBadge(const coreUintW iBadgeIndex, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    ASSERT(iBadgeIndex   < TABLE_BADGES)
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)
    ADD_BIT(m_aaiBadge[iMissionIndex][iSegmentIndex], iBadgeIndex)

    if(STATIC_ISVALID(g_pGame))
    {
        if(g_pGame->GetPlayerIndex(m_pOwner) == 0u)
        {
            // 
            g_pSave->EditGlobalStats      ()                            ->iBadgesEarned += 1u;
            g_pSave->EditLocalStatsArcade ()                            ->iBadgesEarned += 1u;
            g_pSave->EditLocalStatsMission(iMissionIndex)               ->iBadgesEarned += 1u;
            g_pSave->EditLocalStatsSegment(iMissionIndex, iSegmentIndex)->iBadgesEarned += 1u;
        }
    }
}

void cDataTable::GiveBadge(const coreUintW iBadgeIndex)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->GiveBadge(iBadgeIndex, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cDataTable::GiveHelper(const coreUintW iHelperIndex, const coreUintW iMissionIndex)
{
    // 
    ASSERT(iHelperIndex  < TABLE_HELPERS)
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ADD_BIT(m_aiHelper[iMissionIndex], iHelperIndex)
}

void cDataTable::GiveHelper(const coreUintW iHelperIndex)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->GiveHelper(iHelperIndex, g_pGame->GetCurMissionIndex());
}


// ****************************************************************
// constructor
cScoreTable::cScoreTable()noexcept
: m_pOwner (NULL)
{
    // 
    this->Reset();
}


// ****************************************************************
// 
void cScoreTable::Update()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    if(m_fCooldown && !g_pGame->GetCurMission()->GetDelay())
    {
        m_fCooldown.UpdateMax(-1.0f/2.0f, 0.0f);
        if(!m_fCooldown) this->CancelCooldown();
    }
}


// ****************************************************************
// 
void cScoreTable::Reset()
{
    // reset all score values (# no memset)
    m_iScoreTotal = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aiScoreMission[j] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaiScoreSegment[j][i] = 0u;

    // reset combo and chain values (# no memset)
    m_iCurCombo = 0u;
    m_iCurChain = 0u;
    m_fCooldown = 0.0f;
    m_iOverride = UINT32_MAX;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaiMaxSeriesSegment[j][i] = 0u;

    // 
    for(coreUintW k = 0u; k < TABLE_RUNS; ++k)
    {
        // (# no memset) 
        m_aiRunTotal[k] = 0u;
        for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aaiRunMission[k][j] = 0u;
        for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaaiRunSegment[k][j][i] = 0u;

        // (# no memset) 
        m_aiRunMissionIndex[k] = 0xFFu;
        m_aiRunSegmentIndex[k] = 0xFFu;
    }
}


// ****************************************************************
// 
void cScoreTable::RevertSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)

    // 
    m_iScoreTotal                                  -= m_aaiScoreSegment[iMissionIndex][iSegmentIndex];
    m_aiScoreMission [iMissionIndex]               -= m_aaiScoreSegment[iMissionIndex][iSegmentIndex];
    m_aaiScoreSegment[iMissionIndex][iSegmentIndex] = 0u;

    // 
    m_iCurCombo = 0u;
    m_iCurChain = 0u;
    m_fCooldown = 0.0f;
    m_iOverride = UINT32_MAX;
    m_aaiMaxSeriesSegment[iMissionIndex][iSegmentIndex] = 0u;
}

void cScoreTable::RevertSegment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->RevertSegment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cScoreTable::RevertSegmentNew(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)

    // reset all score values (# no memset)
    m_iScoreTotal = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aiScoreMission[j] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaiScoreSegment[j][i] = 0u;

    // 
    m_iCurCombo = 0u;
    m_iCurChain = 0u;
    m_fCooldown = 0.0f;
    m_iOverride = UINT32_MAX;
    m_aaiMaxSeriesSegment[iMissionIndex][iSegmentIndex] = 0u;
}

void cScoreTable::RevertSegmentNew()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->RevertSegmentNew(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cScoreTable::StoreRun(const coreUintW iRunIndex, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT(iRunIndex     < TABLE_RUNS)
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)

    // (# no memcpy) 
    m_aiRunTotal[iRunIndex] = m_iScoreTotal;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aaiRunMission[iRunIndex][j] = m_aiScoreMission[j];
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaaiRunSegment[iRunIndex][j][i] = m_aaiScoreSegment[j][i];

    // 
    m_aiRunMissionIndex[iRunIndex] = iMissionIndex;
    m_aiRunSegmentIndex[iRunIndex] = iSegmentIndex;
}


// ****************************************************************
// 
coreUint32 cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    const coreUint32 iFinalValue = bModified ? this->ModifyValue(iValue) : iValue;

    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    m_iScoreTotal                   += iFinalValue;
    m_aiScoreMission[iMissionIndex] += iFinalValue;

    // 
    if(iSegmentIndex != MISSION_NO_SEGMENT)
    {
        ASSERT(iSegmentIndex < TABLE_SEGMENTS)
        m_aaiScoreSegment[iMissionIndex][iSegmentIndex] += iFinalValue;
    }

    // 
    g_pSave->EditGlobalStats()->iScoreGained += iFinalValue;

    return iFinalValue;
}

coreUint32 cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->AddScore(iValue, bModified, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cScoreTable::AddCombo(const coreUint32 iValue)
{
    // 
    m_iCurCombo += iValue;

    // 
    this->__ChangeMaxSeries(m_iCurCombo);

    // 
    this->RefreshCooldown();
}


// ****************************************************************
// 
void cScoreTable::AddChain(const coreUint32 iValue)
{
    // 
    m_iCurChain += iValue;

    // 
    this->__ChangeMaxSeries(m_iCurChain);

    // 
    this->RefreshCooldown();
}


// ****************************************************************
// 
void cScoreTable::TransferChain()
{
    if(m_iCurChain)
    {
        ASSERT(STATIC_ISVALID(g_pGame))

        // 
        const coreUint32 iScore = this->AddScore(g_pGame->RaiseValue(m_iCurChain), true);
        m_iCurChain = 0u;

        // 
        g_pGame->GetCombatText()->DrawChain(iScore, m_pOwner->GetPosition());
    }
}


// ****************************************************************
// 
void cScoreTable::RefreshCooldown()
{
    // 
    m_fCooldown = 1.0f;
}


// ****************************************************************
// 
void cScoreTable::CancelCooldown()
{
    // 
    if(m_iCurCombo || m_iCurChain)
    {
        m_pOwner->ShowCircle();
        g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_PLACEHOLDER);   // TODO 1: add sound ?
    }

    // 
    this->TransferChain();

    // 
    m_iCurCombo = 0u;
    m_iCurChain = 0u;
    m_fCooldown = 0.0f;
}


// ****************************************************************
// 
void cScoreTable::__ChangeMaxSeries(const coreUint32 iMaxValue, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    m_aaiMaxSeriesSegment[iMissionIndex][iSegmentIndex] = MAX(m_aaiMaxSeriesSegment[iMissionIndex][iSegmentIndex], iMaxValue);
}

void cScoreTable::__ChangeMaxSeries(const coreUint32 iMaxValue)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->__ChangeMaxSeries(iMaxValue, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// constructor
cTimeTable::cTimeTable()noexcept
: m_dFrameTime (1.0 / coreDouble(g_fGameRate))
{
    // 
    this->Reset();
}


// ****************************************************************
// 
void cTimeTable::Update()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_iTimeMono += 1u;

    // 
    //if(!TIME) return;
    if(SPECIAL_FROZEN) return;       
    //ASSERT(TIME == m_dFrameTime)

    // 
    m_iTimeEvent += 1u;

    if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY) && !g_pGame->GetCurMission()->GetDelay())
    {
        const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
        const coreUintW iSegmentIndex = g_pGame->GetCurMission()->GetCurSegmentIndex();

        // 
        m_iTimeTotal += 1u;

        // 
        if(iSegmentIndex != MISSION_NO_SEGMENT)
        {
            ASSERT(iMissionIndex < TABLE_MISSIONS)
            ASSERT(iSegmentIndex < TABLE_SEGMENTS)
            m_aiTimeMission [iMissionIndex]                += 1u;
            m_aaiTimeSegment[iMissionIndex][iSegmentIndex] += 1u;
        }
    }

    // 
    ASSERT(m_iTimeEvent > 0u)
    g_pSave->EditGlobalStats()->iTimePlayed += TABLE_TIME_TO_UINT(__TABLE_TIME_CONVERT(m_iTimeEvent)) - TABLE_TIME_TO_UINT(__TABLE_TIME_CONVERT(m_iTimeEvent - 1u));
}


// ****************************************************************
// 
void cTimeTable::AddShiftGood(const coreUint16 iValue, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)
    m_aiShiftGoodMission [iMissionIndex]                += iValue;
    m_aaiShiftGoodSegment[iMissionIndex][iSegmentIndex] += iValue;
}

void cTimeTable::AddShiftGood(const coreUint16 iValue)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->AddShiftGood(iValue, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cTimeTable::AddShiftBad(const coreUint16 iValue, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)
    m_aiShiftBadMission [iMissionIndex]                += iValue;
    m_aaiShiftBadSegment[iMissionIndex][iSegmentIndex] += iValue;
}

void cTimeTable::AddShiftBad(const coreUint16 iValue)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->AddShiftBad(iValue, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cTimeTable::Reset()
{
    // reset all time values (# no memset)
    m_iTimeMono  = 0u;
    m_iTimeEvent = 0u;
    m_iTimeTotal = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aiTimeMission[j] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaiTimeSegment[j][i] = MISSION_SEGMENT_IS_BOSS(i) ? F_TO_UI(-100.0 / m_dFrameTime) : 0u;

    // reset all shift values (# no memset)
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aiShiftGoodMission[j] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaiShiftGoodSegment[j][i] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aiShiftBadMission [j] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaiShiftBadSegment [j][i] = 0u;
}


// ****************************************************************
// 
void cTimeTable::RevertSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)

    // 
    m_iTimeTotal                                  -= m_aaiTimeSegment[iMissionIndex][iSegmentIndex];
    m_aiTimeMission [iMissionIndex]               -= m_aaiTimeSegment[iMissionIndex][iSegmentIndex];
    m_aaiTimeSegment[iMissionIndex][iSegmentIndex] = MISSION_SEGMENT_IS_BOSS(iSegmentIndex) ? F_TO_UI(-100.0 / m_dFrameTime) : 0u;

    // 
    m_aiShiftGoodMission [iMissionIndex]               -= m_aaiShiftGoodSegment[iMissionIndex][iSegmentIndex];
    m_aaiShiftGoodSegment[iMissionIndex][iSegmentIndex] = 0u;
    m_aiShiftBadMission  [iMissionIndex]               -= m_aaiShiftBadSegment [iMissionIndex][iSegmentIndex];
    m_aaiShiftBadSegment [iMissionIndex][iSegmentIndex] = 0u;
}

void cTimeTable::RevertSegment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->RevertSegment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cTimeTable::RevertSegmentNew(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)

    // 
    m_aaiTimeSegment[iMissionIndex][iSegmentIndex] = MISSION_SEGMENT_IS_BOSS(iSegmentIndex) ? F_TO_UI(-100.0 / m_dFrameTime) : 0u;

    // 
    m_aaiShiftGoodSegment[iMissionIndex][iSegmentIndex] = 0u;
    m_aaiShiftBadSegment [iMissionIndex][iSegmentIndex] = 0u;
}

void cTimeTable::RevertSegmentNew()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->RevertSegmentNew(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cTimeTable::StartBoss(const coreUintW iMissionIndex, const coreUintW iBossIndex)
{
    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iBossIndex    < TABLE_BOSSES)
    m_aaiTimeSegment[iMissionIndex][MISSION_BOSS_TO_SEGMENT(iBossIndex)] = ((iMissionIndex == MISSION_ATER) && (iBossIndex == 0u)) ? 0u : F_TO_UI(coreDouble(-INTERFACE_BANNER_DURATION_BOSS) / m_dFrameTime);
}

void cTimeTable::StartBoss()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->StartBoss(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurBossIndex());
}


// ****************************************************************
// 
coreFloat cTimeTable::GetTimeSegmentSafe(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const
{
    // 
    return (iSegmentIndex != MISSION_NO_SEGMENT) ? MAX0(this->GetTimeSegment(iMissionIndex, iSegmentIndex)) : 0.0f;
}

coreFloat cTimeTable::GetTimeSegmentSafe()const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->GetTimeSegmentSafe(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetLastSegmentIndex());
}


// ****************************************************************
// 
coreInt32 cTimeTable::GetShiftSegmentSafe(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const
{
    // 
    return (iSegmentIndex != MISSION_NO_SEGMENT) ? (coreInt32(this->GetShiftBadSegment(iMissionIndex, iSegmentIndex)) - coreInt32(this->GetShiftGoodSegment(iMissionIndex, iSegmentIndex))) : 0;
}

coreInt32 cTimeTable::GetShiftSegmentSafe()const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->GetShiftSegmentSafe(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetLastSegmentIndex());
}


// ****************************************************************
// 
coreFloat cTimeTable::GetTimeShiftedSegmentSafe(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const
{
    // 
    return this->GetTimeSegmentSafe(iMissionIndex, iSegmentIndex) + I_TO_F(this->GetShiftSegmentSafe(iMissionIndex, iSegmentIndex));
}

coreFloat cTimeTable::GetTimeShiftedSegmentSafe()const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->GetTimeShiftedSegmentSafe(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetLastSegmentIndex());
}