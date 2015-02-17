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
#define GAME_PLAYERS (2u)   // default number of players


// ****************************************************************
// game class
class cGame final
{
private:
    cPlayer m_aPlayer[GAME_PLAYERS];      // player objects
    std::vector<cEnemy*> m_apEnemyList;   // list with pointers to active enemies

    cBulletManager m_BulletManager;       // bullet manager

    cCombatText m_CombatText;             // 
    cInterface  m_Interface;              // 

    cMission* m_pMission;                 //  (should never be NULL) 


public:
    cGame()noexcept;
    ~cGame();

    FRIEND_CLASS(cEnemy)
    DISABLE_COPY(cGame)

    // render and move the game
    void Render();
    void Move();

    // 
    void RenderOverlay();

    // 
    void LoadMission(const int& iID);

    // access game objects
    inline cPlayer*              GetPlayer       (const coreByte& iIndex) {ASSERT(iIndex < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline std::vector<cEnemy*>* GetEnemyList    ()                       {return &m_apEnemyList;}
    inline cBulletManager*       GetBulletManager()                       {return &m_BulletManager;}
    inline cCombatText*          GetCombatText   ()                       {return &m_CombatText;}
    inline cInterface*           GetInterface    ()                       {return &m_Interface;}


private:
    // manage enemies
    inline void __BindEnemy  (cEnemy* pEnemy) {ASSERT(!CONTAINS(m_apEnemyList, pEnemy)) m_apEnemyList.push_back(pEnemy);}
    inline void __UnbindEnemy(cEnemy* pEnemy) {FOR_EACH(it, m_apEnemyList) {if((*it) == pEnemy) {m_apEnemyList.erase(it); return;}} ASSERT(false)}
};


#endif // _P1_GUARD_GAME_H_