//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
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
void cReplay::StartRecording()
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)
    ASSERT(g_pGame && !m_Header.iMagic && m_aaStreamPacket[0].empty())

    // 
    this->Clear();

    // 
    m_Header.iMagic          = REPLAY_FILE_MAGIC;
    m_Header.iVersion        = REPLAY_FILE_VERSION;
    m_Header.iExecutableHash = cReplay::__CalculateExecutableHash();
    m_Header.iStartTimestamp = std::time(NULL);
    m_Header.iStreamCount    = g_pGame->GetCoop() ? GAME_PLAYERS : 1u;
    ASSERT(m_Header.iStreamCount <= REPLAY_STREAMS)

    // 
    m_iCurFrame = 0u;
    m_iStatus   = REPLAY_STATUS_RECORDING;
}


// ****************************************************************
// 
void cReplay::StartPlayback()
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)
    ASSERT(g_pGame && m_Header.iMagic && !m_aaStreamPacket[0].empty())

    // 
    for(coreUintW i = 0u; i < m_Header.iStreamCount; ++i)
    {
        g_pGame->GetPlayer(i)->SetInput(&m_aInput[i]);
    }

    // 
    m_iCurFrame = 0u;
    m_iStatus   = REPLAY_STATUS_PLAYBACK;
}


// ****************************************************************
// 
void cReplay::End()
{
    ASSERT(m_iStatus != REPLAY_STATUS_DISABLED)

    // 
    if(m_iStatus == REPLAY_STATUS_RECORDING)
    {
        m_Header.iReplayHash   = this->__CalculateReplayHash();
        m_Header.iEndTimestamp = std::time(NULL);
        m_Header.iKeyFrameSize = m_aKeyFrame.size();
        for(coreUintW i = 0u; i < REPLAY_STREAMS; ++i)
        {
            m_Header.aiStreamPacketSize[i] = m_aaStreamPacket[i].size();
        }

        // TODO: score, time, actions 
    }

    // 
    m_iStatus = REPLAY_STATUS_DISABLED;
}


