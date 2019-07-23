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
cRutilusMission::cRutilusMission()noexcept
{
    // 
    m_apBoss[0] = &m_Quaternion;
    m_apBoss[1] = &m_Saros;
    m_apBoss[2] = &m_Messier;
}


// ****************************************************************
// setup the Rutilus mission
void cRutilusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(-cSpaceBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

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
        STAGE_BOSS(m_Quaternion)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Saros)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Messier)
    });

    // ################################################################
    // ################################################################
}