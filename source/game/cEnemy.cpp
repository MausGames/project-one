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
: m_fLifeTime (0.0f)
, m_iNumShots (0u)
{
    // load object resources
    this->DefineProgram("object_ship_program");
    this->DefineTexture(0u, "ship_enemy.png");

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
void cEnemy::Configure(const coreInt32& iHealth, const coreVector3& vColor)
{
    // set maximum and current health value
    m_iMaxHealth = m_iCurHealth = iHealth;

    // save color value
    this->SetBaseColor(vColor);
}


// ****************************************************************
// render the enemy
void cEnemy::Render()
{
    ASSERT(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD))

    // 
    this->_UpdateBlink();

    // render the 3d-object
    coreObject3D::Render();
}


// ****************************************************************
// move the enemy
void cEnemy::Move()
{
    ASSERT(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD))

    // 
    m_fLifeTime.Update(1.0f);

    // call individual move routine
    this->__MoveOwn();

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// reduce current health
void cEnemy::TakeDamage(const coreInt32& iDamage, cPlayer* pAttacker)
{
    // 
    if(this->_TakeDamage(iDamage))
    {
        // 
        this->Kill(true);
        return;
    }
}


// ****************************************************************
// add enemy to the game
void cEnemy::Resurrect(const coreSpline2& oPath)
{
    ASSERT(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD))

    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    oPath.CalcPosDir(0.0f, &vPosition, &vDirection);

    // 
    this->Resurrect(vPosition, vDirection);
}

void cEnemy::Resurrect(const coreVector2& vPosition, const coreVector2& vDirection)
{
    // resurrect enemy
    if(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;
    REMOVE_VALUE(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    m_fLifeTime = 0.0f;
    m_iNumShots = 0u;

    // 
    this->__ResurrectOwn();

    // bind enemy to active list
    g_pGame->__BindEnemy(this);

    // add ship to the game
    cShip::_Resurrect(vPosition, vDirection, TYPE_ENEMY);
}


// ****************************************************************
// remove enemy from the game
void cEnemy::Kill(const coreBool& bAnimated)
{
    // kill enemy
    if(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;
    ADD_VALUE(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    this->__KillOwn();

    // unbind enemy from active list
    g_pGame->__UnbindEnemy(this);

    // remove ship from the game
    cShip::_Kill(bAnimated);
}


// ****************************************************************
// 
coreBool cEnemy::DefaultMovePath(const coreSpline2& oPath, const coreVector2& vOffset, const coreFloat& fDistance)
{
    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    oPath.CalcPosDir(MIN(fDistance, oPath.GetTotalDistance()), &vPosition, &vDirection);

    // 
    this->SetPosition (coreVector3(vPosition + vOffset, 0.0f));
    this->SetDirection(coreVector3(vDirection,          0.0f));

    // 
    return (fDistance >= oPath.GetTotalDistance()) ? true : false;
}


// ****************************************************************
// 
coreBool cEnemy::DefaultMoveTarget(const coreVector2& vTarget, const coreFloat& fSpeedTurn, const coreFloat& fSpeedMove)
{
    // 
    const coreVector2 vDiff = vTarget - this->GetPosition().xy();
    const coreVector2 vAim  = vDiff.Normalized();

    // 
    const coreVector2 vDirection = (this->GetDirection().xy() + vAim       * (fSpeedTurn * Core::System->GetTime())).Normalize();
    const coreVector2 vPosition  =  this->GetPosition ().xy() + vDirection * (fSpeedMove * Core::System->GetTime());

    // 
    this->SetPosition (coreVector3(vPosition,  0.0f));
    if(vDiff.LengthSq() < 0.5f) return true;
    this->SetDirection(coreVector3(vDirection, 0.0f));

    return false;
}


// ****************************************************************
// 
void cEnemy::DefaultMoveLerp(const coreVector2& vFromRawPos, const coreVector2& vToRawPos, const coreFloat& fTime)
{
    // 
    this->SetPosition(coreVector3(LERP(vFromRawPos, vToRawPos, fTime) * FOREGROUND_AREA, 0.0f));
}


// ****************************************************************
// 
void cEnemy::DefaultRotate(const coreFloat& fAngle)
{
    // rotate around z-axis
    const coreVector2 vDir = coreVector2::Direction(fAngle);
    this->SetDirection(coreVector3(vDir, 0.0f));
}


// ****************************************************************
// 
void cEnemy::DefaultRotateLerp(const coreFloat& fFromAngle, const coreFloat& fToAngle, const coreFloat& fTime)
{
    // rotate around z-axis
    this->DefaultRotate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
void cEnemy::DefaultOrientate(const coreFloat& fAngle)
{
    // rotate around direction axis
    const coreVector3& vDir = this->GetDirection();
    const coreVector2  vOri = coreVector2::Direction(fAngle);
    this->SetOrientation(coreVector3(-vOri.x*vDir.y, vOri.x*vDir.x, vOri.y));
}


// ****************************************************************
// 
void cEnemy::DefaultOrientateLerp(const coreFloat& fFromAngle, const coreFloat& fToAngle, const coreFloat& fTime)
{
    // rotate around direction axis
    this->DefaultOrientate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
void cEnemy::DefaultMultiate(const coreFloat& fAngle)
{
    // rotate around the rotating direction axis
    const coreVector2 vDir = coreVector2::Direction(fAngle);
    this->SetDirection  (coreVector3(vDir, 0.0f));
    this->SetOrientation(coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));
}


// ****************************************************************
// 
coreBool cEnemy::DefaultShoot(const coreFloat& fFireRate)
{
    ASSERT(fFireRate <= I_TO_F(FRAMERATE))

    // 
    if(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_SILENT))
    {
        // 
        const coreUint8 iCurShots = F_TO_UI(m_fLifeTime * fFireRate) & 0xFF;
        if(m_iNumShots != iCurShots)
        {
            // 
            ++m_iNumShots;
            ASSERT(m_iNumShots == iCurShots)

            // 
            return true;
        }
    }

    return false;
}


// ****************************************************************
// constructor
cScoutEnemy::cScoutEnemy()noexcept
{
    // load models
    this->DefineModel   ("ship_enemy_scout_high.md3");
    this->DefineModelLow("ship_enemy_scout_low.md3");

    // configure the enemy
    this->Configure(30, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
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
    this->DefaultRotate(m_fAngle);
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
    this->DefaultOrientate(m_fAngle);
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
    this->DefaultOrientate(m_fAngle);
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
    this->DefaultMultiate(m_fAngle);
}