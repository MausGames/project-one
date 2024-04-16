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

// TODO 5: replay-player with play, pause, (speed up slow down), time-bar dragging, stage and boss markers auf bar, seitliche liste wie bei speedrun
// TODO 1: snapshot data: both player positions and state (roll, feel, force, interrupt, health, dead/repair (stats), continues, scores, shoot (timer), input), bullets, (chroma (bullets)), background [RP]
// TODO 3: async loading and saving
// TODO 3: hard stage skip, hard enemy kill, hard boss health
// TODO 3: kompression könnte noch verstärkt werden wenn daten von ABABAB auf AAABBB geändert werden vorm komprimieren (geht einfach mit version bump)
// TODO 3: add move-step to keyframe (fallback: go to the marked packet and iterate rev until finding the first move packet)


// ****************************************************************
// 
#define REPLAY_FILE_FOLDER    "replays/"               // 
#define REPLAY_FILE_EXTENSION "p1rp"                   // 
#define REPLAY_FILE_MAGIC     (UINT_LITERAL("P1RP"))   // 
#define REPLAY_FILE_VERSION   (0x00000001u)            // 

#define REPLAY_NAME_LENGTH    (128u)                   // 
#define REPLAY_NAME_INPUT     (20u)                    // 
#define REPLAY_PLAYERS        (PLAYERS)                // 
#define REPLAY_MISSIONS       (MISSIONS)               // 
#define REPLAY_SEGMENTS       (SEGMENTS)               // 
#define REPLAY_RUNS           (CONTINUES)              // 
#define REPLAY_EQUIP_WEAPONS  (EQUIP_WEAPONS)          // 
#define REPLAY_EQUIP_SUPPORTS (EQUIP_SUPPORTS)         // 
#define REPLAY_BITS_FRAME     (10u)                    // 
#define REPLAY_BITS_TYPE      (2u)                     // 
#define REPLAY_BITS_VALUE     (4u)                     // 

STATIC_ASSERT(REPLAY_BITS_FRAME + REPLAY_BITS_TYPE + REPLAY_BITS_VALUE == 16u)

#define __REPLAY_SNAPSHOT_TYPE(x)            (BITVALUE(2u, 13u, (x)))
#define __REPLAY_SNAPSHOT_RUN(x)             (BITVALUE(2u, 11u, (x)))
#define __REPLAY_SNAPSHOT_MISSION(x)         (BITVALUE(4u,  7u, (x)))
#define __REPLAY_SNAPSHOT_SEGMENT(x)         (BITVALUE(3u,  4u, (x)))
#define __REPLAY_SNAPSHOT_INDEX(x)           (BITVALUE(4u,  0u, (x)))

#define REPLAY_SNAPSHOT_MISSION_START(r,m)   (__REPLAY_SNAPSHOT_TYPE(0u) | __REPLAY_SNAPSHOT_RUN(r) | __REPLAY_SNAPSHOT_MISSION(m) | __REPLAY_SNAPSHOT_SEGMENT(0u) | __REPLAY_SNAPSHOT_INDEX(0u))
#define REPLAY_SNAPSHOT_MISSION_END(r,m)     (__REPLAY_SNAPSHOT_TYPE(0u) | __REPLAY_SNAPSHOT_RUN(r) | __REPLAY_SNAPSHOT_MISSION(m) | __REPLAY_SNAPSHOT_SEGMENT(0u) | __REPLAY_SNAPSHOT_INDEX(1u))
#define REPLAY_SNAPSHOT_SEGMENT_START(r,m,s) (__REPLAY_SNAPSHOT_TYPE(0u) | __REPLAY_SNAPSHOT_RUN(r) | __REPLAY_SNAPSHOT_MISSION(m) | __REPLAY_SNAPSHOT_SEGMENT(s)  | __REPLAY_SNAPSHOT_INDEX(2u))
#define REPLAY_SNAPSHOT_SEGMENT_END(r,m,s)   (__REPLAY_SNAPSHOT_TYPE(0u) | __REPLAY_SNAPSHOT_RUN(r) | __REPLAY_SNAPSHOT_MISSION(m) | __REPLAY_SNAPSHOT_SEGMENT(s)  | __REPLAY_SNAPSHOT_INDEX(3u))
#define REPLAY_SNAPSHOT_BOSS(r,m,s,x)        (__REPLAY_SNAPSHOT_TYPE(1u) | __REPLAY_SNAPSHOT_RUN(r) | __REPLAY_SNAPSHOT_MISSION(m) | __REPLAY_SNAPSHOT_SEGMENT(s)  | __REPLAY_SNAPSHOT_INDEX(x))
#define REPLAY_SNAPSHOT_BOSS_DEFAULT(x)      (REPLAY_SNAPSHOT_BOSS(g_pGame->GetContinuesCur(), g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex(), (x)))
#define REPLAY_SNAPSHOT_PROJECTONE(r,x)      (__REPLAY_SNAPSHOT_TYPE(2u) | __REPLAY_SNAPSHOT_RUN(r) | BITVALUE(11u, 0u, (x)))

