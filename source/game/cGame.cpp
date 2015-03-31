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
: m_Interface    (bCoop ? 2u : 1u)
, m_pMission     (NULL)
, m_fTimeMission (0.0f)
, m_iStatus      (0u)
, m_bCoop        (bCoop)
{
    // configure first player
    m_aPlayer[0].Configure  (PLAYER_SHIP_ATK, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[0]);
    m_aPlayer[0].EquipWeapon(0u, cRayWeapon::ID);

    if(m_bCoop)
    {
        // configure second player
        m_aPlayer[1].Configure  (PLAYER_SHIP_DEF, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[1]);
        m_aPlayer[1].EquipWeapon(0u, cRayWeapon::ID);
        STATIC_ASSERT(GAME_PLAYERS == 2u)
    }

    // load first mission
    m_pMission = new cNoMission();

    // reset boss times
    std::memset(m_afTimeBoss, 0, sizeof(m_afTimeBoss));
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    // delete last mission
    SAFE_DELETE(m_pMission)

    // remove all remaining enemies
    ASSERT(m_apEnemyList.empty())
    m_apEnemyList.clear();
}


// ****************************************************************
// render the game
void cGame::Render()
{
    // render all players
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Render();

    // render all active enemies
    FOR_EACH(it, m_apEnemyList) (*it)->__RenderOwnBefore();
    FOR_EACH(it, m_apEnemyList) (*it)->  Render();
    FOR_EACH(it, m_apEnemyList) (*it)->__RenderOwnAfter();

    // render the bullet manager
    m_BulletManager.Render();
}


// ****************************************************************
// render the overlay separately
void cGame::RenderOverlay()
{
    // render all overlay objects
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
    m_pMission->Move();

    // move all players
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Move();

    // move all active enemies
    for(coreUintW i = 0u, ie = m_apEnemyList.size(); i < ie; ++i)
    {
        m_apEnemyList[i]->Move();

        // handle self-removing
        if(ie > m_apEnemyList.size()) {--i; --ie;}
    }

    // move the bullet manager
    m_BulletManager.Move();

    // handle default object collisions
    this->__HandleCollisions();

    // move all overlay objects
    m_CombatText.Move();
    m_Interface .Move();

    // set environment side
    if(m_bCoop) g_pEnvironment->SetTargetSide((m_aPlayer[0].GetPosition().xy() + m_aPlayer[1].GetPosition().xy()) * 0.5f * 0.65f);
           else g_pEnvironment->SetTargetSide( m_aPlayer[0].GetPosition().xy()                                           * 0.65f);
    STATIC_ASSERT(GAME_PLAYERS == 2u)
}


// ****************************************************************
// load new active mission
void cGame::LoadMission(const coreInt32& iID)
{
    if(m_pMission) if(m_pMission->GetID() == iID) return;

    // delete possible old mission
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
    this->LoadMission(cNoMission::ID);
    this->LoadMission(iID);
}


// ****************************************************************
// 
cPlayer* cGame::FindPlayer(const coreVector2& vPosition)
{
    // 
    if(!m_bCoop) return &m_aPlayer[0];

    // 
    cPlayer*  pPlayer = &m_aPlayer[0];
    coreFloat fDiffSq = 1.0e06f;

    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
    {
        cPlayer& oCurPlayer = m_aPlayer[i];
        if(CONTAINS_VALUE(oCurPlayer.GetStatus(), PLAYER_STATUS_DEAD)) continue;

        // 
        const coreFloat fCurDiffSq = (oCurPlayer.GetPosition().xy() - vPosition).LengthSq();
        if(fCurDiffSq < fDiffSq)
        {
            // 
            pPlayer = &oCurPlayer;
            fDiffSq = fCurDiffSq;
        }
    }

    // 
    return pPlayer;
}


// ****************************************************************
// 
cEnemy* cGame::FindEnemy(const coreVector2& vPosition)
{
    // 
    cEnemy*   pEnemy  = m_pMission->GetBoss(0u);
    coreFloat fDiffSq = 1.0e06f;

    FOR_EACH(it, m_apEnemyList)
    {
        cEnemy* pCurEnemy = (*it);

        // 
        const coreFloat fCurDiffSq = (pCurEnemy->GetPosition().xy() - vPosition).LengthSq();
        if(fCurDiffSq < fDiffSq)
        {
            // 
            pEnemy  = pCurEnemy;
            fDiffSq = fCurDiffSq;
        }
    }

    // 
    return pEnemy;
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

                // fly player animated into the game field
                oPlayer.SetPosition   (coreVector3(oPlayer.GetPosition().x, vPos));
                oPlayer.SetNewPos     (coreVector2(oPlayer.GetPosition().xy()));
                oPlayer.SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
            }
        }
    }

    return true;
}


// ****************************************************************
// handle default object collisions
void cGame::__HandleCollisions()
{
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_ENEMY, [](cPlayer* pPlayer, cEnemy* pEnemy, const coreBool& bFirst)
    {

    });

    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_BULLET_ENEMY, [](cPlayer* pPlayer, cBullet* pBullet, const coreBool& bFirst)
    {
        // 
        pPlayer->TakeDamage(pBullet->GetDamage());
        pBullet->Deactivate(true);
    });

    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [](cEnemy* pEnemy, cBullet* pBullet, const coreBool& bFirst)
    {
        // 
        if((ABS(pEnemy->GetPosition().x) >= FOREGROUND_AREA.x * 1.1f) ||
           (ABS(pEnemy->GetPosition().y) >= FOREGROUND_AREA.y * 1.1f)) return;

        // 
        pEnemy ->TakeDamage(pBullet->GetDamage(), s_cast<cPlayer*>(pBullet->GetOwner()));
        pBullet->Deactivate(true);
    });

    //Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, TYPE_BULLET_PLAYER, [](cBullet* pBullet1, cBullet* pBullet2, const coreBool& bFirst)
    //{

    //    pBullet1->Deactivate(true);
    //    pBullet2->Deactivate(true);

    //});
}