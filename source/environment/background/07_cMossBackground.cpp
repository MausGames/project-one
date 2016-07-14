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
cMossBackground::cMossBackground()noexcept
{
    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("dust", "crack", 5u, 4.5f);

    // 
    m_pWater = new cRainWater();
}