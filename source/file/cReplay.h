///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_REPLAY_H_
#define _P1_GUARD_REPLAY_H_

// TODO: 1 replay pro spieler
// TODO: replay-player with play, pause, speed up slow down, time-bar dragging, stage and boss markers auf bar, seitliche liste wie bei speedrun
// TODO: frame must start with 0, offset from keyframe ?
// TODO: track pause, with own event-stream ?
// TODO: get central timestamp (server ?)
// TODO: uInfo loadfile
// TODO: replay and save files strings have to run an explicit '\0' check, use get and set function with max value (both read and write)
// TODO: current frame is reset on engine-reset ?
// TODO: add detailed death-stats, to make analyzing problematic situations easier
// TODO: frame-count in stream-packet can be compressed further by using delta-frames or/and injecting wrap-around packets


// ****************************************************************
// 
#define REPLAY_FILE_FOLDER      "user/replays"           // 
#define REPLAY_FILE_EXTENSION   "p1rp"                   // 
#define REPLAY_FILE_MAGIC       (UINT_LITERAL("P1RP"))   // 
#define REPLAY_FILE_VERSION     (0x00000001u)            // 

#define REPLAY_NAME_LENTH       (128u)                   // 
#define REPLAY_PLAYERS          (PLAYERS)                // 
#define REPLAY_MISSIONS         (MISSIONS)               // 
#define REPLAY_SEGMENTS         (SEGMENTS)               // 

#define REPLAY_TYPE_MOVE        (0u)                     // 
#define REPLAY_TYPE_PRESS       (1u)                     // 
#define REPLAY_TYPE_RELEASE     (2u)                     // 

#define REPLAY_STATUS_DISABLED  (0u)                     // 
#define REPLAY_STATUS_RECORDING (1u)                     // 
#define REPLAY_STATUS_PLAYBACK  (2u)                     // 

#define REPLAY_KEYFRAME_REGULAR(x)       ((x))
#define REPLAY_KEYFRAME_MISSION_START(x) ((x) + 40000u)
#define REPLAY_KEYFRAME_MISSION_END(x)   ((x) + 40100u)
#define REPLAY_KEYFRAME_BOSS_START(x)    ((x) + 50000u)
#define REPLAY_KEYFRAME_BOSS_END(x)      ((x) + 60000u)
// TODO: WAVE ? 


// ****************************************************************
// 
class cReplay final
{
public:
    // 
    struct sHeader final
    {
        coreUint32 iMagic;                                                                 // 
        coreUint32 iVersion;                                                               // 

        coreChar   acName[REPLAY_NAME_LENTH];                                              // 
        coreUint64 iStartTimestamp;                                                        // 
        coreUint64 iEndTimestamp;                                                          // 

        coreUint32 iExecutableHash;                                                        // 
        coreUint32 iReplayHash;                                                            // 
        coreUint32 iBodySize;                                                              // 

        // coreUint64 iCustomData (also save)
        // game version (make all gameplay changes with if/else)
        // update frequency
        coreUint8 iCheated; // mark the replay as not compatible with regular clients

        coreUint8  iGameMode;                                                              // 
        coreUint8  iGameDifficulty;                                                        // 
        coreUint8  iGamePlayers;                                                           // 
        coreUint8  iPacifist;                                                              // 

        coreUint8  iNumPlayers;                                                            // 
        coreUint8  iNumMissions;                                                           // 
        coreUint8  iNumSegments;                                                           // 
        coreUint8  iNumWaves;                                                              // 

        coreInt32  aiMissionList[REPLAY_MISSIONS];                                         // 

        coreUint32 iKeyFrameCount;                                                         // 
        coreUint32 aiStreamPacketCount[REPLAY_PLAYERS];                                    // 

        coreUint32 iTimeTotal;                                                             // 
        coreUint32 aiTimeMission [REPLAY_MISSIONS];                                        // 
        coreUint32 aaiTimeSegment[REPLAY_MISSIONS][REPLAY_SEGMENTS];                       // 

        coreUint32 aiScoreTotal    [REPLAY_PLAYERS];                                       // 
        coreUint32 aaiScoreMission [REPLAY_PLAYERS][REPLAY_MISSIONS];                      // 
        coreUint32 aaaiScoreSegment[REPLAY_PLAYERS][REPLAY_MISSIONS][REPLAY_SEGMENTS];     // 

        coreUint32 aiActionsTotal    [REPLAY_PLAYERS];                                     // 
        coreUint32 aaiActionsMission [REPLAY_PLAYERS][REPLAY_MISSIONS];                    // 
        coreUint32 aaaiActionsSegment[REPLAY_PLAYERS][REPLAY_MISSIONS][REPLAY_SEGMENTS];   // 

        coreUint64 iChecksum;                                                              // 
    };

    // 
    struct sKeyFrame final
    {
        coreUint16 iIdentifier;   // 

        coreUint32 iFrame;        // 
        coreUint64 iTimestamp;    // 

        // both player positions and state (roll, feel, lives, continues, scores, shoot)
        // bullets
    };

    // 
    struct sStreamPacket final
    {
        coreUint32 iData;   // 
    };
    struct sStreamPacketRaw final
    {
        coreUint32 iFrame;   // (up to 19.4 hours with 60 FPS) 
        coreUint8  iType;    // 
        coreUint8  iValue;   // 
    };

    // 
    struct sInfo final
    {
        std::string sPath;     // 
        sHeader     oHeader;   // 
    };


private:
    sHeader m_Header;                                              // 

    std::vector<sKeyFrame>     m_aKeyFrame;                        // 
    std::vector<sStreamPacket> m_aaStreamPacket[REPLAY_PLAYERS];   // 

    sGameInput m_aInput[REPLAY_PLAYERS];                           // 

    coreUint32 m_iCurFrame;                                        // 
    coreUint32 m_aiCurPacket[REPLAY_PLAYERS];                      // 
    coreUint8  m_iStatus;                                          // 


public:
    cReplay()noexcept;

    DISABLE_COPY(cReplay)

    // 
    void CreateGame();

    // 
    void StartRecording();
    void StartPlayback();
    void EndRecording();
    void EndPlayback();

    // 
    void ApplyKeyFrame(const coreUint16 iIdentifier);

    // 
    void Update();

    // 
    coreBool LoadFile(const coreChar* pcPath, const coreBool bOnlyHeader);
    void     SaveFile(const coreChar* pcName);
    void     Clear();

    // 
    inline const sHeader&   GetHeader()const {return m_Header;}
    inline const coreUint8& GetStatus()const {return m_iStatus;}

    // 
    static void LoadInfoList(std::vector<sInfo>* OUTPUT paInfoList);


private:
    // 
    void __SetBodyData(const coreByte*   pData,  const coreUint32   iSize);
    void __GetBodyData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize)const;

    // 
    static coreUint32 __CalculateExecutableHash();
    coreUint32        __CalculateReplayHash()const;

    // 
    inline coreBool __CanStartRecording()const {return !m_Header.iReplayHash &&  m_aaStreamPacket[0].empty();}
    inline coreBool __CanStartPlayback ()const {return  m_Header.iReplayHash && !m_aaStreamPacket[0].empty();}

    // 
    static sStreamPacket    __Pack  (const sStreamPacketRaw& oPacket);
    static sStreamPacketRaw __Unpack(const sStreamPacket&    oPacket);

    // 
    static coreUint64 __GenerateChecksum(const sHeader& oHeader);

    // 
    friend coreBool ValidateReplay(cReplay* OUTPUT pReplay);
};


#endif // _P1_GUARD_REPLAY_H_