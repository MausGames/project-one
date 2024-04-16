///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cIntroMission::cIntroMission()noexcept
{
    // 
    m_apBoss[0] = &m_ProjectOne;
}


// ****************************************************************
// 
void cIntroMission::__RenderOwnBottom()
{
    DEPTH_PUSH

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Turf.Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cIntroMission::__MoveOwnAfter()
{
    
        if(m_Turf.IsActive())
        {
            const coreFloat fOld = m_Turf.CalcPercent();
            
            g_pGame->GetBulletManagerPlayer()->ForEachBullet([this](cBullet* OUTPUT pBullet)
            {
                if(m_Turf.DrawPoint(pBullet->GetPosition().xy() + 0.5f * pBullet->GetFlyMove(), 4.0f, 1u) +
                   m_Turf.DrawPoint(pBullet->GetPosition().xy(),                                4.0f, 1u))
                {
                    //pBullet->Deactivate(true);
                }
            });
            
            const coreFloat fNew = m_Turf.CalcPercent();
            
            if((fOld < 0.8f) && (fNew >= 0.8f))
            {
                m_Turf.DrawAll(1u);
                m_Turf.Flash();
            }
        }
            
    // 
    m_Turf.Move();
}