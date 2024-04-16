///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 1: nearest player dauerhaft oder jedesmal neu ?


// ****************************************************************
// counter identifier
#define CONNECTED_MOUTH (0u)


// ****************************************************************
// vector identifier
#define OLD_MOUTH_ANGLE (0u)
#define SMASH_POSITION  (1u)


// ****************************************************************
// constructor
cAmemasuBoss::cAmemasuBoss()noexcept
: m_fMouthAngle (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize       (coreVector3(0.0f,0.0f,0.0f));
    this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(3000, 0u, COLOR_SHIP_CYAN);

    // 
    m_Top.DefineModelHigh("ship_boss_amemasu_top_high.md3");
    m_Top.DefineModelLow ("ship_boss_amemasu_top_low.md3");
    m_Top.SetSize        (coreVector3(3.0f,3.0f,3.0f));
    m_Top.Configure      (1, 0u, COLOR_SHIP_CYAN);
    m_Top.SetParent      (this);

    // 
    m_Bottom.DefineModelHigh("ship_boss_amemasu_bottom_high.md3");
    m_Bottom.DefineModelLow ("ship_boss_amemasu_bottom_low.md3");
    m_Bottom.SetSize        (coreVector3(3.0f,3.0f,3.0f));
    m_Bottom.Configure      (1, 0u, COLOR_SHIP_CYAN);
    m_Bottom.SetParent      (this);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Tooth); ++i)
    {
        m_Tooth[i].DefineModelHigh("object_boss_amemasu_tooth.md3");
        m_Tooth[i].DefineModelLow ("object_boss_amemasu_tooth.md3");
        m_Tooth[i].SetSize        (coreVector3(2.5f,2.5f,2.5f));
        m_Tooth[i].Configure      (200, 0u, COLOR_SHIP_GREY);
        m_Tooth[i].AddStatus      (ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_ChangePath.Reserve(3u);
    m_ChangePath.AddNode(coreVector2(0.0f, 0.0f),  coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f, 0.01f), coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f,-3.0f),  coreVector2(0.0f,-1.0f));
    m_ChangePath.Refine();

    // 
    m_apStomachTex[0] = Core::Manager::Resource->Get<coreTexture>("environment_mars_diff.png");
    m_apStomachTex[1] = Core::Manager::Resource->Get<coreTexture>("environment_rock_diff.png");
    m_apStomachTex[2] = Core::Manager::Resource->Get<coreTexture>("environment_dust_diff.png");
    m_apStomachTex[3] = Core::Manager::Resource->Get<coreTexture>("environment_earth_diff.png");

    // ENERGIE ATEM
}


// ****************************************************************
// 
void cAmemasuBoss::__ResurrectOwn()
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableContainer     (coreVector2(0.0f,0.0f));
    pMission->SetContainerClamp   (false);
    pMission->SetContainerOverdraw(false);

    // 
    m_aiCounter[CONNECTED_MOUTH] = 1;




    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Tooth); ++i)
    {
        m_Tooth[i].Resurrect();

        
    }
      
    m_Tooth[0].SetPosition   (coreVector3(0.0f, 0.6f * FOREGROUND_AREA.y, 0.0f));
    m_Tooth[0].SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    m_Tooth[0].SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    m_Tooth[1].SetPosition   (coreVector3(0.0f, -0.2f * FOREGROUND_AREA.y, 0.0f));
    m_Tooth[1].SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    m_Tooth[1].SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    m_Tooth[2].SetPosition   (coreVector3(0.0f, 0.2f * FOREGROUND_AREA.y, 0.0f));
    m_Tooth[2].SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    m_Tooth[2].SetOrientation(coreVector3(-1.0f,0.0f,0.0f));

    m_Tooth[3].SetPosition   (coreVector3(0.0f, -0.6f * FOREGROUND_AREA.y, 0.0f));
    m_Tooth[3].SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    m_Tooth[3].SetOrientation(coreVector3(-1.0f,0.0f,0.0f));
}


// ****************************************************************
// 
void cAmemasuBoss::__KillOwn(const coreBool bAnimated)
{
}


