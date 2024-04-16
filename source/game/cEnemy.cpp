///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cEnemy::cEnemy()noexcept
: m_fLifeTime       (0.0f)
, m_fLifeTimeBefore (0.0f)
, m_iLastAttacker   (0u)
{
    // load object resources
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_blink_program");

    // reset base properties
    this->ResetProperties();
}


// ****************************************************************
// configure the enemy
void cEnemy::Configure(const coreInt32 iHealth, const coreVector3 vColor, const coreBool bInverted)
{
    // set health and color
    this->SetMaxHealth(iHealth);
    this->SetBaseColor(vColor, bInverted);
}


// ****************************************************************
// render the enemy
void cEnemy::Render()
{
        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM))
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))
    {
        // 
        this->_EnableBlink();

        // 
        cLodObject::RenderHighObject(this);
    }
}


// ****************************************************************
// move the enemy
void cEnemy::Move()
{
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))
    {
        // 
        m_fLifeTimeBefore = m_fLifeTime;
        m_fLifeTime.Update(1.0f);

        // call individual move routines
        this->__MoveOwn();

        // move the 3d-object
        this->coreObject3D::Move();

        // 
        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD) && g_pForeground->IsVisibleObject(this))
        {
            this->SetEnabled(HAS_FLAG(m_iStatus, ENEMY_STATUS_HIDDEN) ? CORE_OBJECT_ENABLE_MOVE : CORE_OBJECT_ENABLE_ALL);
            this->ChangeType(TYPE_ENEMY);   // # make it available in cEnemyManager::ForEachEnemy
        }
        else
        {
            this->SetEnabled(CORE_OBJECT_ENABLE_MOVE);
        }

        // 
        const coreModelPtr& pLowQuad = Core::Manager::Object->GetLowQuad();
        if(HAS_FLAG(m_iStatus, ENEMY_STATUS_GHOST))
        {
            if(m_pVolume.GetHandle() == NULL) this->DefineVolume(pLowQuad);
        }
        else
        {
            if(m_pVolume.GetHandle() == pLowQuad.GetHandle()) this->DefineVolume(NULL);
        }

        // TODO 1: better would be a shield which is only visible on bullet-hits (and tighter, maybe around silhouette)
        //if(STATIC_ISVALID(g_pGame)) 
        //{
        //    if(HAS_FLAG(m_iStatus, ENEMY_STATUS_INVINCIBLE))
        //    {
        //        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_SHIELDED)) g_pGame->GetShieldManager()->BindEnemy(this);
        //    }
        //    else
        //    {
        //        if(HAS_FLAG(m_iStatus, ENEMY_STATUS_SHIELDED)) g_pGame->GetShieldManager()->UnbindEnemy(this);
        //    }
        //}
    }

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// reduce current health
coreInt32 cEnemy::TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2 vImpact, cPlayer* pAttacker)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    const coreBool bMulti = (pAttacker && g_pGame->IsMulti());
    m_iLastAttacker = bMulti ? (pAttacker - g_pGame->GetPlayer(0u)) : 0u;

    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_INVINCIBLE))
    {
        // forward to parent
        if(this->IsChild()) return m_apMember.front()->TakeDamage(iDamage, iElement, vImpact, pAttacker);

        if(iDamage > 0)
        {
            // 
            const coreInt32 iPower = (bMulti || (this->GetMaxHealth() == 1)) ? 1 : GAME_PLAYERS;
            const coreInt32 iTaken = this->_TakeDamage(iDamage * iPower, iElement, vImpact) / iPower;
            ASSERT(!(this->GetMaxHealth() % iPower))

            if(iTaken)
            {
                // 
                this->RefreshColor();
                this->InvokeBlink();

                // 
                if(this->IsParent())
                {
                    FOR_EACH(it, m_apMember)
                    {
                        (*it)->RefreshColor(this->GetCurHealthPct());
                        (*it)->InvokeBlink();
                    }
                }

                if(pAttacker)
                {
                    // 
                    pAttacker->GetScoreTable()->RefreshCooldown();

                    if(HAS_FLAG(m_iStatus, ENEMY_STATUS_BOSS))
                    {
                        // 
                        pAttacker->GetScoreTable()->AddChain(iTaken);
                    }

                    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_WORTHLESS))
                    {
                        // 
                        pAttacker->GetDataTable()->EditCounterTotal  ()->iDamageGiven += iTaken;
                        pAttacker->GetDataTable()->EditCounterMission()->iDamageGiven += iTaken;
                        pAttacker->GetDataTable()->EditCounterSegment()->iDamageGiven += iTaken;

                        // 
                        g_pSave->EditGlobalStats      ()->iDamageGiven += iTaken;
                        g_pSave->EditLocalStatsMission()->iDamageGiven += iTaken;
                        g_pSave->EditLocalStatsSegment()->iDamageGiven += iTaken;
                    }
                }
            }

            if(!m_iCurHealth)
            {
                if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_IMMORTAL))   // TODO 1: some immortal enemies killed in scripts need to give score, move stuff into own "GiveScore" function or so
                {
                    // 
                    this->Kill(true);

                    if(pAttacker)
                    {
                        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_WORTHLESS))
                        {
                            // 
                            const coreUint32 iScore = pAttacker->GetScoreTable()->AddScore(10u * m_iMaxHealth, true);
                            pAttacker->GetScoreTable()->AddCombo(1u);

                            // 
                            g_pGame->GetCombatText()->DrawScore(iScore, this->GetPosition(), !g_pGame->GetEnemyManager()->GetNumEnemiesAlive());
                        }
                    }
                }
            }

            return iTaken;
        }
    }

    return 0;
}


