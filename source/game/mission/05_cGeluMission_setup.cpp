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
// setup the Gelu mission
void cGeluMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_INTRO)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pEnvironment->ChangeBackground(cVolcanoBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        g_pEnvironment->SetTargetSpeedNow(6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance (split)
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
    {
        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->GetInterface()->ShowMission(this);
        }

        STAGE_FINISH_AFTER(MISSION_WAIT_PLAY)
    });

    // ################################################################
    // change background appearance (split)
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // the walls are coming at you
    // 1) erste sub-gruppe muss (schnell) still stehen, damit der spieler die wände bewegen sieht
    // 2) weitere flache sub-gruppen um mechanik stärker begreifbar zu machen, e.g. dass alle wände betroffen sind
    // gleich nach erste gruppe spikes erzeugen kommt schnell zum geschäft nachdem mechanik gezeigt wurde, geht gut
    // gegner durch wand-verschiebung zu verstecken generiert nervige pausen, besser die gegner flugbahn mitverschieben
    // wenn 2 wände aufeinander zukommen, sollten gegner von nicht den tunnel entlang fliegen, das macht es zu leicht
    // wenn wände auf spieler zukommen müssen flache gegner-gruppen (ohne bewegung) darüber stehen, damit spieler drauf schießt und sieht, dass er sie zurückdrängen kann
    // am ende müssen wände vom spieler zurückgedrückt und eingerastet werden
    // beim zusammendrücken sollten immer nur zwei wände betroffen sein, vier sind zu viel
    // gegner sollten nicht von mehr als 2 richtungen gleichzeitig kommen (außer eck-gruppe), sonst dreht sich spieler verwirrt im kreis
    // delay bevor wände sich zurückziehen ist schwer kontrollierbar und begreifbar
    // gruppe in den ecken zeigt das enemy-anchoring sehr gut und schaut lustig aus, sub-gruppe für speed-runner die sich die wände absichtlich herziehen
    // in der finalen phase lassen profis die mauern näher kommen um durch kurze wege schneller töten zu können
    // TODO 1: fix not overlaying game-area with thin wallpapers (maybe additional blocker)
    // TODO 1: fully disable collision with bullets, wall-collision is tested instead
    // TODO 1: handle pulse bullet penetration, handle wave/surge
    // TODO 1: helfer fliegt über schirm wärend spikes erzeugt werden, oder in/entlang eine der spike-lines!
    // TODO 1: water reflection and shadow pop in if enemies spawn in the middle, also bullets reflect in air -> definitiv kein wasser, space ?
    // TODO 1: wenn dodge zurück kommt, lass alle geschosse wegfliegen, wenn wand einrastet, ansonsten vielleicht in muster
    // TODO 1: don't smooth at the end when pushing walls to 0
    // TODO 1: vielleicht eck-gruppe als zweites
    // TODO 1: handle assert in SetArea (both single and coop)
    // TODO 1: fix away-moving wall being tracked incorrectly for collision/damage
    // TODO 1: gegner sieht man orsch unter den stacheln
    // TODO 1: etwas mus blinken oder reagieren bei treffern (e.g. die stacheln ?, eine unsichtbare linie am rand (im spielfield))
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
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
            pPath2->AddNode(coreVector2(-1.3f,-1.3f), coreVector2(1.0f,1.0f).Normalized());
            pPath2->AddStop(coreVector2(-0.9f,-0.9f), coreVector2(1.0f,1.0f).Normalized());
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f, 1.0f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,-1.0f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 65u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.25f);
                pEnemy->Configure(4, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(10u)
            STAGE_GET_FLOAT_ARRAY(afOffCurrent, 4u)
            STAGE_GET_FLOAT_ARRAY(afOffTarget,  4u)
            STAGE_GET_UINT       (iWallBullets)
            STAGE_GET_UINT       (iActive, iActive = BITLINE(POST_WALLS))
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  4u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  5u, 14u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 15u, 19u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 20u, 24u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 25u, 36u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 37u, 46u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 47u, 56u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 57u, 57u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 58u, 58u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 59u, 59u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 60u, 60u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 61u, 61u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 62u, 62u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 63u, 63u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 64u, 64u)
            else if(STAGE_SUB(16u)) STAGE_DELAY_START

            if(m_iStageSub == 6u)
            {
                g_pGame->GetBulletManagerEnemy()->ForEachBullet([](const cBullet* pBullet)
                {
                    g_pSpecialEffects->CreateSplashColor(pBullet->GetPosition(), 5.0f, 3u, COLOR_ENERGY_ORANGE);
                });
            }
        }

        //   3
        // 0   1
        //   2

        const coreUint8 iDefend = (m_iStageSub >= 8u) ? 2u : ((m_iStageSub >= 6u) ? 1u : 0u);
        const coreBool  bDelay  = pSquad1->IsFinished();

        coreVector2 vOffFrom  = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[0], afOffCurrent[2]);
        coreVector2 vOffTo    = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[1], afOffCurrent[3]);
        coreVector2 vAreaFrom = -FOREGROUND_AREA + vOffFrom;
        coreVector2 vAreaTo   =  FOREGROUND_AREA - vOffTo;

        if(iActive) g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
        {
            const coreVector2 vPos  = pBullet->GetPosition().xy();
            const coreVector2 vDir  = pBullet->GetFlyDir();
            const coreFloat   fPush = 0.005f * I_TO_F(pBullet->GetDamage()) * (iDefend ? -4.0f : 3.0f) * RCP(I_TO_F(g_pGame->GetNumPlayers()));

                 if(HAS_BIT(iActive, 0u) && (vPos.x < vAreaFrom.x) && (vDir.x < 0.0f)) {afOffTarget[0] += fPush; pBullet->Deactivate(true);}
            else if(HAS_BIT(iActive, 1u) && (vPos.x > vAreaTo  .x) && (vDir.x > 0.0f)) {afOffTarget[1] += fPush; pBullet->Deactivate(true);}
                 if(HAS_BIT(iActive, 2u) && (vPos.y < vAreaFrom.y) && (vDir.y < 0.0f)) {afOffTarget[2] += fPush; pBullet->Deactivate(true);}
            else if(HAS_BIT(iActive, 3u) && (vPos.y > vAreaTo  .y) && (vDir.y > 0.0f)) {afOffTarget[3] += fPush; pBullet->Deactivate(true);}
        });

        for(coreUintW i = 0u; i < POST_WALLS; ++i)
        {
            if(!HAS_BIT(iActive, i)) continue;

            if(iDefend)
            {
                if(bDelay)
                {
                    if((afOffTarget[i] <= 0.0f) && (afOffCurrent[i] <= 0.01f))
                    {
                        g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                        {
                            const coreUintW iWall = pBullet->GetDamage() - 1;
                            ASSERT(iWall < POST_WALLS)

                            if(iWall == i) pBullet->Deactivate(true);
                        });

                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                        REMOVE_BIT(iActive, i)
                        if(!iActive)
                        {
                            STAGE_DELAY_END
                        }

                        g_pPostProcessing->SetWallOffset(i, 0.0f);

                        continue;
                    }
                }

                if((iDefend == 1u) == (i < 2u)) afOffTarget[i] += 0.07f * TIME;
            }
            else
            {
                afOffTarget[i] -= 0.07f * TIME;
            }

            afOffTarget[i] = CLAMP(afOffTarget[i], 0.0f, 1.0f);

            const coreFloat fDiff = afOffTarget[i] - afOffCurrent[i];
            afOffCurrent[i] += SIGN(fDiff) * 0.9f * TIME * SmoothTowards(ABS(fDiff), 0.07f);

            g_pPostProcessing->SetWallOffset(i, afOffCurrent[i]);
            //g_pPostProcessing->SetWallOffset(i, afOffTarget[i]);
        }

        vOffFrom  = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[0], afOffCurrent[2]);   // re-calculated with new offsets
        vOffTo    = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[1], afOffCurrent[3]);
        vAreaFrom = -FOREGROUND_AREA + vOffFrom;
        vAreaTo   =  FOREGROUND_AREA - vOffTo;

        const coreVector2 vPlayerAreaFrom = PLAYER_AREA_DEFAULT.xy() + vOffFrom;
        const coreVector2 vPlayerAreaTo   = PLAYER_AREA_DEFAULT.zw() - vOffTo;

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(coreVector4(vPlayerAreaFrom, vPlayerAreaTo));
        });

        if(m_iStageSub >= 2u)
        {
            constexpr coreUintW iTotalBullets = POST_WALLS * 20u;

            if((iWallBullets < iTotalBullets) && STAGE_TICK_FREE(15.0f, 0.0f))
            {
                for(coreUintW i = 0u; i < POST_WALLS; ++i)
                {
                    const coreVector2 vPos = coreVector2(1.2f,1.2f);
                    const coreVector2 vDir = coreVector2(1.0f,0.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(i + 1, 0.0f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.5f);

                    iWallBullets += 1u;
                }

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
            }

            if(iWallBullets == iTotalBullets)
            {
                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    if(pPlayer->IsNormal())
                    {
                        const coreVector2 vPos = pPlayer->GetPosition().xy();

                        if((vPos.x <= vPlayerAreaFrom.x + CORE_MATH_PRECISION) ||
                           (vPos.x >= vPlayerAreaTo  .x - CORE_MATH_PRECISION) ||
                           (vPos.y <= vPlayerAreaFrom.y + CORE_MATH_PRECISION) ||
                           (vPos.y >= vPlayerAreaTo  .y - CORE_MATH_PRECISION))
                        {
                            pPlayer->TakeDamage(5u, ELEMENT_NEUTRAL, vPos);
                            std::memset(afOffTarget, 0, sizeof(coreFloat) * POST_WALLS);
                        }
                    }
                });
            }

            coreUintW aiIndex[POST_WALLS] = {};
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                const coreUintW iWall = pBullet->GetDamage() - 1;
                ASSERT(iWall < POST_WALLS)

                const coreFloat fOffset = 1.07f - 2.2f * afOffCurrent[iWall];
                const coreFloat fSide   = (I_TO_F(aiIndex[iWall]) - 9.5f) / 10.0f;

                coreVector2 vPos = coreVector2(fSide, fOffset) * FOREGROUND_AREA;
                coreVector2 vDir = coreVector2(0.0f, -1.0f);
                switch(iWall)
                {
                default: ASSERT(false)
                case 0u: vPos = -vPos.Rotated90(); vDir = -vDir.Rotated90(); break;
                case 1u: vPos =  vPos.Rotated90(); vDir =  vDir.Rotated90(); break;
                case 2u: vPos = -vPos;             vDir = -vDir;             break;
                case 3u: break;
                }

                pBullet->SetPosition(coreVector3(vPos, 0.0f));
                pBullet->SetFlyDir  (vDir);

                if(pBullet->GetFlyTime() == 0.0f) g_pSpecialEffects->CreateSplashColor(pBullet->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_ORANGE);

                ++aiIndex[iWall];
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, (((i >= 5u && i < 15u) || (i >= 37u && i < 47u)) ? 1.6f : 0.0f) + ((i >= 15u && i < 25u) ? (0.4f * I_TO_F(i % 5u)) : 0.0f))

            if(i >= 25u && i < 37u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((((i - 25u) % 3u) == 1u) ? 0.3f : 0.0f, (((i - 25u) % 3u) == 2u) ? 0.3f : 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 57u)
            {
                const coreSpline2* pPath = (i < 15u || (i >= 37u && i < 57u)) ? pPath1 : pPath3;

                if(pPath == pPath3) STAGE_REPEAT(pPath->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 5u) - 2.0f) * 0.2f, 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                coreFloat fHeight;
                switch(i - 57u)
                {
                default: ASSERT(false)
                case 3u: case 1u: fHeight =  0.6f; break;
                case 0u: case 5u: fHeight =  0.2f; break;
                case 2u: case 7u: fHeight = -0.2f; break;
                case 6u: case 4u: fHeight = -0.6f; break;
                }

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(fHeight, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }

                 if(i <  5u) {}
            else if(i < 10u) pEnemy->Rotate270();
            else if(i < 15u) pEnemy->Rotate90 ();
            else if(i < 20u) {}
            else if(i < 25u) pEnemy->Rotate180();
            else if(i < 28u) {}
            else if(i < 31u) pEnemy->Rotate90 ();
            else if(i < 34u) pEnemy->Rotate180();
            else if(i < 37u) pEnemy->Rotate270();
            else if(i < 42u) pEnemy->Rotate270();
            else if(i < 47u) pEnemy->Rotate90 ();
            else if(i < 52u) {}
            else if(i < 57u) pEnemy->Rotate180();
            else if(i == 57u || i == 59u || i == 60u || i == 63u) pEnemy->Rotate270();
            else if(i == 58u || i == 61u || i == 62u || i == 64u) pEnemy->Rotate270()->InvertX();

            coreUintW iAnchor = 0u;
                 if(i <  5u) iAnchor = BIT(4u);
            else if(i < 10u) iAnchor = BIT(0u);
            else if(i < 15u) iAnchor = BIT(1u);
            else if(i < 20u) {}
            else if(i < 25u) {}
            else if(i < 28u) iAnchor = BIT(0u) + BIT(3u);
            else if(i < 31u) iAnchor = BIT(0u) + BIT(4u);
            else if(i < 34u) iAnchor = BIT(1u) + BIT(4u);
            else if(i < 37u) iAnchor = BIT(1u) + BIT(3u);
            else if(i < 42u) iAnchor = BIT(0u);
            else if(i < 47u) iAnchor = BIT(1u);
            else if(i < 52u) iAnchor = BIT(4u);
            else if(i < 57u) iAnchor = BIT(3u);
            else if(i == 57u || i == 59u || i == 60u || i == 63u) iAnchor = BIT(0u);
            else if(i == 58u || i == 61u || i == 62u || i == 64u) iAnchor = BIT(1u);

            coreVector2 vPostFactor = (vAreaTo - vAreaFrom) / (FOREGROUND_AREA * 2.0f) + 0.3f;
            coreVector2 vPostOffset = (vAreaTo + vAreaFrom) * 0.5f;

                 if(HAS_BIT(iAnchor, 0u)) {vPostFactor.x = 1.0f; vPostOffset.x = vAreaFrom.x + FOREGROUND_AREA.x;}   // left   |
            else if(HAS_BIT(iAnchor, 1u)) {vPostFactor.x = 1.0f; vPostOffset.x = vAreaTo  .x - FOREGROUND_AREA.x;}   // right  |
            else if(HAS_BIT(iAnchor, 2u)) {vPostFactor.x = 1.0f; vPostOffset.x = 0.0f;}                              // middle |
                 if(HAS_BIT(iAnchor, 3u)) {vPostFactor.y = 1.0f; vPostOffset.y = vAreaFrom.y + FOREGROUND_AREA.y;}   // bottom -
            else if(HAS_BIT(iAnchor, 4u)) {vPostFactor.y = 1.0f; vPostOffset.y = vAreaTo  .y - FOREGROUND_AREA.y;}   // top    -
            else if(HAS_BIT(iAnchor, 5u)) {vPostFactor.y = 1.0f; vPostOffset.y = 0.0f;}                              // middle -

            pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() * vPostFactor + vPostOffset, 0.0f));
        });

        STAGE_WAVE("FÜNFUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset wallpaper offset
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        for(coreUintW i = 0u; i < POST_WALLS; ++i)
        {
            g_pPostProcessing->SetWallOffset(i, 0.0f);
        }

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(PLAYER_AREA_DEFAULT);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // geometry falls together and forms safe spots
    // gegner am anfang sollten den spieler nie in die enge treiben, den säulen auszuweichen ist anstrengend genug (TODO 1: das wird derzeit nicht gemacht)
    // erste säule gegenüber von erster gegnerwelle
    // kreuz-tunnel, erster gegner muss seitlich anfangen, letzter gegner oben, nach doppel-gegner nicht zurück zum vorherigen
    // TODO 1: beim start von letzter phase wackelt einmal alles bevor es sich bewegt ?
    // TODO 1: damage on crush, no movement between blocks, where to move when crushed ?
    // TODO 1: badge: one (single) stomp contains a special enemy to attack or item to collect like Dr Robotnik, in the 2. block to appear
    // TODO 1: final phase should switch from 110110110 to 10101 pattern at middle
    // TODO 1: blöcke müssen echte gegner sein, für bonus punkte und handling von pulse etc
    // TODO 1: move shake (and color management if not yet) to mission code, it's only visual
    // TODO 1: maybe focus on tunnel fight and remove stomping part (removes Sonic-endboss badge), though the side-move part is also boring
    // TODO 1: maybe switch each phase to making the area more smaller, and opening it up to a cross and movement
    // TODO 1: maybe blocks trying to crush you get immediately attackable
    // TODO 1: create better fang model with low-detail version
    // TODO 1: allow attacking and destroying blocks
    // TODO 1: smoke zwischen bewegenden steinen
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreFloat fStep = 0.44f;

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
            pPath2->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 65u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_GET_START(9u)
            STAGE_GET_FLOAT(fStompTime)
            STAGE_GET_UINT (iStompCount)
            STAGE_GET_UINT (iStompTarget)
            STAGE_GET_UINT (iStompTargetNext)
            STAGE_GET_FLOAT(fChangeDelay)
            STAGE_GET_FLOAT(fOffsetTotal)
            STAGE_GET_UINT (iOffsetTick)
            STAGE_GET_UINT (iOffsetSpawn)
            STAGE_GET_UINT (iShakeState)
        STAGE_GET_END

        iShakeState = 0u;
        STATIC_ASSERT(GELU_FANGS <= sizeof(iShakeState)*8u)

        const auto nFangPositionFunc = [](const coreUintW iIndex)
        {
            ASSERT(iIndex < GELU_FANGS)

            const coreUintW X = iIndex % GELU_FANGS_DIMENSION;
            const coreUintW Y = iIndex / GELU_FANGS_DIMENSION;

            return coreVector2(I_TO_F(X) - 2.0f, I_TO_F(Y) - 2.0f) * 0.44f;
        };

        const auto nSetFangPositionFunc = [&](const coreUintW iIndex, const coreVector2 vOffset)
        {
            ASSERT(iIndex < GELU_FANGS)

            m_aFangRaw[iIndex].SetPosition(coreVector3((nFangPositionFunc(iIndex) + vOffset) * FOREGROUND_AREA, 0.0f));
        };

        const auto nSetFangPositionDualFunc = [&](const coreUintW iIndex, const coreVector2 vOffset)
        {
            nSetFangPositionFunc(                    iIndex,  vOffset);
            nSetFangPositionFunc((GELU_FANGS - 1u) - iIndex, -vOffset);
        };

        const auto nTakeTimeFunc = [this](const coreFloat fDelay, const coreFloat fOffset, const coreBool bShake = true)
        {
            const coreFloat fOld = m_fStageSubTimeBefore - fOffset;
            const coreFloat fNew = m_fStageSubTime       - fOffset;

            if(bShake && InBetween(fDelay, fOld, fNew)) g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

            return CLAMP(fNew * RCP(fDelay), 0.0f, 1.0f);
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  4u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  5u,  9u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 10u, 14u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 15u, 19u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 20u, 24u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 25u, 29u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 30u, 30u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 31u, 31u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 32u, 32u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 33u, 33u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 34u, 35u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 36u, 36u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 37u, 37u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 38u, 39u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 40u, 59u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 60u, 64u)
            else if(STAGE_SUB(17u)) STAGE_DELAY_START_CLEAR
        }

        if(STAGE_BEGINNING)
        {
            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                this->EnableFang(i);
        }

        if(m_iStageSub == 1u)
        {
            const coreFloat fTime1 = nTakeTimeFunc(1.4f, 0.0f);
            const coreFloat fTime2 = nTakeTimeFunc(1.4f, 1.4f);
            const coreFloat fTime3 = nTakeTimeFunc(1.0f, 2.8f);

            const coreVector2 vPos1 = LERPBR(coreVector2(-2.2f, 0.0f), coreVector2(0.0f, 0.0f),  fTime1);
            const coreVector2 vPos2 = LERPBR(coreVector2( 2.2f, 0.0f), coreVector2(0.0f, 0.0f),  fTime2);
            const coreVector2 vUp   = LERPBR(coreVector2( 0.0f, 0.0f), coreVector2(0.0f, fStep), fTime3);

            for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
            {
                nSetFangPositionDualFunc(i,                        vPos1 - vUp);
                nSetFangPositionDualFunc(i + GELU_FANGS_DIMENSION, vPos2 - vUp);
            }
        }
        else if(m_iStageSub == 7u)
        {
            if(fStompTime) fChangeDelay += 1.0f * TIME;

            const coreFloat fWait  = nTakeTimeFunc(0.7f, 0.0f + fChangeDelay, false);
            const coreFloat fTime1 = nTakeTimeFunc(1.0f, 0.7f + fChangeDelay);
            const coreFloat fTime2 = nTakeTimeFunc(1.0f, 1.7f + fChangeDelay);
            const coreFloat fTime3 = nTakeTimeFunc(1.0f, 2.7f + fChangeDelay);
            const coreFloat fTime4 = nTakeTimeFunc(1.0f, 3.7f + fChangeDelay);
            const coreFloat fTime5 = nTakeTimeFunc(1.0f, 4.7f + fChangeDelay);

            if(fWait)
            {
                if(!fTime1)
                {
                    ADD_BIT(iShakeState,                     GELU_FANGS_DIMENSION)
                    ADD_BIT(iShakeState, (GELU_FANGS - 1u) - GELU_FANGS_DIMENSION)
                }
                else if(!fTime2)
                {
                    ADD_BIT(iShakeState,                     (GELU_FANGS_DIMENSION + 3u))
                    ADD_BIT(iShakeState, (GELU_FANGS - 1u) - (GELU_FANGS_DIMENSION + 3u))
                }
                else if(!fTime3)
                {
                    ADD_BIT(iShakeState,                     (GELU_FANGS_DIMENSION + 2u))
                    ADD_BIT(iShakeState, (GELU_FANGS - 1u) - (GELU_FANGS_DIMENSION + 2u))
                }
            }

            if(fTime4 <= 0.0f)
            {
                const coreVector2 vPos1 = LERPBR(coreVector2(0.0f, -3.0f * fStep), coreVector2(0.0f, 0.0f), fTime1);
                const coreVector2 vPos2 = LERPBR(coreVector2(0.0f,  3.0f * fStep), coreVector2(0.0f, 0.0f), fTime2);
                const coreVector2 vPos3 = LERPBR(coreVector2(0.0f,  1.0f * fStep), coreVector2(0.0f, 0.0f), fTime3);

                for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
                {
                    nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION,      ((i == 0u) ? coreVector2(0.0f,0.0f) : -vPos1));
                    nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION + 1u, ((i == 4u) ? coreVector2(0.0f,0.0f) : -vPos2));
                }
                for(coreUintW i = 0u; i < 2u; ++i)
                {
                    nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION + 2u, -vPos3);
                }
            }
            else if(fTime5 <= 0.0f)
            {
                const coreVector2 vPos = LERPBR(coreVector2(0.0f, 0.0f), coreVector2(fStep, 0.0f), fTime4);

                for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
                {
                    nSetFangPositionDualFunc(i + GELU_FANGS_DIMENSION, ((i <  3u) ? -vPos : coreVector2(0.0f,0.0f)));
                    nSetFangPositionDualFunc(i,                        ((i >= 2u) ?  vPos : coreVector2(0.0f,0.0f)));
                }
            }
            else
            {
                const coreVector2 vPos = LERPBR(coreVector2(0.0f, 0.0f), coreVector2(0.0f, fStep), fTime5);

                for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
                {
                    nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION,      (i <  3u) ? -vPos : coreVector2(0.0f,0.0f));
                    nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION + 1u, (i >= 2u) ?  vPos : coreVector2(0.0f,0.0f));
                }
                for(coreUintW i = 0u; i < 2u; ++i)
                {
                    nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION + 2u, HIDDEN_POS);
                }
            }
        }
        else if(m_iStageSub == 15u)
        {
            const coreFloat fTime = nTakeTimeFunc(1.0f, 0.0f);

            const coreVector2 vPos = LERPBR(coreVector2(0.0f, fStep), coreVector2(0.0f, 0.0f), fTime);

            for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
            {
                nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION + 1u, (i <  3u) ? -vPos : coreVector2(0.0f,0.0f));
                nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION,      (i >= 2u) ?  vPos : coreVector2(0.0f,0.0f));

                nSetFangPositionFunc(i * GELU_FANGS_DIMENSION + 2u, HIDDEN_POS);
            }
        }
        else if(m_iStageSub == 16u)
        {
            if(STAGE_BEGINNING2)
            {
                for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                {
                    nSetFangPositionFunc(i, coreVector2(0.0f,0.0f));

                    const coreVector2 vPos   = m_aFangRaw[i].GetPosition().xy();
                    const coreFloat   fShift = (((i / GELU_FANGS_DIMENSION) % 2u) ? -1.0f : 1.0f) * (vPos.x + (((i % GELU_FANGS_DIMENSION) < 2u) ? 0.0f : (fStep * FOREGROUND_AREA.x)));

                    m_aFangRaw[i].SetPosition(coreVector3(fShift, vPos.y, 0.0f));
                }
            }

            const coreFloat fSpeed = LERP(0.0f, 0.5f, MIN(m_fStageSubTime, 1.0f)) * TIME;

            const coreFloat fOffsetTotalPrev = fOffsetTotal;
            fOffsetTotal += fSpeed;

            if(F_TO_UI(fOffsetTotalPrev * RCP(fStep)) != F_TO_UI(fOffsetTotal * RCP(fStep)))
            {
                iOffsetTick += 1u;

                if((iOffsetTick % 3u) != 0u)
                {
                    for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
                    {
                        const coreFloat fShift = ((i % 2u) ? -1.0f : 1.0f) * -3.0f * fStep * FOREGROUND_AREA.x;
                        const coreUintW iIndex = i * GELU_FANGS_DIMENSION + ((GELU_FANGS_DIMENSION - 1u) - (iOffsetSpawn % GELU_FANGS_DIMENSION));

                        m_aFangRaw[iIndex].SetPosition(coreVector3(fShift, m_aFangRaw[iIndex].GetPosition().y, 0.0f));
                    }

                    iOffsetSpawn += 1u;
                }
                else if((iOffsetTick % 6u) == 3u)
                {
                    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                    {
                        const coreFloat fShift  = (((i - 60u) % 2u) ? -1.0f : 1.0f) * -3.0f * fStep * FOREGROUND_AREA.x;
                        const coreFloat fHeight = (I_TO_F(i - 60u) - 2.0f) * fStep * FOREGROUND_AREA.y;

                        pEnemy->SetPosition(coreVector3(fShift, fHeight, 0.0f));
                    });
                }
            }

            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            {
                const coreFloat fMove = (((i / GELU_FANGS_DIMENSION) % 2u) ? -1.0f : 1.0f) * fSpeed * FOREGROUND_AREA.x;

                m_aFangRaw[i].SetPosition(m_aFangRaw[i].GetPosition() + coreVector3(fMove, 0.0f, 0.0f));
            }

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                const coreFloat fMove = (((i - 60u) % 2u) ? -1.0f : 1.0f) * fSpeed * FOREGROUND_AREA.x;

                pEnemy->SetPosition(pEnemy->GetPosition() + coreVector3(fMove, 0.0f, 0.0f));
            });
        }
        else if(m_iStageSub == 17u)
        {
            if(STAGE_BEGINNING2)
            {
                for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                {
                    if(g_pForeground->IsVisibleObject(&m_aFangRaw[i]))
                    {
                        m_aFangRaw[i].SetColor3(coreVector3(COLOR_SHIP_BLUE));
                    }
                    else
                    {
                        this->DisableFang(i, false);
                    }
                }
            }

            if(std::none_of(m_aFangRaw, m_aFangRaw + GELU_FANGS, [](const cLodObject& oFang) {return oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE);}))
            {
                STAGE_DELAY_END
            }
        }

        if(m_iStageSub < 8u)
        {
            if(STAGE_TIME_BETWEEN(4.0f, 5.0f))
            {
                ADD_BIT(iShakeState, (4u * GELU_FANGS_DIMENSION) - GELU_FANGS_DIMENSION)
            }
            else if(STAGE_TIME_AFTER(5.0f) && ((m_iStageSub < 7u) || fStompTime))
            {
                const coreBool bUpsideDown = (iStompCount % 2u);

                const coreFloat fStompTimePrev = fStompTime;
                fStompTime += 0.9f * TIME;

                if(InBetween(0.5f, fStompTimePrev, fStompTime))
                {
                    const coreFloat fSide = g_pGame->FindPlayerDual(iStompCount % 2u)->GetPosition().x / FOREGROUND_AREA.x;

                    iStompTargetNext = CLAMP(F_TO_UI((fSide + 1.1f) / fStep), 0u, GELU_FANGS_DIMENSION - 1u);

                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                }

                if(fStompTime >= 0.5f)
                {
                    ADD_BIT(iShakeState, iStompTargetNext + (bUpsideDown ? (4u * GELU_FANGS_DIMENSION) : 0u))
                }

                if(fStompTime >= 1.2f)
                {
                    fStompTime   = 0.0f;
                    iStompCount  = iStompCount + 1u;
                    iStompTarget = iStompTargetNext;
                }

                const coreVector2 vFrom = coreVector2(0.0f, 3.0f * fStep * (bUpsideDown ? -1.0f : 1.0f));
                const coreVector2 vTo   = coreVector2(0.0f, 0.0f);
                const coreVector2 vPos  = LERP(vTo, vFrom, ABS(COS(DelayTime(fStompTime, 0.5f, 0.2f) * (1.0f*PI))));

                for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
                {
                    const coreBool bStill = (i == (bUpsideDown ? 4u : 0u));

                    nSetFangPositionFunc(iStompTargetNext + i * GELU_FANGS_DIMENSION, bStill ? vTo : vFrom);
                    nSetFangPositionFunc(iStompTarget     + i * GELU_FANGS_DIMENSION, bStill ? vTo : vPos);
                }
            }
        }

        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
        {
            cLodObject& oFang = m_aFangRaw[i];
            if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            if(HAS_BIT(iShakeState, i))
            {
                oFang.SetPosition(oFang.GetPosition() + coreVector3(Core::Rand->Float(-0.5f, 0.5f), 0.0f, 0.0f));   // TODO 1: remove random
                oFang.SetColor3  (COLOR_SHIP_ORANGE);
            }
            else
            {
                oFang.SetColor3(coreVector3(0.5f,0.5f,0.5f));
            }

            // copied from group "direction blocks"
            cPlayer::TestCollision(PLAYER_TEST_ALL, &oFang, [](cPlayer* OUTPUT pPlayer, const coreObject3D* pFang, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                const coreVector2 vDiff = AlongCrossNormal(pPlayer->GetOldPos() - pFang->GetPosition().xy());
                const coreVector2 vAbs  = vDiff.Processed(ABS);
                const coreVector2 vPos  = pPlayer->GetPosition().xy() * vAbs.yx() + pFang->GetPosition().xy() * vAbs + pFang->GetCollisionRange().xy() * vDiff;

                if(coreVector2::Dot(vDiff, pPlayer->GetMove()) > 0.0f) return;

                pPlayer->SetPosition(coreVector3(vPos, 0.0f));
            });

            const auto nBulletWayCollFunc = [&](cBullet* OUTPUT pBullet, const coreObject3D* pFang, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                pBullet->Deactivate(true);

                if(m_iStageSub == 17u) this->DisableFang(i, true);
            };
            Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &oFang, nBulletWayCollFunc);
            Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY,  &oFang, nBulletWayCollFunc);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i == 30u) ? 0.4f : ((i == 31u) ? 0.6f : 0.8f), (i < 25u) ? (0.2f * I_TO_F(i % 5u) + ((i < 5u) ? 4.0f : 0.0f)) : ((i < 40u) ? ((i == 30u) ? 3.0f : 0.0f) : ((i < 60u) ? (0.4f * I_TO_F(i - 40u)) : 0.0f)))

            const coreSpline2* pPath = (i < 10u) ? pPath2 : pPath1;

            if(pPath == pPath1) STAGE_REPEAT(pPath->GetTotalDistance())

            if(i < 30u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 10u || i >= 25u) ? ((I_TO_F(i % 5u) - 2.0f) * 0.2f) : (0.5f - 0.5f * I_TO_F((i - 10u) / 5u)), 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  5u) pEnemy->Rotate90 ();
                else if(i < 10u) pEnemy->Rotate270();
                else if(i < 15u) pEnemy->Rotate90 ();
                else if(i < 20u) pEnemy->Rotate270();
                else if(i < 25u) pEnemy->Rotate90 ();
                else if(i < 30u) pEnemy->Rotate270();
            }
            else if(i < 40u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                switch(i - 30u)
                {
                default: ASSERT(false)
                case 0u: pEnemy->Rotate270(); break;
                case 1u:                      break;
                case 2u: pEnemy->Rotate90 (); break;
                case 3u:                      break;
                case 4u: pEnemy->Rotate270(); break;
                case 5u: pEnemy->Rotate180(); break;
                case 6u: pEnemy->Rotate90 (); break;
                case 7u: pEnemy->Rotate180(); break;
                case 8u: pEnemy->Rotate90 (); break;
                case 9u:                      break;
                }
            }
            else if(i < 60u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((i - 40u) % 2u) ? -0.18f : 0.18f, 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                // nothing
            }

            if((i >= 30u && i < 60u) && STAGE_TICK_LIFETIME(15.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 1.0f, pEnemy, vPos,  vDir)->ChangeSize(1.6f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 1.0f, pEnemy, vPos, -vDir)->ChangeSize(1.6f);
            }
        });

        STAGE_WAVE("SECHSUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all fangs
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            this->DisableFang(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u, 3u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // blocks react to certain move or turn directions
    // arrows in your direction are transparent
    // use as shield from enemy attacks
    // erste reihe hat keine pfeile nach unten, und später keine nach oben, um sicher zu stellen, dass spieler am ende auf richtiger seite ist
    // erste reihe hat keine zwei gleichen pfeile hintereinander, außer bei letztem abschnitt
    // erster gegner muss verdeckt sein um kollision zu zeigen
    // zweite gruppe muss seitlich sein um flip mechanik zu zeigen
    // dritte gruppe zwingt spieler durch blöcke durch zu fliegen
    // line pattern should have no arrows down and only 1-turn changes, to make navigation easier in this first moving pattern
    // block pattern should not have same direction twice
    // checkerboard pattern should provide a distinct path to kill enemies as fast as possible (also in coop, equal amount of enemies for each player) (also every empty field has a single arrow it points at)
    // there should be an empty line between pattern changes and every pattern starts at 0 (deterministic)
    // TODO 1: high tide in nCreateWay
    // TODO 1: add correction for spawning (changes slightly due to rounding) [] []  []
    // TODO 1: breaking (speed) movement into one direction needs to align with 6-cube pattern along screen
    // TODO 1: player should not be able to shoot and move between (neighbor-)blocks
    // TODO 1: player should not be able to fly "through" blocks, can happen if they move against fly direction
    // TODO 1: coop: both need to be in same direction | depends on side the block is (could be confusing on left-right movement)
    // TODO 1: in coop, richtung ändert sich erst wenn beide in die selbe schauen, ansonsten bleibt sie unverändert (hmm, oder doch beide richtungen ?, is leichter zu kommunizieren)
    // TODO 1: aber was ist wenn blöcke nur auf der eigenen seite reagieren, und auch anders angeordnet sind (e.g. wenn linien von oben kommen ist link-rechts vertauscht), könnte lustig sein
    // TODO 1: blocks pressing player against wall need to make damage
    // TODO 1: block should not allow player to "press into", same with Fang objects in dynamic wall wave
    // TODO 1: allow rotation already at the border of arrows
    // TODO 1: player may forget on block-wave that he can block the bullets
    // TODO 1: enemies in the line wave and block wave need to have more variety per zone
    // TODO 1: how to show player that touching blocks is harmless, energy-effect is usually perceived as bad
    // TODO 1: dance dance revolution as badge (+ extra score ?)
    // TODO 1: maybe in line sub-stage have 2 empty lines between 1-2-1-2, and only use left and right
    // TWIST: (boss?) line of blocks fom left and right at the same time, but with different direction, can crush player
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        constexpr coreFloat fStep      = 0.36f;
        constexpr coreFloat fStepDelay = 0.067f;

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
            pPath2->AddNode(coreVector2(0.0f,  (4.0f * fStep - 0.0014f)), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f, -(4.0f * fStep - 0.0014f)), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f,  3.5f * fStep), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f, -2.5f * fStep), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 41u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(10, COLOR_SHIP_BLUE);
            });
        });

        STAGE_GET_START(6u)
            STAGE_GET_VEC2 (vSpawnTime)
            STAGE_GET_FLOAT(fSpawnOffset)
            STAGE_GET_FLOAT(fCheckerBreak)
            STAGE_GET_UINT (iCheckerCount)
            STAGE_GET_UINT (iPatternCount)
        STAGE_GET_END

        const auto nCreateWay = [this](const coreVector2 vPosition, const coreVector2 vDirection)
        {
            for(coreUintW i = 0u; i < GELU_WAYS; ++i)
            {
                coreObject3D* pGenerate = (*m_Way.List())[i];

                if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    this->EnableWay(i, vPosition, vDirection);
                    return;
                }
            }

            WARN_IF(true) {}
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  2u,  3u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1,  6u, 10u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 11u, 18u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 19u, 30u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1, 31u, 40u)
            else if(STAGE_SUB(9u))
            {
                for(coreUintW i = 0u; i < GELU_WAYS; ++i)
                    this->DisableWay(i, true);
            }

                 if(m_iStageSub == 2u) iPatternCount = (((iPatternCount + 1u) % 4u) < 2u) ? 1u : 0u;
            else if(m_iStageSub >= 5u) iPatternCount = 0u;

            fSpawnOffset = 0.74f + FRACT(vSpawnTime.y * fStepDelay) * fStep;
        }

        coreVector2 vMove;
             if(m_iStageSub < 6u) vMove = coreVector2(1.0f, 0.0f) * (TIME * 15.0f);
        else if(m_iStageSub < 9u) vMove = coreVector2(0.0f,-1.0f) * (TIME * 25.0f);
        else                      vMove = coreVector2(0.0f, 0.0f);

        if(m_iStageSub == 8u)
        {
            if(iCheckerCount == 18u)
            {
                fCheckerBreak += 1.0f * TIME;
                vMove         *= 1.0f - STEPH3(0.6f, 0.72f, fCheckerBreak);
            }
        }

        const coreVector2 vSpawnTimePre = vSpawnTime;
        vSpawnTime += vMove;

        if(m_iStageSub < 6u)
        {
            if(F_TO_SI(vSpawnTimePre.x * fStepDelay) != F_TO_SI(vSpawnTime.x * fStepDelay))
            {
                iPatternCount += 1u;

                const coreUint8 iStep = iPatternCount % 4u;

                     if(m_iStageSub == 1u) nCreateWay(coreVector2(-1.3f,0.0f) * FOREGROUND_AREA, StepRotated90((iStep == 2u) ? 0u : iStep));
                else if(m_iStageSub <  5u) nCreateWay(coreVector2(-1.3f,0.0f) * FOREGROUND_AREA, (iStep % 2u) ? coreVector2(1.0f,0.0f) : coreVector2(-1.0f,0.0f));
                else if(m_iStageSub == 5u) nCreateWay(coreVector2(-1.3f,0.0f) * FOREGROUND_AREA, coreVector2(0.0f,1.0f));
            }
        }
        else
        {
            if(F_TO_SI(vSpawnTimePre.y * fStepDelay) != F_TO_SI(vSpawnTime.y * fStepDelay))
            {
                iPatternCount += 1u;

                if(m_iStageSub == 6u)
                {
                    if(iPatternCount % 2u)
                    {
                        for(coreUintW i = 0u; i < 6u; ++i)
                        {
                            const coreUint8 iStep = (iPatternCount / 2u) % 4u;

                            nCreateWay(coreVector2((I_TO_F(i) - 2.5f) * fStep, 1.3f) * FOREGROUND_AREA, StepRotated90((iStep == 2u) ? 0u : iStep));
                        }
                    }
                }
                else if(m_iStageSub == 7u)
                {
                    if(iPatternCount >= 2u)
                    {
                        for(coreUintW i = 0u; i < 6u; ++i)
                        {
                            if(((iPatternCount / 2u) % 2u) == ((i / 2u) % 2u)) continue;

                            nCreateWay(coreVector2((I_TO_F(i) - 2.5f) * fStep, 1.3f) * FOREGROUND_AREA, StepRotated90(((iPatternCount / 2u) + (iPatternCount / 4u) + (i / 4u)) % 4u));
                        }
                    }
                }
                else if(m_iStageSub == 8u)
                {
                    if((iPatternCount >= 2u) && (iPatternCount < 8u))
                    {
                        for(coreUintW i = 0u; i < 6u; ++i)
                        {
                            if((iPatternCount % 2u) == (i % 2u)) continue;

                            iCheckerCount += 1u;

                            coreVector2 vDir;
                            switch(iCheckerCount)
                            {
                            default: ASSERT(false)
                            case 5u: case 10u: case 12u: case 17u:           vDir = coreVector2( 1.0f, 0.0f); break;
                            case 2u: case  7u: case  9u: case 14u:           vDir = coreVector2(-1.0f, 0.0f); break;
                            case 1u: case  3u: case  8u: case 13u: case 15u: vDir = coreVector2( 0.0f, 1.0f); break;
                            case 4u: case  6u: case 11u: case 16u: case 18u: vDir = coreVector2( 0.0f,-1.0f); break;
                            }

                            nCreateWay(coreVector2((I_TO_F(i) - 2.5f) * fStep, 1.3f) * FOREGROUND_AREA, vDir);
                        }
                    }
                }
            }
        }

        for(coreUintW i = 0u; i < GELU_WAYS; ++i)
        {
            coreObject3D* pWay = (*m_Way.List())[i];
            if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayActive, i)) continue;

            const coreVector2 vOldPos = pWay->GetPosition().xy();
            const coreVector2 vNewPos = vOldPos + vMove;

            pWay->SetPosition(coreVector3(vNewPos, 0.0f));

            if(g_pForeground->IsVisiblePoint(vOldPos, 1.3f) && !g_pForeground->IsVisiblePoint(vNewPos, 1.3f))
                this->DisableWay(i, false);

            if(coreVector2::Dot(g_pGame->FindPlayerSide(pWay->GetPosition().xy())->GetDirection().xy(), pWay->GetDirection().xy()) > 0.9f)
            {
                REMOVE_BIT(m_iWayVisible, i)
            }
            else
            {
                ADD_BIT(m_iWayVisible, i)

                cPlayer::TestCollision(PLAYER_TEST_ALL, pWay, [](cPlayer* OUTPUT pPlayer, const coreObject3D* pWay, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    const coreVector2 vDiff = AlongCrossNormal(pPlayer->GetOldPos() - pWay->GetPosition().xy());
                    const coreVector2 vAbs  = vDiff.Processed(ABS);
                    const coreVector2 vPos  = pPlayer->GetPosition().xy() * vAbs.yx() + pWay->GetPosition().xy() * vAbs + pWay->GetCollisionRange().xy() * vDiff;

                    if(coreVector2::Dot(vDiff, pPlayer->GetMove()) > 0.0f) return;

                    pPlayer->SetPosition(coreVector3(vPos, 0.0f));
                });

                const auto nBulletWayCollFunc = [](cBullet* OUTPUT pBullet, const coreObject3D* pWay, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    pBullet->Deactivate(true);
                };
                Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, pWay, nBulletWayCollFunc);
                Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY,  pWay, nBulletWayCollFunc);
            }
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)   // copied from group "force rotation"
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);

            if(!pPlayer->IsRolling())
            {
                const coreVector2 vPos = pPlayer->GetPosition().xy();

                for(coreUintW j = 0u; j < GELU_WAYS; ++j)
                {
                    coreObject3D* pWay = (*m_Way.List())[j];
                    if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !HAS_BIT(m_iWayActive, i) || pWay->GetAlpha()) continue;

                    const coreVector2 vSize = pWay->GetCollisionRange().xy();
                    const coreVector2 vDiff = MapToAxis(pWay->GetPosition().xy() - vPos, pWay->GetDirection().xy());

                    if((ABS(vDiff.x) < vSize.x) && (ABS(vDiff.y) < vSize.y))
                    {
                        pPlayer->SetDirection(pWay->GetDirection());
                        pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_TURN);
                        break;
                    }
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 11u) ? 0.5f : (25.0f / FOREGROUND_AREA.y), ((i < 1u) ? 1.9f : ((i < 11u) ? 0.0f : ((i < 19u) ? (I_TO_F((i - 11u) / 2u) * 2.0f * fStep - 0.17f) : ((i < 31u) ? (I_TO_F((i - 19u) / 3u) * 2.0f * fStep) : 0.0f)))) + ((i < 11) ? 0.0f : fSpawnOffset))

            if(i < 11u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 1u) ? 0.0f : ((i < 2u) ? 0.6f : (((i < 4u) ? (I_TO_F(i - 2u) * 0.3f + 0.5f) : ((i < 6u) ? (((i - 4u) % 2u) ? -0.6f : 0.6f) : (I_TO_F(i - 8u) * 0.3f))))), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 1u) {}
                else if(i < 2u) pEnemy->Rotate90 ();
                else if(i < 4u) pEnemy->Rotate270();
                else if(i < 5u) pEnemy->Rotate90 ();
                else if(i < 6u) pEnemy->Rotate270();
            }
            else if(i < 31u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((i < 19u) ? COS(fLifeTime * 2.0f) : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(((i < 19u) ? (((i - 11u) % 2u) ? -1.5f : 1.5f) : ((I_TO_F((i - 19u) % 3u) - 1.0f) * 2.0f)) * fStep, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                coreUintW X, Y;
                switch(i - 31u)
                {
                default: ASSERT(false)
                case 0u: X = 1u; Y = 1u; break;
                case 1u: X = 0u; Y = 2u; break;
                case 2u: X = 4u; Y = 2u; break;
                case 3u: X = 1u; Y = 3u; break;
                case 4u: X = 3u; Y = 3u; break;
                case 5u: X = 5u; Y = 3u; break;
                case 6u: X = 4u; Y = 4u; break;
                case 7u: X = 1u; Y = 5u; break;
                case 8u: X = 3u; Y = 5u; break;
                case 9u: X = 5u; Y = 5u; break;
                }

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(X) - 2.5f) * fStep, I_TO_F(Y) * fStep);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
            }

            if((i < 11u) && STAGE_TICK_LIFETIME(10.0f, 0.0f) && ((s_iTick % 10u) < 2u))
            {
                const coreUintW   iCount = (i < 6u) ? 4u : 16u;
                const coreVector2 vPos   = pEnemy->GetPosition().xy();
                const coreFloat   fBase  = ((i >= 1u && i < 6u) ? pEnemy->AimAtPlayerSideRev() : pEnemy->AimAtPlayerDual(((s_iTick % 20u) < 10u) ? 0u : 1u)).Angle();

                for(coreUintW j = iCount; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iCount - 1u) * 0.5f) * 8.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                }
            }
        });

        STAGE_WAVE("SIEBENUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all ways
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        for(coreUintW i = 0u; i < GELU_WAYS; ++i)
            this->DisableWay(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)   // copied from group "force rotation"
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // snap to grid
    // player should not be moved if his current orb gets destroyed, to not move him into possible attack without being able to know or react (but provide way to both move into desired valid orb from there, and to return to far valid node as fallback)
    // do not reduce movement-speed while shooting, feels sluggish
    // moving enemies between orbs cause waiting time and frustration, but moving on nodes causes too much pressure related to attacks (because fine evasion is not possible) -> no enemy should move, player needs to be able to kill them on his own pace
    // rotating orbs is impossible from a UX perspective
    // attacks while moving between orbs (with sluggishness) are too hard to control precisely
    // the spawn pattern should make sure that it is impossible for the single player to stand at a location where the next enemy might spawn
    // moving or stretching orbs is possible (e.g. inner 4 orbs to outer), but a meaningful enemy pattern related to it might be problematic
    // attack waves from above (guitar hero) (or any direction) while attacking enemies was ok, but cut due to time
    // TODO 1: entferne cross movement
    // TODO 1: bullets have different distances from each other
    // TODO 1: bullets should not disappear when causing a hit
    // TODO 1: kugerl die verschwinden sollen, aber der spieler noch belegt, sollen erst verschwinden nachdem er wegmoved (chained at corners)
    // TODO 1: orb wave special effects
    // TODO 1: die finale welle brauch noch einen twist
    // TODO 1: animation-offset for each gameplay object (everywhere!)
    // TODO 1: guitar hero as badge (+ extra score ?)
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreFloat fOrbLen = 0.5f;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f),           coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f, 1.5f) * fOrbLen, coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(8u);
            pPath2->AddNode (coreVector2( 0.5f, 0.5f) * fOrbLen, coreVector2(-1.0f, 0.0f));
            pPath2->AddNodes(coreVector2(-0.5f, 0.5f) * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath2->AddNodes(coreVector2(-0.5f,-0.5f) * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2( 1.0f, 0.0f));
            pPath2->AddNodes(coreVector2( 0.5f,-0.5f) * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath2->AddNode (coreVector2( 0.5f, 0.5f) * fOrbLen, coreVector2( 0.0f, 1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(8u);
            pPath3->AddNode (coreVector2(-1.5f, 1.5f) * fOrbLen, coreVector2( 1.0f, 0.0f));
            pPath3->AddNodes(coreVector2( 1.5f, 1.5f) * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath3->AddNodes(coreVector2( 1.5f,-1.5f) * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath3->AddNodes(coreVector2(-1.5f,-1.5f) * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath3->AddNode (coreVector2(-1.5f, 1.5f) * fOrbLen, coreVector2( 0.0f, 1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(4, COLOR_SHIP_MAGENTA);
            });
        });

        STAGE_GET_START(21u + 3u * GAME_PLAYERS)
            STAGE_GET_UINT64_ARRAY(aiSnakePtr, 10u)
            STAGE_GET_UINT        (iSnakeNum)
            STAGE_GET_UINT_ARRAY  (aiTarget,  GAME_PLAYERS)
            STAGE_GET_VEC2_ARRAY  (avOldMove, GAME_PLAYERS)
        STAGE_GET_END

        // 12 13 14 15
        // 08 09 10 11
        // 04 05 06 07
        // 00 01 02 03

        const auto nPosFrom = [](const coreUintW iIndex)
        {
            return coreVector2::Direction(I_TO_F(iIndex) / I_TO_F(GELU_ORBS - 1u) * (9.0f*PI)) * FOREGROUND_AREA * 2.0f;   // -1 looks better
        };

        const auto nPosTo = [](const coreUintW iIndex)
        {
            return coreVector2(I_TO_F(iIndex % 4u) - 1.5f, I_TO_F(iIndex / 4u) - 1.5f) * FOREGROUND_AREA * fOrbLen;
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  4u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  5u,  5u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  6u,  6u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 10u, 10u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 11u, 11u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 12u, 12u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 13u, 13u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 14u, 14u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 16u, 16u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 17u, 17u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 18u, 18u)
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 19u, 19u)
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad1, 20u, 21u)
            else if(STAGE_SUB(22u)) STAGE_RESURRECT(pSquad1, 22u, 23u)
            else if(STAGE_SUB(23u)) STAGE_RESURRECT(pSquad1, 24u, 25u)
            else if(STAGE_SUB(24u)) STAGE_RESURRECT(pSquad1, 26u, 27u)
            else if(STAGE_SUB(25u)) STAGE_RESURRECT(pSquad1, 28u, 35u)
            else if(STAGE_SUB(26u))
            {
                for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                    this->DisableOrb(i, true);

                STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
                });
            }

                 if(m_iStageSub ==  4u) this->DisableOrb( 3u, true);
            else if(m_iStageSub ==  6u) this->DisableOrb(15u, true);
            else if(m_iStageSub ==  9u) this->DisableOrb( 0u, true);
            else if(m_iStageSub == 11u) this->DisableOrb(12u, true);
            else if(m_iStageSub == 14u) this->DisableOrb( 8u, true);
            else if(m_iStageSub == 15u) this->DisableOrb(14u, true);
            else if(m_iStageSub == 16u) this->DisableOrb( 1u, true);
            else if(m_iStageSub == 17u) this->DisableOrb( 7u, true);
            else if(m_iStageSub == 18u) this->DisableOrb(13u, true);
            else if(m_iStageSub == 19u) this->DisableOrb( 4u, true);
            else if(m_iStageSub == 20u) this->DisableOrb(11u, true);
            else if(m_iStageSub == 21u) this->DisableOrb( 2u, true);
        }

