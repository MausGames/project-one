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
: m_Header      {}
, m_aSnapshot   {}
, m_aaPacket    {}
, m_aInput      {}
, m_iCurFrame   (0u)
, m_aiCurPacket {}
, m_iStatus     (REPLAY_STATUS_DISABLED)
{
}


// ****************************************************************
// 
void cReplay::CreateGame()
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)
    ASSERT(!STATIC_ISVALID(g_pGame) && this->__CanStartPlayback())

    // 
    sGameOptions oOptions = {};
    oOptions.iPlayers     = m_Header.iNumPlayers;
    oOptions.iDifficulty  = m_Header.iOptionDifficulty;
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < REPLAY_EQUIP_WEAPONS;  ++j) oOptions.aaiWeapon [i][j] = m_Header.aaiOptionWeapon [i][j];
        for(coreUintW j = 0u; j < REPLAY_EQUIP_SUPPORTS; ++j) oOptions.aaiSupport[i][j] = m_Header.aaiOptionSupport[i][j];
    }

    // 
    STATIC_NEW(g_pGame, oOptions, m_Header.aiMissionList, m_Header.iNumMissions)
    g_pGame->LoadMissionIndex(m_Header.iMissionStartIndex);
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
    m_Header.iStartTimestamp = std::time(NULL);
    m_Header.iNumPlayers     = g_pGame->GetCoop() ? REPLAY_PLAYERS : 1u;
    m_Header.iNumMissions    = g_pGame->GetNumMissions();
    m_Header.iNumSegments    = REPLAY_SEGMENTS;

    // 
    m_Header.iOptionDifficulty = g_pGame->GetOptions().iDifficulty;
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < REPLAY_EQUIP_WEAPONS;  ++j) m_Header.aaiOptionWeapon [i][j] = g_pGame->GetOptions().aaiWeapon [i][j];
        for(coreUintW j = 0u; j < REPLAY_EQUIP_SUPPORTS; ++j) m_Header.aaiOptionSupport[i][j] = g_pGame->GetOptions().aaiSupport[i][j];
    }

    // 
    m_Header.iConfigUpdateFreq = g_CurConfig.Game.iUpdateFreq;
    m_Header.iConfigVersion    = 1u;   // TODO 1 

    // 
    m_Header.iMissionStartIndex = g_pGame->GetCurMissionIndex();
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
    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
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
    m_Header.iEndTimestamp  = std::time(NULL);
    m_Header.iFrameCount    = m_iCurFrame;
    m_Header.iSnapshotCount = m_aSnapshot.size();
    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
    {
        m_Header.aiPacketCount[i] = m_aaPacket[i].size();
    }

    // 
    {
        const cTimeTable* pTable = g_pGame->GetTimeTable();

        m_Header.iTimeTotal = pTable->GetTimeTotalRaw();
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aiTimeMission[j] = pTable->GetTimeMissionRaw(j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaiTimeSegment[j][i] = pTable->GetTimeSegmentRaw(j, i);
    }

    // 
    for(coreUintW k = 0u, ke = m_Header.iNumPlayers; k < ke; ++k)
    {
        const cScoreTable* pTable = g_pGame->GetPlayer(k)->GetScoreTable();

        m_Header.aiScoreTotal[k] = pTable->GetScoreTotal();
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) m_Header.aaiScoreMission[k][j] = pTable->GetScoreMission(j);
        for(coreUintW j = 0u, je = m_Header.iNumMissions; j < je; ++j) for(coreUintW i = 0u, ie = m_Header.iNumSegments; i < ie; ++i) m_Header.aaaiScoreSegment[k][j][i] = pTable->GetScoreSegment(j, i);
    }

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
void cReplay::ApplySnapshot(const coreUint16 iIdentifier)
{
    if(!STATIC_ISVALID(g_pGame)) return;

    if(m_iStatus == REPLAY_STATUS_RECORDING)
    {
        // 
        sSnapshot oSnapshot = {};
        oSnapshot.iIdentifier = iIdentifier;
        oSnapshot.iFrame      = m_iCurFrame;

        // 
        m_aSnapshot.push_back(oSnapshot);
    }
    else if(m_iStatus == REPLAY_STATUS_PLAYBACK)
    {
        for(coreUintW i = 0u, ie = m_aSnapshot.size(); i < ie; ++i)
        {
            const sSnapshot oSnapshot = m_aSnapshot[i];
            if(oSnapshot.iIdentifier == iIdentifier)
            {
                // 
                ASSERT(oSnapshot.iFrame == m_iCurFrame)
                break;
            }
        }
    }
}


