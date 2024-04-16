///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cReplay::cReplay()noexcept
: m_Header       {}
, m_aSnapshot    {}
, m_aChange      {}
, m_aaPacket     {}
, m_aInput       {}
, m_iCurFrame    (0u)
, m_iCurSnapshot (0u)
, m_iCurChange   (0u)
, m_aiCurPacket  {}
, m_iEnvState    (0u)
, m_eMode        (REPLAY_MODE_DISABLED)
, m_eStatus      (SAVE_STATUS_OK)
{
    // 
    m_aSnapshot.reserve(0xFFFu / sizeof(sSnapshot));
    m_aChange  .reserve(0xFFu  / sizeof(sChange));
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i) m_aaPacket[i].reserve(0xFFFFu / sizeof(sPacket));
}


// ****************************************************************
// 
void cReplay::CreateGame()
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)
    ASSERT(!STATIC_ISVALID(g_pGame) && this->__CanStartPlayback())

    // 
    InitFramerate(m_Header.iConfigUpdateFreq, m_Header.iConfigGameSpeed);

    // 
    sGameOptions oOptions = {};
    oOptions.iKind        = m_Header.iOptionKind;
    oOptions.iType        = m_Header.iOptionType;
    oOptions.iMode        = m_Header.iOptionMode;
    oOptions.iDifficulty  = m_Header.iOptionDifficulty;
    oOptions.iFlags       = m_Header.iOptionFlags;
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        oOptions.aiShield[i] = m_Header.aiOptionShield[i];
        for(coreUintW j = 0u; j < REPLAY_EQUIP_WEAPONS;  ++j) oOptions.aaiWeapon [i][j] = m_Header.aaiOptionWeapon [i][j];
        for(coreUintW j = 0u; j < REPLAY_EQUIP_SUPPORTS; ++j) oOptions.aaiSupport[i][j] = m_Header.aaiOptionSupport[i][j];
    }

    // 
    STATIC_NEW(g_pGame, oOptions, m_Header.aiMissionList, m_Header.iNumMissions)
}


// ****************************************************************
// 
void cReplay::RecreateGame()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    STATIC_DELETE(g_pGame)

    // 
    this->CreateGame();
}


// ****************************************************************
// 
void cReplay::StartRecording()
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)
    ASSERT(STATIC_ISVALID(g_pGame) && this->__CanStartRecording())

    // 
    this->Clear();

    // 
    m_iCurFrame    = 0u;
    m_iCurSnapshot = 0u;
    m_iCurChange   = 0u;
    m_iEnvState    = 0u;
    m_eMode        = REPLAY_MODE_RECORDING;

    // 
    std::memset(m_aInput,      0, sizeof(m_aInput));
    std::memset(m_aiCurPacket, 0, sizeof(m_aiCurPacket));

    // 
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        m_aInput[i].iMoveStep = 8u;
    }

    // 
    m_Header.iMagic          = REPLAY_FILE_MAGIC;
    m_Header.iVersion        = REPLAY_FILE_VERSION;
    m_Header.iViewTimestamp  = std::time(NULL);
    m_Header.iStartTimestamp = m_Header.iViewTimestamp;
    m_Header.iBitsFrame      = REPLAY_BITS_FRAME;
    m_Header.iBitsType       = REPLAY_BITS_TYPE;
    m_Header.iBitsValue      = REPLAY_BITS_VALUE;
    m_Header.iNumPlayers     = g_pGame->IsMulti() ? REPLAY_PLAYERS : 1u;
    m_Header.iNumMissions    = g_pGame->GetNumMissions();
    m_Header.iNumSegments    = REPLAY_SEGMENTS;

    // 
    for(coreUintW i = 0u; i < 9u; ++i)
    {
        m_Header.avPackVector[i] = UnpackDirection(i);
    }

    // 
    m_Header.bSaveFirstPlay = g_pSave->GetHeader().oProgress.bFirstPlay;
    for(coreUintW i = 0u; i < REPLAY_MISSIONS;                   ++i) m_Header.aiSaveAdvance [i] = g_pSave->GetHeader().oProgress.aiAdvance [i];
    for(coreUintW i = 0u; i < REPLAY_MISSIONS;                   ++i) m_Header.aiSaveFragment[i] = g_pSave->GetHeader().oProgress.aiFragment[i];
    for(coreUintW i = 0u; i < REPLAY_MISSIONS;                   ++i) for(coreUintW j = 0u; j < REPLAY_SEGMENTS; ++j) m_Header.aaSaveiBadge[i][j] = g_pSave->GetHeader().oProgress.aaiBadge[i][j];
    for(coreUintW i = 0u; i < REPLAY_MISSIONS;                   ++i) m_Header.aiSaveHelper  [i] = g_pSave->GetHeader().oProgress.aiHelper  [i];
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Header.aiSaveTrophy); ++i) m_Header.aiSaveTrophy  [i] = g_pSave->GetHeader().oProgress.aiTrophy  [i];
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Header.aiSaveUnlock); ++i) m_Header.aiSaveUnlock  [i] = g_pSave->GetHeader().oProgress.aiUnlock  [i];
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Header.aiSaveState);  ++i) m_Header.aiSaveState   [i] = g_pSave->GetHeader().oProgress.aiState   [i];

    // 
    m_Header.iOptionKind       = g_pGame->GetOptions().iKind;
    m_Header.iOptionType       = g_pGame->GetOptions().iType;
    m_Header.iOptionMode       = g_pGame->GetOptions().iMode;
    m_Header.iOptionDifficulty = g_pGame->GetOptions().iDifficulty;
    m_Header.iOptionFlags      = g_pGame->GetOptions().iFlags;
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        m_Header.aiOptionShield[i] = g_pGame->GetOptions().aiShield[i];
        for(coreUintW j = 0u; j < REPLAY_EQUIP_WEAPONS;  ++j) m_Header.aaiOptionWeapon [i][j] = g_pGame->GetOptions().aaiWeapon [i][j];
        for(coreUintW j = 0u; j < REPLAY_EQUIP_SUPPORTS; ++j) m_Header.aaiOptionSupport[i][j] = g_pGame->GetOptions().aaiSupport[i][j];
    }

    // 
    std::memcpy(m_Header.acConfigLanguage, coreData::StrFilename(Core::Config->GetString(CORE_CONFIG_BASE_LANGUAGE)), 2u);
    for(coreUintW i = 0u; i < SCORE_PLAYERS; ++i)
    {
        m_Header.aiConfigInputType  [i] =  g_CurConfig.Input.aiType       [i];
        m_Header.aiConfigFireMode   [i] =  g_CurConfig.Input.aiFireMode   [i];
        m_Header.aiConfigControlMode[i] =  g_CurConfig.Input.aiControlMode[i];
    }
    m_Header.iConfigGameDirection = g_CurConfig.Game.iGameDirection;
    m_Header.iConfigGameSpeed     = GetCurGameSpeed();
    m_Header.iConfigMirrorMode    = g_CurConfig.Game.iMirrorMode;
    m_Header.iConfigHudDirection  = g_CurConfig.Game.iHudDirection;
    m_Header.iConfigHudType       = g_CurConfig.Game.iHudType;
    m_Header.iConfigBackRotation  = GetCurBackRotation();
    m_Header.iConfigBackSpeed     = GetCurBackSpeed();
    m_Header.iConfigUpdateFreq    = GetCurUpdateFreqReal();
    m_Header.iConfigVersion       = g_pGame->GetVersion();
    m_Header.iConfigPureMode      = g_CurConfig.Game.iPureMode;

    // 
    m_Header.iConfigHitStop       = g_CurConfig.Graphics.iHitStop;
    m_Header.iConfigRotationTurn  = g_CurConfig.Legacy.iRotationTurn;
    m_Header.iConfigPriorityMove  = g_CurConfig.Legacy.iPriorityMove;

    // 
    m_Header.iSystemOs  = GetSystemOsIndex();
    m_Header.iSystemCpu = GetSystemCpuIndex();
    m_Header.iSystemGpu = GetSystemGpuIndex();

    // 
    m_Header.iRaise = g_pGame->GetRaise();

    // 
    m_Header.iMissionIndexStart = g_pGame->GetCurMissionIndex();
    m_Header.iMissionTakeFrom   = g_pGame->GetCurMission()->GetTakeFrom();
    m_Header.iMissionTakeTo     = g_pGame->GetCurMission()->GetTakeTo();
    std::memcpy(m_Header.aiMissionList, g_pGame->GetMissionList(), sizeof(coreInt32) * g_pGame->GetNumMissions());

    Core::Log->Info("Replay recording started");
}


