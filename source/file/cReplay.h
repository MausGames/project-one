///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_REPLAY_H_
#define _P1_GUARD_REPLAY_H_

// TODO 5: replay-player with play, pause, speed up slow down, time-bar dragging, stage and boss markers auf bar, seitliche liste wie bei speedrun
// TODO 1: track pause, with own event-stream ?
// TODO 5: sInfo loadfile
// TODO 3: add detailed death-stats, to make analyzing problematic situations easier
// TODO 3: frame-count in stream-packet can be compressed further by using delta-frames and injecting wrap-around packets at max delta
// TODO 3: check if num of streams and snapshots and sizes per stream would exceed anything, also replayhash + bodysize (those two are referencing the second file)
// TODO 1: snapshot data: both player positions and state (roll, feel, force, interrupt, health, dead/repair (stats), continues, scores, shoot (timer), input), bullets, (chroma (bullets)), background
// TODO 1: handle unsuccessful saving


// ****************************************************************
// 
#define REPLAY_FILE_FOLDER      "replays"                // 
#define REPLAY_FILE_EXTENSION   "p1rp"                   // 
#define REPLAY_FILE_MAGIC       (UINT_LITERAL("P1RP"))   // 
#define REPLAY_FILE_VERSION     (0x00000001u)            // 

#define REPLAY_NAME_LENGTH      (128u)                   // 
#define REPLAY_PLAYERS          (PLAYERS)                // 
#define REPLAY_MISSIONS         (MISSIONS)               // 
#define REPLAY_SEGMENTS         (SEGMENTS)               // 
#define REPLAY_EQUIP_WEAPONS    (EQUIP_WEAPONS)          // 
#define REPLAY_EQUIP_SUPPORTS   (EQUIP_SUPPORTS)         // 

#define REPLAY_SNAPSHOT_REGULAR(x)         (x)
#define REPLAY_SNAPSHOT_MISSION_START(m)   (10000u + 100u * (m) + 0u)
#define REPLAY_SNAPSHOT_MISSION_END(m)     (10000u + 100u * (m) + 1u)
#define REPLAY_SNAPSHOT_SEGMENT_START(m,s) (20000u + 100u * (m) + 1u * (s))
#define REPLAY_SNAPSHOT_SEGMENT_END(m,s)   (20000u + 100u * (m) + 1u * (s))

enum eReplayType : coreUint8
{
    REPLAY_TYPE_MOVE    = 0u,   // 
    REPLAY_TYPE_PRESS   = 1u,   // 
    REPLAY_TYPE_RELEASE = 2u    // 
};

enum eReplayStatus : coreUint8
{
    REPLAY_STATUS_DISABLED  = 0u,   // 
    REPLAY_STATUS_RECORDING = 1u,   // 
    REPLAY_STATUS_PLAYBACK  = 2u    // 
};


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

        coreChar   acName[REPLAY_NAME_LENGTH];                                             // 
        coreUint64 iStartTimestamp;                                                        // 
        coreUint64 iEndTimestamp;                                                          // 

        coreUint32 iFrameCount;                                                            // 
        coreUint32 iSnapshotCount;                                                         // 
        coreUint32 aiPacketCount[REPLAY_PLAYERS];                                          // 

        coreUint8  iNumPlayers;                                                            // 
        coreUint8  iNumMissions;                                                           // 
        coreUint8  iNumSegments;                                                           // 

        coreUint8  iOptionType;                                                            // 
        coreUint8  iOptionMode;                                                            // 
        coreUint8  iOptionDifficulty;                                                      // 
        coreUint8  aaiOptionWeapon [REPLAY_PLAYERS][REPLAY_EQUIP_WEAPONS];                 // 
        coreUint8  aaiOptionSupport[REPLAY_PLAYERS][REPLAY_EQUIP_SUPPORTS];                // 

        coreUint16 iConfigUpdateFreq;                                                      // 
        coreUint16 iConfigVersion;                                                         // 

        coreUint8  iMissionStartIndex;                                                     // 
        coreInt32  aiMissionList[REPLAY_MISSIONS];                                         // 

        coreUint32 aiScoreTotal    [REPLAY_PLAYERS];                                       // 
        coreUint32 aaiScoreMission [REPLAY_PLAYERS][REPLAY_MISSIONS];                      // 
        coreUint32 aaaiScoreSegment[REPLAY_PLAYERS][REPLAY_MISSIONS][REPLAY_SEGMENTS];     // 

        coreUint32 iTimeTotal;                                                             // 
        coreUint32 aiTimeMission [REPLAY_MISSIONS];                                        // 
        coreUint32 aaiTimeSegment[REPLAY_MISSIONS][REPLAY_SEGMENTS];                       // 

        coreUint16 aiShiftGoodMission [REPLAY_MISSIONS];                                   // 
        coreUint16 aaiShiftGoodSegment[REPLAY_MISSIONS][REPLAY_SEGMENTS];                  // 
        coreUint16 aiShiftBadMission  [REPLAY_MISSIONS];                                   // 
        coreUint16 aaiShiftBadSegment [REPLAY_MISSIONS][REPLAY_SEGMENTS];                  // 

        coreUint64 iChecksum;                                                              // 
    };

    // 
    struct sSnapshot final
    {
        coreUint16 iIdentifier;   // 
        coreUint32 iFrame;        // 
    };

    // 
    struct sPacket final
    {
        coreUint32 iData;    // 
    };
    struct sPacketRaw final
    {
        coreUint32 iFrame;   // 
        coreUint8  iType;    // 
        coreUint8  iValue;   // 
    };

    // 
    struct sInfo final
    {
        coreString sPath;     // 
        sHeader    oHeader;   // 
    };


private:
    sHeader m_Header;                                 // 

    coreList<sSnapshot> m_aSnapshot;                  // 
    coreList<sPacket>   m_aaPacket[REPLAY_PLAYERS];   // 

    sGameInput m_aInput[REPLAY_PLAYERS];              // 

    coreUint32 m_iCurFrame;                           // 
    coreUint32 m_aiCurPacket[REPLAY_PLAYERS];         // 

    eReplayStatus m_iStatus;                          // 


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
    void ApplySnapshot(const coreUint16 iIdentifier);

    // 
    void Update();

    // 
    coreBool LoadFile(const coreChar* pcPath, const coreBool bOnlyHeader);
    void     SaveFile(const coreChar* pcName);
    void     Clear();

    // 
    inline const sHeader&       GetHeader()const {return m_Header;}
    inline const eReplayStatus& GetStatus()const {return m_iStatus;}

    // 
    static void LoadInfoList(coreList<sInfo>* OUTPUT paInfoList);


private:
    // 
    inline coreBool __CanStartRecording()const {return !m_Header.iEndTimestamp &&  m_aaPacket[0].empty();}
    inline coreBool __CanStartPlayback ()const {return  m_Header.iEndTimestamp && !m_aaPacket[0].empty();}

    // 
    coreBool __SetBodyData(const coreByte*   pData,  const coreUint32   iSize);
    coreBool __GetBodyData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize)const;

    // 
    static sPacket    __Pack  (const sPacketRaw& oPacket);
    static sPacketRaw __Unpack(const sPacket&    oPacket);

    // 
    static void __CheckHeader(sHeader* OUTPUT pHeader);

    // 
    static coreUint64 __GenerateChecksum(const sHeader& oHeader);
};


#endif // _P1_GUARD_REPLAY_H_