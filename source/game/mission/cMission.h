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
// TODO: set progress when finishing segment, not when starting, but consider mission-wrapping
// TODO: low-resolution object_sphere for small sphere objects (what about bullet_orb) ?
// TODO: change all missions to STATIC_MEMORY (check memory, it would put all missions always in memory)
// TODO: check if TYPE_NEVO_BOMB still needed
// TODO: do not create objects and load resources of unused game-objects and bosses (e.g. move waves into own classes ? but then ?)
// TODO: in delay, replace cScoutEnemy with something which does not load any resources (may need to support instancing)


// ****************************************************************
// mission definitions
#define MISSION_PLAYERS    (PLAYERS)   // 
#define MISSION_BOSSES     (BOSSES)    // default number of bosses per mission
#define MISSION_WAVES      (WAVES)     // 
#define MISSION_NO_BOSS    (0xFFu)     // no boss currently active (error-value)
#define MISSION_NO_WAVE    (0xFFu)     // 
#define MISSION_NO_SEGMENT (0xFFu)     // 

#define MISSION_SEGMENT_IS_BOSS(i) ((i) >= MISSION_WAVES)
#define MISSION_BOSS_TO_SEGMENT(i) ((i) +  MISSION_WAVES)
#define MISSION_WAVE_TO_SEGMENT(i) ((i))

STATIC_ASSERT((MISSION_BOSSES == 2u) && (MISSION_WAVES == 10u))

#define TAKE_ALWAYS  (0x00u)
#define TAKE_MISSION (0xFFu)


// ****************************************************************
// mission specific definitions
#define VIRIDO_TRAILS               (4u)                                              // 
#define VIRIDO_BALLS                (2u)                                              // 
#define VIRIDO_BALLS_RAWS           (VIRIDO_BALLS * (VIRIDO_TRAILS + 1u))             // 
#define VIRIDO_PADDLES              (3u)                                              // 
#define VIRIDO_BARRIERS             (15u)                                             // 
#define VIRIDO_BARRIERS_RAWS        (VIRIDO_BARRIERS)                                 // 
#define VIRIDO_LASERS               (4u)                                              // 
#define VIRIDO_LASERS_RAWS          (VIRIDO_LASERS * 2u)                              // 
#define VIRIDO_SHADOWS              (16u)                                             // 
#define VIRIDO_SHADOWS_RAWS         (VIRIDO_SHADOWS)                                  // 
#define VIRIDO_BALL_SPEED           (1.5f)                                            // 

#define NEVO_BOMBS                  (6u)                                              // 
#define NEVO_BOMBS_RAWS             (NEVO_BOMBS)                                      // 
#define NEVO_LINES                  (4u)                                              // 
#define NEVO_BLASTS                 (NEVO_BOMBS)                                      // 
#define NEVO_BLASTS_RAWS            (NEVO_BLASTS * (NEVO_LINES + 1u))                 // 
#define NEVO_TILES                  (16u)                                             // 
#define NEVO_TILES_RAWS             (NEVO_TILES)                                      // 
#define NEVO_ARROWS                 (21u)                                             // 
#define NEVO_ARROWS_RAWS            (NEVO_ARROWS)                                     // 
#define NEVO_BLOCKS                 (12u)                                             // 
#define NEVO_BLOCKS_RAWS            (NEVO_BLOCKS * 2u)                                // 
#define NEVO_BOMB_SIZE              (4.0f)                                            // 

#define HARENA_SPIKES               (36u)                                             // 
#define HARENA_SPIKES_RAWS          (HARENA_SPIKES * 2u)                              // 
#define HARENA_SPIKE_DIMENSION      (6u)                                              // 

#define RUTILUS_TELEPORTER          (2u)                                              // 
#define RUTILUS_TELEPORTER_COLOR(x) ((x) ? COLOR_ENERGY_BLUE : COLOR_ENERGY_ORANGE)   // 
#define RUTILUS_PLATES              (5u)                                              // 
#define RUTILUS_PLATES_RAWS         (RUTILUS_PLATES)                                  // 
#define RUTILUS_AREAS               (2u)                                              // 
#define RUTILUS_WAVES               (4u)                                              // 
#define RUTILUS_WAVES_RAWS          (RUTILUS_WAVES)                                   // 

#define GELU_FANGS                  (25u)                                             // 
#define GELU_FANGS_RAWS             (GELU_FANGS)                                      // 
#define GELU_FANGS_DIMENSION        (5u)                                              // 
#define GELU_WAYS                   (26u)                                             // 
#define GELU_WAYS_RAWS              (GELU_WAYS * 2u)                                  // 
#define GELU_ORBS                   (16u)                                             // 
#define GELU_ORBS_RAWS              (GELU_ORBS)                                       // 
#define GELU_LINES                  (24u)                                             // 
#define GELU_LINES_RAWS             (GELU_LINES)                                      // 

