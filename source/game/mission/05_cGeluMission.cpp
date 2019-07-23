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
// constructor
cGeluMission::cGeluMission()noexcept
{
    // 
    m_apBoss[0] = &m_Tartarus;
    m_apBoss[1] = &m_Phalaris;
    m_apBoss[2] = &m_Chol;
}


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
            g_pEnvironment->ChangeBackground(-cVolcanoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

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
        STAGE_BOSS(m_Tartarus)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Phalaris)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Chol)
    });

    // ################################################################
    // ################################################################
}