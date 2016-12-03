//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BOSS_H_
#define _P1_GUARD_BOSS_H_

// TODO: boomerangs of Dharuk may generate double-hits because of rotating box collision (when moving away from it), bitfield with reset (player-num)
// TODO: remove warning 4738
// TODO: boss0101, both boss and duplicate should have own trace
// TODO: boss0101, correct outline sides


// ****************************************************************
// boss definitions
#define BOSS_TIMERS   (4u)   // 
#define BOSS_COUNTERS (8u)   // 
#define BOSS_VECTORS  (6u)   // 


// ****************************************************************
// boss specific definitions
#define DHARUK_BOOMERANGS  (4u)                                         // 
#define DHARUK_TRAILS      (3u)                                         // 
#define DHARUK_RAWS        (DHARUK_BOOMERANGS * (DHARUK_TRAILS + 1u))   // 
#define DHARUK_WIDTH       (0.667f)                                     // 
#define DHARUK_HEIGHT      (0.8f)                                       // 

#define TORUS_RAY_SIZE     (coreVector3(0.7f,50.0f,0.7f))   // 
#define TORUS_RAY_TEXSIZE  (coreVector2(0.5f,1.5f))         // 
#define TORUS_RAY_OFFSET   (8.0f)                           // 
#define TORUS_RAYWAVE_SIZE (coreVector3(1.6f,5.0f,1.3f))    // 

#define VAUS_SCOUTS_TOTAL  (DEFINED(_CORE_DEBUG_) ? 16 : 80)   // 
#define VAUS_SCOUTS_X      (8u)                                // 
#define VAUS_SCOUTS_Y      (2u)                                // 
#define VAUS_SHOTS         (10u)                               // 


// ****************************************************************
// phase management macros
#define PHASE_CONTROL_TIMER(a,b,c)  this->_PhaseTimer (a, __LINE__, b, c, [&](const coreFloat  fTime, const coreFloat fTimeBefore, const coreBool __bEnd)
#define PHASE_CONTROL_TICKER(a,b,c) this->_PhaseTicker(a, __LINE__, b, c, [&](const coreUint16 iTick,                              const coreBool __bEnd)
#define PHASE_CONTROL_PAUSE(a,b)    PHASE_CONTROL_TICKER(a, 1u, b)

#define PHASE_TIME_POINT(t)         (InBetween((t), fTimeBefore, fTime))
#define PHASE_TIME_BEFORE(t)        (fTime <  (t))
#define PHASE_TIME_AFTER(t)         (fTime >= (t))
#define PHASE_TIME_BETWEEN(t,u)     (InBetween(fTime, (t), (u)))
#define PHASE_BEGINNING             (PHASE_TIME_POINT(0.0f))

#define PHASE_RESET(i)              {m_aTimer[i].Stop(); m_aiTimerLine[i] = 0u;}
#define PHASE_FINISHED              (__bEnd)

#define LERP_LINEAR                 (&LERP <coreFloat>)
#define LERP_SMOOTH                 (&LERPS<coreFloat>)
#define LERP_SMOOTH_REV             (&LerpSmoothRev)
#define LERP_BREAK                  (&LERPB<coreFloat>)
#define LERP_BREAK_REV              (&LerpBreakRev)


// ****************************************************************
// boss entity interface
class INTERFACE cBoss : public cEnemy
{
protected:
    coreTimer  m_aTimer     [BOSS_TIMERS];    // 
    coreUint16 m_aiTimerLine[BOSS_TIMERS];    // 

    coreInt16   m_aiCounter[BOSS_COUNTERS];   // 
    coreVector3 m_avVector [BOSS_VECTORS];    // 

    coreUint8 m_iPhase;                       // 
    coreUint8 m_iLevel;                       // 


public:
    cBoss()noexcept;
    virtual ~cBoss()override;

    DISABLE_COPY(cBoss)
    ENABLE_ID

    // 
    inline const coreUint8& GetPhase()const {return m_iPhase;}
    inline const coreUint8& GetLevel()const {return m_iLevel;}


protected:
    // 
    template <typename F, typename G> void _PhaseTimer (const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreFloat  fSpeed, G&& nLerpFunc,         F&& nUpdateFunc);   // [](const coreFloat fTime, const coreFloat fTimeBefore, const coreBool __bEnd) -> void, [](const coreFloat x, const coreFloat y, const coreFloat s) -> coreFloat
    template <typename F>             void _PhaseTicker(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreUint16 iTicks, const coreFloat fRate, F&& nUpdateFunc);   // [](const coreUint16 iTick, const coreBool __bEnd) -> void
};


