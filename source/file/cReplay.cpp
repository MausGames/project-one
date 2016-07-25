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
{

}


// ****************************************************************
// 
void cReplay::LoadFile(const coreChar* pcName)
{
    // 
    coreArchive oArchive(pcName);
    coreFile* pHeader = oArchive.GetFile("header");
    coreFile* pBody   = oArchive.GetFile("body");

    // 
    sFileData oFileData;
    std::memcpy(&oFileData, pHeader->GetData(), sizeof(sFileData));

    // 
    this->SetData(pBody->GetData(), pBody->GetSize());


}


// ****************************************************************
// 
void cReplay::SaveFile(const coreChar* pcName)const
{
    // 
    sFileData* pFileData = new sFileData();
    pFileData->iMagic   = REPLAY_FILE_MAGIC;
    pFileData->iVersion = REPLAY_FILE_VERSION;
    std::snprintf(pFileData->acName, 64u, "%s", "Testname");

    // 
    coreByte*  pData;
    coreUint32 iSize;
    this->GetData(&pData, &iSize);

    // 
    coreFile* pHeader = new coreFile("header", r_cast<coreByte*>(pFileData), sizeof(sFileData));
    coreFile* pBody   = new coreFile("body",   pData,                        iSize);
    pBody->Compress();

    // 
    coreArchive oArchive;
    oArchive.AddFile(pHeader);
    oArchive.AddFile(pBody);
    oArchive.Save(pcName);


}


// ****************************************************************
// 
void cReplay::SetData(const coreByte* pInput, const coreUint32 iSize)
{


}


// ****************************************************************
// 
void cReplay::GetData(coreByte** OUTPUT ppOutput, coreUint32* OUTPUT piSize)const
{


}