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
#include "0201_cUnknown0201Boss.cpp"
#include "0202_cUnknown0202Boss.cpp"
#include "0203_cLeviathanBoss.cpp"
#include "0301_cUnknown0301Boss.cpp"
#include "0302_cTigerBoss.cpp"
#include "0303_cUnknown0303Boss.cpp"
#include "0401_cUnknown0401Boss.cpp"
#include "0402_cUnknown0402Boss.cpp"
#include "0403_cUnknown0403Boss.cpp"
#include "0501_cUnknown0501Boss.cpp"
#include "0502_cPhalarisBoss.cpp"
#include "0503_cCholBoss.cpp"
#include "0601_cUnknown0601Boss.cpp"
#include "0602_cShelobBoss.cpp"
#include "0603_cUnknown0603Boss.cpp"
#include "0701_cUnknown0701Boss.cpp"
#include "0702_cUnknown0702Boss.cpp"
#include "0703_cUnknown0703Boss.cpp"
#include "9901_cProjectOneBoss.cpp"