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
// constructor
cTinkerMission::cTinkerMission()noexcept
{
    // 
    m_apBoss[0] = &m_ProjectOne;
    m_apBoss[1] = &m_ProjectOne;
    m_apBoss[2] = &m_ProjectOne;
}


// ****************************************************************
// setup the Tinker mission
void cTinkerMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetSpeed(4.0f);
            g_pGame->StartIntro();
        }

        STAGE_FINISH_AFTER(GAME_INTRO_DURATION)
    });

    /*
    // ################################################################
    // template
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.25f, 0.2f * I_TO_F(i))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });
    */

    // ################################################################
    // enemies move and shoot with player 
    // (different attacks) 
    // different speeds and offsets 
    // different flips XY/X/Y 
    // different rotation offsets 0/90/180/270 
    // invert shoot 
    // coop: average position 
    STAGE_MAIN
    {
        //STAGE_ADD_PATH(pPath1)
        //{
        //    pPath1->Reserve(2u);
        //    pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->Refine();
        //});

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)     
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.3f,1.3f,1.3f));
                pEnemy->Configure(30, COLOR_SHIP_ORANGE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 5u, 7u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 1u, 4u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 3u, 3u)
        }

        const cPlayer*    pPlayer    = g_pGame->GetPlayer(0u);
        const coreVector2 vBasePos   = pPlayer->GetPosition ().xy() / FOREGROUND_AREA;
        const coreVector2 vBaseDir   = pPlayer->GetDirection().xy();
        const coreBool    bBaseShoot = pPlayer->GetInput()->iActionHold & BITLINE(WEAPON_MODES);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vNewPos   = vBasePos;
            coreVector2 vNewDir   = vBaseDir;
            coreBool    bNewShoot = bBaseShoot;

            switch(i)
            {
            default: ASSERT(false)
            case 0u: vNewPos = -vNewPos; vNewDir = -vNewDir; break;

            case 1u: vNewPos.x =  0.9f; vNewDir = coreVector2(-1.0f, 0.0f); break;
            case 2u: vNewPos.x = -0.9f; vNewDir = coreVector2( 1.0f, 0.0f); break;
            case 3u: vNewPos.y =  0.9f; vNewDir = coreVector2( 0.0f,-1.0f); break;
            case 4u: vNewPos.y = -0.9f; vNewDir = coreVector2( 0.0f, 1.0f); break;

            case 5u: vNewPos = -vNewPos * 0.5f + coreVector2(-0.5f,0.5f); vNewDir = -vNewDir; break;
            case 6u: vNewPos = -vNewPos * 0.5f + coreVector2( 0.5f,0.5f); vNewDir = -vNewDir; break;
            case 7u: vNewPos = -vNewPos * 0.5f + coreVector2( 0.0f,0.0f); vNewDir = -vNewDir; break;
            }

            pEnemy->SetPosition (coreVector3(vNewPos * FOREGROUND_AREA, 0.0f));
            pEnemy->SetDirection(coreVector3(vNewDir,                   0.0f));

            if(bNewShoot && STAGE_TICK_LIFETIME(18.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 2.0f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // enemies survive invincible and merge 
    STAGE_MAIN
    {
        //STAGE_ADD_PATH(pPath1)
        //{
        //    pPath1->Reserve(2u);
        //    pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->Refine();
        //});

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 31u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);

                if(i < 16u) pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(76u)
            STAGE_GET_FLOAT_ARRAY(afMergeTime, 15u)
            STAGE_GET_VEC2_ARRAY (avMergePos,  30u)
            STAGE_GET_UINT       (iValid)
        STAGE_GET_END

            /*
        const auto nTargetFunc = [](const coreUintW i)
        {
            coreVector2 vTarget;
                 if(i < 16u) vTarget = coreVector2((I_TO_F(i % 8u)  - 3.5f) * 0.2f, (i < 8) ? 0.0f : 0.1f);
            else if(i < 24u) vTarget = coreVector2((I_TO_F(i - 16u) - 3.5f) * 0.2f, 0.2f);
            else if(i < 28u) vTarget = coreVector2((I_TO_F(i - 24u) - 1.5f) * 0.3f, 0.4f);
            else if(i < 30u) vTarget = coreVector2((I_TO_F(i - 28u) - 0.5f) * 0.4f, 0.6f);
            else             vTarget = coreVector2(0.0f,                            0.8f);

            return vTarget;
        };
        */
        const auto nTargetFunc = [](const coreUintW i)
        {
            coreVector2 vTarget;
                 if(i < 16u) {vTarget = coreVector2((I_TO_F(i % 8u)  - 3.5f) * 0.2f, (i < 8) ? 0.6f : 0.7f);                                                         /*if(i % 2u) vTarget = vTarget;*/}
            else if(i < 24u) {vTarget = coreVector2((I_TO_F(i - 16u) - 3.5f) * 0.2f, 0.6f);                   vTarget = vTarget.Rotated90();                         /*if(i % 2u) vTarget = vTarget;*/}
            else if(i < 28u) {vTarget = coreVector2((I_TO_F(i - 24u) - 1.5f) * 0.3f, 0.6f);                   vTarget = vTarget.Rotated90().Rotated90();             /*if(i % 2u) vTarget = vTarget;*/}
            else if(i < 30u) {vTarget = coreVector2((I_TO_F(i - 28u) - 0.5f) * 0.4f, 0.6f);                   vTarget = vTarget.Rotated90().Rotated90().Rotated90(); /*if(i % 2u) vTarget = vTarget;*/}
            else              vTarget = coreVector2(0.0f,                            0.8f);

            return vTarget;
        };

        for(coreUintW i = 0u; i < 15u; ++i)
        {
            const coreUintW iIndex1 = i * 2u;
            const coreUintW iIndex2 = i * 2u + 1u;

            cEnemy* pEnemy1 = pSquad1->GetEnemy(iIndex1);
            cEnemy* pEnemy2 = pSquad1->GetEnemy(iIndex2);

            if(CONTAINS_FLAG(pEnemy1->GetStatus(), ENEMY_STATUS_DEAD))
            {
                pEnemy1->AddStatus(ENEMY_STATUS_GHOST);
                continue;
            }
            if(CONTAINS_FLAG(pEnemy2->GetStatus(), ENEMY_STATUS_DEAD)) {pEnemy2->AddStatus(ENEMY_STATUS_GHOST); continue;}

            if(!CONTAINS_BIT(iValid, i))
            {
                if(pEnemy1->GetCurHealth() != 0u) continue;
                if(pEnemy2->GetCurHealth() != 0u) continue;

                avMergePos[iIndex1] = pEnemy1->GetPosition().xy();
                avMergePos[iIndex2] = pEnemy2->GetPosition().xy();

                ADD_BIT(iValid, i)
            }
        }

        for(coreUintW i = 0u; i < 15u; ++i)
        {
            if(!CONTAINS_BIT(iValid, i)) continue;

            const coreUintW iIndex1 = i * 2u;
            const coreUintW iIndex2 = i * 2u + 1u;

            cEnemy* pEnemy1 = pSquad1->GetEnemy(iIndex1);
            cEnemy* pEnemy2 = pSquad1->GetEnemy(iIndex2);

            const coreUintW iNewIndex = (i < 8u) ? (16u + i) : ((i < 12u) ? (24u + i - 8u) : ((i < 14u) ? (28u + i - 12u) : 30u));

            coreFloat& fTime = afMergeTime[i];
            fTime += 1.0f * Core::System->GetTime();

            if(fTime >= 1.0f)
            {
                pEnemy1->Kill(true);
                pEnemy2->Kill(false);

                pSquad1->GetEnemy(iNewIndex)->Resurrect();

                REMOVE_BIT(iValid, i)
            }
            else
            {
                const coreVector2 vPos1 = avMergePos[iIndex1];
                const coreVector2 vPos2 = avMergePos[iIndex2];

                const coreVector2 vTarget = nTargetFunc(iNewIndex) * FOREGROUND_AREA;

                pEnemy1->SetPosition(coreVector3(LERPB(vTarget, vPos1, 1.0f - fTime), 0.0f));
                pEnemy2->SetPosition(coreVector3(LERPB(vTarget, vPos2, 1.0f - fTime), 0.0f));

                const coreVector2 vDir = coreVector2::Direction(LERPB(5.0f*PI, 1.0f*PI, 1.0f - fTime));
                
                pEnemy1->SetDirection(coreVector3(vDir, 0.0f));
                pEnemy2->SetDirection(coreVector3(vDir, 0.0f));
            }
        }


        // set valid, set mergepos 

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            //STAGE_REPEAT(pPath1->GetTotalDistance())

            //const coreVector2 vFactor = coreVector2((i < 8u) ? -1.0f : 1.0f, (i < 8u) ? -1.0f : 1.0f);
            //const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            //pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            const coreUintW iMergeIndex = i / 2u;

            if(!CONTAINS_BIT(iValid, iMergeIndex))
                pEnemy->SetPosition(coreVector3(nTargetFunc(i) * FOREGROUND_AREA, 0.0f));

        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // enemies survive invincible
    // - all: delay attack, to get players accustomed
    // FAILURE 
    STAGE_MAIN
    {//bewegung entfernen, zu ähnlich wie 1, links rechts kreuzen                                                                               
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,1.0f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 44u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iShootOrder)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 19u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 20u, 43u)
        }

        coreBool  bActive    = false;
        cEnemy*   pTarget    = NULL;
        coreFloat fTargetVal = FLT_MAX;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 20u)
            {
                STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

                const coreFloat fPosX = I_TO_F(i % 10u) * 0.2f - 0.9f;

                if(STAGE_BRANCH(pPath1->GetTotalDistance(), 4.0f))
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(fPosX, 0.0f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                }
                else
                {
                    const coreFloat fValue = FMOD(fLifeTime + ((i < 10u) ? 1.0f : 0.0f) + 1.0f, 4.0f);
                    const coreFloat fPosY  = (fValue < 2.0f) ? LERPS(-1.0f, 1.0f, MIN(fValue, 1.0f)) : (LERPS(1.0f, -1.0f, MIN(fValue-2.0f, 1.0f)));

                    pEnemy->SetPosition(coreVector3(fPosX, fPosY, 0.0f) * FOREGROUND_AREA3);
                }

                if(i < 10u) pEnemy->Rotate90();
            }
            else
            {
                STAGE_LIFETIME(pEnemy, 0.5f, 0.2f * I_TO_F((i-20u) % 12u) - LERPB(0.0f, 2.0f, MIN(pEnemy->GetLifeTime() * 0.5f, 1.0f)))

                STAGE_REPEAT(pPath2->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f, ((i-20u) < 12u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = coreVector2((((i-20u) < 12u) ? -1.0f : 1.0f) * (0.1f + LERPB(1.0f, 0.0f, MIN(pEnemy->GetLifeTime() * 0.5f, 1.0f))), 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(pEnemy->GetCurHealth())
            {
                bActive = true;
            }
            else
            {
                if(STAGE_TICK_TIME_FREE(10.0f, 0.0f))
                {
                    const coreFloat fCurValue = (i < 20u) ? pEnemy->GetPosition().arr((i < 10u) ? 1u : 0u) : pEnemy->GetPosition().xy().LengthSq();
                    if(fCurValue < fTargetVal)
                    {
                        pTarget    = pEnemy;
                        fTargetVal = fCurValue;
                    }
                }
            }
        });

        if(STAGE_TICK_TIME_FREE(2.0f, 0.0f))
        {
            cEnemy*     pEnemy;
            coreVector2 vPos;
            coreVector2 vDir;

            if(++iShootOrder >= 10u) iShootOrder = 0u;
            const coreBool  bType   = (iShootOrder % 2u) ? true : false;
            const coreFloat fHeight = I_TO_F((iShootOrder * 3u) % 10u) * 0.2f - 0.9f;

            if(m_iStageSub == 1u)
            {
                pEnemy = pSquad1->GetEnemy(bType ? 0u : 10u);
                vPos   = bType ? coreVector2(pEnemy->GetPosition().x, fHeight * FOREGROUND_AREA.y) : coreVector2(fHeight * FOREGROUND_AREA.x, pEnemy->GetPosition().y);
                vDir   = bType ? coreVector2(-SIGN(pEnemy->GetPosition().x), 0.0f)                 : coreVector2(0.0f, -SIGN(pEnemy->GetPosition().y));
            }
            else
            {
                pEnemy = pSquad1->GetEnemy(bType ? 20u : 32u);
                vPos   = coreVector2(pEnemy->GetPosition().x, fHeight * FOREGROUND_AREA.y);
                vDir   = coreVector2(SIGN(pEnemy->GetPosition().x), 0.0f);
            }

            if(pEnemy->GetLifeTime() >= 1.5f)
            {
                for(coreUintW i = 8u; i--; )
                {
                    const coreVector2 vOffset = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) + 0.5f) * 45.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, pEnemy, vPos + 5.0f * vOffset, vDir)->ChangeSize(1.2f);
                }
            }
        }

        if(!bActive)
        {
            if(pTarget) pTarget->Kill(true);

            if((m_iStageSub == 1u) && pSquad1->IsFinished())
            {
                iShootOrder = 0u;
                return;
            }
        }

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // bending away from bullets 
    STAGE_MAIN
    {
        //STAGE_ADD_PATH(pPath1)
        //{
        //    pPath1->Reserve(2u);
        //    pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->Refine();
        //});

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 24u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, COLOR_SHIP_RED);
            });

             pSquad1->GetEnemy(0u)->SetPosition(coreVector3(0.0f,0.0f,0.0f) * FOREGROUND_AREA3);
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vOffset = coreVector2(0.0f,0.0f);

            g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                 coreVector2 vDiff  = vOffset - pBullet->GetPosition().xy(); vDiff.y *= 0.0f;
                const coreFloat   fLenSq = vDiff.LengthSq();

                if(fLenSq < 100.0f)
                {
                    vOffset += vDiff.Normalized() * SQRT(100.0f - fLenSq);
                }
            });

            pEnemy->SetPosition(coreVector3(LERP(pEnemy->GetPosition().xy(), vOffset, 2.0f * Core::System->GetTime()), 0.0f));
            
            pEnemy->SetPosition(pEnemy->GetPosition() * coreVector3(1.0f,0.0f,0.0f));

        });

        

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})

        // der ausweichende gegner weicht aus sobald er getroffen wird
    });

    // ################################################################
    // gets moved by shooting 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(0.0f,0.0f,0.0f));
                pEnemy->Configure  (2000, COLOR_SHIP_ORANGE);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vForce)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, COLL_REF(vForce))
        {
            //vForce += (pEnemy->GetPosition().xy() - pBullet->GetPosition().xy()).Normalized(pBullet->GetFlyDir()) * 10.0f;
            vForce -= AlongCrossNormal(pBullet->GetFlyDir()) * 5.0f;
        });

        
            

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vNewPos = pEnemy->GetPosition().xy();

            if(!vForce.IsNull())
            {
                vNewPos  += vForce * Core::System->GetTime();
                vForce *= FrictionFactor(3.0f);
            }

            
                 if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; vForce.x =  ABS(vForce.x);}
            else if(vNewPos.x >  FOREGROUND_AREA.x) {vNewPos.x =  FOREGROUND_AREA.x; vForce.x = -ABS(vForce.x);}
                 if(vNewPos.y < -FOREGROUND_AREA.y) {vNewPos.y = -FOREGROUND_AREA.y; vForce.y =  ABS(vForce.y);}
            else if(vNewPos.y >  FOREGROUND_AREA.y) {vNewPos.y =  FOREGROUND_AREA.y; vForce.y = -ABS(vForce.y);}

            pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})

        // pushing enemies are on left and right side and shooting on them will move them to the other side, they die when reaching the other side (is only second wave?)
    });

    // ################################################################
    // laser rows from both axis 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.9f), coreVector2(1.0f,0.0f));
            pPath1->AddNode(coreVector2( 1.2f,0.9f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.9f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.9f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_BLUE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 23u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 24u, 35u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.3f, 0.4f * I_TO_F(i % 6u))

            const coreSpline2* pPath = (i < 12u || i >= 24u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? 1.0f : -1.0f, (i < 6u || i >= 12u) ? ((i < 24u) ? 1.0f : 0.0f) : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, (i < 24u) ? 0.0f : ((i < 30u) ? -0.1f : 0.1f));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);


            if(i % 2u)
            {
                pEnemy->SetPosition (pEnemy->GetPosition ().RotatedZ90() * coreVector3(1.0f,1.0f,1.0f));
                pEnemy->SetDirection(pEnemy->GetDirection().yxz());
            }


            if(STAGE_TICK_LIFETIME(30.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                if(i < 24u) g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([](cConeBullet* OUTPUT pBullet)
        {
            cShip* pOwner = pBullet->GetOwner();
            const coreVector2 vDir = pBullet->GetFlyDir();

            if(IsHorizontal(vDir)) pBullet->SetPosition(coreVector3(pBullet->GetPosition().x, pOwner ->GetPosition().y, pBullet->GetPosition().z));
                              else pBullet->SetPosition(coreVector3(pOwner ->GetPosition().x, pBullet->GetPosition().y, pBullet->GetPosition().z));
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // enemy leaves damaging area behind on death 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.25f, 0.2f * I_TO_F(i))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // laying mines in path 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(100, COLOR_SHIP_YELLOW);
            });

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3(-0.8f,1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3(-0.8f,1.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(2u)->SetPosition(coreVector3(-0.8f,1.6f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(3u)->SetPosition(coreVector3( 0.8f,1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(4u)->SetPosition(coreVector3( 0.8f,1.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(5u)->SetPosition(coreVector3( 0.8f,1.6f,0.0f) * FOREGROUND_AREA3);

            pSquad1->GetEnemy(0u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(1u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(2u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(3u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(4u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(5u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const cPlayer*    pPlayer = pEnemy->NearestPlayer();
            const coreVector2 vDiff   = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();

            if(coreVector2::Dot(vDiff, pEnemy->GetDirection().xy()) <= 0.0f)
            {
                if(pEnemy->GetDirection().x == 0.0f) pEnemy->SetDirection(coreVector3(SIGN(vDiff.x), 0.0f, 0.0f));
                                                else pEnemy->SetDirection(coreVector3(0.0f, SIGN(vDiff.y), 0.0f));
            }

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 30.0f);


            if(STAGE_TICK_TIME_FREE(3.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cMineBullet>(5, 0.0f, pEnemy, vPos, vDir);
            }
        });

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            pBulletPlayer->Deactivate(true, vIntersection.xy());

            const coreInt32 iDamage = pBulletEnemy->GetDamage();
            if(iDamage == 1) pBulletEnemy->Deactivate(true, vIntersection.xy());
                        else pBulletEnemy->SetDamage(iDamage - 1);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // wave-like movement with lasers 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 24u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_RED);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.5f, 0.2f * I_TO_F(i % 12u))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(((i < 12u) ? -1.0f : 1.0f) * 0.3f + 0.4f * SIN(2.0f*PI * I_TO_F(i % 12u) / 12.0f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            pEnemy->SetDirection(coreVector3((i < 12u) ? -1.0f : 1.0f, 0.0f, 0.0f));

            if(STAGE_TICK_TIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 2.0f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([](cConeBullet* OUTPUT pBullet)
        {
            pBullet->SetPosition(pBullet->GetPosition() + coreVector3(0.0f, Core::System->GetTime() * FOREGROUND_AREA.y * -1.5f, 0.0f));
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // bullet-lines from the side 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(100, COLOR_SHIP_YELLOW);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 2u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.2f * I_TO_F((i % 8u) + (((i % 8u) < 4u) ? 0u : 2u)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2((i < 8u) ? -1.0f : 1.0f, (i < 8u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        if(STAGE_TICK_TIME_FREE(10.0f, 0.0f))
        {
            const coreFloat fHeight = 1.1 * (1.0f - 2.0f * (I_TO_F((s_iTick-1u) % 41u) / 40.0f));

            const coreVector2 vPos = coreVector2(1.2f, fHeight) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2(1.0f, 0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.0f, NULL,  vPos, -vDir)->ChangeSize(1.2f); 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.0f, NULL, -vPos,  vDir)->ChangeSize(1.2f); 
        }

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // move to side when crossing player (Ikaruga) 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(100, COLOR_SHIP_YELLOW);
            });

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3(-0.8f,1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3(-0.8f,1.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(2u)->SetPosition(coreVector3(-0.8f,1.6f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(3u)->SetPosition(coreVector3( 0.8f,1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(4u)->SetPosition(coreVector3( 0.8f,1.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(5u)->SetPosition(coreVector3( 0.8f,1.6f,0.0f) * FOREGROUND_AREA3);

            pSquad1->GetEnemy(0u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(1u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(2u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(3u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(4u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(5u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 2u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const cPlayer*    pPlayer = pEnemy->NearestPlayer();
            const coreVector2 vDiff   = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();

            if(coreVector2::Dot(vDiff, pEnemy->GetDirection().xy()) <= 0.0f)
            {
                if(pEnemy->GetDirection().x == 0.0f) pEnemy->SetDirection(coreVector3(SIGN(vDiff.x), 0.0f, 0.0f));
                                                else pEnemy->SetDirection(coreVector3(0.0f, SIGN(vDiff.y), 0.0f));
            }

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 30.0f);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // main-enemy with surrounding sub-enemies 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(0.0f,0.0f,0.0f));   
                pEnemy->Configure  (100, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus  (ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_ORANGE);

                pEnemy->Resurrect();
            });
        });

        cEnemy*         pParent = pSquad1->GetEnemy(0u);
        const coreFloat fBase   = pParent->GetDirection().xy().Angle();

        if(pSquad2->IsFinished())
        {
            pParent->Kill(true);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {

            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayer()->GetPosition().xy(), 20.0f, 2.0f);
            

        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            

            const coreFloat fAngle = fBase + m_fStageTime + 2.0f*PI * I_TO_F(i) / 5.0f;

            const coreVector2 vDir = coreVector2::Direction(fAngle);
            const coreVector2 vPos = pParent->GetPosition().xy() + vDir * 10.0f;

            pEnemy->SetPosition (coreVector3(vPos,              0.0f));
            pEnemy->SetDirection(coreVector3(-vDir.Rotated90(), 0.0f));

        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // shotting axis-aligned from all sides
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 7u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.15f);
                pEnemy->Configure(40, COLOR_SHIP_RED);
            });

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3( 1.2f,-0.9f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3(-0.9f,-1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(2u)->SetPosition(coreVector3( 0.9f, 1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(3u)->SetPosition(coreVector3(-1.2f, 0.9f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(4u)->SetPosition(coreVector3( 0.9f, 1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(5u)->SetPosition(coreVector3( 1.2f,-0.9f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(6u)->SetPosition(coreVector3(-0.9f,-1.2f,0.0f) * FOREGROUND_AREA3);

            pSquad1->GetEnemy(0u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy(1u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(2u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(3u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(4u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(5u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy(6u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 3u, 6u)
        }

        STAGE_GET_START(8u)
            STAGE_GET_FLOAT_ARRAY(afOldOffset, 4u)
            STAGE_GET_FLOAT_ARRAY(afNewOffset, 4u)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i == 2u || i == 3u || i == 4u) ? 1.0f : 0.0f)

            STAGE_REPEAT(1.5f)

            const coreBool bRotate = (i == 1u || i == 2u || i == 4u || i == 6u);

            if(STAGE_TAKEOFF)
            {
                afOldOffset[i % 4u] = pEnemy                 ->GetPosition().arr(bRotate);
                afNewOffset[i % 4u] = pEnemy->NearestPlayer()->GetPosition().arr(bRotate);
            }
            else if(STAGE_LIFETIME_BEFORE(1.0f))
            {
                const coreVector2 vOldPos = coreVector2(afOldOffset[i % 4u], pEnemy->GetPosition().arr(!bRotate)) / FOREGROUND_AREA;
                const coreVector2 vNewPos = coreVector2(afNewOffset[i % 4u], pEnemy->GetPosition().arr(!bRotate)) / FOREGROUND_AREA;
                const coreFloat   fSign   = SIGN(coreVector2::Dot(pEnemy->GetDirection().xy(), coreVector2(-1.0f,1.0f)) * (afNewOffset[i % 4u] - afOldOffset[i % 4u]));
                const coreFloat   fTime   = LERPB(0.0f, 1.0f, fLifeTime);

                pEnemy->DefaultMoveLerp     (vOldPos, vNewPos,         fTime);
                pEnemy->DefaultOrientateLerp(0.0f*PI, 2.0f*PI * fSign, fTime);

                if(bRotate) pEnemy->SetPosition(pEnemy->GetPosition().yxz());
            }
            else if(STAGE_LIFETIME_BEFORE(1.5f))
            {
                if(STAGE_TICK_LIFETIME(12.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();
                    const coreVector2 vTan = vDir.Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.8f, pEnemy, vPos + vTan*1.5f, vDir)->ChangeSize(1.2f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.8f, pEnemy, vPos - vTan*1.5f, vDir)->ChangeSize(1.2f);
                }
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // invincible enemy blocking path
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.0f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.4f),  coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.75f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(200, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();

                pEnemy->GiveShield(ELEMENT_BLUE);             
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cStarEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fShootAngle, fShootAngle = 0.5f*PI)
        STAGE_GET_END

        if(pSquad2->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad2,  0u,  4u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad2,  5u,  9u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad2, 10u, 14u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad2, 15u, 19u)
            else if(STAGE_SUB(5u)) pSquad1->GetEnemy(0u)->Kill(true);
        }

        if(m_iStageSub >= 3u)
        {
            fShootAngle += 0.4f*PI * Core::System->GetTime();
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_TIME(12.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = coreVector2::Direction(fShootAngle);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, I_TO_F(i % 5u) * 0.1f)

            const coreVector2 vFlyDir = coreVector2::Direction(DEG_TO_RAD(((i % 5u) + (((i % 10u) < 5u) ? 10u : 4u)) * 30.0f));
            const coreVector2 vFlyPos = vFlyDir * pPath2->CalcPosition(CLAMP(fLifeTime, 0.0f, pPath2->GetTotalDistance())).y;

            pEnemy->SetPosition(coreVector3(vFlyPos * FOREGROUND_AREA, 0.0f));

            if(STAGE_TICK_LIFETIME(1.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // side-attacking enemy lines
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(1.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(1.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_BLUE);
                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.2f * I_TO_F((i % 8u) + (((i % 8u) < 4u) ? 0u : 2u)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2((i < 8u) ? -1.0f : 1.0f, (i < 8u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_TIME(0.9f, (i < 8u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = coreVector2(-SIGN(pEnemy->GetPosition().x), 0.0f);
                const coreVector2 vTan = vDir.Rotated90() * FOREGROUND_AREA;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos + vTan * 0.05f, vDir)->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos - vTan * 0.05f, vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // ################################################################
}


#if 0

void UnusedWaves()
{
    // ################################################################
    // turning and constantly shooting enemy in center 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(2.0f,2.0f,2.0f));
                pEnemy->Configure(200, COLOR_SHIP_CYAN);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vShootDir, vShootDir = coreVector2(0.0f,-1.0f))
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->SetPosition(coreVector3(0.0f,0.0f,0.0f));

            const coreVector2 vTarget = pEnemy->AimAtPlayer().Normalized();
            const coreFloat   fPower  = coreVector2::Dot(vTarget, vShootDir);
            const coreFloat   fSign   = (fPower > 0.0f) ? coreVector2::Dot(vTarget, vShootDir.Rotated90()) : coreVector2::Dot(vTarget, -vShootDir.Rotated90());

            // sudden turn of enemy after moving to a certain threshold is shit, either use it as wall, or constantly rotate  

            vShootDir = coreVector2::Direction(vShootDir.Angle() + MIN(ABS(fPower) * 5.0f, 1.0f) * SIGN(fSign) * 2.0f * Core::System->GetTime());

            if(STAGE_TICK_TIME(12.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = vShootDir.Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // infinity flying enemy shooting on teleport
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition (coreVector3( 0.5f, 1.2f,0.0f) * FOREGROUND_AREA3);
                pEnemy->SetSize     (coreVector3( 1.3f, 1.3f,1.3f));
                pEnemy->SetDirection(coreVector3(-1.0f,-2.0f,0.0f).Normalized());
                pEnemy->Configure   (200, COLOR_SHIP_ORANGE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 40.0f);

            const auto nShootFunc = [&](const coreVector2& vPos, const coreVector2& vDir)
            {
                const coreVector2 vTan = vDir.Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos + vTan * 6.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos + vTan * 2.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos - vTan * 2.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos - vTan * 6.0f, vDir);
            };

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) {nShootFunc(vCurPos, coreVector2( 1.0f, 0.0f)); vCurPos.x =  ABS(vCurPos.x); nShootFunc(vCurPos, coreVector2(-1.0f, 0.0f));}
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) {nShootFunc(vCurPos, coreVector2(-1.0f, 0.0f)); vCurPos.x = -ABS(vCurPos.x); nShootFunc(vCurPos, coreVector2( 1.0f, 0.0f));}
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) {nShootFunc(vCurPos, coreVector2( 0.0f, 1.0f)); vCurPos.y =  ABS(vCurPos.y); nShootFunc(vCurPos, coreVector2( 0.0f,-1.0f));}
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) {nShootFunc(vCurPos, coreVector2( 0.0f,-1.0f)); vCurPos.y = -ABS(vCurPos.y); nShootFunc(vCurPos, coreVector2( 0.0f, 1.0f));}

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
        });

        STAGE_WAVE
    });

    // ################################################################
    // infinity flying enemy shooting from his side (TODO: Zero Ranger effect)  
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition (coreVector3( 0.5f, 1.2f,0.0f) * FOREGROUND_AREA3);
                pEnemy->SetSize     (coreVector3( 1.3f, 1.3f,1.3f));
                pEnemy->SetDirection(coreVector3(-1.0f,-2.0f,0.0f).Normalized());
                pEnemy->Configure   (200, COLOR_SHIP_ORANGE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 40.0f);

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) vCurPos.x =  ABS(vCurPos.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) vCurPos.x = -ABS(vCurPos.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) vCurPos.y =  ABS(vCurPos.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) vCurPos.y = -ABS(vCurPos.y);

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));

            if(STAGE_TICK_TIME(14.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.8f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.8f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // two side-shooting caterpillars winding down
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_RED);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.22f * I_TO_F(i % 8u))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i < 8u) ? -0.5f : 0.5f, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_TIME(1.8f, (i < 8u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos,  vDir)->ChangeSize(1.8f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos, -vDir)->ChangeSize(1.8f);
            }

            /*
            for(coreUintW j = 0u; j < 13u; ++j)
            {
                if((i < 8u) ? STAGE_POSITION_POINT(pEnemy, (1.0f - I_TO_F(j) / 6.0f)                * FOREGROUND_AREA.y * 1.1f, y) :
                              STAGE_POSITION_POINT(pEnemy, (1.0f - I_TO_F(j) / 6.0f - 1.0f / 12.0f) * FOREGROUND_AREA.y * 1.1f, y))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos,  vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos, -vDir)->ChangeSize(1.8f);
                }
            }
            */
        });

        STAGE_WAVE
    });

    // ################################################################
    // split by bullets into four areas 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(1.2f,1.2f,0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (20, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cMinerEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 3u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 4u, 7u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, I_TO_F(i) * 0.2f)

            if(STAGE_TAKEOFF)
            {
               // pEnemy->SetPosition(coreVector3(coreVector2(1.2f,1.2f) * FOREGROUND_AREA, 0.0f));
            }

            if(STAGE_LIFETIME_AFTER(0.0f))
            {
                const coreVector2 vPos = coreVector2((i % 2u) ? -1.0f : 1.0f, (i / 2u) ? -1.0f : 1.0f) * 0.1f + coreVector2(0.8f,0.8f);

                pEnemy->DefaultMoveSmooth(vPos, 10.0f, 1.0f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, I_TO_F(i) * 0.2f)

            const coreVector2 vFrom = (i ? coreVector2(1.3f,1.2f) : coreVector2(1.2f,1.3f));
            const coreVector2 vTo   = (i ? coreVector2(1.3f,0.0f) : coreVector2(0.0f,1.3f));

            pEnemy->DefaultMoveLerp(vFrom, vTo, LERP(0.0f, 1.0f, MIN(fLifeTime * 0.6f, 1.0f)));

            if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = (ABS(vPos.x) > ABS(vPos.y)) ? coreVector2(-SIGN(vPos.x), 0.0f) : coreVector2(0.0f, -SIGN(vPos.y));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos, vDir);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // split by bullets into nine areas 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(20, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cMinerEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                const coreVector2 vDir = StepRotated90(i);

                pEnemy->SetPosition (coreVector3(-vDir * (FOREGROUND_AREA * 1.3f) + vDir.Rotated90() * (FOREGROUND_AREA * 0.4f), 0.0f));
                pEnemy->SetDirection(coreVector3( vDir, 0.0f));
                pEnemy->Configure   (20, COLOR_SHIP_BLUE);
                pEnemy->AddStatus   (ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iGroup)
        STAGE_GET_END

        // OLD
        if(pSquad1->IsFinished())
        {
            if(iGroup * 4u < pSquad1->GetNumEnemies())
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
                {
                    if((i >= iGroup * 4u) && (i < (iGroup+1u) * 4u))
                    {
                        pEnemy->Resurrect();
                        pEnemy->SetPosition(coreVector3(0.0f,1.1f,0.0f) * FOREGROUND_AREA3);
                    }
                });
            }

            iGroup += 1u;
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {

            pEnemy->DefaultMoveSmooth(coreVector2(0.0f, 0.3f), 10.0f, 1.0f);


        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vDir = StepRotated90(i);

            if(STAGE_TICK_LIFETIME(6.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos, vDir);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // move to last player location 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(0.0f,1.1f,0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (200, COLOR_SHIP_ORANGE);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vLastPosition, vLastPosition = g_pGame->GetPlayer(0u)->GetPosition().xy())
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy->GetPosition().xy() - vLastPosition).LengthSq() < 1.0f)
            {
                vLastPosition = g_pGame->GetPlayer(0u)->GetPosition().xy();

                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 20u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    // TODO: lower blue balls get overlapped 
                }
            }

            const coreVector2 vDiff  = vLastPosition - pEnemy->GetPosition().xy();
            const coreFloat   fLenSq = vDiff.LengthSq(); danger
            const coreVector2 vDir   = vDiff.Normalized(); danger
            const coreVector2 vPos   = pEnemy->GetPosition().xy() + vDir * (Core::System->GetTime() * 20.0f * SmoothTowards(fLenSq, 5.0f));

            pEnemy->SetPosition (coreVector3(vPos, 0.0f));
            pEnemy->SetDirection(coreVector3(vDir, 0.0f));
        });

        STAGE_WAVE
    });

    // ################################################################
    // invincibility switches every few seconds (old version)
    // blinken wie in zelda  
    // fixed position  
    // position based on player position   
    // snake like enemy-group  
    STAGE_START_HERE
    STAGE_MAIN
    {
        // special already, ball gets caught! in every group, to change pattern!!!  
        // diamond in r-type fish 
        // boss taucht dann von unten auf (drehungs-effekt), und banner inkl breakout steine(!) von oben geht los
        if(STAGE_BEGINNING) this->EnableBall(0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized()); // TODO: adapt for single fight    

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 60u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                //pEnemy->SetPosition(coreVector3(I_TO_F(i - 2u) * 0.2f, 1.1f, 0.0f) * FOREGROUND_AREA3);
                pEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure  (4, COLOR_SHIP_GREY);
                pEnemy->AddStatus  (ENEMY_STATUS_GHOST);
            });
        });

        STAGE_GET_START(3u)
            //STAGE_GET_UINT64(iState, iState = 0b0000'000000'000000'000000'000000'111111000000'000000111111'101010010101)
            STAGE_GET_UINT64(iState, iState = 0b0000'000000'000000'000000'000000'111111000000'000000111111'110011110011)
            STAGE_GET_UINT  (iCount)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection)//, COLL_VAL(pSquad1))
        {
            //const coreUintW i = pSquad1->GetIndex(pEnemy);

            if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
            {
                //const coreVector2 vPos = pEnemy->GetPosition ().xy();
                //const coreVector2 vDir = pEnemy->GetDirection().xy();

               //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u, 11u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 12u, 35u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 36u, 59u)
        }

        const coreBool bSwitch     = (FRACT(m_fStageTime*1.0f) < FRACT(m_fStageTimeBefore*1.0f)) ? true : false;
        const coreBool bSwitchFast = (FRACT(m_fStageTime*4.0f) < FRACT(m_fStageTimeBefore*4.0f)) ? true : false;

        if(m_iStageSub < 3u)
        {
            if(bSwitch)
            {
                iState = ~iState;
            }
        }
        else
        {
            if(bSwitchFast)
            {
                iCount += 1u;
                iState = BIT(((iCount + 0u) % 24u) + 36u) | BIT(((iCount + 12u) % 24u) + 36u) |
                         BIT(((iCount + 1u) % 24u) + 36u) | BIT(((iCount + 13u) % 24u) + 36u);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.2f * I_TO_F(i % 6u))

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((I_TO_F(i % 6u) - 2.5f) * 0.2f + ((i < 12u || i >= 36u) ? 0.0f : 0.25f), (((i % 12u) < 6u) && (i < 36u)) ? 0.0f : 0.22f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);

            if(i == 4u || i == 11u || i == 16u || i == 21u || i == 28u || i == 33u || i == 23u || i == 35u) pEnemy->SetDirection(coreVector3( 1.0f,0.0f,0.0f));
            if(i == 1u || i == 6u  || i == 13u || i == 20u || i == 25u || i == 32u || i == 18u || i == 30u) pEnemy->SetDirection(coreVector3(-1.0f,0.0f,0.0f));

                 if(i < 12u) pEnemy->Rotate90 ();
            else if(i < 24u) pEnemy->Rotate270()->InvertY();
            else if(i < 42u) ;
            else if(i < 48u) pEnemy->Rotate90 ();
            else if(i < 54u) pEnemy->Rotate180();
            else if(i < 60u) pEnemy->Rotate270();

            if((i < 36u) ? bSwitch : bSwitchFast)
            {
                if(CONTAINS_BIT(iState, i))
                {
                    if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                    {
                        //pEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.2f);//0.8f);
                        pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                        pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);
                        //pEnemy->RemoveStatus(ENEMY_STATUS_HIDDEN);
                        pEnemy->SetAlpha(1.0f);

                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->GetDirection().xy();
                        const coreVector2 vTan = vDir.Rotated90();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos + 1.0f*vTan, vDir)->ChangeSize(1.2f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos - 1.0f*vTan, vDir)->ChangeSize(1.2f);
                    }
                }
                else if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_GHOST))
                {
                    //pEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 0.0f);//1.5f);
                    pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                    pEnemy->AddStatus   (ENEMY_STATUS_GHOST);
                    //pEnemy->AddStatus   (ENEMY_STATUS_HIDDEN);
                    pEnemy->SetAlpha(0.4f);

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 5.0f, 3u, COLOR_ENERGY_ORANGE);
                }
            }

            if(pEnemy->ReachedDeath())
            {
                //const coreVector2 vPos  = pEnemy->GetPosition().xy();
                    //const coreFloat   fBase = pEnemy->AimAtPlayer().Angle();
                    //const coreFloat   fBase = pEnemy->GetDirection().xy().Angle();

                    for(coreUintW j = 6u; j--; )
                    {
                        if(j < 2u || j > 3u) continue;

                        //const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.5f) * 3.5f) + fBase);

                        //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
                    }
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });
}

#endif

/*
        const coreVector2 vPos = pEnemy->GetPosition().xy();

        for(coreUintW j = 20u; j--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f));

            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (j & 0x01u) ? 0.6f : 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (j & 0x01u) ? 0.6f : 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
            // TODO: lower blue balls get overlapped 
        }
*/

/*
        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [this](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            pBulletPlayer->Deactivate(true, vIntersection.xy());

            const coreInt32 iDamage = pBulletEnemy->GetDamage();
            if(iDamage == 1) pBulletEnemy->Deactivate(true, vIntersection.xy());
                        else pBulletEnemy->SetDamage(iDamage - 1);
        });
*/

/*
        // perpendicular explosion on border
        const auto nExplode = [&](const coreVector2& vDirection)
        {
            const coreVector2 vPos = pEnemy->GetPosition().xy();
            const coreVector2 vDir = vDirection;
            const coreVector2 vTan = vDirection.Rotated90();

            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, pEnemy, vPos + vTan *  4.0f, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, pEnemy, vPos + vTan * -4.0f, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, pEnemy, vPos + vTan *  2.0f, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, pEnemy, vPos + vTan * -2.0f, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.6f, pEnemy, vPos + vTan *  0.0f, vDir)->ChangeSize(1.7f);

            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.3f, pEnemy, vPos, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.5f, pEnemy, vPos, vDir)->ChangeSize(1.7f);
            //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.6f, pEnemy, vPos, vDir)->ChangeSize(1.7f);
        };

        const coreVector2 vCurPos = pEnemy->GetPosition ().xy();
        coreVector2       vCurDir = pEnemy->GetDirection().xy();

                if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) {pEnemy->Kill(true); nExplode(coreVector2( 1.0f, 0.0f));}//vCurDir.x =  ABS(vCurDir.x);
        else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) {pEnemy->Kill(true); nExplode(coreVector2(-1.0f, 0.0f));}//vCurDir.x = -ABS(vCurDir.x);
                if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) {pEnemy->Kill(true); nExplode(coreVector2( 0.0f, 1.0f));}//vCurDir.y =  ABS(vCurDir.y);
        else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) {pEnemy->Kill(true); nExplode(coreVector2( 0.0f,-1.0f));}//vCurDir.y = -ABS(vCurDir.y);

        pEnemy->SetDirection(coreVector3(vCurDir, 0.0f));
*/

/*
        // damage multiple enemies at once
        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, COLL_VAL(pSquad1))
        {
            const coreUintW i = pSquad1->GetIndex(pEnemy);

            for(coreUintW j = coreMath::FloorAlign(i, 8u), je = coreMath::CeilAlign(i + 1u, 8u); j < je; ++j)
            {
                if(j == i) continue;

                //pSquad1->GetEnemy(j)->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()));
            }
        });
*/

/*
        // map bitfield to game area
        constexpr coreUintW iBitfieldNum = POW2(24u) / 32u;

        STAGE_GET_START(iBitfieldNum)
            STAGE_GET_UINT_ARRAY(aiEnabled, iBitfieldNum)
        STAGE_GET_END

        const auto nEnableFunc = [&](const coreVector2& vPos, coreVector2* OUTPUT vNewPos)
        {
            const coreUintW X = F_TO_UI(CLAMP((vPos.x / FOREGROUND_AREA.x) * 11.5f + 11.5f, 0.0f, 23.0f));
            const coreUintW Y = F_TO_UI(CLAMP((vPos.y / FOREGROUND_AREA.y) * 11.5f + 11.5f, 0.0f, 23.0f));
            const coreUintW I = X + Y * 24u;
            const coreUintW B = I / 32u;
            const coreUintW S = I % 32;

            ASSERT(B < iBitfieldNum)

            if(CONTAINS_BIT(aiEnabled[B], S))
                return false;

            vNewPos->x = ((I_TO_F(X) - 11.5f) / 11.5f) * FOREGROUND_AREA.x;
            vNewPos->y = ((I_TO_F(Y) - 11.5f) / 11.5f) * FOREGROUND_AREA.x;

            ADD_BIT(aiEnabled[B], S)
            return true;
        };
*/