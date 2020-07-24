///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// render the LOD object (low-polygon only)
void cLodObject::Render(const coreProgramPtr& pProgram)
{
    if(!this->IsEnabled(CORE_OBJECT_ENABLE_RENDER)) return;

    // check for model status
    ASSERT(m_pModelLow)
    if(!m_pModelLow.IsUsable()) return;

    // enable the shader-program
    ASSERT(pProgram)
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // update all object uniforms
    coreProgram* pLocal = pProgram.GetResource();
    pLocal->SendUniform(CORE_SHADER_UNIFORM_3D_POSITION, this->GetPosition());
    pLocal->SendUniform(CORE_SHADER_UNIFORM_3D_SIZE,     this->GetSize());
    pLocal->SendUniform(CORE_SHADER_UNIFORM_3D_ROTATION, m_vRotation);
    pLocal->SendUniform(CORE_SHADER_UNIFORM_COLOR,       m_vColor);

    // draw the model
    m_pModelLow->Enable();
    m_pModelLow->Draw();
}