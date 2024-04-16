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
// setup the Gelu mission
void cGeluMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cVolcanoBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
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
    // kill at the same time
    // first 2 of type A
    // then 3 of type B
    // then 5 of type A + B
    // 1: 2 enemies jumping at bottom (0.5f, 0.5f)
    // 2: 2 jumping left, 2 jumping right (0.25f,0.25f | 0.25f, 0.25f)
    // 3: 20 jumping at bottom (X/0.6f, 0.4frest)
    // ##: jump at player, reflect from side
    // TODO: !!! everyone awakes at the same time       
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
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u,  1u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 2u,  5u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 6u, 15u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            
            STAGE_LIFETIME(pEnemy, 0.6f, avJumpData[i].w + (I_TO_F(i % 2u) * 0.2f + ((i < 4u) ? 0.0f : 1.0f)))
            
            if(pEnemy->GetCurHealth() == 0)
            {
                avJumpData[i].w += fLifeSpeed * Core::System->GetTime();
                
                if(avJumpData[i].w >= 5.0f * fLifeSpeed)
                {
                    pEnemy->SetCurHealthPct(0.5f);
                    pEnemy->RefreshColor();
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
            
                const auto nSumFunc = [](const coreVector2& v) {return v.x + v.y;};
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
                    const cPlayer* pPlayer = pEnemy->NearestPlayerDual(0u);   // TODO    

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
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_WAVE("ZWEIUNDSECHZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_WAVE("DREIUNDSECHZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 5u})
    //{
    //    STAGE_BOSS(m_Tartarus, {60.0f, 120.0f, 180.0, 240.0f})
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
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_WAVE("SECHSUNDSECHZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 7u})
    {
        STAGE_WAVE("SIEBENUNDSECHZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        STAGE_WAVE("ACHTUNDSECHZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 11u})
    //{
    //    STAGE_BOSS(m_Phalaris, {60.0f, 120.0f, 180.0, 240.0f})
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
        STAGE_WAVE("EINUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 13u})
    {
        STAGE_WAVE("ZWEIUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });
STAGE_START_HERE
    // ################################################################
    // snap to grid
    // ((line moves down from the top and "catches" player
    // ((helper in the line, on the right hand side (75% or so)
    // ((fast interpolated movement, shooting does not influence speed, make quick movement inbetween possible (new start)
    // ((4x4
    // 
    // enemies move snakelike along rails (into player ?) (down like bubble bobble water)
    // evasion minigame (bullets or enemies move straight down) (dance dance revolution effect, but without arrows) // beware of coop, never all on left or right (like aabb, but abab should be fine)
    // some lines break down later
    // moving orbs
    // 
    // orbs/webs get less with time, until only middle quad stays and moves around with player
    // bullets with holes between orbs, player has to move in the right moment (left-right line)
    // last 4 orbs from center stretch to border (vielleicht nur 1 achse ?, dann angriff auf kurze achsen um spieler in die mitte zu zwingen) (oder rotations angriff von mitte) (oder durchgehender angriff über mitte von beiden seiten)
    // die letzten 4 orbs sind unten, gegner mit durchgehenden angriffen kommen von oben (und bleiben dort, um sie nicht von seite anzugreifen), und dann von oben seitlich
    // 1        2  -> to force player on lower line
    // 
    //    #--#
    //    |  |
    // #--#--#--#
    // 
    // 
    // player should not be moved if his current orb gets destroyed, to not move him into possible attack without being able to know or react (but provide way to both move into desired valid orb from there, and to return to far valid node as fallback)
    // TODO: highlight target orb or web ?
    // TODO: entferne cross movement
    // TODO: bullets have different distances from each other, maybe fix in POINT functions
    // TODO: bullets should not disappear here
    // TODO: kugerl die verschwinden sollen, aber der spieler noch belegt, verschwinden erst nachdem er wegmoved
    // TODO: nicht alle kugerl gleichzeitig zerstörem, verzögert
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        constexpr coreFloat fOrbLen     = 0.5f;
        constexpr coreFloat fOrbLenFore = fOrbLen * FOREGROUND_AREA.x;
        constexpr coreFloat fSpawn      = 1.2f / fOrbLen;

#if 0
        STAGE_ADD_PATH(pPath4)    
        {
            pPath4->Reserve(18u);
            pPath4->AddNode (coreVector2(-1.5f, fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            pPath4->AddNodes(coreVector2(-1.5f, 1.5f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2( 1.0f, 0.0f));
            pPath4->AddNodes(coreVector2( 1.5f, 1.5f)   * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath4->AddNodes(coreVector2( 1.5f, 0.5f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath4->AddNodes(coreVector2(-1.5f, 0.5f)   * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath4->AddNodes(coreVector2(-1.5f,-0.5f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2( 1.0f, 0.0f));
            pPath4->AddNodes(coreVector2( 1.5f,-0.5f)   * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath4->AddNodes(coreVector2( 1.5f,-1.5f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath4->AddNodes(coreVector2(-1.5f,-1.5f)   * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath4->AddNode (coreVector2(-1.5f,-fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            // no refine
        });
#endif

        STAGE_ADD_PATH(pPath4)    
        {
            pPath4->Reserve(24u);
            pPath4->AddNode (coreVector2(-1.5f, 1.5f)   * fOrbLen, coreVector2( 1.0f, 0.0f));
            pPath4->AddNodes(coreVector2( 1.5f, 1.5f)   * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath4->AddNodes(coreVector2( 1.5f, 0.5f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath4->AddNodes(coreVector2(-1.5f, 0.5f)   * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath4->AddNodes(coreVector2(-1.5f,-0.5f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2( 1.0f, 0.0f));
            pPath4->AddNodes(coreVector2( 1.5f,-0.5f)   * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath4->AddNodes(coreVector2( 1.5f,-1.5f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));

            pPath4->AddNodes(coreVector2(-1.5f,-1.5f)   * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath4->AddNodes(coreVector2(-1.5f,-0.5f)   * fOrbLen, coreVector2( 0.0f, 1.0f), coreVector2( 1.0f, 0.0f));
            pPath4->AddNodes(coreVector2( 1.5f,-0.5f)   * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath4->AddNodes(coreVector2( 1.5f, 0.5f)   * fOrbLen, coreVector2( 0.0f, 1.0f), coreVector2(-1.0f, 0.0f));
            pPath4->AddNodes(coreVector2(-1.5f, 0.5f)   * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath4->AddNode (coreVector2(-1.5f, 1.5f)   * fOrbLen, coreVector2( 0.0f, 1.0f));

            // no refine
        });

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(14u);
            pPath1->AddNode (coreVector2(-1.0f, fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            pPath1->AddNodes(coreVector2(-1.0f, 1.0f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNodes(coreVector2( 1.0f, 1.0f)   * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNodes(coreVector2( 1.0f, 0.0f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNodes(coreVector2(-1.0f, 0.0f)   * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNodes(coreVector2(-1.0f,-1.0f)   * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNodes(coreVector2( 1.0f,-1.0f)   * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode (coreVector2( 1.0f,-fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            // no refine
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            // no refine
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(1.0f, fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            pPath3->AddNode(coreVector2(1.0f,-fSpawn) * fOrbLen, coreVector2( 0.0f,-1.0f));
            // no refine
        });

        STAGE_ADD_PATH(pPath5)
        {
            pPath5->Reserve(8u);
            pPath5->AddNode (coreVector2(-0.5f, 0.5f) * fOrbLen, coreVector2( 1.0f, 0.0f));
            pPath5->AddNodes(coreVector2( 0.5f, 0.5f) * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath5->AddNodes(coreVector2( 0.5f,-0.5f) * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath5->AddNodes(coreVector2(-0.5f,-0.5f) * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath5->AddNode (coreVector2(-0.5f, 0.5f) * fOrbLen, coreVector2( 0.0f, 1.0f));
            // no refine
        });

        STAGE_ADD_PATH(pPath6)
        {
            pPath6->Reserve(8u);
            pPath6->AddNode (coreVector2(-1.5f, 1.5f) * fOrbLen, coreVector2( 1.0f, 0.0f));
            pPath6->AddNodes(coreVector2( 1.5f, 1.5f) * fOrbLen, coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath6->AddNodes(coreVector2( 1.5f,-1.5f) * fOrbLen, coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
            pPath6->AddNodes(coreVector2(-1.5f,-1.5f) * fOrbLen, coreVector2(-1.0f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath6->AddNode (coreVector2(-1.5f, 1.5f) * fOrbLen, coreVector2( 0.0f, 1.0f));
            // no refine
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 60u)         
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_MAGENTA);
            });
        });

        STAGE_GET_START(21u + 3u * GAME_PLAYERS)
            STAGE_GET_UINT_ARRAY  (aiTarget,  GAME_PLAYERS)
            STAGE_GET_VEC2_ARRAY  (avOldMove, GAME_PLAYERS)
            STAGE_GET_UINT64_ARRAY(aiBulletPtr, 10u)
            STAGE_GET_UINT        (iBulletNum)
        STAGE_GET_END

        const auto nPosFrom = [](const coreUintW iIndex)
        {
            return coreVector2::Direction(I_TO_F(iIndex) / I_TO_F(GELU_ORBS - 1u) * (9.0f*PI)) * FOREGROUND_AREA * 2.0f;
        };

        const auto nPosTo = [](const coreUintW iIndex)
        {
            return coreVector2(I_TO_F(iIndex % 4u) - 1.5f, I_TO_F(iIndex / 4u) - 1.5f) * FOREGROUND_AREA * fOrbLen;
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  2u)   
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  4u, 11u)   
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 20u, 27u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 28u, 35u)
            else if(STAGE_SUB(5u)) STAGE_RESSURECT(pSquad1, 36u, 43u)

            // 12 13 14 15
            // 08 09 10 11
            // 04 05 06 07
            // 00 01 02 03
            if(m_iStageSub == 2u)
            {
                // 12    14 15
                // 08 09 10 11
                // 04 05 06 07
                // 00 01 02 03
                //this->DisableOrb(13u, true);
                this->DisableOrb( 0u, true);
                this->DisableOrb( 3u, true);
                this->DisableOrb(12u, true);
                this->DisableOrb(15u, true);
            }
            else if(m_iStageSub == 3u)
            {
                // 12    14 15
                // 08 09 10
                //    05 06 07
                // 00 01 02 03
                //this->DisableOrb( 4u, true);
                //this->DisableOrb(11u, true);
                this->DisableOrb( 1u, true);
                this->DisableOrb( 2u, true);
                this->DisableOrb( 4u, true);
                this->DisableOrb( 7u, true);
                this->DisableOrb( 8u, true);
                this->DisableOrb(11u, true);
                this->DisableOrb(13u, true);
                this->DisableOrb(14u, true);
            }
            else if(m_iStageSub == 4u)
            {
                //       14
                // 08 09 10
                //    05 06
                // 00 01 02 03
                this->DisableOrb( 7u, true);
                this->DisableOrb(12u, true);
                this->DisableOrb(15u, true);
            }
            else if(m_iStageSub == 5u)
            {
                //
                //
                //    05 06
                // 00 01 02 03
                this->DisableOrb( 8u, true);
                this->DisableOrb( 9u, true);
                this->DisableOrb(10u, true);
                this->DisableOrb(14u, true);
                // 6er welle von oben
                // 1er von links
                // 1er von rechts
                // beide seiten
            }
            else if(m_iStageSub == 40u)  
            {
                //
                //
                //    05 06
                //    01 02
                this->DisableOrb( 2u, true);
                this->DisableOrb( 3u, true);
            }
            else if(m_iStageSub == 50u)  
            {
                for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                    this->DisableOrb(i, true);

                STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
                });
            }
        }

        if(STAGE_BEGINNING)
        {
            for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                this->EnableOrb(i);
        }

        if(STAGE_TIME_BEFORE(3.0f))
        {
            for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                m_aOrbRaw[i].SetPosition(coreVector3(LERPB(nPosFrom(i), nPosTo(i), CLAMP(m_fStageTime - 2.0f * (1.0f - (I_TO_F(i) / I_TO_F(GELU_ORBS - 1u))), 0.0f, 1.0f)), 0.0f));
        }
        else if(STAGE_TIME_POINT(3.0f))
        {
            for(coreUintW i = 0u; i < GELU_WEBS; ++i)
                this->EnableWeb(i);

            for(coreUintW i = 0u; i < GELU_ORBS; ++i)
                m_aOrbRaw[i].SetPosition(coreVector3(nPosTo(i), 0.0f));

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                coreUintW iIndex = 0u;
                coreFloat fLenSq = FLT_MAX;

                for(coreUintW j = 0u; j < GELU_ORBS; ++j)
                {
                    const coreFloat fCurLenSq = (m_aOrbRaw[j].GetPosition().xy() - pPlayer->GetPosition().xy()).LengthSq();
                    if(fCurLenSq < fLenSq)
                    {
                        iIndex = j;
                        fLenSq = fCurLenSq;
                    }
                }

                aiTarget [i] = iIndex;
                avOldMove[i] = pPlayer->GetInput()->vMove;

                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);
            });

            // TODO: orb waves
        }
        else
        {
            const auto nChangeTarget = [&](const coreUintW iIndex, const coreUint32 iNewTarget)
            {
                ASSERT(iNewTarget < GELU_ORBS)

                if(m_aOrbRaw[iNewTarget].IsEnabled(CORE_OBJECT_ENABLE_ALL))
                    aiTarget[iIndex] = iNewTarget;

                if(!m_aOrbRaw[aiTarget[iIndex]].IsEnabled(CORE_OBJECT_ENABLE_ALL))   // fallback
                {
                    const coreUint32 A = aiTarget[iIndex];
                         if((A ==  0u) || (A ==  1u) || (A ==  4u)) aiTarget[iIndex] =  5u;
                    else if((A ==  2u) || (A ==  3u) || (A ==  7u)) aiTarget[iIndex] =  6u;
                    else if((A ==  8u) || (A == 12u) || (A == 13u)) aiTarget[iIndex] =  9u;
                    else if((A == 11u) || (A == 14u) || (A == 15u)) aiTarget[iIndex] = 10u;
                }
            };

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreUint8   x      = (aiTarget[i] % 4u);
                const coreUint8   y      = (aiTarget[i] / 4u);
                const sGameInput* pInput = pPlayer->GetInput();

                if(SIGNUM(avOldMove[i].x) != SIGNUM(pInput->vMove.x))
                {
                         if((x > 0u) && (pInput->vMove.x < 0.0f)) nChangeTarget(i, aiTarget[i] - 1u);
                    else if((x < 3u) && (pInput->vMove.x > 0.0f)) nChangeTarget(i, aiTarget[i] + 1u);
                }
                if(SIGNUM(avOldMove[i].y) != SIGNUM(pInput->vMove.y))
                {
                         if((y > 0u) && (pInput->vMove.y < 0.0f)) nChangeTarget(i, aiTarget[i] - 4u);
                    else if((y < 3u) && (pInput->vMove.y > 0.0f)) nChangeTarget(i, aiTarget[i] + 4u);
                }

                avOldMove[i] = pInput->vMove;

                const coreVector2 vDiff = m_aOrbRaw[aiTarget[i]].GetPosition().xy() - pPlayer->GetPosition().xy();
                if(!vDiff.IsNull())
                {
                    const coreVector2 vPos = pPlayer->GetPosition().xy() + vDiff.Normalized() * (pPlayer->CalcMoveSpeed() * 3.0f * TIME * SmoothTowards(vDiff.Length(), 3.0f));  // 150.0f // pPlayer->CalcMoveSpeed() * 3.0f
                    pPlayer->SetPosition(coreVector3(vPos, 0.0f));
                }
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, ((i < 3u) ? 2.5f : 0.2f) + 0.1f * I_TO_F(i % ((i < 4u) ? 4u : 8u)))

            const coreSpline2* pPath = (i < 20u) ? pPath3 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2((i < 8u) ? 1.0f : -1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2((i < 20u) ? 0.0f : ((I_TO_F(((i - 20u) / 2u) % 4u) - 1.5f) * fOrbLen), 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
            
            if(i < 4u) pEnemy->Rotate270();

            //     if(i <  4u) {}
            //else if(i <  8u) pEnemy->Rotate90 ();
            //else if(i < 12u) pEnemy->Rotate270();
            //else if(i < 16u) pEnemy->Rotate180();
            //else if(i < 20u) {}
            //else if(i < 28u) {}
            //else if(i < 36u) pEnemy->Rotate90 ();

            if(!pEnemy->WasTeleporting() &&
               (STAGE_POSITION_POINT(pEnemy, -1.5f * fOrbLenFore, x) ||
                STAGE_POSITION_POINT(pEnemy, -0.5f * fOrbLenFore, x) ||
                STAGE_POSITION_POINT(pEnemy,  0.5f * fOrbLenFore, x) ||
                STAGE_POSITION_POINT(pEnemy,  1.5f * fOrbLenFore, x) ||
                STAGE_POSITION_POINT(pEnemy, -1.5f * fOrbLenFore, y) ||
                STAGE_POSITION_POINT(pEnemy, -0.5f * fOrbLenFore, y) ||
                STAGE_POSITION_POINT(pEnemy,  0.5f * fOrbLenFore, y) ||
                STAGE_POSITION_POINT(pEnemy,  1.5f * fOrbLenFore, y)))
            //; if(STAGE_TICK_LIFETIME(15.0f, 0.0f))
            {
                //const coreVector2 vPos = s_vPositionPoint;//pEnemy->GetPosition().xy();//
                //const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                //g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, vPos,  vDir)->ChangeSize(1.7f);
                //g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, vPos, -vDir)->ChangeSize(1.7f);
            }
        });

        if(STAGE_TICK_FREE(7.0f, 0.0f))
        {
            if(iBulletNum < 8u)
            {
                const coreVector2 vPos = m_aOrbRaw[3].GetPosition().xy();
                const coreVector2 vDir = coreVector2(1.0f,0.0f);

                cBullet* pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.7f);
                aiBulletPtr[iBulletNum++] = P_TO_UI(pBullet);
            }
        }

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cWaveBullet>([&](cWaveBullet* OUTPUT pBullet)
        {
            const coreUintW i = std::find(aiBulletPtr, aiBulletPtr + iBulletNum, P_TO_UI(pBullet)) - aiBulletPtr;
            ASSERT(i != SIZE_MAX)

            const coreVector2 vFactor = coreVector2((i < 4u) ? 1.0f : -1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2((i < 20u) ? 0.0f : ((I_TO_F(((i - 20u) / 2u) % 4u) - 1.5f) * fOrbLen), 0.0f);

            const coreSpline2* pPath = (i < 4u) ? pPath5 : pPath6;

            const coreFloat fLifeTime = FMOD(pBullet->GetFlyTime() * 0.7f, pPath->GetTotalDistance());


            coreVector2 vPosition;
            coreVector2 vDirection;
            pPath->CalcPosDir(CLAMP(fLifeTime, 0.0f, pPath->GetTotalDistance()), &vPosition, &vDirection);
        

            pBullet->SetPosition(coreVector3((vPosition + vOffset) * vFactor * FOREGROUND_AREA, 0.0f));
            pBullet->SetFlyDir  ((vDirection * vFactor).Normalized(vDirection));
            
        });

        STAGE_WAVE("DREIUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all webs and orbs
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        for(coreUintW i = 0u; i < GELU_WEBS; ++i)
            this->DisableWeb(i, false);

        for(coreUintW i = 0u; i < GELU_ORBS; ++i)
            this->DisableOrb(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN({TAKE_ALWAYS, 17u})
    //{
    //    STAGE_BOSS(m_Chol, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}