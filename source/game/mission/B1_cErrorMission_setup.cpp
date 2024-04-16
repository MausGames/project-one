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
// setup the Error mission
void cErrorMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->StartIntro();
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // ################################################################

    /*
    // ################################################################
    // template
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_PURPLE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        //STAGE_GET_START(u)
        //STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 1u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.2f * I_TO_F(i))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });
    */

    // ################################################################
    // invincibility switches every few seconds (old version)
    // blinken wie in zelda  
    // fixed position  
    // position based on player position   
    // snake like enemy-group  
    STAGE_MAIN({TAKE_ALWAYS})
    {
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
            //STAGE_GET_UINT64(iState, iState = 0b0000'000000'000000'000000'000000'111111000000'000000111111'101010010101u)
            STAGE_GET_UINT64(iState, iState = 0b0000'000000'000000'000000'000000'111111000000'000000111111'110011110011u)
            STAGE_GET_UINT  (iCount)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit)//, COLL_VAL(pSquad1))
        {
            if(!bFirstHit) return;

            //const coreUintW i = pSquad1->GetIndex(pEnemy);

            if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
            {
                //const coreVector2 vPos = pEnemy->GetPosition ().xy();
                //const coreVector2 vDir = pEnemy->GetDirection().xy();

               //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 11u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 12u, 35u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 36u, 59u)
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

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);   // TODO 1: vOffset * vFactor (or # no factor multiplication)

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
                if(HAS_BIT(iState, i))
                {
                    if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
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
                else if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST))
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

    // ################################################################
    // enemies merge into bigger enemies
    // junks begin to attack   
    // destroy junks at end   
    STAGE_START_HERE
    STAGE_MAIN({TAKE_ALWAYS})
    {
        //STAGE_ADD_PATH(pPath1)
        //{
        //    pPath1->Reserve(2u);
        //    pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
        //    pPath1->Refine();
        //});

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 22u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);

                if(i < 12u) pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cCinderEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(0.4f,0.4f,0.4f));
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE); // turn later into invulnarable/reflect, or manual reflect, to not collide with player
            });
        });

        STAGE_GET_START(190u)
            STAGE_GET_VEC2_ARRAY  (avFlyDir,   36u)
            STAGE_GET_VEC2_ARRAY  (avTarget,   36u)
            STAGE_GET_FLOAT_ARRAY (afFlySpeed, 36u)
            STAGE_GET_UINT64_ARRAY(aiType,     5u)
        STAGE_GET_END



        const auto nPosFunc = [](const coreUintW i)
        {
            if(i < 12u) return coreVector2((I_TO_F(i) - 5.5f) * 0.15f, 0.5f) * FOREGROUND_AREA;
            if(i < 18u) return coreVector2((I_TO_F(i - 12u) - 5.5f) * 0.3f, -0.5f) * FOREGROUND_AREA;
            if(i < 21u) return coreVector2(0.0f,0.0f) * FOREGROUND_AREA;
                        return coreVector2(0.0f,0.0f) * FOREGROUND_AREA;
        };

        for(coreUintW i = 12u; i < 22u; ++i)
        {
            const coreUint8 iType  = (i < 18u) ? 0u        : ((i < 21u) ? 1u        : 2u);
            const coreUintW iCount = (i < 18u) ? 6u        : ((i < 21u) ? 12u       : 36u);
            const coreUintW iShift = (i < 18u) ? (i - 12u) : ((i < 21u) ? (i - 18u) : 0u);

            const coreUintW iShiftCount = iShift * iCount;
            const coreUintW iLine = BITLINE(iCount) << iShiftCount;

            if(!!(iType & 0x01u) == HAS_FLAG(aiType[0], iLine) &&
               !!(iType & 0x02u) == HAS_FLAG(aiType[1], iLine))
            {
                if(!HAS_FLAG(aiType[2], iLine) &&
                    HAS_FLAG(aiType[4], iLine))
                {
                    for(coreUintW j = 0u; j < iCount; ++j)
                    {
                        const coreUintW iIndex = iShiftCount + j;
                        avFlyDir  [iIndex] = pSquad2->GetEnemy(iIndex)->GetPosition().xy();
                        afFlySpeed[iIndex] = -0.5f * I_TO_F(j);

                        avTarget[iIndex] = nPosFunc(i);
                    }

                    ADD_FLAG   (aiType[2], iLine)
                    REMOVE_FLAG(aiType[4], iLine)
                }
                else if(HAS_FLAG(aiType[3], iLine))
                {
                    REMOVE_FLAG(aiType[3], iLine)
                    pSquad1->GetEnemy(i)->Resurrect();
                }
            }

        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.3f, 0.2f * I_TO_F(i))

            if(pEnemy->ReachedDeath())
            {

            const coreUint8 iType  = (i < 12u) ? 0u : ((i < 18u) ? 1u        : ((i < 21u) ? 2u        : 3u));
            const coreUintW iCount = (i < 12u) ? 3u : ((i < 18u) ? 6u        : ((i < 21u) ? 12u       : 36u));
            const coreUintW iShift = (i < 12u) ? i  : ((i < 18u) ? (i - 12u) : ((i < 21u) ? (i - 18u) : 0u));

                for(coreUintW j = 0u; j < iCount; ++j)
                {
                    const coreUintW iIndex = iShift * iCount + j;

                    cEnemy* pJunk = pSquad2->GetEnemy(iIndex);

                    pJunk->SetPosition(pEnemy->GetPosition());
                    pJunk->Resurrect();

                    avFlyDir[iIndex] = coreVector2::Direction(I_TO_F(j % iCount) * (2.0f * RCP(I_TO_F(iCount))) * PI);
                    afFlySpeed[iIndex] = 200.0f;

                    SET_BIT(aiType[0], iIndex, iType & 0x01u)
                    SET_BIT(aiType[1], iIndex, iType & 0x02u)
                    SET_BIT(aiType[2], iIndex, false)
                    SET_BIT(aiType[3], iIndex, false)
                    SET_BIT(aiType[4], iIndex, true)
                }
            }

            pEnemy->SetPosition(coreVector3(nPosFunc(i), 0.0f));
            //STAGE_REPEAT(pPath1->GetTotalDistance())