// ****************************************************************
// add enemy to the game
void cEnemy::Resurrect()
{
    // resurrect enemy
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    const coreBool bSingle = HAS_FLAG(m_iStatus, ENEMY_STATUS_SINGLE);
    const coreBool bEnergy = HAS_FLAG(m_iStatus, ENEMY_STATUS_ENERGY);
    const coreBool bBottom = HAS_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM);
    ASSERT(!bEnergy || (bEnergy && bSingle))   
    ASSERT(!bBottom || (bBottom && bSingle))   

    // 
    m_fLifeTime       = 0.0f;
    m_fLifeTimeBefore = 0.0f;

    if(bEnergy)
    {
        // add ship to glow and outline
        g_pGlow->BindObject(this);
        if(!bBottom) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);
    }
    else if(bSingle)
    {
        // add ship to global shadow and outline
        cShadow::GetGlobalContainer()->BindObject(this);
        if(!bBottom) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);
    }

    if(HAS_FLAG(m_iStatus, ENEMY_STATUS_BOSS))
    {
        // 
        if(this->IsParent()) FOR_EACH(it, m_apMember) (*it)->ChangeType(TYPE_ENEMY);
        this->ChangeType(TYPE_ENEMY);
    }

    // add ship to the game
    this->_Resurrect();

    // 
    this->__ResurrectOwn();

    // 
    if(this->IsParent()) FOR_EACH(it, m_apMember) (*it)->Resurrect();
}