// ****************************************************************
// 
void cReplay::StartPlayback()
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)
    ASSERT(STATIC_ISVALID(g_pGame) && this->__CanStartPlayback())

    // 
    m_iCurFrame    = 0u;
    m_iCurSnapshot = 0u;
    m_iCurChange   = 0u;
    m_iEnvState    = 0u;
    m_eMode        = REPLAY_MODE_PLAYBACK;   // # before mission load

    // 
    std::memset(m_aInput,      0, sizeof(m_aInput));
    std::memset(m_aiCurPacket, 0, sizeof(m_aiCurPacket));

    // 
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        m_aInput[i].iMoveStep = 8u;
    }

    // 
    g_pSave->SetIgnore(true);

    // 
    g_pGame->LoadMissionIndex(m_Header.iMissionIndexStart, m_Header.iMissionTakeFrom, m_Header.iMissionTakeTo);

    // 
    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
    {
        g_pGame->GetPlayer(i)->SetInput(&m_aInput[i]);
    }

    Core::Log->Info("Replay playback started");
}


// ****************************************************************
// 
void cReplay::EndRecording()
{
    ASSERT(m_eMode == REPLAY_MODE_RECORDING)
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_Header.iEndTimestamp  = std::time(NULL);
    m_Header.iFrameCount    = m_iCurFrame;
    m_Header.iSnapshotCount = m_aSnapshot.size();
    m_Header.iChangeCount   = m_aChange  .size();
    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
    {
        m_Header.aiPacketCount[i] = m_aaPacket[i].size();
    }

    // 
    m_Header.iMissionIndexEnd = g_pGame->GetCurMissionIndex();
    m_Header.iNumRuns         = g_pGame->GetPlayer(0u)->GetDataTable()->GetCounterTotal().iContinuesUsed;

    // 
    for(coreUintW k = 0u, ke = m_Header.iNumPlayers; k < ke; ++k)
    {
        const cScoreTable* pTable = g_pGame->GetPlayer(k)->GetScoreTable();

        m_Header.aiScoreTotal[k] = pTable->GetScoreTotal();
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aaiScoreMission[k][j] = pTable->GetScoreMission(j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaaiScoreSegment[k][j][i] = pTable->GetScoreSegment(j, i);
    }

    // 
    {
        const cTimeTable* pTable = g_pGame->GetTimeTable();

        m_Header.iTimeTotal = TABLE_TIME_TO_UINT(pTable->GetTimeTotal());
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiTimeMission[j] = TABLE_TIME_TO_UINT(pTable->GetTimeMission(j));
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaiTimeSegment[j][i] = TABLE_TIME_TO_UINT(pTable->GetTimeSegment(j, i));
    }

    // 
    for(coreUintW k = 0u, ke = m_Header.iNumPlayers; k < ke; ++k)
    {
        const cScoreTable* pTable = g_pGame->GetPlayer(k)->GetScoreTable();

        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaaiMaxSeriesSegment[k][j][i] = pTable->GetMaxSeriesSegment(j, i);
    }

    // 
    {
        const cTimeTable* pTable = g_pGame->GetTimeTable();

        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiShiftGoodMission[j] = pTable->GetShiftGoodMission(j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaiShiftGoodSegment[j][i] = pTable->GetShiftGoodSegment(j, i);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiShiftBadMission [j] = pTable->GetShiftBadMission (j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaiShiftBadSegment [j][i] = pTable->GetShiftBadSegment (j, i);
    }

    // 
    for(coreUintW l = 0u, le = m_Header.iNumPlayers; l < le; ++l)
    {
        const cScoreTable* pTable = g_pGame->GetPlayer(l)->GetScoreTable();

        for(coreUintW k = 0u, ke = m_Header.iNumRuns; k < ke; ++k)
        {
            m_Header.aaiRunTotal       [l][k] = pTable->GetRunTotal       (k);
            for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aaaiRunMission[l][k][j] = pTable->GetRunMission(k, j);
            for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaaaiRunSegment[l][k][j][i] = pTable->GetRunSegment(k, j, i);
            m_Header.aaiRunMissionIndex[l][k] = pTable->GetRunMissionIndex(k);
            m_Header.aaiRunSegmentIndex[l][k] = pTable->GetRunSegmentIndex(k);
        }
    }

    // 
    for(coreUintW k = 0u, ke = m_Header.iNumPlayers; k < ke; ++k)
    {
        const cDataTable* pTable = g_pGame->GetPlayer(k)->GetDataTable();

        m_Header.aiDataDamageGiven   [k] = pTable->GetCounterTotal().iDamageGiven;
        m_Header.aiDataDamageTaken   [k] = pTable->GetCounterTotal().iDamageTaken;
        m_Header.aiDataContinuesUsed [k] = pTable->GetCounterTotal().iContinuesUsed;
        m_Header.aiDataRepairsUsed   [k] = pTable->GetCounterTotal().iRepairsUsed;
        m_Header.aiDataShiftGoodAdded[k] = pTable->GetCounterTotal().iShiftGoodAdded;
        m_Header.aiDataShiftBadAdded [k] = pTable->GetCounterTotal().iShiftBadAdded;
        m_Header.aiDataTurnsMade     [k] = pTable->GetCounterTotal().iTurnsMade;
        m_Header.aiDataMovesMade     [k] = pTable->GetCounterTotal().iMovesMade;
        m_Header.aiDataBulletsShot   [k] = pTable->GetCounterTotal().iBulletsShot;
    }

    // 
    {
        const cDataTable* pTable = g_pGame->GetPlayer(0u)->GetDataTable();

        m_Header.iMedalArcade = pTable->GetMedalArcade();
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiMedalMission[j] = pTable->GetMedalMission(j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaiMedalSegment[j][i] = pTable->GetMedalSegment(j, i);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiFragment    [j] = pTable->GetFragmentAll (j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaiBadge       [j][i] = pTable->GetBadgeAll    (j, i);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiHelper      [j] = pTable->GetHelperAll   (j);
    }

    // 
    m_eMode = REPLAY_MODE_DISABLED;

    Core::Log->Info("Replay recording ended");
}


// ****************************************************************
// 
void cReplay::EndPlayback()
{
    ASSERT(m_eMode == REPLAY_MODE_PLAYBACK)
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    g_pSave->SetIgnore(false);

    // 
    m_eMode = REPLAY_MODE_DISABLED;

    Core::Log->Info("Replay playback ended");
}


// ****************************************************************
// 
void cReplay::Cancel()
{
    // 
    if(m_eMode == REPLAY_MODE_RECORDING)
    {
        this->EndRecording();
    }
    else if(m_eMode == REPLAY_MODE_PLAYBACK)
    {
        this->EndPlayback();
    }

    // 
    this->Clear();
}


// ****************************************************************
// 
void cReplay::ApplySnapshot(const coreUint16 iIdentifier)
{
    if(!STATIC_ISVALID(g_pGame)) return;

    if(m_eMode == REPLAY_MODE_RECORDING)
    {
        // 
        sSnapshot oSnapshot = {};
        oSnapshot.iFrame      = m_iCurFrame;
        oSnapshot.iIdentifier = iIdentifier;

        // 
        oSnapshot.iMarkChange = m_aChange.size();
        for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
        {
            oSnapshot.aiMarkPacket[i] = m_aaPacket[i].size();
        }

        // 
        for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
        {
            const sGameInput* pInput = g_pGame->GetPlayer(i)->GetInput();

            oSnapshot.aiActionPress  [i] = pInput->iActionPress;
            oSnapshot.aiActionRelease[i] = pInput->iActionRelease;
            oSnapshot.aiActionHold   [i] = pInput->iActionHold;
        }

        // 
        for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
        {
            const cPlayer* pPlayer = g_pGame->GetPlayer(i);

            oSnapshot.avPlayerPos [i] = pPlayer->GetPosition ().xy();
            oSnapshot.avPlayerDir [i] = pPlayer->GetDirection().xy();
            oSnapshot.afWeaponTime[i] = pPlayer->GetWeapon(0u)->GetCooldownTime();
        }

        // 
        m_aSnapshot.push_back(oSnapshot);
    }
    else if(m_eMode == REPLAY_MODE_PLAYBACK)
    {
        if(m_iCurSnapshot < m_aSnapshot.size())
        {
            if(m_aSnapshot[m_iCurSnapshot].iIdentifier != iIdentifier)
            {
                coreBool bFound = false;
                for(coreUintW i = 0u, ie = m_aSnapshot.size(); i < ie; ++i)
                {
                    if(m_aSnapshot[i].iIdentifier == iIdentifier)
                    {
                        m_iCurSnapshot = i;
                        bFound = true;
                        break;
                    }
                }
                if(!bFound) return;
            }

            const sSnapshot oSnapshot = m_aSnapshot[m_iCurSnapshot];

            // 
            m_iCurFrame = oSnapshot.iFrame;

            // 
            m_iCurChange = oSnapshot.iMarkChange;
            for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
            {
                m_aiCurPacket[i] = oSnapshot.aiMarkPacket[i];
            }

            // 
            for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
            {
                m_aInput[i].iActionPress   = oSnapshot.aiActionPress  [i];
                m_aInput[i].iActionRelease = oSnapshot.aiActionRelease[i];
                m_aInput[i].iActionHold    = oSnapshot.aiActionHold   [i];
            }

            // 
            for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
            {
                cPlayer* pPlayer = g_pGame->GetPlayer(i);

                pPlayer->SetPosition (coreVector3(oSnapshot.avPlayerPos[i], pPlayer->GetPosition ().z));
                pPlayer->SetDirection(coreVector3(oSnapshot.avPlayerDir[i], pPlayer->GetDirection().z));
                pPlayer->GetWeapon(0u)->SetCooldownTime(oSnapshot.afWeaponTime[i]);
            }

            m_iCurSnapshot += 1u;
        }
    }
}


// ****************************************************************
// 
void cReplay::ProcessEnvMission()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    ASSERT(iMissionIndex < REPLAY_MISSIONS)

    if(!HAS_BIT(m_iEnvState, iMissionIndex))
    {
        ADD_BIT(m_iEnvState, iMissionIndex)

        if(m_eMode == REPLAY_MODE_RECORDING)
        {
            const cOutdoor* pOutdoor = g_pEnvironment->GetBackground()->GetOutdoor();

            // 
            m_Header.aiEnvSeed            [iMissionIndex] = pOutdoor ? pOutdoor->GetSeed() : 0u;
            m_Header.afEnvFlyOffsetMission[iMissionIndex] = g_pEnvironment->GetFlyOffset();
        }
        else if(m_eMode == REPLAY_MODE_PLAYBACK)
        {
            // 
            g_pEnvironment->OverrideFlyOffset(m_Header.afEnvFlyOffsetMission[iMissionIndex]);
        }
    }
}


// ****************************************************************
// 
void cReplay::ProcessEnvSegment()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    const coreUintW iSegmentIndex = g_pGame->GetCurMission()->GetCurSegmentIndex();
    ASSERT(iMissionIndex < REPLAY_MISSIONS)
    ASSERT(iSegmentIndex < REPLAY_SEGMENTS)

    if(m_eMode == REPLAY_MODE_RECORDING)
    {
        // 
        m_Header.aafEnvFlyOffsetSegment[iMissionIndex][iSegmentIndex] = g_pEnvironment->GetFlyOffset();
    }
    else if(m_eMode == REPLAY_MODE_PLAYBACK)
    {
        // 
        g_pEnvironment->OverrideFlyOffset(m_Header.aafEnvFlyOffsetSegment[iMissionIndex][iSegmentIndex]);
    }
}


// ****************************************************************
// 
void cReplay::Update()
{
#if defined(_CORE_DEBUG_)
    if(!TIME) return;
#endif

    if(!STATIC_ISVALID(g_pGame)) return;

    // 
    if(!HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) return;
    ++m_iCurFrame;

    if(m_eMode == REPLAY_MODE_RECORDING)
    {
        const auto nNewPacketFunc = [this](const coreUintW iIndex, const coreUint8 iType, const coreUint8 iValue)
        {
            // 
            sPacketRaw oPacketRaw = {};
            oPacketRaw.iFrame = m_iCurFrame & BITLINE(REPLAY_BITS_FRAME);
            oPacketRaw.iType  = iType;
            oPacketRaw.iValue = iValue;

            // 
            const sPacket oNewPacket = cReplay::__Pack(oPacketRaw);

            // 
            m_aaPacket[iIndex].push_back(oNewPacket);
        };

        for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
        {
            const sGameInput* pNewInput = g_pGame->GetPlayer(i)->GetInput();

            // 
            if(pNewInput->iMoveStep != m_aInput[i].iMoveStep)
            {
                ASSERT(pNewInput->vMove == MapToAxis(UnpackDirection(pNewInput->iMoveStep), g_pPostProcessing->GetDirectionGame()))
                nNewPacketFunc(i, REPLAY_TYPE_MOVE, pNewInput->iMoveStep);
            }

            // 
            if(pNewInput->iActionPress)
            {
                for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
                {
                    if(HAS_BIT(pNewInput->iActionPress, j)) nNewPacketFunc(i, REPLAY_TYPE_PRESS, j);
                }
            }
            if(pNewInput->iActionRelease)
            {
                for(coreUintW j = 0u; j < INPUT_KEYS_ACTION; ++j)
                {
                    if(HAS_BIT(pNewInput->iActionRelease, j)) nNewPacketFunc(i, REPLAY_TYPE_RELEASE, j);
                }
            }

            // 
            if(!(m_iCurFrame & BITLINE(REPLAY_BITS_FRAME)))
            {
                nNewPacketFunc(i, REPLAY_TYPE_MISC, 0u);   // needs to be the last package of a frame
            }

            // 
            m_aInput[i] = (*pNewInput);
        }

        const auto nNewChangeFunc = [this](const coreUint8 iType, coreUint8* OUTPUT piOldValue, const coreUint8 iNewValue)
        {
            if((*piOldValue) != iNewValue)
            {
                (*piOldValue) = iNewValue;

                // 
                sChange oChange = {};
                oChange.iFrame = m_iCurFrame;
                oChange.iType  = iType;
                oChange.iValue = iNewValue;

                // 
                m_aChange.push_back(oChange);
            }
        };

        // 
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_0,   &m_Header.aiSaveFragment[0],      g_pSave->GetHeader().oProgress.aiFragment[0]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_1,   &m_Header.aiSaveFragment[1],      g_pSave->GetHeader().oProgress.aiFragment[1]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_2,   &m_Header.aiSaveFragment[2],      g_pSave->GetHeader().oProgress.aiFragment[2]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_3,   &m_Header.aiSaveFragment[3],      g_pSave->GetHeader().oProgress.aiFragment[3]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_4,   &m_Header.aiSaveFragment[4],      g_pSave->GetHeader().oProgress.aiFragment[4]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_5,   &m_Header.aiSaveFragment[5],      g_pSave->GetHeader().oProgress.aiFragment[5]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_6,   &m_Header.aiSaveFragment[6],      g_pSave->GetHeader().oProgress.aiFragment[6]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_7,   &m_Header.aiSaveFragment[7],      g_pSave->GetHeader().oProgress.aiFragment[7]);
        nNewChangeFunc(REPLAY_CHANGE_PROGRESS_FRAGMENT_8,   &m_Header.aiSaveFragment[8],      g_pSave->GetHeader().oProgress.aiFragment[8]);
        nNewChangeFunc(REPLAY_CHANGE_CONFIG_CONTROL_MODE_1, &m_Header.aiConfigControlMode[0], g_CurConfig.Input   .aiControlMode[0]);
        nNewChangeFunc(REPLAY_CHANGE_CONFIG_CONTROL_MODE_2, &m_Header.aiConfigControlMode[1], g_CurConfig.Input   .aiControlMode[1]);
        nNewChangeFunc(REPLAY_CHANGE_CONFIG_MIRROR_MODE,    &m_Header.iConfigMirrorMode,      g_CurConfig.Game    .iMirrorMode);
        nNewChangeFunc(REPLAY_CHANGE_CONFIG_HIT_STOP,       &m_Header.iConfigHitStop,         g_CurConfig.Graphics.iHitStop);
    }
    else if(m_eMode == REPLAY_MODE_PLAYBACK)
    {
        for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
        {
            sGameInput& oCurInput = m_aInput[i];

            // 
            oCurInput.iActionPress   = 0u;
            oCurInput.iActionRelease = 0u;

            for(coreUintW j = m_aiCurPacket[i], je = m_aaPacket[i].size(); j < je; ++j)   // multiple package on the same frame
            {
                const sPacketRaw oPacketRaw = cReplay::__Unpack(m_aaPacket[i][j]);
                if(oPacketRaw.iFrame == (m_iCurFrame & BITLINE(REPLAY_BITS_FRAME)))
                {
                    // 
                    switch(oPacketRaw.iType)
                    {
                    case REPLAY_TYPE_MOVE:
                        oCurInput.iMoveStep = oPacketRaw.iValue;
                        break;

                    case REPLAY_TYPE_PRESS:
                        oCurInput.iActionPress |= BIT(oPacketRaw.iValue);
                        oCurInput.iActionHold  |= BIT(oPacketRaw.iValue);
                        break;

                    case REPLAY_TYPE_RELEASE:
                        oCurInput.iActionRelease |=  BIT(oPacketRaw.iValue);
                        oCurInput.iActionHold    &= ~BIT(oPacketRaw.iValue);
                        break;

                    case REPLAY_TYPE_MISC:
                        break;

                    default:
                        WARN_IF(true) {}
                        break;
                    }

                    // always skip current package (especially at the end)
                    m_aiCurPacket[i] = j + 1u;

                    // 
                    if((oPacketRaw.iType == REPLAY_TYPE_MISC) && (oPacketRaw.iValue == 0u))
                        break;
                }
                else
                {
                    // 
                    m_aiCurPacket[i] = j;
                    break;
                }
            }

            // 
            oCurInput.vMove = MapToAxis(UnpackDirection(oCurInput.iMoveStep), g_pPostProcessing->GetDirectionGame());
        }

        for(coreUintW j = m_iCurChange, je = m_aChange.size(); j < je; ++j)   // multiple package on the same frame
        {
            const sChange oChange = m_aChange[j];
            if(oChange.iFrame == m_iCurFrame)
            {
                // 
                switch(oChange.iType)
                {
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_0:   m_Header.aiSaveFragment[0]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_1:   m_Header.aiSaveFragment[1]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_2:   m_Header.aiSaveFragment[2]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_3:   m_Header.aiSaveFragment[3]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_4:   m_Header.aiSaveFragment[4]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_5:   m_Header.aiSaveFragment[5]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_6:   m_Header.aiSaveFragment[6]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_7:   m_Header.aiSaveFragment[7]      = oChange.iValue; break;
                case REPLAY_CHANGE_PROGRESS_FRAGMENT_8:   m_Header.aiSaveFragment[8]      = oChange.iValue; break;
                case REPLAY_CHANGE_CONFIG_CONTROL_MODE_1: m_Header.aiConfigControlMode[0] = oChange.iValue; break;
                case REPLAY_CHANGE_CONFIG_CONTROL_MODE_2: m_Header.aiConfigControlMode[1] = oChange.iValue; break;
                case REPLAY_CHANGE_CONFIG_MIRROR_MODE:    m_Header.iConfigMirrorMode      = oChange.iValue; break;
                case REPLAY_CHANGE_CONFIG_HIT_STOP:       m_Header.iConfigHitStop         = oChange.iValue; break;

                default:
                    WARN_IF(true) {}
                    break;
                }

                // 
                m_iCurChange = j + 1u;
            }
            else
            {
                // 
                m_iCurChange = j;
                break;
            }
        }
    }
}


// ****************************************************************
// 
coreBool cReplay::LoadFile(const coreChar* pcPath, const coreBool bOnlyHeader)
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)

    // 
    this->Clear();

    // 
    if(!coreData::FileExists(pcPath))
    {
        Core::Log->Warning("Replay (%s) does not exists!", pcPath);
        return false;
    }

    // 
    coreArchive oArchive(pcPath);
    coreFile* pHeaderFile = oArchive.GetFile("header");
    coreFile* pBodyFile   = oArchive.GetFile("body");
    WARN_IF(!pHeaderFile || !pBodyFile)
    {
        Core::Log->Warning("Replay (%s) could not be loaded!", pcPath);
        return false;
    }

    // 
    if(coreData::Decompress(pHeaderFile->GetData(), pHeaderFile->GetSize(), r_cast<coreByte*>(&m_Header), sizeof(sHeader)) != CORE_OK)
    {
        std::memcpy(&m_Header, pHeaderFile->GetData(), MIN(pHeaderFile->GetSize(), sizeof(sHeader)));
    }

    // 
    WARN_IF((m_Header.iMagic    != REPLAY_FILE_MAGIC)   ||
            (m_Header.iVersion  >  REPLAY_FILE_VERSION) ||
            (m_Header.iChecksum != cReplay::__GenerateChecksum(m_Header)))
    {
        Core::Log->Warning("Replay (%s) is not a valid replay-file!", pcPath);
        return false;
    }

    // 
    cReplay::__CheckHeader(&m_Header);

    if(!bOnlyHeader)
    {
        // 
        pBodyFile->Decompress(m_Header.iBodySize);

        // 
        WARN_IF((m_Header.iBodySize != pBodyFile->GetSize()) || !this->__SetBodyData(pBodyFile->GetData(), pBodyFile->GetSize()))
        {
            Core::Log->Warning("Replay (%s) is corrupt!", pcPath);
            return false;
        }
    }

    Core::Log->Info("Replay (%s) loaded", pcPath);
    return true;
}


