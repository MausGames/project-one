///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// setup the Demo mission
void cDemoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->StartIntro();
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // ################################################################
}