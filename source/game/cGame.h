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
#define GAME_PLAYERS        (PLAYERS)   // default number of players
#define GAME_INTRO_DELAY    (0.2f)      // 
#define GAME_INTRO_DURATION (3.5f)      // 

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
    cBulletManager m_BulletManager;          // bullet manager

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
    inline cPlayer*              GetPlayer       (const coreUintW& iIndex) {ASSERT(iIndex < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline std::vector<cEnemy*>* GetEnemyList    ()                        {return &m_apEnemyList;}
    inline cBulletManager*       GetBulletManager()                        {return &m_BulletManager;}
    inline cCombatText*          GetCombatText   ()                        {return &m_CombatText;}
    inline cInterface*           GetInterface    ()                        {return &m_Interface;}
    inline cMission*             GetMission      ()const                   {return m_pMission;}

    // 
    cPlayer* FindPlayer(const coreVector2& vPosition);
    cEnemy*  FindEnemy (const coreVector2& vPosition);

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


#endif // _P1_GUARD_GAME_H_