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
cTinkerMission::cTinkerMission()noexcept
{
    // 
    m_apBoss[0] = &m_ProjectOne;
    m_apBoss[1] = &m_ProjectOne;
    m_apBoss[2] = &m_ProjectOne;
}


// ****************************************************************
// setup the Tinker mission
void cTinkerMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetSpeed(4.0f);
            g_pGame->StartIntro();
        }

        STAGE_FINISH_AFTER(GAME_INTRO_DURATION)
    });

    // OTHER: 
    // killing enemy creates a bullet line you need to roll, 1 enemy, 1 enemy, 2 enemies, 3/4 enemies 

    // ################################################################
    // infinity flying enemy shooting on teleport
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, COLOR_SHIP_ORANGE);

                pEnemy->SetPosition (coreVector3( 0.5f, 1.2f,0.0f) * FOREGROUND_AREA3);
                pEnemy->SetSize     (coreVector3( 1.3f, 1.3f,1.3f));
                pEnemy->SetDirection(coreVector3(-1.0f,-2.0f,0.0f).Normalized());
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 40.0f);

            const auto nShootFunc = [&](const coreVector2& vPos, const coreVector2& vDir)
            {
                const coreVector2 vTan = vDir.Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos + vTan * 6.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos + vTan * 2.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos - vTan * 2.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos - vTan * 6.0f, vDir);
            };

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) {nShootFunc(vCurPos, coreVector2( 1.0f, 0.0f)); vCurPos.x =  ABS(vCurPos.x); nShootFunc(vCurPos, coreVector2(-1.0f, 0.0f));}
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) {nShootFunc(vCurPos, coreVector2(-1.0f, 0.0f)); vCurPos.x = -ABS(vCurPos.x); nShootFunc(vCurPos, coreVector2( 1.0f, 0.0f));}
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) {nShootFunc(vCurPos, coreVector2( 0.0f, 1.0f)); vCurPos.y =  ABS(vCurPos.y); nShootFunc(vCurPos, coreVector2( 0.0f,-1.0f));}
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) {nShootFunc(vCurPos, coreVector2( 0.0f,-1.0f)); vCurPos.y = -ABS(vCurPos.y); nShootFunc(vCurPos, coreVector2( 0.0f, 1.0f));}

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
        });

        STAGE_WAVE
    });

    // ################################################################
    // split by bullets into four areas and teach turn and roll 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_YELLOW);
                pEnemy->SetPosition(coreVector3(coreVector2(1.2f,1.2f) * FOREGROUND_AREA, 0.0f));
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cMinerEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                const coreVector2 vDir = StepRotated90(i);

                pEnemy->Configure(20, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
                pEnemy->Resurrect();
            });
        });

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 3u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 4u, 7u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, I_TO_F(i) * 0.2f)

            if(STAGE_TAKEOFF)
            {
               // pEnemy->SetPosition(coreVector3(coreVector2(1.2f,1.2f) * FOREGROUND_AREA, 0.0f));
            }

            if(STAGE_LIFETIME_AFTER(0.0f))
            {
                const coreVector2 vPos = coreVector2((i % 2u) ? -1.0f : 1.0f, (i / 2u) ? -1.0f : 1.0f) * 0.1f + coreVector2(0.8f,0.8f);

                pEnemy->DefaultMoveSmooth(vPos, 10.0f, 1.0f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, I_TO_F(i) * 0.2f)

            const coreVector2 vFrom = (i ? coreVector2(1.3f,1.2f) : coreVector2(1.2f,1.3f));
            const coreVector2 vTo   = (i ? coreVector2(1.3f,0.0f) : coreVector2(0.0f,1.3f));

            pEnemy->DefaultMoveLerp(vFrom, vTo, LERP(0.0f, 1.0f, MIN(fLifeTime * 0.6f, 1.0f)));

            if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = (ABS(vPos.x) > ABS(vPos.y)) ? coreVector2(-SIGN(vPos.x), 0.0f) : coreVector2(0.0f, -SIGN(vPos.y));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos, vDir);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // turning and constantly shooting enemy
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, COLOR_SHIP_CYAN);
                pEnemy->SetSize(coreVector3(2.0f,2.0f,2.0f));
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vShootDir, vShootDir = coreVector2(0.0f,-1.0f))
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->SetPosition(coreVector3(0.0f,0.0f,0.0f));

            const coreVector2 vTarget = pEnemy->AimAtPlayer().Normalized();
            const coreFloat   fPower  = coreVector2::Dot(vTarget, vShootDir);
            const coreFloat   fSign   = (fPower > 0.0f) ? coreVector2::Dot(vTarget, vShootDir.Rotated90()) : coreVector2::Dot(vTarget, -vShootDir.Rotated90());

            // sudden turn of enemy after moving to a certain threshold is shit, either use it as wall, or constantly rotate  

            vShootDir = coreVector2::Direction(vShootDir.Angle() + MIN(ABS(fPower) * 5.0f, 1.0f) * SIGN(fSign) * 2.0f * Core::System->GetTime());

            if(STAGE_TICK_TIME(12.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = vShootDir.Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // infinity flying enemy shooting from his side
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, COLOR_SHIP_ORANGE);

                pEnemy->SetPosition (coreVector3( 0.5f, 1.2f,0.0f) * FOREGROUND_AREA3);
                pEnemy->SetSize     (coreVector3( 1.3f, 1.3f,1.3f));
                pEnemy->SetDirection(coreVector3(-1.0f,-2.0f,0.0f).Normalized());
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 40.0f);

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) vCurPos.x =  ABS(vCurPos.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) vCurPos.x = -ABS(vCurPos.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) vCurPos.y =  ABS(vCurPos.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) vCurPos.y = -ABS(vCurPos.y);

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));

            if(STAGE_TICK_TIME(14.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.8f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.8f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // bullet attack explodes on border
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, COLOR_SHIP_RED);
                pEnemy->SetSize(coreVector3(1.4f,1.4f,1.4f));
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            const coreVector2 vTarget = coreVector2(i ? (0.3f * (-3.0f + 2.0f * I_TO_F(i))) : 0.0f,0.0f);

            if(STAGE_TAKEOFF)
            {
                pEnemy->SetPosition(coreVector3((vTarget + coreVector2(0.0f,1.0f)) * FOREGROUND_AREA, 0.0f));
            }

            pEnemy->DefaultMoveSmooth(vTarget, 40.0f, 10.0f);

            if(STAGE_TICK_LIFETIME(1.25f, (i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos, vDir)->ChangeSize(2.0f);
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            if(!IN_FOREGROUND_AREA(pBullet->GetPosition(), 1.1f))
            {
                const coreVector2 vPos = pBullet->GetPosition().xy();

                for(coreUintW i = 20u; i--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos,  vDir)->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos, -vDir)->ChangeSize(1.4f);
                    // TODO: lower purple balls get overlapped   
                }

                pBullet->Deactivate(true);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // two side-shooting caterpillars winding down
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_RED);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.22f * I_TO_F(i % 8u))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i < 8u) ? -0.5f : 0.5f, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_TIME(1.8f, (i < 8u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos,  vDir)->ChangeSize(1.8f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos, -vDir)->ChangeSize(1.8f);
            }

            /*
            for(coreUintW j = 0u; j < 13u; ++j)
            {
                if((i < 8u) ? STAGE_POSITION_POINT(pEnemy, (1.0f - I_TO_F(j) / 6.0f)                * FOREGROUND_AREA.y * 1.1f, y) :
                              STAGE_POSITION_POINT(pEnemy, (1.0f - I_TO_F(j) / 6.0f - 1.0f / 12.0f) * FOREGROUND_AREA.y * 1.1f, y))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos,  vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos, -vDir)->ChangeSize(1.8f);
                }
            }
            */
        });

        STAGE_WAVE
    });

    // ################################################################
    // split by bullets into nine areas and teach turn and roll 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cMinerEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                const coreVector2 vDir = StepRotated90(i);

                pEnemy->Configure(20, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
                pEnemy->Resurrect(-vDir * (FOREGROUND_AREA * 1.3f) + vDir.Rotated90() * (FOREGROUND_AREA * 0.4f), vDir);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iGroup)
        STAGE_GET_END

        // OLD
        if(pSquad1->IsFinished())
        {
            if(iGroup * 4u < pSquad1->GetNumEnemies())
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
                {
                    if((i >= iGroup * 4u) && (i < (iGroup+1u) * 4u))
                    {
                        pEnemy->Resurrect(coreVector2(0.0f, 1.1f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
                    }
                });
            }

            iGroup += 1u;
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {

            pEnemy->DefaultMoveSmooth(coreVector2(0.0f, 0.3f), 10.0f, 1.0f);


        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vDir = StepRotated90(i);

            if(STAGE_TICK_LIFETIME(6.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos, vDir);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // big enemy splits up into smaller enemies on death 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(100, COLOR_SHIP_YELLOW);
                pEnemy->Resurrect(coreVector2(0.0f, 1.1f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_RED);
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->DefaultMoveSmooth(coreVector2(0.0f, 0.3f), 10.0f, 1.0f);

            if(pEnemy->ReachedDeath())
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy2, j)
                {
                    pEnemy2->Resurrect(pEnemy->GetPosition().xy(), StepRotated90X(j));
                });
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayer()->GetPosition().xy(), LERP(40.0f, 20.0f, MIN(pEnemy->GetLifeTime(), 1.0f)), 2.0f);
        });

        STAGE_WAVE
    });

    // ################################################################
    // can only kill in order 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                pEnemy->Resurrect(coreVector2(I_TO_F(i - 2u) * 0.2f, 1.1f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });

            cEnemy* pChief = pSquad1->GetEnemy(1u);

            pChief->SetBaseColor(COLOR_SHIP_BLUE);
            pChief->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->DefaultMoveSmooth(coreVector2(I_TO_F(i - 2u) * 0.2f, 0.3f), 10.0f, 1.0f);

            if(pEnemy->ReachedDeath() && !pSquad1->IsFinished())
            {
                cEnemy* pNewChief = pSquad1->FindEnemyRev(pEnemy->GetPosition().xy());

                pNewChief->SetBaseColor(COLOR_SHIP_BLUE);
                pNewChief->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // move to last player location 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, COLOR_SHIP_ORANGE);
                pEnemy->Resurrect(coreVector2(0.0f,1.1f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vLastPosition, vLastPosition = g_pGame->GetPlayer(0u)->GetPosition().xy())
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy->GetPosition().xy() - vLastPosition).LengthSq() < 1.0f)
            {
                vLastPosition = g_pGame->GetPlayer(0u)->GetPosition().xy();

                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 20u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    // TODO: lower blue balls get overlapped 
                }
            }

            const coreVector2 vDiff  = vLastPosition - pEnemy->GetPosition().xy();
            const coreFloat   fLenSq = vDiff.LengthSq();
            const coreVector2 vDir   = vDiff.Normalized();
            const coreVector2 vPos   = pEnemy->GetPosition().xy() + vDir * (Core::System->GetTime() * 20.0f * SmoothTowards(fLenSq, 5.0f));

            pEnemy->SetPosition (coreVector3(vPos, 0.0f));
            pEnemy->SetDirection(coreVector3(vDir, 0.0f));
        });

        STAGE_WAVE
    });

    // ################################################################
    // explode into bullets on death 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 12u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_RED);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.1f, 0.15f * I_TO_F(i))

            STAGE_REPEAT(0.5f)

            pEnemy->SetPosition(coreVector3(0.85f * SIN((fLifeTime * 4.0f*PI + (I_TO_F(i) / 2.0f))), 1.2f - 4.8f * fLifeTime, 0.0f) * FOREGROUND_AREA3);

            if(pEnemy->ReachedDeath())
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 20u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (j & 0x01u) ? 0.6f : 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (j & 0x01u) ? 0.6f : 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    // TODO: lower blue balls get overlapped 
                }
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
    });

    // ################################################################
    // ################################################################
}