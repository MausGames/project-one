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
// TODO: render wind later to prevent depth/overdraw issues
// TODO: correct reverse-tracking when hitting the walls (position correction) ? only for 45degree, also on other code locations ?


// ****************************************************************
// player definitions
#define PLAYER_WEAPONS            (1u)       // number of weapons a player can carry
#define PLAYER_LIVES              (LIVES)    // 
#define PLAYER_SHIELD             (SHIELD)   // 
#define PLAYER_COLLISION_MIN      (0.5f)     // 
#define PLAYER_WIND_SIZE          (4.5f)     // 
#define PLAYER_BUBBLE_SIZE        (6.0f)     // 
#define PLAYER_ROLL_SPEED         (1.0f)     // 
#define PLAYER_ROLL_COOLDOWN      (60.0f)    // (60 is fastest cooldown, ship is vulnerable for a single frame) 
#define PLAYER_FEEL_TIME          (3.0f)     // 
#define PLAYER_FEEL_TIME_CONTINUE (5.0f)     // 

#define PLAYER_SHIP_ATK (0u)        // 
#define PLAYER_SHIP_DEF (1u)        // 
#define PLAYER_NO_ROLL  (0xFFu)     // 
#define PLAYER_NO_FEEL  (-100.0f)   // 

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


// ****************************************************************
// player entity class
class cPlayer final : public cShip
{
private:
    cWeapon* m_apWeapon[PLAYER_WEAPONS];                        // main weapon objects (bullet factories, should never be NULL)

    const sGameInput* m_pInput;                                 // pointer to associated input set (should never be NULL)

    coreVector2 m_vForce;                                       // 
    coreFlow    m_fFeelTime;                                    // 
    coreFlow    m_fRollTime;                                    // 
    coreUint8   m_iFeelType;                                    // 
    coreUint8   m_iRollDir;                                     // 

    cScoreTable m_ScoreTable;                                   // 

    coreProgramPtr m_pNormalProgram;                            // 
    coreProgramPtr m_pDarkProgram;                              // 
    coreFlow       m_fAnimation;                                // 

    coreObject3D m_Dot;                                         // 
    coreObject3D m_Wind;                                        // 
    coreObject3D m_Bubble;                                      // 
    coreObject3D m_Exhaust;                                     // 

    coreLookup<const coreObject3D*, coreUint32> m_aCollision;   // 


public:
    cPlayer()noexcept;
    ~cPlayer()final;

    DISABLE_COPY(cPlayer)

    // configure the player
    void Configure  (const coreUintW iShipType, const coreVector3& vColor);
    void EquipWeapon(const coreUintW iIndex, const coreInt32 iID);
    void GiveShield ();

    // render and move the player
    void Render()final;
    void Move  ()final;

    // reduce current health
    coreBool TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2& vImpact);

    // control life and death
    void Resurrect(const coreVector2& vPosition);
    void Kill     (const coreBool     bAnimated);

    // 
    void StartRolling(const coreVector2& vDirection);
    void EndRolling  ();
    inline coreBool IsRolling()const {return (m_iRollDir != PLAYER_NO_ROLL);}

    // 
    void StartFeeling(const coreFloat fTime, const coreUint8 iType);
    void EndFeeling  ();
    inline coreBool IsFeeling()const {return (m_fFeelTime > PLAYER_NO_FEEL);}

    // 
    inline void     ActivateNormalShading()      {this->DefineProgram(m_pNormalProgram);}
    inline void     ActivateDarkShading  ()      {this->DefineProgram(m_pDarkProgram);}
    inline coreBool IsDarkShading        ()const {return (this->GetProgram().GetHandle() == m_pDarkProgram.GetHandle()) ? true : false;}

    // 
    void EnableWind   (const coreVector2& vDirection);
    void DisableWind  ();
    void EnableBubble ();
    void DisableBubble();
    void UpdateExhaust(const coreFloat fStrength);

    // 
    inline cWeapon*     GetWeapon    (const coreUintW iIndex)const {ASSERT((iIndex < PLAYER_WEAPONS) && m_apWeapon[iIndex]) return m_apWeapon[iIndex];}
    inline cScoreTable* GetScoreTable()                            {return &m_ScoreTable;}

    // set object properties
    inline void SetInput(const sGameInput*  pInput) {m_pInput = pInput;}
    inline void SetForce(const coreVector2& vForce) {m_vForce = vForce;}

    // get object properties
    inline const sGameInput*  GetInput   ()const {ASSERT(m_pInput) return m_pInput;}
    inline const coreVector2& GetForce   ()const {return m_vForce;}
    inline       coreFloat    GetFeelTime()const {return (m_iFeelType <= 1u) ? m_fFeelTime : 0.0f;}

    // 
    template <typename F> static FORCE_INLINE void TestCollision(const coreInt32 iType,        F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit) -> void
    template <typename F> static FORCE_INLINE void TestCollision(coreObject3D* OUTPUT pObject, F&& nCallback);   // [](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit) -> void


private:
    // 
    coreBool __TestCollisionPrecise(const coreObject3D* pObject, coreVector3* OUTPUT pvIntersection, coreBool* OUTPUT pbFirstHit);
    coreBool __NewCollision        (const coreObject3D* pObject);
    void     __UpdateCollisions    ();
};


// ****************************************************************
// 
template <typename F> FORCE_INLINE void cPlayer::TestCollision(const coreInt32 iType, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, iType, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        coreVector3 vNewIntersection;
        coreBool    bNewFirstHit;
        if(pPlayer->__TestCollisionPrecise(pObject, &vNewIntersection, &bNewFirstHit))
        {
            nCallback(pPlayer, d_cast<typename TRAIT_ARG_TYPE(F, 1u)>(pObject), vNewIntersection, bNewFirstHit);
        }
    });
}

template <typename F> FORCE_INLINE void cPlayer::TestCollision(coreObject3D* OUTPUT pObject, F&& nCallback)
{
    // 
    Core::Manager::Object->TestCollision(TYPE_PLAYER, pObject, [&](cPlayer* OUTPUT pPlayer, coreObject3D* OUTPUT pObject, const coreVector3& vIntersection, const coreBool bFirstHit)
    {
        // 
        coreVector3 vNewIntersection;
        coreBool    bNewFirstHit;
        if(pPlayer->__TestCollisionPrecise(pObject, &vNewIntersection, &bNewFirstHit))
        {
            nCallback(pPlayer, d_cast<typename TRAIT_ARG_TYPE(F, 1u)>(pObject), vNewIntersection, bNewFirstHit);
        }
    });
}


#endif // _P1_GUARD_PLAYER_H_