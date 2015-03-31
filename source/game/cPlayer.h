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
#define PLAYER_WEAPONS        (1u)     // number of weapons a player can carry
#define PLAYER_COLLISION_SIZE (0.3f)   // 
#define PLAYER_SHIP_ATK       (0u)     // 
#define PLAYER_SHIP_DEF       (1u)     // 

#define PLAYER_COMBO(x) (LERP(50.0f, 1.0f, RCP(1.0f + I_TO_F(x)*0.0002f)))

enum ePlayerStatus : coreUint8
{
    PLAYER_STATUS_DEAD            = 0x01u,   // completely removed from the game
    PLAYER_STATUS_NO_INPUT_MOVE   = 0x02u,   // disable player movement (user controls only)
    PLAYER_STATUS_NO_INPUT_WEAPON = 0x04u,   // disable player weapons
    PLAYER_STATUS_NO_INPUT_SKILL  = 0x08u,   // disable player skills
    PLAYER_STATUS_NO_INPUT_ALL    = PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_WEAPON | PLAYER_STATUS_NO_INPUT_SKILL
};


// ****************************************************************
// player entity class
class cPlayer final : public cShip
{
private:
    cWeapon*  m_apWeapon[PLAYER_WEAPONS];       // main weapon objects (bullet factories, should never be NULL)
    coreUintW m_iInputIndex;                    // input set identifier

    coreUint32 m_iScoreMission;                 // 
    coreUint32 m_aiScoreBoss[MISSION_BOSSES];   // 

    coreUint32 m_iComboValue[2];                // absolute values for combo calculations (0 = current value, 1 = max value) 
    coreUint32 m_iChainValue[2];                // 
    coreFlow   m_fChainCooldown;                // 

    coreVector2 m_vNewPos;                      // new position for smooth movement and animation


public:
    cPlayer()noexcept;
    ~cPlayer();

    DISABLE_COPY(cPlayer)

    // configure the player
    void Configure  (const coreUintW& iShipType, const coreVector3& vColor, const coreUintW& iInputIndex);
    void EquipWeapon(const coreUintW& iIndex, const coreInt32& iID);

    // render and move the player
    void Render()override;
    void Move  ()override;

    // reduce current health
    void TakeDamage(const coreInt32& iDamage);

    // control life and death
    void Resurrect(const coreVector2& vPosition);
    void Kill     (const coreBool&    bAnimated);

    // control scoring stats
    void AddScore(const coreUint32& iValue, const coreBool& bModified);
    void AddCombo(const coreUint32& iValue);
    void AddCombo(const coreFloat&  fModifier);
    void AddChain(const coreUint32& iValue);
    void ResetStats();

    // set object properties
    inline void SetNewPos(const coreVector2& vNewPos) {m_vNewPos = vNewPos;}

    // get object properties
    inline const coreUintW&   GetInputIndex  ()const                        {return m_iInputIndex;}
    inline const coreUint32&  GetScoreMission()const                        {return m_iScoreMission;}
    inline const coreUint32&  GetScoreBoss   (const coreUintW& iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_aiScoreBoss[iIndex];}
    inline       coreFloat    GetCurCombo    ()const                        {return PLAYER_COMBO(m_iComboValue[0]);}
    inline       coreFloat    GetMaxCombo    ()const                        {return PLAYER_COMBO(m_iComboValue[1]);}
    inline const coreUint32&  GetCurChain    ()const                        {return m_iChainValue[0];}
    inline const coreUint32&  GetMaxChain    ()const                        {return m_iChainValue[1];}
    inline const coreVector2& GetNewPos      ()const                        {return m_vNewPos;}
};


#endif // _P1_GUARD_PLAYER_H_