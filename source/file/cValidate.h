//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_VALIDATE_H_
#define _P1_GUARD_VALIDATE_H_


// ****************************************************************
// validation disabled
inline FUNC_NOALIAS coreBool ValidateReplay(cReplay* OUTPUT pReplay) {return true;}
inline FUNC_NOALIAS coreBool ValidateSave  (cSave*   OUTPUT pSave)   {return true;}


#endif // _P1_GUARD_VALIDATE_H_