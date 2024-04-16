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
// TODO 2: [MF] removing ghost status (player, enemy, bullet) should reset firsthit property on collision somehow (maybe add own collision-tracker and merge with the one in player-class)
// TODO 3: repair enemy only in coop ? (not duel)


// ****************************************************************
// game definitions
#define GAME_PLAYERS        (PLAYERS)     // default number of players
#define GAME_HELPERS        (HELPERS)     // 
#define GAME_CONTINUES      (CONTINUES)   // 
#define GAME_INTRO_OFFSET   (0.4f)        // 
#define GAME_INTRO_DURATION (4.1f)        // 

enum eGameStatus : coreUint16
{
    GAME_STATUS_INTRO    = 0x0001u,   // 
    GAME_STATUS_OUTRO    = 0x0002u,   // 
    GAME_STATUS_PLAY     = 0x0004u,   // 
    GAME_STATUS_LOADING  = 0x0008u,   // 
    GAME_STATUS_DEFEATED = 0x0010u,   // 
    GAME_STATUS_CONTINUE = 0x0020u,   // 
    GAME_STATUS_QUICK    = 0x0040u,   // 
    GAME_STATUS_NAMELESS = 0x0080u,   // 
    GAME_STATUS_FINISHED = 0x0100u    // 
};

enum eGameOutro : coreUint8
{
    GAME_OUTRO_MISSION = 0u,    // 
    GAME_OUTRO_SEGMENT,         // 
    GAME_OUTRO_BEGINNING,       // 
    GAME_OUTRO_ENDING_NORMAL,   // 
    GAME_OUTRO_ENDING_SECRET    // 
};

