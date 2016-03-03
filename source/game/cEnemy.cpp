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
{
    // load object resources
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_program");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,-1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f, 0.0f,1.0f));

    // set initial status
    m_iStatus = ENEMY_STATUS_DEAD;
}


// ****************************************************************
// configure the enemy
void cEnemy::Configure(const coreInt32 iHealth, const coreVector3& vColor)
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
    if(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;

    // 
    m_fLifeTime.Update(1.0f);

    // call individual move routines
    this->__MoveOwn();

    // 
    this->_UpdateBlink();

    // move the 3d-object
    coreObject3D::Move();
}


// ****************************************************************
// reduce current health
void cEnemy::TakeDamage(const coreInt32 iDamage, cPlayer* pAttacker)
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
void cEnemy::Resurrect()
{
    // 
    this->Resurrect(coreVector2(FLT_MAX,FLT_MAX), coreVector2(0.0f,-1.0f));
}

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
    const coreBool bBoss = CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_BOSS);
    const coreBool bMute = CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_MUTE);

    // 
    m_fLifeTime = 0.0f;

    // add ship to the game
    cShip::_Resurrect(bBoss || bMute, vPosition, vDirection, bMute ? 0 : TYPE_ENEMY);

    // 
    this->__ResurrectOwn();
}


// ****************************************************************
// remove enemy from the game
void cEnemy::Kill(const coreBool bAnimated)
{
    // kill enemy
    if(CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_DEAD)) return;
    ADD_VALUE(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    const coreBool bBoss = CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_BOSS);
    const coreBool bMute = CONTAINS_VALUE(m_iStatus, ENEMY_STATUS_MUTE);

    // 
    if(bAnimated)
    {
        if(bBoss) g_pSpecialEffects->MacroExplosionPhysicalBig  (this->GetPosition());
             else g_pSpecialEffects->MacroExplosionPhysicalSmall(this->GetPosition());
    }

    // remove ship from the game
    cShip::_Kill(bBoss || bMute, bAnimated);

    // 
    this->__KillOwn(bAnimated);
}


// ****************************************************************
// 
coreBool cEnemy::DefaultMovePath(const coreSpline2& oRawPath, const coreVector2& vFactor, const coreVector2& vRawOffset, const coreFloat fRawDistance)
{
    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    oRawPath.CalcPosDir(MIN(fRawDistance, oRawPath.GetTotalDistance()), &vPosition, &vDirection);

    // 
    this->SetPosition (coreVector3(((vPosition  * vFactor) + vRawOffset) * FOREGROUND_AREA, 0.0f));
    this->SetDirection(coreVector3( (vDirection * vFactor).Normalize(),                     0.0f));

    // 
    return (fRawDistance >= oRawPath.GetTotalDistance()) ? true : false;
}


