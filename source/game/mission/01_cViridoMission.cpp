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
// constructor
cViridoMission::cViridoMission()noexcept
: m_Ball           (VIRIDO_BALLS)
, m_BallTrail      (VIRIDO_BALLS * VIRIDO_TRAILS)
, m_apPaddleOwner  {}
, m_Barrier        (VIRIDO_BARRIERS)
, m_apBarrierOwner {}
, m_aiBarrierDir   {}
, m_iRealState     (0u)
, m_iStickyState   (0u)
, m_iBounceState   (0u)
, m_fAnimation     (0.0f)
{
    // 
    m_apBoss[0] = &m_Dharuk;
    m_apBoss[1] = &m_Torus;
    m_apBoss[2] = &m_Vaus;

    // create ball lists
    m_Ball     .DefineProgram("effect_energy_invert_inst_program");
    m_BallTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_BALLS_RAWS; ++i)
        {
            // determine object type
            const coreUintW iType = i % (VIRIDO_TRAILS + 1u);

            // load object resources
            coreObject3D* pBall = &m_aBallRaw[i];
            pBall->DefineModel  ("object_sphere.md3");
            pBall->DefineTexture(0u, "effect_energy.png");
            pBall->DefineProgram("effect_energy_invert_program");

            // set object properties
            pBall->SetSize   (coreVector3(2.7f,2.7f,2.7f));
            pBall->SetColor3 (COLOR_ENERGY_GREEN * 0.8f);
            pBall->SetTexSize(coreVector2(4.5f,4.5f));
            pBall->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            if(iType) m_BallTrail.BindObject(pBall);
                 else m_Ball     .BindObject(pBall);
        }
    }

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
    {
        // 
        const coreBool bBoss = i ? false : true;

        // 
        m_aPaddle[i].DefineModel         ("object_paddle.md3");
        m_aPaddle[i].DefineTexture       (0u, "effect_energy.png");
        m_aPaddle[i].DefineProgram       ("effect_energy_flat_direct_program");
        m_aPaddle[i].SetSize             (bBoss ? coreVector3(3.5f,2.5f,2.5f) : coreVector3(2.5f,2.5f,2.5f));
        m_aPaddle[i].SetColor3           (bBoss ? COLOR_ENERGY_BLUE           : COLOR_ENERGY_RED);
        m_aPaddle[i].SetTexSize          (coreVector2(1.2f,0.25f) * 0.5f);
        m_aPaddle[i].SetEnabled          (CORE_OBJECT_ENABLE_NOTHING);
        m_aPaddle[i].SetCollisionModifier(coreVector3(0.6f,1.0f,1.0f));

        // 
        m_aPaddleSphere[i].DefineModel("object_sphere.md3");
        m_aPaddleSphere[i].SetSize    (bBoss ? coreVector3(30.0f,30.0f,30.0f) : coreVector3(15.0f,15.0f,15.0f));
    }

    // 
    m_Barrier.DefineProgram("effect_energy_flat_direct_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_BARRIERS_RAWS; ++i)
        {
            // load object resources
            coreObject3D* pBarrier = &m_aBarrierRaw[i];
            pBarrier->DefineModel  ("object_barrier.md3");
            pBarrier->DefineTexture(0u, "effect_energy.png");
            pBarrier->DefineProgram("effect_energy_flat_direct_program");

            // set object properties
            pBarrier->SetSize   (coreVector3(7.5f,2.5f,2.5f));
            pBarrier->SetColor3 (COLOR_ENERGY_BLUE);
            pBarrier->SetTexSize(coreVector2(1.2f,0.25f) * 0.5f);
            pBarrier->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Barrier.BindObject(pBarrier);
        }
    }

    // 
    g_pGlow->BindList(&m_Ball);
    g_pGlow->BindList(&m_BallTrail);
    g_pGlow->BindList(&m_Barrier);
}


// ****************************************************************
// destructor
cViridoMission::~cViridoMission()
{
    // 
    g_pGlow->UnbindList(&m_Ball);
    g_pGlow->UnbindList(&m_BallTrail);
    g_pGlow->UnbindList(&m_Barrier);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS; ++i)
        this->DisableBall(i, false);

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        this->DisablePaddle(i, false);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
        this->DisableBarrier(i, false);
}


// ****************************************************************
// 
void cViridoMission::EnableBall(const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection)
{
    // 
    ASSERT(iIndex < VIRIDO_BALLS)
    coreObject3D* pBall  = (*m_Ball     .List())[iIndex];
    coreObject3D* pTrail = (*m_BallTrail.List())[iIndex*VIRIDO_TRAILS];

    // 
    if(pBall->GetType()) return;
    pBall->ChangeType(TYPE_VIRIDO_BALL);

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject, const coreFloat fAlpha)
    {
        pObject->SetPosition (coreVector3(vPosition,  0.0f));
        pObject->SetDirection(coreVector3(vDirection, 0.0f));
        pObject->SetAlpha    (fAlpha);
        pObject->SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    };
    nInitFunc(pBall, 1.0f);
    for(coreUintW i = 0u; i < VIRIDO_TRAILS; ++i) nInitFunc(pTrail + i, 1.0f - I_TO_F(i+1u) / I_TO_F(VIRIDO_TRAILS));

    // 
    g_pSpecialEffects->MacroEruptionColorSmall(coreVector3(vPosition, 0.0f), vDirection, COLOR_ENERGY_GREEN);
}


