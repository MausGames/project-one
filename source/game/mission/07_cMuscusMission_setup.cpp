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
        g_pEnvironment->SetTargetSpeedNow(6.0f);

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
    // TODO 1: badge, kill 1 or 3 hidden enemies
    // TODO 1: coop exploit, spieler stehen einfach ganz links und rechts, was ist wenn immer nur einer ein licht hat, abwechselnd, spieler 2 zuerst
    // TODO 1: hardmode: zusätzliche Mario geister die man nicht töten kann (vlt. nur einer)
    // TODO 1: vielleicht die flachen linien im teleporter gegner schräger machen (beachte den lichtkegel)
    // TODO 1: die beiden 4er wellen entfernen, wenn die fürn orsch sind
    // TODO 1: erster gegner in 6x6 gruppe stirbt zu schnell, wenn spieler grad richtig schießt
    // TODO 1: mehr zwischen-subwave flickern hinzufügen, bei anderen wellen nach 10
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: vielleicht wurm-gegner hören nach kurzer zeit oder sobald sie sichtbar sind zu schießen auf
    // TODO 1: bei raster-gruppe bleibt der spieler einfach ganz unten und schaut nach oben
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
                pEnemy->Configure((i == 23u || i == 27u) ? 120 : 10, 0u, COLOR_SHIP_GREEN);
                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        STAGE_GET_START(iNumData + 6u)
            STAGE_GET_FLOAT_ARRAY(afFade, iNumData)
            STAGE_GET_FLOAT      (fBlind)
            STAGE_GET_FLOAT      (fBlindLoop)
            STAGE_GET_UINT64     (iLight)
            STAGE_GET_UINT       (iStepHit)
            STAGE_GET_UINT       (iStepRemove)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        STATIC_ASSERT(iNumData <= sizeof(iLight)*8u)

        cHeadlight* pHeadlight = d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight();

        if(STAGE_CLEARED)
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

            if((m_iStageSub == 3u) || (m_iStageSub == 5u) || (m_iStageSub == 7u))
            {
                pHeadlight->PlayFlicker(0u);
            }

            std::memset(afFade, 0, sizeof(coreFloat) * iNumData);
            iLight = 0x0000000000000000u;
        }

        if(m_iStageSub == 10u)
        {
            if(STAGE_SUBTIME_POINT(1.0f))
            {
                 fBlind = 6.5f;
                 pHeadlight->PlayFlicker(1u);
            }
        }
        else if(m_iStageSub == 20u)
        {
            if(STAGE_SUBTIME_POINT(0.6f))
            {
                iLight = 0xFFFFFFFFFFFFFFFFu;
            }
        }
        else if((m_iStageSub >= 21u) && (m_iStageSub <= 24u))
        {
            fBlindLoop -= 1.0f * TIME;
            if(fBlindLoop <= 0.0f)
            {
                fBlindLoop += 3.2f;

                fBlind = 2.0f;
                pHeadlight->PlayFlicker(1u);
            }
        }

        if(fBlind)
        {
            fBlind = MAX(fBlind - 1.0f * TIME, 0.0f);
            if(!fBlind) pHeadlight->PlayFlicker(0u);
        }

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i == 6u) ? 1.0f : ((i == 19u) ? 2.0f : ((i >= 8u && i < 16u) ? (0.5f * I_TO_F((i - 8u) % 4u)) : 0.0f)))

            coreBool bDisappear = false;
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
                const coreVector2 vOffset = coreVector2(0.0f, (i < 8u) ? (((i - 6u) % 2u) ? -0.9f : 0.9f) : ((i < 12u) ? (((i - 8u) % 2u) ? 0.9f : -0.9f) : 0.9f));

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

                const coreVector2 vDir = -vPos.Normalized();

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
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
                case 0u: vPos = coreVector2( 0.8f, 0.8f); break;
                case 1u: vPos = coreVector2(-0.8f,-0.8f); break;
                case 2u: vPos = coreVector2( 0.4f, 0.0f); break;
                case 3u: vPos = coreVector2(-0.8f, 0.4f); break;
                case 4u: vPos = coreVector2( 0.4f,-0.8f); break;
                case 5u: vPos = coreVector2( 0.0f, 0.8f); break;
                case 6u: vPos = coreVector2(-0.8f, 0.0f); break;
                case 7u: vPos = coreVector2( 0.0f,-0.8f); break;
                case 8u:
                case 9u: vPos = coreVector2( 0.8f, 0.0f); break;
                }

                const coreVector2 vDir = -vPos.Normalized();

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }
            else if(i < 65u)
            {
                const coreUintW iIndex = i - 29u;

                const coreVector2 vPos = (coreVector2(I_TO_F(iIndex % 6u), I_TO_F(iIndex / 6u)) - 2.5f) * 0.4f;
                const coreVector2 vDir = pEnemy->AimAtPlayerDual((iIndex + (iIndex / 6u)) % 2u).Normalized();

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));

                     if(iStepRemove == 0u) bDisappear = (iIndex != 16u);
                else if(iStepRemove == 1u) bDisappear = (iIndex != 25u);
                else if(iStepRemove == 2u) bDisappear = (iIndex !=  8u);
                else if(iStepRemove == 3u) bDisappear = (iIndex != 28u);
                else                       bDisappear = true;

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
                        if(iStepRemove != 4u) iLight = 0xFFFFFFFFFFFFFFFFu;

                        bShootWave = true;
                    }
                }
            }

            if(!fBlind)
            {
                if(g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy(), 1.05f) && !pEnemy->ReachedDeath())
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - (pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * pPlayer->GetVisualRadius() * 0.0f);
                        const coreFloat   fDot  = coreVector2::Dot(pPlayer->GetDirection().xy(), vDiff.Normalized());

                        if(fDot > 0.96f)
                        {
                            if(i >= 29u && i < 65u)
                            {
                                // disappear
                                if(bDisappear) REMOVE_BIT(iLight, i % iNumData)
                            }
                            else
                            {
                                // appear
                                if((i == 28u) && (iStepHit < 9u))
                                {
                                    iStepHit += 1u;
                                }
                                else
                                {
                                    ADD_BIT(iLight, i % iNumData)
                                }
                            }
                        }
                    });
                }
            }

            coreFloat& fFade = afFade[i % iNumData];

            if(HAS_BIT(iLight, i % iNumData)) fFade = MIN(fFade + 4.0f * TIME, 1.0f);
                                         else fFade = MAX(fFade - 4.0f * TIME, 0.0f);

            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * ((i == 23u || i == 27u) ? 2.3f : 1.5f) * LERPS(2.0f, 1.0f, fFade));
            pEnemy->SetAlpha(LERPS(0.0f, 1.0f, fFade));

            if(fFade)
            {
                if(!bDisappear) pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);
                pEnemy->RemoveStatus(ENEMY_STATUS_HIDDEN);
            }
            else
            {
                pEnemy->AddStatus(ENEMY_STATUS_HIDDEN);
            }

            if(i < 6u || (i >= 16u && i < 24u) || i == 27u || bShootWave)
            {
                if(STAGE_TICK_LIFETIME((i == 23u) ? 3.0f : 2.0f, 0.0f) || bShootWave)
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();

                    for(coreUintW j = 6u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 30.0f + 15.0f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.7f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.7f);
                    }
                }
            }

            if((i >= 6u && i < 16u) || (i >= 24u && i < 29u))
            {
                if(STAGE_TICK_LIFETIME(20.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.7f);
                }
            }
        });

        if(!bPostpone) STAGE_WAVE(0u, "SIEBENUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight()->ResetFlicker();

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
    // TODO 1: badge
    // TODO 1: spawn of rotation-blocks too dangerous, can easily hit player (is it still? first block now comes from top)
    // TODO 1: helper bekommt -5s combat text wenn er getroffen wird (oder was auch immer der spieler bekommen würde, wenn er getroffen wird)
    // TODO 1: vielleicht bei phasen-wechsel alle blöcke der alten phase die unsichtbar sind verschwinden lassen (oder kollision deaktivieren)
    // TODO 1: hard mode: alles 45 grad gedreht
    // TODO 1: übergang von diagonal aufs nächste muss gehandelt werden, damit sich nicht zwei zu sehr überlagern (vielleicht skip 1 line (+ delay enemy))
    // TODO 1: übergang von tetris aufs nächste muss gehandelt werden (vielleicht), gegner könnten zu früh kommen
    // TODO 1: maybe reduce health again, thought then other changes need to compensate for it (e.g. higher frequency, more enemies at the end, etc.)
    // TODO 1: gegner in 12 12 12 pattern anders (siehe notizen) (achtung, die gegner sind im code 01237654)
    // TODO 1: TOP, bei 12 12 12 pattern, gegner eigentlich ganz oben mit abstand platzieren, nicht folgen lassen
    // TODO 1: die ersten 4 gruppen sind noch etwas fishy, vor allem die rauf-runter gruppe, adden die value ? diese 4 gruppen sollten die mechanik introducen
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreFloat fStep        = 0.275f;
        constexpr coreFloat fStepDelay   = 0.35f;
        constexpr coreFloat fSpeed       = 0.8f;
        constexpr coreFloat fFreqSlow    = 1.1f;
        constexpr coreFloat fFreqFast    = 2.0f;
        constexpr coreUintW iTypeMapSize = coreMath::CeilAlign(MUSCUS_GENERATES, 4u) / 4u;

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

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 76u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(10 + 10, 0u, COLOR_SHIP_BLUE);
            });
        });

        STAGE_GET_START(iTypeMapSize + MUSCUS_GENERATES * 3u + 6u)
            STAGE_GET_UINT_ARRAY (aiGenerateTypeMap, iTypeMapSize)
            STAGE_GET_VEC2_ARRAY (avGenerateData,    MUSCUS_GENERATES)
            STAGE_GET_FLOAT_ARRAY(afGenerateTime,    MUSCUS_GENERATES)
            STAGE_GET_FLOAT      (fRotation,         fRotation =  0.5f*PI;)
            STAGE_GET_FLOAT      (fSpawn,            fSpawn    = -1.0f)
            STAGE_GET_UINT       (iSpawnTick)
            STAGE_GET_UINT       (iSpawnCount)
            STAGE_GET_FLOAT      (fSpawnOffset)
            STAGE_GET_UINT       (iCreateStart)
        STAGE_GET_END

        coreUint8* aiGenerateType = r_cast<coreUint8*>(aiGenerateTypeMap);

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

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 24u, 27u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 28u, 35u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 36u, 47u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 48u, 59u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 60u, 67u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 68u, 75u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 8u,  23u)
            else if(STAGE_SUB(10u))
            {
                this->TestGenerate(false);
                STAGE_DELAY_START_CLEAR
            }

            if((m_iStageSub == 1u) || (m_iStageSub == 2u) || (m_iStageSub == 5u) || (m_iStageSub == 6u) || (m_iStageSub == 7u) || (m_iStageSub == 8u) || (m_iStageSub == 9u))
            {
                iSpawnCount  = 0u;
                fSpawnOffset = (1.0f - fSpawn) / (fSpeed * fFreqFast);
            }
        }

        if(STAGE_BEGINNING)
        {
            this->TestGenerate(true);
        }

        fSpawn += TIME * (fSpeed * RCP(pPath1->GetTotalDistance() * 0.5f)) * ((m_iStageSub >= 7u) ? fFreqFast : fFreqSlow);
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
            else if(m_iStageSub < 5u)
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
            else if(m_iStageSub < 6u)
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
            else if(m_iStageSub < 7u)
            {
                // diagonal
                const coreUint8 iSide = 4u + (iSpawnCount % 4u);

                for(coreUintW i = 0u; i < 11u; ++i)
                {
                    if(i != 5u) nCreateGenerateFunc(iSide, 1.0f, fStep * (I_TO_F(i) - 5.0f), 0.0f);
                }
            }
            else if(m_iStageSub < 8u)
            {
                // 1122 1122
                const coreUint8 iSide = (iSpawnCount % 2u) ? 2u : 0u;

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    nCreateGenerateFunc(0u, 1.0f, fStep * (I_TO_F(i + iSide + ((i >= 2u) ? 2u : 0u)) - 3.5f), 0.0f);
                }
            }
            else if(m_iStageSub < 9u)
            {
                // 12 12 12 12
                const coreUint8 iSide = (iSpawnCount % 2u) ? 0u : 1u;

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    nCreateGenerateFunc(0u, 1.0f, fStep * (I_TO_F(i * 2u + iSide) - 3.5f), 0.0f);
                }
            }
            else if((m_iStageSub == 9u) && !iSpawnCount)
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

        if(m_iStageSub == 1u)
        {
            cHelper* pHelper = g_pGame->GetHelper(ELEMENT_GREEN);

            if(STAGE_BEGINNING)
            {
                pHelper->Resurrect();
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
                }
            }
        }
        else if((m_iStageSub == 9u) && iSpawnCount)
        {
            fRotation += TIME * (0.5f*PI) * LERPH3(0.0f, 1.0f, MIN(m_fStageSubTime * 0.1f, 1.0f));
        }
        else if(m_iStageSub == 10u)
        {
            fRotation += TIME * (0.5f*PI) * LERPH3(1.0f, 0.0f, MIN(m_fStageSubTime * 1.0f, 1.0f));

            for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
                this->ShowGenerate(i, 1.0f);

            if(m_fStageSubTime >= 1.5f)
            {
                for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
                    this->DisableGenerate(i, true);

                g_pSpecialEffects->MacroExplosionColorBig(coreVector3(0.0f,0.0f,0.0f), COLOR_ENERGY_GREEN);

                cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

                pBackground->GetHeadlight()->ResetFlicker();
                pBackground->SetEnableLightning(true);
                pBackground->SetEnableHeadlight(false);
                pBackground->FlashLightning();

                STAGE_DELAY_END
            }
        }

        const coreVector2 vRotaDir = coreVector2::Direction(fRotation);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 24u)
            {
                STAGE_LIFETIME(pEnemy, fSpeed, (i < 4u) ? 2.0f : ((i < 8u) ? 0.0f : 3.0f))

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f) * fStep * ((i < 8u) ? 2.0f : 1.0f), 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  4u) {}
                else if(i <  8u) pEnemy->Rotate180();
                else if(i < 12u) {}
                else if(i < 16u) pEnemy->Rotate90 ();
                else if(i < 20u) pEnemy->Rotate180();
                else if(i < 24u) pEnemy->Rotate270();
            }
            else if(i < 36u)
            {
                STAGE_LIFETIME(pEnemy, 0.6f, 0.4f * I_TO_F((i - 24u) % 4u))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 28u) ? ((I_TO_F(i - 24u) - 1.5f) * 0.6f) : (((i - 28u) % 2u) ? -0.3f : 0.9f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 28u) pEnemy->Rotate90 ();
                else if(i < 32u) {}
                else if(i < 36u) pEnemy->Rotate180();

                if(i < 28u && i % 2u) pEnemy->InvertX();
            }
            else if(i < 60u)
            {
                STAGE_LIFETIME(pEnemy, fSpeed, 1.0f)

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 36u) % 6u) - 2.5f) * fStep, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 42u) pEnemy->Rotate90 ();
                else if(i < 48u) pEnemy->Rotate270();
                else if(i < 54u) {}
                else if(i < 60u) pEnemy->Rotate180();
            }
            else if(i < 76u)
            {
                STAGE_LIFETIME(pEnemy, fSpeed, fSpawnOffset + (I_TO_F(((i - 60u) % 2u) * 2u + (((i - 60u) / 4u) % 2u)) * (1.3f / fFreqFast)))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((((i - 60u) % 8u) < 4u) ? 1.0f : -1.0f) * (((i < 68u) && (((i - 60u) % 4u) < 2u)) ? -1.0f : 1.0f), 1.0f);
                const coreVector2 vOffset = coreVector2(((i < 68u) ? (I_TO_F((i - 60u) % 4u) + 0.5f) : (I_TO_F((((i < 72u) ? (i - 68u) : (79u - i)) % 4u) * 2u) - 3.5f)) * fStep, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
        });

        coreUintW iRotaIndex = 0u;

        for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
        {
            coreObject3D* pGenerate = this->GetGenerate(i);
            if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            afGenerateTime[i] += 1.0f * TIME;

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

                if(fTime < (g_pGame->IsEasy() ? 1.1f : 0.55f) * fLength)
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
                const coreFloat fTime   = CLAMP(afGenerateTime[i] * 0.5f - ABS(I_TO_F(iRotaIndex - 5u)) * 0.5f, 0.0f, 1.0f);
                const coreFloat fOffset = (I_TO_F(iRotaIndex - 5u) * fStep + LERPB(1.5f, 0.0f, fTime) * ((iRotaIndex < 5u) ? -1.0f : 1.0f)) * ((m_iStageSub == 10u) ? LERPB(0.0f, 1.0f, CLAMP(1.5f - m_fStageSubTime, 0.0f, 1.0f)) : ((m_iStageSub > 10u) ? 0.0f : 1.0f));

                pGenerate->SetPosition (coreVector3(vRotaDir * fOffset * FOREGROUND_AREA, 0.0f));
                pGenerate->SetDirection(coreVector3(vRotaDir,                             0.0f));

                if(!fTime)           this->ShowGenerate(i, 2.0f);
                if(iRotaIndex == 5u) this->ShowGenerate(i, 1.0f);
                if(iRotaIndex == 5u) pGenerate->SetPosition(pGenerate->GetPosition().RotatedZ90());

                iRotaIndex += 1u;
            }
        }

        STAGE_WAVE(1u, "ACHTUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
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
    // TODO 1: helper in one of the balls
    // TODO 1: badge, hidden balls 1-N, other color, with special effect and sound
    // TODO 1: big pearl cluster at the end
    // TODO 1: fix 7-pearl loops (add: if there is something to fix, not sure)
    // TODO 1: player scoring and combat text with strike-state etc.
    // TODO 1: spieler wird am schießen gehindert, interrupt, vielleicht wenn die lampe kaputt geht, von der vorherigen wave (diese gruppe sollte auch im dunkeln starten)
    // TODO 1: background side-movement might be easier for direct attack from top
    // TODO 1: maybe also update strike-spline target tangent in real-time (e.g. when enemy moves into strike the animation gets slower)
    // TODO 1: enemy verliert tatsächlich lebens-punkte !
    // TODO 1: shoot-counter sollte irgendwie resettet werden, damit die wurst-angriffe nicht unterbrochen beginnen (sollten sie am ende unterbrochen werden, isses wurst) (STAGE_TICK_EXTERN ?)
    // TODO 1: striking pearls need a different appearance (color would be good, white?)
    // TODO 1: striking pearls sollten sich strecken (direction anpassen + Y size größer + leichte XY size kleiner)
    // TODO 1: striking attack sollte weiter weggehn wenn sie näher bei target sind, vielleicht beschleunigt das mehr
    // TODO 1: pearls in the final phase might be hidden by the bullets at the beginning, making comprehension harder
    // TODO 1: vielleicht sollten die 3 linien näher in die mitte, dort ist es schwierig die kugerl einzusammeln
    // TODO 1: größere explosion am ende, allgemein größerer impact effect der pearls (direktional ?) + screenshake
    // TODO 1: auch größerer effekt beim einsammeln der pearls (e.g. wave effect)
    // TODO 1: die 3 linien fühlen sich noch nicht so perfekt an, sollten aber nicht zu schwer sein (ruhe vor dem sturm)
    // TODO 1: attack pattern in end-wave changes after collecting half the pearls
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: make sure pearl animation-order is correct on all sub-stages
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
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
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(1000, 0u, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(5u + MUSCUS_PEARLS * 3u)
            STAGE_GET_FLOAT      (fEnemyTime)
            STAGE_GET_FLOAT      (fEnemyRota, fEnemyRota = 1.0f*PI)
            STAGE_GET_VEC2       (vEnemyOldPos)
            STAGE_GET_UINT       (iPearlSpawn)
            STAGE_GET_FLOAT_ARRAY(afPearlTime, MUSCUS_PEARLS)
            STAGE_GET_VEC2_ARRAY (avPearlMove, MUSCUS_PEARLS)
        STAGE_GET_END

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

        if(!m_iPearlActive)
        {
            cEnemy* pEnemy = pSquad1->GetEnemy(0u);

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
            else if(STAGE_SUB(12u)) pEnemy->Kill(true);

            if((m_iStageSub == 2u) || (m_iStageSub == 6u) || (m_iStageSub == 7u) || (m_iStageSub == 8u) || (m_iStageSub == 10u) || (m_iStageSub == 11u))
                fEnemyTime = 0.0f;

            vEnemyOldPos = pEnemy->GetPosition().xy() / FOREGROUND_AREA;

            std::memset(afPearlTime, 0, sizeof(coreFloat)   * MUSCUS_PEARLS);
            std::memset(avPearlMove, 0, sizeof(coreVector2) * MUSCUS_PEARLS);
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
                case  0u: vPos = coreVector2( 0.3f, 0.0f); break;
                case  1u: vPos = coreVector2( 0.6f, 0.0f); break;
                case  2u: vPos = coreVector2( 0.6f,-0.3f); break;
                case  3u: vPos = coreVector2( 0.3f,-0.3f); break;
                case  4u: vPos = coreVector2( 0.0f,-0.3f); break;
                case  5u: vPos = coreVector2( 0.0f,-0.6f); break;
                case  6u: vPos = coreVector2(-0.3f,-0.6f); break;
                case  7u: vPos = coreVector2(-0.3f,-0.3f); break;
                case  8u: vPos = coreVector2(-0.3f, 0.0f); break;
                case  9u: vPos = coreVector2(-0.6f, 0.0f); break;
                case 10u: vPos = coreVector2(-0.6f, 0.3f); break;
                case 11u: vPos = coreVector2(-0.3f, 0.3f); break;
                case 12u: vPos = coreVector2( 0.0f, 0.3f); break;
                case 13u: vPos = coreVector2( 0.3f, 0.3f); break;
                case 14u: vPos = coreVector2( 0.0f, 0.3f); break;
                case 15u: vPos = coreVector2( 0.0f, 0.0f); break;
                }

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
                const coreVector2 vPos = coreVector2::Direction(I_TO_F(i / 6u) * ((2.0f/3.0f)*PI) - fPearlTime + I_TO_F(i % 6u) * 0.08f) * (I_TO_F(i % 6u) * 0.15f + LERPB(1.2f * SQRT2, 0.2f, MIN1(fPearlTime * 0.5f)));

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
                if(vDiff.LengthSq() < POW2(4.5f))
                {
                    this->StrikeAttack(j, pPlayer, pSquad1->GetEnemy(0u));
                    g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_WHITE);

                    if(m_iStageSub == 6u)
                    {
                        iPearlSpawn += 1u;
                        if(iPearlSpawn < 16u) this->EnablePearl(iPearlSpawn);
                    }
                }
            }
        });

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
            }
            else if(m_iStageSub <= 9u)
            {
                const cPlayer*    pPlayer = pEnemy->NearestPlayerDual((m_iStageSub == 8u) ? 0u : 1u);
                const coreVector2 vDiff   = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();
                const coreFloat   fSpeed  = LERPH3(0.0f, 3.0f, fEnemyTime);

                const coreVector2 vPos = coreVector2(pEnemy->GetPosition().x + vDiff.x * (fSpeed * TIME), 0.9f * FOREGROUND_AREA.y);
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                pEnemy->SetPosition (coreVector3(vPos, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir, 0.0f));
            }
            else if(m_iStageSub <= 11u)
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
                    if(STAGE_TICK_LIFETIME(0.7f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = pEnemy->AimAtPlayerDual(s_iTick % 2u).Angle();

                        for(coreUintW j = 5u; j--; )
                        {
                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                        }
                    }
                }
                else if(m_iStageSub == 6u)
                {
                    if(STAGE_TICK_LIFETIME(0.7f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = pEnemy->AimAtPlayerDual(s_iTick % 2u).Angle();

                        for(coreUintW j = 20u; j--; )
                        {
                            if((s_iTick & 0x01u) == ((j / 2u) & 0x01u)) continue;

                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 9.5f) * 5.0f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                        }
                    }
                }
                else if(m_iStageSub == 7u)
                {
                    if(STAGE_TICK_LIFETIME(16.0f, 0.0f) && ((s_iTick % 16u) < 8u))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                    }
                }
                else if(m_iStageSub <= 9u)
                {
                    if(STAGE_TICK_LIFETIME(16.0f, 0.0f) && ((s_iTick % 8u) < 4u))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                    }
                }
                else if(m_iStageSub == 10u)
                {
                    if(STAGE_TICK_LIFETIME(16.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                    }
                }
                else if(m_iStageSub == 11u)
                {
                    if(STAGE_TICK_LIFETIME(1.0f, 0.0f))
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreFloat   fBase = 0.0f;

                        for(coreUintW j = 40u; j--; )
                        {
                            if((s_iTick & 0x01u) == ((j / 4u) & 0x01u)) continue;

                            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f) + fBase);

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos,  vDir)->ChangeSize(1.4f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, -vDir)->ChangeSize(1.4f);
                        }
                    }
                }
            }
        });

        STAGE_WAVE(2u, "NEUNUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
            this->DisablePearl(i, false);

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
    // TODO 1: interface icons ersetzen sich auch (+ continue) -> general icon implementation
    // TODO 1: badge: follow all paths correctly -> auf COOP achten
    // TODO 1: transformed player should not kill other (transforming) enemy for coop -> IMMORTAL and do custom handling
    // TODO 1: in the bullet-wave phase, maybe move enemies more far away, to force player rotation (same distance like first groups ?)
    // TODO 1: gegner in trail in schussrichtung drehen (egal ob von kommender oder in gehende richtung)
    // TODO 1: in coop, nur 1 spieler transformiert ?, der bekommt rosa hit-box + lifes
    // TODO 1: wenn man im gegner ist sollten mehr interessantere gruppen als ein blöder teppich kommen (aber trotzdem gegner-masse, weil man ja die wilde waffe hat) (noch immer? vielleicht andere shooting enemies)
    // TODO 1: coop, vielleicht werden spieler an die gleiche stelle teleportiert, oder sie teleportieren sich gegenseitig
    // TODO 1: coop, in step-by-step trail könnten zwei gleichzeitig erscheinen mit schräger +-xy versetzung
    // TODO 1: vielleicht transformation nach jedem jump, aber waffe nur am ende (naja, vielleicht zu verwirrend)
    // TODO 1: damaging two enemies at the exact same time can be confusing, maybe ignore somehow damage on second enemy (immediate resurrect) (achtung auf coop, nur bei mutli-kill vom selben player)
    // TODO 1: shooting enemy is not used that often, maybe re-arrange ? or spread out ?
    // TODO 1: should the size of the shooting enemy be increased ? (also for player transformation)
    // TODO 1: first (few?) enemies should fly further into center, to show teleportation better
    // TODO 1: gegner anordnung vor zweiter trail/highspeed gruppe sollte so geändert werden, dass man besser in die nächste gruppe startet
    // TODO 1: verwandlung ist sehr kurz, könnte ok sein, muss dann aber bei boss mehrmals verwendet werden
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: schräge gegner-bewegungen im finale ?
    // TODO 1: helper kommt einmal aus old-position vom spieler nach teleport
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreUintW iNumData = 14u;

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

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 104u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_PURPLE);
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
        });

        STAGE_ADD_SQUAD(pSquad2, cMinerEnemy, 9u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(30, 0u, COLOR_SHIP_MAGENTA);
            });
        });

        STAGE_GET_START(iNumData * 3u)
            STAGE_GET_VEC2_ARRAY (avPosStorage, iNumData)
            STAGE_GET_FLOAT_ARRAY(afPosTime,    iNumData)
        STAGE_GET_END

        const auto nTeleportFunc = [](const cEnemy* pEnemy, const coreBool bTransform)
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

                    if(bTransform)
                    {
                        pPlayer->TurnIntoEnemy();
                        g_pSpecialEffects->MacroExplosionColorBig(vNewPos, COLOR_ENERGY_MAGENTA);
                    }

                    const coreVector3 vPos  = vOldPos;
                    const coreVector3 vDiff = vNewPos - vPos;
                    const coreUintW   iNum  = MAX(F_TO_UI(vDiff.Length() / 1.9f), 2u);

                    for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, pPlayer->GetEnergyColor());
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
                const coreVector2 vPos = (vDir * -1.2f + vDir.Rotated90() * ((I_TO_F(i) - 19.5f) * 0.055f)) * FOREGROUND_AREA;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.5f);
            }
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,   1u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  2u,   9u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 12u,  19u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 60u,  75u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 10u,  11u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad2,  0u,   0u)   // #
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad2,  1u,   2u)   // #
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad2,  3u,   5u)   // #
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 20u,  33u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad2,  6u,   6u)   // #
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 48u,  51u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 52u,  53u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 54u,  57u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 58u,  59u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 92u, 103u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 34u,  47u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad2,  7u,   8u)   // #
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 76u,  91u)

            if((m_iStageSub >= 11u) && (m_iStageSub <= 15u))
            {
                coreVector2 vDir;
                switch(m_iStageSub - 11u)
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

        constexpr coreFloat fBulletSpeed = (1.2f * BULLET_SPEED_FACTOR) / FOREGROUND_AREA.y;

        if(m_iStageSub == 15u)
        {
            if(STAGE_TICK_FREE2(fBulletSpeed * (3.0f/2.6f), 0.0f))
            {
                nShootWaveFunc(coreVector2(0.0f,-1.0f));
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 92u) ? 0.8f : fBulletSpeed, (i < 60u) ? 0.0f : ((i < 92u) ? (0.3f * I_TO_F((i - 60u) % 16u)) : (0.05f + (2.6f/3.0f) * I_TO_F((i - 92u) / 4u))))

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
            else if(i < 48u)
            {
                if(!pEnemy->ReachedDeath())
                {
                    coreVector2& vPosStorage = avPosStorage[i % iNumData];
                    coreFloat&   fPosTime    = afPosTime   [i % iNumData];

                    if(STAGE_TAKEOFF) vPosStorage = pEnemy->GetPosition().xy();

                    const coreUintW iCurNum = 14u - pSquad1->GetNumEnemiesAlive();
                    if((i < 34u) || (i - 34u <= iCurNum + (iCurNum ? 1u : 0u))) fPosTime += 2.0f * TIME;

                    const coreFloat fLerp = LERPB(0.0f, 1.0f, MIN(fPosTime, 1.0f));

                    pEnemy->SetPosition      (coreVector3(fPosTime ? LERP(vPosStorage.Normalized() * (1.3f * SQRT2) * FOREGROUND_AREA, vPosStorage, fLerp) : HIDDEN_POS, 0.0f));
                    pEnemy->DefaultRotateLerp(0.0f*PI, 1.0f*PI, fLerp);

                    if(fPosTime >= 0.9f)
                    {
                        if(pEnemy->HasStatus(ENEMY_STATUS_GHOST)) pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                    }
                    else
                    {
                        if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST)) pEnemy->AddStatus(ENEMY_STATUS_GHOST);
                    }
                }
            }
            else if(i < 60u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 52u || (i >= 54u && i < 58u)) ? ((I_TO_F(i % 4u) - 1.5f) * 0.3f) : ((i % 2u) ? -0.8f : -0.5f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 52u) pEnemy->Rotate180();
                else if(i < 54u) {}
                else if(i < 58u) pEnemy->Rotate90 ();
                else if(i < 60u) pEnemy->Rotate270();
            }
            else if(i < 92u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f, ((i - 60u) % 2u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((((i - 60u) % 16u) * 8u) % 11u) - 5.0f) * 0.15f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 68u) {}
                else if(i < 76u) pEnemy->Rotate90 ();
                else if(i < 84u) pEnemy->Rotate180();
                else if(i < 92u) pEnemy->Rotate270();
            }
            else if(i < 104u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((11u - (i - 92u)) * 5u) % 12u) - 5.5f) * 0.16f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            nTeleportFunc(pEnemy, false);
        });

        const coreBool bTransform = (m_iStageSub >= 17u) && (pSquad2->GetNumEnemiesAlive() < g_pGame->GetNumPlayers());

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i < 1u) ? -0.9f : ((i < 3u) ? ((i % 2u) ? 0.6f : -0.6f) : ((i < 6u) ? ((I_TO_F(i - 3u) - 1.0f) * 0.4f) : ((i < 7u) ? 0.9f : ((i % 2u) ? 0.3f : -0.3f)))), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i < 1u) {}
            else if(i < 3u) pEnemy->Rotate90 ();
            else if(i < 6u) pEnemy->Rotate270();
            else if(i < 7u) {}
            else if(i < 9u) {}

            nTeleportFunc(pEnemy, bTransform);

            if(STAGE_LIFETIME_AFTER(0.3f) && STAGE_TICK_LIFETIME(5.0f, 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition().xy();
                const coreFloat   fBase = pEnemy->AimAtPlayerSideRev().Angle();

                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.5f);
                }
            }

            if(pEnemy->ReachedDeath())
            {
                g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cViewBullet>([&](cViewBullet* OUTPUT pBullet)
                {
                    if(pBullet->GetOwner() == pEnemy) pBullet->Deactivate(true);
                });
            }
        });

        if(STAGE_CLEARED) nReturnFunc();

        STAGE_WAVE(3u, "VIERZIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            if(pPlayer->IsEnemyLook()) pPlayer->TurnIntoPlayer();
        });

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
    // TODO 1: enemies moving outside creating badges, objects to collect or shoot, circles to fly
    // TODO 1: effekte beim einschlagen in die wand, und beim einschlagen in legion, und beim wiederbelebt werden
    // TODO 1: helfer is in mitte ?
    // TODO 1: similar to zelda water boss https://youtu.be/HRQXOCU8OWA?t=1005
    // TODO 1: create particle effect (interpolated) on the side first, to make the player move away into the center
    // TODO 1: kern des kerns bleibt am leben (ghost), und bekommt dann noch super-angriffe ? kugel-ring ?
    // TODO 1: letzte der 4 gruppen kommt von oben, aber 3te gruppe greift man schon dort an, dann sind gegner getötet worden bevor sie ins feld kamen, und ihre position am rand war "irgendwo" (gegner sollten nicht aus/in die seite kommen in die sie fliegen werden)
    // TODO 1: in der mitte energie kugel die größer wird und dann die gegner vom rand anzieht
    // TODO 1: easy: remove rotation and bullet-count on non-rotating (to make holes bigger)
    // TODO 1: hard-mode: geschoss-linie die im kreis rotiert, bei allen stages!, auch bei legion, gegen legion rotation (andere bullet-farbe)
    // TODO 1: double twister gefällt mir noch nicht so gut (ersetzen oder entfernen) (zb. old double-snakes mit fixen linien mit löchern (though the are shit with hard-mode))
    // TODO 1: sollte wirklich eine bullet-wave zweimal vorkommen ? (derzeitige erklärung wäre eine "absichtliche wieder-erkennung")
    // TODO 1: bei "quads with different speed" vielleicht die löcher bei letzter stufe etwas breiter machen
    // TODO 1: nochmal alle timings anschauen und anpassen (zeit zwischen mini-stages, länge der mini-stages, länge der steigerungen pro mini-stage)
    // TODO 1: fix rotating and crashing enemies having unintended symmetric direction
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: bei dem gegner den man in bullet-phase für badge abschießen kann sind blitze und partikel, und seine farbe blinkt (für paar s)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        constexpr coreUintW iNumEnemies = 40u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 1.2f,-0.8f), coreVector2(1.0f, 0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2( 1.2f, 0.8f), coreVector2(-1.0f,0.0f));
            pPath2->AddNode(coreVector2(-1.2f,-0.8f), coreVector2(-1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(3u);
            pPath3->AddNode(coreVector2(-0.8f,-1.2f), coreVector2(0.0f, 1.0f));
            pPath3->AddNode(coreVector2( 0.0f, 0.8f), coreVector2(1.0f, 0.0f));
            pPath3->AddNode(coreVector2( 0.8f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));   // increase distance between first and last enemy
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

        STAGE_GET_START(iNumEnemies * 3u + 9u)
            STAGE_GET_VEC2_ARRAY (avChildMove,  iNumEnemies)
            STAGE_GET_FLOAT_ARRAY(afChildAngle, iNumEnemies)
            STAGE_GET_UINT       (iStageMini)
            STAGE_GET_FLOAT      (fStageMiniTime)
            STAGE_GET_VEC4       (vLegionPos)
            STAGE_GET_FLOAT      (fLegionRota)
            STAGE_GET_FLOAT      (fLegionTime)
            STAGE_GET_UINT       (iLegionCount)
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

        const coreMatrix2 mEnemyRota = coreMatrix3::Rotation(1.0f * TIME).m12();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 2.0f, 0.25f * I_TO_F(i % 10u))

            if(iStageMini == 0u)
            {
                if(pEnemy->ReachedDeath())
                {
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);

                    g_pSpecialEffects->MacroDestructionDark(pEnemy);

                    //   3
                    // 1   0
                    //   2
                    const coreVector2 vSide   = (i < 20u) ? coreVector2((i < 10u) ? 1.0f : -1.0f, 0.0f) : coreVector2(0.0f, (i < 30u) ? -1.0f : 1.0f);
                    const coreVector2 vTarget = vSide.Rotated90() * ((I_TO_F(i % 10u) - 4.5f) * 0.2f) + vSide;

                    avChildMove[i] = (vTarget * FOREGROUND_AREA * 1.05f - pEnemy->GetPosition().xy()).Normalized();
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

                             if((vNewPos.x < -FOREGROUND_AREA.x * 1.05f) && (vMove.x < 0.0f)) {vNewPos *= -FOREGROUND_AREA.x * 1.05f * RCP(vNewPos.x); vMove = coreVector2(0.0f,0.0f);}
                        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.05f) && (vMove.x > 0.0f)) {vNewPos *=  FOREGROUND_AREA.x * 1.05f * RCP(vNewPos.x); vMove = coreVector2(0.0f,0.0f);}
                             if((vNewPos.y < -FOREGROUND_AREA.y * 1.05f) && (vMove.y < 0.0f)) {vNewPos *= -FOREGROUND_AREA.y * 1.05f * RCP(vNewPos.y); vMove = coreVector2(0.0f,0.0f);}
                        else if((vNewPos.y >  FOREGROUND_AREA.y * 1.05f) && (vMove.y > 0.0f)) {vNewPos *=  FOREGROUND_AREA.y * 1.05f * RCP(vNewPos.y); vMove = coreVector2(0.0f,0.0f);}

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
                    const coreFloat   fTime   = CLAMP(1.0f - 1.0f * (fStageMiniTime - 0.05f * fDelay), 0.0f, 1.0f);
                    const coreVector2 vNewPos = LERPB(vTarget, avChildMove[i], fTime);
                    const coreVector2 vNewDir = coreVector2::Direction(afChildAngle[i] + LERPB(1.0f*PI, 0.0f*PI, fTime));

                    pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));
                }
                else
                {
                    const coreVector2 vNewPos = vTarget;
                    const coreVector2 vNewDir = pEnemy->GetDirection().xy() * mEnemyRota;

                    pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

                    if(nStageMiniTickFunc(1.0f, CORE_MATH_PRECISION - 1.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = (s_iTick % 2u) ? pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized() : (vPos - vLegionTarget).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);   // not immortal
                    }
                }
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
                else nStageMiniChangeFunc(iStageMini + 1u);
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

                    for(coreUintW i = 0u; i < 20u; ++i)
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
                if(s_iTick < 200u)
                {
                    for(coreUintW i = 0u; i < 2u; ++i)
                    {
                        if(i ? (s_iTick % 10u < 5u) : (s_iTick % 20u < 10u))
                        {
                            const coreVector2 vTarget = coreVector2::Direction(I_TO_F(s_iTick) / 80.0f * (i ? (2.0f*PI) : (0.5f*PI)));

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
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iRota, fSpeed, pDummy, -vPos, -vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
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
                    pBullet->Deactivate(true);
            });
        }

        STAGE_WAVE(4u, "EINUNDVIERZIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->KillHelpers();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
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
    // TODO 1: MAIN: helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
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

                const coreVector2 vDir = coreVector2::Direction(STAGE_TAKEOFF ? fAngleTo : (fAngleFrom + AngleDiff(fAngleTo + fShift, fAngleFrom) * 2.5f * TIME));
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

        STAGE_WAVE(5u, "ZWEIUNDVIERZIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pGame->KillHelpers();

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
        UNUSED STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, GEMINGA_ENEMIES_TELEPORT)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_PURPLE);
                pEnemy->AddStatus(ENEMY_STATUS_WORTHLESS);
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

        STAGE_BOSS(m_Geminga, {60.0f, 120.0f, 180.0, 240.0f})
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