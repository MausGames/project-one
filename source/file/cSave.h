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
// TODO: still add backup mechanism, you never know if the application writes garbage even though a temp-file is generated


// ****************************************************************
// 
#define SAVE_FILE_FOLDER    "user"                   // 
#define SAVE_FILE_EXTENSION "p1sv"                   // 
#define SAVE_FILE_MAGIC     (UINT_LITERAL("P1SV"))   // 
#define SAVE_FILE_VERSION   (0x00000001u)            // 

#define SAVE_NAME_LENTH     (128u)                   // 
#define SAVE_PLAYERS        (PLAYERS)                // 
#define SAVE_MISSIONS       (MISSIONS)               // 
#define SAVE_SEGMENTS       (SEGMENTS)               // 
#define SAVE_MEDALS         (MEDAL_MAX)              // 


// ****************************************************************
// 
class cSave final
{
public:
    // 
    struct sGlobalStats final
    {
        coreUint64 iTimePlayed;         // 
        coreUint64 iScoreGained;        // 
        coreUint32 iMissionsDone;       // 
        coreUint32 iBossesDone;         // 
        coreUint32 iWavesDone;          // 
        coreUint32 iEnemiesDone;        // 

        coreUint64 iDamageGiven;        // 
        coreUint32 iDamageTaken;        // 
        coreUint32 iContinuesUsed;      // 
        coreUint32 iRepairsUsed;        // 
        coreUint64 iMovesMade;          // 
        coreUint32 iTurnsMade;          // 
        coreUint32 iRollsMade;          // 
        coreUint64 iBulletsShot;        // 
        coreUint32 aiMedalsEarned[5];   // 
        coreUint32 iFragmentsEarned;    // 
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
        coreUint64 iMovesMade;                    // 
        coreUint32 iTurnsMade;                    // 
        coreUint32 iRollsMade;                    // 
        coreUint64 iBulletsShot;                  // 
        coreUint32 aiMedalsEarned[SAVE_MEDALS];   // 
        coreUint32 iFragmentsEarned;              // 
    };

    // 
    struct sOptions final
    {
        coreUint8 iPlayers;                  // 
        coreUint8 aiWeapon [SAVE_PLAYERS];   // 
        coreUint8 aiSupport[SAVE_PLAYERS];   // 
    };

    // 
    struct sProgress final
    {
        coreBool   bFirstPlay;                                      // 
        coreUint8  aiDisclosure   [SAVE_MISSIONS];                  // 
        coreUint8  aiMedalMission [SAVE_MISSIONS];                  // 
        coreUint8  aaiMedalSegment[SAVE_MISSIONS][SAVE_SEGMENTS];   // 
        coreUint8  aiFragment     [SAVE_MISSIONS];                  // 
        coreUint64 iUnlock;                                         // 
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
    sHeader     m_Header;   // 
    std::string m_sPath;    // 


public:
    cSave()noexcept;
    ~cSave();

    DISABLE_COPY(cSave)

    // 
    sGlobalStats* EditGlobalStats      ();
    sLocalStats*  EditLocalStatsMission(const coreUintW iMissionIndex);
    sLocalStats*  EditLocalStatsMission();
    sLocalStats*  EditLocalStatsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    sLocalStats*  EditLocalStatsSegment();
    sOptions*     EditOptions          ();
    sProgress*    EditProgress         ();

    // 
    coreBool LoadFile();
    void     SaveFile();
    void     Clear();

    // 
    inline const sHeader& GetHeader()const {return m_Header;}


private:
    // 
    static coreUint64 __GenerateChecksum(const sHeader& oHeader);

    // 
    friend coreBool ValidateSave(cSave* OUTPUT pSave);
};


#endif // _P1_GUARD_SAVE_H_