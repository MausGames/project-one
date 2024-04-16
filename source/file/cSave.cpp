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
, m_sPath  (SAVE_FILE_FOLDER "/save." SAVE_FILE_EXTENSION)
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
}


// ****************************************************************
// 
coreBool cSave::LoadFile()
{
    // 
    this->Clear();

    // 
    coreArchive oArchive(m_sPath.c_str());
    coreFile* pHeader = oArchive.GetFile("header");
    WARN_IF(!pHeader)
    {
        Core::Log->Warning("Save (%s) could not be loaded!", m_sPath.c_str());
        return false;
    }

    // 
    pHeader->Decompress();

    // 
    std::memcpy(&m_Header, pHeader->GetData(), MIN(pHeader->GetSize(), sizeof(sHeader)));
    WARN_IF((m_Header.iMagic    != SAVE_FILE_MAGIC)   ||
            (m_Header.iVersion  != SAVE_FILE_VERSION) ||
            (m_Header.iChecksum != cSave::__GenerateChecksum(m_Header)))
    {
        this->Clear();

        coreData::FileCopy(m_sPath.c_str(), PRINT("%s.invalid_%s", m_sPath.c_str(), coreData::DateTimePrint("%Y%m%d_%H%M%S")));

        Core::Log->Warning("Save (%s) is not a valid save-file!", m_sPath.c_str());
        return false;
    }

    Core::Log->Info("Save (%s) loaded", m_sPath.c_str());
    return true;
}


// ****************************************************************
// 
void cSave::SaveFile()
{
    // 
    m_Header.iSaveTimestamp = std::time(NULL);
    m_Header.iSaveCount     = m_Header.iSaveCount + 1u;
    m_Header.iChecksum      = cSave::__GenerateChecksum(m_Header);

    // 
    coreFile* pHeader = new coreFile("header", r_cast<coreByte*>(new sHeader(m_Header)), sizeof(sHeader));

    // 
    pHeader->Compress();

    // 
    coreData::CreateFolder(m_sPath.c_str());

    // 
    coreArchive oArchive;
    oArchive.AddFile(pHeader);
    oArchive.Save(m_sPath.c_str());

    Core::Log->Info("Save (%s) saved", m_sPath.c_str());
}


// ****************************************************************
// 
void cSave::Clear()
{
    // 
    std::memset(&m_Header, 0, sizeof(m_Header));

    // 
    m_Header.iMagic     = SAVE_FILE_MAGIC;
    m_Header.iVersion   = SAVE_FILE_VERSION;
    m_Header.bFirstPlay = true;

    // 
    for(coreUintW i = 0u; i < SAVE_PLAYERS; ++i)
    {
        m_Header.aiOptionWeapon [i] = 1u;
        m_Header.aiOptionSupport[i] = 1u;
    }
}


// ****************************************************************
// 
coreUint64 cSave::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashMurmur64A(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}