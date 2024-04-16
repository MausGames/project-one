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
    // first groups are positioned a bit from center, to give player a safe space to navigate among unpredictable enemy-evasion movements
    // evasion needs visual feedback, especially on multi-jumper
    // directly evading enemies evade diagonal, so evasion is valid from any attack-direction
    // don't shoot while still at original position, will merge bullets especially on 3. group
    // [deprecated] only disable collision while enemy is at original position, otherwise feels unfair when they fly through bullets
    // [deprecated] multi-jumper needs to shoot late, to note cause immediate hit when moving into player
    // multi-jumper should not start at the cross of last position of path-enemy
    // multi-jumper should not move together again after splitting up
    // next groups need to spawn (source, target) where player would not attack from/into the previous group
    // uhrzeiger-gruppen haben die selbe winkel-geschwindigkeit, um sie leichter zu steuern
    // in sternen-gruppe hat jeder seinen eigenen pfad, geht nur mit ungerader anzahl gegner
    // TODO 1: badge
    // TODO 1: multi-jumper shoot even later when moving into player (otherwise shoot normal)
    // TODO 1: enemies should react to bullets from different players separately
    // TODO 1: improve path
    // TODO 1: jump-enemy should have 8 holes, not 4, maybe shift one of the 2 missing bullets
    // TODO 1: jump-enemies sollten muster haben, das nicht so weit an den rand geht (0.9f is zu weit, spieler muss zu lang fliegen, und kann sich nicht schützen wenn gegner in ihn reinfliegt und an rand drückt)
    // TODO 1: maybe new attack patterns to improve player movement, e.g. sometimes player just moves around (in path-group, and jumper-group), which is boring
    // TODO 1: attacks should really not be made when enemies move away and to original position, can happen suddenly
    // TODO 1: multi-jumper gets attackable before moving to last position, which might be wrong
    // TODO 1: reichweite der bullet-berührung vielleicht (dynamisch, oder sub-gruppen abhängig) erhöhen (e.g. in sternen gruppe, pfad gruppe)
    // TODO 1: SQRT2 not necessary ? (oh wait, 0.7071 on corner)
    // TODO 1: gegner sollen leuchten, wenn sie getriggert sind (für interpoliertes movement)
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreUintW iNumEnemiesAll  = 63u;
        constexpr coreUintW iNumEnemiesJump = 4u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(21u);
            pPath1->AddNode(coreVector2( 0.0f, 0.0f), coreVector2( 1.0f, 1.0f).Normalized());
            pPath1->AddNode(coreVector2( 0.4f, 0.4f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.1f, 0.8f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.3f, 0.4f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.6f, 0.1f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.9f, 0.4f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2(-0.6f, 0.7f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.3f, 0.4f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.7f,-0.6f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.5f,-0.8f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.0f,-0.5f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.5f,-0.8f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.7f,-0.3f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.4f, 0.0f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.1f,-0.3f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-0.6f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.7f,-0.3f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.2f, 0.5f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.5f, 0.8f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.8f, 0.5f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f,-1.0f).Normalized());
            pPath1->Refine();
            pPath1->Refine();   // double
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, iNumEnemiesAll)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            });
        });

        STAGE_GET_START(iNumEnemiesAll + iNumEnemiesJump)
            STAGE_GET_FLOAT_ARRAY(afRecover, iNumEnemiesAll)
            STAGE_GET_UINT_ARRAY (aiTrail,   iNumEnemiesJump)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 24u, 35u)   // #
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 16u, 23u)   // #
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 36u, 47u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 48u, 52u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 53u, 57u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 58u, 58u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 59u, 62u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreBool bActive = false;

            coreVector2 vStart, vLocation, vEvade;
            if(i < 24u)
            {
                switch(i)
                {
                default: ASSERT(false)
                case  0u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f, 0.2f) + coreVector2( 0.1f, 0.1f); vEvade = coreVector2( 0.2f, 0.2f);             break;
                case  1u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f, 0.2f) + coreVector2( 0.1f,-0.1f); vEvade = coreVector2( 0.2f,-0.2f);             break;
                case  2u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f, 0.2f) + coreVector2(-0.1f, 0.1f); vEvade = coreVector2(-0.2f, 0.2f);             break;
                case  3u: vStart = coreVector2(-1.2f, 0.2f); vLocation = coreVector2(-0.4f, 0.2f) + coreVector2(-0.1f,-0.1f); vEvade = coreVector2(-0.2f,-0.2f);             break;
                case  4u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f, 0.2f) + coreVector2( 0.1f, 0.1f); vEvade = coreVector2( 0.2f, 0.2f);             break;
                case  5u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f, 0.2f) + coreVector2( 0.1f,-0.1f); vEvade = coreVector2( 0.2f,-0.2f);             break;
                case  6u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f, 0.2f) + coreVector2(-0.1f, 0.1f); vEvade = coreVector2(-0.2f, 0.2f);             break;
                case  7u: vStart = coreVector2( 1.2f, 0.2f); vLocation = coreVector2( 0.4f, 0.2f) + coreVector2(-0.1f,-0.1f); vEvade = coreVector2(-0.2f,-0.2f);             break;

                case  8u: vStart = coreVector2(-1.2f,-0.8f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2(-0.3f,-0.3f); vEvade = coreVector2(-0.2f, 0.2f);             break;
                case  9u: vStart = coreVector2(-1.2f,-0.8f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2(-0.3f,-0.3f); vEvade = coreVector2( 0.2f,-0.2f);             break;
                case 10u: vStart = coreVector2(-1.2f, 1.2f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2(-0.3f, 0.3f); vEvade = coreVector2(-0.2f,-0.2f);             break;
                case 11u: vStart = coreVector2(-1.2f, 1.2f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2(-0.3f, 0.3f); vEvade = coreVector2( 0.2f, 0.2f);             break;
                case 12u: vStart = coreVector2( 1.2f,-0.8f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2( 0.3f,-0.3f); vEvade = coreVector2(-0.2f,-0.2f);             break;
                case 13u: vStart = coreVector2( 1.2f,-0.8f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2( 0.3f,-0.3f); vEvade = coreVector2( 0.2f, 0.2f);             break;
                case 14u: vStart = coreVector2( 1.2f, 1.2f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2( 0.3f, 0.3f); vEvade = coreVector2(-0.2f, 0.2f);             break;
                case 15u: vStart = coreVector2( 1.2f, 1.2f); vLocation = coreVector2( 0.0f, 0.2f) + coreVector2( 0.3f, 0.3f); vEvade = coreVector2( 0.2f,-0.2f);             break;

                case 16u: vStart = coreVector2(-1.2f,-1.2f); vLocation = coreVector2(-0.9f,-0.9f);                            vEvade = coreVector2(-0.2f, 0.0f) - vLocation; break;
                case 17u: vStart = coreVector2( 1.2f,-1.2f); vLocation = coreVector2( 0.9f,-0.9f);                            vEvade = coreVector2( 0.2f, 0.0f) - vLocation; break;
                case 18u: vStart = coreVector2(-1.2f,-1.2f); vLocation = coreVector2(-0.9f,-0.9f);                            vEvade = coreVector2( 0.2f, 0.3f) - vLocation; break;
                case 19u: vStart = coreVector2( 1.2f,-1.2f); vLocation = coreVector2( 0.9f,-0.9f);                            vEvade = coreVector2(-0.2f, 0.3f) - vLocation; break;
                case 20u: vStart = coreVector2(-1.2f,-1.2f); vLocation = coreVector2(-0.9f,-0.9f);                            vEvade = coreVector2(-0.2f, 0.6f) - vLocation; break;
                case 21u: vStart = coreVector2( 1.2f,-1.2f); vLocation = coreVector2( 0.9f,-0.9f);                            vEvade = coreVector2( 0.2f, 0.6f) - vLocation; break;
                case 22u: vStart = coreVector2(-1.2f,-1.2f); vLocation = coreVector2(-0.9f,-0.9f);                            vEvade = coreVector2( 0.2f, 0.8f) - vLocation; break;
                case 23u: vStart = coreVector2( 1.2f,-1.2f); vLocation = coreVector2( 0.9f,-0.9f);                            vEvade = coreVector2(-0.2f, 0.8f) - vLocation; break;
                }
            }
            else if(i < 36u)
            {
                vStart    = coreVector2(0.0f,1.2f);
                vLocation = coreVector2(0.0f,0.2f);
                vEvade    = coreVector2::Direction(((I_TO_F(i - 24u) + 0.5f) / 12.0f) * (2.0f*PI)) * (I_TO_F(((i - 8u) % 3u) + 1u) * 0.25f);
            }
            else if(i < 59u)
            {
                coreFloat fFactor = 0.0f;
                if(i < 48u)
                {
                    const coreFloat   fSide   = (i < 42u) ? -1.0f : 1.0f;
                    const coreFloat   fHeight = (I_TO_F((i - 36u) % 6u) - 1.5f) * 0.2f;
                    const coreVector2 vFrom   = coreVector2( 0.9f * fSide, fHeight);
                    const coreVector2 vTo     = coreVector2(-0.9f * fSide, fHeight);

                    fFactor   = MIN1(afRecover[i] * RCP((vFrom - vTo).Length()));
                    vStart    = coreVector2(1.2f * fSide, fHeight);
                    vLocation = LERP(vFrom, vTo, fFactor);
                }
                else if(i < 53u)
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(i - 48u) * (1.0f/5.0f) * (2.0f*PI));

                    fFactor   = MIN1(afRecover[i] * 0.15f);
                    vStart    = vDir * (1.2f * SQRT2);
                    vLocation = vDir * (0.5f * SIN(LERP(0.5f*PI, 4.0f*PI, fFactor)));
                }
                else if(i < 58u)
                {
                    const coreFloat fHeight = (I_TO_F((i - 53u) % 5u) + 1.0f) * 0.2f;

                    fFactor   = MIN1(afRecover[i] * 0.2f);
                    vStart    = coreVector2(-1.2f,-1.2f);
                    vLocation = MapToAxis(coreVector2(0.0f, fHeight), coreVector2::Direction(LERP(-0.25f*PI, -3.0f*PI, fFactor)));
                }
                else
                {
                    fFactor   = MIN1(afRecover[i] * RCP(pPath1->GetTotalDistance()));
                    vStart    = coreVector2(1.2f,0.0f);
                    vLocation = pPath1->CalcPositionLerp(fFactor);
                }

                if(fFactor >= 1.0f) bActive = true;

                vEvade = coreVector2(0.0f,0.0f);
            }
            else
            {
                const coreUint32 iTrail = aiTrail[i - 59u];

                     if(iTrail ==  0u) vLocation = coreVector2(-0.5f,0.9f);
                else if(iTrail >= 12u) vLocation = coreVector2( 0.0f,0.0f);
                else
                {
                         if((i == 60u)             && (iTrail >= 4u)) vLocation = coreVector2::Direction(I_TO_F(iTrail) * (3.0f/7.0f) * (2.0f*PI) + (1.0f*PI)) * 0.9f;
                    else if((i == 62u)             && (iTrail >= 4u)) vLocation = coreVector2::Direction(I_TO_F(iTrail) * (3.0f/7.0f) * (2.0f*PI) + (1.5f*PI)) * 0.3f;
                    else if((i == 61u || i == 62u) && (iTrail >= 2u)) vLocation = coreVector2::Direction(I_TO_F(iTrail) * (3.0f/7.0f) * (2.0f*PI) + (0.5f*PI)) * 0.5f;
                    else                                              vLocation = coreVector2::Direction(I_TO_F(iTrail) * (3.0f/7.0f) * (2.0f*PI) + (0.0f*PI)) * 0.7f;
                }

                vStart = coreVector2(-0.5f,1.2f);
                vEvade = coreVector2( 0.0f,0.0f);
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
            if(i < 36u)
            {
                if(bHit)
                {
                    if(!afRecover[i]) g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 5.0f, 3u, COLOR_ENERGY_PURPLE);
                    afRecover[i] = 0.65f;
                }

                if(afRecover[i])
                {
                    afRecover[i] = MAX(afRecover[i] - 1.0f * TIME, 0.0f);

                    if((fDistSq > POW2(4.0f)) && STAGE_TICK_LIFETIME(2.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }

                bActive = (fDistSq > (vEvade * FOREGROUND_AREA).LengthSq() - POW2(10.0f));
            }
            else if(i < 59u)
            {
                if(bHit)
                {
                    afRecover[i] += 3.0f * TIME * RCP(I_TO_F(g_pGame->GetNumPlayers()));
                }

                if(afRecover[i])
                {
                    if(STAGE_TICK_LIFETIME((i == 58u) ? 10.0f : 2.0f, 0.0f))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
            }
            else
            {
                coreUint32& iTrail = aiTrail[i - 59u];

                if(bHit && (fDistSq < POW2(2.0f)))
                {
                    iTrail += 1u;
                    if(iTrail < 12u) g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 50.0f, 10u, COLOR_ENERGY_PURPLE);
                }

                if(STAGE_TICK_LIFETIME(0.5f, -0.9f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();

                    for(coreUintW j = 36u; j--; )
                    {
                        if(((j + (i - 59u) * 4u) % 18u) < 14u) continue;

                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 5.0f - 2.5f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    }
                }

                bActive = (iTrail >= 12u);
            }

            if(STAGE_LIFETIME_AFTER(0.5f) && bActive)
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
            else
            {
                if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                    pEnemy->AddStatus   (ENEMY_STATUS_GHOST);
                }
            }

            const coreVector2 vTarget = afRecover[i] ? (vLocation + vEvade) : vLocation;

            pEnemy->DefaultMoveSmooth(vTarget, 120.0f, 12.0f);
        });

        STAGE_WAVE("EINUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        //g_pGame->GetHelper(ELEMENT_)->Kill(false);   // TODO 1

        STAGE_FINISH_NOW
    });

    // ################################################################
    // snowstorm makes the windscreen dirty
    // player will evade snow after explosion, it will drive players away
    // player can fly above the first group, to assume a more aggressive position
    // enemy creating snow while moving makes him nearly immune, even with offset or timed creation, it also does not fit to the rest and adds no pressure
    // low health, enemies are protected by snow
    // 1-4: show bullet interaction
    // 5-6: show slow down
    // 7: ramp up, tease invert
    // 8: invert
    // creating bullets from walls did not feel good, it was just back-and-forth for the player
    // creating snow without action (drops, avalanche, etc.) felt disconnected and random
    // hiding enemies in the snow does not fit to the rest of the mechanic
    // enemy at the bottom shows invert-blast, shows that not every enemy attacks, and adds a pause
    // after some time, snow gets overwhelming and you cannot see changes anymore, so the invert-mechanic was able to help on that
    // gegner schützen sich gegenseitig mit dem schnee den sie erzeugen
    // TODO 1: adjust (and fix!) snow for all weapons
    // TODO 1: maybe adjust alpha shading (for fade-out on stage-end)
    // TODO 1: badge
    // TODO 1: permanent-effekt wenn spieler verlangsamt ist (faded, wegen raus-rein-raus-rein)
    // TODO 1: schnee wirbel auf wenn man in oder aus schnee geht (vlt. nur permanent-effekt)
    // TODO 1: final enemy needs better integration (blitze, rauch, ramp-up, soll er zerstört werden, soll es helfer sein)
    // TODO 1: improve pacing/add pause before "final enemy"
    // TODO 1: schnee hat weiterhin bullets geschluckt nach punkte-vergabe, sollte nicht sein
    STAGE_MAIN({TAKE_ALWAYS, 1u})
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
            pPath2->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.0f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 31u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(10, COLOR_SHIP_BLUE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.7f);
                pEnemy->Configure(10, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_GET_START(3u)
            STAGE_GET_UINT (iExplosions)
            STAGE_GET_FLOAT(fVortexAngle, fVortexAngle = 0.5f*PI)
            STAGE_GET_FLOAT(fVortexSpeed)
        STAGE_GET_END

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  8u, 10u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 11u, 13u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 30u, 30u)   // #
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 14u, 14u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 16u, 17u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 18u, 18u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 19u, 19u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 20u, 21u)
            else if(STAGE_SUB(16u))
            {
                STAGE_RESURRECT(pSquad1, 22u, 25u)
                STAGE_RESURRECT(pSquad2,  0u,  0u)
            }
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 26u, 29u)
            else if(STAGE_SUB(18u))
            {
                m_Snow.Disable(1.0f);
                pSquad2->ClearEnemies(true);
            }
        }

        if(STAGE_BEGINNING)
        {
            m_Snow.Enable();
        }

        const auto nLineFunc = [this](const coreVector2 vPosition, const coreFloat fSize, const coreBool bHorizontal, const eSnowType eType)
        {
            m_Snow.DrawLine(vPosition, fSize, bHorizontal, eType);

            if(eType)
            {
                g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    const coreVector2 vDiff = pBullet->GetPosition().xy() - vPosition;
                    if(ABS(vDiff.arr(bHorizontal)) < fSize) pBullet->Deactivate(true);
                });
            }
        };

        const auto nPointFunc = [this](const coreVector2 vPosition, const coreFloat fSize, const eSnowType eType)
        {
            m_Snow.DrawPoint(vPosition, fSize, eType);

            if(eType)
            {
                g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    const coreVector2 vDiff = pBullet->GetPosition().xy() - vPosition;
                    if(vDiff.LengthSq() < POW2(fSize)) pBullet->Deactivate(true);
                });
            }
        };

        const auto nAllFunc = [this](const eSnowType eType)
        {
            m_Snow.DrawAll(eType);
            if(eType) g_pGame->GetBulletManagerPlayer()->ClearBullets(true);
        };

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 14u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.5f * ((i < 4u) ? 0.0f : ((i < 8u) ? I_TO_F((i - 4u) % 2u) : I_TO_F((i - 8u) % 3u))))

                const coreFloat fValue = 0.5f + 0.5f * SIN(MAX(fLifeTime, 0.0f) + 0.5f*PI);

                if(i < 4u)
                {
                    pEnemy->SetPosition(coreVector3(((i < 2u) ? 0.6f : 0.2f) * ((i % 2u) ? 1.0f : -1.0f), LERP(-0.8f, 1.2f, fValue), 0.0f) * FOREGROUND_AREA3);

                    if(pEnemy->ReachedDeath())
                    {
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, true, SNOW_TYPE_ADD);
                    }
                }
                else if(i < 8u)
                {
                    pEnemy->SetPosition(coreVector3(LERP(-0.8f, 1.2f, fValue) * ((i < 6u) ? 1.0f : -1.0f), ((i - 4u) % 2u) ? 0.4f : 0.8f, 0.0f) * FOREGROUND_AREA3);

                    if(pEnemy->ReachedDeath())
                    {
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, false, SNOW_TYPE_ADD);
                    }
                }
                else
                {
                    pEnemy->SetPosition(coreVector3((I_TO_F((i - 8u) % 3u) - 1.0f) * ((i < 11u) ? 0.6f : -0.6f), LERP(0.0f, 1.2f, fValue), 0.0f) * FOREGROUND_AREA3);

                    if(pEnemy->ReachedDeath())
                    {
                        if(++iExplosions == 3u)
                        {
                            nAllFunc(SNOW_TYPE_ADD);
                        }
                        else
                        {
                            nPointFunc(pEnemy->GetPosition().xy(), 15.0f, SNOW_TYPE_REMOVE);
                        }
                    }
                }

                pEnemy->DefaultRotate(fLifeTime * 3.0f);

                if(STAGE_TICK_LIFETIME(60.0f, 0.0f) && ((s_iTick % ((i < 8u) ? 16u : 8u)) < 4u))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                }
            }
            else if(i < 22u)
            {
                STAGE_LIFETIME(pEnemy, (i < 18u) ? 0.7f : 1.4f, (i < 18u) ? ((i == 17u) ? 0.5f : 0.1f) : 0.0f)

                if(i < 18u)
                {
                    const coreFloat X = 0.9f * SIN(fLifeTime + I_TO_F(((i - 14u) * 2u + 1u) % 5u) * (0.5f*PI));
                    const coreFloat Y = FmodRange(1.2f - MAX0(fLifeTime), -1.2f, 1.2f);

                    pEnemy->SetPosition(coreVector3(X, Y, 0.0f) * FOREGROUND_AREA3);
                }
                else
                {
                    const coreFloat   fSide = (i < 20u) ? -1.0f : 1.0f;
                    const coreVector2 vDir  = coreVector2::Direction((fLifeTime - I_TO_F((i - 18u) % 2u) * (1.0f*PI)) * fSide);
                    const coreFloat   fLen  = LERPB(1.2f, 0.6f, MIN1(fLifeTime * 0.1f)) * FOREGROUND_AREA.x;

                    pEnemy->SetPosition(coreVector3(vDir * fLen, 0.0f));
                }

                if(pEnemy->ReachedDeath())
                {
                    nPointFunc(pEnemy->GetPosition().xy(), 30.0f, SNOW_TYPE_INVERT);
                }

                pEnemy->DefaultRotate(fLifeTime * 3.0f);

                if(STAGE_TICK_LIFETIME(60.0f / fLifeSpeed, 0.0f) && ((s_iTick % 8u) < 4u))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                }
            }
            else if(i < 31u)
            {
                STAGE_LIFETIME(pEnemy, (i < 30u) ? 1.0f : 0.5f, (i < 30u) ? (2.0f + 0.4f * I_TO_F((i - 22u) % 4u)) : 0.0f)

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 26u) ? ((I_TO_F(i - 22u) - 1.5f) * 0.6f) : ((i < 30u) ? ((I_TO_F(((i - 26u) * 2u + 1u) % 5u) - 1.5f) * 0.6f) : -0.8f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 22u) pEnemy->Rotate90 ();
                else if(i == 23u) pEnemy->Rotate90 ()->InvertX();
                else if(i == 24u) pEnemy->Rotate90 ();
                else if(i == 25u) pEnemy->Rotate90 ()->InvertX();
                else if(i <  30u) {}
                else if(i <  31u) pEnemy->Rotate270();

                if(pEnemy->ReachedDeath())
                {
                    if(i < 26u)
                    {
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, true,  SNOW_TYPE_INVERT);
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, false, SNOW_TYPE_INVERT);
                    }
                    else if(i < 30u)
                    {
                        nAllFunc(SNOW_TYPE_INVERT);
                    }
                    else
                    {
                        nPointFunc(pEnemy->GetPosition().xy(), 30.0f, SNOW_TYPE_INVERT);
                    }
                }

                pEnemy->DefaultRotate(fLifeTimeBase * 3.0f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

            fVortexSpeed  = CLAMP(fVortexSpeed + ((m_iStageSub == 17u) ? -0.5f : 0.5f) * TIME, -1.0f, 1.0f);
            fVortexAngle += fVortexSpeed * TIME;

            pEnemy->DefaultRotate(fVortexAngle);

            if(STAGE_TICK_LIFETIME(20.0f / fLifeSpeed, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.4f);
                //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos, -vDir)->ChangeSize(1.4f);
            }
        });

        if(m_Snow.IsActive())
        {
            g_pGame->GetBulletManagerPlayer()->ForEachBullet([this](cBullet* OUTPUT pBullet)
            {
                if(m_Snow.DrawPoint(pBullet->GetPosition().xy() + 0.5f * pBullet->GetFlyMove(), 4.0f, SNOW_TYPE_REMOVE) +
                   m_Snow.DrawPoint(pBullet->GetPosition().xy(),                                4.0f, SNOW_TYPE_REMOVE) +
                   m_Snow.DrawPoint(pBullet->GetPosition().xy() - 0.5f * pBullet->GetFlyMove(), 3.0f, SNOW_TYPE_REMOVE))
                {
                    pBullet->Deactivate(true);
                }
            });

            const cEnemy* pVortexEnemy = pSquad2->GetEnemy(0u);
            if(!pVortexEnemy->HasStatus(ENEMY_STATUS_DEAD))
            {
                m_Snow.DrawPoint(pVortexEnemy->GetPosition().xy(), 6.0f, SNOW_TYPE_ADD);   // after player-bullet interaction
            }
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            pPlayer->SetMoveSpeed((!pPlayer->IsRolling() && m_Snow.IsActive() && m_Snow.TestCollision(pPlayer->GetPosition().xy())) ? 0.2f : 1.0f);
        });

        STAGE_WAVE("ZWEIUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->GetHelper(ELEMENT_BLUE)->Kill(false);

        m_Snow.Disable(0.0f);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetMoveSpeed(1.0f);
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
    // everything moves against current direction
    // kein infinity für gegner, schwer nachvollziehbar, könnte sich einer verstecken
    // alle gegner im dungeon sind optional, man könnte einen verpassen und backtracking is unmöglich, portale sind fad, auf punkte achten!
    // [deprecated] in dungeon tunnel, gegner dürfen sich nicht hinter einer geraden befinden, die werden sonst einfach mit-erschossen
    // alle gegner im dungeon haben ihre eigene row, colum, und sind nicht identisch zwischen räumen
    // gegner-anordnung sollen den spieler dazu bringen sich zu drehen, unterschied 90 (leicht) und 180 (schwer)
    // erste gegner gruppe muss den spieler zum drehen bringen, während geschosse ihm noch nicht schaden zufügen können, aber schon gut sichtbar sind
    // fallen an türen locken den spieler dort hin, und weil sie unterschiedlich sind weiß man wo man rein und raus gekommen ist
    // gegner gruppen in den ersten zwei phasen müssen einen guten flow haben (mit augenmerk auf spieler-rotation), aufpassen, dass die nächsten gegner nicht gleich erschossen werden
    // letzter gegner der zweiten phase muss den spieler zum dungeon-eingang leiten (dreht sich schon)
    // eckiger tunnel im dungeon bereitet den spieler vor
    // außerhalb des dungeons sollten keine gegner sein, außer einer, weil man sie sonst leicht verpassen kann
    // TODO 1: could be during a sandstorm, rainstorm or other visual distortion to remove the background movement
    // TODO 1: ((bullets im letzten phasenwechsel fliegen nach außen (ghost) statt zerstört zu werden)) -> vielleicht immer (vielleicht nur partikel nach außen, allgemeiner effekt ?)
    // TODO 1: badge: enemy at the back of initial room
    // TODO 1: maybe not all bits in first phase are activatable form the start (fade in)
    // TODO 1: harte kollision im dungeon (direct -> 1 sec stop, otherwise just bump)
    // TODO 1: one way would be to turn player 180 degree when colliding with wall (but not with obstacles in middle) (what about reverse movement?)
    // TODO 1: coop: average, only move if both look at same direction, both control both ships
    // TODO 1: player respawn! coop! (alles friert ein, helfer bewegt spieler an alte stelle, langsame beschleunigung)
    // TODO 1: geschoss-wände in der press-kammer sollten nur zerstört werden wenn sie an "ihren" rand gelangen (mit damage/id regeln), oder einfach alle invincible erzeugen, und movement clampen!
    // TODO 1: geschoss-wände in der press-kammer sollten auch entlang unendlich sein, damit ein schöner raster-rahmen entsteht
    // TODO 1: increase ForEachBulletTyped efficiency (and GetNumBullets)
    // TODO 1: press-wände müssen verschwinden, wenn spieler sie berührt (auch bei feel)
    // TODO 1: bullets in walls brauchen 100% collision range (handle BULLET_COLLISION_FACTOR in), vielleicht auch bei vielen andere waves, implement API
    // TODO 1: helper sollte pfeil + wellen haben, um den spieler dorthin zu locken
    STAGE_MAIN({TAKE_ALWAYS, 2u})
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

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 63u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(6u)
            STAGE_GET_VEC2(vGlobalOffset)
            STAGE_GET_UINT(iBulletField)
            STAGE_GET_UINT(iTrapIndex)
            STAGE_GET_UINT(iCreationDelay)
            STAGE_GET_UINT(iCreationEnemy, iCreationEnemy = 44u)
        STAGE_GET_END

        cEnemy*  pDummy  = pSquad1->GetEnemy(0u);
        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_BLUE);

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  2u,  6u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  7u, 11u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 12u, 16u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 17u, 26u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 27u, 34u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 35u, 42u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1, 43u, 43u)
            else if(STAGE_SUB(9u)) STAGE_RESURRECT(pSquad1, 44u, 62u)

            if((m_iStageSub == 6u) || (m_iStageSub == 9u))
            {
                vGlobalOffset = coreVector2(0.0f,0.0f);
                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
            }
        }

        constexpr coreFloat fScale = 0.1f;
        const auto nPositionFunc = [&](const coreInt32 x, const coreInt32 y, const coreVector2 vShift = coreVector2(0.0f,0.0f))
        {
            return (coreVector2(I_TO_F(x), I_TO_F(y) + 2.5f) + vShift) * (FOREGROUND_AREA * fScale * 6.0f) + vGlobalOffset;
        };

        if((m_iStageSub == 9u) && (iCreationDelay < 6u))
        {
            const auto nCreateBlockFunc = [&](const coreInt32 x, const coreInt32 y, const coreUint8 iShape = 0u)
            {
                const coreVector2 vBase = nPositionFunc(x, y);

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
                const coreVector2 vBase = nPositionFunc(x, y);

                pSquad1->GetEnemy(iCreationEnemy++)->SetPosition(coreVector3(vBase, 0.0f));
            };

            switch(iCreationDelay++)
            {
            default: ASSERT(false)
            case 0u:
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

                nCreateEnemyFunc( 0, -6);
                break;

            case 1u:
                nCreateBlockFunc( 0,  0, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0,  1, BIT(3u) | BIT(0u));
                nCreateBlockFunc( 1,  1, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 2,  1, BIT(2u) | BIT(1u));
                nCreateBlockFunc( 2,  2, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 2,  3, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 1,  3, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 0,  3, BIT(2u) | BIT(0u));
                nCreateBlockFunc(-1,  3, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-1,  4, BIT(3u) | BIT(1u));

                nCreateEnemyFunc( 0,  1);
                nCreateEnemyFunc( 2,  1);
                nCreateEnemyFunc( 2,  3);
                break;

            case 2u:
                nCreateBlockFunc(-3,  5, BIT(3u) | BIT(2u) | BIT(0u));
                nCreateBlockFunc(-2,  5, BIT(2u) | BIT(1u));
                nCreateBlockFunc( 0,  5, BIT(2u));
                nCreateBlockFunc( 1,  5, BIT(2u) | BIT(1u));
                nCreateBlockFunc(-3,  6, BIT(3u));
                nCreateBlockFunc(-1,  6, BIT(0u));
                nCreateBlockFunc( 0,  6, BIT(1u));
                nCreateBlockFunc( 1,  6, BIT(1u));
                nCreateBlockFunc(-3,  7, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0,  7, BIT(0u));
                nCreateBlockFunc( 1,  7, BIT(1u));
                nCreateBlockFunc(-3,  8, BIT(3u));
                nCreateBlockFunc(-2,  8, BIT(0u));
                nCreateBlockFunc( 1,  8, BIT(1u) | BIT(0u));
                nCreateBlockFunc(-3,  9, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-2,  9, BIT(0u));
                nCreateBlockFunc(-1,  9, BIT(1u));
                nCreateBlockFunc( 0,  9, BIT(0u));
                nCreateBlockFunc( 1,  9, BIT(1u) | BIT(0u));

                nCreateEnemyFunc(-3,  5);
                nCreateEnemyFunc( 1,  6);
                nCreateEnemyFunc(-1,  7);
                nCreateEnemyFunc(-2,  8);
                nCreateEnemyFunc( 0,  9);
                break;

            case 3u:
                nCreateBlockFunc(-1, 10, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-1, 11, BIT(3u) | BIT(1u));

                nCreateBlockFunc(-3, 12, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-2, 12, BIT(2u) | BIT(0u));
                nCreateBlockFunc(-1, 12, BIT(0u));
                nCreateBlockFunc( 0, 12, BIT(2u) | BIT(1u) | BIT(0u));
                nCreateBlockFunc( 1, 12, BIT(2u) | BIT(1u));
                nCreateBlockFunc(-3, 13, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-1, 13, BIT(0u));
                nCreateBlockFunc( 0, 13, BIT(0u));
                nCreateBlockFunc( 1, 13, BIT(1u));
                nCreateBlockFunc(-3, 14, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-2, 14, BIT(1u));
                nCreateBlockFunc( 0, 14, BIT(1u));
                nCreateBlockFunc( 1, 14, BIT(1u));
                nCreateBlockFunc(-3, 15, BIT(3u));
                nCreateBlockFunc(-2, 15, BIT(0u));
                nCreateBlockFunc(-1, 15, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 0, 15, BIT(0u));
                nCreateBlockFunc( 1, 15, BIT(1u));
                nCreateBlockFunc(-3, 16, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-2, 16, BIT(0u));
                nCreateBlockFunc( 0, 16, BIT(0u));
                nCreateBlockFunc( 1, 16, BIT(1u) | BIT(0u));

                nCreateEnemyFunc( 1, 12);
                nCreateEnemyFunc(-1, 13);
                nCreateEnemyFunc(-3, 14);
                nCreateEnemyFunc( 0, 15);
                nCreateEnemyFunc(-2, 16);
                break;

            case 4u:
                nCreateBlockFunc(-1, 17, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-1, 18, BIT(3u) | BIT(1u));

                nCreateBlockFunc(-3, 19, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-2, 19, BIT(2u) | BIT(0u));
                nCreateBlockFunc(-1, 19, BIT(0u));
                nCreateBlockFunc( 0, 19, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 1, 19, BIT(2u) | BIT(1u));
                nCreateBlockFunc(-3, 20, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0, 20, BIT(1u));
                nCreateBlockFunc( 1, 20, BIT(1u));
                nCreateBlockFunc(-3, 21, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-2, 21, BIT(1u));
                nCreateBlockFunc(-1, 21, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 0, 21, BIT(1u));
                nCreateBlockFunc( 1, 21, BIT(1u));
                nCreateBlockFunc(-3, 22, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-2, 22, BIT(0u));
                nCreateBlockFunc( 0, 22, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 1, 22, BIT(1u));
                nCreateBlockFunc(-3, 23, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-2, 23, BIT(0u));
                nCreateBlockFunc(-1, 23, BIT(0u));
                nCreateBlockFunc( 0, 23, BIT(0u));
                nCreateBlockFunc( 1, 23, BIT(1u) | BIT(0u));

                nCreateEnemyFunc(-2, 19);
                nCreateEnemyFunc( 0, 20);
                nCreateEnemyFunc( 1, 21);
                nCreateEnemyFunc(-1, 22);
                nCreateEnemyFunc(-3, 23);
                break;

            case 5u:
                iTrapIndex = g_pGame->GetBulletManagerEnemy()->GetNumBullets();

                for(coreUintW i = 0u; i < 34u; ++i)
                {
                    const coreVector2 vPos = HIDDEN_POS;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.0f, pDummy, vPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }

                pHelper->Resurrect();
                pHelper->SetPosition(coreVector3(nPositionFunc(-1, 21), 0.0f));
                break;
            }
        }

        const coreFloat   fGlobalSpeed = 30.0f;
        const coreVector2 vGlobalDir   = g_pGame->GetPlayer(0u)->GetDirection().xy();
        const coreVector2 vGlobalMove  = vGlobalDir * (-fGlobalSpeed * TIME);

        vGlobalOffset += vGlobalMove;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, (i < 2u) ? (0.5f + 0.2f * I_TO_F(i % 2u)) : ((i < 17u) ? (0.2f * I_TO_F((i - 2u) % 5u)) : 0.0f))

            if(i < 44u)
            {
                const coreSpline2* pPath = (i < 2u || i >= 27u) ? pPath1 : pPath2;

                if(pPath == pPath2) STAGE_REPEAT(pPath->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 2u) ? ((i % 2u) ? -0.9f : -0.7f) : ((i < 17u) ? -0.8f : ((i < 27u) ? ((I_TO_F(i - 17u) - 4.5f) * 0.22f) : ((i < 43u) ? ((I_TO_F((i - 27u) % 4u) - 1.5f) * 0.3f) : 0.0f))), 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  2u) pEnemy->Rotate270();
                else if(i <  7u) pEnemy->Rotate90 ();
                else if(i < 12u) pEnemy->Rotate180();
                else if(i < 17u) {}
                else if(i < 27u) pEnemy->Rotate180();
                else if(i < 31u) pEnemy->Rotate180();
                else if(i < 35u) {}
                else if(i < 39u) pEnemy->Rotate270();
                else if(i < 43u) pEnemy->Rotate90 ();
                else if(i < 44u) {}
            }
            else
            {
                const coreVector2 vNewPos = pEnemy->GetPosition().xy() + vGlobalMove;

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            if(i >= 43u) pEnemy->DefaultRotate(fLifeTime * (2.0f*PI));
        });

        if(m_iStageSub < 6u)
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
        else if(m_iStageSub < 9u)
        {
            const coreInt32 iStep = PackDirection(vGlobalMove) + 1;

            coreBool bExist = false;
            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](const cOrbBullet* pBullet)
            {
                if(pBullet->GetDamage() == iStep) bExist = true;
            });

            if(!bExist)
            {
                for(coreUintW i = 0u; i < 22u; ++i)
                {
                    const coreVector2 vPos = MapToAxis(coreVector2((I_TO_F(i) - 11.0f) * 0.11f, 1.1f * 1.1f) * FOREGROUND_AREA, vGlobalDir);
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(iStep, 0.0f, pDummy, vPos, vDir)->ChangeSize(1.9f);
                }
            }
        }
        else if(iTrapIndex)
        {
            coreUint32 i = 0u - iTrapIndex;
            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
            {
                if(i < 8u)   // two blocks
                {
                    const coreVector2 vBase   = nPositionFunc(-1, 4, coreVector2(0.0f, (i / 4u) ? -0.25f : 0.25f));
                    const coreVector2 vOffset = coreVector2(I_TO_F(i % 2u) - 0.5f, I_TO_F((i / 2u) % 2u) - 0.5f) * 0.11f * FOREGROUND_AREA;
                    const coreFloat   fHeight = SIN(m_fStageSubTime * (1.0f*PI) + I_TO_F(i / 4u) * (1.0f*PI)) * 10.0f;

                    pBullet->SetPosition(coreVector3(vBase + vOffset + coreVector2(fHeight, 0.0f), 0.0f));
                }
                else if(i < 24u)   // rotator
                {
                    const coreVector2 vBase   = nPositionFunc(-1, 17, coreVector2(0.0f,0.5f));
                    const coreVector2 vOffset = coreVector2::Direction(m_fStageSubTime * (0.5f*PI) + I_TO_F(i - 8u) * (0.1f*PI)) * 18.0f;

                    pBullet->SetPosition(coreVector3(vBase + vOffset, 0.0f));
                }
                else if(i < 34u)   // snake
                {
                    const coreVector2 vBase   = nPositionFunc(-1, 10, coreVector2(0.0f, 0.5f + 0.2f * (I_TO_F(i - 24u) - 4.5f)));
                    const coreFloat   fHeight = SIN(m_fStageSubTime * (-1.0f*PI) + I_TO_F(i - 24u) * (0.2f*PI)) * 10.0f;

                    pBullet->SetPosition(coreVector3(vBase + coreVector2(fHeight, 0.0f), 0.0f));
                }

                ++i;
            });
        }

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
        {
            coreVector2 vNewPos = pBullet->GetPosition().xy() + vGlobalMove;

            if(m_iStageSub < 6u)
            {
                      if((vNewPos.x < -FOREGROUND_AREA.x * 1.4f) && (vGlobalMove.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.8f;
                 else if((vNewPos.x >  FOREGROUND_AREA.x * 1.4f) && (vGlobalMove.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.8f;
                      if((vNewPos.y < -FOREGROUND_AREA.y * 1.4f) && (vGlobalMove.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.8f;
                 else if((vNewPos.y >  FOREGROUND_AREA.y * 1.4f) && (vGlobalMove.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.8f;
            }

            pBullet->SetPosition(coreVector3(vNewPos, 0.0f));
        });

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vNewPos = pHelper->GetPosition().xy() + vGlobalMove;

            pHelper->SetPosition(coreVector3(vNewPos, 0.0f));

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreVector2 vDiff = pPlayer->GetPosition().xy() - vNewPos;

                if(vDiff.LengthSq() < POW2(4.0f))
                {
                    pSquad1->ClearEnemies(true);
                    pHelper->Kill(true);
                }
            });
        }

        STAGE_WAVE("DREIUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->GetHelper(ELEMENT_BLUE)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // stick to one side and stay there
    // side movement of galaga groups helps to attack them even when they attack directly
    // splitting side-groups to 4-4-4-4 (instead of 8-8) makes it easier to prevent getting hit
    // wenn man bei den seiten-gegnern im letzten moment umschwenkt und drauf hält, muss man überleben, der cross-point soll sich nicht bewegen (beware of coop)
    // gegner können sich nicht zu schnell in/gegen flugrichtung bewegen, man nimmt sie nicht war wegen dem schnellen boden
    // hat nicht funktioniert: pang (boring + slow), flip speed (too unpredictable crashing into stuff), rotate (there was no meaningful mechanic, just visual, player-controlled needs incentive and this is only done by shooting along border)
    // erste gruppe soll dog-fight feeling vermitteln
    // TODO 1: make sure player-respawn does not cause "push" issues
    // TODO 1: own UFO model
    // TODO 1: springende gegner zweite welle is orsch auszuweichen
    // TODO 1: jumping worm is too boring
    // TODO 1: can be used to highlight player beging freed from morning-star (fels reißt leicht aus mit bröckerl)
    // TODO 1: beschleunigung und force müssen viel viel schneller sein, spieler muss überrascht werden, richtig BAM (mit speed lines ?)
    // TODO 1: gegner mit geschoss linien aus beiden seiten, vertikale und horizontale bewegung
    // TODO 1: erste gruppe vielleicht zu lang
    // TODO 1: vielleicht 2. galaga gruppe alternierend auf-ab bewegen statt links-rechts
    // TODO 1: die links-rechts wurm gruppe is zu schnell oder viel, vielleicht von 4 auf 3, oder beschleunigen
    // TODO 1: der ruck am ende is zu stark, kA warum
    // TODO 1: ganz am anfang wird man nach unten gedrückt, aber wenn man nicht schießt kann man entkommen, geschosse am unteren rand
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

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 92u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(30, COLOR_SHIP_MAGENTA);
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

        STAGE_GET_START(34u + 2u * GAME_PLAYERS)
            STAGE_GET_VEC2_ARRAY(avForce,   16u)
            STAGE_GET_VEC2_ARRAY(avTurn,    GAME_PLAYERS)
            STAGE_GET_FLOAT     (fOldSpeed, fOldSpeed = g_pEnvironment->GetSpeed())
            STAGE_GET_UINT      (iPushBack, iPushBack  = true)
        STAGE_GET_END

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 11u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 12u, 27u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 28u, 43u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 52u, 67u)   // #
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 44u, 51u)   // #
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 68u, 75u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 76u, 91u)
            else if(STAGE_SUB(8u)) STAGE_DELAY_START_CLEAR

            if(m_iStageSub == 3u) STAGE_RESURRECT(pSquad2, 0u, 0u)
        }

        const coreFloat fBoost    = CLAMP01(m_fStageTime * 0.35f);
        const coreFloat fNewSpeed = 16.0f;
        const coreFloat fCurSpeed = LERPB(fOldSpeed, fNewSpeed, fBoost);

        if(m_iStageSub < 8u)
        {
            g_pEnvironment->SetTargetSpeedNow(fCurSpeed);
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
            const coreFloat fRotaDist  = fShakeDist + 1.0f;
            const coreFloat fTime      = MIN(DelayTime(m_fStageSubTime, fShakeDist, 1.0f), fTotalDist);

            pHelper->SetPosition(coreVector3(pPath5->CalcPosition(fTime) * FOREGROUND_AREA, 0.0f));

            if(STAGE_SUBTIME_POINT(fShakeDist))
            {
                g_pEnvironment->SetTargetSpeedNow(fOldSpeed);
                c_cast<coreFloat&>(g_pEnvironment->GetSpeed()) = fOldSpeed;

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_MAGENTA);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                iPushBack = false;

                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pPlayer->ApplyForce(coreVector2(0.0f,1000.0f));
                    avTurn[i] = pPlayer->GetDirection().xy();
                });
            }

            if(STAGE_SUBTIME_BETWEEN(fShakeDist, fRotaDist))
            {
                const coreVector2 vRota = coreVector2::Direction(LERPB(0.0f*PI, 4.0f*PI, STEP(fShakeDist, fRotaDist, m_fStageSubTime)));

                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pPlayer->SetDirection(coreVector3(MapToAxis(avTurn[i], vRota), 0.0f));
                    pPlayer->AddStatus (PLAYER_STATUS_NO_INPUT_TURN);
                });
            }
            else if(STAGE_SUBTIME_POINT(fRotaDist))
            {
                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pPlayer->SetDirection(coreVector3(avTurn[i], 0.0f));
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
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
            const coreVector2 vPush = g_pEnvironment->GetDirection() * LERPB(0.5f, -1.0f, fBoost);   // also hide Y movement (forward leaning)

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetPosition(pPlayer->GetPosition() + coreVector3(vPush, 0.0f));
            });
        }

        const coreVector4 vArea   = PLAYER_AREA_DEFAULT * RCP(iPushBack ? LERPH3(1.0f, PLAYER_AREA_FACTOR, MIN(m_fStageTime * 3.0f, 1.0f)) : 1.0f);
        const coreFloat   fExaust = 0.5f * SIN(LERPB(0.0f*PI, 1.0f*PI, fBoost));

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea      (vArea);
            pPlayer->UpdateExhaust(fExaust);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 12u) ? 0.9f : ((i < 44u) ? 1.5f : ((i < 52u) ? 1.4f : 1.1f)), (i < 12u) ? (3.0f + 0.6f * I_TO_F(i)) : ((i >= 52u && i < 68u) ? (0.2f * I_TO_F((i - 52u) % 16u) + 1.2f * I_TO_F((i - 52u) / 4u)) : ((i < 76u) ? (0.2f * I_TO_F((i - 12u) % 8u)) : ((i < 84u) ? 0.0f : 0.1f))))

            if(i < 12u)
            {
                STAGE_REPEAT(pPath4->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i * 5u + 4u) % 12u) - 5.5f) * 0.15f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection   (coreVector3(0.0f,1.0f,0.0f));
            }
            else if(i < 44u)
            {
                const coreSpline2* pPath = (i < 28u) ? pPath1 : pPath2;

                const coreVector2 vFactor = coreVector2((((i - 12u) % 16u) < 8u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                if(pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime))
                {
                    const coreFloat   fTime = MIN((fLifeTime - pPath->GetTotalDistance()) * 1.5f, 1.0f);
                    const coreVector2 vMove = coreVector2(0.2f * SIN(m_fStageSubTime * (0.5f*PI)), 0.0f);
                    const coreVector2 vFrom = pEnemy->GetPosition().xy();
                    const coreVector2 vTo   = (coreVector2(I_TO_F(((i - 12u) % 4u) + (((i >= 28u) && (((i - 12u) / 2u) % 2u)) ? 2u : 1u)) * 0.15f, (((i - 12u) % 8u) < 4u) ? 0.6f : 0.4f) * vFactor.InvertedX() + vMove) * FOREGROUND_AREA;

                    pEnemy->SetPosition(coreVector3(LERPH3(vFrom.x, vTo.x, fTime), LERP(vFrom.y, vTo.y, fTime), 0.0f));
                }
            }
            else if(i < 68u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((i < 52u) || (((i - 52) % 8u) >= 4u)) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                const coreVector2 vPos   = pEnemy->GetPosition().xy();
                const coreVector2 vNorm  = AlongCrossNormal(-vPos);
                const coreFloat   fValue = coreVector2::Dot(vPos, vNorm.Rotated90()) / (FOREGROUND_AREA.x * 1.0f);
                const coreFloat   fPower = ((i >= 52u) && (vPos.y <= 0.0f)) ? 0.0f : 10.0f;
                const coreFloat   fJump  = fPower * ABS(SIN(fValue * (1.0f*PI)));

                pEnemy->SetPosition(coreVector3(vPos + vNorm * fJump, 0.0f));

                if(i < 52u) pEnemy->DefaultRotate(fLifeTime * -3.0f);
            }
            else
            {
                coreVector2& vForce = avForce[(i - 68u) % 16u];

                if(STAGE_TAKEOFF)
                {
                    pEnemy->SetPosition(coreVector3(-1.2f,-0.9f,0.0f) * FOREGROUND_AREA3);
                    vForce = coreVector2::Direction(I_TO_F((i - 68u) % 8u) / 8.0f * (-0.1f*PI) - ((i < 76u) ? (0.1f*PI) : (0.05f*PI))) * 1.65f;

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

        if((((m_iStageSub >= 1u) && (m_iStageSub < 4u)) || (m_iStageSub == 5u)) && STAGE_TIME_AFTER(2.0f) && STAGE_TICK_FREE(3.5f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
        {
            const coreUintW iNumEnemies = pSquad1->GetNumEnemiesAlive();
            if(iNumEnemies)
            {
                coreUintW iShooter = (s_iTick * ((iNumEnemies == 11u) ? 13u : 11u)) % iNumEnemies;
                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    if(iShooter--) return;

                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = (m_iStageSub == 2u) ? coreVector2(0.0f,-1.0f) : pEnemy->AimAtPlayerSideRev().Normalized();

                    if(vPos.y > 0.0f) g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                });
            }
        }

        STAGE_WAVE("VIERUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->GetHelper(ELEMENT_MAGENTA)->Kill(false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea      (PLAYER_AREA_DEFAULT);
            pPlayer->UpdateExhaust(0.0f);
            pPlayer->RemoveStatus (PLAYER_STATUS_NO_INPUT_TURN);
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
    // TODO 1: enemy-attacks should be more front-shield-like, more mass basically, like in die recorded video
    // TODO 1: linien im bullet-teppich sollten abwechselnd geshiftet sein (oder 1/3 ?)
    // TODO 1: remove catch-forwarding if separator stays disabled, maybe also make swing-direction identical
    // TODO 1: simple bullet-waves inbetween for the player to overcome
    // TODO 1: vielleicht mini-gegner oder boss-intro (10s) um morning-star zu erhalten
    // TODO 1: first side enemies on bullet-carpet more to the middle (maybe 50%)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        
        // TODO 1
        if(m_apStarOwner[0] && (m_apStarOwner[0] != &m_Zeroth) && !m_iStarState)
        {
            if((m_apStarOwner[0]->GetPosition().xy() - this->GetStar(0u)->GetPosition().xy()).LengthSq() >= POW2(CALOR_CHAIN_CONSTRAINT1))
            {
                m_Boulder.Resurrect();
                m_Boulder.SetPosition(coreVector3(0.0f,-1.25f,0.0f) * FOREGROUND_AREA3);

                this->StartSwing();

                g_pSpecialEffects->CreateSplashColor(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3, SPECIAL_SPLASH_SMALL, COLOR_ENERGY_WHITE);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }
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

        const cShip* pCatchObject1 = this->GetCatchObject(0u);
        const cShip* pCatchObject2 = this->GetCatchObject(1u);
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

        if(STAGE_TIME_POINT(4.0f))
        {
            m_Zeroth.ResurrectIntro();
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy == pCatchObject1) || (pEnemy == pCatchObject2)) return;

            STAGE_LIFETIME(pEnemy, 0.7f, (i < 4u) ? (0.2f * I_TO_F(i)) : ((i < 14u) ? (0.2f * I_TO_F((i - 4u) % 10u)) : ((i < 35u) ? (0.75f * I_TO_F((i - 14u) / 7u)) : ((i < 55u) ? (0.2f * I_TO_F((i - 35u) % 10u)) : (5.0f + (0.2f * I_TO_F(i - 55u)))))))

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
            const coreVector2 vOffset = coreVector2(((i >= 1u && i < 6u) ? (((i - 1u) % 2u) ? -0.4f : 0.4f) : 0.0f) * ((i < 5u) ? 1.0f : COS(fLifeTime * 2.0f)), 0.0f);

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

        if((m_iStageSub >= 8u) && (m_iStageSub <= 11u) && STAGE_TICK_FREE(2.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
        {
            cEnemy* pDummy = pSquad1->GetEnemy(0u);

            for(coreUintW j = 30u; j--; )
            {
                const coreVector2 vPos = coreVector2((I_TO_F(j) - 14.5f + ((s_iTick % 2u) ? -0.25f : 0.25f)) * 0.08f, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.5f, pDummy, vPos, vDir)->ChangeSize(1.3f);
            }
        }

        STAGE_WAVE("FÜNFUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        //g_pGame->GetHelper(ELEMENT_)->Kill(false);   // TODO 1

        STAGE_FINISH_NOW
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
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
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

            this->BumpLoad(I_TO_F(pBullet->GetDamage()) * 0.03f * RCP(I_TO_F(g_pGame->GetNumPlayers())));

            pBullet->Deactivate(true, vIntersection.xy());
        });

        STAGE_WAVE("SECHSUNDDREISSIG", {60.0f, 80.0f, 100.0f, 120.0f})
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        //g_pGame->GetHelper(ELEMENT_)->Kill(false);   // TODO 1

        this->DisableLoad(false);

        STAGE_FINISH_NOW
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