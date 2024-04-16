///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TABLE_H_
#define _P1_GUARD_TABLE_H_

// TODO 4: handle -INTERFACE_BANNER_DURATION_BOSS differently (also remove MAX(this->GetTimeSegment(iMissionIndex, iSegmentIndex), 0.0f))


// ****************************************************************
// table definitions
#define TABLE_MISSIONS (MISSIONS)   // 
#define TABLE_BOSSES   (BOSSES)     // 
#define TABLE_SEGMENTS (SEGMENTS)   // 

#define TABLE_TIME_TO_UINT(x)   (F_TO_UI((x)  * 1000.0f))
#define TABLE_TIME_TO_FLOAT(x)  (I_TO_F ((x)) / 1000.0f))

#define __TABLE_SCORE_COMBO(x)  (1.0f + 0.1f * I_TO_F(x))
#define __TABLE_TIME_CONVERT(x) (I_TO_F(x) * m_fFrameTime)


// ****************************************************************
// data table class
class cDataTable final
{
public:
    // 
    struct sCounter final
    {
        coreUint64 iDamageGiven;       // 
        coreUint32 iDamageTaken;       // 
        coreUint32 iContinuesUsed;     // 
        coreUint32 iRepairsUsed;       // 
        coreUint64 iMovesMade;         // 
        coreUint32 iTurnsMade;         // 
        coreUint32 iRollsMade;         // 
        coreUint64 iBulletsShot;       // 
        coreUint64 iChromaCollected;   // 
        coreUint32 iItemsCollected;    // 
    };


private:
    sCounter m_CounterTotal;                                       // 
    sCounter m_aCounterMission [TABLE_MISSIONS];                   // 
    sCounter m_aaCounterSegment[TABLE_MISSIONS][TABLE_SEGMENTS];   // 

    coreUint8 m_aiMedalMission [TABLE_MISSIONS];                   // 
    coreUint8 m_aaiMedalSegment[TABLE_MISSIONS][TABLE_SEGMENTS];   // 

    coreUint8  m_aiFragment[TABLE_MISSIONS];                       // (bitfield) 
    coreUint32 m_aiBadge   [TABLE_MISSIONS];                       // (bitfield) 

    const cPlayer* m_pOwner;                                       // 


public:
    cDataTable()noexcept;

    DISABLE_COPY(cDataTable)

    // 
    void Update();

    // 
    void Reset();

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
    void GiveBadge(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    void GiveBadge();

    // 
    inline void SetOwner(const cPlayer* pOwner) {m_pOwner = pOwner;}

    // 
    inline const sCounter&  GetCounterTotal  ()const                                                             {return m_CounterTotal;}
    inline const sCounter&  GetCounterMission(const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aCounterMission [iMissionIndex];}
    inline const sCounter&  GetCounterSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaCounterSegment[iMissionIndex][iSegmentIndex];}
    inline const coreUint8& GetMedalMission  (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiMedalMission  [iMissionIndex];}
    inline const coreUint8& GetMedalSegment  (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiMedalSegment [iMissionIndex][iSegmentIndex];}
    inline       coreBool   GetFragment      (const coreUintW iMissionIndex, const coreUintW iBossIndex)const    {ASSERT(iMissionIndex < TABLE_MISSIONS && iBossIndex    < TABLE_BOSSES)   return HAS_BIT(m_aiFragment[iMissionIndex], iBossIndex);}
    inline       coreBool   GetBadge         (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return HAS_BIT(m_aiBadge   [iMissionIndex], iSegmentIndex);}
};


// ****************************************************************
// score table class
class cScoreTable final
{
private:
    coreProtect<coreUint32> m_iScoreTotal;                                       // 
    coreProtect<coreUint32> m_aiScoreMission [TABLE_MISSIONS];                   // 
    coreProtect<coreUint32> m_aaiScoreSegment[TABLE_MISSIONS][TABLE_SEGMENTS];   // 

