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

// TODO: adapt depth values of bullets (outlining, enemy<>player)


// ****************************************************************
// bullet definitions
#define BULLET_SET_INIT_SIZE (32u)     // initial allocation size when creating a new bullet set
#define BULLET_AREA_FACTOR   (1.2f)    // size factor for foreground area where the bullet remains active
#define BULLET_SPEED_FACTOR  (30.0f)   // 

enum eBulletStatus : coreUint8
{
    BULLET_STATUS_READY  = 0x01u,     // bullet is ready to be created
    BULLET_STATUS_ACTIVE = 0x02u      // bullet is currently flying around, doing stuff (no checking required, is managed)
};


// ****************************************************************
// bullet interface
class INTERFACE cBullet : public coreObject3D
{
protected:
    coreInt32 m_iDamage;   // damage value
    coreFloat m_fSpeed;    // 

    cShip* m_pOwner;       // associated owner of the bullet


public:
    cBullet()noexcept;
    virtual ~cBullet() {}

    FRIEND_CLASS(cBulletManager)
    ENABLE_COPY (cBullet)
    ENABLE_ID

    // move the bullet
    void Move()override;

    // control status
    void Activate  (const coreInt32& iDamage, const coreFloat& fSpeed, cShip* pOwner, const coreInt32& iType, const coreVector2& vPosition, const coreVector2& vDirection);
    void Deactivate(const coreBool&  bAnimated);

    // get object properties
    inline const coreInt32& GetDamage()const {return m_iDamage;}
    inline const coreFloat& GetSpeed ()const {return m_fSpeed;}
    inline       cShip*     GetOwner ()const {return m_pOwner;}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {ASSERT(false) return "";}
    static inline cOutline*       ConfigOutlineObject       () {ASSERT(false) return g_pOutlineFull;}


private:
    // own routines for derived classes (render functions executed by manager)
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
        coreUintW iCurBullet;         // current bullet (next one to check)

        sBulletSet()noexcept;
        ~sBulletSet();
    };


private:
    coreLookup<coreInt32, sBulletSetGen*> m_apBulletSet;   // bullet sets (each for a different inherited bullet class)


public:
    cBulletManager()noexcept;
    ~cBulletManager();

    DISABLE_COPY(cBulletManager)

    // render and move the bullet manager
    void Render();
    void Move();

    // add and remove bullets
    template <typename T> RETURN_RESTRICT T* AddBullet(const coreInt32& iDamage, const coreFloat& fSpeed, cShip* pOwner, const coreInt32& iType, const coreVector2& vPosition, const coreVector2& vDirection);
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
    ASSIGN_ID(1, "Ray")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.5f,2.5f,2.5f)); this->SetColor3(coreVector3(0.9f,0.8f,0.4f) * 0.95f);}

    // change default color (yellow)
    inline void MakeOrange() {this->SetColor3(coreVector3(1.0f,0.4f,0.0f));}
    inline void MakeRed   () {}
    inline void MakeViolet() {}
    inline void MakeBlue  () {}
    inline void MakeGreen () {}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {return "effect_energy_direct_inst_program";}
    static inline cOutline*       ConfigOutlineObject       () {return g_pOutlineDirect;}


private:
    // move the ray bullet
    void __MoveOwn()override;
};


// 2 PULSE


// ****************************************************************
// orb bullet class
class cOrbBullet final : public cBullet
{
private:
    coreFlow m_fAnimation;   // animation value


public:
    cOrbBullet()noexcept;

    ENABLE_COPY(cOrbBullet)
    ASSIGN_ID(3, "Orb")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.6f,1.6f,1.6f)); this->SetColor3(coreVector3(0.09f,0.387f,0.9f));}

    // change default color (blue)
    inline void MakeYellow() {}
    inline void MakeOrange() {this->SetColor3(coreVector3(1.0f,0.3f, 0.0f));}
    inline void MakeRed   () {this->SetColor3(coreVector3(0.9f,0.25f,0.25f) * 0.8f);}
    inline void MakeViolet() {this->SetColor3(coreVector3(0.4f,0.15f,1.0f));}
    inline void MakeGreen () {this->SetColor3(coreVector3(0.3f,0.7f, 0.3f)  * 0.8f);}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {return "effect_energy_inst_program";}
    static inline cOutline*       ConfigOutlineObject       () {return g_pOutlineFull;}


