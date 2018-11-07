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
// TODO: manual bitpacking, as this is written to file


// ****************************************************************
// 
#define REPLAY_FILE_FOLDER      "replays"              // 
#define REPLAY_FILE_EXTENSION   "p1rp"                 // 
#define REPLAY_FILE_MAGIC       (coreUint32('P1RP'))   // 
#define REPLAY_FILE_VERSION     (0x00000001u)          // 

#define REPLAY_NAME_LENTH       (128u)                 // 
#define REPLAY_STREAMS          (PLAYERS)              // 
#define REPLAY_MISSIONS         (MISSIONS)             // 
#define REPLAY_BOSSES           (BOSSES)               // 

#define REPLAY_TYPE_MOVE        (0u)                   // 
#define REPLAY_TYPE_PRESS       (1u)                   // 
#define REPLAY_TYPE_RELEASE     (2u)                   // 

#define REPLAY_STATUS_DISABLED  (0u)                   // 
#define REPLAY_STATUS_RECORDING (1u)                   // 
#define REPLAY_STATUS_PLAYBACK  (2u)                   // 

#define REPLAY_KEYFRAME_REGULAR(x)       ((x))
#define REPLAY_KEYFRAME_MISSION_START(x) ((x) + 40000u)
#define REPLAY_KEYFRAME_MISSION_END(x)   ((x) + 40100u)
#define REPLAY_KEYFRAME_BOSS_START(x)    ((x) + 50000u)
#define REPLAY_KEYFRAME_BOSS_END(x)      ((x) + 60000u)


// ****************************************************************
// 
class cReplay final
{
public:
    // 
    struct sHeader final
    {
        coreUint32 iPreMagic;                                                           // 
        coreUint32 iVersion;                                                            // 
        coreChar   acName[REPLAY_NAME_LENTH];                                           // 

        coreUint32 iExecutableHash;                                                     // 
        coreUint32 iReplayHash;                                                         // 

        coreUint64 iStartTimestamp;                                                     // 
        coreUint64 iEndTimestamp;                                                       // 

        coreUint8  iGameMode;                                                           // 
        coreUint8  iGameDifficulty;                                                     // 
        coreUint8  iGamePlayers;                                                        // 

        coreUint8  iNumStreams;                                                         // 
        coreUint8  iNumMissions;                                                        // 
        coreUint8  iNumBosses;                                                          // 

        coreInt32  aiMissionList[REPLAY_MISSIONS];                                      // 

        coreUint32 iKeyFrameCount;                                                      // 
        coreUint32 aiStreamPacketCount[REPLAY_STREAMS];                                 // 

        coreFloat  fTimeTotal;                                                          // 
        coreFloat  afTimeMission[REPLAY_MISSIONS];                                      // 
        coreFloat  aafTimeBoss  [REPLAY_MISSIONS][REPLAY_BOSSES];                       // 

        coreUint32 aiScoreTotal   [REPLAY_STREAMS];                                     // 
        coreUint32 aaiScoreMission[REPLAY_STREAMS][REPLAY_MISSIONS];                    // 
        coreUint32 aaaiScoreBoss  [REPLAY_STREAMS][REPLAY_MISSIONS][REPLAY_BOSSES];     // 

        coreUint32 aiActionsTotal   [REPLAY_STREAMS];                                   // 
        coreUint32 aaiActionsMission[REPLAY_STREAMS][REPLAY_MISSIONS];                  // 
        coreUint32 aaaiActionsBoss  [REPLAY_STREAMS][REPLAY_MISSIONS][REPLAY_BOSSES];   // 

        coreUint32 iPostMagic;                                                          // 
    };

    // 
    struct sKeyFrame final
    {
        coreUint16 iIdentifier;   // 

        coreUint32 iFrame;        // 
        coreUint64 iTimestamp;    // 
    };

    // 
    struct sStreamPacket final
    {
        coreUint32 iFrame    : 22;   // (up to 19.4 hours with 60 FPS) 
        coreUint32 iType     : 2;    // 
        coreUint32 iValue    : 5;    // 
        coreUint32 iReserved : 3;    // 
    };
    STATIC_ASSERT(sizeof(sStreamPacket) == 4u)
    STATIC_ASSERT(INPUT_KEYS_ACTION     <= 5u)

    // 
    struct sInfo final
    {
        std::string sPath;     // 
        sHeader     oHeader;   // 
    };


private:
    sHeader m_Header;                                              // 

    std::vector<sKeyFrame>     m_aKeyFrame;                        // 
    std::vector<sStreamPacket> m_aaStreamPacket[REPLAY_STREAMS];   // 

    sGameInput m_aInput[REPLAY_STREAMS];                           // 

    coreUint32 m_iCurFrame;                                        // 
    coreUint32 m_aiCurPacket[REPLAY_STREAMS];                      // 
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
    static void LoadInfoList(std::vector<sInfo>* OUTPUT paInfoList);

    // 
    inline const sHeader&   GetHeader()const {return m_Header;}
    inline const coreUint8& GetStatus()const {return m_iStatus;}


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
    friend coreBool ValidateReplay(cReplay* OUTPUT pReplay);
};


#endif // _P1_GUARD_REPLAY_H_