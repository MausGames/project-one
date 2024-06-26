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
// setup the Ater mission
void cAterMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cDarkBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

        g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
        g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
        g_pEnvironment->SetTargetSpeedNow    (1.2f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS})
    {
        cDarkBackground* pBackground = d_cast<cDarkBackground*>(g_pEnvironment->GetBackground());

        pBackground->Appear();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // show mission name
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_NAMELESS))
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
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        constexpr coreFloat fBaseScale = 1.5f;

        UNUSED STAGE_ADD_SQUAD(pSquad1, cMeteorEnemy, PROJECTONE_ENEMIES_METEOR)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 3.0f * fBaseScale);
                pEnemy->Configure(50 * 50, COLOR_SHIP_PURPLE / COLOR_SHIP_PURPLE.Max());
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_SECRET);
            });
        });

        UNUSED STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, PROJECTONE_ENEMIES_LEGION)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_WORTHLESS);
            });
        });

        if(g_pGame->IsEasy()) STAGE_BOSS(m_ProjectOne, {300.0f, 450.0f, 600.0,  750.0f, 1500.0f})
                         else STAGE_BOSS(m_ProjectOne, {460.0f, 690.0f, 920.0, 1150.0f, 2300.0f})   // +10
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMeteorEnemy>();
        g_pGame->PrefetchBoss();
    });

    // ################################################################
    // end
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(STAGE_BEGINNING)
        {
            coreUint8& iBonus1 = g_pSave->EditProgress()->aiAdvance[9];
            iBonus1 = MAX(iBonus1, 1u);

            coreUint8& iBonus2 = g_pSave->EditProgress()->aiAdvance[10];
            iBonus2 = MAX(iBonus2, 1u);
        }

        if(m_bSecret)
        {
            STAGE_FINISH_NOW
        }
        else
        {
            STAGE_FINISH_AFTER(MISSION_WAIT_OUTRO)
        }
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(m_bSecret)
        {
            STAGE_FINISH_NOW
        }
        else
        {
            if(STAGE_BEGINNING)
            {
                if(m_iCredits < 1u)
                {
                    g_pGame->StartOutro(GAME_OUTRO_ENDING_NORMAL);

                    g_pGame->GetInterface()->SetVisibleSpeed(0.5f);
                }
                else
                {
                    ASSERT(m_iTakeTo != TAKE_MISSION)
                    g_pGame->StartOutro(GAME_OUTRO_SEGMENT);   // do not fade music
                }

                g_MusicPlayer.SelectName("ending_normal");
                g_MusicPlayer.Play();
            }
        }
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u, 6u})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->AddStatus(PLAYER_STATUS_INVINCIBLE_2);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_TRAINING, 6u})
    {
        this->TransformPlayers();

        cDarkBackground* pBackground = d_cast<cDarkBackground*>(g_pEnvironment->GetBackground());

        for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
        {
            pBackground->SetBlockHeight(i, 0.0f);
            pBackground->SetBlockColor (i, DARK_COLOR_DEFAULT * 0.5f);
        }

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_TRAINING, 6u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u, 6u})
    {
        STAGE_BOSS(m_Eigengrau, {140.0f + 1.0f, 210.0f, 280.0f, 350.0f, 700.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTiltBullet>();
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u, 6u})
    {
        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_INVINCIBLE);
            pPlayer->RemoveStatus(PLAYER_STATUS_INVINCIBLE_2);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u, 6u})
    {
        if(STAGE_BEGINNING)
        {
            if(m_iCredits < 2u)
            {
                g_pGame->StartOutro(GAME_OUTRO_ENDING_SECRET);
            }
            else
            {
                ASSERT(m_iTakeTo != TAKE_MISSION)
                g_pGame->StartOutro(GAME_OUTRO_SEGMENT);   // do not fade music
            }
        }
    });

    // ################################################################
    // ################################################################
}