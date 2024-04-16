///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SAVE_H_
#define _P1_GUARD_SAVE_H_

// TODO 3: add more stats, e.g. for favorite weapon+support, direction
// TODO 5: move time, shoot time, move WASD, move dir 1234, min and max
// TODO 3: add save (and loading) indicator somewhere on screen (corner), maybe only for important saves (changes, game start+end) to reduce spam
// TODO 5: output if loading (even backup) or saving did not work -> indicator in game, message box in menu
// TODO 4: fix _WEAPONS and _SUPPORTS defines in save and replay
// TODO 2: disable save stats while playing replays
// TODO 2: disable or handle save stats while in error-mission
// TODO 3: add various static asserts for values and bitfields, on save & replay & table (e.g. STATIC_ASSERT(SEGMENTS * BADGES <= sizeof(aiBadge[0])*8u))
// TODO 1: scoring + leaderboard also needs to be version specific
// TODO 1: check for, and ask for import of savegame (+ whole user folder?) from demo (Steam) (save_demo.p1sv or user_demo_1000 folder, could also be general, if savegame is (also) stored globally (delete copy after import)), if no save is available on startup -> needs own menu state in intro menu (show date-time of file, maybe also some meta-data (name, time played, max mission))
// TODO 2: iDamageGiven and iEnemiesDone only considers non-worthless enemies (also not objects)


// ****************************************************************
// 
#define SAVE_FILE_FOLDER    ""                       // 
#define SAVE_FILE_EXTENSION "p1sv"                   // 
#define SAVE_FILE_MAGIC     (UINT_LITERAL("P1SV"))   // 
#define SAVE_FILE_VERSION   (0x00000003u)            // 

#define SAVE_NAME_LENGTH    (32u)                    // 
#define SAVE_PLAYERS        (PLAYERS)                // 
#define SAVE_MISSIONS       (MISSIONS)               // 
#define SAVE_SEGMENTS       (SEGMENTS)               // 
#define SAVE_TYPES          (3u + 1u)                // 
#define SAVE_MODES          (3u + 1u)                // 
#define SAVE_DIFFICULTIES   (3u + 1u)                // 
#define SAVE_EQUIP_WEAPONS  (EQUIP_WEAPONS)          // 
#define SAVE_EQUIP_SUPPORTS (EQUIP_SUPPORTS)         // 
#define SAVE_MEDALS         (MEDAL_MAX)              // 

enum eSaveStatus : coreUint8
{
    SAVE_STATUS_OK            = 0u,
    SAVE_STATUS_ERROR_UNKNOWN = 1u,
    SAVE_STATUS_ERROR_SPACE   = 2u,
    SAVE_STATUS_ERROR_ACCESS  = 3u
};

enum eSaveFeat : coreUint8   // # never change bits after release
{
    FEAT_TWOHUNDRED = 0u
};

enum eSaveTrophy : coreUint8   // # never change bits after release
{
    TROPHY_ONECOLORCLEAR = 0u
};

enum eSaveUnlock : coreUint8   // # never change bits after release
{
    UNLOCK_MIRRORMODE   = 0u,
    UNLOCK_GAMESPEEDUP  = 1u,
    UNLOCK_POWERSHIELD  = 2u,
    UNLOCK_WEAPON_PULSE = 10u,   // TODO 1: look at loca-text
    UNLOCK_WEAPON_WAVE  = 11u,
    UNLOCK_WEAPON_TESLA = 12u,
    UNLOCK_WEAPON_ANTI  = 13u,
};

enum eSaveNew : coreUint8   // # never change bits after release
{
    NEW_MAIN_START        = 0u,
    NEW_MAIN_EXTRA        = 1u,
    NEW_MAIN_CONFIG       = 2u,
    NEW_CONFIG_GAME       = 10u,
    NEW_CONFIG_MIRRORMODE = 11u,
    NEW_ARMORY_GAMESPEED  = 20u,
    NEW_ARMORY_SHIELD     = 21u,
    NEW_ARMORY_WEAPON     = 22u
};

enum eSaveState : coreUint8   // # never change bits after release
{
    STATE_AFTER_FIRST   = 0u,
    STATE_STORY_VIRIDO  = 1u,
    STATE_STORY_NEVO    = 2u,
    STATE_STORY_HARENA  = 3u,
    STATE_STORY_RUTILUS = 4u,
    STATE_STORY_GELU    = 5u,
    STATE_STORY_CALOR   = 6u,
    STATE_STORY_MUSCUS  = 7u,
    STATE_STORY_ATER    = 8u,
    STATE_STORY_BONUS1  = 9u,
    STATE_STORY_BONUS2  = 10u
};


                  
template <typename T, typename F> coreBool AllVariants(T& tArray, F&& nFunction)
{
    for(coreUintW i = 0u; i < SAVE_TYPES; ++i)
    {
        for(coreUintW j = 0u; j < SAVE_MODES; ++j)
        {
            for(coreUintW k = 0u; k < SAVE_DIFFICULTIES; ++k)
            {
                if(nFunction(tArray[i][j][k]))
                    return true;
            }
        }
    }
    return false;
}

