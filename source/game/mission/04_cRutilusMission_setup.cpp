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
// setup the Rutilus mission
void cRutilusMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSpaceBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // teleport everything
    // every representation where the teleporter can be avoided will not work, as the player will always avoid it due to unpredictability, so teleporter need to span across screen
    // dangers of teleportation (especially with changing directions) are hard to assess, so players will avoid using it with the ship
    // when the player has to use the teleporter, the situation needs to be easy and predictable (only one location with enemies)
    // it is important that the player has to use the feature in some way
    //      special care needs to be taken for split-screen coop, players cannot change sides
    // tests were done with rotation (unpredictable), movement (unnecessary), 90 degree difference (unpredictable), infinity (unnecessary)
    // 1: show player the mechanic
    // 2-5: force the player to use teleporter
    // 6: enemies need to move the player into a safe location
    // 7: groups are not in center, to make movement a bit easier (attacking enemies will not attack player)
    // 8-11: 1-2-2-1 pattern
    // (TODO: portale werden am ende zu gegnern, ZeroRanger)
    // TODO: badge: in der mitte am anfang
    // TODO: fix coop, teleportation into other field not possible -> maybe relax and allow it, force teleport with effect back on end, check and change player-area, should make vertical possible again (repair enemy ?)
    // TODO: on vertical, enemy from below may ram into player
    // TODO: get rid of "fake enemy"
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 27u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(2u)
            STAGE_GET_UINT (iTransitionState)
            STAGE_GET_FLOAT(fTransitionTime)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB( 2u)) STAGE_RESSURECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESSURECT(pSquad1,  8u,  9u)
            else if(STAGE_SUB( 4u)) STAGE_RESSURECT(pSquad1, 10u, 11u)
            else if(STAGE_SUB( 5u)) STAGE_RESSURECT(pSquad1, 12u, 13u)
            else if(STAGE_SUB( 6u)) STAGE_RESSURECT(pSquad1, 14u, 15u)
            else if(STAGE_SUB( 7u)) STAGE_RESSURECT(pSquad1, 16u, 21u)
            else if(STAGE_SUB( 8u)) STAGE_RESSURECT(pSquad1, 22u, 22u)
            else if(STAGE_SUB( 9u)) STAGE_RESSURECT(pSquad1, 23u, 23u)
            else if(STAGE_SUB(10u)) STAGE_RESSURECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB(11u)) STAGE_RESSURECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB(12u)) STAGE_RESSURECT(pSquad1, 26u, 26u)   // fake enemy

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;
        }

        if(STAGE_BEGINNING)
        {
            for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
                this->EnableTeleporter(i);
        }

        const coreBool bVertical = false;

        fTransitionTime += 1.0f * Core::System->GetTime();

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                const coreFloat   fTime = MIN(m_fStageTime, 1.2f);
                const coreVector2 vPos  = coreVector2((SIN(fTime / 1.2f * 0.5f*PI) * 2.0f - 1.0f) * (1.2f - fTime), LERP(-0.3f, 0.4f, SIN(fTime / 1.2f * 1.0f*PI))) * FOREGROUND_AREA;

                m_aTeleporter[0].SetPosition(coreVector3( vPos,    0.0f));
                m_aTeleporter[1].SetPosition(coreVector3(-vPos,    0.0f));
                m_aTeleporter[0].SetSize    (coreVector3(1.0f,1.0f,1.0f));
                m_aTeleporter[1].SetSize    (coreVector3(1.0f,1.0f,1.0f));

                if(fTransitionTime >= 1.2f)
                {
                    iTransitionState += 1u;

                    this->SetTeleporterActive(1u);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) g_pSpecialEffects->CreateSplashColor(m_aTeleporter[i].GetPosition(), 50.0f, 10u, RUTILUS_TELEPORTER_COLOR(i));
                }
            }
            else if(iTransitionState == 1u)
            {
                const coreFloat fTime = MIN(m_fStageTime - 1.2f, 1.0f);
                const coreFloat fSize = LERPB(0.0f, 1.0f, fTime) * 1.05f * FOREGROUND_AREA.x;

                m_aTeleporter[0].SetSize(coreVector3(fSize, 1.0f, 1.0f));
                m_aTeleporter[1].SetSize(coreVector3(fSize, 1.0f, 1.0f));

                if(fTransitionTime >= 2.2f)
                {
                    iTransitionState += 1u;
                }
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(iTransitionState == 0u)
            {
                if(fTransitionTime >= 0.5f)
                {
                    iTransitionState += 1u;

                    this->SetTeleporterActive(0u);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__LineEffect(i);
                }
            }
            else if(iTransitionState == 1u)
            {
                const coreFloat fTime = LERPS(0.0f, 1.0f, CLAMP((fTransitionTime - 1.0f) * 0.5f, 0.0f, 1.0f));

                if(bVertical)
                {
                    const coreVector2 vPos = LERP(coreVector2(0.0f,0.3f), coreVector2(0.7f,0.0f), fTime) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2::Direction(fTime * 0.5f*PI);

                    m_aTeleporter[0].SetPosition (coreVector3(-vPos,             0.0f));
                    m_aTeleporter[1].SetPosition (coreVector3( vPos,             0.0f));
                    m_aTeleporter[0].SetDirection(coreVector3(-vDir.InvertedY(), 0.0f));
                    m_aTeleporter[1].SetDirection(coreVector3( vDir,             0.0f));
                }
                else
                {
                    const coreVector2 vPos = LERP(coreVector2(0.0f,0.3f), coreVector2(0.0f,0.7f), fTime) * FOREGROUND_AREA;
                    const coreVector2 vDir = coreVector2::Direction(fTime * 1.0f*PI);

                    m_aTeleporter[0].SetPosition (coreVector3(-vPos, 0.0f));
                    m_aTeleporter[1].SetPosition (coreVector3( vPos, 0.0f));
                    m_aTeleporter[1].SetDirection(coreVector3( vDir, 0.0f));
                }

                if(fTransitionTime >= 3.0f)
                {
                    iTransitionState += 1u;
                }
            }
            else if(iTransitionState == 2u)
            {
                if(fTransitionTime >= 3.5f)
                {
                    iTransitionState += 1u;

                    this->SetTeleporterActive(1u);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__LineEffect(i);
                }
            }
        }
        else if(m_iStageSub == 12u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
            }

            const coreFloat fPos = (LERPB(0.0f, 0.7f, 1.0f - fTransitionTime) + SIN(fTransitionTime * 1.0f*PI) * 0.3f) * FOREGROUND_AREA.y;

            if(bVertical)
            {
                m_aTeleporter[0].SetPosition(coreVector3(-fPos, 0.0f, 0.0f));
                m_aTeleporter[1].SetPosition(coreVector3( fPos, 0.0f, 0.0f));
            }
            else
            {
                m_aTeleporter[0].SetPosition(coreVector3(0.0f, -fPos, 0.0f));
                m_aTeleporter[1].SetPosition(coreVector3(0.0f,  fPos, 0.0f));
            }

            if(fTransitionTime >= 1.0f)
            {
                for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
                    this->DisableTeleporter(i, true);

                pSquad1->GetEnemy(26u)->Kill(false);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 6u) ? 2.0f : ((i >= 14u && i < 16u) ? 3.5f : 0.0f))

            if(i < 14u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = (i < 6u) ? coreVector2((I_TO_F(i) - 2.5f) * 0.2f, ABS(I_TO_F(i) - 2.5f) * 0.05f - 0.025f) : coreVector2(I_TO_F(i % 2u) * 0.2f - 0.82f, 0.2f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  6u) {}
                else if(i <  8u) pEnemy->Rotate270();
                else if(i < 10u) pEnemy->Rotate270()->InvertY();
                else if(i < 12u) pEnemy->Rotate90 ()->InvertY();
                else if(i < 14u) pEnemy->Rotate90 ();
            }
            else if(i < 26u)
            {
                const coreVector2 vFactor = coreVector2(1.0f, (i == 22u || i == 25u) ? -1.0f : 1.0f);
                const coreVector2 vOffset = (i < 16u) ? coreVector2(0.0f,0.2f) : ((i < 22u) ? coreVector2((I_TO_F((i - 16u) % 3u) - 1.0f) * 0.2f + 0.3f, ABS(I_TO_F((i - 16u) % 3u) - 1.0f) * 0.05f + 0.2f) : coreVector2((i % 2u) ? -0.3f : 0.3f, 0.2f));

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 15u) pEnemy->Rotate90 ();
                else if(i < 16u) pEnemy->Rotate270();
                else if(i < 19u) pEnemy->Rotate180();

                if(bVertical) pEnemy->Rotate90();
            }
            else
            {
                pEnemy->SetPosition(coreVector3(1000.0f,1000.0f,0.0f));
            }

            if(STAGE_TICK_LIFETIME(1.25f, 0.0f))
            {
                const coreUintW   iCount = (i < 6u || (i >= 14 && i < 22u)) ? 1u : 5u;
                const coreVector2 vPos   = pEnemy->GetPosition().xy();
                const coreFloat   fBase  = pEnemy->AimAtPlayerSide().Angle();

                for(coreUintW j = iCount; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iCount - 1u) * 0.5f) * 4.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
                }
            }
        });

        STAGE_WAVE("SECHSUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all teleporters
    STAGE_MAIN
    {
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            this->DisableTeleporter(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Quaternion, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Saros, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    STAGE_BOSS(m_Messier, {60.0f, 120.0f, 180.0, 240.0f})
    //});

    // ################################################################
    // ################################################################
}