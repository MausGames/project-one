///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreUint64 g_iChallengeUnlocked = 0u;

static coreBool    s_bInitAchievements = false;
static coreBool    s_bInitLeaderboards = false;
static coreUint8   s_iStep             = 0u;
static std::time_t s_iTime             = 0u;

static coreUint32 s_aaaiCacheScoreArcade   [SCORE_PURES][SCORE_TYPES][SCORE_DIFFICULTIES]                                 = {};
static coreUint32 s_aaaaiCacheScoreMission [SCORE_PURES][SCORE_TYPES][SCORE_DIFFICULTIES][SCORE_MISSIONS]                 = {};
static coreUint32 s_aaaaaiCacheScoreSegment[SCORE_PURES][SCORE_TYPES][SCORE_DIFFICULTIES][SCORE_MISSIONS][SCORE_SEGMENTS] = {};

static coreUint32 s_aaaiCacheTimeArcade   [SCORE_PURES][SCORE_TYPES][SCORE_DIFFICULTIES]                                 = {};
static coreUint32 s_aaaaiCacheTimeMission [SCORE_PURES][SCORE_TYPES][SCORE_DIFFICULTIES][SCORE_MISSIONS]                 = {};
static coreUint32 s_aaaaaiCacheTimeSegment[SCORE_PURES][SCORE_TYPES][SCORE_DIFFICULTIES][SCORE_MISSIONS][SCORE_SEGMENTS] = {};

static corePlatformFileHandle s_iFileHandle = 0u;
static coreBool               s_bFileLock   = false;


// ****************************************************************
// 
void InitAchievements()
{
    if(g_bDemoVersion || DEFINED(_CORE_SWITCH_)) return;

    // 
    if(s_bInitAchievements) return;
    s_bInitAchievements = true;

    // 
    for(coreUintW i = 0u; i < 10u; ++i)
    {
        const coreChar* pcLocal    = PRINT("progress_%02zu", i);
        const coreChar* pcPlatform = PRINT("PROGRESS_%02zu", i);

        Core::Platform->DefineAchievement(pcLocal, pcPlatform, pcPlatform);
    }

    // 
    for(coreUintW i = 0u; i < 8u; ++i)
    {
        for(coreUintW j = 0u; j < 6u; ++j)
        {
            const coreChar* pcLocal    = PRINT("stage_%02zu_%02zu", i, j + 1u);
            const coreChar* pcPlatform = PRINT("STAGE_%02zu_%02zu", i, j + 1u);

            Core::Platform->DefineAchievement(pcLocal, pcPlatform, pcPlatform);
        }
    }

    // 
    for(coreUintW i = 0u; i < 8u; ++i)
    {
        const coreChar* pcLocal    = PRINT("challenge_%02zu", i + 1u);
        const coreChar* pcPlatform = PRINT("CHALLENGE_%02zu", i + 1u);

        Core::Platform->DefineAchievement(pcLocal, pcPlatform, pcPlatform);
    }

    // 
    Core::Platform->DefineStat("achievement_badge",     "ACHIEVEMENT_BADGE",     "");
    Core::Platform->DefineStat("achievement_trophy",    "ACHIEVEMENT_TROPHY",    "");
    Core::Platform->DefineStat("achievement_stage_s",   "ACHIEVEMENT_STAGE_S",   "");
    Core::Platform->DefineStat("achievement_stage_x",   "ACHIEVEMENT_STAGE_X",   "");
    Core::Platform->DefineStat("achievement_stage_200", "ACHIEVEMENT_STAGE_200", "");
}


