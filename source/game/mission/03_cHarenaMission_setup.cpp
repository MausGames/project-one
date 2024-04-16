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
// setup the Harena mission
void cHarenaMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cDesertBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // mothership spawns enemies
    // warrior always keeps a certain amount of arrows active
    // enemies fly into shoot direction (either direction or average bullets of a player)
    // enemies position themselves directly before player, and follow turn with delay
    // enemies sliede left and right, up and down, to "intercept" and annoy player
    // warrior has shoield of enemies
    //      TODO: single enemy which recovers # times
    //      TODO: enemies are used like bullets
    //      TODO: enemies don't give score
    // TODO: optimize getting dead enemy
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.8f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u) // TODO
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(200, COLOR_SHIP_ORANGE);

                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 30u) // TODO
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vTarget)
        STAGE_GET_END

        if(pSquad2->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad2, 0u,  7u)
            //else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad2, 6u, 11u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(false && STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                cEnemy* pDead = NULL;

                pSquad2->ForEachEnemyAll([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
                {
                    if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))
                        pDead = pEnemy;
                });

                if(pDead)
                {
                    const coreVector2 vAim = pEnemy->AimAtPlayerDual(0u).Normalized();
                    const coreVector2 vDir = (vAim + 0.1f * (I_TO_F((s_iTick % 2u)) - 0.5f)).Normalized();

                    pDead->SetPosition (pEnemy->GetPosition());
                    pDead->SetDirection(coreVector3(vDir, 0.0f));   // TODO   
                    pDead->Resurrect();
                }
            }
        });

        const cPlayer* pPlayer = g_pGame->FindPlayerDual(0u);   // TODO    

        const coreVector2 vDiff = pPlayer->GetPosition().xy() / FOREGROUND_AREA - vTarget;
        vTarget += vDiff.Normalized() * SmoothTowards(vDiff.Length(), 20.0f) * 40.0f * Core::System->GetTime();


        const cEnemy* pParent = pSquad1->GetEnemy(0u);
        
        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.2f * I_TO_F(i))

            if(STAGE_TAKEOFF) pEnemy->SetPosition(pParent->GetPosition() + 5.0f * coreVector3(coreVector2::Rand(), 0.0f));

            //const coreVector2 vDiff = pEnemy->GetPosition().xy() - pParent->GetPosition().xy();
            
            const coreVector2 vDir = coreVector2::Direction(fLifeTime);

            coreVector2 vNewPos = pParent->GetPosition().xy() + vDir * 10.0f;
            
            //vNewPos += -vDiff.Normalized() * Core::System->GetTime() * 1.0f + vDiff.Rotated90() * Core::System->GetTime();

            pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
        });
        /*
        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if(STAGE_TAKEOFF) pEnemy->SetPosition(pSquad1->GetEnemy(0u)->GetPosition());

            if(i < 4u)
            {
                const coreFloat x = SIN(fLifeTime);
                const coreFloat y = vTarget.y + ((I_TO_F(i) - 1.5f) * 0.1f);

                pEnemy->SetPosition(coreVector3(x, y, 0.0f) * FOREGROUND_AREA3);
            }
            else
            {
                const coreFloat y = COS(fLifeTime);
                const coreFloat x = vTarget.x + ((I_TO_F(i - 4u) - 1.5f) * 0.1f);

                pEnemy->SetPosition(coreVector3(x, y, 0.0f) * FOREGROUND_AREA3);
            }
        });
*/

