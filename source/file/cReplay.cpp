///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cReplay::cReplay()noexcept
: m_Header         {}
, m_aKeyFrame      {}
, m_aaStreamPacket {{}}
, m_aInput         {{}}
, m_iCurFrame      (0u)
, m_aiCurPacket    {}
, m_iStatus        (REPLAY_STATUS_DISABLED)
{
}


// ****************************************************************
// 
void cReplay::CreateGame()
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)
    ASSERT(!STATIC_ISVALID(g_pGame) && this->__CanStartPlayback())

    // 
    sGameConfig oConfig = {};
    oConfig.iDifficulty =  m_Header.iGameDifficulty;
    oConfig.bCoop       = (m_Header.iGamePlayers > 1u) ? true : false;
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        oConfig.aaiWeapon [i][0] = 0u;
        oConfig.aaiSupport[i][0] = 0u;
    }

    // 
    STATIC_NEW(g_pGame, oConfig, m_Header.aiMissionList, m_Header.iNumMissions)
    g_pGame->LoadNextMission();
}


// ****************************************************************
// 
void cReplay::StartRecording()
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)
    ASSERT(STATIC_ISVALID(g_pGame) && this->__CanStartRecording())

    // 
    this->Clear();

    // 
    m_Header.iMagic          = REPLAY_FILE_MAGIC;
    m_Header.iVersion        = REPLAY_FILE_VERSION;
    m_Header.iExecutableHash = cReplay::__CalculateExecutableHash();
    m_Header.iStartTimestamp = std::time(NULL);
    m_Header.iGameMode       = 0u;
    m_Header.iGameDifficulty = g_pGame->GetDifficulty();
    m_Header.iGamePlayers    = g_pGame->GetCoop() ? GAME_PLAYERS : 1u;
    m_Header.iPacifist       = g_pGame->GetPacifist();
    m_Header.iNumStreams     = g_pGame->GetCoop() ? GAME_PLAYERS : 1u;
    m_Header.iNumMissions    = g_pGame->GetNumMissions();
    m_Header.iNumBosses      = REPLAY_BOSSES;
    m_Header.iNumWaves       = REPLAY_WAVES;
    ASSERT(m_Header.iNumStreams <= REPLAY_STREAMS)

    // 
    std::memcpy(m_Header.aiMissionList, g_pGame->GetMissionList(), sizeof(coreInt32) * g_pGame->GetNumMissions());

    // 
    m_iCurFrame = 0u;
    m_iStatus   = REPLAY_STATUS_RECORDING;

    Core::Log->Info("Replay recording started");
}


// ****************************************************************
// 
void cReplay::StartPlayback()
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)
    ASSERT(STATIC_ISVALID(g_pGame) && this->__CanStartPlayback())

    // 
    for(coreUintW i = 0u, ie = m_Header.iNumStreams; i < ie; ++i)
    {
        g_pGame->GetPlayer(i)->SetInput(&m_aInput[i]);
    }

    // 
    m_iCurFrame = 0u;
    m_iStatus   = REPLAY_STATUS_PLAYBACK;

    Core::Log->Info("Replay playback started");
}


// ****************************************************************
// 
void cReplay::EndRecording()
{
    ASSERT(m_iStatus == REPLAY_STATUS_RECORDING)
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_Header.iReplayHash    = this->__CalculateReplayHash();
    m_Header.iEndTimestamp  = std::time(NULL);
    m_Header.iKeyFrameCount = m_aKeyFrame.size();
    for(coreUintW i = 0u, ie = m_Header.iNumStreams; i < ie; ++i)
    {
        m_Header.aiStreamPacketCount[i] = m_aaStreamPacket[i].size();
    }

    // 
    {
        const cTimeTable* pTable = g_pGame->GetTimeTable();

        m_Header.iTimeTotal = pTable->GetTimeTotalRaw();
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiTimeMission[j] = pTable->GetTimeMissionRaw(j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumBosses; i < ie; ++i) m_Header.aaiTimeBoss[j][i] = pTable->GetTimeBossRaw(j, i);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumWaves;  i < ie; ++i) m_Header.aaiTimeWave[j][i] = pTable->GetTimeWaveRaw(j, i);
    }

    // 
    for(coreUintW k = 0u, ke = m_Header.iNumStreams; k < ke; ++k)
    {
        const cScoreTable* pTable = g_pGame->GetPlayer(k)->GetScoreTable();

        m_Header.aiScoreTotal[k] = pTable->GetScoreTotal();
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aaiScoreMission[k][j] = pTable->GetScoreMission(j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumBosses; i < ie; ++i) m_Header.aaaiScoreBoss[k][j][i] = pTable->GetScoreBoss(j, i);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumWaves;  i < ie; ++i) m_Header.aaaiScoreWave[k][j][i] = pTable->GetScoreWave(j, i);
    }

    // 
    std::memset(m_Header.aiActionsTotal,    0, sizeof(m_Header.aiActionsTotal));
    std::memset(m_Header.aaiActionsMission, 0, sizeof(m_Header.aaiActionsMission));
    std::memset(m_Header.aaaiActionsBoss,   0, sizeof(m_Header.aaaiActionsBoss));
    std::memset(m_Header.aaaiActionsWave,   0, sizeof(m_Header.aaaiActionsWave));

    // 
    m_iStatus = REPLAY_STATUS_DISABLED;

    Core::Log->Info("Replay recording ended");
}


