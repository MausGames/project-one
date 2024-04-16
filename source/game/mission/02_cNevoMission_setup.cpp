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
// setup the Nevo mission
void cNevoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // bomberman hero
    // + and x
    // moving bombs
    // multiple bombs with different timer
    // super-huge bomb
    // bombs dont affect each other
    // blast only has instant damage (not lingering), not outline, and (blast thickness <= bomb thickness) (never more)
    // rotation (inklusive explosions-indikator)
    // anzeigen explosions-indikator wie bei leviathan laser
    //
    // bombs straight, don't move (during warning) or only slight, from above (2 tick)
    // bombs from above 45deg, constant speed, bounce 0.7f x (2 tick)
    // bombs rotating from sides (no rotation on explosion, because instant damage) (4 tick)
    
    // bombs +
    // bombs x
    // bombs +,x moving linear
    // bombs rotating
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f, 1.2f), coreVector2(1.0f,-1.0f).Normalized());
            pPath1->AddNode(coreVector2( 1.2f,-1.2f), coreVector2(1.0f,-1.0f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 24u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(4u + NEVO_BOMBS * 4u)
            STAGE_GET_VEC3       (vHelperData, vHelperData = coreVector3(1.0f,0.0f,0.0f))
            STAGE_GET_UINT       (iBombIndex)
            STAGE_GET_UINT_ARRAY (aiType,  NEVO_BOMBS)
            STAGE_GET_FLOAT_ARRAY(afDelay, NEVO_BOMBS)
            STAGE_GET_VEC2_ARRAY (avMove,  NEVO_BOMBS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 17u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 18u, 23u)
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect();
            pHelper->SetPosition(coreVector3(-1.2f,0.5f,0.0f) * FOREGROUND_AREA3);
        }

        if(STAGE_CLEARED)
        {
            for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
                this->DisableBomb(i, true);

            for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
                this->DisableBlast(i, true);
        }

        const auto nTargetFunc = [](const coreBool bType, const coreVector2& vTarget, coreVector2* OUTPUT pvPos, coreVector2* OUTPUT pvForce)
        {
            ASSERT(pvPos && pvForce)

            const coreVector2 vAxis = bType ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
            const coreVector2 vSide = vTarget.Processed(SIGN) * FOREGROUND_AREA;

            (*pvPos)   = -1.2f * vSide * vAxis + vTarget * vAxis.yx();
            (*pvForce) = (vTarget + vSide) * vAxis;
        };

        coreVector2 vHelperPos = pHelper->GetPosition().xy();
        vHelperPos += vHelperData.xy() * (30.0f * Core::System->GetTime());

        if(((vHelperPos.x < -FOREGROUND_AREA.x * 1.2f) && (vHelperData.x < 0.0f)) ||
           ((vHelperPos.x >  FOREGROUND_AREA.x * 1.2f) && (vHelperData.x > 0.0f)) ||
           ((vHelperPos.y < -FOREGROUND_AREA.y * 1.2f) && (vHelperData.y < 0.0f)) ||
           ((vHelperPos.y >  FOREGROUND_AREA.y * 1.2f) && (vHelperData.y > 0.0f)))
        {
            const cPlayer* pPlayer = g_pGame->FindPlayerDual(0u); // TODO   

            coreVector2 vPos, pvForce;
            nTargetFunc(IsHorizontal(vHelperData.xy()), pPlayer->GetPosition().xy(), &vPos, &pvForce);

            vHelperPos = vPos;
            vHelperData.xy(pvForce.Normalized());
        }

        pHelper->SetPosition(coreVector3(vHelperPos, 0.0f));

        for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
        {
            cLodObject& oBomb = m_aBombRaw[i];

            if(oBomb.IsEnabled(CORE_OBJECT_ENABLE_ALL))
            {
                const coreFloat fPrevDelay = afDelay[i];
                afDelay[i] += 1.0f * Core::System->GetTime();

                if((fPrevDelay < 2.0f) && (afDelay[i] >= 2.0f))
                    this->EnableBlast(i);

                if(aiType[i] == 1u) 
                {
                    // stand still
                }
                else if(aiType[i] == 2u)
                {
                    const coreVector2 vNewPos = oBomb.GetPosition().xy() + avMove[i] * Core::System->GetTime();

                    oBomb.SetPosition(coreVector3(vNewPos, 0.0f));

                         if((vNewPos.x < -FOREGROUND_AREA.x * 1.1f) && (avMove[i].x < 0.0f)) avMove[i].x =  ABS(avMove[i].x);
                    else if((vNewPos.x >  FOREGROUND_AREA.x * 1.1f) && (avMove[i].x > 0.0f)) avMove[i].x = -ABS(avMove[i].x);
                         if((vNewPos.y < -FOREGROUND_AREA.y * 1.1f) && (avMove[i].y < 0.0f)) avMove[i].y =  ABS(avMove[i].y);
                    else if((vNewPos.y >  FOREGROUND_AREA.y * 1.1f) && (avMove[i].y > 0.0f)) avMove[i].y = -ABS(avMove[i].y);
                }
                else if(aiType[i] == 3u)
                {
                    oBomb.SetPosition (coreVector3(oBomb.GetPosition().xy() + avMove[i] * Core::System->GetTime(), 0.0f));
                    oBomb.SetDirection(coreVector3(coreVector2::Direction(afDelay[i] * ((i % 2u) ? 1.0f : -1.0f)), 0.0f));

                    avMove[i] *= FrictionFactor(1.5f);
                }
            }

            if(this->GetBombGone(i))
            {
                const coreVector2 vPos = oBomb.GetPosition ().xy();
                const coreVector2 vDir = oBomb.GetDirection().xy();

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = MapToAxis(pPlayer->GetPosition().xy() - vPos, vDir.InvertedX());

                    if((ABS(vDiff.x) < 3.0f) || (ABS(vDiff.y) < 3.0f))
                    {
                        if(pPlayer->IsNormal()) pPlayer->TakeDamage(10, ELEMENT_ORANGE, pPlayer->GetPosition().xy());
                    }
                });

                afDelay[i] = 0.0f;
            }
        }

        if(m_iStageSub == 1u)
        {
            if(g_pForeground->IsVisiblePoint(pHelper->GetPosition().xy()) &&
               (IsHorizontal(vHelperData.xy()) ? STAGE_POSITION_POINT(pHelper, 0.0f, x) : STAGE_POSITION_POINT(pHelper, 0.0f, y)))
            {
                const coreUintW iIndex  = (iBombIndex++) % NEVO_BOMBS;
                
                this->EnableBomb(iIndex, true);
                
                m_aBombRaw[iIndex].SetPosition (coreVector3(s_vPositionPoint, 0.0f));
                m_aBombRaw[iIndex].SetDirection(coreVector3(0.0f,1.0f,0.0f));
                
                aiType[iIndex] = 1u;
            }
        }
        else if(m_iStageSub == 2u)
        {
            if(g_pForeground->IsVisiblePoint(pHelper->GetPosition().xy()) &&
                    ((IsHorizontal(vHelperData.xy()) ? STAGE_POSITION_POINT(pHelper,  0.5f * FOREGROUND_AREA.x, x) : STAGE_POSITION_POINT(pHelper,  0.5f * FOREGROUND_AREA.y, y)) ||
                     (IsHorizontal(vHelperData.xy()) ? STAGE_POSITION_POINT(pHelper, -0.5f * FOREGROUND_AREA.x, x) : STAGE_POSITION_POINT(pHelper, -0.5f * FOREGROUND_AREA.y, y))))
            {
                const coreUintW iIndex  = (iBombIndex++) % NEVO_BOMBS;
                
                this->EnableBomb(iIndex, true);
                
                m_aBombRaw[iIndex].SetPosition (coreVector3(s_vPositionPoint, 0.0f));
                m_aBombRaw[iIndex].SetDirection(coreVector3(coreVector2(1.0f,1.0f).Normalized(), 0.0f));
                
                aiType[iIndex] = 1u;
            }
        }
        else if(m_iStageSub == 3u)
        {
            if(g_pForeground->IsVisiblePoint(pHelper->GetPosition().xy()) &&
                    ((IsHorizontal(vHelperData.xy()) ? STAGE_POSITION_POINT(pHelper,  0.5f * FOREGROUND_AREA.x, x) : STAGE_POSITION_POINT(pHelper,  0.5f * FOREGROUND_AREA.y, y)) ||
                     (IsHorizontal(vHelperData.xy()) ? STAGE_POSITION_POINT(pHelper, -0.5f * FOREGROUND_AREA.x, x) : STAGE_POSITION_POINT(pHelper, -0.5f * FOREGROUND_AREA.y, y))))
            {
                const coreUintW iIndex  = (iBombIndex++) % NEVO_BOMBS;
                
                this->EnableBomb(iIndex, true);
                
                m_aBombRaw[iIndex].SetPosition (coreVector3(s_vPositionPoint, 0.0f));
                m_aBombRaw[iIndex].SetDirection((iIndex % 2u) ? coreVector3(coreVector2(1.0f,1.0f).Normalized(), 0.0f) : coreVector3(0.0f,1.0f,0.0f));

                avMove[iIndex] = 30.0f * vHelperData.xy().Rotated90() * ((coreVector2::Dot(vHelperData.xy().Rotated90(), s_vPositionPoint) < 0.0f) ? -1.0f : 1.0f);
                aiType[iIndex] = 2u;
            }
        }
        else if(m_iStageSub == 4u)
        {
            if(!CONTAINS_FLAG(pHelper->GetStatus(), HELPER_STATUS_DEAD))
            {
                if(!g_pForeground->IsVisibleObject(pHelper))
                    pHelper->Kill(false);
            }
            else if(STAGE_TICK_FREE(0.75f, 0.0f))
            {
                const cPlayer*  pPlayer = g_pGame->FindPlayerDual(((s_iTick % 4u) < 2u) ? 0u : 1u);
                const coreUintW iIndex  = (iBombIndex++) % NEVO_BOMBS;

                this->EnableBomb(iIndex, false);

                coreVector2 vPos, pvForce;
                nTargetFunc(s_iTick % 2u, pPlayer->GetPosition().xy(), &vPos, &pvForce);

                m_aBombRaw[iIndex].SetPosition(coreVector3(vPos, 0.0f));

                avMove[iIndex] = 1.5f * pvForce;
                aiType[iIndex] = 3u;
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, pPath1->GetTotalDistance() * 0.5f * I_TO_F((i % 6u) / 3u) + 0.2f * I_TO_F(i % 3u))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 6u) < 3u) ? 1.0f : -1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("SECHSZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all bombs
    STAGE_MAIN
    {
        for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
            this->DisableBomb(i, false);

        for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
            this->DisableBlast(i, false);

        g_pGame->GetHelper(ELEMENT_YELLOW)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Nautilus, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Amemasu, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Leviathan, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}