// ****************************************************************
// 
void CheckAchievements()
{
    if(g_bDemoVersion || DEFINED(_CORE_SWITCH_)) return;

    // 
    const auto nStageFunc = [](const coreUintW iMissionIndex)
    {
        for(coreUintW i = 0u; i < 6u; ++i)
        {
            if(HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[iMissionIndex][i], 3u))
            {
                Core::Platform->UnlockAchievement(PRINT("stage_%02zu_%02zu", iMissionIndex, i + 1u));
            }
        }
    };

    // 
    if(++s_iStep >= 13u) s_iStep = 0u;

    switch(s_iStep)
    {
    case 0u:
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[0][5];})) Core::Platform->UnlockAchievement("progress_00");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[1][5];})) Core::Platform->UnlockAchievement("progress_01");
        break;

    case 1u:
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[2][5];})) Core::Platform->UnlockAchievement("progress_02");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[3][5];})) Core::Platform->UnlockAchievement("progress_03");
        break;

    case 2u:
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[4][5];})) Core::Platform->UnlockAchievement("progress_04");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[5][5];})) Core::Platform->UnlockAchievement("progress_05");
        break;

    case 3u:
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[6][5];})) Core::Platform->UnlockAchievement("progress_06");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[7][5];})) Core::Platform->UnlockAchievement("progress_07");
        break;

    case 4u:
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[8][5];})) Core::Platform->UnlockAchievement("progress_08");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[8][6];})) Core::Platform->UnlockAchievement("progress_09");
        break;

    case 5u:
        nStageFunc(0u);
        nStageFunc(1u);
        break;

    case 6u:
        nStageFunc(2u);
        nStageFunc(3u);
        break;

    case 7u:
        nStageFunc(4u);
        nStageFunc(5u);
        break;

    case 8u:
        nStageFunc(6u);
        nStageFunc(7u);
        break;

    case 9u:
        {
            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 0u))
            {
                if(HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiTrophy, TROPHY_ONECOLORCLEAR))
                {
                    Core::Platform->UnlockAchievement("challenge_01");
                    ADD_BIT(g_iChallengeUnlocked, 0u)
                }
            }

            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 3u))
            {
                coreUint32 iCount = 0u;
                for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 5u; ++j)
                    {
                        if(HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 0u))      iCount += 1u;
                        if(HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 1u) && i) iCount += 1u;
                    }
                }
                if(iCount >= 75u)
                {
                    Core::Platform->UnlockAchievement("challenge_04");
                    ADD_BIT(g_iChallengeUnlocked, 3u)
                }
                Core::Platform->ModifyStat("achievement_badge", iCount);
            }
        }
        break;

    case 10u:
        {
            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 1u))
            {
                coreUint32 iCount = 0u;
                for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 6u; ++j)
                    {
                        if(ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[i][j] == MEDAL_DARK);})) iCount += 1u;
                    }
                }
                if(ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][5] == MEDAL_DARK);})) iCount += 1u;
                if(ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][6] == MEDAL_DARK);})) iCount += 1u;
                if(iCount >= 50u)
                {
                    Core::Platform->UnlockAchievement("challenge_02");
                    ADD_BIT(g_iChallengeUnlocked, 1u)
                }
                Core::Platform->ModifyStat("achievement_stage_x", iCount);
            }

            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 2u))
            {
                if(ALL_MEDAL_ARCADE(iMedal) {return (iMedal == MEDAL_DARK);}))
                {
                    Core::Platform->UnlockAchievement("challenge_03");
                    ADD_BIT(g_iChallengeUnlocked, 2u)
                }
            }
        }
        break;

    case 11u:
        {
            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 4u))
            {
                coreUint32 iCount = 0u;
                for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 6u; ++j)
                    {
                        if(ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[i][j] >= MEDAL_PLATINUM);})) iCount += 1u;
                    }
                }
                if(ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][5] >= MEDAL_PLATINUM);})) iCount += 1u;
                if(ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][6] >= MEDAL_PLATINUM);})) iCount += 1u;
                if(iCount >= 50u)
                {
                    Core::Platform->UnlockAchievement("challenge_05");
                    ADD_BIT(g_iChallengeUnlocked, 4u)
                }
                Core::Platform->ModifyStat("achievement_stage_s", iCount);
            }

            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 5u))
            {
                if(ALL_MEDAL_ARCADE(iMedal) {return (iMedal >= MEDAL_PLATINUM);}))
                {
                    Core::Platform->UnlockAchievement("challenge_06");
                    ADD_BIT(g_iChallengeUnlocked, 5u)
                }
            }
        }
        break;

    case 12u:
        {
            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 6u))
            {
                coreUint32 iCount = 0u;
                for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 6u; ++j)
                    {
                        if(HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 3u))
                            iCount += 1u;
                    }
                }
                if(iCount >= MENU_EXTRA_TROPHIES)
                {
                    Core::Platform->UnlockAchievement("challenge_07");
                    ADD_BIT(g_iChallengeUnlocked, 6u)
                }
                Core::Platform->ModifyStat("achievement_trophy", iCount);
            }

            // 
            if(!HAS_BIT(g_iChallengeUnlocked, 7u))
            {
                coreUint32 iCount = 0u;
                if(ALL_STATS_SEGMENT(oStats)
                {
                    for(coreUintW i = 0u; i < SAVE_MISSIONS; ++i)
                    {
                        for(coreUintW j = 0u; j < SAVE_SEGMENTS; ++j)
                        {
                            if(HAS_BIT(oStats[i][j].iFeat, FEAT_TWOHUNDRED))
                                iCount += 1u;
                        }
                    }
                    return (iCount >= 10u);
                }))
                {
                    Core::Platform->UnlockAchievement("challenge_08");
                    ADD_BIT(g_iChallengeUnlocked, 7u)
                }
                Core::Platform->ModifyStat("achievement_stage_200", iCount);
            }
        }
        break;

    default:
        UNREACHABLE
        break;
    }
}


