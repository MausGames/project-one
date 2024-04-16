///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

static coreBool  s_bInit = false;
static coreUint8 s_iStep = 0u;


// ****************************************************************
// 
void InitAchievements()
{
    if(g_bDemoVersion) return;

    // 
    if(s_bInit) return;
    s_bInit = true;

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
    if(g_bDemoVersion) return;

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
                for(coreUintW i = 1u; i < MISSION_BASE - 1u; ++i)
                {
                    for(coreUintW j = 0u; j < 5u; ++j)
                    {
                        if(!HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 0u)) return false;
                        if(!HAS_BIT(g_pSave->GetHeader().oProgress.aaiBadge[i][j], 1u)) return false;
                    }
                }
                return true;
            }())
            {
                Core::Platform->UnlockAchievement("challenge_04");
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }
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
    return (iCount >= 6u);
}))
{
    Core::Platform->UnlockAchievement("challenge_06");
}


*/