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
        STAGE_FINISH_AFTER(MISSION_WAIT_INTRO)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
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
    // leave destroyable residue on its way (r-type leaver)
    // enemies cannot move on the same path in infinity as the bullets would just overlay or need some handling  
    // multiple following enemies with the same move pattern will overlay after some time  
    // enemy direction creates a whirl 
    // destroying player bullets on impact is bad 
    // changing speed breaks bullet pattern 
    // snake is also bad 
    // bit-matrix or clearing on contact is bad 
    // start direction against whirl direction to not clutter the bullets and show the whirl effect   
    // TODO 1: handle dodge and enemy inside   
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });

            pSquad1->GetEnemy( 0u)->SetPosition(coreVector3(-0.8f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 0u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy( 1u)->SetPosition(coreVector3( 0.8f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 1u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy( 2u)->SetPosition(coreVector3(-1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 2u)->SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 3u)->SetPosition(coreVector3( 1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 3u)->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 4u)->SetPosition(coreVector3(-1.2f,-0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 4u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy( 5u)->SetPosition(coreVector3( 0.0f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 5u)->SetDirection(coreVector3( 1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 6u)->SetPosition(coreVector3( 1.2f, 0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 6u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy( 7u)->SetPosition(coreVector3( 0.0f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 7u)->SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());

            pSquad1->GetEnemy( 8u)->SetPosition(coreVector3( 0.8f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 8u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy( 9u)->SetPosition(coreVector3(-0.8f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 9u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy(10u)->SetPosition(coreVector3( 1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(10u)->SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(11u)->SetPosition(coreVector3(-1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(11u)->SetDirection(coreVector3( 1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(12u)->SetPosition(coreVector3(-1.2f, 0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(12u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(13u)->SetPosition(coreVector3( 0.0f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(13u)->SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(14u)->SetPosition(coreVector3( 1.2f,-0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(14u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(15u)->SetPosition(coreVector3( 0.0f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(15u)->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalized());
        });

        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fWhirlSpeed)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 10u, 10u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 11u, 11u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 12u, 13u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 14u, 15u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((m_iStageSub == 7u) && (fWhirlSpeed < 1.0f)) return;

            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vDiff   = pEnemy->AimAtPlayerDual((((i / 4u) % 2u) ? ((i / 2u) % 2u) : (i % 2u)) ? 0u : 1u);
            const coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            coreVector2       vCurDir = pEnemy->GetDirection().xy();

            if(coreVector2::Dot(vDiff, vCurDir) <= 0.0f) vCurDir = vCurDir.Rotated90() * SIGN(coreVector2::Dot(vDiff, vCurDir.Rotated90()));

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);

            pEnemy->DefaultMoveForward(vCurDir, 29.0f);

            if(STAGE_TICK_TIME(9.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();
                const coreVector2 vTan = vDir.Rotated90() * 2.0f;

                if(coreMath::IsNear(vDir.x, 0.0f) || coreMath::IsNear(vDir.y, 0.0f))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos + vTan, vDir);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos - vTan, vDir);
                }
                else
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos + vTan, vDir);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos - vTan, vDir);
                }
            }
        });

        if(m_iStageSub >= 7u)
        {
            fWhirlSpeed = MIN(fWhirlSpeed + 0.3f * TIME, 1.0f);

            const coreMatrix2 mRota    = coreMatrix3::Rotation(LERPS(0.0f, 0.7f, fWhirlSpeed) * TIME).m12();
            const coreMatrix2 mRotaRev = mRota.Transposed();

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRotaRev, 0.0f));
            });

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRota, 0.0f));
            });
        }

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            pBulletEnemy->Deactivate(true, vIntersection.xy());
        });

        STAGE_WAVE("SIEBEN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // switch everything on
    // 3: in last sub-pattern, either outer or middle tile already marked, as otherwise it would be too easy to activate all
    // 3: for coop, make sure tiles are equally distributed
    // 1,2: blending in tiles needs to be equally delayed, to not favor certain start-positions
    // enemies need to die fast after loosing invincibility
    // TODO 1: badge, helper marks N fields to active in that order
    // TODO 1: badge, 1-2 der gegner sind schon vorher abschießbar
    // TODO 1: ON tiles have X (eher nicht, schlecht in japan) or other distinct difference (maybe only in lower-right corner, or center)
    // TODO 1: wenn letztes tile schrumpft kommen funken bei size=0 und grüner helfer erscheint und fliegt weg
    // TODO 1: create a image with the light tiles ?
    // TODO 1: flipswitch galaxy: blocking tiles moving around, lasers blocking movement between tiles
    // TODO 1: maybe just kill everything when plates are active -> contrast to pacman stage
    // TODO 1: extend collision-range of tiles (to occupy 100%) not visuals, except on overlying sub-stage (like for spikes) (check with size first)
    // TODO 1: in second wave, 2 plate should already be active (2,2 und 1,3 (start with 0, oben links), oder 2,3 + 3,2)
    // TWIST (boss?): plate moves around and has positive effect when activated, negative when deactivated, never locks in
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

        STAGE_WAVE("ACHT", {20.0f, 30.0f, 40.0f, 50.0f})
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
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u, 3u})
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
    // TODO 1: 2 gegner pro seite, rotating arrow, start mit oben, gegen uhrzeigersinn
    // TODO 1: vertical enemies offset so players have to work together
    // TODO 1: use object_arrow.md3
    // TODO 1: mehr masse, zumindest eine breit gefächerte gegner-welle (möglichst früh ?)
    // TODO 1: enemies with 4 arrows
    // TODO 1: 4x4 (or more) box puzzle
    // TODO 1: big enemy with changing arrow
    // TODO 1: eine gruppe mit super-vielen
    STAGE_MAIN({TAKE_ALWAYS, 2u})
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

        const auto nEnableArrowFunc = [&](const coreUintW iArrowIndex, const coreUintW iEnemyIndex, const coreVector2 vDirection)
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

        STAGE_WAVE("NEUN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all arrows
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
            this->DisableArrow(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // bomberman hero
    // gegner fliegt auf den spieler zu, weniger nervig, spieler muss trotzdem manchmal ausweichen, lebenspunkte müssen dementsprechend hoch sein um den burst auszugleichen
    // diagonal position makes linear bombs stronger, linear position makes diagonal bombs stronger
    // make sure there is a pause between 4 and 5, make sure to start 5 from different side then 4
    // consistent explosion delays make it easier to focus one the enemies simultaneously
    // letting the enemy spawn bombs made the pattern too simple, a separate spawner creates more pressure
    // TODO 1: if not used otherwise, hide helper in bomb, it flies away on burst-line
    // TODO 1: maybe two from XY at the same time
    // TODO 1: maybe only diagonal position on linear explosion and vice-versa
    // TODO 1: 2-3 quickly after another, with pattern or just aim
    // TODO 1: reference: Monolith
    // TODO 1: smaller mines
    // TODO 1: super-huge bomb
    // TODO 1: highlight current enemy rotation
    STAGE_MAIN({TAKE_ALWAYS, 3u})
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

        const auto nTargetFunc = [](const coreBool bType, const coreVector2 vTarget, coreVector2* OUTPUT pvPos, coreVector2* OUTPUT pvForce)
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

        STAGE_WAVE("ZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all bombs
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
            this->DisableBomb(i, false);

        for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
            this->DisableBlast(i, false);

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
    // occupy areas with energy
    // rotation der großen dreiecke entgegen der flug-drehrichtung (auch gruppe 1)
    // flug-rotation um bewegung gefährlicher zu machen und gegen safe-spots
    // erste welle so aufbauen um dreiecke möglichst gleichmäßig zu verteilen, spieler kann trotzdem überlappen, könnte aber zeit kosten
    // hohe geschwindigkeit erhöht intensity und aufmerksamkeit
    // wenn flug-drehrichtung anfängt, zweite welle von der seite kommen, von wo die großen dreiecke kommen werden
    // TODO 1: helfer spawnt und fliegt in Ntem teleportiertem dreieck, verschwindet nach nächster teleportation, kleiner funken-effekt bei kill (falls doch noch sichtbar)
    // TODO 1: something after the last wave (changing the triangles (size, movement), or the number, or ...), or start ? hmmm, no I want to start up quick
    // TODO 1: also focus on difference from d/generate wave, make are strongly occupied ?
    // TODO 1: create an absurd big object
    // TODO 1: gegner-masse kommt auf spieler zu, alle mit energy, man muss loch rein schießen
    // TODO 1: small enemy triangles stay (maybe only for hard)
    // TODO 1: keep triangles from growing with attacks
    // TODO 1: movement only from right to left
    // TODO 1: (previously bullet shrink+grow, mentioned in bottom of Waves page)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
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

        STAGE_WAVE("ELF", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all blocks
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
            this->DisableBlock(i, false);

        g_pGame->GetHelper(ELEMENT_ORANGE)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // dicke kugerl die am rand bleiben
    // - all: bullet speed is much lower than enemy speed, to create a perceptible path for the player to use
    // - all: bullet frequency is not too high, to better distinguish individual bullets
    // - all: enemy attack begins later, to not slow down bullets immediately after shooting
    // - 1,2: enemy path is not too wide, to not slow down bullets immediately after shooting
    // TODO 1: enemy or golden ball (1-N) hides in blue balls, to attack for badge
    // TODO 1: blue helper in one of the bullets
    // TODO 1: multiple bullets at the same time as directed wave
    // TODO 1: increase end-size
    // TODO 1: maybe bullets flying across screen (enemies at border)
    // TODO 1: maybe all bullets get gravity, "flooding" the bottom (ZeroRanger kugerl-auge-boss)
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.5f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.5f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve (8u);
            pPath2->AddNode (coreVector2(-0.5f, 0.5f), coreVector2( 1.0f, 0.0f));
            pPath2->AddNodes(coreVector2( 0.5f, 0.5f), coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath2->AddNodes(coreVector2( 0.5f,-0.5f), coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath2->AddNodes(coreVector2(-0.5f,-0.5f), coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath2->AddNode (coreVector2(-0.5f, 0.5f), coreVector2( 0.0f, 1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,0.0f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(200, COLOR_SHIP_PURPLE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 2u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 3u, 3u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, 0.0f)

            const coreVector2 vFactor = coreVector2((i % 2u) ? -1.0f : 1.0f, (i == 2u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            if(i == 3u)
            {
                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (fLifeTime * 5.0f);
            }
            else
            {
                if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
                {
                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                }
                else
                {
                    pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                }
            }

            if(STAGE_LIFETIME_AFTER_BASE(((i == 3u) ? pPath3 : pPath1)->GetTotalDistance()) && STAGE_TICK_LIFETIME_BASE(6.0f, (i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                if(i == 3u)
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, -vDir)            ->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos,  vDir.Rotated90())->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, -vDir.Rotated90())->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                }
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            const coreFloat fFactor = LERP(0.05f, 1.0f, MIN(pBullet->GetFlyTime() * 0.5f, 1.0f));

            pBullet->SetSize(coreVector3(15.0f,15.0f,15.0f) * fFactor);
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            if(coreMath::IsNear(pBullet->GetSpeed(), 0.1f)) return;

            if(!g_pForeground->IsVisiblePoint(pBullet->GetPosition().xy(), 1.0f))
            {
                pBullet->SetSpeed(0.1f);
                return;
            }

            Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, pBullet, [](cBullet* OUTPUT pBullet1, cBullet* OUTPUT pBullet2, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                if(!g_pForeground->IsVisiblePoint(pBullet1->GetPosition().xy(), 1.0f))
                {
                    pBullet2->SetSpeed(0.1f);
                    return;
                }
            });
        });

        STAGE_WAVE("ZWÖLF", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_BOSS(m_Leviathan, {60.0f, 120.0f, 180.0, 240.0f})
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