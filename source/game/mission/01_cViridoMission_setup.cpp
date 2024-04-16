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
// setup the Virido mission
void cViridoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateDarkShading();
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
        g_pEnvironment->ChangeBackground(cGrassBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        g_pEnvironment->SetTargetSpeed(4.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        g_pGame->GetInterface()->ShowMission(this);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u, 2u, 3u, 4u, 5u})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeightNow(0.4583f, -13.83f);
            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(1u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(2u, 0.0f);
            g_pEnvironment->GetBackground()->SetDecalDensity (0u, 0.0f);
            g_pEnvironment->GetBackground()->SetAirDensity   (0u, 0.0f);
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // enemies survive invincible
    // soll man schaffen ohne drehen und rollen
    // gegner-angriffe sollen noch nicht gezielt sein (auch wenn es im intro schon war)
    // bewegungen wie bei alter erster gruppe für mehr dynamic, 3 reihen links rechts links, frei aufgeteilt oder kleine gruppe
    // gegner sind in gruppen unverwundbar und sterben gemeinsam, nicht sub-stages
    // 
    // 2 3er
    // 4 2er, abstand übereinander
    // street middle
    // 3er links rechts ???
    // 1er links rechts
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.0f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 38u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.35f);
                pEnemy->Configure(4, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        constexpr coreUintW aaiPair[][2] = {{ 0u,  1u}, { 2u,  3u}, { 4u,  5u},
                                            { 6u,  8u}, { 7u,  9u}, {10u, 12u}, {11u, 13u},
                                            {14u, 15u}, {16u, 17u}, {18u, 19u}, {20u, 21u},
                                            {22u, 24u}, {23u, 28u}, {25u, 27u}, {26u, 33u},
                                            {29u, 30u}, {31u, 36u}, {32u, 34u}, {35u, 37u}};

        for(coreUintW i = 0u; i < ARRAY_SIZE(aaiPair); ++i)
        {
            cEnemy* A = pSquad1->GetEnemy(aaiPair[i][0]);
            cEnemy* B = pSquad1->GetEnemy(aaiPair[i][1]);

            if((A->GetCurHealth() == 0) && (B->GetCurHealth() == 0))
            {
                A->Kill(true);
                B->Kill(true);
            }
        }

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 13u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 14u, 21u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 22u, 37u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.0f)

            const coreFloat x1 = (i < 22u) ? (I_TO_F(i % 2u) * 0.25f - 0.125f) : (I_TO_F((i - 22u) % 4u) * 0.25f - 0.375f);
            const coreFloat x2 = (i < 6u) ? (I_TO_F(i / 2u) * 0.5f - 0.5f) : ((i < 14u) ? (I_TO_F((i - 6u) / 4u) * 1.0f - 0.5f) : 0.0f);
            const coreFloat y  = (i < 14u) ? (((i / 2u) % 2u) ? 0.8f : 0.0f) : ((i < 22u) ? (I_TO_F((i - 14u) / 2u) * 0.25f) : (I_TO_F((i - 22u) / 4u) * 0.25f));

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(x1 + x2, y);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            pEnemy->SetDirection   (coreVector3(0.0f,-1.0f,0.0f));

            if((i < 24u) || STAGE_LIFETIME_BEFORE(1.2f))
            {
                if(STAGE_TICK_LIFETIME(0.7f, (i % 2u) ? 0.5f : 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);
                    const coreVector2 vTan = vDir.Rotated90() * 1.5f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.25f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.25f);
                }
            }
        });

        STAGE_WAVE("EINS", {20.0f, 30.0f, 40.0f, 50.0f})
    });
