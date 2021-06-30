///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector2 cBoss::s_vPositionPoint = coreVector2(0.0f,0.0f);


// ****************************************************************
// constructor
cBoss::cBoss()noexcept
: m_aiTimerLine   {}
, m_aiCounter     {}
, m_avVector      {}
, m_vLastPosition (coreVector2(FLT_MAX,FLT_MAX))
, m_fLastDirAngle (0.0f)
, m_fLastOriAngle (0.0f)
, m_iPhase        (0u)
, m_iLevel        (0u)
{
    // 
    for(coreUintW i = 0u; i < BOSS_TIMERS; ++i)
        m_aTimer[i].Set(1.0f, 1.0f, 0u);

    // 
    this->AddStatus(ENEMY_STATUS_BOSS);
    this->AddStatus(ENEMY_STATUS_SINGLE);
    this->AddStatus(ENEMY_STATUS_IMMORTAL);

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
void cBoss::ChangePhase(const coreUint8 iPhase)
{
    // 
    this->StorePosition();
    this->StoreRotation();

    // 
    ASSERT(m_iPhase != iPhase)
    m_iPhase = iPhase;
}


// ****************************************************************
// 
void cBoss::StorePosition(const coreVector2 vPos)
{
    m_vLastPosition = vPos / FOREGROUND_AREA;
}

void cBoss::StorePosition()
{
    this->StorePosition(this->GetPosition().xy());
}


// ****************************************************************
// 
void cBoss::StoreRotation(const coreVector3 vDir, const coreVector3 vOri)
{
    m_fLastDirAngle = (vDir.xy())                                                 .Angle();
    m_fLastOriAngle = (vOri.yz() * coreVector2(vDir.x ? RCP(vDir.x) : 0.0f, 1.0f)).Angle();
}

void cBoss::StoreRotation()
{
    this->StoreRotation(this->GetDirection(), this->GetOrientation());
}


// ****************************************************************
// 
void cBoss::_StartBoss()
{
    // 
    g_pGame->GetCurMission()->ActivateBoss(this);
}


// ****************************************************************
// 
void cBoss::_EndBoss(const coreBool bAnimated)
{
    // 
    g_pGame->GetCurMission()->DeactivateBoss();
}


// ****************************************************************
// 
UNITY_BUILD
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
#include "0801_cProjectOneBoss.cpp"
#include "0802_cEigengrauBoss.cpp"