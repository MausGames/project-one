///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHIP_H_
#define _P1_GUARD_SHIP_H_

// TODO: change high-resolution models with pre-baked normal-maps or tessellation ?
// TODO: reduce ship health and all damage values to 16-bit (also reorder for better packing if possible)
// TODO: do not create explosions if ship is far outside of view-port (just ships or in general special-effects ?)
// TODO: ReachedHealthPct, GetCurHealthPct -> ReachedHealthPct(0.7f) was triggered while interface was showing 71%


// ****************************************************************
// ship definitions
#define SHIP_INVERTED_BIT (24u)   // for inverted base color interpolation

#define SHIP_SHADER_ATTRIBUTE_BLINK (CORE_SHADER_ATTRIBUTE_DIV_TEXPARAM_NUM + 1u)


// ****************************************************************
// ship interface
class INTERFACE cShip : public cLodObject
{
protected:
    coreProtect<coreInt32> m_iMaxHealth;   // maximum health value
    coreProtect<coreInt32> m_iCurHealth;   // current health value
    coreProtect<coreInt32> m_iPreHealth;   // 

    coreVector2 m_vOldPos;                 // 

    coreUint32 m_iBaseColor;               // packed base color for interpolations
    coreFlow   m_fBlink;                   // blink intensity (to highlight successful hits)


protected:
    cShip()noexcept;
    ~cShip()override = default;


public:
    ENABLE_COPY(cShip)

    // transformation functions (raw parameters are multiplied with FOREGROUND_AREA)
    coreBool DefaultMovePath     (const coreSpline2* pRawPath, const coreVector2& vFactor, const coreVector2& vRawOffset, const coreFloat fDistance);
    coreBool DefaultMoveTarget   (const coreVector2& vTarget, const coreFloat fSpeedMove, const coreFloat fSpeedTurn);
    coreBool DefaultMoveSmooth   (const coreVector2& vRawPosition, const coreFloat fSpeedMove, const coreFloat fDistThreshold);
    void     DefaultMoveForward  (const coreVector2& vDirection, const coreFloat fSpeedMove);
    void     DefaultMoveLerp     (const coreVector2& vFromRawPos, const coreVector2& vToRawPos, const coreFloat fTime);
    void     DefaultRotate       (const coreFloat fAngle);
    coreBool DefaultRotateSmooth (const coreVector2& vDirection, const coreFloat fSpeedTurn, const coreFloat fDistThreshold);
    void     DefaultRotateLerp   (const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime);
    void     DefaultOrientate    (const coreFloat fAngle);
    void     DefaultOrientateLerp(const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime);
    void     DefaultAxiate       (const coreFloat fAngle);
    void     DefaultAxiateLerp   (const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime);
    void     DefaultMultiate     (const coreFloat fAngle);
    void     DefaultMultiateLerp (const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime);

    
    inline cShip* InvertX  () {this->SetPosition( this->GetPosition().InvertedX ()); this->SetDirection( this->GetDirection().InvertedX ()); return this;}
    inline cShip* InvertY  () {this->SetPosition( this->GetPosition().InvertedY ()); this->SetDirection( this->GetDirection().InvertedY ()); return this;}
    inline cShip* Rotate90 () {this->SetPosition( this->GetPosition().RotatedZ90()); this->SetDirection( this->GetDirection().RotatedZ90()); return this;}
    inline cShip* Rotate180() {this->SetPosition(-this->GetPosition());              this->SetDirection(-this->GetDirection());              return this;}
    inline cShip* Rotate270() {this->SetPosition(-this->GetPosition().RotatedZ90()); this->SetDirection(-this->GetDirection().RotatedZ90()); return this;}
    inline cShip* MapToAxis(const coreVector2& vAxis) {this->SetPosition(coreVector3(::MapToAxis(this->GetPosition().xy(), vAxis), 0.0f)); this->SetDirection(coreVector3(::MapToAxis(this->GetDirection().xy(), vAxis), 0.0f)); return this;}


    // 
    void SetBaseColor(const coreVector3& vColor, const coreBool bInverted = false);