// ****************************************************************
// remove enemy from the game
void cEnemy::Kill(const coreBool bAnimated)
{
    // kill enemy
    if(HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD)) return;
    ADD_FLAG(m_iStatus, ENEMY_STATUS_DEAD)

    // 
    const coreBool bSingle = HAS_FLAG(m_iStatus, ENEMY_STATUS_SINGLE);
    const coreBool bEnergy = HAS_FLAG(m_iStatus, ENEMY_STATUS_ENERGY);
    const coreBool bBottom = HAS_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM);
    ASSERT(!bEnergy || (bEnergy && bSingle))
    ASSERT(!bBottom || (bBottom && bSingle))

    // 
    if(STATIC_ISVALID(g_pGame)) g_pGame->GetShieldManager()->UnbindEnemy(this);

    // 
    if(bAnimated && this->GetType())
    {
        // 
        if(HAS_BIT(m_iBaseColor, SHIP_INVERTED_BIT))
        {
            const coreVector3 vColor = (g_pEnvironment->GetBackground()->GetID() == cSnowBackground::ID) ? COLOR_FIRE_BLUE : COLOR_FIRE_ORANGE;
            g_pSpecialEffects->MacroDestructionColor(this, vColor);
        }
        else
        {
            g_pSpecialEffects->MacroDestructionDark(this);
        }

        // 
        if(STATIC_ISVALID(g_pGame) && (g_pGame->GetEnemyManager()->GetNumEnemiesAlive() <= 1u))
        {
            g_pGame->GetCrashManager()->AddCrash(*this, this->GetPosition().xy() - this->GetPosition().xy().Normalized() * 10.0f, NULL);
        }
    }

    if(bEnergy)
    {
        // remove ship from glow and outline
        g_pGlow->UnbindObject(this);
        if(!bBottom) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindObject(this);
    }
    else if(bSingle)
    {
        // remove ship from global shadow and outline
        cShadow::GetGlobalContainer()->UnbindObject(this);
        if(!bBottom) g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindObject(this);
    }

    // disable collision
    this->ChangeType(0);

    // remove ship from the game
    this->_Kill(bAnimated);

    // 
    this->__KillOwn(bAnimated);

    // 
    if(this->IsParent()) FOR_EACH(it, m_apMember) (*it)->Kill(false);   // # never animate
}


// ****************************************************************
// reset base properties
void cEnemy::ResetProperties()
{
    // set object properties
    this->SetPosition   (coreVector3(HIDDEN_POS,0.0f));
    this->SetSize       (coreVector3(1.0f, 1.0f,1.0f) * ENEMY_SIZE_FACTOR);
    this->SetDirection  (coreVector3(0.0f,-1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f, 0.0f,1.0f));

    // set initial status
    m_iStatus = ENEMY_STATUS_DEAD;
}


// ****************************************************************
// 
cPlayer* cEnemy::LastAttacker()const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return g_pGame->GetPlayer(m_iLastAttacker);
}


// ****************************************************************
// 
cPlayer* cEnemy::NearestPlayerSide()const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return g_pGame->FindPlayerSide(this->GetPosition().xy());
}

cPlayer* cEnemy::NearestPlayerSideRev()const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return g_pGame->FindPlayerSide(this->GetPosition().xy().InvertedX());
}

cPlayer* cEnemy::NearestPlayerDual(const coreUintW iIndex)const
{
    // 
    ASSERT(STATIC_ISVALID(g_pGame))
    return g_pGame->FindPlayerDual(iIndex);
}


// ****************************************************************
// 
coreVector2 cEnemy::AimAtPlayerSide()const
{
    // 
    return (this->NearestPlayerSide()->GetPosition().xy() - this->GetPosition().xy());
}

coreVector2 cEnemy::AimAtPlayerSideRev()const
{
    // 
    return (this->NearestPlayerSideRev()->GetPosition().xy() - this->GetPosition().xy());
}

coreVector2 cEnemy::AimAtPlayerDual(const coreUintW iIndex)const
{
    // 
    return (this->NearestPlayerDual(iIndex)->GetPosition().xy() - this->GetPosition().xy());
}


// ****************************************************************
// 
void cEnemy::_SetParent(cEnemy* pParent)
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
    // render all additional enemies
    FOR_EACH(it, m_apAdditional)
    {
        if((*it)->HasStatus(ENEMY_STATUS_DEAD))
            continue;

        (*it)->Render();
    }

    // loop through all enemy sets
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
    {
        if(!m_apEnemySet[i]) continue;
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;

        // 
        if(!pEnemyActive->GetCurEnabled()) continue;

        // render all active enemies
        cLodObject::RenderHighList(pEnemyActive);
    }
}

