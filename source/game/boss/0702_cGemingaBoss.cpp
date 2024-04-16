///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 1: boss intro, ein anderer mächtiger boss entsteht (fledermaus, Dharuk, bumeränge sammeln sich und er entsteht aus einer energy-version), schreit, banner kommt, kampf geht los, und wird dann gefressen, nach biss lange in der mitte, musik ist weg, neues banner kommt
// TODO 1: mund soll im offenen zustand leicht auf und ab flattern, wie bei Chol
// pillen und pacman aussehen passen sehr gut zusammen
// TODO 1: im inneren (der secret phase) kämpft man gegen den gefressenen boss
// TODO 1: MAIN: fragment, easy, hard (decision), coop, 3 badges, boss health, medal goal, intro, outro, foreshadow
// TODO 1: death: jaw opens like Pacmans death (make sure geometry does not collide), and sides will move away from each other (like a spring when backs touch) and will explode
// TODO 1: foreshadow: other boss, flies in the background, but will be eaten by the real boss
// TODO 1: player bullets shot into mouth should be on the same height
// using teleporter was too complicated, either you had to attack boss and attack teleporter at the same time, which was just too much, or if teleporter used as "don't hit them", hitting them completely confuses the player
// TODO 1: death: rotation wird langsamer, dann fetzen zusammen, bei einschlag starke drehung die langsamer wird, bei stillstand -> pacman death
// man muss merken, dass die sichtbarkeit der bumeränge kürzer wird
// TODO 1: 1 helfer kommt aus stampfer raus


// ****************************************************************
// counter identifier
#define CONNECTED_MOUTH (0u)
#define SMASH_COUNT     (1u)


// ****************************************************************
// vector identifier
#define OLD_MOUTH_ANGLE (0u)
#define SMASH_POSITION  (1u)
#define FOLLOW_VALUES   (2u)
#define BOUNCE_FORCE    (3u)
#define FINAL_ROTATION  (4u)


