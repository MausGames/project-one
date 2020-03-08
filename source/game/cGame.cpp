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
cGame::cGame(const coreUint8 iDifficulty, const coreBool bCoop, const coreInt32* piMissionList, const coreUintW iNumMissions)noexcept
: m_BulletManagerPlayer (TYPE_BULLET_PLAYER)
, m_BulletManagerEnemy  (TYPE_BULLET_ENEMY)
, m_Interface           (bCoop ? GAME_PLAYERS : 1u)
, m_pRepairEnemy        (NULL)
, m_piMissionList       (piMissionList)
, m_iNumMissions        (iNumMissions)
, m_pCurMission         (NULL)
, m_iCurMissionIndex    (coreUintW(-1))
, m_fTimeInOut          (0.0f)
, m_iContinues          (GAME_CONTINUES)
, m_fPacifistDamage     (0.0f)
, m_bPacifist           (false)
, m_iDepthLevel         (0u)
, m_iOutroType          (0u)
, m_iStatus             (0u)
, m_iDifficulty         (iDifficulty)
, m_bCoop               (bCoop)
{
    ASSERT(m_piMissionList && (m_iNumMissions <= MISSIONS))

#if defined(_P1_DEBUG_RANDOM_)

    // 
    if(!m_bCoop && (CORE_RAND_RUNTIME & 0x01u))
        m_aPlayer[0].Configure(PLAYER_SHIP_DEF, COLOR_SHIP_BLUE);
    else

#endif

    // configure first player
    m_aPlayer[0].Configure  (PLAYER_SHIP_ATK, COLOR_SHIP_RED);
    m_aPlayer[0].EquipWeapon(0u, cRayWeapon::ID);
    if(!m_iDifficulty) m_aPlayer[0].GiveShield();

    if(m_bCoop)
    {
        // configure second player
        m_aPlayer[1].Configure  (PLAYER_SHIP_DEF, COLOR_SHIP_BLUE);
        m_aPlayer[1].EquipWeapon(0u, cRayWeapon::ID);
        if(!m_iDifficulty) m_aPlayer[1].GiveShield();

        // 
        m_aPlayer[0].SetInput(&g_aGameInput[0]);
        m_aPlayer[1].SetInput(&g_aGameInput[1]);

        // 
        m_aPlayer[0].SetArea(coreVector4(-FOREGROUND_AREA, FOREGROUND_AREA * coreVector2(-0.1f,1.0f)));
        m_aPlayer[1].SetArea(coreVector4(-FOREGROUND_AREA * coreVector2(-0.1f,1.0f), FOREGROUND_AREA));

        // 
        g_pPostProcessing->SetSplitScreen(true);

        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }

    // 
    m_Interface.UpdateLayout();
    m_Interface.UpdateEnabled();

    // load first mission
    m_pCurMission = new cNoMission();
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    // 
    this->__ClearAll(!DEFINED(_CORE_DEBUG_));   // # prevent assertions when force-quitting

    // delete last mission
    SAFE_DELETE(m_pCurMission)

    // 
    g_pWindscreen->ClearAdds(true);

    // 
    g_pPostProcessing->SetSaturationAll(1.0f);
    g_pPostProcessing->SetSplitScreen  (false);

    // 
    g_pEnvironment->SetTargetDirection(ENVIRONMENT_DEFAULT_DIRECTION);
    g_pEnvironment->SetTargetSide     (ENVIRONMENT_DEFAULT_SIDE);
    g_pEnvironment->SetTargetSpeed    (ENVIRONMENT_DEFAULT_SPEED);
}


