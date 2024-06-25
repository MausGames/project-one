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
, m_piHealthGoal     (NULL)
, m_iMaxHealthGoal   (INT32_MAX)
, m_iPhase           (0u)
, m_fPhaseTime       (0.0f)
, m_fPhaseTimeBefore (0.0f)
, m_fStartup         (-1.0f)
, m_bControlAgain    (false)
, m_iHelperSpawn     (0u)
, m_iHelperHit       (0u)
, m_bActive          (false)
, m_bForeshadow      (false)
, m_bSkipped         (g_pGame->SkipLoadingBoss())
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

    STATIC_ASSERT(BOSS_HELPERS <= sizeof(m_iHelperSpawn)*8u)
    STATIC_ASSERT(BOSS_HELPERS <= sizeof(m_iHelperHit)  *8u)
}


// ****************************************************************
// destructor
cBoss::~cBoss()
{
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
coreFloat cBoss::CalcHealthGoal()const
{
    const coreInt32* piGoal = m_piHealthGoal;
    if(piGoal && (*piGoal))
    {
        const coreInt32 iCurHealth = MIN(this->GetCurHealth(), m_iMaxHealthGoal);
        do
        {
            const coreInt32 iHigh = (*(piGoal));
            const coreInt32 iLow  = (*(piGoal + 1u));

            if(iCurHealth > iLow)
            {
                return STEP(I_TO_F(iLow), I_TO_F(iHigh), I_TO_F(iCurHealth));
            }
        }
        while(*(++piGoal));
    }

    return this->GetCurHealthPct();
}


// ****************************************************************
// 
void cBoss::_StartBoss()
{
    // 
    g_pGame->GetTimeTable()->StartBoss();

    // 
    g_pGame->GetInterface()->ShowBoss(this);

    // 
    m_fStartup = INTERFACE_BANNER_DURATION_BOSS;
}


// ****************************************************************
// 
void cBoss::_EndBoss()
{
    ASSERT(!HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED))

    // 
    m_bActive = false;

    // 
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)   // # before DeactivateBoss and __CloseSegment
    {
        pPlayer->GetScoreTable()->CancelCooldown();
        pPlayer->GetScoreTable()->ResetOverride();
    });

    // 
    g_pGame->GetCurMission()->DeactivateBoss();

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    ADD_FLAG(g_pSave->EditProgress()->aiHelper[iMissionIndex], m_iHelperHit)
}


// ****************************************************************
// 
void cBoss::_UpdateBoss()
{
    ASSERT(!m_bSkipped)

    // 
    m_fPhaseTimeBefore = m_fPhaseTime;
    m_fPhaseTime.Update(1.0f);

    // 
    if(m_fStartup > 0.0f) m_fStartup.UpdateMax(-1.0f, 0.0f);

    if(m_bActive && !m_bForeshadow)
    {
        // 
        g_pGame->ForEachPlayerAll([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->GetScoreTable()->SetOverride(1.0f - this->GetCurHealthPct());
        });

        // 
        if(InBetween(INTERFACE_BANNER_DURATION_ALERT, m_fLifeTimeBefore, m_fLifeTime))
        {
            const coreChar* pcName = this->GetMusicName();
            if(pcName && pcName[0])
            {
                g_pGame->CancelFadeMusic();

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

                // # delay by one frame
                Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, pShield, [&](cBullet* OUTPUT pBullet, const coreObject3D* pShield, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

                    if(!bHit)
                    {
                        bHit = true;

                        pBullet->Deactivate(true, vIntersection.xy());

                        d_cast<cPlayer*>(pBullet->GetOwner())->GetScoreTable()->RefreshCooldown();
                        d_cast<cPlayer*>(pBullet->GetOwner())->GetScoreTable()->AddChain(100);
                    }
                });

                if(bHit)
                {
                    ADD_BIT(m_iHelperHit, i)

                    pHelper->Kill(true);

                    g_pGame->GetCombatText()->DrawText(Core::Language->GetString("UNKNOWN"), pHelper->GetPosition(), COLOR_MENU_INSIDE);

                    g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 1.0f, 1.0f, SOUND_HELPER);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW j)
                    {
                        pPlayer->GetDataTable()->GiveHelper(i);
                    });

                    if(this->HasAllHelpers())
                    {
                        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW j)
                        {
                            pPlayer->ActivateDarkShading();
                            g_pSpecialEffects->CreateSplashDark(pPlayer->GetPosition(), SPECIAL_SPLASH_SMALL);
                        });
                    }

                    g_pSave->EditGlobalStats      ()->iHelperHit += 1u;
                    g_pSave->EditLocalStatsArcade ()->iHelperHit += 1u;
                    g_pSave->EditLocalStatsMission()->iHelperHit += 1u;
                    g_pSave->EditLocalStatsSegment()->iHelperHit += 1u;
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
    if(!g_pGame->IsFragment()) return false;

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
    pHelper->Kill(false);

    // 
    if(bAnimated)
    {
        g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, pHelper->GetColor3());
        g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
    }
}


// ****************************************************************
// 
void cBoss::_CreateFragment(const coreUint8 iType, const coreVector2 vPosition)
{
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    const coreUintW iBossIndex    = 0u;

    if(!HAS_BIT(REPLAY_WRAP_PROGRESS_FRAGMENT[iMissionIndex], iBossIndex) || (iType == 8u) || ((iType == 0u) && REPLAY_WRAP_PROGRESS_FIRSTPLAY))
    {
        g_pGame->GetItemManager()->AddItem<cFragmentItem>(vPosition, iType, iMissionIndex, iBossIndex);
    }
}

void cBoss::_CreateFragment(const coreUint8 iType)
{
    // 
    this->_CreateFragment(iType, this->GetPosition().xy());
}


// ****************************************************************
// 
coreInt32 cBoss::_RoundHealth(const coreInt32 iHealth)const
{
    const coreFloat fMaxHealth = I_TO_F(this->GetMaxHealth());
    const coreFloat fPercent   = I_TO_F(iHealth) / fMaxHealth;   // # normal division
    const coreFloat fLerp      = ROUND(LERP(1.0f, 100.0f, fPercent));
    const coreFloat fStep      = STEP(1.0f, 100.0f, fLerp);

    return F_TO_SI(fMaxHealth * fStep);
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
#include "9901_cIntroBoss.cpp"