// ****************************************************************
// 
void cViridoMission::DisableBall(const coreUintW iIndex, const coreBool bAnimated)
{
    // 
    ASSERT(iIndex < VIRIDO_BALLS)
    coreObject3D* pBall  = (*m_Ball     .List())[iIndex];
    coreObject3D* pTrail = (*m_BallTrail.List())[iIndex*VIRIDO_TRAILS];

    // 
    if(!pBall->GetType()) return;
    pBall->ChangeType(0);

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    };
    nExitFunc(pBall);
    for(coreUintW i = 0u; i < VIRIDO_TRAILS; ++i) nExitFunc(pTrail + i);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionColorSmall(pBall->GetPosition(), COLOR_ENERGY_GREEN);
}


// ****************************************************************
// 
void cViridoMission::EnablePaddle(const coreUintW iIndex, const cShip* pOwner)
{
    ASSERT(iIndex < VIRIDO_PADDLES)
    coreObject3D& oPaddle = m_aPaddle[iIndex];

    // 
    m_apPaddleOwner[iIndex] = pOwner;

    // 
    if(oPaddle.GetType()) return;
    oPaddle.ChangeType(TYPE_VIRIDO_PADDLE);

    // 
    oPaddle.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&oPaddle);
}


// ****************************************************************
// 
void cViridoMission::DisablePaddle(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_PADDLES)
    coreObject3D& oPaddle = m_aPaddle[iIndex];

    // 
    if(!oPaddle.GetType()) return;
    oPaddle.ChangeType(0);

    // 
    oPaddle.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&oPaddle);
}


