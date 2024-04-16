///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENEMY_H_
#define _P1_GUARD_ENEMY_H_

// TODO 4: manager: Find, ForEach, ForEachAll -> typed
// TODO 3: implement own enemy-types for custom-enemies which would require instancing
// TODO 3: add score-value to cEnemy class, either for base, or for extra score
// TODO 2: make sure ENEMY_STATUS_DAMAGING is used for damaging contact, and no additional checks and (duplicate) TakeDamage calls are made
// TODO 4: get rid of ENEMY_SIZE_FACTOR, because lots of places override it directly anyway
// TODO 4: move parent-child system to custom-enemy or boss-enemy, if not elsewhere required
// TODO 3: scripted enemy rotation should use DefaultRotate/DefaultOrientate instead of manually using coreVector2::Direction + SetDirection/SetOrientation, to utilize the rota cache
// TODO 1: completely remove PlayerSide aiming (for coop)
// TODO 3: normale gruppen-gegner mit TOP haben doppelte outline (einmal im batch, und einmal durch TOP)
// TODO 3 instancing for special effects, can be done with shield manager, or new system in enemy manager
// TODO 3: warrior model eiert etwas beim drehen um die Z-achse (siehe secret enemies bei mimic-wave)


// ****************************************************************
// enemy definitions
#define ENEMY_SET_INIT    (8u)      // initial size when creating a new enemy set
#define ENEMY_SET_COUNT   (16u)     // 
#define ENEMY_SIZE_FACTOR (1.05f)   // 

enum eEnemyStatus : coreUint32
{
    ENEMY_STATUS_DEAD         = 0x0001u,   // completely removed from the game
    ENEMY_STATUS_ASSIGNED     = 0x0002u,   // enemy is currently assigned to something
    ENEMY_STATUS_CHILD        = 0x0004u,   // 
    ENEMY_STATUS_SHIELDED     = 0x0008u,   // TODO 1: currently not used, needs to be automatic ? 
    ENEMY_STATUS_BOSS         = 0x0010u,   // 
    ENEMY_STATUS_SINGLE       = 0x0020u,   // 
    ENEMY_STATUS_ENERGY       = 0x0040u,   // 
    ENEMY_STATUS_BOTTOM       = 0x0080u,   // 
    ENEMY_STATUS_TOP          = 0x0100u,   //       
    ENEMY_STATUS_INVINCIBLE   = 0x0200u,   // 
    ENEMY_STATUS_DAMAGING     = 0x0400u,   // 
    ENEMY_STATUS_IMMORTAL     = 0x0800u,   // 
    ENEMY_STATUS_GHOST_PLAYER = 0x1000u,   // 
    ENEMY_STATUS_GHOST_BULLET = 0x2000u,   // 
    ENEMY_STATUS_GHOST        = ENEMY_STATUS_GHOST_PLAYER | ENEMY_STATUS_GHOST_BULLET,
    ENEMY_STATUS_HIDDEN       = 0x4000u,   // 
    ENEMY_STATUS_WORTHLESS    = 0x8000u,   // TODO 1: should be changed to explicit score value in configure (boss?) + setter ?
    ENEMY_STATUS_LIGHT = 0x10000u,
    ENEMY_STATUS_SECRET = 0x20000u
   // ENEMY_STATUS_UNDER = 0x20000u
};

    //ENEMY_STATUS_INVINCIBLE  = 0x0100u,   //    ### geschosse werden reflektiert (bubble)  
    //ENEMY_STATUS_DAMAGING    = 0x0200u,   //    ### kollision verursacht schaden (spikes)  
    //ENEMY_STATUS_IMMORTAL    = 0x0400u,   //    ### soll nicht sterben, geschosse gehen hindurch  
    //ENEMY_STATUS_GHOST       = 0x0800u,   //    ### keine kollisionen mit spieler oder geschosse mehr    

// ****************************************************************
// enemy entity interface
class INTERFACE cEnemy : public cShip
{
protected:
    coreFlow  m_fLifeTime;         // 
    coreFloat m_fLifeTimeBefore;   // 

