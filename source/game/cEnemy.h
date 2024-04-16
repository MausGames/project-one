///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENEMY_H_
#define _P1_GUARD_ENEMY_H_

// TODO: disable texture filtering for enemy texture (NEAREST, also default_black and default_white)
// TODO: manager: Find, ForEach, ForEachAll -> typed 
// TODO: implement own enemy-types for custom-enemies which would require instancing
// TODO: virtual void Render()override; -> final


// ****************************************************************
// enemy definitions
#define ENEMY_SET_INIT    (8u)      // initial size when creating a new enemy set
#define ENEMY_SET_COUNT   (8u)      // 
//#define ENEMY_AREA_FACTOR (1.2f)    // 
#define ENEMY_SIZE_FACTOR (1.05f)   // 

enum eEnemyStatus : coreUint16
{
    ENEMY_STATUS_DEAD        = 0x0001u,   // completely removed from the game
    ENEMY_STATUS_ASSIGNED    = 0x0002u,   // enemy is currently assigned to something
    ENEMY_STATUS_BOSS        = 0x0004u,   // 
    ENEMY_STATUS_SINGLE      = 0x0008u,   // 
    ENEMY_STATUS_ENERGY      = 0x0010u,   // 
    ENEMY_STATUS_CHILD       = 0x0020u,   // 
    ENEMY_STATUS_SHIELDED    = 0x0040u,   // 
    ENEMY_STATUS_INVINCIBLE  = 0x0080u,   // 
    ENEMY_STATUS_IMMORTAL    = 0x0100u    // 
};


// ****************************************************************
// enemy entity interface
class INTERFACE cEnemy : public cShip
{
protected:
    coreFlow  m_fLifeTime;           // 
    coreFloat m_fLifeTimeBefore;     // 

    coreSet<cEnemy*> m_apMember;     // 

    static cEnemy* s_pLastDamaged;   // 


public:
    cEnemy()noexcept;
    virtual ~cEnemy()override;

    FRIEND_CLASS(cEnemyManager)
    ENABLE_COPY (cEnemy)
    ENABLE_ID

    // configure the enemy
    void Configure (const coreInt32 iHealth, const coreVector3& vColor, const coreBool bInverted = false);
    void GiveShield(const coreUint8 iElement, const coreInt16 iHealth = 0);

    // render and move the enemy
    virtual void Render()override;
    void         Move  ()final;

    // reduce current health
    coreBool TakeDamage(coreInt32 iDamage, const coreUint8 iElement, const coreVector2& vImpact, cPlayer* pAttacker);

    // control life and death
    void Resurrect();
    void Resurrect(const coreSpline2* pPath,     const coreVector2& vFactor, const coreVector2& vOffset);
    void Resurrect(const coreVector2& vPosition, const coreVector2& vDirection);
    void Kill     (const coreBool     bAnimated);

    // 
    inline coreBool IsParent()const {return !CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_CHILD) && !m_apMember.empty();}
    inline coreBool IsChild ()const {return  CONTAINS_FLAG(m_iStatus, ENEMY_STATUS_CHILD) && !m_apMember.empty();}

    // 
    cPlayer*    NearestPlayer()const;
    coreVector2 AimAtPlayer  ()const;
    coreVector2 AimAtPlayer  (const cPlayer* pPlayer)const;

    // get object properties
    inline const coreFloat& GetLifeTime      ()const {return m_fLifeTime;}
    inline const coreFloat& GetLifeTimeBefore()const {return m_fLifeTimeBefore;}

    // 
    static inline cEnemy* GetLastDamaged() {return s_pLastDamaged;}


protected:
    // 
    void _SetParent(cEnemy* OUTPUT pParent);


private:
    // own routines for derived classes (render functions executed by manager)
    virtual void __ResurrectOwn   ()                         {}
    virtual void __KillOwn        (const coreBool bAnimated) {}
    virtual void __RenderOwnUnder ()                         {}
    virtual void __RenderOwnAttack()                         {}
    virtual void __RenderOwnOver  ()                         {}
    virtual void __MoveOwn        ()                         {}
};


