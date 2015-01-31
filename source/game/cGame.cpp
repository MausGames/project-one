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
{
    // create first player
    m_aPlayer[0].Configure  (1, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[0]);
    m_aPlayer[0].EquipWeapon(cRayWeapon::ID);
    m_aPlayer[0].Resurrect  (coreVector2(-10.0f,0.0f));

    if(true)
    {
        // TODO #
        m_aPlayer[1].Configure(0, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[1]);
        m_aPlayer[1].Resurrect(coreVector2(10.0f,0.0f));
    }




    std::memset(m_apTest, 0, sizeof(m_apTest));

    m_apTest[0] = new cScoutEnemy();
    m_apTest[1] = new cWarriorEnemy();
    m_apTest[2] = new cStarEnemy();
    m_apTest[3] = new cArrowEnemy();
    m_apTest[4] = new cMinerEnemy();
    m_apTest[5] = new cFreezerEnemy();
    m_apTest[6] = new cCinderEnemy();

    //m_apTest[1]->SetSize(coreVector3(0.99f, 1.0f, 1.0f));

    for(int i = 0; i < int(ARRAY_SIZE(m_apTest)); ++i)
    {
        if(m_apTest[i]) m_apTest[i]->SetPosition(coreVector3(FOREGROUND_AREA.x * I_TO_F(i-3) * 0.22f, 10.0f, 0.0f));
       // if(m_apTest[i]) m_apTest[i]->SetDirection(coreVector3(-1.0f,-1.0f,0.0f).Normalize());
        if(m_apTest[i]) m_apEnemyList.push_back(m_apTest[i]);
    }
}


// ****************************************************************
// destructor
cGame::~cGame()
{
    // clear memory
    m_apEnemyList.clear();


    for(int i = 0; i < int(ARRAY_SIZE(m_apTest)); ++i)
        SAFE_DELETE(m_apTest[i])
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
         if(m_aPlayer[0].GetStatus() & PLAYER_STATUS_DEAD) vCenterPos =  m_aPlayer[1].GetPosition().xy();
    else if(m_aPlayer[1].GetStatus() & PLAYER_STATUS_DEAD) vCenterPos =  m_aPlayer[0].GetPosition().xy();
                                                      else vCenterPos = (m_aPlayer[0].GetPosition().xy() +
                                                                         m_aPlayer[1].GetPosition().xy()) * 0.5f;
    STATIC_ASSERT(GAME_PLAYERS == 2)

    // set environment side
    g_pEnvironment->SetTargetSide(vCenterPos * 0.65f);
}