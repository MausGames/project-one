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
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_AFTER(1.5f)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        g_pEnvironment->ChangeBackground(cSpaceBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
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
    STAGE_MAIN({TAKE_ALWAYS, 0u, 1u})
    {
        if(STAGE_BEGINNING)
        {
            cSpaceBackground* pBackground = d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground());

            pBackground->SetGroundDensity(0u, 0.0f);
            pBackground->SetCoverColor(COLOR_MENU_BLUE);
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // force rotation
    // enemies spawn against turn-direction at beginning, because player stays away from them, and then is already positioned for the first enemy of the next sub-stage
    // enemies spawning against shoot-direction feels too cumbersome
    // enemies are aligned with arrow pattern
    // position can be control a lot here, make sure to have proper player<>enemy placement
    // plate movement is opposite to enemy movement, otherwise annoying attack-bubbles may appear
    // TODO 1: schräges background movement schaut gut aus, von oben rechts oder links ?
    // TODO 1: platten sollten in ein muster auseinander gehen anstatt einfach nur zu verschwinden (vlt. einfach nur rauf runter rauf runter)
    // TODO 1: effect und noise wenn man versucht sich zu drehen (död död, XX)
    // TODO 1: helfer fliegt am anfang casually vorbei (an mittelpunkt) und dropt die erste plate (schräg, e.g. von oben-links ? quer zu background bewegung)
    // TODO 1: badge, another plate flies across the screen with a certain enemy in sight, killing from the plate gives badge
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f),  coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.95f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 52u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.4f);
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
            });
        });

        STAGE_GET_START(4u)
            STAGE_GET_UINT (iTransitionState)
            STAGE_GET_FLOAT(fTransitionTime)
            STAGE_GET_FLOAT(fConveyerTime)
            STAGE_GET_FLOAT(fConveyerSpeed)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 16u, 19u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 20u, 23u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 24u, 27u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 28u, 29u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 30u, 31u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 32u, 33u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 34u, 35u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 36u, 37u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 38u, 39u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 40u, 41u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 42u, 43u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 44u, 45u)
            else if(STAGE_SUB(17u)) STAGE_RESURRECT(pSquad1, 46u, 47u)
            else if(STAGE_SUB(18u)) STAGE_RESURRECT(pSquad1, 48u, 49u)
            else if(STAGE_SUB(19u)) STAGE_RESURRECT(pSquad1, 50u, 51u)

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;

            if(STAGE_CLEARED)
            {
                for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
                    this->DisablePlate(i, true);
            }
        }

        STAGE_FOREACH_PLAYER(pPlayer, i)   // before plates change, positions may not be updated yet
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);

            if(!pPlayer->IsRolling())
            {
                const coreVector2 vPos = pPlayer->GetPosition().xy();

                for(coreUintW j = 0u; j < RUTILUS_PLATES; ++j)
                {
                    coreObject3D& oPlate = m_aPlateRaw[j];
                    if(!oPlate.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                    const coreVector2 vSize = oPlate.GetCollisionRange().xy();
                    const coreVector2 vDiff = MapToAxis(oPlate.GetPosition().xy() - vPos, oPlate.GetDirection().xy());

                    if((ABS(vDiff.x) < vSize.x) && (ABS(vDiff.y) < vSize.y))
                    {
                        pPlayer->SetDirection(oPlate.GetDirection());
                        pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_TURN);
                        break;
                    }
                }
            }
        });

        if(STAGE_BEGINNING)
        {
            this->EnablePlate(0u, 0.0f, 0.0f, 0.5f);
        }

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 2u)
        {
            m_aPlateRaw[0].SetDirection(coreVector3(coreVector2::Direction(LERPB(0.0f*PI, 0.5f*PI, MIN(fTransitionTime, 1.0f))), 0.0f));
            m_avPlateData[0].zw(coreVector2(1.0f,1.0f) * LERPB(0.5f, 0.75f, MIN(fTransitionTime, 1.0f)));
        }
        else if(m_iStageSub == 3u)
        {
            m_aPlateRaw[0].SetDirection(coreVector3(coreVector2::Direction(LERPB(0.5f*PI, 1.0f*PI, MIN(fTransitionTime, 1.0f))), 0.0f));
            m_avPlateData[0].zw(coreVector2(1.0f,1.0f) * LERPB(0.75f, 1.0f, MIN(fTransitionTime, 1.0f)));
        }
        else if(m_iStageSub == 4u)
        {
            m_aPlateRaw[0].SetDirection(coreVector3(coreVector2::Direction(LERPB(1.0f*PI, 1.5f*PI, MIN(fTransitionTime, 1.0f))), 0.0f));
            m_avPlateData[0].zw(coreVector2(1.0f,1.0f));
        }
        else if(m_iStageSub == 5u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                this->EnablePlate(3u, 0.5f, 0.25f, -0.5f);

                this->SetPlateOffset(0u, -0.25f);
                this->SetPlateScale (0u, -0.5f);

                m_aPlateRaw[0].SetDirection(coreVector3( 1.0f,0.0f,0.0f));
                m_aPlateRaw[3].SetDirection(coreVector3(-1.0f,0.0f,0.0f));
            }
        }
        else if(m_iStageSub == 8u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                this->EnablePlate(1u, 0.0f,  -0.125f, -0.25);
                this->EnablePlate(2u, 0.0f,   0.125f, -0.25);
                this->EnablePlate(4u, 0.625f, 0.625f, -0.25);

                this->SetPlateOffset(0u, -0.375f);
                this->SetPlateScale (0u, -0.25);
                this->SetPlateOffset(3u,  0.375f);
                this->SetPlateScale (3u, -0.25);

                m_aPlateRaw[1].SetDirection(coreVector3(-1.0f,0.0f,0.0f));
                m_aPlateRaw[2].SetDirection(coreVector3( 1.0f,0.0f,0.0f));
            }
        }
        else if(m_iStageSub >= 12u)
        {
            fConveyerSpeed = MIN(fConveyerSpeed + 1.0f * TIME, 1.0f);
            fConveyerTime  = FmodRange(fConveyerTime - LERPH3(0.0f, 1.0f, fConveyerSpeed) * TIME, 0.0f, 4.0f);

            for(coreUintW i = 0; i < 4u; ++i)
            {
                m_avPlateData[i].xy(coreVector2(1.0f,1.0f) * FmodRange((I_TO_F(i) - 1.5f - fConveyerTime) * 0.25f, -0.625f, 0.375f));
            }

            m_aPlateRaw[4].SetDirection(m_aPlateRaw[F_TO_UI(fConveyerTime)].GetDirection());
            m_aPlateRaw[4].SetColor3   (m_aPlateRaw[F_TO_UI(fConveyerTime)].GetColor3());
            m_avPlateData[4].xy(coreVector2(1.0f,1.0f) * LERP(0.625f, 0.375f, FRACT(fConveyerTime)));
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.275f * I_TO_F(i % ((i < 28u) ? 4u : 2u)) + (((i < 20u) || (i >= 28u && i < 30u) || (i >= 36u && i < 38u)) ? 0.5f : 0.0f))

            const coreSpline2* pPath = (i < 12u) ? pPath1 : pPath2;

            if(pPath == pPath2) STAGE_REPEAT(pPath->GetTotalDistance())

            if(i < 28u)
            {
                const coreVector2 vFactor = coreVector2(1.0f, ((i >= 16u && i < 18u) || (i >= 20u && i < 28u)) ? -1.0f : 1.0f);
                const coreVector2 vOffset = coreVector2(((I_TO_F(i % 4u) - 1.5f) * 0.275f) + ((i < 20u) ? 0.0f : -0.55f), 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  4u) {}
                else if(i <  8u) pEnemy->Rotate270();
                else if(i < 12u) pEnemy->Rotate180();
                else if(i < 16u) pEnemy->Rotate90 ();
                else if(i < 20u) pEnemy->Rotate90 ();
                else if(i < 24u) pEnemy->Rotate180();
                else if(i < 28u) {}
            }
            else
            {
                coreFloat fSide;
                switch(((i - 28u) / 2u) % 4u)
                {
                default: ASSERT(false)
                case 0u: fSide = 0.275f * -3.5f; break;
                case 1u: fSide = 0.275f *  3.5f; break;
                case 2u: fSide = 0.275f * -1.5f; break;
                case 3u: fSide = 0.275f *  1.5f; break;
                }

                const coreVector2 vFactor = coreVector2(1.0f,  1.0f);
                const coreVector2 vOffset = coreVector2(fSide, 0.0f);

                pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(pEnemy->ReachedDeath())
            {
                for(coreUintW j = 0u; j < 4u; ++j)
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = StepRotated90(j);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.5f, pEnemy, vPos, vDir)->ChangeSize(2.5f);
                }
            }
        });

        STAGE_WAVE("NEUNZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all plates
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
            this->DisablePlate(i, false);

        STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // screen rotation and rotated shooting
    // rotation at the start needs proper introduction, feedback and time, so the player can get accustomed
    // on further rotation-changes waiting time can be reduced to keep total time low, player understands already
    // rotation should not be too fast, the player can lose control, but not too slow, to make the mechanic significant
    // bullets can be made unaffected by rotation, but does not really feel better, bullets rotating with the background created better visual clusters
    // make sure to adjust start-time from diagonally flying enemies
    // TODO 1: draw border elements on rotation (not shake)
    // TODO 1: permanent special effect on helper (auspuff strahl ?)
    // TODO 1: letzte 4 gruppen, spieler rammt rein bei transition, weil die position relativ ist
    // TODO 1: badge: shoot at helper
    // TODO 1: whole border gets particle effect on helper impact (delayed)
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2( 1.15f,-1.15f), coreVector2(-1.0f,1.0f).Normalized());
            pPath2->AddNode(coreVector2(-1.15f, 1.15f), coreVector2(-1.0f,1.0f).Normalized());
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 68u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_GREEN);
            });
        });

        STAGE_GET_START(6u)
            STAGE_GET_UINT (iTransitionState)
            STAGE_GET_FLOAT(fTransitionTime)
            STAGE_GET_FLOAT(fRotationValue)
            STAGE_GET_FLOAT(fRotationSpeed)
            STAGE_GET_FLOAT(fRotationFrom)
            STAGE_GET_FLOAT(fRotationTo)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  8u, 11u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 12u, 15u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 16u, 19u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 20u, 23u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 24u, 27u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 28u, 31u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 32u, 35u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 36u, 39u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 40u, 43u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 44u, 51u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 52u, 55u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 56u, 59u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 60u, 63u)
            else if(STAGE_SUB(16u)) STAGE_RESURRECT(pSquad1, 60u, 63u)
            else if(STAGE_SUB(17u)) STAGE_DELAY_START

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_GREEN);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect();
        }

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                pHelper->SetPosition(coreVector3(LERPB(1.08f, -1.2f, MAX(1.0f - fTransitionTime, 0.0f)) * FOREGROUND_AREA.x, 0.0f, 0.0f));

                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;

                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_BIG, COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                }
            }
            else if(iTransitionState == 1u)
            {
                fRotationValue = LERPB(0.25f*PI, 0.0f*PI, CLAMP(1.0f - (fTransitionTime - 1.5f) / 1.5f, 0.0f, 1.0f));

                if(fTransitionTime >= 3.0f)
                {
                    iTransitionState += 1u;

                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                }
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
            }

            fRotationSpeed = MAX(fRotationSpeed - 0.2f * TIME, -0.4f);
        }
        else if(m_iStageSub == 13u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                fRotationSpeed = -fRotationSpeed;
            }
        }
        else if(m_iStageSub == 17u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                fRotationSpeed = 0.0f;
                fRotationFrom  = (fRotationValue);
                fRotationTo    = (fRotationValue < -1.0f*PI) ? (-2.0f*PI) : (0.0f*PI);
            }

            const coreFloat fTime = MAX(1.0f - fTransitionTime, 0.0f);

            pHelper->SetPosition(coreVector3(LERPB(-1.2f, 1.08f, fTime) * FOREGROUND_AREA.x, 0.0f, 0.0f));

            fRotationValue = LERPB(fRotationTo, fRotationFrom, fTime);

            if(fTransitionTime >= 1.0f)
            {
                pHelper->Kill(false);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                STAGE_DELAY_END
            }
        }

        fRotationValue = FMOD(fRotationValue + fRotationSpeed * TIME, 2.0f*PI);
        const coreVector2 vDirection = coreVector2::Direction(fRotationValue);

        g_pPostProcessing->SetDirectionGame(vDirection);

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.2f * I_TO_F(i % 4u) + ((i < 4u) ? 3.0f : ((i >= 8u && i < 16u) ? -0.3f : ((i >= 20u && i < 24u) ? 2.0f : ((i >= 52u && i < 56u) ? 1.0f : 0.0f)))))

            const coreSpline2* pPath = (i < 8u || (i >= 20u && i < 36u) || i >= 52u) ? pPath1 : pPath2;
            const coreVector2  vAxis = (pPath == pPath1) ? coreVector2(0.0f,1.0f)    : (coreVector2(-1.0f,1.0f) * (1.0f / SQRT2));
            const coreFloat    fSide = (pPath == pPath1) ? ((i % 2u) ? 0.1f : -0.1f) : 0.0f;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f) * ((i < 8u || i >= 20u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = MapToAxis(coreVector2(((i < 8u) ? 0.0f : ((i < 12u) ? -0.5f : ((i < 16u) ? 0.5f : ((i < 20u) ? 0.0f : ((i < 36u) ? -0.5f : 0.0f))))) + fSide, 0.0f), vAxis);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);

                 if(i <  4u) {}
            else if(i <  8u) pEnemy->Rotate270();
            else if(i < 20u) {}
            else if(i < 24u) pEnemy->Rotate90 ();
            else if(i < 28u) {}
            else if(i < 32u) pEnemy->Rotate270();
            else if(i < 36u) pEnemy->Rotate180();
            else if(i < 40u) pEnemy->Rotate270();
            else if(i < 44u) pEnemy->Rotate180();
            else if(i < 48u) pEnemy->Rotate90 ();
            else if(i < 52u) {}
            else if(i < 64u)
            {
                pEnemy->SetPosition (coreVector3(MapToAxis(pEnemy->GetPosition ().xy(), vDirection), 0.0f));
                pEnemy->SetDirection(coreVector3(MapToAxis(pEnemy->GetDirection().xy(), vDirection), 0.0f));
            }

            if(STAGE_TICK_LIFETIME_BASE(6.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerSide().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
            }
        });

        STAGE_WAVE("ZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset screen rotation
    STAGE_MAIN({TAKE_ALWAYS, 1u})
    {
        g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,1.0f));

        g_pGame->GetHelper(ELEMENT_GREEN)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION, 1u})
    {
        d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground())->SetCoverColor(LERP(COLOR_MENU_BLUE, COLOR_MENU_MAGENTA, m_fStageTime));

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 2u, 3u})
    {
        if(STAGE_BEGINNING)
        {
            d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground())->SetCoverColor(COLOR_MENU_MAGENTA);
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // teleport everything
    // every representation where the teleporter can be avoided will not work, as the player will always avoid it due to unpredictability, so teleporter need to span across screen
    // dangers of teleportation (especially with changing directions) are hard to assess, so players will avoid using it with the ship
    // when the player has to use the teleporter, the situation needs to be easy and predictable (only one location with enemies)
    // it is important that the player has to use the feature in some way
    //      special care needs to be taken for split-screen coop, players cannot change sides
    // tests were done with rotation (unpredictable), movement (unnecessary), 90 degree difference (unpredictable), infinity (unnecessary)
    // moving both portals into their direction or against it, can cause space-folding or -expansion which is physically impossible (e.g. folding would catch objects in the portal and crush them)
    // 1: show player the mechanic
    // 2-5: force the player to use teleporter
    // 6: enemies need to move the player into a safe location
    // 7: groups are not in center, to make movement a bit easier (attacking enemies will not attack player)
    // 8-11: 1-2-2-1 pattern
    // (TODO 1: portale werden am ende zu gegnern, ZeroRanger)
    // TODO 1: badge: in der mitte am anfang
    // TODO 1: fix coop, teleportation into other field not possible -> maybe relax and allow it, force teleport with effect back on end, check and change player-area, should make vertical possible again (repair enemy ?)
    // TODO 1: on vertical, enemy from below may ram into player
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 26u)
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
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  8u,  9u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1, 10u, 11u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1, 12u, 13u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1, 14u, 15u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1, 16u, 21u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 22u, 22u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 23u, 23u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 24u, 24u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 25u, 25u)
            else if(STAGE_SUB(12u)) STAGE_DELAY_START

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;
        }

        cHelper* pHelperOrange = g_pGame->GetHelper(ELEMENT_ORANGE);
        cHelper* pHelperBlue   = g_pGame->GetHelper(ELEMENT_BLUE);

        if(STAGE_BEGINNING)
        {
            pHelperOrange->Resurrect();
            pHelperBlue  ->Resurrect();
        }

        const coreBool bVertical = false;

        fTransitionTime += 1.0f * TIME;

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

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) this->EnableTeleporter(i);
                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) g_pSpecialEffects->CreateSplashColor(m_aTeleporter[i].GetPosition(), 50.0f, 10u, RUTILUS_TELEPORTER_COLOR(i));
                    this->SetTeleporterActive(1u);
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

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__TeleporterEffect(i);
                    this->SetTeleporterActive(0u);
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

                    for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i) cRutilusMission::__TeleporterEffect(i);
                    this->SetTeleporterActive(1u);
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

                pHelperOrange->Kill(false);
                pHelperBlue  ->Kill(false);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                STAGE_DELAY_END
            }
        }

        pHelperOrange->SetPosition(m_aTeleporter[0].GetPosition());
        pHelperBlue  ->SetPosition(m_aTeleporter[1].GetPosition());

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

            if(STAGE_TICK_LIFETIME(1.2f, 0.0f))
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

        STAGE_WAVE("EINUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all teleporters
    STAGE_MAIN({TAKE_ALWAYS, 2u})
    {
        for(coreUintW i = 0u; i < RUTILUS_TELEPORTER; ++i)
            this->DisableTeleporter(i, false);

        g_pGame->GetHelper(ELEMENT_ORANGE)->Kill(false);
        g_pGame->GetHelper(ELEMENT_BLUE)  ->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // slowdown areas
    // bubble works as shield to hide from attacks
    // side-moving sphere works with continue attack, but not with wave-attack, front-moving sphere never works, round-moving sphere not sure
    // first group shows the speed-reduction without attacks, next one with attacks
    // enemies should attack from the opposite side in coop, so target-player can hide behind bubble
    // NOTE: this stage was made while doing "Another Side" in parallel, so it might need more exploration
    // TODO 1: effekt um verlangsamte objekte herum (schein)
    // TODO 1: reduce bullet-animation + dark-animation speed
    // TODO 1: weapon-shoot-speed, enemy-shoot-speed (wenn nötig)
    // TODO 1: make a sub-stage with inverted speed and enemies scattered around (1.5f, 0.5f) (but how to put player into the bubble ? do I even want to if 0.5 is outside ?)
    // TODO 1: multiple bubbles ? (2) in circles
    // TODO 1: multiple smaller bubbles are created from center (like bubbles) and move around slightly
    // TODO 1: change intro helper movement
    // TODO 1: shift time-bubble to the side on disable ?
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        constexpr coreUintW iRegisterSize = GAME_PLAYERS + 1u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 19u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.3f);
                pEnemy->Configure(30, COLOR_SHIP_MAGENTA);

                if(i < 4u)
                {
                    pEnemy->SetPosition (coreVector3((I_TO_F(i) - 1.5f) * 0.2f, 1.3f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
                }
                else if(i < 10u)
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F((i - 4u) % 3u) * ((4.0f/6.0f)*PI) - ((1.0f/6.0f)*PI)) * coreVector2((i < 7u) ? -1.0f : 1.0f, 1.0f);

                    pEnemy->SetPosition (coreVector3(vDir *  1.4f, 0.0f) * FOREGROUND_AREA3);
                    pEnemy->SetDirection(coreVector3(vDir * -1.0f, 0.0f));
                }
            });
        });

        STAGE_GET_START(2u * iRegisterSize + 4u)
            STAGE_GET_UINT_ARRAY (aiRegisterID,    iRegisterSize)
            STAGE_GET_FLOAT_ARRAY(aiRegisterSpeed, iRegisterSize)
            STAGE_GET_UINT       (iTransitionState)
            STAGE_GET_FLOAT      (fTransitionTime)
            STAGE_GET_FLOAT      (fAreaMove)
            STAGE_GET_FLOAT      (fAreaLerp)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  4u,  4u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  5u,  5u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  6u,  6u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1, 10u, 10u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 11u, 11u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 12u, 12u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 13u, 14u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 15u, 15u)
            else if(STAGE_SUB(13u)) STAGE_RESURRECT(pSquad1, 16u, 16u)
            else if(STAGE_SUB(14u)) STAGE_RESURRECT(pSquad1, 17u, 17u)
            else if(STAGE_SUB(15u)) STAGE_RESURRECT(pSquad1, 18u, 18u)
            else if(STAGE_SUB(16u)) STAGE_DELAY_START

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_PURPLE);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect();
        }

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                const coreFloat   fTime = MIN(fTransitionTime * 0.5f, 1.0f);
                const coreVector2 vDir  = coreVector2::Direction(LERP(1.5f*PI, -0.5f*PI, fTime));

                pHelper->SetPosition(coreVector3(vDir * LERP(-1.2f, 0.0f, fTime) * FOREGROUND_AREA.x, 0.0f));

                if(fTransitionTime >= 2.0f)
                {
                    iTransitionState += 1u;

                    this->EnableArea();
                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_PURPLE);
                }
            }
        }
        else if((m_iStageSub >= 8u) && (m_iStageSub < 16u))
        {
            fAreaMove += 1.0f * TIME;

            if(m_iStageSub < 12u) fAreaLerp = MIN(fAreaLerp + 0.5f * TIME, 1.0f);
                             else fAreaLerp = MAX(fAreaLerp - 0.5f * TIME, 0.0f);

            pHelper->SetPosition(coreVector3(SIN(fAreaMove) * 0.5f * FOREGROUND_AREA.x * LERPH3(0.0f, 1.0f, fAreaLerp), 0.0f, 0.0f));
        }
        else if(m_iStageSub == 16u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                this->DisableArea(true);
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_PURPLE);
            }

            if(fTransitionTime >= 0.6f)
            {
                pHelper->Kill(false);

                g_pSpecialEffects->MacroExplosionColorBig(pHelper->GetPosition(), COLOR_ENERGY_PURPLE);

                STAGE_DELAY_END
            }
        }

        m_aArea[0].SetPosition(pHelper->GetPosition());

        const coreVector2 vTestPos    = m_aArea[0].GetPosition().xy();
        const coreFloat   fTestFromSq = POW2(m_aArea[1].GetSize().x) - 100.0f;   // inner sphere
        const coreFloat   fTestToSq   = POW2(m_aArea[1].GetSize().x) +  50.0f;
        const coreFloat   fSpeedSlow  = 0.2f;
        const coreFloat   fSpeedFast  = 1.0f;

        const auto nCalcSpeed = [&](const coreVector2 vPosition)
        {
            const coreVector2 vDiff = vPosition - vTestPos;
            return LERP(fSpeedSlow, fSpeedFast, STEPH3(fTestFromSq, fTestToSq, vDiff.LengthSq()));
        };

        if(m_aArea[0].IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetSpeed(nCalcSpeed(pPlayer->GetPosition().xy()));
            });

            const auto nBulletSlowFunc = [&](cBullet* OUTPUT pBullet)
            {
                coreFloat fBase = 0.0f;
                for(coreUintW i = 0u; i < iRegisterSize; ++i)
                {
                    if(aiRegisterID[i] == 0u)
                    {
                        aiRegisterID   [i] = pBullet->GetID();
                        aiRegisterSpeed[i] = pBullet->GetSpeed();
                    }
                    if(aiRegisterID[i] == coreUint32(pBullet->GetID()))
                    {
                        fBase = aiRegisterSpeed[i];
                        break;
                    }
                }
                ASSERT(fBase)

                pBullet->SetSpeed(fBase * nCalcSpeed(pBullet->GetPosition().xy()));
            };
            g_pGame->GetBulletManagerPlayer()->ForEachBullet(nBulletSlowFunc);
            g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nBulletSlowFunc);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 4u) ? 2.0f : 0.0f)

            if(i < 10u)
            {
                if(STAGE_LIFETIME_AFTER(0.0f)) pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 25.0f * nCalcSpeed(pEnemy->GetPosition().xy()));

                coreVector2       vCurPos = pEnemy->GetPosition ().xy();
                const coreVector2 vCurDir = pEnemy->GetDirection().xy();

                     if((vCurPos.x < -FOREGROUND_AREA.x * 1.2f) && (vCurDir.x < 0.0f)) vCurPos.x =  ABS(vCurPos.x);
                else if((vCurPos.x >  FOREGROUND_AREA.x * 1.2f) && (vCurDir.x > 0.0f)) vCurPos.x = -ABS(vCurPos.x);
                     if((vCurPos.y < -FOREGROUND_AREA.y * 1.2f) && (vCurDir.y < 0.0f)) vCurPos.y =  ABS(vCurPos.y);
                else if((vCurPos.y >  FOREGROUND_AREA.y * 1.2f) && (vCurDir.y > 0.0f)) vCurPos.y = -ABS(vCurPos.y);

                pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
            }
            else if(i < 15u)
            {
                coreFloat fSide;
                switch(i - 10u)
                {
                default: ASSERT(false)
                case 0u: fSide =  0.00f; break;
                case 1u: fSide = -0.75f; break;
                case 2u: fSide =  0.75f; break;
                case 3u: fSide = -0.25f; break;
                case 4u: fSide =  0.25f; break;
                }
                const coreVector2 vFactor = coreVector2(1.0f,  1.0f);
                const coreVector2 vOffset = coreVector2(fSide, 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i == 15u) {}
                else if(i == 16u) pEnemy->Rotate90 ();
                else if(i == 17u) pEnemy->Rotate180();
                else if(i == 18u) pEnemy->Rotate270();
            }

            if(i < 4u)
            {
            }
            else if(i < 15u)
            {
                if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerSideRev().Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                }
            }
            else
            {
                if(STAGE_TICK_LIFETIME(0.9f, 0.0f))
                {
                    const coreVector2 vPos  = pEnemy->GetPosition().xy();
                    const coreFloat   fBase = pEnemy->AimAtPlayerDual((i < 17u) ? 0u : 1u).Angle();

                    for(coreUintW j = 30u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 14.5f) * 4.0f) + fBase);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
                    }
                }
            }
        });

        STAGE_WAVE("ZWEIUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all areas
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        this->DisableArea(false);

        g_pGame->GetHelper(ELEMENT_PURPLE)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION, 3u})
    {
        d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground())->SetCoverColor(LERP(COLOR_MENU_MAGENTA, COLOR_MENU_RED, m_fStageTime));

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS, 4u, 5u, 10u})
    {
        if(STAGE_BEGINNING)
        {
            d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground())->SetCoverColor(COLOR_MENU_RED);
        }

        STAGE_FINISH_PLAY
    });

    // ################################################################
    // gravitation influences bullets
    // gravity both requires to get near to enemies, and allows to shoot in a curve
    // geschoss-gruppe muss mit selber flugbahn erzeugt werden um die gravitation leichter nachvollziehbar zu machen
    // bewegende gegner sind zu leicht zu treffen
    // gravity with distance-falloff is too inconsistent regarding gameplay (too far away is boring, to near is too unpredictable)
    // speed-preservation is easy to grasp, but the created flight-patterns are too boring
    // TODO 1: gravitation should work equally with all bullet types (basis-speed verwenden, von cWeapon, oder eher bullet, muss ich eh speichern für längen-veränderung)
    // TODO 1: distortion for waves ?
    // TODO 1: partikel die in die mitte gezogen werden, mit zusätzlichen waves
    // TODO 1: only half of the bullet wave is created for the line enemies, also not the full bullet-tripplet
    // TODO 1: use ParaLerp for disappearance
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.3f,1.3f), coreVector2(1.0f,-1.0f).Normalized());
            pPath1->AddStop(coreVector2(-0.9f,0.9f), coreVector2(1.0f,-1.0f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.3f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(2u * GAME_PLAYERS + 2u)
            STAGE_GET_VEC2_ARRAY(avTarget, GAME_PLAYERS)
            STAGE_GET_UINT      (iTransitionState)
            STAGE_GET_FLOAT     (fTransitionTime)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 19u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 20u, 27u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 28u, 35u)
            else if(STAGE_SUB(6u)) STAGE_DELAY_START

            iTransitionState = 0u;
            fTransitionTime  = 0.0f;
        }

        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_RED);

        if(STAGE_BEGINNING)
        {
            pHelper->Resurrect();
        }

        fTransitionTime += 1.0f * TIME;

        if(m_iStageSub == 1u)
        {
            if(iTransitionState == 0u)
            {
                pHelper->SetPosition(coreVector3(0.0f, LERPB(-1.2f, 0.0f, MIN(fTransitionTime, 1.0f)) * FOREGROUND_AREA.y, 0.0f));

                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;

                    this->EnableWave();
                    g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
                }
            }
        }
        else if(m_iStageSub == 3u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                ADD_BIT(m_iWaveDir, 1u)
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_SMALL, COLOR_ENERGY_RED);
            }
        }
        else if(m_iStageSub == 5u)
        {
            if(iTransitionState == 0u)
            {
                pHelper->SetPosition(coreVector3(0.0f, LERPB(0.0f, -0.95f, MIN(fTransitionTime, 1.0f)) * FOREGROUND_AREA.y, 0.0f));

                if(fTransitionTime >= 1.0f)
                {
                    iTransitionState += 1u;
                }
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(iTransitionState == 0u)
            {
                iTransitionState += 1u;

                g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                this->DisableWave(true);
                g_pSpecialEffects->CreateSplashColor(pHelper->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_RED);
            }

            const coreFloat fTime = fTransitionTime * 1.9f;

            pHelper->SetPosition(coreVector3(0.0f, (LERPB(-0.95f, 0.1f, fTime) - fTime) * FOREGROUND_AREA.y, 0.0f));

            if(fTransitionTime >= 0.7f)
            {
                pHelper->Kill(false);

                STAGE_DELAY_END
            }
        }

        if(HAS_BIT(m_iWaveActive, RUTILUS_WAVES))
        {
            const coreFloat fSign = ((m_iStageSub >= 3u) ? 1.0f : -1.0f);

            coreFloat fPower = 0.0f;
            const auto nGravityFunc = [&](cBullet* OUTPUT pBullet)
            {
                const coreVector2 vDiff = pHelper->GetPosition().xy() - pBullet->GetPosition().xy();
                const coreVector2 vPush = (vDiff * fSign).Normalized();
                const coreVector2 vMove = pBullet->GetFlyDir() * pBullet->GetSpeed() + vPush * (fPower * TIME * (60.0f/16.0f));

                pBullet->SetFlyDir(vMove.Normalized());
                pBullet->SetSpeed (vMove.Length());

                if(pBullet->GetFlyTime() >= 5.0f)
                    pBullet->Deactivate(true);
            };

            fPower = 6.0f;
            g_pGame->GetBulletManagerPlayer()->ForEachBullet(nGravityFunc);

            fPower = 0.1f;
            g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nGravityFunc);

            m_aWaveRaw[0].SetPosition(pHelper->GetPosition());
        }

        if(STAGE_TICK_FREE(9.0f, 0.0f) && ((s_iTick % 9u) == 6u))
        {
            STAGE_FOREACH_PLAYER_ALL(pPlayer, i)
            {
                avTarget[i] = pPlayer->GetPosition().xy();
            });
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 12u) ? 1.0f : (1.0f + 0.2f * I_TO_F((i - 12u) % 8u)))

            if(i < 12u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((i % 3u) == 1u) ? 0.3f : 0.0f, ((i % 3u) == 2u) ? -0.3f : 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                     if(i <  3u) {}
                else if(i <  6u) pEnemy->Rotate90 ();
                else if(i <  9u) pEnemy->Rotate180();
                else if(i < 12u) pEnemy->Rotate270();
            }
            else if(i < 36u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 12u) % 8u) - 3.5f) * 0.25f, 0.0f);

                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            }

            if(STAGE_TICK_TIME(9.0f, 0.0f) && ((s_iTick % 9u) >= 6u))
            {
                const coreVector2 vTarget = avTarget[pEnemy->NearestPlayerSide() - g_pGame->GetPlayer(0u)];

                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = (vTarget - vPos).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.1f, pEnemy, vPos, vDir)->ChangeSize(1.4f);
            }
        });

        STAGE_WAVE("DREIUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all waves
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        this->DisableWave(false);

        g_pGame->GetHelper(ELEMENT_RED)->Kill(false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // meteor split-up with enemies at the end
    // wenn zerstört fliegt gegner in richtung wo er raus schaut dreht sich und schießt wield um sich, verhält sich sonst wie meteorit
    // bounce zwischen meteoriten is total nicht nachvollziehbar, Ikaruga und RSG haben sowas nicht (nur bounce mit wand oder konsistente aufteilung)
    // meteoriten mit gegnern darin müssen etwas mehr aushalten um nicht unabsichtlich beim erzeugen schon zerstört zu werden
    // TODO 1: einige gegner ohne dass sie im meteor stecken (am anfang)
    // TODO 1: entweder hier oder bei boss, die kleinen meteoriten, wenn sie zerstört werden fliegen auf den bildschirm und erzeugen kleine cracks (keine distortion, nur decal)
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        constexpr coreUintW iNumMedium = 8u;
        constexpr coreUintW iNumSmall  = 25u + 4u * iNumMedium;
        constexpr coreUintW iNumTotal  = iNumMedium + iNumSmall;

        STAGE_ADD_SQUAD(pSquad1, cMeteorEnemy, iNumTotal)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(1, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->Configure(30, COLOR_SHIP_MAGENTA);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            });
        });

        STAGE_GET_START(iNumTotal * 2u + 13u)
            STAGE_GET_VEC2_ARRAY (avMove, iNumTotal)
            STAGE_GET_FLOAT_ARRAY(afAddRotation, 4u)
            STAGE_GET_FLOAT_ARRAY(afAddOffset,   4u)
            STAGE_GET_UINT       (iAddIndex)
            STAGE_GET_UINT       (iSpawnMedium, iSpawnMedium = 1u)
            STAGE_GET_UINT       (iSpawnSmall,  iSpawnSmall  = iNumMedium)
            STAGE_GET_UINT       (iSpawnTotal)
            STAGE_GET_UINT       (iImpact)
        STAGE_GET_END

        const coreFloat fRotation = FMOD(m_fStageTime * 1.1f, 2.0f*PI);

        if(iSpawnTotal < 30u)
        {
            if(STAGE_BEGINNING || STAGE_TICK_FREE(5.0f, 0.0f))
            {
                const coreBool  bMedium = ((iSpawnTotal % 6u) == 5u);
                const coreUintW iIndex  = bMedium ? (iSpawnMedium++) : (iSpawnSmall++);

                cEnemy* pNewEnemy = pSquad1->GetEnemy(iIndex);

                pNewEnemy->SetPosition (coreVector3(SIN(I_TO_F(iSpawnTotal) * 1.5f), 1.3f, 0.0f) * FOREGROUND_AREA3);
                pNewEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * (bMedium ? 5.0f : 3.0f));
                pNewEnemy->SetMaxHealth(bMedium ? 30 : 4);
                pNewEnemy->Resurrect();

                avMove[iIndex] = coreVector2(-SIGN(pNewEnemy->GetPosition().x), -2.0f).Normalized();

                iSpawnTotal += 1u;
            }
        }
        else if(iSpawnTotal == 30u)
        {
            cEnemy* pNewEnemy = pSquad1->GetEnemy(0u);

            pNewEnemy->SetPosition (coreVector3(0.0f,1.5f,0.0f) * FOREGROUND_AREA3);
            pNewEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 11.0f);
            pNewEnemy->SetMaxHealth(120);
            pNewEnemy->Resurrect();

            avMove[0] = coreVector2(0.0f,-1.0f);

            iSpawnTotal += 1u;
        }