// ****************************************************************
// enemy squad class
class cEnemySquad final
{
private:
    std::vector<cEnemy*> m_apEnemy;   // 


public:
    cEnemySquad() = default;
    ~cEnemySquad();

    FRIEND_CLASS(cEnemyManager)
    ENABLE_COPY (cEnemySquad)

    // 
    template <typename T> void AllocateEnemies(const coreUint8 iNumEnemies);
    void FreeEnemies();
    void ClearEnemies(const coreBool bAnimated);

    // 
    cEnemy* FindEnemy   (const coreVector2& vPosition)const;
    cEnemy* FindEnemyRev(const coreVector2& vPosition)const;
    template <typename F> void ForEachEnemy   (F&& nFunction)const;   // [](cEnemy* OUTPUT pEnemy, const coreUintW i) -> void
    template <typename F> void ForEachEnemyAll(F&& nFunction)const;   // [](cEnemy* OUTPUT pEnemy, const coreUintW i) -> void

    // 
    inline cEnemy* GetEnemy(const coreUintW iIndex)const {ASSERT(iIndex < m_apEnemy.size()) return m_apEnemy[iIndex];}

    // 
    inline coreUintW GetNumEnemies        ()const {return m_apEnemy.size();}
    inline coreUintW GetNumEnemiesAlive   ()const {return std::count_if(m_apEnemy.begin(), m_apEnemy.end(), [](const cEnemy* pEnemy) {return !CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD);});}
    inline coreFloat GetNumEnemiesAlivePct()const {return I_TO_F(this->GetNumEnemiesAlive()) * RCP(I_TO_F(this->GetNumEnemies()));}
    inline coreBool  IsFinished           ()const {return std::none_of (m_apEnemy.begin(), m_apEnemy.end(), [](const cEnemy* pEnemy) {return !CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD);});}
};


// ****************************************************************
// enemy manager class
class cEnemyManager final
{
private:
    // 
    using uRepeatFunc = void (*) (cEnemy* OUTPUT, const coreUint8, const coreVariant16&);

    // 
    struct sRepeatEntry final
    {
        cEnemy*       pEnemy;       // 
        coreUint8     iMaxRepeat;   // 
        coreUint8     iCurRepeat;   // 
        coreUint8     iMaxDelay;    // 
        coreUint8     iCurDelay;    // 
        coreVariant16 oData;        // 
        uRepeatFunc   nFunction;    // 
    };

    // enemy set structure
    struct INTERFACE sEnemySetGen
    {
        coreBatchList oEnemyActive;   // list with active enemies
        coreUintW     iTopEnemy;      // 

        sEnemySetGen()noexcept;
        virtual ~sEnemySetGen() = default;
    };
    template <typename T> struct sEnemySet final : public sEnemySetGen
    {
        std::vector<T*> apEnemyPool;   // semi-dynamic container with all enemies

        sEnemySet()noexcept;
        ~sEnemySet()final;
    };


private:
    sEnemySetGen*    m_apEnemySet[ENEMY_SET_COUNT];   // enemy sets (each for a different inherited enemy class)
    coreSet<cEnemy*> m_apAdditional;                  // pointers to additional enemies

    std::vector<sRepeatEntry> m_aRepeatEntry;         // 


public:
    cEnemyManager()noexcept;
    ~cEnemyManager();

    DISABLE_COPY(cEnemyManager)

    // render and move the enemy manager
    void Render      ();
    void RenderUnder ();
    void RenderAttack();
    void RenderOver  ();
    void Move        ();

    // add and remove enemies
    template <typename T> RETURN_RESTRICT T* AllocateEnemy();
    void FreeEnemy(cEnemy** OUTPUT ppEnemy);
    void ClearEnemies(const coreBool bAnimated);

    // 
    cEnemy* FindEnemy   (const coreVector2& vPosition)const;
    cEnemy* FindEnemyRev(const coreVector2& vPosition)const;
    template <typename F> void ForEachEnemy(F&& nFunction)const;   // [](cEnemy* OUTPUT pEnemy) -> void