    coreUint8 m_iLastAttacker;     // 
    coreBool  m_bWasDamaged;       // 
    coreUint16 m_iScore;
    coreInt32 m_iDamageForwarded;   // 

    coreSet<cEnemy*> m_apMember;   // 

    static coreUint8 s_iExhaustCount;   // 


public:
    cEnemy()noexcept;
    virtual ~cEnemy()override = default;

    FRIEND_CLASS(cEnemyManager)
    ENABLE_COPY (cEnemy)
    ENABLE_ID

    // configure the enemy
    void Configure(const coreInt32 iHealth, const coreUint16 iScore, const coreVector3 vColor, const coreBool bInverted = false, const coreBool bIgnored = false);

    // render and move the enemy
    void Render()final;
    void Move  ()final;

    // reduce current health
    coreInt32 TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2 vImpact, cPlayer* OUTPUT pAttacker);

    // control life and death
    void Resurrect();
    void Kill     (const coreBool bAnimated);

    // reset base properties
    void ResetProperties();

    // 
    void RefreshColorAll(const coreFloat fFactor);
    void RefreshColorAll();
    void InvokeBlinkAll ();

    // 
    inline const coreBool& WasDamaged()const {return m_bWasDamaged;}
    
    inline const coreInt32& GetDamageForwarded()const {return m_iDamageForwarded;}
    inline void ResetDamageForwarded() {m_iDamageForwarded = 0;}

    // 
    inline coreBool IsParent()const {return !m_apMember.empty() && !HAS_FLAG(m_iStatus, ENEMY_STATUS_CHILD);}
    inline coreBool IsChild ()const {return !m_apMember.empty() &&  HAS_FLAG(m_iStatus, ENEMY_STATUS_CHILD);}

    //
    void ChangeToBottom();
    void ChangeToTop   ();
    void ChangeToNormal();
    
    inline coreUintW GetOutlineStyle()const {return HAS_FLAG(m_iStatus, ENEMY_STATUS_LIGHT) ? OUTLINE_STYLE_LIGHT : OUTLINE_STYLE_FULL;}

    // 
    cPlayer*    LastAttacker        ()const;
    cPlayer*    NearestPlayerSide   ()const;
    cPlayer*    NearestPlayerSideRev()const;
    cPlayer*    NearestPlayerDual   (const coreUintW iIndex)const;
    coreVector2 AimAtPlayerSide     ()const;
    coreVector2 AimAtPlayerSideRev  ()const;
    coreVector2 AimAtPlayerDual     (const coreUintW iIndex)const;

    // get object properties
    inline  const coreFloat& GetLifeTime      ()const {return m_fLifeTime;}
    inline  const coreFloat& GetLifeTimeBefore()const {return m_fLifeTimeBefore;}
    virtual eSoundEffect     GetExplosionSound()const {return SOUND_ENEMY_EXPLOSION_01;}

    // enemy configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_ship_blink_inst_program";}


protected:
    // 
    void _SetParent(cEnemy* pParent);

    // 
    static coreFloat _GetExhaustOffset();


private:
    // own routines for derived classes (render functions executed by manager)
    virtual void __ResurrectOwn   ()                         {}
    virtual void __KillOwn        (const coreBool bAnimated) {}
    virtual void __RenderOwnBottom()                         {}
    virtual void __RenderOwnUnder ()                         {}
    virtual void __RenderOwnOver  ()                         {}
    virtual void __RenderOwnTop   ()                         {}
    virtual void __MoveOwn        ()                         {}
};


// ****************************************************************
// enemy manager class
class cEnemyManager final
{
private:
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
        coreMemoryPool oMemoryPool;   // 
        coreList<T*>   apEnemyPool;   // semi-dynamic container with all enemies

        sEnemySet()noexcept;
        ~sEnemySet()final;
    };


private:
    sEnemySetGen*    m_apEnemySet[ENEMY_SET_COUNT];   // enemy sets (each for a different inherited enemy class)
    coreSet<cEnemy*> m_apAdditional;                  // pointers to additional enemies


