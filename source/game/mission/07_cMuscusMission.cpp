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
cMuscusMission::cMuscusMission()noexcept
{
    // 
    m_apBoss[0] = &m_Orlac;
    m_apBoss[1] = &m_Geminga;
    m_apBoss[2] = &m_Nagual;
}