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
cSave::cSave()noexcept
: m_Header {}
, m_sPath  (coreData::UserFolder(SAVE_FILE_FOLDER "/save." SAVE_FILE_EXTENSION))
{
    // 
    this->LoadFile();
}


// ****************************************************************
// destructor
cSave::~cSave()
{
    // 
    this->SaveFile();

    // 
    Core::Manager::Resource->UpdateFunctions();
}


// ****************************************************************
// 
RETURN_NONNULL cSave::sGlobalStats* cSave::EditGlobalStats()
{
    // 
    return &m_Header.oGlobalStats;
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsMission(const coreUintW iMissionIndex)
{
    // 
    ASSERT(iMissionIndex < REPLAY_MISSIONS)
    return &m_Header.aLocalStatsMission[iMissionIndex];
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsMission()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsMission(g_pGame->GetCurMissionIndex());
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    if(iSegmentIndex != MISSION_NO_SEGMENT)
    {
        ASSERT(iMissionIndex < REPLAY_MISSIONS)
        ASSERT(iSegmentIndex < REPLAY_SEGMENTS)
        return &m_Header.aaLocalStatsSegment[iMissionIndex][iSegmentIndex];
    }

    // 
    static sLocalStats s_LocalStatsDummy;
    return &s_LocalStatsDummy;
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsSegment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsSegment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}

RETURN_NONNULL cSave::sOptions* cSave::EditOptions()
{
    // 
    return &m_Header.oOptions;
}

RETURN_NONNULL cSave::sProgress* cSave::EditProgress()
{
    // 
    return &m_Header.oProgress;
}


// ****************************************************************
// 
coreBool cSave::LoadFile()
{
    // 
    if(!cSave::__LoadHeader(&m_Header, m_sPath.c_str()))
    {
        // 
        coreData::FileMove(m_sPath.c_str(), PRINT("%s.invalid_%s", m_sPath.c_str(), coreData::DateTimePrint("%Y%m%d_%H%M%S")));

        // 
        if(!cSave::__LoadHeader(&m_Header, PRINT("%s.backup", m_sPath.c_str())))
        {
            // 
            this->Clear();
            return false;
        }
    }

    // 
    cSave::__CheckHeader(&m_Header);
    return true;
}


// ****************************************************************
// 
void cSave::SaveFile()
{
    // 
    m_Header.iSaveTimestamp = std::time(NULL);
    m_Header.iSaveCount     = m_Header.iSaveCount + 1u;

    // 
    coreByte* pHeaderData = new coreByte[sizeof(sHeader)];
    std::memcpy(pHeaderData, &m_Header, sizeof(sHeader));

    Core::Manager::Resource->AttachFunction([=, this]()
    {
        // 
        coreData::FileMove(m_sPath.c_str(), PRINT("%s.backup", m_sPath.c_str()));

        // 
        sHeader* pHeader   = r_cast<sHeader*>(pHeaderData);
        pHeader->iChecksum = cSave::__GenerateChecksum(*pHeader);

        // 
        coreArchive oArchive;
        oArchive.CreateFile("header", pHeaderData, sizeof(sHeader))->Compress();

        // 
        oArchive.Save(m_sPath.c_str());

        Core::Log->Info("Save (%s) saved", m_sPath.c_str());
        return CORE_OK;
    });
}


// ****************************************************************
// 
void cSave::Clear()
{
    // 
    std::memset(&m_Header, 0, sizeof(m_Header));

    // 
    m_Header.iMagic               = SAVE_FILE_MAGIC;
    m_Header.iVersion             = SAVE_FILE_VERSION;
    m_Header.oProgress.bFirstPlay = true;

    // 
    for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i)
    {
        m_Header.oOptions.aaiWeapon [i][0] = 1u;
        m_Header.oOptions.aaiSupport[i][0] = 1u;
    }
}


// ****************************************************************
// 
coreBool cSave::__LoadHeader(sHeader* OUTPUT pHeader, const coreChar* pcPath)
{
    // 
    coreArchive oArchive(pcPath);
    coreFile* pHeaderFile = oArchive.GetFile("header");
    WARN_IF(!pHeaderFile)
    {
        Core::Log->Warning("Save (%s) could not be loaded!", pcPath);
        return false;
    }

    // 
    pHeaderFile->Decompress();

    // 
    std::memcpy(pHeader, pHeaderFile->GetData(), MIN(pHeaderFile->GetSize(), sizeof(sHeader)));
    WARN_IF((pHeader->iMagic    != SAVE_FILE_MAGIC)   ||
            (pHeader->iVersion  != SAVE_FILE_VERSION) ||
            (pHeader->iChecksum != cSave::__GenerateChecksum(*pHeader)))
    {
        Core::Log->Warning("Save (%s) is not a valid save-file!", pcPath);
        return false;
    }

    Core::Log->Info("Save (%s) loaded", pcPath);
    return true;
}


// ****************************************************************
// 
void cSave::__CheckHeader(sHeader* OUTPUT pHeader)
{
    // 
    pHeader->oOptions.acName[SAVE_NAME_LENGTH - 1u] = '\0';

    // 
    pHeader->oOptions.iStandard = CLAMP(pHeader->oOptions.iStandard, 0u, SAVE_MISSIONS-1u);
    pHeader->oOptions.iTraining = CLAMP(pHeader->oOptions.iTraining, 0u, SAVE_MISSIONS-1u);
    pHeader->oOptions.iPlayers  = CLAMP(pHeader->oOptions.iPlayers,  1u, SAVE_PLAYERS);
    for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < SAVE_EQUIP_WEAPONS;  ++j) pHeader->oOptions.aaiWeapon [i][j] = CLAMP(pHeader->oOptions.aaiWeapon [i][j], 0u, WEAPONS -1u);
        for(coreUintW j = 0u; j < SAVE_EQUIP_SUPPORTS; ++j) pHeader->oOptions.aaiSupport[i][j] = CLAMP(pHeader->oOptions.aaiSupport[i][j], 0u, SUPPORTS-1u);
    }

    // 
    for(coreUintW i = 0u; i < SAVE_MISSIONS; ++i)
    {
        pHeader->oProgress.aiAdvance     [i] = CLAMP(pHeader->oProgress.aiAdvance     [i], 0u, SAVE_SEGMENTS);
        pHeader->oProgress.aiMedalMission[i] = CLAMP(pHeader->oProgress.aiMedalMission[i], 0u, SAVE_MEDALS-1u);
        for(coreUintW j = 0u; j < SAVE_SEGMENTS; ++j)
        {
            pHeader->oProgress.aaiMedalSegment[i][j] = CLAMP(pHeader->oProgress.aaiMedalSegment[i][j], 0u, SAVE_MEDALS-1u);
        }
    }
}


// ****************************************************************
// 
coreUint64 cSave::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashMurmur64(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}