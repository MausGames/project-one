//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_BOSS_H_
#define _P1_GUARD_BOSS_H_


// ****************************************************************
// boss definitions
#define BOSS_TIMERS (4u)   // 


// ****************************************************************
// boss entity interface
class INTERFACE cBoss : public cEnemy
{
protected:
    coreUint8 m_iPhase;                // 
    coreUint8 m_iLevel;                // 

    coreTimer m_aTimer[BOSS_TIMERS];   // 


public:
    cBoss()noexcept;
    virtual ~cBoss() {}

    DISABLE_COPY(cBoss)
    ENABLE_ID

    // 
    inline const coreUint8& GetPhase()const {return m_iPhase;}
    inline const coreUint8& GetLevel()const {return m_iLevel;}
};


// ****************************************************************
// crossfield boss class
class cCrossfieldBoss final : public cBoss
{
public:
    cCrossfieldBoss()noexcept;

    DISABLE_COPY(cCrossfieldBoss)
    ASSIGN_ID(101, "Crossfield")


private:
    // 
    void __RenderOwnBefore()override;
    void __MoveOwn        ()override;
};


#endif // _P1_GUARD_BOSS_H_