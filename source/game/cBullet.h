///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BULLET_H_
#define _P1_GUARD_BULLET_H_

// TODO 3: add memory pool for bullets, instead of always reallocating
// TODO 3: remove tons of template instantiations (also enemies ? other templates ?) (CreateBullet and AllocateEnemy create tons of symbols)
// TODO 3: sort bullet classes (color, enemy<>player, normal<>special), improve array indexing and caching
// TODO 2: lots of bullets with direction-outlines can create holes in outlines by nearly-invisible backsides (can this even be fixed ?)
// TODO 3: reorder bullets either yellow->green or green->yellow, so they are overlapping consistently (in default order)
// TODO 4: surge-bullets to wave-weapon, rename one of it (probably wave-weapon to surge-weapon, code-only anyway)
// TODO 4: ID 0 is not used
// TODO 4: check if ForEachBullet or ForEachBulletTypes is faster, and adjust all use-cases accordingly (also FindBullet* and GetNumBullets*)
// TODO 4: grow bullet code could be moved out of NevoMission to a GlobalMove function
// TODO 3: implement reserve-function, called before large bullet-requests >=100 (same for enemies, for every squad?)
// TODO 3: player bullets in der luft überlagern sich komisch (transparenz von alten geschossen überdecken neue geschosse) und eigene outlines werden manchmal drüber gerendert


// ****************************************************************
// bullet definitions
#define BULLET_SET_COUNT        (20u)     // 
#define BULLET_SPEED_FACTOR     (30.0f)   // 
#define BULLET_DEPTH_FACTOR     (0.8f)    // 
#define BULLET_COLLISION_FACTOR (0.6f)    // (for enemy bullets) 

#define BULLET_SHADER_ATTRIBUTE_DEPTH     "a_v1Depth"
#define BULLET_SHADER_ATTRIBUTE_DEPTH_NUM (CORE_SHADER_ATTRIBUTE_USER_NUM + 0u)

enum eBulletStatus : coreUint8
{
    BULLET_STATUS_READY     = 0x01u,   // bullet is ready to be created
    BULLET_STATUS_ACTIVE    = 0x02u,   // bullet is currently flying around, doing stuff (no checking required, is managed)
    BULLET_STATUS_REFLECTED = 0x04u,   // 
    BULLET_STATUS_PENETRATE = 0x08u,   // TODO 1: remove    
    BULLET_STATUS_IMMORTAL  = 0x10u,   // 
    BULLET_STATUS_GHOST     = 0x20u,   // 
    BULLET_STATUS_FRESH     = 0x40u    // 
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
    coreFlow  m_fFade;               // 

    coreUint8 m_iElement;            // 

    coreFlow    m_fFlyTime;          // 
    coreVector2 m_vFlyDir;           // 

    coreFloat m_fAnimSpeed;          // 

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
    void Activate  (const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2 vPosition, const coreVector2 vDirection, const coreInt32 iType);
    void Deactivate(const coreBool bAnimated, const coreVector2 vImpact, const coreVector2 vForce);
    void Deactivate(const coreBool bAnimated, const coreVector2 vImpact);
    void Deactivate(const coreBool bAnimated);

    // 
    void Reflect(const coreObject3D* pObject, const coreVector2 vIntersection, const coreVector2 vForceNormal = coreVector2(0.0f,0.0f));

    // 
    void Ignore();

    // 
    inline cBullet* ChangeHeight           (const coreFloat   fValue)  {this->SetPosition         (coreVector3(this->GetPosition().xy(), fValue)); return this;}   // not related to tilting
    inline cBullet* ChangeSize             (const coreFloat   fFactor) {this->SetSize             (this->GetSize   () * fFactor);                  return this;}
    inline cBullet* ChangeTexSize          (const coreFloat   fFactor) {this->SetTexSize          (this->GetTexSize() * fFactor);                  return this;}
    inline cBullet* ChangeCollisionModifier(const coreVector3 vValue)  {this->SetCollisionModifier(vValue);                                        return this;}   // # unhandled/absolute change

    // 
    inline void     AddStatus   (const coreInt32 iStatus)      {ADD_FLAG       (m_iStatus, iStatus)}
    inline void     RemoveStatus(const coreInt32 iStatus)      {REMOVE_FLAG    (m_iStatus, iStatus)}
    inline coreBool HasStatus   (const coreInt32 iStatus)const {return HAS_FLAG(m_iStatus, iStatus);}

    // set object properties
    inline void SetDamage (const coreInt32   iDamage)  {m_iDamage  = iDamage;}
    inline void SetSpeed  (const coreFloat   fSpeed)   {m_fSpeed   = fSpeed * BULLET_SPEED_FACTOR;}
    inline void SetFade   (const coreFloat   fFade)    {m_fFade    = fFade;}
    inline void SetFlyTime(const coreFloat   fFlyTime) {m_fFlyTime = fFlyTime;}
    inline void SetFlyDir (const coreVector2 vFlyDir)  {m_vFlyDir  = vFlyDir;}
    
