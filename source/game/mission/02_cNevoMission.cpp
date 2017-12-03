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
cNevoMission::cNevoMission()noexcept
: m_vForce  (coreVector2(0.0f,0.0f))
, m_vImpact (coreVector2(0.0f,0.0f))
{
    // 
    m_apBoss[0] = &m_Nautilus;
    m_apBoss[1] = &m_Amemasu;
    m_apBoss[2] = &m_Leviathan;

    // 
    m_Container.DefineModel  ("object_sphere.md3");
    m_Container.DefineTexture(0u, "default_white.png");
    m_Container.DefineProgram("object_ship_program");
    m_Container.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 5.0f);
}


// ****************************************************************
// setup the Nevo mission
void cNevoMission::__SetupOwn()
{
    // ################################################################
    // 
    STAGE_MAIN
    {
        if(STAGE_BEGINNING)
        {
            g_pEnvironment->ChangeBackground(-cSeaBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

            g_pGame->GetInterface()->ShowMission(this);
            g_pGame->StartIntro();
        }

        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
            STAGE_FINISH_NOW
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Nautilus, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Amemasu, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });

    // ################################################################
    // 
    STAGE_MAIN
    {
        STAGE_BOSS(m_Leviathan, coreVector2(0.0f,2.0f), coreVector2(0.0f,-1.0f))
    });
}


// ****************************************************************
// 
void cNevoMission::__RenderOwnAttack()
{
    DEPTH_PUSH

    // 
    m_Container.Render();
}


// ****************************************************************
// 
void cNevoMission::__MoveOwnAfter()
{
    coreVector2 vNewPos = m_Container.GetPosition().xy();

    // 
    if(!m_vForce.IsNull())
    {
        vNewPos  += m_vForce * Core::System->GetTime();
        m_vForce *= 1.0f - 3.0f * Core::System->GetTime();
    }

    // 
    m_vImpact = coreVector2(0.0f,0.0f);

    // 
         if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; if(m_vForce.x < 0.0f) m_vImpact = coreVector2(-FOREGROUND_AREA.x * 1.1f, m_Container.GetPosition().y); m_vForce.x =  ABS(m_vForce.x);}
    else if(vNewPos.x >  FOREGROUND_AREA.x) {vNewPos.x =  FOREGROUND_AREA.x; if(m_vForce.x > 0.0f) m_vImpact = coreVector2( FOREGROUND_AREA.x * 1.1f, m_Container.GetPosition().y); m_vForce.x = -ABS(m_vForce.x);}
         if(vNewPos.y < -FOREGROUND_AREA.y) {vNewPos.y = -FOREGROUND_AREA.y; if(m_vForce.y < 0.0f) m_vImpact = coreVector2(m_Container.GetPosition().x, -FOREGROUND_AREA.y * 1.1f); m_vForce.y =  ABS(m_vForce.y);}
    else if(vNewPos.y >  FOREGROUND_AREA.y) {vNewPos.y =  FOREGROUND_AREA.y; if(m_vForce.y > 0.0f) m_vImpact = coreVector2(m_Container.GetPosition().x,  FOREGROUND_AREA.y * 1.1f); m_vForce.y = -ABS(m_vForce.y);}

    // 
    m_Container.SetPosition(coreVector3(vNewPos, 0.0f));
    m_Container.Move();
}