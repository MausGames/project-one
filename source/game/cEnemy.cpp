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
: m_fLifeTime       (0.0f)
, m_fLifeTimeBefore (0.0f)
{
    // load object resources
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_program");

    // set object properties
    this->SetSize       (coreVector3(1.0f, 1.0f,1.0f) * ENEMY_SIZE_FACTOR);
    this->SetDirection  (coreVector3(0.0f,-1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f, 0.0f,1.0f));

    // set initial status
    m_iStatus = ENEMY_STATUS_DEAD;
}


// ****************************************************************
// configure the enemy
void cEnemy::Configure(const coreInt32 iHealth, const coreVector3& vColor)
{
    // set health and color value
    this->SetMaxHealth(iHealth);
    this->SetBaseColor(vColor);
}


// ****************************************************************
// 
void cEnemy::GiveShield(const coreUint8 iElement, const coreInt16 iHealth)
{
    // 
    g_pGame->GetShieldManager()->BindEnemy(this, iElement, iHealth);
}


// ****************************************************************
// render the enemy
void cEnemy::Render()
{
    if(!CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))
    {
        // 
        this->_EnableBlink();

        // render the 3d-object
        this->coreObject3D::Render();
    }
}


// ****************************************************************
// move the enemy
void cEnemy::Move()
{
    // 
    this->_UpdateAlwaysBefore();

    if(!CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))
    {
        // 
        m_fLifeTimeBefore = m_fLifeTime;
        m_fLifeTime.Update(1.0f);

        // call individual move routines
        this->__MoveOwn();

        // move the 3d-object
        this->coreObject3D::Move();
    }

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// reduce current health
coreBool cEnemy::TakeDamage(coreInt32 iDamage, const coreUint8 iElement, const coreVector2& vImpact, cPlayer* pAttacker)
{
    // 
    if(this->IsChild()) return m_apMember.front()->TakeDamage(iDamage, iElement, vImpact, pAttacker);

    // 
    if(!CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_INVINCIBLE))
    {
        // 
        g_pGame->GetShieldManager()->AbsorbDamage(this, &iDamage, iElement);
        if(iDamage)
        {
            // 
            if(pAttacker)
            {
                const coreUint32 iValue = ABS(CLAMP(iDamage, this->GetCurHealth() - this->GetMaxHealth(), this->GetCurHealth()));

                pAttacker->GetScoreTable()->AddCombo(iValue);
                pAttacker->GetScoreTable()->AddChain(iValue);
            }

            // 
            if(this->_TakeDamage(iDamage, iElement, vImpact))
            {
                if(!CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_IMMORTAL))
                    this->Kill(true);

                return true;
            }

            // 
            if(this->IsParent())
            {
                FOR_EACH(it, m_apMember)
                {
                    (*it)->RefreshColor(this->GetCurHealthPct());
                    (*it)->InvokeBlink();
                }
            }
        }
    }

    return false;
}


// ****************************************************************
// add enemy to the game
void cEnemy::Resurrect()
{
    // 
    this->Resurrect(coreVector2(FLT_MAX,FLT_MAX), coreVector2(0.0f,-1.0f));
}

void cEnemy::Resurrect(const coreSpline2* pPath, const coreVector2& vFactor, const coreVector2& vOffset)
{
    ASSERT(CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))

    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    pPath->CalcPosDir(0.0f, &vPosition, &vDirection);

    // 
    this->Resurrect((vPosition * vFactor) + vOffset, (vDirection * vFactor).Normalized());
}

void cEnemy::Resurrect(const coreVector2& vPosition, const coreVector2& vDirection)
{
    // resurrect enemy
    if(!CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    const coreBool bBoss   = CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_BOSS);
    const coreBool bSingle = CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_SINGLE);

    // 
    m_fLifeTime       = 0.0f;
    m_fLifeTimeBefore = 0.0f;

    // add ship to the game
    this->_Resurrect(bSingle, vPosition, vDirection, TYPE_ENEMY);

    // 
    this->__ResurrectOwn();

    // 
    if(this->IsParent()) FOR_EACH(it, m_apMember) (*it)->Resurrect(vPosition, vDirection);
}


// ****************************************************************
// remove enemy from the game
void cEnemy::Kill(const coreBool bAnimated)
{
    // kill enemy
    if(CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_DEAD)) return;
    ADD_FLAG(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    const coreBool bBoss   = CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_BOSS);
    const coreBool bSingle = CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_SINGLE);

    // 
    g_pGame->GetShieldManager()->UnbindEnemy(this);

    // 
    if(bAnimated && this->IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        if(bBoss) g_pSpecialEffects->MacroExplosionPhysicalBig  (this->GetPosition());
             else g_pSpecialEffects->MacroExplosionPhysicalSmall(this->GetPosition());
    }

    // remove ship from the game
    this->_Kill(bSingle, bAnimated);

    // 
    this->__KillOwn(bAnimated);

    // 
    if(this->IsParent()) FOR_EACH(it, m_apMember) (*it)->Kill(false);   // # never animate
}


// ****************************************************************
// 
cPlayer* cEnemy::NearestPlayer()const
{
    return g_pGame->FindPlayer(this->GetPosition().xy());
}


