///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BULLET_H_
#define _P1_GUARD_BULLET_H_

// TODO: pre-allocate bullets (at least for player) at the beginning to improve resource loading
// TODO: use prefetch with more precise numbers (also in enemy-manager)
// TODO: align bullet memory ? (also check other possible locations (e.g. enemies))
// TODO: remove tons of template instantiations (also enemies ?)
// TODO: add memory pool object for bullets ? also for enemy
// TODO: make ray bullet smoother geometrically (front round)
// TODO: sort bullet classes (color, enemy<>player, normal<>special), improve array indexing and caching
// TODO: shift spear-bullet collision like ray-bullet
// TODO: bullet -> to POD-type with single parent object


// ****************************************************************
// bullet definitions
#define BULLET_SET_INIT     (16u)     // initial size when creating a new bullet set
#define BULLET_SET_COUNT    (16u)     // 
#define BULLET_SPEED_FACTOR (30.0f)   // 
#define BULLET_DEPTH_FACTOR (0.8f)    // 

#define BULLET_SHADER_ATTRIBUTE_DEPTH (CORE_SHADER_ATTRIBUTE_DIV_TEXPARAM_NUM + 1u)

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
    coreInt32 m_iDamage;             // damage value
    coreFloat m_fSpeed;              // 
    cShip*    m_pOwner;              // associated owner of the bullet

    coreFloat m_fDepth;              // 
    coreFlow  m_fAnimation;          // animation value

    coreUint8 m_iElement;            // 

    coreFlow    m_fFlyTime;          // 
    coreVector2 m_vFlyDir;           // 

    static cRotaCache s_RotaCache;   // 


public:
    cBullet()noexcept;
    virtual ~cBullet()override = default;

    FRIEND_CLASS(cBulletManager)
    ENABLE_COPY (cBullet)
    ENABLE_ID

    // move the bullet
    void Move()final;

    // control status
    void Activate  (const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32 iType);
    void Deactivate(const coreBool bAnimated, const coreVector2& vImpact);
    void Deactivate(const coreBool bAnimated);

    // 
    void Reflect(const coreObject3D* pObject);
    void Reflect(const coreVector2&  vNormal);

    // 
    inline cBullet* ChangeSize (const coreFloat fFactor) {this->SetSize (this->GetSize () * fFactor); return this;}
    inline cBullet* ChangeAlpha(const coreFloat fFactor) {this->SetAlpha(this->GetAlpha() * fFactor); return this;}

    // get object properties
    inline const coreInt32&   GetDamage ()const {return m_iDamage;}
    inline const coreFloat&   GetSpeed  ()const {return m_fSpeed;}
    inline       cShip*       GetOwner  ()const {return m_pOwner;}
    inline const coreUint8&   GetElement()const {return m_iElement;}
    inline const coreFloat&   GetFlyTime()const {return m_fFlyTime;}
    inline const coreVector2& GetFlyDir ()const {return m_vFlyDir;}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {ASSERT(false) return "";}
    static inline coreUintW       ConfigOutlineStyle        () {ASSERT(false) return OUTLINE_STYLE_FULL;}
    static inline coreBool        ConfigShadow              () {ASSERT(false) return false;}

    // 
    static void GlobalInit() {}
    static void GlobalExit() {}


protected:
    // change default color
    inline void _MakeWhite (const coreFloat fFactor) {m_iElement = ELEMENT_WHITE;  this->__SetColorRand(COLOR_ENERGY_WHITE  * fFactor);}
    inline void _MakeYellow(const coreFloat fFactor) {m_iElement = ELEMENT_YELLOW; this->__SetColorRand(COLOR_ENERGY_YELLOW * fFactor);}
    inline void _MakeOrange(const coreFloat fFactor) {m_iElement = ELEMENT_ORANGE; this->__SetColorRand(COLOR_ENERGY_ORANGE * fFactor);}
    inline void _MakeRed   (const coreFloat fFactor) {m_iElement = ELEMENT_RED;    this->__SetColorRand(COLOR_ENERGY_RED    * fFactor);}
    inline void _MakePurple(const coreFloat fFactor) {m_iElement = ELEMENT_PURPLE; this->__SetColorRand(COLOR_ENERGY_PURPLE * fFactor);}
    inline void _MakeBlue  (const coreFloat fFactor) {m_iElement = ELEMENT_BLUE;   this->__SetColorRand(COLOR_ENERGY_BLUE   * fFactor);}
    inline void _MakeCyan  (const coreFloat fFactor) {m_iElement = ELEMENT_CYAN;   this->__SetColorRand(COLOR_ENERGY_CYAN   * fFactor);}
    inline void _MakeGreen (const coreFloat fFactor) {m_iElement = ELEMENT_GREEN;  this->__SetColorRand(COLOR_ENERGY_GREEN  * fFactor);}

    // 
    void _EnableDepth(const coreProgramPtr& pProgram)const;
    void _EnableDepth()const;


