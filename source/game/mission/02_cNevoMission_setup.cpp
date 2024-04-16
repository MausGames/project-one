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
// setup the Nevo mission
void cNevoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
        g_pEnvironment->SetTargetSpeed(4.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        g_pGame->GetInterface()->ShowMission(this);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u, 2u, 3u, 4u, 5u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // bomberman hero
    // gegner fliegt auf den spieler zu, weniger nervig, spieler muss trotzdem manchmal ausweichen, lebenspunkte müssen dementsprechend hoch sein um den burst auszugleichen
    // diagonal position makes linear bombs stronger, linear position makes diagonal bombs stronger
    // make sure there is a pause between 4 and 5, make sure to start 5 from different side then 4
    // consistent explosion delays make it easier to focus one the enemies simultaneously
    // letting the enemy spawn bombs made the pattern too simple, a separate spawner creates more pressure
    // TODO: if not used otherwise, hide helper in bomb, it flies away on burst-line
    // TODO: maybe two from XY at the same time
    // TODO: maybe only diagonal position on linear explosion and vice-versa
    // TODO: 2-3 quickly after another, with pattern or just aim
    // TODO: reference: Monolith
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 24u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(50, COLOR_SHIP_RED);
            });
        });

        STAGE_GET_START(8u + NEVO_BOMBS * 4u)
            STAGE_GET_VEC2       (vClusterPos, vClusterPos = coreVector2(-1.2f,0.5f) * FOREGROUND_AREA)
            STAGE_GET_VEC2       (vClusterDir, vClusterDir = coreVector2( 1.0f,0.0f))
            STAGE_GET_UINT       (iCurBomb)
            STAGE_GET_UINT       (iCurEnemy)
            STAGE_GET_UINT       (iWarpCount)
            STAGE_GET_UINT       (iRotaCount)
            STAGE_GET_FLOAT_ARRAY(afDelay, NEVO_BOMBS)
            STAGE_GET_VEC2_ARRAY (avMove,  NEVO_BOMBS)
            STAGE_GET_UINT_ARRAY (aiType,  NEVO_BOMBS)
        STAGE_GET_END

        const auto nTargetFunc = [](const coreBool bType, const coreVector2& vTarget, coreVector2* OUTPUT pvPos, coreVector2* OUTPUT pvForce)
        {
            ASSERT(pvPos && pvForce)

            const coreVector2 vAxis  = bType ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
            const coreVector2 vSign  = vTarget.Processed(SIGN) * FOREGROUND_AREA;
            const coreVector2 vClamp = vTarget.Processed(CLAMP, -(FOREGROUND_AREA.x - 10.0f), (FOREGROUND_AREA.x - 10.0f));

            (*pvPos)   = -1.2f * vSign * vAxis + vClamp * vAxis.yx();
            (*pvForce) = (vTarget + vSign) * vAxis;
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  8u, 12u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 13u, 17u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 18u, 23u)
        }

        if(STAGE_CLEARED)
        {
            for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
                this->DisableBomb(i, true);

            for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
                this->DisableBlast(i, true);
        }
        else
        {
            vClusterPos += vClusterDir * (30.0f * TIME);

            if(((vClusterPos.x < -FOREGROUND_AREA.x * 1.2f) && (vClusterDir.x < 0.0f)) ||
               ((vClusterPos.x >  FOREGROUND_AREA.x * 1.2f) && (vClusterDir.x > 0.0f)) ||
               ((vClusterPos.y < -FOREGROUND_AREA.y * 1.2f) && (vClusterDir.y < 0.0f)) ||
               ((vClusterPos.y >  FOREGROUND_AREA.y * 1.2f) && (vClusterDir.y > 0.0f)))
            {
                const cPlayer* pPlayer = g_pGame->FindPlayerDual((((iWarpCount++) % 4u) < 2u) ? 0u : 1u);

                coreVector2 vPos, vForce;
                nTargetFunc(IsHorizontal(vClusterDir), pPlayer->GetPosition().xy(), &vPos, &vForce);

                vClusterPos = vPos;
                vClusterDir = vForce.Normalized();
            }

            for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
            {
                cLodObject& oBomb = m_aBombRaw[i];

                if(oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL))
                {
                    const coreFloat fPrevDelay = afDelay[i];
                    afDelay[i] += 1.0f * TIME;

                    if(InBetween(2.0f, fPrevDelay, afDelay[i]))
                        this->EnableBlast(i);

                    if(aiType[i] >= 1u)
                    {
                        oBomb.SetPosition(coreVector3(oBomb.GetPosition().xy() + avMove[i] * TIME, 0.0f));
                        avMove[i] *= FrictionFactor(1.5f);
                    }
                    if(aiType[i] == 2u)
                    {
                        oBomb.SetDirection(coreVector3(coreVector2::Direction(afDelay[i] * ((i % 2u) ? 1.0f : -1.0f)), 0.0f));
                    }
                }

                if(this->GetBombGone(i))
                {
                    const coreVector2 vPos = oBomb.GetPosition ().xy();
                    const coreVector2 vDir = oBomb.GetDirection().xy();

                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = MapToAxisInv(pPlayer->GetPosition().xy() - vPos, vDir);

                        if((ABS(vDiff.x) < 3.0f) || (ABS(vDiff.y) < 3.0f))
                        {
                            if(pPlayer->IsNormal()) pPlayer->TakeDamage(10, ELEMENT_ORANGE, pPlayer->GetPosition().xy());
                        }
                    });

                    afDelay[i] = 0.0f;
                }
            }

            if(STAGE_TICK_FREE(0.75f, 0.0f) && (iRotaCount || ((s_iTick % 2u) == 0u)))
            {
                const cPlayer*    pPlayer = g_pGame->FindPlayerDual(((s_iTick % 8u) < 4u) ? 0u : 1u);
                const coreBool    bType   = (m_iStageSub <= 2u) ? true : ((m_iStageSub <= 4u) ? false : !((iRotaCount++) % 2u));
                const coreUintW   iNum    = (m_iStageSub == 3u || m_iStageSub == 4u) ? 3u : 1u;
                const coreVector2 vOffset = StepRotated45((s_iTick % 8u) / 2u);

                coreVector2 vPos, vForce;
                nTargetFunc(bType, pPlayer->GetPosition().xy(), &vPos, &vForce);

                for(coreUintW i = 0u; i < iNum; ++i)
                {
                    const coreUintW iIndex = (iCurBomb + i) % NEVO_BOMBS;

                    this->EnableBomb(iIndex, false);

                    m_aBombRaw[iIndex].SetPosition (coreVector3(vPos + 10.0f * (vOffset * (I_TO_F(i) - 0.5f * I_TO_F(iNum - 1u)) - vForce.Normalized()), 0.0f));
                    m_aBombRaw[iIndex].SetDirection((m_iStageSub % 2u) ? coreVector3(0.0f,1.0f,0.0f) : coreVector3(1.0f,1.0f,0.0f).Normalized());

                    avMove[iIndex] = 1.5f * vForce;
                    aiType[iIndex] = (m_iStageSub == 5u) ? 2u : 1u;
                }

                iCurBomb += (m_iStageSub == 5u) ? 1u : 3u;
            }

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(iCurEnemy == i)
                {
                    if(pEnemy->ReachedDeath()) iCurEnemy += 1u;

                    coreVector2 vRealDir = vClusterDir;
                    for(coreUintW j = 1u; j < m_iStageSub; ++j) vRealDir = vRealDir.Rotated90();

                    pEnemy->SetPosition (coreVector3(vClusterPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vRealDir,    0.0f));
                }
                else
                {
                    pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                }
            });
        }

        STAGE_WAVE("SECHSZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all bombs
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
            this->DisableBomb(i, false);

        for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
            this->DisableBlast(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // switch everything on
    // 3: in last sub-pattern, either outer or middle tile already marked, as otherwise it would be too easy to activate all
    // 3: for coop, make sure tiles are equally distributed
    // 1,2: blending in tiles needs to be equally delayed, to not favor certain start-positions
    // enemies need to die fast after loosing invincibility
    // TODO: ON tiles have X (eher nicht, schlecht in japan) or other distinct difference (maybe only in lower-right corner, or center)
    // TODO: wenn letztes tile schrumpft kommen funken bei size=0 und grüner helfer erscheint und fliegt weg
    // TODO: badge, helper marks N fields to active in that order
    // TODO: badge, 1-2 der gegner sind schon vorher abschießbar
    // TODO: create a image with the light tiles ?
    // TODO: flipswitch galaxy: blocking tiles moving around, lasers blocking movement between tiles
    // TODO: maybe just kill everything when plates are active -> contrast to pacman stage
    // TODO: extend collision-range of tiles (to occupy 100%) not visuals, except on overlying sub-stage (like for spikes) (check with size first)
    // TODO: in second wave, 2 plate shgould alreay by active (2,2 und 1,3 (start with 0, oben links), oder 2,3 + 3,2)
    // TWIST: plate moves around and has positive effect when activated, negative when deactivated, never locks in
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.2f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->AddNode(coreVector2( 0.8f,-1.2f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 30u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_GET_START(5u + GAME_PLAYERS)
            STAGE_GET_UINT      (iTileState)
            STAGE_GET_UINT      (iTileDone)
            STAGE_GET_UINT      (iTileWait)
            STAGE_GET_FLOAT     (fTileMove)
            STAGE_GET_UINT      (iGauntlet)
            STAGE_GET_UINT_ARRAY(aiRemember, GAME_PLAYERS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  9u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 10u, 19u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 29u)

            for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                this->DisableTile(i, true);

            iTileWait = 0u;
        }

        if(!iTileWait)
        {
            if(!m_aTileRaw[0].IsEnabled(CORE_OBJECT_ENABLE_ALL))
            {
                STATIC_ASSERT(NEVO_TILES == 16u)

                if(m_iStageSub == 1u)
                {
                    for(coreUintW i = 0u; i < 9u; ++i)
                        this->EnableTile(i, 3u);
                }
                else if(m_iStageSub == 2u)
                {
                    for(coreUintW i = 0u; i < 16u; ++i)
                        this->EnableTile(i, 4u);
                }
                else if(m_iStageSub == 3u)
                {
                    if(!iGauntlet) iGauntlet = 1u;
                }

                iTileState = 0u;
                iTileDone  = 0u;
                iTileWait  = 1u;

                std::memset(aiRemember, 0, sizeof(coreUint32) * GAME_PLAYERS);
            }
        }

        if(iGauntlet)
        {
            if(iGauntlet == 1u)
            {
                this->EnableTile(2u, 4u);

                iGauntlet += 1u;
            }
            else if(iGauntlet == 3u)
            {
                this->EnableTile(8u,  4u);
                this->EnableTile(14u, 4u);

                iGauntlet += 1u;
            }
            else if(iGauntlet == 5u)
            {
                this->EnableTile(1u, 4u);
                this->EnableTile(7u, 4u);

                fTileMove  = 0.0f;
                iGauntlet += 1u;
            }
            else if(iGauntlet == 6u)
            {
                fTileMove += 0.6f * TIME;

                m_aTileRaw[1].SetPosition(coreVector3(LERPS(coreVector2(-0.25f,0.75f), coreVector2(-0.25f,-0.75f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
                m_aTileRaw[7].SetPosition(coreVector3(LERPS(coreVector2( 0.75f,0.25f), coreVector2(-0.75f, 0.25f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
            }
            else if(iGauntlet == 7u)
            {
                this->EnableTile(12u, 4u);
                this->EnableTile(13u, 4u);

                m_aTileRaw[12].SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                m_aTileRaw[13].SetPosition(coreVector3(HIDDEN_POS, 0.0f));

                fTileMove  = 0.0f;
                iGauntlet += 1u;
            }
            else if(iGauntlet == 8u)
            {
                fTileMove += 1.0f * TIME;

                const coreVector2 vPos = coreVector2::Direction(fTileMove);
                const coreVector2 vDir = coreVector2::Direction(fTileMove * -2.0f);

                m_aTileRaw[12].SetPosition(coreVector3(vPos * (FOREGROUND_AREA *  0.5f), 0.0f));
                m_aTileRaw[13].SetPosition(coreVector3(vPos * (FOREGROUND_AREA * -0.5f), 0.0f));

                m_aTileRaw[12].SetDirection(coreVector3(vDir, 0.0f));
                m_aTileRaw[13].SetDirection(coreVector3(vDir, 0.0f));
            }
            else if(iGauntlet == 9u)
            {
                this->EnableTile(9u,  4u);
                this->EnableTile(10u, 4u);
                this->EnableTile(11u, 4u);

                m_aTileRaw[9] .SetPosition(coreVector3(0.0f,0.0f,0.0f));
                m_aTileRaw[10].SetPosition(coreVector3(0.0f,0.0f,0.0f));
                m_aTileRaw[11].SetPosition(coreVector3(0.0f,0.0f,0.0f));

                m_aTileRaw[9] .SetSize(m_aTileRaw[9] .GetSize() * 0.7f * 3.0f);
                m_aTileRaw[10].SetSize(m_aTileRaw[10].GetSize() * 0.7f * 2.0f);
                m_aTileRaw[11].SetSize(m_aTileRaw[11].GetSize() * 0.7f);

                iTileState = BIT(10u);
                iGauntlet += 1u;
            }
        }

        coreBool bComplete = !iTileDone;

        if(!iTileDone)
        {
            for(coreUintW i = 0u; i < NEVO_TILES; ++i)
            {
                coreObject3D& oTile = m_aTileRaw[i];
                if(!oTile.IsEnabled(CORE_OBJECT_ENABLE_ALL) || (m_afTileTime[i] < 0.0f)) continue;

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    if(pPlayer->IsRolling()) return;

                    const coreVector2 vDiff = MapToAxisInv(pPlayer->GetPosition().xy() - oTile.GetPosition().xy(), oTile.GetDirection().xy());

                    if((ABS(vDiff.x) < oTile.GetCollisionRange().x) &&
                       (ABS(vDiff.y) < oTile.GetCollisionRange().y))
                    {
                        if(!HAS_BIT(aiRemember[j], i)) TOGGLE_BIT(iTileState, i)
                        ADD_BIT(aiRemember[j], i)
                    }
                    else
                    {
                        REMOVE_BIT(aiRemember[j], i)
                    }
                });

                if(HAS_BIT(iTileState, i))
                {
                    oTile.SetColor3(COLOR_ENERGY_YELLOW);
                }
                else
                {
                    oTile.SetColor3(COLOR_ENERGY_BLUE);
                    bComplete = false;
                }
            }
        }

        if(bComplete)
        {
            if(iGauntlet && (iGauntlet < 10u))
            {
                for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                    this->DisableTile(i, true);

                iGauntlet += 1u;
                bComplete  = false;
            }
            else
            {
                for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                    m_aTileRaw[i].SetColor3(COLOR_ENERGY_GREEN);
            }
        }

        if(bComplete) iTileDone = 1u;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.2f * I_TO_F(i % 5u) + (((i % 10u) < 5u) ? 0.0f : (0.5f * pPath1->GetTotalDistance())))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 10u) < 5u) ? 1.0f : -1.0f, (i < 20u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i >= 10u && i < 15u) pEnemy->Rotate90();
            else if(i >= 15u && i < 20u) pEnemy->Rotate270();

            if(!(i % 5u) && STAGE_TICK_TIME(0.8f, 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                const coreFloat   fBase = pEnemy->GetDirection().xy().Rotated90().Angle();

                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 8.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                }
            }

            if(bComplete)
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_BLUE);
                }
            }
        });

        STAGE_WAVE("SIEBZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all tiles
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        for(coreUintW i = 0u; i < NEVO_TILES; ++i)
            this->DisableTile(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_WAVE("ACHTZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 5u})
    //{
    //    STAGE_BOSS(m_Nautilus, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u, 7u, 8u, 9u, 10u, 11u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // attack triggers on multiple sides
    // multiple enemies protect each others weakspots (like for barrier group)
    // 1 side, then multiple sides
    // (rotating arrow (90 degree))
    // (flipping side with each kill)
    // 4x4 enemy block with pattern (see paper)
    // multiple same near each other (in line or adjacent) <> multiple mixed near each other
    // first: -> with v, then <- with ^, then -> with v+>/^ (seite hätte besseren flow)
    // rotation im uhrzeigersinn, dann entgegen (wegen anordnung der ersten linie 4x4 gruppe)
    // maximal 2 spieler-rotation pro angreifbarer ausrichtung
    // erste 3 gegner sind tutorial
    // TODO: 2 gegner pro seite, rotating arrow, start mit oben, gegen uhrzeigersinn
    // TODO: vertical enemies offset so players have to work together
    // TODO: use object_arrow.md3
    STAGE_MAIN({TAKE_ALWAYS, 6u})
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
            pPath2->AddNode(coreVector2(1.3f,0.0f), coreVector2(-1.0f,0.0f));
            pPath2->AddStop(coreVector2(0.0f,0.0f), coreVector2(-1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 43u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_GREEN);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        constexpr coreUintW iMapSize = 16u;

        STAGE_GET_START(iMapSize)
            STAGE_GET_UINT_ARRAY(aiArrowMapRaw, iMapSize)
        STAGE_GET_END

        coreUint8* aiArrowMap = r_cast<coreUint8*>(aiArrowMapRaw);

        const auto nEnableArrowFunc = [&](const coreUintW iArrowIndex, const coreUintW iEnemyIndex, const coreVector2& vDirection)
        {
            const cEnemy*   pEnemy = pSquad1->GetEnemy(iEnemyIndex);
            const coreUint8 iPack  = PackDirection(vDirection) / 2u;
            ASSERT(iPack < 4u)

            this->EnableArrow(iArrowIndex, pEnemy, vDirection);
            aiArrowMap[(iEnemyIndex * 4u + iPack) % (iMapSize * 4u)] = iArrowIndex + 1u;
        };

        for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
        {
            if(m_apArrowOwner[i] && m_apArrowOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableArrow(i, true);
        }

        if(STAGE_CLEARED)
        {
            std::memset(aiArrowMap, 0, sizeof(coreUint32) * iMapSize);

            if(STAGE_SUB(1u))
            {
                STAGE_RESURRECT(pSquad1, 0u, 0u)
                nEnableArrowFunc( 0u,  0u, coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 1u, 1u)
                nEnableArrowFunc( 1u,  1u, coreVector2( 0.0f, 1.0f));
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 2u, 2u)
                nEnableArrowFunc( 2u,  2u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 3u,  2u, coreVector2( 1.0f, 0.0f));
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 3u, 6u)
                nEnableArrowFunc( 4u,  3u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc( 5u,  4u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc( 6u,  5u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc( 7u,  6u, coreVector2( 0.0f, 1.0f));
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESURRECT(pSquad1, 7u, 10u)
                nEnableArrowFunc( 0u,  7u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 1u,  8u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 2u,  9u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 3u, 10u, coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(6u))
            {
                STAGE_RESURRECT(pSquad1, 11u, 14u)
                nEnableArrowFunc( 4u, 11u, coreVector2(-1.0f, 0.0f));
                nEnableArrowFunc( 5u, 12u, coreVector2(-1.0f, 0.0f));
                nEnableArrowFunc( 6u, 13u, coreVector2(-1.0f, 0.0f));
                nEnableArrowFunc( 7u, 14u, coreVector2(-1.0f, 0.0f));
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 15u, 18u)
                nEnableArrowFunc( 8u, 15u, coreVector2( 1.0f, 0.0f));
                nEnableArrowFunc( 9u, 16u, coreVector2( 1.0f, 0.0f));
                nEnableArrowFunc(10u, 17u, coreVector2( 1.0f, 0.0f));
                nEnableArrowFunc(11u, 18u, coreVector2( 1.0f, 0.0f));
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 19u, 22u)
                nEnableArrowFunc(12u, 19u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc(13u, 20u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc(14u, 21u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc(15u, 22u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc(16u, 22u, coreVector2(-1.0f, 0.0f));
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 23u, 26u)
                nEnableArrowFunc( 0u, 23u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 1u, 24u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 2u, 25u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 3u, 26u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 4u, 26u, coreVector2( 1.0f, 0.0f));
            }
            else if(STAGE_SUB(10u))
            {
                STAGE_RESURRECT(pSquad1, 27u, 42u)
                nEnableArrowFunc( 5u, 27u, coreVector2(-1.0f, 0.0f));
                nEnableArrowFunc( 6u, 28u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 7u, 29u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc( 8u, 30u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc( 9u, 31u, coreVector2( 1.0f, 0.0f));
                nEnableArrowFunc(10u, 32u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc(11u, 33u, coreVector2( 1.0f, 0.0f));
                nEnableArrowFunc(12u, 34u, coreVector2( 1.0f, 0.0f));
                nEnableArrowFunc(13u, 35u, coreVector2(-1.0f, 0.0f));
                nEnableArrowFunc(14u, 36u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc(15u, 37u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc(16u, 38u, coreVector2(-1.0f, 0.0f));
                nEnableArrowFunc(17u, 39u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc(18u, 40u, coreVector2( 0.0f,-1.0f));
                nEnableArrowFunc(19u, 41u, coreVector2( 0.0f, 1.0f));
                nEnableArrowFunc(20u, 42u, coreVector2( 1.0f, 0.0f));
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 27u)
            {
                STAGE_LIFETIME(pEnemy, 0.7f, (i < 3u) ? 0.0f : (0.2f * I_TO_F((i - 3u) % 4u)))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 0u) pEnemy->Rotate270();
                else if(i == 1u) pEnemy->Rotate90 ();
                else if(i == 2u) pEnemy->Rotate270();
                else if(i <  7u) pEnemy->Rotate90 ();
                else if(i < 11u) pEnemy->Rotate270();
                else if(i < 15u) pEnemy->Rotate90 ();
                else if(i < 19u) pEnemy->Rotate270();
                else if(i < 23u) {}
                else if(i < 27u) pEnemy->Rotate180();
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 2.1f, 0.0f)

                const coreVector2 vFactor = coreVector2((((i - 27u) % 4) < 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F((i - 27u) % 4u) * 0.2f - 0.3f, I_TO_F((i - 27u) / 4u) * 0.2f - 0.3f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset, fLifeTime);   // # no factor multiplication
            }
        });

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS, COLL_VAL(pSquad1), COLL_VAL(aiArrowMap))
        {
            if(!bFirstHit) return;

            const coreUintW   i     = pSquad1->GetIndex(pEnemy);
            const coreVector2 vSide = AlongCrossNormal(-pBullet->GetFlyDir());
            const coreUint8   iPack = PackDirection(vSide) / 2u;
            ASSERT(iPack < 4u)

            coreUint8& iEntry = aiArrowMap[(i * 4u + iPack) % (iMapSize * 4u)];
            if(iEntry)
            {
                this->DisableArrow(iEntry - 1u, true);

                iEntry = 0u;

                coreUint32& iEntryAll = r_cast<coreUint32&>(aiArrowMap[(i * 4) % (iMapSize * 4u)]);
                if(!iEntryAll)
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                }

                const coreVector2 vPos  = pEnemy->GetPosition().xy();
                const coreFloat   fBase = vSide.Angle();

                for(coreUintW j = 3u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 1.0f) * 36.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        });

        STAGE_WAVE("EINUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all arrows
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
            this->DisableArrow(i, false);

        STAGE_FINISH_NOW
    });

STAGE_START_HERE
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
    
    // partikel gezogen ()  werden gegen schussrichtung eingesaugt, bei schuss muss er linear nach in schussrichtung gehn und sich ausbreiten in weite
    // unter gegen, geschosse, container
    //
    // container in mitte
    // laser von oben und folgt mustern und verfolgt spieler
    // einzelne schüsse (mit druckwelle)
    // laser ändert winkel im kreis herum und schwingt hin und her an container vorbei
    // laser wird super-dick
    // fat laser with warning: from top, left, right, bottom, top -> and around 90 degree -> ball moves down enemies above -> ball moves up enemies below
    // strahl wird immer kleiner, dann kommt ein kleiner gegner mit dem strahl den man noch töten muss
    // 
    // shelter werden durch erste schüsse erzeugt, die einzeln sind, danach kontinuierlich
    // super-laser wird durch gegen-richtungs ansaugen angezeigt
    // gegner sollen sich bewegen und kurven fahren
    // it should not be possible to stand in the same shadow for two consecutive big shots
    // TODO: add border objects below each border (2x per shelter) instead of the wobbling
    STAGE_MAIN({TAKE_ALWAYS, 7u})
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
            pPath2->AddNode(coreVector2(-0.5f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 0.5f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(3u);
            pPath3->AddNode(coreVector2(-0.5f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2( 0.5f, 0.0f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2( 0.5f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath4->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 100u)                            
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(6u)
            STAGE_GET_FLOAT(fShelterTime)
            STAGE_GET_FLOAT(fBeamDuration)
            STAGE_GET_FLOAT(fBeamFade, fBeamFade = 1.0f)
            STAGE_GET_UINT (iChanged)
            STAGE_GET_FLOAT(fDelay)
            STAGE_GET_FLOAT(fRotation)
        STAGE_GET_END

        const auto nShootBeamFunc = [&](const coreFloat fDuration, const coreFloat fFadeIn, const coreFloat fFadeOut)
        {
            fBeamDuration = fDuration;
            fBeamFade     = fFadeOut;

            this->FadeBeam(true, fFadeIn);
        };

        if(fBeamDuration)
        {
            fBeamDuration = MAX(fBeamDuration - 1.0f * TIME, 0.0f);
            if(!fBeamDuration)
            {
                this->FadeBeam(false, fBeamFade);
            }
        }

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  9u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 10u, 19u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 29u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 30u, 39u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 50u, 65u)

            if(m_iStageSub > 2u)
            {
                fBeamDuration = 0.0f;
                if(m_fBeamTime >= 1.0f) this->FadeBeam(false, fBeamFade);
            }
            iChanged = 0u;
            fRotation = 0.0f;
        }

        cHelper* pHelperBlue  = g_pGame->GetHelper(ELEMENT_BLUE);
        cHelper* pHelperGreen = g_pGame->GetHelper(ELEMENT_GREEN);

        if(STAGE_BEGINNING)
        {
            this->EnableBeam();
            this->EnableShelter(0u);
            this->EnableShelter(1u);

            pHelperBlue ->Resurrect();
            pHelperGreen->Resurrect();
        }

        if(m_iStageSub >= 2u)
        {
            fShelterTime = MIN(fShelterTime + 0.3f * TIME, 1.0f);
        }
        
        const coreFloat fOffset = 1.0f * SIN(m_fStageTime * (0.2f*PI));
        const coreVector2 vShelterDir = coreVector2::Direction(m_fStageTime * (0.2f*PI));
        const coreVector2 vShelterDir2 = coreVector2::Direction(m_fStageTime * -(0.2f*PI));

        for(coreUintW i = 0u; i < NEVO_SHELTERS; ++i)
        {
            cLodObject& oShelter = m_aShelter[i];

            const coreVector2 vFrom = i ? coreVector2(-0.4f,-1.5f) : coreVector2(1.5f,0.4f);
            const coreVector2 vTo   = i ? coreVector2(-0.4f,-0.4f * fOffset) : coreVector2(0.4f,0.4f * fOffset);//(i ? -vShelterDir2 : vShelterDir2) * 0.45f;//
            const coreVector2 vPos  = LERPB(vFrom, vTo * 1.0f, fShelterTime) * FOREGROUND_AREA;

            const coreVector2 vDir = i ? vShelterDir.InvertedX() : vShelterDir;

            oShelter.SetPosition (coreVector3(vPos, -2.0f));   // make sure bullets always hit, models are shifted
            oShelter.SetDirection(coreVector3(vDir,  0.0f));

            (i ? pHelperBlue : pHelperGreen)->SetPosition(coreVector3(oShelter.GetPosition().xy() - vDir * 13.0f, 0.0f));

            oShelter.ActivateModelLowOnly();
            {
                cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, &oShelter, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pShelter, const coreVector3& vIntersection, const coreBool bFirstHit)
                {
                    if(!bFirstHit) return;

                    // 
                    const coreVector2 vDiff = pPlayer->GetOldPos() - pShelter->GetPosition().xy();
                    pPlayer->ApplyForce(vDiff.Normalized() * 100.0f);

                    // 
                    g_pSpecialEffects->CreateSplashColor(vIntersection, 5.0f, 3u, COLOR_ENERGY_WHITE);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                });

                Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &oShelter, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pShelter, const coreVector3& vIntersection, const coreBool bFirstHit)
                {
                    if(!bFirstHit) return;

                    // 
                    //pBullet->Reflect(pShelter, vIntersection.xy(), 3.0f);
                    pBullet->Deactivate(true);
                });
            }
            oShelter.ActivateModelDefault();
        }

        if(!InBetween(m_fBeamTime, 1.0f, 2.0f))
        {
            cPlayer::TestCollision(PLAYER_TEST_NORMAL, &m_Beam, [this](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBeam, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                for(coreUintW i = 0u; i < NEVO_SHELTERS; ++i)
                {
                    coreObject3D& oBack = m_aShelterBack[i];

                    coreVector3 vNewIntersection;
                    coreBool    bNewFirstHit;
                    if(pPlayer->TestCollisionPrecise(&oBack, &vNewIntersection, &bNewFirstHit))
                    {
                        return;
                    }
                }

                pPlayer->TakeDamage(10, ELEMENT_MAGENTA, vIntersection.xy());

                g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_MAGENTA);
            });
        }

        if(m_iStageSub <= 2u)
        {
            if(STAGE_BEGINNING || STAGE_TICK_FREE(1.0f/3.0f, 0.0f))
            {
                m_vBeamPos = coreVector2(g_pGame->GetPlayer(0u)->GetPosition().x, FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = coreVector2(0.0f,-1.0f);
                m_fBeamWidth = 10.0f;

                nShootBeamFunc(2.0f, 1.0f, 1.0f);
            }
        }
        else if(m_iStageSub == 3u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                nShootBeamFunc(0.0f, 1.0f, 1.0f);
                iChanged = 1u;
            }

            if(iChanged)
            {
                fRotation += 0.2f * TIME;
                const coreFloat fTime = FRACT(fRotation + 0.25f);
                const coreFloat fSide = LERP(-1.0f, 1.0f, MIN(fTime, 1.0f - fTime) * 2.0f);

                m_vBeamPos = coreVector2(FOREGROUND_AREA.x * 1.7f, fSide * FOREGROUND_AREA.y);
                m_vBeamDir = coreVector2(-1.0f,0.0f);
                m_fBeamWidth = 10.0f;
            }
        }
        else if(m_iStageSub == 4u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                nShootBeamFunc(0.0f, 1.0f, 1.0f);
                iChanged = 1u;
            }

            if(iChanged)
            {
                fRotation += 0.5f * TIME;
                const coreFloat fTime = (fRotation + 0.5f*PI);
                const coreVector2 vDir  = coreVector2::Direction(fTime);
                const coreVector2 vDir2 = coreVector2::Direction(fTime + 0.0f*0.2f*PI * SIN(fTime));

                m_vBeamPos = vDir * (FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = -vDir2;
                m_fBeamWidth = 10.0f;
            }
        }
        else if(m_iStageSub == 5u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                fDelay = m_fStageTime * 1.0f/4.0f - 1.0f;
                iChanged = 1u;
            }

            if(iChanged && STAGE_TICK_FREE(1.0f/4.0f, fDelay))
            {
                coreVector2 vDir;
                switch(s_iTick % 4u)
                {
                default: ASSERT(false)
                case 0u: vDir = coreVector2( 0.0f, 1.0f); break;
                case 1u: vDir = coreVector2( 1.0f, 0.0f); break;
                case 2u: vDir = coreVector2(-1.0f, 0.0f); break;
                case 3u: vDir = coreVector2( 0.0f,-1.0f); break;
                }

                m_vBeamPos = vDir * (FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = -vDir;
                m_fBeamWidth = 60.0f;

                nShootBeamFunc(3.0f, 2.0f, 1.0f);
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                nShootBeamFunc(0.0f, 2.0f, 1.0f);
                iChanged = 1u;
            }

            if(iChanged)
            {
                fRotation += 0.5f * TIME;
                const coreVector2 vDir = coreVector2::Direction(fRotation);

                m_vBeamPos = vDir * (FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = -vDir;
                m_fBeamWidth = 60.0f;
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 1.0f + 0.2f * I_TO_F(i % 5u))
            
            //const coreVector2 vRawBeamPos = m_vBeamPos / FOREGROUND_AREA;
            //const coreFloat fLen = 1.7f;
            //STAGE_REPEAT(fLen)
            //
            //pEnemy->DefaultMoveLerp(vRawBeamPos, vRawBeamPos + m_vBeamDir * fLen, fLifeTime * RCP(fLen));
            //pEnemy->SetPosition(coreVector3((vRawBeamPos + m_vBeamDir * 0.8f) * FOREGROUND_AREA, 0.0f));
            
            if(i < 20u)
            {
                //const coreVector2 vRawBeamPos = m_vBeamPos / FOREGROUND_AREA;
                //const coreVector2 vPos = coreVector2(vRawBeamPos.x, (I_TO_F(i % 5u) - 2.0f) * 0.4f);
                
                //pEnemy->SetPosition(coreVector3(InBetween(m_fBeamTime, 1.0f, 2.0f) ? HIDDEN_POS : (vPos * FOREGROUND_AREA), 0.0f));
                STAGE_REPEAT(pPath2->GetTotalDistance())
                
                const coreVector2 vFactor = coreVector2(-1.0f,1.0f);
                const coreVector2 vOffset = coreVector2( 0.0f,0.0f);
                
                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                
                if((i % 10u) < 5u) pEnemy->InvertX();//pEnemy->Rotate270();
                else  pEnemy->Rotate90();
            }
            else if(i < 30u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 20u) % 10u) - 4.5f) * 0.2f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }
            else if(i < 50u)
            {
                const coreVector2 vPos = coreVector2::Direction(I_TO_F(i % 10u) * (0.2f*PI));
                
                pEnemy->SetPosition(coreVector3(InBetween(m_fBeamTime, 1.0f, 2.0f) ? HIDDEN_POS : (vPos * FOREGROUND_AREA), 0.0f));
            }
            else
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 5u) - 2.0f) * 0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 55u) {}
                else if(i < 60u) pEnemy->Rotate90 ();
                else if(i < 65u) pEnemy->Rotate180();
                else if(i < 70u) pEnemy->Rotate270();
                
                const cLodObject& oShelter = m_aShelter[(i < 58u) ? 1u : 0u];
                
                const coreVector2 vDir = coreVector2::Direction(2.0f*PI * (I_TO_F(((i - 50u) % 8u) + (((i - 50u) % 8u) / 2u)) - 0.5f) / 12.0f);
                const coreVector2 vPos = oShelter.GetPosition().xy() + vDir * 13.0f;
                
                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
            
            
            
            //const coreVector2 vFactor = coreVector2(0.5f,1.0f);
            //const coreVector2 vOffset = coreVector2(1.5f,0.0f);
//
            //pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            //
            //pEnemy->Rotate270();
            
            
            //const coreVector2 vFactor = coreVector2(0.5f,1.0f);
            //const coreVector2 vOffset = coreVector2(-1.5f,0.0f);
//
            //pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
//
            //if((i % 10u) < 5u) pEnemy->Rotate270();
            //else  pEnemy->Rotate90();


            //const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            //const coreVector2 vOffset = coreVector2((I_TO_F(i % 5u) - 2.0f) * 0.3f, 0.0f);
//
            //pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);
//
            //if((i % 10u) < 5u) pEnemy->Rotate180();
            
        });

        STAGE_WAVE("ZWEIUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all shelters and beam
    STAGE_MAIN({TAKE_ALWAYS, 7u})
    {
        for(coreUintW i = 0u; i < NEVO_SHELTERS; ++i)
            this->DisableShelter(i, false);

        this->DisableBeam(false);

        g_pGame->GetHelper(ELEMENT_BLUE) ->Kill(false);
        g_pGame->GetHelper(ELEMENT_GREEN)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // occupy areas with energy
    // rotation der großen dreiecke entgegen der flug-drehrichtung (auch gruppe 1)
    // flug-rotation um bewegung gefährlicher zu machen und gegen safe-spots
    // erste welle so aufbauen um dreiecke möglichst gleichmäßig zu verteilen, spieler kann trotzdem überlappen, könnte aber zeit kosten
    // hohe geschwindigkeit erhöht intensity und aufmerksamkeit
    // wenn flug-drehrichtung anfängt, zweite welle von der seite kommen, von wo die großen dreiecke kommen werden
    // TODO: helfer spawnt und fliegt in Ntem teleportiertem dreieck, verschwindet nach nächster teleportation, kleiner funken-effekt bei kill (falls doch noch sichtbar)
    // TODO: something after the last wave (changing the triangles (size, movement), or the number, or ...), or start ? hmmm, no I want to start up quick
    // TODO: also focus on difference from d/generate wave, make are strongly occupied ?
    // TODO: create an absurd big object
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 24u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_FLOAT(fMoveSpeed)
            STAGE_GET_FLOAT(fMoveAngle)
        STAGE_GET_END

        constexpr coreUintW iNumBig = 4u;

        for(coreUintW i = iNumBig; i < NEVO_BLOCKS; ++i)
        {
            if(m_apBlockOwner[i] && m_apBlockOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableBlock(i, true);
        }

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 16u, 19u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 20u, 23u)

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(!pEnemy->ReachedDeath()) this->EnableBlock((i < iNumBig) ? i : ((i % (NEVO_BLOCKS - iNumBig)) + iNumBig), pEnemy, (i < iNumBig) ? 10.0f : -5.0f);
            });

            if(STAGE_CLEARED)
            {
                for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
                    this->DisableBlock(i, true);

                g_pGame->GetHelper(ELEMENT_ORANGE)->Kill(true);
            }
        }

        if(m_iStageSub >= 5u)
        {
            fMoveSpeed = MIN(fMoveSpeed + 0.5f * TIME, 1.0f);
            fMoveAngle = fMoveAngle - 0.2f * TIME;

            const coreVector2 vMove = coreVector2::Direction(fMoveAngle) * (55.0f * TIME * fMoveSpeed);

            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                if(i >= iNumBig) return;

                coreVector2 vNewPos = pEnemy->GetPosition().xy() + vMove;

                     if(vNewPos.x < -FOREGROUND_AREA.x * 1.45f) vNewPos.x =  ABS(vNewPos.x);
                else if(vNewPos.x >  FOREGROUND_AREA.x * 1.45f) vNewPos.x = -ABS(vNewPos.x);
                     if(vNewPos.y < -FOREGROUND_AREA.y * 1.45f) vNewPos.y =  ABS(vNewPos.y);
                else if(vNewPos.y >  FOREGROUND_AREA.y * 1.45f) vNewPos.y = -ABS(vNewPos.y);

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 4u || i >= 12u)
            {
                STAGE_LIFETIME(pEnemy, 0.8f, 0.0f)

                const coreFloat   fSide = ((i / 4u) % 2u) ? 1.0f : -1.0f;
                const coreFloat   fVal  = fLifeTime * 1.8f;
                const coreVector2 vDir  = coreVector2::Direction((fVal - I_TO_F(i % 4u) * (0.5f*PI)) * fSide);
                const coreFloat   fLen  = LERPB(FOREGROUND_AREA.x * 1.2f, 0.0f, MIN(fVal * 0.1f, 1.0f));

                pEnemy->SetPosition (coreVector3(vDir * fLen,               0.0f));
                pEnemy->SetDirection(coreVector3(vDir.Rotated90() * -fSide, 0.0f));
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 0.8f, 0.5f * I_TO_F(i % 4u) + ((i < 8u) ? 3.0f : 0.0f))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f, (i < 8u) ? 1.0f : -1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f) * -0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->Rotate270();
            }
        });

        STAGE_WAVE("DREIUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all blocks
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
            this->DisableBlock(i, false);

        g_pGame->GetHelper(ELEMENT_ORANGE)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 11u})
    //{
    //    STAGE_BOSS(m_Amemasu, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 12u, 13u, 14u, 15u, 16u, 17u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // everything moves against current direction
    // kein infinity für gegner, schwer nachvollziehbar, könnte sich einer verstecken
    // gegner bis zum finalen raum im dungeon dürfen nicht optional sein
    // in dungeon tunnel, gegner dürfen sich nicht hinter einer geraden befinden, die werden sonst einfach mit-erschossen
    // gegner-anordnung sollen den spieler dazu bringen sich zu drehen, unterschied 90 (leicht) und 180 (schwer)
    // erste gegner gruppe muss den spieler zum drehen bringen, während geschosse ihm noch nicht schaden zufügen können, aber schon gut sichtbar sind
    // TODO: could be during a sandstorm, rainstorm or other visual distortion to remove the background movement
    // TODO: ((bullets im letzten phasenwechsel fliegen nach außen (ghost) statt zerstört zu werden)) -> vielleicht immer
    // TODO: badge: enemy at the back of initial room
    // TODO: maybe not all bits in first phase are activatable form the start (fade in)
    // TODO: dungeon phases need to be created across N (60) frames
    // TODO: harte kollision im dungeon (direct -> 1 sec stop, otherwise just bump)
    // TODO: coop: average, only move if both look at same direction, both control both ships
    // TODO: star field ? lines from the side |> -> ?
    // TODO: infinity bullets für nicht-star patterns sollten früher sein
    STAGE_MAIN({TAKE_ALWAYS, 12u})
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
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 44u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(3u)
            STAGE_GET_VEC2(vGlobalOffset)
            STAGE_GET_UINT(iBulletField)
        STAGE_GET_END

        cEnemy* pDummy = pSquad1->GetEnemy(0u);

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  2u,  6u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  7u, 11u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 12u, 16u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 17u, 24u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 25u, 32u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 33u, 43u)

