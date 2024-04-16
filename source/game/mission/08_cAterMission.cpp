///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cAterMission::cAterMission()noexcept
{
    // 
    m_apBoss[0] = &m_ProjectOne;
    m_apBoss[1] = &m_Eigengrau;
}