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
        g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeightNow(0.4583f, -13.83f);
        g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
        g_pEnvironment->GetBackground()->SetGroundDensity(1u, 0.0f);
        g_pEnvironment->GetBackground()->SetGroundDensity(2u, 0.0f);
        g_pEnvironment->GetBackground()->SetDecalDensity (0u, 0.0f);
        g_pEnvironment->GetBackground()->SetAirDensity   (0u, 0.0f);
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
    // - 3: starts with offset, to not fly into players
    // - 4: reflect all bullets and players with force, except with barrel roll
    // - 5: arranged to allow only one kill per turn, and to improve coop gameplay
    // TODO 1: barriers have cut-off outline in maze-group   
    // TODO 1: bullets (even single, like pulse) should be reflected with offset to highlight reflection (not 180 degree)
    // TODO 1: huge shield, diagonal shield, moving enemies
    STAGE_MAIN({TAKE_ALWAYS, 0u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.5f,1.4f), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(-0.5f,0.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.0f,1.4f + 0.38f * 1.0f), coreVector2(0.0f,-1.0f));
            pPath2->AddStop(coreVector2(0.0f,0.0f + 0.38f * 0.5f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 18u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(10, COLOR_SHIP_CYAN);
            });
        });

        if((m_iStageSub == 4u) && (pSquad1->GetNumEnemiesAlive() == 6u))
        {
            Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_VIRIDO_BARRIER, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pBarrier, const coreVector3 vIntersection, const coreBool bFirstHit)
            {
                pBullet->SetFlyDir(pBarrier->GetDirection().xy());
            });
        }

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
                STAGE_RESURRECT(pSquad1, 2u, 3u)
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 2u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy( 2u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy( 2u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy( 3u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(14u, pSquad1->GetEnemy( 3u), coreVector2( 1.0f, 0.0f), 1.0f);
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 4u, 7u)
                this->EnableBarrier( 0u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 1u, pSquad1->GetEnemy( 4u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy( 5u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy( 5u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 4u, pSquad1->GetEnemy( 6u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 5u, pSquad1->GetEnemy( 6u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy( 7u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy( 7u), coreVector2(-1.0f, 0.0f), 1.0f);
            }
            else if(STAGE_SUB(4u))
            {
                STAGE_RESURRECT(pSquad1, 8u, 13u)
                this->EnableBarrier( 8u, pSquad1->GetEnemy( 8u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 9u, pSquad1->GetEnemy( 9u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(10u, pSquad1->GetEnemy(10u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(11u, pSquad1->GetEnemy(11u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(12u, pSquad1->GetEnemy(12u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier(13u, pSquad1->GetEnemy(13u), coreVector2( 0.0f,-1.0f), 1.0f);
            }
            else if(STAGE_SUB(5u))
            {
                STAGE_RESURRECT(pSquad1, 14u, 17u)
                this->EnableBarrier( 4u, pSquad1->GetEnemy(14u), coreVector2(-1.0f, 0.0f), 1.0f);   // # render order
                this->EnableBarrier( 1u, pSquad1->GetEnemy(15u), coreVector2( 0.0f,-1.0f), 1.0f);
                this->EnableBarrier( 2u, pSquad1->GetEnemy(15u), coreVector2( 1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 3u, pSquad1->GetEnemy(16u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 0u, pSquad1->GetEnemy(16u), coreVector2( 0.0f,-1.0f), 1.0f);   // # render order
                this->EnableBarrier( 5u, pSquad1->GetEnemy(16u), coreVector2(-1.0f, 0.0f), 1.0f);
                this->EnableBarrier( 6u, pSquad1->GetEnemy(17u), coreVector2( 0.0f, 1.0f), 1.0f);
                this->EnableBarrier( 7u, pSquad1->GetEnemy(17u), coreVector2( 1.0f, 0.0f), 1.0f);
            }
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, (i >= 8u && i < 14u) ? (I_TO_F(i-8u) * 0.15f) : 0.0f)

            const coreSpline2* pPath = (i >= 14u) ? pPath2 : pPath1;

            const coreVector2 vFactor = coreVector2((i >= 8u) ? 0.0f : ((i <= 1u || i >= 4u) ? ((i % 2u) ? 0.5f : -0.5f) : -1.0f), (i <= 1u || i >= 8u) ? ((i >= 14u) ? -1.0f : 1.0f) : ((i % 2u) ? 1.0f : -1.0f));
            const coreVector2 vOffset = coreVector2((i >= 8u) ? ((i >= 14u) ? ((I_TO_F((i-14u) % 2u) - 0.5f) * 0.38f) : ((I_TO_F(i-8u) - 2.5f) * 0.38f)) : 0.0f, (i >= 14u) ? (I_TO_F((i-14u) / 2u) * 0.38f) : 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);   // TODO 1: vOffset * vFactor

            if(i == 2u || i == 3u || i == 4u || i == 5u) pEnemy->Rotate90();

            if(STAGE_LIFETIME_AFTER(0.7f) && STAGE_TICK_LIFETIME(1.0f, 0.0f))
            {
                const coreVector2 vPos  = pEnemy->GetPosition().xy();
                const coreFloat   fBase = ((i >= 8u && i < 14u) ? pEnemy->GetDirection().xy() : pEnemy->AimAtPlayerSide()).Angle();

                for(coreUintW j = 3u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 1.0f) * 4.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.2f*0.5f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
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
    // TODO 1: fix ASSERT for getting the laser  
    // TODO 1: purple helper visible at border of one laser
    // TODO 1: vielleicht sollte in welle 3 X und Y nicht gleichzeitig kommen, oder X X Y Y XY
    // TODO 1: lila laser hält geschosse auf
    STAGE_MAIN({TAKE_ALWAYS, 1u})
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

                     if((vPos.x < -FOREGROUND_AREA.x) && (vDir.x < 0.0f)) vDir.x =  ABS(vDir.x);
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
    // TODO 1: player gets bigger and smaller when hitting a certain enemy, should not affect collision or gameplay
    // TODO 1: shrinking enemies have different attack (single dense wave)
    // TODO 1: multiple enemies are attached to each other and will grow independently
    STAGE_MAIN({TAKE_ALWAYS, 3u})
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 9u)
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
                pEnemy->SetSize    (coreVector3(1.0f,1.0f,1.0f) * (5.0f - 3.8f * pEnemy->GetCurHealthPct()));
            }
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
    // TODO 1: check for and fix shadow artifacts, when jumping behind near clipping plane of shadow viewport (maybe fade out near plane)
    // TODO 1: huge enemy causing a stomp (shake, throwing everything up) on landing, target player, player also fly into the air and rotate around Z (disable rotation control) (but quick, to make bullet evasion meaningful
    // TODO 1: chess jump movement
    // TODO 1: create impact-wave when landing (especially the bigger)
    // TODO 1: use side-jumping (from same-kill group) ?
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(10, COLOR_SHIP_ORANGE);
            });
        });

        STAGE_GET_START(9u * 20u)
            STAGE_GET_VEC2_ARRAY (avOldPos,   20u)
            STAGE_GET_VEC2_ARRAY (avNewPos,   20u)
            STAGE_GET_FLOAT_ARRAY(avOldAngle, 20u)
            STAGE_GET_FLOAT_ARRAY(avNewAngle, 20u)
            STAGE_GET_FLOAT_ARRAY(afJump,     20u)
            STAGE_GET_FLOAT_ARRAY(afWait,     20u, for(coreUintW i = 0u; i < 20u; ++i) afWait[i] = 0.5f;)
            STAGE_GET_UINT_ARRAY (aiCount,    20u)
        STAGE_GET_END

        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
        {
            if(m_apShadowOwner[i] && m_apShadowOwner[i]->HasStatus(ENEMY_STATUS_DEAD))
                this->DisableShadow(i, true);
        }

        if(STAGE_CLEARED)
        {
            if(STAGE_SUB(1u))
            {
                STAGE_RESURRECT(pSquad1, 0u, 3u)
                pSquad1->GetEnemy( 0u)->SetPosition(coreVector3( 0.6f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 0u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy( 1u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 1u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy( 2u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 2u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy( 3u)->SetPosition(coreVector3(-0.6f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 3u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            }
            else if(STAGE_SUB(2u))
            {
                STAGE_RESURRECT(pSquad1, 4u, 11u)
                pSquad1->GetEnemy( 4u)->SetPosition(coreVector3( 1.3f, 0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 4u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 5u)->SetPosition(coreVector3( 1.3f, 0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 5u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 6u)->SetPosition(coreVector3( 1.3f,-0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 6u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 7u)->SetPosition(coreVector3( 1.3f,-0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 7u)->SetDirection(coreVector3(-1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 8u)->SetPosition(coreVector3(-1.3f,-0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 8u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy( 9u)->SetPosition(coreVector3(-1.3f,-0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy( 9u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy(10u)->SetPosition(coreVector3(-1.3f, 0.2f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(10u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
                pSquad1->GetEnemy(11u)->SetPosition(coreVector3(-1.3f, 0.6f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(11u)->SetDirection(coreVector3( 1.0f, 0.0f,0.0f));
            }
            else if(STAGE_SUB(3u))
            {
                STAGE_RESURRECT(pSquad1, 12u, 19u)
                pSquad1->GetEnemy(12u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(12u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(13u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(13u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(14u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(14u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(15u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(15u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(16u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(16u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(17u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(17u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(18u)->SetPosition(coreVector3( 0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(18u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
                pSquad1->GetEnemy(19u)->SetPosition(coreVector3(-0.2f, 1.3f,0.0f) * FOREGROUND_AREA3); pSquad1->GetEnemy(19u)->SetDirection(coreVector3( 0.0f,-1.0f,0.0f));
            }
        }

        constexpr coreFloat fWaitSpeed = 0.9f;
        constexpr coreFloat fJumpSpeed = 0.7f;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, (i < 12u) ? (0.2f * I_TO_F(i % 4u)) : (((1.0f/fWaitSpeed) + (1.0f/fJumpSpeed)) * 0.125f * I_TO_F(i % 8u)))

            if(STAGE_TAKEOFF)
            {
                avOldPos  [i] = pEnemy->GetPosition ().xy();
                avOldAngle[i] = pEnemy->GetDirection().xy().Angle();
            }

            if(STAGE_LIFETIME_AFTER(0.0f))
            {
                if(afWait[i] < 1.0f)
                {
                    afWait[i] += fWaitSpeed * TIME;
                    if(afWait[i] >= 1.0f)
                    {
                        afJump[i] = 0.0f;
                        afWait[i] = 1.0f;

                        aiCount[i] += 1u;

                        avOldPos  [i] = pEnemy->GetPosition ().xy();
                        avOldAngle[i] = pEnemy->GetDirection().xy().Angle();

                        if(i < 4u)
                        {
                            avNewPos  [i] = coreVector2(0.75f - 0.5f * I_TO_F(i % 4u), 0.8f) * FOREGROUND_AREA * ((aiCount[i] % 2u) ? -1.0f : 1.0f);
                            avNewAngle[i] = avOldAngle[i] + 1.0f*PI;
                        }
                        else if(i < 12u)
                        {
                            avNewPos  [i] = coreVector2(0.8f, 0.75f - 0.5f * I_TO_F(i % 4u)) * FOREGROUND_AREA * ((aiCount[i] % 2u) ? -1.0f : 1.0f) * ((i >= 8u) ? -1.0f : 1.0f);
                            avNewAngle[i] = avOldAngle[i] + 1.0f*PI;
                        }
                        else
                        {
                            avNewPos  [i] = pEnemy->NearestPlayerDual(i % 2u)->GetPosition().xy();
                            avNewAngle[i] = avOldAngle[i] + 2.0f*PI;
                        }

                        this->EnableShadow(i % VIRIDO_SHADOWS, pEnemy, avNewPos[i]);
                    }
                }
                else
                {
                    afJump[i] += fJumpSpeed * TIME;
                    if(afJump[i] >= 1.0f)
                    {
                        afJump[i] = 1.0f;
                        afWait[i] = 0.0f;

                        const coreVector2 vPos = pEnemy->GetPosition().xy();

                        for(coreUintW j = 4u; j--; )
                        {
                            const coreVector2 vDir = StepRotated90(j);
                            const coreVector2 vTan = vDir.Rotated90() * 3.0f;

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos + vTan, vDir)->ChangeSize(1.4f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos,        vDir)->ChangeSize(1.4f);
                            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pEnemy, vPos - vTan, vDir)->ChangeSize(1.4f);
                        }

                        this->DisableShadow(i % VIRIDO_SHADOWS, true);
                    }
                }

                const coreFloat   fSin    = SIN(PI * afJump[i]);
                const coreVector2 vPos    = LERP(avOldPos[i], avNewPos[i], afJump[i]) * (1.0f - 0.45f * fSin);
                const coreFloat   fHeight = 50.0f * fSin;

                pEnemy->SetPosition         (coreVector3(vPos, fHeight));
                pEnemy->DefaultRotateLerp   (avOldAngle[i], avNewAngle[i], afJump[i]);
                pEnemy->DefaultOrientateLerp(0.0f*PI,       2.0f*PI,       afJump[i]);
            }
        });

        STAGE_WAVE("FÜNF", {20.0f, 30.0f, 40.0f, 50.0f})
    });

    // ################################################################
    // reset all shadows
    STAGE_MAIN({TAKE_ALWAYS, 4u})
    {
        for(coreUintW i = 0u; i < VIRIDO_SHADOWS; ++i)
            this->DisableShadow(i, false);

        STAGE_FINISH_NOW
    });

    // ################################################################
    // enemy charges straight forward on death (r-type fish)
    // - 2: should start on same spot and opposite of first group, to reduce accidental collision (which is more likely with 2x2)  
    // no passive attack, not impact attack, no bounce, because it's too overwhelming together with the green ball  
    // fire speed fast enough to create visual line  
    // 2x2 instead of 1x4 (enemy block instead of line), to force player keeping fire at the same position while an enemy is already flying at them  
    // TODO 1: give central effect to highlight damaging touch   
    // TODO 1: single enemy ping-ponging around (and keeps attacking you)
    // TODO 1: gegner schiffe bleiben im rand stecken, können für extra punkte abgeschossen werden
    // TODO 1: enemies just charge infinitely
    STAGE_MAIN({TAKE_ALWAYS, 5u})
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f,1.2f * SQRT2), coreVector2(0.0f,-1.0f));
            pPath1->AddStop(coreVector2(0.0f,0.7f),         coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cArrowEnemy, 32u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.45f,1.45f,1.45f));
                pEnemy->Configure(10, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);
            });
        });

        STAGE_GET_START(33u)
            STAGE_GET_FLOAT_ARRAY(afSpeed, 32u)
            STAGE_GET_FLOAT      (fAngle)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            if((pEnemy->GetCurHealth() == 0) && !g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()))
                pEnemy->Kill(true);
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESURRECT(pSquad1,  0u,  7u)
            else if(STAGE_SUB(2u)) STAGE_RESURRECT(pSquad1,  8u, 15u)
            else if(STAGE_SUB(3u)) STAGE_RESURRECT(pSquad1, 16u, 31u)
        }

             if(m_iStageSub == 2u) fAngle += 0.25f*PI * TIME;
        else if(m_iStageSub == 3u) fAngle -= 0.25f*PI * TIME;
        const coreMatrix2 mRota = coreMatrix3::Rotation(fAngle).m12();

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.85f, 0.3f * I_TO_F(i % 2u))

            if(pEnemy->ReachedDeath())
            {
                pEnemy->AddStatus(ENEMY_STATUS_DAMAGING);
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            }

            if(pEnemy->GetCurHealth() == 0)
            {
                afSpeed[i] += 80.0f * TIME;

                pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * ((20.0f + afSpeed[i]) * TIME));

                if(STAGE_TICK_TIME(30.0f, 0.0f))
                {
                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (i < 16u) ? 0.5f : 0.0f, pEnemy, vPos, vDir)->ChangeSize(1.6f);
                }
            }
            else if(i < 8u)
            {
                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 4u) - 1.5f + (((i % 4u) < 2u) ? -1.0f : 1.0f)) * 0.22f, ((i % 8u) < 4u) ? 0.0f : 0.22f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection   (coreVector3(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), 0.0f));
            }
            else
            {
                const coreVector2 vFactor = coreVector2(1.0f, ((i % 8u) < 4u) ? 1.0f : -1.0f);
                const coreVector2 vOffset = coreVector2((I_TO_F(i % 2u) - 0.5f) * 0.22f, ((i % 4u) < 2u) ? 0.0f : 0.22f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

                if(i >= 24u) pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy().Rotated90() * mRota, 0.0f));
                        else pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy()             * mRota, 0.0f));

                pEnemy->SetDirection(coreVector3(pEnemy->AimAtPlayerDual(i % 2u).Normalized(), 0.0f));
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