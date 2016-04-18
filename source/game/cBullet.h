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

// TODO: pre-allocate bullets (at least for player) at the beginning to improve resource loading
// TODO: check for overlapping bullets and place one of them on another depth plane


// ****************************************************************
// bullet definitions
#define BULLET_SET_INIT_SIZE (16u)     // initial allocation size when creating a new bullet set
#define BULLET_AREA_FACTOR   (1.2f)    // size factor for foreground area where the bullet remains active
#define BULLET_SPEED_FACTOR  (30.0f)   // 

enum eBulletStatus : coreUint8
{
    BULLET_STATUS_READY  = 0x01u,      // bullet is ready to be created
    BULLET_STATUS_ACTIVE = 0x02u       // bullet is currently flying around, doing stuff (no checking required, is managed)
};


// ****************************************************************
// bullet interface
class INTERFACE cBullet : public coreObject3D
{
protected:
    coreInt32 m_iDamage;      // damage value
    coreFloat m_fSpeed;       // 
    cShip*    m_pOwner;       // associated owner of the bullet

    coreUint8 m_iElement;     // 
    coreFlow  m_fAnimation;   // animation value


public:
    cBullet()noexcept;
    virtual ~cBullet()override = default;

    FRIEND_CLASS(cBulletManager)
    ENABLE_COPY (cBullet)
    ENABLE_ID

    // move the bullet
    void Move()override;

    // control status
    void Activate  (const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32 iType);
    void Deactivate(const coreBool  bAnimated);

    // 
    inline cBullet* MakeSmaller(const coreFloat    fFactor) {this->SetSize  (this->GetSize  () * fFactor); return this;}
    inline cBullet* MakeSmaller(const coreVector3& vFactor) {this->SetSize  (this->GetSize  () * vFactor); return this;}
    inline cBullet* MakeDarker (const coreFloat    fFactor) {this->SetColor3(this->GetColor3() * fFactor); return this;}
    inline cBullet* MakeLighter(const coreFloat    fFactor) {this->SetAlpha (this->GetAlpha () * fFactor); return this;}

    // get object properties
    inline const coreInt32& GetDamage ()const {return m_iDamage;}
    inline const coreFloat& GetSpeed  ()const {return m_fSpeed;}
    inline       cShip*     GetOwner  ()const {return m_pOwner;}
    inline const coreUint8& GetElement()const {return m_iElement;}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {ASSERT(false) return "";}
    static inline coreUintW       ConfigOutlineStyle        () {ASSERT(false) return OUTLINE_STYLE_FULL;}
    static inline coreBool        ConfigShadow              () {ASSERT(false) return false;}

    // 
    static void GlobalInit() {}
    static void GlobalExit() {}


protected:
    // change default color
    inline void _MakeYellow(const coreFloat fFactor) {m_iElement = ELEMENT_YELLOW; this->SetColor3(COLOR_ENERGY_YELLOW * fFactor);}
    inline void _MakeOrange(const coreFloat fFactor) {m_iElement = ELEMENT_ORANGE; this->SetColor3(COLOR_ENERGY_ORANGE * fFactor);}
    inline void _MakeRed   (const coreFloat fFactor) {m_iElement = ELEMENT_RED;    this->SetColor3(COLOR_ENERGY_RED    * fFactor);}
    inline void _MakePurple(const coreFloat fFactor) {m_iElement = ELEMENT_PURPLE; this->SetColor3(COLOR_ENERGY_PURPLE * fFactor);}
    inline void _MakeBlue  (const coreFloat fFactor) {m_iElement = ELEMENT_BLUE;   this->SetColor3(COLOR_ENERGY_BLUE   * fFactor);}
    inline void _MakeGreen (const coreFloat fFactor) {m_iElement = ELEMENT_GREEN;  this->SetColor3(COLOR_ENERGY_GREEN  * fFactor);}


private:
    // own routines for derived classes (render functions executed by manager)
    virtual void __ImpactOwn      () {}
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
        coreUintW     iCurBullet;      // current bullet (next one to check in pool)

        sBulletSetGen()noexcept;
        virtual ~sBulletSetGen() = default;
    };
    template <typename T> struct sBulletSet final : public sBulletSetGen
    {
        std::vector<T> aBulletPool;   // semi-dynamic container with all bullets

        explicit sBulletSet(cOutline* pOutline)noexcept;
        ~sBulletSet()override;
    };


private:
    coreLookup<coreInt32, sBulletSetGen*> m_apBulletSet;   // bullet sets (each for a different inherited bullet class)

    cOutline  m_Outline;                                   // 
    coreInt32 m_iType;                                     // 


