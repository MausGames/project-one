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
cIntroMission::cIntroMission()noexcept
{
    // 
    m_apBoss[0] = &m_ProjectOne;
    m_apBoss[1] = &m_ProjectOne;
    m_apBoss[2] = &m_ProjectOne;
}


// ****************************************************************
// setup the Intro mission
void cIntroMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateNormalShading();
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
        }

        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pGame->StartIntro();
        }

        if(STAGE_TIME_POINT(0.6f))
        {
            g_pEnvironment->ChangeBackground(cCloudBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeed(4.0f);
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.6f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
                if(i < 8u) pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iBulletShot)
        STAGE_GET_END

        if(g_pGame->GetBulletManagerPlayer()->GetNumBullets())
        {
            iBulletShot = 1u;
        }
        if(STAGE_TIME_POINT(10.0f) && !iBulletShot)
        {
            g_pGame->ActivatePacifist();
        }

        if(STAGE_CLEARED && STAGE_SUB(1u))
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                if(i >= 8u) pEnemy->Resurrect();
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.9f, 0.22f * I_TO_F(i % 8u))

            if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath1->GetTotalDistance())) {}

            const coreVector2 vFactor = coreVector2((i < 8u)    ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2((i & 0x01u) ? -0.1f : 0.1f, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f, 0.4f), coreVector2(1.0f,0.0f));
            pPath1->AddNode(coreVector2( 1.2f,-0.4f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
                if(i < 8u) pEnemy->Resurrect();
            });
        });

        if(STAGE_CLEARED && STAGE_SUB(1u))
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                if(i >= 8u) pEnemy->Resurrect();
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.9f, 0.21f * I_TO_F(i % 8u))

            if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath1->GetTotalDistance())) {}

            const coreVector2 vFactor = coreVector2((i < 8u) ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, 0.4f + ((i & 0x02u) ? -0.1f : 0.1f));

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_LIFETIME_POINT(0.3f) || STAGE_LIFETIME_POINT(0.4f) || STAGE_LIFETIME_POINT(0.5f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.4f,1.4f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.4f,0.6f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 2u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, (i & 0x01u) ? COLOR_SHIP_ORANGE : COLOR_SHIP_RED, true);
                pEnemy->Resurrect();

                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 2.0f);
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.0f)

            if(STAGE_LIFETIME_BEFORE(pPath1->GetTotalDistance()))
            {
                const coreVector2 vFactor = coreVector2((i & 0x01u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(STAGE_TICK_LIFETIME(20.0f, 0.0f))
            {
                const coreUint16 iTick = s_iTick % 80u;
                //if((iTick < 10u) || (iTick >= 20u && iTick < 30u))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * DEG_TO_RAD(9.0f));

                    if(i) vDir = vDir.InvertedX();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
                }
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_POINT(3.0f))
        {
            g_pGame->GetInterface()->ShowStory("Project One");
        }

        STAGE_BOSS(m_ProjectOne, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))

        if(STAGE_BEGINNING) this->ActivateWave();  
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            this->DeactivateWave();  

            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
                pPlayer->SetPosition(coreVector3(2.0f * FOREGROUND_AREA.x, 0.0f, 0.0f));
            });

            g_pGame->GetInterface()->SetVisible(false);

            g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
        }

        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateDarkShading();
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // ################################################################
}


// ****************************************************************
// 
void cIntroMission::__MoveOwnBefore()
{
    //if(CONTAINS_FLAG(g_pGame->GetPlayer(0u)->GetStatus(), PLAYER_STATUS_DEAD)) return;
    //
    //g_pEnvironment->SetTargetDirection(
    //coreVector2(g_pGame->GetPlayer(0u)->GetPosition().x / FOREGROUND_AREA.x * 0.2f, 1.0f).Normalized()
    //);
}