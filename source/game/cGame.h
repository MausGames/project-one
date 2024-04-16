///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_GAME_H_
#define _P1_GUARD_GAME_H_

// TODO 3: enemy bullet (and enemy?) cleanup on mission unload (probably not really necessary, can be re-used for next mission)
// TODO 5: maybe spawn players in flight direction, mission start and continue ?
// TODO 2: FindPlayer may find player outside of area (during resurrection)
// TODO 2: removing ghost status (player, enemy, bullet) should reset firsthit property on collision somehow (but needs separation between sub-ghost states, maybe add own collision-tracker and merge with the one in player-class)
// TODO 3: repair enemy only in coop ? (not duel)
// TODO 3: temporarily changing to no-background (on mission start) unloads all resources, even though we were just on that background (caching old background ?)


// ****************************************************************
// game definitions
#define GAME_PLAYERS        (PLAYERS)     // default number of players
#define GAME_HELPERS        (HELPERS)     // 
#define GAME_CONTINUES      (CONTINUES)   // 
#define GAME_INTRO_OFFSET   (0.4f)        // 
#define GAME_INTRO_DURATION (4.1f)        // 

enum eGameStatus : coreUint8
{
    GAME_STATUS_INTRO    = 0x01u,   // 
    GAME_STATUS_OUTRO    = 0x02u,   // 
    GAME_STATUS_PLAY     = 0x04u,   // 
    GAME_STATUS_LOADING  = 0x08u,   // 
    GAME_STATUS_DEFEATED = 0x10u,   // 
    GAME_STATUS_FINISHED = 0x20u    // 
};

enum eGameType : coreUint8
{
    GAME_TYPE_SOLO = 0u,   // (opposite of MULTI) 
    GAME_TYPE_COOP,        // 
    GAME_TYPE_DUEL,        // 
    GAME_TYPE_MAX          // 
};

enum eGameMode : coreUint8
{
    GAME_MODE_STANDARD = 0u,   // 
    GAME_MODE_PACIFIST,        // 
    GAME_MODE_MASOCHIST,       // 
    GAME_MODE_MAX              // 
};

enum eGameDifficulty : coreUint8
{
    GAME_DIFFICULTY_EASY = 0u,   // 
    GAME_DIFFICULTY_NORMAL,      // 
    GAME_DIFFICULTY_HARD,        // 
    GAME_DIFFICULTY_MAX          // 
};

struct sGameOptions final
{
    coreUint8 iType;                                             // 
    coreUint8 iMode;                                             // 
    coreUint8 iDifficulty;                                       // 
    coreUint8 aaiWeapon [GAME_PLAYERS][PLAYER_EQUIP_WEAPONS];    // 
    coreUint8 aaiSupport[GAME_PLAYERS][PLAYER_EQUIP_SUPPORTS];   // 
};


// ****************************************************************
// 
static constexpr coreInt32 __GAME_MISSION_LIST_MAIN[] =
{
    cIntroMission  ::ID,
    cViridoMission ::ID,
    cNevoMission   ::ID,
    cHarenaMission ::ID,
    cRutilusMission::ID,
    cGeluMission   ::ID,
    cCalorMission  ::ID,
    cMuscusMission ::ID,
    cAterMission   ::ID,
    cBonus1Mission ::ID,
    cBonus2Mission ::ID,
    cErrorMission  ::ID
};

static constexpr coreInt32 __GAME_MISSION_LIST_DEMO[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    cDemoMission::ID
};

#define GAME_MISSION_LIST_MAIN (__GAME_MISSION_LIST_MAIN), ARRAY_SIZE(__GAME_MISSION_LIST_MAIN)
#define GAME_MISSION_LIST_DEMO (__GAME_MISSION_LIST_DEMO), ARRAY_SIZE(__GAME_MISSION_LIST_DEMO)

struct sMissionData final
{
    coreInt32       iID;      // 
    const coreChar* pcName;   // 
    coreVector3     vColor;   // 
};

static constexpr sMissionData g_aMissionData[] =
{
    {cIntroMission  ::ID, cIntroMission  ::Name, cCloudBackground  ::Color},
    {cViridoMission ::ID, cViridoMission ::Name, cGrassBackground  ::Color},
    {cNevoMission   ::ID, cNevoMission   ::Name, cSeaBackground    ::Color},
    {cHarenaMission ::ID, cHarenaMission ::Name, cDesertBackground ::Color},
    {cRutilusMission::ID, cRutilusMission::Name, cSpaceBackground  ::Color},
    {cGeluMission   ::ID, cGeluMission   ::Name, cVolcanoBackground::Color},
    {cCalorMission  ::ID, cCalorMission  ::Name, cSnowBackground   ::Color},
    {cMuscusMission ::ID, cMuscusMission ::Name, cMossBackground   ::Color},
    {cAterMission   ::ID, cAterMission   ::Name, cDarkBackground   ::Color},
    {cBonus1Mission ::ID, cBonus1Mission ::Name, cDarkBackground   ::Color},   // #
    {cBonus2Mission ::ID, cBonus2Mission ::Name, cDarkBackground   ::Color}    // #
};
// TODO 1: handle game end based on mission-list


