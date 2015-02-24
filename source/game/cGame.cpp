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
cGame::cGame(const bool& bCoop)noexcept
: m_Interface    (bCoop ? 2 : 1)
, m_pMission     (NULL)
, m_fTimeMission (0.0f)
, m_iStatus      (0)
, m_bCoop        (bCoop)
{
    // configure first player
    m_aPlayer[0].Configure  (PLAYER_SHIP_OFF, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[0]);
    m_aPlayer[0].EquipWeapon(0, cRayWeapon::ID);

    if(m_bCoop)
    {
        // configure second player
        m_aPlayer[1].Configure  (PLAYER_SHIP_DEF, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[1]);
        m_aPlayer[1].EquipWeapon(0, cRayWeapon::ID);
        STATIC_ASSERT(GAME_PLAYERS == 2)
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
    for(coreByte i = 0; i < GAME_PLAYERS; ++i)
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
    // move all players
    for(coreByte i = 0; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Move();

    // move all active enemies
    FOR_EACH(it, m_apEnemyList)
        (*it)->Move();

    // move the bullet manager
    m_BulletManager.Move();

    // handle default object collisions
    this->__HandleCollisions();

    // move all overlay objects
    m_CombatText.Move();
    m_Interface .Move();

    // calculate center of player positions
    coreVector2 vCenterPos;
         if(CONTAINS_VALUE(m_aPlayer[1].GetStatus(), PLAYER_STATUS_DEAD)) vCenterPos =  m_aPlayer[0].GetPosition().xy();
    else if(CONTAINS_VALUE(m_aPlayer[0].GetStatus(), PLAYER_STATUS_DEAD)) vCenterPos =  m_aPlayer[1].GetPosition().xy();
                                                                     else vCenterPos = (m_aPlayer[0].GetPosition().xy()  +
                                                                                        m_aPlayer[1].GetPosition().xy()) * 0.5f;
    // set environment side
    g_pEnvironment->SetTargetSide(vCenterPos * 0.65f);
    STATIC_ASSERT(GAME_PLAYERS == 2)





    // 
    m_fTimeMission.Update(1.0f);
    if(m_fTimeMission > 0.0f)
    {
        if(m_pMission->GetCurBoss())
            m_afTimeBoss[m_pMission->GetCurBossNum()].Update(1.0f);
    }
}


// ****************************************************************
// load new active mission
void cGame::LoadMission(const int& iID)
{
    if(m_pMission) if(m_pMission->GetID() == iID) return;

    // delete possible old mission
    SAFE_DELETE(m_pMission)

    // create new mission
    switch(iID)
    {
    default: ASSERT(false)
    case cNoMission     ::ID: m_pMission = new cNoMission     (); break;
    case cMellanMission ::ID: m_pMission = new cMellanMission (); break;
    case cNevoMission   ::ID: m_pMission = new cNevoMission   (); break;
    case cHarenaMission ::ID: m_pMission = new cHarenaMission (); break;
    case cRutilusMission::ID: m_pMission = new cRutilusMission(); break;
    case cGeluMission   ::ID: m_pMission = new cGeluMission   (); break;
    case cCalorMission  ::ID: m_pMission = new cCalorMission  (); break;
    case cMuscusMission ::ID: m_pMission = new cMuscusMission (); break;
    }

    if(iID != cNoMission::ID)
    {
        // 
        m_fTimeMission = -GAME_INTRO_TIME;
        std::memset(m_afTimeBoss, 0, sizeof(m_afTimeBoss));

        // 
        m_iStatus = GAME_STATUS_INTRO;

        if(m_bCoop)
        {
            // 
            for(coreByte i = 0; i < GAME_PLAYERS; ++i)
            {
                m_aPlayer[i].Kill(false);
                m_aPlayer[i].Resurrect(coreVector2(40.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS-1)), -70.0f));
            }
        }
        else
        {
            // 
            m_aPlayer[0].Kill(false);
            m_aPlayer[0].Resurrect(coreVector2(0.0f,-70.0f));
        }
    }
}


// ****************************************************************
// 
void cGame::RestartMission()
{
    // 
    const int iID = m_pMission->GetID();
    ASSERT(iID != cNoMission::ID)

    // 
    this->LoadMission(cNoMission::ID);
    this->LoadMission(iID);
}


// ****************************************************************
// handle default object collisions
void cGame::__HandleCollisions()
{
    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_ENEMY, [](cPlayer* pPlayer, cEnemy* pEnemy, const bool& bFirst)
    {

    });

    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_BULLET_ENEMY, [](cPlayer* pPlayer, cBullet* pBullet, const bool& bFirst)
    {

    });

    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [](cEnemy* pEnemy, cBullet* pBullet, const bool& bFirst)
    {
        pEnemy->TakeDamage(pBullet->GetDamage(), s_cast<cPlayer*>(pBullet->GetOwner()));
        pBullet->Deactivate(true);
    });
}