// ****************************************************************
// render the game
void cGame::Render()
{
    __DEPTH_GROUP_SHIP   // # 1
    {
        // render all players
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            m_aPlayer[i].Render();

        // render all enemies
        m_EnemyManager.Render();
    }

    __DEPTH_GROUP_UNDER
    {
        DEPTH_PUSH_DOUBLE

        glDepthMask(false);
        {
            // 
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                m_aPlayer[i].RenderBefore();
        }
        glDepthMask(true);

        // render low-priority bullet manager
        m_BulletManagerPlayer.Render();

        // render underlying objects
        m_EnemyManager.RenderUnder();
        m_pCurMission->RenderUnder();
    }

    __DEPTH_GROUP_SHIP   // # 2
    {
        // apply deferred outline-layer
        g_pOutline->Apply();
    }

    __DEPTH_GROUP_OVER
    {
        DEPTH_PUSH

        // 
        m_ChromaManager.Render();
        m_ItemManager  .Render();
        m_ShieldManager.Render();

        // render overlying objects
        m_EnemyManager.RenderOver();
        m_pCurMission->RenderOver();
    }

    __DEPTH_GROUP_TOP
    {
        DEPTH_PUSH_DOUBLE

        // render special-effects
        g_pSpecialEffects->Render();

        // render high-priority bullet manager
        m_BulletManagerEnemy.Render();

        // render top objects
        m_EnemyManager.RenderTop();
        m_pCurMission->RenderTop();

        glDisable(GL_DEPTH_TEST);
        {
            // 
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                m_aPlayer[i].RenderAfter();
        }
        glEnable(GL_DEPTH_TEST);
    }

    __DEPTH_RESET
}


// ****************************************************************
// move the game
void cGame::Move()
{
    // handle intro and outro animation
    if(!this->__HandleIntro()) return;
    if(!this->__HandleOutro()) return;

    // 
    m_TimeTable.Update();

    // 
    this->__HandlePacifist();

    // move the mission
    m_pCurMission->MoveBefore();
    {
        // move all players
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            m_aPlayer[i].Move();

        // move all enemies
        m_EnemyManager.Move();
    }
    m_pCurMission->MoveAfter();

    // move the bullet managers
    m_BulletManagerPlayer.Move();
    m_BulletManagerEnemy .Move();

    // 
    m_ChromaManager.Move();

    // 
    m_ItemManager.Move();

    // 
    m_ShieldManager.Move();

    // handle default object collisions
    this->__HandleCollisions();

    // 
    this->__HandleDefeat();

    // move all overlay objects
    m_CombatText.Move();
    m_Interface .Move();
}


// ****************************************************************
// render the overlay separately
void cGame::RenderOverlay()
{
    // render combat text and interface
    m_CombatText.Render();
    m_Interface .Render();
}


// ****************************************************************
// load new active mission
void cGame::LoadMissionID(const coreInt32 iID)
{
    if(m_pCurMission) if(m_pCurMission->GetID() == iID) return;

    // 
    this->__ClearAll(false);

    // delete possible old mission
    SAFE_DELETE(m_pCurMission)

    // create new mission
    switch(iID)
    {
    default: ASSERT(false)
    case cNoMission     ::ID: m_pCurMission = new cNoMission     (); break;
    case cViridoMission ::ID: m_pCurMission = new cViridoMission (); break;
    case cNevoMission   ::ID: m_pCurMission = new cNevoMission   (); break;
    case cHarenaMission ::ID: m_pCurMission = new cHarenaMission (); break;
    case cRutilusMission::ID: m_pCurMission = new cRutilusMission(); break;
    case cGeluMission   ::ID: m_pCurMission = new cGeluMission   (); break;
    case cCalorMission  ::ID: m_pCurMission = new cCalorMission  (); break;
    case cMuscusMission ::ID: m_pCurMission = new cMuscusMission (); break;
    case cIntroMission  ::ID: m_pCurMission = new cIntroMission  (); break;
    }

    // 
    m_iCurMissionIndex = std::find(m_piMissionList, m_piMissionList + m_iNumMissions, iID) - m_piMissionList;
    ASSERT(m_iCurMissionIndex < m_iNumMissions)

    if(iID != cNoMission::ID)
    {
        // setup the mission
        m_pCurMission->Setup();

        // set initial status
        m_iStatus = GAME_STATUS_LOADING;
    }

    Core::Log->Info("Mission (%s) created", m_pCurMission->GetName());
}


// ****************************************************************
// 
void cGame::LoadMissionIndex(const coreUintW iIndex)
{
    ASSERT(iIndex < m_iNumMissions)
    this->LoadMissionID(m_piMissionList[iIndex]);
}


// ****************************************************************
// 
void cGame::LoadNextMission()
{
    // 
    this->LoadMissionIndex(m_iCurMissionIndex + 1u);

    // 
    if(m_pCurMission->GetID() == cNoMission::ID)
    {
        ASSERT(CONTAINS_FLAG(m_iStatus, GAME_STATUS_OUTRO))

        // 
        REMOVE_FLAG(m_iStatus, GAME_STATUS_OUTRO)
        ADD_FLAG   (m_iStatus, GAME_STATUS_FINISHED)
    }
}


