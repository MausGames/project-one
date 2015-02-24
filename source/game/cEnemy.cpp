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
cEnemy::cEnemy()noexcept
{
    // load object resources
    this->DefineProgram("object_ship_program");
    this->DefineTexture(0, "ship_enemy.png");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,-1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f, 0.0f,1.0f));

    // set initial status
    m_iStatus = ENEMY_STATUS_DEAD;
}


// ****************************************************************
// destructor
cEnemy::~cEnemy()
{
    // remove enemy from the game
    this->Kill(false);
}


// ****************************************************************
// configure the enemy
void cEnemy::Configure(const int& iHealth, const coreVector3& vColor)
{
    // set maximum and current health value
    m_iMaxHealth = m_iCurHealth = iHealth;

    // save color value
    this->SetBaseColor(vColor);
}


// ****************************************************************
// move the enemy
void cEnemy::Move()
{
    // call individual move routine
    this->__MoveOwn();

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// reduce current health
void cEnemy::TakeDamage(const int& iDamage, cPlayer* pAttacker)
{
    // 
    m_iCurHealth -= iDamage;

    // 
    if(m_iCurHealth <= 0)
    {
        m_iCurHealth = 0;
    }

    // 
    this->SetColor3(LERP(coreVector3(0.5f,0.5f,0.5f), this->GetBaseColor(), (I_TO_F(m_iCurHealth) / I_TO_F(m_iMaxHealth))));
}


// ****************************************************************
// add enemy to the game
void cEnemy::Resurrect(const coreVector2& vPosition)
{
    // resurrect enemy
    if(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;
    REMOVE_VALUE(m_iStatus, ENEMY_STATUS_DEAD)

    // bind enemy to active list
    g_pGame->__BindEnemy(this);

    // add ship to the game
    cShip::_Resurrect(vPosition, TYPE_ENEMY);
}


// ****************************************************************
// remove enemy from the game
void cEnemy::Kill(const bool& bAnimated)
{
    // kill enemy
    if(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;
    ADD_VALUE(m_iStatus, ENEMY_STATUS_DEAD)

    // unbind enemy from active list
    g_pGame->__UnbindEnemy(this);

    // remove ship from the game
    cShip::_Kill(bAnimated);
}


// ****************************************************************
// constructor
cScoutEnemy::cScoutEnemy()noexcept
{
    // load models
    this->DefineModel   ("ship_enemy_scout_high.md3");
    this->DefineModelLow("ship_enemy_scout_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// constructor
cWarriorEnemy::cWarriorEnemy()noexcept
{
    // load models
    this->DefineModel   ("ship_enemy_warrior_high.md3");
    this->DefineModelLow("ship_enemy_warrior_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(51.0f/360.0f, 100.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// constructor
cStarEnemy::cStarEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_star_high.md3");
    this->DefineModelLow("ship_enemy_star_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// move the star enemy
void cStarEnemy::__MoveOwn()
{
    // update rotation angle
    m_fAngle.Update(-3.0f);

    // rotate around z-axis
    const coreVector2 vDir = coreVector2::Direction(m_fAngle);
    this->SetDirection(coreVector3(vDir, 0.0f));
}


// ****************************************************************
// constructor
cArrowEnemy::cArrowEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_arrow_high.md3");
    this->DefineModelLow("ship_enemy_arrow_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(34.0f/360.0f, 100.0f/100.0f, 100.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// move the arrow enemy
void cArrowEnemy::__MoveOwn()
{
    // update rotation angle
    m_fAngle.Update(-3.0f);

    // rotate around direction axis
    const coreVector3& vDir = this->GetDirection();
    const coreVector2  vOri = coreVector2::Direction(m_fAngle);
    this->SetOrientation(coreVector3(-vOri.x*vDir.y, vOri.x*vDir.x, vOri.y));
}


// ****************************************************************
// constructor
cMinerEnemy::cMinerEnemy()noexcept
{
    // load models
    this->DefineModel   ("ship_enemy_miner_high.md3");
    this->DefineModelLow("ship_enemy_miner_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(183.0f/360.0f, 70.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// constructor
cFreezerEnemy::cFreezerEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_freezer_high.md3");
    this->DefineModelLow("ship_enemy_freezer_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(208.0f/360.0f, 32.0f/100.0f, 90.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// move the freezer enemy
void cFreezerEnemy::__MoveOwn()
{
    // update rotation angle (opposed to arrow enemy)
    m_fAngle.Update(2.0f);

    // rotate around direction axis
    const coreVector3& vDir = this->GetDirection();
    const coreVector2  vOri = coreVector2::Direction(m_fAngle);
    this->SetOrientation(coreVector3(-vOri.x*vDir.y, vOri.x*vDir.x, vOri.y));
}


// ****************************************************************
// constructor
cCinderEnemy::cCinderEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_cinder_high.md3");
    this->DefineModelLow("ship_enemy_cinder_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(0.0f/360.0f, 0.0f/100.0f, 60.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// move the cinder enemy
void cCinderEnemy::__MoveOwn()
{
    // update rotation angle
    m_fAngle.Update(-2.0f);

    // rotate around the rotating direction axis
    const coreVector2 vDir = coreVector2::Direction(m_fAngle);
    this->SetDirection  (coreVector3(vDir, 0.0f));
    this->SetOrientation(coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));
}