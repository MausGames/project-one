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
// setup the Harena mission
void cHarenaMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cDesertBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

        g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
        g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
        g_pEnvironment->SetTargetSpeedNow    (6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u, 2u})
    {
        cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

        pBackground->GetOutdoor()->LerpHeightNow(-0.5f, -30.0f);
        pBackground->SetGroundDensity(0u, 0.0f);
        pBackground->SetGroundDensity(1u, 0.0f);
        pBackground->SetGroundDensity(2u, 0.0f);
        pBackground->SetGroundDensity(3u, 0.0f);
        pBackground->SetGroundDensity(5u, 1.0f);
        pBackground->SetTrail(true);

        STAGE_FINISH_NOW
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
    // ghost appears for few seconds (zelda wizzrobe)
    // - no individual enemies at the beginning, otherwise the player won't see the mechanics if they just shoot them down
    // - on first wave, two groups active at the same time to continue with changes while keeping enemies active for a longer time
    // - on first wave, two different invincible states, hidden and flickering, to give the player perception of change while he has to wait (loading bar)
    // - on first wave, diamond shape to not be able to kill 2 groups at the same time
    // - on snake wave, pattern should not have too long straight lines, as this makes it too easy, and should have no corners, as this creates placement artifacts
    // - stuttering side-movement needs to be only in one direction, otherwise player does not recognize the move pattern
    // - stuttering side-movement needs a safe-space (bottom screen) because being in the middle of them is hard to evade, as it's already hard to hit
    // - player should already look in the right direction at the start of the threshold wave, the previous wave is doing that with the movement-direction of the enemies
    // - on threshold wave, the threshold should be shifted a bit, so that players don't hit with a side attack, and enemy bullets fly at an angle (more beautiful)
    // - in order wave, no 3 in a row should be attackable from one and the same position (2 is ok because that's logical), if possible don't move 2 fields between 2 attacks and only rotate by 90 degrees
    // - memory (invisible enemies that you can hit) didn't work at all, didn't create enough tension, and was too easy (just mow down), but would be good for badge
    // TASK: destroy all broken enemies on their first appearance
    // TASK: let the very last enemy survive, alone
    // TASK EXTRA: kill 4 very quick enemies before they disappear
    // ACHIEVEMENT: follow the head of the snake group for at least 10 seconds
    // TODO 1: hardmode: bad visibility (sand storm) in one of the missions here, maybe has to disappear completely in a sinusoidal manner (or in another way), also bullets due to pressure
    // TODO 1: hardmode: enemies also shoot when getting invisible
    m_aInsanityStage[0] = [this]()
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(17u);
            pPath1->AddNode(coreVector2( 0.0f,  0.0f),  coreVector2( 1.0f, 1.0f).Normalized());
            pPath1->AddNode(coreVector2( 0.35f, 0.5f),  coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.0f,  0.85f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.35f, 0.5f),  coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.0f,  0.0f),  coreVector2( 1.0f,-1.0f).Normalized());
            pPath1->AddNode(coreVector2( 0.35f,-0.5f),  coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.0f, -0.85f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.35f,-0.5f),  coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.0f,  0.0f),  coreVector2( 1.0f, 1.0f).Normalized());
            pPath1->AddNode(coreVector2( 0.5f,  0.35f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.85f, 0.0f),  coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.5f, -0.35f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.0f,  0.0f),  coreVector2(-1.0f, 1.0f).Normalized());
            pPath1->AddNode(coreVector2(-0.5f,  0.35f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.85f, 0.0f),  coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.5f, -0.35f), coreVector2( 1.0f, 0.0f));
            pPath1->AddLoop();
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 158u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        constexpr coreUintW  iRange      = sizeof(coreUint32) * 8u;
        constexpr coreUint32 iSnakeCount = 6u;
        constexpr coreFloat  fQuickDelay = 1.8f;

        STAGE_GET_START(12u + iSnakeCount + GAME_PLAYERS)
            STAGE_GET_UINT       (iStateActive)
            STAGE_GET_UINT       (iStateGhost)
            STAGE_GET_FLOAT      (fChangeTime)
            STAGE_GET_UINT       (iChangeCount)
            STAGE_GET_UINT_ARRAY (aiSnakeIndex, iSnakeCount)
            STAGE_GET_UINT       (iSnakeShift,  iSnakeShift = iSnakeCount)
            STAGE_GET_FLOAT      (fLastValue)
            STAGE_GET_UINT       (iLastDual)
            STAGE_GET_UINT       (iFlipShow)
            STAGE_GET_UINT       (iFlipHide)
            STAGE_GET_UINT       (iFlipCount)
            STAGE_GET_UINT       (iQuickCount)
            STAGE_GET_UINT       (iBlockShot)
            STAGE_GET_FLOAT_ARRAY(afFollowTime, GAME_PLAYERS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
            if(m_iInsanity)
            {
                     if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 124u, 125u)
                else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 126u, 141u)
                else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 142u, 157u)
                else if(STAGE_SUB(13u)) {}
            }
            else
            {
                     if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,   0u,  15u)
                else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  16u,  31u)
                else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  32u,  47u)
                else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  48u,  55u)
                else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  56u,  59u)   // quick wave
                else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  60u,  75u)
                else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  76u,  81u)
                else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  82u,  93u)
                else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1,  94u, 123u)
            }

            iStateActive = 0u;
            iStateGhost  = 0u;

            fChangeTime  = 0.0f;
            iChangeCount = 0u;
        }

        ASSERT(pSquad1->GetNumEnemiesAlive() < iRange)

        const coreFloat  fChangeTimePrev  = fChangeTime;
        const coreUint32 iChangeCountPrev = iChangeCount;
        fChangeTime += 1.0f * TIME;

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_GREEN);

        if((m_iStageSub == 5u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
            pHelper->SetPosition(coreVector3(0.0f,0.0f,0.0f));
        }
        else if((m_iStageSub == 6u) && STAGE_BEGINNING2)
        {
            pHelper->Kill(false);
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            if(fChangeTime >= fQuickDelay)
            {
                pHelper->SetAlpha(1.0f);
            }
            else if(fChangeTime >= fQuickDelay - 1.0f)
            {
                pHelper->SetAlpha((g_CurConfig.Graphics.iFlash && (FRACT(fChangeTime * 20.0f) < 0.5f)) ? 0.9f : 0.4f);
                pHelper->RemoveStatus(HELPER_STATUS_HIDDEN);
            }
            else
            {
                pHelper->AddStatus(HELPER_STATUS_HIDDEN);
            }
        }

        coreBool bPostpone = false;

        if(m_iStageSub == 1u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b0000'0000'0000'1111u, 0u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b0000'0000'1111'0000u, 0u % iRange);
            constexpr coreUint32 iGroupC = coreMath::RotateLeft32(0b0000'1111'0000'0000u, 0u % iRange);
            constexpr coreUint32 iGroupD = coreMath::RotateLeft32(0b1111'0000'0000'0000u, 0u % iRange);

            if(FRACT(fChangeTime * 1.05f) < FRACT(fChangeTimePrev * 1.05f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 4u)
                {
                default: UNREACHABLE
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupD; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupA; break;
                case 2u: iStateActive &= ~iGroupC; iStateGhost = iGroupB; break;
                case 3u: iStateActive &= ~iGroupD; iStateGhost = iGroupC; break;
                }
            }
        }
        else if(m_iStageSub == 2u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b11110000'11110000u, 16u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b00001111'00001111u, 16u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: UNREACHABLE
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }
            }
        }
        else if(m_iStageSub == 3u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b11111111'00000000u, 32u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b00000000'11111111u, 32u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: UNREACHABLE
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }
            }
        }
        else if(m_iStageSub == 4u)
        {
            if(FRACT(fChangeTime * 2.5f) < FRACT(fChangeTimePrev * 2.5f))
            {
                iChangeCount += 1u;

                iStateActive = (iChangeCount % 2u) ? 0xFFFFFFFFu : 0x00000000u;
                iStateGhost  = (iChangeCount % 2u) ? 0x00000000u : 0xFFFFFFFFu;
            }
        }
        else if(m_iStageSub == 5u)
        {
            if(fChangeTime >= fQuickDelay + 1.2f)
            {
                pSquad1->ClearEnemies(false);
                bPostpone = true;
            }
            else if(InBetween(fQuickDelay, fChangeTimePrev, fChangeTime))
            {
                iChangeCount += 1u;

                iStateActive = coreMath::RotateLeft32(0b1111u, 56u % iRange);
                iStateGhost  = 0x00000000u;
            }
        }
        else if(m_iStageSub == 6u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b11111111'00000000u, 60u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b00000000'11111111u, 60u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: UNREACHABLE
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }
            }
        }
        else if(m_iStageSub == 7u)
        {
            iChangeCount = 1u - iChangeCount;

            iStateActive = 0x00000000u;
            iStateGhost  = 0xFFFFFFFFu;

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                coreBool bPast = true;

                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    if(pPlayer->GetPosition().x < pEnemy->GetPosition().x + 10.0f) bPast = false;
                });

                if(bPast) ADD_BIT(iStateActive, i % iRange)
            });
        }
        else if(m_iStageSub == 8u)
        {
            iChangeCount = 1u - iChangeCount;

            if(fChangeTime >= (1.0f/0.75f))
            {
                iStateGhost = 0xFFFFFFFFu;
            }

            if(fChangeTime >= (2.0f/0.75f))
            {
                constexpr coreUintW iFirst = 82u;

                switch(pSquad1->GetNumEnemiesAlive())
                {
                default: UNREACHABLE
                case 12u: iStateActive = BIT((iFirst +  3u) % iRange); break;
                case 11u: iStateActive = BIT((iFirst +  7u) % iRange); break;
                case 10u: iStateActive = BIT((iFirst + 10u) % iRange); break;
                case  9u: iStateActive = BIT((iFirst +  6u) % iRange); break;
                case  8u: iStateActive = BIT((iFirst + 11u) % iRange); break;
                case  7u: iStateActive = BIT((iFirst +  1u) % iRange); break;
                case  6u: iStateActive = BIT((iFirst +  5u) % iRange); break;
                case  5u: iStateActive = BIT((iFirst +  8u) % iRange); break;
                case  4u: iStateActive = BIT((iFirst +  4u) % iRange); break;
                case  3u: iStateActive = BIT((iFirst +  2u) % iRange); break;
                case  2u: iStateActive = BIT((iFirst +  9u) % iRange); break;
                case  1u: iStateActive = BIT((iFirst +  0u) % iRange); break;
                case  0u: iStateActive = 0u;                           break;
                }
            }
        }
        else if(m_iStageSub == 9u)
        {
            if(FRACT(fChangeTime * 5.0f) < FRACT(fChangeTimePrev * 5.0f))
            {
                iChangeCount += 1u;

                if(iSnakeShift >= 2u)
                {
                    if(aiSnakeIndex[iSnakeShift - 1u]) REMOVE_BIT(iStateActive, aiSnakeIndex[iSnakeShift - 1u] % iRange)
                    if(aiSnakeIndex[iSnakeShift - 1u]) REMOVE_BIT(iStateGhost,  aiSnakeIndex[iSnakeShift - 1u] % iRange)
                    if(aiSnakeIndex[iSnakeShift - 2u]) ADD_BIT   (iStateActive, aiSnakeIndex[iSnakeShift - 2u] % iRange)

                    const coreUint32 iNewShift = MIN(pSquad1->GetNumEnemiesAlive(), iSnakeCount);
                    if(iSnakeShift != iNewShift)
                    {
                        iSnakeShift = iNewShift;
                    }
                    else
                    {
                        std::memmove(aiSnakeIndex + 1u, aiSnakeIndex, sizeof(coreUint32) * (iSnakeShift - 1u));
                        aiSnakeIndex[0] = 0u;

                        for(coreUintW i = 94u; i < 124u; ++i)
                        {
                            if(!pSquad1->GetEnemy(i)->HasStatus(ENEMY_STATUS_DEAD) && !HAS_BIT(iStateGhost, i % iRange))
                            {
                                ADD_BIT(iStateGhost, i % iRange)
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

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreFloat   fOffset = (0.25f/5.0f) + (1.0f/5.0f) * I_TO_F(iSnakeCount - iSnakeShift);
                const coreVector2 vPos    = pPath1->CalcPosition(FMOD(MAX0(fChangeTime - fOffset), pPath1->GetTotalDistance()));
                const coreVector2 vDiff   = pPlayer->GetPosition().xy() - vPos * FOREGROUND_AREA;

                if(vDiff.LengthSq() < POW2(10.0f))
                {
                    afFollowTime[i] += 1.0f * TIME;
                    if(afFollowTime[i] >= 10.0f) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pPlayer->GetPosition())
                }
                else
                {
                    afFollowTime[i] = 0.0f;
                }
            });
        }
        else if(m_iStageSub == 10u)
        {
            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;
                iStateGhost   = 0xFFFFFFFFu;
            }
        }
        else if(m_iStageSub == 11u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b10101010'10101010u, 126u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b01010101'01010101u, 126u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: UNREACHABLE
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }

                if((iChangeCount == 3u) && m_Tiger.ResurrectHelper(ELEMENT_ORANGE, false))
                {
                    ASSERT(pHelper != g_pGame->GetHelper(ELEMENT_ORANGE))
                    g_pGame->GetHelper(ELEMENT_ORANGE)->SetPosition(coreVector3(0.0f,0.5f,0.0f) * FOREGROUND_AREA3);
                }
            }
        }
        else if(m_iStageSub == 12u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b10011001'10011001u, 142u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b01100110'01100110u, 142u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: UNREACHABLE
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }
            }
        }

        const auto nAimFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            coreVector2 vDir;
                 if(i <  48u) vDir = pEnemy->AimAtPlayerDual((i / 8u) % 2u);
            else if(i <  60u) vDir = pEnemy->AimAtPlayerDual(i % 2u);
            else if(i <  76u) vDir = pEnemy->AimAtPlayerSide();
            else if(i <  94u) vDir = pEnemy->AimAtPlayerDual(((i - 76u) / 3u) % 2u);
            else if(i < 124u) vDir = pEnemy->AimAtPlayerDual((pSquad1->GetNumEnemiesAlive() <= 15u) ? 1u : 0u);
            else if(i < 126u) vDir = pEnemy->AimAtPlayerSide();
            else              vDir = pEnemy->AimAtPlayerDual(((i - 126u) / 8u) % 2u);

            pEnemy->SetDirection(coreVector3(vDir.Normalized(), 0.0f));
        };

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const coreVector3 vSaveDir = pEnemy->GetDirection();   // movement patterns should not affect direction and aim

            if(i < 60u)
            {
                // no path movement
            }
            else if(i < 76u)
            {
                STAGE_LIFETIME(pEnemy, 0.7f, 0.7f * I_TO_F((i - 60u) / 4u))

                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F((i - 60u) % 4u) * 0.2f - 0.9f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->InvertY();
                if(((i - 60u) / 4u) % 2u) pEnemy->Rotate180();
            }
            else if(i < 82u)
            {
                STAGE_LIFETIME(pEnemy, 1.1f, 1.3f * I_TO_F((i - 76u) % 2u))

                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i - 76u) - 2.5f) * 0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }

            if(iChangeCount != iChangeCountPrev)
            {
                if(i < 16u)
                {
                    const coreVector2 vPos = (0.75f * StepRotated90X(i / 4u) + 0.25f * StepRotated90X(i % 4u)) * 0.5f * SQRT2;

                    pEnemy->SetPosition(coreVector3(vPos.Rotated45() * FOREGROUND_AREA, 0.0f));
                }
                else if(i < 32u)
                {
                    const coreVector2 vPos = coreVector2(((i - 16u) % 2u) ? 0.8f : 0.9f, (I_TO_F((i - 16u) % 8u) - 3.5f) * 0.27f);

                    pEnemy->SetPosition(coreVector3(-vPos.Rotated90() * FOREGROUND_AREA, 0.0f));

                         if(i < 24u) {}
                    else if(i < 32u) pEnemy->Rotate180();
                }
                else if(i < 48u)
                {
                    const coreBool bVertical = (i < 40u);
                    const coreBool bGroup    = (iChangeCount % 2u);

                    if(bVertical == bGroup)
                    {
                        const coreFloat   fSide   = -0.95f + 1.9f/7.0f * I_TO_F((i - 32u) % 8u);
                        const coreVector2 vTarget = pEnemy->NearestPlayerDual(((i - 32u) / 8u) % 2u)->GetPosition().xy();

                        if(bVertical) pEnemy->SetPosition(coreVector3(vTarget.x, fSide * FOREGROUND_AREA.y, 0.0f));
                                 else pEnemy->SetPosition(coreVector3(fSide * FOREGROUND_AREA.x, vTarget.y, 0.0f));
                    }
                }
                else if(i < 56u)
                {
                    STAGE_LIFETIME(pEnemy, 0.8f, 0.325f * I_TO_F(i - 48u))

                    STAGE_REPEAT(pPath2->GetTotalDistance())

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F((((i - 48u) % 4u) * 2u + 1u) % 5u) - 1.5f) * 0.3f - 0.4f, 0.0f);

                    pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                    pEnemy->Rotate90()->InvertX();
                }
                else if(i < 60u)
                {
                    const coreVector2 vPos = coreVector2(0.0f, 0.9f);

                    pEnemy->SetPosition(coreVector3(MapStepRotated90(vPos * FOREGROUND_AREA, i - 56u), 0.0f));
                }
                else if(i < 82u)
                {
                    // no point movement
                }
                else if(i < 94u)
                {
                    const coreVector2 vPos = coreVector2((I_TO_F((i - 82u) % 3u) - 1.0f) * 0.3f, 0.8f);

                    pEnemy->SetPosition(coreVector3(MapStepRotated90(vPos * FOREGROUND_AREA, (i - 82u) / 3u), 0.0f));
                }
                else if(i < 124u)
                {
                    if(aiSnakeIndex[0] == i)
                    {
                        const coreFloat   fOffset = (1.0f/5.0f) * I_TO_F(iSnakeCount - iSnakeShift);
                        const coreVector2 vPos    = pPath1->CalcPosition(FMOD(fChangeTime - fOffset, pPath1->GetTotalDistance()));

                        pEnemy->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                    }
                }
                else if(i < 126u)
                {
                    const coreVector2 vPos = coreVector2(((i - 124u) % 2u) ? -0.6f : 0.6f, 0.6f);

                    pEnemy->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }
                else if(i < 142u)
                {
                    const coreVector2 vPos = coreVector2(((i - 126u) % 2u) ? 0.8f : 0.9f, (I_TO_F((i - 126u) % 8u) - 3.5f) * 0.27f);

                    pEnemy->SetPosition(coreVector3(-vPos * FOREGROUND_AREA, 0.0f));

                         if(i < 134u) {}
                    else if(i < 142u) pEnemy->Rotate180();
                }
                else if(i < 158u)
                {
                    const coreUintW X = ((i - 142u))      % 2u;
                    const coreUintW Y = ((i - 142u) / 2u) % 2u;

                    const coreVector2 vPos = coreVector2((I_TO_F(X) + 2.5f) * 0.27f + ((Y == 1u) ? -0.1f : 0.0f), (I_TO_F(Y) + 2.5f) * 0.27f);

                    pEnemy->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));

                         if(i < 146u) {}
                    else if(i < 150u) pEnemy->InvertY();
                    else if(i < 154u) pEnemy->InvertX()->InvertY();
                    else if(i < 158u) pEnemy->InvertX();
                }
            }

            pEnemy->SetDirection(vSaveDir);

            if(iChangeCount != iChangeCountPrev)
            {
                if(HAS_BIT(iStateActive, i % iRange))
                {
                    if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                    {
                        pEnemy->SetBaseColor(COLOR_SHIP_GREEN);
                        pEnemy->SetAlpha    (1.0f);
                        pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                        iBlockShot = !iBlockShot;

                        if((!g_pGame->IsEasy() || iBlockShot) && g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()) && !m_iInsanity)
                        {
                            nAimFunc(pEnemy, i);

                            const coreVector2 vPos = pEnemy->GetPosition ().xy();
                            const coreVector2 vDir = pEnemy->GetDirection().xy();
                            const coreVector2 vTan = vDir.Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.3f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.3f);

                            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 3u, COLOR_ENERGY_ORANGE);
                            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                        }
                    }
                }
                else
                {
                    if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                    {
                        pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                        pEnemy->SetAlpha    (0.0f);
                        pEnemy->AddStatus   (ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                    }
                }
            }

            if(!HAS_BIT(iStateActive, i % iRange) && HAS_BIT(iStateGhost, i % iRange))
            {
                nAimFunc(pEnemy, i);

                pEnemy->SetAlpha    ((g_CurConfig.Graphics.iFlash && (FRACT(fChangeTime * 20.0f) < 0.5f)) ? 0.9f : 0.4f);
                pEnemy->RemoveStatus(ENEMY_STATUS_HIDDEN);
            }

            if(g_pGame->IsTask())
            {
                if((m_iStageSub == 9u) && (pSquad1->GetNumEnemiesAlive() == 1u))
                {
                    if(!fLastValue) iLastDual = g_pGame->GetPlayerIndex(pEnemy->NearestPlayerDual(1u));

                    fLastValue += 0.8f * TIME;

                    if(fLastValue >= 1.8f)
                    {
                        pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * (320.0f * (fLastValue - 1.8f + 0.25f) * TIME));
                    }
                    else
                    {
                        coreVector2 vPos = pEnemy->GetPosition().xy();
                        coreVector2 vDir = pEnemy->AimAtPlayerDual(iLastDual).Normalized();

                             if(fLastValue >= 1.5f) vDir *= coreMatrix3::Rotation(LERPB(0.0f*PI, -1.0f*PI, (fLastValue - 1.5f) / 0.3f)).m12();
                        else if(fLastValue >= 1.0f) vPos += vDir * (60.0f * (fLastValue - 1.5f) * TIME);

                        pEnemy->SetPosition (coreVector3(CLAMP(vPos.x, -FOREGROUND_AREA.x, FOREGROUND_AREA.x), CLAMP(vPos.y, -FOREGROUND_AREA.y, FOREGROUND_AREA.y), 0.0f));
                        pEnemy->SetDirection(coreVector3(vDir, 0.0f));
                    }

                    if(fLastValue >= 1.0f)
                    {
                        pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);
                        STAGE_DELAY_START
                    }

                    if(!g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()))
                    {
                        STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())

                        pEnemy->Kill(true);
                        STAGE_DELAY_END
                    }
                }

                constexpr coreUintW aiFlip[] = {14u, 22u, 26u, 41u, 65u, 74u, 79u};

                const coreUintW iFlipIndex = std::find(aiFlip, aiFlip + ARRAY_SIZE(aiFlip), i) - aiFlip;

                if((iFlipIndex < ARRAY_SIZE(aiFlip)) && !HAS_BIT(iFlipHide, iFlipIndex))
                {
                    if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                    {
                        pEnemy->SetOrientation(coreVector3(0.0f,0.0f,-1.0f));
                        pEnemy->AddStatus     (ENEMY_STATUS_CRASH);
                        ADD_BIT(iFlipShow, iFlipIndex)

                        if(pEnemy->ReachedDeath())
                        {
                            if(++iFlipCount == ARRAY_SIZE(aiFlip))
                            {
                                STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawProgress(iFlipCount, ARRAY_SIZE(aiFlip), pEnemy->GetPosition());
                                g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iFlipCount, ARRAY_SIZE(aiFlip)), SOUND_ITEM_02);
                            }
                        }
                    }
                    else if(HAS_BIT(iFlipShow, iFlipIndex))
                    {
                        pEnemy->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
                        pEnemy->RemoveStatus  (ENEMY_STATUS_CRASH);
                        ADD_BIT(iFlipHide, iFlipIndex)
                    }
                }

                if(HAS_BIT(iFlipShow, iFlipIndex) && !HAS_BIT(iFlipHide, iFlipIndex))
                {
                    pEnemy->DefaultRotate(pEnemy->GetLifeTime() * 5.0f);
                }
            }

            if(g_pGame->IsTaskExtra())
            {
                if(i >= 56u && i < 60u)
                {
                    if(pEnemy->ReachedDeath())
                    {
                        if(++iQuickCount == 4u) STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                    }
                }
            }

            this->CrashEnemy(pEnemy);
        });

        if(!m_iInsanity)
        {
            if(STAGE_BEGINNING)
            {
                g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,1.0f).Normalized(), 30.0f);
            }

            if(!bPostpone) STAGE_WAVE(0u, "3-1", {50.0f, 75.0f, 100.0f, 125.0f, 250.0f})   // DREIZEHN
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        m_aInsanityStage[0]();
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cConeBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,1.0f).Normalized());

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // whac-a-mole
    // - smoke can distract from bullets
    // - non-linear movement is harder to track
    // - shooting on "hide" instead of "show" is too overwhelming, identifying both initial bullet-direction and movement at the same time
    // - 8-enemy pattern is quite overwhelming (order: 0, 3, 5, 2, 6, 1, 7, 4 (flawed)) (order: 0, 3, 6, 2, 7, 4, 1, 5 (perfect)) (0, 5, 2, 7, 4, 1, 6, 3 (circular))
    // - during rotation, if move pattern moves against rotation the enemy stays in the air, which looks weird
    // - hiding current enemy movement did not add any value, especially on rotating 8 enemies it is just too overwhelming (maybe for hard mode ? might be too light, also change pattern or order ?)
    // - first enemy flies from below maybe through player, to show there is no harm
    // TASK: kill golden enemy
    // TASK: collect all eggs
    // ACHIEVEMENT: attack every enemy once as the wrong target
    // TODO 1: hardmode: big can change, like in tiger boss, just (iBig << 1u), maybe not on the first 1-2 hits
    // TODO 5: coop: one enemy must be hit by both within the time limit
    // TODO 5: idea: enemies which move close together (into the center)
    // TODO 5: idea: eggs are layed when hitting an enemy, and stay in final phase? (will the last egg be hidden?)
    // TODO 5: badge: ride the wave
    m_aInsanityStage[1] = [this]()
    {
        constexpr coreUintW iNumEnemies = 8u;

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.1f);

                pEnemy->Configure(1000, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(15u + 4u * iNumEnemies)
            STAGE_GET_UINT      (iVisible)
            STAGE_GET_UINT      (iBig)
            STAGE_GET_UINT      (iGold)
            STAGE_GET_UINT      (iHit)
            STAGE_GET_UINT      (iPoints)
            STAGE_GET_UINT      (iIteration)
            STAGE_GET_UINT      (iSingle)
            STAGE_GET_UINT      (iEggState)
            STAGE_GET_UINT      (iRevengeState)
            STAGE_GET_FLOAT     (fRotaSpeed)
            STAGE_GET_FLOAT     (fRotaValue)
            STAGE_GET_FLOAT     (fTime,  fTime  = 1.0f;)
            STAGE_GET_FLOAT     (fDelay, fDelay = 1.0f;)
            STAGE_GET_VEC2_ARRAY(avPosFrom, iNumEnemies)
            STAGE_GET_VEC2_ARRAY(avPosTo,   iNumEnemies)
            STAGE_GET_VEC2      (vSpawnPos)
        STAGE_GET_END

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);

        if(STAGE_BEGINNING)
        {
            avPosTo[0] = m_iInsanity ? HIDDEN_POS : coreVector2( 0.0f,-1.3f);
            avPosTo[1] = m_iInsanity ? HIDDEN_POS : coreVector2( 0.0f, 1.3f);
            avPosTo[2] = m_iInsanity ? HIDDEN_POS : coreVector2(-1.3f, 0.0f);
            avPosTo[3] = m_iInsanity ? HIDDEN_POS : coreVector2( 1.3f, 0.0f);
            avPosTo[4] = HIDDEN_POS;
            avPosTo[5] = HIDDEN_POS;
            avPosTo[6] = HIDDEN_POS;
            avPosTo[7] = HIDDEN_POS;

            this->EnableCorrect();
        }

        const auto nAttackFunc = [&](cEnemy* pEnemy, const coreUintW i)
        {
            const coreVector2 vPos  = pEnemy->GetPosition().xy();
            const coreFloat   fBase = pEnemy->AimAtPlayerDual((iIteration % 2u) ? 0u : 1u).Angle();
            const coreUintW   iNum  = g_pGame->IsEasy() ? 3u : 1u;

            for(coreUintW j = iNum; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iNum - 1u) * 0.5f) * 3.0f) + fBase);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.6f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
            }

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_MAGENTA);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        };

        const auto nPunishFunc = [&](cEnemy* pEnemy)
        {
            const coreVector2 vPos = pEnemy->GetPosition().xy();

            for(coreUintW j = 10u; j--; )
            {
                if(g_pGame->IsEasy() && (j % 2u)) continue;

                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 18.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.6f, pEnemy, vPos,  vDir)->ChangeSize(1.6f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.6f, pEnemy, vPos, -vDir)->ChangeSize(1.6f);
            }

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_MAGENTA);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        };

        const coreUint32 iPrevVisible = iVisible;
        const coreUint32 iPrevBig     = iBig;
        const coreUint32 iPrevGold    = iGold;
        const coreUint32 iPrevPoints  = iPoints;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(HAS_BIT(iVisible, i) && pEnemy->WasDamaged())
            {
                if(!iHit)
                {
                    if(HAS_BIT(iGold, i))
                    {
                        STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                        REMOVE_BIT(iGold, i)

                        g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_YELLOW);
                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                    }
                    else if(!HAS_BIT(iBig, i))
                    {
                        nPunishFunc(pEnemy);

                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.5f, 1.0f, SOUND_EFFECT_ERROR);
                        g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                        ADD_BIT(iRevengeState, i)
                        if(iRevengeState == BITLINE(iNumEnemies)) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pEnemy->GetPosition())
                    }
                    else
                    {
                        iPoints += 1u;

                        if(!m_iInsanity)
                        {
                            this->AddExtraScore(pEnemy->LastAttacker(), 100u, pEnemy->GetPosition());
                        }

                        if(iPoints > 15u)
                        {
                            REMOVE_BIT(iBig, i)
                            if(!iBig) pEnemy->Kill(true);
                        }

                        if(iPoints == 23u)
                        {
                            iSingle = i;
                        }

                        if(iPoints == 3u)
                        {
                            this->DisableCorrect(false);
                        }

                        if(m_iInsanity)
                        {
                            iBig |= iBig << 1u;
                            pEnemy->Kill(true);
                        }

                        g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_BLUE);
                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.5f, 1.0f, SOUND_EFFECT_SUCCESS);
                    }
                }

                ADD_BIT(iHit, i)
            }

            this->CrashEnemy(pEnemy);
        });

        const coreFloat fSpeed = 1.0f + (m_iInsanity ? 0.0f : (0.8f * (I_TO_F(iNumEnemies - pSquad1->GetNumEnemiesAlive()) / I_TO_F(iNumEnemies - 1u))));

        fTime = MIN1(fTime + 0.6f * fSpeed * TIME);
        if(fTime >= 1.0f)
        {
            pHelper->Kill(false);

            iVisible = (iPoints < 23u) ? BITLINE(iNumEnemies) : BIT(iSingle);

            fDelay = MIN1(fDelay + 0.9f * fSpeed * TIME);
            if((fDelay >= 1.0f) || iHit)
            {
                iVisible = 0u;
                iHit     = 0u;
                fTime    = 0.0f;
                fDelay   = 0.0f;

                iIteration += 1u;

                const coreBool bOnce = (iPoints != iPrevPoints);

                if(iPoints == 0u)
                {
                    iBig = BIT(0u);
                }
                else if((iPoints == 7u) && bOnce)
                {
                    if(g_pGame->IsTask())
                    {
                        iGold = BIT(2u);
                    }
                }
                else if(iPoints == 8u)
                {
                    iGold = 0u;
                }
                else if((iPoints == 11u) && bOnce)
                {
                    pHelper->Resurrect(false);
                }
                else if((iPoints == 15u) && bOnce)
                {
                    if(g_pGame->IsTask())
                    {
                        for(coreUintW i = 0u; i < HARENA_EGGS; ++i)
                            this->EnableEgg(i);
                    }

                    iBig = BITLINE(iNumEnemies);
                }
                else if(iPoints >= 23u)
                {
                    for(coreUintW i = 0u; i < HARENA_EGGS; ++i)
                        this->DisableEgg(i, false);

                    if(bOnce)
                    {
                        for(coreUintW j = 0u; j < iNumEnemies; ++j)
                        {
                            if(++iSingle >= iNumEnemies) iSingle = 0u;
                            if(!pSquad1->GetEnemy(iSingle)->HasStatus(ENEMY_STATUS_DEAD)) break;
                        }
                    }

                    iBig = BIT(iSingle);
                }

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    const coreUintW iIndex = i + iIteration;

                    avPosFrom[i] = avPosTo[i];

                    if(m_iInsanity)
                    {
                        ASSERT(iPoints < 11u)   // no rotation

                        if(iPoints < 6u)
                        {
                            coreUintW iStep;
                            switch(iIndex % 6u)
                            {
                            default: UNREACHABLE
                            case 0u: iStep = 0u; break;
                            case 1u: iStep = 2u; break;
                            case 2u: iStep = 4u; break;
                            case 3u: iStep = 1u; break;
                            case 4u: iStep = 5u; break;
                            case 5u: iStep = 3u; break;
                            }
                            if(i < 6u) avPosTo[i] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(iStep) * 60.0f + 90.0f + CORE_MATH_PRECISION)) * 0.95f;

                            if((iPoints == 1u) && (i == 1u)) vSpawnPos = avPosFrom[i];
                            m_Tiger.KillHelper(ELEMENT_BLUE, false);
                        }
                    }
                    else
                    {
                        if(iPoints < 1u)
                        {
                            if(i < 1u) avPosTo[i] = coreVector2(0.0f, (iIteration % 2u) ? 0.5f : -0.5f);
                        }
                        else if(iPoints < 5u)
                        {
                            coreUintW iStep;
                            switch(iIndex % 4u)
                            {
                            default: UNREACHABLE
                            case 0u: iStep = 0u; break;
                            case 1u: iStep = 2u; break;
                            case 2u: iStep = 3u; break;
                            case 3u: iStep = 1u; break;
                            }
                            if(i < 4u) avPosTo[i] = coreVector2(1.0f, (iIteration % 2u) ? -1.0f : 1.0f) * (I_TO_F(iStep) - 1.5f) * 0.5f;
                        }
                        else if(iPoints < 9u)
                        {
                            coreUintW iStep;
                            switch(iIndex % 6u)
                            {
                            default: UNREACHABLE
                            case 0u: iStep = 0u; break;
                            case 1u: iStep = 3u; break;
                            case 2u: iStep = 4u; break;
                            case 3u: iStep = 5u; break;
                            case 4u: iStep = 2u; break;
                            case 5u: iStep = 1u; break;
                            }
                            if(i < 6u) avPosTo[i] = coreVector2((iStep % 2u) ? 1.0f : -1.0f, I_TO_F(iStep / 2u) - 1.0f) * 0.7f;
                        }
                        else if(iPoints < 23u)
                        {
                            coreUintW iStep;
                            switch(iIndex % 8u)
                            {
                            default: UNREACHABLE
                            case 0u: iStep = 0u; break;
                            case 1u: iStep = 3u; break;
                            case 2u: iStep = 6u; break;
                            case 3u: iStep = 2u; break;
                            case 4u: iStep = 7u; break;
                            case 5u: iStep = 4u; break;
                            case 6u: iStep = 1u; break;
                            case 7u: iStep = 5u; break;
                            }
                            avPosTo[i] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(iStep) * 45.0f + 22.5f + CORE_MATH_PRECISION)) * 0.95f;
                        }
                        else
                        {
                            if(i == iSingle) avPosTo[i] = pEnemy->NearestPlayerDual((iIteration % 2u) ? 0u : 1u)->GetPosition().xy() / FOREGROUND_AREA;
                        }
                    }

                    if(avPosFrom[i] == HIDDEN_POS) avPosFrom[i] = avPosTo[i].Processed(SIGN) * 1.2f;
                });
            }
            else if(m_iInsanity)
            {
                if(!vSpawnPos.IsNull() && m_Tiger.ResurrectHelper(ELEMENT_BLUE, false))
                {
                    ASSERT(pHelper != g_pGame->GetHelper(ELEMENT_BLUE))
                    g_pGame->GetHelper(ELEMENT_BLUE)->SetPosition(coreVector3(vSpawnPos * FOREGROUND_AREA, 0.0f));
                }
            }
        }

        if(iPoints >= 11u)
        {
            fRotaSpeed = MIN1(fRotaSpeed + 0.1f * TIME);
        }

        if(iPoints >= 23u)
        {
            const coreMatrix2 mRotaRel = coreMatrix3::Rotation(fRotaSpeed * (0.3f*PI) * TIME).m12();
            for(coreUintW i = 0u; i < iNumEnemies; ++i)
            {
                if(i == iSingle) continue;

                avPosFrom[i] *= mRotaRel;
                avPosTo  [i] *= mRotaRel;
            }
        }

        fRotaValue += fRotaSpeed * (0.3f*PI) * TIME;
        const coreMatrix2 mRotaAbs = coreMatrix3::Rotation(fRotaValue).m12();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(iPoints >= 23u) pEnemy->SetPosition(coreVector3(LERP(avPosFrom[i], avPosTo[i], fTime)            * FOREGROUND_AREA, 0.0f));   // relative
                          else pEnemy->SetPosition(coreVector3(LERP(avPosFrom[i], avPosTo[i], fTime) * mRotaAbs * FOREGROUND_AREA, 0.0f));   // absolute

            if(!HAS_BIT(iVisible, i))
            {
                if(STAGE_TICK_FREE(60.0f, 0.0f))
                {
                    g_pSpecialEffects->CreateSplashColor(coreVector3(pEnemy->GetPosition().xy(), HAS_BIT(iBig, i) ? 0.0f : SPECIAL_DEEP), 0.0f, 1u, HAS_BIT(iBig, i) ? COLOR_ENERGY_BLUE : (COLOR_ENERGY_WHITE * 0.7f), false, true);
                }

                if(!pHelper->HasStatus(HELPER_STATUS_DEAD) && (i == 4u))
                {
                    pHelper->SetPosition(pEnemy->GetPosition());
                }
            }

            if(HAS_BIT(iVisible, i) && !HAS_BIT(iPrevVisible, i))
            {
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                if(g_pForeground->IsVisibleObject(pEnemy) && !m_iInsanity)
                {
                    if(iPoints >= 23u)
                    {
                        nPunishFunc(pEnemy);
                    }
                    else
                    {
                        if(!g_pGame->IsEasy() || ((iPoints < 15u) && HAS_BIT(iBig, i)))
                        {
                            nAttackFunc(pEnemy, i);
                        }
                    }
                }
            }
            else if(!HAS_BIT(iVisible, i) && HAS_BIT(iPrevVisible, i))
            {
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            }

            if(HAS_BIT(iBig, i) && !HAS_BIT(iPrevBig, i))
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
            }
            else if(!HAS_BIT(iBig, i) && HAS_BIT(iPrevBig, i))
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.1f);
                pEnemy->SetBaseColor(COLOR_SHIP_GREY);
            }

            if(HAS_BIT(iGold, i) && !HAS_BIT(iPrevGold, i))
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->SetBaseColor(COLOR_SHIP_YELLOW);
            }
            else if(!HAS_BIT(iGold, i) && HAS_BIT(iPrevGold, i))
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.1f);
                pEnemy->SetBaseColor(COLOR_SHIP_GREY);
            }

            if(HAS_BIT(iBig, i))
            {
                m_Correct.SetPosition(HAS_BIT(iVisible, i) ? coreVector3(HIDDEN_POS, 0.0f) : pEnemy->GetPosition());
            }
        });

        if(g_pGame->IsTask())
        {
            for(coreUintW i = 0u; i < HARENA_EGGS; ++i)
            {
                coreObject3D* pEgg = this->GetEgg(i);
                if(!pEgg->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                pEgg->SetPosition(coreVector3((StepRotated45X(i) * mRotaAbs) * 0.95f * FOREGROUND_AREA, 0.0f));

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pEgg->GetPosition().xy() - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(5.0f))
                    {
                        this->DisableEgg(i, true);

                        if(++iEggState >= HARENA_EGGS)
                        {
                            STAGE_BADGE(1u, BADGE_NORMAL, pEgg->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawCountdown(iEggState, HARENA_EGGS, pEgg->GetPosition());
                            g_pSpecialEffects->PlaySound(pEgg->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iEggState, HARENA_EGGS), SOUND_ITEM_01);
                        }
                    }
                });
            }
        }

        m_iStageSub = MAX(m_iStageSub, iPoints + 1u);

        if(!m_iInsanity)
        {
            cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

            pBackground->SetVeilAlpha(STEPH3(0.0f, 30.0f, m_fStageTime));

            if(STAGE_BEGINNING)
            {
                g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,0.0f), 30.0f);
            }

            STAGE_WAVE(1u, "3-2", {50.0f, 75.0f, 100.0f, 125.0f, 250.0f})   // VIERZEHN +5
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        m_aInsanityStage[1]();
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cViewBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        this->DisableCorrect(false);

        for(coreUintW i = 0u; i < HARENA_EGGS; ++i)
            this->DisableEgg(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetVeilAlpha(1.0f);
        pBackground->SetTrail(false);

        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,0.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // enemies form a tower and fall over
    // - should not create bullets, to focus on the movement and tower (bullets are too distracting) (except for final enemy)
    // - should be fast enough to be a threat, but needs visual space to properly identify movement
    // - complete vertical or horizontal movement is quite easy
    // - don't spawn too far on the side to properly blend them in, but not too near center, to highlight the tower structure
    // - try to stack enemies all facing into the same direction, otherwise it looks not as fascinating
    // - first group, show tower and bounce mechanic
    // - second group, show infinite displaced tower
    // - first group, shoot away from the second tower, to make better use of the space, and highlight the displacement mechanic
    // - for rotating groups, left around, right around, left around
    // - it is important that the bounce start-direction can be properly identified
    // - moving enemies need to look into their fly-direction, to not cause teleportation when hitting them outside the view (though it's more likely they are hit from where they come and not from where they go)
    // - background sandstorm hides shadow artifacts
    // - player can kill a lot of sub-groups by staying at the bottom, which is fine I guess
    // TASK: destroy all unshielded enemies
    // TASK: collect enough flummies at the end
    // ACHIEVEMENT: dance at least 30 seconds with the final enemy without getting hit
    // TODO 1: hardmode: every enemy attacks, or explodes into bullets
    // TODO 1: hardmode: N infinity enemies or bullets
    // TODO 5: coop: take turns hitting opponents
    m_aInsanityStage[2] = [this]()
    {
        constexpr coreUintW iNumEnemies = 61u;
        constexpr coreUintW iNumData    = 20u;   // max number of concurrent active enemies (beware of mover-enemy being active longer)
        constexpr coreFloat fSpeed      = 50.0f;
        constexpr coreFloat fSpeedBig   = 60.0f;

        constexpr coreUintW iHelperIndex  = 8u;
        constexpr coreUintW iEnablerIndex = 52u;
        constexpr coreUintW iMoverIndex   = 59u; STATIC_ASSERT(iMoverIndex == iNumEnemies - 2u)
        constexpr coreUintW iBigIndex     = 60u; STATIC_ASSERT(iBigIndex   == iNumEnemies - 1u)

        constexpr coreUintW iFlummiRequire = HARENA_FLUMMIS / 2u;

        constexpr coreUint8 aiTarget[] = {2u, 13u, 20u, 34u, 37u, 45u, 49u};
        const auto nIsTargetFunc = [&](const coreUint8 iIndex)
        {
            return (!m_iInsanity && g_pGame->IsTask() && std::memchr(aiTarget, iIndex, ARRAY_SIZE(aiTarget)));
        };

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                const coreBool bEnabler = (i == iEnablerIndex);
                const coreBool bMover   = (i == iMoverIndex);
                const coreBool bBig     = (i == iBigIndex);

                pEnemy->SetAlpha (0.0f);
                pEnemy->Configure((bEnabler || bMover) ? 1 : (bBig ? 400 : (nIsTargetFunc(i) ? 30 : 1000)), COLOR_SHIP_BLACK);
                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN | ENEMY_STATUS_WORTHLESS);

                if(bEnabler || bMover || bBig || nIsTargetFunc(i)) pEnemy->RemoveStatus(ENEMY_STATUS_IMMORTAL);
                if(bBig)                                           pEnemy->RemoveStatus(ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(iNumEnemies * 2u + iNumData * 4u + HARENA_FLUMMIS + 7u)
            STAGE_GET_FLOAT_ARRAY(afFall,   iNumEnemies)
            STAGE_GET_FLOAT_ARRAY(afHeight, iNumEnemies)
            STAGE_GET_VEC2_ARRAY (avMove,   iNumData)
            STAGE_GET_VEC2_ARRAY (avRotate, iNumData)
            STAGE_GET_FLOAT      (fDanceTimestamp)
            STAGE_GET_VEC2       (vHelperMove)
            STAGE_GET_UINT       (iMoverActive)
            STAGE_GET_UINT       (iPoints)
            STAGE_GET_UINT       (iKillCount)
            STAGE_GET_UINT       (iFlummiCollect)
            STAGE_GET_UINT_ARRAY (aiFlummiDir, HARENA_FLUMMIS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
            if(m_iInsanity)
            {
                     if(STAGE_SUB(  1u)) STAGE_RESURRECT(pSquad1, 0u, 15u)
            }
            else
            {
                     if(STAGE_SUB(  1u)) STAGE_RESURRECT(pSquad1, 0u, 60u)
                else if(STAGE_SUB(100u)) STAGE_DELAY_START_CLEAR
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_CYAN);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + vHelperMove * (fSpeed * TIME);
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        if(STAGE_BEGINNING)
        {
            this->EnableAim();
        }

        if(m_iStageSub == 100u)
        {
            if(g_pGame->IsTask())
            {
                if(STAGE_BEGINNING2)
                {
                    const cEnemy* pBig = pSquad1->GetEnemy(iBigIndex);

                    for(coreUintW i = 0u; i < HARENA_FLUMMIS; ++i)
                    {
                        this->EnableFlummi(i);

                        this->GetFlummi(i)->SetPosition(pBig->GetPosition());
                        aiFlummiDir[i] = coreVector2::Direction(I_TO_F(i) / I_TO_F(HARENA_FLUMMIS) * (2.0f*PI)).PackFloat2x16();
                    }

                    g_pSpecialEffects->CreateSplashColor(pBig->GetPosition(), 100.0f, 20u, COLOR_ENERGY_ORANGE);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
                }
                else if(STAGE_SUBTIME_AFTER(6.0f))
                {
                    STAGE_DELAY_END

                    for(coreUintW i = 0u; i < HARENA_FLUMMIS; ++i)
                    {
                        coreObject3D* pFlummi = this->GetFlummi(i);
                        if(!pFlummi->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                        this->DisableFlummi(i, false);

                        g_pSpecialEffects->CreateBlowColor(pFlummi->GetPosition(), pFlummi->GetPosition().Normalized(), 50.0f, 10u, COLOR_ENERGY_ORANGE);
                    }
                }

                for(coreUintW i = 0u; i < HARENA_FLUMMIS; ++i)
                {
                    coreObject3D* pFlummi = this->GetFlummi(i);
                    if(!pFlummi->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                    coreVector2 vCurPos = pFlummi->GetPosition().xy();
                    coreVector2 vCurDir = coreVector2::UnpackFloat2x16(aiFlummiDir[i]);

                    vCurPos += vCurDir * ((30.0f + 5.0f * I_TO_F(i % 3u)) * TIME);
                    STATIC_ASSERT(coreMath::IsAligned(HARENA_FLUMMIS, 3u))

                         if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.1f); vCurDir.x =  ABS(vCurDir.x);}
                    else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.1f); vCurDir.x = -ABS(vCurDir.x);}
                         if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.1f); vCurDir.y =  ABS(vCurDir.y);}
                    else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.1f); vCurDir.y = -ABS(vCurDir.y);}

                    pFlummi->SetPosition(coreVector3(vCurPos, 0.0f));
                    aiFlummiDir[i] = vCurDir.PackFloat2x16();

                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pFlummi->GetPosition().xy() - pPlayer->GetPosition().xy();
                        if(vDiff.LengthSq() < POW2(5.0f))
                        {
                            this->DisableFlummi(i, true);

                            if(++iFlummiCollect >= iFlummiRequire)
                            {
                                STAGE_BADGE(1u, BADGE_NORMAL, pFlummi->GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawCountdown(iFlummiCollect, iFlummiRequire, pFlummi->GetPosition());
                            }

                            g_pSpecialEffects->PlaySound(pFlummi->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iFlummiCollect, iFlummiRequire), SOUND_ITEM_01);   // always
                        }
                    });
                }
            }
            else
            {
                STAGE_DELAY_END
            }
        }

        for(coreUintW i = 0u; i < HARENA_FLOORS; ++i)
        {
            if(m_apFloorOwner[i] && m_apFloorOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableFloor(i, true);
        }

        coreUint32 iAssert = 0u;
        coreBool   bFirst  = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if(!pEnemy->HasStatus(ENEMY_STATUS_HIDDEN))
            {
                ASSERT(!HAS_BIT(iAssert, i % iNumData))
                ADD_BIT(iAssert, i % iNumData)
            }

            coreVector2& vMove   = STAGE_SINK_VEC2(avMove  [i % iNumData]);
            coreVector2& vRotate = STAGE_SINK_VEC2(avRotate[i % iNumData]);

            const coreBool bEnabler = (i == iEnablerIndex);
            const coreBool bMover   = (i == iMoverIndex);
            const coreBool bBig     = (i == iBigIndex);

            if(nIsTargetFunc(i) && pEnemy->ReachedDeath())   // killed by player
            {
                this->AddExtraScore(pEnemy->LastAttacker(), 100u, pEnemy->GetPosition());

                if(++iKillCount == ARRAY_SIZE(aiTarget))
                {
                    STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                }
                else
                {
                    g_pGame->GetCombatText()->DrawProgress(iKillCount, ARRAY_SIZE(aiTarget), pEnemy->GetPosition());
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iKillCount, ARRAY_SIZE(aiTarget)), SOUND_ITEM_02);
                }
            }

            if(STAGE_TAKEOFF)
            {
                afHeight[i] = 60.0f + 10.0f * I_TO_F(i);
            }

            const coreFloat fGround = (((i == 0u) || pSquad1->GetEnemy(i-1u)->GetLostHealth()) ? 0.0f : (pSquad1->GetEnemy(i-1u)->GetPosition().z + 9.0f));

            afFall  [i] += -120.0f   * TIME;
            afHeight[i] += afFall[i] * TIME;

            if(afHeight[i] < fGround)
            {
                afFall  [i] = MAX0(afFall[i]);
                afHeight[i] = fGround;
            }

            const coreBool bFirstDamage = (pEnemy->ReachedHealth(pEnemy->GetMaxHealth() - 1));
            const coreBool bAnyDamage   = (pEnemy->GetCurHealth() != pEnemy->GetMaxHealth());

            if(bFirstDamage || (bMover && (iMoverActive == 1u)))
            {
                vMove     = pEnemy->GetDirection().xy();
                vRotate.x = pEnemy->GetDirection().xy().Angle();
                vRotate.y = 0.0f;

                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_PLAYER);

                if(!bMover)                               pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
                if(!bBig && !nIsTargetFunc(i))            pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);
                if(!bMover && !bBig && !nIsTargetFunc(i)) pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                if(bEnabler) iMoverActive = 1u;
                if(bMover)   iMoverActive = 2u;

                this->DisableFloor(i % HARENA_FLOORS, false);

                if(bBig || (m_iInsanity && (i == 15u)))
                {
                    this->DisableAim(true);
                }

                if(bBig)
                {
                    fDanceTimestamp = g_pGame->GetTimeTable()->GetTimeTotal();
                }

                if(!bMover)
                {
                    g_pSpecialEffects->CreateBlowColor(pEnemy->GetPosition(), pEnemy->GetDirection(), 80.0f, 20u, COLOR_ENERGY_BLUE);
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
                }
            }

            if(bAnyDamage || (bMover && (iMoverActive == 2u)))
            {
                coreVector2 vCurPos = pEnemy->GetPosition().xy();
                coreVector2 vCurDir = vMove;

                vCurPos += vCurDir * ((bBig ? fSpeedBig : fSpeed) * TIME);

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.05f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.05f); vCurDir.x =  ABS(vCurDir.x);}
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.05f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.05f); vCurDir.x = -ABS(vCurDir.x);}
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.05f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.05f); vCurDir.y =  ABS(vCurDir.y);}
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.05f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.05f); vCurDir.y = -ABS(vCurDir.y);}

                pEnemy->SetPosition(coreVector3(vCurPos, afHeight[i]));
                vMove = vCurDir;

                vRotate.y += (bBig ? 0.5f : 1.0f) * TIME;
                pEnemy->DefaultRotate(vRotate.x + vRotate.y * (2.0f*PI));

                if(!bMover && !bBig && (vRotate.y >= (nIsTargetFunc(i) ? 5.0f : (g_pGame->IsEasy() ? 1.5f : 3.0f))))
                {
                    pEnemy->Kill(true);

                    if(!m_iInsanity)
                    {
                        this->AddExtraScore(pEnemy->LastAttacker(), 100u, pEnemy->GetPosition());

                        if(i == iHelperIndex)
                        {
                            pHelper->Resurrect(true);
                            pHelper->SetPosition(pEnemy->GetPosition());

                            vHelperMove = vMove;

                            g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), 25.0f, 5u, COLOR_ENERGY_CYAN);
                        }
                    }

                    iPoints += 1u;
                }
            }
            else
            {
                coreVector2 vPos;
                coreVector2 vDir;

                constexpr coreFloat fDistance  = 0.4f;
                constexpr coreFloat fDistance2 = 0.9f;

                if(m_iInsanity)
                {
                    if(i < 8u)
                    {
                        vPos = StepRotated45((i * 3u) % 8u).InvertedX() * fDistance;
                        vDir = vPos.Normalized();
                    }
                    else
                    {
                        coreFloat fOffset;
                        switch((i - 8u) / 2u)
                        {
                        default: UNREACHABLE
                        case 0u: fOffset = -1.0f; break;
                        case 1u: fOffset =  1.0f; break;
                        case 2u: fOffset =  1.0f; break;
                        case 3u: fOffset = -1.0f; break;
                        }

                        const coreFloat fSide = (((i - 8u) / 2u) % 2u) ? -1.0f : 1.0f;

                        vPos = coreVector2( fSide,      fOffset) * fDistance * 2.0f;
                        vDir = coreVector2(-fSide, SIGN(fOffset)).Normalized() * ((i % 2u) ? 1.0f : -1.0f);
                    }
                }
                else
                {
                    if(i < 8u)
                    {
                        vPos = coreVector2( 0.0f,1.0f) * fDistance;
                        vDir = coreVector2(-1.0f,1.0f).Normalized();
                    }
                    else if(i < 12u)
                    {
                        vPos = coreVector2(1.0f,1.0f) * fDistance;
                        vDir = StepRotated90X((i - 8u + 1u) % 4u);
                    }
                    else if(i < 16u)
                    {
                        vPos = coreVector2(-1.0f,1.0f) * fDistance;
                        vDir = StepRotated90X((i - 12u + 1u) % 4u);
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
                    else if(i < 40u)
                    {
                        vPos = MapToAxis(StepRotated45((i - 32u) % 8u), coreVector2::Direction(fLifeTime)) * fDistance2;
                        vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();
                    }
                    else if(i < 46u)
                    {
                        vPos = coreVector2(FmodRange(fLifeTime - I_TO_F(i - 40u) * (2.6f/6.0f), -1.3f, 1.3f), ((i - 40u) % 2u) ? fDistance2 : -fDistance2);
                        vDir = coreVector2(-1.0f, ((i - 40u) % 2u) ? -1.0f : 1.0f).Normalized();
                    }
                    else if(i < 52u)
                    {
                        vPos = coreVector2((I_TO_F(i - 46u) - 2.5f) * 0.35f, FmodRange(-fLifeTime, -1.3f, 1.3f));
                        vDir = coreVector2(0.0f,-1.0f).Normalized();
                    }
                    else if(i < 59u)
                    {
                        const cEnemy* pMover = pSquad1->GetEnemy(iMoverIndex);

                        vPos = pMover->GetPosition ().xy() / FOREGROUND_AREA;
                        vDir = pMover->GetDirection().xy();
                    }
                    else if(i < 60u)
                    {
                        ASSERT(bMover)

                        vPos = coreVector2(0.0f,1.0f) * fDistance;
                        vDir = coreVector2(1.0f,2.0f).Normalized();
                    }
                    else
                    {
                        ASSERT(bBig)

                        vPos = coreVector2( 0.0f,0.0f);
                        vDir = coreVector2(-2.0f,1.0f).Normalized();
                    }
                }

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, afHeight[i]));
                pEnemy->SetDirection(coreVector3(vDir, 0.0f));
            }

            if((pEnemy->HasStatus(ENEMY_STATUS_GHOST) || bMover) && !afHeight[i])
            {
                pEnemy->SetBaseColor(COLOR_SHIP_RED);
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);

                pEnemy->ChangeToNormal();
            }

            const coreFloat fFactor     = bBig ? 1.5f : 1.0f;
            const coreFloat fScale      =       1.3f - 0.3f * STEPH3( 0.0f,  3.0f, afHeight[i]);
            const coreFloat fVisibility = CLAMP(1.0f - 1.0f * STEPH3(35.0f, 54.0f, afHeight[i]), pEnemy->GetAlpha(), 1.0f);
            const coreFloat fAlpha      = CLAMP(1.0f - 1.0f * STEPH3(49.0f, 54.0f, afHeight[i]), pEnemy->GetAlpha(), 1.0f);

            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * fVisibility * fScale * fFactor);
            pEnemy->SetAlpha(fAlpha);

            if(pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) && fVisibility)
            {
                this->EnableFloor(i % HARENA_FLOORS, pEnemy, fFactor);
                pEnemy->RemoveStatus(ENEMY_STATUS_HIDDEN);
            }

            if(bBig && bAnyDamage)
            {
                if(STAGE_TICK_LIFETIME(20.0f, 0.0f) && (!g_pGame->IsEasy() || ((s_iTick % 6u) < 3u)))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.4f, pEnemy, vPos, vDir)->ChangeSize(1.4f);

                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(bBig && pEnemy->ReachedDeath())
            {
                fDanceTimestamp = 0.0f;
            }

            if(!bAnyDamage || bMover)
            {
                if(!bFirst)
                {
                    bFirst = true;

                    const coreVector2 vPos = ((i == 0u) && pEnemy->HasStatus(ENEMY_STATUS_GHOST)) ? HIDDEN_POS : (pEnemy->GetPosition().xy() + pEnemy->GetDirection().xy() * (7.0f * fFactor));
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    m_Aim.SetPosition (coreVector3(vPos, 0.0f));
                    m_Aim.SetDirection(coreVector3(vDir, 0.0f));

                    const coreFloat fValue = STEPH3(1.0f, 9.0f, pEnemy->GetPosition().z);
                    const coreFloat fSize  = LERP  (1.5f, 2.5f, fValue);

                    m_Aim.SetSize (coreVector3(1.0f,1.0f,1.0f) * fSize);
                    m_Aim.SetAlpha(1.0f - fValue);
                }
            }

            this->CrashEnemy(pEnemy);
        });

        if(fDanceTimestamp)
        {
            const coreFloat fCurTimestamp = g_pGame->GetTimeTable()->GetTimeTotal();

            coreBool bReset = !HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY);
            STAGE_FOREACH_PLAYER(pPlayer, j)
            {
                if(pPlayer->WasDamaged()) bReset = true;
            });

            if(bReset) fDanceTimestamp = fCurTimestamp;

            if(fCurTimestamp - fDanceTimestamp >= 30.0f)
            {
                STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pSquad1->GetEnemy(iBigIndex)->GetPosition())
            }
        }

        m_iStageSub = MAX(m_iStageSub, iPoints + 1u);

        if(!m_iInsanity)
        {
            cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

            if(STAGE_BEGINNING)
            {
                pBackground->GetOutdoor()->LerpHeight(1.0f, 0.0f);
            }

            const coreFloat fEnvLerp = pBackground->GetOutdoor()->GetLerp();

            pBackground->SetGroundDensity(0u, STEP(0.5f, 1.0f, fEnvLerp));
            pBackground->SetGroundDensity(1u, STEP(0.5f, 1.0f, fEnvLerp));
            pBackground->SetGroundDensity(2u, STEP(0.5f, 1.0f, fEnvLerp));
            pBackground->SetGroundDensity(3u, STEP(0.5f, 1.0f, fEnvLerp));
            pBackground->SetGroundDensity(5u, STEP(0.5f, 1.0f, 1.0f - fEnvLerp));

            if(STAGE_BEGINNING)
            {
                g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,-1.0f), 30.0f);
            }

            STAGE_WAVE(2u, "3-3", {45.0f, 65.0f, 90.0f, 110.0f, 220.0f})   // FÜNFZEHN +5
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        m_aInsanityStage[2]();
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < HARENA_FLOORS; ++i)
            this->DisableFloor(i, false);

        for(coreUintW i = 0u; i < HARENA_FLUMMIS; ++i)
            this->DisableFlummi(i, false);

        this->DisableAim(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetVeilAlpha(1.0f);

        pBackground->GetOutdoor()->LerpHeightNow(1.0f, 0.0f);
        pBackground->SetGroundDensity(0u, 1.0f);
        pBackground->SetGroundDensity(1u, 1.0f);
        pBackground->SetGroundDensity(2u, 1.0f);
        pBackground->SetGroundDensity(3u, 1.0f);
        pBackground->SetGroundDensity(5u, 0.0f);

        g_pEnvironment->SetTargetDirectionNow(coreVector2(0.0f,-1.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // mothership spawns enemies
    // - create 1-2 arrow burst attacks without mother as introduction, player may think those are normal enemies, but should get suspicious
    // - wave attack should not spin too fast, as this makes it impossible to properly slip through a hole, but fast enough to make the player move
    // - shield may require the player to destroy it from the coming spin-side, to properly attack the mother between his attacks
    // - player can actually stand still against burst attacks without getting hit, but that's fine
    // - targeted border attack requires player to move and attack to create proper holes
    // - line border attack requires the player to always shoot holes, while the targeted border attack can be avoided with movement
    // - many of the spawn locations have last player position in mind, based on possible attack locations
    // - arrows should have enough life to drive player back if he just keeps shooting at them, but not too much to provide action
    // - time between burst attacks gives the player a chance to attack
    // - slight initial delay for the burst attack, to reduce clustering (due to mother movement)
    // - coop attack order: intro 1 0, single 1 0 1, from the right 0 1
    // - number of enemies/arrows determined by worst-case, 3x16 + 100 (transition from top-lines to multi-shield)
    // - sun pattern should come from above, this is easier to counter (enemy pattern before goes downwards) and easier to navigate because you can then kill the mother from below (natural direction)
    // - one sun-layer must rotate inverted, otherwise it is far too easy to destroy the shield
    // - moving mothers (from the right) should be exactly between the arrows to reduce the risk of a bad player attack (where only 1 of 2 bullets hits)
    // - multi-shield and sun must expand slowly, otherwise the player will no longer be able to react
    // - multi-shield needs good spacing between layers so player can fly in if he wants
    // - you could disable resurrection of every second enemy and the patterns would all still be beautiful
    // TASK: kill N arrows at the start
    // TASK: kill all marked arrows
    // ACHIEVEMENT: never destroy one of the small enemies
    // TODO 1: hardmode: every killed enemy makes an attack (target single ?)
    // TODO 1: pacifist: holes in line border attack
    m_aInsanityStage[3] = [this]()
    {
        constexpr coreUintW iNumEnemies = 170u;
        constexpr coreUintW iNumTypes   = coreMath::CeilAlign(iNumEnemies,  4u) /  4u;
        constexpr coreUintW iNumState   = coreMath::CeilAlign(iNumEnemies, 16u) / 16u;

        // {sub-stage, child-number}
        constexpr coreUint32 aaiBreak[][2] = {{2u, 1u}, {3u, 1u}, {7u, 26u}, {7u, 39u}, {7u, 57u}, {8u, 1u}, {9u, 20u}, {9u, 48u}, {10u, 36u}};

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.4f),  coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.75f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.4f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.4f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(50 + 20, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.15f);
                pEnemy->Configure(g_pGame->IsEasy() ? 6 : 10, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_NODELAY);
            });
        });

        STAGE_GET_START(9u + iNumTypes + iNumState + iNumEnemies)
            STAGE_GET_UINT      (iTakeStart)
            STAGE_GET_UINT      (iLoopKill)
            STAGE_GET_UINT      (iLoopCount1)
            STAGE_GET_UINT      (iLoopCount2)
            STAGE_GET_UINT      (iLoopState)
            STAGE_GET_UINT      (iInitCount)
            STAGE_GET_UINT      (iBreakCount)
            STAGE_GET_UINT      (iBreakRef)
            STAGE_GET_UINT      (iHelperState)
            STAGE_GET_UINT_ARRAY(aiType,   iNumTypes)
            STAGE_GET_UINT_ARRAY(aiState,  iNumState)
            STAGE_GET_UINT_ARRAY(aiVector, iNumEnemies)
        STAGE_GET_END

        STATIC_ASSERT(sizeof(coreUint32) * iNumTypes >= iNumEnemies)
        coreUint8* aiByteType = r_cast<coreUint8*>(aiType);

        const auto nSetState = [&](const coreUintW iIndex, const coreUint8 iValue)
        {
            ASSERT((iIndex < iNumState * 16u) && (iValue < 4u))
            SET_BITVALUE(aiState[iIndex / 16u], 2u, (iIndex % 16u) * 2u, iValue)
        };

        const auto nGetState = [&](const coreUintW iIndex)
        {
            ASSERT(iIndex < iNumState * 16u)
            return GET_BITVALUE(aiState[iIndex / 16u], 2u, (iIndex % 16u) * 2u);
        };

        const auto nHasBreak = [&]()
        {
            iBreakRef += 1u;

            for(coreUintW i = 0u; i < ARRAY_SIZE(aaiBreak); ++i)
            {
                if((aaiBreak[i][0] == m_iStageSub) && (aaiBreak[i][1] == iBreakRef))
                    return 2u;
            }

            return 0u;
        };

        const auto nTakeChildFunc = [&](const coreUint8 iType, const coreUint8 iState = 0u)
        {
            for(coreUintW i = iTakeStart, ie = pSquad2->GetNumEnemies(); i < ie; ++i)
            {
                cEnemy* pCurrent = pSquad2->GetEnemy(i);

                if(pCurrent->HasStatus(ENEMY_STATUS_DEAD))
                {
                    iTakeStart = i + 1u;

                    aiByteType[i] = iType;
                    aiVector  [i] = coreVector2(0.0f,0.0f).PackFloat2x16();   // reset

                    if(g_pGame->IsTask())
                    {
                        nSetState(i, iState);

                        if(iState == 2u) pCurrent->SetBaseColor(COLOR_SHIP_BLACK, false, true);
                                    else pCurrent->SetBaseColor(COLOR_SHIP_MAGENTA);
                    }

                    d_cast<cArrowEnemy*>(pCurrent)->SetAngle(0.0f);

                    pCurrent->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    pCurrent->Resurrect();

                    return pCurrent;
                }
            }

            WARN_IF(true) {}
            return pSquad2->GetEnemy(0u);
        };

        const auto nReturnChildFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i, const coreBool bAnimated)
        {
            pEnemy->Kill(bAnimated);
            iTakeStart = MIN(iTakeStart, i);
        };

        const auto nDropShieldFunc = [&](const coreUint8 iType)
        {
            ASSERT(iType >= 4u)
            STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
            {
                if(aiByteType[i] == iType)
                {
                    nReturnChildFunc(pEnemy, i, InBetween(pEnemy->GetPosition().xy(), -FOREGROUND_AREA * 1.1f, FOREGROUND_AREA * 1.1f));
                }
            });
        };

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            if(pEnemy->ReachedDeath())   // killed by player
            {
                iTakeStart = MIN(iTakeStart, i);
                iLoopKill  = iLoopKill + 1u;

                if(!m_iInsanity) STAGE_FAILTROPHY

                if(g_pGame->IsTask())
                {
                    const coreUint8 iState = nGetState(i);

                    if(iState == 1u)
                    {
                        if(++iInitCount >= 20u)
                        {
                            STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawCountdown(iInitCount, 20u, pEnemy->GetPosition());
                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iInitCount, 20u), SOUND_ITEM_01);
                        }
                    }
                    else if(iState == 2u)
                    {
                        if(++iBreakCount == ARRAY_SIZE(aaiBreak))
                        {
                            STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawProgress(iBreakCount, ARRAY_SIZE(aaiBreak), pEnemy->GetPosition());
                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iBreakCount, ARRAY_SIZE(aaiBreak)), SOUND_ITEM_02);
                        }
                    }
                }
            }
        });

        if(pSquad1->IsFinished())
        {
            if(m_iInsanity == HARENA_INSANITY_P1)
            {
                     if(STAGE_SUB(15u)) {}
            }
            else if(m_iInsanity)
            {
                     if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
                else if(STAGE_SUB(14u)) {}   // may not be used
            }
            else
            {
                     if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
                else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
                else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
                else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
                else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  4u)
                else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  5u,  5u)
                else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
                else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
                else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1,  9u, 11u)
                else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 12u, 12u)
                else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 13u, 14u)
                else if(STAGE_SUB(12u))
                {
                    pSquad2->ClearEnemies(true);

                    if(!iLoopKill) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
                }

                iBreakRef = 0u;
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_ORANGE);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + coreVector2(-50.0f,0.0f) * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        g_pSpecialEffects->ShakeOverride(0.5f);

        if(m_iStageSub == 1u)
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f) && (s_iTick < 30u))
            {
                cEnemy*         pChild = nTakeChildFunc(0u, 1u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreFloat   fOff = (s_iTick < 15u) ? 1.0f : -1.0f;
                const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vAim = (g_pGame->FindPlayerDual((s_iTick < 15u) ? 0u : 1u)->GetPosition().xy() - vPos).Normalized();
                const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                aiVector[iIndex] = vDir.PackFloat2x16();
            }
        }
        else if((m_iStageSub == 4u) || (m_iStageSub == 5u))
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f))
            {
                if(!iHelperState)
                {
                    iHelperState = true;

                    const coreFloat   fOff = g_pGame->FindPlayerDual(0u)->GetPosition().y;
                    const coreVector2 vPos = coreVector2(1.2f * FOREGROUND_AREA.x, fOff);

                    pHelper->Resurrect(false);
                    pHelper->SetPosition(coreVector3(vPos, 0.0f));
                }
                else
                {
                    cEnemy*         pChild = nTakeChildFunc(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreFloat   fOff = g_pGame->FindPlayerDual((m_iStageSub == 4u) ? 0u : 1u)->GetPosition().y;
                    const coreVector2 vPos = coreVector2(1.2f * FOREGROUND_AREA.x, fOff);

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    aiVector[iIndex] = coreVector2(-1.0f,0.0f).PackFloat2x16();
                }
            }
        }
        else if(m_iStageSub == 7u)
        {
            if(STAGE_BEGINNING2 || STAGE_TICK_FREE2(1.0f, 0.0f))
            {
                for(coreUintW j = 0u; j < 16u; ++j)
                {
                    cEnemy*         pChild = nTakeChildFunc(0u, nHasBreak());
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreFloat   fOff = (I_TO_F(j) - 7.5f) * 0.1375f;
                    const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    aiVector[iIndex] = coreVector2(0.0f,-1.0f).PackFloat2x16();
                }
            }
        }
        else if(m_iStageSub == 9u)
        {
            if(STAGE_TICK_FREE2(2.5f, 0.0f))
            {
                for(coreUintW j = 0u; j < 16u; ++j)
                {
                    if((s_iTick % 2u) == ((j / 4u) % 2u)) continue;

                    cEnemy*         pChild = nTakeChildFunc(0u, nHasBreak());
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreFloat   fOff = (I_TO_F(j) - 7.5f) * 0.1375f;
                    const coreVector2 vPos = coreVector2(-1.2f, fOff) * FOREGROUND_AREA;

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    aiVector[iIndex] = coreVector2(1.0f,0.0f).PackFloat2x16();
                }
            }
        }
        else if(m_iStageSub == 11u)
        {
            if(STAGE_TICK_FREE2(0.9f * (g_pGame->IsEasy() ? 0.7f : 1.0f), 0.0f))
            {
                for(coreUintW j = 0u; j < 32u; ++j)
                {
                    cEnemy*         pChild = nTakeChildFunc(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreFloat   fOff = (I_TO_F(j % 16u) - 7.5f) * 0.1375f;
                    const coreVector2 vPos = coreVector2(fOff, (j < 16u) ? 1.2f : -1.2f) * FOREGROUND_AREA;
                    const coreVector2 vAim = (g_pGame->FindPlayerDual((j < 16u) ? 0u : 1u)->GetPosition().xy() - vPos).Normalized();

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    aiVector[iIndex] = vAim.PackFloat2x16();
                }
            }
        }
        else if(m_iStageSub == 13u)
        {
            if(STAGE_TICK_FREE2(10.0f, 0.0f))
            {
                if(!(s_iTick % 16u))
                {
                    if(++iLoopCount1 >= 5u) iLoopState = 1u;
                }

                if(!iLoopState)
                {
                    cEnemy*         pChild = nTakeChildFunc(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreVector2 vDir = StepRotated90(iLoopCount1 % 4u);
                    const coreFloat   fOff = (I_TO_F(s_iTick % 16u) - 7.5f) * 0.1375f;
                    const coreVector2 vPos = (vDir * -1.2f + vDir.Rotated90() * fOff) * FOREGROUND_AREA;

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    aiVector[iIndex] = vDir.PackFloat2x16();
                }
                else if(iLoopKill < 40u)
                {
                    iLoopCount2 += 1u;

                    cEnemy*         pChild = nTakeChildFunc(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreFloat   fOff = (I_TO_F(((iLoopCount2 / 2u) * 7u) % 17u) - 8.0f) * 0.1375f;
                    const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    aiVector[iIndex] = coreVector2(0.0f,-1.0f).PackFloat2x16();
                }

                if((s_iTick == 47u) && m_Tiger.ResurrectHelper(ELEMENT_GREEN, false))
                {
                    ASSERT(pHelper != g_pGame->GetHelper(ELEMENT_GREEN))
                    g_pGame->GetHelper(ELEMENT_GREEN)->SetPosition(coreVector3(-1.2f, -7.5f * 0.1375f, 0.0f) * FOREGROUND_AREA3);
                }
            }
        }

        if(!m_avChildData.empty())
        {
            FOR_EACH(it, m_avChildData)
            {
                const sChildData& oData = (*it);

                cEnemy*         pChild = nTakeChildFunc(oData.iType);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                pChild->SetPosition(coreVector3(oData.vPosition, 0.0f));

                aiVector[iIndex] = oData.vMove.PackFloat2x16();
            }

            m_avChildData.clear();
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i >= 9u && i < 12u) ? 0.8f : 1.0f, (i < 1u) ? 5.7f : ((i >= 9u && i < 12u) ? (0.5f * I_TO_F(i - 9u)) : 0.0f))

            if(i >= 9u && i < 12u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i - 9u) - 1.0f) * 0.55f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }
            else if(i < 15u)
            {
                const coreVector2 vFactor = coreVector2(1.0f, (i == 4u || i == 6u || i == 7u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = coreVector2((i == 1u || i == 2u || i == 4u || i == 6u || i == 7u) ? ((i % 2u) ? 0.75f : -0.75f) : 0.0f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i ==  3u) pEnemy->Rotate90 ();
                else if(i == 12u) pEnemy->Rotate270();
                else if(i == 13u) pEnemy->Rotate180();

                if(i < 3u)
                {
                    pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayerDual((i % 2u) ? 0u : 1u).Normalized(), 0.0f));
                }
                else
                {
                    const coreFloat fBase = pEnemy->GetDirection().xy().Angle();
                    pEnemy->DefaultRotateLerp(fBase + (0.0f*PI), fBase + (2.0f*PI), BLENDB(CLAMP01(fLifeTime * RCP(pPath1->GetTotalDistance()))));
                }
            }
            else
            {
                pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            }

            if(i < 3u)
            {
                if(STAGE_TICK_LIFETIME(10.0f, 0.0f) && InBetween(s_iTick % 30u, 5u, 20u))
                {
                    cEnemy*         pChild = nTakeChildFunc(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreVector2 vAim = pEnemy->GetDirection().xy();
                    const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                    pChild->SetPosition(pEnemy->GetPosition());

                    aiVector[iIndex] = vDir.PackFloat2x16();

                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else if(i == 5u)
            {
                if(STAGE_TICK_LIFETIME(2.0f, 0.0f) && ((s_iTick % 3u) < 2u))
                {
                    for(coreUintW j = 0u; j < 20u; ++j)
                    {
                        cEnemy*         pChild = nTakeChildFunc(1u);
                        const coreUintW iIndex = pSquad2->GetIndex(pChild);

                        pChild->SetPosition(pEnemy->GetPosition());

                        aiVector[iIndex] = coreVector2(((I_TO_F(j) / 20.0f) + CORE_MATH_PRECISION) * (2.0f*PI) * ((s_iTick % 3u) ? 1.0f : -1.0f), 0.0f).PackFloat2x16();
                    }

                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(STAGE_TAKEOFF)
            {
                if((i >= 1u && i < 8u) || (i >= 13u && i < 15u))
                {
                    for(coreUintW j = 0u; j < 10u; ++j)
                    {
                        cEnemy*         pChild = nTakeChildFunc(i << 2u, (i < 3u) ? nHasBreak() : 0u);
                        const coreUintW iIndex = pSquad2->GetIndex(pChild);

                        pChild->SetPosition(pEnemy->GetPosition());

                        aiVector[iIndex] = coreVector2(((I_TO_F(j) / 10.0f) + CORE_MATH_PRECISION) * (2.0f*PI) * -1.0f, 12.0f).PackFloat2x16();
                    }
                }
                else if(i == 8u)
                {
                    for(coreUintW k = 0u; k < 5u; ++k)
                    {
                        for(coreUintW j = 0u, je = 24u + 12u * k; j < je; ++j)
                        {
                            if((j % 6u) < 3u) continue;

                            cEnemy*         pChild = nTakeChildFunc(i << 2u, nHasBreak());
                            const coreUintW iIndex = pSquad2->GetIndex(pChild);

                            pChild->SetPosition(pEnemy->GetPosition());

                            aiVector[iIndex] = coreVector2(((I_TO_F(j) / I_TO_F(je)) + CORE_MATH_PRECISION) * (2.0f*PI) * ((k % 2u) ? 1.0f : -1.0f), (12.0f + 6.0f * I_TO_F(k)) * 1.6f).PackFloat2x16();
                        }
                    }
                }
                else if(i == 12u)
                {
                    for(coreUintW k = 0u; k < 12u; ++k)
                    {
                        for(coreUintW j = 0u, je = (k < 2u) ? 12u : 6u; j < je; ++j)
                        {
                            cEnemy*         pChild = nTakeChildFunc(i << 2u, nHasBreak());
                            const coreUintW iIndex = pSquad2->GetIndex(pChild);

                            pChild->SetPosition(pEnemy->GetPosition());

                            aiVector[iIndex] = coreVector2(((I_TO_F(j) / I_TO_F(je)) + CORE_MATH_PRECISION) * (2.0f*PI) * ((k == 0u) ? 1.0f : -1.0f), 12.0f + 6.0f * I_TO_F(k)).PackFloat2x16();
                        }
                    }
                }
            }

            if(pEnemy->ReachedDeath())
            {
                if(i) nDropShieldFunc(i << 2u);
            }
        });

        const coreBool bEffectTick = STAGE_TICK_FREE(30.0f, 0.0f);

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            this->CrashEnemy(pEnemy);   // should not be affected by automatic kill

            const coreUint8 iType = aiByteType[i];

            coreVector2 vRaw = coreVector2::UnpackFloat2x16(aiVector[i]);

            if(iType == 0u)   // bullet
            {
                const coreVector2 vNewPos = pEnemy->GetPosition().xy() + vRaw * (50.0f * TIME);
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(iType == 1u)   // wave
            {
                const coreVector2 vCenter = pEnemy->GetPosition().xy() - coreVector2::Direction(vRaw.x) * vRaw.y;

                vRaw.x +=  0.7f * TIME * SIGN(vRaw.x);
                vRaw.y += 40.0f * TIME;

                const coreVector2 vNewPos = vCenter + coreVector2::Direction(vRaw.x) * vRaw.y;
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(iType == 2u)   // bounce bullet
            {
                coreVector2 vNewPos = pEnemy->GetPosition().xy() + vRaw * (50.0f * TIME);

                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.1f) && (vRaw.x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x * 1.1f); vRaw.x =  ABS(vRaw.x);}
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.1f) && (vRaw.x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x * 1.1f); vRaw.x = -ABS(vRaw.x);}
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.1f) && (vRaw.y < 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.1f); vRaw.y =  ABS(vRaw.y);}
                else if((vNewPos.y >  FOREGROUND_AREA.y * 1.1f) && (vRaw.y > 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y - FOREGROUND_AREA.y * 1.1f); vRaw.y = -ABS(vRaw.y);}

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(iType >= 4u)   // shield
            {
                const coreVector2 vCenter = pSquad1->GetEnemy(iType >> 2u)->GetPosition().xy();
                const coreFloat   fLen    = LERPB(0.0f, vRaw.y, MIN1(fLifeTime * (((m_iStageSub == 8u) || (m_iStageSub == 10u)) ? 0.45f : 1.0f)));

                vRaw.x = FMOD(vRaw.x + 1.0f * TIME * SIGN(vRaw.x), 2.0f*PI);

                const coreVector2 vNewPos = vCenter + coreVector2::Direction(vRaw.x) * fLen;
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            aiVector[i] = vRaw.PackFloat2x16();

            pEnemy->DefaultRotate(fLifeTime * 5.0f);

            if((iType == 0u) || (iType == 1u))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                if((vPos.x < -FOREGROUND_AREA.x * 1.3f) ||
                   (vPos.x >  FOREGROUND_AREA.x * 1.3f) ||
                   (vPos.y < -FOREGROUND_AREA.y * 1.3f) ||
                   (vPos.y >  FOREGROUND_AREA.y * 1.3f))
                {
                    if(fLifeTime >= 1.0f) nReturnChildFunc(pEnemy, i, false);
                }
            }
            else if(iType == 2u)
            {
                if(fLifeTime >= 19.0f) nReturnChildFunc(pEnemy, i, true);
            }

            const coreUint8 iState = nGetState(i);

            if(iState == 2u)
            {
                if(bEffectTick) g_pSpecialEffects->CreateSplashSmoke(pEnemy->GetPosition(), 5.0f, 1u, coreVector3(1.0f,1.0f,1.0f));
            }
        });

        if(!m_iInsanity)
        {
            cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

            pBackground->SetVeilAlpha(1.0f - STEPH3(0.0f, 30.0f, m_fStageTime));

            STAGE_WAVE(3u, "3-4", {50.0f, 75.0f, 100.0f, 125.0f, 250.0f})   // SECHSZEHN
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        m_aInsanityStage[3]();
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetVeilAlpha(0.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // spikes from the ground and such
    // - effects when spawning enemies are too distracting
    // - ring-wave, or cone-shaped from center sucked (even after retrying ring-wave)
    // - (old: trying to let player move from a soon-to-be-spike to a soon-to-be-no-spike feels bad and hard to use, the time difference needs to be greatly extended which reduces pacing, a jump like in mario is missing (to create player-induced short invincibility from spikes))
    // - sawtooth pacing (fast slow fast)
    // - enemies spawn in spikes, thereby avoiding possible collisions with players
    // - enemy spawning must have a nice pattern and delay
    // - the first two spikes must spawn slowly and in the middle so that the player can see exactly what is happening
    // - the player has to see exactly where spikes will soon appear and where there is danger (color plates and expand spikes)
    // TASK: collect all the good plates
    // TASK: a single enemy with lots of health needs to be killed, but disappears after some time
    // ACHIEVEMENT: only move along the middle line until everything goes silent
    // TODO 1: hard mode: infinity movement (only X or Y always, even with possible pattern issues, though maybe some have nice transitions) (also enemies ?)
    // TODO 1: darker circles should be on the plate where the spikes come out?
    // TODO 5: badge: % time moving on purple plates
    // TODO 5: badge: touch each plate at least once (needs visual highlight)
    m_aInsanityStage[4] = [this]()
    {
        constexpr coreUintW iNumData  = 16u;
        constexpr coreUintW iChampion = 58u;

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 59u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                if(i == iChampion)
                {
                    pEnemy->Configure(400, COLOR_SHIP_CYAN);
                }
                else
                {
                    pEnemy->Configure(10, COLOR_SHIP_CYAN);
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                }

                pEnemy->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.1f);
            });
        });

        STAGE_GET_START(6u + iNumData)
            STAGE_GET_UINT       (iMiddleState)
            STAGE_GET_UINT       (iFlipCount)
            STAGE_GET_UINT       (iGoodCount)
            STAGE_GET_UINT       (iSpikeCount)
            STAGE_GET_FLOAT      (fSpikeDelay)
            STAGE_GET_FLOAT      (fEnemyDelay)
            STAGE_GET_FLOAT_ARRAY(afIntroTime, iNumData)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        const auto nToIndexFunc = [](const coreUintW iColumn, const coreUintW iRow)
        {
            ASSERT((iColumn < HARENA_SPIKE_DIMENSION) && (iRow < HARENA_SPIKE_DIMENSION))
            return iColumn + iRow * HARENA_SPIKE_DIMENSION;
        };

        const auto nToPositionFunc = [&](const coreUintW iColumn, const coreUintW iRow)
        {
            return this->GetSpikeBoard(nToIndexFunc(iColumn, iRow))->GetPosition().xy();
        };

        if(STAGE_CLEARED)
        {
            if(m_iInsanity)
            {
                     if(STAGE_SUB(10u)) {STAGE_RESURRECT(pSquad1, 42u, 57u) fEnemyDelay = 0.0f;}
                else if(STAGE_SUB(11u))
                {
                    for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                        this->DisableSpike(i, true);
                }
            }
            else
            {
                     if(STAGE_SUB( 1u)) {STAGE_RESURRECT(pSquad1,  0u,  3u) fEnemyDelay = g_pGame->IsEasy() ? 0.0f : 0.0f;}
                else if(STAGE_SUB( 2u)) {STAGE_RESURRECT(pSquad1,  4u,  9u) fEnemyDelay = g_pGame->IsEasy() ? 2.5f : 2.0f;}
                else if(STAGE_SUB( 3u)) {STAGE_RESURRECT(pSquad1, 10u, 15u) fEnemyDelay = g_pGame->IsEasy() ? 6.0f : 5.0f;}
                else if(STAGE_SUB( 4u)) {STAGE_RESURRECT(pSquad1, 16u, 23u) fEnemyDelay = g_pGame->IsEasy() ? 5.5f : 5.0f;}
                else if(STAGE_SUB( 5u)) {STAGE_RESURRECT(pSquad1, 24u, 29u) fEnemyDelay = g_pGame->IsEasy() ? 4.7f : 4.0f;}
                else if(STAGE_SUB( 6u)) {STAGE_RESURRECT(pSquad1, 58u, 58u) fEnemyDelay = g_pGame->IsEasy() ? 0.0f : 0.0f;}
                else if(STAGE_SUB( 7u)) {STAGE_RESURRECT(pSquad1, 30u, 37u) fEnemyDelay = g_pGame->IsEasy() ? 5.5f : 5.0f;}
                else if(STAGE_SUB( 8u)) {STAGE_RESURRECT(pSquad1, 38u, 41u) fEnemyDelay = g_pGame->IsEasy() ? 2.0f : 2.0f; if(!iMiddleState) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())}
                else if(STAGE_SUB( 9u))
                {
                    for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                        this->DisableSpike(i, true);
                }
            }

            iSpikeCount = 0u;
            fSpikeDelay = (m_iStageSub == 1u) ? 1.0f : 0.5f;

            std::memset(afIntroTime, 0, sizeof(coreFloat) * iNumData);
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_RED);

        if((m_iStageSub == 3u) && STAGE_SUBTIME_POINT(4.0f))
        {
            pHelper->Resurrect(false);
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            constexpr coreUintW iHelperSpike = 35u;

            const coreObject3D* pBoard = this->GetSpikeBoard(iHelperSpike);

            if(pBoard->IsEnabled(CORE_OBJECT_ENABLE_MOVE) && this->GetSpikeLaunched(iHelperSpike))
            {
                pHelper->SetPosition(pBoard->GetPosition());
            }
            else
            {
                if(g_pForeground->IsVisibleObject(pHelper)) pHelper->Kill(false);
            }
        }

        if(STAGE_BEGINNING)
        {
            if(!m_iInsanity)
            {
                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                    this->EnableSpike(i, true);
            }
        }

        const coreFloat fTickSpeed = g_pGame->IsEasy() ? 0.7f : 1.0f;
        const coreFloat fTickTime  = g_pGame->IsEasy() ? 0.9f : 1.0f;

        if(fSpikeDelay > 0.0f)
        {
            fSpikeDelay -= 1.0f * TIME;
        }
        else if(m_iStageSub == 1u)
        {
            // intro single fields
            if(STAGE_TICK_FREE(2.0f * fTickSpeed, 0.0f) && (!(s_iTick % 3u) || (iSpikeCount >= 3u)))
            {
                const coreUintW iIndex = ((iSpikeCount + 21u) * 11u) % HARENA_SPIKES;

                const coreUintW iColumn = iIndex % HARENA_SPIKE_DIMENSION;
                const coreUintW iRow    = iIndex / HARENA_SPIKE_DIMENSION;

                this->LaunchSpike(nToIndexFunc(iColumn, iRow), 2.0f * fTickTime, g_pGame->IsTask() && (iSpikeCount == 8u));

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 2u)
        {
            // lines raining down
            if(STAGE_TICK_FREE(6.0f * fTickSpeed, 0.0f))
            {
                const coreUintW iColumn1 =  (((iSpikeCount)      / HARENA_SPIKE_DIMENSION)       % 3u);
                const coreUintW iColumn2 = ((((iSpikeCount + 3u) / HARENA_SPIKE_DIMENSION) + 2u) % 3u) + 3u;

                const coreUintW iRow1 = (iSpikeCount)      % HARENA_SPIKE_DIMENSION;
                const coreUintW iRow2 = (iSpikeCount + 3u) % HARENA_SPIKE_DIMENSION;

                                      this->LaunchSpike(nToIndexFunc(iColumn1, iRow1), 2.0f * fTickTime);
                if(iSpikeCount >= 3u) this->LaunchSpike(nToIndexFunc(iColumn2, iRow2), 2.0f * fTickTime, g_pGame->IsTask() && (iSpikeCount == 11u));

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 3u)
        {
            // left and right lines
            if(STAGE_TICK_FREE(2.0f * fTickSpeed, 0.0f))
            {
                const coreUintW iColumn = iSpikeCount % HARENA_SPIKE_DIMENSION;

                for(coreUintW i = 0u; i < HARENA_SPIKE_DIMENSION; ++i)
                {
                    const coreUintW iRealColumn = (i % 2u) ? ((HARENA_SPIKE_DIMENSION - 1u) - iColumn) : iColumn;

                    this->LaunchSpike(nToIndexFunc(iRealColumn, i), 2.0f * fTickTime, g_pGame->IsTask() && (iSpikeCount == 3u) && (i == 1u));
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 4u)
        {
            // slow quads from the bottom
            if(STAGE_TICK_FREE(1.0f * fTickSpeed, 0.0f))
            {
                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                {
                    const coreUintW iColumn = (i % HARENA_SPIKE_DIMENSION) / 2u;
                    const coreUintW iRow    = (i / HARENA_SPIKE_DIMENSION) / 2u;

                    if(((iColumn + iRow + iSpikeCount) % (HARENA_SPIKE_DIMENSION / 2u))) continue;

                    this->LaunchSpike(i, 2.0f * fTickTime, g_pGame->IsTask() && (iSpikeCount == 2u) && (i == 28u));
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 5u)
        {
            // large lines from left with hole
            if(STAGE_TICK_FREE(3.0f * fTickSpeed, 0.0f))
            {
                const coreUintW iColumn = iSpikeCount % HARENA_SPIKE_DIMENSION;

                coreUintW iSkip;
                switch((iSpikeCount / 6u) % 4u)
                {
                default: UNREACHABLE
                case 0u: iSkip = 1u; break;
                case 1u: iSkip = 3u; break;
                case 2u: iSkip = 4u; break;
                case 3u: iSkip = 2u; break;
                }

                for(coreUintW i = 0u; i < HARENA_SPIKE_DIMENSION; ++i)
                {
                    if(i == iSkip) continue;

                    this->LaunchSpike(nToIndexFunc(iColumn, i), 2.0f * fTickTime, g_pGame->IsTask() && (iSpikeCount == 15u) && (i == 2u));
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 6u)
        {
            // diagonal lines (champion)
            if(STAGE_TICK_FREE(2.0f * fTickSpeed, 0.0f))
            {
                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                {
                    if(((i * 3u) % 5u) != (iSpikeCount % 5u)) continue;

                    this->LaunchSpike(i, 2.0f * fTickTime);
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 7u)
        {
            // checkerboard
            if(STAGE_TICK_FREE(1.2f * fTickSpeed, 0.0f))
            {
                coreUintW iQuad;
                switch(iSpikeCount % 4u)
                {
                default: UNREACHABLE
                case 0u: iQuad = 0u; break;
                case 1u: iQuad = 1u; break;
                case 2u: iQuad = 3u; break;
                case 3u: iQuad = 2u; break;
                }

                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                {
                    const coreUintW iColumn = i % HARENA_SPIKE_DIMENSION;
                    const coreUintW iRow    = i / HARENA_SPIKE_DIMENSION;

                    if((HAS_BIT(iQuad, 0u) != ((iColumn % 2u) != 0u)) || (HAS_BIT(iQuad, 1u) != ((iRow % 2u) != 0u))) continue;

                    this->LaunchSpike(i, 2.0f * fTickTime, g_pGame->IsTask() && (iSpikeCount == 3u) && (i == 20u));
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 8u)
        {
            // spawn below player
        }
        else if(m_iStageSub == 10u)
        {
            const coreUintW iNewCount = pSquad1->GetNumEnemiesAlive() + ((m_fStageSubTime < 2.5f) ? 1u : 0u);

            if(iFlipCount != iNewCount)
            {
                iFlipCount = iNewCount;

                constexpr coreFloat fInfinity = 3600.0f;

                switch(iNewCount)
                {
                default: UNREACHABLE
                case 17u: this->EnableSpike( 2u, false); this->EnableSpike(30u, false);                                                                       break;
                case 16u: this->EnableSpike( 0u, false); this->EnableSpike(29u, false); this->LaunchSpike( 2u, fInfinity); this->LaunchSpike(30u, fInfinity); break;
                case 15u: this->EnableSpike(12u, false); this->EnableSpike(35u, false); this->LaunchSpike( 0u, fInfinity); this->LaunchSpike(29u, fInfinity); break;
                case 14u: this->EnableSpike( 5u, false); this->EnableSpike(33u, false); this->LaunchSpike(12u, fInfinity); this->LaunchSpike(35u, fInfinity); break;
                case 13u: this->EnableSpike(11u, false); this->EnableSpike(31u, false); this->LaunchSpike( 5u, fInfinity); this->LaunchSpike(33u, fInfinity); break;
                case 12u: this->EnableSpike( 3u, false); this->EnableSpike(18u, false); this->LaunchSpike(11u, fInfinity); this->LaunchSpike(31u, fInfinity); break;
                case 11u: this->EnableSpike( 1u, false); this->EnableSpike(17u, false); this->LaunchSpike( 3u, fInfinity); this->LaunchSpike(18u, fInfinity); break;
                case 10u: this->EnableSpike(24u, false); this->EnableSpike(34u, false); this->LaunchSpike( 1u, fInfinity); this->LaunchSpike(17u, fInfinity); break;
                case  9u: this->EnableSpike(10u, false); this->EnableSpike(23u, false); this->LaunchSpike(24u, fInfinity); this->LaunchSpike(34u, fInfinity); break;
                case  8u: this->EnableSpike( 6u, false); this->EnableSpike(25u, false); this->LaunchSpike(10u, fInfinity); this->LaunchSpike(23u, fInfinity); break;
                case  7u: this->EnableSpike(28u, false); this->EnableSpike(32u, false); this->LaunchSpike( 6u, fInfinity); this->LaunchSpike(25u, fInfinity); break;
                case  6u: this->EnableSpike( 4u, false); this->EnableSpike( 7u, false); this->LaunchSpike(28u, fInfinity); this->LaunchSpike(32u, fInfinity); break;
                case  5u: this->EnableSpike(13u, false); this->EnableSpike(27u, false); this->LaunchSpike( 4u, fInfinity); this->LaunchSpike( 7u, fInfinity); break;
                case  4u: this->EnableSpike( 8u, false); this->EnableSpike(22u, false); this->LaunchSpike(13u, fInfinity); this->LaunchSpike(27u, fInfinity); break;
                case  3u: this->EnableSpike(16u, false); this->EnableSpike(26u, false); this->LaunchSpike( 8u, fInfinity); this->LaunchSpike(22u, fInfinity); break;
                case  2u: this->EnableSpike( 9u, false); this->EnableSpike(19u, false); this->LaunchSpike(16u, fInfinity); this->LaunchSpike(26u, fInfinity); break;
                case  1u:                                                               this->LaunchSpike( 9u, fInfinity); this->LaunchSpike(19u, fInfinity); break;
                case  0u:                                                                                                                                     break;
                }
            }
        }

        if(m_iStageSub < 11u)
        {
            for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
            {
                const coreObject3D* pBoard = this->GetSpikeBoard(i);
                if(!pBoard->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreBool bIsActive = this->GetSpikeLaunched(i);
                const coreBool bIsQuiet  = this->GetSpikeQuiet   (i) && (m_iStageSub == 8u);
                const coreBool bIsGood   = this->GetSpikeGood    (i);

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pPlayer->GetPosition().xy() - pBoard->GetPosition().xy();

                    if((ABS(vDiff.x) < pBoard->GetCollisionRange().x) &&
                       (ABS(vDiff.y) < pBoard->GetCollisionRange().y))
                    {
                        if(bIsGood)
                        {
                            this->RetractSpike(i);

                            if(++iGoodCount == 6u)
                            {
                                STAGE_BADGE(0u, BADGE_EASY, pBoard->GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawProgress(iGoodCount, 6u, pBoard->GetPosition());
                                g_pSpecialEffects->PlaySound(pBoard->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iGoodCount, 6u), SOUND_ITEM_01);
                            }
                        }
                        else
                        {
                            if(bIsActive &&  pPlayer->IsNormal ()) pPlayer->TakeDamage(10, ELEMENT_NEUTRAL, pPlayer->GetPosition().xy());
                            if(bIsQuiet  && !pPlayer->IsRolling()) this->LaunchSpike(i, 8.0f * RCP(I_TO_F(g_pGame->GetNumPlayers())) * (g_pGame->IsEasy() ? 0.7f : 1.0f));
                        }
                    }
                });

                if(bIsActive && (m_fStageSubTime >= fEnemyDelay))
                {
                    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, j)
                    {
                        if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST)) return;

                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - pBoard->GetPosition().xy();

                        if(vDiff.LengthSq() < POW2(pBoard->GetCollisionRange().x))
                        {
                            pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.2f, SOUND_EFFECT_SWIPE_01);
                        }
                    });
                }
            }
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            if(!coreMath::IsNear(pPlayer->GetPosition().x, 0.0f))
            {
                iMiddleState = true;
                STAGE_FAILTROPHY
            }
        });

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreUintW iColumn, iRow;
            switch(i)
            {
            default: UNREACHABLE
            case  0u: iColumn = 0u; iRow = 2u; break;
            case  1u: iColumn = 4u; iRow = 5u; break;
            case  2u: iColumn = 3u; iRow = 1u; break;
            case  3u: iColumn = 2u; iRow = 3u; break;

            case  4u: iColumn = 0u; iRow = 5u; break;
            case  5u: iColumn = 1u; iRow = 0u; break;
            case  6u: iColumn = 2u; iRow = 5u; break;
            case  7u: iColumn = 3u; iRow = 0u; break;
            case  8u: iColumn = 4u; iRow = 5u; break;
            case  9u: iColumn = 5u; iRow = 0u; break;

            case 10u: iColumn = 5u; iRow = 0u; break;
            case 11u: iColumn = 5u; iRow = 1u; break;
            case 12u: iColumn = 5u; iRow = 2u; break;
            case 13u: iColumn = 0u; iRow = 3u; break;
            case 14u: iColumn = 0u; iRow = 4u; break;
            case 15u: iColumn = 0u; iRow = 5u; break;

            case 16u: iColumn = 0u; iRow = 1u; break;
            case 17u: iColumn = 0u; iRow = 3u; break;
            case 18u: iColumn = 5u; iRow = 2u; break;
            case 19u: iColumn = 5u; iRow = 4u; break;
            case 20u: iColumn = 2u; iRow = 0u; break;
            case 21u: iColumn = 4u; iRow = 0u; break;
            case 22u: iColumn = 1u; iRow = 5u; break;
            case 23u: iColumn = 3u; iRow = 5u; break;

            case 24u: iColumn = 0u; iRow = 1u; break;
            case 25u: iColumn = 2u; iRow = 1u; break;
            case 26u: iColumn = 4u; iRow = 1u; break;
            case 27u: iColumn = 1u; iRow = 3u; break;
            case 28u: iColumn = 3u; iRow = 3u; break;
            case 29u: iColumn = 5u; iRow = 3u; break;

            case 30u: iColumn = 1u; iRow = 0u; break;
            case 31u: iColumn = 5u; iRow = 0u; break;
            case 32u: iColumn = 1u; iRow = 4u; break;
            case 33u: iColumn = 5u; iRow = 4u; break;
            case 34u: iColumn = 0u; iRow = 1u; break;
            case 35u: iColumn = 4u; iRow = 1u; break;
            case 36u: iColumn = 0u; iRow = 5u; break;
            case 37u: iColumn = 4u; iRow = 5u; break;

            case 38u: iColumn = 5u; iRow = 0u; break;
            case 39u: iColumn = 1u; iRow = 1u; break;
            case 40u: iColumn = 4u; iRow = 3u; break;
            case 41u: iColumn = 2u; iRow = 5u; break;

            case 42u: iColumn = 1u; iRow = 0u; break;
            case 43u: iColumn = 2u; iRow = 0u; break;
            case 44u: iColumn = 4u; iRow = 0u; break;
            case 45u: iColumn = 0u; iRow = 1u; break;
            case 46u: iColumn = 3u; iRow = 1u; break;
            case 47u: iColumn = 5u; iRow = 1u; break;
            case 48u: iColumn = 0u; iRow = 2u; break;
            case 49u: iColumn = 1u; iRow = 2u; break;
            case 50u: iColumn = 0u; iRow = 3u; break;
            case 51u: iColumn = 4u; iRow = 3u; break;
            case 52u: iColumn = 5u; iRow = 3u; break;
            case 53u: iColumn = 2u; iRow = 4u; break;
            case 54u: iColumn = 5u; iRow = 4u; break;
            case 55u: iColumn = 2u; iRow = 5u; break;
            case 56u: iColumn = 3u; iRow = 5u; break;
            case 57u: iColumn = 4u; iRow = 5u; break;

            case 58u: iColumn = 0u; iRow = 0u; ASSERT(i == iChampion) break;
            }

            if(i == iChampion)
            {
                const coreFloat   fTime = pEnemy->GetLifeTime() * 0.1f;
                const coreVector2 vPos  = LERP(coreVector2(-1.0f,1.0f), coreVector2(1.0f,-1.0f), fTime) * 1.45f * FOREGROUND_AREA;

                pEnemy->SetPosition(coreVector3(vPos, 0.0f));

                if(g_pGame->IsTask() && pEnemy->ReachedDeath())
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                }

                if(fTime >= 1.0f)
                {
                    pEnemy->Kill(false);
                    bPostpone = true;
                }
            }
            else
            {
                pEnemy->SetPosition(coreVector3(nToPositionFunc(iColumn, iRow), 0.0f));
            }

            if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
            {
                pEnemy->SetSize(coreVector3(0.0f,0.0f,0.0f));
            }
            else
            {
                coreFloat& fIntroTime = STAGE_SINK_FLOAT(afIntroTime[i % iNumData]);

                fIntroTime += 3.0f * TIME;

                const coreFloat fValue = BLENDB(MIN1(fIntroTime));
                const coreFloat fSpin  = m_fStageTime * 3.0f + fValue;

                pEnemy->SetSize      (coreVector3(1.0f,1.0f,1.0f) * ((i == iChampion) ? 1.8f : 1.3f) * fValue);
                pEnemy->DefaultRotate((0.5f*PI) * fSpin);
            }

            this->CrashEnemy(pEnemy);
        });

        if(!m_iInsanity)
        {
            cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

            pBackground->SetGroundDensity(0u, 1.0f - STEPH3(0.0f, 30.0f, m_fStageTime));
            pBackground->SetGroundDensity(1u, 1.0f - STEPH3(0.0f, 30.0f, m_fStageTime));

            if(STAGE_BEGINNING)
            {
                g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,1.0f), 30.0f);
            }

            if(!bPostpone)
            {
                if(g_pGame->IsEasy()) STAGE_WAVE(4u, "3-5", {75.0f, 110.0f, 150.0f, 185.0f, 370.0f})   // SIEBZEHN
                                 else STAGE_WAVE(4u, "3-5", {65.0f,  95.0f, 130.0f, 160.0f, 320.0f})
            }
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        m_aInsanityStage[4]();
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
            this->DisableSpike(i, false);

        STAGE_FINISH_NOW
    });

