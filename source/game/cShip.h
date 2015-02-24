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
    coreUint m_iBaseColor;      // packed base color for interpolations

    int m_iMaxHealth;           // maximum health value (armor)
    int m_iCurHealth;           // current health value (armor)
    coreVector2 m_vNewPos;      // new position for smooth movement and animation


protected:
    constexpr_weak cShip()noexcept;
    ~cShip() {}


public:
    DISABLE_COPY(cShip)

    // define the visual appearance
    inline const coreModelPtr& DefineModelLow(const coreModelPtr& pModel) {m_pModelLow = pModel;                                          return m_pModelLow;}
    inline const coreModelPtr& DefineModelLow(const char*         pcName) {m_pModelLow = Core::Manager::Resource->Get<coreModel>(pcName); return m_pModelLow;}

    // render the ship (low-polygon)
    using coreObject3D::Render;
    void Render(const coreProgramPtr& pProgram)override;

    // set object properties
    inline void SetBaseColor(const coreVector3& vBaseColor) {m_iBaseColor = coreVector4(vBaseColor, 0.0f).PackUnorm4x8();}

    // get object properties
    inline coreVector3        GetBaseColor()const {return coreVector4::UnpackUnorm4x8(m_iBaseColor).xyz();}
    inline const int&         GetMaxHealth()const {return m_iMaxHealth;}
    inline const int&         GetCurHealth()const {return m_iCurHealth;}
    inline const coreVector2& GetNewPos   ()const {return m_vNewPos;}


protected:
    // 
    void _Resurrect(const coreVector2& vPosition, const int& iType);
    void _Kill     (const bool&        bAnimated);
};


// ****************************************************************
// constructor
constexpr_weak cShip::cShip()noexcept
: m_iBaseColor (0)
, m_iMaxHealth (0)
, m_iCurHealth (0)
, m_vNewPos    (coreVector2(0.0f,0.0f))
{
}


#endif // _P1_GUARD_SHIP_H_