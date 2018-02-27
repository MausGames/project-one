//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TABLE_H_
#define _P1_GUARD_TABLE_H_


// ****************************************************************
// table definitions
#define TABLE_MISSIONS (MISSIONS)   // 
#define TABLE_BOSSES   (BOSSES)     // 

#define __TABLE_SCORE_COMBO(x) (LERP(50.0f, 1.0f, RCP(1.0f + I_TO_F(x)*0.0002f)))


// ****************************************************************
// score table class
class cScoreTable final
{
private:
    coreProtect<coreUint32> m_iScoreTotal;                                    // 
    coreProtect<coreUint32> m_aiScoreMission[TABLE_MISSIONS];                 // 
    coreProtect<coreUint32> m_aaiScoreBoss  [TABLE_MISSIONS][TABLE_BOSSES];   // 

    coreProtect<coreUint32> m_aiComboValue[2];                                // absolute values for combo calculations (0 = current value, 1 = max value) 
    coreProtect<coreUint32> m_aiChainValue[2];                                // 
    coreProtect<coreFloat>  m_fChainCooldown;                                 // 

    const cPlayer* m_pOwner;                                                  // 


public:
    cScoreTable()noexcept;

    DISABLE_COPY(cScoreTable)

    // 
    void Update();

    // 
    void Reset();

    // control scoring stats 
    void AddScore(const coreUint32 iValue, const coreBool bModified, const coreUintW iMissionIndex, const coreUintW iBossIndex);
    void AddScore(const coreUint32 iValue, const coreBool bModified);
    void AddCombo(const coreUint32 iValue);
    void AddCombo(const coreFloat  fModifier);
    void AddChain(const coreUint32 iValue);

    // 
    void ReduceCombo  ();
    void TransferChain();

    // 
    inline void SetOwner(const cPlayer* pOwner) {m_pOwner = pOwner;}

    // 
    inline coreUint32 GetScoreTotal   ()const                                                          {return m_iScoreTotal;}
    inline coreUint32 GetScoreMission (const coreUintW iMissionIndex)const                             {ASSERT(iMissionIndex < TABLE_MISSIONS)                              return m_aiScoreMission[iMissionIndex];}
    inline coreUint32 GetScoreBoss    (const coreUintW iMissionIndex, const coreUintW iBossIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iBossIndex < TABLE_BOSSES) return m_aaiScoreBoss  [iMissionIndex][iBossIndex];}
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
    coreProtect<coreFlow> m_fTimeEvent;                                    // 

    coreProtect<coreFlow> m_fTimeTotal;                                    // 
    coreProtect<coreFlow> m_afTimeMission[TABLE_MISSIONS];                 // total time per mission 
    coreProtect<coreFlow> m_aafTimeBoss  [TABLE_MISSIONS][TABLE_BOSSES];   // total time per boss battle 


public:
    cTimeTable()noexcept;

    DISABLE_COPY(cTimeTable)

    // 
    void Update();

    // 
    void Reset();

    // 
    inline coreFloat GetTimeEvent  ()const                                                          {return m_fTimeEvent;}
    inline coreFloat GetTimeTotal  ()const                                                          {return m_fTimeTotal;}
    inline coreFloat GetTimeMission(const coreUintW iMissionIndex)const                             {ASSERT(iMissionIndex < TABLE_MISSIONS)                              return m_afTimeMission[iMissionIndex];}
    inline coreFloat GetTimeBoss   (const coreUintW iMissionIndex, const coreUintW iBossIndex)const {ASSERT(iMissionIndex < TABLE_MISSIONS && iBossIndex < TABLE_BOSSES) return m_aafTimeBoss  [iMissionIndex][iBossIndex];}
};


#endif // _P1_GUARD_TABLE_H_