public:
    cEnemyManager()noexcept;
    ~cEnemyManager();

    DISABLE_COPY(cEnemyManager)

    // render and move the enemy manager
    void Render      ();
    void RenderBottom();
    void RenderUnder ();
    void RenderOver  ();
    void RenderTop   ();
    void MoveBefore  ();
    void MoveMiddle  ();
    void MoveAfter   ();

    // add and remove enemies
    template <typename T> RETURN_RESTRICT T* AllocateEnemy();
    void FreeEnemy(cEnemy** OUTPUT ppEnemy);
    void ClearEnemies(const coreBool bAnimated);

    // 
    cEnemy* FindEnemy   (const coreVector2 vPosition)const;
    cEnemy* FindEnemyRev(const coreVector2 vPosition)const;
    template <typename F> void ForEachEnemy(F&& nFunction)const;   // [](cEnemy* OUTPUT pEnemy) -> void

    // 
    template <typename T> void PrefetchEnemy();

    // 
    inline void BindEnemy  (cEnemy* pEnemy) {ASSERT(!m_apAdditional.count(pEnemy)) m_apAdditional.insert(pEnemy);}
    inline void UnbindEnemy(cEnemy* pEnemy) {ASSERT( m_apAdditional.count(pEnemy)) m_apAdditional.erase (pEnemy);}

    // 
    inline coreUintW GetNumEnemiesAlive()const {coreUintW iNum = 0u; this->ForEachEnemy([&](void*) {++iNum;}); return iNum;}
};


// ****************************************************************
// enemy squad class
class cEnemySquad final
{
private:
    coreList<cEnemy*> m_apEnemy;   // 


public:
    cEnemySquad() = default;
    ~cEnemySquad();

    FRIEND_CLASS(cEnemyManager)
    ENABLE_COPY (cEnemySquad)

    // 
    template <typename T> void AllocateEnemies(const coreUint8 iNumEnemies);
    void FreeEnemies();
    void ClearEnemies(const coreBool bAnimated)const;

    // 
    cEnemy* FindEnemy   (const coreVector2 vPosition)const;
    cEnemy* FindEnemyRev(const coreVector2 vPosition)const;
    template <typename F> void ForEachEnemy   (F&& nFunction)const;   // [](cEnemy* OUTPUT pEnemy, const coreUintW i) -> void
    template <typename F> void ForEachEnemyAll(F&& nFunction)const;   // [](cEnemy* OUTPUT pEnemy, const coreUintW i) -> void

    // 
    inline cEnemy*   GetEnemy(const coreUintW iIndex)const {ASSERT(iIndex < m_apEnemy.size()) return m_apEnemy[iIndex];}
    inline coreUintW GetIndex(const cEnemy*   pEnemy)const {const coreUintW iIndex = m_apEnemy.first_index(c_cast<cEnemy*>(pEnemy)); ASSERT(iIndex < m_apEnemy.size()) return iIndex;}

    // 
    inline coreUintW GetNumEnemies        ()const {return m_apEnemy.size();}
    inline coreUintW GetNumEnemiesAlive   ()const {return std::count_if(m_apEnemy.begin(), m_apEnemy.end(), [](const cEnemy* pEnemy) {return !pEnemy->HasStatus(ENEMY_STATUS_DEAD);});}
    inline coreFloat GetNumEnemiesAlivePct()const {return I_TO_F(this->GetNumEnemiesAlive()) * RCP(I_TO_F(this->GetNumEnemies()));}
    inline coreBool  IsFinished           ()const {return std::none_of (m_apEnemy.begin(), m_apEnemy.end(), [](const cEnemy* pEnemy) {return !pEnemy->HasStatus(ENEMY_STATUS_DEAD);});}


private:
    // 
    static cEnemyManager* __GetDefaultEnemyManager();
};


// ****************************************************************
// dummy enemy class
class cDummyEnemy final : public cEnemy
{
public:
    cDummyEnemy()noexcept;

    ENABLE_COPY(cDummyEnemy)
    ASSIGN_ID(0, "Dummy")
};


// ****************************************************************
// scout enemy class
class cScoutEnemy final : public cEnemy
{
private:
    coreObject3D m_Exhaust;   // 

    coreFlow m_fAnimation;    // 


public:
    cScoutEnemy()noexcept;

    ENABLE_COPY(cScoutEnemy)
    ASSIGN_ID(1, "Scout")

