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
: m_iPhase  (0u)
, m_iLevel  (0u)
, m_vOldPos (coreVector2(0.0f,0.0f))
{
    // 
    std::memset(m_aiCounter, 0, sizeof(m_aiCounter));

    // 
    m_MoveTimer.Set(1.0f, 1.0f, 1u);
    for(coreUintW i = 0u; i < BOSS_TIMERS; ++i) m_aShootTimer[i].Set(1.0f, 1.0f, 0);
}


// ****************************************************************
// 
void cBoss::_PhaseMove(const coreVector2& vFromPos, const coreVector2& vToPos, const coreFloat& fSpeed, const std::function<coreFloat(coreFloat, coreFloat, coreFloat)>&& nLerp, const std::function<void()>&& nInitFunc, const std::function<void(coreFloat)>&& nUpdateFunc, const std::function<void()>&& nExitFunc)
{
    if(!m_MoveTimer.GetStatus())
    {
        // 
        m_MoveTimer.Play(CORE_TIMER_PLAY_RESET);

        // 
        nInitFunc();
        nUpdateFunc(0.0f);
    }
    else
    {
        // 
        if(m_MoveTimer.Update(fSpeed))
        {
            // 
            this->SetPosition(coreVector3(vToPos * FOREGROUND_AREA, 0.0f));
            m_vOldPos = vToPos;

            // 
            nUpdateFunc(1.0f);
            nExitFunc();
        }
        else
        {
            // 
            const coreFloat fTime = nLerp(0.0f, 1.0f, m_MoveTimer.GetValue(CORE_TIMER_GET_NORMAL));

            // 
            this->SetPosition(coreVector3(LERP(vFromPos, vToPos, fTime) * FOREGROUND_AREA, 0.0f));
            nUpdateFunc(fTime);
        }
    }
}


// ****************************************************************
// 
void cBoss::_PhaseShoot(const coreUintW& iTimerIndex, const coreUint16& iShots, const coreFloat& fSpeed, const std::function<void()>&& nInitFunc, const std::function<void(coreUint16)>&& nUpdateFunc, const std::function<void()>&& nExitFunc)
{
    ASSERT(iTimerIndex < BOSS_TIMERS)
    coreTimer& oTimer = m_aShootTimer[iTimerIndex];

    if(!oTimer.GetStatus())
    {
        // 
        oTimer.Play(CORE_TIMER_PLAY_RESET);

        // 
        nInitFunc();
    }

    // 
    if(oTimer.Update(fSpeed))
    {
        // 
        nUpdateFunc(oTimer.GetCurLoops());

        if(oTimer.GetCurLoops() >= iShots)
        {
            // 
            oTimer.Stop();
            nExitFunc();
        }
    }
}