// ****************************************************************
// restart currently active mission
void cGame::RestartMission()
{
    // hold old mission (to keep resources valid)
    cMission* pOldMission = m_pCurMission;
    m_pCurMission = NULL;

    // 
    this->LoadMissionID(pOldMission->GetID());
    SAFE_DELETE(pOldMission)
}


// ****************************************************************
// 
void cGame::StartIntro()
{
    ASSERT(!CONTAINS_FLAG(m_iStatus, GAME_STATUS_PLAY))
    ASSERT(!CONTAINS_FLAG(m_iStatus, GAME_STATUS_OUTRO))

    // 
    ADD_FLAG(m_iStatus, GAME_STATUS_INTRO)

    // 
    m_fTimeInOut = -GAME_INTRO_DELAY;

    for(coreUintW i = 0u, ie = (m_bCoop ? GAME_PLAYERS : 1u); i < ie; ++i)
    {
        // 
        m_aPlayer[i].Kill(false);
        m_aPlayer[i].Resurrect();
        m_aPlayer[i].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);

        // 
        const coreFloat fSide = m_bCoop ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS-1u))) : 0.0f;
        m_aPlayer[i].SetPosition(coreVector3(fSide, -140.0f, 0.0f));
    }
}


// ****************************************************************
// 
void cGame::StartOutro(const coreUint8 iType)
{
    ASSERT(!CONTAINS_FLAG(m_iStatus, GAME_STATUS_INTRO))

    // 
    REMOVE_FLAG(m_iStatus, GAME_STATUS_PLAY)
    ADD_FLAG   (m_iStatus, GAME_STATUS_OUTRO)

    // 
    m_fTimeInOut = 0.0f;
    m_iOutroType = iType;

    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);

    // 
    m_Interface.SetVisible(false);

    // 
    g_pReplay->ApplyKeyFrame(REPLAY_KEYFRAME_MISSION_END(m_pCurMission->GetID()));
}


// ****************************************************************
// 
void cGame::UseContinue()
{
    ASSERT(CONTAINS_FLAG(m_iStatus, GAME_STATUS_DEFEATED))

    // 
    REMOVE_FLAG(m_iStatus, GAME_STATUS_DEFEATED)
    this->StartIntro();

    // 
    for(coreUintW i = 0u, ie = (m_bCoop ? GAME_PLAYERS : 1u); i < ie; ++i)
        m_aPlayer[i].StartFeeling(PLAYER_FEEL_TIME_CONTINUE, 2u);

    // 
    ASSERT(m_iContinues)
    m_iContinues -= 1u;
}


// ****************************************************************
// 
void cGame::ActivatePacifist()
{
    // 
    ASSERT(!m_bPacifist)
    m_bPacifist = true;

    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].AddStatus(PLAYER_STATUS_PACIFIST);
}


// ****************************************************************
// 
void cGame::ChangeDepthLevel(const coreUint8 iLevelNear, const coreUint8 iLevelFar)const
{
    // 
    ASSERT(iLevelNear < iLevelFar)
    glDepthRange(0.05f * I_TO_F(iLevelNear), 0.05f * I_TO_F(iLevelFar));
}

void cGame::PushDepthLevel(const coreUint8 iLevels)
{
    // 
    ASSERT(m_iDepthLevel >= (m_iDepthDebug & ~BIT(7u)) + iLevels)
    m_iDepthLevel -= iLevels;

    // 
    this->ChangeDepthLevel(m_iDepthLevel, m_iDepthLevel + iLevels);
}

void cGame::PushDepthLevelShip()
{
    // 
    ASSERT(m_iDepthDebug & BIT(7u))
    __DEPTH_GROUP_SHIP
}


// ****************************************************************
// 
RETURN_NONNULL cPlayer* cGame::FindPlayer(const coreVector2& vPosition)
{
    // 
    if(!m_bCoop) return &m_aPlayer[0];

    // 
    cPlayer*  pPlayer = &m_aPlayer[0];
    coreFloat fLenSq  = FLT_MAX;

    // 
    this->ForEachPlayer([&](cPlayer* OUTPUT pCurPlayer, const coreUintW i)
    {
        // 
        const coreFloat fCurLenSq = (pCurPlayer->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq < fLenSq)
        {
            // 
            pPlayer = pCurPlayer;
            fLenSq  = fCurLenSq;
        }
    });

    ASSERT(pPlayer)
    return pPlayer;
}


