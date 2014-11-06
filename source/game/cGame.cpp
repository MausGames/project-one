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
cGame::cGame()
{
    // create first player
    m_aPlayer[0].Configure(0, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[0]);
    m_aPlayer[0].Resurrect(coreVector2(-10.0f,0.0f));
    
    if(false)
    {
        // TODO #
        m_aPlayer[1].Configure(1, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB(), g_CurConfig.Input.aiType[1]);
        m_aPlayer[1].Resurrect(coreVector2(10.0f,0.0f));
    }
}


// ****************************************************************
// destructor
cGame::~cGame()
{

}


// ****************************************************************
// render the game
void cGame::Render()
{
    // render all players
    for(coreByte i = 0; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Render();
}


// ****************************************************************
// move the game
void cGame::Move()
{
    // move all players
    for(coreByte i = 0; i < GAME_PLAYERS; ++i)
        m_aPlayer[i].Move();

    // calculate center of player positions
    coreVector2 vCenterPos;
         if(m_aPlayer[0].IsDead()) vCenterPos =  m_aPlayer[1].GetPosition().xy();
    else if(m_aPlayer[1].IsDead()) vCenterPos =  m_aPlayer[0].GetPosition().xy();
                              else vCenterPos = (m_aPlayer[0].GetPosition().xy() + m_aPlayer[1].GetPosition().xy()) * 0.5f;
    STATIC_ASSERT(GAME_PLAYERS == 2);

    // set environment side
    g_pEnvironment->SetTargetSide(vCenterPos * 0.65f);
}