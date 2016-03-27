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
// ship definitions
#define SHIP_SHADER_ATTRIBUTE_BLINK (CORE_SHADER_ATTRIBUTE_DIV_TEXPARAM_NUM + 1u)


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
    coreInt32 m_iPreHealth;      // 

    coreFloat m_fBlink;          // blink intensity (to highlight successful hits)


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
    void Render(const coreProgramPtr& pProgram)override;

    // 
    inline bool ReachedHealth   (const coreInt32 iHealth)    {this->__ReachOnce(); return InBetween(iHealth,                                    m_iCurHealth, ABS(m_iPreHealth));}
    inline bool ReachedHealthPct(const coreFloat fHealthPct) {this->__ReachOnce(); return InBetween(F_TO_SI(fHealthPct * I_TO_F(m_iMaxHealth)), m_iCurHealth, ABS(m_iPreHealth));}
    inline bool ReachedDeath    ()                           {this->__ReachOnce(); return ((m_iCurHealth == 0) && (m_iPreHealth != 0));}

    // add or remove status values
    inline void AddStatus   (const coreInt32 iStatus) {ADD_VALUE   (m_iStatus, iStatus)}
    inline void RemoveStatus(const coreInt32 iStatus) {REMOVE_VALUE(m_iStatus, iStatus)}

    // set object properties
    inline void SetMaxHealth(const coreInt32    iMaxHealth) {m_iMaxHealth = iMaxHealth;}
    inline void SetBaseColor(const coreVector3& vBaseColor) {m_iBaseColor = coreVector4(vBaseColor, 0.0f).PackUnorm4x8(); this->SetColor3(vBaseColor);}

    // get object properties
    inline const coreModelPtr& GetModelHigh   ()const {return m_pModelHigh;}
    inline const coreModelPtr& GetModelLow    ()const {return m_pModelLow;}
    inline       coreVector3   GetBaseColor   ()const {return coreVector4::UnpackUnorm4x8(m_iBaseColor).xyz();}
    inline const coreInt32&    GetMaxHealth   ()const {return m_iMaxHealth;}
    inline const coreInt32&    GetCurHealth   ()const {return m_iCurHealth;}
    inline       coreFloat     GetCurHealthPct()const {return I_TO_F(m_iCurHealth) * RCP(I_TO_F(m_iMaxHealth));}
    inline       coreFloat     GetBlink       ()const {return MIN(m_fBlink, 1.0f);}


protected:
    // 
    coreBool _TakeDamage(const coreInt32 iDamage, const coreUint8 iElement);

    // 
    void _Resurrect(const coreBool bSingle, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32 iType);
    void _Kill     (const coreBool bSingle, const coreBool     bAnimated);

    // 
    void _UpdateBlink();
    void _EnableBlink();

    // 
    void _UpdateAlways();


private:
    // 
    inline void __ReachOnce() {if((m_iCurHealth == 0) && (m_iPreHealth > 0)) m_iPreHealth = -m_iPreHealth;}
};


#endif // _P1_GUARD_SHIP_H_