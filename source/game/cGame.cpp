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
cGame::cGame(const coreUint8 iDifficulty, const coreBool bCoop, const coreInt32* piMissionList, const coreUintW iNumMissions)noexcept
: m_BulletManagerPlayer (TYPE_BULLET_PLAYER)
, m_BulletManagerEnemy  (TYPE_BULLET_ENEMY)
, m_Interface           (bCoop ? GAME_PLAYERS : 1u)
, m_piMissionList       (piMissionList)
, m_iNumMissions        (iNumMissions)
, m_pCurMission         (NULL)
, m_iCurMissionIndex    (coreUintW(-1))
, m_fTimeInOut          (0.0f)
, m_iDepthLevel         (0u)
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

    if(m_bCoop)
    {
        // configure second player
        m_aPlayer[1].Configure  (PLAYER_SHIP_DEF, COLOR_SHIP_BLUE);
        m_aPlayer[1].EquipWeapon(0u, cRayWeapon::ID);

        // 
        m_aPlayer[0].SetInput(&g_aGameInput[0]);
        m_aPlayer[1].SetInput(&g_aGameInput[1]);

        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }

    // load first mission
    m_pCurMission = new cNoMission();
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    constexpr coreBool bAnimated = !DEFINED(_CORE_DEBUG_);

    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Kill(bAnimated);

    // 
    m_EnemyManager.ClearEnemies(bAnimated);

    // 
    m_BulletManagerPlayer.ClearBullets(bAnimated);
    m_BulletManagerEnemy .ClearBullets(bAnimated);

    // 
    m_ChromaManager.ClearChromas(bAnimated);

    // 
    m_ItemManager.ClearItems(bAnimated);

    // 
    m_ShieldManager.ClearShields(bAnimated);

    // delete last mission
    SAFE_DELETE(m_pCurMission)

    // 
    g_pEnvironment->SetTargetDirection(ENVIRONMENT_DEFAULT_DIRECTION);
    g_pEnvironment->SetTargetSide     (ENVIRONMENT_DEFAULT_SIDE);
    g_pEnvironment->SetTargetSpeed    (ENVIRONMENT_DEFAULT_SPEED);
}


