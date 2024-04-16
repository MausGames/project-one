///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ACHIEVEMENTS_H_
#define _P1_GUARD_ACHIEVEMENTS_H_

// NOTE: make sure that all achievements can be handled from the save-game


// ****************************************************************
// 
#define SCORE_FILE_MAGIC     (UINT_LITERAL("P1  ") % 0xFFFFu)   // 
#define SCORE_FILE_VERSION   (0x0001u)                          // 

#define SCORE_PLAYERS        (PLAYERS)                          // 
#define SCORE_MISSIONS       (MISSIONS - 2u)                    // 
#define SCORE_SEGMENTS       (SEGMENTS)                         // 
#define SCORE_EQUIP_WEAPONS  (EQUIP_WEAPONS)                    // 
#define SCORE_EQUIP_SUPPORTS (EQUIP_SUPPORTS)                   // 

struct sScoreData final
{
    coreUint16 iMagic;
    coreUint16 iVersion;

    coreUint8  iOptionKind;
    coreUint8  iOptionType;
    coreUint8  iOptionMode;
    coreUint8  iOptionDifficulty;
    coreUint8  iOptionFlags;
    coreUint8  aiOptionShield  [SCORE_PLAYERS];
    coreUint8  aaiOptionWeapon [SCORE_PLAYERS][SCORE_EQUIP_WEAPONS];
    coreUint8  aaiOptionSupport[SCORE_PLAYERS][SCORE_EQUIP_SUPPORTS];

    coreChar   acConfigLanguage   [2];
    coreUint8  aiConfigInputType  [SCORE_PLAYERS];
    coreUint8  aiConfigFireMode   [SCORE_PLAYERS];
    coreUint8  aiConfigControlMode[SCORE_PLAYERS];
    coreUint8  iConfigGameDirection;
    coreUint8  iConfigGameSpeed;
    coreUint8  iConfigMirrorMode;
    coreUint8  iConfigHudDirection;
    coreUint8  iConfigHudType;
    coreUint8  iConfigBackRotation;
    coreUint8  iConfigBackSpeed;
    coreUint16 iConfigUpdateFreq;
    coreUint16 iConfigVersion;

    coreUint8  iSystemOs;
    coreUint8  iSystemCpu;
    coreUint8  iSystemGpu;

    coreUint32 aaiScoreMission   [SCORE_PLAYERS][SCORE_MISSIONS];
    coreUint32 aiTimeMission     [SCORE_MISSIONS];
    coreUint16 aiShiftGoodMission[SCORE_MISSIONS];
    coreUint16 aiShiftBadMission [SCORE_MISSIONS];
    coreUint8  aaiMedalSegment   [SCORE_MISSIONS / 2][SCORE_SEGMENTS];

    coreUint8  iBadge;
    coreUint8  aiHelper[SCORE_MISSIONS];
};
STATIC_ASSERT(sizeof(sScoreData) <= 0xFFu)
STATIC_ASSERT(coreMath::IsAligned(SCORE_MISSIONS, 2u) && (MEDAL_MAX <= 0x0Fu))


// ****************************************************************
// 
extern void InitAchievements();
extern void CheckAchievements();


// ****************************************************************
// 
extern void InitLeaderboards();
extern void UploadLeaderboardsArcade (                                                              const coreUint32 iScore);
extern void UploadLeaderboardsMission(const coreUintW iMissionIndex,                                const coreUint32 iScore);
extern void UploadLeaderboardsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex, const coreUint32 iScore);


#endif // _P1_GUARD_ACHIEVEMENTS_H_