//
            //const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            //const coreVector2 vOffset = coreVector2(0.0f,0.0f);
//
            //pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
//
            //     if(i >= 12u && i < 18u) pEnemy->Rotate90();
            //else if(i >= 18u && i < 21u) pEnemy->Rotate270();
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)    // needed ?  

            if(HAS_BIT(aiType[2], i))
            {
                afFlySpeed[i] = MIN(afFlySpeed[i] + 3.0f * TIME, 1.0f);

                if(afFlySpeed[i] >= 0.0f) // do other calculation in the meantime   
                {
                    const coreVector2 vNewPos = LERPB(avTarget[i], avFlyDir[i], 1.0f - afFlySpeed[i]);

                    pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
                }

                if(afFlySpeed[i] >= 1.0f)
                {
                    pEnemy->Kill(false);
                    ADD_BIT(aiType[3], i)
                }
            }
            else
            {

                //if(afFlySpeed[i])
                {
                    afFlySpeed[i] *= FrictionFactor(5.0f);
                }
                const coreVector2 vDir = avFlyDir  [i];
                const coreFloat fSpeed = afFlySpeed[i];

                coreVector2 vNewPos = pEnemy->GetPosition().xy() + vDir * (fSpeed * TIME);


                     if(vNewPos.x < -FOREGROUND_AREA.x * 1.05f) {vNewPos.x = -FOREGROUND_AREA.x * 1.05f; avFlyDir[i].x =  ABS(avFlyDir[i].x);}   // TODO 1: bounce correction
                else if(vNewPos.x >  FOREGROUND_AREA.x * 1.05f) {vNewPos.x =  FOREGROUND_AREA.x * 1.05f; avFlyDir[i].x = -ABS(avFlyDir[i].x);}
                     if(vNewPos.y < -FOREGROUND_AREA.y * 1.05f) {vNewPos.y = -FOREGROUND_AREA.y * 1.05f; avFlyDir[i].y =  ABS(avFlyDir[i].y);}
                else if(vNewPos.y >  FOREGROUND_AREA.y * 1.05f) {vNewPos.y =  FOREGROUND_AREA.y * 1.05f; avFlyDir[i].y = -ABS(avFlyDir[i].y);}


                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

        });

        STAGE_WAVE("fünf", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // enemies survive invincible
    // - all: delay attack, to get players accustomed
    // FAILURE 
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u, 19u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 20u, 43u)
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
                if(STAGE_TICK_FREE(20.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
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

        if(STAGE_TICK_FREE(2.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
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
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 3u, 5u)
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

            pEnemy->SetPosition(coreVector3(LERP(pEnemy->GetPosition().xy(), vOffset, 2.0f * TIME), 0.0f));
            
            pEnemy->SetPosition(pEnemy->GetPosition() * coreVector3(1.0f,0.0f,0.0f));

        });

        

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})

        // der ausweichende gegner weicht aus sobald er getroffen wird
    });

    // ################################################################
    // gets moved by shooting 
    STAGE_MAIN({TAKE_ALWAYS})
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

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_REF(vForce))
        {
            if(!bFirstHit) return;

            //vForce += (pEnemy->GetPosition().xy() - pBullet->GetPosition().xy()).Normalized(pBullet->GetFlyDir()) * 10.0f;
            vForce -= AlongCrossNormal(pBullet->GetFlyDir()) * 5.0f;
        });

        
            

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vNewPos = pEnemy->GetPosition().xy();

            if(!vForce.IsNull())
            {
                vNewPos  += vForce * TIME;
                vForce *= FrictionFactor(3.0f);
            }

            
                 if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; vForce.x =  ABS(vForce.x);}   // TODO 1: bounce correction
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
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 23u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 24u, 35u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.3f, 0.4f * I_TO_F(i % 6u))

            const coreSpline2* pPath = (i < 12u || i >= 24u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? 1.0f : -1.0f, (i < 6u || i >= 12u) ? ((i < 24u) ? 1.0f : 0.0f) : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, (i < 24u) ? 0.0f : ((i < 30u) ? -0.1f : 0.1f));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);   // TODO 1: vOffset * vFactor (or # no factor multiplication)


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
    STAGE_MAIN({TAKE_ALWAYS})
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
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const cPlayer*    pPlayer = pEnemy->NearestPlayerDual(0u);
            const coreVector2 vDiff   = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();

            if(coreVector2::Dot(vDiff, pEnemy->GetDirection().xy()) <= 0.0f)
            {
                if(pEnemy->GetDirection().x == 0.0f) pEnemy->SetDirection(coreVector3(SIGN(vDiff.x), 0.0f, 0.0f));
                                                else pEnemy->SetDirection(coreVector3(0.0f, SIGN(vDiff.y), 0.0f));
            }

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 30.0f);


            if(STAGE_TICK_TIME(3.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cMineBullet>(5, 0.0f, pEnemy, vPos, vDir);
            }
        });

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
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
    STAGE_MAIN({TAKE_ALWAYS})
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
            pBullet->SetPosition(pBullet->GetPosition() + coreVector3(0.0f, TIME * FOREGROUND_AREA.y * -1.5f, 0.0f));
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // bullet-lines from the side 
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 2u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.2f * I_TO_F((i % 8u) + (((i % 8u) < 4u) ? 0u : 2u)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2((i < 8u) ? -1.0f : 1.0f, (i < 8u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        if(STAGE_TICK_FREE(10.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
        {
            const coreFloat fHeight = 1.1f * (1.0f - 2.0f * (I_TO_F(s_iTick % 41u) / 40.0f));

            const coreVector2 vPos = coreVector2(1.2f, fHeight) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2(1.0f, 0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.0f, NULL,  vPos, -vDir)->ChangeSize(1.2f); 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.0f, NULL, -vPos,  vDir)->ChangeSize(1.2f); 
        }

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // move to side when crossing player (Ikaruga) 
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 2u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const cPlayer*    pPlayer = pEnemy->NearestPlayerDual(0u);
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
    STAGE_MAIN({TAKE_ALWAYS})
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

            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayerDual(0u)->GetPosition().xy(), 20.0f, 2.0f);
            

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
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 2u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 3u, 6u)
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
                afOldOffset[i % 4u] = pEnemy                       ->GetPosition().arr(bRotate);
                afNewOffset[i % 4u] = pEnemy->NearestPlayerDual(0u)->GetPosition().arr(bRotate);
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
    STAGE_MAIN({TAKE_ALWAYS})
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

                //pEnemy->GiveShield(ELEMENT_BLUE);             
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad2,  0u,  4u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad2,  5u,  9u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad2, 10u, 14u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad2, 15u, 19u)
            else if(STAGE_SUB(5u)) pSquad1->GetEnemy(0u)->Kill(true);
        }

        if(m_iStageSub >= 3u)
        {
            fShootAngle += 0.4f*PI * TIME;
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

            const coreVector2 vFlyDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F((i % 5u) + (((i % 10u) < 5u) ? 10u : 4u)) * 30.0f));
            const coreVector2 vFlyPos = vFlyDir * pPath2->CalcPosition(CLAMP(fLifeTime, 0.0f, pPath2->GetTotalDistance())).y;

            pEnemy->SetPosition(coreVector3(vFlyPos * FOREGROUND_AREA, 0.0f));

            if(STAGE_TICK_LIFETIME(1.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual(0u).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // side-attacking enemy lines
    STAGE_MAIN({TAKE_ALWAYS})
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
    // bullet attack explodes on border
    STAGE_MAIN({TAKE_ALWAYS})                         
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(200, COLOR_SHIP_PURPLE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.0f)

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f, ((i < 2u) ? 1.0f : -1.0f));
            const coreVector2 vOffset = coreVector2(i ? ((i < 2u) ? 0.4f : -0.4f) : 0.0f, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i >= 1u) pEnemy->Rotate90();   

            pEnemy->SetDirection(coreVector3(1.0f,0.0f,0.0f));

            if(STAGE_TICK_LIFETIME(1.25f, 0.0f))//(i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();//(StepRotated90(s_iTick % 4u) * FOREGROUND_AREA - vPos).Normalized();////pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(0, 3.0f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(2.0f);
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            if(!g_pForeground->IsVisiblePoint(pBullet->GetPosition().xy()))
            {
                const coreVector2 vPos = pBullet->GetPosition().xy();

                for(coreUintW i = 20u; i--; )
                {
                    //if((i % 4u) < 1u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos,  vDir)->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos, -vDir)->ChangeSize(1.4f);
                    // TODO 1: lower purple balls get overlapped   
                }

                pBullet->Deactivate(false);
            }
        });

        STAGE_WAVE("fünf", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // kill at the same time
    // first 2 of type A
    // then 3 of type B
    // then 5 of type A + B
    // 1: 2 enemies jumping at bottom (0.5f, 0.5f)
    // 2: 2 jumping left, 2 jumping right (0.25f,0.25f | 0.25f, 0.25f)
    // 3: 20 jumping at bottom (X/0.6f, 0.4frest)
    // ##: jump at player, reflect from side
    // TODO 1: !!! everyone awakes at the same time       
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * ((i < 6u) ? 1.3f : 1.0f));
                pEnemy->Configure((i < 6u) ? 50 : 4, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
                
                
                
                if(i >= 6u) pSquad1->GetEnemy(i)->SetPosition(coreVector3((I_TO_F(i-6u) - 4.5f) * 0.2f, 0.0f, 0.0f) * FOREGROUND_AREA3);
            });

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3(-0.5f, 0.0f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3( 0.5f, 0.0f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(2u)->SetPosition(coreVector3( 0.0f,-0.5f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(3u)->SetPosition(coreVector3( 0.0f, 0.7f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(4u)->SetPosition(coreVector3( 0.0f,-0.7f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(5u)->SetPosition(coreVector3( 0.0f, 0.5f,0.0f) * FOREGROUND_AREA3);
            
        });

        STAGE_GET_START(64u)
            STAGE_GET_VEC4_ARRAY(avJumpData, 16u)   // (old side, new side, height, time)
        STAGE_GET_END

        coreBool bAllDead = true;
        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(pEnemy->GetCurHealth() > 0) bAllDead = false;
        });
        if(bAllDead)
        {
            STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
            {
                pEnemy->Kill(true);
            });
        }

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u,  1u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 2u,  5u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 6u, 15u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            
            STAGE_LIFETIME(pEnemy, 0.6f, avJumpData[i].w + (I_TO_F(i % 2u) * 0.2f + ((i < 4u) ? 0.0f : 1.0f)))
            
            if(pEnemy->GetCurHealth() == 0)
            {
                avJumpData[i].w += fLifeSpeed * TIME;
                
                if(avJumpData[i].w >= 5.0f * fLifeSpeed)
                {
                    pEnemy->SetCurHealthPct(0.5f);
                    pEnemy->RefreshColor(0.5f);
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * ((i < 6u) ? 1.3f : 1.0f));
                    avJumpData[i].w = 0.0f;
                }
                else if(avJumpData[i].w >= 4.0f * fLifeSpeed)
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * ((i < 6u) ? 1.3f : 1.0f) * (1.0f + 0.1f * SIN(30.0f*PI * avJumpData[i].w)));
                    return;
                }
                else
                {
                    return;
                }
            }
            
            const coreVector2 vGravity = (i < 2u) ? coreVector2(0.0f,-1.0f) : ((i < 6u) ? coreVector2((i < 4u) ? 1.0f : -1.0f,0.0f) : coreVector2(0.0f,1.0f));
            
                const auto nSumFunc = [](const coreVector2 v) {return v.x + v.y;};
            if(STAGE_BRANCH(1.0f, 2.0f))
            {
                const coreFloat fHeight = LERPB(-1.0f, 1.2f, 1.0f - MAX(fLifeTime, 0.0f));
                pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() * vGravity.Rotated90().Processed(ABS) + (fHeight * FOREGROUND_AREA.y) * vGravity, 0.0f));
                
                
                    avJumpData[i].y = nSumFunc(pEnemy ->GetPosition().xy() * vGravity.Rotated90());
            }
            else
            {
                
                if(STAGE_LIFETIME_POINT(0.5f))
                {
                    const cPlayer* pPlayer = pEnemy->NearestPlayerDual(0u);   // TODO 1 

                    avJumpData[i].x = nSumFunc(pEnemy ->GetPosition().xy() * vGravity.Rotated90());
                    avJumpData[i].y = nSumFunc((pPlayer->GetPosition().xy() * 2.0f - pEnemy->GetPosition().xy()) * vGravity.Rotated90());
                    avJumpData[i].z = MAX(nSumFunc(pPlayer->GetPosition().xy() * vGravity), -0.0f * FOREGROUND_AREA.y);
                    
                    const coreFloat fDiff = avJumpData[i].y - avJumpData[i].x;
                    avJumpData[i].y = MAX(ABS(fDiff), 1.0f * FOREGROUND_AREA.x) * SIGN(fDiff) + avJumpData[i].x;
                }

                if(STAGE_LIFETIME_AFTER(0.5f))
                {
                    


                    const coreFloat fDir = avJumpData[i].y - avJumpData[i].x;
                    
                    const coreVector2 vCurPos = pEnemy->GetPosition().xy() * vGravity.Rotated90();
                    
                    coreBool bBounce = false;

                    // TODO 1: bounce correction ???
                    if(IsHorizontal(vGravity))
                    {
                             if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (fDir < 0.0f)) {bBounce = true; avJumpData[i].x -= (avJumpData[i].x - -FOREGROUND_AREA.y * 1.1f) * 2.0f; avJumpData[i].y -= (avJumpData[i].y - -FOREGROUND_AREA.y * 1.1f) * 2.0f;}
                        else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (fDir > 0.0f)) {bBounce = true; avJumpData[i].x -= (avJumpData[i].x -  FOREGROUND_AREA.y * 1.1f) * 2.0f; avJumpData[i].y -= (avJumpData[i].y -  FOREGROUND_AREA.y * 1.1f) * 2.0f;}
                    }
                    else
                    {
                             if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (fDir < 0.0f)) {bBounce = true; avJumpData[i].x -= (avJumpData[i].x - -FOREGROUND_AREA.x * 1.1f) * 2.0f; avJumpData[i].y -= (avJumpData[i].y - -FOREGROUND_AREA.x * 1.1f) * 2.0f;}
                        else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (fDir > 0.0f)) {bBounce = true; avJumpData[i].x -= (avJumpData[i].x -  FOREGROUND_AREA.x * 1.1f) * 2.0f; avJumpData[i].y -= (avJumpData[i].y -  FOREGROUND_AREA.x * 1.1f) * 2.0f;}
                    }
                    
                    
                    const coreFloat fTime = (fLifeTime - 0.5f) / 1.5f;

                    const coreFloat fSide   = LERP(avJumpData[i].x, avJumpData[i].y, fTime);
                    const coreFloat fHeight = LERP(-1.0f * FOREGROUND_AREA.y, avJumpData[i].z, SIN(fTime * 1.0f*PI));

                    pEnemy->SetPosition(coreVector3(vGravity * fHeight + vGravity.Rotated90() * fSide, 0.0f));
                         
                
                    if(!bBounce && STAGE_TICK_TIME(15.0f, 0.0f) && !pEnemy->GetMove().IsNull())
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->GetMove().Normalized();
                        
                        //const coreVector2 vTan = vDir.Rotated90();
        
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, -vDir)->ChangeSize(1.25f);
                    }
                    
                }
                else
                {
                    const coreFloat fSide   = avJumpData[i].y;
                    const coreFloat fHeight = -1.0f * FOREGROUND_AREA.y;
                    
                    pEnemy->SetPosition(coreVector3(vGravity * fHeight + vGravity.Rotated90() * fSide, 0.0f));
                }
            }

        });

        STAGE_WAVE("EINUNDSECHZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // ################################################################
}


