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
// setup the Intro mission
void cIntroMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateNormalShading();
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->StartIntro();
        }

        if(STAGE_TIME_POINT(0.6f))
        {
            g_pEnvironment->ChangeBackground(cCloudBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeed(4.0f);   // TODO: set earlier, because of acceleration delay   
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // simple without attacks, show scoring
    // - 5: from different side than previous group, to not fly into players
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.4f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.2f,0.8f), coreVector2(1.0f,0.0f));
            pPath2->AddNode(coreVector2( 1.2f,0.0f), coreVector2(1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iBulletShot)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 17u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 18u, 23u)
            else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1, 24u, 29u)
            else if(STAGE_SUB(6u)) STAGE_RESSURECT(pSquad1, 30u, 35u)
        }

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
            STAGE_LIFETIME(pEnemy, 1.2f, 0.22f * I_TO_F(i % 6u))

            const coreSpline2* pPath = ((i % 24u) < 12u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? -1.0f : 1.0f, (i < 24u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2((i >= 24u) ? -0.2f : 0.0f, 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // simple without attacks, show wave gameplay
    // - 2: enemies cannot be killed in one run by shooting upwards, so they may fly another run
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f), 0.7f);
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f), 1.2f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 30u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 11u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 12u, 29u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.1f, 0.22f * (I_TO_F(i % 6u) + 2.0f * I_TO_F((i % ((i < 12u) ? 12u : 18u)) / 6u)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);//coreVector2(1.0f, (i < 24u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2((((i % 12u) < 6u) ? 1.0f : -1.0f) * ((i < 12u) ? 0.4f : ((i < 24u) ? 0.8f : 0.0f)) + ((i % 2u) ? -0.06f : 0.06f) * SIN(1.0f*PI * fLifeTime), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // simple with attacks
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f, 0.8f), coreVector2(1.2f,-0.8f).Normalized());
            pPath1->AddNode(coreVector2( 1.2f,-0.8f), coreVector2(1.2f,-0.8f).Normalized());
            pPath1->Refine();
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
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 16u, 23u)
            else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1, 24u, 31u)
            else if(STAGE_SUB(6u)) STAGE_RESSURECT(pSquad1, 32u, 47u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.21f * I_TO_F((i < 16u) ? (i % 4u) : (i % 8u)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i < 16u) ? ((i % 8u) < 4u) : ((i % 16u) < 8u)) ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.1f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_LIFETIME(5.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual((i < 16u) ? ((i / 4u) % 2u) : ((i / 8u) % 2u)).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.6f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // simple with heavy attacks
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(1.4f,0.5f), coreVector2(-1.0f,0.0f));
            pPath1->AddStop(coreVector2(0.0f,0.5f), coreVector2(-1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-0.4f,1.4f), coreVector2(0.0f,-1.0f), 0.9f/1.4f);
            pPath2->AddStop(coreVector2(-0.4f,0.5f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(120, COLOR_SHIP_RED, true);
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

            const coreSpline2* pPath = i ? pPath2 : pPath1;

            if(STAGE_LIFETIME_BEFORE(pPath->GetTotalDistance()))
            {
                const coreVector2 vFactor = coreVector2((i % 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath     (pPath, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection        (coreVector3(0.0f,-1.0f,0.0f));
                pEnemy->DefaultOrientateLerp(0.0f, 4.0f*PI * vFactor.x, LERPB(0.0f, 1.0f, fLifeTime * RCP(pPath->GetTotalDistance())));
            }
            else if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(s_iTick) * DEG_TO_RAD(27.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos,  vDir)->MakeWhite()->ChangeSize(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, -vDir)->MakeWhite()->ChangeSize(1.4f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // movement training
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_RED, true);
            });
        });

        if(STAGE_CLEARED)
        {
            const coreUint8 iFrom = m_iStageSub;
            const coreUint8 iTo   = m_iStageSub + ((iFrom == 8u) ? 1u : 0u);
            if((iTo < pSquad1->GetNumEnemies()) && STAGE_SUB(iTo + 1u)) STAGE_RESSURECT(pSquad1, iFrom, iTo)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.9f, 0.0f)

            STAGE_REPEAT(pPath1->GetTotalDistance())

            coreFloat fSide;
            switch(i)
            {
            default: ASSERT(false)
            case 0u: fSide =  0.2f; break;
            case 1u: fSide =  0.6f; break;
            case 2u: fSide = -0.2f; break;
            case 3u: fSide = -0.6f; break;
            case 4u: fSide =  0.6f; break;
            case 5u: fSide = -0.6f; break;
            case 6u: fSide =  0.2f; break;
            case 7u: fSide = -0.2f; break;
            case 8u: fSide =  0.2f; break;
            case 9u: fSide = -0.2f; break;
            }

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(fSide, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_TIME(12.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, pEnemy, vPos,  vDir)->MakeWhite()->ChangeSize(1.2f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, pEnemy, vPos, -vDir)->MakeWhite()->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->StartOutro(2u);
        }

        STAGE_BOSS(m_ProjectOne, {1.0f, 2.0f, 3.0, 4.0f})
    });

    // ################################################################
    // ################################################################
}