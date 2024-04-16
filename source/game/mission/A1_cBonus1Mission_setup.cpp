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
// setup the Bonus1 mission
void cBonus1Mission::__SetupOwn()
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
        g_pEnvironment->ChangeBackground(cDesertBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

        g_pEnvironment->SetTargetDirectionNow(ENVIRONMENT_DEFAULT_DIRECTION);
        g_pEnvironment->SetTargetSideNow     (ENVIRONMENT_DEFAULT_SIDE);
        g_pEnvironment->SetTargetSpeedNow    (6.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
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
    // hide from super laser
    // blöcke die von einer seite schützen, und geschosse aufhalten (spieler, gegner)
    // 90 degree, then 45 degree
    // 90 degree flip, then 180 degree flip
    // laser dreht sich 360
    // gegner der für angriff verantwortlich ist kommt von unten ins bild wärend er schießt, blöcke werden zerstört, dodge hinter ihn
    // coop
    //   |        |
    //   | O    O | O
    // O |        |
    //   |
    // zwei quads (für schräge ecken) mit tiefe zeichnen und depth-test verwenden
    // 
    // partikel gezogen ()  werden gegen schussrichtung eingesaugt, bei schuss muss er linear nach in schussrichtung gehn und sich ausbreiten in weite
    // unter gegen, geschosse, container
    //
    // container in mitte
    // laser von oben und folgt mustern und verfolgt spieler
    // einzelne schüsse (mit druckwelle)
    // laser ändert winkel im kreis herum und schwingt hin und her an container vorbei
    // laser wird super-dick
    // fat laser with warning: from top, left, right, bottom, top -> and around 90 degree -> ball moves down enemies above -> ball moves up enemies below
    // strahl wird immer kleiner, dann kommt ein kleiner gegner mit dem strahl den man noch töten muss
    // 
    // shelter werden durch erste schüsse erzeugt, die einzeln sind, danach kontinuierlich
    // super-laser wird durch gegen-richtungs ansaugen angezeigt
    // gegner sollen sich bewegen und kurven fahren
    // it should not be possible to stand in the same shadow for two consecutive big shots
    // TODO: add border objects below each border (2x per shelter) instead of the wobbling
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        UNUSED STAGE_ADD_PATH(pPath1)               
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-0.5f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2( 0.5f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        UNUSED STAGE_ADD_PATH(pPath3)               
        {
            pPath3->Reserve(3u);
            pPath3->AddNode(coreVector2(-0.5f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2( 0.5f, 0.0f), coreVector2(0.0f,-1.0f));
            pPath3->AddNode(coreVector2( 0.5f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath3->Refine();
        });

        STAGE_ADD_PATH(pPath4)
        {
            pPath4->Reserve(2u);
            pPath4->AddNode(coreVector2(0.0f,1.2f), coreVector2(0.0f,-1.0f));
            pPath4->AddNode(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
            pPath4->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 100u)                            
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.6f);
                pEnemy->Configure(4, 0u, cIntroMission::RetrieveEnemyColor(i));
            });
        });

        STAGE_GET_START(6u)
            STAGE_GET_FLOAT(fShelterTime)
            STAGE_GET_FLOAT(fBeamDuration)
            STAGE_GET_FLOAT(fBeamFade, fBeamFade = 1.0f)
            STAGE_GET_UINT (iChanged)
            STAGE_GET_FLOAT(fDelay)
            STAGE_GET_FLOAT(fRotation)
        STAGE_GET_END

        const auto nShootBeamFunc = [&](const coreFloat fDuration, const coreFloat fFadeIn, const coreFloat fFadeOut)
        {
            fBeamDuration = fDuration;
            fBeamFade     = fFadeOut;

            this->FadeBeam(true, fFadeIn);
            
            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_EFFECT_DUST);
        };

        if(fBeamDuration)
        {
            fBeamDuration = MAX0(fBeamDuration - 1.0f * TIME);
            if(!fBeamDuration)
            {
                this->FadeBeam(false, fBeamFade);
            }
        }

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  9u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 10u, 19u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 29u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 30u, 39u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 50u, 65u)

            if(m_iStageSub > 2u)
            {
                fBeamDuration = 0.0f;
                if(m_fBeamTime >= 1.0f) this->FadeBeam(false, fBeamFade);
            }

            iChanged  = 0u;
            fRotation = 0.0f;
        }

        cHelper* pHelperBlue  = g_pGame->GetHelper(ELEMENT_BLUE);
        cHelper* pHelperGreen = g_pGame->GetHelper(ELEMENT_GREEN);

        if(STAGE_BEGINNING)
        {
            this->EnableBeam();
            this->EnableShelter(0u);
            this->EnableShelter(1u);

            pHelperBlue ->Resurrect(false);
            pHelperGreen->Resurrect(false);
        }

        if(m_iStageSub >= 2u)
        {
            fShelterTime = MIN1(fShelterTime + 0.3f * TIME);
        }
        
        const coreFloat fOffset = 1.0f * SIN(m_fStageTime * (0.2f*PI));
        const coreVector2 vShelterDir = coreVector2::Direction(m_fStageTime * (0.2f*PI));
        UNUSED const coreVector2 vShelterDir2 = coreVector2::Direction(m_fStageTime * -(0.2f*PI));                  

        for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i)
        {
            coreObject3D& oShelter = m_aShelter[i];

            const coreVector2 vFrom = i ? coreVector2(-0.4f,-1.5f) : coreVector2(1.5f,0.4f);
            const coreVector2 vTo   = i ? coreVector2(-0.4f,-0.4f * fOffset) : coreVector2(0.4f,0.4f * fOffset);//(i ? -vShelterDir2 : vShelterDir2) * 0.45f;//
            const coreVector2 vPos  = LERPB(vFrom, vTo * 1.0f, fShelterTime) * FOREGROUND_AREA;

            const coreVector2 vDir = i ? vShelterDir.InvertedX() : vShelterDir;

            oShelter.SetPosition (coreVector3(vPos, -2.0f));   // make sure bullets always hit, models are shifted
            oShelter.SetDirection(coreVector3(vDir,  0.0f));

            (i ? pHelperBlue : pHelperGreen)->SetPosition(coreVector3(oShelter.GetPosition().xy() - vDir * 13.0f, 0.0f));

            cPlayer::TestCollision(PLAYER_TEST_NORMAL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE, &oShelter, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pShelter, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                if(!bFirstHit) return;

                // 
                const coreVector2 vDiff = pPlayer->GetOldPos() - pShelter->GetPosition().xy();
                pPlayer->ApplyForceRaw(vDiff.Normalized() * 20.0f);

                // 
                g_pSpecialEffects->CreateSplashColor(vIntersection, 5.0f, 3u, COLOR_ENERGY_WHITE);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            });

            Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &oShelter, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pShelter, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                if(!bFirstHit) return;

                // 
                //pBullet->Reflect(pShelter, vIntersection.xy(), 3.0f);
                pBullet->Deactivate(true);
            });
        }

        if(!InBetween(m_fBeamTime, 1.0f, 2.0f))
        {
            cPlayer::TestCollision(PLAYER_TEST_NORMAL, &m_Beam, [this](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBeam, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i)
                {
                    const coreObject3D& oBack1 = m_aShelterBack1[i];

                    coreVector3 vNewIntersection;
                    coreBool    bNewFirstHit;
                    if(pPlayer->TestCollisionPrecise(&oBack1, &vNewIntersection, &bNewFirstHit))
                    {
                        return;
                    }
                }

                pPlayer->TakeDamage(10, ELEMENT_MAGENTA, vIntersection.xy());

                g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_MAGENTA);
            });
        }

        if(m_iStageSub <= 2u)
        {
            if(STAGE_BEGINNING || STAGE_TICK_FREE(1.0f/3.0f, 0.0f))
            {
                m_vBeamPos = coreVector2(g_pGame->GetPlayer(0u)->GetPosition().x, FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = coreVector2(0.0f,-1.0f);
                m_fBeamWidth = 10.0f;

                nShootBeamFunc(2.0f, 1.0f, 1.0f);
            }
        }
        else if(m_iStageSub == 3u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                nShootBeamFunc(0.0f, 1.0f, 1.0f);
                iChanged = 1u;
            }

            if(iChanged)
            {
                fRotation += 0.2f * TIME;
                const coreFloat fTime = FRACT(fRotation + 0.25f);
                const coreFloat fSide = LERP(-1.0f, 1.0f, MIN(fTime, 1.0f - fTime) * 2.0f);

                m_vBeamPos = coreVector2(FOREGROUND_AREA.x * 1.7f, fSide * FOREGROUND_AREA.y);
                m_vBeamDir = coreVector2(-1.0f,0.0f);
                m_fBeamWidth = 10.0f;
            }
        }
        else if(m_iStageSub == 4u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                nShootBeamFunc(0.0f, 1.0f, 1.0f);
                iChanged = 1u;
            }

            if(iChanged)
            {
                fRotation += 0.5f * TIME;
                const coreFloat fTime = (fRotation + 0.5f*PI);
                const coreVector2 vDir  = coreVector2::Direction(fTime);
                const coreVector2 vDir2 = coreVector2::Direction(fTime + 0.0f*0.2f*PI * SIN(fTime));

                m_vBeamPos = vDir * (FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = -vDir2;
                m_fBeamWidth = 10.0f;
            }
        }
        else if(m_iStageSub == 5u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                fDelay = m_fStageTime * 1.0f/4.0f - 1.0f;
                iChanged = 1u;
            }

            if(iChanged && STAGE_TICK_FREE(1.0f/4.0f, fDelay))
            {
                coreVector2 vDir;
                switch(s_iTick % 4u)
                {
                default: ASSERT(false)
                case 0u: vDir = coreVector2( 0.0f, 1.0f); break;
                case 1u: vDir = coreVector2( 1.0f, 0.0f); break;
                case 2u: vDir = coreVector2(-1.0f, 0.0f); break;
                case 3u: vDir = coreVector2( 0.0f,-1.0f); break;
                }

                m_vBeamPos = vDir * (FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = -vDir;
                m_fBeamWidth = 60.0f;

                nShootBeamFunc(3.0f, 2.0f, 1.0f);
            }
        }
        else if(m_iStageSub == 6u)
        {
            if(!iChanged && (m_fBeamTime <= -2.0f))
            {
                nShootBeamFunc(0.0f, 2.0f, 1.0f);
                iChanged = 1u;
            }

            if(iChanged)
            {
                fRotation += 0.5f * TIME;
                const coreVector2 vDir = coreVector2::Direction(fRotation);

                m_vBeamPos = vDir * (FOREGROUND_AREA.y * 1.7f);
                m_vBeamDir = -vDir;
                m_fBeamWidth = 60.0f;
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 1.0f + 0.2f * I_TO_F(i % 5u))
            
            //const coreVector2 vRawBeamPos = m_vBeamPos / FOREGROUND_AREA;
            //const coreFloat fLen = 1.7f;
            //STAGE_REPEAT(fLen)
            //
            //pEnemy->DefaultMoveLerp(vRawBeamPos, vRawBeamPos + m_vBeamDir * fLen, fLifeTime * RCP(fLen));
            //pEnemy->SetPosition(coreVector3((vRawBeamPos + m_vBeamDir * 0.8f) * FOREGROUND_AREA, 0.0f));
            
            if(i < 20u)
            {
                //const coreVector2 vRawBeamPos = m_vBeamPos / FOREGROUND_AREA;
                //const coreVector2 vPos = coreVector2(vRawBeamPos.x, (I_TO_F(i % 5u) - 2.0f) * 0.4f);
                
                //pEnemy->SetPosition(coreVector3(InBetween(m_fBeamTime, 1.0f, 2.0f) ? HIDDEN_POS : (vPos * FOREGROUND_AREA), 0.0f));
                STAGE_REPEAT(pPath2->GetTotalDistance())
                
                const coreVector2 vFactor = coreVector2(-1.0f,1.0f);
                const coreVector2 vOffset = coreVector2( 0.0f,0.0f);
                
                pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
                
                if((i % 10u) < 5u) pEnemy->InvertX();//pEnemy->Rotate270();
                else  pEnemy->Rotate90();
            }
            else if(i < 30u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F((i - 20u) % 10u) - 4.5f) * 0.2f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);

                pEnemy->Rotate90();
            }
            else if(i < 50u)
            {
                const coreVector2 vPos = coreVector2::Direction(I_TO_F(i % 10u) * (0.2f*PI));
                
                pEnemy->SetPosition(coreVector3(InBetween(m_fBeamTime, 1.0f, 2.0f) ? HIDDEN_POS : (vPos * FOREGROUND_AREA), 0.0f));
            }
            else
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 5u) - 2.0f) * 0.3f, 0.0f);

                pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);

                     if(i < 55u) {}
                else if(i < 60u) pEnemy->Rotate90 ();
                else if(i < 65u) pEnemy->Rotate180();
                else if(i < 70u) pEnemy->Rotate270();
                
                const coreObject3D& oShelter = m_aShelter[(i < 58u) ? 1u : 0u];
                
                const coreVector2 vDir = coreVector2::Direction(2.0f*PI * (I_TO_F(((i - 50u) % 8u) + (((i - 50u) % 8u) / 2u)) - 0.5f) / 12.0f);
                const coreVector2 vPos = oShelter.GetPosition().xy() + vDir * 13.0f;
                
                pEnemy->SetPosition(coreVector3(vPos, 0.0f));
            }
            
            
            
            //const coreVector2 vFactor = coreVector2(0.5f,1.0f);
            //const coreVector2 vOffset = coreVector2(1.5f,0.0f);
