///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_TRACKER_H_
#define _P1_GUARD_TRACKER_H_


// ****************************************************************
// tracker definitions
enum eTrackerStatus : coreUint8
{
    TRACKER_STATUS_DEAD = 0x01u   // completely removed from the game
};


// ****************************************************************
// tracker entity class
class cTracker final : public cShip
{
private:
    coreObject3D m_Range;     // 
    coreObject3D m_Wind;      // 
    coreObject3D m_Exhaust;   // 

    coreFlow m_fAnimation;    // 


public:
    cTracker()noexcept;
    ~cTracker()final;

    DISABLE_COPY(cTracker)

    // render and move the tracker
    void Render      ()final;
    void RenderBefore();
    void RenderMiddle();
    void RenderAfter ();
    void Move        ()final;

    // control life and death
    void Resurrect();
    void Kill     (const coreBool bAnimated);

    // 
    void EnableRange  ();
    void DisableRange ();
    void EnableWind   ();
    void DisableWind  ();
    void UpdateExhaust(const coreFloat fStrength);
};


#endif // _P1_GUARD_TRACKER_H_