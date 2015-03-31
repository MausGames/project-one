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


// ****************************************************************
// boss definitions
#define BOSS_TIMERS   (4u)   // 
#define BOSS_COUNTERS (8u)   // 


// ****************************************************************
// phase management macros
#define PHASE_MAIN_MOVE(a,b,c,d) this->_PhaseMove (a, b, c, d
#define PHASE_MAIN_SHOOT(a,b,c)  this->_PhaseShoot(a, b, c
#define PHASE_SUB_INIT()         ,[&]()
#define PHASE_SUB_UPDATE(...)    ,[&](__VA_ARGS__)
#define PHASE_SUB_EXIT()         ,[&]()
#define PHASE_END                );

#define PHASE_RESET_TIMER_MOVE     {this->m_MoveTimer.Stop();}
#define PHASE_RESET_TIMER_SHOOT(i) {this->m_aShootTimer[i].Stop();}

#define LERP_LINEAR    (&LERP <coreFloat>)
#define LERP_SMOOTH    (&LERPS<coreFloat>)
#define LERP_BREAK     (&LERPB<coreFloat>)
#define LERP_BREAK_REV ([](const coreFloat& x, const coreFloat& y, const coreFloat& s) {return LERPB(y, x, 1.0f-s);})


// ****************************************************************
// 
#define CROSSFIELD_BOOMERANGS (4u)   // 
#define CROSSFIELD_TRAILS     (3u)   // 


// ****************************************************************
// boss entity interface
class INTERFACE cBoss : public cEnemy
{
protected:
    coreUint8 m_iPhase;                     // 
    coreUint8 m_iLevel;                     // 

    coreTimer m_MoveTimer;                  // 
    coreTimer m_aShootTimer[BOSS_TIMERS];   // 
    coreInt16 m_aiCounter[BOSS_COUNTERS];   // 

    coreVector2 m_vOldPos;                  // 


public:
    cBoss()noexcept;
    virtual ~cBoss() {}

    DISABLE_COPY(cBoss)
    ENABLE_ID

    // 
    inline const coreUint8& GetPhase()const {return m_iPhase;}
    inline const coreUint8& GetLevel()const {return m_iLevel;}


protected:
    // default phase routines (# quite a mess, but good enough) 
    void        _PhaseMove (const coreVector2& vFromPos,    const coreVector2& vToPos, const coreFloat& fSpeed, const std::function<coreFloat(coreFloat, coreFloat, coreFloat)>&& nLerp, const std::function<void()>&& nInitFunc, const std::function<void(coreFloat)>&&  nUpdateFunc, const std::function<void()>&& nExitFunc);
    inline void _PhaseMove (const coreVector2& vFromPos,    const coreVector2& vToPos, const coreFloat& fSpeed, const std::function<coreFloat(coreFloat, coreFloat, coreFloat)>&& nLerp,                                          const std::function<void(coreFloat)>&&  nUpdateFunc, const std::function<void()>&& nExitFunc) {this->_PhaseMove (vFromPos, vToPos, fSpeed, std::move(nLerp), [](){}, std::move(nUpdateFunc), std::move(nExitFunc));}
    inline void _PhaseMove (const coreVector2& vFromPos,    const coreVector2& vToPos, const coreFloat& fSpeed, const std::function<coreFloat(coreFloat, coreFloat, coreFloat)>&& nLerp,                                          const std::function<void(coreFloat)>&&  nUpdateFunc)                                          {this->_PhaseMove (vFromPos, vToPos, fSpeed, std::move(nLerp), [](){}, std::move(nUpdateFunc), [](){});}
    void        _PhaseShoot(const coreUintW&   iTimerIndex, const coreUint16&  iShots, const coreFloat& fSpeed,                                                                          const std::function<void()>&& nInitFunc, const std::function<void(coreUint16)>&& nUpdateFunc, const std::function<void()>&& nExitFunc);
    inline void _PhaseShoot(const coreUintW&   iTimerIndex, const coreUint16&  iShots, const coreFloat& fSpeed,                                                                                                                   const std::function<void(coreUint16)>&& nUpdateFunc, const std::function<void()>&& nExitFunc) {this->_PhaseShoot(iTimerIndex, iShots, fSpeed,                [](){}, std::move(nUpdateFunc), std::move(nExitFunc));}
    inline void _PhaseShoot(const coreUintW&   iTimerIndex, const coreUint16&  iShots, const coreFloat& fSpeed,                                                                                                                   const std::function<void(coreUint16)>&& nUpdateFunc)                                          {this->_PhaseShoot(iTimerIndex, iShots, fSpeed,                [](){}, std::move(nUpdateFunc), [](){});}
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

    coreUint16 m_iPackedDir;                             // 
    coreFlow   m_fAnimation;                             // animation value


public:
    cCrossfieldBoss()noexcept;
    ~cCrossfieldBoss();

    DISABLE_COPY(cCrossfieldBoss)
    ASSIGN_ID(101, "Crossfield")


private:
    // 
    void __RenderOwnBefore()override;
    void __MoveOwn        ()override;
    void __RenderOwnAfter ()override;

    // 
    coreVector2 __RepeatPosition (const coreVector2& vPosition, const coreFloat& fThreshold, coreBool* OUTPUT pbChange);
    void        __RotateDirection(const coreFloat& fAngleFrom, const coreFloat& fAngleTo, const coreFloat& fLerp);
    void        __EncodeDirection(const coreUintW& iIndex, const coreVector2& vDirection);
    coreVector2 __DecodeDirection(const coreUintW& iIndex);
};


// ****************************************************************
// Torus boss class
class cTorusBoss final : public cBoss
{
private:
    coreObject3D m_aCircle[2];   // 

    coreFlow m_fAnimation;       // animation value


public:
    cTorusBoss()noexcept;
    ~cTorusBoss();

    DISABLE_COPY(cTorusBoss)
    ASSIGN_ID(102, "Torus")


private:
    // 
    void __RenderOwnBefore()override;
    void __MoveOwn        ()override;
    void __RenderOwnAfter ()override;
};


// ****************************************************************
// Vaus boss class
class cVausBoss final : public cBoss
{
private:
    coreObject3D m_Bump;     // 

    coreFlow m_fAnimation;   // animation value


public:
    cVausBoss()noexcept;
    ~cVausBoss();

    DISABLE_COPY(cVausBoss)
    ASSIGN_ID(103, "Vaus")


private:
    // 
    void __RenderOwnBefore()override;
    void __MoveOwn        ()override;
    void __RenderOwnAfter ()override;
};


#endif // _P1_GUARD_BOSS_H_