STATIC_ASSERT((REPLAY_RUNS + 1u <= BIT(2u)) && (REPLAY_MISSIONS <= BIT(4u)) && (REPLAY_SEGMENTS <= BIT(3u)))

#define REPLAY_WRAP_PROGRESS_FIRSTPLAY   ((g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK) ? g_pReplay->GetHeader().bSaveFirstPlay : g_pSave->GetHeader().oProgress.bFirstPlay)
#define REPLAY_WRAP_PROGRESS_FRAGMENT    ((g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK) ? g_pReplay->GetHeader().aiSaveFragment : g_pSave->GetHeader().oProgress.aiFragment)
#define REPLAY_WRAP_PROGRESS_STATE       ((g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK) ? g_pReplay->GetHeader().aiSaveState    : g_pSave->GetHeader().oProgress.aiState)

#define REPLAY_WRAP_CONFIG_CONTROL_MODE  ((g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK) ? g_pReplay->GetHeader().aiConfigControlMode : g_CurConfig.Input   .aiControlMode)
#define REPLAY_WRAP_CONFIG_MIRROR_MODE   ((g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK) ? g_pReplay->GetHeader().iConfigMirrorMode   : g_CurConfig.Game    .iMirrorMode)
#define REPLAY_WRAP_CONFIG_HIT_STOP      ((g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK) ? g_pReplay->GetHeader().iConfigHitStop      : g_CurConfig.Graphics.iHitStop)
#define REPLAY_WRAP_CONFIG_ROTATION_TURN ((g_pReplay->GetMode() == REPLAY_MODE_PLAYBACK) ? g_pReplay->GetHeader().iConfigRotationTurn : g_CurConfig.Legacy  .iRotationTurn)

#if defined(_CORE_SWITCH_)
    #define REPLAY_SLOTSYSTEM (1u)
#endif
#define REPLAY_SLOTS (20u)

enum eReplayType : coreUint8
{
    REPLAY_TYPE_MOVE    = 0u,   // 
    REPLAY_TYPE_PRESS   = 1u,   // 
    REPLAY_TYPE_RELEASE = 2u,   // 
    REPLAY_TYPE_MISC    = 3u    // 
};

enum eReplayMode : coreUint8
{
    REPLAY_MODE_DISABLED  = 0u,   // 
    REPLAY_MODE_RECORDING = 1u,   // 
    REPLAY_MODE_PLAYBACK  = 2u    // 
};

enum eReplayChange : coreUint8
{
    REPLAY_CHANGE_PROGRESS_FRAGMENT_0   = 0u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_1   = 1u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_2   = 2u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_3   = 3u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_4   = 4u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_5   = 5u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_6   = 6u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_7   = 7u,     // 
    REPLAY_CHANGE_PROGRESS_FRAGMENT_8   = 8u,     // 
    REPLAY_CHANGE_CONFIG_CONTROL_MODE_1 = 100u,   // 
    REPLAY_CHANGE_CONFIG_CONTROL_MODE_2 = 101u,   // 
    REPLAY_CHANGE_CONFIG_MIRROR_MODE    = 102u,   // 
    REPLAY_CHANGE_CONFIG_HIT_STOP       = 103u    // 
};

STATIC_ASSERT(REPLAY_CHANGE_PROGRESS_FRAGMENT_8 - REPLAY_CHANGE_PROGRESS_FRAGMENT_0 == FRAGMENTS - 1u)


