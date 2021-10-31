///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
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
// TODO 3: correct reverse-tracking when hitting the walls (position correction) ? only for 45degree, also on other code locations ?
// TODO 3: when applying force with (all) objects (collision with container) always quantize 4 or 8, but not in general (wind)
// TODO 4: remove dark shading resources, if not required anymore (maybe for bonus phases)
// TODO 3: add more delay to bubble/feeling (to stay longer invincible after bubble disappeared)


// ****************************************************************
// player definitions
#define PLAYER_EQUIP_WEAPONS      (EQUIP_WEAPONS)   // number of weapons a player can carry
#define PLAYER_EQUIP_SUPPORTS     (EQUIP_SUPPORTS)  // 
#define PLAYER_LIVES              (LIVES)           // 
#define PLAYER_SHIELD             (SHIELD)          // 
#define PLAYER_COLLISION_MIN      (0.15f)           // 
#define PLAYER_AREA_FACTOR        (1.06f)           // 
#define PLAYER_RANGE_SIZE         (1.04f)           // 
#define PLAYER_WIND_SIZE          (4.5f)            // 
#define PLAYER_BUBBLE_SIZE        (4.8f)            // 
#define PLAYER_ROLL_SPEED         (1.0f)            // 
#define PLAYER_ROLL_COOLDOWN      (FRAMERATE_MAX)   // (ship is vulnerable for a single frame) 
#define PLAYER_FEEL_TIME          (3.0f)            // 
#define PLAYER_FEEL_TIME_CONTINUE (5.0f)            // 
#define PLAYER_FEEL_TIME_REPAIR   (5.0f)            // 
#define PLAYER_FEEL_TIME_SHIELD   (10.0f)           // 
#define PLAYER_INTERRUPT          (2.0f)            // 
#define PLAYER_DESATURATE         (1.2f)            // 

#define PLAYER_SHIP_ATK  (0u)        // 
#define PLAYER_SHIP_DEF  (1u)        // 
#define PLAYER_SHIP_P1   (2u)        // 
#define PLAYER_NO_ROLL   (0xFFu)     // 
#define PLAYER_NO_FEEL   (-100.0f)   // 
#define PLAYER_NO_IGNORE (-100.0f)   // 

STATIC_ASSERT(PLAYER_INTERRUPT > (1.0f / PLAYER_ROLL_SPEED))

enum ePlayerStatus : coreUint8
{
    PLAYER_STATUS_DEAD           = 0x01u,   // completely removed from the game
    PLAYER_STATUS_SHIELDED       = 0x02u,   // 
    PLAYER_STATUS_NO_INPUT_MOVE  = 0x04u,   // disable player movement (user controls only)
    PLAYER_STATUS_NO_INPUT_SHOOT = 0x08u,   // disable player weapons
    PLAYER_STATUS_NO_INPUT_ROLL  = 0x10u,   // 
    PLAYER_STATUS_NO_INPUT_TURN  = 0x20u,   // 
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
    cWeapon* m_apWeapon[PLAYER_EQUIP_WEAPONS];               // main weapon objects (bullet factories, should never be NULL)

    const sGameInput* m_pInput;                              // pointer to associated input set (should never be NULL)
    coreVector4       m_vArea;                               // 

    coreVector2 m_vForce;                                    // 
    coreFloat   m_fSpeed;                                    // 

    coreFlow  m_fRollTime;                                   // 
    coreFlow  m_fFeelTime;                                   // 
    coreFlow  m_fIgnoreTime;                                 // 
    coreUint8 m_iRollDir;                                    // 
    coreUint8 m_iFeelType;                                   // 
    coreUint8 m_iIgnoreType;                                 // 

    coreFlow  m_fInterrupt;                                  // 
    coreFlow  m_fLightningTime;                              // 
    coreFloat m_fLightningAngle;                             // 

    coreFlow m_fDesaturate;                                  // 

    coreProtect<coreInt32> m_iMaxShield;                     // 
    coreProtect<coreInt32> m_iCurShield;                     // 

    cDataTable  m_DataTable;                                 // 
    cScoreTable m_ScoreTable;                                // 

    coreProgramPtr m_pNormalProgram;                         // 
    coreProgramPtr m_pDarkProgram;                           // 
    coreFlow       m_fAnimation;                             // 

    coreVector2 m_vOldDir;                                   // 
    coreFlow    m_fRangeValue;                               // 
    coreFlow    m_fArrowValue;                               // 

    coreObject3D m_Dot;                                      // 
    coreObject3D m_Range;                                    // 
    coreObject3D m_Arrow;                                    // 
    coreObject3D m_Wind;                                     // 
    coreObject3D m_Bubble;                                   // 
    coreObject3D m_aShield[2];                               // 
    coreObject3D m_Exhaust;                                  // 

    coreMap<const coreObject3D*, coreUint32> m_aCollision;   // 


public:
    cPlayer()noexcept;
    ~cPlayer()final;

    DISABLE_COPY(cPlayer)

    // configure the player
    void Configure   (const coreUintW iShipType);
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

    // control life and death
    void Resurrect();
    void Kill     (const coreBool bAnimated);

    // 
    void StartRolling (const coreVector2 vDirection);
    void EndRolling   ();
    void StartFeeling (const coreFloat fTime, const coreUint8 iType);
    void EndFeeling   ();
    void StartIgnoring(const coreUint8 iType);
    void EndIgnoring  ();

