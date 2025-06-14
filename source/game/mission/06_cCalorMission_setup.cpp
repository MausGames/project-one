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
// setup the Calor mission
void cCalorMission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cSnowBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

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
        cSnowBackground* pBackground = d_cast<cSnowBackground*>(g_pEnvironment->GetBackground());

        pBackground->GetOutdoor()->LerpHeightNow(-0.5f, -34.0f);
        pBackground->SetGroundDensity(0u, 1.0f);
        pBackground->SetGroundDensity(1u, 0.0f);
        pBackground->SetGroundDensity(2u, 0.0f);
        pBackground->SetAirDensity   (1u, 1.0f);

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
    // evade being attacked
    // - 2. group shows single-enemy split-up, extended in 3. group
    // - first groups are positioned a bit from center, to give player a safe space to navigate among unpredictable enemy-evasion movements
    // - evasion needs visual feedback, especially on multi-jumper
    // - directly evading enemies should evade diagonal, so evasion is valid from any attack-direction
    // - don't let enemies shoot while still at original position, otherwise will merge bullets especially on 3. group
    // - multi-jumper should not start at the cross from last position of path-enemy
    // - multi-jumper should not move together again after splitting up
    // - next groups need to spawn (source, target) where player would not attack from/into the previous group
    // - the clock group has the same angular speed to make it easier to control
    // - (old: in star group, everyone has their own path, only goes with an odd number of enemies)
    // - enemies move smooth, flakes move flat
    // TASK: collect all moving flakes
    // TASK: intercept every move by the jumping enemies
    // ACHIEVEMENT: destroy all enemies while your partner needs repair
    // TODO 1: hardmode: enemies return after some time
    // TODO 1: hardmode: enemy splits up, the original location attacks, the other keeps moving
    // TODO 1: implement own task-objects instead of re-using hails, also adjust count again (CALOR_HAILS) (name: flake?)
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        constexpr coreUintW iNumData  = 16u;
        constexpr coreUintW iMaxTrail = 12u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(21u);
            pPath1->AddNode(coreVector2( 0.0f, 0.0f), coreVector2( 1.0f, 1.0f).Normalized());
            pPath1->AddNode(coreVector2( 0.4f, 0.4f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.1f, 0.8f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.3f, 0.4f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.6f, 0.1f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.9f, 0.4f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2(-0.6f, 0.7f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.3f, 0.4f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.7f,-0.6f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.5f,-0.8f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.0f,-0.5f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.5f,-0.8f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.7f,-0.3f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.4f, 0.0f), coreVector2(-1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.1f,-0.3f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-0.6f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.7f,-0.3f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.2f, 0.5f), coreVector2( 0.0f, 1.0f));
            pPath1->AddNode(coreVector2( 0.5f, 0.8f), coreVector2( 1.0f, 0.0f));
            pPath1->AddNode(coreVector2( 0.8f, 0.5f), coreVector2( 0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.0f, 0.0f), coreVector2(-1.0f,-1.0f).Normalized());
            pPath1->Refine();
            pPath1->Refine();   // double
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(3u);
            pPath2->AddNode(coreVector2(-0.8f,-1.3f), coreVector2(0.0f, 1.0f));
            pPath2->AddNode(coreVector2( 0.0f, 0.8f), coreVector2(1.0f, 0.0f));
            pPath2->AddNode(coreVector2( 0.8f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 81u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.7f);
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            });
        });

        STAGE_GET_START(iNumData * 3u + 8u)
            STAGE_GET_FLOAT_ARRAY(afRecover,    iNumData)
            STAGE_GET_FLOAT_ARRAY(afRecoverOld, iNumData)
            STAGE_GET_UINT_ARRAY (aiRecoverHit, iNumData)
            STAGE_GET_FLOAT      (fShootTime)
            STAGE_GET_UINT       (iTrail)
            STAGE_GET_UINT       (iTrailOld)
            STAGE_GET_UINT       (iHelperState)
            STAGE_GET_UINT       (iIntercept)
            STAGE_GET_UINT       (iUnrepaired)
            STAGE_GET_UINT       (iHailState)
            STAGE_GET_UINT       (iHailEffect)
        STAGE_GET_END

        ASSERT(pSquad1->GetNumEnemiesAlive() <= iNumData)

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 16u, 31u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 32u, 43u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 44u, 55u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 56u, 64u)   // skip 6
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 65u, 70u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 71u, 76u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 77u, 80u)
            else
            {
                if(!iUnrepaired) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
            }

            if(g_pGame->IsTask())
            {
                switch(m_iStageSub)
                {
                case 5u: this->EnableHail(0u); break;
                case 7u: this->EnableHail(1u); break;
                case 8u: this->EnableHail(2u); break;
                case 9u: this->EnableHail(3u); this->EnableHail(4u); break;
                }
            }

            std::memset(afRecover,    0, sizeof(coreFloat)  * iNumData);
            std::memset(afRecoverOld, 0, sizeof(coreFloat)  * iNumData);
            std::memset(aiRecoverHit, 0, sizeof(coreUint32) * iNumData);

            fShootTime = 0.0f;
        }

        const coreFloat fOldShootTime = fShootTime;
        if(std::any_of(afRecover, afRecover + iNumData, [](const coreFloat A) {return (A != 0.0f);}))
        {
            fShootTime += 1.0f * TIME;
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreBool bActive = false;

            coreFloat&  fRecover    = STAGE_SINK_FLOAT(afRecover   [i % iNumData]);
            coreUint32& iRecoverHit = STAGE_SINK_UINT (aiRecoverHit[i % iNumData]);

            coreVector2 vStart, vLocation, vEvade;
            if(i < 44u)
            {
                switch(i)
                {
                default: UNREACHABLE
                case  0u: vStart = coreVector2(-1.3f, 0.2f); vLocation = coreVector2(-0.4f, 0.3f) + coreVector2( 0.1f, 0.1f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case  1u: vStart = coreVector2(-1.3f, 0.2f); vLocation = coreVector2(-0.4f, 0.3f) + coreVector2( 0.1f,-0.1f); vEvade = coreVector2( 0.2f,-0.2f); break;
                case  2u: vStart = coreVector2(-1.3f, 0.2f); vLocation = coreVector2(-0.4f, 0.3f) + coreVector2(-0.1f, 0.1f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case  3u: vStart = coreVector2(-1.3f, 0.2f); vLocation = coreVector2(-0.4f, 0.3f) + coreVector2(-0.1f,-0.1f); vEvade = coreVector2(-0.2f,-0.2f); break;
                case  4u: vStart = coreVector2( 1.3f, 0.2f); vLocation = coreVector2( 0.4f, 0.3f) + coreVector2( 0.1f, 0.1f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case  5u: vStart = coreVector2( 1.3f, 0.2f); vLocation = coreVector2( 0.4f, 0.3f) + coreVector2( 0.1f,-0.1f); vEvade = coreVector2( 0.2f,-0.2f); break;
                case  6u: vStart = coreVector2( 1.3f, 0.2f); vLocation = coreVector2( 0.4f, 0.3f) + coreVector2(-0.1f, 0.1f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case  7u: vStart = coreVector2( 1.3f, 0.2f); vLocation = coreVector2( 0.4f, 0.3f) + coreVector2(-0.1f,-0.1f); vEvade = coreVector2(-0.2f,-0.2f); break;

                case  8u: vStart = coreVector2(-1.3f,-0.8f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2(-0.3f,-0.3f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case  9u: vStart = coreVector2(-1.3f,-0.8f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2(-0.3f,-0.3f); vEvade = coreVector2( 0.2f,-0.2f); break;
                case 10u: vStart = coreVector2(-1.3f, 1.3f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2(-0.3f, 0.3f); vEvade = coreVector2(-0.2f,-0.2f); break;
                case 11u: vStart = coreVector2(-1.3f, 1.3f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2(-0.3f, 0.3f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case 12u: vStart = coreVector2( 1.3f,-0.8f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2( 0.3f,-0.3f); vEvade = coreVector2(-0.2f,-0.2f); break;
                case 13u: vStart = coreVector2( 1.3f,-0.8f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2( 0.3f,-0.3f); vEvade = coreVector2( 0.2f, 0.2f); break;
                case 14u: vStart = coreVector2( 1.3f, 1.3f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2( 0.3f, 0.3f); vEvade = coreVector2(-0.2f, 0.2f); break;
                case 15u: vStart = coreVector2( 1.3f, 1.3f); vLocation = coreVector2( 0.0f, 0.3f) + coreVector2( 0.3f, 0.3f); vEvade = coreVector2( 0.2f,-0.2f); break;

                case 16u: vStart = coreVector2(-1.9f, 0.0f); vLocation = coreVector2(-0.9f, 0.0f);                            vEvade = coreVector2( 0.3f, 0.0f); break;
                case 17u: vStart = coreVector2(-1.9f, 0.0f); vLocation = coreVector2(-0.9f, 0.0f);                            vEvade = coreVector2( 0.6f, 0.0f); break;
                case 18u: vStart = coreVector2(-1.9f, 0.0f); vLocation = coreVector2(-0.9f, 0.0f);                            vEvade = coreVector2( 0.9f, 0.0f); break;
                case 19u: vStart = coreVector2(-1.9f, 0.0f); vLocation = coreVector2(-0.9f, 0.0f);                            vEvade = coreVector2( 1.2f, 0.0f); break;
                case 20u: vStart = coreVector2( 1.3f, 0.3f); vLocation = coreVector2( 0.3f, 0.3f);                            vEvade = coreVector2(-0.3f, 0.0f); break;
                case 21u: vStart = coreVector2( 1.3f, 0.3f); vLocation = coreVector2( 0.3f, 0.3f);                            vEvade = coreVector2(-0.6f, 0.0f); break;
                case 22u: vStart = coreVector2( 1.3f, 0.3f); vLocation = coreVector2( 0.3f, 0.3f);                            vEvade = coreVector2(-0.9f, 0.0f); break;
                case 23u: vStart = coreVector2( 1.3f, 0.3f); vLocation = coreVector2( 0.3f, 0.3f);                            vEvade = coreVector2(-1.2f, 0.0f); break;
                case 24u: vStart = coreVector2(-1.3f, 0.6f); vLocation = coreVector2(-0.3f, 0.6f);                            vEvade = coreVector2( 0.3f, 0.0f); break;
                case 25u: vStart = coreVector2(-1.3f, 0.6f); vLocation = coreVector2(-0.3f, 0.6f);                            vEvade = coreVector2( 0.6f, 0.0f); break;
                case 26u: vStart = coreVector2(-1.3f, 0.6f); vLocation = coreVector2(-0.3f, 0.6f);                            vEvade = coreVector2( 0.9f, 0.0f); break;
                case 27u: vStart = coreVector2(-1.3f, 0.6f); vLocation = coreVector2(-0.3f, 0.6f);                            vEvade = coreVector2( 1.2f, 0.0f); break;
                case 28u: vStart = coreVector2( 1.9f, 0.9f); vLocation = coreVector2( 0.9f, 0.9f);                            vEvade = coreVector2(-0.3f, 0.0f); break;
                case 29u: vStart = coreVector2( 1.9f, 0.9f); vLocation = coreVector2( 0.9f, 0.9f);                            vEvade = coreVector2(-0.6f, 0.0f); break;
                case 30u: vStart = coreVector2( 1.9f, 0.9f); vLocation = coreVector2( 0.9f, 0.9f);                            vEvade = coreVector2(-0.9f, 0.0f); break;
                case 31u: vStart = coreVector2( 1.9f, 0.9f); vLocation = coreVector2( 0.9f, 0.9f);                            vEvade = coreVector2(-1.2f, 0.0f); break;

                case 32u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2(-0.3f, 0.3f); break;
                case 33u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2(-0.4f, 0.0f); break;
                case 34u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2(-0.3f,-0.3f); break;
                case 35u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2( 0.3f, 0.3f); break;
                case 36u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2( 0.4f, 0.0f); break;
                case 37u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2( 0.3f,-0.3f); break;
                case 38u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2(-0.8f, 0.3f); break;
                case 39u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2(-0.9f, 0.0f); break;
                case 40u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2(-0.8f,-0.3f); break;
                case 41u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2( 0.8f, 0.3f); break;
                case 42u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2( 0.9f, 0.0f); break;
                case 43u: vStart = coreVector2( 0.0f, 1.3f); vLocation = coreVector2( 0.0f, 0.6f);                            vEvade = coreVector2( 0.8f,-0.3f); break;
                }
            }
            else if(i < 77u)
            {
                coreFloat fFactor = 0.0f;
                if(i < 56u)
                {
                    const coreFloat   fSide   = (i < 50u) ? -1.0f : 1.0f;
                    const coreFloat   fHeight = (I_TO_F((i - 44u) % 6u) - 1.5f) * 0.2f;
                    const coreVector2 vFrom   = coreVector2(((i % 2u) ?  0.85f :  0.95f) * fSide, fHeight);
                    const coreVector2 vTo     = coreVector2(((i % 2u) ? -0.95f : -0.85f) * fSide, fHeight);

                    fFactor   = MIN1(fRecover * RCP((vFrom - vTo).Length()));
                    vStart    = coreVector2(1.3f * fSide, fHeight);
                    vLocation = LERP(vFrom, vTo, fFactor);
                }
                else if(i < 65u)
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(i - 56u) * (1.0f/9.0f) * (2.0f*PI));

                    fFactor   = MIN1(fRecover * 0.15f);
                    vStart    = vDir * (1.3f * SQRT2);
                    vLocation = MapToAxis(vDir * LERP(0.3f, 0.6f, fFactor), coreVector2::Direction(LERP(0.0f*PI, 4.0f*PI, fFactor)));

                    if((i == 56u) && !pEnemy->HasStatus(ENEMY_STATUS_DEAD)) g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP(0.0f*PI, 1.0f*PI, fFactor)), 1.0f);
                }
                else if(i < 71u)
                {
                    const coreFloat fHeight = (I_TO_F(i - 65u) + 0.5f) * 0.2f;

                    fFactor   = MIN1(fRecover * 0.2f);
                    vStart    = coreVector2(1.3f,1.3f);
                    vLocation = MapToAxis(coreVector2(0.0f, fHeight), coreVector2::Direction(LERP(-1.25f*PI, -4.0f*PI, fFactor)));

                    if((i == 65u) && !pEnemy->HasStatus(ENEMY_STATUS_DEAD)) g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP(1.0f*PI, 2.0f*PI, fFactor)), 1.0f);
                }
                else
                {
                    fFactor   = CLAMP01((fRecover - I_TO_F(i - 71u) * 0.2f) * RCP(pPath1->GetTotalDistance()));
                    vStart    = coreVector2(-1.3f,0.0f);
                    vLocation = pPath1->CalcPositionLerp(fFactor);

                    if((i == 71u) && !pEnemy->HasStatus(ENEMY_STATUS_DEAD)) g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERP(0.0f*PI, 2.0f*PI, fFactor)), 1.0f);
                }

                if(fFactor >= 1.0f) bActive = true;

                vEvade = coreVector2(0.0f,0.0f);
            }
            else
            {
                     if(iTrailOld == 0u)        vLocation = coreVector2(-0.5f,0.9f);
                else if(iTrailOld >= iMaxTrail) vLocation = coreVector2( 0.0f,0.0f);
                else
                {
                         if((i == 78u)             && (iTrailOld >= 6u)) vLocation = coreVector2::Direction(I_TO_F(iTrailOld) * (3.0f/7.0f) * (-2.0f*PI) + 1.5f * 0.3f) * 0.9f;
                    else if((i == 80u)             && (iTrailOld >= 6u)) vLocation = coreVector2::Direction(I_TO_F(iTrailOld) * (3.0f/7.0f) * (-2.0f*PI) - 1.5f * 0.3f) * 0.9f;
                    else if((i == 79u || i == 80u) && (iTrailOld >= 3u)) vLocation = coreVector2::Direction(I_TO_F(iTrailOld) * (3.0f/7.0f) * (-2.0f*PI) + 0.5f * 0.3f) * 0.9f;
                    else                                                 vLocation = coreVector2::Direction(I_TO_F(iTrailOld) * (3.0f/7.0f) * (-2.0f*PI) - 0.5f * 0.3f) * 0.9f;
                }

                vStart = coreVector2(-0.5f,1.3f);
                vEvade = coreVector2( 0.0f,0.0f);
            }

            if(STAGE_TAKEOFF)
            {
                pEnemy->SetPosition(coreVector3(vStart * FOREGROUND_AREA, 0.0f));
            }

            iRecoverHit = 0u;
            if(STAGE_LIFETIME_AFTER(0.5f))
            {
                g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](const cBullet* pBullet)
                {
                    const coreVector2 vDiff1 = pEnemy->GetPosition().xy() - (pBullet->GetPosition().xy() + pBullet->GetFlyMove());
                    const coreVector2 vDiff2 = pEnemy->GetPosition().xy() - (pBullet->GetPosition().xy());
                    const coreVector2 vDiff3 = pEnemy->GetPosition().xy() - (pBullet->GetPosition().xy() - pBullet->GetFlyMove());

                    if((vDiff1.LengthSq() < POW2(10.0f)) ||
                       (vDiff2.LengthSq() < POW2(10.0f)) ||
                       (vDiff3.LengthSq() < POW2(10.0f)))
                    {
                        cPlayer* pPlayer = d_cast<cPlayer*>(pBullet->GetOwner());

                        ADD_BIT(iRecoverHit, g_pGame->GetPlayerIndex(pPlayer))

                        if(pEnemy->HasStatus(ENEMY_STATUS_GHOST)) pPlayer->GetScoreTable()->RefreshCooldown();
                    }
                });
            }

            const coreFloat fDistSq = (pEnemy->GetPosition().xy() - vLocation * FOREGROUND_AREA).LengthSq();
            if(i < 44u)
            {
                bActive = (fDistSq > (vEvade * FOREGROUND_AREA).LengthSq() - POW2(10.0f));

                if(iRecoverHit)
                {
                    fRecover = 1.0f;
                }

                if(fRecover && (fDistSq > POW2((m_iStageSub >= 3u) ? 20.0f : 4.0f)))
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, 1.7f, 0.0f) && (!g_pGame->IsEasy() || ((s_iTick % 3u) < 1u)))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.7f);

                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
            }
            else if(i < 77u)
            {
                if(fRecover)
                {
                    if(STAGE_TICK_EXTERN(fShootTime, fOldShootTime, 1.7f, 0.0f) && (!g_pGame->IsEasy() || ((s_iTick % 3u) < 1u)))
                    {
                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = pEnemy->AimAtPlayerDual(s_iTick % 2u).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.7f);

                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                }
            }
            else
            {
                bActive = (iTrail >= iMaxTrail) && (fDistSq < POW2(2.0f));

                if(iRecoverHit && !bActive && (fDistSq < POW2(2.0f)) && (iTrail == iTrailOld))
                {
                    iTrail += 1u;
                }
            }

            if(STAGE_LIFETIME_AFTER(0.5f) && bActive)
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);
                }
            }
            else
            {
                if(!pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_GREY);
                    pEnemy->AddStatus   (ENEMY_STATUS_GHOST);
                }
            }

            const coreVector2 vTarget = fRecover ? (vLocation + vEvade) : vLocation;

            pEnemy->DefaultMoveSmooth(vTarget, 120.0f, 12.0f);

            if(g_pGame->IsTask() && (i >= 77u))
            {
                if(iTrail && !HAS_BIT(iIntercept, iTrail) && (fDistSq >= POW2(10.0f)))
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - pPlayer->GetPosition().xy();
                        if(vDiff.LengthSq() < POW2(9.0f))
                        {
                            ADD_BIT(iIntercept, iTrail)
                            const coreUint32 iCount = coreMath::PopCount(iIntercept);

                            if(iCount >= iMaxTrail)
                            {
                                STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
                            }
                            else
                            {
                                g_pGame->GetCombatText()->DrawCountdown(coreMath::PopCount(iIntercept), iMaxTrail, pEnemy->GetPosition());
                                g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iCount, iMaxTrail), SOUND_ITEM_01);
                            }

                            g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_YELLOW);
                        }
                    });
                }
            }

            if(pEnemy->ReachedDeath())
            {
                if(!g_pGame->GetRepairEnemy()->GetPlayer())
                {
                    iUnrepaired += 1u;
                    STAGE_FAILTROPHY
                }
            }
        });

        if((m_iStageSub >= 5u) && (m_iStageSub < 10u))
        {
            coreUintW iSplit, iOffset;
            if(m_iStageSub == 5u)
            {
                iSplit  = 2u;
                iOffset = 2u;
            }
            else
            {
                iSplit  = 1u;
                iOffset = 0u;
            }

            const coreUintW iStep = iNumData / iSplit;

            for(coreUintW i = 0u; i < iSplit; ++i)
            {
                const coreUintW iIndex = i * iStep + iOffset;

                coreFloat fMax = 0.0f;
                for(coreUintW j = 0u; j < iStep; ++j) fMax = MAX(fMax, afRecover[(iIndex + j) % iNumData]);

                coreUint32 iAll = 0u;
                for(coreUintW j = 0u; j < iStep; ++j) iAll |= aiRecoverHit[(iIndex + j) % iNumData];

                fMax += 1.0f * TIME * I_TO_F(coreMath::PopCount(iAll)) * RCP(I_TO_F(g_pGame->GetNumPlayers()));

                for(coreUintW j = 0u; j < iStep; ++j) afRecover[(iIndex + j) % iNumData] = fMax;
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            coreFloat& fRecover    = STAGE_SINK_FLOAT(afRecover   [i % iNumData]);
            coreFloat& fRecoverOld = STAGE_SINK_FLOAT(afRecoverOld[i % iNumData]);

            if(InBetween(0.0f, fRecoverOld, fRecover))
            {
                g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), 10.0f, 3u, COLOR_ENERGY_GREEN);
            }

            if(iTrailOld != iTrail)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 40u; j--; )
                {
                    if(((j + (i - 77u) * 5u) % 20u) < (g_pGame->IsEasy() ? 18u : 17u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f - 2.25f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.7f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, -vDir)->ChangeSize(1.7f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 5u, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }

            fRecoverOld = fRecover;
        });

        iTrailOld = iTrail;

        if(g_pGame->IsTask())
        {
            for(coreUintW i = 0u; i < CALOR_HAILS; ++i)
            {
                coreObject3D* pHail = this->GetHail(i);
                if(!pHail->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                coreFloat fTime;
                switch(i)
                {
                default: UNREACHABLE
                case 0u: fTime = STEP(0.0f,  2.0f, afRecover[50u % iNumData]); break;
                case 1u: fTime = STEP(2.0f,  4.0f, afRecover[56u % iNumData]); break;
                case 2u: fTime = STEP(2.0f,  4.0f, afRecover[65u % iNumData]); break;
                case 3u: fTime = STEP(1.0f,  5.0f, afRecover[71u % iNumData]); break;
                case 4u: fTime = STEP(7.0f, 11.0f, afRecover[71u % iNumData]); break;
                }

                coreVector2 vPos;
                switch(i)
                {
                default: UNREACHABLE
                case 0u: vPos = LERP(coreVector2(-1.2f,-0.8f), coreVector2( 1.2f,-0.8f), fTime); break;
                case 1u: vPos = LERP(coreVector2( 0.5f, 1.2f), coreVector2(-0.5f,-1.2f), fTime); break;
                case 2u: vPos = coreVector2::Direction(LERP(LERP(-1.25f*PI, -4.0f*PI, 2.0f/5.0f), LERP(-1.25f*PI, -4.0f*PI, 4.0f/5.0f) + (2.0f*PI), fTime)) * 0.5f; break;
                case 3u: vPos = pPath2->CalcPositionLerp(fTime).Rotated90();                     break;
                case 4u: vPos = pPath2->CalcPositionLerp(fTime).Rotated90().InvertedX();         break;
                }

                     if(fTime >= 1.0f) this->DisableHail(i, true);
                else if(fTime <= 0.0f) vPos = HIDDEN_POS;

                pHail->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));

                if(fTime && !HAS_BIT(iHailEffect, i))
                {
                    ADD_BIT(iHailEffect, i)
                    g_pSpecialEffects->CreateSplashColor(pHail->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_YELLOW);
                }

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pHail->GetPosition().xy() - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(6.0f))
                    {
                        this->DisableHail(i, true);

                        if(++iHailState >= 5u)
                        {
                            STAGE_BADGE(0u, BADGE_EASY, pHail->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawProgress(iHailState, 5u, pHail->GetPosition());
                            g_pSpecialEffects->PlaySound(pHail->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iHailState, 5u), SOUND_ITEM_02);
                        }
                    }
                });
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_ORANGE);

        if(m_iStageSub == 4u)
        {
            const coreUintW iIndex = 32u;

            if(!iHelperState && afRecover[iIndex % iNumData])
            {
                iHelperState = 1u;

                pHelper->Resurrect(false);
                pHelper->SetPosition(pSquad1->GetEnemy(iIndex)->GetPosition());
            }
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vTarget = coreVector2(0.0f,1.3f) * FOREGROUND_AREA;

            if(pHelper->DefaultMoveSmooth(vTarget, 120.0f, 12.0f))
            {
                pHelper->Kill(false);
            }
        }

        STAGE_WAVE(0u, "6-1", {50.0f, 75.0f, 100.0f, 125.0f, 250.0f})   // EINUNDDREISSIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cWaveBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < CALOR_HAILS; ++i)
            this->DisableHail(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // snowstorm makes the windscreen dirty
    // - player will avoid snow after explosion, it will drive players away
    // - player can fly above the first group, to assume a more aggressive position
    // - enemy creating snow while moving makes him nearly immune, even with offset or timed creation, it also does not fit to the rest and adds no pressure
    // - low health, enemies are protected by snow
    // - 1-4: show bullet interaction
    // - 5-6: show slow down
    // - 7: ramp up, tease invert
    // - 8: invert
    // - creating bullets from walls or snow did not feel good, it was just back-and-forth for the player
    // - creating snow without action (drops, avalanche, etc.) felt disconnected and random
    // - hiding enemies (completely) in the snow does not fit to the rest of the mechanic
    // - enemy at the bottom shows invert-blast, shows that not every enemy attacks, and adds a pause
    // - after some time, snow gets overwhelming and you cannot see changes anymore, so the invert-mechanic was able to help on that
    // - enemies protect each other with the snow they create
    // TASK: destroy all enemies under the snow
    // TASK: get stuck in snow
    // TASK EXTRA: collect all snowing objects
    // ACHIEVEMENT: clean the whole screen before destroying the last enemy
    // TODO 1: hardmode: player is slipping
    // TODO 1: hardmode: snow can freeze player
    // TODO 1: hardmode: snow is created all the time (snowing, avalanche)
    // TODO 1: adjust (and fix!) snow for all weapons [RP]
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cFreezerEnemy, 35u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.1f);
                pEnemy->Configure(10, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cFreezerEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.1f);
                pEnemy->Configure(10, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_BOTTOM | ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_WORTHLESS);
            });

            const auto nInitFunc = [&](const coreUintW iIndex, const coreVector2 vPos, const coreVector2 vDir)
            {
                pSquad2->GetEnemy(iIndex)->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pSquad2->GetEnemy(iIndex)->SetDirection(coreVector3(vDir,                   0.0f));
            };

            nInitFunc(0u, coreVector2( 1.0f,-3.0f) * 0.25f, coreVector2(-1.0f, 0.0f));
            nInitFunc(1u, coreVector2( 3.0f, 1.0f) * 0.25f, coreVector2( 0.0f,-1.0f));
            nInitFunc(2u, coreVector2(-3.0f, 1.0f) * 0.25f, coreVector2( 1.0f, 0.0f));
        });

        STAGE_GET_START(4u + CALOR_HAILS)
            STAGE_GET_UINT       (iExplosions)
            STAGE_GET_UINT       (iHiddenCount)
            STAGE_GET_UINT       (iHailCreate)
            STAGE_GET_UINT       (iHailState)
            STAGE_GET_FLOAT_ARRAY(afHailTime, CALOR_HAILS)
        STAGE_GET_END

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  8u, 10u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 11u, 13u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 34u, 34u)   // #
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 14u, 14u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 16u, 17u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 18u, 18u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 19u, 19u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 20u, 21u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 22u, 25u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 26u, 33u)
            else if(STAGE_SUB(18u))
            {
                this->DisableSnow(true);
                if(!m_Snow.AnyData()) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())   // snow not yet inverted
            }

            if(m_iStageSub == 16u)
            {
                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
            }

            if(g_pGame->IsTask())
            {
                if(m_iStageSub == 8u)
                {
                    STAGE_RESURRECT(pSquad2, 0u, 2u)
                }
            }

            if(g_pGame->IsTaskExtra())
            {
                if((m_iStageSub == 3u) || (m_iStageSub == 6u) || (m_iStageSub == 11u) || (m_iStageSub == 14u) || (m_iStageSub == 17u))
                {
                    if(++iHailCreate >= CALOR_HAILS) iHailCreate = 0u;

                    this->EnableHail(iHailCreate);
                    this->GetHail(iHailCreate)->SetPosition(coreVector3(0.0f,1.3f,0.0f) * FOREGROUND_AREA3);
                }
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_BLUE);

        if((m_iStageSub == 17u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
            pHelper->SetPosition(coreVector3(0.0f,1.3f,0.0f) * FOREGROUND_AREA3);
        }

        if(STAGE_BEGINNING)
        {
            this->EnableSnow();
        }

        const auto nLineFunc = [this](const coreVector2 vPosition, const coreFloat fSize, const coreBool bHorizontal, const eSnowType eType)
        {
            m_Snow.DrawLine(vPosition, fSize, bHorizontal, eType);

            if(eType)
            {
                g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    const coreVector2 vDiff = pBullet->GetPosition().xy() - vPosition;
                    if(ABS(vDiff.arr(bHorizontal)) < fSize) pBullet->Deactivate(true);
                });
            }
        };

        const auto nPointFunc = [this](const coreVector2 vPosition, const coreFloat fSize, const eSnowType eType)
        {
            m_Snow.DrawPoint(vPosition, fSize, eType);

            if(eType)
            {
                g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    const coreVector2 vDiff = pBullet->GetPosition().xy() - vPosition;
                    if(vDiff.LengthSq() < POW2(fSize)) pBullet->Deactivate(true);
                });
            }
        };

        const auto nAllFunc = [this](const eSnowType eType)
        {
            m_Snow.DrawAll(eType);
            if(eType != SNOW_TYPE_REMOVE) g_pGame->GetBulletManagerPlayer()->ClearBullets(true);
            if(eType == SNOW_TYPE_ADD)    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
        };

        const coreFloat fHailSpeed = g_pGame->IsEasy() ? 0.4f : 0.7f;

        if((m_iStageSub >= 16u) && (m_iStageSub < 18u))
        {
            if((STAGE_SUBTIME_AFTER(0.5f) || (m_iStageSub >= 17u)) && STAGE_TICK_FREE(12.0f * fHailSpeed, 0.0f))
            {
                const coreVector2 vPos = coreVector2((I_TO_F((s_iTick * 3u) % 11u) - 5.0f) / 5.0f * -1.0f, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                const coreInt32 iDamage = s_iTick + 1;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(iDamage, 1.1f * fHailSpeed, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.7f);
            }

            g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                const coreFloat   fOffset = SIN(pBullet->GetFlyTime() * fHailSpeed + I_TO_F(pBullet->GetDamage()) * GA);
                const coreVector2 vDir    = coreVector2(fOffset, -1.5f).Normalized();

                pBullet->SetFlyDir(vDir);
            });
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreFloat   fOffset = COS(pHelper->GetLifeTime() * fHailSpeed * 2.0f);
            const coreVector2 vDir    = coreVector2(fOffset, -1.5f).Normalized();
            const coreVector2 vPos    = pHelper->GetPosition().xy() + vDir * (1.1f * fHailSpeed * BULLET_SPEED_FACTOR * TIME);

            if(vPos.y < -FOREGROUND_AREA.y * 1.3f) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        if(g_pGame->IsTask())
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(m_afSnowStuck[i] >= 1.0f)
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, pPlayer->GetPosition())
                }
            });
        }

        if(g_pGame->IsTaskExtra())
        {
            for(coreUintW i = 0u; i < CALOR_HAILS; ++i)
            {
                coreObject3D* pHail = this->GetHail(i);
                if(!pHail->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                afHailTime[i] += 1.0f * TIME;

                const coreFloat   fOffset = SIN(afHailTime[i] * fHailSpeed + I_TO_F(i + 2u) * -GA);
                const coreVector2 vDir    = coreVector2(fOffset, -1.5f).Normalized();
                const coreVector2 vPos    = pHail->GetPosition().xy() + vDir * (1.1f * fHailSpeed * BULLET_SPEED_FACTOR * TIME);

                if(vPos.y < -FOREGROUND_AREA.y * 1.3f) this->DisableHail(i, false);

                pHail->SetPosition(coreVector3(vPos, 0.0f));

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    const coreVector2 vDiff = pHail->GetPosition().xy() - pPlayer->GetPosition().xy();
                    if(vDiff.LengthSq() < POW2(5.0f))
                    {
                        this->DisableHail(i, true);

                        if(++iHailState >= 5u)
                        {
                            STAGE_BADGE(2u, BADGE_HARD, pHail->GetPosition())
                        }
                        else
                        {
                            g_pGame->GetCombatText()->DrawProgress(iHailState, 5u, pHail->GetPosition());
                            g_pSpecialEffects->PlaySound(pHail->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iHailState, 5u), SOUND_ITEM_01);
                        }
                    }
                });
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if(i < 14u)
            {
                STAGE_LIFETIME(pEnemy, 1.0f, 0.5f * ((i < 4u) ? 0.0f : ((i < 8u) ? I_TO_F((i - 4u) % 2u) : I_TO_F((i - 8u) % 3u))) + ((i >= 11u && i < 14u) ? 4.0f : 0.0f))

                const coreFloat fValue = 0.5f + 0.5f * SIN(MAX(fLifeTime, 0.0f) + 0.5f*PI);

                if(i < 4u)
                {
                    pEnemy->SetPosition(coreVector3(((i < 2u) ? 0.6f : 0.2f) * ((i % 2u) ? 1.0f : -1.0f), LERP(-0.8f, 1.3f, fValue), 0.0f) * FOREGROUND_AREA3);

                    if(pEnemy->ReachedDeath())
                    {
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, true, SNOW_TYPE_ADD);
                    }
                }
                else if(i < 8u)
                {
                    pEnemy->SetPosition(coreVector3(LERP(-0.8f, 1.3f, fValue) * ((i < 6u) ? 1.0f : -1.0f), ((i - 4u) % 2u) ? 0.4f : 0.8f, 0.0f) * FOREGROUND_AREA3);

                    if(pEnemy->ReachedDeath())
                    {
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, false, SNOW_TYPE_ADD);
                    }
                }
                else
                {
                    pEnemy->SetPosition(coreVector3((I_TO_F((i - 8u) % 3u) - 1.0f) * ((i < 11u) ? 0.6f : -0.6f), LERP(0.0f, 1.3f, fValue), 0.0f) * FOREGROUND_AREA3);

                    if(pEnemy->ReachedDeath())
                    {
                        if(++iExplosions == 3u)
                        {
                            nAllFunc(SNOW_TYPE_ADD);

                            cSnowBackground* pBackground = d_cast<cSnowBackground*>(g_pEnvironment->GetBackground());

                            pBackground->GetOutdoor()->LerpHeightNow(1.0f, 0.0f);
                            pBackground->SetGroundDensity(0u, 0.0f, true);
                            pBackground->SetGroundDensity(1u, 1.0f, true);
                            pBackground->SetGroundDensity(2u, 1.0f, true);
                            pBackground->SetAirDensity   (1u, 0.0f, true);

                            g_pEnvironment->SetTargetDirectionNow(coreVector2(0.0f,-1.0f));
                        }
                        else
                        {
                            nPointFunc(pEnemy->GetPosition().xy(), 15.0f, SNOW_TYPE_REMOVE);
                        }
                    }
                }

                pEnemy->DefaultRotate(fLifeTime * 3.0f);

                if(STAGE_TICK_LIFETIME(60.0f, 0.0f) && ((s_iTick % ((i < 8u) ? 16u : 8u)) < (g_pGame->IsEasy() ? 2u : 4u)))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.7f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_BLUE);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else if(i < 22u)
            {
                STAGE_LIFETIME(pEnemy, (i < 18u) ? 0.7f : 1.4f, (i < 18u) ? ((i == 17u) ? 0.5f : 0.1f) : 0.0f)

                if(i < 18u)
                {
                    const coreFloat X = 0.9f * SIN(fLifeTime + I_TO_F(((i - 14u) * 2u + 1u) % 5u) * (0.5f*PI));
                    const coreFloat Y = FmodRange(1.3f - MAX0(fLifeTime), -1.3f, 1.3f);

                    pEnemy->SetPosition(coreVector3(X, Y, 0.0f) * FOREGROUND_AREA3);
                }
                else
                {
                    const coreFloat   fSide = (i < 20u) ? -1.0f : 1.0f;
                    const coreVector2 vDir  = coreVector2::Direction((fLifeTime - I_TO_F((i - 18u) % 2u) * (1.0f*PI)) * fSide);
                    const coreFloat   fLen  = LERPB(1.3f, 0.6f, MIN1(fLifeTime * 0.1f)) * FOREGROUND_AREA.x;

                    pEnemy->SetPosition(coreVector3(vDir * fLen, 0.0f));
                }

                if(pEnemy->ReachedDeath())
                {
                    nPointFunc(pEnemy->GetPosition().xy(), 30.0f, SNOW_TYPE_INVERT);
                }

                pEnemy->DefaultRotate(fLifeTime * 3.0f);

                if(STAGE_TICK_LIFETIME(60.0f / fLifeSpeed, 0.0f) && ((s_iTick % 8u) < (g_pGame->IsEasy() ? 2u : 4u)))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.7f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_BLUE);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else if(i < 35u)
            {
                STAGE_LIFETIME(pEnemy, (i < 34u) ? 0.8f : 0.5f, (i < 34u) ? (3.5f + 0.325f * I_TO_F((i - 22u) % 8u)) : 0.0f)

                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 26u) ? ((I_TO_F(i - 22u) - 1.5f) * 0.6f) : ((i < 34u) ? ((I_TO_F((((i - 26u) % 4u) * 2u + 1u) % 5u) - 1.5f) * 0.6f) : -0.8f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 22u) {}
                else if(i == 23u) pEnemy->InvertY  ();
                else if(i == 24u) {}
                else if(i == 25u) pEnemy->InvertY  ();
                else if(i <  34u) pEnemy->Rotate90 ();
                else if(i <  35u) pEnemy->Rotate270();

                if(pEnemy->ReachedDeath())
                {
                    if(i < 26u)
                    {
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, true,  SNOW_TYPE_INVERT);
                        nLineFunc(pEnemy->GetPosition().xy(), 5.0f, false, SNOW_TYPE_INVERT);
                    }
                    else if(i < 34u)
                    {
                        nAllFunc(SNOW_TYPE_INVERT);
                    }
                    else
                    {
                        nPointFunc(pEnemy->GetPosition().xy(), 30.0f, SNOW_TYPE_INVERT);
                    }
                }

                pEnemy->DefaultRotate(fLifeTimeBase * 3.0f);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 15.0f);

            ASSERT(m_Snow.IsActive())
            const coreBool bHitOld = m_Snow.TestCollision(pEnemy->GetOldPos());
            const coreBool bHitNew = m_Snow.TestCollision(pEnemy->GetPosition().xy());

            pEnemy->SetAlpha(bHitNew ? 0.2f : 1.0f);

            if(bHitOld != bHitNew)
            {
                g_pSpecialEffects->CreateSplashDot(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_WHITE);
            }

            if(pEnemy->ReachedDeath())
            {
                if(++iHiddenCount >= pSquad2->GetNumEnemies())
                {
                    STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
                }
                else
                {
                    g_pSpecialEffects->PlaySound(pEnemy->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iHiddenCount, pSquad2->GetNumEnemies()), SOUND_ITEM_02);
                }
            }

            if(!g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy(), 1.3f))
            {
                pEnemy->Kill(false);
            }
        });

        if((m_iStageSub == 9u) && STAGE_BEGINNING2) g_pEnvironment->SetTargetDirectionLerp(coreVector2(0.0f,1.0f), 30.0f);

        STAGE_WAVE(1u, "6-2", {55.0f, 80.0f, 110.0f, 135.0f, 270.0f})   // ZWEIUNDDREISSIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pGame->KillHelpers();

        this->DisableSnow(false);

        for(coreUintW i = 0u; i < CALOR_HAILS; ++i)
            this->DisableHail(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        cSnowBackground* pBackground = d_cast<cSnowBackground*>(g_pEnvironment->GetBackground());

        pBackground->GetOutdoor()->LerpHeightNow(1.0f, 0.0f);
        pBackground->SetGroundDensity(0u, 0.0f);
        pBackground->SetGroundDensity(1u, 1.0f);
        pBackground->SetGroundDensity(2u, 1.0f);
        pBackground->SetAirDensity   (1u, 0.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // everything moves against current direction
    // - no infinity for opponents, difficult to understand, some could be hiding
    // - all enemies in the dungeon are optional, you could miss one and backtracking is impossible, portals are boring, pay attention to points!
    // - (old: in dungeon start-tunnel, opponents should not be behind a straight line, otherwise they will simply be shot when turning (though I think tat's good to show the dungeon movement))
    // - all enemies in the dungeon have their own row, colum, and are not identical between rooms
    // - enemy arrangement should make the player turn, distinguish between 90 (easy) and 180 (hard)
    // - first enemy group has to make the player turn while bullets cannot yet do any damage to him, but are already clearly visible
    // - traps on doors lure the player there, and because they are different you know where you got in and out
    // - enemy groups in the first two phases must have a good flow (with attention to player rotation), be careful that the next opponents are not shot straight away
    // - last enemy of the second phase must guide the player to the dungeon entrance (already rotating)
    // - start-tunnel in dungeon prepares the player
    // - there should be no enemies outside the dungeon (except a special one), otherwise you can easily miss them
    // - bullets in dungeon are created in order to have consistent straight line overlap everywhere (bottom to top, left to right), corners are ignored and random though
    // - corners in dungeon have double-bullets, but they are important for collision handling
    // - collision handling in the dungeon is extremely unstable, and can fail with slightly different parameters, should be fixed or not reused (possible issues: player collision handling, bullet collision range (player needs to touch cluster), global move, bullet test order, cGame callback being called later, ...)
    // - background needs to move slower, to improve visual distinction with the moving bullets (could also be during a sandstorm, rainstorm or other visual distortion to completely remove the background movement)
    // - fullscreen arrow/cone (to show the current move direction) distracts too much from chests and the target arrow
    // TASK: collect hidden treasure-boxes in dungeons (open up when flying over it)
    // TASK: destroy secret enemy at the back of the initial room
    // ACHIEVEMENT: create a prison with a maximum size of 4x4 bullets
    // TODO 1: hardmode: attacking enemies
    // TODO 1: hardmode: moving enemies in dungeon
    // TODO 1: hardmode: additional attacking pseudo-enemy, made of bullet?), or infinity object, even in dungeon (though similar to P1 object)
    // TODO 3: late update: make sure corners in dungeon are visually consistent (depth)
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.3f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 64u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_GET_START(17u)
            STAGE_GET_VEC2 (vGlobalOffset)
            STAGE_GET_VEC2 (vGlobalMove)
            STAGE_GET_VEC2 (vGlobalDir, vGlobalDir = coreVector2(0.0f,1.0f))
            STAGE_GET_UINT (iBulletField)
            STAGE_GET_UINT (iTrapIndex)
            STAGE_GET_UINT (iSecretIndex)
            STAGE_GET_UINT (iCreationDelay)
            STAGE_GET_UINT (iCreationEnemy, iCreationEnemy = 44u)
            STAGE_GET_UINT (iCreationChest)
            STAGE_GET_UINT (iChestCount)
            STAGE_GET_UINT (iReset)
            STAGE_GET_UINT (iHitState)
            STAGE_GET_UINT (iHitStateOld)
            STAGE_GET_FLOAT(fSpeedFix)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  1u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  2u,  6u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  7u, 11u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 12u, 16u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 17u, 26u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 27u, 34u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 35u, 42u)
            else if(STAGE_SUB(8u)) STAGE_RESURRECT(pSquad1, 43u, 43u)
            else if(STAGE_SUB(9u)) STAGE_RESURRECT(pSquad1, 44u, 63u)

            if((m_iStageSub == 6u) || (m_iStageSub == 9u))
            {
                vGlobalOffset = coreVector2(0.0f,0.0f);

                g_pGame->GetBulletManagerEnemy()->ClearBulletsTyped<cQuadBullet>(true);
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }

            if(m_iStageSub == 9u)
            {
                this->ResetCollPlayerBullet();
            }
        }

        cHelper* pHelperBlue = g_pGame->GetHelper(ELEMENT_BLUE);
        cHelper* pHelperCyan = g_pGame->GetHelper(ELEMENT_CYAN);
        cEnemy*  pDummy      = pSquad1->GetEnemy(0u);

        constexpr coreFloat fScale = 0.1f;

        const auto nPositionFunc = [&](const coreInt32 x, const coreInt32 y, const coreVector2 vShift = coreVector2(0.0f,0.0f))
        {
            return (coreVector2(I_TO_F(x), I_TO_F(y) + 2.5f) + vShift) * (FOREGROUND_AREA * fScale * 6.0f) + vGlobalOffset;
        };

        const auto nAddBulletFunc = [&](const coreInt32 iDamage, const coreVector2 vPosition, const coreVector2 vDirection, const coreFloat fScale = 1.3f)
        {
            return g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(iDamage, 0.0f, pDummy, vPosition, vDirection)->ChangeSize(fScale)->ChangeCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.0f)->AddStatus(BULLET_STATUS_IMMORTAL);
        };

        const auto nAddBulletWallFunc = [&](const coreInt32 iDamage, const coreVector2 vPosition, const coreVector2 vDirection, const coreFloat fScale = 1.3f)
        {
            return g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(iDamage, 0.0f, pDummy, vPosition, vDirection)->ChangeSize(fScale)->ChangeCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.5f)->AddStatus(BULLET_STATUS_IMMORTAL | BULLET_STATUS_GHOST);
        };

        if((m_iStageSub == 9u) && (iCreationDelay < 6u))
        {
            const auto nCreateBlockFunc = [&](const coreInt32 x, const coreInt32 y, const coreUint8 iShape = 0u)
            {
                const coreVector2 vBase = nPositionFunc(x, y);

                if(iShape)
                {
                    //   0
                    // 3   1
                    //   2
                    for(coreUintW i = 0u; i < 7u; ++i)
                    {
                        const coreVector2 vPos = coreVector2(I_TO_F(i) - 3.0f, 3.0f) * (FOREGROUND_AREA * fScale);
                        const coreVector2 vDir = coreVector2(0.0f,1.0f);

                        if(HAS_BIT(iShape, 0u)) nAddBulletWallFunc(6,  vPos                         + vBase, vDir);
                        if(HAS_BIT(iShape, 2u)) nAddBulletWallFunc(6,  vPos.InvertedY()             + vBase, vDir);
                        if(HAS_BIT(iShape, 3u)) nAddBulletWallFunc(7, -vPos            .Rotated90() + vBase, vDir);
                        if(HAS_BIT(iShape, 1u)) nAddBulletWallFunc(7, -vPos.InvertedY().Rotated90() + vBase, vDir);
                    }
                }
                else
                {
                    const coreVector2 vPos = coreVector2(0.5f,0.5f) * 0.11f * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    nAddBulletFunc(5,  vPos             + vBase, vDir);
                    nAddBulletFunc(5, -vPos             + vBase, vDir);
                    nAddBulletFunc(5,  vPos.Rotated90() + vBase, vDir);
                    nAddBulletFunc(5, -vPos.Rotated90() + vBase, vDir);
                }
            };

            const auto nCreateEnemyFunc = [&](const coreInt32 x, const coreInt32 y)
            {
                const coreVector2 vBase = nPositionFunc(x, y);

                pSquad1->GetEnemy(iCreationEnemy++)->SetPosition(coreVector3(vBase, 0.0f));
            };

            const auto nCreateChestFunc = [&](const coreInt32 x, const coreInt32 y)
            {
                if(g_pGame->IsTask())
                {
                    const coreVector2 vBase = nPositionFunc(x, y);

                    this->EnableChest(iCreationChest);
                    this->GetChest(iCreationChest++)->SetPosition(coreVector3(vBase, 0.0f));
                }
            };

            switch(iCreationDelay++)
            {
            default: UNREACHABLE
            case 0u:
                nCreateBlockFunc(-2, -4, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-1, -4, BIT(2u));
                nCreateBlockFunc( 0, -4, BIT(2u));
                nCreateBlockFunc( 1, -4, BIT(2u));
                nCreateBlockFunc( 2, -4, BIT(2u) | BIT(1u));
                nCreateBlockFunc(-2, -3, BIT(3u));
                nCreateBlockFunc( 2, -3, BIT(1u));
                nCreateBlockFunc(-2, -2, BIT(3u));
                nCreateBlockFunc( 2, -2, BIT(1u));
                nCreateBlockFunc(-2, -1, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-1, -1, BIT(0u));
                nCreateBlockFunc( 1, -1, BIT(0u));
                nCreateBlockFunc( 2, -1, BIT(1u) | BIT(0u));

                if(g_pGame->IsTask())
                {
                    iSecretIndex = iCreationEnemy;
                    ASSERT(iSecretIndex)

                    nCreateEnemyFunc( 0, -5);
                }
                break;

            case 1u:
                nCreateBlockFunc( 0,  0, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0,  1, BIT(3u) | BIT(0u));
                nCreateBlockFunc( 1,  1, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 2,  1, BIT(2u) | BIT(1u));
                nCreateBlockFunc( 2,  2, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0,  3, BIT(3u) | BIT(2u));
                nCreateBlockFunc( 1,  3, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 2,  3, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 0,  4, BIT(3u) | BIT(1u));

                nCreateEnemyFunc( 1,  1);
                nCreateEnemyFunc( 2,  2);
                nCreateEnemyFunc( 1,  3);
                break;

            case 2u:
                nCreateBlockFunc(-2,  5, BIT(3u) | BIT(2u) | BIT(0u));
                nCreateBlockFunc(-1,  5, BIT(2u) | BIT(1u));
                nCreateBlockFunc( 1,  5, BIT(2u));
                nCreateBlockFunc( 2,  5, BIT(2u) | BIT(1u));
                nCreateBlockFunc(-2,  6, BIT(3u));
                nCreateBlockFunc( 0,  6, BIT(0u));
                nCreateBlockFunc( 1,  6, BIT(1u));
                nCreateBlockFunc( 2,  6, BIT(1u));
                nCreateBlockFunc(-2,  7, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 1,  7, BIT(0u));
                nCreateBlockFunc( 2,  7, BIT(1u));
                nCreateBlockFunc(-2,  8, BIT(3u));
                nCreateBlockFunc(-1,  8, BIT(0u));
                nCreateBlockFunc( 2,  8, BIT(1u) | BIT(0u));
                nCreateBlockFunc(-2,  9, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-1,  9, BIT(0u));
                nCreateBlockFunc( 0,  9, BIT(1u));
                nCreateBlockFunc( 1,  9, BIT(0u));
                nCreateBlockFunc( 2,  9, BIT(1u) | BIT(0u));

                nCreateEnemyFunc(-2,  5);
                nCreateEnemyFunc( 2,  6);
                nCreateEnemyFunc( 0,  7);
                nCreateEnemyFunc(-1,  8);
                nCreateEnemyFunc( 1,  9);

                nCreateChestFunc(-2,  9);
                nCreateChestFunc( 1,  6);
                break;

            case 3u:
                nCreateBlockFunc( 0, 10, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0, 11, BIT(3u) | BIT(1u));

                nCreateBlockFunc(-2, 12, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-1, 12, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 0, 12, BIT(0u));
                nCreateBlockFunc( 1, 12, BIT(2u) | BIT(1u) | BIT(0u));
                nCreateBlockFunc( 2, 12, BIT(2u) | BIT(1u));
                nCreateBlockFunc(-2, 13, BIT(3u) | BIT(0u));
                nCreateBlockFunc( 0, 13, BIT(0u));
                nCreateBlockFunc( 1, 13, BIT(0u));
                nCreateBlockFunc( 2, 13, BIT(1u));
                nCreateBlockFunc(-2, 14, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-1, 14, BIT(1u));
                nCreateBlockFunc( 1, 14, BIT(1u));
                nCreateBlockFunc( 2, 14, BIT(1u));
                nCreateBlockFunc(-2, 15, BIT(3u));
                nCreateBlockFunc(-1, 15, BIT(0u));
                nCreateBlockFunc( 0, 15, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 1, 15, BIT(0u));
                nCreateBlockFunc( 2, 15, BIT(1u));
                nCreateBlockFunc(-2, 16, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-1, 16, BIT(0u));
                nCreateBlockFunc( 1, 16, BIT(0u));
                nCreateBlockFunc( 2, 16, BIT(1u) | BIT(0u));

                nCreateEnemyFunc( 2, 12);
                nCreateEnemyFunc( 0, 13);
                nCreateEnemyFunc(-2, 14);
                nCreateEnemyFunc( 1, 15);
                nCreateEnemyFunc(-1, 16);

                nCreateChestFunc( 0, 14);
                nCreateChestFunc( 1, 12);
                break;

            case 4u:
                nCreateBlockFunc( 0, 17, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 0, 18, BIT(3u) | BIT(1u));

                nCreateBlockFunc(-2, 19, BIT(3u) | BIT(2u));
                nCreateBlockFunc(-1, 19, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 0, 19, BIT(0u));
                nCreateBlockFunc( 1, 19, BIT(2u) | BIT(0u));
                nCreateBlockFunc( 2, 19, BIT(2u) | BIT(1u));
                nCreateBlockFunc(-2, 20, BIT(3u) | BIT(1u));
                nCreateBlockFunc( 1, 20, BIT(1u));
                nCreateBlockFunc( 2, 20, BIT(1u));
                nCreateBlockFunc(-2, 21, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-1, 21, BIT(1u));
                nCreateBlockFunc( 0, 21, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 1, 21, BIT(1u));
                nCreateBlockFunc( 2, 21, BIT(1u));
                nCreateBlockFunc(-2, 22, BIT(3u) | BIT(1u));
                nCreateBlockFunc(-1, 22, BIT(0u));
                nCreateBlockFunc( 1, 22, BIT(1u) | BIT(0u));
                nCreateBlockFunc( 2, 22, BIT(1u));
                nCreateBlockFunc(-2, 23, BIT(3u) | BIT(0u));
                nCreateBlockFunc(-1, 23, BIT(0u));
                nCreateBlockFunc( 0, 23, BIT(0u));
                nCreateBlockFunc( 1, 23, BIT(0u));
                nCreateBlockFunc( 2, 23, BIT(1u) | BIT(0u));

                nCreateEnemyFunc(-1, 19);
                nCreateEnemyFunc( 1, 20);
                nCreateEnemyFunc( 2, 21);
                nCreateEnemyFunc( 0, 22);
                nCreateEnemyFunc(-2, 23);

                nCreateChestFunc(-1, 20);
                nCreateChestFunc( 1, 23);
                break;

            case 5u:
                iTrapIndex = g_pGame->GetBulletManagerEnemy()->GetNumBullets();

                for(coreUintW i = 0u; i < 34u; ++i)
                {
                    const coreVector2 vPos = HIDDEN_POS;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    nAddBulletFunc(4, vPos, vDir, 1.6f);
                }

                nCreateEnemyFunc(1000, 1000);

                pHelperBlue->AddStatus(HELPER_STATUS_HIDDEN);

                pHelperBlue->Resurrect(false);
                pHelperBlue->SetPosition(coreVector3(nPositionFunc(0, 21), 0.0f));

                pHelperCyan->Resurrect(false);
                pHelperCyan->SetPosition(coreVector3(nPositionFunc(3, 12), 0.0f));

                this->EnableAim(pHelperBlue);
                break;
            }
        }

        const coreVector2 vBaseDir = g_pGame->FindPlayerDual(0u)->GetDirection().xy();

        coreBool bSameBase = true;
        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            if(!SameDirection(vBaseDir, pPlayer->GetDirection().xy()))
                bSameBase = false;

            if(m_iStageSub < 10u) pPlayer->ShowArrow(0u);
        });

        if(bSameBase) vGlobalDir = vBaseDir;

        const coreFloat fGlobalSpeed = g_pGame->IsEasy() ? 20.0f : 30.0f;

        vGlobalMove = iReset ? -vGlobalOffset : (vGlobalDir * (-fGlobalSpeed * TIME * (1.0f - (g_pGame->IsVersion(12u) ? fSpeedFix : MIN1(fSpeedFix)))));
        fSpeedFix   = MAX0(fSpeedFix - 1.0f * TIME);

        vGlobalOffset += vGlobalMove;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, (i < 2u) ? (0.5f + 0.2f * I_TO_F(i % 2u)) : ((i < 17u) ? (0.2f * I_TO_F((i - 2u) % 5u)) : 0.0f))

            if(i < 44u)
            {
                const coreSpline2* pPath = (i < 2u || i >= 27u) ? pPath1 : pPath2;

                if(pPath == pPath2) STAGE_REPEAT(pPath->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 2u) ? ((i % 2u) ? -0.9f : -0.7f) : ((i < 17u) ? -0.8f : ((i < 27u) ? ((I_TO_F(i - 17u) - 4.5f) * 0.22f) : ((i < 43u) ? ((I_TO_F((i - 27u) % 4u) - 1.5f) * 0.3f) : 0.0f))), 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  2u) pEnemy->Rotate270();
                else if(i <  7u) pEnemy->Rotate90 ();
                else if(i < 12u) pEnemy->Rotate180();
                else if(i < 17u) {}
                else if(i < 27u) pEnemy->Rotate180();
                else if(i < 31u) pEnemy->Rotate180();
                else if(i < 35u) {}
                else if(i < 39u) pEnemy->Rotate270();
                else if(i < 43u) pEnemy->Rotate90 ();
                else if(i < 44u) {}
            }
            else
            {
                const coreVector2 vNewPos = pEnemy->GetPosition().xy() + vGlobalMove;

                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            if(i >= 43u) pEnemy->DefaultRotate(m_fStageSubTime * (1.4f*PI));

            if(iSecretIndex && (i == iSecretIndex) && pEnemy->ReachedDeath())
            {
                STAGE_BADGE(1u, BADGE_NORMAL, pEnemy->GetPosition())
            }
        });

        if(m_iStageSub < 6u)
        {
            constexpr coreUintW iBullets = 18u;
            STATIC_ASSERT(iBullets <= sizeof(iBulletField)*8u)

            for(coreUintW i = 0u; i < iBullets; ++i)
            {
                if(HAS_BIT(iBulletField, i))      continue;
                if(g_pGame->IsEasy() && (i % 2u)) continue;

                const coreVector2 vBulletPos = vGlobalOffset + (coreVector2(I_TO_F(i % 6u), I_TO_F(i / 6u) * 2.0f + I_TO_F(i % 2u)) / 6.0f - 0.5f) * (FOREGROUND_AREA * 2.8f);

                if((ABS(vBulletPos.x) > FOREGROUND_AREA.x * 1.4f) || (ABS(vBulletPos.y) > FOREGROUND_AREA.y * 1.4f))
                {
                    ADD_BIT(iBulletField, i)

                    const coreVector2 vPos = coreVector2(0.5f,0.5f) * 0.11f * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2(0.0f,1.0f);

                    nAddBulletFunc(5,  vPos             + vBulletPos, vDir);
                    nAddBulletFunc(5, -vPos             + vBulletPos, vDir);
                    nAddBulletFunc(5,  vPos.Rotated90() + vBulletPos, vDir);
                    nAddBulletFunc(5, -vPos.Rotated90() + vBulletPos, vDir);
                }
            }
        }
        else if(m_iStageSub < 9u)
        {
            if(!g_pGame->GetBulletManagerEnemy()->GetNumBulletsEst())
            {
                for(coreUintW j = 0u; j < 4u; ++j)
                {
                    for(coreUintW i = 0u; i < 66u; ++i)
                    {
                        const coreVector2 vPos = MapStepRotated90(coreVector2((I_TO_F(i) - 33.0f) * 0.11f, 1.21f) * FOREGROUND_AREA, j);
                        const coreVector2 vDir = coreVector2(0.0f,1.0f);

                        nAddBulletFunc(j + 1, vPos, vDir);
                    }
                }
            }

            coreFloat afRange[4] = {1000.0f};   // +y, -x, -y, +x

            const coreVector2 vBorder = (vGlobalOffset - vGlobalMove).Processed(FmodRange, 0.0f, FOREGROUND_AREA.x * 0.11f);

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
            {
                const coreUintW iDamage = pBullet->GetDamage();
                ASSERT((iDamage >= 1u) && (iDamage <= 4u))

                const coreVector2 vDir = UnpackDirection((iDamage - 1u) * 2u);

                coreVector2 vNewPos = pBullet->GetPosition().xy();

                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.21f) && (vDir.x < 0.0f)) vNewPos.x = -FOREGROUND_AREA.x * 1.32f + vBorder.x;   // #
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.21f) && (vDir.x > 0.0f)) vNewPos.x =  FOREGROUND_AREA.x * 1.21f + vBorder.x;
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.21f) && (vDir.y < 0.0f)) vNewPos.y = -FOREGROUND_AREA.y * 1.32f + vBorder.y;   // #
                else if((vNewPos.y >  FOREGROUND_AREA.y * 1.21f) && (vDir.y > 0.0f)) vNewPos.y =  FOREGROUND_AREA.y * 1.21f + vBorder.y;

                pBullet->SetPosition(coreVector3(vNewPos, 0.0f));

                afRange[iDamage - 1u] = pBullet->GetPosition().arr(iDamage % 2u);
            });

            if((afRange[3] - afRange[1] < 15.0f) &&
               (afRange[0] - afRange[2] < 15.0f))
            {
                STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
            }

            STAGE_COLL_PLAYER_BULLET(pPlayer, pBullet, vIntersection, bFirstHit, COLL_REF(vGlobalOffset))
            {
                vGlobalOffset = coreVector2(0.0f,0.0f);

                g_pGame->GetBulletManagerEnemy()->ClearBulletsTyped<cQuadBullet>(true);
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            });

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->HasStatus(PLAYER_STATUS_REPAIRED))
                {
                    vGlobalOffset = coreVector2(0.0f,0.0f);

                    g_pGame->GetBulletManagerEnemy()->ClearBulletsTyped<cQuadBullet>(true);
                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_06);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
                }
            });
        }
        else
        {
            if(iTrapIndex)
            {
                iReset = 1u;

                coreUint32 i = 0u - iTrapIndex;
                g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
                {
                    if((ABS(pBullet->GetPosition().x) < FOREGROUND_AREA.x * 1.3f) &&
                       (ABS(pBullet->GetPosition().y) < FOREGROUND_AREA.y * 1.3f))
                    {
                        iReset = 0u;
                    }

                    if(i < 8u)   // two blocks
                    {
                        const coreVector2 vBase   = nPositionFunc(0, 4, coreVector2(0.0f, (i / 4u) ? -0.25f : 0.25f));
                        const coreVector2 vOffset = coreVector2(I_TO_F(i % 2u) - 0.5f, I_TO_F((i / 2u) % 2u) - 0.5f) * 0.11f * FOREGROUND_AREA;
                        const coreFloat   fHeight = SIN(m_fStageSubTime * (1.0f*PI) + I_TO_F(i / 4u) * (1.0f*PI)) * 10.0f;

                        pBullet->SetPosition(coreVector3(vBase + vOffset + coreVector2(fHeight, 0.0f), 0.0f));
                    }
                    else if(i < 24u)   // rotator
                    {
                        const coreVector2 vBase   = nPositionFunc(0, 17, coreVector2(0.0f,0.5f));
                        const coreVector2 vOffset = coreVector2::Direction(m_fStageSubTime * (0.5f*PI) + I_TO_F(i - 8u) * (0.1f*PI)) * 18.0f;

                        pBullet->SetPosition(coreVector3(vBase + vOffset, 0.0f));
                    }
                    else if(i < 34u)   // snake
                    {
                        const coreVector2 vBase   = nPositionFunc(0, 10, coreVector2(0.0f, 0.5f + 0.2f * (I_TO_F(i - 24u) - 4.5f)));
                        const coreFloat   fHeight = SIN(m_fStageSubTime * (-1.0f*PI) + I_TO_F(i - 24u) * (0.2f*PI)) * 10.0f;

                        pBullet->SetPosition(coreVector3(vBase + coreVector2(fHeight, 0.0f), 0.0f));
                    }

                    ++i;
                });
            }

            if(g_pGame->IsTask())
            {
                for(coreUintW i = 0u; i < CALOR_CHESTS; ++i)
                {
                    coreObject3D* pChest = this->GetChest(i);
                    if(!pChest->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                    const coreVector2 vNewPos = pChest->GetPosition().xy() + vGlobalMove;

                    pChest->SetPosition(coreVector3(vNewPos, 0.0f));

                    if(this->IsChestReady(i))
                    {
                        STAGE_FOREACH_PLAYER(pPlayer, j)
                        {
                            const coreVector2 vDiff = pPlayer->GetPosition().xy() - pChest->GetPosition().xy();

                            if((ABS(vDiff.x) < pChest->GetCollisionRange().x) &&
                               (ABS(vDiff.y) < pChest->GetCollisionRange().y))
                            {
                                this->DisableChest(i, true);

                                if(++iChestCount == CALOR_CHESTS)
                                {
                                    STAGE_BADGE(0u, BADGE_EASY, pChest->GetPosition())
                                }
                                else
                                {
                                    g_pGame->GetCombatText()->DrawProgress(iChestCount, CALOR_CHESTS, pChest->GetPosition());
                                    g_pSpecialEffects->PlaySound(pChest->GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iChestCount, CALOR_CHESTS), SOUND_ITEM_01);
                                }
                            }
                        });
                    }
                }
            }

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                if(pPlayer->HasStatus(PLAYER_STATUS_REPAIRED))
                {
                    pPlayer->SetPosition(g_pGame->GetPlayer(1u - i)->GetPosition());
                    STATIC_ASSERT(GAME_PLAYERS == 2u)

                    fSpeedFix = g_pGame->IsVersion(12u) ? 1.6f : 2.1f;
                }
            });

            iHitStateOld = iHitState;
            iHitState    = 0u;

            STAGE_COLL_PLAYER_BULLET(pPlayer, pBullet, vIntersection, bFirstHit, COLL_REF(vGlobalMove), COLL_REF(vGlobalDir), COLL_REF(fSpeedFix), COLL_REF(iHitState), COLL_REF(iHitStateOld))
            {
                const coreInt32 iDamage = pBullet->GetDamage();

                if(iDamage == 4) return;

                const coreVector2 vAxis  = (iDamage == 6) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
                const coreVector2 vDiff  = pPlayer->GetPosition().xy() - pBullet->GetPosition().xy();
                const coreVector2 vRange = vAxis.yx() * 2.1f + vAxis * 1.9f;   // create alcove in corner

                if((ABS(vDiff.x) < vRange.x) &&
                   (ABS(vDiff.y) < vRange.y))
                {
                    const coreVector2 vDiffOld = pPlayer->GetOldPos() - (pBullet->GetPosition().xy() - vGlobalMove);
                    const coreVector2 vNorm    = AlongCrossNormal(vDiffOld * vAxis);
                    const coreVector2 vPos     = pPlayer->GetPosition().xy() * vAxis.yx() + pBullet->GetPosition().xy() * vAxis + vNorm * 1.9f;

                    pPlayer->SetPosition(coreVector3(vPos, 0.0f));

                    if(SameDirection(vGlobalDir, -vNorm))
                    {
                        fSpeedFix = g_pGame->IsVersion(12u) ? 1.6f : 1.1f;
                    }

                    const coreUintW iIndex = g_pGame->GetPlayerIndex(pPlayer);

                    ADD_BIT(iHitState, iIndex)
                    if(!HAS_BIT(iHitStateOld, iIndex))
                    {
                        if(pPlayer->IsNormal()) pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vIntersection.xy());
                    }
                }
            });
        }

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
        {
            coreVector2 vNewPos = pBullet->GetPosition().xy() + vGlobalMove;

            if(m_iStageSub < 6u)
            {
                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.4f) && (vGlobalMove.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.8f;
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.4f) && (vGlobalMove.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.8f;
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.4f) && (vGlobalMove.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.8f;
                else if((vNewPos.y >  FOREGROUND_AREA.y * 1.4f) && (vGlobalMove.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.8f;
            }

            pBullet->SetPosition(coreVector3(vNewPos, 0.0f));

            pBullet->SetAnimation(0.2f * m_fStageTime);
            pBullet->SetAnimSpeed(0.0f);
        });

        if(!pHelperBlue->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vNewPos = pHelperBlue->GetPosition().xy() + vGlobalMove;

            pHelperBlue->SetPosition(coreVector3(vNewPos, 0.0f));

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreVector2 vDiff = pPlayer->GetPosition().xy() - vNewPos;

                if(vDiff.LengthSq() < POW2(6.0f))
                {
                    pSquad1->ClearEnemies(true);
                    pHelperBlue->Kill(true);
                    pHelperCyan->Kill(true);

                    this->DisableAim(true);
                }
            });
        }

        if(!pHelperCyan->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vNewPos = pHelperCyan->GetPosition().xy() + vGlobalMove;

            pHelperCyan->SetPosition(coreVector3(vNewPos, 0.0f));
        }

        if(STAGE_BEGINNING) g_pEnvironment->SetTargetSpeedLerp(2.0f, 10.0f);

        if(g_pGame->IsEasy()) STAGE_WAVE(2u, "6-3", {50.0f, 70.0f, 95.0f, 115.0f, 225.0f})   // DREIUNDDREISSIG
                         else STAGE_WAVE(2u, "6-3", {45.0f, 65.0f, 90.0f, 110.0f, 220.0f})
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < CALOR_CHESTS; ++i)
            this->DisableChest(i, false);

        this->DisableAim(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 3u, 4u})
    {
        g_pEnvironment->SetTargetSpeedNow(2.0f);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // stick to one side and stay there
    // - side movement of galaga groups helps to attack them even when they attack directly
    // - splitting side-groups to 4-4-4-4 (instead of 8-8) makes it easier to prevent getting hit
    // - if you switch at the lat moment on the side-moving enemies and keep shooting, you have to survive, the cross-point should not move (beware of coop)
    // - enemies cannot move too quickly in the direction of flight, you don't notice them because of the fast background
    // - did not work: pang (boring + slow), flip speed (too unpredictable crashing into stuff), rotate (there was no meaningful mechanic, just visual, player-controlled needs incentive and this is only done by shooting along border)
    // - the first group should feel like a dog-fight
    // - collecting items as a task is difficult because there is one degree of freedom missing and you have to wait until the items fall on your axis
    // - in galaga, only the bottom enemies shoot, but ignoring it here creates a better bullet-pattern
    // TASK: destroy the UFO
    // TASK: reach the top, while on full speed, with back-fire push
    // ACHIEVEMENT: destroy all regular enemies while shooting down
    // TODO 5: badge: order on first skeet-shooting
    // TODO 5: badge: something with/during the helper at the end
    // TODO 1: hardmode: direction changes all the time
    // TODO 1: hardmode: you get tossed up on a regular basis
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(4u);
            pPath1->AddNode(coreVector2( 0.2f,  1.3f),  coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.7f,  0.0f),  coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.45f,-0.25f), coreVector2(1.0f, 0.0f));
            pPath1->AddNode(coreVector2(-0.2f,  0.0f),  coreVector2(0.0f, 1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(6u);
            pPath2->AddNode(coreVector2(-1.3f, -0.8f),  coreVector2( 1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.2f,  0.0f),  coreVector2( 0.0f, 1.0f));
            pPath2->AddNode(coreVector2(-0.45f, 0.25f), coreVector2(-1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.7f,  0.0f),  coreVector2( 0.0f,-1.0f));
            pPath2->AddNode(coreVector2(-0.45f,-0.25f), coreVector2( 1.0f, 0.0f));
            pPath2->AddNode(coreVector2(-0.2f,  0.0f),  coreVector2( 0.0f, 1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_PATH(pPath3)
        {
            pPath3->Reserve(6u);
            pPath3->AddNode (coreVector2(-1.0f, 1.3f), coreVector2(0.0f,-1.0f));
            pPath3->AddNodes(coreVector2(-1.0f,-1.0f), coreVector2(0.0f,-1.0f), coreVector2(1.0f,0.0f));
            pPath3->AddNodes(coreVector2( 1.0f,-1.0f), coreVector2(1.0f, 0.0f), coreVector2(0.0f,1.0f));
            pPath3->AddNode (coreVector2( 1.0f, 1.3f), coreVector2(0.0f, 1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f, 1.4f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f,-1.4f), coreVector2(0.0f,-1.0f));
            pPath4->Refine();
        });

        STAGE_ADD_PATH(pPath5)
        {
            pPath5->Reserve(11u);
            pPath5->AddNode (coreVector2(-1.2f, 0.6f), coreVector2( 1.0f, 0.0f), 0.5f);
            pPath5->AddNode (coreVector2( 0.2f, 0.4f), coreVector2( 1.0f, 0.0f), 2.0f);
            pPath5->AddNode (coreVector2( 0.4f, 0.6f), coreVector2( 0.0f, 1.0f), 1.5f);
            pPath5->AddNode (coreVector2( 0.2f, 0.8f), coreVector2(-1.0f, 0.0f), 1.0f);
            pPath5->AddNode (coreVector2( 0.0f, 0.6f), coreVector2( 0.0f,-1.0f), 0.5f);
            pPath5->AddNode (coreVector2( 0.0f, 0.5f), coreVector2( 0.0f,-1.0f), 0.0f);
            pPath5->AddNodes(coreVector2( 0.0f,-1.1f), coreVector2( 0.0f,-1.0f),              coreVector2( 0.0f, 1.0f), 5.0f, 1.0f);
            pPath5->AddNodes(coreVector2(-0.2f,-0.6f), coreVector2(-1.0f, 1.0f).Normalized(), coreVector2( 1.0f, 0.0f), 0.0f, 1.0f);
            pPath5->AddNode (coreVector2( 1.2f,-0.4f), coreVector2( 1.0f, 0.0f), 3.0f);
            pPath5->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 104u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.55f);
                pEnemy->Configure(30, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cUfoEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(3.0f,1.0f,1.0f));
                pEnemy->Configure(30 + 10, COLOR_SHIP_BLACK, false, true);
                pEnemy->AddStatus(ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(37u + 2u * GAME_PLAYERS)
            STAGE_GET_VEC2_ARRAY (avForce,   16u)
            STAGE_GET_VEC2_ARRAY (avTurn,    GAME_PLAYERS)
            STAGE_GET_FLOAT_ARRAY(afBoost,   2u)
            STAGE_GET_FLOAT      (fOldSpeed, fOldSpeed = g_pEnvironment->GetSpeed())
            STAGE_GET_UINT       (iPushBack, iPushBack = 1u)
            STAGE_GET_UINT       (iFrontShot)
        STAGE_GET_END

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  11u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1, 20u,  35u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 36u,  51u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 64u,  79u)   // #
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 52u,  63u)   // #
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 12u,  12u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 13u,  13u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 14u,  15u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 16u,  17u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 18u,  19u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 80u,  87u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 88u, 103u)
            else if(STAGE_SUB(13u))
            {
                STAGE_DELAY_START_CLEAR
                if(!iFrontShot) STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
            }

                 if(m_iStageSub == 2u) g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
            else if(m_iStageSub == 3u) STAGE_RESURRECT(pSquad2, 0u, 0u)

            if((m_iStageSub == 1u) || (m_iStageSub == 2u))
            {
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 2.0f, 1.0f, SOUND_EFFECT_FLY);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
            }
        }

        if(m_iStageSub >= 1u) afBoost[0] = MIN1(afBoost[0] + 0.35f * TIME);
        if(m_iStageSub >= 2u) afBoost[1] = MIN1(afBoost[1] + 0.35f * TIME);

        const coreFloat fNewSpeed = 16.0f;
        const coreFloat fCurSpeed = LERPB(fOldSpeed, fNewSpeed, afBoost[0]) * 0.5f + LERPB(fOldSpeed, fNewSpeed, afBoost[1]) * 0.5f;

        if(m_iStageSub < 13u)
        {
            g_pEnvironment->SetTargetSpeedNow(fCurSpeed);
        }
        else
        {
            cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW);

            if(STAGE_BEGINNING2)
            {
                pHelper->Resurrect(false);
            }

            const coreFloat fShakeDist = pPath5->TranslateDistance(7u, 0.0f);
            const coreFloat fTotalDist = pPath5->GetTotalDistance();
            const coreFloat fRotaDist  = fShakeDist + 1.0f;
            const coreFloat fTime      = MIN(DelayTime(m_fStageSubTime, fShakeDist, 1.0f), fTotalDist);

            pHelper->SetPosition(coreVector3(pPath5->CalcPosition(fTime) * FOREGROUND_AREA, 0.0f));

            if(STAGE_SUBTIME_POINT(fShakeDist))
            {
                g_pEnvironment->SetTargetSpeedNow(fOldSpeed);

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_YELLOW);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
                g_pSpecialEffects->PlaySound(pHelper->GetPosition(), 1.0f, 0.7f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);

                iPushBack = 0u;

                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pPlayer->ApplyForce(coreVector2(0.0f,300.0f));
                    avTurn[i] = pPlayer->GetDirection().xy();
                });
            }

            if(STAGE_SUBTIME_BETWEEN(fShakeDist, fRotaDist))
            {
                const coreVector2 vRota = coreVector2::Direction(LERPB(0.0f*PI, 4.0f*PI, STEP(fShakeDist, fRotaDist, m_fStageSubTime)));

                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pPlayer->SetDirection(coreVector3(MapToAxis(avTurn[i], vRota), 0.0f));
                    pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_TURN);
                });
            }
            else if(STAGE_SUBTIME_POINT(fRotaDist))
            {
                STAGE_FOREACH_PLAYER(pPlayer, i)
                {
                    pPlayer->SetDirection(coreVector3(avTurn[i], 0.0f));
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
                });
            }

            if(fTime >= fTotalDist)
            {
                pHelper->Kill(false);

                STAGE_DELAY_END
            }
        }

        if(iPushBack)
        {
            const coreFloat   fEnvSpeed = g_pEnvironment->GetSpeed();
            const coreVector2 vEnvDir   = g_pEnvironment->GetDirection();

            g_pSpecialEffects->CreateGust(STEP(fOldSpeed, fNewSpeed, ABS(fEnvSpeed)), vEnvDir.Angle());

            const coreFloat   fValue   = (afBoost[0] ? LERPB(0.5f, -0.4f, afBoost[0]) : 0.0f) + (afBoost[1] ? LERPB(0.3f, -0.6f, afBoost[1]) : 0.0f);
            const coreVector2 vPush    = vEnvDir *     (fValue *  60.0f * TIME);   // strong enough to hide Y movement (forward leaning)
            const coreVector2 vReverse = vEnvDir * MAX0(fValue * -12.0f * TIME);

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                const coreBool bBackFire = (pPlayer->GetWeapon(0u)->GetCooldown() && SameDirection(pPlayer->GetDirection().xy(), coreVector2(0.0f,-1.0f)));

                pPlayer->SetPosition(pPlayer->GetPosition() + coreVector3(bBackFire ? vReverse : vPush, 0.0f));
            });
        }

        const coreVector4 vArea   = PLAYER_AREA_DEFAULT * RCP(iPushBack ? LERPH3(1.0f, PLAYER_AREA_FACTOR, MIN1(m_fStageTime * 3.0f)) : 1.0f);   // move player a bit away from the border
        const coreFloat   fExaust = MAX0(SIN(LERPB(0.0f*PI, 1.0f*PI, afBoost[0])) * 0.5f + SIN(LERPB(0.0f*PI, 1.0f*PI, afBoost[1])) * 0.5f);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea  (vArea);
            pPlayer->SetThrust(fExaust);

            if(fExaust > 0.05f)
            {
                pPlayer->SetDirection(coreVector3(0.0f,1.0f,0.0f));
                pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_TURN);

                pPlayer->ShowArrow(0u);
            }
            else if(iPushBack)
            {
                pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
            }
        });

        STAGE_FOREACH_PLAYER(pPlayer, i)
        {
            if(g_pGame->IsTask())
            {
                if((pPlayer->GetPosition().y >= vArea.w) && (afBoost[1] >= 1.0f))
                {
                    STAGE_BADGE(1u, BADGE_NORMAL, pPlayer->GetPosition())
                }
            }
        });

        const coreFloat fMoveSpeed = g_pGame->IsEasy() ? 0.7f : 1.0f;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, ((i < 12u) ? 0.9f : ((i < 20u) ? 0.7f : ((i < 52u) ? 1.5f : ((i < 64u) ? 1.4f : 1.1f)))) * (((i >= 12u && i < 20u) || (i >= 52u)) ? fMoveSpeed : 1.0f), (i < 12u) ? (3.0f + 0.7f * I_TO_F(i)) : ((i < 20u) ? 0.0f : ((i < 52u) ? (0.2f * I_TO_F((i - 20u) % 8u) + ((i < 36u) ? 4.0f : 0.0f)) : ((i < 64u) ? (0.2f * I_TO_F(i - 52u)) : ((i < 80u) ? (0.2f * I_TO_F((i - 64u) % 16u) + 1.2f * I_TO_F(MIN((i - 64u) / 4u, 2u))) : ((i < 88u) ? (0.2f * I_TO_F(i - 80u)) : ((i < 96u) ? 0.0f : 0.1f)))))))

            if(i < 12u)
            {
                STAGE_REPEAT(pPath4->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i * 5u + 4u) % 12u) - 5.5f) * 0.15f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection   (coreVector3(0.0f,1.0f,0.0f));
            }
            else if(i < 20u)
            {
                STAGE_REPEAT(pPath4->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((((i - 12u) % 4u) * 2u + 1u) % 5u) - 1.5f) * 0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 16u) {}
                else if(i < 18u) pEnemy->Rotate90 ();
                else if(i < 20u) pEnemy->Rotate270();

                pEnemy->InvertX();

                if(STAGE_TICK_LIFETIME(15.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pSquad1->GetEnemy(0u), vPos,  vDir)->ChangeSize(1.7f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pSquad1->GetEnemy(0u), vPos, -vDir)->ChangeSize(1.7f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 1u, COLOR_ENERGY_YELLOW);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }
            else if(i < 52u)
            {
                const coreSpline2* pPath = (i < 36u) ? pPath1 : pPath2;

                const coreVector2 vFactor = coreVector2((((i - 20u) % 16u) < 8u) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                if(pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime))
                {
                    const coreFloat   fTime = MIN1((fLifeTime - pPath->GetTotalDistance()) * 1.5f);
                    const coreVector2 vMove = coreVector2(0.2f * SIN(m_fStageSubTime * (0.5f*PI)), 0.0f);
                    const coreVector2 vFrom = pEnemy->GetPosition().xy();
                    const coreVector2 vTo   = (coreVector2(I_TO_F(((i - 20u) % 4u) + (((i >= 36u) && (((i - 20u) / 2u) % 2u)) ? 2u : 1u)) * 0.15f, (((i - 20u) % 8u) < 4u) ? 0.6f : 0.4f) * vFactor.InvertedX() + vMove) * FOREGROUND_AREA;

                    pEnemy->SetPosition(coreVector3(LERPH3(vFrom.x, vTo.x, fTime), LERP(vFrom.y, vTo.y, fTime), 0.0f));
                }
            }
            else if(i < 80u)
            {
                STAGE_REPEAT(pPath3->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(((i < 64u) || ((i >= 68u) && (i < 72u))) ? 1.0f : -1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath3, vFactor, vOffset * vFactor, fLifeTime);

                const coreVector2 vPos   = pEnemy->GetPosition().xy();
                const coreVector2 vNorm  = AlongCrossNormal(-vPos);
                const coreFloat   fValue = coreVector2::Dot(vPos, vNorm.Rotated90()) / (FOREGROUND_AREA.x * 1.0f);
                const coreFloat   fPower = ((i >= 64u) && (vPos.y <= 0.0f)) ? 0.0f : 12.0f;
                const coreFloat   fJump  = fPower * ABS(SIN(fValue * (1.0f*PI)));

                pEnemy->SetPosition(coreVector3(vPos + vNorm * fJump, 0.0f));

                if(i < 64u) pEnemy->DefaultRotate(fLifeTime * -3.0f);
            }
            else
            {
                coreVector2& vForce = avForce[(i - 80u) % 16u];

                if(STAGE_TAKEOFF)
                {
                    pEnemy->SetPosition(coreVector3(-1.2f,-0.9f,0.0f) * FOREGROUND_AREA3);
                    vForce = coreVector2::Direction(I_TO_F((i - 80u) % 8u) / 8.0f * (-0.1f*PI) - ((i < 88u) ? (0.1f*PI) : (0.05f*PI))) * 1.65f;

                    if(i % 2u)
                    {
                        pEnemy->InvertX();
                        vForce = vForce.InvertedX();
                    }
                }

                vForce.y -= 1.0f * fMoveSpeed * TIME;

                coreVector2 vNewPos = pEnemy->GetPosition().xy() + vForce * FOREGROUND_AREA * (1.5f * fMoveSpeed * TIME);

                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.1f) && (vForce.x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x * 1.1f); vForce.x =  ABS(vForce.x);}
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.1f) && (vForce.x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x * 1.1f); vForce.x = -ABS(vForce.x);}
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.1f) && (vForce.y < 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.1f); vForce.y =  ABS(vForce.y);}

                pEnemy->SetPosition (coreVector3(vNewPos,             0.0f));
                pEnemy->SetDirection(coreVector3(vForce.Normalized(), 0.0f));
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, 2.5f)

            const coreVector2 vFactor = coreVector2(1.0f,1.2f);
            const coreVector2 vOffset = coreVector2(0.9f,0.0f);

            if(pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime)) pEnemy->Kill(false);

            pEnemy->SetDirection(coreVector3(1.0f,0.0f,0.0f));
            pEnemy->Rotate270();

            if(g_pGame->IsTask() && pEnemy->ReachedDeath()) STAGE_BADGE(0u, BADGE_EASY, pEnemy->GetPosition())
        });

        if((m_iStageSub == 1u) && STAGE_TIME_AFTER(1.0f) && STAGE_TICK_FREE2(3.5f, 0.0f))
        {
            const coreFloat fSide   = (s_iTick % 2u) ? -1.0f  :  1.0f;
            const coreFloat fHeight = (s_iTick % 2u) ? -0.95f : -1.0f;

            const coreVector2 vPos = coreVector2(fSide * -1.3f, fHeight) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2(fSide,         0.0f);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pSquad1->GetEnemy(0u), vPos, vDir)->ChangeSize(1.7f);
        }

        const coreBool  bActive = g_pGame->IsEasy() ? ((m_iStageSub == 2u) || (m_iStageSub == 3u)) : ((m_iStageSub == 1u) || (m_iStageSub == 2u) || (m_iStageSub == 3u) || (m_iStageSub == 5u));
        const coreBool  bDelay  = (m_iStageSub == 2u) ? STAGE_SUBTIME_AFTER(4.0f / 1.5f) : STAGE_TIME_AFTER(2.0f);
        const coreFloat fSpeed  = ((m_iStageSub == 5u) ? 7.0f : 3.5f) * (g_pGame->IsEasy() ? 0.5f : 1.0f);

        if(bActive && bDelay && STAGE_TICK_FREE2(fSpeed, 0.0f))
        {
            const coreUintW iNumEnemies = pSquad1->GetNumEnemiesAlive();
            if(iNumEnemies)
            {
                coreUintW iShooter = (s_iTick * ((iNumEnemies == 11u) ? 13u : 11u)) % iNumEnemies;
                STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
                {
                    if(iShooter--) return;

                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = (m_iStageSub == 2u) ? coreVector2(0.0f,-1.0f) : pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                    if(vPos.y > 0.0f)
                    {
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.7f);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 3u, COLOR_ENERGY_YELLOW);
                        g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                    }
                });
            }
        }

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS, COLL_REF(iFrontShot))
        {
            if(!SameDirection90(pBullet->GetFlyDir(), coreVector2(0.0f,-1.0f)))
            {
                iFrontShot += 1u;
                STAGE_FAILTROPHY
            }
        });

        STAGE_WAVE(3u, "6-4", {80.0f, 120.0f, 160.0f, 200.0f, 400.0f})   // VIERUNDDREISSIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        g_pGame->KillHelpers();

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->SetArea     (PLAYER_AREA_DEFAULT);
            pPlayer->SetThrust   (0.0f);
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // wait for play
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // smaller stone is dragged by player with chain
    // - 2: should align with swing-direction, which is easier to hit because swing-direction is against fly-direction
    // - 3: clustered enemies should feel like hitting a brick-building with a wrecking ball
    // - 8: removing bullet-carpet is really satisfying (feedback)
    // - bullet-carpet pattern needs to make vertically moving enemies visible within
    // - bullet-carpet needs to move slower, in relation to swing-speed
    // - enemies within bullet-carpet should not shoot
    // - temporarily loose ability to remove bullet-carpet to scare and pressure player shortly
    // - enemies only come from above, to improve the feeling of facing an army
    // - attacks should stay local to enemies, the difficulty is attacking and navigating around enemies without getting hit from their local attacks
    // - (old: in coop, captured enemies need to be usable on the whole screen, e.g. transfer to other player)
    // TASK: do enough damage to Zeroth before time runs out
    // TASK: attack specific targets with your normal weapon
    // ACHIEVEMENT: in coop only capture enemies with a single player
    // COOP: dirrection of both morning stars is different
    // TODO 1: hardmode: enemies keep attacking while being swung
    // TODO 1: hardmode: permanent bullets from top
    // TODO 5: badge: destroy N bullets with morning star (+ display, but only at the end)
    // TODO 5: badge: some enemies are paired (swing one on the other), only once
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        
        // TODO 1: only cleanup
        for(coreUintW i = 0u; i < CALOR_STARS; ++i)
        {
            if(m_apStarOwner[i] && (m_apStarOwner[i] != &m_Zeroth) && !m_iStarSwing)
            {
                const coreVector2 vDiff = m_apStarOwner[i]->GetPosition().xy() - this->GetStar(i)->GetPosition().xy();

                if(vDiff.LengthSq() >= POW2(CALOR_CHAIN_CONSTRAINT1))
                {
                    m_Boulder.Resurrect();
                    m_Boulder.SetPosition(coreVector3(0.0f,-1.25f,0.0f) * FOREGROUND_AREA3);

                    this->CatchObject(i, &m_Boulder);

                    this->StartSwing(1.0f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3, SPECIAL_SPLASH_SMALL, COLOR_ENERGY_WHITE * 0.8f);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    g_pSpecialEffects->PlaySound(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3, 0.5f, 1.5f, SOUND_EFFECT_SHAKE_01);
                    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);

                    g_pEnvironment->SetTargetSpeedLerp(6.0f, 10.0f);
                }
            }
        }
        

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.5f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.5f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.8f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 63u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(4, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cWarriorEnemy, 9u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(30, COLOR_SHIP_CYAN);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_GET_START(11u)
            STAGE_GET_VEC2 (vHelperMove)
            STAGE_GET_VEC2 (vBullPos)
            STAGE_GET_VEC2 (vBullMove)
            STAGE_GET_VEC2 (vBullMoveReal)
            STAGE_GET_FLOAT(fBullTime)
            STAGE_GET_UINT (iBullCount)
            STAGE_GET_UINT (iAnyCatch)
        STAGE_GET_END

        const cShip* pCatchObject1 = this->GetCatchObject(0u);
        const cShip* pCatchObject2 = this->GetCatchObject(1u);
        STATIC_ASSERT(CALOR_STARS == 2u)

        if(pCatchObject1 && (pCatchObject1 != g_pGame->GetPlayer(0u))) ADD_BIT(iAnyCatch, 0u)
        if(pCatchObject2 && (pCatchObject2 != g_pGame->GetPlayer(1u))) ADD_BIT(iAnyCatch, 1u)

        if(!g_pGame->IsMulti() || (coreMath::PopCount(iAnyCatch) > 1u)) STAGE_FAILTROPHY

        coreBool bLastCatch = false;
        if(pSquad1->IsFinished() && (pSquad2->GetNumEnemiesAlive() == 1u) && m_Boulder.HasStatus(ENEMY_STATUS_DEAD))
        {
            STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
            {
                if((pEnemy == pCatchObject1) || (pEnemy == pCatchObject2))
                    bLastCatch = true;
            });
        }

        if(STAGE_CLEARED || bLastCatch)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u, 13u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1, 14u, 27u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 28u, 34u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad2,  0u,  0u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 35u, 44u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad2,  1u,  4u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad2,  5u,  5u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 55u, 62u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 45u, 54u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad2,  6u,  6u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad2,  7u,  7u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad2,  8u,  8u)
            else if(STAGE_SUB(14u))
            {
                if(g_pGame->IsMulti() && (coreMath::PopCount(iAnyCatch) == 1u))
                {
                    STAGE_BADGE(3u, BADGE_ACHIEVEMENT, g_pGame->FindPlayerDual(0u)->GetPosition())
                }

                // destroy possible remaining enemy
                pSquad2->ClearEnemies(true);
            }
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_PURPLE);

        if((m_iStageSub == 6u) && STAGE_BEGINNING2)
        {
            pHelper->Resurrect(false);
            pHelper->SetPosition(m_Boulder.GetPosition());

            vHelperMove = (m_apStarOwner[0]->GetPosition().xy() - this->GetStar(0u)->GetPosition().xy()).Rotated90().Normalized() * 140.0f;
        }

        if(!pHelper->HasStatus(HELPER_STATUS_DEAD))
        {
            const coreVector2 vPos = pHelper->GetPosition().xy() + vHelperMove * TIME;
            if(!g_pForeground->IsVisiblePoint(vPos / FOREGROUND_AREA, 1.2f)) pHelper->Kill(false);

            pHelper->SetPosition(coreVector3(vPos, 0.0f));
        }

        if(STAGE_TIME_POINT(4.0f))
        {
            m_Zeroth.ResurrectIntro();
        }

        if(g_pGame->IsTask())
        {
            if(STAGE_BEGINNING2)
            {
                     if(m_iStageSub ==  3u) {fBullTime =  2.0f; vBullPos = coreVector2(-0.9f,-1.3f); vBullMove = coreVector2( 0.0f, 1.0f);}
                else if(m_iStageSub ==  6u) {fBullTime =  2.0f; vBullPos = coreVector2( 1.3f,-0.9f); vBullMove = coreVector2(-1.0f, 0.0f);}
                else if(m_iStageSub ==  7u) {fBullTime =  3.0f; vBullPos = coreVector2( 0.9f,-1.3f); vBullMove = coreVector2( 0.0f, 1.0f);}
                else if(m_iStageSub == 10u) {fBullTime = 10.0f; vBullPos = coreVector2(-1.3f, 0.9f); vBullMove = coreVector2( 1.0f, 0.0f);}
            }

            if(fBullTime > 0.0f)
            {
                const coreFloat fOldTime = fBullTime;
                fBullTime -= 1.0f * TIME;

                if(InBetween(0.0f, fBullTime, fOldTime))
                {
                    this->DisableBull(true);
                    this->EnableBull();

                    m_Bull.SetPosition(coreVector3(vBullPos * FOREGROUND_AREA, 0.0f));

                    vBullMoveReal = vBullMove;
                }
            }

            if(!m_Bull.HasStatus(ENEMY_STATUS_DEAD))
            {
                const coreVector2 vNewPos = m_Bull.GetPosition().xy() + vBullMoveReal * (30.0f * TIME);

                m_Bull.SetPosition(coreVector3(vNewPos, 0.0f));

                if(!g_pForeground->IsVisiblePoint(vNewPos / FOREGROUND_AREA, 1.3f) || m_Bull.WasDamaged())
                {
                    this->DisableBull(false);
                }

                if(m_Bull.WasDamaged())
                {
                    if(++iBullCount == 4u)
                    {
                        STAGE_BADGE(1u, BADGE_NORMAL, m_Bull.GetPosition())
                    }
                    else
                    {
                        g_pGame->GetCombatText()->DrawProgress(iBullCount, 4u, m_Bull.GetPosition());
                        g_pSpecialEffects->PlaySound(m_Bull.GetPosition(), 1.0f, SPECIAL_SOUND_PROGRESS(iBullCount, 4u), SOUND_ITEM_02);
                    }

                    g_pSpecialEffects->MacroExplosionColorSmall(m_Bull.GetPosition(), COLOR_ENERGY_ORANGE);
                    g_pSpecialEffects->PlaySound(m_Bull.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);
                }
            }
        }

        if(!m_Zeroth.HasStatus(ENEMY_STATUS_DEAD))
        {
            const coreFloat fPercent    = I_TO_F(m_Zeroth.GetLostHealth   ()) / 590.0f;   // -10
            const coreFloat fPrePercent = I_TO_F(m_Zeroth.GetPreLostHealth()) / 590.0f;

            for(coreUintW i = 1u; i < 5u; ++i)
            {
                if(InBetween(I_TO_F(i) * 0.2f, fPrePercent, fPercent))
                {
                    cPlayer* pPlayer = m_Zeroth.LastAttacker();
                    this->AddExtraScore(pPlayer, 200u, pPlayer->GetPosition());
                }
            }

            if(fPercent >= 1.0f)
            {
                if(g_pGame->IsTask()) STAGE_BADGE(0u, BADGE_EASY, m_Zeroth.GetPosition())
                m_Zeroth.HideTail();
            }
            else if(fPercent >= 0.2f)
            {
                g_pGame->GetCombatText()->AttachMarker(0u, PRINT("%.0f%%", CEIL((1.0f - fPercent) * 100.0f)), m_Zeroth.GetPosition(), COLOR_MENU_INSIDE, false);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy == pCatchObject1) || (pEnemy == pCatchObject2)) return;

            STAGE_LIFETIME(pEnemy, 0.7f, (i < 4u) ? (0.2f * I_TO_F(i)) : ((i < 14u) ? (0.2f * I_TO_F((i - 4u) % 10u)) : ((i < 35u) ? (1.2f * I_TO_F(((i - 14u) / 7u) % 2u)) : ((i < 55u) ? (0.2f * I_TO_F((i - 35u) % 10u) + ((i < 45u) ? 0.0f : 5.0f)) : (0.4f * I_TO_F(i - 55u))))))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            if(i < 14u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((i < 4u) ? ((((i / 2u) % 2u) ? 0.9f : 0.7f) * ((i % 2u) ? 1.0f : -1.0f)) : ((I_TO_F(i - 4u) - 4.5f) * -0.15f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 35u)
            {
                const coreUintW   A     = (i - 14u) % 7u;
                const coreUintW   B     = (i - 14u) / 7u;
                const coreVector2 vBase = coreVector2::Direction((I_TO_F(A) / 6.0f) * (2.0f*PI)) * (A ? 0.2f : 0.0f);

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((B % 2u) ? 0.5f : -0.5f) * ((i < 28u) ? 1.0f : COS(fLifeTime * 2.0f)), 0.0f) + vBase;

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 55u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(((i - 35u) * 2u) % 5u) - 2.0f) * ((i < 45u) ? -0.4f : 0.4f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else if(i < 63u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 55u) % 4u) - 1.5f) * 0.5f + ((i < 59u) ? -0.125f : 0.125f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
                if(!(i % 2u)) pEnemy->InvertX();
            }

            if((i < 45u || i >= 55u) && STAGE_TICK_LIFETIME(10.0f, 0.0f) && (!g_pGame->IsEasy() || (s_iTick < 3u)))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.5f);

                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            if((pEnemy == pCatchObject1) || (pEnemy == pCatchObject2)) return;

            STAGE_LIFETIME(pEnemy, 0.7f, (i < 1u) ? 0.0f : ((i < 6u) ? (0.8f * I_TO_F((i - 1u) % 4u)) : 0.5f))

            const coreSpline2* pPath = (i == 6u) ? pPath2 : pPath1;

            if(pPath == pPath1) STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(((i >= 1u && i < 6u) ? (((i - 1u) % 2u) ? -0.4f : 0.4f) : 0.0f) * ((i < 5u) ? 1.0f : COS(fLifeTime * 2.0f)), 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

            if((i < 6u) && STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 3u; j--; )
                {
                    if(g_pGame->IsEasy() && (j != 1u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 1.0f) * 20.0f) + (1.0f*PI));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.5f);
                }

                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }
        });

        if((m_iStageSub >= 10u) && (m_iStageSub <= 13u) && STAGE_TICK_FREE(g_pGame->IsEasy() ? 1.5f : 2.0f, 0.0f))
        {
            cEnemy* pDummy = pSquad1->GetEnemy(0u);

            for(coreUintW j = 30u; j--; )
            {
                if(g_pGame->IsEasy() && (((j + s_iTick * 2u) / 3u) % 2u)) continue;

                const coreVector2 vPos = coreVector2((I_TO_F(j) - 14.5f + ((s_iTick % 2u) ? -0.25f : 0.25f)) * 0.08f, (j % 2u) ? 1.2f : 1.22f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.5f, pDummy, vPos, vDir)->ChangeSize(1.5f);
            }
        }

        g_pGame->GetBulletManagerPlayer()->ForEachBullet([](cBullet* OUTPUT pBullet)   // prevent annoying multi-bounce (sound)
        {
            if(pBullet->HasStatus(BULLET_STATUS_REFLECTED))
            {
                pBullet->AddStatus(BULLET_STATUS_GHOST);
            }
        });

        STAGE_WAVE(4u, "6-5", {70.0f, 105.0f, 140.0f, 175.0f, 350.0f})   // FÜNFUNDDREISSIG
    },
    STAGE_PRE()
    {
        g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTriangleBullet>();
        g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cFlipBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        g_pGame->KillHelpers();

        this->DisableBull(false);

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
    // keep shooting and charge it (astral chain)
    // - enemies must be positioned in such a way that you have to let go of the king
    // - first few orbs need to introduce player to orb-charge, and orb-attack (TODO 1: and orb-shrink if possible)
    // - orb must shrink fast enough, to make it meaningful
    // - enemies to the left and right of the king should not be too close, otherwise you will accidentally shoot them while dodging
    // TODO 1: force bullet order ? red more important ?
    // TODO 1: continuous sound to improve understanding grow<>shrink
    // TODO 1: turn sparks into waves when the ball is charged ?
    // TODO 1: beginnt sich ab nem bestimmten orb zu bewegen (vaus ball ?) ?
    // TODO 1: how to practically introduce orb shrinking ? through (light) movement ?
    // TODO 1: do not remove energy-effect on king after resurrection ?
    // TODO 1: maybe he is caught in an ice block
    // TODO 1: rotate (all balls) helper around enemy to highlight state
    // TODO 1: MAIN: task-check, helper, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), auf boss übertragen (general, easy, coop), sound, attack size/count/speed, enemy size, object size, background rota/speed
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.2f,0.95f), coreVector2(1.0f,0.0f));
            pPath2->AddStop(coreVector2( 0.0f,0.95f), coreVector2(1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.8f);
                pEnemy->Configure(200, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fRotation, fRotation = 1.0f*PI)
        STAGE_GET_END

        cEnemy* pKing = pSquad1->GetEnemy(0u);

        if(STAGE_BEGINNING)
        {
            this->EnableLoad(pKing);
        }

        const coreUintW iLoadStage = (F_TO_UI(m_afLoadPower[0]) != F_TO_UI(m_afLoadPower[1])) ? F_TO_UI(m_afLoadPower[0]) : 0u;

             if(iLoadStage ==  4u) STAGE_RESURRECT(pSquad2, 0u,  3u)
        else if(iLoadStage ==  7u) STAGE_RESURRECT(pSquad2, 4u,  7u)
        else if(iLoadStage == 10u) STAGE_RESURRECT(pSquad2, 8u, 15u)
        else if(iLoadStage == 12u)
        {
            this->DisableLoad(true);

            pKing->SetBaseColor(COLOR_SHIP_RED);
            pKing->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

            pKing->Kill(false);   // reset lifetime
            pKing->Resurrect();

            g_pSpecialEffects->MacroExplosionColorBig(pKing->GetPosition(), COLOR_ENERGY_RED);

            if(pSquad2->GetNumEnemies() == pSquad2->GetNumEnemiesAlive())
                STAGE_BADGE(2u, BADGE_HARD, pKing->GetPosition())
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.0f)

            fRotation = FMOD(fRotation + m_afLoadPower[0] * 0.7f * TIME, 2.0f*PI);

            if(pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE))
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->DefaultRotate  (fRotation);

                if(STAGE_TICK_TIME(0.6f, 0.75f))   // TODO 1: STAGE_TICK_TIME2 instead ?
                {
                    for(coreUintW j = 0u, je = F_TO_UI(m_afLoadPower[0]); j < je; ++j)
                    {
                        const coreVector2 vPos = m_aLoadRaw[j].GetPosition().xy();
                        const coreVector2 vDir = (g_pGame->FindPlayerDual(s_iTick % 2u)->GetPosition().xy() - vPos).Normalized();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.1f);
                    }
                }
            }
            else
            {
                pEnemy->SetPosition  (coreVector3(SIN(fLifeTime) * 1.0f * FOREGROUND_AREA.x, pEnemy->GetPosition().yz()));
                pEnemy->DefaultRotate(fRotation);

                if(STAGE_TICK_LIFETIME(30.0f, 0.0f) && ((s_iTick % 8u) < 2u))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition().xy();
                    const coreFloat   fBase = I_TO_F(s_iTick) * (0.015f*PI);

                    for(coreUintW j = 6u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 30.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos,  vDir)->ChangeSize(1.1f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.1f, pEnemy, vPos, -vDir)->ChangeSize(1.1f);
                    }
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 4.0f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(I_TO_F(i % 4u) * -0.18f - 0.41f, 0.0f);

            pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);

                 if(i <  4u) pEnemy->Rotate270();
            else if(i <  8u) pEnemy->Rotate270()->InvertX();
            else if(i < 12u) pEnemy->InvertX();
            else if(i < 16u) {}

            const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

            pEnemy->SetDirection(coreVector3(vDir, 0.0f));

            if(STAGE_LIFETIME_AFTER(2.0f) && STAGE_TICK_TIME(0.6f, ((i % 8u) < 4u) ? 0.0f : 0.5f))   // TODO 1: STAGE_TICK_TIME2 instead ?
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
            }
        });

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, bFirstHit, COLL_THIS)
        {
            if(!bFirstHit || (pEnemy->GetID() != cWarriorEnemy::ID)) return;

            this->BumpLoad(I_TO_F(pBullet->GetDamage()) * 0.03f * RCP(I_TO_F(g_pGame->GetNumPlayers())));

            pBullet->Deactivate(true, vIntersection.xy());
        });

        STAGE_WAVE(5u, "6-?", {60.0f, 80.0f, 100.0f, 120.0f, 240.0f})   // SECHSUNDDREISSIG
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

        this->DisableLoad(false);

        STAGE_FINISH_NOW
    });

