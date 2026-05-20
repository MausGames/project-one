///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_PASSWORDS_H_
#define _P1_GUARD_PASSWORDS_H_


// ****************************************************************
// 
enum eCheatRender : coreUint8
{
    CHEAT_RENDER_DEFAULT = 0u,
    CHEAT_RENDER_PIXEL   = 1u,
    CHEAT_RENDER_RETRO   = 2u,
    CHEAT_RENDER_SEPIA   = 3u,
};


// ****************************************************************
// 
extern coreBool     g_bCheatP1;         // 
extern eCheatRender g_eCheatRender;     // 
extern coreBool     g_bCheatOverdraw;   // 


// ****************************************************************
// 
extern coreBool ApplyPassword    (const coreChar* pcText);
extern void     ChangeRenderStyle(const eCheatRender eRender, const coreBool bOverdraw);


#endif // _P1_GUARD_PASSWORDS_H_