// ****************************************************************
// 
coreBool cReplay::SaveFile(const coreUint8 iSlot)
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)

    // 
    coreByte*  pBodyData = NULL;
    coreUint32 iBodySize = 0;
    WARN_IF(!this->__GetBodyData(&pBodyData, &iBodySize)) return true;

    // 
    if(!m_Header.iViewTimestamp)
    {
        m_Header.iViewTimestamp = std::time(NULL);
    }

    // 
    m_Header.iBodySize = iBodySize;
    m_Header.iChecksum = cReplay::__GenerateChecksum(m_Header);

    // 
    coreByte* pHeaderData = new coreByte[sizeof(sHeader)];
    std::memcpy(pHeaderData, &m_Header, sizeof(sHeader));

    // 
    coreArchive oArchive;
    oArchive.CreateFile("header", pHeaderData, sizeof(sHeader))->Compress();
    oArchive.CreateFile("body",   pBodyData,   iBodySize)      ->Compress();

    // 
    #if defined(REPLAY_SLOTSYSTEM)
        const coreChar* pcPath = coreData::UserFolderPrivate(PRINT(REPLAY_FILE_FOLDER "replay_%04u." REPLAY_FILE_EXTENSION, iSlot));
    #else
        const coreChar* pcPath = coreData::UserFolderPrivate(coreData::DateTimePrint(REPLAY_FILE_FOLDER "replay_%Y%m%d_%H%M%S." REPLAY_FILE_EXTENSION, TIMEMAP_LOCAL(m_Header.iViewTimestamp)));
    #endif
    WARN_IF(oArchive.Save(pcPath) != CORE_OK)
    {
        coreUint64 iAvailable;
        coreData::SystemSpace(&iAvailable, NULL);

             if(iAvailable < 1u * 1024u * 1024u)                           m_eStatus = SAVE_STATUS_ERROR_SPACE;
        else if(!coreData::FolderWritable(coreData::StrDirectory(pcPath))) m_eStatus = SAVE_STATUS_ERROR_ACCESS;
        else                                                               m_eStatus = SAVE_STATUS_ERROR_UNKNOWN;

        Core::Log->Warning("Replay (%s) could not be saved (status %u)", pcPath, coreUint32(m_eStatus));
        return false;
    }

    // 
    coreFile::FlushFilesystem();

    Core::Log->Info("Replay (%s) saved", oArchive.GetPath());
    return true;
}


