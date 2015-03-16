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
#define MISSION_BOSSES  (3u)     // default number of bosses per mission
#define MISSION_NO_BOSS (0xFF)   // no boss currently active (error-value)


// ****************************************************************
// phase management macros
#define PHASE_MAIN         m_anPhase.push_back([&]()                               // 
#define PHASE_SUB(t)       ((m_fPhaseTimeBefore <= (t)) && ((t) < m_fPhaseTime))   // 
#define PHASE_END_NOW      {m_anPhase.pop_back(); m_fPhaseTime = 0.0f;}            // 
#define PHASE_END_AFTER(t) {if(m_fPhaseTime >= (t)) PHASE_END_NOW}                 // 


// ****************************************************************
// mission interface
class INTERFACE cMission
{
protected:
    cBoss* m_apBoss[MISSION_BOSSES];                 // pointers to all available bosses

    cBoss*    m_pCurBoss;                            // pointer to currently active boss
    coreUintW m_iCurBossIndex;                       // index of the active boss (or error-value)

    std::vector<std::function<void()> > m_anPhase;   // 

    coreFlow  m_fPhaseTime;                          // 
    coreFloat m_fPhaseTimeBefore;                    // 


public:
    cMission()noexcept;
    virtual ~cMission() {}

    DISABLE_COPY(cMission)
    ENABLE_ID

    // setup the mission
    void Setup();

    // move the mission
    void Move();

    // access mission objects
    inline cBoss*           GetBoss        (const coreUintW& iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*           GetCurBoss     ()const                        {return m_pCurBoss;}
    inline const coreUintW& GetCurBossIndex()const                        {return m_iCurBossIndex;}


protected:
    // set active boss
    void _SetCurBoss(const coreUintW& iIndex);
    void _SetCurBoss(const cBoss*     pBoss);


private:
    // setup and move routines for derived classes
    virtual void __SetupOwn() {}
    virtual void __MoveOwn () {}
};


// ****************************************************************
// empty mission class
class cNoMission final : public cMission
{
public:
    cNoMission()noexcept {}

    DISABLE_COPY(cNoMission)
    ASSIGN_ID(0, "Nothing")
};


// ****************************************************************
// Virido mission class
class cViridoMission final : public cMission
{
private:
    cCrossfieldBoss m_Crossfield;   // 

    cScoutEnemy   m_aScout[10];     // 
    cWarriorEnemy m_Warrior;        // 

    coreSpline2 m_Spline;           // 


public:
    cViridoMission()noexcept;

    DISABLE_COPY(cViridoMission)
    ASSIGN_ID(1, "Virido")


private:
    // setup and move the Virido mission
    void __SetupOwn()override;
    void __MoveOwn ()override;
};


// ****************************************************************
// Nevo mission class
class cNevoMission final : public cMission
{
public:
    cNevoMission()noexcept {}

    DISABLE_COPY(cNevoMission)
    ASSIGN_ID(2, "Nevo")
};


// ****************************************************************
// Harena mission class
class cHarenaMission final : public cMission
{
public:
    cHarenaMission()noexcept {}

    DISABLE_COPY(cHarenaMission)
    ASSIGN_ID(3, "Harena")
};


// ****************************************************************
// Rutilus mission class
class cRutilusMission final : public cMission
{
public:
    cRutilusMission()noexcept {}

    DISABLE_COPY(cRutilusMission)
    ASSIGN_ID(4, "Rutilus")
};


// ****************************************************************
// Gelu mission class
class cGeluMission final : public cMission
{
public:
    cGeluMission()noexcept {}

    DISABLE_COPY(cGeluMission)
    ASSIGN_ID(5, "Gelu")
};


// ****************************************************************
// Calor mission class
class cCalorMission final : public cMission
{
public:
    cCalorMission()noexcept {}

    DISABLE_COPY(cCalorMission)
    ASSIGN_ID(6, "Calor")
};


// ****************************************************************
// Muscus mission class
class cMuscusMission final : public cMission
{
public:
    cMuscusMission()noexcept {}

    DISABLE_COPY(cMuscusMission)
    ASSIGN_ID(7, "Muscus")
};


// ****************************************************************
// Ater mission class
class cAterMission final : public cMission
{
public:
    cAterMission()noexcept {}

    DISABLE_COPY(cAterMission)
    ASSIGN_ID(8, "Ater")
};


// ****************************************************************
// Veneta ??? Mellan Rupes Shor
//class ??? final : public cMission
//{
//public:
//    ???()noexcept {}
//
//    DISABLE_COPY(???)
//    ASSIGN_ID(666, "???")
//};


#endif // _P1_GUARD_MISSION_H_