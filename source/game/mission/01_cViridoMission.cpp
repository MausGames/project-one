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
: m_Ball         (VIRIDO_BALLS)
, m_BallTrail    (VIRIDO_BALLS * VIRIDO_TRAILS)
, m_apOwner      {}
, m_iRealState   (0u)
, m_iStickyState (0u)
, m_iBounceState (0u)
, m_fAnimation   (0.0f)
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
    g_pGlow->BindList(&m_Ball);
    g_pGlow->BindList(&m_BallTrail);

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
}


// ****************************************************************
// destructor
cViridoMission::~cViridoMission()
{
    // 
    g_pGlow->UnbindList(&m_Ball);
    g_pGlow->UnbindList(&m_BallTrail);

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS; ++i)
        this->DisableBall(i, false);

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        this->DisablePaddle(i, false);
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
    m_apOwner[iIndex] = pOwner;

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
// setup the Virido mission
void cViridoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(-cGrassBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pEnvironment->GetBackground()->GetOutdoor()->LerpHeightNow(0.4583f, -13.83f);
            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(1u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(2u, 0.0f);
            g_pEnvironment->GetBackground()->SetDecalDensity (0u, 0.0f);
            //g_pEnvironment->GetBackground()->SetAirDensity   (0u, 0.0f);

            g_pEnvironment->SetTargetSpeed(5.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    /*

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-0.8f, 1.5f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2( 0.8f,-1.5f), coreVector2(0.0f,-1.0f));
            pPath1->Refine();
        });
        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(2u);
            pPath2->AddNode(coreVector2(-1.5f, 0.2f), coreVector2(1.0f,0.0f));
            pPath2->AddNode(coreVector2( 1.5f,-0.4f), coreVector2(1.0f,0.0f));
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, (i/5u) ? COLOR_SHIP_YELLOW : COLOR_SHIP_BLUE);
                pEnemy->Resurrect();
            });
        });
        STAGE_ADD_SQUAD(pSquad2, cScoutEnemy, 10u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad2, pEnemy, i)
            {
                pEnemy->Configure(4, (i/5u) ? COLOR_SHIP_YELLOW : COLOR_SHIP_BLUE);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.3f * I_TO_F(i))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreFloat   fSide   = (i/5u) ? -1.0f : 1.0f;
            const coreVector2 vFactor = coreVector2(fSide, 1.0f);
            const coreVector2 vOffset = coreVector2(0.1f * I_TO_F(i%5u - 1u) * fSide, 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.3f * I_TO_F(i+10u))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath2->GetTotalDistance())

            const coreFloat   fSide   = (i/5u) ? -1.0f : 1.0f;
            const coreVector2 vFactor = coreVector2(fSide, 1.0f);
            const coreVector2 vOffset = coreVector2(0.0f, 0.7f - 0.1f * I_TO_F(i%5u));

            pEnemy->DefaultMovePath(pPath2, vFactor, vOffset, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(3u);
            pPath1->AddNode(coreVector2(-0.6f, 1.5f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(-0.1f,-0.1f), coreVector2(1.0f,-1.0f).Normalized());
            pPath1->AddNode(coreVector2( 1.5f,-0.6f), coreVector2(1.0f, 0.0f));
            pPath1->Refine();
        });
        STAGE_ADD_PATH(pPath2)
        {
            pPath2->Reserve(3u);
            pPath2->AddNode(coreVector2(-0.6f, 1.5f).Rotated90().InvertedX(), coreVector2(0.0f,-1.0f).Rotated90().InvertedX());
            pPath2->AddNode(coreVector2(-0.1f,-0.1f).Rotated90().InvertedX(), coreVector2(1.0f,-1.0f).Rotated90().InvertedX().Normalized());
            pPath2->AddNode(coreVector2( 1.5f,-0.6f).Rotated90().InvertedX(), coreVector2(1.0f, 0.0f).Rotated90().InvertedX());
            pPath2->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 12u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, COLOR_SHIP_RED);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 2.0f * I_TO_F(i/3u))

            const coreSpline2* pPath = (i >= 6u) ? pPath2 : pPath1;

            STAGE_REMOVE_LIFETIME(pEnemy, pPath->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i%6u) >= 3u) ? -1.0f : 1.0f, 1.0f);
            const coreVector2 vOffset = coreVector2(0.2f * I_TO_F((i%3u) - 1u), ((i%3u) == 1u) ? -0.2f : 0.0f);

            pEnemy->DefaultMovePath(pPath, vFactor, vOffset, fLifeTime);

            //if(STAGE_LIFETIME_AFTER(0.3f) && STAGE_TICK_COUNTED(10u, 6u, i))
            //{
            //    const coreVector2 vPos = pEnemy->GetPosition().xy();
            //    const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();
            //
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 2.0f, pEnemy, vPos, vDir);
            //}
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(-1.5f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->AddNode(coreVector2( 1.5f,0.8f), coreVector2(1.0f,0.0f));
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 30u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(32, (i%10u) ? COLOR_SHIP_YELLOW : COLOR_SHIP_BLUE);
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.2f, 0.4f * I_TO_F(i) - 1.0f * I_TO_F(i/10u))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(((i/10u) & 0x01u) ? -1.0f : 1.0f, 1.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, coreVector2(0.0f,0.0f), fLifeTime);

            //if((i%10u) && STAGE_LIFETIME_AFTER(0.5f) && STAGE_TICK(6u))
            //{
            //    const coreVector2 vPos = pEnemy->GetPosition ().xy();
            //    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();
            //
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir);
            //    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir);
            //}

            if((i%10u) && STAGE_LIFETIME_AFTER(0.5f) && STAGE_TICK_OFFSET(18u, (i & 0x01u) ? 9u : 0u))
            {
                g_pGame->GetEnemyManager()->AttachFunction(pEnemy, 3u, 0.05f, NULL, [](cEnemy* OUTPUT pEnemy, const coreUint8 iCurRepeat, const coreVariant16& oData)
                {
                    const coreVector2 vPos = pEnemy->GetPosition ().xy();
                    const coreVector2 vDir = pEnemy->GetDirection().xy().Rotated90();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos,  vDir);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, vPos, -vDir);
                });
            }
        });

        STAGE_FINISH_CLEARED
    });

    */

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Dharuk, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });

    /*

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f) * 2.5f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i & 0x03u) * 0.333f));
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.75f, 0.2f * I_TO_F(i))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.025f * I_TO_F(MIN(i + 8u, pSquad1->GetNumEnemies() - 1u - i + 4u)) * ((i & 0x04u) ? -1.0f : 1.0f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    */

    // ################################################################
    // 
    STAGE_MAIN
    {
        //UNUSED STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 16u)    
        //{
        //    STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
        //    {
        //        pEnemy->Configure(4, COLOR_SHIP_RED);    
        //    });
        //});

        //STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        //{
        //    STAGE_REMOVE_AREA(pEnemy)
        //});

        STAGE_BOSS(m_Torus, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });

    /*

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_PATH(pPath1)
        {
            pPath1->Reserve(2u);
            pPath1->AddNode(coreVector2(0.0f, 1.2f), coreVector2(0.0f,-1.0f));
            pPath1->AddNode(coreVector2(0.0f,-1.2f), coreVector2(0.0f,-1.0f) * 2.5f);
            pPath1->Refine();
        });

        STAGE_ADD_SQUAD(pSquad1, cScoutEnemy, 20u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(4, LERP(COLOR_SHIP_YELLOW, COLOR_SHIP_RED, I_TO_F(i & 0x03u) * 0.333f));
                pEnemy->Resurrect();
            });
        });

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.75f, 0.2f * I_TO_F(i))

            STAGE_REMOVE_LIFETIME(pEnemy, pPath1->GetTotalDistance())

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2(0.025f * I_TO_F(MIN(i + 8u, pSquad1->GetNumEnemies() - 1u - i + 4u)) * ((i & 0x04u) ? -1.0f : 1.0f), 0.0f);

            pEnemy->DefaultMovePath(pPath1, vFactor, vOffset, fLifeTime);
        });

        STAGE_FINISH_CLEARED
    });

    */

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

        STAGE_BOSS(m_Vaus, coreVector2(0.0f,-2.0f), coreVector2(0.0f,-1.0f))
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
        const cShip* pOwner = m_apOwner[i];
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
    m_iBounceState = 0u;
    STATIC_ASSERT(VIRIDO_PADDLES+1u <= sizeof(m_iBounceState)*8u)

    // 
    if(!CONTAINS_BIT(m_iStickyState, 1u))
    {
        // 
        Core::Manager::Object->TestCollision(TYPE_VIRIDO_PADDLE, TYPE_VIRIDO_BALL, [this](coreObject3D* OUTPUT pPaddle, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(coreVector2::Dot(pPaddle->GetDirection().xy(), pBall->GetDirection().xy()) >= 0.0f)
                return;

            // 
            const coreUintW     iIndex        = pPaddle - m_aPaddle;
            const coreObject3D& oPaddleSphere = m_aPaddleSphere[iIndex];

            // 
            coreVector3 vImpact;
            if(coreObjectManager::TestCollision(&oPaddleSphere, pBall, &vImpact))
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
        // 
        //pBullet->Deactivate(true, vIntersection.xy());
        pBullet->Reflect(pBall);
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



        oBall.SetSize(coreVector3(2.7f,2.7f,2.7f) * 1.2f);  
        oBall.Move();  



        // 
        Core::Manager::Object->TestCollision(TYPE_ENEMY, &oBall, [&](cEnemy* OUTPUT pEnemy, coreObject3D* OUTPUT pBall, const coreVector3& vIntersection, const coreBool bFirstHit)
        {
            // 
            if(!bFirstHit) return;
            if(pEnemy->GetID() != cCinderEnemy::ID) return;

            // 
            const coreFloat fNewLenSq = (pEnemy->GetPosition().xy() - vOldBallPos).LengthSq();
            if(fNewLenSq < fCurLenSq)
            {
                pCurEnemy  = pEnemy;
                fCurLenSq  = fNewLenSq;
                vCurImpact = vIntersection.xy();
            }
        });



        oBall.SetSize(coreVector3(2.7f,2.7f,2.7f));  
        oBall.Move();  



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
            //pCurEnemy->TakeDamage(10, ELEMENT_GREEN, vCurImpact, NULL);

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