// ****************************************************************
// 
coreVector2 cEnemy::AimAtPlayer()const
{
    // 
    return this->AimAtPlayer(this->NearestPlayer());
}

coreVector2 cEnemy::AimAtPlayer(const cPlayer* pPlayer)const
{
    // 
    return (pPlayer->GetPosition().xy() - this->GetPosition().xy());
}


// ****************************************************************
// 
void cEnemy::_SetParent(cEnemy* OUTPUT pParent)
{
    ASSERT(!this->IsParent())

    if(!m_apMember.empty())
    {
        // 
        m_apMember.front()->m_apMember.erase(this);
        m_apMember.clear();
    }

    if(pParent)
    {
        ASSERT(!pParent->IsChild())

        // 
        m_apMember.insert(pParent);
        this->AddStatus(ENEMY_STATUS_CHILD);

        // 
        pParent->m_apMember.insert(this);
        pParent->RemoveStatus(ENEMY_STATUS_CHILD);
    }
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
: oEnemyActive (ENEMY_SET_INIT)
, iTopEnemy    (0u)
{
}


// ****************************************************************
// constructor
cEnemyManager::cEnemyManager()noexcept
: m_apEnemySet {}
{
    // 
    Core::Manager::Object->TestCollision(TYPE_ENEMY, [](coreObject3D*, coreObject3D*, coreVector3, coreBool) {});
}


// ****************************************************************
// destructor
cEnemyManager::~cEnemyManager()
{
    ASSERT(m_apAdditional.empty())

    // 
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
        SAFE_DELETE(m_apEnemySet[i])

    // clear memory
    m_apAdditional.clear();
}


// ****************************************************************
// render the enemy manager
void cEnemyManager::Render()
{
    // loop through all enemy sets
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
    {
        if(!m_apEnemySet[i]) continue;
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;

        // 
        if(!pEnemyActive->GetCurEnabled()) continue;

        // 
        FOR_EACH(it, *pEnemyActive->List()) s_cast<cEnemy*>(*it)->ActivateModelDefault();
        {
            // 
            pEnemyActive->RenderCustom([](coreFloat* OUTPUT pData, const cEnemy* pObject)
            {
                (*pData) = pObject->GetBlink();
            },
            [](cEnemy* OUTPUT pObject)
            {
                pObject->_EnableBlink();
            });
        }
        FOR_EACH(it, *pEnemyActive->List()) s_cast<cEnemy*>(*it)->ActivateModelLowOnly();
    }

    // render all additional enemies
    FOR_EACH(it, m_apAdditional)
    {
        if(CONTAINS_FLAG((*it)->GetStatus(), ENEMY_STATUS_DEAD))
            continue;

        (*it)->Render();
    }
}

#define __RENDER_OWN(f)                                               \
{                                                                     \
    /* */                                                             \
    auto nRenderFunc = [](cEnemy* OUTPUT pEnemy)                      \
    {                                                                 \
        if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))     \
            return;                                                   \
                                                                      \
        pEnemy->f();                                                  \
    };                                                                \
                                                                      \
    /* loop through all enemy sets */                                 \
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)                   \
    {                                                                 \
        if(!m_apEnemySet[i]) continue;                                \
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive; \
                                                                      \
        /* render all active enemies */                               \
        FOR_EACH(it, *pEnemyActive->List())                           \
            nRenderFunc(s_cast<cEnemy*>(*it));                        \
    }                                                                 \
                                                                      \
    /* render all additional enemies */                               \
    FOR_EACH(it, m_apAdditional)                                      \
        nRenderFunc(*it);                                             \
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
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
    {
        if(!m_apEnemySet[i]) continue;
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;

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
    cEnemy*       pEnemy    = (*ppEnemy);
    sEnemySetGen* pEnemySet = m_apEnemySet[pEnemy->GetID()];

    // 
    pEnemy->Kill(false);

    // 
    pEnemy->RemoveStatus(ENEMY_STATUS_ASSIGNED);
    pEnemySet->oEnemyActive.UnbindObject(pEnemy);

    // 
    pEnemySet->iTopEnemy = 0u;

    // 
    (*ppEnemy) = NULL;
}


// ****************************************************************
// remove all enemies
void cEnemyManager::ClearEnemies(const coreBool bAnimated)
{
    // loop trough all enemy sets
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
    {
        if(!m_apEnemySet[i]) continue;
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;

        // deactivate all active enemies
        FOR_EACH(it, *pEnemyActive->List())
            s_cast<cEnemy*>(*it)->Kill(bAnimated);
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
    // update rotation angle (opposed to freezer enemy)
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


// ****************************************************************
// constructor
cCustomEnemy::cCustomEnemy()noexcept
{
    // 
    this->AddStatus(ENEMY_STATUS_SINGLE);

    // 
    g_pGame->GetEnemyManager()->BindEnemy(this);
}


// ****************************************************************
// destructor
cCustomEnemy::~cCustomEnemy()
{
    // 
    this->Kill(false);

    // 
    g_pGame->GetEnemyManager()->UnbindEnemy(this);
}