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
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_BOSSES; ++i) m_aaiScoreBoss[j][i] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_WAVES;  ++i) m_aaiScoreWave[j][i] = 0u;

    // reset combo and chain values (# no memset)
    m_aiComboValue[1] = m_aiComboValue[0] = 0u;
    m_aiChainValue[1] = m_aiChainValue[0] = 0u;
    m_fChainCooldown = 0.0f;
}


// ****************************************************************
// 
void cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iBossIndex, const coreUintW iWaveIndex)
{
    const coreUint32 iFinalValue = bModified ? (iValue * this->GetCurCombo()) : iValue;

    // 
    ASSERT(iMissionIndex < TABLE_MISSIONS)
    m_iScoreTotal                   += iFinalValue;
    m_aiScoreMission[iMissionIndex] += iFinalValue;

    // 
    if(iBossIndex != MISSION_NO_BOSS)
    {
        ASSERT(iBossIndex < TABLE_BOSSES)
        m_aaiScoreBoss[iMissionIndex][iBossIndex] += iFinalValue;
    }

    // 
    if(iWaveIndex != MISSION_NO_WAVE)
    {
        ASSERT(iWaveIndex < TABLE_WAVES)
        m_aaiScoreWave[iMissionIndex][iWaveIndex] += iFinalValue;
    }
}

void cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->AddScore(iValue, bModified, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurBossIndex(), g_pGame->GetCurMission()->GetCurWaveIndex());
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
    m_aiComboValue[0] *= 0.5f;
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
    m_fTimeEvent.Update(1.0f);

    if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
    {
        const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
        const coreUintW iBossIndex    = g_pGame->GetCurMission()->GetCurBossIndex();
        const coreUintW iWaveIndex    = g_pGame->GetCurMission()->GetCurWaveIndex();

        // update total and mission time
        ASSERT(iMissionIndex < TABLE_MISSIONS)
        m_fTimeTotal                  .Update(1.0f);
        m_afTimeMission[iMissionIndex].Update(1.0f);

        // update boss time
        if(iBossIndex != MISSION_NO_BOSS)
        {
            ASSERT(iBossIndex < TABLE_BOSSES)
            m_aafTimeBoss[iMissionIndex][iBossIndex].Update(1.0f);
        }

        // update wave time
        if(iWaveIndex != MISSION_NO_WAVE)
        {
            ASSERT(iWaveIndex < TABLE_WAVES)
            m_aafTimeWave[iMissionIndex][iWaveIndex].Update(1.0f);
        }
    }
}


// ****************************************************************
// 
void cTimeTable::Reset()
{
    // reset all time values (# no memset)
    m_fTimeEvent = 0.0f;
    m_fTimeTotal = 0.0f;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_afTimeMission[j] = 0.0f;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_BOSSES; ++i) m_aafTimeBoss[j][i] = -INTERFACE_BANNER_DURATION;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_WAVES;  ++i) m_aafTimeWave[j][i] = 0.0f;
}


// ****************************************************************
// 
coreFloat cTimeTable::GetTimeBossWave(const coreUintW iMissionIndex, const coreUintW iBossIndex, const coreUintW iWaveIndex)const
{
    // 
    if(iBossIndex != MISSION_NO_BOSS) return this->GetTimeBoss(iMissionIndex, iBossIndex);
    if(iWaveIndex != MISSION_NO_WAVE) return this->GetTimeWave(iMissionIndex, iWaveIndex);
    return 0.0f;
}