    inline void SetAnimation(const coreFloat fAnimation) {m_fAnimation = fAnimation;}
    inline void SetAnimSpeed(const coreFloat fAnimSpeed) {m_fAnimSpeed = fAnimSpeed;}

    // get object properties
    inline const coreInt32&   GetDamage ()const {return m_iDamage;}
    inline       coreFloat    GetSpeed  ()const {return m_fSpeed * (1.0f / BULLET_SPEED_FACTOR);}
    inline       cShip*       GetOwner  ()const {return m_pOwner;}
    inline const coreUint8&   GetElement()const {return m_iElement;}
    inline const coreFloat&   GetFlyTime()const {return m_fFlyTime;}
    inline const coreVector2& GetFlyDir ()const {return m_vFlyDir;}
    inline       coreVector2  GetFlyMove()const {return m_vFlyDir * (m_fSpeed * TIME);}

    // bullet configuration values
    static inline const coreChar* ConfigProgramInstancedName() {UNREACHABLE return "";}
    static inline coreUintW       ConfigOutlineStyle        () {UNREACHABLE return OUTLINE_STYLE_FULL;}
    static inline coreBool        ConfigShadow              () {UNREACHABLE return false;}
    static inline coreBool        ConfigGlow                () {UNREACHABLE return false;}
    static inline coreUintW       ConfigReserve             () {UNREACHABLE return 0u;}

    // 
    static void GlobalInit() {}
    static void GlobalExit() {}


protected:
    // change default color
    inline void _MakeWhite  (const coreFloat fFactor) {m_iElement = ELEMENT_WHITE;   this->__SetColorRand(COLOR_ENERGY_WHITE   * fFactor);}
    inline void _MakeYellow (const coreFloat fFactor) {m_iElement = ELEMENT_YELLOW;  this->__SetColorRand(COLOR_ENERGY_YELLOW  * fFactor);}
    inline void _MakeOrange (const coreFloat fFactor) {m_iElement = ELEMENT_ORANGE;  this->__SetColorRand(COLOR_ENERGY_ORANGE  * fFactor);}
    inline void _MakeRed    (const coreFloat fFactor) {m_iElement = ELEMENT_RED;     this->__SetColorRand(COLOR_ENERGY_RED     * fFactor);}
    inline void _MakeMagenta(const coreFloat fFactor) {m_iElement = ELEMENT_MAGENTA; this->__SetColorRand(COLOR_ENERGY_MAGENTA * fFactor);}
    inline void _MakePurple (const coreFloat fFactor) {m_iElement = ELEMENT_PURPLE;  this->__SetColorRand(COLOR_ENERGY_PURPLE  * fFactor);}
    inline void _MakeBlue   (const coreFloat fFactor) {m_iElement = ELEMENT_BLUE;    this->__SetColorRand(COLOR_ENERGY_BLUE    * fFactor);}
    inline void _MakeCyan   (const coreFloat fFactor) {m_iElement = ELEMENT_CYAN;    this->__SetColorRand(COLOR_ENERGY_CYAN    * fFactor);}
    inline void _MakeGreen  (const coreFloat fFactor) {m_iElement = ELEMENT_GREEN;   this->__SetColorRand(COLOR_ENERGY_GREEN   * fFactor);}

    // 
    void _EnableDepth(const coreProgramPtr& pProgram)const;
    void _EnableDepth()const;


private:
    // own routines for derived classes (render functions executed by manager)
    virtual void __ImpactOwn      (const coreVector2 vImpact, const coreVector2 vForce) {}
    virtual void __ReflectOwn     ()                                                    {}
    virtual void __RenderOwnBefore()                                                    {}
    virtual void __RenderOwnAfter ()                                                    {}
    virtual void __MoveOwn        ()                                                    {}

    // 
    inline void __SetColorRand(const coreVector3 vColor) {this->SetColor3(vColor * Core::Rand->Float(0.8f, 1.0f));}
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
        
        coreUintW iOutline;

        sBulletSetGen()noexcept;
        virtual ~sBulletSetGen() = default;
    };
    template <typename T> struct sBulletSet final : public sBulletSetGen
    {
        coreList<T> aBulletPool;   // semi-dynamic container with all bullets

        explicit sBulletSet(cOutline* pOutline)noexcept;
        ~sBulletSet()final;
    };


private:
    sBulletSetGen* m_apBulletSet[BULLET_SET_COUNT];   // bullet sets (each for a different inherited bullet class)

    cOutline  m_Outline;                              // 
    coreInt32 m_iType;                                // 

    coreUint8 m_aiOrder[BULLET_SET_COUNT];            // 


