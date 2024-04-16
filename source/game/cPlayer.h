///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_PLAYER_H_
#define _P1_GUARD_PLAYER_H_

// TODO 3: add all weapons to player directly in class
// TODO 3: all parts of player-rendering should be batched for multiplayer
// TODO 3: render wind, bubble, etc. in group for multiplayer
// TODO 1: check which operations have to be done outside of dead-check
// TODO 3: add in-game hint for roll-cooldown end ((just) acoustic)
// TODO 3: when applying force with (all) objects (collision with container) always quantize 4 or 8, but not in general (wind)
// TODO 3: add more delay to bubble/feeling (to stay longer invincible after bubble disappeared)
// TODO 4: PLAYER_FEEL_TIME_SHIELD still used ?
// TODO 3: ein großer teil aller sub-objekte wird ständig bei glow ge-added und removed, sollte permanent sein (resurrect<>kill), und mit setenabled gesteuert werden, auch bei tracker und P1
// TODO 3: m_vOldDir sollte bei resurrection/repair zurückgesetzt werden
// TODO 3: (gyro aktualisiert sich nicht bei HUD rotation (auch bei game rotation ?))


// ****************************************************************
// player definitions
#define PLAYER_EQUIP_WEAPONS       (EQUIP_WEAPONS)      // number of weapons a player can carry
#define PLAYER_EQUIP_SUPPORTS      (EQUIP_SUPPORTS)     // 
#define PLAYER_LIVES               (LIVES)              // 
#define PLAYER_COLLISION_MIN       (0.2f)               // 
#define PLAYER_SIZE_FACTOR         (1.15f)              // 
#define PLAYER_SIZE_FACTOR_EXT     (1.15f * m_fScale)   // 
#define PLAYER_AREA_FACTOR         (1.06f)              // 
#define PLAYER_RANGE_SIZE          (1.04f)              // 
#define PLAYER_WIND_SIZE           (4.5f)               // 
#define PLAYER_BUBBLE_SIZE         (4.8f)               // 
#define PLAYER_ROLL_SPEED          (0.4f)               // 
#define PLAYER_ROLL_COOLDOWN       (FRAMERATE_MAX)      // (ship is vulnerable for a single frame) 
#define PLAYER_FEEL_TIME           (3.0f)               // 
#define PLAYER_FEEL_TIME_CONTINUE  (5.0f)               // 
#define PLAYER_FEEL_TIME_REPAIR    (5.0f)               // 
#define PLAYER_FEEL_TIME_COOP      (0.1f)               // 
#define PLAYER_FEEL_TIME_SHIELD    (10.0f)              // 
#define PLAYER_IGNORE_TIME         (1.1f)               // 
#define PLAYER_INTERRUPT           (0.3f)               // 
#define PLAYER_DESATURATE          (1.2f)               // 
#define PLAYER_DESATURATE_FRAGMENT (1.8f)               // 

#define PLAYER_SHIP_ATK  (0u)        // 
#define PLAYER_SHIP_DEF  (1u)        // 
#define PLAYER_SHIP_P1   (2u)        // 
#define PLAYER_NO_ROLL   (0xFFu)     // 
#define PLAYER_NO_FEEL   (-100.0f)   // 
#define PLAYER_NO_IGNORE (-100.0f)   // 

#define PLAYER_ACTION_SHOOT(i,j)   ((i) * WEAPON_MODES + (j))
#define PLAYER_ACTION_TURN_LEFT    (PLAYER_EQUIP_WEAPONS * WEAPON_MODES)
#define PLAYER_ACTION_TURN_RIGHT   (PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 1u)
#define PLAYER_ACTION_ROLL         (PLAYER_EQUIP_WEAPONS * WEAPON_MODES)   // TODO 1: not used
#define PLAYER_ACTION_SHOOT_UP     (PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 2u)
#define PLAYER_ACTION_SHOOT_LEFT   (PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 3u)
#define PLAYER_ACTION_SHOOT_DOWN   (PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 4u)
#define PLAYER_ACTION_SHOOT_RIGHT  (PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 5u)
#define PLAYER_ACTION_RAPID_FIRE   (PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 7u)
#define PLAYER_ACTION_CHANGE_SPEED (PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 8u)

#define PLAYER_AREA_DEFAULT (coreVector4(-FOREGROUND_AREA, FOREGROUND_AREA) * PLAYER_AREA_FACTOR)

//STATIC_ASSERT(PLAYER_INTERRUPT > (1.0f / PLAYER_ROLL_SPEED))

