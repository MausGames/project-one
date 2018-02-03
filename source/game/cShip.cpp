//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cShip::cShip()noexcept
: m_iBaseColor (0u)
, m_iMaxHealth (0)
, m_iCurHealth (0)
, m_iPreHealth (0)
, m_vOldPos    (coreVector2(0.0f,0.0f))
, m_fBlink     (0.0f)
{
    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// render the ship (low-polygon only)
void cShip::Render(const coreProgramPtr& pProgram)
{
    if(!this->IsEnabled(CORE_OBJECT_ENABLE_RENDER)) return;

    // check for model status
    ASSERT(m_pModelLow)
    if(!m_pModelLow.IsUsable()) return;

    // enable the shader-program
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // update all object uniforms
    coreProgram* pLocal = pProgram.GetResource();
    pLocal->SendUniform(CORE_SHADER_UNIFORM_3D_POSITION, this->GetPosition());
    pLocal->SendUniform(CORE_SHADER_UNIFORM_3D_SIZE,     this->GetSize());
    pLocal->SendUniform(CORE_SHADER_UNIFORM_3D_ROTATION, m_vRotation);
    pLocal->SendUniform(CORE_SHADER_UNIFORM_COLOR,       m_vColor);   // # alpha

    // draw the model
    m_pModelLow->Enable();
    m_pModelLow->Draw();
}


// ****************************************************************
// 
coreBool cShip::DefaultMovePath(const coreSpline2* pRawPath, const coreVector2& vFactor, const coreVector2& vRawOffset, const coreFloat fDistance)
{
    // 
    coreVector2 vPosition;
    coreVector2 vDirection;
    pRawPath->CalcPosDir(MAX(fDistance, 0.0f), &vPosition, &vDirection);

    // 
    this->SetPosition (coreVector3(((vPosition  * vFactor) + vRawOffset) * FOREGROUND_AREA, 0.0f));
    this->SetDirection(coreVector3( (vDirection * vFactor).Normalized(),                    0.0f));

    // 
    return (fDistance >= pRawPath->GetTotalDistance()) ? true : false;
}


// ****************************************************************
// 
coreBool cShip::DefaultMoveTarget(const coreVector2& vTarget, const coreFloat fSpeedMove, const coreFloat fSpeedTurn)
{
    ASSERT((fSpeedMove >= 0.0f) && (fSpeedTurn >= 0.0f))

    // 
    const coreVector2 vDiff = vTarget - this->GetPosition().xy();
    if(vDiff.IsNull()) return true;

    // 
    const coreVector2 vAim = vDiff.Normalized();
    const coreVector2 vDir = (this->GetDirection().xy() + vAim * (fSpeedTurn * Core::System->GetTime())).Normalized();
    const coreVector2 vPos =  this->GetPosition ().xy() + vDir * (fSpeedMove * Core::System->GetTime());

    // 
    this->SetPosition (coreVector3(vPos, 0.0f));
    this->SetDirection(coreVector3(vDir, 0.0f));

    // 
    return (vDiff.LengthSq() < 0.5f) ? true : false;
}


// ****************************************************************
// 
coreBool cShip::DefaultMoveSmooth(const coreVector2& vRawPosition, const coreFloat fSpeedMove, const coreFloat fDistThreshold)
{
    ASSERT((fSpeedMove >= 0.0f) && (fDistThreshold >= 0.0f))

    // 
    const coreVector2 vDiff = vRawPosition * FOREGROUND_AREA - this->GetPosition().xy();
    if(vDiff.IsNull()) return true;

    // 
    const coreVector2 vAim = vDiff.Normalized();
    const coreFloat   fLen = SmoothTowards(vDiff.Length(), fDistThreshold);
    const coreVector2 vPos = this->GetPosition().xy() + vAim * (fLen * fSpeedMove * Core::System->GetTime());

    // 
    this->SetPosition(coreVector3(vPos, 0.0f));

    // 
    return (fLen < 0.1f) ? true : false;
}


// ****************************************************************
// 
void cShip::DefaultMoveForward(const coreVector2& vDirection, const coreFloat fSpeedMove)
{
    // 
    const coreVector2 vPosition = this->GetPosition().xy() + vDirection * (fSpeedMove * Core::System->GetTime());

    // 
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));
}


// ****************************************************************
// 
void cShip::DefaultMoveLerp(const coreVector2& vFromRawPos, const coreVector2& vToRawPos, const coreFloat fTime)
{
    // 
    this->SetPosition(coreVector3(LERP(vFromRawPos, vToRawPos, fTime) * FOREGROUND_AREA, 0.0f));
}


