///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// setup the Ater mission
void cAterMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            if(pPlayer->IsEnemyLook()) pPlayer->TurnIntoPlayer();
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_INTRO)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pEnvironment->ChangeBackground(cDarkBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        g_pEnvironment->SetTargetSpeedNow(4.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->GetInterface()->ShowMission(this);
        }

        STAGE_FINISH_AFTER(MISSION_WAIT_PLAY)
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_BOSS(m_ProjectOne, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 7u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 7u})
    //{
    //    STAGE_BOSS(m_Eigengrau, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}