// ****************************************************************
// depth-level management
#define DEPTH_PUSH           {g_pGame->PushDepthLevel(1u);}
#define DEPTH_PUSH_DOUBLE    {g_pGame->PushDepthLevel(2u);}
#define DEPTH_PUSH_SHIP      {g_pGame->PushDepthLevelShip();}

#define __DEPTH_GROUP_SHIP   {this->ChangeDepthLevel(9u, 11u);}                // 2 levels, static
#define __DEPTH_GROUP_BOTTOM {m_iDepthDebug = 11u;     m_iDepthLevel = 20u;}   // 9 levels, dynamic, 2 in game
#define __DEPTH_GROUP_UNDER  {}                                                // shared with DEPTH_GROUP_BOTTOM
#define __DEPTH_GROUP_OVER   {m_iDepthDebug = BIT(7u); m_iDepthLevel = 9u;}    // 9 levels, dynamic, 3 in game (can use DEPTH_PUSH_SHIP)
#define __DEPTH_GROUP_TOP    {m_iDepthDebug = 0u;}                             // shared with DEPTH_GROUP_OVER
#define __DEPTH_RESET        {this->ChangeDepthLevel(0u, 20u);}


// ****************************************************************
// game class
class cGame final
{
private:
    cPlayer m_aPlayer[GAME_PLAYERS];        // player objects
    cHelper m_aHelper[GAME_HELPERS];        // 

    cEnemyManager  m_EnemyManager;          // enemy manager
    cBulletManager m_BulletManagerPlayer;   // low-priority bullet manager
    cBulletManager m_BulletManagerEnemy;    // high-priority bullet manager
    cItemManager   m_ItemManager;           // 
    cShieldManager m_ShieldManager;         // 
    cCrashManager  m_CrashManager;          // 

    cInterface  m_Interface;                // interface overlay
    cCombatText m_CombatText;               // combat text overlay

    cRepairEnemy* m_pRepairEnemy;           // 

    const coreInt32* m_piMissionList;       // (should never be NULL) 
    coreUintW        m_iNumMissions;        // 

    cMission* m_pCurMission;                // currently active mission (should never be NULL)
    coreUintW m_iCurMissionIndex;           // 

    cTimeTable m_TimeTable;                 // 
    coreFlow   m_fTimeInOut;                // 

    coreUint8 m_iContinues;                 // 

    coreUint8 m_iDepthLevel;                // 
    coreUint8 m_iDepthDebug;                // 

    coreUint8 m_iOutroType;                 // 

    coreBool m_bVisibleCheck;               // 

    sGameOptions m_Options;                 // 
    coreUint16   m_iVersion;                // 

    coreUint8 m_iStatus;                    // 


public:
    cGame(const sGameOptions oOptions, const coreInt32* piMissionList, const coreUintW iNumMissions)noexcept;
    ~cGame();

    DISABLE_COPY(cGame)

    // render and move the game
    void Render();
    void Move();

    // render and move the overlay separately
    void RenderOverlay();
    void MoveOverlay();

    // control active mission
    void LoadMissionID   (const coreInt32 iID,    const coreUint8 iTakeFrom = 0u, const coreUint8 iTakeTo = TAKE_MISSION);
    void LoadMissionIndex(const coreUintW iIndex, const coreUint8 iTakeFrom = 0u, const coreUint8 iTakeTo = TAKE_MISSION);
    void LoadNextMission();
    void RestartMission();

    // 
    void StartIntro();
    void StartOutro(const coreUint8 iType);

    // 
    void UseContinue();

    // 
    void ChangeDepthLevel  (const coreUint8 iLevelNear, const coreUint8 iLevelFar)const;
    void PushDepthLevel    (const coreUint8 iLevels);
    void PushDepthLevelShip();
    //void PopDepthLevel    (const coreUint8 iLevels);

    // 
    inline void SetVisibleCheck(const coreBool bCheck) {m_bVisibleCheck = bCheck;}

