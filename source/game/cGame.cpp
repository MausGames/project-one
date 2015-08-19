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
cGame::cGame(const coreBool& bCoop)noexcept
: m_BulletManagerPlayer (TYPE_BULLET_PLAYER)
, m_BulletManagerEnemy  (TYPE_BULLET_ENEMY)
, m_Interface           (bCoop ? 2u : 1u)
, m_pMission            (NULL)
, m_fTimeMission        (0.0f)
, m_afTimeBoss          {}
, m_iStatus             (0u)
, m_bCoop               (bCoop)
{
    // configure first player
    m_aPlayer[0].Configure  (PLAYER_SHIP_ATK, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[0]);
    m_aPlayer[0].EquipWeapon(0u, REF_ID(cRayWeapon::ID));

    if(m_bCoop)
    {
        // configure second player
        m_aPlayer[1].Configure  (PLAYER_SHIP_DEF, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[1]);
        m_aPlayer[1].EquipWeapon(0u, REF_ID(cPulseWeapon::ID));
        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }

    // load first mission
    m_pMission = new cNoMission();
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    // 
    m_BulletManagerPlayer.ClearBullets(false);
    m_BulletManagerEnemy .ClearBullets(false);

    // delete last mission
    m_EnemyManager.ClearEnemies(false);
    SAFE_DELETE(m_pMission)
}


// ****************************************************************
// render the game
void cGame::Render()
{
    // render all players
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Render();

    // render all enemies
    m_EnemyManager.Render();

    // move everything to the back (can be overdrawn)
    glDepthRange(GAME_DEPTH_WEAK);
    {
        // 
        m_EnemyManager.RenderWeak();
        m_pMission   ->RenderWeak();

        // apply weak effect outline-layer
        g_aaOutline[PRIO_WEAK][STYLE_FULL]  .Apply();
        g_aaOutline[PRIO_WEAK][STYLE_DIRECT].Apply();
    }
    glDepthRange(GAME_DEPTH_PLAYER);
    {
        // render low-priority bullet manager
        m_BulletManagerPlayer.Render();

        // apply player-bullet outline-layer
        g_aaOutline[PRIO_PLAYER][STYLE_FULL]  .Apply();
        g_aaOutline[PRIO_PLAYER][STYLE_DIRECT].Apply();
    }

    // move everything to the front (should always be visible to player)
    glDepthRange(GAME_DEPTH_STRONG);
    {
        // 
        m_EnemyManager.RenderStrong();
        m_pMission   ->RenderStrong();

        // apply strong effect outline-layer
        g_aaOutline[PRIO_STRONG][STYLE_FULL]  .Apply();
        g_aaOutline[PRIO_STRONG][STYLE_DIRECT].Apply();

        // 
        m_EnemyManager.RenderAfter();
        m_pMission   ->RenderAfter();
    }
    glDepthRange(GAME_DEPTH_ENEMY);
    {
        // render high-priority bullet manager
        glDepthFunc(GL_ALWAYS);
        m_BulletManagerEnemy.Render();
        glDepthFunc(GL_LEQUAL);

        // apply enemy-bullet outline-layer
        g_aaOutline[PRIO_ENEMY][STYLE_FULL]  .Apply();
        g_aaOutline[PRIO_ENEMY][STYLE_DIRECT].Apply();
    }

    // reset depth range
    glDepthRange(0.0f, 1.0f);
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
    // handle intro animation
    if(!this->__HandleIntro()) return;

    // update total mission and boss time (if currently active)
    m_fTimeMission.Update(1.0f);
    if(m_pMission->GetCurBoss()) m_afTimeBoss[m_pMission->GetCurBossIndex()].Update(1.0f);

    // move the mission
    m_pMission->MoveBefore();
    {
        // move all players
        for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            m_aPlayer[i].Move();

        // move all enemies
        m_EnemyManager.Move();
    }
    m_pMission->MoveAfter();

    // move the bullet managers
    m_BulletManagerPlayer.Move();
    m_BulletManagerEnemy .Move();

    // handle default object collisions
    this->__HandleCollisions();

    // move all overlay objects
    m_CombatText.Move();
    m_Interface .Move();
}


// ****************************************************************
// load new active mission
void cGame::LoadMission(const coreInt32& iID)
{
    if(m_pMission) if(m_pMission->GetID() == iID) return;

    // delete possible old mission
    m_EnemyManager.ClearEnemies(false);
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
    }
    m_pMission->Setup();

    if(iID != cNoMission::ID)
    {
        // 
        m_Interface.ShowMission(m_pMission);

        // 
        m_fTimeMission = -(GAME_INTRO_DURATION + GAME_INTRO_DELAY);
        for(coreUintW i = 0u; i < MISSION_BOSSES; ++i) m_afTimeBoss[i] = -INTERFACE_BANNER_DURATION;

        // set initial status
        m_iStatus = GAME_STATUS_INTRO | GAME_STATUS_LOADING;

        if(m_bCoop)
        {
            // reset all available players
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            {
                m_aPlayer[i].Kill(false);
                m_aPlayer[i].Resurrect(coreVector2(20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS-1)), -100.0f));
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
}


