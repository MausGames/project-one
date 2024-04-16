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


// ****************************************************************
// boss definitions
#define BOSS_TIMERS   (8u)    // 
#define BOSS_COUNTERS (10u)   // 
#define BOSS_VECTORS  (10u)   // 


// ****************************************************************
// boss specific definitions
#define DHARUK_TRAILS             (3u)                                         // 
#define DHARUK_DUPLICATE_RAWS     (2u * DHARUK_TRAILS)                         // 
#define DHARUK_BOOMERANGS         (4u)                                         // 
#define DHARUK_BOOMERANGS_RAWS    (DHARUK_BOOMERANGS * (DHARUK_TRAILS + 1u))   // 
#define DHARUK_WIDTH              (0.5f)                                       // 
#define DHARUK_HEIGHT             (0.8f)                                       // 

#define TORUS_TURRETS             (6u)                                         // 
#define TORUS_GUNNERS             (6u)                                         // 
#define TORUS_CHARGERS            (4u)                                         // 
#define TORUS_DRIVERS             (6u)                                         // 
#define TORUS_WAVERS              (4u)                                         // 
#define TORUS_BOSS_ROTATION       (1.2f)                                       // 
#define TORUS_TURRET_SPEED        (-0.2f)                                      // 
#define TORUS_GUNNER_SPEED        (0.2f)                                       // 

#define NAUTILUS_ATTACH_DIST      (-10.0f)                                     // 
#define NAUTILUS_INK_TIME         (10.0f)                                      // 
#define NAUTILUS_INK_SPEED        (1.65f)                                      // 

#define LEVIATHAN_PARTS           (5u)                                         // 
#define LEVIATHAN_PARTS_BODIES    (LEVIATHAN_PARTS - 2u)                       // 
#define LEVIATHAN_RAYS            (LEVIATHAN_PARTS)                            // 
#define LEVIATHAN_RAYS_RAWS       (2u * LEVIATHAN_RAYS)                        // 
#define LEVIATHAN_RADIUS_OUTER    (FOREGROUND_AREA.x * 0.8f)                   // 
#define LEVIATHAN_RADIUS_INNER    (10.0f)                                      // 
#define LEVIATHAN_RAY_OFFSET(i)   ((i) ? 3.6f : 4.8f)                          // 
#define LEVIATHAN_RAY_HEIGHT      (0.2f)                                       // 
#define LEVIATHAN_RAY_SIZE        (coreVector3(0.9f,50.0f,0.9f))               // 
#define LEVIATHAN_RAY_TEXSIZE     (coreVector2(0.5f,1.5f))                     // 
#define LEVIATHAN_RAYWAVE_SIZE    (coreVector3(2.0f,7.0f,2.0f))                // 
#define LEVIATHAN_RAYWAVE_TEXSIZE (coreVector2(0.5f,0.5f))                     // 
#define LEVIATHAN_TILES           (20u)                                        // 

#define TIGER_SIDES               (4u)                                         // 
#define TIGER_STINGS              (32u * TIGER_SIDES)                          // 
#define TIGER_STINGS_SIDE         (32u)                                        // 
#define TIGER_WEAPONS             (5u)                                         // 
#define TIGER_SUBS                (3u)                                         // 
#define TIGER_DAMAGE              (70)                                         // 

#define MESSIER_RINGS             (3u)                                         // 
#define MESSIER_ENEMIES           (8u)                                         // 

#define CHOL_WINGS                (4u)                                         // 

#define ZEROTH_LIMBS              (6u)                                         // 
#define ZEROTH_ICES               (2u)                                         // 

#define PROJECTONE_SHIELDS        (HELPERS - 1u)                               // 


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
#define PHASE_FINISHED                  (__bEnd)


// ****************************************************************
// 
#define LERP_LINEAR     (&LERP  <coreFloat>)
#define LERP_SMOOTH     (&LERPS <coreFloat>)
#define LERP_SMOOTH_REV (&LerpSmoothRev<coreFloat>)
#define LERP_BREAK      (&LERPB <coreFloat>)
#define LERP_BREAK_REV  (&LERPBR<coreFloat>)
#define LERP_HERMITE3   (&LERPH3<coreFloat>)
#define LERP_HERMITE5   (&LERPH5<coreFloat>)


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

    static coreVector2 s_vPositionPoint;      // 


public:
    cBoss()noexcept;
    virtual ~cBoss()override;

    DISABLE_COPY(cBoss)
    ENABLE_ID_EX

    // 
    void ChangePhase(const coreUint8 iPhase);

    // 
    void StorePosition(const coreVector2 vPos);
    void StorePosition();
    void StoreRotation(const coreVector3 vDir, const coreVector3 vOri);
    void StoreRotation();

    // 
    inline const coreUint8& GetPhase()const {return m_iPhase;}


