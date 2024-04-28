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
        g_pEnvironment->ChangeBackground(cGrassBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

        g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
        g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
        g_pEnvironment->SetTargetSpeedNow    (6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
    {
        cGrassBackground* pBackground = d_cast<cGrassBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetCoverAlpha(1.0f);

        pBackground->SetGroundDensity(0u, 0.0f);
        pBackground->SetGroundDensity(1u, 0.0f);
        pBackground->SetGroundDensity(2u, 0.0f);
        pBackground->SetGroundDensity(3u, 0.0f);
        pBackground->SetDecalDensity (0u, 0.0f);
        pBackground->SetAirDensity   (2u, 1.0f);

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
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // shields on sides to force attacks from certain directions
    // - player position after killing enemies (based on their shield position) needs to be considered when spawning next wave
    // - first long shields need to push 50% of the area, to remove safe-spots
    // - long side-moving pattern needs to start at corners, one up one down, to prevent easy corner killing
    // - barrier indices are all manual, to control render order
    // TASK: destroy all marked shields
    // TASK: kill enemy with reflected bullet
    // ACHIEVEMENT: reflect a single bullet at least 20 times
    // TODO 1: hard mode: reflected bullets cause damage
    // TODO 1: drum shield needs blink!
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreFloat fWidth       = 0.38f;
        constexpr coreUintW iReflectSize = 128u;
        constexpr coreUintW iReflectWrap = iReflectSize / 2u * sizeof(coreUint32);

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
            pPath2->AddNode(coreVector2(0.0f, 1.5f + fWidth * 1.0f), coreVector2(0.0f,-1.0f));
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

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 52u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.2f);
                pEnemy->Configure(10, COLOR_SHIP_CYAN);
            });
        });

        STAGE_GET_START(3u + iReflectSize)
            STAGE_GET_VEC2      (vHelperMove)
            STAGE_GET_UINT      (iDrumNum)
            STAGE_GET_UINT_ARRAY(aiReflect, iReflectSize)
        STAGE_GET_END

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
                this->EnableBarrier( 0u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy( 0u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy( 1u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy( 1u), coreVector2( 0.0f,-1.0f), 1.0f);
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 2u, 7u)
                this->EnableBarrier( 4u, pSquad1->GetEnemy( 2u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 5u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy( 5u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 6u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy( 7u), coreVector2( 0.0f, 1.0f), 1.0f);

                if(g_pGame->IsTask()) this->StartDrumBeat(0u, 9u);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 8u, 13u)
                this->EnableBarrier(10u, pSquad1->GetEnemy( 8u), coreVector2( 1.0f, 0.0f).Rotated90(), 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy( 8u), coreVector2(-1.0f, 0.0f).Rotated90(), 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy( 9u), coreVector2( 1.0f, 0.0f),             1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy( 9u), coreVector2(-1.0f, 0.0f),             1.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy(10u), coreVector2( 1.0f, 0.0f).Rotated90(), 1.0f);
                this->EnableBarrier(15u, pSquad1->GetEnemy(10u), coreVector2(-1.0f, 0.0f).Rotated90(), 1.0f);
                this->EnableBarrier(16u, pSquad1->GetEnemy(11u), coreVector2( 1.0f, 0.0f).Rotated90(), 1.0f);
                this->EnableBarrier(17u, pSquad1->GetEnemy(11u), coreVector2(-1.0f, 0.0f).Rotated90(), 1.0f);
                this->EnableBarrier(18u, pSquad1->GetEnemy(12u), coreVector2( 1.0f, 0.0f),             1.0f);
                this->EnableBarrier(19u, pSquad1->GetEnemy(12u), coreVector2(-1.0f, 0.0f),             1.0f);
                this->EnableBarrier(20u, pSquad1->GetEnemy(13u), coreVector2( 1.0f, 0.0f).Rotated90(), 1.0f);
                this->EnableBarrier(21u, pSquad1->GetEnemy(13u), coreVector2(-1.0f, 0.0f).Rotated90(), 1.0f);

                if(g_pGame->IsTask()) this->StartDrumBeat(1u, 16u);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 14u, 17u)
                this->EnableBarrier(24u, pSquad1->GetEnemy(14u), coreVector2( 0.0f, 1.0f), 3.0f);   // only first two on top
                this->EnableBarrier(25u, pSquad1->GetEnemy(15u), coreVector2( 1.0f, 0.0f), 3.0f);
                this->EnableBarrier( 0u, pSquad1->GetEnemy(16u), coreVector2( 0.0f,-1.0f), 3.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy(17u), coreVector2(-1.0f, 0.0f), 3.0f);
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESURRECT(pSquad1, 18u, 23u)
                this->EnableBarrier(16u, pSquad1->GetEnemy(18u), coreVector2( 0.0f, 1.0f), 1.0f);   // 0
                this->EnableBarrier(17u, pSquad1->GetEnemy(18u), coreVector2( 0.0f, 1.0f), 1.0f);   // 1
                this->EnableBarrier( 2u, pSquad1->GetEnemy(19u), coreVector2( 0.0f, 1.0f), 1.0f);   // 2   // below
                this->EnableBarrier( 3u, pSquad1->GetEnemy(19u), coreVector2( 0.0f, 1.0f), 1.0f);   // 3   // below
                this->EnableBarrier(18u, pSquad1->GetEnemy(20u), coreVector2( 0.0f, 1.0f), 1.0f);   // 0
                this->EnableBarrier(19u, pSquad1->GetEnemy(20u), coreVector2( 0.0f, 1.0f), 1.0f);   // 1
                this->EnableBarrier(20u, pSquad1->GetEnemy(21u), coreVector2( 0.0f, 1.0f), 1.0f);   // 2
                this->EnableBarrier(21u, pSquad1->GetEnemy(21u), coreVector2( 0.0f, 1.0f), 1.0f);   // 3
                this->EnableBarrier( 4u, pSquad1->GetEnemy(22u), coreVector2( 0.0f, 1.0f), 1.0f);   // 0   // below
                this->EnableBarrier( 5u, pSquad1->GetEnemy(22u), coreVector2( 0.0f, 1.0f), 1.0f);   // 1   // below
                this->EnableBarrier(22u, pSquad1->GetEnemy(23u), coreVector2( 0.0f, 1.0f), 1.0f);   // 2
                this->EnableBarrier(23u, pSquad1->GetEnemy(23u), coreVector2( 0.0f, 1.0f), 1.0f);   // 3

                if(g_pGame->IsTask()) this->StartDrumBeat(0u, 19u);
            }
            else if(STAGE_SUB(6u))
            {
                STAGE_RESURRECT(pSquad1, 24u, 27u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy(24u), coreVector2(-1.0f, 1.0f).Normalized(), 1.0f);   // top-left first
                this->EnableBarrier( 1u, pSquad1->GetEnemy(24u), coreVector2( 1.0f, 1.0f).Normalized(), 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy(25u), coreVector2(-1.0f, 1.0f).Normalized(), 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy(25u), coreVector2( 1.0f, 1.0f).Normalized(), 1.0f);
                this->EnableBarrier( 8u, pSquad1->GetEnemy(26u), coreVector2(-1.0f, 1.0f).Normalized(), 1.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy(26u), coreVector2( 1.0f, 1.0f).Normalized(), 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy(27u), coreVector2(-1.0f, 1.0f).Normalized(), 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy(27u), coreVector2( 1.0f, 1.0f).Normalized(), 1.0f);

                if(g_pGame->IsTask()) this->StartDrumBeat(1u, 6u);
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 28u, 31u)
                this->EnableBarrier(12u, pSquad1->GetEnemy(28u), coreVector2(-1.0f, 0.0f), 3.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy(28u), coreVector2( 1.0f, 0.0f), 3.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy(29u), coreVector2(-1.0f, 0.0f), 3.0f);
                this->EnableBarrier(15u, pSquad1->GetEnemy(29u), coreVector2( 1.0f, 0.0f), 3.0f);
                this->EnableBarrier(16u, pSquad1->GetEnemy(30u), coreVector2(-1.0f, 0.0f), 3.0f);
                this->EnableBarrier(17u, pSquad1->GetEnemy(30u), coreVector2( 1.0f, 0.0f), 3.0f);
                this->EnableBarrier(18u, pSquad1->GetEnemy(31u), coreVector2(-1.0f, 0.0f), 3.0f);
                this->EnableBarrier(19u, pSquad1->GetEnemy(31u), coreVector2( 1.0f, 0.0f), 3.0f);
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 32u, 35u)
                this->EnableBarrier( 3u, pSquad1->GetEnemy(32u), coreVector2( 0.0f, 1.0f), 1.0f);   // order
                this->EnableBarrier( 2u, pSquad1->GetEnemy(33u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy(34u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 0u, pSquad1->GetEnemy(35u), coreVector2( 0.0f, 1.0f), 1.0f);

                if(g_pGame->IsTask()) this->StartDrumBeat(0u, 0u);
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 36u, 39u)
                this->EnableBarrier( 6u, pSquad1->GetEnemy(36u), coreVector2( 0.0f, 1.0f), 4.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy(37u), coreVector2( 0.0f, 1.0f), 4.0f);
                this->EnableBarrier( 8u, pSquad1->GetEnemy(38u), coreVector2( 0.0f, 1.0f), 4.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy(39u), coreVector2( 0.0f, 1.0f), 4.0f);

                this->SetBarrierIgnore(false);
            }
            else if(STAGE_SUB(10u))
            {
                STAGE_RESURRECT(pSquad1, 40u, 51u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy(40u), coreVector2(-1.0f, 0.0f), 1.0f);   // horizontal <> vertical
                this->EnableBarrier(15u, pSquad1->GetEnemy(40u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(16u, pSquad1->GetEnemy(41u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy(42u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier(17u, pSquad1->GetEnemy(42u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy(43u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy(43u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier(18u, pSquad1->GetEnemy(43u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(19u, pSquad1->GetEnemy(44u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 4u, pSquad1->GetEnemy(45u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 5u, pSquad1->GetEnemy(46u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier(20u, pSquad1->GetEnemy(47u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy(48u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier(21u, pSquad1->GetEnemy(48u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy(49u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier(22u, pSquad1->GetEnemy(49u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy(50u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy(50u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier(23u, pSquad1->GetEnemy(50u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(24u, pSquad1->GetEnemy(50u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy(51u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier(25u, pSquad1->GetEnemy(51u), coreVector2( 0.0f, 1.0f), 1.0f);

                if(g_pGame->IsTask())
                {
                    this->StartDrumBeat(0u, 15u);
                    this->StartDrumBeat(1u, 20u);
                }
                else
                {
                    this->DisableBarrier(15u, false);
                    this->DisableBarrier(20u, false);
                }

                this->SetBarrierRange (1.3f);
                this->SetBarrierIgnore(true);
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_BLUE);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect(false);
            pHelper->SetPosition(coreVector3(-0.8f,1.8f,0.0f) * FOREGROUND_AREA3);

            vHelperMove = coreVector2(1.0f,-2.0f).Normalized() * 30.0f;
        }
        else if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vNewPos = pHelper->GetPosition().xy() + vHelperMove * TIME;

            if(((vNewPos.x < -FOREGROUND_AREA.x * 1.2f) && (vHelperMove.x < 0.0f)) ||
               ((vNewPos.x >  FOREGROUND_AREA.x * 1.2f) && (vHelperMove.x > 0.0f)) ||
               ((vNewPos.y < -FOREGROUND_AREA.y * 1.2f) && (vHelperMove.y < 0.0f)) ||
               ((vNewPos.y >  FOREGROUND_AREA.y * 1.2f) && (vHelperMove.y > 0.0f)))
            {
                if(vNewPos.y < -FOREGROUND_AREA.y * 1.2f)
                {
                    STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
                    {
                        this->AddExtraScore(pPlayer, 100u, coreVector3(vNewPos, 0.0f));
                    });
                }

                pHelper->Kill(false);
            }

            pHelper->SetPosition(coreVector3(vNewPos, 0.0f));

            for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            {
                const coreObject3D& oBarrier = m_aBarrierRaw[i];
                if(!oBarrier.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                coreVector3 vIntersection;
                if(Core::Manager::Object->TestCollision(pHelper, &oBarrier, &vIntersection))
                {
                    if(coreVector2::Dot(vHelperMove, oBarrier.GetDirection().xy()) < 0.0f)
                    {
                        g_pSpecialEffects->CreateSplashColor(vIntersection, 10.0f, 5u, COLOR_ENERGY_BLUE);
                    }

                    vHelperMove = coreVector2::Reflect(vHelperMove, oBarrier.GetDirection().xy());
                }
            }
        }

        if(m_iStageSub == 5u)
        {
            constexpr coreVector2 avBase[] =
            {
                coreVector2( 1.0f, 0.0f),
                coreVector2(-1.0f, 0.0f),
                coreVector2( 1.0f, 0.0f).Rotated90(),
                coreVector2(-1.0f, 0.0f).Rotated90()
            };

            for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            {
                coreObject3D& oBarrier = m_aBarrierRaw[i];
                if(!m_apBarrierOwner[i]) continue;

                const cEnemy*   pEnemy = d_cast<const cEnemy*>(m_apBarrierOwner[i]);
                const coreUintW iIndex = pSquad1->GetIndex(pEnemy);

                const coreFloat   fTime = pEnemy->GetLifeTime();
                const coreVector2 vRota = coreVector2::Direction(fTime * (0.6f*PI));
                const coreVector2 vDir  = MapToAxis(avBase[((iIndex % 2u) * 2u) + (i % 2u)], vRota);

                oBarrier.SetDirection(coreVector3(vDir, 0.0f));
            }
        }
        else if((m_iStageSub == 8u) || (m_iStageSub == 9u))
        {
            for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            {
                coreObject3D& oBarrier = m_aBarrierRaw[i];
                if(!m_apBarrierOwner[i]) continue;

                const coreVector2 vDir = m_apBarrierOwner[i]->GetPosition().xy().Normalized();

                oBarrier.SetDirection(coreVector3((m_iStageSub == 9u) ? vDir.Rotated90() : -vDir, 0.0f));
            }
        }
        else if(m_iStageSub == 10u)
        {
            if(pSquad1->GetNumEnemiesAlive() <= 1u)
            {
                for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
                    this->DisableBarrier(i, true);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 2u || i >= 40u) ? 1.2f : 0.6f, (i < 8u || i >= 32u) ? 0.0f : ((i < 18u) ? (0.4f * I_TO_F((i - 8u) % 6u)) : ((i < 24u) ? (0.5f * I_TO_F(i - 18u)) : ((i < 28u) ? (0.7f * I_TO_F(i - 24u)) : (0.7f * I_TO_F(i - 28u))))))

            if(i < 2u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i % 2u) ? 0.25f : -0.25f, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 8u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i - 2u) - 2.5f) * fWidth, 0.4f * SIN((fLifeTime * 0.6f + I_TO_F((i - 2u) / 2u)) * (1.0f*PI) + (1.0f*PI)));

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate180();
            }
            else if(i < 14u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 8u) * 2u + 2u) % 6u) - 2.5f) * 0.34f * ((i < 11u) ? 1.0f : -1.0f), 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayerSide().Normalized(), 0.0f));
            }
            else if(i < 18u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(-0.54f,0.0f);

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

                pEnemy->DefaultRotate(fLifeTime * -3.0f);
            }
            else if(i < 28u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,-1.0f);
                const coreVector2 vOffset = coreVector2((((i - 24u) % 2u) ? 0.4f : -0.4f) * SIN(m_fStageSubTime * (0.5f*PI)), 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 32u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((((i - 28u) % 2u) ? -1.0f : 1.0f) * SIN(m_fStageSubTime * (0.5f*PI) + (0.5f*PI)), 0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }
            else if(i < 40u)
            {
                const coreFloat   fSide = (i < 36u) ? 1.0f : -1.0f;
                const coreVector2 vDir  = coreVector2::Direction((fLifeTime * (g_pGame->IsEasy() ? 1.5f : 2.0f) - I_TO_F((i - 32u) % 4u) * ((i < 36u) ? (0.25f*PI) : (0.5f*PI))) * fSide);
                const coreFloat   fLen  = LERPB(1.85f, 0.6f, MIN1(fLifeTime * 0.5f)) * FOREGROUND_AREA.x;

                pEnemy->SetPosition (coreVector3(vDir * fLen,  0.0f));
                pEnemy->SetDirection(coreVector3(vDir * -1.0f, 0.0f));
            }
            else if(i < 52u)
            {
                const coreUintW x = (i - 40u) % 3u;
                const coreUintW y = (i - 40u) / 3u;

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(x) - 1.0f) * fWidth, (I_TO_F(y) - 1.5f) * fWidth);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                if(i == 50u) pEnemy->DefaultRotate(fLifeTime * 3.0f);
            }

            if(STAGE_LIFETIME_AFTER(0.7f))
            {
                if((i < 40u) && STAGE_TICK_LIFETIME(1.0f, 0.0f))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition().xy();
                    const coreFloat   fBase = pEnemy->AimAtPlayerDual(i % 2u).Angle();
                    const coreUintW   iNum  = g_pGame->IsEasy() ? 2u : 3u;

                    for(coreUintW j = iNum; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iNum - 1u) * 0.5f) * 2.5f) + fBase);

                        const coreFloat fSpeed = ((iNum == 3u) && (j == 1u)) ? 0.72f : 0.7f;

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, fSpeed, pEnemy, vPos, vDir)->ChangeSize(1.5f);
                    }

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 5u, COLOR_ENERGY_YELLOW);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
                else if((i == 50u) && STAGE_TICK_LIFETIME(8.0f, 0.0f) && ((s_iTick % 8u) >= (g_pGame->IsEasy() ? 6u : 4u)))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(((s_iTick % 16u) >= 8u) ? 1u : 0u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 0.7f, pEnemy, vPos, vDir)->ChangeSize(1.5f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_YELLOW);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
        });

        if(g_pGame->IsTask())
        {
            STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS)
            {
                if(!bFirstHit) return;

                if(pBullet->HasStatus(BULLET_STATUS_REFLECTED))
                {
                    const coreInt32 iDamage = pBullet->GetDamage() * (g_pGame->IsMulti() ? 1 : GAME_PLAYERS);
                    if(iDamage >= pEnemy->GetCurHealth())
                    {
                        STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                    }
                }
            });

            if(m_fStageSubTime > 0.0f)   // prevent effect
            {
                const coreBool bEffectTick = STAGE_TICK_FREE(30.0f, 0.0f);

                for(coreUintW i = 0u; i < VIRIDO_DRUMS; ++i)
                {
                    const coreUintW iIndex = m_aiDrumIndex[i];
                    if(iIndex != UINT8_MAX)
                    {
                        const coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

                        if(m_aiDrumCount[i] >= 20u)
                        {
                            this->EndDrumBeat(i, true);
                            this->DisableBarrier(iIndex, false);

                            if(++iDrumNum >= 7u)
                            {
                                STAGE_BADGE(0u, BADGE_EASY, oBarrier.GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawProgress(iDrumNum, 7u, oBarrier.GetPosition());
                                g_pSpecialEffects->PlaySound(oBarrier.GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iDrumNum, 7u), SOUND_ITEM_02);
                            }

                            g_pSpecialEffects->PlaySound(oBarrier.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
                        }

                        if(bEffectTick)
                        {
                            const coreVector2 vPos = oBarrier.GetPosition().xy() + oBarrier.GetDirection().xy().Rotated90() * (I_TO_F((s_iTick * 4u) % 15u) - 7.0f);
                            if(g_pForeground->IsVisiblePoint(vPos)) g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 0.0f, 1u, COLOR_ENERGY_RED);
                        }
                    }
                }
            }
        }

        if(m_iStageSub == 7u)
        {
            STATIC_ASSERT(coreMath::IsAligned(iReflectSize, 2u))
            coreUint8* aiFlip  = r_cast<coreUint8*>(aiReflect);
            coreUint8* aiCount = r_cast<coreUint8*>(aiReflect + iReflectSize / 2u);

            g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                coreUintW iSize;
                switch(pBullet->GetID())
                {
                default: UNREACHABLE
                case cRayBullet  ::ID: iSize = sizeof(cRayBullet);   break;
                case cPulseBullet::ID: iSize = sizeof(cPulseBullet); break;
                case cSurgeBullet::ID: iSize = sizeof(cSurgeBullet); break;
                case cTeslaBullet::ID: iSize = sizeof(cTeslaBullet); break;
                }

                const coreUintW iIndex = (P_TO_UI(pBullet) / iSize) % iReflectWrap;

                if(pBullet->GetFlyTime() < 0.1f)
                {
                    if(aiFlip[iIndex]) aiCount[iIndex] = 0u;
                    aiFlip[iIndex] = 0u;
                }
                else
                {
                    aiFlip[iIndex] = 1u;
                }

                if(pBullet->HasStatus(BULLET_STATUS_REFLECTED))
                {
                    pBullet->RemoveStatus(BULLET_STATUS_REFLECTED);
                    if(++aiCount[iIndex] >= 20u) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pBullet->GetPosition())
                }
            });
        }

        cGrassBackground* pBackground = d_cast<cGrassBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetCoverAlpha(1.0f - STEPH3(0.5f, 3.5f, m_fStageTime));

        if(STAGE_BEGINNING)
        {
            pBackground->GetOutdoor()->LerpHeight(1.0f, 0.0f, 50u);

            pBackground->SetGroundDensity(0u, 1.0f, true);
            pBackground->SetGroundDensity(1u, 1.0f, true);
            pBackground->SetGroundDensity(2u, 1.0f, true);
            pBackground->SetGroundDensity(3u, 1.0f, true);
            pBackground->SetDecalDensity (0u, 1.0f, true);
        }

        const coreFloat fEnvLerp = pBackground->GetOutdoor()->GetLerp();

        pBackground->SetGroundDensity(3u, STEP(0.0f, 0.5f, 1.0f - fEnvLerp));
        pBackground->SetAirDensity   (2u, STEP(0.9f, 1.0f, 1.0f - fEnvLerp));

        STAGE_WAVE(0u, "1-1", {35.0f, 50.0f, 70.0f, 85.0f, 170.0f})   // EINS
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            this->DisableBarrier(i, false);

        this->SetBarrierRange (1.0f);
        this->SetBarrierIgnore(true);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        cGrassBackground* pBackground = d_cast<cGrassBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetCoverAlpha(0.0f);

        pBackground->GetOutdoor()->LerpHeightNow(1.0f, 0.0f);
        pBackground->SetGroundDensity(0u, 1.0f);
        pBackground->SetGroundDensity(1u, 1.0f);
        pBackground->SetGroundDensity(2u, 1.0f);
        pBackground->SetGroundDensity(3u, 0.0f);
        pBackground->SetDecalDensity (0u, 1.0f);
        pBackground->SetAirDensity   (2u, 0.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // cut with laser through everything
    // - the delay when rays "appear" is important, not when they "stop"
    // - rays should not come from below, first segments main direction is upwards
    // - 1., 2. coming from upper left, 3., 4. coming from upper right
    // - 1. starts right to left, 2. starts left to right, 3. starts right to left, 4. starts left to right
    // - enemies on the line should spawn in the middle of the line
    // - first two patterns have no single equal line
    // - bullets have a 3-delay pattern, but on the side and when enemies die the pattern might break
    // - fast line shows the player that there is no harm, in preparation for the multi-line wave
    // - enemies in 3. need to be positioned to easily kill them (to reduce pressure, which is reserved for later)
    // - enemies without line should not move too fast, as the sudden transition might overwhelm the player
    // - both rotating waves should not rotate into the same direction
    // - the multi lines should shoot down in a nice pattern, that you have to avoid while being pushed against the wall
    // TASK: 4 objects that lie on a laser and disappear after a short time must be shot
    // TASK: marked line must be touched for 1 second (pole dancing)
    // TASK EXTRA: destroy enemies in a certain order in the final phase, changes depending on the starting enemy
    // ACHIEVEMENT: touch every laser at least once
    // HARD: lasers block bullets
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        constexpr coreUintW iNumData = 8u;

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 43u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(30, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_GET_START(iNumData * 3u + 7u)
            STAGE_GET_FLOAT_ARRAY(afDistance, iNumData)
            STAGE_GET_VEC2_ARRAY (avMove,     iNumData)
            STAGE_GET_UINT64     (iLineTouch)
            STAGE_GET_FLOAT      (fGlobeTime)
            STAGE_GET_UINT       (iGlobeCount)
            STAGE_GET_UINT       (iCurLaser)
            STAGE_GET_UINT       (iOrderCur)
            STAGE_GET_UINT       (iOrderType)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        if(m_iStageSub < 9u)
        {
            for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
            {
                if(m_apLaserOwner[i] && m_apLaserOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                    this->DisableLaser(i, true);
            }
        }

        if(STAGE_CLEARED)
        {
            const auto nEnableLaserFunc = [&](const coreUintW iIndex)
            {
                this->EnableLaser(iCurLaser, pSquad1->GetEnemy(iIndex));
                if(++iCurLaser >= VIRIDO_LASERS) iCurLaser = 0u;
            };

            const auto nEnableGlobeFunc = [&](const coreFloat fTime)
            {
                this->EnableGlobe();
                fGlobeTime = fTime;
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

                if(g_pGame->IsTask()) nEnableGlobeFunc(10.0f);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 7u, 10u)
                nEnableLaserFunc( 7u);
                nEnableLaserFunc( 8u);
                nEnableLaserFunc( 9u);
                nEnableLaserFunc(10u);

                if(g_pGame->IsTask()) this->StartPoleDance(2u);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 11u, 14u)
                nEnableLaserFunc(11u);
                nEnableLaserFunc(12u);
                nEnableLaserFunc(13u);
                nEnableLaserFunc(14u);

                if(g_pGame->IsTask()) nEnableGlobeFunc(10.0f);
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

                if(g_pGame->IsTask()) nEnableGlobeFunc(5.0f);
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

                if(g_pGame->IsTask()) nEnableGlobeFunc(5.0f);
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 35u, 42u)
                nEnableLaserFunc(35u);
            }

            std::memset(afDistance, 0, sizeof(coreFloat) * iNumData);
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_PURPLE);

        if((m_iStageSub == 7u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
        }

        constexpr coreUintW iSharedIndex = 35u;

        const coreObject3D* pSharedLaser  = (*m_Laser.List())[iSharedIndex % VIRIDO_LASERS];
        const coreBool      bSharedActive = pSharedLaser->IsEnabled(CORE_OBJECT_ENABLE_ALL);   // outside, for consistent movement

        constexpr coreFloat fRange = FOREGROUND_AREA.x * 3.0f / 6.0f;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vBasePos, vBaseDir;
            coreFloat   fDelay, fShift, fSide;
            switch(i)
            {
            default: UNREACHABLE
            case  0u: vBasePos = coreVector2( 0.7f,-0.1f); vBaseDir = coreVector2(-2.0f,-1.0f).Normalized(); fDelay = 0.0f;        fShift =           0.0f; fSide = -1.0f; break;
            case  1u: vBasePos = coreVector2( 0.2f,-0.7f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.3f;        fShift =         -15.0f; fSide =  1.0f; break;
            case  2u: vBasePos = coreVector2( 0.0f, 0.8f); vBaseDir = coreVector2( 1.0f, 1.0f).Normalized(); fDelay = 0.45f;       fShift =         -35.0f; fSide =  1.0f; break;

            case  3u: vBasePos = coreVector2( 0.1f, 0.8f); vBaseDir = coreVector2( 1.0f, 2.0f).Normalized(); fDelay = 0.0f;        fShift =         -30.0f; fSide =  1.0f; break;
            case  4u: vBasePos = coreVector2( 0.2f, 0.2f); vBaseDir = coreVector2( 1.0f,-1.0f).Normalized(); fDelay = 0.3f;        fShift =         -35.0f; fSide =  1.0f; break;
            case  5u: vBasePos = coreVector2( 0.5f,-0.6f); vBaseDir = coreVector2( 1.0f, 5.0f).Normalized(); fDelay = 0.4f;        fShift =           0.0f; fSide = -1.0f; break;
            case  6u: vBasePos = coreVector2(-0.8f, 0.1f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.7f;        fShift =         -25.0f; fSide = -1.0f; break;

            case  7u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;        fShift =         -30.0f; fSide = -1.0f; break;
            case  8u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.25f;       fShift =         -30.0f; fSide =  1.0f; break;
            case  9u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;        fShift =         -30.0f; fSide = -1.0f; break;
            case 10u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.75f;       fShift =         -30.0f; fSide =  1.0f; break;

            case 11u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;        fShift =           0.0f; fSide = -1.0f; break;
            case 12u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.25f;       fShift =           0.0f; fSide =  1.0f; break;
            case 13u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;        fShift =           0.0f; fSide = -1.0f; break;
            case 14u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.75f;       fShift =           0.0f; fSide =  1.0f; break;

            case 15u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.0f;        fShift =           0.0f; fSide =  1.0f; break;

            case 16u: vBasePos = coreVector2( 0.0f,-0.8f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.0f * 0.9f; fShift =           0.0f; fSide =  1.0f; break;
            case 17u: vBasePos = coreVector2( 0.0f,-0.7f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.1f * 0.9f; fShift =           0.0f; fSide = -1.0f; break;
            case 18u: vBasePos = coreVector2( 0.0f,-0.6f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.2f * 0.9f; fShift =           0.0f; fSide =  1.0f; break;
            case 19u: vBasePos = coreVector2( 0.0f,-0.5f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.3f * 0.9f; fShift =           0.0f; fSide = -1.0f; break;
            case 20u: vBasePos = coreVector2( 0.0f,-0.4f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.4f * 0.9f; fShift =           0.0f; fSide =  1.0f; break;
            case 21u: vBasePos = coreVector2( 0.0f,-0.3f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f * 0.9f; fShift =           0.0f; fSide = -1.0f; break;
            case 22u: vBasePos = coreVector2( 0.0f,-0.2f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.6f * 0.9f; fShift =           0.0f; fSide =  1.0f; break;
            case 23u: vBasePos = coreVector2( 0.0f,-0.1f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.7f * 0.9f; fShift =           0.0f; fSide = -1.0f; break;

            case 24u: vBasePos = coreVector2( 0.0f, 0.1f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.0f;        fShift = fRange * -0.0f; fSide =  1.0f; break;
            case 25u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;        fShift = fRange * -1.0f; fSide = -1.0f; break;
            case 26u: vBasePos = coreVector2( 0.0f,-0.1f); vBaseDir = coreVector2( 1.0f, 0.0f);              fDelay = 0.0f;        fShift = fRange * -2.0f; fSide =  1.0f; break;
            case 27u: vBasePos = coreVector2( 0.1f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.0f;        fShift = fRange * -3.0f; fSide =  1.0f; break;
            case 28u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.0f;        fShift = fRange * -4.0f; fSide = -1.0f; break;
            case 29u: vBasePos = coreVector2(-0.1f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.0f;        fShift = fRange * -5.0f; fSide =  1.0f; break;

            case 30u:
            case 31u:
            case 32u:
            case 33u:
            case 34u:
                vBasePos = coreVector2(0.0f,0.0f); vBaseDir = coreVector2::Direction(I_TO_F(i) * (0.4f*PI)); fDelay = 0.0f; fShift = 0.0f; fSide = -1.0f;
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
            else if(i < 11u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 0.6f)) * (FOREGROUND_AREA * 1.25f);
            else if(i < 15u) vLerpPos = LERP (-vTan,                  vTan, 0.5f - 0.5f * COS(fTime * 2.5f)) * (FOREGROUND_AREA * 1.25f);
            else if(i < 16u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 2.0f)) * (FOREGROUND_AREA * 1.2f);
            else if(i < 24u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 1.2f)) * (FOREGROUND_AREA * 1.25f);
            else if(i < 30u) vLerpPos = LERPB(vBasePos - vTan * 2.0f, vBasePos,          MIN1(fTime * 1.2f)) * (FOREGROUND_AREA)        * coreMatrix3::Rotation(fTime *  2.0f).m12();
            else if(i < 35u) vLerpPos = LERPB(-vTan * 1.4f,          -vTan * 0.5f,       MIN1(fTime * 1.2f)) * (FOREGROUND_AREA * 1.2f) * coreMatrix3::Rotation(fTime * -1.1f).m12();
            else if(i < 43u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 0.8f)) * (FOREGROUND_AREA * 1.2f);
            else UNREACHABLE

            coreVector2 vLerpDir;
                 if(i <  7u) vLerpDir = vBaseDir;
            else if(i < 11u) vLerpDir = vBaseDir;
            else if(i < 15u) vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime *  2.0f).m12();
            else if(i < 16u) vLerpDir = vBaseDir;
            else if(i < 24u) vLerpDir = vBaseDir;
            else if(i < 30u) vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime *  2.0f).m12();
            else if(i < 35u) vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime * -1.1f).m12();
            else if(i < 43u) vLerpDir = vBaseDir;
            else UNREACHABLE

            coreFloat& fDistance = STAGE_SINK_FLOAT(afDistance[i % iNumData]);

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
                    if(fDistance > 0.0f) fDistance -= MAX0(g_pForeground->RayIntersection(vNewPos, -vLerpDir, 1.2f));
                }
            }
            else if(i >= 24u && i < 30u)
            {
                if(fDistance > FOREGROUND_AREA.x * 1.5f) fDistance -= FOREGROUND_AREA.x * 3.0f;
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

                        if(g_pGame->IsTaskExtra())
                        {
                            iOrderType = i;
                            ASSERT(iOrderType)
                        }
                    }
                }
                else
                {
                    ASSERT((i - iSharedIndex) < iNumData)

                    coreVector2& vMove    = avMove[i - iSharedIndex];
                    coreVector2  vNewPos2 = pEnemy->GetPosition().xy() + vMove * (20.0f * TIME);

                         if((vNewPos2.x < -FOREGROUND_AREA.x * 1.1f) && (vMove.x < 0.0f)) {vNewPos2.x -= 2.0f * (vNewPos2.x + FOREGROUND_AREA.x * 1.1f); vMove.x =  ABS(vMove.x);}
                    else if((vNewPos2.x >  FOREGROUND_AREA.x * 1.1f) && (vMove.x > 0.0f)) {vNewPos2.x -= 2.0f * (vNewPos2.x - FOREGROUND_AREA.x * 1.1f); vMove.x = -ABS(vMove.x);}
                         if((vNewPos2.y < -FOREGROUND_AREA.y * 1.1f) && (vMove.y < 0.0f)) {vNewPos2.y -= 2.0f * (vNewPos2.y + FOREGROUND_AREA.y * 1.1f); vMove.y =  ABS(vMove.y);}
                    else if((vNewPos2.y >  FOREGROUND_AREA.y * 1.1f) && (vMove.y > 0.0f)) {vNewPos2.y -= 2.0f * (vNewPos2.y - FOREGROUND_AREA.y * 1.1f); vMove.y = -ABS(vMove.y);}

                    pEnemy->SetPosition (coreVector3(vNewPos2, 0.0f));
                    pEnemy->SetDirection(coreVector3(vMove,    0.0f));

                    if(iOrderType)
                    {
                        const coreUint32 iOrderIndex = (iOrderType + (i - ((i > iOrderType) ? 1u : 0u)) * 3u) % (iNumData - 1u);

                        if(iOrderIndex < 3u)
                        {
                            if(!pEnemy->HasStatus(ENEMY_STATUS_CUSTOM))
                            {
                                pEnemy->SetBaseColor(COLOR_SHIP_BLACK, false, true);
                                pEnemy->AddStatus   (ENEMY_STATUS_CUSTOM);
                            }

                            g_pGame->GetCombatText()->AttachMarker(i % COMBAT_MARKERS, coreData::ToChars(iOrderIndex + 1u), pEnemy->GetPosition(), COLOR_MENU_INSIDE, false);
                        }

                        if(pEnemy->ReachedDeath())
                        {
                            if(iOrderIndex < 3u)
                            {
                                if(++iOrderCur == 3u) STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                            }
                            else
                            {
                                STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
                                {
                                    if(pEnemy2->HasStatus(ENEMY_STATUS_CUSTOM))
                                    {
                                        pEnemy2->SetBaseColor(COLOR_SHIP_PURPLE);
                                    }
                                });

                                iOrderType = 0u;
                            }
                        }
                    }
                }
            }

            if(STAGE_TICK_TIME2(12.0f, 0.0f) && ((s_iTick % 12u) >= 9u) && (!g_pGame->IsEasy() || ((s_iTick % 36u) < 12u)))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.3f)->SetFlyTime(-0.5f);   // preserve patterns in rotation waves
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.3f)->SetFlyTime(-0.5f);

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_CYAN);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }

            if((i == 6u) || (i == 14u) || (i == 19u) || (i == 30u))
            {
                if(g_pGame->IsTask() && !m_Globe.HasStatus(ENEMY_STATUS_DEAD))
                {
                    fGlobeTime -= 1.0f * TIME;
                    if(fGlobeTime <= 0.0f)
                    {
                        this->DisableGlobe(true);
                    }

                    coreFloat fGlobeDist;
                    switch(i)
                    {
                    default: UNREACHABLE
                    case  6u: fGlobeDist =   0.0f; break;
                    case 14u: fGlobeDist =   0.0f; break;
                    case 19u: fGlobeDist =  30.0f; break;
                    case 30u: fGlobeDist = -30.0f; break;
                    }

                    m_Globe.SetPosition(coreVector3(vLerpPos + vLerpDir * fGlobeDist, 0.0f));
                    m_Globe.SetSize    (coreVector3(1.0f,1.0f,1.0f) * 3.0f * BLENDH3(MIN1(fGlobeTime)));

                    if(pEnemy->ReachedDeath() || m_Globe.WasDamaged())
                    {
                        this->DisableGlobe(false);
                        g_pSpecialEffects->MacroExplosionColorSmall(m_Globe.GetPosition(), COLOR_ENERGY_PURPLE);
                    }

                    if(m_Globe.WasDamaged())
                    {
                        if(++iGlobeCount == 4u)
                        {
                            STAGE_BADGE(0u, BADGE_EASY, m_Globe.GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawProgress(iGlobeCount, 4u, m_Globe.GetPosition());
                            g_pSpecialEffects->PlaySound(m_Globe.GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iGlobeCount, 4u), SOUND_ITEM_02);
                        }

                        g_pSpecialEffects->PlaySound(m_Globe.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                    }
                }
            }

            if((i == 25u) || (i == 28u))
            {
                pHelper->SetPosition(coreVector3(vLerpPos, 0.0f));
                if(pEnemy->ReachedDeath())
                {
                    pHelper->Kill(true);
                }
            }

            if(pEnemy->ReachedDeath())
            {
                if(!HAS_BIT(iLineTouch, MIN(i, iSharedIndex)))
                {
                    STAGE_FAILTROPHY
                }
            }
        });

        if(m_iLaserTouch)
        {
            for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
            {
                if(HAS_BIT(m_iLaserTouch, i) && m_apLaserOwner[i])
                {
                    const cEnemy*   pEnemy = d_cast<const cEnemy*>(m_apLaserOwner[i]);
                    const coreUintW iIndex = pSquad1->GetIndex(pEnemy);

                    ADD_BIT(iLineTouch, iIndex)

                    if(coreMath::PopCount(iLineTouch) >= 36u) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
                }
            }
        }

        if(m_iPoleIndex != UINT8_MAX)
        {
            const coreVector3 vPolePos = (*m_Laser.List())[m_iPoleIndex]->GetPosition();

            if(m_fPoleCount >= 1.0f)
            {
                STAGE_BADGE(1u, BADGE_NORMAL, vPolePos)
                this->EndPoleDance(true);
            }
            else if(m_fPoleCount > 0.0f)
            {
                g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.0f%%", FLOOR((1.0f - m_fPoleCount) * 100.0f)), vPolePos, COLOR_MENU_INSIDE, false);
            }
        }

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,1.0f).Normalized(), 30.0f);
        }

        STAGE_WAVE(1u, "1-2", {40.0f, 60.0f, 80.0f, 100.0f, 200.0f})   // ZWEI
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
            this->DisableLaser(i, false);

        this->DisableGlobe(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u, 3u})
    {
        cGrassBackground* pBackground = d_cast<cGrassBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetGroundDensity(4u, 1.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,1.0f).Normalized());

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // enemies jump into camera
    // - target shadow is essential, also to communicate direct targeting in third sub-stage
    // - bullet patterns should provide a lot of safe space, as jumping into the camera may overwhelm the player
    // - on circle group, enemies should not jump 3 or 5 places (around), so that not everyone can be killed form the center
    // - on 2x2 group, they may jump into positions of previous enemies, but that should be fine
    // - hopping enemies always need a 50% shift partner, so the gameplay keeps on moving (which makes those groups look faster, even though they have frequent pauses due to hopping)
    // - big enemy stomp bullets should not have holes, so you have to jump over them (on normal)
    // - big enemy stomp bullets can also be circumvented by staying near him (if attack comes from him), and compensate rotation by jumping over him for extra damage
    // - circle group, 2x2 group, and chess group should jump in alternating directions
    // TASK: fly over marked shadows
    // TASK: jump over big enemy
    // TASK EXTRA: kill all single-jumpers
    // ACHIEVEMENT: do not move while enemies are in the air
    // TODO 1: hardmode: enemies attack from the air
    // TODO 1: wave bullets should have a nice render ordering in 2x2 group
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        constexpr coreUintW iNumData   = 12u;
        constexpr coreUintW iBigIndex  = 67u;
        constexpr coreFloat fBaseSpeed = 0.9f;

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 68u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.1f);

                if(i == iBigIndex)
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.8f);
                    pEnemy->Configure(200 + 100, COLOR_SHIP_ORANGE);
                }
                else
                {
                    pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                    pEnemy->Configure(10, COLOR_SHIP_ORANGE);
                }

                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_GHOST_PLAYER);
            });
        });

        STAGE_GET_START(2u * GAME_PLAYERS + 9u * iNumData + 7u)
            STAGE_GET_VEC2_ARRAY (avPlayerPos,  GAME_PLAYERS)
            STAGE_GET_VEC2_ARRAY (avOldPos,     iNumData)
            STAGE_GET_VEC2_ARRAY (avNewPos,     iNumData)
            STAGE_GET_FLOAT_ARRAY(avOldAngle,   iNumData)
            STAGE_GET_FLOAT_ARRAY(avNewAngle,   iNumData)
            STAGE_GET_FLOAT_ARRAY(afJump,       iNumData)
            STAGE_GET_FLOAT_ARRAY(afWait,       iNumData)
            STAGE_GET_UINT_ARRAY (aiCount,      iNumData)
            STAGE_GET_UINT       (iLeapCount)
            STAGE_GET_UINT       (iBigShakeNum)
            STAGE_GET_UINT       (iSingleHit)
            STAGE_GET_UINT       (iShadowHit)
            STAGE_GET_UINT       (iShadowState)
            STAGE_GET_UINT       (iPlayerMove)
            STAGE_GET_FLOAT      (fPlayerJump)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        for(coreUintW i = 0u; i < VIRIDO_SHADOWS_ENEMY; ++i)
        {
            if(m_apShadowOwner[i] && m_apShadowOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableShadow(i, true);
        }

        const coreFloat fEasySpeed = (g_pGame->IsEasy() ? 0.4f : fBaseSpeed);

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
                    return coreVector2::Direction(I_TO_F(iIndex - 20u) * (0.8f*PI)) * 1.3f * SQRT2;
                };

                STAGE_RESURRECT(pSquad1, 20u, 24u)
                nInitFunc(20u, nPosFunc(20u), -nPosFunc(20u).Normalized());
                nInitFunc(21u, nPosFunc(21u), -nPosFunc(21u).Normalized());
                nInitFunc(22u, nPosFunc(22u), -nPosFunc(22u).Normalized());
                nInitFunc(23u, nPosFunc(23u), -nPosFunc(23u).Normalized());
                nInitFunc(24u, nPosFunc(24u), -nPosFunc(24u).Normalized());
            }
            else if(STAGE_SUB(5u))
            {
                const auto nPosFunc = [](const coreUintW iIndex)
                {
                    return StepRotated45((((iIndex - 25u) / 4u) * 3u + 6u) % 8u).Processed(SIGNUM) * 1.3f;
                };

                STAGE_RESURRECT(pSquad1, 25u, 36u)
                nInitFunc(25u, nPosFunc(25u), -nPosFunc(25u).Normalized());
                nInitFunc(26u, nPosFunc(26u), -nPosFunc(26u).Normalized());
                nInitFunc(27u, nPosFunc(27u), -nPosFunc(27u).Normalized());
                nInitFunc(28u, nPosFunc(28u), -nPosFunc(28u).Normalized());
                nInitFunc(29u, nPosFunc(29u), -nPosFunc(29u).Normalized());
                nInitFunc(30u, nPosFunc(30u), -nPosFunc(30u).Normalized());
                nInitFunc(31u, nPosFunc(31u), -nPosFunc(31u).Normalized());
                nInitFunc(32u, nPosFunc(32u), -nPosFunc(32u).Normalized());
                nInitFunc(33u, nPosFunc(33u), -nPosFunc(33u).Normalized());
                nInitFunc(34u, nPosFunc(34u), -nPosFunc(34u).Normalized());
                nInitFunc(35u, nPosFunc(35u), -nPosFunc(35u).Normalized());
                nInitFunc(36u, nPosFunc(36u), -nPosFunc(36u).Normalized());
            }
            else if(STAGE_SUB(6u))
            {
                const auto nPosFunc = [](const coreUintW iIndex)
                {
                    return coreVector2::Direction(I_TO_F(((iIndex - 37u) + 2u) % 8u) * (0.25f*PI)) * 1.3f * SQRT2;
                };

                STAGE_RESURRECT(pSquad1, 37u, 44u)
                nInitFunc(37u, nPosFunc(37u), -nPosFunc(37u).Normalized());
                nInitFunc(38u, nPosFunc(38u), -nPosFunc(38u).Normalized());
                nInitFunc(39u, nPosFunc(39u), -nPosFunc(39u).Normalized());
                nInitFunc(40u, nPosFunc(40u), -nPosFunc(40u).Normalized());
                nInitFunc(41u, nPosFunc(41u), -nPosFunc(41u).Normalized());
                nInitFunc(42u, nPosFunc(42u), -nPosFunc(42u).Normalized());
                nInitFunc(43u, nPosFunc(43u), -nPosFunc(43u).Normalized());
                nInitFunc(44u, nPosFunc(44u), -nPosFunc(44u).Normalized());
            }
            else if(STAGE_SUB(7u))
            {
                STAGE_RESURRECT(pSquad1, 45u, 52u)
                nInitFunc(45u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(46u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(47u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(48u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(49u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
                nInitFunc(50u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
                nInitFunc(51u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
                nInitFunc(52u, coreVector2( 0.0f,-1.3f), coreVector2( 0.0f, 1.0f));
            }
            else if(STAGE_SUB(8u))
            {
                STAGE_RESURRECT(pSquad1, 53u, 58u)
                nInitFunc(53u, coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
                nInitFunc(54u, coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
                nInitFunc(55u, coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
                nInitFunc(56u, coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
                nInitFunc(57u, coreVector2(-1.3f, 0.0f), coreVector2( 1.0f, 0.0f));
                nInitFunc(58u, coreVector2( 1.3f, 0.0f), coreVector2(-1.0f, 0.0f));
            }
            else if(STAGE_SUB(9u))
            {
                STAGE_RESURRECT(pSquad1, 59u, 66u)
                nInitFunc(59u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(60u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(61u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(62u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(63u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(64u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(65u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
                nInitFunc(66u, coreVector2( 0.0f, 1.3f), coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(10u))
            {
                STAGE_RESURRECT(pSquad1, 67u, 67u)
                nInitFunc(67u, coreVector2( 0.0f, 1.5f), coreVector2( 0.0f,-1.0f));
            }
            else if(STAGE_SUB(11u))
            {
                if(!iPlayerMove) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
            }

            for(coreUintW i = 0u; i < iNumData; ++i) afJump [i] = 0.0f;
            for(coreUintW i = 0u; i < iNumData; ++i) afWait [i] = 1.0f - 0.5f * fEasySpeed;
            for(coreUintW i = 0u; i < iNumData; ++i) aiCount[i] = 0u;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_GREEN);

        constexpr coreUintW aiQuad[] = {10u, 32u, 37u, 47u, 51u};

        if(g_pGame->IsTask())
        {
            for(coreUintW j = 0u; j < VIRIDO_SHADOWS_ENEMY; ++j)
            {
                const coreObject3D& oShadow = m_aShadowRaw[j];
                if(!oShadow.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                if(HAS_BIT(m_iShadowType, j))
                {
                    STAGE_FOREACH_PLAYER(pPlayer, i)
                    {
                        if(!m_apShadowOwner[j]) return;

                        const coreVector2 vDiff = pPlayer->GetPosition().xy() - oShadow.GetPosition().xy();
                        if(vDiff.LengthSq() < POW2(8.0f))
                        {
                            const coreUintW iIndex = pSquad1->GetIndex(d_cast<const cEnemy*>(m_apShadowOwner[j]));
                            const coreUintW iBit   = std::find(aiQuad, aiQuad + ARRAY_SIZE(aiQuad), iIndex) - aiQuad;

                            ASSERT(iBit < ARRAY_SIZE(aiQuad))
                            ADD_BIT(iShadowState, iBit)

                            this->DisableShadow(j, true);

                            if(++iShadowHit >= ARRAY_SIZE(aiQuad))
                            {
                                STAGE_BADGE(0u, BADGE_EASY, oShadow.GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawProgress(iShadowHit, ARRAY_SIZE(aiQuad), oShadow.GetPosition());
                                g_pSpecialEffects->PlaySound(oShadow.GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iShadowHit, ARRAY_SIZE(aiQuad)), SOUND_ITEM_01);
                            }
                        }
                    });
                }
            }
        }

        constexpr coreFloat fWaitSpeed    = 0.9f;
        constexpr coreFloat fJumpSpeed    = 0.75f;
        constexpr coreFloat fDelay        = (1.0f/fWaitSpeed) / fBaseSpeed + (1.0f/fJumpSpeed);

        constexpr coreFloat fWaitSpeedHop = 2.7f;
        constexpr coreFloat fJumpSpeedHop = 2.1f;
        constexpr coreFloat fDelayHop     = (1.0f/fWaitSpeedHop) / fBaseSpeed + (1.0f/fJumpSpeedHop);

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 12u) ? (0.2f * I_TO_F(i % 4u)) : ((i < 20u) ? (fDelay * 0.125f * I_TO_F(i % 8u)) : ((i < 25u) ? (fDelay * 0.25f * I_TO_F(i - 20u)) : ((i < 53u) ? 0.0f : ((i < 67u) ? (fDelayHop * 0.5f * I_TO_F((i - 53u) % 2u)) : 0.0f)))))

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

            const coreBool bHop    = (i >= 53u && i < 67u);
            const coreBool bSingle = (i >= 20u && i < 25u);

            if(i == iBigIndex)
            {
                const coreBool bPillar = ((fWait < (fWaitSpeed * fEasySpeed)) && !pEnemy->ReachedDeath());

                pEnemy->SetCollisionModifier(bPillar ? coreVector3(2.0f,2.0f,40.0f) : coreVector3(1.0f,1.0f,1.0f));

                if(pEnemy->ReachedDeath())
                {
                    pHelper->Kill(true);
                }
            }
            else if(bSingle)
            {
                if(g_pGame->IsTaskExtra() && pEnemy->ReachedDeath())
                {
                    if(++iSingleHit >= 5u)
                    {
                        STAGE_BADGE(2u, BADGE_HARD, coreVector3(0.0f,0.0f,0.0f))
                    }
                    else
                    {
                        g_pGame->GetCombatText()->DrawCountdown(iSingleHit, 5u, coreVector3(0.0f,0.0f,0.0f));
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iSingleHit, 5u), SOUND_PLACEHOLDER);
                    }
                }
            }

            if(STAGE_LIFETIME_AFTER(0.0f) && !pEnemy->ReachedDeath())
            {
                if(fWait < 1.0f)
                {
                    fWait += (bHop ? fWaitSpeedHop : fWaitSpeed) * fEasySpeed * TIME;
                    if(fWait >= 1.0f)
                    {
                        fJump = 0.0f;
                        fWait = 1.0f;

                        pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);

                        vOldPos   = pEnemy->GetPosition ().xy();
                        vOldAngle = pEnemy->GetDirection().xy().Angle();

                        if(i < 4u)   // one line
                        {
                            vNewPos   = coreVector2((1.5f - I_TO_F(i % 4u)) * 0.6f, 0.9f) * FOREGROUND_AREA * ((iCount % 2u) ? 1.0f : -1.0f);
                            vNewAngle = vOldAngle + 1.0f*PI;
                        }
                        else if(i < 12u)   // two lines
                        {
                            vNewPos   = coreVector2(0.9f, (1.5f - I_TO_F(i % 4u)) * 0.6f) * FOREGROUND_AREA * ((iCount % 2u) ? 1.0f : -1.0f) * ((i >= 8u) ? -1.0f : 1.0f);
                            vNewAngle = vOldAngle + 1.0f*PI;
                        }
                        else if(i < 20u)   // target
                        {
                            vNewPos   = pEnemy->NearestPlayerDual((i + iCount) % 2u)->GetPosition().xy();
                            vNewAngle = vOldAngle + 2.0f*PI;
                        }
                        else if(i < 25u)   // single
                        {
                            const coreFloat fLen = (iCount == 0u) ? -0.6f : -1.3f;

                            vNewPos   = coreVector2::Direction(I_TO_F(i - 20u) * (0.8f*PI)) * fLen * SQRT2 * FOREGROUND_AREA;
                            vNewAngle = vOldAngle + 2.0f*PI;
                        }
                        else if(i < 37u)   // 2x2
                        {
                            const coreUintW iIndex = i - 25u;

                            const coreFloat x = I_TO_F((iIndex)      % 2u) - 0.5f;
                            const coreFloat y = I_TO_F((iIndex / 2u) % 2u) - 0.5f;

                            const coreVector2 vTarget = StepRotated45(((iIndex / 4u) * 3u + (iCount * 3u)) % 8u).Processed(SIGNUM);

                            vNewPos   = (coreVector2(x, y) * 0.22f + vTarget * 0.79f) * FOREGROUND_AREA;
                            vNewAngle = vOldAngle - 1.25f*PI;
                        }
                        else if(i < 45u)   // circle
                        {
                            vNewPos   = StepRotated45(((i - 37u) + (iCount * 6u)) % 8u) * 0.9f * FOREGROUND_AREA;
                            vNewAngle = vOldAngle + 1.25f*PI;
                        }
                        else if(i < 53u)   // chess
                        {
                            const coreUintW iIndex = i - 45u;

                            const coreFloat x = I_TO_F((iIndex)      % 2u) - 0.5f;
                            const coreFloat y = I_TO_F((iIndex / 2u) % 2u) - 0.5f;

                            const coreVector2 vTarget = StepRotated90X(iCount % 4u) * (1.0f/SQRT2);

                            vNewPos   = (coreVector2(x, y) + vTarget * 0.5f) * 1.2f * FOREGROUND_AREA * ((i < 49u) ? 1.0f : -1.0f);
                            vNewAngle = vOldAngle + 1.0f*PI;
                        }
                        else if(i < 59u)   // side hop
                        {
                            const coreFloat fSign = ((iCount % 20u) < 10u) ? 1.0f : -1.0f;

                            vNewPos   = coreVector2((I_TO_F(iCount % 10u) - 4.5f) * 0.22f * (((i - 53u) % 2u) ? -1.0f : 1.0f) * fSign, (I_TO_F(i - 53u) - 2.5f) * 0.35f + ((iCount % 2u) ? -0.1f : 0.1f)) * FOREGROUND_AREA;
                            vNewAngle = vOldAngle;
                        }
                        else if(i < 67u)   // top hop
                        {
                            const coreFloat fSign = ((iCount % 20u) < 10u) ? 1.0f : -1.0f;

                            vNewPos   = coreVector2((I_TO_F((i - 59u) % 8u) - 3.5f) * 0.25f, LERP(0.95f, -0.95f, (I_TO_F(iCount % 10u) + (((i - 59u) % 2u) ? 0.25f : -0.25f)) / 9.0f) * fSign) * FOREGROUND_AREA;
                            vNewAngle = vOldAngle;
                        }
                        else if(i < 68u)   // big target
                        {
                            ASSERT(i == iBigIndex)

                            if(iCount % 2u)
                            {
                                vNewPos = vOldPos - vOldPos.Normalized() * FOREGROUND_AREA * 0.5f;
                            }
                            else
                            {
                                vNewPos = pEnemy->NearestPlayerDual((iCount / 2u) % 2u)->GetPosition().xy();
                            }
                            vNewAngle = vOldAngle + 2.0f*PI;
                        }
                        else UNREACHABLE

                        if(!pEnemy->ReachedDeath())
                        {
                            const coreUintW iBit  = std::find(aiQuad, aiQuad + ARRAY_SIZE(aiQuad), i) - aiQuad;
                            const coreBool  bQuad = g_pGame->IsTask() && (iBit < ARRAY_SIZE(aiQuad)) && !HAS_BIT(iShadowState, iBit);

                            this->EnableShadow(i % VIRIDO_SHADOWS_ENEMY, pEnemy, vNewPos, bQuad);
                        }

                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), (i == iBigIndex) ? 1.0f : 0.6f, 0.7f, SOUND_EFFECT_WOOSH_02);
                        if(i == iBigIndex) g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

                        iCount += 1u;

                        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
                        {
                            pHelper->Kill(false);
                        }
                    }

                    if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
                    {
                        pHelper->SetPosition(coreVector3(LERP(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-0.8f), SIN(MIN1(fWait / fEasySpeed) * PI)) * FOREGROUND_AREA, 0.0f));
                    }
                }
                else
                {
                    fJump += (bHop ? fJumpSpeedHop : fJumpSpeed) * TIME;
                    if(fJump >= 1.0f)
                    {
                        fJump = 1.0f;
                        fWait = 0.0f;

                        pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);

                        if(i == iBigIndex)
                        {
                            const coreVector2 vDir = StepRotated90(iCount % 4u);

                            for(coreUintW j = 32u; j--; )
                            {
                                if(g_pGame->IsEasy() && ((j % 4u < 2u))) continue;

                                const coreVector2 vPos = (vDir * -1.2f + vDir.Rotated90() * ((I_TO_F(j) - 15.5f) * 0.072f)) * FOREGROUND_AREA;

                                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.25f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                            }

                            g_pDistortion    ->CreateWave       (pEnemy->GetPosition(), 5.0f, 1.7f);
                            g_pSpecialEffects->CreateBlastSphere(pEnemy->GetPosition(), 4.0f, 2.0f, COLOR_ENERGY_WHITE);
                            g_pSpecialEffects->ShakeScreen      (SPECIAL_SHAKE_SMALL);

                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 0.7f, 1.0f, SOUND_EFFECT_SHAKE_01);
                            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);

                            if(++iBigShakeNum == 2u)
                            {
                                pHelper->Resurrect(false);
                            }

                            fPlayerJump = 1.0f;

                            cPlayer::TestCollision(PLAYER_TEST_NORMAL, pEnemy, [](cPlayer* OUTPUT pPlayer, const cEnemy* pEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
                            {
                                pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vIntersection.xy());   // # no first-hit check
                            });
                        }
                        else
                        {
                            const coreVector2 vPos = pEnemy->GetPosition().xy();

                            if(bHop)
                            {
                                const coreVector2 vDir = pEnemy->AimAtPlayerDual((i - 53u) % 2u).Normalized();
                                const coreVector2 vTan = vDir.Rotated90() * 1.5f;

                                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.6f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.6f);
                                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.6f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.6f);

                                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
                                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 0.8f, 1.7f, SOUND_EFFECT_DUST);
                            }
                            else
                            {
                                if(bSingle && (iCount == 2u))
                                {
                                    pEnemy->Kill(false);
                                    bPostpone = true;
                                }
                                else
                                {
                                    for(coreUintW j = 4u; j--; )
                                    {
                                        const coreVector2 vDir = StepRotated90(j);
                                        const coreVector2 vTan = vDir.Rotated90() * 3.0f;

                                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.6f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.6f);
                                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.6f, pEnemy, vPos,        vDir)->ChangeSize(1.6f);
                                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.6f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.6f);
                                    }

                                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 0.8f, 1.7f, SOUND_EFFECT_DUST);
                                }
                            }

                        }

                        this->DisableShadow(i % VIRIDO_SHADOWS_ENEMY, false);
                    }
                }

                const coreFloat   fSin    = SIN(fJump * (1.0f*PI));
                const coreVector2 vPos    = LERP(vOldPos, vNewPos, fJump) * (1.0f - 0.45f * fSin);
                const coreFloat   fHeight = 50.0f * fSin;

                pEnemy->SetPosition      (coreVector3(vPos, fHeight));
                pEnemy->DefaultRotateLerp(vOldAngle, vNewAngle, fJump);

                if(!bHop) pEnemy->DefaultOrientateLerp(0.0f*PI, 2.0f*PI, fJump);

                if((fHeight > 1.0f) && g_pForeground->IsVisibleObject(pEnemy))
                {
                    STAGE_FOREACH_PLAYER(pPlayer, i)
                    {
                        if(!pPlayer->GetMove().IsNull())
                        {
                            ADD_BIT(iPlayerMove, i)
                            STAGE_FAILTROPHY
                        }
                    });
                }
            }
        });

        if(fPlayerJump)
        {
            fPlayerJump = MAX0(fPlayerJump - 1.0f * TIME);

            const coreFloat   fHeight = 35.0f * SIN(fPlayerJump * (1.0f*PI));
            const coreMatrix2 mRota   = coreMatrix3::Rotation(TIME * (3.0f*PI)).m12();

            cEnemy* pBig = pSquad1->GetEnemy(iBigIndex);

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetPosition (coreVector3(pPlayer->GetPosition ().xy(), fHeight));
                pPlayer->SetDirection(coreVector3(pPlayer->GetDirection().xy() * mRota, pPlayer->GetDirection().z).Normalized());

                const coreUintW iShadowIndex = VIRIDO_SHADOWS_ENEMY + i;

                if(fPlayerJump)
                {
                    if(!pPlayer->HasStatus(PLAYER_STATUS_GHOST))
                    {
                        pPlayer->AddStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_TOP);

                        this->EnableShadow(iShadowIndex, pPlayer, pPlayer->GetPosition().xy(), false);

                        g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), SPECIAL_SPLASH_TINY, pPlayer->GetEnergyColor());

                        avPlayerPos[i] = pPlayer->GetPosition().xy();
                    }

                    m_aShadowRaw[iShadowIndex].SetPosition(coreVector3(pPlayer->GetPosition().xy(), 0.0f));
                }
                else
                {
                    if(pPlayer->HasStatus(PLAYER_STATUS_GHOST))
                    {
                        pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), pPlayer->GetDirection().z));
                        pPlayer->RemoveStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_TOP);

                        this->DisableShadow(iShadowIndex, false);

                        g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), 25.0f, 5u, pPlayer->GetEnergyColor());

                        if(g_pGame->IsTask() && !pBig->HasStatus(ENEMY_STATUS_DEAD))
                        {
                            const coreVector2 vDiff1 = pPlayer->GetPosition().xy() - avPlayerPos[i];
                            const coreVector2 vDiff2 = pPlayer->GetPosition().xy() - pBig->GetPosition().xy();

                            if(!vDiff1.IsNull() && !vDiff2.IsNull() && (coreVector2::Dot(vDiff1, vDiff2) > 0.0f))
                            {
                                pBig->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 0.9f);

                                const coreVector3 vRayPos = coreVector3(avPlayerPos[i],      0.0f);
                                const coreVector3 vRayDir = coreVector3(vDiff1.Normalized(), 0.0f);

                                coreFloat fHitDistance = 0.0f;
                                coreUint8 iHitCount    = 1u;
                                if(Core::Manager::Object->TestCollision(pBig, vRayPos, vRayDir, &fHitDistance, &iHitCount) && (iHitCount & 0x02u) && (POW2(fHitDistance) < vDiff1.LengthSq()))
                                {
                                    if(++iLeapCount >= 3u)
                                    {
                                        STAGE_BADGE(1u, BADGE_NORMAL, pBig->GetPosition())
                                    }
                                    else
                                    {
                                        g_pGame->GetCombatText()->DrawCountdown(iLeapCount, 3u, pBig->GetPosition());
                                        g_pSpecialEffects->PlaySound(pBig->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iLeapCount, 3u), SOUND_ITEM_02);
                                    }
                                }
                            }
                        }
                    }
                }
            });
        }

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(-1.0f,1.0f).Normalized(), 30.0f);
        }

        if(!bPostpone) STAGE_WAVE(2u, "1-3", {65.0f, 95.0f, 130.0f, 160.0f, 320.0f})   // DREI
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cWaveBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
            this->DisableShadow(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_GHOST);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(-1.0f,1.0f).Normalized());

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // enemy charges straight forward on death (r-type fish)
    // - fire speed fast enough to create visual line, but not too fast, to create holes on sharp angles (which have less predictable path, so it should be possible to micro-dodge them)
    // - small circle waves look like fireworks, big circle wave looks like a water-wheel
    // - angle and number of bounces of bounce enemy needs to be comprehensible, and the player-position should end near the bottom
    // - directions on fixed-direction moving enemies needs to give some holes for players to fly through, directions only facing into middle did not work
    // - general spawn-focus is at the top, so that the player has a safe space below
    // TASK: N enemy ships get stuck in the wall and have to be shot down
    // TASK: shoot N enemies at target object
    // TASK EXTRA: continue attacking big ship when bouncing around
    // ACHIEVEMENT: intercept the big enemy 7 times while it rushes around
    // TODO 1: hard-mode: enemies which charge infinitely (teleport, N-times)
    // TODO 1: hard-mode: every enemy bounces once (what about bouncy enemy?), or 1 infinity move (and bouncy enemy just moves N times infinitely)
    // TODO 1: hard-mode: enemies explode into near attack
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreUintW iNumData     = 16u;
        constexpr coreUintW iBounceIndex = 82u;

        constexpr coreUint8 aiStuck[] = {34u, 39u, 44u, 48u, 50u, 57u};
        const auto nIsStuckFunc = [&](const coreUint8 iIndex)
        {
            return g_pGame->IsTask() && std::memchr(aiStuck, iIndex, ARRAY_SIZE(aiStuck));
        };

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.3f * SQRT2), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f),         coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.7f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(2u);
            pPath3->AddNode(coreVector2(-0.6f, 1.5f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2( 0.6f,-1.5f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath4->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 83u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                const coreBool bStuck = nIsStuckFunc(i);

                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * ((i == iBounceIndex) ? 2.5f : 1.7f));
                pEnemy->Configure((i == iBounceIndex) ? 200 : 10, bStuck ? COLOR_SHIP_BLACK : COLOR_SHIP_YELLOW, false, bStuck);
                pEnemy->AddStatus(ENEMY_STATUS_TOP | ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(iNumData + 7u + GAME_PLAYERS)
            STAGE_GET_FLOAT_ARRAY(afSpeed, iNumData)
            STAGE_GET_FLOAT      (fAngle)
            STAGE_GET_FLOAT      (fTargetTime)
            STAGE_GET_UINT       (iTargetHit)
            STAGE_GET_UINT       (iBounce)
            STAGE_GET_UINT       (iStuckNum)
            STAGE_GET_UINT_ARRAY (aiTouchNum, GAME_PLAYERS)
            STAGE_GET_VEC2       (vHelperMove)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_RED);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(pEnemy->HasStatus(ENEMY_STATUS_WORTHLESS)) return;

            if(pEnemy->GetLostHealth() >= 10)
            {
                coreVector2 vPos = pEnemy->GetPosition ().xy();
                coreVector2 vDir = pEnemy->GetDirection().xy();

                if((i == iBounceIndex) && (iBounce < 8u))
                {
                    const auto nBounceFunc = [&]()
                    {
                        iBounce += 1u;

                        g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 100.0f, 20u, COLOR_ENERGY_RED);
                        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 0.5f, 1.7f, SOUND_EFFECT_SHAKE_01);
                        g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
                    };

                         if((vPos.x < -FOREGROUND_AREA.x * 1.1f) && (vDir.x < 0.0f)) {vPos.x -= 2.0f * (vPos.x + FOREGROUND_AREA.x * 1.1f); vDir.x =  ABS(vDir.x); nBounceFunc();}
                    else if((vPos.x >  FOREGROUND_AREA.x * 1.1f) && (vDir.x > 0.0f)) {vPos.x -= 2.0f * (vPos.x - FOREGROUND_AREA.x * 1.1f); vDir.x = -ABS(vDir.x); nBounceFunc();}
                         if((vPos.y < -FOREGROUND_AREA.y * 1.1f) && (vDir.y < 0.0f)) {vPos.y -= 2.0f * (vPos.y + FOREGROUND_AREA.y * 1.1f); vDir.y =  ABS(vDir.y); nBounceFunc();}
                    else if((vPos.y >  FOREGROUND_AREA.y * 1.1f) && (vDir.y > 0.0f)) {vPos.y -= 2.0f * (vPos.y - FOREGROUND_AREA.y * 1.1f); vDir.y = -ABS(vDir.y); nBounceFunc();}

                    pEnemy->SetPosition (coreVector3(vPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir, 0.0f));
                }
                else
                {
                    if(((vPos.x < -FOREGROUND_AREA.x * 1.1f) && (vDir.x < 0.0f)) ||
                       ((vPos.x >  FOREGROUND_AREA.x * 1.1f) && (vDir.x > 0.0f)) ||
                       ((vPos.y < -FOREGROUND_AREA.y * 1.1f) && (vDir.y < 0.0f)) ||
                       ((vPos.y >  FOREGROUND_AREA.y * 1.1f) && (vDir.y > 0.0f)))
                    {
                        pEnemy->ApplyScore();

                        if(nIsStuckFunc(i))
                        {
                            coreVector2 vNewPos = vPos;

                            if(IsHorizontal(vNewPos)) vNewPos -= vDir * ((ABS(vNewPos.x) - FOREGROUND_AREA.x * 1.1f) * SIGN(vNewPos.x) * RCP(vDir.x));
                                                 else vNewPos -= vDir * ((ABS(vNewPos.y) - FOREGROUND_AREA.y * 1.1f) * SIGN(vNewPos.y) * RCP(vDir.y));

                            pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                            pEnemy->SetCurHealth(1);
                            pEnemy->AddStatus   (ENEMY_STATUS_WORTHLESS);
                            pEnemy->RemoveStatus(ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST_BULLET);

                            afSpeed[i % iNumData] = 0.0f;

                            g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 100.0f, 20u, COLOR_ENERGY_RED);
                            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
                            g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.4f, 0.7f, SOUND_EFFECT_WOOSH_02);
                        }
                        else
                        {
                            pEnemy->Kill(true);

                            if(i == iBounceIndex)
                            {
                                pHelper->Resurrect(false);
                                pHelper->SetPosition(pEnemy->GetPosition());

                                vHelperMove = (g_pGame->IsEasy() ? pEnemy->GetDirection().xy() : pEnemy->AimAtPlayerDual(0u).Normalized()) * (0.5f * BULLET_SPEED_FACTOR);

                                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 100.0f, 40u, COLOR_ENERGY_RED);
                                g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_09);
                                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);

                                STAGE_FAILTROPHY
                            }
                            else
                            {
                                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 50.0f, 10u, COLOR_ENERGY_RED);
                            }

                            if(g_pGame->IsEasy() && (m_iStageSub >= 9u))
                            {
                                g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                                {
                                    const coreVector2 vDiff = pBullet->GetPosition().xy() - pEnemy->GetPosition().xy();
                                    if(vDiff.LengthSq() < POW2(10.0f))
                                    {
                                        pBullet->Deactivate(true);
                                    }
                                });
                            }
                        }
                    }
                }

                if(m_Target.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    const coreVector2 vDiff = pEnemy->GetPosition().xy() - m_Target.GetPosition().xy();

                    if(vDiff.LengthSq() < POW2(m_Target.GetVisualRadius() * 1.3f))
                    {
                        pEnemy->Kill(true);
                        pEnemy->ApplyScore();

                        if(++iTargetHit >= 3u)
                        {
                            this->DisableTarget(false);
                            g_pSpecialEffects->MacroExplosionColorBig(m_Target.GetPosition(), COLOR_ENERGY_CYAN);

                            STAGE_BADGE(1u, BADGE_NORMAL, m_Target.GetPosition())
                        }
                        else
                        {
                            g_pSpecialEffects->PlaySound(m_Target.GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iTargetHit, 3u), SOUND_ITEM_01);
                        }
                    }
                }
            }
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 32u, 36u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 37u, 45u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 46u, 61u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 82u, 82u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 62u, 73u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 74u, 81u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(9u)) STAGE_RESURRECT(pSquad1, 16u, 31u)

            std::memset(afSpeed, 0, sizeof(coreFloat) * iNumData);
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + vHelperMove * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

             if(m_iStageSub == 8u) fAngle += 0.25f*PI * TIME;
        else if(m_iStageSub == 9u) fAngle -= 0.25f*PI * TIME;
        const coreMatrix2 mRota = coreMatrix3::Rotation(fAngle).m12();

        if(g_pGame->IsTask())
        {
            if((m_iStageSub == 8u) && STAGE_BEGINNING2)
            {
                this->EnableTarget();
            }
            else if((m_iStageSub == 9u) && STAGE_BEGINNING2)
            {
                this->DisableTarget(true);
            }

            if(m_Target.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
            {
                fTargetTime += 1.0f * TIME;

                m_Target.SetPosition(coreVector3(coreVector2(LERPB(-1.3f, -0.9f, MIN1(fTargetTime * 0.5f)), 0.0f) * mRota * FOREGROUND_AREA, 0.0f));
                m_Target.SetSize    (coreVector3(1.0f,1.0f,1.0f) * (3.0f + 0.5f * I_TO_F(3u - iTargetHit)));

                g_pGame->GetCombatText()->AttachMarker(1u, coreData::ToChars(3u - iTargetHit), m_Target.GetPosition(), COLOR_MENU_INSIDE, false);
            }
        }

        coreBool bPostpone = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, (i < 46u || i == iBounceIndex) ? 0.85f : 0.6f, (i < 32u) ? (0.3f * I_TO_F(i % 2u)) : ((i < 46u) ? 0.0f : ((i < 62u) ? (0.5f + 0.165f * I_TO_F(i - 46u)) : ((i < 74u) ? (0.5f + 1.1f * I_TO_F((i - 62u) / 4u)) : ((i < 82u) ? 0.5f : 0.0f)))))

            if(pEnemy->HasStatus(ENEMY_STATUS_WORTHLESS))
            {
                if(pEnemy->ReachedDeath())
                {
                    if(++iStuckNum >= ARRAY_SIZE(aiStuck))
                    {
                        STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                    }
                    else
                    {
                        g_pGame->GetCombatText()->DrawProgress(iStuckNum, ARRAY_SIZE(aiStuck), pEnemy->GetPosition());
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iStuckNum, ARRAY_SIZE(aiStuck)), SOUND_ITEM_02);
                    }

                    g_pSpecialEffects->MacroEruptionColorBig(pEnemy->GetPosition(), -pEnemy->GetDirection().xy(), COLOR_ENERGY_RED);
                }

                coreFloat& fTime = STAGE_SINK_FLOAT(afSpeed[i % iNumData]);

                fTime += 1.0f * TIME;

                if(fTime >= 3.0f)
                {
                    pEnemy->Kill(true);
                    bPostpone = true;
                }

                if(STAGE_TICK_TIME(60.0f, 0.0f)) g_pSpecialEffects->CreateBlowColor(pEnemy->GetPosition(), -pEnemy->GetDirection(), 50.0f, 1u, COLOR_ENERGY_RED);
            }
            else
            {
                if(pEnemy->GetLostHealth() >= 10)
                {
                    if(!pEnemy->HasStatus(ENEMY_STATUS_DAMAGING))
                    {
                        pEnemy->AddStatus((i == iBounceIndex) ? ENEMY_STATUS_DAMAGING : (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_GHOST_BULLET));

                        g_pSpecialEffects->CreateBlowColor(pEnemy->GetPosition(), -pEnemy->GetDirection(), 75.0f, 10u, COLOR_ENERGY_RED);
                        g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), (i == iBounceIndex) ? 1.6f : 1.2f, 1.0f, SOUND_EFFECT_DUST);
                    }

                    coreFloat& fSpeed = STAGE_SINK_FLOAT(afSpeed[i % iNumData]);

                    fSpeed += ((i == iBounceIndex) ? 20.0f : 80.0f) * TIME;

                    pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * ((20.0f + fSpeed) * TIME));

                    if(STAGE_TICK_TIME(30.0f, 0.0f) && ((i == iBounceIndex) || (i < 32u) || (fSpeed < 40.0f)))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = g_pGame->IsEasy() ? pEnemy->GetDirection().xy() : pEnemy->AimAtPlayerDual(((i < 8u) ? (i / 2u) : ((i < 32u) ? (i / 4u) : i)) % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, (i >= 16u && i < 32u) ? 0.0f : 0.5f, pEnemy, vPos, vDir)->ChangeSize(1.25f);

                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }

                    if(g_pGame->IsTaskExtra() && (i == iBounceIndex))
                    {
                        const coreFloat fPercent = pEnemy->GetCurHealthPct();

                        if(fPercent <= 0.0f)
                        {
                            pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);

                            STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                        }
                        else g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.0f%%", FLOOR((1.0f - fPercent) * 100.0f)), pEnemy->GetPosition(), COLOR_MENU_INSIDE, false);
                    }
                }
                else
                {
                    if(i < 8u)
                    {
                        const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                        const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f + (((i % 4u) < 2u) ? -1.0f : 1.0f)) * 0.22f, ((i % 8u) < 4u) ? 0.0f : 0.22f);

                        pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                        pEnemy->SetDirection   (coreVector3(pEnemy->AimAtPlayerDual((i / 2u) % 2u).Normalized(), 0.0f));
                    }
                    else if(i < 32u)
                    {
                        const coreVector2 vFactor = coreVector2(1.0f, ((i % 8u) < 4u) ? 1.0f : -1.0f);
                        const coreVector2 vOffset = coreVector2((I_TO_F(i % 2u) - 0.5f) * 0.22f, ((i % 4u) < 2u) ? 0.0f : 0.22f);

                        pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                        if(i >= 24u) pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy().Rotated90() * mRota, 0.0f));
                                else pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy()             * mRota, 0.0f));

                        pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayerDual((i / 4u) % 2u).Normalized(), 0.0f));
                    }
                    else if(i < 37u)
                    {
                        const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                        const coreVector2 vOffset = coreVector2((I_TO_F(i - 32u) - 2.0f) * 0.3f, 0.11f);

                        pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                        pEnemy->SetDirection   (coreVector3(coreVector2::Direction((I_TO_F(i - 32u) - 2.0f) * (0.125f*PI) + (1.0f*PI)), 0.0f));
                    }
                    else if(i < 46u)
                    {
                        const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                        const coreVector2 vOffset = coreVector2(0.0f,0.11f);

                        pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                        pEnemy->ToAxis(coreVector2::Direction((I_TO_F(i - 37u) - 4.0f) * (0.15f*PI)));
                    }
                    else if(i < 62u)
                    {
                        STAGE_REPEAT(pPath4->GetTotalDistance())

                        const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                        const coreVector2 vOffset = coreVector2((I_TO_F((i - 46u) % 4u) - 1.5f) * 0.5f, 0.0f);

                        pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);
                        pEnemy->SetDirection   (coreVector3(StepRotated90X(((i - 46u) + (i - 46u) / 4u) % 4u), 0.0f));
                    }
                    else if(i < 74u)
                    {
                        STAGE_REPEAT(pPath3->GetTotalDistance())

                        const coreVector2 vDir = MapToAxis(StepRotated90(i % 4u), coreVector2::Direction(fLifeTime * 4.0f));

                        const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                        const coreVector2 vOffset = coreVector2(0.0f,0.0f) + vDir * 0.17f;

                        pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);
                        pEnemy->SetDirection   (coreVector3(vDir, 0.0f));

                             if(i < 66u) {}
                        else if(i < 70u) pEnemy->InvertX();
                        else if(i < 74u) {}

                        if(F_TO_UI(fLifeTimeBase * RCP(pPath3->GetTotalDistance())) % 2u) pEnemy->InvertX();
                    }
                    else if(i < 82u)
                    {
                        const coreVector2 vDir = MapToAxis(StepRotated45(i % 8u), coreVector2::Direction(fLifeTime * 3.0f));

                        const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                        const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                        pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                        pEnemy->ToAxis(vDir);
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
            }
        });

        STAGE_COLL_PLAYER_ENEMY(pPlayer, pEnemy, vIntersection, bFirstHit, COLL_THIS, COLL_VAL(pSquad1), COLL_VAL(aiTouchNum))
        {
            if(!bFirstHit) return;

            const cEnemy* pBounce = pSquad1->GetEnemy(iBounceIndex);
            if((pEnemy == pBounce) && pEnemy->HasStatus(ENEMY_STATUS_DAMAGING))
            {
                const coreUintW iIndex = g_pGame->GetPlayerIndex(pPlayer);
                if(++aiTouchNum[iIndex] >= 7u) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, pEnemy->GetPosition())
            }
        });

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(1.0f,1.0f).Normalized(), 30.0f);
        }

        if(!bPostpone) STAGE_WAVE(3u, "1-4", {60.0f, 90.0f, 120.0f, 150.0f, 300.0f})   // VIER
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTriangleBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        this->DisableTarget(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u, 5u})
    {
        cGrassBackground* pBackground = d_cast<cGrassBackground*>(g_pEnvironment->GetBackground());

        pBackground->SetGroundDensity(4u, 0.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(1.0f,1.0f).Normalized());

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // enemies move and shoot with player
    // - if enemies should react to shooting, they should check for player-weapon cooldown
    // - enemies should always shoot in the opposite direction to avoid easy kills (especially if their movement-direction is fixed, or one of their position-axes) (except in the finale)
    // - in crosshair group, if they spawn from a corner you can see all 4 groups even if you fly at the edge or the corner, but they still need to spawn far enough from the edge to avoid collisions when the player is in the corner
    // - in crosshair group, if they spawn from a corner they won't be accidentally shot down, the same for the final group (+ the final group should spawn in such a way that no two enemies overlap each other, looks bad)
    // - enemies not visible are not allowed to shoot (especially in crosshair group)
    // - in co-op, if player dies, target player should be changed, but not when revived
    // - in hunter group, enemies have to be positioned in such a way that you can't go to the edge "from the start" and shoot everything down
    // - in hunter group, offset+multiplier must be set so that all enemies remain visible
    // - (old: straight line or wave towards player (even with multiplier) was too complicated, collisions and bumping were unavoidable, especially when you have to dodge bullets, or it was too confusing and you wanted to let them pass and attack from the side)
    // - fast infinity group is separated into two sides, because otherwise the number of bullets was far too high and complicated, the player only concentrated on dodging and didn't notice how the enemies were moving
    // - circle group should not rotate too quickly, otherwise it would be too easy (as they just fly into the player-attack without much resistance)
    // - prison group is structured in such a way that for each player direction 3 enemies shoot from different sides into the middle (T shape), for the corner enemies you cannot destroy 2 enemies of a line without being attacked at least once, for the middle enemies there is a specific order (but that's ok)
    // - enemies (except in fast infinity group) should not move with -1.0f, otherwise navigation relative to the enemies is too difficult, especially in the wall group
    // - hunters can't be too slow, otherwise there won't be any holes between their bullets
    // TASK: collect all moving energy beans
    // TASK: 3 secret enemies that only show up at certain positions
    // TASK EXTRA: crosshair enemies must be killed in a specific order
    // ACHIEVEMENT: destroy all enemies while shooting to the right (in regular game rotation)
    // TODO 1: hard mode: 1:1 invert-mimic for both players, with fast laser bullets (yellow bullets?), or completely invert movement controls
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        constexpr coreUintW iHelperIndex = 23u;

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 58u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(30, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST_PLAYER);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cWarriorEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(8u)
            STAGE_GET_VEC2 (vBaseRefPos)
            STAGE_GET_UINT (iCrossState)
            STAGE_GET_UINT (iBeanState)
            STAGE_GET_UINT (iHiddenState)
            STAGE_GET_UINT (iNonRightShot)
            STAGE_GET_UINT (iPlayerTarget, iPlayerTarget = 1u)
            STAGE_GET_FLOAT(vPlayerLerp)
        STAGE_GET_END

        const cPlayer* pPlayer1 = g_pGame->GetPlayer(0u);
        const cPlayer* pPlayer2 = g_pGame->GetPlayer(1u);
        STATIC_ASSERT(GAME_PLAYERS == 2u)

        const coreBool bCleared = pSquad1->IsFinished();

             if(pPlayer1->HasStatus(PLAYER_STATUS_DEAD)) iPlayerTarget = 1u;
        else if(pPlayer2->HasStatus(PLAYER_STATUS_DEAD)) iPlayerTarget = 0u;
        else if(bCleared)                                iPlayerTarget = 1u - iPlayerTarget;

             if(bCleared)      vPlayerLerp = I_TO_F(iPlayerTarget);
        else if(iPlayerTarget) vPlayerLerp = MIN1(vPlayerLerp + 1.0f*TIME);
        else                   vPlayerLerp = MAX0(vPlayerLerp - 1.0f*TIME);

        const coreVector2 vBasePos = LERPS(pPlayer1->GetPosition().xy(), pPlayer2->GetPosition().xy(), vPlayerLerp) / FOREGROUND_AREA;
        const coreVector2 vBaseDir = iPlayerTarget ? pPlayer2->GetDirection().xy() : pPlayer1->GetDirection().xy();

        if(bCleared)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  4u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 19u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 20u, 27u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 28u, 35u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 36u, 41u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 42u, 45u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1, 46u, 57u)
            else if(STAGE_SUB(9u))
            {
                if(!iNonRightShot) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
            }

            if(g_pGame->IsTask())
            {
                pSquad2->ClearEnemies(true);
                     if(m_iStageSub == 1u) STAGE_RESURRECT(pSquad2, 0u, 0u)
                else if(m_iStageSub == 4u) STAGE_RESURRECT(pSquad2, 1u, 1u)
                else if(m_iStageSub == 7u) STAGE_RESURRECT(pSquad2, 2u, 2u)
            }

            const coreVector2 vPos = vBasePos;
            const coreVector2 vDir = -vPos.Normalized();

            vBaseRefPos = (vPos + vDir * g_pForeground->RayIntersection(vPos, vDir, 1.4f)) / FOREGROUND_AREA;

            if(m_iStageSub == 6u)
            {
                for(coreUintW i = 0u; i < VIRIDO_BEANS; ++i)
                    this->EnableBean(i);
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_ORANGE);

        if(g_pGame->IsTask())
        {
            for(coreUintW i = 0u; i < VIRIDO_BEANS; ++i)
            {
                coreObject3D* pBean = this->GetBean(i);
                if(!pBean->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreFloat fTime   = 1.3f + I_TO_F(i) * 0.5f + (i ? 2.0f : 0.5f) - m_fStageSubTime * (i ? 1.05f : 0.8f);
                const coreFloat fOffset = i ? (vBasePos.x * ((i % 2u) ? -1.0f : 0.5f) * 0.8f + LERP(-0.5f, 0.5f, (I_TO_F((i * 3u) % VIRIDO_BEANS) / I_TO_F(VIRIDO_BEANS - 1u)))) : vBasePos.x;
                const coreFloat fHeight = LERPBR(-1.3f, 1.3f, STEP(-1.3f, 1.3f, fTime));

                if(fTime < -1.3f) this->DisableBean(i, false);

                pBean->SetPosition(coreVector3(fOffset, fHeight, 0.0f) * FOREGROUND_AREA3);

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pBean->GetPosition().xy() - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(8.0f))
                    {
                        this->DisableBean(i, true);

                        if(++iBeanState >= VIRIDO_BEANS)
                        {
                            STAGE_BADGE(0u, BADGE_EASY, pBean->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawCountdown(iBeanState, VIRIDO_BEANS, pBean->GetPosition());
                            g_pSpecialEffects->PlaySound(pBean->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iBeanState, VIRIDO_BEANS), SOUND_ITEM_01);
                        }
                    }
                });
            }
        }

        const coreVector2 vRota = coreVector2::Direction(m_fStageSubTime * 0.5f);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i >= 36u && i < 42u && g_pGame->IsTask()) ? 10.2f : 0.0f)

            coreVector2 vNewPos    = vBasePos;
            coreVector2 vNewRefPos = vBaseRefPos;
            coreVector2 vNewDir    = vBaseDir;

            switch(i)
            {
            default: UNREACHABLE

            // mirror
            case  0u: vNewPos = vNewPos * -0.85f + coreVector2(-1.0f,-1.0f).Rotated45() * 0.24f; break;
            case  1u: vNewPos = vNewPos * -0.85f + coreVector2( 1.0f,-1.0f).Rotated45() * 0.24f; break;
            case  2u: vNewPos = vNewPos * -0.85f + coreVector2(-1.0f, 1.0f).Rotated45() * 0.24f; break;
            case  3u: vNewPos = vNewPos * -0.85f + coreVector2( 1.0f, 1.0f).Rotated45() * 0.24f; break;

            // rotated mirror
            case  4u: vNewPos = vNewPos * -0.85f + coreVector2(-1.0f,-1.0f).Rotated45() * 0.12f; vNewPos =  vNewPos.Rotated90(); break;
            case  5u: vNewPos = vNewPos * -0.85f + coreVector2( 1.0f,-1.0f).Rotated45() * 0.12f; vNewPos =  vNewPos.Rotated90(); break;
            case  6u: vNewPos = vNewPos * -0.85f + coreVector2(-1.0f, 1.0f).Rotated45() * 0.12f; vNewPos =  vNewPos.Rotated90(); break;
            case  7u: vNewPos = vNewPos * -0.85f + coreVector2( 1.0f, 1.0f).Rotated45() * 0.12f; vNewPos =  vNewPos.Rotated90(); break;
            case  8u: vNewPos = vNewPos * -0.85f + coreVector2(-1.0f,-1.0f).Rotated45() * 0.12f; vNewPos = -vNewPos.Rotated90(); break;
            case  9u: vNewPos = vNewPos * -0.85f + coreVector2( 1.0f,-1.0f).Rotated45() * 0.12f; vNewPos = -vNewPos.Rotated90(); break;
            case 10u: vNewPos = vNewPos * -0.85f + coreVector2(-1.0f, 1.0f).Rotated45() * 0.12f; vNewPos = -vNewPos.Rotated90(); break;
            case 11u: vNewPos = vNewPos * -0.85f + coreVector2( 1.0f, 1.0f).Rotated45() * 0.12f; vNewPos = -vNewPos.Rotated90(); break;

            // crosshair
            case 12u: vNewPos = vNewPos + coreVector2( 1.0f, 0.0f) * 0.6f;  vNewRefPos = coreVector2( 0.0f, 1.0f) * 1.4f; break;
            case 13u: vNewPos = vNewPos + coreVector2(-1.0f, 0.0f) * 0.6f;  vNewRefPos = coreVector2( 0.0f,-1.0f) * 1.4f; break;
            case 14u: vNewPos = vNewPos + coreVector2( 0.0f, 1.0f) * 0.6f;  vNewRefPos = coreVector2(-1.0f, 0.0f) * 1.4f; break;
            case 15u: vNewPos = vNewPos + coreVector2( 0.0f,-1.0f) * 0.6f;  vNewRefPos = coreVector2( 1.0f, 0.0f) * 1.4f; break;
            case 16u: vNewPos = vNewPos + coreVector2( 1.0f, 0.0f) * 0.84f; vNewRefPos = coreVector2( 0.0f, 1.0f) * 1.4f; break;
            case 17u: vNewPos = vNewPos + coreVector2(-1.0f, 0.0f) * 0.84f; vNewRefPos = coreVector2( 0.0f,-1.0f) * 1.4f; break;
            case 18u: vNewPos = vNewPos + coreVector2( 0.0f, 1.0f) * 0.84f; vNewRefPos = coreVector2(-1.0f, 0.0f) * 1.4f; break;
            case 19u: vNewPos = vNewPos + coreVector2( 0.0f,-1.0f) * 0.84f; vNewRefPos = coreVector2( 1.0f, 0.0f) * 1.4f; break;

            // circle
            case 20u: vNewPos = vNewPos * -0.5f + vRota              * 0.8f; vNewRefPos =  coreVector2(0.0f,1.0f)              * (1.4f * SQRT2); break;
            case 21u: vNewPos = vNewPos * -0.5f + vRota.Rotated45 () * 0.8f; vNewRefPos =  coreVector2(0.0f,1.0f).Rotated45 () * (1.4f * SQRT2); break;
            case 22u: vNewPos = vNewPos * -0.5f + vRota.Rotated90 () * 0.8f; vNewRefPos =  coreVector2(0.0f,1.0f).Rotated90 () * (1.4f * SQRT2); break;
            case 23u: vNewPos = vNewPos * -0.5f + vRota.Rotated135() * 0.8f; vNewRefPos =  coreVector2(0.0f,1.0f).Rotated135() * (1.4f * SQRT2); break;
            case 24u: vNewPos = vNewPos * -0.5f - vRota              * 0.8f; vNewRefPos = -coreVector2(0.0f,1.0f)              * (1.4f * SQRT2); break;
            case 25u: vNewPos = vNewPos * -0.5f - vRota.Rotated45 () * 0.8f; vNewRefPos = -coreVector2(0.0f,1.0f).Rotated45 () * (1.4f * SQRT2); break;
            case 26u: vNewPos = vNewPos * -0.5f - vRota.Rotated90 () * 0.8f; vNewRefPos = -coreVector2(0.0f,1.0f).Rotated90 () * (1.4f * SQRT2); break;
            case 27u: vNewPos = vNewPos * -0.5f - vRota.Rotated135() * 0.8f; vNewRefPos = -coreVector2(0.0f,1.0f).Rotated135() * (1.4f * SQRT2); break;

            // fast infinity
            case 28u: vNewPos.x = (vNewPos.x + 1.8f) * -2.0f - 0.6f; vNewPos.y =  0.96f; vNewRefPos.x = vNewPos.x; vNewRefPos.y =  1.4f; break;
            case 29u: vNewPos.x = (vNewPos.x + 1.8f) *  2.0f - 0.6f; vNewPos.y =  0.72f; vNewRefPos.x = vNewPos.x; vNewRefPos.y =  1.4f; break;
            case 30u: vNewPos.x = (vNewPos.x + 1.8f) * -3.0f - 0.6f; vNewPos.y = -0.96f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = -1.4f; break;
            case 31u: vNewPos.x = (vNewPos.x + 1.8f) *  3.0f - 0.6f; vNewPos.y = -0.72f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = -1.4f; break;
            case 32u: vNewPos.x = (vNewPos.x + 1.8f) * -2.0f + 0.6f; vNewPos.y =  0.96f; vNewRefPos.x = vNewPos.x; vNewRefPos.y =  1.4f; break;
            case 33u: vNewPos.x = (vNewPos.x + 1.8f) *  2.0f + 0.6f; vNewPos.y =  0.72f; vNewRefPos.x = vNewPos.x; vNewRefPos.y =  1.4f; break;
            case 34u: vNewPos.x = (vNewPos.x + 1.8f) * -3.0f + 0.6f; vNewPos.y = -0.96f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = -1.4f; break;
            case 35u: vNewPos.x = (vNewPos.x + 1.8f) *  3.0f + 0.6f; vNewPos.y = -0.72f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = -1.4f; break;

            // wall
            case 36u: vNewPos = vNewPos * -0.85f + coreVector2(1.0f,0.0f) *  1.0f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = 1.4f; break;
            case 37u: vNewPos = vNewPos * -0.85f + coreVector2(1.0f,0.0f) *  0.6f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = 1.4f; break;
            case 38u: vNewPos = vNewPos * -0.85f + coreVector2(1.0f,0.0f) *  0.2f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = 1.4f; break;
            case 39u: vNewPos = vNewPos * -0.85f + coreVector2(1.0f,0.0f) * -0.2f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = 1.4f; break;
            case 40u: vNewPos = vNewPos * -0.85f + coreVector2(1.0f,0.0f) * -0.6f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = 1.4f; break;
            case 41u: vNewPos = vNewPos * -0.85f + coreVector2(1.0f,0.0f) * -1.0f; vNewRefPos.x = vNewPos.x; vNewRefPos.y = 1.4f; break;

            // hunters
            case 42u: vNewPos = vNewPos * -0.3f + coreVector2(0.0f, 1.0f) * 0.7f; vNewRefPos = coreVector2(1.0f, 1.0f) * 1.4f; break;
            case 43u: vNewPos = vNewPos * -0.3f + coreVector2(0.0f,-1.0f) * 0.7f; vNewRefPos = coreVector2(1.0f,-1.0f) * 1.4f; break;
            case 44u: vNewPos = vNewPos *  0.3f + coreVector2(0.0f, 1.0f) * 0.7f; vNewRefPos = coreVector2(1.0f, 1.0f) * 1.4f; break;
            case 45u: vNewPos = vNewPos *  0.3f + coreVector2(0.0f,-1.0f) * 0.7f; vNewRefPos = coreVector2(1.0f,-1.0f) * 1.4f; break;

            // prison
            case 46u: vNewPos = vNewPos * 0.5f + coreVector2(-0.24f,0.6f);             vNewRefPos = coreVector2(-1.0f, 1.0f) * 1.4f;                                 break;
            case 47u: vNewPos = vNewPos * 0.5f + coreVector2( 0.0f, 0.6f);             vNewRefPos = coreVector2( 0.5f, 1.0f) * 1.4f; vNewDir =  vNewDir.Rotated90(); break;
            case 48u: vNewPos = vNewPos * 0.5f + coreVector2( 0.24f,0.6f);             vNewRefPos = coreVector2( 1.0f, 1.0f) * 1.4f; vNewDir = -vNewDir;             break;
            case 49u: vNewPos = vNewPos * 0.5f + coreVector2(-0.24f,0.6f).Rotated90(); vNewRefPos = coreVector2( 1.0f, 1.0f) * 1.4f;                                 break;
            case 50u: vNewPos = vNewPos * 0.5f + coreVector2( 0.0f, 0.6f).Rotated90(); vNewRefPos = coreVector2( 1.0f,-0.5f) * 1.4f; vNewDir =  vNewDir.Rotated90(); break;
            case 51u: vNewPos = vNewPos * 0.5f + coreVector2( 0.24f,0.6f).Rotated90(); vNewRefPos = coreVector2( 1.0f,-1.0f) * 1.4f; vNewDir = -vNewDir;             break;
            case 52u: vNewPos = vNewPos * 0.5f - coreVector2(-0.24f,0.6f);             vNewRefPos = coreVector2( 1.0f,-1.0f) * 1.4f;                                 break;
            case 53u: vNewPos = vNewPos * 0.5f - coreVector2( 0.0f, 0.6f);             vNewRefPos = coreVector2(-0.5f,-1.0f) * 1.4f; vNewDir =  vNewDir.Rotated90(); break;
            case 54u: vNewPos = vNewPos * 0.5f - coreVector2( 0.24f,0.6f);             vNewRefPos = coreVector2(-1.0f,-1.0f) * 1.4f; vNewDir = -vNewDir;             break;
            case 55u: vNewPos = vNewPos * 0.5f - coreVector2(-0.24f,0.6f).Rotated90(); vNewRefPos = coreVector2(-1.0f,-1.0f) * 1.4f;                                 break;
            case 56u: vNewPos = vNewPos * 0.5f - coreVector2( 0.0f, 0.6f).Rotated90(); vNewRefPos = coreVector2(-1.0f, 0.5f) * 1.4f; vNewDir =  vNewDir.Rotated90(); break;
            case 57u: vNewPos = vNewPos * 0.5f - coreVector2( 0.24f,0.6f).Rotated90(); vNewRefPos = coreVector2(-1.0f, 1.0f) * 1.4f; vNewDir = -vNewDir;             break;
            }

            vNewPos = LERPB(vNewRefPos, vNewPos, MIN1(fLifeTime * 0.5f));

                 if(i >= 28u && i < 36u) vNewPos.x = FmodRange(vNewPos.x, -1.2f, 1.2f);
            else if(i >= 36u && i < 42u) vNewPos.y = LERP(1.25f, -1.25f, FRACT(MAX0(fLifeTime * 0.5f)));
            else if(i >= 42u && i < 46u) vNewPos.x = LERP(1.25f, -1.25f, FRACT(MAX0(fLifeTime * 0.6f - I_TO_F(i - 42u) * 0.25f)));

            pEnemy->SetPosition (coreVector3(vNewPos * FOREGROUND_AREA, 0.0f));
            pEnemy->SetDirection(coreVector3(vNewDir * -1.0f,           0.0f));

            if(!pEnemy->ReachedDeath() && STAGE_LIFETIME_AFTER(0.5f) && STAGE_TICK_LIFETIME(8.0f, 0.0f) && (!g_pGame->IsEasy() || (((s_iTick % 8u) >= 4u) && (i != 47u && i != 50u && i != 53u && i != 56u))))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                if(g_pForeground->IsVisiblePoint(vPos, 1.15f))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.15f, pEnemy, vPos, vDir)->ChangeSize(1.5f);

                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(g_pGame->IsTaskExtra())
            {
                constexpr coreUintW aiCrossOrder[] = {14u, 12u, 18u, 13u, 15u, 16u, 19u, 17u};

                if((i >= 12u && i < 20u) && (iCrossState < ARRAY_SIZE(aiCrossOrder)))
                {
                    if(i == aiCrossOrder[iCrossState])
                    {
                        if(!pEnemy->HasStatus(ENEMY_STATUS_CUSTOM))
                        {
                            pEnemy->SetBaseColor(COLOR_SHIP_BLACK, false, true);
                            pEnemy->AddStatus   (ENEMY_STATUS_CUSTOM);
                        }

                        g_pGame->GetCombatText()->AttachMarker(i % COMBAT_MARKERS, "X", pEnemy->GetPosition(), COLOR_MENU_INSIDE, false);
                    }

                    if(pEnemy->ReachedDeath())
                    {
                        if(i == aiCrossOrder[iCrossState])
                        {
                            if(++iCrossState >= ARRAY_SIZE(aiCrossOrder)) STAGE_BADGE(2u, BADGE_HARD, pEnemy->GetPosition())
                        }
                        else
                        {
                            STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
                            {
                                if(pEnemy2->HasStatus(ENEMY_STATUS_CUSTOM))
                                {
                                    pEnemy2->SetBaseColor(COLOR_SHIP_RED);
                                }
                            });

                            iCrossState = 0xFFu;
                        }
                    }
                }
            }

            if(i == iHelperIndex)
            {
                if(STAGE_TAKEOFF)
                {
                    pHelper->Resurrect(false);
                }
                else if(pEnemy->ReachedDeath())
                {
                    pHelper->Kill(false);
                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), 25.0f, 5u, COLOR_ENERGY_ORANGE);
                }

                pHelper->SetPosition(coreVector3(pEnemy->GetPosition().xy() + coreVector2(1.0f,-1.0f) * 6.2f, 0.0f));
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreVector2 vNewPos    = vBasePos;
            coreVector2 vNewRefPos = vBaseRefPos;

            switch(i)
            {
            default: UNREACHABLE
            case 0u: vNewPos = vNewPos * coreVector2(-1.0f,1.0f) + coreVector2( 1.0f, 1.0f) * 1.9f; vNewRefPos = coreVector2( 1.0f, 1.0f) * 3.0f; break;
            case 1u: vNewPos = vNewPos * coreVector2(-1.0f,1.0f) + coreVector2(-1.0f,-1.0f) * 1.9f; vNewRefPos = coreVector2(-1.0f,-1.0f) * 3.0f; break;
            case 2u: vNewPos = vNewPos * coreVector2( 1.0f,1.0f) + coreVector2(-1.0f, 0.0f) * 1.9f; vNewRefPos = coreVector2(-1.0f, 0.0f) * 3.0f; break;
            }

            vNewPos = LERPB(vNewRefPos, vNewPos, MIN1(fLifeTime * 0.7f));

            const coreVector2 vNewDir = coreVector2::Direction(fLifeTime * (2.0f*PI));

            pEnemy->SetPosition (coreVector3(vNewPos * FOREGROUND_AREA + vNewDir * 0.5f, 0.0f));   // # to remove the wobbling here, but should not be fixed in general
            pEnemy->SetDirection(coreVector3(vNewDir,                                    0.0f));

            if(pEnemy->ReachedDeath())
            {
                if(++iHiddenState >= pSquad2->GetNumEnemies())
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                }
                else
                {
                    g_pGame->GetCombatText()->DrawProgress(iHiddenState, pSquad2->GetNumEnemies(), pEnemy->GetPosition());
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iHiddenState, pSquad2->GetNumEnemies()), SOUND_ITEM_02);
                }
            }
        });

        g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](const cBullet* pBullet)
        {
            if(!SameDirection90(pBullet->GetFlyDir(), coreVector2(1.0f,0.0f)))
            {
                iNonRightShot += 1u;
                STAGE_FAILTROPHY
            }
        });

        if(STAGE_BEGINNING)
        {
            g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,1.0f), 30.0f);
        }

        STAGE_WAVE(4u, "1-5", {60.0f, 90.0f, 120.0f, 150.0f, 300.0f})   // FÜNF
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cConeBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < VIRIDO_BEANS; ++i)
            this->DisableBean(i, false);

        STAGE_FINISH_NOW
    });

#if defined(_P1_UNUSED_)

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
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
    // TODO 1: MAIN: task-check, helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    // TODO 1: enemies are big player small, then enemis are small player big
    STAGE_MAIN({TAKE_ALWAYS, 5u})
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

        STAGE_WAVE(5u, "1-?", {60.0f, 80.0f, 100.0f, 120.0f, 240.0f})   // SECHS
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMeteorEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cConeBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cWaveBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTriangleBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cFlipBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cViewBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pGame->KillHelpers();

        STAGE_FINISH_NOW
    });

#endif

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(0.0f,1.0f));

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_BOSS(m_Torus, {145.0f, 215.0f, 290.0, 360.0f, 720.0f})   // +5
    },
    STAGE_PRE()
    {
        g_pGame->PrefetchBoss();
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(!g_pGame->GetItemManager()->GetNumItems() && !g_pGame->GetInterface()->IsFragmentActive())
        {
            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // end
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_OUTRO)
    });

    // ################################################################
    // story state
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        ADD_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_VIRIDO)
        STAGE_FINISH_NOW
    });

    // ################################################################
    // ################################################################
}