public:
    explicit cBulletManager(const coreInt32 iType)noexcept;
    ~cBulletManager();

    DISABLE_COPY(cBulletManager)

    // render and move the bullet manager
    void Render();
    void RenderAfter();
    void Move();

    // add and remove bullets
    template <typename T> RETURN_RESTRICT T* AddBullet(const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2 vPosition, const coreVector2 vDirection);
    void ClearBullets(const coreBool bAnimated);
    template <typename T> void ClearBulletsTyped(const coreBool bAnimated);

    // 
    inline cBullet*          FindBullet     (const coreVector2 vPosition)const;
    template <typename T> T* FindBulletTyped(const coreVector2 vPosition)const;
    template <typename F>             FORCE_INLINE void ForEachBullet     (F&& nFunction)const;   // [](cBullet* OUTPUT pBullet) -> void
    template <typename T, typename F> FORCE_INLINE void ForEachBulletTyped(F&& nFunction)const;   // [](T*       OUTPUT pBullet) -> void

    // 
    template <typename T> void PrefetchBullet();
    template <typename T> void ReserveBullet(const coreUintW iNumBullets);

    // 
    void OverrideOrder(const coreUint8* piNewOrder, const coreUintW iSize);
    void ResetOrder();

    // 
    inline coreUintW                       GetNumBullets        ()const {coreUintW iNum = 0u; this->ForEachBullet        ([&](void*) {++iNum;}); return iNum;}
    template <typename T> inline coreUintW GetNumBulletsTyped   ()const {coreUintW iNum = 0u; this->ForEachBulletTyped<T>([&](void*) {++iNum;}); return iNum;}
    inline coreUintW                       GetNumBulletsEst     ()const {return Core::Manager::Object->GetObjectList(m_iType).size();}   // can spike on bullet-reallocation
    template <typename T> inline coreUintW GetNumBulletsTypedEst()const {return m_apBulletSet[T::ID] ? m_apBulletSet[T::ID]->oBulletActive.GetSize() : 0u;}
};


// ****************************************************************
// ray bullet class
// TODO 1: split up ray-bullet for normal and gravity
class cRayBullet final : public cBullet
{
private:
    coreFloat m_fScale;   // 


public:
    cRayBullet()noexcept;

    ENABLE_COPY(cRayBullet)
    ASSIGN_ID(1, "Ray")

    // reset base properties
    inline void ResetProperties() {this->MakeYellow(); this->SetSize(coreVector3(0.0f,0.0f,0.0f)); this->SetTexSize(coreVector2(0.4f,0.2f) * 0.7f); m_fAnimation = 0.09f; m_fFade = 0.0f; m_fScale = 1.0f;}

    // 
    inline cRayBullet* ChangeScale(const coreFloat fScale) {m_fScale = fScale; return this;}