// ****************************************************************
// 
void cAmemasuBoss::__MoveOwn()
{
    cNevoMission* pMission   = d_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

    // 
    this->_UpdateBoss();

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(coreVector2(pContainer->GetPosition().x / FOREGROUND_AREA.x, 2.0f), pContainer->GetPosition().xy() / FOREGROUND_AREA, fTime);

            if(PHASE_TIME_AFTER(0.5f)) m_fMouthAngle = SIN(1.0f*PI * (fTime * 2.0f - 1.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pMission->DisableContainer(false);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.75f), fTime);

            if(PHASE_TIME_AFTER(0.5f)) m_fMouthAngle = SIN(0.1f*PI * (fTime * 2.0f - 1.0f));

            if(PHASE_TIME_POINT(0.85f))
                this->_StartBoss();

            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                PHASE_CHANGE_TO(10u)       
        });
    }

    // ################################################################
    // transition to inner phase
    else if(m_iPhase == 10u)
    {
        g_pEnvironment->SetTargetSpeed(0.0f, 1.0f);    

        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK_REV)
        {

            static coreSpline2 aMove[GAME_PLAYERS]; // s_, ganzes file
            static coreFloat afAngle[GAME_PLAYERS];
                

            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i) 
                {
                    pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_TURN);
                    pPlayer->ChangeType(0);

                    aMove[i].Reserve(2u);
                    aMove[i].AddNode(pPlayer->GetPosition().xy(), coreVector2(0.0f,-1.0f));
                    aMove[i].AddNode(this   ->GetPosition().xy(), coreVector2(0.0f, 8.0f));
                    aMove[i].Refine();

                    afAngle[i] = pPlayer->GetDirection().xy().Angle();
                });
            }

            if(PHASE_TIME_BEFORE(0.1f)) m_fMouthAngle = SIN(0.1f*PI + 0.1f*PI * (fTime * 10.0f));
            if(PHASE_TIME_AFTER (0.9f)) m_fMouthAngle = SIN(0.2f*PI * (1.0f - (fTime * 10.0f - 9.0f)));

            const coreFloat fMoveTime = MIN(fTime / 0.9f, 1.0f);

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vDir = coreVector2::Direction(afAngle[i] + fTime * (4.0f*PI));

                pPlayer->SetPosition (coreVector3(aMove[i].CalcPositionLerp(fMoveTime), 0.0f));


                //const coreVector2 vDiff = this->GetPosition().xy() - pPlayer->GetPosition().xy();
                //const coreVector2 vMove = vDiff.Normalized() * (TIME * fTime * 100.0f * SmoothTowards(vDiff.Length(), 5.0f));
                //const coreVector2 vPos  = pPlayer->GetPosition().xy() + vMove;
                //
                //pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pMission->EnableContainer     (coreVector2(0.0f, 2.0f * FOREGROUND_AREA.y));
                pMission->SetContainerClamp   (false);
                pMission->SetContainerOverdraw(false);

                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->SetPosition(coreVector3(0.0f, -2.0f * FOREGROUND_AREA.y, 0.0f));
                });

                g_pEnvironment->ChangeBackground(cStomachBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_SMOOTH)
        {
            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(CLAMP(fTime*1.4f, 0.0f, 1.0f)).y;
            this->SetPosition(coreVector3(0.0f, (m_vLastPosition.y + fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            const coreFloat fHeightContainer = m_ChangePath.CalcPositionLerp(CLAMP((fTime*3.0f - 1.0f) / 2.0f, 0.0f, 1.0f)).y;
            pContainer->SetPosition(coreVector3(0.0f, -(-3.75f - fHeightContainer) * FOREGROUND_AREA.y, 0.0f));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreFloat fSide = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;

                const coreFloat fHeightPlayer = m_ChangePath.CalcPositionLerp(CLAMP((fTime*3.0f - 1.0f) / 2.0f, 0.0f, 1.0f)).y;
                pPlayer->SetPosition(coreVector3(fSide, (-3.75f - fHeightPlayer) * FOREGROUND_AREA.y, 0.0f));

                STATIC_ASSERT(GAME_PLAYERS == 2u)
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(60u)

                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);
                    pPlayer->ChangeType(TYPE_PLAYER);
                });
            }
        });
    }

    // ################################################################
    // transition back to outer phase
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_TURN);
                    pPlayer->ChangeType(0);
                });

                m_fMouthAngle = 0.0f;

                g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,1.0f));
            }

            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(CLAMP((LERPS(0.0f, 1.0f, fTime)*3.0f - 1.0f) / 2.0f, 0.0f, 1.0f)).y;
            this->SetPosition(coreVector3(0.0f, (-3.0f - fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            const coreVector2 vDir = coreVector2::Direction(fTime * (10.0f*PI));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vMove = coreVector2(-pPlayer->GetPosition().x, 1.0f) * (TIME * 90.0f);
                const coreVector2 vPos  = pPlayer->GetPosition().xy() + vMove;

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_BREAK)
        {
            this->DefaultMoveLerp     (coreVector2(0.0f,0.0f), coreVector2(0.0f,-0.75f), fTime);
            this->DefaultRotateLerp   (1.0f*PI,                2.0f*PI,                  fTime);
            this->DefaultOrientateLerp(1.5f*PI,                2.5f*PI,                  fTime);

            m_fMouthAngle = SIN(0.1f*PI * fTime);

            const coreVector2 vDir = coreVector2::Direction(fTime * (9.0f*PI));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vPos = this->GetPosition().xy().InvertedY();

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(40u)

                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);
                    pPlayer->ChangeType(TYPE_PLAYER);
                });
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 2.0f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) m_avVector[OLD_MOUTH_ANGLE].x = m_fMouthAngle;

            m_fMouthAngle = LERP(m_avVector[OLD_MOUTH_ANGLE].x, 0.0f, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[CONNECTED_MOUTH] = 0;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u || m_iPhase == 33u || m_iPhase == 35u || m_iPhase == 37u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) m_avVector[SMASH_POSITION].y = this->NearestPlayerDual(0u)->GetPosition().y / FOREGROUND_AREA.y;

            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.85f, SIN(PI * fTime)), LERPS(m_vLastPosition.y, m_avVector[SMASH_POSITION].y, fTime)) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 8.0f - 12.0f + ((m_iPhase == 34u || m_iPhase == 38u) ? 45.0f : 0.0f)));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, this->GetPosition().xy(),  vDir)            ->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, this->GetPosition().xy(), -vDir)            ->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, this->GetPosition().xy(),  vDir.Rotated90())->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, this->GetPosition().xy(), -vDir.Rotated90())->ChangeSize(1.4f);
                }

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u || m_iPhase == 34u || m_iPhase == 36u || m_iPhase == 38u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 39u)
    {
        PHASE_CONTROL_TIMER(0u, 0.9f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING) m_aiCounter[CONNECTED_MOUTH] = 1;

            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(0.0f,0.75f), fTime);
            this->DefaultOrientateLerp(1.5f*PI,         3.5f*PI,                 fTime);

            m_fMouthAngle = LERP(0.0f, SIN(0.1f*PI), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING) m_avVector[OLD_MOUTH_ANGLE].x = m_fMouthAngle;

            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(0.0f,0.75f), fTime);
            this->DefaultOrientateLerp(0.5f*PI,         3.5f*PI,                 fTime);//2.5f*PI,                 fTime);

            m_fMouthAngle = LERP(m_avVector[OLD_MOUTH_ANGLE].x, 0.0f, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[CONNECTED_MOUTH] = 0;

                for(coreUintW i = 20u; i--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

                    // 
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, this->GetPosition().xy(),  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, this->GetPosition().xy(), -vDir)->ChangeSize(1.3f);
                    // TODO 1: lower blue balls get overlapped   
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.85f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 42u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            const coreVector2 vPos = coreVector2(m_vLastPosition.x, LERP(m_vLastPosition.y, -m_vLastPosition.y, fTime)) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            for(coreUintW i = 0u; i < 30u; ++i)
            {
                if(PHASE_POSITION_POINT(this, (0.7f - 0.2f * I_TO_F(i)) * FOREGROUND_AREA.y, y))
                {
                    if(i & 0x01u) s_vPositionPoint = s_vPositionPoint.InvertedX();

                    const coreVector2 vDir = coreVector2(-SIGN(s_vPositionPoint.x), 0.0f);

                    for(coreUintW j = 0u; j < 5u; ++j)
                    {
                        const coreVector2 vPos2 = s_vPositionPoint + coreVector2(0.0f, I_TO_F(j-2u) * 0.05f * FOREGROUND_AREA.y);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, this, vPos2, vDir);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos2, 0.0f), 5.0f, 1u, COLOR_ENERGY_ORANGE);
                    }
                }
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 43u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.0f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(50u)   

                m_aiCounter[CONNECTED_MOUTH] = 1;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        static coreUintW iFollow = 0u; 
        static coreVector2 vPrev = this->GetDirection().xy();  
        static coreVector2 vCur  = this->GetDirection().xy();  
        static coreFlow fTurn = 1.0f;

        PHASE_CONTROL_TIMER(0u, 0.05f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                iFollow = 0u;
            }

            this->DefaultMoveForward(this->GetDirection().xy(), 15.0f);

            constexpr coreFloat fIterations = 30.0f;
            m_fMouthAngle = 0.5f + 0.5f * SIN(1.0f*PI * (fTime * fIterations));

            for(coreUintW i = 0u; i < F_TO_UI(fIterations); ++i) 
            {
                if(PHASE_TIME_POINT(I_TO_F(i) * (2.0f / fIterations)))
                {
                    const coreFloat fAngle = vCur.Angle();
                    for(coreUintW j = 0u; j < 6u; ++j)
                    {
                        const coreVector2 vDir = coreVector2::Direction(fAngle + DEG_TO_RAD(I_TO_F(j) * 8.0f - 20.0f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.8f - 0.3f * ABS(I_TO_F(j) - 2.5f), this, this->GetPosition().xy(), vDir)->ChangeSize(1.4f);
                    }
                }
            }

            const coreVector2 vAim = this->AimAtPlayerDual(iFollow).Normalized();

            if(coreVector2::Dot(vCur, vAim) <= (1.0f / SQRT2))// && coreMath::IsNear(m_fMouthAngle, 0.0f))
            {
                vPrev = vCur;
                vCur  = AlongCrossNormal(vAim);
                fTurn = 0.0f;
            }

            fTurn.Update(3.0f);
            this->SetDirection  (coreVector3(LERPS(vPrev, vCur, MIN(fTurn, 1.0f)).Normalized(),  0.0f));
            this->SetOrientation(coreVector3(this->GetDirection().xy().Rotated90(), 0.0f));
            //this->SetOrientation(coreVector3(0.0f, this->GetDirection().yx().Rotated90()));

            //this->SetDirection  (coreVector3(vAim,             0.0f));
            //this->SetOrientation(coreVector3(vAim.Rotated90(), 0.0f));


            PHASE_CONTROL_TICKER(1u, 0u, 12.0f, LERP_LINEAR)
            {
                if(iTick & 0x04u) return;
                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, this, this->GetPosition().xy(), vAim)->ChangeSize(1.3f);
            });


            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(50u)   
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {




        PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = coreVector2(0.0f/*FMOD(I_TO_F(iTick) * 0.7f, 2.0f) - 1.0f*/, 1.2f) * FOREGROUND_AREA;
            const coreVector2 vDir = (g_pGame->FindPlayerSide(vPos)->GetPosition().xy() - vPos).Normalized(); // TODO 1: FindPlayer 

            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.3f, this, vPos, vDir)->ChangeSize(1.2f);
        });
    }

    // ################################################################
    // ################################################################


    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Tooth); ++i)
    {
        const coreFloat fOffset = m_Tooth[i].GetOrientation().x * (-1.1f - 2.0f * (1.0f-m_Tooth[i].GetCurHealthPct()));

        m_Tooth[i].SetPosition(coreVector3(fOffset * FOREGROUND_AREA.x, m_Tooth[i].GetPosition().yz()));
        m_Tooth[i].Move();
    }



    if(m_aiCounter[CONNECTED_MOUTH])
    {
        // 
        const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.2f*PI * m_fMouthAngle, coreVector3::Cross(this->GetDirection(), this->GetOrientation()).Normalized()).m123();
        const coreVector3 vDir1 = this->GetDirection() * mRota;
        const coreVector3 vDir2 = this->GetDirection() * mRota.Transposed();
        const coreVector3 vPos1 = this->GetPosition () + (vDir1 - this->GetDirection()) * (m_Top   .GetSize().x * 3.5f);
        const coreVector3 vPos2 = this->GetPosition () + (vDir2 - this->GetDirection()) * (m_Bottom.GetSize().x * 3.5f);

        // 
        m_Top.SetPosition   (vPos1);
        m_Top.SetDirection  (vDir1);
        m_Top.SetOrientation(this->GetOrientation());

        // 
        m_Bottom.SetPosition   (vPos2);
        m_Bottom.SetDirection  (vDir2);
        m_Bottom.SetOrientation(this->GetOrientation());
    }
}


// spieler werden verschluckt, schiff-objekte werden versteckt
// boss wird nach unten aus dem bildschirm interpoliert
// dabei wird background geändert (transition typ ausprobieren)
// spieler fliegen von unten rein in neuen background (andere dinge von oben)
// umgekehrt beim ausspucken
// sobald boss nicht mehr sichtbar ist spieler einfliegen um uebergang kurz zu halten
// schießt dann torpedos von unten nach oben