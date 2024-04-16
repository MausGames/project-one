///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 1: hard mode: x3 gegner und x3 attacks (= x9)
// TODO 1: hard mode: helfer kommen in den wellen vor und greifen mit an


// ****************************************************************
// setup the Intro mission
void cIntroMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            if(m_bFirstPlay) pPlayer->SetBaseColor(i ? COLOR_SHIP_YELLOW : COLOR_SHIP_BLUE);
            pPlayer->AddStatus(PLAYER_STATUS_INVINCIBLE);
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
            if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_QUICK))
            {
                STAGE_FINISH_NOW
            }
            else
            {
                STAGE_FINISH_AFTER(MISSION_WAIT_INTRO)
            }
        }
    });

    // ################################################################
    // start
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(m_bFirstPlay)
        {
            ASSERT(g_pEnvironment->GetBackground()->GetID() == cCloudBackground::ID)

            g_pEnvironment->SetTargetSpeedNow(LERP(ENVIRONMENT_DEFAULT_SPEED, 4.0f, MIN1(m_fStageTime * 0.12f)));

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

            g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
            g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
            g_pEnvironment->SetTargetSpeedNow    (4.0f);

            g_pGame->StartIntro();

            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // show mission name
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_NAMELESS))
        {
            STAGE_FINISH_NOW
        }
        else
        {
            if(!m_bFirstPlay && STAGE_BEGINNING)
            {
                g_pGame->GetInterface()->ShowMission(this);
            }

            STAGE_FINISH_AFTER(MISSION_WAIT_PLAY)
        }
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // simple without attacks 1
    // teaches: moving (general), shooting (general, sustained), combo, enemies from every side, enemies fly infinite (#1), segment structure, sub-stage structure
    // - 5: from different side than previous group, to not fly into players
    // TASK: hit all additional enemies
    // ACHIEVEMENT: destroy all enemies without pressing a single button
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
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, 0u, cIntroMission::__GetEnemyColor(i));
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cCinderEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_BLACK);
            });
        });

        STAGE_GET_START(2u + GAME_PLAYERS * 2u)
            STAGE_GET_UINT       (iExtraState)
            STAGE_GET_UINT       (iAnyButton)
            STAGE_GET_UINT_ARRAY (aiInputState, GAME_PLAYERS)
            STAGE_GET_FLOAT_ARRAY(afInputDelay, GAME_PLAYERS)
        STAGE_GET_END

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 17u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 18u, 23u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 24u, 29u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 30u, 35u)
            else if(STAGE_SUB(7u))
            {
                pSquad2->ClearEnemies(true);

                if(!iAnyButton) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, coreVector3(0.0f,0.0f,0.0f))
            }

            if(!m_bFirstPlay && g_pGame->IsTask())
            {
                     if(m_iStageSub == 2u) STAGE_RESURRECT(pSquad2, 0u, 0u)
                else if(m_iStageSub == 3u) STAGE_RESURRECT(pSquad2, 1u, 1u)
                else if(m_iStageSub == 4u) STAGE_RESURRECT(pSquad2, 2u, 2u)
                else if(m_iStageSub == 5u) STAGE_RESURRECT(pSquad2, 3u, 3u)
            }
        }

        if(Core::Input->GetAnyButton(CORE_INPUT_PRESS))
        {
            iAnyButton = 1u;
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            coreUint32& iInputState = aiInputState[i];
            coreFloat&  fInputDelay = afInputDelay[i];

            if(!HAS_BIT(iInputState, 0u) && !pPlayer->GetMove().IsNull())
            {
                ADD_BIT(iInputState, 0u)
                fInputDelay = 0.0f;
            }
            else if(!HAS_BIT(iInputState, 1u) && (pPlayer->GetWeapon(0u)->GetCooldown() && (pPlayer->GetDirection().y > 0.0f)))
            {
                ADD_BIT(iInputState, 1u)
            }

            fInputDelay += 1.0f * TIME;

            if(!HAS_BIT(iInputState, 0u) && (fInputDelay >= 2.0f))
            {
                this->EnableManual(i, 0u);
                this->EnableManual(i, 1u);
                this->EnableManual(i, 2u);
                this->EnableManual(i, 3u);
            }
            else if(!HAS_BIT(iInputState, 1u) && (fInputDelay >= 3.0f))
            {
                this->EnableManual(i, 4u);
                this->EnableManual(i, 7u);
                this->EnableManual(i, 8u);
                this->EnableManual(i, 9u);
                this->EnableManual(i, 10u);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.22f * I_TO_F(i % 6u))

            const coreSpline2* pPath = ((i % 24u) < 12u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2((((i % 12u) < 6u) ? -1.0f : 1.0f) * ((i < 12u) ? 1.1f : 1.0f), (i < 24u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2((i < 24u) ? 0.0f : -0.2f, 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
        });

        if(g_pGame->IsTask())
        {
            STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
            {
                constexpr coreFloat afOffset[] = {0.0f, 0.0f, 0.0f, 0.0f};
                constexpr coreUint8 aiStep  [] = {3u,   1u,   3u,   1u};
                ASSERT(i < ARRAY_SIZE(afOffset))
                ASSERT(i < ARRAY_SIZE(aiStep))

                const coreFloat fTime   = pEnemy->GetLifeTime();
                const coreFloat fHeight = LERP(1.3f, 0.9f, SIN(fTime * (1.0f*PI)));

                pEnemy->SetPosition(coreVector3(afOffset[i], fHeight, 0.0f) * FOREGROUND_AREA3);

                pEnemy->ToAxis(StepRotated90(aiStep[i]));

                if(pEnemy->ReachedDeath())
                {
                    if(++iExtraState >= pSquad2->GetNumEnemies())
                    {
                        STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                    }
                    else
                    {
                        g_pGame->GetCombatText()->DrawProgress(iExtraState, pSquad2->GetNumEnemies(), pEnemy->GetPosition());
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iExtraState, pSquad2->GetNumEnemies()), SOUND_ITEM_COLLECT);
                    }
                }

                if(fTime >= 1.0f)
                {
                    pEnemy->Kill(false);
                }
            });
        }

        STAGE_WAVE(0u, "0-1", {4.0f, 6.0f + 5.0f, 8.0f + 5.0f, 10.0f + 5.0f, 20.0f + 5.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // simple without attacks 2
    // teaches: use whole game area, enemies fly infinite (#2), turning, dodging attacks (#1)
    // [deprecated] - 2: enemies cannot be killed in one run by shooting upwards, so they may fly another run
    // seiten-gruppen sollten mit delay anfangen, damit man zeit hat das badge zu verstehen
    // angriffs-gruppe muss auf der seite anfangen, damit der spieler nicht zu sehr vom ersten angriff des spiels überrascht wird
    // TASK: destroy enemies up and down
    // ACHIEVEMENT: destroy only one enemy per turn
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 60u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, 0u, cIntroMission::__GetEnemyColor((i % 4u) + (i / 12u) * 4u));
            });
        });

        STAGE_GET_START(1u + GAME_PLAYERS * 3u)
            STAGE_GET_UINT       (iResetArea)
            STAGE_GET_UINT_ARRAY (aiLastPack,   GAME_PLAYERS)
            STAGE_GET_UINT_ARRAY (aiInputState, GAME_PLAYERS)
            STAGE_GET_FLOAT_ARRAY(afInputDelay, GAME_PLAYERS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 23u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 24u, 47u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 48u, 59u)
        }

        coreBool bFront = false;
        coreBool bBack  = false;
        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
                 if((            i < 12u) && !pEnemy->HasStatus(ENEMY_STATUS_DEAD)) bFront = true;
            else if((i >= 12u && i < 24u) && !pEnemy->HasStatus(ENEMY_STATUS_DEAD)) bBack  = true;
        });

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            coreUint32& iInputState = aiInputState[i];
            coreFloat&  fInputDelay = afInputDelay[i];

            if(!HAS_BIT(iInputState, 0u) && (pPlayer->GetDirection().y < 1.0f - CORE_MATH_PRECISION))
            {
                ADD_BIT(iInputState, 0u)
                ADD_BIT(iResetArea,  0u)
            }
            else if(!HAS_BIT(iInputState, 1u) && (pPlayer->GetDirection().y > -1.0f + CORE_MATH_PRECISION))
            {
                ADD_BIT(iInputState, 1u)
                ADD_BIT(iResetArea,  1u)
            }

            if(!bFront || !bBack) fInputDelay += 1.0f * TIME;

            if((!HAS_BIT(iInputState, 0u) || !HAS_BIT(iInputState, 1u)) && (fInputDelay >= 4.0f))
            {
                this->EnableManual(i, 5u);
                this->EnableManual(i, 6u);
                this->EnableManual(i, 7u);
                this->EnableManual(i, 8u);
                this->EnableManual(i, 9u);
                this->EnableManual(i, 10u);
            }
        });

        const coreFloat   fShift = (m_iStageSub == 1u) ? (BLENDH3(MIN1(m_fStageSubTime)) * 12.0f) : 0.0f;
        const coreVector4 vArea  = PLAYER_AREA_DEFAULT + coreVector4(0.0f,1.0f,0.0f,0.0f) * (HAS_BIT(iResetArea, 0u) ? 0.0f : fShift)
                                                       - coreVector4(0.0f,0.0f,0.0f,1.0f) * (HAS_BIT(iResetArea, 1u) ? 0.0f : fShift);

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
                const coreVector2 vOffset = coreVector2(((i < 24u) ? 0.9f : 0.7f) + ((i % 2u) ? -0.06f : 0.06f) * SIN(((1.0f/1.2f)*PI) * fLifeTime), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 12u) pEnemy->Rotate270();
                else if(i < 24u) pEnemy->Rotate90 ();
                else if(i < 36u) pEnemy->Rotate180();
                else if(i < 48u) pEnemy->Rotate180()->InvertX();

                if(!m_bFirstPlay && pEnemy->ReachedDeath() && (!g_pGame->IsEasy() || ((i % 4u) < 2u)))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(pEnemy->LastAttackerIndex()).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.5f, pEnemy, vPos, vDir)->ChangeSize(1.6f);

                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 0.6f, 0.2f * I_TO_F((i - 48u)) + (0.6f/1.1f))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 48u) * 5u) % 12u) - 5.5f) * 0.16f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(STAGE_TICK_LIFETIME(g_pGame->IsEasy() ? 2.0f : 4.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();
                    const coreVector2 vTan = vDir.Rotated90() * ((s_iTick % 2u) ? -1.0f : 1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, (vDir + vTan * 0.02f).Normalized())->ChangeSize(1.5f);

                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(g_pGame->IsTask() && (i < 24u) && pEnemy->ReachedDeath() && (m_iStageSub == 2u))
            {
                STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
            }

            if(pEnemy->ReachedDeath())
            {
                const cPlayer*  pPlayer = pEnemy->LastAttacker();
                const coreUintW iIndex  = g_pGame->GetPlayerIndex(pPlayer);

                if(aiLastPack[iIndex] != 0xFFu)
                {
                    const coreUint8 iNewPack = PackDirection(pPlayer->GetDirection().xy()) + 1u;

                    aiLastPack[iIndex] = (aiLastPack[iIndex] == iNewPack) ? 0xFFu : iNewPack;
                }
            }
        });

        if(STAGE_CLEARED)
        {
            coreBool bBadge = true;

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                if(aiLastPack[i] == 0xFFu) bBadge = false;
            });

            if(bBadge) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, coreVector3(0.0f,0.0f,0.0f))
        }

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,1.0f).Normalized(), 10.0f);
        }

        STAGE_WAVE(1u, "0-2", {8.0f, 12.0f + 5.0f, 16.0f + 5.0f, 20.0f + 5.0f, 40.0f + 5.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cConeBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cFlipBullet>();
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
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,1.0f).Normalized());

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // simple with attacks
    // teaches: dodging attack (#2), invincibility, damaging touch, badge (#1)
    // geschwindigkeit des charges sollte am anfang hoch sein, damit der spieler (nahezu) unmöglich ausweichen kann
    // chargende gegner aus der ecke werden nicht so wahrscheinlich schon beim spawnen getötet, zusätzlich sind sie eine kurze zeit immun
    // erster chargender gegner kommt von dort wo der spieler unwahrscheinlicher ist
    // TASK: collect all items
    // ACHIEVEMENT: merge 4 enemies
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.25f, 0.8f), coreVector2(1.25f,-0.8f).Normalized());
            pPath1->AddNode(coreVector2( 1.25f,-0.8f), coreVector2(1.25f,-0.8f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 32u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, 0u, cIntroMission::__GetEnemyColor(i));

                if(i >= 24u) pEnemy->AddStatus(ENEMY_STATUS_TOP);
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
        });

        STAGE_ADD_SQUAD(pSquad2, cFreezerEnemy, 8u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_BLACK);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iCardCount)
        STAGE_GET_END

        if(pSquad1->IsFinished())
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
            else if(STAGE_SUB(10u))
            {
                pSquad2->ClearEnemies(true);
            }

            if(!m_bFirstPlay && g_pGame->IsTask())
            {
                     if(m_iStageSub == 1u) STAGE_RESURRECT(pSquad2, 0u, 3u)
                else if(m_iStageSub == 2u) STAGE_RESURRECT(pSquad2, 4u, 7u)
            }
        }

        coreVector2 vSharedPos = coreVector2(0.0f,0.0f);
        coreVector2 vSharedDir = coreVector2(0.0f,0.0f);
        coreBool    bShared    = true;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 24u)
            {
                STAGE_LIFETIME(pEnemy, 1.2f, 0.25f * I_TO_F(i % 4u))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((i % 8u) < 4u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.1f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(i < 4u)
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

                if((i >= 8u) && STAGE_TICK_LIFETIME(5.0f, 0.0f) && (!g_pGame->IsEasy() || (s_iTick < 4u)))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual((i / 4u) % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.6f, pEnemy, vPos, vDir)->ChangeSize(1.6f);

                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else
            {
                const coreFloat fDelay = (i == 24u) ? 1.0f : 0.4f;

                if(STAGE_SUBTIME_AFTER(fDelay))
                {
                    STAGE_LIFETIME(pEnemy, 1.2f, 1.2f * fDelay)

                    const coreVector2 vTarget = pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy();
                    const coreFloat   fTime   = BLENDB(MIN1(pEnemy->GetLifeTime() - fDelay));

                    pEnemy->DefaultMoveTarget(vTarget, LERP(120.0f, 50.0f, fTime), LERP(10.0f, 2.0f, fTime));

                    if(fTime < 0.3f)
                    {
                        pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);
                    }
                    else
                    {
                        pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);
                    }

                    if(!m_bFirstPlay && STAGE_TICK_LIFETIME(10.0f, 0.0f) && (!g_pGame->IsEasy() || (s_iTick < 4u)))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition ().xy();
                        const coreVector2 vDir = pEnemy->GetDirection().xy();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.5f, pEnemy, vPos, vDir)->ChangeSize(1.3f);

                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
            }

            const coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

            if(vSharedDir.IsNull())
            {
                vSharedPos = vCurPos;
                vSharedDir = vCurDir;
            }

            if((vSharedPos - vCurPos).LengthSq() > POW2(0.5f)) bShared = false;
            if(!SameDirection(vSharedDir, vCurDir))            bShared = false;
            if(!g_pForeground->IsVisiblePoint(vCurPos))        bShared = false;
        });

        if(!m_bFirstPlay && g_pGame->IsTask())
        {
            STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
            {
                STAGE_LIFETIME(pEnemy, 1.2f, 0.25f * I_TO_F(i % 4u))

                const coreVector2 vFactor = coreVector2(((i % 8u) < 4u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.1f);

                if(pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime))
                {
                    pEnemy->Kill(false);
                }

                if(pEnemy->ReachedDeath())
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cCardBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.7f)->AddStatus(BULLET_STATUS_GHOST);
                }
            });

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cCardBullet>([&](cCardBullet* OUTPUT pBullet)
            {
                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    const coreVector2 vDiff = pBullet->GetPosition().xy() - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(7.0f))
                    {
                        pBullet->Deactivate(true);

                        if(++iCardCount >= pSquad2->GetNumEnemies())
                        {
                            STAGE_BADGE(0u, BADGE_EASY, pBullet->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawCountdown(iCardCount, pSquad2->GetNumEnemies(), pBullet->GetPosition());
                            g_pSpecialEffects->PlaySound(pBullet->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iCardCount, pSquad2->GetNumEnemies()), SOUND_ITEM_COLLECT);
                        }
                    }
                });
            });
        }

        if(bShared && (pSquad1->GetNumEnemiesAlive() == 4u))
        {
            STAGE_BADGE(3u, BADGE_ACHIEVEMENT, coreVector3(vSharedPos, 0.0f))
        }

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,0.0f), 10.0f);
        }

        STAGE_WAVE(2u, "0-3", {11.0f, 16.0f + 5.0f, 22.0f + 5.0f, 27.0f + 5.0f, 54.0f + 5.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTriangleBullet>();
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,0.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // simple with heavy attacks
    // teaches: enemy health, causing damage, moving slow, small hitbox, badge (#2)
    // gegner unten sollten möglichst nah am rand sein, damit der spieler sich drehen muss
    // TASK: try to destroy the impaled enemy
    // ACHIEVEMENT: only do damage during odd seconds
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
            pPath3->AddNode(coreVector2(0.0f, 1.4f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,-1.4f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(120, 0u, cIntroMission::__GetEnemyColor(i + 4u));
            });

            pSquad1->GetEnemy(0u)->AddStatus(ENEMY_STATUS_IMMORTAL);
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iTimeState)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 1u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 2u, 3u)
            else if(STAGE_SUB(4u))
            {
                g_pGame->GetBulletManagerEnemy()->ResetOrder();
            }
        }

        if(STAGE_BEGINNING)
        {
            constexpr coreUint8 aiNewOrder[] = {cWaveBullet::ID, cViewBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        }

        if(m_iStageSub == 1u)
        {
            if(STAGE_BEGINNING2)
            {
                m_Intro.ResurrectIntro();
            }
        }
        else if(m_iStageSub == 2u)
        {
            if(STAGE_BEGINNING2)
            {
                m_Intro.Kill(false);
            }
        }

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 1u)
            {
                STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(-0.85f,0.0f);

                if(pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime))
                {
                    pEnemy->Kill(false);
                    bPostpone = true;

                    m_vSkewerColor = pEnemy->GetColor3();
                }

                pEnemy->DefaultRotate(fLifeTime * 4.0f + (0.5f*PI));
                pEnemy->Rotate270();

                const coreVector3 vBossDir = -pEnemy->GetDirection().RotatedZ135();
                const coreVector3 vBossPos =  pEnemy->GetPosition () - vBossDir * 12.0f;

                m_Intro.SetPosition (vBossPos);
                m_Intro.SetDirection(vBossDir);

                if(g_pGame->IsTask())
                {
                    const coreFloat fPercent = pEnemy->GetCurHealthPct();

                    if(fPercent <= 0.0f)
                    {
                        pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);

                        STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                    }
                    else g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.0f%%", FLOOR((1.0f - fPercent) * 100.0f)), pEnemy->GetPosition(), COLOR_MENU_INSIDE);
                }

                if(!m_bFirstPlay && STAGE_TICK_LIFETIME(g_pGame->IsEasy() ? 1.5f : 3.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();

                    for(coreUintW j = 50u; j--; )
                    {
                        if(((j / 2u) % 5u) != ((s_iTick * 3u) % 5u)) continue;

                        const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) / 50.0f * (1.0f*PI));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.3f, pEnemy, vPos,  vDir)->ChangeSize(1.8f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.3f, pEnemy, vPos, -vDir)->ChangeSize(1.8f);
                    }

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 1.8f, (i == 3u) ? 1.0f : 0.0f)

                const coreSpline2* pPath = (i < 2u) ? pPath1 : pPath2;

                if(STAGE_LIFETIME_BEFORE(pPath->GetTotalDistance()))
                {
                    const coreVector2 vFactor = coreVector2((i < 3u) ? 1.0f : -1.0f, 1.0f);
                    const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                    pEnemy->DefaultMovePath     (pPath, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection        (coreVector3(0.0f,-1.0f,0.0f));
                    pEnemy->DefaultOrientateLerp(0.0f, 4.0f*PI * vFactor.x, BLENDB(fLifeTime * RCP(pPath->GetTotalDistance())));
                }
                else if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(s_iTick) * DEG_TO_RAD(27.0f));

                                           g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.7f);
                    if(!g_pGame->IsEasy()) g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, -vDir)->ChangeSize(1.7f);

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 25.0f, 1u, COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(pEnemy->WasDamaged())
            {
                const coreBool bOdd = F_TO_UI(m_fStageTime) % 2u;

                if(!HAS_BIT(iTimeState, 0u))
                {
                    ADD_BIT(iTimeState, 0u)
                    SET_BIT(iTimeState, 1u, bOdd)
                }

                if(bOdd != HAS_BIT(iTimeState, 1u))
                {
                    ADD_BIT(iTimeState, 2u)
                }
            }
        });

        if(!bPostpone && STAGE_CLEARED)
        {
            if(!HAS_BIT(iTimeState, 2u)) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, coreVector3(0.0f,0.0f,0.0f))
        }

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,-1.0f).Normalized(), 10.0f);
        }

        if(!bPostpone) STAGE_WAVE(3u, "0-4", {7.0f, 10.0f + 5.0f, 14.0f + 5.0f, 17.0f + 5.0f, 34.0f + 5.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cWaveBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cViewBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cCardBullet>();
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,-1.0f).Normalized());

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // movement training
    // teaches: moving fast, (extra score), (taking damage), (shield), (lives), badge (#3)
    // first enemy of this group needs to start on the other side of the last enemy from the previous group
    // some enemies need low health, because they might be reached too late for a full attack (but low health will make the next enemy spawn sooner)
    // side waves force player to turn again (a bit stressful this time)
    // side waves and side enemies need to be alternating, also in relation to hole-positions
    // delay between full waves needs to be adjusted to show alle effects even in the worst-case position of the player
    // last enemy shows, that not all enemies can be killed in one go, under some conditions
    // TASK: move through all the bullet walls
    // ACHIEVEMENT: neither cause nor take any damage
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 11u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(30, 0u, cIntroMission::__GetEnemyColor(i - (i >= 4u) - (i >= 6u)));
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT(iWaveCount)
            STAGE_GET_UINT(iDamageState)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1, 0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1, 1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 4u,  4u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 5u,  5u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 6u,  6u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 7u,  7u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 8u,  8u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 9u, 10u)
            else if(STAGE_SUB(11u))
            {
                g_pGame->GetBulletManagerEnemy()->ResetOrder();

                if(!iDamageState) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, coreVector3(0.0f,0.0f,0.0f))
            }

            pSquad1->GetEnemy(4u)->SetCurHealthPct(0.1f);
            pSquad1->GetEnemy(4u)->RefreshColor();
            pSquad1->GetEnemy(6u)->SetCurHealthPct(0.1f);
            pSquad1->GetEnemy(6u)->RefreshColor();
        }

        if(STAGE_BEGINNING)
        {
            constexpr coreUint8 aiNewOrder[] = {cQuadBullet::ID, cOrbBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        }

        const coreFloat fBulletSpeed = g_pGame->IsEasy() ? 0.8f : 1.1f;
        const coreFloat fEnemySpeed  = (fBulletSpeed * BULLET_SPEED_FACTOR) / FOREGROUND_AREA.y;

        if(m_iStageSub >= 10u)
        {
            if(STAGE_BEGINNING2 || STAGE_SUBTIME_POINT(2.0f) || STAGE_SUBTIME_POINT(4.0f) || STAGE_SUBTIME_POINT(6.0f))
            {
                coreUintW iNum = 4u;
                     if(STAGE_SUBTIME_POINT(2.0f)) iNum = 3u;
                else if(STAGE_SUBTIME_POINT(4.0f)) iNum = 2u;
                else if(STAGE_SUBTIME_POINT(6.0f)) iNum = 1u;

                const coreVector2 vDir    = coreVector2(0.0f,-1.0f);
                const coreFloat   fOffset = I_TO_F(iNum % 4u) / 2.0f;

                for(coreUintW i = 0u; i < 36u; ++i)
                {
                    if((i % (iNum * 2u)) >= iNum) continue;

                    const coreVector2 vPos = (vDir * -1.3f - vDir.Rotated90() * ((I_TO_F(i) - 17.5f + fOffset) * 0.06f)) * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u - iNum, fBulletSpeed, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.9f);
                }
            }

            if(g_pGame->IsTask())
            {
                g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    STAGE_FOREACH_PLAYER(pPlayer, i)
                    {
                        const coreUint32 iDamage = pBullet->GetDamage();

                        if((iDamage < 5u) && (iDamage > iWaveCount) && (pBullet->GetPosition().y < pPlayer->GetPosition().y))
                        {
                            if(++iWaveCount == 4u)
                            {
                                STAGE_BADGE(0u, BADGE_EASY, pPlayer->GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawProgress(iWaveCount, 4u, pPlayer->GetPosition());
                                g_pSpecialEffects->PlaySound(pPlayer->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iWaveCount, 4u), SOUND_ITEM_COLLECT);
                            }
                        }
                    });
                });
            }
        }

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 9u)
            {
                STAGE_LIFETIME(pEnemy, fEnemySpeed, 0.3f)

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
                }

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(fSide, 0.0f);

                if(pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime))
                {
                    pEnemy->Kill(false);
                    bPostpone = true;
                }

                if(i == 4u) pEnemy->Rotate270();

                if(STAGE_TAKEOFF)
                {
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    for(coreUintW j = 0u; j < 26u; ++j)
                    {
                        const coreVector2 vPos = (vDir * -1.3f - vDir.Rotated90() * fSide) * FOREGROUND_AREA;
                        const coreVector2 vTan = vDir.Rotated90() * (I_TO_F(j) * 0.06f + 0.2f) * FOREGROUND_AREA;

                        const coreFloat fSpeed = fBulletSpeed - I_TO_F(j) * 0.01f;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, pEnemy, vPos + vTan, vDir)->ChangeSize(1.9f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, pEnemy, vPos - vTan, vDir)->ChangeSize(1.9f);
                    }
                }

                if(!m_bFirstPlay && STAGE_TICK_LIFETIME(8.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = MapToAxis(coreVector2::Direction((I_TO_F((s_iTick * 2u) % 5u) - 2.0f) * 0.2f), pEnemy->GetDirection().xy());

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos, vDir)->ChangeSize(1.3f);

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 10.0f, 1u, COLOR_ENERGY_CYAN);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else
            {
                STAGE_LIFETIME(pEnemy, fEnemySpeed * 2.0f, g_pGame->IsEasy() ? 12.0f : 15.0f)

                const coreVector2 vFactor = coreVector2(1.0f, (i == 9u) ? 1.0f : -1.0f);
                const coreVector2 vOffset = coreVector2((i == 9u) ? 0.6f : 0.3f, 0.0f);

                if(pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime))
                {
                    pEnemy->Kill(false);
                    bPostpone = true;
                }

                pEnemy->Rotate270();
            }

            if(pEnemy->WasDamaged())
            {
                iDamageState += 1u;
            }
        });

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            if(pPlayer->WasDamaged())
            {
                iDamageState += 1u;
            }
        });

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,-1.0f), 10.0f);
        }

        if(!bPostpone) STAGE_WAVE(4u, "0-5", {19.0f, 28.0f + 5.0f, 38.0f + 5.0f, 47.0f + 5.0f, 94.0f + 5.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(0.0f,-1.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        UNUSED STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(120, 0u, m_vSkewerColor);
                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_BOSS(m_Intro, {70.0f, 105.0f, 140.0, 175.0f, 350.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->PrefetchBoss();
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(!g_pGame->GetItemManager()->GetNumItems())
        {
            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                if(m_bFirstPlay)
                {
                    pPlayer->SetBaseColor(COLOR_SHIP_BLACK);
                }
                pPlayer->RemoveStatus(PLAYER_STATUS_INVINCIBLE);
            });

            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(!g_pGame->GetItemManager()->GetNumItems() && !g_pGame->GetInterface()->IsFragmentActive())
        {
            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // end
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_OUTRO)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        if(m_bFirstPlay)
        {
            if(STAGE_BEGINNING)
            {
                g_pGame->StartOutro(GAME_OUTRO_BEGINNING);
                g_pGame->FadeMusic(0.16f);
            }
        }
        else
        {
            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // ################################################################
}