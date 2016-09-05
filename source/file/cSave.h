//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SAVE_H_
#define _P1_GUARD_SAVE_H_


// ****************************************************************
// 
#define SAVE_FILE_EXTENSION "p1sv"                 // 
#define SAVE_FILE_MAGIC     (coreUint32('P1SV'))   // 
#define SAVE_FILE_VERSION   (0x00000001u)          // 


// ****************************************************************
// 
class cSave final
{


    friend FUNC_NOALIAS coreBool ValidateSave(cSave* OUTPUT pSave);
};


#endif // _P1_GUARD_SAVE_H_