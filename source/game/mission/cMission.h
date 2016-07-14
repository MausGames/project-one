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

// TODO: reuse paths and squads over stages
// TODO: add visible debug-spline
// TODO: prevent multiple calculations in script-commands (because of macro variables)
// TODO: init-value for get-variables


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
#define STAGE_MAIN                      m_anStage.emplace(__LINE__, [this]()
#define STAGE_MAIN_WAIT(t)              STAGE_MAIN{STAGE_FINISH_AFTER(1.0f)});
#define STAGE_START_HERE                m_anStage.clear(); STAGE_MAIN{g_pGame->StartIntro(); if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) STAGE_FINISH_NOW});

#define STAGE_CLEARED                   (std::all_of(m_apSquad.begin(), m_apSquad.end(), [](const tSquadPtr& pSquad) {return pSquad->IsFinished();}))

#define STAGE_FINISH_NOW                {this->SkipStage();}
#define STAGE_FINISH_CLEARED            {if(STAGE_CLEARED)       STAGE_FINISH_NOW}
#define STAGE_FINISH_AFTER(t)           {if(m_fStageTime >= (t)) STAGE_FINISH_NOW}

#define STAGE_ADD_PATH(n)               auto n = this->_AddPath    (__LINE__,      [this](coreSpline2* OUTPUT n)
#define STAGE_ADD_SQUAD(n,t,c)          auto n = this->_AddSquad<t>(__LINE__, (c), [this](cEnemySquad* OUTPUT n)

#define STAGE_FOREACH_PLAYER(e,i)       g_pGame->ForEachPlayer([&](cPlayer* OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_ENEMY(s,e,i)      (s)->ForEachEnemy     ([&](cEnemy*  OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_ENEMY_ALL(s,e,i)  (s)->ForEachEnemyAll  ([&](cEnemy*  OUTPUT e, const coreUintW i)

#define __STAGE_GET_INT(c)              {if((c) > m_iIntSize)   {SAFE_DELETE_ARRAY(m_piInt)   m_piInt   = new coreInt16[c]; m_iIntSize   = (c); std::memset(m_piInt,   0, sizeof(coreInt16) * m_iIntSize);}}   const coreUintW UNUSED iNewIntSize   = (c);
#define __STAGE_GET_FLOAT(c)            {if((c) > m_iFloatSize) {SAFE_DELETE_ARRAY(m_pfFloat) m_pfFloat = new coreFloat[c]; m_iFloatSize = (c); std::memset(m_pfFloat, 0, sizeof(coreFloat) * m_iFloatSize);}} const coreUintW UNUSED iNewFloatSize = (c);
#define __STAGE_GET_CHECK               {ASSERT((iIntIndex <= iNewIntSize) && (iFloatIndex <= iNewFloatSize))}
#define STAGE_GET_END                   {ASSERT((iIntIndex == iNewIntSize) && (iFloatIndex == iNewFloatSize))}
#define STAGE_GET_START(i,f)            coreUintW UNUSED iIntIndex = 0u; coreUintW UNUSED iFloatIndex = 0u; __STAGE_GET_INT(i) __STAGE_GET_FLOAT(f)
#define STAGE_GET_INT(n,...)            coreInt16&   n =                     (m_piInt   [iIntIndex]);   iIntIndex   += 1u;       __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_FLOAT(n,...)          coreFloat&   n =                     (m_pfFloat [iFloatIndex]); iFloatIndex += 1u;       __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC2(n,...)           coreVector2& n = r_cast<coreVector2&>(m_pfFloat [iFloatIndex]); iFloatIndex += 2u;       __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC3(n,...)           coreVector3& n = r_cast<coreVector3&>(m_pfFloat [iFloatIndex]); iFloatIndex += 3u;       __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC4(n,...)           coreVector4& n = r_cast<coreVector4&>(m_pfFloat [iFloatIndex]); iFloatIndex += 4u;       __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_INT_ARRAY(n,c,...)    coreInt16*   n =                     (&m_piInt  [iIntIndex]);   iIntIndex   += 1u * (c); __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_FLOAT_ARRAY(n,c,...)  coreFloat*   n =                     (&m_pfFloat[iFloatIndex]); iFloatIndex += 1u * (c); __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC2_ARRAY(n,c,...)   coreVector2* n = r_cast<coreVector2*>(&m_pfFloat[iFloatIndex]); iFloatIndex += 2u * (c); __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC3_ARRAY(n,c,...)   coreVector3* n = r_cast<coreVector3*>(&m_pfFloat[iFloatIndex]); iFloatIndex += 3u * (c); __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC4_ARRAY(n,c,...)   coreVector4* n = r_cast<coreVector4*>(&m_pfFloat[iFloatIndex]); iFloatIndex += 4u * (c); __STAGE_GET_CHECK {if(STAGE_BEGINNING) {__VA_ARGS__;}}

