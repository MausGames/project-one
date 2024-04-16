///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
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
    std::memset(&m_aiBadge,    0, sizeof(m_aiBadge));
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

    // 
    g_pSave->EditGlobalStats      ()->aiMedalsEarned[iMedal] += 1u;
    g_pSave->EditLocalStatsMission()->aiMedalsEarned[iMedal] += 1u;
    g_pSave->EditLocalStatsSegment()->aiMedalsEarned[iMedal] += 1u;

    // 
    coreUint8& iMedalMission = g_pSave->EditProgress()->aiMedalMission[iMissionIndex];
    iMedalMission = MAX(iMedalMission, iMedal);
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

    // 
    g_pSave->EditGlobalStats      ()                            ->aiMedalsEarned[iMedal] += 1u;
    g_pSave->EditLocalStatsMission(iMissionIndex)               ->aiMedalsEarned[iMedal] += 1u;
    g_pSave->EditLocalStatsSegment(iMissionIndex, iSegmentIndex)->aiMedalsEarned[iMedal] += 1u;

    // 
    coreUint8& iMedalSegment = g_pSave->EditProgress()->aaiMedalSegment[iMissionIndex][iSegmentIndex];
    iMedalSegment = MAX(iMedalSegment, iMedal);
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

    // 
    g_pSave->EditGlobalStats      ()                                                  ->iFragmentsEarned += 1u;
    g_pSave->EditLocalStatsMission(iMissionIndex)                                     ->iFragmentsEarned += 1u;
    g_pSave->EditLocalStatsSegment(iMissionIndex, MISSION_BOSS_TO_SEGMENT(iBossIndex))->iFragmentsEarned += 1u;

    // 
    ADD_BIT(g_pSave->EditProgress()->aiFragment[iMissionIndex], iBossIndex)
}

void cDataTable::GiveFragment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->GiveFragment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurBossIndex());
}


// ****************************************************************
// 
void cDataTable::GiveBadge(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    ASSERT(iSegmentIndex < TABLE_SEGMENTS)
    ADD_BIT(m_aiBadge[iMissionIndex], iSegmentIndex)

    // 
    g_pSave->EditGlobalStats      ()                            ->iBadgesEarned += 1u;
    g_pSave->EditLocalStatsMission(iMissionIndex)               ->iBadgesEarned += 1u;
    g_pSave->EditLocalStatsSegment(iMissionIndex, iSegmentIndex)->iBadgesEarned += 1u;

    // 
    ADD_BIT(g_pSave->EditProgress()->aiBadge[iMissionIndex], iSegmentIndex)
}

void cDataTable::GiveBadge()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->GiveBadge(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
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
    // 
    if(m_fChainCooldown)
    {
        m_fChainCooldown.UpdateMax(-1.0f, 0.0f);
        if(!m_fChainCooldown) this->TransferChain();
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
    m_aiComboValue[1] = m_aiComboValue[0] = 0u;
    m_aiChainValue[1] = m_aiChainValue[0] = 0u;
    m_fChainCooldown = 0.0f;
}


// ****************************************************************
// 
void cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    const coreUint32 iFinalValue = bModified ? F_TO_UI(I_TO_F(iValue) * this->GetCurCombo()) : iValue;

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
}

void cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    this->AddScore(iValue, bModified, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}


// ****************************************************************
// 
void cScoreTable::AddCombo(const coreUint32 iValue)
{
    // 
    m_aiComboValue[0] += iValue;
    m_aiComboValue[1]  = MAX(m_aiComboValue[0], m_aiComboValue[1]);
}


// ****************************************************************
// 
void cScoreTable::AddChain(const coreUint32 iValue)
{
    // 
    m_aiChainValue[0] += iValue;
    m_aiChainValue[1]  = MAX(m_aiChainValue[0], m_aiChainValue[1]);

    // 
    m_fChainCooldown = 1.0f;
}


// ****************************************************************
// 
void cScoreTable::ReduceCombo()
{
    // 
    m_aiComboValue[0] /= 2u;
}


// ****************************************************************
// 
void cScoreTable::TransferChain()
{
    if(m_aiChainValue[0])
    {
        // 
        this->AddScore(m_aiChainValue[0], true);
        m_aiChainValue[0] = 0u;

        // 
        m_fChainCooldown = 0.0f;
    }
}


// ****************************************************************
// constructor
cTimeTable::cTimeTable()noexcept
: m_fFrameTime (Core::System->GetTime())
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
    //if(!Core::System->GetTime()) return;
    if(Core::System->GetTime() < 0.001f) return;
    ASSERT(Core::System->GetTime() == m_fFrameTime)

    // 
    m_iTimeEvent += 1u;

    if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
    {
        const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
        const coreUintW iSegmentIndex = g_pGame->GetCurMission()->GetCurSegmentIndex();

        // update total and mission time
        ASSERT(iMissionIndex < TABLE_MISSIONS)
        m_iTimeTotal                   += 1u;
        m_aiTimeMission[iMissionIndex] += 1u;

        // update segment time
        if(iSegmentIndex != MISSION_NO_SEGMENT)
        {
            ASSERT(iSegmentIndex < TABLE_SEGMENTS)
            m_aaiTimeSegment[iMissionIndex][iSegmentIndex] += 1u;
        }
    }

    // 
    ASSERT(m_iTimeEvent > 0u)
    g_pSave->EditGlobalStats()->iTimePlayed += TABLE_TIME_TO_UINT(__TABLE_TIME_CONVERT(m_iTimeEvent)) - TABLE_TIME_TO_UINT(__TABLE_TIME_CONVERT(m_iTimeEvent - 1u));
}


// ****************************************************************
// 
void cTimeTable::Reset()
{
    // reset all time values (# no memset)
    m_iTimeEvent = 0u;
    m_iTimeTotal = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aiTimeMission[j] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_SEGMENTS; ++i) m_aaiTimeSegment[j][i] = MISSION_SEGMENT_IS_BOSS(i) ? F_TO_UI(-INTERFACE_BANNER_DURATION_BOSS * RCP(m_fFrameTime)) : 0u;
}


// ****************************************************************
// 
coreFloat cTimeTable::GetTimeSegmentSafe(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const
{
    // 
    return (iSegmentIndex != MISSION_NO_SEGMENT) ? MAX(this->GetTimeSegment(iMissionIndex, iSegmentIndex), 0.0f) : 0.0f;
}

coreFloat cTimeTable::GetTimeSegmentSafe()const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->GetTimeSegmentSafe(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}