    // 
    inline void ActivateNormalShading() {this->DefineProgram(m_pNormalProgram);}
    inline void ActivateDarkShading  () {this->DefineProgram(m_pDarkProgram);}

    // 
    inline coreBool IsNormal     ()const {return (!this->IsRolling() && !this->IsFeeling() && !this->IsIgnoring());}
    inline coreBool IsRolling    ()const {return (m_iRollDir    != PLAYER_NO_ROLL);}
    inline coreBool IsFeeling    ()const {return (m_fFeelTime   >  PLAYER_NO_FEEL);}
    inline coreBool IsIgnoring   ()const {return (m_fIgnoreTime >  PLAYER_NO_IGNORE);}
    inline coreBool IsDarkShading()const {return (this->GetProgram().GetHandle() == m_pDarkProgram.GetHandle());}

    // 
    void EnableRange  ();
    void DisableRange ();
    void EnableArrow  ();
    void DisableArrow ();
    void EnableWind   (const coreVector2 vDirection);
    void DisableWind  ();
    void EnableBubble ();
    void DisableBubble();
    void EnableShield ();
    void DisableShield();
    void UpdateExhaust(const coreFloat fStrength);

    // 
    coreBool TestCollisionPrecise(const coreObject3D* pObject, coreVector3* OUTPUT pvIntersection, coreBool* OUTPUT pbFirstHit);

    // 
    inline void ApplyForce     (const coreVector2 vForce) {m_vForce += vForce; this->SetPosition(coreVector3(m_vOldPos, 0.0f));}
    inline void ApplyForceTimed(const coreVector2 vForce) {m_vForce += vForce * TIME;}

    // 
    coreVector2 CalcMove     ()const;
    coreFloat   CalcMoveSpeed()const;

    // 
    inline cWeapon*     GetWeapon    (const coreUintW iIndex)const {ASSERT((iIndex < PLAYER_EQUIP_WEAPONS) && m_apWeapon[iIndex]) return m_apWeapon[iIndex];}
    inline cDataTable*  GetDataTable ()                            {return &m_DataTable;}
    inline cScoreTable* GetScoreTable()                            {return &m_ScoreTable;}

    // set object properties
    inline void SetInput     (const sGameInput* pInput)      {m_pInput      = pInput;}
    inline void SetArea      (const coreVector4 vArea)       {m_vArea       = vArea; ASSERT(vArea.xy() < vArea.zw())}
    inline void SetForce     (const coreVector2 vForce)      {m_vForce      = vForce;}
    inline void SetSpeed     (const coreFloat   fSpeed)      {m_fSpeed      = fSpeed;}
    inline void SetInterrupt (const coreFloat   fInterrupt)  {m_fInterrupt  = fInterrupt;}
    inline void SetDesaturate(const coreFloat   fDesaturate) {m_fDesaturate = fDesaturate;}
    inline void SetCurShield (const coreUint8   iCurShield)  {m_iCurShield  = iCurShield;}

    // get object properties
    inline const sGameInput*  GetInput       ()const {ASSERT(m_pInput) return m_pInput;}
    inline const coreVector4& GetArea        ()const {return m_vArea;}
    inline const coreVector2& GetForce       ()const {return m_vForce;}
    inline const coreFloat&   GetSpeed       ()const {return m_fSpeed;}
    inline const coreFloat&   GetInterrupt   ()const {return m_fInterrupt;}
    inline const coreFloat&   GetDesaturate  ()const {return m_fDesaturate;}
    inline       coreInt32    GetMaxShield   ()const {return m_iMaxShield;}
    inline       coreInt32    GetCurShield   ()const {return m_iCurShield;}
    inline       coreFloat    GetCurShieldPct()const {return I_TO_F(m_iCurShield) * RCP(I_TO_F(m_iMaxShield));}

    // 
    template <typename F> static FORCE_INLINE void TestCollision(const ePlayerTest eTest, const coreInt32 iType,        F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit) -> void
    template <typename F> static FORCE_INLINE void TestCollision(const ePlayerTest eTest, coreObject3D* OUTPUT pObject, F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit) -> void


private:
    // 
    void __EquipShield();

    // 
    coreBool __NewCollision(const coreObject3D* pObject);
    void     __UpdateCollisions();
};


// ****************************************************************
// 
template <typename F> FORCE_INLINE void cPlayer::TestCollision(const ePlayerTest eTest, const coreInt32 iType, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, iType, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        if(pPlayer->IsRolling() ? HAS_FLAG(eTest, PLAYER_TEST_ROLL) : (pPlayer->IsFeeling() ? HAS_FLAG(eTest, PLAYER_TEST_FEEL) : (pPlayer->IsIgnoring() ? HAS_FLAG(eTest, PLAYER_TEST_IGNORE) : HAS_FLAG(eTest, PLAYER_TEST_NORMAL))))
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

template <typename F> FORCE_INLINE void cPlayer::TestCollision(const ePlayerTest eTest, coreObject3D* OUTPUT pObject, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, pObject, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        // 
        if(pPlayer->IsRolling() ? HAS_FLAG(eTest, PLAYER_TEST_ROLL) : (pPlayer->IsFeeling() ? HAS_FLAG(eTest, PLAYER_TEST_FEEL) : (pPlayer->IsIgnoring() ? HAS_FLAG(eTest, PLAYER_TEST_IGNORE) : HAS_FLAG(eTest, PLAYER_TEST_NORMAL))))
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


#endif // _P1_GUARD_PLAYER_H_