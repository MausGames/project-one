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
}