#define CALOR_LOADS                 (12u)                                             // 
#define CALOR_LOADS_RAWS            (CALOR_LOADS)                                     // 
#define CALOR_CHAINS                (16u)                                             // 
#define CALOR_STARS                 (MISSION_PLAYERS)                                 // 
#define CALOR_STARS_RAWS            (CALOR_STARS * (CALOR_CHAINS + 1u))               // 
#define CALOR_CHAIN_CONSTRAINT      (46.0f)                                           // 

#define MUSCUS_GENERATES            (24u)                                             // 
#define MUSCUS_GENERATES_RAWS       (MUSCUS_GENERATES * 2u)                           // 
#define MUSCUS_PEARLS               (21u)                                             // 
#define MUSCUS_PEARLS_RAWS          (MUSCUS_PEARLS * 2u)                              // 


// ****************************************************************
// stage management macros
#define STAGE_MAIN(...)                        if([this]() {static constexpr coreUint8 A[] = __VA_ARGS__; return cMission::_TakeRange(m_iTakeFrom, m_iTakeTo, A, ARRAY_SIZE(A));}()) m_anStage.emplace(__LINE__, [this]()
#define STAGE_SUB(i)                           ((m_iStageSub < (i)) && [&]() {m_iStageSub = (i); m_fStageSubTime = 0.0f; m_fStageSubTimeBefore = 0.0f; return true;}())

#define STAGE_FINISH_NOW                       {this->SkipStage();}
#define STAGE_FINISH_PLAY                      {if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) STAGE_FINISH_NOW}
#define STAGE_FINISH_AFTER(t)                  {if(m_fStageTime >= (t))                              STAGE_FINISH_NOW}

#define STAGE_MEDAL_GOAL(...)                  {static constexpr coreFloat A[] = __VA_ARGS__; this->SetMedalGoal(A); STATIC_ASSERT((ARRAY_SIZE(A) == 4u) && (A[0] < A[1]) && (A[1] < A[2]) && (A[2] < A[3]))}
#define STAGE_BOSS(e,...)                      {if(STAGE_BEGINNING) {STAGE_MEDAL_GOAL(__VA_ARGS__) (e).Resurrect();} if((e).HasStatus(ENEMY_STATUS_DEAD)) STAGE_FINISH_NOW}
#define STAGE_WAVE(n,...)                      {if(STAGE_BEGINNING) {STAGE_MEDAL_GOAL(__VA_ARGS__) this->ActivateWave(n);} if(STAGE_CLEARED) {this->DeactivateWave(); if(this->_UpdateWait()) STAGE_FINISH_NOW}}

#define STAGE_START_HERE                       {m_anStage.clear(); STAGE_MAIN({TAKE_ALWAYS}) {if(STAGE_BEGINNING) g_pGame->StartIntro(); STAGE_FINISH_PLAY});}

#define STAGE_CLEARED                          (std::all_of(m_apSquad.begin(), m_apSquad.end(), [](const cEnemySquad* pSquad) {return pSquad->IsFinished();}))
#define STAGE_RESURRECT(s,f,t)                 {STAGE_FOREACH_ENEMY_ALL(s, pEnemy, i) {if((coreInt32(i) >= coreInt32(f)) && (coreInt32(i) <= coreInt32(t))) pEnemy->Resurrect();}); ASSERT((coreInt32(f) <= coreInt32(t)) && (coreInt32(t) < coreInt32((s)->GetNumEnemies())))}
#define STAGE_BADGE(b,p)                       {this->GiveBadge(b, p);}

#define STAGE_DELAY_START                      {UNUSED STAGE_ADD_SQUAD(pDelay, cScoutEnemy, 1u) {pDelay->GetEnemy(0u)->Configure(1, COLOR_SHIP_GREY); pDelay->GetEnemy(0u)->Resurrect();});}
#define STAGE_DELAY_END                        {m_apSquad.back()->GetEnemy(0u)->Kill(false);}

#define STAGE_ADD_PATH(n)                      const auto n = this->_AddPath    (__LINE__,      [](coreSpline2* OUTPUT n)
#define STAGE_ADD_SQUAD(n,t,c)                 const auto n = this->_AddSquad<t>(__LINE__, (c), [](cEnemySquad* OUTPUT n)

#define STAGE_COLL_PLAYER_ENEMY(a,b,i,f,...)   if(!m_nCollPlayerEnemy)  m_nCollPlayerEnemy  = ([__VA_ARGS__](cPlayer* OUTPUT a, cEnemy*  OUTPUT b, const coreVector3& i, const coreBool f)   // NOLINT
#define STAGE_COLL_PLAYER_BULLET(a,b,i,f,...)  if(!m_nCollPlayerBullet) m_nCollPlayerBullet = ([__VA_ARGS__](cPlayer* OUTPUT a, cBullet* OUTPUT b, const coreVector3& i, const coreBool f)   // NOLINT
#define STAGE_COLL_ENEMY_BULLET(a,b,i,f,...)   if(!m_nCollEnemyBullet)  m_nCollEnemyBullet  = ([__VA_ARGS__](cEnemy*  OUTPUT a, cBullet* OUTPUT b, const coreVector3& i, const coreBool f)   // NOLINT
#define COLL_VAL(x)                             x = s_cast<typename std::conditional<!std::is_reference<decltype(x)>::value, decltype(x), void>::type>(x)
#define COLL_REF(x)                            &x = s_cast<typename std::conditional< std::is_reference<decltype(x)>::value, decltype(x), void>::type>(x)
#define COLL_THIS                              this

