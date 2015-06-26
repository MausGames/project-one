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

// TODO: boomerangs of Crossfield may generate double-hits because of rotating box collision (when moving away from it)


// ****************************************************************
// boss definitions
#define BOSS_TIMERS   (4u)   // 
#define BOSS_COUNTERS (8u)   // 
#define BOSS_VECTORS  (4u)   // 


// ****************************************************************
// phase management macros
#define PHASE_CONTROL_TIMER(a,b,c)  this->_PhaseTimer (a, __LINE__, b, c, [&](const coreFloat&  fTime, const coreFloat& fTimeBefore, const coreBool& __bEnd)   // 
#define PHASE_CONTROL_TICKER(a,b,c) this->_PhaseTicker(a, __LINE__, b, c, [&](const coreUint16& iTick,                               const coreBool& __bEnd)   // 
#define PHASE_SUB(t)                ((fTimeBefore <= (t)) && ((t) <= fTime))                                                                                   // 
#define PHASE_RESET(i)              {m_aTimer[i].Stop(); m_aiTimerLine[i] = 0u;}                                                                               // 
#define PHASE_FINISHED              (__bEnd)                                                                                                                   // 

#define LERP_LINEAR    (&LERP <coreFloat>)
#define LERP_SMOOTH    (&LERPS<coreFloat>)
#define LERP_BREAK     (&LERPB<coreFloat>)
#define LERP_BREAK_REV ([](const coreFloat& x, const coreFloat& y, const coreFloat& s) {return LERPB(y, x, 1.0f-s);})


// ****************************************************************
// 
#define CROSSFIELD_BOOMERANGS (4u)                                                 // 
#define CROSSFIELD_TRAILS     (3u)                                                 // 
#define CROSSFIELD_RAWS       (CROSSFIELD_BOOMERANGS * (CROSSFIELD_TRAILS + 1u))   // 

#define TORUS_RAY_SIZE        (coreVector3(0.7f,50.0f,0.7f))   // 
#define TORUS_RAY_TEXSIZE     (coreVector2(0.5f, 1.5f))        // 
#define TORUS_RAY_OFFSET      (8.0f)                           // 
#define TORUS_RAYWAVE_SIZE    (coreVector3(1.6f, 5.0f,1.3f))   // 


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
    virtual ~cBoss() {}

    DISABLE_COPY(cBoss)
    ENABLE_ID

    // 
    inline const coreUint8& GetPhase()const {return m_iPhase;}
    inline const coreUint8& GetLevel()const {return m_iLevel;}


protected:
    // 
    void _PhaseTimer (const coreUintW& iTimerIndex, const coreUint16 iCodeLine, const coreFloat&  fSpeed, const std::function<coreFloat(coreFloat, coreFloat, coreFloat)>&& nLerpFunc, const std::function<void(coreFloat, coreFloat, coreBool)>&& nUpdateFunc);
    void _PhaseTicker(const coreUintW& iTimerIndex, const coreUint16 iCodeLine, const coreUint16& iTicks, const coreFloat& fRate,                                                      const std::function<void(coreUint16,           coreBool)>&& nUpdateFunc);
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
    ~cCrossfieldBoss();

    DISABLE_COPY(cCrossfieldBoss)
    ASSIGN_ID(101, "Crossfield")


private:
    // 
    void __ResurrectOwn   ()override;
    void __KillOwn        ()override;
    void __RenderOwnWeak  ()override;
    void __RenderOwnStrong()override;
    void __MoveOwn        ()override;

    // 
    coreVector2 __RepeatPosition  (const coreVector2& vPosition, const coreFloat& fThreshold, coreBool* OUTPUT pbChange);
    void        __EncodeDirection (const coreUintW& iIndex, const coreVector2& vDirection);
    coreVector2 __DecodeDirection (const coreUintW& iIndex);
    void        __EnableBoomerang (const coreUintW& iIndex, const coreVector2& vPosition, const coreVector2& vDirection);
    void        __DisableBoomerang(const coreUintW& iIndex, const coreBool& bAnimated);
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
    ~cTorusBoss();

    DISABLE_COPY(cTorusBoss)
    ASSIGN_ID(102, "Torus")

    // 
    void Render()override;


private:
    // 
    void __ResurrectOwn   ()override;
    void __KillOwn        ()override;
    void __RenderOwnStrong()override;
    void __RenderOwnAfter ()override;
    void __MoveOwn        ()override;

    // 
    coreVector3 __GetRotaDirection(const coreFloat& fBaseAngle);
    void        __SetRotaAttack   (const coreInt16& iType, const coreBool& bAnimated);
    void        __EnableRay       (const coreUintW& iIndex);
    void        __DisableRay      (const coreUintW& iIndex);
    void        __CreateOverdrive (const coreUintW& iIndex, const coreVector3& vIntersect, const coreFloat& fTime, const coreBool& bGround);
};


// ****************************************************************
// Vaus boss class
class cVausBoss final : public cBoss
{
private:
    coreUint8 m_iScoutOrder;   // 


public:
    cVausBoss()noexcept;
    ~cVausBoss();

    DISABLE_COPY(cVausBoss)
    ASSIGN_ID(103, "Vaus")


private:
    // 
    void __ResurrectOwn()override;
    void __KillOwn     ()override;
    void __MoveOwn     ()override;
};


#endif // _P1_GUARD_BOSS_H_