#if 0
            for(coreUintW i = 0u; i < 30u; ++i)
            {
                const coreVector2 vPos = coreVector2((I_TO_F(i) - 14.5f) * 0.11f, 1.1f * 1.5f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos,             vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos,             vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos.Rotated90(), vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos.Rotated90(), vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }

            for(coreUintW i = 0u; i < 5u; ++i)
            {
                const coreVector2 vPos = coreVector2((I_TO_F(i) - 2.0f) * 0.44f, (i % 2u) ? 1.35f : 1.2f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos,             vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos,             vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos.Rotated90(), vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos.Rotated90(), vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
#endif

            if(m_iStageSub == 5u)
            {
                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
            }
            else if(m_iStageSub == 7u)
            {
                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                constexpr coreFloat fScale = 0.1f;

                coreUintW iEnemyIndex = 33u;

                const auto nCreateBlockFunc = [&](const coreInt32 x, const coreInt32 y, const coreUint8 iShape = 0u)
                {
                    const coreVector2 vBase = coreVector2(I_TO_F(x), I_TO_F(y) + 2.5f) * (FOREGROUND_AREA * fScale * 6.0f);

                    if(iShape)
                    {
                        //   0
                        // 3   1
                        //   2
                        for(coreUintW i = 0u; i < 7u; ++i)
                        {
                            const coreVector2 vPos = coreVector2(I_TO_F(i) - 3.0f, 3.0f) * (FOREGROUND_AREA * fScale);
                            const coreVector2 vDir = coreVector2(0.0f,1.0f);

                            if(HAS_BIT(iShape, 0u)) g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos             + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                            if(HAS_BIT(iShape, 2u)) g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos             + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                            if(HAS_BIT(iShape, 1u)) g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos.Rotated90() + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                            if(HAS_BIT(iShape, 3u)) g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos.Rotated90() + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        }
                    }
                    else
                    {
                        const coreVector2 vPos = coreVector2(0.5f,0.5f) * 0.11f * FOREGROUND_AREA;
                        const coreVector2 vDir = coreVector2(0.0f,1.0f);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos             + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos             + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos.Rotated90() + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos.Rotated90() + vBase, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                };

                const auto nCreateEnemyFunc = [&](const coreInt32 x, const coreInt32 y)
                {
                    const coreVector2 vBase = coreVector2(I_TO_F(x), I_TO_F(y) + 2.5f) * (FOREGROUND_AREA * fScale * 6.0f);

                    pSquad1->GetEnemy(iEnemyIndex++)->SetPosition(coreVector3(vBase, 0.0f));
                };

                nCreateBlockFunc( 1, -1, BIT(0u));
                nCreateBlockFunc( 2, -1, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 2, -2, BIT(1u));
                nCreateBlockFunc( 2, -3, BIT(1u));
                nCreateBlockFunc( 2, -4, BIT(2u) | BIT(1u));
                nCreateBlockFunc( 1, -4, BIT(2u));
                nCreateBlockFunc( 0, -4, BIT(2u));
                nCreateBlockFunc(-1, -4, BIT(2u));
                nCreateBlockFunc(-2, -4, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-2, -3, BIT(3u));
                nCreateBlockFunc(-2, -2, BIT(3u));
                nCreateBlockFunc(-2, -1, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-1, -1, BIT(0u));

                nCreateBlockFunc( 0,  0, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0,  1, BIT(3u) | BIT(0u)); nCreateEnemyFunc( 0,  1);
                nCreateBlockFunc( 1,  1, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 2,  1, BIT(2u) | BIT(1u)); nCreateEnemyFunc( 2,  1);
                nCreateBlockFunc( 2,  2, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 2,  3, BIT(1u) | BIT(0u)); nCreateEnemyFunc( 2,  3);
                nCreateBlockFunc( 1,  3, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 0,  3, BIT(2u) | BIT(0u));
                nCreateBlockFunc(-1,  3, BIT(3u) | BIT(2u)); nCreateEnemyFunc(-1,  3);
                nCreateBlockFunc(-1,  4, BIT(3u) | BIT(1u)); nCreateEnemyFunc(-1,  4);

                nCreateBlockFunc( 0,  5, BIT(2u));
                nCreateBlockFunc( 1,  5, BIT(2u) | BIT(1u));
                nCreateBlockFunc( 1,  6, BIT(1u));
                nCreateBlockFunc( 1,  7, BIT(1u));
                nCreateBlockFunc( 1,  8, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 0,  8, BIT(0u));
                nCreateBlockFunc(-1,  8, BIT(0u));
                nCreateBlockFunc(-2,  8, BIT(0u));
                nCreateBlockFunc(-3,  8, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-3,  7, BIT(3u));
                nCreateBlockFunc(-3,  6, BIT(3u));
                nCreateBlockFunc(-3,  5, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-2,  5, BIT(2u));

                nCreateBlockFunc(-3,  8);
                nCreateBlockFunc(-3,  6);
                nCreateBlockFunc(-2,  7);
                nCreateBlockFunc(-2,  5);
                nCreateBlockFunc(-1,  8);
                nCreateBlockFunc(-1,  6);
                nCreateBlockFunc( 0,  7);
                nCreateBlockFunc( 0,  5);
                nCreateBlockFunc( 1,  8);
                nCreateBlockFunc( 1,  6);

                nCreateEnemyFunc(-2,  6);
                nCreateEnemyFunc(-3,  7);
                nCreateEnemyFunc(-2,  8);
                nCreateEnemyFunc( 0,  6);
                nCreateEnemyFunc( 1,  7);
                nCreateEnemyFunc( 0,  8);
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_BLUE);

        if(m_iStageSub == 5u)
        {
            if(STAGE_BEGINNING2)
            {
                pHelper->Resurrect();
            }

            pHelper->SetPosition(coreVector3(0.0f, LERPB(1.2f, 0.0f, MIN(m_fStageSubTime, 1.0f)) * FOREGROUND_AREA.y, 0.0f));

            if(STAGE_SUBTIME_POINT(1.0f))
            {
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_BLUE);
            }
        }

        const coreFloat   fGlobalSpeed = 30.0f;
        const coreVector2 vGlobalDir   = g_pGame->GetPlayer(0u)->GetDirection().xy();
        const coreVector2 vGlobalMove  = vGlobalDir * (-fGlobalSpeed * TIME);

        vGlobalOffset += vGlobalMove;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, ((i < 2u) ? (0.5f + 0.2f * I_TO_F(i % 2u)) : ((i < 17u) ? (0.2f * I_TO_F((i - 2u) % 5u)) : 0.0f)))

            const coreSpline2* pPath = (i < 2u || i >= 17u) ? pPath1 : pPath2;

            if(pPath == pPath2) STAGE_REPEAT(pPath->GetTotalDistance())

            if(i < 33u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 2u) ? ((i % 2u) ? -0.9f : -0.7f) : ((i < 17u) ? -0.8f : ((I_TO_F((i - 17u) % 4u) - 1.5f) * 0.3f)), 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  2u) pEnemy->Rotate270();
                else if(i <  7u) pEnemy->Rotate90 ();
                else if(i < 12u) pEnemy->Rotate180();
                else if(i < 17u) {}
                else if(i < 21u) pEnemy->Rotate180();
                else if(i < 25u) {}
                else if(i < 29u) pEnemy->Rotate270();
                else if(i < 33u) pEnemy->Rotate90 ();
            }
            else
            {
                const coreVector2 vNewPos = pEnemy->GetPosition().xy() + vGlobalMove;

                pEnemy->SetPosition  (coreVector3(vNewPos, 0.0f));
                pEnemy->DefaultRotate(fLifeTime * (2.0f*PI));
            }
        });

        if(m_iStageSub < 5u)
        {
            constexpr coreUintW iBullets = 18u;
            STATIC_ASSERT(iBullets <= sizeof(iBulletField)*8u)

            for(coreUintW i = 0u; i < iBullets; ++i)
            {
                if(HAS_BIT(iBulletField, i)) continue;

                const coreVector2 vBulletPos = vGlobalOffset + (coreVector2(I_TO_F(i % 6u), I_TO_F(i / 6u) * 2.0f + I_TO_F(i % 2u)) / 6.0f - 0.5f) * (FOREGROUND_AREA * 2.8f);

                if((ABS(vBulletPos.x) > FOREGROUND_AREA.x * 1.4f) || (ABS(vBulletPos.y) > FOREGROUND_AREA.y * 1.4f))
                {
                    ADD_BIT(iBulletField, i)

                    const coreVector2 vPos = coreVector2(0.5f,0.5f) * 0.11f * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos             + vBulletPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos             + vBulletPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy,  vPos.Rotated90() + vBulletPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, -vPos.Rotated90() + vBulletPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            }
        }
        else if(m_iStageSub < 7u)
        {
            if(((m_fStageSubTime >= 1.0f) || (m_iStageSub > 5u)) && STAGE_TICK_FREE(fGlobalSpeed / 5.0f, 0.0f))
            {
                const coreVector2 vPos = pHelper->GetPosition().xy();
                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, vPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
        }

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
        {
            coreVector2 vNewPos = pBullet->GetPosition().xy() + vGlobalMove;

            if(m_iStageSub < 7u)
            {
                      if((vNewPos.x < -FOREGROUND_AREA.x * 1.4f) && (vGlobalMove.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.8f;
                 else if((vNewPos.x >  FOREGROUND_AREA.x * 1.4f) && (vGlobalMove.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.8f;
                      if((vNewPos.y < -FOREGROUND_AREA.y * 1.4f) && (vGlobalMove.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.x * 2.8f;
                 else if((vNewPos.y >  FOREGROUND_AREA.y * 1.4f) && (vGlobalMove.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.x * 2.8f;
            }

            pBullet->SetPosition(coreVector3(vNewPos, 0.0f));
        });

        STAGE_WAVE("SECHSUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 12u})
    {
        g_pGame->GetHelper(ELEMENT_BLUE)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 13u})
    {
        STAGE_WAVE("SIEBENUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        STAGE_WAVE("ACHTUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 17u})
    //{
    //    STAGE_BOSS(m_Leviathan, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}