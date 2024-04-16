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
cSave::cSave()noexcept
: m_Header  {}
, m_sPath   (coreData::UserFolderPrivate(PRINT(SAVE_FILE_FOLDER "save%s." SAVE_FILE_EXTENSION, g_bDemoVersion ? "_demo" : "")))
, m_iToken  (0u)
, m_bIgnore (false)
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
    if(!m_bIgnore)
    {
        // 
        return &m_Header.oGlobalStats;
    }

    // 
    static sGlobalStats s_GlobalStatsDummy;
    return &s_GlobalStatsDummy;
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsMission(const coreUint8 iType, const coreUint8 iMode, const coreUint8 iDifficulty, const coreUintW iMissionIndex)
{
    if(!m_bIgnore)
    {
        // 
        ASSERT(iMissionIndex < REPLAY_MISSIONS)
        return &m_Header.aaaaLocalStatsMission[iType][iMode][iDifficulty][iMissionIndex];
    }

    // 
    static sLocalStats s_LocalStatsDummy;
    return &s_LocalStatsDummy;
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsMission(const coreUintW iMissionIndex)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsMission(g_pGame->GetType(), g_pGame->GetMode(), g_pGame->GetDifficulty(), iMissionIndex);
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsMission()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsMission(g_pGame->GetType(), g_pGame->GetMode(), g_pGame->GetDifficulty(), g_pGame->GetCurMissionIndex());
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsSegment(const coreUint8 iType, const coreUint8 iMode, const coreUint8 iDifficulty, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    if(!m_bIgnore)
    {
        // 
        if(iSegmentIndex != MISSION_NO_SEGMENT)
        {
            ASSERT(iMissionIndex < REPLAY_MISSIONS)
            ASSERT(iSegmentIndex < REPLAY_SEGMENTS)
            return &m_Header.aaaaaLocalStatsSegment[iType][iMode][iDifficulty][iMissionIndex][iSegmentIndex];
        }

        WARN_IF(false) {}
    }

    // 
    static sLocalStats s_LocalStatsDummy;
    return &s_LocalStatsDummy;
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsSegment(g_pGame->GetType(), g_pGame->GetMode(), g_pGame->GetDifficulty(), iMissionIndex, iSegmentIndex);
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsSegment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsSegment(g_pGame->GetType(), g_pGame->GetMode(), g_pGame->GetDifficulty(), g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}

RETURN_NONNULL cSave::sOptions* cSave::EditOptions()
{
    if(!m_bIgnore)
    {
        // 
        return &m_Header.oOptions;
    }

    // 
    static sOptions s_OptionsDummy;
    return &s_OptionsDummy;
}

RETURN_NONNULL cSave::sProgress* cSave::EditProgress()
{
    if(!m_bIgnore)
    {
        // 
        return &m_Header.oProgress;
    }

    // 
    static sProgress s_ProgressDummy;
    return &s_ProgressDummy;
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
    Core::Manager::Resource->DetachFunction(m_iToken);

    // 
    static coreByte s_aHeaderData[sizeof(sHeader)];
    std::memcpy(s_aHeaderData, &m_Header, sizeof(sHeader));

    m_iToken = Core::Manager::Resource->AttachFunction([this]()
    {
        // 
        coreData::FileMove(m_sPath.c_str(), PRINT("%s.backup", m_sPath.c_str()));

        // 
        sHeader* pHeader   = r_cast<sHeader*>(s_aHeaderData);
        pHeader->iChecksum = cSave::__GenerateChecksum(*pHeader);

        // 
        coreByte*  pCompressedData;
        coreUint32 iCompressedSize;
        if(coreData::Compress(s_aHeaderData, sizeof(sHeader), &pCompressedData, &iCompressedSize) != CORE_OK)
        {
            pCompressedData = s_aHeaderData;
            iCompressedSize = sizeof(sHeader);
        }

        // 
        coreArchive oArchive;
        oArchive.CreateFile("header", pCompressedData, iCompressedSize);

        // 
        WARN_IF(oArchive.Save(m_sPath.c_str()) != CORE_OK)
        {
            Core::Log->Warning("Save (%s) could not be saved", m_sPath.c_str());
            return CORE_OK;
        }

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
    ADD_BIT_EX(m_Header.oProgress.aiNew, NEW_MAIN_CONFIG)

    // 
    for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i)
    {
        m_Header.oOptions.aiShield  [i]    = 30u;
        m_Header.oOptions.aaiWeapon [i][0] = 1u;
        m_Header.oOptions.aaiSupport[i][0] = 0u;
    }
}


// ****************************************************************
// 
coreBool cSave::__LoadHeader(sHeader* OUTPUT pHeader, const coreChar* pcPath)
{
    // 
    if(!coreData::FileExists(pcPath))
    {
        Core::Log->Warning("Save (%s) does not exist!", pcPath);
        return false;
    }

    // 
    coreArchive oArchive(pcPath);
    coreFile* pHeaderFile = oArchive.GetFile("header");
    WARN_IF(!pHeaderFile)
    {
        Core::Log->Warning("Save (%s) could not be loaded!", pcPath);
        return false;
    }

    // 
    if(coreData::Decompress(pHeaderFile->GetData(), pHeaderFile->GetSize(), r_cast<coreByte*>(pHeader), sizeof(sHeader)) != CORE_OK)
    {
        std::memcpy(pHeader, pHeaderFile->GetData(), MIN(pHeaderFile->GetSize(), sizeof(sHeader)));
    }

    // 
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
    pHeader->oOptions.iType       = CLAMP(pHeader->oOptions.iType,       0u, GAME_TYPE_MAX      -1u);
    pHeader->oOptions.iMode       = CLAMP(pHeader->oOptions.iMode,       0u, GAME_MODE_MAX      -1u);
    pHeader->oOptions.iDifficulty = CLAMP(pHeader->oOptions.iDifficulty, 0u, GAME_DIFFICULTY_MAX-1u);
    for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i)
    {
        pHeader->oOptions.aiShield[i] = CLAMP(pHeader->oOptions.aiShield[i], 0u, SHIELD_MAX);
        for(coreUintW j = 0u; j < SAVE_EQUIP_WEAPONS;  ++j) pHeader->oOptions.aaiWeapon [i][j] = CLAMP(pHeader->oOptions.aaiWeapon [i][j], 0u, WEAPONS -1u);
        for(coreUintW j = 0u; j < SAVE_EQUIP_SUPPORTS; ++j) pHeader->oOptions.aaiSupport[i][j] = CLAMP(pHeader->oOptions.aaiSupport[i][j], 0u, SUPPORTS-1u);
    }

    // 
    for(coreUintW i = 0u; i < SAVE_MISSIONS; ++i)
    {
        pHeader->oProgress.aiAdvance[i] = CLAMP(pHeader->oProgress.aiAdvance[i], 0u, SAVE_SEGMENTS);
    }
    for(coreUintW i = 0u; i < SAVE_TYPES; ++i)
    {
        for(coreUintW j = 0u; j < SAVE_MODES; ++j)
        {
            for(coreUintW k = 0u; k < SAVE_DIFFICULTIES; ++k)
            {
                for(coreUintW l = 0u; l < SAVE_MISSIONS; ++l)
                {
                    pHeader->oProgress.aaaaiMedalMission[i][j][k][l] = CLAMP(pHeader->oProgress.aaaaiMedalMission[i][j][k][l], 0u, SAVE_MEDALS-1u);
                    for(coreUintW m = 0u; m < SAVE_SEGMENTS; ++m)
                    {
                        pHeader->oProgress.aaaaaiMedalSegment[i][j][k][l][m] = CLAMP(pHeader->oProgress.aaaaaiMedalSegment[i][j][k][l][m], 0u, SAVE_MEDALS-1u);
                    }
                }
            }
        }
    }

    // handle players which use ALT+F4 after finishing the intro mission
    if(pHeader->oProgress.bFirstPlay && pHeader->oProgress.aiAdvance[1])
    {
        pHeader->oProgress.bFirstPlay = false;
        if(!g_bDemoVersion) ADD_BIT_EX(pHeader->oProgress.aiNew, NEW_MAIN_EXTRA)
    }

#if defined(_CORE_DEBUG_)
    std::memset(pHeader->oProgress.aiFragment, 1, sizeof(pHeader->oProgress.aiFragment));
#endif
    
    
    if(!HAS_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_MIRRORMORE))  g_CurConfig.Game.iMirrorMode = 0u;
    if(!HAS_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_GAMESPEEDUP)) g_CurConfig.Game.iGameSpeed  = MIN(g_CurConfig.Game.iGameSpeed, 100u);
}


// ****************************************************************
// 
coreUint64 cSave::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashXXH64(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}