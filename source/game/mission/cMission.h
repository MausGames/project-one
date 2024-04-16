///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MISSION_H_
#define _P1_GUARD_MISSION_H_

// TODO: reuse paths (beware of reserve assertion, and resize on refine) and squads over stages
// TODO: add visible debug-spline
// TODO: prevent multiple calculations in script-commands (because of macro variables), also boss
// TODO: assertion for "active boss should be alive"
// TODO: STAGE_FLYPAST with dot-product or simpler per-axis
// TODO: there seems to be a bug in STAGE_TICK_TIME, which sometimes gives early or late ticks with 30.0f speed, compared with STAGE_TICK_LIFETIME
// TODO: wrap m_piData in function with RETURN_RESTRICT


// ****************************************************************
// mission definitions
#define MISSION_BOSSES      (BOSSES)                                   // default number of bosses per mission
#define MISSION_WAVES       (WAVES)                                    // 
#define MISSION_NO_BOSS     (0xFFu)                                    // no boss currently active (error-value)
#define MISSION_NO_WAVE     (0xFFu)                                    // 
#define MISSION_NO_SEGMENT  (0xFFu)                                    // 
#define MISSION_STAGE_DELAY (INTERFACE_BANNER_DURATION_SCORE - 0.5f)   // 

#define MISSION_SEGMENT_IS_BOSS(i) ((i) % 6u == 5u)
#define MISSION_BOSS_TO_SEGMENT(i) ((i) * 6u  + 5u)
#define MISSION_WAVE_TO_SEGMENT(i) ((i) + (i) / 5u)


// ****************************************************************
// mission specific definitions
#define VIRIDO_TRAILS        (4u)                                    // 
#define VIRIDO_BALLS         (2u)                                    // 
#define VIRIDO_BALLS_RAWS    (VIRIDO_BALLS * (VIRIDO_TRAILS + 1u))   // 
#define VIRIDO_PADDLES       (3u)                                    // 
#define VIRIDO_BARRIERS      (14u)                                   // 
#define VIRIDO_BARRIERS_RAWS (VIRIDO_BARRIERS)                       // 
#define VIRIDO_LASERS        (4u)                                    // 
#define VIRIDO_LASERS_RAWS   (VIRIDO_LASERS * 2u)                    // 
#define VIRIDO_SHADOWS       (16u)                                   // 
#define VIRIDO_SHADOWS_RAWS  (VIRIDO_SHADOWS)                        // 
#define VIRIDO_BALL_SPEED    (1.5f)                                  // 


// ****************************************************************
// stage management macros
#define STAGE_MAIN                      m_anStage.emplace(__LINE__, [this]()
#define STAGE_SUB(i)                    ((m_iStageSub < (i)) && [&]() {m_iStageSub = (i); return true;}())

#define STAGE_FINISH_NOW                {this->SkipStage();}
#define STAGE_FINISH_AFTER(t)           {if(m_fStageTime >= (t)) STAGE_FINISH_NOW}

#define STAGE_MEDAL_GOAL(...)           {static constexpr coreFloat A[] = __VA_ARGS__; this->SetMedalGoal(A); STATIC_ASSERT((ARRAY_SIZE(A) == 4u) && (A[0] < A[1]) && (A[1] < A[2]) && (A[2] < A[3]))}
#define STAGE_BOSS(e,...)               {if(STAGE_BEGINNING) {STAGE_MEDAL_GOAL(__VA_ARGS__) (e).Resurrect();} if(CONTAINS_FLAG((e).GetStatus(), ENEMY_STATUS_DEAD)) STAGE_FINISH_NOW}
#define STAGE_WAVE(n,...)               {if(STAGE_BEGINNING) {STAGE_MEDAL_GOAL(__VA_ARGS__) this->ActivateWave(n);} if(STAGE_CLEARED) {this->DeactivateWave(); STAGE_FINISH_NOW}}

