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

            const coreVector2 vDiff = pEnemy->GetPosition().xy() - pParent->GetPosition().xy();
            
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
    // hide from super laser
    // blöcke die von einer seite schützen, und geschosse aufhalten (spieler, gegner)
    // 90 degree, then 45 degree
    // 90 degree flip, then 180 degree flip
    // laser dreht sich 360
    // gegner der für angriff verantwortlich ist kommt von unten ins bild wärend er schießt, blöcke werden zerstört, dodge hinter ihn
    // coop
    //   |        |
    //   | O    O | O
    // O |        |
    //   |
    // zwei quads (für schräge ecken) mit tiefe zeichnen und depth-test verwenden
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.25f, 0.2f * I_TO_F(i))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("ZWEIUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Urtica, {60.0f, 120.0f, 180.0, 240.0f})
    //});

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