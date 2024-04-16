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
// setup the Rutilus mission
void cRutilusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_QUICK))
        {
            STAGE_FINISH_NOW
        }
        else
        {
            STAGE_FINISH_AFTER(MISSION_WAIT_INTRO)
        }
    });

    // ################################################################
    // start
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pEnvironment->ChangeBackground(cSpaceBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        g_pEnvironment->SetTargetSpeedNow(4.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // show mission name
    STAGE_MAIN({TAKE_MISSION})
    {
        if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_CONTINUE))
        {
            STAGE_FINISH_NOW
        }
        else
        {
            if(STAGE_BEGINNING)
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
    // force rotation
    // enemies spawn against turn-direction at beginning, because player stays away from them, and then is already positioned for the first enemy of the next sub-stage
    // enemies spawning against shoot-direction feels too cumbersome
    // enemies are aligned with arrow pattern (if possible)
    // player-position can be controlled a lot here, make sure to have proper player<>enemy placement
    // plate movement is opposite to enemy movement, otherwise annoying attack-bubbles may appear (can be reduced by increasing speed of enemies, but still annoying)
    // enemies flying perpendicular to shoot direction also need to move near the border, to make switching directions necessary
    // during moving plates, enemies on the side either still is bad (you can just mow them down), and moving too (too confusing, you crash all the time)
    // finaler ansturm ist kein problem, wenn man weiß von welcher seite er anfängt
    // TODO 1: hard mode: alles ist um 90 grad gedreht! (color/dir changes every X.X seconds) -> but adjust affected enemies in double-plate sub-stages to be meaningful in both types
    // TODO 1: fix badges !!!!!!!
    // TODO 1: effect (+ sound) when getting forced and unforced (also for boss) (not permanent, but when entering/exiting)
    // TODO 1: maybe also move left-right! (plates can disappear by moving up and down) (maybe shrink center-plate when switching to left-right, or just begin rotating) (maybe also just skip top-bottom with 2 plates, and go straight to 4)
    // TODO 1: badge: versuch N mal zu turnen, anzeige beginnt nach M mal (oder gar keine anzeige, hidden badge, na zm. für die letzten 3 mal)
    // TODO 1: badge: N schwer zu erreichende gegner die sofort verschwinden, speziell markiert, black
    // TODO 1: badge: kugerl bei rush-gruppe einsammeln
    // TODO 1: badge: nicht zu lange auf einer platte, hin und her, timer für blau, etc.
    // TODO 1: badge: another plate flies across the screen with a certain enemy in sight, killing from the plate gives badge (oder für boss)
    // TODO 1: MAIN: task-check, regular score, badges, sound
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.3f),  coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.95f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(3u);
            pPath3->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f), 1.5f);
            pPath3->AddStop(coreVector2(0.0f, 0.9f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f), 2.5f);
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 110u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cStarEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_BLACK);
                pEnemy->AddStatus(ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(GAME_PLAYERS + 5u)
            STAGE_GET_UINT_ARRAY(aiTryCount, GAME_PLAYERS)
            STAGE_GET_UINT      (iBonusCount)
            STAGE_GET_UINT      (iTransitionState)
            STAGE_GET_FLOAT     (fTransitionTime)
            STAGE_GET_FLOAT     (fConveyerTime)
            STAGE_GET_FLOAT     (fConveyerSpeed)
        STAGE_GET_END

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB( 1u)) {STAGE_RESURRECT(pSquad1,   0u,   3u)}
            else if(STAGE_SUB( 2u)) {STAGE_RESURRECT(pSquad1,   4u,   7u)}
            else if(STAGE_SUB( 3u)) {STAGE_RESURRECT(pSquad1,   8u,  11u) STAGE_RESURRECT(pSquad2, 0u, 0u)}
            else if(STAGE_SUB( 4u)) {STAGE_RESURRECT(pSquad1,  12u,  15u)}
            else if(STAGE_SUB( 5u)) {STAGE_RESURRECT(pSquad1,  68u, 109u)}   // #
            else if(STAGE_SUB( 6u)) {STAGE_RESURRECT(pSquad1,  28u,  31u)}   // #
            else if(STAGE_SUB( 7u)) {STAGE_RESURRECT(pSquad1,  56u,  57u) STAGE_RESURRECT(pSquad2, 1u, 1u)}   // #
            else if(STAGE_SUB( 8u)) {STAGE_RESURRECT(pSquad1,  58u,  59u)}
            else if(STAGE_SUB( 9u)) {STAGE_RESURRECT(pSquad1,  60u,  67u)}
            else if(STAGE_SUB(10u)) {STAGE_RESURRECT(pSquad1,  16u,  19u) STAGE_RESURRECT(pSquad2, 2u, 2u)}   // #
            else if(STAGE_SUB(11u)) {STAGE_RESURRECT(pSquad1,  20u,  27u)}
            else if(STAGE_SUB(12u)) {STAGE_RESURRECT(pSquad1,  32u,  33u)}
            else if(STAGE_SUB(13u)) {STAGE_RESURRECT(pSquad1,  34u,  35u) STAGE_RESURRECT(pSquad2, 3u, 3u)}
            else if(STAGE_SUB(14u)) {STAGE_RESURRECT(pSquad1,  36u,  37u)}
            else if(STAGE_SUB(15u)) {STAGE_RESURRECT(pSquad1,  38u,  39u)}
            else if(STAGE_SUB(16u)) {STAGE_RESURRECT(pSquad1,  40u,  41u)}
            else if(STAGE_SUB(17u)) {STAGE_RESURRECT(pSquad1,  42u,  43u)}
            else if(STAGE_SUB(18u)) {STAGE_RESURRECT(pSquad1,  44u,  45u)}
            else if(STAGE_SUB(19u)) {STAGE_RESURRECT(pSquad1,  46u,  47u)}
            else if(STAGE_SUB(20u)) {STAGE_RESURRECT(pSquad1,  48u,  49u)}
            else if(STAGE_SUB(21u)) {STAGE_RESURRECT(pSquad1,  50u,  51u)}
            else if(STAGE_SUB(22u)) {STAGE_RESURRECT(pSquad1,  52u,  53u) STAGE_RESURRECT(pSquad2, 4u, 4u)}
            else if(STAGE_SUB(23u)) {STAGE_RESURRECT(pSquad1,  54u,  55u)}
            else if(STAGE_SUB(24u))
            {
                for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
                    this->DisablePlate(i, true);

                g_pEnvironment->SetTargetDirection(coreVector2(0.0f,1.0f), 1.0f);

                pSquad2->ClearEnemies(true);
            }

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;

            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.25f * STEP(1.0f, 24.0f, I_TO_F(m_iStageSub)));
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_GREEN);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + coreVector2(1.5f * BULLET_SPEED_FACTOR, 0.0f) * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        const coreBool bRotated = g_pGame->IsHard();

        if(STAGE_BEGINNING)
        {
            this->EnablePlate(0u, 0.0f, 0.0f, 0.0f, 0.5f);

            this->SetPlateRotated(0u, !bRotated);
            this->SetPlateRotated(1u, !bRotated);
            this->SetPlateRotated(2u,  bRotated);
            this->SetPlateRotated(3u,  bRotated);
            this->SetPlateRotated(4u,  bRotated);

            g_pSpecialEffects->CreateSplashColor(coreVector3(0.0f,0.0f,0.0f), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
        }

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 2u)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERPB(0.0f*PI, 0.5f*PI, MIN1(fTransitionTime))));

            this->SetPlateDirection(0u, g_pEnvironment->GetDirection());
            m_avPlateData[0].zw(coreVector2(1.0f,1.0f) * LERPB(0.5f, 0.75f, MIN(fTransitionTime, 1.0f)));
        }
        else if(m_iStageSub == 3u)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERPB(0.5f*PI, 1.0f*PI, MIN1(fTransitionTime))));

            this->SetPlateDirection(0u, g_pEnvironment->GetDirection());
            m_avPlateData[0].zw(coreVector2(1.0f,1.0f) * LERPB(0.75f, 1.0f, MIN(fTransitionTime, 1.0f)));
        }
        else if(m_iStageSub == 4u)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERPB(1.0f*PI, 1.5f*PI, MIN1(fTransitionTime))));

            this->SetPlateDirection(0u, g_pEnvironment->GetDirection());
            m_avPlateData[0].zw(coreVector2(1.0f,1.0f));
        }
        else if(m_iStageSub == 6u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                this->EnablePlate(1u, 0.5f, 0.25f, 0.0f, -0.5f);

                this->SetPlateOffset(0u, -0.25f);
                this->SetPlateScale (0u, -0.5f);

                this->SetPlateDirection(0u, coreVector2( 1.0f,0.0f));
                this->SetPlateDirection(1u, coreVector2(-1.0f,0.0f));
            }

            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERPB(1.5f*PI, 1.75f*PI, MIN1(fTransitionTime))));
        }
        else if(m_iStageSub == 10u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                this->EnablePlate(2u, -0.25f, -0.25f, -0.5f, -0.5f);
                this->EnablePlate(3u,  0.25f,  0.25f, -0.5f, -0.5f);

                this->SetPlateOffset(0u, -0.5f);
                this->SetPlateScale (0u,  0.0f);
                this->SetPlateOffset(1u,  0.5f);
                this->SetPlateScale (1u,  0.0f);

                this->SetPlateDirection(2u, coreVector2( 1.0f,0.0f));
                this->SetPlateDirection(3u, coreVector2(-1.0f,0.0f));
            }
            else if((iTransitionState == 1u) && (STAGE_SUBTIME_POINT(1.0f / RUTILUS_PLATE_SPEED)))
            {
                iTransitionState += 1u;

                this->EnablePlate(0u, -0.25f, -0.25f, -0.5f, -0.5f);

                this->DisablePlate(2u, false);

                this->SetPlateRotated(0u, bRotated);
                this->SetPlateRotated(1u, bRotated);
            }
        }
        else if(m_iStageSub == 12u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                this->EnablePlate(1u, 0.0f, -0.125f, 0.0f, -0.25f);
                this->EnablePlate(2u, 0.0f,  0.125f, 0.0f, -0.25f);
                this->EnablePlate(4u, 5.0f,  5.0f,   0.0f, -0.25f);

                this->SetPlateOffset(0u, -0.375f);
                this->SetPlateScale (0u, -0.25f);
                this->SetPlateOffset(3u,  0.375f);
                this->SetPlateScale (3u, -0.25f);

                this->SetPlateDirection(1u, coreVector2(-1.0f,0.0f));
                this->SetPlateDirection(2u, coreVector2( 1.0f,0.0f));
            }
        }
        else if(m_iStageSub >= 16u)
        {
            fConveyerSpeed = MIN1(fConveyerSpeed + 0.5f * TIME);
            fConveyerTime  = FmodRange(fConveyerTime - fConveyerSpeed * TIME * ((m_iStageSub >= 20u) ? -1.0f : 1.0f), 0.0f, 4.0f);

            for(coreUintW i = 0; i < 4u; ++i)
            {
                m_avPlateData[i].xy(coreVector2(1.0f,1.0f) * FmodRange((I_TO_F(i) - 1.5f - fConveyerTime) * 0.25f, -0.625f, 0.375f));
            }

            m_aPlateRaw[4].SetDirection(m_aPlateRaw[F_TO_UI(fConveyerTime)].GetDirection());   // without wrapper
            m_aPlateRaw[4].SetColor3   (m_aPlateRaw[F_TO_UI(fConveyerTime)].GetColor3());
            m_avPlateData[4].xy(coreVector2(1.0f,1.0f) * LERP(0.625f, 0.375f, FRACT(fConveyerTime)));
        }

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 68u) ? 1.0f : 1.2f, (i < 60u) ? (0.275f * I_TO_F(i % ((i < 32u) ? 4u : 2u)) + (((i < 20u) || (i >= 28u && i < 34u) || (i >= 40u && i < 42u)) ? 0.5f : 0.0f)) : ((i < 68u) ? (0.325f * I_TO_F((i - 60u) % 8u)) : (0.4f * I_TO_F((i - 68u) % 42u) + 1.0f)))

            if(i < 68u)
            {
                const coreSpline2* pPath = (i < 12u) ? pPath1 : pPath2;

                if(pPath == pPath2) STAGE_REPEAT(pPath->GetTotalDistance())

                if(i < 32u)
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(((I_TO_F(i % 4u) - 1.5f) * 0.275f) + ((i < 20u) ? 0.0f : ((i < 28u) ? 0.55f : -0.55f)), 0.0f);

                    pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                         if(i <  4u) {}
                    else if(i <  8u) pEnemy->Rotate270();
                    else if(i < 12u) pEnemy->Rotate180();
                    else if(i < 16u) pEnemy->Rotate90 ()->InvertX();
                    else if(i < 20u) pEnemy->Rotate90 ();
                    else if(i < 24u) pEnemy->Rotate180();
                    else if(i < 28u) {}
                    else if(i < 56u) {}
                }
                else if(i < 56u)
                {
                    coreFloat fSide;
                    switch(((i - 32u) / 2u) % 4u)
                    {
                    default: ASSERT(false)
                    case 0u: fSide = -3.5f; break;
                    case 1u: fSide =  3.5f; break;
                    case 2u: fSide = -1.5f; break;
                    case 3u: fSide =  1.5f; break;
                    }
                    fSide *= 0.275f;

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(fSide, 0.0f);

                    pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
                }
                else
                {
                    coreFloat fSide, fFlip;
                    switch(i - 56u)
                    {
                    default: ASSERT(false)
                    case  0u: fSide =  3.5f; fFlip = -1.0f; break;
                    case  1u: fSide =  3.5f; fFlip = -1.0f; break;
                    case  2u: fSide = -3.5f; fFlip = -1.0f; break;
                    case  3u: fSide = -3.5f; fFlip = -1.0f; break;
                    case  4u: fSide =  3.5f; fFlip =  1.0f; break;
                    case  5u: fSide = -3.5f; fFlip =  1.0f; break;
                    case  6u: fSide =  3.5f; fFlip =  1.0f; break;
                    case  7u: fSide = -3.5f; fFlip =  1.0f; break;
                    case  8u: fSide =  3.5f; fFlip =  1.0f; break;
                    case  9u: fSide = -3.5f; fFlip =  1.0f; break;
                    case 10u: fSide =  3.5f; fFlip =  1.0f; break;
                    case 11u: fSide = -3.5f; fFlip =  1.0f; break;
                    }
                    fSide *= 0.275f;

                    const coreVector2 vFactor = coreVector2(1.0f, fFlip);
                    const coreVector2 vOffset = coreVector2(fSide, 0.0f);

                    pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
                }
            }
            else
            {
                constexpr coreInt32 A[] =
                {
                    0, 2, 4, 2, 0, 2, 0, -2, -4, -6, -4, -2, 0, 2, 4, 2, 0, -2, 0, 0, 0, 0, 0, 0, 2, 0, 2, 4, 6, 4, 6, 4, 2, 0, 2, 4, 2, 4, 6, 4, 2, 0   // ikaruga
                };

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F(A[i - 68u]) * 0.275f * 0.5f, 0.0f);

                if(pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime))
                {
                    pEnemy->Kill(false);
                    bPostpone = true;
                }

                pEnemy->Rotate90();
            }

            if(pEnemy->ReachedDeath())
            {
                if(!g_pGame->IsEasy())
                {
                    for(coreUintW j = 0u; j < 4u; ++j)
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = StepRotated90(j);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, pEnemy, vPos, vDir)->ChangeSize(2.5f);
                    }

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                }

                if(i == 6u)
                {
                    pHelper->Resurrect(true);
                    pHelper->SetPosition(pEnemy->GetPosition());
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i == 1u) ? -0.5f : ((i == 2u) ? -0.5f : 0.0f), 0.0f);

            if(pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime))
            {
                pEnemy->Kill(false);
                bPostpone = true;
            }

                 if(i == 2u) pEnemy->Rotate180();
            else if(i == 3u) pEnemy->Rotate90 ();
            else if(i == 4u) pEnemy->Rotate180();

            if(pEnemy->ReachedDeath())
            {
                if(++iBonusCount == pSquad2->GetNumEnemies()) STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                else g_pGame->GetCombatText()->DrawProgress(iBonusCount, pSquad2->GetNumEnemies(), pEnemy->GetPosition());
            }
        });

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            if(!pPlayer->IsRolling() && pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_TURN))
            {
                if(HAS_BIT(pPlayer->GetInput()->iActionPress, PLAYER_ACTION_TURN_LEFT) ||
                   HAS_BIT(pPlayer->GetInput()->iActionPress, PLAYER_ACTION_TURN_RIGHT))
                {
                    constexpr coreUintW iTotal = 15u;

                    aiTryCount[i] += 1u;

                         if(aiTryCount[i] >= iTotal)      STAGE_BADGE(0u, BADGE_EASY, pPlayer->GetPosition())
                    else if(aiTryCount[i] >= iTotal - 3u) g_pGame->GetCombatText()->DrawCountdown(aiTryCount[i], iTotal, pPlayer->GetPosition());
                }
            }
        });

        if(!bPostpone) STAGE_WAVE(0u, "4-1", {50.0f, 75.0f, 100.0f, 125.0f})   // NEUNZEHN
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
            this->DisablePlate(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.25f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // screen rotation and rotated shooting
    // rotation at the start needs proper introduction, feedback and time, so the player can get accustomed
    // on further rotation-changes waiting time can be reduced to keep total time low, player understands already
    // rotation should not be too fast, the player can lose control, but not too slow, to make the mechanic significant and interesting
    // bullets can be made unaffected by rotation, but does not really feel better, bullets rotating with the background created better visual clusters
    // make sure to adjust start-time from diagonally flying enemies
    // both 4er groups coming from each side spawn 1243, which is a bit more interesting than 1234
    // first circle group shows that there are enemies which do not shoot, to make second non-shooting circle group in a more intense situation less surprising
    // shooting enemies coming fro multiple different sides was nearly impossible to implement, it is just too overwhelming (the double-diagonal group is an exception, because they merge into a single shoot-direction)
    // very spread-out linear-flying shooting enemies were also too overwhelming, every shooting group needs a single point of attack to compensate for the rotation difficulty
    // still circle group should not be too far on the outside, otherwise it is too annoying shooting them down, but not too much in the center, as it gets too easy
    // linear flying enemies and 4 corner enemies at the end both work by evading bullets which get affected by the rotation, while the enemies stand still, which creates some kind of maelstrom (similar to r-type leaver wave, or messier boss)
    // 4 corner enemies (both groups) are in blind spot on purpose
    // TASK: shoot multiple times on the helper, at specific intervals
    // TASK: kill a certain group of enemies in order
    // TASK: move into all 4 invisible corners
    // TODO 1: hardmode: player cannot rotate, is rotating all the time, player bullets rotate with background, rotation is based on player position or other actions
    // TODO 1: teilweise bessere bullet patterns ? https://www.youtube.com/watch?v=1uTQDKAN0sM https://www.youtube.com/watch?v=KJHt4cq1ti0
    // TODO 1: MAIN: task-check, regular score, sound
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2( 1.2f,-1.2f), coreVector2(-1.0f,1.0f).Normalized());
            pPath2->AddNode(coreVector2(-1.2f, 1.2f), coreVector2(-1.0f,1.0f).Normalized());
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(1.2f,-1.2f), coreVector2(-1.0f,1.0f).Normalized());
            pPath3->AddStop(coreVector2(0.9f,-0.9f), coreVector2(-1.0f,1.0f).Normalized());
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(1.2f,-1.2f), coreVector2(-1.0f,1.0f).Normalized());
            pPath4->AddStop(coreVector2(0.6f,-0.6f), coreVector2(-1.0f,1.0f).Normalized());
            pPath4->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 116u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);   // due to tight spawning
            });
        });

        STAGE_GET_START(11u)
            STAGE_GET_UINT (iTransitionState)
            STAGE_GET_FLOAT(fTransitionTime)
            STAGE_GET_FLOAT(fRotationValue)
            STAGE_GET_FLOAT(fRotationSpeed)
            STAGE_GET_FLOAT(fRotationFrom)
            STAGE_GET_FLOAT(fRotationTo)
            STAGE_GET_FLOAT(fCloseTime)
            STAGE_GET_FLOAT(fCapsDelay)
            STAGE_GET_UINT (iCapsCount)
            STAGE_GET_UINT (iOrderState)
            STAGE_GET_UINT (iEdgeState)
        STAGE_GET_END

        const auto nScreenSplashFunc = [](const coreVector2 vSplashPos)
        {
            for(coreUintW i = 0u; i < 4u; ++i)
            {
                const coreVector2 vDir = StepRotated90(i);

                for(coreUintW j = 0u; j < 30u; ++j)
                {
                    const coreVector2 vPos = (vDir + vDir.Rotated90() * ((I_TO_F(j) - 14.5f) / 14.5f)) * 1.1f * FOREGROUND_AREA;

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 1u, COLOR_ENERGY_MAGENTA);
                }
            }

            g_pSpecialEffects->CreateSplashColor(coreVector3(vSplashPos, 0.0f), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_MAGENTA);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,   0u,   3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,   4u,   7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,   8u,  11u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  12u,  15u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  16u,  19u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  68u,  71u)   // #
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  20u,  23u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  24u,  27u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1,  28u,  31u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1,  32u,  35u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1,  36u,  39u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1,  40u,  43u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1,  44u,  51u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1,  92u,  99u)   // #
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1,  52u,  55u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1,  56u,  59u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1,  60u,  63u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1,  64u,  67u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1,  76u,  91u)   // #
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 100u, 115u)   // #
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad1,  72u,  75u)   // #
            else if(STAGE_SUB(22u)) STAGE_DELAY_START_CLEAR

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;

            g_pGame->SetVisibleCheck(m_iStageSub != 21u);

            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.25f + 0.25f * STEP(1.0f, 22.0f, I_TO_F(m_iStageSub)));
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect(false);
        }

        const coreFloat fRotationValueOld = fRotationValue;

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                pHelper->SetPosition(coreVector3(LERPB(1.08f, -1.2f, MAX(1.0f - fTransitionTime, 0.0f)) * FOREGROUND_AREA.x, 0.0f, 0.0f));

                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;

                    nScreenSplashFunc(pHelper->GetPosition().xy());

                    this->EnableCapsule();
                }
            }
            else if(iTransitionState == 1u)
            {
                fRotationValue = LERPB(0.25f*PI, 0.0f*PI, CLAMP(1.0f - (fTransitionTime - 1.5f) / 1.5f, 0.0f, 1.0f));

                if(fTransitionTime >= 3.0f)
                {
                    iTransitionState += 1u;

                    nScreenSplashFunc(pHelper->GetPosition().xy());
                }
            }
        }
        else if(m_iStageSub == 7u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                nScreenSplashFunc(pHelper->GetPosition().xy());
            }

            fRotationSpeed = MAX(fRotationSpeed - 0.2f * TIME, -0.6f);
        }
        else if(m_iStageSub == 15u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                nScreenSplashFunc(pHelper->GetPosition().xy());

                fRotationSpeed = -fRotationSpeed;
            }
        }
        else if(m_iStageSub == 22u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                fRotationSpeed = 0.0f;
                fRotationFrom  = (fRotationValue);
                fRotationTo    = (fRotationValue > 1.0f*PI) ? (2.0f*PI) : (0.0f*PI);

                this->DisableCapsule(true);
            }

            const coreFloat fTime = MAX0(1.0f - fTransitionTime);

            pHelper->SetPosition(coreVector3(LERPB(-1.2f, 1.08f, fTime) * FOREGROUND_AREA.x, 0.0f, 0.0f));

            fRotationValue = LERPB(fRotationTo, fRotationFrom, fTime);

            if(fTransitionTime >= 1.0f)
            {
                pHelper->Kill(false);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                STAGE_DELAY_END
            }
        }

        if(m_iStageSub == 6u)
        {
            if(pSquad1->GetNumEnemiesAlive() < 4u)
            {
                fCloseTime += 1.0f * TIME;
            }
        }

        if(m_iStageSub == 21u)
        {
            g_pGame->GetBulletManagerPlayer()->ForEachBullet([](cBullet* OUTPUT pBullet)
            {
                pBullet->AddStatus(BULLET_STATUS_IMMORTAL);
                if(pBullet->GetFlyTime() >= 3.0f) pBullet->Deactivate(true);
            });
        }
        else if(m_iStageSub == 22u)
        {
            g_pGame->GetBulletManagerPlayer()->ForEachBullet([](cBullet* OUTPUT pBullet)
            {
                pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
            });
        }

        if(m_Capsule.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            constexpr coreFloat  fCapsAnimation = 0.2f;
            constexpr coreUint32 iCapsTotal     = 5u;

            m_Capsule.SetPosition(pHelper->GetPosition());
            m_Capsule.SetSize    (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
            m_Capsule.SetAlpha   (1.0f - MIN1(fCapsDelay / fCapsAnimation));

            const coreFloat fCapsDelayOld = fCapsDelay;
            fCapsDelay = MAX0(fCapsDelay - 1.0f * TIME);

            if(InBetween(fCapsAnimation, fCapsDelay, fCapsDelayOld))
            {
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_MAGENTA);
            }

            if(fCapsDelay < fCapsAnimation)
            {
                Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &m_Capsule, [&](cBullet* OUTPUT pBullet, const coreObject3D* pCapsule, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    if(fCapsDelay < fCapsAnimation)
                    {
                        pBullet->Deactivate(true);

                        if(++iCapsCount >= iCapsTotal) STAGE_BADGE(1u, BADGE_NORMAL, m_Capsule.GetPosition())
                        else g_pGame->GetCombatText()->DrawCountdown(iCapsCount, iCapsTotal, m_Capsule.GetPosition());

                        fCapsDelay = (iCapsCount >= iCapsTotal) ? 3600.0f : 10.0f;

                        g_pSpecialEffects->MacroExplosionColorSmall(pHelper->GetPosition(), COLOR_ENERGY_MAGENTA);
                    }
                });
            }
        }

        fRotationValue = FmodRange(fRotationValue + fRotationSpeed * TIME, 0.0f*PI, 2.0f*PI);
        const coreVector2 vDirection       = coreVector2::Direction(fRotationValue);
        const coreVector2 vDirectionMirror = g_CurConfig.Game.iMirrorMode ? vDirection.InvertedX() : vDirection;

        g_pPostProcessing->SetDirectionGame(vDirection);

        if(fRotationValue != fRotationValueOld)
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->ShowArrow(0u);
            });
        }

        const auto nMarkEdgeFunc = [&](const coreUintW iIndex)
        {
            if(!HAS_BIT(iEdgeState, iIndex))
            {
                ADD_BIT(iEdgeState, iIndex)

                const coreVector3 vPos   = coreVector3(MapToAxis(StepRotated90(iIndex), vDirection) * FOREGROUND_AREA * 0.95f, 0.0f);
                const coreUint32  iCount = coreMath::PopCount(iEdgeState);

                if(iCount == 4u) STAGE_BADGE(0u, BADGE_EASY, vPos)
                else g_pGame->GetCombatText()->DrawProgress(iCount, 4u, vPos);
            }
        };

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            const coreVector2 vRealPos = MapToAxisInv(pPlayer->GetPosition().xy(), vDirection);

                 if(vRealPos.x < -FOREGROUND_AREA.x * 1.2f) nMarkEdgeFunc(1u);
            else if(vRealPos.x >  FOREGROUND_AREA.x * 1.2f) nMarkEdgeFunc(3u);
                 if(vRealPos.y < -FOREGROUND_AREA.x * 1.2f) nMarkEdgeFunc(2u);
            else if(vRealPos.y >  FOREGROUND_AREA.x * 1.2f) nMarkEdgeFunc(0u);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.9f, (i < 68u) ? (0.3f * I_TO_F(i % 4u) + ((i < 4u) ? 3.0f : ((i >= 8u && i < 16u) ? -0.3f : ((i >= 20u && i < 24u) ? 2.0f : ((i >= 52u && i < 56u) ? 1.0f : 0.0f))))) : ((i < 76u) ? 0.0f : ((i < 100u) ? (0.15f * I_TO_F((i - 76u) % 8u)) : (0.4f * I_TO_F(i - 100u)))))

            if(STAGE_LIFETIME_AFTER(0.0f) && pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);

            if(i < 68u)
            {
                const coreSpline2* pPath = ((i < 8u) || (i >= 20u && i < 36u) || (i >= 52u)) ? pPath1 : pPath2;
                const coreFloat    fSide = ((i < 8u) || (i >= 16u && i < 36u) || (i >= 52u)) ? ((i % 2u) ? 0.1f : -0.1f) : 0.0f;
                const coreVector2  vAxis = (pPath == pPath1) ? coreVector2(0.0f,1.0f) : (coreVector2(-1.0f,1.0f) * (1.0f / SQRT2));

                STAGE_REPEAT(pPath->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f) * ((i >= 8u && i < 20u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = MapToAxis(coreVector2(((i < 8u) ? 0.0f : ((i < 12u) ? -0.5f : ((i < 16u) ? 0.5f : ((i < 20u) ? 0.0f : ((i < 36u) ? -0.5f : 0.0f))))) + fSide, 0.0f), vAxis);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  4u) {}
                else if(i <  8u) pEnemy->Rotate270();
                else if(i < 20u) {}
                else if(i < 24u) pEnemy->Rotate90 ();
                else if(i < 28u) {}
                else if(i < 32u) pEnemy->Rotate180();
                else if(i < 36u) pEnemy->Rotate270();
                else if(i < 40u) pEnemy->Rotate270();
                else if(i < 44u) pEnemy->Rotate180();
                else if(i < 48u) pEnemy->Rotate90 ();
                else if(i < 52u) {}
                else if(i < 56u) {}
                else if(i < 60u) pEnemy->Rotate90 ();
                else if(i < 64u) pEnemy->Rotate270();
                else if(i < 68u) pEnemy->Rotate180();
            }
            else if(i < 76u)
            {
                const coreFloat fCloseMove = (i < 72u) ? LERPS(1.0f, 0.3f, MIN1(fCloseTime * 0.2f)) : 1.0f;

                const coreVector2 vFactor = coreVector2(1.0f,1.0f) * fCloseMove;
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 68u || i == 72u) {}
                else if(i == 69u || i == 73u) pEnemy->Rotate90 ();
                else if(i == 70u || i == 74u) pEnemy->Rotate180();
                else if(i == 71u || i == 75u) pEnemy->Rotate270();
            }
            else if(i < 92u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->ToAxis(coreVector2::Direction(I_TO_F(i - 76u) / 16.0f * (-2.0f*PI) + (0.75f*PI)));
            }
            else if(i < 100u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->ToAxis(coreVector2::Direction(I_TO_F(i - 92u) / 8.0f * (-2.0f*PI) + (1.75f*PI)));

                if(iOrderState < 8u)
                {
                    if((i - 92u) == iOrderState)
                    {
                        if(!pEnemy->HasStatus(ENEMY_STATUS_CUSTOM))
                        {
                            pEnemy->SetBaseColor(COLOR_SHIP_BLACK, false, true);
                            pEnemy->AddStatus   (ENEMY_STATUS_CUSTOM);
                        }

                        g_pGame->GetCombatText()->AttachMarker(i % COMBAT_MARKERS, "X", pEnemy->GetPosition(), COLOR_MENU_INSIDE);
                    }

                    if(pEnemy->ReachedDeath())
                    {
                        if((i - 92u) == iOrderState)
                        {
                            if(++iOrderState == 8u) STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                        }
                        else
                        {
                            STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
                            {
                                if(pEnemy2->HasStatus(ENEMY_STATUS_CUSTOM))
                                {
                                    pEnemy2->SetBaseColor(COLOR_SHIP_RED);
                                }
                            });

                            iOrderState = 0xFFu;
                        }
                    }
                }
            }
            else if(i < 116u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }

            if((i >= 52u && i < 68u) || (i >= 72u && i < 92u) || (i >= 100u))
            {
                pEnemy->ToAxis(vDirectionMirror);
            }

            if(((i < 76u) || (i >= 100u)) && STAGE_TICK_LIFETIME_BASE(6.0f, 0.0f) && (!g_pGame->IsEasy() || ((i >= 68u && i < 76u) ? ((s_iTick % 10u) < 4u) : (s_iTick < 4u))))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual((i / 4u) % 2u).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.5f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_MAGENTA);
            }
        });

        STAGE_WAVE(1u, "4-2", {55.0f, 80.0f, 110.0f, 135.0f})   // ZWANZIG
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        g_pGame->SetVisibleCheck(true);

        g_pGame->GetBulletManagerPlayer()->ForEachBullet([](cBullet* OUTPUT pBullet)
        {
            pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
        });

        g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,1.0f));

        this->DisableCapsule(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.5f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // slowdown areas
    // bubble works as shield to hide from attacks
    // side-moving sphere works with continue attack, but not with wave-attack, front-moving sphere never works, round-moving sphere not sure
    // first group shows the speed-reduction without attacks, next one with attacks
    // enemies should attack from the opposite side in coop, so target-player can hide behind bubble
    // enemies in matrix wave should already get a bit into middle without slow-down (which highlights the effect), except on the side where player will likely be
    // sobald speedup-bubble erscheint kommen 4 gegner gleichzeitig und können schnell getötet werden, das ist ok, das problem für den spieler sind die geschosse die dann schon auf dem weg sind
    // zwei gegner hintereinander sollten nicht von der selben stelle spawnen, durch die zeit-verzögerten bullets sind die gleich wieder tot weil der nachhall so lang ist
    // zwei gegner gleichzeitig mit weitem wellen-angriff funktioniert nicht, die geschosse überlagern sich zu sehr, oder bei shift kommen die angriffe zu oft
    // bei finalen wellen-angriff gegnern müssen die schon 1 mal schießen bevor sie zerstört werden können
    // TASK: collect fast objects, which can only be caught within slowdown bubble
    // TODO 1: hardmode: slowdown/safe field follows player
    // TODO 1: effekt um verlangsamte objekte herum (schein)
    // TODO 1: player bullets sollten nicht so stark gekürzt werden
        // TODO 1: enemies in matrix wave may need a different configuration, to have more movement possibilities
        // TODO 1: nochmal schauen ob man den wellen-angriffen der letzten gegner gescheit ausweichen kann (hab bubble-größe wieder zurückgesetzt von 22 auf 20, könnte damit zu tun haben), ansonsten vielleicht offsets ändern 0.5f,0.0f,0.5f,0.0f
        // TODO 1: gegner links und rechts nach matrix phase, sollten vielleicht von 4+4 auf 2+2+4 geändert werden
    // TODO 1: MAIN: task-check, hard idea, regular score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, background rota/speed
    // TODO 1: further slowdown: enemy exhaust, particle effects, sound effects, bubble
    // TODO 1:  #### vielleicht gerade gegner-angriffe (wenn sich bubble bewegt)
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        constexpr coreUintW iNumData = 6u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 41u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(30, 0u, COLOR_SHIP_PURPLE);

                if(i < 4u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F(i) - 1.5f) * 0.2f, 1.3f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                }
                else if(i < 10u)
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F((i - 4u) % 3u) * ((4.0f/6.0f)*PI) - ((1.0f/6.0f)*PI)) * coreVector2((i < 7u) ? -1.0f : 1.0f, 1.0f);
                    const coreVector2 vPos = vDir * 1.4f;

                    pEnemy->SetPosition (coreVector3( vPos, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(-vDir, 0.0f));
                }
                else if(i < 19u)
                {
                }
                else if(i < 25u)
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(i - 19u) / 6.0f * (2.0f*PI));
                    const coreVector2 vPos = vDir * (1.6f + I_TO_F(i - 19u) * 0.2f) + vDir.Rotated90() * 0.3f;

                    pEnemy->SetPosition (coreVector3( vPos, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(-vDir, 0.0f));
                }
                else if(i < 29u)
                {
                    const coreVector2 vDir = StepRotated90(i % 4u);
                    const coreVector2 vPos = vDir * 1.3f;

                    pEnemy->SetPosition (coreVector3( vPos, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(-vDir, 0.0f));
                }
            });
        });

        STAGE_GET_START(5u + iNumData)
            STAGE_GET_UINT      (iTransitionState)
            STAGE_GET_FLOAT     (fTransitionTime)
            STAGE_GET_FLOAT     (fAreaMove)
            STAGE_GET_FLOAT     (fAreaLerp)
            STAGE_GET_UINT      (iTockState)
            STAGE_GET_UINT_ARRAY(aiOwnTick, iNumData)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u,  4u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  5u,  5u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  6u,  6u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 10u, 10u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 11u, 11u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 12u, 12u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 13u, 14u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 16u, 16u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 17u, 17u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 18u, 18u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 19u, 24u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 25u, 28u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 29u, 32u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 33u, 36u)
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 37u, 37u)
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad1, 38u, 38u)
            else if(STAGE_SUB(22u)) STAGE_RESURRECT(pSquad1, 39u, 39u)
            else if(STAGE_SUB(23u)) STAGE_RESURRECT(pSquad1, 40u, 40u)
            else if(STAGE_SUB(24u)) STAGE_DELAY_START_CLEAR

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;

            std::memset(aiOwnTick, 0, sizeof(coreUint32) * iNumData);

            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.5f + 0.25f * STEP(1.0f, 24.0f, I_TO_F(m_iStageSub)));
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_PURPLE);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect(false);
        }

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                const coreFloat   fTime = MIN(fTransitionTime * 0.5f, 1.0f);
                const coreVector2 vDir  = coreVector2::Direction(LERP(1.5f*PI, -0.5f*PI, fTime));

                pHelper->SetPosition(coreVector3(vDir * LERP(-1.2f, 0.0f, fTime) * FOREGROUND_AREA.x, 0.0f));

                if(fTransitionTime >= 2.0f)
                {
                    iTransitionState += 1u;

                    this->EnableArea();
                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_PURPLE);

                    for(coreUintW i = 0u; i < RUTILUS_TOCKS; ++i)
                    {
                        const coreVector2 vPos = coreVector2(I_TO_F((i * 2u) % 5u) * -0.5f + 1.0f, I_TO_F(i) * 0.5f - 1.0f) + coreVector2(4.0f,4.0f);

                        this->EnableTock(i);
                        this->GetTock(i)->SetPosition(coreVector3(vPos, 0.0f) * FOREGROUND_AREA3);
                    }
                }
            }
        }
        else if((m_iStageSub >= 8u) && (m_iStageSub < 16u))
        {
            fAreaMove += 1.0f * TIME;

            if(m_iStageSub < 12u) fAreaLerp = MIN1(fAreaLerp + 0.5f * TIME);
                             else fAreaLerp = MAX0(fAreaLerp - 0.5f * TIME);

            pHelper->SetPosition(coreVector3(SIN(fAreaMove) * 0.5f * FOREGROUND_AREA.x * BLENDH3(fAreaLerp), 0.0f, 0.0f));
        }
        else if(m_iStageSub == 16u)
        {
            if(STAGE_BEGINNING2)
            {
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_PURPLE);
            }

            this->SetAreaScale(ParaLerp(1.0f, 0.8f, 4.2f, BLENDH3(MIN(m_fStageSubTime * 0.85f, 1.0f))));
        }
        else if(m_iStageSub == 17u)
        {
            if(STAGE_BEGINNING2)
            {
                fAreaMove = 0.0f;

                this->EnableSafe();
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_PURPLE);
            }

            if(m_fSafeTime < 2.0f)
            {
                const coreVector2 vPos   = m_Safe.GetPosition().xy();
                const coreFloat   fLenSq = POW2(m_Safe.GetSize().x);

                g_pGame->GetBulletManagerEnemy ()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    if((pBullet->GetPosition().xy() - vPos).LengthSq() < fLenSq) pBullet->Deactivate(true);
                });
            }
        }
        else if((m_iStageSub >= 18u) && (m_iStageSub < 24u))
        {
            fAreaMove += 1.0f * TIME;

            if(m_iStageSub < 20u) fAreaLerp = MIN1(fAreaLerp + 0.25f * TIME);
                             else fAreaLerp = MAX0(fAreaLerp - 0.25f * TIME);

            pHelper->SetPosition(coreVector3(0.0f, SIN(fAreaMove) * 0.5f * FOREGROUND_AREA.y * BLENDH3(fAreaLerp), 0.0f));
        }
        else if(m_iStageSub == 24u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                this->DisableArea(true);
                this->DisableSafe(true);
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_PURPLE);
            }

            if(fTransitionTime >= 0.6f)
            {
                pHelper->Kill(false);

                g_pSpecialEffects->MacroExplosionColorBig(pHelper->GetPosition(), COLOR_ENERGY_PURPLE);

                STAGE_DELAY_END
            }
        }

        m_aArea[0].SetPosition(m_Safe.IsEnabled(CORE_OBJECT_ENABLE_ALL) ? coreVector3(0.0f,0.0f,0.0f) : pHelper->GetPosition());
        m_Safe    .SetPosition(m_Safe.IsEnabled(CORE_OBJECT_ENABLE_ALL) ? pHelper->GetPosition()      : coreVector3(HIDDEN_POS, 0.0f));

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 4u) ? 2.0f : 0.0f)

            const coreFloat fSpeed        = this->CalcAreaSpeed(pEnemy->GetPosition().xy(), 1.0f);
            const coreFloat fSlowLifeTime = fLifeTime * ((i < 19u) ? 1.0f : 0.4f);

            if(i < 10u || (i >= 19u && i < 29u))
            {
                if(STAGE_LIFETIME_AFTER(0.0f)) pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 25.0f * fSpeed);

                coreVector2       vCurPos = pEnemy->GetPosition ().xy();
                const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.3f) && (vCurDir.x < 0.0f)) vCurPos.x += FOREGROUND_AREA.x * 2.6f;
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.3f) && (vCurDir.x > 0.0f)) vCurPos.x -= FOREGROUND_AREA.x * 2.6f;
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.3f) && (vCurDir.y < 0.0f)) vCurPos.y += FOREGROUND_AREA.y * 2.6f;
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.3f) && (vCurDir.y > 0.0f)) vCurPos.y -= FOREGROUND_AREA.y * 2.6f;

                pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            }
            else if(i < 15u)
            {
                coreFloat fSide;
                switch(i - 10u)
                {
                default: ASSERT(false)
                case 0u: fSide =  0.0f;  break;
                case 1u: fSide = -0.75f; break;
                case 2u: fSide =  0.75f; break;
                case 3u: fSide = -0.25f; break;
                case 4u: fSide =  0.25f; break;
                }
                const coreVector2 vFactor = coreVector2(1.0f,  1.0f);
                const coreVector2 vOffset = coreVector2(fSide, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fSlowLifeTime);
            }
            else if(i < 29u || i >= 37u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fSlowLifeTime);

                     if(i == 15u) {}
                else if(i == 16u) pEnemy->Rotate90 ();
                else if(i == 17u) pEnemy->Rotate180();
                else if(i == 18u) pEnemy->Rotate270();
                else if(i == 37u) {}
                else if(i == 38u) pEnemy->Rotate90 ();
                else if(i == 39u) pEnemy->Rotate180();
                else if(i == 40u) pEnemy->Rotate270();
            }
            else
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 29u) % 4u) - 1.5f) * 0.5f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fSlowLifeTime);

                pEnemy->Rotate90();

                     if(i == 29u || i == 31u || i == 34u || i == 36u) {}
                else if(i == 30u || i == 32u || i == 33u || i == 35u) pEnemy->InvertX();
            }

            if(i < 4u)
            {
            }
            else if(i < 15u || (i >= 19u && i < 37u))
            {
                if(STAGE_TICK_LIFETIME(10.0f, 0.0f) && g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy(), 1.2f))
                {
                    if(F_TO_UI(I_TO_F(s_iTick - 1u) * fSpeed) != F_TO_UI(I_TO_F(s_iTick) * fSpeed))
                    {
                        coreUint32& iOwnTick = STAGE_SINK_UINT(aiOwnTick[i % iNumData]);

                        if(!g_pGame->IsEasy() || ((iOwnTick % 8u) < 4u))
                        {
                            const coreVector2 vPos = pEnemy->GetPosition().xy();
                            const coreVector2 vDir = pEnemy->AimAtPlayerSideRev().Normalized();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                        }

                        iOwnTick += 1u;
                    }
                }
            }
            else
            {
                if(STAGE_TICK_LIFETIME(0.9f, (i >= 37u) ? 1.5f : 0.0f))
                {
                    if(F_TO_UI(I_TO_F(s_iTick - 1u) * fSpeed) != F_TO_UI(I_TO_F(s_iTick) * fSpeed))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = pEnemy->AimAtPlayerDual((i - 15u) % 2u).Angle();
                        const coreUintW   iNum  = g_pGame->IsEasy() ? 15u : 30u;

                        for(coreUintW j = iNum; j--; )
                        {
                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iNum - 1u) * 0.5f) * 4.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                        }
                    }
                }
            }
        });

        for(coreUintW i = 0u; i < RUTILUS_TOCKS; ++i)
        {
            coreObject3D* pTock = this->GetTock(i);
            if(!pTock->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreFloat fSpeed  = this->CalcAreaSpeed(pTock->GetPosition().xy(), 3.0f);
            const coreBool  bActive = (fSpeed < 0.5f);

            coreVector2 vCurDir = coreVector2(-1.0f,-3.0f).Normalized();
            coreVector2 vCurPos = pTock->GetPosition().xy() + vCurDir * (fSpeed * 50.0f * TIME);

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) vCurPos.x += FOREGROUND_AREA.x * 2.4f;
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) vCurPos.x -= FOREGROUND_AREA.x * 2.4f;
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) vCurPos.y += FOREGROUND_AREA.y * 2.4f;
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) vCurPos.y -= FOREGROUND_AREA.y * 2.4f;

            pTock->SetPosition(coreVector3(vCurPos, 0.0f));
            pTock->SetAlpha   (bActive ? 1.0f : 0.5f);

            if(bActive) STAGE_FOREACH_PLAYER(pPlayer, j)
            {
                const coreVector2 vDiff = pTock->GetPosition().xy() - pPlayer->GetPosition().xy();
                if(vDiff.LengthSq() < POW2(5.0f))
                {
                    this->DisableTock(i, true);

                    if(++iTockState == RUTILUS_TOCKS) STAGE_BADGE(1u, BADGE_NORMAL, pTock->GetPosition())
                    else g_pGame->GetCombatText()->DrawProgress(iTockState, RUTILUS_TOCKS, pTock->GetPosition());

                    g_pSpecialEffects->PlaySound(pTock->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iTockState, RUTILUS_TOCKS), SOUND_PLACEHOLDER);
                }
            });
        }

        STAGE_WAVE(2u, "4-3", {60.0f, 80.0f, 100.0f, 120.0f})   // EINUNDZWANZIG
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        this->DisableArea(false);
        this->DisableSafe(false);

        for(coreUintW i = 0u; i < RUTILUS_TOCKS; ++i)
            this->DisableTock(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.75f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // gravitation influences bullets
    // gravity both requires to get near to enemies, and allows to shoot in a curve
    // geschoss-gruppe muss mit selber flugbahn erzeugt werden um die gravitation leichter nachvollziehbar zu machen
    // alle gegner sollen gleichzeitig schießen, wodurch flugbahnen clustern und leichter nachvollziehbar sind
    // bewegende gegner sind zu leicht zu treffen (stille gegner erfordern es sich mit der mechanik auseinander zu setzen), einige ausnahmen können eingebaut werden zum relaxen
    // gravity with distance-falloff is too inconsistent regarding gameplay (too far away is boring, to near is too unpredictable)
    // speed-preservation is easy to grasp, but the created flight-patterns are too boring
    // vertikal und horizontal hat identisches gameplay, der einzige unterschied ist, dass eines geschosse abstoßt und das andere anzieht
    // TASK: attack multiple targets at the same time
    // TASK: fly multiple times around the helper
    // TODO 1: gravitation should work equally with all bullet types (basis-speed verwenden, von cWeapon, oder eher bullet, muss ich eh speichern für längen-veränderung)
    // TODO 1: distortion for waves ?
    // TODO 1: improve player bullet curve shape, with special vertex shader and different bullet entities
    // TODO 1: improve coop targeting
    // TODO 1: improve boring middle enemy-waves (first 3 are good, last 2 are good)
    // TODO 1: bullet-linien sollten nicht unterbrochen werden, zumindest am anfang, nicht so wichtig beim kill
    // TODO 1: MAIN: task-check, easy, hard idea, coop, regular score, badges, sound, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreUintW iNumTargets = 16u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f).Normalized());
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f).Normalized());
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 108u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(4u * iNumTargets + GAME_PLAYERS + 4u)
            STAGE_GET_VEC2_ARRAY (avStart,  iNumTargets)
            STAGE_GET_VEC2_ARRAY (avTarget, iNumTargets)
            STAGE_GET_UINT       (iTransitionState)
            STAGE_GET_FLOAT      (fTransitionTime)
            STAGE_GET_FLOAT_ARRAY(afAroundAngle, GAME_PLAYERS)
            STAGE_GET_UINT       (iAroundCount)
            STAGE_GET_UINT       (iSlapState)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumTargets)

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,   7u)   // #
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  8u,  23u)   // #
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 48u,  59u)   // ##
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 60u,  67u)   // ###
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 68u,  75u)   // ###
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 76u,  83u)   // ###
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 24u,  31u)   // #
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 32u,  47u)   // #
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 84u,  91u)   // ####
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 92u, 107u)   // ####
            else if(STAGE_SUB(11u)) STAGE_DELAY_START_CLEAR

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;

            for(coreUintW i = 0u; i < iNumTargets; ++i)
                avStart[i] = HIDDEN_POS;

            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.75f + 0.25f * STEP(1.0f, 11.0f, I_TO_F(m_iStageSub)));
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_RED);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect(false);
        }

        if(m_iStageSub >= 2u)
        {
            coreBool bSlapSolved = true;
            for(coreUintW i = 0u; i < RUTILUS_SLAPS; ++i)
            {
                coreObject3D* pSlap = this->GetSlap(i);
                if(!pSlap->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                if(m_afSlapValue[i] < 0.95f) bSlapSolved = false;
            }

            if(bSlapSolved && (iSlapState < 4u))
            {
                for(coreUintW i = 0u; i < RUTILUS_SLAPS; ++i)
                    this->DisableSlap(i, true);

                if(iSlapState == 0u)
                {
                    for(coreUintW i = 0u; i < 2u; ++i)
                        this->EnableSlap(i);

                    this->GetSlap(0u)->SetPosition(coreVector3(-0.5f, 0.0f,0.0f) * FOREGROUND_AREA3);
                    this->GetSlap(1u)->SetPosition(coreVector3( 0.5f, 0.0f,0.0f) * FOREGROUND_AREA3);
                }
                else if(iSlapState == 1u)
                {
                    for(coreUintW i = 0u; i < 3u; ++i)
                        this->EnableSlap(i);

                    this->GetSlap(0u)->SetPosition(coreVector3(-0.7f,-0.7f,0.0f) * FOREGROUND_AREA3);
                    this->GetSlap(1u)->SetPosition(coreVector3( 0.7f, 0.7f,0.0f) * FOREGROUND_AREA3);
                    this->GetSlap(2u)->SetPosition(coreVector3( 0.0f, 0.5f,0.0f) * FOREGROUND_AREA3);
                }
                else if(iSlapState == 2u)
                {
                    for(coreUintW i = 0u; i < 4u; ++i)
                        this->EnableSlap(i);

                    this->GetSlap(0u)->SetPosition(coreVector3(-0.2f,-0.5f,0.0f) * FOREGROUND_AREA3);
                    this->GetSlap(1u)->SetPosition(coreVector3(-0.2f, 0.5f,0.0f) * FOREGROUND_AREA3);
                    this->GetSlap(2u)->SetPosition(coreVector3( 0.2f,-0.5f,0.0f) * FOREGROUND_AREA3);
                    this->GetSlap(3u)->SetPosition(coreVector3( 0.2f, 0.5f,0.0f) * FOREGROUND_AREA3);
                }
                else if(iSlapState == 3u)
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, coreVector3(0.25f,0.0f,0.0f) * FOREGROUND_AREA3);
                }

                iSlapState += 1u;
            }

            if(m_iStageSub == 11u)
            {
                for(coreUintW i = 0u; i < RUTILUS_SLAPS; ++i)
                    this->DisableSlap(i, true);

                iSlapState = 100u;
            }
        }

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                pHelper->SetPosition(coreVector3(0.0f, LERPB(-1.2f, 0.0f, MIN(fTransitionTime, 1.0f)) * FOREGROUND_AREA.y, 0.0f));

                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;

                    this->EnableWave(1u);
                    this->SetWaveDelayed(false);
                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
                }
            }
        }
        else if(m_iStageSub == 3u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
            }

            const coreVector2 vDir = coreVector2::Direction(LERPB(0.0f*PI, -0.25f*PI, MIN1(m_fStageSubTime * 0.5f)));
            this->SetWaveDirection(vDir);
        }
        else if(m_iStageSub == 5u)
        {
            if(iTransitionState == 0u)
            {
                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;

                    this->SetWavePull (true);
                    this->SetWavePower(1.5f);
                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
                }
            }
        }
        else if(m_iStageSub == 7u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
            }

            const coreVector2 vDir = coreVector2::Direction(LERPB(-0.25f*PI, -0.5f*PI, MIN1(m_fStageSubTime * 0.5f)));
            this->SetWaveDirection(vDir);
        }
        else if(m_iStageSub == 9u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
            }

            const coreVector2 vPos = coreVector2(0.0f, LERPB(0.0f, -0.9f, MIN1(m_fStageSubTime * 0.5f)));
            pHelper->SetPosition(coreVector3(vPos * FOREGROUND_AREA, pHelper->GetPosition().z));
        }
        else if(m_iStageSub == 11u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                this->DisableWave(true);
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
            }

            pHelper->SetPosition(coreVector3(ParaLerp(0.0f, 0.3f, -1.2f, fTransitionTime / 0.8f) * FOREGROUND_AREA.x, pHelper->GetPosition().yz()));

            if(fTransitionTime >= 0.8f)
            {
                pHelper->Kill(false);

                STAGE_DELAY_END
            }
        }

        if(HAS_BIT(m_iWaveActive, RUTILUS_WAVES))
        {
            this->SetWavePosition(pHelper->GetPosition().xy());

            if(STAGE_TICK_FREE(10.0f, 0.0f))
            {
                if(this->GetWavePull()) g_pSpecialEffects->CreateChargeColor(pHelper->GetPosition(), 25.0f, 1u, COLOR_ENERGY_RED);
                                   else g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), 25.0f, 1u, COLOR_ENERGY_RED);
            }

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                const coreVector2 vDiff    = pPlayer->GetPosition().xy() - pHelper->GetPosition().xy();
                const coreVector2 vDiffOld = pPlayer->GetOldPos()        - pHelper->GetOldPos();

                if(!vDiff.IsNull() && !vDiffOld.IsNull())
                {
                    afAroundAngle[i] += AngleDiff(vDiff.Angle(), vDiffOld.Angle());

                    if(ABS(afAroundAngle[i]) >= 2.0f*PI)
                    {
                        afAroundAngle[i] = FMOD(afAroundAngle[i], 2.0f*PI);
                        iAroundCount    += 1u;
                    }
                }

                     if(iAroundCount >= 4u) STAGE_BADGE(0u, BADGE_EASY, pHelper->GetPosition())
                else if(iAroundCount >= 1u) g_pGame->GetCombatText()->AttachMarker(0u, coreData::ToChars(4u - iAroundCount), pHelper->GetPosition(), COLOR_MENU_INSIDE);
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 60u) ? 1.0f : 0.7f, (i < 84u) ? ((i < 8u || (i >= 24u && i < 32u) || (i >= 48u && i < 60u)) ? 1.0f : ((i >= 68u && i < 76u) ? 2.0f : 0.2f)) : ((i < 92u) ? (0.4f * I_TO_F(i - 84u) + 1.0f) : (0.4f * I_TO_F(i - 92u) + 0.2f)))

            if(i < 48u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 8u) - 3.5f) * 0.28f, (i % 2u) ? -0.05f : 0.05f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  8u) {}
                else if(i < 16u) pEnemy->Rotate90 ();
                else if(i < 24u) pEnemy->Rotate270();
                else if(i < 32u) {}
                else if(i < 40u) pEnemy->Rotate90 ();
                else if(i < 48u) pEnemy->Rotate270();
            }
            else if(i < 60u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 48u) % 3u) - 1.0f) * 0.28f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 51u) {}
                else if(i < 54u) pEnemy->Rotate90 ();
                else if(i < 57u) pEnemy->Rotate180();
                else if(i < 60u) pEnemy->Rotate270();
            }
            else if(i < 84u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 60u) % 8u) - 3.5f) * 0.28f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                      if(i < 68u) {}
                 else if(i < 76u) pEnemy->Rotate90 ();
                 else if(i < 84u) pEnemy->Rotate270();
            }
            else if(i < 92u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i - 84u) - 3.5f) * 0.28f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
                if(i % 2u) pEnemy->InvertX();
            }
            else if(i < 108u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 92u + 1u) * 3u) % 8u) - 3.5f) * 0.28f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayerSide().Normalized(), 0.0f));

            const coreFloat fShootSpeed = 9.0f / fLifeSpeed;

            if(!g_pGame->IsEasy() && STAGE_TICK_LIFETIME_BASE(fShootSpeed, (i < 84u) ? 0.0f : (fLifeOffset * -fShootSpeed)) && ((s_iTick % 13u) >= 9u))
            {
                coreVector2& vStart  = STAGE_SINK_VEC2(avStart [i % iNumTargets]);
                coreVector2& vTarget = STAGE_SINK_VEC2(avTarget[i % iNumTargets]);

                if(((s_iTick % 13u) == 9u) || (vStart == HIDDEN_POS))
                {
                    vStart  = pEnemy->GetPosition().xy();
                    vTarget = pEnemy->NearestPlayerSide()->GetPosition().xy();
                }

                const coreVector2 vPos =  vStart;
                const coreVector2 vDir = (vTarget - vPos).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.6f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_YELLOW);
            }
        });

        STAGE_WAVE(3u, "4-4", {50.0f, 75.0f, 100.0f, 125.0f})   // ZWEIUNDZWANZIG
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        this->DisableWave(false);

        for(coreUintW i = 0u; i < RUTILUS_SLAPS; ++i)
            this->DisableSlap(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pEnvironment->GetBackground()->SetGroundDensity(0u, 1.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // meteor split-up with enemies at the end
    // wenn zerstört fliegt gegner in richtung wo er raus schaut dreht sich und schießt wield um sich, verhält sich sonst wie meteorit
    // bounce zwischen meteoriten is total nicht nachvollziehbar, Ikaruga und RSG haben sowas nicht (nur bounce mit wand und konsistente aufteilung)
    // meteoriten mit gegnern darin müssen etwas mehr aushalten um nicht unabsichtlich beim erzeugen schon zerstört zu werden
    // middle meteors flying in line are faster, as an additional modifier
    // they are called meteors consistently, but actually they are asteroids
    // violette gegner am ende müssen vorher introduced werden, deswegen ist es wichtig einen davor schon zu erzeugen, aber eher bei wenigen meteoriten, damit man den gegner leichter beobachten kann
    // rundum gegner sollten nicht vollgas in die mitte fahren (sondern mit winkel), da es leichter ist die gegner rechtzeitig zu erkennen und ihnen auszuweichen
    // mittlere meteoriten erzeugen kleine meteoriten abhängig von ihrem winkel, es sollten keine kleinen erzeugt werden die sich achsen-ausgerichtet bewegen (2 von ihnen werden sonst sofort wieder zerstört)
    // TASK: destroy the big meteorite before impact
    // TASK: heb dir meteoriten vom anfang bis zu einer bestimmten sub-phase auf
    // TASK: destroy the debris meteor and collect all debris bevor they disappear
    // TODO 1: hard mode: every small meteor splits into 4 more smaller meteors
    // TODO 1: hard mode: indestructible meteors
    // TODO 1: hard mode: meteor bounce
    // TODO 1: orb angriff is zu ähnlich zur snow-wave
    // TODO 1: entweder hier oder bei boss, die kleinen meteoriten, wenn sie zerstört werden fliegen auf den bildschirm und erzeugen kleine cracks (keine distortion, nur decal)
    // TODO 1: MAIN: task-check, regular score, sound, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        constexpr coreUintW iNumMeteors = 21u;   // including big meteor
        constexpr coreUintW iNumAdds2   = 6u;
        constexpr coreUintW iNumAdds3   = 8u;
        constexpr coreUintW iLeftover   = 2u;
        constexpr coreFloat fBaseScale  = 1.5f;

        STAGE_ADD_SQUAD(pSquad1, cMeteorEnemy, iNumMeteors)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(1, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_SECRET);

                if(g_pGame->IsEasy()) pEnemy->AddStatus(ENEMY_STATUS_GHOST_PLAYER);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, iNumAdds2)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.6f);
                pEnemy->Configure(50, 0u, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            });
        });

        STAGE_ADD_SQUAD(pSquad3, cArrowEnemy, iNumAdds3)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad3, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.6f);
                pEnemy->Configure(1, 0u, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            });
        });

        STAGE_GET_START(iNumMeteors * 3u + iNumAdds2 * 5u + iNumAdds3 * 1u + iLeftover + 9u)
            STAGE_GET_VEC2_ARRAY (avMeteorMove,     iNumMeteors)
            STAGE_GET_FLOAT_ARRAY(afMeteorRotation, iNumMeteors)
            STAGE_GET_VEC2_ARRAY (avAddMove,        iNumAdds2)
            STAGE_GET_FLOAT_ARRAY(afAddRotation,    iNumAdds2)
            STAGE_GET_FLOAT_ARRAY(afAddOffset,      iNumAdds2)
            STAGE_GET_UINT_ARRAY (aiAddIndex,       iNumAdds2 + iNumAdds3)
            STAGE_GET_UINT_ARRAY (aiBeginIndex,     iLeftover, for(coreUintW i = 0u; i < iLeftover; ++i) aiBeginIndex[i] = UINT32_MAX;)
            STAGE_GET_UINT       (iCurMeteorIndex)
            STAGE_GET_UINT       (iCurAdd2Index)
            STAGE_GET_UINT       (iCurAdd3Index)
            STAGE_GET_UINT       (iTypeMap)
            STAGE_GET_UINT       (iImpact)
            STAGE_GET_UINT       (iCreateAdd)
            STAGE_GET_UINT       (iDebrisCount)
            STAGE_GET_UINT       (iHelperIndex,  iHelperIndex  = UINT32_MAX)
            STAGE_GET_UINT       (iCrumbleIndex, iCrumbleIndex = UINT32_MAX)
        STAGE_GET_END

        const auto nSpawnMeteorFunc = [&](const coreUintW iIndex, const coreVector2 vPos, const coreVector2 vMove, const coreUint8 iType)
        {
            for(coreUintW i = 0u; i < iNumMeteors; ++i)
            {
                if(++iCurMeteorIndex >= iNumMeteors) iCurMeteorIndex = 1u;
                if(!iType) iCurMeteorIndex = 0u;

                cEnemy* pNewEnemy = pSquad1->GetEnemy(iCurMeteorIndex);

                if(pNewEnemy->HasStatus(ENEMY_STATUS_DEAD) && !pNewEnemy->ReachedDeath())
                {
                    coreFloat fScale;
                    coreFloat fSpeed;
                    coreInt32 iHealth;
                    switch(iType)
                    {
                    default: ASSERT(false)
                    case 0u: fScale = 11.0f * fBaseScale; fSpeed = 50.0f; iHealth = 120; break;
                    case 1u: fScale =  3.0f * fBaseScale; fSpeed = 40.0f; iHealth =  10; break;
                    case 2u: fScale =  5.0f * fBaseScale; fSpeed = 25.0f; iHealth =  50; break;
                    case 3u: fScale =  3.0f * fBaseScale; fSpeed = 40.0f; iHealth = 100; break;
                    }

                    pNewEnemy->SetPosition (coreVector3(vPos,     0.0f) * FOREGROUND_AREA3);
                    pNewEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * fScale);
                    pNewEnemy->SetBaseColor((iType == 3u) ? (COLOR_SHIP_YELLOW / COLOR_SHIP_YELLOW.Max()) : (COLOR_SHIP_PURPLE / COLOR_SHIP_PURPLE.Max()));
                    pNewEnemy->SetMaxHealth(iHealth);
                    pNewEnemy->Resurrect();

                    avMeteorMove    [iCurMeteorIndex] = vMove * fSpeed;
                    afMeteorRotation[iCurMeteorIndex] = I_TO_F(iIndex) * (0.25f*PI);   // needs to be a deterministic offset

                    SET_BIT(iTypeMap, iCurMeteorIndex, (iType == 2u))
                    STATIC_ASSERT(iNumMeteors <= sizeof(coreUint32)*8u)

                    return;
                }
            }

            WARN_IF(true) {}
        };

        const auto nSpawnAdd2Func = [&]()
        {
            ASSERT(iCurAdd2Index < iNumAdds2)
            aiAddIndex[iCurAdd2Index] = iCurMeteorIndex;

            cEnemy* pAdd = pSquad2->GetEnemy(iCurAdd2Index);
            pAdd->Resurrect();

            cEnemy* pParent = pSquad1->GetEnemy(iCurMeteorIndex);
            pParent->SetBaseColor(COLOR_SHIP_BLUE / COLOR_SHIP_BLUE.Max());
            pParent->SetMaxHealth(50);
            pParent->SetCurHealth(50);

            iCurAdd2Index += 1u;
        };

        const auto nSpawnAdd3Func = [&]()
        {
            ASSERT(iCurAdd3Index < iNumAdds3)
            aiAddIndex[iCurAdd3Index + iNumAdds2] = iCurMeteorIndex;

            cEnemy* pAdd = pSquad3->GetEnemy(iCurAdd3Index);
            pAdd->Resurrect();

            cEnemy* pParent = pSquad1->GetEnemy(iCurMeteorIndex);
            pParent->SetBaseColor(COLOR_SHIP_CYAN / COLOR_SHIP_CYAN.Max());
            pParent->SetMaxHealth(30);
            pParent->SetCurHealth(30);

            iCurAdd3Index += 1u;
        };

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_CYAN);

        if(STAGE_BEGINNING)
        {
            constexpr coreUint8 aiNewOrder[] = {cOrbBullet::ID, cQuadBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        }

        coreUintW iActive = 0u;
        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)   // includes ReachedDeath()
        {
            iActive += HAS_BIT(iTypeMap, i) ? 4u : 1u;
        });

        if(iCrumbleIndex != UINT32_MAX)
        {
            if(pSquad1->GetEnemy(iCrumbleIndex)->ReachedDeath())
            {
                iCrumbleIndex = UINT32_MAX;
            }
            else
            {
                iActive += iLeftover;
            }
        }

        if(iActive <= iLeftover)   // not STAGE_CLEARED, it would be triggered by a lonely middle meteor explosion
        {
            if(STAGE_SUB(1u))
            {
                for(coreUintW i = 0u; i < 20u; ++i)
                {
                    const coreVector2 vPos  = coreVector2((I_TO_F((i * 3u) % 10u) - 4.5f) * 0.2f, 1.3f + I_TO_F(i) * 0.2f);
                    const coreVector2 vMove = coreVector2((i % 2u) ? -1.0f : 1.0f, -2.0f).Normalized();

                    nSpawnMeteorFunc(i, vPos, vMove, 1u);

                    if(i == 16u) nSpawnAdd3Func();
                }

                pHelper->Resurrect(false);
                iHelperIndex = iCurMeteorIndex;
            }
            else if(STAGE_SUB(2u))
            {
                coreUintW iNum = 0u;
                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    if(!pEnemy->HasStatus(ENEMY_STATUS_DEAD))
                    {
                        ASSERT(iNum < iLeftover)
                        aiBeginIndex[iNum++] = i;
                    }
                });

                for(coreUintW i = 0u; i < 10u; ++i)
                {
                    const coreVector2 vPos  = coreVector2((I_TO_F(i) - 4.5f) * 0.21f, 1.3f + I_TO_F(i) * 0.2f);
                    const coreVector2 vMove = coreVector2(0.0f,-1.0f);

                    nSpawnMeteorFunc(i, vPos, vMove, 1u);

                    if(i == 8u) nSpawnAdd3Func();
                }
            }
            else if(STAGE_SUB(3u))
            {
                for(coreUintW i = 0u; i < 10u; ++i)
                {
                    const coreFloat fSide = (i % 2u) ? -1.0f : 1.0f;

                    const coreVector2 vPos  = coreVector2(fSide * (1.3f + I_TO_F(i) * 0.2f), (I_TO_F(i) - 4.5f) * -0.21f);
                    const coreVector2 vMove = coreVector2(fSide * -1.0f, 0.0f);

                    nSpawnMeteorFunc(i, vPos, vMove, 1u);

                    if(i == 5u) nSpawnAdd3Func();
                }
            }
            else if(STAGE_SUB(4u))
            {
                for(coreUintW i = 0u; i < 10u; ++i)
                {
                    const coreVector2 vPos  = coreVector2(0.0f,1.3f);
                    const coreVector2 vMove = coreVector2::Direction((I_TO_F(i) - 4.5f) * (0.05f*PI) + (1.0f*PI));

                    nSpawnMeteorFunc(i, vPos, vMove, 1u);

                    if(i == 8u) nSpawnAdd3Func();
                }

                const coreVector2 vPos  = coreVector2(0.0f, 1.4f);
                const coreVector2 vMove = coreVector2(0.0f,-1.0f);

                nSpawnMeteorFunc(10u, vPos, vMove, 2u);
            }
            else if(STAGE_SUB(5u))
            {
                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    const coreVector2 vDir = StepRotated90(i);

                    const coreVector2 vPos  = (vDir * -1.4f);
                    const coreVector2 vMove = (vDir *  2.0f + vDir.Rotated90()).Normalized();

                    nSpawnMeteorFunc(i, vPos, vMove, 2u);
                }

                iCreateAdd = 1u;
            }
            else if(STAGE_SUB(6u))
            {
                for(coreUintW i = 0u; i < 8u; ++i)
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * (0.25f*PI));

                    const coreVector2 vPos  = (vDir * -1.3f * SQRT2);
                    const coreVector2 vMove = (vDir *  2.0f - vDir.Rotated90()).Normalized();

                    nSpawnMeteorFunc(i, vPos, vMove, 1u);

                    if(i % 2u) nSpawnAdd3Func();
                }
            }
            else if(STAGE_SUB(7u))
            {
                const coreVector2 vPos  = coreVector2(-1.3f,0.0f);
                const coreVector2 vMove = coreVector2( 1.0f,0.0f);

                nSpawnMeteorFunc(0u, vPos, vMove, 3u);
                iCrumbleIndex = iCurMeteorIndex;
            }
            else if(STAGE_SUB(8u))
            {
                const coreVector2 vDir = coreVector2(2.0f,-1.0f).Normalized();

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    const coreVector2 vPos  = coreVector2(-0.5f,1.4f) - vDir * (I_TO_F(i) * 0.5f);
                    const coreVector2 vMove = vDir * 2.0f;

                    nSpawnMeteorFunc(i, vPos, vMove, 2u);
                }

                iCreateAdd = 1u;
            }
            else if(STAGE_SUB(9u))
            {
                const coreVector2 vPos  = coreVector2(0.0f, 1.6f);
                const coreVector2 vMove = coreVector2(0.0f,-1.0f);

                nSpawnMeteorFunc(0u, vPos, vMove, 0u);
            }
            else if(STAGE_SUB(10u))
            {
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreBool bBig     = (i == 0u);
            const coreBool bMedium  = HAS_BIT(iTypeMap, i);
            const coreBool bCrumble = (i == iCrumbleIndex);

            if(pEnemy->ReachedDeath())
            {
                if(bBig)
                {
                    for(coreUintW j = 0u; j < 4u; ++j)
                    {
                        coreVector2 vDir;
                        switch(j)
                        {
                        default: ASSERT(false)
                        case 0u: vDir = coreVector2(-2.0f,1.0f).Normalized(); break;
                        case 1u: vDir = coreVector2(-1.0f,2.0f).Normalized(); break;
                        case 2u: vDir = coreVector2( 1.0f,2.0f).Normalized(); break;
                        case 3u: vDir = coreVector2( 2.0f,1.0f).Normalized(); break;
                        }

                        const coreVector2 vPos  = pEnemy->GetPosition().xy() / FOREGROUND_AREA + vDir * 0.1f;
                        const coreVector2 vMove = vDir;

                        nSpawnMeteorFunc(j, vPos, vMove, 1u);
                        nSpawnAdd2Func();
                    }

                    for(coreUintW j = 0u; j < 2u; ++j)
                    {
                        const coreVector2 vDir = coreVector2(j ? -1.0f : 1.0f, 2.0f).Normalized();

                        const coreVector2 vPos  = pEnemy->GetPosition().xy() / FOREGROUND_AREA + vDir * 0.1f;
                        const coreVector2 vMove = vDir;

                        nSpawnMeteorFunc(j, vPos, vMove, 2u);
                    }

                    if(!iImpact) STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                }
                else if(bMedium)
                {
                    for(coreUintW j = 0u; j < 4u; ++j)
                    {
                        const coreVector2 vDir = MapStepRotated90X(avMeteorMove[i].Normalized(), j);

                        const coreVector2 vPos  = pEnemy->GetPosition().xy() / FOREGROUND_AREA + vDir * 0.1f;
                        const coreVector2 vMove = vDir;

                        nSpawnMeteorFunc(j, vPos, vMove, 1u);

                        if(iCreateAdd)
                        {
                            iCreateAdd -= 1u;
                            nSpawnAdd2Func();
                        }
                    }
                }
            }

            if(bCrumble)
            {
                const coreInt32 iCurTick = pEnemy->GetCurHealth() / 10;
                const coreInt32 iPreTick = pEnemy->GetPreHealth() / 10;

                for(coreUintW j = iCurTick, je = iPreTick; j < je; ++j)
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = (coreVector2::Direction(I_TO_F(j) * GA) - avMeteorMove[i].Normalized()).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cDebrisBullet>(5, 0.1f, pEnemy, vPos, vDir)->ChangeSize(1.0f)->AddStatus(BULLET_STATUS_GHOST);
                }
            }

            constexpr coreFloat fLimit = FOREGROUND_AREA.y * -1.0f;

            if(bBig && (pEnemy->GetPosition().y <= fLimit))
            {
                if(!iImpact)
                {
                    iImpact = 1u;
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                }

                pEnemy->SetPosition(coreVector3(0.0f, fLimit, 0.0f));
            }
            else
            {
                pEnemy->DefaultRotate(afMeteorRotation[i] * (bMedium ? -1.0f : 1.0f));
                afMeteorRotation[i] += 1.1f * TIME;

                if(avMeteorMove[i])
                {
                    coreVector2 vCurPos  = pEnemy->GetPosition().xy();
                    coreVector2 vCurMove = avMeteorMove[i];

                    vCurPos += vCurMove * TIME;

                    if(!bBig)
                    {
                        const coreVector2 vArea = FOREGROUND_AREA * 1.2f - pEnemy->GetCollisionRadius();

                             if((vCurPos.x < -vArea.x) && (vCurMove.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + vArea.x); vCurMove.x =  ABS(vCurMove.x);}
                        else if((vCurPos.x >  vArea.x) && (vCurMove.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - vArea.x); vCurMove.x = -ABS(vCurMove.x);}
                             if((vCurPos.y < -vArea.y) && (vCurMove.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + vArea.y); vCurMove.y =  ABS(vCurMove.y);}
                        else if((vCurPos.y >  vArea.y) && (vCurMove.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - vArea.y); vCurMove.y = -ABS(vCurMove.y);}
                    }

                    pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
                    avMeteorMove[i] = vCurMove;
                }
            }

            for(coreUintW j = 0u; j < iLeftover; ++j)
            {
                constexpr coreUintW iTargetSub = 5u;

                if(i == aiBeginIndex[j])
                {
                    if(pEnemy->ReachedDeath())
                    {
                        aiBeginIndex[j] = UINT32_MAX;
                    }
                    else if(m_iStageSub == iTargetSub)
                    {
                        STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                        aiBeginIndex[j] = UINT32_MAX;
                    }
                    else
                    {
                        g_pGame->GetCombatText()->AttachMarker(j, coreData::ToChars(iTargetSub - m_iStageSub), pEnemy->GetPosition(), COLOR_MENU_INSIDE);
                    }
                    break;
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUintW iIndex  = aiAddIndex[i];
            const cEnemy*   pParent = pSquad1->GetEnemy(iIndex);

            if(pParent->ReachedDeath() || !pEnemy->HasStatus(ENEMY_STATUS_GHOST))
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);

                    avAddMove    [i] = -pEnemy->GetDirection().xy() * 25.0f;
                    afAddRotation[i] = pParent->GetDirection().xy().Angle();
                    afAddOffset  [i] = fLifeTimeBase;
                }

                if(avAddMove[i])
                {
                    coreVector2 vCurPos  = pEnemy->GetPosition().xy();
                    coreVector2 vCurMove = avAddMove[i];

                    vCurPos += vCurMove * TIME;

                         if((vCurPos.x < -FOREGROUND_AREA.x * 1.0f) && (vCurMove.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.0f); vCurMove.x =  ABS(vCurMove.x);}
                    else if((vCurPos.x >  FOREGROUND_AREA.x * 1.0f) && (vCurMove.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.0f); vCurMove.x = -ABS(vCurMove.x);}
                         if((vCurPos.y < -FOREGROUND_AREA.y * 1.0f) && (vCurMove.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.0f); vCurMove.y =  ABS(vCurMove.y);}
                    else if((vCurPos.y >  FOREGROUND_AREA.y * 1.0f) && (vCurMove.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.0f); vCurMove.y = -ABS(vCurMove.y);}

                    pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
                    avAddMove[i] = vCurMove;
                }

                afAddRotation[i] = FMOD(afAddRotation[i] - 2.4f * TIME, -2.0f*PI);

                pEnemy->DefaultRotate(afAddRotation[i]);

                if(STAGE_TICK_LIFETIME_BASE(26.0f, 26.0f * afAddOffset[i]) && ((s_iTick % 4u) < 2u))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(2.0f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_BLUE);
                }
            }
            else
            {
                const coreVector3 vOffset = pParent->GetRotation().QuatApply(coreVector3(0.0f,-3.5f,0.0f) * fBaseScale);

                pEnemy->SetPosition   (pParent->GetPosition   () + vOffset);
                pEnemy->SetDirection  (pParent->GetDirection  ());
                pEnemy->SetOrientation(pParent->GetOrientation());
            }
        });

        STAGE_FOREACH_ENEMY(pSquad3, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUintW iIndex  = aiAddIndex[i + iNumAdds2];
            const cEnemy*   pParent = pSquad1->GetEnemy(iIndex);

            const coreVector3 vOffset = pParent->GetRotation().QuatApply(coreVector3(0.0f,-3.5f,0.0f) * fBaseScale);

            pEnemy->SetPosition   (pParent->GetPosition   () + vOffset);
            pEnemy->SetDirection  (pParent->GetDirection  ());
            pEnemy->SetOrientation(pParent->GetOrientation());

            d_cast<cArrowEnemy*>(pEnemy)->SetAngle(0.0f);

            if(pParent->ReachedDeath())
            {
                pEnemy->Kill(false);

                const coreVector2 vPos = pParent->GetPosition().xy();

                for(coreUintW j = 10u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 18.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos,  vDir)->ChangeSize(1.6f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, -vDir)->ChangeSize(1.6f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_CYAN);
            }
        });

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const cEnemy* pParent = pSquad1->GetEnemy(iHelperIndex);

            const coreVector3 vOffset = pParent->GetRotation().QuatApply(coreVector3(0.0f,-3.5f,0.0f) * fBaseScale);

            pHelper->SetPosition   (pParent->GetPosition   () + vOffset);
            pHelper->SetAngle      (pParent->GetDirection  ().xy().Angle());
            pHelper->SetOrientation(pParent->GetOrientation());

            if(pParent->ReachedDeath()) pHelper->Kill(true);
        }

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cDebrisBullet>([&](cDebrisBullet* OUTPUT pBullet)
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreVector2 vDiff = pBullet->GetPosition().xy() - pPlayer->GetPosition().xy();
                if(vDiff.LengthSq() < POW2(5.0f))
                {
                    pBullet->Deactivate(true);

                    if(++iDebrisCount >= 10u) STAGE_BADGE(1u, BADGE_NORMAL, pBullet->GetPosition())
                    else g_pGame->GetCombatText()->DrawCountdown(iDebrisCount, 10u, pBullet->GetPosition());
                }
            });
        });

        STAGE_WAVE(4u, "4-5", {50.0f, 75.0f, 100.0f, 125.0f})   // DREIUNDZWANZIG
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->KillHelpers();

        g_pGame->GetBulletManagerEnemy()->ResetOrder();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // teleport everything
    // every representation where the teleporter can be avoided will not work, as the player will always avoid it due to unpredictability, so teleporter need to span across screen
    // dangers of teleportation (especially with changing directions) are hard to assess, so players will avoid using it with the ship
    // when the player has to use the teleporter, the situation needs to be easy and predictable (only one location with enemies)
    // it is important that the player has to use the feature in some way
    // [deprecated] special care needs to be taken for split-screen coop, players cannot change sides
    // tests were done with rotation (unpredictable), movement (unnecessary), 90 degree difference (unpredictable), infinity (unnecessary)
    // moving both portals into their direction or against it, can cause space-folding or -expansion which is physically impossible (e.g. folding would catch objects in the portal and crush them)
    // 1: show player the mechanic
    // 2-5: force the player to use teleporter
    // 6: enemies need to move the player into a safe location
    // 7: groups are not in center, to make movement a bit easier (attacking enemies will not attack player)
    // 8-11: 1-2-2-1 pattern
    // (TODO 1: portale werden am ende zu gegnern, ZeroRanger)
    // TODO 1: badge: in der mitte am anfang
    // TODO 1: on vertical, enemy from below may ram into player
    // TODO 1: if player starts in the center, helper switch places (permanent) and teleport players outside
    // TODO 1: add quad and sphere object in front of teleporters to show direction, oder pfeile, oder links und rechts (wegen kreuz)
    // TODO 1: man soll bei 1 und 2 nicht seitlich vorbeischießen können, aber trotzdem visuellen rand haben
    // TODO 1: bei kreuz, geschosse in der mitte werden zerstört
    // TODO 1: vielleicht teleporter-transition sofort nach farb-effekt, mit LERPB
    // TODO 1: invincible enemy flies down the left site and shoots infinity bullets to the right
    // TODO 1: !!!! am teleportations-ausgang werden alle angriffe zerstört und spieler ist kurzzeitig unverwundbar
    // TODO 1: !!!! vielleicht teleportation mit rechenfehler (InvertedX wieder entfernen)
    // TODO 1: !!!! gegner sollten sich nicht bewegen, sie spawnen aus portalen (unsichtbar, bis erswter teleportation)
    // TODO 1: !!!! wenn portale als hindernisse wahrgenommen werden, sollten sie auch als solche funktionieren
    // TODO 1: links und rechts teleporter, meteoriten kommen raus in unendlichkeit, sind manchmal so arranged (seitlich und von oben-schräg), dass man durch teleporter durch muss
    // TODO 1: MAIN: task-check, helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 126u)                     
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(30, 0u, COLOR_SHIP_YELLOW);

                if(i < 14u)
                {
                }
                else if(i < 20u)
                {
                    pEnemy->SetPosition (coreVector3(-0.9f, 1.3f + I_TO_F(i - 14u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                    
                    if(i % 2u) pEnemy->InvertX();
                    
                    //pEnemy->Rotate90();
                }
                else if(i < 26u)
                {
                    pEnemy->SetPosition (coreVector3(-0.9f, 1.3f + I_TO_F(i - 20u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

                    pEnemy->InvertX();
                    
                    if(i % 2u) pEnemy->InvertX();
                    
                    //pEnemy->Rotate90();
                }
                else if(i < 32u)
                {
                    pEnemy->SetPosition (coreVector3(((i - 26u) % 2u) ? -0.7f : -0.5f, 1.3f + I_TO_F(i - 26u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

                    pEnemy->InvertY();
                    
                    if(i % 2u) pEnemy->InvertX();
                }
                else if(i < 38u)
                {
                    pEnemy->SetPosition (coreVector3(((i - 32u) % 2u) ? -0.7f : -0.5f, 1.3f + I_TO_F(i - 32u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

                    pEnemy->InvertY()->InvertX();
                    
                    if(i % 2u) pEnemy->InvertX();
                }
                else if(i < 46u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F(i - 38u) - 3.5f) * 0.3f, 1.3f + I_TO_F(i - 38u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                    
                    
                    if(i % 2u) pEnemy->InvertY();
                }
                else if(i < 54u)
                {
                    pEnemy->SetPosition (coreVector3(0.9f, 1.3f + I_TO_F(i - 46u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                    
                    pEnemy->Rotate90();
                    if(i % 2u) pEnemy->Rotate180();
                }
                else if(i < 60u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F((i - 54u) % 3u) - 1.0f) * 0.9f, 1.3f + I_TO_F((i - 54u) / 3u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                }
                /*
                else if(i < 28u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F(i - 20u) - 3.5f) * 0.2f, -1.3f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetPosition (coreVector3((i < 24u) ? -0.9f : 0.9f, -1.3f + I_TO_F(i - 20u) * -0.2f, 0.0f) * FOREGROUND_AREA3 * coreVector3(1.0f, (i < 24u) ? -1.0f : 1.0f, 1.0f));
                    pEnemy->SetDirection(coreVector3(0.0f,1.0f,0.0f)* coreVector3(1.0f, (i < 24u) ? -1.0f : 1.0f, 1.0f));
                }
                else if(i < 34u)
                {
                    pEnemy->SetPosition (coreVector3(1.3f, (I_TO_F(i - 28u) - 2.5f) * 0.3f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(-1.0f,0.0f,0.0f));

                    pEnemy->Rotate90();
                }
                else if(i < 40u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F(i - 34u) - 2.5f) * 0.3f, 1.3f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

                    pEnemy->Rotate90();
                }
                else if(i < 48u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F(i - 40u) - 3.5f) * 0.2f * 0.0f - 0.4f, 1.3f + I_TO_F(i - 40u) * 0.2f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));

                    pEnemy->Rotate90();
                }
                else if(i < 56u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F(i - 56u) - 3.5f) * 0.2f, -1.3f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetPosition (coreVector3((i < 52u) ? -0.9f : 0.9f, -1.3f + I_TO_F(i - 56u) * -0.2f, 0.0f) * FOREGROUND_AREA3 * coreVector3(1.0f, (i < 52u) ? -1.0f : 1.0f, 1.0f));
                    pEnemy->SetDirection(coreVector3(0.0f,1.0f,0.0f)* coreVector3(1.0f, (i < 52u) ? -1.0f : 1.0f, 1.0f));

                    pEnemy->Rotate90();
                }
                 */
            });
        });

        STAGE_GET_START(4u + RUTILUS_TELEPORTER * 2u)
            STAGE_GET_UINT      (iTransitionState)
            STAGE_GET_FLOAT     (fTransitionTime)
            STAGE_GET_FLOAT     (fMoveSpeed)
            STAGE_GET_FLOAT     (fMoveTime)
            STAGE_GET_VEC2_ARRAY(avOldPosition, RUTILUS_TELEPORTER)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  8u,  9u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 10u, 13u)
            
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 14u, 19u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 20u, 25u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 26u, 31u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 32u, 37u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 38u, 45u)
            
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 46u, 53u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 54u, 59u)
            
            //else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 28u, 33u)
            //else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 34u, 39u)
            //else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            //else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 40u, 47u)
            //else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 48u, 55u)

            //else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            //else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1,  6u, 11u)
            //else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 12u, 19u)
            //else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 20u, 27u)
            
            //else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 28u, 33u)
            //else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 34u, 45u)
            //else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            //else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            
            //     if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            //else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            //else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  8u,  9u)
            //else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 10u, 11u)
            //else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 12u, 13u)
            //else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 14u, 15u)
            //else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 16u, 21u)
            //else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 22u, 22u)
            //else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 23u, 23u)
            //else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 24u, 24u)
            //else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB(18u)) STAGE_DELAY_START_CLEAR

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;

            for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
                avOldPosition[i] = m_aTeleporter[i].GetPosition().xy() / FOREGROUND_AREA;
        }

        cHelper* pHelperOrange = g_pGame->GetHelper(ELEMENT_ORANGE);
        cHelper* pHelperBlue   = g_pGame->GetHelper(ELEMENT_BLUE);

        if(STAGE_BEGINNING)
        {
            pHelperOrange->Resurrect(false);
            pHelperBlue  ->Resurrect(false);
        }

        fTransitionTime += 1.0f * TIME;

        fMoveSpeed = MIN(fMoveSpeed + 1.0f * TIME, 1.0f);
        fMoveTime  = fMoveTime + fMoveSpeed * TIME;

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                const coreFloat   fTime = MIN(fTransitionTime, 1.2f);
                const coreVector2 vPos  = coreVector2((SIN(fTime / 1.2f * 0.5f*PI) * 2.0f - 1.0f) * (1.2f - fTime) - 0.4f*0.0f, LERP(/*-0.55f*/-0.3f, 0.4f, SIN(fTime / 1.2f * 1.0f*PI))) * FOREGROUND_AREA;

                m_aTeleporter[0].SetPosition(coreVector3( vPos,    0.0f));
                m_aTeleporter[1].SetPosition(coreVector3(-vPos,    0.0f));
                m_aTeleporter[0].SetSize    (coreVector3(0.0f,1.0f,1.0f));
                m_aTeleporter[1].SetSize    (coreVector3(0.0f,1.0f,1.0f));

                if(fTransitionTime >= 1.2f)
                {
                    iTransitionState += 1u;

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) this->EnableTeleporter(i);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) g_pSpecialEffects->CreateSplashColor(m_aTeleporter[i].GetPosition(), 50.0f, 10u, RUTILUS_TELEPORTER_COLOR(i));
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) g_pDistortion->CreateWave(m_aTeleporter[i].GetPosition(), DISTORTION_WAVE_SMALL);
                    this->SetTeleporterActive(1u);
                }
            }
            else if(iTransitionState == 1u)
            {
                const coreFloat fTime = MIN(fTransitionTime - 1.2f, 1.0f);
                const coreFloat fSize = LERPB(0.0f, /*0.67f*/1.1f, fTime) * FOREGROUND_AREA.x;

                m_aTeleporter[0].SetSize(coreVector3(fSize, 1.0f, 1.0f));
                m_aTeleporter[1].SetSize(coreVector3(fSize, 1.0f, 1.0f));
                
                
                m_aTeleporter[0].SetPosition (coreVector3(1.05f,0.0f,0.0f) * FOREGROUND_AREA3);
                m_aTeleporter[1].SetPosition (coreVector3(-1.05f,0.0f,0.0f) * FOREGROUND_AREA3);
                m_aTeleporter[0].SetDirection(coreVector3(1.0f,0.0f,0.0f));
                m_aTeleporter[1].SetDirection(coreVector3(1.0f,0.0f,0.0f));
                
                
                //m_aTeleporter[0].SetPosition (coreVector3(0.0f,-0.3f,0.0f) * FOREGROUND_AREA3);
                //m_aTeleporter[1].SetPosition (coreVector3(0.0f,0.3f,0.0f) * FOREGROUND_AREA3);
                //m_aTeleporter[0].SetDirection(coreVector3(0.0f,1.0f,0.0f));
                //m_aTeleporter[1].SetDirection(coreVector3(0.0f,1.0f,0.0f));

                //m_aTeleporter[0].SetPosition (coreVector3(0.0f,0.0f,0.0f) * FOREGROUND_AREA3);
                //m_aTeleporter[1].SetPosition (coreVector3(0.0f,1.05f,0.0f) * FOREGROUND_AREA3);
                //m_aTeleporter[0].SetDirection(coreVector3(0.0f,1.0f,0.0f));
                //m_aTeleporter[1].SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                
                
                //m_aTeleporter[0].SetPosition (coreVector3(1.05f*0.5f,0.0f,0.0f) * FOREGROUND_AREA3);
                //m_aTeleporter[1].SetPosition (coreVector3(-1.05f*0.5f,0.0f,0.0f) * FOREGROUND_AREA3);
                //m_aTeleporter[0].SetDirection(coreVector3(0.0f,1.0f,0.0f));
                //m_aTeleporter[1].SetDirection(coreVector3(0.0f,1.0f,0.0f));
                //m_aTeleporter[0].SetSize(coreVector3(fSize*0.5f, 1.0f, 1.0f));
                //m_aTeleporter[1].SetSize(coreVector3(fSize*0.5f, 1.0f, 1.0f));
                
                //m_aTeleporter[1].SetPosition (coreVector3(0.0f,1.05f,0.0f) * FOREGROUND_AREA3);
                //m_aTeleporter[1].SetDirection(coreVector3(0.0f,-1.0f,0.0f));

                if(fTransitionTime >= 2.2f)
                {
                    iTransitionState += 1u;
                }
            }
        }
        else if(false && (m_iStageSub >= 5u) && (m_iStageSub <= 9u))
        {
            if((m_iStageSub == 5u) && STAGE_BEGINNING2)
            {
                fMoveSpeed = 0.0f;
                fMoveTime  = 0.0f;
            }

            //const coreFloat fHeight = fMoveTime * 0.3f;

            //m_aTeleporter[0].SetPosition(coreVector3(-0.4f, FmodRange(fHeight - 0.55f, -1.1f, 1.1f), 0.0f) * FOREGROUND_AREA3);
            //m_aTeleporter[1].SetPosition(coreVector3( 0.4f, FmodRange(fHeight + 0.55f, -1.1f, 1.1f), 0.0f) * FOREGROUND_AREA3);

            const coreFloat fHeight = (TriangleWave(fMoveTime * 0.15f + 0.25f) * 2.0f - 1.0f) * /*0.55f*/0.8f;

            m_aTeleporter[0].SetPosition(coreVector3(/*-0.4f*/0.0f, fHeight - /*0.55f*/0.3f, 0.0f) * FOREGROUND_AREA3);
            m_aTeleporter[1].SetPosition(coreVector3(/* 0.4f*/0.0f, fHeight + /*0.55f*/0.3f, 0.0f) * FOREGROUND_AREA3);
        }
        else if(m_iStageSub == 10u)
        {
            if(iTransitionState == 0u)
            {
                if(fTransitionTime >= 0.5f)
                {
                    iTransitionState += 1u;

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__TeleporterEffect(i);
                    this->SetTeleporterActive(0u);
                }
            }
            else if(iTransitionState == 1u)
            {
                const coreFloat   fTime = LERPS(0.0f, 1.0f, CLAMP((fTransitionTime - 1.0f) * 0.5f, 0.0f, 1.0f));
                //const coreVector2 vPos  = LERP(coreVector2(0.0f,0.3f), coreVector2(0.7f,0.0f), fTime) * FOREGROUND_AREA;
                const coreVector2 vDir  = coreVector2::Direction(fTime * -1.5f*PI);

                m_aTeleporter[0].SetPosition (coreVector3(LERP(avOldPosition[0], coreVector2( /*0.55f*/0.7f,0.0f), fTime) * FOREGROUND_AREA,             0.0f));
                m_aTeleporter[1].SetPosition (coreVector3(LERP(avOldPosition[1], coreVector2(-/*0.55f*/0.7f,0.0f), fTime) * FOREGROUND_AREA,             0.0f));
                m_aTeleporter[0].SetDirection(coreVector3(-vDir.InvertedY(), 0.0f));
                m_aTeleporter[1].SetDirection(coreVector3( vDir,             0.0f));

                const coreFloat fSize = LERPB(0.67f, 1.05f, fTime) * FOREGROUND_AREA.x;

                m_aTeleporter[0].SetSize(coreVector3(fSize, 1.0f, 1.0f));
                m_aTeleporter[1].SetSize(coreVector3(fSize, 1.0f, 1.0f));

                if(fTransitionTime >= 3.0f)
                {
                    iTransitionState += 1u;
                }
            }
            else if(iTransitionState == 2u)
            {
                if(fTransitionTime >= 3.5f)
                {
                    iTransitionState += 1u;

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__TeleporterEffect(i);
                    this->SetTeleporterActive(1u);
                }
            }
        }
        else if((m_iStageSub >= 12u) && (m_iStageSub <= 13u))
        {
            if((m_iStageSub == 12u) && STAGE_BEGINNING2)
            {
                fMoveSpeed = 0.0f;
                fMoveTime  = 0.0f;
            }

            const coreFloat fHeight = (TriangleWave(fMoveTime * 0.1f + ((0.7f + 1.1f) / 1.1f) * 0.25f) * 2.0f - 1.0f) * 1.1f * FOREGROUND_AREA.x;

            m_aTeleporter[0].SetPosition(coreVector3( fHeight, 0.0f, 0.0f));
            m_aTeleporter[1].SetPosition(coreVector3(-fHeight, 0.0f, 0.0f));
        }
        else if(m_iStageSub == 14u)
        {
            if(iTransitionState == 0u)
            {
                if(fTransitionTime >= 0.5f)
                {
                    iTransitionState += 1u;

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__TeleporterEffect(i);
                    this->SetTeleporterActive(0u);
                }
            }
            else if(iTransitionState == 1u)
            {
                const coreFloat   fTime = LERPS(0.0f, 1.0f, CLAMP((fTransitionTime - 1.0f) * 0.5f, 0.0f, 1.0f));
                //const coreVector2 vPos  = LERP(coreVector2(0.0f,0.3f), coreVector2(0.7f,0.0f), fTime) * FOREGROUND_AREA;
                //const coreVector2 vDir  = coreVector2::Direction(fTime * 0.5f*PI);

                m_aTeleporter[0].SetPosition (coreVector3(LERP(avOldPosition[0], coreVector2(0.0f,0.0f), fTime) * FOREGROUND_AREA,             0.0f));
                m_aTeleporter[1].SetPosition (coreVector3(LERP(avOldPosition[1], coreVector2(0.0f,0.0f), fTime) * FOREGROUND_AREA,             0.0f));
                m_aTeleporter[0].SetDirection(coreVector3(coreVector2::Direction(LERP(-0.5f*PI, 0.75f*PI, fTime)), 0.0f));
                m_aTeleporter[1].SetDirection(coreVector3(coreVector2::Direction(LERP(0.5f*PI, -0.75f*PI, fTime)),             0.0f));

                if(fTransitionTime >= 3.0f)
                {
                    iTransitionState += 1u;
                }
            }
            else if(iTransitionState == 2u)
            {
                if(fTransitionTime >= 3.5f)
                {
                    iTransitionState += 1u;

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__TeleporterEffect(i);
                    this->SetTeleporterActive(1u);
                }
            }
        }
        else if((m_iStageSub >= 16u) && (m_iStageSub <= 17u))
        {
            if((m_iStageSub == 16u) && STAGE_BEGINNING2)
            {
                fMoveSpeed = 0.0f;
                fMoveTime  = 0.0f;
            }

            const coreVector2 vDir = coreVector2::Direction(fMoveTime * -0.5f + 1.25f*PI);

            m_aTeleporter[0].SetDirection(coreVector3(vDir.Rotated90(), 0.0f));
            m_aTeleporter[1].SetDirection(coreVector3(vDir, 0.0f));
        }
        else if(m_iStageSub == 18u)
        {
            const coreFloat fPos = (LERPB(0.0f, 0.7f, 1.0f - fTransitionTime) + SIN(fTransitionTime * 1.0f*PI) * 0.3f) * FOREGROUND_AREA.y;

            m_aTeleporter[0].SetPosition(coreVector3(-fPos, 0.0f, 0.0f));
            m_aTeleporter[1].SetPosition(coreVector3( fPos, 0.0f, 0.0f));

            if(fTransitionTime >= 1.0f)
            {
                for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
                    this->DisableTeleporter(i, true);

                pHelperOrange->Kill(false);
                pHelperBlue  ->Kill(false);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                STAGE_DELAY_END
            }
        }

        pHelperOrange->SetPosition(m_aTeleporter[0].GetPosition());
        pHelperBlue  ->SetPosition(m_aTeleporter[1].GetPosition());

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 6u) ? 2.0f : 0.0f)

            if(i >= 14u && i < 100u)
            {
                if(STAGE_LIFETIME_AFTER(0.0f)) pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 35.0f);

                coreVector2       vCurPos = pEnemy->GetPosition ().xy();
                const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.3f) && (vCurDir.x < 0.0f)) vCurPos.x += FOREGROUND_AREA.x * 2.6f;
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.3f) && (vCurDir.x > 0.0f)) vCurPos.x -= FOREGROUND_AREA.x * 2.6f;
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.3f) && (vCurDir.y < 0.0f)) vCurPos.y += FOREGROUND_AREA.y * 2.6f;
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.3f) && (vCurDir.y > 0.0f)) vCurPos.y -= FOREGROUND_AREA.y * 2.6f;

                pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            }
            else
            {
                // shadowing
                // STAGE_LIFETIME(pEnemy, 1.0f, (i < 6u) ? 2.0f : ((i >= 14u && i < 16u) ? 3.5f : 0.0f))

                if(i < 14u)
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = (i < 6u) ? coreVector2((I_TO_F(i) - 2.5f) * 0.2f, ABS(I_TO_F(i) - 2.5f) * 0.05f - 0.025f) : coreVector2(I_TO_F(i % 2u) * 0.2f - 0.82f, 0.2f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                         if(i <  6u) {}
                    else if(i <  8u) pEnemy->Rotate270();
                    else if(i < 10u) pEnemy->Rotate270()->InvertY();
                    else if(i < 12u) pEnemy->Rotate90 ()->InvertY();
                    else if(i < 14u) pEnemy->Rotate90 ();
                }
                else if(i < 26u)
                {
                    const coreVector2 vFactor = coreVector2(1.0f, (i == 22u || i == 25u) ? -1.0f : 1.0f);
                    const coreVector2 vOffset = (i < 16u) ? coreVector2(0.0f,0.2f) : ((i < 22u) ? coreVector2((I_TO_F((i - 16u) % 3u) - 1.0f) * 0.2f + 0.3f, ABS(I_TO_F((i - 16u) % 3u) - 1.0f) * 0.05f + 0.2f) : coreVector2((i % 2u) ? -0.3f : 0.3f, 0.2f));

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                         if(i < 15u) pEnemy->Rotate90 ();
                    else if(i < 16u) pEnemy->Rotate270();
                    else if(i < 19u) pEnemy->Rotate180();

                    pEnemy->Rotate90();
                }
            }

            //if(STAGE_TICK_LIFETIME(1.2f, 0.0f))
            //if(STAGE_TICK_TIME(1.2f, 0.0f))
            if(STAGE_TICK_TIME(10.0f, 0.0f))   // TODO 1: STAGE_TICK_TIME2 instead ?
            //if(false && STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreUintW   iCount = 1u;//(i < 6u || (i >= 14 && i < 22u)) ? 1u : 5u;
                const coreVector2 vPos   = pEnemy->GetPosition().xy();
                const coreFloat   fBase  = pEnemy->AimAtPlayerSide().Angle();

                for(coreUintW j = iCount; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iCount - 1u) * 0.5f) * 4.0f*0.5f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 0.7f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        });

        STAGE_WAVE(5u, "4-?", {60.0f, 80.0f, 100.0f, 120.0f})   // VIERUNDZWANZIG
    });

    // ################################################################
    // reset helper
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            this->DisableTeleporter(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        constexpr coreFloat fBaseScale = 1.5f;

        UNUSED STAGE_ADD_SQUAD(pSquad1, cMeteorEnemy, MESSIER_ENEMIES_SMALL)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 3.0f * fBaseScale);
                pEnemy->Configure(10, 0u, COLOR_SHIP_PURPLE / COLOR_SHIP_PURPLE.Max());
                pEnemy->AddStatus(/*ENEMY_STATUS_INVINCIBLE |*/ ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_SECRET);
            });
        });

        UNUSED STAGE_ADD_SQUAD(pSquad2, cMeteorEnemy, MESSIER_ENEMIES_BIG)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 5.0f * fBaseScale);
                pEnemy->Configure(50 * 100, 0u, COLOR_SHIP_PURPLE / COLOR_SHIP_PURPLE.Max());
                pEnemy->AddStatus(/*ENEMY_STATUS_INVINCIBLE |*/ ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_TOP | ENEMY_STATUS_SECRET);
            });
        });

        STAGE_BOSS(m_Messier, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // end
    STAGE_MAIN({TAKE_MISSION})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_OUTRO)
    });

    // ################################################################
    // ################################################################
}