// ****************************************************************
// 
coreBool cReplay::LoadData(const coreByte* pData, const coreUint32 iSize)
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)
    ASSERT(pData && iSize)

    // 
    this->Clear();

    
    coreByte*  pFullData = NULL;
    coreUint32 iFullSize = 0u;
    
    coreDataScope<coreByte> oScope = NULL;

    // 
    if(coreData::Decompress(pData, iSize, &pFullData, &iFullSize, 10u * 1024u * 1024u) != CORE_OK)
    {
        pFullData = c_cast<coreByte*>(pData);
        iFullSize = iSize;
    }
    else
    {
        oScope = pFullData;
    }
    
    const coreByte*  pBodyData = pFullData + sizeof(sHeader);
    const coreUint32 iBodySize = iFullSize - sizeof(sHeader);
    
    std::memcpy(&m_Header, pFullData, sizeof(sHeader));

    // 
    WARN_IF((m_Header.iMagic    != REPLAY_FILE_MAGIC)   ||
            (m_Header.iVersion  >  REPLAY_FILE_VERSION) ||
            (m_Header.iChecksum != cReplay::__GenerateChecksum(m_Header)))
    {
        Core::Log->Warning("Replay data is not a valid replay-file!");
        return false;
    }

    // 
    cReplay::__CheckHeader(&m_Header);

    // 
    WARN_IF((m_Header.iBodySize != iBodySize) || !this->__SetBodyData(pBodyData, iBodySize))
    {
        Core::Log->Warning("Replay data is corrupt!");
        return false;
    }

    // 
    m_Header.iViewTimestamp = 0u;

    Core::Log->Info("Replay data loaded");
    return true;
}


