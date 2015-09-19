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
cVolcanoBackground::cVolcanoBackground()noexcept
{
    // create outdoor-surface object
    m_pOutdoor = new cOutdoor("rock", "lava", 7u, 4.0f);

    // 
    m_pWater = new cWater();
}