///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BOSS_H_
#define _P1_GUARD_BOSS_H_

// TODO 5: boss0101, boomerangs may generate double-hits because of rotating box collision (when moving away from it), bitfield with reset (player-num)
// TODO 5: boss0101, energy direction in front-part of main-model should be inverted
// TODO 5: boss0101, something should grow when small boomerangs begin to create the duplicate
// TODO 5: boss0101, shadow and type rebinding for duplicate
// TODO 5: boss0101, definition for 1.5f (and related multiplications)
// TODO 5: boss0102, add slight explosion where rays hit the screen
// TODO 5: boss0102, separate emitters to three objects, to make them blue
// TODO 5: boss0103, remove small hitch when finishing rotation in the middle shortly before beginning laser-phase
// TODO 3: transformation properties are invalid on start (basically for phase 0), should this be handled ?
// TODO 1: [MF] [HIGH] check if PHASE_AGAIN is required on some PHASE_RESET calls (if 1.0f is reached, all calculations will be repeated with 0.0f, for a (nearly) perfect loop (fractions are still not handled))
// TODO 4: remove counters and vectors (first remove usage)
// TODO 1: remove leviathan demo code + in-mission


// ****************************************************************
// boss definitions
#define BOSS_TIMERS   (8u)    // 
#define BOSS_COUNTERS (16u)   // 
#define BOSS_VECTORS  (12u)   // 

#define BOSS_PLAYERS  (PLAYERS)        // 
#define BOSS_HELPERS  (HELPERS - 1u)   // 


// ****************************************************************
// boss specific definitions
#define DHARUK_TRAILS             (3u)                                         // 
#define DHARUK_DUPLICATE_RAWS     (2u * DHARUK_TRAILS)                         // 
#define DHARUK_BOOMERANGS         (4u)                                         // 
#define DHARUK_BOOMERANGS_RAWS    (DHARUK_BOOMERANGS * (DHARUK_TRAILS + 1u))   // 
#define DHARUK_WIDTH              (0.5f)                                       // 
#define DHARUK_HEIGHT             (0.75f)                                      // 
#define DHARUK_OUTSIDE            (1.5f)                                       // 

#define TORUS_TURRETS             (2u)                                         // 
#define TORUS_GUNNERS             (4u)                                         // 
#define TORUS_CHARGERS            (2u)                                         // 
#define TORUS_DRIVERS             (4u)                                         // 
#define TORUS_WAVERS              (4u)                                         // 
#define TORUS_BARRIERS            (8u)                                         // 
#define TORUS_BOSS_ROTATION       (1.2f)                                       // 
#define TORUS_RUMBLE_VOLUME       (1.5f)                                       // 

#define NAUTILUS_ATTACH_DIST      (-10.0f)                                     // 
#define NAUTILUS_INK_TIME         (10.0f)                                      // 
#define NAUTILUS_INK_SPEED        (1.65f)                                      // 

#define LEVIATHAN_PARTS           (5u)                                         // 
#define LEVIATHAN_PARTS_BODIES    (LEVIATHAN_PARTS - 2u)                       // 
#define LEVIATHAN_RAYS            (LEVIATHAN_PARTS)                            // 
#define LEVIATHAN_RAYS_RAWS       (2u * LEVIATHAN_RAYS)                        // 
#define LEVIATHAN_MARKS           (LEVIATHAN_PARTS)                            // 
#define LEVIATHAN_MARKS_RAWS      (LEVIATHAN_MARKS)                            // 
#define LEVIATHAN_RADIUS_OUTER    (FOREGROUND_AREA.x * 0.8f)                   // 
#define LEVIATHAN_RADIUS_INNER_1  (10.0f)                                      // 
#define LEVIATHAN_RADIUS_INNER_2  (10.0f)                                      // 
#define LEVIATHAN_RAY_OFFSET(i)   ((i) ? 3.6f : 4.8f)                          // 
#define LEVIATHAN_RAY_HEIGHT      (0.2f)                                       // 
#define LEVIATHAN_RAY_SIZE        (coreVector3(1.4f,50.0f,1.4f))               // 
#define LEVIATHAN_RAY_TEXSIZE     (coreVector2(0.5f,1.5f))                     // 
#define LEVIATHAN_RAYWAVE_SIZE    (coreVector3(2.8f,7.0f,2.8f))                // 
#define LEVIATHAN_RAYWAVE_TEXSIZE (coreVector2(0.5f,0.5f))                     // 
#define LEVIATHAN_TILES           (16u)                                        // 

#define TIGER_SIDES               (4u)                                         // 
#define TIGER_STINGS              (32u * TIGER_SIDES)                          // 
#define TIGER_STINGS_SIDE         (32u)                                        // 
#define TIGER_WEAPONS             (5u)                                         // 
#define TIGER_SUBS                (5u)                                         // 
#define TIGER_AIMS                (5u)                                         // 
#define TIGER_DAMAGE              (70)                                         // 

#define MESSIER_SHELLS            (2u)                                         // 
#define MESSIER_RINGS             (3u)                                         // 
#define MESSIER_TRAILS            (3u)                                         // 
#define MESSIER_ENEMIES_SMALL     (8u)                                         // 
#define MESSIER_ENEMIES_BIG       (8u)                                         // 
#define MESSIER_METEOR_RANGE      (1.35f)                                      // 
#define MESSIER_SHOOT_RATE        (1.8f)                                       // 
#define MESSIER_SHOOT_RADIUS      (8.0f * (BULLET_SPEED_FACTOR / MESSIER_SHOOT_RATE))   // 
#define MESSIER_SHOOT_STEP        (0.01f)                                      // 
#define MESSIER_SHOOT_TIMES       (3u)                                         // 
#define MESSIER_SCALE             (10.0f)                                      // 

#define CHOL_WINGS                (4u)                                         // 
#define CHOL_SHIFT                (4.0f)                                       // 
#define CHOL_FAKE_SCORE           (57300u)                                     // 

#define ZEROTH_LIMBS              (6u)                                         // 
#define ZEROTH_ICES               (2u)                                         // 
#define ZEROTH_LIMB_HEAD          (0u)                                         // 
#define ZEROTH_LIMB_TAIL          (3u)                                         // 
#define ZEROTH_RADIUS             (8.0f)                                       // 
#define ZEROTH_RADIUS_BULLET      (2.0f)                                       // 
#define ZEROTH_SPEED_SLOW         (8.0f)                                       // 
#define ZEROTH_SPEED_FAST         (16.0f)                                      // 
#define ZEROTH_LASER_SIZE         (coreVector3(1.65f,45.0f,1.65f))             // 
#define ZEROTH_LASER_TEXSIZE      (coreVector2(0.5f,1.0f))                     // 
#define ZEROTH_LASERWAVE_SIZE     (coreVector3(2.9f,22.5f,2.9f))               // 
#define ZEROTH_LASERWAVE_TEXSIZE  (coreVector2(0.5f,1.5f))                     // 

