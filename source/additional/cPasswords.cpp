///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreBool g_bCheatP1 = false;


// ****************************************************************
// 
coreBool ApplyPassword(const coreChar* pcText)
{
    coreBool bRestart = false;

    // 
    ASSERT(pcText)
    const coreUint64 iHash = coreHashXXH64(pcText);

    // 
    switch(iHash)
    {
    // ZINNOBER red
    case 18123262277673915876u:
        {
            for(coreUintW i = 0u; i < SAVE_MISSIONS; ++i) g_pSave->EditProgress()->aiAdvance[i] = MAX(g_pSave->EditProgress()->aiAdvance[i], 7u);
        }
        break;

    // INDIGO blue
    case 13535575702665342035u:
        {
            std::memset(g_pSave->EditProgress()->aiFragment, 1, sizeof(g_pSave->EditProgress()->aiFragment));
        }
        break;

    // SEPIA brown
    case 4083581750562567063u:
        {
            g_pSave->EditProgress()->bFirstPlay   = true;
            g_pSave->EditProgress()->aiAdvance[0] = 0u;

            REMOVE_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_VIRIDO)
            REMOVE_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_NEVO)
            REMOVE_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_HARENA)
            REMOVE_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_RUTILUS)
            REMOVE_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_GELU)
            REMOVE_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_CALOR)
            REMOVE_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_MUSCUS)

            bRestart = true;
        }
        break;

    // SAFRAN orange
    case 12809514335711897236u:
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_MIRRORMODE)
        }
        break;

    // FUCHSIN pink
    case 17376649531488240964u:
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_GAMESPEEDUP)
        }
        break;

    // GAMBOGE yellow
    case 14976658933825851731u:
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_POWERSHIELD)
        }
        break;

    // MAUVE purple
    case 11137863330129252440u:
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_PULSE)
        }
        break;

    // JADE green
    case 2924535458287069379u:
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_WAVE)
        }
        break;

    // SELADON green
    case 2162088830537969827u:
        {
            g_bCheatP1 = true;
        }
        break;

    default:
        // password not valid
        return false;
    }

    // 
    if(bRestart)
    {
        g_pMenu->GetMsgBox()->ShowInformation(Core::Language->GetString("INFORMATION_RESTART"), []()
        {
            Core::RequestRestart();
        });
    }

    // password valid
    return true;
}