// ****************************************************************
// 
void cViridoMission::EnableBarrier(const coreUintW iIndex, const cShip* pOwner, const coreVector2& vDirection)
{
    ASSERT(iIndex < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

    // 
    m_apBarrierOwner[iIndex] = pOwner;
    m_aiBarrierDir  [iIndex] = PackDirection(vDirection);

    // 
    if(oBarrier.GetType()) return;
    oBarrier.ChangeType(TYPE_VIRIDO_BARRIER);

    // 
    oBarrier.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cViridoMission::DisableBarrier(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < VIRIDO_BARRIERS)
    coreObject3D& oBarrier = m_aBarrierRaw[iIndex];

    // 
    if(!oBarrier.GetType()) return;
    oBarrier.ChangeType(0);

    // 
    oBarrier.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// setup the Virido mission
void cViridoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ActivateDarkShading();
        });

        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(-cGrassBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));

            g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeightNow(0.4583f, -13.83f);
            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(1u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(2u, 0.0f);
            g_pEnvironment->GetBackground()->SetDecalDensity (0u, 0.0f);
            g_pEnvironment->GetBackground()->SetAirDensity   (0u, 0.0f);

            g_pEnvironment->SetTargetSpeed(5.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // top, bottom, side groups creating bullet carpet
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.2f,0.9f), coreVector2(1.0f,0.0f));
            pPath1->AddNode(coreVector2( 1.2f,0.9f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(0.9f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath2->AddNode(coreVector2(0.9f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 36u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_BLUE);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1,  0u,  5u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1,  6u, 11u)
            else if(STAGE_SUB(3u)) STAGE_RESSURECT(pSquad1, 12u, 23u)
            else if(STAGE_SUB(4u)) STAGE_RESSURECT(pSquad1, 24u, 35u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.4f * I_TO_F(i % 6u))

            const coreSpline2* pPath = (i < 12u || i >= 24u) ? pPath1 : pPath2;

            STAGE_REPEAT(pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i % 12u) < 6u) ? 1.0f : -1.0f, (i < 6u || i >= 12u) ? ((i < 24u) ? 1.0f : 0.0f) : -1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, (i < 24u) ? 0.0f : ((i < 30u) ? -0.1f : 0.1f));

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);

            if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                if(i < 24u) g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // shields on sides to force attacks from certain directions 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 8u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(I_TO_F(i - 2u) * 0.2f, 1.1f, 0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (50, COLOR_SHIP_ORANGE);

                pEnemy->Resurrect();
            });
        });

        

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->DefaultMoveSmooth(coreVector2(I_TO_F(i - 2u) * 0.2f, 0.3f), 10.0f, 1.0f);  

            if(STAGE_BEGINNING)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = coreVector2(1.0f,0.0f);

                cBullet* pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 0.9f, pEnemy, vPos + vDir * 5.0f, vDir);  
                pBullet->SetSize(pBullet->GetSize() * coreVector3(2.0f,1.0f,1.0f));
            }
            else
            {

            }
        });



        STAGE_WAVE
    });

    // ################################################################
    // train through the middle, enemies survive invincible 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 24u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.0f,1.0f,1.0f) * 1.1f);
                pEnemy->Configure(4, COLOR_SHIP_YELLOW);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);

                pEnemy->Resurrect();
            });
        });

        coreBool bActive = false;

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.5f, 0.2f * I_TO_F(i % 12u) - LERPB(0.0f, 2.0f, MIN(m_fStageTime * 0.5f, 1.0f)))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f, (i < 12u) ? -1.0f : 1.0f);
            const coreVector2 vOffset = coreVector2(((i < 12u) ? -1.0f : 1.0f) * (0.1f + LERPB(1.0f, 0.0f, MIN(m_fStageTime * 0.5f, 1.0f))), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);

            //if(STAGE_TICK_TIME(2.0f, 0.0f))
            //{
            //    const coreVector2 vPos =  pEnemy->GetPosition ().xy();
            //    const coreVector2 vDir = -pEnemy->GetDirection().xy().Rotated135();
            //
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            //}
            if(STAGE_TICK_LIFETIME(4.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir)->ChangeSize(1.2f);
                //if(i < 24u) g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir)->ChangeSize(1.2f);
            }

            //if(pEnemy->ReachedDeath()) pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            if(pEnemy->GetCurHealth()) bActive = true;
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([](cFlipBullet* OUTPUT pBullet)
        {
            const coreFloat fAngle = LERP(0.0f*PI, 0.9f*PI, ABS(pBullet->GetPosition().x) / FOREGROUND_AREA.x) + ((pBullet->GetPosition().x > 0.0f) ? (1.0f*PI) : (0.0f*PI));
            pBullet->SetFlyDir(coreVector2::Direction(fAngle));
        });

        if(!bActive) pSquad1->ClearEnemies(true);

        STAGE_WAVE
    });

    // ################################################################
    // big enemy splits up into smaller enemies on death 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cWarriorEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(0.0f,1.1f,0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (100, COLOR_SHIP_YELLOW);

                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cArrowEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_RED);
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->DefaultMoveSmooth(coreVector2(0.0f, 0.3f), 10.0f, 1.0f);  

            if(pEnemy->ReachedDeath())
            {
                STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy2, j)
                {
                    pEnemy2->SetPosition (pEnemy->GetPosition());
                    pEnemy2->SetDirection(coreVector3(StepRotated90X(j), 0.0f));

                    pEnemy2->Resurrect();
                });
            }
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            pEnemy->DefaultMoveTarget(pEnemy->NearestPlayer()->GetPosition().xy(), LERP(40.0f, 20.0f, MIN(pEnemy->GetLifeTime(), 1.0f)), 2.0f);
        });

        STAGE_WAVE
    });

    // ################################################################
    // bullet attack explodes on border 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(200, COLOR_SHIP_RED);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 2u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            const coreVector2 vTarget = coreVector2(i ? (0.3f * (-3.0f + 2.0f * I_TO_F(i))) : 0.0f,0.0f);

            if(STAGE_TAKEOFF)
            {
                pEnemy->SetPosition(coreVector3((vTarget + coreVector2(0.0f,1.0f)) * FOREGROUND_AREA, 0.0f));
            }

            pEnemy->DefaultMoveSmooth(vTarget, 40.0f, 10.0f);  

            if(STAGE_TICK_LIFETIME(1.25f, (i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.8f, pEnemy, vPos, vDir)->ChangeSize(2.0f);
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            if(!g_pForeground->IsVisiblePoint(pBullet->GetPosition().xy()))
            {
                const coreVector2 vPos = pBullet->GetPosition().xy();

                for(coreUintW i = 20u; i--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos,  vDir)->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.7f, pBullet->GetOwner(), vPos, -vDir)->ChangeSize(1.4f);
                    // TODO: lower purple balls get overlapped   
                }

                pBullet->Deactivate(true);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Dharuk)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_GET_START(1u)
            STAGE_GET_FLOAT(fOldAngle, fOldAngle = g_pEnvironment->GetDirection().Angle())
        STAGE_GET_END

        g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(fOldAngle, 0.0f*PI, MIN(m_fStageTime * 0.5f, 1.0f))));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // enemy has weakpoint which he keeps away from player 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_ADD_SQUAD(pSquad2, cStarEnemy, 1u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.25f, 0.2f * I_TO_F(i))

            STAGE_REPEAT(pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.0f,0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);


            cEnemy* pBuddy = pSquad2->GetEnemy(i);


            const coreFloat fCurAngle    = STAGE_TAKEOFF ? 0.0f : (pBuddy->GetPosition().xy() - pEnemy->GetPosition().xy()).Normalized().Angle();
            const coreFloat fTargetAngle = (-pEnemy->AimAtPlayer()).Normalized().Angle();

            const coreFloat fDiff = (AngleDiff(fTargetAngle, fCurAngle));

            const coreFloat fAngle = fCurAngle + fDiff * Core::System->GetTime() * 2.0f;

            const coreVector2 vDir = coreVector2::Direction(fAngle);
            const coreVector2 vPos = pEnemy->GetPosition().xy() + vDir * 10.0f;

            pSquad2->GetEnemy(i)->SetPosition(coreVector3(vPos, 0.0f));
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 1-2
        g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(0.0f*PI, -0.5f*PI, MIN(m_fStageTime * 0.5f, 1.0f))));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // explode into bullets on death
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 12u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_RED);

                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.1f, 0.15f * I_TO_F(i))

            STAGE_REPEAT(0.5f)

            pEnemy->SetPosition(coreVector3(0.85f * SIN((fLifeTime * 4.0f*PI + (I_TO_F(i) / 2.0f))), 1.2f - 4.8f * fLifeTime, 0.0f).yxz() * FOREGROUND_AREA3);
            pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * (3.0f - 2.0f * pEnemy->GetCurHealthPct()));

            if(pEnemy->ReachedDeath())
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                for(coreUintW j = 20u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (j & 0x01u) ? 0.6f : 0.7f, pEnemy, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, (j & 0x01u) ? 0.6f : 0.7f, pEnemy, vPos, -vDir)->ChangeSize(1.3f);
                    // TODO: lower blue balls get overlapped 
                }
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 2-3
        g_pEnvironment->SetTargetSpeed(LERPS(5.0f, -5.0f, MIN(m_fStageTime * 0.5f, 1.0f)));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // r-type leaver 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cMinerEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(100, COLOR_SHIP_YELLOW);
            });

            pSquad1->GetEnemy(0u)->SetPosition(coreVector3(-0.8f,1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(1u)->SetPosition(coreVector3(-0.8f,1.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(2u)->SetPosition(coreVector3(-0.8f,1.6f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(3u)->SetPosition(coreVector3( 0.8f,1.2f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(4u)->SetPosition(coreVector3( 0.8f,1.4f,0.0f) * FOREGROUND_AREA3);
            pSquad1->GetEnemy(5u)->SetPosition(coreVector3( 0.8f,1.6f,0.0f) * FOREGROUND_AREA3);

            pSquad1->GetEnemy(0u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(1u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(2u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(3u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(4u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
            pSquad1->GetEnemy(5u)->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 3u, 5u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            const cPlayer*    pPlayer = pEnemy->NearestPlayer();
            const coreVector2 vDiff   = pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy();

            if(coreVector2::Dot(vDiff, pEnemy->GetDirection().xy()) <= 0.0f)
            {
                if(pEnemy->GetDirection().x == 0.0f) pEnemy->SetDirection(coreVector3(SIGN(vDiff.x), 0.0f, 0.0f));
                                                else pEnemy->SetDirection(coreVector3(0.0f, SIGN(vDiff.y), 0.0f));
            }

            pEnemy->DefaultMoveForward(pEnemy->GetDirection().xy(), 30.0f);


            if(STAGE_TICK_TIME_FREE(3.0f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cMineBullet>(5, 0.0f, pEnemy, vPos, vDir);
            }
        });

        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [this](cBullet* OUTPUT pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

            pBulletPlayer->Deactivate(true, vIntersection.xy());

            const coreInt32 iDamage = pBulletEnemy->GetDamage();
            if(iDamage == 1) pBulletEnemy->Deactivate(true, vIntersection.xy());
                        else pBulletEnemy->SetDamage(iDamage - 1);
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 3-4
        g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(-0.5f*PI, 0.0f*PI, MIN(m_fStageTime * 0.5f, 1.0f))));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // dicke kugerl die am rand bleiben 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 4u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetSize  (coreVector3(1.4f,1.4f,1.4f));
                pEnemy->Configure(200, COLOR_SHIP_RED);
            });
        });

        if(STAGE_CLEARED)
        {
                 if(STAGE_SUB(1u)) STAGE_RESSURECT(pSquad1, 0u, 0u)
            else if(STAGE_SUB(2u)) STAGE_RESSURECT(pSquad1, 1u, 3u)
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.4f, 0.0f)

            const coreVector2 vTarget = coreVector2(i ? (0.3f * (-3.0f + 1.5f * I_TO_F(i))) : 0.0f, 0.5f);

            if(STAGE_TAKEOFF)
            {
                pEnemy->SetPosition(coreVector3((vTarget + coreVector2(0.0f,1.0f)) * FOREGROUND_AREA, 0.0f));
            }

            pEnemy->DefaultMoveSmooth(vTarget, 40.0f, 10.0f);  

            if(STAGE_TICK_LIFETIME(20.0f, (i == 2u) ? 0.5f : 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.9f, pEnemy, vPos, vDir)->ChangeSize(0.0f);
            }
        });

#define __SPEED2 (0.2f * BULLET_SPEED_FACTOR)
        
#define __SPEED (0.1f * BULLET_SPEED_FACTOR)

        
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([](cOrbBullet* OUTPUT pBullet)
        {
            const coreFloat fLen    = (pBullet->GetPosition().xy() - pBullet->GetOwner()->GetPosition().xy()).Length();
            const coreFloat fFactor = CLAMP(fLen / FOREGROUND_AREA.Length(), 0.0f, 1.0f);
            pBullet->SetSpeed(LERP(0.8f * BULLET_SPEED_FACTOR, 0.1f * BULLET_SPEED_FACTOR, fFactor));
            pBullet->SetSize(coreVector3(1.5f,1.5f,1.5f) * 10.0f * fFactor);
        });

        /*
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([](cConeBullet* OUTPUT pBullet)
        {
            //if(pBullet->GetSpeed() == __SPEED) return;
            if(coreMath::IsNear(pBullet->GetSpeed(), __SPEED)) return;

            if(!IN_FOREGROUND_AREA(pBullet->GetPosition(), 1.1f))
            {
                pBullet->SetSpeed(__SPEED);
                return;
            }

            Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, pBullet, [](cBullet* OUTPUT pBullet1, cBullet* OUTPUT pBullet2, const coreVector3& vIntersection, const coreBool bFirstHit)
            {
                //if(pBullet1->GetSpeed() != __SPEED) return;

                //if(coreMath::IsNear(pBullet1->GetSpeed(), __SPEED))
                   // pBullet2->SetSpeed(__SPEED2);

                pBullet2->SetSpeed(MAX(MIN(pBullet1->GetSpeed() + 0.2f * BULLET_SPEED_FACTOR, pBullet2->GetSpeed(), 0.9f * BULLET_SPEED_FACTOR), 0.0f));
            });
        });
        */


        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // between 4-5
        g_pEnvironment->SetTargetSpeed(LERPS(-5.0f, 5.0f, MIN(m_fStageTime * 0.5f, 1.0f)));

        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // enemies jump into camera 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 3u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(I_TO_F(i - 2u) * 0.2f, 1.1f, 0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (50, COLOR_SHIP_ORANGE);

                //pEnemy->Resurrect();
            });

            pSquad1->GetEnemy(0u)->Resurrect();
        });

        STAGE_GET_START(18u)
            STAGE_GET_VEC2_ARRAY (avOldPos, 3u)
            STAGE_GET_VEC2_ARRAY (avNewPos, 3u)
            STAGE_GET_FLOAT_ARRAY(afJump,   3u)
            STAGE_GET_FLOAT_ARRAY(afWait,   3u)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.2f * I_TO_F(i))

            if(afWait[i] < 1.0f)
            {
                afWait[i] += Core::System->GetTime();
                if(afWait[i] >= 1.0f)
                {
                    afJump[i] = 0.0f;
                    afWait[i] = 1.0f;

                    avOldPos[i] = pEnemy                 ->GetPosition().xy();
                    avNewPos[i] = pEnemy->NearestPlayer()->GetPosition().xy();
                }
            }
            else
            {
                afJump[i] += Core::System->GetTime();
                if(afJump[i] >= 1.0f)
                {
                    afJump[i] = 1.0f;
                    afWait[i] = 0.0f;
                }
            }

            const coreVector2 vPos    = LERP(avOldPos[i], avNewPos[i], afJump[i]);
            const coreFloat   fHeight = 50.0f * SIN(PI * afJump[i]);
            const coreFloat   fAngle  = LERP(0.0f*PI, 2.0f*PI, afJump[i]);

            pEnemy->SetPosition    (coreVector3(vPos, fHeight));
            pEnemy->DefaultMultiate(fAngle);
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Torus)
    });

    // ################################################################
    // enemy charges straight forward on death 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 8u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_IMMORTAL);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(8u)
            STAGE_GET_FLOAT_ARRAY(afSpeed, 8u)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.2f * I_TO_F(i))

            if(pEnemy->GetCurHealth() == 0)
            {
                afSpeed[i] += 80.0f * Core::System->GetTime();

                pEnemy->SetPosition(pEnemy->GetPosition() + pEnemy->GetDirection() * ((20.0f + afSpeed[i]) * Core::System->GetTime()));

                if(!g_pForeground->IsVisiblePoint(pEnemy->GetPosition().xy()))
                {
                    pEnemy->Kill(true);
                }
            }
            else
            {
                STAGE_REPEAT(pPath1->GetTotalDistance())

                const coreVector2 vFactor = coreVector2(1.0f,1.0f);
                const coreVector2 vOffset = coreVector2(((i < 12u) ? -1.0f : 1.0f) * 0.3f + 0.4f * SIN(2.0f*PI * I_TO_F(i % 12u) / 12.0f), 0.0f);

                pEnemy->DefaultMovePath(pPath1, vFactor, vOffset * vFactor, fLifeTime);
                pEnemy->SetDirection(coreVector3((i < 12u) ? -1.0f : 1.0f, 0.0f, 0.0f));
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // bend 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_BLUE);
                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));

                pEnemy->Resurrect();
            });
        });


        
        STAGE_GET_START(32u)
            STAGE_GET_VEC2_ARRAY(avForce, 16u)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, pSquad1, avForce)
        {
            for(coreUintW i = 0u; i < 16u; ++i)
            {
                if(pEnemy == pSquad1->GetEnemy(i))
                {
                    avForce[i] -= pBullet->GetDirection().xy() * 6.0f;
                    break;
                }
            }
        });

        
            


        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUintW x = i % 8u;
            const coreUintW y = i / 8u;

            const coreVector2 vTarget = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + 0.2f * I_TO_F(y));

            if(STAGE_TAKEOFF)
            {
               // pEnemy->SetPosition(coreVector3((vTarget + coreVector2(0.0f, 1.0f + 0.12f * I_TO_F(x))) * FOREGROUND_AREA, 0.0f));
                pEnemy->SetPosition(coreVector3((vTarget) * FOREGROUND_AREA, 0.0f));
            }

           // pEnemy->DefaultMoveSmooth(vTarget, 40.0f, 10.0f);  


            //const coreObject3D& oBall = m_aBallRaw[0];

            //const coreVector2 vDiff = pEnemy->GetPosition().xy() - oBall.GetPosition().xy();
            //const coreFloat   fLen  = vDiff.Length();
            //const coreFloat   fStr  = SQRT(MAX(15.0 - fLen, 0.0)) * 0.5f;
            //
            //pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + vDiff.Normalized() * fStr, 0.0f));

            
            //const coreVector2 vDiff = pEnemy->GetPosition().xy() - oBall.GetPosition().xy();
            //const coreFloat   fLen  = vDiff.Length();
            //const coreFloat   fStr  = SQRT(MAX(100.0 - fLen, 0.0)) * 0.5f;
            //
            //pEnemy->SetPosition(coreVector3(vTarget * FOREGROUND_AREA + vDiff.Normalized() * fStr, 0.0f));


            if(STAGE_TICK_TIME(1.2f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });

        

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vNewPos = pEnemy->GetPosition().xy();

            if(!avForce[i].IsNull())
            {
                vNewPos  += avForce[i] * Core::System->GetTime();
                avForce[i] *= FrictionFactor(3.0f);
            }

            
                 if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; avForce[i].x =  ABS(avForce[i].x);}
            else if(vNewPos.x >  FOREGROUND_AREA.x) {vNewPos.x =  FOREGROUND_AREA.x; avForce[i].x = -ABS(avForce[i].x);}
                 if(vNewPos.y < -FOREGROUND_AREA.y) {vNewPos.y = -FOREGROUND_AREA.y; avForce[i].y =  ABS(avForce[i].y);}
            else if(vNewPos.y >  FOREGROUND_AREA.y) {vNewPos.y =  FOREGROUND_AREA.y; avForce[i].y = -ABS(avForce[i].y);}

            pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
        });

        STAGE_WAVE

        // pushing enemies are on left and right side and shooting on them will move them to the other side, they die when reaching the other side (is only second wave?)
    });

    // ################################################################
    // push 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 16u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(30, COLOR_SHIP_BLUE);
                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));

                pEnemy->Resurrect();
            });
        });


        
        STAGE_GET_START(32u)
            STAGE_GET_VEC2_ARRAY(avForce, 16u)
        STAGE_GET_END

        STAGE_COLL_ENEMY_BULLET(pEnemy, pBullet, vIntersection, pSquad1, avForce)
        {
            for(coreUintW i = 0u; i < 16u; ++i)
            {
                if(pEnemy == pSquad1->GetEnemy(i))
                {
                    avForce[i] -= pBullet->GetDirection().xy() * 6.0f;
                    break;
                }
            }
        });

        
            


        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreUintW x = i % 8u;
            const coreUintW y = i / 8u;

            const coreVector2 vTarget = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + 0.2f * I_TO_F(y));

            if(STAGE_TAKEOFF)
            {
               // pEnemy->SetPosition(coreVector3((vTarget + coreVector2(0.0f, 1.0f + 0.12f * I_TO_F(x))) * FOREGROUND_AREA, 0.0f));
                pEnemy->SetPosition(coreVector3((vTarget) * FOREGROUND_AREA, 0.0f));
            }

           // pEnemy->DefaultMoveSmooth(vTarget, 40.0f, 10.0f);  


            //const coreObject3D& oBall = m_aBallRaw[0];

            //const coreVector2 vDiff = pEnemy->GetPosition().xy() - oBall.GetPosition().xy();
            //const coreFloat   fLen  = vDiff.Length();
            //const coreFloat   fStr  = SQRT(MAX(15.0 - fLen, 0.0)) * 0.5f;
            //
            //pEnemy->SetPosition(coreVector3(pEnemy->GetPosition().xy() + vDiff.Normalized() * fStr, 0.0f));

            
            //const coreVector2 vDiff = pEnemy->GetPosition().xy() - oBall.GetPosition().xy();
            //const coreFloat   fLen  = vDiff.Length();
            //const coreFloat   fStr  = SQRT(MAX(100.0 - fLen, 0.0)) * 0.5f;
            //
            //pEnemy->SetPosition(coreVector3(vTarget * FOREGROUND_AREA + vDiff.Normalized() * fStr, 0.0f));


            if(STAGE_TICK_TIME(1.2f, 0.0f))
            {
                const coreVector2 vPos = pEnemy->GetPosition ().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
            }
        });

        

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            coreVector2 vNewPos = pEnemy->GetPosition().xy();

            if(!avForce[i].IsNull())
            {
                vNewPos  += avForce[i] * Core::System->GetTime();
                avForce[i] *= FrictionFactor(3.0f);
            }

            
                 if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; avForce[i].x =  ABS(avForce[i].x);}
            else if(vNewPos.x >  FOREGROUND_AREA.x) {vNewPos.x =  FOREGROUND_AREA.x; avForce[i].x = -ABS(avForce[i].x);}
                 if(vNewPos.y < -FOREGROUND_AREA.y) {vNewPos.y = -FOREGROUND_AREA.y; avForce[i].y =  ABS(avForce[i].y);}
            else if(vNewPos.y >  FOREGROUND_AREA.y) {vNewPos.y =  FOREGROUND_AREA.y; avForce[i].y = -ABS(avForce[i].y);}

            pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
        });

        STAGE_WAVE

        // pushing enemies are on left and right side and shooting on them will move them to the other side, they die when reaching the other side (is only second wave?)
    });

    // ################################################################
    // can only kill in order 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 5u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(I_TO_F(i - 2u) * 0.2f, 1.1f, 0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus  (ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });

            cEnemy* pChief = pSquad1->GetEnemy(1u);

            pChief->SetBaseColor(COLOR_SHIP_BLUE);
            pChief->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->DefaultMoveSmooth(coreVector2(I_TO_F(i - 2u) * 0.2f, 0.3f), 10.0f, 1.0f);  

            if(pEnemy->ReachedDeath() && !pSquad1->IsFinished())
            {
                cEnemy* pNewChief = pSquad1->FindEnemyRev(pEnemy->GetPosition().xy());

                pNewChief->SetBaseColor(COLOR_SHIP_BLUE);
                pNewChief->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // invincibility switches every few seconds 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 8u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->SetPosition(coreVector3(I_TO_F(i - 2u) * 0.2f, 1.1f, 0.0f) * FOREGROUND_AREA3);
                pEnemy->Configure  (50, COLOR_SHIP_ORANGE);
                pEnemy->AddStatus  (ENEMY_STATUS_INVINCIBLE);

                pEnemy->Resurrect();
            });
        });

        STAGE_GET_START(1u)
            STAGE_GET_UINT(iSwitch)
        STAGE_GET_END

        if(FMOD(m_fStageTime, 1.0f) < FMOD(m_fStageTimeBefore, 1.0f))
        {
            iSwitch = 1u - iSwitch;
        }

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            pEnemy->DefaultMoveSmooth(coreVector2(I_TO_F(i - 2u) * 0.2f, 0.3f), 10.0f, 1.0f);  

            if(i % 2u == iSwitch)
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 0.8f);
                pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                pEnemy->RemoveStatus(ENEMY_STATUS_INVINCIBLE);
            }
            else
            {
                pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.5f);
                pEnemy->SetBaseColor(COLOR_SHIP_ORANGE);
                pEnemy->AddStatus(ENEMY_STATUS_INVINCIBLE);
            }
        });

        STAGE_WAVE
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        UNUSED STAGE_ADD_SQUAD(pSquad1, cCinderEnemy, 24u)//18u)//VAUS_SCOUTS_X * VAUS_SCOUTS_Y)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(40, COLOR_SHIP_GREY);
            });
        });

        STAGE_BOSS(m_Vaus)
    });

    // ################################################################
    // ################################################################
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnUnder()
{
    // 
    //m_BallTrail.Render();
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnAttack()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_BallTrail.Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    m_Ball.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Ball);

    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        m_aPaddle[i].Render();
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyObject(&m_aPaddle[i]);

    // 
    m_Barrier.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_DIRECT)->ApplyList(&m_Barrier);
}


