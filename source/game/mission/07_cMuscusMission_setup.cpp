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
// setup the Muscus mission
void cMuscusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            if(pPlayer->IsEnemyLook()) pPlayer->TurnIntoPlayer();
        });

        STAGE_FINISH_NOW
    });

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
        g_pEnvironment->ChangeBackground(cMossBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

        g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
        g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
        g_pEnvironment->SetTargetSpeedNow    (6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
    {
        cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

        pBackground->GetHeadlight()->ResetFlicker();
        pBackground->SetEnableLightning(false);
        pBackground->SetEnableHeadlight(true);

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
    // ghost enemies only move when nobody is looking
    // gegner müssen "mehr" machen wenn sie im schatten sind, damit sich die dunkel-phase lohnt
    // kontinuierliche geschosse erzeugen eine sich windende schlange, ist aber leicht zu erraten
    // kleine wellen von aktueller position ist wesentlich subtiler, und extrem erschreckend wenn das licht aus ist
    // geschosse sollen nur navigation erschweren, wenn ich auf der linie des gegners bin sollt ich ihn angreifen können, ohne dass mir ein geschoss im weg steht
    // waiting time before and after the light goes out, to comprehend the situation, enemy starts far away and "taps" to the player, to emphasis the situation and give enough time for the player to react
    // dicker gegner (am ende) kann überraschen
    // wenn gegner außerhalb des lichts von anfang an sichtbar sind, wird die ganze spannung weggenommen (die mechanik wird nicht voll ausgereizt), wenn sie unsichtbar sind muss man ihre position aber erraten können (geschosse)
    // wenn gegner sich anders verhalten innerhalb und außerhalb licht, kommt das nicht zur geltung (zu kurz), da innerhalb des lichts auch gleich getötet wird
    // flucht vor licht (und spieler) ist nervig wenn man versucht auf die gegner zu zielen
    // mehrere verfolgende gegner konvergieren zu stark auf den gleichen pfad
    // bei 6x6 gruppe sollte das erste ziel am mittleren rand anfangen (um leichter gefunden zu werden), aber nicht dort wo man abhängig von der vorherigen gruppe hinschießen würde
    // in the 6x6 wave, having enemies stay disappeared instead of appearing again when looking away is much more comprehensible, and removes some back-and-forth animation issues
    // gegner in der 6x6 gruppe sollten nicht ganz am rand sein, weil sie sonst nicht gut auffallen
    // no collision with player, which makes 6x6 easier
    // the two single snakes and the jumping enemy are right-left-right
    // there should be a pause after the first big enemy, due to the high shooting frequency requiring the player to navigate to safety first
    // in the on-and-off scenario at the end it should be possible to light-up two enemies in a row if fast enough
    // erster gegner muss weit am rand sein (um die unsichtbarkeits mechanik zu erklären), und sich bewegen (um auch gleich die beziehung mit den geschossen als positions-indikator zu erklären), zweiter gegner muss weit weg vom ersten sein
    // beim teleporter gegner sollte die nächste position nicht zu nah am kreuz der vorherigen position sein, beachte den lichtkegel
    // currently blending-in and blending-out is binary (which is much smoother), except in 6x6 wave, where it gets mixed with the lerp-value, to make sure no enemy is visible inside the cone
    // TASK: destroy all hidden enemies
    // TASK: touch an invisible enemy
    // TASK EXTRA: charge the sun
    // ACHIEVEMENT: destroy 5 enemies while no light is shining on them
    // COOP: active light alternates between players
    // TODO 5: badge: let them all stare at you (keep all in 6x6 visible for 1 second)
    // TODO 5: badge: etwas einsammeln/berühren, dass man nur mit licht sieht, nur in bestimmter phase, N items
    // TODO 1: hardmode: zusätzliche Mario geister die man nicht töten kann (vlt. nur einer)
    // TODO 1: disable shadow and water-reflection (completely) for segments in darkness, to hide popping artifacts (though only for shadow)
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreUintW iNumData = 36u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.0f), coreVector2(1.0f,0.0f), 0.7f);
            pPath1->AddNode(coreVector2( 1.2f,0.0f), coreVector2(1.0f,0.0f), 0.7f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 65u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * ((i == 23u || i == 27u) ? 2.3f : 1.6f));
                pEnemy->Configure((i == 27u) ? 240 : (i == 23u) ? 120 : 10, 0u, COLOR_SHIP_GREEN);
                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.6f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_HIDDEN | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(iNumData + 6u)
            STAGE_GET_FLOAT_ARRAY(afFade, iNumData)
            STAGE_GET_FLOAT      (fBlind)
            STAGE_GET_FLOAT      (fBlindLoop)
            STAGE_GET_UINT       (iStepHit)
            STAGE_GET_UINT       (iStepRemove)
            STAGE_GET_UINT       (iHiddenState)
            STAGE_GET_UINT       (iDarkCount)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        cHeadlight* pHeadlight = d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight();

        const auto nVisibilityFunc = [](const cPlayer* pPlayer, const coreVector2 vPosition)
        {
            const coreVector2 vPos = pPlayer->GetPosition ().xy();
            const coreVector2 vDir = pPlayer->GetDirection().xy();
            const coreVector2 vTan = vDir.Rotated90();

            const coreVector2 vDiff = vPosition - (vPos + vDir * 3.5f);
            const coreFloat   fLen  = coreVector2::Dot(vDiff, vDir) * 0.29f;
            const coreFloat   fSide = coreVector2::Dot(vDiff, vTan);
            const coreFloat   fMax  = vPosition.Processed(ABS).Max();

            return (1.0f - STEPH3(fLen, fLen + 4.0f, ABS(fSide))) * (1.0f - STEPH3(1.0f, 1.1f, fMax / FOREGROUND_AREA.x));
        };

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  4u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  5u,  5u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 16u, 16u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 17u, 17u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 18u, 18u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 19u, 19u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 20u, 20u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 21u, 21u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 22u, 22u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 23u, 23u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1,  6u,  6u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 28u, 28u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 29u, 64u)   // #
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB(22u)) STAGE_RESURRECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB(23u)) STAGE_RESURRECT(pSquad1, 26u, 26u)
            else if(STAGE_SUB(24u)) STAGE_RESURRECT(pSquad1, 27u, 27u)
            else if(STAGE_SUB(25u)) if(fBlind) fBlind = CORE_MATH_PRECISION;

            if(g_pGame->IsTask())
            {
                pSquad2->ClearEnemies(false);
                     if(m_iStageSub ==  2u) STAGE_RESURRECT(pSquad2, 0u, 0u)
                else if(m_iStageSub == 10u) STAGE_RESURRECT(pSquad2, 1u, 1u)
                else if(m_iStageSub == 17u) STAGE_RESURRECT(pSquad2, 2u, 2u)
                else if(m_iStageSub == 18u) STAGE_RESURRECT(pSquad2, 3u, 3u)
                else if(m_iStageSub == 20u) STAGE_RESURRECT(pSquad2, 4u, 4u)
            }

            if(g_pGame->IsTaskExtra())
            {
                if(m_iStageSub == 18u)
                {
                    this->EnableSun();
                    m_Sun.SetPosition(coreVector3(0.0f,1.3f,0.0f) * FOREGROUND_AREA3);
                }
            }

            std::memset(afFade, 0, sizeof(coreFloat) * iNumData);
        }

        if(STAGE_BEGINNING)
        {
            pHeadlight->PlayFlicker(HEADLIGHT_TYPE_ON);
        }

        if(g_pGame->IsMulti())
        {
            if(((m_iStageSub == 10u) && STAGE_SUBTIME_AFTER(3.0f)) || (m_iStageSub > 10u))
            {
                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pHeadlight->SetDefault(i, true);
                });
            }
            else if(!pHeadlight->IsFlickering())
            {
                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pHeadlight->SetDefault(i, (((m_iStageSub + 1u) % 2u) == i));
                });
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_BLUE);

        if((m_iStageSub == 7u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.9f);

            if(pHelper->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, pHelper->GetLifeTime()))
            {
                pHelper->Kill(false);
            }

            STAGE_FOREACH_PLAYER(pPlayer, j)
            {
                if(!pHeadlight->GetDefault(j)) return;

                pHelper->SetAlpha(nVisibilityFunc(pPlayer, pHelper->GetPosition().xy()));
            });
        }

        if(m_Sun.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            const coreVector2 vNewPos = m_Sun.GetPosition().xy() + coreVector2(0.0f,-1.0f) * (5.0f * TIME);

            if(vNewPos.y < FOREGROUND_AREA.y * -1.3f)
                this->DisableSun(false);

            m_Sun.SetPosition(coreVector3(vNewPos, 0.0f));

            coreBool bCharge = false;
            STAGE_FOREACH_PLAYER(pPlayer, j)
            {
                if(!pHeadlight->GetDefault(j)) return;

                if(nVisibilityFunc(pPlayer, m_Sun.GetPosition().xy()))
                    bCharge = true;
            });

            if(bCharge)
            {
                m_fSunValue.UpdateMin(0.4f, 1.0f);
                if(m_fSunValue >= 1.0f)
                {
                    STAGE_BADGE(2u, BADGE_HARD, m_Sun.GetPosition())
                }
                else if(STAGE_TICK_FREE(30.0f, 0.0f))
                {
                    g_pSpecialEffects->CreateSplashColor(m_Sun.GetPosition(), 25.0f, 1u, COLOR_ENERGY_ORANGE);
                }
            }
        }

        if(m_iStageSub == 10u)
        {
            if(STAGE_SUBTIME_POINT(1.0f))
            {
                 fBlind = 6.5f;
                 pHeadlight->PlayFlicker(HEADLIGHT_TYPE_OFF);
            }
        }
        else if((m_iStageSub >= 21u) && (m_iStageSub <= 24u))
        {
            fBlindLoop -= 1.0f * TIME;
            if(fBlindLoop <= 0.0f)
            {
                fBlindLoop += 3.2f;

                fBlind = 2.0f;
                pHeadlight->PlayFlicker(HEADLIGHT_TYPE_OFF);
            }
        }

        if(fBlind)
        {
            fBlind = MAX0(fBlind - 1.0f * TIME);
            if(!fBlind) pHeadlight->PlayFlicker(HEADLIGHT_TYPE_ON);
        }

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i >= 6u && i < 16u) ? 0.85f : 1.0f, (i == 6u) ? 1.3f : ((i == 19u) ? 2.0f : ((i >= 8u && i < 16u) ? (0.5f * I_TO_F((i - 8u) % 4u) + ((i >= 8u && i < 12u) ? 0.5f : 0.0f)) : 0.0f)))

            coreBool bDisappear = false;
            coreBool bInverted  = false;
            coreBool bShootWave = false;

            if(i < 2u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f, i ? 0.9f : -0.9f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }
            else if(i < 6u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((i % 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f, I_TO_F(4u - i) * 0.3f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 16u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f, (i < 8u) ? (((i - 6u) % 2u) ? -0.9f : 0.9f) : ((i < 12u) ? (((i - 8u) % 2u) ? -0.9f : 0.9f) : (((i - 12u) % 2u) ? -0.3f : 0.9f)));

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  8u) pEnemy->Rotate90 ();
                else if(i < 12u) pEnemy->Rotate90 ();
                else if(i < 14u) {}
                else if(i < 16u) pEnemy->Rotate180();
            }
            else if(i < 19u)
            {
                coreVector2 vPos;
                switch(i - 16u)
                {
                default: ASSERT(false)
                case 0u: vPos = coreVector2(-0.9f, 0.0f); break;
                case 1u: vPos = coreVector2( 0.9f,-0.9f); break;
                case 2u: vPos = coreVector2( 0.0f, 0.9f); break;
                }

                pEnemy->SetPosition  (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->DefaultRotate(fLifeTime * 5.0f);
            }
            else if(i < 28u)
            {
                if(STAGE_TAKEOFF)
                {
                    const auto nSpawnPosFunc = [&]()
                    {
                        if(i == 19u) return coreVector2(0.0f,1.0f);

                        coreVector2 vPos = coreVector2(0.0f,0.0f);
                        coreUintW   iNum = 0u;

                        STAGE_FOREACH_PLAYER(pPlayer, i)
                        {
                            vPos -= pPlayer->GetPosition().xy();
                            iNum += 1u;
                        });

                        return (vPos * RCP(I_TO_F(iNum))).Processed(SIGN);
                    };

                    const coreVector2 vPos = nSpawnPosFunc() * FOREGROUND_AREA * 0.9f;
                    const coreVector2 vDir = -vPos.Normalized();

                    pEnemy->SetPosition (coreVector3(vPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir, 0.0f));
                }

                if(STAGE_LIFETIME_AFTER(0.0f)) pEnemy->DefaultMoveTarget(pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy(), 30.0f, 2.0f);
            }
            else if(i < 29u)
            {
                coreVector2 vPos;
                switch(iStepHit)
                {
                default: ASSERT(false)
                case 0u: vPos = coreVector2(-0.8f, 0.8f); break;
                case 1u: vPos = coreVector2( 0.8f,-0.8f); break;
                case 2u: vPos = coreVector2(-0.8f, 0.0f); break;
                case 3u: vPos = coreVector2( 0.8f, 0.8f); break;
                case 4u: vPos = coreVector2(-0.8f,-0.8f); break;
                case 5u: vPos = coreVector2( 0.0f, 0.8f); break;
                case 6u: vPos = coreVector2( 0.8f, 0.0f); break;
                case 7u: vPos = coreVector2( 0.0f,-0.8f); break;
                case 8u:
                case 9u: vPos = coreVector2(-0.8f, 0.0f); break;
                }

                pEnemy->SetPosition  (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->DefaultRotate(fLifeTime * 5.0f);
            }
            else if(i < 65u)
            {
                const coreUintW iIndex = i - 29u;

                const coreVector2 vPos = (coreVector2(I_TO_F(iIndex % 6u), I_TO_F(iIndex / 6u)) - 2.5f) * 0.36f;
                const coreVector2 vDir = pEnemy->AimAtPlayerDual((iIndex + (iIndex / 6u)) % 2u).Normalized();

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));

                     if(iStepRemove == 0u) bDisappear = (iIndex != 28u);
                else if(iStepRemove == 1u) bDisappear = (iIndex !=  8u);
                else if(iStepRemove == 2u) bDisappear = (iIndex != 19u);
                else if(iStepRemove == 3u) bDisappear = (iIndex != 15u);
                else                       bDisappear = true;

                bInverted = true;

                if(iStepRemove == 4u)
                {
                    if(!pEnemy->GetAlpha())
                    {
                        pEnemy->Kill(false);
                        bPostpone = true;
                    }
                }
                else
                {
                    if(pEnemy->ReachedDeath())
                    {
                        iStepRemove += 1u;
                        bShootWave = true;
                    }
                }
            }

            coreFloat fAlpha = bInverted ? BLENDS(MIN1(fLifeTime)) : 0.0f;

            if(!pEnemy->ReachedDeath())
            {
                if(!fBlind || pHeadlight->IsFlickering())
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        if(!pHeadlight->GetDefault(j)) return;

                        const coreFloat fValue = nVisibilityFunc(pPlayer, pEnemy->GetPosition().xy());

                        if(bInverted)
                        {
                            // disappear
                            fAlpha = MIN(fAlpha, bDisappear ? (1.0f - fValue) : 1.0f);
                        }
                        else
                        {
                            // appear
                            if((i == 28u) && (iStepHit < 9u))
                            {
                                if(fValue) iStepHit += 1u;
                            }
                            else
                            {
                                fAlpha = MAX(fAlpha, fValue);
                            }
                        }
                    });
                }
            }
            else
            {
                coreBool bShine = false;
                if(!fBlind || pHeadlight->IsFlickering())
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        if(!pHeadlight->GetDefault(j)) return;

                        if(nVisibilityFunc(pPlayer, pEnemy->GetPosition().xy()))
                            bShine = true;
                    });
                }

                if(!bShine)
                {
                    if(++iDarkCount >= 5u) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pEnemy->GetPosition())
                }
            }

            coreFloat& fFade = STAGE_SINK_FLOAT(afFade[i % iNumData]);

            if(bInverted) fFade = MIN(fFade, fAlpha);

            if(fAlpha >= 1.0f) fFade = MIN1(fFade + 5.0f * TIME);
                          else fFade = MAX0(fFade - 5.0f * TIME);

            pEnemy->SetAlpha(fFade);

            if(fFade)
            {
                if(!bDisappear) pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);
                pEnemy->RemoveStatus(ENEMY_STATUS_HIDDEN);
            }
            else
            {
                if(!bDisappear) pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);
                pEnemy->AddStatus(ENEMY_STATUS_HIDDEN);
            }

            if(i < 6u || (i >= 16u && i < 24u) || bShootWave)
            {
                if(STAGE_TICK_LIFETIME((i == 23u) ? 3.0f : 2.0f, 0.0f) || bShootWave)
                {
                    const coreVector2 vPos   = pEnemy->GetPosition().xy();
                    const coreUintW   iNum   = (bShootWave ? 6u : 3u) * (g_pGame->IsEasy() ? 1u : 2u);
                    const coreFloat   fShift = ((s_iTick % 2u) || bShootWave) ? 0.5f : 0.0f;

                    for(coreUintW j = iNum; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) + fShift) * (180.0f / I_TO_F(iNum))));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.9f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.9f);
                    }

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_BLUE);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if((i >= 6u && i < 16u) || (i >= 24u && i < 29u))
            {
                if((!pEnemy->GetAlpha() || (i == 27u)) && STAGE_TICK_LIFETIME(20.0f / fLifeSpeed, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = g_pGame->IsEasy() ? ((i < 12u || i == 28u) ? -pEnemy->GetPosition().xy().Normalized() : pEnemy->GetDirection().xy()) : pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (i == 27u) ? 0.0f : 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.9f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_BLUE);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(g_pGame->IsTask())
            {
                if(!bInverted && pEnemy->HasStatus(ENEMY_STATUS_HIDDEN))
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();
                        if(vDiff.LengthSq() < POW2(4.0f))
                        {
                            STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                        }
                    });
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vPos;
            switch(i)
            {
            default: ASSERT(false)
            case 0u: vPos = coreVector2( 0.0f, 0.0f); break;
            case 1u: vPos = coreVector2(-0.9f,-0.9f); break;
            case 2u: vPos = coreVector2( 0.8f, 0.8f); break;
            case 3u: vPos = coreVector2( 0.9f,-0.9f); break;
            case 4u: vPos = coreVector2(-0.9f, 0.9f); break;
            }

            pEnemy->SetPosition  (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            pEnemy->DefaultRotate(fLifeTime * 5.0f);

            if(pEnemy->ReachedDeath())
            {
                if(++iHiddenState >= pSquad2->GetNumEnemies())
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                }
                else
                {
                    g_pGame->GetCombatText()->DrawProgress(iHiddenState, pSquad2->GetNumEnemies(), pEnemy->GetPosition());
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iHiddenState, pSquad2->GetNumEnemies()), SOUND_ITEM_COLLECT);
                }
            }
        });

        if(!bPostpone) STAGE_WAVE(0u, "7-1", {55.0f, 80.0f, 110.0f, 135.0f, 270.0f})   // SIEBENUNDDREISSIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        this->DisableSun(false);

        cHeadlight* pHeadlight = d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight();

        pHeadlight->ResetFlicker();

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            pHeadlight->SetDefault(i, true);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // damaging fields hide themselves (d/generation)
    // idea: field chases player and gets visible regularly (often)
    // idea: snake
    // idea: linien auf allen 4 seiten die sich unsichtbar richtung mitte bewegen (außen-blöcke sichtbar)
    // idea: block in enemy
    // enemies should be aligned to same boundary as blocks, at least the still ones
    // alle blöcke sollten raster-aligned sein, wenn möglich
    // grow kann ich nicht machen, der spieler muss die größe einschätzen können, somit muss sowohl größe als auch geschwindigkeit konstant bleiben
    // spieler muss nicht sehen wie jeder block aus bild geht, lenkt nur ab, und ist eh klar wenn intro-block sichtbar bild verlässt
    // wenn sich gegner bewegen, entweder mit dem muster, oder 90/180 zum muster (wird derzeit nicht eingehalten)
    // blöcke zu verfolgen ist mental schon sehr komplex, weswegen man die twists in grenzen halten sollte
    // gegner die mit den blöcken moven können ruhig mehr life haben, weil spawning eh im rythmus stattfindet
    // bei übergang zu links-rechts welle muss spieler in die mitte gedrängt werden, wegen des plötzlichen richtungs-wechsel
    // TASK: touch all diamond fields
    // TASK: spin once against direction of last line
    // ACHIEVEMENT: finish the stage without ever letting a damaging field disappear
    // TODO 5: badge: show arrows how to pass through holes ##
    // TODO 5: badge: attack enemies in certain order in last wave oder left-right oder up-down wave, oder in einzel-wave davor
    // TODO 5: badge: get N blocks (count-down) at N places at the bottom
    // TODO 1: hard mode: alles 45 grad gedreht
    // TODO 1: hard mode: der rand is mit generates zugedeckt (man sieht die ecken?)
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreFloat fStep        = 0.275f;
        constexpr coreFloat fStepDelay   = 0.35f;
        constexpr coreFloat fSpeed       = 0.8f;
        constexpr coreFloat fFreqSlow    = 1.1f;
        constexpr coreFloat fFreqFast    = 2.0f;
        constexpr coreUintW iTypeMapSize = coreMath::CeilAlign(MUSCUS_GENERATES, 4u) / 4u;

        // {sub-stage from, sub-stage to, generate-number}
        constexpr coreUint32 aaiTouch[][3] = {{2u, 3u, 19u}, {4u, 4u, 23u}, {5u, 5u, 24u}, {6u, 6u, 13u}, {7u, 7u, 15u}};

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
            pPath2->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 100u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(10, 0u, COLOR_SHIP_BLUE);
            });
        });

        STAGE_GET_START(iTypeMapSize + MUSCUS_GENERATES * 3u + 10u)
            STAGE_GET_UINT_ARRAY (aiGenerateTypeMap, iTypeMapSize)
            STAGE_GET_VEC2_ARRAY (avGenerateData,    MUSCUS_GENERATES)
            STAGE_GET_FLOAT_ARRAY(afGenerateTime,    MUSCUS_GENERATES)
            STAGE_GET_FLOAT      (fRotation,         fRotation =  0.5f*PI;)
            STAGE_GET_FLOAT      (fSpawn,            fSpawn    = -1.0f)
            STAGE_GET_UINT       (iSpawnTick)
            STAGE_GET_UINT       (iSpawnCount)
            STAGE_GET_FLOAT      (fSpawnOffset)
            STAGE_GET_UINT       (iCreateStart)
            STAGE_GET_UINT       (iCreateCount)
            STAGE_GET_UINT       (iTouchState)
            STAGE_GET_UINT       (iDisableState)
            STAGE_GET_UINT       (iAroundState)
        STAGE_GET_END

        const coreFloat fPrevRotation = fRotation;

        coreUint8* aiGenerateType = r_cast<coreUint8*>(aiGenerateTypeMap);

        const auto nTouchFunc = [&]()
        {
            if(g_pGame->IsTask())
            {
                for(coreUintW i = 0u; i < ARRAY_SIZE(aaiTouch); ++i)
                {
                    if((aaiTouch[i][0] <= m_iStageSub) && (aaiTouch[i][1] >= m_iStageSub) && (aaiTouch[i][2] == iCreateCount))
                    {
                        this->StartDiamond(iCreateStart - 1u);
                        break;
                    }
                }
            }
        };

        const auto nCreateGenerateFunc = [&](const coreUint8 iType, const coreFloat fFactor, const coreFloat fOffset, const coreFloat fDelay)
        {
            for(coreUintW i = iCreateStart; i < MUSCUS_GENERATES; ++i)
            {
                const coreObject3D* pGenerate = this->GetGenerate(i);

                if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    iCreateStart = i + 1u;
                    this->EnableGenerate(i);

                    aiGenerateType[i] = iType;
                    avGenerateData[i] = coreVector2(fFactor, fOffset);
                    afGenerateTime[i] = -fDelay;

                    iCreateCount += 1u;
                    nTouchFunc();

                    return;
                }
            }

            WARN_IF(true) {}
        };

        const auto nMoveGenerateFunc = [](const coreSpline2* pRawPath, const coreVector2 vFactor, const coreVector2 vRawOffset, const coreFloat fTime)
        {
            const coreVector2 vPos = pRawPath->CalcPosition(FMOD(MAX(fTime, 0.0f), pRawPath->GetTotalDistance()));
            return coreVector3(((vPos * vFactor) + vRawOffset) * FOREGROUND_AREA, 0.0f);
        };

        const auto nTestAllGenerateFunc = [this](const coreBool bTest, const coreBool bAll)
        {
            for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
            {
                if(bAll || (this->GetGenerate(i)->GetAlpha() < 0.1f))
                {
                    this->TestGenerate(i, bTest);
                }
            }
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  8u, 19u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 20u, 31u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 32u, 43u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 68u, 83u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 84u, 99u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1, 44u, 67u)   // #
            else if(STAGE_SUB(9u))
            {
                nTestAllGenerateFunc(false, true);
                STAGE_DELAY_START_CLEAR

                if(!iDisableState) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, coreVector3(0.0f,0.0f,0.0f))
            }

            if((m_iStageSub == 1u) || (m_iStageSub == 2u) || (m_iStageSub == 4u) || (m_iStageSub == 5u) || (m_iStageSub == 6u) || (m_iStageSub == 7u) || (m_iStageSub == 8u))
            {
                iSpawnCount  = 0u;
                fSpawnOffset = (1.0f - fSpawn) / (fSpeed * fFreqFast);
            }

            if((m_iStageSub == 5u) || (m_iStageSub == 6u))
            {
                nTestAllGenerateFunc(false, false);
            }

            for(coreUintW i = 0u; i < ARRAY_SIZE(aaiTouch); ++i)
            {
                if(aaiTouch[i][0] == m_iStageSub)
                {
                    iCreateCount = 0u;
                    break;
                }
            }
        }

        fSpawn += TIME * (fSpeed * RCP(pPath1->GetTotalDistance() * 0.5f)) * ((m_iStageSub >= 6u) ? fFreqFast : fFreqSlow);
        if(fSpawn >= 1.0f)
        {
            fSpawn -= 1.0f;

            const cPlayer*  pTarget = g_pGame->FindPlayerDual(iSpawnTick % 2u);
            const coreFloat fOffset = (ROUND(((pTarget->GetPosition().x / FOREGROUND_AREA.x) / fStep) + 0.5f) - 0.5f) * fStep;

            if(m_iStageSub < 2u)
            {
                if(iSpawnTick < 1u)
                {
                    // #
                    nCreateGenerateFunc(0u, 1.0f, fOffset, 0.0f);
                }
                else
                {
                    //     #
                    // ##  #
                    if(iSpawnCount % 2u)
                    {
                        nCreateGenerateFunc(0u, 1.0f, fOffset,         0.0f);
                        nCreateGenerateFunc(0u, 1.0f, fOffset + fStep, 0.0f);
                    }
                    else
                    {
                        nCreateGenerateFunc(0u, 1.0f, fOffset, 0.0f);
                        nCreateGenerateFunc(0u, 1.0f, fOffset, fStepDelay);
                    }
                }
            }
            else if(m_iStageSub < 4u)
            {
                // ##   ##   #   ##
                // ##  ##   ###   ##  ####
                coreFloat fClamp;
                switch(iSpawnCount % 5u)
                {
                default: ASSERT(false)
                case 0u:
                    fClamp = CLAMP(fOffset, -3.5f * fStep, 2.5f * fStep);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp + fStep, 0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         fStepDelay);
                    nCreateGenerateFunc(0u, 1.0f, fClamp + fStep, fStepDelay);
                    break;
                case 1u:
                    fClamp = CLAMP(fOffset, -2.5f * fStep, 2.5f * fStep);
                    nCreateGenerateFunc(0u, 1.0f, fClamp - fStep, 0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         fStepDelay);
                    nCreateGenerateFunc(0u, 1.0f, fClamp + fStep, fStepDelay);
                    break;
                case 2u:
                    fClamp = CLAMP(fOffset, -2.5f * fStep, 2.5f * fStep);
                    nCreateGenerateFunc(0u, 1.0f, fClamp - fStep, 0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         fStepDelay);
                    nCreateGenerateFunc(0u, 1.0f, fClamp + fStep, 0.0f);
                    break;
                case 3u:
                    fClamp = CLAMP(fOffset, -2.5f * fStep, 2.5f * fStep);
                    nCreateGenerateFunc(0u, 1.0f, fClamp - fStep, fStepDelay);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         fStepDelay);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,         0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp + fStep, 0.0f);
                    break;
                case 4u:
                    fClamp = CLAMP(fOffset, -1.5f * fStep, 2.5f * fStep);
                    nCreateGenerateFunc(0u, 1.0f, fClamp - fStep * 2.0f, 0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp - fStep,        0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp,                0.0f);
                    nCreateGenerateFunc(0u, 1.0f, fClamp + fStep,        0.0f);
                    break;
                }
            }
            else if(m_iStageSub < 5u)
            {
                if(iSpawnCount == 0u)
                {
                    // ###  ###
                    for(coreUintW i = 0u; i < 8u; ++i)
                    {
                        if((i != 3u) && (i != 4u)) nCreateGenerateFunc(0u, 1.0f, fStep * (I_TO_F(i) - 3.5f), 0.0f);
                    }
                }
                else
                {
                    // 2 4 5 3
                    const coreUint8 iSide = (iSpawnCount % 2u) ? 3u : 1u;

                    coreUintW iHole;
                    switch(iSpawnCount % 4u)
                    {
                    default: ASSERT(false)
                    case 0u: iHole = 3u; break;
                    case 1u: iHole = 2u; break;
                    case 2u: iHole = 4u; break;
                    case 3u: iHole = 5u; break;
                    }

                    for(coreUintW i = 0u; i < 8u; ++i)
                    {
                        if(i != iHole) nCreateGenerateFunc(iSide, 1.0f, fStep * (I_TO_F(i) - 3.5f), 0.0f);
                    }
                }
            }
            else if(m_iStageSub < 6u)
            {
                // diagonal
                const coreUint8 iSide = 4u + (iSpawnCount % 4u);

                for(coreUintW i = 0u; i < 11u; ++i)
                {
                    if(i != 5u) nCreateGenerateFunc(iSide, 1.0f, fStep * (I_TO_F(i) - 5.0f), 0.0f);
                }
            }
            else if(m_iStageSub < 7u)
            {
                if(iSpawnCount > 0u)
                {
                    // 1122 1122
                    const coreUint8 iSide = (iSpawnCount % 2u) ? 0u : 2u;

                    for(coreUintW i = 0u; i < 4u; ++i)
                    {
                        nCreateGenerateFunc(0u, 1.0f, fStep * (I_TO_F(i + iSide + ((i >= 2u) ? 2u : 0u)) - 3.5f), 0.0f);
                    }
                }
            }
            else if(m_iStageSub < 8u)
            {
                // 12 12 12 12
                const coreUint8 iSide = (iSpawnCount % 2u) ? 1u : 0u;

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    nCreateGenerateFunc(0u, 1.0f, fStep * (I_TO_F(i * 2u + iSide) - 3.5f), 0.0f);
                }
            }
            else if((m_iStageSub == 8u) && !iSpawnCount)
            {
                // rotation
                for(coreUintW i = 0u; i < 11u; ++i)
                {
                    nCreateGenerateFunc(8u, 1.0f, 0.0f, 0.0f);
                }
            }

            #if 0

                // ramp
                if((iSpawnCount % 2u) == 0u)
                {
                    const coreFloat fSide = ((iSpawnCount % 4u) < 2u) ? -1.0f : 1.0f;

                    for(coreUintW i = 0u; i < 7u; ++i)
                    {
                        nCreateGenerateFunc(0u, 1.0f, fStep * (I_TO_F(i) - 3.5f) * fSide, fStepDelay * I_TO_F(i));
                    }
                }

            #endif

            iSpawnTick  += 1u;   // never reset
            iSpawnCount += 1u;   // reset on sub-stage
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_GREEN);

        if(m_iStageSub == 1u)
        {
            if(STAGE_BEGINNING)
            {
                pHelper->Resurrect(false);
                nCreateGenerateFunc(0u, 1.0f, 0.0f, 0.0f);
            }

            constexpr coreFloat fDelay = 0.6f;

            if(!HAS_FLAG(pHelper->GetStatus(), HELPER_STATUS_DEAD) && STAGE_TIME_AFTER(fDelay))
            {
                pHelper->SetPosition(coreVector3(LERP(-1.2f, 0.0f, m_fStageTime - fDelay) * FOREGROUND_AREA.x, 0.0f, 0.0f));

                if(STAGE_TIME_AFTER(1.0f + fDelay))
                {
                    pHelper->Kill(false);

                    this->BangGenerate(0u);

                    g_pSpecialEffects->MacroEruptionColorSmall(pHelper->GetPosition(), coreVector2(1.0f,0.0f), COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
                    g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
                }
            }
        }
        else if((m_iStageSub == 8u) && iSpawnCount)
        {
            fRotation += TIME * (0.5f*PI) * LERPH3(0.0f, 1.0f, MIN1(m_fStageSubTime * 0.1f));

            nTestAllGenerateFunc(STAGE_SUBTIME_AFTER(2.0f), true);
        }
        else if(m_iStageSub == 9u)
        {
            fRotation += TIME * (0.5f*PI) * LERPH3(1.0f, 0.0f, MIN1(m_fStageSubTime * 1.0f));

            for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
                this->ShowGenerate(i, 1.0f);

            if(STAGE_SUBTIME_AFTER(1.5f))
            {
                for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
                    this->DisableGenerate(i, true);

                g_pSpecialEffects->MacroExplosionColorBig(coreVector3(0.0f,0.0f,0.0f), COLOR_ENERGY_GREEN);
                g_pSpecialEffects->PlaySound(coreVector3(0.0f,0.0f,0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
                g_pSpecialEffects->PlaySound(coreVector3(0.0f,0.0f,0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_09);

                cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

                pBackground->GetHeadlight()->ResetFlicker();
                pBackground->SetEnableLightning(true);
                pBackground->SetEnableHeadlight(false);
                pBackground->FlashLightning();

                STAGE_DELAY_END
            }
        }

        const coreVector2 vRotaDir     = coreVector2::Direction(fRotation     * (g_pGame->IsEasy() ? 0.7f : 1.0f));
        const coreVector2 vPrevRotaDir = coreVector2::Direction(fPrevRotation * (g_pGame->IsEasy() ? 0.7f : 1.0f));

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 8u)
            {
                STAGE_LIFETIME(pEnemy, 0.8f, (i < 4u) ? 4.0f : 0.0f)

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f) * fStep * 2.0f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 4u) {}
                else if(i < 8u) pEnemy->Rotate180();
            }
            else if(i < 20u)
            {
                STAGE_LIFETIME(pEnemy, 0.6f, (2.6f/6.0f) * I_TO_F(i - 8u))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 8u) % 4u) - 1.5f) * fStep * 2.0f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
                if(i % 2u) pEnemy->InvertX();
            }
            else if(i < 68u)
            {
                STAGE_LIFETIME(pEnemy, 0.8f, (i < 44u) ? 1.5f : 3.0f)

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 20u) % 6u) - 2.5f) * fStep, 0.05f * ((i % 2u) ? -1.0f : 1.0f) * ((i < 44u) ? -1.0f : 1.0f));

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 26u) pEnemy->Rotate90 ();
                else if(i < 32u) pEnemy->Rotate270();
                else if(i < 38u) {}
                else if(i < 44u) pEnemy->Rotate180();
                else if(i < 50u) {}
                else if(i < 56u) pEnemy->Rotate90 ();
                else if(i < 62u) pEnemy->Rotate180();
                else if(i < 68u) pEnemy->Rotate270();
            }
            else if(i < 100u)
            {
                STAGE_LIFETIME(pEnemy, fSpeed, fSpawnOffset + (I_TO_F((((i - 68u) / 4u) % 4u) + ((i < 84u) ? 1u : 0u)) * (1.3f / fFreqFast)))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((((i - 68u) / 4u) % 2u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(((i < 84u) ? (I_TO_F(((i - 66u) % 4u) + ((((i - 66u) / 2u) % 2u) * 2u)) - 1.5f) : (I_TO_F(((i - 84u) % 4u) * 2u) - 3.5f)) * fStep, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
        });

        coreUintW iRotaIndex   = 0u;
        coreUintW iCenterIndex = 0u;

        for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
        {
            coreObject3D* pGenerate = this->GetGenerate(i);
            if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            if(this->IsGenerateDiamond(i) && this->IsGenerateHit(i))
            {
                this->TestGenerate(i, false);

                if(++iTouchState >= ARRAY_SIZE(aaiTouch))
                {
                    STAGE_BADGE(0u, BADGE_EASY, pGenerate->GetPosition())
                }
                else
                {
                    g_pGame->GetCombatText()->DrawProgress(iTouchState, ARRAY_SIZE(aaiTouch), pGenerate->GetPosition());
                    g_pSpecialEffects->PlaySound(pGenerate->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iTouchState, ARRAY_SIZE(aaiTouch)), SOUND_ITEM_COLLECT);
                }
            }

            if(aiGenerateType[i] < 8u)
            {
                // 7 0 4
                // 3   1
                // 6 2 5
                const coreBool  bDiagonal = (aiGenerateType[i] >= 4u);
                const coreFloat fLength   = bDiagonal ? SQRT2 : 1.0f;
                const coreFloat fTime     = afGenerateTime[i] * fSpeed * RCP(fLength);

                const coreVector2 vFactor = coreVector2(avGenerateData[i].x, fLength);
                const coreVector2 vOffset = coreVector2(avGenerateData[i].y, 0.0f);

                pGenerate->SetPosition (nMoveGenerateFunc(pPath1, vFactor, vOffset * vFactor, fTime));
                pGenerate->SetDirection(coreVector3(bDiagonal ? 1.0f : 0.0f, 1.0f, 0.0f).Normalized());

                switch(aiGenerateType[i] % 4u)
                {
                default: ASSERT(false)
                case 0u: break;
                case 1u: pGenerate->SetPosition( pGenerate->GetPosition().RotatedZ90()); break;
                case 2u: pGenerate->SetPosition(-pGenerate->GetPosition());              break;
                case 3u: pGenerate->SetPosition(-pGenerate->GetPosition().RotatedZ90()); break;
                }
                if(bDiagonal) pGenerate->SetPosition(pGenerate->GetPosition().RotatedZ45());

                if(fTime < ((g_pGame->IsEasy() && HAS_FLAG(pHelper->GetStatus(), HELPER_STATUS_DEAD)) ? 1.1f : 0.6f) * fLength)
                {
                    this->ShowGenerate(i, 0.1f);
                }
                else if(fTime >= pPath1->GetTotalDistance())
                {
                    iCreateStart = MIN(iCreateStart, i);
                    this->DisableGenerate(i, false);
                }
            }
            else if(aiGenerateType[i] == 8u)
            {
                const coreFloat fTime   = CLAMP01(afGenerateTime[i] * 0.5f - ABS(I_TO_F(iRotaIndex - 5u)) * 0.5f);
                const coreFloat fOffset = (I_TO_F(iRotaIndex - 5u) * fStep + LERPB(1.5f, 0.0f, fTime) * ((iRotaIndex < 5u) ? -1.0f : 1.0f)) * ((m_iStageSub == 9u) ? BLENDB(CLAMP01(1.5f - m_fStageSubTime)) : ((m_iStageSub > 9u) ? 0.0f : 1.0f));

                pGenerate->SetPosition (coreVector3(vRotaDir * fOffset * FOREGROUND_AREA, 0.0f));
                pGenerate->SetDirection(coreVector3(vRotaDir,                             0.0f));

                if(!fTime)           this->ShowGenerate(i, 2.0f);
                if(iRotaIndex == 5u) this->ShowGenerate(i, 1.0f);
                if(iRotaIndex == 5u) pGenerate->SetPosition(pGenerate->GetPosition().RotatedZ90());
                if(iRotaIndex == 5u) iCenterIndex = i;

                iRotaIndex += 1u;
            }

            if(g_pForeground->IsVisiblePoint(pGenerate->GetPosition().xy()) && !this->IsGenerateVisible(i))
            {
                iDisableState = 1u;
            }

            afGenerateTime[i] += 1.0f * TIME;   // after update, to align with enemies
        }

        if(g_pGame->IsTask() && (m_iStageSub == 8u))
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                ASSERT(GAME_PLAYERS*2u <= sizeof(iAroundState)*8u)

                const coreObject3D* pCenter = this->GetGenerate(iCenterIndex);

                const coreVector2 vDiffOld = pPlayer->GetOldPos()        - pCenter->GetPosition().xy();
                const coreVector2 vDiffCur = pPlayer->GetPosition().xy() - pCenter->GetPosition().xy();

                const coreBool bSide = (coreVector2::Dot(vDiffCur, vRotaDir) > 0.0f);

                const coreFloat fDotOld = coreVector2::Dot(vDiffOld, vPrevRotaDir.Rotated90());
                const coreFloat fDotCur = coreVector2::Dot(vDiffCur, vRotaDir    .Rotated90());

                if( bSide && (fDotCur >  0.0f) && (fDotOld <= 0.0f)) ADD_BIT(iAroundState, i * 2u + 0u)
                if(!bSide && (fDotCur <= 0.0f) && (fDotOld >  0.0f)) ADD_BIT(iAroundState, i * 2u + 1u)

                if(GET_BITVALUE(iAroundState, 2u, i * 2u) == BITLINE(2u))
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, pPlayer->GetPosition())
                }
            });
        }

        STAGE_WAVE(1u, "7-2", {60.0f, 90.0f, 120.0f, 150.0f, 300.0f})   // ACHTUNDDREISSIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
            this->DisableGenerate(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

        pBackground->GetHeadlight()->ResetFlicker();
        pBackground->SetEnableLightning(true);
        pBackground->SetEnableHeadlight(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // collect lots of shiny pills (pacman, snake)
    // idea: enemy has orb shield or tail you need to collect
    // idea: bouncing pearls (linear, not gravity, like pong)
    // in the chain-stage, balls need to be far enough apart to not accidentally spawn ball into player
    // enemy should die with the impact of the last ball, not require the player to attack, to remove confusion and delay
    // ball animations need to be in a sequential pattern
    // the combination of collecting stuff and shooting at stuff was a disaster (because of the requirement to collect everything to advance), it was possible to give collecting a meaning, by causing "changes" (e.g. enemy invincibility), but it felt very sluggish (A, B, A, B), also enemies dropping the balls cause players to move into enemies while killing (for speed) which felt forced
    // the first wave of pearls cannot be evaded, to show that they can be collected
    // enemy flying in circle should not be too fast, as otherwise you cannot get out anymore behind him
    // bouncy balls sollten nicht zu niedrig bouncen (sonst zu leicht), und konsistente X geschwindigkeit haben (sonst zu schwer, bräuchten alle andere bewegungs-geschwindigkeit vom spieler)
    // von unten kommende bouncy balls sollten nicht zu nah am rand sein, schaut besser aus und is angenehmer zu fangen wenn man einen (am rand) verpasst
    // geschoss-abstände bei den kontinuierlichen angriffen sollten breit genug sein, damit man durch kann, aber nicht zu breit um gefährlich/blockierend zu bleiben, vor allem beim folgenden gegner wichtig
    // TASK: try to destroy the enemy with your normal weapon / attack enemy when his shield is down, until his health is gone
    // TASK: find all hidden pearls
    // ACHIEVEMENT: strike the enemy only while its shield is up
    // TODO 1: hardmode: two enemies
    // TODO 1: hardmode: fake pearls inbetween/mixed (blocky)
    // TODO 1: background side-movement might be easier for direct attack from top
    // TODO 1: striking pearls sollten sich strecken (direction anpassen + Y size größer + leichte XY size kleiner) (schon mit partikeln gemacht ?)
    // TODO 1: striking attack sollte weiter weggehn wenn sie näher bei target sind, vielleicht beschleunigt das mehr
    // TODO 1: maybe also update strike-spline target tangent in real-time (e.g. when enemy moves into strike the animation gets slower)
    // TODO 5: badge: fang den blauen flummi in der flummi phase (verchwindet bei X), oder der vom boss wie ein geschoss erzeugt wird
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        constexpr coreUintW iHiddenFirst = 31u;
        constexpr coreUintW iHiddenCount = 3u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.4f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2( 0.5f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(-0.5f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.6f);
                pEnemy->Configure(400, 0u, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_IMMORTAL);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(15u + MUSCUS_PEARLS * 3u)
            STAGE_GET_FLOAT      (fShieldDown)
            STAGE_GET_FLOAT      (fShootTime)
            STAGE_GET_FLOAT      (fEnemyTime)
            STAGE_GET_FLOAT      (fEnemyRota, fEnemyRota = 1.0f*PI)
            STAGE_GET_FLOAT      (fEnemySide)
            STAGE_GET_VEC2       (vEnemyOldPos)
            STAGE_GET_VEC2       (vHitForce)
            STAGE_GET_VEC2       (vHitOffset)
            STAGE_GET_UINT       (iPearlSpawn)
            STAGE_GET_UINT       (iHiddenFound)
            STAGE_GET_UINT       (iUnshielded)
            STAGE_GET_UINT       (iFlashState)
            STAGE_GET_FLOAT_ARRAY(afPearlTime, MUSCUS_PEARLS)
            STAGE_GET_VEC2_ARRAY (avPearlMove, MUSCUS_PEARLS)
        STAGE_GET_END

        cEnemy* pKnight = pSquad1->GetEnemy(0u);

        const auto nCreatePearlWaveFunc = [this](const coreUintW iCount)
        {
            for(coreUintW i = 0u; i < iCount; ++i)
            {
                this->DisablePearl(i, false);   // cancel animation for last few pearls
                this->EnablePearl (i);
            }
        };

        const auto nMovePearlFunc = [](const coreSpline2* pRawPath, const coreVector2 vFactor, const coreVector2 vRawOffset, const coreFloat fTime)
        {
            const coreVector2 vPos = pRawPath->CalcPosition(FMOD(MAX(fTime, 0.0f), pRawPath->GetTotalDistance()));
            return coreVector3(((vPos * vFactor) + vRawOffset) * FOREGROUND_AREA, 0.0f);
        };

        const auto nSetShieldFunc = [&](const coreBool bActive)
        {
            fShieldDown = bActive ? 0.0f : 4.0f;

            if(bActive)
            {
                pKnight->AddStatus(ENEMY_STATUS_INVINCIBLE);

                g_pSpecialEffects->CreateWhirlColor(pKnight->GetPosition(), SPECIAL_WHIRL_SMALL, COLOR_ENERGY_BLUE);
                g_pSpecialEffects->PlaySound(pKnight->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_SWIPE);
            }
            else
            {
                pKnight->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                g_pSpecialEffects->CreateWhirlColor(pKnight->GetPosition(), SPECIAL_WHIRL_SMALL, COLOR_ENERGY_BLUE);
                g_pSpecialEffects->PlaySound(pKnight->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_SWIPE_3);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }
        };

        if(!(m_iPearlActive & ~(BITLINE(iHiddenCount) << iHiddenFirst)))
        {
                 if(STAGE_SUB( 1u)) nCreatePearlWaveFunc(14u);
            else if(STAGE_SUB( 2u)) nCreatePearlWaveFunc( 5u);
            else if(STAGE_SUB( 3u)) nCreatePearlWaveFunc( 5u);
            else if(STAGE_SUB( 4u)) nCreatePearlWaveFunc(16u);
            else if(STAGE_SUB( 5u)) nCreatePearlWaveFunc(16u);
            else if(STAGE_SUB( 6u)) nCreatePearlWaveFunc( 1u);
            else if(STAGE_SUB( 7u)) nCreatePearlWaveFunc(20u);
            else if(STAGE_SUB( 8u)) nCreatePearlWaveFunc(12u);
            else if(STAGE_SUB( 9u)) nCreatePearlWaveFunc(20u);
            else if(STAGE_SUB(10u)) nCreatePearlWaveFunc(18u);
            else if(STAGE_SUB(11u)) nCreatePearlWaveFunc(12u);
            else if(STAGE_SUB(12u))
            {
                pKnight->Kill(true);
                if(!iUnshielded) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pKnight->GetPosition())
            }

            if((m_iStageSub == 2u) || (m_iStageSub == 6u) || (m_iStageSub == 7u) || (m_iStageSub == 8u) || (m_iStageSub == 10u) || (m_iStageSub == 11u))
            {
                fShootTime = 0.0f;
                fEnemyTime = 0.0f;
            }

            if(m_iStageSub == 8u)
            {
                fEnemySide = pKnight->GetPosition().x;
            }

            if(m_iStageSub == 10u)
            {
                g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,1.0f), 10.0f);
            }

            vEnemyOldPos = pKnight->GetPosition().xy() / FOREGROUND_AREA;

            std::memset(afPearlTime, 0, sizeof(coreFloat)   * MUSCUS_PEARLS);
            std::memset(avPearlMove, 0, sizeof(coreVector2) * MUSCUS_PEARLS);
        }

        const coreFloat fOldShootTime = fShootTime;
        fShootTime += 1.0f * TIME;

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW);

        if((m_iStageSub == 3u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
        }

        if(STAGE_BEGINNING2)
        {
            if(g_pGame->IsTask() && (m_iStageSub == 6u))
            {
                for(coreUintW i = 0u; i < iHiddenCount; ++i)
                {
                    this->EnablePearl(i + iHiddenFirst);
                    ADD_BIT(m_iPearlHidden, i + iHiddenFirst)
                }
            }
            else if(m_iStageSub == 7u)
            {
                for(coreUintW i = 0u; i < iHiddenCount; ++i)
                {
                    this->DisablePearl(i + iHiddenFirst, false);
                }
            }
        }

        if(!pKnight->HasStatus(ENEMY_STATUS_DEAD) && (pKnight->GetCurHealth() > 0))
        {
            if(FRACT(m_fStageTime * 0.05f) < FRACT(m_fStageTimeBefore * 0.05f))
            {
                nSetShieldFunc(false);
            }

            if(fShieldDown)
            {
                fShieldDown = MAX0(fShieldDown - 1.0f * TIME);
                if(!fShieldDown) nSetShieldFunc(true);
            }
        }

        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        {
            coreObject3D* pPearl = this->GetPearl(i);
            if(!this->IsPearlActive(i)) continue;

            coreFloat&   fPearlTime = afPearlTime[i];
            coreVector2& vPearlMove = avPearlMove[i];

            if(m_iStageSub == 1u)
            {
                const coreVector2 vPos = coreVector2((I_TO_F(i) - 6.5f) * 0.15f, SIN(fPearlTime * 1.3f + I_TO_F(i) * 0.15f) + LERPB(1.2f, 0.0f, MIN(fPearlTime * 0.5f, 1.0f)));

                pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
            else if(m_iStageSub <= 5u)
            {
                const coreSpline2* pPath   = (m_iStageSub <= 3u) ? pPath3 : pPath2;
                const coreVector2  vFactor = coreVector2((m_iStageSub == 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2  vOffset = coreVector2((m_iStageSub <= 3u) ? 0.0f : ((m_iStageSub == 4u) ? -0.5f : (SIN(I_TO_F(i) / 16.0f * (4.0f*PI)) * 0.2f + 0.5f)), 0.0f);
                const coreFloat    fTime   = fPearlTime * 1.2f - I_TO_F(i) * 0.15f;

                pPearl->SetPosition(nMovePearlFunc(pPath, vFactor, vOffset * vFactor, fTime));

                     if(m_iStageSub == 4u) pPearl->SetPosition(-pPearl->GetPosition().RotatedZ90());
                else if(m_iStageSub == 5u) pPearl->SetPosition( pPearl->GetPosition().RotatedZ90());
            }
            else if(m_iStageSub == 6u)
            {
                coreVector2 vPos;
                switch(i)
                {
                default: ASSERT(false)
                case  0u: vPos = coreVector2( 2.0f, 0.0f); break;
                case  1u: vPos = coreVector2( 3.0f, 0.0f); break;
                case  2u: vPos = coreVector2( 4.0f, 0.0f); break;
                case  3u: vPos = coreVector2( 4.0f,-1.0f); break;
                case  4u: vPos = coreVector2( 4.0f,-2.0f); break;
                case  5u: vPos = coreVector2( 3.0f,-2.0f); break;
                case  6u: vPos = coreVector2( 2.0f,-2.0f); break;
                case  7u: vPos = coreVector2( 1.0f,-2.0f); break;
                case  8u: vPos = coreVector2( 0.0f,-2.0f); break;
                case  9u: vPos = coreVector2( 0.0f,-3.0f); break;
                case 10u: vPos = coreVector2( 0.0f,-4.0f); break;
                case 11u: vPos = coreVector2(-1.0f,-4.0f); break;
                case 12u: vPos = coreVector2(-2.0f,-4.0f); break;
                case 13u: vPos = coreVector2(-2.0f,-3.0f); break;
                case 14u: vPos = coreVector2(-2.0f,-2.0f); break;
                case 15u: vPos = coreVector2(-2.0f,-1.0f); break;
                case 16u: vPos = coreVector2(-2.0f, 0.0f); break;
                case 17u: vPos = coreVector2(-3.0f, 0.0f); break;
                case 18u: vPos = coreVector2(-4.0f, 0.0f); break;
                case 19u: vPos = coreVector2(-4.0f, 1.0f); break;
                case 20u: vPos = coreVector2(-4.0f, 2.0f); break;
                case 21u: vPos = coreVector2(-3.0f, 2.0f); break;
                case 22u: vPos = coreVector2(-2.0f, 2.0f); break;
                case 23u: vPos = coreVector2(-1.0f, 2.0f); break;
                case 24u: vPos = coreVector2( 0.0f, 2.0f); break;
                case 25u: vPos = coreVector2( 1.0f, 2.0f); break;
                case 26u: vPos = coreVector2( 2.0f, 2.0f); break;
                case 27u: vPos = coreVector2( 1.0f, 2.0f); break;
                case 28u: vPos = coreVector2( 0.0f, 2.0f); break;
                case 29u: vPos = coreVector2( 0.0f, 1.0f); break;
                case 30u: vPos = coreVector2( 0.0f, 0.0f); break;

                case 31u: vPos = coreVector2( 2.0f,-4.0f); break; STATIC_ASSERT((iHiddenFirst == 31u) && (iHiddenCount == 3u))
                case 32u: vPos = coreVector2(-4.0f,-2.0f); break;
                case 33u: vPos = coreVector2( 0.0f, 4.0f); break;
                }
                vPos *= 0.2f;

                pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
            else if(m_iStageSub == 7u)
            {
                const coreVector2 vPos = coreVector2::Direction((fPearlTime + I_TO_F(i) * (0.2f*PI)) * ((i < 10u) ? -1.0f : 1.0f)) * (LERPS(1.4f, 0.5f, MIN(fPearlTime * 0.5f, 1.0f)) + ((i < 10u) ? 0.2f : 0.0f));

                pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
            else if(m_iStageSub <= 9u)
            {
                if(m_iStageSub == 8u)
                {
                    if(!fPearlTime)
                    {
                        coreUint8 iHeight;
                        switch(i % 6u)
                        {
                        default: ASSERT(false)
                        case 0u: iHeight = 1u; break;
                        case 1u: iHeight = 4u; break;
                        case 2u: iHeight = 2u; break;
                        case 3u: iHeight = 5u; break;
                        case 4u: iHeight = 0u; break;
                        case 5u: iHeight = 3u; break;
                        }

                        pPearl->SetPosition(coreVector3((I_TO_F(i) - 5.5f) * 0.15f, -1.2f, 0.0f) * FOREGROUND_AREA3);
                        vPearlMove = coreVector2(0.0f, I_TO_F(iHeight) * 0.15f + 1.0f);
                    }
                }
                else
                {
                    if(fPearlTime <= I_TO_F(i) * 0.1f)
                    {
                        const coreFloat fSide   = ((i % 8u) < 4u) ? -1.0f : 1.0f;
                        const coreFloat fHeight = I_TO_F(i / 4u) * 0.4f - 0.6f;

                        pPearl->SetPosition(coreVector3(-1.2f * fSide, fHeight, 0.0f) * FOREGROUND_AREA3);
                        vPearlMove = coreVector2(1.0f * fSide, 0.0f);
                    }
                }

                coreVector2 vCurPos  = pPearl->GetPosition().xy();
                coreVector2 vCurMove = vPearlMove;

                vCurMove.y -= 1.0f * TIME;
                vCurPos    += vCurMove * (60.0f * TIME);

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurMove.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.1f); vCurMove.x =  ABS(vCurMove.x);}
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurMove.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.1f); vCurMove.x = -ABS(vCurMove.x);}
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurMove.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.1f); vCurMove.y =  ABS(vCurMove.y);}
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurMove.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.1f); vCurMove.y = -ABS(vCurMove.y);}

                pPearl->SetPosition(coreVector3(vCurPos, 0.0f));
                vPearlMove = vCurMove;
            }
            else if(m_iStageSub == 10u)
            {
                const coreVector2 vPos = coreVector2::Direction(I_TO_F(i / 6u) * ((2.0f/3.0f)*PI) - fPearlTime * 1.5f + I_TO_F(i % 6u) * 0.12f) * (I_TO_F(i % 6u) * 0.15f + LERPB(1.2f * SQRT2, 0.2f, MIN1(fPearlTime * 0.5f)));

                pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
            else if(m_iStageSub == 11u)
            {
                const coreFloat fTime = (fPearlTime - (1.0f/0.6f)) * 0.5f - I_TO_F(i % 4u) * 0.5f;

                if(fTime < 0.0f)
                {
                    pPearl->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    pPearl->SetAlpha   (0.0f);   // blend-in
                }
                else
                {
                    const coreVector2 vPos = coreVector2::Direction(I_TO_F(i) * (2.0f*PI) / 12.0f) * LERPS(0.2f, 1.0f, fTime) * BLENDH3(CLAMP01(fTime));

                    pPearl->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }
            }

            fPearlTime += 1.0f * TIME;
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            for(coreUintW j = 0u; j < MUSCUS_PEARLS; ++j)
            {
                const coreObject3D* pPearl = this->GetPearl(j);
                if(!this->IsPearlActive(j)) continue;

                const coreVector2 vDiff = pPlayer->GetPosition().xy() - pPearl->GetPosition().xy();
                if(vDiff.LengthSq() < POW2(5.0f))
                {
                    const coreBool bHidden = HAS_BIT(m_iPearlHidden, j);
                    if(bHidden)
                    {
                        if(++iHiddenFound >= iHiddenCount)
                        {
                            STAGE_BADGE(1u, BADGE_NORMAL, pPearl->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawProgress(iHiddenFound, iHiddenCount, pPearl->GetPosition());
                            g_pSpecialEffects->PlaySound(pPearl->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iHiddenFound, iHiddenCount), SOUND_ITEM_COLLECT);
                        }
                    }

                    pPlayer->GetScoreTable()->RefreshCooldown();

                    this->StrikeAttack(j, pPlayer, pKnight);
                    g_pSpecialEffects->PlaySound(pPearl->GetPosition(), 1.0f, this->RetrievePearlPitch(), SOUND_EFFECT_PEARL);

                    if((m_iStageSub == 6u) && !bHidden)
                    {
                        iPearlSpawn += 1u;
                        if(iPearlSpawn < 31u) this->EnablePearl(iPearlSpawn);
                    }
                }
            }
        });

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            constexpr coreUintW iPearlIndex = 3u;

            pHelper->SetPosition(this->GetPearl(iPearlIndex)->GetPosition() + coreVector3(3.0f,0.0f,0.0f));
            if(!this->IsPearlActive(iPearlIndex)) pHelper->Kill(false);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            fEnemyTime = MIN(fEnemyTime + 0.6f * TIME, 1.0f);

            if(m_iStageSub == 1u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(m_iStageSub <= 7u)
            {
                coreVector2 vPos, vDir;

                if(m_iStageSub <= 5u)
                {
                    vPos = LERPS(coreVector2(0.0f,0.9f), coreVector2(-0.9f,0.9f), fEnemyTime);
                    vDir = coreVector2::Direction(LERPS(-1.0f*PI, 1.25f*PI, fEnemyTime));
                }
                else if(m_iStageSub == 6u)
                {
                    vPos = LERPS(coreVector2(-0.9f,0.9f), coreVector2(0.9f,0.9f), fEnemyTime);
                    vDir = coreVector2::Direction(LERPS(1.25f*PI, -3.25f*PI, fEnemyTime));
                }
                else
                {
                    vPos = LERPS(coreVector2(0.9f,0.9f), coreVector2(0.0f,0.9f), fEnemyTime);
                    vDir = coreVector2::Direction(LERPS(-3.25f*PI, -1.0f*PI, fEnemyTime));
                }

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));

                g_pEnvironment->SetTargetDirectionNow(vPos.Normalized());
            }
            else if(m_iStageSub <= 9u)
            {
                const cPlayer*  pPlayer = pEnemy->NearestPlayerDual((m_iStageSub == 8u) ? 0u : 1u);
                const coreFloat fDiff   = pPlayer->GetPosition().x - fEnemySide;
                const coreFloat fSpeed  = LERPH3(0.0f, 3.0f, fEnemyTime);

                const coreVector2 vPos = coreVector2(fEnemySide + fDiff * (fSpeed * TIME), 0.9f * FOREGROUND_AREA.y);
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                pEnemy->SetPosition (coreVector3(vPos, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir, 0.0f));

                g_pEnvironment->SetTargetDirectionNow(coreVector2(vPos.x, FOREGROUND_AREA.y * 6.0f).Normalized());

                fEnemySide = vPos.x;
            }
            else if(m_iStageSub <= 12u)
            {
                coreVector2 vPos, vDir;

                fEnemyRota += 0.8f * TIME;

                if(m_iStageSub == 10u)
                {
                    vDir = coreVector2::Direction(fEnemyRota);
                    vPos = LERPH3(vEnemyOldPos, -vDir * LERPH3(0.9f, 0.7f, fEnemyTime), fEnemyTime);
                }
                else
                {
                    vDir = coreVector2::Direction(fEnemyRota);
                    vPos = -vDir * LERPH3(0.7f, 0.0f, fEnemyTime);
                }

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }

            if(fEnemyTime >= 0.5f)
            {
                if(m_iStageSub == 1u)
                {
                }
                else if(m_iStageSub <= 5u)
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, g_pGame->IsEasy() ? 0.4f : 0.7f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = pEnemy->AimAtPlayerDual(s_iTick % 2u).Angle();

                        for(coreUintW j = 5u; j--; )
                        {
                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                        }

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 10u, COLOR_ENERGY_CYAN);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
                else if(m_iStageSub == 6u)
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, g_pGame->IsEasy() ? 0.4f : 0.7f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = pEnemy->AimAtPlayerDual(s_iTick % 2u).Angle();

                        for(coreUintW j = 20u; j--; )
                        {
                            if((s_iTick & 0x01u) == ((j / 2u) & 0x01u)) continue;

                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 9.5f) * 5.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                        }

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 10u, COLOR_ENERGY_CYAN);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
                else if(m_iStageSub == 7u)
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, 16.0f, 0.0f) && ((s_iTick % 16u) < (g_pGame->IsEasy() ? 4u : 8u)))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual((s_iTick / 16u) % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_CYAN);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
                else if(m_iStageSub <= 9u)
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, 16.0f, 0.0f) && ((s_iTick % 8u) < (g_pGame->IsEasy() ? 2u : 4u)))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_CYAN);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
                else if(m_iStageSub == 10u)
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, 16.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = g_pGame->IsEasy() ? pEnemy->GetDirection().xy() : pEnemy->AimAtPlayerDual((s_iTick / 4u) % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_CYAN);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
                else if(m_iStageSub == 11u)
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, g_pGame->IsEasy() ? 0.5f : 1.0f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = 0.0f;

                        for(coreUintW j = 40u; j--; )
                        {
                            if((s_iTick & 0x01u) == ((j / 4u) & 0x01u)) continue;

                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos,  vDir)->ChangeSize(1.6f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, -vDir)->ChangeSize(1.6f);
                        }

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 10u, COLOR_ENERGY_CYAN);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
            }

            const coreUint64 iStrikeState = this->GetStrikeState();
            if(iStrikeState)
            {
                for(coreUintW j = 0u; j < MUSCUS_PEARLS; ++j)
                {
                    if(HAS_BIT(iStrikeState, j))
                    {
                        this->AddExtraScore(this->GetStrikePlayer(j), 40u, pEnemy->GetPosition());
                        vHitForce += this->GetStrikeDir(j) * 3.0f;

                        if(fShieldDown) iUnshielded += 1u;

                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.2f, SOUND_EFFECT_DUST);
                    }
                }
            }

            vHitOffset = LERP(vHitOffset, vHitForce, 30.0f * TIME);
            vHitForce *= FrictionFactor(12.0f);

            pEnemy->SetPosition   (pEnemy->GetPosition() + coreVector3(vHitOffset, 0.0f));   // after enemy attack
            pEnemy->SetOrientation(coreVector3(vHitOffset * 0.3f, 1.0f).Normalized());

            if(g_pGame->IsTask() && pEnemy->ReachedDeath())
            {
                STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())

                nSetShieldFunc(true);

                g_pSpecialEffects->MacroExplosionDarkSmall(pEnemy->GetPosition());
                g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_03);
            }
        });

        cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

        const coreFloat fAlpha  = pBackground->GetLightningAlpha ();
        const coreBool  bStatus = pBackground->GetLightningStatus();

             if((iFlashState % 2u) == 0u &&  bStatus) iFlashState += 1u;
        else if((iFlashState % 2u) == 1u && !bStatus) iFlashState += 1u;

        if(m_Geminga.HasStatus(ENEMY_STATUS_DEAD) && (iFlashState == 5u))
        {
            m_Geminga.ResurrectIntro();
        }
        else if(!m_Geminga.HasStatus(ENEMY_STATUS_DEAD) && (iFlashState == 6u))
        {
            m_Geminga.Kill(false);
            m_Geminga.SetAlpha(1.0f);
        }

        if(!m_Geminga.HasStatus(ENEMY_STATUS_DEAD))
        {
            m_Geminga.SetAlpha(fAlpha * 0.1f);
        }

        STAGE_WAVE(2u, "7-3", {60.0f, 90.0f, 120.0f, 150.0f, 300.0f})   // NEUNUNDDREISSIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        g_pGame->GetShieldManager()->SetActive(true);

        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
            this->DisablePearl(i, false);

        m_Geminga.Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // trade places with others
    // disable bullets of attacking enemies when jumping into them, otherwise unavoidable
    // das design kann/muss die position des spielers kontrollieren
    // first enemies should be far enough away to introduce teleport
    // enemies can be in a line to show multi-jump
    // high speed killing with correct rotation -> COOP (double-ended left right, you can start from each side)
    // zu komplexe highspeed-gruppen hatten schlechtes feeling, max zwei rotations-richtungen
    // in highspeed-gruppe gegner müssen weit genug auseinander sein (zur seite), um nicht unabsichtlich mehrere hintereinander anzugreifen
    // vor zweiter highspeed-gruppe, gegner müssen so angeordnet sein, dass man in einer guten ecke startet
    // some shooting enemies are near each other, to make teleport risky
    // first wave in multi-wave group (from same direction) should not start with enemies in the line along the previous group
    // enemies in multi-wave group should be behind bullets, to give the player enough room to react after a teleportation
    // in multi-wave group there should be no two enemies in the same column
    // die bewegenden gruppen sind wichtig um nicht nur starr einen fixen pfad zu folgen, sondern auch reaktions-geschick zu zeigen
    // der zweite trail ist so rotiert, dass der erste gegner der nächsten gruppe nicht zu nah am spieler ist
    // wenn transformiert mit gefächertem angriff, sollten nicht alle gegner von einer richtung oder entlang einer linie fliegen, sonst sterben alle sofort
    // bei abwechselnden wellen, kann es vorkommen (1mal) dass gegner nicht sofort angegriffen werden können, weil die welle noch an ihnen vorbeiziehen muss, das ist ok und verlangt vom spieler zeitliche präzision
    // der versteckte trail darf nicht sofort wieder nach unten gehen, wo die letzte welle noch entlang fliegt
    // der versteckte trail muss mit 1 gegner anfangen, der den start vorgibt, danach immer mit zwei gegnern um die geschwindigkeit des spielers zu erhöhen (wie bei kirby fast-food mini-game)
    // spawnende trail gegner dürfen niemals mit spieler kollidieren
    // es sollten immer min 2 gegner bei den wave gruppen für coop sein
    // in der multi-wave gruppe sollten manche gegner auf einer anderen höhe sein, damit man nicht schnell die gesamte gruppe zerstören kann wenn man sich schnell dreht
    // TASK: teleport 5 times for only a very short distance
    // TASK: jump over 3 lines at once
    // TASK EXTRA: destroy all quick enemies
    // ACHIEVEMENT: travel a large total distance through teleportation (3332.5 normal, 6419.2 exaggerated)
    // TODO 1: hardmode: all cinder enemies attack after teleportation, or teleportation attacks you
    // TODO 1: hardmode: something dangerous will be left behind after teleportation
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreUintW iNumData    = 16u;
        constexpr coreUintW iShortCount = 5u;
        constexpr coreUintW iLineCount  = 3u;

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
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 160u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure((i < 96u) ? 4 : 8, 0u, COLOR_SHIP_PURPLE);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST_PLAYER);
            });

            const auto nInitFunc = [&](const coreUintW iIndex, const coreVector2 vPos)
            {
                pSquad1->GetEnemy(iIndex)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            };

            nInitFunc(12u, coreVector2(-0.7f,-0.7f));
            nInitFunc(13u, coreVector2(-0.7f, 0.3f));
            nInitFunc(14u, coreVector2( 0.7f, 0.3f));
            nInitFunc(15u, coreVector2( 0.7f,-0.3f));
            nInitFunc(16u, coreVector2(-0.3f,-0.3f));
            nInitFunc(17u, coreVector2(-0.3f, 0.7f));
            nInitFunc(18u, coreVector2( 0.3f, 0.7f));
            nInitFunc(19u, coreVector2( 0.3f,-0.7f));

            nInitFunc(20u, coreVector2( 0.7f,-0.7f));
            nInitFunc(21u, coreVector2( 0.7f, 0.0f));
            nInitFunc(22u, coreVector2(-0.7f, 0.0f));
            nInitFunc(23u, coreVector2(-0.7f, 0.7f));
            nInitFunc(24u, coreVector2( 0.5f, 0.7f));
            nInitFunc(25u, coreVector2( 0.5f, 0.2f));
            nInitFunc(26u, coreVector2(-0.3f, 0.2f));
            nInitFunc(27u, coreVector2(-0.3f, 0.5f));
            nInitFunc(28u, coreVector2(-0.0f, 0.5f));
            nInitFunc(29u, coreVector2(-0.0f,-0.5f));
            nInitFunc(30u, coreVector2( 0.3f,-0.5f));
            nInitFunc(31u, coreVector2( 0.3f,-0.2f));
            nInitFunc(32u, coreVector2(-0.5f,-0.2f));
            nInitFunc(33u, coreVector2(-0.5f,-0.7f));

            nInitFunc(34u, coreVector2( 0.0f, 0.7f));
            nInitFunc(35u, coreVector2(-0.7f, 0.7f));
            nInitFunc(36u, coreVector2(-0.7f, 0.3f));
            nInitFunc(37u, coreVector2( 0.3f, 0.3f));
            nInitFunc(38u, coreVector2( 0.3f, 0.7f));
            nInitFunc(39u, coreVector2( 0.7f, 0.7f));
            nInitFunc(40u, coreVector2( 0.7f, 0.0f));
            nInitFunc(41u, coreVector2(-0.7f, 0.0f));
            nInitFunc(42u, coreVector2(-0.7f,-0.7f));
            nInitFunc(43u, coreVector2(-0.3f,-0.7f));
            nInitFunc(44u, coreVector2(-0.3f,-0.3f));
            nInitFunc(45u, coreVector2( 0.7f,-0.3f));
            nInitFunc(46u, coreVector2( 0.7f,-0.7f));
            nInitFunc(47u, coreVector2( 0.0f,-0.7f));
            nInitFunc(48u, coreVector2( 0.0f, 0.3f));
        });

        STAGE_ADD_SQUAD(pSquad2, cMinerEnemy, 13u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(30, 0u, COLOR_SHIP_MAGENTA);
            });
        });

        STAGE_GET_START(iNumData * 3u + iLineCount + 8u)
            STAGE_GET_VEC2       (vHelperMove)
            STAGE_GET_VEC2_ARRAY (avPosStorage, iNumData)
            STAGE_GET_FLOAT_ARRAY(afPosTime,    iNumData)
            STAGE_GET_FLOAT_ARRAY(afLineHeight, iLineCount, for(coreUintW i = 0u; i < iLineCount; ++i) afLineHeight[i] = 1000.0f;)
            STAGE_GET_FLOAT      (fEasyDelay)
            STAGE_GET_FLOAT      (fDistance)
            STAGE_GET_UINT       (iShortMade)
            STAGE_GET_UINT       (iLineIndex)
            STAGE_GET_UINT       (iQuickState)
            STAGE_GET_UINT       (iFlashState)
        STAGE_GET_END

        ASSERT((pSquad1->GetNumEnemiesAlive() <= iNumData) || (m_iStageSub >= 19u))

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + vHelperMove * ((50.0f + 100.0f * pHelper->GetLifeTime()) * TIME);
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        constexpr coreFloat fBulletSpeed = 1.2f;
        constexpr coreFloat fMoveSpeed   = (fBulletSpeed * BULLET_SPEED_FACTOR) / FOREGROUND_AREA.y;

        const auto nTeleportFunc = [&](const cEnemy* pEnemy, const coreBool bTransform, const coreBool bHelper)
        {
            if(pEnemy->ReachedDeath())
            {
                cPlayer* pPlayer = pEnemy->LastAttacker();
                if(!pPlayer->IsEnemyLook())
                {
                    const coreVector3 vOldPos = pPlayer->GetPosition();
                    const coreVector3 vNewPos = pEnemy ->GetPosition();

                    pPlayer->SetPosition(vNewPos);

                    g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                    {
                        if(pBullet->GetOwner() == pPlayer) pBullet->Deactivate(true);
                    });

                    const coreFloat fLen = (vNewPos - vOldPos).Length();
                    fDistance += fLen;

                    if(fDistance >= 5000.0f)
                    {
                        STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pEnemy->GetPosition())
                    }

                    if(g_pGame->IsTask())
                    {
                        if(fLen < 6.0f)
                        {
                            if(++iShortMade >= iShortCount)
                            {
                                STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawProgress(iShortMade, iShortCount, pEnemy->GetPosition());
                                g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iShortMade, iShortCount), SOUND_ITEM_COLLECT);
                            }
                        }

                        if(m_iStageSub >= 16u)
                        {
                            coreBool bBigJump = true;
                            for(coreUintW i = 0u; i < iLineCount; ++i)
                            {
                                     if(vOldPos.y > afLineHeight[i] * FOREGROUND_AREA.y) bBigJump = false;
                                else if(vNewPos.y < afLineHeight[i] * FOREGROUND_AREA.y) bBigJump = false;
                            }

                            if(bBigJump) STAGE_BADGE(2u, BADGE_NORMAL, pEnemy->GetPosition())
                        }
                    }

                    if(bTransform)
                    {
                        pPlayer->TurnIntoEnemy();
                        pPlayer->SetDirection(pEnemy->GetDirection());

                        g_pSpecialEffects->MacroExplosionColorBig(vNewPos, COLOR_ENERGY_MAGENTA);
                        g_pSpecialEffects->PlaySound(vNewPos, 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_03);
                        g_pSpecialEffects->RumblePlayer(pPlayer, SPECIAL_RUMBLE_BIG, 250u);
                    }

                    if(bHelper)
                    {
                        pHelper->Resurrect(false);
                        pHelper->SetPosition(vOldPos);

                        vHelperMove = -AlongCrossXNormal(vOldPos.xy());
                    }

                    if(g_pGame->IsEasy()) fEasyDelay = 0.3f;

                    const coreVector3 vPos  = vOldPos;
                    const coreVector3 vDiff = vNewPos - vPos;
                    const coreUintW   iNum  = MAX(F_TO_UI(vDiff.Length() / 1.7f), 2u);

                    for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, pPlayer->GetEnergyColor());
                }
            }
            else
            {
                if(iShortMade < iShortCount)
                {
                    STAGE_FOREACH_PLAYER(pPlayer, i)
                    {
                        if(pPlayer->IsEnemyLook()) return;

                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();
                        if(vDiff.LengthSq() < POW2(6.0f))
                        {
                            g_pGame->GetCombatText()->AttachMarker(i, "X", pEnemy->GetPosition(), COLOR_MENU_INSIDE);
                        }
                    });
                }
            }
        };

        const auto nReturnFunc = []()
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->IsEnemyLook())
                {
                    pPlayer->TurnIntoPlayer();

                    g_pSpecialEffects->MacroExplosionDarkBig(pPlayer->GetPosition());
                }
            });
        };

        const auto nShootWaveFunc = [&](const coreVector2 vDir)
        {
            for(coreUintW i = 0u; i < 40u; ++i)
            {
                if(g_pGame->IsEasy() && (((i + 2u) % 8u) < 4u)) continue;

                const coreVector2 vPos = (vDir * -1.2f + vDir.Rotated90() * ((I_TO_F(i) - 19.5f) * 0.055f)) * FOREGROUND_AREA;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, fBulletSpeed, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.5f);
            }

            if(m_iStageSub == 16u)
            {
                if(++iLineIndex >= iLineCount) iLineIndex = 0u;
                afLineHeight[iLineIndex] = 1.2f;
            }
        };

        if((m_iStageSub >= 19u) && pSquad2->IsFinished())
        {
                 if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad2, 10u, 10u)   // #
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad2, 11u, 11u)   // #
            else if(STAGE_SUB(22u)) STAGE_RESURRECT(pSquad2, 12u, 12u)   // #
        }

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  2u,  9u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 12u, 19u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 61u, 76u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 89u, 95u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 10u, 11u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad2,  0u,  0u)   // #
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad2,  1u,  2u)   // #
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad2,  3u,  5u)   // #
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 20u, 33u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad2,  6u,  6u)   // #
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 49u, 52u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 53u, 54u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 55u, 58u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 59u, 60u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 77u, 88u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 34u, 48u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad2,  7u, g_pGame->IsMulti() ? 8u : 7u)   // #
            else if(STAGE_SUB(19u))
            {
                STAGE_RESURRECT(pSquad1, 96u, 159u)
                STAGE_RESURRECT(pSquad2,  9u,   9u)   // #
            }

            if((m_iStageSub >= 12u) && (m_iStageSub <= 16u))
            {
                coreVector2 vDir;
                switch(m_iStageSub - 12u)
                {
                default: ASSERT(false)
                case 0u: vDir = coreVector2( 0.0f, 1.0f); break;
                case 1u: vDir = coreVector2( 0.0f,-1.0f); break;
                case 2u: vDir = coreVector2(-1.0f, 0.0f); break;
                case 3u: vDir = coreVector2( 1.0f, 0.0f); break;
                case 4u: vDir = coreVector2( 0.0f,-1.0f); break;
                }

                nShootWaveFunc(vDir);
            }

            std::memset(avPosStorage, 0, sizeof(coreVector2) * iNumData);
            std::memset(afPosTime,    0, sizeof(coreFloat)   * iNumData);
        }

        if(m_iStageSub == 16u)
        {
            if(STAGE_TICK_FREE2(fMoveSpeed * (3.0f/2.6f), 0.0f))
            {
                nShootWaveFunc(coreVector2(0.0f,-1.0f));
            }
        }

        if(m_iStageSub >= 16u)
        {
            for(coreUintW i = 0u; i < iLineCount; ++i)
            {
                afLineHeight[i] -= fMoveSpeed * TIME;
            }
        }

        fEasyDelay = MAX0(fEasyDelay - 1.0f * TIME);

        coreBool bPostpone = false;

        const coreBool bHelper = (m_iStageSub == 7u) && vHelperMove.IsNull();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 77u) ? 0.8f : ((i < 89u) ? fMoveSpeed : ((i < 96u) ? 1.4f : 1.3f)), (i < 61u) ? 0.0f : ((i < 77u) ? (0.3f * I_TO_F((i - 61u) % 16u)) : ((i < 89u) ? ((((i - 77u) % 2u) ? 0.25f : 0.05f) + (2.6f/3.0f) * I_TO_F((i - 77u) / 4u)) : ((i < 96u) ? (1.0f + 0.6f * I_TO_F(i - 89u)) : (3.0f + (2.6f/16.0f) * I_TO_F(i - 96u))))))

            if(i < 12u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 2u) ? ((i % 2u) ? 0.45f : -0.45f) : ((i < 10u) ? ((I_TO_F((i - 2u) % 4u) - 1.5f) * 0.45f) : ((i % 2u) ? 0.225f : -0.225f)), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  2u) {}
                else if(i <  6u) pEnemy->Rotate90 ();
                else if(i < 10u) pEnemy->Rotate270();
                else if(i < 12u) pEnemy->Rotate180();
            }
            else if(i < 49u)
            {
                if(!pEnemy->ReachedDeath())
                {
                    coreVector2& vPosStorage = STAGE_SINK_VEC2 (avPosStorage[i % iNumData]);
                    coreFloat&   fPosTime    = STAGE_SINK_FLOAT(afPosTime   [i % iNumData]);

                    if(STAGE_TAKEOFF) vPosStorage = pEnemy->GetPosition().xy();

                    const coreUintW iCurNum = 15u - pSquad1->GetNumEnemiesAlive();
                         if(i < 34u)                                  fPosTime += 2.0f * TIME;
                    else if(i - 34u <= iCurNum + (iCurNum ? 1u : 0u)) fPosTime += 3.0f * TIME;

                    const coreFloat fLerp = BLENDB(MIN1(fPosTime));

                    pEnemy->SetPosition      (coreVector3(fPosTime ? LERP(vPosStorage.Normalized() * (1.3f * SQRT2) * FOREGROUND_AREA, vPosStorage, fLerp) : HIDDEN_POS, 0.0f));
                    pEnemy->DefaultRotateLerp(0.0f*PI, 1.0f*PI, fLerp);

                    if(fPosTime >= 0.9f)
                    {
                        if(pEnemy->HasStatus(ENEMY_STATUS_GHOST_BULLET)) pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);
                    }
                    else
                    {
                        if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST_BULLET)) pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);
                    }
                }
            }
            else if(i < 61u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 53u || (i >= 55u && i < 59u)) ? ((I_TO_F(i % 4u) - 1.5f) * 0.3f) : ((i % 2u) ? -0.8f : -0.5f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 53u) pEnemy->Rotate180();
                else if(i < 55u) {}
                else if(i < 59u) pEnemy->Rotate90 ();
                else if(i < 61u) pEnemy->Rotate270();
            }
            else if(i < 77u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f, ((i - 61u) % 2u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((((i - 61u) % 16u) * 8u) % 11u) - 5.0f) * 0.15f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 69u) {}
                else if(i < 77u) pEnemy->Rotate90();
            }
            else if(i < 89u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((11u - (i - 77u)) * 5u) % 12u) - 5.5f) * 0.16f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 96u)
            {
                coreFloat fSide;
                coreUint8 iStep;
                switch(i - 89u)
                {
                default: ASSERT(false)
                case 0u: fSide = -0.5f; iStep = 0u; break;
                case 1u: fSide =  0.5f; iStep = 0u; break;
                case 2u: fSide = -1.5f; iStep = 1u; break;
                case 3u: fSide = -1.5f; iStep = 2u; break;
                case 4u: fSide = -1.5f; iStep = 3u; break;
                case 5u: fSide = -0.5f; iStep = 1u; break;
                case 6u: fSide = -0.5f; iStep = 0u; break;
                }
                fSide *= 0.45f;

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(fSide, 0.0f);

                if(pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime))
                {
                    pEnemy->Kill(false);
                    bPostpone = true;
                }

                pEnemy->ToAxis(StepRotated90(iStep));

                if(g_pGame->IsTaskExtra() && pEnemy->ReachedDeath())
                {
                    if(++iQuickState >= 7u)
                    {
                        STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                    }
                    else
                    {
                        g_pGame->GetCombatText()->DrawCountdown(iQuickState, 7u, pEnemy->GetPosition());
                        g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iQuickState, 7u), SOUND_PLACEHOLDER);
                    }
                }
            }
            else if(i < 160u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 96u) * 23u) % 64u) - 31.5f) * 0.03f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            nTeleportFunc(pEnemy, false, bHelper);
        });

        const coreBool bTransform = (m_iStageSub == 18u) || (m_iStageSub == 19u);

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 9u) ? 0.8f : 0.4f, (i == 9u) ? 1.28f : 0.0f)

            if(i < 9u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 1u) ? -0.9f : ((i < 3u) ? ((i % 2u) ? 0.6f : -0.6f) : ((i < 6u) ? ((I_TO_F(i - 3u) - 1.0f) * 0.4f) : ((i < 7u) ? 0.9f : (g_pGame->IsMulti() ? ((i % 2u) ? 0.3f : -0.3f) : 0.0f)))), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 1u) {}
                else if(i < 3u) pEnemy->Rotate90 ();
                else if(i < 6u) pEnemy->Rotate270();
                else if(i < 7u) {}
                else if(i < 9u) pEnemy->Rotate180();
            }
            else if(i < 13u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((((i - 9u) % 4u) * 2u + 1u) % 5u) - 1.5f) * 0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if((i != 7u) && (i != 8u))
            {
                const coreVector2 vAim = pEnemy->AimAtPlayerSide();
                if(!vAim.IsNull()) pEnemy->SetDirection(coreVector3(vAim.Normalized(), 0.0f));
            }

            nTeleportFunc(pEnemy, bTransform, false);

            if(!fEasyDelay && STAGE_LIFETIME_AFTER(0.3f * (fLifeSpeed / 0.8f)) && STAGE_TICK_LIFETIME(5.0f * (0.8f / fLifeSpeed), 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();

                for(coreUintW j = 5u; j--; )
                {
                    if(g_pGame->IsEasy() && ((j == 0u) || (j == 4u))) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.5f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_MAGENTA);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }

            if(pEnemy->ReachedDeath())
            {
                g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cViewBullet>([&](cViewBullet* OUTPUT pBullet)
                {
                    if(pBullet->GetOwner() == pEnemy) pBullet->Deactivate(true);
                });
            }
        });

        if(g_pGame->IsMulti() && bTransform)
        {
            STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS)
            {
                if(pBullet->GetID() == cViewBullet::ID)
                {
                    if(m_iStageSub < 19u) pBullet->AddStatus   (BULLET_STATUS_GHOST);
                                     else pBullet->RemoveStatus(BULLET_STATUS_GHOST);
                }
            });
        }

        if(STAGE_CLEARED) nReturnFunc();

        cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

        const coreFloat fAlpha  = pBackground->GetLightningAlpha ();
        const coreBool  bStatus = pBackground->GetLightningStatus();

             if((iFlashState % 2u) == 0u &&  bStatus) iFlashState += 1u;
        else if((iFlashState % 2u) == 1u && !bStatus) iFlashState += 1u;

        if(m_Geminga.HasStatus(ENEMY_STATUS_DEAD) && (iFlashState == 5u))
        {
            m_Geminga.ResurrectIntro();
        }
        else if(!m_Geminga.HasStatus(ENEMY_STATUS_DEAD) && (iFlashState == 6u))
        {
            m_Geminga.Kill(false);
            m_Geminga.SetAlpha(1.0f);
        }

        if(!m_Geminga.HasStatus(ENEMY_STATUS_DEAD))
        {
            m_Geminga.SetAlpha(fAlpha * 0.2f);
        }

             if((m_iStageSub ==  4u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f, 0.0f), 10.0f);
        else if((m_iStageSub == 12u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,-1.0f), 10.0f);

        if(!bPostpone) STAGE_WAVE(3u, "7-4", {55.0f, 80.0f, 110.0f, 135.0f, 270.0f})   // VIERZIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cViewBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            if(pPlayer->IsEnemyLook())
            {
                pPlayer->TurnIntoPlayer();
            }
        });

        m_Geminga.Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(0.0f,-1.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // corpses remain on the field
    // enemies should not get attached to the side they are spawning from to reduce issues (steep angles or out-of-view kills) (but why are last two groups different ? though it looks better)
    // first bullet wave needs an opening at the bottom, as player will most likely start there
    // moving enemy waves need to have good flow when killing one after another, and distinct paths (when all waves would spawn together)
    // corners should not be safe-spots
    // every bullet in the corpse-phase should be removed in the middle (though inverting it or twisting it would be allowed)
    // legion sollte mit einem spread-angriff anfangen, sobald er angreifbar ist
    // legion muss bei erster bewegung einen schritt zurückmachen, damit der spieler sieht, dass bewegung stattfindet, und um die distanz zum spieler zu erhöhen damit die erste richtige bewegung den spieler nicht gleich zerquetscht
    // similar to zelda water boss https://youtu.be/HRQXOCU8OWA?t=1005
    // TASK: destroy all zombies
    // TASK: touch the helper for N seconds
    // TASK EXTRA: never touch any of the enemies
    // TODO 1: hard-mode: geschoss-linie die im kreis rotiert, bei allen stages!, auch bei legion, gegen legion rotation (andere bullet-farbe)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        constexpr coreUintW iNumEnemies = 40u;

        // {mini-stage, mini-time, enemy-number}
        constexpr coreUint32 aaiRepeat[][3] = {{4u, 20u, 3u}, {4u, 30u, 13u}, {4u, 40u, 23u}, {6u, 20u, 2u}, {6u, 40u, 7u}, {6u, 30u, 15u}, {8u, 30u, 32u}, {8u, 20u, 36u}, {8u, 40u, 38u}};
        constexpr coreUintW  iNumRepeats    = ARRAY_SIZE(aaiRepeat);

        STATIC_ASSERT(coreMath::IsAligned(iNumRepeats, MUSCUS_ZOMBIES))

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 1.3f,-0.8f), coreVector2(1.0f, 0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2( 1.3f, 0.8f), coreVector2(-1.0f,0.0f));
            pPath2->AddNode(coreVector2(-1.3f,-0.8f), coreVector2(-1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(3u);
            pPath3->AddNode(coreVector2(-0.8f,-1.3f), coreVector2(0.0f, 1.0f));
            pPath3->AddNode(coreVector2( 0.0f, 0.8f), coreVector2(1.0f, 0.0f));
            pPath3->AddNode(coreVector2( 0.8f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.3f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.3f,-1.6f), coreVector2(0.0f,-1.0f));   // increase distance between first and last enemy
            pPath4->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(iNumEnemies * 3u + iNumRepeats + 14u)
            STAGE_GET_VEC2_ARRAY (avChildMove,  iNumEnemies)
            STAGE_GET_FLOAT_ARRAY(afChildAngle, iNumEnemies)
            STAGE_GET_FLOAT_ARRAY(afAliveTime,  iNumRepeats)
            STAGE_GET_UINT       (iAlivePoints)
            STAGE_GET_UINT       (iStageMini)
            STAGE_GET_FLOAT      (fStageMiniTime)
            STAGE_GET_VEC4       (vLegionPos)
            STAGE_GET_FLOAT      (fLegionRota)
            STAGE_GET_FLOAT      (fLegionTime)
            STAGE_GET_UINT       (iLegionCount)
            STAGE_GET_UINT       (iHelperTouch)
            STAGE_GET_UINT       (iEnemyTouch)
            STAGE_GET_UINT       (iReverseState)
            STAGE_GET_UINT       (iFlashState)
        STAGE_GET_END

        const coreFloat fStageMiniTimeBefore = fStageMiniTime;

        const auto nStageMiniTickFunc = [&](const coreFloat fSpeed, const coreFloat fOffset)
        {
            return (fStageMiniTimeBefore < fStageMiniTime) && STAGE_TICK_EXTERN(fStageMiniTime, fStageMiniTimeBefore, fSpeed, fOffset);
        };

        const auto nStageMiniChangeFunc = [&](const coreUint32 iStage)
        {
            iStageMini     = iStage;
            fStageMiniTime = 0.0f;
        };

        if(m_iStageSub < 5u)
        {
            coreUint8 iNumAlive = 0u;
            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(pEnemy->GetCurHealth()) iNumAlive += 1u;
            });

            if(!iNumAlive)
            {
                     if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  9u)
                else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 10u, 19u)
                else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 29u)
                else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 30u, 39u)
                else if(STAGE_SUB(5u)) {}
            }
        }
        else
        {
            fStageMiniTime += 1.0f * TIME;

            if((iStageMini == 0u) && (fStageMiniTime >= 1.0f))
            {
                nStageMiniChangeFunc(1u);
            }
            else if((iStageMini == 10u) && (fStageMiniTime >= 1.0f))
            {
                nStageMiniChangeFunc(11u);

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    avChildMove [i] = pEnemy->GetPosition ().xy();
                    afChildAngle[i] = pEnemy->GetDirection().xy().Angle();
                });
            }
            else if((iStageMini == 11u) && (fStageMiniTime >= 3.0f))
            {
                nStageMiniChangeFunc(12u);

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST);
                    pEnemy->SetMaxHealth(10);
                    pEnemy->SetCurHealth(10);
                    pEnemy->RefreshColor(1.0f);
                });
            }

            const coreVector2 vEnvDir = coreVector2::Direction(LERP(-1.0f*PI, -6.0f*PI, STEPH3(0.0f, 80.0f, m_fStageSubTime)));
            g_pEnvironment->SetTargetDirectionNow(vEnvDir);
        }

        if(g_pGame->IsTask())
        {
            for(coreUintW i = 0u; i < iNumRepeats; ++i)
            {
                if((aaiRepeat[i][0] == iStageMini) && (fStageMiniTimeBefore < fStageMiniTime) && InBetween(I_TO_F(aaiRepeat[i][1]) * 0.1f, fStageMiniTimeBefore, fStageMiniTime))
                {
                    afAliveTime[i] = 6.0f;

                    cEnemy* pEnemy = pSquad1->GetEnemy(aaiRepeat[i][2]);

                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                    pEnemy->SetCurHealth(pEnemy->GetMaxHealth());
                    pEnemy->RefreshColor(1.0f);

                    this->EnableZombie(i % MUSCUS_ZOMBIES);

                    g_pSpecialEffects->CreateBlowColor(pEnemy->GetPosition(), -pEnemy->GetPosition().Normalized(), 100.0f, 20u, COLOR_ENERGY_CYAN);
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
                }
            }

            for(coreUintW i = 0u; i < iNumRepeats; ++i)
            {
                if(afAliveTime[i])
                {
                    cEnemy* pEnemy = pSquad1->GetEnemy(aaiRepeat[i][2]);

                    m_aZombie[i % MUSCUS_ZOMBIES].SetPosition (pEnemy->GetPosition () - pEnemy->GetDirection() * 0.4f);
                    m_aZombie[i % MUSCUS_ZOMBIES].SetDirection(pEnemy->GetDirection());

                    if(pEnemy->ReachedDeath())
                    {
                        pEnemy->ApplyScore();
                        pEnemy->AddStatus(ENEMY_STATUS_GHOST);

                        this->DisableZombie(i % MUSCUS_ZOMBIES, true);

                        afAliveTime[i] = 0.0f;

                        if(++iAlivePoints >= iNumRepeats)
                        {
                            STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawProgress(iAlivePoints, iNumRepeats, pEnemy->GetPosition());
                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iAlivePoints, iNumRepeats), SOUND_ITEM_COLLECT);
                        }

                        g_pSpecialEffects->MacroExplosionDarkSmall(pEnemy->GetPosition());
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_09);
                    }

                    afAliveTime[i] = MAX0(afAliveTime[i] - 1.0f * TIME);
                    if(!afAliveTime[i])
                    {
                        pEnemy->AddStatus(ENEMY_STATUS_GHOST);
                        pEnemy->RefreshColor(0.0f);

                        this->DisableZombie(i % MUSCUS_ZOMBIES, true);

                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
                    }
                }
            }
        }

        coreVector2 vLegionTarget = coreVector2(0.0f,0.0f);
        if(iStageMini == 12u)
        {
            fLegionRota += 1.0f * TIME;
            fLegionTime -= 1.0f * TIME;

            if(fLegionTime <= 0.0f)
            {
                const coreVector2 vTarget = iLegionCount ? g_pGame->FindPlayerDual(((iLegionCount - 1u) / 2u) % 2u)->GetPosition().xy() : coreVector2(0.0f, 0.2f * FOREGROUND_AREA.y);
                const coreVector2 vDiff   = vTarget - vLegionPos.zw();

                vLegionPos.xy(vLegionPos.zw());
                vLegionPos.zw(vLegionPos.zw() + vDiff.Normalized() * MIN(vDiff.Length(), 25.0f));

                fLegionTime  += 2.0f;
                iLegionCount += 1u;
            }

            vLegionTarget = LERPH3(vLegionPos.xy(), vLegionPos.zw(), CLAMP(2.0f - fLegionTime, 0.0f, 1.0f));
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_RED);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            pHelper->SetPosition(coreVector3(vLegionTarget, 0.0f));
            if(STAGE_CLEARED) pHelper->Kill(true);

            if(g_pGame->IsTask() && STAGE_TICK_FREE(60.0f, 0.0f))
            {
                coreBool bTouch = false;
                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pPlayer->GetPosition().xy() - pHelper->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(5.0f))
                    {
                        bTouch = true;
                    }
                });

                if(bTouch)
                {
                    if(++iHelperTouch >= 300u)
                    {
                        STAGE_BADGE(1u, BADGE_NORMAL, pHelper->GetPosition())
                    }
                    else g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.1f", 5.0f - FloorFactor(I_TO_F(iHelperTouch) / 60.0f, 10.0f)), pHelper->GetPosition(), COLOR_MENU_INSIDE);
                }
            }
        }

        const coreMatrix2 mEnemyRota = coreMatrix3::Rotation(1.0f * TIME).m12();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 2.0f, 0.25f * I_TO_F(i % 10u))

            if(iStageMini <= 1u)
            {
                //   2
                // 1   0
                //   3
                const coreVector2 vSide   = (i < 20u) ? coreVector2((i < 10u) ? 1.0f : -1.0f, 0.0f) : coreVector2(0.0f, (i < 30u) ? 1.0f : -1.0f);
                const coreVector2 vTarget = vSide.Rotated90() * ((I_TO_F(i % 10u) - 4.5f) * 0.2f) + vSide;

                if(pEnemy->ReachedDeath())
                {
                    pEnemy->ApplyScore();
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);

                    g_pSpecialEffects->MacroDestructionDark(pEnemy);
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_09);

                    avChildMove[i] = (vTarget * FOREGROUND_AREA * 1.05f - pEnemy->GetPosition().xy()).Normalized();

                    if(i == 9u - (iReverseState % 10u) + (iReverseState / 10u) * 10u)
                    {
                        if(++iReverseState == iNumEnemies) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pEnemy->GetPosition())
                    }
                    else
                    {
                        iReverseState = 0xFFu;
                    }
                }

                if(pEnemy->GetCurHealth())
                {
                    const coreSpline2* pPath = (i < 10u) ? pPath1 : ((i < 20u) ? pPath2 : ((i < 30u) ? pPath3 : pPath4));

                    STAGE_REPEAT(pPath->GetTotalDistance())

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                    pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
                }
                else
                {
                    coreVector2& vMove = avChildMove[i];
                    if(!vMove.IsNull())
                    {
                        coreVector2       vNewPos = pEnemy->GetPosition().xy() + vMove * (100.0f * TIME);
                        const coreVector2 vNewDir = coreVector2::Direction(pEnemy->GetDirection().xy().Angle() + (10.0f * TIME));

                        if(((vNewPos.x < -FOREGROUND_AREA.x * 1.05f) && (vMove.x < 0.0f)) ||
                           ((vNewPos.x >  FOREGROUND_AREA.x * 1.05f) && (vMove.x > 0.0f)) ||
                           ((vNewPos.y < -FOREGROUND_AREA.y * 1.05f) && (vMove.y < 0.0f)) ||
                           ((vNewPos.y >  FOREGROUND_AREA.y * 1.05f) && (vMove.y > 0.0f)))
                        {
                            vNewPos = vTarget * FOREGROUND_AREA * 1.05f;
                            vMove   = coreVector2(0.0f,0.0f);

                            g_pSpecialEffects->CreateSplashColor(coreVector3(vNewPos, 0.0f), 10.0f, 3u, COLOR_ENERGY_WHITE * 0.8f);
                        }

                        pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                        pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));
                    }
                }
            }
            else if(iStageMini >= 11u)
            {
                const coreFloat   fDelay  = I_TO_F((i * 7u) % 40u);
                const coreVector2 vTarget = vLegionTarget + coreVector2::Direction(GA * fDelay - fLegionRota) * BLENDB((fDelay + 1.0f) / 40.0f) * 15.0f;

                if(iStageMini == 11u)
                {
                    const coreFloat   fTime   = CLAMP01(1.0f - 1.0f * (fStageMiniTime       - 0.05f * fDelay));
                    const coreFloat   fTime2  = CLAMP01(1.0f - 1.0f * (fStageMiniTimeBefore - 0.05f * fDelay));
                    const coreVector2 vNewPos = LERPB(vTarget, avChildMove[i], fTime);
                    const coreVector2 vNewDir = coreVector2::Direction(afChildAngle[i] + LERPB(1.0f*PI, 0.0f*PI, fTime));

                    pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

                    if((fTime == 0.0f) && (fTime2 != 0.0f))
                    {
                        g_pSpecialEffects->CreateBlowColor(pEnemy->GetPosition(), pEnemy->GetPosition().Normalized(), 50.0f, 5u, COLOR_ENERGY_WHITE * 0.8f);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS((i * 7u) % 40u, 40u), SOUND_EFFECT_WOOSH_2);
                    }
                }
                else
                {
                    const coreVector2 vNewPos = vTarget;
                    const coreVector2 vNewDir = pEnemy->GetDirection().xy() * mEnemyRota;

                    pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

                    if(nStageMiniTickFunc(1.0f, CORE_MATH_PRECISION - 1.0f) && (!g_pGame->IsEasy() || (s_iTick % 2u)))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = (s_iTick % 2u) ? pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized() : (vPos - vLegionTarget).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);   // not immortal

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vNewPos, 0.0f), 10.0f, 3u, COLOR_ENERGY_RED);
                        g_pSpecialEffects->PlaySound(coreVector3(vNewPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
            }

            if(g_pGame->IsTaskExtra())
            {
                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(5.0f))
                    {
                        if(++iEnemyTouch == 1u)
                        {
                            g_pGame->GetCombatText()->DrawText(Core::Language->GetString("HIT"), pPlayer->GetPosition(), COLOR_MENU_YELLOW);

                            g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_YELLOW);
                            g_pSpecialEffects->PlaySound(pPlayer->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
                        }
                    }
                });
            }
        });

        cEnemy* pDummy = pSquad1->GetEnemy(0u);

        if((iStageMini == 1u) || (iStageMini == 8u))   // open and half open quad [fixed]
        {
            if(nStageMiniTickFunc(1.0f, 0.0f))
            {
                if(s_iTick < 6u)
                {
                    for(coreUintW i = 0u; i < 80u; ++i)
                    {
                        if((s_iTick < 4u) ? ((i >= ((s_iTick + 2u) % 4u) * 20u) && (i < (((s_iTick + 2u) % 4u) + 1u) * 20u)) : ((s_iTick % 2u) == ((i / 5u) % 2u))) continue;

                        if(g_pGame->IsEasy() && ((i % 20u) < 10u)) continue;

                        const coreVector2 vSide   = (i < 40u) ? coreVector2((i < 20u) ? 1.0f : -1.0f, 0.0f) : coreVector2(0.0f, (i < 60u) ? -1.0f : 1.0f);
                        const coreVector2 vTarget = vSide.Rotated90() * ((I_TO_F(i % 20u) - 9.5f) * 0.1f) + vSide;

                        const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                        const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                        const coreFloat   fSpeed = vTarget.Length() * 0.7f;
                        const coreInt32   iRota  = (iStageMini == 8u) ? ((s_iTick % 2u) ? 7 : 6) : 5;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                }
                else nStageMiniChangeFunc(iStageMini + 1u);
            }
        }
        else if(iStageMini == 2u)   // starfield [consistent]
        {
            if(nStageMiniTickFunc(18.0f, 0.0f))
            {
                if(s_iTick < 80u)
                {
                    if(!g_pGame->IsEasy() || (s_iTick % 2u))
                    {
                        const coreVector2 vTarget = coreVector2::Direction(I_TO_F(s_iTick) * GA);

                        for(coreUintW i = 0u; i < 2u; ++i)
                        {
                            const coreVector2 vPos   = vTarget * FOREGROUND_AREA * (1.2f + (i ? 0.05f : 0.0f)) * SQRT2;
                            const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                            const coreFloat   fSpeed = vTarget.Length() * 1.0f;
                            const coreInt32   iRota  = 5;

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        }
                    }
                }
                else nStageMiniChangeFunc(iStageMini + 1u);

                if(s_iTick == 80u) pHelper->Resurrect(true);
            }
        }
        else if(iStageMini == 3u)   // circle with different rotation [increasing]
        {
            if(nStageMiniTickFunc(0.5f, -0.7f))
            {
                if(s_iTick < 6u)
                {
                    for(coreUintW i = 0u; i < 40u; ++i)
                    {
                        const coreVector2 vTarget = coreVector2::Direction(I_TO_F(i) / 80.0f * (2.0f*PI));
                        const coreBool    bFlip   = ((s_iTick % 2u) == ((i / ((s_iTick >= 4u) ? 5u : ((s_iTick >= 2u) ? 10u : 20u))) % 2u));

                        const coreVector2 vPos   = vTarget * FOREGROUND_AREA * (1.2f + (bFlip ? 0.05f : 0.0f)) * SQRT2;
                        const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                        const coreFloat   fSpeed = 0.6f;
                        const coreInt32   iRota  = bFlip ? 7 : 6;

                        if(g_pGame->IsEasy() && (((s_iTick % 2u) != 0u) != bFlip)) continue;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy,  vPos,  vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, -vPos, -vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                }
                else nStageMiniChangeFunc(iStageMini + 1u);
            }
        }
        else if(iStageMini == 4u)   // rotating lines [consistent]
        {
            if(nStageMiniTickFunc(1.2f, -0.1f))
            {
                if(s_iTick < 11u)
                {
                    const coreVector2 vTarget = coreVector2::Direction(I_TO_F(s_iTick) * (0.7f*PI));

                    for(coreUintW i = g_pGame->IsEasy() ? 10u : 0u; i < 20u; ++i)
                    {
                        const coreVector2 vPos   = vTarget * FOREGROUND_AREA * (1.2f + I_TO_F(i) * 0.08f) * SQRT2;
                        const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                        const coreFloat   fSpeed = 1.0f;
                        const coreInt32   iRota  = (i < 10u) ? 7 : 6;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                }
                else nStageMiniChangeFunc(iStageMini + 1u);
            }
        }
        else if(iStageMini == 5u)   // quads with different speed (new) [increasing]
        {
            if(nStageMiniTickFunc(0.5f, 0.3f))
            {
                if(s_iTick < 8u)
                {
                    const coreUintW iNum = (s_iTick >= 4u) ? 6u : ((s_iTick >= 2u) ? 12u : 24u);

                    for(coreUintW i = 0u; i < 96u; ++i)
                    {
                        if(g_pGame->IsEasy() && ((i % iNum) < (iNum / 2u))) continue;

                        const coreVector2 vSide   = StepRotated90(i / 24u);
                        const coreVector2 vTarget = vSide.Rotated90() * ((I_TO_F(i % 24u) - 11.5f) * 0.1f * (20.0f/24.0f)) + vSide;

                        const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                        const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                        const coreFloat   fSpeed = vTarget.Length() * (((s_iTick % 2u) == (((i + iNum / 2u) / iNum) % 2u)) ? 0.5f : 1.0f);
                        const coreInt32   iRota  = (s_iTick >= 6u) ? ((s_iTick % 2u) ? 7 : 6) : 5;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                }
                else nStageMiniChangeFunc(iStageMini + 1u);
            }
        }
        else if(iStageMini == 6u)   // double twister [consistent]
        {
            if(nStageMiniTickFunc(30.0f, 0.0f))
            {
                if(s_iTick < 230u)
                {
                    if((s_iTick % 30u) < (g_pGame->IsEasy() ? 10u : 20u))
                    {
                        for(coreUintW i = 0u; i < 2u; ++i)
                        {
                            const coreVector2 vTarget = coreVector2::Direction(I_TO_F(s_iTick) / 20.0f * (0.5f*PI) + (i ? (0.5f*PI) : (0.0f*PI)));

                            const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f * SQRT2;
                            const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                            const coreFloat   fSpeed = vTarget.Length() * 1.3f;
                            const coreInt32   iRota  = 5;

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy,  vPos,  vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, -vPos, -vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        }
                    }
                }
                else nStageMiniChangeFunc(iStageMini + 1u);
            }
        }
        else if(iStageMini == 7u)   // rotating spheres [consistent]
        {
            if(nStageMiniTickFunc(12.0f, 0.0f))
            {
                if(s_iTick < 60u)
                {
                    if(!g_pGame->IsEasy() || !(s_iTick % 2u))
                    {
                        const coreVector2 vBase = coreVector2::Direction(I_TO_F(s_iTick) * GA);

                        for(coreUintW i = 0u; i < 4u; ++i)
                        {
                            const coreVector2 vTarget = vBase + 0.08f * StepRotated90(i);

                            const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f * SQRT2;
                            const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                            const coreFloat   fSpeed = vTarget.Length() * 1.0f;
                            const coreInt32   iRota  = (s_iTick % 2u) ? 7 : 6;

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        }
                    }
                }
                else nStageMiniChangeFunc(iStageMini + 1u);
            }
        }
        else if(iStageMini == 9u)   // two snakes (new) [fixed]
        {
            if(nStageMiniTickFunc(30.0f, 0.0f))
            {
                if(s_iTick < 160u)
                {
                    const coreVector2 vSide   = StepRotated90(((s_iTick / 20u) + 1u) % 4u);
                    const coreVector2 vTarget = vSide.Rotated90() * ((I_TO_F((s_iTick % 10u) + ((s_iTick < 80u) ? 0u : 10u)) - 9.5f) * (((s_iTick % 20u) < 10) ? 0.1f : -0.1f)) + vSide;

                    const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vTarget.Length() * 1.3f;
                    const coreInt32   iRota  = 5;

                                           g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy,  vPos,  vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    if(!g_pGame->IsEasy()) g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, -vPos, -vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
                else nStageMiniChangeFunc(iStageMini + 1u);
            }
        }

        if(iStageMini < 12u)
        {
            const coreMatrix2 mBulletRota    = coreMatrix3::Rotation(0.7f * TIME).m12();
            const coreMatrix2 mBulletRotaRev = mBulletRota.Transposed();

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
            {
                switch(pBullet->GetDamage())
                {
                case 5:
                    break;

                case 6:
                    pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mBulletRota, 0.0f));
                    pBullet->SetFlyDir  (pBullet->GetFlyDir() * mBulletRota);
                    break;

                case 7:
                    pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mBulletRotaRev, 0.0f));
                    pBullet->SetFlyDir  (pBullet->GetFlyDir() * mBulletRotaRev);
                    break;

                default:
                    ASSERT(false)
                    break;
                }

                if((pBullet->GetPosition().xy().LengthSq() < POW2(2.0f)) || (pBullet->GetFlyTime() >= 10.0f))
                {
                    pBullet->Deactivate(true);
                    g_pGame->PlayVanishSound(pBullet->GetPosition());
                }
            });
        }

        if(g_pGame->IsTaskExtra() && STAGE_CLEARED)
        {
            if(!iEnemyTouch) STAGE_BADGE(2u, BADGE_HARD, pHelper->GetPosition())
        }

        cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

        const coreFloat fAlpha  = pBackground->GetLightningAlpha ();
        const coreBool  bStatus = pBackground->GetLightningStatus();

             if((iFlashState % 2u) == 0u &&  bStatus) iFlashState += 1u;
        else if((iFlashState % 2u) == 1u && !bStatus) iFlashState += 1u;

        if(m_Geminga.HasStatus(ENEMY_STATUS_DEAD) && (iFlashState == 5u))
        {
            m_Geminga.ResurrectIntro();
        }
        else if(!m_Geminga.HasStatus(ENEMY_STATUS_DEAD) && (iFlashState == 6u))
        {
            m_Geminga.Kill(false);
            m_Geminga.SetAlpha(1.0f);
        }

        if(!m_Geminga.HasStatus(ENEMY_STATUS_DEAD))
        {
            m_Geminga.SetAlpha(fAlpha * 0.3f);
        }

        STAGE_WAVE(4u, "7-5", {85.0f, 125.0f, 170.0f, 210.0f, 420.0f})   // EINUNDVIERZIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTriangleBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < MUSCUS_ZOMBIES; ++i)
            this->DisableZombie(i, false);

        m_Geminga.Kill(false);

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
    // enemy has weakpoint which he keeps away from player
    // - all: miners rotation start against fly direction, to make evading the initial attack easier
    // - 2: cinders in pair always rotate together away from player when one is shot from outer direction, to prevent ping-pong effect
    // TODO 1: miner should be grey ?   
    // TODO 1: don't actually rotate away, but stay fixed or rotate consistent
    // TODO 1: rotate like an atom across center-enemy
    // TODO 1: snakehead with tail, destroy tail
    // TODO 1: chain -> other weakpoint destroys enemy protecting next weakpoint (inside enemy but visible) across the field
    // TODO 1: weakpoint rolls around border with high speed, flies like ZeroRanger fast enemies across screen, are like a wall with enemy at center
    // TODO 1: weakpoint dies faster, whole wave needs to be faster
    // TODO 1: actually damage and blink other enemy
    // TODO 1: MAIN: task-check, helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 5u})
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
                pEnemy->Configure(1, 0u, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cCinderEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, 0u, COLOR_SHIP_YELLOW);
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
            if(pEnemy->ReachedDeath() && ((i < 2u) || pSquad2->GetEnemy(i ^ 0x01u)->HasStatus(ENEMY_STATUS_DEAD)))
            {
                cEnemy* pParent = pSquad1->GetEnemy((i < 2u) ? i : ((i+2u) / 2u));
                pParent->Kill(true);
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) {STAGE_RESURRECT(pSquad1, 0u, 1u) STAGE_RESURRECT(pSquad2, 0u, 1u)}
            else if(STAGE_SUB(2u)) {STAGE_RESURRECT(pSquad1, 2u, 3u) STAGE_RESURRECT(pSquad2, 2u, 5u)}
            else if(STAGE_SUB(3u)) {STAGE_RESURRECT(pSquad1, 4u, 5u) STAGE_RESURRECT(pSquad2, 6u, 9u)}
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

            const coreFloat fTurn = coreVector2::Dot(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), vShootDir.Rotated90());

            pEnemy->SetDirection(coreVector3(STAGE_TAKEOFF ? -pEnemy->GetDirection().xy() : coreVector2::Direction(vShootDir.Angle() + MIN(ABS(fTurn) * 5.0f, 1.0f) * SIGN(fTurn) * -1.2f * TIME), 0.0f));

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
                const coreFloat fAngleTo   = ((i == 3u || i == 5u || i == 7u || i == 9u) ? (afTarget[i-1u] + (0.5f*PI)) : afTarget[i]);
                const coreFloat fShift     = ((i == 3u || i == 5u || i == 7u || i == 9u) ? (afSign  [i-1u])             : afSign  [i]) * ABS(AngleDiff(fAngleTo, fAngleFrom));

                const coreVector2 vDir = coreVector2::Direction(STAGE_TAKEOFF ? fAngleTo : AngleLerp(fAngleFrom, fAngleTo + fShift, 2.5f * TIME));
                const coreVector2 vPos = pParent->GetPosition().xy() + vDir * 13.0f;

                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
            else
            {
                coreVector2&      vDir = aiFreeMove[i-6u];
                const coreVector2 vPos = pEnemy->GetPosition().xy() + vDir * (40.0f * TIME);

                     if((vPos.x < -FOREGROUND_AREA.x) && (vDir.x < 0.0f)) vDir.x =  ABS(vDir.x);   // TODO 1: bounce correction
                else if((vPos.x >  FOREGROUND_AREA.x) && (vDir.x > 0.0f)) vDir.x = -ABS(vDir.x);
                     if((vPos.y < -FOREGROUND_AREA.y) && (vDir.y < 0.0f)) vDir.y =  ABS(vDir.y);
                else if((vPos.y >  FOREGROUND_AREA.y) && (vDir.y > 0.0f)) vDir.y = -ABS(vDir.y);

                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
        });

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_VAL(pSquad1), COLL_VAL(pSquad2), COLL_VAL(afTarget), COLL_VAL(afSign), COLL_VAL(aiFreeState), COLL_VAL(aiFreeMove))
        {
            if(!bFirstHit || (pEnemy->GetID() != cCinderEnemy::ID)) return;

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

        STAGE_WAVE(5u, "7-?", {60.0f, 80.0f, 100.0f, 120.0f, 240.0f})   // ZWEIUNDVIERZIG
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
        UNUSED STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, GEMINGA_ENEMIES_TELEPORT)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 0.0f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_PURPLE);
                pEnemy->AddStatus(ENEMY_STATUS_BOTTOM | ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS);
            });
        });

        UNUSED STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, GEMINGA_ENEMIES_LEGION)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_BOSS(m_Geminga, {190.0f, 285.0f, 380.0, 475.0f, 950.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
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
    // ################################################################
}