enum eGameKind : coreUint8
{
    GAME_KIND_ALL = 0u,   // 
    GAME_KIND_MISSION,    // 
    GAME_KIND_SEGMENT,    // 
    GAME_KIND_MAX         // 
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

enum eGameFlag : coreUint8
{
    GAME_FLAG_TASK       = 0x01u,   // 
    GAME_FLAG_TASK_EXTRA = 0x02u,   // 
    GAME_FLAG_FRAGMENT   = 0x04u    // 
};

struct sGameOptions final
{
    coreUint8 iKind;                                             // 
    coreUint8 iType;                                             // 
    coreUint8 iMode;                                             // 
    coreUint8 iDifficulty;                                       // 
    coreUint8 iFlags;                                            // 
    coreUint8 aiShield  [GAME_PLAYERS];                          // 
    coreUint8 aaiWeapon [GAME_PLAYERS][PLAYER_EQUIP_WEAPONS];    // 
    coreUint8 aaiSupport[GAME_PLAYERS][PLAYER_EQUIP_SUPPORTS];   // 
};


// ****************************************************************
// 
static constexpr coreInt32 GAME_MISSION_LIST_MAIN[] =
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

static constexpr coreInt32 GAME_MISSION_LIST_DEMO[] =
{
    cIntroMission ::ID,
    cViridoMission::ID,
    cNevoMission  ::ID
};

struct sMissionData final
{
    coreInt32       iID;       // 
    const coreChar* pcName;    // 
    coreVector3     vColor;    // 
    coreVector3     vColor2;   // 
    coreVector2     vIcon;     // 
    const coreChar* pcBoss;    // 
};

static constexpr sMissionData g_aMissionData[] =
{
    {cIntroMission  ::ID, cIntroMission  ::Name, cCloudBackground  ::Color, cCloudBackground  ::Color2, cCloudBackground  ::Icon, cIntroBoss     ::Name},
    {cViridoMission ::ID, cViridoMission ::Name, cGrassBackground  ::Color, cGrassBackground  ::Color2, cGrassBackground  ::Icon, cTorusBoss     ::Name},
    {cNevoMission   ::ID, cNevoMission   ::Name, cSeaBackground    ::Color, cSeaBackground    ::Color2, cSeaBackground    ::Icon, cLeviathanBoss ::Name},
    {cHarenaMission ::ID, cHarenaMission ::Name, cDesertBackground ::Color, cDesertBackground ::Color2, cDesertBackground ::Icon, cTigerBoss     ::Name},
    {cRutilusMission::ID, cRutilusMission::Name, cSpaceBackground  ::Color, cSpaceBackground  ::Color2, cSpaceBackground  ::Icon, cMessierBoss   ::Name},
    {cGeluMission   ::ID, cGeluMission   ::Name, cVolcanoBackground::Color, cVolcanoBackground::Color2, cVolcanoBackground::Icon, cCholBoss      ::Name},
    {cCalorMission  ::ID, cCalorMission  ::Name, cSnowBackground   ::Color, cSnowBackground   ::Color2, cSnowBackground   ::Icon, cZerothBoss    ::Name},
    {cMuscusMission ::ID, cMuscusMission ::Name, cMossBackground   ::Color, cMossBackground   ::Color2, cMossBackground   ::Icon, cGemingaBoss   ::Name},
    {cAterMission   ::ID, cAterMission   ::Name, cDarkBackground   ::Color, cDarkBackground   ::Color2, cDarkBackground   ::Icon, cProjectOneBoss::Name},
    {cBonus1Mission ::ID, cBonus1Mission ::Name, cDarkBackground   ::Color, cDarkBackground   ::Color2, cDarkBackground   ::Icon, cVausBoss      ::Name},   // #
    {cBonus2Mission ::ID, cBonus2Mission ::Name, cDarkBackground   ::Color, cDarkBackground   ::Color2, cDarkBackground   ::Icon, cNautilusBoss  ::Name}    // #
};


// ****************************************************************
// 
struct sFragmentData final
{
    coreUintW   iIndex;    // 
    coreVector2 vOffset;   // 
    coreVector2 vShift;    // 
    coreVector2 vSize;     // 
    coreVector3 vColor;    // 
};

static constexpr sFragmentData g_aFragmentData[] =
{
    {1u, coreVector2(  0.0f, 1024.0f -  516.0f) / 1024.0f, coreVector2( 0.01f, 0.0f),   coreVector2(452.0f,517.0f) / 1024.0f, cCloudBackground  ::Color2 * 0.9f},
    {7u, coreVector2(378.0f, 1024.0f - 1023.0f) / 1024.0f, coreVector2( 0.0f,  0.01f),  coreVector2(648.0f,329.0f) / 1024.0f, cGrassBackground  ::Color2 * 0.9f},
    {3u, coreVector2(638.0f, 1024.0f -  328.0f) / 1024.0f, coreVector2( 0.0f, -0.005f), coreVector2(388.0f,329.0f) / 1024.0f, cSeaBackground    ::Color2 * 0.9f},
    {4u, coreVector2(  0.0f, 1024.0f - 1023.0f) / 1024.0f, coreVector2( 0.0f,  0.02f),  coreVector2(372.0f,370.0f) / 1024.0f, cDesertBackground ::Color2 * 0.9f},
    {6u, coreVector2(439.0f, 1024.0f -  783.0f) / 1024.0f, coreVector2( 0.0f,  0.0f),   coreVector2(588.0f,533.0f) / 1024.0f, cSpaceBackground  ::Color2 * 0.9f},
    {2u, coreVector2(252.0f, 1024.0f -  453.0f) / 1024.0f, coreVector2( 0.0f,  0.005f), coreVector2(524.0f,454.0f) / 1024.0f, cVolcanoBackground::Color2 * 0.9f},
    {5u, coreVector2(  0.0f, 1024.0f - 1023.0f) / 1024.0f, coreVector2( 0.0f, -0.01f),  coreVector2(520.0f,647.0f) / 1024.0f, cSnowBackground   ::Color2 * 0.9f},
    {8u, coreVector2(760.0f, 1024.0f -  837.0f) / 1024.0f, coreVector2(-0.01f, 0.0f),   coreVector2(264.0f,369.0f) / 1024.0f, cMossBackground   ::Color2 * 0.9f},
    {9u, coreVector2(250.0f, 1024.0f -  773.0f) / 1024.0f, coreVector2( 0.0f,  0.0f),   coreVector2(524.0f,524.0f) / 1024.0f, cDarkBackground   ::Color2 * 0.9f},
    {0u, coreVector2(0.0f,0.0f),                           coreVector2( 0.0f,  0.0f),   coreVector2(0.0f,0.0f),               cDarkBackground   ::Color2 * 0.9f},   // #
    {0u, coreVector2(0.0f,0.0f),                           coreVector2( 0.0f,  0.0f),   coreVector2(0.0f,0.0f),               cDarkBackground   ::Color2 * 0.9f}    // #
};

#define FRAGMENT_POSITION(x) (g_aFragmentData[x].vOffset + g_aFragmentData[x].vSize * 0.5f - 0.5f)
#define FRAGMENT_DIRECTION   (coreVector2(1.0f,1.0f).Normalized())

STATIC_ASSERT(ARRAY_SIZE(g_aFragmentData) >= FRAGMENTS)
STATIC_ASSERT(ARRAY_SIZE(g_aFragmentData) == ARRAY_SIZE(g_aMissionData))


// ****************************************************************
// depth-level management
#define DEPTH_PUSH           {g_pGame->PushDepthLevel(1u);}
#define DEPTH_PUSH_DOUBLE    {g_pGame->PushDepthLevel(2u);}
#define DEPTH_PUSH_SHIP      {g_pGame->PushDepthLevelShip();}
#define DEPTH_POP            {g_pGame->PopDepthLevel(1u);}

#define __DEPTH_GROUP_SHIP   {this->ChangeDepthLevel(8u, 10u);}                // 2 levels, static
#define __DEPTH_GROUP_BOTTOM {m_iDepthDebug = 10u;     m_iDepthLevel = 20u;}   // 10 levels, dynamic, 3 in game
#define __DEPTH_GROUP_UNDER  {}                                                // shared with DEPTH_GROUP_BOTTOM
#define __DEPTH_GROUP_OVER   {m_iDepthDebug = BIT(7u); m_iDepthLevel = 8u;}    // 8 levels, dynamic, 3 in game (can use DEPTH_PUSH_SHIP)
#define __DEPTH_GROUP_TOP    {m_iDepthDebug = 0u;}                             // shared with DEPTH_GROUP_OVER
#define __DEPTH_RESET        {this->ChangeDepthLevel(0u, 20u);}


// ****************************************************************
// game class
class cGame final
{
private:
    cPlayer  m_aPlayer[GAME_PLAYERS];        // player objects
    cHelper  m_aHelper[GAME_HELPERS];        // 
    cTracker m_Tracker;                      // 

