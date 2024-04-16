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
// setup the Rutilus mission
void cRutilusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSpaceBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // teleport everything
    // every representation where the teleporter can be avoided will not work, as the player will always avoid it due to unpredictability, so teleporter need to span across screen
    // dangers of teleportation (especially with changing directions) are hard to assess, so players will avoid using it with the ship
    // when the player has to use the teleporter, the situation needs to be easy and predictable (only one location with enemies)
    // it is important that the player has to use the feature in some way
    //      special care needs to be taken for split-screen coop, players cannot change sides
    // tests were done with rotation (unpredictable), movement (unnecessary), 90 degree difference (unpredictable), infinity (unnecessary)
    // moving both portals into their direction or against it, can cause space-folding or -expansion which is physically impossible (e.g. folding would catch objects in the portal and crush them)
    // 1: show player the mechanic
    // 2-5: force the player to use teleporter
    // 6: enemies need to move the player into a safe location
    // 7: groups are not in center, to make movement a bit easier (attacking enemies will not attack player)
    // 8-11: 1-2-2-1 pattern
    // (TODO: portale werden am ende zu gegnern, ZeroRanger)
    // TODO: badge: in der mitte am anfang
    // TODO: fix coop, teleportation into other field not possible -> maybe relax and allow it, force teleport with effect back on end, check and change player-area, should make vertical possible again (repair enemy ?)
    // TODO: on vertical, enemy from below may ram into player
    // TODO: get rid of "fake enemy"
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 27u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT (iTransitionState)
            STAGE_GET_FLOAT(fTransitionTime)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB( 2u)) STAGE_RESSURECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESSURECT(pSquad1,  8u,  9u)
            else if(STAGE_SUB( 4u)) STAGE_RESSURECT(pSquad1, 10u, 11u)
            else if(STAGE_SUB( 5u)) STAGE_RESSURECT(pSquad1, 12u, 13u)
            else if(STAGE_SUB( 6u)) STAGE_RESSURECT(pSquad1, 14u, 15u)
            else if(STAGE_SUB( 7u)) STAGE_RESSURECT(pSquad1, 16u, 21u)
            else if(STAGE_SUB( 8u)) STAGE_RESSURECT(pSquad1, 22u, 22u)
            else if(STAGE_SUB( 9u)) STAGE_RESSURECT(pSquad1, 23u, 23u)
            else if(STAGE_SUB(10u)) STAGE_RESSURECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB(11u)) STAGE_RESSURECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB(12u)) STAGE_RESSURECT(pSquad1, 26u, 26u)   // fake enemy

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;
        }

        cHelper* pHelperOrange = g_pGame->GetHelper(ELEMENT_ORANGE);
        cHelper* pHelperBlue   = g_pGame->GetHelper(ELEMENT_BLUE);

        if(STAGE_BEGINNING)
        {
            pHelperOrange->Resurrect();
            pHelperBlue  ->Resurrect();
        }

        const coreBool bVertical = false;

        fTransitionTime += 1.0f * Core::System->GetTime();

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                const coreFloat   fTime = MIN(m_fStageTime, 1.2f);
                const coreVector2 vPos  = coreVector2((SIN(fTime / 1.2f * 0.5f*PI) * 2.0f - 1.0f) * (1.2f - fTime), LERP(-0.3f, 0.4f, SIN(fTime / 1.2f * 1.0f*PI))) * FOREGROUND_AREA;

                m_aTeleporter[0].SetPosition(coreVector3( vPos,    0.0f));
                m_aTeleporter[1].SetPosition(coreVector3(-vPos,    0.0f));
                m_aTeleporter[0].SetSize    (coreVector3(1.0f,1.0f,1.0f));
                m_aTeleporter[1].SetSize    (coreVector3(1.0f,1.0f,1.0f));

                if(fTransitionTime >= 1.2f)
                {
                    iTransitionState += 1u;

                    this->SetTeleporterActive(1u);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) this->EnableTeleporter(i);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) g_pSpecialEffects->CreateSplashColor(m_aTeleporter[i].GetPosition(), 50.0f, 10u, RUTILUS_TELEPORTER_COLOR(i));
                }
            }
            else if(iTransitionState == 1u)
            {
                const coreFloat fTime = MIN(m_fStageTime - 1.2f, 1.0f);
                const coreFloat fSize = LERPB(0.0f, 1.0f, fTime) * 1.05f * FOREGROUND_AREA.x;

                m_aTeleporter[0].SetSize(coreVector3(fSize, 1.0f, 1.0f));
                m_aTeleporter[1].SetSize(coreVector3(fSize, 1.0f, 1.0f));

                if(fTransitionTime >= 2.2f)
                {
                    iTransitionState += 1u;
                }
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(iTransitionState == 0u)
            {
                if(fTransitionTime >= 0.5f)
                {
                    iTransitionState += 1u;

                    this->SetTeleporterActive(0u);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__LineEffect(i);
                }
            }
            else if(iTransitionState == 1u)
            {
                const coreFloat fTime = LERPS(0.0f, 1.0f, CLAMP((fTransitionTime - 1.0f) * 0.5f, 0.0f, 1.0f));

                if(bVertical)
                {
                    const coreVector2 vPos = LERP(coreVector2(0.0f,0.3f), coreVector2(0.7f,0.0f), fTime) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2::Direction(fTime * 0.5f*PI);

                    m_aTeleporter[0].SetPosition (coreVector3(-vPos,             0.0f));
                    m_aTeleporter[1].SetPosition (coreVector3( vPos,             0.0f));
                    m_aTeleporter[0].SetDirection(coreVector3(-vDir.InvertedY(), 0.0f));
                    m_aTeleporter[1].SetDirection(coreVector3( vDir,             0.0f));
                }
                else
                {
                    const coreVector2 vPos = LERP(coreVector2(0.0f,0.3f), coreVector2(0.0f,0.7f), fTime) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2::Direction(fTime * 1.0f*PI);

                    m_aTeleporter[0].SetPosition (coreVector3(-vPos, 0.0f));
                    m_aTeleporter[1].SetPosition (coreVector3( vPos, 0.0f));
                    m_aTeleporter[1].SetDirection(coreVector3( vDir, 0.0f));
                }

                if(fTransitionTime >= 3.0f)
                {
                    iTransitionState += 1u;
                }
            }
            else if(iTransitionState == 2u)
            {
                if(fTransitionTime >= 3.5f)
                {
                    iTransitionState += 1u;

                    this->SetTeleporterActive(1u);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__LineEffect(i);
                }
            }
        }
        else if(m_iStageSub == 12u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
            }

            const coreFloat fPos = (LERPB(0.0f, 0.7f, 1.0f - fTransitionTime) + SIN(fTransitionTime * 1.0f*PI) * 0.3f) * FOREGROUND_AREA.y;

            if(bVertical)
            {
                m_aTeleporter[0].SetPosition(coreVector3(-fPos, 0.0f, 0.0f));
                m_aTeleporter[1].SetPosition(coreVector3( fPos, 0.0f, 0.0f));
            }
            else
            {
                m_aTeleporter[0].SetPosition(coreVector3(0.0f, -fPos, 0.0f));
                m_aTeleporter[1].SetPosition(coreVector3(0.0f,  fPos, 0.0f));
            }

            if(fTransitionTime >= 1.0f)
            {
                for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
                    this->DisableTeleporter(i, true);

                pSquad1->GetEnemy(26u)->Kill(false);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                pHelperOrange->Kill(false);
                pHelperBlue  ->Kill(false);
            }
        }

        pHelperOrange->SetPosition(m_aTeleporter[0].GetPosition());
        pHelperBlue  ->SetPosition(m_aTeleporter[1].GetPosition());

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 6u) ? 2.0f : ((i >= 14u && i < 16u) ? 3.5f : 0.0f))

            if(i < 14u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = (i < 6u) ? coreVector2((I_TO_F(i) - 2.5f) * 0.2f, ABS(I_TO_F(i) - 2.5f) * 0.05f - 0.025f) : coreVector2(I_TO_F(i % 2u) * 0.2f - 0.82f, 0.2f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  6u) {}
                else if(i <  8u) pEnemy->Rotate270();
                else if(i < 10u) pEnemy->Rotate270()->InvertY();
                else if(i < 12u) pEnemy->Rotate90 ()->InvertY();
                else if(i < 14u) pEnemy->Rotate90 ();
            }
            else if(i < 26u)
            {
                const coreVector2 vFactor = coreVector2(1.0f, (i == 22u || i == 25u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = (i < 16u) ? coreVector2(0.0f,0.2f) : ((i < 22u) ? coreVector2((I_TO_F((i - 16u) % 3u) - 1.0f) * 0.2f + 0.3f, ABS(I_TO_F((i - 16u) % 3u) - 1.0f) * 0.05f + 0.2f) : coreVector2((i % 2u) ? -0.3f : 0.3f, 0.2f));

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 15u) pEnemy->Rotate90 ();
                else if(i < 16u) pEnemy->Rotate270();
                else if(i < 19u) pEnemy->Rotate180();

                if(bVertical) pEnemy->Rotate90();
            }
            else
            {
                pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            }

            if(STAGE_TICK_LIFETIME(1.2f, 0.0f))
            {
                const coreUintW   iCount = (i < 6u || (i >= 14 && i < 22u)) ? 1u : 5u;
                const coreVector2 vPos   = pEnemy->GetPosition().xy();
                const coreFloat   fBase  = pEnemy->AimAtPlayerSide().Angle();

                for(coreUintW j = iCount; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iCount - 1u) * 0.5f) * 4.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        });

        STAGE_WAVE("SECHSUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all teleporters
    STAGE_MAIN
    {
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            this->DisableTeleporter(i, false);

        g_pGame->GetHelper(ELEMENT_ORANGE)->Kill(false);
        g_pGame->GetHelper(ELEMENT_BLUE)  ->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Quaternion, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Saros, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // meteor split-up with enemies at the end
    // geschoses kommen auf schussstelle, wie als würde man teilchen abschaben, nur vom ersten meteorit
    // dann aufteil-phase, wird immer kleiner
    // letzter meteorit enthällt gegner, aber braucht ne art transition, der sich in 2 4 aufteilt
    // gegner steckt kopfüber in meteoriten
    //
    // mehrere kleine meteoriten und zwei mittlere
    // dann großer nach unten schläft ein
    // großer wird zerstört und erzeugt 3 mittlere
    // mittlere erzeugen kleine aber mit gegnern darin
    // rotieren um z
    // wenn zerstört fliegt gegner in richtung wo er rauschaut dreht sich und schießt wield um sich, verhällt sich sonst wie meteorit
    // TODO: UNVOLLSTÄNDIG
    // bounce zwischen meteoriten is total nicht nachvollziehbar, Ikaruga und RSG haben sowas nicht
    // meteoriten mit gegnern darin müssen etwas mehr aushalten um nicht unabsichtlich beim erzeugen schon zerstört zu werden
    STAGE_MAIN
    {
        constexpr coreUintW iMeteorNum = 90u;//10u+2u+6u+1u+3u+9u;        

        STAGE_ADD_SQUAD(pSquad1, cMeteorEnemy, iMeteorNum)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 10.0f);
                pEnemy->Configure(1, COLOR_SHIP_PURPLE);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });

            pSquad1->GetEnemy(0u)->SetMaxHealth(200);
            pSquad1->GetEnemy(0u)->Resurrect();
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 8u)    
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(50, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            });
        });

        STAGE_GET_START(iMeteorNum * 2u + 5u + 4u)
            STAGE_GET_VEC2_ARRAY(avMove, iMeteorNum, avMove[0] = coreVector2(0.0f,-1.0f))
            STAGE_GET_FLOAT     (fRotation)
            STAGE_GET_FLOAT_ARRAY     (afRotationAdd, 4u)
            STAGE_GET_UINT      (iSpawnFirst)
            STAGE_GET_UINT      (iSpawnSmall,  iSpawnSmall = 8u)
            STAGE_GET_UINT      (iSpawnMedium, iSpawnMedium = 1u)
            STAGE_GET_UINT      (iImpact)
        STAGE_GET_END

        fRotation = FMOD(fRotation + 1.1f * Core::System->GetTime(), 2.0f*PI);

        if(iSpawnFirst < 30u)
        {
            if(STAGE_BEGINNING || STAGE_TICK_FREE(5.0f, 0.0f))
            {
                const coreBool bMedium = ((iSpawnFirst % 6u) == 5u);
                const coreUintW iIndex = bMedium ? (iSpawnMedium++) : (iSpawnSmall++);

                cEnemy* pNewEnemy = pSquad1->GetEnemy(iIndex);

                pNewEnemy->SetPosition(coreVector3(SIN(I_TO_F(iSpawnFirst) * 1.5f + 0.0f), 1.3f, 0.0f) * FOREGROUND_AREA3);
                pNewEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * (bMedium ? 5.0f : 3.0f));
                pNewEnemy->SetMaxHealth(bMedium ? 30 : 4);
                pNewEnemy->Resurrect();

                avMove[iIndex] = coreVector2(0.0f,-1.0f).Normalized();

                iSpawnFirst += 1u;
            }
        }
        else if(iSpawnFirst == 30u)
        {
            cEnemy* pNewEnemy = pSquad1->GetEnemy(0u);

            pNewEnemy->SetPosition(coreVector3(0.0f,1.5f,0.0f) * FOREGROUND_AREA3);
            pNewEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 11.0f);

            avMove[0u] = coreVector2(0.0f,-1.0f);

            iSpawnFirst += 1u;
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
            {
                if(j <= i) return;

                const coreVector2 vDiff   = (pEnemy->GetPosition().xy()   - pEnemy2->GetPosition().xy());
                const coreFloat   fRadius = (pEnemy->GetCollisionRadius() + pEnemy2->GetCollisionRadius()) * 0.7f;

                if(vDiff.LengthSq() < POW2(fRadius))
                {
                    const coreVector2 vNormal = vDiff.Normalized();
                    const coreVector2 vCopy1  = avMove[i];
                    const coreVector2 vCopy2  = avMove[j];

                    if(i) avMove[i] = (coreVector2::Reflect(vCopy1,  vNormal) + vCopy2 * MAX(coreVector2::Dot(vCopy2.Normalized(),  vNormal), 0.0f)).Normalized();
                    avMove[j] = (coreVector2::Reflect(vCopy2, -vNormal) + vCopy1 * MAX(coreVector2::Dot(vCopy1.Normalized(), -vNormal), 0.0f)).Normalized();

                    if(!i) pEnemy2->SetPosition(pEnemy->GetPosition() - coreVector3(vDiff.Normalized() * fRadius, 0.0f));
                }
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreFloat fRadius = pEnemy->GetCollisionRadius();
            const coreBool  bMedium = (fRadius > 5.0f);
            const coreBool  bBig    = (i == 0u);

            if(pEnemy->ReachedDeath())
            {
                if(bBig)
                {
                    for(coreUintW j = 0u; j < 2u; ++j)
                    {
                        const coreUintW iIndex = (iSpawnMedium++);
                        cEnemy* pNewEnemy = pSquad1->GetEnemy(iIndex);

                        const coreVector2 vDir = coreVector2(j ? -1.0f : 1.0f, 1.0f).Normalized();
                        avMove[iIndex] = vDir;

                        pNewEnemy->SetPosition(pEnemy->GetPosition() + coreVector3(vDir * 2.0f, 0.0f));
                        pNewEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
                        pNewEnemy->SetMaxHealth(30);
                        pNewEnemy->Resurrect();

                        //iSpawnMedium += 1u;
                    }

                    pEnemy->Kill(true);
                }
                if(bMedium)
                {

                    for(coreUintW j = 0u; j < 4u; ++j)
                    {
                        const coreUintW iIndex = 33u + i * 4u + j;
                        cEnemy* pNewEnemy = pSquad1->GetEnemy(iIndex);

                        const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) * (0.5f*PI) + (0.25f*PI));
                        avMove[iIndex] = vDir;

                        pNewEnemy->SetPosition(pEnemy->GetPosition() + coreVector3(vDir * 2.0f, 0.0f));
                        pNewEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 3.0f);
                        pNewEnemy->SetMaxHealth(4);

                        if(iIndex >= 33u && iIndex < 37u)
                        {
                            cEnemy* pNewEnemy2 = pSquad2->GetEnemy(iIndex - 33u);
                            pNewEnemy2->Resurrect();

                            pNewEnemy->SetMaxHealth(10);
                        }

                        pNewEnemy->Resurrect();

                        //iSpawnSmall += 1u;
                    }

                    pEnemy->Kill(true);
                }
                else
                {
                    pEnemy->Kill(true);
                }
            }

            if(!i && (pEnemy->GetPosition().y < -(FOREGROUND_AREA.y * 1.0f)))
            {
                if(!iImpact)
                {
                    iImpact = 1u;
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                }

                //if(STAGE_TICK_LIFETIME_BASE(2.0f, 0.0f))
                //{
                //    const coreVector2 vPos = coreVector2(0.0f, FOREGROUND_AREA.y * -1.1f);
//
                //    for(coreUintW j = 21u; j--; )
                //    {
                //        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f - 90.0f));
//
                //        g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                //    }
                //}
            }
            else
            {

                pEnemy->SetDirection(coreVector3(coreVector2::Direction((fRotation + I_TO_F(i)) * (bMedium ? -1.0f : 1.0f)), 0.0f));

                if(avMove[i])
                {
                    coreVector2 vCurPos = pEnemy->GetPosition().xy();
                    coreVector2 vCurDir = avMove[i];

                    vCurPos += vCurDir * ((bBig ? 50.0f : (bMedium ? 20.0f : 25.0f)) * Core::System->GetTime());

                    if(i)
                    {
                             if((vCurPos.x < -(FOREGROUND_AREA.x * 1.2f - fRadius)) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
                        else if((vCurPos.x >  (FOREGROUND_AREA.x * 1.2f - fRadius)) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                             if((vCurPos.y < -(FOREGROUND_AREA.y * 1.2f - fRadius)) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
                        else if((vCurPos.y >  (FOREGROUND_AREA.y * 1.2f - fRadius)) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);
                    }

                    pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
                    avMove[i] = vCurDir;
                }

            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const cEnemy* pParent = pSquad1->GetEnemy(i + 33u);
            if(CONTAINS_FLAG(pParent->GetStatus(), ENEMY_STATUS_DEAD))
            {

                coreVector2& vMove = avMove[i + 33u];

                if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);

                    vMove = -pEnemy->GetDirection().xy();
                    afRotationAdd[i] = fRotation;
                }

                if(vMove)
                {
                    coreVector2 vCurPos = pEnemy->GetPosition().xy();
                    coreVector2 vCurDir = vMove;

                    vCurPos += vCurDir * (25.0f * Core::System->GetTime());

                    {
                             if((vCurPos.x < -FOREGROUND_AREA.x * 1.0f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
                        else if((vCurPos.x >  FOREGROUND_AREA.x * 1.0f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                             if((vCurPos.y < -FOREGROUND_AREA.y * 1.0f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
                        else if((vCurPos.y >  FOREGROUND_AREA.y * 1.0f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);
                    }

                    pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
                    vMove = vCurDir;
                }

                afRotationAdd[i] = FMOD(afRotationAdd[i] + 2.4f * Core::System->GetTime(), 2.0f*PI);

                pEnemy->SetDirection(coreVector3(coreVector2::Direction((afRotationAdd[i] + I_TO_F(i + 33u)) * 1.0f), 0.0f));

                if(STAGE_TICK_LIFETIME_BASE(26.0f, 0.0f) && ((s_iTick % 4u) < 2u))   // TODO: tick-1 ? 
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                }
            }
            else
            {
                const coreVector3 vOffset = pParent->GetRotation().QuatApply(coreVector3(0.0f,-3.0f,0.0f));

                pEnemy->SetPosition   (pParent->GetPosition   () + vOffset);
                pEnemy->SetDirection  (pParent->GetDirection  ());
                pEnemy->SetOrientation(pParent->GetOrientation());
            }
        });

        STAGE_WAVE("SECHSUNDFÜNFZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });
STAGE_START_HERE
    // ################################################################
    // gravitation influences bullets
    // enemy bullets first
    // then player bullets or vice versa
    // gravity inverts
    // gravity gets stronger
    // enemies rotate around orbit
    // helper reflects (all bullets, player, enemy), to not allow crossing
    // partikel die in die mitte gezogen werden, mit zusätzlichen waves
    //    mehrere geschosse hintereinander, gleichzeitig vom selben, erzeugen veränderliche muster wegen unterschiedlicher gravitatsions-wirkung
    // bewegende gegner sind leichter
    // both spherical gravity, both with speed-preservation and true gravity (speed-changing) are hard to use especially with enemy-bullets accelerating
    
    // TODO: give dual-shot consistent distance on bending
    // helper geht ganz nach unten und saugt an
    // kleine gegner welle von oben in Y0.0, dann noch ne kleine in Y0.4, dnan große in Y0.8
    // gravity both requires to get near to enemies, and allows to shoot in a curve
    // TODO: distortion for waves ?
    // TODO: UNVOLLSTÄNDIG
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,1.2f), coreVector2(1.0f,-1.0f).Normalized());
            pPath1->AddStop(coreVector2(-0.8f,0.8f), coreVector2(1.0f,-1.0f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.8f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 29u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT (iTransitionState)
            STAGE_GET_FLOAT(fTransitionTime)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 19u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 20u, 27u)
            else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1, 28u, 28u)   // fake enemy

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_RED);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect();
        }

        fTransitionTime += 1.0f * Core::System->GetTime();

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                pHelper->SetPosition(coreVector3(0.0f, LERPB(-1.2f, 0.0f, fTransitionTime) * FOREGROUND_AREA.y, 0.0f));

                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;
                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);

                    this->EnableWave();
                }
            }
        }
        else if(m_iStageSub == 2u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);

                ADD_BIT(m_iWaveDir, 1u)
            }
        }
        else if(m_iStageSub == 3u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);

                ADD_BIT(m_iWaveDir, 0u)
            }
        }
        else if(m_iStageSub == 4u)
        {
            if(iTransitionState == 0u)
            {
                pHelper->SetPosition(coreVector3(0.0f, LERPS(0.0f, -0.95f, fTransitionTime) * FOREGROUND_AREA.y, 0.0f));

                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;
                }
            }
        }
        else if(m_iStageSub == 5u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                this->DisableWave(true);
            }

            const coreFloat fTime = fTransitionTime * 1.9f;

            pHelper->SetPosition(coreVector3(0.0f, (LERPB(-0.95f, 0.1f, fTime) - fTime) * FOREGROUND_AREA.y, 0.0f));

            if(fTransitionTime >= 0.7f)
            {
                pSquad1->GetEnemy(28u)->Kill(false);

                pHelper->Kill(false);
            }
        }

        if(CONTAINS_BIT(m_iWaveActive, RUTILUS_WAVES))
        {
            const coreFloat   fSign = ((m_iStageSub >= 2u) ? 1.0f : -1.0f);
            const coreVector2 vRota = ((m_iStageSub >= 3u) ? coreVector2(0.01f,1.0f) : coreVector2(1.0f,0.01f)) * fSign;
            //const coreVector2 vRota = ((m_iStageSub >= 3u) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f)) * fSign;

            const auto nGravityFunc = [&](cBullet* OUTPUT pBullet)
            {
                const coreVector2 vDiff = pHelper->GetPosition().xy() - pBullet->GetPosition().xy();
                const coreVector2 vPush = (vDiff /** vRota*/*fSign).Normalized();

                pBullet->SetFlyDir(SmoothAim(pBullet->GetFlyDir(), vPush, pBullet->GetSpeed() * 0.3f      * 1.0f));
                if(pBullet->GetFlyTime() >= 6.0f) pBullet->Deactivate(true);
                
                //const coreVector2 vNewForce = pBullet->GetFlyDir() * pBullet->GetSpeed() + vPush * pBullet->GetSpeed() * 0.5f * (60.0f * Core::System->GetTime()) * RCP(MAX(vDiff.Length(), 8.0f) + 0.0f);
                //pBullet->SetFlyDir(vNewForce.Normalized());
                //pBullet->SetSpeed(vNewForce.Length());
            };

            g_pGame->GetBulletManagerPlayer()->ForEachBullet(nGravityFunc);
            g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nGravityFunc);

            m_aWaveRaw[0].SetPosition(pHelper->GetPosition());
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 6u) ? 1.0f : ((i < 12u) ? 0.0f : (0.2f * I_TO_F((i - 12u) % 8u))))

            if(i < 12u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((i % 3u) == 1u) ? 0.3f : 0.0f, ((i % 3u) == 2u) ? -0.3f : 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  3u) {}
                else if(i <  6u) pEnemy->Rotate90 ();
                else if(i <  9u) pEnemy->Rotate180();
                else if(i < 12u) pEnemy->Rotate270();
            }
            else if(i < 28u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 12u) % 8u) - 3.5f) * 0.25f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                pEnemy->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            }

            if(STAGE_TICK_LIFETIME(9.0f, 0.0f) && ((s_iTick % 9u) >= 6u))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
            }
        });

        STAGE_WAVE("SIEBENUNDFÜNFZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all waves
    STAGE_MAIN
    {
        this->DisableWave(false);

        g_pGame->GetHelper(ELEMENT_RED)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Messier, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}