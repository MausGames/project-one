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
: m_Header    {}
, m_sPath     (coreData::UserFolderPrivate(PRINT(SAVE_FILE_FOLDER "save."      SAVE_FILE_EXTENSION)))
, m_sPathDemo (coreData::UserFolderPrivate(PRINT(SAVE_FILE_FOLDER "save_demo." SAVE_FILE_EXTENSION)))
, m_iToken    (0u)
, m_bIgnore   (false)
, m_eStatus   (SAVE_STATUS_OK)
, m_iActive   (0u)
{
    // 
    if(g_bDemoVersion) m_sPath = m_sPathDemo;

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

    // 
    FOR_EACH(it, m_apScoreQueue) MANAGED_DELETE(*it)
    m_apScoreQueue.clear();
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

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsArcade(const coreUint8 iType, const coreUint8 iMode, const coreUint8 iDifficulty)
{
    if(!m_bIgnore)
    {
        // 
        if(!STATIC_ISVALID(g_pGame) || (g_pGame->GetKind() == GAME_KIND_ALL))
        {
            // 
            return &m_Header.aaaLocalStatsArcade[iType][iMode][iDifficulty];
        }
    }

    // 
    static sLocalStats s_LocalStatsDummy;
    return &s_LocalStatsDummy;
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsArcade()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsArcade(g_pGame->GetType(), g_pGame->GetMode(), g_pGame->GetDifficulty());
}

RETURN_NONNULL cSave::sLocalStats* cSave::EditLocalStatsMission(const coreUint8 iType, const coreUint8 iMode, const coreUint8 iDifficulty, const coreUintW iMissionIndex)
{
    if(!m_bIgnore)
    {
        // 
        if(!STATIC_ISVALID(g_pGame) || (g_pGame->GetKind() == GAME_KIND_ALL) || (g_pGame->GetKind() == GAME_KIND_MISSION))
        {
            // 
            ASSERT(iMissionIndex < SAVE_MISSIONS)
            return &m_Header.aaaaLocalStatsMission[iType][iMode][iDifficulty][iMissionIndex];
        }
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
        if(iSegmentIndex != MISSION_NO_SEGMENT)
        {
            // 
            ASSERT(iMissionIndex < SAVE_MISSIONS)
            ASSERT(iSegmentIndex < SAVE_SEGMENTS)
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
coreBool cSave::LoadFile(const coreChar* pcPath)
{
    // 
    if(!cSave::__LoadHeader(&m_Header, &m_apScoreQueue, pcPath))
    {
        // 
        coreData::FileMove(pcPath, PRINT("%s.invalid_%s", pcPath, coreData::DateTimePrint("%Y%m%d_%H%M%S")));

        // 
        if(!cSave::__LoadHeader(&m_Header, &m_apScoreQueue, PRINT("%s.backup", pcPath)))
        {
            // 
            this->Clear();
            return false;
        }
    }

    // 
    cSave::__UpgradeHeader(&m_Header);
    cSave::__CheckHeader  (&m_Header);
    return true;
}

coreBool cSave::LoadFile()
{
    return this->LoadFile(m_sPath.c_str());
}


// ****************************************************************
// 
void cSave::SaveFile()
{
    // 
    this->ResetStatus();
    m_iActive = 1u;

    // 
    m_Header.iSaveTimestamp = std::time(NULL);
    m_Header.iSaveCount     = m_Header.iSaveCount + 1u;

    // 
    Core::Manager::Resource->DetachFunction(m_iToken);

    // 
    static coreByte s_aHeaderData[sizeof(sHeader)];
    std::memcpy(s_aHeaderData, &m_Header, sizeof(sHeader));

    // 
    static coreByte*  s_pQueueData = NULL;
    static coreUint32 s_iQueueSize = 0u;
    cSave::__CreateQueueData(m_apScoreQueue, &s_pQueueData, &s_iQueueSize);

    m_iToken = Core::Manager::Resource->AttachFunction([this]()
    {
    #if !defined(_CORE_SWITCH_)

        // 
        coreData::FileMove(m_sPath.c_str(), PRINT("%s.backup", m_sPath.c_str()));

    #endif

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
        if(s_pQueueData)
        {
            oArchive.CreateFile("score", s_pQueueData, s_iQueueSize)->Compress();
            s_pQueueData = NULL;
        }

        // 
        WARN_IF(oArchive.Save(m_sPath.c_str()) != CORE_OK)
        {
            coreUint64 iAvailable;
            coreData::SystemSpace(&iAvailable, NULL);

                 if(iAvailable < 1u * 1024u * 1024u)                                    m_eStatus = SAVE_STATUS_ERROR_SPACE;
            else if(!coreData::FolderWritable(coreData::StrDirectory(m_sPath.c_str()))) m_eStatus = SAVE_STATUS_ERROR_ACCESS;
            else                                                                        m_eStatus = SAVE_STATUS_ERROR_UNKNOWN;

            // 
            m_iActive = 2u;

            Core::Log->Warning("Save (%s) could not be saved (status %u)", m_sPath.c_str(), coreUint32(m_eStatus));
            return CORE_OK;
        }

        // 
        coreFile::FlushFilesystem();

        // 
        m_iActive = 2u;

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
        m_Header.oOptions.aiShield  [i]    = i ? 0u : 10u;
        m_Header.oOptions.aaiWeapon [i][0] = 1u;
        m_Header.oOptions.aaiSupport[i][0] = 0u;
    }
}


// ****************************************************************
// 
void cSave::ImportDemo()
{
    ASSERT(!g_bDemoVersion)

    // 
    if(this->LoadFile(m_sPathDemo.c_str()))
    {
        // 
        m_Header.oOptions.iNavigation = 0u;
        ADD_BIT_EX(m_Header.oProgress.aiNew,   NEW_MAIN_START)
        ADD_BIT_EX(m_Header.oProgress.aiNew,   NEW_MAIN_SCORE)
        //ADD_BIT_EX(m_Header.oProgress.aiNew,   NEW_MAIN_REPLAY)   // [RP]
        ADD_BIT_EX(m_Header.oProgress.aiNew,   NEW_MAIN_EXTRA)
        ADD_BIT_EX(m_Header.oProgress.aiState, STATE_DEMO_IMPORTED)
    }
}


// ****************************************************************
// 
coreBool cSave::CanImportDemo()const
{
    return !g_bDemoVersion && !coreData::FileExists(m_sPath.c_str()) && !coreData::FileExists(PRINT("%s.backup", m_sPath.c_str())) && coreData::FileExists(m_sPathDemo.c_str());
}


// ****************************************************************
// 
coreUint32 cSave::NextReplayNum()
{
    return ++m_Header.iReplayCount;
}


// ****************************************************************
// 
coreBool cSave::__LoadHeader(sHeader* OUTPUT pHeader, coreSet<sScorePack*>* OUTPUT pQueue, const coreChar* pcPath)
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
            (pHeader->iVersion  >  SAVE_FILE_VERSION) ||
            (pHeader->iChecksum != cSave::__GenerateChecksum(*pHeader)))
    {
        Core::Log->Warning("Save (%s) is not a valid save-file!", pcPath);
        return false;
    }

    // 
    coreFile* pScoreFile = oArchive.GetFile("score");
    if(pScoreFile)
    {
        // 
        pScoreFile->Decompress();

        // 
        WARN_IF(!cSave::__RestoreQueueData(pQueue, pScoreFile->GetData(), pScoreFile->GetSize()))
        {
            Core::Log->Warning("Save (%s) contains a broken score-queue!", pcPath);
        }
    }

    Core::Log->Info("Save (%s) loaded", pcPath);
    return true;
}


// ****************************************************************
// 
void cSave::__UpgradeHeader(sHeader* OUTPUT pHeader)
{
    // 
    if(pHeader->iVersion <= 1u)   // TODO 1: legacy version, this is the only one which can be deleted after some time
    {
        struct sProgressOld final
        {
            coreUint8  aiPadding1[6221];
            coreUint8  aiHelper  [SAVE_MISSIONS];
            coreUint8  aiFragment[SAVE_MISSIONS];
            coreUint8  aaiBadge  [SAVE_MISSIONS][SAVE_SEGMENTS];
            coreUint64 aiState   [1];
            coreUint64 aiTrophy  [2];
            coreUint64 aiUnlock  [2];
            coreUint64 aiNew     [2];
        };

        sProgressOld oProgressOld;
        std::memcpy(&oProgressOld, &pHeader->oProgress, sizeof(sProgress)); STATIC_ASSERT(sizeof(sProgress) == sizeof(sProgressOld))

        std::memcpy(pHeader->oProgress.aiHelper,    oProgressOld.aiHelper,   sizeof(sProgress::aiHelper));   STATIC_ASSERT(sizeof(sProgress::aiHelper)   == sizeof(sProgressOld::aiHelper))
        std::memcpy(pHeader->oProgress.aiFragment,  oProgressOld.aiFragment, sizeof(sProgress::aiFragment)); STATIC_ASSERT(sizeof(sProgress::aiFragment) == sizeof(sProgressOld::aiFragment))
        std::memcpy(pHeader->oProgress.aaiBadge,    oProgressOld.aaiBadge,   sizeof(sProgress::aaiBadge));   STATIC_ASSERT(sizeof(sProgress::aaiBadge)   == sizeof(sProgressOld::aaiBadge))
        std::memcpy(pHeader->oProgress.aiState,     oProgressOld.aiState,    sizeof(sProgress::aiState));    STATIC_ASSERT(sizeof(sProgress::aiState)    == sizeof(sProgressOld::aiState))
        std::memcpy(pHeader->oProgress.aiTrophy,    oProgressOld.aiTrophy,   sizeof(sProgress::aiTrophy));   STATIC_ASSERT(sizeof(sProgress::aiTrophy)   == sizeof(sProgressOld::aiTrophy))
        std::memcpy(pHeader->oProgress.aiUnlock,    oProgressOld.aiUnlock,   sizeof(sProgress::aiUnlock));   STATIC_ASSERT(sizeof(sProgress::aiUnlock)   == sizeof(sProgressOld::aiUnlock))
        std::memcpy(pHeader->oProgress.aiNew,       oProgressOld.aiNew,      sizeof(sProgress::aiNew));      STATIC_ASSERT(sizeof(sProgress::aiNew)      == sizeof(sProgressOld::aiNew))
    }

    // 
    if(pHeader->iVersion <= 2u)
    {
        if(pHeader->oProgress.aiAdvance[1] >= 7) ADD_BIT_EX(pHeader->oProgress.aiState, STATE_STORY_VIRIDO)
        if(pHeader->oProgress.aiAdvance[2] >= 7) ADD_BIT_EX(pHeader->oProgress.aiState, STATE_STORY_NEVO)
        if(pHeader->oProgress.aiAdvance[3] >= 7) ADD_BIT_EX(pHeader->oProgress.aiState, STATE_STORY_HARENA)
        if(pHeader->oProgress.aiAdvance[4] >= 7) ADD_BIT_EX(pHeader->oProgress.aiState, STATE_STORY_RUTILUS)
        if(pHeader->oProgress.aiAdvance[5] >= 7) ADD_BIT_EX(pHeader->oProgress.aiState, STATE_STORY_GELU)
        if(pHeader->oProgress.aiAdvance[6] >= 7) ADD_BIT_EX(pHeader->oProgress.aiState, STATE_STORY_CALOR)
        if(pHeader->oProgress.aiAdvance[7] >= 7) ADD_BIT_EX(pHeader->oProgress.aiState, STATE_STORY_MUSCUS)
    }

    // 
    if(pHeader->iVersion <= 3u)
    {
        if(!pHeader->oProgress.bFirstPlay)
        {
            ADD_BIT_EX(pHeader->oProgress.aiNew, NEW_MAIN_SCORE)
        }
    }

    // 
    //if(pHeader->iVersion <= 4u)   // [RP]
    //{
    //    if(!pHeader->oProgress.bFirstPlay)
    //    {
    //        ADD_BIT_EX(pHeader->oProgress.aiNew, NEW_MAIN_REPLAY)
    //    }
    //}

    // 
    pHeader->iVersion = SAVE_FILE_VERSION;
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
        pHeader->oProgress.aiAdvance[i] = CLAMP(pHeader->oProgress.aiAdvance[i], 0u, SAVE_SEGMENTS+1u);
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
    if(pHeader->oProgress.bFirstPlay && (pHeader->oProgress.aiAdvance[0] >= 7u))
    {
        pHeader->oProgress.bFirstPlay = false;
        if(!g_bDemoVersion)
        {
            ADD_BIT_EX(pHeader->oProgress.aiNew, NEW_MAIN_START)
            ADD_BIT_EX(pHeader->oProgress.aiNew, NEW_MAIN_SCORE)
            //ADD_BIT_EX(pHeader->oProgress.aiNew, NEW_MAIN_REPLAY)   // [RP]
            ADD_BIT_EX(pHeader->oProgress.aiNew, NEW_MAIN_EXTRA)
        }
    }

#if defined(_CORE_SWITCH_)

    // 
    REMOVE_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_GAMESPEEDUP)

#endif

    // 
    if(!HAS_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_MIRRORMODE))   g_CurConfig.Game.iMirrorMode = 0u;
    if(!HAS_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_GAMESPEEDUP))  g_CurConfig.Game.iGameSpeed  = MIN(g_CurConfig.Game.iGameSpeed, 100u);
    if(!HAS_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_POWERSHIELD))  for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i) pHeader->oOptions.aiShield [i]    = MIN(pHeader->oOptions.aiShield [i],    SHIELD_DEFAULT);
    if(!HAS_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_WEAPON_PULSE)) for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i) pHeader->oOptions.aaiWeapon[i][0] = MIN(pHeader->oOptions.aaiWeapon[i][0], 1u);
    if(!HAS_BIT_EX(pHeader->oProgress.aiUnlock, UNLOCK_WEAPON_WAVE))  for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i) pHeader->oOptions.aaiWeapon[i][0] = MIN(pHeader->oOptions.aaiWeapon[i][0], 2u);
}


