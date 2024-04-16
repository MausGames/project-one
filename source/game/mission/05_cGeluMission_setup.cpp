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

    // ################################################################
    // <REPLACE>                                                       
    STAGE_MAIN({TAKE_ALWAYS, 14u})
    {
        STAGE_WAVE("DREIUNDSIEBZIG", {20.0f, 30.0f, 40.0f, 50.0f})
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