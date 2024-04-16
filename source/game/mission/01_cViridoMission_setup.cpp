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
// setup the Virido mission
void cViridoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        //STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        //{
        //    pPlayer->ActivateDarkShading();
        //});

        STAGE_FINISH_NOW
    });

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
        g_pEnvironment->ChangeBackground(cGrassBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        g_pEnvironment->SetTargetSpeedNow(6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance (split)
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
    {
#if !defined(_P1_VIDEO_)
        //g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeightNow(0.4583f, -13.83f);
        //g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
        //g_pEnvironment->GetBackground()->SetGroundDensity(1u, 0.0f);
        //g_pEnvironment->GetBackground()->SetGroundDensity(2u, 0.0f);
        //g_pEnvironment->GetBackground()->SetDecalDensity (0u, 0.0f);
        //g_pEnvironment->GetBackground()->SetAirDensity   (0u, 0.0f);
#endif

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
    // shields on sides to force attacks from certain directions
    // [deprecated] - 3: starts with offset, to not fly into players
    // [deprecated] - 4: reflect all bullets and players with force, except with barrel roll
    // [deprecated] - 5: arranged to allow only one kill per turn, and to improve coop gameplay
    // position after killing enemies (based on their shield position) needs to be considered when spawning next wave
    // first long shields need to push 50% of the area, to remove safe-spots
    // side-moving long shields need to push in to wall, to remove the passage longer
    // frequency (and amplitude) of wave-pattern needs to be high enough to allow easy side-attacking
    // barrier indices are manual, to control overdraw
    // TODO 1: bullets (even single, like pulse) should be reflected with offset to highlight reflection (not 180 degree)
    // TODO 1: render-reihenfolge anpassen
    // TODO 1: barriers have cut-off outline in maze-group   
    // TODO 1: gruppe mit schrägen schilden in mitte is weird, startet so dass man gleich alle gegner zerstört, und wirkt sehr abrupt, weil sonst nichts mit schrägen schilden vorkommt
    // TODO 1: noch eine gruppe mit rotation oder schräg ?
    // TODO 1: is pPath2 still required, does group spawn and end correctly considering the size
    // TODO 1: add better blend-out animation (make smaller/bigger ?)
    // TODO 1: enemy-attacks on wave-pattern needs to be reduced
    // TODO 1: enemy-attack on dungeon needs to be delayed further
    // TODO 1: dungeon with or without extra-walls ?
    // TODO 1: maybe spinning enemies should move faster into the middle, but what about their shooting ?
    // TODO 1: wave-pattern needs better fly-in
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreFloat fWidth = 0.38f;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.4f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.4f + fWidth * 1.0f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f, 0.0f + fWidth * 0.5f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f, 1.4f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,-1.4f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 53u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_CYAN);
            });
        });

        for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
        {
            if(m_apBarrierOwner[i] && m_apBarrierOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableBarrier(i, true);
        }

        if(STAGE_CLEARED)
        {
            if(STAGE_SUB(1u))
            {
                STAGE_RESURRECT(pSquad1, 0u, 1u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy( 1u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy( 1u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 2u, 7u)
                this->EnableBarrier( 4u, pSquad1->GetEnemy( 2u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier( 5u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy( 5u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 6u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy( 7u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 8u, 13u)
                this->EnableBarrier(10u, pSquad1->GetEnemy( 8u), coreVector2( 1.0f, 0.0f).Rotated90(), 0, 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy( 8u), coreVector2(-1.0f, 0.0f).Rotated90(), 0, 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy( 9u), coreVector2( 1.0f, 0.0f),             0, 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy( 9u), coreVector2(-1.0f, 0.0f),             0, 1.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy(10u), coreVector2( 1.0f, 0.0f).Rotated90(), 0, 1.0f);
                this->EnableBarrier(15u, pSquad1->GetEnemy(10u), coreVector2(-1.0f, 0.0f).Rotated90(), 0, 1.0f);
                this->EnableBarrier(16u, pSquad1->GetEnemy(11u), coreVector2( 1.0f, 0.0f).Rotated90(), 0, 1.0f);
                this->EnableBarrier(17u, pSquad1->GetEnemy(11u), coreVector2(-1.0f, 0.0f).Rotated90(), 0, 1.0f);
                this->EnableBarrier(18u, pSquad1->GetEnemy(12u), coreVector2( 1.0f, 0.0f),             0, 1.0f);
                this->EnableBarrier(19u, pSquad1->GetEnemy(12u), coreVector2(-1.0f, 0.0f),             0, 1.0f);
                this->EnableBarrier(20u, pSquad1->GetEnemy(13u), coreVector2( 1.0f, 0.0f).Rotated90(), 0, 1.0f);
                this->EnableBarrier(21u, pSquad1->GetEnemy(13u), coreVector2(-1.0f, 0.0f).Rotated90(), 0, 1.0f);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 14u, 17u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy(14u), coreVector2( 0.0f, 1.0f), 0, 3.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy(15u), coreVector2( 1.0f, 0.0f), 0, 3.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy(16u), coreVector2( 0.0f,-1.0f), 0, 3.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy(17u), coreVector2(-1.0f, 0.0f), 0, 3.0f);
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESURRECT(pSquad1, 18u, 23u)
                this->EnableBarrier( 4u, pSquad1->GetEnemy(18u), coreVector2( 1.0f, 0.0f),             6, 1.0f);
                this->EnableBarrier( 5u, pSquad1->GetEnemy(18u), coreVector2(-1.0f, 0.0f),             6, 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy(19u), coreVector2( 1.0f, 0.0f).Rotated90(), 6, 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy(19u), coreVector2(-1.0f, 0.0f).Rotated90(), 6, 1.0f);
                this->EnableBarrier( 8u, pSquad1->GetEnemy(20u), coreVector2( 1.0f, 0.0f),             6, 1.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy(20u), coreVector2(-1.0f, 0.0f),             6, 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy(21u), coreVector2( 1.0f, 0.0f).Rotated90(), 6, 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy(21u), coreVector2(-1.0f, 0.0f).Rotated90(), 6, 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy(22u), coreVector2( 1.0f, 0.0f),             6, 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy(22u), coreVector2(-1.0f, 0.0f),             6, 1.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy(23u), coreVector2( 1.0f, 0.0f).Rotated90(), 6, 1.0f);
                this->EnableBarrier(15u, pSquad1->GetEnemy(23u), coreVector2(-1.0f, 0.0f).Rotated90(), 6, 1.0f);
            }
            else if(STAGE_SUB(6u))
            {
                STAGE_RESURRECT(pSquad1, 24u, 28u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy(24u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy(24u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy(25u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy(26u), coreVector2(-1.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(16u, pSquad1->GetEnemy(26u), coreVector2( 1.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(17u, pSquad1->GetEnemy(27u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(18u, pSquad1->GetEnemy(28u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(19u, pSquad1->GetEnemy(28u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 29u, 32u)
                this->EnableBarrier( 6u, pSquad1->GetEnemy(29u), coreVector2( 0.0f, 1.0f), 0, 3.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy(30u), coreVector2( 0.0f, 1.0f), 0, 3.0f);
                this->EnableBarrier( 8u, pSquad1->GetEnemy(31u), coreVector2( 0.0f, 1.0f), 0, 3.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy(32u), coreVector2( 0.0f, 1.0f), 0, 3.0f);
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 33u, 36u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy(33u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy(34u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy(35u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy(36u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 37u, 40u)
                this->EnableBarrier( 6u, pSquad1->GetEnemy(37u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy(38u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
                this->EnableBarrier( 8u, pSquad1->GetEnemy(39u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy(40u), coreVector2( 0.0f,-1.0f), INT8_MAX, 1.0f);
            }
            else if(STAGE_SUB(10u))
            {
                STAGE_RESURRECT(pSquad1, 41u, 52u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy(41u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy(41u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy(43u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy(43u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier( 4u, pSquad1->GetEnemy(44u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier( 5u, pSquad1->GetEnemy(44u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy(44u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy(45u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy(46u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy(47u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy(49u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(15u, pSquad1->GetEnemy(49u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(16u, pSquad1->GetEnemy(50u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(17u, pSquad1->GetEnemy(50u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(18u, pSquad1->GetEnemy(50u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier(19u, pSquad1->GetEnemy(51u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                this->EnableBarrier(20u, pSquad1->GetEnemy(51u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                this->EnableBarrier(21u, pSquad1->GetEnemy(52u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(22u, pSquad1->GetEnemy(52u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                this->EnableBarrier(23u, pSquad1->GetEnemy(52u), coreVector2( 0.0f, 1.0f), 0, 1.0f);

                //this->EnableBarrier(24u, pSquad1->GetEnemy(42u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                //this->EnableBarrier(25u, pSquad1->GetEnemy(42u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                //this->EnableBarrier(26u, pSquad1->GetEnemy(43u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                //this->EnableBarrier(27u, pSquad1->GetEnemy(45u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                //this->EnableBarrier(28u, pSquad1->GetEnemy(46u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                //this->EnableBarrier(29u, pSquad1->GetEnemy(47u), coreVector2( 0.0f,-1.0f), 0, 1.0f);
                //this->EnableBarrier(30u, pSquad1->GetEnemy(47u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
                //this->EnableBarrier(31u, pSquad1->GetEnemy(48u), coreVector2(-1.0f, 0.0f), 0, 1.0f);
                //this->EnableBarrier(32u, pSquad1->GetEnemy(48u), coreVector2( 1.0f, 0.0f), 0, 1.0f);
                //this->EnableBarrier(33u, pSquad1->GetEnemy(49u), coreVector2( 0.0f, 1.0f), 0, 1.0f);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 2u || i >= 41u) ? 1.2f : 0.6f, (i < 8u || (i >= 24u && i < 29u) || i >= 33u) ? 0.0f : ((i < 18u) ? (0.4f * I_TO_F((i - 8u) % 6u)) : ((i < 24u) ? (0.5f * I_TO_F(i - 18u)) : (0.8f * I_TO_F(i - 29u)))))

            if(i < 2u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i % 2u) ? 0.25f : -0.25f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 8u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i - 2u) - 2.5f) * fWidth, MIN1(fLifeTime) * 0.4f * SIN((fLifeTime * 0.3f + I_TO_F(i - 2u) * 0.15f) * (2.0f*PI)));

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate180();
            }
            else if(i < 14u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 8u) * 2u + 2u) % 6u) - 2.5f) * 0.34f * ((i < 11u) ? 1.0f : -1.0f), 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 18u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(-0.54f, 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 15u) pEnemy->Rotate180();
                else if(i < 16u) pEnemy->Rotate270();
                else if(i < 17u) {}
                else if(i < 18u) pEnemy->Rotate90 ();
            }
            else if(i < 24u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 18u) % 3u) - 2.0f) * fWidth, 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();

                if((i - 18u) % 3u == 1u) pEnemy->InvertX();
            }
            else if(i < 29u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((I_TO_F(i - 24u) - 2.0f) * fWidth), (i == 26u) ? 0.15f : 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 33u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((((i - 29u) % 2u) ? -0.6f : 0.6f) * SIN(fLifeTime * (1.0f*PI)), 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate180();
            }
            else if(i < 41u)
            {
                const coreFloat   fSide = (i < 37u) ? -1.0f : 1.0f;
                const coreVector2 vDir  = coreVector2::Direction((fLifeTime * 3.0f - I_TO_F((i - 33u) % 4u) * ((i < 37u) ? (0.1f*PI) : (0.5f*PI))) * fSide);
                const coreFloat   fLen  = LERPB(1.6f, 0.6f, MIN1(fLifeTime * 0.3f)) * FOREGROUND_AREA.x;

                pEnemy->SetPosition(coreVector3(vDir * fLen, 0.0f));
            }
            else if(i < 53u)
            {
                const coreUintW x = (i - 41u) % 3u;
                const coreUintW y = (i - 41u) / 3u;

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(x) - 1.0f) * fWidth, (I_TO_F(y) - 1.5f) * fWidth);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(STAGE_LIFETIME_AFTER(0.7f) && STAGE_TICK_LIFETIME(1.0f, 0.0f) && (i < 41u || i == 51u))
            {
                const coreVector2 vPos  = pEnemy->GetPosition().xy();
                const coreFloat   fBase = pEnemy->AimAtPlayerSide().Angle();

                for(coreUintW j = 3u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 1.0f) * 4.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 0.6f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        });

        STAGE_WAVE("EINS", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all barriers
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            this->DisableBarrier(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // cut with laser through everything
    // the delay when rays "appear" is important, not when they "stop"
    // rays should not come from below, first segments main direction is upwards
    // 1, 2 coming from upper left, 3, 4 coming from upper right
    // 1 starts right to left, 2 starts left to right, 3 starts right to left, 4 starts left to right
    // enemies on the line spawn in the middle of the line
    // first two patterns have no single equal line
    // no intro-crossing in first two patterns
    // bullets have a 3-delay pattern, but on the side and when enemies die the pattern might break
    // fast line shows the player that there is no harm, in preparation for the multi-line wave
    // enemies in 3 need to be positioned to easily kill them (to reduce pressure, which is reserved for later)
    // TODO 1: purple helper visible at border of one laser
    // TODO 1: fix laser collision
    // TODO 1: invert rotation of cross, or of 5-eck ?
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreUintW iNumData = 8u;

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 43u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(30, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_GET_START(iNumData * 3u + 1u)
            STAGE_GET_FLOAT_ARRAY(afDistance, iNumData)
            STAGE_GET_VEC2_ARRAY (avMove,     iNumData)
            STAGE_GET_UINT       (iCurLaser)
        STAGE_GET_END

        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
        {
            if(m_apLaserOwner[i] && m_apLaserOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableLaser(i, true);
        }

        if(STAGE_CLEARED)
        {
            const auto nEnableLaserFunc = [&](const coreUintW iIndex)
            {
                this->EnableLaser(iCurLaser, pSquad1->GetEnemy(iIndex));
                if(++iCurLaser >= VIRIDO_LASERS) iCurLaser = 0u;
            };

            if(STAGE_SUB(1u))
            {
                STAGE_RESURRECT(pSquad1, 0u, 2u)
                nEnableLaserFunc( 0u);
                nEnableLaserFunc( 1u);
                nEnableLaserFunc( 2u);
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 3u, 6u)
                nEnableLaserFunc( 3u);
                nEnableLaserFunc( 4u);
                nEnableLaserFunc( 5u);
                nEnableLaserFunc( 6u);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 7u, 10u)
                nEnableLaserFunc( 7u);
                nEnableLaserFunc( 8u);
                nEnableLaserFunc( 9u);
                nEnableLaserFunc(10u);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 11u, 14u)
                nEnableLaserFunc(11u);
                nEnableLaserFunc(12u);
                nEnableLaserFunc(13u);
                nEnableLaserFunc(14u);
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESURRECT(pSquad1, 15u, 15u)
                nEnableLaserFunc(15u);
            }
            else if(STAGE_SUB(6u))
            {
                STAGE_RESURRECT(pSquad1, 16u, 23u)
                nEnableLaserFunc(16u);
                nEnableLaserFunc(17u);
                nEnableLaserFunc(18u);
                nEnableLaserFunc(19u);
                nEnableLaserFunc(20u);
                nEnableLaserFunc(21u);
                nEnableLaserFunc(22u);
                nEnableLaserFunc(23u);
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 24u, 29u)
                nEnableLaserFunc(24u);
                nEnableLaserFunc(25u);
                nEnableLaserFunc(26u);
                nEnableLaserFunc(27u);
                nEnableLaserFunc(28u);
                nEnableLaserFunc(29u);
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 30u, 34u)
                nEnableLaserFunc(30u);
                nEnableLaserFunc(31u);
                nEnableLaserFunc(32u);
                nEnableLaserFunc(33u);
                nEnableLaserFunc(34u);
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 35u, 42u)
                nEnableLaserFunc(35u);
            }

            std::memset(afDistance, 0, sizeof(coreFloat) * iNumData);
        }

        constexpr coreUintW iSharedIndex = 35u;

        const coreObject3D* pSharedLaser  = (*m_Laser.List())[iSharedIndex % VIRIDO_LASERS];
        const coreBool      bSharedActive = pSharedLaser->IsEnabled(CORE_OBJECT_ENABLE_ALL);   // outside, for consistent movement

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vBasePos, vBaseDir;
            coreFloat   fDelay, fShift, fSide;
            switch(i)
            {
            default: ASSERT(false)
            case  0u: vBasePos = coreVector2( 0.7f,-0.1f); vBaseDir = coreVector2(-2.0f,-1.0f).Normalized(); fDelay = 0.0f;        fShift =    0.0f; fSide = -1.0f; break;
            case  1u: vBasePos = coreVector2( 0.2f,-0.7f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.3f;        fShift =  -15.0f; fSide =  1.0f; break;
            case  2u: vBasePos = coreVector2( 0.0f, 0.8f); vBaseDir = coreVector2( 1.0f, 1.0f).Normalized(); fDelay = 0.45f;       fShift =  -35.0f; fSide =  1.0f; break;

            case  3u: vBasePos = coreVector2( 0.1f, 0.8f); vBaseDir = coreVector2( 1.0f, 2.0f).Normalized(); fDelay = 0.0f;        fShift =  -30.0f; fSide =  1.0f; break;
            case  4u: vBasePos = coreVector2( 0.2f, 0.2f); vBaseDir = coreVector2( 1.0f,-1.0f).Normalized(); fDelay = 0.3f;        fShift =  -35.0f; fSide =  1.0f; break;
            case  5u: vBasePos = coreVector2( 0.5f,-0.6f); vBaseDir = coreVector2( 1.0f, 5.0f).Normalized(); fDelay = 0.4f;        fShift =    0.0f; fSide = -1.0f; break;
            case  6u: vBasePos = coreVector2(-0.8f, 0.1f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.7f;        fShift =  -25.0f; fSide = -1.0f; break;

            case  7u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;        fShift =  -30.0f; fSide = -1.0f; break;
            case  8u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.25f;       fShift =  -30.0f; fSide =  1.0f; break;
            case  9u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;        fShift =  -30.0f; fSide = -1.0f; break;
            case 10u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.75f;       fShift =  -30.0f; fSide =  1.0f; break;

            case 11u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;        fShift =    0.0f; fSide = -1.0f; break;
            case 12u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.25f;       fShift =    0.0f; fSide =  1.0f; break;
            case 13u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;        fShift =    0.0f; fSide = -1.0f; break;
            case 14u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.75f;       fShift =    0.0f; fSide =  1.0f; break;

            case 15u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.0f;        fShift =    0.0f; fSide =  1.0f; break;

            case 16u: vBasePos = coreVector2( 0.0f,-0.8f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.0f * 0.9f; fShift =  -70.0f; fSide =  1.0f; break;
            case 17u: vBasePos = coreVector2( 0.0f,-0.7f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.1f * 0.9f; fShift =  -70.0f; fSide = -1.0f; break;
            case 18u: vBasePos = coreVector2( 0.0f,-0.6f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.2f * 0.9f; fShift =  -70.0f; fSide =  1.0f; break;
            case 19u: vBasePos = coreVector2( 0.0f,-0.5f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.3f * 0.9f; fShift =  -70.0f; fSide = -1.0f; break;
            case 20u: vBasePos = coreVector2( 0.0f,-0.4f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.4f * 0.9f; fShift =  -70.0f; fSide =  1.0f; break;
            case 21u: vBasePos = coreVector2( 0.0f,-0.3f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f * 0.9f; fShift =  -70.0f; fSide = -1.0f; break;
            case 22u: vBasePos = coreVector2( 0.0f,-0.2f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.6f * 0.9f; fShift =  -70.0f; fSide =  1.0f; break;
            case 23u: vBasePos = coreVector2( 0.0f,-0.1f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.7f * 0.9f; fShift =  -70.0f; fSide = -1.0f; break;

            case 24u: vBasePos = coreVector2( 0.0f, 0.1f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.0f;        fShift =    0.0f; fSide =  1.0f; break;
            case 25u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;        fShift =  -20.0f; fSide = -1.0f; break;
            case 26u: vBasePos = coreVector2( 0.0f,-0.1f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.0f;        fShift =  -40.0f; fSide =  1.0f; break;
            case 27u: vBasePos = coreVector2( 0.1f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.0f;        fShift =  -60.0f; fSide =  1.0f; break;
            case 28u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.0f;        fShift =  -80.0f; fSide = -1.0f; break;
            case 29u: vBasePos = coreVector2(-0.1f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.0f;        fShift = -100.0f; fSide =  1.0f; break;

            case 30u:
            case 31u:
            case 32u:
            case 33u:
            case 34u:
                vBasePos = coreVector2(0.0f,0.0f); vBaseDir = coreVector2::Direction(I_TO_F(i) * (0.4f*PI)); fDelay = 0.0f; fShift = 0.0f; fSide = 1.0f;
                break;

            case 35u:
            case 36u:
            case 37u:
            case 38u:
            case 39u:
            case 40u:
            case 41u:
            case 42u:
                vBasePos = coreVector2(0.0f,0.0f); vBaseDir = coreVector2(1.0f,0.0f); fDelay = 0.0f; fShift = 0.0f; fSide = 1.0f;
                break;
            }

            STAGE_LIFETIME(pEnemy, 0.25f, fDelay)

            const coreVector2 vTan  = vBaseDir.Rotated90() * fSide;
            const coreFloat   fTime = MAX0(fLifeTime);

            coreVector2 vLerpPos;
                 if(i <  7u) vLerpPos = LERPB(vBasePos - vTan * 2.0f, vBasePos,          MIN1(fTime * 1.2f)) * (FOREGROUND_AREA);
            else if(i < 11u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 0.6f)) * (FOREGROUND_AREA * 1.2f);
            else if(i < 15u) vLerpPos = LERP (-vTan,                  vTan, 0.5f - 0.5f * COS(fTime * 2.1f)) * (FOREGROUND_AREA * 1.2f);
            else if(i < 16u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 2.0f)) * (FOREGROUND_AREA * 1.2f);
            else if(i < 24u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 0.6f)) * (FOREGROUND_AREA * 1.2f);
            else if(i < 30u) vLerpPos = LERPB(vBasePos - vTan * 2.0f, vBasePos,          MIN1(fTime * 1.2f)) * (FOREGROUND_AREA)        * coreMatrix3::Rotation(fTime * 2.0f).m12();
            else if(i < 35u) vLerpPos = LERPB(-vTan * 1.4f,          -vTan * 0.5f,       MIN1(fTime * 1.2f)) * (FOREGROUND_AREA * 1.2f) * coreMatrix3::Rotation(fTime * 1.1f).m12();
            else if(i < 43u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 0.8f)) * (FOREGROUND_AREA * 1.2f);
            else ASSERT(false)

            coreVector2 vLerpDir;
                 if(i <  7u) vLerpDir = vBaseDir;
            else if(i < 11u) vLerpDir = vBaseDir;
            else if(i < 15u) vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime * 1.65f).m12();
            else if(i < 16u) vLerpDir = vBaseDir;
            else if(i < 24u) vLerpDir = vBaseDir;
            else if(i < 30u) vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime * 2.0f).m12();
            else if(i < 35u) vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime * 1.1f).m12();
            else if(i < 43u) vLerpDir = vBaseDir;
            else ASSERT(false)

            coreFloat& fDistance = afDistance[i % iNumData];

            if(STAGE_TAKEOFF) fDistance = fShift;
            fDistance += 100.0f * (fLifeTime - fLifeTimeBefore);

            const coreVector2 vNewPos = vLerpPos + vLerpDir * fDistance;

            if(i < 16u)   // also for multi-line wave
            {
                if(((vNewPos.x < -FOREGROUND_AREA.x * 1.2f) && (vLerpDir.x < 0.0f)) ||
                   ((vNewPos.x >  FOREGROUND_AREA.x * 1.2f) && (vLerpDir.x > 0.0f)) ||
                   ((vNewPos.y < -FOREGROUND_AREA.y * 1.2f) && (vLerpDir.y < 0.0f)) ||
                   ((vNewPos.y >  FOREGROUND_AREA.y * 1.2f) && (vLerpDir.y > 0.0f)))
                {
                    if(fDistance > 0.0f) fDistance -= MAX(g_pForeground->RayIntersection(vNewPos, -vLerpDir, 1.2f), 0.0f);
                }
            }
            else
            {
                if(fDistance > FOREGROUND_AREA.x * 1.4f) fDistance -= FOREGROUND_AREA.x * 2.8f;
            }

            if(pEnemy == m_apLaserOwner[i % VIRIDO_LASERS])
            {
                coreObject3D* pLaser = (*m_Laser.List())[i % VIRIDO_LASERS];

                pLaser->SetPosition (coreVector3(vLerpPos, 0.0f));
                pLaser->SetDirection(coreVector3(vLerpDir, 0.0f));
            }

            if(i < iSharedIndex)
            {
                pEnemy->SetPosition (coreVector3(vNewPos,  0.0f));
                pEnemy->SetDirection(coreVector3(vLerpDir, 0.0f));
            }
            else
            {
                if(bSharedActive)
                {
                    const coreVector2 vNewPos2 = coreVector2((I_TO_F(i - iSharedIndex) - 3.5f) * 0.25f * FOREGROUND_AREA.x, pSharedLaser->GetPosition().y);

                    pEnemy->SetPosition (coreVector3(vNewPos2, 0.0f));
                    pEnemy->SetDirection(coreVector3(vLerpDir, 0.0f));

                    if(pEnemy->ReachedDeath())
                    {
                        this->DisableLaser(iSharedIndex % VIRIDO_LASERS, true);

                        for(coreUintW j = 0u; j < iNumData; ++j)
                        {
                            avMove[j] = coreVector2::Direction((I_TO_F(j) - 3.5f) * (-0.12f*PI));
                        }
                    }
                }
                else
                {
                    ASSERT((i - iSharedIndex) < iNumData)

                    coreVector2& vMove    = avMove[i - iSharedIndex];
                    coreVector2  vNewPos2 = pEnemy->GetPosition().xy() + vMove * (30.0f * TIME);

                         if((vNewPos2.x < -FOREGROUND_AREA.x * 1.1f) && (vMove.x < 0.0f)) {vNewPos2.x -= 2.0f * (vNewPos2.x + FOREGROUND_AREA.x * 1.1f); vMove.x =  ABS(vMove.x);}
                    else if((vNewPos2.x >  FOREGROUND_AREA.x * 1.1f) && (vMove.x > 0.0f)) {vNewPos2.x -= 2.0f * (vNewPos2.x - FOREGROUND_AREA.x * 1.1f); vMove.x = -ABS(vMove.x);}
                         if((vNewPos2.y < -FOREGROUND_AREA.y * 1.1f) && (vMove.y < 0.0f)) {vNewPos2.y -= 2.0f * (vNewPos2.y + FOREGROUND_AREA.y * 1.1f); vMove.y =  ABS(vMove.y);}
                    else if((vNewPos2.y >  FOREGROUND_AREA.y * 1.1f) && (vMove.y > 0.0f)) {vNewPos2.y -= 2.0f * (vNewPos2.y - FOREGROUND_AREA.y * 1.1f); vMove.y = -ABS(vMove.y);}

                    pEnemy->SetPosition (coreVector3(vNewPos2, 0.0f));
                    pEnemy->SetDirection(coreVector3(vMove,    0.0f));
                }
            }

            if(STAGE_TICK_TIME(12.0f, 0.0f) && ((s_iTick % 12u) < 3u))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.1f)->SetFlyTime(-0.5f);   // preserve patterns in rotation waves
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.1f)->SetFlyTime(-0.5f);
            }
        });

        STAGE_WAVE("ZWEI", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all lasers
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
            this->DisableLaser(i, false);

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
    // enemy has weakpoint which he keeps away from player
    // - all: miners rotation start against fly direction, to make evading the initial attack easier
    // - 2: cinders in pair always rotate together away from player when one is shot from outer direction, to prevent ping-pong effect
    // TODO 1: miner should be grey ?   
    // TODO 1: don't actually rotate away, but stay fixed or rotate consistent
    // TODO 1: rotate like an atom across center-enemy
    // TODO 1: snakehead with tail, destroy tail
    // TODO 1: chain -> other weakpoint destroys enemy protecting next weakpoint (inside enemy but visible) across the field
    // TODO 1: weakpoint rolls around border with high speed, flies like ZeroRanger fast enemies across screen, are like a wall with enemy at center
    // TODO 1: weakpoint dies faster, whole wave needs to be faster
    // TODO 1: actually damage and blink other enemy
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.5f,1.4f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.5f,0.0f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(5u);
            pPath2->AddNode(coreVector2( 0.5f, 0.0f), coreVector2( 0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 0.0f,-0.5f), coreVector2(-1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.5f, 0.0f), coreVector2( 0.0f, 1.0f));
            pPath2->AddNode(coreVector2( 0.0f, 0.5f), coreVector2( 1.0f, 0.0f));
            pPath2->AddLoop();
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(1, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cCinderEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(32u)
            STAGE_GET_FLOAT_ARRAY(afTarget,    10u, afTarget[1] = 1.0f*PI, afTarget[2] = 1.0f*PI, afTarget[6] = 1.5f*PI, afTarget[8] = 0.5f*PI)
            STAGE_GET_FLOAT_ARRAY(afSign,      10u)
            STAGE_GET_UINT_ARRAY (aiFreeState,  4u)
            STAGE_GET_VEC2_ARRAY (aiFreeMove,   4u)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            if(pEnemy->ReachedDeath() && ((i < 2u) || pSquad2->GetEnemy(i ^ 0x01u)->HasStatus(ENEMY_STATUS_DEAD)))
            {
                cEnemy* pParent = pSquad1->GetEnemy((i < 2u) ? i : ((i+2u) / 2u));
                pParent->Kill(true);
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) {STAGE_RESURRECT(pSquad1, 0u, 1u) STAGE_RESURRECT(pSquad2, 0u, 1u)}
            else if(STAGE_SUB(2u)) {STAGE_RESURRECT(pSquad1, 2u, 3u) STAGE_RESURRECT(pSquad2, 2u, 5u)}
            else if(STAGE_SUB(3u)) {STAGE_RESURRECT(pSquad1, 4u, 5u) STAGE_RESURRECT(pSquad2, 6u, 9u)}
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            const coreVector2 vShootDir = pEnemy->GetDirection().xy();

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            if(STAGE_BRANCH(pPath1->GetTotalDistance(), pPath2->GetTotalDistance()))
            {
                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if((i)    % 2u) pEnemy->Rotate180();
            if((i/2u) % 2u) pEnemy->Rotate90();

            const coreFloat fTurn = coreVector2::Dot(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), vShootDir.Rotated90());

            pEnemy->SetDirection(coreVector3(STAGE_TAKEOFF ? -pEnemy->GetDirection().xy() : coreVector2::Direction(vShootDir.Angle() + MIN(ABS(fTurn) * 5.0f, 1.0f) * SIGN(fTurn) * -1.2f * TIME), 0.0f));

            if(STAGE_TICK_LIFETIME_BASE(25.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if((i < 6u) || !aiFreeState[i-6u])
            {
                const cEnemy*     pParent    = pSquad1->GetEnemy((i < 2u) ? i : ((i+2u) / 2u));
                const coreVector2 vParentDir = ((pEnemy->GetPosition().xy() + pParent->GetMove()) - pParent->GetPosition().xy()).Normalized();

                const coreFloat fAngleFrom = vParentDir.Angle();
                const coreFloat fAngleTo   = ((i == 3u || i == 5u || i == 7u || i == 9u) ? (afTarget[i-1u] + (0.5f*PI)) : afTarget[i]);
                const coreFloat fShift     = ((i == 3u || i == 5u || i == 7u || i == 9u) ? (afSign  [i-1u])             : afSign  [i]) * ABS(AngleDiff(fAngleTo, fAngleFrom));

                const coreVector2 vDir = coreVector2::Direction(STAGE_TAKEOFF ? fAngleTo : (fAngleFrom + AngleDiff(fAngleTo + fShift, fAngleFrom) * 2.5f * TIME));
                const coreVector2 vPos = pParent->GetPosition().xy() + vDir * 13.0f;

                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
            else
            {
                coreVector2&      vDir = aiFreeMove[i-6u];
                const coreVector2 vPos = pEnemy->GetPosition().xy() + vDir * (40.0f * TIME);

                     if((vPos.x < -FOREGROUND_AREA.x) && (vDir.x < 0.0f)) vDir.x =  ABS(vDir.x);   // TODO 1: bounce correction
                else if((vPos.x >  FOREGROUND_AREA.x) && (vDir.x > 0.0f)) vDir.x = -ABS(vDir.x);
                     if((vPos.y < -FOREGROUND_AREA.y) && (vDir.y < 0.0f)) vDir.y =  ABS(vDir.y);
                else if((vPos.y >  FOREGROUND_AREA.y) && (vDir.y > 0.0f)) vDir.y = -ABS(vDir.y);

                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
        });

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_VAL(pSquad1), COLL_VAL(pSquad2), COLL_VAL(afTarget), COLL_VAL(afSign), COLL_VAL(aiFreeState), COLL_VAL(aiFreeMove))
        {
            if(!bFirstHit || (pEnemy->GetID() != cCinderEnemy::ID)) return;

            const coreUintW i          = pSquad2->GetIndex(pEnemy);
            const coreFloat fNewTarget = AlongCrossNormal(pBullet->GetFlyDir()).Angle() + CORE_MATH_PRECISION;

            if(i < 6u)
            {
                if(i == 3u || i == 5u) {afTarget[i-1u] = fNewTarget - (0.5f*PI); afSign[i-1u] =  0.1f;}
                                  else {afTarget[i]    = fNewTarget;             afSign[i]    = -0.1f;}
            }
            else if(!aiFreeState[i-6u])
            {
                const cEnemy*     pParent    = pSquad1->GetEnemy((i < 2u) ? i : ((i+2u) / 2u));
                const coreVector2 vParentDir = (pEnemy->GetPosition().xy() - pParent->GetPosition().xy()).Normalized();

                const coreFloat fAngleFrom = vParentDir.Angle();
                const coreFloat fAngleTo   = fNewTarget;
                const coreFloat fShift     = ((i == 7u || i == 9u) ? 0.1f : -0.1f) * ABS(AngleDiff(fAngleTo, fAngleFrom));

                aiFreeState[i-6u] = 1u;
                aiFreeMove [i-6u] = (AngleDiff(fAngleTo + fShift, fAngleFrom) < 0.0f) ? vParentDir.Rotated135() : -vParentDir.Rotated45();
            }
        });

        STAGE_WAVE("DREI", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // explode into bullets on death
    // - all: number of bullet waves is uneven, to create a better pattern visually
    // - all: number of enemies is uneven, to let them cross the center alternately, but still in equal distance
    // TODO 1: slower with size
    // TODO 1: huge enemies can shrink (and become faster)
    // TODO 1: player gets bigger and smaller when hitting a certain enemy, should not affect collision or gameplay, but movement speed? or gets bigger over time, and gets smaller again by spinning and shooting around
    // TODO 1: shrinking enemies have different attack (single dense wave)
    // TODO 1: multiple enemies are attached to each other and will grow independently
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 90u)                       
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.1f, (1.0f/9.0f) * I_TO_F(i))

            STAGE_REPEAT(0.5f)

            if(pEnemy->GetCurHealth())
            {
                pEnemy->SetPosition(coreVector3(0.75f * SIN((fLifeTime * 4.0f*PI + (1.0f/9.0f) * I_TO_F(i) * 2.0f*PI)), 1.4f * (1.0f - 4.0f * fLifeTime), 0.0f) * FOREGROUND_AREA3);
                pEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * LERP(5.0f, 1.2f, pEnemy->GetCurHealthPct()));
            }
            else
            {
                if(pEnemy->ReachedDeath())
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);

                    const coreVector2 vPos = pEnemy->GetPosition().xy();

                    for(coreUintW j = 8u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 22.5f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    }

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                }

                const coreFloat fNewSize = MAX0(pEnemy->GetSize().x - 4.0f * TIME);

                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * fNewSize);

                if(!fNewSize)
                {
                    pEnemy->Kill(false);
                }

                d_cast<cStarEnemy*>(pEnemy)->SetAngle(d_cast<cStarEnemy*>(pEnemy)->GetAngle() - 9.0f * TIME);
            }
            /*
            else
            {
                if(pEnemy->ReachedDeath())
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
                    pEnemy->AddStatus(ENEMY_STATUS_GHOST);
                }

                const coreFloat fNewSize = pEnemy->GetSize().x - 4.0f * TIME;
                const coreUint8 iOldTick = F_TO_UI(pEnemy->GetSize().x);
                const coreUint8 iNewTick = F_TO_UI(fNewSize);

                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * fNewSize);

                if(iOldTick != iNewTick)
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();

                    for(coreUintW j = 4u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) + 0.5f * I_TO_F(iNewTick % 2u)) * 45.0f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    }
                }

                if(iNewTick == 0u)
                {
                    pEnemy->Kill(false);
                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                }

                d_cast<cStarEnemy*>(pEnemy)->SetAngle(d_cast<cStarEnemy*>(pEnemy)->GetAngle() - 9.0f * TIME);
            }
             */
        });

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            //pPlayer->SetScale(3.0f);
        });

        STAGE_WAVE("VIER", {20.0f, 30.0f, 40.0f, 50.0f})
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
    // enemies jump into camera
    // - all: target shadow is essential, to communicate direct targeting in third sub-stage
    // - 1,2: bullet patterns provide a lot of safe space, as jumping into the camera may overwhelm the player
    // on circle group, enemies should not jump 3 or 5, so not everyone can be killed form the center
    // on 2x2 enemies, they may jump into position from previous enemies, but that should be fine
    // hopping enemies always need a 50% shift partner, so the gameplay keeps on moving (which makes those groups look faster, even though they have frequent pauses due to hopping)
    // big enemy stomp sollte keine löcher haben, damit man drüber springen muss
    // big enemy stomp can also be circumvented by staying near him, and compensate rotation or jumping over him for extra damage
    // circle group jumps into opposite circle-direction of 2x2 and chess group
    // TODO 1: check for and fix shadow artifacts, when jumping behind near clipping plane of shadow viewport (maybe fade out near plane)
    // TODO 1: create impact-wave when landing (especially the bigger), distortion
    // TODO 1: create effect for player jump takeof and landing, and maybe also with different colored shadow
    // TODO 1: enemies need to be rendered below player bullets (at least the big one)
    // TODO 1: big sollte extra box haben die den schaden weiterleitet (und bullet zerstört! damit nicht doppelt zählt), statt volume-change
    // TODO 1: hard mode: multiple jumps
    // TODO 1: badge: jump over big enemy
    // TODO 1: helper gets stomped out of side on big enemy
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        constexpr coreUintW iNumData  = 12u;
        constexpr coreUintW iBigIndex = 62u;

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 63u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                if(i == iBigIndex)
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.8f);
                    pEnemy->Configure(120, COLOR_SHIP_ORANGE);
                }
                else
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                    pEnemy->Configure(10, COLOR_SHIP_ORANGE);
                }
            });
        });

        STAGE_GET_START(9u * iNumData + 1u)
            STAGE_GET_VEC2_ARRAY (avOldPos,   iNumData)
            STAGE_GET_VEC2_ARRAY (avNewPos,   iNumData)
            STAGE_GET_FLOAT_ARRAY(avOldAngle, iNumData)
            STAGE_GET_FLOAT_ARRAY(avNewAngle, iNumData)
            STAGE_GET_FLOAT_ARRAY(afJump,     iNumData)
            STAGE_GET_FLOAT_ARRAY(afWait,     iNumData)
            STAGE_GET_UINT_ARRAY (aiCount,    iNumData)
            STAGE_GET_FLOAT      (fPlayerJump)
        STAGE_GET_END

        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
        {
            if(m_apShadowOwner[i] && m_apShadowOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableShadow(i, true);
        }

        if(STAGE_CLEARED)
        {
            const auto nInitFunc = [&](const coreUintW iIndex, const coreVector2 vPos, const coreVector2 vDir)
            {
                pSquad1->GetEnemy(iIndex)->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pSquad1->GetEnemy(iIndex)->SetDirection(coreVector3(vDir,                   0.0f));
            };

            if(STAGE_SUB(1u))
            {
                STAGE_RESURRECT(pSquad1, 0u, 3u)
                nInitFunc( 0u, coreVector2( 0.6f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc( 1u, coreVector2( 0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc( 2u, coreVector2(-0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc( 3u, coreVector2(-0.6f, 1.3f), coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 4u, 11u)
                nInitFunc( 4u, coreVector2( 1.3f, 0.6f), coreVector2(-1.0f, 0.0f));
                nInitFunc( 5u, coreVector2( 1.3f, 0.2f), coreVector2(-1.0f, 0.0f));
                nInitFunc( 6u, coreVector2( 1.3f,-0.2f), coreVector2(-1.0f, 0.0f));
                nInitFunc( 7u, coreVector2( 1.3f,-0.6f), coreVector2(-1.0f, 0.0f));
                nInitFunc( 8u, coreVector2(-1.3f,-0.6f), coreVector2( 1.0f, 0.0f));
                nInitFunc( 9u, coreVector2(-1.3f,-0.2f), coreVector2( 1.0f, 0.0f));
                nInitFunc(10u, coreVector2(-1.3f, 0.2f), coreVector2( 1.0f, 0.0f));
                nInitFunc(11u, coreVector2(-1.3f, 0.6f), coreVector2( 1.0f, 0.0f));
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 12u, 19u)
                nInitFunc(12u, coreVector2( 0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(13u, coreVector2(-0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(14u, coreVector2( 0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(15u, coreVector2(-0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(16u, coreVector2( 0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(17u, coreVector2(-0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(18u, coreVector2( 0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(19u, coreVector2(-0.2f, 1.3f), coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(4u))
            {
                const auto nPosFunc = [](const coreUintW iIndex)
                {
                    return StepRotated45(((iIndex / 4u) * 3u + 6u) % 8u).Processed(SIGNUM) * 1.3f;
                };

                STAGE_RESURRECT(pSquad1, 20u, 31u)
                nInitFunc(20u, nPosFunc(20u), -nPosFunc(20u).Normalized());
                nInitFunc(21u, nPosFunc(21u), -nPosFunc(21u).Normalized());
                nInitFunc(22u, nPosFunc(22u), -nPosFunc(22u).Normalized());
                nInitFunc(23u, nPosFunc(23u), -nPosFunc(23u).Normalized());
                nInitFunc(24u, nPosFunc(24u), -nPosFunc(24u).Normalized());
                nInitFunc(25u, nPosFunc(25u), -nPosFunc(25u).Normalized());
                nInitFunc(26u, nPosFunc(26u), -nPosFunc(26u).Normalized());
                nInitFunc(27u, nPosFunc(27u), -nPosFunc(27u).Normalized());
                nInitFunc(28u, nPosFunc(28u), -nPosFunc(28u).Normalized());
                nInitFunc(29u, nPosFunc(29u), -nPosFunc(29u).Normalized());
                nInitFunc(30u, nPosFunc(30u), -nPosFunc(30u).Normalized());
                nInitFunc(31u, nPosFunc(31u), -nPosFunc(31u).Normalized());
            }
            else if(STAGE_SUB(5u))
            {
                const auto nPosFunc = [](const coreUintW iIndex)
                {
                    return coreVector2::Direction(I_TO_F(((iIndex - 32u) + 2u) % 8u) * (0.25f*PI)) * 1.3f * SQRT2;
                };

                STAGE_RESURRECT(pSquad1, 32u, 39u)
                nInitFunc(32u, nPosFunc(32u), -nPosFunc(32u).Normalized());
                nInitFunc(33u, nPosFunc(33u), -nPosFunc(33u).Normalized());
                nInitFunc(34u, nPosFunc(34u), -nPosFunc(34u).Normalized());
                nInitFunc(35u, nPosFunc(35u), -nPosFunc(35u).Normalized());
                nInitFunc(36u, nPosFunc(36u), -nPosFunc(36u).Normalized());
                nInitFunc(37u, nPosFunc(37u), -nPosFunc(37u).Normalized());
                nInitFunc(38u, nPosFunc(38u), -nPosFunc(38u).Normalized());
                nInitFunc(39u, nPosFunc(39u), -nPosFunc(39u).Normalized());
            }
            else if(STAGE_SUB(6u))
            {
                STAGE_RESURRECT(pSquad1, 40u, 47u)
                nInitFunc(40u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(41u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(42u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(43u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(44u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
                nInitFunc(45u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
                nInitFunc(46u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
                nInitFunc(47u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 48u, 53u)
                nInitFunc(48u, coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
                nInitFunc(49u, coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
                nInitFunc(50u, coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
                nInitFunc(51u, coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
                nInitFunc(52u, coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
                nInitFunc(53u, coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 54u, 61u)
                nInitFunc(54u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(55u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(56u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(57u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(58u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(59u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(60u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(61u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 62u, 62u)
                nInitFunc(62u, coreVector2( 0.0f, 1.5f), coreVector2( 0.0f,-1.0f));
            }

            for(coreUintW i = 0u; i < iNumData; ++i) afJump [i] = 0.0f;
            for(coreUintW i = 0u; i < iNumData; ++i) afWait [i] = 0.5f;
            for(coreUintW i = 0u; i < iNumData; ++i) aiCount[i] = 0u;
        }

        constexpr coreFloat fWaitSpeed    = 0.9f;
        constexpr coreFloat fJumpSpeed    = 0.7f;
        constexpr coreFloat fDelay        = (1.0f/fWaitSpeed) + (1.0f/fJumpSpeed);

        constexpr coreFloat fWaitSpeedHop = 2.7f;
        constexpr coreFloat fJumpSpeedHop = 2.1f;
        constexpr coreFloat fDelayHop     = (1.0f/fWaitSpeedHop) + (1.0f/fJumpSpeedHop);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 12u) ? (0.2f * I_TO_F(i % 4u)) : ((i < 20u) ? (fDelay * 0.125f * I_TO_F(i % 8u)) : ((i < 48u) ? 0.0f : ((i < 62u) ? (fDelayHop * 0.5f * I_TO_F((i - 48u) % 2u)) : 0.0f))))

            const coreUintW iEntry = i % iNumData;

            coreVector2& vOldPos   = avOldPos  [iEntry];
            coreVector2& vNewPos   = avNewPos  [iEntry];
            coreFloat&   vOldAngle = avOldAngle[iEntry];
            coreFloat&   vNewAngle = avNewAngle[iEntry];
            coreFloat&   fJump     = afJump    [iEntry];
            coreFloat&   fWait     = afWait    [iEntry];
            coreUint32&  iCount    = aiCount   [iEntry];

            if(STAGE_TAKEOFF)
            {
                vOldPos   = pEnemy->GetPosition ().xy();
                vOldAngle = pEnemy->GetDirection().xy().Angle();
            }

            
            if(i == iBigIndex)
            {
                const coreBool bPillar = ((fWait < 1.0f) && !pEnemy->ReachedDeath());

                pEnemy->DefineVolume        (bPillar ? pEnemy->GetModelHigh() : NULL);
                pEnemy->SetCollisionModifier(coreVector3(1.0f, 1.0f, bPillar ? 10.0f : 1.0f));
            }
            

            const coreBool bHop = (i >= 48u && i < 62u);

            if(STAGE_LIFETIME_AFTER(0.0f))
            {
                if(fWait < 1.0f)
                {
                    fWait += (bHop ? fWaitSpeedHop : fWaitSpeed) * TIME;
                    if(fWait >= 1.0f)
                    {
                        fJump = 0.0f;
                        fWait = 1.0f;

                        vOldPos   = pEnemy->GetPosition ().xy();
                        vOldAngle = pEnemy->GetDirection().xy().Angle();

                        if(i < 4u)
                        {
                            vNewPos   = coreVector2((1.5f - I_TO_F(i % 4u)) * 0.45f, 0.8f) * FOREGROUND_AREA * ((iCount % 2u) ? 1.0f : -1.0f);
                            vNewAngle = vOldAngle + 1.0f*PI;
                        }
                        else if(i < 12u)
                        {
                            vNewPos   = coreVector2(0.8f, (1.5f - I_TO_F(i % 4u)) * 0.45f) * FOREGROUND_AREA * ((iCount % 2u) ? 1.0f : -1.0f) * ((i >= 8u) ? -1.0f : 1.0f);
                            vNewAngle = vOldAngle + 1.0f*PI;
                        }
                        else if(i < 20u || i == iBigIndex)
                        {
                            vNewPos   = pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy();
                            vNewAngle = vOldAngle + 2.0f*PI;
                        }
                        else if(i < 32u)
                        {
                            const coreUintW iIndex = i - 20u;

                            const coreFloat x = I_TO_F((iIndex)      % 2u) - 0.5f;
                            const coreFloat y = I_TO_F((iIndex / 2u) % 2u) - 0.5f;

                            const coreVector2 vTarget = StepRotated45(((iIndex / 4u) * 3u + (iCount * 3u)) % 8u).Processed(SIGNUM);

                            vNewPos   = (coreVector2(x, y) * 0.3f + vTarget * 0.7f) * FOREGROUND_AREA;
                            vNewAngle = vOldAngle - 1.25f*PI;
                        }
                        else if(i < 40u)
                        {
                            vNewPos   = coreVector2::Direction(I_TO_F(((i - 32u) + (iCount * 6u)) % 8u) * (0.25f*PI)) * 0.9f * FOREGROUND_AREA;
                            vNewAngle = vOldAngle + 1.25f*PI;
                        }
                        else if(i < 48u)
                        {
                            const coreUintW iIndex = i - 40u;

                            const coreFloat x = I_TO_F((iIndex)      % 2u) - 0.5f;
                            const coreFloat y = I_TO_F((iIndex / 2u) % 2u) - 0.5f;

                            const coreVector2 vTarget = StepRotated90X(iCount % 4u) * (1.0f/SQRT2);

                            vNewPos   = (coreVector2(x, y) + vTarget * 0.5f) * FOREGROUND_AREA * ((i < 44u) ? 1.0f : -1.0f);
                            vNewAngle = vOldAngle + 1.0f*PI;
                        }
                        else if(i < 54u)
                        {
                            const coreFloat fSign = ((iCount % 20u) < 10u) ? 1.0f : -1.0f;

                            vNewPos   = coreVector2((I_TO_F(iCount % 10u) - 4.5f) * 0.22f * (((i - 48u) % 2u) ? -1.0f : 1.0f) * fSign, (I_TO_F(i - 48u) - 2.5f) * 0.35f + ((iCount % 2u) ? -0.1f : 0.1f)) * FOREGROUND_AREA;
                            vNewAngle = vOldAngle;
                        }
                        else if(i < 62u)
                        {
                            const coreFloat fSign = ((iCount % 20u) < 10u) ? 1.0f : -1.0f;

                            vNewPos   = coreVector2((I_TO_F((i - 54u) % 8u) - 3.5f) * 0.25f, LERP(0.95f, -0.95f, (I_TO_F(iCount % 10u) + (((i - 54u) % 2u) ? 0.25f : -0.25f)) / 9.0f) * fSign) * FOREGROUND_AREA;
                            vNewAngle = vOldAngle;
                        }
                        else ASSERT(false)

                        if(!pEnemy->ReachedDeath()) this->EnableShadow(i % VIRIDO_SHADOWS, pEnemy, vNewPos);

                        iCount += 1u;
                    }
                }
                else
                {
                    fJump += (bHop ? fJumpSpeedHop : fJumpSpeed) * TIME;
                    if(fJump >= 1.0f)
                    {
                        fJump = 1.0f;
                        fWait = 0.0f;

                        const coreVector2 vPos = pEnemy->GetPosition().xy();

                        if(i == iBigIndex)
                        {
                            for(coreUintW j = 80u; j--; )
                            {
                                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f));

                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos,  vDir)            ->ChangeSize(1.4f);
                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos,  vDir.Rotated90())->ChangeSize(1.4f);
                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos, -vDir)            ->ChangeSize(1.4f);
                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos, -vDir.Rotated90())->ChangeSize(1.4f);
                            }

                            fPlayerJump = 1.0f;
                            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                        }
                        else
                        {
                            if(bHop)
                            {
                                const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();
                                const coreVector2 vTan = vDir.Rotated90() * 3.0f;

                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.4f);
                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos,        vDir)->ChangeSize(1.4f);
                                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.4f);
                            }
                            else
                            {
                                for(coreUintW j = 4u; j--; )
                                {
                                    const coreVector2 vDir = StepRotated90(j);
                                    const coreVector2 vTan = vDir.Rotated90() * 3.0f;

                                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.4f);
                                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos,        vDir)->ChangeSize(1.4f);
                                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.4f);
                                }
                            }
                        }

                        this->DisableShadow(i % VIRIDO_SHADOWS, false);
                    }
                }

                const coreFloat   fSin    = SIN(fJump * (1.0f*PI));
                const coreVector2 vPos    = LERP(vOldPos, vNewPos, fJump) * (1.0f - 0.45f * fSin);
                const coreFloat   fHeight = 50.0f * fSin;

                pEnemy->SetPosition      (coreVector3(vPos, fHeight));
                pEnemy->DefaultRotateLerp(vOldAngle, vNewAngle, fJump);

                if(!bHop) pEnemy->DefaultOrientateLerp(0.0f*PI, 2.0f*PI, fJump);
            }
        });

        if(fPlayerJump)
        {
            fPlayerJump = MAX0(fPlayerJump - 1.0f * TIME);

            const coreFloat   fHeight = 30.0f * SIN(fPlayerJump * (1.0f*PI));
            const coreMatrix2 mRota   = coreMatrix3::Rotation(TIME * (3.0f*PI)).m12();

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetPosition (coreVector3(pPlayer->GetPosition ().xy(), fHeight));
                pPlayer->SetDirection(coreVector3(pPlayer->GetDirection().xy() * mRota, pPlayer->GetDirection().z));

                if(fPlayerJump)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_GHOST);
                }
                else
                {
                    pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), pPlayer->GetDirection().z));
                    pPlayer->RemoveStatus(PLAYER_STATUS_GHOST);
                }
            });
        }

        STAGE_WAVE("FÜNF", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all shadows
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
            this->DisableShadow(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_GHOST);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // enemy charges straight forward on death (r-type fish)
    // - (old) 2: should start on same spot and opposite of first group, to reduce accidental collision (which is more likely with 2x2)
    // [deprecated] no passive attack, not impact attack, no bounce, because it's too overwhelming together with the green ball
    // fire speed fast enough to create visual line, but not too fast, to create holes on sharp angles (which have less predictable path)
    // [deprecated] 2x2 instead of 1x4 (enemy block instead of line), to force player keeping fire at the same position while an enemy is already flying at them
    // both circle waves should start from left, and follow the same flip-movement and rotation
    // small circle waves look like fireworks, big circle wave looks like a water-wheel
    // angle and number of bounces of bounce enemy needs to be comprehensible, and the player should end near the bottom
    // directions on fixed-direction moving enemies needs to give some holes for players to fly through, directions only facing into middle did not work
    // spawn-fokus ist oben, damit der spieler unten einen sicheren platz hat
    // TODO 1: give central effect to highlight damaging touch
    // TODO 1: N gegner schiffe bleiben im rand stecken, können für extra punkte abgeschossen werden, oder badge
    // TODO 1: enemies which charge infinitely (N-times)
    // TODO 1: hard-mode: every enemy bounces once (what about bouncy enemy?), oder 1 infinity move (and bouncy enemy just moves N times infinitely)
    // TODO 1: letzte 3 gruppen (player-aim) bisschen variabler von positionierung machen (e.g. (infinity) side-movement ?)
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        constexpr coreUintW iNumData     = 16u;
        constexpr coreUintW iBounceIndex = 82u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f * SQRT2), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f),         coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-0.4f, 1.5f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 0.4f,-1.5f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 83u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * ((i == iBounceIndex) ? 2.5f : 1.5f));
                pEnemy->Configure((i == iBounceIndex) ? 50 : 10, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(iNumData + 2u)
            STAGE_GET_FLOAT_ARRAY(afSpeed, iNumData)
            STAGE_GET_FLOAT      (fAngle)
            STAGE_GET_UINT       (iBounce)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(pEnemy->GetCurHealth() == 0)
            {
                if((i == iBounceIndex) && (iBounce < 8u))
                {
                    coreVector2 vPos = pEnemy->GetPosition ().xy();
                    coreVector2 vDir = pEnemy->GetDirection().xy();

                         if((vPos.x < -FOREGROUND_AREA.x * 1.1f) && (vDir.x < 0.0f)) {vPos.x -= 2.0f * (vPos.x + FOREGROUND_AREA.x * 1.1f); vDir.x =  ABS(vDir.x); iBounce += 1u;}
                    else if((vPos.x >  FOREGROUND_AREA.x * 1.1f) && (vDir.x > 0.0f)) {vPos.x -= 2.0f * (vPos.x - FOREGROUND_AREA.x * 1.1f); vDir.x = -ABS(vDir.x); iBounce += 1u;}
                         if((vPos.y < -FOREGROUND_AREA.y * 1.1f) && (vDir.y < 0.0f)) {vPos.y -= 2.0f * (vPos.y + FOREGROUND_AREA.y * 1.1f); vDir.y =  ABS(vDir.y); iBounce += 1u;}
                    else if((vPos.y >  FOREGROUND_AREA.y * 1.1f) && (vDir.y > 0.0f)) {vPos.y -= 2.0f * (vPos.y - FOREGROUND_AREA.y * 1.1f); vDir.y = -ABS(vDir.y); iBounce += 1u;}

                    pEnemy->SetPosition (coreVector3(vPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir, 0.0f));
                }
                else if(!g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()))
                {
                    pEnemy->Kill(true);
                }
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 32u, 36u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 37u, 45u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 46u, 61u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 62u, 73u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 74u, 81u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 82u, 82u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(9u)) STAGE_RESURRECT(pSquad1, 16u, 31u)

            std::memset(afSpeed, 0, sizeof(coreFloat) * iNumData);
        }

             if(m_iStageSub == 8u) fAngle += 0.25f*PI * TIME;
        else if(m_iStageSub == 9u) fAngle -= 0.25f*PI * TIME;
        const coreMatrix2 mRota = coreMatrix3::Rotation(fAngle).m12();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 46u || i == iBounceIndex) ? 0.85f : 0.6f, (i < 32u) ? (0.3f * I_TO_F(i % 2u)) : ((i < 46u) ? 0.0f : ((i < 62u) ? (0.5f + 0.165f * I_TO_F(i - 46u)) : ((i < 74u) ? (0.5f + 1.1f * I_TO_F((i - 62u) / 4u)) : ((i < 82u) ? 0.5f : 0.0f)))))

            if(pEnemy->ReachedDeath())
            {
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_GHOST);
            }

            if(pEnemy->GetCurHealth() == 0)
            {
                coreFloat& fSpeed = afSpeed[i % iNumData];

                fSpeed += ((i == iBounceIndex) ? 20.0f : 80.0f) * TIME;

                pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * ((20.0f + fSpeed) * TIME));

                if(STAGE_TICK_TIME(30.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (i >= 16u && i < 32u) ? 0.0f : 0.5f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                }
            }
            else
            {
                if(i < 8u)
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f + (((i % 4u) < 2u) ? -1.0f : 1.0f)) * 0.22f, ((i % 8u) < 4u) ? 0.0f : 0.22f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection   (coreVector3(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), 0.0f));
                }
                else if(i < 32u)
                {
                    const coreVector2 vFactor = coreVector2(1.0f, ((i % 8u) < 4u) ? 1.0f : -1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F(i % 2u) - 0.5f) * 0.22f, ((i % 4u) < 2u) ? 0.0f : 0.22f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                    if(i >= 24u) pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy().Rotated90() * mRota, 0.0f));
                            else pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy()             * mRota, 0.0f));

                    pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), 0.0f));
                }
                else if(i < 37u)
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F(i - 32u) - 2.0f) * 0.3f, 0.22f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection   (coreVector3(coreVector2::Direction((I_TO_F(i - 32u) - 2.0f) * (0.125f*PI) + (1.0f*PI)), 0.0f));
                }
                else if(i < 46u)
                {
                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F((i - 37u) % 3u) - 1.0f) * 0.3f, 0.22f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection   (coreVector3(0.0f,-1.0f,0.0f));

                         if(i < 40u) {}
                    else if(i < 43u) pEnemy->Rotate90 ();
                    else if(i < 46u) pEnemy->Rotate270();
                }
                else if(i < 62u)
                {
                    STAGE_REPEAT(pPath3->GetTotalDistance())

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2((I_TO_F((i - 46u) % 4u) - 1.5f) * 0.5f, 0.0f);

                    pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection   (coreVector3(StepRotated90X(((i - 46u) + (i - 46u) / 4u) % 4u), 0.0f));
                }
                else if(i < 74u)
                {
                    STAGE_REPEAT(pPath2->GetTotalDistance())

                    const coreVector2 vDir = MapToAxis(StepRotated90(i % 4u), coreVector2::Direction(fLifeTime * 4.0f));

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(0.0f,0.0f) + vDir * 0.16f;

                    pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection   (coreVector3(vDir, 0.0f));

                         if(i < 66u) {}
                    else if(i < 70u) pEnemy->InvertX();
                    else if(i < 74u) {}

                    if(F_TO_UI(fLifeTimeBase * RCP(pPath2->GetTotalDistance())) % 2u) pEnemy->InvertX();
                }
                else if(i < 82u)
                {
                    STAGE_REPEAT(pPath2->GetTotalDistance())

                    const coreVector2 vDir = MapToAxis(StepRotated45(i % 8u), coreVector2::Direction(fLifeTime * -4.0f));

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(0.0f,0.0f) + vDir * 0.3f;

                    pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection   (coreVector3(-vDir, 0.0f));

                    if(F_TO_UI(fLifeTimeBase * RCP(pPath2->GetTotalDistance())) % 2u) pEnemy->InvertX();
                }
                else if(i < 83u)
                {
                    ASSERT(i == iBounceIndex)

                    const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                    const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                    pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                    pEnemy->SetDirection   (coreVector3(1.0f,-3.0f,0.0f).Normalized());
                }
            }
        });

        STAGE_WAVE("SECHS", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 6u})
    {
        STAGE_BOSS(m_Torus, {60.0f, 120.0f, 180.0, 240.0f})
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