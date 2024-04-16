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

// TODO: add all weapons to player directly in class
// TODO: all parts of player-rendering should be batched for coop
// TODO: check which operations have to be done outside of dead-check
// TODO: add in-game hint for roll-cooldown end ((just) acoustic)
// TODO: render wind, bubble, etc. in group for coop
// TODO: correct reverse-tracking when hitting the walls (position correction) ? only for 45degree, also on other code locations ?
// TODO: orange/red exhaust for second ship ?
// TODO: when applying force with (all) objects (collision with container) always quantize 4 or 8, but not in general (wind)


// ****************************************************************
// player definitions
#define PLAYER_EQUIP_WEAPONS      (EQUIP_WEAPONS)   // number of weapons a player can carry
#define PLAYER_EQUIP_SUPPORTS     (EQUIP_SUPPORTS)  // 
#define PLAYER_LIVES              (LIVES)           // 
#define PLAYER_SHIELD             (SHIELD)          // 
#define PLAYER_COLLISION_MIN      (0.15f)           // 
#define PLAYER_WIND_SIZE          (4.5f)            // 
#define PLAYER_BUBBLE_SIZE        (6.0f)            // 
#define PLAYER_ROLL_SPEED         (1.0f)            // 
#define PLAYER_ROLL_COOLDOWN      (FRAMERATE_MAX)   // (ship is vulnerable for a single frame) 
#define PLAYER_FEEL_TIME          (3.0f)            // 
#define PLAYER_FEEL_TIME_CONTINUE (5.0f)            // 
#define PLAYER_FEEL_TIME_REPAIR   (5.0f)            // 
#define PLAYER_FEEL_TIME_SHIELD   (10.0f)           // 
#define PLAYER_INTERRUPT          (0.3f)            // 
#define PLAYER_DESATURATE         (1.2f)            // 

#define PLAYER_SHIP_ATK  (0u)        // 
#define PLAYER_SHIP_DEF  (1u)        // 
#define PLAYER_SHIP_P1   (2u)        // 
#define PLAYER_NO_ROLL   (0xFFu)     // 
#define PLAYER_NO_FEEL   (-100.0f)   // 
#define PLAYER_NO_IGNORE (-100.0f)   // 

//STATIC_ASSERT(PLAYER_INTERRUPT > (1.0f / PLAYER_ROLL_SPEED))

enum ePlayerStatus : coreUint8
{
    PLAYER_STATUS_DEAD           = 0x01u,   // completely removed from the game
    PLAYER_STATUS_PACIFIST       = 0x02u,   // 
    PLAYER_STATUS_SHIELDED       = 0x04u,   // 
    PLAYER_STATUS_NO_INPUT_MOVE  = 0x08u,   // disable player movement (user controls only)
    PLAYER_STATUS_NO_INPUT_SHOOT = 0x10u,   // disable player weapons
    PLAYER_STATUS_NO_INPUT_ROLL  = 0x20u,   // 
    PLAYER_STATUS_NO_INPUT_TURN  = 0x40u,   // 
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

    cDataTable  m_DataTable;                                 // 
    cScoreTable m_ScoreTable;                                // 

    coreProgramPtr m_pNormalProgram;                         // 
    coreProgramPtr m_pDarkProgram;                           // 
    coreFlow       m_fAnimation;                             // 

    coreObject3D m_Dot;                                      // 
    coreObject3D m_Range;                                    // 
    coreObject3D m_Wind;                                     // 
    coreObject3D m_Bubble;                                   // 
    coreObject3D m_Shield;                                   // 
    coreObject3D m_Exhaust;                                  // 

    coreMap<const coreObject3D*, coreUint32> m_aCollision;   // 


public:
    coreObject3D m_Wind2;                                     //            
    cPlayer()noexcept;
    ~cPlayer()final;

    DISABLE_COPY(cPlayer)

    // configure the player
    void Configure   (const coreUintW iShipType, const coreVector3& vColor);
    void EquipWeapon (const coreUintW iIndex, const coreInt32 iID);
    void EquipSupport(const coreUintW iIndex, const coreInt32 iID);

    // render and move the player
    void Render      ()final;
    void RenderBefore();
    void RenderAfter ();
    void Move        ()final;