#define __RENDER_OWN(f)                                               \
{                                                                     \
    /* */                                                             \
    const auto nRenderFunc = [](cEnemy* OUTPUT pEnemy)                \
    {                                                                 \
        if(pEnemy->HasStatus(ENEMY_STATUS_DEAD))                      \
            return;                                                   \
                                                                      \
        pEnemy->f();                                                  \
    };                                                                \
                                                                      \
    /* render all additional enemies */                               \
    FOR_EACH(it, m_apAdditional)                                      \
        nRenderFunc(*it);                                             \
                                                                      \
    /* loop through all enemy sets */                                 \
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)                   \
    {                                                                 \
        if(!m_apEnemySet[i]) continue;                                \
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive; \
                                                                      \
        /* render all active enemies */                               \
        FOR_EACH(it, *pEnemyActive->List())                           \
            nRenderFunc(d_cast<cEnemy*>(*it));                        \
    }                                                                 \
} 

void cEnemyManager::RenderBottom() {__RENDER_OWN(__RenderOwnBottom)

DEPTH_PUSH
//glDisable(GL_DEPTH_TEST);
    FOR_EACH(it, m_apAdditional)
    {
        if((*it)->HasStatus(ENEMY_STATUS_DEAD))
            continue;

            if((*it)->HasStatus(ENEMY_STATUS_BOTTOM))
        if(!(*it)->HasStatus(ENEMY_STATUS_DEAD))
        {
            // 
            (*it)->_EnableBlink();
    
            // 
            cLodObject::RenderHighObject(*it);
            g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(*it);
        }
    }
    
//glEnable(GL_DEPTH_TEST);


}
void cEnemyManager::RenderUnder () {__RENDER_OWN(__RenderOwnUnder)}
void cEnemyManager::RenderOver  () {__RENDER_OWN(__RenderOwnOver)}
void cEnemyManager::RenderTop   () {__RENDER_OWN(__RenderOwnTop)}

#undef __RENDER_OWN


// ****************************************************************
// move the enemy manager
void cEnemyManager::MoveBefore()
{
    // 
    FOR_EACH(it, m_apAdditional)
        (*it)->_UpdateAlwaysBefore();

    // 
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
    {
        if(!m_apEnemySet[i]) continue;
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;

        // 
        FOR_EACH(it, *pEnemyActive->List())
            d_cast<cEnemy*>(*it)->_UpdateAlwaysBefore();
    }
}

void cEnemyManager::MoveMiddle()
{
    // move all additional enemies (# bosses need to move before other enemies)
    FOR_EACH(it, m_apAdditional)
        (*it)->Move();

    // loop through all enemy sets
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
    {
        if(!m_apEnemySet[i]) continue;
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;

        // move the enemy set
        pEnemyActive->MoveNormal();
    }
}

void cEnemyManager::MoveAfter()
{
    // not used
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
            d_cast<cEnemy*>(*it)->Kill(bAnimated);
    }

    // deactivate all additional enemies
    FOR_EACH(it, m_apAdditional)
        (*it)->Kill(bAnimated);
}


