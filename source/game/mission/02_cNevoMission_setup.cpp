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
    // TODO 1: handle enemy inside (on feel)   
    // TODO 1: homing und non-homing müssen sich optisch unterscheiden (zm. irgendein effekt on top)
    // TODO 1: make enemies damaging ?
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreFloat fRange = 1.25f;

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 48u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_TOP);
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

            nSetFunc(pSquad1->GetEnemy(32u), coreVector2( 0.8f, 1.3f), coreVector2( 0.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(33u), coreVector2(-0.8f,-1.3f), coreVector2( 0.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(34u), coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(35u), coreVector2(-1.3f, 0.0f), coreVector2( 1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(36u), coreVector2(-1.3f, 0.8f), coreVector2( 1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(37u), coreVector2( 0.0f,-1.3f), coreVector2( 1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(38u), coreVector2( 1.3f,-0.8f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(39u), coreVector2( 0.0f, 1.3f), coreVector2(-1.0f,-1.0f));

            nSetFunc(pSquad1->GetEnemy(40u), coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(41u), coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(42u), coreVector2( 1.5f, 0.0f), coreVector2(-1.0f, 0.0f));
            nSetFunc(pSquad1->GetEnemy(43u), coreVector2(-1.5f, 0.0f), coreVector2( 1.0f, 0.0f));

            nSetFunc(pSquad1->GetEnemy(44u), coreVector2( 1.3f, 1.3f), coreVector2(-1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(45u), coreVector2( 1.5f, 1.5f), coreVector2(-1.0f,-1.0f));
            nSetFunc(pSquad1->GetEnemy(46u), coreVector2(-1.3f,-1.3f), coreVector2( 1.0f, 1.0f));
            nSetFunc(pSquad1->GetEnemy(47u), coreVector2(-1.5f,-1.5f), coreVector2( 1.0f, 1.0f));
        });

        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fWhirlSpeed)
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
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 36u, 37u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 38u, 39u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 40u, 43u)
            else if(STAGE_SUB(20u)) STAGE_RESURRECT(pSquad1, 44u, 47u)
        }

        if(STAGE_BEGINNING)
        {
            constexpr coreUint8 aiNewOrder[] = {cTriangleBullet::ID, cQuadBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
                 if((m_iStageSub ==  3u) && (m_fStageSubTime < 2.0f)) return;
            else if((m_iStageSub ==  5u) && (m_fStageSubTime < 4.0f)) return;
            else if((m_iStageSub ==  7u) && (m_fStageSubTime < 2.0f)) return;
            else if((m_iStageSub == 13u) && (fWhirlSpeed     < 1.0f)) return;

            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            coreVector2 vCurDir = pEnemy->GetDirection().xy();

            const coreBool bFollow = (i >= 24u && i < 40u);

            if(bFollow)
            {
                const coreUintW   iBase = i - 24u;
                const coreVector2 vDiff = pEnemy->AimAtPlayerDual((((iBase / 4u) % 2u) ? ((iBase / 2u) % 2u) : (iBase % 2u)) ? 0u : 1u);

                if(coreVector2::Dot(vDiff, vCurDir) <= 0.0f) vCurDir = vCurDir.Rotated90() * SIGN(coreVector2::Dot(vDiff, vCurDir.Rotated90()));

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

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            pEnemy->DefaultMoveForward(vCurDir, 29.0f);

            if(STAGE_TICK_TIME(9.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();
                const coreVector2 vTan = vDir.Rotated90() * 2.0f;

                if(g_pForeground->IsVisiblePoint(vPos, fRange))
                {
                    if(vDir.IsAligned())
                    {
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos + vTan, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos - vTan, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                    }
                    else
                    {
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos + vTan, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos - vTan, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
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
        }
        else if(m_iStageSub >= 13u)
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

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](const cBullet* pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(g_pGame->IsCoop())
            {
                     if((pBulletPlayer->GetOwner() == g_pGame->GetPlayer(0u)) && (pBulletEnemy->GetID() != cQuadBullet    ::ID)) return;
                else if((pBulletPlayer->GetOwner() == g_pGame->GetPlayer(1u)) && (pBulletEnemy->GetID() != cTriangleBullet::ID)) return;
            }

            pBulletEnemy->Deactivate(true, vIntersection.xy());
        });

        if(g_pGame->GetBulletManagerEnemy()->GetNumBulletsEst() > 1000u)   // to prevent infinite creation
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([](cBullet* OUTPUT pBullet)
            {
                if(pBullet->GetFlyTime() > 20.0f) pBullet->Deactivate(true);
            });
        }

        STAGE_WAVE("SIEBEN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->GetBulletManagerEnemy()->ResetOrder();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // switch everything on
    // 6: in last sub-pattern, some middle tiles already marked, as otherwise it would be too easy to activate all
    // 6: for coop, make sure tiles are equally distributed
    // 1-5: blending in tiles needs to be equally delayed, to not favor certain start-positions
    // enemies need to die fast after loosing invincibility
    // rotating grid should not be too fast, as back-and-forth flipping can easily happen, and player needs to be able to move with (faster than) rotation
    // TODO 1: badge, helper marks N fields to active in that order
    // TODO 1: badge, 1-2 der gegner sind schon vorher abschießbar
    // TODO 1: wenn letztes tile schrumpft kommen funken bei size=0 und grüner helfer erscheint und fliegt weg
    // TODO 1: create an image with the light tiles ?
    // TODO 1: flipswitch galaxy: blocking tiles moving around, lasers blocking movement between tiles
    // TODO 1: maybe just kill everything when plates are active -> contrast to pacman stage
    // TODO 1: in second wave, 2 plate should already be active (2,2 und 1,3 (start with 0, oben links), oder 2,3 + 3,2)
    // TODO 1: color and tex change should be in mission code
    // TODO 1: adjust enemy movement patterns
    // TODO 1: hardmode: flipswitch galaxy, || =, mit blink delay wie bei rot+blau blöcke in Mario 3D World
    // TWIST (boss?): plate moves around and has positive effect when activated, negative when deactivated, never locks in
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.3f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->AddNode(coreVector2( 0.8f,-1.3f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 60u)
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
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 30u, 39u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 40u, 49u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 50u, 59u)

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

                iTileState = 0u;
                iTileDone  = 0u;
                iTileWait  = 1u;

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
                    const coreFloat fSpeed = (((i % 3u) % 2u) ? -1.0f : 1.0f) * 12.0f * TIME;

                    coreVector2 vNewPos = m_aTileRaw[i].GetPosition().xy() + coreVector2(0.0f, fSpeed);

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
                this->EnableTile(0u, 4u);
                this->EnableTile(1u, 4u);

                fTileMove  = 0.0f;
                iGauntlet += 1u;
            }
            else if(iGauntlet == 6u)
            {
                fTileMove += 0.6f * TIME;

                m_aTileRaw[0].SetPosition(coreVector3(LERPS(coreVector2(-0.25f,0.75f), coreVector2(-0.25f,-0.75f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
                m_aTileRaw[1].SetPosition(coreVector3(LERPS(coreVector2( 0.75f,0.25f), coreVector2(-0.75f, 0.25f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
            }
            else if(iGauntlet == 7u)
            {
                this->EnableTile(2u, 4u);
                this->EnableTile(3u, 4u);
                this->EnableTile(4u, 4u);

                m_aTileRaw[2].SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                m_aTileRaw[3].SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                m_aTileRaw[4].SetPosition(coreVector3(HIDDEN_POS, 0.0f));

                fTileMove  = 0.0f;
                iGauntlet += 1u;
            }
            else if(iGauntlet == 8u)
            {
                fTileMove += 1.0f * TIME;

                const coreVector2 vPos = coreVector2::Direction(fTileMove);
                const coreVector2 vDir = coreVector2::Direction(fTileMove * -2.0f);

                m_aTileRaw[2].SetPosition(coreVector3((vPos * (FOREGROUND_AREA * 0.5f)),                      0.0f));
                m_aTileRaw[3].SetPosition(coreVector3((vPos * (FOREGROUND_AREA * 0.5f)).Rotated60 () * -1.0f, 0.0f));
                m_aTileRaw[4].SetPosition(coreVector3((vPos * (FOREGROUND_AREA * 0.5f)).Rotated120(),         0.0f));

                m_aTileRaw[2].SetDirection(coreVector3(vDir, 0.0f));
                m_aTileRaw[3].SetDirection(coreVector3(vDir, 0.0f));
                m_aTileRaw[4].SetDirection(coreVector3(vDir, 0.0f));
            }
            else if(iGauntlet == 9u)
            {
                for(coreUintW i = 0u, ie = 5u; i < ie; ++i)
                {
                    const coreUintW iIndex = i + 5u;
                    ASSERT(iIndex < NEVO_TILES)

                    this->EnableTile(iIndex, 4u);

                    m_aTileRaw[iIndex].SetPosition(coreVector3(0.0f,0.0f,0.0f));
                    m_aTileRaw[iIndex].SetSize    (m_aTileRaw[iIndex].GetSize() * 0.7f * (I_TO_F(ie) - I_TO_F(i)));
                }

                iTileState = BIT(8u) | BIT(6u);
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
                    oTile.SetColor3 (COLOR_ENERGY_YELLOW);
                    oTile.SetTexSize(coreVector2(1.0f,1.0f) * 1.0f);
                }
                else
                {
                    oTile.SetColor3 (COLOR_ENERGY_BLUE);
                    oTile.SetTexSize(coreVector2(1.0f,1.0f) * 0.0f);

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

                iTileState = 0u;
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

            const coreVector2 vFactor = coreVector2(((i % 10u) < 5u) ? 1.0f : -1.0f, ((i % 30u) < 20u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i >= 10u && i < 15u) pEnemy->Rotate90 ();
            else if(i >= 15u && i < 20u) pEnemy->Rotate270();
            else if(i >= 40u && i < 45u) pEnemy->Rotate90 ();
            else if(i >= 45u && i < 50u) pEnemy->Rotate270();

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
    // [deprecated] (rotating arrow (90 degree)) (flipping side with each kill) (enemies with 2/3/4 arrows) (big enemy with changing arrow (rotating or flipping) (over time, or after a kill))
    // erste 3 gegner-wellen sind tutorial
    // der letzte gegner sollte von hinten angreifbar sein, weil man dann eh nicht mehr nachkommt
    // sekundär-gegner in 2x2 sollten nicht nach vorne schauen, mann kommt nicht nach nachdem man die primär-gegner entfernt hat, aus selben grund sollten primär-gegner nicht nach hinten schauen
    // gegner bewegungen sollten nicht zu schnell sein, man ist sonst überfordert die direction zu matchen
    // jede sub-gruppe braucht einen default-flow, schnellere flows können bestimmt gefunden werden, aber gar kein flow is orsch
    // enemies are either distributed with 0.2f or 0.5f
    // TODO 1: bullets get reflected in a weird way, and can hit wrong enemies in puzzle-levels
    // TODO 1: eine gruppe die im kreis außen fliegt, kann nur von einer richtung zerstört werden
    // TODO 1: 6x6 puzzle anpassen (falls nötig), einzelne pfeile anpassen (falls nötig)
    // TODO 1: vielleicht noch eine gruppe in der freien stelle
    // TODO 1: add more enemies (einfach gegner-blöcke und linien verlängern), irgendwas um die niedrige geschwindigkeit auszugleichen
    // TODO 1: letzte große gruppe vielleicht shiften oder aufteilen (um sie von der kleinen davor zu unterscheiden!), oder rotiert infinity nach rechts, oder chess-aufteilung
    // TODO 1: schauen ob gegner und pfeile groß genug sind, sollten schön groß sein
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

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 124u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_GREEN);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        constexpr coreUintW iMapSize = 36u;   // max number of concurrent enemies

        STAGE_GET_START(iMapSize)
            STAGE_GET_UINT_ARRAY(aiArrowMapRaw, iMapSize)
        STAGE_GET_END

        coreUint8* aiArrowMap = r_cast<coreUint8*>(aiArrowMapRaw);
        STATIC_ASSERT(NEVO_ARROWS < 256u)

        const auto nEnableArrowFunc = [&](const coreUintW iArrowIndex, const coreUintW iEnemyIndex, const coreUint8 iStep)
        {
            const cEnemy* pEnemy = pSquad1->GetEnemy(iEnemyIndex);

            this->EnableArrow(iArrowIndex, pEnemy, StepRotated90(iStep));
            aiArrowMap[(iEnemyIndex * 4u + iStep) % (iMapSize * 4u)] = iArrowIndex + 1u;
        };

        UNUSED const auto nSwapArrowFunc = [&](const coreUintW iEnemyIndex, const coreUint8 iStep1, const coreUint8 iStep2)
        {
            ASSERT(iStep1 != iStep2)

            const cEnemy* pEnemy = pSquad1->GetEnemy(iEnemyIndex);

            coreUint8& A = aiArrowMap[(iEnemyIndex * 4u + iStep1) % (iMapSize * 4u)];
            coreUint8& B = aiArrowMap[(iEnemyIndex * 4u + iStep2) % (iMapSize * 4u)];

            std::swap(A, B);

            if(A) {this->DisableArrow(A, false); this->EnableArrow(A, pEnemy, StepRotated90(iStep1));}
            if(B) {this->DisableArrow(B, false); this->EnableArrow(B, pEnemy, StepRotated90(iStep2));}
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
                nEnableArrowFunc(0u, 0u, 2u);
                nEnableArrowFunc(1u, 1u, 2u);
                nEnableArrowFunc(2u, 2u, 2u);
                nEnableArrowFunc(3u, 3u, 2u);
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 4u, 7u)
                nEnableArrowFunc(4u, 4u, 1u);
                nEnableArrowFunc(5u, 5u, 1u);
                nEnableArrowFunc(6u, 6u, 1u);
                nEnableArrowFunc(7u, 7u, 1u);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 8u, 11u)
                nEnableArrowFunc( 8u,  8u, 0u);
                nEnableArrowFunc( 9u,  9u, 0u);
                nEnableArrowFunc(10u, 10u, 0u);
                nEnableArrowFunc(11u, 11u, 0u);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 12u, 23u)
                nEnableArrowFunc(16u, 12u, 1u);
                nEnableArrowFunc(17u, 13u, 1u);
                nEnableArrowFunc(18u, 14u, 1u);
                nEnableArrowFunc(19u, 15u, 0u);
                nEnableArrowFunc(20u, 16u, 0u);
                nEnableArrowFunc(21u, 17u, 0u);
                nEnableArrowFunc(22u, 18u, 2u);
                nEnableArrowFunc(23u, 19u, 2u);
                nEnableArrowFunc(24u, 20u, 2u);
                nEnableArrowFunc(25u, 21u, 1u);
                nEnableArrowFunc(26u, 22u, 1u);
                nEnableArrowFunc(27u, 23u, 1u);
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESURRECT(pSquad1, 24u, 39u)
                nEnableArrowFunc( 0u, 24u, 3u);
                nEnableArrowFunc( 1u, 25u, 3u);
                nEnableArrowFunc( 2u, 26u, 2u);
                nEnableArrowFunc( 3u, 27u, 2u);
                nEnableArrowFunc( 4u, 28u, 3u);
                nEnableArrowFunc( 5u, 29u, 3u);
                nEnableArrowFunc( 6u, 30u, 2u);
                nEnableArrowFunc( 7u, 31u, 2u);
                nEnableArrowFunc( 8u, 32u, 2u);
                nEnableArrowFunc( 9u, 33u, 2u);
                nEnableArrowFunc(10u, 34u, 1u);
                nEnableArrowFunc(11u, 35u, 1u);
                nEnableArrowFunc(12u, 36u, 2u);
                nEnableArrowFunc(13u, 37u, 2u);
                nEnableArrowFunc(14u, 38u, 1u);
                nEnableArrowFunc(15u, 39u, 1u);
            }
            else if(STAGE_SUB(6u))
            {
                STAGE_RESURRECT(pSquad1, 40u, 47u)
                nEnableArrowFunc(16u, 40u, 0u);
                nEnableArrowFunc(17u, 41u, 3u);
                nEnableArrowFunc(18u, 42u, 2u);
                nEnableArrowFunc(19u, 43u, 1u);
                nEnableArrowFunc(20u, 44u, 3u);
                nEnableArrowFunc(21u, 45u, 0u);
                nEnableArrowFunc(22u, 46u, 2u);   // # swapped
                nEnableArrowFunc(23u, 47u, 1u);   // # swapped
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 48u, 63u)
                nEnableArrowFunc( 0u, 48u, 2u);
                nEnableArrowFunc( 1u, 49u, 2u);
                nEnableArrowFunc( 2u, 50u, 1u);
                nEnableArrowFunc( 3u, 51u, 1u);
                nEnableArrowFunc( 4u, 52u, 0u);
                nEnableArrowFunc( 5u, 53u, 3u);
                nEnableArrowFunc( 6u, 54u, 0u);
                nEnableArrowFunc( 7u, 55u, 3u);
                nEnableArrowFunc( 8u, 56u, 1u);
                nEnableArrowFunc( 9u, 57u, 0u);
                nEnableArrowFunc(10u, 58u, 1u);
                nEnableArrowFunc(11u, 59u, 0u);
                nEnableArrowFunc(12u, 60u, 2u);
                nEnableArrowFunc(13u, 61u, 2u);
                nEnableArrowFunc(14u, 62u, 3u);
                nEnableArrowFunc(15u, 63u, 3u);
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 64u, 65u)
                nEnableArrowFunc(16u, 64u, 1u);
                nEnableArrowFunc(17u, 65u, 1u);
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 66u, 81u)
                nEnableArrowFunc( 2u, 66u, 1u);
                nEnableArrowFunc( 3u, 67u, 2u);
                nEnableArrowFunc( 4u, 68u, 0u);
                nEnableArrowFunc( 5u, 69u, 2u);
                nEnableArrowFunc( 6u, 70u, 3u);
                nEnableArrowFunc( 7u, 71u, 2u);
                nEnableArrowFunc( 8u, 72u, 3u);
                nEnableArrowFunc( 9u, 73u, 3u);
                nEnableArrowFunc(10u, 74u, 1u);
                nEnableArrowFunc(11u, 75u, 2u);
                nEnableArrowFunc(12u, 76u, 0u);
                nEnableArrowFunc(13u, 77u, 1u);
                nEnableArrowFunc(14u, 78u, 0u);
                nEnableArrowFunc(15u, 79u, 2u);
                nEnableArrowFunc(18u, 80u, 0u);
                nEnableArrowFunc(19u, 81u, 3u);
            }
            else if(STAGE_SUB(10u))
            {
                STAGE_RESURRECT(pSquad1, 82u, 83u)
                nEnableArrowFunc(0u, 82u, 1u);
                nEnableArrowFunc(1u, 83u, 1u);
            }
            else if(STAGE_SUB(11u))
            {
                STAGE_RESURRECT(pSquad1, 84u, 119u)
                nEnableArrowFunc( 2u,  84u, 1u);
                nEnableArrowFunc( 3u,  85u, 2u);
                nEnableArrowFunc( 4u,  86u, 0u);
                nEnableArrowFunc( 5u,  87u, 2u);
                nEnableArrowFunc( 6u,  88u, 1u);
                nEnableArrowFunc( 7u,  89u, 2u);
                nEnableArrowFunc( 8u,  90u, 0u);
                nEnableArrowFunc( 9u,  91u, 3u);
                nEnableArrowFunc(10u,  92u, 0u);
                nEnableArrowFunc(11u,  93u, 2u);
                nEnableArrowFunc(12u,  94u, 3u);
                nEnableArrowFunc(13u,  95u, 3u);
                nEnableArrowFunc(14u,  96u, 1u);
                nEnableArrowFunc(15u,  97u, 1u);
                nEnableArrowFunc(16u,  98u, 1u);
                nEnableArrowFunc(17u,  99u, 2u);
                nEnableArrowFunc(18u, 100u, 1u);
                nEnableArrowFunc(19u, 101u, 1u);
                nEnableArrowFunc(20u, 102u, 0u);
                nEnableArrowFunc(21u, 103u, 3u);
                nEnableArrowFunc(22u, 104u, 3u);
                nEnableArrowFunc(23u, 105u, 2u);
                nEnableArrowFunc(24u, 106u, 3u);
                nEnableArrowFunc(25u, 107u, 3u);
                nEnableArrowFunc(26u, 108u, 1u);
                nEnableArrowFunc(27u, 109u, 2u);
                nEnableArrowFunc(28u, 110u, 0u);
                nEnableArrowFunc(29u, 111u, 1u);
                nEnableArrowFunc(30u, 112u, 1u);
                nEnableArrowFunc(31u, 113u, 2u);
                nEnableArrowFunc(32u, 114u, 0u);
                nEnableArrowFunc(33u, 115u, 3u);
                nEnableArrowFunc(34u, 116u, 0u);
                nEnableArrowFunc(35u, 117u, 3u);
                nEnableArrowFunc(36u, 118u, 0u);
                nEnableArrowFunc(37u, 119u, 3u);
            }
        }

        coreUint8 iActive = 0u;
        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            ADD_BIT(iActive, PackDirection(-pPlayer->GetDirection().xy()))
        });

        this->SetArrowActive(iActive);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 40u)
            {
                STAGE_LIFETIME(pEnemy, 0.5f, (i < 12u) ? (0.21f * I_TO_F(i % 4u)) : ((i < 24u) ? (0.21f * I_TO_F(i - 12u)) : (0.5f * I_TO_F((i - 24u) / 4u) + ((i < 32u) ? 0.0f : 0.28f))))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 8u) ? 0.5f : ((i < 12u) ? 0.0f : ((i < 24u) ? ((I_TO_F((((i - 12u) / 3u) * 2u + 1u) % 5u) - 1.5f) * 0.5f) : ((I_TO_F(((i - 24u) / 4u) % 2u) - 0.5f) * ((i < 32u) ? 0.8f : -0.8f) + (I_TO_F((i - 24u) % 4u) - 1.5f) * 0.2f))), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  4u) pEnemy->Rotate270();
                else if(i <  8u) {}
                else if(i < 12u) pEnemy->Rotate180();
                else if(i < 24u) pEnemy->Rotate90 ();
                else if(i < 40u) {}
            }
            else if(i < 64u)
            {
                STAGE_LIFETIME(pEnemy, 0.5f, (i < 48u) ? (0.32f * I_TO_F(i - 40u)) : (0.64f * I_TO_F((i - 48u) / 4u) + ((((i - 48u) % 4u) < 2u) ? 0.0f : 0.16f)))

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((i < 48u) ? ((((i - 40u) / 2u) % 2u) ? -1.0f : 1.0f) : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2((i < 48u) ? (((i - 40u) % 2u) ? 0.25f : -0.75f) : ((I_TO_F((i - 48u) % 2u) + ((((i - 48u) / 4u) % 2u) ? 1.5f : -2.5f)) * 0.2f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 48u) pEnemy->Rotate270();
                else if(i < 64u) pEnemy->Rotate180();
            }
            else if((i < 66u) || (i >= 82 && i < 84u))
            {
                STAGE_LIFETIME(pEnemy, 2.1f, 0.0f)

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F(i % 2u) * 0.2f + 0.7f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }
            else if(i < 82u)
            {
                STAGE_LIFETIME(pEnemy, 2.1f, 0.0f)

                const coreVector2 vFactor = coreVector2((((i - 66u) % 4u) < 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F((i - 66u) % 4u) * 0.2f - 0.3f, I_TO_F((i - 66u) / 4u) * 0.2f - 0.3f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset, fLifeTime);   // # no factor multiplication
            }
            else if(i < 120u)
            {
                STAGE_LIFETIME(pEnemy, 2.1f, 0.0f)

                const coreVector2 vFactor = coreVector2((((i - 84u) % 6u) < 3u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F((i - 84u) % 6u) * 0.2f - 0.5f, I_TO_F((i - 84u) / 6u) * 0.2f - 0.5f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset, fLifeTime);   // # no factor multiplication
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

                const coreUint32& iEntryAll = r_cast<const coreUint32&>(aiArrowMap[(i * 4u) % (iMapSize * 4u)]);
                if(!iEntryAll)
                {
                    pEnemy->Kill(true);
                }

                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = vSide;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
            }

            pBullet->Deactivate(true);
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
    // make sure there is a pause between 4 and 5(rota), make sure to start 5(rota) from different side then 4
    // consistent explosion delays make it easier to focus on the enemies simultaneously
    // letting the enemy spawn bombs made the pattern too simple, a separate spawner creates more pressure
    // 2 safe-spots after another in bomb-hail phase should not share same axis (related to enemy movement)
    // wall phase patterns need to be deterministic (also need to start deterministic)
    // bounce and rotation directions need to be opposite
    // linear movement can't use diagonal bombs as the explosion-lines will pop
    // TODO 1: if not used otherwise, hide helper in bomb, it flies away on burst-line
    // TODO 1: highlight current enemy rotation
    // TODO 1: increase safe-block (artificially) in bomb-hail phase
    // TODO 1: anderes gegner-targeting ? wegen safe-spots in raster and wall
    // TODO 1: there should be a pause after the 3x3 wave (otherwise the first bomb of the new wave gets hidden by the previous explosion)
    // TODO 1: there should be a pause on 3x3 and wall start
    // TODO 1: vielleicht rotierende bomben als vorletztes (würde einiges vereinfachen)
    // TODO 1: spieler sollte nicht mit gegner kollidieren können ?
    // TODO 1: gegner sollte sich nicht so linear und roboter-haft bewegen, vielleicht sinus (mittig langsam) ?
    // TODO 1: letzte phase is ziemlich brutal, vielleicht 2 linien ohne strahl
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 58u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.6f);
                pEnemy->Configure(50, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_TOP);
            });
        });

        STAGE_GET_START(9u + NEVO_BOMBS * 4u)
            STAGE_GET_VEC2       (vClusterPos, vClusterPos = coreVector2(-1.3f,0.5f) * FOREGROUND_AREA)
            STAGE_GET_VEC2       (vClusterDir, vClusterDir = coreVector2( 1.0f,0.0f))
            STAGE_GET_UINT       (iCurBomb)
            STAGE_GET_UINT       (iCurEnemy)
            STAGE_GET_UINT       (iWarpCount)
            STAGE_GET_UINT       (iRotaCount)
            STAGE_GET_UINT       (iWallCount)
            STAGE_GET_FLOAT_ARRAY(afDelay, NEVO_BOMBS)
            STAGE_GET_VEC2_ARRAY (avMove,  NEVO_BOMBS)
            STAGE_GET_UINT_ARRAY (aiType,  NEVO_BOMBS)
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
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 20u, 25u)   // 6
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 26u, 31u)   // 6
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 32u, 37u)   // 6
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 38u, 43u)   // 6
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 44u, 49u)   // 6
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 50u, 57u)   // 8

            iWallCount = 0u;
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

            if(((vClusterPos.x < -FOREGROUND_AREA.x * 1.3f) && (vClusterDir.x < 0.0f)) ||
               ((vClusterPos.x >  FOREGROUND_AREA.x * 1.3f) && (vClusterDir.x > 0.0f)) ||
               ((vClusterPos.y < -FOREGROUND_AREA.y * 1.3f) && (vClusterDir.y < 0.0f)) ||
               ((vClusterPos.y >  FOREGROUND_AREA.y * 1.3f) && (vClusterDir.y > 0.0f)))
            {
                const cPlayer* pPlayer = g_pGame->FindPlayerDual((((iWarpCount++) % 4u) < 2u) ? 0u : 1u);

                coreVector2 vPos, vForce;
                nTargetFunc(IsHorizontal(vClusterDir), 1.3f, pPlayer->GetPosition().xy(), &vPos, &vForce);

                vClusterPos = vPos;   // no position correction
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

                    const coreUint32 iType = aiType[i];

                    if((iType == 1u) || (iType == 2u))   // break movement
                    {
                        oBomb.SetPosition(coreVector3(oBomb.GetPosition().xy() + avMove[i] * TIME, 0.0f));
                        avMove[i] *= FrictionFactor(1.5f);

                        if(iType == 2u) oBomb.SetDirection(coreVector3(coreVector2::Direction(afDelay[i] * ((i % 2u) ? 1.0f : -1.0f)), 0.0f));
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
                        const coreVector2 vRota   = coreVector2::Direction(afDelay[i] * (-0.5f*PI) * (avMove[i].Length() * 0.01f));
                        const coreFloat   fLen    = LERPB(1.2f * SQRT2, 0.9f, MIN1(afDelay[i] * 0.5f)) * FOREGROUND_AREA.x;
                        const coreVector2 vNewPos = MapToAxis(-avMove[i].Normalized(), vRota) * fLen;

                        oBomb.SetPosition(coreVector3(vNewPos, 0.0f));
                    }
                    else if((iType == 6u) || (iType == 7u))   // lerp movement
                    {
                        coreVector2 vNewPos = oBomb.GetPosition().xy();
                        vNewPos.arr(iType == 7u) = LERPB(avMove[i].x, avMove[i].y, MIN1(afDelay[i] * 0.5f));

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

                    afDelay[i] = 0.0f;
                }
            }

            if(m_iStageSub == 6u)
            {
                if(STAGE_TICK_FREE(0.5f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
                {
                    iWallCount += 1u;

                    const coreUint8 iFrom = (iWallCount % 3u) * 3u + 1u;
                    const coreUint8 iTo   = (iWallCount % 3u) * 3u + 4u;

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
                if(STAGE_TICK_FREE(0.25f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
                {
                    iWallCount += 1u;

                    coreUint8 iX, iY;
                    switch(iWallCount % 8u)
                    {
                    default: ASSERT(false)
                    case 0u: iX = 3u; iY = 3u; break;
                    case 1u: iX = 5u; iY = 7u; break;
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
                if(STAGE_TICK_FREE(0.75f, 0.0f) && (iRotaCount || ((s_iTick % 2u) == 0u)))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
                {
                    const cPlayer*    pPlayer = g_pGame->FindPlayerDual(((s_iTick % 8u) < 4u) ? 0u : 1u);
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

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(iCurEnemy == i)
                {
                    if(pEnemy->ReachedDeath()) iCurEnemy += 1u;

                    const coreVector2 vRealDir = MapStepRotated90(vClusterDir, (m_iStageSub - 1u) % 4u);

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
    // TODO 1: create an absurd big object/triangle in the middle
    // TODO 1: gegner-masse kommt auf spieler zu, alle mit energy, man muss loch rein schießen
    // TODO 1: small enemy triangles stay (maybe only for hard)
    // TODO 1: keep triangles from growing with attacks
    // TODO 1: movement only from right to left
    // TODO 1: (previously bullet shrink+grow, mentioned in bottom of Waves page)
    // TODO 1: gegner versuchen dich in mustern zu berühren, schlange,
    // TODO 1: frei gewordene objekte fliegen auf spieler zu oder in kreiselbewegung nach außen
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

#if defined(_P1_VIDEO_)
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 24u)
#else
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 24u)
#endif
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
#if defined(_P1_VIDEO_)
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
#else
                pEnemy->Configure(4, COLOR_SHIP_ORANGE);
#endif
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
            else if(STAGE_SUB(10u))
            {
                for(coreUintW i = 0u; i < NEVO_BLOCKS; ++i)
                    this->DisableBlock(i, true);

                g_pGame->GetHelper(ELEMENT_ORANGE)->Kill(true);
            }

            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                if(!pEnemy->ReachedDeath()) this->EnableBlock((i < iNumBig) ? i : ((i % (NEVO_BLOCKS - iNumBig)) + iNumBig), pEnemy, (i < iNumBig) ? 10.0f : -5.0f);
            });
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

                     if(vNewPos.x < -FOREGROUND_AREA.x * 1.45f) vNewPos.x =  ABS(vNewPos.x);   // TODO 1: shift correction
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
    // [deprecated] - all: bullet speed is much lower than enemy speed, to create a perceptible path for the player to use
    // [deprecated] - all: bullet frequency is not too high, to better distinguish individual bullets
    // [deprecated] - all: enemy attack begins later, to not slow down bullets immediately after shooting
    // [deprecated] - 1,2: enemy path is not too wide, to not slow down bullets immediately after shooting
    // bullets vergrößern is orsch, du kannst nicht schießen während du versuchst zwischen geschossen durch zu gehen, aber dadurch bist du zu schnell, schiff wegdrehen is un-intuitiv, bullets zerstören ab größe is 1-dimensional (vergrößern sollte eher für die andere gruppe aufgehoben werden (mit gegnern))
    // gruppen die danach kommen, können/sollten von den angriffen der gruppe davor verdeckt werden
    // derzeitiger fokus liegt darauf den spieler sich vorsichtig durch die masse bewegen zu lassen, wie durch wasser schwimmen
    // gegner die sich gegenüber sind sollen den spieler dazu bringen hin un her zu drehen, um den einen anzugreifen, und die angriffe des anderen durchdringbar zu machen
    // gegner die geschosse hinter sich herziehen haben fokus auf flow
    // die vertikalen linien sollen den spieler dazu bringen 90grad durch zu fliegen
    // gegner die geschosse erzeugen sind angreifbar aufgrund der range (gegner und bullets) und ihrer bewegung
    // TODO 1: enemy or golden ball (1-N) hides in blue balls, to attack for badge
    // TODO 1: blue helper in one of the bullets
    // TODO 1: maybe all bullets get gravity, "flooding" the bottom (ZeroRanger kugerl-auge-boss)
    // TODO 1: mehr bewegung wie im bälle-bad
    // TODO 1: they bounce once
    // TODO 1: große kugerl explodieren in viele kleine
    // TODO 1: 3 rotieren vom center nach außen
    // TODO 1: handle BULLET_COLLISION_FACTOR in cOrbBullet (add own GrowBullet/ShrinkBullet class ? own bullet-manager ? beware ENABLE_ID is final) (search for BULLET_COLLISION_FACTOR)
    // TODO 1: gruppen müssen noch besser angeordnet werden, vielleicht noch mit 1 weiteren filler
    // TODO 1: wall von oben sollte etwas komplexeres muster haben, vielleicht 3er-welle statt derzeit 2er, auch gegner darin anpassen
    // TODO 1: wall von oben und die beiden vertical linien sind zu ähnlich, jedoch die linien zu drehen könnte den spieler am ursprung der wall positionieren, was nicht gut ist
    STAGE_MAIN({TAKE_ALWAYS, 5u})
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

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 19u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(50, COLOR_SHIP_PURPLE);
            });
        });

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
        }

        if(m_iStageSub == 5u)
        {
            if(STAGE_TICK_FREE(1.5f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
            {
                const coreVector2 vPos = coreVector2(0.4f, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pSquad1->GetEnemy(0u),  vPos,  vDir)->ChangeSize(1.0f)->ChangeTexSize(3.0f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pSquad1->GetEnemy(0u), -vPos, -vDir)->ChangeSize(1.0f)->ChangeTexSize(3.0f);
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(STAGE_TICK_FREE(1.3f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
            {
                for(coreUintW i = 0u; i < 10u; ++i)
                {
                    if((i % 2u) == (s_iTick % 2u)) continue;

                    const coreVector2 vPos = coreVector2((I_TO_F(i) - 4.5f) * 0.22f, 1.2f) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.0f)->ChangeTexSize(3.0f);
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i == 14u) ? 0.5f : 0.7f, (i < 6u) ? 0.0f : ((i < 8u) ? 1.0f : ((i < 12u) ? (0.3f * I_TO_F(i - 8u) + 0.5f) : 0.0f)))

            if(i < 2u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                if(i == 1u) pEnemy->Rotate90();

                if(STAGE_TICK_LIFETIME(6.0f, 0.0f) && (((s_iTick + 7u) % 8u) < ((i == 0u) ? 1u : 2u)))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle() + ((i == 0u) ? 0.0f : DEG_TO_RAD((s_iTick % 2u) ? 7.0f : -7.0f));

                    for(coreUintW j = 5u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 28.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
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

                if(STAGE_TICK_LIFETIME(8.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90() * ((s_iTick % 2u) ? -1.0f : 1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.4f, pEnemy, vPos, vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                }
            }
            else if(i < 12u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 8u) ? 0.8f : (I_TO_F(i - 8u) * 0.3f - 0.9f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 7u)             pEnemy->Rotate180();
                else if(i == 8u || i == 10u) pEnemy->Rotate90 ()->InvertX();
                else if(i == 9u || i == 11u) pEnemy->Rotate90 ();
            }
            else if(i < 14u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 13u) ? 0.5f : -0.5f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(STAGE_TICK_LIFETIME(1.3f, 0.0f))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = (s_iTick % 2u) ? 18.0f : 0.0f;

                    for(coreUintW j = 10u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f + fBase));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.4f, pEnemy, vPos, vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                    }
                }
            }
            else if(i < 15u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (fLifeTime * 5.0f);   // affects bullet direction

                if(STAGE_TICK_LIFETIME(12.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90() * ((s_iTick % 2u) ? -1.0f : 1.0f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.4f, pEnemy, vPos,  vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                }
            }
            else if(i < 17u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 15u) pEnemy->Rotate90 ();
                else if(i == 16u) pEnemy->Rotate270();

                if(STAGE_TICK_LIFETIME(3.0f, 0.0f) && (((s_iTick + ((i == 15u) ? 4u : 0u)) % 8u) == 0u))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                    const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();

                    for(coreUintW j = 10u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 4.5f) * 14.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
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

                if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual((i == 17u) ? 1u : 0u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f)->ChangeTexSize(3.0f);
                }
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            const coreFloat fNewSize = MIN(pBullet->GetSize().x + (7.0f * TIME), 15.0f);

            pBullet->SetSize(coreVector3(1.0f,1.0f,1.0f) * fNewSize);
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            if(coreMath::IsNear(pBullet->GetSpeed(), 0.1f)) return;

            if(((pBullet->GetPosition().x <  -FOREGROUND_AREA.x * 1.0f) && (pBullet->GetFlyDir().x < 0.0f)) ||
               ((pBullet->GetPosition().x >=  FOREGROUND_AREA.y * 1.0f) && (pBullet->GetFlyDir().x > 0.0f)) ||
               ((pBullet->GetPosition().y <  -FOREGROUND_AREA.y * 1.0f) && (pBullet->GetFlyDir().y < 0.0f)) ||
               ((pBullet->GetPosition().y >=  FOREGROUND_AREA.y * 1.0f) && (pBullet->GetFlyDir().y > 0.0f)))
            {
                pBullet->SetSpeed(0.1f);
                return;
            }

            Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, pBullet, [](cBullet* OUTPUT pBullet1, cBullet* OUTPUT pBullet2, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                if(((pBullet1->GetPosition().x <  -FOREGROUND_AREA.x * 1.0f) && (pBullet2->GetFlyDir().x < 0.0f)) ||   // 1 2
                   ((pBullet1->GetPosition().x >=  FOREGROUND_AREA.y * 1.0f) && (pBullet2->GetFlyDir().x > 0.0f)) ||
                   ((pBullet1->GetPosition().y <  -FOREGROUND_AREA.y * 1.0f) && (pBullet2->GetFlyDir().y < 0.0f)) ||
                   ((pBullet1->GetPosition().y >=  FOREGROUND_AREA.y * 1.0f) && (pBullet2->GetFlyDir().y > 0.0f)))
                {
                    pBullet2->SetSpeed(0.1f);
                    return;
                }
            });
        });

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pPlayerBullet, cBullet* OUTPUT pEnemyBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!bFirstHit || !g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            pEnemyBullet->SetSize(coreVector3(1.0f,1.0f,1.0f) * (pEnemyBullet->GetSize().x * POW(0.9f, I_TO_F(pPlayerBullet->GetDamage()))));

            pPlayerBullet->Deactivate(true);
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