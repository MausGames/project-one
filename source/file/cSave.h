///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SAVE_H_
#define _P1_GUARD_SAVE_H_

// TODO: save bak (multiple?)
// TODO: compress to make modifications harder


// ****************************************************************
// 
#define SAVE_FILE_FOLDER    "saves"                // 
#define SAVE_FILE_EXTENSION "p1sv"                 // 
#define SAVE_FILE_MAGIC     (coreUint32('P1SV'))   // 
#define SAVE_FILE_VERSION   (0x00000001u)          // 

#define SAVE_MISSIONS       (MISSIONS)             // 
#define SAVE_BOSSES         (BOSSES)               // 
#define SAVE_WAVES          (WAVES)                // 


// ****************************************************************
// 
class cSave final
{
private:
    coreBool m_aabUnlockedBoss[SAVE_MISSIONS][SAVE_BOSSES];
    coreBool m_aabUnlockedWave[SAVE_MISSIONS][SAVE_WAVES];

    coreBool m_aabParticle[SAVE_MISSIONS][SAVE_BOSSES];


public:


private:
    // 
    friend coreBool ValidateSave(cSave* OUTPUT pSave);
};


#endif // _P1_GUARD_SAVE_H_