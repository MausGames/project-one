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
#define GAME_PLAYERS    (PLAYERS)   // default number of players
#define GAME_INTRO_TIME (4.0f)      // 

enum cGameStatus : coreByte
{
    GAME_STATUS_INTRO = 0x01,   // 
    GAME_STATUS_PLAY  = 0x02,   // 
    GAME_STATUS_OUTRO = 0x03    // (two bits) 
};


// ****************************************************************
// game class
class cGame final
{
private:
    cPlayer m_aPlayer[GAME_PLAYERS];         // player objects
    std::vector<cEnemy*> m_apEnemyList;      // list with pointers to active enemy objects
    cBulletManager m_BulletManager;          // bullet manager

    cCombatText m_CombatText;                // combat text overlay
    cInterface  m_Interface;                 // interface overlay

    cMission* m_pMission;                    // active mission (should never be NULL)

    coreFlow m_fTimeMission;                 // total time in mission
    coreFlow m_afTimeBoss[MISSION_BOSSES];   // total time per boss battle

    coreByte m_iStatus;                      // 
    bool m_bCoop;                            // 


public:
    cGame(const bool& bCoop)noexcept;
    ~cGame();

    FRIEND_CLASS(cEnemy)
    DISABLE_COPY(cGame)

    // render and move the game
    void Render();
    void Move();

    // render the overlay separately
    void RenderOverlay();

    // control active mission
    void LoadMission(const int& iID);
    void RestartMission();

    // access game objects
    inline cPlayer*              GetPlayer       (const coreByte& iIndex) {ASSERT(iIndex < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline std::vector<cEnemy*>* GetEnemyList    ()                       {return &m_apEnemyList;}
    inline cBulletManager*       GetBulletManager()                       {return &m_BulletManager;}
    inline cCombatText*          GetCombatText   ()                       {return &m_CombatText;}
    inline cInterface*           GetInterface    ()                       {return &m_Interface;}
    inline cMission*             GetMission      ()const                  {return m_pMission;}

    // get object properties
    inline const float&    GetTimeMission()const                       {return m_fTimeMission;}
    inline const float&    GetTimeBoss   (const coreByte& iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_afTimeBoss[iIndex];}
    inline const coreByte& GetStatus     ()const                       {return m_iStatus;}
    inline const bool&     GetCoop       ()const                       {return m_bCoop;}


private:
    // handle default object collisions
    void __HandleCollisions();

    // manage enemies
    inline void __BindEnemy  (cEnemy* pEnemy) {ASSERT(!CONTAINS(m_apEnemyList, pEnemy)) m_apEnemyList.push_back(pEnemy);}
    inline void __UnbindEnemy(cEnemy* pEnemy) {FOR_EACH(it, m_apEnemyList) {if((*it) == pEnemy) {m_apEnemyList.erase(it); return;}} ASSERT(false)}
};


#endif // _P1_GUARD_GAME_H_