///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreUint8 cEnemy::s_iExhaustCount = 0u;


// ****************************************************************
// constructor
cEnemy::cEnemy()noexcept
: m_fLifeTime        (0.0f)
, m_fLifeTimeBefore  (0.0f)
, m_iLastAttacker    (0u)
, m_bWasDamaged      (false)
, m_iScore           (0u)
, m_iDamageForwarded (0)
{
    // load object resources
    this->DefineTexture(0u, "ship_enemy.png");
    this->DefineProgram("object_ship_blink_program");

    // reset base properties
    this->ResetProperties();
    
    
    this->DefineTexture(1u, "menu_background_black.png");
}


// ****************************************************************
// configure the enemy
void cEnemy::Configure(const coreInt32 iHealth, const coreUint16 iScore, const coreVector3 vColor, const coreBool bInverted, const coreBool bIgnored)
{
    // set health and color
    this->SetMaxHealth(iHealth);
    this->SetBaseColor(vColor, bInverted, bIgnored);

    // 
    m_iScore = iScore;
}


// ****************************************************************
// render the enemy
void cEnemy::Render()
{
        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM))
        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_TOP))
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
        m_bWasDamaged = false;

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

        // reduce collision overhead for ghost enemies without bounding volume
        const coreModelPtr& pLowQuad = Core::Manager::Object->GetLowQuad();
        if(HAS_FLAG(m_iStatus, ENEMY_STATUS_GHOST))
        {
            if(m_pVolume.GetHandle() == NULL) this->DefineVolume(pLowQuad);
        }
        else
        {
            if(m_pVolume.GetHandle() == pLowQuad.GetHandle()) this->DefineVolume(NULL);
        }

        // 
        if(STATIC_ISVALID(g_pGame))
        {
            if(HAS_FLAG(m_iStatus, ENEMY_STATUS_INVINCIBLE) && !HAS_FLAG(m_iStatus, ENEMY_STATUS_SECRET)) g_pGame->GetShieldManager()->GetEffect(SHIELD_EFFECT_INVINCIBLE)->BindEnemy  (this);
            else                                                                                          g_pGame->GetShieldManager()->GetEffect(SHIELD_EFFECT_INVINCIBLE)->UnbindEnemy(this);
                //if(HAS_FLAG(m_iStatus, ENEMY_STATUS_DAMAGING)   && !HAS_FLAG(m_iStatus, ENEMY_STATUS_SECRET)) g_pGame->GetShieldManager()->GetEffect(SHIELD_EFFECT_DAMAGING)  ->BindEnemy  (this);
                //else                                                                                          g_pGame->GetShieldManager()->GetEffect(SHIELD_EFFECT_DAMAGING)  ->UnbindEnemy(this);
            // TODO 1: same shield state is checked in both types
        }
    }

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// reduce current health
coreInt32 cEnemy::TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2 vImpact, cPlayer* OUTPUT pAttacker)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    const coreBool bMulti = (pAttacker && g_pGame->IsMulti());
    m_iLastAttacker = bMulti ? (pAttacker - g_pGame->GetPlayer(0u)) : 0u;

    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_INVINCIBLE))
    {
        // 
        m_bWasDamaged = (m_iCurHealth != 0) && (iDamage != 0);

        // forward to parent
        if(this->IsChild())
        {
            const coreInt32 iTaken = m_apMember.front()->TakeDamage(iDamage, iElement, vImpact, pAttacker);
            m_iDamageForwarded += iTaken;
            return iTaken;
        }

        if(iDamage)
        {
            // 
            const coreInt32 iPower = (bMulti || (this->GetMaxHealth() == 1)) ? 1 : GAME_PLAYERS;
            const coreInt32 iTaken = ABS(this->_TakeDamage(iDamage * iPower, iElement, vImpact) / iPower);
            ASSERT(!(this->GetMaxHealth() % iPower))

            if(iTaken)
            {
                // 
                this->RefreshColorAll();
                this->InvokeBlinkAll();

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
                            const coreUint32 iScore = pAttacker->GetScoreTable()->AddScore(m_iScore ? m_iScore : (10u * m_iMaxHealth), true);
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
    const coreBool bTop    = HAS_FLAG(m_iStatus, ENEMY_STATUS_TOP);
    ASSERT(!bEnergy || (bEnergy && bSingle))   
    //ASSERT(!bBottom || (bBottom && bSingle))   

    // 
    m_fLifeTime       = 0.0f;
    m_fLifeTimeBefore = 0.0f;
    
    m_bWasDamaged = false;

    if(bEnergy)
    {
        // add ship to glow and outline
        g_pGlow->BindObject(this);
        if(!bBottom && !bTop) g_pOutline->GetStyle(this->GetOutlineStyle())->BindObject(this);
    }
    else if(bSingle)
    {
        // add ship to global shadow and outline
        cShadow::GetGlobalContainer()->BindObject(this);
        if(!bBottom && !bTop) g_pOutline->GetStyle(this->GetOutlineStyle())->BindObject(this);
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
    const coreBool bTop    = HAS_FLAG(m_iStatus, ENEMY_STATUS_TOP);
    ASSERT(!bEnergy || (bEnergy && bSingle))
    //ASSERT(!bBottom || (bBottom && bSingle))

    // 
    if(STATIC_ISVALID(g_pGame))
    {
        g_pGame->GetShieldManager()->GetEffect(SHIELD_EFFECT_INVINCIBLE)->UnbindEnemy(this);
        g_pGame->GetShieldManager()->GetEffect(SHIELD_EFFECT_DAMAGING)  ->UnbindEnemy(this);
    }

    // 
    if(bAnimated)// && this->IsEnabled(CORE_OBJECT_ENABLE_RENDER)) sollten am rand explodieren // this->GetType())   
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
        g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, this->GetExplosionSound());

        // 
        if(STATIC_ISVALID(g_pGame) && (g_pGame->GetEnemyManager()->GetNumEnemiesAlive() <= 1u))
        {
            // TODO 1: position 0.0f,0.0f
            //g_pGame->GetCrashManager()->AddCrash(*this, this->GetPosition().xy() - this->GetPosition().xy().Normalized() * 10.0f, NULL);
        }
    }

    if(bEnergy)
    {
        // remove ship from glow and outline
        g_pGlow->UnbindObject(this);
        if(!bBottom && !bTop) g_pOutline->GetStyle(this->GetOutlineStyle())->UnbindObject(this);
    }
    else if(bSingle)
    {
        // remove ship from global shadow and outline
        cShadow::GetGlobalContainer()->UnbindObject(this);
        if(!bBottom && !bTop) g_pOutline->GetStyle(this->GetOutlineStyle())->UnbindObject(this);
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
    this->SetColor4     (coreVector4(1.0f, 1.0f,1.0f,1.0f));

    // set initial status
    m_iStatus = ENEMY_STATUS_DEAD;
}

// ****************************************************************
// 
void cEnemy::RefreshColorAll(const coreFloat fFactor)
{
    // 
    this->RefreshColor(fFactor);

    // 
    if(this->IsParent())
    {
        FOR_EACH(it, m_apMember) (*it)->RefreshColor(fFactor);
    }

}
void cEnemy::RefreshColorAll()
{
    // 
    this->RefreshColorAll(this->GetCurHealthPct());
}


// ****************************************************************
// 
void cEnemy::InvokeBlinkAll()
{
    // 
    this->InvokeBlink();

    // 
    if(this->IsParent())
    {
        FOR_EACH(it, m_apMember)(*it)->InvokeBlink();
    }
}


// ****************************************************************
// 
void cEnemy::ChangeToBottom()
{
    ASSERT(HAS_FLAG(m_iStatus, ENEMY_STATUS_SINGLE))

    // 
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))
    {
        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM) && !HAS_FLAG(m_iStatus, ENEMY_STATUS_TOP))
            g_pOutline->GetStyle(this->GetOutlineStyle())->UnbindObject(this);
    }

    // 
    ADD_FLAG   (m_iStatus, ENEMY_STATUS_BOTTOM)
    REMOVE_FLAG(m_iStatus, ENEMY_STATUS_TOP)
}

