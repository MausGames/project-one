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

// TODO: enemy bullet (and enemy?) cleanup on mission unload
// TODO: dead-check on ForEachPlayer for non-coop ?


// ****************************************************************
// game definitions
#define GAME_PLAYERS        (PLAYERS)   // default number of players
#define GAME_INTRO_DELAY    (0.2f)      // 
#define GAME_INTRO_DURATION (3.5f)      // 

enum cGameStatus : coreUint8
{
    GAME_STATUS_INTRO   = 0x01u,   // 
    GAME_STATUS_OUTRO   = 0x02u,   // 
    GAME_STATUS_PLAY    = 0x04u,   // 
    GAME_STATUS_LOADING = 0x08u    // 
};


// ****************************************************************
// depth-level management
#define DEPTH_PUSH           {g_pGame->PushDepthLevel();}
#define __DEPTH_LEVEL_UNDER  {glDepthRange(0.9f, 1.0f);}
#define __DEPTH_LEVEL_SHIP   {glDepthRange(0.8f, 0.9f);}
#define __DEPTH_LEVEL_ATTACK {m_iDepthLevel = 9u;}
#define __DEPTH_LEVEL_OVER   {g_pGame->PushDepthLevel();}
#define __DEPTH_RESET        {glDepthRange(0.0f, 1.0f);}


// ****************************************************************
// game class
class cGame final
{
private:
    cPlayer m_aPlayer[GAME_PLAYERS];         // player objects

    cEnemyManager  m_EnemyManager;           // enemy manager
    cBulletManager m_BulletManagerPlayer;    // low-priority bullet manager
    cBulletManager m_BulletManagerEnemy;     // high-priority bullet manager
    cShieldManager m_ShieldManager;          // 

    cCombatStats m_CombatStats;              // combat stats overlay
    cCombatText  m_CombatText;               // combat text overlay
    cInterface   m_Interface;                // interface overlay

    cMission* m_pMission;                    // active mission (should never be NULL)

    coreFlow m_fTimeGame;                    // 
    coreFlow m_fTimeMission;                 // total time in mission
    coreFlow m_afTimeBoss[MISSION_BOSSES];   // total time per boss battle

    coreUint8 m_iDepthLevel;                 // 

    coreUint8 m_iStatus;                     // 
    coreBool  m_bCoop;                       // 


public:
    explicit cGame(const coreBool bCoop)noexcept;
    ~cGame();

    DISABLE_COPY(cGame)

    // render and move the game
    void Render();
    void Move();

    // render the overlay separately
    void RenderOverlay();

    // control active mission
    void LoadMission(const coreInt32 iID);
    void RestartMission();

    // 
    inline void StartIntro() {ADD_VALUE(m_iStatus, GAME_STATUS_INTRO)}
    inline void StartOutro() {ADD_VALUE(m_iStatus, GAME_STATUS_OUTRO) REMOVE_VALUE(m_iStatus, GAME_STATUS_PLAY)}

    // 
    void PushDepthLevel();
    void OffsetDepthLevel(const coreFloat fOffset)const;

    // 
    cPlayer* FindPlayer(const coreVector2& vPosition);
    template <typename F> void ForEachPlayer(F&& nFunction);   // [](cPlayer* OUTPUT pPlayer, const coreUintW i) -> void

    // access game objects
    inline cPlayer*        GetPlayer             (const coreUintW iIndex) {ASSERT(iIndex < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline cEnemyManager*  GetEnemyManager       ()                       {return &m_EnemyManager;}
    inline cBulletManager* GetBulletManagerPlayer()                       {return &m_BulletManagerPlayer;}
    inline cBulletManager* GetBulletManagerEnemy ()                       {return &m_BulletManagerEnemy;}
    inline cShieldManager* GetShieldManager      ()                       {return &m_ShieldManager;}
    inline cCombatStats*   GetCombatStats        ()                       {return &m_CombatStats;}
    inline cCombatText*    GetCombatText         ()                       {return &m_CombatText;}
    inline cInterface*     GetInterface          ()                       {return &m_Interface;}
    inline cMission*       GetMission            ()const                  {ASSERT(m_pMission) return m_pMission;}

    // get object properties
    inline const coreFloat& GetTimeGame   ()const                       {return m_fTimeGame;}
    inline const coreFloat& GetTimeMission()const                       {return m_fTimeMission;}
    inline const coreFloat& GetTimeBoss   (const coreUintW iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_afTimeBoss[iIndex];}
    inline const coreUint8& GetStatus     ()const                       {return m_iStatus;}
    inline const coreBool&  GetCoop       ()const                       {return m_bCoop;}


private:
    // 
    coreBool __HandleIntro();
    coreBool __HandleOutro();
    void     __HandleCollisions();
};


// ****************************************************************
// 
template <typename F> void cGame::ForEachPlayer(F&& nFunction)
{
    // 
    if(!m_bCoop) {nFunction(&m_aPlayer[0], 0u); return;}

    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
    {
        cPlayer* pPlayer = &m_aPlayer[i];
        if(CONTAINS_VALUE(pPlayer->GetStatus(), PLAYER_STATUS_DEAD)) continue;

        // 
        nFunction(pPlayer, i);
    }
}


#endif // _P1_GUARD_GAME_H_