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

// TODO: add more stats, e.g. for favorite weapon, support
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
#define SAVE_BOSSES         (BOSSES)                 // 
#define SAVE_WAVES          (WAVES)                  // 


// ****************************************************************
// 
class cSave final
{
public:
    // 
    struct sGlobalStats final
    {
        coreUint64 iTimePlayed;      // 
        coreUint64 iScoreMade;       // 
        coreUint32 iMissionsDone;    // 
        coreUint32 iBossesDone;      // 
        coreUint32 iWavesDone;       // 
        coreUint32 iEnemiesDone;     // 
        coreUint64 iDamageGiven;     // 
        coreUint32 iDamageTaken;     // 

        coreUint64 aiReserved[16];   // 
    };

    // 
    struct sLocalStats final
    {
        coreUint32 iTimeBest;        // 
        coreUint64 iTimeTotal;       // 
        coreUint32 iScoreBest;       // 
        coreUint64 iScoreTotal;      // 
        coreUint32 iCountStart;      // 
        coreUint32 iCountEnd;        // 

        coreUint64 aiReserved[16];   // 
    };

    // 
    struct sHeader final
    {
        coreUint32   iMagic;                                           // 
        coreUint32   iVersion;                                         // 

        coreUint64   iSaveTimestamp;                                   // 
        coreUint32   iSaveCount;                                       // 

        coreUint8    iOptionPlayers;                                   // 
        coreUint8    aiOptionWeapon [SAVE_PLAYERS];                    // 
        coreUint8    aiOptionSupport[SAVE_PLAYERS];                    // 

        sGlobalStats oGlobalStats;                                     // 
        sLocalStats  aLocalStatsMission[SAVE_MISSIONS];                // 
        sLocalStats  aaLocalStatsBoss  [SAVE_MISSIONS][SAVE_BOSSES];   // 
        sLocalStats  aaLocalStatsWave  [SAVE_MISSIONS][SAVE_WAVES];    // 

        coreBool     bFirstPlay;                                       // 
        coreUint8    aiMission [SAVE_MISSIONS];                        // 
        coreUint8    aiParticle[SAVE_MISSIONS];                        // 

        coreUint64   aiReserved[64];                                   // 

        coreUint64   iChecksum;                                        // 
    };


private:
    sHeader     m_Header;   // 
    std::string m_sPath;    // 


public:
    cSave()noexcept;
    ~cSave();

    DISABLE_COPY(cSave)

    // 
    coreBool LoadFile();
    void     SaveFile();
    void     Clear();

    // 
    inline sHeader* GetHeader() {return &m_Header;}


private:
    // 
    static coreUint64 __GenerateChecksum(const sHeader& oHeader);

    // 
    friend coreBool ValidateSave(cSave* OUTPUT pSave);
};


#endif // _P1_GUARD_SAVE_H_