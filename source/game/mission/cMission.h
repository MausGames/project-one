///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MISSION_H_
#define _P1_GUARD_MISSION_H_

// TODO 3: reuse paths (beware of reserve assertion, and resize on refine) and squads over stages
// TODO 3: prevent multiple calculations in script-commands (because of macro variables), also boss
// TODO 3: assertion for "active boss should be alive"
// TODO 3: STAGE_FLYPAST with dot-product or simpler per-axis
// TODO 2: there seems to be a bug in STAGE_TICK_TIME, which sometimes gives early or late ticks with 30.0f speed, compared with STAGE_TICK_LIFETIME
// TODO 3: wrap m_piData in function with RETURN_RESTRICT
// TODO 3: set progress when finishing segment, not when starting, but consider mission-wrapping
// TODO 3: low-resolution object_sphere for small sphere objects (what about bullet_orb) ?
// TODO 3: change all missions to STATIC_MEMORY (check memory, it would put all missions always in memory), or create 2 max-size blocks (old, cur), also in Ater mission ?
// TODO 3: do not create objects and load resources of unused game-objects and bosses (e.g. move waves into own classes ? but then ?)
// TODO 4: move as much gameplay from gameplay-objects from mission to stages, except for mission-shared stuff, animation stuff, or special-cases requiring before-after update (teleportation)
// TODO 1: chain should shatter into pieces on disable, should drag the stone to player on swing-start, boulder should use ice-shader, multiple boulders, clearing/resetting swing and catch attributes etc.
// TODO 1: check if all allocated enemy numbers are correct
// TODO 2: generate objects are preventing each others outlines while being alpha 0 (especially on diagonal movement)
// TODO 3: nevo: render-reihenfolge der blasts is statisch, manchmal überlagern sie sich, nicht konsistent
// TODO 4: mission code sometimes accesses variables directly without wrapper-functions (mixed), bosses always need wrapper functions, should this be handled consistently ?
// TODO 4: warum sind s_iTick und co. static ?
// TODO 3: change m_piData into static buffer (needs manual clear), that way I can also remove init-number (! sometimes there are 2 missions in memory at the same time)
// TODO 3: morning star chain should be above player wind
// TODO 3: insanity functions in harena copy (and override) some of the mission code, maybe this can be cleaned up
// TODO 1: what happens to morningstar if any player is dead
// TODO 4: remove direct access with .List() or Raw containers in all missions (change to Getters)
// TODO 4: change fangs back from cLodObject to coreObject3D, if lod not required (other objects as well ? (low == high))
// TODO 3: delay sollte zeit anhalten
// TODO 3: hail should only use one batchlist, both can be merged
// TODO 3: cMission::Close() sollte auch nach dem finalen/secret boss aufgerufen werden (mission und/oder segment ?)
// TODO 3: manual graphics are not updated when changing resolution during pause in intro mission
// TODO 3: manual/tutorial might react strange on inverted and toggled firing mode, because iActionHold is inspected
// TODO 1: wenn P1 oder Eigengrau besiegt wird, beim ersten mal kommen immer credits, danach kommen keine bei single segment und single mission, aber immer bei arcade
// TODO 1: add 3 different pearl-pitch tracks (wave, boss, p1) and reset state properly
// TODO 3: flash-teleportation (mission, boss, p1) should be by the player doing the most damage, not the last attacker


// ****************************************************************
// mission definitions
#define MISSION_PLAYERS    (PLAYERS)   // 
#define MISSION_BOSSES     (BOSSES)    // default number of bosses per mission
#define MISSION_WAVES      (WAVES)     // 
#define MISSION_NO_BOSS    (0xFFu)     // no boss currently active (error-value)
#define MISSION_NO_WAVE    (0xFFu)     // 
#define MISSION_NO_SEGMENT (0xFFu)     // 
#define MISSION_WAIT_INTRO (1.5f)      // 
#define MISSION_WAIT_OUTRO (2.0f)      // 
#define MISSION_WAIT_PLAY  (6.9f)      // 

#define MISSION_SEGMENT_IS_BOSS(i) ((i) >= MISSION_WAVES)
#define MISSION_BOSS_TO_SEGMENT(i) ((i) +  MISSION_WAVES)
#define MISSION_WAVE_TO_SEGMENT(i) ((i))

#define TAKE_ALWAYS   (0x00u)
#define TAKE_MISSION  (0xFEu)
#define TAKE_TRAINING (0xFFu)


// ****************************************************************
// mission specific definitions
#define VIRIDO_TRAILS               (4u)                                              // 
#define VIRIDO_BALLS                (2u)                                              // 
#define VIRIDO_BALLS_RAWS           (VIRIDO_BALLS * (VIRIDO_TRAILS + 1u))             // 
#define VIRIDO_PADDLES              (3u)                                              // 
#define VIRIDO_BARRIERS             (26u)                                             // 
#define VIRIDO_BARRIERS_RAWS        (VIRIDO_BARRIERS)                                 // 
#define VIRIDO_BARRIERS_FIRST       (15u)                                             // 
#define VIRIDO_BARRIERS_SECOND      (VIRIDO_BARRIERS - VIRIDO_BARRIERS_FIRST)         // 
#define VIRIDO_LASERS               (8u)                                              // 
#define VIRIDO_LASERS_RAWS          (VIRIDO_LASERS * 2u)                              // 
#define VIRIDO_SHADOWS_ENEMY        (20u)                                             // 
#define VIRIDO_SHADOWS              (VIRIDO_SHADOWS_ENEMY + MISSION_PLAYERS)          // 
#define VIRIDO_SHADOWS_RAWS         (VIRIDO_SHADOWS)                                  // 
#define VIRIDO_HINTS                (12u)                                             // 
#define VIRIDO_HINTS_RAWS           (VIRIDO_HINTS)                                    // 
#define VIRIDO_BEANS                (16u)                                             // 
#define VIRIDO_BEANS_RAWS           (VIRIDO_BEANS * 2u)                               // 
#define VIRIDO_DRUMS                (2u)                                              // 
#define VIRIDO_BALL_SPEED           (1.5f)                                            // 
#define VIRIDO_BARRIER_FREE         (r_cast<cShip*>(1u))                              // 

#define NEVO_BOMBS                  (24u)                                             // 
#define NEVO_BOMBS_RAWS             (NEVO_BOMBS)                                      // 
#define NEVO_LINES                  (4u)                                              // 
#define NEVO_BLASTS                 (NEVO_BOMBS)                                      // 
#define NEVO_BLASTS_RAWS            (NEVO_BLASTS * (NEVO_LINES + 1u))                 // 
#define NEVO_TILES                  (20u)                                             // 
#define NEVO_TILES_RAWS             (NEVO_TILES)                                      // 
#define NEVO_ARROWS                 (38u)                                             // 
#define NEVO_ARROWS_RAWS            (NEVO_ARROWS)                                     // 
#define NEVO_BLOCKS                 (2u)//(70u)                                       // (disabled) 
#define NEVO_BLOCKS_RAWS            (NEVO_BLOCKS * 2u)                                // 
#define NEVO_SCRAPS                 (4u)                                              // 
#define NEVO_CHIPS                  (10u)                                             // 
#define NEVO_CHIPS_RAWS             (NEVO_CHIPS * 2u)                                 // 
#define NEVO_BOMB_SIZE              (4.0f)                                            // 
#define NEVO_TILE_SPEED             (2.0f)                                            // 

#define HARENA_FLOORS               (10u)                                             // 
#define HARENA_FLOORS_RAWS          (HARENA_FLOORS)                                   // 
#define HARENA_SPIKES               (36u)                                             // 
#define HARENA_SPIKES_RAWS          (HARENA_SPIKES * 2u)                              // 
#define HARENA_SPIKE_DIMENSION      (6u)                                              //    
#define HARENA_SPIKE_LAUNCH         (1.0f)                                            // 
#define HARENA_SPIKE_FADE           (0.5f)                                            // 
#define HARENA_EGGS                 (8u)                                              // 
#define HARENA_EGGS_RAWS            (HARENA_EGGS * 2u)                                // 
#define HARENA_FLUMMIS              (21u)                                             // 
#define HARENA_FLUMMIS_RAWS         (HARENA_FLUMMIS)                                  // 
#define HARENA_INSANITY_P1          (10u)                                             // 

#define RUTILUS_TELEPORTER          (2u)                                              // 
#define RUTILUS_TELEPORTER_COLOR(x) ((x) ? COLOR_ENERGY_BLUE : COLOR_ENERGY_ORANGE)   // 
#define RUTILUS_PLATES              (5u)                                              // 
#define RUTILUS_PLATES_RAWS         (RUTILUS_PLATES)                                  // 
#define RUTILUS_AREAS               (2u)                                              // 
#define RUTILUS_WAVES               (4u)                                              // 
#define RUTILUS_WAVES_RAWS          (RUTILUS_WAVES)                                   // 
#define RUTILUS_TOCKS               (5u)                                              // 
#define RUTILUS_TOCKS_RAWS          (RUTILUS_TOCKS * 2u)                              // 
#define RUTILUS_SLAPS               (4u)                                              // 
#define RUTILUS_SLAPS_RAWS          (RUTILUS_SLAPS * 2u)                              // 
#define RUTILUS_PLATE_SPEED         (1.0f)                                            // 
#define RUTILUS_AREA_SIZE           (20.0f)                                           // 
#define RUTILUS_SAFE_SIZE           (20.0f)                                           // 
#define RUTILUS_AREA_REGISTRY       (MISSION_PLAYERS + 2u)                            // 