#define STAGE_RESSURECT(s,f,t)          {STAGE_FOREACH_ENEMY_ALL(s, pEnemy, i) {if((coreInt32(i) >= coreInt32(f)) && (coreInt32(i) <= coreInt32(t))) pEnemy->Resurrect();});}
#define STAGE_CLEARED                   (std::all_of(m_apSquad.begin(), m_apSquad.end(), [](const cEnemySquad* pSquad) {return pSquad->IsFinished();}))

#define STAGE_ADD_PATH(n)               const auto n = this->_AddPath    (__LINE__,      [](coreSpline2* OUTPUT n)
#define STAGE_ADD_SQUAD(n,t,c)          const auto n = this->_AddSquad<t>(__LINE__, (c), [](cEnemySquad* OUTPUT n)

  
//#define STAGE_COLL_PLAYER_ENEMY(a,b,i)   this->_CollisionPlayerEnemy (__LINE__, [=](cPlayer* OUTPUT a, cEnemy*  OUTPUT b, const coreVector3& i)
//#define STAGE_COLL_PLAYER_BULLET(a,b,i)  this->_CollisionPlayerBullet(__LINE__, [=](cPlayer* OUTPUT a, cBullet* OUTPUT b, const coreVector3& i)
//#define STAGE_COLL_ENEMY_BULLET(a,b,i)   this->_CollisionEnemyBullet (__LINE__, [=](cEnemy*  OUTPUT a, cBullet* OUTPUT b, const coreVector3& i)
#define STAGE_COLL_PLAYER_ENEMY(a,b,v,...)   if(!m_nCollPlayerEnemy)  m_nCollPlayerEnemy  = ([__VA_ARGS__](cPlayer* OUTPUT a, cEnemy*  OUTPUT b, const coreVector3& v)
#define STAGE_COLL_PLAYER_BULLET(a,b,v,...)  if(!m_nCollPlayerBullet) m_nCollPlayerBullet = ([__VA_ARGS__](cPlayer* OUTPUT a, cBullet* OUTPUT b, const coreVector3& v)
#define STAGE_COLL_ENEMY_BULLET(a,b,v,...)   if(!m_nCollEnemyBullet)  m_nCollEnemyBullet  = ([__VA_ARGS__](cEnemy*  OUTPUT a, cBullet* OUTPUT b, const coreVector3& v)
#define COLL_VAL(x)                           x = s_cast<typename std::conditional<!std::is_reference<decltype(x)>::value, decltype(x), void>::type>(x)
#define COLL_REF(x)                          &x = s_cast<typename std::conditional< std::is_reference<decltype(x)>::value, decltype(x), void>::type>(x)

#define STAGE_START_HERE {m_anStage.clear(); STAGE_MAIN {if(STAGE_BEGINNING) {g_pGame->GetInterface()->ShowMission(this); g_pGame->StartIntro();} if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) STAGE_FINISH_NOW});}
  

#define STAGE_FOREACH_PLAYER(e,i)       g_pGame->ForEachPlayer   ([&](cPlayer* OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_PLAYER_ALL(e,i)   g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_ENEMY(s,e,i)      (s)->ForEachEnemy        ([&](cEnemy*  OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_ENEMY_ALL(s,e,i)  (s)->ForEachEnemyAll     ([&](cEnemy*  OUTPUT e, const coreUintW i)