#define STAGE_FOREACH_PLAYER(e,i)              g_pGame->ForEachPlayer   ([&](cPlayer* OUTPUT e, const coreUintW i)   // NOLINT
#define STAGE_FOREACH_PLAYER_ALL(e,i)          g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT e, const coreUintW i)   // NOLINT
#define STAGE_FOREACH_ENEMY(s,e,i)             (s)->ForEachEnemy        ([&](cEnemy*  OUTPUT e, const coreUintW i)   // NOLINT
#define STAGE_FOREACH_ENEMY_ALL(s,e,i)         (s)->ForEachEnemyAll     ([&](cEnemy*  OUTPUT e, const coreUintW i)   // NOLINT

#define STAGE_GET_START(c)                     {if((c) > m_iDataSize) {ZERO_DELETE(m_piData) STATIC_ASSERT((c) < 0xFFu) m_iDataSize = (c); m_piData = ZERO_NEW(coreUint32, m_iDataSize);}} coreUintW iDataIndex = 0u; constexpr coreUintW iCurDataSize = (c);
#define STAGE_GET_END                          {ASSERT(iDataIndex == iCurDataSize)}
#define STAGE_GET_INT(n,...)                   coreInt32&                n = r_cast<coreInt32&>  ( m_piData[iDataIndex]); iDataIndex += 1u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT(n,...)                  coreUint32&               n = r_cast<coreUint32&> ( m_piData[iDataIndex]); iDataIndex += 1u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT64(n,...)                coreUint64&               n = r_cast<coreUint64&> ( m_piData[iDataIndex]); iDataIndex += 2u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_FLOAT(n,...)                 coreFloat&                n = r_cast<coreFloat&>  ( m_piData[iDataIndex]); iDataIndex += 1u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC2(n,...)                  coreVector2&              n = r_cast<coreVector2&>( m_piData[iDataIndex]); iDataIndex += 2u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC3(n,...)                  coreVector3&              n = r_cast<coreVector3&>( m_piData[iDataIndex]); iDataIndex += 3u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC4(n,...)                  coreVector4&              n = r_cast<coreVector4&>( m_piData[iDataIndex]); iDataIndex += 4u;       {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_INT_ARRAY(n,c,...)           coreInt32*   const OUTPUT n = r_cast<coreInt32*>  (&m_piData[iDataIndex]); iDataIndex += 1u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT_ARRAY(n,c,...)          coreUint32*  const OUTPUT n = r_cast<coreUint32*> (&m_piData[iDataIndex]); iDataIndex += 1u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_UINT64_ARRAY(n,c,...)        coreUint64*  const OUTPUT n = r_cast<coreUint64*> (&m_piData[iDataIndex]); iDataIndex += 2u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_FLOAT_ARRAY(n,c,...)         coreFloat*   const OUTPUT n = r_cast<coreFloat*>  (&m_piData[iDataIndex]); iDataIndex += 1u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC2_ARRAY(n,c,...)          coreVector2* const OUTPUT n = r_cast<coreVector2*>(&m_piData[iDataIndex]); iDataIndex += 2u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC3_ARRAY(n,c,...)          coreVector3* const OUTPUT n = r_cast<coreVector3*>(&m_piData[iDataIndex]); iDataIndex += 3u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}
#define STAGE_GET_VEC4_ARRAY(n,c,...)          coreVector4* const OUTPUT n = r_cast<coreVector4*>(&m_piData[iDataIndex]); iDataIndex += 4u * (c); {if(STAGE_BEGINNING) {__VA_ARGS__;}}

#define STAGE_LIFETIME(e,m,a)                                                                         \
    UNUSED const coreFloat fLifeSpeed          = (m);                                                 \
    UNUSED const coreFloat fLifeOffset         = (a);                                                 \
    UNUSED const coreFloat fLifeTimeBase       = (e)->GetLifeTime()       * fLifeSpeed - fLifeOffset; \
    UNUSED const coreFloat fLifeTimeBeforeBase = (e)->GetLifeTimeBefore() * fLifeSpeed - fLifeOffset; \
    UNUSED coreFloat       fLifeTime           = fLifeTimeBase;                                       \
    UNUSED coreFloat       fLifeTimeBefore     = fLifeTimeBeforeBase;

#define STAGE_BRANCH(x,y)                      ((fLifeTime < (x)) || [&]() {fLifeTime = FMOD(fLifeTime - (x), (y)); fLifeTimeBefore = FMOD(fLifeTimeBefore - (x), (y)); if(fLifeTimeBefore > fLifeTime) fLifeTimeBefore -= (y); return false;}())
#define STAGE_REPEAT(x)                        {if(STAGE_BRANCH(x, x)) {}}