#if 0

void UnusedWaves()
{
    // ################################################################
    // turning and constantly shooting enemy in center 
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 2u)
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

            // sudden turn of enemy after moving to a certain threshold is bad, either use it as wall, or constantly rotate  

            vShootDir = coreVector2::Direction(vShootDir.Angle() + MIN(ABS(fPower) * 5.0f, 1.0f) * SIGN(fSign) * 2.0f * TIME);

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
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 40.0f);

            const auto nShootFunc = [&](const coreVector2 vPos, const coreVector2 vDir)
            {
                const coreVector2 vTan = vDir.Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos + vTan * 6.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos + vTan * 2.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos - vTan * 2.0f, vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.0f, pEnemy, vPos - vTan * 6.0f, vDir);
            };

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

           // TODO 1: shift correction
                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) {nShootFunc(vCurPos, coreVector2( 1.0f, 0.0f)); vCurPos.x =  ABS(vCurPos.x); nShootFunc(vCurPos, coreVector2(-1.0f, 0.0f));}
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) {nShootFunc(vCurPos, coreVector2(-1.0f, 0.0f)); vCurPos.x = -ABS(vCurPos.x); nShootFunc(vCurPos, coreVector2( 1.0f, 0.0f));}
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) {nShootFunc(vCurPos, coreVector2( 0.0f, 1.0f)); vCurPos.y =  ABS(vCurPos.y); nShootFunc(vCurPos, coreVector2( 0.0f,-1.0f));}
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) {nShootFunc(vCurPos, coreVector2( 0.0f,-1.0f)); vCurPos.y = -ABS(vCurPos.y); nShootFunc(vCurPos, coreVector2( 0.0f, 1.0f));}

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
        });

        STAGE_WAVE
    });

    // ################################################################
    // infinity flying enemy shooting from his side (TODO 1: Zero Ranger effect)  
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            //else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 40.0f);

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

           // TODO 1: shift correction
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
    STAGE_MAIN({TAKE_ALWAYS})
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
    STAGE_MAIN({TAKE_ALWAYS})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 4u, 7u)
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
    STAGE_MAIN({TAKE_ALWAYS})
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
    STAGE_MAIN({TAKE_ALWAYS})
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
            STAGE_GET_VEC2(vLastPosition, vLastPosition = g_pGame->GetPlayer(0u)->GetPosition().xy()) // TODO 1: coop 
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy->GetPosition().xy() - vLastPosition).LengthSq() < 1.0f)
            {
                vLastPosition = g_pGame->GetPlayer(0u)->GetPosition().xy(); // TODO 1: coop 

                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 20u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    // TODO 1: lower blue balls get overlapped 
                }
            }

            const coreVector2 vDiff  = vLastPosition - pEnemy->GetPosition().xy();
            const coreFloat   fLenSq = vDiff.LengthSq(); danger
            const coreVector2 vDir   = vDiff.Normalized(); danger
            const coreVector2 vPos   = pEnemy->GetPosition().xy() + vDir * (TIME * 20.0f * SmoothTowards(fLenSq, 5.0f));

            pEnemy->SetPosition (coreVector3(vPos, 0.0f));
            pEnemy->SetDirection(coreVector3(vDir, 0.0f));
        });

        STAGE_WAVE
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
            // TODO 1: lower blue balls get overlapped 
        }
