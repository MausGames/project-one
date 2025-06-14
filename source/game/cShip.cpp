///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

cRotaCache cShip::s_RotaCache = cRotaCache();


// ****************************************************************
// constructor
cShip::cShip()noexcept
: m_iMaxHealth (0)
, m_iCurHealth (0)
, m_iPreHealth (0)
, m_vOldPos    (coreVector2(0.0f,0.0f))
, m_iBaseColor (0u)
, m_fBlink     (0.0f)
{
    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cShip::SetBaseColor(const coreVector3 vColor, const coreBool bInverted, const coreBool bIgnored, const coreBool bWhite)
{
    // 
    m_iBaseColor = coreVector4(vColor, 0.0f).PackUnorm4x8();
    if(bInverted) ADD_BIT(m_iBaseColor, SHIP_INVERTED_BIT)
    if(bIgnored)  ADD_BIT(m_iBaseColor, SHIP_IGNORED_BIT)
    if(bWhite)    ADD_BIT(m_iBaseColor, SHIP_WHITE_BIT)

    // 
    this->SetColor3(vColor);
    if(m_iCurHealth) this->RefreshColor();
}


// ****************************************************************
// 
coreBool cShip::DefaultMovePath(const coreSpline2* pRawPath, const coreVector2 vFactor, const coreVector2 vRawOffset, const coreFloat fDistance)
{
    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    pRawPath->CalcPosDir(CLAMP(fDistance, 0.0f, pRawPath->GetTotalDistance()), &vPosition, &vDirection);

    // (factor-variable can also change speed along only one axis) 
    this->SetPosition (coreVector3(((vPosition  * vFactor) + vRawOffset) * FOREGROUND_AREA, 0.0f));
    this->SetDirection(coreVector3( (vDirection * vFactor).Normalized(vDirection),          0.0f));

    // 
    return (fDistance >= pRawPath->GetTotalDistance());
}


// ****************************************************************
// 
coreBool cShip::DefaultMoveTarget(const coreVector2 vTarget, const coreFloat fSpeedMove, const coreFloat fSpeedTurn)
{
    ASSERT((fSpeedMove >= 0.0f) && (fSpeedTurn >= 0.0f))

    // 
    const coreVector2 vDiff = vTarget - this->GetPosition().xy();
    if(vDiff.IsNull()) return true;

    // 
    const coreVector2 vAim = vDiff.Normalized();
    const coreVector2 vDir = SmoothAim(this->GetDirection().xy(), vAim, fSpeedTurn);
    const coreVector2 vPos = this->GetPosition().xy() + vDir * (fSpeedMove * TIME);

    // 
    this->SetPosition (coreVector3(vPos, 0.0f));
    this->SetDirection(coreVector3(vDir, 0.0f));

    // 
    return (vDiff.LengthSq() < POW2(0.7f));
}


// ****************************************************************
// 
coreBool cShip::DefaultMoveSmooth(const coreVector2 vRawPosition, const coreFloat fSpeedMove, const coreFloat fDistThreshold)
{
    ASSERT((fSpeedMove >= 0.0f) && (fDistThreshold >= 0.0f))

    // 
    const coreVector2 vDiff = vRawPosition * FOREGROUND_AREA - this->GetPosition().xy();
    if(vDiff.IsNull()) return true;

    // 
    const coreVector2 vAim = vDiff.Normalized();
    const coreFloat   fLen = SmoothTowards(vDiff.Length(), fDistThreshold);
    const coreVector2 vPos = this->GetPosition().xy() + vAim * (fLen * fSpeedMove * TIME);

    // 
    this->SetPosition(coreVector3(vPos, 0.0f));

    // 
    return (fLen < 0.1f);
}


// ****************************************************************
// 
void cShip::DefaultMoveForward(const coreVector2 vDirection, const coreFloat fSpeedMove)
{
    // 
    const coreVector2 vPosition = this->GetPosition().xy() + vDirection * (fSpeedMove * TIME);

    // 
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));
}


// ****************************************************************
// 
void cShip::DefaultMoveLerp(const coreVector2 vFromRawPos, const coreVector2 vToRawPos, const coreFloat fTime)
{
    // 
    this->SetPosition(coreVector3(LERP(vFromRawPos, vToRawPos, fTime) * FOREGROUND_AREA, 0.0f));
}


