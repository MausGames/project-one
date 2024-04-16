///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TABLE_H_
#define _P1_GUARD_TABLE_H_

// TODO 4: handle -INTERFACE_BANNER_DURATION_BOSS differently (also remove MAX(this->GetTimeSegment(iMissionIndex, iSegmentIndex), 0.0f)) -> currently changed to negative-number with coreDouble(I_TO_F(x))


// ****************************************************************
// table definitions
#define TABLE_MISSIONS (MISSIONS)       // 
#define TABLE_BOSSES   (BOSSES)         // 
#define TABLE_SEGMENTS (SEGMENTS)       // 
#define TABLE_BADGES   (BADGES)         // 
#define TABLE_HELPERS  (HELPERS - 1u)   // 
#define TABLE_RUNS     (CONTINUES)      // 

#define TABLE_TIME_TO_UINT(x)   (F_TO_UI  (coreDouble(x) * 1000.0))
#define TABLE_TIME_TO_FLOAT(x)  (coreFloat(coreDouble(x) / 1000.0))

#define __TABLE_TIME_CONVERT(x) (coreFloat(coreDouble(I_TO_F(x)) * m_dFrameTime))


// ****************************************************************
// data table class
class cDataTable final
{
public:
    // 
    struct sCounter final
    {
        coreUint64 iDamageGiven;      // 
        coreUint32 iDamageTaken;      // 
        coreUint16 iContinuesUsed;    // 
        coreUint16 iRepairsUsed;      // 
        coreUint16 iShiftGoodAdded;   // 
        coreUint16 iShiftBadAdded;    // 
        coreUint32 iTurnsMade;        // 
        coreUint64 iMovesMade;        // 
        coreUint64 iBulletsShot;      // 
    };


private:
    sCounter m_CounterTotal;                                       // 
    sCounter m_aCounterMission [TABLE_MISSIONS];                   // 
    sCounter m_aaCounterSegment[TABLE_MISSIONS][TABLE_SEGMENTS];   // 

    coreUint8 m_aiMedalMission [TABLE_MISSIONS];                   // 
    coreUint8 m_aaiMedalSegment[TABLE_MISSIONS][TABLE_SEGMENTS];   // 

    coreUint8 m_aiFragment[TABLE_MISSIONS];                        // (bitfield) 
    coreUint8 m_aaiBadge  [TABLE_MISSIONS][TABLE_SEGMENTS];        // (bitfield) 
    coreUint8 m_aiHelper  [TABLE_MISSIONS];                        // (bitfield) 

    const cPlayer* m_pOwner;                                       // 


public:
    cDataTable()noexcept;

    DISABLE_COPY(cDataTable)

    // 
    void Update();

    // 
    void Reset();

    // 
    void RevertSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void RevertSegment();

    // 
    RETURN_NONNULL sCounter* EditCounterTotal  ();
    RETURN_NONNULL sCounter* EditCounterMission(const coreUintW iMissionIndex);
    RETURN_NONNULL sCounter* EditCounterMission();
    RETURN_NONNULL sCounter* EditCounterSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    RETURN_NONNULL sCounter* EditCounterSegment();

