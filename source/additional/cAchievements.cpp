///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

static coreBool    s_bInitAchievements = false;
static coreBool    s_bInitLeaderboards = false;
static coreUint8   s_iStep             = 0u;
static std::time_t s_iTime             = 0u;

static coreUint32 s_aaiCacheScoreArcade   [SCORE_TYPE][SCORE_DIFFICULTIES]                                 = {};
static coreUint32 s_aaaiCacheScoreMission [SCORE_TYPE][SCORE_DIFFICULTIES][SCORE_MISSIONS]                 = {};
static coreUint32 s_aaaaiCacheScoreSegment[SCORE_TYPE][SCORE_DIFFICULTIES][SCORE_MISSIONS][SCORE_SEGMENTS] = {};

static coreUint32 s_aaiCacheTimeArcade   [SCORE_TYPE][SCORE_DIFFICULTIES]                                 = {};
static coreUint32 s_aaaiCacheTimeMission [SCORE_TYPE][SCORE_DIFFICULTIES][SCORE_MISSIONS]                 = {};
static coreUint32 s_aaaaiCacheTimeSegment[SCORE_TYPE][SCORE_DIFFICULTIES][SCORE_MISSIONS][SCORE_SEGMENTS] = {};


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
    for(coreUintW i = 0u; i < 4u; ++i)
    {
        const coreChar* pcLocal    = PRINT("challenge_%02zu", i + 1u);
        const coreChar* pcPlatform = PRINT("CHALLENGE_%02zu", i + 1u);

        Core::Platform->DefineAchievement(pcLocal, pcPlatform, pcPlatform);
    }
}


// ****************************************************************
// 
void CheckAchievements()
{
    if(g_bDemoVersion || DEFINED(_CORE_SWITCH_)) return;

    // 
    if(++s_iStep >= 8u) s_iStep = 0u;

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

    switch(s_iStep)
    {
    case 0u:
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[0][5];})) Core::Platform->UnlockAchievement("progress_00");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[1][5];})) Core::Platform->UnlockAchievement("progress_01");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[2][5];})) Core::Platform->UnlockAchievement("progress_02");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[3][5];})) Core::Platform->UnlockAchievement("progress_03");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[4][5];})) Core::Platform->UnlockAchievement("progress_04");
        break;

    case 1u:
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[5][5];})) Core::Platform->UnlockAchievement("progress_05");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[6][5];})) Core::Platform->UnlockAchievement("progress_06");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[7][5];})) Core::Platform->UnlockAchievement("progress_07");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[8][5];})) Core::Platform->UnlockAchievement("progress_08");
        if(ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[8][6];})) Core::Platform->UnlockAchievement("progress_09");
        break;

    case 2u:
        nStageFunc(0u);
        nStageFunc(1u);
        break;

    case 3u:
        nStageFunc(2u);
        nStageFunc(3u);
        break;

    case 4u:
        nStageFunc(4u);
        nStageFunc(5u);
        break;

    case 5u:
        nStageFunc(6u);
        nStageFunc(7u);
        break;

    case 6u:
        {
            if(HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiTrophy, TROPHY_ONECOLORCLEAR))
            {
                Core::Platform->UnlockAchievement("challenge_01");
            }

            if([]()
            {
                for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 6u; ++j)
                    {
                        if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[i][j] == MEDAL_DARK);})) return false;
                    }
                }
                if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][5] == MEDAL_DARK);})) return false;
                if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][6] == MEDAL_DARK);})) return false;
                return true;
            }())
            {
                Core::Platform->UnlockAchievement("challenge_02");
            }
        }
        break;

    case 7u:
        {
            if(ALL_MEDAL_ARCADE(iMedal) {return (iMedal == MEDAL_DARK);}))
            {
                Core::Platform->UnlockAchievement("challenge_03");
            }

            if([]()
            {
                for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 5u; ++j)
                    {
                        if(!HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 0u))      return false;
                        if(!HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 1u) && i) return false;
                    }
                }
                return true;
            }())
            {
                Core::Platform->UnlockAchievement("challenge_04");
            }

