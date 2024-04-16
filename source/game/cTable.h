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

// TODO: handle -INTERFACE_BANNER_DURATION_BOSS differently


// ****************************************************************
// table definitions
#define TABLE_MISSIONS (MISSIONS)   // 
#define TABLE_BOSSES   (BOSSES)     // 
#define TABLE_WAVES    (WAVES)      // 
#define TABLE_STAGES   (STAGES)     // 

#define __TABLE_SCORE_COMBO(x)  (LERP(50.0f, 1.0f, RCP(1.0f + I_TO_F(x) * 0.0002f)))
#define __TABLE_TIME_CONVERT(x) (I_TO_F(x) * FRAMERATE_TIME)


// ****************************************************************
// score table class
class cScoreTable final
{
private:
    coreProtect<coreUint32> m_iScoreTotal;                                    // 
    coreProtect<coreUint32> m_aiScoreMission[TABLE_MISSIONS];                 // 
    coreProtect<coreUint32> m_aaiScoreBoss  [TABLE_MISSIONS][TABLE_BOSSES];   // 
    coreProtect<coreUint32> m_aaiScoreWave  [TABLE_MISSIONS][TABLE_WAVES];    // 

    coreProtect<coreUint32> m_aiComboValue[2];                                // absolute values for combo calculations (0 = current value, 1 = max value) 
    coreProtect<coreUint32> m_aiChainValue[2];                                // 
    coreProtect<coreFlow>   m_fChainCooldown;                                 // 


    coreUint8 m_aaiMedal[TABLE_MISSIONS][TABLE_STAGES];   


    const cPlayer* m_pOwner;                                                  // 


public:
    cScoreTable()noexcept;

    DISABLE_COPY(cScoreTable)

    // 
    void Update();

    // 
    void Reset();

    // control scoring stats 
    void AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iBossIndex, const coreUintW iWaveIndex);
    void AddScore(const coreUint32 iValue, const coreBool bModified);
    void AddCombo(const coreUint32 iValue);
    void AddChain(const coreUint32 iValue);

    // 
    void ReduceCombo  ();
    void TransferChain();

    // 
    inline void SetOwner(const cPlayer* pOwner) {m_pOwner = pOwner;}


    void CalcBonusMedal(const coreUintW iMissionIndex)const;
    void CalcBonusSurvive()const;
    void CalcBonusTime(const coreUintW iMissionIndex, const coreUintW iBossIndex, const coreUintW iWaveIndex)const; // somewhere else   
    

    // 
    inline coreUint32 GetScoreTotal   ()const                                                          {return m_iScoreTotal;}
    inline coreUint32 GetScoreMission (const coreUintW iMissionIndex)const                             {ASSERT(iMissionIndex < TABLE_MISSIONS)                              return m_aiScoreMission[iMissionIndex];}
    inline coreUint32 GetScoreBoss    (const coreUintW iMissionIndex, const coreUintW iBossIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iBossIndex < TABLE_BOSSES) return m_aaiScoreBoss  [iMissionIndex][iBossIndex];}
    inline coreUint32 GetScoreWave    (const coreUintW iMissionIndex, const coreUintW iWaveIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iWaveIndex < TABLE_WAVES)  return m_aaiScoreWave  [iMissionIndex][iWaveIndex];}
    inline coreFloat  GetCurCombo     ()const                                                          {return __TABLE_SCORE_COMBO(m_aiComboValue[0]);}
    inline coreFloat  GetMaxCombo     ()const                                                          {return __TABLE_SCORE_COMBO(m_aiComboValue[1]);}
    inline coreUint32 GetCurChain     ()const                                                          {return m_aiChainValue[0];}
    inline coreUint32 GetMaxChain     ()const                                                          {return m_aiChainValue[1];}
    inline coreFloat  GetChainCooldown()const                                                          {return m_fChainCooldown;}
};


// ****************************************************************
// time table class
class cTimeTable final
{
private:
    coreProtect<coreUint32> m_iTimeEvent;                                    // 

    coreProtect<coreUint32> m_iTimeTotal;                                    // 
    coreProtect<coreUint32> m_aiTimeMission[TABLE_MISSIONS];                 // total time per mission 
    coreProtect<coreUint32> m_aaiTimeBoss  [TABLE_MISSIONS][TABLE_BOSSES];   // total time per boss battle 
    coreProtect<coreUint32> m_aaiTimeWave  [TABLE_MISSIONS][TABLE_WAVES];    // 


public:
    cTimeTable()noexcept;

    DISABLE_COPY(cTimeTable)

    // 
    void Update();

    // 
    void Reset();

    // 
    coreFloat GetTimeBossWave(const coreUintW iMissionIndex, const coreUintW iBossIndex, const coreUintW iWaveIndex)const;

    // 
    inline coreUint32 GetTimeEventRaw  ()const                                                          {return m_iTimeEvent;}
    inline coreUint32 GetTimeTotalRaw  ()const                                                          {return m_iTimeTotal;}
    inline coreUint32 GetTimeMissionRaw(const coreUintW iMissionIndex)const                             {ASSERT(iMissionIndex < TABLE_MISSIONS)                              return m_aiTimeMission[iMissionIndex];}
    inline coreUint32 GetTimeBossRaw   (const coreUintW iMissionIndex, const coreUintW iBossIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iBossIndex < TABLE_BOSSES) return m_aaiTimeBoss  [iMissionIndex][iBossIndex];}
    inline coreUint32 GetTimeWaveRaw   (const coreUintW iMissionIndex, const coreUintW iWaveIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iWaveIndex < TABLE_WAVES)  return m_aaiTimeWave  [iMissionIndex][iWaveIndex];}
    inline coreFloat  GetTimeEvent     ()const                                                          {return __TABLE_TIME_CONVERT(this->GetTimeEventRaw  ());}
    inline coreFloat  GetTimeTotal     ()const                                                          {return __TABLE_TIME_CONVERT(this->GetTimeTotalRaw  ());}
    inline coreFloat  GetTimeMission   (const coreUintW iMissionIndex)const                             {return __TABLE_TIME_CONVERT(this->GetTimeMissionRaw(iMissionIndex));}
    inline coreFloat  GetTimeBoss      (const coreUintW iMissionIndex, const coreUintW iBossIndex)const {return __TABLE_TIME_CONVERT(this->GetTimeBossRaw   (iMissionIndex, iBossIndex));}
    inline coreFloat  GetTimeWave      (const coreUintW iMissionIndex, const coreUintW iWaveIndex)const {return __TABLE_TIME_CONVERT(this->GetTimeWaveRaw   (iMissionIndex, iWaveIndex));}
};


#endif // _P1_GUARD_TABLE_H_