// ****************************************************************
// 
void InitLeaderboards()
{
    if(g_bDemoVersion || !g_bLeaderboards) return;

    // 
    if(s_bInitLeaderboards) return;
    s_bInitLeaderboards = true;

    // ...
}


// ****************************************************************
// 
static void FillBaseScoreData(sScoreData* pData)
{
    ASSERT(pData)

    // ...
}


// ****************************************************************
// 
static void DeletePack(cSave::sScorePack* pPack)
{
    const coreUint16 iReplayID = pPack->oData.iReplayID;

    // 
    g_pSave->GetScoreQueue()->erase(pPack);
    MANAGED_DELETE(pPack)   // # after erase

    if(iReplayID)
    {
        // 
        coreBool bStillUsed = false;
        FOR_EACH(it, *g_pSave->GetScoreQueue())
        {
            if((*it)->oData.iReplayID == iReplayID)
            {
                bStillUsed = true;
                break;
            }
        }

        // 
        if(!bStillUsed)
        {
            FOR_EACH(it, *g_pSave->GetReplayQueue())
            {
                if((*it)->iID == iReplayID)
                {
                    cSave::sReplayPack* pReplay = (*it);   // # copy out of iterator

                    g_pSave->GetReplayQueue()->erase(it);
                    SAFE_DELETE_ARRAY(pReplay->pData)
                    MANAGED_DELETE(pReplay)

                    break;
                }
            }
        }
    }
}


// ****************************************************************
// 
static void QueueReplay(coreUint16* OUTPUT piReplayID, coreByte** OUTPUT ppReplayData, const coreUint32 iReplaySize)
{
    ASSERT(piReplayID && ppReplayData)

    if(!(*ppReplayData)) return;
    ASSERT(!(*piReplayID))

    // 
    cSave::sReplayPack* pPack = MANAGED_NEW(cSave::sReplayPack);

    // ...

    // 
    g_pSave->GetReplayQueue()->push_back(pPack);

    // 
    (*piReplayID)   = pPack->iID;
    (*ppReplayData) = NULL;
}


// ****************************************************************
// 
static void QueueScore(const coreUint8 iMissionIndex, const coreUint8 iSegmentIndex, const coreUint32 iScore, const coreBool bPure, const sScoreData& oData)
{
    // 
    cSave::sScorePack* pPack = MANAGED_NEW(cSave::sScorePack);

    // ...

    // 
    FOR_EACH(it, *g_pSave->GetScoreQueue())
    {
        cSave::sScorePack* pCur = (*it);

        if(((pCur->iStatus == 0u) || (pCur->iStatus == 2u) || (pCur->iStatus == 4u)) &&
           (pCur->iType         == pPack->iType)         &&
           (pCur->iMissionIndex == pPack->iMissionIndex) &&
           (pCur->iSegmentIndex == pPack->iSegmentIndex) &&
           (pCur->iScore        <  pPack->iScore))
        {
            DeletePack(pCur);
            break;
        }
    }

    // 
    g_pSave->GetScoreQueue()->push_back(pPack);

    // 
    s_iTime = 0u;
}