// ****************************************************************
// 
void cReplay::EndPlayback()
{
    ASSERT(m_iStatus == REPLAY_STATUS_PLAYBACK)
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_iStatus = REPLAY_STATUS_DISABLED;

    Core::Log->Info("Replay playback ended");
}


// ****************************************************************
// 
void cReplay::ApplyKeyFrame(const coreUint16 iIdentifier)
{
    if(!STATIC_ISVALID(g_pGame)) return;

    if(m_iStatus == REPLAY_STATUS_RECORDING)
    {

    }
    else if(m_iStatus == REPLAY_STATUS_PLAYBACK)
    {

    }
}


// ****************************************************************
// 
void cReplay::Update()
{
    if(!STATIC_ISVALID(g_pGame)) return;

    // 
    if(!CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) return;
    ++m_iCurFrame;

    if(m_iStatus == REPLAY_STATUS_RECORDING)
    {
        const auto nNewPacketFunc = [this](const coreUintW iIndex, const coreUint32 iType, const coreUint32 iValue)
        {
            ASSERT((m_iCurFrame <= BITLINE(22u)) && (iType <= BITLINE(2u)) && (iValue <= BITLINE(4u)))

            // 
            sStreamPacket oNewPacket;
            oNewPacket.iFrame    = m_iCurFrame;
            oNewPacket.iType     = iType;
            oNewPacket.iValue    = iValue;
            oNewPacket.iReserved = 0u;

            // 
            ASSERT(m_aaStreamPacket[iIndex].empty() || std::memcmp(&m_aaStreamPacket[iIndex].back(), &oNewPacket, sizeof(sStreamPacket)))
            m_aaStreamPacket[iIndex].push_back(oNewPacket);
        };

        for(coreUintW i = 0u, ie = m_Header.iNumStreams; i < ie; ++i)
        {
            const sGameInput* pNewInput = g_pGame->GetPlayer(i)->GetInput();

            // 
            if(!coreMath::IsNear(pNewInput->vMove.x, m_aInput[i].vMove.x) ||
               !coreMath::IsNear(pNewInput->vMove.y, m_aInput[i].vMove.y))
            {
                nNewPacketFunc(i, REPLAY_TYPE_MOVE, PackDirection(pNewInput->vMove));
            }

            // 
            if(pNewInput->iActionPress)   nNewPacketFunc(i, REPLAY_TYPE_PRESS,   pNewInput->iActionPress);
            if(pNewInput->iActionRelease) nNewPacketFunc(i, REPLAY_TYPE_RELEASE, pNewInput->iActionRelease);

            // 
            m_aInput[i] = (*pNewInput);
        }
    }
    else if(m_iStatus == REPLAY_STATUS_PLAYBACK)
    {
        for(coreUintW i = 0u, ie = m_Header.iNumStreams; i < ie; ++i)
        {
            sGameInput& oCurInput = m_aInput[i];

            // 
            oCurInput.iActionPress   = 0u;
            oCurInput.iActionRelease = 0u;

            for(coreUintW j = m_aiCurPacket[i], je = m_aaStreamPacket[i].size(); j < je; ++j)
            {
                const sStreamPacket& oPacket = m_aaStreamPacket[i][j];
                if(oPacket.iFrame == m_iCurFrame)
                {
                    // 
                    switch(oPacket.iType)
                    {
                    case REPLAY_TYPE_MOVE:
                        oCurInput.vMove = UnpackDirection(oPacket.iValue);
                        break;

                    case REPLAY_TYPE_PRESS:
                        oCurInput.iActionPress = oPacket.iValue;
                        oCurInput.iActionHold |= oPacket.iValue;
                        break;

                    case REPLAY_TYPE_RELEASE:
                        oCurInput.iActionRelease =  oPacket.iValue;
                        oCurInput.iActionHold   &= ~oPacket.iValue;
                        break;

                    default: ASSERT(false)
                    }
                }
                else
                {
                    //ASSERT(oPacket.iFrame > m_iCurFrame)   // TODO: triggers also at the end of stream 

                    // 
                    m_aiCurPacket[i] = j;
                    break;
                }
            }
        }
    }
}