// ****************************************************************
// 
cEnemy* cEnemyManager::FindEnemy(const coreVector2 vPosition)const
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
// 
cEnemy* cEnemyManager::FindEnemyRev(const coreVector2 vPosition)const
{
    // 
    cEnemy*   pEnemy = NULL;
    coreFloat fLenSq = 0.0f;

    // 
    this->ForEachEnemy([&](cEnemy* OUTPUT pCurEnemy)
    {
        // 
        const coreFloat fCurLenSq = (pCurEnemy->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq > fLenSq)
        {
            // 
            pEnemy = pCurEnemy;
            fLenSq = fCurLenSq;
        }
    });

    return pEnemy;
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
        cEnemySquad::__GetDefaultEnemyManager()->FreeEnemy(&(*it));

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
cEnemy* cEnemySquad::FindEnemy(const coreVector2 vPosition)const
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
// 
cEnemy* cEnemySquad::FindEnemyRev(const coreVector2 vPosition)const
{
    // 
    cEnemy*   pEnemy = NULL;
    coreFloat fLenSq = 0.0f;

    // 
    this->ForEachEnemy([&](cEnemy* OUTPUT pCurEnemy, const coreUintW i)
    {
        // 
        const coreFloat fCurLenSq = (pCurEnemy->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq > fLenSq)
        {
            // 
            pEnemy = pCurEnemy;
            fLenSq = fCurLenSq;
        }
    });

    return pEnemy;
}


// ****************************************************************
// 
cEnemyManager* cEnemySquad::__GetDefaultEnemyManager()
{
    ASSERT(STATIC_ISVALID(g_pGame))
    return g_pGame->GetEnemyManager();
}


// ****************************************************************
// constructor
cDummyEnemy::cDummyEnemy()noexcept
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());
}


// ****************************************************************
// constructor
cScoutEnemy::cScoutEnemy()noexcept
{
    // load models
    this->DefineModelHigh("ship_enemy_scout_high.md3");
    this->DefineModelLow ("ship_enemy_scout_low.md3");
}


// ****************************************************************
// constructor
cWarriorEnemy::cWarriorEnemy()noexcept
{
    // load models
    this->DefineModelHigh("ship_enemy_warrior_high.md3");
    this->DefineModelLow ("ship_enemy_warrior_low.md3");
}


// ****************************************************************
// constructor
cStarEnemy::cStarEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_star_high.md3");
    this->DefineModelLow ("ship_enemy_star_low.md3");
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
}


// ****************************************************************
// constructor
cFreezerEnemy::cFreezerEnemy()noexcept
: m_fAngle (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_freezer_high.md3");
    this->DefineModelLow ("ship_enemy_freezer_low.md3");
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
cMeteorEnemy::cMeteorEnemy()noexcept
{
    // load object resources
    this->DefineModelHigh("ship_meteor.md3");
    this->DefineModelLow ("ship_meteor.md3");
    this->DefineTexture  (0u, "environment_stone_diff.png");
    this->DefineTexture  (1u, "environment_stone_norm.png");
    this->DefineProgram  ("object_meteor_blink_program");
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


// ****************************************************************
// constructor
cRepairEnemy::cRepairEnemy()noexcept
: m_pPlayer    (NULL)
, m_vDirection (coreVector2(0.0f,1.0f))
, m_fAnimation (0.0f)
{
    // 
    this->Configure(50, COLOR_SHIP_GREY);
    this->AddStatus(ENEMY_STATUS_SINGLE);
    this->AddStatus(ENEMY_STATUS_IMMORTAL);
    this->AddStatus(ENEMY_STATUS_HIDDEN);
    this->AddStatus(ENEMY_STATUS_WORTHLESS);

    // 
    g_pGame->GetEnemyManager()->BindEnemy(this);

    // 
    this->DefineModelHigh("object_sphere.md3");
    this->DefineModelLow ("object_sphere.md3");
    this->SetSize        (coreVector3(1.0f,1.0f,1.0f) * 5.0f);

    // 
    m_Bubble.DefineModel  ("object_sphere.md3");
    m_Bubble.DefineTexture(0u, "effect_energy.png");
    m_Bubble.DefineProgram("effect_energy_blink_flat_spheric_program");
    m_Bubble.SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.6f, 0.0f));
    m_Bubble.SetTexSize   (coreVector2(1.0f,1.0f) * 3.5f);

    // 
    m_Ship.DefineTexture(0u, "effect_energy.png");
    m_Ship.DefineProgram("effect_energy_blink_invert_program");
    m_Ship.SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.6f, 0.0f));
    m_Ship.SetTexSize   (coreVector2(1.0f,1.0f) * 0.6f);
}