    // 
    void ResetProperties() {cEnemy::ResetProperties(); m_fAnimation = cEnemy::_GetExhaustOffset();}

    // get object properties
    inline eSoundEffect GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_09;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// warrior enemy class
class cWarriorEnemy final : public cEnemy
{
private:
    coreObject3D m_aExhaust[2];   // 

    coreFlow m_fAnimation;        // 


public:
    cWarriorEnemy()noexcept;

    ENABLE_COPY(cWarriorEnemy)
    ASSIGN_ID(2, "Warrior")

    // 
    void ResetProperties() {cEnemy::ResetProperties(); m_fAnimation = cEnemy::_GetExhaustOffset();}

    // get object properties
    inline eSoundEffect GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_02;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// star enemy class
class cStarEnemy final : public cEnemy
{
private:
    coreObject3D m_Exhaust;   // 

    coreFlow m_fAngle;        // rotation angle
    coreFlow m_fAnimation;    // 


public:
    cStarEnemy()noexcept;

    ENABLE_COPY(cStarEnemy)
    ASSIGN_ID(3, "Star")

    // 
    void ResetProperties() {cEnemy::ResetProperties(); m_fAngle = 0.0f; m_fAnimation = cEnemy::_GetExhaustOffset();}

    // 
    inline void SetAngle(const coreFloat fAngle) {m_fAngle = fAngle;}

    // get object properties
    inline const coreFloat& GetAngle         ()const       {return m_fAngle;}
    inline eSoundEffect     GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_10;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// arrow enemy class
class cArrowEnemy final : public cEnemy
{
private:
    coreObject3D m_Exhaust;   // 

    coreFlow m_fAngle;        // rotation angle
    coreFlow m_fAnimation;    // 


public:
    cArrowEnemy()noexcept;

    ENABLE_COPY(cArrowEnemy)
    ASSIGN_ID(4, "Arrow")

    // 
    void ResetProperties() {cEnemy::ResetProperties(); m_fAngle = 0.0f; m_fAnimation = cEnemy::_GetExhaustOffset();}

    // 
    inline void SetAngle(const coreFloat fAngle) {m_fAngle = fAngle;}

    // get object properties
    inline const coreFloat& GetAngle         ()const       {return m_fAngle;}
    inline eSoundEffect     GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_04;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// miner enemy class
class cMinerEnemy final : public cEnemy
{
private:
    coreObject3D m_Exhaust;   // 

    coreFlow m_fAnimation;    // 


public:
    cMinerEnemy()noexcept;

    ENABLE_COPY(cMinerEnemy)
    ASSIGN_ID(5, "Miner")

    // 
    void ResetProperties() {cEnemy::ResetProperties(); m_fAnimation = cEnemy::_GetExhaustOffset();}

    // get object properties
    inline eSoundEffect GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_04;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// freezer enemy class
class cFreezerEnemy final : public cEnemy
{
private:
    coreObject3D m_Exhaust;   // 

    coreFlow m_fAngle;        // rotation angle
    coreFlow m_fAnimation;    // 


public:
    cFreezerEnemy()noexcept;

    ENABLE_COPY(cFreezerEnemy)
    ASSIGN_ID(6, "Freezer")

    // 
    void ResetProperties() {cEnemy::ResetProperties(); m_fAngle = 0.0f; m_fAnimation = cEnemy::_GetExhaustOffset();}

    // 
    inline void SetAngle(const coreFloat fAngle) {m_fAngle = fAngle;}

    // get object properties
    inline const coreFloat& GetAngle         ()const       {return m_fAngle;}
    inline eSoundEffect     GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_06;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// cinder enemy class
class cCinderEnemy final : public cEnemy
{
private:
    coreObject3D m_Exhaust;   // 

    coreFlow m_fAngle;        // rotation angle
    coreFlow m_fAnimation;    // 


public:
    cCinderEnemy()noexcept;

    ENABLE_COPY(cCinderEnemy)
    ASSIGN_ID(7, "Cinder")

    // 
    void ResetProperties() {cEnemy::ResetProperties(); m_fAngle = 0.0f; m_fAnimation = cEnemy::_GetExhaustOffset();}