*/

/*
        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [this](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
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
        const auto nExplode = [&](const coreVector2 vDirection)
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

        const auto nEnableFunc = [&](const coreVector2 vPos, coreVector2* OUTPUT vNewPos)
        {
            const coreUintW X = F_TO_UI(CLAMP((vPos.x / FOREGROUND_AREA.x) * 11.5f + 11.5f, 0.0f, 23.0f));
            const coreUintW Y = F_TO_UI(CLAMP((vPos.y / FOREGROUND_AREA.y) * 11.5f + 11.5f, 0.0f, 23.0f));
            const coreUintW I = X + Y * 24u;
            const coreUintW B = I / 32u;
            const coreUintW S = I % 32;

            ASSERT(B < iBitfieldNum)

            if(HAS_BIT(aiEnabled[B], S))
                return false;

            vNewPos->x = ((I_TO_F(X) - 11.5f) / 11.5f) * FOREGROUND_AREA.x;
            vNewPos->y = ((I_TO_F(Y) - 11.5f) / 11.5f) * FOREGROUND_AREA.x;

            ADD_BIT(aiEnabled[B], S)
            return true;
        };
*/
/*

        // cluster ball attack
        if(STAGE_TICK_LIFETIME(5.0f, 0.0f))
        {
            const coreVector2 vPos = pEnemy->GetPosition().xy();
            const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();       

            for(coreUintW j = 8u; j--; )
            {
                const coreVector2 vShift = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 45.0f)); // TODO 1: StepRotated45 (or X)

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, pEnemy, vPos + vShift * 5.0f, vDir)->ChangeSize(1.4f);
            }
        }


        // bullet wall
        coreVector2 vSafePos  = coreVector2::Direction(m_fStageTime) * 0.5f;
        coreVector2 vSafeSize = coreVector2(0.4f,0.4f);

        if(nStageMiniTickFunc(8.0f, 0.0f))
        {
            for(coreUintW i = 0u; i < 24u; ++i)
            {
                const coreVector2 vPos = coreVector2((I_TO_F(i) - 11.5f) * 0.1f + FMOD(vSafePos.x, 0.1f), 1.1f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                if(!coreMath::IsNear(vPos.x, vSafePos.x * FOREGROUND_AREA.x, vSafeSize.x * FOREGROUND_AREA.x))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, pDummy, vPos, vDir)->ChangeSize(1.3f);
                }
            }

            if(false) for(coreUintW i = 0u; i < 24u; ++i)
            {
                const coreVector2 vPos = coreVector2(1.1f, (I_TO_F(i) - 11.5f) * 0.1f + FMOD(vSafePos.y, 0.1f)) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(-1.0f,0.0f);

                if(!coreMath::IsNear(vPos.y, vSafePos.y * FOREGROUND_AREA.y, vSafeSize.y * FOREGROUND_AREA.y))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pDummy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        }


        // side waves
        if(nStageMiniTickFunc(2.0f, 0.0f))
        {
            for(coreUintW i = 0u; i < 20u; ++i)
            {
                if((s_iTick % 2u) == (i / 10u)) continue;

                const coreFloat fSide = (i < 10u) ? 1.0f : -1.0f;

                const coreVector2 vPos = coreVector2(fSide * -1.0f, (I_TO_F(i % 10u) - 4.5f) * ((fStageMiniTime < 5.0f) ? 0.05f : 0.1f) + FmodRange(fStageMiniTime * 0.7f, -1.0f, 1.1f)) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(fSide, 0.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pDummy, vPos, vDir)->ChangeSize(1.3f);
            }
        }


        // bending bullets from top
        if(nStageMiniTickFunc(4.0f, 0.0f))
        {
            for(coreUintW i = 0u; i < 10u; ++i)
            {
                const coreVector2 vPos = coreVector2((I_TO_F(i) - 4.5f) * 0.04f + FmodRange(fStageMiniTime * 2.5f, -1.0f, 1.0f), 1.1f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                const coreFloat fSpeed = 1.0f + I_TO_F((s_iTick % 2u) ? i : (4u - i)) * 0.03f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, pDummy, vPos, vDir)->ChangeSize(1.3f);
            }
        }
*/


