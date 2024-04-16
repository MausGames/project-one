///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_WORM_H_
#define _P1_GUARD_WORM_H_


// ****************************************************************
// worm definitions
#define WORM_ANCHORS (16u)   // 


// ****************************************************************
// worm class
class cWorm final
{
private:
    coreVector3 m_avAnchor[WORM_ANCHORS];
    coreFlow m_fTick;

    coreFloat m_fSpeed; // tick speed
    coreUint8 m_iCount; // used anchors


public:
    cWorm()noexcept;

    DISABLE_COPY(cWorm)

};


#endif // _P1_GUARD_WORM_H_