// ****************************************************************
// 
static void QueueTime(const coreUint8 iMissionIndex, const coreUint8 iSegmentIndex, const coreUint32 iTimeShifted, const coreBool bPure, const sScoreData& oData)
{
#if 0   // # disable time leaderboards

    // 
    cSave::sScorePack* pPack = MANAGED_NEW(cSave::sScorePack);

    // ...

    // 
    FOR_EACH(it, *g_pSave->GetScoreQueue())
    {
        cSave::sScorePack* pCur = (*it);

        if(((pCur->iStatus == 0u) || (pCur->iStatus == 2u) || (pCur->iStatus == 4u)) &&
           (pCur->iType         == pPack->iType)         &&
           (pCur->iMissionIndex == pPack->iMissionIndex) &&
           (pCur->iSegmentIndex == pPack->iSegmentIndex) &&
           (pCur->iScore        >  pPack->iScore))   // #
        {
            DeletePack(pCur);
            break;
        }
    }

    // 
    g_pSave->GetScoreQueue()->push_back(pPack);

    // 
    s_iTime = 0u;

#endif
}


// ****************************************************************
// 
static coreBool IsPure(const sScoreData& oData)
{
    return (oData.iConfigGameSpeed    == SCORE_PURE_GAMESPEED)    &&
           (oData.iConfigBackRotation == SCORE_PURE_BACKROTATION) &&
           (oData.iConfigBackSpeed    == SCORE_PURE_BACKSPEED)    &&
           (oData.iConfigUpdateFreq   == SCORE_PURE_UPDATEFREQ)   &&
           (oData.aiOptionShield[0]   == SCORE_PURE_SHIELD)       &&
           ((oData.iOptionType == GAME_TYPE_SOLO) || (oData.aiOptionShield[1] == SCORE_PURE_SHIELD));
}


// ****************************************************************
// 
void UploadLeaderboardsArcade(const coreUint32 iScore, const coreUint32 iTimeShifted, coreByte** OUTPUT ppReplayData, const coreUint32 iReplaySize)
{
    if(g_bDemoVersion || !g_bLeaderboards || !g_CurConfig.Game.iLeaderboard || (g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK)) return;

    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    sScoreData oData = {};
    FillBaseScoreData(&oData);

    // ...

    {
        coreUint32& iScoreCache = s_aaaiCacheScoreArcade[0][oData.iOptionType][oData.iOptionDifficulty];
        if(iScore && (!iScoreCache || (iScoreCache < iScore)))
        {
            iScoreCache = iScore;

            // ...
        }
    }
    if(IsPure(oData))
    {
        coreUint32& iScoreCache = s_aaaiCacheScoreArcade[1][oData.iOptionType][oData.iOptionDifficulty];
        if(iScore && (!iScoreCache || (iScoreCache < iScore)))
        {
            iScoreCache = iScore;

            // ...
        }
    }

    {
        coreUint32& iTimeCache = s_aaaiCacheTimeArcade[0][oData.iOptionType][oData.iOptionDifficulty];
        if(iTimeShifted && (!iTimeCache || (iTimeCache > iTimeShifted)))
        {
            iTimeCache = iTimeShifted;

            // ...
        }
    }
    if(IsPure(oData))
    {
        coreUint32& iTimeCache = s_aaaiCacheTimeArcade[1][oData.iOptionType][oData.iOptionDifficulty];
        if(iTimeShifted && (!iTimeCache || (iTimeCache > iTimeShifted)))
        {
            iTimeCache = iTimeShifted;

            // ...
        }
    }
}

