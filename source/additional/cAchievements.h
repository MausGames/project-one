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

// TODO 3: update local score/time cache with download-request data
// TODO 3: add locks just in case
// TODO 3: exe hash

// NOTE: make sure that all achievements can be handled from the save-game


// ****************************************************************
// 
#define SCORE_FILE_MAGIC         (UINT_LITERAL("P1  ") % 0xFFFFu)         // 
#define SCORE_FILE_VERSION       (0x0003u)                                // 

#define SCORE_PLAYERS            (PLAYERS)                                // 
#define SCORE_MISSIONS           (MISSIONS - 2u)                          // (aligned to 2) 
#define SCORE_MISSIONS_EX        (MISSIONS - 3u + CONTINUES)              // 
#define SCORE_SEGMENTS           (SEGMENTS)                               // 
#define SCORE_PURES              (2u)                                     // 
#define SCORE_TYPES              (GAME_TYPE_MAX)                          // 
#define SCORE_DIFFICULTIES       (GAME_DIFFICULTY_MAX)                    // 
#define SCORE_EQUIP_WEAPONS      (EQUIP_WEAPONS)                          // 
#define SCORE_EQUIP_SUPPORTS     (EQUIP_SUPPORTS)                         // 

#define SCORE_PURE_GAMESPEED     (100u)                                   // 
#define SCORE_PURE_BACKROTATION  (1u)                                     // 
#define SCORE_PURE_BACKSPEED     (100u)                                   // 
#define SCORE_PURE_UPDATEFREQ    (60u)                                    // 
#define SCORE_PURE_SHIELD        (0u)                                     // 

#define SCORE_LIMIT_COOLDOWN     (DEFINED(_CORE_SWITCH_) ? 1.2f : 0.0f)   // 
#define SCORE_LIMIT_UPLOAD_RATE  (DEFINED(_CORE_SWITCH_) ? 3u   : 10u)    // 
#define SCORE_LIMIT_UPLOAD_DELAY (8u)                                     // 

STATIC_ASSERT(SCORE_PURE_UPDATEFREQ >= F_TO_UI(FRAMERATE_MIN))

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
    coreUint8  iConfigPureMode;

    coreUint8  iSystemOs;
    coreUint8  iSystemCpu;
    coreUint8  iSystemGpu;

    coreUint16 aiContinues[2];
    coreUint8  iRaise;

    coreUint8  aiPadding1[29];

    coreUint32 aiScoreMission    [SCORE_MISSIONS_EX];
    coreUint32 aiTimeMission     [SCORE_MISSIONS];
    coreUint16 aiShiftGoodMission[SCORE_MISSIONS];
    coreUint16 aiShiftBadMission [SCORE_MISSIONS];
    coreUint8  aaiMedalSegment   [SCORE_MISSIONS / 2u][SCORE_SEGMENTS];

    coreUint8  iBadge;
    coreUint8  aiHelper[SCORE_MISSIONS];

    coreUint16 iReplayID;   // only for temporary use

    coreUint64 iTimestamp;
};

STATIC_ASSERT(sizeof(sScoreData) == 0x100u)
STATIC_ASSERT(coreMath::IsAligned(SCORE_MISSIONS, 2u) && (MEDAL_MAX <= 0x0Fu))


// ****************************************************************
// 
extern void InitAchievements();
extern void CheckAchievements();

extern coreUint64 g_iChallengeUnlocked;   // 


// ****************************************************************
// 
extern void InitLeaderboards();
extern void UploadLeaderboardsArcade (                                                              const coreUint32 iScore, const coreUint32 iTimeShifted, coreByte** OUTPUT ppReplayData, const coreUint32 iReplaySize);
extern void UploadLeaderboardsMission(const coreUintW iMissionIndex,                                const coreUint32 iScore, const coreUint32 iTimeShifted);
extern void UploadLeaderboardsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex, const coreUint32 iScore, const coreUint32 iTimeShifted);
extern void CheckLeaderboards();
extern const sScoreData* GetScoreData(const corePlatformScore* pScore);


#endif // _P1_GUARD_ACHIEVEMENTS_H_