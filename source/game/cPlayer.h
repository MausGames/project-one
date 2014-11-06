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
// player entity class
class cPlayer final : public coreObject3D
{
private:
    coreVector2 m_vNewPos;    // new position for smooth movement and animation
    bool m_bDead;             // completely removed from the game

    coreByte m_iInputIndex;   // input set identifier 
    

public:
    cPlayer()noexcept;
    ~cPlayer();

    // configure the player
    void Configure(const coreByte& iAppearanceType, const coreVector3& vColor, const coreByte& iInputIndex);

    // render and move the player
    void Render()override;
    void Move  ()override;

    // control life and death
    void Resurrect(const coreVector2& vPosition);
    void Kill     (const bool&        bAnimated);
    inline const bool& IsDead()const {return m_bDead;}

    // get object properties
    inline const coreByte& GetInputIndex()const {return m_iInputIndex;}


private:
    DISABLE_COPY(cPlayer)
};


#endif // _P1_GUARD_PLAYER_H_