void cEnemy::ChangeToTop()
{
    ASSERT(HAS_FLAG(m_iStatus, ENEMY_STATUS_SINGLE))

    // 
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))
    {
        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM) && !HAS_FLAG(m_iStatus, ENEMY_STATUS_TOP))
            g_pOutline->GetStyle(this->GetOutlineStyle())->UnbindObject(this);
    }

    // 
    REMOVE_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM)
    ADD_FLAG   (m_iStatus, ENEMY_STATUS_TOP)
}

void cEnemy::ChangeToNormal()
{
    ASSERT(HAS_FLAG(m_iStatus, ENEMY_STATUS_SINGLE))

    // 
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_DEAD))
    {
        if(HAS_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM) || HAS_FLAG(m_iStatus, ENEMY_STATUS_TOP))
            g_pOutline->GetStyle(this->GetOutlineStyle())->BindObject(this);
    }

    // 
    REMOVE_FLAG(m_iStatus, ENEMY_STATUS_BOTTOM)
    REMOVE_FLAG(m_iStatus, ENEMY_STATUS_TOP)
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

    // 
    this->RefreshColor((pParent ? pParent : this)->GetCurHealthPct());
}


// ****************************************************************
// 
coreFloat cEnemy::_GetExhaustOffset()
{
    if(++s_iExhaustCount >= 10u) s_iExhaustCount = 0u;
    return I_TO_F(s_iExhaustCount) * 0.1f;
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

void cEnemyManager::RenderBottom()
{
    __RENDER_OWN(__RenderOwnBottom)

    DEPTH_PUSH

    /* */ 
    const auto nRenderFunc = [](cEnemy* OUTPUT pEnemy)
    {
        if(pEnemy->HasStatus(ENEMY_STATUS_DEAD))
            return;

        if(pEnemy->HasStatus(ENEMY_STATUS_BOTTOM))
        {
            pEnemy->_EnableBlink();

            // 
            cLodObject::RenderHighObject(pEnemy);
            g_pOutline->GetStyle(pEnemy->GetOutlineStyle())->ApplyObject(pEnemy);   // TODO 1: batching
        }
    };

    /* render all additional enemies */
    FOR_EACH(it, m_apAdditional)
        nRenderFunc(*it);

    /* loop through all enemy sets */
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)
    {
        if(!m_apEnemySet[i]) continue;
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;

        /* render all active enemies */
        FOR_EACH(it, *pEnemyActive->List())
            nRenderFunc(d_cast<cEnemy*>(*it));
    }
}