/*
        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 20.0f + 40.0f * fLifeTime);

            const coreVector2 vPos = pEnemy->GetPosition().xy();

            if((vPos.x < -FOREGROUND_AREA.x * 1.2f) ||
               (vPos.x >  FOREGROUND_AREA.x * 1.2f) ||
               (vPos.y < -FOREGROUND_AREA.y * 1.2f) ||
               (vPos.y >  FOREGROUND_AREA.y * 1.2f))
            {
                pEnemy->Kill(false);
            }
        });
        */

        STAGE_WAVE("EINUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Urtica, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // enemies form a tower and fall over
    // TODO: give central effect to highlight damaging touch   
    // TODO: check for and fix shadow artifacts, when jumping behind near clipping plane of shadow viewport (maybe fade out near plane)
    // add effect to highlight bottom enemies
    // if the last enemy is invincible, he should just die when starting to move
    // 
    // should not create bullets, to focus on the movement and and tower (bullets are too distracting)
    // should be fast enough to be a thread, but needs visual space to properly identify movement
    // complete vertical or horizontal movement is too easy
    // don't spawn too far on the side to properly blend them in, but not too near center, to highlight the tower structure
    // try to stack enemies all facing into the same direction, otherwise it looks not as fascinating
    // 4: stacking only two gives a lot of safe ways to kill them when they try to hit you
    // 1: show tower and flight mechanic
    // 2: show infinite displaced tower
    // 1: shoot away from second tower, to give spacial distance, and highlight the displacement mechanic
    // 3, 4, 5: left around, right around, left around
    // it is important that the flight start-direction can be properly identified
    STAGE_MAIN
    {
        constexpr coreUintW iEnemies  = 40u;
        constexpr coreFloat fDistance = 0.4f;

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, iEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetAlpha (0.0f);
                pEnemy->Configure(1, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(iEnemies * 6u)
            STAGE_GET_VEC2_ARRAY (avMove,   iEnemies)
            STAGE_GET_VEC2_ARRAY (avRotate, iEnemies)
            STAGE_GET_FLOAT_ARRAY(afFall,   iEnemies)
            STAGE_GET_FLOAT_ARRAY(afHeight, iEnemies)
        STAGE_GET_END

        bool bClearedEarly = true;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if(STAGE_TAKEOFF)
            {
                afHeight[i] = 60.0f + 10.0f * I_TO_F(i);
            }

            const coreFloat fFloor = (((i == 0u) || !avMove[i-1u].IsNull()) ? 0.0f : (pSquad1->GetEnemy(i-1u)->GetPosition().z + 9.0f));

            afFall  [i] += -120.0f   * TIME;
            afHeight[i] += afFall[i] * TIME;

            if(afHeight[i] < fFloor)
            {
                afFall  [i] = MAX(afFall[i], 0.0f);
                afHeight[i] = fFloor;
            }

            if(avMove[i].IsNull() && pEnemy->ReachedDeath())
            {
                avMove  [i]   = pEnemy->GetDirection().xy();
                avRotate[i].x = pEnemy->GetDirection().xy().Angle();

                for(coreUintW j = i+1u, je = pSquad1->GetNumEnemies(); j < je; ++j)
                {
                    if(!afFall[j] || (j == i+1u))
                    {
                        afFall[j] = MIN(5.0f + 0.6f * afHeight[j], 100.0f);
                    }
                }

                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            }

            if(!avMove[i].IsNull())
            {
                coreVector2 vCurPos = pEnemy->GetPosition().xy();
                coreVector2 vCurDir = avMove[i];

                vCurPos += vCurDir * (70.0f * TIME);

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.0f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.0f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.0f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.0f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);

                pEnemy->SetPosition(coreVector3(vCurPos, afHeight[i]));
                avMove[i] = vCurDir;

                avRotate[i].y += 1.0f * TIME;
                pEnemy->DefaultRotate(avRotate[i].x + avRotate[i].y * (2.0f*PI));

                if(avRotate[i].y >= 3.0f) pEnemy->Kill(true);
            }
            else
            {
                coreVector2 vPos;
                coreVector2 vDir;

                if(i < 8u)
                {
                    vPos = coreVector2( 0.0f,1.0f) * fDistance;
                    vDir = coreVector2(-1.0f,1.0f).Normalized();
                }
                else if(i < 12u)
                {
                    vPos = coreVector2( 1.0f,1.0f) * fDistance;
                    vDir = coreVector2(-1.0f,1.0f).Normalized();
                }
                else if(i < 16u)
                {
                    vPos = coreVector2(-1.0f,1.0f) * fDistance;
                    vDir = coreVector2( 1.0f,1.0f).Normalized();
                }
                else if(i < 24u)
                {
                    vPos = coreVector2::Direction(fLifeTime) * fDistance;
                    vDir = coreVector2::Direction(fLifeTime);
                }
                else if(i < 32u)
                {
                    vPos = coreVector2::Direction(fLifeTime * -1.0f) * fDistance;
                    vDir = coreVector2::Direction(fLifeTime *  3.0f);
                }
                else
                {
                    vPos = StepRotated90X((i - 32u) / 2u).Processed(SIGN) * fDistance;
                    vDir = pEnemy->AimAtPlayerSide().Normalized();
                }

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, afHeight[i]));
                pEnemy->SetDirection(coreVector3(vDir, 0.0f));

                bClearedEarly = false;
            }

            if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST) && !afHeight[i])
            {
                pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
            }

            const coreFloat fVisibility = CLAMP((54.0f - afHeight[i]) * 0.2f, pEnemy->GetAlpha() / 0.8f, 1.0f);
            const coreBool  bGhost      = CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST);

            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * fVisibility * (bGhost ? 1.0f : 1.3f));
            pEnemy->SetAlpha(fVisibility * (bGhost ? 0.8f : 1.0f));
        });

        if(bClearedEarly) pSquad1->ClearEnemies(true);

        STAGE_WAVE("SECHSUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Tiger, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Lucifer, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}