// ****************************************************************
// constructor
cGemingaBoss::cGemingaBoss()noexcept
: m_fMouthAngle (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize       (coreVector3( 3.0f,3.0f,3.0f));
    this->SetOrientation(coreVector3(-1.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(8500, COLOR_SHIP_RED);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    m_InsideTop.DefineModelHigh("ship_boss_amemasu_top_inside.md3");
    m_InsideTop.DefineModelLow ("ship_boss_amemasu_top_inside.md3");
    m_InsideTop.SetSize        (this->GetSize());
    m_InsideTop.Configure      (1, COLOR_SHIP_RED);
    m_InsideTop.AddStatus      (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_HIDDEN);
    m_InsideTop.SetParent      (this);

    // 
    m_InsideBottom.DefineModelHigh("ship_boss_amemasu_bottom_inside.md3");
    m_InsideBottom.DefineModelLow ("ship_boss_amemasu_bottom_inside.md3");
    m_InsideBottom.SetSize        (this->GetSize());
    m_InsideBottom.Configure      (1, COLOR_SHIP_RED);
    m_InsideBottom.AddStatus      (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_HIDDEN);
    m_InsideBottom.SetParent      (this);

    // 
    m_Top.DefineModelHigh("ship_boss_amemasu_top_high.md3");
    m_Top.DefineModelLow ("ship_boss_amemasu_top_low.md3");
    m_Top.DefineVolume   ("ship_boss_amemasu_top_volume.md3");
    m_Top.SetSize        (this->GetSize());
    m_Top.Configure      (1, COLOR_SHIP_RED);
    m_Top.AddStatus      (ENEMY_STATUS_INVINCIBLE);
    m_Top.SetParent      (this);

    // 
    m_Bottom.DefineModelHigh("ship_boss_amemasu_bottom_high.md3");
    m_Bottom.DefineModelLow ("ship_boss_amemasu_bottom_low.md3");
    m_Bottom.DefineVolume   ("ship_boss_amemasu_bottom_volume.md3");
    m_Bottom.SetSize        (this->GetSize());
    m_Bottom.Configure      (1, COLOR_SHIP_RED);
    m_Bottom.AddStatus      (ENEMY_STATUS_INVINCIBLE);
    m_Bottom.SetParent      (this);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Tooth); ++i)
    {
        m_Tooth[i].DefineModelHigh("object_boss_amemasu_tooth.md3");
        m_Tooth[i].DefineModelLow ("object_boss_amemasu_tooth.md3");
        m_Tooth[i].SetSize        (coreVector3(2.5f,2.5f,2.5f));
        m_Tooth[i].Configure      (200, COLOR_SHIP_GREY);
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

    STATIC_ASSERT(offsetof(cGemingaBoss, m_InsideTop)    < offsetof(cGemingaBoss, m_Top))
    STATIC_ASSERT(offsetof(cGemingaBoss, m_InsideBottom) < offsetof(cGemingaBoss, m_Bottom))
}


// ****************************************************************
// 
void cGemingaBoss::__ResurrectOwn()
{
    //cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    //pMission->EnableContainer     (coreVector2(0.0f,0.0f));
    //pMission->SetContainerClamp   (false);
    //pMission->SetContainerOverdraw(false);

    // 
    m_aiCounter[CONNECTED_MOUTH] = 1;



#if 0
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
#endif
}


// ****************************************************************
// 
void cGemingaBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cGemingaBoss::__MoveOwn()
{
    cMuscusMission* pMission = d_cast<cMuscusMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    if(this->ReachedDeath())
    {
        g_pSpecialEffects->MacroDestructionDark(&m_Top);
        g_pSpecialEffects->MacroDestructionDark(&m_Bottom);
        this->Kill(false);   
    }

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);
    const cEnemySquad* pSquad2 = g_pGame->GetCurMission()->GetEnemySquad(1u);
    ASSERT(pSquad1->GetNumEnemies() == GEMINGA_ENEMIES_TELEPORT)
    ASSERT(pSquad2->GetNumEnemies() == GEMINGA_ENEMIES_LEGION)

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, (1.0f/6.85f), LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                m_Dharuk.Resurrect();
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK_REV)
        {
            //this->DefaultMoveLerp(coreVector2(pContainer->GetPosition().x / FOREGROUND_AREA.x, 2.0f), pContainer->GetPosition().xy() / FOREGROUND_AREA, fTime);
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.0f), fTime);

            if(PHASE_TIME_AFTER(0.5f)) m_fMouthAngle = SIN(1.0f*PI * (fTime * 2.0f - 1.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
                
                m_Dharuk.Kill(false);

                //pMission->DisableContainer(false);
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.75f), fTime);
            this->DefaultOrientateLerp(0.5f*PI,    2.5f*PI,                 fTime);

            if(PHASE_TIME_AFTER(0.5f)) m_fMouthAngle = SIN(0.1f*PI * (fTime * 2.0f - 1.0f));

            if(PHASE_TIME_POINT(0.2f))
                this->_StartBoss();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 4u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.4f)
        {
            //PHASE_CHANGE_TO(50u)
            //PHASE_CHANGE_TO(10u)
            PHASE_CHANGE_TO(20u)
        });
    }

    // ################################################################
    // transition to inner phase
    else if(m_iPhase == 10u)
    {
        g_pEnvironment->SetTargetSpeed(2.0f, 1.0f);     

        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK_REV)
        {

            static coreSpline2 aMove[GAME_PLAYERS]; // s_, ganzes file
            static coreFloat afAngle[GAME_PLAYERS];
                

            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_TURN);

                    aMove[i].ClearNodes();       
                    aMove[i].Reserve(2u);
                    aMove[i].AddNode(pPlayer->GetPosition().xy(), coreVector2(0.0f, 1.0f));
                    aMove[i].AddNode(this   ->GetPosition().xy(), coreVector2(0.0f,-1.0f), 8.0f);
                    aMove[i].Refine();

                    afAngle[i] = pPlayer->GetDirection().xy().Angle();
                });
            }

            //if(PHASE_TIME_BEFORE(0.1f)) m_fMouthAngle = SIN(0.1f*PI + 0.1f*PI * (fTime * 10.0f));
            if(PHASE_TIME_AFTER (0.85f)) m_fMouthAngle = LERPBR(1.0f, 0.0f, (fTime - 0.85f) / 0.15f);// SIN(0.2f*PI * (1.0f - (fTime * 10.0f - 9.0f)));

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

                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->SetPosition (coreVector3(0.0f, -2.0f * FOREGROUND_AREA.y, 0.0f));
                    pPlayer->SetDirection(coreVector3(0.0f,1.0f,0.0f));
                    pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_SHOOT);
                });
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            m_InsideTop   .AddStatus(ENEMY_STATUS_GHOST);
            m_InsideBottom.AddStatus(ENEMY_STATUS_GHOST);
            m_Top         .AddStatus(ENEMY_STATUS_GHOST);
            m_Bottom      .AddStatus(ENEMY_STATUS_GHOST);

            //g_pEnvironment->ChangeBackground(cStomachBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
            g_pEnvironment->ChangeBackground(cStomachBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_SMOOTH)
        {
            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(MIN1(fTime * 1.4f)).y;
            this->SetPosition(coreVector3(0.0f, (m_vLastPosition.y + fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreFloat fSide = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;

                const coreFloat fHeightPlayer = m_ChangePath.CalcPositionLerp(CLAMP01((fTime * 3.0f - 1.0f) / 2.0f)).y;
                pPlayer->SetPosition (coreVector3(fSide, (-3.75f - fHeightPlayer) * FOREGROUND_AREA.y, 0.0f));

                STATIC_ASSERT(GAME_PLAYERS == 2u)
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(60u)

                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_ALL);
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
            static coreFloat afAngle[GAME_PLAYERS];
        
            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_TURN);

                    afAngle[i] = pPlayer->GetDirection().xy().Angle();
                });

                m_fMouthAngle = 0.0f;

                m_InsideTop   .RemoveStatus(ENEMY_STATUS_GHOST);
                m_InsideBottom.RemoveStatus(ENEMY_STATUS_GHOST);
                m_Top         .RemoveStatus(ENEMY_STATUS_GHOST);
                m_Bottom      .RemoveStatus(ENEMY_STATUS_GHOST);
            }

            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(CLAMP01((BLENDS(fTime) * 3.0f - 1.0f) / 2.0f)).y;
            this->SetPosition(coreVector3(0.0f, (-3.0f - fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
            this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vMove = coreVector2(-pPlayer->GetPosition().x / FOREGROUND_AREA.x, 1.0f) * (TIME * 90.0f);
                const coreVector2 vPos  = pPlayer->GetPosition().xy() + vMove;
                
                const coreVector2 vDir = coreVector2::Direction(afAngle[i] + fTime * (12.0f*PI));

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));

                if(vPos.y > FOREGROUND_AREA.y * 1.3f) pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_SHOOT);
            });

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_SHOOT);
                    pPlayer->TurnIntoEnemy();
                });

                //g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,1.0f));
                g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            }

            this->DefaultMoveLerp     (coreVector2(0.0f,0.0f), coreVector2(0.0f,-0.75f), fTime);
            //this->DefaultRotateLerp   (1.0f*PI,                2.0f*PI,                  fTime);
            //this->DefaultOrientateLerp(1.5f*PI,                2.5f*PI,                  fTime);

            m_fMouthAngle = LERPH3(0.5f - 0.5f * COS(2.0f*PI * STEP(0.0f, 0.5f, fTime)), SIN(0.1f*PI), fTime);

            const coreVector2 vDir = coreVector2::Direction(fTime * (7.0f*PI));

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
                    pPlayer->RemoveStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_ALL);
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
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_LINEAR)
        {
            const coreBool bExit = (this->GetLostHealth() >= 6400);

            if(PHASE_BEGINNING) m_avVector[SMASH_POSITION].xy(bExit ? coreVector2(0.0f,0.0f) : this->NearestPlayerDual(m_aiCounter[SMASH_COUNT] % 2)->GetPosition().xy() / FOREGROUND_AREA);

            const coreVector2 vFrom = m_vLastPosition;
            const coreVector2 vTo   = coreVector2(-0.85f, m_avVector[SMASH_POSITION].y);

            const coreVector2 vPos = coreVector2(LERP(vFrom.x, vTo.x, SIN(PI * fTime)), LERP(vFrom.y, vTo.y, BLENDS(fTime))) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
            {
                if(bExit)
                {
                    if(m_avVector[SMASH_POSITION].z)
                    {
                        PHASE_CHANGE_TO(33u)
                    }
                    else
                    {
                        PHASE_RESET(0u)
                        PHASE_AGAIN

                        this->StorePosition();

                        m_avVector[SMASH_POSITION].z = 1.0f;
                    }
                }
                else
                {
                    PHASE_CHANGE_INC
                }

                const coreVector2 vPos2 = this->GetPosition().xy();

                for(coreUintW i = 0u; i < 9u; ++i)
                {
                    if((i % 2u) == (m_aiCounter[SMASH_COUNT] % 2u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) - 4.0f) * 13.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos2,  vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos2, -vDir)->ChangeSize(1.8f);
                }

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                m_aiCounter[SMASH_COUNT] += 1;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(31u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(44u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING) m_avVector[OLD_MOUTH_ANGLE].x = m_fMouthAngle;

            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(0.0f,0.8f), fTime);
            this->DefaultOrientateLerp(0.5f*PI,         3.5f*PI,                fTime);

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
    else if((m_iPhase == 41u) || (m_iPhase == 44u))
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, -0.85f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 42u) || (m_iPhase == 45u))
    {
        const coreBool bSecond = (m_iPhase == 45u);

        PHASE_CONTROL_TIMER(0u, bSecond ? 0.15f : 0.3f, LERP_SMOOTH)
        {
            const coreVector2 vPos = coreVector2(m_vLastPosition.x, bSecond ? (SIN(fTime * (2.0f*PI)) * 0.8f) : LERP(m_vLastPosition.y, -m_vLastPosition.y, fTime)) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(bSecond ? -vPos : vPos.InvertedX(), 0.0f));

            for(coreUintW i = 0u; i < 7u; ++i)
            {
                if(PHASE_POSITION_POINT(this, 0.76f * (1.0f - (2.0f/6.0f) * I_TO_F(i)) * FOREGROUND_AREA.y, y))
                {
                    if(i % 2u) s_vPositionPoint = bSecond ? -s_vPositionPoint : s_vPositionPoint.InvertedX();

                    const coreVector2 vDir = coreVector2(-SIGN(s_vPositionPoint.x), 0.0f);

                    for(coreUintW j = 0u; j < 9u; ++j)
                    {
                        const coreVector2 vPos2 = s_vPositionPoint + coreVector2((j % 2u) ? 1.0f : -1.0f, (I_TO_F(j) - 4.0f) * 0.07f * FOREGROUND_AREA.y);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, this, vPos2, vDir)->ChangeSize(1.5f);

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
    else if((m_iPhase == 43u) || (m_iPhase == 46u))
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.0f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO((m_iPhase == 46u) ? 100u : 30u)

                m_aiCounter[CONNECTED_MOUTH] = 1;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 3.0f, LERP_HERMITE3)
        {
            m_fMouthAngle = SIN(LERP(0.1f*PI, 0.0f*PI, fTime));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        static coreUintW iFollow = 0u; 
        static coreVector2 vPrev = this->GetDirection().xy();  
        static coreVector2 vCur  = this->GetDirection().xy();  
        static coreFlow fTurn = 1.0f;


        const coreBool bExit = (this->GetLostHealth() >= 1000);

        if(PHASE_BEGINNING2)
        {
            iFollow = 0u;
        }
        
        if(bExit)
        {
            if(!m_avVector[FOLLOW_VALUES].x) m_avVector[FOLLOW_VALUES].x = 0.5f;
            m_avVector[FOLLOW_VALUES].x += 0.5f * TIME;
        }

        const coreFloat fMoveSpeed = 1.0f + m_avVector[FOLLOW_VALUES].x;

        this->DefaultMoveForward(this->GetDirection().xy(), 15.0f * fMoveSpeed);
        
        if(bExit)
        {
            if(!g_pForeground->IsVisiblePoint(this->GetPosition().xy(), 1.4f))
            {
                PHASE_CHANGE_TO(70u)
            }
        }

        const coreFloat fOldSnapValue = m_avVector[FOLLOW_VALUES].y;
        m_avVector[FOLLOW_VALUES].y += 1.5f * fMoveSpeed * TIME;

        m_fMouthAngle = 0.5f - 0.5f * COS(1.0f*PI * m_avVector[FOLLOW_VALUES].y);

        if(InBetweenExt(0.5f, FMOD(fOldSnapValue, 2.0f), FMOD(m_avVector[FOLLOW_VALUES].y, 2.0f)) == 1)
        {
            const coreVector2 vPos   = this->GetPosition().xy();
            const coreFloat   fAngle = vCur.Angle();

            for(coreUintW j = 0u; j < 6u; ++j)
            {
                const coreVector2 vDir   = coreVector2::Direction(fAngle + DEG_TO_RAD((I_TO_F(j) - 2.5f) * 8.0f));
                const coreFloat   fSpeed = 1.8f - 0.3f * ABS(I_TO_F(j) - 2.5f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(2.0f);
            }
        }

        if(!bExit)
        {
            const coreVector2 vAim = this->AimAtPlayerDual(iFollow).Normalized();

            if(coreVector2::Dot(vCur, vAim) <= 0.4f)
            {
                vPrev = vCur;
                vCur  = AlongCrossNormal(vAim);
                fTurn = 0.0f;
            }
        }

        fTurn.UpdateMin(3.0f * fMoveSpeed, 1.0f);
        this->SetDirection(coreVector3(LERPS(vPrev, vCur, fTurn).Normalized(), 0.0f));

        this->__DefaultOrientation();

#if 0
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < GEMINGA_ENEMIES_TELEPORT; ++i)
            {
                pSquad1->GetEnemy(i)->Resurrect();
            }
        }

        coreSpline2 oPath1;

        oPath1.Reserve(2u);
        oPath1.AddNode(coreVector2(0.0f,1.4f), coreVector2(0.0f,-1.0f));
        oPath1.AddNode(coreVector2(0.0f,0.9f), coreVector2(0.0f,-1.0f));
        oPath1.Refine();

        const auto nTeleportFunc = [](const cEnemy* pEnemy, const coreBool bTransform)
        {
            if(pEnemy->ReachedDeath())
            {
                cPlayer* pPlayer = pEnemy->LastAttacker();
                if(!pPlayer->IsEnemyLook())
                {
                    const coreVector3 vOldPos = pPlayer->GetPosition();
                    const coreVector3 vNewPos = pEnemy ->GetPosition();

                    pPlayer->SetPosition(vNewPos);

                    g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                    {
                        if(pBullet->GetOwner() == pPlayer) pBullet->Deactivate(true);
                    });

                    if(bTransform)
                    {
                        pPlayer->TurnIntoEnemy();
                        g_pSpecialEffects->MacroExplosionColorBig(vNewPos, COLOR_ENERGY_MAGENTA);
                    }

                    const coreVector3 vPos  = vOldPos;
                    const coreVector3 vDiff = vNewPos - vPos;
                    const coreUintW   iNum  = MAX(F_TO_UI(vDiff.Length() / 1.9f), 2u);

                    for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, pPlayer->GetEnergyColor());
                }
            }
        };

        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vFactor = coreVector2(1.0f,1.0f);
            const coreVector2 vOffset = coreVector2((i % 2u) ? 0.2f : -0.6f, 0.0f);

            pEnemy->DefaultMovePath(&oPath1, vFactor, vOffset * vFactor, fLifeTime);

                 if(i < 2u) {}
            else if(i < 4u) pEnemy->Rotate90 ();
            else if(i < 6u) pEnemy->Rotate180();
            else if(i < 8u) pEnemy->Rotate270();

            nTeleportFunc(pEnemy, false);
        });