void cEnemyManager::RenderUnder () {__RENDER_OWN(__RenderOwnUnder)}
void cEnemyManager::RenderOver  () {__RENDER_OWN(__RenderOwnOver)}
void cEnemyManager::RenderTop   () {

DEPTH_PUSH

    /* */                                                            
    const auto nRenderFunc = [](cEnemy* OUTPUT pEnemy)               
    {                                                                
        if(pEnemy->HasStatus(ENEMY_STATUS_DEAD))                     
            return;   
        
        if(pEnemy->HasStatus(ENEMY_STATUS_TOP))
        {
            pEnemy->_EnableBlink();
    
            // 
            cLodObject::RenderHighObject(pEnemy);
            g_pOutline->GetStyle(pEnemy->GetOutlineStyle())->ApplyObject(pEnemy);   // TODO 1: batching
        }
    };                                                               
                                                                     
    /* render all additional enemies */                              
    FOR_EACH(it, m_apAdditional)                                     
        nRenderFunc(*it);                                            
                                                                     
    /* loop through all enemy sets */                                
    for(coreUintW i = 0u; i < ENEMY_SET_COUNT; ++i)                  
    {                                                                
        if(!m_apEnemySet[i]) continue;                               
        coreBatchList* pEnemyActive = &m_apEnemySet[i]->oEnemyActive;
                                                                     
        /* render all active enemies */                              
        FOR_EACH(it, *pEnemyActive->List())                          
            nRenderFunc(d_cast<cEnemy*>(*it));                       
    }                                                                
    
//glEnable(GL_DEPTH_TEST);

__RENDER_OWN(__RenderOwnTop)   // always after base
}

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
void cEnemySquad::ClearEnemies(const coreBool bAnimated)const
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
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_scout_high.md3");
    this->DefineModelLow ("ship_enemy_scout_low.md3");

    // 
    m_Exhaust.DefineModel  ("object_tube_open.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_flat_direct_program");
    m_Exhaust.SetColor3    (COLOR_ENERGY_WHITE * 0.7f);
    m_Exhaust.SetTexSize   (coreVector2(0.5f,0.25f));
}


// ****************************************************************
// 
void cScoutEnemy::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cScoutEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pGlow->UnbindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cScoutEnemy::__RenderOwnUnder()
{
    // 
    m_Exhaust.Render();
}


