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
cSeaBackground::cSeaBackground()noexcept
{
    // create outdoor-surface object 
    m_pOutdoor = new cOutdoor("dust", "dust", 2, 4.0f);
}