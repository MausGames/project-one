//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cViridoMission::cViridoMission()noexcept
: m_Ball         (VIRIDO_BALLS)
, m_BallTrail    (VIRIDO_BALLS * VIRIDO_TRAILS)
, m_apOwner      {}
, m_iBounceState (0u)
, m_bBounceReal  (false)
, m_fAnimation   (0.0f)
{
    // 
    m_apBoss[0] = &m_Crossfield;
    m_apBoss[1] = &m_Torus;
    m_apBoss[2] = &m_Vaus;

    // create ball lists
    m_Ball     .DefineProgram("effect_energy_invert_inst_program");
    m_BallTrail.DefineProgram("effect_energy_invert_inst_program");
    {
        for(coreUintW i = 0u; i < VIRIDO_RAWS; ++i)
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
        m_aPaddle[i].DefineModel  ("object_boss_vaus_paddle.md3");
        m_aPaddle[i].DefineTexture(0u, "effect_energy.png");
        m_aPaddle[i].DefineProgram("effect_energy_bullet_direct_program");
        m_aPaddle[i].SetSize      (bBoss ? coreVector3(3.5f,2.5f,2.5f) : coreVector3(2.5f,2.5f,2.5f));
        m_aPaddle[i].SetColor3    (bBoss ? COLOR_ENERGY_BLUE           : COLOR_ENERGY_RED);
        m_aPaddle[i].SetTexSize   (coreVector2(1.2f,0.25f) * 0.5f);
        m_aPaddle[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
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
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        this->DisablePaddle(i, false);
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
            g_pEnvironment->ChangeBackground(-REF_ID(cGrassBackground::ID), ENVIRONMENT_MIX_FADE, 1.0f);
            g_pEnvironment->GetBackground()->GetOutdoor()->SetHeightOffset(-24.5f);
            g_pEnvironment->GetBackground()->GetOutdoor()->SetHeightFactor(0.0f);
            g_pEnvironment->GetBackground()->GetOutdoor()->SetHeightOffset(-13.83f);
            g_pEnvironment->GetBackground()->GetOutdoor()->SetHeightFactor(0.4583f);

            g_pEnvironment->GetBackground()->SetGroundDensity(0u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(1u, 0.0f);
            g_pEnvironment->GetBackground()->SetGroundDensity(2u, 0.0f);
            g_pEnvironment->GetBackground()->SetDecalDensity (0u, 0.0f);
            g_pEnvironment->GetBackground()->SetAirDensity   (0u, 0.0f);

            g_pEnvironment->SetTargetSpeed  (5.0f);

            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
        {
            STAGE_FINISH_NOW
        }
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_ADD_SQUAD(pSquad1, cStarEnemy, 6u)
        {
            STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
            {
                pEnemy->Configure(5, LERP(COLOR_SHIP_BLUE, COLOR_SHIP_GREEN, I_TO_F(i) / I_TO_F(pSquad1->GetNumEnemies() - 1u)));
                pEnemy->Resurrect(coreVector2((i & 0x01u) ? -0.9f : 0.9f, 1.2f) * FOREGROUND_AREA);
            });
        });

        STAGE_GET_START(0u, 12u)
        STAGE_GET_VEC2_ARRAY(avDir, 6u)
        STAGE_GET_END

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 0.7f, -0.1f * I_TO_F(i))

            STAGE_REMOVE_AREA(pEnemy)

            if(fLifeTime >= 0.0f)
            {
                if(avDir[i].IsNull()) avDir[i] = pEnemy->AimAtPlayer();

                pEnemy->DefaultMoveForward(avDir[i], 30.0f);
            }
        });

        STAGE_FINISH_CLEARED
    });

    // ################################################################
    // 
    STAGE_MAIN
    {

    });

    // ################################################################
    // 
    STAGE_MAIN
    {

    });

    // ################################################################
    // 
    STAGE_MAIN
    {

    });

    // ################################################################
    // 
    STAGE_MAIN
    {

    });

    // ################################################################
    // 
    STAGE_MAIN
    {

    });

    // ################################################################
    // 
    STAGE_START_HERE
    STAGE_MAIN
    {
        // 
        if(STAGE_BEGINNING) m_Crossfield.Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));

        // 
        if(CONTAINS_FLAG(m_Crossfield.GetStatus(), ENEMY_STATUS_DEAD))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // 
        g_pEnvironment->SetTargetDirection(coreVector2::Direction(LERPS(-1.0f*PI, 0.0f*PI, m_fStageTime*0.5f)));

        // 
        STAGE_FINISH_AFTER(2.0f)
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        if(g_pEnvironment->GetBackground()->GetID() == cCloudBackground::ID)
        {
            // 
            //const coreFloat fNewAlpha = 0.33f * CLAMP(1.0f - m_fStageTime / 4.0f, 0.0f, 1.0f);
            //s_cast<cCloudBackground*>(g_pEnvironment->GetBackground())->SetCloudAlpha(fNewAlpha);
            //s_cast<cCloudBackground*>(g_pEnvironment->GetBackground())->ReduceClouds();
        }

        // 
        STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        // 
        if(STAGE_BEGINNING) m_Torus.Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));

        // 
        if(CONTAINS_FLAG(m_Torus.GetStatus(), ENEMY_STATUS_DEAD))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    //STAGE_MAIN
    //{
    //    if(STAGE_SUB(0.0f))
    //    {
    //        // 
    //        for(coreUintW i = 0u; i < VIRIDO_SCOUTS; ++i)
    //            m_aScout[i].SetCollisionModifier(coreVector3(1.2f,1.2f,1.5f)); // TODO # make quadratic! 
    //
    //
    //        m_Warrior.SetSize(m_Warrior.GetSize() * 1.2f);
    //
    //
    //        // 
    //        m_Vaus.Resurrect(coreVector2(0.0f,-2.0f) * FOREGROUND_AREA, coreVector2(0.0f,1.0f));
    //    }
    //
    //    // 
    //    if(g_pGame->GetEnemyList()->empty())
    //        STAGE_FINISH_NOW
    //});
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnUnder()
{
    // 
    m_BallTrail.Render();
}


