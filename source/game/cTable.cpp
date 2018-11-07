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
        if(!(m_fChainCooldown = MAX(m_fChainCooldown - Core::System->GetTime(), 0.0f)))
            this->TransferChain();
    }
}


// ****************************************************************
// 
void cScoreTable::Reset()
{
    // (# no memset) 
    m_iScoreTotal = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_aiScoreMission[j] = 0u;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_BOSSES; ++i) m_aaiScoreBoss[j][i] = 0u;

    // 
    m_aiComboValue[1] = m_aiComboValue[0] = 0u;
    m_aiChainValue[1] = m_aiChainValue[0] = 0u;
    m_fChainCooldown = 0.0f;
}


// ****************************************************************
// 
void cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iBossIndex)
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
}

void cScoreTable::AddScore(const coreUint32 iValue, const coreBool bModified)
{
    // 
    this->AddScore(iValue, bModified, g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurBossIndex());
}


// ****************************************************************
// 
void cScoreTable::AddCombo(const coreUint32 iValue)
{
    const coreUint32 iOld = F_TO_UI(this->GetCurCombo());
    {
        // 
        m_aiComboValue[0] += iValue;
        m_aiComboValue[1]  = MAX(m_aiComboValue[0], m_aiComboValue[1]);
    }
    const coreUint32 iNew = F_TO_UI(this->GetCurCombo());

    // 
    if(iOld != iNew) g_pGame->GetCombatText()->AddCombo(iNew, m_pOwner->GetPosition());
}

void cScoreTable::AddCombo(const coreFloat fModifier)
{
    // TODO # 
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
    // TODO # 
}


// ****************************************************************
// 
void cScoreTable::TransferChain()
{
    if(m_aiChainValue[0])
    {
        // 
        g_pGame->GetCombatText()->AddChain(m_aiChainValue[0], m_pOwner->GetPosition());

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
    // 
    m_fTimeEvent.Update(1.0f);

    if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
    {
        // update total and mission time
        m_fTimeTotal.Update(1.0f);
        m_afTimeMission[g_pGame->GetCurMissionIndex()].Update(1.0f);

        // update boss time
        if(g_pGame->GetCurMission()->GetCurBoss()) m_aafTimeBoss[g_pGame->GetCurMissionIndex()][g_pGame->GetCurMission()->GetCurBossIndex()].Update(1.0f);
    }
}


// ****************************************************************
// 
void cTimeTable::Reset()
{
    // (# no memset) 
    m_fTimeEvent = 0.0f;
    m_fTimeTotal = 0.0f;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) m_afTimeMission[j] = 0.0f;
    for(coreUintW j = 0u; j < TABLE_MISSIONS; ++j) for(coreUintW i = 0u; i < TABLE_BOSSES; ++i) m_aafTimeBoss[j][i] = -INTERFACE_BANNER_DURATION;
}