#define GELU_FANGS                  (25u)                                             // 
#define GELU_FANGS_RAWS             (GELU_FANGS)                                      // 
#define GELU_FANGS_DIMENSION        (5u)                                              //    
#define GELU_WAYS                   (26u)                                             // 
#define GELU_WAYS_RAWS              (GELU_WAYS * 2u)                                  // 
#define GELU_ORBS                   (20u)                                             // 
#define GELU_ORBS_RAWS              (GELU_ORBS)                                       // 
#define GELU_LINES                  (24u)                                             // 
#define GELU_LINES_RAWS             (GELU_LINES)                                      // 
#define GELU_COINS                  (4u)                                              // 
#define GELU_COINS_RAWS             (GELU_COINS * 2u)                                 // 
#define GELU_GAPS                   (5u)                                              // 
#define GELU_GAPS_RAWS              (GELU_GAPS)                                       // 
#define GELU_SHINES                 (16u)                                             // 
#define GELU_SHINES_RAWS            (GELU_SHINES)                                     // 
#define GELU_DROPS                  (8u)                                              //                             
#define GELU_DROPS_RAWS             (GELU_DROPS)                                      //                             
#define GELU_FANG_COLOR             (coreVector3(1.0f,1.0f,1.0f) * 0.25f)             // 
#define GELU_FANG_STEP              (0.44f)                                           // 
#define GELU_WAY_STEP               (0.36f)                                           // 
#define GELU_POSITIONS              (MAX(GELU_FANGS, GELU_WAYS))                      // 

#define CALOR_LOADS                 (1u)//(12u)                                       // (disabled) 
#define CALOR_LOADS_RAWS            (CALOR_LOADS)                                     // 
#define CALOR_CHAINS                (28u)                                             // 
#define CALOR_STARS                 (MISSION_PLAYERS)                                 // 
#define CALOR_STARS_RAWS            (CALOR_STARS * (CALOR_CHAINS + 1u))               // 
#define CALOR_HAILS                 (3u)                                              // 
#define CALOR_HAILS_RAWS            (CALOR_HAILS * 2u)                                // 
#define CALOR_CHESTS                (6u)                                              // 
#define CALOR_CHESTS_RAWS           (CALOR_CHESTS * 2u)                               // 
#define CALOR_CHAIN_CONSTRAINT1     (20.0f)                                           // 
#define CALOR_CHAIN_CONSTRAINT2     (46.0f)                                           // 
#define CALOR_SWING_SPEED           (7.0f)                                            // 

#define MUSCUS_GENERATES            (40u)                                             // 
#define MUSCUS_GENERATES_RAWS       (MUSCUS_GENERATES * 2u)                           // 
#define MUSCUS_PEARLS               (44u)                                             // 
#define MUSCUS_PEARLS_RAWS          (MUSCUS_PEARLS * 2u)                              // 
#define MUSCUS_ZOMBIES              (3u)                                              // 

#define INTRO_MANUALS               (11u)                                             // 


// ****************************************************************
// stage management macros
#define STAGE_MAIN(...)                        if([this]() {static constexpr coreUint8 A[] = __VA_ARGS__; return cMission::_TakeRange(m_iTakeFrom, m_iTakeTo, A, ARRAY_SIZE(A));}()) m_anStage.emplace(__LINE__, [this]()
#define STAGE_SUB(i)                           ((m_iStageSub < (i)) && [&]() {m_iStageSub = (i); m_fStageSubTime = 0.0f; m_fStageSubTimeBefore = 0.0f; return true;}())

#define STAGE_FINISH_NOW                       {this->SkipStage();}
#define STAGE_FINISH_PLAY                      {if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY)) STAGE_FINISH_NOW}
#define STAGE_FINISH_AFTER(t)                  {if(m_fStageTime >= (t))                              STAGE_FINISH_NOW}

#define STAGE_MEDAL_GOAL(...)                  {static constexpr coreFloat A[] = __VA_ARGS__; this->SetMedalGoal(A); STATIC_ASSERT((ARRAY_SIZE(A) == 4u) && (A[0] < A[1]) && (A[1] < A[2]) && (A[2] < A[3]))}
#define STAGE_BOSS(e,...)                      {if(STAGE_BEGINNING) {STAGE_MEDAL_GOAL(__VA_ARGS__) (e).Resurrect();} if((e).HasStatus(ENEMY_STATUS_DEAD)) STAGE_FINISH_NOW}
#define STAGE_WAVE(i,n,...)                    {if(STAGE_BEGINNING) {STAGE_MEDAL_GOAL(__VA_ARGS__) this->ActivateWave(i, n);} if(STAGE_CLEARED) {this->DeactivateWave(); if(this->_UpdateWait()) STAGE_FINISH_NOW}}

#define STAGE_CLEARED                          (std::all_of(m_apSquad.begin(), m_apSquad.end(), [](const cEnemySquad* pSquad) {return pSquad->IsFinished();}))
#define STAGE_RESURRECT(s,f,t)                 {STAGE_FOREACH_ENEMY_ALL(s, pEnemy, __i) {if((coreIntW(__i) >= coreIntW(f)) && (coreIntW(__i) <= coreIntW(t))) pEnemy->Resurrect();}); ASSERT((coreIntW(f) <= coreIntW(t)) && (coreIntW(t) < coreIntW((s)->GetNumEnemies())))}
#define STAGE_BADGE(i,b,p)                     {this->GiveBadge(i, b, p);}

#define STAGE_DELAY_START                      {UNUSED STAGE_ADD_SQUAD(pDelay, cDummyEnemy, 1u) {pDelay->GetEnemy(0u)->Configure(1, 0u, COLOR_SHIP_GREY); pDelay->GetEnemy(0u)->Resurrect(); m_bDelay = true;});}
#define STAGE_DELAY_START_CLEAR                {STAGE_DELAY_START g_pGame->GetBulletManagerEnemy()->ClearBullets(true);}
#define STAGE_DELAY_END                        {m_apSquad.back()->GetEnemy(0u)->Kill(false); m_bDelay = false;}

#define STAGE_SINK_UINT(x)                     (bIsDead ? r_cast<coreUint32&> (s_aiSink) : (x))
#define STAGE_SINK_FLOAT(x)                    (bIsDead ? r_cast<coreFloat&>  (s_aiSink) : (x))
#define STAGE_SINK_VEC2(x)                     (bIsDead ? r_cast<coreVector2&>(s_aiSink) : (x))

#define STAGE_ADD_PATH(n)                      coreSpline2* const OUTPUT n = this->_AddPath    (__LINE__,      [&](coreSpline2* OUTPUT n)
#define STAGE_ADD_SQUAD(n,t,c)                 cEnemySquad* const OUTPUT n = this->_AddSquad<t>(__LINE__, (c), [&](cEnemySquad* OUTPUT n)

#define STAGE_COLL_PLAYER_ENEMY(a,b,i,f,...)   if(!m_nCollPlayerEnemy)  m_nCollPlayerEnemy  = ([__VA_ARGS__](cPlayer* OUTPUT a, cEnemy*  OUTPUT b, const coreVector3 i, const coreBool f)
#define STAGE_COLL_PLAYER_BULLET(a,b,i,f,...)  if(!m_nCollPlayerBullet) m_nCollPlayerBullet = ([__VA_ARGS__](cPlayer* OUTPUT a, cBullet* OUTPUT b, const coreVector3 i, const coreBool f)
#define STAGE_COLL_ENEMY_BULLET(a,b,i,f,...)   if(!m_nCollEnemyBullet)  m_nCollEnemyBullet  = ([__VA_ARGS__](cEnemy*  OUTPUT a, cBullet* OUTPUT b, const coreVector3 i, const coreBool f)
#define COLL_VAL(x)                             x = s_cast<typename std::conditional<!std::is_reference<decltype(x)>::value, decltype(x), void>::type>(x)
#define COLL_REF(x)                            &x = s_cast<typename std::conditional< std::is_reference<decltype(x)>::value, decltype(x), void>::type>(x)
#define COLL_THIS                              this

#define STAGE_FOREACH_PLAYER(e,i)              g_pGame->ForEachPlayer   ([&](cPlayer* OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_PLAYER_ALL(e,i)          g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_ENEMY(s,e,i)             (s)->ForEachEnemy        ([&](cEnemy*  OUTPUT e, const coreUintW i)
#define STAGE_FOREACH_ENEMY_ALL(s,e,i)         (s)->ForEachEnemyAll     ([&](cEnemy*  OUTPUT e, const coreUintW i)

#define STAGE_GET_START(c)                     {if((c) > m_iDataSize) {ALIGNED_DELETE(m_piData) STATIC_ASSERT((c) <= 0xFFu) m_iDataSize = (c); m_piData = ALIGNED_NEW(coreUint32, m_iDataSize, ALIGNMENT_CACHE); std::memset(m_piData, 0, sizeof(coreUint32) * m_iDataSize);}} coreUintW iDataIndex = 0u; constexpr coreUintW iCurDataSize = (c);
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
    UNUSED       coreFloat fLifeTime           = fLifeTimeBase;                                       \
    UNUSED       coreFloat fLifeTimeBefore     = fLifeTimeBeforeBase;                                 \
    UNUSED const coreBool  bIsDead             = (e)->HasStatus(ENEMY_STATUS_DEAD);