public:
    explicit cBulletManager(const coreInt32 iType)noexcept;
    ~cBulletManager();

    DISABLE_COPY(cBulletManager)

    // render and move the bullet manager
    void Render();
    void Move();

    // add and remove bullets
    template <typename T> RETURN_RESTRICT T* AddBullet(const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2& vPosition, const coreVector2& vDirection);
    void ClearBullets(const coreBool bAnimated);

    // 
    cBullet* FindBullet(const coreVector2& vPosition);
    template <typename F> void ForEachBullet(F&& nFunction);   // [](cBullet* OUTPUT pBullet) -> void

    // 
    template <typename T> void PrefetchBullet();
};


// ****************************************************************
// ray bullet class
class cRayBullet final : public cBullet
{
public:
    cRayBullet()noexcept;

    ENABLE_COPY(cRayBullet)
    ASSIGN_ID(1, "Ray")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.5f,2.5f,2.5f));}

    // change default color
    inline cRayBullet* MakeYellow() {this->_MakeYellow(0.9f); return this;}
    inline cRayBullet* MakeOrange() {this->_MakeOrange(1.0f); return this;}
    inline cRayBullet* MakeRed   () {return this;}
    inline cRayBullet* MakePurple() {this->_MakePurple(1.0f); return this;}
    inline cRayBullet* MakeBlue  () {return this;}
    inline cRayBullet* MakeGreen () {return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __MoveOwn()override;
};


// ****************************************************************
// pulse bullet class
class cPulseBullet final : public cBullet
{
public:
    cPulseBullet()noexcept;

    ENABLE_COPY(cPulseBullet)
    ASSIGN_ID(2, "Pulse")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.5f,2.5f,2.5f) * 1.3f);}

    // change default color
    inline cPulseBullet* MakeYellow() {return this;}
    inline cPulseBullet* MakeOrange() {return this;}
    inline cPulseBullet* MakeRed   () {return this;}
    inline cPulseBullet* MakePurple() {this->_MakePurple(1.0f); return this;}
    inline cPulseBullet* MakeBlue  () {return this;}
    inline cPulseBullet* MakeGreen () {return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __MoveOwn()override;
};


// ****************************************************************
// orb bullet class
class cOrbBullet final : public cBullet
{
public:
    cOrbBullet()noexcept;

    ENABLE_COPY(cOrbBullet)
    ASSIGN_ID(3, "Orb")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.6f,1.6f,1.6f));}

    // change default color
    inline cOrbBullet* MakeYellow() {return this;}
    inline cOrbBullet* MakeOrange() {this->_MakeOrange(0.9f); return this;}
    inline cOrbBullet* MakeRed   () {this->_MakeRed   (0.9f); return this;}
    inline cOrbBullet* MakePurple() {this->_MakePurple(0.9f); return this;}
    inline cOrbBullet* MakeBlue  () {this->_MakeBlue  (0.9f); return this;}
    inline cOrbBullet* MakeGreen () {this->_MakeGreen (0.8f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn()override;
    void __MoveOwn  ()override;
};


// ****************************************************************
// cone bullet class
class cConeBullet final : public cBullet
{
public:
    cConeBullet()noexcept;

    ENABLE_COPY(cConeBullet)
    ASSIGN_ID(4, "Cone")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.35f,1.55f,1.35f) * 1.05f);}

    // change default color
    inline cConeBullet* MakeYellow() {this->_MakeYellow(0.8f); return this;}
    inline cConeBullet* MakeOrange() {this->_MakeOrange(1.0f); return this;}
    inline cConeBullet* MakeRed   () {return this;}
    inline cConeBullet* MakePurple() {return this;}
    inline cConeBullet* MakeBlue  () {return this;}
    inline cConeBullet* MakeGreen () {this->_MakeGreen (1.0f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn()override;
    void __MoveOwn  ()override;
};


// ****************************************************************
// wave bullet class
class cWaveBullet final : public cBullet
{
public:
    cWaveBullet()noexcept;

    ENABLE_COPY(cWaveBullet)
    ASSIGN_ID(5, "Wave")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.5f,1.5f,1.5f) * 1.25f);}

    // change default color
    inline cWaveBullet* MakeYellow() {return this;}
    inline cWaveBullet* MakeOrange() {return this;}
    inline cWaveBullet* MakeRed   () {return this;}
    inline cWaveBullet* MakePurple() {return this;}
    inline cWaveBullet* MakeBlue  () {return this;}
    inline cWaveBullet* MakeGreen () {this->_MakeGreen(1.1f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn()override;
    void __MoveOwn  ()override;
};


// ****************************************************************
// tesla bullet class
class cTeslaBullet final : public cBullet
{
private:
    coreFlow m_fLightningTime;   // 
    coreBool m_bLightningSide;   // 


public:
    cTeslaBullet()noexcept;

    ENABLE_COPY(cTeslaBullet)
    ASSIGN_ID(6, "Tesla")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.5f,2.5f,2.5f)); m_fLightningTime = 1.0f;}

    // change default color
    inline cTeslaBullet* MakeYellow() {return this;}
    inline cTeslaBullet* MakeOrange() {return this;}
    inline cTeslaBullet* MakeRed   () {return this;}
    inline cTeslaBullet* MakePurple() {return this;}
    inline cTeslaBullet* MakeBlue  () {this->_MakeBlue(0.9f); return this;}
    inline cTeslaBullet* MakeGreen () {return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_spheric_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __MoveOwn()override;
};


