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
// setup the Demo mission
void cDemoMission::__SetupOwn()
{
    // intro 1-3 (cloud)
    // lasers (grass)
    // verfolgen (sand)
    // throw enemies (snow)
    // ghosts (dark)
    // turn on (lava)
    // force dir (water)
    // slow bubble (space)

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
        g_pEnvironment->ChangeBackground(cCloudBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        g_pEnvironment->SetTargetSpeedNow(4.0f);

        g_pGame->StartIntro();

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_MISSION})
    {
        STAGE_FINISH_AFTER(MISSION_WAIT_PLAY)
    });

    // ################################################################
    // change background appearance
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_FINISH_PLAY
    });

    // ################################################################
    // simple without attacks, show scoring
    // - 5: from different side than previous group, to not fly into players
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.4f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.4f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.2f,0.8f), coreVector2(1.0f,0.0f));
            pPath2->AddNode(coreVector2( 1.2f,0.0f), coreVector2(1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iBulletShot)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 12u, 17u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 18u, 23u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 24u, 29u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 30u, 35u)
        }

        if(g_pGame->GetBulletManagerPlayer()->GetNumBullets())
        {
            iBulletShot = 1u;
        }
        if(STAGE_TIME_POINT(10.0f) && !iBulletShot)
        {
            //g_pGame->ActivatePacifist();
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.22f * I_TO_F(i % 6u))

            const coreSpline2* pPath = ((i % 24u) < 12u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? -1.0f : 1.0f, (i < 24u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2((i >= 24u) ? -0.2f : 0.0f, 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset * vFactor, fLifeTime);
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // simple with attacks
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f, 0.8f), coreVector2(1.2f,-0.8f).Normalized());
            pPath1->AddNode(coreVector2( 1.2f,-0.8f), coreVector2(1.2f,-0.8f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 48u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED, true);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  3u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  4u,  7u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 16u, 23u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 24u, 31u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 32u, 47u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.21f * I_TO_F((i < 16u) ? (i % 4u) : (i % 8u)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i < 16u) ? ((i % 8u) < 4u) : ((i % 16u) < 8u)) ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.1f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(STAGE_TICK_LIFETIME(5.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayerDual((i < 16u) ? ((i / 4u) % 2u) : ((i / 8u) % 2u)).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.6f, pEnemy, vPos, vDir)->MakeWhite()->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // simple with heavy attacks
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(1.4f,0.5f), coreVector2(-1.0f,0.0f));
            pPath1->AddStop(coreVector2(0.0f,0.5f), coreVector2(-1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-0.4f,1.4f), coreVector2(0.0f,-1.0f), 0.9f/1.4f);
            pPath2->AddStop(coreVector2(-0.4f,0.5f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(120, COLOR_SHIP_RED, true);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.8f, (i == 1u) ? 1.0f : 0.0f)

            const coreSpline2* pPath = i ? pPath2 : pPath1;

            if(STAGE_LIFETIME_BEFORE(pPath->GetTotalDistance()))
            {
                const coreVector2 vFactor = coreVector2((i % 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f,0.0f);

                pEnemy->DefaultMovePath     (pPath, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection        (coreVector3(0.0f,-1.0f,0.0f));
                pEnemy->DefaultOrientateLerp(0.0f, 4.0f*PI * vFactor.x, LERPB(0.0f, 1.0f, fLifeTime * RCP(pPath->GetTotalDistance())));
            }
            else if(STAGE_TICK_LIFETIME(10.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(s_iTick) * DEG_TO_RAD(27.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos,  vDir)->MakeWhite()->ChangeSize(1.4f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, vPos, -vDir)->MakeWhite()->ChangeSize(1.4f);
            }
        });

        STAGE_WAVE("", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cGrassBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // cut with laser through everything
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 15u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.25f,1.25f,1.25f));
                pEnemy->Configure(30, COLOR_SHIP_PURPLE);
            });
        });

        STAGE_GET_START(15u)
            STAGE_GET_FLOAT_ARRAY(afDistance, 15u)
        STAGE_GET_END

        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
        {
            if(m_apLaserOwner[i] && m_apLaserOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableLaser(i, true);
        }

        if(STAGE_CLEARED)
        {
            if(STAGE_SUB(1u))
            {
                STAGE_RESURRECT(pSquad1, 0u, 2u)
                this->EnableLaser(0u, pSquad1->GetEnemy( 0u));
                this->EnableLaser(1u, pSquad1->GetEnemy( 1u));
                this->EnableLaser(2u, pSquad1->GetEnemy( 2u));
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 3u, 6u)
                this->EnableLaser(3u, pSquad1->GetEnemy( 3u));
                this->EnableLaser(0u, pSquad1->GetEnemy( 4u));
                this->EnableLaser(1u, pSquad1->GetEnemy( 5u));
                this->EnableLaser(2u, pSquad1->GetEnemy( 6u));
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 7u, 10u)
                this->EnableLaser(3u, pSquad1->GetEnemy( 7u));
                this->EnableLaser(0u, pSquad1->GetEnemy( 8u));
                this->EnableLaser(1u, pSquad1->GetEnemy( 9u));
                this->EnableLaser(2u, pSquad1->GetEnemy(10u));
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 11u, 14u)
                this->EnableLaser(3u, pSquad1->GetEnemy(11u));
                this->EnableLaser(0u, pSquad1->GetEnemy(12u));
                this->EnableLaser(1u, pSquad1->GetEnemy(13u));
                this->EnableLaser(2u, pSquad1->GetEnemy(14u));
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vBasePos, vBaseDir;
            coreFloat   fDelay, fShift, fSide;
            switch(i)
            {
            default: ASSERT(false)
            case  0u: vBasePos = coreVector2( 0.7f,-0.1f); vBaseDir = coreVector2(-2.0f,-1.0f).Normalized(); fDelay = 0.0f;  fShift =   0.0f; fSide = -1.0f; break;
            case  1u: vBasePos = coreVector2( 0.2f,-0.7f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.3f;  fShift = -15.0f; fSide =  1.0f; break;
            case  2u: vBasePos = coreVector2( 0.0f, 0.8f); vBaseDir = coreVector2( 1.0f, 1.0f).Normalized(); fDelay = 0.45f; fShift = -35.0f; fSide =  1.0f; break;

            case  3u: vBasePos = coreVector2( 0.1f, 0.8f); vBaseDir = coreVector2( 1.0f, 2.0f).Normalized(); fDelay = 0.0f;  fShift = -30.0f; fSide =  1.0f; break;
            case  4u: vBasePos = coreVector2( 0.2f, 0.2f); vBaseDir = coreVector2( 1.0f,-1.0f).Normalized(); fDelay = 0.3f;  fShift = -35.0f; fSide =  1.0f; break;
            case  5u: vBasePos = coreVector2( 0.5f,-0.6f); vBaseDir = coreVector2( 1.0f, 5.0f).Normalized(); fDelay = 0.4f;  fShift =   0.0f; fSide = -1.0f; break;
            case  6u: vBasePos = coreVector2(-0.8f, 0.1f); vBaseDir = coreVector2( 1.0f,-2.0f).Normalized(); fDelay = 0.7f;  fShift = -25.0f; fSide = -1.0f; break;

            case  7u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;  fShift = -10.0f; fSide = -1.0f; break;
            case  8u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.25f; fShift = -10.0f; fSide =  1.0f; break;
            case  9u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;  fShift = -10.0f; fSide = -1.0f; break;
            case 10u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f,-1.0f);              fDelay = 0.75f; fShift = -10.0f; fSide =  1.0f; break;

            case 11u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.0f;  fShift =   0.0f; fSide = -1.0f; break;
            case 12u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.25f; fShift =   0.0f; fSide =  1.0f; break;
            case 13u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2(-1.0f, 0.0f);              fDelay = 0.5f;  fShift =   0.0f; fSide = -1.0f; break;
            case 14u: vBasePos = coreVector2( 0.0f, 0.0f); vBaseDir = coreVector2( 0.0f, 1.0f);              fDelay = 0.75f; fShift =   0.0f; fSide =  1.0f; break;
            }

            STAGE_LIFETIME(pEnemy, 0.25f, fDelay)

            const coreVector2 vTan  = vBaseDir.Rotated90() * fSide;
            const coreFloat   fTime = MAX(fLifeTime, 0.0f);

            coreVector2 vLerpPos;
                 if(i < 7u)  vLerpPos = LERPB(vBasePos - vTan * 2.0f, vBasePos,           MIN(fTime * 1.2f, 1.0f)) * (FOREGROUND_AREA);
            else if(i < 11u) vLerpPos = LERP (-vTan,                  vTan,             FRACT(fTime * 0.6f))       * (FOREGROUND_AREA * 1.2f);
            else             vLerpPos = LERP (-vTan,                  vTan, 0.5f - 0.5f * COS(fTime * 1.4f))       * (FOREGROUND_AREA * 1.2f);

            coreVector2 vLerpDir;
                 if(i < 7u)  vLerpDir = vBaseDir;
            else if(i < 11u) vLerpDir = vBaseDir;
            else             vLerpDir = vBaseDir * coreMatrix3::Rotation(fTime * 1.1f).m12();

            if(STAGE_TAKEOFF) afDistance[i] = fShift;
            afDistance[i] += 100.0f * (fLifeTime - fLifeTimeBefore);

            const coreVector2 vNewPos = vLerpPos + vLerpDir * afDistance[i];

            if(((vNewPos.x < -FOREGROUND_AREA.x * 1.2f) && (vLerpDir.x < 0.0f)) ||
               ((vNewPos.x >  FOREGROUND_AREA.x * 1.2f) && (vLerpDir.x > 0.0f)) ||
               ((vNewPos.y < -FOREGROUND_AREA.y * 1.2f) && (vLerpDir.y < 0.0f)) ||
               ((vNewPos.y >  FOREGROUND_AREA.y * 1.2f) && (vLerpDir.y > 0.0f)))
            {
                if(afDistance[i] > 0.0f) afDistance[i] -= MAX(g_pForeground->RayIntersection(vNewPos, -vLerpDir, 1.2f), 0.0f);
            }

            coreObject3D* pLaser = (*m_Laser.List())[i % VIRIDO_LASERS];
            ASSERT(pEnemy->ReachedDeath() || (pEnemy == m_apLaserOwner[i % VIRIDO_LASERS]))

            pEnemy->SetPosition (coreVector3(vNewPos,  0.0f));
            pEnemy->SetDirection(coreVector3(vLerpDir, 0.0f));
            pLaser->SetPosition (coreVector3(vLerpPos, 0.0f));
            pLaser->SetDirection(coreVector3(vLerpDir, 0.0f));

            if(STAGE_TICK_TIME(12.0f, 0.0f) && ((s_iTick % 12u) < 3u))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos,  vDir)->ChangeSize(1.1f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.9f, pEnemy, vPos, -vDir)->ChangeSize(1.1f);
            }
        });

        STAGE_WAVE("DREI", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all lasers
    STAGE_MAIN({TAKE_ALWAYS})
    {
        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
            this->DisableLaser(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cDesertBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // leave destroyable residue on its way (r-type leaver)
    STAGE_MAIN({TAKE_ALWAYS, 8u})
    {
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_YELLOW);
            });

            pSquad1->GetEnemy( 0u)->SetPosition(coreVector3(-0.8f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 0u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy( 1u)->SetPosition(coreVector3( 0.8f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 1u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy( 2u)->SetPosition(coreVector3(-1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 2u)->SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 3u)->SetPosition(coreVector3( 1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 3u)->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 4u)->SetPosition(coreVector3(-1.2f,-0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 4u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy( 5u)->SetPosition(coreVector3( 0.0f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 5u)->SetDirection(coreVector3( 1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy( 6u)->SetPosition(coreVector3( 1.2f, 0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 6u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy( 7u)->SetPosition(coreVector3( 0.0f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 7u)->SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());

            pSquad1->GetEnemy( 8u)->SetPosition(coreVector3( 0.8f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 8u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy( 9u)->SetPosition(coreVector3(-0.8f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 9u)->SetDirection(coreVector3( 0.0f, 1.0f,0.0f));
            pSquad1->GetEnemy(10u)->SetPosition(coreVector3( 1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(10u)->SetDirection(coreVector3(-1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(11u)->SetPosition(coreVector3(-1.2f, 0.0f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(11u)->SetDirection(coreVector3( 1.0f,-1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(12u)->SetPosition(coreVector3(-1.2f, 0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(12u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(13u)->SetPosition(coreVector3( 0.0f,-1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(13u)->SetDirection(coreVector3( 1.0f, 1.0f,0.0f).Normalized());
            pSquad1->GetEnemy(14u)->SetPosition(coreVector3( 1.2f,-0.8f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(14u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
            pSquad1->GetEnemy(15u)->SetPosition(coreVector3( 0.0f, 1.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(15u)->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalized());
        });

        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fWhirlSpeed)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  5u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  6u,  7u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1, 10u, 10u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1, 11u, 11u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 12u, 13u)
            else if(STAGE_SUB(12u)) STAGE_RESURRECT(pSquad1, 14u, 15u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((m_iStageSub == 7u) && (fWhirlSpeed < 1.0f)) return;

            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vDiff   = pEnemy->AimAtPlayerDual((((i / 4u) % 2u) ? ((i / 2u) % 2u) : (i % 2u)) ? 0u : 1u);
            const coreVector2 vCurPos = pEnemy->GetPosition ().xy();
            coreVector2       vCurDir = pEnemy->GetDirection().xy();

            if(coreVector2::Dot(vDiff, vCurDir) <= 0.0f) vCurDir = vCurDir.Rotated90() * SIGN(coreVector2::Dot(vDiff, vCurDir.Rotated90()));

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) vCurDir.x =  ABS(vCurDir.x);
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) vCurDir.x = -ABS(vCurDir.x);
                 if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) vCurDir.y =  ABS(vCurDir.y);
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) vCurDir.y = -ABS(vCurDir.y);

            pEnemy->DefaultMoveForward(vCurDir, 29.0f);

            if(STAGE_TICK_TIME(9.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();
                const coreVector2 vTan = vDir.Rotated90() * 2.0f;

                if(coreMath::IsNear(vDir.x, 0.0f) || coreMath::IsNear(vDir.y, 0.0f))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos + vTan, vDir);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (5, 0.0f, pEnemy, vPos - vTan, vDir);
                }
                else
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos + vTan, vDir);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.0f, pEnemy, vPos - vTan, vDir);
                }
            }
        });

        if(m_iStageSub >= 7u)
        {
            fWhirlSpeed = MIN(fWhirlSpeed + 0.3f * TIME, 1.0f);

            const coreMatrix2 mRota    = coreMatrix3::Rotation(LERPS(0.0f, 0.7f, fWhirlSpeed) * TIME).m12();
            const coreMatrix2 mRotaRev = mRota.Transposed();

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRotaRev, 0.0f));
            });

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
            {
                pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mRota, 0.0f));
            });
        }

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
        {
            pBulletEnemy->Deactivate(true, vIntersection.xy());
        });

        STAGE_WAVE("ACHT", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSnowBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // mothership spawns enemies
    STAGE_MAIN({TAKE_ALWAYS})
    {
        constexpr coreUintW iNumEnemies = 104u;
        constexpr coreUintW iNumTypes   = (iNumEnemies + 3u) / 4u;

        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.30f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.75f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 8u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 2.0f);
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, iNumEnemies)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(4 * 2, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING | ENEMY_STATUS_WORTHLESS);
            });
        });

        STAGE_GET_START(2u + iNumTypes + 2u * iNumEnemies)
            STAGE_GET_UINT      (iTakeStart)
            STAGE_GET_UINT      (iTakeAlive)
            STAGE_GET_UINT_ARRAY(aiType,   iNumTypes)
            STAGE_GET_VEC2_ARRAY(avVector, iNumEnemies)
        STAGE_GET_END

        STATIC_ASSERT(sizeof(coreUint32) * iNumTypes >= iNumEnemies)
        coreUint8* aiByteType = r_cast<coreUint8*>(aiType);

        const auto nTakeChildFunc = [&](const coreUint8 iType)
        {
            for(coreUintW i = iTakeStart, ie = pSquad2->GetNumEnemies(); i < ie; ++i)
            {
                cEnemy* pCurrent = pSquad2->GetEnemy(i);

                if(pCurrent->HasStatus(ENEMY_STATUS_DEAD))
                {
                    iTakeStart = i + 1u;
                    iTakeAlive = iTakeAlive + 1u;

                    aiByteType[i] = iType;
                    avVector  [i] = coreVector2(0.0f,0.0f);

                    pCurrent->SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                    pCurrent->Resurrect();

                    return pCurrent;
                }
            }

            WARN_IF(true) {}
            return pSquad2->GetEnemy(0u);
        };

        const auto nDropShieldFunc = [&](const coreUint8 iType)
        {
            ASSERT(iType >= 4u)
            STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
            {
                if(aiByteType[i] == iType) pEnemy->Kill(true);
            });
        };

        if(pSquad1->IsFinished())
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 1u, 1u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 2u, 2u)
            else if(STAGE_SUB(4u)) STAGE_RESURRECT(pSquad1, 3u, 3u)
            else if(STAGE_SUB(5u)) STAGE_RESURRECT(pSquad1, 4u, 4u)
            else if(STAGE_SUB(6u)) STAGE_RESURRECT(pSquad1, 5u, 5u)
            else if(STAGE_SUB(7u)) STAGE_RESURRECT(pSquad1, 6u, 7u)

            if(pSquad1->IsFinished()) pSquad2->ClearEnemies(true);
        }

        if(m_fStageTime <= 3.0f)
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
            {
                cEnemy*         pChild = nTakeChildFunc(0u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreFloat   fOff = ((s_iTick % 30u) < 15u) ? 1.0f : -1.0f;
                const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;
                const coreVector2 vAim = (g_pGame->FindPlayerDual(((s_iTick % 30u) < 15u) ? 0u : 1u)->GetPosition().xy() - vPos).Normalized();
                const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                avVector[iIndex] = vDir;
            }
        }
        else if(m_iStageSub == 4u || m_iStageSub == 5u)
        {
            if(STAGE_TICK_FREE(10.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
            {
                cEnemy*         pChild = nTakeChildFunc(0u);
                const coreUintW iIndex = pSquad2->GetIndex(pChild);

                const coreFloat   fOff = g_pGame->FindPlayerDual((m_iStageSub == 4u) ? 0u : 1u)->GetPosition().y;
                const coreVector2 vPos = coreVector2(1.2f * FOREGROUND_AREA.x, fOff);

                pChild->SetPosition(coreVector3(vPos, 0.0f));

                avVector[iIndex] = coreVector2(-1.0f,0.0f);
            }
        }
        else if(m_iStageSub == 7u)
        {
            if(STAGE_TICK_FREE(1.0f, 0.0f))   // TODO 1: sub-time, STAGE_TICK_FREE2 ?
            {
                for(coreUintW j = 0u; j < 16u; ++j)
                {
                    cEnemy*         pChild = nTakeChildFunc(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreFloat   fOff = (I_TO_F(j) - 7.5f) * 0.1375f;
                    const coreVector2 vPos = coreVector2(fOff, 1.2f) * FOREGROUND_AREA;

                    pChild->SetPosition(coreVector3(vPos, 0.0f));

                    avVector[iIndex] = coreVector2(0.0f,-1.0f);
                }
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, i ? 0.0f : 4.0f)

            const coreVector2 vFactor = coreVector2(1.0f, (i == 4u || i == 6u || i == 7u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2((i == 0u || i == 3u || i == 5u) ? 0.0f : ((i % 2u) ? 0.75f : -0.75f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            if(i == 3u) pEnemy->Rotate90();

            if(i < 3u)
            {
                if(STAGE_TICK_LIFETIME(10.0f, 0.0f) && InBetween(s_iTick % 30u, 5u, 20u))
                {
                    cEnemy*         pChild = nTakeChildFunc(0u);
                    const coreUintW iIndex = pSquad2->GetIndex(pChild);

                    const coreVector2 vAim = pEnemy->AimAtPlayerDual(((s_iTick % 60u) < 30u) ? 0u : 1u).Normalized();   // TODO 1: coop, abwechslung
                    const coreVector2 vDir = (vAim + vAim.Rotated90() * (0.05f * ((s_iTick % 2u) ? -1.0f : 1.0f))).Normalized();

                    pChild->SetPosition(pEnemy->GetPosition());

                    avVector[iIndex] = vDir;
                }
            }
            else if(i == 5u)
            {
                if(STAGE_TICK_LIFETIME(2.0f, 0.0f) && ((s_iTick % 3u) < 2u))
                {
                    for(coreUintW j = 0u; j < 20u; ++j)
                    {
                        cEnemy*         pChild = nTakeChildFunc(1u);
                        const coreUintW iIndex = pSquad2->GetIndex(pChild);

                        pChild->SetPosition(pEnemy->GetPosition());

                        avVector[iIndex].x = ((I_TO_F(j) / 20.0f) + CORE_MATH_PRECISION) * (2.0f*PI) * ((s_iTick % 3u) ? 1.0f : -1.0f);
                    }
                }
            }

            if(i >= 1u)
            {
                if(STAGE_TAKEOFF)
                {
                    for(coreUintW j = 0u; j < 10u; ++j)
                    {
                        cEnemy*         pChild = nTakeChildFunc(i << 2u);
                        const coreUintW iIndex = pSquad2->GetIndex(pChild);

                        pChild->SetPosition(pEnemy->GetPosition());

                        avVector[iIndex].x = ((I_TO_F(j) / 10.0f) + CORE_MATH_PRECISION) * (2.0f*PI) * -1.0f;
                    }
                }

                if(pEnemy->ReachedDeath())
                {
                    nDropShieldFunc(i << 2u);
                }
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUint8 iType = aiByteType[i];

            if(iType == 0u)   // bullet
            {
                const coreVector2 vNewPos = pEnemy->GetPosition().xy() + avVector[i] * (50.0f * TIME);
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(iType == 1u)   // wave
            {
                const coreVector2 vCenter = pEnemy->GetPosition().xy() - coreVector2::Direction(avVector[i].x) * avVector[i].y;

                avVector[i].x += 0.7f * TIME * SIGN(avVector[i].x);
                avVector[i].y += 40.0f * TIME;

                const coreVector2 vNewPos = vCenter + coreVector2::Direction(avVector[i].x) * avVector[i].y;
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }
            else if(iType >= 4u)   // shield
            {
                const coreVector2 vCenter = pSquad1->GetEnemy(iType >> 2u)->GetPosition().xy();

                avVector[i].x += 1.0f * TIME * SIGN(avVector[i].x);
                avVector[i].y  = LERPB(0.0f, 12.0f, MIN(fLifeTime, 1.0f));

                const coreVector2 vNewPos = vCenter + coreVector2::Direction(avVector[i].x) * avVector[i].y;
                pEnemy->SetPosition(coreVector3(vNewPos, 0.0f));
            }

            const coreVector2 vNewDir = coreVector2::Direction(fLifeTime * 5.0f);
            pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

            if(iType == 0u)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                if((vPos.x < -FOREGROUND_AREA.x * 1.3f) ||
                   (vPos.x >  FOREGROUND_AREA.x * 1.3f) ||
                   (vPos.y < -FOREGROUND_AREA.y * 1.3f) ||
                   (vPos.y >  FOREGROUND_AREA.y * 1.3f))
                {
                    pEnemy->Kill(false);
                }
            }
            else if(iType == 1u)
            {
                if(avVector[i].y >= 100.0f)
                {
                    pEnemy->Kill(false);
                }
            }

            if(pEnemy->HasStatus(ENEMY_STATUS_DEAD))
            {
                iTakeStart = MIN(iTakeStart, i);
                iTakeAlive = iTakeAlive - 1;
            }
        });

        STAGE_WAVE("EINUNDDREISSIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cMossBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));

            cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());

            pBackground->GetHeadlight()->ResetFlicker();
            pBackground->SetEnableLightning(false);
            pBackground->SetEnableHeadlight(true);
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // ghost enemies only move when nobody is looking
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.0f), coreVector2(1.0f,0.0f), 0.7f);
            pPath1->AddNode(coreVector2( 1.2f,0.0f), coreVector2(1.0f,0.0f), 0.7f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 11u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure((i == 10) ? 120 : 10, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            });

            pSquad1->GetEnemy(6u)->SetPosition (coreVector3(0.0f, 0.9f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(6u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        });

        STAGE_GET_START(14u)
            STAGE_GET_FLOAT_ARRAY(afFade, 11u)
            STAGE_GET_UINT64     (iLight)
            STAGE_GET_FLOAT      (fBlind)
        STAGE_GET_END

        const auto nSpawnPosFunc = []()
        {
            coreVector2 vPos = coreVector2(0.0f,0.0f);
            coreUintW   iNum = 0u;

            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                vPos -= pPlayer->GetPosition().xy();
                iNum += 1u;
            });

            return (vPos * RCP(I_TO_F(iNum))).Processed(SIGN);
        };

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB( 1u)) STAGE_RESURRECT(pSquad1,  0u,  0u)
            else if(STAGE_SUB( 2u)) STAGE_RESURRECT(pSquad1,  1u,  1u)
            else if(STAGE_SUB( 3u)) STAGE_RESURRECT(pSquad1,  2u,  2u)
            else if(STAGE_SUB( 4u)) STAGE_RESURRECT(pSquad1,  3u,  3u)
            else if(STAGE_SUB( 5u)) STAGE_RESURRECT(pSquad1,  4u,  4u)
            else if(STAGE_SUB( 6u)) STAGE_RESURRECT(pSquad1,  5u,  5u)
            else if(STAGE_SUB( 7u)) STAGE_RESURRECT(pSquad1,  6u,  6u)
            else if(STAGE_SUB( 8u)) STAGE_RESURRECT(pSquad1,  7u,  7u)
            else if(STAGE_SUB( 9u)) STAGE_RESURRECT(pSquad1,  8u,  8u)
            else if(STAGE_SUB(10u)) STAGE_RESURRECT(pSquad1,  9u,  9u)
            else if(STAGE_SUB(11u)) STAGE_RESURRECT(pSquad1, 10u, 10u)

            if(((m_iStageSub == 3u) || (m_iStageSub == 7u)) && !STAGE_CLEARED)
            {
                const coreBool bBlackout = (m_iStageSub == 7u);
                if(bBlackout) fBlind = 6.0f;

                d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight()->PlayFlicker(bBlackout ? 1u : 0u);
            }
        }

        if(fBlind)
        {
            fBlind = MAX(fBlind - 1.0f * TIME, 0.0f);
            if(!fBlind) d_cast<cMossBackground*>(g_pEnvironment->GetBackground())->GetHeadlight()->PlayFlicker(0u);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            if((i == 6u) && STAGE_LIFETIME_BEFORE(1.2f)) return;

            if(i < 2u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(0.0f, i ? 0.9f : -0.9f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->Rotate90();
            }
            else if(i < 6u)
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2((i % 2u) ? -1.0f : 1.0f, 1.0f);
                const coreVector2 vOffset = coreVector2(0.0f, I_TO_F(4u - i) * 0.3f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
            }
            else
            {
                if(STAGE_TAKEOFF)
                {
                    const coreVector2 vPos = nSpawnPosFunc() * FOREGROUND_AREA * 0.9f;
                    const coreVector2 vDir = -vPos.Normalized();

                    pEnemy->SetPosition (coreVector3(vPos, 0.0f));
                    pEnemy->SetDirection(coreVector3(vDir, 0.0f));
                }

                const cPlayer* pPlayer = pEnemy->NearestPlayerDual(i % 2u);

                pEnemy->DefaultMoveTarget(pPlayer->GetPosition().xy(), 30.0f, 2.0f);
            }

            if(!fBlind)
            {
                if((ABS(pEnemy->GetPosition().x) < FOREGROUND_AREA.x * 1.0f) &&
                   (ABS(pEnemy->GetPosition().y) < FOREGROUND_AREA.y * 1.0f))
                {
                    STAGE_FOREACH_PLAYER(pPlayer, j)
                    {
                        const coreVector2 vDiff = pEnemy->GetPosition().xy() - (pPlayer->GetPosition().xy() + pPlayer->GetDirection().xy() * pPlayer->GetVisualRadius());
                        const coreFloat   fDot  = coreVector2::Dot(pPlayer->GetDirection().xy(), vDiff.Normalized());

                        if(fDot > 0.96f) ADD_BIT(iLight, i)
                    });
                }
            }

            if(HAS_BIT(iLight, i)) afFade[i] = MIN(afFade[i] + 4.0f * TIME, 1.0f);

            pEnemy->SetSize (coreVector3(1.0f,1.0f,1.0f) * ((i == 10u) ? 1.9f : 1.1f) * LERPS(2.0f, 1.0f, afFade[i]));
            pEnemy->SetAlpha(LERPS(0.0f, 1.0f, afFade[i]));

            if(STAGE_TICK_LIFETIME((i == 10u) ? 3.0f : 2.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 4u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 45.0f + 22.5f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.6f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.6f);
                }
            }

            if(HAS_BIT(iLight, i))
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_GHOST))
                {
                    pEnemy->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
                }
            }
        });

        STAGE_WAVE("ZWEIUNDNEUNZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cVolcanoBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // switch everything on
    STAGE_MAIN({TAKE_ALWAYS})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.2f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->AddNode(coreVector2( 0.8f,-1.2f), coreVector2(1.6f,-2.4f).Normalized());
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 30u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_GREY);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            });
        });

        STAGE_GET_START(5u + GAME_PLAYERS)
            STAGE_GET_UINT      (iTileState)
            STAGE_GET_UINT      (iTileDone)
            STAGE_GET_UINT      (iTileWait)
            STAGE_GET_FLOAT     (fTileMove)
            STAGE_GET_UINT      (iGauntlet)
            STAGE_GET_UINT_ARRAY(aiRemember, GAME_PLAYERS)
        STAGE_GET_END

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  9u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1, 10u, 19u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 20u, 29u)

            for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                this->DisableTile(i, true);

            iTileWait = 0u;
        }

        if(!iTileWait)
        {
            if(!m_aTileRaw[0].IsEnabled(CORE_OBJECT_ENABLE_ALL))
            {
                //STATIC_ASSERT(NEVO_TILES == 16u)

                if(m_iStageSub == 1u)
                {
                    for(coreUintW i = 0u; i < 9u; ++i)
                        this->EnableTile(i, 3u);
                }
                else if(m_iStageSub == 2u)
                {
                    for(coreUintW i = 0u; i < 16u; ++i)
                        this->EnableTile(i, 4u);
                }
                else if(m_iStageSub == 3u)
                {
                    if(!iGauntlet) iGauntlet = 1u;
                }

                iTileState = 0u;
                iTileDone  = 0u;
                iTileWait  = 1u;

                std::memset(aiRemember, 0, sizeof(coreUint32) * GAME_PLAYERS);
            }
        }

        if(iGauntlet)
        {
            if(iGauntlet == 1u)
            {
                this->EnableTile(2u, 4u);

                iGauntlet += 1u;
            }
            else if(iGauntlet == 3u)
            {
                this->EnableTile(8u,  4u);
                this->EnableTile(14u, 4u);

                iGauntlet += 1u;
            }
            else if(iGauntlet == 5u)
            {
                this->EnableTile(1u, 4u);
                this->EnableTile(7u, 4u);

                fTileMove  = 0.0f;
                iGauntlet += 1u;
            }
            else if(iGauntlet == 6u)
            {
                fTileMove += 0.6f * TIME;

                m_aTileRaw[1].SetPosition(coreVector3(LERPS(coreVector2(-0.25f,0.75f), coreVector2(-0.25f,-0.75f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
                m_aTileRaw[7].SetPosition(coreVector3(LERPS(coreVector2( 0.75f,0.25f), coreVector2(-0.75f, 0.25f), fTileMove) * (FOREGROUND_AREA * 1.1f), 0.0f));
            }
            else if(iGauntlet == 7u)
            {
                this->EnableTile(12u, 4u);
                this->EnableTile(13u, 4u);

                m_aTileRaw[12].SetPosition(coreVector3(HIDDEN_POS, 0.0f));
                m_aTileRaw[13].SetPosition(coreVector3(HIDDEN_POS, 0.0f));

                fTileMove  = 0.0f;
                iGauntlet += 1u;
            }
            else if(iGauntlet == 8u)
            {
                fTileMove += 1.0f * TIME;

                const coreVector2 vPos = coreVector2::Direction(fTileMove);
                const coreVector2 vDir = coreVector2::Direction(fTileMove * -2.0f);

                m_aTileRaw[12].SetPosition(coreVector3(vPos * (FOREGROUND_AREA *  0.5f), 0.0f));
                m_aTileRaw[13].SetPosition(coreVector3(vPos * (FOREGROUND_AREA * -0.5f), 0.0f));

                m_aTileRaw[12].SetDirection(coreVector3(vDir, 0.0f));
                m_aTileRaw[13].SetDirection(coreVector3(vDir, 0.0f));
            }
            else if(iGauntlet == 9u)
            {
                this->EnableTile(9u,  4u);
                this->EnableTile(10u, 4u);
                this->EnableTile(11u, 4u);

                m_aTileRaw[9] .SetPosition(coreVector3(0.0f,0.0f,0.0f));
                m_aTileRaw[10].SetPosition(coreVector3(0.0f,0.0f,0.0f));
                m_aTileRaw[11].SetPosition(coreVector3(0.0f,0.0f,0.0f));

                m_aTileRaw[9] .SetSize(m_aTileRaw[9] .GetSize() * 0.7f * 3.0f);
                m_aTileRaw[10].SetSize(m_aTileRaw[10].GetSize() * 0.7f * 2.0f);
                m_aTileRaw[11].SetSize(m_aTileRaw[11].GetSize() * 0.7f);

                iTileState = BIT(10u);
                iGauntlet += 1u;
            }
        }

        coreBool bComplete = !iTileDone;

        if(!iTileDone)
        {
            for(coreUintW i = 0u; i < NEVO_TILES; ++i)
            {
                coreObject3D& oTile = m_aTileRaw[i];
                if(!oTile.IsEnabled(CORE_OBJECT_ENABLE_ALL) || (m_afTileTime[i] < 0.0f)) continue;

                STAGE_FOREACH_PLAYER(pPlayer, j)
                {
                    if(pPlayer->IsRolling()) return;

                    const coreVector2 vDiff = MapToAxisInv(pPlayer->GetPosition().xy() - oTile.GetPosition().xy(), oTile.GetDirection().xy());

                    if((ABS(vDiff.x) < oTile.GetCollisionRange().x) &&
                       (ABS(vDiff.y) < oTile.GetCollisionRange().y))
                    {
                        if(!HAS_BIT(aiRemember[j], i)) TOGGLE_BIT(iTileState, i)
                        ADD_BIT(aiRemember[j], i)
                    }
                    else
                    {
                        REMOVE_BIT(aiRemember[j], i)
                    }
                });

                if(HAS_BIT(iTileState, i))
                {
                    oTile.SetColor3(COLOR_ENERGY_YELLOW);
                }
                else
                {
                    oTile.SetColor3(COLOR_ENERGY_BLUE);
                    bComplete = false;
                }
            }
        }

        if(bComplete)
        {
            if(iGauntlet && (iGauntlet < 10u))
            {
                for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                    this->DisableTile(i, true);

                iGauntlet += 1u;
                bComplete  = false;
            }
            else
            {
                for(coreUintW i = 0u; i < NEVO_TILES; ++i)
                    m_aTileRaw[i].SetColor3(COLOR_ENERGY_GREEN);
            }
        }

        if(bComplete) iTileDone = 1u;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.8f, 0.2f * I_TO_F(i % 5u) + (((i % 10u) < 5u) ? 0.0f : (0.5f * pPath1->GetTotalDistance())))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 10u) < 5u) ? 1.0f : -1.0f, (i < 20u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i >= 10u && i < 15u) pEnemy->Rotate90();
            else if(i >= 15u && i < 20u) pEnemy->Rotate270();

            if(!(i % 5u) && STAGE_TICK_TIME(0.8f, 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition ().xy();
                const coreFloat   fBase = pEnemy->GetDirection().xy().Rotated90().Angle();

                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 8.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                }
            }

            if(bComplete)
            {
                if(pEnemy->HasStatus(ENEMY_STATUS_INVINCIBLE))
                {
                    pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                    pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                    g_pSpecialEffects->CreateSplashColor(pEnemy->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_BLUE);
                }
            }
        });

        STAGE_WAVE("SIEBZEHN", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all tiles
    STAGE_MAIN({TAKE_ALWAYS})
    {
        for(coreUintW i = 0u; i < NEVO_TILES; ++i)
            this->DisableTile(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // force rotation
    STAGE_MAIN({TAKE_ALWAYS})
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

        STAGE_FOREACH_PLAYER(pPlayer, i)   // before plate changes, positions may not be updated yet
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_TURN);

            if(!pPlayer->IsRolling())
            {
                const coreVector2 vPos = pPlayer->GetPosition().xy();

                for(coreUintW j = 0u; j < RUTILUS_PLATES; ++j)
                {
                    const coreVector2 vSize = m_aPlateRaw[j].GetCollisionRange().xy();
                    const coreVector2 vDiff = MapToAxis(m_aPlateRaw[j].GetPosition().xy() - vPos, m_aPlateRaw[j].GetDirection().xy());

                    if((ABS(vDiff.x) < vSize.x) && (ABS(vDiff.y) < vSize.y))
                    {
                        pPlayer->SetDirection(m_aPlateRaw[j].GetDirection());
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

        STAGE_WAVE("ACHTUNDVIERZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all plates
    STAGE_MAIN({TAKE_ALWAYS})
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
    // 
    STAGE_MAIN({TAKE_ALWAYS})
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(cSpaceBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        }

        STAGE_FINISH_AFTER(1.0f)
    });

    // ################################################################
    // slowdown areas
    STAGE_MAIN({TAKE_ALWAYS})
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
            else if(STAGE_SUB(16u)) STAGE_DELAY_START_CLEAR

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

        const auto nCalcSpeedFunc = [&](const coreVector2 vPosition)
        {
            const coreVector2 vDiff = vPosition - vTestPos;
            return LERP(fSpeedSlow, fSpeedFast, STEPH3(fTestFromSq, fTestToSq, vDiff.LengthSq()));
        };

        if(m_aArea[0].IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            STAGE_FOREACH_PLAYER(pPlayer, i)
            {
                pPlayer->SetMoveSpeed(nCalcSpeedFunc(pPlayer->GetPosition().xy()));
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

                pBullet->SetSpeed(fBase * nCalcSpeedFunc(pBullet->GetPosition().xy()));
            };
            g_pGame->GetBulletManagerPlayer()->ForEachBullet(nBulletSlowFunc);
            g_pGame->GetBulletManagerEnemy ()->ForEachBullet(nBulletSlowFunc);
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 4u) ? 2.0f : 0.0f)

            if(i < 10u)
            {
                if(STAGE_LIFETIME_AFTER(0.0f)) pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 25.0f * nCalcSpeedFunc(pEnemy->GetPosition().xy()));

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

        STAGE_WAVE("EINDUNDFÜNFZIG", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all areas
    STAGE_MAIN({TAKE_ALWAYS})
    {
        this->DisableArea(false);

        g_pGame->GetHelper(ELEMENT_PURPLE)->Kill(false);

        STAGE_FINISH_NOW
    });
}