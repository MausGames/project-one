///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SAVE_H_
#define _P1_GUARD_SAVE_H_

// TODO: add more stats, e.g. for favorite weapon+support, direction
// TODO: move time, shoot time, move WASD, move dir 1234, min and max
// TODO: add save indicator somewhere on screen (corner), maybe only for important saves (changes, game start+end) to reduce spam
// TODO: output if loading (even backup) or saving did not work -> indicator in game, message box in menu
// TODO: fix _WEAPONS and _SUPPORTS defines in save and replay
// TODO: disable save stats while playing replays
// TODO: disable or handle save stats while in error-mission


// ****************************************************************
// 
#define SAVE_FILE_FOLDER    ""                       // 
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
        coreUint64 iTimePlayed;                   // 
        coreUint64 iScoreGained;                  // 
        coreUint32 iMissionsDone;                 // 
        coreUint32 iBossesDone;                   // 
        coreUint32 iWavesDone;                    // 
        coreUint32 iEnemiesDone;                  // TODO 

        coreUint64 iDamageGiven;                  // 
        coreUint32 iDamageTaken;                  // 
        coreUint32 iContinuesUsed;                // 
        coreUint32 iRepairsUsed;                  // 
        coreUint64 iMovesMade;                    // TODO (+table) 
        coreUint32 iTurnsMade;                    // TODO (+table) 
        coreUint32 iRollsMade;                    // TODO (+table) 
        coreUint64 iBulletsShot;                  // TODO (+table) 
        coreUint64 iChromaCollected;              // 
        coreUint32 iItemsCollected;               // 
        coreUint32 aiMedalsEarned[SAVE_MEDALS];   // 
        coreUint32 iFragmentsEarned;              // 
        coreUint32 iBadgesEarned;                 // 
    };

    // 
    struct sLocalStats final
    {
        coreUint32 iTimeBest;                     // 
        coreUint32 iTimeWorst;                    // 
        coreUint64 iTimeTotal;                    // 
        coreUint32 iScoreBest;                    // 
        coreUint32 iScoreWorst;                   // 
        coreUint64 iScoreTotal;                   // 
        coreUint32 iCountStart;                   // 
        coreUint32 iCountEnd;                     // 

        coreUint64 iDamageGiven;                  // 
        coreUint32 iDamageTaken;                  // 
        coreUint32 iContinuesUsed;                // 
        coreUint32 iRepairsUsed;                  // 
        coreUint64 iMovesMade;                    // TODO (+table) 
        coreUint32 iTurnsMade;                    // TODO (+table) 
        coreUint32 iRollsMade;                    // TODO (+table) 
        coreUint64 iBulletsShot;                  // TODO (+table) 
        coreUint64 iChromaCollected;              // 
        coreUint32 iItemsCollected;               // 
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
        coreUint8 iPlayers;                                        // 
        coreUint8 iDifficulty;                                     // 
        coreUint8 aaiWeapon [SAVE_PLAYERS][SAVE_EQUIP_WEAPONS];    // 
        coreUint8 aaiSupport[SAVE_PLAYERS][SAVE_EQUIP_SUPPORTS];   // 
    };

    // 
    struct sProgress final
    {
        coreBool   bFirstPlay;                                      // 
        coreUint8  aiAdvance      [SAVE_MISSIONS];                  // 
        coreUint8  aiMedalMission [SAVE_MISSIONS];                  // 
        coreUint8  aaiMedalSegment[SAVE_MISSIONS][SAVE_SEGMENTS];   // 
        coreUint8  aiFragment     [SAVE_MISSIONS];                  // (bitfield) 
        coreUint32 aiBadge        [SAVE_MISSIONS];                  // (bitfield) 
        coreUint64 iTrophy;                                         // (bitfield) 
        coreUint64 iUnlock;                                         // (bitfield) 
    };

    // 
    struct sHeader final
    {
        coreUint32   iMagic;                                              // 
        coreUint32   iVersion;                                            // 

        coreUint64   iSaveTimestamp;                                      // 
        coreUint32   iSaveCount;                                          // 

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


public:
    cSave()noexcept;
    ~cSave();

    DISABLE_COPY(cSave)

    // 
    RETURN_NONNULL sGlobalStats* EditGlobalStats      ();
    RETURN_NONNULL sLocalStats*  EditLocalStatsMission(const coreUintW iMissionIndex);
    RETURN_NONNULL sLocalStats*  EditLocalStatsMission();
    RETURN_NONNULL sLocalStats*  EditLocalStatsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
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