    cEnemyManager   m_EnemyManager;          // enemy manager
    cBulletManager  m_BulletManagerPlayer;   // low-priority bullet manager
    cBulletManager  m_BulletManagerEnemy;    // high-priority bullet manager
    cItemManager    m_ItemManager;           // 
    cShieldManager  m_ShieldManager;         // 
    cCrashManager   m_CrashManager;          // 
    cExhaustManager m_ExhaustManager;        // 
    
    cBulletManager m_BulletManagerPlayerTop;

    cInterface  m_Interface;                // interface overlay
    cCombatText m_CombatText;               // combat text overlay

    cRepairEnemy* m_pRepairEnemy;           // 

    const coreInt32* m_piMissionList;       // (should never be NULL) 
    coreUintW        m_iNumMissions;        // 

    cMission* m_pCurMission;                // currently active mission (should never be NULL)
    coreUintW m_iCurMissionIndex;           // 

    cTimeTable m_TimeTable;                 // 
    coreFlow   m_fTimeInOut;                // 

    coreFlow  m_fMusicFade;                 // 
    coreFloat m_fMusicSpeed;                // 
    coreFloat m_fMusicVolume;               // 
    
    coreFlow m_fHitDelay;
    coreBool m_bDefeatDelay;                // 

    coreUint8 m_iContinues;                 // 

    coreUint8 m_iDepthLevel;                // 
    coreUint8 m_iDepthDebug;                // 

    coreUint8 m_iOutroType;                 // 

    coreBool  m_bVisibleCheck;              // 
    coreUint8 m_iRepairMove;                // 

    sGameOptions m_Options;                 // 
    coreUint16   m_iVersion;                // 

    coreUint16 m_iStatus;                   // 


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

    // 
    void MoveAlways();

    // control active mission
    void LoadMissionID   (const coreInt32 iID,    const coreUint8 iTakeFrom = 0u, const coreUint8 iTakeTo = TAKE_MISSION);
    void LoadMissionIndex(const coreUintW iIndex, const coreUint8 iTakeFrom = 0u, const coreUint8 iTakeTo = TAKE_MISSION);
    void LoadNextMission();

    // 
    void StartIntro();
    void StartOutro(const coreUint8 iType);

    // 
    void FadeMusic(const coreFloat fSpeed);
    inline void SetMusicVolume(const coreFloat fVolume) {m_fMusicVolume = fVolume;}

    // 
    void PlayHitSound    (const coreVector3 vPosition);
    void PlayReflectSound(const coreVector3 vPosition);

    // 
    void UseContinue();
    void UseNext();
    void UseRestart();

    // 
    void DisableMissionName();

    // 
    void RepairPlayer();

    // 
    void ChangeDepthLevel  (const coreUint8 iLevelNear, const coreUint8 iLevelFar)const;
    void PushDepthLevel    (const coreUint8 iLevels);
    void PushDepthLevelShip();
    void PopDepthLevel    (const coreUint8 iLevels);

    // 
    coreVector3 CalculateCamShift()const;

    // 
    inline void SetVisibleCheck(const coreBool bCheck)                        {m_bVisibleCheck = bCheck;}
    inline void SetRepairMove  (const coreUintW iIndex, const coreBool bMove) {ASSERT(iIndex < GAME_PLAYERS) SET_BIT(m_iRepairMove, iIndex, bMove)}
    inline coreBool HasRepairMove(const coreUintW iIndex) {ASSERT(iIndex < GAME_PLAYERS) return HAS_BIT(m_iRepairMove, iIndex);}

