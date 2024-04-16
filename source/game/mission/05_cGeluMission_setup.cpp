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
// setup the Gelu mission
void cGeluMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cVolcanoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Tartarus, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Phalaris, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Chol, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // ################################################################
}