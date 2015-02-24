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
    pProgram->SendUniform(CORE_SHADER_UNIFORM_COLOR,       m_vColor);

    // draw the model
    m_pModelLow->Enable();
    m_pModelLow->Draw();
}


// ****************************************************************
// add ship to the game
void cShip::_Resurrect(const coreVector2& vPosition, const int& iType)
{
    // reset ship properties
    m_iCurHealth = m_iMaxHealth;
    m_vNewPos    = vPosition;
    this->SetPosition(coreVector3(vPosition, 0.0f));

    // add ship to global shadow and outline
    cShadow::BindGlobalObject(this);
    g_pOutlineFull->BindObject(this);

    // enable collision
    this->ChangeType(iType);
}


// ****************************************************************
// remove ship from the game
void cShip::_Kill(const bool& bAnimated)
{
    // remove ship from global shadow and outline
    cShadow::UnbindGlobalObject(this);
    g_pOutlineFull->UnbindObject(this);

    // disable collision
    this->ChangeType(0);
}