#define GEMINGA_ENEMIES_TELEPORT  (4u)                                         // 
#define GEMINGA_ENEMIES_LEGION    (10u)                                        // 

#define PROJECTONE_SHIELDS        (HELPERS - 1u)                               // 
#define PROJECTONE_CLONES         (5u)                                         // 
#define PROJECTONE_FRAGMENTS      (FRAGMENTS - 1u)                             // 
#define PROJECTONE_ENEMIES_METEOR (8u)                                         // 
#define PROJECTONE_ENEMIES_LEGION (4u)                                         // 
#define PROJECTONE_COLL_SCALE     (1.1f)                                       // 

#define EIGENGRAU_LAYERS          (4u)                                         // 
#define EIGENGRAU_PARASITES       (1u)//18u)                                        // 
#define EIGENGRAU_FOLLOWERS       (1u)//52u)                                        // 
#define EIGENGRAU_DEPTH           (-340.0f)                                    // 


// ****************************************************************
// phase management macros
#define PHASE_CONTROL_TIMER(a,b,c)      this->_PhaseTimer (a, __LINE__, b, c,    [&](const coreFloat  fTime, const coreFloat fTimeBefore, const coreBool __bEnd)   // NOLINT
#define PHASE_CONTROL_TICKER(a,b,c,d)   this->_PhaseTicker(a, __LINE__, b, c, d, [&](const coreUint16 iTick,                              const coreBool __bEnd)   // NOLINT
#define PHASE_CONTROL_PAUSE(a,b)        PHASE_CONTROL_TICKER(a, 1u, b, LERP_LINEAR)

#define PHASE_TIME_POINT(t)             (InBetween((t), fTimeBefore, fTime))
#define PHASE_TIME_BEFORE(t)            (fTime <  (t))
#define PHASE_TIME_AFTER(t)             (fTime >= (t))
#define PHASE_TIME_BETWEEN(t,u)         (InBetween(fTime, (t), (u)))
#define PHASE_BEGINNING                 (PHASE_TIME_POINT(0.0f))

#define PHASE_MAINTIME_POINT(t)         (InBetween((t), m_fPhaseTimeBefore, m_fPhaseTime))
#define PHASE_MAINTIME_BEFORE(t)        (m_fPhaseTime <  (t))
#define PHASE_MAINTIME_AFTER(t)         (m_fPhaseTime >= (t))
#define PHASE_MAINTIME_BETWEEN(t,u)     (InBetween(m_fPhaseTime, (t), (u)))
#define PHASE_BEGINNING2                (PHASE_MAINTIME_POINT(0.0f))

#define PHASE_POSITION_POINT(e,t,v)     (STAGE_POSITION_POINT  (e, t, v))
#define PHASE_POSITION_BEFORE(e,t,v)    (STAGE_POSITION_BEFORE (e, t, v))
#define PHASE_POSITION_AFTER(e,t,v)     (STAGE_POSITION_AFTER  (e, t, v))
#define PHASE_POSITION_BETWEEN(e,t,u,v) (STAGE_POSITION_BETWEEN(e, t, u, v))
#define PHASE_FLYPAST(e,f,v)            (STAGE_FLYPAST         (e, f, v))

#define PHASE_CHANGE_TO(i)              {this->ChangePhase(i);}
#define PHASE_CHANGE_INC                {this->ChangePhase(m_iPhase + 1u);}
#define PHASE_RESET(i)                  {m_aTimer[i].Stop(); m_aiTimerLine[i] = 0u;}
#define PHASE_AGAIN                     {m_bControlAgain = true;}
#define PHASE_FINISHED                  (__bEnd)


// ****************************************************************
// 
#define LERP_LINEAR     (&LERP         <coreFloat>)
#define LERP_SMOOTH     (&LERPS        <coreFloat>)
#define LERP_SMOOTH_REV (&LerpSmoothRev<coreFloat>)
#define LERP_BREAK      (&LERPB        <coreFloat>)
#define LERP_BREAK_REV  (&LERPBR       <coreFloat>)
#define LERP_HERMITE3   (&LERPH3       <coreFloat>)
#define LERP_HERMITE5   (&LERPH5       <coreFloat>)


// ****************************************************************
// boss entity interface
class INTERFACE cBoss : public cEnemy
{
protected:
    coreTimer  m_aTimer     [BOSS_TIMERS];    // 
    coreUint16 m_aiTimerLine[BOSS_TIMERS];    // 

    coreInt16   m_aiCounter[BOSS_COUNTERS];   // 
    coreVector4 m_avVector [BOSS_VECTORS];    // 

    coreVector2 m_vLastPosition;              // 
    coreVector3 m_vLastDirection;             // 
    coreVector3 m_vLastOrientation;           // 

    coreUint8 m_iPhase;                       // 
    coreFlow  m_fPhaseTime;                   // 
    coreFloat m_fPhaseTimeBefore;             // 

    coreBool m_bControlAgain;                 // 

    coreUint8 m_iHelperSpawn;                 // 
    coreUint8 m_iHelperHit;                   // 

    coreBool m_bActive;                       // 
    coreBool m_bForeshadow;                   // 

    static coreVector2 s_vPositionPoint;      // 


public:
    cBoss()noexcept;
    virtual ~cBoss()override;

    DISABLE_COPY(cBoss)
    ENABLE_ID_EX
    ENABLE_EXTRA

    // 
    void ChangePhase(const coreUint8 iPhase);

    // 
    void StorePosition(const coreVector2 vPos);
    void StorePosition();
    void StoreRotation(const coreVector3 vDir, const coreVector3 vOri);
    void StoreRotation();

    // 
    inline coreBool HasAllHelpers()const {return (m_iHelperHit == BITLINE(BOSS_HELPERS));}

    // get object properties
    inline  const coreUint8& GetPhase    ()const {return m_iPhase;}
    inline  const coreUint8& GetHelperHit()const {return m_iHelperHit;}
    virtual const coreChar*  GetMusicName()const {return "";}


protected:
    // 
    void _StartBoss();
    void _EndBoss();

    // 
    void _UpdateBoss();

    // 
    void _ResurrectBoss();

    // 
    coreBool _ResurrectHelper(const coreUint8 iElement, const coreBool bSmooth);
    void     _KillHelper     (const coreUint8 iElement, const coreBool bAnimated);

