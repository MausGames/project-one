///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_HELPER_H_
#define _P1_GUARD_HELPER_H_


// ****************************************************************
// helper definitions
enum eHelperStatus : coreUint8
{
    HELPER_STATUS_DEAD   = 0x01u,   // completely removed from the game
    HELPER_STATUS_HIDDEN = 0x02u,   // 
    HELPER_STATUS_BOTTOM = 0x04u,   // 
    HELPER_STATUS_TOP    = 0x08u    // 
};


// ****************************************************************
// helper entity class
class cHelper final : public cShip
{
private:
    coreFlow  m_fLifeTime;             // 
    coreFloat m_fLifeTimeBefore;       // 
    coreBool  m_bSmooth;               // 

    coreFloat m_fAngleOverride;        // 

    coreUint8 m_iElement;              // 

    coreObject3D  m_Wind;              // 
    coreObject3D* m_pShield;           // (dynamic) 

    static coreFlow    s_fAnimation;   // 
    static coreVector2 s_vDirection;   // 


public:
    cHelper()noexcept;
    ~cHelper()final;

    DISABLE_COPY(cHelper)

    // configure the helper
    void Configure(const coreUint8 iElement);

    // render and move the helper
    void Render      ()final;
    void RenderBefore();
    void RenderAfter ();
    void Move        ()final;

    // control life and death
    void Resurrect(const coreBool bSmooth);
    void Kill     (const coreBool bAnimated);

    // 
    void EnableShield();
    void DisableShield(const coreBool bAnimated);

    // 
    inline coreObject3D* GetShield()const {return m_pShield;}
    
    inline void SetPosition(const coreVector3 vPosition) {this->coreObject3D::SetPosition(vPosition); m_Wind.SetPosition(vPosition); if(m_pShield) m_pShield->SetPosition(vPosition);}
    
    inline void SetAngle(const coreFloat fAngle) {m_fAngleOverride = fAngle;}

    // get object properties
    inline const coreFloat& GetLifeTime      ()const {return m_fLifeTime;}
    inline const coreFloat& GetLifeTimeBefore()const {return m_fLifeTimeBefore;}

    // 
    static void GlobalUpdate();
    static void GlobalReset();
};


#endif // _P1_GUARD_HELPER_H_