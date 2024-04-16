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
// TODO 5: add save indicator somewhere on screen (corner), maybe only for important saves (changes, game start+end) to reduce spam
// TODO 5: output if loading (even backup) or saving did not work -> indicator in game, message box in menu
// TODO 4: fix _WEAPONS and _SUPPORTS defines in save and replay
// TODO 2: disable save stats while playing replays
// TODO 2: disable or handle save stats while in error-mission
// TODO 3: add various static asserts for values and bitfields, on save & replay & table (e.g. STATIC_ASSERT(SEGMENTS * BADGES <= sizeof(aiBadge[0])*8u))
// TODO 1: scoring + leaderboard also needs to be version specific
// TODO 1: iBadge needs to be handled correctly, 64-bit ? 5 badges per segment ? [SAVE_MISSIONS][SAVE_SEGMENTS]
// TODO 1: handle unsuccessful saving (dialog box, retry, cancel, show available disc space, test for write access)
// TODO 1: check for, and ask for import of savegame (+ whole user folder?) from demo (Steam) (could also be general, if savegame is (also) stored globally (delete copy after import)), if no save is available on startup -> needs own menu state in intro menu (show date-time of file, maybe also some meta-data (name, time played, max mission))


// ****************************************************************
// 
#define SAVE_FILE_FOLDER    "files"                  // 
#define SAVE_FILE_EXTENSION "p1sv"                   // 
#define SAVE_FILE_MAGIC     (UINT_LITERAL("P1SV"))   // 
#define SAVE_FILE_VERSION   (0x00000001u)            // 

