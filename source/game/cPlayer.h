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
enum ePlayerStatus : coreByte
{
    PLAYER_STATUS_DEAD = 0x01   // completely removed from the game
};


// ****************************************************************
// player entity class
class cPlayer final : public cShip
{
private:
    cWeapon* m_pWeapon;       // main weapon object (bullet factory)
    coreByte m_iInputIndex;   // input set identifier


public:
    cPlayer()noexcept;
    ~cPlayer();

    DISABLE_COPY(cPlayer)

    // configure the player
    void Configure  (const coreByte& iAppearanceType, const coreVector3& vColor, const coreByte& iInputIndex);
    void EquipWeapon(const int&      iID);

    // render and move the player
    void Render()override;
    void Move  ()override;

    // control life and death
    void Resurrect(const coreVector2& vPosition);
    void Kill     (const bool&        bAnimated);

    // get object properties
    inline const coreByte& GetInputIndex()const {return m_iInputIndex;}
};


#endif // _P1_GUARD_PLAYER_H_