// ****************************************************************
// 
coreUint8 cGame::CalcMedal(const coreFloat fTime, const coreUint32 iDamageTaken, const coreFloat* pfMedalGoal)
{
    ASSERT(pfMedalGoal && (pfMedalGoal[0] < pfMedalGoal[1]) && (pfMedalGoal[1] < pfMedalGoal[2]) && (pfMedalGoal[2] < pfMedalGoal[3]))

    // 
    coreUint8 iMedal;
         if(fTime <= pfMedalGoal[0]) iMedal = MEDAL_DARK;
    else if(fTime <= pfMedalGoal[1]) iMedal = MEDAL_PLATINUM;
    else if(fTime <= pfMedalGoal[2]) iMedal = MEDAL_GOLD;
    else if(fTime <= pfMedalGoal[3]) iMedal = MEDAL_SILVER;
    else                             iMedal = MEDAL_BRONZE;

    // 
    iMedal -= MIN(iDamageTaken, coreUint32(iMedal - MEDAL_BRONZE));

    return iMedal;
}


// ****************************************************************
// 
coreUint32 cGame::CalcBonusTime(const coreFloat fTime)
{
    // 
    return F_TO_UI(LERP(20000.0f, 100.0f, MIN(fTime * (1.0f/60.0f), 1.0f)));
}


// ****************************************************************
// 
coreUint32 cGame::CalcBonusMedal(const coreUint8 iMedal)
{
    // 
    switch(iMedal)
    {
    default: ASSERT(false)
    case MEDAL_DARK:     return 10000u;
    case MEDAL_PLATINUM: return  5000u;
    case MEDAL_GOLD:     return  3000u;
    case MEDAL_SILVER:   return  2000u;
    case MEDAL_BRONZE:   return  1000u;
    case MEDAL_NONE:     return     0u;
    }
}


// ****************************************************************
// 
coreUint32 cGame::CalcBonusSurvive(const coreUint32 iDamageTaken, const coreBool bWasDead)
{
    // 
    if(!bWasDead)
    {
        // 
        switch(iDamageTaken)
        {
        case 0u: return 200000u;
        case 1u: return 100000u;
        case 2u: return  50000u;
        case 3u: return  30000u;
        case 4u: return  20000u;
        default: return  10000u - MIN(50u * (iDamageTaken - LIVES), 9999u);
        }

        STATIC_ASSERT(LIVES == 5u)
    }

    return 0u;
}


// ****************************************************************
// handle intro animation
coreBool cGame::__HandleIntro()
{
    if(CONTAINS_FLAG(m_iStatus, GAME_STATUS_LOADING))
    {
        // do not start while game resources are still loading
        if(Core::Manager::Resource->IsLoading()) return false;
        REMOVE_FLAG(m_iStatus, GAME_STATUS_LOADING)
    }

    if(CONTAINS_FLAG(m_iStatus, GAME_STATUS_INTRO))
    {
        // 
        m_fTimeInOut.Update(1.0f);

        if(m_fTimeInOut >= GAME_INTRO_DURATION)
        {
            ASSERT(!CONTAINS_FLAG(m_iStatus, GAME_STATUS_OUTRO))

            // end intro and start actual game
            REMOVE_FLAG(m_iStatus, GAME_STATUS_INTRO)
            ADD_FLAG   (m_iStatus, GAME_STATUS_PLAY)

            // re-enable player controls
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                m_aPlayer[i].RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);

            // 
            m_Interface.SetVisible(true);

            // 
            g_pReplay->ApplyKeyFrame(REPLAY_KEYFRAME_MISSION_START(m_pCurMission->GetID()));
        }
        else
        {
            // create spline for intro animation (YZ)
            static coreSpline2 s_Spline;
            if(!s_Spline.GetNumNodes())
            {
                s_Spline.Reserve(3u);
                s_Spline.AddNode(coreVector2(-140.0f,-10.0f), coreVector2( 1.0f, 0.0f));
                s_Spline.AddNode(coreVector2(  10.0f, 10.0f), coreVector2(-1.0f,-1.0f).Normalized());
                s_Spline.AddNode(coreVector2( -30.0f,  0.0f), coreVector2(-1.0f, 0.0f));
                s_Spline.Refine();
            }

            this->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                // calculate new player position and rotation
                const coreFloat   fTime = CLAMP((i ? 0.0f : 0.08f) + m_fTimeInOut / GAME_INTRO_DURATION, 0.0f, 1.0f);
                const coreVector2 vPos  = s_Spline.CalcPositionLerp(LERPB(0.0f, 1.0f,    fTime));
                const coreVector2 vDir  = coreVector2::Direction   (LERPS(0.0f, 4.0f*PI, fTime));

                // 
                if((pPlayer->GetPosition().y < -FOREGROUND_AREA.y) && (vPos.x >= -FOREGROUND_AREA.y))
                {
                    g_pSpecialEffects->PlaySound      (pPlayer->GetPosition(), 1.0f, SOUND_RUSH_LONG);
                    g_pSpecialEffects->CreateBlowColor(pPlayer->GetPosition(), coreVector3(0.0f,1.0f,0.0f), SPECIAL_BLOW_SMALL, COLOR_FIRE_BLUE);
                }

                // fly player animated into the game field
                pPlayer->SetPosition   (coreVector3(pPlayer->GetPosition().x, vPos));
                pPlayer->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
                pPlayer->SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
                pPlayer->UpdateExhaust (LERPB(1.0f, 0.0f, fTime));
            });
        }
    }

    return true;
}