enum ePlayerStatus : coreUint16
{
    PLAYER_STATUS_DEAD           = 0x0001u,   // completely removed from the game
    PLAYER_STATUS_SHIELDED       = 0x0002u,   // 
    PLAYER_STATUS_GHOST          = 0x0004u,   // 
    PLAYER_STATUS_INVINCIBLE     = 0x0008u,   // 
    PLAYER_STATUS_INVINCIBLE_2   = 0x0010u,   // 
    PLAYER_STATUS_HEALER         = 0x0020u,   // 
    PLAYER_STATUS_TOP            = 0x0040u,   // 
    PLAYER_STATUS_KEEP_RANGE     = 0x0080u,   // TODO 1: still used ?
    PLAYER_STATUS_GYRO           = 0x0100u,   // 
    PLAYER_STATUS_ARRANGE        = 0x0200u,   // 
    PLAYER_STATUS_WEAK_BACK      = 0x0400u,   // 
    PLAYER_STATUS_REPAIRED       = 0x0800u,   // 
    PLAYER_STATUS_NO_INPUT_MOVE  = 0x1000u,   // disable player movement (user controls only)
    PLAYER_STATUS_NO_INPUT_SHOOT = 0x2000u,   // disable player weapons
    PLAYER_STATUS_NO_INPUT_ROLL  = 0x4000u,   // 
    PLAYER_STATUS_NO_INPUT_TURN  = 0x8000u,   // 
    PLAYER_STATUS_NO_INPUT_ALL   = PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_SHOOT | PLAYER_STATUS_NO_INPUT_ROLL | PLAYER_STATUS_NO_INPUT_TURN
};

enum ePlayerTest : coreUint8
{
    PLAYER_TEST_NORMAL = 0x01u,   // 
    PLAYER_TEST_ROLL   = 0x02u,   // 
    PLAYER_TEST_FEEL   = 0x04u,   // 
    PLAYER_TEST_IGNORE = 0x08u,   // 
    PLAYER_TEST_ALL    = PLAYER_TEST_NORMAL | PLAYER_TEST_ROLL | PLAYER_TEST_FEEL | PLAYER_TEST_IGNORE
};
ENABLE_BITWISE(ePlayerTest)


// ****************************************************************
// player entity class
class cPlayer final : public cShip
{
private:
    // 
    struct sRayData final
    {
        coreUint32  iFrame;       // 
        coreVector2 vPosition;    // 
        coreVector2 vDirection;   // 
    };


private:
    cWeapon* m_apWeapon[PLAYER_EQUIP_WEAPONS];                // main weapon objects (bullet factories, should never be NULL)

    const sGameInput* m_pInput;                               // pointer to associated input set (should never be NULL)
    coreVector4       m_vArea;                                // 

    coreVector2 m_vForce;                                     // 
    coreFloat   m_fScale;                                     // 
    coreFloat   m_fThrust;                                    // 
    coreFloat   m_fTilt;                                      // 
    coreBool    m_bRainbow;                                   // 

    coreProtect<coreFloat> m_fMoveSpeed;                      // 
    coreProtect<coreFloat> m_fShootSpeed;                     // 
    coreFloat              m_fAnimSpeed;                      // (also affects feel and ignore duration)   

    coreFlow  m_fRollTime;                                    // 
    coreFlow  m_fFeelTime;                                    // 
    coreFlow  m_fIgnoreTime;                                  // 
    coreUint8 m_iRollDir;                                     // 
    coreUint8 m_iFeelType;                                    // 
    coreUint8 m_iIgnoreType;                                  // 

    coreFlow  m_fInterrupt;                                   // 
    coreFlow  m_fLightningTime;                               // 
    coreFloat m_fLightningAngle;                              // 

    coreFlow m_fDesaturate;                                   // 

    coreProtect<coreInt32> m_iMaxShield;                      // 
    coreProtect<coreInt32> m_iCurShield;                      // 
    coreProtect<coreInt32> m_iPreShield;                      // 

    cDataTable  m_DataTable;                                  // 
    cScoreTable m_ScoreTable;                                 // 

    coreProgramPtr m_pNormalProgram;                          // 
    coreProgramPtr m_pDarkProgram;                            // 
    coreFlow       m_fAnimation;                              // 
    coreUint16     m_iLook;                                   // 
    coreFloat      m_fRangeScale;                             // 
    coreVector3    m_vMenuColor;                              // 
    coreVector3    m_vLedColor;                               // 

