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
cGame::cGame(const coreBool bCoop)noexcept
: m_BulletManagerPlayer (TYPE_BULLET_PLAYER)
, m_BulletManagerEnemy  (TYPE_BULLET_ENEMY)
, m_Interface           (bCoop ? GAME_PLAYERS : 1u)
, m_pMission            (NULL)
, m_fTimeGame           (0.0f)
, m_fTimeMission        (0.0f)
, m_afTimeBoss          {}
, m_iDepthLevel         (0u)
, m_iStatus             (0u)
, m_bCoop               (bCoop)
{
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
        m_aPlayer[0].SetInput(&g_aGameInput[1]);

        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }

    // load first mission
    m_pMission = new cNoMission();
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
    m_ShieldManager.ClearShields(bAnimated);

    // delete last mission
    SAFE_DELETE(m_pMission)
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
            m_pMission   ->RenderUnder();

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
    }

    __DEPTH_LEVEL_ATTACK
    {
        // render attacks and gameplay objects
        m_EnemyManager.RenderAttack();
        m_pMission   ->RenderAttack();
    }

    __DEPTH_LEVEL_OVER
    {
        glDisable(GL_DEPTH_TEST);
        {
            // render overlying effects
            m_EnemyManager.RenderOver();
            m_pMission   ->RenderOver();
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
// move the game
void cGame::Move()
{
    // handle intro and outro animation
    if(!this->__HandleIntro()) return;
    if(!this->__HandleOutro()) return;

    // update total game time
    m_fTimeGame.Update(1.0f);

    if(CONTAINS_FLAG(m_iStatus, GAME_STATUS_PLAY) || CONTAINS_FLAG(m_iStatus, GAME_STATUS_INTRO))
    {
        // update total mission and boss time
        m_fTimeMission.Update(1.0f);
        if(m_pMission->GetCurBoss()) m_afTimeBoss[m_pMission->GetCurBossIndex()].Update(1.0f);
    }

    // move the mission
    m_pMission->MoveBefore();
    {
        // move all players
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            m_aPlayer[i].Move();

        // move all enemies
        if(!m_pMission->IsWaiting()) m_EnemyManager.Move();
    }
    m_pMission->MoveAfter();

    // move the bullet managers
    m_BulletManagerPlayer.Move();
    m_BulletManagerEnemy .Move();

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
// load new active mission
void cGame::LoadMission(const coreInt32 iID)
{
    if(m_pMission) if(m_pMission->GetID() == iID) return;

    // 
    m_BulletManagerPlayer.ClearBullets(true);
    m_BulletManagerEnemy .ClearBullets(true);

    // delete possible old mission
    m_EnemyManager.ClearEnemies(true);
    SAFE_DELETE(m_pMission)

    // create new mission
    switch(iID)
    {
    default: ASSERT(false)
    case cNoMission     ::ID: m_pMission = new cNoMission     (); break;
    case cViridoMission ::ID: m_pMission = new cViridoMission (); break;
    case cNevoMission   ::ID: m_pMission = new cNevoMission   (); break;
    case cHarenaMission ::ID: m_pMission = new cHarenaMission (); break;
    case cRutilusMission::ID: m_pMission = new cRutilusMission(); break;
    case cGeluMission   ::ID: m_pMission = new cGeluMission   (); break;
    case cCalorMission  ::ID: m_pMission = new cCalorMission  (); break;
    case cMuscusMission ::ID: m_pMission = new cMuscusMission (); break;
    case cIntroMission  ::ID: m_pMission = new cIntroMission  (); break;
    }

    if(iID != cNoMission::ID)
    {
        // 
        m_pMission->Setup();

        // 
        m_fTimeGame    = 0.0f;
        m_fTimeMission = -(GAME_INTRO_DURATION + GAME_INTRO_DELAY);
        for(coreUintW i = 0u; i < MISSION_BOSSES; ++i) m_afTimeBoss[i] = -INTERFACE_BANNER_DURATION;

        // set initial status
        m_iStatus = GAME_STATUS_LOADING;

        if(m_bCoop)
        {
            // reset all available players
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            {
                m_aPlayer[i].Kill(true);
                m_aPlayer[i].Resurrect(coreVector2(20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS-1u)), -100.0f));
                m_aPlayer[i].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
            }
        }
        else
        {
            // reset only the first player
            m_aPlayer[0].Kill(true);
            m_aPlayer[0].Resurrect(coreVector2(0.0f,-100.0f));
            m_aPlayer[0].AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
        }
    }
}


// ****************************************************************
// 
void cGame::LoadNextMission()
{
    // 
    this->LoadMission(m_pMission->GetID() + 1);
}


// ****************************************************************
// restart current mission
void cGame::RestartMission()
{
    // save old mission
    cMission* pOldMission = m_pMission;
    m_pMission = NULL;

    // 
    this->LoadMission(pOldMission->GetID());
    SAFE_DELETE(pOldMission)
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
        if(m_fTimeMission >= 0.0f)
        {
            // end intro and start actual game
            REMOVE_FLAG(m_iStatus, GAME_STATUS_INTRO)
            ADD_FLAG   (m_iStatus, GAME_STATUS_PLAY)

            // re-enable player controls
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                m_aPlayer[i].RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);

            // 
            m_Interface.SetVisible(true);
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
                const coreFloat   fTime = CLAMP((i ? 1.0f : 1.08f) + m_fTimeMission / GAME_INTRO_DURATION, 0.0f, 1.0f);
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



        return false;
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



    if(m_pMission->GetID() == cIntroMission::ID)
    {
        this->LoadMission(cViridoMission::ID);
    }
    else
    {

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
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_ENEMY, [](cPlayer* OUTPUT pPlayer, cEnemy* OUTPUT pEnemy, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(15, ELEMENT_NEUTRAL);
        pEnemy ->TakeDamage(50, ELEMENT_NEUTRAL, pPlayer);

        // 
        g_pSpecialEffects->MacroExplosionPhysicalSmall(vIntersection);
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_BULLET_ENEMY, [](cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        pPlayer->TakeDamage(pBullet->GetDamage(), pBullet->GetElement());
        pBullet->Deactivate(true, vIntersection.xy());
    });

    // 
    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [](cEnemy* OUTPUT pEnemy, cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        if((ABS(pEnemy->GetPosition().x) >= FOREGROUND_AREA.x * 1.1f) ||
           (ABS(pEnemy->GetPosition().y) >= FOREGROUND_AREA.y * 1.1f)) return;

        // 
        pEnemy ->TakeDamage(pBullet->GetDamage(), pBullet->GetElement(), s_cast<cPlayer*>(pBullet->GetOwner()));
        pBullet->Deactivate(true, vIntersection.xy());

        // 
        g_pSpecialEffects->RumblePlayer(s_cast<cPlayer*>(pBullet->GetOwner()), SPECIAL_RUMBLE_DEFAULT);
    });

    // 
    m_EnemyManager.ForEachEnemy([](cEnemy* OUTPUT pEnemy)
    {
        pEnemy->ActivateModelDefault();
    });
}