// ****************************************************************
// 
class cReplay final
{
public:
    // 
    struct sHeader final
    {
        coreUint32  iMagic;                                                                              // 
        coreUint32  iVersion;                                                                            // 

        coreChar    acName[REPLAY_NAME_LENGTH];                                                          // 
        coreUint64  iViewTimestamp;                                                                      // 
        coreUint64  iStartTimestamp;                                                                     // 
        coreUint64  iEndTimestamp;                                                                       // 

        coreUint32  iFrameCount;                                                                         // 
        coreUint32  iSnapshotCount;                                                                      // 
        coreUint32  iChangeCount;                                                                        // 
        coreUint32  aiPacketCount[REPLAY_PLAYERS];                                                       // 

        coreVector2 avPackVector[9];                                                                    // 

        coreUint8   iBitsFrame;                                                                          // 
        coreUint8   iBitsType;                                                                           // 
        coreUint8   iBitsValue;                                                                          // 

        coreUint8   aiPadding1[29];                                                                      // 

        coreInt32   aiMissionList[REPLAY_MISSIONS];                                                      // 
        coreUint8   iMissionIndexStart;                                                                  // 
        coreUint8   iMissionIndexEnd;                                                                    // 
        coreUint8   iMissionTakeFrom;                                                                    // 
        coreUint8   iMissionTakeTo;                                                                      // 

        coreUint8   iNumPlayers;                                                                         // 
        coreUint8   iNumMissions;                                                                        // 
        coreUint8   iNumSegments;                                                                        // 
        coreUint8   iNumRuns;                                                                            // 

        coreUint8   aiPadding2[52];                                                                      // 

        coreUint64  aiEnvSeed             [REPLAY_MISSIONS];                                             // 
        coreFloat   afEnvFlyOffsetMission [REPLAY_MISSIONS];                                             // 
        coreFloat   aafEnvFlyOffsetSegment[REPLAY_MISSIONS][REPLAY_SEGMENTS];                            // 

        coreBool    bSaveFirstPlay;                                                                      // 
        coreUint8   aiSaveAdvance [REPLAY_MISSIONS];                                                     // 
        coreUint8   aiSaveFragment[REPLAY_MISSIONS];                                                     // (bitfield) 
        coreUint8   aaSaveiBadge  [REPLAY_MISSIONS][REPLAY_SEGMENTS];                                    // (bitfield) 
        coreUint8   aiSaveHelper  [REPLAY_MISSIONS];                                                     // (bitfield) 
        coreUint64  aiSaveTrophy  [2];                                                                   // (bitfield) 
        coreUint64  aiSaveUnlock  [2];                                                                   // (bitfield) 
        coreUint64  aiSaveState   [1];                                                                   // (bitfield) 

        coreUint8   iOptionKind;                                                                         // 
        coreUint8   iOptionType;                                                                         // 
        coreUint8   iOptionMode;                                                                         // 
        coreUint8   iOptionDifficulty;                                                                   // 
        coreUint8   iOptionFlags;                                                                        // 
        coreUint8   aiOptionShield  [REPLAY_PLAYERS];                                                    // 
        coreUint8   aaiOptionWeapon [REPLAY_PLAYERS][REPLAY_EQUIP_WEAPONS];                              // 
        coreUint8   aaiOptionSupport[REPLAY_PLAYERS][REPLAY_EQUIP_SUPPORTS];                             // 

        coreChar    acConfigLanguage   [2];                                                              // 
        coreUint8   aiConfigInputType  [REPLAY_PLAYERS];                                                 // 
        coreUint8   aiConfigFireMode   [REPLAY_PLAYERS];                                                 // 
        coreUint8   aiConfigControlMode[REPLAY_PLAYERS];                                                 // 
        coreUint8   iConfigGameDirection;                                                                // 
        coreUint8   iConfigGameSpeed;                                                                    // 
        coreUint8   iConfigMirrorMode;                                                                   // 
        coreUint8   iConfigHudDirection;                                                                 // 
        coreUint8   iConfigHudType;                                                                      // 
        coreUint8   iConfigBackRotation;                                                                 // 
        coreUint8   iConfigBackSpeed;                                                                    // 
        coreUint16  iConfigUpdateFreq;                                                                   // 
        coreUint16  iConfigVersion;                                                                      // 
        coreUint8   iConfigPureMode;                                                                     // 

