//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

std::vector<coreObject3D*>  cGlobalBindContainer::s_apGlobalObject; // = NULL;
std::vector<coreBatchList*> cGlobalBindContainer::s_apGlobalList;   // = NULL;


// ****************************************************************
// constructor
cBindContainer::cBindContainer()noexcept
{
}


// ****************************************************************
// destructor
cBindContainer::~cBindContainer()
{
    ASSERT(m_apObject.empty() && m_apList.empty())

    // remove all remaining objects and lists
    this->ClearObjects();
    this->ClearLists();
}


// ****************************************************************
// check for duplicate objects and lists
void cBindContainer::_CheckDuplicates()const
{
#if defined(_CORE_DEBUG_)

    // check for duplicate objects and lists
    FOR_EACH(it, m_apObject) FOR_EACH_SET(et, it+1, m_apObject) ASSERT((*it) != (*et))
    FOR_EACH(it, m_apList)   FOR_EACH_SET(et, it+1, m_apList)   ASSERT((*it) != (*et))

#endif
}


// ****************************************************************
// exit the global bind container interface
void cGlobalBindContainer::_GlobalExit()
{
    ASSERT(s_apGlobalObject.empty() && s_apGlobalList.empty())

    // remove all remaining global objects and lists
    cGlobalBindContainer::ClearGlobalObjects();
    cGlobalBindContainer::ClearGlobalLists();
}


// ****************************************************************
// check for global duplicate objects and lists
void cGlobalBindContainer::_CheckGlobalDuplicates()
{
#if defined(_CORE_DEBUG_)

    // check for global duplicate objects and lists
    FOR_EACH(it, s_apGlobalObject) FOR_EACH_SET(et, it+1, s_apGlobalObject) ASSERT((*it) != (*et))
    FOR_EACH(it, s_apGlobalList)   FOR_EACH_SET(et, it+1, s_apGlobalList)   ASSERT((*it) != (*et))

#endif
}