protected:
    // 
    void _StartBoss();
    void _EndBoss(const coreBool bAnimated);

    // 
    void _UpdateBoss();

    // 
    template <typename F, typename G> void _PhaseTimer (const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreFloat  fSpeed,                        G&& nLerpFunc, F&& nUpdateFunc);   // [](const coreFloat x, const coreFloat y, const coreFloat s) -> coreFloat, [](const coreFloat  fTime, const coreFloat fTimeBefore, const coreBool __bEnd) -> void
    template <typename F, typename G> void _PhaseTicker(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreUint16 iTicks, const coreFloat fRate, G&& nLerpFunc, F&& nUpdateFunc);   // [](const coreFloat x, const coreFloat y, const coreFloat s) -> coreFloat, [](const coreUint16 iTick,                              const coreBool __bEnd) -> void
};


// ****************************************************************
// Dharuk boss class
class cDharukBoss final : public cBoss
{
private:
    cCustomEnemy  m_Duplicate;                               // 
    coreBatchList m_DuplicateTrail;                          // 
    coreObject3D  m_aDuplicateRaw[DHARUK_DUPLICATE_RAWS];    // 

    coreBatchList m_Boomerang;                               // 
    coreBatchList m_BoomerangTrail;                          // 
    coreObject3D  m_aBoomerangRaw[DHARUK_BOOMERANGS_RAWS];   // 

    coreObject3D m_Summon;                                   // 

    coreUint8 m_iPackedDir;                                  // 
    coreFlow  m_fAnimation;                                  // animation value


public:
    cDharukBoss()noexcept;

    DISABLE_COPY(cDharukBoss)
    ASSIGN_ID_EX(101, "DHARUK", coreVector3(0.0f,0.0f,0.0f))


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
    void __DisableSummon();

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
    coreUint8 m_iGunnerActive;                         // 
    coreUint8 m_iGunnerMove;                           // 
    coreUint8 m_iChargerActive;                        // 
    coreUint8 m_iDriverActive;                         // 
    coreUint8 m_iWaverActive;                          // 

    coreUint8 m_iDecalState;                           // 


public:
    cTorusBoss()noexcept;

    DISABLE_COPY(cTorusBoss)
    ASSIGN_ID_EX(102, "TORUS", COLOR_MENU_GREEN)


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __RenderOwnOver ()final;
    void __MoveOwn       ()final;

    // 
    void __EnableSummon (const coreVector2 vPosition, const coreVector3 vColor);
    void __DisableSummon();

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

    DISABLE_COPY(cVausBoss)
    ASSIGN_ID_EX(103, "VAUS", coreVector3(0.0f,0.0f,0.0f))


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

    DISABLE_COPY(cNautilusBoss)
    ASSIGN_ID_EX(201, "NAUTILUS", coreVector3(0.0f,0.0f,0.0f))


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
private:
    cCustomEnemy m_Top;                 // 
    cCustomEnemy m_Bottom;              // 

    cCustomEnemy m_Tooth[4];            // 

    coreFloat m_fMouthAngle;            // 

    coreSpline2 m_ChangePath;           // 

    coreTexturePtr m_apStomachTex[4];   // 


public:
    cAmemasuBoss()noexcept;

    DISABLE_COPY(cAmemasuBoss)
    ASSIGN_ID_EX(202, "AMEMASU", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;
};


// ****************************************************************
// Leviathan boss class
class cLeviathanBoss final : public cBoss
{
private:
    cCustomEnemy m_Head;                            // 
    cCustomEnemy m_aBody[LEVIATHAN_PARTS_BODIES];   // 
    cCustomEnemy m_Tail;                            // 

    coreBatchList m_Ray;                            // 
    coreBatchList m_RayWave;                        // 
    coreObject3D  m_aRayRaw[LEVIATHAN_RAYS_RAWS];   // 

    coreFlow   m_afRayTime[LEVIATHAN_RAYS];         // 
    coreUint16 m_iDecalState;                       // 

    coreVector2 m_avSwimDir  [LEVIATHAN_PARTS];     // 
    coreUint8   m_aiSwimCount[LEVIATHAN_PARTS];     // 

    coreFlow m_fAnimation;                          // animation value
    coreFlow m_fMovement;                           // 


public:
    cLeviathanBoss()noexcept;

    DISABLE_COPY(cLeviathanBoss)
    ASSIGN_ID_EX(203, "LEVIATHAN", COLOR_MENU_CYAN)


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __RenderOwnOver ()final;
    void __MoveOwn       ()final;