// ****************************************************************
// 
void cReplay::Update()
{
    if(!g_pGame) return;

    // 
    if((g_pGame->GetTimeMission() < 0.0f) || !CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) return;
    ++m_iCurFrame;

    if(m_iStatus == REPLAY_STATUS_RECORDING)
    {
        auto nNewPacketFunc = [this](const coreUintW iIndex, const coreUint32 iType, const coreUint32 iValue)
        {
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

        for(coreUintW i = 0u; i < m_Header.iStreamCount; ++i)
        {
            const sInput* pNewInput = g_pGame->GetPlayer(i)->GetInput();

            // 
            if(!coreMath::InRange(pNewInput->vMove.x, m_aInput[i].vMove.x, CORE_MATH_PRECISION) ||
               !coreMath::InRange(pNewInput->vMove.y, m_aInput[i].vMove.y, CORE_MATH_PRECISION))
            {
                nNewPacketFunc(i, REPLAY_TYPE_MOVE, pNewInput->vMove.IsNull() ? 8u : (F_TO_UI(ROUND(pNewInput->vMove.Angle() / (0.25f*PI))) & 0x07u));
            }

            // 
            if(pNewInput->iButtonPress)   nNewPacketFunc(i, REPLAY_TYPE_PRESS,   pNewInput->iButtonPress);
            if(pNewInput->iButtonRelease) nNewPacketFunc(i, REPLAY_TYPE_RELEASE, pNewInput->iButtonRelease);

            // 
            m_aInput[i] = (*pNewInput);
        }
    }
    else if(m_iStatus == REPLAY_STATUS_PLAYBACK)
    {
        for(coreUintW i = 0u; i < m_Header.iStreamCount; ++i)
        {
            sInput& oCurInput = m_aInput[i];

            // 
            oCurInput.iButtonPress   = 0u;
            oCurInput.iButtonRelease = 0u;

            for(coreUintW j = m_aiCurPacket[i], je = m_aaStreamPacket[i].size(); j < je; ++j)
            {
                const sStreamPacket& oPacket = m_aaStreamPacket[i][j];
                if(oPacket.iFrame == m_iCurFrame)
                {
                    // 
                    switch(oPacket.iType)
                    {
                    case REPLAY_TYPE_MOVE:
                        oCurInput.vMove = (oPacket.iValue == 8u) ? coreVector2(0.0f,0.0f) : coreVector2::Direction(I_TO_F(oPacket.iValue) * (0.25f*PI));
                        break;

                    case REPLAY_TYPE_PRESS:
                        oCurInput.iButtonPress = oPacket.iValue;
                        oCurInput.iButtonHold |= oPacket.iValue;
                        break;

                    case REPLAY_TYPE_RELEASE:
                        oCurInput.iButtonRelease =  oPacket.iValue;
                        oCurInput.iButtonHold   &= ~oPacket.iValue;
                        break;

                    default: ASSERT(false)
                    }
                }
                else
                {
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
    if(!pHeader || !pBody)
    {
        Core::Log->Warning("Replay (%s) could not be loaded!", pcPath);
        return false;
    }

    // 
    std::memcpy(&m_Header, pHeader->GetData(), sizeof(sHeader));
    if((m_Header.iMagic   != REPLAY_FILE_MAGIC) ||
       (m_Header.iVersion != REPLAY_FILE_VERSION))
    {
        Core::Log->Warning("Replay (%s) is not a valid replay-file!", pcPath);
        return false;
    }

    // 
    if(!bOnlyHeader) this->__SetBodyData(pBody->GetData(), pBody->GetSize());
    return true;
}


// ****************************************************************
// 
void cReplay::SaveFile(const coreChar* pcName)
{
    ASSERT(m_iStatus == REPLAY_STATUS_DISABLED)

    // 
    std::snprintf(m_Header.acName, ARRAY_SIZE(m_Header.acName), "%s", pcName);
    sHeader* pHeaderData = new sHeader(m_Header);

    // 
    coreByte*  pBodyData = NULL;
    coreUint32 iBodySize = 0;
    this->__GetBodyData(&pBodyData, &iBodySize);
    ASSERT(pBodyData && iBodySize)

    // 
    coreFile* pHeader = new coreFile("header", r_cast<coreByte*>(pHeaderData), sizeof(sHeader));
    coreFile* pBody   = new coreFile("body",   pBodyData,                      iBodySize);

    // 
    coreArchive oArchive;
    oArchive.AddFile(pHeader);
    oArchive.AddFile(pBody);
    oArchive.Save(PRINT(REPLAY_FILE_FOLDER "/%s_%s." REPLAY_FILE_EXTENSION, pcName, coreData::DateTimePrint("%Y%m%d_%H%M%S")));
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
void cReplay::LoadInfoList(std::vector<uInfo>* OUTPUT paInfoList)
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
        cReplay oNewReplay;
        oNewReplay.LoadFile(it->c_str(), true);

        // 
        paInfoList->emplace_back(std::move(*it), oNewReplay.m_Header);
    }
}


// ****************************************************************
// 
void cReplay::__SetBodyData(const coreByte* pData, const coreUint32 iSize)
{
    // 
    coreByte*  pPlainData;
    coreUint32 iPlainSize;
    coreData::DecompressDeflate(pData, iSize, &pPlainData, &iPlainSize);

    // 
    coreByte* pCursor = pPlainData;

    // 
    m_aKeyFrame.resize(m_Header.iKeyFrameSize);
    std::memcpy(m_aKeyFrame.data(), pCursor, sizeof(sKeyFrame) * m_Header.iKeyFrameSize);
    pCursor += sizeof(sKeyFrame) * m_Header.iKeyFrameSize;

    for(coreUintW i = 0u; i < m_Header.iStreamCount; ++i)
    {
        // 
        m_aaStreamPacket[i].resize(m_Header.aiStreamPacketSize[i]);
        std::memcpy(m_aaStreamPacket[i].data(), pCursor, sizeof(sStreamPacket) * m_Header.aiStreamPacketSize[i]);
        pCursor += sizeof(sStreamPacket) * m_Header.aiStreamPacketSize[i];
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
    coreData::CompressDeflate(pPlainData, iPlainSize, ppData, piSize);

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
        s_iExecutableHash = coreHashCRC32(oExecutable.GetData(), oExecutable.GetSize());
    }

    return s_iExecutableHash;
}


// ****************************************************************
// 
coreUint32 cReplay::__CalculateReplayHash()const
{
    coreUint32 iReplayHash = 0u;

    // 
    iReplayHash ^= coreHashCRC32(r_cast<const coreByte*>(m_aKeyFrame.data()), sizeof(sKeyFrame) * m_aKeyFrame.size());
    for(coreUintW i = 0u; i < REPLAY_STREAMS; ++i)
    {
        iReplayHash ^= coreHashCRC32(r_cast<const coreByte*>(m_aaStreamPacket[i].data()), sizeof(sStreamPacket) * m_aaStreamPacket[i].size());
    }

    return iReplayHash;
}