//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_INTERFACE_H_
#define _P1_GUARD_INTERFACE_H_


// ****************************************************************
// 
class cInterface final : public coreMenu
{
private:
    coreLabel m_Health;   // 


public:
    cInterface()noexcept;

    DISABLE_COPY(cInterface)

    // 
    void Move()override;
};


#endif // _P1_GUARD_INTERFACE_H_