    // 
    inline void HideHelpers    () {for(coreUintW i = 0u; i < GAME_HELPERS; ++i) if(m_aHelper[i].HasStatus(HELPER_STATUS_DEAD)) m_aHelper[i].SetPosition(coreVector3(HIDDEN_POS, 0.0f));}
    inline void KillHelpers    () {for(coreUintW i = 0u; i < GAME_HELPERS; ++i) m_aHelper[i].Kill(false);}
    //inline void KillRepairEnemy() {if(m_pRepairEnemy) m_pRepairEnemy->TakeDamage(m_pRepairEnemy->GetCurHealth(), ELEMENT_NEUTRAL, coreVector2(0.0f,0.0f), NULL);}

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
    inline coreBool IsTask     ()const                          {return HAS_FLAG(this->GetFlags(), GAME_FLAG_TASK);}
    inline coreBool IsTaskExtra()const                          {return HAS_FLAG(this->GetFlags(), GAME_FLAG_TASK_EXTRA);}
    inline coreBool IsFragment ()const                          {return HAS_FLAG(this->GetFlags(), GAME_FLAG_FRAGMENT);}
    inline coreBool IsVersion  (const coreUint16 iVersion)const {return (this->GetVersion   () >= iVersion);}
    
    inline coreUintW GetPlayerIndex(const cPlayer* pPlayer)const {ASSERT(pPlayer) return (pPlayer - &m_aPlayer[0]);}

    // access game objects
    inline cPlayer*         GetPlayer             (const coreUintW iIndex)   {ASSERT(iIndex                   < GAME_PLAYERS) return &m_aPlayer[iIndex];}
    inline cHelper*         GetHelper             (const coreUint8 iElement) {ASSERT(iElement - ELEMENT_WHITE < GAME_HELPERS) return &m_aHelper[iElement - ELEMENT_WHITE];}
    inline cTracker*        GetTracker            ()                         {return &m_Tracker;}
    inline cEnemyManager*   GetEnemyManager       ()                         {return &m_EnemyManager;}
    inline cBulletManager*  GetBulletManagerPlayer()                         {return &m_BulletManagerPlayer;}
    inline cBulletManager*  GetBulletManagerEnemy ()                         {return &m_BulletManagerEnemy;}
    inline cItemManager*    GetItemManager        ()                         {return &m_ItemManager;}
    inline cShieldManager*  GetShieldManager      ()                         {return &m_ShieldManager;}
    inline cCrashManager*   GetCrashManager       ()                         {return &m_CrashManager;}
    inline cExhaustManager* GetExhaustManager     ()                         {return &m_ExhaustManager;}
    inline cInterface*      GetInterface          ()                         {return &m_Interface;}
    inline cCombatText*     GetCombatText         ()                         {return &m_CombatText;}
    inline cMission*        GetCurMission         ()const                    {ASSERT(m_pCurMission) return m_pCurMission;}
    inline const coreUintW& GetCurMissionIndex    ()const                    {return m_iCurMissionIndex;}
    inline cTimeTable*      GetTimeTable          ()                         {return &m_TimeTable;}
    
    inline cBulletManager*  GetBulletManagerPlayerTop()                         {return &m_BulletManagerPlayerTop;}
    

    // get object properties
    inline       coreUint8     GetNumPlayers ()const {return this->IsMulti() ? GAME_PLAYERS : 1u;}
    inline const coreInt32*    GetMissionList()const {ASSERT(m_piMissionList) return m_piMissionList;}
    inline const coreUintW&    GetNumMissions()const {return m_iNumMissions;}
    inline const coreUint8&    GetContinues  ()const {return m_iContinues;}
    inline const coreUint8&    GetOutroType  ()const {return m_iOutroType;}
    inline const sGameOptions& GetOptions    ()const {return m_Options;}
    inline const coreUint8&    GetKind       ()const {return m_Options.iKind;}
    inline const coreUint8&    GetType       ()const {return m_Options.iType;}
    inline const coreUint8&    GetMode       ()const {return m_Options.iMode;}
    inline const coreUint8&    GetDifficulty ()const {return m_Options.iDifficulty;}
    inline const coreUint8&    GetFlags      ()const {return m_Options.iFlags;}
    inline const coreUint16&   GetVersion    ()const {return m_iVersion;}
    inline const coreUint16&   GetStatus     ()const {return m_iStatus;}

    // 
    static coreUint8  CalcMedal       (const coreFloat fTime, const coreFloat* pfMedalGoal);
    static coreFloat  CalcMedalTime   (const coreFloat fTime, const coreFloat* pfMedalGoal);
    static coreUint32 CalcBonusTime   (const coreFloat fTime, const coreFloat* pfMedalGoal);
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