// ****************************************************************
// render the game
void cGame::Render()
{
    __DEPTH_LEVEL_SHIP   // # 1
    {
        // render all players
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            m_aPlayer[i].Render();

        // render all enemies
        m_EnemyManager.Render();
    }

    __DEPTH_LEVEL_UNDER
    {
        glDepthMask(false);
        {
            // render underlying effects
            m_EnemyManager.RenderUnder();
            m_pCurMission->RenderUnder();

            // 
            m_ShieldManager.Render();
        }
        glDepthMask(true);

        // render low-priority bullet manager
        m_BulletManagerPlayer.Render();
    }

    __DEPTH_LEVEL_SHIP   // # 2
    {
        // apply deferred outline-layer
        g_pOutline->Apply();

        // 
        m_ChromaManager.Render();

        // 
        m_ItemManager.Render();
    }

    __DEPTH_LEVEL_ATTACK
    {
        // render attacks and gameplay objects
        m_EnemyManager.RenderAttack();
        m_pCurMission->RenderAttack();
    }

    __DEPTH_LEVEL_OVER
    {
        glDisable(GL_DEPTH_TEST);
        {
            // render overlying effects
            m_EnemyManager.RenderOver();
            m_pCurMission->RenderOver();
        }
        glEnable(GL_DEPTH_TEST);

        // render special-effects
        g_pSpecialEffects->Render(true);

        // render high-priority bullet manager
        m_BulletManagerEnemy.Render();
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

    // move the mission
    m_pCurMission->MoveBefore();
    {
        // move all players
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            m_aPlayer[i].Move();

        // move all enemies
        if(!m_pCurMission->IsWaiting()) m_EnemyManager.Move();
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
    // apply combat stats
    m_CombatStats.Apply();

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
    m_BulletManagerPlayer.ClearBullets(true);
    m_BulletManagerEnemy .ClearBullets(true);

    // 
    m_ChromaManager.ClearChromas(true);

    // 
    m_ItemManager.ClearItems(true);

    // delete possible old mission
    m_EnemyManager.ClearEnemies(true);
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
        // 
        m_pCurMission->Setup();

        // set initial status
        m_iStatus = GAME_STATUS_LOADING;

        if(m_bCoop)
        {
            // reset all available players
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            {
                m_aPlayer[i].Kill(false);
                m_aPlayer[i].Resurrect(coreVector2(20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS-1u)), -100.0f));
                m_aPlayer[i].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
            }
        }
        else
        {
            // reset only the first player
            m_aPlayer[0].Kill(false);
            m_aPlayer[0].Resurrect(coreVector2(0.0f,-100.0f));
            m_aPlayer[0].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
        }
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
    if(m_iCurMissionIndex == m_iNumMissions - 1u)
    {
        // TODO
    }
    else
    {
        // 
        this->LoadMissionIndex(m_iCurMissionIndex + 1u);
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
}


// ****************************************************************
// 
void cGame::StartOutro()
{
    ASSERT(!CONTAINS_FLAG(m_iStatus, GAME_STATUS_INTRO))

    // 
    REMOVE_FLAG(m_iStatus, GAME_STATUS_PLAY)
    ADD_FLAG   (m_iStatus, GAME_STATUS_OUTRO)

    // 
    m_fTimeInOut = 0.0f;

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
void cGame::PushDepthLevel()
{
    // 
    m_iDepthLevel -= 1u;
    ASSERT(m_iDepthLevel)

    // 
    glDepthRange(0.1f * I_TO_F(m_iDepthLevel - 1u),
                 0.1f * I_TO_F(m_iDepthLevel));
}


// ****************************************************************
// 
void cGame::OffsetDepthLevel(const coreFloat fOffset)const
{
    // 
    glDepthRange(0.1f * I_TO_F(m_iDepthLevel - 1u),
                 0.1f * I_TO_F(m_iDepthLevel) - fOffset);
}


// ****************************************************************
// 
cPlayer* cGame::FindPlayer(const coreVector2& vPosition)
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
                s_Spline.AddNode(coreVector2(-140.0f,-10.0f), coreVector2( 1.0f, 0.0f));
                s_Spline.AddNode(coreVector2(  10.0f, 10.0f), coreVector2(-1.0f,-1.0f).Normalized());
                s_Spline.AddNode(coreVector2( -30.0f,  0.0f), coreVector2(-1.0f, 0.0f));
                s_Spline.Refine();
            }

            this->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                // calculate new player position and rotation
                const coreFloat   fTime = CLAMP((i ? 0.0f : 0.08f) + m_fTimeInOut / GAME_INTRO_DURATION, 0.0f, 1.0f);
                const coreVector2 vPos  = s_Spline.CalcPosition (LERPB(0.0f, s_Spline.GetTotalDistance(), fTime));
                const coreVector2 vDir  = coreVector2::Direction(LERPS(0.0f, 4.0f*PI,                     fTime));

                // 
                if((pPlayer->GetPosition().y < -FOREGROUND_AREA.y) && (vPos.x >= -FOREGROUND_AREA.y))
                {
                    g_pSpecialEffects->PlaySound      (pPlayer->GetPosition(), 1.0f, SOUND_RUSH_LONG);
                    g_pSpecialEffects->CreateBlowColor(pPlayer->GetPosition(), coreVector3(0.0f,1.0f,0.0f), SPECIAL_BLOW_SMALL, COLOR_FIRE_BLUE);
                }

                // fly player animated into the game field
                pPlayer->SetPosition   (coreVector3(pPlayer->GetPosition().x, vPos));
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

        this->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            // calculate new player position and rotation
            const coreFloat   fTime = MAX((i ? -0.16f : 0.0f) + m_fTimeInOut, 0.0f);
            const coreFloat   fPos  = MIN(pPlayer->GetPosition().y + 90.0f * fTime * Core::System->GetTime(), 1000.0f);
            const coreVector2 vDir  = coreVector2::Direction(LERPS(0.0f, 2.0f*PI, 0.6f * fTime));

            // fly player animated out of the game field
            pPlayer->SetPosition   (coreVector3(pPlayer->GetPosition().x, fPos, pPlayer->GetPosition().z));
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
    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
    {
        if(!CONTAINS_FLAG(m_aPlayer[i].GetStatus(), PLAYER_STATUS_DEAD))
            return;
    }

    // TODO
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
    cPlayer::TestCollision(TYPE_ENEMY, [](cPlayer* OUTPUT pPlayer, cEnemy* OUTPUT pEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(15, ELEMENT_NEUTRAL, vIntersection.xy());
        pEnemy ->TakeDamage(50, ELEMENT_NEUTRAL, vIntersection.xy(), pPlayer);

        // 
        g_pSpecialEffects->MacroExplosionPhysicalDarkSmall(vIntersection);
    });

    // 
    cPlayer::TestCollision(TYPE_BULLET_ENEMY, [](cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        pPlayer->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy());
        pBullet->Deactivate(true, vIntersection.xy());
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [](cEnemy* OUTPUT pEnemy, cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        //if((ABS(vIntersection.x) >= FOREGROUND_AREA.x * 1.1f) ||
        //   (ABS(vIntersection.y) >= FOREGROUND_AREA.y * 1.1f)) return;

        // 
        pEnemy ->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), vIntersection.xy(), d_cast<cPlayer*>(pBullet->GetOwner()));
        pBullet->Deactivate(true, vIntersection.xy());

        // 
        g_pSpecialEffects->RumblePlayer(d_cast<cPlayer*>(pBullet->GetOwner()), SPECIAL_RUMBLE_DEFAULT);


        const coreVector2 fDiff   = vIntersection.xy() - pBullet->GetPosition().xy();
        const coreFloat   fOffset = pBullet->GetCollisionRange().y - coreVector2::Dot(fDiff, pBullet->GetDirection().xy());
        pBullet->SetPosition(pBullet->GetPosition() - fOffset * pBullet->GetDirection());

        //g_pGame->GetChromaManager()->AddChroma(vIntersection.xy(), -pBullet->GetDirection().xy(), CHROMA_SCALE_TINY, pEnemy->GetBaseColor());     

    });

    // 
    cPlayer::TestCollision(TYPE_CHROMA, [](cPlayer* OUTPUT pPlayer, cChromaBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        pBullet->Deactivate(true, vIntersection.xy());
    });

    // 
    cPlayer::TestCollision(TYPE_ITEM, [](cPlayer* OUTPUT pPlayer, cItem* OUTPUT pItem, const coreVector3& vIntersection, const coreBool bFirstHit)
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