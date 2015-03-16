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
cOutline::cOutline(const coreChar* pcProgramSingleName, const coreChar* pcProgramInstancedName)noexcept
{
    // load shader-programs for outlined objects
    s_pProgramSingle    = Core::Manager::Resource->Get<coreProgram>(pcProgramSingleName);
    s_pProgramInstanced = Core::Manager::Resource->Get<coreProgram>(pcProgramInstancedName);
}


// ****************************************************************
// apply outline-effect
void cOutline::Apply()
{
    glDepthMask(false);
    {
        // draw single objects
        FOR_EACH(it, m_apObject)
            (*it)->Render(s_pProgramSingle);

        // draw lists with objects
        FOR_EACH(it, m_apList)
            (*it)->Render(s_pProgramInstanced, s_pProgramSingle);
    }
    glDepthMask(true);
}