    // 
    template <typename T> void PrefetchEnemy();

    // 
    void AttachFunction(cEnemy* pEnemy, const coreUint8 iRepeat, const coreFloat fDelay, const coreVariant16& oData, uRepeatFunc nFunction);   // [](cEnemy* OUTPUT pEnemy, const coreUint8 iCurRepeat, const coreVariant16& oData) -> void

    // 
    inline void BindEnemy  (cEnemy* pEnemy) {ASSERT(!m_apAdditional.count(pEnemy)) m_apAdditional.insert(pEnemy);}
    inline void UnbindEnemy(cEnemy* pEnemy) {ASSERT( m_apAdditional.count(pEnemy)) m_apAdditional.erase (pEnemy);}

    // 
    inline coreUintW GetNumEnemiesAlive()const {coreUintW iNum = 0u; this->ForEachEnemy([&](void*) {++iNum;}); return iNum;}
};


// ****************************************************************
// scout enemy class
class cScoutEnemy final : public cEnemy
{
public:
    cScoutEnemy()noexcept;

    ENABLE_COPY(cScoutEnemy)
    ASSIGN_ID(1, "Scout")
};


// ****************************************************************
// warrior enemy class
class cWarriorEnemy final : public cEnemy
{
public:
    cWarriorEnemy()noexcept;

    ENABLE_COPY(cWarriorEnemy)
    ASSIGN_ID(2, "Warrior")
};


// ****************************************************************
// star enemy class
class cStarEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cStarEnemy()noexcept;

    ENABLE_COPY(cStarEnemy)
    ASSIGN_ID(3, "Star")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// arrow enemy class
class cArrowEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cArrowEnemy()noexcept;

    ENABLE_COPY(cArrowEnemy)
    ASSIGN_ID(4, "Arrow")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// miner enemy class
class cMinerEnemy final : public cEnemy
{
public:
    cMinerEnemy()noexcept;

    ENABLE_COPY(cMinerEnemy)
    ASSIGN_ID(5, "Miner")
};


// ****************************************************************
// freezer enemy class
class cFreezerEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cFreezerEnemy()noexcept;

    ENABLE_COPY(cFreezerEnemy)
    ASSIGN_ID(6, "Freezer")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// cinder enemy class
class cCinderEnemy final : public cEnemy
{
private:
    coreFlow m_fAngle;   // rotation angle


public:
    cCinderEnemy()noexcept;

    ENABLE_COPY(cCinderEnemy)
    ASSIGN_ID(7, "Cinder")

    // 
    inline void SetAngle(const coreFloat fAngle) {m_fAngle = fAngle;}


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// custom enemy class
class cCustomEnemy final : public cEnemy
{
public:
    cCustomEnemy()noexcept;
    ~cCustomEnemy()final;

    ENABLE_COPY(cCustomEnemy)
    ASSIGN_ID(666, "Custom")

    // 
    inline void SetParent(cEnemy* OUTPUT pParent) {this->_SetParent(pParent);}
};


// ****************************************************************
// 
template <typename T> void cEnemySquad::AllocateEnemies(const coreUint8 iNumEnemies)
{
    ASSERT(m_apEnemy.empty())

    // 
    m_apEnemy.reserve(iNumEnemies);

    // 
    for(coreUintW i = iNumEnemies; i--; )
    {
        T* pEnemy = g_pGame->GetEnemyManager()->AllocateEnemy<T>();
        m_apEnemy.push_back(pEnemy);


        pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f));
    }
}


// ****************************************************************
// 
template <typename F> void cEnemySquad::ForEachEnemy(F&& nFunction)const
{
    // 
    for(coreUintW i = 0u, ie = m_apEnemy.size(); i < ie; ++i)
    {
        cEnemy* pEnemy = m_apEnemy[i];
        if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD) && !pEnemy->ReachedDeath()) continue;    // # for scripting

        // 
        nFunction(pEnemy, i);
    }
}

