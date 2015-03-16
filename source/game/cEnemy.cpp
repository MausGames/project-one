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
: m_pPath       (NULL)
, m_vPathOffset (coreVector2(0.0f,0.0f))
, m_fLifeTime   (0.0f)
, m_iNumShots   (0)
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
void cEnemy::Configure(const coreInt32& iHealth, const coreVector3& vColor)
{
    // set maximum and current health value
    m_iMaxHealth = m_iCurHealth = iHealth;

    // save color value
    this->SetBaseColor(vColor);
}


// ****************************************************************
// 
void cEnemy::SetupTrack(coreSpline2* pPath, const coreVector2& vPathOffset)
{
    // 
    m_pPath       = pPath;
    m_vPathOffset = vPathOffset;
}


// ****************************************************************
// render the enemy
void cEnemy::Render()
{
    ASSERT(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD))

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
    m_iCurHealth -= iDamage;

    // 
    if(m_iCurHealth <= 0)
    {
        m_iCurHealth = 0;
        this->Kill(true);
        return;
    }

    // 
    this->SetColor3(LERP(coreVector3(0.5f,0.5f,0.5f), this->GetBaseColor(), (I_TO_F(m_iCurHealth) / I_TO_F(m_iMaxHealth))));
}


// ****************************************************************
// add enemy to the game
void cEnemy::Resurrect()
{
    ASSERT(m_pPath)
    ASSERT(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD))

    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    m_pPath->CalcPosDir(0.0f, &vPosition, &vDirection);

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
    m_iNumShots = 0;

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

    // unbind enemy from active list
    g_pGame->__UnbindEnemy(this);

    // remove ship from the game
    cShip::_Kill(bAnimated);
}


// ****************************************************************
// 
coreBool cEnemy::_DefaultShooting(const coreFloat& fFireRate)
{
    ASSERT(fFireRate <= 30.0f)

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
// 
coreBool cEnemy::_DefaultMovementPath(const coreFloat& fDistance)
{
    if(m_pPath)
    {
        // 
        if(fDistance >= m_pPath->GetTotalDistance())
        {
            this->Kill(false);
            return true;
        }

        // 
        coreVector2 vPosition;
        coreVector2 vDirection;
        m_pPath->CalcPosDir(fDistance, &vPosition, &vDirection);

        // 
        this->SetPosition (coreVector3(vPosition + m_vPathOffset, 0.0f));
        this->SetDirection(coreVector3(vDirection,                0.0f));
    }

    return false;
}


// ****************************************************************
// 
coreBool cEnemy::_DefaultMovementTarget(const coreVector2& vTarget, const coreFloat& fSpeedTurn, const coreFloat& fSpeedMove)
{
    // 
    const coreVector2 vDiff = vTarget - this->GetPosition().xy();
    if(vDiff.LengthSq() < 0.5f)
    {
        this->Kill(false);
        return true;
    }

    // 
    const coreVector2 vAim       = vDiff.Normalized();
    const coreVector2 vDirection = (this->GetDirection().xy() + vAim       * (fSpeedTurn * Core::System->GetTime())).Normalize();
    const coreVector2 vPosition  =  this->GetPosition ().xy() + vDirection * (fSpeedMove * Core::System->GetTime());

    // 
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));

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
// 
void cScoutEnemy::__MoveOwn()
{
    // 
    if(this->_DefaultMovementPath(30.0f * m_fLifeTime)) return;

    if(this->_DefaultShooting(10.0f))
    {
        cPlayer* pPlayer = g_pGame->FindPlayer(this->GetPosition().xy());

        g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(),
                                                           (pPlayer->GetPosition().xy() - this->GetPosition().xy()).Normalize());
    }

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
// 
void cWarriorEnemy::__MoveOwn()
{
    // 
    if(this->_DefaultMovementPath(10.0f * m_fLifeTime)) return;

    if(this->_DefaultShooting(5.0f))
    {
        cPlayer* pPlayer = g_pGame->FindPlayer(this->GetPosition().xy());
        const coreVector2 vPos = pPlayer->GetPosition().xy();
        const coreVector2 vDir = (vPos - this->GetPosition().xy()).Normalize();
        const coreVector2 vTan = vDir.yx();

        g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(),  vDir);
        g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), (vDir + vTan*0.1f).Normalize());
        g_pGame->GetBulletManager()->AddBullet<cOrbBullet>(5, this, TYPE_BULLET_ENEMY, this->GetPosition().xy(), (vDir - vTan*0.1f).Normalize());
    }

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