private:
    // move the orb bullet
    void __MoveOwn()override;
};


// ****************************************************************
// cone bullet class
class cConeBullet final : public cBullet
{
private:
    coreFlow m_fAnimation;   // animation value


public:
    cConeBullet()noexcept;

    ENABLE_COPY(cConeBullet)
    ASSIGN_ID(4, "Cone")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.35f,1.55f,1.35f)); this->SetColor3(coreVector3(1.0f,0.4f,0.0f));}

    // change default color (orange)
    inline void MakeYellow() {this->SetColor3(coreVector3(0.9f,0.8f,0.4f) * 0.9f);}
    inline void MakeRed   () {}
    inline void MakeViolet() {}
    inline void MakeBlue  () {}
    inline void MakeGreen () {}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {return "effect_energy_inst_program";}
    static inline cOutline*       ConfigOutlineObject       () {return g_pOutlineDirect;}


private:
    // move the cone bullet
    void __MoveOwn()override;
};


// ****************************************************************
// wave bullet class
class cWaveBullet final : public cBullet
{
private:
    coreFlow m_fAnimation;   // animation value


public:
    cWaveBullet()noexcept;

    ENABLE_COPY(cWaveBullet)
    ASSIGN_ID(5, "Wave")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(0.35f,1.55f,0.35f)); this->SetColor3(coreVector3(0.3f,0.7f,0.3f) * 1.2f);}

    // change default color (green)
    inline void MakeYellow() {}
    inline void MakeOrange() {}
    inline void MakeRed   () {}
    inline void MakeViolet() {}
    inline void MakeBlue  () {}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {return "effect_energy_direct_inst_program";}
    static inline cOutline*       ConfigOutlineObject       () {return g_pOutlineDirect;}


private:
    // move the wave bullet
    void __MoveOwn()override;
};


// ****************************************************************
// constructor
template <typename T> cBulletManager::sBulletSet<T>::sBulletSet()noexcept
: iCurBullet (0u)
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
template <typename T> RETURN_RESTRICT T* cBulletManager::AddBullet(const coreInt32& iDamage, const coreFloat& fSpeed, cShip* pOwner, const coreInt32& iType, const coreVector2& vPosition, const coreVector2& vDirection)
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
    const coreUintW iSize = pSet->aBulletPool.size();

    // loop through all bullets
    for(coreUintW i = iSize; i--; )
    {
        if(++pSet->iCurBullet >= iSize) pSet->iCurBullet = 0u;

        // check current bullet status
        T* pBullet = &pSet->aBulletPool[pSet->iCurBullet];
        if(CONTAINS_VALUE(pBullet->GetStatus(), BULLET_STATUS_READY))
        {
            // prepare bullet and add to active list
            pBullet->Activate(iDamage, fSpeed, pOwner, iType, vPosition, vDirection);
            pBullet->ResetProperties();
            pSet->oBulletActive.BindObject(pBullet);

            return pBullet;
        }
    }

    // increase list and pool size by 100%
    pSet->oBulletActive.Reallocate(iSize * 2u);
    pSet->aBulletPool  .resize    (iSize * 2u);

    // re-add all active bullets (addresses may have changed)
    pSet->oBulletActive.Clear();
    FOR_EACH(it, pSet->aBulletPool)
    {
        if(CONTAINS_VALUE(it->GetStatus(), BULLET_STATUS_ACTIVE))
            pSet->oBulletActive.BindObject(&(*it));
    }

    // execute again with first new bullet (overhead should be low, requested bullet set is cached)
    pSet->iCurBullet = iSize - 1u;
    return this->AddBullet<T>(iDamage, fSpeed, pOwner, iType, vPosition, vDirection);
}


#endif // _P1_GUARD_BULLET_H_