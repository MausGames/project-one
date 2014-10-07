//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CONFIG_H_
#define _P1_GUARD_CONFIG_H_


// ****************************************************************
// configuration variables
#define CONFIG_SHADOW     "Graphics", "Shadow",     (1)
#define CONFIG_REFLECTION "Graphics", "Reflection", (1)

struct cConfig
{
    int iShadow;       // shadow level
    int iReflection;   // water reflection level
};

extern cConfig g_CurConfig;   // current and active configuration structure
extern cConfig g_OldConfig;   // handle for old values


// ****************************************************************
// configuration functions
extern void LoadConfig();
extern void SaveConfig();


#endif // _P1_GUARD_CONFIG_H_