// ****************************************************************
// 
coreBool cReplay::SaveData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize)
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)
    ASSERT(ppData && piSize)

    // 
    coreByte*  pBodyData = NULL;
    coreUint32 iBodySize = 0;
    WARN_IF(!this->__GetBodyData(&pBodyData, &iBodySize)) return true;

    // 
    m_Header.iBodySize = iBodySize;
    m_Header.iChecksum = cReplay::__GenerateChecksum(m_Header);

    // 
    const coreUint32 iFullSize = sizeof(sHeader) + iBodySize;
    coreByte*        pFullData = new coreByte[iFullSize];

    // 
    std::memcpy(pFullData,                   &m_Header, sizeof(sHeader));
    std::memcpy(pFullData + sizeof(sHeader), pBodyData, iBodySize);

    // 
    if(coreData::Compress(pFullData, iFullSize, ppData, piSize) != CORE_OK)
    {
        (*ppData) = pFullData;
        (*piSize) = iFullSize;
    }
    else
    {
        SAFE_DELETE_ARRAY(pFullData)
    }

    SAFE_DELETE_ARRAY(pBodyData)

    Core::Log->Info("Replay data saved");
    return true;
}


// ****************************************************************
// 
void cReplay::Clear()
{
    ASSERT(m_eMode == REPLAY_MODE_DISABLED)

    // 
    m_iCurFrame    = 0u;
    m_iCurSnapshot = 0u;
    m_iCurChange   = 0u;
    m_iEnvState    = 0u;

    // 
    std::memset(&m_Header,     0, sizeof(m_Header));
    std::memset(m_aInput,      0, sizeof(m_aInput));
    std::memset(m_aiCurPacket, 0, sizeof(m_aiCurPacket));

    // 
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        m_aInput[i].iMoveStep = 8u;
    }

    // 
    m_aSnapshot.clear();
    m_aChange  .clear();
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i) m_aaPacket[i].clear();
}