#define STAGE_LIFETIME(e,m,a)                                                                     \
    const coreFloat UNUSED fLifeSpeed      = (m);                                                 \
    const coreFloat UNUSED fLifeOffset     = (a);                                                 \
    coreFloat       UNUSED fLifeTime       = (e)->GetLifeTime()       * fLifeSpeed + fLifeOffset; \
    coreFloat       UNUSED fLifeTimeBefore = (e)->GetLifeTimeBefore() * fLifeSpeed + fLifeOffset; \
    coreFloat       UNUSED fLifeTimeDelay  = 0.0f;                                                \
    coreFloat       UNUSED fLifeTimePoint  = 0.0f;                                                \
    coreFloat       UNUSED fHealthPctPoint = 0.0f;                                                \
    coreVector2     UNUSED vPositionPoint  = coreVector2(0.0f,0.0f);                              \
    coreBool        UNUSED bEnablePosition = ((e)->GetLifeTime() != 0.0f);                        \
    if(!CONTAINS_FLAG(e->GetStatus(), ENEMY_STATUS_DEAD)) e->SetEnabled((fLifeTime >= 0.0f) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);

#define __STAGE_ROUND(x)                (I_TO_F(F_TO_UI((x) * FRAMERATE_VALUE * RCP(fLifeSpeed))) / (FRAMERATE_VALUE * RCP(fLifeSpeed)))
#define STAGE_BRANCH(x,y)               ((fLifeTime < (x)) || [&](){const coreFloat fRound = /*__STAGE_ROUND*/(y); fLifeTime = FMOD(fLifeTime - (x), fRound); fLifeTimeBefore = FMOD(fLifeTimeBefore - (x), fRound); bEnablePosition &= (fLifeTime >= fLifeTimeBefore); return false;}())
#define STAGE_TICK(c)                   (!(F_TO_UI(fLifeTime * FRAMERATE_VALUE * RCP(fLifeSpeed)) % (c)))
#define STAGE_WAIT(t)                   {m_fStageWait = (t);}

#define STAGE_DELAY_ADD(t,u)            {const coreFloat fDelay = CLAMP(fLifeTime - (t), 0.0f, (u)); fLifeTime -= fDelay; fLifeTimeDelay += fDelay;}
#define STAGE_DELAY_RESET               {fLifeTime += fLifeTimeDelay; fLifeTimeDelay = 0.0f;}

#define STAGE_REMOVE_LIFETIME(e,t)      {if(fLifeTime >= (t)) {(e)->Kill(false); return;}}
#define STAGE_REMOVE_AREA(e)                                              \
    if(((e)->GetPosition().x < -FOREGROUND_AREA.x * ENEMY_AREA_FACTOR) || \
       ((e)->GetPosition().x >  FOREGROUND_AREA.x * ENEMY_AREA_FACTOR) || \
       ((e)->GetPosition().y < -FOREGROUND_AREA.y * ENEMY_AREA_FACTOR) || \
       ((e)->GetPosition().y >  FOREGROUND_AREA.y * ENEMY_AREA_FACTOR))   \
       {(e)->Kill(false); return;}

#define STAGE_TIME_POINT(t)             (InBetween((t), m_fStageTimeBefore, m_fStageTime))
#define STAGE_TIME_BEFORE(t)            (m_fStageTime <  (t))
#define STAGE_TIME_AFTER(t)             (m_fStageTime >= (t))
#define STAGE_TIME_BETWEEN(t,u)         (InBetween(m_fStageTime, (t), (u)))
#define STAGE_BEGINNING                 (STAGE_TIME_POINT(0.0f))

#define STAGE_LIFETIME_POINT(t)         (InBetween((t), fLifeTimeBefore, fLifeTime) && [&](){fLifeTimePoint = (t); return true;}())
#define STAGE_LIFETIME_BEFORE(t)        (fLifeTime <  (t))
#define STAGE_LIFETIME_AFTER(t)         (fLifeTime >= (t))
#define STAGE_LIFETIME_BETWEEN(t,u)     (InBetween(fLifeTime, __STAGE_ROUND(t), __STAGE_ROUND(u)))