#if defined(_P1_UNUSED_)

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // can only kill in order
    // - coop last enemy, need to work together
    // - alternating left and right
    // - double group only wide pattern to make it easier to dodge
    // - order furthest apart
    // TODO 1: show 3142 group at start, matrix of enemies
    // TODO 1: simon says, linear up and down, to make distinction easy, and to prevent accidental hits (order forward, backward, mirrored, to stay fair but variable)
    // TODO 1: static assertion, none of the chief orders may exceed the enemy count
    // TODO 1: MAIN: task-check, helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        UNUSED STAGE_ADD_PATH(pPath1)          
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.3f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2(-0.9f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 69u)                                    
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                //if(i == 8u) pEnemy->SetSize(coreVector3(1.3f,1.3f,1.3f));

                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                //pEnemy->Configure((i == 8u) ? 200 : 50, COLOR_SHIP_GREY);
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT (iChiefNum)
            STAGE_GET_FLOAT(fShotOffset)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                  if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 8u)
             //else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 16u, 31u)
        }

        const auto nCreateChiefFunc = [&]()
        {
            //constexpr coreUintW aiChiefOrder[] = {5u, 3u, 4u, 2u, 7u, 0u, 6u, 1u, 8u};
            constexpr coreUintW aiChiefOrder[] = {6u, 3u, 0u, 7u, 4u, 8u, 1u, 5u, 2u};

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
            STAGE_LIFETIME(pEnemy, 0.8f + 0.4f * I_TO_F(i / 3u), (2.6f/3.0f) * I_TO_F(i % 3u))

            STAGE_REPEAT(pPath2->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(I_TO_F(i / 3u) * -0.3f - 0.1f, 0.0f);

            pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

            pEnemy->Rotate90();
            if((i / 3u) % 2u) pEnemy->InvertX();

            /*
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
            else 
            */
            if(pEnemy->ReachedDeath())
            {
                if(!pSquad1->IsFinished())
                {
                    nCreateChiefFunc();
                }
            }

            if(!pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE))
            {
                if(STAGE_TICK_TIME((iChiefNum == 9u) ? 3.0f : 1.1f, fShotOffset))   // TODO 1: STAGE_TICK_TIME2 instead ?
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();
                    //const coreUintW   iType = 4u - ((iChiefNum == 9u) ? (s_iTick % 3u) : (((iChiefNum - 1u) / 2u) % 3u));
                    const coreUintW   iType = 4u;

                    for(coreUintW j = 40u; j--; )
                    {
                        if(((j / iType) + (s_iTick & 0x01u)) & 0x01u) continue;

                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.2f, pEnemy, vPos,  vDir)->ChangeSize(1.15f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.2f, pEnemy, vPos, -vDir)->ChangeSize(1.15f);
                    }
                }
            }
        });

        STAGE_WAVE(5u, "3-?", {60.0f, 80.0f, 100.0f, 120.0f, 240.0f})   // ACHTZEHN
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMeteorEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cConeBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cWaveBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTriangleBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cFlipBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cViewBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pGame->KillHelpers();

        STAGE_FINISH_NOW
    });

#endif

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetGroundDensity(0u, 0.0f);
        pBackground->SetGroundDensity(1u, 0.0f);

        g_pEnvironment->SetTargetDirectionNow(coreVector2(0.0f,1.0f));

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
        STAGE_BOSS(m_Tiger, {145.0f, 215.0f, 290.0, 360.0f, 720.0f})   // +5
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cRocketBullet>();
        g_pGame->PrefetchBoss();
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
    // story state
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(m_bStory)
        {
            m_iOutroSub = 11u;
        }

        ADD_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_HARENA)
        STAGE_FINISH_NOW
    });

    // ################################################################
    // ################################################################
}