// ****************************************************************
// mine bullet class
class cMineBullet final : public cBullet
{
private:
    coreVector2 m_vFlyDir;        // 

    static coreObject3D s_Wave;   // 


public:
    cMineBullet()noexcept;

    ENABLE_COPY(cMineBullet)
    ASSIGN_ID(7, "Mine")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.0f,2.0f,2.0f)); m_vFlyDir = coreVector2(0.0f,0.0f);}

    // 
    inline cMineBullet* SetFlyDir(const coreVector2& vFlyDir) {ASSERT(vFlyDir.IsNormalized()) m_vFlyDir = vFlyDir; return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_ship_glow_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return true;}

    // 
    static void GlobalInit();
    static void GlobalExit();


private:
    // execute own routines
    void __ImpactOwn      ()override;
    void __RenderOwnBefore()override;
    void __MoveOwn        ()override;
};


// ****************************************************************
// rocket bullet class
class cRocketBullet final : public cBullet
{
public:
    cRocketBullet()noexcept;

    ENABLE_COPY(cRocketBullet)
    ASSIGN_ID(8, "Rocket")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.7f,1.7f,1.7f));}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_ship_glow_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return true;}


private:
    // execute own routines
    void __ImpactOwn()override;
    void __MoveOwn  ()override;
};


// ****************************************************************
// constructor
template <typename T> cBulletManager::sBulletSet<T>::sBulletSet(cOutline* pOutline)noexcept
{
    // set shader-program
    oBulletActive.DefineProgram(T::ConfigProgramInstancedName());

    // 
    T::GlobalInit();

    // add bullet set to global shadow, outline and glow
    if(T::ConfigShadow()) cShadow::GetGlobalContainer()->BindList(&oBulletActive);
    pOutline->GetStyle(T::ConfigOutlineStyle())->BindList(&oBulletActive);
    g_pGlow->BindList(&oBulletActive);

    // set bullet pool to initial size
    aBulletPool.resize(BULLET_SET_INIT_SIZE);
}


// ****************************************************************
// destructor
template <typename T> cBulletManager::sBulletSet<T>::~sBulletSet()
{
    // 
    T::GlobalExit();

    // remove bullet set from global shadow and glow
    if(T::ConfigShadow()) cShadow::GetGlobalContainer()->UnbindList(&oBulletActive);
    g_pGlow->UnbindList(&oBulletActive);

    // clear memory
    aBulletPool.clear();
}


// ****************************************************************
// add bullet to the game
template <typename T> RETURN_RESTRICT T* cBulletManager::AddBullet(const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2& vPosition, const coreVector2& vDirection)
{
    // get requested bullet set
    sBulletSet<T>* pSet;
    if(!m_apBulletSet.count(REF_ID(T::ID)))
    {
        // create new bullet set
        pSet = new sBulletSet<T>(&m_Outline);
        m_apBulletSet.emplace(REF_ID(T::ID), pSet);
    }
    else pSet = s_cast<sBulletSet<T>*>(m_apBulletSet.at(REF_ID(T::ID)));

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
            pBullet->Activate(iDamage, fSpeed, pOwner, vPosition, vDirection, m_iType);
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
    return this->AddBullet<T>(iDamage, fSpeed, pOwner, vPosition, vDirection);
}


// ****************************************************************
// 
template <typename F> void cBulletManager::ForEachBullet(F&& nFunction)
{
    // 
    const auto& oBulletList = Core::Manager::Object->GetObjectList(m_iType);
    FOR_EACH(it, oBulletList)
    {
        cBullet* pBullet = s_cast<cBullet*>(*it);
        if(!pBullet) continue;

        // 
        nFunction(pBullet);
    }
}


// ****************************************************************
// 
template <typename T> void cBulletManager::PrefetchBullet()
{
    if(!m_apBulletSet.count(REF_ID(T::ID)))
    {
        // 
        m_apBulletSet.emplace(REF_ID(T::ID), new sBulletSet<T>(&m_Outline));
    }
}


#endif // _P1_GUARD_BULLET_H_