    // 
    inline void HideHelpers    () {for(coreUintW i = 0u; i < GAME_HELPERS; ++i) if(m_aHelper[i].HasStatus(HELPER_STATUS_DEAD)) m_aHelper[i].SetPosition(coreVector3(HIDDEN_POS, 0.0f));}
    inline void KillRepairEnemy() {if(m_pRepairEnemy) m_pRepairEnemy->TakeDamage(m_pRepairEnemy->GetCurHealth(), ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f), NULL);}

    // 
    RETURN_NONNULL cPlayer* FindPlayerSide(const coreVector2 vPosition);
    RETURN_NONNULL cPlayer* FindPlayerDual(const coreUintW   iIndex);
    template <typename F> void ForEachPlayer   (F&& nFunction);   // [](cPlayer* OUTPUT pPlayer, const coreUintW i) -> void
    template <typename F> void ForEachPlayerAll(F&& nFunction);   // [](cPlayer* OUTPUT pPlayer, const coreUintW i) -> void

    // 
    inline coreBool IsMulti    ()const                          {return (this->GetType      () != GAME_TYPE_SOLO);}
    inline coreBool IsCoop     ()const                          {return (this->GetType      () == GAME_TYPE_COOP);}
    inline coreBool IsDuel     ()const                          {return (this->GetType      () == GAME_TYPE_DUEL);}
    inline coreBool IsPacifist ()const                          {return (this->GetMode      () == GAME_MODE_PACIFIST);}
    inline coreBool IsMasochist()const                          {return (this->GetMode      () == GAME_MODE_MASOCHIST);}
    inline coreBool IsEasy     ()const                          {return (this->GetDifficulty() == GAME_DIFFICULTY_EASY);}
    inline coreBool IsHard     ()const                          {return (this->GetDifficulty() == GAME_DIFFICULTY_HARD);}
    inline coreBool IsVersion  (const coreUint16 iVersion)const {return (this->GetVersion   () >= iVersion);}

    // access game objects
    inline cPlayer*         GetPlayer             (const coreUintW iIndex)   {ASSERT(iIndex                   < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline cHelper*         GetHelper             (const coreUint8 iElement) {ASSERT(iElement - ELEMENT_WHITE < GAME_HELPERS) return &m_aHelper[iElement - ELEMENT_WHITE];}
    inline cEnemyManager*   GetEnemyManager       ()                         {return &m_EnemyManager;}
    inline cBulletManager*  GetBulletManagerPlayer()                         {return &m_BulletManagerPlayer;}
    inline cBulletManager*  GetBulletManagerEnemy ()                         {return &m_BulletManagerEnemy;}
    inline cItemManager*    GetItemManager        ()                         {return &m_ItemManager;}
    inline cShieldManager*  GetShieldManager      ()                         {return &m_ShieldManager;}
    inline cCrashManager*   GetCrashManager       ()                         {return &m_CrashManager;}
    inline cInterface*      GetInterface          ()                         {return &m_Interface;}
    inline cCombatText*     GetCombatText         ()                         {return &m_CombatText;}
    inline cMission*        GetCurMission         ()const                    {ASSERT(m_pCurMission) return m_pCurMission;}
    inline const coreUintW& GetCurMissionIndex    ()const                    {return m_iCurMissionIndex;}
    inline cTimeTable*      GetTimeTable          ()                         {return &m_TimeTable;}

    // get object properties
    inline       coreUint8     GetNumPlayers ()const {return this->IsMulti() ? GAME_PLAYERS : 1u;}
    inline const coreInt32*    GetMissionList()const {ASSERT(m_piMissionList) return m_piMissionList;}
    inline const coreUintW&    GetNumMissions()const {return m_iNumMissions;}
    inline const coreUint8&    GetContinues  ()const {return m_iContinues;}
    inline const coreUint8&    GetOutroType  ()const {return m_iOutroType;}
    inline const sGameOptions& GetOptions    ()const {return m_Options;}
    inline const coreUint8&    GetType       ()const {return m_Options.iType;}
    inline const coreUint8&    GetMode       ()const {return m_Options.iMode;}
    inline const coreUint8&    GetDifficulty ()const {return m_Options.iDifficulty;}
    inline const coreUint16&   GetVersion    ()const {return m_iVersion;}
    inline const coreUint8&    GetStatus     ()const {return m_iStatus;}

    // 
    static coreUint8  CalcMedal       (const coreFloat fTime, const coreFloat* pfMedalGoal);
    static coreFloat  CalcMedalTime   (const coreFloat fTime, const coreFloat* pfMedalGoal);
    static coreUint32 CalcBonusTime   (const coreFloat fTime);
    static coreUint32 CalcBonusMedal  (const coreUint8 iMedal);
    static coreUint32 CalcBonusBadge  (const coreUint8 iBadge);
    static coreUint32 CalcBonusSurvive(const coreUint32 iDamageTaken, const coreBool bWasDead);


private:
    // 
    coreBool __HandleIntro();
    coreBool __HandleOutro();
    void     __HandleDefeat();
    void     __HandleCollisions();

    // 
    void __ClearAll(const coreBool bAnimated);
};


// ****************************************************************
// 
template <typename F> void cGame::ForEachPlayer(F&& nFunction)
{
    // 
    for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
    {
        cPlayer* pPlayer = &m_aPlayer[i];
        if(pPlayer->HasStatus(PLAYER_STATUS_DEAD)) continue;

        // 
        nFunction(pPlayer, i);
    }
}


// ****************************************************************
// 
template <typename F> void cGame::ForEachPlayerAll(F&& nFunction)
{
    // 
    for(coreUintW i = 0u, ie = this->GetNumPlayers(); i < ie; ++i)
    {
        // 
        nFunction(&m_aPlayer[i], i);
    }
}


#endif // _P1_GUARD_GAME_H_