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
// exit the global bind container interface
void cGlobalBindContainer::_GlobalExit()
{
    ASSERT(s_apGlobalObject.empty() && s_apGlobalList.empty())

    // remove all remaining global objects and lists
    cGlobalBindContainer::ClearGlobalObjects();
    cGlobalBindContainer::ClearGlobalLists();
}