    coreVector2 m_vOldDir;                                    // 
    coreVector2 m_vSmoothOri;                                 // 
    coreFloat   m_fSmoothThrust;                              // 
    coreFloat   m_fSmoothTilt;                                // 
    coreFlow    m_fRangeValue;                                // 
    coreFlow    m_fArrowValue;                                // 
    coreFlow    m_fGyroValue;                                 // 
    coreFlow    m_fBubbleValue;                               // 
    coreFlow    m_fCircleValue;                               // 
    coreFlow    m_fBoost;                                     // 
    coreUint8   m_iLastMove;                                  // 
    coreUint8   m_iLastHold;                                  // 
    coreUint8   m_iShootToggle;                               // 

    coreObject3D m_Dot;                                       // 
    coreObject3D m_Range;                                     // 
    coreObject3D m_Arrow;                                     // 
    coreObject3D m_Gyro;                                      // 
    coreObject3D m_Wind;                                      // 
    coreObject3D m_Bubble;                                    // 
    coreObject3D m_aShield[2];                                // 
    coreObject3D m_Circle;                                    // 
    coreObject3D m_Exhaust;                                   // 

    coreMap<const coreObject3D*, coreUint32> m_aiCollision;   // 
    coreMap<const coreObject3D*, sRayData>   m_aRayData;      // 
    
    coreBool m_bWasDamaged;
    coreBool m_bGiveUp;


public:
    cPlayer()noexcept;
    ~cPlayer()final;

    DISABLE_COPY(cPlayer)

    // configure the player
    void Configure   (const coreUintW iShipType);
    void EquipShield (const coreInt32 iShield);
    void EquipWeapon (const coreUintW iIndex, const coreInt32 iID);
    void EquipSupport(const coreUintW iIndex, const coreInt32 iID);

    // render and move the player
    void Render      ()final;
    void RenderBefore();
    void RenderMiddle();
    void RenderAfter ();
    void Move        ()final;

