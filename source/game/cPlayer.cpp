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
cPlayer::cPlayer()
: m_bDead (true)
{
    // load object resources
    this->DefineModel  ("ship_player_off.md3");
    this->DefineTexture(0, "ship_player_off.png");
    this->DefineProgram("object_ship_program");

    // set object properties
    this->SetPosition   (coreVector3(0.0f,0.0f,0.0f));
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
    this->SetColor3     (coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cPlayer::~cPlayer()
{
    // remove player from the game
    this->Kill(false);
}


// ****************************************************************
// render the player
void cPlayer::Render()
{
    if(m_bDead) return;

    // render the 3d-object
    coreObject3D::Render();
}


// ****************************************************************
// move the player
void cPlayer::Move()
{
    if(m_bDead) return;

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect()
{
    // resurrect player
    if(!m_bDead) return;
    m_bDead = false;

    // add player to global shadow and outline
    cShadow::BindGlobalObject(this);
    g_pOutline->BindObject(this);
}


// ****************************************************************
// remove player from the game
void cPlayer::Kill(const bool& bAnimated)
{
    // kill player
    if(m_bDead) return;
    m_bDead = true;

    // remove player from global shadow and outline
    cShadow::UnbindGlobalObject(this);
    g_pOutline->UnbindObject(this);
}