template <typename F> void cEnemySquad::ForEachEnemyAll(F&& nFunction)const
{
    // 
    for(coreUintW i = 0u, ie = m_apEnemy.size(); i < ie; ++i)
    {
        // 
        nFunction(m_apEnemy[i], i);
    }
}


// ****************************************************************
// constructor
template <typename T> cEnemyManager::sEnemySet<T>::sEnemySet()noexcept
{
    STATIC_ASSERT(T::ID != cCustomEnemy::ID)

    // set shader-program
    oEnemyActive.DefineProgram("object_ship_blink_inst_program");

    // 
    oEnemyActive.CreateCustom(sizeof(coreFloat), [](coreVertexBuffer* OUTPUT pBuffer)
    {
        pBuffer->DefineAttribute(SHIP_SHADER_ATTRIBUTE_BLINK, 1u, GL_FLOAT, false, 0u);
    },
    [](coreFloat* OUTPUT pData, const cEnemy* pEnemy)
    {
        (*pData) = pEnemy->GetBlink();
    },
    [](const coreProgramPtr& pProgram, const cEnemy* pEnemy)
    {
        pEnemy->_EnableBlink(pProgram);
    });

    // add enemy set to global shadow and outline
    cShadow::GetGlobalContainer()->BindList(&oEnemyActive);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindList(&oEnemyActive);

    // set enemy pool to initial size
    apEnemyPool.resize(ENEMY_SET_INIT);
    apEnemyPool[0] = new T();   // already request resources
}


// ****************************************************************
// destructor
template <typename T> cEnemyManager::sEnemySet<T>::~sEnemySet()
{
    // 
    FOR_EACH(it, apEnemyPool)
        SAFE_DELETE(*it)

    // remove enemy set from global shadow and outline
    cShadow::GetGlobalContainer()->UnbindList(&oEnemyActive);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindList(&oEnemyActive);

    // clear memory
    apEnemyPool.clear();
}


// ****************************************************************
// add enemy to the game
template <typename T> RETURN_RESTRICT T* cEnemyManager::AllocateEnemy()
{
    // get requested enemy set
    this->PrefetchEnemy<T>();
    sEnemySet<T>* pSet = d_cast<sEnemySet<T>*>(m_apEnemySet[T::ID]);

    // save current pool size
    const coreUintW iSize = pSet->apEnemyPool.size();

    // loop through all enemies
    while(pSet->iTopEnemy < iSize)
    {
        // check current enemy status
        T*& pEnemy = pSet->apEnemyPool[pSet->iTopEnemy++];
        if(!pEnemy) pEnemy = new T();
        if(!CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_ASSIGNED))
        {
            // prepare enemy and add to active list
            pEnemy->AddStatus(ENEMY_STATUS_ASSIGNED);
            pSet->oEnemyActive.BindObject(pEnemy);

            return pEnemy;
        }
    }

    // increase list and pool size by 100%
    pSet->oEnemyActive.Reallocate(iSize * 2u);
    pSet->apEnemyPool .resize    (iSize * 2u);

    // execute again with first new enemy
    return this->AllocateEnemy<T>();
}


// ****************************************************************
// 
template <typename F> void cEnemyManager::ForEachEnemy(F&& nFunction)const
{
    // 
    const std::vector<coreObject3D*>& oEnemyList = Core::Manager::Object->GetObjectList(TYPE_ENEMY);
    FOR_EACH(it, oEnemyList)
    {
        cEnemy* pEnemy = d_cast<cEnemy*>(*it);
        if(!pEnemy || pEnemy->IsChild()) continue;

        // 
        nFunction(pEnemy);
    }
}


// ****************************************************************
// 
template <typename T> void cEnemyManager::PrefetchEnemy()
{
    STATIC_ASSERT(T::ID < ENEMY_SET_COUNT)

    if(!m_apEnemySet[T::ID])
    {
        // create new enemy set
        m_apEnemySet[T::ID] = new sEnemySet<T>();

        Core::Log->Info("Enemy Set (%s) created", T::Name);
    }
}


#endif // _P1_GUARD_ENEMY_H_