// ****************************************************************
// 
coreBool cReplay::LoadFile(const coreChar* pcPath, const coreBool bOnlyHeader)
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)

    // 
    this->Clear();

    // 
    coreArchive oArchive(pcPath);
    coreFile* pHeader = oArchive.GetFile("header");
    coreFile* pBody   = oArchive.GetFile("body");
    WARN_IF(!pHeader || !pBody)
    {
        Core::Log->Warning("Replay (%s) could not be loaded!", pcPath);
        return false;
    }

    // 
    pHeader->Unscramble();

    // 
    std::memcpy(&m_Header, pHeader->GetData(), MIN(pHeader->GetSize(), sizeof(sHeader)));
    WARN_IF((m_Header.iMagic    != SAVE_FILE_MAGIC)   ||
            (m_Header.iVersion  != SAVE_FILE_VERSION) ||
            (m_Header.iBodySize != pBody->GetSize())  ||
            (m_Header.iChecksum != cReplay::__GenerateChecksum(m_Header)))
    {
        Core::Log->Warning("Replay (%s) is not a valid replay-file!", pcPath);
        return false;
    }

    // 
    if(!bOnlyHeader) this->__SetBodyData(pBody->GetData(), pBody->GetSize());

    Core::Log->Info("Replay (%s) loaded", pcPath);
    return true;
}


// ****************************************************************
// 
void cReplay::SaveFile(const coreChar* pcName)
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)

    // 
    coreByte*  pBodyData = NULL;
    coreUint32 iBodySize = 0;
    this->__GetBodyData(&pBodyData, &iBodySize);
    ASSERT(pBodyData && iBodySize)

    // dont save if nothing to save (assert in compress-func) 

    // 
    coreData::StrCopy(pcName, m_Header.acName, ARRAY_SIZE(m_Header.acName));
    m_Header.iBodySize = iBodySize;
    m_Header.iChecksum = cReplay::__GenerateChecksum(m_Header);

    // 
    coreFile* pHeader = new coreFile("header", r_cast<coreByte*>(new sHeader(m_Header)), sizeof(sHeader));
    coreFile* pBody   = new coreFile("body", pBodyData, iBodySize);
   // coreFile* pSave   = new coreFile("save", pBodyData, iBodySize);

    // 
    pHeader->Scramble();

    // 
    coreData::CreateFolder(REPLAY_FILE_FOLDER);

    // 
    coreArchive oArchive;
    oArchive.AddFile(pHeader);
    oArchive.AddFile(pBody);
    oArchive.Save(coreData::DateTimePrint(REPLAY_FILE_FOLDER "/replay_%Y%m%d_%H%M%S." REPLAY_FILE_EXTENSION));

    Core::Log->Info("Replay (%s) saved", oArchive.GetPath());
}


// ****************************************************************
// 
void cReplay::Clear()
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)

    // 
    std::memset(&m_Header,     0, sizeof(m_Header));
    std::memset(m_aInput,      0, sizeof(m_aInput));
    std::memset(m_aiCurPacket, 0, sizeof(m_aiCurPacket));

    // 
    m_aKeyFrame.clear();
    for(coreUintW i = 0u; i < REPLAY_STREAMS; ++i)
    {
        m_aaStreamPacket[i].clear();
        m_aaStreamPacket[i].reserve(0xFFFFu / sizeof(sStreamPacket));
    }
}