// ****************************************************************
// 
void cScoutEnemy::__MoveOwn()
{
    constexpr coreFloat fStrength = 0.07f;

    // 
    m_fAnimation.UpdateMod(0.75f, 1.0f);

    // 
    const coreFloat fLen  = fStrength * 25.0f;
    const coreFloat fSize = 1.0f - fStrength * 0.25f;

    // 
    m_Exhaust.SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * this->GetSize().x));
    m_Exhaust.SetTexOffset  (coreVector2(0.0f, m_fAnimation));
    m_Exhaust.SetPosition   (this->GetPosition   () - this->GetDirection() * (m_Exhaust.GetSize().y + 2.5f * this->GetSize().x));
    m_Exhaust.SetDirection  (this->GetDirection  ());
    m_Exhaust.SetOrientation(this->GetOrientation());
    m_Exhaust.SetAlpha      (this->GetAlpha      () * (this->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
    m_Exhaust.Move();
}


// ****************************************************************
// constructor
cWarriorEnemy::cWarriorEnemy()noexcept
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_warrior_high.md3");
    this->DefineModelLow ("ship_enemy_warrior_low.md3");

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aExhaust); ++i)
    {
        // 
        m_aExhaust[i].DefineModel  ("object_tube_open.md3");
        m_aExhaust[i].DefineTexture(0u, "effect_energy.png");
        m_aExhaust[i].DefineProgram("effect_energy_flat_direct_program");
        m_aExhaust[i].SetColor3    (COLOR_ENERGY_WHITE * 0.7f);
        m_aExhaust[i].SetTexSize   (coreVector2(0.5f,0.25f));
    }
}


// ****************************************************************
// 
void cWarriorEnemy::__ResurrectOwn()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aExhaust); ++i)
        g_pGlow->BindObject(&m_aExhaust[i]);
}


// ****************************************************************
// 
void cWarriorEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aExhaust); ++i)
        g_pGlow->UnbindObject(&m_aExhaust[i]);
}


// ****************************************************************
// 
void cWarriorEnemy::__RenderOwnUnder()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aExhaust); ++i)
        m_aExhaust[i].Render();
}


// ****************************************************************
// 
void cWarriorEnemy::__MoveOwn()
{
    constexpr coreFloat fStrength = 0.07f;

    // 
    m_fAnimation.UpdateMod(0.75f, 1.0f);

    // 
    const coreFloat fLen  = fStrength * 30.0f;
    const coreFloat fSize = 1.0f - fStrength * 0.25f;

    // 
    const coreVector3 vTan = coreVector3::Cross(this->GetDirection(), this->GetOrientation()).Normalized();

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aExhaust); ++i)
    {
        // 
        m_aExhaust[i].SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * this->GetSize().x));
        m_aExhaust[i].SetTexOffset  (coreVector2(0.0f, m_fAnimation));
        m_aExhaust[i].SetPosition   (this->GetPosition   () - this->GetDirection() * (m_aExhaust[i].GetSize().y + 1.85f * this->GetSize().x) + vTan * ((i ? -1.4f : 1.4f) * this->GetSize().x));
        m_aExhaust[i].SetDirection  (this->GetDirection  ());
        m_aExhaust[i].SetOrientation(this->GetOrientation());
        m_aExhaust[i].SetAlpha      (this->GetAlpha      () * (this->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
        m_aExhaust[i].Move();
    }
}


// ****************************************************************
// constructor
cStarEnemy::cStarEnemy()noexcept
: m_fAngle     (0.0f)
, m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_star_high.md3");
    this->DefineModelLow ("ship_enemy_star_low.md3");

    // 
    m_Exhaust.DefineModel  ("object_sphere.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_flat_program");
    m_Exhaust.SetColor3    (COLOR_ENERGY_WHITE * 0.5f);
    m_Exhaust.SetTexSize   (coreVector2(1.0f,1.0f) * 2.0f);
}


// ****************************************************************
// 
void cStarEnemy::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cStarEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pGlow->UnbindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cStarEnemy::__RenderOwnUnder()
{
    // 
    m_Exhaust.Render();
}