    // 
    void GiveMedalMission(const coreUint8 iMedal, const coreUintW iMissionIndex);
    void GiveMedalMission(const coreUint8 iMedal);
    void GiveMedalSegment(const coreUint8 iMedal, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void GiveMedalSegment(const coreUint8 iMedal);

    // 
    void GiveFragment(const coreUintW iMissionIndex, const coreUintW iBossIndex);
    void GiveFragment();

    // 
    void GiveBadge(const coreUintW iBadgeIndex, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void GiveBadge(const coreUintW iBadgeIndex);

    // 
    void GiveHelper(const coreUintW iHelperIndex, const coreUintW iMissionIndex);
    void GiveHelper(const coreUintW iHelperIndex);

    // 
    inline void SetOwner(const cPlayer* pOwner) {m_pOwner = pOwner;}

    // 
    inline const sCounter&  GetCounterTotal  ()const                                                             {return m_CounterTotal;}
    inline const sCounter&  GetCounterMission(const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aCounterMission [iMissionIndex];}
    inline const sCounter&  GetCounterSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaCounterSegment[iMissionIndex][iSegmentIndex];}
    inline const coreUint8& GetMedalMission  (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiMedalMission  [iMissionIndex];}
    inline const coreUint8& GetMedalSegment  (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiMedalSegment [iMissionIndex][iSegmentIndex];}
    inline const coreUint8& GetFragmentAll   (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiFragment      [iMissionIndex];}
    inline const coreUint8& GetBadgeAll      (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiBadge        [iMissionIndex][iSegmentIndex];}
    inline const coreUint8& GetHelperAll     (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiHelper        [iMissionIndex];}
    inline       coreBool   GetFragment      (const coreUintW iMissionIndex, const coreUintW iBossIndex)const    {ASSERT(iMissionIndex < TABLE_MISSIONS && iBossIndex    < TABLE_BOSSES)   return HAS_BIT(m_aiFragment[iMissionIndex], iBossIndex);}
    inline       coreBool   GetBadge         (const coreUintW iBadgeIndex,  const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iBadgeIndex < TABLE_BADGES && iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return HAS_BIT(m_aaiBadge[iMissionIndex][iSegmentIndex], iBadgeIndex);}
    inline       coreBool   GetHelper        (const coreUintW iHelperIndex, const coreUintW iMissionIndex)const  {ASSERT(iHelperIndex < TABLE_HELPERS && iMissionIndex < TABLE_MISSIONS)   return HAS_BIT(m_aiHelper[iMissionIndex], iHelperIndex);}
};


// ****************************************************************
// score table class
class cScoreTable final
{
private:
    coreProtect<coreUint32> m_iScoreTotal;                                                    // 
    coreProtect<coreUint32> m_aiScoreMission [TABLE_MISSIONS];                                // 
    coreProtect<coreUint32> m_aaiScoreSegment[TABLE_MISSIONS][TABLE_SEGMENTS];                // 

    coreProtect<coreUint32> m_iCurCombo;                                                      // 
    coreProtect<coreUint32> m_iCurChain;                                                      // 

    coreProtect<coreFlow>   m_fCooldown;                                                      // 
    coreProtect<coreUint32> m_iOverride;                                                      // 

    coreProtect<coreUint32> m_aaiMaxSeriesSegment[TABLE_MISSIONS][TABLE_SEGMENTS];            // 

    coreProtect<coreUint32> m_aiRunTotal       [TABLE_RUNS];                                   // 
    coreProtect<coreUint32> m_aaiRunMission    [TABLE_RUNS][TABLE_MISSIONS];                   // 
    coreProtect<coreUint32> m_aaaiRunSegment   [TABLE_RUNS][TABLE_MISSIONS][TABLE_SEGMENTS];   // 
    coreProtect<coreUint8>  m_aiRunMissionIndex[TABLE_RUNS];                                   // 
    coreProtect<coreUint8>  m_aiRunSegmentIndex[TABLE_RUNS];                                   // 

    cPlayer* m_pOwner;                                                                         // 


public:
    cScoreTable()noexcept;

    DISABLE_COPY(cScoreTable)

    // 
    void Update();

    // 
    void Reset();

