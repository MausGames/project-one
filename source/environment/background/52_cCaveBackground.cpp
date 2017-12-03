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
cCaveBackground::cCaveBackground()noexcept
{

}


// ****************************************************************
// 
void cCaveBackground::__RenderOwn()
{
    // 
    glDisable(GL_DEPTH_TEST);
    m_Headlight.Render();
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cCaveBackground::__UpdateOwn()
{
    // 
    m_Headlight.UpdateDefault();
}