    // 
    //inline void RefreshColor(const coreFloat fFactor) {const coreFloat fNewFactor = CLAMP((fFactor - 0.2f) * (1.0f/0.8f) * (0.8f/0.6f), 0.0f, 1.0f); this->SetColor3(LERP(COLOR_SHIP_GREY * 0.5f, this->GetBaseColor(), CONTAINS_BIT(m_iBaseColor, SHIP_INVERTED_BIT) ? (1.0f - fNewFactor) : fNewFactor));}
    inline void RefreshColor(const coreFloat fFactor) {const coreFloat fNewFactor = CLAMP((fFactor - 0.2f) * (1.0f/0.8f) * (0.8f/0.6f), 0.0f, 1.0f); this->SetColor3(LERP(COLOR_SHIP_GREY * 1.0f, this->GetBaseColor(), CONTAINS_BIT(m_iBaseColor, SHIP_INVERTED_BIT) ? (1.0f - fNewFactor) : fNewFactor));}
    inline void RefreshColor()                        {this->RefreshColor(this->GetCurHealthPct());}
    inline void InvokeBlink ()                        {m_fBlink = 1.2f;}

    // 
    inline coreBool ReachedHealth   (const coreInt32 iHealth)const    {return InBetween(iHealth,                                    m_iCurHealth, m_iPreHealth);}
    inline coreBool ReachedHealthPct(const coreFloat fHealthPct)const {return InBetween(F_TO_SI(fHealthPct * I_TO_F(m_iMaxHealth)), m_iCurHealth, m_iPreHealth);}
    inline coreBool ReachedDeath    ()const                           {return ((m_iCurHealth == 0) && (m_iPreHealth != 0));}
    inline coreBool WasDamaged      ()const                           {return (m_iCurHealth < m_iPreHealth);}
    inline coreBool WasTeleporting  ()const                           {return (this->GetMove().LengthSq() > (0.125f * FOREGROUND_AREA.x * FOREGROUND_AREA.y));}

    // add or remove status values
    inline void AddStatus   (const coreInt32 iStatus) {ADD_FLAG   (m_iStatus, iStatus)}
    inline void RemoveStatus(const coreInt32 iStatus) {REMOVE_FLAG(m_iStatus, iStatus)}

    // set object properties
    inline void SetMaxHealth   (const coreInt32 iMaxHealth)    {m_iMaxHealth = iMaxHealth;}
    inline void SetCurHealth   (const coreInt32 iCurHealth)    {m_iCurHealth = iCurHealth;}
    inline void SetCurHealthPct(const coreFloat fCurHealthPct) {m_iCurHealth = F_TO_SI(fCurHealthPct * I_TO_F(m_iMaxHealth)); ASSERT((fCurHealthPct >= 0.0f) && (fCurHealthPct <= 1.0f))}

    // get object properties
    inline       coreInt32    GetMaxHealth   ()const {return m_iMaxHealth;}
    inline       coreInt32    GetCurHealth   ()const {return m_iCurHealth;}
    inline       coreFloat    GetCurHealthPct()const {return I_TO_F(m_iCurHealth) * RCP(I_TO_F(m_iMaxHealth));}
    inline       coreInt32    GetPreHealth   ()const {return m_iPreHealth;}
    inline       coreFloat    GetPreHealthPct()const {return I_TO_F(m_iPreHealth) * RCP(I_TO_F(m_iMaxHealth));}
    inline const coreVector2& GetOldPos      ()const {return m_vOldPos;}
    inline       coreVector2  GetMove        ()const {return this->GetPosition().xy() - m_vOldPos;}
    inline       coreVector3  GetBaseColor   ()const {return coreVector4::UnpackUnorm4x8(m_iBaseColor).xyz();}
    inline       coreFloat    GetBlink       ()const {return MIN(m_fBlink, 1.0f);}


protected:
    // 
    coreInt32 _TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2& vImpact);

    // 
    void _Resurrect();
    void _Kill     (const coreBool bAnimated);

    // 
    void _EnableBlink(const coreProgramPtr& pProgram)const;
    void _EnableBlink()const;

    // 
    void _UpdateAlwaysBefore();
    void _UpdateAlwaysAfter();
};


#endif // _P1_GUARD_SHIP_H_