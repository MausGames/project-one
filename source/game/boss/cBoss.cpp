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
cBoss::cBoss()noexcept
: m_iPhase (0u)
, m_iLevel (0u)
{
    // 
    for(coreUintW i = 0u; i < BOSS_TIMERS; ++i)
        m_aTimer[i].Set(1.0f, 1.0f, 0u);

    // 
    std::memset(m_aiTimerLine, 0, sizeof(m_aiTimerLine));
    std::memset(m_aiCounter,   0, sizeof(m_aiCounter));
    std::memset(m_avVector,    0, sizeof(m_avVector));
}


// ****************************************************************
// 
void cBoss::_PhaseTimer(const coreUintW& iTimerIndex, const coreUint16 iCodeLine, const coreFloat& fSpeed, const std::function<coreFloat(coreFloat, coreFloat, coreFloat)>&& nLerpFunc, const std::function<void(coreFloat, coreFloat, coreBool)>&& nUpdateFunc)
{
    // 
    ASSERT(iTimerIndex < BOSS_TIMERS)
    coreTimer&  oTimer     = m_aTimer     [iTimerIndex];
    coreUint16& iTimerLine = m_aiTimerLine[iTimerIndex];

    // 
    if(iTimerLine != iCodeLine)
    {
        iTimerLine = iCodeLine;

        // 
        oTimer.SetMaxLoops(1u);
        oTimer.Play(CORE_TIMER_PLAY_RESET);

        // 
        nUpdateFunc(0.0f, 0.0f, false);
    }
    else
    {
        // 
        const coreFloat fTimeBefore = nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL));
        oTimer.Update(fSpeed);
        const coreFloat fTime       = nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL));

        // 
        nUpdateFunc(fTime, fTimeBefore, !oTimer.GetStatus());
    }
}


// ****************************************************************
// 
void cBoss::_PhaseTicker(const coreUintW& iTimerIndex, const coreUint16 iCodeLine, const coreUint16& iTicks, const coreFloat& fRate, const std::function<void(coreUint16, coreBool)>&& nUpdateFunc)
{
    // 
    ASSERT(iTimerIndex < BOSS_TIMERS)
    coreTimer&  oTimer     = m_aTimer     [iTimerIndex];
    coreUint16& iTimerLine = m_aiTimerLine[iTimerIndex];

    // 
    if(iTimerLine != iCodeLine)
    {
        iTimerLine = iCodeLine;

        // 
        oTimer.SetMaxLoops(0u);
        oTimer.Play(CORE_TIMER_PLAY_RESET);
    }

    // 
    if(oTimer.Update(fRate))
    {
        // 
        if((oTimer.GetCurLoops() >= iTicks) && iTicks)
            oTimer.Pause();

        // 
        nUpdateFunc(oTimer.GetCurLoops()-1u, !oTimer.GetStatus());
    }
}