// ****************************************************************
// destructor
cRepairEnemy::~cRepairEnemy()
{
    // 
    this->Kill(false);

    // 
    g_pGame->GetEnemyManager()->UnbindEnemy(this);
}


// ****************************************************************
// 
void cRepairEnemy::AssignPlayer(cPlayer* pPlayer)
{
    ASSERT(pPlayer)

    // 
    m_pPlayer = pPlayer;

    // 
    m_vDirection = coreVector2(-SIGN(pPlayer->GetPosition().x), -SIGN(pPlayer->GetPosition().y) * 2.0f).Normalized();
    m_fAnimation = 0.0f;

    // 
    this->SetPosition(pPlayer->GetPosition());

    // 
    m_Ship.DefineModelHigh(pPlayer->GetModelHigh());
    m_Ship.DefineModelLow (pPlayer->GetModelLow ());

    // 
    m_Bubble.SetAlpha(0.0f);
    m_Ship  .SetAlpha(0.0f);
}


// ****************************************************************
// 
void cRepairEnemy::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Bubble);
    g_pGlow->BindObject(&m_Ship);
}


// ****************************************************************
// 
void cRepairEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pGlow->UnbindObject(&m_Bubble);
    g_pGlow->UnbindObject(&m_Ship);
}


// ****************************************************************
// 
void cRepairEnemy::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Bubble);

    // 
    this->_EnableBlink(m_Bubble.GetProgram());
    m_Bubble.Render();

    DEPTH_PUSH

    // 
    this->_EnableBlink(m_Ship.GetProgram());
    m_Ship.Render();

    // 
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Ship);
}


// ****************************************************************
// 
void cRepairEnemy::__MoveOwn()
{
    ASSERT(m_pPlayer)

    // 
    m_fAnimation.UpdateMod(0.2f, 4.0f);

    // 
    const coreVector2 vNewPos = this->GetPosition().xy() + m_vDirection * (30.0f * TIME);
    const coreVector2 vNewDir = coreVector2::Direction(m_fAnimation * (8.0f*PI));
    const coreVector4 vArea   = m_pPlayer->GetArea();

    // 
         if((vNewPos.x < vArea.x) && (m_vDirection.x < 0.0f)) m_vDirection.x =  ABS(m_vDirection.x);
    else if((vNewPos.x > vArea.z) && (m_vDirection.x > 0.0f)) m_vDirection.x = -ABS(m_vDirection.x);
         if((vNewPos.y < vArea.y) && (m_vDirection.y < 0.0f)) m_vDirection.y =  ABS(m_vDirection.y);
    else if((vNewPos.y > vArea.w) && (m_vDirection.y > 0.0f)) m_vDirection.y = -ABS(m_vDirection.y);

    // 
    this->SetPosition (coreVector3(vNewPos, 0.0f));
    this->SetDirection(coreVector3(vNewDir, 0.0f));

    // 
    const coreFloat fAlpha = MIN(m_Bubble.GetAlpha() + 4.0f * TIME, 1.0f);

    // 
    m_Bubble.SetPosition (coreVector3(vNewPos, 0.0f));
    m_Bubble.SetDirection(coreVector3(vNewDir, 0.0f));
    m_Bubble.SetSize     (fAlpha * this->GetSize() * PLAYER_SIZE_FACTOR);
    m_Bubble.SetAlpha    (fAlpha);
    m_Bubble.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.5f));
    m_Bubble.Move();

    // 
    m_Ship.SetPosition (coreVector3(vNewPos, 0.0f));
    m_Ship.SetDirection(coreVector3(vNewDir, 0.0f));
    m_Ship.SetSize     (fAlpha * coreVector3(1.0f,1.0f,1.0f) * PLAYER_SIZE_FACTOR);
    m_Ship.SetAlpha    (fAlpha);
    m_Ship.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
    m_Ship.Move();

    // 
    m_pPlayer->SetPosition(this->GetPosition());
}