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
, m_nRoutine  (NULL)
{
    // load object resources
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_program");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,-1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f, 0.0f,1.0f));

    // set initial status
    m_iStatus = ENEMY_STATUS_DEAD | ENEMY_STATUS_READY;
    m_aiNumShots[1] = m_aiNumShots[0] = 0u;
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
// move the enemy
void cEnemy::Move()
{
    ASSERT(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD))

    // 
    m_fLifeTime.Update(1.0f);

    // call individual move routines
    this->__MoveOwn();
    if(m_nRoutine) m_nRoutine(this);

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// reduce current health
void cEnemy::TakeDamage(const coreInt32& iDamage, cPlayer* pAttacker)
{
    // 
    if(pAttacker)
    {
        pAttacker->AddCombo(1u);
        pAttacker->AddChain(1u);   // TODO # clamp on health 
    }

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
void cEnemy::Resurrect(const coreSpline2& oPath, const coreVector2& vFactor, const coreVector2& vOffset)
{
    ASSERT(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD))

    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    oPath.CalcPosDir(0.0f, &vPosition, &vDirection);

    // 
    this->Resurrect((vPosition * vFactor) + vOffset, (vDirection * vFactor).Normalize());
}

void cEnemy::Resurrect(const coreVector2& vPosition, const coreVector2& vDirection)
{
    // resurrect enemy
    if(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;
    REMOVE_VALUE(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    m_fLifeTime     = 0.0f;
    m_aiNumShots[1] = m_aiNumShots[0] = 0u;

    // 
    const coreBool bBoss = CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_BOSS);
    if(bBoss) g_pGame->GetEnemyManager()->BindEnemy(this);

    // add ship to the game
    cShip::_Resurrect(bBoss, vPosition, vDirection, TYPE_ENEMY);

    // 
    this->__ResurrectOwn();
}


// ****************************************************************
// remove enemy from the game
void cEnemy::Kill(const coreBool& bAnimated)
{
    // kill enemy
    if(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;
    ADD_VALUE(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    m_nRoutine = NULL;

    // 
    if(bAnimated)
    {
        if(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_BOSS))
             g_pSpecialEffects->MacroExplosionPhysicalBig  (this->GetPosition());
        else g_pSpecialEffects->MacroExplosionPhysicalSmall(this->GetPosition());
    }

    // 
    const coreBool bBoss = CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_BOSS);
    if(bBoss) g_pGame->GetEnemyManager()->UnbindEnemy(this);

    // remove ship from the game
    cShip::_Kill(bBoss, bAnimated);

    // 
    this->__KillOwn();
}


// ****************************************************************
// 
coreBool cEnemy::DefaultMovePath(const coreSpline2& oPath, const coreVector2& vFactor, const coreVector2& vOffset, const coreFloat& fDistance)
{
    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    oPath.CalcPosDir(MIN(fDistance, oPath.GetTotalDistance()), &vPosition, &vDirection);

    // 
    this->SetPosition (coreVector3((vPosition  * vFactor) + vOffset,   0.0f));
    this->SetDirection(coreVector3((vDirection * vFactor).Normalize(), 0.0f));

    // 
    return (fDistance >= oPath.GetTotalDistance()) ? true : false;
}


// ****************************************************************
// 
coreBool cEnemy::DefaultMoveTarget(const coreVector2& vTarget, const coreFloat& fSpeedMove, const coreFloat& fSpeedTurn)
{
    ASSERT((fSpeedMove >= 0.0f) && (fSpeedTurn >= 0.0f))

    // 
    const coreVector2 vDiff = vTarget - this->GetPosition().xy();
    const coreVector2 vAim  = vDiff.Normalized();

    // 
    const coreVector2 vDirection = (this->GetDirection().xy() + vAim       * (fSpeedTurn * Core::System->GetTime())).Normalize();
    const coreVector2 vPosition  =  this->GetPosition ().xy() + vDirection * (fSpeedMove * Core::System->GetTime());

    // 
    this->SetPosition (coreVector3(vPosition,  0.0f));
    if(vDiff.LengthSq() < 0.1f) return true;
    this->SetDirection(coreVector3(vDirection, 0.0f));

    return false;
}


// ****************************************************************
// 
coreBool cEnemy::DefaultMoveSmooth(const coreVector2& vPosition, const coreFloat& fSpeedMove, const coreFloat& fClampMove)
{
    ASSERT((fSpeedMove >= 0.0f) && (fClampMove >= 0.0f))

    // 
    const coreVector2 vDiff = vPosition - this->GetPosition().xy();
    const coreVector2 vAim  = vDiff.Normalized();
    const coreFloat   fLen  = MIN(vDiff.Length() * fSpeedMove, fClampMove);

    // 
    const coreVector2 vNewPos = this->GetPosition().xy() + vAim * (fLen * Core::System->GetTime());

    // 
    this->SetPosition(coreVector3(vNewPos, 0.0f));
    if(fLen < 0.1f) return true;

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
coreBool cEnemy::DefaultRotateSmooth(const coreVector2& vDirection, const coreFloat& fSpeedTurn, const coreFloat& fClampTurn)
{
    ASSERT(vDirection.IsNormalized() && (fSpeedTurn >= 0.0f) && (fClampTurn >= 0.0f))

    return true;
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
coreBool cEnemy::DefaultShoot(const coreFloat& fFireRate, const coreUint8& iMaxShots)
{
    ASSERT(fFireRate <= FRAMERATE_VALUE)

    // 
    if(!CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_SILENT))
    {
        // 
        if(m_aiNumShots[0] >= iMaxShots) return false;

        // 
        const coreUint8 iCurShots = F_TO_UI(m_fLifeTime * fFireRate) & 0xFFu;
        if(m_aiNumShots[1] != iCurShots)
        {
            m_aiNumShots[1] = iCurShots;

            // 
            ++m_aiNumShots[0];
            return true;
        }
    }

    return false;
}


// ****************************************************************
// constructor
cEnemyManager::sEnemySetGen::sEnemySetGen()noexcept
: oEnemyActive (ENEMY_SET_INIT_SIZE)
{
}


// ****************************************************************
// constructor
cEnemyManager::cEnemyManager()noexcept
{
}


// ****************************************************************
// destructor
cEnemyManager::~cEnemyManager()
{
    ASSERT(m_apAdditional.empty())

    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // remove enemy set from global shadow and outline
        cShadow::GetGlobalContainer()->UnbindList(pEnemyActive);
        g_aaOutline[PRIO_WEAK][STYLE_FULL].UnbindList(pEnemyActive);

        // delete enemy set
        SAFE_DELETE(*it)
    }

    // clear memory
    m_apEnemySet  .clear();
    m_apAdditional.clear();
}


// ****************************************************************
// render the enemy manager
void cEnemyManager::Render()
{
    // loop through all enemy sets
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        if(!pEnemyActive->GetCurEnabled()) continue;

        // 
        FOR_EACH(et, *pEnemyActive->List())
            s_cast<cEnemy*>(*et)->_UpdateBlink();

        FOR_EACH(et, *pEnemyActive->List())
            (*et)->DefineModel(s_cast<cEnemy*>(*et)->GetModelHigh());
        //pEnemyActive->DefineModelOver(NULL);

        // render enemy set
        pEnemyActive->Render();

        FOR_EACH(et, *pEnemyActive->List())
            (*et)->DefineModel(s_cast<cEnemy*>(*et)->GetModelLow());

        //pEnemyActive->DefineModelOver(s_cast<cShip*>(pEnemyActive->List()->front())->GetModelLow());
    }

    // 
    FOR_EACH(it, m_apAdditional)
    {
        cEnemy* pEnemy = (*it);

        pEnemy->_UpdateBlink();
        pEnemy->Render();
    }
}

void cEnemyManager::RenderWeak()
{
    // 
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // 
        FOR_EACH(et, *pEnemyActive->List())
            s_cast<cEnemy*>(*et)->__RenderOwnWeak();
    }

    // 
    FOR_EACH(it, m_apAdditional)
        (*it)->__RenderOwnWeak();
}

