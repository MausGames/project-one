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

// TODO: change high-resolution models with pre-baked normal-maps or tessellation ?


// ****************************************************************
// ship interface
class INTERFACE cShip : public coreObject3D
{
protected:
    coreModelPtr m_pModelHigh;   // high-polygon model objects (used for default shading)
    coreModelPtr m_pModelLow;    // low-polygon model object (used for shadow, outline)
    coreUint32   m_iBaseColor;   // packed base color for interpolations

    coreInt32 m_iMaxHealth;      // maximum health value (armor)
    coreInt32 m_iCurHealth;      // current health value (armor)

    coreFloat m_fBlink;          // blink intensity (to highlight successful hits)


protected:
    cShip()noexcept;
    ~cShip() {}


public:
    ENABLE_COPY(cShip)

    // define the visual appearance
    inline const coreModelPtr& DefineModelHigh(const coreModelPtr& pModel) {m_pModelHigh = pModel;                                          this->DefineModel(m_pModelHigh); return m_pModelLow;}
    inline const coreModelPtr& DefineModelHigh(const coreChar*     pcName) {m_pModelHigh = Core::Manager::Resource->Get<coreModel>(pcName); this->DefineModel(m_pModelHigh); return m_pModelLow;}
    inline const coreModelPtr& DefineModelLow (const coreModelPtr& pModel) {m_pModelLow  = pModel;                                          return m_pModelLow;}
    inline const coreModelPtr& DefineModelLow (const coreChar*     pcName) {m_pModelLow  = Core::Manager::Resource->Get<coreModel>(pcName); return m_pModelLow;}

    // render the ship (low-polygon only)
    using coreObject3D::Render;
    void Render(const coreProgramPtr& pProgram)override;

    // add or remove status values
    inline void AddStatus   (const coreInt32& iStatus) {ADD_VALUE   (m_iStatus, iStatus)}
    inline void RemoveStatus(const coreInt32& iStatus) {REMOVE_VALUE(m_iStatus, iStatus)}

    // set object properties
    inline void SetBaseColor(const coreVector3& vBaseColor) {m_iBaseColor = coreVector4(vBaseColor, 0.0f).PackUnorm4x8(); this->SetColor3(vBaseColor);}

    // get object properties
    inline const coreModelPtr& GetModelHigh   ()const {return m_pModelHigh;}
    inline const coreModelPtr& GetModelLow    ()const {return m_pModelLow;}
    inline       coreVector3   GetBaseColor   ()const {return coreVector4::UnpackUnorm4x8(m_iBaseColor).xyz();}
    inline const coreInt32&    GetMaxHealth   ()const {return m_iMaxHealth;}
    inline const coreInt32&    GetCurHealth   ()const {return m_iCurHealth;}
    inline const coreFloat     GetCurHealthPct()const {return I_TO_F(m_iCurHealth) * RCP(I_TO_F(m_iMaxHealth));}


protected:
    // 
    coreBool _TakeDamage(const coreInt32& iDamage);

    // 
    void _Resurrect(const coreBool& bSingle, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32& iType);
    void _Kill     (const coreBool& bSingle, const coreBool&    bAnimated);

    // 
    void _UpdateBlink();
};


#endif // _P1_GUARD_SHIP_H_