    coreProtect<coreUint32> m_aiComboValue[2];                                   // absolute values for combo calculations (0 = current value, 1 = max value) 
    coreProtect<coreUint32> m_aiChainValue[2];                                   // 
    coreProtect<coreFlow>   m_fComboCooldown;                                    // 

    const cPlayer* m_pOwner;                                                     // 


public:
    cScoreTable()noexcept;

    DISABLE_COPY(cScoreTable)

    // 
    void Update();

    // 
    void Reset();

    // control scoring stats   
    coreUint32 AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    coreUint32 AddScore(const coreUint32 iValue, const coreBool bModified, const coreVector3 vPosition);
    coreUint32 AddScore(const coreUint32 iValue, const coreBool bModified);
    void       AddCombo(const coreUint32 iValue);
    void       AddChain(const coreUint32 iValue);

    // 
    void RefreshCombo ();
    void CancelCombo  ();
    void TransferCombo();
    void TransferChain();

    // 
    inline void SetOwner(const cPlayer* pOwner) {m_pOwner = pOwner;}

    // 
    inline coreUint32 GetScoreTotal   ()const                                                             {return m_iScoreTotal;}
    inline coreUint32 GetScoreMission (const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiScoreMission [iMissionIndex];}
    inline coreUint32 GetScoreSegment (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiScoreSegment[iMissionIndex][iSegmentIndex];}
    inline coreFloat  GetCurCombo     ()const                                                             {return __TABLE_SCORE_COMBO(m_aiComboValue[0]);}
    inline coreFloat  GetMaxCombo     ()const                                                             {return __TABLE_SCORE_COMBO(m_aiComboValue[1]);}
    inline coreUint32 GetCurChain     ()const                                                             {return m_aiChainValue[0];}
    inline coreUint32 GetMaxChain     ()const                                                             {return m_aiChainValue[1];}
    inline coreFloat  GetComboCooldown()const                                                             {return m_fComboCooldown;}
};


// ****************************************************************
// time table class
class cTimeTable final
{
private:
    coreProtect<coreUint32> m_iTimeEvent;                                       // 

    coreProtect<coreUint32> m_iTimeTotal;                                       // 
    coreProtect<coreUint32> m_aiTimeMission [TABLE_MISSIONS];                   // total time per mission 
    coreProtect<coreUint32> m_aaiTimeSegment[TABLE_MISSIONS][TABLE_SEGMENTS];   // 

    coreFloat m_fFrameTime;                                                     // 


public:
    cTimeTable()noexcept;

    DISABLE_COPY(cTimeTable)

    // 
    void Update();

    // 
    void Reset();

    // 
    coreFloat GetTimeSegmentSafe(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const;
    coreFloat GetTimeSegmentSafe()const;

    // 
    inline coreUint32 GetTimeEventRaw  ()const                                                             {return m_iTimeEvent;}
    inline coreUint32 GetTimeTotalRaw  ()const                                                             {return m_iTimeTotal;}
    inline coreUint32 GetTimeMissionRaw(const coreUintW iMissionIndex)const                                {ASSERT(iMissionIndex < TABLE_MISSIONS)                                   return m_aiTimeMission [iMissionIndex];}
    inline coreUint32 GetTimeSegmentRaw(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iSegmentIndex < TABLE_SEGMENTS) return m_aaiTimeSegment[iMissionIndex][iSegmentIndex];}
    inline coreFloat  GetTimeEvent     ()const                                                             {return __TABLE_TIME_CONVERT(this->GetTimeEventRaw  ());}
    inline coreFloat  GetTimeTotal     ()const                                                             {return __TABLE_TIME_CONVERT(this->GetTimeTotalRaw  ());}
    inline coreFloat  GetTimeMission   (const coreUintW iMissionIndex)const                                {return __TABLE_TIME_CONVERT(this->GetTimeMissionRaw(iMissionIndex));}
    inline coreFloat  GetTimeSegment   (const coreUintW iMissionIndex, const coreUintW iSegmentIndex)const {return __TABLE_TIME_CONVERT(this->GetTimeSegmentRaw(iMissionIndex, iSegmentIndex));}
};


#endif // _P1_GUARD_TABLE_H_