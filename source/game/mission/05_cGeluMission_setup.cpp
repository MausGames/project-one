///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"
#include <numeric>   // std::accumulate


// ****************************************************************
// setup the Gelu mission
void cGeluMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cVolcanoBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

        g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
        g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
        g_pEnvironment->SetTargetSpeedNow    (6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
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
    // push
    // bullets fly fast to give visual waves with space inbetween (but should not be shot too often to compensate)
    // in mill, player should not be able to pass when enemies are axis aligned
    // in 3 only five enemies / half the circle should peek out
    // in 2 lines should move over the center
    // in 1 enemies should not reach bottom
    // in 1 and 2 back of enemies should not be reachable without shooting at them first
    // TASK: hit specific enemies to reactivate them
    // TASK: collect all coins in the dungeon
    // ACHIEVEMENT: hit every enemy at least one time
    // TODO 1: hardmode: force is much stronger, and all enemies bounce at some point
    // TODO 1: hardmode: enemies get bigger, based on life or force
    // TODO 1: // 0b1101'1011u, // 0b1101'1011u, ?
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreFloat fDungeonFactor  = 0.095f;
        constexpr coreFloat fDungeonMax     = 660.0f;
        constexpr coreUintW iDungeonEnemies = 32u;

        STATIC_ASSERT(iDungeonEnemies == 32u)

        constexpr coreUint8 aiDungeon[] =
        {
            0u,
            0b1111'1111u,
            0u, 0u, 0u, 0u, 0u, 0u,
            0b0101'0101u,
            0u,
            0b1010'1010u,
            0u, 0u, 0u, 0u, 0u, 0u,
            0b0001'1000u,
            0u,
            0b1110'0111u,
            0u, 0u, 0u, 0u, 0u, 0u,
            0b0100'0000u,
            0b0010'0000u,
            0b0001'0000u,
            0b0000'1000u,
            0b0000'0100u,
            0b0000'0010u,
            0b0000'0001u,
            0u, 0u, 0u,
            0b1111'1000u,
            0u, 0u, 0u,
            0b0001'1111u,
            0u, 0u, 0u,
            0b1000'0000u,
            0b1000'0000u,
            0b1000'0000u,
            0b0000'0001u,
            0b0000'0001u,
            0b0000'0001u,
            0b1000'0000u,
            0b1000'0000u,
            0b1000'0000u,
            0b0000'0001u,
            0b0000'0001u,
            0b0000'0001u,
            0u, 0u,
            0b0001'1000u,
        };

        constexpr coreUint8 aiDungeonCoin[] =
        {
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
            0b0000'1000u,
            0u, 0u, 0u, 0u, 0u, 0u,
            0b0001'0000u,
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
            0b0000'0100u,
            0u, 0u, 0u, 0u,
            0b0010'0000u,
            0u, 0u, 0u, 0u, 0u,
            0b0000'1000u,
            0u, 0u, 0u,
            0b0001'0000u,
            0u, 0u, 0u, 0u, 0u, 0u, 0u,
            0b0000'0100u,
            0u, 0u, 0u, 0u, 0u,
            0b0010'0000u,
            0u, 0u, 0u
        };

        constexpr coreUintW iDungeonTotal = std::accumulate(aiDungeon, aiDungeon + ARRAY_SIZE(aiDungeon), 0u, [](const coreUintW A, const coreUint8 B) {return A + coreMath::PopCount(B);});

        STATIC_ASSERT(ARRAY_SIZE(aiDungeon) == ARRAY_SIZE(aiDungeonCoin))

        static constexpr coreUint8 s_aiMark[] = {10u, 18u, 27u, 32u, 44u, 66u, 76u, 80u, 88u};   // # needs to be static
        const auto nIsMarkFunc = [&](const coreUint8 iIndex)
        {
            return std::memchr(s_aiMark, iIndex, ARRAY_SIZE(s_aiMark));
        };
        const auto nGetMarkIndex = [&](const coreUint8 iIndex)
        {
            ASSERT(nIsMarkFunc(iIndex))
            return s_cast<const coreUint8*>(nIsMarkFunc(iIndex)) - s_aiMark;
        };

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.3f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2( 0.0f,0.0f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 96u + iDungeonEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.45f);
                pEnemy->Configure((i < 32u) ? 50 : ((i < 40u) ? 220 : ((i < 72u) ? 120 : ((i < 80u) ? 200 : ((i < 88u) ? 220 : ((i < 96u) ? 200 : 1000))))), 0u, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_WORTHLESS);

                pEnemy->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.1f);

                if(g_pGame->IsTask() && nIsMarkFunc(i)) pEnemy->SetBaseColor(COLOR_SHIP_BLACK, false, true);

                if(i < 40u)
                {
                    const coreVector2 vPos = (i < 32u) ? coreVector2((I_TO_F(i % 8u) - 3.5f) * 0.24f, (i < 8u) ? 1.3f : 1.54f) : (coreVector2(0.0f,1.74f) + 0.32f * coreVector2::Direction(I_TO_F(i % 8u) * (0.25f*PI)));
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                    pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir,                   0.0f));

                         if(i >= 16u && i < 24u) pEnemy->Rotate270();
                    else if(i >= 24u && i < 32u) pEnemy->Rotate90();
                }
                else if(i < 72u)
                {
                    coreFloat fOffset;
                    switch((i - 40u) / 8u)
                    {
                    default: ASSERT(false)
                    case 0u: fOffset =  0.5f; break;
                    case 1u: fOffset = -0.5f; break;
                    case 2u: fOffset =  0.5f; break;
                    case 3u: fOffset =  0.0f; break;
                    }

                    const coreVector2 vDir = StepRotated90((i - 40u) / 8u);
                    const coreVector2 vPos = vDir.Rotated90() * fOffset - vDir * (I_TO_F((i - 40u) % 8u) * 0.24f + 1.54f);

                    pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
                }
                else if(i < 80u)
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(i - 72u) * (0.25f*PI) + (0.125f*PI));
                    const coreVector2 vPos = vDir * -1.2f * SQRT2;

                    pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
                }
            });
        });

        STAGE_GET_START(181u)
            STAGE_GET_VEC2_ARRAY(avForce,     80u, for(coreUintW i = 0u; i < 80u; ++i) avForce[i] = ((i < 72u) ? 85.0f : 100.0f) * pSquad1->GetEnemy(i)->GetDirection().xy();)
            STAGE_GET_VEC2_ARRAY(avBouncePos, 2u,  avBouncePos[0] = 1.2f * SQRT2 * FOREGROUND_AREA;        avBouncePos[1] = avBouncePos[0].InvertedX();)
            STAGE_GET_VEC2_ARRAY(avBounceDir, 2u,  avBounceDir[0] = coreVector2(-1.5f,-1.0f).Normalized(); avBounceDir[1] = coreVector2(1.0f,-1.5f).Normalized();)
            STAGE_GET_FLOAT     (fBounceForce,     fBounceForce   = 200.0f;)
            STAGE_GET_FLOAT     (fMillAngle,       fMillAngle     = 0.5f*PI)
            STAGE_GET_FLOAT     (fMillForce)
            STAGE_GET_UINT      (iDragActive)
            STAGE_GET_UINT      (iDragActiveCoin)
            STAGE_GET_FLOAT     (fDragOffset)
            STAGE_GET_FLOAT     (fDragForce,       fDragForce     = 100.0f;)
            STAGE_GET_UINT      (iCoinState)
            STAGE_GET_UINT      (iMarkState)
            STAGE_GET_UINT      (iMarkCount)
            STAGE_GET_UINT      (iHitField)
            STAGE_GET_UINT      (iHitCount)
            STAGE_GET_UINT      (iShakeState)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= sizeof(iHitField)*8u)

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS, COLL_VAL(pSquad1), COLL_VAL(avForce), COLL_REF(fBounceForce), COLL_REF(fMillForce), COLL_REF(fDragForce), COLL_REF(iMarkState), COLL_REF(iMarkCount), COLL_REF(iHitField), COLL_REF(iHitCount), COLL_VAL(nIsMarkFunc), COLL_VAL(nGetMarkIndex))
        {
            if(!bFirstHit) return;

            const coreUintW i = pSquad1->GetIndex(pEnemy);

            coreUint8 iGroupNum = 0u;
                 if(i < 16u) iGroupNum = 2u;
            else if(i < 32u) iGroupNum = 4u;
            else if(i < 96u) iGroupNum = 8u;
            else             iGroupNum = 32u;

            const coreFloat fPower = I_TO_F(pBullet->GetDamage()) * RCP(I_TO_F(g_pGame->GetNumPlayers()));

            coreVector2 vForceDir = coreVector2(0.0f,0.0f);
                 if(i < 16u) vForceDir     = fPower * 6.0f * coreVector2( 0.0f,-1.0f);
            else if(i < 24u) vForceDir     = fPower * 6.0f * coreVector2( 1.0f, 0.0f);
            else if(i < 32u) vForceDir     = fPower * 6.0f * coreVector2(-1.0f, 0.0f);
            else if(i < 40u) vForceDir     = fPower * 5.5f * AlongCrossNormal(-pBullet->GetFlyDir());
            else if(i < 72u) vForceDir     = fPower * 6.0f * StepRotated90((i - 40u) / 8u);
            else if(i < 80u) vForceDir.x   = fPower * 2.5f;
            else if(i < 88u) fBounceForce += fPower * 7.5f;
            else if(i < 96u) fMillForce   -= fPower * 0.4f;
            else             fDragForce   += fPower * 6.0f;

            for(coreUintW j = coreMath::FloorAlign(i, iGroupNum), je = coreMath::CeilAlign(i + 1u, iGroupNum); j < je; ++j)
            {
                     if(i < 72u) avForce[j] += vForceDir;
                else if(i < 80u) avForce[j] += vForceDir.x * coreVector2::Direction(I_TO_F(j - 72u) * (0.25f*PI));

                pSquad1->GetEnemy(j)->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()), false);
            }

            pBullet->Deactivate(true);
            pBullet->AddStatus(BULLET_STATUS_GHOST);
            g_pGame->PlayHitSound(vIntersection);

            if(pEnemy->ReachedDeath())
            {
                this->AddExtraScore(pEnemy->LastAttacker(), 100u * iGroupNum, pEnemy->GetPosition());
            }

            if(!HAS_BIT(iHitField, i % (sizeof(iHitField)*8u)))
            {
                ADD_BIT(iHitField, i % (sizeof(iHitField)*8u))
                if(++iHitCount >= 96u + /*iDungeonTotal*/55u) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())   // TODO 1: MSVC wants to capture this constexpr (async lambda)
            }

            if(g_pGame->IsTask() && nIsMarkFunc(i) && !HAS_BIT(iMarkState, nGetMarkIndex(i)))
            {
                ADD_BIT(iMarkState, nGetMarkIndex(i))

                pEnemy->SetBaseColor(COLOR_SHIP_RED);

                if(++iMarkCount >= ARRAY_SIZE(s_aiMark))
                {
                    STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                }
                else
                {
                    g_pGame->GetCombatText()->DrawProgress(iMarkCount, ARRAY_SIZE(s_aiMark), pEnemy->GetPosition());
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iMarkCount, ARRAY_SIZE(s_aiMark)), SOUND_ITEM_02);
                }

                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
                g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_DUST);
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 15u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 16u, 31u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 32u, 39u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 40u, 71u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 72u, 79u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 88u, 95u)   // #
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 80u, 87u)   // #
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1, 96u, 96u + iDungeonEnemies - 1u)
            else if(STAGE_SUB(9u))
            {
                constexpr coreUintW iMargin = 4u;
                if(iHitCount >= 96u + iDungeonTotal - iMargin) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
            }

            iHitField = 0u;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW);

        if((m_iStageSub == 3u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
        }

        if((m_iStageSub == 7u) && fBounceForce)
        {
            for(coreUintW i = 0u; i < 2u; ++i)
            {
                coreVector2& vPos = avBouncePos[i];
                coreVector2& vDir = avBounceDir[i];

                vPos += vDir * (fBounceForce * TIME);

                     if((vPos.x < -FOREGROUND_AREA.x * 0.88f) && (vDir.x < 0.0f)) {vPos.x -= 2.0f * (vPos.x + FOREGROUND_AREA.x * 0.88f); vDir.x =  ABS(vDir.x);}
                else if((vPos.x >  FOREGROUND_AREA.x * 0.88f) && (vDir.x > 0.0f)) {vPos.x -= 2.0f * (vPos.x - FOREGROUND_AREA.x * 0.88f); vDir.x = -ABS(vDir.x);}
                     if((vPos.y < -FOREGROUND_AREA.y * 0.88f) && (vDir.y < 0.0f)) {vPos.y -= 2.0f * (vPos.y + FOREGROUND_AREA.y * 0.88f); vDir.y =  ABS(vDir.y);}
                else if((vPos.y >  FOREGROUND_AREA.y * 0.88f) && (vDir.y > 0.0f)) {vPos.y -= 2.0f * (vPos.y - FOREGROUND_AREA.y * 0.88f); vDir.y = -ABS(vDir.y);}
            }

            fBounceForce *= FrictionFactor(3.0f);
        }
        else if((m_iStageSub == 6u) && fMillForce)
        {
            fMillAngle += fMillForce * TIME;
            fMillForce *= FrictionFactor(6.0f);
        }
        else if((m_iStageSub == 8u) && fDragForce)
        {
            const coreFloat fOldOffset = fDragOffset;

            fDragOffset += fDragForce * TIME;
            fDragForce  *= FrictionFactor(4.0f);

            const coreUintW iFrom = F_TO_UI(fOldOffset  * fDungeonFactor);
            const coreUintW iTo   = F_TO_UI(fDragOffset * fDungeonFactor);

            for(coreUintW i = iFrom; i < iTo; ++i)
            {
                const coreUintW iEntry     = ARRAY_SIZE(aiDungeon) - 1u - MIN(i, ARRAY_SIZE(aiDungeon) - 1u);
                const coreUint8 iLevel     = aiDungeon    [iEntry];
                const coreUint8 iLevelCoin = aiDungeonCoin[iEntry];

                for(coreUintW j = 0u; j < 8u; ++j)
                {
                    const coreVector2 vPos  = coreVector2((I_TO_F(j) - 3.5f) * -0.27f, 1.3f);
                    const coreVector2 vCorr = coreVector2(0.0f, (I_TO_F(i + 1u) / fDungeonFactor - fDragOffset) / FOREGROUND_AREA.y);

                    if(HAS_BIT(iLevel, j))
                    {
                        const coreUintW iIndex = coreMath::BitScanFwd(~coreUint64(iDragActive));
                        ASSERT(iIndex < iDungeonEnemies)

                        cEnemy* pEnemy = pSquad1->GetEnemy(96u + iIndex);
                        ADD_BIT(iDragActive, iIndex)

                        pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA + vCorr, 0.0f));
                        pEnemy->RemoveStatus(ENEMY_STATUS_SKIPEXPLOSION);

                        REMOVE_BIT(iHitField, iIndex % (sizeof(iHitField)*8u))
                    }

                    if(HAS_BIT(iLevelCoin, j))
                    {
                        if(g_pGame->IsTask())
                        {
                            const coreUintW iIndex = coreMath::BitScanFwd(~coreUint64(iDragActiveCoin));
                            ASSERT(iIndex < GELU_COINS)

                            this->EnableCoin(iIndex);
                            ADD_BIT(iDragActiveCoin, iIndex)

                            this->GetCoin(iIndex)->SetPosition(coreVector3(vPos * FOREGROUND_AREA + vCorr, 0.0f));
                        }
                    }
                }
            }
        }

        const coreVector2 vMillDir = coreVector2::Direction(fMillAngle);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.4f, 0.0f)

            if(i < 80u)
            {
                coreVector2 vNewPos = pEnemy->GetPosition().xy();

                if(!avForce[i].IsNull())
                {
                    vNewPos    += avForce[i] * TIME;
                    avForce[i] *= FrictionFactor(3.0f);
                }

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(i < 88u)
            {
                const coreVector2 vNewPos = avBouncePos[(i < 84u) ? 0u : 1u] + StepRotated90X((i - 80u) % 4u) * 0.17f * FOREGROUND_AREA;

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(i < 96u)
            {
                const coreFloat   fOffset = (fLifeTime < pPath1->GetTotalDistance()) ? (pPath1->CalcPosition(fLifeTime).x * ((i < 92u) ? 1.0f : -1.0f)) : 0.0f;
                const coreVector2 vNewPos = vMillDir * ((I_TO_F((i - 88u) % 8u) - 3.5f) * 0.26f + fOffset) * FOREGROUND_AREA;

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(i < 96u + iDungeonEnemies)
            {
                if(HAS_BIT(iDragActive, i - 96u))
                {
                    coreVector2 vNewPos = pEnemy->GetPosition().xy() + coreVector2(0.0f, -fDragForce * TIME);

                    if(fDragOffset >= fDungeonMax)
                    {
                        if(!iShakeState && (vNewPos.y < 0.0f))
                        {
                            iShakeState = 1u;

                            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
                            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
                        }

                        vNewPos.y = MAX(vNewPos.y, 0.0f);
                    }

                    if(vNewPos.y < -1.3f * FOREGROUND_AREA.y)
                    {
                        REMOVE_BIT(iDragActive, i - 96u)
                        pEnemy->AddStatus(ENEMY_STATUS_SKIPEXPLOSION);
                    }

                    pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
                }
            }

                 if(i == 72u) g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP((-2.0f/4.0f)*PI, (-0.0f/4.0f)*PI, pEnemy->GetCurHealthPct())), 1.0f);
            else if(i == 88u) g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP((-4.0f/4.0f)*PI, (-2.0f/4.0f)*PI, pEnemy->GetCurHealthPct())), 1.0f);
            else if(i == 80u) g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP((-6.0f/4.0f)*PI, (-4.0f/4.0f)*PI, pEnemy->GetCurHealthPct())), 1.0f);

            const coreUintW iDiv = g_pGame->IsEasy() ? 2u : 1u;

            if(STAGE_TICK_LIFETIME(0.75f * (0.8f / fLifeSpeed), 0.0f) && !(s_iTick % iDiv))
            {
                if(!((i >= 40u && i < 72u) || (i >= 96u)) || g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy(), 1.25f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual((s_iTick / iDiv + m_iStageSub - 1u) % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.3f, pEnemy, vPos, vDir)->ChangeSize(1.6f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_YELLOW);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(g_pGame->IsTask() && nIsMarkFunc(i))
            {
                if(!HAS_BIT(iMarkState, nGetMarkIndex(i)))
                {
                    d_cast<cCinderEnemy*>(pEnemy)->SetAngle(0.0f*PI);
                }
                else
                {
                    ASSERT((i >= 1u) && (i < pSquad1->GetNumEnemies()))

                    const cEnemy* pPrev   = pSquad1->GetEnemy(i - 1u);
                    const cEnemy* pNext   = pSquad1->GetEnemy(i + 1u);
                    const cEnemy* pLeader = pNext->HasStatus(ENEMY_STATUS_DEAD) ? pPrev : pNext;

                    d_cast<cCinderEnemy*>(pEnemy)->SetAngle(d_cast<const cCinderEnemy*>(pLeader)->GetAngle());
                }
            }
        });

        if(g_pGame->IsTask())
        {
            for(coreUintW i = 0u; i < GELU_COINS; ++i)
            {
                coreObject3D* pCoin = this->GetCoin(i);
                if(!pCoin->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreVector2 vNewPos = pCoin->GetPosition().xy() + coreVector2(0.0f, -fDragForce * TIME);

                if(vNewPos.y < -1.3f * FOREGROUND_AREA.y)
                {
                    this->DisableCoin(i, false);
                    REMOVE_BIT(iDragActiveCoin, i)
                }

                pCoin->SetPosition(coreVector3(vNewPos, 0.0f));

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pCoin->GetPosition().xy() - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(5.0f))
                    {
                        this->DisableCoin(i, true);
                        REMOVE_BIT(iDragActiveCoin, i)

                        if(++iCoinState >= 8u)
                        {
                            STAGE_BADGE(1u, BADGE_NORMAL, pCoin->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawCountdown(iCoinState, 8u, pCoin->GetPosition());
                            g_pSpecialEffects->PlaySound(pCoin->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iCoinState, 8u), SOUND_ITEM_01);
                        }
                    }
                });
            }
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            constexpr coreUintW iIndex = 32u;

            coreVector2 vPos = coreVector2(0.0f,0.0f);
            for(coreUintW i = 0u; i < 8u; ++i)
            {
                vPos += pSquad1->GetEnemy(iIndex + i)->GetPosition().xy() / 8.0f;
            }

            // TODO 1: clean this all up here      
            {
            const auto nBounceFunc = [&]()
            {
                for(coreUintW i = 1u; i < 8u; ++i)
                {
                    //vPos += pSquad1->GetEnemy(iIndex + i)->GetPosition().xy() / 8.0f;
                    avForce[iIndex + i] = avForce[iIndex];
                }
            };

                 if((vPos.x < -FOREGROUND_AREA.x * 1.1f) && (avForce[iIndex].x < 0.0f)) {vPos.x -= 2.0f * (vPos.x + FOREGROUND_AREA.x * 1.1f); avForce[iIndex].x =  ABS(avForce[iIndex].x); nBounceFunc(); for(coreUintW i = 0u; i < 8u; ++i) {coreVector3 A = pSquad1->GetEnemy(iIndex + i)->GetPosition(); A.x -= 2.0f * (A.x + FOREGROUND_AREA.x * 1.1f); pSquad1->GetEnemy(iIndex + i)->SetPosition(A);}}
            else if((vPos.x >  FOREGROUND_AREA.x * 1.1f) && (avForce[iIndex].x > 0.0f)) {vPos.x -= 2.0f * (vPos.x - FOREGROUND_AREA.x * 1.1f); avForce[iIndex].x = -ABS(avForce[iIndex].x); nBounceFunc(); for(coreUintW i = 0u; i < 8u; ++i) {coreVector3 A = pSquad1->GetEnemy(iIndex + i)->GetPosition(); A.x -= 2.0f * (A.x - FOREGROUND_AREA.x * 1.1f); pSquad1->GetEnemy(iIndex + i)->SetPosition(A);}}
                 if((vPos.y < -FOREGROUND_AREA.y * 1.1f) && (avForce[iIndex].y < 0.0f)) {vPos.y -= 2.0f * (vPos.y + FOREGROUND_AREA.y * 1.1f); avForce[iIndex].y =  ABS(avForce[iIndex].y); nBounceFunc(); for(coreUintW i = 0u; i < 8u; ++i) {coreVector3 A = pSquad1->GetEnemy(iIndex + i)->GetPosition(); A.y -= 2.0f * (A.y + FOREGROUND_AREA.y * 1.1f); pSquad1->GetEnemy(iIndex + i)->SetPosition(A);}}
            else if((vPos.y >  FOREGROUND_AREA.y * 1.1f) && (avForce[iIndex].y > 0.0f)) {vPos.y -= 2.0f * (vPos.y - FOREGROUND_AREA.y * 1.1f); avForce[iIndex].y = -ABS(avForce[iIndex].y); nBounceFunc(); for(coreUintW i = 0u; i < 8u; ++i) {coreVector3 A = pSquad1->GetEnemy(iIndex + i)->GetPosition(); A.y -= 2.0f * (A.y - FOREGROUND_AREA.y * 1.1f); pSquad1->GetEnemy(iIndex + i)->SetPosition(A);}}
            }

            pHelper->SetPosition(coreVector3(vPos, 0.0f));

            if(pSquad1->GetEnemy(iIndex)->ReachedDeath())
            {
                pHelper->Kill(true);
            }
        }

        STAGE_WAVE(0u, "5-1", {50.0f, 75.0f, 100.0f, 125.0f, 250.0f})   // FÜNFUNDZWANZIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < GELU_COINS; ++i)
            this->DisableCoin(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(-1.0f,0.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // the walls are coming at you
    // [deprecated] 1) erste sub-gruppe muss (schnell) still stehen, damit der spieler die wände bewegen sieht
    // [deprecated] 2) weitere flache sub-gruppen um mechanik stärker begreifbar zu machen, e.g. dass alle wände betroffen sind
    // [deprecated] gleich nach erste gruppe spikes erzeugen kommt schnell zum geschäft nachdem mechanik gezeigt wurde, geht gut
    // gegner durch wand-verschiebung zu verstecken generiert nervige pausen, besser die gegner flugbahn mitverschieben
    // wenn 2 wände aufeinander zukommen, sollten gegner von nicht den tunnel entlang fliegen, das macht es zu leicht
    // wenn wände auf spieler zukommen müssen flache gegner-gruppen (ohne bewegung) darüber stehen, damit spieler drauf schießt und sieht, dass er sie zurückdrängen kann
    // am ende müssen wände vom spieler zurückgedrückt und eingerastet werden
    // beim zusammendrücken sollten immer nur zwei wände betroffen sein, vier sind zu viel (außer im finale)
    // gegner sollten nicht von mehr als 2 richtungen gleichzeitig kommen (außer eck-gruppe), sonst dreht sich spieler verwirrt im kreis
    // [deprecated] delay bevor wände sich zurückziehen ist schwer kontrollierbar und begreifbar
    // gruppe in den ecken zeigt das enemy-anchoring sehr gut und schaut lustig aus, sub-gruppe für speed-runner die sich die wände absichtlich herziehen
    // in der finalen phase lassen profis die mauern näher kommen um durch kurze wege schneller töten zu können
    // do not render this above water (wall-bullets and enemies reflect and pop in)
    // erste gruppe zieht spieler an die richtige position
    // erste box-bewegung muss abseits sein, damit eine bewegung entsteht bei egal welcher start-position
    // enemies are ghost+hidden, to prevent them from getting shot and rendering shadow, if they start in middle but are behind wall
    // gegner sieht man orsch unter den stacheln (müssen groß, andersfarbig und weit genug weg sein von wand)
    // TASK: destroy specific enemies first
    // TASK: attack walls in certain order during the box phase
    // TASK EXTRA: find and destroy all hidden enemies
    // ACHIEVEMENT: never hit the walls / never miss any shot
    // TODO 1: hard mode: attacking the border creates attacks (stings fly away, and respawn a second later ?)
    // TODO 1: etwas muss blinken oder reagieren bei treffern (e.g. die stacheln ?, eine unsichtbare linie am rand (im spielfield))
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreFloat fOffMin = 0.0f;
        constexpr coreFloat fOffMax = 1.0f;

        const coreUint8 aiBoxOrder[] = {2u, 0u, 3u, 0u, 2u, 1u, 3u, 1u, 2u, 3u, 0u, 1u, 0u, 2u, 3u, UINT8_MAX};   // TODO 1: nAdvanceOrderFunc cannot access aiBoxOrder

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
            pPath2->AddNode(coreVector2(1.3f,1.3f), coreVector2(-1.0f,-1.0f).Normalized());
            pPath2->AddStop(coreVector2(0.9f,0.9f), coreVector2(-1.0f,-1.0f).Normalized());
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f, 1.0f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,-1.0f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 78u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREEN);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cFreezerEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(POST_WALLS * 2u + 9u)
            STAGE_GET_FLOAT_ARRAY(afOffCurrent, POST_WALLS)
            STAGE_GET_FLOAT_ARRAY(afOffTarget,  POST_WALLS)
            STAGE_GET_FLOAT      (fRotation)
            STAGE_GET_FLOAT      (fRotationBreak)
            STAGE_GET_UINT       (iWallBullets)
            STAGE_GET_UINT       (iOrderState)
            STAGE_GET_UINT       (iOrderWall, iOrderWall  = UINT8_MAX)
            STAGE_GET_UINT       (iHiddenState)
            STAGE_GET_UINT       (iFirstState)
            STAGE_GET_UINT       (iActive, iActive = BITLINE(POST_WALLS))
            STAGE_GET_UINT       (iOutsideHit)
        STAGE_GET_END

        const auto nFlashFunc = [](const coreUint8 iTarget, const coreBool bRumble = true)
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](const cBullet* pBullet)
            {
                const coreUintW iWall = pBullet->GetDamage() - 1;
                ASSERT(iWall < POST_WALLS)

                if(HAS_BIT(iTarget, iWall)) g_pSpecialEffects->CreateSplashColor(pBullet->GetPosition() + pBullet->GetDirection() * 1.5f, 5.0f, 3u, COLOR_ENERGY_ORANGE);
            });

            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            if(bRumble) g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
        };

        const auto nHighlightFunc = [](const coreUint8 iOld, const coreUint8 iNew)
        {
            ASSERT(iOld != iNew)

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([&](cConeBullet* OUTPUT pBullet)
            {
                const coreUintW iWall = pBullet->GetDamage() - 1;
                ASSERT(iWall < POST_WALLS)

                     if(iWall == iOld) pBullet->MakeOrange();
                else if(iWall == iNew) pBullet->MakeBlue  ();
            });
        };

        const auto nAdvanceOrderFunc = [&]()
        {
            ASSERT(iOrderState < ARRAY_SIZE(aiBoxOrder))

            if(iOrderWall < POST_WALLS) nFlashFunc(BIT(iOrderWall), false);

            nHighlightFunc(iOrderWall, aiBoxOrder[iOrderState]);
            iOrderWall = aiBoxOrder[iOrderState];
        };

        const auto nShakeFunc = [](const coreBool bRumble = true)
        {
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
            if(bRumble) g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
        };

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  6u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  7u, 15u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 16u, 22u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 23u, 25u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 26u, 28u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 29u, 31u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 32u, 34u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 35u, 37u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 38u, 40u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 41u, 41u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 42u, 51u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 52u, 61u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 62u, 62u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 63u, 63u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 64u, 64u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 65u, 65u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 66u, 66u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 67u, 67u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 68u, 68u)
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 69u, 69u)
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad1, 70u, 77u)
            else if(STAGE_SUB(22u)) STAGE_DELAY_START

            if(m_iStageSub == 4u)
            {
                m_Chol.ResurrectIntro(0u);
            }

            if(g_pGame->IsTaskExtra())
            {
                     if(m_iStageSub == 13u) STAGE_RESURRECT(pSquad2, 0u, 1u)
                else if(m_iStageSub == 22u) pSquad2->ClearEnemies(true);
            }

            coreUint8 iFlash = 0x00u;
            switch(m_iStageSub)
            {
            case 11u: iFlash = BIT(0u) + BIT(1u); break;
            case 13u: iFlash = BIT(2u) + BIT(3u); break;
            case 21u: iFlash = 0xFFu;             break;
            }

            if(iFlash)
            {
                for(coreUintW i = 0u; i < POST_WALLS; ++i) afOffTarget[i] = MAX(afOffTarget[i], fOffMin);
                nFlashFunc(iFlash);
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_ORANGE);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect(false);
        }
        if(STAGE_TIME_BEFORE(2.0f))
        {
            pHelper->SetPosition(coreVector3(m_fStageTime * 1.55f - 1.2f, 0.95f, 0.0f) * FOREGROUND_AREA3);
        }
        else if(STAGE_TIME_POINT(2.0f))
        {
            pHelper->Kill(false);
        }

        //   3
        // 0   1
        //   2

        const coreUint8 iDefend = (m_iStageSub >= 21u) ? 3u : ((m_iStageSub >= 13u) ? 2u : ((m_iStageSub >= 11u) ? 1u : 0u));
        const coreBool  bDelay  = pSquad1->IsFinished();

        coreVector2 vOffFrom  = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[0], afOffCurrent[2]);
        coreVector2 vOffTo    = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[1], afOffCurrent[3]);
        coreVector2 vAreaFrom = -FOREGROUND_AREA + vOffFrom;
        coreVector2 vAreaTo   =  FOREGROUND_AREA - vOffTo;

        if(m_iStageSub < 11u)
        {
            const coreFloat fBase = g_pGame->IsEasy() ? 0.25f : 0.3f;

            if(m_iStageSub < 2u)
            {
            }
            else if(m_iStageSub == 2u)
            {
                afOffTarget[0] = LERPBR(0.0f, fBase * 2.0f, CLAMP(m_fStageSubTime / 1.5f, 0.0f, 1.0f));

                if(STAGE_SUBTIME_POINT(0.0f)) nFlashFunc(BIT(0u));
                if(STAGE_SUBTIME_POINT(1.5f)) nShakeFunc();
            }
            else if(m_iStageSub == 3u)
            {
                afOffTarget[0] = LERPBR(fBase * 2.0f, 0.0f,         CLAMP((m_fStageSubTime),               0.0f, 1.0f));
                afOffTarget[1] = LERPBR(0.0f,         fBase * 2.0f, CLAMP((m_fStageSubTime - 1.0f) / 1.5f, 0.0f, 1.0f));

                if(STAGE_SUBTIME_POINT(1.0f)) nFlashFunc(BIT(1u));
                if(STAGE_SUBTIME_POINT(1.0f) || STAGE_SUBTIME_POINT(2.5f)) nShakeFunc();
            }
            else if(m_iStageSub == 4u)
            {
                afOffTarget[1] = LERPBR(fBase * 2.0f, 0.0f,         CLAMP((m_fStageSubTime),               0.0f, 1.0f));
                afOffTarget[2] = LERPBR(0.0f,         fBase * 1.0f, CLAMP((m_fStageSubTime - 1.0f) / 1.5f, 0.0f, 1.0f));
                afOffTarget[3] = afOffTarget[2];

                if(STAGE_SUBTIME_POINT(1.0f)) nFlashFunc(BIT(2u) + BIT(3u));
                if(STAGE_SUBTIME_POINT(1.0f) || STAGE_SUBTIME_POINT(2.5f)) nShakeFunc();
            }
            else
            {
                if(m_iStageSub == 10u) fRotationBreak += (1.0f/1.5f) * TIME;
                fRotation += (1.0f - MIN1(fRotationBreak)) * TIME;

                const coreFloat fOffset = LERPH3(-1.0f, 1.0f, TriangleWave(fRotation * 0.2f)) * BLENDH3(MIN1(fRotation * 0.1f));

                afOffTarget[2] = fBase * (1.0f + fOffset);
                afOffTarget[3] = fBase * (1.0f - fOffset);

                if(m_iStageSub == 10u)
                {
                    if(STAGE_SUBTIME_POINT(1.5f)) nFlashFunc(BIT(0u) + BIT(1u));
                    if(STAGE_SUBTIME_POINT(3.0f)) nShakeFunc();

                    if(g_pGame->IsTask())
                    {
                        if(STAGE_SUBTIME_POINT(3.0f))
                        {
                            nAdvanceOrderFunc();
                        }
                        if(STAGE_SUBTIME_POINT(12.0f))
                        {
                            if(iOrderState < ARRAY_SIZE(aiBoxOrder) - 1u)
                            {
                                iOrderState = ARRAY_SIZE(aiBoxOrder) - 1u;
                                nAdvanceOrderFunc();
                            }
                        }
                    }

                    if(STAGE_SUBTIME_BEFORE(1.5f))
                    {
                    }
                    else if(STAGE_SUBTIME_BEFORE(3.0f))
                    {
                        afOffTarget[0] = LERPBR(0.0f, fBase, CLAMP((m_fStageSubTime - 1.5f) / 1.5f, 0.0f, 1.0f));
                        afOffTarget[1] = afOffTarget[0];
                    }
                    else if(STAGE_SUBTIME_BEFORE(12.0f))
                    {
                        const coreFloat fTime = m_fStageSubTime - 3.0f;

                        coreVector2 vFrom, vTo;
                        coreFloat   fLerp;

                             if(fTime < 1.5f) {vFrom = coreVector2( 0.0f, fOffset * (fBase/0.2f)); vTo = coreVector2( 1.0f, 0.0f); fLerp = STEP(0.0f, 1.5f, fTime);}
                        else if(fTime < 3.0f) {vFrom = coreVector2( 1.0f, 0.0f);                   vTo = coreVector2(-1.0f,-1.0f); fLerp = STEP(1.5f, 3.0f, fTime);}
                        else if(fTime < 4.5f) {vFrom = coreVector2(-1.0f,-1.0f);                   vTo = coreVector2( 0.0f, 1.0f); fLerp = STEP(3.0f, 4.5f, fTime);}
                        else if(fTime < 6.0f) {vFrom = coreVector2( 0.0f, 1.0f);                   vTo = coreVector2( 1.0f,-1.0f); fLerp = STEP(4.5f, 6.0f, fTime);}
                        else if(fTime < 7.5f) {vFrom = coreVector2( 1.0f,-1.0f);                   vTo = coreVector2(-1.0f, 0.0f); fLerp = STEP(6.0f, 7.5f, fTime);}
                        else                  {vFrom = coreVector2(-1.0f, 0.0f);                   vTo = coreVector2( 0.0f, 0.0f); fLerp = STEP(7.5f, 9.0f, fTime);}

                        const coreFloat   fNarrow = LERP(0.0f, 0.1f, STEP(0.0f, 9.0f, fTime));
                        const coreVector2 vTarget = LERPS(vFrom, vTo, fLerp) * (0.2f + fNarrow * 0.5f);

                        afOffTarget[0] = fBase + fNarrow + vTarget.x;
                        afOffTarget[1] = fBase + fNarrow - vTarget.x;
                        afOffTarget[2] = fBase + fNarrow + vTarget.y;
                        afOffTarget[3] = fBase + fNarrow - vTarget.y;
                    }
                    else
                    {
                        afOffTarget[0] = LERPBR(fBase + 0.1f, 0.0f, CLAMP(m_fStageSubTime - 12.0f, 0.0f, 1.0f));
                        afOffTarget[1] = afOffTarget[0];
                        afOffTarget[2] = afOffTarget[0];
                        afOffTarget[3] = afOffTarget[0];

                        if(STAGE_SUBTIME_POINT(13.0f)) nShakeFunc();
                    }
                }
            }

            g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                const coreVector2 vPos = pBullet->GetPosition().xy();
                const coreVector2 vDir = pBullet->GetFlyDir();

                const coreBool A = (vPos.x < vAreaFrom.x) && (vDir.x < 0.0f);
                const coreBool B = (vPos.x > vAreaTo  .x) && (vDir.x > 0.0f);
                const coreBool C = (vPos.y < vAreaFrom.y) && (vDir.y < 0.0f);
                const coreBool D = (vPos.y > vAreaTo  .y) && (vDir.y > 0.0f);

                if(A || B || C || D)
                {
                    pBullet->Deactivate(false);
                    iOutsideHit += 1u;
                }

                if(((iOrderWall == 0u) && A) ||
                   ((iOrderWall == 1u) && B) ||
                   ((iOrderWall == 2u) && C) ||
                   ((iOrderWall == 3u) && D))
                {
                    if(++iOrderState >= ARRAY_SIZE(aiBoxOrder) - 1u)
                    {
                        STAGE_BADGE(1u, BADGE_NORMAL, pBullet->GetOwner()->GetPosition())
                    }

                    nAdvanceOrderFunc();

                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, SPECIAL_SOUND_PROGRESS(iOrderState, ARRAY_SIZE(aiBoxOrder) - 1u), SOUND_ITEM_01);
                }
            });

            for(coreUintW i = 0u; i < POST_WALLS; ++i)
            {
                afOffCurrent[i] = afOffTarget[i];
                g_pPostProcessing->SetWallOffset(i, afOffCurrent[i]);
            }
        }
        else
        {
            const coreFloat fPower = 0.002f * (iDefend ? -4.0f : 3.0f) * (bDelay ? 3.0f : 1.0f) * (g_pGame->IsEasy() ? (iDefend ? 2.0f : 0.5f) : 1.0f) * RCP(I_TO_F(iDefend ? g_pGame->GetNumPlayers() : 1u));

            if(iActive) g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                const coreVector2 vPos  = pBullet->GetPosition().xy();
                const coreVector2 vDir  = pBullet->GetFlyDir();
                const coreFloat   fPush = fPower * I_TO_F(pBullet->GetDamage());

                const auto nPushFunc = [&](const coreUintW iIndex)
                {
                    ASSERT(iIndex < POST_WALLS)

                         if(fPush < 0.0f) afOffTarget[iIndex] = MIN(afOffTarget[iIndex], fOffMax);
                    else if(fPush > 0.0f) afOffTarget[iIndex] = MAX(afOffTarget[iIndex], fOffMin);

                    afOffTarget[iIndex] += fPush;

                    if((fPush < 0.0f) && (afOffTarget[iIndex] < 0.0f)) afOffTarget[iIndex] = -1.0f;   // snap in
                };

                     if(HAS_BIT(iActive, 0u) && (vPos.x < vAreaFrom.x) && (vDir.x < 0.0f)) {nPushFunc(0); pBullet->Deactivate(true); iOutsideHit += 1u;}
                else if(HAS_BIT(iActive, 1u) && (vPos.x > vAreaTo  .x) && (vDir.x > 0.0f)) {nPushFunc(1); pBullet->Deactivate(true); iOutsideHit += 1u;}
                else if(HAS_BIT(iActive, 2u) && (vPos.y < vAreaFrom.y) && (vDir.y < 0.0f)) {nPushFunc(2); pBullet->Deactivate(true); iOutsideHit += 1u;}
                else if(HAS_BIT(iActive, 3u) && (vPos.y > vAreaTo  .y) && (vDir.y > 0.0f)) {nPushFunc(3); pBullet->Deactivate(true); iOutsideHit += 1u;}
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

                            nShakeFunc();

                            REMOVE_BIT(iActive, i)
                            if(!iActive)
                            {
                                STAGE_DELAY_END
                                if(!iOutsideHit) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
                            }

                            g_pPostProcessing->SetWallOffset(i, 0.0f);

                            continue;
                        }
                    }

                    if((iDefend == 3u) || ((iDefend == 1u) == (i < 2u))) afOffTarget[i] = MAX(afOffTarget[i] + 0.07f * TIME * (g_pGame->IsEasy() ? 0.9f : 1.0f), fOffMin - (bDelay ? 1.0f : 0.01f));
                }
                else
                {
                    afOffTarget[i] = MIN(afOffTarget[i] - 0.07f * TIME * (g_pGame->IsEasy() ? 1.1f : 1.0f), fOffMax + 0.01f);
                }

                const coreFloat fDiff = afOffTarget[i] - afOffCurrent[i];
                afOffCurrent[i] += SIGN(fDiff) * 0.9f * TIME * SmoothTowards(ABS(fDiff), 0.05f);

                afOffTarget [i] = CLAMP(afOffTarget [i], fOffMin - 1.0f, fOffMax + 1.0f);
                afOffCurrent[i] = CLAMP(afOffCurrent[i], fOffMin,        fOffMax);

                g_pPostProcessing->SetWallOffset(i, afOffCurrent[i]);
            }
        }

        const coreVector2 vPlayerAreaFromPre = PLAYER_AREA_DEFAULT.xy() + vOffFrom;
        const coreVector2 vPlayerAreaToPre   = PLAYER_AREA_DEFAULT.zw() - vOffTo;

        vOffFrom  = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[0], afOffCurrent[2]);   // re-calculated with new offsets
        vOffTo    = 2.2f * FOREGROUND_AREA * coreVector2(afOffCurrent[1], afOffCurrent[3]);
        vAreaFrom = -FOREGROUND_AREA + vOffFrom;
        vAreaTo   =  FOREGROUND_AREA - vOffTo;

        const coreVector2 vPlayerAreaFrom = PLAYER_AREA_DEFAULT.xy() + vOffFrom;
        const coreVector2 vPlayerAreaTo   = PLAYER_AREA_DEFAULT.zw() - vOffTo;

        if(vPlayerAreaFrom < vPlayerAreaTo)
        {
            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                pPlayer->SetArea(coreVector4(vPlayerAreaFrom, vPlayerAreaTo));
            });
        }
        else
        {
            for(coreUintW j = 0u; j < POST_WALLS; ++j) afOffTarget[j] = -1.0f;
        }

        if(m_iStageSub >= 1u)
        {
            constexpr coreUintW iTotalBullets = POST_WALLS * 20u;

            if((iWallBullets < iTotalBullets) && STAGE_TICK_FREE(15.0f, 0.0f))
            {
                for(coreUintW i = 0u; i < POST_WALLS; ++i)
                {
                    const coreVector2 vPos = HIDDEN_POS;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(i + 1, 0.0f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.5f)->AddStatus(BULLET_STATUS_GHOST);

                    iWallBullets += 1u;
                }

                nShakeFunc(false);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }

            if(iWallBullets == iTotalBullets)
            {
                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    if(pPlayer->IsNormal())
                    {
                        const coreVector2 vPos = pPlayer->GetPosition().xy();

                        if(((vPos.x <= vPlayerAreaFromPre.x + CORE_MATH_PRECISION) && HAS_BIT(iActive, 0u)) ||
                           ((vPos.x >= vPlayerAreaToPre  .x - CORE_MATH_PRECISION) && HAS_BIT(iActive, 1u)) ||
                           ((vPos.y <= vPlayerAreaFromPre.y + CORE_MATH_PRECISION) && HAS_BIT(iActive, 2u)) ||
                           ((vPos.y >= vPlayerAreaToPre  .y - CORE_MATH_PRECISION) && HAS_BIT(iActive, 3u)))
                        {
                            pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vPos);
                            if(m_iStageSub >= 11u) for(coreUintW j = 0u; j < POST_WALLS; ++j) afOffTarget[j] = -1.0f;
                        }
                    }
                });
            }

            coreUintW aiIndex[POST_WALLS] = {};
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                const coreUintW iWall = pBullet->GetDamage() - 1;
                ASSERT(iWall < POST_WALLS)

                const coreFloat fOffset = 1.07f - 2.2f * afOffCurrent[iWall] + (((iWall == iOrderWall) && (aiIndex[iWall] % 2u)) ? 0.04f : 0.0f);
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
            const coreFloat fAcross = (i >= 16u && i < 23u) ? 1.5f : 1.0f;

            STAGE_LIFETIME(pEnemy, fAcross * 0.8f, fAcross * (((i < 7u) ? 2.0f : ((i < 16u) ? 1.5f : ((i < 26u) ? 2.5f : ((i == 41u) ? 10.2f : 0.0f)))) + ((i < 7u) ? (0.2f * I_TO_F(i)) : ((i < 16u) ? (0.23f * I_TO_F(i - 7u)) : ((i < 23u) ? (0.3f * I_TO_F((i - 16u) % 7u)) : ((i < 42u) ? (0.2f * I_TO_F((i - 23u) % 3u)) : 0.0f))))))

            if(i < 42u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((i < 7u) ? ((I_TO_F(i) - 3.0f) * 0.15f) : ((i < 16u) ? ((I_TO_F((i - 7u) % 3u) - 1.0f) * 0.45f) : ((i < 23u) ? ((I_TO_F((i - 16u) % 7u) - 3.0f) * 0.24f) : ((I_TO_F((i - 23u) % 3u) - 1.0f) * 0.45f)))), 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 62u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 42u) % 5u) - 2.0f) * 0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 70u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                coreFloat fHeight;
                switch(i - 62u)
                {
                default: ASSERT(false)
                case 3u: case 1u: fHeight =  0.6f; break;
                case 0u: case 5u: fHeight =  0.2f; break;
                case 2u: case 7u: fHeight = -0.2f; break;
                case 6u: case 4u: fHeight = -0.6f; break;
                }

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(fHeight, 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                const coreSpline2* pPath = ((i - 70u) % 2u) ? pPath2 : pPath1;

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
            }

                 if(i <  16u) {}
            else if(i <  23u) {pEnemy->Rotate90(); if(i % 2u) pEnemy->InvertX();}
            else if(i <  41u) {pEnemy->Rotate90(); if(i % 2u) pEnemy->InvertX(); if(((i - 23u) / 3u) % 2u) pEnemy->InvertY();}
            else if(i <  42u) {}
            else if(i <  47u) pEnemy->Rotate270();
            else if(i <  52u) pEnemy->Rotate270()->InvertX();
            else if(i <  57u) {}
            else if(i <  62u) pEnemy->Rotate180();
            else if(i == 62u || i == 64u || i == 65u || i == 68u) {}
            else if(i == 63u || i == 66u || i == 67u || i == 69u) pEnemy->InvertY();
            else if(i <  72u) {}
            else if(i <  74u) pEnemy->Rotate90 ();
            else if(i <  76u) pEnemy->Rotate180();
            else if(i <  78u) pEnemy->Rotate270();

            coreUintW iAnchor = 0u;

                 if(i <  42u) {}
            else if(i <  47u) iAnchor = BIT(0u);
            else if(i <  52u) iAnchor = BIT(1u);
            else if(i <  57u) iAnchor = BIT(4u);
            else if(i <  62u) iAnchor = BIT(3u);
            else if(i <  70u) {}
            else if(i == 70u) iAnchor = BIT(4u);
            else if(i == 71u) iAnchor = BIT(4u) + BIT(1u);
            else if(i == 72u) iAnchor = BIT(1u);
            else if(i == 73u) iAnchor = BIT(1u) + BIT(3u);
            else if(i == 74u) iAnchor = BIT(3u);
            else if(i == 75u) iAnchor = BIT(3u) + BIT(0u);
            else if(i == 76u) iAnchor = BIT(0u);
            else if(i == 77u) iAnchor = BIT(0u) + BIT(4u);

            coreVector2 vPostFactor = (vAreaTo - vAreaFrom) / (FOREGROUND_AREA * 2.0f) + 0.3f;
            coreVector2 vPostOffset = (vAreaTo + vAreaFrom) * 0.5f;

                 if(HAS_BIT(iAnchor, 0u)) {vPostFactor.x = 1.0f; vPostOffset.x = vAreaFrom.x + FOREGROUND_AREA.x;}   // left   |
            else if(HAS_BIT(iAnchor, 1u)) {vPostFactor.x = 1.0f; vPostOffset.x = vAreaTo  .x - FOREGROUND_AREA.x;}   // right  |
            else if(HAS_BIT(iAnchor, 2u)) {vPostFactor.x = 1.0f; vPostOffset.x = 0.0f;}                              // middle |
                 if(HAS_BIT(iAnchor, 3u)) {vPostFactor.y = 1.0f; vPostOffset.y = vAreaFrom.y + FOREGROUND_AREA.y;}   // bottom -
            else if(HAS_BIT(iAnchor, 4u)) {vPostFactor.y = 1.0f; vPostOffset.y = vAreaTo  .y - FOREGROUND_AREA.y;}   // top    -
            else if(HAS_BIT(iAnchor, 5u)) {vPostFactor.y = 1.0f; vPostOffset.y = 0.0f;}                              // middle -

            pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() * vPostFactor + vPostOffset, 0.0f));

            if((fLifeTime >= 0.0f) && pEnemy->HasStatus(ENEMY_STATUS_GHOST)) pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

            if(g_pGame->IsTask())
            {
                constexpr coreUintW aiFirstOrder[] = {23u, 26u, 30u, 32u, 35u, 39u};

                if((i >= 23u + 3u * iFirstState && i < 41u) && (iFirstState < ARRAY_SIZE(aiFirstOrder)))
                {
                    if(i == aiFirstOrder[iFirstState])
                    {
                        if(!pEnemy->HasStatus(ENEMY_STATUS_CUSTOM))
                        {
                            pEnemy->SetBaseColor(COLOR_SHIP_BLACK, false, true);
                            pEnemy->AddStatus   (ENEMY_STATUS_CUSTOM);
                        }

                        g_pGame->GetCombatText()->AttachMarker(i % COMBAT_MARKERS, "X", pEnemy->GetPosition(), COLOR_MENU_INSIDE, false);
                    }

                    if(pEnemy->ReachedDeath())
                    {
                        if(i == aiFirstOrder[iFirstState])
                        {
                            if(++iFirstState >= ARRAY_SIZE(aiFirstOrder))
                            {
                                STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                            }
                            else
                            {
                                g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iFirstState, ARRAY_SIZE(aiFirstOrder)), SOUND_ITEM_02);
                            }
                        }
                        else
                        {
                            STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
                            {
                                if(pEnemy2->HasStatus(ENEMY_STATUS_CUSTOM))
                                {
                                    pEnemy2->SetBaseColor(COLOR_SHIP_GREEN);
                                }
                            });

                            iFirstState = 0xFFu;
                        }
                    }
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

            pEnemy->DefaultRotate(fLifeTime * (2.0f*PI));

                 if(i == 0u) {}
            else if(i == 1u) pEnemy->Rotate180();

            if(pEnemy->ReachedDeath())
            {
                if(++iHiddenState >= pSquad2->GetNumEnemies())
                {
                    STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                }
                else
                {
                    g_pGame->GetCombatText()->DrawProgress(iHiddenState, pSquad2->GetNumEnemies(), pEnemy->GetPosition());
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iHiddenState, pSquad2->GetNumEnemies()), SOUND_PLACEHOLDER);
                }
            }
        });

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,0.0f), 30.0f);
        }

        STAGE_WAVE(1u, "5-2", {50.0f, 75.0f, 100.0f, 125.0f, 250.0f})   // SECHSUNDZWANZIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cConeBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        g_pPostProcessing->Reset();

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(PLAYER_AREA_DEFAULT);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        cVolcanoBackground* pBackground = d_cast<cVolcanoBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetGroundDensity(1u, 0.0f);

        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,0.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // geometry falls together and forms safe spots
    // gegner am anfang sollten den spieler nie in die enge treiben, den säulen auszuweichen ist anstrengend genug -> keine flachen gruppen
    // erste säule gegenüber von erster gegnerwelle
    // kreuz-tunnel, erster gegner muss seitlich anfangen, letzter gegner oben, nach doppel-gegner nicht zurück zum vorherigen
    // in erster phase, gegner oben und unten nutzen die blöcke als schild, nicht gleichzeitig oben und unten, war zu schwer die stampf-attacken zu sehen
    // in kreuz-tunnel, gegner von angriffen zu entkoppeln erhöhte engagement, im zweiten teil die gegner seitlich statt im tunnel bewegen zu lassen macht es weniger einfach alle zu töten, weil man fürs ausweichen nicht ständig draufhalten kann
    // TASK: destroy the elevator robotnik enemy
    // TASK: collect all gap objects
    // TASK EXTRA: touch all black stones
    // ACHIEVEMENT: destroy 30 enemies from inside the rocks
    // TODO 1: hardmode: blocks have stings (but not always, and they attack certain areas, e.g in the tunnel from both sides), enemies start attacking
    // TODO 1: move shake (and color management if not yet) to mission code, it's only visual
    // TODO 1: smoke zwischen bewegenden steinen (smoke+partikel? oder nur smoke?)
    // TODO 1: enemy-exhaust effekt ist nicht sichtbar über den steinen
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        constexpr coreFloat fStep = GELU_FANG_STEP;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(3u);
            pPath2->AddNode(coreVector2(-0.3f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 0.3f, 0.0f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(-0.3f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(-0.3f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2( 0.3f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(3u);
            pPath4->AddNode(coreVector2(-0.5f,1.3f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2( 0.0f,0.6f), coreVector2(1.0f, 0.0f));
            pPath4->AddNode(coreVector2( 0.5f,1.3f), coreVector2(0.0f, 1.0f));
            pPath4->Refine();
        });

        STAGE_ADD_PATH(pPath5)
        {
            pPath5->Reserve(2u);
            pPath5->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath5->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath5->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 90u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(14u)
            STAGE_GET_FLOAT(fStompTime)
            STAGE_GET_UINT (iStompCount)
            STAGE_GET_UINT (iStompTarget)
            STAGE_GET_UINT (iStompTargetNext)
            STAGE_GET_UINT (iStompInvert)
            STAGE_GET_UINT (iStompInvertNext)
            STAGE_GET_UINT (iEffectState)
            STAGE_GET_FLOAT(fChangeDelay)
            STAGE_GET_FLOAT(fWaveTime)
            STAGE_GET_UINT (iElevatorState)
            STAGE_GET_UINT (iDarkState)
            STAGE_GET_UINT (iGapCollected)
            STAGE_GET_UINT (iBackgroundLerp)
            STAGE_GET_UINT (iWithinCount)
        STAGE_GET_END

        coreUint32 iShakeState = 0u;
        STATIC_ASSERT(GELU_FANGS <= sizeof(iShakeState)*8u)

        const auto nFangPositionFunc = [](const coreUintW iIndex)
        {
            ASSERT(iIndex < GELU_FANGS)

            const coreUintW X = iIndex % GELU_FANGS_DIMENSION;
            const coreUintW Y = iIndex / GELU_FANGS_DIMENSION;

            return coreVector2(I_TO_F(X) - 2.0f, I_TO_F(Y) - 2.0f) * fStep;
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

            if(bShake && InBetween(fDelay, fOld, fNew))
            {
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound  (SPECIAL_RELATIVE, 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }

            return CLAMP01(fNew * RCP(fDelay));
        };

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  4u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  5u,  9u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 10u, 14u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 15u, 19u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 20u, 24u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 25u, 29u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 30u, 39u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 40u, 49u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 50u, 50u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 51u, 51u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 52u, 52u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 53u, 54u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 55u, 55u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 56u, 56u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 57u, 58u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 59u, 59u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 60u, 79u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 80u, 89u)
            else if(STAGE_SUB(19u))
            {
                STAGE_DELAY_START_CLEAR

                for(coreUintW i = 0u; i < GELU_GAPS; ++i)
                    this->DisableGap(i, true);
            }

            if(m_iStageSub == 17u)
            {
                m_Chol.ResurrectIntro(1u);
            }

            if(g_pGame->IsTask())
            {
                     if(m_iStageSub ==  1u) STAGE_RESURRECT(pSquad2, 0u, 0u)
                else if(m_iStageSub == 10u) pSquad2->ClearEnemies(false);
            }
        }

        this->SetCrushFree  (m_iStageSub >= 9u || STAGE_TIME_BEFORE(5.0f));
        this->SetCrushLong  (m_iStageSub >= 18u);
        this->SetCrushIgnore(m_iStageSub >= 18u);

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_PURPLE);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect(false);
        }
        if(STAGE_TIME_BEFORE(2.8f))
        {
            const cLodObject& oAnchor = m_aFangRaw[5];

            const coreFloat fPush = oAnchor.GetPosition().x + oAnchor.GetVisualRange().x * -1.2f;
            const coreFloat fLeft = LERP(-1.7f, -0.3f, m_fStageTime / 2.8f) * FOREGROUND_AREA.x;

            pHelper->SetPosition(coreVector3(MIN(fLeft, fPush), oAnchor.GetPosition().y, 0.0f));
        }
        else if(STAGE_TIME_POINT(2.8f))
        {
            pHelper->Kill(true);
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

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetArea(PLAYER_AREA_DEFAULT + coreVector4(0.0f,1.0f,0.0f,-1.0f) * 1.95f * (m_aFangRaw[0].GetSize().x * fTime3));
            });
        }
        else if(m_iStageSub == 9u)
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

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetArea(PLAYER_AREA_DEFAULT + coreVector4(0.0f,1.0f,0.0f,-1.0f) * 1.95f * (m_aFangRaw[0].GetSize().x * (1.0f - fTime3)));
            });
        }
        else if(m_iStageSub == 17u)
        {
            if(STAGE_BEGINNING2)
            {
                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                fWaveTime = 0.0f;
            }

            const coreFloat fTime1 = nTakeTimeFunc(1.0f, 0.0f);
            const coreFloat fTime2 = nTakeTimeFunc(1.0f, 1.0f);

            const coreVector2 vPos1 = LERPBR(coreVector2(0.0f, fStep), coreVector2(0.0f, 0.0f), fTime1);
            const coreVector2 vPos2 = LERPBR(coreVector2(0.0f, fStep), coreVector2(0.0f, 0.0f), fTime2);

            for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)
            {
                nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION,      (i >= 2u) ?  vPos1 : coreVector2(0.0f,0.0f));
                nSetFangPositionDualFunc(i * GELU_FANGS_DIMENSION + 1u, (i <  3u) ? -vPos2 : coreVector2(0.0f,0.0f));

                nSetFangPositionFunc(i * GELU_FANGS_DIMENSION + 2u, HIDDEN_POS);
            }
        }
        else if((m_iStageSub == 18u) || (m_iStageSub == 19u))
        {
            if(m_iStageSub == 18u)
            {
                if(STAGE_BEGINNING2)
                {
                    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                    {
                        if((i % GELU_FANGS_DIMENSION) == 2u)
                        {
                            this->DisableFang(i, false);
                        }
                    }

                    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                    {
                        const coreFloat fShift  = ((((i - 80u) % GELU_FANGS_DIMENSION) % 2u) ? -1.0f : 1.0f) * ((i < 85u) ? 3.0f : 2.0f) * -3.0f;
                        const coreFloat fHeight = I_TO_F((i - 80u) % GELU_FANGS_DIMENSION) - 2.0f;

                        pEnemy->SetPosition(coreVector3(fShift, fHeight, 0.0f) * FOREGROUND_AREA3 * fStep);
                    });

                    for(coreUintW i = 0u; i < GELU_GAPS; ++i)
                    {
                        this->EnableGap(i);

                        const coreFloat fShift  = ((i % 2u) ? -1.0f : 1.0f) * -3.0f;
                        const coreFloat fHeight = I_TO_F(i) - 2.0f;

                        this->GetGap(i)->SetPosition(coreVector3(fShift, fHeight, 0.0f) * FOREGROUND_AREA3 * fStep);
                    }
                }

                if(STAGE_SUBTIME_BETWEEN(0.5f, 1.2f))
                {
                    iShakeState = 0xFFFFFFFFu;
                }

                if(STAGE_SUBTIME_BEFORE(1.2f) || STAGE_SUBTIME_POINT(1.2f))
                {
                    for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                    {
                        nSetFangPositionFunc(i, coreVector2(0.0f,0.0f));
                    }
                }
            }
            else
            {
                if(std::none_of(m_aFangRaw, m_aFangRaw + GELU_FANGS, [](const cLodObject& oFang) {return oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE);}))
                {
                    STAGE_DELAY_END
                }
            }

            const coreFloat fSpeed = (g_pGame->IsEasy() ? 0.6f : 1.0f) * LERP(0.0f, 0.5f, (m_iStageSub == 18u) ? CLAMP01(m_fStageSubTime - 1.2f) : 1.0f) * TIME * FOREGROUND_AREA.x;

            const auto nMoveFunc = [&](coreObject3D* OUTPUT pObject, const coreFloat fMove)
            {
                coreVector3 vNewPos = pObject->GetPosition() + coreVector3(fMove * fSpeed, 0.0f, 0.0f);

                     if((vNewPos.x < -FOREGROUND_AREA.x * 3.0f * fStep) && (fMove < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 6.0f * fStep;
                else if((vNewPos.x >  FOREGROUND_AREA.x * 3.0f * fStep) && (fMove > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 6.0f * fStep;

                pObject->SetPosition(vNewPos);
            };

            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            {
                nMoveFunc(&m_aFangRaw[i], ((i / GELU_FANGS_DIMENSION) % 2u) ? -1.5f : 1.0f);
            }

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                nMoveFunc(pEnemy, (((i - 80u) % GELU_FANGS_DIMENSION) % 2u) ? -1.5f : 1.0f);
            });

            if(g_pGame->IsTask())
            {
                for(coreUintW i = 0u; i < GELU_GAPS; ++i)
                {
                    coreObject3D* pGap = this->GetGap(i);
                    if(!pGap->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                    nMoveFunc(pGap, (i % 2u) ? -1.5f : 1.0f);

                    if(this->IsGapActive(i))
                    {
                        STAGE_FOREACH_PLAYER(pPlayer, j)
                        {
                            const coreVector2 vDiff = MapToAxisInv(pGap->GetPosition().xy() - pPlayer->GetPosition().xy(), pGap->GetDirection().xy());

                            if((ABS(vDiff.x) < pGap->GetCollisionRange().x) &&
                               (ABS(vDiff.y) < pGap->GetCollisionRange().y))
                            {
                                this->DisableGap(i, true);

                                if(++iGapCollected >= GELU_GAPS)
                                {
                                    STAGE_BADGE(1u, BADGE_NORMAL, pGap->GetPosition())
                                }
                                else
                                {
                                    g_pGame->GetCombatText()->DrawProgress(iGapCollected, GELU_GAPS, pGap->GetPosition());
                                    g_pSpecialEffects->PlaySound(pGap->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iGapCollected, GELU_GAPS), SOUND_ITEM_01);
                                }
                            }
                        });
                    }
                }
            }
        }

        if(m_iStageSub < 10u)
        {
            if(STAGE_TIME_BETWEEN(4.0f, 5.0f))
            {
                ADD_BIT(iShakeState, (4u * GELU_FANGS_DIMENSION) - GELU_FANGS_DIMENSION)
            }
            else if(STAGE_TIME_AFTER(5.0f) && ((m_iStageSub < 9u) || fStompTime))
            {
                const coreBool bUpsideDown = (iStompCount % 2u);

                const coreFloat fStompTimePrev = fStompTime;
                fStompTime += (g_pGame->IsEasy() ? 0.6f : 1.0f) * 0.9f * TIME;

                if(InBetween(0.5f, fStompTimePrev, fStompTime))
                {
                    const coreFloat fSide = g_pGame->FindPlayerDual(iStompCount % 2u)->GetPosition().x / FOREGROUND_AREA.x;

                    iStompTargetNext = CLAMP(F_TO_UI((fSide + 1.1f) / fStep), 0u, GELU_FANGS_DIMENSION - 1u);
                    iStompInvertNext = (iStompCount % 5u) == 0u;

                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    g_pSpecialEffects->PlaySound(coreVector3(nFangPositionFunc(iStompTarget + GELU_FANGS_DIMENSION * 2u) * FOREGROUND_AREA, 0.0f), 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
                    g_pSpecialEffects->RumblePlayer(NULL, iStompInvert ? SPECIAL_RUMBLE_BIG : SPECIAL_RUMBLE_SMALL, 250u);
                }

                if(fStompTime >= 0.5f)
                {
                    for(coreUintW j = 0u; j < GELU_FANGS_DIMENSION; ++j)
                    {
                        if((iStompInvertNext != 0u) != (j == iStompTargetNext)) ADD_BIT(iShakeState, j + (bUpsideDown ? (4u * GELU_FANGS_DIMENSION) : 0u))
                    }
                }

                if(fStompTime >= 1.2f)
                {
                    fStompTime   = 0.0f;
                    iStompCount  = iStompCount + 1u;
                    iStompTarget = iStompTargetNext;
                    iStompInvert = iStompInvertNext;

                    if(iStompCount == 8u) iElevatorState = iStompTarget;
                }

                const coreVector2 vFrom = coreVector2(0.0f, 3.0f * fStep * (bUpsideDown ? -1.0f : 1.0f));
                const coreVector2 vTo   = coreVector2(0.0f, 0.0f);
                const coreVector2 vPos  = LERP(vTo, vFrom, ABS(COS(DelayTime(fStompTime, 0.5f, 0.2f) * (1.0f*PI))));

                for(coreUintW i = 0u; i < GELU_FANGS_DIMENSION; ++i)   // y
                {
                    const coreBool bStill = (i == (bUpsideDown ? 4u : 0u));

                    for(coreUintW j = 0u; j < GELU_FANGS_DIMENSION; ++j)   // x
                    {
                        const coreVector2 vOffset = bStill ? vTo : (((iStompInvert != 0u) != (j == iStompTarget)) ? vPos : vFrom);

                        nSetFangPositionFunc(j + i * GELU_FANGS_DIMENSION, vOffset);
                    }
                }

                cLodObject& oHole     = m_aFangRaw[iElevatorState + 2u * GELU_FANGS_DIMENSION];
                cEnemy*     pRobotnik = pSquad2->GetEnemy(0u);

                if(g_pGame->IsTask() && (iStompCount == 8u))
                {
                    oHole.SetAlpha(0.0f);

                    pRobotnik->SetPosition(oHole.GetPosition());
                    if(pRobotnik->ReachedDeath())
                    {
                        STAGE_BADGE(0u, BADGE_EASY, pRobotnik->GetPosition())
                    }
                }
                else
                {
                    oHole.SetAlpha(1.0f);
                }
            }
        }

        const coreFloat fShake = 0.9f * (0.5f - TriangleWave(m_fStageTime * 10.0f));

        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
        {
            cLodObject& oFang = m_aFangRaw[i];
            if(!oFang.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            if(HAS_BIT(iShakeState, i))
            {
                oFang.SetPosition(oFang.GetPosition() + coreVector3(fShake, 0.0f, 0.0f));
                oFang.SetColor3  (COLOR_SHIP_RED * 0.3f);

                if(!HAS_BIT(iEffectState, i))
                {
                    ADD_BIT(iEffectState, i)
                    //g_pSpecialEffects->CreateBreakupColor(&oFang, 10.0f, 100u, COLOR_ENERGY_RED);
                }
            }
            else
            {
                oFang.SetColor3((m_iStageSub == 19u) ? (COLOR_SHIP_YELLOW * 0.3f) : GELU_FANG_COLOR);

                REMOVE_BIT(iEffectState, i)
            }

            if(g_pGame->IsTaskExtra() && (m_iStageSub >= 10u) && (m_iStageSub < 17u))
            {
                // 11 18  >  0 3
                // 06 13  >  1 2
                constexpr coreUintW aiCorner   [] = {11u, 6u, 13u, 18u};
                constexpr coreUintW aiDarkOrder[] = {0u, 2u, 3u, 2u, 1u, 3u};//, 0u, 1u};
                STATIC_ASSERT(std::all_of(aiDarkOrder, aiDarkOrder + ARRAY_SIZE(aiDarkOrder), [](const coreUintW A) {return (A < ARRAY_SIZE(aiCorner));}))

                if((iDarkState < ARRAY_SIZE(aiDarkOrder)) && (i == aiCorner[aiDarkOrder[iDarkState]]))
                {
                    oFang.SetColor3(GELU_FANG_COLOR * 0.5f);

                    if(HAS_BIT(m_iTouchState, i))
                    {
                        if(++iDarkState == ARRAY_SIZE(aiDarkOrder))
                        {
                            STAGE_BADGE(2u, BADGE_HARD, oFang.GetPosition())
                        }
                    }
                }
            }
        }

        if((m_iStageSub >= 9u) && (m_iStageSub < 18u))
        {
            const coreFloat fWaveTimePrev = fWaveTime;
            fWaveTime += 1.0f * TIME;

            if(m_iStageSub < 17u)
            {
                if((fWaveTime >= 7.0f) && STAGE_TICK_EXTERN(fWaveTime, fWaveTimePrev, 1.0f * (g_pGame->IsEasy() ? 0.5f : 1.0f), 0.0f))
                {
                    constexpr coreUintW A[] = {1u, 3u, 2u, 0u};

                    const coreVector2 vDir = StepRotated90(A[s_iTick % ARRAY_SIZE(A)]);
                    const coreVector2 vPos = vDir * FOREGROUND_AREA * -1.2f;

                    for(coreUintW i = 12u; i--; )
                    {
                        const coreVector2 vOffset = (vDir.Rotated90() * (I_TO_F(i % 6u) - 2.5f) - vDir * I_TO_F(i / 6u)) * 3.1f;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.0f, pSquad1->GetEnemy(0u), (vPos + vOffset), vDir)->ChangeSize(1.5f);
                    }
                }
            }
            else
            {
                if((fWaveTime >= 2.0f) && STAGE_TICK_EXTERN(fWaveTime, fWaveTimePrev, 2.2f * (g_pGame->IsEasy() ? 0.5f : 1.0f), 0.0f))
                {
                    const coreFloat fShift = (I_TO_F((s_iTick % 4u) % 3u) - 1.0f) * (((s_iTick / 4u) % 2u) ? -1.0f : 1.0f) * 1.5f;

                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);
                    const coreVector2 vPos = vDir * FOREGROUND_AREA * -1.2f;

                    for(coreUintW i = 6u; i--; )
                    {
                        const coreVector2 vOffset = (vDir.Rotated90() * (I_TO_F(i % 3u) - 1.0f + fShift) - vDir * I_TO_F(i / 3u)) * 3.1f;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.3f, pSquad1->GetEnemy(0u), vPos + vOffset, vDir)->ChangeSize(1.5f);
                    }
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i == 50u) ? 0.4f : ((i == 51u) ? 0.6f : ((i < 50u) ? 1.0f : 0.8f)), (i < 50u) ? (0.25f * I_TO_F(i % 5u) + ((i < 5u) ? 7.0f : 0.0f)) : ((i < 60u) ? ((i == 50u) ? 3.0f : 0.0f) : ((i < 80u) ? (0.4f * I_TO_F(i - 60u)) : 0.0f)))

            const coreSpline2* pPath = (i < 10u) ? pPath3 : ((i < 30u) ? pPath2 : ((i < 50u) ? pPath4 : ((i < 60u) ? pPath5 : pPath1)));

            if(pPath != pPath5) STAGE_REPEAT(pPath->GetTotalDistance())

            if(i < 50u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 30u) ? 0.0f : (((i / 5u) % 2u) ? 0.5f : -0.5f), 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  5u) pEnemy->Rotate90();
                else if(i < 10u) pEnemy->Rotate90()->InvertX();
                else if(i < 15u) pEnemy->Rotate90();
                else if(i < 20u) pEnemy->Rotate90()->InvertY();
                else if(i < 25u) pEnemy->Rotate90()->InvertX();
                else if(i < 30u) pEnemy->Rotate90()->InvertX()->InvertY();
                else if(i < 40u) pEnemy->InvertX ();
                else if(i < 50u) pEnemy->InvertX ()->Rotate180();
            }
            else if(i < 60u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                switch(i - 50u)
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
                case 8u: pEnemy->Rotate270(); break;
                case 9u:                      break;
                }
            }
            else if(i < 80u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 60u) % 5u) - 4.0f) * 0.2f, 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
                if(i % 2u) pEnemy->InvertX();
            }
            else
            {
                pEnemy->DefaultRotate(fLifeTimeBase * 2.0f*PI);
            }

            if(pEnemy->ReachedDeath())
            {
                const coreUintW iIndex = pEnemy->LastAttackerIndex();
                if(m_abCrushImmune[iIndex] && m_abCrushInside[iIndex])
                {
                    if(++iWithinCount >= 30u) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pEnemy->GetPosition())
                }
            }
        });

        cVolcanoBackground* pBackground = d_cast<cVolcanoBackground*>(g_pEnvironment->GetBackground());

        if(!pBackground->GetOutdoor()->IsLerping())
        {
                 if(iBackgroundLerp == 0u) {pBackground->GetOutdoor()->LerpHeight( 0.5f, -11.0f); iBackgroundLerp++;}
            else if(iBackgroundLerp == 1u) {pBackground->GetOutdoor()->LerpHeight(-0.5f, -26.0f); iBackgroundLerp++;}
        }

        if(iBackgroundLerp == 2u)
        {
            const coreFloat fEnvLerp = pBackground->GetOutdoor()->GetLerp();

            pBackground->SetGroundDensity(0u, STEP(0.5f, 1.0f, 1.0f - fEnvLerp));
        }

        if((m_iStageSub == 10u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,1.0f), 10.0f);

        STAGE_WAVE(2u, "5-3", {55.0f, 80.0f, 110.0f, 135.0f, 270.0f})   // SIEBENUNDZWANZIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cFlipBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            this->DisableFang(i, false);

        for(coreUintW i = 0u; i < GELU_GAPS; ++i)
            this->DisableGap(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea(PLAYER_AREA_DEFAULT);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 3u, 4u, 5u})
    {
        cVolcanoBackground* pBackground = d_cast<cVolcanoBackground*>(g_pEnvironment->GetBackground());

        pBackground->GetOutdoor()->LerpHeightNow(-0.5f, -26.0f);
        pBackground->SetGroundDensity(0u, 0.0f);
        pBackground->SetGroundDensity(1u, 0.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // snap to grid
    // player should not be moved if his current orb gets destroyed, to not move him into possible attack without being able to know or react (but provide way to both move into desired valid orb from there, and to return to far valid node as fallback)
    // [deprecated] do not reduce movement-speed while shooting, feels sluggish
    // moving enemies between orbs cause waiting time and frustration, but moving on nodes causes too much pressure related to attacks (because fine evasion is not possible) -> no enemy should move, player needs to be able to kill them on his own pace
    // rotating orbs is impossible from a UX perspective
    // [deprecated] attacks while moving between orbs (with sluggishness) are too hard to control precisely
    // the spawn pattern should make sure that it is impossible for the single player to stand at a location where the next enemy might spawn
    // [deprecated] moving or stretching orbs is possible (e.g. inner 4 orbs to outer), but a meaningful enemy pattern related to it might be problematic
    // lines can be used even when they blend-out, as long as the target orb is enabled
    // moving everything statically is possible and feels nice, but does not add any depth (so it's only used to improve the rail-sequence)
    // in rail-sequence, make sure enemies are stretched out, so player has to move all the way, and nearly touch the sides, and enemies do not die too quickly
    // in 3. grid, 2/2+2/3 (X/Y, start 0, oben links) is ne todesfalle
    // TASK: touch every orb at least once (collect all shines)
    // TASK: hit all additional targets
    // ACHIEVEMENT: be on top of an orb which does not exist anymore
    // TODO 1: hardmode: crossing lines ?
    // TODO 1: hardmode: a bug trying to follow and bite you
    // TODO 1: add input-cache du allow quick movement, e.g. right up up right down
    // TODO 5: badge: collect yellow blocks as badge (+ extra score ?)
    // TODO 5: badge: move along a marked line after another
    // TODO 5: badge: guitar hero
    // TODO 5: badge: items in finaler phase einsammeln die von oben herunterfliegen
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreFloat fOrbLen = 0.5f;
        constexpr coreUintW iOrbNum = 16u;
        STATIC_ASSERT(iOrbNum <= GELU_ORBS)

        constexpr coreUint8 aiSurf[] = {11u, 22u, 33u, 44u, 55u};

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.3f),           coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,1.5f) * fOrbLen, coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 91u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.8f);
                pEnemy->Configure((i < 55u) ? 4 : 8, 0u, COLOR_SHIP_MAGENTA);
            });
        });

        STAGE_GET_START(3u * GAME_PLAYERS + 6u)
            STAGE_GET_UINT_ARRAY(aiTarget,  GAME_PLAYERS)
            STAGE_GET_VEC2_ARRAY(avOldMove, GAME_PLAYERS)
            STAGE_GET_VEC2      (vSurferMove)
            STAGE_GET_UINT      (iSurferCount)
            STAGE_GET_UINT      (iShineCollected)
            STAGE_GET_UINT      (iBulletTick)
            STAGE_GET_UINT      (iBulletCount)
        STAGE_GET_END

        // 12 13 14 15
        // 08 09 10 11
        // 04 05 06 07
        // 00 01 02 03

        // ()09()10()11()
        // 20  21  22  23
        // ()06()07()08()
        // 16  17  18  19
        // ()03()04()05()
        // 12  13  14  15
        // ()00()01()02()

        const auto nPosFromFunc = [](const coreUintW iIndex)
        {
            ASSERT(iIndex < iOrbNum)
            return coreVector2::Direction(I_TO_F(iIndex) / I_TO_F(iOrbNum - 1u) * (9.0f*PI)) * FOREGROUND_AREA * 2.0f;   // -1 looks better
        };

        const auto nPosToFunc = [](const coreUintW iIndex)
        {
            ASSERT(iIndex < iOrbNum)
            return coreVector2(I_TO_F(iIndex % 4u) - 1.5f, I_TO_F(iIndex / 4u) - 1.5f) * FOREGROUND_AREA * fOrbLen;
        };

        const auto nLineIndexFunc = [](coreUintW iOrb1, coreUintW iOrb2)
        {
            ASSERT((iOrb1 < iOrbNum) && (iOrb2 < iOrbNum))

            if(iOrb1 > iOrb2) std::swap(iOrb1, iOrb2);
            ASSERT(iOrb1 != iOrb2)

            const coreUintW iLine = (iOrb2 - iOrb1 == 4u) ? (iOrb1 + 12u) : (iOrb1 - (iOrb1 / 4u));
            ASSERT(iLine < GELU_LINES)

            return iLine;
        };

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_CYAN);

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  3u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  4u,  6u)

            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 10u, 10u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 11u, 11u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 12u, 12u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 13u, 13u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 14u, 14u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 16u, 16u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 17u, 17u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 18u, 18u)

            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 19u, 19u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 20u, 20u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 21u, 21u)
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 22u, 22u)
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad1, 23u, 23u)
            else if(STAGE_SUB(22u)) STAGE_RESURRECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB(23u)) STAGE_RESURRECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB(24u)) STAGE_RESURRECT(pSquad1, 26u, 26u)
            else if(STAGE_SUB(25u)) STAGE_RESURRECT(pSquad1, 27u, 27u)
            else if(STAGE_SUB(26u)) STAGE_RESURRECT(pSquad1, 28u, 28u)
            else if(STAGE_SUB(27u)) STAGE_RESURRECT(pSquad1, 29u, 29u)
            else if(STAGE_SUB(28u)) STAGE_RESURRECT(pSquad1, 30u, 30u)

            else if(STAGE_SUB(29u)) STAGE_RESURRECT(pSquad1, 31u, 31u)
            else if(STAGE_SUB(30u)) STAGE_RESURRECT(pSquad1, 32u, 32u)
            else if(STAGE_SUB(31u)) STAGE_RESURRECT(pSquad1, 33u, 33u)
            else if(STAGE_SUB(32u)) STAGE_RESURRECT(pSquad1, 34u, 34u)
            else if(STAGE_SUB(33u)) STAGE_RESURRECT(pSquad1, 35u, 35u)
            else if(STAGE_SUB(34u)) STAGE_RESURRECT(pSquad1, 36u, 36u)
            else if(STAGE_SUB(35u)) STAGE_RESURRECT(pSquad1, 37u, 37u)
            else if(STAGE_SUB(36u)) STAGE_RESURRECT(pSquad1, 38u, 38u)

            else if(STAGE_SUB(37u)) STAGE_RESURRECT(pSquad1, 39u, 46u)
            else if(STAGE_SUB(38u)) STAGE_RESURRECT(pSquad1, 47u, 54u)

            else if(STAGE_SUB(39u)) STAGE_RESURRECT(pSquad1, 55u, 90u)
            else if(STAGE_SUB(40u)) STAGE_DELAY_START_CLEAR

            if(m_iStageSub == 32u)
            {
                m_Chol.ResurrectIntro(2u);
            }

            constexpr coreUint8 aiPattern1[] = { 0u,  1u,  2u,  3u,  4u,  6u,  7u,  8u, 10u, 13u, 17u, 18u, 19u, 20u, 22u, 23u};
            constexpr coreUint8 aiPattern2[] = { 2u,  3u,  4u,  5u,  7u,  9u, 11u, 12u, 13u, 15u, 17u, 18u, 19u, 20u, 21u, 22u};
            constexpr coreUint8 aiPattern3[] = { 0u,  3u,  4u,  5u,  7u,  8u,  9u, 10u, 13u, 14u, 15u, 16u, 17u, 18u, 21u, 23u};

            const coreUint8* piPattern   = NULL;
            coreUintW        iPatternNum = 0u;

            switch(m_iStageSub)
            {
            case  5u: piPattern = aiPattern1; iPatternNum = ARRAY_SIZE(aiPattern1); break;
            case 11u: piPattern = aiPattern2; iPatternNum = ARRAY_SIZE(aiPattern2); break;
            case 17u: piPattern = aiPattern3; iPatternNum = ARRAY_SIZE(aiPattern3); break;
            }

            if(iPatternNum)
            {
                for(coreUintW i = 0u; i < GELU_LINES; ++i)
                {
                    if(std::memchr(piPattern, i, iPatternNum))
                    {
                        if(!this->IsLineEnabled(i)) this->EnableLine(i);
                    }
                    else
                    {
                        if(this->IsLineEnabled(i)) this->DisableLine(i, true);
                    }
                }

                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.5f, 1.0f, SOUND_EFFECT_SWIPE_01);
            }

            const auto nDisableFunc = [this](const coreUintW iIndex)
            {
                this->DisableOrb(iIndex, true);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            };

            switch(m_iStageSub)
            {
            case 20u: nDisableFunc( 3u); break;
            case 22u: nDisableFunc(15u); pHelper->Resurrect(true); break;
            case 25u: nDisableFunc( 0u); break;
            case 27u: nDisableFunc(12u); break;
            case 30u: nDisableFunc( 8u); break;
            case 31u: nDisableFunc(14u); break;
            case 32u: nDisableFunc( 1u); break;
            case 33u: nDisableFunc( 7u); break;
            case 34u: nDisableFunc(13u); break;
            case 35u: nDisableFunc( 4u); break;
            case 36u: nDisableFunc(11u); break;
            case 37u: nDisableFunc( 2u); break;
            }

            if((m_iStageSub == 17u) || (m_iStageSub == 29u) || (m_iStageSub == 38u))
            {
                iBulletTick = 0u;
            }

            if(g_pGame->IsTask())
            {
                if(m_iStageSub == 5u)
                {
                    for(coreUintW i = 0u; i < iOrbNum; ++i)
                        this->EnableShine(i);
                }
                else if(m_iStageSub == 37u)
                {
                    for(coreUintW i = 0u; i < iOrbNum; ++i)
                        this->DisableShine(i, true);
                }
            }
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreFloat   fTime = MAX0(pHelper->GetLifeTime() - 0.1f) * 1.3f;
            const coreVector2 vPos  = m_aOrbRaw[15].GetPosition().xy() + coreVector2(0.0f, ParaLerp(0.0f, -6.0f, 15.0f, fTime));

            pHelper->SetPosition(coreVector3(vPos, 0.0f));

            if(fTime >= 1.0f) pHelper->Kill(false);
        }

        if(m_iStageSub >= 37u)
        {
            constexpr coreUintW aiExpand[] = {5u, 6u, 9u, 10u};

            const coreVector2 vOldPos = m_aOrbRaw[aiExpand[0]].GetPosition().xy();

            coreVector2 vFactor = coreVector2(1.0f,1.0f);
            coreVector2 vOffset = coreVector2(0.0f,0.0f);

            if(m_iStageSub == 37u)
            {
                const coreVector2 vValue = LERPS(coreVector2(1.0f,1.0f), coreVector2(4.0f,0.0f), MIN1(m_fStageSubTime * 0.5f));
                for(coreUintW i = 0u; i < ARRAY_SIZE(aiExpand); ++i)
                {
                    m_aOrbRaw[aiExpand[i]].SetPosition(coreVector3(nPosToFunc(aiExpand[i]) * vValue, 0.0f));
                }

                if(vOldPos.y) vFactor = m_aOrbRaw[aiExpand[0]].GetPosition().xy() / vOldPos;
            }
            else if(m_iStageSub == 39u)
            {
                const coreVector2 vValue = LERPS(coreVector2(0.0f,0.0f), coreVector2(0.0f,-2.0f), MIN1(m_fStageSubTime * 0.5f));
                for(coreUintW i = 0u; i < ARRAY_SIZE(aiExpand); ++i)
                {
                    m_aOrbRaw[aiExpand[i]].SetPosition(coreVector3(nPosToFunc(aiExpand[i]) * coreVector2(4.0f,0.0f) + vValue * (fOrbLen * FOREGROUND_AREA.y), 0.0f));
                }

                vOffset = m_aOrbRaw[aiExpand[0]].GetPosition().xy() - vOldPos;
            }
            else if(m_iStageSub == 40u)
            {
                const coreVector2 vValue = coreVector2(ParaLerp(4.0f, 4.2f, 0.0f, MIN1(m_fStageSubTime * 2.0f)), 0.0f);
                for(coreUintW i = 0u; i < ARRAY_SIZE(aiExpand); ++i)
                {
                    m_aOrbRaw[aiExpand[i]].SetPosition(coreVector3(nPosToFunc(aiExpand[i]) * vValue + coreVector2(0.0f,-2.0f) * (fOrbLen * FOREGROUND_AREA.y), 0.0f));
                }

                if(vOldPos.x) vFactor.x = m_aOrbRaw[aiExpand[0]].GetPosition().x / vOldPos.x;
            }

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                pPlayer->SetPosition(coreVector3(pPlayer->GetPosition().xy() * vFactor + vOffset, 0.0f));   // remove interpolation-delay
            });
        }

        if(m_iStageSub == 40u)
        {
            if(STAGE_SUBTIME_AFTER(0.5f))
            {
                STAGE_DELAY_END

                for(coreUintW i = 0u; i < iOrbNum; ++i)
                    this->DisableOrb(i, false);

                m_Surfer.Kill(true);

                STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
                });

                g_pSpecialEffects->MacroExplosionColorBig(m_aOrbRaw[5].GetPosition(), COLOR_ENERGY_CYAN);
                g_pSpecialEffects->PlaySound(m_aOrbRaw[5].GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
                g_pSpecialEffects->PlaySound(m_aOrbRaw[5].GetPosition(), 1.0f, 1.0f, SOUND_EFFECT_SHAKE_02);
            }
        }

        if(STAGE_BEGINNING)
        {
            for(coreUintW i = 0u; i < iOrbNum; ++i)
                this->EnableOrb(i);

            this->SetLineMode(0u);
        }

        if(STAGE_TIME_BEFORE(3.0f))
        {
            for(coreUintW i = 0u; i < iOrbNum; ++i)
                m_aOrbRaw[i].SetPosition(coreVector3(LERPB(nPosFromFunc(i), nPosToFunc(i), CLAMP01(m_fStageTime - 2.0f * (1.0f - (I_TO_F(i) / I_TO_F(GELU_ORBS - 1u))))), 0.0f));
        }
        else if(STAGE_TIME_POINT(3.0f))
        {
            for(coreUintW i = 0u; i < GELU_LINES; ++i)
                this->EnableLine(i);

            for(coreUintW i = 0u; i < iOrbNum; ++i)
                m_aOrbRaw[i].SetPosition(coreVector3(nPosToFunc(i), 0.0f));

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                aiTarget [i] = i ? 2u : 1u;
                avOldMove[i] = pPlayer->GetInput()->vMove;

                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);
                this->ConsumeFreezeMove(i);

                const coreVector3 vPos = m_aOrbRaw[aiTarget[i]].GetPosition();

                g_pSpecialEffects->CreateSplashColor(vPos, SPECIAL_SPLASH_SMALL, COLOR_ENERGY_CYAN);
                g_pSpecialEffects->CreateBlastSphere(vPos, SPECIAL_BLAST_SMALL,  COLOR_ENERGY_CYAN);
            });

            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.5f, 1.0f, SOUND_EFFECT_SWIPE_01);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
        }
        else
        {
            const auto nChangeTargetFunc = [&](const coreUintW iIndex, const coreUint32 iNewTarget)
            {
                ASSERT(iNewTarget < iOrbNum)

                if(this->IsOrbEnabled(iNewTarget) && (!this->IsOrbEnabled(aiTarget[iIndex]) || m_aLineRaw[nLineIndexFunc(aiTarget[iIndex], iNewTarget)].IsEnabled(CORE_OBJECT_ENABLE_ALL)))
                    aiTarget[iIndex] = iNewTarget;

                if(!this->IsOrbEnabled(aiTarget[iIndex]))   // fallback
                {
                    const coreUint32 A = aiTarget[iIndex];
                         if((A ==  0u) || (A ==  1u) || (A ==  4u)) aiTarget[iIndex] =  5u;
                    else if((A ==  2u) || (A ==  3u) || (A ==  7u)) aiTarget[iIndex] =  6u;
                    else if((A ==  8u) || (A == 12u) || (A == 13u)) aiTarget[iIndex] =  9u;
                    else if((A == 11u) || (A == 14u) || (A == 15u)) aiTarget[iIndex] = 10u;
                }
            };

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                if(!pPlayer->HasStatus(PLAYER_STATUS_NO_INPUT_MOVE)) return;

                if(!this->IsOrbEnabled(aiTarget[i])) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pPlayer->GetPosition())

                const coreUint8   x      = (aiTarget[i] % 4u);
                const coreUint8   y      = (aiTarget[i] / 4u);
                const coreVector2 vMove = pPlayer->HasStatus(PLAYER_STATUS_DEAD) ? coreVector2(0.0f,0.0f) : this->ConsumeFreezeMove(i);

                const coreVector2 vDiff1 = m_aOrbRaw[aiTarget[i]].GetPosition().xy() - pPlayer->GetPosition().xy();

                if((ABS(vDiff1.y) < 3.0f) && (SIGNUM(avOldMove[i].x) != SIGNUM(vMove.x)))
                {
                         if((x > 0u) && (vMove.x < 0.0f)) nChangeTargetFunc(i, aiTarget[i] - 1u);
                    else if((x < 3u) && (vMove.x > 0.0f)) nChangeTargetFunc(i, aiTarget[i] + 1u);

                    avOldMove[i].x = vMove.x;
                }
                else if((ABS(vDiff1.x) < 3.0f) && (SIGNUM(avOldMove[i].y) != SIGNUM(vMove.y)))
                {
                         if((y > 0u) && (vMove.y < 0.0f)) nChangeTargetFunc(i, aiTarget[i] - 4u);
                    else if((y < 3u) && (vMove.y > 0.0f)) nChangeTargetFunc(i, aiTarget[i] + 4u);

                    avOldMove[i].y = vMove.y;
                }

                const coreVector2 vDiff2 = m_aOrbRaw[aiTarget[i]].GetPosition().xy() - pPlayer->GetPosition().xy();
                if(!vDiff2.IsNull())
                {
                    const coreVector2 vPos = pPlayer->GetPosition().xy() + vDiff2.Normalized() * (3.0f * pPlayer->CalcMoveSpeed() * TIME * SmoothTowards(vDiff2.Length(), 3.0f));
                    pPlayer->SetPosition(coreVector3(vPos, 0.0f));

                    if((vDiff2.LengthSq() > POW2(3.0f)) && STAGE_TICK_FREE(30.0f, 0.0f))
                    {
                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_CYAN);
                    }
                }
            });

            if(g_pGame->IsTask())
            {
                for(coreUintW i = 0u; i < GELU_SHINES; ++i)
                {
                    const coreObject3D& oShine = m_aShineRaw[i];
                    if(!this->IsShineActive(i)) continue;

                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = oShine.GetPosition().xy() - pPlayer->GetPosition().xy();
                        if(vDiff.LengthSq() < POW2(5.0f))
                        {
                            this->DisableShine(i, true);

                            if(++iShineCollected >= GELU_SHINES)
                            {
                                STAGE_BADGE(0u, BADGE_EASY, oShine.GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawCountdown(iShineCollected, GELU_SHINES, oShine.GetPosition());
                                g_pSpecialEffects->PlaySound(oShine.GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iShineCollected, GELU_SHINES), SOUND_ITEM_01);
                            }
                        }
                    });
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 55u) ? 1.5f : 2.0f, (i < 1u) ? 7.0f : ((i < 39u) ? 0.0f : ((i < 47u) ? 2.0f : ((i < 55u) ? 3.5f : ((2.6f/12.0f) * I_TO_F(i - 55u) + 3.0f)))))

            if(i < 55u)
            {
                coreUintW iOffset, iRota;
                switch(i)
                {
                default: ASSERT(false)
                case  0u:           iOffset = 0u; iRota = 0u; break;
                case  1u:           iOffset = 0u; iRota = 1u; break;
                case  2u:           iOffset = 1u; iRota = 3u; break;
                case  3u:           iOffset = 2u; iRota = 3u; break;
                case  4u:           iOffset = 0u; iRota = 2u; break;
                case  5u:           iOffset = 1u; iRota = 2u; break;
                case  6u:           iOffset = 2u; iRota = 2u; break;

                case  7u: case 19u: iOffset = 2u; iRota = 3u; break;
                case  8u: case 20u: iOffset = 2u; iRota = 0u; break;
                case  9u: case 21u: iOffset = 0u; iRota = 2u; break;
                case 10u: case 22u: iOffset = 1u; iRota = 3u; break;
                case 11u: case 23u: iOffset = 0u; iRota = 1u; break;
                case 12u: case 24u: iOffset = 1u; iRota = 2u; break;
                case 13u: case 25u: iOffset = 1u; iRota = 0u; break;
                case 14u: case 26u: iOffset = 0u; iRota = 3u; break;
                case 15u: case 27u: iOffset = 1u; iRota = 1u; break;
                case 16u: case 28u: iOffset = 0u; iRota = 0u; break;
                case 17u: case 29u: iOffset = 2u; iRota = 2u; break;
                case 18u: case 30u: iOffset = 2u; iRota = 1u; break;

                case 31u:           iOffset = 2u; iRota = 3u; break;
                case 32u:           iOffset = 2u; iRota = 0u; break;
                case 33u:           iOffset = 2u; iRota = 2u; break;
                case 34u:           iOffset = 2u; iRota = 1u; break;
                case 35u:           iOffset = 1u; iRota = 0u; break;
                case 36u:           iOffset = 1u; iRota = 3u; break;
                case 37u:           iOffset = 1u; iRota = 1u; break;
                case 38u:           iOffset = 1u; iRota = 2u; break;

                case 39u: case 47u: iOffset = 0u; iRota = 0u; break;
                case 40u: case 48u: iOffset = 1u; iRota = 0u; break;
                case 41u: case 49u: iOffset = 2u; iRota = 0u; break;
                case 42u: case 50u: iOffset = 3u; iRota = 0u; break;
                case 43u: case 51u: iOffset = 0u; iRota = 2u; break;
                case 44u: case 52u: iOffset = 1u; iRota = 2u; break;
                case 45u: case 53u: iOffset = 2u; iRota = 2u; break;
                case 46u: case 54u: iOffset = 3u; iRota = 2u; break;
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
            }
            else
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 55u) * 5u) % 9u) - 4.0f) * (0.7f/4.0f), 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }
        });

        if(STAGE_TIME_AFTER(3.5f) && ((m_iStageSub != 38u) || STAGE_SUBTIME_AFTER(1.0f)) && !STAGE_CLEARED)
        {
            if(STAGE_TICK_FREE((9.0f/4.0f) * (g_pGame->IsEasy() ? 0.5f : 1.0f), 0.0f))
            {
                const auto nShootFunc = [&](const coreVector2 vPos, const coreVector2 vDir)
                {
                    for(coreUintW i = 0u; i < 4u; ++i)
                    {
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pSquad1->GetEnemy(0u), vPos - vDir * (I_TO_F(i) * 3.0f), vDir)->ChangeSize(2.2f);
                    }

                    if(g_pGame->IsTask())
                    {
                        if((++iBulletCount < 256u) && std::memchr(aiSurf, iBulletCount, ARRAY_SIZE(aiSurf)))
                        {
                            this->EnableSurfer();
                            m_Surfer.SetPosition(coreVector3(vPos - vDir * 14.0f, 0.0f));

                            vSurferMove = vDir * (1.0f * BULLET_SPEED_FACTOR);
                        }
                    }
                };

                if(m_iStageSub < 17u)
                {
                    if(iBulletTick % 2u)
                    {
                        constexpr coreUintW A[] = {0u, 2u, 1u, 3u};

                        const coreVector2 vDir = coreVector2(0.0f,-1.0f);
                        const coreVector2 vPos = coreVector2(nPosToFunc(A[(iBulletTick / 2u) % 4u]).x, 1.2f * FOREGROUND_AREA.y);

                        nShootFunc(vPos, vDir);
                    }
                }
                else if(m_iStageSub < 29u)
                {
                    if(iBulletTick % 2u)
                    {
                        const coreVector2 vDir = coreVector2(((iBulletTick / 2u) % 2u) ? -1.0f : 1.0f, 0.0f);
                        const coreVector2 vPos = coreVector2(vDir.x * (-1.2f * FOREGROUND_AREA.x), nPosToFunc((3u - (iBulletTick / 2u) % 4u) * 4u).y);

                        nShootFunc(vPos, vDir);
                    }
                }
                else if(m_iStageSub < 38u)
                {
                    if(iBulletTick % 2u)
                    {
                        const coreVector2 vDir = coreVector2(0.0f,-1.0f);
                        const coreVector2 vPos = vDir * (-1.2f * FOREGROUND_AREA);

                        nShootFunc(vPos, vDir);
                    }
                }
                else if(m_iStageSub < 40u)
                {
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);
                    const coreVector2 vPos = coreVector2(fOrbLen * ((iBulletTick % 2u) ? 2.0f : -2.0f), 1.2f) * FOREGROUND_AREA;

                    nShootFunc(vPos, vDir);
                }

                iBulletTick += 1u;
            }
        }

        if(g_pGame->IsTask() && !m_Surfer.HasStatus(ENEMY_STATUS_DEAD))
        {
            const coreVector2 vNewPos = m_Surfer.GetPosition().xy() + vSurferMove * TIME;

            if(((vNewPos.x < -FOREGROUND_AREA.x * 1.3f) && (vSurferMove.x < 0.0f)) ||
               ((vNewPos.x >  FOREGROUND_AREA.x * 1.3f) && (vSurferMove.x > 0.0f)) ||
               ((vNewPos.y < -FOREGROUND_AREA.y * 1.3f) && (vSurferMove.y < 0.0f)) ||
               ((vNewPos.y >  FOREGROUND_AREA.y * 1.3f) && (vSurferMove.y > 0.0f)))
            {
                this->DisableSurfer(false);
            }

            m_Surfer.SetPosition(coreVector3(vNewPos, 0.0f));

            if(m_Surfer.WasDamaged())
            {
                this->DisableSurfer(false);

                if(++iSurferCount == ARRAY_SIZE(aiSurf))
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, m_Surfer.GetPosition())
                }
                else
                {
                    g_pGame->GetCombatText()->DrawProgress(iSurferCount, ARRAY_SIZE(aiSurf), m_Surfer.GetPosition());
                    g_pSpecialEffects->PlaySound(m_Surfer.GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iSurferCount, ARRAY_SIZE(aiSurf)), SOUND_ITEM_02);
                }

                g_pSpecialEffects->MacroExplosionColorSmall(m_Surfer.GetPosition(), COLOR_ENERGY_YELLOW);
                g_pSpecialEffects->PlaySound(m_Surfer.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
            }
        }

             if((m_iStageSub ==  1u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,1.0f).Normalized(), 10.0f);
        else if((m_iStageSub == 37u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,1.0f),              10.0f);

        STAGE_WAVE(3u, "5-4", {50.0f, 75.0f, 100.0f, 125.0f, 250.0f})   // ACHTUNDZWANZIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cWaveBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < GELU_ORBS; ++i)
            this->DisableOrb(i, false);

        for(coreUintW i = 0u; i < GELU_LINES; ++i)
            this->DisableLine(i, false);

        for(coreUintW i = 0u; i < GELU_SHINES; ++i)
            this->DisableShine(i, false);

        this->DisableSurfer(false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
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
    // blocks react to certain move or turn directions
    // arrows in your direction are transparent
    // use as shield from enemy attacks
    // erste reihe hat keine pfeile nach unten, und später keine nach oben, um sicher zu stellen, dass spieler am ende auf richtiger seite ist
    // erste reihe hat keine zwei gleichen pfeile hintereinander, außer bei letztem abschnitt
    // erster gegner muss verdeckt sein um kollision zu zeigen
    // zweite gruppe muss seitlich sein um flip mechanik zu zeigen
    // dritte gruppe zwingt spieler durch blöcke durch zu fliegen
    // [deprecated] line pattern should have no arrows down and only 1-turn changes, to make navigation easier in this first moving pattern
    // block pattern should not have same direction twice
    // checkerboard pattern should provide a distinct path to kill enemies as fast as possible (also in coop, equal amount of enemies for each player) (also every empty field has a single arrow it points at) (you can shortcut by flying close to enemies, but that's fine)
    // there should be an empty line between pattern changes and every pattern starts at 0 (deterministic)
    // collision modifier needs to be big enough, so player cannot squeeze throw 2 blocks touching their corners
    // bei linien-welle, nicht alle 4 varianten in einer reihe, damit der spieler sich immer drehen muss, aber so dass einzige drehung sowohl links oder rechts ein loch öffnet
    // blöcke sollen von links kommen, um von regulären side-scrollern abzuweichen (e.g. revert-part in Gradius V)
    // in linien welle, linie sollt nicht durchgehend ausgerichtet sein weil zu leicht, gegner die separat fliegen sind interessanter, aber nicht entlang bewegungs-richtung weil zu leicht, gruppe oben drückt den spieler zurück während er sie töten will, gruppe unten setzten den spieler unter druck nicht zerquetscht zu werden
    // in cluster gruppe, gegner müssen durch blöcke fliegen, damit sie nicht in einem zug zerstört werden können
    // TASK: dance dance revolution variant
    // TASK: various blocks spin around and get destroyed when touched
    // ACHIEVEMENT: never touch a solid block
    // TODO 1: hardmode: arrows drehen oder flippen sich alle N sekunden
    // TODO 1: hardmode: no arrows visible, maybe under exception (timed, blinking, when disabled)
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        constexpr coreFloat fStep        = GELU_WAY_STEP;
        constexpr coreFloat fStepDelay   = 0.067f;
        constexpr coreUintW iSingleIndex = 57u;

        constexpr coreUint8 aiDancePattern[] = {1u, 2u, 3u, 0u, 3u, 0u, 1u, 0u, 3u, 0u, 3u, 2u, 1u, 0u, 1u};

        // {sub-stage, way-number}
        constexpr coreUint32 aaiFree[][2] = {{6u, 1u}, {8u, 13u}, {10u, 20u}, {11u, 20u}, {12u, 5u}};

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

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f,  (4.0f * fStep - 0.0014f)), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f, -(4.0f * fStep - 0.0014f)), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f,  3.5f * fStep), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f, -2.5f * fStep), coreVector2(0.0f,-1.0f));
            pPath4->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 58u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(10, 0u, COLOR_SHIP_BLUE);
            });
        });

        STAGE_GET_START(11u)
            STAGE_GET_VEC2 (vSpawnTime)
            STAGE_GET_FLOAT(fSpawnOffset)
            STAGE_GET_FLOAT(fCheckerBreak)
            STAGE_GET_UINT (iCheckerCount)
            STAGE_GET_UINT (iPatternCount)
            STAGE_GET_UINT (iPatternReset)
            STAGE_GET_UINT (iCreateStart)
            STAGE_GET_UINT (iFreeState)
            STAGE_GET_UINT (iDanceIndex)
            STAGE_GET_UINT (iTouchState)
        STAGE_GET_END

        const auto nFreeFunc = [&]()
        {
            if(g_pGame->IsTask())
            {
                for(coreUintW i = 0u; i < ARRAY_SIZE(aaiFree); ++i)
                {
                    if((aaiFree[i][0] == m_iStageSub) && (aaiFree[i][1] == iPatternReset))
                    {
                        this->SetWayFree(iCreateStart - 1u, true);
                        break;
                    }
                }
            }
        };

        const auto nCreateWayFunc = [&](const coreVector2 vPosition, const coreVector2 vDirection)
        {
            for(coreUintW i = iCreateStart; i < GELU_WAYS; ++i)
            {
                const coreObject3D* pWay = this->GetWay(i);

                if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    iCreateStart = i + 1u;
                    this->EnableWay(i, vPosition, vDirection);

                    iPatternReset += 1u;
                    nFreeFunc();

                    return;
                }
            }

            WARN_IF(true) {}
        };

        const auto nDestroyWayFunc = [&](const coreUintW iIndex)
        {
            iCreateStart = MIN(iCreateStart, iIndex);
            this->DisableWay(iIndex, false);
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  4u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  5u,  8u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  9u, 13u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 14u, 18u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 19u, 26u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 57u, 57u)   // #
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 27u, 34u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 35u, 46u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 47u, 56u)
            else if(STAGE_SUB(13u))
            {
                for(coreUintW i = 0u; i < GELU_WAYS; ++i)
                    this->DisableWay(i, true);

                if(!iTouchState) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
            }

                 if(m_iStageSub == 4u) iPatternCount = (((iPatternCount + 1u) % 4u) < 2u) ? 1u : 0u;
            else if(m_iStageSub >= 7u) iPatternCount = 0u;

            iPatternReset = 0u;

            fSpawnOffset = 0.74f + FRACT(vSpawnTime.arr((m_iStageSub < 10u) ? 0u : 1u) * fStepDelay) * fStep;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);

        this->SetCrushFree  (true);
        this->SetCrushLong  (true);
        this->SetCrushIgnore(true);

        const coreFloat fFactor = g_pGame->IsEasy() ? 0.8f : 1.0f;

        coreVector2 vMove;
             if(m_iStageSub <  9u) vMove = coreVector2(1.0f, 0.0f) * (TIME * fFactor * 15.0f);
        else if(m_iStageSub < 10u) vMove = coreVector2(1.0f, 0.0f) * (TIME * fFactor * LERP(15.0f, 80.0f, MIN1(m_fStageSubTime * 0.15f)));
        else if(m_iStageSub < 13u) vMove = coreVector2(0.0f,-1.0f) * (TIME * fFactor * 25.0f);
        else                       vMove = coreVector2(0.0f, 0.0f);

        if(m_iStageSub == 12u)
        {
            if(iCheckerCount == 18u)
            {
                const coreFloat fOldBreak = fCheckerBreak;

                fCheckerBreak += 1.0f * TIME * fFactor;
                vMove         *= 1.0f - STEPH3(0.6f, 0.72f, fCheckerBreak);

                if(InBetween(0.72f, fOldBreak, fCheckerBreak))
                {
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 0.6f, 1.3f, SOUND_EFFECT_SHAKE_01);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
                }
            }
        }

        const coreVector2 vSpawnTimePre = vSpawnTime;
        vSpawnTime += vMove;

        if(m_iStageSub < 10u)   // from left
        {
            if(F_TO_SI(vSpawnTimePre.x * fStepDelay) != F_TO_SI(vSpawnTime.x * fStepDelay))
            {
                iPatternCount += 1u;

                const coreFloat fFix = FRACT(vSpawnTime.x * fStepDelay) * fStep;

                if(m_iStageSub < 8u)   // single
                {
                    const coreUint8 iTick = iPatternCount % 4u;

                         if(m_iStageSub <= 3u) nCreateWayFunc(coreVector2(-1.3f + fFix, 0.0f) * FOREGROUND_AREA, StepRotated90((iTick == 2u) ? 0u : iTick));
                    else if(m_iStageSub <= 5u) nCreateWayFunc(coreVector2(-1.3f + fFix, 0.0f) * FOREGROUND_AREA, (iTick % 2u) ? coreVector2(1.0f,0.0f) : coreVector2(-1.0f,0.0f));
                    else if(m_iStageSub <= 7u) nCreateWayFunc(coreVector2(-1.3f + fFix, 0.0f) * FOREGROUND_AREA, coreVector2(0.0f,1.0f));
                }
                else if(m_iStageSub < 9u)   // scattered
                {
                    if((iPatternCount >= 2u) && ((iPatternCount % 2u) == 0u))
                    {
                        const coreUint8 iTick = (iPatternCount / 2u) % 4u;

                        for(coreUintW i = 0u; i < 6u; ++i)
                        {
                            if((iTick % 2u) == (i % 2u)) continue;

                            nCreateWayFunc(coreVector2(-1.3f + fFix, (I_TO_F(i) - 2.5f) * fStep) * FOREGROUND_AREA, StepRotated90(((iTick / 2u) * 2u + i) % 4u));
                        }
                    }
                }
                else   // dance
                {
                    if(iPatternCount >= 2u)
                    {
                        const coreUint8 iTick = iPatternCount - 2u;

                        if(iTick < ARRAY_SIZE(aiDancePattern))
                        {
                            for(coreUintW i = 0u; i < 3u; ++i)
                            {
                                nCreateWayFunc(coreVector2(-1.3f + fFix, (I_TO_F(i) - 1.0f) * fStep) * FOREGROUND_AREA, StepRotated90(aiDancePattern[iTick]));
                            }
                        }
                    }
                }
            }
        }
        else   // from top
        {
            if(F_TO_SI(vSpawnTimePre.y * fStepDelay) != F_TO_SI(vSpawnTime.y * fStepDelay))
            {
                iPatternCount += 1u;

                const coreFloat fFix = FRACT(vSpawnTime.y * fStepDelay) * fStep;

                if(m_iStageSub == 10u)   // lines
                {
                    if((iPatternCount % 2u) == 1u)
                    {
                        const coreUint8 iTick = (iPatternCount / 2u) % 4u;

                        for(coreUintW i = 0u; i < 6u; ++i)
                        {
                            nCreateWayFunc(coreVector2((I_TO_F(i) - 2.5f) * fStep, 1.3f + fFix) * FOREGROUND_AREA, StepRotated90(((iTick + i) * 2u + iTick) % 4u));
                        }
                    }

                    if(iPatternCount == 3u)
                    {
                        pHelper->Resurrect(false);
                        pHelper->SetPosition(coreVector3(2.5f * fStep, 1.3f + fFix, 0.0f) * FOREGROUND_AREA3);
                    }
                }
                else if(m_iStageSub == 11u)   // 2x2 blocks
                {
                    if(iPatternCount >= 2u)
                    {
                        for(coreUintW i = 0u; i < 6u; ++i)
                        {
                            if(((iPatternCount / 2u) % 2u) == ((i / 2u) % 2u)) continue;

                            nCreateWayFunc(coreVector2((I_TO_F(i) - 2.5f) * fStep, 1.3f + fFix) * FOREGROUND_AREA, StepRotated90(((iPatternCount / 2u) + (iPatternCount / 4u) + (i / 4u)) % 4u));
                        }
                    }
                }
                else if(m_iStageSub == 12u)   // labyrinth
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

                            nCreateWayFunc(coreVector2((I_TO_F(i) - 2.5f) * fStep, 1.3f + fFix) * FOREGROUND_AREA, vDir);
                        }
                    }
                }
            }
        }

        for(coreUintW i = 0u; i < GELU_WAYS; ++i)
        {
            coreObject3D* pWay   = this->GetWay  (i);
            coreObject3D* pArrow = this->GetArrow(i);
            if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !this->IsWayActive(i)) continue;

            const coreVector2 vOldPos = pWay->GetPosition().xy();
            const coreVector2 vNewPos = vOldPos + vMove;

            pWay->SetPosition(coreVector3(vNewPos, 0.0f));

            if(g_pForeground->IsVisiblePoint(vOldPos, 1.3f) && !g_pForeground->IsVisiblePoint(vNewPos, 1.3f))
            {
                nDestroyWayFunc(i);
            }

            if(HAS_BIT(m_iWayFree, i))
            {
                pWay  ->SetDirection(coreVector3(0.0f,1.0f,0.0f));
                pArrow->SetDirection(coreVector3(coreVector2::Direction(m_fStageTime * 2.0f), 0.0f));

                if(HAS_BIT(m_iTouchState, i))
                {
                    nDestroyWayFunc(i);

                    if(++iFreeState >= ARRAY_SIZE(aaiFree))
                    {
                        STAGE_BADGE(1u, BADGE_NORMAL, pWay->GetPosition())
                    }
                    else
                    {
                        g_pGame->GetCombatText()->DrawProgress(iFreeState, ARRAY_SIZE(aaiFree), pWay->GetPosition());
                        g_pSpecialEffects->PlaySound(pWay->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iFreeState, ARRAY_SIZE(aaiFree)), SOUND_ITEM_02);
                    }

                    g_pSpecialEffects->CreateBreakupColor(pWay, 80.0f, 15u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
                    g_pSpecialEffects->PlaySound(pWay->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
                }
            }
        }

        coreBool bPostpone = false;

        if(m_iStageSub == 9u)
        {
            if(STAGE_SUBTIME_AFTER(7.1f / fFactor))
            {
                pSquad1->GetEnemy(iSingleIndex)->Kill(false);
                bPostpone = true;
            }

            if(g_pGame->IsTask() && (iDanceIndex < ARRAY_SIZE(aiDancePattern)))
            {
                for(coreUintW i = 0u; i < GELU_WAYS; ++i)
                {
                    coreObject3D* pWay = this->GetWay(i);
                    if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !this->IsWayActive(i)) continue;

                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pWay->GetPosition().xy() - pPlayer->GetPosition().xy();
                        if((ABS(vDiff.x) < pWay->GetCollisionRange().x) &&
                           (ABS(vDiff.y) < pWay->GetCollisionRange().y))
                        {
                            if(SameDirection(pWay->GetDirection().xy(), pPlayer->GetDirection().xy()))
                            {
                                if(PackDirection(pWay->GetDirection().xy()) == aiDancePattern[iDanceIndex] * 2u)
                                {
                                    if(++iDanceIndex >= ARRAY_SIZE(aiDancePattern))
                                    {
                                        STAGE_BADGE(0u, BADGE_EASY, pPlayer->GetPosition())
                                    }
                                    else
                                    {
                                        g_pSpecialEffects->PlaySound(pWay->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iDanceIndex, ARRAY_SIZE(aiDancePattern)), SOUND_ITEM_01);
                                    }
                                }
                            }
                        }
                    });
                }
            }
        }

        STAGE_FOREACH_PLAYER(pPlayer, j)
        {
            if(m_abCrushInside[j]) ADD_BIT(iTouchState, j)
        });

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + vMove;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 35u) ? 0.5f : (25.0f * fFactor / FOREGROUND_AREA.y), ((i < 1u) ? 2.0f : ((i < 19u) ? 0.0f : ((i < 35u) ? ((2.6f/8.0f) * I_TO_F((i - 19u) % 8u) + 2.0f) : ((i < 47u) ? (2.0f * fStep * I_TO_F((i - 35u) / 3u)) : 0.0f)))) + ((i < 35u) ? 0.0f : fSpawnOffset))

            if(i < 19u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 1u) ? 0.0f : ((i < 2u) ? -0.3f : (((i < 3u) ? 0.3f : ((i < 9u) ? ((((((i - 3u) / 2u) % 2u)) ? 0.65f : -0.65f) + ((i % 2u) ? -0.15f : 0.15f)) : ((I_TO_F((i - 9u) % 5u) - 2.0f) * 0.3f))))), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  3u) {}
                else if(i <  7u) pEnemy->Rotate270();
                else if(i <  9u) pEnemy->Rotate90 ();
                else if(i < 14u) pEnemy->Rotate180();
                else if(i < 19u) {}
            }
            else if(i < 35u)
            {
                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 27u) ? -0.9f : 0.85f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 19u || i == 23u) pEnemy->Rotate90 ();
                else if(i == 20u || i == 24u) pEnemy->Rotate180();
                else if(i == 21u || i == 25u) pEnemy->Rotate270();
                else if(i == 22u || i == 26u) {}
                else if((i - 27u) % 2u)       pEnemy->Rotate90 ()->InvertX();
                else                          pEnemy->Rotate270()->InvertX();
            }
            else if(i < 47u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 35u) % 3u) - 1.0f) * 2.0f * fStep, 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (m_fStageTime * (-0.7f*PI) + (I_TO_F(i - 35u) / 12.0f) * (2.0f*PI));
            }
            else if(i < 57u)
            {
                coreUintW X, Y, D;
                switch(i - 47u)
                {
                default: ASSERT(false)
                case 0u: X = 1u; Y = 1u; D = 2u; break;
                case 1u: X = 0u; Y = 2u; D = 3u; break;
                case 2u: X = 4u; Y = 2u; D = 3u; break;
                case 3u: X = 1u; Y = 3u; D = 1u; break;
                case 4u: X = 3u; Y = 3u; D = 2u; break;
                case 5u: X = 5u; Y = 3u; D = 1u; break;
                case 6u: X = 4u; Y = 4u; D = 0u; break;
                case 7u: X = 1u; Y = 5u; D = 2u; break;
                case 8u: X = 3u; Y = 5u; D = 1u; break;
                case 9u: X = 5u; Y = 5u; D = 2u; break;
                }

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(X) - 2.5f) * fStep, I_TO_F(Y) * fStep);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection   (coreVector3(StepRotated90(D), 0.0f));
            }
            else
            {
                ASSERT(i == iSingleIndex)
                pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            }

            if((i < 27u) && STAGE_TICK_LIFETIME(10.0f, 0.0f) && ((s_iTick % 10u) == 2u))
            {
                const coreUintW   iCount = ((i == 2u) || (i >= 14u && i < 19u)) ? 28u : 6u;
                const coreVector2 vPos   = pEnemy->GetPosition().xy();
                const coreFloat   fBase  = pEnemy->AimAtPlayerDual(((i < 3u) ? (i + 1u) : ((i < 9u) ? ((i - 3u) / 2u) : ((i < 19u) ? ((i - 9u) / 5u) : i))) % 2u).Angle();

                for(coreUintW j = iCount; j--; )
                {
                    if(g_pGame->IsEasy() && ((j % 4u) < 2u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iCount - 1u) * 0.5f) * 4.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(2.0f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 5u, COLOR_ENERGY_BLUE);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });

             if((m_iStageSub ==  1u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2(-1.0f,1.0f).Normalized(), 10.0f);
        else if((m_iStageSub == 10u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2( 0.0f,1.0f),              10.0f);

        if(!bPostpone)
        {
            if(g_pGame->IsEasy()) STAGE_WAVE(4u, "5-5", {70.0f, 100.0f, 135.0f, 165.0f, 325.0f})   // NEUNUNDZWANZIG
                             else STAGE_WAVE(4u, "5-5", {65.0f,  95.0f, 130.0f, 160.0f, 320.0f})
        }
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < GELU_WAYS; ++i)
            this->DisableWay(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)   // copied from group "force rotation"
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
        });

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
    // TODO 1: damagable state stays longer, or until teleport
    // TODO 1: linie (entlang) (die aufeinander prallt)
    // TODO 1: schwachen kreis anzeigen, in dem gegner angreifbar sind (berührung), kreis kann kleiner werden, (gegner auf anderer seite werden aktiv)
    // TODO 1: gegner die nur aus der ferne angreifbar sind (und von nah-gegnern beschützt werden) (schauen anders aus ?)
    // TODO 1: ringelspiel (ikaruga), bienen-waben blöcke (mit invert in mitte, und umgekehrt), checkerboard
    // TODO 1: SCHAU ob man eine andere mechanik von der liste rein-mergen kann ####################################################
    // TODO 1: fixe position, wie minenfeld, ferne gegner greifen an
    // TODO 1: add bouncy ball, which enemies evade
    // TODO 1: MAIN: task-check, helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 100u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREY);

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
            if(i % 2u) pEnemy->AddStatus   (ENEMY_STATUS_INVINCIBLE); else pEnemy->RemoveStatus   (ENEMY_STATUS_INVINCIBLE);

            STAGE_FOREACH_PLAYER(pPlayer, j)
            {
                const coreVector2 vPlayerPos = pPlayer->GetPosition().xy() * mTransformRev;

                const coreVector2 vDiff = vEnemyPos - vPlayerPos;
                const coreVector2 vDir  = vDiff.Normalized();
                const coreFloat   fLen  = vDiff.Length();
                const coreFloat   fStr  = MAX(24.0f - fLen, 0.0f) / 24.0f;
                const coreFloat   fDot  = vEnemyMove.IsNull() ? 0.5f : MAX(coreVector2::Dot(vDir, -vEnemyMove.Normalized()), 0.0f);

                const coreVector2 vRealTarget = LERP(vWrap, vEnemyPos + vDir * 35.0f, fStr*0.0f);
                const coreVector2 vChange     = vRealTarget - vEnemyPos;

                pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + (vChange * ((fStr*0.0f * fDot * 9.0f + 2.0f) * TIME)) * mTransform, 0.0f));

                if(fStr)
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_RED);
            if(i % 2u) pEnemy->RemoveStatus   (ENEMY_STATUS_INVINCIBLE); else pEnemy->AddStatus   (ENEMY_STATUS_INVINCIBLE);
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

                                 vCurPos.x =     CLAMP(vCurPos.x, -1.08f * FOREGROUND_AREA.x, 1.08f * FOREGROUND_AREA.x);
            if(vCurPos.y < 0.0f) vCurPos.y = FmodRange(vCurPos.y, -1.2f  * FOREGROUND_AREA.y, 1.2f  * FOREGROUND_AREA.y);

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

        STAGE_WAVE(5u, "5-?", {60.0f, 80.0f, 100.0f, 120.0f, 240.0f})   // DREISSIG
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
        STAGE_BOSS(m_Chol, {145.0f, 215.0f, 290.0, 360.0f, 720.0f})
    },
    STAGE_PRE()
    {
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
        if(m_bStory)
        {
            m_iOutroSub = 13u;
        }

        STAGE_FINISH_AFTER(MISSION_WAIT_OUTRO)
    });

    // ################################################################
    // ################################################################
}