void UploadLeaderboardsMission(const coreUintW iMissionIndex, const coreUint32 iScore, const coreUint32 iTimeShifted)
{
    if(DEFINED(_CORE_DEBUG_)) return;

    if(g_bDemoVersion || !g_bLeaderboards || !g_CurConfig.Game.iLeaderboard || (g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK)) return;

    ASSERT(STATIC_ISVALID(g_pGame))
    ASSERT(iMissionIndex < SCORE_MISSIONS)

    // 
    sScoreData oData = {};
    FillBaseScoreData(&oData);

    // ...

    {
        coreUint32& iScoreCache = s_aaaaiCacheScoreMission[0][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex];
        if(iScore && (!iScoreCache || (iScoreCache < iScore)))
        {
            iScoreCache = iScore;

            // ...
        }
    }
    if(IsPure(oData))
    {
        coreUint32& iScoreCache = s_aaaaiCacheScoreMission[1][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex];
        if(iScore && (!iScoreCache || (iScoreCache < iScore)))
        {
            iScoreCache = iScore;

            // ...
        }
    }

    {
        coreUint32& iTimeCache = s_aaaaiCacheTimeMission[0][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex];
        if(iTimeShifted && (!iTimeCache || (iTimeCache > iTimeShifted)))
        {
            iTimeCache = iTimeShifted;

            // ...
        }
    }
    if(IsPure(oData))
    {
        coreUint32& iTimeCache = s_aaaaiCacheTimeMission[1][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex];
        if(iTimeShifted && (!iTimeCache || (iTimeCache > iTimeShifted)))
        {
            iTimeCache = iTimeShifted;

            // ...
        }
    }
}

void UploadLeaderboardsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex, const coreUint32 iScore, const coreUint32 iTimeShifted)
{
    if(DEFINED(_CORE_DEBUG_)) return;

    if(g_bDemoVersion || !g_bLeaderboards || !g_CurConfig.Game.iLeaderboard || (g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK)) return;

    ASSERT(STATIC_ISVALID(g_pGame))
    ASSERT(iMissionIndex < SCORE_MISSIONS)
    ASSERT(iSegmentIndex < SCORE_SEGMENTS)

    // 
    sScoreData oData = {};
    FillBaseScoreData(&oData);

    // ...

    {
        coreUint32& iScoreCache = s_aaaaaiCacheScoreSegment[0][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex][iSegmentIndex];
        if(iScore && (!iScoreCache || (iScoreCache < iScore)))
        {
            iScoreCache = iScore;

            // ...
        }
    }
    if(IsPure(oData))
    {
        coreUint32& iScoreCache = s_aaaaaiCacheScoreSegment[1][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex][iSegmentIndex];
        if(iScore && (!iScoreCache || (iScoreCache < iScore)))
        {
            iScoreCache = iScore;

            // ...
        }
    }

    {
        coreUint32& iTimeCache = s_aaaaaiCacheTimeSegment[0][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex][iSegmentIndex];
        if(iTimeShifted && (!iTimeCache || (iTimeCache > iTimeShifted)))
        {
            iTimeCache = iTimeShifted;

            // ...
        }
    }
    if(IsPure(oData))
    {
        coreUint32& iTimeCache = s_aaaaaiCacheTimeSegment[1][oData.iOptionType][oData.iOptionDifficulty][iMissionIndex][iSegmentIndex];
        if(iTimeShifted && (!iTimeCache || (iTimeCache > iTimeShifted)))
        {
            iTimeCache = iTimeShifted;

            // ...
        }
    }
}