#define STAGE_BRANCH(x,y)                      ((fLifeTime < (x)) || [&]() {fLifeTime = FMOD(fLifeTime - (x), (y)); fLifeTimeBefore = FMOD(fLifeTimeBefore - (x), (y)); if(fLifeTimeBefore > fLifeTime) fLifeTimeBefore -= (y); return false;}())
#define STAGE_REPEAT(x)                        {if(STAGE_BRANCH(x, x)) {}}

#define STAGE_TICK_EXTERN(a,b,c,o)             ((s_iTick = F_TO_UI((a) * (c) - (o)) - 1u) != coreUint16(F_TO_UI((b) * (c) - (o)) - 1u))   // wrap into function, to only calc c and o once
#define STAGE_TICK_FREE(c,o)                   ((m_fStageTimeBefore    >= 0.0f) &&             STAGE_TICK_EXTERN(m_fStageTime,    m_fStageTimeBefore,    RoundFreq(c), o))
#define STAGE_TICK_FREE2(c,o)                  ((m_fStageSubTimeBefore >= 0.0f) &&             STAGE_TICK_EXTERN(m_fStageSubTime, m_fStageSubTimeBefore, RoundFreq(c), o))
#define STAGE_TICK_TIME(c,o)                   ((fLifeTimeBeforeBase   >= 0.0f) && !bIsDead && STAGE_TICK_FREE (c, o))
#define STAGE_TICK_TIME2(c,o)                  ((fLifeTimeBeforeBase   >= 0.0f) && !bIsDead && STAGE_TICK_FREE2(c, o))
#define STAGE_TICK_LIFETIME(c,o)               ((fLifeTimeBeforeBase   >= 0.0f) && !bIsDead && STAGE_TICK_EXTERN(fLifeTime,       fLifeTimeBefore,       c, o))   // RoundFreq((c) * fLifeSpeed) / fLifeSpeed
#define STAGE_TICK_LIFETIME_BASE(c,o)          ((fLifeTimeBeforeBase   >= 0.0f) && !bIsDead && STAGE_TICK_EXTERN(fLifeTimeBase,   fLifeTimeBeforeBase,   c, o))

#define STAGE_TIME_POINT(t)                    (InBetween((t), m_fStageTimeBefore, m_fStageTime))
#define STAGE_TIME_BEFORE(t)                   (m_fStageTime <  (t))
#define STAGE_TIME_AFTER(t)                    (m_fStageTime >= (t))
#define STAGE_TIME_BETWEEN(t,u)                (InBetween(m_fStageTime, (t), (u)))
#define STAGE_BEGINNING                        (STAGE_TIME_POINT(0.0f))

#define STAGE_SUBTIME_POINT(t)                 (InBetween((t), m_fStageSubTimeBefore, m_fStageSubTime))
#define STAGE_SUBTIME_BEFORE(t)                (m_fStageSubTime <  (t))
#define STAGE_SUBTIME_AFTER(t)                 (m_fStageSubTime >= (t))
#define STAGE_SUBTIME_BETWEEN(t,u)             (InBetween(m_fStageSubTime, (t), (u)))
#define STAGE_BEGINNING2                       ((STAGE_SUBTIME_POINT(0.0f) && (m_fStageSubTimeBefore != 0.0f)) || (m_fStageSubTime == 0.0f))

#define STAGE_LIFETIME_POINT(t)                (InBetween((t), fLifeTimeBefore, fLifeTime) && [&]() {s_fLifeTimePoint = (t); return true;}())
#define STAGE_LIFETIME_BEFORE(t)               (fLifeTime     <  (t) && fLifeTime     >= 0.0f)
#define STAGE_LIFETIME_BEFORE_BASE(t)          (fLifeTimeBase <  (t) && fLifeTimeBase >= 0.0f)
#define STAGE_LIFETIME_AFTER(t)                (fLifeTime     >= (t))
#define STAGE_LIFETIME_AFTER_BASE(t)           (fLifeTimeBase >= (t))
#define STAGE_LIFETIME_BETWEEN(t,u)            (InBetween(fLifeTime, (t), (u)))
#define STAGE_TAKEOFF                          ((InBetween(0.0f, fLifeTimeBeforeBase, fLifeTimeBase) && (fLifeTimeBeforeBase != 0.0f)) || (fLifeTimeBase == 0.0f))   // TODO 1: negative fLifeOffset can break this (will never be true)

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
    using uCollPlayerEnemyType  = std::function<void(cPlayer* OUTPUT, cEnemy*  OUTPUT, const coreVector3, const coreBool)>;
    using uCollPlayerBulletType = std::function<void(cPlayer* OUTPUT, cBullet* OUTPUT, const coreVector3, const coreBool)>;
    using uCollEnemyBulletType  = std::function<void(cEnemy*  OUTPUT, cBullet* OUTPUT, const coreVector3, const coreBool)>;


protected:
    cBoss* m_apBoss[MISSION_BOSSES];                        // pointers to all available bosses

    cBoss*    m_pCurBoss;                                   // pointer to currently active boss
    coreUintW m_iCurBossIndex;                              // index of the active boss (or error-value)
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

    coreUint8 m_iBadgeGiven;                                // 

    coreUint8 m_iRecordBroken;                              // 

    uCollPlayerEnemyType  m_nCollPlayerEnemy;               // 
    uCollPlayerBulletType m_nCollPlayerBullet;              // 
    uCollEnemyBulletType  m_nCollEnemyBullet;               // 

    coreUint8 m_iTakeFrom;                                  // 
    coreUint8 m_iTakeTo;                                    // 

    coreBool m_bDelay;                                      // 

    coreBool m_bRepeat;                                     // 

    static coreUint16  s_iTick;                             // 
    static coreFloat   s_fLifeTimePoint;                    // 
    static coreFloat   s_fHealthPctPoint;                   // 
    static coreVector2 s_vPositionPoint;                    // 
    static coreUint32  s_aiSink[2];                         // 


