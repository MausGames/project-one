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
    bool m_bDead;   // completely removed from the game


public:
    cPlayer()noexcept;
    ~cPlayer();

    // render and move the player
    void Render()override;
    void Move  ()override;

    // control life and death
    void Resurrect();
    void Kill(const bool& bAnimated);
    inline const bool& IsDead()const {return m_bDead;}


private:
    DISABLE_COPY(cPlayer)
};


#endif // _P1_GUARD_PLAYER_H_