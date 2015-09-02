//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ENEMY_H_
#define _P1_GUARD_ENEMY_H_

// TODO: definitely add automatic enemy batching like the bullet manager! 


// ****************************************************************
// enemy definitions
#define ENEMY_SET_INIT_SIZE          (8u)   // initial allocation size when creating a new enemy set
#define ENEMY_SHADER_ATTRIBUTE_BLINK (CORE_SHADER_ATTRIBUTE_DIV_TEXPARAM_NUM + 1u)

enum eEnemyStatus : coreUint8
{
    ENEMY_STATUS_DEAD   = 0x01u,   // completely removed from the game
    ENEMY_STATUS_SILENT = 0x02u,   // not able to attack
    ENEMY_STATUS_BOSS   = 0x04u,   // 
    ENEMY_STATUS_READY  = 0x08u    // enemy is ready to be created
};


// ****************************************************************
// enemy entity interface
class INTERFACE cEnemy : public cShip
{
protected:
    coreFlow  m_fLifeTime;                     // 
    coreUint8 m_aiNumShots[2];                 // (0 = value, 1 = helper) 

    std::function<void(cEnemy*)> m_nRoutine;   // custom move routine


public:
    cEnemy()noexcept;
    virtual ~cEnemy() {}

    FRIEND_CLASS(cEnemyManager)
    ENABLE_COPY (cEnemy)
    ENABLE_ID

    // configure the enemy
    void Configure(const coreInt32& iHealth, const coreVector3& vColor);

    // render and move the enemy
    void Render()override;
    void Move  ()override;

    // reduce current health
    void TakeDamage(const coreInt32& iDamage, cPlayer* pAttacker);

    // control life and death
    void Resurrect();
    void Resurrect(const coreSpline2& oPath,     const coreVector2& vFactor, const coreVector2& vOffset);
    void Resurrect(const coreVector2& vPosition, const coreVector2& vDirection);
    void Kill     (const coreBool&    bAnimated);

    // 
    template <typename F> void ChangeRoutine(F&& nRoutine) {m_nRoutine = nRoutine;}   // [](cEnemy* OUTPUT pEnemy) -> void

    // (raw parameters are multiplied with FOREGROUND_AREA) 
    coreBool DefaultMovePath     (const coreSpline2& oRawPath, const coreVector2& vFactor, const coreVector2& vRawOffset, const coreFloat& fRawDistance);
    coreBool DefaultMoveTarget   (const coreVector2& vTarget, const coreFloat& fSpeedMove, const coreFloat& fSpeedTurn);
    coreBool DefaultMoveSmooth   (const coreVector2& vRawPosition, const coreFloat& fSpeedMove, const coreFloat& fClampMove);
    void     DefaultMoveLerp     (const coreVector2& vFromRawPos, const coreVector2& vToRawPos, const coreFloat& fTime);
    void     DefaultRotate       (const coreFloat& fAngle);
    coreBool DefaultRotateSmooth (const coreVector2& vDirection, const coreFloat& fSpeedTurn, const coreFloat& fClampTurn);
    void     DefaultRotateLerp   (const coreFloat& fFromAngle, const coreFloat& fToAngle, const coreFloat& fTime);
    void     DefaultOrientate    (const coreFloat& fAngle);
    void     DefaultOrientateLerp(const coreFloat& fFromAngle, const coreFloat& fToAngle, const coreFloat& fTime);
    void     DefaultMultiate     (const coreFloat& fAngle);
    coreBool DefaultShoot        (const coreFloat& fFireRate, const coreUint8& iMaxShots);

    // get object properties
    inline const coreFloat& GetLifeTime()const {return m_fLifeTime;}
    inline const coreUint8& GetNumShots()const {return m_aiNumShots[0];}


private:
    // own routines for derived classes (render functions executed by enemy manager)
    virtual void __ResurrectOwn   ()                          {}
    virtual void __KillOwn        (const coreBool& bAnimated) {}
    virtual void __RenderOwnWeak  ()                          {}
    virtual void __RenderOwnStrong()                          {}
    virtual void __RenderOwnAfter ()                          {}
    virtual void __MoveOwn        ()                          {}
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

        sEnemySetGen()noexcept;
        virtual ~sEnemySetGen() {}
    };
    template <typename T> struct sEnemySet final : public sEnemySetGen
    {
        std::vector<T> aEnemyPool;   // semi-dynamic container with all enemies
        coreUintW      iCurEnemy;    // current enemy (next one to check)

        sEnemySet()noexcept;
    };