// ****************************************************************
// 
void cReplay::LoadInfoList(std::vector<sInfo>* OUTPUT paInfoList)
{
    ASSERT(paInfoList)

    // 
    std::vector<std::string> asFile;
    coreData::ScanFolder(REPLAY_FILE_FOLDER, "*." REPLAY_FILE_EXTENSION, &asFile);

    // 
    paInfoList->reserve(asFile.size());

    FOR_EACH(it, asFile)
    {
        // 
        cReplay oReplay;
        oReplay.LoadFile(it->c_str(), true);

        // 
        sInfo oInfo;
        oInfo.sPath   = std::move(*it);
        oInfo.oHeader = oReplay.m_Header;

        // 
        paInfoList->push_back(std::move(oInfo));
    }
}


// ****************************************************************
// 
void cReplay::__SetBodyData(const coreByte* pData, const coreUint32 iSize)
{
    // 
    coreByte*  pPlainData;
    coreUint32 iPlainSize;
    coreData::Decompress(pData, iSize, &pPlainData, &iPlainSize);

    // 
    coreByte* pCursor = pPlainData;

    // 
    m_aKeyFrame.resize(m_Header.iKeyFrameCount);
    std::memcpy(m_aKeyFrame.data(), pCursor, sizeof(sKeyFrame) * m_Header.iKeyFrameCount);
    pCursor += sizeof(sKeyFrame) * m_Header.iKeyFrameCount;

    for(coreUintW i = 0u, ie = m_Header.iNumStreams; i < ie; ++i)
    {
        // 
        m_aaStreamPacket[i].resize(m_Header.aiStreamPacketCount[i]);
        std::memcpy(m_aaStreamPacket[i].data(), pCursor, sizeof(sStreamPacket) * m_Header.aiStreamPacketCount[i]);
        pCursor += sizeof(sStreamPacket) * m_Header.aiStreamPacketCount[i];
    }

    // 
    SAFE_DELETE_ARRAY(pPlainData)
}


// ****************************************************************
// 
void cReplay::__GetBodyData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize)const
{
    coreUint32 iPlainSize = 0u;

    // 
    iPlainSize += sizeof(sKeyFrame) * m_aKeyFrame.size();
    for(coreUintW i = 0u; i < REPLAY_STREAMS; ++i)
    {
        iPlainSize += sizeof(sStreamPacket) * m_aaStreamPacket[i].size();
    }

    // 
    coreByte* pPlainData = new coreByte[iPlainSize];
    coreByte* pCursor    = pPlainData;

    // 
    std::memcpy(pCursor, m_aKeyFrame.data(), sizeof(sKeyFrame) * m_aKeyFrame.size());
    pCursor += sizeof(sKeyFrame) * m_aKeyFrame.size();

    for(coreUintW i = 0u; i < REPLAY_STREAMS; ++i)
    {
        // 
        std::memcpy(pCursor, m_aaStreamPacket[i].data(), sizeof(sStreamPacket) * m_aaStreamPacket[i].size());
        pCursor += sizeof(sStreamPacket) * m_aaStreamPacket[i].size();
    }

    // 
    coreData::Compress(pPlainData, iPlainSize, ppData, piSize);

    // 
    SAFE_DELETE_ARRAY(pPlainData)
}


// ****************************************************************
// 
coreUint32 cReplay::__CalculateExecutableHash()
{
    static coreUint32 s_iExecutableHash = 0u;

    // 
    if(!s_iExecutableHash)
    {
        coreFile oExecutable(coreData::AppPath());
        s_iExecutableHash = coreHashMurmur2(oExecutable.GetData(), oExecutable.GetSize());
    }

    return s_iExecutableHash;
}


// ****************************************************************
// 
coreUint32 cReplay::__CalculateReplayHash()const
{
    coreUint32 iReplayHash = 0u;

    // 
    iReplayHash ^= coreHashMurmur2(r_cast<const coreByte*>(m_aKeyFrame.data()), sizeof(sKeyFrame) * m_aKeyFrame.size());
    for(coreUintW i = 0u; i < REPLAY_STREAMS; ++i)
    {
        iReplayHash ^= coreHashMurmur2(r_cast<const coreByte*>(m_aaStreamPacket[i].data()), sizeof(sStreamPacket) * m_aaStreamPacket[i].size());
    }

    return iReplayHash;
}


// ****************************************************************
// 
coreUint64 cReplay::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashMurmur64A(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}