    // 
    void _CreateFragment(const coreUint8 iType, const coreVector2 vPosition);
    void _CreateFragment(const coreUint8 iType);

    // 
    template <typename F, typename G> void _PhaseTimer (const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreFloat  fSpeed,                        G&& nLerpFunc, F&& nUpdateFunc);   // [](const coreFloat x, const coreFloat y, const coreFloat s) -> coreFloat, [](const coreFloat  fTime, const coreFloat fTimeBefore, const coreBool __bEnd) -> void
    template <typename F, typename G> void _PhaseTicker(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreUint16 iTicks, const coreFloat fRate, G&& nLerpFunc, F&& nUpdateFunc);   // [](const coreFloat x, const coreFloat y, const coreFloat s) -> coreFloat, [](const coreUint16 iTick,                              const coreBool __bEnd) -> void
};


// ****************************************************************
// Dharuk boss class
class cDharukBoss final : public cBoss
{
private:
    cCustomEnemy  m_Duplicate;                                  // 
    coreBatchList m_DuplicateTrail;                             // 
    coreObject3D  m_aDuplicateRaw[DHARUK_DUPLICATE_RAWS];       // 
    coreFloat     m_fDuplicateValue;                            // 

    coreBatchList m_Boomerang;                                  // 
    coreBatchList m_BoomerangTrail;                             // 
    coreObject3D  m_aBoomerangRaw   [DHARUK_BOOMERANGS_RAWS];   // 
    coreFloat     m_afBoomerangValue[DHARUK_BOOMERANGS];        // 

    coreObject3D m_Summon;                                      // 

    coreFlow m_afVisibility[2];                                 // 
    coreBool m_bHelperEvent;                                    // 

    coreUint8 m_iPackedDir;                                     // 
    coreFlow  m_fAnimation;                                     // animation value


public:
    cDharukBoss()noexcept;
    ~cDharukBoss()final;