#endif

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        if(!this->IsStarEnabled(0u))
        {
            // 
            g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
            {
                this->EnableStar(i, pPlayer, coreVector2(0.0f,0.0f));
                this->SetStarLength(i, 0.0f);
            });

            // 
            this->StartSwing(1.0f);
        }

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        g_pEnvironment->SetTargetDirectionNow(coreVector2(0.0f,1.0f));
        g_pEnvironment->SetTargetSpeedNow    (6.0f);

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
        STAGE_BOSS(m_Zeroth, {160.0f, 240.0f, 320.0, 400.0f, 800.0f})   // +5
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
    // story
    if(m_bStory) STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        cTracker* pTracker = g_pGame->GetTracker();

        if(STAGE_BEGINNING)
        {
            pTracker->Resurrect();
            pTracker->EnableWind();
            pTracker->EnableRange();
            pTracker->SetPosition(coreVector3(0.0f,1.5f,0.0f) * FOREGROUND_AREA3);
            pTracker->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        }

        if(STAGE_TIME_POINT(2.0f))
        {
            g_pGame->FadeMusic(0.35f);

            m_pNightmareSound->PlayRelative(this, 0.4f, 1.0f, false, SOUND_EFFECT);
        }

        if(STAGE_TIME_BEFORE(5.0f))
        {
            g_pPostProcessing->SetChroma(STEP(3.0f, 5.0f, m_fStageTime) * 1.5f);

            pTracker->SetPosition(coreVector3(0.0f, LERPB(1.5f, 0.6f, STEP(2.0f, 4.0f, m_fStageTime)), 0.0f) * FOREGROUND_AREA3);
        }
        else if(STAGE_TIME_POINT(5.3f))
        {
            g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,-1.0f));
        }
        else if(STAGE_TIME_POINT(5.6f))
        {
            pTracker->Kill(false);

            g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,1.0f));

            g_pEnvironment->ChangeBackground(cDarkBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);
            g_pEnvironment->SetTargetSpeedNow(0.0f);

            Core::Manager::Resource->UpdateWait();

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
                pPlayer->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            });

            g_pGame->GetBulletManagerPlayer()->ClearBullets(false);
        }
        else if(STAGE_TIME_POINT(5.9f))
        {
            g_pPostProcessing->SetChroma(0.0f);

            g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);
            g_pPostProcessing->SetFrameValue(1.95f);
            g_pPostProcessing->SetFrameAnimation(7.0f);
        }
        else if(STAGE_TIME_POINT(6.7f))
        {
            g_pPostProcessing->SetFrameValue(0.0f);

            this->EnableRanges();
        }
        else if(STAGE_TIME_POINT(7.5f))
        {
            g_pPostProcessing->SetChroma(1.5f);

            g_pEnvironment->ChangeBackground(cDarkBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

            Core::Manager::Resource->UpdateWait();
        }
        else if(STAGE_TIME_POINT(7.8f))
        {
            g_pPostProcessing->SetChroma(0.0f);

            g_pEnvironment->ChangeBackground(cSnowBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);
            g_pEnvironment->SetTargetSpeedNow(ENVIRONMENT_DEFAULT_SPEED);

            Core::Manager::Resource->UpdateWait();

            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                const coreFloat fSide = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;

                pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);
                pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_SHOOT);
                pPlayer->SetPosition(coreVector3(fSide, -0.75f * FOREGROUND_AREA.y, 0.0f));
            });

            this->DisableRanges(false);

            if(m_pNightmareSound->EnableRef(this)) m_pNightmareSound->Stop();
        }
        else if(STAGE_TIME_AFTER(8.2f))
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
        ADD_BIT_EX(g_pSave->EditProgress()->aiState, STATE_STORY_CALOR)
        STAGE_FINISH_NOW
    });

    // ################################################################
    // ################################################################
}