    // 
    inline void SetAngle(const coreFloat fAngle) {m_fAngle = fAngle;}

    // get object properties
    inline const coreFloat& GetAngle         ()const       {return m_fAngle;}
    inline eSoundEffect     GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_07;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;
};


// ****************************************************************
// meteor enemy class
class cMeteorEnemy final : public cEnemy
{
public:
    cMeteorEnemy()noexcept;

    ENABLE_COPY(cMeteorEnemy)
    ASSIGN_ID(8, "Meteor")

    // get object properties
    inline eSoundEffect GetExplosionSound()const final {return SOUND_ENEMY_EXPLOSION_07;}

    // enemy configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_meteor_blink_inst_program";}
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
// repair enemy class
class cRepairEnemy final : public cEnemy
{
private:
    cPlayer* m_pPlayer;         // 

    coreVector2 m_vDirection;   // 
    coreFlow    m_fAnimation;   // 

    coreObject3D m_Bubble;      // 
    cLodObject   m_Ship;        // 


public:
    cRepairEnemy()noexcept;
    ~cRepairEnemy()final;

    ENABLE_COPY(cRepairEnemy)
    ASSIGN_ID(1337, "Repair")

    // 
    void AssignPlayer(cPlayer* pPlayer);

    // 
    inline cPlayer* GetPlayer()const {return m_pPlayer;}


private:
    // execute own routines
    void __ResurrectOwn ()final;
    void __KillOwn      (const coreBool bAnimated)final;
    void __RenderOwnOver()final;
    void __MoveOwn      ()final;
};


// ****************************************************************
// constructor
template <typename T> cEnemyManager::sEnemySet<T>::sEnemySet()noexcept
{
    STATIC_ASSERT(T::ID != cCustomEnemy::ID)

    // set shader-program
    oEnemyActive.DefineProgram(T::ConfigProgramInstancedName());

    // 
    oEnemyActive.CreateCustom(sizeof(coreFloat), [](coreVertexBuffer* OUTPUT pBuffer)
    {
        pBuffer->DefineAttribute(SHIP_SHADER_ATTRIBUTE_BLINK_NUM, 1u, GL_FLOAT, sizeof(coreFloat), false, 0u, 0u);
    },
    [](coreByte* OUTPUT pData, const coreObject3D* pEnemy)
    {
        (*r_cast<coreFloat*>(pData)) = d_cast<const cEnemy*>(pEnemy)->GetBlink();
    },
    [](const coreProgramPtr& pProgram, const coreObject3D* pEnemy)
    {
        d_cast<const cEnemy*>(pEnemy)->_EnableBlink(pProgram);
    });

    // add enemy set to global shadow and outline
    cShadow::GetGlobalContainer()->BindList(&oEnemyActive);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindList(&oEnemyActive);

    // 
    oMemoryPool.Configure(sizeof(T), ENEMY_SET_INIT);

    // set enemy pool to initial size
    apEnemyPool.resize(ENEMY_SET_INIT);
    apEnemyPool[0] = POOLED_NEW(oMemoryPool, T);   // already request resources
}


// ****************************************************************
// destructor
template <typename T> cEnemyManager::sEnemySet<T>::sEnemySet::~sEnemySet()
{
    // 
    FOR_EACH(it, apEnemyPool) POOLED_DELETE(oMemoryPool, *it)

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
        if(!pEnemy) pEnemy = POOLED_NEW(pSet->oMemoryPool, T);
        if(!pEnemy->HasStatus(ENEMY_STATUS_ASSIGNED))
        {
            // prepare enemy and add to active list
            pEnemy->ResetProperties();
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
    const coreList<coreObject3D*>& oEnemyList = Core::Manager::Object->GetObjectList(TYPE_ENEMY);
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
        T* pEnemy = cEnemySquad::__GetDefaultEnemyManager()->AllocateEnemy<T>();
        m_apEnemy.push_back(pEnemy);
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
        if(pEnemy->HasStatus(ENEMY_STATUS_DEAD) && !pEnemy->ReachedDeath()) continue;    // # for scripting

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


#endif // _P1_GUARD_ENEMY_H_