/*
    // ################################################################
    // top, bottom, side groups creating bullet carpet
    // - all: bullet patterns provide a lot of safe space, so players have time to think
    // - 2: push players consistently away into safe space, to reduce confusion when trying to fly into or under enemies
    STAGE_MAIN({TAKE_ALWAYS, u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.9f), coreVector2(1.0f,0.0f));
            pPath1->AddNode(coreVector2( 1.2f,0.9f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.9f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.9f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_BLUE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 23u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 24u, 35u)
        }

        if((m_iStageSub == 2u) && (pSquad1->GetNumEnemiesAlive() == 6u))
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->GetPosition().y < 0.0f)
                {
                    const coreVector2 vForce = pPlayer->GetForce();
                    if(!vForce.IsNull()) pPlayer->SetForce(coreVector2(0.0f,1.0f) * vForce.Length());
                }
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.4f * I_TO_F(i % 6u))

            const coreSpline2* pPath = (i < 12u || i >= 24u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? 1.0f : -1.0f, (i >= 6u && i < 12u) ? -1.0f : ((i < 24u) ? 1.0f : 0.0f));
            const coreVector2 vOffset = coreVector2(0.0f, (i >= 6u && i < 12u) ? -0.1f : ((i < 24u) ? 0.0f : ((i < 30u) ? 0.8f : 0.9f)));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);   // TODO: vOffset * vFactor

            if((i < 24u) || STAGE_LIFETIME_BEFORE(1.2f))
            {
                if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.25f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.25f);
                }
            }
        });

        STAGE_WAVE("EINS", {20.0f, 30.0f, 40.0f, 50.0f})
    });
*/
    // ################################################################
    // shields on sides to force attacks from certain directions
    // - 3: starts with offset, to not fly into players
    // - 4: reflect all bullets and players with force, except with barrel roll
    // - 5: arranged to allow only one kill per turn, and to improve coop gameplay
    // TODO: barriers have cut-off outline in maze-group   
    // TODO: bullets (even single, like pulse) should be reflected with offset to highlight reflection (not 180 degree)
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.5f,1.4f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(-0.5f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.4f + 0.38f * 1.0f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.0f + 0.38f * 0.5f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 18u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_CYAN);
            });
        });

        if((m_iStageSub == 4u) && (pSquad1->GetNumEnemiesAlive() == 6u))
        {
            Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_VIRIDO_BARRIER, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pBarrier, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                pBullet->SetFlyDir(pBarrier->GetDirection().xy());
            });
        }

        for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
        {
            if(m_apBarrierOwner[i] && CONTAINS_FLAG(m_apBarrierOwner[i]->GetStatus(), ENEMY_STATUS_DEAD))
                this->DisableBarrier(i, true);
        }

        if(STAGE_CLEARED)
        {
            if(STAGE_SUB(1u))
            {
                STAGE_RESSURECT(pSquad1, 0u, 1u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy( 1u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy( 1u), coreVector2( 0.0f,-1.0f), 1.0f);
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESSURECT(pSquad1, 2u, 3u)
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 2u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy( 2u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy( 2u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy( 3u), coreVector2( 1.0f, 0.0f), 1.0f);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESSURECT(pSquad1, 4u, 7u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy( 5u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy( 5u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 4u, pSquad1->GetEnemy( 6u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 5u, pSquad1->GetEnemy( 6u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy( 7u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy( 7u), coreVector2(-1.0f, 0.0f), 1.0f);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESSURECT(pSquad1, 8u, 13u)
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 8u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy( 9u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy(10u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy(11u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy(12u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy(13u), coreVector2( 0.0f,-1.0f), 1.0f);
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESSURECT(pSquad1, 14u, 17u)
                this->EnableBarrier( 4u, pSquad1->GetEnemy(14u), coreVector2(-1.0f, 0.0f), 1.0f);   // # render order
                this->EnableBarrier( 1u, pSquad1->GetEnemy(15u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy(15u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy(16u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 0u, pSquad1->GetEnemy(16u), coreVector2( 0.0f,-1.0f), 1.0f);   // # render order
                this->EnableBarrier( 5u, pSquad1->GetEnemy(16u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy(17u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy(17u), coreVector2( 1.0f, 0.0f), 1.0f);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, (i >= 8u && i < 14u) ? (I_TO_F(i-8u) * 0.15f) : 0.0f)

            const coreSpline2* pPath = (i >= 14u) ? pPath2 : pPath1;

            const coreVector2 vFactor = coreVector2((i >= 8u) ? 0.0f : ((i <= 1u || i >= 4u) ? ((i % 2u) ? 0.5f : -0.5f) : -1.0f), (i <= 1u || i >= 8u) ? ((i >= 14u) ? -1.0f : 1.0f) : ((i % 2u) ? 1.0f : -1.0f));
            const coreVector2 vOffset = coreVector2((i >= 8u) ? ((i >= 14u) ? ((I_TO_F((i-14u) % 2u) - 0.5f) * 0.38f) : ((I_TO_F(i-8u) - 2.5f) * 0.38f)) : 0.0f, (i >= 14u) ? (I_TO_F((i-14u) / 2u) * 0.38f) : 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);   // TODO: vOffset * vFactor

            if(i == 2u || i == 3u || i == 4u || i == 5u) pEnemy->Rotate90();

            if(STAGE_LIFETIME_AFTER(0.7f) && STAGE_TICK_LIFETIME(1.0f, 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition().xy();
                const coreFloat   fBase = ((i >= 8u && i < 14u) ? pEnemy->GetDirection().xy() : pEnemy->AimAtPlayerSide()).Angle();

                for(coreUintW j = 3u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 1.0f) * 4.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        });

        STAGE_WAVE("ZWEI", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all barriers
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            this->DisableBarrier(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // cut with laser through everything
    // the delay when rays "appear" is important, not when they "stop" 
    // rays should not come from below, first segments main direction is upwards 
    // 1, 2 coming from upper left, 3, 4 coming from upper right 
    // 1 starts right to left, 2 starts left to right, 3 starts right to left, 4 starts left to right 
    // enemies on the line spawn in the middle of the line 
    // first two patterns have no single equal line 
    // no intro-crossing in first two patterns 
    // TODO: fix ASSERT for getting the laser  
    // TODO: purple helper visible at border of one laser
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 15u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.25f,1.25f,1.25f));
                pEnemy->Configure(30, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_GET_START(15u)
            STAGE_GET_FLOAT_ARRAY(afDistance, 15u)
        STAGE_GET_END

        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
        {
            if(m_apLaserOwner[i] && CONTAINS_FLAG(m_apLaserOwner[i]->GetStatus(), ENEMY_STATUS_DEAD))
                this->DisableLaser(i, true);
        }

        if(STAGE_CLEARED)
        {
            if(STAGE_SUB(1u))
            {
                STAGE_RESSURECT(pSquad1, 0u, 2u)
                this->EnableLaser(0u, pSquad1->GetEnemy( 0u));
                this->EnableLaser(1u, pSquad1->GetEnemy( 1u));
                this->EnableLaser(2u, pSquad1->GetEnemy( 2u));
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESSURECT(pSquad1, 3u, 6u)
                this->EnableLaser(3u, pSquad1->GetEnemy( 3u));
                this->EnableLaser(0u, pSquad1->GetEnemy( 4u));
                this->EnableLaser(1u, pSquad1->GetEnemy( 5u));
                this->EnableLaser(2u, pSquad1->GetEnemy( 6u));
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESSURECT(pSquad1, 7u, 10u)
                this->EnableLaser(3u, pSquad1->GetEnemy( 7u));
                this->EnableLaser(0u, pSquad1->GetEnemy( 8u));
                this->EnableLaser(1u, pSquad1->GetEnemy( 9u));
                this->EnableLaser(2u, pSquad1->GetEnemy(10u));
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESSURECT(pSquad1, 11u, 14u)
                this->EnableLaser(3u, pSquad1->GetEnemy(11u));
                this->EnableLaser(0u, pSquad1->GetEnemy(12u));
                this->EnableLaser(1u, pSquad1->GetEnemy(13u));
                this->EnableLaser(2u, pSquad1->GetEnemy(14u));
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vBasePos, vBaseDir;
            coreFloat   fDelay, fShift, fSide;
            switch(i)
            {
            default: ASSERT(false)
            case  0u: vBasePos = coreVector2( 0.7f,-0.1f); vBaseDir = coreVector2(-2.0f,-1.0f).Normalized(); fDelay = 0.0f;  fShift =   0.0f; fSide = -1.0f; break;
            case  1u: vBasePos = coreVector2( 0.2f,-0.7f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.3f;  fShift = -15.0f; fSide =  1.0f; break;
            case  2u: vBasePos = coreVector2( 0.0f, 0.8f); vBaseDir = coreVector2( 1.0f, 1.0f).Normalized(); fDelay = 0.45f; fShift = -35.0f; fSide =  1.0f; break;

            case  3u: vBasePos = coreVector2( 0.1f, 0.8f); vBaseDir = coreVector2( 1.0f, 2.0f).Normalized(); fDelay = 0.0f;  fShift = -30.0f; fSide =  1.0f; break;
            case  4u: vBasePos = coreVector2( 0.2f, 0.2f); vBaseDir = coreVector2( 1.0f,-1.0f).Normalized(); fDelay = 0.3f;  fShift = -35.0f; fSide =  1.0f; break;
            case  5u: vBasePos = coreVector2( 0.5f,-0.6f); vBaseDir = coreVector2( 1.0f, 5.0f).Normalized(); fDelay = 0.4f;  fShift =   0.0f; fSide = -1.0f; break;
            case  6u: vBasePos = coreVector2(-0.8f, 0.1f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.7f;  fShift = -25.0f; fSide = -1.0f; break;

            case  7u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;  fShift = -10.0f; fSide = -1.0f; break;
            case  8u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.25f; fShift = -10.0f; fSide =  1.0f; break;
            case  9u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;  fShift = -10.0f; fSide = -1.0f; break;
            case 10u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.75f; fShift = -10.0f; fSide =  1.0f; break;

            case 11u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;  fShift =   0.0f; fSide = -1.0f; break;
            case 12u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.25f; fShift =   0.0f; fSide =  1.0f; break;
            case 13u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;  fShift =   0.0f; fSide = -1.0f; break;
            case 14u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.75f; fShift =   0.0f; fSide =  1.0f; break;
            }

            STAGE_LIFETIME(pEnemy, 0.25f, fDelay)

            const coreVector2 vTan  = vBaseDir.Rotated90() * fSide;
            const coreFloat   fTime = MAX(fLifeTime, 0.0f);

            coreVector2 vLerpPos;
                 if(i < 7u)  vLerpPos = LERPB(vBasePos - vTan * 2.0f, vBasePos,           MIN(fTime * 1.2f, 1.0f)) * (FOREGROUND_AREA);
            else if(i < 11u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 0.6f))       * (FOREGROUND_AREA * 1.2f);
            else             vLerpPos = LERP (-vTan,                  vTan, 0.5f - 0.5f * COS(fTime * 1.4f))       * (FOREGROUND_AREA * 1.2f);

            coreVector2 vLerpDir;
                 if(i < 7u)  vLerpDir = vBaseDir;
            else if(i < 11u) vLerpDir = vBaseDir;
            else             vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime * 1.1f).m12();

            if(STAGE_TAKEOFF) afDistance[i] = fShift;
            afDistance[i] += 100.0f * (fLifeTime - fLifeTimeBefore);

            const coreVector2 vNewPos = vLerpPos + vLerpDir * afDistance[i];

            if(((vNewPos.x < -FOREGROUND_AREA.x * 1.2f) && (vLerpDir.x < 0.0f)) ||
               ((vNewPos.x >  FOREGROUND_AREA.x * 1.2f) && (vLerpDir.x > 0.0f)) ||
               ((vNewPos.y < -FOREGROUND_AREA.y * 1.2f) && (vLerpDir.y < 0.0f)) ||
               ((vNewPos.y >  FOREGROUND_AREA.y * 1.2f) && (vLerpDir.y > 0.0f)))
            {
                if(afDistance[i] > 0.0) afDistance[i] -= MAX(g_pForeground->RayIntersection(vNewPos, -vLerpDir, 1.2f), 0.0f);
            }

            coreObject3D* pLaser = (*m_Laser.List())[i % VIRIDO_LASERS];
            ASSERT(pEnemy->ReachedDeath() || (pEnemy == m_apLaserOwner[i % VIRIDO_LASERS]))

            pEnemy->SetPosition (coreVector3(vNewPos,  0.0f));
            pEnemy->SetDirection(coreVector3(vLerpDir, 0.0f));
            pLaser->SetPosition (coreVector3(vLerpPos, 0.0f));
            pLaser->SetDirection(coreVector3(vLerpDir, 0.0f));

            if(STAGE_TICK_TIME(12.0f, 0.0f) && ((s_iTick % 12u) < 3u))   // TODO: tick-1 ? 
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.1f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.1f);
            }
        });

        STAGE_WAVE("DREI", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all lasers
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
            this->DisableLaser(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // big enemy splits up into smaller enemies on death
    // - all: cinders have low health, to make killing them with accidental hits easier
    // - all: cinders start at corners, to reduce accidental hits when enemies fly in
    // - all: arrows spawn with offset, to reduce accidental killing them before getting visible
    // TODO: too similar to meteor wave
    // TODO: spawn small enemies from all sides
    // TODO: create a lot more containers (3 each side ?)
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.3f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2( 0.0f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_RED);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 28u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vLastPos)
        STAGE_GET_END

        const auto nSpawnFunc = [&](const coreVector2 vPos, const coreUintW iIndex)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy2, i)
            {
                if((i >= iIndex * 4u) && (i < (iIndex + 1u) * 4u))
                {
                    const coreVector2 vDir = StepRotated90X(i % 4u);

                    pEnemy2->SetPosition (coreVector3(vPos + vDir * 4.0f, 0.0f));
                    pEnemy2->SetDirection(coreVector3(vDir,               0.0f));

                    pEnemy2->Resurrect();
                }
            });
        };

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy1, i)
        {
            if(pEnemy1->ReachedDeath()) nSpawnFunc(pEnemy1->GetPosition().xy(), i);
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 4u)
            else if(STAGE_SUB(3u)) nSpawnFunc(vLastPos, 5u);
            else if(STAGE_SUB(4u)) nSpawnFunc(vLastPos, 6u);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.9f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f) * ((i % 2u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i >= 3u) pEnemy->Rotate90();
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy(), LERPB(50.0f, 20.0f, MIN(pEnemy->GetLifeTime(), 1.0f)), 2.0f);

            if(STAGE_LIFETIME_AFTER(0.4f) && STAGE_TICK_LIFETIME(6.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, pEnemy, vPos, vDir)->ChangeSize(1.1f);
            }

            vLastPos = pEnemy->GetPosition().xy();
        });

        STAGE_COLL_PLAYER_ENEMY(pPlayer, pEnemy, vIntersection, bFirstHit, COLL_THIS)
        {
            if(!bFirstHit || (pEnemy->GetID() != cCinderEnemy::ID)) return;

            STAGE_BADGE(BADGE_EASY, pPlayer->GetPosition())
            pEnemy->TakeDamage(100, ELEMENT_NEUTRAL, vIntersection.xy(), pPlayer);
        });

        STAGE_WAVE("VIER", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // enemies move and shoot with player
    // schießen auf cooldown der waffe
    // gegner müssen immer gegen schussrichtung schießen um leichte kills zu vermeiden (speziell wenn ihre richtung fix ist, oder eine ihrer pos-achsen)
    // gegner in 3, wenn sie von ner schräge kommen sieht man alle 4 selbst wenn man am rand oder der ecke fliegt, müssen aber weit genug vom rand spawnen um kollisionen zu vermeiden wenn spieler in ecke ist
    // enemies not visible are not allowed to shoot (especially in sub 3)
    // in coop: wenn spieler stirbt soll ziel-spieler gewechselt werden, aber nicht bei wiederbelebung
    // TODO: flying directly between two enemies can cause infinity push between two enemies (especially in sub-phase 4)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.2f,1.2f,1.2f));
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(4u)
            STAGE_GET_VEC2 (vBaseRefPos)
            STAGE_GET_UINT (iPlayerTarget, iPlayerTarget = 1u)
            STAGE_GET_FLOAT(vPlayerLerp)
        STAGE_GET_END

        const cPlayer* pPlayer1 = g_pGame->GetPlayer(0u);
        const cPlayer* pPlayer2 = g_pGame->GetPlayer(1u);
        STATIC_ASSERT(GAME_PLAYERS == 2u)

        const coreBool bCleared = STAGE_CLEARED;

             if(CONTAINS_FLAG(pPlayer1->GetStatus(), PLAYER_STATUS_DEAD)) iPlayerTarget = 1u;
        else if(CONTAINS_FLAG(pPlayer2->GetStatus(), PLAYER_STATUS_DEAD)) iPlayerTarget = 0u;
        else if(bCleared)                                                 iPlayerTarget = 1u - iPlayerTarget;

             if(bCleared)      vPlayerLerp = I_TO_F(iPlayerTarget);
        else if(iPlayerTarget) vPlayerLerp = MIN(vPlayerLerp + 1.0f*Core::System->GetTime(), 1.0f);
        else                   vPlayerLerp = MAX(vPlayerLerp - 1.0f*Core::System->GetTime(), 0.0f);

        const coreVector2 vBasePos   = LERPS(pPlayer1->GetPosition ().xy(), pPlayer2->GetPosition ().xy(), vPlayerLerp) / FOREGROUND_AREA;
        const coreVector2 vBaseDir   = LERPS(pPlayer1->GetDirection().xy(), pPlayer2->GetDirection().xy(), vPlayerLerp).Normalized();
        const coreBool    bBaseShoot = pPlayer1->GetWeapon(0u)->GetCooldown() || pPlayer2->GetWeapon(0u)->GetCooldown();

        if(bCleared)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  4u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 19u)

            const coreVector2 vPos = vBasePos;
            const coreVector2 vDir = -vPos.Normalized();

            vBaseRefPos = (vPos + vDir * g_pForeground->RayIntersection(vPos, vDir, 1.4f)) / FOREGROUND_AREA;
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vNewPos    = vBasePos;
            coreVector2 vNewRefPos = vBaseRefPos;

            switch(i)
            {
            default: ASSERT(false)

            case  0u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f,-0.12f).Rotated45(); break;
            case  1u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f,-0.12f).Rotated45(); break;
            case  2u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f, 0.12f).Rotated45(); break;
            case  3u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f, 0.12f).Rotated45(); break;

            case  4u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f,-0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  5u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f,-0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  6u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f, 0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  7u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f, 0.12f).Rotated45(); vNewPos =  vNewPos.Rotated90(); break;
            case  8u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f,-0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;
            case  9u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f,-0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;
            case 10u: vNewPos = -vNewPos * 0.85f + coreVector2(-0.12f, 0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;
            case 11u: vNewPos = -vNewPos * 0.85f + coreVector2( 0.12f, 0.12f).Rotated45(); vNewPos = -vNewPos.Rotated90(); break;

            case 12u: vNewPos = vNewPos + coreVector2( 0.6f,  0.0f);  vNewRefPos = coreVector2( 0.0f, 1.4f); break;
            case 13u: vNewPos = vNewPos + coreVector2(-0.6f,  0.0f);  vNewRefPos = coreVector2( 0.0f,-1.4f); break;
            case 14u: vNewPos = vNewPos + coreVector2( 0.0f,  0.6f);  vNewRefPos = coreVector2(-1.4f, 0.0f); break;
            case 15u: vNewPos = vNewPos + coreVector2( 0.0f, -0.6f);  vNewRefPos = coreVector2( 1.4f, 0.0f); break;
            case 16u: vNewPos = vNewPos + coreVector2( 0.84f, 0.0f);  vNewRefPos = coreVector2( 0.0f, 1.4f); break;
            case 17u: vNewPos = vNewPos + coreVector2(-0.84f, 0.0f);  vNewRefPos = coreVector2( 0.0f,-1.4f); break;
            case 18u: vNewPos = vNewPos + coreVector2( 0.0f,  0.84f); vNewRefPos = coreVector2(-1.4f, 0.0f); break;
            case 19u: vNewPos = vNewPos + coreVector2( 0.0f, -0.84f); vNewRefPos = coreVector2( 1.4f, 0.0f); break;
            }

            vNewPos = LERPB(vNewRefPos, vNewPos, MIN(0.7f * fLifeTime, 1.0f));

            pEnemy->SetPosition (coreVector3(vNewPos * FOREGROUND_AREA, 0.0f));
            pEnemy->SetDirection(coreVector3(-vBaseDir,                 0.0f));

            if(bBaseShoot && STAGE_LIFETIME_AFTER(0.5f) && STAGE_TICK_LIFETIME(8.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                if(g_pForeground->IsVisiblePoint(vPos, 1.15f)) g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.15f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE("FÜNF", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // boss 1
    //STAGE_MAIN({TAKE_ALWAYS, 5u})
    //{
    //    STAGE_BOSS(m_Dharuk, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_MISSION, 5u})
    //{
    //    STAGE_GET_START(1u)
    //        STAGE_GET_FLOAT(fOldAngle, fOldAngle = g_pEnvironment->GetDirection().Angle())
    //    STAGE_GET_END
    //
    //    g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(fOldAngle, 0.0f*PI, MIN(m_fStageTime * 0.5f, 1.0f))));
    //
    //    STAGE_FINISH_AFTER(2.0f)
    //});

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u, 7u, 8u, 9u, 10u, 11u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // enemy has weakpoint which he keeps away from player
    // - all: miners rotation start against fly direction, to make evading the initial attack easier
    // - 2: cinders in pair always rotate together away from player when one is shot from outer direction, to prevent ping-pong effect
    // TODO: miner should be grey ?   
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.5f,1.4f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.5f,0.0f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(5u);
            pPath2->AddNode(coreVector2( 0.5f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 0.0f,-0.5f), coreVector2(-1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.5f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath2->AddNode(coreVector2( 0.0f, 0.5f), coreVector2( 1.0f, 0.0f));
            pPath2->AddLoop();
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(1, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cCinderEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(32u)
            STAGE_GET_FLOAT_ARRAY(afTarget,    10u, afTarget[1] = 1.0f*PI, afTarget[2] = 1.0f*PI, afTarget[6] = 1.5f*PI, afTarget[8] = 0.5f*PI)
            STAGE_GET_FLOAT_ARRAY(afSign,      10u)
            STAGE_GET_UINT_ARRAY (aiFreeState,  4u)
            STAGE_GET_VEC2_ARRAY (aiFreeMove,   4u)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            if(pEnemy->ReachedDeath() && ((i < 2u) || CONTAINS_FLAG(pSquad2->GetEnemy(i ^ 0x01u)->GetStatus(), ENEMY_STATUS_DEAD)))
            {
                cEnemy* pParent = pSquad1->GetEnemy((i < 2u) ? i : ((i+2u) / 2u));
                pParent->Kill(true);
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) {STAGE_RESSURECT(pSquad1, 0u, 1u) STAGE_RESSURECT(pSquad2, 0u, 1u)}
            else if(STAGE_SUB(2u)) {STAGE_RESSURECT(pSquad1, 2u, 3u) STAGE_RESSURECT(pSquad2, 2u, 5u)}
            else if(STAGE_SUB(3u)) {STAGE_RESSURECT(pSquad1, 4u, 5u) STAGE_RESSURECT(pSquad2, 6u, 9u)}
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            const coreVector2 vShootDir = pEnemy->GetDirection().xy();

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if((i)    % 2u) pEnemy->Rotate180();
            if((i/2u) % 2u) pEnemy->Rotate90();

            const coreFloat fTurn = coreVector2::Dot(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), vShootDir.Rotated90());

            pEnemy->SetDirection(coreVector3(STAGE_TAKEOFF ? -pEnemy->GetDirection().xy() : coreVector2::Direction(vShootDir.Angle() + MIN(ABS(fTurn) * 5.0f, 1.0f) * SIGN(fTurn) * -1.2f * Core::System->GetTime()), 0.0f));

            if(STAGE_TICK_LIFETIME_BASE(25.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if((i < 6u) || !aiFreeState[i-6u])
            {
                const cEnemy*     pParent    = pSquad1->GetEnemy((i < 2u) ? i : ((i+2u) / 2u));
                const coreVector2 vParentDir = ((pEnemy->GetPosition().xy() + pParent->GetMove()) - pParent->GetPosition().xy()).Normalized();

                const coreFloat fAngleFrom = vParentDir.Angle();
                const coreFloat fAngleTo   = ((i == 3u || i == 5u || i == 7u || i == 9u) ? (afTarget[i-1u] + (0.5f*PI)) : afTarget[i]);
                const coreFloat fShift     = ((i == 3u || i == 5u || i == 7u || i == 9u) ? (afSign  [i-1u])             : afSign  [i]) * ABS(AngleDiff(fAngleTo, fAngleFrom));

                const coreVector2 vDir = coreVector2::Direction(STAGE_TAKEOFF ? fAngleTo : (fAngleFrom + AngleDiff(fAngleTo + fShift, fAngleFrom) * 2.5f * Core::System->GetTime()));
                const coreVector2 vPos = pParent->GetPosition().xy() + vDir * 13.0f;

                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
            else
            {
                coreVector2&      vDir = aiFreeMove[i-6u];
                const coreVector2 vPos = pEnemy->GetPosition().xy() + vDir * (40.0f * Core::System->GetTime());

                     if((vPos.x < -FOREGROUND_AREA.x) && (vDir.x < 0.0f)) vDir.x =  ABS(vDir.x);
                else if((vPos.x >  FOREGROUND_AREA.x) && (vDir.x > 0.0f)) vDir.x = -ABS(vDir.x);
                     if((vPos.y < -FOREGROUND_AREA.y) && (vDir.y < 0.0f)) vDir.y =  ABS(vDir.y);
                else if((vPos.y >  FOREGROUND_AREA.y) && (vDir.y > 0.0f)) vDir.y = -ABS(vDir.y);

                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
        });

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_VAL(pSquad1), COLL_VAL(pSquad2), COLL_VAL(afTarget), COLL_VAL(afSign), COLL_VAL(aiFreeState), COLL_VAL(aiFreeMove))
        {
            if(!bFirstHit || (pEnemy->GetID() != cCinderEnemy::ID)) return;

            const coreUintW i          = pSquad2->GetIndex(pEnemy);
            const coreFloat fNewTarget = AlongCrossNormal(pBullet->GetFlyDir()).Angle() + CORE_MATH_PRECISION;

            if(i < 6u)
            {
                if(i == 3u || i == 5u) {afTarget[i-1u] = fNewTarget - (0.5f*PI); afSign[i-1u] =  0.1f;}
                                  else {afTarget[i]    = fNewTarget;             afSign[i]    = -0.1f;}
            }
            else if(!aiFreeState[i-6u])
            {
                const cEnemy*     pParent    = pSquad1->GetEnemy((i < 2u) ? i : ((i+2u) / 2u));
                const coreVector2 vParentDir = (pEnemy->GetPosition().xy() - pParent->GetPosition().xy()).Normalized();

                const coreFloat fAngleFrom = vParentDir.Angle();
                const coreFloat fAngleTo   = fNewTarget;
                const coreFloat fShift     = ((i == 7u || i == 9u) ? 0.1f : -0.1f) * ABS(AngleDiff(fAngleTo, fAngleFrom));

                aiFreeState[i-6u] = 1u;
                aiFreeMove [i-6u] = (AngleDiff(fAngleTo + fShift, fAngleFrom) < 0.0f) ? vParentDir.Rotated135() : -vParentDir.Rotated45();
            }
        });

        STAGE_WAVE("SECHS", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // explode into bullets on death
    // - all: number of bullet waves is uneven, to create a better pattern visually
    // - all: number of enemies is uneven, to let them cross the center alternately, but still in equal distance
    // TODO: slower with size
    STAGE_MAIN({TAKE_ALWAYS, 7u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 9u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.1f, (1.0f/9.0f) * I_TO_F(i))

            STAGE_REPEAT(0.5f)

            if(pEnemy->GetCurHealth())
            {
                pEnemy->SetPosition(coreVector3(0.75f * SIN((fLifeTime * 4.0f*PI + (1.0f/9.0f) * I_TO_F(i) * 2.0f*PI)), 1.4f * (1.0f - 4.0f * fLifeTime), 0.0f) * FOREGROUND_AREA3);
                pEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * (5.0f - 3.8f * pEnemy->GetCurHealthPct()));
            }
            else
            {
                if(pEnemy->ReachedDeath())
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);
                }

                const coreFloat fNewSize = pEnemy->GetSize().x - 4.0f * Core::System->GetTime();
                const coreUint8 iOldTick = F_TO_UI(pEnemy->GetSize().x);
                const coreUint8 iNewTick = F_TO_UI(fNewSize);

                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * fNewSize);

                if(iOldTick != iNewTick)
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();

                    for(coreUintW j = 4u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) + 0.5f * I_TO_F(iNewTick % 2u)) * 45.0f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    }
                }

                if(iNewTick == 0u)
                {
                    pEnemy->Kill(false);
                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                }

                d_cast<cStarEnemy*>(pEnemy)->SetAngle(d_cast<cStarEnemy*>(pEnemy)->GetAngle() - 9.0f * Core::System->GetTime());
            }
        });

        STAGE_WAVE("SIEBEN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // leave destroyable residue on its way (r-type leaver)
    // enemies cannot move on the same path in infinity as the bullets would just overlay or need some handling  
    // multiple following enemies with the same move pattern will overlay after some time  
    // enemy direction creates a whirl 
    // destroying player bullets on impact is bad 
    // changing speed breaks bullet pattern 
    // snake is also bad 
    // bit-matrix or clearing on contact is bad 
    // start direction against whirl direction to not clutter the bullets and show the whirl effect   
    // TODO: handle dodge and enemy inside   
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });

            pSquad1->GetEnemy( 0u)->SetPosition(coreVector3(-0.8f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 0u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy( 1u)->SetPosition(coreVector3( 0.8f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 1u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy( 2u)->SetPosition(coreVector3(-1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 2u)->SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 3u)->SetPosition(coreVector3( 1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 3u)->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 4u)->SetPosition(coreVector3(-1.2f,-0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 4u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy( 5u)->SetPosition(coreVector3( 0.0f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 5u)->SetDirection(coreVector3( 1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 6u)->SetPosition(coreVector3( 1.2f, 0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 6u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy( 7u)->SetPosition(coreVector3( 0.0f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 7u)->SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());

            pSquad1->GetEnemy( 8u)->SetPosition(coreVector3( 0.8f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 8u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy( 9u)->SetPosition(coreVector3(-0.8f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 9u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy(10u)->SetPosition(coreVector3( 1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(10u)->SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(11u)->SetPosition(coreVector3(-1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(11u)->SetDirection(coreVector3( 1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(12u)->SetPosition(coreVector3(-1.2f, 0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(12u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(13u)->SetPosition(coreVector3( 0.0f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(13u)->SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(14u)->SetPosition(coreVector3( 1.2f,-0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(14u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(15u)->SetPosition(coreVector3( 0.0f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(15u)->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalized());
        });

        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fWhirlSpeed)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESSURECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESSURECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESSURECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESSURECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESSURECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB( 6u)) STAGE_RESSURECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 7u)) STAGE_RESSURECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB( 8u)) STAGE_RESSURECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB( 9u)) STAGE_RESSURECT(pSquad1, 10u, 10u)
            else if(STAGE_SUB(10u)) STAGE_RESSURECT(pSquad1, 11u, 11u)
            else if(STAGE_SUB(11u)) STAGE_RESSURECT(pSquad1, 12u, 13u)
            else if(STAGE_SUB(12u)) STAGE_RESSURECT(pSquad1, 14u, 15u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((m_iStageSub == 7u) && (fWhirlSpeed < 1.0f)) return;

            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vDiff   = pEnemy->AimAtPlayerDual((((i / 4u) % 2u) ? ((i / 2u) % 2u) : (i % 2u)) ? 0u : 1u);
            const coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            coreVector2       vCurDir = pEnemy->GetDirection().xy();

            if(coreVector2::Dot(vDiff, vCurDir) <= 0.0f) vCurDir = vCurDir.Rotated90() * SIGN(coreVector2::Dot(vDiff, vCurDir.Rotated90()));

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);

            pEnemy->DefaultMoveForward(vCurDir, 29.0f);

            if(STAGE_TICK_TIME(9.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();
                const coreVector2 vTan = vDir.Rotated90() * 2.0f;

                if(coreMath::IsNear(vDir.x, 0.0f) || coreMath::IsNear(vDir.y, 0.0f))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos + vTan, vDir);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos - vTan, vDir);
                }
                else
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos + vTan, vDir);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos - vTan, vDir);
                }
            }
        });

        if(m_iStageSub >= 7u)
        {
            fWhirlSpeed = MIN(fWhirlSpeed + 0.3f * Core::System->GetTime(), 1.0f);

            const coreMatrix2 mRota    = coreMatrix3::Rotation(LERPS(0.0f, 0.7f, fWhirlSpeed) * Core::System->GetTime()).m12();
            const coreMatrix2 mRotaRev = mRota.Transposed();

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRotaRev, 0.0f));
            });

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRota, 0.0f));
            });
        }

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            pBulletEnemy->Deactivate(true, vIntersection.xy());
        });

        STAGE_WAVE("ACHT", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // dicke kugerl die am rand bleiben
    // - all: bullet speed is much lower than enemy speed, to create a perceptible path for the player to use
    // - all: bullet frequency is not too high, to better distinguish individual bullets
    // - all: enemy attack begins later, to not slow down bullets immediately after shooting
    // - 1,2: enemy path is not too wide, to not slow down bullets immediately after shooting
    // TODO: enemy or golden ball (1-N) hides in blue balls, to attack for badge
    // TODO: blue helper in one of the bullets
    STAGE_MAIN({TAKE_ALWAYS, 9u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.5f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.5f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve (8u);
            pPath2->AddNode (coreVector2(-0.5f, 0.5f), coreVector2( 1.0f, 0.0f));
            pPath2->AddNodes(coreVector2( 0.5f, 0.5f), coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath2->AddNodes(coreVector2( 0.5f,-0.5f), coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath2->AddNodes(coreVector2(-0.5f,-0.5f), coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath2->AddNode (coreVector2(-0.5f, 0.5f), coreVector2( 0.0f, 1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,0.0f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(200, COLOR_SHIP_PURPLE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 3u, 3u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, 0.0f)

            const coreVector2 vFactor = coreVector2((i % 2u) ? -1.0f : 1.0f, (i == 2u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            if(i == 3u)
            {
                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (fLifeTime * 5.0f);
            }
            else
            {
                if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
                {
                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                }
                else
                {
                    pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                }
            }

            if(STAGE_LIFETIME_AFTER_BASE(((i == 3u) ? pPath3 : pPath1)->GetTotalDistance()) && STAGE_TICK_LIFETIME_BASE(6.0f, (i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                if(i == 3u)
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, -vDir)            ->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos,  vDir.Rotated90())->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, -vDir.Rotated90())->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                }
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            const coreFloat fFactor = LERP(0.05f, 1.0f, MIN(pBullet->GetFlyTime() * 0.5f, 1.0f));

            pBullet->SetSize(coreVector3(15.0f,15.0f,15.0f) * fFactor);
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            if(coreMath::IsNear(pBullet->GetSpeed(), 0.1f)) return;

            if(!g_pForeground->IsVisiblePoint(pBullet->GetPosition().xy(), 1.0f))
            {
                pBullet->SetSpeed(0.1f);
                return;
            }

            Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, pBullet, [](cBullet* OUTPUT pBullet1, cBullet* OUTPUT pBullet2, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                if(!g_pForeground->IsVisiblePoint(pBullet1->GetPosition().xy(), 1.0f))
                {
                    pBullet2->SetSpeed(0.1f);
                    return;
                }
            });
        });

        STAGE_WAVE("NEUN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // enemies jump into camera
    // - all: target shadow is essential, to communicate direct targeting in third sub-stage
    // - 1,2: bullet patterns provide a lot of safe space, as jumping into the camera may overwhelm the player
    // TODO: check for and fix shadow artifacts, when jumping behind near clipping plane of shadow viewport (maybe fade out near plane)
    STAGE_MAIN({TAKE_ALWAYS, 10u})
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(9u * 20u)
            STAGE_GET_VEC2_ARRAY (avOldPos,   20u)
            STAGE_GET_VEC2_ARRAY (avNewPos,   20u)
            STAGE_GET_FLOAT_ARRAY(avOldAngle, 20u)
            STAGE_GET_FLOAT_ARRAY(avNewAngle, 20u)
            STAGE_GET_FLOAT_ARRAY(afJump,     20u)
            STAGE_GET_FLOAT_ARRAY(afWait,     20u, for(coreUintW i = 0u; i < 20u; ++i) afWait[i] = 0.5f;)
            STAGE_GET_UINT_ARRAY (aiCount,    20u)
        STAGE_GET_END

        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
        {
            if(m_apShadowOwner[i] && CONTAINS_FLAG(m_apShadowOwner[i]->GetStatus(), ENEMY_STATUS_DEAD))
                this->DisableShadow(i, true);
        }

        if(STAGE_CLEARED)
        {
            if(STAGE_SUB(1u))
            {
                STAGE_RESSURECT(pSquad1, 0u, 3u)
                pSquad1->GetEnemy( 0u)->SetPosition(coreVector3( 0.6f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 0u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy( 1u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 1u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy( 2u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 2u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy( 3u)->SetPosition(coreVector3(-0.6f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 3u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESSURECT(pSquad1, 4u, 11u)
                pSquad1->GetEnemy( 4u)->SetPosition(coreVector3( 1.3f, 0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 4u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 5u)->SetPosition(coreVector3( 1.3f, 0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 5u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 6u)->SetPosition(coreVector3( 1.3f,-0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 6u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 7u)->SetPosition(coreVector3( 1.3f,-0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 7u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 8u)->SetPosition(coreVector3(-1.3f,-0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 8u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 9u)->SetPosition(coreVector3(-1.3f,-0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 9u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy(10u)->SetPosition(coreVector3(-1.3f, 0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(10u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy(11u)->SetPosition(coreVector3(-1.3f, 0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(11u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESSURECT(pSquad1, 12u, 19u)
                pSquad1->GetEnemy(12u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(12u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(13u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(13u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(14u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(14u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(15u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(15u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(16u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(16u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(17u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(17u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(18u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(18u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(19u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(19u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            }
        }

        constexpr coreFloat fWaitSpeed = 0.9f;
        constexpr coreFloat fJumpSpeed = 0.7f;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 12u) ? (0.2f * I_TO_F(i % 4u)) : (((1.0f/fWaitSpeed) + (1.0f/fJumpSpeed)) * 0.125f * I_TO_F(i % 8u)))

            if(STAGE_TAKEOFF)
            {
                avOldPos  [i] = pEnemy->GetPosition ().xy();
                avOldAngle[i] = pEnemy->GetDirection().xy().Angle();
            }

            if(STAGE_LIFETIME_AFTER(0.0f))
            {
                if(afWait[i] < 1.0f)
                {
                    afWait[i] += fWaitSpeed * Core::System->GetTime();
                    if(afWait[i] >= 1.0f)
                    {
                        afJump[i] = 0.0f;
                        afWait[i] = 1.0f;

                        aiCount[i] += 1u;

                        avOldPos  [i] = pEnemy->GetPosition ().xy();
                        avOldAngle[i] = pEnemy->GetDirection().xy().Angle();

                        if(i < 4u)
                        {
                            avNewPos  [i] = coreVector2(0.75f - 0.5f * I_TO_F(i % 4u), 0.8f) * FOREGROUND_AREA * ((aiCount[i] % 2u) ? -1.0f : 1.0f);
                            avNewAngle[i] = avOldAngle[i] + 1.0f*PI;
                        }
                        else if(i < 12u)
                        {
                            avNewPos  [i] = coreVector2(0.8f, 0.75f - 0.5f * I_TO_F(i % 4u)) * FOREGROUND_AREA * ((aiCount[i] % 2u) ? -1.0f : 1.0f) * ((i >= 8u) ? -1.0f : 1.0f);
                            avNewAngle[i] = avOldAngle[i] + 1.0f*PI;
                        }
                        else
                        {
                            avNewPos  [i] = pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy();
                            avNewAngle[i] = avOldAngle[i] + 2.0f*PI;
                        }

                        this->EnableShadow(i % VIRIDO_SHADOWS, pEnemy, avNewPos[i]);
                    }
                }
                else
                {
                    afJump[i] += fJumpSpeed * Core::System->GetTime();
                    if(afJump[i] >= 1.0f)
                    {
                        afJump[i] = 1.0f;
                        afWait[i] = 0.0f;

                        const coreVector2 vPos = pEnemy->GetPosition().xy();

                        for(coreUintW j = 4u; j--; )
                        {
                            const coreVector2 vDir = StepRotated90(j);
                            const coreVector2 vTan = vDir.Rotated90() * 3.0f;

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.4f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos,        vDir)->ChangeSize(1.4f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.4f);
                        }

                        this->DisableShadow(i % VIRIDO_SHADOWS, true);
                    }
                }

                const coreFloat   fSin    = SIN(PI * afJump[i]);
                const coreVector2 vPos    = LERP(avOldPos[i], avNewPos[i], afJump[i]) * (1.0f - 0.45f * fSin);
                const coreFloat   fHeight = 50.0f * fSin;

                pEnemy->SetPosition         (coreVector3(vPos, fHeight));
                pEnemy->DefaultRotateLerp   (avOldAngle[i], avNewAngle[i], afJump[i]);
                pEnemy->DefaultOrientateLerp(0.0f*PI,       2.0f*PI,       afJump[i]);
            }
        });

        STAGE_WAVE("ZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all shadows
    STAGE_MAIN({TAKE_ALWAYS, 10u})
    {
        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
            this->DisableShadow(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // boss 2
    //STAGE_MAIN({TAKE_ALWAYS, 11u})
    //{
    //    if(STAGE_BEGINNING)
    //    {
    //       g_pEnvironment->SetTargetSpeed(0.0f);
    //    }
    //
    //    STAGE_BOSS(m_Torus, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 12u, 13u, 14u, 15u, 16u, 17u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // enemy charges straight forward on death (r-type fish)
    // - 2: should start one same spot an opposite of first group, to reduce accidental collision (which is more likely with 2x2)  
    // no passive attack, not impact attack, no bounce, because it's too overwhelming together with the green ball  
    // fire speed fast enough to create visual line  
    // 2x2 instead of 1x4, to force player keeping fire at the same position while an enemy is already flying at them  
    // TODO: give central effect to highlight damaging touch   
    STAGE_MAIN({TAKE_ALWAYS, 12u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f * SQRT2), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f),         coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 32u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.45f,1.45f,1.45f));
                pEnemy->Configure(10, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(33u)
            STAGE_GET_FLOAT_ARRAY(afSpeed, 32u)
            STAGE_GET_FLOAT      (fAngle)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy->GetCurHealth() == 0) && !g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()))
                pEnemy->Kill(true);
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 16u, 31u)
        }

             if(m_iStageSub == 2u) fAngle += 0.25f*PI * Core::System->GetTime();
        else if(m_iStageSub == 3u) fAngle -= 0.25f*PI * Core::System->GetTime();
        const coreMatrix2 mRota = coreMatrix3::Rotation(fAngle).m12();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.85f, 0.3f * I_TO_F(i % 2u))

            if(pEnemy->ReachedDeath())
            {
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            }

            if(pEnemy->GetCurHealth() == 0)
            {
                afSpeed[i] += 80.0f * Core::System->GetTime();

                pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * ((20.0f + afSpeed[i]) * Core::System->GetTime()));

                if(STAGE_TICK_TIME(30.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (i < 16u) ? 0.5f : 0.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                }
            }
            else if(i < 8u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f + (((i % 4u) < 2u) ? -1.0f : 1.0f)) * 0.22f, ((i % 8u) < 4u) ? 0.0f : 0.22f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection   (coreVector3(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), 0.0f));
            }
            else
            {
                const coreVector2 vFactor = coreVector2(1.0f, ((i % 8u) < 4u) ? 1.0f : -1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 2u) - 0.5f) * 0.22f, ((i % 4u) < 2u) ? 0.0f : 0.22f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(i >= 24u) pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy().Rotated90() * mRota, 0.0f));
                        else pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy()             * mRota, 0.0f));

                pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), 0.0f));
            }
        });

        STAGE_WAVE("ELF", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // bend
    // cannot throw enemies at each other, may cause unstable movement  
    // cannot let all enemies of a sub-wave pass a single point, too easy (includes circle movement)  
    // push-strength needs to be scaled to make sure that a whole 2x2 group can be killed in a single sweep by grazing one of their sides  
    //     on first sub-wave center group needs to move into player, as they will naturally try to shoot and dodge them, see the mechanic, and evade their first bullet-wave  
    // make this wave easier for advanced players, as the enemy-movement might not be as deterministic  
    // difference between attackable and non-attackable must be very clear
    //     on first wave, one of the up-flying enemies need to fly first with some delay, to show the bullet-attack
    // coop: player not receiving two groups in second sub-wave needs to receive two groups in first sub-wave
    // TODO: completely disable player-enemy collision, just in case
    STAGE_MAIN({TAKE_ALWAYS, 13u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 100u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.3f,1.3f,1.3f));
                pEnemy->Configure(4, COLOR_SHIP_GREY);

                d_cast<cStarEnemy*>(pEnemy)->SetAngle(I_TO_F(i) * 2.0f/5.0f * PI);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 11u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 12u, 35u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 36u, 99u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vAlong = (i >= 12u && i < 36u) ? coreVector2(-1.0f,0.0f) : coreVector2(0.0f,1.0f);
            const coreVector2 vSide  = coreVector2(1.0f, ((i < 36u) && (((i + 4u) % 12u) < 8u)) ? -1.0f : 1.0f);

            const coreMatrix2 mTransform    = coreMatrix3::Scaling(vSide).m12() * coreMatrix3::Rotation(vAlong).m12();
            const coreMatrix2 mTransformRev = mTransform.Inverted();

            coreVector2 vTarget;
            if(i < 36u)
            {
                const coreUintW x  = ((i / 4u) % 3u);
                const coreUintW y  = ((i / 4u) / 3u) % 2u;
                const coreUintW x2 = (i / 2u) % 2u;
                const coreUintW y2 = (i % 2u);

                vTarget   = coreVector2(-0.7f + 0.7f * I_TO_F(x), -0.5f + 1.2f * I_TO_F(y) + 0.1f * I_TO_F(x2)) + 0.2f * coreVector2(-0.5f + 1.0f * I_TO_F(x2), -0.5f + 1.0f * I_TO_F(y2));
                vTarget.y = vTarget.y - ((i < 12u) ? 1.0f : 1.2f) * fLifeTime + 0.8f * I_TO_F((x + 1u) % 3u) + 2.0f;
            }
            else
            {
                const coreUintW x = (i - 36u) % 8u;
                const coreUintW y = (i - 36u) / 8u;

                vTarget   = coreVector2(-0.9f + 0.25f * I_TO_F(x), 1.2f - 0.3f * I_TO_F(y) + 0.15f * I_TO_F(x % 2u));
                vTarget.y = vTarget.y - 1.4f * fLifeTime + 2.4f;
            }

            if(STAGE_TAKEOFF) pEnemy->SetPosition(coreVector3((vTarget * FOREGROUND_AREA + coreVector2(0.0f,1.0f)) * mTransform, 0.0f));   // make sure to add offset due to imprecision

            const coreVector2 vEnemyPos  = pEnemy->GetPosition().xy() * mTransformRev;
            const coreVector2 vEnemyMove = pEnemy->GetMove()          * mTransformRev;

            coreVector2 vWrap = vTarget * FOREGROUND_AREA;
            if(vWrap    .y < 0.0f)    vWrap.y = FmodRange(vWrap.y, -1.2f * FOREGROUND_AREA.y, 1.2f * FOREGROUND_AREA.y);
            if(vEnemyPos.y < vWrap.y) vWrap.y = vWrap.y - 2.4f * FOREGROUND_AREA.y;

            pEnemy->SetBaseColor(COLOR_SHIP_GREY);
            pEnemy->AddStatus   (ENEMY_STATUS_INVINCIBLE);

            STAGE_FOREACH_PLAYER(pPlayer, j)
            {
                const coreVector2 vPlayerPos = pPlayer->GetPosition().xy() * mTransformRev;

                const coreVector2 vDiff = vEnemyPos - vPlayerPos;
                const coreVector2 vDir  = vDiff.Normalized();
                const coreFloat   fLen  = vDiff.Length();
                const coreFloat   fStr  = MAX(24.0f - fLen, 0.0f) / 24.0f;
                const coreFloat   fDot  = vEnemyMove.IsNull() ? 0.5f : MAX(coreVector2::Dot(vDir, -vEnemyMove.Normalized()), 0.0f);

                const coreVector2 vRealTarget = LERP(vWrap, vEnemyPos + vDir * 35.0f, fStr);
                const coreVector2 vChange     = vRealTarget - vEnemyPos;

                pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + (vChange * ((fStr * fDot * 9.0f + 2.0f) * Core::System->GetTime())) * mTransform, 0.0f));

                if(fStr)
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_RED);
                    pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                }
            });

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
            {
                if(pEnemy == pEnemy2) return; // TODO: check for (j <= i), or add note for double-calculations

                const coreVector2 vDiff  = pEnemy->GetPosition().xy() - pEnemy2->GetPosition().xy();
                const coreFloat   fPower = POW2(pEnemy->GetCollisionRadius() * 2.0f) - vDiff.LengthSq();

                if((fPower > 0.0f) && !vDiff.IsNull())
                {
                    constexpr coreVector2 vBase = coreVector2(0.0f,0.0f);

                    const coreFloat A = (pEnemy ->GetPosition().xy() - vBase).LengthSq();
                    const coreFloat B = (pEnemy2->GetPosition().xy() - vBase).LengthSq();

                    const coreVector2 vPush = vDiff.Normalized() * (fPower * Core::System->GetTime());

                    if(A > B) pEnemy ->SetPosition(coreVector3(pEnemy ->GetPosition().xy() + vPush, 0.0f));
                         else pEnemy2->SetPosition(coreVector3(pEnemy2->GetPosition().xy() - vPush, 0.0f));
                }
            });

            coreVector2 vCurPos = pEnemy->GetPosition().xy() * mTransformRev;

                                 vCurPos.x = CLAMP(vCurPos.x, -1.08f * FOREGROUND_AREA.x, 1.08f * FOREGROUND_AREA.x);
            if(vCurPos.y < 0.0f) vCurPos.y = FmodRange(vCurPos.y, -1.2f * FOREGROUND_AREA.y, 1.2f * FOREGROUND_AREA.y);

            pEnemy->SetPosition(coreVector3(vCurPos * mTransform, 0.0f));

            const coreBool bInsideOld = g_pForeground->IsVisiblePoint(pEnemy->GetOldPos()        * vAlong);
            const coreBool bInsideNew = g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy() * vAlong);
            const coreBool bForward   = (coreVector2::Dot(pEnemy->GetMove(), vAlong.InvertedX() * vSide.y) < 0.0f);

            if(((bInsideOld && !bInsideNew) || ((g_pGame->GetDifficulty() > 1u) && !bInsideOld && bInsideNew)) && bForward)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = ((i < 12u) ? pEnemy->AimAtPlayerDual((i / 8u) % 2u) : pEnemy->AimAtPlayerSide()).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, vDir)->ChangeSize(1.4f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 3u, COLOR_ENERGY_ORANGE);
            }
        });

        STAGE_WAVE("ZWÖLF", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // push
    // bullets fly fast to give visual waves with space inbetween (but should not be shot too often to compensate) 
    // in 4 player should not be able to pass when enemies are axis aligned 
    // in 3 only five enemies / half the circle should peek out 
    // in 2 lines should move over the center 
    // in 1 enemies should not reach bottom 
    // in 1 and 2 back of enemies should not be reachable without shooting at them first 
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2( 0.0f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 48u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.27f,1.27f,1.27f));
                pEnemy->Configure((i < 32u) ? 50 : 200, COLOR_SHIP_ORANGE);

                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));

                if(i < 40u)
                {
                    const coreVector2 vPos = (i < 32u) ? coreVector2((I_TO_F(i % 8u) - 3.5f) * 0.24f, (i < 8u) ? 1.3f : 1.54f) : (coreVector2(0.0f,1.74f) + 0.32f * coreVector2::Direction(I_TO_F(i % 8u) * (0.25f*PI)));
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                    pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir,                   0.0f));

                         if(i >= 16u && i < 24u) pEnemy->Rotate270();
                    else if(i >= 24u && i < 32u) pEnemy->Rotate90();
                }
            });
        });

        STAGE_GET_START(82u)
            STAGE_GET_VEC2_ARRAY(avForce, 40u, for(coreUintW i = 0u; i < 40u; ++i) avForce[i] = 85.0f * pSquad1->GetEnemy(i)->GetDirection().xy();)
            STAGE_GET_FLOAT     (fMillForce)
            STAGE_GET_FLOAT     (fMillAngle, fMillAngle = 0.5f*PI)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_VAL(pSquad1), COLL_VAL(avForce), COLL_REF(fMillForce))
        {
            if(!bFirstHit) return;

            const coreUintW i = pSquad1->GetIndex(pEnemy);

            coreUint8 iGroupNum;
                 if(i < 16u) iGroupNum = 2u;
            else if(i < 32u) iGroupNum = 4u;
            else             iGroupNum = 8u;

            const coreFloat fPower = I_TO_F(pBullet->GetDamage()) * (g_pGame->GetCoop() ? 0.5f : 1.0f);

            coreVector2 vForceDir;
                 if(i < 16u) vForceDir   = fPower * 6.0f * coreVector2( 0.0f,-1.0f);
            else if(i < 24u) vForceDir   = fPower * 6.0f * coreVector2( 1.0f, 0.0f);
            else if(i < 32u) vForceDir   = fPower * 6.0f * coreVector2(-1.0f, 0.0f);
            else if(i < 40u) vForceDir   = fPower * 6.0f * AlongCrossNormal(-pBullet->GetFlyDir());
            else             fMillForce -= fPower * 0.4f;

            for(coreUintW j = coreMath::FloorAlign(i, iGroupNum), je = coreMath::CeilAlign(i + 1u, iGroupNum); j < je; ++j)
            {
                if(i < 40u) avForce[j] += vForceDir;
                if(i != j)  pSquad1->GetEnemy(j)->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()));
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 15u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 16u, 31u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 32u, 39u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 40u, 47u)
        }

        if(fMillForce)
        {
            fMillAngle += fMillForce * Core::System->GetTime();
            fMillForce *= FrictionFactor(6.0f);
        }

        const coreVector2 vMillDir = coreVector2::Direction(fMillAngle);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.4f, 0.0f)

            if(i < 40u)
            {
                coreVector2 vNewPos = pEnemy->GetPosition().xy();

                if(!avForce[i].IsNull())
                {
                    vNewPos    += avForce[i] * Core::System->GetTime();
                    avForce[i] *= FrictionFactor(3.0f);
                }

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else
            {
                const coreFloat   fOffset = (fLifeTime < pPath1->GetTotalDistance()) ? (pPath1->CalcPosition(fLifeTime).x * ((i < 44u) ? 1.0f : -1.0f)) : 0.0f;
                const coreVector2 vNewPos = vMillDir * ((I_TO_F(i % 8u) - 3.5f) * 0.26f + fOffset) * FOREGROUND_AREA;

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            if(STAGE_TICK_LIFETIME(20.0f * (0.8f / fLifeSpeed), 0.0f) && ((s_iTick % 20u) >= 18u))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual((i < 16u || i >= 40u) ? ((i / 4u) % 2u) : ((i < 32u) ? ((i / 8u) % 2u) : (i % 2u))).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.25f);
            }
        });

        STAGE_WAVE("DREIZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // can only kill in order
    // coop last enemy, need to work together   
    // abwechselnd links rechts 
    // doppelgruppe nur breites pattern um ausweichen zu erleichtern 
    // reihenfolge am weitesten entfernt 
    // TODO: show 3142 group at start, matrix of enemies
    STAGE_MAIN({TAKE_ALWAYS, 15u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.3f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2(-0.9f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 9u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                if(i == 8u) pEnemy->SetSize(coreVector3(1.3f,1.3f,1.3f));

                pEnemy->Configure((i == 8u) ? 200 : 50, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT (iChiefNum)
            STAGE_GET_FLOAT(fShotOffset)
        STAGE_GET_END

        const auto nCreateChiefFunc = [&]()
        {
            constexpr coreUintW aiChiefOrder[] = {5u, 3u, 4u, 2u, 7u, 0u, 6u, 1u, 8u};

            ASSERT(iChiefNum < ARRAY_SIZE(aiChiefOrder))
            cEnemy* pNewChief = pSquad1->GetEnemy(aiChiefOrder[iChiefNum]);

            pNewChief->SetSize     (pNewChief->GetSize() * 1.25f);
            pNewChief->SetBaseColor(COLOR_SHIP_BLUE);
            pNewChief->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

            iChiefNum  += 1u;
            fShotOffset = FRACT(m_fStageTime * ((iChiefNum == 9u) ? 3.0f : 1.1f)) - 0.05f;

            g_pSpecialEffects->CreateSplashColor(pNewChief->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_BLUE);
        };

        if(STAGE_TIME_POINT(2.0f)) nCreateChiefFunc();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.2f * I_TO_F(i % 4u))

            const coreVector2 vFactor = coreVector2((i < 4u) ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, (i == 8u) ? 0.0f : (0.4f - 0.4f * I_TO_F(i % 4u)));

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i == 8u) pEnemy->Rotate90();

            if(pEnemy->ReachedDeath())
            {
                if(!pSquad1->IsFinished())
                {
                    if(iChiefNum <= 7u) nCreateChiefFunc();
                    if(iChiefNum == 7u) nCreateChiefFunc();
                    if(iChiefNum == 8u) if(pSquad1->GetNumEnemiesAlive() == 1u) nCreateChiefFunc();
                }
            }
            else if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_INVINCIBLE))
            {
                if(STAGE_TICK_TIME((iChiefNum == 9u) ? 3.0f : 1.1f, fShotOffset))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();
                    const coreUintW   iType = 4u - ((iChiefNum == 9u) ? (s_iTick % 3u) : (((iChiefNum - 1u) / 2u) % 3u));   // TODO: tick-1 ? 

                    for(coreUintW j = 48u; j--; )
                    {
                        if(((j / iType) + (s_iTick & 0x01u)) & 0x01u) continue;

                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 23.5f) * 5.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.15f);
                    }
                }
            }
        });

        STAGE_WAVE("VIERZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // ghost appears for few seconds (zelda wizzrobe)
    // keine einzelnen gegner am anfang, sonst sieht der spieler die mechanik nicht, wenn er sie einfach über den haufen schießt
    // 1: two groups active at the same time to continue with changes while keeping enemies active for a longer time
    // 1: two different invincible states, hidden and flickering, to give the player perception of change while he has to wait (loadbar)
    // 3: pattern should not have too long straight lines, as this makes it too easy, and should have no corners, as this creates placement artifacts
    // 1: diamond shape to not be able to kill 2 groups at the same time
    // TODO: maybe just a circle shape in last sub-stage (or double-circle)
    STAGE_MAIN({TAKE_ALWAYS, 16u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(9u);
            pPath1->AddNode(coreVector2( 0.0f,  0.0f),  coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.45f,-0.45f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.0f, -0.9f),  coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.45f,-0.45f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.0f,  0.0f),  coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.45f, 0.45f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.0f,  0.9f),  coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.45f, 0.45f), coreVector2( 0.0f,-1.0f));
            pPath1->AddLoop();
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 52u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        constexpr coreUint64 iGroupA     = 0b00000000'00000000'0000'0000'0000'1111u;
        constexpr coreUint64 iGroupB     = 0b00000000'00000000'0000'0000'1111'0000u;
        constexpr coreUint64 iGroupC     = 0b00000000'00000000'0000'1111'0000'0000u;
        constexpr coreUint64 iGroupD     = 0b00000000'00000000'1111'0000'0000'0000u;
        constexpr coreUint64 iGroupE     = 0b00000000'11111111'0000'0000'0000'0000u;
        constexpr coreUint64 iGroupF     = 0b11111111'00000000'0000'0000'0000'0000u;
        constexpr coreUint32 iSnakeCount = 6u;

        STAGE_GET_START(9u + iSnakeCount)
            STAGE_GET_UINT64    (iStateActive)
            STAGE_GET_UINT64    (iStateGhost)
            STAGE_GET_FLOAT     (fChangeTime)
            STAGE_GET_UINT      (iChangeCount)
            STAGE_GET_UINT_ARRAY(aiSnakeIndex, iSnakeCount)
            STAGE_GET_UINT      (iSnakeShift,  iSnakeShift = iSnakeCount)
            STAGE_GET_FLOAT     (fLastValue)
            STAGE_GET_UINT      (iLastDual)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 15u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 16u, 31u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 32u, 51u)

            fChangeTime  = 0.0f;
            iChangeCount = 0u;
        }

        const coreFloat  fChangeTimePrev  = fChangeTime;
        const coreUint32 iChangeCountPrev = iChangeCount;
        fChangeTime += 1.0f * Core::System->GetTime();

        if(m_iStageSub == 1u)
        {
            if(FRACT(fChangeTime * 1.1f) < FRACT(fChangeTimePrev * 1.1f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 4u)
                {
                default: ASSERT(false)
                case 0u: iStateActive &= ~iGroupC; iStateGhost = iGroupD; break;
                case 1u: iStateActive &= ~iGroupA; iStateGhost = iGroupC; break;
                case 2u: iStateActive &= ~iGroupB; iStateGhost = iGroupA; break;
                case 3u: iStateActive &= ~iGroupD; iStateGhost = iGroupB; break;
                }
            }
        }
        else if(m_iStageSub == 2u)
        {
            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: ASSERT(false)
                case 0u: iStateActive &= ~iGroupF; iStateGhost = iGroupF; break;
                case 1u: iStateActive &= ~iGroupE; iStateGhost = iGroupE; break;
                }
            }
        }
        else if(m_iStageSub == 3u)
        {
            if(FRACT(fChangeTime * 5.0f) < FRACT(fChangeTimePrev * 5.0f))
            {
                iChangeCount += 1u;

                if(iSnakeShift >= 2u)
                {
                    REMOVE_BIT(iStateActive, aiSnakeIndex[iSnakeShift - 1u])
                    REMOVE_BIT(iStateGhost,  aiSnakeIndex[iSnakeShift - 1u])
                    ADD_BIT   (iStateActive, aiSnakeIndex[iSnakeShift - 2u])

                    const coreUint32 iNewShift = MIN(pSquad1->GetNumEnemiesAlive(), iSnakeCount);
                    if(iSnakeShift != iNewShift)
                    {
                        iSnakeShift = iNewShift;
                    }
                    else
                    {
                        std::memmove(aiSnakeIndex + 1u, aiSnakeIndex, sizeof(coreUint32) * (iSnakeShift - 1u));
                        aiSnakeIndex[0] = 0u;

                        for(coreUintW i = 32u; i < 52u; ++i)
                        {
                            if(!CONTAINS_FLAG(pSquad1->GetEnemy(i)->GetStatus(), ENEMY_STATUS_DEAD) && !CONTAINS_BIT(iStateGhost, i))
                            {
                                ADD_BIT(iStateGhost, i)
                                aiSnakeIndex[0] = i;

                                break;
                            }
                        }
                    }
                }
                else
                {
                    aiSnakeIndex[0] = 0u;
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(iChangeCount != iChangeCountPrev)
            {
                if(i < 16u)
                {
                    const coreUintW x  = ((i / 4u) % 2u);
                    const coreUintW y  = ((i / 4u) / 2u) % 2u;
                    const coreUintW x2 = (i % 2u);
                    const coreUintW y2 = (i / 2u) % 2u;

                    const coreVector2 vPos = 0.75f * coreVector2(-0.5f + 1.0f * I_TO_F(x), -0.5f + 1.0f * I_TO_F(y)) + 0.25f * coreVector2(-0.5f + 1.0f * I_TO_F(x2), -0.5f + 1.0f * I_TO_F(y2));

                    pEnemy->SetPosition(coreVector3(vPos.Rotated45() * FOREGROUND_AREA, 0.0f));
                }
                else if(i < 32u)
                {
                    const coreBool bVertical = (i < 24u);
                    const coreBool bGroup    = (iChangeCount % 2u);
                    if(bVertical == bGroup)
                    {
                        const coreFloat   fSide   = -0.95f + 1.9f/7.0f * I_TO_F((i - 16u) % 8u);
                        const coreVector2 vTarget = pEnemy->NearestPlayerDual((i / 8u) % 2u)->GetPosition().xy();

                        if(bVertical) pEnemy->SetPosition(coreVector3(vTarget.x, fSide * FOREGROUND_AREA.y, 0.0f));
                                 else pEnemy->SetPosition(coreVector3(fSide * FOREGROUND_AREA.x, vTarget.y, 0.0f));
                    }
                }
                else
                {
                    if(aiSnakeIndex[0] == i)
                    {
                        const coreFloat   fOffset = (1.0f/5.0f) * I_TO_F(iSnakeCount - iSnakeShift);
                        const coreVector2 vPos    = pPath1->CalcPosition(FMOD(fChangeTime - fOffset, pPath1->GetTotalDistance()));

                        pEnemy->SetPosition(coreVector3(-vPos.Rotated90() * FOREGROUND_AREA, 0.0f));
                    }
                }

                if(CONTAINS_BIT(iStateActive, i))
                {
                    if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                    {
                        pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                        pEnemy->SetAlpha    (1.0f);
                        pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                        const coreVector2 vPos = pEnemy->GetPosition ().xy();
                        const coreVector2 vDir = pEnemy->GetDirection().xy();
                        const coreVector2 vTan = vDir.Rotated90();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.25f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.25f);
                    }
                }
                else
                {
                    if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                    {
                        pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                        pEnemy->SetAlpha    (0.0f);
                        pEnemy->AddStatus   (ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                    }
                }
            }

            if(!CONTAINS_BIT(iStateActive, i) && CONTAINS_BIT(iStateGhost, i))
            {
                pEnemy->SetDirection(coreVector3(((i < 32u) ? pEnemy->AimAtPlayerDual((i / 8u) % 2u) : pEnemy->AimAtPlayerSide()).Normalized(), 0.0f));
                pEnemy->SetAlpha    ((FRACT(fChangeTime * 20.0f) < 0.5f) ? 0.9f : 0.4f);
                pEnemy->RemoveStatus(ENEMY_STATUS_HIDDEN);
            }

            if((m_iStageSub == 3u) && (pSquad1->GetNumEnemiesAlive() == 1u))
            {
                if(!fLastValue) iLastDual = pEnemy->NearestPlayerSide() - g_pGame->GetPlayer(0u);

                fLastValue += 0.8f * Core::System->GetTime();

                if(fLastValue >= 1.8f)
                {
                    pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * (320.0f * (fLastValue - 1.8f + 0.25f) * Core::System->GetTime()));
                }
                else
                {
                    coreVector2 vPos = pEnemy->GetPosition().xy();
                    coreVector2 vDir = pEnemy->AimAtPlayerDual(iLastDual).Normalized();

                         if(fLastValue >= 1.5f) vDir *= coreMatrix3::Rotation(LERPB(0.0f*PI, -1.0f*PI, (fLastValue - 1.5f) / 0.3f)).m12();
                    else if(fLastValue >= 1.0f) vPos += vDir * (60.0f * (fLastValue - 1.5f) * Core::System->GetTime());

                    pEnemy->SetPosition (coreVector3(CLAMP(vPos.x, -FOREGROUND_AREA.x, FOREGROUND_AREA.x), CLAMP(vPos.y, -FOREGROUND_AREA.y, FOREGROUND_AREA.y), 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir, 0.0f));
                }

                if(!g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()))
                {
                    STAGE_BADGE(BADGE_NORMAL, pEnemy->GetPosition())
                    pEnemy->Kill(true);
                }
            }
        });

        STAGE_WAVE("FÜNFZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // boss 3
    //STAGE_MAIN({TAKE_ALWAYS, 17u})
    //{
    //    STAGE_BOSS(m_Vaus, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // end
    STAGE_MAIN({TAKE_MISSION})
    {
        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // ################################################################
}