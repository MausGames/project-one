//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_PLAYER_H_
#define _P1_GUARD_PLAYER_H_

// TODO: add all weapons to player directly in class
// TODO: collision adjustment was not executed once
// TODO: all parts of player-rendering should be batched for coop


// ****************************************************************
// player definitions
#define PLAYER_WEAPONS        (1u)     // number of weapons a player can carry
#define PLAYER_COLLISION_SIZE (0.2f)   // 
#define PLAYER_WIND_SIZE      (4.5f)   // 
#define PLAYER_BUBBLE_SIZE    (6.0f)   // 
#define PLAYER_ROLL_SPEED     (3.3f)   // 
#define PLAYER_ROLL_COOLDOWN  (4.0f)   // 

#define PLAYER_SHIP_ATK   (0u)   // 
#define PLAYER_SHIP_DEF   (1u)   // 
#define PLAYER_DARK_ON    (0u)   // 
#define PLAYER_DARK_OFF   (1u)   // 
#define PLAYER_DARK_RESET (2u)   // 

#define PLAYER_NO_ROLL (0xFFu)   // 

#define __PLAYER_COMBO(x) (LERP(50.0f, 1.0f, RCP(1.0f + I_TO_F(x)*0.0002f)))

enum ePlayerStatus : coreUint8
{
    PLAYER_STATUS_DEAD           = 0x01u,   // completely removed from the game
    PLAYER_STATUS_NO_INPUT_MOVE  = 0x02u,   // disable player movement (user controls only)
    PLAYER_STATUS_NO_INPUT_SHOOT = 0x04u,   // disable player weapons
    PLAYER_STATUS_NO_INPUT_ROLL  = 0x08u,   // 
    PLAYER_STATUS_NO_INPUT_ALL   = PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_SHOOT | PLAYER_STATUS_NO_INPUT_ROLL,
    PLAYER_STATUS_DARKNESS       = 0x10u    // 
};


// ****************************************************************
// player entity class
class cPlayer final : public cShip
{
private:
    cWeapon* m_apWeapon[PLAYER_WEAPONS];                     // main weapon objects (bullet factories, should never be NULL)

    const sGameInput* m_pInput;                              // pointer to associated input set (should never be NULL)

    coreVector2 m_vForce;                                    // 
    coreFlow    m_fRollTime;                                 // 
    coreUint8   m_iRollDir;                                  // 

    coreProtect<coreUint32> m_iScoreGame;                    // 
    coreProtect<coreUint32> m_iScoreMission;                 // 
    coreProtect<coreUint32> m_aiScoreBoss[MISSION_BOSSES];   // 

    coreProtect<coreUint32> m_iComboValue[2];                // absolute values for combo calculations (0 = current value, 1 = max value) 
    coreProtect<coreUint32> m_iChainValue[2];                // 
    coreProtect<coreFloat>  m_fChainCooldown;                // 

    coreProgramPtr m_pDarkProgram;                           // 
    coreFlow       m_fAnimation;                             // 

    coreObject3D m_Wind;                                     // 
    coreObject3D m_Bubble;                                   // 
    coreObject3D m_Exhaust;                                  // 


public:
    cPlayer()noexcept;
    ~cPlayer()final;

    ENABLE_COPY(cPlayer)

    // configure the player
    void Configure  (const coreUintW iShipType, const coreVector3& vColor);
    void EquipWeapon(const coreUintW iIndex, const coreInt32 iID);

    // render and move the player
    void Render()final;
    void Move  ()final;

    // reduce current health
    coreBool TakeDamage(const coreInt32 iDamage, const coreUint8 iElement);

    // control life and death
    void Resurrect(const coreVector2& vPosition);
    void Kill     (const coreBool     bAnimated);

    // control scoring stats
    void AddScore(const coreUint32 iValue, const coreBool bModified);
    void AddCombo(const coreUint32 iValue);
    void AddCombo(const coreFloat  fModifier);
    void AddChain(const coreUint32 iValue);
    void ReduceCombo  ();
    void TransferChain();
    void ResetStats   ();

    // 
    void StartRolling(const coreVector2& vDirection);
    void EndRolling  ();
    inline coreBool IsRolling()const {return (m_iRollDir != PLAYER_NO_ROLL);}

    // 
    void TransformDark(const coreUint8 iStatus);

    // 
    void EnableWind   ();
    void DisableWind  ();
    void EnableBubble ();
    void DisableBubble();
    void UpdateExhaust(const coreFloat fStrength);

    // 
    inline cWeapon* GetWeapon(const coreUintW iIndex)const {ASSERT((iIndex < PLAYER_WEAPONS) && m_apWeapon[iIndex]) return m_apWeapon[iIndex];}

    // set object properties
    inline void SetInput(const sGameInput*  pInput) {m_pInput = pInput;}
    inline void SetForce(const coreVector2& vForce) {m_vForce = vForce;}

    // get object properties
    inline const sGameInput*  GetInput        ()const                       {ASSERT(m_pInput) return m_pInput;}
    inline const coreVector2& GetForce        ()const                       {return m_vForce;}
    inline       coreUint32   GetScoreGame    ()const                       {return m_iScoreGame;}
    inline       coreUint32   GetScoreMission ()const                       {return m_iScoreMission;}
    inline       coreUint32   GetScoreBoss    (const coreUintW iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_aiScoreBoss[iIndex];}
    inline       coreFloat    GetCurCombo     ()const                       {return __PLAYER_COMBO(m_iComboValue[0]);}
    inline       coreFloat    GetMaxCombo     ()const                       {return __PLAYER_COMBO(m_iComboValue[1]);}
    inline       coreUint32   GetCurChain     ()const                       {return m_iChainValue[0];}
    inline       coreUint32   GetMaxChain     ()const                       {return m_iChainValue[1];}
    inline       coreFloat    GetChainCooldown()const                       {return m_fChainCooldown;}
};


#endif // _P1_GUARD_PLAYER_H_