#define STAGE_GET_START(c)              {if((c) > m_iDataSize) {ZERO_DELETE(m_piData) STATIC_ASSERT((c) < 0xFFu) m_iDataSize = (c); m_piData = ZERO_NEW(coreUint32, m_iDataSize);}} coreUintW iDataIndex = 0u; constexpr coreUintW iCurDataSize = (c);
#define STAGE_GET_END                   {ASSERT(iDataIndex == iCurDataSize)}
#define STAGE_GET_INT(n,...)            coreInt32&                n = r_cast<coreInt32&>  ( m_piData[iDataIndex]); iDataIndex += 1u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT(n,...)           coreUint32&               n = r_cast<coreUint32&> ( m_piData[iDataIndex]); iDataIndex += 1u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT64(n,...)         coreUint64&               n = r_cast<coreUint64&> ( m_piData[iDataIndex]); iDataIndex += 2u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_FLOAT(n,...)          coreFloat&                n = r_cast<coreFloat&>  ( m_piData[iDataIndex]); iDataIndex += 1u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC2(n,...)           coreVector2&              n = r_cast<coreVector2&>( m_piData[iDataIndex]); iDataIndex += 2u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC3(n,...)           coreVector3&              n = r_cast<coreVector3&>( m_piData[iDataIndex]); iDataIndex += 3u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC4(n,...)           coreVector4&              n = r_cast<coreVector4&>( m_piData[iDataIndex]); iDataIndex += 4u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_INT_ARRAY(n,c,...)    coreInt32*   const OUTPUT n = r_cast<coreInt32*>  (&m_piData[iDataIndex]); iDataIndex += 1u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT_ARRAY(n,c,...)   coreUint32*  const OUTPUT n = r_cast<coreUint32*> (&m_piData[iDataIndex]); iDataIndex += 1u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT64_ARRAY(n,c,...) coreUint64*  const OUTPUT n = r_cast<coreUint64*> (&m_piData[iDataIndex]); iDataIndex += 2u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_FLOAT_ARRAY(n,c,...)  coreFloat*   const OUTPUT n = r_cast<coreFloat*>  (&m_piData[iDataIndex]); iDataIndex += 1u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC2_ARRAY(n,c,...)   coreVector2* const OUTPUT n = r_cast<coreVector2*>(&m_piData[iDataIndex]); iDataIndex += 2u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC3_ARRAY(n,c,...)   coreVector3* const OUTPUT n = r_cast<coreVector3*>(&m_piData[iDataIndex]); iDataIndex += 3u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC4_ARRAY(n,c,...)   coreVector4* const OUTPUT n = r_cast<coreVector4*>(&m_piData[iDataIndex]); iDataIndex += 4u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}

#define STAGE_LIFETIME(e,m,a)                                                                         \
    UNUSED const coreFloat fLifeSpeed          = (m);                                                 \
    UNUSED const coreFloat fLifeOffset         = (a);                                                 \
    UNUSED const coreFloat fLifeTimeBase       = (e)->GetLifeTime()       * fLifeSpeed - fLifeOffset; \
    UNUSED const coreFloat fLifeTimeBeforeBase = (e)->GetLifeTimeBefore() * fLifeSpeed - fLifeOffset; \
    UNUSED coreFloat       fLifeTime           = fLifeTimeBase;                                       \
    UNUSED coreFloat       fLifeTimeBefore     = fLifeTimeBeforeBase;

#define STAGE_BRANCH(x,y)               ((fLifeTime < (x)) || [&]() {fLifeTime = FMOD(fLifeTime - (x), (y)); fLifeTimeBefore = FMOD(fLifeTimeBefore - (x), (y)); if(fLifeTimeBefore > fLifeTime) fLifeTimeBefore -= (y); return false;}())
#define STAGE_REPEAT(x)                 {if(STAGE_BRANCH(x, x)) {}}

#define STAGE_TICK_TIME_FREE(c,o)       ((m_fStageTimeBefore  >= 0.0f) && ((s_iTick = F_TO_UI(m_fStageTime  * (c) - (o))) != (F_TO_UI(m_fStageTimeBefore  * (c) - (o)))))
#define STAGE_TICK_TIME(c,o)            ((fLifeTimeBeforeBase >= 0.0f) && STAGE_TICK_TIME_FREE(c, o))
#define STAGE_TICK_LIFETIME(c,o)        ((fLifeTimeBeforeBase >= 0.0f) && ((s_iTick = F_TO_UI(fLifeTime     * (c) - (o))) != (F_TO_UI(fLifeTimeBefore     * (c) - (o)))))
#define STAGE_TICK_LIFETIME_BASE(c,o)   ((fLifeTimeBeforeBase >= 0.0f) && ((s_iTick = F_TO_UI(fLifeTimeBase * (c) - (o))) != (F_TO_UI(fLifeTimeBeforeBase * (c) - (o)))))