// ****************************************************************
// 
coreBool cReplay::HasData()
{
    // 
    if(!m_aSnapshot.empty()) return true;
    if(!m_aChange  .empty()) return true;

    // 
    for(coreUintW i = 0u; i < m_Header.iNumPlayers; ++i)
    {
        if(!m_aaPacket[i].empty()) return true;
    }

    return false;
}


// ****************************************************************
// 
void cReplay::SetName(const coreChar* pcName)
{
    //ASSERT(coreData::StrIsUpper(pcName))

    // 
    coreData::StrCopy(m_Header.acName, REPLAY_NAME_LENGTH, pcName);
}

void cReplay::SetNameDefault(const coreUint8 iSlot)
{
#if defined(_CORE_SWITCH_)

    // 
    this->SetName(PRINT("REPLAY %u", iSlot + 1u));

#else

    // 
    this->SetName(PRINT("REPLAY-%u", g_pSave->NextReplayFileNum()));

#endif
}


// ****************************************************************
// 
void cReplay::LoadInfoList(coreList<sInfo>* OUTPUT paInfoList)
{
    ASSERT(paInfoList)

    // 
    coreList<coreString> asFile;
    coreData::FolderScan(coreData::UserFolderPrivate(REPLAY_FILE_FOLDER), "*." REPLAY_FILE_EXTENSION, &asFile);

    // 
    paInfoList->reserve(asFile.size());

    FOR_EACH(it, asFile)
    {
        // 
        cReplay oReplay;
        if(oReplay.LoadFile(it->c_str(), true))
        {
            // 
            sInfo oInfo;
            oInfo.sPath   = std::move(*it);
            oInfo.iSlot   = 0xFFu;
            oInfo.oHeader = oReplay.m_Header;

            // 
            paInfoList->push_back(std::move(oInfo));
        }
    }

#if defined(REPLAY_SLOTSYSTEM)

    const coreUintW iNum = paInfoList->size();

    for(coreUintW i = 0u; i < REPLAY_SLOTS; ++i)
    {
        const coreChar* pcFilename = PRINT("replay_%04zu." REPLAY_FILE_EXTENSION, i);

        coreBool bFound = false;
        for(coreUintW j = 0u; j < iNum; ++j)
        {
            if(!std::strcmp(coreData::StrFilename((*paInfoList)[j].sPath.c_str()), pcFilename))
            {
                (*paInfoList)[j].iSlot = i;

                bFound = true;
                break;
            }
        }

        if(!bFound)
        {
            // 
            sInfo oInfo;
            oInfo.sPath   = pcFilename;
            oInfo.iSlot   = i;
            oInfo.oHeader = {};

            // 
            coreData::PrintBase(oInfo.oHeader.acName, REPLAY_NAME_LENGTH, "%s %zu", Core::Language->GetString("REPLAY_SLOT"), i + 1u);

            // 
            paInfoList->push_back(std::move(oInfo));
        }
    }

    // 
    std::sort(paInfoList->begin(), paInfoList->end(), [](const sInfo& A, const sInfo& B) {return (A.iSlot < B.iSlot);});

#else

    // 
    std::sort(paInfoList->begin(), paInfoList->end(), [](const sInfo& A, const sInfo& B) {return (A.oHeader.iViewTimestamp > B.oHeader.iViewTimestamp);});

#endif
}