#define STAGE_TICK_FREE(c,o)                   ((m_fStageTimeBefore  >= 0.0f) && ((s_iTick = F_TO_UI(m_fStageTime  * (c) - (o))) != (F_TO_UI(m_fStageTimeBefore  * (c) - (o)))))
#define STAGE_TICK_TIME(c,o)                   ((fLifeTimeBeforeBase >= 0.0f) && STAGE_TICK_FREE(c, o))
#define STAGE_TICK_LIFETIME(c,o)               ((fLifeTimeBeforeBase >= 0.0f) && ((s_iTick = F_TO_UI(fLifeTime     * (c) - (o))) != (F_TO_UI(fLifeTimeBefore     * (c) - (o)))))
#define STAGE_TICK_LIFETIME_BASE(c,o)          ((fLifeTimeBeforeBase >= 0.0f) && ((s_iTick = F_TO_UI(fLifeTimeBase * (c) - (o))) != (F_TO_UI(fLifeTimeBeforeBase * (c) - (o)))))

#define STAGE_TIME_POINT(t)                    (InBetween((t), m_fStageTimeBefore, m_fStageTime))
#define STAGE_TIME_BEFORE(t)                   (m_fStageTime <  (t))
#define STAGE_TIME_AFTER(t)                    (m_fStageTime >= (t))
#define STAGE_TIME_BETWEEN(t,u)                (InBetween(m_fStageTime, (t), (u)))
#define STAGE_BEGINNING                        (STAGE_TIME_POINT(0.0f))

#define STAGE_SUBTIME_POINT(t)                 (InBetween((t), m_fStageSubTimeBefore, m_fStageSubTime))
#define STAGE_SUBTIME_BEFORE(t)                (m_fStageSubTime <  (t))
#define STAGE_SUBTIME_AFTER(t)                 (m_fStageSubTime >= (t))
#define STAGE_SUBTIME_BETWEEN(t,u)             (InBetween(m_fStageSubTime, (t), (u)))

#define STAGE_LIFETIME_POINT(t)                (InBetween((t), fLifeTimeBefore, fLifeTime) && [&]() {s_fLifeTimePoint = (t); return true;}())
#define STAGE_LIFETIME_BEFORE(t)               (fLifeTime     <  (t) && fLifeTime     >= 0.0f)
#define STAGE_LIFETIME_BEFORE_BASE(t)          (fLifeTimeBase <  (t) && fLifeTimeBase >= 0.0f)
#define STAGE_LIFETIME_AFTER(t)                (fLifeTime     >= (t))
#define STAGE_LIFETIME_AFTER_BASE(t)           (fLifeTimeBase >= (t))
#define STAGE_LIFETIME_BETWEEN(t,u)            (InBetween(fLifeTime, (t), (u)))
#define STAGE_TAKEOFF                          (InBetween(0.0f, fLifeTimeBeforeBase, fLifeTimeBase) || (fLifeTimeBase == 0.0f))

#define STAGE_HEALTHPCT_POINT(e,t)             ((e)->ReachedHealthPct(t) && [&]() {s_fHealthPctPoint = (t); return true;}())
#define STAGE_HEALTHPCT_BEFORE(e,t)            ((e)->GetCurHealthPct() <  (t))
#define STAGE_HEALTHPCT_AFTER(e,t)             ((e)->GetCurHealthPct() >= (t))
#define STAGE_HEALTHPCT_BETWEEN(e,t,u)         (InBetween((e)->GetCurHealthPct(), (t), (u)))
#define STAGE_DIED(e)                          ((e)->ReachedDeath())

#define STAGE_POSITION_POINT(e,t,v)            (InBetweenExt((t), (e)->GetOldPos().v, (e)->GetPosition().v) && [&]() {s_vPositionPoint = (e)->GetPosition().xy(); s_vPositionPoint.v = (t); return true;}())
#define STAGE_POSITION_BEFORE(e,t,v)           ((e)->GetPosition().v <  (t))
#define STAGE_POSITION_AFTER(e,t,v)            ((e)->GetPosition().v >= (t))
#define STAGE_POSITION_BETWEEN(e,t,u,v)        (InBetweenExt((e)->GetPosition().v, (t), (u)))
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
private:
    // 
    using uCollPlayerEnemyType  = std::function<void(cPlayer* OUTPUT, cEnemy*  OUTPUT, const coreVector3&, const coreBool)>;
    using uCollPlayerBulletType = std::function<void(cPlayer* OUTPUT, cBullet* OUTPUT, const coreVector3&, const coreBool)>;
    using uCollEnemyBulletType  = std::function<void(cEnemy*  OUTPUT, cBullet* OUTPUT, const coreVector3&, const coreBool)>;


