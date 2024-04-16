///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////


// ****************************************************************
// setup the Virido mission
void cViridoMission::__SetupOwn()
{
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
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(-cGrassBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));

            g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeightNow(0.4583f, -13.83f);
            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(1u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(2u, 0.0f);
            g_pEnvironment->GetBackground()->SetDecalDensity (0u, 0.0f);
            g_pEnvironment->GetBackground()->SetAirDensity   (0u, 0.0f);

            g_pEnvironment->SetTargetSpeed(5.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // top, bottom, side groups creating bullet carpet
    // - all: bullet patterns provide a lot of safe space, so players have time to think
    // - 2: push players consistently away into safe space, to reduce confusion when trying to fly into or under enemies
    STAGE_MAIN
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
                    const coreVector2& vForce = pPlayer->GetForce();
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
            const coreVector2 vOffset = coreVector2(0.0f, (i >= 6u && i < 12u) ? -0.1f : ((i < 24u) ? 0.0f : ((i < 30u) ? -0.1f : 0.1f)));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);

            if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                if(i < 24u) g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // shields on sides to force attacks from certain directions
    // - 3: starts with offset, to not fly into players
    // - 4: reflect all bullets and players with force, except with barrel roll
    // - 5: arranged to allow only one kill per turn, and to improve coop gameplay
    STAGE_MAIN
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

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 17u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_CYAN);
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
                STAGE_RESSURECT(pSquad1, 0u, 0u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f, 1.0f));
                this->EnableBarrier( 1u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESSURECT(pSquad1, 1u, 2u)
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 1u), coreVector2(-1.0f, 0.0f));
                this->EnableBarrier( 9u, pSquad1->GetEnemy( 1u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier(10u, pSquad1->GetEnemy( 2u), coreVector2( 1.0f, 0.0f));
                this->EnableBarrier(11u, pSquad1->GetEnemy( 2u), coreVector2( 0.0f, 1.0f));
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESSURECT(pSquad1, 3u, 6u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f, 1.0f));
                this->EnableBarrier( 1u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier( 2u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f, 1.0f));
                this->EnableBarrier( 3u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier( 4u, pSquad1->GetEnemy( 5u), coreVector2( 1.0f, 0.0f));
                this->EnableBarrier( 5u, pSquad1->GetEnemy( 5u), coreVector2(-1.0f, 0.0f));
                this->EnableBarrier( 6u, pSquad1->GetEnemy( 6u), coreVector2( 1.0f, 0.0f));
                this->EnableBarrier( 7u, pSquad1->GetEnemy( 6u), coreVector2(-1.0f, 0.0f));
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESSURECT(pSquad1, 7u, 12u)
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 7u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier( 9u, pSquad1->GetEnemy( 8u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier(10u, pSquad1->GetEnemy( 9u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier(11u, pSquad1->GetEnemy(10u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier(12u, pSquad1->GetEnemy(11u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier(13u, pSquad1->GetEnemy(12u), coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESSURECT(pSquad1, 13u, 16u)
                this->EnableBarrier( 4u, pSquad1->GetEnemy(13u), coreVector2(-1.0f, 0.0f));   // # render order
                this->EnableBarrier( 1u, pSquad1->GetEnemy(14u), coreVector2( 0.0f,-1.0f));
                this->EnableBarrier( 2u, pSquad1->GetEnemy(14u), coreVector2( 1.0f, 0.0f));
                this->EnableBarrier( 3u, pSquad1->GetEnemy(15u), coreVector2( 0.0f, 1.0f));
                this->EnableBarrier( 0u, pSquad1->GetEnemy(15u), coreVector2( 0.0f,-1.0f));   // # render order
                this->EnableBarrier( 5u, pSquad1->GetEnemy(15u), coreVector2(-1.0f, 0.0f));
                this->EnableBarrier( 6u, pSquad1->GetEnemy(16u), coreVector2( 0.0f, 1.0f));
                this->EnableBarrier( 7u, pSquad1->GetEnemy(16u), coreVector2( 1.0f, 0.0f));
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, (i >= 7u && i < 13u) ? (I_TO_F(i-7u) * 0.15f) : 0.0f)

            const coreSpline2* pPath = (i >= 13u) ? pPath2 : pPath1;

            const coreVector2 vFactor = coreVector2((i == 0u || i >= 7u) ? 0.0f : (((i % 2u) ? 1.0f : -1.0f) * ((i >= 3u) ? -0.5f : 1.0f)), (i == 0u || i >= 7u) ? ((i >= 13u) ? -1.0f : 1.0f) : ((i % 2u) ? 1.0f : -1.0f));
            const coreVector2 vOffset = coreVector2((i >= 7u) ? ((i >= 13u) ? ((I_TO_F((i-13u) % 2u) - 0.5f) * 0.38f) : ((I_TO_F(i-7u) - 2.5f) * 0.38f)) : 0.0f, (i >= 13u) ? (I_TO_F((i-13u) / 2u) * 0.38f) : ((i >= 7u) ? -0.05f : 0.0f));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);

            if(i == 1u || i == 2u || i == 3u || i == 4u) pEnemy->Rotate90();

            if(STAGE_LIFETIME_AFTER(1.5f) && STAGE_TICK_LIFETIME(1.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all barriers
    STAGE_MAIN
    {
        for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            this->DisableBarrier(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // enemies survive invincible
    // - all: delay attack, to get players accustomed
    // FAILURE 
    STAGE_MAIN
    {//bewegung entfernen, zu ähnlich wie 1, links rechts kreuzen                                                                               
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,1.0f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 44u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iShootOrder)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 19u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 20u, 43u)
        }

        coreBool  bActive    = false;
        cEnemy*   pTarget    = NULL;
        coreFloat fTargetVal = FLT_MAX;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 20u)
            {
                STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

                const coreFloat fPosX = I_TO_F(i % 10u) * 0.2f - 0.9f;

                if(STAGE_BRANCH(pPath1->GetTotalDistance(), 4.0f))
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(fPosX, 0.0f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                }
                else
                {
                    const coreFloat fValue = FMOD(fLifeTime + ((i < 10u) ? 1.0f : 0.0f) + 1.0f, 4.0f);
                    const coreFloat fPosY  = (fValue < 2.0f) ? LERPS(-1.0f, 1.0f, MIN(fValue, 1.0f)) : (LERPS(1.0f, -1.0f, MIN(fValue-2.0f, 1.0f)));

                    pEnemy->SetPosition(coreVector3(fPosX, fPosY, 0.0f) * FOREGROUND_AREA3);
                }

                if(i < 10u) pEnemy->Rotate90();
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 0.5f, 0.2f * I_TO_F((i-20u) % 12u) - LERPB(0.0f, 2.0f, MIN(pEnemy->GetLifeTime() * 0.5f, 1.0f)))

                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f, ((i-20u) < 12u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = coreVector2((((i-20u) < 12u) ? -1.0f : 1.0f) * (0.1f + LERPB(1.0f, 0.0f, MIN(pEnemy->GetLifeTime() * 0.5f, 1.0f))), 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(pEnemy->GetCurHealth())
            {
                bActive = true;
            }
            else
            {
                const coreFloat fCurValue = (i < 20u) ? pEnemy->GetPosition().arr((i < 10u) ? 1u : 0u) : pEnemy->GetPosition().xy().LengthSq();
                if(fCurValue < fTargetVal)
                {
                    pTarget    = pEnemy;
                    fTargetVal = fCurValue;
                }

                STATIC_ASSERT(FRAMERATE_VALUE == 60.0f)
            }
        });

        if(STAGE_TICK_TIME_FREE(2.0f, 0.0f))
        {
            cEnemy*     pEnemy;
            coreVector2 vPos;
            coreVector2 vDir;

            if(++iShootOrder >= 10u) iShootOrder = 0u;
            const coreBool  bType   = (iShootOrder % 2u) ? true : false;
            const coreFloat fHeight = I_TO_F((iShootOrder * 3u) % 10u) * 0.2f - 0.9f;

            if(m_iStageSub == 1u)
            {
                pEnemy = pSquad1->GetEnemy(bType ? 0u : 10u);
                vPos   = bType ? coreVector2(pEnemy->GetPosition().x, fHeight * FOREGROUND_AREA.y) : coreVector2(fHeight * FOREGROUND_AREA.x, pEnemy->GetPosition().y);
                vDir   = bType ? coreVector2(-SIGN(pEnemy->GetPosition().x), 0.0f)                 : coreVector2(0.0f, -SIGN(pEnemy->GetPosition().y));
            }
            else
            {
                pEnemy = pSquad1->GetEnemy(bType ? 20u : 32u);
                vPos   = coreVector2(pEnemy->GetPosition().x, fHeight * FOREGROUND_AREA.y);
                vDir   = coreVector2(SIGN(pEnemy->GetPosition().x), 0.0f);
            }

            if(pEnemy->GetLifeTime() >= 1.5f)
            {
                for(coreUintW i = 8u; i--; )
                {
                    const coreVector2 vOffset = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) + 0.5f) * 45.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, pEnemy, vPos + 5.0f * vOffset, vDir)->ChangeSize(1.2f);
                }
            }
        }

        if(!bActive)
        {
            ASSERT(pTarget)
            pTarget->Kill(true);

            if((m_iStageSub == 1u) && pSquad1->IsFinished())
            {
                iShootOrder = 0u;
                return;
            }
        }

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // big enemy splits up into smaller enemies on death
    // - all: cinders have low health, to make killing them with accidental hits easier
    // - all: cinders start at corners, to reduce accidental hits when enemies fly in
    // - all: arrows spawn with offset, to reduce accidental killing them before getting visible
    // - 2: moves around, to increase accidental hits during normal battle
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.3f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2( 0.0f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.8f,-1.3f), coreVector2(0.0f,1.0f));
            pPath2->AddNode(coreVector2(0.8f, 1.3f), coreVector2(0.0f,1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_RED);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy1, i)
        {
            if(pEnemy1->ReachedDeath())
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy2, j)
                {
                    if((j >= i*4u) && (j < (i+1u)*4u))
                    {
                        const coreVector3 vDir = coreVector3(StepRotated90X(j % 4u), 0.0f);

                        pEnemy2->SetPosition (pEnemy1->GetPosition() + vDir * 4.0f);
                        pEnemy2->SetDirection(vDir);

                        pEnemy2->Resurrect();
                    }
                });
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 4u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i == 0) ? 1.9f : 1.0f, 0.0f)

            if(i == 0u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f) * ((i % 2u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset, fLifeTime);

                if(i >= 3u) pEnemy->Rotate90();
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayer()->GetPosition().xy(), LERP(40.0f, 20.0f, MIN(pEnemy->GetLifeTime(), 1.0f)), 2.0f);

            if(STAGE_LIFETIME_AFTER(0.4f) && STAGE_TICK_LIFETIME(6.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, pEnemy, vPos, vDir)->ChangeSize(1.1f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // bullet attack explodes on border
    // FAILURE 
    STAGE_MAIN                         
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 3u)
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
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.0f)

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f, ((i < 2u) ? 1.0f : -1.0f));
            const coreVector2 vOffset = coreVector2(i ? ((i < 2u) ? 0.4f : -0.4f) : 0.0f, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i >= 1u) pEnemy->Rotate90();   

            pEnemy->SetDirection(coreVector3(1.0f,0.0f,0.0f));

            if(STAGE_TICK_LIFETIME(1.25f, 0.0f))//(i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();//(StepRotated90(s_iTick % 4u) * FOREGROUND_AREA - vPos).Normalized();////pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(0, 3.0f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(2.0f);
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            if(!g_pForeground->IsVisiblePoint(pBullet->GetPosition().xy()))
            {
                const coreVector2 vPos = pBullet->GetPosition().xy();

                for(coreUintW i = 20u; i--; )
                {
                    //if((i % 4u) < 1u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos,  vDir)->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos, -vDir)->ChangeSize(1.4f);
                    // TODO: lower purple balls get overlapped   
                }

                pBullet->Deactivate(false);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Dharuk, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fOldAngle, fOldAngle = g_pEnvironment->GetDirection().Angle())
        STAGE_GET_END

        g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(fOldAngle, 0.0f*PI, MIN(m_fStageTime * 0.5f, 1.0f))));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // enemy has weakpoint which he keeps away from player
    // - all: miners rotation start against fly direction, to make evading the initial attack easier
    // - 2: cinders in pair always rotate together away from player when one is shot from outer direction, to prevent ping-pong effect
    STAGE_MAIN
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
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cCinderEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_BROWN);
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

            const coreFloat fTurn = coreVector2::Dot(pEnemy->AimAtPlayer().Normalized(), vShootDir.Rotated90());

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
                const coreFloat fAngleTo   =  (i == 3u || i == 5u || i == 7u || i == 9u) ? (afTarget[i-1u] + (0.5f*PI)) : afTarget[i];
                const coreFloat fShift     = ((i == 3u || i == 5u || i == 7u || i == 9u) ?  afSign  [i-1u]              : afSign  [i]) * ABS(AngleDiff(fAngleTo, fAngleFrom));

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

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, COLL_VAL(pSquad1), COLL_VAL(pSquad2), COLL_VAL(afTarget), COLL_VAL(afSign), COLL_VAL(aiFreeState), COLL_VAL(aiFreeMove))
        {
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

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 1-2
        g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(0.0f*PI, -0.5f*PI, MIN(m_fStageTime * 0.5f, 1.0f))));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // explode into bullets on death
    // - all: number of bullet waves is uneven, to create a better pattern visually
    // - all: number of enemies is uneven, to let them cross the center alternately, but still in equal distance
    STAGE_MAIN
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

                    for(coreUintW j = 5u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) + 0.5f * I_TO_F(iNewTick % 2u)) * 36.0f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
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

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 2-3
        g_pEnvironment->SetTargetSpeed(LERPS(5.0f, -5.0f, MIN(m_fStageTime * 0.5f, 1.0f)));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // leave destroyable residue on its way (r-type leaver)
    // enemies cannot move on the same path in infinity as the bullets would just overlay or need some handling  
    // multiple following enemies with the same move pattern will overlay at some point  
    // FAILURE 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 14u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                //pEnemy->Configure(50, COLOR_SHIP_YELLOW);
                pEnemy->Configure(120, COLOR_SHIP_YELLOW);
            });

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3(-0.8f, 1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3( 0.8f,-1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(2u)->SetPosition(coreVector3(-1.2f, 0.0f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(3u)->SetPosition(coreVector3( 1.2f, 0.0f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(4u)->SetPosition(coreVector3(-1.2f,-0.8f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(5u)->SetPosition(coreVector3( 1.2f, 0.8f,0.0f) * FOREGROUND_AREA3);

            pSquad1->GetEnemy( 6u)->SetPosition(coreVector3( 0.0f,-1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy( 7u)->SetPosition(coreVector3( 0.0f, 1.2f,0.0f) * FOREGROUND_AREA3);

            pSquad1->GetEnemy( 8u)->SetPosition(coreVector3(-1.2f, 0.75f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy( 9u)->SetPosition(coreVector3(-1.2f,-0.45f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(10u)->SetPosition(coreVector3(-1.2f, 0.15f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(11u)->SetPosition(coreVector3( 1.2f,-0.15f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(12u)->SetPosition(coreVector3( 1.2f, 0.45f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(13u)->SetPosition(coreVector3( 1.2f,-0.75f,0.0f) * FOREGROUND_AREA3);


            pSquad1->GetEnemy(0u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(1u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy(2u)->SetDirection(coreVector3( 1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(3u)->SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(4u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(5u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));

            pSquad1->GetEnemy( 6u)->SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 7u)->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalized());

            pSquad1->GetEnemy( 8u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy( 9u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(10u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(11u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(12u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(13u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
        });

        if(STAGE_CLEARED)
        {
            //if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 8u, 13u)

           //     if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u,  1u)
           //else 
           //         if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 2u,  3u)
           //else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 4u,  7u)

                if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u,  0u)
           else 
                    if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 2u,  2u)
           else 
                        if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 5u,  6u)
            // if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 8u,  8u)

           // else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 8u,  11u)
           // else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1, 6u, 11u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const coreFloat fSpeed = LERP(1.5f, 1.5f, pEnemy->GetCurHealthPct());  

            //if(i >= 2u && i <= 5u)
            if(i < 8u)
            {
                const cPlayer*    pPlayer = pEnemy->NearestPlayer();
                const coreVector2 vDiff   = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();

                if(coreVector2::Dot(vDiff, pEnemy->GetDirection().xy()) <= 0.0f)
                {
                    if(coreVector2::Dot(vDiff, pEnemy->GetDirection().xy().Rotated90()) <= 0.0f) pEnemy->SetDirection(-pEnemy->GetDirection().RotatedZ90());
                                                                                            else pEnemy->SetDirection( pEnemy->GetDirection().RotatedZ90());
                }

                pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 20.0f * fSpeed);//(i % 2u) ? 35.0f : 25.0f);
            }
            else
            {
                pEnemy->DefaultMoveTarget(pEnemy->NearestPlayer()->GetPosition().xy(), 30.0f, 5.0f);
            }

             /*
            coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            coreVector2 vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) vCurPos.x =  ABS(vCurPos.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) vCurPos.x = -ABS(vCurPos.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) vCurPos.y =  ABS(vCurPos.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) vCurPos.y = -ABS(vCurPos.y);

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            */

            const coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            coreVector2       vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);

            pEnemy->SetDirection(coreVector3(vCurDir, 0.0f));


            if(STAGE_TICK_TIME_FREE(9.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                if(i >= 8u) g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>    (5, 0.0f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
                else if(coreMath::IsNear(vDir.x, 0.0f) || coreMath::IsNear(vDir.y, 0.0f))
                     g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos, vDir);
                else g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos, vDir);
            }
        });

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            //pBulletPlayer->Deactivate(true, vIntersection.xy());
            pBulletEnemy ->Deactivate(true, vIntersection.xy());
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 3-4
        g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(-0.5f*PI, 0.0f*PI, MIN(m_fStageTime * 0.5f, 1.0f))));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // dicke kugerl die am rand bleiben
    // - all: bullet speed is much lower than enemy speed, to create a perceptible path for the player to use
    // - all: bullet frequency is not too high, to better distinguish individual bullets
    // - all: enemy attack begins later, to not slow down bullets immediately after shooting
    // - 1,2: enemy path is not too wide, to not slow down bullets immediately after shooting
    STAGE_MAIN
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

            if(STAGE_LIFETIME_AFTER(((i == 3u) ? pPath3 : pPath1)->GetTotalDistance()) && STAGE_TICK_LIFETIME(6.0f, (i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

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

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 4-5
        g_pEnvironment->SetTargetSpeed(LERPS(-5.0f, 5.0f, MIN(m_fStageTime * 0.5f, 1.0f)));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // enemies jump into camera
    // - all: target shadow is essential, to communicate direct targeting in third sub-stage
    // - 1,2: bullet patterns provide a lot of safe space, as jumping into the camera may overwhelm the player
    STAGE_MAIN
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
                            avNewPos  [i] = pEnemy->NearestPlayer()->GetPosition().xy();
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

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all shadows
    STAGE_MAIN
    {
        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
            this->DisableShadow(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Torus, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // enemy charges straight forward on death (r-type fish)
    // - 2: should start one same spot an opposite of first group, to reduce accidental collision (which is more likely with 2x2)  
    // no passive attack, not impact attack, no bounce, because it's too overwhelming together with the green ball  
    // fire speed fast enough to create visual line  
    // 2x2 instead of 1x4, to force player keeping fire at the same position while an enemy is already flying at them  
    // TODO: final bullet explosion needs more than 1024 particles 
    STAGE_MAIN
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
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            }

            if(pEnemy->GetCurHealth() == 0)
            {
                afSpeed[i] += 80.0f * Core::System->GetTime();

                pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * ((20.0f + afSpeed[i]) * Core::System->GetTime()));

                if(STAGE_TICK_TIME(30.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (i < 16u) ? 0.5f : 0.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                }
            }
            else if(i < 8u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f + (((i % 4u) < 2u) ? -1.0f : 1.0f)) * 0.22f, ((i % 8u) < 4u) ? 0.0f : 0.22f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection   (coreVector3(pEnemy->AimAtPlayer().Normalized(), 0.0f));
            }
            else
            {
                const coreVector2 vFactor = coreVector2(1.0f, ((i % 8u) < 4u) ? 1.0f : -1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 2u) - 0.5f) * 0.22f, ((i % 4u) < 2u) ? 0.0f : 0.22f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(i >= 24u) pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy().Rotated90() * mRota, 0.0f));
                        else pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy()             * mRota, 0.0f));

                pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayer().Normalized(), 0.0f));
            }
        });

        STAGE_COLL_PLAYER_ENEMY(pPlayer, pEnemy, vIntersection)
        {
            if(pEnemy->GetCurHealth() == 0)
            {
                pPlayer->TakeDamage(15, ELEMENT_NEUTRAL, vIntersection.xy());
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // bend
    // enemies get bent away
    // enemies get pushed away (Mario)
    // enemies follow ball
    // enemies move around all the time and their direction is modified by ball
    STAGE_MAIN
    {
        if(STAGE_BEGINNING) this->EnableBall(0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized()); // TODO: adapt for single fight    


        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 40u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_BLUE);
                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i) * 2.0f/5.0f * PI);

               //if(!(i % 5u)) 
                if(i >= 0u && i <= 7u)
                    pEnemy->Configure(50, COLOR_SHIP_YELLOW);

                pEnemy->Resurrect();

            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT64(iNearBall)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUintW x = i % 8u;
            const coreUintW y = i / 8u;
            //const coreUintW x = i % 6u;
            //const coreUintW y = i / 6u;

            const coreBool bWide = (y % 2u) ? false : false;

            ///*const*/ coreVector2 vTarget = coreVector2(bWide ? (-0.9f + 0.25f * I_TO_F(x)) : (-0.7f + 0.2f * I_TO_F(x)), 0.8f - 0.4f * I_TO_F(y));//(y) ? 0.67f : -0.67f);
            /*const*/ coreVector2 vTarget = coreVector2(bWide ? (-0.9f + 0.25f * I_TO_F(x)) : (-0.7f + 0.2f * I_TO_F(x)), 0.9f - 0.25f * I_TO_F(y));//(y) ? 0.67f : -0.67f);
            //vTarget = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + 0.2f * I_TO_F(y) - 1.14f * (0.5f + 0.5f*SIN(m_fStageTime * 1.2f)));
            //vTarget = vTarget.Rotated90();
            //vTarget = coreVector2(-0.5f + 0.2f * I_TO_F(x), 0.8f);
            //if(y >= 1) vTarget = vTarget.Rotated90();
            //if(y >= 2) vTarget = vTarget.Rotated90();
            //if(y >= 3) vTarget = vTarget.Rotated90();

            //const coreMatrix2 vRota = coreMatrix3::Rotation(m_fStageTime).m12();
            //vTarget = vTarget * vRota;

            if(STAGE_TAKEOFF)
            {
               // pEnemy->SetPosition(coreVector3((vTarget + coreVector2(0.0f, 1.0f + 0.12f * I_TO_F(x))) * FOREGROUND_AREA, 0.0f));
                pEnemy->SetPosition(coreVector3((vTarget) * FOREGROUND_AREA, 0.0f));
            }

            // pEnemy->DefaultMoveSmooth(vTarget, 40.0f, 10.0f);  

            #if 1
            const coreObject3D& oBall = m_aBallRaw[0];
            if(oBall.IsEnabled(CORE_OBJECT_ENABLE_ALL))
            {
                const coreVector2 vDiff  = pEnemy->GetPosition().xy() - oBall.GetPosition().xy();
                const coreVector2 vDodge = vDiff.IsNull() ? coreVector2(0.0f,1.0f) : vDiff.Normalized();
                const coreFloat   fLen   = vDiff.IsNull() ? 100.0f : vDiff.Length();
                const coreFloat   fStr   = MAX(20.0 - fLen, 0.0) / 20.0f;
                const coreFloat   fDot   = MAX(coreVector2::Dot(vDodge, oBall.GetDirection().xy()), 0.0f);

                const coreVector2 vNewTarget = LERP(vTarget * FOREGROUND_AREA, pEnemy->GetPosition().xy() + vDodge * 35.0f, fStr);
                const coreVector2 vMove      = vNewTarget - pEnemy->GetPosition().xy();

#if 0
                const coreBool bNowNearBall = (fLen < 15.0f) ? true : false;
                if(bNowNearBall && !CONTAINS_BIT(iNearBall, i))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    //const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();
                    //const coreVector2 vDir = coreVector2(0.0f,1.0f);
                    const coreVector2 vDir = oBall.GetDirection().xy().Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.3f); 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, -vDir)->ChangeSize(1.3f); 
                }
                SET_BIT(iNearBall, i, bNowNearBall)
#endif

                pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + vMove * ((fStr * fDot * 9.0f + 2.0f) * Core::System->GetTime()), 0.0f));

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
                {
                    if(pEnemy == pEnemy2) return;

                    const coreVector2 vDiff2 = pEnemy->GetPosition().xy() - pEnemy2->GetPosition().xy();
                    const coreFloat   fPower = POW2(pEnemy->GetCollisionRadius() * 2.0f) - vDiff2.LengthSq();

                    if(fPower > 0.0f)
                    {
                        const coreFloat A = (pEnemy ->GetPosition().xy() - oBall.GetPosition().xy()).LengthSq();
                        const coreFloat B = (pEnemy2->GetPosition().xy() - oBall.GetPosition().xy()).LengthSq();

                        const coreVector2 vPush = vDiff2.Normalized() * (fPower * Core::System->GetTime());

                        if(A > B) pEnemy ->SetPosition(coreVector3(pEnemy ->GetPosition().xy() + vPush, 0.0f));
                             else pEnemy2->SetPosition(coreVector3(pEnemy2->GetPosition().xy() - vPush, 0.0f));
                    }
                });

                coreVector2 vCurPos = pEnemy->GetPosition ().xy();

                vCurPos.x = CLAMP(vCurPos.x, -FOREGROUND_AREA.x, FOREGROUND_AREA.x);
                vCurPos.y = CLAMP(vCurPos.y, -FOREGROUND_AREA.y, FOREGROUND_AREA.y);

                pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            }
            #endif

            #if 0
            coreVector2 vOffset = coreVector2(0.0f,0.0f);
            coreUintW iCount = 1u;
            coreFloat fLenSq = FLT_MAX;

            g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                /*
                coreVector2     vDiff  = vOffset - pBullet->GetPosition().xy(); vDiff.y *= 0.0f;
                const coreFloat fLenSq = vDiff.LengthSq();

                if(fLenSq < 100.0f)
                {
                    vOffset += vDiff.Normalized() * SQRT(100.0f - fLenSq);
                }
                */
                //vOffset = vOffset * (I_TO_F(iCount-1u)/I_TO_F(iCount)) + pBullet->GetPosition().xy() * (1.0f/I_TO_F(iCount));
                //iCount++;

                const coreVector2 vDiff = pEnemy->GetPosition().xy() - pBullet->GetPosition().xy();
                if(vDiff.LengthSq() < fLenSq)
                {
                    vOffset = pBullet->GetPosition().xy() * pBullet->GetDirection().yx().Processed(ABS) + pEnemy->GetPosition().xy() * pBullet->GetDirection().xy().Processed(ABS);
                    fLenSq = vDiff.LengthSq();
                }
            });


            const coreObject3D& oBall = m_aBallRaw[0];
            if(oBall.IsEnabled(CORE_OBJECT_ENABLE_ALL))
            {
                const coreVector2 vDiff = pEnemy->GetPosition().xy() - vOffset/*oBall.GetPosition().xy()*/;
                const coreVector2 vDir  = vDiff.Normalized();
                const coreFloat   fLen  = vDiff.Length();
                const coreFloat   fStr  = MAX(20.0 - fLen, 0.0) / 20.0f;
                const coreFloat   fDot  = 1.0f;//MAX(coreVector2::Dot(vDir, oBall.GetDirection().xy()), 0.0f);

                const coreVector2 vNewTarget = LERP(vTarget * FOREGROUND_AREA, pEnemy->GetPosition().xy() + vDir * 15.0f, fStr);
                const coreVector2 vMove      = vNewTarget - pEnemy->GetPosition().xy();

                pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + vMove * ((fStr * fDot * 9.0f + 2.0f) * Core::System->GetTime()), 0.0f));

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
                {
                    if(pEnemy == pEnemy2) return;

                    const coreVector2 vDiff2 = pEnemy->GetPosition().xy() - pEnemy2->GetPosition().xy();
                    const coreFloat   fPower = POW2(pEnemy->GetCollisionRadius() * 2.0f) - vDiff2.LengthSq();

                    if(fPower > 0.0f)
                    {
                        const coreFloat A = (pEnemy ->GetPosition().xy() - vOffset/*oBall.GetPosition().xy()*/).LengthSq();
                        const coreFloat B = (pEnemy2->GetPosition().xy() - vOffset/*oBall.GetPosition().xy()*/).LengthSq();

                        const coreVector2 vPush = vDiff2.Normalized() * (fPower * Core::System->GetTime());

                        if(A > B) pEnemy ->SetPosition(coreVector3(pEnemy ->GetPosition().xy() + vPush, 0.0f));
                             else pEnemy2->SetPosition(coreVector3(pEnemy2->GetPosition().xy() - vPush, 0.0f));
                    }
                });

                coreVector2 vCurPos = pEnemy->GetPosition ().xy();

                vCurPos.x = CLAMP(vCurPos.x, -FOREGROUND_AREA.x, FOREGROUND_AREA.x);
                vCurPos.y = CLAMP(vCurPos.y, -FOREGROUND_AREA.y, FOREGROUND_AREA.y);

                pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            }
            Core::Debug->InspectValue("Offset", vOffset);
            #endif
            



            //if(!(i % 5u) && STAGE_TICK_TIME(1.2f, 0.0f))
            if(
               // !(i % 5u) && 
                (i >= 0u && i <= 7u) &&
               STAGE_TICK_TIME(15.0f, 0.0f) && ((s_iTick % 15u) < 3u))
            {

                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });


        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // push
    STAGE_START_HERE
    STAGE_MAIN
    {
        if(STAGE_BEGINNING) this->EnableBall(0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized()); // TODO: adapt for single fight    

        //STAGE_ADD_PATH(pPath1)
        //{    
        //    pPath1->Reserve(2u);
        //    pPath1->AddNode(coreVector2(1.2f,0.0f), coreVector2(-1.0f,0.0f));
        //    pPath1->AddStop(coreVector2(0.8f,0.0f), coreVector2(-1.0f,0.0f));
        //    pPath1->Refine();
        //});

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_BLUE);
                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));
            });

            //pSquad1->GetEnemy(0u)->SetPosition(coreVector3( 1.2f, 0.8f,0.0f) * FOREGROUND_AREA3);
            //pSquad1->GetEnemy(1u)->SetPosition(coreVector3( 1.2f, 0.6f,0.0f) * FOREGROUND_AREA3);
            //pSquad1->GetEnemy(2u)->SetPosition(coreVector3( 1.2f, 0.4f,0.0f) * FOREGROUND_AREA3);
            //pSquad1->GetEnemy(3u)->SetPosition(coreVector3( 1.2f, 0.2f,0.0f) * FOREGROUND_AREA3);
            //pSquad1->GetEnemy(4u)->SetPosition(coreVector3(-1.2f,-0.2f,0.0f) * FOREGROUND_AREA3);
            //pSquad1->GetEnemy(5u)->SetPosition(coreVector3(-1.2f,-0.4f,0.0f) * FOREGROUND_AREA3);
            //pSquad1->GetEnemy(6u)->SetPosition(coreVector3(-1.2f,-0.6f,0.0f) * FOREGROUND_AREA3);
            //pSquad1->GetEnemy(7u)->SetPosition(coreVector3(-1.2f,-0.8f,0.0f) * FOREGROUND_AREA3);

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3( 1.2f, 0.8f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3( 1.2f, 0.6f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(2u)->SetPosition(coreVector3( 1.2f,-0.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(3u)->SetPosition(coreVector3( 1.2f,-0.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(4u)->SetPosition(coreVector3(-1.2f, 0.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(5u)->SetPosition(coreVector3(-1.2f, 0.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(6u)->SetPosition(coreVector3(-1.2f,-0.6f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(7u)->SetPosition(coreVector3(-1.2f,-0.8f,0.0f) * FOREGROUND_AREA3);
        });

        STAGE_GET_START(33u)
            STAGE_GET_VEC2_ARRAY(avForce, 16u, for(coreUintW i = 0u; i < 8u; ++i) avForce[i] = coreVector2(((i < 4u) ? -40.0f : 40.0f), 0.0f);)
            STAGE_GET_UINT      (iForceDir)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, COLL_VAL(pSquad1), COLL_VAL(avForce), COLL_REF(iForceDir))
        {
            const coreUintW i = pSquad1->GetIndex(pEnemy);
            
            for(coreUintW j = coreMath::FloorAlign(i, 4u), je = coreMath::CeilAlign(i + 1u, 4u); j < je; ++j)
            {
                avForce[j] += 7.0f * coreVector2((CONTAINS_BIT(iForceDir, j) ? -1.0f : 1.0f) * ((i < 4u) ? -1.0f : 1.0f), 0.0f);
            }

            //if(i < 8u) avForce[i] += 10.0f * coreVector2((CONTAINS_BIT(iForceDir, i) ? -1.0f : 1.0f) * ((i < 4u) ? -1.0f : 1.0f), 0.0f);
            //      else avForce[i] +=  6.0f * -AlongCrossNormal(pBullet->GetFlyDir());
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  8u,  9u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 10u, 15u)
        }

        /*
        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.3f * I_TO_F(i % 2u))

            const coreVector2 vFactor = coreVector2((i < 4u) ? 1.0f : -1.0f, (i < 4u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, I_TO_F(i % 4u) * 0.25f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });
        */
        

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const coreVector2 vOldPos = pEnemy->GetPosition().xy();
            coreVector2       vNewPos = vOldPos;

            if(!avForce[i].IsNull())
            {
                vNewPos    += avForce[i] * Core::System->GetTime();
                avForce[i] *= FrictionFactor(3.0f);
            }

                 if((vNewPos.x < -FOREGROUND_AREA.x) && (avForce[i].x < 0.0f)) {vNewPos.x = -FOREGROUND_AREA.x; avForce[i].x =  ABS(avForce[i].x); if(i < 8u) TOGGLE_BIT(iForceDir, i)}
            else if((vNewPos.x >  FOREGROUND_AREA.x) && (avForce[i].x > 0.0f)) {vNewPos.x =  FOREGROUND_AREA.x; avForce[i].x = -ABS(avForce[i].x); if(i < 8u) TOGGLE_BIT(iForceDir, i)}
                 if((vNewPos.y < -FOREGROUND_AREA.y) && (avForce[i].y < 0.0f)) {vNewPos.y = -FOREGROUND_AREA.y; avForce[i].y =  ABS(avForce[i].y); if(i < 8u) TOGGLE_BIT(iForceDir, i)}
            else if((vNewPos.y >  FOREGROUND_AREA.y) && (avForce[i].y > 0.0f)) {vNewPos.y =  FOREGROUND_AREA.y; avForce[i].y = -ABS(avForce[i].y); if(i < 8u) TOGGLE_BIT(iForceDir, i)}

            pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));

#if 1
            if((vOldPos.x > -FOREGROUND_AREA.x) &&
               (vOldPos.x <  FOREGROUND_AREA.x) &&
               (vOldPos.y > -FOREGROUND_AREA.y) &&
               (vOldPos.y <  FOREGROUND_AREA.y))
            {
                constexpr coreFloat fFactor = 0.15f;
                const coreVector2 vOldTick = ((vOldPos + FOREGROUND_AREA) * fFactor).Processed(FLOOR);
                const coreVector2 vNewTick = ((vNewPos + FOREGROUND_AREA) * fFactor).Processed(FLOOR);

                const coreVector2 vStep = (vNewTick - vOldTick).Processed(SIGN);


                for(coreUintW j = F_TO_UI(vOldTick.x), je = F_TO_UI(vNewTick.x); j != je; j += F_TO_SI(vStep.x))
                {
                    const coreVector2 vPos = coreVector2((I_TO_F(j) / fFactor) - FOREGROUND_AREA.x, pEnemy->GetPosition().y);
                    //const coreVector2 vDir = coreVector2(0.0f,1.0f);
                    const coreVector2 vDir = (g_pGame->FindPlayer(vPos)->GetPosition().xy() - vPos).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                }
                for(coreUintW j = F_TO_UI(vOldTick.y), je = F_TO_UI(vNewTick.y); j != je; j += F_TO_SI(vStep.y))
                {
                    const coreVector2 vPos = coreVector2(pEnemy->GetPosition().x, (I_TO_F(j) / fFactor) - FOREGROUND_AREA.y);
                    //const coreVector2 vDir = coreVector2(1.0f,0.0f);
                    const coreVector2 vDir = (g_pGame->FindPlayer(vPos)->GetPosition().xy() - vPos).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);

                    //for(coreUintW i = 4u; i--; )
                    //{
                    //    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 45.0f + 22.5f));

                    //    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    //    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, -vDir)->ChangeSize(1.4f);
                    //}
                }
            }

#endif
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})

        // 4-6 on right side (or 50% left and right 3+3), going fixed to left, then right and will die there
        //       * //       * //
        //       * // *       //
        //       * //       * //
        // *       // *       //
        // *       //       * //
        // *       // *       //
        // 1-2 single will follow against shoot direction
        // block or cross follow against shoot direction

        // pushing enemies are on left and right side and shooting on them will move them to the other side, they die when reaching the other side (is only second wave?)   
    });

    // ################################################################
    // can only kill in order
    // coop last enemy, need to work together   
    // abwechselnd links rechts 
    // doppelgruppe nur breites pattern um ausweichen zu erleichtern 
    // reihenfolge am weitesten entfernt 
    STAGE_MAIN
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
                if(STAGE_TICK_TIME_FREE((iChiefNum == 9u) ? 3.0f : 1.1f, fShotOffset))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();
                    const coreUintW   iType = 4u - ((iChiefNum == 9u) ? (s_iTick % 3u) : (((iChiefNum - 1u) / 2u) % 3u));

                    for(coreUintW j = 48u; j--; )
                    {
                        if(((j / iType) + (s_iTick & 0x01u)) & 0x01u) continue;

                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 23.5f) * 5.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.15f);
                    }
                }
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // invincibility switches every few seconds
    STAGE_MAIN
    {
        // special already, ball gets caught! in every group, to change pattern!!!  
        // diamond in r-type fish 
        // boss taucht dann von unten auf (drehungs-effekt), und banner inkl breakout steine(!) von oben geht los
        if(STAGE_BEGINNING) this->EnableBall(0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized()); // TODO: adapt for single fight    

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 60u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(I_TO_F(i - 2u) * 0.2f, 1.1f, 0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (4, COLOR_SHIP_GREY);
                pEnemy->AddStatus  (ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_GET_START(3u)
            STAGE_GET_UINT64(iState, iState = 0b0000'000000'000000'000000'000000'111111000000'000000111111'101010010101)
            STAGE_GET_UINT  (iCount)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 11u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 12u, 35u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 36u, 59u)
        }

        const coreBool bSwitch     = (FRACT(m_fStageTime)      < FRACT(m_fStageTimeBefore))      ? true : false;
        const coreBool bSwitchFast = (FRACT(m_fStageTime*4.0f) < FRACT(m_fStageTimeBefore*4.0f)) ? true : false;

        if(m_iStageSub < 3u)
        {
            if(bSwitch)
            {
                iState = ~iState;
            }
        }
        else
        {
            if(bSwitchFast)
            {
                iCount += 1u;
                iState = BIT((iCount % 24u) + 36u) | BIT(((iCount + 12u) % 24u) + 36u);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.2f * I_TO_F(i % 6u))

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((I_TO_F(i % 6u) - 2.5f) * 0.3f + ((i < 12u || i >= 36u) ? 0.0f : 0.25f), (((i % 12u) < 6u) && (i < 36u)) ? 0.0f : 0.22f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);

                 if(i < 12u) pEnemy->Rotate90 ();
            else if(i < 24u) pEnemy->Rotate270()->InvertY();
            else if(i < 42u) ;
            else if(i < 48u) pEnemy->Rotate90 ();
            else if(i < 54u) pEnemy->Rotate180();
            else if(i < 60u) pEnemy->Rotate270();

            if((i < 36u) ? bSwitch : bSwitchFast)
            {
                if(CONTAINS_BIT(iState, i))
                {
                    pEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 0.8f);
                    pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 5.0f, 3u, COLOR_ENERGY_ORANGE);

                    const coreVector2 vPos  = pEnemy->GetPosition().xy();
                    //const coreFloat   fBase = pEnemy->AimAtPlayer().Angle();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();

                    for(coreUintW j = 6u; j--; )
                    {
                        if(j < 2u || j > 3u) continue;

                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.5f) * 3.5f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
                    }
                }
                else
                {
                    pEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                    pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                    pEnemy->AddStatus   (ENEMY_STATUS_INVINCIBLE);
                }
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})

        // right side, cross pattern two lines
        // left and upper side, two lines, each line own color, inverted for upper
        // around pattern, current active state flies around in circle (two states or long state)
            // abwechselnd, drehung, gegenseitig schützen
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        UNUSED STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 24u)//18u)//VAUS_SCOUTS_X * VAUS_SCOUTS_Y)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_GREY);
            });
        });

        STAGE_BOSS(m_Vaus, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // ################################################################









    /*
    STAGE_START_HERE
    STAGE_MAIN
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
        }

        if((m_iStageSub == 2u) && (pSquad1->GetNumEnemiesAlive() == 6u))
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->GetPosition().y < 0.0f)
                {
                    const coreVector2& vForce = pPlayer->GetForce();
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
            const coreVector2 vOffset = coreVector2(0.0f, (i >= 6u && i < 12u) ? -0.1f : ((i < 24u) ? 0.0f : ((i < 30u) ? -0.1f : 0.1f)));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);

            if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                if(i < 24u) g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    */
    STAGE_START_HERE
    STAGE_MAIN
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
        }

        if((m_iStageSub == 2u) && (pSquad1->GetNumEnemiesAlive() == 6u))
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->GetPosition().y < 0.0f)
                {
                    const coreVector2& vForce = pPlayer->GetForce();
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
            const coreVector2 vOffset = coreVector2(0.0f, (i >= 6u && i < 12u) ? -0.1f : ((i < 24u) ? 0.0f : ((i < 30u) ? -0.1f : 0.1f)));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);

            if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                if(i < 24u) g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("The way to insanity", {20.0f, 30.0f, 40.0f, 50.0f})
    });








}