// ****************************************************************
// 
void cShip::DefaultRotate(const coreFloat fAngle)
{
    // rotate around z-axis
    const coreVector2 vDir = s_RotaCache.Direction(fAngle);
    this->SetDirection(coreVector3(vDir, 0.0f));
}


// ****************************************************************
// 
coreBool cShip::DefaultRotateSmooth(const coreVector2 vDirection, const coreFloat fSpeedTurn, const coreFloat fDistThreshold)
{
    ASSERT(vDirection.IsNormalized() && (fSpeedTurn >= 0.0f) && (fDistThreshold >= 0.0f))

    // 
    const coreVector2 vDiff = vDirection - this->GetDirection().xy();
    if(vDiff.IsNull()) return true;

    // 
    const coreVector2 vAim = vDiff.Normalized();
    const coreFloat   fLen = SmoothTowards(vDiff.Length(), fDistThreshold);
    const coreVector2 vDir = SmoothAim(this->GetDirection().xy(), vAim, fLen * fSpeedTurn);

    // 
    this->SetDirection(coreVector3(vDir, 0.0f));

    // 
    return (fLen < 0.1f);
}


// ****************************************************************
// 
void cShip::DefaultRotateLerp(const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime)
{
    // rotate around z-axis
    this->DefaultRotate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
void cShip::DefaultOrientate(const coreFloat fAngle)
{
    // rotate around direction axis
    const coreVector2 vDir = this->GetDirection().xy();
    const coreVector2 vOri = s_RotaCache.Direction(fAngle);
    this->SetOrientation(OriRoundDir(vOri, vDir));
}


// ****************************************************************
// 
void cShip::DefaultOrientateLerp(const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime)
{
    // rotate around direction axis
    this->DefaultOrientate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
void cShip::DefaultAxiate(const coreFloat fAngle, const coreVector3 vBaseOri)
{
    // 
    const coreVector3 vDir  = this->GetDirection();
    const coreVector3 vOri  = coreVector3::Cross(vDir, coreVector3::Cross(vDir, vBaseOri).Normalized());
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(fAngle, vDir).m123();
    this->SetOrientation(vOri * mRota);
}


// ****************************************************************
// 
void cShip::DefaultAxiateLerp(const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime)
{
    // 
    this->DefaultAxiate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
void cShip::DefaultMultiate(const coreFloat fAngle)
{
    // rotate around the rotating direction axis
    const coreVector2 vDir = s_RotaCache.Direction(fAngle);
    this->SetDirection  (coreVector3(vDir, 0.0f));
    this->SetOrientation(OriRoundDir(vDir, vDir));
}


// ****************************************************************
// 
void cShip::DefaultMultiateLerp(const coreFloat fFromAngle, const coreFloat fToAngle, const coreFloat fTime)
{
    // 
    this->DefaultMultiate(LERP(fFromAngle, fToAngle, fTime));
}


// ****************************************************************
// 
coreInt32 cShip::_TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2 vImpact)
{
    // 
    const coreInt32 iHealthDamage = CLAMP(iDamage, m_iCurHealth - m_iMaxHealth, m_iCurHealth);
    m_iCurHealth -= iHealthDamage;

    // 
    return iHealthDamage;
}


// ****************************************************************
// add ship to the game
void cShip::_Resurrect()
{
    ASSERT(m_iMaxHealth > 0)

    // reset ship properties
    m_iCurHealth = m_iMaxHealth;
    m_iPreHealth = m_iMaxHealth;
    m_vOldPos    = HIDDEN_POS;   // # for consistency
    this->RefreshColor(1.0f);

    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// remove ship from the game
void cShip::_Kill(const coreBool bAnimated)
{
    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// 
void cShip::_EnableBlink(const coreProgramPtr& pProgram)const
{
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable()) return;

    // 
    pProgram->SendUniform("u_v1Blink", this->GetBlink());
}

void cShip::_EnableBlink()const
{
    // 
    this->_EnableBlink(this->GetProgram());
}


// ****************************************************************
// 
void cShip::_UpdateAlwaysBefore()
{
    // 
    m_vOldPos = this->GetPosition().xy();   // old frozen
}


// ****************************************************************
// 
void cShip::_UpdateAlwaysAfter()
{
    // 
    m_iPreHealth = m_iCurHealth;

    // 
    if(g_MenuInput.bScreenshot) m_fBlink = 0.0f;
    m_fBlink.UpdateMax(g_CurConfig.Graphics.iFlash ? -10.0f : -2.0f, 0.0f);
}