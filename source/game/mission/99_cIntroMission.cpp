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
: m_bFirstPlay (g_pSave->GetHeader().oProgress.bFirstPlay)
{
    // 
    m_apBoss[0] = &m_Intro;

    // 
    m_aManual[0].ChangeOffset(-TOOLTIP_DEFAULT_OFFSET);
    m_aManual[1].ChangeOffset( TOOLTIP_DEFAULT_OFFSET);
}


// ****************************************************************
// 
void cIntroMission::__RenderOwnTop()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aManual); ++i)
        m_aManual[i].Render();
}


// ****************************************************************
// 
void cIntroMission::__MoveOwnAfter()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aManual); ++i)
        m_aManual[i].Move();
}