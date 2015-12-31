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
    if(!m_pModelLow.IsUsable()) return;

    // enable the shader-program
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // update all object uniforms
    pProgram->SendUniform(CORE_SHADER_UNIFORM_3D_POSITION, this->GetPosition());
    pProgram->SendUniform(CORE_SHADER_UNIFORM_3D_SIZE,     this->GetSize());
    pProgram->SendUniform(CORE_SHADER_UNIFORM_3D_ROTATION, m_vRotation);
    pProgram->SendUniform(CORE_SHADER_UNIFORM_COLOR,       m_vColor);   // # alpha

    // draw the model
    m_pModelLow->Enable();
    m_pModelLow->Draw();
}


// ****************************************************************
// 
coreBool cShip::_TakeDamage(const coreInt32& iDamage)
{
    // 
    if((m_iCurHealth -= iDamage) <= 0)
        m_iCurHealth = 0;

    // 
    this->SetColor3(LERP(coreVector3(0.5f,0.5f,0.5f), this->GetBaseColor(), 5.0f * MIN(this->GetCurHealthPct(), 0.2f)));

    // 
    if(iDamage) m_fBlink = 1.2f;

    // 
    return m_iCurHealth ? false : true;
}


// ****************************************************************
// add ship to the game
void cShip::_Resurrect(const coreBool& bSingle, const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32& iType)
{
    // reset ship properties
    m_iCurHealth = m_iMaxHealth;
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
    this->ChangeType(iType);
}


// ****************************************************************
// remove ship from the game
void cShip::_Kill(const coreBool& bSingle, const coreBool& bAnimated)
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
void cShip::_UpdateBlink()
{
    // 
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PRINTSCREEN), CORE_INPUT_PRESS))
        m_fBlink = 0.0f;

    // 
    m_fBlink = MAX(m_fBlink - 15.0f*Core::System->GetTime(), 0.0f);
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