// ****************************************************************
// 
void CheckLeaderboards()
{
    if(g_bDemoVersion || !g_bLeaderboards || !g_CurConfig.Game.iLeaderboard) return;

    const std::time_t iNewTime = std::time(NULL);
    const coreBool    bTick    = (iNewTime >= s_iTime + SCORE_LIMIT_UPLOAD_DELAY);

    if(!bTick) return;
    s_iTime = iNewTime;

    // 
    if(g_pSave->GetScoreQueue()->empty()) return;

    // 
    if(!Core::Platform->HasConnection()) return;

    // ...

    std::sort(g_pSave->GetScoreQueue()->begin(), g_pSave->GetScoreQueue()->end(), [](const cSave::sScorePack* A, const cSave::sScorePack* B)
    {
        // pure before any
        if( HAS_BIT(A->iType, 7u) && !HAS_BIT(B->iType, 7u)) return true;
        if(!HAS_BIT(A->iType, 7u) &&  HAS_BIT(B->iType, 7u)) return false;

        // score before time
        if(!HAS_BIT(A->iType, 0u) &&  HAS_BIT(B->iType, 0u)) return true;
        if( HAS_BIT(A->iType, 0u) && !HAS_BIT(B->iType, 0u)) return false;

        // mission before segment
        if( (A->iSegmentIndex >= SCORE_SEGMENTS) && !(B->iSegmentIndex >= SCORE_SEGMENTS)) return true;
        if(!(A->iSegmentIndex >= SCORE_SEGMENTS) &&  (B->iSegmentIndex >= SCORE_SEGMENTS)) return false;

        // arcade before mission
        if( (A->iMissionIndex >= SCORE_MISSIONS) && !(B->iMissionIndex >= SCORE_MISSIONS)) return true;
        if(!(A->iMissionIndex >= SCORE_MISSIONS) &&  (B->iMissionIndex >= SCORE_MISSIONS)) return false;

        return (A->iSegmentIndex < B->iSegmentIndex);
    });

    coreUintW iRateLimit = SCORE_LIMIT_UPLOAD_RATE;

    FOR_EACH(it, *g_pSave->GetScoreQueue())
    {
        cSave::sScorePack* pCur = (*it);

        const coreBool bHasReplay = (pCur->oData.iReplayID != 0u);

        if(pCur->iStatus == 0u)
        {
            pCur->iStatus = 1u;

            // ...
        }
        else if(pCur->iStatus == 2u)
        {
            pCur->iStatus = 3u;

            // ...
        }
        else if(pCur->iStatus == 2u)
        {
            if(!s_bFileLock)
            {
                s_bFileLock   = true;
                pCur->iStatus = 3u;

                coreByte*  pReplayData = NULL;
                coreUint32 iReplaySize = 0u;
                FOR_EACH(et, *g_pSave->GetReplayQueue())
                {
                    if((*et)->iID == pCur->oData.iReplayID)
                    {
                        pReplayData = (*et)->pData;
                        iReplaySize = (*et)->iSize;
                        break;
                    }
                }

                WARN_IF(!pReplayData)
                {
                    s_iFileHandle = 0u;
                    pCur->iStatus = 4u;
                }
                else
                {
                    // ...
                }
            }
        }
        else if(pCur->iStatus == 4u)
        {
            pCur->iStatus = 5u;

            // ...
        }

        if(--iRateLimit == 0u) break;
    }
}


// ****************************************************************
// 
const sScoreData* GetScoreData(const corePlatformScore* pScore)
{
    static sScoreData s_Static;

    if(!pScore->iDataSize)
    {
        std::memset(&s_Static, 0, sizeof(sScoreData));
        return &s_Static;
    }

    const sScoreData* pData = r_cast<const sScoreData*>(pScore->aData);

    if(pData->iVersion <= 2u)
    {
        std::memcpy (&s_Static, pData, sizeof(sScoreData));
        std::memmove(&s_Static.iSystemOs,      r_cast<coreByte*>(&s_Static) + 34u, sizeof(coreUint8)  * 3u);
        std::memmove( s_Static.aiScoreMission, r_cast<coreByte*>(&s_Static) + 40u, sizeof(coreUint32) * SCORE_MISSIONS);

        return &s_Static;
    }

    return pData;
}


/*

if([]()
{
    for(coreUintW i = 0u; i < MISSION_BASE; ++i)
    {
        if(!ALL_MEDAL_MISSION(aiMedal) {return (aiMedal[i] == MEDAL_DARK);})) return false;
    }
    return true;
}())
{
    Core::Platform->UnlockAchievement("challenge_XX");
}
 
 
if([]()
{
    for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
    {
        for(coreUintW j = 0u; j < 6u; ++j)
        {
            if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[i][j] && (iDifficulty == 2u));})) return false;
        }
    }
    if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][5] && (iDifficulty == 2u));})) return false;
    if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][6] && (iDifficulty == 2u));})) return false;
    return true;
}())
{
    //Core::Platform->UnlockAchievement("challenge_XX");     
}

*/