protected:
    cBoss* m_apBoss[MISSION_BOSSES];                        // pointers to all available bosses

    cBoss*    m_pCurBoss;                                   // pointer to currently active boss
    coreUintW m_iCurBossIndex;                              // index of the active boss (or error-value)

    coreUintW m_iCurWaveCount;                              // 
    coreUintW m_iCurWaveIndex;                              // 

    coreUintW m_iCurSegmentIndex;                           // 

    coreMap<coreUint16, std::function<void()>> m_anStage;   // 
    coreMap<coreUint16, coreSpline2*>          m_apPath;    // 
    coreMap<coreUint16, cEnemySquad*>          m_apSquad;   // 

    coreUint32* m_piData;                                   // 
    coreUint8   m_iDataSize;                                // 

    coreUint16 m_iStageNum;                                 // 
    coreFlow   m_fStageTime;                                // 
    coreFloat  m_fStageTimeBefore;                          // 
    coreUint8  m_iStageSub;                                 // 
    coreFlow   m_fStageSubTime;                             // 
    coreFloat  m_fStageSubTimeBefore;                       // 
    coreFlow   m_fStageWait;                                // 

    const coreFloat* m_pfMedalGoal;                         // 

    coreBool m_bBadgeGiven;                                 // 

    uCollPlayerEnemyType  m_nCollPlayerEnemy;               // 
    uCollPlayerBulletType m_nCollPlayerBullet;              // 
    uCollEnemyBulletType  m_nCollEnemyBullet;               // 

    coreUint8 m_iTakeFrom;                                  // 
    coreUint8 m_iTakeTo;                                    // 

    coreBool m_bRepeat;                                     // 

    static coreUint16  s_iTick;                             // 
    static coreFloat   s_fLifeTimePoint;                    // 
    static coreFloat   s_fHealthPctPoint;                   // 
    static coreVector2 s_vPositionPoint;                    // 