void cEnemyManager::RenderStrong()
{
    // 
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // 
        FOR_EACH(et, *pEnemyActive->List())
            s_cast<cEnemy*>(*et)->__RenderOwnStrong();
    }

    // 
    FOR_EACH(it, m_apAdditional)
        (*it)->__RenderOwnStrong();
}

void cEnemyManager::RenderAfter()
{
    // 
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // 
        FOR_EACH(et, *pEnemyActive->List())
            s_cast<cEnemy*>(*et)->__RenderOwnAfter();
    }

    // 
    FOR_EACH(it, m_apAdditional)
        (*it)->__RenderOwnAfter();
}


// ****************************************************************
// move the enemy manager
void cEnemyManager::Move()
{
    // loop through all enemy sets
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // loop through all enemies
        FOR_EACH_DYN(et, *pEnemyActive->List())
        {
            coreObject3D* pEnemy = (*et);

            // check current enemy status
            if(CONTAINS_VALUE(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))
            {
                // clean up enemy and make ready again
                pEnemy->SetStatus(pEnemy->GetStatus() | ENEMY_STATUS_READY);
                DYN_REMOVE(et, *pEnemyActive->List())
            }
            else DYN_KEEP(et)
        }

        // move the enemy set (after deletions)
        pEnemyActive->MoveNormal();
    }

    // 
    auto apCopy = m_apAdditional;
    FOR_EACH(it, apCopy)
        (*it)->Move();
}


