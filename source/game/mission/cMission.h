//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MISSION_H_
#define _P1_GUARD_MISSION_H_


// ****************************************************************
// mission definitions
#define MISSION_BOSSES  (3u)      // default number of bosses per mission
#define MISSION_NO_BOSS (0xFFu)   // no boss currently active (error-value)


// ****************************************************************
// mission specific definitions
#define VIRIDO_BALLS      (2u)                                    // 
#define VIRIDO_TRAILS     (4u)                                    // 
#define VIRIDO_RAWS       (VIRIDO_BALLS * (VIRIDO_TRAILS + 1u))   // 
#define VIRIDO_PADDLES    (PLAYERS + 1u)                          // 
#define VIRIDO_BALL_SPEED (1.5f)                                  // 


// ****************************************************************
// stage management macros
#define STAGE_MAIN              m_anStage[__LINE__] = ([this]()                    // 

#define STAGE_TIME_POINT(t)     (InBetween(t, m_fStageTimeBefore, m_fStageTime))   // 
#define STAGE_TIME_AFTER(t)     ((t) >= m_fStageTime)                              // 
#define STAGE_TIME_BEFORE(t)    ((t) <  m_fStageTime)                              // 
#define STAGE_TIME_BETWEEN(t,u) (InBetween(m_fStageTime, t, u))                    // 
#define STAGE_TIME_INIT         (STAGE_TIME_POINT(0.0f))                           // 

#define STAGE_FINISH_NOW        {this->SkipStage();}                               // 
#define STAGE_FINISH_AFTER(t)   {if(m_fStageTime >= (t)) STAGE_FINISH_NOW}         // 

#define STAGE_ADD_PATH(n)      auto n = this->_AddPath    (__LINE__,    [&](coreSpline2* OUTPUT n)
#define STAGE_ADD_SQUAD(n,t,c) auto n = this->_AddSquad<t>(__LINE__, c, [&](cEnemySquad* OUTPUT n)

#define STAGE_SQUAD_INIT(x,f) {x->ForEachEnemyAll([&](cEnemy* OUTPUT pEnemy, const coreUintW i) {pEnemy->f;});}


// ****************************************************************
// mission interface
class INTERFACE cMission
{
private:
    using tStageFunc = std::function<void()>;
    using tPathPtr   = std::unique_ptr<coreSpline2>;
    using tSquadPtr  = std::unique_ptr<cEnemySquad>;


protected:
    cBoss* m_apBoss[MISSION_BOSSES];                // pointers to all available bosses

    cBoss*    m_pCurBoss;                           // pointer to currently active boss
    coreUintW m_iCurBossIndex;                      // index of the active boss (or error-value)

    coreLookup<coreUint16, tStageFunc> m_anStage;   // 
    coreLookup<coreUint16, tPathPtr>   m_apPath;    // 
    coreLookup<coreUint16, tSquadPtr>  m_apSquad;   // 

    coreUint16 m_iStageNum;                         // 

    coreFlow  m_fStageTime;                         // 
    coreFloat m_fStageTimeBefore;                   // 


public:
    cMission()noexcept;
    virtual ~cMission() = default;

    DISABLE_COPY(cMission)
    ENABLE_ID

    // setup the mission
    void Setup();

    // render and move the mission
    void RenderUnder ();
    void RenderAttack();
    void RenderOver  ();
    void MoveBefore  ();
    void MoveAfter   ();

    // 
    inline void     SkipStage ()      {m_fStageTime = 0.0f; m_anStage.pop_back();}
    inline coreBool IsFinished()const {return m_anStage.empty();}

    // set active boss
    void SetCurBoss(const coreUintW iIndex);
    void SetCurBoss(const cBoss*    pBoss);

    // access mission objects
    inline cBoss*           GetBoss        (const coreUintW iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*           GetCurBoss     ()const                       {return m_pCurBoss;}
    inline const coreUintW& GetCurBossIndex()const                       {return m_iCurBossIndex;}


protected:
    // 
    template             <typename F> coreSpline2* _AddPath (const coreUint16 iCodeLine,                       F&& nInitFunc);   // [](coreSpline2* OUTPUT pPath)  -> void
    template <typename T, typename F> cEnemySquad* _AddSquad(const coreUint16 iCodeLine, const coreUint8 iNum, F&& nInitFunc);   // [](cEnemySquad* OUTPUT pSquad) -> void


private:
    // own routines for derived classes
    virtual void __SetupOwn       () {}
    virtual void __RenderOwnUnder () {}
    virtual void __RenderOwnAttack() {}
    virtual void __RenderOwnOver  () {}
    virtual void __MoveOwnBefore  () {}
    virtual void __MoveOwnAfter   () {}
};


// ****************************************************************
// empty mission class
class cNoMission final : public cMission
{
public:
    cNoMission() = default;

