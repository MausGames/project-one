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
    this->AddStatus(ENEMY_STATUS_SINGLE);

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
#include "0101_cCrossfieldBoss.cpp"
#include "0102_cTorusBoss.cpp"
#include "0103_cVausBoss.cpp"
#include "9901_cProjectOneBoss.cpp"