// ****************************************************************
// remove all enemies
void cEnemyManager::ClearEnemies(const coreBool& bAnimated)
{
    // loop trough all enemy sets
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // deactivate each active enemy
        FOR_EACH(et, *pEnemyActive->List())
            s_cast<cEnemy*>(*et)->Kill(bAnimated);

        // clear list
        pEnemyActive->Clear();
    }

    // 
    auto apCopy = m_apAdditional;
    FOR_EACH(it, apCopy)
        (*it)->Kill(bAnimated);

    // 
    m_apAdditional.clear();
}


// ****************************************************************
// 
cEnemy* cEnemyManager::FindEnemy(const coreVector2& vPosition)
{
    // 
    cEnemy*   pEnemy = NULL;
    coreFloat fLenSq = 1.0e06f;

    // 
    const auto& oEnemyList = Core::Manager::Object->GetObjectList(TYPE_ENEMY);
    FOR_EACH(it, oEnemyList)
    {
        cEnemy* pCurEnemy = s_cast<cEnemy*>(*it);
        if(!pCurEnemy) continue;

        // 
        const coreFloat fCurLenSq = (pCurEnemy->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq < fLenSq)
        {
            // 
            pEnemy = pCurEnemy;
            fLenSq = fCurLenSq;
        }
    }

    // 
    return pEnemy;
}


// ****************************************************************
// constructor
cScoutEnemy::cScoutEnemy()noexcept
{
    // load models
    this->DefineModelHigh("ship_enemy_scout_high.md3");
    this->DefineModelLow ("ship_enemy_warrior_low.md3");

    // configure the enemy
    this->Configure(10, coreVector3(201.0f/360.0f, 74.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// constructor
cWarriorEnemy::cWarriorEnemy()noexcept
{
    // load models
    this->DefineModelHigh("ship_enemy_warrior_high.md3");
    this->DefineModelLow ("ship_enemy_warrior_low.md3");

    // configure the enemy
    this->Configure(400, coreVector3(51.0f/360.0f, 100.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// constructor
cStarEnemy::cStarEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_star_high.md3");
    this->DefineModelLow ("ship_enemy_star_low.md3");

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
    this->DefineModelHigh("ship_enemy_arrow_high.md3");
    this->DefineModelLow ("ship_enemy_arrow_low.md3");

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
    this->DefineModelHigh("ship_enemy_miner_high.md3");
    this->DefineModelLow ("ship_enemy_miner_low.md3");

    // configure the enemy
    this->Configure(100, coreVector3(183.0f/360.0f, 70.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// constructor
cFreezerEnemy::cFreezerEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_freezer_high.md3");
    this->DefineModelLow ("ship_enemy_freezer_low.md3");

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
    this->DefineModelHigh("ship_enemy_cinder_high.md3");
    this->DefineModelLow ("ship_enemy_cinder_low.md3");

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