    // reduce current health
    coreInt32 TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2 vImpact);

    // 
    void HealHealth(const coreUint8 iHealth);
    void HealShield(const coreUint8 iShield);

    // control life and death
    void Resurrect();
    void Kill     (const coreBool bAnimated);

    // 
    void ShowArrow(const coreUint8 iType);
    void ShowCircle();

    // 
    void StartRolling (const coreVector2 vDirection);
    void StartRolling ();
    void EndRolling   ();
    void StartFeeling (const coreFloat fTime, const coreUint8 iType);
    void EndFeeling   ();
    void StartIgnoring(const coreUint8 iType);
    void EndIgnoring  ();

    // 
    inline void ActivateNormalShading() {this->DefineProgram(m_pNormalProgram);}
    inline void ActivateDarkShading  () {this->DefineProgram(m_pDarkProgram);}

    // 
    void TurnIntoEnemy ();
    void TurnIntoPlayer();

    // 
    inline coreBool IsNormal     ()const {return (!this->IsRolling() && !this->IsFeeling() && !this->IsIgnoring());}
    inline coreBool IsRolling    ()const {return false;}
    inline coreBool IsRolling2   ()const {return (m_iRollDir    != PLAYER_NO_ROLL);}
    inline coreBool IsFeeling    ()const {return (m_fFeelTime   >  PLAYER_NO_FEEL);}
    inline coreBool IsIgnoring   ()const {return (m_fIgnoreTime >  PLAYER_NO_IGNORE);}
    inline coreBool IsDarkShading()const {return (this->GetProgram().GetHandle() == m_pDarkProgram.GetHandle());}
    inline coreBool IsRainbow    ()const {return (m_bRainbow);}
    inline coreBool IsEnemyLook  ()const {return (m_apWeapon[0]->GetID() == cEnemyWeapon::ID);}

    // 
    void EnableRange   ();
    void DisableRange  ();
    void EnableArrow   ();
    void DisableArrow  ();
    void EnableGyro    ();
    void DisableGyro   ();
    void EnableWind    (const coreVector2 vDirection = coreVector2(0.0f,0.0f));
    void DisableWind   ();
    void EnableBubble  ();
    void DisableBubble ();
    void EnableShield  ();
    void DisableShield ();
    void EnableCircle  ();
    void DisableCircle ();
    void EnableExhaust ();
    void DisableExhaust();

    // 
    inline const coreModelPtr& GetRangeModel ()const {return m_Range.GetModel();}
    inline const coreFloat&    GetRangeScale ()const {return m_fRangeScale;}
    inline       coreVector3   GetEnergyColor()const {return m_Range.GetColor3() * (1.0f/1.1f);}
    inline const coreVector3&  GetMenuColor  ()const {return m_vMenuColor;}
    inline const coreVector3&  GetLedColor   ()const {return m_vLedColor;}

    // 
    coreBool TestCollisionPrecise(const coreObject3D* pObject, coreVector3* OUTPUT pvIntersection, coreBool* OUTPUT pbFirstHit);
    coreBool TestCollisionPrecise(const coreVector2 vRayPos, const coreVector2 vRayDir, const coreObject3D* pRef, coreFloat* OUTPUT pfHitDistance, coreUint8* OUTPUT piHitCount, coreBool* OUTPUT pbFirstHit);

    // 
    inline void ApplyForce     (const coreVector2 vForce) {m_vForce += vForce; this->SetPosition(coreVector3(m_vOldPos, 0.0f));}
    inline void ApplyForceRaw  (const coreVector2 vForce) {m_vForce += vForce;}
    inline void ApplyForceTimed(const coreVector2 vForce) {m_vForce += vForce * TIME;}

    // 
    coreVector2 CalcMove     ()const;
    coreFloat   CalcMoveSpeed()const;

    // 
    inline cWeapon*     GetWeapon    (const coreUintW iIndex)const {ASSERT((iIndex < PLAYER_EQUIP_WEAPONS) && m_apWeapon[iIndex]) return m_apWeapon[iIndex];}
    inline cDataTable*  GetDataTable ()                            {return &m_DataTable;}
    inline cScoreTable* GetScoreTable()                            {return &m_ScoreTable;}

    // set object properties
    inline void SetInput       (const sGameInput* pInput)        {m_pInput      = pInput;}
    inline void SetArea        (const coreVector4 vArea)         {m_vArea       = vArea; ASSERT(vArea.xy() < vArea.zw())}
    inline void SetForce       (const coreVector2 vForce)        {m_vForce      = vForce;}
    inline void SetScale       (const coreFloat   fScale)        {m_fScale      = fScale;}
    inline void SetThrust      (const coreFloat   fThrust)       {m_fThrust     = fThrust;}
    inline void SetTilt        (const coreFloat   fTilt)         {m_fTilt       = fTilt;}
    inline void SetRainbow     (const coreBool    bRainbow)      {m_bRainbow    = bRainbow;}
    inline void SetMoveSpeed   (const coreFloat   fMoveSpeed)    {m_fMoveSpeed  = fMoveSpeed;}
    inline void SetShootSpeed  (const coreFloat   fShootSpeed)   {m_fShootSpeed = fShootSpeed;}
    inline void SetAnimSpeed   (const coreFloat   fAnimSpeed)    {m_fAnimSpeed  = fAnimSpeed;}
    inline void SetInterrupt   (const coreFloat   fInterrupt)    {m_fInterrupt  = fInterrupt;}
    inline void SetDesaturate  (const coreFloat   fDesaturate)   {m_fDesaturate = fDesaturate;}
    inline void SetMaxShield   (const coreInt32   iMaxShield)    {m_iMaxShield  = iMaxShield;}
    inline void SetCurShield   (const coreInt32   iCurShield)    {m_iCurShield  = iCurShield;}
    inline void SetCurShieldPct(const coreFloat   fCurShieldPct) {m_iCurShield = F_TO_SI(fCurShieldPct * I_TO_F(m_iMaxShield)); ASSERT((fCurShieldPct >= 0.0f) && (fCurShieldPct <= 1.0f))}

    // get object properties
    inline const sGameInput*  GetInput       ()const {ASSERT(m_pInput) return m_pInput;}
    inline const coreVector4& GetArea        ()const {return m_vArea;}
    inline const coreVector2& GetForce       ()const {return m_vForce;}
    inline const coreFloat&   GetScale       ()const {return m_fScale;}
    inline const coreFloat&   GetTilt        ()const {return m_fTilt;}
    inline       coreFloat    GetMoveSpeed   ()const {return m_fMoveSpeed;}
    inline       coreFloat    GetShootSpeed  ()const {return m_fShootSpeed;}
    inline const coreFloat&   GetAnimSpeed   ()const {return m_fAnimSpeed;}
    inline const coreFloat&   GetInterrupt   ()const {return m_fInterrupt;}
    inline const coreFloat&   GetDesaturate  ()const {return m_fDesaturate;}
    inline       coreInt32    GetMaxShield   ()const {return m_iMaxShield;}
    inline       coreInt32    GetCurShield   ()const {return m_iCurShield;}
    inline       coreFloat    GetCurShieldPct()const {return I_TO_F(m_iCurShield) / I_TO_F(m_iMaxShield);}   // # normal division
    inline       coreInt32    GetPreShield   ()const {return m_iPreShield;}
    inline       coreFloat    GetPreShieldPct()const {return I_TO_F(m_iPreShield) / I_TO_F(m_iMaxShield);}   // # normal division
    
    // 
    template <typename F> static FORCE_INLINE void TestCollision(const ePlayerTest eTest, const coreInt32 iType,                                                          F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit) -> void
    template <typename F> static FORCE_INLINE void TestCollision(const ePlayerTest eTest, coreObject3D* OUTPUT pObject,                                                   F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit) -> void
    template <typename F> static FORCE_INLINE void TestCollision(const ePlayerTest eTest, const coreVector2 vRayPos, const coreVector2 vRayDir, const coreObject3D* pRef, F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, const coreFloat* pfHitDistance, const coreUint8 iHitCount,     const coreBool bFirstHit) -> void
    

    void SetPosition (const coreVector3 vPosition);
    void SetDirection(const coreVector3 vDirection);
    // coreObject3D::Move in teleport
    
    inline coreBool ReachedHealth   (const coreInt32 iHealth)const    {UNREACHABLE return false;}
    inline coreBool ReachedHealthPct(const coreFloat fHealthPct)const {UNREACHABLE return false;}
    inline const coreBool& WasDamaged()const {return m_bWasDamaged;}


