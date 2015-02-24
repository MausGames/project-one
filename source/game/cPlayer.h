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


// ****************************************************************
// player definitions
#define PLAYER_WEAPONS  (1u)   // number of weapons a player can carry
#define PLAYER_SHIP_OFF (0)    // 
#define PLAYER_SHIP_DEF (1)    // 

#define PLAYER_COMBO(x) (LERP(50.0f, 1.0f, RCP(1.0f + I_TO_F(x)*0.0002f)))

enum ePlayerStatus : coreByte
{
    PLAYER_STATUS_DEAD            = 0x01,   // completely removed from the game
    PLAYER_STATUS_NO_INPUT_MOVE   = 0x02,   // 
    PLAYER_STATUS_NO_INPUT_WEAPON = 0x04,   // 
    PLAYER_STATUS_NO_INPUT_SKILL  = 0x08,   // 
    PLAYER_STATUS_NO_INPUT_ALL    = PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_WEAPON | PLAYER_STATUS_NO_INPUT_SKILL
};


// ****************************************************************
// player entity class
class cPlayer final : public cShip
{
private:
    cWeapon* m_apWeapon[PLAYER_WEAPONS];      // main weapon objects (bullet factories, should never be NULL)
    coreByte m_iInputIndex;                   // input set identifier

    coreUint m_iScoreMission;                 // 
    coreUint m_aiScoreBoss[MISSION_BOSSES];   // 

    coreUint m_iComboValue[2];                // absolute values for combo calculations (0 = current value, 1 = max value) 
    coreUint m_iChainValue[2];                // 
    coreFlow m_fChainCooldown;                // 


public:
    cPlayer()noexcept;
    ~cPlayer();

    DISABLE_COPY(cPlayer)

    // configure and equip the player
    void Configure  (const coreByte& iShipType, const coreVector3& vColor, const coreByte& iInputIndex);
    void EquipWeapon(const coreByte& iIndex, const int& iID);

    // render and move the player
    void Render()override;
    void Move  ()override;

    // reduce current health
    void TakeDamage(const int& iDamage);

    // control life and death
    void Resurrect(const coreVector2& vPosition);
    void Kill     (const bool&        bAnimated);

    // control scoring stats
    void AddScore(const coreUint& iValue, const bool& bModified);
    void AddCombo(const coreUint& iValue);
    void AddCombo(const float&    fModifier);
    void AddChain(const coreUint& iValue);
    void ResetStats();

    // get object properties
    inline const coreByte& GetInputIndex  ()const                       {return m_iInputIndex;}
    inline const coreUint& GetScoreMission()const                       {return m_iScoreMission;}
    inline const coreUint& GetScoreBoss   (const coreByte& iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_aiScoreBoss[iIndex];}
    inline float           GetCurCombo    ()const                       {return PLAYER_COMBO(m_iComboValue[0]);}
    inline float           GetMaxCombo    ()const                       {return PLAYER_COMBO(m_iComboValue[1]);}
    inline const coreUint& GetCurChain    ()const                       {return m_iChainValue[0];}
    inline const coreUint& GetMaxChain    ()const                       {return m_iChainValue[1];}
};


#endif // _P1_GUARD_PLAYER_H_