    DISABLE_COPY(cNoMission)
    ASSIGN_ID(0, "Nothing")
};


// ****************************************************************
// Virido mission class
class cViridoMission final : public cMission
{
private:
    cCrossfieldBoss m_Crossfield;                   // 
    cTorusBoss      m_Torus;                        // 
    cVausBoss       m_Vaus;                         // 

    coreBatchList m_Ball;                           // 
    coreBatchList m_BallTrail;                      // 
    coreObject3D  m_aBallRaw[VIRIDO_RAWS];          // 

    coreObject3D m_aPaddle      [VIRIDO_PADDLES];   // 
    coreObject3D m_aPaddleSphere[VIRIDO_PADDLES];   // 
    cShip*       m_apOwner      [VIRIDO_PADDLES];   // 

    coreUint8 m_iBounceState;                       // 
    coreBool  m_bBounceReal;                        // 

    coreFlow m_fAnimation;                          // animation value


public:
    cViridoMission()noexcept;
    ~cViridoMission()override;

    DISABLE_COPY(cViridoMission)
    ASSIGN_ID(1, "Virido")

    // 
    void EnableBall (const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection);
    void DisableBall(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePaddle (const coreUintW iIndex, cShip* pOwner);
    void DisablePaddle(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void     SetBounceReal (const coreBool  bStatus)     {m_bBounceReal = bStatus;}
    inline coreBool GetBounceState(const coreUintW iIndex)const {ASSERT(iIndex < sizeof(m_iBounceState)*8u) return CONTAINS_BIT(m_iBounceState, iIndex);}

    // 
    inline coreObject3D* GetBall  (const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_BALLS)   return &m_aBallRaw[iIndex * (VIRIDO_TRAILS + 1u)];}
    inline coreObject3D* GetPaddle(const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_PADDLES) return &m_aPaddle [iIndex];}


private:
    // execute own routines
    void __SetupOwn       ()override;
    void __RenderOwnUnder ()override;
    void __RenderOwnAttack()override;
    void __MoveOwnAfter   ()override;
};


// ****************************************************************
// Nevo mission class
class cNevoMission final : public cMission
{
public:
    cNevoMission() = default;

    DISABLE_COPY(cNevoMission)
    ASSIGN_ID(2, "Nevo")
};


// ****************************************************************
// Harena mission class
class cHarenaMission final : public cMission
{
public:
    cHarenaMission() = default;

    DISABLE_COPY(cHarenaMission)
    ASSIGN_ID(3, "Harena")
};


// ****************************************************************
// Rutilus mission class
class cRutilusMission final : public cMission
{
public:
    cRutilusMission() = default;

    DISABLE_COPY(cRutilusMission)
    ASSIGN_ID(4, "Rutilus")
};


// ****************************************************************
// Gelu mission class
class cGeluMission final : public cMission
{
public:
    cGeluMission() = default;

    DISABLE_COPY(cGeluMission)
    ASSIGN_ID(5, "Gelu")
};


// ****************************************************************
// Calor mission class
class cCalorMission final : public cMission
{
public:
    cCalorMission() = default;

    DISABLE_COPY(cCalorMission)
    ASSIGN_ID(6, "Calor")
};


// ****************************************************************
// Muscus mission class
class cMuscusMission final : public cMission
{
public:
    cMuscusMission() = default;

    DISABLE_COPY(cMuscusMission)
    ASSIGN_ID(7, "Muscus")
};


// ****************************************************************
// Ater mission class
class cAterMission final : public cMission
{
public:
    cAterMission() = default;

    DISABLE_COPY(cAterMission)
    ASSIGN_ID(8, "Ater")
};


// ****************************************************************
// Intro mission class
class cIntroMission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cIntroMission()noexcept;

    DISABLE_COPY(cIntroMission)
    ASSIGN_ID(99, "Intro")


private:
    // execute own routines
    void __SetupOwn()override;
};


// ****************************************************************
// 
template <typename F> coreSpline2* cMission::_AddPath(const coreUint16 iCodeLine, F&& nInitFunc)
{
    if(!m_apPath.count(iCodeLine))
    {
        // 
        tPathPtr pNewPath = std::make_unique<coreSpline2>();
        nInitFunc(pNewPath.get());

        // 
        m_apPath[iCodeLine] = std::move(pNewPath);
    }

    return m_apPath.at(iCodeLine).get();
}


// ****************************************************************
// 
template <typename T, typename F> cEnemySquad* cMission::_AddSquad(const coreUint16 iCodeLine, const coreUint8 iNum, F&& nInitFunc)
{
    if(!m_apSquad.count(iCodeLine))
    {
        // 
        tSquadPtr pNewSquad = std::make_unique<cEnemySquad>();
        pNewSquad->AllocateEnemies<T>(iNum);
        nInitFunc(pNewSquad.get());

        // 
        m_apSquad[iCodeLine] = std::move(pNewSquad);
    }

    return m_apSquad.at(iCodeLine).get();
}


#endif // _P1_GUARD_MISSION_H_