private:
    // 
    coreBool __NewCollision(const coreObject3D* pObject);
    sRayData __NewRayData  (const coreVector2 vRayPos, const coreVector2 vRayDir, const coreObject3D* pRef);
    void     __UpdateCollisions();

    // 
    coreBool __CheckPlayerTest(const ePlayerTest eTest)const;
};


// ****************************************************************
// 
template <typename F> FORCE_INLINE void cPlayer::TestCollision(const ePlayerTest eTest, const coreInt32 iType, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, iType, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        if(pPlayer->__CheckPlayerTest(eTest))
        {
            // 
            coreVector3 vNewIntersection;
            coreBool    bNewFirstHit;
            if(pPlayer->TestCollisionPrecise(pObject, &vNewIntersection, &bNewFirstHit))
            {
                nCallback(pPlayer, d_cast<typename TRAIT_ARG_TYPE(F, 1u)>(pObject), vNewIntersection, bNewFirstHit);
            }
        }
    });
}


// ****************************************************************
// 
template <typename F> FORCE_INLINE void cPlayer::TestCollision(const ePlayerTest eTest, coreObject3D* OUTPUT pObject, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, pObject, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        if(pPlayer->__CheckPlayerTest(eTest))
        {
            // 
            coreVector3 vNewIntersection;
            coreBool    bNewFirstHit;
            if(pPlayer->TestCollisionPrecise(pObject, &vNewIntersection, &bNewFirstHit))
            {
                nCallback(pPlayer, d_cast<typename TRAIT_ARG_TYPE(F, 1u)>(pObject), vNewIntersection, bNewFirstHit);
            }
        }
    });
}


// ****************************************************************
// 
template <typename F> FORCE_INLINE void cPlayer::TestCollision(const ePlayerTest eTest, const coreVector2 vRayPos, const coreVector2 vRayDir, const coreObject3D* pRef, F&& nCallback)
{
    // 
    const coreList<coreObject3D*>& oPlayerList = Core::Manager::Object->GetObjectList(TYPE_PLAYER);
    FOR_EACH(it, oPlayerList)
    {
        // 
        cPlayer* pPlayer = d_cast<cPlayer*>(*it);
        if(pPlayer && pPlayer->__CheckPlayerTest(eTest))
        {
            // 
            coreFloat fNewHitDistance;
            coreUint8 iNewHitCount;
            coreBool  bNewFirstHit;
            if(pPlayer->TestCollisionPrecise(vRayPos, vRayDir, pRef, &fNewHitDistance, &iNewHitCount, &bNewFirstHit))
            {
                nCallback(pPlayer, &fNewHitDistance, iNewHitCount, bNewFirstHit);
            }
        }
    }
}


#endif // _P1_GUARD_PLAYER_H_