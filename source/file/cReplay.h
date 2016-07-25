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


// ****************************************************************
// 
#define REPLAY_STREAMS      (PLAYERS)   // 

#define REPLAY_FILE_MAGIC   (coreUint32('P1RP'))   // 
#define REPLAY_FILE_VERSION (0x00000001u)          // 

#define REPLAY_TYPE_MOVE    (0u)   // 
#define REPLAY_TYPE_PRESS   (1u)   // 
#define REPLAY_TYPE_RELEASE (2u)   // 


// ****************************************************************
// 
class cReplay final
{
private:
    // 
    struct sFileData final
    {
        coreUint32 iMagic;
        coreUint32 iVersion;
        coreChar   acName[64]; 
    };

    // 
    struct sBaseData final
    {
        coreUint32 iReplayHash;
        coreUint32 iExecutableHash;
        coreChar   acDateTime[16];
        coreUint8  iStreamNum; 
    };

    // 
    struct sKeyFrame final
    {
        coreUint32 iFrame;
        coreUint16 iIdentifier; 
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


private:
    sBaseData m_BaseData;                                         // 

    std::vector<sKeyFrame>     m_aKeyFrame;                       // 
    std::vector<sStreamPacket> m_aStreamPacket[REPLAY_STREAMS];   // 

    cInput m_aInput[REPLAY_STREAMS];                              // 


public:
    cReplay()noexcept;

    DISABLE_COPY(cReplay)

    // 
    void StartRecording();
    void StartPlayback();

    // 
    void Update();

    // 
    void LoadFile(const coreChar* pcName);
    void SaveFile(const coreChar* pcName)const;

    // 
    void SetData(const coreByte*   pInput,   const coreUint32   iSize);
    void GetData(coreByte** OUTPUT ppOutput, coreUint32* OUTPUT piSize)const;
};


#endif // _P1_GUARD_REPLAY_H_