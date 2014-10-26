//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

cConfig g_CurConfig;       
cConfig g_OldConfig;   


// ****************************************************************
// load configuration
void LoadConfig()
{
    // load configuration file
    Core::Config->Load();

    // read specific values
    g_OldConfig.iShadow     = Core::Config->GetInt(CONFIG_SHADOW);
    g_OldConfig.iReflection = Core::Config->GetInt(CONFIG_REFLECTION);

    // forward values to the current structure
    g_CurConfig = g_OldConfig;
}


// ****************************************************************
// save configuration
void SaveConfig()
{
    // equalize both structures
    g_OldConfig = g_CurConfig;

    // write specific values
    Core::Config->SetInt(CONFIG_SHADOW,     g_OldConfig.iShadow);
    Core::Config->SetInt(CONFIG_REFLECTION, g_OldConfig.iReflection);

    // save configuration file
    Core::Config->Save();
}