// ****************************************************************
// 
void cReplay::Update()
{
    if(!STATIC_ISVALID(g_pGame)) return;

    // 
    if(!HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) return;
    ++m_iCurFrame;

    if(m_iStatus == REPLAY_STATUS_RECORDING)
    {
        const auto nNewPacketFunc = [this](const coreUintW iIndex, const coreUint32 iType, const coreUint32 iValue)
        {
            // 
            sPacketRaw oNewPacketRaw = {};
            oNewPacketRaw.iFrame = m_iCurFrame;
            oNewPacketRaw.iType  = iType;
            oNewPacketRaw.iValue = iValue;

            // 
            const sPacket oNewPacket = cReplay::__Pack(oNewPacketRaw);

            // 
            ASSERT(m_aaPacket[iIndex].empty() || std::memcmp(&m_aaPacket[iIndex].back(), &oNewPacket, sizeof(sPacket)))
            m_aaPacket[iIndex].push_back(oNewPacket);
        };

        for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
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
        for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
        {
            sGameInput& oCurInput = m_aInput[i];

            // 
            oCurInput.iActionPress   = 0u;
            oCurInput.iActionRelease = 0u;

            for(coreUintW j = m_aiCurPacket[i], je = m_aaPacket[i].size(); j < je; ++j)
            {
                const sPacketRaw oPacketRaw = cReplay::__Unpack(m_aaPacket[i][j]);
                if(oPacketRaw.iFrame == m_iCurFrame)
                {
                    // 
                    switch(oPacketRaw.iType)
                    {
                    case REPLAY_TYPE_MOVE:
                        oCurInput.vMove = UnpackDirection(oPacketRaw.iValue);
                        break;

                    case REPLAY_TYPE_PRESS:
                        oCurInput.iActionPress = oPacketRaw.iValue;
                        oCurInput.iActionHold |= oPacketRaw.iValue;
                        break;

                    case REPLAY_TYPE_RELEASE:
                        oCurInput.iActionRelease =  oPacketRaw.iValue;
                        oCurInput.iActionHold   &= ~oPacketRaw.iValue;
                        break;

                    default:
                        WARN_IF(true) {}
                        break;
                    }
                }
                else
                {
                    ASSERT(oPacketRaw.iFrame > m_iCurFrame)

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
    coreFile* pHeaderFile = oArchive.GetFile("header");
    coreFile* pBodyFile   = oArchive.GetFile("body");
    WARN_IF(!pHeaderFile || !pBodyFile)
    {
        Core::Log->Warning("Replay (%s) could not be loaded!", pcPath);
        return false;
    }

    // 
    pHeaderFile->Decompress();

    // 
    std::memcpy(&m_Header, pHeaderFile->GetData(), MIN(pHeaderFile->GetSize(), sizeof(sHeader)));
    WARN_IF((m_Header.iMagic    != REPLAY_FILE_MAGIC)    ||
            (m_Header.iVersion  != REPLAY_FILE_VERSION)  ||
            (m_Header.iBodySize != pBodyFile->GetSize()) ||
            (m_Header.iChecksum != cReplay::__GenerateChecksum(m_Header)))
    {
        Core::Log->Warning("Replay (%s) is not a valid replay-file!", pcPath);
        return false;
    }

    // 
    cReplay::__CheckHeader(&m_Header);

    // 
    if(!bOnlyHeader)
    {
        WARN_IF(!this->__SetBodyData(pBodyFile->GetData(), pBodyFile->GetSize()))
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
void cReplay::SaveFile(const coreChar* pcName)
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)

    // 
    coreByte*  pBodyData = NULL;
    coreUint32 iBodySize = 0;
    if(!this->__GetBodyData(&pBodyData, &iBodySize) || !pBodyData || !iBodySize) return;

    // 
    coreData::StrCopy(m_Header.acName, ARRAY_SIZE(m_Header.acName), pcName);
    m_Header.iBodySize = iBodySize;
    m_Header.iChecksum = cReplay::__GenerateChecksum(m_Header);

    // 
    coreByte* pHeaderData = new coreByte[sizeof(sHeader)];
    std::memcpy(pHeaderData, &m_Header, sizeof(sHeader));

    // 
    coreArchive oArchive;
    oArchive.CreateFile("header", pHeaderData, sizeof(sHeader))->Compress();
    oArchive.CreateFile("body",   pBodyData,   iBodySize);

    // 
    oArchive.Save(coreData::UserFolder(coreData::DateTimePrint(REPLAY_FILE_FOLDER "/replay_%Y%m%d_%H%M%S." REPLAY_FILE_EXTENSION)));

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
    m_aSnapshot.clear();
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        m_aaPacket[i].clear();
        m_aaPacket[i].reserve(0xFFFFu / sizeof(sPacket));
    }
}


// ****************************************************************
// 
void cReplay::LoadInfoList(coreList<sInfo>* OUTPUT paInfoList)
{
    ASSERT(paInfoList)

    // 
    coreList<coreString> asFile;
    coreData::ScanFolder(coreData::UserFolder(REPLAY_FILE_FOLDER), "*." REPLAY_FILE_EXTENSION, &asFile);

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
            oInfo.oHeader = oReplay.m_Header;

            // 
            paInfoList->push_back(std::move(oInfo));
        }
    }
}


// ****************************************************************
// 
coreBool cReplay::__SetBodyData(const coreByte* pData, const coreUint32 iSize)
{
    coreUint32 iTargetSize = 0u;

    // 
    iTargetSize += sizeof(sSnapshot) * m_Header.iSnapshotCount;
    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
    {
        iTargetSize += sizeof(sPacket) * m_Header.aiPacketCount[i];
    }

    // 
    coreByte*  pPlainData;
    coreUint32 iPlainSize;
    if(coreData::Decompress(pData, iSize, &pPlainData, &iPlainSize) != CORE_OK) return false;

    // 
    if(iPlainSize != iTargetSize)
    {
        SAFE_DELETE_ARRAY(pPlainData)
        return false;
    }

    // 
    coreByte* pCursor = pPlainData;

    // 
    m_aSnapshot.resize(m_Header.iSnapshotCount);
    std::memcpy(m_aSnapshot.data(), pCursor, sizeof(sSnapshot) * m_Header.iSnapshotCount);
    pCursor += sizeof(sSnapshot) * m_Header.iSnapshotCount;

    for(coreUintW i = 0u, ie = m_Header.iNumPlayers; i < ie; ++i)
    {
        // 
        m_aaPacket[i].resize(m_Header.aiPacketCount[i]);
        std::memcpy(m_aaPacket[i].data(), pCursor, sizeof(sPacket) * m_Header.aiPacketCount[i]);
        pCursor += sizeof(sPacket) * m_Header.aiPacketCount[i];
    }

    // 
    SAFE_DELETE_ARRAY(pPlainData)
    return true;
}


// ****************************************************************
// 
coreBool cReplay::__GetBodyData(coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize)const
{
    coreUint32 iPlainSize = 0u;

    // 
    iPlainSize += sizeof(sSnapshot) * m_aSnapshot.size();
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        iPlainSize += sizeof(sPacket) * m_aaPacket[i].size();
    }

    // 
    if(!iPlainSize) return false;

    // 
    coreByte* pPlainData = new coreByte[iPlainSize];
    coreByte* pCursor    = pPlainData;

    // 
    std::memcpy(pCursor, m_aSnapshot.data(), sizeof(sSnapshot) * m_aSnapshot.size());
    pCursor += sizeof(sSnapshot) * m_aSnapshot.size();

    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        // 
        std::memcpy(pCursor, m_aaPacket[i].data(), sizeof(sPacket) * m_aaPacket[i].size());
        pCursor += sizeof(sPacket) * m_aaPacket[i].size();
    }

    // 
    if(coreData::Compress(pPlainData, iPlainSize, ppData, piSize) != CORE_OK)
    {
        SAFE_DELETE_ARRAY(pPlainData)
        return false;
    }

    // 
    SAFE_DELETE_ARRAY(pPlainData)
    return true;
}


