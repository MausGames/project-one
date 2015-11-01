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
#define MISSION_BOSSES  (3u)      // default number of bosses per mission
#define MISSION_NO_BOSS (0xFFu)   // no boss currently active (error-value)


// ****************************************************************
// mission specific definitions
#define VIRIDO_BALLS      (2u)                                    // 
#define VIRIDO_TRAILS     (4u)                                    // 
#define VIRIDO_RAWS       (VIRIDO_BALLS * (VIRIDO_TRAILS + 1u))   // 
#define VIRIDO_PADDLES    (PLAYERS + 1u)                          // 
#define VIRIDO_BALL_SPEED (1.5f)                                  // 


// ****************************************************************
// stage management macros
#define STAGE_MAIN            m_aiStageLine.push_back(__LINE__); m_anStage.push_back([this]()           // 
#define STAGE_SUB(t)          ((m_fStageTimeBefore <= (t)) && ((t) < m_fStageTime))                     // 
#define STAGE_FINISH_NOW      {m_fStageTime = 0.0f; m_aiStageLine.pop_back(); m_anStage.pop_back();}    // 
#define STAGE_FINISH_AFTER(t) {if(m_fStageTime >= (t)) STAGE_FINISH_NOW}                                // 
#define STAGE_START_HERE      {if(DEFINED(_CORE_DEBUG_)) {m_aiStageLine.clear(); m_anStage.clear();}}   // 

#define STAGE_ADD_PATH(n) \
    auto n = this->_AddPath(__LINE__, [](coreSpline2* OUTPUT n)

#define STAGE_ADD_ENEMY(n,t,c)                        \
    t* n = g_pGame->GetEnemyManager()->AddEnemy<t>(); \
    n->Resurrect();                                   \
    n->SetBaseColor(c);                               \
    n->ChangeRoutine([=](cEnemy* OUTPUT n)


// ****************************************************************
// mission interface
class INTERFACE cMission
{
private:
    using tStageFunc = std::function<void()>;
    using tPathPtr   = std::shared_ptr<coreSpline2>;


protected:
    cBoss* m_apBoss[MISSION_BOSSES];             // pointers to all available bosses

    cBoss*    m_pCurBoss;                        // pointer to currently active boss
    coreUintW m_iCurBossIndex;                   // index of the active boss (or error-value)

    std::vector<tStageFunc> m_anStage;           // 
    std::vector<coreUint16> m_aiStageLine;       // 
    coreUint16 m_iStageNum;                      // 

    coreLookup<coreUint16, tPathPtr> m_apPath;   // 

    coreFlow  m_fStageTime;                      // 
    coreFloat m_fStageTimeBefore;                // 


public:
    cMission()noexcept;
    virtual ~cMission() {}

    DISABLE_COPY(cMission)
    ENABLE_ID

    // setup the mission
    void Setup();

    // render and move the mission
    void RenderWeak  ();
    void RenderStrong();
    void RenderAfter ();
    void MoveBefore  ();
    void MoveAfter   ();

    // set active boss
    void SetCurBoss(const coreUintW& iIndex);
    void SetCurBoss(const cBoss*     pBoss);

    // access mission objects
    inline cBoss*           GetBoss        (const coreUintW& iIndex)const {ASSERT(iIndex < MISSION_BOSSES) return m_apBoss[iIndex];}
    inline cBoss*           GetCurBoss     ()const                        {return m_pCurBoss;}
    inline const coreUintW& GetCurBossIndex()const                        {return m_iCurBossIndex;}


protected:
    // 
    template <typename F> const tPathPtr& _AddPath(const coreUint16& iIndex, F&& nInitFunc);   // [](coreSpline2* OUTPUT pPath) -> void


private:
    // own routines for derived classes
    virtual void __SetupOwn       () {}
    virtual void __RenderOwnWeak  () {}
    virtual void __RenderOwnStrong() {}
    virtual void __RenderOwnAfter () {}
    virtual void __MoveOwnBefore  () {}
    virtual void __MoveOwnAfter   () {}
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
    cCrossfieldBoss m_Crossfield;                   // 
    cTorusBoss      m_Torus;                        // 
    cVausBoss       m_Vaus;                         // 

    coreBatchList m_Ball;                           // 
    coreBatchList m_BallTrail;                      // 
    coreObject3D  m_aBallRaw[VIRIDO_RAWS];          // 

    coreObject3D m_aPaddle      [VIRIDO_PADDLES];   // 
    coreObject3D m_aPaddleSphere[VIRIDO_PADDLES];   // 
    cShip*       m_apOwner      [VIRIDO_PADDLES];   // 

    coreUint8 m_iBounceState;                       // 
    coreBool  m_bBounceReal;                        // 

    coreLabel m_Overlay;                            //      
    coreFloat m_fOverlayTime;                       //      

    coreFlow m_fAnimation;                          // animation value


public:
    cViridoMission()noexcept;
    ~cViridoMission();

    DISABLE_COPY(cViridoMission)
    ASSIGN_ID(1, "Virido")

    // 
    void EnableBall (const coreUintW& iIndex, const coreVector2& vPosition, const coreVector2& vDirection);
    void DisableBall(const coreUintW& iIndex, const coreBool& bAnimated);

    // 
    void EnablePaddle (const coreUintW& iIndex, cShip* pOwner);
    void DisablePaddle(const coreUintW& iIndex, const coreBool& bAnimated);

    // 
    inline void     SetBounceReal (const coreBool&  bStatus)     {m_bBounceReal = bStatus;}
    inline coreBool GetBounceState(const coreUintW& iIndex)const {ASSERT(iIndex < sizeof(m_iBounceState)*8u) return CONTAINS_BIT(m_iBounceState, iIndex);}

    // 
    inline coreObject3D* GetBall  (const coreUintW& iIndex) {ASSERT(iIndex < VIRIDO_BALLS)   return &m_aBallRaw[iIndex * (VIRIDO_TRAILS + 1u)];}
    inline coreObject3D* GetPaddle(const coreUintW& iIndex) {ASSERT(iIndex < VIRIDO_PADDLES) return &m_aPaddle [iIndex];}


private:
    // execute own routines
    void __SetupOwn       ()override;
    void __RenderOwnWeak  ()override;
    void __RenderOwnStrong()override;
    void __RenderOwnAfter ()override;
    void __MoveOwnAfter   ()override;
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
// 
template <typename F> const cMission::tPathPtr& cMission::_AddPath(const coreUint16& iIndex, F&& nInitFunc)
{
    if(!m_apPath.count(iIndex))
    {
        // 
        tPathPtr pNewPath = std::make_shared<coreSpline2>();
        nInitFunc(pNewPath.get());

        // 
        m_apPath[iIndex] = std::move(pNewPath);
    }

    return m_apPath.at(iIndex);
}


#endif // _P1_GUARD_MISSION_H_