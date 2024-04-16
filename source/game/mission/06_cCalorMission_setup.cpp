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
// setup the Calor mission
void cCalorMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cSnowBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
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
    // evade being attacked
    // 2. group shows single-enemy split-up extended in 3. group
    // groups are positioned a bit from center, to give player a safe space to navigate among unpredictable enemy-evasion movements
    // evasion needs visual feedback, especially on multi-jumper
    // don't shoot while still at original position, will merge bullets especially on 3. group
    // only disable collision while enemy is at original position, otherwise feels unfair when they fly through bullets
    // multi-jumper needs to shoot late, to note cause immediate hit when moving into player
    // TODO: badge
    // TODO: multi-jumper shoot even later when moving into player (otherwise shoot normal)
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 29u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(10, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            });
        });

        STAGE_GET_START(30u)
            STAGE_GET_FLOAT_ARRAY(afRecover, 29u)
            STAGE_GET_UINT       (iTrail)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 16u, 27u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 28u, 28u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vStart, vLocation, vEvade;
            if(i < 16u)
            {
                switch(i)
                {
                default: ASSERT(false)
                case  0u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f,0.2f) + coreVector2( 0.1f, 0.1f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case  1u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f,0.2f) + coreVector2( 0.1f,-0.1f); vEvade = coreVector2( 0.2f,-0.2f); break;
                case  2u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f,0.2f) + coreVector2(-0.1f, 0.1f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case  3u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f,0.2f) + coreVector2(-0.1f,-0.1f); vEvade = coreVector2(-0.2f,-0.2f); break;
                case  4u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f,0.2f) + coreVector2( 0.1f, 0.1f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case  5u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f,0.2f) + coreVector2( 0.1f,-0.1f); vEvade = coreVector2( 0.2f,-0.2f); break;
                case  6u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f,0.2f) + coreVector2(-0.1f, 0.1f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case  7u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f,0.2f) + coreVector2(-0.1f,-0.1f); vEvade = coreVector2(-0.2f,-0.2f); break;

                case  8u: vStart = coreVector2(-1.2f,-0.8f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2(-0.3f,-0.3f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case  9u: vStart = coreVector2(-1.2f,-0.8f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2(-0.3f,-0.3f); vEvade = coreVector2( 0.2f,-0.2f); break;
                case 10u: vStart = coreVector2(-1.2f, 1.2f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2(-0.3f, 0.3f); vEvade = coreVector2(-0.2f,-0.2f); break;
                case 11u: vStart = coreVector2(-1.2f, 1.2f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2(-0.3f, 0.3f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case 12u: vStart = coreVector2( 1.2f,-0.8f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2( 0.3f,-0.3f); vEvade = coreVector2(-0.2f,-0.2f); break;
                case 13u: vStart = coreVector2( 1.2f,-0.8f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2( 0.3f,-0.3f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case 14u: vStart = coreVector2( 1.2f, 1.2f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2( 0.3f, 0.3f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case 15u: vStart = coreVector2( 1.2f, 1.2f); vLocation = coreVector2( 0.0f,0.2f) + coreVector2( 0.3f, 0.3f); vEvade = coreVector2( 0.2f,-0.2f); break;
                }
            }
            else if(i < 28u)
            {
                vStart    = coreVector2(0.0f,1.2f);
                vLocation = coreVector2(0.0f,0.2f);
                vEvade    = coreVector2::Direction(((I_TO_F(i - 16u) + 0.5f) / 12.0f) * (2.0f*PI)) * (I_TO_F(((i + 0u) % 3u) + 1u) * 0.24f);
            }
            else
            {
                vStart    = coreVector2(0.0f,1.2f);
                vLocation = (iTrail < 7u) ? (coreVector2::Direction(I_TO_F(iTrail) * (3.0f/7.0f) * (2.0f*PI) + (0.0f*PI)) * 0.6f) : coreVector2(0.0f,0.0f);
                vEvade    = coreVector2(0.0f,0.0f);
            }

            if(STAGE_TAKEOFF)
            {
                pEnemy->SetPosition(coreVector3(vStart * FOREGROUND_AREA, 0.0f));
            }

            coreBool bHit = false;
            if(STAGE_LIFETIME_AFTER(0.5f))
            {
                const cBullet* pBullet = g_pGame->GetBulletManagerPlayer()->FindBullet(vLocation * FOREGROUND_AREA);
                if(pBullet)
                {
                    const coreVector2 vDiff1 = (vLocation * FOREGROUND_AREA) - (pBullet->GetPosition().xy() + pBullet->GetFlyMove());
                    const coreVector2 vDiff2 = (vLocation * FOREGROUND_AREA) - (pBullet->GetPosition().xy());
                    if((vDiff1.LengthSq() < POW2(10.0f)) || (vDiff2.LengthSq() < POW2(10.0f)))
                    {
                        bHit = true;
                    }
                }
            }

            const coreFloat fDistSq = (pEnemy->GetPosition().xy() - vLocation * FOREGROUND_AREA).LengthSq();
            if(i < 28u)
            {
                if(bHit)
                {
                    if(!afRecover[i]) g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 5.0f, 3u, COLOR_ENERGY_PURPLE);
                    afRecover[i] = 0.52f;
                }

                if(afRecover[i])
                {
                    afRecover[i] = MAX(afRecover[i] - 1.0f * TIME, 0.0f);

                    if((fDistSq > 4.0f) && STAGE_TICK_LIFETIME(2.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
            }
            else
            {
                if(fDistSq < 4.0f)
                {
                    if(iTrail && STAGE_TICK_LIFETIME(10.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual(((s_iTick % 4u) < 2u) ? 0u : 1u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                    }

                    if(bHit)
                    {
                        iTrail += 1u;
                        if(iTrail < 8u) g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 50.0f, 10u, COLOR_ENERGY_PURPLE);
                    }
                }
            }

            if((i < 28u) ? afRecover[i] : (iTrail >= 8u))
            {
                if(HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
            else
            {
                if(!HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST) && (fDistSq < 9.0f))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                    pEnemy->AddStatus   (ENEMY_STATUS_GHOST);
                }
            }

            const coreVector2 vTarget = afRecover[i] ? (vLocation + vEvade) : vLocation;

            pEnemy->DefaultMoveSmooth(vTarget, 120.0f, 12.0f);
        });

        STAGE_WAVE("SECHSUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // snowstorm makes the windscreen dirty
    // player will evade snow after explosion, it will drive players away
    // player can fly above the first group, to assume a more aggressive position
    // enemy creating snow while moving makes him nearly immune, even with offset or timed creation
    // low health, enemies are protected by snow
    // 1-4: show bullet interaction
    // 5-6: show slow down
    // 7: ramp up, tease invert
    // 8: invert
    // TODO: adjust (and fix!) snow for all weapons  
    // TODO: maybe adjust alpha shading (for fade-out on stage-end)  
    // TODO: badge  
    // TODO: schnee wirbel auf wenn man in oder aus schnee geht
    // TODO: maybe make snow edges darker to add contrast
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 14u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(10, COLOR_SHIP_BLUE);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iExplosions)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1,  8u, 10u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1, 11u, 13u)
            else if(STAGE_SUB(9u)) m_Snow.Disable(1.0f);
        }

        if(STAGE_BEGINNING)
        {
            m_Snow.Enable();
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.5f * ((i < 4u) ? 0.0f : ((i < 8u) ? I_TO_F((i - 4u) % 2u) : I_TO_F((i - 8u) % 3u))))

            const coreFloat fValue = 0.5f + 0.5f * SIN(MAX(fLifeTime, 0.0f) + 0.5f*PI);

            if(i < 4u)
            {
                pEnemy->SetPosition(coreVector3(((i < 2u) ? 0.6f : 0.2f) * ((i % 2u) ? 1.0f : -1.0f), LERP(-0.8f, 1.2f, fValue), 0.0f) * FOREGROUND_AREA3);

                if(pEnemy->ReachedDeath())
                {
                    m_Snow.DrawLine(pEnemy->GetPosition().xy(), 5.0f, true, 1u);
                    g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                    {
                        const coreVector2 vDiff = pBullet->GetPosition().xy() - pEnemy->GetPosition().xy();
                        if(ABS(vDiff.y) < 5.0f) pBullet->Deactivate(true);
                    });
                }
            }
            else if(i < 8u)
            {
                pEnemy->SetPosition(coreVector3(LERP(-0.8f, 1.2f, fValue) * ((i < 6u) ? 1.0f : -1.0f), ((i - 4u) % 2u) ? 0.4f : 0.8f, 0.0f) * FOREGROUND_AREA3);

                if(pEnemy->ReachedDeath())
                {
                    m_Snow.DrawLine(pEnemy->GetPosition().xy(), 5.0f, false, 1u);
                    g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                    {
                        const coreVector2 vDiff = pBullet->GetPosition().xy() - pEnemy->GetPosition().xy();
                        if(ABS(vDiff.x) < 5.0f) pBullet->Deactivate(true);
                    });
                }
            }
            else
            {
                pEnemy->SetPosition(coreVector3((I_TO_F((i - 8u) % 3u) - 1.0f) * ((i < 11u) ? 0.6f : -0.6f), LERP(0.0f, 1.2f, fValue), 0.0f) * FOREGROUND_AREA3);

                if(pEnemy->ReachedDeath())
                {
                    if(++iExplosions == 3u)
                    {
                        m_Snow.DrawAll(1u);
                        g_pGame->GetBulletManagerPlayer()->ClearBullets(true);
                    }
                    else
                    {
                        m_Snow.DrawPoint(pEnemy->GetPosition().xy(), 15.0f, 0u);
                    }
                }
            }

            pEnemy->DefaultRotate(fLifeTime * 3.0f);

            if(STAGE_TICK_LIFETIME(60.0f, 0.0f) && ((s_iTick % ((i < 8u) ? 16u : 8u)) < 4u))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.4f);
            }
        });

        if(m_Snow.IsActive())
        {
            g_pGame->GetBulletManagerPlayer()->ForEachBullet([this](cBullet* OUTPUT pBullet)
            {
                if(m_Snow.DrawPoint(pBullet->GetPosition().xy() + 0.5f * pBullet->GetFlyMove(), 4.0f, 0u) +
                   m_Snow.DrawPoint(pBullet->GetPosition().xy(),                                4.0f, 0u))
                {
                    pBullet->Deactivate(true);
                }
            });

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetSpeed((!pPlayer->IsRolling() && m_Snow.TestCollision(pPlayer->GetPosition().xy())) ? 0.5f : 1.0f);
            });
        }

        STAGE_WAVE("SIEBENUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset snow
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        m_Snow.Disable(0.0f);

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            pPlayer->SetSpeed(1.0f);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_WAVE("ACHTUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 5u})
    //{
    //    STAGE_BOSS(m_Fenrir, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u, 7u, 8u, 9u, 10u, 11u})
    {
        if(STAGE_BEGINNING)
        {
            
            this->EnableStar(0u, g_pGame->GetPlayer(0u)); (*m_Star.List())[0]->SetPosition(coreVector3(0.0f,0.0f,0.0f));

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // keep shooting and charge it (astral chain)
    // gegner müssen so positioniert sein, dass du vom king ablassen musst
    // first few orbs need to introduce player to orb-charge, and orb-attack (TODO: and orb-shrink if possible)
    // orb must shrink fast enough, to make it meaningful
    // gegner links und rechts vom king dürfen nicht zu nah sein, sonst schießt man sie unabsichtlich ab beim ausweichen
    // TODO: force bullet order ? red more important ?
    // TODO: continuous sound to improve understanding grow<>shrink
    // TODO: funkten bei ball-aufladung in welle verwandeln ?
    // TODO: beginnt sich ab nem bestimmten orb zu bewegen (vaus ball ?) ?
    // TODO: how to practically introduce orb shrinking ? durch (leichte) bewegung ?
    // TODO: do not remove energy-effect on king after resurrection ?
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.2f,0.95f), coreVector2(1.0f,0.0f));
            pPath2->AddStop(coreVector2( 0.0f,0.95f), coreVector2(1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.8f);
                pEnemy->Configure(200, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fRotation, fRotation = 1.0f*PI)
        STAGE_GET_END

        cEnemy* pKing = pSquad1->GetEnemy(0u);

        if(STAGE_BEGINNING)
        {
            this->EnableLoad(pKing);
        }

        const coreUintW iLoadStage = (F_TO_UI(m_afLoadPower[0]) != F_TO_UI(m_afLoadPower[1])) ? F_TO_UI(m_afLoadPower[0]) : 0u;

             if(iLoadStage ==  4u) STAGE_RESURRECT(pSquad2, 0u,  3u)
        else if(iLoadStage ==  7u) STAGE_RESURRECT(pSquad2, 4u,  7u)
        else if(iLoadStage == 10u) STAGE_RESURRECT(pSquad2, 8u, 15u)
        else if(iLoadStage == 12u)
        {
            this->DisableLoad(true);

            pKing->SetBaseColor(COLOR_SHIP_RED);
            pKing->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

            pKing->Kill(false);   // reset lifetime
            pKing->Resurrect();

            g_pSpecialEffects->MacroExplosionColorBig(pKing->GetPosition(), COLOR_ENERGY_RED);

            if(pSquad2->GetNumEnemies() == pSquad2->GetNumEnemiesAlive())
                STAGE_BADGE(BADGE_HARD, pKing->GetPosition())
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

            fRotation = FMOD(fRotation + m_afLoadPower[0] * 0.7f * TIME, 2.0f*PI);

            if(HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_INVINCIBLE))
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (fRotation);

                if(STAGE_TICK_TIME(0.6f, 0.75f))
                {
                    for(coreUintW j = 0u, je = F_TO_UI(m_afLoadPower[0]); j < je; ++j)
                    {
                        const coreVector2 vPos = m_aLoadRaw[j].GetPosition().xy();
                        const coreVector2 vDir = (g_pGame->FindPlayerDual(s_iTick % 2u)->GetPosition().xy() - vPos).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.1f);
                    }
                }
            }
            else
            {
                pEnemy->SetPosition  (coreVector3(SIN(fLifeTime) * 1.0f * FOREGROUND_AREA.x, pEnemy->GetPosition().yz()));
                pEnemy->DefaultRotate(fRotation);

                if(STAGE_TICK_LIFETIME(30.0f, 0.0f) && ((s_iTick % 8u) < 2u))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition().xy();
                    const coreFloat   fBase = I_TO_F(s_iTick) * (0.015f*PI);

                    for(coreUintW j = 6u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 30.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.1f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos, -vDir)->ChangeSize(1.1f);
                    }
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 4.0f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(I_TO_F(i % 4u) * -0.18f - 0.41f, 0.0f);

            pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                 if(i <  4u) pEnemy->Rotate270();
            else if(i <  8u) pEnemy->Rotate270()->InvertX();
            else if(i < 12u) pEnemy->InvertX();
            else if(i < 16u) {}

            const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

            pEnemy->SetDirection(coreVector3(vDir, 0.0f));

            if(STAGE_LIFETIME_AFTER(2.0f) && STAGE_TICK_TIME(0.6f, ((i % 8u) < 4u) ? 0.0f : 0.5f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
            }
        });

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS)
        {
            if(!bFirstHit || (pEnemy->GetID() != cWarriorEnemy::ID)) return;

            this->BumpLoad(I_TO_F(pBullet->GetDamage()) * 0.04f * (g_pGame->GetCoop() ? 0.5f : 1.0f));

            pBullet->Deactivate(true);
        });

        STAGE_WAVE("EINUNDACHTZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all loads
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        this->DisableLoad(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // stick to one side and stay there
    // side movement of galaga groups helps to attack them even when they attack directly
    // splitting side-groups to 4-4-4-4 (instead of 8-8) makes it easier to prevent getting hit
    // wenn man bei den seiten-gegnern im letzten moment umschwenkt und drauf hält, muss man überleben, der cross-point soll sich nicht bewegen (beware of coop)
    // TODO: make sure respawn does not cause "push" issues
    // TODO: rotate player on final push
    // TODO: own UFO model
    // TODO: make sure first sub-group highlights the speed-back-push
    // TODO: gegner welche seitlich runterkommen sieht man schwer (springen vielleicht auf die seite zu)
    // TODO: springende gegner zweite welle is orsch auszuweichen
    // TODO: can be used to highlight player beging freed from morning-star (fels reißt leicht aus mit bröckerl)
    // TODO: beschleunigung und force müssen viel viel schneller sein, spieler muss überrascht werden, richtig BAM
    STAGE_MAIN({TAKE_ALWAYS, 7u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(4u);
            pPath1->AddNode(coreVector2( 0.2f,  1.2f),  coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.7f,  0.0f),  coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.45f,-0.25f), coreVector2(1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.2f,  0.0f),  coreVector2(0.0f, 1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(6u);
            pPath2->AddNode(coreVector2(-1.2f, -0.8f),  coreVector2( 1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.2f,  0.0f),  coreVector2( 0.0f, 1.0f));
            pPath2->AddNode(coreVector2(-0.45f, 0.25f), coreVector2(-1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.7f,  0.0f),  coreVector2( 0.0f,-1.0f));
            pPath2->AddNode(coreVector2(-0.45f,-0.25f), coreVector2( 1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.2f,  0.0f),  coreVector2( 0.0f, 1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(6u);
            pPath3->AddNode (coreVector2(-1.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath3->AddNodes(coreVector2(-1.0f,-1.0f), coreVector2(0.0f,-1.0f), coreVector2(1.0f,0.0f));
            pPath3->AddNodes(coreVector2( 1.0f,-1.0f), coreVector2(1.0f, 0.0f), coreVector2(0.0f,1.0f));
            pPath3->AddNode (coreVector2( 1.0f, 1.2f), coreVector2(0.0f, 1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath4->Refine();
        });

        STAGE_ADD_PATH(pPath5)
        {
            pPath5->Reserve(11u);
            pPath5->AddNode (coreVector2(-1.2f, 0.6f), coreVector2( 1.0f, 0.0f), 0.5f);
            pPath5->AddNode (coreVector2( 0.2f, 0.4f), coreVector2( 1.0f, 0.0f), 2.0f);
            pPath5->AddNode (coreVector2( 0.4f, 0.6f), coreVector2( 0.0f, 1.0f), 1.5f);
            pPath5->AddNode (coreVector2( 0.2f, 0.8f), coreVector2(-1.0f, 0.0f), 1.0f);
            pPath5->AddNode (coreVector2( 0.0f, 0.6f), coreVector2( 0.0f,-1.0f), 0.5f);
            pPath5->AddNode (coreVector2( 0.0f, 0.5f), coreVector2( 0.0f,-1.0f), 0.0f);
            pPath5->AddNodes(coreVector2( 0.0f,-1.1f), coreVector2( 0.0f,-1.0f),              coreVector2( 0.0f, 1.0f), 5.0f, 1.0f);
            pPath5->AddNodes(coreVector2(-0.2f,-0.6f), coreVector2(-1.0f, 1.0f).Normalized(), coreVector2( 1.0f, 0.0f), 0.0f, 1.0f);
            pPath5->AddNode (coreVector2( 1.2f,-0.4f), coreVector2( 1.0f, 0.0f), 3.0f);
            pPath5->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 84u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(10, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(3.0f,1.0f,1.0f));
                pEnemy->Configure(30, COLOR_SHIP_MAGENTA);
            });
        });

        STAGE_GET_START(50u)
            STAGE_GET_VEC2_ARRAY(avForce, 24u)
            STAGE_GET_FLOAT     (fOldSpeed, fOldSpeed = g_pEnvironment->GetSpeed())
            STAGE_GET_UINT      (iPushBack, iPushBack  = true)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  4u, 19u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 35u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 36u, 43u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 44u, 59u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 60u, 67u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 68u, 83u)
            else if(STAGE_SUB(8u)) STAGE_DELAY_START

            if(m_iStageSub == 3u) STAGE_RESURRECT(pSquad2, 0u, 0u)
        }

        if(m_iStageSub < 8u)
        {
            g_pEnvironment->SetTargetSpeed(LERPH3(fOldSpeed, 16.0f, MIN(m_fStageTime * 0.5f, 1.0f)));
        }
        else
        {
            cHelper* pHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);

            if(m_fStageSubTime == 0.0f)
            {
                pHelper->Resurrect();
            }

            const coreFloat fShakeDist = pPath5->TranslateDistance(7u, 0.0f);
            const coreFloat fTotalDist = pPath5->GetTotalDistance();
            const coreFloat fTime      = MIN(DelayTime(m_fStageSubTime, fShakeDist, 1.0f), fTotalDist);

            pHelper->SetPosition(coreVector3(pPath5->CalcPosition(fTime) * FOREGROUND_AREA, 0.0f));

            if((m_fStageSubTime >= fShakeDist) && (m_fStageSubTimeBefore < fShakeDist))
            {
                g_pEnvironment->SetTargetSpeed(fOldSpeed);
                c_cast<coreFloat&>(g_pEnvironment->GetSpeed()) = fOldSpeed;

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_MAGENTA);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                iPushBack = false;

                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pPlayer->ApplyForce(coreVector2(0.0f,350.0f));
                });
            }

            if(fTime >= fTotalDist)
            {
                pHelper->Kill(false);

                STAGE_DELAY_END
            }
        }

        if(iPushBack)
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreFloat fPush = LERPH3(0.0f, -FOREGROUND_AREA.y, MIN(m_fStageTime * 0.02f, 1.0f));   // also hide Y movement (forward leaning)
                pPlayer->SetPosition(pPlayer->GetPosition() + coreVector3(0.0f, fPush, 0.0f));
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 4u) ? 0.9f : ((i < 36u) ? 1.5f : 1.15f), (i < 4u) ? (2.0f + 0.6f * I_TO_F(i)) : ((i >= 44u && i < 60u) ? (0.2f * I_TO_F((i - 44u) % 16u)) : ((i < 68u) ? (0.2f * I_TO_F((i - 4u) % 8u)) : ((i < 76u) ? 0.0f : 0.1f))))

            if(i < 4u)
            {
                STAGE_REPEAT(pPath4->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F(i) * 0.4f - 0.6f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 36u)
            {
                const coreSpline2* pPath = (i < 20u) ? pPath1 : pPath2;

                const coreVector2 vFactor = coreVector2((((i - 4u) % 16u) < 8u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                if(pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime))
                {
                    const coreFloat   fTime = MIN((fLifeTime - pPath->GetTotalDistance()) * 1.5f, 1.0f);
                    const coreVector2 vFrom = pEnemy->GetPosition().xy();
                    const coreVector2 vTo   = (coreVector2(I_TO_F(((i - 4u) % 4u) + 1u) * 0.15f, (((i - 4u) % 8u) < 4u) ? 0.6f : 0.4f) * vFactor.InvertedX() + coreVector2(0.2f * SIN(m_fStageSubTime * (0.5f*PI)), 0.0f)) * FOREGROUND_AREA;

                    pEnemy->SetPosition(coreVector3(LERPH3(vFrom.x, vTo.x, fTime), LERP(vFrom.y, vTo.y, fTime), 0.0f));
                }
            }
            else if(i < 60u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((i < 44u) || (((i - 44) % 8u) < 4u)) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                coreVector2& vForce = avForce[i - 60u];

                if(STAGE_TAKEOFF)
                {
                    pEnemy->SetPosition(coreVector3(-1.2f,-0.9f,0.0f) * FOREGROUND_AREA3);
                    vForce = coreVector2::Direction(I_TO_F((i - 60u) % 8u) / 8.0f * (-0.1f*PI) - (0.1f*PI)) * 1.65f;

                    if(i % 2u)
                    {
                        pEnemy->InvertX();
                        vForce = vForce.InvertedX();
                    }
                }

                vForce.y -= 1.0f * TIME;

                coreVector2 vNewPos = pEnemy->GetPosition().xy() + vForce * FOREGROUND_AREA * 1.5f * TIME;

                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.1f) && (vForce.x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x * 1.1f); vForce.x =  ABS(vForce.x);}
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.1f) && (vForce.x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x * 1.1f); vForce.x = -ABS(vForce.x);}
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.1f) && (vForce.y < 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.1f); vForce.y =  ABS(vForce.y);}

                pEnemy->SetPosition (coreVector3(vNewPos,             0.0f));
                pEnemy->SetDirection(coreVector3(vForce.Normalized(), 0.0f));
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 2.5f)

            const coreVector2 vFactor = coreVector2(1.0f,1.2f);
            const coreVector2 vOffset = coreVector2(0.9f,0.0f);

            if(pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime)) pEnemy->Kill(false);

            pEnemy->SetDirection(coreVector3(1.0f,0.0f,0.0f));
            pEnemy->Rotate270();

            if(pEnemy->ReachedDeath()) STAGE_BADGE(BADGE_EASY, pEnemy->GetPosition())
        });

        if((m_iStageSub >= 2u) && (m_iStageSub < 4u) && STAGE_TICK_FREE(2.5f, 0.0f))
        {
            const coreUintW iNumEnemies = pSquad1->GetNumEnemiesAlive();
            if(iNumEnemies)
            {
                coreUintW iShooter = (s_iTick * 11u) % iNumEnemies;
                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    if(iShooter--) return;

                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                });
            }
        }

        STAGE_WAVE("ZWEIUNDACHTZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->GetHelper(ELEMENT_MAGENTA)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // corpses remain on the field
    // enemies should not get attached to the side they are spawning from to reduce issues (steep angles or out-of-view kills)
    // first bullet wave needs an opening at the bottom, as player will most likely start there
    // moving enemy waves need to have good flow when killing one after another, and distinct paths (when all waves would spawn together)
    // corners should not be safe-spots
    // every bullet in the corpse-phase should be removed in the middle (though inverting it or twisting it would be allowed)
    // TODO: for coop create two safe-spots
    // TODO: enemies moving outside creating badges
    // TODO: effekte beim einschlagen in die wand, und beim einschlagen in legion, und beim wiederbelebt werden
    // TODO: wave where middle bullets faster than outer bullets, or left-right faster than up-down, then invert
    // TODO: helfer is in mitte ?
    // TODO: similar to zelda water boss https://youtu.be/HRQXOCU8OWA?t=1005
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
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

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 40u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(120u + 8u)
            STAGE_GET_VEC2_ARRAY (avChildMove,  40u)
            STAGE_GET_FLOAT_ARRAY(afChildAngle, 40u)
            STAGE_GET_UINT       (iStageMini)
            STAGE_GET_FLOAT      (fStageMiniTime)
            STAGE_GET_VEC4       (vLegionPos)
            STAGE_GET_FLOAT      (fLegionTime)
            STAGE_GET_UINT       (iLegionCount)
        STAGE_GET_END

        const coreFloat fStageMiniTimeBefore = fStageMiniTime;

        const auto nStageMiniTickFunc = [&](const coreFloat fSpeed, const coreFloat fOffset)
        {
            return ((s_iTick = F_TO_UI(fStageMiniTime * fSpeed - fOffset) - 1u) != coreUint16(F_TO_UI(fStageMiniTimeBefore * fSpeed - fOffset) - 1u));
        };

        const auto nStageMiniChangeFunc = [&](const coreUint32 iStage)
        {
            iStageMini     = iStage;
            fStageMiniTime = 0.0f;
        };

        if(m_iStageSub < 5u)
        {
            coreUint8 iNumAlive = 0u;
            pSquad1->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
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
                    pEnemy->RefreshColor();
                });
            }
        }

        coreVector2 vLegionTarget = coreVector2(0.0f,0.0f);
        if(iStageMini == 12u)
        {
            fLegionTime -= 1.0f * TIME;

            if(fLegionTime <= 0.0f)
            {
                const coreVector2 vTarget = iLegionCount ? g_pGame->FindPlayerDual(((iLegionCount - 1u) / 2u) % 2u)->GetPosition().xy() : coreVector2(0.0f, 0.2f * FOREGROUND_AREA.y);
                const coreVector2 vDiff   = vTarget - vLegionPos.zw();

                vLegionPos.xy(vLegionPos.zw());
                vLegionPos.zw(vLegionPos.zw() + vDiff.Normalized() * MIN(vDiff.Length(), 20.0f));

                fLegionTime  += 2.0f;
                iLegionCount += 1u;
            }

            vLegionTarget = LERPH3(vLegionPos.xy(), vLegionPos.zw(), CLAMP(2.0f - fLegionTime, 0.0f, 1.0f));
        }

        const coreMatrix3 mRota = coreMatrix3::Rotation(1.0f * TIME);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 2.0f, 0.25f * I_TO_F(i % 10u))

            if(iStageMini == 0u)
            {
                if(pEnemy->ReachedDeath())
                {
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);

                    g_pSpecialEffects->MacroDestructionDark(pEnemy);

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
                const coreVector2 vTarget = vLegionTarget + coreVector2::Direction(GA * fDelay) * LERPB(0.0f, 1.0f, (fDelay + 1.0f) / 40.0f) * 15.0f;

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
                    const coreVector2 vNewDir = pEnemy->GetDirection().xy() * mRota;

                    pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

                    if(nStageMiniTickFunc(1.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = (s_iTick % 2u) ? (vPos - vLegionTarget).Normalized() : pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
            }
        });

        cEnemy* pDummy = pSquad1->GetEnemy(0u);

        if(iStageMini == 1u)
        {
            if(nStageMiniTickFunc(1.0f, 0.0f))
            {
                if(s_iTick < 6u)
                {
                    for(coreUintW i = 0u; i < 80u; ++i)
                    {
                        if((s_iTick < 4u) ? ((i >= ((s_iTick + 2u) % 4u) * 20u) && (i < (((s_iTick + 2u) % 4u) + 1u) * 20u)) : ((s_iTick % 2u) == ((i / 5u) % 2u))) continue;

                        const coreVector2 vSide   = (i < 40u) ? coreVector2((i < 20u) ? 1.0f : -1.0f, 0.0f) : coreVector2(0.0f, (i < 60u) ? -1.0f : 1.0f);
                        const coreVector2 vTarget = (vSide.Rotated90() * ((I_TO_F(i % 20u) - 9.5f) * 0.1f) + vSide) * 1.1f;
                        const coreFloat   fSpeed  = 0.6f * vTarget.Length();

                        const coreVector2 vPos = vTarget * FOREGROUND_AREA;
                        const coreVector2 vDir = -vPos.Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
                else nStageMiniChangeFunc(2u);
            }
        }
        else if(iStageMini == 2u)
        {
            if(nStageMiniTickFunc(30.0f, 0.0f))
            {
                if(s_iTick < 160u)
                {
                    const coreVector2 vSide   = StepRotated90(((s_iTick / 20u) + 1u) % 4u);
                    const coreVector2 vTarget = (vSide.Rotated90() * ((I_TO_F(s_iTick % 20u) - 9.5f) * ((s_iTick < 80u) ? -0.1f : 0.1f)) + vSide) * 1.1f;
                    const coreFloat   fSpeed  = 1.2f * vTarget.Length();

                    const coreVector2 vPos = vTarget * FOREGROUND_AREA;
                    const coreVector2 vDir = -vPos.Normalized();

                                      g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos,             vDir)            ->ChangeSize(1.3f);
                    if(s_iTick < 80u) g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos * -1.0f,     vDir * -1.0f)    ->ChangeSize(1.3f);
                                 else g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos.Rotated90(), vDir.Rotated90())->ChangeSize(1.3f);
                }
                else nStageMiniChangeFunc(10u);
            }
        }

        if(iStageMini < 12u)
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([](cQuadBullet* OUTPUT pBullet)
            {
                if(pBullet->GetPosition().LengthSq() < POW2(2.0f))
                    pBullet->Deactivate(true);
            });
        }

        STAGE_WAVE("DREIUNDACHTZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 11u})
    //{
    //    STAGE_BOSS(m_Shelob, {60.0f, 120.0f, 180.0, 240.0f})
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
    // smaller stone is dragged by player with chain
    // 2: should align with swing-direction, which is easier to hit because swing-direction is against fly-direction
    // 3: clustered enemies should feel like hitting a brick-building with a wrecking ball
    // 8: removing bullet-carpet is really satisfying (feedback)
    // bullet-carpet pattern needs to make vertically moving enemies visible within
    // bullet-carpet needs to move slower, in relation to swing-speed
    // enemies within bullet-carpet should not shoot
    // temporarily loose ability to remove bullet-carpet to scare and pressure player shortly
    // enemies only come from above, to improve the feeling of facing an army
    // attacks should stay local to enemies, the difficulty is attacking and navigating around enemies without getting hit from their local attacks
    // in coop, captured enemies need to be usable on the whole screen, e.g. transfer to other player
    // TODO: boulder needs more particles when exploding as of now, try to use minimum step/power in MacroDestruction*
    // TODO: coop omg
    // TODO: helfer kommt aus stein raus ?
    // TODO: wave of big ships may overlap bad from timing
    // TODO: how to communicate that normal attacks are useless, maybe prevent shooting, maybe first enemies should wait above (but then second sub-wave might run directly into you)
    // TODO: attacks should be more front-shield-like, more mass basically, like in die recorded video
    STAGE_MAIN({TAKE_ALWAYS, 12u})
    {
        // TODO: remove    
        if(STAGE_BEGINNING)
        {
            this->EnableStar(0u, g_pGame->GetPlayer(0u));
            (*m_Star.List())[0]->SetPosition(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3);

            if(g_pGame->GetCoop())
            {
                this->EnableStar(1u, g_pGame->GetPlayer(1u));
                (*m_Star.List())[1]->SetPosition(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3);
            }

            m_Boulder.Resurrect();
            m_Boulder.SetPosition(coreVector3(0.0f,0.0f,0.0f));

            this->StartSwing();
        }

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.5f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.5f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.8f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 59u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cWarriorEnemy, 9u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(30, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        const cEnemy* pCatchObject1 = this->GetCatchObject(0u);
        const cEnemy* pCatchObject2 = this->GetCatchObject(1u);
        STATIC_ASSERT(CALOR_STARS == 2u)

        coreBool bLastCatch = false;
        if(pSquad1->IsFinished() && (pSquad2->GetNumEnemiesAlive() == 1u) && HAS_FLAG(m_Boulder.GetStatus(), ENEMY_STATUS_DEAD))
        {
            STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
            {
                if((pEnemy == pCatchObject1) || (pEnemy == pCatchObject2))
                    bLastCatch = true;
            });
        }

        if(STAGE_CLEARED || bLastCatch)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u, 13u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 14u, 34u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad2,  0u,  0u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 35u, 44u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad2,  1u,  5u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 45u, 54u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 55u, 58u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad2,  6u,  6u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad2,  7u,  7u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad2,  8u,  8u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy == pCatchObject1) || (pEnemy == pCatchObject2)) return;

            STAGE_LIFETIME(pEnemy, 0.7f, (i < 4u) ? (1.0f + (0.2f * I_TO_F(i))) : ((i < 14u) ? (0.2f * I_TO_F((i - 4u) % 10u)) : ((i < 35u) ? (0.75f * I_TO_F((i - 14u) / 7u)) : ((i < 55u) ? (0.2f * I_TO_F((i - 35u) % 10u)) : (5.0f + (0.2f * I_TO_F(i - 55u)))))))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            if(i < 14u || i >= 55u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 4u || i >= 55u) ? ((i % 2u) ? 0.9f : -0.9f) : ((I_TO_F(i - 4u) - 4.5f) * -0.15f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 35u)
            {
                const coreUintW   A     = (i - 14u) % 7u;
                const coreUintW   B     = (i - 14u) / 7u;
                const coreVector2 vBase = coreVector2::Direction((I_TO_F(A) / 6.0f) * (2.0f*PI)) * (A ? 0.2f : 0.0f);

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((B % 2u) ? 0.5f : -0.5f) * ((i < 28u) ? 1.0f : COS(fLifeTime * 2.0f)), 0.0f) + vBase;

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 55u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 35u) * 2u) % 5u) - 2.0f) * ((i < 45u) ? -0.4f : 0.4f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }

            if((i < 55u) && STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            if((pEnemy == pCatchObject1) || (pEnemy == pCatchObject2)) return;

            STAGE_LIFETIME(pEnemy, 0.7f, (i < 1u) ? 0.0f : ((i < 6u) ? (0.8f * I_TO_F((i - 1u) % 5u)) : 0.5f))

            const coreSpline2* pPath = (i == 6u) ? pPath2 : pPath1;

            if(pPath == pPath1) STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(((i >= 1 && i < 6u) ? (((i - 1u) % 2u) ? -0.4f : 0.4f) : 0.0f) * ((i < 5u) ? 1.0f : COS(fLifeTime * 2.0f)), 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

            if((i < 6u) && STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 3u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 1.0f) * 10.0f) + (1.0f*PI));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        });

        if((m_iStageSub >= 8u) && (m_iStageSub <= 11u) && STAGE_TICK_FREE(2.0f, 0.0f))
        {
            cEnemy* pDummy = pSquad1->GetEnemy(0u);

            for(coreUintW j = 30u; j--; )
            {
                const coreVector2 vPos = coreVector2((I_TO_F(j) - 14.5f) * 0.08f, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.5f, pDummy, vPos, vDir)->ChangeSize(1.3f);
            }
        }

        STAGE_WAVE("SECHSUNDACHTZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 13u})
    {
        STAGE_WAVE("SIEBENUNDACHTZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        STAGE_WAVE("ACHTUNDACHTZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 17u})
    //{
    //    STAGE_BOSS(m_Zeroth, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}