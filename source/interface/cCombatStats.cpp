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
    m_Bar.DefineTexture(0u, "default_white.png");
    m_Bar.DefineProgram("default_2d_program");

    // 
    m_Circle.DefineTexture(0u, "default_white.png");
    m_Circle.DefineProgram("menu_circle_program");
}


// ****************************************************************
// apply combat stats
void cCombatStats::Apply()
{
    // 
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        // 
        for(coreUintW j = 0u; j < PLAYER_WEAPONS; ++j)
            pPlayer->GetWeapon(j)->RenderOverlay();
    });

    /*

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


    const coreFloat   fValue = 1.0f - FRACT(0.1f*coreFloat(Core::System->GetTotalTime()));
    const coreVector3 vColor = (fValue < 0.25f) ? COLOR_MENU_RED : COLOR_MENU_YELLOW;

    m_Circle.SetPosition (coreVector2(0.0f,-0.04f));
    m_Circle.SetCenter   (g_pForeground->Project(coreVector3(0.0f,0.0f,0.0f)) * g_vMenuCenter);
    m_Circle.SetSize     (coreVector2(0.1f,0.1f));
    m_Circle.SetColor4   (coreVector4(vColor * 0.9f, 0.8f));
    m_Circle.SetTexOffset(coreVector2(2.0f*PI*fValue, 0.0f));
    m_Circle.Move();
    m_Circle.Render();

    */
}