/*
    // ################################################################
    // enemies survive invincible
    // soll man schaffen ohne drehen und rollen
    // gegner-angriffe sollen noch nicht gezielt sein (auch wenn es im intro schon war)
    // bewegungen wie bei alter erster gruppe für mehr dynamic, 3 reihen links rechts links, frei aufgeteilt oder kleine gruppe
    // gegner sind in gruppen unverwundbar und sterben gemeinsam, nicht sub-stages
    // 
    // 2 3er
    // 4 2er, abstand übereinander
    // street middle
    // 3er links rechts ???
    // 1er links rechts
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.0f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 38u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.35f);
                pEnemy->Configure(4, COLOR_SHIP_BLUE);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        constexpr coreUintW aaiPair[][2] = {{ 0u,  1u}, { 2u,  3u}, { 4u,  5u},
                                            { 6u,  8u}, { 7u,  9u}, {10u, 12u}, {11u, 13u},
                                            {14u, 15u}, {16u, 17u}, {18u, 19u}, {20u, 21u},
                                            {22u, 24u}, {23u, 28u}, {25u, 27u}, {26u, 33u},
                                            {29u, 30u}, {31u, 36u}, {32u, 34u}, {35u, 37u}};

        for(coreUintW i = 0u; i < ARRAY_SIZE(aaiPair); ++i)
        {
            cEnemy* A = pSquad1->GetEnemy(aaiPair[i][0]);
            cEnemy* B = pSquad1->GetEnemy(aaiPair[i][1]);

            if((A->GetCurHealth() == 0) && (B->GetCurHealth() == 0))
            {
                A->Kill(true);
                B->Kill(true);
            }
        }

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  6u, 13u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 14u, 21u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 22u, 37u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.0f)

            const coreFloat x1 = (i < 22u) ? (I_TO_F(i % 2u) * 0.25f - 0.125f) : (I_TO_F((i - 22u) % 4u) * 0.25f - 0.375f);
            const coreFloat x2 = (i < 6u) ? (I_TO_F(i / 2u) * 0.5f - 0.5f) : ((i < 14u) ? (I_TO_F((i - 6u) / 4u) * 1.0f - 0.5f) : 0.0f);
            const coreFloat y  = (i < 14u) ? (((i / 2u) % 2u) ? 0.8f : 0.0f) : ((i < 22u) ? (I_TO_F((i - 14u) / 2u) * 0.25f) : (I_TO_F((i - 22u) / 4u) * 0.25f));

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(x1 + x2, y);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            pEnemy->SetDirection   (coreVector3(0.0f,-1.0f,0.0f));

            if((i < 24u) || STAGE_LIFETIME_BEFORE(1.2f))
            {
                if(STAGE_TICK_LIFETIME(0.7f, (i % 2u) ? 0.5f : 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = coreVector2(0.0f,-1.0f);
                    const coreVector2 vTan = vDir.Rotated90() * 1.5f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.25f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.2f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.25f);
                }
            }
        });

        STAGE_WAVE("EINS", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // top, bottom, side groups creating bullet carpet
    // - all: bullet patterns provide a lot of safe space, so players have time to think
    // - 2: push players consistently away into safe space, to reduce confusion when trying to fly into or under enemies
    STAGE_MAIN({TAKE_ALWAYS, u})
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
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 23u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 24u, 35u)
        }

        if((m_iStageSub == 2u) && (pSquad1->GetNumEnemiesAlive() == 6u))
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->GetPosition().y < 0.0f)
                {
                    const coreVector2 vForce = pPlayer->GetForce();
                    if(!vForce.IsNull()) pPlayer->SetForce(coreVector2(0.0f,1.0f) * vForce.Length());
                }
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.4f * I_TO_F(i % 6u))

            const coreSpline2* pPath = (i < 12u || i >= 24u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? 1.0f : -1.0f, (i >= 6u && i < 12u) ? -1.0f : ((i < 24u) ? 1.0f : 0.0f));
            const coreVector2 vOffset = coreVector2(0.0f, (i >= 6u && i < 12u) ? -0.1f : ((i < 24u) ? 0.0f : ((i < 30u) ? 0.8f : 0.9f)));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);   // TODO 1: vOffset * vFactor (or # no factor multiplication)

            if((i < 24u) || STAGE_LIFETIME_BEFORE(1.2f))
            {
                if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.25f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.25f);
                }
            }
        });

        STAGE_WAVE("EINS", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // big enemy splits up into smaller enemies on death
    // - all: cinders have low health, to make killing them with accidental hits easier
    // - all: cinders start at corners, to reduce accidental hits when enemies fly in
    // - all: arrows spawn with offset, to reduce accidental killing them before getting visible
    // TODO 1: too similar to meteor wave
    // TODO 1: spawn small enemies from all sides
    // TODO 1: create a lot more containers (3 each side ?)
    // TODO 1: after 1-2 waves, enemies spawn only from other enemies
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.3f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->AddStop(coreVector2( 0.0f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_RED);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 28u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_VEC2(vLastPos)
        STAGE_GET_END

        const auto nSpawnFunc = [&](const coreVector2 vPos, const coreUintW iIndex)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy2, i)
            {
                if((i >= iIndex * 4u) && (i < (iIndex + 1u) * 4u))
                {
                    const coreVector2 vDir = StepRotated90X(i % 4u);

                    pEnemy2->SetPosition (coreVector3(vPos + vDir * 4.0f, 0.0f));
                    pEnemy2->SetDirection(coreVector3(vDir,               0.0f));

                    pEnemy2->Resurrect();
                }
            });
        };

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy1, i)
        {
            if(pEnemy1->ReachedDeath()) nSpawnFunc(pEnemy1->GetPosition().xy(), i);
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 4u)
            else if(STAGE_SUB(3u)) nSpawnFunc(vLastPos, 5u);
            else if(STAGE_SUB(4u)) nSpawnFunc(vLastPos, 6u);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.9f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f) * ((i % 2u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i >= 3u) pEnemy->Rotate90();
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy(), LERPB(50.0f, 20.0f, MIN(pEnemy->GetLifeTime(), 1.0f)), 2.0f);

            if(STAGE_LIFETIME_AFTER(0.4f) && STAGE_TICK_LIFETIME(6.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.3f, pEnemy, vPos, vDir)->ChangeSize(1.1f);
            }

            vLastPos = pEnemy->GetPosition().xy();
        });

        STAGE_COLL_PLAYER_ENEMY(pPlayer, pEnemy, vIntersection, bFirstHit, COLL_THIS)
        {
            if(!bFirstHit || (pEnemy->GetID() != cCinderEnemy::ID)) return;

            STAGE_BADGE(0u, BADGE_EASY, pPlayer->GetPosition())
            pEnemy->TakeDamage(100, ELEMENT_NEUTRAL, vIntersection.xy(), pPlayer);
        });

        STAGE_WAVE("VIER", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // grow and shrink bullets
    // shrink
    // grow
    // lots of overlap popping away
    // X stretch changes to Y stretch
    // break up into multiple bullets
    // 
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        //STAGE_GET_START(u)
        //STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 1u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.2f * I_TO_F(i))

            //STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();       

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 1.0f, pEnemy, vPos, vDir)->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.0f);
            }
        });



        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pPlayerBullet, cBullet* OUTPUT pEnemyBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            if(bFirstHit)
            {
                pEnemyBullet->SetSize   (coreVector3(1.0f,1.0f,1.0f) * (pEnemyBullet->GetSize   ().x + 1.5f * I_TO_F(pPlayerBullet->GetDamage())));
                pEnemyBullet->SetTexSize(coreVector2(1.0f,1.0f)      * (pEnemyBullet->GetTexSize().x + 0.01f)); // 0.12f
                
                if(pEnemyBullet->GetSize().x >= 15.0f)
                    pEnemyBullet->Deactivate(true);
                
                pPlayerBullet->Deactivate(true);
            }
        });
#if 0
        g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pPlayerBullet)
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pEnemyBullet)
            {
                pEnemyBullet->SetSize(coreVector3(1.0f,1.0f,1.0f) * (pEnemyBullet->GetSize().x + 0.02f));
                
            });
        });
#endif
#if 0
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            const coreFloat fNewSize = MIN(pBullet->GetSize().x + (7.0f * TIME), 15.0f);

            pBullet->SetSize(coreVector3(1.0f,1.0f,1.0f) * fNewSize);
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([](cFlipBullet* OUTPUT pBullet)
        {
            const coreFloat fNewSize = MAX(pBullet->GetSize().x - (7.0f * TIME), 5.0f);

            pBullet->SetSize(coreVector3(1.0f,0.35f,1.0f) * fNewSize);
        });

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pPlayerBullet, cBullet* OUTPUT pEnemyBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            if(bFirstHit)
            {
                if(pEnemyBullet->GetID() == cOrbBullet::ID)
                {
                    pEnemyBullet->SetSize(coreVector3(1.0f,1.0f,1.0f) * (pEnemyBullet->GetSize().x * POW(0.9f, I_TO_F(pPlayerBullet->GetDamage()))));
                }
                else
                {
                    const coreFloat fFactor = (20.0f - pEnemyBullet->GetSize().x) * 0.1f;
                    pEnemyBullet->SetSize(coreVector3(1.0f,0.35f,1.0f) * (pEnemyBullet->GetSize().x + fFactor));
                }

                pPlayerBullet->Deactivate(true);
            }
        });
#endif
        STAGE_WAVE("SECHSUNDSECHZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // automatic forward movement
    // rückwärts fliegen (dem spieler zeit geben) (beschleunigung interpolieren ??, aber sehr kurz ??)
    // stillstand (in mitte) (rotation + dodge muss genutzt werden), gegner kommen auf ihn zu, geschosse kommen von diagonale (visibility)
    //   - bewegung geht in die mitte (erst Y, dann X), links-rechts in coop, weil gut sichtbar welche gegner welchen spieler dann angreifen
    // erste welle startet nach 2 turns
    // (automatic right-turn on wall-collision ?)
    // schiff hat kleine explosionen und beginnt zu brennen -> orangener helfer kommt am ende raus
    // man kann nicht strafen
    
    // zwei gegner weit auseinander von einer seite fliegen gegenüber (einer wird durch direktes anfliegen getötet, der andere durch 90grad abwarten)
    // durchgezogene linien
    // beim rückwärtsfliegen kann man wand zum stabilisieren ausnutzen -> gegner die am rand (mittig) spawnen und dort bleiben und angreifen (aber bei coop geht dann ecke ecke, vielleicht ein stückchen weg vom rand)
    
    // zwei von oben
    // zwei von rechts
    // zwei von links
    // fünf von rechts
    // fünf von links
    // von allen 8 seiten einzeln (oder im doppel, einzeln hätt aber mehr kontrast ?), muster, start oben rechts, endet links
    // <flip> + wait (r-type remainer)
    // 
    // geschosse von beiden seiten der gegner, gehen mit gegner mit um "wand" zu bauen
    // pacifist ? -> attack on old position when spawning ?   ODER MITTE
    // shared damage ?
    // (maybe movement in pulses)
    // TODO 1: general solution to properly override direction
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 32u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_BLUE);
                
                if(i < 16u)
                {
                    pEnemy->SetPosition(coreVector3((I_TO_F(i % 2u) - 1.5f) * 0.25f - 0.5f, 1.2f, 0.0f) * FOREGROUND_AREA3);
    
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                    
                         if(i <  4u/2u) {}
                    else if(i <  8u/2u) pEnemy->Rotate180();
                    else if(i < 12u/2u) pEnemy->Rotate90 ();
                    else if(i < 16u/2u) pEnemy->Rotate270();
                }
                else
                {
                    const coreVector2 vDir = UnpackDirection((((i - 16u) / 2u) * 3u) % 8u);
                    const coreVector2 vTan = vDir.Rotated90();
                    
                    pEnemy->SetPosition(coreVector3(((vDir * 2.0f).Processed(CLAMP, -1.0f, 1.0f) * 1.2f + vTan * ((i % 2u) ? -0.1f : 0.1f)) * FOREGROUND_AREA, 0.0f));
                    
                    pEnemy->SetDirection(coreVector3(-vDir, 0.0f));
                }
            });
        });

        //STAGE_GET_START(48u)
        //    STAGE_GET_VEC2_ARRAY(avTarget, 24u)
        //STAGE_GET_END
#if 1
        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_VAL(pSquad1))
        {
            if(!bFirstHit) return;

            const coreUintW i = pSquad1->GetIndex(pEnemy);

            coreUint8 iGroupNum;
            if(i < 16u) iGroupNum = 2u;
                   else iGroupNum = 2u;

            for(coreUintW j = coreMath::FloorAlign(i, iGroupNum), je = coreMath::CeilAlign(i + 1u, iGroupNum); j < je; ++j)
            {
                if(i != j) pSquad1->GetEnemy(j)->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()));
            }
        });
#endif
        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  2u,  3u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 16u, 17u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 18u, 19u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 20u, 21u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 22u, 23u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 24u, 25u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 26u, 27u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 28u, 29u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 30u, 31u)

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                if(m_iStageSub == 1u)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);

                    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(pPlayer->GetPosition(), COLOR_FIRE_ORANGE);
                    // TODO 1: smoke explosion and smoke should come out 1 second before
                }
                else if(m_iStageSub == 14u)
                {
                    pPlayer->SetDirection(-pPlayer->GetDirection());

                    g_pSpecialEffects->MacroExplosionPhysicalColorSmall(pPlayer->GetPosition(), COLOR_FIRE_ORANGE);
                }
            });
        }

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            const coreFloat   fSign      = (m_iStageSub >= 14u) ? -1.0f : 1.0f;
            const coreVector2 vEffectPos = pPlayer->GetPosition().xy() - pPlayer->GetDirection().xy() * (2.0f * fSign);

            //coreVector2 vNewPos = pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * (50.0f * TIME * 0.7f * fSign);
            //coreVector2 vNewPos = pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * (pPlayer->CalcMoveSpeed() * TIME * 0.7f * fSign);
            coreVector2 vNewPos = pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * (pPlayer->CalcMoveSpeed() * TIME * fSign);

            const coreVector4 vArea = pPlayer->GetArea();

            vNewPos.x = CLAMP(vNewPos.x, vArea.x, vArea.z);
            vNewPos.y = CLAMP(vNewPos.y, vArea.y, vArea.w);

            pPlayer->SetPosition(coreVector3(vNewPos, 0.0f));

            g_pSpecialEffects->CreateSplashFire (coreVector3(vEffectPos, 0.0f),  5.0f, 2u, COLOR_FIRE_ORANGE);
            g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), 10.0f, 1u, COLOR_FIRE_ORANGE);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 25.0f);
            

            coreVector2       vCurPos = pEnemy->GetPosition ().xy();
            const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.3f) && (vCurDir.x < 0.0f)) vCurPos.x =  ABS(vCurPos.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.3f) && (vCurDir.x > 0.0f)) vCurPos.x = -ABS(vCurPos.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.3f) && (vCurDir.y < 0.0f)) vCurPos.y =  ABS(vCurPos.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.3f) && (vCurDir.y > 0.0f)) vCurPos.y = -ABS(vCurPos.y);

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            
            //if(STAGE_LIFETIME_POINT(0.0f)) 
                //avTarget[i] = pEnemy->NearestPlayerSide()->GetPosition().xy();

            if(STAGE_TICK_LIFETIME(20.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = -pEnemy->GetDirection().xy();  
                //const coreVector2 vDir = (avTarget[i] - vPos).Normalized();
                //const coreVector2 vDir = StepRotated90(i % 4u);
                const coreVector2 vTan = vDir.Rotated90() * ((i % 2u) ? -1.0f : 1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, pEnemy, vPos, vTan)->ChangeSize(1.4f);   // fast enough to not run into it from behind
            }

            if(pEnemy->ReachedDeath())
            {
                g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cWaveBullet>([&](cWaveBullet* OUTPUT pBullet)
                {
                    if(pBullet->GetOwner() == pEnemy) pBullet->Deactivate(true);
                });
            }
        });

        STAGE_WAVE("SECHSUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // ((belongs to automatic forward movement))
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
        });

        STAGE_FINISH_NOW
    });
    
    // direct homing-worm attack
    #if 0
            if(false && (iSnakeNum < 8u) && STAGE_TICK_FREE(9.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
            {
                const coreVector2 vPos = coreVector2(0.0f,0.0f);
                const coreVector2 vDir = coreVector2(1.0f,0.0f);

                cBullet* pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f*0.0f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(2.2f);
                pBullet->AddStatus(BULLET_STATUS_IMMORTAL);

                aiSnakePtr[iSnakeNum++] = P_TO_UI(pBullet);
                // beware of pointer reallocation
            }

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cWaveBullet>([&](cWaveBullet* OUTPUT pBullet)
            {
                const coreUintW i = std::find(aiSnakePtr, aiSnakePtr + iSnakeNum, P_TO_UI(pBullet)) - aiSnakePtr;
                if(i >= iSnakeNum) return;

                const coreSpline2* pPath = (i < 4u) ? pPath2 : pPath3;
                const coreFloat    fTime = FMOD(pBullet->GetFlyTime() * 0.9f, pPath->GetTotalDistance());

                coreVector2 vPosition;
                coreVector2 vDirection;
                pPath->CalcPosDir(CLAMP(fTime, 0.0f, pPath->GetTotalDistance()), &vPosition, &vDirection);

                pBullet->SetPosition(coreVector3(vPosition * FOREGROUND_AREA, 0.0f));
                pBullet->SetFlyDir  (vDirection);
            });

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cWaveBullet>([&](cWaveBullet* OUTPUT pBullet)
            {
                const coreUintW i = std::find(aiSnakePtr, aiSnakePtr + iSnakeNum, P_TO_UI(pBullet)) - aiSnakePtr;
                if(i >= iSnakeNum) return;
                
                const coreFloat fSpeedTurn = i ? 15.0f : 3.0f;
                const coreFloat fSpeedMove = 40.0f;
                
                const coreObject3D* pTarget = i ? s_cast<const coreObject3D*>(I_TO_P(aiSnakePtr[i - 1u])) : g_pGame->FindPlayerDual(i % 2u);

                const coreVector2 vDiff = pTarget->GetPosition().xy() - pBullet->GetPosition().xy();
                if(vDiff.IsNull()) return;
    
                // 
                const coreVector2 vAim = vDiff.Normalized();
                const coreVector2 vDir = SmoothAim(pBullet->GetDirection().xy(), vAim, fSpeedTurn);
                const coreVector2 vPos = pBullet->GetPosition().xy() + vDir * (fSpeedMove * TIME);
            
                // 
                pBullet->SetPosition (coreVector3(vPos, 0.0f));
                pBullet->SetDirection(coreVector3(vDir, 0.0f));
                pBullet->SetFlyDir   (vDir);
            });
    #endif
    
    // Gradius V intro enemy movement
    STAGE_ADD_PATH(pPath3)
    {
        pPath3->Reserve(4u);
        pPath3->AddNode (coreVector2(-0.65f, 1.3f), coreVector2(0.0f,-1.0f));
        pPath3->AddNodes(coreVector2(-0.65f,-0.5f), coreVector2(0.0f,-1.0f), coreVector2(1.0f,2.0f).Normalized() * 1.6f);
        pPath3->AddNode (coreVector2( 1.7f, -1.3f), coreVector2(1.0f,-1.0f).Normalized());
        pPath3->Refine();
    });
*/