// ****************************************************************
// Dharuk boss class
class cDharukBoss final : public cBoss
{
private:
    coreObject3D m_Duplicate;                        // 
    coreObject3D m_aDuplicateTrail[DHARUK_TRAILS];   // 

    coreBatchList m_Boomerang;                       // 
    coreBatchList m_BoomerangTrail;                  // 
    coreObject3D  m_aBoomerangRaw[DHARUK_RAWS];      // 

    coreUint8 m_iPackedDir;                          // 
    coreFlow  m_fAnimation;                          // animation value


public:
    cDharukBoss()noexcept;

    DISABLE_COPY(cDharukBoss)
    ASSIGN_ID(101, "Dharuk")


private:
    // execute own routines
    void __ResurrectOwn   ()final;
    void __KillOwn        (const coreBool bAnimated)final;
    void __RenderOwnUnder ()final;
    void __RenderOwnAttack()final;
    void __MoveOwn        ()final;

    // 
    coreVector2 __RepeatPosition  (const coreVector2& vPosition, const coreFloat fThreshold, coreBool* OUTPUT pbChange);
    void        __EncodeDirection (const coreUintW iIndex, const coreVector2& vDirection);
    coreVector2 __DecodeDirection (const coreUintW iIndex);
    void        __EnableBoomerang (const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection);
    void        __DisableBoomerang(const coreUintW iIndex, const coreBool bAnimated);
};


// ****************************************************************
// Torus boss class
class cTorusBoss final : public cBoss
{
private:
    coreObject3D m_aRay    [3];   // 
    coreObject3D m_aRayWave[3];   // 

    coreObject3D m_Emitter;       // 
    coreObject3D m_aCircle[2];    // 

    coreFlow m_fAnimation;        // animation value
    coreFlow m_fRotation;         // 


public:
    cTorusBoss()noexcept;

    DISABLE_COPY(cTorusBoss)
    ASSIGN_ID(102, "Torus")

    // 
    void Render()final;


private:
    // execute own routines
    void __ResurrectOwn   ()final;
    void __KillOwn        (const coreBool bAnimated)final;
    void __RenderOwnAttack()final;
    void __MoveOwn        ()final;

    // 
    coreVector3 __GetRotaDirection(const coreFloat fBaseAngle);
    void        __SetRotaAttack   (const coreInt16 iType, const coreBool bAnimated);
    void        __EnableRay       (const coreUintW iIndex);
    void        __DisableRay      (const coreUintW iIndex);
    void        __CreateOverdrive (const coreUintW iIndex, const coreVector3& vIntersect, const coreFloat fTime, const coreBool bGround);
};


// ****************************************************************
// Vaus boss class
class cVausBoss final : public cBoss
{
private:
    coreObject3D m_aRay[3];         // 

    cCustomEnemy m_aCompanion[2];   // 
    coreUint8    m_iScoutOrder;     // 


public:
    cVausBoss()noexcept;

    DISABLE_COPY(cVausBoss)
    ASSIGN_ID(103, "Vaus")


private:
    // execute own routines
    void __ResurrectOwn   ()final;
    void __KillOwn        (const coreBool bAnimated)final;
    void __RenderOwnAttack()final;
    void __MoveOwn        ()final;

    // 
    coreBool __ExecuteCompanionAttack(const coreUintW iType, const coreFloat fTime);
};


// ****************************************************************
// 
class cNautilusBoss final : public cBoss
{
public:
    cNautilusBoss()noexcept;