// ****************************************************************
// 
cReplay::sPacket cReplay::__Pack(const sPacketRaw& oPacket)
{
    ASSERT((oPacket.iFrame <= BITLINE(22u)) && (oPacket.iType <= BITLINE(2u)) && (oPacket.iValue <= BITLINE(5u)))
    STATIC_ASSERT(INPUT_KEYS_ACTION <= 5u)

    // 
    sPacket oOutput = {};
    oOutput.iData = BITVALUE(22u, 7u, oPacket.iFrame) |
                    BITVALUE( 2u, 5u, oPacket.iType)  |
                    BITVALUE( 5u, 0u, oPacket.iValue);

    return oOutput;
}

cReplay::sPacketRaw cReplay::__Unpack(const sPacket& oPacket)
{
    // 
    sPacketRaw oOutput = {};
    oOutput.iFrame = GET_BITVALUE(oPacket.iData, 22u, 7u);
    oOutput.iType  = GET_BITVALUE(oPacket.iData,  2u, 5u);
    oOutput.iValue = GET_BITVALUE(oPacket.iData,  5u, 0u);

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
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        pHeader->aiPacketCount[i] = CLAMP(pHeader->aiPacketCount[i], 0u, 10u * 1024u * 1024u);   // # heuristic
    }

    // 
    pHeader->iNumPlayers  = CLAMP(pHeader->iNumPlayers,  1u, REPLAY_PLAYERS);
    pHeader->iNumMissions = CLAMP(pHeader->iNumMissions, 1u, REPLAY_MISSIONS);
    pHeader->iNumSegments = CLAMP(pHeader->iNumSegments, 1u, REPLAY_SEGMENTS);

    // 
    for(coreUintW i = 0u; i < REPLAY_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < REPLAY_EQUIP_WEAPONS;  ++j) pHeader->aaiOptionWeapon [i][j] = CLAMP(pHeader->aaiOptionWeapon [i][j], 0u, WEAPONS -1u);
        for(coreUintW j = 0u; j < REPLAY_EQUIP_SUPPORTS; ++j) pHeader->aaiOptionSupport[i][j] = CLAMP(pHeader->aaiOptionSupport[i][j], 0u, SUPPORTS-1u);
    }

    // 
    pHeader->iConfigUpdateFreq = CLAMP(pHeader->iConfigUpdateFreq, F_TO_UI(FRAMERATE_MIN), F_TO_UI(FRAMERATE_MAX));
    pHeader->iConfigVersion    = CLAMP(pHeader->iConfigVersion,    1u,                     1u);   // TODO 1 

    // 
    pHeader->iMissionStartIndex = CLAMP(pHeader->iMissionStartIndex, 0u, REPLAY_MISSIONS-1u);
}


// ****************************************************************
// 
coreUint64 cReplay::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashMurmur64(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}