// ****************************************************************
// 
coreBool cReplay::RenameReplay(const coreChar* pcPath, const coreChar* pcName)
{
    // 
    if(!coreData::FileExists(pcPath))
    {
        Core::Log->Warning("Replay (%s) does not exists!", pcPath);
        return false;
    }

    // 
    coreArchive oArchive(pcPath);
    coreFile* pHeaderFile = oArchive.GetFile("header");
    coreFile* pBodyFile   = oArchive.GetFile("body");
    WARN_IF(!pHeaderFile || !pBodyFile)
    {
        Core::Log->Warning("Replay (%s) could not be loaded!", pcPath);
        return false;
    }

    pHeaderFile->Decompress(sizeof(sHeader));
    {
        sHeader* pHeader = r_cast<sHeader*>(pHeaderFile->EditData());

        coreData::StrCopy(pHeader->acName, REPLAY_NAME_LENGTH, pcName);
        pHeader->iChecksum = cReplay::__GenerateChecksum(*pHeader);
    }
    pHeaderFile->Compress();

    // 
    WARN_IF(oArchive.Save(pcPath) != CORE_OK)
    {
        return false;
    }

    // 
    coreFile::FlushFilesystem();

    Core::Log->Info("Replay (%s) renamed", oArchive.GetPath());
    return true;
}


// ****************************************************************
// 
coreBool cReplay::__SetBodyData(const coreByte* pData, const coreUint32 iSize)
{
    coreUint32 iTargetSize = 0u;

    // 
    iTargetSize += sizeof(sSnapshot) * m_Header.iSnapshotCount;
    iTargetSize += sizeof(sChange)   * m_Header.iChangeCount;
    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
    {
        iTargetSize += sizeof(sPacket) * m_Header.aiPacketCount[i];
    }

    // 
    if(!pData || (iSize != iTargetSize)) return false;

    // 
    const coreByte* pCursor = pData;

    // 
    m_aSnapshot.resize(m_Header.iSnapshotCount);
    std::memcpy(m_aSnapshot.data(), pCursor, sizeof(sSnapshot) * m_Header.iSnapshotCount);
    pCursor += sizeof(sSnapshot) * m_Header.iSnapshotCount;

    // 
    m_aChange.resize(m_Header.iChangeCount);
    std::memcpy(m_aChange.data(), pCursor, sizeof(sChange) * m_Header.iChangeCount);
    pCursor += sizeof(sChange) * m_Header.iChangeCount;

    // 
    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
    {
        // 
        m_aaPacket[i].resize(m_Header.aiPacketCount[i]);
        std::memcpy(m_aaPacket[i].data(), pCursor, sizeof(sPacket) * m_Header.aiPacketCount[i]);
        pCursor += sizeof(sPacket) * m_Header.aiPacketCount[i];

        // revert delta encoding
        for(coreUintW j = 1u, je = m_aaPacket[i].size(); j < je; ++j)
        {
            m_aaPacket[i][j].iData = (m_aaPacket[i][j]     .iData & ~BITLINE(REPLAY_BITS_FRAME))  +
                                   (((m_aaPacket[i][j]     .iData &  BITLINE(REPLAY_BITS_FRAME))  +
                                     (m_aaPacket[i][j - 1u].iData &  BITLINE(REPLAY_BITS_FRAME))) & BITLINE(REPLAY_BITS_FRAME));
        }
    }

    ASSERT(pData + iSize == pCursor)

    return true;
}


// ****************************************************************
// 
coreBool cReplay::__GetBodyData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize)
{
    ASSERT(ppData && piSize)

    coreUint32 iSize = 0u;

    // 
    iSize += sizeof(sSnapshot) * m_aSnapshot.size();
    iSize += sizeof(sChange)   * m_aChange  .size();
    for(coreUintW i = 0u; i < m_Header.iNumPlayers; ++i)
    {
        iSize += sizeof(sPacket) * m_aaPacket[i].size();
    }

    // 
    if(!iSize) return false;

    // 
    coreByte* pData   = new coreByte[iSize];
    coreByte* pCursor = pData;

    // 
    std::memcpy(pCursor, m_aSnapshot.data(), sizeof(sSnapshot) * m_aSnapshot.size());
    pCursor += sizeof(sSnapshot) * m_aSnapshot.size();

    // 
    std::memcpy(pCursor, m_aChange.data(), sizeof(sChange) * m_aChange.size());
    pCursor += sizeof(sChange) * m_aChange.size();

    // 
    coreList<sPacket> aCopy;
    for(coreUintW i = 0u; i < m_Header.iNumPlayers; ++i)
    {
        // apply delta encoding (reduces average compressed file size by 30-50%)
        aCopy = m_aaPacket[i];
        for(coreUintW j = aCopy.size(); (j--) > 1u; )
        {
            aCopy[j].iData = (aCopy[j]     .iData & ~BITLINE(REPLAY_BITS_FRAME))  +
                           (((aCopy[j]     .iData &  BITLINE(REPLAY_BITS_FRAME))  -
                             (aCopy[j - 1u].iData &  BITLINE(REPLAY_BITS_FRAME))) & BITLINE(REPLAY_BITS_FRAME));
        }

        // 
        std::memcpy(pCursor, aCopy.data(), sizeof(sPacket) * aCopy.size());
        pCursor += sizeof(sPacket) * aCopy.size();
    }

    ASSERT(pData + iSize == pCursor)

    // 
    (*ppData) = pData;
    (*piSize) = iSize;

    return true;
}