#define STAGE_TIME_POINT(t)             (InBetween((t), m_fStageTimeBefore, m_fStageTime))
#define STAGE_TIME_BEFORE(t)            (m_fStageTime <  (t))
#define STAGE_TIME_AFTER(t)             (m_fStageTime >= (t))
#define STAGE_TIME_BETWEEN(t,u)         (InBetween(m_fStageTime, (t), (u)))
#define STAGE_BEGINNING                 (STAGE_TIME_POINT(0.0f))

#define STAGE_LIFETIME_POINT(t)         (InBetween((t), fLifeTimeBefore, fLifeTime) && [&]() {s_fLifeTimePoint = (t); return true;}())
#define STAGE_LIFETIME_BEFORE(t)        (fLifeTime <  (t) && fLifeTime >= 0.0f)
#define STAGE_LIFETIME_AFTER(t)         (fLifeTime >= (t))
#define STAGE_LIFETIME_AFTER_BASE(t)         (fLifeTimeBase >= (t))
#define STAGE_LIFETIME_BETWEEN(t,u)     (InBetween(fLifeTime, (t), (u)))
#define STAGE_TAKEOFF                   (InBetween(0.0f, fLifeTimeBeforeBase, fLifeTimeBase) || (fLifeTimeBase == 0.0f))

#define STAGE_HEALTHPCT_POINT(e,t)      ((e)->ReachedHealthPct(t) && [&]() {s_fHealthPctPoint = (t); return true;}())
#define STAGE_HEALTHPCT_BEFORE(e,t)     ((e)->GetCurHealthPct() <  (t))
#define STAGE_HEALTHPCT_AFTER(e,t)      ((e)->GetCurHealthPct() >= (t))
#define STAGE_HEALTHPCT_BETWEEN(e,t,u)  (InBetween((e)->GetCurHealthPct(), (t), (u)))
#define STAGE_DIED(e)                   ((e)->ReachedDeath())

#define STAGE_POSITION_POINT(e,t,v)     (InBetweenExt((t), (e)->GetOldPos().v, (e)->GetPosition().v) && [&]() {s_vPositionPoint = (e)->GetPosition().xy(); s_vPositionPoint.v = (t); return true;}())
#define STAGE_POSITION_BEFORE(e,t,v)    ((e)->GetPosition().v <  (t))
#define STAGE_POSITION_AFTER(e,t,v)     ((e)->GetPosition().v >= (t))
#define STAGE_POSITION_BETWEEN(e,t,u,v) (InBetweenExt((e)->GetPosition().v, (t), (u)))
#define STAGE_FLYPAST(e,f,v)                         \
    ((e)->GetPosition().v < (f)->GetPosition().v) ^  \
    ((e)->GetPosition().v < (f)->GetOldPos  ().v) || \
    ((e)->GetOldPos  ().v < (f)->GetPosition().v) ^  \
    ((e)->GetOldPos  ().v < (f)->GetOldPos  ().v) || \
    ((f)->GetPosition().v < (e)->GetPosition().v) ^  \
    ((f)->GetPosition().v < (e)->GetOldPos  ().v) || \
    ((f)->GetOldPos  ().v < (e)->GetPosition().v) ^  \
    ((f)->GetOldPos  ().v < (e)->GetOldPos  ().v)


// ****************************************************************
// mission interface
class INTERFACE cMission
{
protected:
    cBoss* m_apBoss[MISSION_BOSSES];                           // pointers to all available bosses

    cBoss*    m_pCurBoss;                                      // pointer to currently active boss
    coreUintW m_iCurBossIndex;                                 // index of the active boss (or error-value)

    coreUintW m_iCurWaveCount;                                 // 
    coreUintW m_iCurWaveIndex;                                 // 

    coreUintW m_iCurSegmentIndex;                              // 

    coreLookup<coreUint16, std::function<void()>> m_anStage;   // 
    coreLookup<coreUint16, coreSpline2*>          m_apPath;    // 
    coreLookup<coreUint16, cEnemySquad*>          m_apSquad;   // 

    coreUint32* m_piData;                                      // 
    coreUint8   m_iDataSize;                                   // 

    coreUint16 m_iStageNum;                                    // 
    coreFlow   m_fStageTime;                                   // 
    coreFloat  m_fStageTimeBefore;                             // 
    coreUint8  m_iStageSub;                                    // 