    DISABLE_COPY(cNautilusBoss)
    ASSIGN_ID(201, "Nautilus")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cAmemasuBoss final : public cBoss
{
public:
    cAmemasuBoss()noexcept;

    DISABLE_COPY(cAmemasuBoss)
    ASSIGN_ID(202, "Amemasu")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cLeviathanBoss final : public cBoss
{
public:
    cLeviathanBoss()noexcept;

    DISABLE_COPY(cLeviathanBoss)
    ASSIGN_ID(203, "Leviathan")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUrticaBoss final : public cBoss
{
public:
    cUrticaBoss()noexcept;

    DISABLE_COPY(cUrticaBoss)
    ASSIGN_ID(301, "Urtica")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cTigerBoss final : public cBoss
{
public:
    cTigerBoss()noexcept;

    DISABLE_COPY(cTigerBoss)
    ASSIGN_ID(302, "Tiger MK-III")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cLuciferBoss final : public cBoss
{
public:
    cLuciferBoss()noexcept;

    DISABLE_COPY(cLuciferBoss)
    ASSIGN_ID(303, "Lucifer")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cQuaternionBoss final : public cBoss
{
public:
    cQuaternionBoss()noexcept;

    DISABLE_COPY(cQuaternionBoss)
    ASSIGN_ID(401, "Quaternion")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cSarosBoss final : public cBoss
{
public:
    cSarosBoss()noexcept;

    DISABLE_COPY(cSarosBoss)
    ASSIGN_ID(402, "Saros")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cMessierBoss final : public cBoss
{
public:
    cMessierBoss()noexcept;

    DISABLE_COPY(cMessierBoss)
    ASSIGN_ID(403, "Messier 87")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cTartarusBoss final : public cBoss
{
public:
    cTartarusBoss()noexcept;

    DISABLE_COPY(cTartarusBoss)
    ASSIGN_ID(501, "Tartarus")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cPhalarisBoss final : public cBoss
{
public:
    cPhalarisBoss()noexcept;

    DISABLE_COPY(cPhalarisBoss)
    ASSIGN_ID(502, "Phalaris")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cCholBoss final : public cBoss
{
public:
    cCholBoss()noexcept;

    DISABLE_COPY(cCholBoss)
    ASSIGN_ID(503, "Chol")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cFenrirBoss final : public cBoss
{
public:
    cFenrirBoss()noexcept;

    DISABLE_COPY(cFenrirBoss)
    ASSIGN_ID(601, "Fenrir")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cShelobBoss final : public cBoss
{
public:
    cShelobBoss()noexcept;

    DISABLE_COPY(cShelobBoss)
    ASSIGN_ID(602, "Shelob")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cZerothBoss final : public cBoss
{
public:
    cZerothBoss()noexcept;

    DISABLE_COPY(cZerothBoss)
    ASSIGN_ID(603, "?")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cOrlacBoss final : public cBoss
{
public:
    cOrlacBoss()noexcept;

    DISABLE_COPY(cOrlacBoss)
    ASSIGN_ID(701, "Orlac")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cGemingaBoss final : public cBoss
{
public:
    cGemingaBoss()noexcept;

    DISABLE_COPY(cGemingaBoss)
    ASSIGN_ID(702, "Geminga")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cNagualBoss final : public cBoss
{
public:
    cNagualBoss()noexcept;

    DISABLE_COPY(cNagualBoss)
    ASSIGN_ID(703, "Nagual")


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
    ASSIGN_ID(9901, "Project One")


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;
};


#if defined(_CORE_MSVC_) && !defined(_CORE_X64_)
    #pragma warning(push)
    #pragma warning(disable : 4738)
#endif


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
    const coreFloat fTime       = nLerpFunc(0.0f, 1.0f, oTimer.GetValue(CORE_TIMER_GET_NORMAL));

    // 
    nUpdateFunc(fTime, fTimeBefore, !oTimer.GetStatus());
}


// ****************************************************************
// 
template <typename F> void cBoss::_PhaseTicker(const coreUintW iTimerIndex, const coreUint16 iCodeLine, const coreUint16 iTicks, const coreFloat fRate, F&& nUpdateFunc)
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
        oTimer.SetMaxLoops(0u);
        oTimer.Play(CORE_TIMER_PLAY_RESET);
    }

    // 
    if(oTimer.Update(fRate))
    {
        // 
        if((oTimer.GetCurLoops() >= iTicks) && iTicks)
            oTimer.Pause();

        // 
        nUpdateFunc(oTimer.GetCurLoops()-1u, !oTimer.GetStatus());
    }
}


#if defined(_CORE_MSVC_) && !defined(_CORE_X64_)
    #pragma warning(pop)
#endif


#endif // _P1_GUARD_BOSS_H_