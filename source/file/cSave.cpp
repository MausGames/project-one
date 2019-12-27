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
// 
cSave::sGlobalStats* cSave::EditGlobalStats()
{
    // 
    return &m_Header.oGlobalStats;
}

cSave::sLocalStats* cSave::EditLocalStatsMission(const coreUintW iMissionIndex)
{
    // 
    ASSERT(iMissionIndex < REPLAY_MISSIONS)
    return &m_Header.aLocalStatsMission[iMissionIndex];
}

cSave::sLocalStats* cSave::EditLocalStatsMission()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsMission(g_pGame->GetCurMissionIndex());
}

cSave::sLocalStats* cSave::EditLocalStatsSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    ASSERT(iMissionIndex < REPLAY_MISSIONS)
    ASSERT(iSegmentIndex < REPLAY_SEGMENTS)
    return &m_Header.aaLocalStatsSegment[iMissionIndex][iSegmentIndex];
}

cSave::sLocalStats* cSave::EditLocalStatsSegment()
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return this->EditLocalStatsSegment(g_pGame->GetCurMissionIndex(), g_pGame->GetCurMission()->GetCurSegmentIndex());
}

cSave::sOptions* cSave::EditOptions()
{
    // 
    return &m_Header.oOptions;
}

cSave::sProgress* cSave::EditProgress()
{
    // 
    return &m_Header.oProgress;
}


// ****************************************************************
// 
coreUint64 cSave::__GenerateChecksum(const sHeader& oHeader)
{
    // 
    STATIC_ASSERT(offsetof(sHeader, iChecksum) == sizeof(sHeader) - sizeof(sHeader::iChecksum))
    return coreHashMurmur64A(r_cast<const coreByte*>(&oHeader), sizeof(sHeader) - sizeof(sHeader::iChecksum));
}