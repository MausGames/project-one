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
// init the outline-layer
void cOutline::Init(const coreChar* pcProgramSingleName, const coreChar* pcProgramInstancedName)
{
    // load shader-programs for outlined objects
    s_pProgramSingle    = Core::Manager::Resource->Get<coreProgram>(pcProgramSingleName);
    s_pProgramInstanced = Core::Manager::Resource->Get<coreProgram>(pcProgramInstancedName);
}


// ****************************************************************
// exit the outline-layer
void cOutline::Exit()
{
    // unload shader-programs
    s_pProgramSingle    = NULL;
    s_pProgramInstanced = NULL;
}


// ****************************************************************
// apply outline-layer
void cOutline::Apply()
{
    //glDepthMask(false);
    {
        // draw single objects
        FOR_EACH(it, this->GetObjectSet())
            (*it)->Render(s_pProgramSingle);

        // draw lists with objects
        FOR_EACH(it, this->GetListSet())
            (*it)->Render(s_pProgramInstanced, s_pProgramSingle);
    }
    //glDepthMask(true);
}