// ****************************************************************
// 
coreBool cEnemy::DefaultMoveTarget(const coreVector2& vTarget, const coreFloat fSpeedMove, const coreFloat fSpeedTurn)
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
coreBool cEnemy::DefaultMoveSmooth(const coreVector2& vRawPosition, const coreFloat fSpeedMove, const coreFloat fClampMove)
{
    ASSERT((fSpeedMove >= 0.0f) && (fClampMove >= 0.0f))

    // 
    const coreVector2 vDiff = vRawPosition * FOREGROUND_AREA - this->GetPosition().xy();
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
void cEnemy::DefaultMoveLerp(const coreVector2& vFromRawPos, const coreVector2& vToRawPos, const coreFloat fTime)
{
    // 
    this->SetPosition(coreVector3(LERP(vFromRawPos, vToRawPos, fTime) * FOREGROUND_AREA, 0.0f));
}


// ****************************************************************
// 
void cEnemy::DefaultRotate(const coreFloat fAngle)
{
    // rotate around z-axis
    const coreVector2 vDir = coreVector2::Direction(fAngle);
    this->SetDirection(coreVector3(vDir, 0.0f));
}


// ****************************************************************
// 
coreBool cEnemy::DefaultRotateSmooth(const coreVector2& vDirection, const coreFloat fSpeedTurn, const coreFloat fClampTurn)
{
    ASSERT(vDirection.IsNormalized() && (fSpeedTurn >= 0.0f) && (fClampTurn >= 0.0f))

    // TODO # 
    ASSERT(false)
    return true;
}


// ****************************************************************
// 
void cEnemy::DefaultRotateLerp(const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime)
{
    // rotate around z-axis
    this->DefaultRotate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
void cEnemy::DefaultOrientate(const coreFloat fAngle)
{
    // rotate around direction axis
    const coreVector3& vDir = this->GetDirection();
    const coreVector2  vOri = coreVector2::Direction(fAngle);
    this->SetOrientation(coreVector3(-vOri.x*vDir.y, vOri.x*vDir.x, vOri.y));
}


// ****************************************************************
// 
void cEnemy::DefaultOrientateLerp(const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime)
{
    // rotate around direction axis
    this->DefaultOrientate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
void cEnemy::DefaultMultiate(const coreFloat fAngle)
{
    // rotate around the rotating direction axis
    const coreVector2 vDir = coreVector2::Direction(fAngle);
    this->SetDirection  (coreVector3(vDir, 0.0f));
    this->SetOrientation(coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));
}


// ****************************************************************
// destructor
cEnemySquad::~cEnemySquad()
{
    // 
    this->FreeEnemies();
}


// ****************************************************************
// 
void cEnemySquad::FreeEnemies()
{
    // 
    FOR_EACH(it, m_apEnemy)
        g_pGame->GetEnemyManager()->FreeEnemy(&(*it));

    // clear memory
    m_apEnemy.clear();
}


// ****************************************************************
// 
void cEnemySquad::ClearEnemies(const coreBool bAnimated)
{
    // 
    FOR_EACH(it, m_apEnemy)
        (*it)->Kill(bAnimated);
}


// ****************************************************************
// 
cEnemy* cEnemySquad::FindEnemy(const coreVector2& vPosition)
{
    // 
    cEnemy*   pEnemy = NULL;
    coreFloat fLenSq = FLT_MAX;

    // 
    this->ForEachEnemy([&](cEnemy* OUTPUT pCurEnemy, const coreUintW i)
    {
        // 
        const coreFloat fCurLenSq = (pCurEnemy->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq < fLenSq)
        {
            // 
            pEnemy = pCurEnemy;
            fLenSq = fCurLenSq;
        }
    });

    return pEnemy;
}


// ****************************************************************
// constructor
cEnemyManager::sEnemySetGen::sEnemySetGen()noexcept
: oEnemyActive (ENEMY_SET_INIT_SIZE)
, iTopEnemy    (0u)
{
}


// ****************************************************************
// destructor
cEnemyManager::~cEnemyManager()
{
    ASSERT(m_apAdditional.empty())

    // 
    FOR_EACH(it, m_apEnemySet)
        SAFE_DELETE(*it)

    // clear memory
    m_apEnemySet  .clear();
    m_apAdditional.clear();
}


// ****************************************************************
// render the enemy manager
void cEnemyManager::Render()
{
    // 
    auto nRenderFunc = [](cEnemy* OUTPUT pEnemy)
    {
        if(CONTAINS_VALUE(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))
            return;

        pEnemy->_EnableBlink();
        pEnemy->Render();
    };

    // loop through all enemy sets
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;
        if(!pEnemyActive->GetCurEnabled()) continue;

        // 
        FOR_EACH(et, *pEnemyActive->List()) (*et)->DefineModel(s_cast<cShip*>(*et)->GetModelHigh());
        {
            if(pEnemyActive->IsInstanced())
            {
                // 
                pEnemyActive->UpdateCustom([](coreFloat* OUTPUT pData, const cShip* pObject)
                {
                    *pData = pObject->GetBlink();
                });
                pEnemyActive->Render();
            }
            else
            {
                // 
                FOR_EACH(et, *pEnemyActive->List())
                    nRenderFunc(s_cast<cEnemy*>(*et));
            }
        }
        FOR_EACH(et, *pEnemyActive->List()) (*et)->DefineModel(s_cast<cShip*>(*et)->GetModelLow());
    }

    // 
    FOR_EACH(it, m_apAdditional)
        nRenderFunc(*it);
}

#define __RENDER_OWN(f)                                            \
{                                                                  \
    /* */                                                          \
    auto nRenderFunc = [](cEnemy* OUTPUT pEnemy)                   \
    {                                                              \
        if(CONTAINS_VALUE(pEnemy->GetStatus(), ENEMY_STATUS_DEAD)) \
            return;                                                \
                                                                   \
        pEnemy->f();                                               \
    };                                                             \
                                                                   \
    /* loop through all enemy sets */                              \
    FOR_EACH(it, m_apEnemySet)                                     \
    {                                                              \
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;        \
                                                                   \
        /* render all active enemies */                            \
        FOR_EACH(et, *pEnemyActive->List())                        \
            nRenderFunc(s_cast<cEnemy*>(*et));                     \
    }                                                              \
                                                                   \
    /* render all additional enemies */                            \
    FOR_EACH(it, m_apAdditional)                                   \
        nRenderFunc(*it);                                          \
}  

void cEnemyManager::RenderUnder () {__RENDER_OWN(__RenderOwnUnder)}
void cEnemyManager::RenderAttack() {__RENDER_OWN(__RenderOwnAttack)}
void cEnemyManager::RenderOver  () {__RENDER_OWN(__RenderOwnOver)}

#undef __RENDER_OWN


// ****************************************************************
// move the enemy manager
void cEnemyManager::Move()
{
    // loop through all enemy sets
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // move the enemy set
        pEnemyActive->MoveNormal();
    }

    // 
    FOR_EACH(it, m_apAdditional)
        (*it)->Move();
}