    const coreFloat* m_pfMedalGoal;                            // 

    static coreUint16  s_iTick;                                // 
    static coreFloat   s_fLifeTimePoint;                       // 
    static coreFloat   s_fHealthPctPoint;                      // 
    static coreVector2 s_vPositionPoint;                       // 

    std::function<void(cPlayer* OUTPUT, cEnemy*  OUTPUT, const coreVector3&)> m_nCollPlayerEnemy;
    std::function<void(cPlayer* OUTPUT, cBullet* OUTPUT, const coreVector3&)> m_nCollPlayerBullet;
    std::function<void(cEnemy*  OUTPUT, cBullet* OUTPUT, const coreVector3&)> m_nCollEnemyBullet;


public:
    cMission()noexcept;
    virtual ~cMission();

    DISABLE_COPY(cMission)
    ENABLE_ID

    // setup the mission
    void Setup();

    // render and move the mission
    void RenderUnder();
    void RenderOver ();
    void RenderTop  ();
    void MoveBefore ();
    void MoveAfter  ();

    // 
    void            SkipStage ();
    inline coreBool IsFinished()const {return m_anStage.empty();}

    // 
    void ActivateBoss  (const cBoss* pBoss);
    void DeactivateBoss();

    // 
    void ActivateWave  (const coreChar* pcName);
    void DeactivateWave();

    // 
    inline void SetMedalGoal(const coreFloat* pfMedalGoal) {m_pfMedalGoal = pfMedalGoal; ASSERT(pfMedalGoal)}

