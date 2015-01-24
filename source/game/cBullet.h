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
#define BULLET_SET_INIT_SIZE (32u)   // initial allocation size when creating a new bullet set

enum eBulletStatus : coreByte
{
    BULLET_STATUS_READY  = 0x01,     // bullet is ready to be created
    BULLET_STATUS_ACTIVE = 0x02      // bullet is currently flying around, doing stuff
};


// ****************************************************************
// bullet interface
class INTERFACE cBullet : public coreObject3D
{
public:
    cBullet()noexcept;
    virtual ~cBullet();

    ENABLE_ID

    // render and move the bullet
    void Render()override;
    void Move  ()override;

    // control status
    void Activate  (const int&  iType, const coreVector2& vPosition, const coreVector2& vDirection);
    void Deactivate(const bool& bAnimated);

    // shader-program name for bullet set
    static inline const char* GetInstancedProgramName() {ASSERT(false) return "";}
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
    template <typename T> struct sBulletSet : public sBulletSetGen
    {
        std::vector<T> aBulletPool;   // semi-dynamic container with all bullets
        coreUint iCurBullet;          // current bullet (next one to check)

        sBulletSet()noexcept;
    };


private:
    coreLookup<int, sBulletSetGen*> m_apBulletSet;   // bullet sets (each for a different inherited bullet class)


public:
    cBulletManager()noexcept;
    ~cBulletManager();

    // render and move the bullet manager
    void Render();
    void Move();

    // add and remove bullets
    template <typename T> T* AddBullet(const int& iType, const coreVector2& vPosition, const coreVector2& vDirection);
    void ClearBullets();


private:
    DISABLE_COPY(cBulletManager)
};


// ****************************************************************
// ray bullet class
class cRay final : public cBullet
{
public:
    cRay()noexcept;
    ~cRay();

    ASSIGN_ID(1, "Ray")

    // shader-program name for bullet-set
    static inline const char* GetInstancedProgramName() {return "effect_energy_inst_program";}
};


// ****************************************************************
// orb bullet class
class cOrb final : public cBullet
{
public:
    cOrb()noexcept;
    ~cOrb();

    ASSIGN_ID(2, "Orb")

    // shader-program name for bullet-set
    static inline const char* GetInstancedProgramName() {return "effect_energy_inst_program";}
};


// ****************************************************************
// constructor
template <typename T> cBulletManager::sBulletSet<T>::sBulletSet()noexcept
: iCurBullet (0)
{
    // set shader-program
    oBulletActive.DefineProgram(T::GetInstancedProgramName());

    // set bullet pool to initial size
    aBulletPool.resize(BULLET_SET_INIT_SIZE);
}


// ****************************************************************
// add bullet to the game
template <typename T> T* cBulletManager::AddBullet(const int& iType, const coreVector2& vPosition, const coreVector2& vDirection)
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
        if(pBullet->GetStatus() & BULLET_STATUS_READY)
        {
            // prepare bullet and add to active list
            pBullet->Activate(iType, vPosition, vDirection);
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
        if(it->GetStatus() & BULLET_STATUS_ACTIVE)
            pSet->oBulletActive.BindObject(&(*it));
    }

    // execute again with first new bullet (overhead should be low, requested bullet set is cached)
    pSet->iCurBullet = iSize-1;
    return this->AddBullet<T>(iType, vPosition, vDirection);
}


#endif // _P1_GUARD_BULLET_H_