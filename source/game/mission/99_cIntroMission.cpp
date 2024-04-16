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
    // simple without attacks, show group bonus
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.4f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });
        /*
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(1.2f,0.0f), coreVector2(1.0f, 0.0f));
            pPath1->Refine();
        });
        */

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.2f,0.8f), coreVector2(1.0f,0.0f));
            pPath2->AddNode(coreVector2( 1.2f,0.0f), coreVector2(1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 24u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
            });
        });

        STAGE_GET_START(1u)
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

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 17u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 18u, 23u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.22f * I_TO_F(i % 6u))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath((i < 12u) ? pPath1 : pPath2, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE
    });

    // ################################################################
    // simple without attacks, show infinity groups
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f), 0.7f);
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f), 1.2f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 12u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.22f * (I_TO_F(i % 6u) + 2.0f * I_TO_F(i / 6u)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(((i < 6u) ? 0.6f : -0.6f) + ((i % 2u) ? -0.05f : 0.05f), 0.0f); // ((i / 6u + 1u) % 3u) - 1u) * 0.6f 

            pEnemy->DefaultMovePath (pPath1, vFactor, vOffset * vFactor, fLifeTime);
            //pEnemy->DefaultOrientate(fLifeTime * 2.5f);
        });

        STAGE_WAVE
    });

    // ################################################################
    // simple with attacks
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.6f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.2f, 1.2f), coreVector2(1.0f,-1.0f).Normalized());  
            pPath2->AddNode(coreVector2( 1.2f,-1.2f), coreVector2(1.0f,-1.0f).Normalized());  
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 48u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1, 16u, 23u)
            else if(STAGE_SUB(6u)) STAGE_RESSURECT(pSquad1, 24u, 31u)
            else if(STAGE_SUB(7u)) STAGE_RESSURECT(pSquad1, 32u, 47u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.21f * I_TO_F((i < 16u) ? (i % 4u) : (i % 8u)))

            //if((i < 16u) ? ((i % 16u) < 8u) : (((i-16u) % 32u) < 16u))
            //{
            //    STAGE_REPEAT(pPath1->GetTotalDistance())
            //
            //    const coreVector2 vFactor = coreVector2(((i < 16u) ? ((i % 8u) < 4u) : ((i % 16u) < 8u)) ? -1.0f : 1.0f, 1.0f);
            //    const coreVector2 vOffset = coreVector2(0.0f,0.0f);
            //
            //    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            //}
            //else
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((i < 16u) ? ((i % 8u) < 4u) : ((i % 16u) < 8u)) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(STAGE_LIFETIME_POINT(0.2f) || STAGE_LIFETIME_POINT(0.3f) || STAGE_LIFETIME_POINT(0.4f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // heavy bullet pattern
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            //pPath1->AddNode(coreVector2(0.4f,1.4f), coreVector2(0.0f,-1.0f));
            //pPath1->AddStop(coreVector2(0.4f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 1.4f,0.5f), coreVector2(-1.0f,0.0f));
            pPath1->AddStop(coreVector2(-0.4f,0.5f), coreVector2(-1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            //pPath1->AddNode(coreVector2(0.4f,1.4f), coreVector2(0.0f,-1.0f));
            //pPath1->AddStop(coreVector2(0.4f,0.5f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 1.4f,0.5f), coreVector2(-1.0f,0.0f));
            pPath2->AddStop(coreVector2(-0.4f,0.5f), coreVector2(-1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(100, COLOR_SHIP_RED, true);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.8f, (i == 1u) ? 1.0f : 0.0f)

            if(STAGE_LIFETIME_BEFORE(pPath1->GetTotalDistance()))
            {
                const coreVector2 vFactor = coreVector2(i ? ((i % 2u) ? 1.0f : -1.0f) : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->SetDirection        (coreVector3(0.0f,-1.0f,0.0f));
                pEnemy->DefaultMovePath     (pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultOrientateLerp(0.0f, 4.0f*PI * vFactor.x, LERPB(0.0f, 1.0f, fLifeTime * RCP(pPath1->GetTotalDistance())));
            }
            else if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(s_iTick) * DEG_TO_RAD(27.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos,  vDir)->MakeWhite()->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, -vDir)->MakeWhite()->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE
    });

    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.6f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.2f, 1.2f), coreVector2(1.0f,-1.0f).Normalized());  
            pPath2->AddNode(coreVector2( 1.2f,-1.2f), coreVector2(1.0f,-1.0f).Normalized());  
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 48u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1, 16u, 23u)
            else if(STAGE_SUB(6u)) STAGE_RESSURECT(pSquad1, 24u, 31u)
            else if(STAGE_SUB(7u)) STAGE_RESSURECT(pSquad1, 32u, 47u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.21f * I_TO_F((i < 16u) ? (i % 4u) : (i % 8u)))

            //if((i < 16u) ? ((i % 16u) < 8u) : (((i-16u) % 32u) < 16u))
            //{
            //    STAGE_REPEAT(pPath1->GetTotalDistance())
            //
            //    const coreVector2 vFactor = coreVector2(((i < 16u) ? ((i % 8u) < 4u) : ((i % 16u) < 8u)) ? -1.0f : 1.0f, 1.0f);
            //    const coreVector2 vOffset = coreVector2(0.0f,0.0f);
            //
            //    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            //}
            //else
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((i < 16u) ? ((i % 8u) < 4u) : ((i % 16u) < 8u)) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(STAGE_LIFETIME_POINT(0.2f) || STAGE_LIFETIME_POINT(0.3f) || STAGE_LIFETIME_POINT(0.4f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE
    });

    // 20 bullets von selber position mit unterschiedlichem speed

    //alte sturm gruppen

    /*
    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f), 2.7f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 40u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_YELLOW, true);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.6f, 0.1f * I_TO_F(i))

            //STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((-0.1f * I_TO_F(i % 4u) - 0.1f + ((i >= 8u && i < 12) ? 0.2f : 0.0f)) * (((i / 4u) & 0x01u) ? -1.2f : 1.2f), 0.0f);

            if(STAGE_TAKEOFF)
            {
                pEnemy->SetPosition(coreVector3((vOffset + coreVector2(0.0f,1.2f)) * FOREGROUND_AREA, 0.0f));
            }

            //pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayer()->GetPosition().xy(), 40.0f, 5.0f);
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f), 2.0f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 40u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i) * RCP(I_TO_F(pSquad1->GetNumEnemies() - 1u))), true);
                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(20u)
            STAGE_GET_INT_ARRAY(aiLane, 20u,
            {
                constexpr coreInt32 x[] = {1,0,2,3,1,2,1,2,0,2,1,3,2,1,0,1,2,3,1,2};
                std::memcpy(aiLane, x, sizeof(x));
            })
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.75f, 0.4f * I_TO_F(i >> 1u) + ((i & 0x01u) ? 0.15f : 0.0f))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            coreFloat x = (I_TO_F(aiLane[i >> 1u]) * 0.667f - 1.0f) * 0.6f;
            if(i & 0x01u) x += SIGN(x) * -0.1f;

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(x, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
        });

        STAGE_WAVE
    });
    */

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_POINT(3.0f))
        {
            g_pGame->GetInterface()->ShowStory("Project One");
        }

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(100, COLOR_SHIP_RED, true);
                pEnemy->Resurrect();
            });

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3( 0.3f,1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3(-0.3f,1.2f,0.0f) * FOREGROUND_AREA3);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.5f * I_TO_F(i))

            if(STAGE_LIFETIME_AFTER(0.0f)) pEnemy->DefaultMoveForward(coreVector2(0.0f,-1.0f), 30.0f);
        });

        STAGE_BOSS(m_ProjectOne, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
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
    static int test = 0;

    if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_DEFEATED) && test == 0)
    {
        
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateDarkShading();
        });

        g_pGame->GetInterface()->ShowStory("Project One");

        //g_pGame->StartOutro();

        g_pPostProcessing->SetValue(0.0f);

        test = 1;
    }



    //if(CONTAINS_FLAG(g_pGame->GetPlayer(0u)->GetStatus(), PLAYER_STATUS_DEAD)) return;
    //
    //g_pEnvironment->SetTargetDirection(
    //coreVector2(g_pGame->GetPlayer(0u)->GetPosition().x / FOREGROUND_AREA.x * 0.2f, 1.0f).Normalized()
    //);
}