public:
    cMission()noexcept;
    virtual ~cMission();

    DISABLE_COPY(cMission)
    ENABLE_ID

    // setup the mission
    void Setup(const coreUint8 iTakeFrom, const coreUint8 iTakeTo);

    // 
    void Open();
    void Close();

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
    void ActivateWave  (const coreUintW iIndex, const coreChar* pcName);
    void DeactivateWave();

    // 
    inline void SetMedalGoal(const coreFloat* pfMedalGoal) {m_pfMedalGoal = pfMedalGoal; ASSERT(pfMedalGoal)}

    // 
    void GiveBadge(const coreUintW iIndex, const coreUint8 iBadge, const coreVector3 vPosition);

    // 
    void AddExtraScore(cPlayer* OUTPUT pPlayer, const coreUint32 iScore, const coreVector3 vPosition);

    // 
    inline void CollPlayerEnemy (cPlayer* OUTPUT pPlayer, cEnemy*  OUTPUT pEnemy,  const coreVector3 vIntersection, const coreBool bFirstHit) {if(m_nCollPlayerEnemy)  m_nCollPlayerEnemy (pPlayer, pEnemy,  vIntersection, bFirstHit);}
    inline void CollPlayerBullet(cPlayer* OUTPUT pPlayer, cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit) {if(m_nCollPlayerBullet) m_nCollPlayerBullet(pPlayer, pBullet, vIntersection, bFirstHit);}
    inline void CollEnemyBullet (cEnemy*  OUTPUT pEnemy,  cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit) {if(m_nCollEnemyBullet)  m_nCollEnemyBullet (pEnemy,  pBullet, vIntersection, bFirstHit);}

    // 
    template <typename F> inline void SetCollPlayerEnemy (F&& nCollFunc) {if(!m_nCollPlayerEnemy)  m_nCollPlayerEnemy  = nCollFunc;}
    template <typename F> inline void SetCollPlayerBullet(F&& nCollFunc) {if(!m_nCollPlayerBullet) m_nCollPlayerBullet = nCollFunc;}
    template <typename F> inline void SetCollEnemyBullet (F&& nCollFunc) {if(!m_nCollEnemyBullet)  m_nCollEnemyBullet  = nCollFunc;}

    // 
    inline void ResetCollPlayerEnemy () {m_nCollPlayerEnemy  = NULL;}
    inline void ResetCollPlayerBullet() {m_nCollPlayerBullet = NULL;}
    inline void ResetCollEnemyBullet () {m_nCollEnemyBullet  = NULL;}

    // access mission objects
    inline cBoss*           GetBoss           (const coreUintW iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*           GetCurBoss        ()const                       {return m_pCurBoss;}
    inline const coreUintW& GetCurBossIndex   ()const                       {return m_iCurBossIndex;}
    inline const coreUintW& GetCurWaveIndex   ()const                       {return m_iCurWaveIndex;}
    inline const coreUintW& GetCurSegmentIndex()const                       {return m_iCurSegmentIndex;}
    inline cEnemySquad*     GetEnemySquad     (const coreUintW iIndex)const {ASSERT(iIndex < m_apSquad.size()) return m_apSquad.get_valuelist()[iIndex];}
    inline const coreUint8& GetStageSub       ()const                       {return m_iStageSub;}
    inline const coreFloat* GetMedalGoal      ()const                       {return m_pfMedalGoal;}
    inline const coreUint8& GetRecordBroken   ()const                       {return m_iRecordBroken;}
    inline const coreUint8& GetTakeFrom       ()const                       {return m_iTakeFrom;}
    inline const coreUint8& GetTakeTo         ()const                       {return m_iTakeTo;}
    inline const coreBool&  GetDelay          ()const                       {return m_bDelay;}

    // get object properties
    virtual const coreChar* GetMusicName()const {return "";}


protected:
    // 
    template             <typename F> RETURN_RESTRICT coreSpline2* _AddPath (const coreUint16 iCodeLine,                       F&& nInitFunc);   // [](coreSpline2* OUTPUT pPath)  -> void
    template <typename T, typename F> RETURN_RESTRICT cEnemySquad* _AddSquad(const coreUint16 iCodeLine, const coreUint8 iNum, F&& nInitFunc);   // [](cEnemySquad* OUTPUT pSquad) -> void

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
    ASSIGN_ID(0, "NOTHING")
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

    coreBatchList m_Barrier1;                               // 
    coreBatchList m_Barrier2;                               // 
    coreObject3D  m_aBarrierRaw   [VIRIDO_BARRIERS_RAWS];   // 
    const cShip*  m_apBarrierOwner[VIRIDO_BARRIERS];        // 
    coreVector2   m_avBarrierPos  [VIRIDO_BARRIERS];        // 
    coreVector2   m_avBarrierDir  [VIRIDO_BARRIERS];        // 
    coreBool      m_bBarrierSlow;                           // 
    coreBool      m_bBarrierClamp;                          // 
    coreBool      m_bBarrierReflect;                        // 

    coreBatchList m_Laser;                                  // 
    coreBatchList m_LaserWave;                              // 
    coreObject3D  m_aLaserRaw   [VIRIDO_LASERS_RAWS];       // 
    const cShip*  m_apLaserOwner[VIRIDO_LASERS];            // 
    coreVector2   m_avLaserPos  [VIRIDO_LASERS];            // 
    coreVector2   m_avLaserDir  [VIRIDO_LASERS];            // 
    coreFlow      m_afLaserTick [MISSION_PLAYERS];          // 
    coreBool      m_bLaserCross;                            // 

    coreBatchList m_Shadow;                                 // 
    coreObject3D  m_aShadowRaw   [VIRIDO_SHADOWS_RAWS];     // 
    const cShip*  m_apShadowOwner[VIRIDO_SHADOWS];          // 
    coreUint32    m_iShadowType;                            // 

    coreBatchList m_Hint;                                   // 
    coreObject3D  m_aHintRaw     [VIRIDO_HINTS_RAWS];       // 
    coreUint8     m_aiHintBarrier[VIRIDO_HINTS_RAWS];       // 

    coreBatchList m_Bean;                                   // 
    coreBatchList m_BeanWave;                               // 
    coreObject3D  m_aBeanRaw[VIRIDO_BEANS_RAWS];            // 

    cCustomEnemy m_Globe;                                   // 

    coreObject3D m_Target;                                  // 
    coreObject3D m_aTargetWave[2];                          // 

    coreUint16 m_aiDrumCount[VIRIDO_DRUMS];                 // 
    coreUint8  m_aiDrumIndex[VIRIDO_DRUMS];                 // 

    coreFlow  m_fPoleCount;                                 // 
    coreUint8 m_iPoleIndex;                                 // 

    coreUint8 m_iRealState;                                 // 
    coreUint8 m_iStickyState;                               // (only between first ball and first paddle) 
    coreUint8 m_iBounceState;                               // 

    coreFlow m_fAnimation;                                  // animation value


public:
    cViridoMission()noexcept;
    ~cViridoMission()final;

    DISABLE_COPY(cViridoMission)
    ASSIGN_ID(1, "VIRIDO")

    // 
    void EnableBall (const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection);
    void DisableBall(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePaddle (const coreUintW iIndex, const cShip* pOwner);
    void DisablePaddle(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableBarrier (const coreUintW iIndex, const cShip* pOwner, const coreVector2 vDirection, const coreFloat fScale);
    void DisableBarrier(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableLaser (const coreUintW iIndex, const cShip* pOwner);
    void DisableLaser(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableShadow (const coreUintW iIndex, const cShip* pOwner, const coreVector2 vPosition, const coreBool bQuad);
    void DisableShadow(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableHint (const coreUintW iIndex, const coreUintW iBarrier);
    void DisableHint(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableBean (const coreUintW iIndex);
    void DisableBean(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableTarget ();
    void DisableTarget(const coreBool bAnimated);

    // 
    void StartDrumBeat(const coreUintW iDrum, const coreUintW iBarrier);
    void EndDrumBeat  (const coreUintW iDrum, const coreBool bAnimated);

    // 
    void StartPoleDance(const coreUintW iIndex);
    void EndPoleDance  (const coreBool bAnimated);

    // 
    void        SetBarrierScale  (const coreUintW iIndex, const coreFloat fScale);
    inline void SetBarrierSlow   (const coreBool bSlow)    {m_bBarrierSlow    = bSlow;}
    inline void SetBarrierClamp  (const coreBool bClamp)   {m_bBarrierClamp   = bClamp;}
    inline void SetBarrierReflect(const coreBool bReflect) {m_bBarrierReflect = bReflect;}

    // 
    inline void SetLaserCross(const coreBool bCross) {m_bLaserCross = bCross;}

    // 
    inline void MakeReal    (const coreUintW iIndex)       {ADD_BIT(m_iRealState, iIndex)}
    inline void MakeSticky  ()                             {ADD_BIT(m_iStickyState, 0u)}
    inline void UnmakeSticky(const coreVector2 vDirection) {m_iStickyState = 0; m_aBallRaw[0].SetDirection(coreVector3(vDirection, 0.0f));}

    // 
    inline const coreUint8& GetRealState  ()const {return m_iRealState;}
    inline coreBool         GetStickyState()const {return HAS_BIT(m_iStickyState, 1u);}
    inline const coreUint8& GetBounceState()const {return m_iBounceState;}

    // 
    inline coreObject3D* GetBall   (const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_BALLS)    return &m_aBallRaw   [iIndex * (VIRIDO_TRAILS + 1u)];}
    inline coreObject3D* GetPaddle (const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_PADDLES)  return &m_aPaddle    [iIndex];}
    inline coreObject3D* GetBarrier(const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_BARRIERS) return &m_aBarrierRaw[iIndex];}
    inline coreObject3D* GetLaser  (const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_LASERS)   return &m_aLaserRaw  [iIndex * 2u];}
    inline coreObject3D* GetBean   (const coreUintW iIndex) {ASSERT(iIndex < VIRIDO_BEANS)    return &m_aBeanRaw   [iIndex * 2u];}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_01_intro.ogg";}


private:
    // execute own routines
    void __SetupOwn      ()final;
    void __RenderOwnUnder()final;
    void __RenderOwnOver ()final;
    void __RenderOwnTop  ()final;
    void __MoveOwnBefore ()final;
    void __MoveOwnAfter  ()final;

    // 
    static void __BounceEffect(const coreVector2 vEffectPos);
};


// ****************************************************************
// Nevo mission class
class cNevoMission final : public cMission
{
private:
    cLeviathanBoss m_Leviathan;                       // 

    coreBatchList m_Bomb;                             // 
    cLodObject    m_aBombRaw  [NEVO_BOMBS_RAWS];      // 
    coreFlow      m_afBombTime[NEVO_BOMBS];           // 
    coreUint32    m_iBombGone;                        // 

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
    coreFloat     m_afArrowAlpha[NEVO_ARROWS];        // 
    coreUint8     m_aiArrowDir  [NEVO_ARROWS];        // 
    coreUint8     m_iArrowActive;                     // 
    coreUint64    m_iArrowFake;                       // 
    coreBool      m_bArrowEnabled;                    // 

    coreBatchList m_Block;                            // 
    coreBatchList m_BlockWave;                        // 
    coreObject3D  m_aBlockRaw   [NEVO_BLOCKS_RAWS];   // 
    const cShip*  m_apBlockOwner[NEVO_BLOCKS];        // 
    coreFloat     m_afBlockScale[NEVO_BLOCKS];        // 
    coreFloat     m_afBlockRota [NEVO_BLOCKS];        // 

    coreObject3D m_aScrap     [NEVO_SCRAPS];          // 
    coreFlow     m_afScrapTime[NEVO_SCRAPS];          // 

    coreBatchList m_Chip;                             // 
    coreBatchList m_ChipWave;                         // 
    coreObject3D  m_aChipRaw  [NEVO_CHIPS_RAWS];      // 
    coreFlow      m_afChipTime[NEVO_CHIPS];           // 

    coreObject3D m_Guide;                             // 

    coreObject3D m_Trend;                             // 

    cLodObject  m_Container;                          // 
    coreVector2 m_vForce;                             // 
    coreVector2 m_vImpact;                            // 
    coreBool    m_bClamp;                             // 
    coreBool    m_bOverdraw;                          // 

    coreObject3D m_aDemoRange[2];                     // 
    coreFlow     m_fDemoRangeAnim;                    // 

    coreDummyPtr m_apResCache[23];                    // 
    coreSoundPtr m_pNightmareSound;                   // 

    coreFlow m_fAnimation;                            // animation value


public:
    cNevoMission()noexcept;
    ~cNevoMission()final;

    DISABLE_COPY(cNevoMission)
    ASSIGN_ID(2, "NEVO")

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
    void EnableArrow (const coreUintW iIndex, const cShip* pOwner, const coreVector2 vDirection);
    void DisableArrow(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableBlock (const coreUintW iIndex, const cShip* pOwner, const coreFloat fScale);
    void DisableBlock(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableScrap (const coreUintW iIndex, const coreVector2 vPosition);
    void DisableScrap(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableChip (const coreUintW iIndex);
    void DisableChip(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableGuide ();
    void DisableGuide(const coreBool bAnimated);

    // 
    void EnableTrend ();
    void DisableTrend(const coreBool bAnimated);

    // 
    void EnableContainer (const coreVector2 vPosition);
    void DisableContainer(const coreBool bAnimated);

    // 
    void EnableRanges ();
    void DisableRanges(const coreBool bAnimated);

    // 
    void SetTileStyle(const coreUintW iIndex, const coreUint8 iStyle);

    // 
    inline coreBool IsTileEnabled(const coreUintW iIndex)const {ASSERT(iIndex < NEVO_TILES) return (m_afTileTime[iIndex] > 0.0f);}

    // 
    inline void SetArrowDir    (const coreUintW iIndex, const coreVector2 vDirection) {ASSERT(iIndex < NEVO_ARROWS) m_aiArrowDir[iIndex] = PackDirection(vDirection);}
    inline void SetArrowFake   (const coreUintW iIndex)                               {ASSERT(iIndex < NEVO_ARROWS) ASSERT(m_aArrowRaw[iIndex].IsEnabled(CORE_OBJECT_ENABLE_MOVE)) ADD_BIT(m_iArrowFake, iIndex)}
    inline void SetArrowEnabled(const coreBool bEnabled)                              {m_bArrowEnabled = bEnabled;}

    // 
    inline void SetContainerForce   (const coreVector2 vForce)    {m_vForce    = vForce;}
    inline void SetContainerClamp   (const coreBool    bClamp)    {m_bClamp    = bClamp;}
    inline void SetContainerOverdraw(const coreBool    bOverdraw) {m_bOverdraw = bOverdraw;}

    // 
    inline       coreFloat    GetBombTime       (const coreUintW iIndex)const {ASSERT(iIndex < NEVO_BOMBS)  return m_afBombTime[iIndex];}
    inline       coreBool     GetBombGone       (const coreUintW iIndex)const {ASSERT(iIndex < NEVO_BOMBS)  return HAS_BIT(m_iBombGone,  iIndex);}
    inline const coreUint8&   GetArrowDir       (const coreUintW iIndex)const {ASSERT(iIndex < NEVO_ARROWS) return m_aiArrowDir[iIndex];}
    inline       coreBool     GetArrowFake      (const coreUintW iIndex)const {ASSERT(iIndex < NEVO_ARROWS) return HAS_BIT(m_iArrowFake, iIndex);}
    inline const coreBool&    GetArrowEnabled   ()const                       {return m_bArrowEnabled;}
    inline const coreVector2& GetContainerForce ()const                       {return m_vForce;}
    inline const coreVector2& GetContainerImpact()const                       {return m_vImpact;}

    // 
    inline cLodObject*   GetBomb     (const coreUintW iIndex) {ASSERT(iIndex < NEVO_BOMBS) return &m_aBombRaw[iIndex];}
    inline coreObject3D* GetTile     (const coreUintW iIndex) {ASSERT(iIndex < NEVO_TILES) return &m_aTileRaw[iIndex];}
    inline coreObject3D* GetChip     (const coreUintW iIndex) {ASSERT(iIndex < NEVO_CHIPS) return &m_aChipRaw[iIndex * 2u];}
    inline cLodObject*   GetContainer()                       {return &m_Container;}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_02_intro.ogg";}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __RenderOwnTop   ()final;
    void __MoveOwnAfter   ()final;
};


// ****************************************************************
// Harena mission class
class cHarenaMission final : public cMission
{
private:
    // 
    struct sChildData final
    {
        coreVector2 vPosition;   // 
        coreVector2 vMove;       // 
        coreUint8   iType;       // 
    };


private:
    cTigerBoss m_Tiger;                                 // 

    coreList<sChildData> m_avChildData;                 // 

    coreBatchList m_Floor;                              // 
    coreObject3D  m_aFloorRaw   [HARENA_FLOORS_RAWS];   // 
    const cShip*  m_apFloorOwner[HARENA_FLOORS];        // 

    coreBatchList m_Spike;                              // 
    coreBatchList m_SpikeBoard;                         // 
    coreObject3D  m_aSpikeRaw  [HARENA_SPIKES_RAWS];    // 
    coreFlow      m_afSpikeTime[HARENA_SPIKES];         // 
    coreFlow      m_afSpikeCur [HARENA_SPIKES];         // 
    coreFloat     m_afSpikeMax [HARENA_SPIKES];         // 
    coreUint64    m_iSpikeGood;                         // 
    coreUint64    m_iSpikeBad;                          // 

    coreBatchList m_Egg;                                // 
    coreBatchList m_EggWave;                            // 
    coreObject3D  m_aEggRaw[HARENA_EGGS_RAWS];          // 

    coreBatchList m_Flummi;                             // 
    coreObject3D  m_aFlummiRaw[HARENA_FLUMMIS_RAWS];    // 

    coreObject3D m_Aim;                                 // 

    std::function<void()> m_aInsanityStage[5];          // 
    coreUint8             m_iInsanity;                  // 

    coreFlow m_fAnimation;                              // animation value


public:
    cHarenaMission()noexcept;
    ~cHarenaMission()final;

    DISABLE_COPY(cHarenaMission)
    ASSIGN_ID(3, "HARENA")

    // 
    void EnableFloor (const coreUintW iIndex, const cShip* pOwner, const coreFloat fScale);
    void DisableFloor(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableSpike (const coreUintW iIndex, const coreBool bDelayed);
    void DisableSpike(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableEgg (const coreUintW iIndex);
    void DisableEgg(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableFlummi (const coreUintW iIndex);
    void DisableFlummi(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableAim ();
    void DisableAim(const coreBool bAnimated);

    // 
    void CreateExternChild(const coreVector2 vPosition, const coreVector2 vMove, const coreUint8 iType);

    // 
    inline void LaunchSpike (const coreUintW iIndex, const coreFloat fTime, const coreBool bGood = false) {ASSERT(iIndex < HARENA_SPIKES) ASSERT(fTime > HARENA_SPIKE_LAUNCH + HARENA_SPIKE_FADE) m_afSpikeCur[iIndex] = 0.0f; m_afSpikeMax[iIndex] = fTime; SET_BIT(m_iSpikeGood, iIndex, bGood)}
    inline void RetractSpike(const coreUintW iIndex)                                                      {ASSERT(iIndex < HARENA_SPIKES) if(m_afSpikeMax[iIndex]) m_afSpikeCur[iIndex] = MAX(m_afSpikeCur[iIndex], m_afSpikeMax[iIndex] - HARENA_SPIKE_FADE); ADD_BIT(m_iSpikeBad, iIndex)}

    // 
    void PlayInsanity();
    void ChangeInsanity(const coreUint8 iInsanity);
    void ChangeInsanityP1();

    // 
    void CrashEnemy(cEnemy* OUTPUT pEnemy)const;

    // 
    inline coreBool GetSpikeLaunched(const coreUintW iIndex)const {ASSERT(iIndex < HARENA_SPIKES) return (m_afSpikeMax[iIndex] && InBetween(m_afSpikeCur[iIndex], HARENA_SPIKE_LAUNCH + 0.1f, m_afSpikeMax[iIndex] - HARENA_SPIKE_FADE));}
    inline coreBool GetSpikeQuiet   (const coreUintW iIndex)const {ASSERT(iIndex < HARENA_SPIKES) return (m_afSpikeMax[iIndex] == 0.0f);}
    inline coreBool GetSpikeGood    (const coreUintW iIndex)const {ASSERT(iIndex < HARENA_SPIKES) return (m_afSpikeMax[iIndex] && InBetween(m_afSpikeCur[iIndex], 0.0f, m_afSpikeMax[iIndex] - HARENA_SPIKE_FADE * 0.5f) && HAS_BIT(m_iSpikeGood, iIndex) && !HAS_BIT(m_iSpikeBad, iIndex));}

    // 
    inline coreObject3D* GetSpikeBoard(const coreUintW iIndex) {ASSERT(iIndex < HARENA_SPIKES)  return &m_aSpikeRaw [iIndex * 2u + 1u];}
    inline coreObject3D* GetEgg       (const coreUintW iIndex) {ASSERT(iIndex < HARENA_EGGS)    return &m_aEggRaw   [iIndex * 2u];}
    inline coreObject3D* GetFlummi    (const coreUintW iIndex) {ASSERT(iIndex < HARENA_FLUMMIS) return &m_aFlummiRaw[iIndex];}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_03_intro.ogg";}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnTop   ()final;
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
    coreVector2  m_avTeleporterRota[RUTILUS_TELEPORTER];   // 
    coreUint8    m_iTeleporterActive;                      // 

    coreBatchList m_Plate;                                 // 
    coreObject3D  m_aPlateRaw   [RUTILUS_PLATES_RAWS];     // 
    coreFlow      m_afPlateTime [RUTILUS_PLATES];          // 
    coreFlow      m_afPlatePulse[RUTILUS_PLATES];          // 
    coreFloat     m_afPlateSide [RUTILUS_PLATES];          // 
    coreVector4   m_avPlateData [RUTILUS_PLATES];          // 
    coreUint8     m_iPlateActive;                          // 
    coreUint8     m_iPlateRotated;                         // 

    coreObject3D m_aArea[RUTILUS_AREAS];                   // 
    coreFlow     m_fAreaTime;                              // 
    coreFloat    m_fAreaScale;                             // 
    coreFloat    m_fAreaSpeed;                             // 
    coreBool     m_bAreaUpdate;                            // 

    coreObject3D m_Safe;                                   // 
    coreFlow     m_fSafeTime;                              // 

    coreBatchList m_Wave;                                  // 
    coreObject3D  m_aWaveRaw  [RUTILUS_WAVES_RAWS];        // 
    coreFlow      m_afWaveTime[RUTILUS_WAVES];             // 
    coreVector2   m_vWavePos;                              // 
    coreVector2   m_vWaveDir;                              // 
    coreUint8     m_iWaveData;                             // 
    coreUint8     m_iWaveActive;                           // 
    coreUint8     m_iWaveType;                             // 
    coreFloat     m_fWavePower;                            // 
    coreModelPtr  m_apWaveModel[2];                        // 

    coreBatchList m_Tock;                                  // 
    coreBatchList m_TockWave;                              // 
    coreObject3D  m_aTockRaw[RUTILUS_TOCKS_RAWS];          // 

    coreBatchList m_Slap;                                  // 
    coreBatchList m_SlapWave;                              // 
    coreObject3D  m_aSlapRaw   [RUTILUS_TOCKS_RAWS];       // 
    coreFlow      m_afSlapValue[RUTILUS_TOCKS];            // 

    coreObject3D m_Capsule;                                // 
    coreObject3D m_aCapsuleWave[4];                        // 

    coreUint8 m_aiMoveFlip[MISSION_PLAYERS];               // 

    coreUint8 m_aiWarnDir[MISSION_PLAYERS];                // 

    coreUint32 m_aiRegisterID   [RUTILUS_AREA_REGISTRY];   // 
    coreFloat  m_afRegisterSpeed[RUTILUS_AREA_REGISTRY];   // 

    coreFlow m_fAnimation;                                 // animation value


public:
    cRutilusMission()noexcept;
    ~cRutilusMission()final;

    DISABLE_COPY(cRutilusMission)
    ASSIGN_ID(4, "RUTILUS")

    // 
    void EnableTeleporter (const coreUintW iIndex);
    void DisableTeleporter(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePlate (const coreUintW iIndex, const coreFloat fOffsetFrom, const coreFloat fOffsetTo, const coreFloat fScaleFrom, const coreFloat fScaleTo);
    void DisablePlate(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableArea ();
    void DisableArea(const coreBool bAnimated);

    // 
    void EnableSafe ();
    void DisableSafe(const coreBool bAnimated);

    // 
    void EnableWave (const coreUint8 iType);
    void DisableWave(const coreBool bAnimated);

    // 
    void EnableTock (const coreUintW iIndex);
    void DisableTock(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableSlap (const coreUintW iIndex);
    void DisableSlap(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableCapsule ();
    void DisableCapsule(const coreBool bAnimated);

    // 
    inline coreBool IsPlateEnabled(const coreUintW iIndex)const {ASSERT(iIndex < RUTILUS_PLATES) return m_aPlateRaw[iIndex].IsEnabled(CORE_OBJECT_ENABLE_MOVE);}

    // 
    coreFloat CalcAreaSpeed(const coreVector2 vPosition, const coreFloat fFactorFast)const;

    // 
    inline void SetTeleporterActive(const coreUint8 iActive) {m_iTeleporterActive = iActive;}

    // 
    inline void SetPlateDirection(const coreUintW iIndex, const coreVector2 vDirection) {ASSERT(iIndex < RUTILUS_PLATES) m_aPlateRaw  [iIndex].SetDirection(coreVector3(vDirection, 0.0f));}
    inline void SetPlateSide     (const coreUintW iIndex, const coreFloat   fSide)      {ASSERT(iIndex < RUTILUS_PLATES) m_afPlateSide[iIndex] = fSide;}
    inline void SetPlateOffsetNow(const coreUintW iIndex, const coreFloat   fOffset)    {ASSERT(iIndex < RUTILUS_PLATES) m_afPlateTime[iIndex] = 1.0f; m_avPlateData[iIndex].xy(coreVector2(fOffset, fOffset));}
    inline void SetPlateOffset   (const coreUintW iIndex, const coreFloat   fOffset)    {ASSERT(iIndex < RUTILUS_PLATES) m_afPlateTime[iIndex] = 0.0f; m_avPlateData[iIndex].xy(coreVector2(m_avPlateData[iIndex].y, fOffset));}
    inline void SetPlateScale    (const coreUintW iIndex, const coreFloat   fScale)     {ASSERT(iIndex < RUTILUS_PLATES) m_afPlateTime[iIndex] = 0.0f; m_avPlateData[iIndex].zw(coreVector2(m_avPlateData[iIndex].w, fScale));}
    inline void SetPlateRotated  (const coreUintW iIndex, const coreBool    bRotated)   {ASSERT(iIndex < RUTILUS_PLATES) SET_BIT(m_iPlateRotated, iIndex, bRotated)}

    // 
    inline void SetAreaPosition(const coreVector2 vPosition) {m_aArea[0].SetPosition(coreVector3(vPosition, 0.0f));}
    inline void SetAreaScale   (const coreFloat   fScale)    {m_fAreaScale  = fScale;}
    inline void SetAreaUpdate  (const coreBool    bUpdate)   {m_bAreaUpdate = bUpdate;}

    // 
    inline void SetWavePosition (const coreVector2 vPosition)  {m_vWavePos = vPosition;}
    inline void SetWaveDirection(const coreVector2 vDirection) {m_vWaveDir = vDirection; ASSERT(vDirection.IsNormalized())}
    inline void SetWavePull     (const coreBool    bPull)      {SET_BIT(m_iWaveData, 0u, bPull)}
    inline void SetWaveDelayed  (const coreBool    bDelayed)   {SET_BIT(m_iWaveData, 1u, bDelayed)}
    inline void SetWavePower    (const coreFloat   fPower)     {m_fWavePower = fPower;}

    // 
    inline const coreFloat& GetAreaSpeed  ()const {return m_fAreaSpeed;}
    inline coreBool         GetWavePull   ()const {return HAS_BIT(m_iWaveData, 0u);}
    inline coreBool         GetWaveDelayed()const {return HAS_BIT(m_iWaveData, 1u);}

    // 
    inline coreObject3D* GetTock(const coreUintW iIndex) {ASSERT(iIndex < RUTILUS_TOCKS) return &m_aTockRaw[iIndex * 2u];}
    inline coreObject3D* GetSlap(const coreUintW iIndex) {ASSERT(iIndex < RUTILUS_SLAPS) return &m_aSlapRaw[iIndex * 2u];}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_04_intro.ogg";}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __MoveOwnBefore  ()final;
    void __MoveOwnAfter   ()final;

    // 
    void __UpdateAreaSpeed();

    // 
    void __TeleporterEffect(const coreUintW iIndex)const;
};


// ****************************************************************
// Gelu mission class
class cGeluMission final : public cMission
{
private:
    cCholBoss m_Chol;                                // 

    coreBatchList m_Fang;                            // 
    cLodObject    m_aFangRaw[GELU_FANGS_RAWS];       // 
    coreUint32    m_iFangActive;                     // 

    coreBatchList m_Way;                             // 
    coreBatchList m_WayArrow;                        // 
    coreObject3D  m_aWayRaw[GELU_WAYS_RAWS];         // 
    coreUint32    m_iWayActive;                      // 
    coreUint32    m_iWayVisible;                     // 
    coreUint32    m_iWayGhost;                       // 
    coreUint32    m_iWayFree;                        // 

    coreBatchList m_Orb;                             // 
    coreObject3D  m_aOrbRaw  [GELU_ORBS_RAWS];       // 
    coreFlow      m_afOrbTime[GELU_ORBS];            // 

    coreBatchList m_Line;                            // 
    coreObject3D  m_aLineRaw  [GELU_LINES_RAWS];     // 
    coreFlow      m_afLineTime[GELU_LINES];          // 
    coreUint8     m_iLineMode;                       // 

    coreBatchList m_Coin;                            // 
    coreBatchList m_CoinWave;                        // 
    coreObject3D  m_aCoinRaw[GELU_COINS_RAWS];       // 

    coreBatchList m_Gap;                             // 
    coreObject3D  m_aGapRaw  [GELU_GAPS_RAWS];       // 
    coreFlow      m_afGapTime[GELU_GAPS];            // 
    coreUint8     m_iGapActive;                      // 

    coreBatchList m_Shine;                           // 
    coreObject3D  m_aShineRaw  [GELU_SHINES_RAWS];   // 
    coreFlow      m_afShineTime[GELU_SHINES];        // 
    coreUint16    m_iShineActive;                    // 

    coreVector2 m_avOldPos[GELU_POSITIONS];          // 
    coreUint32  m_iTouchState;                       // 

    coreBool  m_abCrushImmune[MISSION_PLAYERS];      // 
    coreUint8 m_iCrushState;                         // 

    coreFlow m_fAnimation;                           // animation value


public:
    cGeluMission()noexcept;
    ~cGeluMission()final;

    DISABLE_COPY(cGeluMission)
    ASSIGN_ID(5, "GELU")

    // 
    void EnableFang (const coreUintW iIndex);
    void DisableFang(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableWay (const coreUintW iIndex, const coreVector2 vPosition, const coreVector2 vDirection);
    void DisableWay(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableOrb (const coreUintW iIndex);
    void DisableOrb(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableLine (const coreUintW iIndex);
    void DisableLine(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableCoin (const coreUintW iIndex);
    void DisableCoin(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableGap (const coreUintW iIndex);
    void DisableGap(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableShine (const coreUintW iIndex);
    void DisableShine(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline coreBool IsWayActive   (const coreUintW iIndex)const {ASSERT(iIndex < GELU_WAYS) return HAS_BIT(m_iWayActive,  iIndex);}
    inline coreBool IsWayActiveAny()const                       {return (m_iWayActive != 0u);}
    inline coreBool IsWayVisible  (const coreUintW iIndex)const {ASSERT(iIndex < GELU_WAYS) return HAS_BIT(m_iWayVisible, iIndex);}

    // 
    inline coreBool IsOrbEnabled (const coreUintW iIndex)const {ASSERT(iIndex < GELU_ORBS)  return (m_afOrbTime [iIndex] > 0.0f);}
    inline coreBool IsLineEnabled(const coreUintW iIndex)const {ASSERT(iIndex < GELU_LINES) return (m_afLineTime[iIndex] > 0.0f);}

    // 
    inline void SetLineMode(const coreUint8 iMode) {m_iLineMode = iMode;}

    // 
    inline void SetWayFree(const coreUintW iIndex, const coreBool bFree) {ASSERT(iIndex < GELU_WAYS) SET_BIT(m_iWayFree, iIndex, bFree)}

    // 
    inline void SetCrushFree(const coreBool bCrushFree) {SET_BIT(m_iCrushState, 0u, bCrushFree)}   // move through blocks after crush
    inline void SetCrushLong(const coreBool bCrushLong) {SET_BIT(m_iCrushState, 1u, bCrushLong)}   // move in-and-out of blocks after crush

    // 
    inline coreBool IsGapActive(const coreUintW iIndex)const {ASSERT(iIndex < GELU_GAPS) return HAS_BIT(m_iGapActive, iIndex);}

    // 
    inline coreBool IsShineActive(const coreUintW iIndex)const {ASSERT(iIndex < GELU_SHINES) return HAS_BIT(m_iShineActive, iIndex);}

    // 
    inline coreObject3D* GetFang (const coreUintW iIndex) {ASSERT(iIndex < GELU_FANGS) return &m_aFangRaw[iIndex];}
    inline coreObject3D* GetWay  (const coreUintW iIndex) {ASSERT(iIndex < GELU_WAYS)  return &m_aWayRaw [iIndex * 2u];}
    inline coreObject3D* GetArrow(const coreUintW iIndex) {ASSERT(iIndex < GELU_WAYS)  return &m_aWayRaw [iIndex * 2u + 1u];}
    inline coreObject3D* GetOrb  (const coreUintW iIndex) {ASSERT(iIndex < GELU_ORBS)  return &m_aOrbRaw [iIndex];}
    inline coreObject3D* GetLine (const coreUintW iIndex) {ASSERT(iIndex < GELU_LINES) return &m_aLineRaw[iIndex];}
    inline coreObject3D* GetCoin (const coreUintW iIndex) {ASSERT(iIndex < GELU_COINS) return &m_aCoinRaw[iIndex * 2u];}
    inline coreObject3D* GetGap  (const coreUintW iIndex) {ASSERT(iIndex < GELU_GAPS)  return &m_aGapRaw [iIndex];}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_05.ogg";}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __MoveOwnAfter   ()final;

    // 
    void __UpdateCollisionFang();
    void __UpdateCollisionWay();
};


// ****************************************************************
// Calor mission class
class cCalorMission final : public cMission
{
private:
    cZerothBoss m_Zeroth;                             // 

    cSnow    m_Snow;                                  // 
    coreFlow m_fSnowTick;                             // 
    coreFlow m_afSnowStuck[MISSION_PLAYERS];          // 

    coreBatchList m_Load;                             // 
    coreObject3D  m_aLoadRaw[CALOR_LOADS_RAWS];       // 
    coreObject3D  m_LoadCopy;                         // 
    const cShip*  m_pLoadOwner;                       // 
    coreFlow      m_afLoadPower[3];                   // (0 = current | 1 = previous | 2 = bump) 

    coreBatchList m_Hail;                             // 
    coreBatchList m_HailWave;                         // 
    coreObject3D  m_aHailRaw[CALOR_HAILS_RAWS];       // 

    coreBatchList m_Chest;                            // 
    coreBatchList m_ChestWave;                        // 
    coreObject3D  m_aChestRaw  [CALOR_CHESTS_RAWS];   // 
    coreFlow      m_afChestTime[CALOR_CHESTS];        // 

    coreObject3D m_AimArrow;                          // 
    coreObject3D m_aAimSphere[3];                     // 
    const cShip* m_pAimOwner;                         // 
    coreFlow     m_fAimTime;                          // 
    coreFlow     m_fAimAlpha;                         // 
    coreFloat    m_fAimFade;                          // 

    cCustomEnemy m_Bull;                              // 

    coreBatchList m_Star;                             // 
    coreBatchList m_StarChain;                        // 
    coreObject3D  m_aStarRaw    [CALOR_STARS_RAWS];   // 
    const cShip*  m_apStarOwner [CALOR_STARS];        // 
    coreVector2   m_avStarOffset[CALOR_STARS];        // 
    coreFloat     m_afStarLength[CALOR_STARS];        // 
    coreUint8     m_iStarSwing;                       // 
    coreUint8     m_iStarAnimate;                     // 
    coreUint8     m_iStarConnect;                     // 

    coreFloat m_fSwingSpeed;                          // 
    coreFlow  m_fSwingStart;                          // 
    coreFlow  m_afSwingValue[CALOR_STARS];            // 

    cShip*      m_apCatchObject[CALOR_STARS];         // 
    coreVector2 m_avCatchPos   [CALOR_STARS];         // 
    coreVector2 m_avCatchDir   [CALOR_STARS];         // 
    coreFlow    m_fCatchTransfer;                     // 

    cCustomEnemy m_Boulder;                           // 

    coreFlow m_fAnimation;                            // animation value


public:
    cCalorMission()noexcept;
    ~cCalorMission()final;

    DISABLE_COPY(cCalorMission)
    ASSIGN_ID(6, "CALOR")

    // 
    void EnableSnow ();
    void DisableSnow(const coreBool bAnimated);

    // 
    void EnableLoad (const cShip* pOwner);
    void DisableLoad(const coreBool bAnimated);

    // 
    void EnableHail (const coreUintW iIndex);
    void DisableHail(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableChest (const coreUintW iIndex);
    void DisableChest(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableAim (const cShip* pOwner);
    void DisableAim(const coreBool bAnimated);

    // 
    void EnableStar (const coreUintW iIndex, const cShip* pOwner, const coreVector2 vOffset);
    void DisableStar(const coreUintW iIndex, const coreBool bAnimated);

    // 
    inline void BumpLoad(const coreFloat fValue) {ASSERT(fValue > 0.0f) m_afLoadPower[0] = MIN(m_afLoadPower[0] + fValue, I_TO_F(CALOR_LOADS)); m_afLoadPower[2] = 1.0f;}

    // 
    inline coreBool IsChestReady(const coreUintW iIndex)const {ASSERT(iIndex < CALOR_CHESTS) return (m_afChestTime[iIndex] == 0.0f);}

    // 
    inline coreBool IsStarEnabled(const coreUintW iIndex)const {ASSERT(iIndex < CALOR_STARS) return (m_apStarOwner[iIndex] != NULL);}

    // 
    inline void SetStarLength (const coreUintW iIndex, const coreFloat fLength) {ASSERT(iIndex < CALOR_STARS) m_afStarLength[iIndex] = fLength;}
    inline void SetStarConnect(const coreUintW iIndex, const coreBool bConnect) {ASSERT(iIndex < CALOR_STARS) SET_BIT(m_iStarConnect, iIndex, bConnect)}

    // 
    void StartSwing(const coreFloat fSpeed);
    void StopSwing();
    inline void SetSwingSpeed(const coreFloat fSpeed) {ASSERT(m_fSwingStart >= 1.0f) m_fSwingSpeed = fSpeed;}

    // 
    void CatchObject  (const coreUintW iIndex, cShip* pObject);
    void UncatchObject(const coreUintW iIndex);

    // 
    inline const coreFloat& GetStarLength(const coreUintW iIndex)const {ASSERT(iIndex < CALOR_STARS) return m_afStarLength[iIndex];}

    // 
    inline cSnow*        GetSnow       ()                            {return &m_Snow;}
    inline coreObject3D* GetHail       (const coreUintW iIndex)      {ASSERT(iIndex < CALOR_HAILS)  return &m_aHailRaw    [iIndex * 2u];}
    inline coreObject3D* GetChest      (const coreUintW iIndex)      {ASSERT(iIndex < CALOR_CHESTS) return &m_aChestRaw   [iIndex * 2u];}
    inline coreObject3D* GetStar       (const coreUintW iIndex)      {ASSERT(iIndex < CALOR_STARS)  return &m_aStarRaw    [iIndex * (CALOR_CHAINS + 1u)];}
    inline cShip*        GetCatchObject(const coreUintW iIndex)const {ASSERT(iIndex < CALOR_STARS)  return m_apCatchObject[iIndex];}
    inline cCustomEnemy* GetBoulder    ()                            {return &m_Boulder;}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_06_intro.ogg";}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __RenderOwnTop   ()final;
    void __MoveOwnBefore  ()final;
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
    coreUint64    m_iGenerateTest;                           // 
    coreUint64    m_iGenerateHit;                            // 

    coreBatchList m_Pearl;                                   // 
    coreBatchList m_PearlWave;                               // 
    coreObject3D  m_aPearlRaw[MUSCUS_PEARLS_RAWS];           // 
    coreUint64    m_iPearlActive;                            // 
    coreUint64    m_iPearlHidden;                            // 
    coreUint8     m_iPearlPitch;                             // 

    coreObject3D m_aZombie[MUSCUS_ZOMBIES];                  // 

    coreObject3D m_Diamond;                                  // 
    coreUint8    m_iDiamondIndex;                            // 

    coreSpline2  m_aStrikeSpline [MUSCUS_PEARLS];            // 
    coreFlow     m_afStrikeTime  [MUSCUS_PEARLS];            // 
    cPlayer*     m_apStrikePlayer[MUSCUS_PEARLS];            // 
    const cShip* m_apStrikeTarget[MUSCUS_PEARLS];            // 
    coreUint64   m_iStrikeState;                             // 

    coreFlow m_fAnimation;                                   // animation value


public:
    cMuscusMission()noexcept;
    ~cMuscusMission()final;

    DISABLE_COPY(cMuscusMission)
    ASSIGN_ID(7, "MUSCUS")

    // 
    void EnableGenerate (const coreUintW iIndex);
    void DisableGenerate(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnablePearl (const coreUintW iIndex);
    void DisablePearl(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void EnableZombie (const coreUintW iIndex);
    void DisableZombie(const coreUintW iIndex, const coreBool bAnimated);

    // 
    void StartDiamond(const coreUintW iIndex);
    void EndDiamond  (const coreBool bAnimated);

    // 
    inline void ShowGenerate(const coreUintW iIndex, const coreFloat fTime) {ASSERT(iIndex < MUSCUS_GENERATES) if(m_afGenerateTime[iIndex] >= 0.0f) m_afGenerateTime[iIndex] = MAX(m_afGenerateTime[iIndex], fTime);}
    inline void BangGenerate(const coreUintW iIndex)                        {ASSERT(iIndex < MUSCUS_GENERATES) if(m_afGenerateTime[iIndex] >= 0.0f) m_afGenerateBang[iIndex] = 1.0f;}
    inline void TestGenerate(const coreUintW iIndex, const coreBool bTest)  {ASSERT(iIndex < MUSCUS_GENERATES) SET_BIT(m_iGenerateTest, iIndex, bTest)}

    // 
    inline coreBool IsGenerateHit    (const coreUintW iIndex)const {ASSERT(iIndex < MUSCUS_GENERATES) return HAS_BIT(m_iGenerateHit, iIndex);}
    inline coreBool IsGenerateDiamond(const coreUintW iIndex)const {ASSERT(iIndex < MUSCUS_GENERATES) return (iIndex == m_iDiamondIndex);}

    // 
    inline coreBool IsPearlActive  (const coreUintW iIndex)const {ASSERT(iIndex < MUSCUS_PEARLS) return (HAS_BIT(m_iPearlActive, iIndex) && !m_apStrikeTarget[iIndex]);}
    inline coreBool IsPearlValidAny()const                       {return (m_iPearlActive != 0u);}

    // 
    coreFloat RetrievePearlPitch();

    // 
    void StrikeAttack(const coreUintW iIndex, cPlayer* pPlayer, const cShip* pTarget);

    // 
    inline cPlayer*           GetStrikePlayer(const coreUintW iIndex)const {ASSERT(iIndex < MUSCUS_PEARLS) return m_apStrikePlayer[iIndex];}
    inline const coreVector2& GetStrikeDir   (const coreUintW iIndex)const {ASSERT(iIndex < MUSCUS_PEARLS) return m_aStrikeSpline [iIndex].GetNode(1u).tTangent;}
    inline const coreUint64&  GetStrikeState ()const                       {return m_iStrikeState;}

    // 
    inline coreObject3D* GetGenerate(const coreUintW iIndex) {ASSERT(iIndex < MUSCUS_GENERATES) return &m_aGenerateRaw[iIndex * 2u];}
    inline coreObject3D* GetPearl   (const coreUintW iIndex) {ASSERT(iIndex < MUSCUS_PEARLS)    return &m_aPearlRaw   [iIndex * 2u];}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_07_intro.ogg";}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnOver  ()final;
    void __MoveOwnAfter   ()final;
};


// ****************************************************************
// Ater mission class
class cAterMission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 
    cEigengrauBoss  m_Eigengrau;    // 

    cMission* m_pInnerMission;      // 
    coreInt32 m_iNextID;            // 

    cTurf    m_Turf;                // 
    coreBool m_bTurfState;          // 

    coreBool m_Secret;              // 


public:
    cAterMission()noexcept;
    ~cAterMission()final;

    DISABLE_COPY(cAterMission)
    ASSIGN_ID(8, "ATER")

    // 
    void RequestInnerMission(const coreInt32 iID);
    void LoadInnerMission   (const coreInt32 iID);

    // 
    inline void LaunchSecret() {m_Secret = true;}

    // 
    inline RETURN_NONNULL cMission* GetInnerMission()const {ASSERT(m_pInnerMission) return m_pInnerMission;}

    // 
    inline cTurf* GetTurf() {return &m_Turf;}


private:
    // execute own routines
    void __SetupOwn       ()final;
    void __RenderOwnBottom()final;
    void __RenderOwnUnder ()final;
    void __RenderOwnOver  ()final;
    void __RenderOwnTop   ()final;
    void __MoveOwnBefore  ()final;
    void __MoveOwnMiddle  ()final;
    void __MoveOwnAfter   ()final;
};


// ****************************************************************
// Intro mission class
class cIntroMission final : public cMission
{
private:
    cIntroBoss m_Intro;                                         // 

    cFigure  m_aaManual     [MISSION_PLAYERS][INTRO_MANUALS];   // 
    coreFlow m_aafManualTime[MISSION_PLAYERS][INTRO_MANUALS];   // 

    coreBool m_bFirstPlay;                                      // 


public:
    cIntroMission()noexcept;

    DISABLE_COPY(cIntroMission)
    ASSIGN_ID(99, "INTRO")

    // 
    inline void EnableManual(const coreUintW iPlayerIndex, const coreUintW iManualIndex) {ASSERT(iPlayerIndex < MISSION_PLAYERS) ASSERT(iManualIndex < INTRO_MANUALS) m_aafManualTime[iPlayerIndex][iManualIndex] = 3.0f;}

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_00_intro.ogg";}


private:
    // execute own routines
    void __SetupOwn    ()final;
    void __RenderOwnTop()final;
    void __MoveOwnAfter()final;
};


// ****************************************************************
// Bonus1 mission class
class cBonus1Mission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cBonus1Mission()noexcept;

    DISABLE_COPY(cBonus1Mission)
    ASSIGN_ID(101, "BONUS 1")

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_09.ogg";}


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Bonus2 mission class
class cBonus2Mission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cBonus2Mission()noexcept;

    DISABLE_COPY(cBonus2Mission)
    ASSIGN_ID(102, "BONUS 2")

    // get object properties
    inline const coreChar* GetMusicName()const final {return "mission_10.ogg";}


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Error mission class
class cErrorMission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cErrorMission()noexcept;

    DISABLE_COPY(cErrorMission)
    ASSIGN_ID(201, "ERROR")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// Demo mission class
class cDemoMission final : public cMission
{
private:
    cProjectOneBoss m_ProjectOne;   // 


public:
    cDemoMission()noexcept;
    ~cDemoMission()final;

    DISABLE_COPY(cDemoMission)
    ASSIGN_ID(202, "DEMO")


private:
    // execute own routines
    void __SetupOwn()final;
};


// ****************************************************************
// 
template <typename F> RETURN_RESTRICT coreSpline2* cMission::_AddPath(const coreUint16 iCodeLine, F&& nInitFunc)
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
template <typename T, typename F> RETURN_RESTRICT cEnemySquad* cMission::_AddSquad(const coreUint16 iCodeLine, const coreUint8 iNum, F&& nInitFunc)
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
    ASSERT((iFrom <= iTo) && piIndexList && iSize)

    // 
    if(((piIndexList[0] == TAKE_MISSION)  && (iTo != TAKE_MISSION)) ||
       ((piIndexList[0] == TAKE_TRAINING) && (iTo == TAKE_MISSION)))
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