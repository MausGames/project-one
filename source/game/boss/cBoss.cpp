///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector2 cBoss::s_vPositionPoint = coreVector2(0.0f,0.0f);


// ****************************************************************
// constructor
cBoss::cBoss()noexcept
: m_aiTimerLine      {}
, m_aiCounter        {}
, m_avVector         {}
, m_vLastPosition    (coreVector2(FLT_MAX,FLT_MAX))
, m_vLastDirection   (coreVector3(0.0f,0.0f,0.0f))
, m_vLastOrientation (coreVector3(0.0f,0.0f,0.0f))
, m_iPhase           (0u)
, m_fPhaseTime       (0.0f)
, m_fPhaseTimeBefore (0.0f)
, m_bControlAgain    (false)
, m_iHelperSpawn     (0u)
, m_iHelperHit       (0u)
, m_bActive          (false)
, m_bForeshadow      (false)
{
    // 
    for(coreUintW i = 0u; i < BOSS_TIMERS; ++i)
        m_aTimer[i].Set(1.0f, 1.0f, 0u);

    // 
    this->AddStatus(ENEMY_STATUS_BOSS);
    this->AddStatus(ENEMY_STATUS_SINGLE);
    this->AddStatus(ENEMY_STATUS_IMMORTAL);
    this->AddStatus(ENEMY_STATUS_WORTHLESS);

    // 
    g_pGame->GetEnemyManager()->BindEnemy(this);

    STATIC_ASSERT(BOSS_HELPERS <= sizeof(m_iHelperSpawn)*8u)
    STATIC_ASSERT(BOSS_HELPERS <= sizeof(m_iHelperHit)  *8u)
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
    m_iPhase           = iPhase;
    m_fPhaseTime       = 0.0f;
    m_fPhaseTimeBefore = 0.0f;
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
    ASSERT(vDir.IsNormalized() && vOri.IsNormalized())

    m_vLastDirection   = vDir;
    m_vLastOrientation = vOri;
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
    g_pGame->GetTimeTable()->StartBoss();

    // 
    g_pGame->GetInterface()->ShowBoss(this);
}


// ****************************************************************
// 
void cBoss::_EndBoss()
{
    // 
    m_bActive = false;

    // 
    g_pGame->GetCurMission()->DeactivateBoss();

    // 
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->GetScoreTable()->ResetOverride();
    });

    g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        if(pPlayer->HasStatus(PLAYER_STATUS_DEAD))
        {
            pPlayer->Resurrect();

            pPlayer->SetCurHealth(1u);
            if(pPlayer->HasStatus(PLAYER_STATUS_SHIELDED))
            {
                pPlayer->SetCurShield(5u);
            }
        }
    });
}


// ****************************************************************
// 
void cBoss::_UpdateBoss()
{
    // 
    m_fPhaseTimeBefore = m_fPhaseTime;
    m_fPhaseTime.Update(1.0f);

    if(m_bActive && !m_bForeshadow)
    {
        // 
        g_pGame->ForEachPlayerAll([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->GetScoreTable()->SetOverride(1.0f - this->GetCurHealthPct());
        });
        
        if(InBetween(INTERFACE_BANNER_DURATION_ALERT, m_fLifeTimeBefore, m_fLifeTime))
        {
            const coreChar* pcName = this->GetMusicName();
            if(pcName && pcName[0])
            {
                g_MusicPlayer.SelectName(pcName);
                g_MusicPlayer.Play();
            }
        }
    
    
        for(coreUintW i = 0u; i < BOSS_HELPERS; ++i)
        {
            cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
            coreObject3D* pShield = pHelper->GetShield();
            if(pHelper->HasStatus(HELPER_STATUS_DEAD)) continue;
    
            if(pShield)
            {
                coreBool bHit = false;
                Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, pShield, [&](cBullet* OUTPUT pBullet, const coreObject3D* pShield, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

                    if(!bHit)
                    {
                        bHit = true;
    
                        pBullet->Deactivate(true);
    
                        d_cast<cPlayer*>(pBullet->GetOwner())->GetScoreTable()->RefreshCooldown();
                    }
                });
    
                if(bHit)
                {
                    ADD_BIT(m_iHelperHit, i)
    
                    pHelper->Kill(true);
    
                    g_pGame->GetCombatText()->DrawText("???", pHelper->GetPosition(), COLOR_MENU_INSIDE);

                    g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 1.0f, 1.0f, SOUND_HELPER);

                    if(m_iHelperHit == BITLINE(BOSS_HELPERS))
                    {
                        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                        {
                            pPlayer->ActivateDarkShading();

                            g_pSpecialEffects->CreateSplashDark(pPlayer->GetPosition(), SPECIAL_SPLASH_SMALL);
                        });
                    }
                }
            }
        }
    }
}


// ****************************************************************
// 
void cBoss::_ResurrectBoss()
{
    // 
    m_bActive = true;

    // 
    m_bForeshadow = false;

    // 
    g_pGame->GetCurMission()->ActivateBoss(this);
}


// ****************************************************************
// 
coreBool cBoss::_ResurrectHelper(const coreUint8 iElement, const coreBool bSmooth)
{
    const coreUintW iIndex = iElement - ELEMENT_YELLOW;

    // 
    if(HAS_BIT(m_iHelperSpawn, iIndex)) return false;
    ADD_BIT(m_iHelperSpawn, iIndex);

    // 
    cHelper* pHelper = g_pGame->GetHelper(iElement);

    // 
    pHelper->Resurrect(bSmooth);
    pHelper->EnableShield();

    // 
    pHelper->SetPosition(coreVector3(HIDDEN_POS, 0.0f));

    // TODO 1: sound-effect jingle

    return true;
}


// ****************************************************************
// 
void cBoss::_KillHelper(const coreUint8 iElement, const coreBool bAnimated)
{
    // 
    cHelper* pHelper = g_pGame->GetHelper(iElement);

    // 
    pHelper->Kill(bAnimated);
}


// ****************************************************************
// 
UNITY_BUILD
#include "0101_cDharukBoss.cpp"
#include "0101_cDharukSubBoss.cpp"
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
#include "9901_cIntroBoss.cpp"