// ****************************************************************
// 
void cShip::DefaultRotate(const coreFloat fAngle)
{
    // rotate around z-axis
    const coreVector2 vDir = coreVector2::Direction(fAngle);
    this->SetDirection(coreVector3(vDir, 0.0f));
}


// ****************************************************************
// 
coreBool cShip::DefaultRotateSmooth(const coreVector2& vDirection, const coreFloat fSpeedTurn, const coreFloat fDistThreshold)
{
    ASSERT(vDirection.IsNormalized() && (fSpeedTurn >= 0.0f) && (fDistThreshold >= 0.0f))

    // 
    const coreVector2 vDiff = vDirection - this->GetDirection().xy();
    if(vDiff.IsNull()) return true;

    // 
    const coreVector2 vAim = vDiff.Normalized();
    const coreFloat   fLen = SmoothTowards(vDiff.Length(), fDistThreshold);
    const coreVector2 vDir = (this->GetDirection().xy() + vAim * (fLen * fSpeedTurn * Core::System->GetTime())).Normalized();

    // 
    this->SetDirection(coreVector3(vDir, 0.0f));

    // 
    return (fLen < 0.1f) ? true : false;
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
    const coreVector3& vDir = this->GetDirection();
    const coreVector2  vOri = coreVector2::Direction(fAngle);
    this->SetOrientation(coreVector3(-vOri.x*vDir.y, vOri.x*vDir.x, vOri.y));
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
void cShip::DefaultAxiate(const coreFloat fAngle)
{
    // 
    const coreVector3& vDir  = this->GetDirection();
    const coreVector3  vOri  = coreVector3::Cross(vDir, coreVector3::Cross(vDir, coreVector3(0.0f,0.0f,1.0f)).Normalized());
    const coreMatrix3  mRota = coreMatrix4::RotationAxis(fAngle, vDir).m123();
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
    const coreVector2 vDir = coreVector2::Direction(fAngle);
    this->SetDirection  (coreVector3(vDir, 0.0f));
    this->SetOrientation(coreVector3(-vDir.x*vDir.y, vDir.x*vDir.x, vDir.y));
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
coreBool cShip::_TakeDamage(const coreInt32 iDamage, const coreUint8 iElement)
{
    // 
    if((m_iCurHealth -= iDamage) <= 0)
        m_iCurHealth = 0;

    // 
    this->SetColor3(LERP(coreVector3(0.5f,0.5f,0.5f), this->GetBaseColor(), 1.25f * MIN(this->GetCurHealthPct(), 0.8f)));

    // 
    if(iDamage) m_fBlink = 1.2f;

    // 
    return m_iCurHealth ? false : true;
}


// ****************************************************************
// add ship to the game
void cShip::_Resurrect(const coreBool bSingle, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32 iType)
{
    ASSERT(m_iMaxHealth > 0)

    // reset ship properties
    m_iCurHealth = m_iMaxHealth;
    m_iPreHealth = m_iMaxHealth;
    m_vOldPos    = vPosition;
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));
    this->SetColor3   (this->GetBaseColor());

    // add ship to global shadow and outline
    if(bSingle)
    {
        cShadow::GetGlobalContainer()->BindObject(this);
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);
    }

    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // enable collision
    if(iType) this->ChangeType(iType);
}


// ****************************************************************
// remove ship from the game
void cShip::_Kill(const coreBool bSingle, const coreBool bAnimated)
{
    // remove ship from global shadow and outline
    if(bSingle)
    {
        cShadow::GetGlobalContainer()->UnbindObject(this);
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindObject(this);
    }

    // 
    this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // disable collision
    this->ChangeType(0);
}


// ****************************************************************
// 
void cShip::_EnableBlink()
{
    if(!this->GetProgram().IsUsable()) return;

    // 
    this->GetProgram()->Enable();
    this->GetProgram()->SendUniform("u_v1Blink", this->GetBlink());
}


// ****************************************************************
// 
void cShip::_UpdateAlwaysBefore()
{
    // 
    m_vOldPos = this->GetPosition().xy();
}


// ****************************************************************
// 
void cShip::_UpdateAlwaysAfter()
{
    // 
    m_iPreHealth = m_iCurHealth;

    // 
    if(g_MenuInput.bScreenshot)
        m_fBlink = 0.0f;

    // 
    m_fBlink = MAX(m_fBlink - 15.0f*Core::System->GetTime(), 0.0f);
}