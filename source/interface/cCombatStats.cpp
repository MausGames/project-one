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
cCombatStats::cCombatStats()noexcept
{
    // 
    m_Bar.DefineProgram("default_2d_program");
    m_Bar.DefineTexture(0u, "default_white.png");
}


// ****************************************************************
// apply combat stats
void cCombatStats::Apply()
{



    g_pGame->ForEachPlayer([&](const cPlayer* pPlayer)
    {

        if(pPlayer->GetDarkTime())
        {
            m_Bar.SetPosition(coreVector2(0.0f,-0.04f));
            m_Bar.SetCenter  (g_pForeground->Project(pPlayer->GetPosition()) * g_vMenuCenter);
            m_Bar.SetSize    (coreVector2(0.08f * pPlayer->GetDarkTime(),0.01f));
            m_Bar.SetColor4  (coreVector4(0.1f,0.1f,0.1f,0.7f));
            m_Bar.Move();
            m_Bar.Render();
        }

    });



}