// ****************************************************************
// 
cReplay::sPacket cReplay::__Pack(const sPacketRaw& oPacket)
{
    ASSERT((oPacket.iFrame <= BITLINE(REPLAY_BITS_FRAME)) && (oPacket.iType <= BITLINE(REPLAY_BITS_TYPE)) && (oPacket.iValue <= BITLINE(REPLAY_BITS_VALUE)))
    STATIC_ASSERT(INPUT_KEYS_ACTION <= BITLINE(REPLAY_BITS_VALUE))

    // 
    sPacket oOutput = {};
    oOutput.iData = BITVALUE(REPLAY_BITS_FRAME, 0u,                                   oPacket.iFrame) |
                    BITVALUE(REPLAY_BITS_TYPE,  REPLAY_BITS_FRAME,                    oPacket.iType)  |
                    BITVALUE(REPLAY_BITS_VALUE, REPLAY_BITS_FRAME + REPLAY_BITS_TYPE, oPacket.iValue);

    return oOutput;
}

cReplay::sPacketRaw cReplay::__Unpack(const sPacket& oPacket)
{
    // 
    sPacketRaw oOutput = {};
    oOutput.iFrame = GET_BITVALUE(oPacket.iData, REPLAY_BITS_FRAME, 0u);
    oOutput.iType  = GET_BITVALUE(oPacket.iData, REPLAY_BITS_TYPE,  REPLAY_BITS_FRAME);
    oOutput.iValue = GET_BITVALUE(oPacket.iData, REPLAY_BITS_VALUE, REPLAY_BITS_FRAME + REPLAY_BITS_TYPE);

    return oOutput;
}


// ****************************************************************
// 
void cReplay::__CheckHeader(sHeader* OUTPUT pHeader)
{
    // 
    pHeader->acName[REPLAY_NAME_LENGTH - 1u] = '\0';

    // 
    pHeader->iSnapshotCount = CLAMP(pHeader->iSnapshotCount, 0u, 10u * 1024u);   // # heuristic
    pHeader->iChangeCount   = CLAMP(pHeader->iChangeCount,   0u, 10u * 1024u);   // # heuristic
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        pHeader->aiPacketCount[i] = CLAMP(pHeader->aiPacketCount[i], 0u, 10u * 1024u * 1024u);   // # heuristic
    }

    // 
    pHeader->iNumPlayers  = CLAMP(pHeader->iNumPlayers,  1u, REPLAY_PLAYERS);
    pHeader->iNumMissions = CLAMP(pHeader->iNumMissions, 1u, REPLAY_MISSIONS);
    pHeader->iNumSegments = CLAMP(pHeader->iNumSegments, 1u, REPLAY_SEGMENTS);
    pHeader->iNumRuns     = CLAMP(pHeader->iNumRuns,     0u, REPLAY_RUNS);

    // 
    pHeader->iOptionKind       = CLAMP(pHeader->iOptionKind,       0u, GAME_KIND_MAX      -1u);
    pHeader->iOptionType       = CLAMP(pHeader->iOptionType,       0u, GAME_TYPE_MAX      -1u);
    pHeader->iOptionMode       = CLAMP(pHeader->iOptionMode,       0u, GAME_MODE_MAX      -1u);
    pHeader->iOptionDifficulty = CLAMP(pHeader->iOptionDifficulty, 0u, GAME_DIFFICULTY_MAX-1u);
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < REPLAY_EQUIP_WEAPONS;  ++j) pHeader->aaiOptionWeapon [i][j] = CLAMP(pHeader->aaiOptionWeapon [i][j], 0u, WEAPONS -1u);
        for(coreUintW j = 0u; j < REPLAY_EQUIP_SUPPORTS; ++j) pHeader->aaiOptionSupport[i][j] = CLAMP(pHeader->aaiOptionSupport[i][j], 0u, SUPPORTS-1u);
    }

    // 
    pHeader->iConfigGameSpeed    = CLAMP(pHeader->iConfigGameSpeed,    50u, 200u);
    pHeader->iConfigBackRotation = CLAMP(pHeader->iConfigBackRotation, 0u,  1u);
    pHeader->iConfigBackSpeed    = CLAMP(pHeader->iConfigBackSpeed,    50u, 200u);
    pHeader->iConfigUpdateFreq   = CLAMP(pHeader->iConfigUpdateFreq,   60u, 360u);
    pHeader->iConfigVersion      = CLAMP(pHeader->iConfigVersion,      1u,  g_Version.iNumber);

    // 
    pHeader->iConfigHitStop      = CLAMP(pHeader->iConfigHitStop,      0u, 1u);
    pHeader->iConfigRotationTurn = CLAMP(pHeader->iConfigRotationTurn, 0u, 1u);
    pHeader->iConfigPriorityMove = CLAMP(pHeader->iConfigPriorityMove, 0u, 1u);

    // 
    pHeader->iMissionIndexStart = CLAMP(pHeader->iMissionIndexStart, 0u,                          REPLAY_MISSIONS-1u);
    pHeader->iMissionIndexEnd   = CLAMP(pHeader->iMissionIndexEnd,   pHeader->iMissionIndexStart, REPLAY_MISSIONS-1u);

    // 
    if(pHeader->iNumPlayers == 1u) pHeader->iOptionType = GAME_TYPE_SOLO;
}


// ****************************************************************
// 
coreUint64 cReplay::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashXXH64(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}