#define STAGE_HEALTHPCT_POINT(e,t)      ((e)->ReachedHealthPct(t) && [&](){fHealthPctPoint = (t); return true;}())
#define STAGE_HEALTHPCT_BEFORE(e,t)     ((e)->GetCurHealthPct() <  (t))
#define STAGE_HEALTHPCT_AFTER(e,t)      ((e)->GetCurHealthPct() >= (t))
#define STAGE_HEALTHPCT_BETWEEN(e,t,u)  (InBetween((e)->GetCurHealthPct(), (t), (u)))
#define STAGE_DIED(e)                   ((e)->ReachedDeath())

#define STAGE_POSITION_POINT(e,t,v)     (bEnablePosition && InBetweenExt((t), (e)->GetOldPos(). ## v, (e)->GetPosition(). ## v) && [&](){vPositionPoint = (e)->GetPosition().xy(); vPositionPoint. ## v = (t); return true;}())
#define STAGE_POSITION_BEFORE(e,t,v)    ((e)->GetPosition(). ## v <  (t))
#define STAGE_POSITION_AFTER(e,t,v)     ((e)->GetPosition(). ## v >= (t))
#define STAGE_POSITION_BETWEEN(e,t,u,v) (InBetweenExt((e)->GetPosition(). ## v, (t), (u)))
#define STAGE_FLYPAST(e,f,v)                                \
    ((e)->GetPosition(). ## v < (f)->GetPosition(). ## v) ^ \
    ((e)->GetPosition(). ## v < (f)->GetOldPos  (). ## v) | \
    ((e)->GetOldPos  (). ## v < (f)->GetPosition(). ## v) ^ \
    ((e)->GetOldPos  (). ## v < (f)->GetOldPos  (). ## v) | \
    ((f)->GetPosition(). ## v < (e)->GetPosition(). ## v) ^ \
    ((f)->GetPosition(). ## v < (e)->GetOldPos  (). ## v) | \
    ((f)->GetOldPos  (). ## v < (e)->GetPosition(). ## v) ^ \
    ((f)->GetOldPos  (). ## v < (e)->GetOldPos  (). ## v)


// ****************************************************************
// mission interface
class INTERFACE cMission
{
protected:
    // 
    using tStageFunc = std::function<void()>;
    using tPathPtr   = std::unique_ptr<coreSpline2>;
    using tSquadPtr  = std::unique_ptr<cEnemySquad>;


protected:
    cBoss* m_apBoss[MISSION_BOSSES];                  // pointers to all available bosses

    cBoss*    m_pCurBoss;                             // pointer to currently active boss
    coreUintW m_iCurBossIndex;                        // index of the active boss (or error-value)

    coreLookup<coreUint16, tStageFunc> m_anStage;     // 
    coreLookup<coreUint16, tPathPtr>   m_apPath;      // 
    coreLookup<coreUint16, tSquadPtr>  m_apSquad;     // 

    coreInt16* m_piInt;                               // 
    coreFloat* m_pfFloat;                             // 
    coreUint8  m_iIntSize;                            // 
    coreUint8  m_iFloatSize;                          // 

    coreUint16 m_iStageNum;                           // 
    coreFlow   m_fStageWait;                          // 
    coreFlow   m_fStageTime;                          // 
    coreFloat  m_fStageTimeBefore;                    // 


public:
    cMission()noexcept;
    virtual ~cMission();

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
    void            SkipStage ();
    inline coreBool IsWaiting ()const {return m_fStageWait > 0.0f;}
    inline coreBool IsFinished()const {return m_anStage.empty();}

    // set active boss
    void SetCurBoss(const coreUintW iIndex);
    void SetCurBoss(const cBoss*    pBoss);

    // access mission objects
    inline cBoss*           GetBoss        (const coreUintW iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*           GetCurBoss     ()const                       {return m_pCurBoss;}
    inline const coreUintW& GetCurBossIndex()const                       {return m_iCurBossIndex;}
    inline cEnemySquad*     GetEnemySquad  (const coreUintW iIndex)const {ASSERT(iIndex < m_apSquad.size()) return m_apSquad.get_valuelist()[iIndex].get();}


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

    coreUint8 m_iStickyState;                       // (only between first ball and first paddle) 
    coreUint8 m_iBounceState;                       // 
    coreBool  m_bBounceReal;                        // 

    coreFlow m_fAnimation;                          // animation value


public:
    cViridoMission()noexcept;
    ~cViridoMission()final;

    DISABLE_COPY(cViridoMission)
    ASSIGN_ID(1, "Virido")

    // 
    void EnableBall (const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection);
    void DisableBall(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePaddle (const coreUintW iIndex, cShip* pOwner);
    void DisablePaddle(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void     MakeSticky    ()                              {ADD_BIT(m_iStickyState, 0u)}
    inline void     UnmakeSticky  (const coreVector2& vDirection) {m_iStickyState = 0; m_aBallRaw[0].SetDirection(coreVector3(vDirection, 0.0f));}
    inline coreBool GetStickyState()const                         {return CONTAINS_BIT(m_iStickyState, 1u);}

    // 
    inline void      SetBounceReal (const coreBool bStatus) {m_bBounceReal = bStatus;}
    inline coreUint8 GetBounceState()const                  {return m_iBounceState;}

    // 
    inline coreObject3D* GetBall  (const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_BALLS)   return &m_aBallRaw[iIndex * (VIRIDO_TRAILS + 1u)];}
    inline coreObject3D* GetPaddle(const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_PADDLES) return &m_aPaddle [iIndex];}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnAttack()final;
    void __MoveOwnBefore  ()final;
    void __MoveOwnAfter   ()final;

    // 
    static void __BounceEffect(const coreVector2& vEffectPos);
};


// ****************************************************************
// Nevo mission class
class cNevoMission final : public cMission
{
private:
    cUnknown0201Boss m_Unknown0201;   // 
    cUnknown0201Boss m_Unknown0202;   // 
    cUnknown0201Boss m_Unknown0203;   // 


public:
    cNevoMission()noexcept;

    DISABLE_COPY(cNevoMission)
    ASSIGN_ID(2, "Nevo")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Harena mission class
class cHarenaMission final : public cMission
{
private:
    cUnknown0301Boss m_Unknown0301;   // 
    cUnknown0301Boss m_Unknown0302;   // 
    cUnknown0301Boss m_Unknown0303;   // 


public:
    cHarenaMission()noexcept;

    DISABLE_COPY(cHarenaMission)
    ASSIGN_ID(3, "Harena")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Rutilus mission class
class cRutilusMission final : public cMission
{
private:
    cUnknown0401Boss m_Unknown0401;   // 
    cUnknown0401Boss m_Unknown0402;   // 
    cUnknown0401Boss m_Unknown0403;   // 


public:
    cRutilusMission()noexcept;

    DISABLE_COPY(cRutilusMission)
    ASSIGN_ID(4, "Rutilus")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Gelu mission class
class cGeluMission final : public cMission
{
private:
    cUnknown0501Boss m_Unknown0501;   // 
    cUnknown0501Boss m_Unknown0502;   // 
    cUnknown0501Boss m_Unknown0503;   // 


public:
    cGeluMission()noexcept;

    DISABLE_COPY(cGeluMission)
    ASSIGN_ID(5, "Gelu")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Calor mission class
class cCalorMission final : public cMission
{
private:
    cUnknown0601Boss m_Unknown0601;   // 
    cUnknown0601Boss m_Unknown0602;   // 
    cUnknown0601Boss m_Unknown0603;   // 


public:
    cCalorMission()noexcept;

    DISABLE_COPY(cCalorMission)
    ASSIGN_ID(6, "Calor")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Muscus mission class
class cMuscusMission final : public cMission
{
private:
    cUnknown0701Boss m_Unknown0701;   // 
    cUnknown0701Boss m_Unknown0702;   // 
    cUnknown0701Boss m_Unknown0703;   // 


public:
    cMuscusMission()noexcept;

    DISABLE_COPY(cMuscusMission)
    ASSIGN_ID(7, "Muscus")


private:
    // execute own routines
    void __SetupOwn()final;
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
    ~cIntroMission()final;

    DISABLE_COPY(cIntroMission)
    ASSIGN_ID(99, "Intro")


private:
    // execute own routines
    void __SetupOwn     ()final;
    void __MoveOwnBefore()final;
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
        m_apPath.emplace(iCodeLine, std::move(pNewPath));
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
        m_apSquad.emplace(iCodeLine, std::move(pNewSquad));
    }

    return m_apSquad.at(iCodeLine).get();
}


#endif // _P1_GUARD_MISSION_H_