    // reduce current health
    coreInt32 TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2& vImpact);

    // control life and death
    void Resurrect();
    void Kill     (const coreBool bAnimated);

    // 
    void StartRolling(const coreVector2& vDirection);
    void EndRolling  ();
    inline coreBool IsRolling()const {return (m_iRollDir != PLAYER_NO_ROLL);}

    // 
    void StartFeeling(const coreFloat fTime, const coreUint8 iType);
    void EndFeeling  ();
    inline coreBool IsFeeling()const {return (m_fFeelTime > PLAYER_NO_FEEL);}

    // 
    void StartIgnoring(const coreUint8 iType);
    void EndIgnoring  ();
    inline coreBool IsIgnoring()const {return (m_fIgnoreTime > PLAYER_NO_IGNORE);}

    // 
    inline coreBool IsNormal()const {return (!this->IsRolling() && !this->IsFeeling() && !this->IsIgnoring());}

    // 
    inline void     ActivateNormalShading()      {this->DefineProgram(m_pNormalProgram);}
    inline void     ActivateDarkShading  ()      {this->DefineProgram(m_pDarkProgram);}
    inline coreBool IsDarkShading        ()const {return (this->GetProgram().GetHandle() == m_pDarkProgram.GetHandle());}

    // 
    void EnableWind   (const coreVector2& vDirection);
    void DisableWind  ();
    void EnableBubble ();
    void DisableBubble();
    void EnableShield ();
    void DisableShield();
    void UpdateExhaust(const coreFloat fStrength);

    // 
    inline void ApplyForce(const coreVector2& vForce) {this->SetPosition(coreVector3(m_vOldPos, 0.0f)); m_vForce += vForce;}

    // 
    coreVector2 CalcMove     ()const;
    coreFloat   CalcMoveSpeed()const;

    // 
    inline cWeapon*     GetWeapon    (const coreUintW iIndex)const {ASSERT((iIndex < PLAYER_EQUIP_WEAPONS) && m_apWeapon[iIndex]) return m_apWeapon[iIndex];}
    inline cDataTable*  GetDataTable ()                            {return &m_DataTable;}
    inline cScoreTable* GetScoreTable()                            {return &m_ScoreTable;}

    // set object properties
    inline void SetInput     (const sGameInput*  pInput)      {m_pInput      = pInput;}
    inline void SetArea      (const coreVector4& vArea)       {m_vArea       = vArea; ASSERT(vArea.xy() < vArea.zw())}
    inline void SetForce     (const coreVector2& vForce)      {m_vForce      = vForce;}
    inline void SetSpeed     (const coreFloat    fSpeed)      {m_fSpeed      = fSpeed;}
    inline void SetInterrupt (const coreFloat    fInterrupt)  {m_fInterrupt  = fInterrupt;}
    inline void SetDesaturate(const coreFloat    fDesaturate) {m_fDesaturate = fDesaturate;}

    // get object properties
    inline const sGameInput*  GetInput     ()const {ASSERT(m_pInput) return m_pInput;}
    inline const coreVector4& GetArea      ()const {return m_vArea;}
    inline const coreVector2& GetForce     ()const {return m_vForce;}
    inline const coreFloat&   GetSpeed     ()const {return m_fSpeed;}
    inline const coreFloat&   GetInterrupt ()const {return m_fInterrupt;}
    inline const coreFloat&   GetDesaturate()const {return m_fDesaturate;}

    // 
    template <typename F> static FORCE_INLINE void TestCollision(const ePlayerTest eTest, const coreInt32 iType,        F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit) -> void
    template <typename F> static FORCE_INLINE void TestCollision(const ePlayerTest eTest, coreObject3D* OUTPUT pObject, F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit) -> void

    
    /*
    void SetPosition(const coreVector3& vPosition)
    {
        this->coreObject3D::SetPosition(vPosition);

        m_Range  .SetPosition(vPosition);
        m_Dot    .SetPosition(vPosition);
        m_Wind   .SetPosition(vPosition);
        m_Bubble .SetPosition(vPosition);
        m_Shield .SetPosition(vPosition);
        m_Exhaust.SetPosition(vPosition);
    }
     direction sollte auch aktualisiert werden können
     coreObject3D::Move in teleport
    */
    

private:
    // 
    void __EquipShield();

    // 
    coreBool __TestCollisionPrecise(const coreObject3D* pObject, coreVector3* OUTPUT pvIntersection, coreBool* OUTPUT pbFirstHit);
    coreBool __NewCollision        (const coreObject3D* pObject);
    void     __UpdateCollisions    ();
};


// ****************************************************************
// 
template <typename F> FORCE_INLINE void cPlayer::TestCollision(const ePlayerTest eTest, const coreInt32 iType, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, iType, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        if(pPlayer->IsRolling() ? HAS_FLAG(eTest, PLAYER_TEST_ROLL) : (pPlayer->IsFeeling() ? HAS_FLAG(eTest, PLAYER_TEST_FEEL) : (pPlayer->IsIgnoring() ? HAS_FLAG(eTest, PLAYER_TEST_IGNORE) : HAS_FLAG(eTest, PLAYER_TEST_NORMAL))))
        {
            // 
            coreVector3 vNewIntersection;
            coreBool    bNewFirstHit;
            if(pPlayer->__TestCollisionPrecise(pObject, &vNewIntersection, &bNewFirstHit))
            {
                nCallback(pPlayer, d_cast<typename TRAIT_ARG_TYPE(F, 1u)>(pObject), vNewIntersection, bNewFirstHit);
            }
        }
    });
}

template <typename F> FORCE_INLINE void cPlayer::TestCollision(const ePlayerTest eTest, coreObject3D* OUTPUT pObject, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, pObject, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        if(pPlayer->IsRolling() ? HAS_FLAG(eTest, PLAYER_TEST_ROLL) : (pPlayer->IsFeeling() ? HAS_FLAG(eTest, PLAYER_TEST_FEEL) : (pPlayer->IsIgnoring() ? HAS_FLAG(eTest, PLAYER_TEST_IGNORE) : HAS_FLAG(eTest, PLAYER_TEST_NORMAL))))
        {
            // 
            coreVector3 vNewIntersection;
            coreBool    bNewFirstHit;
            if(pPlayer->__TestCollisionPrecise(pObject, &vNewIntersection, &bNewFirstHit))
            {
                nCallback(pPlayer, d_cast<typename TRAIT_ARG_TYPE(F, 1u)>(pObject), vNewIntersection, bNewFirstHit);
            }
        }
    });
}


#endif // _P1_GUARD_PLAYER_H_