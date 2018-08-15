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
// TODO: use uint for game-times ?
// TODO: check for duplicate IDs in mission-lists ? LoadMissionID may behave wrong


// ****************************************************************
// game definitions
#define GAME_PLAYERS        (PLAYERS)   // default number of players
#define GAME_INTRO_DELAY    (0.2f)      // 
#define GAME_INTRO_DURATION (3.5f)      // 

enum eGameStatus : coreUint8
{
    GAME_STATUS_INTRO   = 0x01u,   // 
    GAME_STATUS_OUTRO   = 0x02u,   // 
    GAME_STATUS_PLAY    = 0x04u,   // 
    GAME_STATUS_LOADING = 0x08u    // 
};


// ****************************************************************
// 
static constexpr coreInt32 __GAME_MISSION_LIST_DEFAULT[] =
{
    cIntroMission  ::ID,
    cViridoMission ::ID,
    cNevoMission   ::ID,
    cHarenaMission ::ID,
    cRutilusMission::ID,
    cGeluMission   ::ID,
    cCalorMission  ::ID,
    cMuscusMission ::ID,
    cAterMission   ::ID
};
#define GAME_MISSION_LIST_DEFAULT (__GAME_MISSION_LIST_DEFAULT), ARRAY_SIZE(__GAME_MISSION_LIST_DEFAULT)


// ****************************************************************
// depth-level management
#define DEPTH_PUSH           {g_pGame->PushDepthLevel();}
#define DEPTH_PUSH_IGNORE    {}
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
    cPlayer m_aPlayer[GAME_PLAYERS];        // player objects

    cEnemyManager  m_EnemyManager;          // enemy manager
    cBulletManager m_BulletManagerPlayer;   // low-priority bullet manager
    cBulletManager m_BulletManagerEnemy;    // high-priority bullet manager
    cChromaManager m_ChromaManager;         // 
    cItemManager   m_ItemManager;           // 
    cShieldManager m_ShieldManager;         // 

    cCombatStats m_CombatStats;             // combat stats overlay
    cCombatText  m_CombatText;              // combat text overlay
    cInterface   m_Interface;               // interface overlay

    const coreInt32* m_piMissionList;       // 
    coreUintW        m_iNumMissions;        // 

    cMission* m_pCurMission;                // currently active mission (should never be NULL)
    coreUintW m_iCurMissionIndex;           // 

    cTimeTable m_TimeTable;                 // 
    coreFlow   m_fTimeInOut;                // 

    coreUint8 m_iDepthLevel;                // 

    coreUint8 m_iStatus;                    // 
    coreUint8 m_iDifficulty;                // 
    coreBool  m_bCoop;                      // 


public:
    cGame(const coreUint8 iDifficulty, const coreBool bCoop, const coreInt32* piMissionList, const coreUintW iNumMissions)noexcept;
    ~cGame();

    DISABLE_COPY(cGame)

    // render and move the game
    void Render();
    void Move();

    // render the overlay separately
    void RenderOverlay();

    // control active mission
    void LoadMissionID   (const coreInt32 iID);
    void LoadMissionIndex(const coreUintW iIndex);
    void LoadNextMission();
    void RestartMission();

    // 
    void StartIntro();
    void StartOutro();

    // 
    void PushDepthLevel();
    void OffsetDepthLevel(const coreFloat fOffset)const;

    // 
    cPlayer* FindPlayer(const coreVector2& vPosition);
    template <typename F> void ForEachPlayer   (F&& nFunction);   // [](cPlayer* OUTPUT pPlayer, const coreUintW i) -> void
    template <typename F> void ForEachPlayerAll(F&& nFunction);   // [](cPlayer* OUTPUT pPlayer, const coreUintW i) -> void

    // access game objects
    inline cPlayer*         GetPlayer             (const coreUintW iIndex) {ASSERT(iIndex < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline cEnemyManager*   GetEnemyManager       ()                       {return &m_EnemyManager;}
    inline cBulletManager*  GetBulletManagerPlayer()                       {return &m_BulletManagerPlayer;}
    inline cBulletManager*  GetBulletManagerEnemy ()                       {return &m_BulletManagerEnemy;}
    inline cChromaManager*  GetChromaManager      ()                       {return &m_ChromaManager;}
    inline cItemManager*    GetItemManager        ()                       {return &m_ItemManager;}
    inline cShieldManager*  GetShieldManager      ()                       {return &m_ShieldManager;}
    inline cCombatStats*    GetCombatStats        ()                       {return &m_CombatStats;}
    inline cCombatText*     GetCombatText         ()                       {return &m_CombatText;}
    inline cInterface*      GetInterface          ()                       {return &m_Interface;}
    inline cMission*        GetCurMission         ()const                  {ASSERT(m_pCurMission) return m_pCurMission;}
    inline const coreUintW& GetCurMissionIndex    ()const                  {return m_iCurMissionIndex;}
    inline cTimeTable*      GetTimeTable          ()                       {return &m_TimeTable;}

    // get object properties
    inline const coreInt32* GetMissionList()const {return m_piMissionList;}
    inline const coreUintW& GetNumMissions()const {return m_iNumMissions;}
    inline const coreUint8& GetStatus     ()const {return m_iStatus;}
    inline const coreUint8& GetDifficulty ()const {return m_iDifficulty;}
    inline const coreBool&  GetCoop       ()const {return m_bCoop;}


private:
    // 
    coreBool __HandleIntro();
    coreBool __HandleOutro();
    void     __HandleDefeat();
    void     __HandleCollisions();
};


// ****************************************************************
// 
template <typename F> void cGame::ForEachPlayer(F&& nFunction)
{
    // 
    for(coreUintW i = 0u, ie = (m_bCoop ? GAME_PLAYERS : 1u); i < ie; ++i)
    {
        cPlayer* pPlayer = &m_aPlayer[i];
        if(CONTAINS_FLAG(pPlayer->GetStatus(), PLAYER_STATUS_DEAD)) continue;

        // 
        nFunction(pPlayer, i);
    }
}


// ****************************************************************
// 
template <typename F> void cGame::ForEachPlayerAll(F&& nFunction)
{
    // 
    for(coreUintW i = 0u, ie = (m_bCoop ? GAME_PLAYERS : 1u); i < ie; ++i)
    {
        // 
        nFunction(&m_aPlayer[i], i);
    }
}


#endif // _P1_GUARD_GAME_H_