#if 0
        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_FOREACH_ENEMY(pSquad1, pEnemy2, j)
            {
                if(j <= i) return;

                const coreVector2 vDiff   = (pEnemy->GetPosition().xy()   - pEnemy2->GetPosition().xy());
                const coreFloat   fRadius = (pEnemy->GetCollisionRadius() + pEnemy2->GetCollisionRadius()) * 0.7f;

                if(vDiff.LengthSq() < POW2(fRadius))
                {
                    const coreVector2 vNormal = vDiff.Normalized();
                    const coreVector2 vCopy1  = avMove[i];
                    const coreVector2 vCopy2  = avMove[j];

                    if(i) avMove[i] = (coreVector2::Reflect(vCopy1,  vNormal) + vCopy2 * MAX(coreVector2::Dot(vCopy2/*.Normalized()*/,  vNormal), 0.0f)).Normalized();
                          avMove[j] = (coreVector2::Reflect(vCopy2, -vNormal) + vCopy1 * MAX(coreVector2::Dot(vCopy1/*.Normalized()*/, -vNormal), 0.0f)).Normalized();

                    if(!i) pEnemy2->SetPosition(pEnemy->GetPosition() - coreVector3(vNormal * fRadius, 0.0f));
                }
            });
        });
#endif

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreBool bBig    = (i == 0u);
            const coreBool bMedium = (i < iNumMedium);

            if(pEnemy->ReachedDeath())
            {
                if(bBig)
                {
                    ASSERT(!iAddIndex)
                    iAddIndex = iSpawnSmall;

                    for(coreUintW j = 0u; j < 4u; ++j)
                    {
                        pSquad2->GetEnemy(j)->Resurrect();
                    }

                    for(coreUintW j = 0u; j < 2u; ++j)
                    {
                        const coreUintW   iIndex = (iSpawnMedium++);
                        const coreVector2 vDir   = coreVector2(j ? -1.0f : 1.0f, 2.0f).Normalized();

                        cEnemy* pNewEnemy = pSquad1->GetEnemy(iIndex);

                        pNewEnemy->SetPosition (pEnemy->GetPosition() + coreVector3(vDir * 2.0f, 0.0f));
                        pNewEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
                        pNewEnemy->SetMaxHealth(30);
                        pNewEnemy->Resurrect();

                        avMove[iIndex] = vDir;
                    }
                }

                if(bMedium)
                {
                    for(coreUintW j = 0u; j < 4u; ++j)
                    {
                        const coreUintW   iIndex = (iSpawnSmall++);
                        const coreVector2 vDir   = (coreVector2::Direction(I_TO_F(j) * (0.5f*PI) + (0.25f*PI)) * coreVector2(1.0f,2.0f)).Normalized();

                        cEnemy* pNewEnemy = pSquad1->GetEnemy(iIndex);

                        pNewEnemy->SetPosition (pEnemy->GetPosition() + coreVector3(vDir * 2.0f, 0.0f));
                        pNewEnemy->SetSize     (coreVector3(1.0f,1.0f,1.0f) * 3.0f);
                        pNewEnemy->SetMaxHealth(bBig ? 30 : 4);
                        pNewEnemy->Resurrect();

                        avMove[iIndex] = vDir;
                    }
                }

                pEnemy->Kill(true);
            }

            if(bBig && (pEnemy->GetPosition().y < FOREGROUND_AREA.y * -1.0f))
            {
                if(!iImpact)
                {
                    iImpact = 1u;
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                }
            }
            else
            {
                pEnemy->SetDirection(coreVector3(coreVector2::Direction((fRotation + I_TO_F(i)) * (bMedium ? -1.0f : 1.0f)), 0.0f));

                if(avMove[i])
                {
                    const coreFloat fRadius = pEnemy->GetCollisionRadius();

                    coreVector2 vCurPos = pEnemy->GetPosition().xy();
                    coreVector2 vCurDir = avMove[i];

                    vCurPos += vCurDir * ((bBig ? 50.0f : (bMedium ? 20.0f : 25.0f)) * TIME);

                    if(!bBig)
                    {
                             if((vCurPos.x < -(FOREGROUND_AREA.x * 1.2f - fRadius)) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
                        else if((vCurPos.x >  (FOREGROUND_AREA.x * 1.2f - fRadius)) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                             if((vCurPos.y < -(FOREGROUND_AREA.y * 1.2f - fRadius)) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
                        else if((vCurPos.y >  (FOREGROUND_AREA.y * 1.2f - fRadius)) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);
                    }

                    pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
                    avMove[i] = vCurDir;
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUintW iIndex  = iAddIndex + i;
            const cEnemy*   pParent = pSquad1->GetEnemy(iIndex);
            coreVector2&    vMove   = avMove[iIndex];

            if(pParent->HasStatus(ENEMY_STATUS_DEAD))
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST);

                    vMove = -pEnemy->GetDirection().xy();

                    afAddRotation[i] = fRotation;
                    afAddOffset  [i] = fLifeTimeBase;
                }

                if(vMove)
                {
                    coreVector2 vCurPos = pEnemy->GetPosition().xy();
                    coreVector2 vCurDir = vMove;

                    vCurPos += vCurDir * (25.0f * TIME);

                         if((vCurPos.x < -FOREGROUND_AREA.x * 1.0f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
                    else if((vCurPos.x >  FOREGROUND_AREA.x * 1.0f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                         if((vCurPos.y < -FOREGROUND_AREA.y * 1.0f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
                    else if((vCurPos.y >  FOREGROUND_AREA.y * 1.0f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);

                    pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
                    vMove = vCurDir;
                }

                afAddRotation[i] = FMOD(afAddRotation[i] - 2.4f * TIME, -2.0f*PI);

                pEnemy->SetDirection(coreVector3(coreVector2::Direction((afAddRotation[i] + I_TO_F(iIndex)) * 1.0f), 0.0f));

                if(STAGE_TICK_LIFETIME_BASE(26.0f, 26.0f * afAddOffset[i]) && ((s_iTick % 4u) < 2u))
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                }
            }
            else
            {
                const coreVector3 vOffset = pParent->GetRotation().QuatApply(coreVector3(0.0f,-3.2f,0.0f));

                pEnemy->SetPosition   (pParent->GetPosition   () + vOffset);
                pEnemy->SetDirection  (pParent->GetDirection  ());
                pEnemy->SetOrientation(pParent->GetOrientation());
            }
        });

        STAGE_WAVE("VIERUNDZWANZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // boss
    STAGE_MAIN({TAKE_ALWAYS, 10u})
    {
        if(STAGE_BEGINNING)
        {
            d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground())->SetCoverColor(COLOR_MENU_RED);
            g_pEnvironment->SetTargetSpeed(0.0f);
        }

        UNUSED STAGE_ADD_SQUAD(pSquad1, cMeteorEnemy, MESSIER_ENEMIES)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 3.0f); // 5.0f 3.0f    
                pEnemy->Configure(1, COLOR_SHIP_RED);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_BOSS(m_Messier, {60.0f, 120.0f, 180.0, 240.0f})
    });

    // ################################################################
    // end
    STAGE_MAIN({TAKE_MISSION})
    {
        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // ################################################################
}