    // 
    void __EnableRay      (const coreUintW iIndex, const coreBool bAnimated);
    void __DisableRay     (const coreUintW iIndex, const coreBool bAnimated);
    void __CreateOverdrive(const coreUintW iIndex, const coreVector3 vIntersect, const coreFloat fTime, const coreBool bGround);

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

    DISABLE_COPY(cUrticaBoss)
    ASSIGN_ID_EX(301, "URTICA", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Tiger boss class
class cTigerBoss final : public cBoss
{
private:
    cCustomEnemy m_Track;                        // 
    cCustomEnemy m_aWeapon   [TIGER_SUBS];       // 
    cCustomEnemy m_aWeaponOld[TIGER_SUBS];       // 

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

    coreVector2 m_vGroundPos;                    // 

    coreFlow m_fAnimation;                       // 


public:
    cTigerBoss()noexcept;

    DISABLE_COPY(cTigerBoss)
    ASSIGN_ID_EX(302, "TIGER MK-III", COLOR_MENU_YELLOW)


private:
    // execute own routines
    void __ResurrectOwn ()final;
    void __KillOwn      (const coreBool bAnimated)final;
    void __RenderOwnOver()final;
    void __MoveOwn      ()final;

    // 
    void __EnableStings (const coreUintW iIndex);
    void __DisableStings(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void __EnableWind (const coreVector2 vDir);
    void __DisableWind();

    // 
    void __SwitchWeapon(const coreUintW iType);

    // 
    void __CreateTrail(const coreUintW iIndex, const coreVector3 vIntersect);
};


// ****************************************************************
// Lucifer boss class
class cLuciferBoss final : public cBoss
{
public:
    cLuciferBoss()noexcept;

    DISABLE_COPY(cLuciferBoss)
    ASSIGN_ID_EX(303, "L.U.C.I.F.E.R.", coreVector3(0.0f,0.0f,0.0f))


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

    DISABLE_COPY(cQuaternionBoss)
    ASSIGN_ID_EX(401, "QUATERNION", coreVector3(0.0f,0.0f,0.0f))


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

    DISABLE_COPY(cSarosBoss)
    ASSIGN_ID_EX(402, "SAROS", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Messier boss class
class cMessierBoss final : public cBoss
{
private:
    coreObject3D m_aRing[MESSIER_RINGS];   // 
    coreFlow     m_fRingTime;              // 

    coreFloat m_fTimeFactor;               // 

    coreFlow m_fShootTime;                 // 

    coreFlow m_fAnimation;                 // 
    coreFlow m_fRotation;                  // 


public:
    cMessierBoss()noexcept;

    DISABLE_COPY(cMessierBoss)
    ASSIGN_ID_EX(403, "MESSIER 87", COLOR_MENU_MAGENTA)


private:
    // execute own routines
    void __ResurrectOwn ()final;
    void __KillOwn      (const coreBool bAnimated)final;
    void __RenderOwnOver()final;
    void __MoveOwn      ()final;

    // 
    void __EnableRings ();
    void __DisableRings(const coreBool bAnimated);

    // 
    void     __AddBullet  (const coreFloat fSpeed, const coreVector2 vPosition, const coreVector2 vDirection);
    coreBool __PhaseTicker(const coreFloat fRate);
};


// ****************************************************************
// Tartarus boss class
class cTartarusBoss final : public cBoss
{
public:
    cTartarusBoss()noexcept;

    DISABLE_COPY(cTartarusBoss)
    ASSIGN_ID_EX(501, "TARTARUS", coreVector3(0.0f,0.0f,0.0f))


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

    DISABLE_COPY(cPhalarisBoss)
    ASSIGN_ID_EX(502, "PHALARIS", coreVector3(0.0f,0.0f,0.0f))


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
    coreFlow    m_afWingThrow [CHOL_WINGS];   // 
    coreVector3 m_avWingReturn[CHOL_WINGS];   // 

    coreFloat m_fTilt;                        // 
    coreFloat m_fFlap;                        // 

    coreFlow m_fAnimation;                    // 
    coreFlow m_fResurrection;                 // 


public:
    cCholBoss()noexcept;

    DISABLE_COPY(cCholBoss)
    ASSIGN_ID_EX(503, "CHOL", COLOR_MENU_ORANGE)


private:
    // execute own routines
    void __ResurrectOwn  ()final;
    void __KillOwn       (const coreBool bAnimated)final;
    void __RenderOwnUnder()final;
    void __MoveOwn       ()final;

    // 
    void __EnableFire ();
    void __DisableFire(const coreBool bAnimated);

    // 
    inline void __ChangeWing      (const coreUintW iIndex, const coreUint8 iState) {ASSERT(iIndex < CHOL_WINGS) m_aiWingState[iIndex] = iState; m_afWingThrow[iIndex] = 0.0f;}
    inline void __ChangeWingThrow (const coreUintW iIndex)                         {this->__ChangeWing(iIndex,  1u);}
    inline void __ChangeWingReturn(const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 11u);}
    inline void __ChangeWingIntro (const coreUintW iIndex)                         {this->__ChangeWing(iIndex, 21u);}

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

    DISABLE_COPY(cFenrirBoss)
    ASSIGN_ID_EX(601, "FENRIR", coreVector3(0.0f,0.0f,0.0f))


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

    DISABLE_COPY(cShelobBoss)
    ASSIGN_ID_EX(602, "SHELOB", coreVector3(0.0f,0.0f,0.0f))


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

    cCustomEnemy m_aIce[ZEROTH_ICES];           // 

    coreFlow m_fAnimation;                      // 


public:
    cZerothBoss()noexcept;

    DISABLE_COPY(cZerothBoss)
    ASSIGN_ID_EX(603, "???", COLOR_MENU_BLUE)

    // 
    void ResurrectIntro();


private:
    // execute own routines
    void __ResurrectOwn ()final;
    void __KillOwn      (const coreBool bAnimated)final;
    void __RenderOwnOver()final;
    void __MoveOwn      ()final;

    // 
    void __EnableLaser (const coreVector2 vPosition, const coreVector2 vDirection);
    void __DisableLaser(const coreBool bAnimated);

    // 
    void __CreateCube(const coreVector2 vPosition, const coreVector2 vDirection);

    // 
    void __SetLimbValue(const coreUintW iIndex, const coreFloat fValue);
};


// ****************************************************************
// Orlac boss class
class cOrlacBoss final : public cBoss
{
public:
    cOrlacBoss()noexcept;

    DISABLE_COPY(cOrlacBoss)
    ASSIGN_ID_EX(701, "ORLAC", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Geminga boss class
class cGemingaBoss final : public cBoss
{
private:
    cCustomEnemy m_InsideTop;           // 
    cCustomEnemy m_InsideBottom;        // 
    cCustomEnemy m_Top;                 // 
    cCustomEnemy m_Bottom;              // 

    cCustomEnemy m_Tooth[4];            // 

    coreFloat m_fMouthAngle;            // 

    coreSpline2 m_ChangePath;           // 

    coreTexturePtr m_apStomachTex[4];   // 


public:
    cGemingaBoss()noexcept;

    DISABLE_COPY(cGemingaBoss)
    ASSIGN_ID_EX(702, "GEMINGA", COLOR_MENU_RED)


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;
};


// ****************************************************************
// Nagual boss class
class cNagualBoss final : public cBoss
{
public:
    cNagualBoss()noexcept;

    DISABLE_COPY(cNagualBoss)
    ASSIGN_ID_EX(703, "NAGUAL", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// Project One boss class
class cProjectOneBoss final : public cBoss
{
private:
    cCustomEnemy m_aShield[PROJECTONE_SHIELDS];   // 

    coreSpline2 m_HelperPath;                     // 
    coreUint8   m_iHelperState;                   // 

    coreVector3 m_vLevelColor;                    // 

    coreFlow m_fAnimation;                        // 


public:
    cProjectOneBoss()noexcept;

    DISABLE_COPY(cProjectOneBoss)
    ASSIGN_ID(801, "PROJECT ONE")

    // 
    inline coreVector3 GetColor()const final {return m_vLevelColor;}


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;
};


// ****************************************************************
// Eigengrau boss class
class cEigengrauBoss final : public cBoss
{
private:
    
public:
    cEigengrauBoss()noexcept;

    DISABLE_COPY(cEigengrauBoss)
    ASSIGN_ID_EX(802, "EIGENGRAU", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;
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

    DISABLE_COPY(cIntroBoss)
    ASSIGN_ID_EX(9901, "SHINAI", COLOR_MENU_PURPLE)


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;
};


// ****************************************************************
// 
template <typename F, typename G> void cBoss::_PhaseTimer(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreFloat fSpeed, G&& nLerpFunc, F&& nUpdateFunc)
{
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


// ****************************************************************
// 
template <typename F, typename G> void cBoss::_PhaseTicker(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreUint16 iTicks, const coreFloat fRate, G&& nLerpFunc, F&& nUpdateFunc)
{
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


#endif // _P1_GUARD_BOSS_H_