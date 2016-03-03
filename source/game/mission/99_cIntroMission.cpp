//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cIntroMission::cIntroMission()noexcept
{
    // 
    m_apBoss[0] = &m_ProjectOne;
    m_apBoss[1] = &m_ProjectOne;
    m_apBoss[2] = &m_ProjectOne;
}


// ****************************************************************
// setup the Intro mission
void cIntroMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_INIT)
        {
            g_pEnvironment->ChangeBackground(REF_ID(cNoBackground::ID), ENVIRONMENT_MIX_FADE, 1.0f);
        }

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_INIT)
        {
            g_pGame->GetInterface()->ShowStory("What do you dream ?");
        }

        if(!g_pGame->GetInterface()->IsStoryActive(1.5f))
        {
            g_pEnvironment->ChangeBackground(REF_ID(cCloudBackground::ID), ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeed    (8.0f);

            g_pGame->StartIntro();

            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(CONTAINS_VALUE(g_pGame->GetStatus(), GAME_STATUS_PLAY))
        {
            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // 
        //STAGE_ADD_PATH(pPath)
        //{
        //    pPath->AddNode(coreVector2(-1.5f, 0.2f), coreVector2(1.0f,0.0f));
        //    pPath->AddNode(coreVector2( 1.5f,-0.4f), coreVector2(1.0f,0.0f));
        //});

        // 
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 5u)
        {
            STAGE_SQUAD_INIT(pSquad1, Configure(50, COLOR_SHIP_BLUE))
            STAGE_SQUAD_INIT(pSquad1, Resurrect())
        });
        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 5u)
        {
            STAGE_SQUAD_INIT(pSquad2, Configure(50, COLOR_SHIP_RED))
            STAGE_SQUAD_INIT(pSquad2, Resurrect())
        });

        pSquad1->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            pEnemy->DefaultMoveLerp(coreVector2(I_TO_F(i-2u) * 0.4f - 3.0f, 0.8f), coreVector2(I_TO_F(i-2u) * 0.4f, 0.8f), LERPS(0.0f, 1.0f, CLAMP(pEnemy->GetLifeTime()*0.5f - I_TO_F(i)*0.1f, 0.0f, 1.0f)));
        });

        pSquad2->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            pEnemy->DefaultMoveLerp(-coreVector2(I_TO_F(i-2u) * 0.4f - 3.0f, 0.8f), -coreVector2(I_TO_F(i-2u) * 0.4f, 0.8f), LERPS(0.0f, 1.0f, CLAMP(pEnemy->GetLifeTime()*0.5f - I_TO_F(i)*0.1f, 0.0f, 1.0f)));
        });
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // 
        if(STAGE_TIME_INIT) m_ProjectOne.Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));

        // 
        if(CONTAINS_VALUE(m_ProjectOne.GetStatus(), ENEMY_STATUS_DEAD))
            STAGE_FINISH_NOW
    });
}