// ****************************************************************
// 
void cSave::__CreateQueueData(const coreSet<sScorePack*>& apQueue, coreByte** OUTPUT ppData, coreUint32* OUTPUT piSize)
{
    ASSERT(ppData && piSize)

    // 
    if(*ppData) SAFE_DELETE_ARRAY(*ppData)

    // 
    const coreUint32 iQueueNum  = apQueue.size();
    const coreUint32 iQueueSize = sizeof(coreUint32) + sizeof(sScorePack) * iQueueNum;

    if(iQueueNum)
    {
        coreByte* pQueueData = new coreByte[iQueueSize];
        coreByte* pCursor    = pQueueData;

        std::memcpy(pCursor, &iQueueNum, sizeof(coreUint32)); pCursor += sizeof(coreUint32);

        for(coreUintW i = 0u, ie = iQueueNum; i < ie; ++i)
        {
            std::memcpy(pCursor, apQueue[i], sizeof(sScorePack)); pCursor += sizeof(sScorePack);
        }

        ASSERT(iQueueSize == coreUint32(pCursor - pQueueData))

        (*ppData) = pQueueData;
        (*piSize) = iQueueSize;
    }
    else
    {
        (*ppData) = NULL;
        (*piSize) = 0u;
    }
}


// ****************************************************************
// 
coreBool cSave::__RestoreQueueData(coreSet<sScorePack*>* OUTPUT pQueue, const coreByte* pData, const coreUint32 iSize)
{
    if(iSize < sizeof(coreUint32)) return false;

    const coreByte* pCursor = pData;

    coreUint32 iQueueNum;
    std::memcpy(&iQueueNum, pCursor, sizeof(coreUint32)); pCursor += sizeof(coreUint32);

    if(iSize != sizeof(coreUint32) + sizeof(sScorePack) * iQueueNum) return false;

    ASSERT(pQueue->empty())
    pQueue->reserve(iQueueNum);

    for(coreUintW i = 0u, ie = iQueueNum; i < ie; ++i)
    {
        sScorePack* pPack = MANAGED_NEW(sScorePack);
        std::memcpy(pPack, pCursor, sizeof(sScorePack)); pCursor += sizeof(sScorePack);

        pPack->iStatus = 0u;

        pQueue->push_back(pPack);
    }

    ASSERT(iSize == coreUint32(pCursor - pData))

    return true;
}


// ****************************************************************
// 
coreUint64 cSave::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashXXH64(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}