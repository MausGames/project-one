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
#define BOSS_TIMERS   (6u)   // 
#define BOSS_COUNTERS (8u)   // 
#define BOSS_VECTORS  (8u)   // 


// ****************************************************************
// boss specific definitions
#define DHARUK_TRAILS             (3u)                                         // 
#define DHARUK_DUPLICATE_RAWS     (2u * DHARUK_TRAILS)                         // 
#define DHARUK_BOOMERANGS         (4u)                                         // 
#define DHARUK_BOOMERANGS_RAWS    (DHARUK_BOOMERANGS * (DHARUK_TRAILS + 1u))   // 
#define DHARUK_WIDTH              (0.5f)                                       // 
#define DHARUK_HEIGHT             (0.8f)                                       // 

#define TORUS_TURRETS             (4u)                                         // 
#define TORUS_GUNNERS             (4u)                                         // 
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
#define LEVIATHAN_RADIUS_INNER    (9.0f)                                       // 
#define LEVIATHAN_RAY_OFFSET(i)   ((i) ? 3.0f : 4.0f)                          // 
#define LEVIATHAN_RAY_HEIGHT      (0.1f)                                       // 
#define LEVIATHAN_RAY_SIZE        (coreVector3(0.7f,50.0f,0.7f))               // 
#define LEVIATHAN_RAY_TEXSIZE     (coreVector2(0.5f,1.5f))                     // 
#define LEVIATHAN_RAYWAVE_SIZE    (coreVector3(1.6f,5.0f,1.3f))                // 
#define LEVIATHAN_RAYWAVE_TEXSIZE (coreVector2(0.5f,0.5f))                     // 


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
    coreObject3D m_Emitter;                          // 
    coreObject3D m_aCircle[2];                       // 
    coreObject3D m_Summon;                           // 

    cCustomEnemy  m_aTurret[TORUS_TURRETS];          // 
    coreBatchList m_TurretHull;                      // 
    coreObject3D  m_aTurretHullRaw[TORUS_TURRETS];   // 

    cCustomEnemy  m_aGunner[TORUS_GUNNERS];          // 
    coreBatchList m_GunnerHull;                      // 
    coreObject3D  m_aGunnerHullRaw[TORUS_GUNNERS];   // 

    coreFlow m_fAnimation;                           // animation value
    coreFlow m_fRotationBoss;                        // 
    coreFlow m_fRotationObject;                      // 

    coreUint8 m_iTurretActive;                       // 
    coreUint8 m_iGunnerActive;                       // 
    coreUint8 m_iGunnerMove;                         // 


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

    coreFlow m_fAnimation;                          // animation value
    coreFlow m_fMovement;                           // 


public:
    cLeviathanBoss()noexcept;

    DISABLE_COPY(cLeviathanBoss)
    ASSIGN_ID_EX(203, "LEVIATHAN", COLOR_MENU_CYAN)


private:
    // execute own routines
    void __ResurrectOwn ()final;
    void __KillOwn      (const coreBool bAnimated)final;
    void __RenderOwnOver()final;
    void __MoveOwn      ()final;

    // 
    void __EnableRay      (const coreUintW iIndex);
    void __DisableRay     (const coreUintW iIndex, const coreBool bAnimated);
    void __CreateOverdrive(const coreUintW iIndex, const coreVector3 vIntersect, const coreFloat fTime, const coreBool bGround);

    // 
    void __UpdateHealth();
    void __RefreshHealth();

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
public:
    cTigerBoss()noexcept;

    DISABLE_COPY(cTigerBoss)
    ASSIGN_ID_EX(302, "TIGER MK-III", COLOR_MENU_YELLOW)


private:
    // execute own routines
    void __MoveOwn()final;
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
public:
    cMessierBoss()noexcept;

    DISABLE_COPY(cMessierBoss)
    ASSIGN_ID_EX(403, "MESSIER 87", COLOR_MENU_MAGENTA)


private:
    // execute own routines
    void __MoveOwn()final;
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
public:
    cCholBoss()noexcept;

    DISABLE_COPY(cCholBoss)
    ASSIGN_ID_EX(503, "CHOL", COLOR_MENU_ORANGE)


private:
    // execute own routines
    void __MoveOwn()final;
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
public:
    cZerothBoss()noexcept;

    DISABLE_COPY(cZerothBoss)
    ASSIGN_ID_EX(603, "???", COLOR_MENU_BLUE)


private:
    // execute own routines
    void __MoveOwn()final;
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
public:
    cGemingaBoss()noexcept;

    DISABLE_COPY(cGemingaBoss)
    ASSIGN_ID_EX(702, "GEMINGA", COLOR_MENU_RED)


private:
    // execute own routines
    void __MoveOwn()final;
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
public:
    cProjectOneBoss()noexcept;

    DISABLE_COPY(cProjectOneBoss)
    ASSIGN_ID(801, "PROJECT ONE")

    // 
    inline coreVector3 GetColor()const final {return coreVector3(0.0f,0.0f,0.0f);}


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
public:
    cEigengrauBoss()noexcept;

    DISABLE_COPY(cEigengrauBoss)
    ASSIGN_ID_EX(802, "EIGENGRAU", coreVector3(0.0f,0.0f,0.0f))


private:
    // execute own routines
    void __MoveOwn()final;
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
        oTimer.Update(fRate * RCP(I_TO_F(iTicks)));
        const coreUint16 iTicksAfter  = F_TO_UI(nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL)) * I_TO_F(iTicks));

        // 
        if(iTicksBefore != iTicksAfter) nUpdateFunc(iTicksAfter - 1u, (iTicksAfter == iTicks));
    }
    else
    {
        ASSERT(r_cast<void*>(nLerpFunc) == r_cast<void*>(LERP_LINEAR))
        ASSERT(fRate <= FRAMERATE_MIN)

        // 
        if(oTimer.Update(fRate)) nUpdateFunc(oTimer.GetCurLoops() - 1u, false);
    }
}


#endif // _P1_GUARD_BOSS_H_