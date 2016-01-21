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
: m_aiTimerLine {}
, m_aiCounter   {}
, m_avVector    {}
, m_iPhase      (0u)
, m_iLevel      (0u)
{
    // 
    for(coreUintW i = 0u; i < BOSS_TIMERS; ++i)
        m_aTimer[i].Set(1.0f, 1.0f, 0u);

    // 
    this->AddStatus(ENEMY_STATUS_BOSS);

    // 
    g_pGame->GetEnemyManager()->BindEnemy(this);
}


// ****************************************************************
// destructor
cBoss::~cBoss()
{
    // 
    this->Kill(false);

    // 
    g_pGame->GetEnemyManager()->UnbindEnemy(this);
}


// ****************************************************************
// 
void cBoss::_PhaseTimer(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreFloat fSpeed, const tLerpFunc&& nLerpFunc, const tTimerUpdateFunc&& nUpdateFunc)
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
void cBoss::_PhaseTicker(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreUint16 iTicks, const coreFloat fRate, const tTickerUpdateFunc&& nUpdateFunc)
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