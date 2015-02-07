//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BULLET_H_
#define _P1_GUARD_BULLET_H_


// ****************************************************************
// bullet definitions
#define BULLET_SET_INIT_SIZE (32u)    // initial allocation size when creating a new bullet set
#define BULLET_AREA_FACTOR   (1.5f)   // size factor for foreground area where the bullet remains active

enum eBulletStatus : coreByte
{
    BULLET_STATUS_READY  = 0x01,     // bullet is ready to be created
    BULLET_STATUS_ACTIVE = 0x02      // bullet is currently flying around, doing stuff (no checking required, is managed)
};


// ****************************************************************
// bullet interface
class INTERFACE cBullet : public coreObject3D
{
protected:
    int    m_iDamage;   // damage value
    cShip* m_pOwner;    // associated owner of the bullet


public:
    cBullet()noexcept;
    virtual ~cBullet() {}

    FRIEND_CLASS(cBulletManager)
    ENABLE_COPY (cBullet)
    ENABLE_ID

    // move the bullet
    void Move()override;

    // control status
    void Activate  (const int&  iDamage, cShip* pOwner, const int& iType, const coreVector2& vPosition, const coreVector2& vDirection);
    void Deactivate(const bool& bAnimated);

    // get object properties
    inline const int& GetDamage()const {return m_iDamage;}
    inline cShip*     GetOwner ()const {return m_pOwner;}

    // bullet configuration values
    static inline const char* ConfigProgramInstancedName() {ASSERT(false) return "";}
    static inline cOutline*   ConfigOutlineObject       () {ASSERT(false) return g_pOutlineFull;}


private:
    // render and move routines for derived classes (render functions executed by manager)
    virtual void __RenderOwnBefore() {}
    virtual void __RenderOwnAfter () {}
    virtual void __MoveOwn        () {}
};


// ****************************************************************
// bullet manager class
class cBulletManager final
{
private:
    // bullet set structure
    struct INTERFACE sBulletSetGen
    {
        coreBatchList oBulletActive;   // list with active bullets

        sBulletSetGen()noexcept;
        virtual ~sBulletSetGen() {}
    };
    template <typename T> struct sBulletSet final : public sBulletSetGen
    {
        std::vector<T> aBulletPool;   // semi-dynamic container with all bullets
        coreUint iCurBullet;          // current bullet (next one to check)

        sBulletSet()noexcept;
        ~sBulletSet();
    };


private:
    coreLookup<int, sBulletSetGen*> m_apBulletSet;   // bullet sets (each for a different inherited bullet class)


public:
    cBulletManager()noexcept;
    ~cBulletManager();

    DISABLE_COPY(cBulletManager)

    // render and move the bullet manager
    void Render();
    void Move();

    // add and remove bullets
    template <typename T> T* AddBullet(const int& iDamage, cShip* pOwner, const int& iType, const coreVector2& vPosition, const coreVector2& vDirection);
    void ClearBullets();
};


// ****************************************************************
// ray bullet class
class cRayBullet final : public cBullet
{
private:
    coreFlow m_fAnimation;   // animation value


public:
    cRayBullet()noexcept;

    ENABLE_COPY(cRayBullet)
    ASSIGN_ID(1, "RayBullet")

    // bullet configuration values
    static inline const char* ConfigProgramInstancedName() {return "effect_energy_direct_inst_program";}
    static inline cOutline*   ConfigOutlineObject       () {return g_pOutlineDirect;}


private:
    // move the ray bullet
    void __MoveOwn()override;
};


// ****************************************************************
// orb bullet class
class cOrbBullet final : public cBullet
{
private:
    coreFlow m_fAnimation;   // animation value


public:
    cOrbBullet()noexcept;

    ENABLE_COPY(cOrbBullet)
    ASSIGN_ID(2, "OrbBullet")

    // bullet configuration values
    static inline const char* ConfigProgramInstancedName() {return "effect_energy_inst_program";}
    static inline cOutline*   ConfigOutlineObject       () {return g_pOutlineFull;}


private:
    // move the orb bullet
    void __MoveOwn()override;
};


// ****************************************************************
// constructor
template <typename T> cBulletManager::sBulletSet<T>::sBulletSet()noexcept
: iCurBullet (0)
{
    // set shader-program
    oBulletActive.DefineProgram(T::ConfigProgramInstancedName());

    // add bullet to outline and glow
    T::ConfigOutlineObject()->BindList(&oBulletActive);
    g_pGlow->BindList(&oBulletActive);

    // set bullet pool to initial size
    aBulletPool.resize(BULLET_SET_INIT_SIZE);
}


// ****************************************************************
// destructor
template <typename T> cBulletManager::sBulletSet<T>::~sBulletSet()
{
    // remove bullet from outline and glow
    T::ConfigOutlineObject()->UnbindList(&oBulletActive);
    g_pGlow->UnbindList(&oBulletActive);
}


// ****************************************************************
// add bullet to the game
template <typename T> T* cBulletManager::AddBullet(const int& iDamage, cShip* pOwner, const int& iType, const coreVector2& vPosition, const coreVector2& vDirection)
{
    // get requested bullet set
    sBulletSet<T>* pSet;
    if(!m_apBulletSet.count(T::ID))
    {
        // create new bullet set
        pSet = new sBulletSet<T>();
        m_apBulletSet[T::ID] = pSet;
    }
    else pSet = s_cast<sBulletSet<T>*>(m_apBulletSet[T::ID]);

    // save current pool size
    const coreUint iSize = coreUint(pSet->aBulletPool.size());

    // loop through all bullets
    for(coreUint i = 0; i < iSize; ++i)
    {
        if(++pSet->iCurBullet >= iSize) pSet->iCurBullet = 0;

        // check current bullet status
        T* pBullet = &pSet->aBulletPool[pSet->iCurBullet];
        if(CONTAINS_VALUE(pBullet->GetStatus(), BULLET_STATUS_READY))
        {
            // prepare bullet and add to active list
            pBullet->Activate(iDamage, pOwner, iType, vPosition, vDirection);
            pSet->oBulletActive.BindObject(pBullet);

            return pBullet;
        }
    }

    // increase list and pool size by 100%
    pSet->oBulletActive.Reallocate(iSize*2);
    pSet->aBulletPool  .resize    (iSize*2);

    // re-add all active bullets (addresses may have changed)
    pSet->oBulletActive.Clear();
    FOR_EACH(it, pSet->aBulletPool)
    {
        if(CONTAINS_VALUE(it->GetStatus(), BULLET_STATUS_ACTIVE))
            pSet->oBulletActive.BindObject(&(*it));
    }

    // execute again with first new bullet (overhead should be low, requested bullet set is cached)
    pSet->iCurBullet = iSize-1;
    return this->AddBullet<T>(iDamage, pOwner, iType, vPosition, vDirection);
}


#endif // _P1_GUARD_BULLET_H_