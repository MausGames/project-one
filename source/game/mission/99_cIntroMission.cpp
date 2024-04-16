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
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateNormalShading();
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
        }

        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->StartIntro();
        }

        if(STAGE_TIME_POINT(0.6f))
        {
            g_pEnvironment->ChangeBackground(cCloudBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeed(4.0f);
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_POINT(3.0f))
        {
            g_pGame->GetInterface()->ShowStory("A game by Martin Mauersics");
        }

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.4f,-1.2f), coreVector2(0.0f,-1.0f), 2.5f);
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-0.4f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 0.6f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, (i & 0x01u) ? COLOR_SHIP_ORANGE : COLOR_SHIP_RED, true);
                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(1u, 0u)
            STAGE_GET_UINT(iBulletShot)
        STAGE_GET_END

        if(g_pGame->GetBulletManagerPlayer()->GetNumBullets())
        {
            iBulletShot = 1u;
        }
        if(STAGE_TIME_POINT(10.0f) && !iBulletShot)
        {
            g_pGame->ActivatePacifist();
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.9f, 0.219f * I_TO_F(i))

            coreSpline2* pCurPath;
            if((i < 8u) || STAGE_BRANCH(pPath2->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pCurPath = pPath2;
            }
            else
            {
                pCurPath = pPath2;
            }

            const coreVector2 vFactor = coreVector2((i & 0x02u) ? -1.0f : 1.0f, (pCurPath == pPath1) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2((i & 0x01u) ? -0.1f : 0.1f, 0.0f);

            pEnemy->DefaultMovePath(pCurPath, vFactor, vOffset * vFactor, fLifeTime);

        //    if(i < 8u) STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_POINT(3.0f))
        {
            g_pGame->GetInterface()->ShowStory("revived from the dead");
        }

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f, 1.0f),  coreVector2(1.0f, 0.0f));
            pPath1->AddStop(coreVector2( 0.8f, 0.3f),  coreVector2(1.0f, 0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 12u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(40, (i & 0x01u) ? COLOR_SHIP_ORANGE : COLOR_SHIP_RED, true);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.9f, 0.2f * I_TO_F(i))

            if(STAGE_LIFETIME_BEFORE(pPath1->GetTotalDistance()))
            {
                const coreVector2 vFactor = coreVector2((i < 6u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F(i % 6u) * -0.2f,                    (i < 6u) ? -0.1f : 0.1f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            }
            else
            {

            }

        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {

        // direction change
        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {

        // roll
        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {

        // stronger wave
        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_POINT(3.0f))
        {
            g_pGame->GetInterface()->ShowStory("Project One");
        }

        STAGE_BOSS(m_ProjectOne, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))

        if(STAGE_BEGINNING) this->ActivateWave();  
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            this->DeactivateWave();  

            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
                pPlayer->SetPosition(coreVector3(2.0f * FOREGROUND_AREA.x, 0.0f, 0.0f));
            });

            g_pGame->GetInterface()->SetVisible(false);

            g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
        }

        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateDarkShading();
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // ################################################################
}


// ****************************************************************
// 
void cIntroMission::__MoveOwnBefore()
{
}