#if 0
        if(m_iStageSub == 25u)
        {
            const coreUintW aiExpand[] = {5u, 6u, 9u, 10u};
            const coreFloat fValue     = LERPS(1.0f, 3.0f, MIN(m_fStageSubTime * 0.5f, 1.0f));

            for(coreUintW i = 0u; i < ARRAY_SIZE(aiExpand); ++i)
                m_aOrbRaw[aiExpand[i]].SetPosition(coreVector3(nPosTo(aiExpand[i]) * fValue, 0.0f));
        }
#endif

        if(STAGE_BEGINNING)
        {
            for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                this->EnableOrb(i);
        }

        if(STAGE_TIME_BEFORE(3.0f))
        {
            for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                m_aOrbRaw[i].SetPosition(coreVector3(LERPB(nPosFrom(i), nPosTo(i), CLAMP(m_fStageTime - 2.0f * (1.0f - (I_TO_F(i) / I_TO_F(GELU_ORBS - 1u))), 0.0f, 1.0f)), 0.0f));
        }
        else if(STAGE_TIME_POINT(3.0f))
        {
            for(coreUintW i = 0u; i < GELU_LINES; ++i)
                this->EnableLine(i);

            for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                m_aOrbRaw[i].SetPosition(coreVector3(nPosTo(i), 0.0f));

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                coreUintW iIndex = 0u;
                coreFloat fLenSq = FLT_MAX;

                for(coreUintW j = 0u; j < GELU_ORBS; ++j)
                {
                    const coreFloat fCurLenSq = (m_aOrbRaw[j].GetPosition().xy() - pPlayer->GetPosition().xy()).LengthSq();
                    if(fCurLenSq < fLenSq)
                    {
                        iIndex = j;
                        fLenSq = fCurLenSq;
                    }
                }

                aiTarget [i] = iIndex;
                avOldMove[i] = pPlayer->GetInput()->vMove;

                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);
            });
        }
        else
        {
            const auto nChangeTarget = [&](const coreUintW iIndex, const coreUint32 iNewTarget)
            {
                ASSERT(iNewTarget < GELU_ORBS)

                if(m_afOrbTime[iNewTarget] > 0.0f)
                    aiTarget[iIndex] = iNewTarget;

                if(m_afOrbTime[aiTarget[iIndex]] < 0.0f)   // fallback
                {
                    const coreUint32 A = aiTarget[iIndex];
                         if((A ==  0u) || (A ==  1u) || (A ==  4u)) aiTarget[iIndex] =  5u;
                    else if((A ==  2u) || (A ==  3u) || (A ==  7u)) aiTarget[iIndex] =  6u;
                    else if((A ==  8u) || (A == 12u) || (A == 13u)) aiTarget[iIndex] =  9u;
                    else if((A == 11u) || (A == 14u) || (A == 15u)) aiTarget[iIndex] = 10u;
                }
            };

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(!pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE)) return;

                const coreUint8   x      = (aiTarget[i] % 4u);
                const coreUint8   y      = (aiTarget[i] / 4u);
                const sGameInput* pInput = pPlayer->GetInput();

                if(SIGNUM(avOldMove[i].x) != SIGNUM(pInput->vMove.x))
                {
                         if((x > 0u) && (pInput->vMove.x < 0.0f)) nChangeTarget(i, aiTarget[i] - 1u);
                    else if((x < 3u) && (pInput->vMove.x > 0.0f)) nChangeTarget(i, aiTarget[i] + 1u);
                }
                if(SIGNUM(avOldMove[i].y) != SIGNUM(pInput->vMove.y))
                {
                         if((y > 0u) && (pInput->vMove.y < 0.0f)) nChangeTarget(i, aiTarget[i] - 4u);
                    else if((y < 3u) && (pInput->vMove.y > 0.0f)) nChangeTarget(i, aiTarget[i] + 4u);
                }

                avOldMove[i] = pInput->vMove;

                const coreVector2 vDiff = m_aOrbRaw[aiTarget[i]].GetPosition().xy() - pPlayer->GetPosition().xy();
                if(!vDiff.IsNull())
                {
                    const coreVector2 vPos = pPlayer->GetPosition().xy() + vDiff.Normalized() * (150.0f * TIME * SmoothTowards(vDiff.Length(), 3.0f));
                    pPlayer->SetPosition(coreVector3(vPos, 0.0f));
                }
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.5f, ((i < 1u) ? 7.0f : 0.0f))

            coreUintW iOffset, iRota;
            switch(i)
            {
            default: ASSERT(false)
            case  0u: iOffset = 2u; iRota = 3u; break;
            case  1u: iOffset = 2u; iRota = 0u; break;
            case  2u: iOffset = 0u; iRota = 2u; break;
            case  3u: iOffset = 1u; iRota = 3u; break;
            case  4u: iOffset = 0u; iRota = 1u; break;
            case  5u: iOffset = 1u; iRota = 2u; break;
            case  6u: iOffset = 1u; iRota = 0u; break;
            case  7u: iOffset = 0u; iRota = 3u; break;
            case  8u: iOffset = 1u; iRota = 1u; break;
            case  9u: iOffset = 0u; iRota = 0u; break;
            case 10u: iOffset = 2u; iRota = 2u; break;
            case 11u: iOffset = 2u; iRota = 1u; break;
            case 12u: iOffset = 2u; iRota = 3u; break;
            case 13u: iOffset = 2u; iRota = 0u; break;
            case 14u: iOffset = 2u; iRota = 2u; break;
            case 15u: iOffset = 2u; iRota = 1u; break;
            case 16u: iOffset = 1u; iRota = 0u; break;
            case 17u: iOffset = 1u; iRota = 3u; break;
            case 18u: iOffset = 1u; iRota = 1u; break;
            case 19u: iOffset = 1u; iRota = 2u; break;

            case 20u: iOffset = 2u; iRota = 3u; break;
            case 21u: iOffset = 2u; iRota = 0u; break;
            case 22u: iOffset = 2u; iRota = 2u; break;
            case 23u: iOffset = 2u; iRota = 1u; break;
            case 24u: iOffset = 1u; iRota = 0u; break;
            case 25u: iOffset = 1u; iRota = 3u; break;
            case 26u: iOffset = 1u; iRota = 1u; break;
            case 27u: iOffset = 1u; iRota = 2u; break;
            case 28u: iOffset = 2u; iRota = 3u; break;
            case 29u: iOffset = 2u; iRota = 0u; break;
            case 30u: iOffset = 2u; iRota = 2u; break;
            case 31u: iOffset = 2u; iRota = 1u; break;
            case 32u: iOffset = 1u; iRota = 0u; break;
            case 33u: iOffset = 1u; iRota = 3u; break;
            case 34u: iOffset = 1u; iRota = 1u; break;
            case 35u: iOffset = 1u; iRota = 2u; break;
            }

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((I_TO_F(iOffset) - 1.5f) * fOrbLen, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            switch(iRota)
            {
            default: ASSERT(false)
            case 0u:                      break;
            case 1u: pEnemy->Rotate90 (); break;   // right
            case 2u: pEnemy->Rotate180(); break;
            case 3u: pEnemy->Rotate270(); break;   // left
            }
        });

        if((iSnakeNum < 8u) && STAGE_SUBTIME_AFTER(3.5f) && STAGE_TICK_FREE(9.0f, 0.0f))
        {
            const coreVector2 vPos = m_aOrbRaw[3].GetPosition().xy();
            const coreVector2 vDir = coreVector2(1.0f,0.0f);

            aiSnakePtr[iSnakeNum++] = P_TO_UI(g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.7f));
        }

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cWaveBullet>([&](cWaveBullet* OUTPUT pBullet)
        {
            const coreUintW i = std::find(aiSnakePtr, aiSnakePtr + iSnakeNum, P_TO_UI(pBullet)) - aiSnakePtr;
            if(i >= iSnakeNum) return;

            const coreSpline2* pPath = (i < 4u) ? pPath2 : pPath3;
            const coreFloat    fTime = FMOD(pBullet->GetFlyTime() * 0.9f, pPath->GetTotalDistance());

            coreVector2 vPosition;
            coreVector2 vDirection;
            pPath->CalcPosDir(CLAMP(fTime, 0.0f, pPath->GetTotalDistance()), &vPosition, &vDirection);

            pBullet->SetPosition(coreVector3(vPosition * FOREGROUND_AREA, 0.0f));
            pBullet->SetFlyDir  (vDirection);
        });

        STAGE_WAVE("ACHTUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all lines and orbs
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        for(coreUintW i = 0u; i < GELU_LINES; ++i)
            this->DisableLine(i, false);

        for(coreUintW i = 0u; i < GELU_ORBS; ++i)
            this->DisableOrb(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u, 5u, 6u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // bend
    // cannot throw enemies at each other, may cause unstable movement  
    // cannot let all enemies of a sub-wave pass a single point, too easy (includes circle movement)  
    // push-strength needs to be scaled to make sure that a whole 2x2 group can be killed in a single sweep by grazing one of their sides  
    //     on first sub-wave center group needs to move into player, as they will naturally try to shoot and dodge them, see the mechanic, and evade their first bullet-wave  
    // make this wave easier for advanced players, as the enemy-movement might not be as deterministic  
    // difference between attackable and non-attackable must be very clear
    //     on first wave, one of the up-flying enemies need to fly first with some delay, to show the bullet-attack
    // coop: player not receiving two groups in second sub-wave needs to receive two groups in first sub-wave
    // TODO 1: completely disable player-enemy collision, just in case
    // TODO 1: enemies coming from all sides at the same time
    // TODO 1: enemies want to stay with you, can only be killed when far away
    // TODO 1: remove bend ?
    // TODO 1: fixed fields where enemies bend (gradius mines)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 100u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.3f,1.3f,1.3f));
                pEnemy->Configure(4, COLOR_SHIP_GREY);

                d_cast<cStarEnemy*>(pEnemy)->SetAngle(I_TO_F(i) * 2.0f/5.0f * PI);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 11u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 12u, 35u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 36u, 99u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vAlong = (i >= 12u && i < 36u) ? coreVector2(-1.0f,0.0f) : coreVector2(0.0f,1.0f);
            const coreVector2 vSide  = coreVector2(1.0f, ((i < 36u) && (((i + 4u) % 12u) < 8u)) ? -1.0f : 1.0f);

            const coreMatrix2 mTransform    = coreMatrix3::Scaling(vSide).m12() * coreMatrix3::Rotation(vAlong).m12();
            const coreMatrix2 mTransformRev = mTransform.Inverted();

            coreVector2 vTarget;
            if(i < 36u)
            {
                const coreUintW x  = ((i / 4u) % 3u);
                const coreUintW y  = ((i / 4u) / 3u) % 2u;
                const coreUintW x2 = (i / 2u) % 2u;
                const coreUintW y2 = (i % 2u);

                vTarget   = coreVector2(-0.7f + 0.7f * I_TO_F(x), -0.5f + 1.2f * I_TO_F(y) + 0.1f * I_TO_F(x2)) + 0.2f * coreVector2(-0.5f + 1.0f * I_TO_F(x2), -0.5f + 1.0f * I_TO_F(y2));
                vTarget.y = vTarget.y - ((i < 12u) ? 1.0f : 1.2f) * fLifeTime + 0.8f * I_TO_F((x + 1u) % 3u) + 2.0f;
            }
            else
            {
                const coreUintW x = (i - 36u) % 8u;
                const coreUintW y = (i - 36u) / 8u;

                vTarget   = coreVector2(-0.9f + 0.25f * I_TO_F(x), 1.2f - 0.3f * I_TO_F(y) + 0.15f * I_TO_F(x % 2u));
                vTarget.y = vTarget.y - 1.4f * fLifeTime + 2.4f;
            }

            if(STAGE_TAKEOFF) pEnemy->SetPosition(coreVector3((vTarget * FOREGROUND_AREA + coreVector2(0.0f,1.0f)) * mTransform, 0.0f));   // make sure to add offset due to imprecision

            const coreVector2 vEnemyPos  = pEnemy->GetPosition().xy() * mTransformRev;
            const coreVector2 vEnemyMove = pEnemy->GetMove()          * mTransformRev;

            coreVector2 vWrap = vTarget * FOREGROUND_AREA;
            if(vWrap    .y < 0.0f)    vWrap.y = FmodRange(vWrap.y, -1.2f * FOREGROUND_AREA.y, 1.2f * FOREGROUND_AREA.y);
            if(vEnemyPos.y < vWrap.y) vWrap.y = vWrap.y - 2.4f * FOREGROUND_AREA.y;

            pEnemy->SetBaseColor(COLOR_SHIP_GREY);
            pEnemy->AddStatus   (ENEMY_STATUS_INVINCIBLE);

            STAGE_FOREACH_PLAYER(pPlayer, j)
            {
                const coreVector2 vPlayerPos = pPlayer->GetPosition().xy() * mTransformRev;

                const coreVector2 vDiff = vEnemyPos - vPlayerPos;
                const coreVector2 vDir  = vDiff.Normalized();
                const coreFloat   fLen  = vDiff.Length();
                const coreFloat   fStr  = MAX(24.0f - fLen, 0.0f) / 24.0f;
                const coreFloat   fDot  = vEnemyMove.IsNull() ? 0.5f : MAX(coreVector2::Dot(vDir, -vEnemyMove.Normalized()), 0.0f);

                const coreVector2 vRealTarget = LERP(vWrap, vEnemyPos + vDir * 35.0f, fStr);
                const coreVector2 vChange     = vRealTarget - vEnemyPos;

                pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + (vChange * ((fStr * fDot * 9.0f + 2.0f) * TIME)) * mTransform, 0.0f));

                if(fStr)
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_RED);
                    pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                }
            });

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
            {
                if(pEnemy == pEnemy2) return; // TODO 1: check for (j <= i), or add note for double-calculations

                const coreVector2 vDiff  = pEnemy->GetPosition().xy() - pEnemy2->GetPosition().xy();
                const coreFloat   fPower = POW2(pEnemy->GetCollisionRadius() * 2.0f) - vDiff.LengthSq();

                if((fPower > 0.0f) && !vDiff.IsNull())
                {
                    constexpr coreVector2 vBase = coreVector2(0.0f,0.0f);

                    const coreFloat A = (pEnemy ->GetPosition().xy() - vBase).LengthSq();
                    const coreFloat B = (pEnemy2->GetPosition().xy() - vBase).LengthSq();

                    const coreVector2 vPush = vDiff.Normalized() * (fPower * TIME);

                    if(A > B) pEnemy ->SetPosition(coreVector3(pEnemy ->GetPosition().xy() + vPush, 0.0f));
                         else pEnemy2->SetPosition(coreVector3(pEnemy2->GetPosition().xy() - vPush, 0.0f));
                }
            });

            coreVector2 vCurPos = pEnemy->GetPosition().xy() * mTransformRev;

                                 vCurPos.x = CLAMP(vCurPos.x, -1.08f * FOREGROUND_AREA.x, 1.08f * FOREGROUND_AREA.x);
            if(vCurPos.y < 0.0f) vCurPos.y = FmodRange(vCurPos.y, -1.2f * FOREGROUND_AREA.y, 1.2f * FOREGROUND_AREA.y);

            pEnemy->SetPosition(coreVector3(vCurPos * mTransform, 0.0f));

            const coreBool bInsideOld = g_pForeground->IsVisiblePoint(pEnemy->GetOldPos()        * vAlong);
            const coreBool bInsideNew = g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy() * vAlong);
            const coreBool bForward   = (coreVector2::Dot(pEnemy->GetMove(), vAlong.InvertedX() * vSide.y) < 0.0f);

            if(((bInsideOld && !bInsideNew) || (g_pGame->IsHard() && !bInsideOld && bInsideNew)) && bForward)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = ((i < 12u) ? pEnemy->AimAtPlayerDual((i / 8u) % 2u) : pEnemy->AimAtPlayerSide()).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, vDir)->ChangeSize(1.4f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 5.0f, 3u, COLOR_ENERGY_ORANGE);
            }
        });

        STAGE_WAVE("NEUNUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // push
    // bullets fly fast to give visual waves with space inbetween (but should not be shot too often to compensate) 
    // in 4 player should not be able to pass when enemies are axis aligned 
    // in 3 only five enemies / half the circle should peek out 
    // in 2 lines should move over the center 
    // in 1 enemies should not reach bottom 
    // in 1 and 2 back of enemies should not be reachable without shooting at them first 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2( 0.0f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 48u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.27f,1.27f,1.27f));
                pEnemy->Configure((i < 32u) ? 50 : 200, COLOR_SHIP_ORANGE);

                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));

                if(i < 40u)
                {
                    const coreVector2 vPos = (i < 32u) ? coreVector2((I_TO_F(i % 8u) - 3.5f) * 0.24f, (i < 8u) ? 1.3f : 1.54f) : (coreVector2(0.0f,1.74f) + 0.32f * coreVector2::Direction(I_TO_F(i % 8u) * (0.25f*PI)));
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                    pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir,                   0.0f));

                         if(i >= 16u && i < 24u) pEnemy->Rotate270();
                    else if(i >= 24u && i < 32u) pEnemy->Rotate90();
                }
            });
        });

        STAGE_GET_START(82u)
            STAGE_GET_VEC2_ARRAY(avForce, 40u, for(coreUintW i = 0u; i < 40u; ++i) avForce[i] = 85.0f * pSquad1->GetEnemy(i)->GetDirection().xy();)
            STAGE_GET_FLOAT     (fMillForce)
            STAGE_GET_FLOAT     (fMillAngle, fMillAngle = 0.5f*PI)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_VAL(pSquad1), COLL_VAL(avForce), COLL_REF(fMillForce))
        {
            if(!bFirstHit) return;

            const coreUintW i = pSquad1->GetIndex(pEnemy);

            coreUint8 iGroupNum;
                 if(i < 16u) iGroupNum = 2u;
            else if(i < 32u) iGroupNum = 4u;
            else             iGroupNum = 8u;

            const coreFloat fPower = I_TO_F(pBullet->GetDamage()) * RCP(I_TO_F(g_pGame->GetNumPlayers()));

            coreVector2 vForceDir;
                 if(i < 16u) vForceDir   = fPower * 6.0f * coreVector2( 0.0f,-1.0f);
            else if(i < 24u) vForceDir   = fPower * 6.0f * coreVector2( 1.0f, 0.0f);
            else if(i < 32u) vForceDir   = fPower * 6.0f * coreVector2(-1.0f, 0.0f);
            else if(i < 40u) vForceDir   = fPower * 6.0f * AlongCrossNormal(-pBullet->GetFlyDir());
            else             fMillForce -= fPower * 0.4f;

            for(coreUintW j = coreMath::FloorAlign(i, iGroupNum), je = coreMath::CeilAlign(i + 1u, iGroupNum); j < je; ++j)
            {
                if(i < 40u) avForce[j] += vForceDir;
                if(i != j)  pSquad1->GetEnemy(j)->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()));
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 15u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 16u, 31u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 32u, 39u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 40u, 47u)
        }

        if(fMillForce)
        {
            fMillAngle += fMillForce * TIME;
            fMillForce *= FrictionFactor(6.0f);
        }

        const coreVector2 vMillDir = coreVector2::Direction(fMillAngle);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.4f, 0.0f)

            if(i < 40u)
            {
                coreVector2 vNewPos = pEnemy->GetPosition().xy();

                if(!avForce[i].IsNull())
                {
                    vNewPos    += avForce[i] * TIME;
                    avForce[i] *= FrictionFactor(3.0f);
                }

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else
            {
                const coreFloat   fOffset = (fLifeTime < pPath1->GetTotalDistance()) ? (pPath1->CalcPosition(fLifeTime).x * ((i < 44u) ? 1.0f : -1.0f)) : 0.0f;
                const coreVector2 vNewPos = vMillDir * ((I_TO_F(i % 8u) - 3.5f) * 0.26f + fOffset) * FOREGROUND_AREA;

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            if(STAGE_TICK_LIFETIME(20.0f * (0.8f / fLifeSpeed), 0.0f) && ((s_iTick % 20u) >= 18u))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual((i < 16u || i >= 40u) ? ((i / 4u) % 2u) : ((i < 32u) ? ((i / 8u) % 2u) : (i % 2u))).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.25f);
            }
        });

        STAGE_WAVE("DREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetSpeed(0.0f, 1.0f);
            c_cast<coreFloat&>(g_pEnvironment->GetSpeed()) = 0.0f;                 
        }

        STAGE_BOSS(m_Chol, {60.0f, 120.0f, 180.0, 240.0f})
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