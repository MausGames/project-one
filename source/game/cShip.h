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
// TODO: reduce ship health and all damage values to 16-bit (also reorder for better packing if possible)
// TODO: do not create explosions if ship is far outside of view-port (just ships or in general special-effects ?)


// ****************************************************************
// ship definitions
#define SHIP_SHADER_ATTRIBUTE_BLINK (CORE_SHADER_ATTRIBUTE_DIV_TEXPARAM_NUM + 1u)


// ****************************************************************
// ship interface
class INTERFACE cShip : public coreObject3D
{
protected:
    coreModelPtr m_pModelHigh;                // high-polygon model objects (used for default shading)
    coreModelPtr m_pModelLow;                 // low-polygon model object (used for shadow, outline)
    coreUint32   m_iBaseColor;                // packed base color for interpolations

    coreProtect<coreInt32> m_iMaxHealth;      // maximum health value (armor)
    coreProtect<coreInt32> m_iCurHealth;      // current health value (armor)
    coreProtect<coreInt32> m_iPreHealth;      // 

    coreVector2 m_vOldPos;                    // 

    coreFloat m_fBlink;                       // blink intensity (to highlight successful hits)


protected:
    cShip()noexcept;
    ~cShip()override = default;


public:
    ENABLE_COPY(cShip)

    // define the visual appearance
    inline void DefineModelHigh(std::nullptr_t)               {m_pModelHigh = NULL;                                           this->DefineModel(m_pModelHigh);}
    inline void DefineModelHigh(const coreModelPtr&   pModel) {m_pModelHigh = pModel;                                         this->DefineModel(m_pModelHigh);}
    inline void DefineModelHigh(const coreHashString& sName)  {m_pModelHigh = Core::Manager::Resource->Get<coreModel>(sName); this->DefineModel(m_pModelHigh);}
    inline void DefineModelLow (std::nullptr_t)               {m_pModelLow  = NULL;}
    inline void DefineModelLow (const coreModelPtr&   pModel) {m_pModelLow  = pModel;}
    inline void DefineModelLow (const coreHashString& sName)  {m_pModelLow  = Core::Manager::Resource->Get<coreModel>(sName);}

    // render the ship (low-polygon only)
    using coreObject3D::Render;
    void Render(const coreProgramPtr& pProgram)final;

    // 
    inline void ActivateModelDefault() {this->DefineModel(m_pModelHigh);}
    inline void ActivateModelLowOnly() {this->DefineModel(m_pModelLow);}

    // 
    inline coreBool ReachedHealth   (const coreInt32 iHealth)    {return InBetween(iHealth,                                    m_iCurHealth, m_iPreHealth);}
    inline coreBool ReachedHealthPct(const coreFloat fHealthPct) {return InBetween(F_TO_SI(fHealthPct * I_TO_F(m_iMaxHealth)), m_iCurHealth, m_iPreHealth);}
    inline coreBool ReachedDeath    ()                           {return ((m_iCurHealth == 0) && (m_iPreHealth != 0));}

    // add or remove status values
    inline void AddStatus   (const coreInt32 iStatus) {ADD_FLAG   (m_iStatus, iStatus)}
    inline void RemoveStatus(const coreInt32 iStatus) {REMOVE_FLAG(m_iStatus, iStatus)}

    // set object properties
    inline void SetBaseColor   (const coreVector3& vBaseColor)    {m_iBaseColor = coreVector4(vBaseColor, 0.0f).PackUnorm4x8(); this->SetColor3(vBaseColor);}
    inline void SetMaxHealth   (const coreInt32    iMaxHealth)    {m_iMaxHealth = iMaxHealth;}
    inline void SetCurHealth   (const coreInt32    iCurHealth)    {m_iCurHealth = iCurHealth;}
    inline void GetCurHealthPct(const coreFloat    fCurHealthPct) {m_iCurHealth = F_TO_SI(fCurHealthPct * I_TO_F(m_iMaxHealth)); ASSERT((fCurHealthPct > 0.0f) && (fCurHealthPct <= 1.0f))}

    // get object properties
    inline const coreModelPtr& GetModelHigh   ()const {return m_pModelHigh;}
    inline const coreModelPtr& GetModelLow    ()const {return m_pModelLow;}
    inline       coreVector3   GetBaseColor   ()const {return coreVector4::UnpackUnorm4x8(m_iBaseColor).xyz();}
    inline       coreInt32     GetMaxHealth   ()const {return m_iMaxHealth;}
    inline       coreInt32     GetCurHealth   ()const {return m_iCurHealth;}
    inline       coreFloat     GetCurHealthPct()const {return I_TO_F(m_iCurHealth) * RCP(I_TO_F(m_iMaxHealth));}
    inline const coreVector2&  GetOldPos      ()const {return m_vOldPos;}
    inline       coreFloat     GetBlink       ()const {return MIN(m_fBlink, 1.0f);}


protected:
    // 
    coreBool _TakeDamage(const coreInt32 iDamage, const coreUint8 iElement);

    // 
    void _Resurrect(const coreBool bSingle, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32 iType);
    void _Kill     (const coreBool bSingle, const coreBool     bAnimated);

    // 
    void _EnableBlink();

    // 
    void _UpdateAlwaysBefore();
    void _UpdateAlwaysAfter();
};


#endif // _P1_GUARD_SHIP_H_