    // 
    void RevertSegment   (const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void RevertSegment   ();
    void RevertSegmentNew(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void RevertSegmentNew();

    // 
    void StoreRun(const coreUintW iRunIndex, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);

    // control scoring stats   
    coreUint32 AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    coreUint32 AddScore(const coreUint32 iValue, const coreBool bModified);
    void       AddCombo(const coreUint32 iValue);
    void       AddChain(const coreUint32 iValue);

    // 
    void TransferChain  ();
    void RefreshCooldown();
    void CancelCooldown ();

    // 
    inline void     SetOverride  (const coreFloat fLerp) {m_iOverride = fLerp ? F_TO_UI(LERP(1.0f, 100.0f, fLerp)) : 0u; ASSERT((fLerp >= 0.0f) && (fLerp <= 1.0f))}
    inline void     ResetOverride()                      {m_iOverride = UINT32_MAX;}
    inline coreBool HasOverride  ()const                 {return (m_iOverride != UINT32_MAX);}

    // 
    inline coreUint32 ModifyValue(const coreUint32 iValue)const {return (iValue * this->GetModifier()) / 10u;}

    // 
    inline void SetOwner(cPlayer* pOwner) {m_pOwner = pOwner;}

    // 
    inline coreUint32 GetScoreTotal      ()const                                                                                        {return m_iScoreTotal;}
    inline coreUint32 GetScoreMission    (const coreUintW iMissionIndex)const                                                           {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiScoreMission [iMissionIndex];}
    inline coreUint32 GetScoreSegment    (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const                            {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiScoreSegment[iMissionIndex][iSegmentIndex];}
    inline coreUint32 GetCurCombo        ()const                                                                                        {return m_iCurCombo;}
    inline coreUint32 GetCurChain        ()const                                                                                        {return m_iCurChain;}
    inline coreFloat  GetCooldown        ()const                                                                                        {return m_fCooldown;}
    inline coreUint32 GetModifier        ()const                                                                                        {return this->HasOverride() ? (m_iOverride + 0u) : (m_iCurCombo + 10u);}
    inline coreUint32 GetMaxSeriesSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const                            {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiMaxSeriesSegment[iMissionIndex][iSegmentIndex];}
    inline coreUint32 GetRunTotal        (const coreUintW iRunIndex)const                                                               {ASSERT(iRunIndex < TABLE_RUNS) return m_aiRunTotal[iRunIndex];}
    inline coreUint32 GetRunMission      (const coreUintW iRunIndex, const coreUintW iMissionIndex)const                                {ASSERT(iRunIndex < TABLE_RUNS && iMissionIndex < TABLE_MISSIONS)                                   return m_aaiRunMission [iRunIndex][iMissionIndex];}
    inline coreUint32 GetRunSegment      (const coreUintW iRunIndex, const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iRunIndex < TABLE_RUNS && iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaaiRunSegment[iRunIndex][iMissionIndex][iSegmentIndex];}
    inline coreUint8  GetRunMissionIndex (const coreUintW iRunIndex)const                                                               {ASSERT(iRunIndex < TABLE_RUNS) return m_aiRunMissionIndex[iRunIndex];}
    inline coreUint8  GetRunSegmentIndex (const coreUintW iRunIndex)const                                                               {ASSERT(iRunIndex < TABLE_RUNS) return m_aiRunSegmentIndex[iRunIndex];}


private:
    // 
    void __ChangeMaxSeries(const coreUint32 iMaxValue, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void __ChangeMaxSeries(const coreUint32 iMaxValue);
};


// ****************************************************************
// time table class
class cTimeTable final
{
private:
    coreProtect<coreUint32> m_iTimeMono;                                             // 
    coreProtect<coreUint32> m_iTimeEvent;                                            // 

    coreProtect<coreUint32> m_iTimeTotal;                                            // (including time between segments) 
    coreProtect<coreUint32> m_aiTimeMission [TABLE_MISSIONS];                        // total time per mission 
    coreProtect<coreUint32> m_aaiTimeSegment[TABLE_MISSIONS][TABLE_SEGMENTS];        // 

    coreProtect<coreUint16> m_aiShiftGoodMission [TABLE_MISSIONS];                   // 
    coreProtect<coreUint16> m_aaiShiftGoodSegment[TABLE_MISSIONS][TABLE_SEGMENTS];   // 
    coreProtect<coreUint16> m_aiShiftBadMission  [TABLE_MISSIONS];                   // 
    coreProtect<coreUint16> m_aaiShiftBadSegment [TABLE_MISSIONS][TABLE_SEGMENTS];   // 

    coreDouble m_dFrameTime;                                                         // 


public:
    cTimeTable()noexcept;

    DISABLE_COPY(cTimeTable)

    // 
    void Update();

    // 
    void Reset();

    // 
    void RevertSegment   (const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void RevertSegment   ();
    void RevertSegmentNew(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void RevertSegmentNew();

    // 
    void StartBoss(const coreUintW iMissionIndex, const coreUintW iBossIndex);
    void StartBoss();

    // 
    void AddShiftGood(const coreUint16 iValue, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void AddShiftGood(const coreUint16 iValue);
    void AddShiftBad (const coreUint16 iValue, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void AddShiftBad (const coreUint16 iValue);

    // 
    coreFloat GetTimeSegmentSafe       (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const;
    coreFloat GetTimeSegmentSafe       ()const;
    coreInt32 GetShiftSegmentSafe      (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const;
    coreInt32 GetShiftSegmentSafe      ()const;
    coreFloat GetTimeShiftedSegmentSafe(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const;
    coreFloat GetTimeShiftedSegmentSafe()const;

    // 
    inline coreUint32 GetTimeMonoRaw       ()const                                                             {return m_iTimeMono;}
    inline coreUint32 GetTimeEventRaw      ()const                                                             {return m_iTimeEvent;}
    inline coreUint32 GetTimeTotalRaw      ()const                                                             {return m_iTimeTotal;}
    inline coreUint32 GetTimeMissionRaw    (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiTimeMission [iMissionIndex];}
    inline coreUint32 GetTimeSegmentRaw    (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiTimeSegment[iMissionIndex][iSegmentIndex];}
    inline coreFloat  GetTimeEvent         ()const                                                             {return __TABLE_TIME_CONVERT(this->GetTimeEventRaw  ());}
    inline coreFloat  GetTimeTotal         ()const                                                             {return __TABLE_TIME_CONVERT(this->GetTimeTotalRaw  ());}
    inline coreFloat  GetTimeMission       (const coreUintW iMissionIndex)const                                {return __TABLE_TIME_CONVERT(this->GetTimeMissionRaw(iMissionIndex));}
    inline coreFloat  GetTimeSegment       (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {return __TABLE_TIME_CONVERT(this->GetTimeSegmentRaw(iMissionIndex, iSegmentIndex));}
    inline coreUint16 GetShiftGoodMission  (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiShiftGoodMission [iMissionIndex];}
    inline coreUint16 GetShiftGoodSegment  (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiShiftGoodSegment[iMissionIndex][iSegmentIndex];}
    inline coreUint16 GetShiftBadMission   (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiShiftBadMission  [iMissionIndex];}
    inline coreUint16 GetShiftBadSegment   (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiShiftBadSegment [iMissionIndex][iSegmentIndex];}
    inline coreInt32  GetShiftMission      (const coreUintW iMissionIndex)const                                {return coreInt32(this->GetShiftBadMission(iMissionIndex))                - coreInt32(this->GetShiftGoodMission(iMissionIndex));}
    inline coreInt32  GetShiftSegment      (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {return coreInt32(this->GetShiftBadSegment(iMissionIndex, iSegmentIndex)) - coreInt32(this->GetShiftGoodSegment(iMissionIndex, iSegmentIndex));}
    inline coreFloat  GetTimeShiftedMission(const coreUintW iMissionIndex)const                                {return this->GetTimeMission(iMissionIndex)                + I_TO_F(this->GetShiftMission(iMissionIndex));}
    inline coreFloat  GetTimeShiftedSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {return this->GetTimeSegment(iMissionIndex, iSegmentIndex) + I_TO_F(this->GetShiftSegment(iMissionIndex, iSegmentIndex));}
};


#endif // _P1_GUARD_TABLE_H_