// ****************************************************************
// 
void cEnemyManager::FreeEnemy(cEnemy** OUTPUT ppEnemy)
{
    cEnemy*       pEnemy    = *ppEnemy;
    sEnemySetGen* pEnemySet = m_apEnemySet[pEnemy->GetID()];

    // 
    pEnemy->Kill(false);
    pEnemy->RemoveStatus(ENEMY_STATUS_ASSIGNED);
    pEnemySet->oEnemyActive.UnbindObject(pEnemy);

    // 
    pEnemySet->iTopEnemy = 0u;

    // 
    *ppEnemy = NULL;
}


// ****************************************************************
// remove all enemies
void cEnemyManager::ClearEnemies(const coreBool bAnimated)
{
    // loop trough all enemy sets
    FOR_EACH(it, m_apEnemySet)
    {
        coreBatchList* pEnemyActive = &(*it)->oEnemyActive;

        // deactivate all active enemies
        FOR_EACH(et, *pEnemyActive->List())
            s_cast<cEnemy*>(*et)->Kill(bAnimated);

        // clear list
        pEnemyActive->Clear();
    }

    // deactivate all additional enemies
    FOR_EACH(it, m_apAdditional)
        (*it)->Kill(bAnimated);
}


// ****************************************************************
// 
cEnemy* cEnemyManager::FindEnemy(const coreVector2& vPosition)
{
    // 
    cEnemy*   pEnemy = NULL;
    coreFloat fLenSq = FLT_MAX;

    // 
    this->ForEachEnemy([&](cEnemy* OUTPUT pCurEnemy)
    {
        // 
        const coreFloat fCurLenSq = (pCurEnemy->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq < fLenSq)
        {
            // 
            pEnemy = pCurEnemy;
            fLenSq = fCurLenSq;
        }
    });

    return pEnemy;
}


// ****************************************************************
// constructor
cScoutEnemy::cScoutEnemy()noexcept
{
    // load models
    this->DefineModelHigh("ship_enemy_scout_high.md3");
    this->DefineModelLow ("ship_enemy_scout_low.md3");

    // configure the enemy
    this->Configure(100, COLOR_SHIP_BLUE);
}


// ****************************************************************
// constructor
cWarriorEnemy::cWarriorEnemy()noexcept
{
    // load models
    this->DefineModelHigh("ship_enemy_warrior_high.md3");
    this->DefineModelLow ("ship_enemy_warrior_low.md3");

    // configure the enemy
    this->Configure(100, COLOR_SHIP_YELLOW);
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
    this->Configure(100, COLOR_SHIP_RED);
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
    this->Configure(100, COLOR_SHIP_ORANGE);
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
    this->Configure(100, COLOR_SHIP_CYAN);
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
    this->Configure(100, COLOR_SHIP_ICE);
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
    this->Configure(100, COLOR_SHIP_GREY);
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