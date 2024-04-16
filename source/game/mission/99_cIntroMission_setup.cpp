///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
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
        //STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        //{
        //    pPlayer->ActivateNormalShading();
        //});

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetBaseColor(i ? COLOR_SHIP_YELLOW : COLOR_SHIP_BLUE);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(m_bFirstPlay)
        {
            STAGE_FINISH_AFTER(MISSION_WAIT_INTRO + 3.0f)
        }
        else
        {
            STAGE_FINISH_AFTER(MISSION_WAIT_INTRO)
        }
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(m_bFirstPlay)
        {
            g_pEnvironment->SetTargetSpeedNow(LERPH3(ENVIRONMENT_DEFAULT_SPEED, 4.0f, MIN1(m_fStageTime * 0.15f)));

            if(STAGE_TIME_POINT(1.0f))
            {
                g_pGame->GetInterface()->ShowStory(Core::Language->GetString("WELCOME"));
            }

            if(STAGE_TIME_AFTER(9.0f))
            {
                g_pGame->StartIntro();

                STAGE_FINISH_NOW
            }
        }
        else
        {
            g_pEnvironment->ChangeBackground(cCloudBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeedNow(4.0f);

            g_pGame->StartIntro();

            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_PLAY)
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // simple without attacks 1
    // teaches: moving (general), shooting (general, sustained), combo, enemies from every side, enemies fly infinite (#1), segment structure, sub-stage structure
    // - 5: from different side than previous group, to not fly into players
    // TODO 1: control intro should be separate for each player
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
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_RED);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT (iInputState)
            STAGE_GET_FLOAT(fInputDelay)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 17u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 18u, 23u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 24u, 29u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 30u, 35u)
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            if(!HAS_BIT(iInputState, 0u) && !pPlayer->GetMove().IsNull())
            {
                ADD_BIT(iInputState, 0u)
                fInputDelay = 0.0f;
            }
            else if(!HAS_BIT(iInputState, 1u) && pPlayer->GetWeapon(0u)->GetCooldown())
            {
                ADD_BIT(iInputState, 1u)
                fInputDelay = 0.0f;
            }
        });

        fInputDelay += 1.0f * TIME;

        if(!HAS_BIT(iInputState, 0u) && (fInputDelay >= 2.0f))
        {
            g_pMenu->GetTooltip()->ShowText(g_pForeground->Project2D(g_pGame->GetPlayer(0u)->GetPosition()), TOOLTIP_ONELINER, "MOVE");
            g_pMenu->GetTooltip()->Move();
        }
        else if(!HAS_BIT(iInputState, 1u) && (fInputDelay >= 2.0f))
        {
            g_pMenu->GetTooltip()->ShowText(g_pForeground->Project2D(g_pGame->GetPlayer(0u)->GetPosition()), TOOLTIP_ONELINER, "SHOOT");
            g_pMenu->GetTooltip()->Move();
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

        STAGE_WAVE("TEST", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // simple without attacks 2
    // teaches: use whole game area, enemies fly infinite (#2), turning, dodging attacks (#1), badges
    // [deprecated] - 2: enemies cannot be killed in one run by shooting upwards, so they may fly another run
    // seiten-gruppen sollten mit delay anfangen, damit man zeit hat das badge zu verstehen
    // angriffs-gruppe muss auf der seite anfangen, damit der spieler nicht zu sehr vom ersten angriff des spiels überrascht wird
    // TODO 1: badge sollte über letzter kill-position erscheinen
    // TODO 1: control intro should be separate for each player
    // TODO 1: GetNumEnemiesAlive should only check for the bottom 12 enemies
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.8f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 60u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_RED);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT (iInputState)
            STAGE_GET_FLOAT(fInputDelay)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 23u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 24u, 47u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 48u, 59u)

            if(m_iStageSub == 2u) STAGE_BADGE(0u, BADGE_EASY, coreVector3(0.0f,0.0f,0.0f))
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            if(!HAS_BIT(iInputState, 0u) && pPlayer->GetDirection().x)
            {
                ADD_BIT(iInputState, 0u)
                fInputDelay = 0.0f;
            }
        });

        if(pSquad1->GetNumEnemiesAlive() == 12u)
        {
            fInputDelay += 1.0f * TIME;

            if(!HAS_BIT(iInputState, 0u) && (fInputDelay >= 4.0f))
            {
                g_pMenu->GetTooltip()->ShowText(g_pForeground->Project2D(g_pGame->GetPlayer(0u)->GetPosition()), TOOLTIP_ONELINER, "TURN");
                g_pMenu->GetTooltip()->Move();
            }
        }

        const coreVector4 vArea = PLAYER_AREA_DEFAULT + coreVector4(0.0f,1.0f,0.0f,0.0f) * ((m_iStageSub == 1u) ? (BLENDH3(MIN1(m_fStageSubTime * 1.0f)) * 12.0f) : 0.0f);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(vArea);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 48u)
            {
                STAGE_LIFETIME(pEnemy, 1.1f, 0.2f * I_TO_F(i % 12u) + ((i < 12u) ? 0.0f : 1.0f))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.9f + ((i % 2u) ? -0.06f : 0.06f) * SIN(((1.0f/1.2f)*PI) * fLifeTime), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 12u) pEnemy->Rotate270();
                else if(i < 24u) pEnemy->Rotate90 ();
                else if(i < 36u) {}
                else if(i < 48u) pEnemy->InvertX  ();
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 1.1f, 0.2f * I_TO_F((i - 48u)))

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i - 48u) - 5.5f) * 0.16f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection   (coreVector3(pEnemy->GetPosition().x * 0.01f, -1.0f, 0.0f).Normalized());

                if(STAGE_TICK_LIFETIME(2.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(1.4f);
                }
            }
        });

        STAGE_WAVE("", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(PLAYER_AREA_DEFAULT);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // simple with attacks
    // teaches: dodging attack (#2), invincibility, damaging touch
    // geschwindigkeit des charges sollte am anfang hoch sein, damit der spieler unmöglich ausweichen kann
    // bei zweiter gruppe hat nur einer ein schild, um die wirkung noch mehr zu verdeutlichen (zwischen schild und nicht-schild)
    // chargende gegner aus der ecke werden nicht so wahrscheinlich schon beim spawnen getötet, zusätzlich sind sie eine kurze zeit immun
    // TODO 1: charge sollte passieren, bis sie einmal den spieler berührt haben
    // TODO 1: vielleicht gezielter gerader charge von seite (+ infinity) (könnten aber beim spawnen schon angegriffen werden)
    // TODO 1: badge
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f, 0.8f), coreVector2(1.2f,-0.8f).Normalized());
            pPath1->AddNode(coreVector2( 1.2f,-0.8f), coreVector2(1.2f,-0.8f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 40u)   // TODO 1: or 32
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_RED);
            });

            const auto nInitFunc = [&](const coreUintW iIndex, const coreVector2 vPos, const coreVector2 vDir)
            {
                pSquad1->GetEnemy(iIndex)->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pSquad1->GetEnemy(iIndex)->SetDirection(coreVector3(vDir,                   0.0f));
            };

            nInitFunc(24u, coreVector2(-1.2f, 1.2f), coreVector2( 1.0f,-1.0f).Normalized());
            nInitFunc(25u, coreVector2( 1.2f, 1.2f), coreVector2(-1.0f,-1.0f).Normalized());
            nInitFunc(26u, coreVector2( 1.2f,-1.2f), coreVector2(-1.0f, 1.0f).Normalized());
            nInitFunc(27u, coreVector2(-1.2f,-1.2f), coreVector2( 1.0f, 1.0f).Normalized());
            nInitFunc(28u, coreVector2(-1.2f, 1.2f), coreVector2( 1.0f,-1.0f).Normalized());
            nInitFunc(29u, coreVector2( 1.2f, 1.2f), coreVector2(-1.0f,-1.0f).Normalized());
            nInitFunc(30u, coreVector2( 1.2f,-1.2f), coreVector2(-1.0f, 1.0f).Normalized());
            nInitFunc(31u, coreVector2(-1.2f,-1.2f), coreVector2( 1.0f, 1.0f).Normalized());

            nInitFunc(32u, coreVector2(-1.2f, 1.2f), coreVector2( 1.0f,-1.0f).Normalized());
            nInitFunc(33u, coreVector2( 1.2f, 1.2f), coreVector2(-1.0f,-1.0f).Normalized());
            nInitFunc(34u, coreVector2( 1.2f,-1.2f), coreVector2(-1.0f, 1.0f).Normalized());
            nInitFunc(35u, coreVector2(-1.2f,-1.2f), coreVector2( 1.0f, 1.0f).Normalized());
            nInitFunc(36u, coreVector2(-1.2f, 1.2f), coreVector2( 1.0f,-1.0f).Normalized());
            nInitFunc(37u, coreVector2( 1.2f, 1.2f), coreVector2(-1.0f,-1.0f).Normalized());
            nInitFunc(38u, coreVector2( 1.2f,-1.2f), coreVector2(-1.0f, 1.0f).Normalized());
            nInitFunc(39u, coreVector2(-1.2f,-1.2f), coreVector2( 1.0f, 1.0f).Normalized());
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 16u, 23u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 26u, 27u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 28u, 31u)
            //else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 32u, 33u)
            //else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 34u, 35u)
            //else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 36u, 39u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 24u)
            {
                STAGE_LIFETIME(pEnemy, 1.2f, 0.25f * I_TO_F(i % 4u))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((i % 8u) < 4u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.1f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(i < 5u)
                {
                    if(fLifeTimeBase < pPath1->GetTotalDistance())
                    {
                        pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
                    }
                    else
                    {
                        pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                    }
                }

                if((i >= 8u) && STAGE_TICK_LIFETIME(5.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual((i / 4u) % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.6f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(1.5f);
                }
            }
            else
            {
                const coreFloat fDelay = (i == 24u) ? 1.0f : 0.4f;

                if(STAGE_SUBTIME_BEFORE(fDelay)) return;

                const coreVector2 vTarget = pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy();
                const coreFloat   fTime   = BLENDB(MIN1(pEnemy->GetLifeTime() - fDelay));

                pEnemy->DefaultMoveTarget(vTarget, LERP(120.0f, 50.0f, fTime), LERP(10.0f, 2.0f, fTime));

                if(i >= 32u)
                {
                    pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
                }

                if(fTime < ((i >= 32u) ? 0.8f : 0.7f))
                {
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);
                }
                else
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);
                }
            }
        });

        STAGE_WAVE("", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // simple with heavy attacks
    // teaches: enemy health, causing damage, moving slow, small hitbox
    // gegner unten sollten möglichst nah am rand sein, damit der spieler sich drehen muss
    // TODO 1: badge (beim 4. spawnt ein zusätzlicher gegner ?)
    // TODO 1: kaputte gegner sollten rauchen
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

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f,1.4f), coreVector2(0.0f,-1.0f));
            pPath3->AddStop(coreVector2(0.0f,0.5f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 7u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(120, COLOR_SHIP_RED);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 3u, 3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 4u, 4u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 5u, 5u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 6u, 6u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 1u, 2u)

            pSquad1->GetEnemy(4u)->SetCurHealthPct(0.1f);
            pSquad1->GetEnemy(4u)->RefreshColor();
            pSquad1->GetEnemy(5u)->SetCurHealthPct(0.1f);
            pSquad1->GetEnemy(5u)->RefreshColor();
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 3u)
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

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos,  vDir)->MakeWhite()->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, -vDir)->MakeWhite()->ChangeSize(1.5f);
                }
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 1.2f, 0.0f)

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((i - 3u) % 2u) ? -0.85f : 0.85f, 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (fLifeTime * ((i == 4u || i == 5u) ? 2.0f : -0.5f) + (1.0f*PI));

                     if(i < 5u) pEnemy->Rotate90();
                else if(i < 7u) pEnemy->Rotate90()->InvertX();
            }
        });

        STAGE_WAVE("", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // movement training
    // teaches: moving fast, extra score, taking damage, shield, lives
    // first enemy of this group needs to start on the other side of the last enemy from the previous group
    // some enemies need low health, because they might be reached too late for a full attack
    // side waves force player to turn again (a bit stressful this time)
    // side waves and side enemies need to be alternating, also in relation to hole-positions
    // delay between full waves needs to be adjusted to show alle effects even in the worst-case position of the player
    // last enemy shows, that not all enemies can be killed in one go, under some conditions
    // TODO 1: extra score (2 holes without enemies)
    // TODO 1: badge (wenn man durch die letzte geschoss-welle erfolgreich durch-fliegt ?)
    // TODO 1: make aiHits more stable, vielleicht mit bullet-damage als ID
    // TODO 1: make full-waves come with less delay, based on the player-state (also coop) (if possible) (maybe with the ID)
    // TODO 1: bullets könnten sich etwas besser überlagern (e.g. alle in die selbe richtung, mit selben offset (zwischen loch und nicht-loch welle), also von der seite kommend)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(30, COLOR_SHIP_RED);
            });
        });

        STAGE_GET_START(GAME_PLAYERS)
            STAGE_GET_UINT_ARRAY(aiHits, GAME_PLAYERS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1, 1u, 1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 2u, 2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 3u, 3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 4u, 4u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 5u, 5u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 6u, 6u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 7u, 7u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 8u, 8u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 9u, 9u)

            pSquad1->GetEnemy(4u)->SetCurHealthPct(0.1f);
            pSquad1->GetEnemy(4u)->RefreshColor();
            pSquad1->GetEnemy(5u)->SetCurHealthPct(0.1f);
            pSquad1->GetEnemy(5u)->RefreshColor();
            pSquad1->GetEnemy(6u)->SetCurHealthPct(0.1f);
            pSquad1->GetEnemy(6u)->RefreshColor();
        }

        constexpr coreFloat fBulletSpeed = (1.1f * BULLET_SPEED_FACTOR) / FOREGROUND_AREA.y;

        if(m_iStageSub < 10u)
        {
            if(STAGE_BEGINNING2)
            {
                coreFloat fSide;
                switch(m_iStageSub)
                {
                default: ASSERT(false)
                case 1u: fSide = -0.2f; break;
                case 2u: fSide = -0.6f; break;
                case 3u: fSide =  0.2f; break;
                case 4u: fSide =  0.6f; break;
                case 5u: fSide = -0.6f; break;
                case 6u: fSide = -0.6f; break;
                case 7u: fSide =  0.6f; break;
                case 8u: fSide = -0.2f; break;
                case 9u: fSide =  0.2f; break;
                }

                const coreVector2 vDir = (m_iStageSub == 5u) ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,-1.0f);

                for(coreUintW i = 0u; i < 26u; ++i)
                {
                    const coreVector2 vPos = (vDir * -1.3f - vDir.Rotated90() * fSide) * FOREGROUND_AREA;
                    const coreVector2 vTan = vDir.Rotated90() * (I_TO_F(i) * 0.06f + 0.3f) * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pSquad1->GetEnemy(0u), vPos + vTan, vDir)->MakeWhite()->ChangeSize(1.7f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pSquad1->GetEnemy(0u), vPos - vTan, vDir)->MakeWhite()->ChangeSize(1.7f);
                }
            }
        }
        else
        {
            if(STAGE_BEGINNING2 || STAGE_SUBTIME_POINT(4.0f) || STAGE_SUBTIME_POINT(8.0f) || STAGE_SUBTIME_POINT(9.0f))
            {
                const coreVector2 vDir = STAGE_SUBTIME_POINT(4.0f) ? coreVector2(-1.0f,0.0f) : coreVector2(0.0f,-1.0f);

                for(coreUintW i = 0u; i < 38u; ++i)
                {
                    const coreVector2 vPos = (vDir * -1.3f - vDir.Rotated90() * ((I_TO_F(i) - 18.5f) * 0.06f)) * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pSquad1->GetEnemy(0u), vPos, vDir)->MakeWhite()->ChangeSize(1.7f);
                }
            }

            STAGE_COLL_PLAYER_BULLET(pPlayer, pBullet, vIntersection, bFirstHit, COLL_VAL(aiHits))
            {
                const coreUintW iIndex = pPlayer - g_pGame->GetPlayer(0u);

                if(++aiHits[iIndex] == 1u) pPlayer->SetCurShield(2);
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, fBulletSpeed * ((i == 9u) ? 2.0f : 1.0f), (i == 9u) ? 20.0f : 0.0f)

            STAGE_REPEAT(pPath1->GetTotalDistance())

            coreFloat fSide;
            switch(i)
            {
            default: ASSERT(false)
            case 0u: fSide = -0.2f; break;
            case 1u: fSide = -0.6f; break;
            case 2u: fSide =  0.2f; break;
            case 3u: fSide =  0.6f; break;
            case 4u: fSide = -0.6f; break;
            case 5u: fSide = -0.6f; break;
            case 6u: fSide =  0.6f; break;
            case 7u: fSide = -0.2f; break;
            case 8u: fSide =  0.2f; break;
            case 9u: fSide =  0.6f; break;
            }

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(fSide, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i == 4u) pEnemy->Rotate270();
            else if(i == 9u) pEnemy->Rotate270();
        });

        STAGE_WAVE("", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_BOSS(m_Intro, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // end
    STAGE_MAIN({TAKE_MISSION})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_OUTRO)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->StartOutro(2u);

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                pPlayer->SetBaseColor(COLOR_SHIP_GREY * 0.6f);
            });
        }
    });

    // ################################################################
    // ################################################################
}