// ****************************************************************
// 
void cViridoMission::__MoveOwnBefore()
{
    // 
    m_fAnimation.UpdateMod(0.2f, 10.0f);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS; ++i)
    {
        coreObject3D* pBall = (*m_Ball.List())[i];
        if(!pBall->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreVector3& vSize = pBall->GetSize();

        // 
        coreVector2       vNewDir = pBall->GetDirection().xy();
        const coreVector2 vNewPos = pBall->GetPosition ().xy() + vNewDir * FOREGROUND_AREA * (CONTAINS_BIT(m_iStickyState, 1u) ? 0.0f : (VIRIDO_BALL_SPEED * Core::System->GetTime()));

        // restrict movement to the foreground area
             if((vNewPos.x < -FOREGROUND_AREA.x) && (vNewDir.x < 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2(-vSize.x, 0.0f)); vNewDir.x =  ABS(vNewDir.x); if(!i) ADD_BIT(m_iBounceState, 7u)}
        else if((vNewPos.x >  FOREGROUND_AREA.x) && (vNewDir.x > 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2( vSize.x, 0.0f)); vNewDir.x = -ABS(vNewDir.x); if(!i) ADD_BIT(m_iBounceState, 7u)}
             if((vNewPos.y < -FOREGROUND_AREA.y) && (vNewDir.y < 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2( 0.0f,-vSize.y)); vNewDir.y =  ABS(vNewDir.y); if(!i) ADD_BIT(m_iBounceState, 7u)}
        else if((vNewPos.y >  FOREGROUND_AREA.y) && (vNewDir.y > 0.0f)) {cViridoMission::__BounceEffect(vNewPos + coreVector2( 0.0f, vSize.y)); vNewDir.y = -ABS(vNewDir.y); if(!i) ADD_BIT(m_iBounceState, 7u)}

        // 
        pBall->SetPosition (coreVector3(vNewPos, 0.0f));
        pBall->SetDirection(coreVector3(vNewDir, 0.0f));
        pBall->SetTexOffset(coreVector2(0.0f, m_fAnimation));

        for(coreUintW j = 0u; j < VIRIDO_TRAILS; ++j)
        {
            coreObject3D* pTrail = (*m_BallTrail.List())[i*VIRIDO_TRAILS + j];

            // 
            pTrail->SetAlpha(pTrail->GetAlpha() - 2.0f * Core::System->GetTime());
            if(pTrail->GetAlpha() < 0.0f)
            {
                // 
                pTrail->SetPosition (pBall->GetPosition ());
                pTrail->SetDirection(pBall->GetDirection());
                pTrail->SetAlpha    (1.0f);
            }

            // 
            pTrail->SetSize     (vSize * pTrail->GetAlpha());
            pTrail->SetColor3   (COLOR_ENERGY_GREEN * LERP(0.15f, 0.8f, pTrail->GetAlpha()));
            pTrail->SetTexOffset(coreVector2(0.0f, m_fAnimation));
        }
    }

    // 
    m_Ball     .MoveNormal();
    m_BallTrail.MoveNormal();
}