        coreUint8   iConfigHitStop;                                                                      // 
        coreUint8   iConfigRotationTurn;                                                                 // 
        coreUint8   iConfigPriorityMove;                                                                 // 

        coreUint8   iSystemOs;                                                                           // 
        coreUint8   iSystemCpu;                                                                          // 
        coreUint8   iSystemGpu;                                                                          // 

        coreUint8   iRaise;                                                                              // 

        coreUint8   aiPadding3[42];                                                                      // 

        coreUint32  aiScoreTotal    [REPLAY_PLAYERS];                                                    // 
        coreUint32  aaiScoreMission [REPLAY_PLAYERS][REPLAY_MISSIONS];                                   // 
        coreUint32  aaaiScoreSegment[REPLAY_PLAYERS][REPLAY_MISSIONS][REPLAY_SEGMENTS];                  // 

        coreUint32  iTimeTotal;                                                                          // 
        coreUint32  aiTimeMission [REPLAY_MISSIONS];                                                     // 
        coreUint32  aaiTimeSegment[REPLAY_MISSIONS][REPLAY_SEGMENTS];                                    // 

        coreUint32  aaaiMaxSeriesSegment[REPLAY_PLAYERS][REPLAY_MISSIONS][REPLAY_SEGMENTS];              // 

        coreUint16  aiShiftGoodMission [REPLAY_MISSIONS];                                                // 
        coreUint16  aaiShiftGoodSegment[REPLAY_MISSIONS][REPLAY_SEGMENTS];                               // 
        coreUint16  aiShiftBadMission  [REPLAY_MISSIONS];                                                // 
        coreUint16  aaiShiftBadSegment [REPLAY_MISSIONS][REPLAY_SEGMENTS];                               // 

        coreUint32  aaiRunTotal       [REPLAY_PLAYERS][REPLAY_RUNS];                                     // 
        coreUint32  aaaiRunMission    [REPLAY_PLAYERS][REPLAY_RUNS][REPLAY_MISSIONS];                    // 
        coreUint32  aaaaiRunSegment   [REPLAY_PLAYERS][REPLAY_RUNS][REPLAY_MISSIONS][REPLAY_SEGMENTS];   // 
        coreUint8   aaiRunMissionIndex[REPLAY_PLAYERS][REPLAY_RUNS];                                     // 
        coreUint8   aaiRunSegmentIndex[REPLAY_PLAYERS][REPLAY_RUNS];                                     // 

        coreUint64  aiDataDamageGiven   [REPLAY_PLAYERS];                                                // 
        coreUint32  aiDataDamageTaken   [REPLAY_PLAYERS];                                                // 
        coreUint16  aiDataContinuesUsed [REPLAY_PLAYERS];                                                // 
        coreUint16  aiDataRepairsUsed   [REPLAY_PLAYERS];                                                // 
        coreUint16  aiDataShiftGoodAdded[REPLAY_PLAYERS];                                                // 
        coreUint16  aiDataShiftBadAdded [REPLAY_PLAYERS];                                                // 
        coreUint32  aiDataTurnsMade     [REPLAY_PLAYERS];                                                // 
        coreUint64  aiDataMovesMade     [REPLAY_PLAYERS];                                                // 
        coreUint64  aiDataBulletsShot   [REPLAY_PLAYERS];                                                // 

        coreUint8   iMedalArcade;                                                                        // 
        coreUint8   aiMedalMission [REPLAY_MISSIONS];                                                    // 
        coreUint8   aaiMedalSegment[REPLAY_MISSIONS][REPLAY_SEGMENTS];                                   // 
        coreUint8   aiFragment     [REPLAY_MISSIONS];                                                    // (bitfield) 
        coreUint8   aaiBadge       [REPLAY_MISSIONS][REPLAY_SEGMENTS];                                   // (bitfield) 
        coreUint8   aiHelper       [REPLAY_MISSIONS];                                                    // (bitfield) 

        coreUint8   aiPadding4[47];                                                                      // 

        coreUint32  iBodySize;                                                                           // 
        coreUint64  iChecksum;                                                                           // 
    };

