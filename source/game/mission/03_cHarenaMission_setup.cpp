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
        g_pEnvironment->SetTargetSpeedNow(6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
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
    // ghost appears for few seconds (zelda wizzrobe)
    // keine einzelnen gegner am anfang, sonst sieht der spieler die mechanik nicht, wenn er sie einfach über den haufen schießt
    // 1: two groups active at the same time to continue with changes while keeping enemies active for a longer time
    // 1: two different invincible states, hidden and flickering, to give the player perception of change while he has to wait (loadbar)
    // (old) 3: pattern should not have too long straight lines, as this makes it too easy, and should have no corners, as this creates placement artifacts
    // 1: diamond shape to not be able to kill 2 groups at the same time
    // stuttering side-movement needs to be only in one direction, otherwise player does not recognize the move pattern
    // stuttering side-movement needs a safe-space (at the bottom) because being in the middle of them is hard to evade, as it's already hard to hit
    // player should already look in the right direction at the start of the threshold wave, the previous wave is doing that with the movement-direction of the enemies
    // bei order-wave sollten keine 3 hintereinander von ein-und-derselben position angreifbar sein (2 ist ok, weil das logisch ist), wenn möglich keine 2 felder bewegen zwischen 2 angriffen und drehung nur um 90 grad
    // bei threshold wave sollte der threshold verschoben sein, damit spieler nicht mit seitlichem angriff trifft, und gegner geschosse schräg fliegen (schöner)
    // memory (unsichtbare gegner die man treffen kann) hat überhaupt nicht funktioniert, hat zu wenig tension erzeugt, und war zu leicht (einfach nieder-mähen), aber gut für badge
    // TODO 1: sandsturm
    // TODO 1: 1 or N hidden enemies as badge
    // TODO 1: maybe add more meaning to new second wave (enemies top and bottom)
    // TODO 1: analysieren ob order-wave verbessert werden muss (e.g. schräge bewegungen am ende erlauben ? könnte gschissener, aber schwerer sein (gut gegen ende))
    // TODO 1: change pattern of stutter wave ?
    // TODO 1: adjust aim function nAimFunc, it was not updated after production-phase
    // TODO 1: check the order of the sub-wave, I'm still not happy, especially lines in 2 and 3 are too similar (there were some changes, now 2 is similar to the boss waves)
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: badge: manchmal sind die gegner anders wenn sie sichtbar werden, 1/N abschießen
    // TODO 1: hardmode: bad visibility (sand storm) in einer der missionen hier, muss vielleicht sinus-förmig (oder anders) ganz verschwinden, auch geschosse wegen pressure
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

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 154u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        constexpr coreUintW  iRange      = sizeof(coreUint32) * 8u;
        constexpr coreUint32 iSnakeCount = 6u;

        STAGE_GET_START(7u + iSnakeCount)
            STAGE_GET_UINT      (iStateActive)
            STAGE_GET_UINT      (iStateGhost)
            STAGE_GET_FLOAT     (fChangeTime)
            STAGE_GET_UINT      (iChangeCount)
            STAGE_GET_UINT_ARRAY(aiSnakeIndex, iSnakeCount)
            STAGE_GET_UINT      (iSnakeShift,  iSnakeShift = iSnakeCount)
            STAGE_GET_FLOAT     (fLastValue)
            STAGE_GET_UINT      (iLastDual)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
            if(m_iInsanity)
            {
                     if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 120u, 121u)
                else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 122u, 137u)
                else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 138u, 153u)
                else if(STAGE_SUB(12u)) {}
            }
            else
            {
                     if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,   0u,  15u)
                else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  16u,  31u)
                else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  32u,  47u)
                else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  48u,  55u)
                else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  56u,  71u)
                else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  72u,  77u)
                else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  78u,  89u)
                else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  90u, 119u)
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
                default: ASSERT(false)
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
                default: ASSERT(false)
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
                default: ASSERT(false)
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
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b11111111'00000000u, 56u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b00000000'11111111u, 56u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: ASSERT(false)
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }
            }
        }
        else if(m_iStageSub == 6u)
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
        else if(m_iStageSub == 7u)
        {
            iChangeCount = 1u - iChangeCount;

            if(fChangeTime >= (1.0f/0.75f))
            {
                iStateGhost = 0xFFFFFFFFu;
            }

            if(fChangeTime >= (2.0f/0.75f))
            {
                constexpr coreUintW iFirst = 78u;

                switch(pSquad1->GetNumEnemiesAlive())
                {
                default: ASSERT(false)
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
        else if(m_iStageSub == 8u)
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

                        for(coreUintW i = 90u; i < 120u; ++i)
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
        }
        else if(m_iStageSub == 9u)
        {
            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;
                iStateGhost   = 0xFFFFFFFFu;
            }
        }
        else if(m_iStageSub == 10u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b10101010'10101010u, 122u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b01010101'01010101u, 122u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: ASSERT(false)
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }
            }
        }
        else if(m_iStageSub == 11u)
        {
            constexpr coreUint32 iGroupA = coreMath::RotateLeft32(0b11111111'00000000u, 138u % iRange);
            constexpr coreUint32 iGroupB = coreMath::RotateLeft32(0b00000000'11111111u, 138u % iRange);

            if(FRACT(fChangeTime * 0.75f) < FRACT(fChangeTimePrev * 0.75f))
            {
                iChangeCount += 1u;

                iStateActive |= iStateGhost;

                switch(iChangeCount % 2u)
                {
                default: ASSERT(false)
                case 0u: iStateActive &= ~iGroupA; iStateGhost = iGroupA; break;
                case 1u: iStateActive &= ~iGroupB; iStateGhost = iGroupB; break;
                }
            }
        }

        const auto nAimFunc = [](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            pEnemy->SetDirection(coreVector3(((i < 32u) ? pEnemy->AimAtPlayerDual((i / 8u) % 2u) : pEnemy->AimAtPlayerSide()).Normalized(), 0.0f));
        };

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const coreVector3 vSaveDir = pEnemy->GetDirection();   // movement patterns should not affect direction and aim

            if(i < 56u)
            {
                // no path movement
            }
            else if(i < 72u)
            {
                STAGE_LIFETIME(pEnemy, 0.8f, 0.7f * I_TO_F((i - 56u) / 4u))

                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F((i - 56u) % 4u) * 0.2f - 0.9f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->InvertY();
                if(((i - 56u) / 4u) % 2u) pEnemy->Rotate180();
            }
            else if(i < 78u)
            {
                STAGE_LIFETIME(pEnemy, 1.1f, 1.3f * I_TO_F((i - 72u) % 2u))

                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i - 72u) - 2.5f) * 0.3f, 0.0f);

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
                else if(i < 78u)
                {
                    // no point movement
                }
                else if(i < 90u)
                {
                    const coreVector2 vPos = coreVector2((I_TO_F((i - 78u) % 3u) - 1.0f) * 0.3f, 0.8f);

                    pEnemy->SetPosition(coreVector3(MapStepRotated90(vPos * FOREGROUND_AREA, (i - 78u) / 3u), 0.0f));
                }
                else if(i < 120u)
                {
                    if(aiSnakeIndex[0] == i)
                    {
                        const coreFloat   fOffset = (1.0f/5.0f) * I_TO_F(iSnakeCount - iSnakeShift);
                        const coreVector2 vPos    = pPath1->CalcPosition(FMOD(fChangeTime - fOffset, pPath1->GetTotalDistance()));

                        pEnemy->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                    }
                }
                else if(i < 122u)
                {
                    const coreVector2 vPos = coreVector2(((i - 120u) % 2u) ? -0.6f : 0.6f, 0.6f);

                    pEnemy->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }
                else if(i < 138u)
                {
                    const coreVector2 vPos = coreVector2(((i - 122u) % 2u) ? 0.8f : 0.9f, (I_TO_F((i - 122u) % 8u) - 3.5f) * 0.27f);

                    pEnemy->SetPosition(coreVector3(-vPos * FOREGROUND_AREA, 0.0f));

                         if(i < 130u) {}
                    else if(i < 138u) pEnemy->Rotate180();
                }
                else if(i < 154u)
                {
                    const coreVector2 vPos = coreVector2(((i - 138u) % 2u) ? 0.8f : 0.9f, (I_TO_F((i - 138u) % 8u) - 3.5f) * 0.27f);

                    pEnemy->SetPosition(coreVector3(-vPos.Rotated90() * FOREGROUND_AREA, 0.0f));

                         if(i < 146u) {}
                    else if(i < 154u) pEnemy->Rotate180();
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

                        if(g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()) && !m_iInsanity)
                        {
                            nAimFunc(pEnemy, i);

                            const coreVector2 vPos = pEnemy->GetPosition ().xy();
                            const coreVector2 vDir = pEnemy->GetDirection().xy();
                            const coreVector2 vTan = vDir.Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.25f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.25f);
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

            if((m_iStageSub == 8u) && (pSquad1->GetNumEnemiesAlive() == 1u))
            {
                if(!fLastValue) iLastDual = pEnemy->NearestPlayerSide() - g_pGame->GetPlayer(0u);

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

                if(!g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()))
                {
                    STAGE_BADGE(0u, BADGE_NORMAL, pEnemy->GetPosition())
                    pEnemy->Kill(true);
                }
            }

            this->CrashEnemy(pEnemy);
        });

        if(!m_iInsanity) STAGE_WAVE(0u, "DREIZEHN", {60.0f, 80.0f, 100.0f, 120.0f})
    };
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        m_aInsanityStage[0]();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

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
    // smoke can distract from bullets
    // non-linear movement is harder to track
    // shooting on "hide" instead of "show" is too overwhelming, identifying both initial bullet-direction and movement at the same time
    // (deprecated) 8 pattern is too overwhelming (order: 0, 3, 5, 2, 6, 1, 7, 4) (order: 0, 3, 6, 2, 7, 4, 1, 5)
    // during rotation, if move pattern moves against rotation the enemy stays in the air, which looks weird
    // hiding current enemy movement did not add any value, especially on rotating 8 enemies it is just too overwhelming (maybe for hard mode ? might be too light, also change pattern or order ?)
    // first enemy flies from below maybe through player, to show there is no harm
    // TODO 1: badge, richte großen schaden auf ein (einzelnes) non-ziel an
    // TODO 1: add effect (visual + sound) if the correct target was hit (also if the wrong was hit ? sparkle/selection + düd-düüd)
    // TODO 1: ganz letzter gegner verfolgt einem länger und direkt
    // TODO 1: sandsturm
    // TODO 1: ein schuss wurde vom (aktiven) gegner "neutralisiert" in dem moment wo er sich wieder wegbewegt hat (ohne zu zählen), vielleicht ein timing-issue
    // TODO 1: fix short movements by enemies when transitioning 4->6 and 6->8 (und intro für new enemies)
    // TODO 1: gegner die eng zusammen gehen
    // TODO 1: badge: einer der gegner bekommt ne komplett andere farbe (bevorzugt bei 6, e.g. wenn blau oben links ist, ist rosa unten rechts, damit der spieler es nicht sofort sieht) (vielleicht 3 gegner, mit counter label 1/3)
    // TODO 1: adjust final phase, maybe not targeting player, maybe enemies have actual health, be aware that enemies need to be deterministic (which might be in the current phase, enemies move into rota direction based on player position)
    // TODO 1: add extra score for hits
    // TODO 1: check if bullet-collision really affects the whole shooting area, to never cause any frame-delay when getting visible
    // TODO 1: finale phase ist vielleicht etwas zu schnell (oder steigt zu schnell an)
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: big can change, like in tiger boss, just (iBig << 1u), maybe not on the first 1-2 hits, maybe only for hard
    // TODO 1: coop: jeder muss einem anderen gegner folgen (4+4 bei 8er gruppe), oder ein gegner muss innerhalb des zeitlimits von beiden erwischt werden!
    m_aInsanityStage[1] = [this]()
    {
        constexpr coreUintW iNumEnemies = 8u;

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(1000, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(10u + 4u * iNumEnemies)
            STAGE_GET_UINT      (iVisible)
            STAGE_GET_UINT      (iBig)
            STAGE_GET_UINT      (iHit)
            STAGE_GET_UINT      (iPoints)
            STAGE_GET_UINT      (iIteration)
            STAGE_GET_UINT      (iSingle)
            STAGE_GET_FLOAT     (fRotaSpeed)
            STAGE_GET_FLOAT     (fRotaValue)
            STAGE_GET_FLOAT     (fTime,  fTime  = 1.0f;)
            STAGE_GET_FLOAT     (fDelay, fDelay = 1.0f;)
            STAGE_GET_VEC2_ARRAY(avPosFrom, iNumEnemies)
            STAGE_GET_VEC2_ARRAY(avPosTo,   iNumEnemies)
        STAGE_GET_END

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
        }

        const auto nAttackFunc = [&](cEnemy* pEnemy)
        {
            const coreVector2 vPos = pEnemy->GetPosition().xy();
            const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.6f, pEnemy, vPos, vDir)->ChangeSize(1.5f);
        };

        const auto nPunishFunc = [&](cEnemy* pEnemy)
        {
            const coreVector2 vPos = pEnemy->GetPosition().xy();

            for(coreUintW j = 20u; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 18.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.6f, pEnemy, vPos,  vDir)->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.6f, pEnemy, vPos, -vDir)->ChangeSize(1.5f);
            }
        };

        const coreUint32 iPrevVisible = iVisible;
        const coreUint32 iPrevBig     = iBig;

        const coreFloat fSpeed = 1.0f + (m_iInsanity ? 0.0f : (0.8f * (I_TO_F(iNumEnemies - pSquad1->GetNumEnemiesAlive()) / I_TO_F(iNumEnemies - 1u))));

        fTime = MIN(fTime + 0.6f * fSpeed * TIME, 1.0f);
        if(fTime >= 1.0f)
        {
            iVisible = (iPoints < 23u) ? BITLINE(iNumEnemies) : BIT(iSingle);

            fDelay = MIN(fDelay + 0.9f * fSpeed * TIME, 1.0f);
            if((fDelay >= 1.0f) || iHit)
            {
                iVisible = 0u;
                iHit     = 0u;
                fTime    = 0.0f;
                fDelay   = 0.0f;

                iIteration += 1u;

                if(iPoints == 0u)
                {
                    iBig = 1u;
                }
                else if(iPoints == 15u)
                {
                    iBig = BITLINE(iNumEnemies);
                }
                else if(iPoints >= 23u)
                {
                    for(coreUintW j = 0u; j < iNumEnemies; ++j)
                    {
                        if(++iSingle >= iNumEnemies) iSingle = 0u;
                        if(!pSquad1->GetEnemy(iSingle)->HasStatus(ENEMY_STATUS_DEAD)) break;
                    }

                    iBig = BIT(iSingle);
                }

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    const coreUintW iIndex = i + iIteration;

                    avPosFrom[i] = avPosTo[i];

                    if(m_iInsanity)
                    {
                        ASSERT(iPoints < 11u)

                        if(iPoints < 6u)
                        {
                            coreUintW iStep;
                            switch(iIndex % 6u)
                            {
                            default: ASSERT(false)
                            case 0u: iStep = 0u; break;
                            case 1u: iStep = 2u; break;
                            case 2u: iStep = 4u; break;
                            case 3u: iStep = 1u; break;
                            case 4u: iStep = 5u; break;
                            case 5u: iStep = 3u; break;
                            }
                            if(i < 6u) avPosTo[i] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(iStep) * 60.0f + 90.0f + CORE_MATH_PRECISION)) * 0.95f;
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
                            default: ASSERT(false)
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
                            default: ASSERT(false)
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
                            default: ASSERT(false)
                            case 0u: iStep = 0u; break;
                            case 1u: iStep = 5u; break;
                            case 2u: iStep = 2u; break;
                            case 3u: iStep = 7u; break;
                            case 4u: iStep = 4u; break;
                            case 5u: iStep = 1u; break;
                            case 6u: iStep = 6u; break;
                            case 7u: iStep = 3u; break;
                            }
                            avPosTo[i] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(iStep) * 45.0f + 22.5f + CORE_MATH_PRECISION)) * 0.95f;
                        }
                        else
                        {
                            if(i == iSingle) avPosTo[i] = pEnemy->NearestPlayerDual(iIteration % 2u)->GetPosition().xy() / FOREGROUND_AREA;
                        }
                    }

                    if(avPosFrom[i] == HIDDEN_POS) avPosFrom[i] = avPosTo[i].Processed(SIGN) * 1.2f;
                });
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

            if(HAS_BIT(iVisible, i) && pEnemy->WasDamaged())
            {
                if(!iHit)
                {
                    if(!HAS_BIT(iBig, i))
                    {
                        nPunishFunc(pEnemy);
                    }
                    else
                    {
                        iPoints += 1u;

                        if(iPoints > 15u)
                        {
                            REMOVE_BIT(iBig, i)
                            if(!iBig) pEnemy->Kill(true);
                        }

                        if(iPoints == 23u)
                        {
                            iSingle = i;

                            for(coreUintW j = 0u; j < iNumEnemies; ++j)
                            {
                                avPosFrom[j] *= mRotaAbs;
                                avPosTo  [j] *= mRotaAbs;
                            }
                        }

                        if(m_iInsanity)
                        {
                            iBig |= iBig << 1u;
                            pEnemy->Kill(true);
                        }

                        g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_BLUE);
                    }
                }

                ADD_BIT(iHit, i)
            }

            if(!HAS_BIT(iVisible, i) && STAGE_TICK_FREE(40.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
            {
                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 0.0f, 1u, HAS_BIT(iBig, i) ? COLOR_ENERGY_BLUE : COLOR_ENERGY_WHITE);
            }

            if(HAS_BIT(iVisible, i) && !HAS_BIT(iPrevVisible, i))
            {
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                if(g_pForeground->IsVisibleObject(pEnemy) && !m_iInsanity)
                {
                    if(iPoints >= 23u) nPunishFunc(pEnemy);
                                  else nAttackFunc(pEnemy);
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

            this->CrashEnemy(pEnemy);
        });

        m_iStageSub = MAX(m_iStageSub, iPoints + 1u);

        if(!m_iInsanity)
        {
            cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

            pBackground->SetVeilAlpha(STEPH3(0.0f, 30.0f, m_fStageTime));

            STAGE_WAVE(1u, "VIERZEHN", {60.0f, 80.0f, 100.0f, 120.0f})
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        m_aInsanityStage[1]();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetVeilAlpha(1.0f);

        pBackground->GetOutdoor()->LerpHeightNow(1.0f, 0.0f);
        pBackground->SetGroundDensity(0u, 1.0f);
        pBackground->SetGroundDensity(1u, 1.0f);
        pBackground->SetGroundDensity(2u, 1.0f);
        pBackground->SetGroundDensity(3u, 1.0f);
        pBackground->SetGroundDensity(5u, 0.0f);
        pBackground->SetTrail(false);

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
    // should not create bullets, to focus on the movement and tower (bullets are too distracting) (except for final enemy)
    // should be fast enough to be a thread, but needs visual space to properly identify movement
    // complete vertical or horizontal movement is too easy
    // don't spawn too far on the side to properly blend them in, but not too near center, to highlight the tower structure
    // try to stack enemies all facing into the same direction, otherwise it looks not as fascinating
    // [deprecated] 4: stacking only two gives a lot of safe ways to kill them when they try to hit you
    // 1: show tower and flight mechanic
    // 2: show infinite displaced tower
    // 1: shoot away from second tower, to give spacial distance, and highlight the displacement mechanic
    // 3, 4, 5: left around, right around, left around
    // last 2: top right, top left
    // it is important that the flight start-direction can be properly identified
    // moving enemies need to look into their fly-direction, to not cause teleportation when hitting them outside the view (though it's more likely they are hit from where they come and not from where they go)
    // TODO 1: (might be fixed already) check for and fix shadow artifacts, when jumping behind near clipping plane of shadow viewport (maybe fade out near plane)
    // TODO 1: improve effect to highlight bottom enemies
    // TODO 1: moving tower sollte nicht in seine (exakte) flug-richtung schießen
    // TODO 1: maybe make top enemies more transparent, to not distract too much (especially for the moving enemies, or maybe make the moving enemies 3 + 3 to group them visually), either 100-0% from bottom to top, or N stages
    // TODO 1: player can kill a lot of sub-groups by staying at the bottom (targeting, moving), maybe change position of those enemies (further to the border, but the should still be visible coming from above)
    // TODO 1: helper comes out of last "moving" enemy and flies straight out
    // TODO 1: bottom resizing should also be interpolated
    // TODO 1: add better indicator for fly start-direction (arrow inside enemy, can be same color as attack, big on big enemy)
    // TODO 1: somehow add assertion for iNumData
    // TODO 1: vielleicht bekommen angreifende gegner den energy-shader, oder den damaging-effekt eher so aussehen lassen (eigener shader wie outline)
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: shadow needs to be adjusted by size (e.g. for final enemy)
    // TODO 1: I think the shadow can be under every enemy (blend-in)
    // TODO 1: die anvisierenden gegner könnten sich vom spieler weg-drehen, is weniger nervig
    // TODO 1: eine der star bewegenden gruppen könnte sich eher gegen flugrichtung weg-werfen
    // TODO 1: bewegende in der luft sind schwer von damaging gegnern zu unterscheiden (besonders die einzelnen später), vielleicht geschwindigkeit reduzieren
    // TODO 1: coop: abwechselnd gegner treffen
    // TODO 1: keine kollision mit spieler im turm, nur bullets
    // TODO 1: in der oben-unten aufgeteilten phase kann ich gleich alle in der mittel-linie spawnen lassen, spieler würd eh sonst ganz unten stehn
    // TODO 1: helper is the first bullet of final enemy
    m_aInsanityStage[2] = [this]()
    {
        constexpr coreUintW iNumEnemies = 61u;
        constexpr coreUintW iNumData    = 16u;   // max number of concurrent active enemies (beware of mover-enemy being active longer)

        constexpr coreUintW iEnablerIndex = 52u;
        constexpr coreUintW iMoverIndex   = 59u; STATIC_ASSERT(iMoverIndex == iNumEnemies - 2u)
        constexpr coreUintW iBigIndex     = 60u; STATIC_ASSERT(iBigIndex   == iNumEnemies - 1u)

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                const coreBool bEnabler = (i == iEnablerIndex);
                const coreBool bMover   = (i == iMoverIndex);
                const coreBool bBig     = (i == iBigIndex);

                pEnemy->SetAlpha (0.0f);
                pEnemy->Configure((bEnabler || bMover) ? 1 : 400, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                if(bEnabler || bMover || bBig) pEnemy->RemoveStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(iNumEnemies * 2u + iNumData * 4u + 2u)
            STAGE_GET_FLOAT_ARRAY(afFall,   iNumEnemies)
            STAGE_GET_FLOAT_ARRAY(afHeight, iNumEnemies)
            STAGE_GET_VEC2_ARRAY (avMove,   iNumData)
            STAGE_GET_VEC2_ARRAY (avRotate, iNumData)
            STAGE_GET_UINT       (iMoverActive)
            STAGE_GET_UINT       (iPoints)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
            if(STAGE_SUB(1u))
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
                {
                    if(!m_iInsanity || (i < 16u))
                        pEnemy->Resurrect();
                });
            }
        }

        for(coreUintW i = 0u; i < HARENA_FLOORS; ++i)
        {
            if(m_apFloorOwner[i] && m_apFloorOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableFloor(i, true);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2& vMove   = avMove  [i % iNumData];
            coreVector2& vRotate = avRotate[i % iNumData];

            const coreBool bEnabler = (i == iEnablerIndex);
            const coreBool bMover   = (i == iMoverIndex);
            const coreBool bBig     = (i == iBigIndex);

            if(STAGE_TAKEOFF)
            {
                afHeight[i] = 60.0f + 10.0f * I_TO_F(i);
            }

            const coreFloat fGround = (((i == 0u) || pSquad1->GetEnemy(i-1u)->GetLostHealth()) ? 0.0f : (pSquad1->GetEnemy(i-1u)->GetPosition().z + 9.0f));

            afFall  [i] += -120.0f   * TIME;
            afHeight[i] += afFall[i] * TIME;

            if(afHeight[i] < fGround)
            {
                afFall  [i] = MAX(afFall[i], 0.0f);
                afHeight[i] = fGround;
            }

            const coreBool bFirstDamage = (pEnemy->ReachedHealth(pEnemy->GetMaxHealth() - 1));
            const coreBool bAnyDamage   = (pEnemy->GetCurHealth() != pEnemy->GetMaxHealth());

            if(bFirstDamage || (bMover && (iMoverActive == 1u)))
            {
                vMove     = pEnemy->GetDirection().xy();
                vRotate.x = pEnemy->GetDirection().xy().Angle();
                vRotate.y = 0.0f;

                for(coreUintW j = i+1u, je = pSquad1->GetNumEnemies(); j < je; ++j)
                {
                    if(!afFall[j] || (j == i+1u))
                    {
                        afFall[j] = MIN(5.0f + 0.6f * afHeight[j], 100.0f);
                    }
                }

                if(!bMover) pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
                if(!bBig)   pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);

                if(bEnabler) iMoverActive = 1u;
                if(bMover)   iMoverActive = 2u;

                this->DisableFloor(i % HARENA_FLOORS, false);

                g_pSpecialEffects->CreateBlowColor(pEnemy->GetPosition(), pEnemy->GetDirection(), 50.0f, 5u, COLOR_ENERGY_WHITE);
            }

            if(bAnyDamage || (bMover && (iMoverActive == 2u)))
            {
                coreVector2 vCurPos = pEnemy->GetPosition().xy();
                coreVector2 vCurDir = vMove;

                vCurPos += vCurDir * (60.0f * TIME);

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.0f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.0f); vCurDir.x =  ABS(vCurDir.x);}
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.0f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.0f); vCurDir.x = -ABS(vCurDir.x);}
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.0f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.0f); vCurDir.y =  ABS(vCurDir.y);}
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.0f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.0f); vCurDir.y = -ABS(vCurDir.y);}

                pEnemy->SetPosition(coreVector3(vCurPos, afHeight[i]));
                vMove = vCurDir;

                vRotate.y += (bBig ? 0.5f : 1.0f) * TIME;
                pEnemy->DefaultRotate(vRotate.x + vRotate.y * (2.0f*PI));

                if(!bMover && !bBig && (vRotate.y >= 3.0f))
                {
                    pEnemy->Kill(true);
                    iPoints += 1u;
                }
            }
            else
            {
                coreVector2 vPos;
                coreVector2 vDir;

                constexpr coreFloat fDistance = 0.4f;

                if(m_iInsanity)
                {
                    if(i < 8u)
                    {
                        coreFloat fOffset;
                        switch(i)
                        {
                        default: ASSERT(false)
                        case 0u: fOffset = -0.5f; break;
                        case 1u: fOffset =  0.5f; break;
                        case 2u: fOffset = -1.5f; break;
                        case 3u: fOffset =  1.5f; break;
                        case 4u: fOffset =  0.5f; break;
                        case 5u: fOffset = -0.5f; break;
                        case 6u: fOffset =  1.5f; break;
                        case 7u: fOffset = -1.5f; break;
                        }

                        const coreFloat fSide = (i % 2u) ? -1.0f : 1.0f;

                        vPos = coreVector2(fOffset * 0.4f, fSide * 0.9f);
                        vDir = coreVector2(0.0f, -fSide);
                    }
                    else
                    {
                        coreFloat fOffset;
                        switch((i - 8u) / 2u)
                        {
                        default: ASSERT(false)
                        case 0u: fOffset = -0.5f; break;
                        case 1u: fOffset =  0.5f; break;
                        case 2u: fOffset =  0.5f; break;
                        case 3u: fOffset = -0.5f; break;
                        }

                        const coreFloat fSide = (((i - 8u) / 2u) % 2u) ? -1.0f : 1.0f;

                        vPos = coreVector2(fSide * 0.9f, fOffset * 0.4f);
                        vDir = coreVector2::Direction(fLifeTime * 3.0f);
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
                        vPos = StepRotated45((i - 32u) % 8u) * fDistance;
                        vDir = pEnemy->AimAtPlayerSide().Normalized();
                    }
                    else if(i < 46u)
                    {
                        vPos = coreVector2(FmodRange(fLifeTime - I_TO_F(i - 40u) * 0.8f, -1.2f, 1.2f), ((i - 40u) % 2u) ? 0.6f : -0.6f);
                        vDir = coreVector2(1.0f, ((i - 40u) % 2u) ? 1.0f : -1.0f).Normalized();
                    }
                    else if(i < 52u)
                    {
                        vPos = coreVector2((I_TO_F(i - 46u) - 2.5f) * -0.3f, FmodRange(-fLifeTime, -1.2f, 1.2f));
                        vDir = coreVector2(((i - 46u) % 2u) ? 1.0f : -1.0f, -1.0f).Normalized();
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
                        vDir = coreVector2(1.0f,1.0f).Normalized();
                    }
                    else
                    {
                        ASSERT(bBig)

                        vPos = coreVector2( 0.0f,0.0f);
                        vDir = coreVector2(-1.0f,2.0f).Normalized();
                    }
                }

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, afHeight[i]));
                pEnemy->SetDirection(coreVector3(vDir, 0.0f));
            }

            if(pEnemy->HasStatus(ENEMY_STATUS_GHOST) && !afHeight[i])
            {
                pEnemy->SetBaseColor(COLOR_SHIP_RED);
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
            }

            constexpr coreFloat fAlpha = 1.0f;

            const coreFloat fVisibility = CLAMP(1.0f - STEPH3(49.0f, 54.0f, afHeight[i]), pEnemy->GetAlpha() / fAlpha, 1.0f);
            //const coreFloat fVisibility = CLAMP(1.0f - STEPH3(9.0f, 14.0f, afHeight[i]), pEnemy->GetAlpha() / fAlpha, 1.0f);
            const coreBool  bGhost      = pEnemy->HasStatus(ENEMY_STATUS_GHOST);

            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * fVisibility * (bGhost ? 1.0f : 1.3f) * (bBig ? 1.5f : 1.0f));
            pEnemy->SetAlpha(fVisibility * (bGhost ? fAlpha : 1.0f));

            if(pEnemy->HasStatus(ENEMY_STATUS_HIDDEN) && fVisibility)
            {
                this->EnableFloor(i % HARENA_FLOORS, pEnemy);
                pEnemy->RemoveStatus(ENEMY_STATUS_HIDDEN);
            }

            if(bBig && bAnyDamage)
            {
                if(STAGE_TICK_LIFETIME(20.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.4f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
                }
            }

            this->CrashEnemy(pEnemy);
        });

        m_iStageSub = MAX(m_iStageSub, iPoints + 1u);

        if(!m_iInsanity)
        {
            cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

            pBackground->SetVeilAlpha(1.0f - STEPH3(0.0f, 30.0f, m_fStageTime));

            STAGE_WAVE(2u, "FÜNFZEHN", {60.0f, 80.0f, 100.0f, 120.0f})
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        m_aInsanityStage[2]();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < HARENA_FLOORS; ++i)
            this->DisableFloor(i, false);

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
    // create 1-2 arrow burst attacks without mother as introduction, player may think those are normal enemies, but should get suspicious
    // wave attack should not spin too fast, as this makes it impossible to properly slip through a hole, but fast enough to make the player move
    // shield requires the player to destroy it from the coming spin-side, to properly attack the mother between his attacks
    // player can actually stand still against burst attack, but that's fine
    // targeted border attack requires player to move and attack to create proper holes
    // line border attack requires the player to always shoot holes, while the targeted border attack can be avoided with movement
    // many of the spawn locations have last player position in mind, based on possible attack locations (specify sub-stage numbers)
    // arrows should have enough life to drive player back if he just keeps shooting at them, but not too much to provide action
    // time between burst attacks gives the player a chance to attack (adjust timing)
    // slight initial delay for the burst attack, to reduce clustering
    // coop attack order: intro 1 0, einzeln 1 0 1, von rechts 0 1
    // number of enemies/arrows determined by worst-case, 3x16 + 100 (transition from top-lines to multi-shield)
    // sonnen-schlag sollt von oben kommen, das is leichter zu kontern (gegner-pattern davor geht nach unten) und einfacher zu navigieren, weil man ihn von unten dann töten kann (natürliche richtung)
    // eine sonnen-reihe muss sich invertiert drehen, weil es sonst viel zu leicht ist das schild zu zerstören
    // bewegte mothers sollten exakt zwischen den arrows sein, um die gefahr eines schlechten spieler-angriffs zu reduzieren (bei der nur 1 von 2 geschosse trifft)
    // multi-schild und sonne müssen sich verlangsamt ausbreiten, weil der spieler sonst nicht mehr reagieren kann
    // multi-schild braucht guten abstand zwischen schichten, damit spieler reinfliegen kann, wenn er will
    // TODO 1: pacifist: holes in line border attack
    // TODO 1: BADGE: kill N% enemies at the start
    // TODO 1: BADGE: kill marked enemies on the line-wave / special colored enemies, all need to be killed for a badge (line border attack)
    // TODO 1: marked enemies are across the mission (1 per sub-stage)
    // TODO 1: keep small sunshine-lines ?
    // TODO 1: final explosion causes too many particles (ASSERT), assert already fixed, but still try to reduce
    // TODO 1: large shield (vielleicht) gruppierte linien, XXX   XXX   XXX (2 oder 3 oder 4, etc.), um durch fliegen leichter/interessanter zu machen
    // TODO 1: leicht: letzter angriff kommt von seite, statt von oben-unten (is das wirklich leichter ?)
    // TODO 1: hard: every killed enemy makes an attack (target single ?)
    // TODO 1: sonne hat mir von oben eine gegeben, vielleicht langsamer spawnen (rotation + ausbreitung)
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    m_aInsanityStage[3] = [this]()
    {
        constexpr coreUintW iNumEnemies = 150u;
        constexpr coreUintW iNumTypes   = coreMath::CeilAlign(iNumEnemies, 4u) / 4u;

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
                pEnemy->Configure(50 + 20, 0u, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(10, 0u, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(3u + iNumTypes + iNumEnemies)
            STAGE_GET_UINT      (iTakeStart)
            STAGE_GET_UINT      (iLoopKill)
            STAGE_GET_UINT      (iLoopCount)
            STAGE_GET_UINT_ARRAY(aiType,   iNumTypes)
            STAGE_GET_UINT_ARRAY(aiVector, iNumEnemies)
        STAGE_GET_END

        STATIC_ASSERT(sizeof(coreUint32) * iNumTypes >= iNumEnemies)
        coreUint8* aiByteType = r_cast<coreUint8*>(aiType);

        const auto nTakeChildFunc = [&](const coreUint8 iType)
        {
            for(coreUintW i = iTakeStart, ie = pSquad2->GetNumEnemies(); i < ie; ++i)
            {
                cEnemy* pCurrent = pSquad2->GetEnemy(i);

                if(pCurrent->HasStatus(ENEMY_STATUS_DEAD))
                {
                    iTakeStart = i + 1u;

                    aiByteType[i] = iType;
                    aiVector  [i] = coreVector2(0.0f,0.0f).PackFloat2x16();

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
            }
        });

        if(pSquad1->IsFinished())
        {
            if(m_iInsanity == HARENA_INSANITY_P1)
            {
                     if(STAGE_SUB(14u)) {}
            }
            else if(m_iInsanity)
            {
                     if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
                else if(STAGE_SUB(13u)) {}   // may not be used
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

                else if(pSquad1->IsFinished()) pSquad2->ClearEnemies(true);
            }
        }

        if((m_iStageSub == 1u) && (m_fStageTime <= 3.0f))
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f))
            {
                cEnemy*         pChild = nTakeChildFunc(0u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreFloat   fOff = ((s_iTick % 30u) < 15u) ? 1.0f : -1.0f;
                const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vAim = (g_pGame->FindPlayerDual(((s_iTick % 30u) < 15u) ? 0u : 1u)->GetPosition().xy() - vPos).Normalized();
                const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                aiVector[iIndex] = vDir.PackFloat2x16();
            }
        }
        else if((m_iStageSub == 4u) || (m_iStageSub == 5u))
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f))
            {
                cEnemy*         pChild = nTakeChildFunc(0u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreFloat   fOff = g_pGame->FindPlayerDual((m_iStageSub == 4u) ? 0u : 1u)->GetPosition().y;
                const coreVector2 vPos = coreVector2(1.2f * FOREGROUND_AREA.x, fOff);

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                aiVector[iIndex] = coreVector2(-1.0f,0.0f).PackFloat2x16();
            }
        }
        else if(m_iStageSub == 7u)
        {
            if(STAGE_BEGINNING2 || STAGE_TICK_FREE2(1.0f, 0.0f))
            {
                for(coreUintW j = 0u; j < 16u; ++j)
                {
                    cEnemy*         pChild = nTakeChildFunc(0u);
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

                    cEnemy*         pChild = nTakeChildFunc(0u);
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
            if(STAGE_TICK_FREE2(0.9f, 0.0f))
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
        else if(m_iStageSub == 12u)
        {
            if((iLoopKill < 32u) && STAGE_TICK_FREE2(10.0f, 0.0f))
            {
                if(!(s_iTick % 16u))
                {
                    if(iLoopKill < 16u) iLoopCount += 1u;
                                   else iLoopCount -= 1u;
                }

                cEnemy*         pChild = nTakeChildFunc(0u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreVector2 vDir = StepRotated90(iLoopCount % 4u);
                const coreFloat   fOff = (I_TO_F(s_iTick % 16u) - 7.5f) * 0.1375f;
                const coreVector2 vPos = (vDir * -1.2f + vDir.Rotated90() * fOff) * FOREGROUND_AREA;

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                aiVector[iIndex] = vDir.PackFloat2x16();
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
            STAGE_LIFETIME(pEnemy, (i >= 9u && i < 12u) ? 0.8f : 1.0f, (i < 1u) ? 4.0f : ((i >= 9u && i < 12u) ? (0.5f * I_TO_F(i - 9u)) : 0.0f))

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

                    const coreVector2 vAim = pEnemy->AimAtPlayerDual((i % 2u) ? 0u : 1u).Normalized();
                    const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                    pChild->SetPosition(pEnemy->GetPosition());

                    aiVector[iIndex] = vDir.PackFloat2x16();
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
                }
            }

            if(STAGE_TAKEOFF)
            {
                if((i >= 1u && i < 8u) || (i >= 13u && i < 15u))
                {
                    for(coreUintW j = 0u; j < 10u; ++j)
                    {
                        cEnemy*         pChild = nTakeChildFunc(i << 2u);
                        const coreUintW iIndex = pSquad2->GetIndex(pChild);

                        pChild->SetPosition(pEnemy->GetPosition());

                        aiVector[iIndex] = coreVector2(((I_TO_F(j) / 10.0f) + CORE_MATH_PRECISION) * (2.0f*PI) * -1.0f, 12.0f).PackFloat2x16();
                    }
                }
                else if(i == 8u)
                {
                    for(coreUintW k = 0u; k < 5u; ++k)
                    {
                        for(coreUintW j = 0u, je = 10u + 5u * k; j < je; ++j)
                        {
                            cEnemy*         pChild = nTakeChildFunc(i << 2u);
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
                        for(coreUintW j = 0u, je = (k < 4u) ? 12u : 6u; j < je; ++j)
                        {
                            cEnemy*         pChild = nTakeChildFunc(i << 2u);
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
                const coreFloat   fLen    = LERPB(0.0f, vRaw.y, MIN(fLifeTime * (((m_iStageSub == 8u) || (m_iStageSub == 10u)) ? 0.5f : 1.0f), 1.0f));

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
                    if(pEnemy->GetLifeTime() >= 1.0f) nReturnChildFunc(pEnemy, i, false);
                }
            }
        });

        if(!m_iInsanity) STAGE_WAVE(3u, "SECHSZEHN", {60.0f, 80.0f, 100.0f, 120.0f})
    };
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        m_aInsanityStage[3]();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

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
    // funken beim spawnen von gegner lenkt zu sehr ab
    // ring-wave, or cone-shaped from center sucked (even after retrying ring-wave)
    // [deprecated] trying to let player move from a soon-to-be-spike to a soon-to-be-no-spike feels bad and hard to use, the time difference needs to be greatly extended which reduces pacing, a jump like in mario is missing (to create player-induced short invincibility from spikes)
    // sägezahn pacing (schnell langsam schnell)
    // gegner spawnen in stacheln, dadurch wird mögliche kollision mit spieler vermieden
    // gegner spawning muss schönes muster und delay haben
    // erste zwei stacheln müssen langsam und in der mitte spawnen, damit spieler genau sieht was passiert
    // spieler muss genau sehen wo bald stacheln kommen und wo gefahr ist (einfärben der platten und stacheln)
    // TODO 1: change plate rendering
    // TODO 1: die symmetrische bewegung der stacheln in phase 3 ist schwer für die augen (2.0 -> 2.0) (jetzt etwas weniger, nachdem ich farben aktualisiert hab)
    // TODO 1: maybe do a semi-transparent wave with the spike model when turning red
    // TODO 1: make sure spike range covers the full area
    // TODO 1: add a bit of delay when damage can be taken
    // TODO 1: all enemies have to rotate in the same direction (intro-spin is negative value which gets removed to 0)
    // TODO 1: dünklere kreise sollten auf der platte sein, wo die stacheln raus kommen
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: hard mode: infinity movement (only X or Y always, even with possible pattern issues, though maybe some have nixe transitions) (also enemies ?)
    // TODO 1: vielleicht platten in semi-random pattern spawnen lassen (achtung bei bossen, hab da eh property)
    // TODO 1: platten farbe sollte bei jeder platte leicht anders sein (zm. die graue), fixed pattern
    m_aInsanityStage[4] = [this]()
    {
        constexpr coreUintW iNumEnemies = 58u;

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, 0u, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        STAGE_GET_START(4u + iNumEnemies)
            STAGE_GET_UINT       (iFlipCount)
            STAGE_GET_UINT       (iSpikeCount)
            STAGE_GET_FLOAT      (fSpikeDelay)
            STAGE_GET_FLOAT      (fEnemyDelay)
            STAGE_GET_FLOAT_ARRAY(afIntroTime, iNumEnemies)
        STAGE_GET_END

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
                     if(STAGE_SUB( 9u)) {STAGE_RESURRECT(pSquad1, 42u, 57u) fEnemyDelay = 0.0f;}
                else if(STAGE_SUB(10u))
                {
                    for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                        this->DisableSpike(i, true);
                }
            }
            else
            {
                     if(STAGE_SUB( 1u)) {STAGE_RESURRECT(pSquad1,  0u,  3u) fEnemyDelay = 0.0f;}
                else if(STAGE_SUB( 2u)) {STAGE_RESURRECT(pSquad1,  4u,  9u) fEnemyDelay = 2.0f;}
                else if(STAGE_SUB( 3u)) {STAGE_RESURRECT(pSquad1, 10u, 15u) fEnemyDelay = 5.0f;}
                else if(STAGE_SUB( 4u)) {STAGE_RESURRECT(pSquad1, 16u, 23u) fEnemyDelay = 5.0f;}
                else if(STAGE_SUB( 5u)) {STAGE_RESURRECT(pSquad1, 24u, 29u) fEnemyDelay = 4.0f;}
                else if(STAGE_SUB( 6u)) {STAGE_RESURRECT(pSquad1, 30u, 37u) fEnemyDelay = 5.0f;}
                else if(STAGE_SUB( 7u)) {STAGE_RESURRECT(pSquad1, 38u, 41u) fEnemyDelay = 2.0f;}
                else if(STAGE_SUB( 8u))
                {
                    for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                        this->DisableSpike(i, true);
                }
            }

            iSpikeCount = 0u;
            fSpikeDelay = (m_iStageSub == 1u) ? 1.0f : 0.5f;
        }

        if(STAGE_BEGINNING)
        {
            if(!m_iInsanity)
            {
                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                    this->EnableSpike(i, true);
            }
        }

        if(fSpikeDelay > 0.0f)
        {
            fSpikeDelay -= 1.0f * TIME;
        }
        else if(m_iStageSub == 1u)
        {
            // intro single fields
            if(STAGE_TICK_FREE(2.0f, 0.0f) && (!(s_iTick % 3u) || (iSpikeCount >= 3u)))
            {
                const coreUintW iIndex = ((iSpikeCount + 21u) * 11u) % HARENA_SPIKES;

                const coreUintW iColumn = iIndex % HARENA_SPIKE_DIMENSION;
                const coreUintW iRow    = iIndex / HARENA_SPIKE_DIMENSION;

                this->LaunchSpike(nToIndexFunc(iColumn, iRow), 2.0f);

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 2u)
        {
            // lines raining down
            if(STAGE_TICK_FREE(6.0f, 0.0f))
            {
                const coreUintW iColumn1 =  (((iSpikeCount)      / HARENA_SPIKE_DIMENSION)       % 3u);
                const coreUintW iColumn2 = ((((iSpikeCount + 3u) / HARENA_SPIKE_DIMENSION) + 2u) % 3u) + 3u;

                const coreUintW iRow1 = (iSpikeCount)      % HARENA_SPIKE_DIMENSION;
                const coreUintW iRow2 = (iSpikeCount + 3u) % HARENA_SPIKE_DIMENSION;

                                      this->LaunchSpike(nToIndexFunc(iColumn1, iRow1), 2.0f);
                if(iSpikeCount >= 3u) this->LaunchSpike(nToIndexFunc(iColumn2, iRow2), 2.0f);

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 3u)
        {
            // left and right lines
            if(STAGE_TICK_FREE(2.0f, 0.0f))
            {
                const coreUintW iColumn = iSpikeCount % HARENA_SPIKE_DIMENSION;

                for(coreUintW i = 0u; i < HARENA_SPIKE_DIMENSION; ++i)
                {
                    const coreUintW iRealColumn = (i % 2u) ? ((HARENA_SPIKE_DIMENSION - 1u) - iColumn) : iColumn;

                    this->LaunchSpike(nToIndexFunc(iRealColumn, i), 2.0f);
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 4u)
        {
            // slow quads from the bottom
            if(STAGE_TICK_FREE(1.0f, 0.0f))
            {
                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                {
                    const coreUintW iColumn = (i % HARENA_SPIKE_DIMENSION) / 2u;
                    const coreUintW iRow    = (i / HARENA_SPIKE_DIMENSION) / 2u;

                    if(((iColumn + iRow + iSpikeCount) % (HARENA_SPIKE_DIMENSION / 2u))) continue;

                    this->LaunchSpike(i, 2.0f);
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 5u)
        {
            // large lines from left with hole
            if(STAGE_TICK_FREE(3.0f, 0.0f))
            {
                const coreUintW iColumn = iSpikeCount % HARENA_SPIKE_DIMENSION;

                coreUintW iSkip;
                switch((iSpikeCount / 6u) % 4u)
                {
                default: ASSERT(false)
                case 0u: iSkip = 1u; break;
                case 1u: iSkip = 3u; break;
                case 2u: iSkip = 4u; break;
                case 3u: iSkip = 2u; break;
                }

                for(coreUintW i = 0u; i < HARENA_SPIKE_DIMENSION; ++i)
                {
                    if(i == iSkip) continue;

                    this->LaunchSpike(nToIndexFunc(iColumn, i), 2.0f);
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 6u)
        {
            // checkerboard
            if(STAGE_TICK_FREE(1.2f, 0.0f))
            {
                coreUintW iQuad;
                switch(iSpikeCount % 4u)
                {
                default: ASSERT(false)
                case 0u: iQuad = 0u; break;
                case 1u: iQuad = 1u; break;
                case 2u: iQuad = 3u; break;
                case 3u: iQuad = 2u; break;
                }

                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                {
                    const coreUintW iColumn = i % HARENA_SPIKE_DIMENSION;
                    const coreUintW iRow    = i / HARENA_SPIKE_DIMENSION;

                    if((HAS_BIT(iQuad, 0u) != (iColumn % 2u)) || (HAS_BIT(iQuad, 1u) != (iRow % 2u))) continue;

                    this->LaunchSpike(i, 2.0f);
                }

                iSpikeCount += 1u;
            }
        }
        else if(m_iStageSub == 7u)
        {
            // spawn below player
        }
        else if(m_iStageSub == 9u)
        {
            const coreUintW iNewCount = pSquad1->GetNumEnemiesAlive() + ((m_fStageSubTime < 2.5f) ? 1u : 0u);

            if(iFlipCount != iNewCount)
            {
                iFlipCount = iNewCount;

                switch(iNewCount)
                {
                default: ASSERT(false)
                case 17u: this->EnableSpike( 2u, false); this->EnableSpike(30u, false);                                                                   break;
                case 16u: this->EnableSpike( 0u, false); this->EnableSpike(29u, false); this->LaunchSpike( 2u, 3600.0f); this->LaunchSpike(30u, 3600.0f); break;
                case 15u: this->EnableSpike(12u, false); this->EnableSpike(35u, false); this->LaunchSpike( 0u, 3600.0f); this->LaunchSpike(29u, 3600.0f); break;
                case 14u: this->EnableSpike( 5u, false); this->EnableSpike(33u, false); this->LaunchSpike(12u, 3600.0f); this->LaunchSpike(35u, 3600.0f); break;
                case 13u: this->EnableSpike(11u, false);                                this->LaunchSpike( 5u, 3600.0f); this->LaunchSpike(33u, 3600.0f); break;
                case 12u: this->EnableSpike(31u, false);                                this->LaunchSpike(11u, 3600.0f);                                  break;
                case 11u: this->EnableSpike( 3u, false);                                this->LaunchSpike(31u, 3600.0f);                                  break;
                case 10u: this->EnableSpike(18u, false);                                this->LaunchSpike( 3u, 3600.0f);                                  break;
                case  9u: this->EnableSpike(17u, false);                                this->LaunchSpike(18u, 3600.0f);                                  break;
                case  8u: this->EnableSpike( 1u, false);                                this->LaunchSpike(17u, 3600.0f);                                  break;
                case  7u: this->EnableSpike(34u, false);                                this->LaunchSpike( 1u, 3600.0f);                                  break;
                case  6u: this->EnableSpike(24u, false);                                this->LaunchSpike(34u, 3600.0f);                                  break;
                case  5u: this->EnableSpike(10u, false); this->EnableSpike(23u, false); this->LaunchSpike(24u, 3600.0f);                                  break;
                case  4u: this->EnableSpike( 6u, false); this->EnableSpike(25u, false); this->LaunchSpike(10u, 3600.0f); this->LaunchSpike(23u, 3600.0f); break;
                case  3u: this->EnableSpike(28u, false); this->EnableSpike(32u, false); this->LaunchSpike( 6u, 3600.0f); this->LaunchSpike(25u, 3600.0f); break;
                case  2u: this->EnableSpike( 4u, false); this->EnableSpike( 7u, false); this->LaunchSpike(28u, 3600.0f); this->LaunchSpike(32u, 3600.0f); break;
                case  1u:                                                               this->LaunchSpike( 4u, 3600.0f); this->LaunchSpike( 7u, 3600.0f); break;
                case  0u:                                                                                                                                 break;
                }
            }
        }

        if(m_iStageSub < 10u)
        {
            for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
            {
                const coreObject3D* pBoard = this->GetSpikeBoard(i);
                if(!pBoard->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

                const coreBool bIsActive = this->GetSpikeLaunched(i);
                const coreBool bIsQuiet  = this->GetSpikeQuiet   (i) && (m_iStageSub == 7u);

                if(bIsActive || bIsQuiet)
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - pBoard->GetPosition().xy();

                        if((ABS(vDiff.x) < pBoard->GetCollisionRange().x) &&
                           (ABS(vDiff.y) < pBoard->GetCollisionRange().y))
                        {
                            if(bIsActive &&  pPlayer->IsNormal ()) pPlayer->TakeDamage(10, ELEMENT_NEUTRAL, pPlayer->GetPosition().xy());
                            if(bIsQuiet  && !pPlayer->IsRolling()) this->LaunchSpike(i, 8.0f * RCP(I_TO_F(g_pGame->GetNumPlayers())));
                        }
                    });
                }

                if(bIsActive && (m_fStageSubTime >= fEnemyDelay))
                {
                    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, j)
                    {
                        if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST)) return;

                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - pBoard->GetPosition().xy();

                        if(vDiff.LengthSq() < POW2(pBoard->GetCollisionRange().x))
                        {
                            pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                        }
                    });
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreUintW iColumn, iRow;
            switch(i)
            {
            default: ASSERT(false)
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
            case 44u: iColumn = 3u; iRow = 0u; break;
            case 45u: iColumn = 4u; iRow = 0u; break;
            case 46u: iColumn = 0u; iRow = 1u; break;
            case 47u: iColumn = 5u; iRow = 1u; break;
            case 48u: iColumn = 0u; iRow = 2u; break;
            case 49u: iColumn = 5u; iRow = 2u; break;
            case 50u: iColumn = 0u; iRow = 3u; break;
            case 51u: iColumn = 5u; iRow = 3u; break;
            case 52u: iColumn = 0u; iRow = 4u; break;
            case 53u: iColumn = 5u; iRow = 4u; break;
            case 54u: iColumn = 1u; iRow = 5u; break;
            case 55u: iColumn = 2u; iRow = 5u; break;
            case 56u: iColumn = 3u; iRow = 5u; break;
            case 57u: iColumn = 4u; iRow = 5u; break;
            }

            pEnemy->SetPosition(coreVector3(nToPositionFunc(iColumn, iRow), 0.0f));

            if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
            {
                pEnemy->SetSize(coreVector3(0.0f,0.0f,0.0f));
            }
            else
            {
                afIntroTime[i] += 3.0f * TIME;

                const coreFloat fValue = LERPB(0.0f, 1.0f, MIN(afIntroTime[i], 1.0f));
                const coreFloat fSpin  = MAX(afIntroTime[i] - 0.0f, 0.0f);

                pEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.3f * fValue);
                pEnemy->SetDirection(coreVector3(coreVector2::Direction((1.0f*PI) * LERP(fValue, fSpin, 0.5f)), 0.0f));
            }

            this->CrashEnemy(pEnemy);
        });

        if(!m_iInsanity)
        {
            cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

            pBackground->SetGroundDensity(0u, 1.0f - STEPH3(0.0f, 30.0f, m_fStageTime));
            pBackground->SetGroundDensity(1u, 1.0f - STEPH3(0.0f, 30.0f, m_fStageTime));

            STAGE_WAVE(4u, "SIEBZEHN", {60.0f, 80.0f, 100.0f, 120.0f})
        }
    };
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        m_aInsanityStage[4]();
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

    // ################################################################
    // wait for play
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // can only kill in order
    // coop last enemy, need to work together
    // abwechselnd links rechts
    // doppelgruppe nur breites pattern um ausweichen zu erleichtern
    // reihenfolge am weitesten entfernt
    // TODO 1: show 3142 group at start, matrix of enemies
    // TODO 1: simon says, linear up and down, to make distinction easy, and to prevent accidental hits (order forward, backward, mirrored, to stay fair but variable)
    // TODO 1: static assertion, keine der chief-order darf enemy-count übersteigen
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
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
                //pEnemy->Configure((i == 8u) ? 200 : 50, 0u, COLOR_SHIP_GREY);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREY);
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

        STAGE_WAVE(5u, "ACHTZEHN", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pGame->KillHelpers();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetGroundDensity(0u, 0.0f);
        pBackground->SetGroundDensity(1u, 0.0f);

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
        STAGE_BOSS(m_Tiger, {60.0f, 120.0f, 180.0, 240.0f})
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