private:
    coreLookup<coreInt32, sEnemySetGen*> m_apEnemySet;   // enemy sets (each for a different inherited enemy class)
    coreSet<cEnemy*> m_apAdditional;                     // pointers to additional enemies


public:
    cEnemyManager()noexcept;
    ~cEnemyManager();

    DISABLE_COPY(cEnemyManager)

    // render and move the enemy manager
    void Render      ();
    void RenderWeak  ();
    void RenderStrong();
    void RenderAfter ();
    void Move        ();

    // add and remove enemies
    template <typename T> RETURN_RESTRICT T* AddEnemy();
    void ClearEnemies(const coreBool& bAnimated);

    // 
    cEnemy* FindEnemy(const coreVector2& vPosition);
    template <typename F> void ForEachEnemy(F&& nFunction);   // [](cEnemy* OUTPUT pEnemy) -> void

    // 
    inline void BindEnemy  (cEnemy* pEnemy) {ASSERT(!m_apAdditional.count(pEnemy)) m_apAdditional.insert(pEnemy);}
    inline void UnbindEnemy(cEnemy* pEnemy) {ASSERT( m_apAdditional.count(pEnemy)) m_apAdditional.erase (pEnemy);}
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
    void __MoveOwn()override;
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
    void __MoveOwn()override;
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
    void __MoveOwn()override;
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


private:
    // execute own routines
    void __MoveOwn()override;
};


// ****************************************************************
// constructor
template <typename T> cEnemyManager::sEnemySet<T>::sEnemySet()noexcept
: iCurEnemy (0u)
{
    // set shader-program
    oEnemyActive.DefineProgram("object_ship_inst_program");

    // 
    oEnemyActive.CreateCustom(sizeof(coreFloat), [](coreVertexBuffer* OUTPUT pBuffer)
    {
        pBuffer->DefineAttribute(ENEMY_SHADER_ATTRIBUTE_BLINK, 1u, GL_FLOAT, false, 0u);
    });

    // add enemy set to global shadow and outline
    cShadow::GetGlobalContainer()->BindList(&oEnemyActive);
    g_aaOutline[PRIO_WEAK][STYLE_FULL].BindList(&oEnemyActive);

    // set bullet pool to initial size
    aEnemyPool.resize(ENEMY_SET_INIT_SIZE);
}


// ****************************************************************
// add enemy to the game
template <typename T> RETURN_RESTRICT T* cEnemyManager::AddEnemy()
{
    // get requested enemy set
    sEnemySet<T>* pSet;
    if(!m_apEnemySet.count(REF_ID(T::ID)))
    {
        // create new enemy set
        pSet = new sEnemySet<T>();
        m_apEnemySet[REF_ID(T::ID)] = pSet;
    }
    else pSet = s_cast<sEnemySet<T>*>(m_apEnemySet[REF_ID(T::ID)]);

    // save current pool size
    const coreUintW iSize = pSet->aEnemyPool.size();

    // loop through all enemies
    for(coreUintW i = iSize; i--; )
    {
        if(++pSet->iCurEnemy >= iSize) pSet->iCurEnemy = 0u;

        // check current enemy status
        T* pEnemy = &pSet->aEnemyPool[pSet->iCurEnemy];
        if(CONTAINS_VALUE(pEnemy->GetStatus(), ENEMY_STATUS_READY))
        {
            // add to active list
            pSet->oEnemyActive.BindObject(pEnemy);
            pEnemy->RemoveStatus(ENEMY_STATUS_READY);

            return pEnemy;
        }
    }

    // increase list and pool size by 100%
    pSet->oEnemyActive.Reallocate(iSize * 2u);
    pSet->aEnemyPool  .resize    (iSize * 2u);

    // re-add all active enemies (addresses may have changed)
    pSet->oEnemyActive.Clear();
    FOR_EACH(it, pSet->aEnemyPool)
    {
        if(!CONTAINS_VALUE(it->GetStatus(), ENEMY_STATUS_DEAD))
            pSet->oEnemyActive.BindObject(&(*it));
    }

    // execute again with first new enemy (overhead should be low, requested enemy set is cached)
    pSet->iCurEnemy = iSize - 1u;
    return this->AddEnemy<T>();
}


// ****************************************************************
// 
template <typename F> void cEnemyManager::ForEachEnemy(F&& nFunction)
{
    // 
    const auto& oEnemyList = Core::Manager::Object->GetObjectList(TYPE_ENEMY);
    FOR_EACH(it, oEnemyList)
    {
        cEnemy* pEnemy = s_cast<cEnemy*>(*it);
        if(!pEnemy) continue;

        // 
        nFunction(pEnemy);
    }
}


#endif // _P1_GUARD_ENEMY_H_