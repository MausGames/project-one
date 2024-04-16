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
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSnowBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();                  this->EnableStar(0u, g_pGame->GetPlayer(0u));
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // evade being attacked
    // 2. group shows single-enemy split-up extended in 3. group
    // groups are positioned a bit from center, to give player a safe space to navigate among unpredictable enemy-evasion movements
    // evasion needs visual feedback, especially on multi-jumper
    // don't shoots while still at original position, will merge bullets especially on 3. group
    // only disable collision while enemy is at original position, otherwise feels unfair when they fly through bullets
    // multi-jumper needs to shoot late, to note cause immediate hit when moving into player
    // TODO: badge
    // TODO: multi-jumper shoot even later when moving into player (otherwise shoot normal)
    STAGE_MAIN
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
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 16u, 27u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 28u, 28u)
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
                    if((vDiff1.LengthSq() < 100.0f) || (vDiff2.LengthSq() < 100.0f))
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
                    afRecover[i] = MAX(afRecover[i] - 1.0f * Core::System->GetTime(), 0.0f);

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
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual((s_iTick % 4u) < 2u).Normalized();   // TODO: tick-1 ? 

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
                if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
            else
            {
                if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST) && (fDistSq < 9.0f))
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
    STAGE_MAIN
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
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB(6u)) STAGE_RESSURECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB(7u)) STAGE_RESSURECT(pSquad1,  8u, 10u)
            else if(STAGE_SUB(8u)) STAGE_RESSURECT(pSquad1, 11u, 13u)
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

            if(STAGE_TICK_LIFETIME(60.0f, 0.0f) && ((s_iTick % ((i < 8u) ? 16u : 8u)) < 4u))   // TODO: tick-1 ? 
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.4f);
            }
        });

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

        STAGE_WAVE("SIEBENUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset snow
    STAGE_MAIN
    {
        m_Snow.Disable(0.0f);

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            pPlayer->SetSpeed(1.0f);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Fenrir, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Shelob, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Zeroth, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}