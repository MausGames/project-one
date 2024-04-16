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
cMuscusMission::cMuscusMission()noexcept
{
    // 
    m_apBoss[0] = &m_Orlac;
    m_apBoss[1] = &m_Geminga;
    m_apBoss[2] = &m_Nagual;
}


// ****************************************************************
// setup the Muscus mission
void cMuscusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(-cMossBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

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
        STAGE_BOSS(m_Orlac, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Geminga, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Nagual, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // ################################################################
}