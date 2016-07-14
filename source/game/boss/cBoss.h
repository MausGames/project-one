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

// TODO: boomerangs of Crossfield may generate double-hits because of rotating box collision (when moving away from it), bitfield with reset (player-num)
// TODO: remove warning 4738
// TODO: boss0101, both boss and duplicate should have own trace
// TODO: boss0101, correct outline sides


// ****************************************************************
// boss definitions
#define BOSS_TIMERS   (4u)   // 
#define BOSS_COUNTERS (8u)   // 
#define BOSS_VECTORS  (4u)   // 


// ****************************************************************
// boss specific definitions
#define CROSSFIELD_BOOMERANGS (4u)                                                 // 
#define CROSSFIELD_TRAILS     (3u)                                                 // 
#define CROSSFIELD_RAWS       (CROSSFIELD_BOOMERANGS * (CROSSFIELD_TRAILS + 1u))   // 
#define CROSSFIELD_WIDTH      (0.667f)                                             // 
#define CROSSFIELD_HEIGHT     (0.8f)                                               // 

#define TORUS_RAY_SIZE        (coreVector3(0.7f,50.0f,0.7f))   // 
#define TORUS_RAY_TEXSIZE     (coreVector2(0.5f, 1.5f))        // 
#define TORUS_RAY_OFFSET      (8.0f)                           // 
#define TORUS_RAYWAVE_SIZE    (coreVector3(1.6f, 5.0f,1.3f))   // 

#define VAUS_SCOUTS_TOTAL     (DEFINED(_CORE_DEBUG_) ? 16 : 80)   // 
#define VAUS_SCOUTS_X         (8u)                                // 
#define VAUS_SCOUTS_Y         (2u)                                // 


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

#define LERP_LINEAR    (&LERP <coreFloat>)
#define LERP_SMOOTH    (&LERPS<coreFloat>)
#define LERP_BREAK     (&LERPB<coreFloat>)
#define LERP_BREAK_REV (&LerpBreakRev)


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
// Crossfield boss class
class cCrossfieldBoss final : public cBoss
{
private:
    coreObject3D m_Duplicate;                            // 
    coreObject3D m_aDuplicateTrail[CROSSFIELD_TRAILS];   // 

    coreBatchList m_Boomerang;                           // 
    coreBatchList m_BoomerangTrail;                      // 
    coreObject3D  m_aBoomerangRaw[CROSSFIELD_RAWS];      // 

    coreUint8 m_iPackedDir;                              // 
    coreFlow  m_fAnimation;                              // animation value


public:
    cCrossfieldBoss()noexcept;

    DISABLE_COPY(cCrossfieldBoss)
    ASSIGN_ID(101, "Crossfield")


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
    cCustomEnemy m_aCompanion[2];   // 
    coreUint8    m_iScoutOrder;     // 


public:
    cVausBoss()noexcept;

    DISABLE_COPY(cVausBoss)
    ASSIGN_ID(103, "Vaus")


private:
    // execute own routines
    void __ResurrectOwn()final;
    void __KillOwn     (const coreBool bAnimated)final;
    void __MoveOwn     ()final;

    // 
    coreBool __ExecuteCompanionAttack(const coreUintW iType, const coreFloat fTime);
};


// ****************************************************************
// 
class cUnknown0201Boss final : public cBoss
{
public:
    cUnknown0201Boss()noexcept;

    DISABLE_COPY(cUnknown0201Boss)
    ASSIGN_ID(201, "Unknown0201")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0202Boss final : public cBoss
{
public:
    cUnknown0202Boss()noexcept;

    DISABLE_COPY(cUnknown0202Boss)
    ASSIGN_ID(202, "Unknown0202")


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
class cUnknown0301Boss final : public cBoss
{
public:
    cUnknown0301Boss()noexcept;

    DISABLE_COPY(cUnknown0301Boss)
    ASSIGN_ID(301, "Unknown0301")


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
class cUnknown0303Boss final : public cBoss
{
public:
    cUnknown0303Boss()noexcept;

    DISABLE_COPY(cUnknown0303Boss)
    ASSIGN_ID(303, "Unknown0303")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0401Boss final : public cBoss
{
public:
    cUnknown0401Boss()noexcept;

    DISABLE_COPY(cUnknown0401Boss)
    ASSIGN_ID(401, "Unknown0401")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0402Boss final : public cBoss
{
public:
    cUnknown0402Boss()noexcept;

    DISABLE_COPY(cUnknown0402Boss)
    ASSIGN_ID(402, "Unknown0402")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0403Boss final : public cBoss
{
public:
    cUnknown0403Boss()noexcept;

    DISABLE_COPY(cUnknown0403Boss)
    ASSIGN_ID(403, "Unknown0403")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0501Boss final : public cBoss
{
public:
    cUnknown0501Boss()noexcept;

    DISABLE_COPY(cUnknown0501Boss)
    ASSIGN_ID(501, "Unknown0501")


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
class cUnknown0601Boss final : public cBoss
{
public:
    cUnknown0601Boss()noexcept;

    DISABLE_COPY(cUnknown0601Boss)
    ASSIGN_ID(601, "Unknown0601")


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
class cUnknown0603Boss final : public cBoss
{
public:
    cUnknown0603Boss()noexcept;

    DISABLE_COPY(cUnknown0603Boss)
    ASSIGN_ID(603, "Unknown0603")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0701Boss final : public cBoss
{
public:
    cUnknown0701Boss()noexcept;

    DISABLE_COPY(cUnknown0701Boss)
    ASSIGN_ID(701, "Unknown0701")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0702Boss final : public cBoss
{
public:
    cUnknown0702Boss()noexcept;

    DISABLE_COPY(cUnknown0702Boss)
    ASSIGN_ID(702, "Unknown0702")


private:
    // execute own routines
    void __MoveOwn()final;
};


// ****************************************************************
// 
class cUnknown0703Boss final : public cBoss
{
public:
    cUnknown0703Boss()noexcept;

    DISABLE_COPY(cUnknown0703Boss)
    ASSIGN_ID(703, "Unknown0703")


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


#if !defined(_CORE_X64_)
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


#if !defined(_CORE_X64_)
    #pragma warning(pop)
#endif


#endif // _P1_GUARD_BOSS_H_