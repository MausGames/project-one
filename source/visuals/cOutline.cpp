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
cOutline::cOutline()noexcept
{
    // load shader-programs for outlined objects
    s_pProgramSingle    = Core::Manager::Resource->Get<coreProgram>("effect_outline_program");
    s_pProgramInstanced = Core::Manager::Resource->Get<coreProgram>("effect_outline_inst_program");
}


// ****************************************************************
// destructor
cOutline::~cOutline()
{
    ASSERT(m_apObject.empty() && m_apList.empty())

    // remove all outlined objects and lists
    this->ClearObjects();
    this->ClearLists();
}


// ****************************************************************
// apply outline-effect
void cOutline::Apply()
{
#if defined(_CORE_DEBUG_)

    // check for duplicate objects and lists
    FOR_EACH(it, m_apObject) FOR_EACH_SET(et, it+1, m_apObject) ASSERT((*it) != (*et))
    FOR_EACH(it, m_apList)   FOR_EACH_SET(et, it+1, m_apList)   ASSERT((*it) != (*et))

#endif

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