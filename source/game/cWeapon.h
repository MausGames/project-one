//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WEAPON_H_
#define _P1_GUARD_WEAPON_H_


// ****************************************************************
// 
class INTERFACE cWeapon
{
private:


public:
    cWeapon()noexcept;
    virtual ~cWeapon();


private:
    DISABLE_COPY(cWeapon)
};


#endif // _P1_GUARD_WEAPON_H_