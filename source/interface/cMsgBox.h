//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MSGBOX_H_
#define _P1_GUARD_MSGBOX_H_

// TODO: transfer to Core Engine


// ****************************************************************
// 
class cMsgBox final : public coreObject2D
{
public:
    cMsgBox()noexcept;

    DISABLE_COPY(cMsgBox)
};


#endif // _P1_GUARD_MSGBOX_H_