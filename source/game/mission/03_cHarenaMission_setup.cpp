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
// setup the Harena mission
void cHarenaMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cDesertBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
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
    // TODO: special colored enemies, all need to be killed for a badge (line border attack)
    // TODO: pacifist: holes in line border attack
    // BADGE: kill N% enemies at the start
    // BADGE: kill marked enemies on the line-wave
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreUintW iNumEnemies = 108u;
        constexpr coreUintW iNumTypes   = (iNumEnemies + 3u) / 4u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.30f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.75f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 8u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(4 * 2, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(2u + iNumTypes + 2u * iNumEnemies)
            STAGE_GET_UINT      (iTakeStart)
            STAGE_GET_UINT      (iTakeAlive)
            STAGE_GET_UINT_ARRAY(aiType,   iNumTypes)
            STAGE_GET_VEC2_ARRAY(avVector, iNumEnemies)
        STAGE_GET_END

        STATIC_ASSERT(sizeof(coreUint32) * iNumTypes >= iNumEnemies)
        coreUint8* aiByteType = r_cast<coreUint8*>(aiType);

        const auto nTakeChild = [&](const coreUint8 iType)
        {
            for(coreUintW i = iTakeStart, ie = pSquad2->GetNumEnemies(); i < ie; ++i)
            {
                cEnemy* pCurrent = pSquad2->GetEnemy(i);

                if(HAS_FLAG(pCurrent->GetStatus(), ENEMY_STATUS_DEAD))
                {
                    iTakeStart = i + 1u;
                    iTakeAlive = iTakeAlive + 1u;

                    aiByteType[i] = iType;
                    avVector  [i] = coreVector2(0.0f,0.0f);

                    pCurrent->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    pCurrent->Resurrect();

                    return pCurrent;
                }
            }

            WARN_IF(true) {}
            return pSquad2->GetEnemy(0u);
        };

        const auto nDropShield = [&](const coreUint8 iType)
        {
            ASSERT(iType >= 4u)
            STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
            {
                if(aiByteType[i] == iType) pEnemy->Kill(true);
            });
        };

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 1u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 2u, 2u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 3u, 3u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 4u, 4u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 5u, 5u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 6u, 7u)

            if(pSquad1->IsFinished()) pSquad2->ClearEnemies(true);
        }

        if(m_fStageTime <= 3.0f)
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f))
            {
                cEnemy*         pChild = nTakeChild(0u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreFloat   fOff = ((s_iTick % 30u) < 15u) ? 1.0f : -1.0f;
                const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vAim = (g_pGame->FindPlayerDual(((s_iTick % 30u) < 15u) ? 0u : 1u)->GetPosition().xy() - vPos).Normalized();
                const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                avVector[iIndex] = vDir;
            }
        }
        else if(m_iStageSub == 4u || m_iStageSub == 5u)
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f))
            {
                cEnemy*         pChild = nTakeChild(0u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreFloat   fOff = g_pGame->FindPlayerDual((m_iStageSub == 4u) ? 0u : 1u)->GetPosition().y;
                const coreVector2 vPos = coreVector2(1.2f * FOREGROUND_AREA.x, fOff);

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                avVector[iIndex] = coreVector2(-1.0f,0.0f);
            }
        }
        else if(m_iStageSub == 7u)
        {
            if(STAGE_TICK_FREE(1.0f, 0.0f))
            {
                for(coreUintW j = 0u; j < 16u; ++j)
                {
                    cEnemy*         pChild = nTakeChild(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreFloat   fOff = (I_TO_F(j) - 7.5f) * 0.1375f;
                    const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    avVector[iIndex] = coreVector2(0.0f,-1.0f);
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, i ? 0.0f : 4.0f)

            const coreVector2 vFactor = coreVector2(1.0f, (i == 4u || i == 6u || i == 7u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2((i == 0u || i == 3u || i == 5u) ? 0.0f : ((i % 2u) ? 0.75f : -0.75f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i == 3u) pEnemy->Rotate90();

            if(i < 3u)
            {
                if(STAGE_TICK_LIFETIME(10.0f, 0.0f) && InBetween(s_iTick % 30u, 5u, 20u))
                {
                    cEnemy*         pChild = nTakeChild(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreVector2 vAim = pEnemy->AimAtPlayerDual(((s_iTick % 60u) < 30u) ? 0u : 1u).Normalized();   // TODO: coop, abwechslung
                    const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                    pChild->SetPosition(pEnemy->GetPosition());

                    avVector[iIndex] = vDir;
                }
            }
            else if(i == 5u)
            {
                if(STAGE_TICK_LIFETIME(2.0f, 0.0f) && ((s_iTick % 3u) < 2u))
                {
                    for(coreUintW j = 0u; j < 20u; ++j)
                    {
                        cEnemy*         pChild = nTakeChild(1u);
                        const coreUintW iIndex = pSquad2->GetIndex(pChild);

                        pChild->SetPosition(pEnemy->GetPosition());

                        avVector[iIndex].x = ((I_TO_F(j) / 20.0f) + CORE_MATH_PRECISION) * (2.0f*PI) * ((s_iTick % 3u) ? 1.0f : -1.0f);
                    }
                }
            }

            if(i >= 1u)
            {
                if(STAGE_TAKEOFF)
                {
                    for(coreUintW j = 0u; j < 10u; ++j)
                    {
                        cEnemy*         pChild = nTakeChild(i << 2u);
                        const coreUintW iIndex = pSquad2->GetIndex(pChild);

                        pChild->SetPosition(pEnemy->GetPosition());

                        avVector[iIndex].x = ((I_TO_F(j) / 10.0f) + CORE_MATH_PRECISION) * (2.0f*PI) * -1.0f;
                    }
                }

                if(pEnemy->ReachedDeath())
                {
                    nDropShield(i << 2u);
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUint8 iType = aiByteType[i];

            if(iType == 0u)   // bullet
            {
                const coreVector2 vNewPos = pEnemy->GetPosition().xy() + avVector[i] * (50.0f * TIME);
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(iType == 1u)   // wave
            {
                const coreVector2 vCenter = pEnemy->GetPosition().xy() - coreVector2::Direction(avVector[i].x) * avVector[i].y;

                avVector[i].x += 0.7f * TIME * SIGN(avVector[i].x);
                avVector[i].y += 40.0f * TIME;

                const coreVector2 vNewPos = vCenter + coreVector2::Direction(avVector[i].x) * avVector[i].y;
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(iType >= 4u)   // shield
            {
                const coreVector2 vCenter = pSquad1->GetEnemy(iType >> 2u)->GetPosition().xy();

                avVector[i].x += 1.0f * TIME * SIGN(avVector[i].x);
                avVector[i].y  = LERPB(0.0f, 12.0f, MIN(fLifeTime, 1.0f));

                const coreVector2 vNewPos = vCenter + coreVector2::Direction(avVector[i].x) * avVector[i].y;
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            const coreVector2 vNewDir = coreVector2::Direction(fLifeTime * 5.0f);
            pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

            if(iType == 0u)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                if((vPos.x < -FOREGROUND_AREA.x * 1.3f) ||
                   (vPos.x >  FOREGROUND_AREA.x * 1.3f) ||
                   (vPos.y < -FOREGROUND_AREA.y * 1.3f) ||
                   (vPos.y >  FOREGROUND_AREA.y * 1.3f))
                {
                    pEnemy->Kill(false);
                }
            }
            else if(iType == 1u)
            {
                if(avVector[i].y >= 100.0f)
                {
                    pEnemy->Kill(false);
                }
            }

            if(HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))
            {
                iTakeStart = MIN(iTakeStart, i);
                iTakeAlive = iTakeAlive - 1;
            }
        });

        STAGE_WAVE("EINUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_WAVE("ZWEIUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_WAVE("DREIUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 5u})
    //{
    //    STAGE_BOSS(m_Urtica, {60.0f, 120.0f, 180.0, 240.0f})
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
    // enemies form a tower and fall over
    // TODO: give central effect to highlight damaging touch   
    // TODO: check for and fix shadow artifacts, when jumping behind near clipping plane of shadow viewport (maybe fade out near plane)
    // TODO: enemies come from bottom out of sand (no movement)
    // add effect to highlight bottom enemies
    // if the last enemy is invincible, he should just die when starting to move
    // 
    // should not create bullets, to focus on the movement and and tower (bullets are too distracting)
    // should be fast enough to be a thread, but needs visual space to properly identify movement
    // complete vertical or horizontal movement is too easy
    // don't spawn too far on the side to properly blend them in, but not too near center, to highlight the tower structure
    // try to stack enemies all facing into the same direction, otherwise it looks not as fascinating
    // 4: stacking only two gives a lot of safe ways to kill them when they try to hit you
    // 1: show tower and flight mechanic
    // 2: show infinite displaced tower
    // 1: shoot away from second tower, to give spacial distance, and highlight the displacement mechanic
    // 3, 4, 5: left around, right around, left around
    // it is important that the flight start-direction can be properly identified
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        constexpr coreUintW iEnemies  = 40u;
        constexpr coreFloat fDistance = 0.4f;

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, iEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetAlpha (0.0f);
                pEnemy->Configure(1, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(iEnemies * 6u)
            STAGE_GET_VEC2_ARRAY (avMove,   iEnemies)
            STAGE_GET_VEC2_ARRAY (avRotate, iEnemies)
            STAGE_GET_FLOAT_ARRAY(afFall,   iEnemies)
            STAGE_GET_FLOAT_ARRAY(afHeight, iEnemies)
        STAGE_GET_END

        bool bClearedEarly = true;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if(STAGE_TAKEOFF)
            {
                afHeight[i] = 60.0f + 10.0f * I_TO_F(i);
            }

            const coreFloat fFloor = (((i == 0u) || !avMove[i-1u].IsNull()) ? 0.0f : (pSquad1->GetEnemy(i-1u)->GetPosition().z + 9.0f));

            afFall  [i] += -120.0f   * TIME;
            afHeight[i] += afFall[i] * TIME;

            if(afHeight[i] < fFloor)
            {
                afFall  [i] = MAX(afFall[i], 0.0f);
                afHeight[i] = fFloor;
            }

            if(avMove[i].IsNull() && pEnemy->ReachedDeath())
            {
                avMove  [i]   = pEnemy->GetDirection().xy();
                avRotate[i].x = pEnemy->GetDirection().xy().Angle();

                for(coreUintW j = i+1u, je = pSquad1->GetNumEnemies(); j < je; ++j)
                {
                    if(!afFall[j] || (j == i+1u))
                    {
                        afFall[j] = MIN(5.0f + 0.6f * afHeight[j], 100.0f);
                    }
                }

                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            }

            if(!avMove[i].IsNull())
            {
                coreVector2 vCurPos = pEnemy->GetPosition().xy();
                coreVector2 vCurDir = avMove[i];

                vCurPos += vCurDir * (70.0f * TIME);

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.0f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.0f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.0f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.0f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);

                pEnemy->SetPosition(coreVector3(vCurPos, afHeight[i]));
                avMove[i] = vCurDir;

                avRotate[i].y += 1.0f * TIME;
                pEnemy->DefaultRotate(avRotate[i].x + avRotate[i].y * (2.0f*PI));

                if(avRotate[i].y >= 3.0f) pEnemy->Kill(true);
            }
            else
            {
                coreVector2 vPos;
                coreVector2 vDir;

                if(i < 8u)
                {
                    vPos = coreVector2( 0.0f,1.0f) * fDistance;
                    vDir = coreVector2(-1.0f,1.0f).Normalized();
                }
                else if(i < 12u)
                {
                    vPos = coreVector2( 1.0f,1.0f) * fDistance;
                    vDir = coreVector2(-1.0f,1.0f).Normalized();
                }
                else if(i < 16u)
                {
                    vPos = coreVector2(-1.0f,1.0f) * fDistance;
                    vDir = coreVector2( 1.0f,1.0f).Normalized();
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
                else
                {
                    vPos = StepRotated90X((i - 32u) / 2u).Processed(SIGN) * fDistance;
                    vDir = pEnemy->AimAtPlayerSide().Normalized();
                }

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, afHeight[i]));
                pEnemy->SetDirection(coreVector3(vDir, 0.0f));

                bClearedEarly = false;
            }

            if(HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST) && !afHeight[i])
            {
                pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
            }

            const coreFloat fVisibility = CLAMP((54.0f - afHeight[i]) * 0.2f, pEnemy->GetAlpha() / 0.8f, 1.0f);
            const coreBool  bGhost      = HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST);

            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * fVisibility * (bGhost ? 1.0f : 1.3f));
            pEnemy->SetAlpha(fVisibility * (bGhost ? 0.8f : 1.0f));
        });

        if(bClearedEarly) pSquad1->ClearEnemies(true);

        STAGE_WAVE("SECHSUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 7u})
    {
        STAGE_WAVE("SIEBENUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // whac-a-mole
    // smoke can distract from bullets
    // non-linear movement is harder to track
    // shooting on "hide" instead of "show" is too overwhelming, identifying both initial bullet-direction and movement at the same time
    // 8 pattern is too overwhelming (order: 0, 3, 5, 2, 6, 1, 7, 4)
    // TODO: badge, richte großen schaden auf ein (einzelnes) non-ziel an
    // TODO: add effect (visual + sound) if the correct target was hit
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        constexpr coreUintW iNumEnemies = 6u;

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(1000, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(8u + 4u * iNumEnemies)
            STAGE_GET_UINT      (iVisible)
            STAGE_GET_UINT      (iBig)
            STAGE_GET_UINT      (iHit)
            STAGE_GET_UINT      (iPoints)
            STAGE_GET_UINT      (iIteration)
            STAGE_GET_UINT      (iSingle)
            STAGE_GET_FLOAT     (fTime,  fTime  = 1.0f;)
            STAGE_GET_FLOAT     (fDelay, fDelay = 1.0f;)
            STAGE_GET_VEC2_ARRAY(avPosFrom, iNumEnemies)
            STAGE_GET_VEC2_ARRAY(avPosTo,   iNumEnemies)
        STAGE_GET_END

        if(STAGE_BEGINNING)
        {
            avPosTo[0] = coreVector2( 0.0f,-1.2f);
            avPosTo[1] = coreVector2( 0.0f, 1.2f);
            avPosTo[2] = coreVector2(-1.2f, 0.0f);
            avPosTo[3] = coreVector2( 1.2f, 0.0f);
            avPosTo[4] = HIDDEN_POS;
            avPosTo[5] = HIDDEN_POS;
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

        fTime = MIN(fTime + 0.6f * TIME, 1.0f);
        if(fTime >= 1.0f)
        {
            iVisible = (iPoints < 14u) ? BITLINE(iNumEnemies) : BIT(iSingle);

            fDelay = MIN(fDelay + 0.9f * TIME, 1.0f);
            if(fDelay >= 1.0f || iHit)
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
                else if(iPoints == 8u)
                {
                    iBig = BITLINE(iNumEnemies);
                }
                else if(iPoints >= 14u)
                {
                    for(coreUintW j = 0u; j < iNumEnemies; ++j)
                    {
                        if(++iSingle >= iNumEnemies) iSingle = 0u;
                        if(!HAS_FLAG(pSquad1->GetEnemy(iSingle)->GetStatus(), ENEMY_STATUS_DEAD)) break;
                    }

                    iBig = BIT(iSingle);
                }

                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    const coreUintW iIndex = i + iIteration;

                    avPosFrom[i] = avPosTo[i];

                    if(iPoints < 4u)
                    {
                        coreUintW iStep;
                        switch(iIndex % 4u)
                        {
                        default: ASSERT(false)
                        case 0u: iStep = 0u; break;
                        case 1u: iStep = 3u; break;
                        case 2u: iStep = 1u; break;
                        case 3u: iStep = 2u; break;
                        }
                        avPosTo[i] = (i < 4u) ? StepRotated90(iStep) * 0.95f : HIDDEN_POS;
                    }
                    else if(iPoints < 14u)
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
                        avPosTo[i] = coreVector2::Direction(DEG_TO_RAD(I_TO_F(iStep) * 60.0f + 90.0f + CORE_MATH_PRECISION)) * 0.95f;
                    }
                    else
                    {
                        if(i == iSingle) avPosTo[i] = pEnemy->NearestPlayerDual(iIteration % 2u)->GetPosition().xy() / FOREGROUND_AREA;
                    }

                    if(avPosFrom[i] == HIDDEN_POS) avPosFrom[i] = avPosTo[i].Processed(SIGN) * 1.2f;
                });
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->SetPosition(coreVector3(LERP(avPosFrom[i], avPosTo[i], fTime) * FOREGROUND_AREA, 0.0f));

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

                        if(iPoints > 8u)
                        {
                            REMOVE_BIT(iBig, i)
                            if(!iBig) pEnemy->Kill(true);
                        }
                        if(iPoints == 14u)
                        {
                            iSingle = i;
                        }

                        g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_BLUE);
                    }
                }

                ADD_BIT(iHit, i)
            }

            if(!HAS_BIT(iVisible, i) && STAGE_TICK_FREE(30.0f, 0.0f))
            {
                                     g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 5.0f, 1u, COLOR_ENERGY_WHITE);
                if(HAS_BIT(iBig, i)) g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 5.0f, 1u, COLOR_ENERGY_BLUE);
            }

            if(HAS_BIT(iVisible, i) && !HAS_BIT(iPrevVisible, i))
            {
                pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

                if(g_pForeground->IsVisibleObject(pEnemy))
                {
                    if(iPoints >= 14u) nPunishFunc(pEnemy);
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
        });

        STAGE_WAVE("ACHTUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 11u})
    //{
    //    STAGE_BOSS(m_Tiger, {60.0f, 120.0f, 180.0, 240.0f})
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
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 12u})
    {
        STAGE_WAVE("EINUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // spikes from the ground and such
    // funken beim spawnen von gegner lenkt zu sehr ab
    // ring-wave, or cone-shaped from center sucked
    // trying to let player move from a soon-to-be-spike to a soon-to-be-no-spike feels shit and hard to use, the time difference needs to be greatly extended which reduces pacing, a jump like in mario is missing (to create player-induced short invincibility from spikes)
    // sägezahn pacing (schnell langsam schnell)
    // gegner spawnen in stacheln, dadurch wird mögliche kollision mit spieler vermieden
    // gegner spawning muss schönes muster und delay haben
    // erste zwei stacheln müssen langsam und in der mitte spawnen, damit spieler genau sieht was passiert
    // spieler muss genau sehn wo bald stacheln kommen und wo gefahr ist (einfärben der platten und stacheln)
    // TODO: change plate rendering
    // TODO: die symmetrische bewegung der stacheln in phase 3 ist schwer für die augen (2.0 -> 2.0)
    // TODO: maybe do a semi-transparent wave with the spike model when turning red
    STAGE_MAIN({TAKE_ALWAYS, 13u})
    {
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 34u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        STAGE_GET_START(37u)
            STAGE_GET_UINT       (iSpikeCount)
            STAGE_GET_FLOAT      (fSpikeDelay)
            STAGE_GET_FLOAT      (fEnemyDelay)
            STAGE_GET_FLOAT_ARRAY(afIntroTime, 34u)
        STAGE_GET_END

        const auto nToIndexFunc = [](const coreUintW iColumn, const coreUintW iRow)
        {
            ASSERT((iColumn < HARENA_SPIKE_DIMENSION) && (iRow < HARENA_SPIKE_DIMENSION))
            return iColumn + iRow * HARENA_SPIKE_DIMENSION;
        };

        const auto nToPositionFunc = [&](const coreUintW iColumn, const coreUintW iRow)
        {
            return (*m_SpikeBoard.List())[nToIndexFunc(iColumn, iRow)]->GetPosition().xy();
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) {STAGE_RESURRECT(pSquad1,  0u,  3u) fEnemyDelay = 0.0f;}
            else if(STAGE_SUB(2u)) {STAGE_RESURRECT(pSquad1,  4u,  9u) fEnemyDelay = 2.0f;}
            else if(STAGE_SUB(3u)) {STAGE_RESURRECT(pSquad1, 10u, 15u) fEnemyDelay = 5.0f;}
            else if(STAGE_SUB(4u)) {STAGE_RESURRECT(pSquad1, 16u, 21u) fEnemyDelay = 4.0f;}
            else if(STAGE_SUB(5u)) {STAGE_RESURRECT(pSquad1, 22u, 29u) fEnemyDelay = 5.0f;}
            else if(STAGE_SUB(6u)) {STAGE_RESURRECT(pSquad1, 30u, 33u) fEnemyDelay = 2.0f;}
            else if(STAGE_SUB(7u))
            {
                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                    this->DisableSpike(i, true);
            }

            iSpikeCount = 0u;
            fSpikeDelay = (m_iStageSub == 1u) ? 1.0f : 0.5f;
        }

        if(STAGE_BEGINNING)
        {
            for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                this->EnableSpike(i);
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
        else if(m_iStageSub == 5u)
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
        else if(m_iStageSub == 6u)
        {
            // spawn below player
        }

        if(m_iStageSub < 7u)
        {
            for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
            {
                coreObject3D* pBoard = (*m_SpikeBoard.List())[i];
                if(!pBoard->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

                const coreBool bIsActive = this->GetSpikeLaunched(i);
                const coreBool bIsQuiet  = (m_iStageSub == 6u) && !m_afSpikeMax[i];

                if(bIsActive || bIsQuiet)
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - pBoard->GetPosition().xy();

                        if((ABS(vDiff.x) < pBoard->GetCollisionRange().x) &&
                           (ABS(vDiff.y) < pBoard->GetCollisionRange().y))
                        {
                            if(bIsActive &&  pPlayer->IsNormal ()) pPlayer->TakeDamage(10u, ELEMENT_NEUTRAL, pPlayer->GetPosition().xy());
                            if(bIsQuiet  && !pPlayer->IsRolling()) this->LaunchSpike(i, g_pGame->GetCoop() ? 3.0f : 6.0f);
                        }
                    });
                }

                if(bIsActive && (m_fStageSubTime >= fEnemyDelay))
                {
                    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, j)
                    {
                        if(!HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST)) return;

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
            case 17u: iColumn = 2u; iRow = 1u; break;
            case 18u: iColumn = 4u; iRow = 1u; break;
            case 19u: iColumn = 1u; iRow = 3u; break;
            case 20u: iColumn = 3u; iRow = 3u; break;
            case 21u: iColumn = 5u; iRow = 3u; break;

            case 22u: iColumn = 1u; iRow = 0u; break;
            case 23u: iColumn = 5u; iRow = 0u; break;
            case 24u: iColumn = 1u; iRow = 4u; break;
            case 25u: iColumn = 5u; iRow = 4u; break;
            case 26u: iColumn = 0u; iRow = 1u; break;
            case 27u: iColumn = 4u; iRow = 1u; break;
            case 28u: iColumn = 0u; iRow = 5u; break;
            case 29u: iColumn = 4u; iRow = 5u; break;

            case 30u: iColumn = 5u; iRow = 0u; break;
            case 31u: iColumn = 1u; iRow = 1u; break;
            case 32u: iColumn = 4u; iRow = 3u; break;
            case 33u: iColumn = 2u; iRow = 5u; break;
            }

            pEnemy->SetPosition(coreVector3(nToPositionFunc(iColumn, iRow), 0.0f));

            if(HAS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
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
        });

        STAGE_WAVE("ZEIUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all spikes
    STAGE_MAIN({TAKE_ALWAYS, 13u})
    {
        for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
            this->DisableSpike(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        STAGE_WAVE("DREIUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 17u})
    //{
    //    STAGE_BOSS(m_Lucifer, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}