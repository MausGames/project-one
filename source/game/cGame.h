//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_GAME_H_
#define _P1_GUARD_GAME_H_


// ****************************************************************
// game definitions
#define GAME_PLAYER_NUM (2u)


// ****************************************************************
// game class
class cGame final
{
private:
    cPlayer m_aPlayer[GAME_PLAYER_NUM];


public:
    cGame()noexcept;
    ~cGame();

    // render and move the game
    void Render();
    void Move();

    // access game objects
    inline cPlayer* GetPlayer(const coreByte& iIndex) {return &m_aPlayer[iIndex];}


private:
    DISABLE_COPY(cGame)
};


#endif // _P1_GUARD_GAME_H_