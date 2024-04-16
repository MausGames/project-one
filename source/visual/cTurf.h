///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TURF_H_
#define _P1_GUARD_TURF_H_

// TODO 1: use snow as base instead, the functionality there is quite advanced, if necessary as new base (cCanvas)
// TODO 3: cache and recalculate percentage value only on changes


// ****************************************************************
// 
class cTurf final : public cSnow
{
private:
    coreFlow m_fFlash;   // 


public:
    cTurf()noexcept;

    DISABLE_COPY(cTurf)

    // 
    void Render()final;
    void Move  ()final;

    // 
    coreFloat CalcPercent()const;

    // 
    inline void Flash() {m_fFlash = 1.0f;}
};


#endif // _P1_GUARD_TURF_H_