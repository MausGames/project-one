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

    // killing enemy creates a bullet line you need to roll, 1 enemy, 1 enemy, 2 enemies, 3/4 enemies 

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

        STAGE_GET_START(1u, 0u)
            STAGE_GET_UINT(iGroup)
        STAGE_GET_END

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

            if(STAGE_TICK(10u))
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

        STAGE_GET_START(0u, 2u)
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
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(2, COLOR_SHIP_RED);
                pEnemy->Resurrect(coreVector2(I_TO_F(i - 1u) * 0.2f, 1.1f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->DefaultMoveSmooth(coreVector2(I_TO_F(i - 1u) * 0.2f, 0.3f), 10.0f, 1.0f);

            if(pEnemy->ReachedDeath())
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 40u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f));

                    // 
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