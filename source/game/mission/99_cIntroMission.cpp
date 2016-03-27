//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
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
    STAGE_CONFIG_INT  (1u)
    STAGE_CONFIG_FLOAT(20u)

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_INIT)
        {
            g_pEnvironment->ChangeBackground(REF_ID(cNoBackground::ID), ENVIRONMENT_MIX_FADE, 1.0f);
        }

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_TIME_INIT)
        {
            g_pGame->GetInterface()->ShowStory("What do you dream ?");
        }

        if(!g_pGame->GetInterface()->IsStoryActive(1.5f))
        {
            g_pEnvironment->ChangeBackground(REF_ID(cCloudBackground::ID), ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            g_pEnvironment->SetTargetSpeed  (8.0f);

            g_pGame->StartIntro();

            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(CONTAINS_VALUE(g_pGame->GetStatus(), GAME_STATUS_PLAY))
        {
            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // 
    STAGE_START_HERE
    STAGE_MAIN
    {
        // 
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddStop(coreVector2(-1.5f, 0.8f));
            pPath1->AddNode(coreVector2( 0.0f, 0.8f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->AddNode(coreVector2( 0.0f, 0.8f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath2->AddStop(coreVector2( 0.8f, 0.8f));
            pPath2->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f,-1.0f).Normalize() * 2.0f);
            pPath2->AddStop(coreVector2(-0.8f,-0.8f));
            pPath2->AddNode(coreVector2( 0.0f,-0.8f), coreVector2( 1.0f, 0.0f) * 2.0f);
            pPath2->AddStop(coreVector2( 0.8f,-0.8f));
            pPath2->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f, 1.0f).Normalize() * 2.0f);
            pPath2->AddStop(coreVector2(-0.8f, 0.8f));
            pPath2->AddLoop();
            pPath2->Refine();
        });

        // 
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 5u)
        {
            STAGE_SQUAD_FOREACH_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(200, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i) / I_TO_F(pSquad1->GetNumEnemies() - 1u)));
                pEnemy->Resurrect();
            });

            pSquad1->GetEnemy(0u)->GiveShield(ELEMENT_YELLOW);
        });

        // 
        STAGE_GET_START
        STAGE_GET_INT(iSecondRun)
        STAGE_GET_END

        // 
        STAGE_SQUAD_FOREACH_ALL(pSquad1, pEnemy, i)
        {
            const coreFloat fSide = iSecondRun ? -1.0f : 1.0f;

            STAGE_LIFETIME(pEnemy, fLifeTime*0.6f - I_TO_F(i)*0.2f)

            STAGE_BRANCH(fLifeTimeBefore, pPath1->GetTotalDistance(), pPath2->GetTotalDistance());

            if(STAGE_BRANCH(fLifeTime, pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pEnemy->DefaultMovePath(pPath1, coreVector2(fSide, 1.0f), coreVector2(0.0f,0.0f), fLifeTime);
            }
            else
            {

#define STAGE_TICK(c) (!(F_TO_UI((fLifeTime / 0.6f) * FRAMERATE_VALUE) % F_TO_UI(FRAMERATE_VALUE / I_TO_F(c))))

                pEnemy->DefaultMovePath(pPath2, coreVector2(fSide, 1.0f), coreVector2(0.0f,0.0f), fLifeTime);

             //   if(STAGE_LIFETIME_BETWEEN(0.5f, 0.7f))
                //if(InBetween(std::fmod(fLifeTime, pPath2->GetTotalDistance() / 5.0f), FRAMERATE_ROUND(0.5f), FRAMERATE_ROUND(0.7f)))
                //if(ABS(pEnemy->GetPosition().x) < 0.2f * FOREGROUND_AREA.x)
                
            }

            if(//InBetween( 0.0f * FOREGROUND_AREA.x, vPosBefore.x, pEnemy->GetPosition().x) ||
                   InBetween( 0.1f * FOREGROUND_AREA.x, vPosBefore.x, pEnemy->GetPosition().x) ||
                   InBetween( 0.3f * FOREGROUND_AREA.x, vPosBefore.x, pEnemy->GetPosition().x) ||
                   InBetween(-0.1f * FOREGROUND_AREA.x, vPosBefore.x, pEnemy->GetPosition().x) ||
                   InBetween(-0.3f * FOREGROUND_AREA.x, vPosBefore.x, pEnemy->GetPosition().x) ||
                   InBetween( 0.5f * FOREGROUND_AREA.x, vPosBefore.x, pEnemy->GetPosition().x) ||
                   InBetween(-0.5f * FOREGROUND_AREA.x, vPosBefore.x, pEnemy->GetPosition().x))
                {
                    //if(STAGE_TICK(10u))
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(),                          coreVector2(0.0f,-SIGN(pEnemy->GetPosition().y)))->MakeBlue();
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy() + coreVector2(0.0f,2.5f), coreVector2(0.0f,-SIGN(pEnemy->GetPosition().y)))->MakeBlue();
                }

            if(pEnemy->ReachedDeath())
            {
                // 
                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(),  vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(),  vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(), -vDir)->MakeGreen();
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(), -vDir)->MakeGreen();
                }

                // 
                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
            }
        });

        // 
        if(pSquad1->IsFinished())
        {
            if(iSecondRun++) STAGE_FINISH_NOW
            else
            {
                STAGE_SQUAD_FOREACH_ALL(pSquad1, pEnemy, i)
                {
                    pEnemy->Resurrect();
                });
            }
        }
    });

    /*
    // ################################################################
    // 
    STAGE_MAIN
    {
        // 
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->AddNode(coreVector2(-1.5f, 0.5f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.0f, 0.5f), coreVector2( 1.0f, 0.0f));
        });
        STAGE_ADD_PATH(pPath2)
        {
            pPath2->AddNode(coreVector2( 0.0f, 0.5f), coreVector2( 1.0f, 0.0f));
            pPath2->AddNode(coreVector2( 0.5f, 0.0f), coreVector2( 0.0f,-1.0f) * 0.5f);
            pPath2->AddNode(coreVector2( 0.0f,-0.5f), coreVector2(-1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.5f, 0.0f), coreVector2( 0.0f, 1.0f) * 2.0f);
            pPath2->AddLoop();
            pPath2->Refine();
        });

        // 
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 5u)
        {
            STAGE_SQUAD_INIT(pSquad1, Configure(50, COLOR_SHIP_BLUE))
            STAGE_SQUAD_INIT(pSquad1, Resurrect())
        });
        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 5u)
        {
            STAGE_SQUAD_INIT(pSquad2, Configure(50, COLOR_SHIP_RED))
            STAGE_SQUAD_INIT(pSquad2, Resurrect())
        });

        pSquad1->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            coreFloat fDistance = pEnemy->GetLifeTime()*0.6f - I_TO_F(i)*0.2f;

            if(STAGE_BRANCH(fDistance, pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pEnemy->DefaultMovePath(pPath1, coreVector2(1.0f,1.0f), coreVector2(0.0f,0.0f), fDistance);
            }
            else
            {
                pEnemy->DefaultMovePath(pPath2, coreVector2(1.0f,1.0f), coreVector2(0.0f,0.0f), fDistance);
            }
        });

        pSquad2->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            coreFloat fDistance = pEnemy->GetLifeTime()*0.6f - I_TO_F(i)*0.2f;

            if(STAGE_BRANCH(fDistance, pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pEnemy->DefaultMovePath(pPath1, coreVector2(-1.0f,-1.0f), coreVector2(0.0f,0.0f), fDistance);
            }
            else
            {
                pEnemy->DefaultMovePath(pPath2, coreVector2(-1.0f,-1.0f), coreVector2(0.0f,0.0f), fDistance);
            }
        });

        if(pSquad1->IsFinished() && pSquad2->IsFinished())
            STAGE_FINISH_NOW
    });
    */

    // ################################################################
    // 
    STAGE_MAIN
    {
        // 
        if(STAGE_TIME_INIT) m_ProjectOne.Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));

        // 
        if(CONTAINS_VALUE(m_ProjectOne.GetStatus(), ENEMY_STATUS_DEAD))
            STAGE_FINISH_NOW
    });
}