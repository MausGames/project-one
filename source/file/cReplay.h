//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_REPLAY_H_
#define _P1_GUARD_REPLAY_H_

// TODO: 1 replay pro spieler
// TODO: replay-player with play, pause, speed up slow down, time-bar dragging, stage and boss markers auf bar, seitliche liste wie bei speedrun
// TODO: frame must start with 0, offset from keyframe ?
// TODO: track pause, with own event-stream ?
// TODO: get central timestamp (server ?)
// TODO: uInfo loadfile


// ****************************************************************
// 
#define REPLAY_STREAMS          (PLAYERS)   // 

#define REPLAY_FILE_FOLDER      "replays"              // 
#define REPLAY_FILE_EXTENSION   "p1rp"                 // 
#define REPLAY_FILE_MAGIC       (coreUint32('P1RP'))   // 
#define REPLAY_FILE_VERSION     (0x00000001u)          // 

#define REPLAY_NAME_LENTH       (64u)

#define REPLAY_TYPE_MOVE        (0u)   // 
#define REPLAY_TYPE_PRESS       (1u)   // 
#define REPLAY_TYPE_RELEASE     (2u)   // 

#define REPLAY_STATUS_DISABLED  (0u)   // 
#define REPLAY_STATUS_RECORDING (1u)   // 
#define REPLAY_STATUS_PLAYBACK  (2u)   // 


// ****************************************************************
// 
class cReplay final
{
public:
    // 
    struct sHeader final
    {
        coreUint32 iMagic;                               // 
        coreUint32 iVersion;                             // 
        coreChar   acName[REPLAY_NAME_LENTH];            // 

        coreUint32 iExecutableHash;                      // 
        coreUint32 iReplayHash;                          // 

        coreUint64 iStartTimestamp;                      // 
        coreUint64 iEndTimestamp;                        // 

        coreUint32 iScoreTotal;                          // 
        coreUint32 aiScoreMission[10];                   // 
        coreUint32 aaiScoreBoss[10][3];                  // 

        coreFloat  fTimeTotal;                           // 
        coreFloat  afTimeMission[10];                    // 
        coreFloat  aafTimeBoss[10][3];                   // 

        coreUint32 iActionsTotal;                        // 
        coreUint32 aiActionsMission[10];                 // 
        coreUint32 aaiActionsBoss[10][3];                // 

        coreUint8  iStreamCount;                         // 
        coreUint32 iKeyFrameSize;                        // 
        coreUint32 aiStreamPacketSize[REPLAY_STREAMS];   // 
    };
    // TODO: define length constants 

    // 
    struct sKeyFrame final
    {
        coreUint16 iIdentifier;   // 

        coreUint32 iFrame;        // (not read, but set through identifier) 
        coreUint64 iTimestamp;    // 

        // other information 
    };

    // 
    struct sStreamPacket final
    {
        coreUint32 iFrame    : 22;   // 
        coreUint32 iType     : 2;    // 
        coreUint32 iValue    : 4;    // 
        coreUint32 iReserved : 4;    // 
    };
    STATIC_ASSERT(sizeof(sStreamPacket) == 4u)
    STATIC_ASSERT(INPUT_KEYS_ACTION     <= 4u)

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
    void StartRecording();
    void StartPlayback();
    void End();

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
    friend FUNC_NOALIAS coreBool ValidateReplay(cReplay* OUTPUT pReplay);
};


#endif // _P1_GUARD_REPLAY_H_