// ****************************************************************
// handle outro animation
coreBool cGame::__HandleOutro()
{
    if(CONTAINS_FLAG(m_iStatus, GAME_STATUS_OUTRO))
    {
        // 
        m_fTimeInOut.Update(1.0f);

        this->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            // calculate new player position and rotation
            const coreFloat   fTime = MAX((i ? -0.16f : 0.0f) + m_fTimeInOut, 0.0f);
            const coreFloat   fPos  = MIN(pPlayer->GetPosition().y + 90.0f * fTime * Core::System->GetTime(), 1000.0f);
            const coreVector2 vDir  = coreVector2::Direction(LERPS(0.0f, 2.0f*PI, 0.6f * fTime));

            // fly player animated out of the game field
            pPlayer->SetPosition   (coreVector3(pPlayer->GetPosition().x, fPos, pPlayer->GetPosition().z));
            pPlayer->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
            pPlayer->SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
            pPlayer->UpdateExhaust ((fTime < 0.2f) ? LERPB(0.0f, 0.7f, fTime / 0.2f) : LERPB(0.7f, 0.3f, fTime - 0.2f));
        });
    }

    return true;
}


// ****************************************************************
// 
void cGame::__HandleDefeat()
{
    if(CONTAINS_FLAG(m_iStatus, GAME_STATUS_PLAY))
    {
        coreBool bAllDefeated = true;

        // 
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        {
            cPlayer* pPlayer = &m_aPlayer[i];

            // 
            const coreBool bDefeated = CONTAINS_FLAG(pPlayer->GetStatus(), PLAYER_STATUS_DEAD);
            bAllDefeated = bAllDefeated && bDefeated;

            // 
            g_pPostProcessing->SetSaturation(i, bDefeated ? 0.0f : (1.0f - MIN(pPlayer->GetDesaturate(), 1.0f)));

            if(bDefeated && m_bCoop && (m_pCurMission->GetID() != cNoMission::ID) && !m_pRepairEnemy)
            {
                // 
                m_pRepairEnemy = new cRepairEnemy();
                m_pRepairEnemy->AssignPlayer(pPlayer);
                m_pRepairEnemy->Resurrect();
            }
        }

        if(bAllDefeated)
        {
            if(m_pCurMission->GetID() == cIntroMission::ID)
            {
                // 
                this->StartOutro(1u);
            }
            else
            {
                // 
                REMOVE_FLAG(m_iStatus, GAME_STATUS_PLAY)
                ADD_FLAG   (m_iStatus, GAME_STATUS_DEFEATED)
            }

            if(m_pRepairEnemy)
            {
                // 
                g_pSpecialEffects->CreateSplashDark(m_pRepairEnemy->GetPosition(), SPECIAL_SPLASH_TINY);
                SAFE_DELETE(m_pRepairEnemy)
            }
        }

        if(m_pRepairEnemy && m_pRepairEnemy->ReachedDeath())
        {
            cPlayer* pPlayer = m_pRepairEnemy->GetPlayer();

            // 
            pPlayer->Resurrect();

            // 
            pPlayer->SetPosition    (m_pRepairEnemy->GetPosition());
            pPlayer->SetCurHealthPct(I_TO_F(1u) / I_TO_F(PLAYER_LIVES));
            pPlayer->SetDesaturate  (PLAYER_DESATURATE);
            pPlayer->StartFeeling   (PLAYER_FEEL_TIME_REPAIR, 0u);

            // 
            SAFE_DELETE(m_pRepairEnemy)
        }
    }
}