// ****************************************************************
// move the star enemy
void cStarEnemy::__MoveOwn()
{
    // update rotation angle (opposed to cinder enemy)
    m_fAngle.Update(-3.0f);

    // rotate around z-axis
    this->DefaultRotate(m_fAngle);

    // 
    m_fAnimation.UpdateMod(0.05f, 1.0f);

    // 
    const coreVector2 vDir = coreVector2::Direction(m_fAngle * 0.3f);

    // 
    m_Exhaust.SetSize     (coreVector3(1.0f,1.0f,1.0f) * this->GetVisualRadius() * 0.9f);
    m_Exhaust.SetTexOffset(coreVector2(0.0f, m_fAnimation));
    m_Exhaust.SetPosition (this->GetPosition());
    m_Exhaust.SetDirection(coreVector3(vDir, 0.0f));
    m_Exhaust.SetAlpha    (this->GetAlpha() * (this->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
    m_Exhaust.Move();
}


// ****************************************************************
// constructor
cArrowEnemy::cArrowEnemy()noexcept
: m_fAngle     (0.0f)
, m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_arrow_high.md3");
    this->DefineModelLow ("ship_enemy_arrow_low.md3");

    // 
    m_Exhaust.DefineModel  ("object_tube_open.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_flat_direct_program");
    m_Exhaust.SetColor3    (COLOR_ENERGY_WHITE * 0.7f);
    m_Exhaust.SetTexSize   (coreVector2(0.5f,0.25f));
}

// ****************************************************************
// 
void cArrowEnemy::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cArrowEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pGlow->UnbindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cArrowEnemy::__RenderOwnUnder()
{
    // 
    m_Exhaust.Render();
}


// ****************************************************************
// move the arrow enemy
void cArrowEnemy::__MoveOwn()
{
    // update rotation angle (opposed to freezer enemy)
    m_fAngle.Update(-3.0f);

    // rotate around direction axis
    this->DefaultOrientate(m_fAngle);

    constexpr coreFloat fStrength = 0.07f;

    // 
    m_fAnimation.UpdateMod(0.75f, 1.0f);

    // 
    const coreFloat fLen  = fStrength * 40.0f;
    const coreFloat fSize = 1.0f - fStrength * 0.25f;

    // 
    m_Exhaust.SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * this->GetSize().x));
    m_Exhaust.SetTexOffset  (coreVector2(0.0f, m_fAnimation));
    m_Exhaust.SetPosition   (this->GetPosition () - this->GetDirection() * (m_Exhaust.GetSize().y + 1.65f * this->GetSize().x));
    m_Exhaust.SetDirection  (this->GetDirection());
    m_Exhaust.SetAlpha      (this->GetAlpha    () * (this->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
    m_Exhaust.Move();
}


// ****************************************************************
// constructor
cMinerEnemy::cMinerEnemy()noexcept
: m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_miner_high.md3");
    this->DefineModelLow ("ship_enemy_miner_low.md3");

    // 
    m_Exhaust.DefineModel  ("object_tube_open.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_flat_direct_program");
    m_Exhaust.SetColor3    (COLOR_ENERGY_WHITE * 0.7f);
    m_Exhaust.SetTexSize   (coreVector2(0.75f,0.15f));
}


// ****************************************************************
// 
void cMinerEnemy::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cMinerEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pGlow->UnbindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cMinerEnemy::__RenderOwnUnder()
{
    // 
    m_Exhaust.Render();
}


// ****************************************************************
// 
void cMinerEnemy::__MoveOwn()
{
    constexpr coreFloat fStrength = 0.07f;

    // 
    m_fAnimation.UpdateMod(0.75f * 0.6f, 1.0f);

    // 
    const coreFloat fLen  = fStrength * 25.0f;
    const coreFloat fSize = (1.0f - fStrength * 0.25f) * 1.5f;

    // 
    m_Exhaust.SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * this->GetSize().x));
    m_Exhaust.SetTexOffset  (coreVector2(0.0f, m_fAnimation));
    m_Exhaust.SetPosition   (this->GetPosition   () - this->GetDirection() * (m_Exhaust.GetSize().y + 1.9f * this->GetSize().x));
    m_Exhaust.SetDirection  (this->GetDirection  ());
    m_Exhaust.SetOrientation(this->GetOrientation());
    m_Exhaust.SetAlpha      (this->GetAlpha      () * (this->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
    m_Exhaust.Move();
}


// ****************************************************************
// constructor
cFreezerEnemy::cFreezerEnemy()noexcept
: m_fAngle     (0.0f)
, m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_freezer_high.md3");
    this->DefineModelLow ("ship_enemy_freezer_low.md3");

    // 
    m_Exhaust.DefineModel  ("object_tube_open.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_flat_direct_program");
    m_Exhaust.SetColor3    (COLOR_ENERGY_WHITE * 0.7f);
    m_Exhaust.SetTexSize   (coreVector2(0.5f,0.25f));
}