    // change default color
    inline cRayBullet* MakeWhite  () {this->_MakeWhite  (0.8f); return this;}
    inline cRayBullet* MakeYellow () {this->_MakeYellow (1.0f); return this;}
    inline cRayBullet* MakeOrange () {this->_MakeOrange (1.0f); return this;}
    inline cRayBullet* MakeRed    () {this->_MakeRed    (1.0f); return this;}
    inline cRayBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cRayBullet* MakePurple () {this->_MakePurple (1.0f); return this;}
    inline cRayBullet* MakeBlue   () {this->_MakeBlue   (1.0f); return this;}
    inline cRayBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cRayBullet* MakeGreen  () {this->_MakeGreen  (1.0f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 32u;}
    static constexpr coreFloat       ConfigSpeed               () {return 8.0f;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// pulse bullet class
class cPulseBullet final : public cBullet
{
private:
    coreFloat m_fScale;   // 


public:
    cPulseBullet()noexcept;

    ENABLE_COPY(cPulseBullet)
    ASSIGN_ID(2, "Pulse")

    // reset base properties
    inline void ResetProperties() {this->MakePurple(); this->SetSize(coreVector3(0.0f,0.0f,0.0f)); this->SetTexSize(coreVector2(0.1f,0.1f)); m_fAnimation = 0.0f; m_fFade = 0.0f; m_fScale = 1.0f;}

    // 
    inline cPulseBullet* ChangeScale(const coreFloat fScale) {m_fScale = fScale; return this;}

    // change default color
    inline cPulseBullet* MakeWhite  () {this->_MakeWhite  (0.7f); return this;}
    inline cPulseBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cPulseBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cPulseBullet* MakeRed    () {UNREACHABLE               return this;}
    inline cPulseBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cPulseBullet* MakePurple () {this->_MakePurple (1.0f); return this;}
    inline cPulseBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cPulseBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cPulseBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 32u;}
    static constexpr coreFloat       ConfigSpeed               () {return 6.0f;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// surge bullet class
class cSurgeBullet final : public cBullet
{
private:
    coreFloat m_fScale;   // 


public:
    cSurgeBullet()noexcept;

    ENABLE_COPY(cSurgeBullet)
    ASSIGN_ID(3, "Surge")

    // reset base properties
    inline void ResetProperties() {this->MakeGreen(); this->SetSize(coreVector3(0.0f,0.0f,0.0f)); this->SetTexSize(coreVector2(1.1f,0.25f) * 0.2f); this->SetCollisionModifier(coreVector3(0.8f,1.0f,2.0f)); m_fAnimation = 0.2f; m_fFade = 0.0f; m_fScale = 1.0f;}

    // 
    inline cSurgeBullet* ChangeScale(const coreFloat fScale) {m_fScale = fScale; return this;}

    // change default color
    inline cSurgeBullet* MakeWhite  () {this->_MakeWhite  (0.7f); return this;}
    inline cSurgeBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cSurgeBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cSurgeBullet* MakeRed    () {this->_MakeRed    (1.0f); return this;}
    inline cSurgeBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cSurgeBullet* MakePurple () {UNREACHABLE               return this;}
    inline cSurgeBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cSurgeBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cSurgeBullet* MakeGreen  () {this->_MakeGreen  (1.1f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 32u;}
    static constexpr coreFloat       ConfigSpeed               () {return 6.0f;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// tesla bullet class
class cTeslaBullet final : public cBullet
{
private:
    coreFloat m_fScale;          // 

    coreFlow m_fLightningTime;   // 
    coreBool m_bLightningSide;   // 


public:
    cTeslaBullet()noexcept;

    ENABLE_COPY(cTeslaBullet)
    ASSIGN_ID(4, "Tesla")

    // reset base properties
    inline void ResetProperties() {this->MakeBlue(); this->SetSize(coreVector3(0.0f,0.0f,0.0f)); this->SetTexSize(coreVector2(0.14f,0.14f)); m_fAnimation = 0.09f; m_fFade = 0.0f; m_fScale = 1.0f; m_fLightningTime = 1.0f;}

    // 
    inline cTeslaBullet* ChangeScale(const coreFloat fScale) {m_fScale = fScale; return this;}

    // change default color
    inline cTeslaBullet* MakeWhite  () {this->_MakeWhite  (0.7f); return this;}
    inline cTeslaBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cTeslaBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cTeslaBullet* MakeRed    () {UNREACHABLE               return this;}
    inline cTeslaBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cTeslaBullet* MakePurple () {UNREACHABLE               return this;}
    inline cTeslaBullet* MakeBlue   () {this->_MakeBlue   (0.9f); return this;}
    inline cTeslaBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cTeslaBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_spheric_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 32u;}
    static constexpr coreFloat       ConfigSpeed               () {return 4.0f;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// final bullet class
class cFinalBullet final : public cBullet
{
private:
    coreFloat   m_fScale;      // 
    coreVector3 m_vFlyDir3D;   // 


public:
    cFinalBullet()noexcept;

    ENABLE_COPY(cFinalBullet)
    ASSIGN_ID(19, "Final")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(0.0f,0.0f,0.0f)); this->SetTexSize(coreVector2(0.5f,0.5f) * 0.3f); m_fAnimation = 0.09f; m_fFade = 0.0f; m_fScale = 1.0f;}

    // 
    inline cFinalBullet* SetTiltProperties(const coreVector3 vPosition, const coreVector3 vDirection) {this->SetPosition(vPosition); m_vFlyDir3D = vDirection; return this;}

    // 
    inline cFinalBullet* ChangeScale(const coreFloat fScale) {m_fScale = fScale; return this;}

    // change default color
    inline cFinalBullet* MakeWhite() {this->_MakeWhite(0.8f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return false;}
    static constexpr coreUintW       ConfigReserve             () {return 32u;}
    static constexpr coreFloat       ConfigSpeed               () {return 16.0f;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// orb bullet class
class cOrbBullet final : public cBullet
{
public:
    cOrbBullet()noexcept;

    ENABLE_COPY(cOrbBullet)
    ASSIGN_ID(5, "Orb")

    // reset base properties
    inline void ResetProperties() {this->MakeBlue(); this->SetSize(coreVector3(1.6f,1.6f,1.6f) * 1.1f); this->SetTexSize(coreVector2(0.12f,0.12f)); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // change default color
    inline cOrbBullet* MakeWhite  () {this->_MakeWhite  (0.6f); return this;}
    inline cOrbBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cOrbBullet* MakeOrange () {this->_MakeOrange (0.9f); return this;}
    inline cOrbBullet* MakeRed    () {this->_MakeRed    (0.9f); return this;}
    inline cOrbBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cOrbBullet* MakePurple () {this->_MakePurple (0.9f); return this;}
    inline cOrbBullet* MakeBlue   () {this->_MakeBlue   (1.0f); return this;}
    inline cOrbBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cOrbBullet* MakeGreen  () {this->_MakeGreen  (0.8f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// cone bullet class
class cConeBullet final : public cBullet
{
public:
    cConeBullet()noexcept;

    ENABLE_COPY(cConeBullet)
    ASSIGN_ID(6, "Cone")

    // reset base properties
    inline void ResetProperties() {this->MakeOrange(); this->SetSize(coreVector3(1.35f,1.55f,1.35f) * 1.05f); this->SetTexSize(coreVector2(0.5f,0.2f) * 1.3f); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.3f; m_fFade = 0.0f;}

    // change default color
    inline cConeBullet* MakeWhite  () {this->_MakeWhite  (0.6f); return this;}
    inline cConeBullet* MakeYellow () {this->_MakeYellow (0.8f); return this;}
    inline cConeBullet* MakeOrange () {this->_MakeOrange (1.0f); return this;}
    inline cConeBullet* MakeRed    () {UNREACHABLE               return this;}
    inline cConeBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cConeBullet* MakePurple () {UNREACHABLE               return this;}
    inline cConeBullet* MakeBlue   () {this->_MakeBlue   (1.0f); return this;}
    inline cConeBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cConeBullet* MakeGreen  () {this->_MakeGreen  (1.0f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// wave bullet class
class cWaveBullet final : public cBullet
{
public:
    cWaveBullet()noexcept;

    ENABLE_COPY(cWaveBullet)
    ASSIGN_ID(7, "Wave")

    // reset base properties
    inline void ResetProperties() {this->MakeGreen(); this->SetSize(coreVector3(1.5f,1.5f,1.5f) * 1.3f); this->SetTexSize(coreVector2(1.1f,0.25f) * 0.2f); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.2f; m_fFade = 0.0f;}

    // change default color
    inline cWaveBullet* MakeWhite  () {this->_MakeWhite  (0.7f); return this;}
    inline cWaveBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cWaveBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cWaveBullet* MakeRed    () {this->_MakeRed    (1.0f); return this;}
    inline cWaveBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cWaveBullet* MakePurple () {UNREACHABLE               return this;}
    inline cWaveBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cWaveBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cWaveBullet* MakeGreen  () {this->_MakeGreen  (1.0f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// spear bullet class
class cSpearBullet final : public cBullet
{
public:
    cSpearBullet()noexcept;

    ENABLE_COPY(cSpearBullet)
    ASSIGN_ID(8, "Spear")

    // reset base properties
    inline void ResetProperties() {this->MakeYellow(); this->SetSize(coreVector3(1.45f,1.55f,1.45f) * 2.1f); this->SetTexSize(coreVector2(0.5f,0.2f) * 0.8f); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.15f; m_fFade = 0.0f;}

    // change default color
    inline cSpearBullet* MakeWhite  () {this->_MakeWhite  (0.8f); return this;}
    inline cSpearBullet* MakeYellow () {this->_MakeYellow (0.9f); return this;}
    inline cSpearBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cSpearBullet* MakeRed    () {UNREACHABLE               return this;}
    inline cSpearBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cSpearBullet* MakePurple () {UNREACHABLE               return this;}
    inline cSpearBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cSpearBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cSpearBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// triangle bullet class
class cTriangleBullet final : public cBullet
{
public:
    cTriangleBullet()noexcept;

    ENABLE_COPY(cTriangleBullet)
    ASSIGN_ID(9, "Triangle")

    // reset base properties
    inline void ResetProperties() {this->MakeRed(); this->SetSize(coreVector3(1.5f,1.5f,1.5f)); this->SetTexSize(coreVector2(0.5f,0.2f)); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // change default color
    inline cTriangleBullet* MakeWhite  () {this->_MakeWhite  (0.7f); return this;}
    inline cTriangleBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cTriangleBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cTriangleBullet* MakeRed    () {this->_MakeRed    (1.0f); return this;}
    inline cTriangleBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cTriangleBullet* MakePurple () {this->_MakePurple (1.0f); return this;}
    inline cTriangleBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cTriangleBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cTriangleBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// flip bullet class
class cFlipBullet final : public cBullet
{
public:
    cFlipBullet()noexcept;

    ENABLE_COPY(cFlipBullet)
    ASSIGN_ID(10, "Flip")

    // reset base properties
    inline void ResetProperties() {this->MakePurple(); this->SetSize(coreVector3(2.6f,2.0f,2.6f)); this->SetTexSize(coreVector2(0.4f,0.2f)); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // change default color
    inline cFlipBullet* MakeWhite  () {this->_MakeWhite  (0.6f); return this;}
    inline cFlipBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cFlipBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cFlipBullet* MakeRed    () {UNREACHABLE               return this;}
    inline cFlipBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cFlipBullet* MakePurple () {this->_MakePurple (1.0f); return this;}
    inline cFlipBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cFlipBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cFlipBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// quad bullet class
class cQuadBullet final : public cBullet
{
public:
    cQuadBullet()noexcept;

    ENABLE_COPY(cQuadBullet)
    ASSIGN_ID(11, "Quad")

    // reset base properties
    inline void ResetProperties() {this->MakeCyan(); this->SetSize(coreVector3(1.5f,1.5f,1.5f)); this->SetTexSize(coreVector2(0.5f,0.2f)); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // change default color
    inline cQuadBullet* MakeWhite  () {this->_MakeWhite  (0.7f); return this;}
    inline cQuadBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cQuadBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cQuadBullet* MakeRed    () {this->_MakeRed    (1.0f); return this;}
    inline cQuadBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cQuadBullet* MakePurple () {this->_MakePurple (1.0f); return this;}
    inline cQuadBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cQuadBullet* MakeCyan   () {this->_MakeCyan   (0.9f); return this;}
    inline cQuadBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// view bullet class
class cViewBullet final : public cBullet
{
public:
    cViewBullet()noexcept;

    ENABLE_COPY(cViewBullet)
    ASSIGN_ID(12, "View")

    // reset base properties
    inline void ResetProperties() {this->MakeMagenta(); this->SetSize(coreVector3(1.0f,3.0f,1.0f)); this->SetTexSize(coreVector2(0.2f,0.2f)); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * BULLET_COLLISION_FACTOR); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // change default color
    inline cViewBullet* MakeWhite  () {this->_MakeWhite  (0.7f); return this;}
    inline cViewBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cViewBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cViewBullet* MakeRed    () {UNREACHABLE               return this;}
    inline cViewBullet* MakeMagenta() {this->_MakeMagenta(1.0f); return this;}
    inline cViewBullet* MakePurple () {UNREACHABLE               return this;}
    inline cViewBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cViewBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cViewBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_direct_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_DIRECT;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// card bullet class
class cCardBullet final : public cBullet
{
public:
    cCardBullet()noexcept;

    ENABLE_COPY(cCardBullet)
    ASSIGN_ID(13, "Card")

    // reset base properties
    inline void ResetProperties() {this->MakeWhite(); this->SetSize(coreVector3(1.5f,1.5f,1.5f)); this->SetTexSize(coreVector2(0.5f,0.2f)); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // change default color
    inline cCardBullet* MakeWhite  () {this->_MakeWhite  (0.5f); return this;}
    inline cCardBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cCardBullet* MakeOrange () {UNREACHABLE               return this;}
    inline cCardBullet* MakeRed    () {this->_MakeRed    (1.0f); return this;}
    inline cCardBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cCardBullet* MakePurple () {this->_MakePurple (1.0f); return this;}
    inline cCardBullet* MakeBlue   () {UNREACHABLE               return this;}
    inline cCardBullet* MakeCyan   () {this->_MakeCyan   (0.9f); return this;}
    inline cCardBullet* MakeGreen  () {this->_MakeGreen  (0.8f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_invert_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 8u;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// debris bullet class
class cDebrisBullet final : public cBullet
{
private:
    static coreObject3D s_Wave;   // 


public:
    cDebrisBullet()noexcept;

    ENABLE_COPY(cDebrisBullet)
    ASSIGN_ID(14, "Debris")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.4f,2.4f,2.4f)); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_meteor_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return true;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 8u;}

    // 
    static void GlobalInit();
    static void GlobalExit();


private:
    // execute own routines
    void __ImpactOwn      (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
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
    ASSIGN_ID(15, "Mine")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(2.4f,2.4f,2.4f)); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_ship_glow_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return true;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 16u;}

    // 
    static void GlobalInit();
    static void GlobalExit();


private:
    // execute own routines
    void __ImpactOwn      (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __RenderOwnBefore()final;
    void __MoveOwn        ()final;
};


// ****************************************************************
// rocket bullet class
class cRocketBullet final : public cBullet
{
private:
    const cShip* m_pTarget;   // 


public:
    cRocketBullet()noexcept;

    ENABLE_COPY(cRocketBullet)
    ASSIGN_ID(16, "Rocket")

    // reset base properties
    inline void ResetProperties() {this->SetSize(coreVector3(1.8f,1.8f,1.8f)); m_fAnimation = 0.0f; m_fFade = 0.0f; m_pTarget = NULL;}

    // 
    inline cRocketBullet* SetTarget(const cShip* pTarget) {m_pTarget = pTarget; return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "object_ship_glow_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_FULL;}
    static constexpr coreBool        ConfigShadow              () {return true;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 16u;}


private:
    // execute own routines
    void __ImpactOwn (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __ReflectOwn()final;
    void __MoveOwn   ()final;
};


// ****************************************************************
// grow bullet class
class cGrowBullet final : public cBullet
{
public:
    cGrowBullet()noexcept;

    ENABLE_COPY(cGrowBullet)
    ASSIGN_ID(17, "Grow")

    // reset base properties
    inline void ResetProperties() {this->MakeBlue(); this->SetSize(coreVector3(1.6f,1.6f,1.6f) * 1.1f); this->SetTexSize(coreVector2(0.12f,0.12f) * 3.0f); this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 0.95f); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // change default color
    inline cGrowBullet* MakeWhite  () {UNREACHABLE               return this;}
    inline cGrowBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cGrowBullet* MakeOrange () {this->_MakeOrange (0.9f); return this;}
    inline cGrowBullet* MakeRed    () {this->_MakeRed    (0.9f); return this;}
    inline cGrowBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cGrowBullet* MakePurple () {UNREACHABLE               return this;}
    inline cGrowBullet* MakeBlue   () {this->_MakeBlue   (1.0f); return this;}
    inline cGrowBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cGrowBullet* MakeGreen  () {UNREACHABLE               return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_spheric_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_BULLET_FULL;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return true;}
    static constexpr coreUintW       ConfigReserve             () {return 64u;}


private:
    // execute own routines
    void __ImpactOwn(const coreVector2 vImpact, const coreVector2 vForce)final;
    void __MoveOwn  ()final;
};


// ****************************************************************
// tilt bullet class
class cTiltBullet final : public cBullet
{
private:
    coreVector3 m_vFlyDir3D;   // 


public:
    cTiltBullet()noexcept;

    ENABLE_COPY(cTiltBullet)
    ASSIGN_ID(18, "Tilt")

    // reset base properties
    inline void ResetProperties() {this->MakeWhite(); this->SetSize(coreVector3(1.6f,1.6f,1.6f) * 1.1f); this->SetTexSize(coreVector2(0.12f,0.12f)); m_fAnimation = 0.0f; m_fFade = 0.0f;}

    // 
    inline cTiltBullet* SetTiltProperties(const coreVector3 vPosition, const coreVector3 vDirection) {this->SetPosition(vPosition); m_vFlyDir3D = vDirection; return this;}

    // 
    inline void SetFlyDir3D(const coreVector3 vFlyDir3D) {m_vFlyDir3D = vFlyDir3D;}

    // 
    inline const coreVector3& GetFlyDir3D()const {return m_vFlyDir3D;}

    // change default color
    inline cTiltBullet* MakeWhite  () {this->_MakeWhite  (0.1f); return this;}
    inline cTiltBullet* MakeYellow () {UNREACHABLE               return this;}
    inline cTiltBullet* MakeOrange () {this->_MakeOrange (0.9f); return this;}
    inline cTiltBullet* MakeRed    () {this->_MakeRed    (0.9f); return this;}
    inline cTiltBullet* MakeMagenta() {UNREACHABLE               return this;}
    inline cTiltBullet* MakePurple () {this->_MakePurple (0.9f); return this;}
    inline cTiltBullet* MakeBlue   () {this->_MakeBlue   (1.0f); return this;}
    inline cTiltBullet* MakeCyan   () {UNREACHABLE               return this;}
    inline cTiltBullet* MakeGreen  () {this->_MakeGreen  (0.8f); return this;}

    // bullet configuration values
    static constexpr const coreChar* ConfigProgramInstancedName() {return "effect_energy_bullet_inst_program";}
    static constexpr coreUintW       ConfigOutlineStyle        () {return OUTLINE_STYLE_LIGHT_BULLET_THICK;}
    static constexpr coreBool        ConfigShadow              () {return false;}
    static constexpr coreBool        ConfigGlow                () {return false;}
    static constexpr coreUintW       ConfigReserve             () {return 256u;}


private:
    // execute own routines
    void __ImpactOwn     (const coreVector2 vImpact, const coreVector2 vForce)final;
    void __RenderOwnAfter()final;
    void __MoveOwn       ()final;
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
        pBuffer->DefineAttribute(BULLET_SHADER_ATTRIBUTE_DEPTH_NUM, 1u, GL_FLOAT, sizeof(coreFloat), false, 0u, 0u);
    },
    [](coreByte* OUTPUT pData, const coreObject3D* pBullet)
    {
        (*r_cast<coreFloat*>(pData)) = d_cast<const cBullet*>(pBullet)->m_fDepth;
    },
    [](const coreProgramPtr& pProgram, const coreObject3D* pBullet)
    {
        d_cast<const cBullet*>(pBullet)->_EnableDepth(pProgram);
    });

    // 
    T::GlobalInit();

    // add bullet set to global shadow and glow
    if(T::ConfigShadow()) cShadow::GetGlobalContainer()->BindList(&oBulletActive);
    if(T::ConfigGlow  ()) g_pGlow->BindList(&oBulletActive);

    // add bullet set to outline (will be cleared entirely)
    pOutline->GetStyle(T::ConfigOutlineStyle())->BindList(&oBulletActive);

    // set bullet pool to initial size       
    oBulletActive.Reallocate(T::ConfigReserve());
    aBulletPool  .resize    (T::ConfigReserve());
    
    iOutline = T::ConfigOutlineStyle();
}


// ****************************************************************
// destructor
template <typename T> cBulletManager::sBulletSet<T>::sBulletSet::~sBulletSet()
{
    // 
    T::GlobalExit();

    // remove bullet set from global shadow and glow
    if(T::ConfigShadow()) cShadow::GetGlobalContainer()->UnbindList(&oBulletActive);
    if(T::ConfigGlow  ()) g_pGlow->UnbindList(&oBulletActive);

    // clear memory
    aBulletPool.clear();
}


// ****************************************************************
// add bullet to the game
template <typename T> RETURN_RESTRICT T* cBulletManager::AddBullet(const coreInt32 iDamage, const coreFloat fSpeed, cShip* pOwner, const coreVector2 vPosition, const coreVector2 vDirection)
{
    // get requested bullet set
    this->PrefetchBullet<T>();
    sBulletSet<T>* pSet = d_cast<sBulletSet<T>*>(m_apBulletSet[T::ID]);

    // save current pool size
    const coreUintW iSize = pSet->aBulletPool.size();
    ASSERT(iSize)

    // 
    if(pSet->oBulletActive.GetSize() < iSize)
    {
        // loop through all bullets
        for(coreUintW i = iSize; i--; )
        {
            if(++pSet->iCurBullet >= iSize) pSet->iCurBullet = 0u;

            // check current bullet status
            T* pBullet = &pSet->aBulletPool[pSet->iCurBullet];
            if(pBullet->HasStatus(BULLET_STATUS_READY))
            {
                // prepare bullet and add to active list
                pBullet->Activate(iDamage, fSpeed, pOwner, vPosition, vDirection, m_iType);
                pBullet->ResetProperties();
                pSet->oBulletActive.BindObject(pBullet);
                
                if(T::ID == cTiltBullet::ID)
                {
                    coreSet<coreObject3D*>* pList = pSet->oBulletActive.List();
                    pList->pop_back();
                    pList->push_front(pBullet);
                }

                return pBullet;
            }
        }
    }

    const coreUintW iBefore = P_TO_UI(pSet->aBulletPool.data());

    // 
    FOR_EACH(it, *pSet->oBulletActive.List())
    {
        (*it)->ChangeType(0);
    }

    // increase list and pool size by 100%
    pSet->oBulletActive.Reallocate(iSize * 2u);
    pSet->aBulletPool  .resize    (iSize * 2u);

    const coreUintW iAfter = P_TO_UI(pSet->aBulletPool.data());

    // fix addresses for all active bullets
    FOR_EACH(it, *pSet->oBulletActive.List())
    {
        (*it) = s_cast<coreObject3D*>(I_TO_P(P_TO_UI(*it) - iBefore + iAfter));

        if(HAS_FLAG((*it)->GetStatus(), BULLET_STATUS_ACTIVE)) (*it)->ChangeType(m_iType);
    }

    // execute again with first new bullet
    pSet->iCurBullet = iSize - 1u;
    return this->AddBullet<T>(iDamage, fSpeed, pOwner, vPosition, vDirection);
}


// ****************************************************************
// 
template <typename T> void cBulletManager::ClearBulletsTyped(const coreBool bAnimated)
{
    STATIC_ASSERT(T::ID < BULLET_SET_COUNT)

    if(m_apBulletSet[T::ID])
    {
        // 
        const coreBatchList& oBulletActive = m_apBulletSet[T::ID]->oBulletActive;

        // deactivate all active bullets
        FOR_EACH(it, *oBulletActive.List())
        {
            cBullet* pBullet = d_cast<cBullet*>(*it);

            const coreVector2 vImpact = pBullet->GetPosition().xy();
            const coreVector2 vForce  = pBullet->GetPosition().xy().Normalized();

            pBullet->Deactivate(bAnimated, vImpact, vForce);
        }
    }
}


// ****************************************************************
// 
inline cBullet* cBulletManager::FindBullet(const coreVector2 vPosition)const
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
template <typename T> T* cBulletManager::FindBulletTyped(const coreVector2 vPosition)const
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
template <typename F> FORCE_INLINE void cBulletManager::ForEachBullet(F&& nFunction)const
{
    // 
    const coreList<coreObject3D*>& oBulletList = Core::Manager::Object->GetObjectList(m_iType);   // TODO 1: is shared between normal and top player
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
template <typename T, typename F> FORCE_INLINE void cBulletManager::ForEachBulletTyped(F&& nFunction)const
{
    STATIC_ASSERT(T::ID < BULLET_SET_COUNT)

    if(m_apBulletSet[T::ID])
    {
        // 
        const coreBatchList& oBulletActive = m_apBulletSet[T::ID]->oBulletActive;
        FOR_EACH(it, *oBulletActive.List())
        {
            T* pBullet = d_cast<T*>(*it);
            if(!pBullet->HasStatus(BULLET_STATUS_ACTIVE)) continue;

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


// ****************************************************************
// 
template <typename T> void cBulletManager::ReserveBullet(const coreUintW iNumBullets)
{
    /*
    // get requested bullet set
    this->PrefetchBullet<T>();
    sBulletSet<T>* pSet = d_cast<sBulletSet<T>*>(m_apBulletSet[T::ID]);

    if(pSet->apEnemyPool.size() < iNumBullets) nicht richtig für bullets
    {
        const coreUintW iNumPot = coreMath::CeilPot(iNumBullets);

        const coreUintW iBefore = P_TO_UI(pSet->aBulletPool.data());

        // increase list and pool size by 100%
        pSet->oBulletActive.Reallocate(iNumPot);
        pSet->aBulletPool  .resize    (iNumPot);

        const coreUintW iAfter = P_TO_UI(pSet->aBulletPool.data());

        // fix addresses for all active bullets
        FOR_EACH(it, *pSet->oBulletActive.List())
        {
            (*it) = s_cast<coreObject3D*>(I_TO_P(P_TO_UI(*it) - iBefore + iAfter));
        }
    }
     */
}


#endif // _P1_GUARD_BULLET_H_