#define SAVE_NAME_LENGTH    (32u)                    // 
#define SAVE_PLAYERS        (PLAYERS)                // 
#define SAVE_MISSIONS       (MISSIONS)               // 
#define SAVE_SEGMENTS       (SEGMENTS)               // 
#define SAVE_EQUIP_WEAPONS  (EQUIP_WEAPONS)          // 
#define SAVE_EQUIP_SUPPORTS (EQUIP_SUPPORTS)         // 
#define SAVE_MEDALS         (MEDAL_MAX)              // 


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
        coreUint32 iEnemiesDone;                  // TODO 1 

        coreUint64 iDamageGiven;                  // 
        coreUint32 iDamageTaken;                  // 
        coreUint32 iContinuesUsed;                // 
        coreUint32 iRepairsUsed;                  // 
        coreUint32 iShiftGoodAdded;               // TODO 1 (aber nicht bei mission oder segment ende, sollte in echtzeit aktualisiert werden) 
        coreUint32 iShiftBadAdded;                // TODO 1 (aber nicht bei mission oder segment ende, sollte in echtzeit aktualisiert werden) 
        coreUint64 iMovesMade;                    // TODO 1 (+table) 
        coreUint32 iTurnsMade;                    // TODO 1 (+table) 
        coreUint32 iRollsMade;                    // TODO 1 (+table) 
        coreUint64 iBulletsShot;                  // TODO 1 (+table) 
        coreUint64 iChromaCollected;              // TODO 1: remove 
        coreUint32 iItemsCollected;               // TODO 1: remove 
        coreUint32 aiMedalsEarned[SAVE_MEDALS];   // 
        coreUint32 iFragmentsEarned;              // 
        coreUint32 iBadgesEarned;                 // 
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

        coreUint32 iTimeBestShifted;              // (already shifted) 
        coreUint16 iTimeBestShiftGood;            // 
        coreUint16 iTimeBestShiftBad;             // 
        coreUint32 iTimeWorstShifted;             // (already shifted) 
        coreUint16 iTimeWorstShiftGood;           // 
        coreUint16 iTimeWorstShiftBad;            // 

        coreUint64 iDamageGiven;                  // 
        coreUint32 iDamageTaken;                  // 
        coreUint32 iContinuesUsed;                // 
        coreUint32 iRepairsUsed;                  // 
        coreUint32 iShiftGoodAdded;               // TODO 1 (aber nicht bei mission oder segment ende, sollte in echtzeit aktualisiert werden) 
        coreUint32 iShiftBadAdded;                // TODO 1 (aber nicht bei mission oder segment ende, sollte in echtzeit aktualisiert werden) 
        coreUint64 iMovesMade;                    // TODO 1 (+table) 
        coreUint32 iTurnsMade;                    // TODO 1 (+table) 
        coreUint32 iRollsMade;                    // TODO 1 (+table) 
        coreUint64 iBulletsShot;                  // TODO 1 (+table) 
        coreUint64 iChromaCollected;              // TODO 1: remove 
        coreUint32 iItemsCollected;               // TODO 1: remove 
        coreUint32 aiMedalsEarned[SAVE_MEDALS];   // 
        coreUint32 iFragmentsEarned;              // 
        coreUint32 iBadgesEarned;                 // 
    };

    // 
    struct sOptions final
    {
        coreChar  acName[SAVE_NAME_LENGTH];                        // 
        coreUint8 iStandard;                                       // 
        coreUint8 iTraining;                                       // 
        coreUint8 iType;                                           // 
        coreUint8 iMode;                                           // 
        coreUint8 iDifficulty;                                     // 
        coreUint8 aaiWeapon [SAVE_PLAYERS][SAVE_EQUIP_WEAPONS];    // 
        coreUint8 aaiSupport[SAVE_PLAYERS][SAVE_EQUIP_SUPPORTS];   // 
    };

    // 
    struct sProgress final
    {
        // TODO 1: coop, duel, game modes, difficulty (27 dimensions) (reserve memory for 1 more in each dimension ?)
        coreBool   bFirstPlay;                                      // 
        coreUint8  aiAdvance      [SAVE_MISSIONS];                  // 
        coreUint8  aiMedalMission [SAVE_MISSIONS];                  // 
        coreUint8  aaiMedalSegment[SAVE_MISSIONS][SAVE_SEGMENTS];   // 
        coreUint8  aiFragment     [SAVE_MISSIONS];                  // (bitfield) 
        coreUint64 aiBadge        [SAVE_MISSIONS];                  // (bitfield) 
        coreUint64 iTrophy;                                         // (bitfield) 
        coreUint64 iUnlock;                                         // (bitfield) 
        coreUint64 aiHideMessage[2];                                // (bitfield) 
        coreUint64 aiHideNew    [2];                                // (bitfield) 
    };
    
    enum eShowMessage : coreUint8
    {
        SHOW_MESSAGE_FRAGMENT_01 = 0u,
        SHOW_MESSAGE_FRAGMENT_02
    };
    
    enum eShowNew : coreUint8
    {
        SHOW_NEW_FRAGMENT_01 = 0u,
        SHOW_NEW_FRAGMENT_02
    };
    
    // messages after arcade-input when returning from mission, or after pressing START when getting into game
    // "new" disappears after being seen (condition), but gets effective after exiting affected submenu or subsubmenu (even going deeper, but not tab) (cached separate from save, where CanShowNew is managed)
    // accumulated news on buttons and tabs
    
    // coreBool CanShowNew(const coreBool bCondition, const coreUintW iIndex) {}
    // void HideNew(const coreUintW iIndex);
    // void ApplyNew();

    // 
    struct sHeader final
    {
        coreUint32   iMagic;                                              // 
        coreUint32   iVersion;                                            // 

        coreUint64   iSaveTimestamp;                                      // 
        coreUint32   iSaveCount;                                          // 

        // TODO 1: coop, duel, game modes, difficulty (27 dimensions)
        sGlobalStats oGlobalStats;                                        // 
        sLocalStats  aLocalStatsMission [SAVE_MISSIONS];                  // 
        sLocalStats  aaLocalStatsSegment[SAVE_MISSIONS][SAVE_SEGMENTS];   // 

        sOptions     oOptions;                                            // 
        sProgress    oProgress;                                           // 

        coreUint64   iChecksum;                                           // 
    };


private:
    sHeader    m_Header;   // 
    coreString m_sPath;    // 

    coreUint32 m_iToken;   // 


public:
    cSave()noexcept;
    ~cSave();

    DISABLE_COPY(cSave)

    // 
    RETURN_NONNULL sGlobalStats* EditGlobalStats      ();
    RETURN_NONNULL sLocalStats*  EditLocalStatsMission(const coreUintW iMissionIndex);
    RETURN_NONNULL sLocalStats*  EditLocalStatsMission();
    RETURN_NONNULL sLocalStats*  EditLocalStatsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex); // const coreUint8 iMode (with difficulty)
    RETURN_NONNULL sLocalStats*  EditLocalStatsSegment();
    RETURN_NONNULL sOptions*     EditOptions          ();
    RETURN_NONNULL sProgress*    EditProgress         ();

    // 
    coreBool LoadFile();
    void     SaveFile();
    void     Clear();

    // 
    inline const sHeader& GetHeader()const {return m_Header;}


private:
    // 
    static coreBool __LoadHeader (sHeader* OUTPUT pHeader, const coreChar* pcPath);
    static void     __CheckHeader(sHeader* OUTPUT pHeader);

    // 
    static coreUint64 __GenerateChecksum(const sHeader& oHeader);
};


#endif // _P1_GUARD_SAVE_H_