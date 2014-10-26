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
    m_aPlayer[0].Resurrect();
    m_aPlayer[1].Resurrect();
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
    // 
    m_aPlayer[0].Render();
}


// ****************************************************************
// move the game
void cGame::Move()
{
    // 
    m_aPlayer[0].SetPosition(coreVector3(Core::Input->GetMousePosition() * 150.0f, 0.0f));
    m_aPlayer[0].Move();
}