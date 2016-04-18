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
            g_pEnvironment->ChangeBackground(REF_ID(cNoBackground::ID), ENVIRONMENT_MIX_FADE, 1.0f);
        }

        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->GetInterface()->ShowStory("What do you dream ?");
        }

        if(!g_pGame->GetInterface()->IsStoryActive(2.5f))
        {
            g_pEnvironment->ChangeBackground(REF_ID(cCloudBackground::ID), ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeed  (8.0f);

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
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddStop(coreVector2(-1.5f, 0.95f));
            pPath1->AddNode(coreVector2( 0.0f, 0.95f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->AddNode(coreVector2( 0.0f, 0.95f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath2->AddStop(coreVector2( 0.95f, 0.95f));
            pPath2->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f,-1.0f).Normalize() * 2.0f);
            pPath2->AddStop(coreVector2(-0.95f,-0.95f));
            pPath2->AddNode(coreVector2( 0.0f,-0.95f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath2->AddStop(coreVector2( 0.95f,-0.95f));
            pPath2->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f, 1.0f).Normalize() * 2.0f);
            pPath2->AddStop(coreVector2(-0.95f, 0.95f));
            pPath2->AddLoop();
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(60, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i) / I_TO_F(pSquad1->GetNumEnemies() - 1u)));
                pEnemy->Resurrect();
            });

            pSquad1->GetEnemy(0u)->GiveShield(ELEMENT_YELLOW);
        });

        STAGE_GET_START(1u, 0.0f)
        STAGE_GET_INT(iSecondRun)

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const coreFloat fSide = iSecondRun ? -1.0f : 1.0f;

            STAGE_LIFETIME(pEnemy, 0.6f, -0.2f*I_TO_F(i))

            if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pEnemy->DefaultMovePath(pPath1, coreVector2(fSide, 1.0f), coreVector2(0.0f,0.0f), fLifeTime);
            }
            else
            {
                pEnemy->DefaultMovePath(pPath2, coreVector2(fSide, 1.0f), coreVector2(0.0f,0.0f), fLifeTime);
            }

            if(STAGE_POSITION_POINT(pEnemy, -0.7f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.5f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.3f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.1f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.1f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.3f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.5f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.7f * FOREGROUND_AREA.x, x))
            {
                const coreVector2& vPos = vPositionPoint;
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos,                          coreVector2(0.0f,-SIGN(vPos.y)))->MakeBlue();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos + coreVector2(0.0f,2.5f), coreVector2(0.0f,-SIGN(vPos.y)))->MakeBlue();
            }

            if(STAGE_POSITION_POINT(pEnemy, -0.7f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy, -0.5f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy, -0.3f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy, -0.1f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.1f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.3f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.5f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.7f * FOREGROUND_AREA.y, y))
            {
                const coreVector2& vPos = vPositionPoint;
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos,                          coreVector2(-SIGN(vPos.x),0.0f))->MakeBlue();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos + coreVector2(2.5f,0.0f), coreVector2(-SIGN(vPos.x),0.0f))->MakeBlue();
            }

            if(STAGE_HEALTHPCT_POINT(pEnemy, 0.50f) || STAGE_DIED(pEnemy))
            {
                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(),  vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(),  vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(), -vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(), -vDir)->MakeGreen();
                }

                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
            }
        });

        if(pSquad1->IsFinished())
        {
            if(iSecondRun++) STAGE_FINISH_NOW
            else
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
                {
                    pEnemy->Resurrect();
                });

                pSquad1->GetEnemy(0u)->GiveShield(ELEMENT_YELLOW);
            }
        }
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddStop(coreVector2(-1.5f, 0.95f));
            pPath1->AddNode(coreVector2( 0.0f, 0.95f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->AddNode(coreVector2( 0.0f, 0.95f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath2->AddStop(coreVector2( 0.95f, 0.95f));
            pPath2->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f,-1.0f).Normalize() * 2.0f);
            pPath2->AddStop(coreVector2(-0.95f,-0.95f));
            pPath2->AddNode(coreVector2( 0.0f,-0.95f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath2->AddStop(coreVector2( 0.95f,-0.95f));
            pPath2->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f, 1.0f).Normalize() * 2.0f);
            pPath2->AddStop(coreVector2(-0.95f, 0.95f));
            pPath2->AddLoop();
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(60, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i) / I_TO_F(pSquad1->GetNumEnemies() - 1u)));
                pEnemy->Resurrect();
            });

            pSquad1->GetEnemy(0u)->GiveShield(ELEMENT_YELLOW);
        });

        STAGE_GET_START(1u, 0.0f)
        STAGE_GET_INT(iSecondRun)

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const coreFloat fSide = iSecondRun ? -1.0f : 1.0f;

            STAGE_LIFETIME(pEnemy, 0.6f, -0.2f*I_TO_F(i))

            if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pEnemy->DefaultMovePath(pPath1, coreVector2(fSide, 1.0f), coreVector2(0.0f,0.0f), fLifeTime);
            }
            else
            {
                pEnemy->DefaultMovePath(pPath2, coreVector2(fSide, 1.0f), coreVector2(0.0f,0.0f), fLifeTime);
            }

            if(STAGE_POSITION_POINT(pEnemy, -0.7f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.5f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.3f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.1f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.1f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.3f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.5f * FOREGROUND_AREA.x, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.7f * FOREGROUND_AREA.x, x))
            {
                const coreVector2& vPos = vPositionPoint;
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos,                          coreVector2(0.0f,-SIGN(vPos.y)))->MakeBlue();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos + coreVector2(0.0f,2.5f), coreVector2(0.0f,-SIGN(vPos.y)))->MakeBlue();
            }

            if(STAGE_POSITION_POINT(pEnemy, -0.7f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy, -0.5f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy, -0.3f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy, -0.1f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.1f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.3f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.5f * FOREGROUND_AREA.y, y) ||
               STAGE_POSITION_POINT(pEnemy,  0.7f * FOREGROUND_AREA.y, y))
            {
                const coreVector2& vPos = vPositionPoint;
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos,                          coreVector2(-SIGN(vPos.x),0.0f))->MakeBlue();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos + coreVector2(2.5f,0.0f), coreVector2(-SIGN(vPos.x),0.0f))->MakeBlue();
            }

            if(STAGE_HEALTHPCT_POINT(pEnemy, 0.50f) || STAGE_DIED(pEnemy))
            {
                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(),  vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(),  vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(), -vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(), -vDir)->MakeGreen();
                }

                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
            }
        });

        if(pSquad1->IsFinished())
        {
            if(iSecondRun++) STAGE_FINISH_NOW
            else
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
                {
                    pEnemy->Resurrect();
                });

                pSquad1->GetEnemy(0u)->GiveShield(ELEMENT_YELLOW);
            }
        }
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(-1.5f,0.95f), coreVector2(1.0f,0.0f));
            pPath1->AddNode(coreVector2( 1.5f,0.95f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(60, COLOR_SHIP_YELLOW);
                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(60, COLOR_SHIP_GREEN);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, pPath1->GetTotalDistance() * (I_TO_F(i) / -5.0f))

            if(STAGE_BRANCH(0.0f, pPath1->GetTotalDistance())) {}

            pEnemy->DefaultMovePath(pPath1, coreVector2(1.0f,1.0f), coreVector2(0.0f,0.0f), fLifeTime);

            const coreFloat fOffest = I_TO_F(i) * 0.08f * FOREGROUND_AREA.x;
            if(STAGE_POSITION_POINT(pEnemy, -1.3f * FOREGROUND_AREA.x - fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.9f * FOREGROUND_AREA.x - fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.5f * FOREGROUND_AREA.x - fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.1f * FOREGROUND_AREA.x - fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.3f * FOREGROUND_AREA.x - fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.7f * FOREGROUND_AREA.x - fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy,  1.1f * FOREGROUND_AREA.x - fOffest, x))
            {
                const coreVector2& vPos = vPositionPoint;
                const coreVector2  vDir = coreVector2(0.0f, -SIGN(vPositionPoint.y));
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.1f, pEnemy, vPos, vDir)->MakeYellow();
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, pPath1->GetTotalDistance() * (I_TO_F(i) / -5.0f))

            if(STAGE_BRANCH(0.0f, pPath1->GetTotalDistance())) {}

            pEnemy->DefaultMovePath(pPath1, coreVector2(-1.0f,-1.0f), coreVector2(0.0f,0.0f), fLifeTime);

            const coreFloat fOffest = I_TO_F(i) * 0.08f * FOREGROUND_AREA.x;
            if(STAGE_POSITION_POINT(pEnemy,  1.3f * FOREGROUND_AREA.x + fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.9f * FOREGROUND_AREA.x + fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.5f * FOREGROUND_AREA.x + fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy,  0.1f * FOREGROUND_AREA.x + fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.3f * FOREGROUND_AREA.x + fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy, -0.7f * FOREGROUND_AREA.x + fOffest, x) ||
               STAGE_POSITION_POINT(pEnemy, -1.1f * FOREGROUND_AREA.x + fOffest, x))
            {
                const coreVector2& vPos = vPositionPoint;
                const coreVector2& vDir = coreVector2(0.0f, -SIGN(vPositionPoint.y));
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.1f, pEnemy, vPos, vDir)->MakeGreen();
            }
        });
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // 
        if(STAGE_BEGINNING) m_ProjectOne.Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));

        // 
        if(CONTAINS_VALUE(m_ProjectOne.GetStatus(), ENEMY_STATUS_DEAD))
            STAGE_FINISH_NOW
    });
}