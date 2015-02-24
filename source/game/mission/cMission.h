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
#define MISSION_NO_BOSS (0xFF)   // 


// ****************************************************************
// mission interface
class INTERFACE cMission
{
protected:
    cBoss* m_apBoss[MISSION_BOSSES];   // 

    cBoss*   m_pCurBoss;               // 
    coreByte m_iCurBossNum;            // 


public:
    cMission()noexcept;
    virtual ~cMission() {}

    DISABLE_COPY(cMission)
    ENABLE_ID

    // access mission objects
    inline cBoss*          GetBoss      (const coreByte& iIndex)const {ASSERT (iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*          GetCurBoss   ()const                       {return m_pCurBoss;}
    inline const coreByte& GetCurBossNum()const                       {return m_iCurBossNum;}


protected:
    // 
    void _SetCurBoss(const coreByte& iIndex);
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
// Mellan mission class
class cMellanMission final : public cMission
{
private:
    cCrossfieldBoss m_Crossfield;   // 
    cScoutEnemy     m_aScout[10];   // 


public:
    cMellanMission()noexcept;

    DISABLE_COPY(cMellanMission)
    ASSIGN_ID(1, "Mellan")
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
// Veneta ??? Virido Rupes Shor Ater
//class ??? final : public cMission
//{
//public:
//    ???()noexcept {}
//
//    DISABLE_COPY(???)
//    ASSIGN_ID(666, "???")
//};


#endif // _P1_GUARD_MISSION_H_