//
            //pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
            //
            //pEnemy->Rotate270();
            
            
            //const coreVector2 vFactor = coreVector2(0.5f,1.0f);
            //const coreVector2 vOffset = coreVector2(-1.5f,0.0f);
//
            //pEnemy->DefaultMovePath(pPath2, vFactor, vOffset * vFactor, fLifeTime);
//
            //if((i % 10u) < 5u) pEnemy->Rotate270();
            //else  pEnemy->Rotate90();


            //const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            //const coreVector2 vOffset = coreVector2((I_TO_F(i % 5u) - 2.0f) * 0.3f, 0.0f);
//
            //pEnemy->DefaultMovePath(pPath4, vFactor, vOffset * vFactor, fLifeTime);
//
            //if((i % 10u) < 5u) pEnemy->Rotate180();
            
        });

        STAGE_WAVE(0u, "X1-1", {20.0f, 30.0f, 40.0f, 50.0f, 60.0f})                             
    },
    STAGE_PRE()
    {
        // TODO 1              
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cScoutEnemy>();
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cWarriorEnemy>();
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cStarEnemy>();
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cArrowEnemy>();
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cMinerEnemy>();
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cFreezerEnemy>();
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cCinderEnemy>();
        //g_pGame->GetEnemyManager()->PrefetchEnemy<cMeteorEnemy>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cOrbBullet>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cConeBullet>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cWaveBullet>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cSpearBullet>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cTriangleBullet>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cFlipBullet>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cQuadBullet>();
        //g_pGame->GetBulletManagerEnemy()->PrefetchBullet<cViewBullet>();
    });

    // ################################################################
    // reset helper
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        g_pGame->KillHelpers();

        for(coreUintW i = 0u; i < BONUS1_SHELTERS; ++i)
            this->DisableShelter(i, false);

        this->DisableBeam(false);

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
        STAGE_BOSS(m_Vaus, {100.0f, 150.0f, 200.0, 250.0f, 500.0f})                        
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
    // ################################################################
}