    // 
    struct sSnapshot final
    {
        coreUint32  iFrame;                            // 
        coreUint16  iIdentifier;                       // 

        coreUint16  iMarkChange;                       // 
        coreUint32  aiMarkPacket[REPLAY_PLAYERS];      // 

        coreUint16  aiActionPress  [REPLAY_PLAYERS];   // 
        coreUint16  aiActionRelease[REPLAY_PLAYERS];   // 
        coreUint16  aiActionHold   [REPLAY_PLAYERS];   // (also handles initial input on mission intro) 

        coreVector2 avPlayerPos [REPLAY_PLAYERS];      // 
        coreVector2 avPlayerDir [REPLAY_PLAYERS];      // 
        coreFloat   afWeaponTime[REPLAY_PLAYERS];      // 
    };

    // 
    struct sChange final
    {
        coreUint32 iFrame;   // 
        coreUint8  iType;    // 
        coreUint8  iValue;   // 
    };

    // 
    struct sPacket final
    {
        coreUint16 iData;    // 
    };
    struct sPacketRaw final
    {
        coreUint16 iFrame;   // 
        coreUint8  iType;    // 
        coreUint8  iValue;   // 
    };

    // 
    struct sInfo final
    {
        coreString sPath;     // 
        coreUint8  iSlot;     // 
        sHeader    oHeader;   // 
    };


private:
    sHeader m_Header;                                 // 

    coreList<sSnapshot> m_aSnapshot;                  // 
    coreList<sChange>   m_aChange;                    // 
    coreList<sPacket>   m_aaPacket[REPLAY_PLAYERS];   // 

    sGameInput m_aInput[REPLAY_PLAYERS];              // 

    coreUint32 m_iCurFrame;                           // 
    coreUint32 m_iCurSnapshot;                        // 
    coreUint32 m_iCurChange;                          // 
    coreUint32 m_aiCurPacket[REPLAY_PLAYERS];         // 

    coreUint16 m_iEnvState;                           // 

    eReplayMode m_eMode;                              // 
    eSaveStatus m_eStatus;                            // 


public:
    cReplay()noexcept;

    DISABLE_COPY(cReplay)

    // 
    void CreateGame();
    void RecreateGame();

    // 
    void StartRecording();
    void StartPlayback();
    void EndRecording();
    void EndPlayback();
    void Cancel();

    // 
    void ApplySnapshot(const coreUint16 iIdentifier);

    // 
    void ProcessEnvMission();
    void ProcessEnvSegment();

    // 
    void Update();

    // 
    coreBool LoadFile(const coreChar* pcPath, const coreBool bOnlyHeader);
    coreBool SaveFile(const coreUint8 iSlot);
    coreBool LoadData(const coreByte*   pData,  const coreUint32   iSize);
    coreBool SaveData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize);
    void     Clear();

    // 
    coreBool HasData();

    // 
    void SetName(const coreChar* pcName);
    void SetNameDefault();

    // 
    inline void ResetStatus() {m_eStatus = SAVE_STATUS_OK;}

    // 
    inline const sHeader&     GetHeader()const {return m_Header;}
    inline const eReplayMode& GetMode  ()const {return m_eMode;}
    inline const eSaveStatus& GetStatus()const {return m_eStatus;}

    // 
    static void LoadInfoList(coreList<sInfo>* OUTPUT paInfoList);

    // 
    static coreBool RenameReplay(const coreChar* pcPath, const coreChar* pcName);


private:
    // 
    inline coreBool __CanStartRecording()const {return !m_Header.iEndTimestamp &&  m_aaPacket[0].empty();}
    inline coreBool __CanStartPlayback ()const {return  m_Header.iEndTimestamp && !m_aaPacket[0].empty();}

    // 
    coreBool __SetBodyData(const coreByte*   pData,  const coreUint32   iSize);
    coreBool __GetBodyData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize);

    // 
    static sPacket    __Pack  (const sPacketRaw& oPacket);
    static sPacketRaw __Unpack(const sPacket&    oPacket);

    // 
    static void __CheckHeader(sHeader* OUTPUT pHeader);

    // 
    static coreUint64 __GenerateChecksum(const sHeader& oHeader);
};


#endif // _P1_GUARD_REPLAY_H_