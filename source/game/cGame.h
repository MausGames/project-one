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
#define GAME_PLAYERS        (PLAYERS)        // default number of players
#define GAME_INTRO_DELAY    (0.2f)           // 
#define GAME_INTRO_DURATION (3.5f)           // 

#define GAME_DEPTH_WEAK     (0.4f), (1.0f)   // 
#define GAME_DEPTH_PLAYER   (0.2f), (1.0f)   // 
#define GAME_DEPTH_STRONG   (0.0f), (0.7f)   // 
#define GAME_DEPTH_ENEMY    (0.0f), (0.5f)   // 

enum cGameStatus : coreUint8
{
    GAME_STATUS_INTRO   = 0x01u,   // 
    GAME_STATUS_PLAY    = 0x02u,   // 
    GAME_STATUS_OUTRO   = 0x03u,   // (two bits) 
    GAME_STATUS_LOADING = 0x04u    // 
};


// ****************************************************************
// game class
class cGame final
{
private:
    cPlayer m_aPlayer[GAME_PLAYERS];         // player objects
    std::vector<cEnemy*> m_apEnemyList;      // list with pointers to active enemy objects

    cBulletManager m_BulletManagerPlayer;    // low-priority bullet manager
    cBulletManager m_BulletManagerEnemy;     // high-priority bullet manager

    cCombatStats m_CombatStats;              // combat stats overlay
    cCombatText  m_CombatText;               // combat text overlay
    cInterface   m_Interface;                // interface overlay

    cMission* m_pMission;                    // active mission (should never be NULL)

    coreFlow m_fTimeMission;                 // total time in mission
    coreFlow m_afTimeBoss[MISSION_BOSSES];   // total time per boss battle

    coreUint8 m_iStatus;                     // 
    coreBool  m_bCoop;                       // 


public:
    explicit cGame(const coreBool& bCoop)noexcept;
    ~cGame();

    FRIEND_CLASS(cEnemy)
    DISABLE_COPY(cGame)

    // render and move the game
    void Render();
    void Move();

    // render the overlay separately
    void RenderOverlay();

    // control active mission
    void LoadMission(const coreInt32& iID);
    void RestartMission();

    // access game objects
    inline cPlayer*              GetPlayer             (const coreUintW& iIndex) {ASSERT(iIndex < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline std::vector<cEnemy*>* GetEnemyList          ()                        {return &m_apEnemyList;}
    inline cBulletManager*       GetBulletManagerPlayer()                        {return &m_BulletManagerPlayer;}
    inline cBulletManager*       GetBulletManagerEnemy ()                        {return &m_BulletManagerEnemy;}
    inline cCombatStats*         GetCombatStats        ()                        {return &m_CombatStats;}
    inline cCombatText*          GetCombatText         ()                        {return &m_CombatText;}
    inline cInterface*           GetInterface          ()                        {return &m_Interface;}
    inline cMission*             GetMission            ()const                   {ASSERT(m_pMission) return m_pMission;}

    // 
    cPlayer* RETURN_NONNULL FindPlayer(const coreVector2& vPosition);
    cEnemy*  RETURN_NONNULL FindEnemy (const coreVector2& vPosition);
    template <typename F> void ForEachPlayer(F&& nFunction);

    // get object properties
    inline const coreFloat& GetTimeMission()const                        {return m_fTimeMission;}
    inline const coreFloat& GetTimeBoss   (const coreUintW& iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_afTimeBoss[iIndex];}
    inline const coreUint8& GetStatus     ()const                        {return m_iStatus;}
    inline const coreBool&  GetCoop       ()const                        {return m_bCoop;}


private:
    // 
    coreBool __HandleIntro();
    void     __HandleCollisions();

    // manage enemies
    inline void __BindEnemy  (cEnemy* pEnemy) {ASSERT(!CONTAINS(m_apEnemyList, pEnemy)) m_apEnemyList.push_back(pEnemy);}
    inline void __UnbindEnemy(cEnemy* pEnemy) {FOR_EACH(it, m_apEnemyList) {if((*it) == pEnemy) {m_apEnemyList.erase(it); return;}} ASSERT(false)}
};


// ****************************************************************
// 
template <typename F> void cGame::ForEachPlayer(F&& nFunction)
{
    // 
    for(coreUintW i = 0u; i < GAME_PLAYERS; ++i)
    {
        cPlayer* pCurPlayer = &m_aPlayer[i];
        if(CONTAINS_VALUE(pCurPlayer->GetStatus(), PLAYER_STATUS_DEAD)) continue;

        // 
        nFunction(pCurPlayer);
    }
}


#endif // _P1_GUARD_GAME_H_