// ****************************************************************
// 
void cViridoMission::__RenderOwnAttack()
{
    DEPTH_PUSH

    // 
    m_Ball.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Ball);

    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        m_aPaddle[i].Render();
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        g_pOutline->GetStyle(OUTLINE_STYLE_DIRECT)->ApplyObject(&m_aPaddle[i]);
}


// ****************************************************************
// 
void cViridoMission::__MoveOwnAfter()
{
    // 
    m_fAnimation.Update(0.2f);

    // 
    auto nEffectFunc = [](const coreVector2& vEffectPos)
    {
        g_pSpecialEffects->CreateSplashColor(coreVector3(vEffectPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
        g_pSpecialEffects->PlaySound        (coreVector3(vEffectPos, 0.0f), 1.0f, SOUND_EXPLOSION_ENERGY_SMALL);
    };

    // 
    for(coreUintW i = 0u; i < VIRIDO_BALLS; ++i)
    {
        coreObject3D* pBall = (*m_Ball.List())[i];
        if(!pBall->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        const coreVector3& vSize = pBall->GetSize();

        // 
        coreVector2       vNewDir = pBall->GetDirection().xy();
        const coreVector2 vNewPos = pBall->GetPosition ().xy() + vNewDir * FOREGROUND_AREA * (VIRIDO_BALL_SPEED * Core::System->GetTime());

        // restrict movement to the foreground area
             if((vNewPos.x < -FOREGROUND_AREA.x) && (vNewDir.x < 0.0f)) {nEffectFunc(vNewPos + coreVector2(-vSize.x, 0.0f)); vNewDir.x =  ABS(vNewDir.x);}
        else if((vNewPos.x >  FOREGROUND_AREA.x) && (vNewDir.x > 0.0f)) {nEffectFunc(vNewPos + coreVector2( vSize.x, 0.0f)); vNewDir.x = -ABS(vNewDir.x);}
             if((vNewPos.y < -FOREGROUND_AREA.y) && (vNewDir.y < 0.0f)) {nEffectFunc(vNewPos + coreVector2( 0.0f,-vSize.y)); vNewDir.y =  ABS(vNewDir.y);}
        else if((vNewPos.y >  FOREGROUND_AREA.y) && (vNewDir.y > 0.0f)) {nEffectFunc(vNewPos + coreVector2( 0.0f, vSize.y)); vNewDir.y = -ABS(vNewDir.y);}

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

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
    {
        coreObject3D& oPaddle       = m_aPaddle      [i];
        coreObject3D& oPaddleSphere = m_aPaddleSphere[i];
        if(!oPaddle.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        // 
        const coreBool bBoss  = i ? false : true;

        // 
        const cShip* pOwner = m_apOwner[i];
        if(pOwner)
        {
            oPaddle.SetPosition (coreVector3(pOwner->GetPosition ().xy() + pOwner->GetDirection().xy() * (bBoss ? 3.0f : 4.0f), 0.0f));
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
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_OBJECT(2), [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pBall, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(10, ELEMENT_GREEN);

        // 
        const coreVector3 vCenter = coreVector3((pPlayer->GetPosition().xy() + pBall->GetPosition().xy()) * 0.5f, 0.0f);
        g_pSpecialEffects->MacroExplosionColorSmall(vCenter, COLOR_ENERGY_GREEN);
    });

    // 
    m_iBounceState = 0u;
    STATIC_ASSERT(VIRIDO_PADDLES <= sizeof(m_iBounceState)*8u)

    // 
    Core::Manager::Object->TestCollision(TYPE_OBJECT(3), TYPE_OBJECT(2), [&](coreObject3D* OUTPUT pPaddle, coreObject3D* OUTPUT pBall, const coreBool bFirstHit)
    {
        // 
        if(coreVector2::Dot(pPaddle->GetDirection().xy(), pBall->GetDirection().xy()) >= 0.0f)
            return;

        // 
        for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        {
            if(pPaddle == &m_aPaddle[i])
            {
                const coreObject3D& oPaddleSphere = m_aPaddleSphere[i];

                // 
                if(coreObjectManager::TestCollision(&oPaddleSphere, pBall))
                {
                    const coreVector2 vBallPos = pBall->GetPosition ().xy();
                    const coreVector2 vBallDir = pBall->GetDirection().xy();

                    if(m_bBounceReal)
                    {
                        // 
                        coreVector2 vNewDir = coreVector2::Reflect(vBallDir, (vBallPos - oPaddleSphere.GetPosition().xy()).Normalize());
                        vNewDir.y = MAX(ABS(vNewDir.y), 0.35f) * pPaddle->GetDirection().y;
                        pBall->SetDirection(coreVector3(vNewDir.Normalized(), 0.0f));
                    }
                    else
                    {
                        // 
                        const coreVector2 vNewDir = coreVector2(vBallDir.x, ABS(vBallDir.y) * SIGN(pPaddle->GetDirection().y));
                        pBall->SetDirection(coreVector3(vNewDir, 0.0f));
                    }

                    // 
                    ADD_BIT(m_iBounceState, i)

                    // 
                    nEffectFunc(vBallPos + vBallDir * pBall->GetSize().x);
                }
                break;
            }
        }
    });

    // 
    if(!CONTAINS_FLAG(m_Vaus.GetStatus(), ENEMY_STATUS_DEAD))
    {
        cEnemy*   pCurEnemy = NULL;
        coreFloat fCurLenSq = FLT_MAX;

        // (# only the first ball will be active in this mission stage) 
        coreObject3D& oBall = m_aBallRaw[0];

        // 
        const coreVector2 vBallPos    = oBall.GetPosition ().xy();
        const coreVector2 vBallDir    = oBall.GetDirection().xy();
        const coreVector2 vOldBallPos = vBallPos - vBallDir * FOREGROUND_AREA * (VIRIDO_BALL_SPEED * Core::System->GetTime());

        // 
        Core::Manager::Object->TestCollision(TYPE_ENEMY, &oBall, [&](cEnemy* OUTPUT pEnemy, const coreBool bFirstHit)
        {
            // 
            if(pEnemy->GetID() != cScoutEnemy::ID) return;

            // 
            const coreFloat fNewLenSq = (pEnemy->GetPosition().xy() - vOldBallPos).LengthSq();
            if(fNewLenSq < fCurLenSq)
            {
                pCurEnemy = pEnemy;
                fCurLenSq = fNewLenSq;
            }
        });

        if(pCurEnemy)
        {
            const coreVector2 vDiff = vOldBallPos - pCurEnemy->GetPosition().xy();

            // 
            coreUintW iAxis = 0u;
            if(ABS(vDiff.x) > ABS(vDiff.y)) iAxis = (SIGN(vDiff.x) == SIGN(vBallDir.x)) ? 1u : 0u;
                                       else iAxis = (SIGN(vDiff.y) == SIGN(vBallDir.y)) ? 0u : 1u;

            // 
            coreVector2 vNewDir = vBallDir;
            vNewDir.arr[iAxis] = ABS(vBallDir.arr[iAxis]) * SIGN(vDiff.arr[iAxis]);

            // 
            oBall.SetPosition (coreVector3(vBallPos + vDiff * (3.0f * Core::System->GetTime()), 0.0f));
            oBall.SetDirection(coreVector3(vNewDir, 0.0f));

            // 
            pCurEnemy->Kill(true);

            // 
            nEffectFunc(vBallPos + vBallDir * oBall.GetSize().x);
        }
    }
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
    pBall->ChangeType(TYPE_OBJECT(2));

    // 
    auto nInitFunc = [&](coreObject3D* OUTPUT pObject, const coreFloat fAlpha)
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
    if(m_Ball.List()->empty()) return;

    // 
    ASSERT(iIndex < VIRIDO_BALLS)
    coreObject3D* pBall  = (*m_Ball     .List())[iIndex];
    coreObject3D* pTrail = (*m_BallTrail.List())[iIndex*VIRIDO_TRAILS];

    // 
    if(!pBall->GetType()) return;
    pBall->ChangeType(0);

    // 
    auto nExitFunc = [&](coreObject3D* OUTPUT pObject)
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
void cViridoMission::EnablePaddle(const coreUintW iIndex, cShip* pOwner)
{
    ASSERT(iIndex < VIRIDO_PADDLES)
    coreObject3D& oPaddle = m_aPaddle[iIndex];

    // 
    m_apOwner[iIndex] = pOwner;

    // 
    if(oPaddle.GetType()) return;
    oPaddle.ChangeType(TYPE_OBJECT(3));

    // 
    g_pGlow->BindObject(&oPaddle);
    oPaddle.SetEnabled(CORE_OBJECT_ENABLE_ALL);
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
    g_pGlow->UnbindObject(&oPaddle);
    oPaddle.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}