#define ALL_MEDAL_ARCADE(a)  AllVariants(g_pSave->GetHeader().oProgress.aaaiMedalArcade,    [&](const coreUint8 a)
#define ALL_MEDAL_MISSION(a) AllVariants(g_pSave->GetHeader().oProgress.aaaaiMedalMission,  [&](const coreUint8 a[SAVE_MISSIONS])
#define ALL_MEDAL_SEGMENT(a) AllVariants(g_pSave->GetHeader().oProgress.aaaaaiMedalSegment, [&](const coreUint8 a[SAVE_MISSIONS][SAVE_SEGMENTS])

#define ALL_STATS_ARCADE(a)  AllVariants(g_pSave->GetHeader().aaaLocalStatsArcade,    [&](const cSave::sLocalStats&  a)
#define ALL_STATS_MISSION(a) AllVariants(g_pSave->GetHeader().aaaaLocalStatsMission,  [&](const cSave::sLocalStats (&a)[SAVE_MISSIONS])
#define ALL_STATS_SEGMENT(a) AllVariants(g_pSave->GetHeader().aaaaaLocalStatsSegment, [&](const cSave::sLocalStats (&a)[SAVE_MISSIONS][SAVE_SEGMENTS])
                     


// ****************************************************************
// 
class cSave final
{
public:
    // 
    struct sGlobalStats final
    {
        coreUint64 iScoreGained;                  // 
        coreUint64 iTimePlayed;                   // 
        coreUint32 iMissionsDone;                 // 
        coreUint32 iBossesDone;                   // 
        coreUint32 iWavesDone;                    // 
        coreUint32 iEnemiesDone;                  // 

        coreUint64 iDamageGiven;                  // 
        coreUint32 iDamageTaken;                  // 
        coreUint32 iContinuesUsed;                // 
        coreUint32 iRepairsUsed;                  // 
        coreUint32 iShiftGoodAdded;               // 
        coreUint32 iShiftBadAdded;                // 
        coreUint32 iTurnsMade;                    // 
        coreUint64 iMovesMade;                    // 
        coreUint64 iBulletsShot;                  // 
        coreUint32 aiMedalsEarned[SAVE_MEDALS];   // 
        coreUint32 iBadgesEarned;                 // 
        coreUint32 iHelperHit;                    // 
    };

    // 
    struct sLocalStats final
    {
        coreUint32 iScoreBest;                    // 
        coreUint32 iScoreWorst;                   // 
        coreUint64 iScoreTotal;                   // 
        coreUint32 iTimeBest;                     // 
        coreUint32 iTimeWorst;                    // 
        coreUint64 iTimeTotal;                    // 
        coreUint32 iCountStart;                   // 
        coreUint32 iCountEnd;                     // 

        coreUint32 iMaxSeries;                    // 

        coreUint32 iTimeBestShifted;              // (already shifted) 
        coreUint16 iTimeBestShiftGood;            // 
        coreUint16 iTimeBestShiftBad;             // 
        coreUint32 iTimeWorstShifted;             // (already shifted) 
        coreUint16 iTimeWorstShiftGood;           // 
        coreUint16 iTimeWorstShiftBad;            // 

        coreUint32 iFeat;                         // (bitfield) 

        coreUint64 iDamageGiven;                  // 
        coreUint32 iDamageTaken;                  // 
        coreUint32 iContinuesUsed;                // 
        coreUint32 iRepairsUsed;                  // 
        coreUint32 iShiftGoodAdded;               // 
        coreUint32 iShiftBadAdded;                // 
        coreUint32 iTurnsMade;                    // 
        coreUint64 iMovesMade;                    // 
        coreUint64 iBulletsShot;                  // 
        coreUint32 aiMedalsEarned[SAVE_MEDALS];   // 
        coreUint32 iBadgesEarned;                 // 
        coreUint32 iHelperHit;                    // 
    };

    // 
    struct sOptions final
    {
        coreChar   acName[SAVE_NAME_LENGTH];                        // 
        coreUint32 iNavigation;                                     // 
        coreUint8  iType;                                           // 
        coreUint8  iMode;                                           // 
        coreUint8  iDifficulty;                                     // 
        coreUint8  iFlags;                                          // (bitfield) 
        coreUint8  aiShield  [SAVE_PLAYERS];                        // 
        coreUint8  aaiWeapon [SAVE_PLAYERS][SAVE_EQUIP_WEAPONS];    // 
        coreUint8  aaiSupport[SAVE_PLAYERS][SAVE_EQUIP_SUPPORTS];   // 
    };