// ****************************************************************
// 
void cViridoMission::__MoveOwnAfter()
{
    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
    {
        coreObject3D& oPaddle       = m_aPaddle      [i];
        coreObject3D& oPaddleSphere = m_aPaddleSphere[i];
        if(!oPaddle.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreBool bBoss = i ? false : true;

        // 
        const cShip* pOwner = m_apPaddleOwner[i];
        if(pOwner)
        {
            oPaddle.SetPosition (coreVector3(pOwner->GetPosition ().xy() + pOwner->GetDirection().xy() * 3.0f, 0.0f));
            oPaddle.SetDirection(coreVector3(pOwner->GetDirection().xy(), 0.0f));
        }

        // 
        oPaddle.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
        oPaddle.Move();

        // 
        oPaddleSphere.SetPosition(coreVector3(oPaddle.GetPosition().xy() - oPaddle.GetDirection().xy() * (bBoss ? 27.0f : 12.0f), 0.0f));
        oPaddleSphere.Move();
    }

    // 
    for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
    {
        coreObject3D& oBarrier = m_aBarrierRaw[i];
        if(!oBarrier.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const cShip* pOwner = m_apBarrierOwner[i];
        if(pOwner)
        {
            const coreVector2 vDir = UnpackDirection(m_aiBarrierDir[i]);

            oBarrier.SetPosition (coreVector3(pOwner->GetPosition().xy() + vDir * 3.0f, 0.0f));
            oBarrier.SetDirection(coreVector3(vDir, 0.0f));
        }

        // 
        oBarrier.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
    }

    // 
    m_Barrier.MoveNormal();

    // 
    m_iBounceState = 0u;
    STATIC_ASSERT(VIRIDO_PADDLES+1u <= sizeof(m_iBounceState)*8u)

    // 
    if(!CONTAINS_BIT(m_iStickyState, 1u))
    {
        // 
        Core::Manager::Object->TestCollision(TYPE_VIRIDO_PADDLE, TYPE_VIRIDO_BALL, [this](coreObject3D* OUTPUT pPaddle, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!bFirstHit || (coreVector2::Dot(pPaddle->GetDirection().xy(), pBall->GetDirection().xy()) >= 0.0f))
                return;

            // 
            const coreUintW     iIndex        = pPaddle - m_aPaddle;
            const coreObject3D& oPaddleSphere = m_aPaddleSphere[iIndex];

            // 
            coreVector3 vImpact;
            if(Core::Manager::Object->TestCollision(&oPaddleSphere, pBall, &vImpact))
            {
                const coreVector2 vBallPos   = pBall  ->GetPosition ().xy();
                const coreVector2 vBallDir   = pBall  ->GetDirection().xy();
                const coreVector2 vPaddleDir = pPaddle->GetDirection().xy();

                if(CONTAINS_BIT(m_iRealState, iIndex))
                {
                    // 
                    coreVector2 vNewDir = coreVector2::Reflect(vBallDir, (vBallPos - oPaddleSphere.GetPosition().xy()).Normalized());
                    if(IsHorizontal(vPaddleDir)) vNewDir.x = MAX(ABS(vNewDir.x), 0.75f) * vPaddleDir.x;
                                            else vNewDir.y = MAX(ABS(vNewDir.y), 0.75f) * vPaddleDir.y;

                    // 
                    pBall->SetDirection(coreVector3(vNewDir.Normalized(), 0.0f));
                }
                else
                {
                    // 
                    coreVector2 vNewDir = vBallDir;
                    if(IsHorizontal(vPaddleDir)) vNewDir.x = ABS(vNewDir.x) * vPaddleDir.x;
                                            else vNewDir.y = ABS(vNewDir.y) * vPaddleDir.y;

                    // 
                    pBall->SetDirection(coreVector3(vNewDir, 0.0f));
                }

                // 
                ADD_BIT(m_iBounceState, iIndex)
                if(m_iStickyState) ADD_BIT(m_iStickyState, 1u)

                // 
                cViridoMission::__BounceEffect(vImpact.xy());
            }
        });
    }

    // 
    if(CONTAINS_BIT(m_iStickyState, 1u))
    {
        // only between first ball and first paddle
        coreObject3D& oBall   = m_aBallRaw[0];
        coreObject3D& oPaddle = m_aPaddle [0];

        // 
        oBall.SetPosition(oPaddle.GetPosition() + oPaddle.GetDirection() * (oPaddle.GetCollisionRange().y * 2.0f - 0.3f));
        oBall.Move();
    }

    // 
    cPlayer::TestCollision(TYPE_VIRIDO_BALL, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(10, ELEMENT_GREEN, vIntersection.xy());

        // 
        g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_GREEN);
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_VIRIDO_BALL, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pBullet->Reflect(pBall, vIntersection.xy());
    });




    // 
    cPlayer::TestCollision(TYPE_VIRIDO_BARRIER, [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBarrier, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        const coreVector2 vDir = pBarrier->GetDirection().xy();//(pPlayer->GetPosition().xy() - pBarrier->GetPosition().xy()).Normalized();
        pPlayer->SetForce(vDir * 100.0f);

        // 
        //g_pSpecialEffects->MacroExplosionColorSmall(vIntersection, COLOR_ENERGY_GREEN);
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_VIRIDO_BARRIER, [](cBullet* OUTPUT pBullet, coreObject3D* OUTPUT pBarrier, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pBullet->Reflect(pBarrier, vIntersection.xy(), pBarrier->GetDirection().xy());
    });




    // 
    if(!CONTAINS_FLAG(m_Vaus.GetStatus(), ENEMY_STATUS_DEAD))
    {
        cEnemy*     pCurEnemy  = NULL;
        coreFloat   fCurLenSq  = FLT_MAX;
        coreVector2 vCurImpact = coreVector2(FLT_MAX,FLT_MAX);

        // only first ball will be active in this mission stage
        coreObject3D& oBall = m_aBallRaw[0];

        // 
        const coreVector2 vBallPos    = oBall.GetPosition ().xy();
        const coreVector2 vBallDir    = oBall.GetDirection().xy();
        const coreVector2 vOldBallPos = vBallPos - vBallDir * FOREGROUND_AREA * (CONTAINS_BIT(m_iStickyState, 1u) ? 0.0f : (VIRIDO_BALL_SPEED * Core::System->GetTime()));

        // 
        Core::Manager::Object->TestCollision(TYPE_ENEMY, &oBall, [&](cEnemy* OUTPUT pEnemy, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!bFirstHit || (pEnemy->GetID() != cCinderEnemy::ID)) return;

            // 
            const coreFloat fNewLenSq = (pEnemy->GetPosition().xy() - vOldBallPos).LengthSq();
            if(fNewLenSq < fCurLenSq)
            {
                pCurEnemy  = pEnemy;
                fCurLenSq  = fNewLenSq;
                vCurImpact = vIntersection.xy();
            }
        });

        if(pCurEnemy)
        {
            const coreVector2 vDiff = vOldBallPos - pCurEnemy->GetPosition().xy();

            // 
            coreUintW iAxis = 0u;
            if(IsHorizontal(vDiff)) iAxis = ((vDiff.x * vBallDir.x) >= 0.0f) ? 1u : 0u;
                               else iAxis = ((vDiff.y * vBallDir.y) >= 0.0f) ? 0u : 1u;

            // 
            coreVector2 vNewDir = vBallDir;
            vNewDir.arr(iAxis) = ABS(vBallDir.arr(iAxis)) * SIGN(vDiff.arr(iAxis));

            // 
            oBall.SetPosition (coreVector3(vBallPos + vDiff * (3.0f * Core::System->GetTime()), 0.0f));
            oBall.SetDirection(coreVector3(vNewDir, 0.0f));

            // 
            pCurEnemy->Kill(true);

            // 
            cViridoMission::__BounceEffect(vCurImpact);
        }
    }
}


// ****************************************************************
// 
void cViridoMission::__BounceEffect(const coreVector2& vEffectPos)
{
    g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
    g_pSpecialEffects->PlaySound        (coreVector3(vEffectPos, 0.0f), 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
}