    DISABLE_COPY(cDharukBoss)
    ASSIGN_ID_EX(101, "DHARUK", COLOR_MENU_RED, COLOR_MENU_RED, coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("ダルク")

    // 
    inline const coreBool& GetHelperEvent()const {return m_bHelperEvent;}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __RenderOwnOver ()final;
    void __MoveOwn       ()final;

    // 
    void __EnableDuplicate ();
    void __DisableDuplicate(const coreBool bAnimated);

    // 
    void __EnableBoomerang (const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection);
    void __DisableBoomerang(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableSummon (const coreVector2 vPosition);
    void __DisableSummon(const coreBool bAnimated);

    // 
    inline void __BecomeInvisible1() {m_afVisibility[0] = -1.0f; this      ->SetAlpha(0.0f); this      ->AddStatus(ENEMY_STATUS_HIDDEN);}
    inline void __BecomeInvisible2() {m_afVisibility[1] = -1.0f; m_Duplicate.SetAlpha(0.0f); m_Duplicate.AddStatus(ENEMY_STATUS_HIDDEN);}

    // 
    coreVector2 __RepeatPosition (const coreVector2 vPosition, const coreFloat fThreshold, coreBool* OUTPUT pbChange);
    coreVector2 __RepeatPosition (const coreVector2 vPosition, const coreFloat fThreshold);
    void        __EncodeDirection(const coreUintW iIndex, const coreVector2 vDirection);
    coreVector2 __DecodeDirection(const coreUintW iIndex);
};


// ****************************************************************
// Torus boss class
class cTorusBoss final : public cBoss
{
private:
    coreObject3D m_Emitter;                            // 
    coreObject3D m_aCircle[2];                         // 
    coreObject3D m_Summon;                             // 

    cCustomEnemy  m_aTurret[TORUS_TURRETS];            // 
    coreBatchList m_TurretHull;                        // 
    coreObject3D  m_aTurretHullRaw[TORUS_TURRETS];     // 

    cCustomEnemy  m_aGunner[TORUS_GUNNERS];            // 
    coreBatchList m_GunnerHull;                        // 
    coreObject3D  m_aGunnerHullRaw[TORUS_GUNNERS];     // 

    cCustomEnemy  m_aCharger[TORUS_CHARGERS];          // 
    coreBatchList m_ChargerHull;                       // 
    coreObject3D  m_aChargerHullRaw[TORUS_CHARGERS];   // 

    cCustomEnemy  m_aDriver[TORUS_DRIVERS];            // 
    coreBatchList m_DriverHull;                        // 
    coreObject3D  m_aDriverHullRaw[TORUS_DRIVERS];     // 

    cCustomEnemy  m_aWaver[TORUS_WAVERS];              // 
    coreBatchList m_WaverHull;                         // 
    coreObject3D  m_aWaverHullRaw[TORUS_WAVERS];       // 

    coreFlow m_fAnimation;                             // animation value
    coreFlow m_fRotationBoss;                          // 
    coreFlow m_fRotationObject;                        // 

    coreUint8 m_iTurretActive;                         // 
    coreUint8 m_iTurretMove;                           // 
    coreUint8 m_iGunnerActive;                         // 
    coreUint8 m_iChargerActive;                        // 
    coreUint8 m_iDriverActive;                         // 
    coreUint8 m_iWaverActive;                          // 

    coreUint8 m_iDecalState;                           // 

    coreSoundPtr m_pRumbleSound;                       // 
    coreSoundPtr m_pWooshSound;                        // 


public:
    cTorusBoss()noexcept;
    ~cTorusBoss()final;

    DISABLE_COPY(cTorusBoss)
    ASSIGN_ID_EX(102, "TORUS", COLOR_MENU_GREEN, COLOR_MENU_GREEN, coreVector2(0.75f,0.25f))
    ASSIGN_EXTRA("トーラス")

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_01.ogg";}


private:
    // execute own routines
    void __ResurrectOwn   ()final;
    void __KillOwn        (const coreBool bAnimated)final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __MoveOwn        ()final;

    // 
    void __EnableSummon (const coreVector2 vPosition, const coreVector3 vColor);
    void __DisableSummon(const coreBool bAnimated);

    // 
    void __EnableTurret (const coreUintW iIndex, const coreVector2 vPosition);
    void __DisableTurret(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableGunner (const coreUintW iIndex, const coreVector2 vPosition);
    void __DisableGunner(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableCharger (const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection);
    void __DisableCharger(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableDriver (const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection);
    void __DisableDriver(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableWaver (const coreUintW iIndex, const coreVector2 vPosition);
    void __DisableWaver(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __CreateOverdrive(const coreVector3 vIntersect);

    // 
    void __ChangeColor(const coreUint8 iType);

    // 
    static coreFloat   __PositionToGrind (const coreVector2 vPosition);
    static coreVector2 __GrindToPosition (const coreFloat   fGrind);
    static coreVector2 __GrindToPosition2(const coreFloat   fGrind);
    static coreVector2 __GrindToPosition3(const coreFloat   fGrind);
};


// ****************************************************************
// Vaus boss class
class cVausBoss final : public cBoss
{
private:
    cCustomEnemy m_aCompanion[2];   // 


public:
    cVausBoss()noexcept;
    ~cVausBoss()final;

    DISABLE_COPY(cVausBoss)
    ASSIGN_ID_EX(103, "VAUS", COLOR_MENU_YELLOW, COLOR_MENU_YELLOW, coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("バウス")


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;
};


// ****************************************************************
// Nautilus boss class
class cNautilusBoss final : public cBoss
{
private:
    cCustomEnemy m_aClaw[2];        //
    coreFloat    m_fClawAngle;      // 

    coreObject3D m_InkBullet;       // 
    coreVector2  m_vInkTarget;      // 
    coreFlow     m_afInkAlpha[2];   // 

    coreFlow m_fAnimation;          // animation value
    coreFlow m_fMovement;           // 


public:
    cNautilusBoss()noexcept;
    ~cNautilusBoss()final;

    DISABLE_COPY(cNautilusBoss)
    ASSIGN_ID_EX(201, "NAUTILUS", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __ResurrectOwn ()final;
    void __KillOwn      (const coreBool bAnimated)final;
    void __RenderOwnOver()final;
    void __MoveOwn      ()final;

    // 
    void __CreateInk(const coreUintW iIndex, const coreVector2 vPosition);

    // 
    void __EnableBullet (const coreVector2 vStart, const coreVector2 vEnd);
    void __DisableBullet(const coreBool bAnimated);
};


// ****************************************************************
// Amemasu boss class
class cAmemasuBoss final : public cBoss
{
public:
    cAmemasuBoss()noexcept;
    ~cAmemasuBoss()final;

    DISABLE_COPY(cAmemasuBoss)
    ASSIGN_ID_EX(202, "AMEMASU", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Leviathan boss class
class cLeviathanBoss final : public cBoss
{
private:
    cCustomEnemy m_Head;                              // 
    cCustomEnemy m_aBody[LEVIATHAN_PARTS_BODIES];     // 
    cCustomEnemy m_Tail;                              // 

    coreBatchList m_Ray;                              // 
    coreBatchList m_RayWave;                          // 
    coreObject3D  m_aRayRaw  [LEVIATHAN_RAYS_RAWS];   // 
    coreFlow      m_afRayTime[LEVIATHAN_RAYS];        // 
    coreFloat     m_afRayLen [LEVIATHAN_RAYS];        // 
    coreUint8     m_iRayState;                        // 

    coreBatchList m_Mark;                             // 
    coreObject3D  m_aMarkRaw  [LEVIATHAN_MARKS];      // 

    coreUint16 m_iDecalState;                         // 

    coreVector2 m_avSwimDir  [LEVIATHAN_PARTS];       // 
    coreUint8   m_aiSwimCount[LEVIATHAN_PARTS];       // 

    coreFlow m_fAnimation;                            // animation value
    coreFlow m_fMovement;                             // 

    coreSoundPtr m_pLaserSound;                       // 
    coreSoundPtr m_pRumbleSound;                      // 


public:
    cLeviathanBoss()noexcept;
    ~cLeviathanBoss()final;

    DISABLE_COPY(cLeviathanBoss)
    ASSIGN_ID_EX(203, "LEVIATHAN", COLOR_MENU_CYAN, COLOR_MENU_CYAN, coreVector2(0.5f,0.25f))
    ASSIGN_EXTRA("レヴィアタン")

    // 
    void ResurrectDemo();

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_02.ogg";}


private:
    // execute own routines
    void __ResurrectOwn   ()final;
    void __KillOwn        (const coreBool bAnimated)final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __MoveOwn        ()final;

    // 
    void __EnableRay      (const coreUintW iIndex, const coreBool bAnimated);
    void __DisableRay     (const coreUintW iIndex, const coreBool bAnimated);
    void __BeginRay       (const coreUintW iIndex);
    void __CreateOverdrive(const coreUintW iIndex, const coreVector3 vIntersect, const coreFloat fTime, const coreBool bGround);

    // 
    void __EnableMark (const coreUintW iIndex);
    void __DisableMark(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __UpdateHealth();
    void __RefreshHealth(const coreInt32 iHead, const coreInt32 iBody, const coreInt32 iTail);

    // 
    static FUNC_NOALIAS void      __CalcCurvePosDir(const coreVector3 vAxis, const coreFloat fAngle, const coreVector3 vScale, coreVector3* OUTPUT vPosition, coreVector3* OUTPUT vDirection);
    static FUNC_CONST   coreFloat __CalcAngle      (const coreFloat fDistance, const coreFloat fRadius);

    // 
    cEnemy*                     __GetPart        (const coreUintW iIndex);
    static FUNC_CONST coreFloat __GetPartDistance(const coreUintW iIndex);
};


// ****************************************************************
// Urtica boss class
class cUrticaBoss final : public cBoss
{
public:
    cUrticaBoss()noexcept;
    ~cUrticaBoss()final;

    DISABLE_COPY(cUrticaBoss)
    ASSIGN_ID_EX(301, "URTICA", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Tiger boss class
class cTigerBoss final : public cBoss
{
private:
    cCustomEnemy m_aTrack    [2];                // 
    cCustomEnemy m_aWeapon   [TIGER_SUBS];       // 
    cCustomEnemy m_aWeaponOld[TIGER_SUBS];       // 

    coreObject3D m_aBeam[2];                     // 
    coreVector2  m_vBeamPos;                     // 
    coreFlow     m_fBeamTime;                    // 

    coreObject3D m_aAim[TIGER_AIMS];             // 
    coreObject3D m_AimRing;                      // 

    coreBatchList m_Sting;                       // 
    coreObject3D  m_aStingRaw  [TIGER_STINGS];   // 
    coreFlow      m_afStingTime[TIGER_SIDES];    // 

    coreVector2 m_avPushDir[2];                  // (0 = current | 1 = target) 
    coreFlow    m_fPushPower;                    // 
    coreBool    m_bPushState;                    // 

    coreModelPtr m_aapModelHigh[TIGER_WEAPONS][TIGER_SUBS];   // 
    coreModelPtr m_aapModelLow [TIGER_WEAPONS][TIGER_SUBS];   // 
    coreUint8    m_iWeaponType;                  // 
    coreUint8    m_iWeaponTypeOld;               // 
    coreFlow     m_fWeaponChange;                // 

    coreUint16 m_aiBulletHit[TIGER_WEAPONS];     // 

    coreUint8 m_iDecalState;                     // 

    coreFlow m_fAnimation;                       // 

    coreSoundPtr m_pTankSound;                   // 


public:
    cTigerBoss()noexcept;
    ~cTigerBoss()final;

    DISABLE_COPY(cTigerBoss)
    ASSIGN_ID_EX(302, "TIGER MK-III", COLOR_MENU_YELLOW, COLOR_MENU_YELLOW, coreVector2(0.0f,0.0f))
    ASSIGN_EXTRA("タイガーマーク III")

    // 
    inline coreBool ResurrectHelper(const coreUint8 iElement, const coreBool bSmooth)   {return this->_ResurrectHelper(iElement, bSmooth);}
    inline void     KillHelper     (const coreUint8 iElement, const coreBool bAnimated) {this->_KillHelper(iElement, bAnimated);}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_03.ogg";}


private:
    // execute own routines
    void __ResurrectOwn   ()final;
    void __KillOwn        (const coreBool bAnimated)final;
    void __RenderOwnBottom()final;
    void __RenderOwnOver  ()final;
    void __MoveOwn        ()final;

    // 
    void __EnableBeam (const coreVector2 vPosition);
    void __DisableBeam(const coreBool bAnimated);

    // 
    void __EnableAim (const coreUintW iIndex);
    void __DisableAim(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableStings (const coreUintW iIndex);
    void __DisableStings(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableWind (const coreVector2 vDir);
    void __DisableWind();

    // 
    void __SwitchWeapon(const coreUintW iType);

    // 
    void __ShootWeapon();

    // 
    void __CauseBeamDamage(cPlayer* OUTPUT pTarget);

    // 
    void __CreateTrail    (const coreUintW iIndex, const coreVector3 vIntersect);
    void __CreateOverdrive(const coreVector3 vIntersect);
};


// ****************************************************************
// Lucifer boss class
class cLuciferBoss final : public cBoss
{
public:
    cLuciferBoss()noexcept;
    ~cLuciferBoss()final;

    DISABLE_COPY(cLuciferBoss)
    ASSIGN_ID_EX(303, "L.U.C.I.F.E.R.", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Quaternion boss class
class cQuaternionBoss final : public cBoss
{
public:
    cQuaternionBoss()noexcept;
    ~cQuaternionBoss()final;

    DISABLE_COPY(cQuaternionBoss)
    ASSIGN_ID_EX(401, "QUATERNION", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Saros boss class
class cSarosBoss final : public cBoss
{
public:
    cSarosBoss()noexcept;
    ~cSarosBoss()final;

    DISABLE_COPY(cSarosBoss)
    ASSIGN_ID_EX(402, "SAROS", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Messier boss class
class cMessierBoss final : public cBoss
{
private:
    cCustomEnemy m_aShell[MESSIER_SHELLS];             // 

    coreObject3D m_aRing[MESSIER_RINGS];               // 
    coreFlow     m_fRingTime;                          // 
    coreFlow     m_fRingScreen;                        // 

    coreObject3D m_aClock       [2];                   // 
    coreObject3D m_aaClockTrail [2][MESSIER_TRAILS];   // 
    coreFlow     m_aafClockAlpha[2][MESSIER_TRAILS];   // 
    coreFlow     m_fClockTime;                         // 
    coreBool     m_bClockState;                        // 

    coreObject3D m_Bubble;                             // 
    coreFlow     m_fBubbleTime;                        // 
    coreFlow     m_fBubbleAlpha;                       // 
    coreBool     m_bBubbleState;                       // 

    coreObject3D m_aHole[2];                           // 
    coreBool     m_bHoleState;                         // 

    coreFloat m_fTimeFactor;                           // 
    coreUint8 m_iTimeRevert;                           // 
    coreBool  m_bTimeMusic;                            // 

    coreFlow   m_afShootTime[MESSIER_SHOOT_TIMES];     // 
    coreUint32 m_aiShootTick[MESSIER_SHOOT_TIMES];     // 

    coreUint32 m_iTick;                                // 

    coreFlow m_fMeteorRota;                            // 
    coreFlow m_fBossRota;                              // 

    coreSoundPtr m_pBellSound;                         // 
    coreSoundPtr m_pVoidSound;                         // 


public:
    cMessierBoss()noexcept;
    ~cMessierBoss()final;

    DISABLE_COPY(cMessierBoss)
    ASSIGN_ID_EX(403, "MESSIER 87", COLOR_MENU_MAGENTA, COLOR_MENU_MAGENTA, coreVector2(0.75f,0.0f))
    ASSIGN_EXTRA("メシエ 87")

    // get object properties
    inline const coreChar* GetMusicName   ()const final {return "boss_04.ogg";}
    inline const coreChar* GetMusicNameRev()const       {return "boss_04_reverse.ogg";}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __RenderOwnOver ()final;
    void __RenderOwnTop  ()final;
    void __MoveOwn       ()final;

    // 
    void __EnableRings ();
    void __DisableRings(const coreBool bAnimated);

    // 
    void __EnableClock ();
    void __DisableClock(const coreBool bAnimated);

    // 
    void __EnableBubble (const coreVector3 vColor);
    void __DisableBubble(const coreBool bAnimated);

    // 
    void __EnableHole ();
    void __DisableHole(const coreBool bAnimated);

    // 
    void __SetWavePosition(const coreVector2 vPosition);

    // 
    void     __AddBullet  (const coreUintW iType, const coreFloat fSpeed, const coreVector2 vPosition, const coreVector2 vDirection);
    coreBool __PhaseTicker(const coreUintW iIndex, const coreFloat fRate);
};


// ****************************************************************
// Tartarus boss class
class cTartarusBoss final : public cBoss
{
public:
    cTartarusBoss()noexcept;
    ~cTartarusBoss()final;

    DISABLE_COPY(cTartarusBoss)
    ASSIGN_ID_EX(501, "TARTARUS", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Phalaris boss class
class cPhalarisBoss final : public cBoss
{
public:
    cPhalarisBoss()noexcept;
    ~cPhalarisBoss()final;

    DISABLE_COPY(cPhalarisBoss)
    ASSIGN_ID_EX(502, "PHALARIS", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Chol boss class
class cCholBoss final : public cBoss
{
private:
    cCustomEnemy m_aWing[CHOL_WINGS];         // 
    coreObject3D m_Fire;                      // 

    coreUint8   m_aiWingState [CHOL_WINGS];   // 
    coreFlow    m_afWingTime  [CHOL_WINGS];   // 
    coreVector3 m_avWingReturn[CHOL_WINGS];   // 

    coreModelPtr m_apFireModel[2];            // 
    coreFlow     m_fFireFade;                 // 
    coreUint8    m_iFireType;                 // 
    coreBool     m_bFireActive;               // 

    coreFloat m_fWebLevel;                    // 
    coreFloat m_fWebReverse;                  // 
    coreUintW m_iWebIndex;                    // 
    coreUintW m_iPathIndex;                   // 
    coreUint8 m_iPathSide;                    // 

    coreFloat m_fTilt;                        // 
    coreFloat m_fFlap;                        // 

    coreFloat m_fPush;                        // 

    coreFlow m_fCountdown;                    // 

    coreFlow m_fAnimation;                    // 


public:
    cCholBoss()noexcept;
    ~cCholBoss()final;

    DISABLE_COPY(cCholBoss)
    ASSIGN_ID_EX(503, "CHOL", COLOR_MENU_ORANGE, COLOR_MENU_ORANGE, coreVector2(0.25f,0.0f))
    ASSIGN_EXTRA("コール")

    // 
    void ResurrectIntro(const coreUint8 iSub);

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_05.ogg";}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;

    // 
    void __EnableFire (const coreUint8 iType);
    void __DisableFire(const coreBool bAnimated);

    // 
    inline void __ChangeWing          (const coreUintW iIndex, const coreUint8 iState) {ASSERT(iIndex < CHOL_WINGS) m_aiWingState[iIndex] = iState; m_afWingTime[iIndex] = 0.0f;}
    inline void __ChangeWingInc       (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, m_aiWingState[iIndex] + 1u);}
    inline void __ChangeWingReset     (const coreUintW iIndex)                         {this->__ChangeWing(iIndex,  0u);}
    inline void __ChangeWingIntro     (const coreUintW iIndex)                         {this->__ChangeWing(iIndex,  1u);}
    inline void __ChangeWingThrow1    (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 11u);}
    inline void __ChangeWingThrow2    (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 21u);}
    inline void __ChangeWingReturn    (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 31u);}
    inline void __ChangeWingPull      (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 41u);}
    inline void __ChangeWingSpike     (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 51u);}
    inline void __ChangeWingExplode1  (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 61u);}
    inline void __ChangeWingExplode2  (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 71u);}
    inline void __ChangeWingResurrect1(const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 81u);}
    inline void __ChangeWingResurrect2(const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 91u);}

    // 
    void __RepairWing(const coreUintW iIndex, const coreInt32 iHealth);

    // 
    void __Impact(coreObject3D* OUTPUT pObject, const coreVector2 vPos, const coreVector3 vDir, const coreFloat fTime, const coreFloat fTimeBefore, const coreBool bBig);

    // 
    void __ResurrectFake();
    void __KillFake();
};


// ****************************************************************
// Fenrir boss class
class cFenrirBoss final : public cBoss
{
public:
    cFenrirBoss()noexcept;
    ~cFenrirBoss()final;

    DISABLE_COPY(cFenrirBoss)
    ASSIGN_ID_EX(601, "FENRIR", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Shelob boss class
class cShelobBoss final : public cBoss
{
public:
    cShelobBoss()noexcept;
    ~cShelobBoss()final;

    DISABLE_COPY(cShelobBoss)
    ASSIGN_ID_EX(602, "SHELOB", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Zeroth boss class
class cZerothBoss final : public cBoss
{
private:
    cCustomEnemy m_aLimb      [ZEROTH_LIMBS];   // 
    coreFloat    m_afLimbValue[ZEROTH_LIMBS];   // 

    cCustomEnemy m_Body;                        // 

    coreObject3D m_Laser;                       // 
    coreObject3D m_LaserWave;                   // 
    coreVector2  m_vLaserDir;                   // 
    coreFlow     m_fLaserTime;                  // 
    coreUint8    m_iLaserLimb;                  // 

    coreObject3D m_Indicator;                   // 

    cCustomEnemy m_aIce        [ZEROTH_ICES];   // 
    coreUint8    m_aiIceDir    [ZEROTH_ICES];   // 
    coreBool     m_abIcePreview[ZEROTH_ICES];   // 

    coreVector2 m_vGlobalDir;                   // 

    coreFlow m_fAnimation;                      // 

    coreSoundPtr m_pLaserSound;                 // 


public:
    cZerothBoss()noexcept;
    ~cZerothBoss()final;

    DISABLE_COPY(cZerothBoss)
    ASSIGN_ID_EX(603, "ZEROTH", COLOR_MENU_BLUE, COLOR_MENU_BLUE, coreVector2(0.25f,0.25f))
    ASSIGN_EXTRA("ゼロフ")

    // 
    void ResurrectIntro();

    // 
    inline void HideTail() {for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) this->__SetLimbValue(i, 1.0f);}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_06.ogg";}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __RenderOwnOver ()final;
    void __MoveOwn       ()final;

    // 
    void __EnableLaser (const coreUintW iLimb, const coreBool bAnimated);
    void __DisableLaser(const coreBool bAnimated);
    void __BeginLaser  ();

    // 
    void __CreateCube (const coreVector2 vPosition, const coreVector2 vDirection, const coreBool bPreview);
    void __DestroyCube(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __SetLimbValue(const coreUintW iIndex, const coreFloat fValue);

    //
    void __SetIndicator(const coreFloat fValue, const coreVector3 vColor = coreVector3(0.0f,0.0f,0.0f));
};


// ****************************************************************
// Orlac boss class
class cOrlacBoss final : public cBoss
{
public:
    cOrlacBoss()noexcept;
    ~cOrlacBoss()final;

    DISABLE_COPY(cOrlacBoss)
    ASSIGN_ID_EX(701, "ORLAC", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Geminga boss class
class cGemingaBoss final : public cBoss
{
private:
    cCustomEnemy m_Sphere;             // 
    cCustomEnemy m_InsideTop;          // 
    cCustomEnemy m_InsideBottom;       // 
    cCustomEnemy m_Top;                // 
    cCustomEnemy m_Bottom;             // 

    cDharukBoss m_Dharuk;              // 

    coreFloat m_fMouthAngle;           // 

    coreSpline2 m_ChangePath;          // 
    coreSpline2 m_aPackPath[4];        // 

    coreParticleEffect m_SuckEffect;   // 

    coreSoundPtr m_pVacuumSound;       // 

    coreDummyPtr m_apResCache[25];     // 


public:
    cGemingaBoss()noexcept;
    ~cGemingaBoss()final;

    DISABLE_COPY(cGemingaBoss)
    ASSIGN_ID_EX(702, "NAGUAL"/*"GEMINGA"*/, COLOR_MENU_RED, COLOR_MENU_RED, coreVector2(0.5f,0.0f))
    ASSIGN_EXTRA("ナワル")

    // 
    void ResurrectIntro();

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_07.ogg";}


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;

    // 
    inline void __DefaultOrientation() {this->SetOrientation(coreVector3(this->GetDirection().xy().Rotated90(), 0.0f));}
};


// ****************************************************************
// Nagual boss class
class cNagualBoss final : public cBoss
{
public:
    cNagualBoss()noexcept;
    ~cNagualBoss()final;

    DISABLE_COPY(cNagualBoss)
    ASSIGN_ID_EX(703, "NAGUAL", coreVector3(0.0f,0.0f,0.0f), coreVector3(0.0f,0.0f,0.0f), coreVector2(-1.0f,-1.0f))
    ASSIGN_EXTRA("")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Project One boss class
class cProjectOneBoss final : public cBoss
{
public:
    // 
    static constexpr const coreVector3 Color  = coreVector3(1.0f,1.0f,1.0f);
    static constexpr const coreVector3 Color2 = coreVector3(1.0f,1.0f,1.0f);
    static constexpr const coreVector2 Icon   = coreVector2(0.0f,0.5f);


private:
    cCustomEnemy m_aShield  [PROJECTONE_SHIELDS];     // 
    cCustomEnemy m_aClone   [PROJECTONE_CLONES];      // 
    coreObject3D m_aFragment[PROJECTONE_FRAGMENTS];   // 

    coreObject3D m_Roadsign;                          // 
    coreVector2  m_vRoadsignPos;                      // 
    coreFlow     m_fRoadsignTime;                     // 

    coreObject3D m_Range;                             // 
    coreObject3D m_Arrow;                             // 
    coreObject3D m_Wind;                              // 
    coreObject3D m_Bubble;                            // 
    coreObject3D m_Exhaust;                           // 

    coreFloat m_fSmoothThrust;                        // 

    coreVector2 m_vOldDir;                            // 
    coreFlow    m_fArrowValue;                        // 

    coreSpline2 m_HelperPath;                         // 
    coreUint8   m_iHelperState;                       // 

    coreVector3 m_vLevelColor;                        // 

    coreFlow m_fAnimation;                            // 

    coreFlow  m_fChromaValue;                         // 
    coreFloat m_fChromaSpeed;                         // 

    coreFlow    m_fWaveValue;                         // 
    coreVector3 m_vColorFrom;                         // 
    coreVector3 m_vColorTo;                           // 

    coreFlow  m_fPatternValue;                        // 
    coreFlow  m_fPatternStrength;                     // 
    coreUint8 m_iPatternType;                         // 
    
    coreSoundPtr m_pNightmareSound;                   // 

    coreBool m_bDead;                                 // 

    // yellow
    coreFlow m_fFangTime;                             // 

    // orange
    coreUint8   m_iSlowdown;                          // 
    coreFloat   m_fPushForce;                         // 
    coreVector2 m_vPushDir;                           // 

    // red
    coreFlow m_fPearlFlash;                           // 

    // magenta
    coreFlow  m_fPlateTime;                           // 
    coreUint8 m_iMeteorDir;                           // 
    coreUint8 m_iBulletTick;                          // 
    coreFloat m_fGameAngle;                           // 

    // purple
    coreSpline2 m_aPurplePath[2];                     // 

    // blue
    coreVector2 m_vGlobalDir;                         // 
    coreVector2 m_vBoulderDir;                        // 

    // cyan
    coreFlow m_fArrowRota;                            // 

    // white
    coreFlow m_fFinalValue;                           // 
    coreFlow m_fFinalLerp;                            // 


public:
    cProjectOneBoss()noexcept;
    ~cProjectOneBoss()final;

    DISABLE_COPY(cProjectOneBoss)
    ASSIGN_ID(801, "P1")
    ASSIGN_EXTRA("")

    // get object properties
    inline coreVector3     GetColor    ()const final {return m_vLevelColor;}
    inline coreVector3     GetColor2   ()const final {return m_vLevelColor;}
    inline coreVector2     GetIcon     ()const final {return cProjectOneBoss::Icon;}
    inline const coreChar* GetMusicName()const final {return "boss_08_intro.ogg";}

    // 
    static void CalcColor    (const coreUintW iIndex, coreVector3* OUTPUT pvEnergyColor, coreVector3* OUTPUT pvBlockColor, coreVector3* OUTPUT pvLevelColor, coreVector3* OUTPUT pvBackColor, coreVector3* OUTPUT pvLedColor);
    static void CalcColorLerp(const coreFloat fValue, coreVector3* OUTPUT pvEnergyColor, coreVector3* OUTPUT pvBlockColor, coreVector3* OUTPUT pvLevelColor, coreVector3* OUTPUT pvBackColor, coreVector3* OUTPUT pvLedColor);


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __RenderOwnTop  ()final;
    void __MoveOwn       ()final;

    // 
    void __MoveYellow ();
    void __MoveOrange ();
    void __MoveRed    ();
    void __MoveMagenta();
    void __MovePurple ();
    void __MoveBlue   ();
    void __MoveCyan   ();
    void __MoveGreen  ();
    void __MoveWhite  ();
    void __MoveIntro  ();

    // 
    void __EnableFragment (const coreUintW iIndex);
    void __DisableFragment(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableRoadsign (const coreVector2 vPosition, const coreVector2 vDirection);
    void __DisableRoadsign(const coreBool bAnimated);

    // 
    void __ShowArrow();

    // 
    void __StartFeeling();
    void __EndFeeling  ();

    // 
    void __EnableRange   ();
    void __DisableRange  ();
    void __EnableArrow   ();
    void __DisableArrow  ();
    void __EnableWind    ();
    void __DisableWind   ();
    void __EnableBubble  ();
    void __DisableBubble ();
    void __EnableExhaust ();
    void __DisableExhaust();

    // 
    void __SetEnergyColor(const coreVector3 vColor);

    // 
    void __SwitchHealth(const coreUintW iIndex);
    void __SwitchColor (const coreUintW iIndex, const coreBool bWave, const coreBool bMenu);

    // 
    void __RequestMission(const coreUintW iIndex);
    void __StartMission  (const coreUintW iIndex);
    void __EndMission    (const coreBool bAnimated, const coreBool bReturn);

    // 
    void __EndExplosion(const coreBool bClear);
    void __EndAnimation(const coreFloat fTime);

    // 
    inline void __CreateChroma(const coreFloat fSpeed) {m_fChromaValue = 1.0f; m_fChromaSpeed = fSpeed;}

    // 
    cBullet* __AddRainbowBullet(const coreUintW iType, const coreInt32 iDamage, const coreFloat fSpeed, const coreVector2 vPos, const coreVector2 vDir);
};


// ****************************************************************
// Eigengrau boss class
class cEigengrauBoss final : public cBoss
{
private:
    cLodObject m_aLayer[EIGENGRAU_LAYERS];           // 
    coreObject3D m_Range;
    coreObject3D m_RangePlayer;
    
    coreObject3D m_aArrow[BOSS_PLAYERS];            // 

    cCustomEnemy m_aParasite[EIGENGRAU_PARASITES];   // 
    cCustomEnemy m_aFollower[EIGENGRAU_FOLLOWERS];   // 
    
    coreFlow m_afParasiteAlpha[EIGENGRAU_PARASITES];
    coreFlow m_afFollowerAlpha[EIGENGRAU_FOLLOWERS];

    coreFlow m_fAnimation;                           // 
    coreFlow m_fRotation;
    coreFlow m_fRotationSpeed;
    
    coreFlow  m_fRangeAnim;
    coreFlow  m_fRangeAnimPlayer;
    coreFloat m_fRangeSpeed;
    coreFloat m_fRangeSpeedPlayer;

    coreFullscreen m_Lightning;               // 
    coreFlow       m_fLightningDelay;         // 
    
    coreFloat m_fRotaSpeed;
    
    coreFloat m_fAttackSpeed;
    
    coreUint16 m_iBurstTick;
    
    coreSpline2 m_PlayerPath;

    coreSoundPtr m_pHeartSound;                       // 


public:
    cEigengrauBoss()noexcept;
    ~cEigengrauBoss()final;

    DISABLE_COPY(cEigengrauBoss)
    ASSIGN_ID_EX(802, "EIGENGRAU", COLOR_MENU_WHITE, COLOR_MENU_WHITE, coreVector2(0.0f,0.5f))
    ASSIGN_EXTRA("アイゲングラウ")

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_99.ogg";}


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __RenderOwnTop  ()final;
    void __MoveOwn       ()final;

    // 
    coreVector3 __AimAtPlayerDual(const coreUintW iIndex)const;

    // 
    cBullet* __AddBullet     (const coreInt32 iDamage, const coreFloat fSpeed, const coreVector3 vPos, const coreVector3 vDir);
    void     __AddBulletLine (const coreInt32 iDamage, const coreFloat fSpeed, const coreVector2 vLinePos, const coreVector2 vLineDir);
    void     __AddBulletBurst();

    // 
    void __PhaseChange();

    // 
    void __ResurrectParasite(const coreUintW iIndex);
    void __KillParasite     (const coreUintW iIndex);

    // 
    void __ResurrectFollower(const coreUintW iIndex);
    void __KillFollower     (const coreUintW iIndex);
};


// ****************************************************************
// Intro boss class
class cIntroBoss final : public cBoss
{
private:
    cCustomEnemy m_Blade;    // 
    cCustomEnemy m_Hilt;     // 
    cCustomEnemy m_Shield;   // 


public:
    cIntroBoss()noexcept;
    ~cIntroBoss()final;

    DISABLE_COPY(cIntroBoss)
    ASSIGN_ID_EX(9901, "SHINAI", COLOR_MENU_PURPLE, COLOR_MENU_PURPLE, coreVector2(0.0f,0.25f))
    ASSIGN_EXTRA("シナイ")

    // 
    void ResurrectIntro();

    // get object properties
    inline const coreChar* GetMusicName()const final {return "boss_00.ogg";}


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;

    // 
    coreBool __ResurrectHelperIntro(const coreUint8 iElement, const coreBool bSmooth);
};


// ****************************************************************
// 
template <typename F, typename G> void cBoss::_PhaseTimer(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreFloat fSpeed, G&& nLerpFunc, F&& nUpdateFunc)
{
    do
    {
        // 
        m_bControlAgain = false;

        // 
        ASSERT(iTimerIndex < BOSS_TIMERS)
        coreTimer&  oTimer     = m_aTimer     [iTimerIndex];
        coreUint16& iTimerLine = m_aiTimerLine[iTimerIndex];

        // 
        if(iTimerLine != iCodeLine)
        {
            iTimerLine = iCodeLine;

            // 
            oTimer.SetMaxLoops(1u);
            oTimer.Play(CORE_TIMER_PLAY_RESET);
        }

        // 
        const coreFloat fTimeBefore = nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL));
        oTimer.Update(fSpeed);
        const coreFloat fTimeAfter  = nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL));

        // 
        nUpdateFunc(fTimeAfter, fTimeBefore, !oTimer.GetStatus());
    }
    while(m_bControlAgain);
}


// ****************************************************************
// 
template <typename F, typename G> void cBoss::_PhaseTicker(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreUint16 iTicks, const coreFloat fRate, G&& nLerpFunc, F&& nUpdateFunc)
{
    do
    {
        // 
        m_bControlAgain = false;

        // 
        ASSERT(iTimerIndex < BOSS_TIMERS)
        coreTimer&  oTimer     = m_aTimer     [iTimerIndex];
        coreUint16& iTimerLine = m_aiTimerLine[iTimerIndex];

        // 
        if(iTimerLine != iCodeLine)
        {
            iTimerLine = iCodeLine;

            // 
            oTimer.SetMaxLoops(iTicks ? 1u : 0u);
            oTimer.Play(CORE_TIMER_PLAY_RESET);
        }

        if(iTicks)
        {
            // 
            const coreUint16 iTicksBefore = F_TO_UI(nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL)) * I_TO_F(iTicks));
            oTimer.Update(RoundFreq(fRate) * RCP(I_TO_F(iTicks)));
            const coreUint16 iTicksAfter  = F_TO_UI(nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL)) * I_TO_F(iTicks));

            // 
            if(iTicksBefore != iTicksAfter) nUpdateFunc(iTicksAfter - 1u, (iTicksAfter == iTicks));
        }
        else
        {
            ASSERT(r_cast<void*>(nLerpFunc) == r_cast<void*>(LERP_LINEAR))
            ASSERT(fRate <= FRAMERATE_MIN)

            // 
            if(oTimer.Update(RoundFreq(fRate))) nUpdateFunc(oTimer.GetCurLoops() - 1u, false);
        }
    }
    while(m_bControlAgain);
}


#endif // _P1_GUARD_BOSS_H_