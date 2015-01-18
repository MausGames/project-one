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
    this->SetPosition   (coreVector3(1000.0f, 0.0f,0.0f));
    this->SetDirection  (coreVector3(   0.0f,-1.0f,0.0f));
    this->SetOrientation(coreVector3(   0.0f, 0.0f,1.0f));

    // add enemy to global shadow and outline
    cShadow::BindGlobalObject(this);
    g_pOutline->BindObject(this);


    this->ChangeType(TYPE_ENEMY);
}


// ****************************************************************
// destructor
cEnemy::~cEnemy()
{
    // remove enemy from global shadow and outline
    cShadow::UnbindGlobalObject(this);
    g_pOutline->UnbindObject(this);


    this->ChangeType(0);
}


// ****************************************************************
// render the enemy
void cEnemy::Render()
{
    // call individual render routine
    this->__RenderOwn();

    // render the 3d-object
    coreObject3D::Render();
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
// constructor
cScout::cScout()noexcept
{
    // load models
    this->DefineModel   ("ship_enemy_scout_high.md3");
    this->DefineModelLow("ship_enemy_scout_low.md3");

    // set color value
    this->SetColor3(coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cScout::~cScout()
{
}


// ****************************************************************
// constructor
cWarrior::cWarrior()noexcept
{
    // load models
    this->DefineModel   ("ship_enemy_warrior_high.md3");
    this->DefineModelLow("ship_enemy_warrior_low.md3");

    // set color value
    this->SetColor3(coreVector3(51.0f/360.0f, 100.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cWarrior::~cWarrior()
{
}


// ****************************************************************
// constructor
cStar::cStar()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_star_high.md3");
    this->DefineModelLow("ship_enemy_star_low.md3");

    // set color value
    this->SetColor3(coreVector3(0.0f/360.0f, 68.0f/100.0f, 90.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cStar::~cStar()
{
}


// ****************************************************************
// move the star enemy
void cStar::__MoveOwn()
{
    // update rotation angle
    m_fAngle.Update(-3.0f);

    // rotate around z-axis
    const coreVector2 vDir = coreVector2::Direction(m_fAngle);
    this->SetDirection(coreVector3(vDir, 0.0f));
}


// ****************************************************************
// constructor
cArrow::cArrow()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_arrow_high.md3");
    this->DefineModelLow("ship_enemy_arrow_low.md3");

    // set color value
    this->SetColor3(coreVector3(34.0f/360.0f, 100.0f/100.0f, 100.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cArrow::~cArrow()
{
}


// ****************************************************************
// move the arrow enemy
void cArrow::__MoveOwn()
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
cMiner::cMiner()noexcept
{
    // load models
    this->DefineModel   ("ship_enemy_miner_high.md3");
    this->DefineModelLow("ship_enemy_miner_low.md3");

    // set color value
    this->SetColor3(coreVector3(183.0f/360.0f, 70.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cMiner::~cMiner()
{
}


// ****************************************************************
// constructor
cFreezer::cFreezer()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_freezer_high.md3");
    this->DefineModelLow("ship_enemy_freezer_low.md3");

    // set color value
    this->SetColor3(coreVector3(208.0f/360.0f, 32.0f/100.0f, 90.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cFreezer::~cFreezer()
{
}


// ****************************************************************
// move the freezer enemy
void cFreezer::__MoveOwn()
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
cCinder::cCinder()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModel   ("ship_enemy_cinder_high.md3");
    this->DefineModelLow("ship_enemy_cinder_low.md3");

    // set color value
    this->SetColor3(coreVector3(0.0f/360.0f, 0.0f/100.0f, 60.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cCinder::~cCinder()
{
}


// ****************************************************************
// move the cinder enemy
void cCinder::__MoveOwn()
{
    // update rotation angle
    m_fAngle.Update(-2.0f);

    // rotate around the rotating direction axis
    const coreVector2 vDir = coreVector2::Direction(m_fAngle);
    this->SetDirection  (coreVector3(vDir, 0.0f));
    this->SetOrientation(coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));
}