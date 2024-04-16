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
// setup the Nevo mission
void cNevoMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        g_pEnvironment->SetTargetSpeedNow(6.0f);

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
    // leave destroyable residue on its way (r-type leaver)
    // [deprecated] enemies cannot move on the same path in infinity as the bullets would just overlay or need some handling
    // multiple following enemies with the same move pattern will overlay after some time
    // enemy direction creates a whirl
    // destroying player bullets on impact is bad
    // changing speed breaks bullet pattern
    // following snake is also bad
    // bit-matrix or clearing on contact is bad
    // start direction against whirl direction to not clutter the bullets and show the whirl effect
    // beide sichel-angriffe am ende nicht gleichzeitig, weil spieler sonst eingequetscht wird
    // non-homing gegner müssen schöne muster bilden
    // non-homing muss sich immer so bewegen, dass sie sich nicht mit aktuellem flow mitbewegen (wegen bullet-clustering)
    // TASK: clean a certain amount ot residue
    // TASK: 4 kugeln unter bullets die man abschießen muss
    // TASK: guide one of the followers in circles
    // TODO 1: hard-mode: bullets get bigger with time
    // TODO 1: hard-mode: bullets are bigger in general
    // TODO 1: die geschosse über scraps können zerstört werden ohne die scraps, wenn außerhalb des sichtfelds, weil beides unterschiedliche interaction-ranges haben (remove hiding for rotating scraps?)
    // TODO 1: homing und non-homing müssen sich optisch unterscheiden (zm. irgendein effekt on top) (die finalen wellen könnten sonst verwirren)
    // TODO 1: MAIN: regular score, sound, background rota/speed
    // TODO 1: items abschießen sollte kombo beibehalten (ÜBERALL)
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreFloat fRange = 1.25f;

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 49u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(30, 0u, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_DAMAGING);
            });

            const auto nSetFunc = [](cEnemy* OUTPUT pEnemy, const coreVector2 vPosition, const coreVector2 vDirection)
            {
                pEnemy->SetPosition (coreVector3(vPosition * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDirection.Normalized(),     0.0f));
            };

            nSetFunc(pSquad1->GetEnemy( 0u), coreVector2(-1.3f, 0.9f), coreVector2( 1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy( 1u), coreVector2( 1.6f, 0.3f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy( 2u), coreVector2(-1.9f,-0.3f), coreVector2( 1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy( 3u), coreVector2( 2.2f,-0.9f), coreVector2(-1.0f, 0.0f));

            nSetFunc(pSquad1->GetEnemy( 4u), coreVector2( 1.3f, 0.9f) - coreVector2(-1.0f,-1.0f) * (0.9f/1.3f), coreVector2(-1.0f,-1.0f));   // shift-vectors not normalized
            nSetFunc(pSquad1->GetEnemy( 5u), coreVector2(-1.3f, 0.3f) - coreVector2( 1.0f, 1.0f) * (0.0f/1.3f), coreVector2( 1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy( 6u), coreVector2( 1.3f,-0.3f) - coreVector2(-1.0f,-1.0f) * (0.6f/1.3f), coreVector2(-1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy( 7u), coreVector2(-1.3f,-0.9f) - coreVector2( 1.0f, 1.0f) * (0.3f/1.3f), coreVector2( 1.0f, 1.0f));

            nSetFunc(pSquad1->GetEnemy( 8u), coreVector2(-0.9f,-2.2f), coreVector2( 0.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy( 9u), coreVector2(-0.3f,-1.9f), coreVector2( 0.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(10u), coreVector2( 0.3f,-1.6f), coreVector2( 0.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(11u), coreVector2( 0.9f,-1.3f), coreVector2( 0.0f, 1.0f));

            nSetFunc(pSquad1->GetEnemy(12u), coreVector2(-0.9f,-1.3f), coreVector2(-1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(13u), coreVector2(-0.3f,-1.3f), coreVector2( 1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(14u), coreVector2( 0.3f,-1.3f), coreVector2(-1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(15u), coreVector2( 0.9f,-1.3f), coreVector2( 1.0f, 1.0f));

            nSetFunc(pSquad1->GetEnemy(16u), coreVector2(-0.9f, 1.3f), coreVector2( 0.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(17u), coreVector2(-0.3f, 1.3f), coreVector2( 0.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(18u), coreVector2( 0.3f, 1.3f), coreVector2( 0.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(19u), coreVector2( 0.9f, 1.3f), coreVector2( 0.0f,-1.0f));

            nSetFunc(pSquad1->GetEnemy(20u), coreVector2( 1.3f,-0.9f) - coreVector2(-1.0f, 1.0f) * (0.0f/1.3f), coreVector2(-1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(21u), coreVector2( 1.3f,-0.3f) - coreVector2(-1.0f,-1.0f) * (0.3f/1.3f), coreVector2(-1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(22u), coreVector2( 1.3f, 0.3f) - coreVector2(-1.0f, 1.0f) * (0.6f/1.3f), coreVector2(-1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(23u), coreVector2( 1.3f, 0.9f) - coreVector2(-1.0f,-1.0f) * (0.9f/1.3f), coreVector2(-1.0f,-1.0f));

            nSetFunc(pSquad1->GetEnemy(24u), coreVector2(-0.8f, 1.3f), coreVector2( 0.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(25u), coreVector2( 0.8f,-1.3f), coreVector2( 0.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(26u), coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(27u), coreVector2( 1.3f, 0.0f), coreVector2(-1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(28u), coreVector2(-1.3f,-0.8f), coreVector2( 1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(29u), coreVector2( 0.0f, 1.3f), coreVector2( 1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(30u), coreVector2( 1.3f, 0.8f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(31u), coreVector2( 0.0f,-1.3f), coreVector2(-1.0f, 1.0f));

            nSetFunc(pSquad1->GetEnemy(32u), coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));

            nSetFunc(pSquad1->GetEnemy(33u), coreVector2( 0.8f, 1.3f), coreVector2( 0.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(34u), coreVector2(-0.8f,-1.3f), coreVector2( 0.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(35u), coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(36u), coreVector2(-1.3f, 0.0f), coreVector2( 1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(37u), coreVector2(-1.3f, 0.8f), coreVector2( 1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(38u), coreVector2( 0.0f,-1.3f), coreVector2( 1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(39u), coreVector2( 1.3f,-0.8f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(40u), coreVector2( 0.0f, 1.3f), coreVector2(-1.0f,-1.0f));

            nSetFunc(pSquad1->GetEnemy(41u), coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(42u), coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(43u), coreVector2( 1.5f, 0.0f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(44u), coreVector2(-1.5f, 0.0f), coreVector2( 1.0f, 0.0f));

            nSetFunc(pSquad1->GetEnemy(45u), coreVector2( 1.3f, 1.3f),                         coreVector2(-1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(46u), coreVector2(-1.3f,-1.3f),                         coreVector2( 1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(47u), coreVector2( 1.0f, 1.0f) * (1.3f + 0.2f / SQRT2), coreVector2(-1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(48u), coreVector2(-1.0f,-1.0f) * (1.3f + 0.2f / SQRT2), coreVector2( 1.0f, 1.0f));
        });

        STAGE_GET_START(4u)
            STAGE_GET_FLOAT(fWhirlSpeed)
            STAGE_GET_UINT (iCleanupCount)
            STAGE_GET_UINT (iScrapCount)
            STAGE_GET_UINT (iGuideCount)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 16u, 19u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 20u, 23u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 26u, 26u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 27u, 27u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 28u, 29u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 30u, 31u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 32u, 32u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 33u, 33u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 34u, 34u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 35u, 35u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 36u, 36u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 37u, 38u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 39u, 40u)
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 41u, 44u)
            else if(STAGE_SUB(21u)) STAGE_RESURRECT(pSquad1, 45u, 48u)
        }

        if(STAGE_BEGINNING)
        {
            constexpr coreUint8 aiNewOrder[] = {cTriangleBullet::ID, cQuadBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_CYAN);

        if((m_iStageSub == 6u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
            pHelper->SetPosition(coreVector3(-1.2f,0.0f,0.0f) * FOREGROUND_AREA3);
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + coreVector2(20.0f,0.0f) * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        const auto nCreatScrapFunc = [&](const coreUintW iIndex, const coreVector2 vPosition)
        {
            cEnemy* pDummy = pSquad1->GetEnemy(0u);

            this->EnableScrap(iIndex, vPosition);

            for(coreUintW i = 0u; i < 16u; ++i)
            {
                if((i == 0u) || (i == 3u) || (i == 12u) || (i == 15u)) continue;

                const coreVector2 vPos = vPosition + coreVector2(I_TO_F(i % 4u) - 1.5f, I_TO_F(i / 4u) - 1.5f) * 4.0f;
                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.0f, pDummy, vPos, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
        };

        if(g_pGame->IsTask())
        {
            if((m_iStageSub == 3u) && STAGE_SUBTIME_POINT(3.0f))
            {
                nCreatScrapFunc(0u, coreVector2(0.9f,1.2f) * FOREGROUND_AREA);
            }
            else if((m_iStageSub == 5u) && STAGE_SUBTIME_POINT(5.0f))
            {
                nCreatScrapFunc(1u, coreVector2(1.2f,-0.9f) * FOREGROUND_AREA);
            }
            else if((m_iStageSub == 15u) && STAGE_BEGINNING2)
            {
                nCreatScrapFunc(2u, coreVector2(0.0f, 1.2f) * FOREGROUND_AREA);
                nCreatScrapFunc(3u, coreVector2(0.0f,-1.2f) * FOREGROUND_AREA);
            }

            for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i)
            {
                coreObject3D& oScrap = m_aScrap[i];
                if(!oScrap.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                if((m_afScrapTime[i] >= 6.0f) && !g_pForeground->IsVisibleObject(&oScrap))
                {
                    this->DisableScrap(i, false);
                }

                Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &oScrap, [&](const cBullet* pBullet, const coreObject3D* pScrap, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;
                    if(!oScrap.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) return;

                    this->DisableScrap(i, true);
                    g_pSpecialEffects->PlaySound(oScrap.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);

                    if(++iScrapCount >= NEVO_SCRAPS) STAGE_BADGE(1u, BADGE_NORMAL, oScrap.GetPosition())
                    else g_pGame->GetCombatText()->DrawProgress(iScrapCount, NEVO_SCRAPS, oScrap.GetPosition());
                });
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i != 32u)
            {
                     if((m_iStageSub ==  3u) && (m_fStageSubTime < 2.0f)) return;
                else if((m_iStageSub ==  5u) && (m_fStageSubTime < 4.0f)) return;
                else if((m_iStageSub ==  7u) && (m_fStageSubTime < 2.0f)) return;
                else if((m_iStageSub == 14u) && (fWhirlSpeed     < 1.0f)) return;
            }

            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            coreVector2 vCurDir = pEnemy->GetDirection().xy();

            const coreBool bFollow = (i >= 24u && i < 41u);

            if(bFollow)
            {
                const coreVector2 vDiff = pEnemy->AimAtPlayerDual(vCurDir.IsAligned() ? 0u : 1u);
                const coreFloat   fSide = coreVector2::Dot(vDiff, vCurDir.Rotated90());

                if((coreVector2::Dot(vDiff, vCurDir) <= 0.0f) && !coreMath::IsNear(fSide, 0.0f, 1.0f)) vCurDir = vCurDir.Rotated90() * SIGN(fSide);

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.1f); vCurDir.x =  ABS(vCurDir.x);}
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.1f); vCurDir.x = -ABS(vCurDir.x);}
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.1f); vCurDir.y =  ABS(vCurDir.y);}
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.1f); vCurDir.y = -ABS(vCurDir.y);}
            }
            else
            {
                     if((vCurPos.x < -FOREGROUND_AREA.x * fRange) && (vCurDir.x < 0.0f)) vCurPos.x += FOREGROUND_AREA.x * fRange * 2.0f;
                else if((vCurPos.x >  FOREGROUND_AREA.x * fRange) && (vCurDir.x > 0.0f)) vCurPos.x -= FOREGROUND_AREA.x * fRange * 2.0f;
                     if((vCurPos.y < -FOREGROUND_AREA.y * fRange) && (vCurDir.y < 0.0f)) vCurPos.y += FOREGROUND_AREA.y * fRange * 2.0f;
                else if((vCurPos.y >  FOREGROUND_AREA.y * fRange) && (vCurDir.y > 0.0f)) vCurPos.y -= FOREGROUND_AREA.y * fRange * 2.0f;
            }

            const coreFloat fSpeed = (i == 32u) ? 1.5f : 1.0f;

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            pEnemy->DefaultMoveForward(vCurDir, 29.0f * fSpeed);

            if(STAGE_LIFETIME_BEFORE(10.0f) && STAGE_TICK_TIME2(9.0f * fSpeed, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();
                const coreVector2 vTan = vDir.Rotated90() * 2.0f;

                if(g_pForeground->IsVisiblePoint(vPos, fRange))
                {
                    if(vDir.IsAligned())
                    {
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.1f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.1f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                    else
                    {
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.1f)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.1f)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                }
            }

            if(g_pGame->IsTask() && (i == 32u))
            {
                if(STAGE_TAKEOFF)          this->EnableGuide ();
                if(pEnemy->ReachedDeath()) this->DisableGuide(true);

                if(m_Guide.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    const coreVector2 vDir = StepRotated90((iGuideCount + 1u) % 4u);
                    const coreVector2 vPos = pEnemy->GetPosition().xy() + vDir * 7.0f;

                    m_Guide.SetPosition (coreVector3(vPos, 0.0f));
                    m_Guide.SetDirection(coreVector3(vDir, 0.0f));

                    if(SameDirection(pEnemy->GetDirection().xy(), vDir))
                    {
                        if(++iGuideCount >= 6u)
                        {
                            this->DisableGuide(true);

                            STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                        }
                    }
                }
            }
        });

        if((m_iStageSub >= 3u) && (m_iStageSub < 8u))
        {
            constexpr coreVector4 avFlow[] = {coreVector4(0.0f,0.0f,0.0f,0.0f), coreVector4(0.0f,-20.0f,0.0f,-10.0f), coreVector4(-20.0f,0.0f,20.0f,0.0f)};

            const coreFloat fChange = m_fStageSubTime * 0.5f;

            coreVector4 vLerp;
                 if(m_iStageSub == 3u) vLerp = LERPS(avFlow[0], avFlow[1], MIN1(fChange));
            else if(m_iStageSub == 4u) vLerp = avFlow[1];
            else if(m_iStageSub == 5u) vLerp = (fChange < 1.0f) ? LERPS(avFlow[1], avFlow[0], MIN1(fChange)) : LERPS(avFlow[0], avFlow[2], MIN1(fChange - 1.0f));
            else if(m_iStageSub == 6u) vLerp = avFlow[2];
            else if(m_iStageSub == 7u) vLerp = LERPS(avFlow[2], avFlow[0], MIN1(fChange));
            else ASSERT(false)

            const coreVector2 vMove1 = vLerp.xy() * TIME;
            const coreVector2 vMove2 = vLerp.zw() * TIME;

            const auto nRewindFunc = [](cBullet* OUTPUT pBullet)
            {
                coreVector2 vNewPos = pBullet->GetPosition().xy();

                     if(vNewPos.x < -FOREGROUND_AREA.x * fRange) vNewPos.x += FOREGROUND_AREA.x * fRange * 2.0f;
                else if(vNewPos.x >  FOREGROUND_AREA.x * fRange) vNewPos.x -= FOREGROUND_AREA.x * fRange * 2.0f;
                     if(vNewPos.y < -FOREGROUND_AREA.y * fRange) vNewPos.y += FOREGROUND_AREA.y * fRange * 2.0f;
                else if(vNewPos.y >  FOREGROUND_AREA.y * fRange) vNewPos.y -= FOREGROUND_AREA.y * fRange * 2.0f;

                pBullet->SetPosition(coreVector3(vNewPos, 0.0f));
            };

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() + vMove1, 0.0f));
                nRewindFunc(pBullet);
            });

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() + vMove2, 0.0f));
                nRewindFunc(pBullet);
            });

            for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i)
            {
                coreObject3D& oScrap = m_aScrap[i];
                if(!oScrap.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                oScrap.SetPosition(coreVector3(oScrap.GetPosition().xy() + vMove1, 0.0f));
            }
        }
        else if(m_iStageSub >= 14u)
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

            for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i)
            {
                coreObject3D& oScrap = m_aScrap[i];
                if(!oScrap.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                oScrap.SetPosition(coreVector3(oScrap.GetPosition().xy() * mRotaRev, 0.0f));
            }
        }

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [&](const cBullet* pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(g_pGame->IsCoop())
            {
                     if((pBulletPlayer->GetOwner() == g_pGame->GetPlayer(0u)) && (pBulletEnemy->GetID() != cQuadBullet    ::ID)) return;
                else if((pBulletPlayer->GetOwner() == g_pGame->GetPlayer(1u)) && (pBulletEnemy->GetID() != cTriangleBullet::ID)) return;
            }

            pBulletEnemy->Deactivate(true, vIntersection.xy(), pBulletPlayer->GetFlyDir());

            g_pSpecialEffects->PlaySound(vIntersection, 1.0f, 1.0f, SOUND_PLACEHOLDER);

            iCleanupCount += 1u;
        });

        if(g_pGame->IsEasy())
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([](cBullet* OUTPUT pBullet)
            {
                if(pBullet->GetFlyTime() >= 10.0f) pBullet->Deactivate(true);
            });
        }

        if(g_pGame->GetBulletManagerEnemy()->GetNumBulletsEst() > 1000u)   // to prevent infinite creation
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([](cBullet* OUTPUT pBullet)
            {
                if(pBullet->GetFlyTime() >= 20.0f) pBullet->Deactivate(true);
            });
        }

        if(g_pGame->IsTask() && !STAGE_CLEARED)
        {
            const coreFloat fPercent = I_TO_F(iCleanupCount) / 1700.0f;

                 if(fPercent >= 1.0f)  STAGE_BADGE(0u, BADGE_EASY, coreVector3(0.0f,0.0f,0.0f))
            else if(fPercent >= 0.01f) g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.0f%%", FLOOR(fPercent * 100.0f)), coreVector3(0.0f,0.0f,0.0f), COLOR_MENU_INSIDE);
        }

        STAGE_WAVE(0u, "2-1", {50.0f, 75.0f, 100.0f, 125.0f})   // SIEBEN
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        g_pGame->GetBulletManagerEnemy()->ResetOrder();

        for(coreUintW i = 0u; i < NEVO_SCRAPS; ++i)
            this->DisableScrap(i, false);

        this->DisableGuide(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // switch everything on
    // 6: in last sub-pattern, some middle tiles already marked, as otherwise it would be too easy to activate all
    // 6: for coop, make sure tiles are equally distributed
    // 1-5: blending in tiles needs to be equally delayed, to not favor certain start-positions
    // enemies need to die fast after loosing invincibility
    // rotating grid should not be too fast, as back-and-forth flipping can easily happen, and player needs to be able to move with (faster than) rotation
    // TASK: active some plates in a certain order
    // TASK: kill all unprotected enemies before protection wears off
    // TASK: enable a list of tiles in a very short time
    // TODO 1: hardmode: flipswitch galaxy, || =, mit blink delay wie bei rot+blau blöcke in Mario 3D World (achtung: laser könnten so ähnlich sein wie bomben beim endboss)
    // TODO 1: hardmode: flipswitch galaxy: blocking tiles moving around, lasers blocking movement between tiles
    // TODO 1: adjust enemy movement patterns
    // TODO 1: anderes enabled-pattern bei finaler phase, das aktuelle is viel zu leicht, vielleicht alles disabled ?
    // TODO 1: vielleicht doch auch leichte angriffe auf easy, nur N schuss pro sub-phaes ?
    // TODO 1: MAIN: task-check, regular score, sound, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreUintW iSingleIndex = 60u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.3f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->AddNode(coreVector2( 0.8f,-1.3f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 61u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_GET_START(9u + GAME_PLAYERS)
            STAGE_GET_UINT      (iTileState)
            STAGE_GET_UINT      (iTileDone)
            STAGE_GET_UINT      (iTileWait)
            STAGE_GET_UINT      (iTileScore)
            STAGE_GET_FLOAT     (fTileMove)
            STAGE_GET_FLOAT     (fTimeLimit, fTimeLimit = 4.0f)
            STAGE_GET_UINT      (iGauntlet)
            STAGE_GET_UINT      (iFreeCount)
            STAGE_GET_UINT      (iCurOrder)
            STAGE_GET_UINT_ARRAY(aiRemember, GAME_PLAYERS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  9u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 10u, 19u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 29u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 30u, 39u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 40u, 49u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 60u, 60u)   // #
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 50u, 59u)

            for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                this->DisableTile(i, true);

            iTileWait = 0u;
            iGauntlet = 0u;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + coreVector2(26.0f,13.0f) * (1.0f + pHelper->GetLifeTime()) * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        if(!iTileWait)
        {
            if(!m_aTileRaw[0].IsEnabled(CORE_OBJECT_ENABLE_ALL))
            {
                STATIC_ASSERT(NEVO_TILES >= 16u)

                if(m_iStageSub == 1u)
                {
                    for(coreUintW i = 0u; i < 4u; ++i)
                        this->EnableTile(i, 3u);

                    const coreVector2 vPos = FOREGROUND_AREA * 2.2f / 4.0f;

                    m_aTileRaw[0].SetPosition(coreVector3( vPos,             0.0f));
                    m_aTileRaw[1].SetPosition(coreVector3( vPos.Rotated90(), 0.0f));
                    m_aTileRaw[2].SetPosition(coreVector3(-vPos,             0.0f));
                    m_aTileRaw[3].SetPosition(coreVector3(-vPos.Rotated90(), 0.0f));
                }
                else if(m_iStageSub == 2u)
                {
                    for(coreUintW i = 0u; i < 9u; ++i)
                        this->EnableTile(i, 3u);
                }
                else if(m_iStageSub == 3u)
                {
                    for(coreUintW i = 0u; i < 16u; ++i)
                        this->EnableTile(i, 4u);
                }
                else if(m_iStageSub == 4u)
                {
                    for(coreUintW i = 0u; i < 12u; ++i)
                        this->EnableTile(i, 3u);
                }
                else if(m_iStageSub == 5u)
                {
                    for(coreUintW i = 0u; i < 16u; ++i)
                        this->EnableTile(i, 4u);
                }
                else if(m_iStageSub == 6u)
                {
                    if(!iGauntlet) iGauntlet = 1u;
                }
                else if(m_iStageSub == 7u)
                {
                    if(!iGauntlet) iGauntlet = 1u;
                }

                iTileState = (m_iStageSub == 5u) ? 0b0100'0001'1000'0010u : 0u;
                iTileDone  = 0u;
                iTileWait  = 1u;
                iTileScore = 0u;

                std::memset(aiRemember, 0, sizeof(coreUint32) * GAME_PLAYERS);

                STATIC_ASSERT(NEVO_TILES <= sizeof(coreUint32)*8u)
            }
        }
        else
        {
            if(m_iStageSub == 4u)
            {
                for(coreUintW i = 0u; i < 12u; ++i)
                {
                    const coreFloat fMove = (((i % 3u) == 1u) ? -1.0f : 1.0f) * 12.0f * TIME;

                    coreVector2 vNewPos = m_aTileRaw[i].GetPosition().xy() + coreVector2(0.0f, fMove);

                         if(vNewPos.y < -FOREGROUND_AREA.y * 1.1f * (4.0f/3.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.2f * (4.0f/3.0f);
                    else if(vNewPos.y >  FOREGROUND_AREA.y * 1.1f * (4.0f/3.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.2f * (4.0f/3.0f);

                    m_aTileRaw[i].SetPosition(coreVector3(vNewPos, 0.0f));
                }
            }
            else if(m_iStageSub == 5u)
            {
                const coreMatrix2 mRota = coreMatrix3::Rotation(0.4f * TIME).m12();

                for(coreUintW i = 0u; i < 16u; ++i)
                {
                    m_aTileRaw[i].SetPosition (coreVector3(m_aTileRaw[i].GetPosition ().xy() * mRota, 0.0f));
                    m_aTileRaw[i].SetDirection(coreVector3(m_aTileRaw[i].GetDirection().xy() * mRota, 0.0f));
                }
            }
        }

        const coreUint32 iTileStateOld = iTileState;

        coreBool bPostpone = false;

        if(iGauntlet)
        {
            if(m_iStageSub == 6u)
            {
                if(iGauntlet % 2u)
                {
                    constexpr coreUintW aiIndex[] = {5u, 6u, 9u, 10u};
                    constexpr coreUintW aiOrder[] = {2u, 1u, 0u, 3u, 1u, 2u, 1u, 2u, 3u, 0u};

                    const coreUintW iNum = iGauntlet / 2u;

                    if(iNum < ARRAY_SIZE(aiOrder))
                    {
                        this->DisableTile(aiIndex[aiOrder[iNum]], false);
                        this->EnableTile (aiIndex[aiOrder[iNum]], 4u);

                        iGauntlet += 1u;
                    }
                    else
                    {
                        STAGE_BADGE(2u, BADGE_HARD, coreVector3(0.0f,0.0f,0.0f))

                        pSquad1->GetEnemy(iSingleIndex)->Kill(false);
                        bPostpone = true;
                    }
                }

                fTimeLimit -= 1.0f * TIME;
                if(fTimeLimit < 0.0f)
                {
                    pSquad1->GetEnemy(iSingleIndex)->Kill(false);
                    bPostpone = true;
                }

                g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.0f", CEIL(MAX0(fTimeLimit))), coreVector3(0.0f,0.0f,0.0f), COLOR_MENU_INSIDE);
            }
            else if(m_iStageSub == 7u)
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
                    this->EnableTile(0u, 4u);
                    this->EnableTile(1u, 4u);

                    fTileMove  = 0.0f;
                    iGauntlet += 1u;

                    pHelper->Resurrect(true);
                    pHelper->SetPosition(m_aTileRaw[14].GetPosition());

                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), 25.0f, 5u, COLOR_ENERGY_YELLOW);
                }
                else if(iGauntlet == 6u)
                {
                    fTileMove += 0.6f * TIME;

                    m_aTileRaw[0].SetPosition(coreVector3(LERPS(coreVector2(-0.25f,0.75f), coreVector2(-0.25f,-0.75f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
                    m_aTileRaw[1].SetPosition(coreVector3(LERPS(coreVector2( 0.75f,0.25f), coreVector2(-0.75f, 0.25f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
                }
                else if(iGauntlet == 7u)
                {
                    this->EnableTile(14u, 4u);
                    this->EnableTile(15u, 4u);
                    this->EnableTile(16u, 4u);

                    m_aTileRaw[14].SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    m_aTileRaw[15].SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    m_aTileRaw[16].SetPosition(coreVector3(HIDDEN_POS, 0.0f));

                    fTileMove  = 0.0f;
                    iGauntlet += 1u;
                }
                else if(iGauntlet == 8u)
                {
                    fTileMove += 1.0f * TIME;

                    const coreVector2 vPos = coreVector2::Direction(fTileMove);
                    const coreVector2 vDir = coreVector2::Direction(fTileMove * -2.0f);

                    m_aTileRaw[14].SetPosition(coreVector3((vPos * (FOREGROUND_AREA * 0.5f)),                      0.0f));
                    m_aTileRaw[15].SetPosition(coreVector3((vPos * (FOREGROUND_AREA * 0.5f)).Rotated60 () * -1.0f, 0.0f));
                    m_aTileRaw[16].SetPosition(coreVector3((vPos * (FOREGROUND_AREA * 0.5f)).Rotated120(),         0.0f));

                    m_aTileRaw[14].SetDirection(coreVector3(vDir, 0.0f));
                    m_aTileRaw[15].SetDirection(coreVector3(vDir, 0.0f));
                    m_aTileRaw[16].SetDirection(coreVector3(vDir, 0.0f));
                }
                else if(iGauntlet == 9u)
                {
                    for(coreUintW i = 0u; i < 13u; ++i)
                        this->EnableTile(i, 3u);

                    const coreFloat fScale = ABS(m_aTileRaw[0].GetPosition().x);

                    m_aTileRaw[ 9].SetPosition(coreVector3(-0.5f,-0.5f,0.0f) * fScale);
                    m_aTileRaw[10].SetPosition(coreVector3( 0.5f,-0.5f,0.0f) * fScale);
                    m_aTileRaw[11].SetPosition(coreVector3(-0.5f, 0.5f,0.0f) * fScale);
                    m_aTileRaw[12].SetPosition(coreVector3( 0.5f, 0.5f,0.0f) * fScale);

                    iTileState = 0b1111'000'000'000u;
                    iGauntlet  = 100u;
                }
            }
        }

        coreBool bComplete = !iTileDone;

        if(!iTileDone)
        {
            for(coreUintW i = 0u; i < NEVO_TILES; ++i)
            {
                const coreObject3D& oTile = m_aTileRaw[i];
                if(!this->IsTileEnabled(i)) continue;

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    if(pPlayer->IsRolling()) return;

                    const coreVector2 vDiff = MapToAxisInv(pPlayer->GetPosition().xy() - oTile.GetPosition().xy(), oTile.GetDirection().xy());

                    if((ABS(vDiff.x) < oTile.GetCollisionRange().x) &&
                       (ABS(vDiff.y) < oTile.GetCollisionRange().y))
                    {
                        if(!HAS_BIT(aiRemember[j], i)) TOGGLE_BIT(iTileState, i)
                        ADD_BIT(aiRemember[j], i)

                        if(HAS_BIT(iTileState, i) && !HAS_BIT(iTileScore, i))
                        {
                            ADD_BIT(iTileScore, i)
                            this->AddExtraScore(pPlayer, 100u, oTile.GetPosition());
                        }
                    }
                    else
                    {
                        REMOVE_BIT(aiRemember[j], i)
                    }
                });

                if(HAS_BIT(iTileState, i))
                {
                    this->SetTileStyle(i, 1u);

                    if(!HAS_BIT(iTileStateOld, i)) g_pSpecialEffects->PlaySound(oTile.GetPosition(), 1.0f, 1.0f, SOUND_PLACEHOLDER);
                }
                else
                {
                    this->SetTileStyle(i, 0u);
                    bComplete = false;

                    if(HAS_BIT(iTileStateOld, i)) g_pSpecialEffects->PlaySound(oTile.GetPosition(), 1.0f, 1.0f, SOUND_PLACEHOLDER);
                }
            }

            if(g_pGame->IsTask() && (m_iStageSub == 3u))
            {
                constexpr coreUintW aiOrder[] = {1u, 10u, 7u, 12u};

                for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                {
                    const coreObject3D& oTile = m_aTileRaw[i];
                    if(!this->IsTileEnabled(i)) continue;

                    const coreUintW iIndex = std::find(aiOrder, aiOrder + ARRAY_SIZE(aiOrder), i) - aiOrder;

                    if((iIndex >= iCurOrder) && (iIndex < ARRAY_SIZE(aiOrder)))
                    {
                        g_pGame->GetCombatText()->AttachMarker(iIndex + 1u, coreData::ToChars(iIndex + 1u), oTile.GetPosition(), COLOR_MENU_INSIDE);

                        if(HAS_BIT(iTileState, i) != HAS_BIT(iTileStateOld, i))
                        {
                            if(iIndex == iCurOrder)
                            {
                                if(++iCurOrder >= ARRAY_SIZE(aiOrder)) STAGE_BADGE(1u, BADGE_NORMAL, oTile.GetPosition())
                            }
                            else if((iIndex > iCurOrder) && STAGE_SUBTIME_AFTER(2.0f / NEVO_TILE_SPEED))
                            {
                                iCurOrder = UINT32_MAX;
                            }
                        }
                    }
                }
            }
        }

        if(bComplete)
        {
            if(iGauntlet && (iGauntlet < 100u))
            {
                for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                    this->DisableTile(i, true);

                iTileState = 0u;
                iTileScore = 0u;
                iGauntlet += 1u;
                bComplete  = false;
            }
            else
            {
                for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                    this->SetTileStyle(i, 2u);

                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_PLACEHOLDER);
            }
        }

        if(bComplete) iTileDone = 1u;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i == iSingleIndex)
            {
                pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                return;
            }

            STAGE_LIFETIME(pEnemy, 0.8f, 0.2f * I_TO_F(i % 5u) + (((i % 10u) < 5u) ? 0.0f : (0.5f * pPath1->GetTotalDistance())))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 10u) < 5u) ? 1.0f : -1.0f, ((i % 30u) < 20u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i >= 10u && i < 15u) pEnemy->Rotate90 ();
            else if(i >= 15u && i < 20u) pEnemy->Rotate270();
            else if(i >= 40u && i < 45u) pEnemy->Rotate90 ();
            else if(i >= 45u && i < 50u) pEnemy->Rotate270();

            if(!g_pGame->IsEasy() && !(i % 5u) && STAGE_TICK_TIME2(0.8f, 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                const coreFloat   fBase = pEnemy->GetDirection().xy().Rotated90().Angle();

                for(coreUintW j = 7u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 3.0f) * 5.3f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.5f, pEnemy, vPos,  vDir)->ChangeSize(1.5f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.5f, pEnemy, vPos, -vDir)->ChangeSize(1.5f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_PURPLE);
            }

            if(g_pGame->IsTask())
            {
                constexpr coreUint8 aiFree[] = {4u, 11u, 18u, 37u, 53u, 56u};

                const coreBool bIsFree = std::memchr(aiFree, i, ARRAY_SIZE(aiFree));
                ASSERT(i <= 0xFFu)

                if(bIsFree)
                {
                    if(pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE))
                    {
                        pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                        pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
                    }

                    if(pEnemy->ReachedDeath() && !iTileDone)
                    {
                        if(++iFreeCount >= ARRAY_SIZE(aiFree)) STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                        else g_pGame->GetCombatText()->DrawProgress(iFreeCount, ARRAY_SIZE(aiFree), pEnemy->GetPosition());
                    }
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

        if(!bPostpone) STAGE_WAVE(1u, "2-2", {60.0f, 90.0f, 120.0f, 150.0f})   // ACHT
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < NEVO_TILES; ++i)
            this->DisableTile(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // bomberman hero
    // gegner fliegt auf den spieler zu, weniger nervig, spieler muss trotzdem manchmal ausweichen, lebenspunkte müssen dementsprechend hoch sein um den burst auszugleichen
    // diagonal position makes linear bombs stronger, linear position makes diagonal bombs stronger
    // make sure there is a pause between 4 and 5(rota), make sure to start 5(rota) from different side then 4
    // consistent explosion delays make it easier to focus on the enemies simultaneously
    // letting the enemy spawn bombs made the pattern too simple, a separate spawner creates more pressure
    // 2 safe-spots after another in bomb-hail phase should not share same axis (related to enemy movement)
    // wall phase patterns need to be deterministic (also need to start deterministic)
    // bounce and rotation directions need to be opposite
    // linear movement can't use diagonal bombs as the explosion-lines will pop
    // TASK: kill enemy multiple times from behind
    // TODO 1: if backstabbing should be percent, then go down from 100% to 0% (but where to place the marker ?)
    // TODO 1: hardmode: enemy attacks with simple pattern
    // TODO 1: hardmode: bomben schießen geschosse bei explosion, gekruzt oder entlang strahl
    // TODO 1: hardmode: bomben können angegriffen werden und vorzeitig explodieren, oder blockieren einfach nur angriffe
    // TODO 1: increase safe-block (artificially) in bomb-hail phase (quad-test)
    // TODO 1: blaue kugerl bei wand-bomben sind schwer zu bekommen, sub-stage check hilft aber nicht, weil bomben ja in nächster phase explodieren können
    // TODO 1: MAIN: task-check, regular score, badges, sound, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 58u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.6f);
                pEnemy->Configure(50, 0u, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_GHOST_PLAYER);
            });
        });

        STAGE_GET_START(14u + NEVO_BOMBS * 3u)
            STAGE_GET_VEC2      (vClusterPos, vClusterPos = coreVector2(-1.3f,0.5f) * FOREGROUND_AREA)
            STAGE_GET_VEC2      (vClusterDir, vClusterDir = coreVector2( 1.0f,0.0f))
            STAGE_GET_UINT      (iCurBomb)
            STAGE_GET_UINT      (iCurEnemy)
            STAGE_GET_UINT      (iRotaCount)
            STAGE_GET_UINT      (iWallCount)
            STAGE_GET_UINT      (iBackstabCount)
            STAGE_GET_UINT      (iChipSpawn)
            STAGE_GET_UINT      (iChipCollect)
            STAGE_GET_UINT      (iHelperCount)
            STAGE_GET_VEC2      (vHelperMove)
            STAGE_GET_VEC2_ARRAY(avMove, NEVO_BOMBS)
            STAGE_GET_UINT_ARRAY(aiType, NEVO_BOMBS)
        STAGE_GET_END

        const auto nTargetFunc = [](const coreBool bType, const coreFloat fFactor, const coreVector2 vTarget, coreVector2* OUTPUT pvPos, coreVector2* OUTPUT pvForce)
        {
            ASSERT(pvPos && pvForce)

            const coreVector2 vAxis  = bType ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
            const coreVector2 vSign  = vTarget.Processed(SIGN) * FOREGROUND_AREA;
            const coreVector2 vClamp = vTarget.Processed(CLAMP, -(FOREGROUND_AREA.x - 10.0f), (FOREGROUND_AREA.x - 10.0f));

            (*pvPos)   = -fFactor * vSign * vAxis + vClamp * vAxis.yx();
            (*pvForce) = (vTarget + vSign) * vAxis;
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  4u)   // 5
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  5u,  9u)   // 5
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 10u, 14u)   // 5
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 15u, 19u)   // 5
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 20u, 25u)   //  6
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 26u, 31u)   //  6
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 32u, 37u)   //  6
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 38u, 43u)   //  6
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 44u, 49u)   //  6
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 50u, 57u)   //   8

            iWallCount = 0u;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_ORANGE);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + vHelperMove * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        if(STAGE_BEGINNING)
        {
            this->EnableTrend();
        }

        if(STAGE_CLEARED)
        {
            for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
                this->DisableBomb(i, true);

            for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
                this->DisableBlast(i, true);

            for(coreUintW i = 0u; i < NEVO_CHIPS; ++i)
                this->DisableChip(i, true);

            this->DisableTrend(true);
        }
        else
        {
            vClusterPos += vClusterDir * (30.0f * TIME);

            if(((vClusterPos.x < -FOREGROUND_AREA.x * 1.3f) && (vClusterDir.x < 0.0f)) ||
               ((vClusterPos.x >  FOREGROUND_AREA.x * 1.3f) && (vClusterDir.x > 0.0f)) ||
               ((vClusterPos.y < -FOREGROUND_AREA.y * 1.3f) && (vClusterDir.y < 0.0f)) ||
               ((vClusterPos.y >  FOREGROUND_AREA.y * 1.3f) && (vClusterDir.y > 0.0f)))
            {
                const cPlayer* pPlayer = g_pGame->FindPlayerDual(0u);

                coreVector2 vPos, vForce;
                nTargetFunc(IsHorizontal(vClusterDir), 1.3f, pPlayer->GetPosition().xy(), &vPos, &vForce);

                const coreFloat fCorrection = (vClusterPos.Processed(ABS) - FOREGROUND_AREA * 1.3f).Max();

                vClusterDir = vForce.Normalized();
                vClusterPos = vPos + vClusterDir * fCorrection;
            }

            for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
            {
                cLodObject& oBomb = m_aBombRaw[i];

                if(oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL))
                {
                    const coreFloat  fTime = this->GetBombTime(i);
                    const coreUint32 iType = aiType[i];

                    if((iType == 1u) || (iType == 2u))   // break movement
                    {
                        oBomb.SetPosition(coreVector3(oBomb.GetPosition().xy() + avMove[i] * TIME, 0.0f));
                        avMove[i] *= FrictionFactor(1.5f);

                        if(iType == 2u) oBomb.SetDirection(coreVector3(coreVector2::Direction(fTime * ((i % 2u) ? 1.0f : -1.0f)), 0.0f));
                    }
                    else if(iType == 3u)   // linear movement
                    {
                        coreVector2 vNewPos = oBomb.GetPosition().xy() + avMove[i] * (0.5f * TIME);

                             if((vNewPos.x < -FOREGROUND_AREA.x * 1.2f) && (avMove[i].x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.4f;
                        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.2f) && (avMove[i].x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.4f;
                             if((vNewPos.y < -FOREGROUND_AREA.y * 1.2f) && (avMove[i].y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.4f;
                        else if((vNewPos.y >  FOREGROUND_AREA.y * 1.2f) && (avMove[i].y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.4f;

                        oBomb.SetPosition(coreVector3(vNewPos, 0.0f));
                    }
                    else if(iType == 4u)   // bouncy movement
                    {
                        coreVector2 vNewPos = oBomb.GetPosition().xy() + avMove[i] * (0.5f * TIME);

                             if((vNewPos.x < -FOREGROUND_AREA.x * 1.0f) && (avMove[i].x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x * 1.0f); avMove[i].x =  ABS(avMove[i].x);}
                        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.0f) && (avMove[i].x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x * 1.0f); avMove[i].x = -ABS(avMove[i].x);}
                             if((vNewPos.y < -FOREGROUND_AREA.y * 1.0f) && (avMove[i].y < 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.0f); avMove[i].y =  ABS(avMove[i].y);}
                        else if((vNewPos.y >  FOREGROUND_AREA.y * 1.0f) && (avMove[i].y > 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y - FOREGROUND_AREA.y * 1.0f); avMove[i].y = -ABS(avMove[i].y);}

                        oBomb.SetPosition(coreVector3(vNewPos, 0.0f));
                    }
                    else if(iType == 5u)   // rotating movement
                    {
                        const coreVector2 vRota   = coreVector2::Direction(fTime * (-0.5f*PI) * (avMove[i].Length() * 0.01f));
                        const coreFloat   fLen    = LERPB(1.2f * SQRT2, 0.9f, MIN1(fTime * 0.5f)) * FOREGROUND_AREA.x;
                        const coreVector2 vNewPos = MapToAxis(-avMove[i].Normalized(), vRota) * fLen;

                        oBomb.SetPosition(coreVector3(vNewPos, 0.0f));
                    }
                    else if((iType == 6u) || (iType == 7u))   // lerp movement
                    {
                        coreVector2 vNewPos = oBomb.GetPosition().xy();
                        vNewPos.arr(iType == 7u) = LERPB(avMove[i].x, avMove[i].y, MIN1(fTime * 0.5f));

                        oBomb.SetPosition(coreVector3(vNewPos, 0.0f));
                    }
                    else ASSERT(false)
                }

                if(this->GetBombGone(i))
                {
                    const coreVector2 vPos = oBomb.GetPosition ().xy();
                    const coreVector2 vDir = oBomb.GetDirection().xy();

                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = MapToAxisInv(pPlayer->GetPosition().xy() - vPos, vDir);

                        if((ABS(vDiff.x) < 3.3f) || (ABS(vDiff.y) < 3.3f))
                        {
                            if(pPlayer->IsNormal()) pPlayer->TakeDamage(10, ELEMENT_ORANGE, pPlayer->GetPosition().xy());
                        }
                    });

                    if(++iHelperCount == 10u)
                    {
                        pHelper->Resurrect(false);
                        pHelper->SetPosition(coreVector3(vPos, 0.0f));

                        vHelperMove = vDir.Rotated45().Processed(ABS) * vPos.Processed(SIGN) * -150.0f;
                    }

                    if(g_pGame->IsTask() && (m_iStageSub >= 2u) && (iChipSpawn < NEVO_CHIPS * 2u))
                    {
                        if((++iChipSpawn) % 2u)
                        {
                            const coreUintW iIndex = iChipSpawn / 2u;

                            this->EnableChip(iIndex);
                            this->GetChip(iIndex)->SetPosition(coreVector3(vPos, 0.0f));
                        }
                    }

                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
                }
            }

            if(m_iStageSub == 6u)
            {
                if(STAGE_TICK_FREE2(0.5f, 0.0f))   // 2
                {
                    iWallCount += 1u;

                    const coreUint8 iFrom = (iWallCount % 3u) * 3u + (g_pGame->IsEasy() ? 0u : 1u);
                    const coreUint8 iTo   = (iWallCount % 3u) * 3u + (g_pGame->IsEasy() ? 5u : 4u);

                    for(coreUintW i = 0u; i < 11u; ++i)
                    {
                        if(InBetween(i, iFrom, iTo)) continue;

                        const coreUintW iIndex = (iCurBomb++) % NEVO_BOMBS;

                        this->EnableBomb(iIndex, false);

                        coreVector2 vTarget = HIDDEN_POS;
                        vTarget.arr(iWallCount % 2u) = (I_TO_F(i) - 5.0f) * 8.0f;

                        m_aBombRaw[iIndex].SetPosition (coreVector3(vTarget,  0.0f));
                        m_aBombRaw[iIndex].SetDirection(coreVector3(0.0f,1.0f,0.0f));

                        avMove[iIndex] = coreVector2(-1.2f * FOREGROUND_AREA.y, 0.9f * FOREGROUND_AREA.y) * (((iWallCount % 4u) < 2u) ? 1.0f : -1.0f);
                        aiType[iIndex] = (iWallCount % 2u) ? 6u : 7u;
                    }
                }
            }
            else if(m_iStageSub == 10u)
            {
                if(STAGE_TICK_FREE2(0.25f, 0.0f))   // 2
                {
                    iWallCount += 1u;

                    coreUint8 iX, iY;
                    switch(iWallCount % 8u)
                    {
                    default: ASSERT(false)
                    case 0u: iX = 3u; iY = 3u; break;
                    case 1u: iX = 5u; iY = 7u; break;   // first
                    case 2u: iX = 7u; iY = 3u; break;
                    case 3u: iX = 3u; iY = 5u; break;
                    case 4u: iX = 7u; iY = 7u; break;
                    case 5u: iX = 5u; iY = 3u; break;
                    case 6u: iX = 3u; iY = 7u; break;
                    case 7u: iX = 7u; iY = 5u; break;
                    }

                    coreUint8 aiOrder[11];
                    for(coreUintW i = 0u; i < ARRAY_SIZE(aiOrder); ++i) aiOrder[i] = i;

                    coreData::Shuffle(&*aiOrder, aiOrder + ARRAY_SIZE(aiOrder), iWallCount);
                    std::swap(*(s_cast<coreUint8*>(std::memchr(aiOrder, iX, ARRAY_SIZE(aiOrder)))), aiOrder[iY]);

                    for(coreUintW i = 0u; i < 11u; ++i)
                    {
                        if(i == iY) continue;
                        if(g_pGame->IsEasy() && ((i == iY+1u) || (i == iY-1u))) continue;

                        const coreUintW iIndex = (iCurBomb++) % NEVO_BOMBS;

                        this->EnableBomb(iIndex, false);

                        const coreVector2 vTarget = coreVector2(I_TO_F(aiOrder[i]) - 5.0f, I_TO_F(i) - 5.0f) * 8.0f;

                        m_aBombRaw[iIndex].SetPosition (coreVector3(HIDDEN_POS.x, vTarget.y, 0.0f));
                        m_aBombRaw[iIndex].SetDirection(coreVector3(0.0f,1.0f,0.0f));

                        avMove[iIndex] = coreVector2(((iWallCount % 2u) ? -1.2f : 1.2f) * FOREGROUND_AREA.x, vTarget.x);
                        aiType[iIndex] = 6u;
                    }
                }
            }
            else
            {
                if(STAGE_TICK_FREE(0.75f * (g_pGame->IsEasy() ? 0.7f : 1.0f), 0.0f) && (iRotaCount || ((s_iTick % 2u) == 0u)))
                {
                    const cPlayer*    pPlayer = g_pGame->FindPlayerDual(1u);
                    const coreBool    bType   = (m_iStageSub <= 2u) ? true : ((m_iStageSub <= 4u) ? false : !((iRotaCount++) % 2u));
                    const coreUintW   iNum    = (m_iStageSub == 3u || m_iStageSub == 4u) ? 3u : 1u;
                    const coreVector2 vOffset = StepRotated45((((s_iTick % 4u) < 2u) ? 0u : 2u) + ((m_iStageSub == 3u) ? 1u : 0u));

                    coreVector2 vPos, vForce;
                    nTargetFunc(bType, 1.2f, pPlayer->GetPosition().xy(), &vPos, &vForce);

                    for(coreUintW i = 0u; i < iNum; ++i)
                    {
                        const coreUintW iIndex = (iCurBomb++) % NEVO_BOMBS;

                        this->EnableBomb(iIndex, false);

                        m_aBombRaw[iIndex].SetPosition (coreVector3(vPos + 10.0f * (vOffset * (I_TO_F(i) - 0.5f * I_TO_F(iNum - 1u)) - vForce.Normalized()), 0.0f));
                        m_aBombRaw[iIndex].SetDirection((m_iStageSub == 2u || m_iStageSub == 4u) ? coreVector3(1.0f,1.0f,0.0f).Normalized() : coreVector3(0.0f,1.0f,0.0f));

                        avMove[iIndex] = 1.5f * ((m_iStageSub == 8u) ? vForce.Rotated45() : vForce);
                        aiType[iIndex] = (m_iStageSub == 9u) ? 5u : ((m_iStageSub == 8u) ? 4u : ((m_iStageSub == 7u) ? 3u : ((m_iStageSub == 5u) ? 2u : 1u)));
                    }
                }
            }

            for(coreUintW i = 0u; i < NEVO_CHIPS; ++i)
            {
                coreObject3D* pChip = this->GetChip(i);
                if(!pChip->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pChip->GetPosition().xy() - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(5.0f))
                    {
                        this->DisableChip(i, true);

                        if(++iChipCollect >= NEVO_CHIPS) STAGE_BADGE(1u, BADGE_NORMAL, pChip->GetPosition())
                        else g_pGame->GetCombatText()->DrawProgress(iChipCollect, NEVO_CHIPS, pChip->GetPosition());

                        g_pSpecialEffects->PlaySound(pChip->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iChipCollect, NEVO_CHIPS), SOUND_ITEM_COLLECT);
                    }
                });
            }

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(iCurEnemy == i)
                {
                    if(pEnemy->ReachedDeath())
                    {
                        iCurEnemy += 1u;

                        if(g_pGame->IsTask())
                        {
                            const cPlayer*    pPlayer = pEnemy->LastAttacker();
                            const coreVector2 vDiff   = pEnemy->GetPosition().xy() - pPlayer->GetPosition().xy();

                            if(SameDirection90(vDiff.Normalized(), pEnemy->GetDirection().xy()))   // use old direction
                            {
                                if(++iBackstabCount >= 20u) STAGE_BADGE(2u, BADGE_HARD, pPlayer->GetPosition())
                                else g_pGame->GetCombatText()->DrawCountdown(iBackstabCount, 20u, pPlayer->GetPosition());
                            }
                        }
                    }

                    const coreVector2 vRealDir = MapStepRotated90(vClusterDir, (m_iStageSub - 1u) % 4u);

                    pEnemy->SetPosition (coreVector3(vClusterPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vRealDir,    0.0f));

                    m_Trend.SetPosition (coreVector3(vClusterPos - vRealDir * 0.4f, 0.0f));
                    m_Trend.SetDirection(coreVector3(vRealDir,                      0.0f));
                }
                else
                {
                    pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                }
            });
        }

        STAGE_WAVE(2u, "2-3", {45.0f, 65.0f, 90.0f, 110.0f})   // NEUN
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
            this->DisableBomb(i, false);

        for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
            this->DisableBlast(i, false);

        for(coreUintW i = 0u; i < NEVO_CHIPS; ++i)
            this->DisableChip(i, false);

        this->DisableTrend(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // attack triggers on multiple sides
    // [deprecated] (rotating arrow (90 degree)) (flipping side with each kill) (enemies with 2/3/4 arrows) (big enemy with changing arrow (rotating or flipping) (over time, or after a kill))
    // erste 3 gegner-wellen sind tutorial
    // der letzte gegner sollte von hinten angreifbar sein, weil man dann eh nicht mehr nachkommt
    // sekundär-gegner in 2x2 sollten nicht nach vorne schauen, mann kommt nicht nach nachdem man die primär-gegner entfernt hat, aus selben grund sollten primär-gegner nicht nach hinten schauen
    // gegner bewegungen sollten nicht zu schnell sein, man ist sonst überfordert die direction zu matchen
    // jede sub-gruppe braucht einen default-flow, schnellere flows können bestimmt gefunden werden, aber gar kein flow is orsch
    // enemies are either distributed with 0.2f or 0.5f
    // TASK: some enemies can and should be killed from a different side
    // TASK: never look too long into the same direction
    // TODO 1: hardmode: arrow change direction with tick (visible display), though this is already in the final boss
    // TODO 1: hardmode: jeder gegner hat zwei pfeile (consistent mit der gruppe)
    // TODO 1: 6x6 puzzle anpassen (falls nötig), einzelne pfeile anpassen (falls nötig) (vielleicht auch shiften oder aufteilen oder rotiert infinity nach rechts oder chess-aufteilung (um sie von der kleinen davor zu unterscheiden!))
    // TODO 1: MAIN: task-check, hard idea, regular score, badges, sound, background rota/speed
    // TODO 1: leichter angriff auf easy, vielleicht der einzel-schuss, und auf normal gibts multi-schuss
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreFloat fDistance = 1.1f;

        constexpr coreUint8 aiFake[] = {4u, 18u, 24u, 37u, 47u, 59u, 63u};
        const auto nIsFakeFunc = [&](const coreUint8 iIndex)
        {
            return std::memchr(aiFake, iIndex, ARRAY_SIZE(aiFake));
        };

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

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(1.3f,0.0f), coreVector2(-1.0f,0.0f));
            pPath3->AddStop(coreVector2(0.0f,0.0f), coreVector2(-1.0f,0.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 136u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(4, 0u, COLOR_SHIP_GREEN);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_SECRET);
            });
        });

        constexpr coreUintW iMapSize = 36u;   // max number of concurrent enemies

        STAGE_GET_START(iMapSize + 1u)//3u)
            STAGE_GET_UINT_ARRAY(aiArrowMapRaw, iMapSize)
            STAGE_GET_UINT      (iFakeCount)
            //STAGE_GET_UINT      (iLockRef)
            //STAGE_GET_FLOAT     (fLockTime)
        STAGE_GET_END

        coreUint8* aiArrowMap = r_cast<coreUint8*>(aiArrowMapRaw);
        STATIC_ASSERT(NEVO_ARROWS < 256u)

        const auto nEntryFunc = [](const coreUintW iEnemyIndex, const coreUint8 iStep)
        {
            ASSERT(iStep < 4u)
            return (iEnemyIndex * 4u + iStep) % (iMapSize * 4u);
        };

        const auto nEnableArrowFunc = [&](const coreUintW iArrowIndex, const coreUintW iEnemyIndex, const coreUint8 iStep)
        {
            const cEnemy* pEnemy = pSquad1->GetEnemy(iEnemyIndex);

            this->EnableArrow(iArrowIndex, pEnemy, StepRotated90(iStep));
            aiArrowMap[nEntryFunc(iEnemyIndex, iStep)] = iArrowIndex + 1u;

            if(nIsFakeFunc(iEnemyIndex))
            {
                this->SetArrowFake(iArrowIndex);
                std::memset(&aiArrowMap[nEntryFunc(iEnemyIndex, 0u)], iArrowIndex + 1u, sizeof(coreUint32));
            }
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
                STAGE_RESURRECT(pSquad1, 0u, 3u)
                nEnableArrowFunc(0u, 0u, 0u);
                nEnableArrowFunc(1u, 1u, 0u);
                nEnableArrowFunc(2u, 2u, 0u);
                nEnableArrowFunc(3u, 3u, 0u);
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 4u, 7u)
                nEnableArrowFunc(4u, 4u, 3u);
                nEnableArrowFunc(5u, 5u, 3u);
                nEnableArrowFunc(6u, 6u, 3u);
                nEnableArrowFunc(7u, 7u, 3u);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 8u, 11u)
                nEnableArrowFunc( 8u,  8u, 2u);
                nEnableArrowFunc( 9u,  9u, 2u);
                nEnableArrowFunc(10u, 10u, 2u);
                nEnableArrowFunc(11u, 11u, 2u);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 12u, 23u)
                nEnableArrowFunc(16u, 12u, 3u);
                nEnableArrowFunc(17u, 13u, 3u);
                nEnableArrowFunc(18u, 14u, 3u);
                nEnableArrowFunc(19u, 15u, 2u);
                nEnableArrowFunc(20u, 16u, 2u);
                nEnableArrowFunc(21u, 17u, 2u);
                nEnableArrowFunc(22u, 18u, 0u);
                nEnableArrowFunc(23u, 19u, 0u);
                nEnableArrowFunc(24u, 20u, 0u);
                nEnableArrowFunc(25u, 21u, 3u);
                nEnableArrowFunc(26u, 22u, 3u);
                nEnableArrowFunc(27u, 23u, 3u);
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESURRECT(pSquad1, 24u, 39u)
                nEnableArrowFunc( 0u, 24u, 1u);
                nEnableArrowFunc( 1u, 25u, 1u);
                nEnableArrowFunc( 2u, 26u, 0u);
                nEnableArrowFunc( 3u, 27u, 0u);
                nEnableArrowFunc( 4u, 28u, 1u);
                nEnableArrowFunc( 5u, 29u, 1u);
                nEnableArrowFunc( 6u, 30u, 0u);
                nEnableArrowFunc( 7u, 31u, 0u);
                nEnableArrowFunc( 8u, 32u, 0u);
                nEnableArrowFunc( 9u, 33u, 0u);
                nEnableArrowFunc(10u, 34u, 3u);
                nEnableArrowFunc(11u, 35u, 3u);
                nEnableArrowFunc(12u, 36u, 0u);
                nEnableArrowFunc(13u, 37u, 0u);
                nEnableArrowFunc(14u, 38u, 3u);
                nEnableArrowFunc(15u, 39u, 3u);
            }
            else if(STAGE_SUB(6u))
            {
                STAGE_RESURRECT(pSquad1, 40u, 47u)
                nEnableArrowFunc(16u, 40u, 3u);
                nEnableArrowFunc(17u, 41u, 3u);
                nEnableArrowFunc(18u, 42u, 3u);
                nEnableArrowFunc(19u, 43u, 3u);
                nEnableArrowFunc(20u, 44u, 3u);
                nEnableArrowFunc(21u, 45u, 3u);
                nEnableArrowFunc(22u, 46u, 3u);
                nEnableArrowFunc(23u, 47u, 3u);
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 48u, 63u)
                nEnableArrowFunc( 0u, 48u, 0u);
                nEnableArrowFunc( 1u, 49u, 0u);
                nEnableArrowFunc( 2u, 50u, 3u);
                nEnableArrowFunc( 3u, 51u, 3u);
                nEnableArrowFunc( 4u, 52u, 2u);
                nEnableArrowFunc( 5u, 53u, 1u);
                nEnableArrowFunc( 6u, 54u, 2u);
                nEnableArrowFunc( 7u, 55u, 1u);
                nEnableArrowFunc( 8u, 56u, 3u);
                nEnableArrowFunc( 9u, 57u, 2u);
                nEnableArrowFunc(10u, 58u, 3u);
                nEnableArrowFunc(11u, 59u, 2u);
                nEnableArrowFunc(12u, 60u, 0u);
                nEnableArrowFunc(13u, 61u, 0u);
                nEnableArrowFunc(14u, 62u, 1u);
                nEnableArrowFunc(15u, 63u, 1u);
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 120u, 135u)
                nEnableArrowFunc(22u, 120u, 3u);
                nEnableArrowFunc(23u, 121u, 2u);
                nEnableArrowFunc(24u, 122u, 1u);
                nEnableArrowFunc(25u, 123u, 0u);
                nEnableArrowFunc(26u, 124u, 3u);
                nEnableArrowFunc(27u, 125u, 2u);
                nEnableArrowFunc(28u, 126u, 1u);
                nEnableArrowFunc(29u, 127u, 0u);
                nEnableArrowFunc(30u, 128u, 3u);
                nEnableArrowFunc(31u, 129u, 2u);
                nEnableArrowFunc(32u, 130u, 1u);
                nEnableArrowFunc(33u, 131u, 0u);
                nEnableArrowFunc(34u, 132u, 3u);
                nEnableArrowFunc(35u, 133u, 2u);
                nEnableArrowFunc(36u, 134u, 1u);
                nEnableArrowFunc(37u, 135u, 0u);
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 116u, 117u)
                nEnableArrowFunc(16u, 116u, 3u);
                nEnableArrowFunc(17u, 117u, 3u);
            }
            else if(STAGE_SUB(10u))
            {
                STAGE_RESURRECT(pSquad1, 64u, 79u)
                nEnableArrowFunc( 2u, 64u, 3u);
                nEnableArrowFunc( 3u, 65u, 0u);
                nEnableArrowFunc( 4u, 66u, 2u);
                nEnableArrowFunc( 5u, 67u, 0u);
                nEnableArrowFunc( 6u, 68u, 1u);
                nEnableArrowFunc( 7u, 69u, 0u);
                nEnableArrowFunc( 8u, 70u, 1u);
                nEnableArrowFunc( 9u, 71u, 1u);
                nEnableArrowFunc(10u, 72u, 3u);
                nEnableArrowFunc(11u, 73u, 0u);
                nEnableArrowFunc(12u, 74u, 2u);
                nEnableArrowFunc(13u, 75u, 3u);
                nEnableArrowFunc(14u, 76u, 2u);
                nEnableArrowFunc(15u, 77u, 0u);
                nEnableArrowFunc(18u, 78u, 2u);
                nEnableArrowFunc(19u, 79u, 1u);
            }
            else if(STAGE_SUB(11u))
            {
                STAGE_RESURRECT(pSquad1, 118u, 119u)
                nEnableArrowFunc(0u, 118u, 3u);
                nEnableArrowFunc(1u, 119u, 3u);
            }
            else if(STAGE_SUB(12u))
            {
                STAGE_RESURRECT(pSquad1, 80u, 115u)
                nEnableArrowFunc( 2u,  80u, 3u);
                nEnableArrowFunc( 3u,  81u, 0u);
                nEnableArrowFunc( 4u,  82u, 2u);
                nEnableArrowFunc( 5u,  83u, 0u);
                nEnableArrowFunc( 6u,  84u, 3u);
                nEnableArrowFunc( 7u,  85u, 0u);
                nEnableArrowFunc( 8u,  86u, 2u);
                nEnableArrowFunc( 9u,  87u, 1u);
                nEnableArrowFunc(10u,  88u, 2u);
                nEnableArrowFunc(11u,  89u, 0u);
                nEnableArrowFunc(12u,  90u, 1u);
                nEnableArrowFunc(13u,  91u, 1u);
                nEnableArrowFunc(14u,  92u, 3u);
                nEnableArrowFunc(15u,  93u, 3u);
                nEnableArrowFunc(16u,  94u, 3u);
                nEnableArrowFunc(17u,  95u, 0u);
                nEnableArrowFunc(18u,  96u, 3u);
                nEnableArrowFunc(19u,  97u, 3u);
                nEnableArrowFunc(20u,  98u, 2u);
                nEnableArrowFunc(21u,  99u, 1u);
                nEnableArrowFunc(22u, 100u, 1u);
                nEnableArrowFunc(23u, 101u, 0u);
                nEnableArrowFunc(24u, 102u, 1u);
                nEnableArrowFunc(25u, 103u, 1u);
                nEnableArrowFunc(26u, 104u, 3u);
                nEnableArrowFunc(27u, 105u, 0u);
                nEnableArrowFunc(28u, 106u, 2u);
                nEnableArrowFunc(29u, 107u, 3u);
                nEnableArrowFunc(30u, 108u, 3u);
                nEnableArrowFunc(31u, 109u, 0u);
                nEnableArrowFunc(32u, 110u, 2u);
                nEnableArrowFunc(33u, 111u, 1u);
                nEnableArrowFunc(34u, 112u, 2u);
                nEnableArrowFunc(35u, 113u, 1u);
                nEnableArrowFunc(36u, 114u, 2u);
                nEnableArrowFunc(37u, 115u, 1u);
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + coreVector2(0.0f,30.0f) * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

#if 0
        if(fLockTime >= 0.0f)
        {
            if(m_iArrowActive && (iLockRef != m_iArrowActive))
            {
                iLockRef  = m_iArrowActive;
                fLockTime = 0.0f;
            }

            fLockTime += 1.0f * TIME;

            if(STAGE_CLEARED)
            {
                fLockTime = -1.0f;
                STAGE_BADGE(2u, BADGE_HARD, coreVector3(0.0f,0.0f,0.0f))
            }
            else if(fLockTime >= 3.0f)
            {
                fLockTime = -1.0f;
            }
            else
            {
                g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.1f", MAX0(2.9f - FloorFactor(fLockTime, 10.0f))), coreVector3(0.0f,0.0f,0.0f), COLOR_MENU_INSIDE);
            }
        }
#endif

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreFloat fWaveOffset = 0.0f;

            if(i < 40u)
            {
                STAGE_LIFETIME(pEnemy, 0.5f, (i < 12u) ? (0.21f * fDistance * I_TO_F(i % 4u)) : ((i < 24u) ? (0.21f * fDistance * I_TO_F((i - 12u) % 3u) + 0.65f * I_TO_F((i - 12u) / 3u)) : (0.5f * I_TO_F((i - 24u) / 4u) + ((i < 32u) ? 0.0f : 0.28f))))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 8u) ? 0.5f : ((i < 12u) ? 0.0f : ((i < 24u) ? ((I_TO_F((((i - 12u) / 3u) * 2u + 1u) % 5u) - 1.5f) * 0.5f) : ((I_TO_F(((i - 24u) / 4u) % 2u) - 0.5f) * ((i < 32u) ? 0.8f : -0.8f) + (I_TO_F((i - 24u) % 4u) - 1.5f) * 0.2f * fDistance))), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  4u) pEnemy->Rotate270();
                else if(i <  8u) {}
                else if(i < 12u) pEnemy->Rotate180();
                else if(i < 24u) pEnemy->Rotate90 ();
                else if(i < 40u) {}

                     if(i < 12u) fWaveOffset = I_TO_F((i)       / 4u);
                else if(i < 24u) fWaveOffset = I_TO_F((i - 12u) / 3u);
                else if(i < 40u) fWaveOffset = I_TO_F((i - 24u) / 4u);
            }
            else if(i < 64u)
            {
                STAGE_LIFETIME(pEnemy, 0.5f, (i < 48u) ? (0.325f * I_TO_F(i - 40u)) : (0.65f * I_TO_F((i - 48u) / 4u) + ((((i - 48u) % 4u) < 2u) ? 0.0f : (0.21f * fDistance))))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((i < 48u) ? ((((i - 40u) / 2u) % 2u) ? -1.0f : 1.0f) : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2((i < 48u) ? (((i - 40u) % 2u) ? 0.25f : -0.75f) * (2.0f/3.0f) : ((I_TO_F((i - 48u) % 2u) + ((((i - 48u) / 4u) % 2u) ? 1.5f : -2.5f)) * 0.2f * fDistance), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 48u) pEnemy->Rotate270();
                else if(i < 64u) pEnemy->Rotate180();

                     if(i < 48u) fWaveOffset = I_TO_F((i - 40u) / 1u);
                else if(i < 64u) fWaveOffset = I_TO_F((i - 48u) / 4u);
            }
            else if(i < 80u)
            {
                STAGE_LIFETIME(pEnemy, 2.1f, 0.0f)

                const coreFloat fBreath = 0.2f * fDistance + 0.03f * (0.5f + 0.5f * SIN(m_fStageSubTime * 2.0f + I_TO_F(i)));

                const coreVector2 vFactor = coreVector2((((i - 64u) % 4u) < 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F((i - 64u) % 4u) - 1.5f, I_TO_F((i - 64u) / 4u) - 1.5f) * fBreath;

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset, fLifeTime);   // # no factor multiplication
            }
            else if(i < 116u)
            {
                STAGE_LIFETIME(pEnemy, 2.1f, 0.0f)

                const coreFloat fBreath = 0.2f * fDistance + 0.03f * (0.5f + 0.5f * SIN(m_fStageSubTime * 2.0f + I_TO_F(i)));

                const coreVector2 vFactor = coreVector2((((i - 80u) % 6u) < 3u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F((i - 80u) % 6u) - 2.5f, I_TO_F((i - 80u) / 6u) - 2.5f) * fBreath;

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset, fLifeTime);   // # no factor multiplication
            }
            else if(i < 120u)
            {
                STAGE_LIFETIME(pEnemy, 2.1f, 0.0f)

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F(i % 2u) * 0.2f * fDistance + 0.7f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 0.8f, (3.0f * 2.6f / 16.0f) * I_TO_F(i - 120u))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.8f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if((i - 120u) % 4u == 0u) {}
                else if((i - 120u) % 4u == 1u) pEnemy->Rotate90 ();
                else if((i - 120u) % 4u == 2u) pEnemy->Rotate180();
                else if((i - 120u) % 4u == 3u) pEnemy->Rotate270();
            }

            if(i < 64u) pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + pEnemy->GetDirection().xy().Rotated90() * (3.0f * SIN(m_fStageSubTime * 2.0f + fWaveOffset)), 0.0f));
        });

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS, COLL_VAL(pSquad1), COLL_VAL(aiArrowMap), COLL_REF(iFakeCount), COLL_VAL(nEntryFunc), COLL_VAL(pHelper))
        {
            if(!bFirstHit) return;

            const coreUintW   iEnemyIndex = pSquad1->GetIndex(pEnemy);
            const coreVector2 vSide       = AlongCrossNormal(pBullet->GetFlyDir());
            const coreUint8   iPack       = PackDirection(vSide);

            const coreUint8& iEntry = aiArrowMap[nEntryFunc(iEnemyIndex, iPack / 2u)];
            if(iEntry)
            {
                const coreUintW iArrowIndex = iEntry - 1u;

                this->DisableArrow(iArrowIndex, true);

                pEnemy->Kill(true);
                pEnemy->ApplyScore(d_cast<cPlayer*>(pBullet->GetOwner()));

                std::memset(&aiArrowMap[nEntryFunc(iEnemyIndex, 0u)], 0, sizeof(coreUint32));

                if(!g_pGame->IsEasy())
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = -m_aArrowRaw[iArrowIndex].GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.8f);
                }

                if(this->GetArrowFake(iArrowIndex) && (iPack != this->GetArrowDir(iArrowIndex)))
                {
                    if(++iFakeCount >= ARRAY_SIZE(aiFake)) STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                    else g_pGame->GetCombatText()->DrawProgress(iFakeCount, ARRAY_SIZE(aiFake), pEnemy->GetPosition());
                }

                if(iEnemyIndex == 77u)
                {
                    pHelper->Resurrect(true);
                    pHelper->SetPosition(pEnemy->GetPosition());
                }
            }

            pBullet->Deactivate(true);
            pBullet->AddStatus(BULLET_STATUS_GHOST);
        });

        cSeaBackground* pBackground = d_cast<cSeaBackground*>(g_pEnvironment->GetBackground());

        if(STAGE_BEGINNING) pBackground->GetOutdoor()->LerpHeight(0.2f, -17.0f);

        const coreFloat fEnvLerp = pBackground->GetOutdoor()->GetLerp();

        pBackground->SetGroundDensity(1u, STEP(0.5f, 1.0f, 1.0f - fEnvLerp));
        pBackground->SetGroundDensity(2u, STEP(0.5f, 1.0f, 1.0f - fEnvLerp));
        pBackground->SetGroundDensity(3u, STEP(0.5f, 1.0f, 1.0f - fEnvLerp));
        pBackground->SetGroundDensity(4u, STEP(0.5f, 1.0f, fEnvLerp));
        pBackground->SetGroundDensity(5u, STEP(0.5f, 1.0f, fEnvLerp));

        STAGE_WAVE(3u, "2-4", {50.0f, 75.0f, 100.0f, 125.0f})   // ZEHN
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
            this->DisableArrow(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u, 5u})
    {
        cSeaBackground* pBackground = d_cast<cSeaBackground*>(g_pEnvironment->GetBackground());

        pBackground->GetOutdoor()->LerpHeightNow(0.2f, -17.0f);
        pBackground->SetGroundDensity(1u, 0.0f);
        pBackground->SetGroundDensity(2u, 0.0f);
        pBackground->SetGroundDensity(3u, 0.0f);
        pBackground->SetGroundDensity(4u, 1.0f);
        pBackground->SetGroundDensity(5u, 1.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // dicke kugerl die am rand bleiben
    // [deprecated] - all: bullet speed is much lower than enemy speed, to create a perceptible path for the player to use
    // [deprecated] - all: bullet frequency is not too high, to better distinguish individual bullets
    // [deprecated] - all: enemy attack begins later, to not slow down bullets immediately after shooting
    // [deprecated] - 1,2: enemy path is not too wide, to not slow down bullets immediately after shooting
    // bullets vergrößern mit angriff is orsch, du kannst nicht schießen während du versuchst zwischen geschossen durch zu gehen, aber dadurch bist du zu schnell, schiff wegdrehen is un-intuitiv, bullets zerstören ab größe is 1-dimensional (vergrößern sollte eher für die andere gruppe aufgehoben werden (mit gegnern))
    // gruppen die danach kommen, können/sollten von den angriffen der gruppe davor verdeckt werden
    // derzeitiger fokus liegt darauf den spieler sich vorsichtig durch die masse bewegen zu lassen, wie durch wasser schwimmen
    // gegner die sich gegenüber sind sollen den spieler dazu bringen hin un her zu drehen, um den einen anzugreifen, und die angriffe des anderen durchdringbar zu machen
    // gegner die geschosse hinter sich herziehen haben fokus auf flow
    // die vertikalen linien sollen den spieler dazu bringen 90grad durch zu fliegen
    // gegner die geschosse erzeugen sind angreifbar aufgrund der range (gegner und bullets) und ihrer bewegung
    // grow-bullets aus gegner sollten mit 0.0f statt 1.0 starten, damit man die gegner noch sieht und angriffe inder nähe besser durchkommen
    // TASK: destroy all orange balls
    // TASK: reach a set of specific points
    // TASK: move between the two big spheres
    // TODO 1: hardmode: maybe all bullets get gravity, "flooding" the bottom (ZeroRanger kugerl-auge-boss)
    // TODO 1: hardmode: they bounce once
    // TODO 1: hardmode: große kugerl explodieren in viele kleine
    // TODO 1: hardmode: 3 rotieren vom center nach außen
    // TODO 1: move stuff from here (and Leviathan) into grow-bullet ?
    // TODO 1: MAIN: regular score, auf boss übertragen (general, easy, coop), sound, background rota/speed
    // TODO 1: badge durch big wurde nicht erkannt, in dem moment wo ich getroffen wurde, kann es sein, dass OldPos+FlyMove dann kaputt sind wenn die zeit einfriert ?
    STAGE_MAIN({TAKE_ALWAYS, 4u})
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
            pPath2->AddNode(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,0.0f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(50, 0u, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_GET_START(5u)
            STAGE_GET_UINT(iBubbleMap)
            STAGE_GET_UINT(iBubbleTick)
            STAGE_GET_UINT(iBubbleCount)
            STAGE_GET_UINT(iHelperData)
            STAGE_GET_UINT(iMarkerMap)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  3u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 12u, 12u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 13u, 13u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 14u, 14u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 15u, 16u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 17u, 18u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 19u, 19u)

            iBubbleTick = UINT32_MAX;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_BLUE);

        const coreFloat fBulletSpeed = g_pGame->IsEasy() ? 0.8f : 1.0f;
        const coreFloat fAttackSpeed = g_pGame->IsEasy() ? 0.5f : 1.0f;

        const auto nCreateBubbleFunc = [&](const coreFloat fSpeed, cShip* pOwner, const coreVector2 vPosition, const coreVector2 vDirection, const coreUintW iBubbleIndex = SIZE_MAX, const coreUint32 iBubbleDelay = 0u, const coreUint32 iBubbleDelayEasy = 0u)
        {
            cGrowBullet* pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cGrowBullet>(5, fSpeed * fBulletSpeed, pOwner, vPosition, vDirection);

            if(g_pGame->IsTask() && (iBubbleIndex != SIZE_MAX) && !HAS_BIT(iBubbleMap, iBubbleIndex))
            {
                if(iBubbleTick == UINT32_MAX)
                {
                    iBubbleTick = g_pGame->IsEasy() ? iBubbleDelayEasy : iBubbleDelay;
                }

                if(!(iBubbleTick--))
                {
                    pBullet->MakeOrange();
                    pBullet->SetDamage(10);

                    ADD_BIT(iBubbleMap, iBubbleIndex)
                }
            }

            return pBullet;
        };

        const auto nCreateHelperFunc = [&](const coreFloat fSpeed, cShip* pOwner, const coreVector2 vPosition, const coreVector2 vDirection)
        {
            cGrowBullet* pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cGrowBullet>(5, fSpeed * fBulletSpeed, pOwner, vPosition, vDirection);

            if(!HAS_BIT(iHelperData, 0u))
            {
                pHelper->Resurrect(true);
                pBullet->SetDamage(2);

                ADD_BIT(iHelperData, 0u)
            }

            return pBullet;
        };

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            if((pHelper->GetLifeTime() > 5.0f) && !g_pForeground->IsVisibleObject(pHelper))
            {
                pHelper->Kill(false);
            }
        }

        if(g_pGame->IsTask() && (m_iStageSub == 5u))
        {
            constexpr coreVector2 avMarker[] = {coreVector2(-2.5f,-0.5f), coreVector2(-1.5f,-2.5f), coreVector2(-0.5f,2.5f), coreVector2(0.5f,0.5f), coreVector2(1.5f,-1.5f), coreVector2(2.5f,1.5f)};

            for(coreUintW i = 0u; i < ARRAY_SIZE(avMarker); ++i)
            {
                const coreVector2 vMarkerPos = avMarker[i] * 0.3f * FOREGROUND_AREA;

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = vMarkerPos - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(5.0f))
                    {
                        ADD_BIT(iMarkerMap, i)
                    }
                });

                if(!HAS_BIT(iMarkerMap, i)) g_pGame->GetCombatText()->AttachMarker(i, "X", coreVector3(vMarkerPos, 0.0f), COLOR_MENU_INSIDE);

                if(iMarkerMap == BITLINE(ARRAY_SIZE(avMarker)))
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, coreVector3(vMarkerPos, 0.0f))
                }
            }
        }

        if(m_iStageSub == 5u)
        {
            if(STAGE_TICK_FREE2(1.5f * fAttackSpeed, 0.0f))
            {
                const coreVector2 vPos = coreVector2( 1.2f,0.4f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(-1.0f,0.0f);

                nCreateHelperFunc(0.9f, pSquad1->GetEnemy(0u),  vPos,  vDir)->ChangeSize(1.0f);   // #
                nCreateBubbleFunc(0.9f, pSquad1->GetEnemy(0u), -vPos, -vDir)->ChangeSize(1.0f);
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(STAGE_TICK_FREE2(1.7f * fAttackSpeed, 0.0f))
            {
                for(coreUintW i = 0u; i < 10u; ++i)
                {
                    if((i % 3u) != (s_iTick % 3u)) continue;

                    const coreVector2 vPos = coreVector2((I_TO_F(i) - 4.5f) * 0.22f, 1.2f) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                    nCreateBubbleFunc(0.9f, pSquad1->GetEnemy(0u), vPos, vDir, 0u, 7u, 7u)->ChangeSize(1.0f);
                }
            }
        }
        else if(m_iStageSub == 12u)
        {
            if(STAGE_BEGINNING2)
            {
                const coreVector2 vPos = coreVector2(0.0f, 1.5f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);
                const coreVector2 vTan = vDir.Rotated90() * 10.3f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cGrowBullet>(1, 0.9f * fBulletSpeed, pSquad1->GetEnemy(0u), vPos + vTan, vDir)->ChangeSize(5.0f)->ChangeTexSize(2.5f)->AddStatus(BULLET_STATUS_IMMORTAL);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cGrowBullet>(1, 0.9f * fBulletSpeed, pSquad1->GetEnemy(0u), vPos - vTan, vDir)->ChangeSize(5.0f)->ChangeTexSize(2.5f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i == 14u) ? 0.5f : 0.7f, (i < 6u) ? 0.0f : ((i < 8u) ? 1.0f : ((i < 12u) ? (0.3f * I_TO_F(i - 8u) + 0.5f) : ((i < 19u) ? 0.0f : 1.0f))))

            if(i < 2u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                if(i == 1u) pEnemy->Rotate90();

                if(STAGE_TICK_LIFETIME(6.0f * fAttackSpeed, 0.0f) && (((s_iTick + 7u) % 8u) < ((i == 0u) ? 1u : 2u)))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle() + ((i == 0u) ? 0.0f : DEG_TO_RAD((s_iTick % 2u) ? 7.0f : -7.0f));

                    for(coreUintW j = 5u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 28.0f) + fBase);

                        nCreateBubbleFunc(0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f);
                    }
                }
            }
            else if(i < 6u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 4u) ? 0.5f : -0.5f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 2u || i == 4u) pEnemy->Rotate270();
                else if(i == 3u || i == 5u) pEnemy->Rotate180();

                if(STAGE_TICK_LIFETIME(8.0f * fAttackSpeed, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90() * ((s_iTick % 2u) ? -1.0f : 1.0f);

                    nCreateBubbleFunc(0.4f, pEnemy, vPos, vDir, (i < 4u) ? 1u : 2u, (i < 4u) ? 17u : 19u, (i < 4u) ? 9u : 7u)->ChangeSize(0.0f);
                }
            }
            else if(i < 12u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 8u) ? 0.8f : (I_TO_F(i - 8u) * 0.3f - 0.9f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 6u)             pEnemy->Rotate270();
                else if(i == 7u)             pEnemy->Rotate90 ();
                else if(i == 8u || i == 10u) pEnemy->Rotate90 ()->InvertX();
                else if(i == 9u || i == 11u) pEnemy->Rotate90 ();
            }
            else if(i < 14u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 13u) ? -0.5f : 0.5f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(STAGE_TICK_LIFETIME(1.3f * fAttackSpeed, -0.4f))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = (s_iTick % 2u) ? 18.0f : 0.0f;

                    for(coreUintW j = 10u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f + fBase));

                        nCreateBubbleFunc(0.4f, pEnemy, vPos, vDir)->ChangeSize(0.0f);
                    }
                }
            }
            else if(i < 15u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (fLifeTime * 5.0f);   // affects bullet direction

                if(STAGE_TICK_LIFETIME(12.0f * fAttackSpeed, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90() * ((s_iTick % 2u) ? -1.0f : 1.0f);

                    nCreateBubbleFunc(0.4f, pEnemy, vPos, vDir, 3u, 10u, 4u)->ChangeSize(0.0f);
                }
            }
            else if(i < 17u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 15u) pEnemy->Rotate90 ();
                else if(i == 16u) pEnemy->Rotate270();

                if(STAGE_TICK_LIFETIME(3.0f * fAttackSpeed, 0.0f) && (((s_iTick + ((i == 15u) ? 4u : 0u)) % 8u) == 0u))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();

                    for(coreUintW j = 10u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 4.5f) * 14.0f) + fBase);

                        nCreateBubbleFunc(0.9f, pEnemy, vPos, vDir, 4u, 14u, 14u)->ChangeSize(0.0f);
                    }
                }
            }
            else if(i < 19u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.8f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 17u) pEnemy->Rotate90 ();
                else if(i == 18u) pEnemy->Rotate270();

                if(STAGE_TICK_LIFETIME(4.0f * fAttackSpeed, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual((i == 17u) ? 1u : 0u).Normalized();

                    nCreateBubbleFunc(0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f);
                }
            }
            else if(i < 20u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }
        });

        cBullet* apBig[2] = {};

        const coreBool bEffectTick = STAGE_TICK_FREE(30.0f, 0.0f);

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cGrowBullet>([&](cGrowBullet* OUTPUT pBullet)
        {
            const coreInt32 iDamage = pBullet->GetDamage();

            if(iDamage > 5)
            {
                if(iDamage > (10 + 20 * g_pGame->GetNumPlayers()))
                {
                    pBullet->Deactivate(true);

                    if(++iBubbleCount >= 5u) STAGE_BADGE(2u, BADGE_HARD, pBullet->GetPosition())
                    else g_pGame->GetCombatText()->DrawProgress(iBubbleCount, 5u, pBullet->GetPosition());

                    g_pSpecialEffects->CreateSplashColor(pBullet->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_ORANGE);
                    g_pSpecialEffects->PlaySound(pBullet->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
                }

                if(bEffectTick)
                {
                    const coreVector2 vPos = pBullet->GetPosition().xy() + StepRotated45(s_iTick % 8u) * pBullet->GetVisualRadius();
                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 1u, COLOR_ENERGY_ORANGE);
                }
            }
            else if(iDamage == 1)
            {
                apBig[apBig[0] ? 1u : 0u] = pBullet;
            }
            else if(iDamage == 2)
            {
                pHelper->SetPosition(pBullet->GetPosition() + coreVector3(pBullet->GetFlyMove(), 0.0f));
            }
        });

        if(g_pGame->IsTask() && apBig[0] && apBig[1])
        {
            const coreVector2 vNorm = (apBig[1]->GetPosition().xy() - apBig[0]->GetPosition().xy()).Normalized();

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreVector2 vDiff1   = pPlayer->GetPosition().xy() - apBig[0]->GetPosition().xy();
                const coreVector2 vDiff2   = pPlayer->GetPosition().xy() - apBig[1]->GetPosition().xy();
                const coreVector2 vDiffOld = pPlayer->GetOldPos()        - apBig[0]->GetPosition().xy() + apBig[0]->GetFlyMove();

                const coreFloat fDot    = coreVector2::Dot(vDiff1,   vNorm.Rotated90());
                const coreFloat fDotOld = coreVector2::Dot(vDiffOld, vNorm.Rotated90());

                const coreFloat fSide1 = coreVector2::Dot(vDiff1, vNorm);
                const coreFloat fSide2 = coreVector2::Dot(vDiff2, vNorm);

                if((SIGN(fDot) != SIGN(fDotOld)) && (SIGN(fSide1) != SIGN(fSide2)))
                {
                    STAGE_BADGE(0u, BADGE_EASY, coreVector3(pPlayer->GetPosition().xy(), 0.0f))
                }
            });
        }

        STAGE_WAVE(4u, "2-5", {50.0f, 75.0f, 100.0f, 125.0f})   // ELF
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
    // occupy areas with energy
    // rotation der großen dreiecke entgegen der flug-drehrichtung (auch gruppe 1)
    // flug-rotation um bewegung gefährlicher zu machen und gegen safe-spots
    // erste welle so aufbauen um dreiecke möglichst gleichmäßig zu verteilen, spieler kann trotzdem überlappen, könnte aber zeit kosten
    // hohe geschwindigkeit erhöht intensity und aufmerksamkeit
    // wenn flug-drehrichtung anfängt, zweite welle von der seite kommen, von wo die großen dreiecke kommen werden
    // TODO 1: helfer spawnt und fliegt in Ntem teleportiertem dreieck, verschwindet nach nächster teleportation, kleiner funken-effekt bei kill (falls doch noch sichtbar)
    // TODO 1: something after the last wave (changing the triangles (size, movement), or the number, or ...), or start ? hmmm, no I want to start up quick
    // TODO 1: also focus on difference from d/generate wave, make area strongly occupied ?
    // TODO 1: create an absurd big object/triangle in the middle
    // TODO 1: gegner-masse kommt auf spieler zu, alle mit energy, man muss loch rein schießen
    // TODO 1: small enemy triangles stay (maybe only for hard)
    // TODO 1: keep triangles from growing with attacks
    // TODO 1: movement only from right to left
    // TODO 1: (previously bullet shrink+grow, mentioned in bottom of Waves page)
    // TODO 1: gegner versuchen dich in mustern zu berühren, schlange,
    // TODO 1: frei gewordene objekte fliegen auf spieler zu oder in kreiselbewegung nach außen
    // TODO 1: triangles, spheres, quads, all have different (basic) behavior
    // TODO 1: MAIN: task-check, helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        constexpr coreUintW iBig = 0u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.4f), coreVector2(0.0f,-1.0f));         
            pPath1->AddNode(coreVector2(0.0f,-1.4f), coreVector2(0.0f,-1.0f));        
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.0f, 1.4f), coreVector2(1.0f,-1.0f));
            pPath2->AddNode(coreVector2( 1.0f,-1.4f), coreVector2(1.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(-0.8f, 1.4f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2( 1.4f,-0.8f), coreVector2(1.0f, 0.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 124u)                            
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, 0u, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_TOP);
            });
        });

        STAGE_GET_START(NEVO_BLOCKS * 3u + 3u)
            STAGE_GET_VEC2_ARRAY (avMergePos,  NEVO_BLOCKS)
            STAGE_GET_FLOAT_ARRAY(afMergeTime, NEVO_BLOCKS)
            STAGE_GET_FLOAT      (fMoveSpeed)
            STAGE_GET_FLOAT      (fMoveAngle)
            STAGE_GET_UINT       (iChargeCount)
        STAGE_GET_END

        const auto nForEachBlockFunc = [this](const coreBool bOnlyDead, auto&& nCallback)   // [](coreObject3D* OUTPUT pBlock, const cShip* pOwner, const coreUintW i) -> void
        {
            for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
            {
                coreObject3D* pBlock = (*m_Block.List())[i];
                if(!pBlock->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const cShip* pOwner = m_apBlockOwner[i];
                if(!pOwner || (bOnlyDead && !pOwner->HasStatus(ENEMY_STATUS_DEAD))) continue;

                nCallback(pBlock, pOwner, i);
            }
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 24u, 29u)
            
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 30u, 33u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 34u, 37u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 38u, 41u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 42u, 47u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 16u, 19u)
            
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1,  48u, 55u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1,  56u, 58u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1,  59u, 64u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1,  65u, 72u)
            
            //else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 20u, 23u)
            else if(STAGE_SUB(100u))          
            {
                for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
                    this->DisableBlock(i, true);

                g_pGame->GetHelper(ELEMENT_ORANGE)->Kill(true);
            }

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(!pEnemy->ReachedDeath())
                {
                    const coreUintW iIndex = (i % (NEVO_BLOCKS - 1u)) + 1u;

                    afMergeTime[iIndex] = 0.0f;

                    this->EnableBlock(iIndex, pEnemy, 6.0f);//(i < 4u) ? 10.0f : 5.0f);
                    m_afBlockRota[iIndex] = (0.2f*PI) * m_fStageTime;
                }
            });
        }

        if(m_iStageSub >= 8u)
        {
            if(m_iStageSub < 14u) fMoveSpeed = MIN1(fMoveSpeed + 0.5f * TIME);
                             else fMoveSpeed = MAX0(fMoveSpeed - 0.5f * TIME);

            if(fMoveSpeed)
            {
                fMoveAngle = fMoveAngle - 0.2f * TIME    * 0.35f;

                const coreVector2 vMove = coreVector2::Direction(fMoveAngle) * (55.0f * TIME * fMoveSpeed)    * 0.35f;

                nForEachBlockFunc(true, [&](coreObject3D* OUTPUT pBlock, const cShip* pOwner, const coreUintW i)
                {
                    coreVector2 vNewPos = pBlock->GetPosition().xy() + vMove;

                         if(vNewPos.x < -FOREGROUND_AREA.x * 1.45f) vNewPos.x += FOREGROUND_AREA.x * 2.9f;
                    else if(vNewPos.x >  FOREGROUND_AREA.x * 1.45f) vNewPos.x -= FOREGROUND_AREA.x * 2.9f;
                         if(vNewPos.y < -FOREGROUND_AREA.y * 1.45f) vNewPos.y += FOREGROUND_AREA.y * 2.9f;
                    else if(vNewPos.y >  FOREGROUND_AREA.y * 1.45f) vNewPos.y -= FOREGROUND_AREA.y * 2.9f;

                    pBlock->SetPosition(coreVector3(vNewPos, 0.0f));
                });
            }
        }

        if(m_iStageSub < 14u)
        {
        }
        else if(m_iStageSub < 100u)
        {
            coreObject3D* pBigBlock = (*m_Block.List())[iBig];

            if(m_iStageSub == 14u)
            {
                if(STAGE_BEGINNING2)
                {
                    for(coreUintW i = 1u; i < 8u; ++i)
                    {
                        afMergeTime[i + 1u] = -I_TO_F(i) / 7.0f;
                    }
                }

                if(STAGE_SUBTIME_POINT(2.0f))
                {
                    this->EnableBlock(iBig, pSquad1->GetEnemy(0u), 20.0f);
                    pBigBlock->SetPosition(coreVector3(0.0f,0.0f,0.0f));

                    g_pSpecialEffects->MacroExplosionColorBig(coreVector3(0.0f,0.0f,0.0f), COLOR_ENERGY_ORANGE);
                }
            }

            nForEachBlockFunc(true, [&](coreObject3D* OUTPUT pBlock, const cShip* pOwner, const coreUintW i)
            {
                if(i == iBig) return;

                coreVector2& vPos  = avMergePos [i];
                coreFloat&   fTime = afMergeTime[i];

                if(fTime <= 0.0f) vPos = pBlock->GetPosition().xy();
                fTime += 1.0f * TIME;

                m_afBlockScale[i]  = LERPBR(5.0f, 2.0f, CLAMP01(fTime));
                m_afBlockRota [i] += LERP(0.0f, 10.0f, CLAMP01(fTime)) * TIME;

                const coreVector2 vNewPos = MapToAxis(vPos * ParaLerp(1.0f, 1.1f, 0.0f, (CLAMP01(fTime))), coreVector2::Direction(LERPBR(0.0f*PI, -0.5f*PI, CLAMP01(fTime))));

                pBlock->SetPosition(coreVector3(vNewPos, 0.0f));

                if(fTime >= 1.0f)
                {
                    this->DisableBlock(i, true);

                    if(pBigBlock->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                    {
                        iChargeCount += 1u;
                    }
                }
            });

            if(pBigBlock->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
            {
                m_afBlockScale[iBig] = 20.0f + I_TO_F(iChargeCount) * 1.0f;
            }
        }

        nForEachBlockFunc(false, [&](coreObject3D* OUTPUT pBlock, const cShip* pOwner, const coreUintW i)
        {
            const coreBool bDead = pOwner->HasStatus(ENEMY_STATUS_DEAD);

            if(!bDead)
            {
                pBlock->SetPosition(pOwner->GetPosition());
            }

            m_afBlockRota[i] += (0.2f*PI) * TIME;
            
            
                if(i != iBig) m_afBlockScale[i] = 1.0f + 5.0f * (0.5f + 0.5f * SIN(m_fStageTime * 3.0f));

            if(!bDead || (afMergeTime[i] <= 0.0f))
            {
                cPlayer::TestCollision(PLAYER_TEST_NORMAL, pBlock, [](cPlayer* OUTPUT pPlayer, const coreObject3D* pBlock, const coreVector3 vIntersection, const coreBool bFirstHit)
                {
                    if(!bFirstHit) return;

                    pPlayer->TakeDamage(10, ELEMENT_ORANGE, vIntersection.xy());
                    g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_ORANGE);
                });
            }
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 4u || (i >= 12u && i < 24u))
            {
                STAGE_LIFETIME(pEnemy, 0.8f, 0.0f)

                const coreFloat   fSide = ((i / 4u) % 2u) ? 1.0f : -1.0f;
                const coreFloat   fVal  = fLifeTime * 1.8f;
                const coreVector2 vDir  = coreVector2::Direction((fVal - I_TO_F(i % 4u) * ((i < 12u) ? (0.5f*PI) : (0.5f*PI))) * fSide);
                const coreFloat   fLen  = LERPB(1.3f, 0.1f, MIN1(fLifeTime * 0.2f)) * FOREGROUND_AREA.x;

                pEnemy->SetPosition (coreVector3(vDir * fLen,               0.0f));
                pEnemy->SetDirection(coreVector3(vDir.Rotated90() * -fSide, 0.0f));
            }
            else if(i < 12u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.5f * I_TO_F(i % 4u))// + ((i < 8u) ? 3.0f : 0.0f))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f, (i < 8u) ? 1.0f : -1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f) * -0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->Rotate270();
            }
            else if(i < 30u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.5f * I_TO_F((i - 24u) % 6u))

                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((I_TO_F(i % 6u) - 2.5f) / 2.5f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 42u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.5f * I_TO_F((i - 30u) % 4u))

                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 34u) {}
                else if(i < 38u) pEnemy->Rotate90 ();
                else if(i < 42u) pEnemy->Rotate270();
            }
            else if(i < 48u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

                const coreFloat fLen = FmodRange(-1.3f * SQRT2 + MAX0(fLifeTime), -1.3f * SQRT2, 1.3f * SQRT2);
                const coreFloat fOff = (I_TO_F(i - 42u) - 2.5f) * 0.25f;

                const coreVector2 vDir = coreVector2::Direction(fMoveAngle);   // TODO 1: cache outside?
                const coreVector2 vPos = vDir * fLen + vDir.Rotated90() * fOff;

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }
            else if(i < 56u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.3f * I_TO_F((i - 48u) % 8u))

                const coreFloat fLen = FmodRange(MAX0(fLifeTime), 0.0f, 1.3f * SQRT2);

                const coreVector2 vDir = coreVector2::Direction(I_TO_F(((i - 48u) * 3u) % 8u) * (0.25f*PI));
                const coreVector2 vPos = STAGE_LIFETIME_BEFORE(0.0f) ? HIDDEN_POS : (vDir * fLen);

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }
            else if(i < 65u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

                const coreFloat fLen = FmodRange(fLifeTime * 0.3f, 0.0f, 1.3f * SQRT2);

                const coreVector2 vDir = coreVector2::Direction(fLifeTime * (2.0f*PI) + I_TO_F(i - 56u) * (2.0f*PI) / ((i < 59u) ? 3.0f : 6.0f));
                const coreVector2 vPos = vDir * fLen;

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }
            else if(i < 73u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.3f * I_TO_F((i - 48u) % 8u))

                const coreFloat fLen = FmodRange(MAX0(fLifeTime), 0.0f, 1.3f * SQRT2);

                const coreVector2 vDir = coreVector2::Direction(pEnemy->GetLifeTime() * (0.25f*PI));
                const coreVector2 vPos = STAGE_LIFETIME_BEFORE(0.0f) ? HIDDEN_POS : (vDir * fLen);

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }
        });

        STAGE_WAVE(5u, "2-?", {60.0f, 80.0f, 100.0f, 120.0f})   // ZWÖLF
    });

    // ################################################################
    // reset helper
    if(false) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
            this->DisableBlock(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // demo
    if(g_bDemoVersion) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        cTracker* pTracker = g_pGame->GetTracker();

        if(STAGE_BEGINNING)
        {
            pTracker->Resurrect();
            pTracker->EnableWind();
            pTracker->EnableRange();
            pTracker->SetPosition(coreVector3(0.0f,1.5f,0.0f) * FOREGROUND_AREA3);
            pTracker->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        }

        if(STAGE_TIME_POINT(2.0f))
        {
            g_pGame->FadeMusic(0.35f);

            m_pNightmareSound->PlayRelative(this, 1.0f, 1.0f, false, SOUND_EFFECT);
        }

        if(STAGE_TIME_POINT(3.0f))
        {
            g_pGame->GetInterface ()->SetVisible(false);
            g_pGame->GetCombatText()->SetVisible(false);
        }

        if(STAGE_TIME_BEFORE(5.0f))
        {
            g_pPostProcessing->SetChroma(STEP(3.0f, 5.0f, m_fStageTime) * 2.0f);

            pTracker->SetPosition(coreVector3(0.0f, LERPB(1.5f, 0.7f, STEP(2.0f, 4.0f, m_fStageTime)), 0.0f) * FOREGROUND_AREA3);
        }
        else if(STAGE_TIME_POINT(5.3f))
        {
            g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,-1.0f));
        }
        else if(STAGE_TIME_POINT(5.6f))
        {
            pTracker->Kill(false);

            g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,1.0f));

            g_pEnvironment->ChangeBackground(cDarkBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);
            g_pEnvironment->SetTargetSpeedNow(0.0f);

            Core::Manager::Resource->UpdateResources();
            Core::Manager::Resource->UpdateFunctions();

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
                pPlayer->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            });

            g_pGame->GetBulletManagerPlayer()->ClearBullets(false);
        }
        else if(STAGE_TIME_POINT(5.9f))
        {
            g_pPostProcessing->SetChroma(0.0f);

            g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

            this->EnableRanges();
        }

        else if(STAGE_TIME_POINT(6.7f))
        {
            g_pPostProcessing->SetChroma(1.5f);

            g_pEnvironment->ChangeBackground(cDarkBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

            Core::Manager::Resource->UpdateResources();
            Core::Manager::Resource->UpdateFunctions();
        }
        else if(STAGE_TIME_POINT(7.0f))
        {
            g_pPostProcessing->SetChroma(0.0f);

            this->DisableRanges(false);

            g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);
            g_pEnvironment->SetTargetSpeedNow(6.0f);

            Core::Manager::Resource->UpdateResources();
            Core::Manager::Resource->UpdateFunctions();

            cPlayer* pPlayer = g_pGame->GetPlayer(0u);

            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
            pPlayer->SetMoveSpeed(0.0f);
            pPlayer->SetPosition (coreVector3(0.0f,-0.5f,0.0f) * FOREGROUND_AREA3);
            pPlayer->SetDirection(coreVector3(0.0f, 1.0f,0.0f));

            if(m_pNightmareSound->EnableRef(this)) m_pNightmareSound->Stop();

            m_Leviathan.ResurrectDemo();
        }
        else if(STAGE_TIME_AFTER(7.4f))
        {
            if(m_Leviathan.HasStatus(ENEMY_STATUS_DEAD))
                STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // boss
    if(!g_bDemoVersion) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_BOSS(m_Leviathan, {180.0f, 270.0f, 360.0, 450.0f})
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