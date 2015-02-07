//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHIP_H_
#define _P1_GUARD_SHIP_H_


// ****************************************************************
// ship interface
class INTERFACE cShip : public coreObject3D
{
protected:
    coreModelPtr m_pModelLow;   // low-polygon model object (used for depth-only, shadow, outline)

    int m_iMaxHealth;           // 
    int m_iCurHealth;           // 
    coreVector2 m_vNewPos;      // new position for smooth movement and animation


public:
    constexpr_weak cShip()noexcept;
    virtual ~cShip() {}

    DISABLE_COPY(cShip)

    // define the visual appearance
    inline const coreModelPtr& DefineModelLow(const coreModelPtr& pModel) {m_pModelLow = pModel;                                          return m_pModelLow;}
    inline const coreModelPtr& DefineModelLow(const char*         pcName) {m_pModelLow = Core::Manager::Resource->Get<coreModel>(pcName); return m_pModelLow;}

    // render the ship (low-polygon)
    void Render(const coreProgramPtr& pProgram)override;

    // get object properties
    inline const int& GetMaxHealth()const {return m_iMaxHealth;}
    inline const int& GetCurHealth()const {return m_iCurHealth;}
};


// ****************************************************************
// constructor
constexpr_weak cShip::cShip()noexcept
: m_iMaxHealth (0)
, m_iCurHealth (0)
, m_vNewPos    (coreVector2(0.0f,0.0f))
{
}



#endif // _P1_GUARD_SHIP_H_