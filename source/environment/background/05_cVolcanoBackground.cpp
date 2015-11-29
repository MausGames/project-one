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
    m_pOutdoor->LoadProgram(true);
}


// ****************************************************************
// 
void cVolcanoBackground::__RenderOwn()
{
    // 
    m_Lava.Render();
}


// ****************************************************************
// 
void cVolcanoBackground::__MoveOwn()
{
    // 
    m_Lava.SetFlyOffset(g_pEnvironment->GetFlyOffset());
    m_Lava.Move();
}