// ****************************************************************
// 
void cFreezerEnemy::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cFreezerEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pGlow->UnbindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cFreezerEnemy::__RenderOwnUnder()
{
    // 
    m_Exhaust.Render();
}


// ****************************************************************
// move the freezer enemy
void cFreezerEnemy::__MoveOwn()
{
    // update rotation angle (opposed to arrow enemy)
    m_fAngle.Update(2.0f);

    // rotate around direction axis
    this->DefaultOrientate(m_fAngle);

    constexpr coreFloat fStrength = 0.07f;

    // 
    m_fAnimation.UpdateMod(0.75f, 1.0f);

    // 
    const coreFloat fLen  = fStrength * 25.0f;
    const coreFloat fSize = 1.0f - fStrength * 0.25f;

    // 
    m_Exhaust.SetSize       (coreVector3(fSize, fLen, fSize) * (0.9f * this->GetSize().x));
    m_Exhaust.SetTexOffset  (coreVector2(0.0f, m_fAnimation));
    m_Exhaust.SetPosition   (this->GetPosition () - this->GetDirection() * (m_Exhaust.GetSize().y + 2.35f * this->GetSize().x));
    m_Exhaust.SetDirection  (this->GetDirection());
    m_Exhaust.SetAlpha      (this->GetAlpha    () * (this->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
    m_Exhaust.Move();
}


// ****************************************************************
// constructor
cCinderEnemy::cCinderEnemy()noexcept
: m_fAngle     (0.0f)
, m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_enemy_cinder_high.md3");
    this->DefineModelLow ("ship_enemy_cinder_low.md3");

    // 
    m_Exhaust.DefineModel  ("object_sphere.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_flat_program");
    m_Exhaust.SetColor3    (COLOR_ENERGY_WHITE * 0.5f);
    m_Exhaust.SetTexSize   (coreVector2(1.0f,1.0f) * 3.0f);
}


// ****************************************************************
// 
void cCinderEnemy::__ResurrectOwn()
{
    // 
    g_pGlow->BindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cCinderEnemy::__KillOwn(const coreBool bAnimated)
{
    // 
    g_pGlow->UnbindObject(&m_Exhaust);
}


// ****************************************************************
// 
void cCinderEnemy::__RenderOwnUnder()
{
    // 
    m_Exhaust.Render();
}


// ****************************************************************
// move the cinder enemy
void cCinderEnemy::__MoveOwn()
{
    // update rotation angle (opposed to star enemy)
    m_fAngle.Update(2.0f);

    // rotate around the rotating direction axis
    this->DefaultMultiate(m_fAngle);

    // 
    m_fAnimation.UpdateMod(0.1f, 1.0f);

    // 
    m_Exhaust.SetSize     (coreVector3(1.0f,1.0f,1.0f) * this->GetVisualRadius() * 1.2f);
    m_Exhaust.SetTexOffset(coreVector2(0.0f, m_fAnimation));
    m_Exhaust.SetPosition (this->GetPosition ());
    m_Exhaust.SetDirection(this->GetDirection());
    m_Exhaust.SetAlpha    (this->GetAlpha    () * (this->HasStatus(ENEMY_STATUS_HIDDEN) ? 0.0f : 0.7f));
    m_Exhaust.Move();
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
    this->Configure(50, 0u, COLOR_SHIP_GREY);
    this->AddStatus(ENEMY_STATUS_SINGLE);
    this->AddStatus(ENEMY_STATUS_IMMORTAL);
    this->AddStatus(ENEMY_STATUS_HIDDEN);
    this->AddStatus(ENEMY_STATUS_WORTHLESS);

    // 
    g_pGame->GetEnemyManager()->BindEnemy(this);

    // 
    this->DefineModelHigh("object_sphere.md3");
    this->DefineModelLow ("object_sphere.md3");

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
    this->SetSize    (pPlayer->GetSize    () * 5.0f);

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
    m_Bubble.SetSize     (fAlpha * this->GetSize());
    m_Bubble.SetAlpha    (fAlpha);
    m_Bubble.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.5f));
    m_Bubble.Move();

    // 
    m_Ship.SetPosition (coreVector3(vNewPos, 0.0f));
    m_Ship.SetDirection(coreVector3(vNewDir, 0.0f));
    m_Ship.SetSize     (fAlpha * m_pPlayer->GetSize());
    m_Ship.SetAlpha    (fAlpha);
    m_Ship.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.5f));
    m_Ship.Move();

    // 
    m_pPlayer->SetPosition(this->GetPosition());
}