public:
    cMission()noexcept;
    virtual ~cMission();

    DISABLE_COPY(cMission)
    ENABLE_ID

    // setup the mission
    void Setup(const coreUint8 iTakeFrom, const coreUint8 iTakeTo);

    // render and move the mission
    void RenderBottom();
    void RenderUnder ();
    void RenderOver  ();
    void RenderTop   ();
    void MoveBefore  ();
    void MoveMiddle  ();
    void MoveAfter   ();

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

    // 
    void GiveBadge(const coreUint8 iBadge, const coreVector3& vPosition);

    // 
    inline void CollPlayerEnemy (cPlayer* OUTPUT pPlayer, cEnemy*  OUTPUT pEnemy,  const coreVector3& vIntersection, const coreBool bFirstHit) {if(m_nCollPlayerEnemy)  m_nCollPlayerEnemy (pPlayer, pEnemy,  vIntersection, bFirstHit);}
    inline void CollPlayerBullet(cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit) {if(m_nCollPlayerBullet) m_nCollPlayerBullet(pPlayer, pBullet, vIntersection, bFirstHit);}
    inline void CollEnemyBullet (cEnemy*  OUTPUT pEnemy,  cBullet* OUTPUT pBullet, const coreVector3& vIntersection, const coreBool bFirstHit) {if(m_nCollEnemyBullet)  m_nCollEnemyBullet (pEnemy,  pBullet, vIntersection, bFirstHit);}

    // access mission objects
    inline cBoss*           GetBoss           (const coreUintW iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*           GetCurBoss        ()const                       {return m_pCurBoss;}
    inline const coreUintW& GetCurBossIndex   ()const                       {return m_iCurBossIndex;}
    inline const coreUintW& GetCurWaveIndex   ()const                       {return m_iCurWaveIndex;}
    inline const coreUintW& GetCurSegmentIndex()const                       {return m_iCurSegmentIndex;}
    inline cEnemySquad*     GetEnemySquad     (const coreUintW iIndex)const {ASSERT(iIndex < m_apSquad.size()) return m_apSquad.get_valuelist()[iIndex];}


protected:
    // 
    template             <typename F> coreSpline2* _AddPath (const coreUint16 iCodeLine,                       F&& nInitFunc);   // [](coreSpline2* OUTPUT pPath)  -> void
    template <typename T, typename F> cEnemySquad* _AddSquad(const coreUint16 iCodeLine, const coreUint8 iNum, F&& nInitFunc);   // [](cEnemySquad* OUTPUT pSquad) -> void

    // 
    inline coreBool _UpdateWait() {m_fStageWait.UpdateMax(-1.0f, 0.0f); return !m_fStageWait;}

    // 
    static constexpr FUNC_LOCAL coreBool _TakeRange(const coreUint8 iFrom, const coreUint8 iTo, const coreUint8* piIndexList, const coreUintW iSize);


private:
    // own routines for derived classes
    virtual void __SetupOwn       () {}
    virtual void __RenderOwnBottom() {}
    virtual void __RenderOwnUnder () {}
    virtual void __RenderOwnOver  () {}
    virtual void __RenderOwnTop   () {}
    virtual void __MoveOwnBefore  () {}
    virtual void __MoveOwnMiddle  () {}
    virtual void __MoveOwnAfter   () {}

    // 
    void __OpenSegment ();
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
    cTorusBoss m_Torus;                                     // 

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
    void EnableBarrier (const coreUintW iIndex, const cShip* pOwner, const coreVector2& vDirection, const coreFloat fSize);
    void DisableBarrier(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableLaser (const coreUintW iIndex, const cShip* pOwner);
    void DisableLaser(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableShadow (const coreUintW iIndex, const cShip* pOwner, const coreVector2& vPosition);
    void DisableShadow(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void MakeReal    (const coreUintW iIndex)        {ADD_BIT(m_iRealState, iIndex)}
    inline void MakeSticky  ()                              {ADD_BIT(m_iStickyState, 0u)}
    inline void UnmakeSticky(const coreVector2& vDirection) {m_iStickyState = 0; m_aBallRaw[0].SetDirection(coreVector3(vDirection, 0.0f));}

    // 
    inline const coreUint8& GetRealState  ()const {return m_iRealState;}
    inline coreBool         GetStickyState()const {return HAS_BIT(m_iStickyState, 1u);}
    inline const coreUint8& GetBounceState()const {return m_iBounceState;}

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
    cLeviathanBoss m_Leviathan;                       // 

    coreBatchList m_Bomb;                             // 
    cLodObject    m_aBombRaw  [NEVO_BOMBS_RAWS];      // 
    coreBool      m_abBombGone[NEVO_BOMBS];           // 

    coreBatchList m_Blast;                            // 
    coreBatchList m_BlastLine;                        // 
    coreObject3D  m_aBlastRaw  [NEVO_BLASTS_RAWS];    // 
    coreFlow      m_afBlastTime[NEVO_BLASTS];         // 

    coreBatchList m_Tile;                             // 
    coreObject3D  m_aTileRaw  [NEVO_TILES_RAWS];      // 
    coreFlow      m_afTileTime[NEVO_TILES];           // 

    coreBatchList m_Arrow;                            // 
    coreObject3D  m_aArrowRaw   [NEVO_ARROWS_RAWS];   // 
    const cShip*  m_apArrowOwner[NEVO_ARROWS];        // 
    coreUint8     m_aiArrowDir  [NEVO_ARROWS];        // 

    coreBatchList m_Block;                            // 
    coreBatchList m_BlockWave;                        // 
    coreObject3D  m_aBlockRaw   [NEVO_BLOCKS_RAWS];   // 
    const cShip*  m_apBlockOwner[NEVO_BLOCKS];        // 
    coreFloat     m_afBlockScale[NEVO_BLOCKS];        // 

    cLodObject  m_Container;                          // 
    coreVector2 m_vForce;                             // 
    coreVector2 m_vImpact;                            // 
    coreBool    m_bClamp;                             // 
    coreBool    m_bOverdraw;                          // 

    coreFlow m_fAnimation;                            // animation value


public:
    cNevoMission()noexcept;
    ~cNevoMission()final;

    DISABLE_COPY(cNevoMission)
    ASSIGN_ID(2, "Nevo")

    // 
    void EnableBomb (const coreUintW iIndex, const coreBool bGrow);
    void DisableBomb(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableBlast (const coreUintW iIndex);
    void DisableBlast(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableTile (const coreUintW iIndex, const coreUintW iDimension);
    void DisableTile(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableArrow (const coreUintW iIndex, const cShip* pOwner, const coreVector2& vDirection);
    void DisableArrow(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableBlock (const coreUintW iIndex, const cShip* pOwner, const coreFloat fScale);
    void DisableBlock(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableContainer (const coreVector2& vPosition);
    void DisableContainer(const coreBool bAnimated);

    // 
    inline void SetContainerForce   (const coreVector2& vForce)    {m_vForce    = vForce;}
    inline void SetContainerClamp   (const coreBool     bClamp)    {m_bClamp    = bClamp;}
    inline void SetContainerOverdraw(const coreBool     bOverdraw) {m_bOverdraw = bOverdraw;}

    // 
    inline const coreBool&    GetBombGone       (const coreUintW iIndex)const {ASSERT(iIndex < NEVO_BOMBS) return m_abBombGone[iIndex];}
    inline const coreVector2& GetContainerForce ()const                       {return m_vForce;}
    inline const coreVector2& GetContainerImpact()const                       {return m_vImpact;}

    // 
    inline cLodObject* GetContainer() {return &m_Container;}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __MoveOwnAfter   ()final;
};


// ****************************************************************
// Harena mission class
class cHarenaMission final : public cMission
{
private:
    cTigerBoss m_Tiger;                                // 

    coreBatchList m_Spike;                             // 
    coreBatchList m_SpikeBoard;                        // 
    coreObject3D  m_aSpikeRaw  [HARENA_SPIKES_RAWS];   // 
    coreFlow      m_afSpikeTime[HARENA_SPIKES];        // 
    coreFlow      m_afSpikeCur [HARENA_SPIKES];        // 
    coreFloat     m_afSpikeMax [HARENA_SPIKES];        // 


public:
    cHarenaMission()noexcept;
    ~cHarenaMission()final;

    DISABLE_COPY(cHarenaMission)
    ASSIGN_ID(3, "Harena")

    // 
    void EnableSpike (const coreUintW iIndex);
    void DisableSpike(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void LaunchSpike(const coreUintW iIndex, const coreFloat fTime) {ASSERT(iIndex < HARENA_SPIKES) m_afSpikeCur[iIndex] = 0.0f; m_afSpikeMax[iIndex] = fTime;}

    // 
    inline coreBool GetSpikeLaunched(const coreUintW iIndex)const {ASSERT(iIndex < HARENA_SPIKES) return (m_afSpikeMax[iIndex] && InBetween(m_afSpikeCur[iIndex], 1.1f, m_afSpikeMax[iIndex] - 0.5f));}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __MoveOwnAfter   ()final;
};


// ****************************************************************
// Rutilus mission class
class cRutilusMission final : public cMission
{
private:
    cMessierBoss m_Messier;                                // 

    coreObject3D m_aTeleporter     [RUTILUS_TELEPORTER];   // 
    coreVector2  m_avTeleporterPrev[RUTILUS_TELEPORTER];   // 
    coreUint8    m_iTeleporterActive;                      // 

    coreBatchList m_Plate;                                 // 
    coreObject3D  m_aPlateRaw  [RUTILUS_PLATES_RAWS];      // 
    coreFlow      m_afPlateTime[RUTILUS_PLATES];           // 
    coreVector4   m_avPlateData[RUTILUS_PLATES];           // 

    coreObject3D m_aArea[RUTILUS_AREAS];                   // 
    coreFlow     m_fAreaTime;                              // 

    coreBatchList m_Wave;                                  // 
    coreObject3D  m_aWaveRaw  [RUTILUS_WAVES_RAWS];        // 
    coreFlow      m_afWaveTime[RUTILUS_WAVES];             // 
    coreUint8     m_iWaveActive;                           // 
    coreUint8     m_iWaveDir;                              // 

    coreUint8 m_aiMoveFlip[MISSION_PLAYERS];               // 

    coreFlow m_fAnimation;                                 // animation value


public:
    cRutilusMission()noexcept;
    ~cRutilusMission()final;

    DISABLE_COPY(cRutilusMission)
    ASSIGN_ID(4, "Rutilus")

    // 
    void EnableTeleporter (const coreUintW iIndex);
    void DisableTeleporter(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePlate (const coreUintW iIndex, const coreFloat fFrom, const coreFloat fTo, const coreFloat fScale);
    void DisablePlate(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableArea ();
    void DisableArea(const coreBool bAnimated);

    // 
    void EnableWave ();
    void DisableWave(const coreBool bAnimated);

    // 
    inline void SetTeleporterActive(const coreUint8 iActive) {m_iTeleporterActive = iActive;}

    // 
    inline void SetPlateOffset(const coreUintW iIndex, const coreFloat fOffset) {ASSERT(iIndex < RUTILUS_PLATES) m_afPlateTime[iIndex] = 0.0f; m_avPlateData[iIndex].xy(coreVector2(m_avPlateData[iIndex].y, fOffset));}
    inline void SetPlateScale (const coreUintW iIndex, const coreFloat fScale)  {ASSERT(iIndex < RUTILUS_PLATES) m_afPlateTime[iIndex] = 0.0f; m_avPlateData[iIndex].zw(coreVector2(m_avPlateData[iIndex].w, fScale));}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnOver  ()final;
    void __MoveOwnBefore  ()final;
    void __MoveOwnAfter   ()final;

    // 
    void __TeleporterEffect(const coreUintW iIndex)const;
};


// ****************************************************************
// Gelu mission class
class cGeluMission final : public cMission
{
private:
    cCholBoss m_Chol;                              // 

    coreBatchList m_Fang;                          // 
    cLodObject    m_aFangRaw[GELU_FANGS_RAWS];     // 

    coreBatchList m_Way;                           // 
    coreBatchList m_WayArrow;                      // 
    coreObject3D  m_aWayRaw[GELU_WAYS_RAWS];       // 
    coreUint32    m_iWayActive;                    // 
    coreUint32    m_iWayVisible;                   // 

    coreBatchList m_Orb;                           // 
    coreObject3D  m_aOrbRaw  [GELU_ORBS_RAWS];     // 
    coreFlow      m_afOrbTime[GELU_ORBS];          // 

    coreBatchList m_Line;                          // 
    coreObject3D  m_aLineRaw  [GELU_LINES_RAWS];   // 
    coreFlow      m_afLineTime[GELU_LINES];        // 

    coreFlow m_fAnimation;                         // animation value


public:
    cGeluMission()noexcept;
    ~cGeluMission()final;

    DISABLE_COPY(cGeluMission)
    ASSIGN_ID(5, "Gelu")

    // 
    void EnableFang (const coreUintW iIndex);
    void DisableFang(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableWay (const coreUintW iIndex, const coreVector2& vPosition, const coreVector2& vDirection);
    void DisableWay(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableOrb (const coreUintW iIndex);
    void DisableOrb(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableLine (const coreUintW iIndex);
    void DisableLine(const coreUintW iIndex, const coreBool bAnimated);


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __MoveOwnAfter   ()final;
};


// ****************************************************************
// Calor mission class
class cCalorMission final : public cMission
{
private:
    cZerothBoss m_Zeroth;                            // 

    cSnow m_Snow;                                    // 

    coreBatchList m_Load;                            // 
    coreObject3D  m_aLoadRaw[CALOR_LOADS_RAWS];      // 
    coreObject3D  m_LoadCopy;                        // 
    const cShip*  m_pLoadOwner;                      // 
    coreFlow      m_afLoadPower[3];                  // (0 = current | 1 = previous | 2 = bump) 

    coreBatchList m_Star;                            // 
    coreBatchList m_StarChain;                       // 
    coreObject3D  m_aStarRaw   [CALOR_STARS_RAWS];   // 
    const cShip*  m_apStarOwner[CALOR_STARS];        // 
    coreUint8     m_iStarState;                      // 

    coreFlow m_fSwingStart;                          // 
    coreFlow m_fSwingValue;                          // 

    cEnemy*     m_apCatchObject[CALOR_STARS];        // 
    coreVector2 m_avCatchPos   [CALOR_STARS];        // 
    coreVector2 m_avCatchDir   [CALOR_STARS];        // 
    coreFlow    m_fCatchTransfer;                    // 

    cCustomEnemy m_Boulder;                          // 

    coreFlow m_fAnimation;                           // animation value


public:
    cCalorMission()noexcept;
    ~cCalorMission()final;

    DISABLE_COPY(cCalorMission)
    ASSIGN_ID(6, "Calor")

    // 
    void EnableLoad (const cShip* pOwner);
    void DisableLoad(const coreBool bAnimated);

    // 
    void EnableStar (const coreUintW iIndex, const cShip* pOwner);
    void DisableStar(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void BumpLoad(const coreFloat fValue) {ASSERT(fValue > 0.0f) m_afLoadPower[0] = MIN(m_afLoadPower[0] + fValue, I_TO_F(CALOR_LOADS)); m_afLoadPower[2] = 1.0f;}

    // 
    inline void StartSwing() {m_iStarState = BITLINE(CALOR_STARS); m_fSwingStart = 0.0f; m_fSwingValue = 0.0f;}

    // 
    void CatchObject  (const coreUintW iIndex, cEnemy* pObject);
    void UncatchObject(const coreUintW iIndex);

    // 
    inline cEnemy* GetCatchObject(const coreUintW iIndex)const {ASSERT(iIndex < CALOR_STARS) return m_apCatchObject[iIndex];}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __RenderOwnTop   ()final;
    void __MoveOwnMiddle  ()final;
    void __MoveOwnAfter   ()final;
};


// ****************************************************************
// Muscus mission class
class cMuscusMission final : public cMission
{
private:
    cGemingaBoss m_Geminga;                                  // 

    coreBatchList m_Generate;                                // 
    coreBatchList m_GenerateWave;                            // 
    coreObject3D  m_aGenerateRaw  [MUSCUS_GENERATES_RAWS];   // 
    coreFlow      m_afGenerateTime[MUSCUS_GENERATES];        // 
    coreFlow      m_afGenerateBang[MUSCUS_GENERATES];        // 
    coreFlow      m_afGenerateView[MUSCUS_GENERATES];        // 

    coreBatchList m_Pearl;                                   // 
    coreBatchList m_PearlWave;                               // 
    coreObject3D  m_aPearlRaw[MUSCUS_PEARLS_RAWS];           // 
    coreUint32    m_iPearlActive;                            // 

    coreFlow m_fAnimation;                                   // animation value


public:
    cMuscusMission()noexcept;
    ~cMuscusMission()final;

    DISABLE_COPY(cMuscusMission)
    ASSIGN_ID(7, "Muscus")

    // 
    void EnableGenerate (const coreUintW iIndex);
    void DisableGenerate(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePearl (const coreUintW iIndex);
    void DisablePearl(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void ShowGenerate(const coreUintW iIndex, const coreFloat fTime) {ASSERT(iIndex < MUSCUS_GENERATES) if(m_afGenerateTime[iIndex] >= 0.0f) m_afGenerateTime[iIndex] = fTime;}
    inline void BangGenerate(const coreUintW iIndex)                        {ASSERT(iIndex < MUSCUS_GENERATES) if(m_afGenerateTime[iIndex] >= 0.0f) m_afGenerateBang[iIndex] = 1.0f;}


private:
    // execute own routines
    void __SetupOwn      ()final;
    void __RenderOwnUnder()final;
    void __MoveOwnAfter  ()final;
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
    ASSIGN_ID(100, "Error")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// 
class cDemoMission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cDemoMission()noexcept;

    DISABLE_COPY(cDemoMission)
    ASSIGN_ID(101, "Demo")


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


// ****************************************************************
// 
constexpr FUNC_LOCAL coreBool cMission::_TakeRange(const coreUint8 iFrom, const coreUint8 iTo, const coreUint8* piIndexList, const coreUintW iSize)
{
    // 
    if((piIndexList[0] == TAKE_MISSION) && (iTo != TAKE_MISSION))
        return false;

    // 
    for(coreUintW i = 1u; i < iSize; ++i)
    {
        if((piIndexList[i] >= iFrom) &&
           (piIndexList[i] <= iTo))
            return true;
    }

    // 
    return (iSize == 1u);
}


#endif // _P1_GUARD_MISSION_H_