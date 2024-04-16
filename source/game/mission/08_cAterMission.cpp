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
cAterMission::cAterMission()noexcept
{
    // 
    m_apBoss[0] = &m_ProjectOne;
    m_apBoss[1] = &m_Eigengrau;
    m_apBoss[2] = &m_Eigengrau;
}


// ****************************************************************
// setup the Ater mission
void cAterMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(-cDarkBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

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
        STAGE_BOSS(m_ProjectOne, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Eigengrau, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });

    // ################################################################
    // ################################################################
}