#if 0

            if([]()
            {
                for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 6u; ++j)
                    {
                        if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[i][j] >= MEDAL_PLATINUM);})) return false;
                    }
                }
                if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][5] >= MEDAL_PLATINUM);})) return false;
                if(!ALL_MEDAL_SEGMENT(aaiMedal) {return (aaiMedal[MISSION_ATER][6] >= MEDAL_PLATINUM);})) return false;
                return true;
            }())
            {
                //Core::Platform->UnlockAchievement("challenge_XX");     
            }
            
            if(ALL_MEDAL_ARCADE(iMedal) {return (iMedal >= MEDAL_PLATINUM);}))
            {
                //Core::Platform->UnlockAchievement("challenge_XX");
            }
            
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
                //Core::Platform->UnlockAchievement("challenge_XX");     
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
            
            coreUintW iAchieved = 0u;
            for(coreUintW i = 0u; i < MISSION_BASE - 1u; ++i)
            {
                for(coreUintW j = 0u; j < 6u; ++j)
                {
                    const coreBool bAchieved = HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 3u);
                    if(bAchieved) iAchieved += 1u;
                }
            }
            if(iAchieved)
            {
                //Core::Platform->UnlockAchievement("challenge_XX");     
            }
#endif
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
    if(g_bDemoVersion || !g_bLeaderboards || DEFINED(_CORE_SWITCH_)) return;

    // 
    if(s_bInitLeaderboards) return;
    s_bInitLeaderboards = true;

    // ...
}