// ****************************************************************
// restart current mission
void cGame::RestartMission()
{
    // save mission ID
    const coreInt32 iID = m_pMission->GetID();
    ASSERT(iID != cNoMission::ID)

    // delete mission and create it again
    this->LoadMission(REF_ID(cNoMission::ID));
    this->LoadMission(iID);
}


// ****************************************************************
// 
cPlayer* RETURN_NONNULL cGame::FindPlayer(const coreVector2& vPosition)
{
    // 
    if(!m_bCoop) return &m_aPlayer[0];

    // 
    cPlayer*  pPlayer = &m_aPlayer[0];
    coreFloat fLenSq  = 1.0e06f;

    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
    {
        cPlayer* pCurPlayer = &m_aPlayer[i];
        if(CONTAINS_VALUE(pCurPlayer->GetStatus(), PLAYER_STATUS_DEAD)) continue;

        // 
        const coreFloat fCurLenSq = (pCurPlayer->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq < fLenSq)
        {
            // 
            pPlayer = pCurPlayer;
            fLenSq  = fCurLenSq;
        }
    }

    // 
    ASSERT(pPlayer)
    return pPlayer;
}


// ****************************************************************
// handle intro animation
coreBool cGame::__HandleIntro()
{
    if(CONTAINS_VALUE(m_iStatus, GAME_STATUS_LOADING))
    {
        // do not start while game resources are still loading
        if(Core::Manager::Resource->IsLoading()) return false;
        REMOVE_VALUE(m_iStatus, GAME_STATUS_LOADING)
    }

    if(CONTAINS_VALUE(m_iStatus, GAME_STATUS_INTRO))
    {
        if(m_fTimeMission > 0.0f)
        {
            // end intro and start actual game
            REMOVE_VALUE(m_iStatus, GAME_STATUS_INTRO)
            ADD_VALUE   (m_iStatus, GAME_STATUS_PLAY)

            // re-enable player controls
            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
                m_aPlayer[i].RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);
        }
        else
        {
            // create spline for intro animation (YZ)
            coreSpline2 oSpline;
            oSpline.AddNode(coreVector2(-140.0f,-10.0f), coreVector2( 1.0f, 0.0f));
            oSpline.AddNode(coreVector2(  10.0f, 10.0f), coreVector2(-1.0f,-1.0f).Normalize());
            oSpline.AddNode(coreVector2( -30.0f,  0.0f), coreVector2(-1.0f, 0.0f));

            for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
            {
                cPlayer& oPlayer = m_aPlayer[i];
                if(CONTAINS_VALUE(oPlayer.GetStatus(), PLAYER_STATUS_DEAD)) continue;

                // calculate new player position and rotation
                const coreFloat   fTime = CLAMP((i ? 1.0f : 1.08f) + m_fTimeMission / GAME_INTRO_DURATION, 0.0f, 1.0f);
                const coreVector2 vPos  = oSpline.CalcPosition  (LERPB(0.0f, oSpline.GetTotalDistance(), fTime));
                const coreVector2 vDir  = coreVector2::Direction(LERPS(0.0f, 4.0f*PI,                    fTime));

                // 
                if((oPlayer.GetPosition().y < -FOREGROUND_AREA.y) && (vPos.x >= -FOREGROUND_AREA.y))
                {
                    g_pSpecialEffects->PlaySound      (oPlayer.GetPosition(), 1.0f, SOUND_RUSH_LONG);
                    g_pSpecialEffects->CreateBlowColor(oPlayer.GetPosition(), coreVector3(0.0f,1.0f,0.0f), SPECIAL_BLOW_SMALL, COLOR_FIRE_BLUE);
                    g_MusicPlayer.Control()->Play();  
                }

                // fly player animated into the game field
                oPlayer.SetPosition   (coreVector3(oPlayer.GetPosition().x, vPos));
                oPlayer.SetNewPos     (coreVector2(oPlayer.GetPosition().xy()));
                oPlayer.SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
                oPlayer.UpdateExhaust (LERPB(1.0f, 0.0f, fTime));
            }
        }
    }

    return true;
}


// ****************************************************************
// handle default object collisions
void cGame::__HandleCollisions()
{
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_ENEMY, [](cPlayer* OUTPUT pPlayer, cEnemy* OUTPUT pEnemy, const coreBool& bFirstHit)
    {
        if(!bFirstHit) return;

        // 
        pPlayer->TakeDamage(15);

        // 
        const coreVector3 vCenter = coreVector3((pPlayer->GetPosition().xy() + pEnemy->GetPosition().xy()) * 0.5f, 0.0f);
        g_pSpecialEffects->MacroExplosionPhysicalSmall(vCenter);
    });

    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_BULLET_ENEMY, [](cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreBool& bFirstHit)
    {
        // 
        pPlayer->TakeDamage(pBullet->GetDamage());
        pBullet->Deactivate(true);
    });

    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [](cEnemy* OUTPUT pEnemy, cBullet* OUTPUT pBullet, const coreBool& bFirstHit)
    {
        // 
        if((ABS(pEnemy->GetPosition().x) >= FOREGROUND_AREA.x * 1.1f) ||
           (ABS(pEnemy->GetPosition().y) >= FOREGROUND_AREA.y * 1.1f)) return;

        // 
        pEnemy ->TakeDamage(pBullet->GetDamage(), s_cast<cPlayer*>(pBullet->GetOwner()));
        pBullet->Deactivate(true);
    });
}