    // access mission objects
    inline cBoss*           GetBoss           (const coreUintW iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*           GetCurBoss        ()const                       {return m_pCurBoss;}
    inline const coreUintW& GetCurBossIndex   ()const                       {return m_iCurBossIndex;}
    inline const coreUintW& GetCurWaveIndex   ()const                       {return m_iCurWaveIndex;}
    inline const coreUintW& GetCurSegmentIndex()const                       {return m_iCurSegmentIndex;}
    inline cEnemySquad*     GetEnemySquad     (const coreUintW iIndex)const {ASSERT(iIndex < m_apSquad.size()) return m_apSquad.get_valuelist()[iIndex];}



     
    inline void CollPlayerEnemy (cPlayer* OUTPUT pPlayer, cEnemy*  OUTPUT pEnemy,  const coreVector3& vIntersection) {if(m_nCollPlayerEnemy)  m_nCollPlayerEnemy (pPlayer, pEnemy,  vIntersection);}
    inline void CollPlayerBullet(cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreVector3& vIntersection) {if(m_nCollPlayerBullet) m_nCollPlayerBullet(pPlayer, pBullet, vIntersection);}
    inline void CollEnemyBullet (cEnemy*  OUTPUT pEnemy,  cBullet* OUTPUT pBullet, const coreVector3& vIntersection) {if(m_nCollEnemyBullet)  m_nCollEnemyBullet (pEnemy,  pBullet, vIntersection);}


protected:
    // 
    template             <typename F> coreSpline2* _AddPath (const coreUint16 iCodeLine,                       F&& nInitFunc);   // [](coreSpline2* OUTPUT pPath)  -> void
    template <typename T, typename F> cEnemySquad* _AddSquad(const coreUint16 iCodeLine, const coreUint8 iNum, F&& nInitFunc);   // [](cEnemySquad* OUTPUT pSquad) -> void

    // 
    //template <typename F> void _CollisionPlayerEnemy (const coreUint16 iCodeLine, F&& nHitFunc)   // TODO  
    //{
    //    if(!test) test = nHitFunc;
    //}
    //template <typename F> void _CollisionPlayerBullet(const coreUint16 iCodeLine, F&& nHitFunc);   // TODO  
    //template <typename F> void _CollisionEnemyBullet (const coreUint16 iCodeLine, F&& nHitFunc);   // TODO  


private:
    // own routines for derived classes
    virtual void __SetupOwn      () {}
    virtual void __RenderOwnUnder() {}
    virtual void __RenderOwnOver () {}
    virtual void __RenderOwnTop  () {}
    virtual void __MoveOwnBefore () {}
    virtual void __MoveOwnAfter  () {}

    // 
    void __CloseSegment();
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
    cDharukBoss m_Dharuk;                                   // 
    cTorusBoss  m_Torus;                                    // 
    cVausBoss   m_Vaus;                                     // 

    coreBatchList m_Ball;                                   // 
    coreBatchList m_BallTrail;                              // 
    coreObject3D  m_aBallRaw[VIRIDO_BALLS_RAWS];            // 

    coreObject3D m_aPaddle      [VIRIDO_PADDLES];           // 
    coreObject3D m_aPaddleSphere[VIRIDO_PADDLES];           // 
    const cShip* m_apPaddleOwner[VIRIDO_PADDLES];           // 

    coreBatchList m_Barrier;                                // 
    coreObject3D  m_aBarrierRaw   [VIRIDO_BARRIERS_RAWS];   // 
    const cShip*  m_apBarrierOwner[VIRIDO_BARRIERS];        // 
    coreUint8     m_aiBarrierDir  [VIRIDO_BARRIERS];        // 

    coreBatchList m_Laser;                                  // 
    coreBatchList m_LaserWave;                              // 
    coreObject3D  m_aLaserRaw   [VIRIDO_LASERS_RAWS];       // 
    const cShip*  m_apLaserOwner[VIRIDO_LASERS];            // 

    coreBatchList m_Shadow;                                 // 
    coreObject3D  m_aShadowRaw   [VIRIDO_SHADOWS_RAWS];     // 
    const cShip*  m_apShadowOwner[VIRIDO_SHADOWS];          // 

    coreUint8 m_iRealState;                                 // 
    coreUint8 m_iStickyState;                               // (only between first ball and first paddle) 
    coreUint8 m_iBounceState;                               // 

    coreFlow m_fAnimation;                                  // animation value


public:
    cViridoMission()noexcept;
    ~cViridoMission()final;

    DISABLE_COPY(cViridoMission)
    ASSIGN_ID(1, "Virido")

    // 
    void EnableBall (const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection);
    void DisableBall(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePaddle (const coreUintW iIndex, const cShip* pOwner);
    void DisablePaddle(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableBarrier (const coreUintW iIndex, const cShip* pOwner, const coreVector2& vDirection);
    void DisableBarrier(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableLaser (const coreUintW iIndex, const cShip* pOwner);
    void DisableLaser(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableShadow (const coreUintW iIndex, const cShip* pOwner, const coreVector2& vPosition);
    void DisableShadow(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void             MakeReal      (const coreUintW iIndex)        {ADD_BIT(m_iRealState, iIndex)}
    inline void             MakeSticky    ()                              {ADD_BIT(m_iStickyState, 0u)}
    inline void             UnmakeSticky  (const coreVector2& vDirection) {m_iStickyState = 0; m_aBallRaw[0].SetDirection(coreVector3(vDirection, 0.0f));}
    inline const coreUint8& GetRealState  ()const                         {return m_iRealState;}
    inline coreBool         GetStickyState()const                         {return CONTAINS_BIT(m_iStickyState, 1u);}
    inline const coreUint8& GetBounceState()const                         {return m_iBounceState;}

    // 
    inline coreObject3D* GetBall  (const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_BALLS)   return &m_aBallRaw[iIndex * (VIRIDO_TRAILS + 1u)];}
    inline coreObject3D* GetPaddle(const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_PADDLES) return &m_aPaddle [iIndex];}


private:
    // execute own routines
    void __SetupOwn      ()final;
    void __RenderOwnUnder()final;
    void __RenderOwnOver ()final;
    void __RenderOwnTop  ()final;
    void __MoveOwnBefore ()final;
    void __MoveOwnAfter  ()final;

    // 
    static void __BounceEffect(const coreVector2& vEffectPos);
};


// ****************************************************************
// Nevo mission class
class cNevoMission final : public cMission
{
private:
    cNautilusBoss  m_Nautilus;    // 
    cAmemasuBoss   m_Amemasu;     // 
    cLeviathanBoss m_Leviathan;   // 

    cLodObject  m_Container;      // 
    coreVector2 m_vForce;         // 
    coreVector2 m_vImpact;        // 
    coreBool    m_bClamp;         // 
    coreBool    m_bOverdraw;      // 


public:
    cNevoMission()noexcept;
    ~cNevoMission();

    DISABLE_COPY(cNevoMission)
    ASSIGN_ID(2, "Nevo")

    // 
    void EnableContainer (const coreVector2& vPosition);
    void DisableContainer(const coreBool bAnimated);

    // 
    inline void               SetContainerForce   (const coreVector2& vForce)    {m_vForce    = vForce;}
    inline void               SetContainerClamp   (const coreBool     bClamp)    {m_bClamp    = bClamp;}
    inline void               SetContainerOverdraw(const coreBool     bOverdraw) {m_bOverdraw = bOverdraw;}
    inline const coreVector2& GetContainerForce   ()const                        {return m_vForce;}
    inline const coreVector2& GetContainerImpact  ()const                        {return m_vImpact;}

    // 
    inline cLodObject* GetContainer() {return &m_Container;}


private:
    // execute own routines
    void __SetupOwn     ()final;
    void __RenderOwnOver()final;
    void __MoveOwnAfter ()final;
};


// ****************************************************************
// Harena mission class
class cHarenaMission final : public cMission
{
private:
    cUrticaBoss  m_Urtica;    // 
    cTigerBoss   m_Tiger;     // 
    cLuciferBoss m_Lucifer;   // 


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
    cQuaternionBoss m_Quaternion;   // 
    cSarosBoss      m_Saros;        // 
    cMessierBoss    m_Messier;      // 


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
    cTartarusBoss m_Tartarus;   // 
    cPhalarisBoss m_Phalaris;   // 
    cCholBoss     m_Chol;       // 


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
    cFenrirBoss m_Fenrir;   // 
    cShelobBoss m_Shelob;   // 
    cZerothBoss m_Zeroth;   // 


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
    cOrlacBoss   m_Orlac;     // 
    cGemingaBoss m_Geminga;   // 
    cNagualBoss  m_Nagual;    // 


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
private:
    cProjectOneBoss m_ProjectOne;   // 
    cEigengrauBoss  m_Eigengrau;    // 


public:
    cAterMission()noexcept;

    DISABLE_COPY(cAterMission)
    ASSIGN_ID(8, "Ater")


private:
    // execute own routines
    void __SetupOwn()final;
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
    void __SetupOwn     ()final;
    void __MoveOwnBefore()final;
};


// ****************************************************************
// 
class cErrorMission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cErrorMission()noexcept;

    DISABLE_COPY(cErrorMission)
    ASSIGN_ID(-1, "Error")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// 
class cTinkerMission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cTinkerMission()noexcept;

    DISABLE_COPY(cTinkerMission)
    ASSIGN_ID(-2, "Tinker")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// 
template <typename F> coreSpline2* cMission::_AddPath(const coreUint16 iCodeLine, F&& nInitFunc)
{
    if(!m_apPath.count(iCodeLine))
    {
        // 
        coreSpline2* pNewPath = new coreSpline2();
        nInitFunc(pNewPath);
        ASSERT(pNewPath->GetNumNodes() == pNewPath->GetCapacity())

        // 
        m_apPath.emplace(iCodeLine, pNewPath);
    }

    return m_apPath.at(iCodeLine);
}


// ****************************************************************
// 
template <typename T, typename F> cEnemySquad* cMission::_AddSquad(const coreUint16 iCodeLine, const coreUint8 iNum, F&& nInitFunc)
{
    if(!m_apSquad.count(iCodeLine))
    {
        // 
        cEnemySquad* pNewSquad = new cEnemySquad();
        pNewSquad->AllocateEnemies<T>(iNum);
        nInitFunc(pNewSquad);

        // 
        m_apSquad.emplace(iCodeLine, pNewSquad);
    }

    return m_apSquad.at(iCodeLine);
}


#endif // _P1_GUARD_MISSION_H_