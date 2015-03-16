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

// TODO: change high-resolution models with pre-baked normal-maps


// ****************************************************************
// ship interface
class INTERFACE cShip : public coreObject3D
{
protected:
    coreModelPtr m_pModelLow;    // low-polygon model object (used for shadow, outline)
    coreUint32   m_iBaseColor;   // packed base color for interpolations

    coreInt32 m_iMaxHealth;      // maximum health value (armor)
    coreInt32 m_iCurHealth;      // current health value (armor)


protected:
    constexpr_weak cShip()noexcept;
    ~cShip() {}


public:
    DISABLE_COPY(cShip)

    // define the visual appearance
    inline const coreModelPtr& DefineModelLow(const coreModelPtr& pModel) {m_pModelLow = pModel;                                          return m_pModelLow;}
    inline const coreModelPtr& DefineModelLow(const coreChar*     pcName) {m_pModelLow = Core::Manager::Resource->Get<coreModel>(pcName); return m_pModelLow;}

    // render the ship (low-polygon)
    using coreObject3D::Render;
    void Render(const coreProgramPtr& pProgram)override;

    // add or remove status values
    inline void AddStatus   (const coreInt32& iStatus) {ADD_VALUE   (m_iStatus, iStatus)}
    inline void RemoveStatus(const coreInt32& iStatus) {REMOVE_VALUE(m_iStatus, iStatus)}

    // set object properties
    inline void SetBaseColor(const coreVector3& vBaseColor) {m_iBaseColor = coreVector4(vBaseColor, 0.0f).PackUnorm4x8(); this->SetColor3(vBaseColor);}

    // get object properties
    inline coreVector3      GetBaseColor()const {return coreVector4::UnpackUnorm4x8(m_iBaseColor).xyz();}
    inline const coreInt32& GetMaxHealth()const {return m_iMaxHealth;}
    inline const coreInt32& GetCurHealth()const {return m_iCurHealth;}


protected:
    // 
    void _Resurrect(const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32& iType);
    void _Kill     (const coreBool&    bAnimated);
};


// ****************************************************************
// constructor
constexpr_weak cShip::cShip()noexcept
: m_iBaseColor (0)
, m_iMaxHealth (0)
, m_iCurHealth (0)
{
}


#endif // _P1_GUARD_SHIP_H_