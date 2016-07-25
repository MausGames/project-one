//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_LEADERBOARD_H_
#define _P1_GUARD_LEADERBOARD_H_


// ****************************************************************
// 
#define LEADERBOARD_ENTRIES     (10u)   // 
#define LEADERBOARD_NAME_LENGTH (32u)   // 


// ****************************************************************
// 
class cLeaderboard final : public coreObject2D
{
private:
    coreObject2D m_aLine [LEADERBOARD_ENTRIES];   // 
    coreLabel    m_aRank [LEADERBOARD_ENTRIES];   // 
    coreLabel    m_aName [LEADERBOARD_ENTRIES];   // 
    coreLabel    m_aScore[LEADERBOARD_ENTRIES];   // 


public:
    cLeaderboard()noexcept;

    DISABLE_COPY(cLeaderboard)

    // 
    void Render()final;
    void Move  ()final;
};


#endif // _P1_GUARD_LEADERBOARD_H_