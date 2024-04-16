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
    // GAMBOGE yellow
    case 14976658933825851731u:
        {
            for(coreUintW i = 0u; i < SAVE_MISSIONS; ++i) g_pSave->EditProgress()->aiAdvance[i] = MAX(g_pSave->EditProgress()->aiAdvance[i], 1u);
        }
        break;

    // SAFFRON orange
    case 4606173178869511954u:
        {
            for(coreUintW i = 0u; i < SAVE_MISSIONS; ++i) g_pSave->EditProgress()->aiAdvance[i] = MAX(g_pSave->EditProgress()->aiAdvance[i], 7u);
        }
        break;

    // FUCHSIA pink
    case 8219416175255162026u:
        {
            std::memset(g_pSave->EditProgress()->aiFragment, 1, sizeof(g_pSave->EditProgress()->aiFragment));
        }
        break;

    // ORCHIL purple
    case 16681166332002509082u:
        {
            g_pSave->EditProgress()->bFirstPlay   = true;
            g_pSave->EditProgress()->aiAdvance[0] = 0u;
            bRestart = true;
        }
        break;

    // INDIGO blue
    case 13535575702665342035u:
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_MIRRORMODE)
        }
        break;

    // SEPIA brown
    case 4083581750562567063u:
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_GAMESPEEDUP)
        }
        break;

    // SCARLET red
    case 4594381807352121855u:
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

    // CELADON green
    case 9262066068920926272u:
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