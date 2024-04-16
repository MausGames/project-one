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
            g_pGame->GetInterface()->ShowStory("And so I found you ...", "(demo version)");
        }

        STAGE_FINISH_AFTER(5.0f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cCloudBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeed(5.0f);

            g_pGame->StartIntro();
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(-0.8f, 1.2f), coreVector2(0.0f,-1.0f) * 1.0f);
            pPath1->AddNode(coreVector2(-0.4f,-1.2f), coreVector2(0.0f,-1.0f) * 2.5f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 8u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, ((i & 0x04u) ? 0.667f : 0.0f) + I_TO_F(i & 0x01u) * 0.333f));
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.9f, 0.2f * I_TO_F(i))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2((i & 0x02u) ? -1.0f : 1.0f, -1.0f);
            const coreVector2 vOffset = coreVector2((i & 0x01u) ? -0.1f : 0.1f,  0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f) * 2.5f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i & 0x03u) * 0.333f));
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.75f, 0.2f * I_TO_F(i))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.025f * I_TO_F(MIN(i + 8u, pSquad1->GetNumEnemies() - 1u - i + 4u)) * ((i & 0x04u) ? -1.0f : 1.0f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f) * 1.0f);
            pPath1->AddStop(coreVector2(0.0f, 0.9f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f) * 1.5f);
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f) * 1.0f);
            pPath2->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f) * 1.5f);
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(60, COLOR_SHIP_GREEN);
                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(5, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i) * RCP(I_TO_F(pSquad2->GetNumEnemies() - 1u))));
                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(1u, 1u)
        STAGE_GET_INT  (iSide)
        STAGE_GET_FLOAT(fStop, fStop = pPath1->TranslateDistance(1u, 0.0f))
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.75f, 0.0f)

            //STAGE_DELAY_ADD(fStop, 5.0f)
            {
                STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(i ? -0.5f : 0.5f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
            }
            //STAGE_DELAY_RESET

            const coreVector2 vPos = pEnemy->GetPosition ().xy();
            const coreVector2 vDir = pEnemy->GetDirection().xy();
            const coreVector2 vTan = vDir.Rotated90();

            if(STAGE_LIFETIME_BETWEEN(0.2f, fStop + 5.0f) && STAGE_TICK(30u))
            {
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, pEnemy, vPos + vTan * (CONTAINS_BIT(iSide, i) ? -3.0f : 3.0f), vDir)->MakeGreen();
                TOGGLE_BIT(iSide, i)
            }
            else if(STAGE_LIFETIME_AFTER(fStop + 5.0f) && STAGE_TICK(15u))
            {
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, pEnemy, vPos,  vTan)->MakeGreen();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, pEnemy, vPos, -vTan)->MakeGreen();
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.75f, 0.2f * I_TO_F(i) + 1.5f)

            STAGE_REMOVE_LIFETIME(pEnemy, pPath2->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i & 0x01u) ? -0.15f : 0.15f, 0.0f);

            pEnemy->DefaultMovePath(pPath2, vFactor, vOffset, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    //STAGE_START_HERE
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f) * 2.5f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i % 4u) * 0.25f));
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.6f, 0.1f * I_TO_F(i))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((-0.1f * I_TO_F(i % 4u) - 0.1f + ((i >= 8u && i < 12) ? 0.2f : 0.0f)) * (((i / 4u) & 0x01u) ? -1.2f : 1.2f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f) * 2.5f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 40u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i) * RCP(I_TO_F(pSquad1->GetNumEnemies() - 1u))));
                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(20u, 0u)
        STAGE_GET_INT_ARRAY(aiLane, 20u,
        {
            constexpr coreInt16 x[] = {1,0,2,3,1,2,1,2,0,2,1,3,2,1,0,1,2,3,1,2};
            std::memcpy(aiLane, x, sizeof(x));
        })
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.75f, 0.4f * I_TO_F(i >> 1u) + ((i & 0x01u) ? 0.15f : 0.0f))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            coreFloat x = (aiLane[i >> 1u] * 0.667f - 1.0f) * 0.5f;
            if(i & 0x01u) x += SIGN(x) * -0.1f;

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(x, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // show clean-bonus, by making it easy to kill everything 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(-1.2f, 1.0f), coreVector2(1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.0f, 0.3f), coreVector2(1.0f,-1.0f).Normalized() * 2.0f);
            pPath1->AddNode(coreVector2( 1.2f,-0.4f), coreVector2(1.0f, 0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 12u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i) * RCP(I_TO_F(pSquad1->GetNumEnemies() - 1u))));
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.2f * I_TO_F(i))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i > 5u) & 0x01u) ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, -0.1f * I_TO_F(i & 0x03u));

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_ProjectOne, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });
}


// ****************************************************************
// 
void cIntroMission::__MoveOwnBefore()
{
    if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY) || CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_INTRO))
    {
  //      g_pPostProcessing->SetSaturation(CLAMP((g_pGame->GetTimeMission() - 4.5f) * 0.25f, 0.0f, 1.0f));      // 6.0f
       // g_pPostProcessing->SetSaturation(CLAMP((g_pGame->GetTimeMission() - 4.5f) * 0.1f, 0.0f, 1.0f));      // 6.0f
        // add shadow blocks    
    }
}