#endif
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            m_Dharuk.Resurrect();
            m_Dharuk.ChangePhase(100u);

        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        if(m_Dharuk.HasStatus(ENEMY_STATUS_DEAD))
        {
            PHASE_CHANGE_TO(20u)
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < 7u; ++i)
            {
                const coreVector2 vPos = coreVector2(0.0f, 3.0f - I_TO_F(i)) * 0.3f;

                pMission->EnablePearl(i);
                pMission->GetPearl(i)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
        }

        const coreFloat fTime = MAX0(m_fPhaseTime - 0.5f) * 1.4f;

        coreSpline2 oPath;
        oPath.Reserve(2u);
        oPath.AddNode(coreVector2(0.0f, 1.5f), coreVector2(0.0f,-1.0f));
        oPath.AddNode(coreVector2(0.0f,-1.5f), coreVector2(0.0f,-1.0f));
        oPath.Refine();

        this->DefaultMovePath(&oPath, coreVector2(1.0f,1.0f), coreVector2(0.0f,0.0f), fTime);

        this->__DefaultOrientation();

        m_fMouthAngle = 0.5f + 0.5f * SIN(4.0f*PI * fTime);

        if(fTime >= oPath.GetTotalDistance())
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 71u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < 11u; ++i)
            {
                const coreVector2 vPos = ((i < 6u) ? coreVector2(-2.0f, 3.0f - I_TO_F(i)) : coreVector2(I_TO_F(i - 6u) - 1.0f, -2.0f)) * 0.3f;

                pMission->DisablePearl(i, false);   // TODO 1:  geht nicht, strikes müssen weiter laufen
                pMission->EnablePearl(i);
                pMission->GetPearl(i)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
        }

        const coreFloat fTime = MAX0(m_fPhaseTime - 0.5f) * 1.4f;

        coreSpline2 oPath;
        oPath.Reserve(4u);
        oPath.AddNode (coreVector2(-0.6f, 1.5f), coreVector2(0.0f,-1.0f));
        oPath.AddNodes(coreVector2(-0.6f,-0.6f), coreVector2(0.0f,-1.0f), coreVector2(1.0f,0.0f));
        oPath.AddNode (coreVector2( 1.5f,-0.6f), coreVector2(1.0f, 0.0f));
        oPath.Refine();

        this->DefaultMovePath(&oPath, coreVector2(1.0f,1.0f), coreVector2(0.0f,0.0f), fTime);

        this->__DefaultOrientation();

        m_fMouthAngle = 0.5f + 0.5f * SIN(4.0f*PI * fTime);

        if(fTime >= oPath.GetTotalDistance())
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 72u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < 15u; ++i)
            {
                const coreVector2 vPos = ((i < 6u) ? coreVector2(I_TO_F(i) - 3.0f, 2.0f) : ((i < 9u) ? coreVector2(2.0f, 1.0f - I_TO_F(i - 6u)) : coreVector2(2.0f - I_TO_F(i - 9u), -2.0f))) * 0.3f;

                pMission->DisablePearl(i, false);   // TODO 1:  geht nicht, strikes müssen weiter laufen
                pMission->EnablePearl(i);
                pMission->GetPearl(i)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
        }

        const coreFloat fTime = MAX0(m_fPhaseTime - 0.5f) * 1.4f;

        coreSpline2 oPath;
        oPath.Reserve(6u);
        oPath.AddNode (coreVector2(-1.5f, 0.6f), coreVector2( 1.0f, 0.0f));
        oPath.AddNodes(coreVector2( 0.6f, 0.6f), coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
        oPath.AddNodes(coreVector2( 0.6f,-0.6f), coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
        oPath.AddNode (coreVector2(-1.5f,-0.6f), coreVector2(-1.0f, 0.0f));
        oPath.Refine();

        this->DefaultMovePath(&oPath, coreVector2(1.0f,1.0f), coreVector2(0.0f,0.0f), fTime);

        this->__DefaultOrientation();

        m_fMouthAngle = 0.5f + 0.5f * SIN(4.0f*PI * fTime);

        if(fTime >= oPath.GetTotalDistance())
            PHASE_CHANGE_TO(90u)
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_BREAK)
        {
            this->SetDirection(coreVector3(0.0f,1.0f,0.0f));

            this->DefaultMoveLerp     (coreVector2(0.0f,-1.5f), coreVector2(0.0f,-0.5f), fTime);
            this->DefaultOrientateLerp(1.5f*PI,                 3.5f*PI,                 fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        if(PHASE_BEGINNING2)
        {
            this->SetPosition (coreVector3(0.0f,-0.5f,0.0f) * FOREGROUND_AREA3);
            this->SetDirection(coreVector3(0.0f, 1.0f,0.0f));

            for(coreUintW i = 0u; i < GEMINGA_ENEMIES_LEGION; ++i)
            {
                cEnemy* pEnemy = pSquad2->GetEnemy(i);

                pEnemy->Resurrect();

                pEnemy->SetCurHealth(1);
                pEnemy->RefreshColor();

                pEnemy->SetDirection(coreVector3(coreVector2::Direction(I_TO_F(i) * GA), 0.0f));
            }
        }

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            m_fMouthAngle = 0.5f - 0.5f * COS(2.0f*PI * fTime);
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 2.0f, 0.0f)//0.25f * I_TO_F(i % 10u))

            if(STAGE_TAKEOFF)
            {
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);

                g_pSpecialEffects->MacroDestructionDark(pEnemy);
                
                pEnemy->SetPosition(this->GetPosition());
            }
                
            const coreVector2 vChildMove = coreVector2(I_TO_F(i) - 4.5f, 7.5f).Normalized();

            const coreVector2& vMove = vChildMove;
            //if(!vMove.IsNull())
            if(pEnemy->GetPosition().y < FOREGROUND_AREA.y * 1.05f)
            {
                coreVector2       vNewPos = pEnemy->GetPosition().xy() + vMove * (100.0f * TIME);
                const coreVector2 vNewDir = coreVector2::Direction(pEnemy->GetDirection().xy().Angle() + (10.0f * TIME));

                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.05f) && (vMove.x < 0.0f)) vNewPos *= -FOREGROUND_AREA.x * 1.05f * RCP(vNewPos.x);
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.05f) && (vMove.x > 0.0f)) vNewPos *=  FOREGROUND_AREA.x * 1.05f * RCP(vNewPos.x);
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.05f) && (vMove.y < 0.0f)) vNewPos *= -FOREGROUND_AREA.y * 1.05f * RCP(vNewPos.y);
                else if((vNewPos.y >  FOREGROUND_AREA.y * 1.05f) && (vMove.y > 0.0f)) vNewPos *=  FOREGROUND_AREA.y * 1.05f * RCP(vNewPos.y);

                pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));
            }
        });

        if(PHASE_MAINTIME_AFTER(2.0f))
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            m_fMouthAngle = fTime;
        });

        const coreVector2 vPoint = this->GetPosition().xy() + coreVector2(0.0f,-7.0f);

        const coreInt32 iLostHealth = this->GetLostHealth();
        if(iLostHealth < 3500)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < 24u; ++i)
                {
                    if(((i + 2u * (iTick % 3u)) % 8u) < 4u) continue;

                    const coreVector2 vTarget = coreVector2((I_TO_F(i) - 11.5f) / 11.5f * 1.1f, 1.0f);
                    const coreVector2 vRange  = vTarget - vPoint / (FOREGROUND_AREA * 1.2f);

                    const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vRange.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vRange.Length() * 0.6f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else if(iLostHealth < 4100)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < 1u; ++i)
                {
                    const coreVector2 vTarget = coreVector2((I_TO_F((iTick * 7u) % 24u) - 11.5f) / 11.5f * -1.1f, 1.0f);
                    const coreVector2 vRange  = vTarget - vPoint / (FOREGROUND_AREA * 1.2f);

                    const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vRange.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vRange.Length() * 0.6f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else if(iLostHealth < 4700)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.5f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < 24u; ++i)
                {
                    if(((i % 12u) < 6u) == (iTick % 2u)) continue;

                    const coreVector2 vTarget = coreVector2((I_TO_F(i) - 11.5f) / 11.5f * 1.1f, 1.0f);
                    const coreVector2 vRange  = vTarget - vPoint / (FOREGROUND_AREA * 1.2f);

                    const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vRange.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vRange.Length() * (0.4f + 0.05f * ((iTick % 2u) ? (I_TO_F(5u - (i % 6u))) : I_TO_F(i % 6u)));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else if(iLostHealth < 5300)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < 36u; ++i)
                {
                    if(((i % 6u) < 3u) == ((iTick / 2u) % 2u)) continue;

                    const coreFloat fLen = (coreVector2(1.2f,1.2f) - (vPoint / (FOREGROUND_AREA * 1.2f))).Length();
                    const coreVector2 vTarget = coreVector2::Direction((I_TO_F(i) - 17.5f) * 0.05f + ((iTick % 2u) ? -0.5f : 0.5f)) * fLen;
                    //const coreVector2 vRange  = vTarget - vPoint / (FOREGROUND_AREA * 1.2f);

                    const coreVector2 vPos   = vPoint + vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vTarget.Length() * 0.6f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>((iTick % 2u) ? 7 : 6, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 83u)
    {
        const coreVector2 vPoint = this->GetPosition().xy() + coreVector2(0.0f,-7.0f);

        static coreVector4 avStart[GEMINGA_ENEMIES_LEGION];
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < GEMINGA_ENEMIES_LEGION; ++i)
            {
                const cEnemy* pEnemy = pSquad2->GetEnemy(i);
                avStart[i] = coreVector4(pEnemy->GetPosition().xy(), pEnemy->GetSize().x, pEnemy->GetDirection().xy().Angle());
            }
        }

        for(coreUintW i = 0u; i < GEMINGA_ENEMIES_LEGION; ++i)
        {
            cEnemy* pEnemy = pSquad2->GetEnemy(i);

            const coreFloat fTime = BLENDBR(CLAMP01((m_fPhaseTime - I_TO_F((i * 3u) % GEMINGA_ENEMIES_LEGION) * 0.2f) * 1.0f));

            const coreVector2 vNewPos  = LERP(avStart[i].xy(), vPoint, fTime);
            const coreVector3 vNewSize = coreVector3(1.0f,1.0f,1.0f) * (avStart[i].z * (1.0f - STEPH3(0.7f, 1.0f, fTime)));
            const coreVector2 vNewDir  = coreVector2::Direction(avStart[i].w + LERP(0.0f*PI, 4.0f*PI, fTime));

            pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
            pEnemy->SetSize     (vNewSize);
            pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

            if(fTime >= 1.0f) pEnemy->Kill(false);
        }

        if(pSquad2->IsFinished())
        {
            PHASE_CHANGE_TO(10u)
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 90u)
    {
        coreVector2 vForce = m_avVector[BOUNCE_FORCE].xy();

        if(PHASE_BEGINNING2)
        {
            m_fMouthAngle = 0.0f;
            this->SetPosition(coreVector3(-1.4f,0.9f,0.0f) * FOREGROUND_AREA3);

            for(coreUintW i = 0u; i < 20u; ++i)
            {
                pMission->DisablePearl(i, false);   // TODO 1:  geht nicht, strikes müssen weiter laufen
                pMission->EnablePearl(i);
            }

            vForce = coreVector2(1.1f,0.0f);

            m_Top   .AddStatus(ENEMY_STATUS_DAMAGING);
            m_Bottom.AddStatus(ENEMY_STATUS_DAMAGING);
        }

        coreBool bFinished = true;
        for(coreUintW i = 0u; i < 20u; ++i)
        {
            coreObject3D* pPearl = pMission->GetPearl(i);
            if(!pMission->IsPearlActive(i)) continue;

            bFinished = false;

            const coreFloat fTime = MAX0(m_fPhaseTime * (0.4f*PI) - I_TO_F(i) * 0.1f);
            const coreFloat fLerp = LERPH3(1.2f, 1.0f, CLAMP01(fTime));

            const coreFloat fOffset = COS(fTime + I_TO_F((i / 2u) % 2u) * (1.0f*PI)) * fLerp;
            const coreFloat fHeight = (I_TO_F(i) - 9.5f) * 0.105f;

            const coreVector2 vPos = coreVector2(fOffset, fHeight) * FOREGROUND_AREA;

            pPearl->SetPosition(coreVector3(vPos, 0.0f));
        }

        vForce.y -= 1.0f * TIME;

        coreVector2 vNewPos = this->GetPosition().xy() + vForce * FOREGROUND_AREA * 1.5f * TIME;

             if((vNewPos.x < -FOREGROUND_AREA.x * 1.0f) && (vForce.x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x * 1.0f); vForce.x =  ABS(vForce.x);}
        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.0f) && (vForce.x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x * 1.0f); vForce.x = -ABS(vForce.x);}

        if(bFinished)
        {
            if(vNewPos.y < -FOREGROUND_AREA.y * 1.5f)
            {
                PHASE_CHANGE_INC

                m_Top   .RemoveStatus(ENEMY_STATUS_DAMAGING);
                m_Bottom.RemoveStatus(ENEMY_STATUS_DAMAGING);
            }
        }
        else if((vNewPos.y < -FOREGROUND_AREA.y * 1.0f) && (vForce.y < 0.0f))
        {
            vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.0f); vForce.y =  ABS(vForce.y);
        }

        this->SetPosition(coreVector3(vNewPos, 0.0f));

        this->DefaultRotate(m_fPhaseTime * 5.0f);

        this->__DefaultOrientation();

        m_avVector[BOUNCE_FORCE].xy(vForce);
    }

    // ################################################################
    // 
    else if(m_iPhase == 91u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(80u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 100u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 101u)
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[CONNECTED_MOUTH] = 0;
        }

        m_avVector[FINAL_ROTATION].x += 0.05f * TIME * LERP(0.5f, 1.7f, 1.0f - STEP(0.0f, 0.2f, this->GetCurHealthPct()));

        PHASE_CONTROL_TIMER(0u, 0.05f, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction((4.0f*PI) * m_avVector[FINAL_ROTATION].x + (0.5f*PI));
            const coreFloat   fLen = (STEPB(0.0f, 0.2f, fTime) - STEPBR(0.8f, 1.0f, fTime)) * FOREGROUND_AREA.x * 0.8f;
            //const coreFloat   fLen = (0.5f - 0.5f * COS(fTime * (2.0f*PI))) * FOREGROUND_AREA.x * 0.8f;
            const coreVector2 vPos = vDir * fLen;

            this   ->SetPosition(coreVector3(vPos,         0.0f));
            m_Top   .SetPosition(coreVector3(vPos,         0.0f));
            m_Bottom.SetPosition(coreVector3(vPos * -1.0f, 0.0f));

            m_Top   .SetOrientation(coreVector3(vDir, 0.0f));
            m_Bottom.SetOrientation(coreVector3(vDir, 0.0f));

            const coreMatrix3 mRota = coreMatrix4::RotationAxis((8.0f*PI) * fTime, coreVector3(vDir, 0.0f)).m123();

            m_Top   .SetDirection(coreVector3(vDir.Rotated90(), 0.0f) * mRota);
            m_Bottom.SetDirection(coreVector3(vDir.Rotated90(), 0.0f) * mRota.Transposed());

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)
                PHASE_AGAIN
            }
        });

        
        PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
        {
            const coreBool bOther = ((iTick % 80u) < 40u);
            const coreObject3D* pSource = bOther ? &m_Bottom : &m_Top;
            
            const coreFloat fBase = (pSource->GetOrientation().xy() * (bOther ? 1.0f : -1.0f)).Angle();
            
            
            const coreVector2 vPos = pSource->GetPosition().xy();
            const coreVector2 vDir = coreVector2::Direction(fBase + (0.3f*PI) * ((I_TO_F((iTick * 2u) % 5u) - 2.0f) / 2.0f));

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // ################################################################
    
    
    

    const coreMatrix2 mBulletRota    = coreMatrix3::Rotation(0.2f * TIME).m12();
    const coreMatrix2 mBulletRotaRev = mBulletRota.Transposed();
        const coreVector2 vPoint = this->GetPosition().xy() + coreVector2(0.0f,-7.0f);

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
    {
        const coreVector2 vDiff = vPoint - pBullet->GetPosition().xy();

        switch(pBullet->GetDamage())
        {
        case 5:
            break;

        case 6:
            pBullet->SetPosition(coreVector3(vPoint - vDiff * mBulletRota, 0.0f));
            pBullet->SetFlyDir  ((vDiff * mBulletRota).Normalized());
            break;

        case 7:
            pBullet->SetPosition(coreVector3(vPoint - vDiff * mBulletRotaRev, 0.0f));
            pBullet->SetFlyDir  ((vDiff * mBulletRota).Normalized());
            break;

        default:
            ASSERT(false)
            break;
        }

        if((vDiff.LengthSq() < POW2(2.0f)) || (pBullet->GetFlyTime() >= 10.0f))
            pBullet->Deactivate(true);
    });


    for(coreUintW i = 0u; i < ARRAY_SIZE(m_Tooth); ++i)
    {
        const coreFloat fOffset = m_Tooth[i].GetOrientation().x * (-1.1f - 2.0f * (1.0f-m_Tooth[i].GetCurHealthPct()));

        m_Tooth[i].SetPosition(coreVector3(fOffset * FOREGROUND_AREA.x, m_Tooth[i].GetPosition().yz()));
        m_Tooth[i].Move();
    }



    if(m_aiCounter[CONNECTED_MOUTH])
    {
        const coreFloat fFinalAngle = m_fMouthAngle * (1.0f + 0.08f * SIN(m_fLifeTime * 3.0f));
        
        // 
        const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.2f*PI * fFinalAngle, coreVector3::Cross(this->GetDirection(), this->GetOrientation()).Normalized()).m123();
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

    // 
    m_InsideTop.SetPosition   (m_Top.GetPosition   ());
    m_InsideTop.SetDirection  (m_Top.GetDirection  ());
    m_InsideTop.SetOrientation(m_Top.GetOrientation());

    // 
    m_InsideBottom.SetPosition   (m_Bottom.GetPosition   ());
    m_InsideBottom.SetDirection  (m_Bottom.GetDirection  ());
    m_InsideBottom.SetOrientation(m_Bottom.GetOrientation());

    // 
    if(coreMath::IsNear((m_Top.GetPosition() - m_Bottom.GetPosition()).LengthSq(), 0.0f))
    {
        m_InsideTop   .AddStatus(ENEMY_STATUS_INVINCIBLE);
        m_InsideBottom.AddStatus(ENEMY_STATUS_INVINCIBLE);
    }
    else
    {
        m_InsideTop   .RemoveStatus(ENEMY_STATUS_INVINCIBLE);
        m_InsideBottom.RemoveStatus(ENEMY_STATUS_INVINCIBLE);
    }
    
    

    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        for(coreUintW j = 0u; j < MUSCUS_PEARLS; ++j)
        {
            const coreObject3D* pPearl = pMission->GetPearl(j);
            if(!pMission->IsPearlActive(j)) continue;

            const coreVector2 vDiff = pPlayer->GetPosition().xy() - pPearl->GetPosition().xy();
            if(vDiff.LengthSq() < POW2(4.5f))
            {
                pMission->StrikeAttack(j, pPlayer, this);
                g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_YELLOW);
            }
        }
    });

    if((m_iPhase >= 70u) && (m_iPhase < 80u))
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        {
            const coreObject3D* pPearl = pMission->GetPearl(i);
            if(!pMission->IsPearlActive(i)) continue;

            const coreVector2 vDiff = this->GetPosition().xy() - pPearl->GetPosition().xy();
            if(vDiff.LengthSq() < POW2(4.5f))
            {
                pMission->DisablePearl(i, true);
                g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_YELLOW);

                //const coreVector2 vPos = pPearl->GetPosition().xy();
                //const coreVector2 vDir = (this->NearestPlayerDual(i % 2u)->GetPosition().xy() - vPos).Normalized();
//
                //g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5u, 2.0f, this, vPos, vDir)->ChangeSize(1.6f);
                
                
                //const coreVector2 vPos  = pPearl->GetPosition().xy();
                //const coreFloat   fBase = (this->NearestPlayerDual(i % 2u)->GetPosition().xy() - vPos).Angle();
    //
                //for(coreUintW j = 4u; j--; )
                //{
                //    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 1.5f) * 12.0f) + fBase);
    //
                //    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.4f);
                //}
                
                const coreVector2 vPos = this->GetPosition().xy();

                for(coreUintW j = 4u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 45.0f + 22.5f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos,  vDir)->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos, -vDir)->ChangeSize(1.4f);
                }
            }
        }
    }

    //g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    //{
    //    //pPlayer->SetMoveSpeed(0.5f);
    //});

    const coreUint32 iStrikeState = pMission->GetStrikeState();
    if(iStrikeState)
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        {
            if(HAS_BIT(iStrikeState, i))
            {
                const coreUint8 iDamage = 50u / GAME_PLAYERS * g_pGame->GetNumPlayers();

                this->TakeDamage(iDamage, ELEMENT_NEUTRAL, this->GetPosition().xy(), pMission->GetStrikePlayer(i));
            }
        }
    }
}