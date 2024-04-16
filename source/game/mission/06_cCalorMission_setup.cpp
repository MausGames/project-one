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
// setup the Calor mission
void cCalorMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cSnowBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
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
    // evade being attacked
    // 2. group shows single-enemy split-up extended in 3. group
    // groups are positioned a bit from center, to give player a safe space to navigate among unpredictable enemy-evasion movements
    // evasion needs visual feedback, especially on multi-jumper
    // don't shoot while still at original position, will merge bullets especially on 3. group
    // only disable collision while enemy is at original position, otherwise feels unfair when they fly through bullets
    // multi-jumper needs to shoot late, to note cause immediate hit when moving into player
    // TODO 1: badge
    // TODO 1: multi-jumper shoot even later when moving into player (otherwise shoot normal)
    // TODO 1: add enemies which move to the border (corner) perpendicular to bullet-dir
    // TODO 1: enemies evade but rotate far away from center
    // TODO 1: one enemy separating multiple times 1->2->2x2->2x2x2 and stays there
    // TODO 1: a line of enemies evading, should look like a wave
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
                if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
            else
            {
                if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST) && (fDistSq < 9.0f))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                    pEnemy->AddStatus   (ENEMY_STATUS_GHOST);
                }
            }

            const coreVector2 vTarget = afRecover[i] ? (vLocation + vEvade) : vLocation;

            pEnemy->DefaultMoveSmooth(vTarget, 120.0f, 12.0f);
        });

        STAGE_WAVE("EINUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
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
    // TODO 1: adjust (and fix!) snow for all weapons  
    // TODO 1: maybe adjust alpha shading (for fade-out on stage-end)  
    // TODO 1: badge  
    // TODO 1: schnee wirbel auf wenn man in oder aus schnee geht
    // TODO 1: maybe make snow edges darker to add contrast
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
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            pPlayer->SetSpeed((!pPlayer->IsRolling() && m_Snow.IsActive() && m_Snow.TestCollision(pPlayer->GetPosition().xy())) ? 0.5f : 1.0f);
        });

        STAGE_WAVE("ZWEIUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
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
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u, 3u})
    {
        if(STAGE_BEGINNING)
        {

        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // keep shooting and charge it (astral chain)
    // gegner müssen so positioniert sein, dass du vom king ablassen musst
    // first few orbs need to introduce player to orb-charge, and orb-attack (TODO 1: and orb-shrink if possible)
    // orb must shrink fast enough, to make it meaningful
    // gegner links und rechts vom king dürfen nicht zu nah sein, sonst schießt man sie unabsichtlich ab beim ausweichen
    // TODO 1: force bullet order ? red more important ?
    // TODO 1: continuous sound to improve understanding grow<>shrink
    // TODO 1: funkten bei ball-aufladung in welle verwandeln ?
    // TODO 1: beginnt sich ab nem bestimmten orb zu bewegen (vaus ball ?) ?
    // TODO 1: how to practically introduce orb shrinking ? durch (leichte) bewegung ?
    // TODO 1: do not remove energy-effect on king after resurrection ?
    // TODO 1: maybe he is caught in an ice block
    // TODO 1: rotate (all balls) helper around enemy to highlight state
    STAGE_MAIN({TAKE_ALWAYS, 2u})
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
                STAGE_BADGE(0u, BADGE_HARD, pKing->GetPosition())
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

            fRotation = FMOD(fRotation + m_afLoadPower[0] * 0.7f * TIME, 2.0f*PI);

            if(pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE))
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

            this->BumpLoad(I_TO_F(pBullet->GetDamage()) * 0.04f * RCP(I_TO_F(g_pGame->GetNumPlayers())));

            pBullet->Deactivate(true, vIntersection.xy());
        });

        STAGE_WAVE("DREIUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all loads
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        this->DisableLoad(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // stick to one side and stay there
    // side movement of galaga groups helps to attack them even when they attack directly
    // splitting side-groups to 4-4-4-4 (instead of 8-8) makes it easier to prevent getting hit
    // wenn man bei den seiten-gegnern im letzten moment umschwenkt und drauf hält, muss man überleben, der cross-point soll sich nicht bewegen (beware of coop)
    // TODO 1: make sure respawn does not cause "push" issues
    // TODO 1: rotate player on final push
    // TODO 1: own UFO model
    // TODO 1: make sure first sub-group highlights the speed-back-push
    // TODO 1: gegner welche seitlich runterkommen sieht man schwer (springen vielleicht auf die seite zu)
    // TODO 1: springende gegner zweite welle is orsch auszuweichen
    // TODO 1: can be used to highlight player beging freed from morning-star (fels reißt leicht aus mit bröckerl)
    // TODO 1: beschleunigung und force müssen viel viel schneller sein, spieler muss überrascht werden, richtig BAM (mit speed lines ?)
    // TODO 1: gegner teilen sich auf und bouncen nach unten (balloon harpunen game), may swim at the bottom
    STAGE_MAIN({TAKE_ALWAYS, 3u})
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

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  4u, 19u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 35u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 36u, 43u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 44u, 59u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 60u, 67u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 68u, 83u)
            else if(STAGE_SUB(8u)) STAGE_DELAY_START_CLEAR

            if(m_iStageSub == 3u) STAGE_RESURRECT(pSquad2, 0u, 0u)
        }

        if(m_iStageSub < 8u)
        {
            g_pEnvironment->SetTargetSpeedNow(LERPH3(fOldSpeed, 16.0f, MIN(m_fStageTime * 0.5f, 1.0f)));
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
                g_pEnvironment->SetTargetSpeedNow(fOldSpeed);
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

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(PLAYER_AREA_DEFAULT * RCP(iPushBack ? LERPH3(1.0f, PLAYER_AREA_FACTOR, MIN(m_fStageTime, 1.0f)) : 1.0f));
        });

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

            if(pEnemy->ReachedDeath()) STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
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

        STAGE_WAVE("VIERUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->GetHelper(ELEMENT_MAGENTA)->Kill(false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(PLAYER_AREA_DEFAULT);
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
    // everything moves against current direction
    // kein infinity für gegner, schwer nachvollziehbar, könnte sich einer verstecken
    // gegner bis zum finalen raum im dungeon dürfen nicht optional sein
    // in dungeon tunnel, gegner dürfen sich nicht hinter einer geraden befinden, die werden sonst einfach mit-erschossen
    // gegner-anordnung sollen den spieler dazu bringen sich zu drehen, unterschied 90 (leicht) und 180 (schwer)
    // erste gegner gruppe muss den spieler zum drehen bringen, während geschosse ihm noch nicht schaden zufügen können, aber schon gut sichtbar sind
    // TODO 1: could be during a sandstorm, rainstorm or other visual distortion to remove the background movement
    // TODO 1: ((bullets im letzten phasenwechsel fliegen nach außen (ghost) statt zerstört zu werden)) -> vielleicht immer
    // TODO 1: badge: enemy at the back of initial room
    // TODO 1: maybe not all bits in first phase are activatable form the start (fade in)
    // TODO 1: dungeon phases need to be created across N (60) frames
    // TODO 1: harte kollision im dungeon (direct -> 1 sec stop, otherwise just bump)
    // TODO 1: coop: average, only move if both look at same direction, both control both ships
    // TODO 1: star field ? lines from the side |> -> ?
    // TODO 1: infinity bullets für nicht-star patterns sollten früher sein
    // TODO 1: player respawn! coop! (alles friert ein, helfer bewegt spieler an alte stelle, langsame beschleunigung)
    // TODO 1: attack an enemy at the top before starting with bullets
    // TODO 1: one way would be to turn player 180 degree when colliding with wall (but not with obstacles in middle)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
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

        STAGE_WAVE("FÜNFUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->GetHelper(ELEMENT_BLUE)->Kill(false);

        STAGE_FINISH_NOW
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
    // TODO 1: boulder needs more particles when exploding as of now, try to use minimum step/power in MacroDestruction*
    // TODO 1: coop omg
    // TODO 1: helfer kommt aus stein raus ?
    // TODO 1: wave of big ships may overlap bad from timing
    // TODO 1: how to communicate that normal attacks are useless, maybe prevent shooting, maybe first enemies should wait above (but then second sub-wave might run directly into you)
    // TODO 1: attacks should be more front-shield-like, more mass basically, like in die recorded video
    // TODO 1: linien im bullet-teppich sollten abwechselnd geshiftet sein (oder 1/3 ?)
    // TODO 1: remove catch-forwarding if separator stays disabled, maybe also make swing-direction identical
    // TODO 1: simple bullet-waves inbetween for the player to overcome
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        // TODO 1: remove    
        if(STAGE_BEGINNING)
        {
            this->EnableStar(0u, g_pGame->GetPlayer(0u));
            (*m_Star.List())[0]->SetPosition(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3);

            if(g_pGame->IsMulti())
            {
                this->EnableStar(1u, g_pGame->GetPlayer(1u));
                (*m_Star.List())[1]->SetPosition(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3);

                STATIC_ASSERT(GAME_PLAYERS == 2u)
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
        if(pSquad1->IsFinished() && (pSquad2->GetNumEnemiesAlive() == 1u) && m_Boulder.HasStatus(ENEMY_STATUS_DEAD))
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

        STAGE_WAVE("SECHSUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_BOSS(m_Zeroth, {60.0f, 120.0f, 180.0, 240.0f})
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