    // 
    struct sProgress final
    {
        coreBool   bFirstPlay;                                         // 
        coreUint8  aiAdvance         [SAVE_MISSIONS];                  // 
        coreUint8  aaaiMedalArcade   [SAVE_TYPES][SAVE_MODES][SAVE_DIFFICULTIES];                                 // 
        coreUint8  aaaaiMedalMission [SAVE_TYPES][SAVE_MODES][SAVE_DIFFICULTIES][SAVE_MISSIONS];                  // 
        coreUint8  aaaaaiMedalSegment[SAVE_TYPES][SAVE_MODES][SAVE_DIFFICULTIES][SAVE_MISSIONS][SAVE_SEGMENTS];   // 
        coreUint8  aiFragment        [SAVE_MISSIONS];                  // (bitfield) 
        coreUint8  aaiBadge          [SAVE_MISSIONS][SAVE_SEGMENTS];   // (bitfield) 
        coreUint8  aiHelper          [SAVE_MISSIONS];                  // (bitfield) 
        coreUint64 aiTrophy          [2];                              // (bitfield) 
        coreUint64 aiUnlock          [2];                              // (bitfield) 
        coreUint64 aiNew             [2];                              // (bitfield) 
        coreUint64 aiState           [1];                              // (bitfield) 
    };

    // 
    struct sHeader final
    {
        coreUint32   iMagic;           // 
        coreUint32   iVersion;         // 

        coreUint64   iSaveTimestamp;   // 
        coreUint32   iSaveCount;       // 

        sGlobalStats oGlobalStats;     // 
        sLocalStats  aaaLocalStatsArcade   [SAVE_TYPES][SAVE_MODES][SAVE_DIFFICULTIES];                                 // 
        sLocalStats  aaaaLocalStatsMission [SAVE_TYPES][SAVE_MODES][SAVE_DIFFICULTIES][SAVE_MISSIONS];                  // 
        sLocalStats  aaaaaLocalStatsSegment[SAVE_TYPES][SAVE_MODES][SAVE_DIFFICULTIES][SAVE_MISSIONS][SAVE_SEGMENTS];   // 

        sOptions     oOptions;         // 
        sProgress    oProgress;        // 

        coreUint64   iChecksum;        // 
    };


private:
    sHeader    m_Header;                 // 
    coreString m_sPath;                  // 

    coreUint32 m_iToken;                 // 
    coreBool   m_bIgnore;                // 

    coreAtomic<eSaveStatus> m_eStatus;   // 


public:
    cSave()noexcept;
    ~cSave();

    DISABLE_COPY(cSave)

    // 
    RETURN_NONNULL sGlobalStats* EditGlobalStats      ();
    RETURN_NONNULL sLocalStats*  EditLocalStatsArcade (const coreUint8 iType, const coreUint8 iMode, const coreUint8 iDifficulty);
    RETURN_NONNULL sLocalStats*  EditLocalStatsArcade ();
    RETURN_NONNULL sLocalStats*  EditLocalStatsMission(const coreUint8 iType, const coreUint8 iMode, const coreUint8 iDifficulty, const coreUintW iMissionIndex);
    RETURN_NONNULL sLocalStats*  EditLocalStatsMission(const coreUintW iMissionIndex);
    RETURN_NONNULL sLocalStats*  EditLocalStatsMission();
    RETURN_NONNULL sLocalStats*  EditLocalStatsSegment(const coreUint8 iType, const coreUint8 iMode, const coreUint8 iDifficulty, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    RETURN_NONNULL sLocalStats*  EditLocalStatsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    RETURN_NONNULL sLocalStats*  EditLocalStatsSegment();
    RETURN_NONNULL sOptions*     EditOptions          ();
    RETURN_NONNULL sProgress*    EditProgress         ();

    // 
    coreBool LoadFile();
    void     SaveFile();
    void     Clear();

    // 
    inline void ResetStatus() {m_eStatus = SAVE_STATUS_OK;}

    // 
    inline void SetIgnore(const coreBool bIgnore) {m_bIgnore = bIgnore;}

    // 
    inline const sHeader& GetHeader()const {return m_Header;}
    inline eSaveStatus    GetStatus()const {return m_eStatus;}


private:
    // 
    static coreBool __LoadHeader   (sHeader* OUTPUT pHeader, const coreChar* pcPath);
    static void     __UpgradeHeader(sHeader* OUTPUT pHeader);
    static void     __CheckHeader  (sHeader* OUTPUT pHeader);

    // 
    static coreUint64 __GenerateChecksum(const sHeader& oHeader);
};


#endif // _P1_GUARD_SAVE_H_