// ****************************************************************
// 
void cGame::__HandlePacifist()
{
    if(!m_bPacifist) return;

    // 
    const coreUintW iNumEnemies = m_EnemyManager.GetNumEnemiesAlive();
    if(iNumEnemies)
    {
        // 
        this->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            if(!pPlayer->IsRolling()) m_fPacifistDamage.Update(GAME_PACIFIST_DAMAGE / I_TO_F(m_bCoop ? GAME_PLAYERS : 1u));
        });

        // 
        if((m_fPacifistDamage >= GAME_PACIFIST_DAMAGE) && (F_TO_UI(m_fPacifistDamage) >= iNumEnemies))
        {
            // 
            const coreInt32 iDamage = F_TO_UI(m_fPacifistDamage) / iNumEnemies;
            m_fPacifistDamage -= I_TO_F(iDamage * iNumEnemies);

            // 
            m_EnemyManager.ForEachEnemy([&](cEnemy* OUTPUT pEnemy)
            {
                pEnemy->TakeDamage(iDamage, ELEMENT_NEUTRAL, pEnemy->GetPosition().xy(), NULL);
            });
        }
    }
    else
    {
        // 
        m_fPacifistDamage = 0.0f;
    }
}


// ****************************************************************
// handle default object collisions
void cGame::__HandleCollisions()
{
    // 
    m_EnemyManager.ForEachEnemy([](cEnemy* OUTPUT pEnemy)
    {
        pEnemy->ActivateModelLowOnly();
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_ENEMY, [](cPlayer* OUTPUT pPlayer, cEnemy* OUTPUT pEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        const coreVector2 vDir = (pPlayer->GetPosition().xy() - pEnemy->GetPosition().xy()).Normalized();
        pPlayer->SetForce    (vDir * 100.0f);
        pPlayer->SetInterrupt(PLAYER_INTERRUPT);

        // 
        g_pSpecialEffects->CreateSplashColor(pPlayer->GetPosition(), 50.0f, 10u, coreVector3(1.0f,1.0f,1.0f));
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_NORMAL, TYPE_BULLET_ENEMY, [](cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        pPlayer->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy());
        pBullet->Deactivate(true, vIntersection.xy());
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [](cEnemy* OUTPUT pEnemy, cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        if(!g_pForeground->IsVisiblePoint(vIntersection.xy())) return;

        // 
        if(pEnemy->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner())))
        {
            // 
            pBullet->Deactivate(true, vIntersection.xy());

            // 
            g_pSpecialEffects->RumblePlayer(d_cast<cPlayer*>(pBullet->GetOwner()), SPECIAL_RUMBLE_DEFAULT);
        }
        else
        {
            // 
            pBullet->Reflect(pEnemy);
        }
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_ALL, TYPE_CHROMA, [](cPlayer* OUTPUT pPlayer, cChromaBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        pBullet->Deactivate(true, vIntersection.xy());
    });

    // 
    cPlayer::TestCollision(PLAYER_TEST_ALL, TYPE_ITEM, [](cPlayer* OUTPUT pPlayer, cItem* OUTPUT pItem, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        pItem->Collect(pPlayer);
    });

    // 
    m_EnemyManager.ForEachEnemy([](cEnemy* OUTPUT pEnemy)
    {
        pEnemy->ActivateModelDefault();
    });
}


// ****************************************************************
// 
void cGame::__ClearAll(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Kill(bAnimated);

    // 
    m_EnemyManager       .ClearEnemies(bAnimated);
    m_BulletManagerPlayer.ClearBullets(bAnimated);
    m_BulletManagerEnemy .ClearBullets(bAnimated);
    m_ChromaManager      .ClearChromas(bAnimated);
    m_ItemManager        .ClearItems  (bAnimated);
    m_ShieldManager      .ClearShields(bAnimated);

    // 
    if(m_pRepairEnemy)
    {
        if(bAnimated) g_pSpecialEffects->MacroExplosionDarkSmall(m_pRepairEnemy->GetPosition());
        SAFE_DELETE(m_pRepairEnemy)
    }
}