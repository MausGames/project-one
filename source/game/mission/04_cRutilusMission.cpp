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
cRutilusMission::cRutilusMission()noexcept
{
    // 
    m_apBoss[0] = &m_Quaternion;
    m_apBoss[1] = &m_Saros;
    m_apBoss[2] = &m_Messier;
}