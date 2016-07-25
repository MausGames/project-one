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
#include "0101_cDharukBoss.cpp"
#include "0102_cTorusBoss.cpp"
#include "0103_cVausBoss.cpp"
#include "0201_cNautilusBoss.cpp"
#include "0202_cAmemasuBoss.cpp"
#include "0203_cLeviathanBoss.cpp"
#include "0301_cUrticaBoss.cpp"
#include "0302_cTigerBoss.cpp"
#include "0303_cLuciferBoss.cpp"
#include "0401_cQuaternionBoss.cpp"
#include "0402_cSarosBoss.cpp"
#include "0403_cMessierBoss.cpp"
#include "0501_cTartarusBoss.cpp"
#include "0502_cPhalarisBoss.cpp"
#include "0503_cCholBoss.cpp"
#include "0601_cFenrirBoss.cpp"
#include "0602_cShelobBoss.cpp"
#include "0603_cZerothBoss.cpp"
#include "0701_cOrlacBoss.cpp"
#include "0702_cGemingaBoss.cpp"
#include "0703_cNagualBoss.cpp"
#include "9901_cProjectOneBoss.cpp"