private:
    // own routines for derived classes (render functions executed by manager)
    virtual void __ImpactOwn      (const coreVector2& vImpact) {}
    virtual void __RenderOwnBefore()                           {}
    virtual void __RenderOwnAfter ()                           {}
    virtual void __MoveOwn        ()                           {}

    // 
    inline void __SetColorRand(const coreVector3& vColor) {this->SetColor3(vColor * Core::Rand->Float(0.8f, 1.0f));}
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
        ~sBulletSet()final;
    };


private:
    sBulletSetGen* m_apBulletSet[BULLET_SET_COUNT];   // bullet sets (each for a different inherited bullet class)

    cOutline  m_Outline;                              // 
    coreInt32 m_iType;                                // 


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
    inline cBullet*          FindBullet     (const coreVector2& vPosition)const;
    template <typename T> T* FindBulletTyped(const coreVector2& vPosition)const;
    template <typename F> void             ForEachBullet     (F&& nFunction)const;   // [](cBullet* OUTPUT pBullet) -> void
    template <typename T, typename F> void ForEachBulletTyped(F&& nFunction)const;   // [](T*       OUTPUT pBullet) -> void

    // 
    template <typename T> void PrefetchBullet();

    // 
    inline coreUintW                       GetNumBullets     ()const {coreUintW iNum = 0u; this->ForEachBullet        ([&](void*) {++iNum;}); return iNum;}
    template <typename T> inline coreUintW GetNumBulletsTyped()const {coreUintW iNum = 0u; this->ForEachBulletTyped<T>([&](void*) {++iNum;}); return iNum;}
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
    inline void ResetProperties() {this->MakeYellow(); this->SetSize(coreVector3(3.7f,3.7f,3.7f) * 0.5f); m_fAnimation = 0.09f;}

    // change default color
    inline cRayBullet* MakeWhite () {this->_MakeWhite (0.7f); return this;}
    inline cRayBullet* MakeYellow() {this->_MakeYellow(1.0f); return this;}
    inline cRayBullet* MakeOrange() {this->_MakeOrange(1.0f); return this;}
    inline cRayBullet* MakeRed   () {this->_MakeRed   (1.0f); return this;}
    inline cRayBullet* MakePurple() {this->_MakePurple(1.0f); return this;}
    inline cRayBullet* MakeBlue  () {this->_MakeBlue  (1.0f); return this;}
    inline cRayBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cRayBullet* MakeGreen () {this->_MakeGreen (1.0f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
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
    inline void ResetProperties() {this->MakePurple(); this->SetSize(coreVector3(2.5f,2.5f,2.5f) * 1.3f); m_fAnimation = 0.09f;}

    // change default color
    inline cPulseBullet* MakeWhite () {this->_MakeWhite (0.7f); return this;}
    inline cPulseBullet* MakeYellow() {ASSERT(false)            return this;}
    inline cPulseBullet* MakeOrange() {ASSERT(false)            return this;}
    inline cPulseBullet* MakeRed   () {ASSERT(false)            return this;}
    inline cPulseBullet* MakePurple() {this->_MakePurple(1.0f); return this;}
    inline cPulseBullet* MakeBlue  () {ASSERT(false)            return this;}
    inline cPulseBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cPulseBullet* MakeGreen () {ASSERT(false)            return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __MoveOwn()final;
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
    inline void ResetProperties() {this->MakeBlue(); this->SetSize(coreVector3(1.6f,1.6f,1.6f) * 1.1f); m_fAnimation = 0.0f;}

    // change default color
    inline cOrbBullet* MakeWhite () {this->_MakeWhite (0.6f); return this;}
    inline cOrbBullet* MakeYellow() {ASSERT(false)            return this;}
    inline cOrbBullet* MakeOrange() {this->_MakeOrange(0.9f); return this;}
    inline cOrbBullet* MakeRed   () {this->_MakeRed   (0.9f); return this;}
    inline cOrbBullet* MakePurple() {this->_MakePurple(0.9f); return this;}
    inline cOrbBullet* MakeBlue  () {this->_MakeBlue  (0.9f); return this;}
    inline cOrbBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cOrbBullet* MakeGreen () {this->_MakeGreen (0.8f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
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
    inline void ResetProperties() {this->MakeOrange(); this->SetSize(coreVector3(1.35f,1.55f,1.35f) * 1.05f); m_fAnimation = 0.3f;}

    // change default color
    inline cConeBullet* MakeWhite () {this->_MakeWhite (0.6f); return this;}
    inline cConeBullet* MakeYellow() {this->_MakeYellow(0.8f); return this;}
    inline cConeBullet* MakeOrange() {this->_MakeOrange(1.0f); return this;}
    inline cConeBullet* MakeRed   () {ASSERT(false)            return this;}
    inline cConeBullet* MakePurple() {ASSERT(false)            return this;}
    inline cConeBullet* MakeBlue  () {ASSERT(false)            return this;}
    inline cConeBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cConeBullet* MakeGreen () {this->_MakeGreen (1.0f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
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
    inline void ResetProperties() {this->MakeGreen(); this->SetSize(coreVector3(1.5f,1.5f,1.5f) * 1.3f); m_fAnimation = 0.2f;}

    // change default color
    inline cWaveBullet* MakeWhite () {this->_MakeWhite (0.7f); return this;}
    inline cWaveBullet* MakeYellow() {ASSERT(false)            return this;}
    inline cWaveBullet* MakeOrange() {ASSERT(false)            return this;}
    inline cWaveBullet* MakeRed   () {this->_MakeRed   (1.0f); return this;}
    inline cWaveBullet* MakePurple() {ASSERT(false)            return this;}
    inline cWaveBullet* MakeBlue  () {ASSERT(false)            return this;}
    inline cWaveBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cWaveBullet* MakeGreen () {this->_MakeGreen (1.1f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
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
    inline void ResetProperties() {this->MakeBlue(); this->SetSize(coreVector3(2.5f,2.5f,2.5f)); m_fAnimation = 0.09f; m_fLightningTime = 1.0f;}

    // change default color
    inline cTeslaBullet* MakeWhite () {this->_MakeWhite (0.7f); return this;}
    inline cTeslaBullet* MakeYellow() {ASSERT(false)            return this;}
    inline cTeslaBullet* MakeOrange() {ASSERT(false)            return this;}
    inline cTeslaBullet* MakeRed   () {ASSERT(false)            return this;}
    inline cTeslaBullet* MakePurple() {ASSERT(false)            return this;}
    inline cTeslaBullet* MakeBlue  () {this->_MakeBlue  (0.9f); return this;}
    inline cTeslaBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cTeslaBullet* MakeGreen () {ASSERT(false)            return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_spheric_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// mine bullet class
class cMineBullet final : public cBullet
{
private:
    static coreObject3D s_Wave;   // 


public:
    cMineBullet()noexcept;

    ENABLE_COPY(cMineBullet)
    ASSIGN_ID(7, "Mine")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.0f,2.0f,2.0f)); m_fAnimation = 0.0f;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_ship_glow_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return true;}

    // 
    static void GlobalInit();
    static void GlobalExit();


private:
    // execute own routines
    void __ImpactOwn      (const coreVector2& vImpact)final;
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
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
    inline void ResetProperties() {this->SetSize(coreVector3(1.7f,1.7f,1.7f)); m_fAnimation = 0.0f;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_ship_glow_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return true;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// spear bullet class
class cSpearBullet final : public cBullet
{
public:
    cSpearBullet()noexcept;

    ENABLE_COPY(cSpearBullet)
    ASSIGN_ID(9, "Spear")

    // reset base properties
    inline void ResetProperties() {this->MakeYellow(); this->SetSize(coreVector3(1.45f,1.55f,1.45f) * 2.1f); m_fAnimation = 0.15f;}

    // change default color
    inline cSpearBullet* MakeWhite () {this->_MakeWhite (0.8f); return this;}
    inline cSpearBullet* MakeYellow() {this->_MakeYellow(1.0f); return this;}
    inline cSpearBullet* MakeOrange() {ASSERT(false)            return this;}
    inline cSpearBullet* MakeRed   () {ASSERT(false)            return this;}
    inline cSpearBullet* MakePurple() {ASSERT(false)            return this;}
    inline cSpearBullet* MakeBlue  () {ASSERT(false)            return this;}
    inline cSpearBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cSpearBullet* MakeGreen () {ASSERT(false)            return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// triangle bullet class
class cTriangleBullet final : public cBullet
{
public:
    cTriangleBullet()noexcept;

    ENABLE_COPY(cTriangleBullet)
    ASSIGN_ID(10, "Triangle")

    // reset base properties
    inline void ResetProperties() {this->MakeRed(); this->SetSize(coreVector3(1.5f,1.5f,1.5f)); m_fAnimation = 0.0f;}

    // change default color
    inline cTriangleBullet* MakeWhite () {ASSERT(false)            return this;}
    inline cTriangleBullet* MakeYellow() {ASSERT(false)            return this;}
    inline cTriangleBullet* MakeOrange() {ASSERT(false)            return this;}
    inline cTriangleBullet* MakeRed   () {this->_MakeRed   (1.0f); return this;}
    inline cTriangleBullet* MakePurple() {this->_MakePurple(1.0f); return this;}
    inline cTriangleBullet* MakeBlue  () {ASSERT(false)            return this;}
    inline cTriangleBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cTriangleBullet* MakeGreen () {ASSERT(false)            return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// flip bullet class
class cFlipBullet final : public cBullet
{
public:
    cFlipBullet()noexcept;

    ENABLE_COPY(cFlipBullet)
    ASSIGN_ID(11, "Flip")

    // reset base properties
    inline void ResetProperties() {this->MakePurple(); this->SetSize(coreVector3(2.6f,2.0f,2.6f)); m_fAnimation = 0.0f;}

    // change default color
    inline cFlipBullet* MakeWhite () {this->_MakeWhite (0.6f); return this;}
    inline cFlipBullet* MakeYellow() {ASSERT(false)            return this;}
    inline cFlipBullet* MakeOrange() {ASSERT(false)            return this;}
    inline cFlipBullet* MakeRed   () {ASSERT(false)            return this;}
    inline cFlipBullet* MakePurple() {this->_MakePurple(1.0f); return this;}
    inline cFlipBullet* MakeBlue  () {ASSERT(false)            return this;}
    inline cFlipBullet* MakeCyan  () {ASSERT(false)            return this;}
    inline cFlipBullet* MakeGreen () {ASSERT(false)            return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// quad bullet class
class cQuadBullet final : public cBullet
{
public:
    cQuadBullet()noexcept;

    ENABLE_COPY(cQuadBullet)
    ASSIGN_ID(12, "Quad")

    // reset base properties
    inline void ResetProperties() {this->MakeCyan(); this->SetSize(coreVector3(1.5f,1.5f,1.5f)); m_fAnimation = 0.0f;}

    // change default color
    inline cQuadBullet* MakeWhite () {ASSERT(false)            return this;}
    inline cQuadBullet* MakeYellow() {ASSERT(false)            return this;}
    inline cQuadBullet* MakeOrange() {ASSERT(false)            return this;}
    inline cQuadBullet* MakeRed   () {this->_MakeRed   (1.0f); return this;}
    inline cQuadBullet* MakePurple() {this->_MakePurple(1.0f); return this;}
    inline cQuadBullet* MakeBlue  () {ASSERT(false)            return this;}
    inline cQuadBullet* MakeCyan  () {this->_MakeCyan  (1.0f); return this;}
    inline cQuadBullet* MakeGreen () {ASSERT(false)            return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// 
class cChromaBullet final : public cBullet
{
public:
    cChromaBullet()noexcept;

    ENABLE_COPY(cChromaBullet)
    ASSIGN_ID(13, "Chroma")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.0f,1.0f,1.0f)); m_fAnimation = 0.0f;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2& vImpact)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// constructor
template <typename T> cBulletManager::sBulletSet<T>::sBulletSet(cOutline* pOutline)noexcept
{
    // set shader-program
    oBulletActive.DefineProgram(T::ConfigProgramInstancedName());

    // 
    oBulletActive.CreateCustom(sizeof(coreFloat), [](coreVertexBuffer* OUTPUT pBuffer)
    {
        pBuffer->DefineAttribute(BULLET_SHADER_ATTRIBUTE_DEPTH, 1u, GL_FLOAT, false, 0u);
    },
    [](coreFloat* OUTPUT pData, const cBullet* pBullet)
    {
        (*pData) = pBullet->m_fDepth;
    },
    [](const coreProgramPtr& pProgram, const cBullet* pBullet)
    {
        pBullet->_EnableDepth(pProgram);
    });

    // 
    T::GlobalInit();

    // add bullet set to global shadow and glow
    if(T::ConfigShadow()) cShadow::GetGlobalContainer()->BindList(&oBulletActive);
    g_pGlow->BindList(&oBulletActive);

    // add bullet set to outline (will be cleared entirely)
    pOutline->GetStyle(T::ConfigOutlineStyle())->BindList(&oBulletActive);

    // set bullet pool to initial size
    aBulletPool.resize(BULLET_SET_INIT);
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
    this->PrefetchBullet<T>();
    sBulletSet<T>* pSet = d_cast<sBulletSet<T>*>(m_apBulletSet[T::ID]);

    // save and check current pool size
    const coreUintW iSize = pSet->aBulletPool.size();
    if(pSet->oBulletActive.List()->size() < iSize)
    {
        // loop through all bullets
        for(coreUintW i = iSize; i--; )
        {
            if(++pSet->iCurBullet >= iSize) pSet->iCurBullet = 0u;

            // check current bullet status
            T* pBullet = &pSet->aBulletPool[pSet->iCurBullet];
            if(CONTAINS_FLAG(pBullet->GetStatus(), BULLET_STATUS_READY))
            {
                // prepare bullet and add to active list
                pBullet->Activate(iDamage, fSpeed, pOwner, vPosition, vDirection, m_iType);
                pBullet->ResetProperties();
                pSet->oBulletActive.BindObject(pBullet);

                return pBullet;
            }
        }
    }

    const coreUintW iBefore = P_TO_UI(pSet->aBulletPool.data());

    // increase list and pool size by 100%
    pSet->oBulletActive.Reallocate(iSize * 2u);
    pSet->aBulletPool  .resize    (iSize * 2u);

    const coreUintW iAfter = P_TO_UI(pSet->aBulletPool.data());

    // fix addresses for all active bullets
    FOR_EACH(it, *pSet->oBulletActive.List())
        (*it) = s_cast<coreObject3D*>(I_TO_P(P_TO_UI(*it) - iBefore + iAfter));

    // execute again with first new bullet
    pSet->iCurBullet = iSize - 1u;
    return this->AddBullet<T>(iDamage, fSpeed, pOwner, vPosition, vDirection);
}



// ****************************************************************
// 
inline cBullet* cBulletManager::FindBullet(const coreVector2& vPosition)const
{
    // 
    cBullet*  pBullet = NULL;
    coreFloat fLenSq  = FLT_MAX;

    // 
    this->ForEachBullet([&](cBullet* OUTPUT pCurBullet)
    {
        // 
        const coreFloat fCurLenSq = (pCurBullet->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq < fLenSq)
        {
            // 
            pBullet = pCurBullet;
            fLenSq  = fCurLenSq;
        }
    });

    return pBullet;
}


// ****************************************************************
// 
template <typename T> T* cBulletManager::FindBulletTyped(const coreVector2& vPosition)const
{
    // 
    T*        pBullet = NULL;
    coreFloat fLenSq  = FLT_MAX;

    // 
    this->ForEachBulletTyped<T>([&](T* OUTPUT pCurBullet)
    {
        // 
        const coreFloat fCurLenSq = (pCurBullet->GetPosition().xy() - vPosition).LengthSq();
        if(fCurLenSq < fLenSq)
        {
            // 
            pBullet = pCurBullet;
            fLenSq  = fCurLenSq;
        }
    });

    return pBullet;
}


// ****************************************************************
// 
template <typename F> void cBulletManager::ForEachBullet(F&& nFunction)const
{
    // 
    const std::vector<coreObject3D*>& oBulletList = Core::Manager::Object->GetObjectList(m_iType);
    FOR_EACH(it, oBulletList)
    {
        cBullet* pBullet = d_cast<cBullet*>(*it);
        if(!pBullet) continue;

        // 
        nFunction(pBullet);
    }
}


// ****************************************************************
// 
template <typename T, typename F> void cBulletManager::ForEachBulletTyped(F&& nFunction)const
{
    STATIC_ASSERT(T::ID < BULLET_SET_COUNT)

    if(m_apBulletSet[T::ID])
    {
        // 
        const coreBatchList& oBulletActive = m_apBulletSet[T::ID]->oBulletActive;
        FOR_EACH(it, *oBulletActive.List())
        {
            T* pBullet = d_cast<T*>(*it);
            if(!CONTAINS_FLAG(pBullet->GetStatus(), BULLET_STATUS_ACTIVE)) continue;

            // 
            nFunction(pBullet);
        }
    }
}


// ****************************************************************
// 
template <typename T> void cBulletManager::PrefetchBullet()
{
    STATIC_ASSERT(T::ID < BULLET_SET_COUNT)

    if(!m_apBulletSet[T::ID])
    {
        // create new bullet set
        m_apBulletSet[T::ID] = new sBulletSet<T>(&m_Outline);

        Core::Log->Info("Bullet Set (%s) created", T::Name);
    }
}


#endif // _P1_GUARD_BULLET_H_