// ****************************************************************
// 
static coreUint16 GetUpdateFreqReal()
{
    // 
    if(GetCurUpdateFreq()) return GetCurUpdateFreq();

    // get current display mode
    SDL_DisplayMode oMode = {};
    SDL_GetCurrentDisplayMode(Core::System->GetDisplayIndex(), &oMode);

    // 
    return (oMode.refresh_rate > 0) ? oMode.refresh_rate : SCORE_PURE_UPDATEFREQ;
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
static void QueueScore(const coreUint8 iMissionIndex, const coreUint8 iSegmentIndex, const coreUint32 iScore, const coreBool bPure, const sScoreData& oData)
{
    // 
    cSave::sScorePack* pPack = MANAGED_NEW(cSave::sScorePack);

    // ...

    // 
    FOR_EACH(it, *g_pSave->GetScoreQueue())
    {
        cSave::sScorePack* pCur = (*it);

        if(((pCur->iStatus == 0u) || (pCur->iStatus == 2u)) &&
           (pCur->iType         == pPack->iType)            &&
           (pCur->iMissionIndex == pPack->iMissionIndex)    &&
           (pCur->iSegmentIndex == pPack->iSegmentIndex)    &&
           (pCur->iScore        <  pPack->iScore))
        {
            g_pSave->GetScoreQueue()->erase(it);
            MANAGED_DELETE(pCur)
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
    // 
    cSave::sScorePack* pPack = MANAGED_NEW(cSave::sScorePack);

    // ...

    // 
    FOR_EACH(it, *g_pSave->GetScoreQueue())
    {
        cSave::sScorePack* pCur = (*it);

        if(((pCur->iStatus == 0u) || (pCur->iStatus == 2u)) &&
           (pCur->iType         == pPack->iType)            &&
           (pCur->iMissionIndex == pPack->iMissionIndex)    &&
           (pCur->iSegmentIndex == pPack->iSegmentIndex)    &&
           (pCur->iScore        >  pPack->iScore))   // #
        {
            g_pSave->GetScoreQueue()->erase(it);
            MANAGED_DELETE(pCur)
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
void UploadLeaderboardsArcade(const coreUint32 iScore, const coreUint32 iTimeShifted)
{
    if(g_bDemoVersion || !g_bLeaderboards || DEFINED(_CORE_SWITCH_)) return;

    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    sScoreData oData = {};
    FillBaseScoreData(&oData);

    // ...

    coreUint32& iScoreCache = s_aaiCacheScoreArcade[oData.iOptionType][oData.iOptionDifficulty];
    if(iScore && (!iScoreCache || (iScoreCache <= iScore)))
    {
        iScoreCache = iScore;

        // ...
    }

    coreUint32& iTimeCache = s_aaiCacheTimeArcade[oData.iOptionType][oData.iOptionDifficulty];
    if(iTimeShifted && (!iTimeCache || (iTimeCache >= iTimeShifted)))
    {
        iTimeCache = iTimeShifted;

        // ...
    }
}

void UploadLeaderboardsMission(const coreUintW iMissionIndex, const coreUint32 iScore, const coreUint32 iTimeShifted)
{
    if(g_bDemoVersion || !g_bLeaderboards || DEFINED(_CORE_SWITCH_)) return;

    ASSERT(STATIC_ISVALID(g_pGame))
    ASSERT(iMissionIndex < SCORE_MISSIONS)

    // 
    sScoreData oData = {};
    FillBaseScoreData(&oData);

    // ...

    coreUint32& iScoreCache = s_aaaiCacheScoreMission[oData.iOptionType][oData.iOptionDifficulty][iMissionIndex];
    if(iScore && (!iScoreCache || (iScoreCache <= iScore)))
    {
        iScoreCache = iScore;

        // ...
    }

    coreUint32& iTimeCache = s_aaaiCacheTimeMission[oData.iOptionType][oData.iOptionDifficulty][iMissionIndex];
    if(iTimeShifted && (!iTimeCache || (iTimeCache >= iTimeShifted)))
    {
        iTimeCache = iTimeShifted;

        // ...
    }
}

void UploadLeaderboardsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex, const coreUint32 iScore, const coreUint32 iTimeShifted)
{
    if(g_bDemoVersion || !g_bLeaderboards || DEFINED(_CORE_SWITCH_)) return;

    ASSERT(STATIC_ISVALID(g_pGame))
    ASSERT(iMissionIndex < SCORE_MISSIONS)
    ASSERT(iSegmentIndex < SCORE_SEGMENTS)

    // 
    sScoreData oData = {};
    FillBaseScoreData(&oData);

    // ...

    coreUint32& iScoreCache = s_aaaaiCacheScoreSegment[oData.iOptionType][oData.iOptionDifficulty][iMissionIndex][iSegmentIndex];
    if(iScore && (!iScoreCache || (iScoreCache <= iScore)))
    {
        iScoreCache = iScore;

        // ...
    }

    coreUint32& iTimeCache = s_aaaaiCacheTimeSegment[oData.iOptionType][oData.iOptionDifficulty][iMissionIndex][iSegmentIndex];
    if(iTimeShifted && (!iTimeCache || (iTimeCache >= iTimeShifted)))
    {
        iTimeCache = iTimeShifted;

        // ...
    }
}


// ****************************************************************
// 
void CheckLeaderboards()
{
    if(g_bDemoVersion || !g_bLeaderboards || DEFINED(_CORE_SWITCH_)) return;

    const std::time_t iNewTime = std::time(NULL);
    const coreBool    bTick    = (iNewTime >= s_iTime + 10u);

    if(!bTick) return;
    s_iTime = iNewTime;

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

    FOR_EACH(it, *g_pSave->GetScoreQueue())
    {
        cSave::sScorePack* pCur = (*it);

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
    }
}


// ****************************************************************
// 
const sScoreData* GetScoreData(const coreScore* pScore)
{
    static sScoreData s_Static;

    const sScoreData* pData = r_cast<const sScoreData*>(pScore->aData);

    if(pData->iVersion <= 2u)
    {
        std::memcpy(&s_Static, pData, sizeof(sScoreData));
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
    Core::Platform->UnlockAchievement("challenge_03");
}

*/