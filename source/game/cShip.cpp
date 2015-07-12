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
// render the ship (low-polygon)
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
    this->SetColor3(LERP(coreVector3(0.5f,0.5f,0.5f), this->GetBaseColor(), 5.0f * MIN(I_TO_F(m_iCurHealth) / I_TO_F(m_iMaxHealth), 0.2f)));

    // 
    if(iDamage) m_fBlink = 1.2f;

    return !m_iCurHealth;
}


// ****************************************************************
// add ship to the game
void cShip::_Resurrect(const coreVector2& vPosition, const coreVector2& vDirection, const coreInt32& iType)
{
    // reset ship properties
    m_iCurHealth = m_iMaxHealth;
    this->SetPosition (coreVector3(vPosition,  0.0f));
    this->SetDirection(coreVector3(vDirection, 0.0f));
    this->SetColor3   (this->GetBaseColor());

    // add ship to global shadow and outline
    cShadow::GetGlobalContainer()->BindObject(this);
    g_aaOutline[PRIO_WEAK][STYLE_FULL].BindObject(this);

    // enable collision
    this->ChangeType(iType);
}


// ****************************************************************
// remove ship from the game
void cShip::_Kill(const coreBool& bAnimated)
{
    // remove ship from global shadow and outline
    cShadow::GetGlobalContainer()->UnbindObject(this);
    g_aaOutline[PRIO_WEAK][STYLE_FULL].UnbindObject(this);

    // disable collision
    this->ChangeType(0);
}


// ****************************************************************
// 
void cShip::_UpdateBlink()
{
    if(!this->GetProgram().IsUsable()) return;

    // 
    if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PRINTSCREEN), CORE_INPUT_PRESS))
        m_fBlink = 0.0f;

    // 
    this->GetProgram()->Enable();
    this->GetProgram()->SendUniform("u_v1Blink", MIN(m_fBlink, 1.0f) * 0.8f);

    // 
    if(m_fBlink) m_fBlink = MAX(m_fBlink - 3.0f*Core::System->GetTime(), 0.0f);
}