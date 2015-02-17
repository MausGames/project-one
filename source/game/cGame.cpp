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
cGame::cGame()noexcept
: m_pMission (NULL)
{
    // create first player
    m_aPlayer[0].Configure  (1, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[0]);
    m_aPlayer[0].EquipWeapon(0, cRayWeapon::ID);
    m_aPlayer[0].Resurrect  (coreVector2(-10.0f,-25.0f));

    // coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB() red

    // load first mission
    m_pMission = new cNoMission();
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    // 
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
    FOR_EACH(it, m_apEnemyList)
        (*it)->Render();

    // render the bullet manager
    m_BulletManager.Render();
}


// ****************************************************************
// 
void cGame::RenderOverlay()
{
    // 
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

    // calculate center of player positions
    coreVector2 vCenterPos;
         if(CONTAINS_VALUE(m_aPlayer[0].GetStatus(), PLAYER_STATUS_DEAD)) vCenterPos =  m_aPlayer[1].GetPosition().xy();
    else if(CONTAINS_VALUE(m_aPlayer[1].GetStatus(), PLAYER_STATUS_DEAD)) vCenterPos =  m_aPlayer[0].GetPosition().xy();
                                                                     else vCenterPos = (m_aPlayer[0].GetPosition().xy()  +
                                                                                        m_aPlayer[1].GetPosition().xy()) * 0.5f;
    STATIC_ASSERT(GAME_PLAYERS == 2)

    // set environment side
    g_pEnvironment->SetTargetSide(vCenterPos * 0.65f);



    Core::Manager::Object->TestCollision(TYPE_PLAYER, TYPE_BULLET_ENEMY, [](cPlayer* pPlayer, cBullet* pBullet, const bool& bFirst)
    {

    });
    Core::Manager::Object->TestCollision(TYPE_ENEMY, TYPE_BULLET_PLAYER, [](cEnemy* pEnemy, cBullet* pBullet, const bool& bFirst)
    {
        pEnemy->TakeDamage(pBullet->GetDamage());
        pBullet->Deactivate(true);
    });



    // 
    m_CombatText.Move();
    m_Interface .Move();
}


// ****************************************************************
// 
void cGame::LoadMission(const int& iID)
{
    if(m_pMission) if(m_pMission->GetID() == iID) return;

    // delete possible old mission
    SAFE_DELETE(m_pMission)

    // load new mission
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
}