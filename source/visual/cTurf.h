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


// ****************************************************************
// 
#define TURF_SIZE (64u)   // 


// ****************************************************************
// 
class cTurf final : public coreFullscreen, public coreResourceRelation
{
private:
    coreTexturePtr m_pTurfMap;     // 
    coreUint8*     m_piTurfData;   // 

    coreFloat m_fVisibility;       // 
    coreFloat m_fDelay;            // 
    coreFlow  m_fFlash;            // 
    coreBool  m_bDirty;            // 


public:
    cTurf()noexcept;
    ~cTurf()final;

    DISABLE_COPY(cTurf)

    // 
    void Render()final;
    void Move  ()final;

    // 
    void Enable ();
    void Disable(const coreFloat fDelay);

    // 
    coreUintW DrawPoint(const coreVector2 vPosition, const coreFloat fSize, const coreUint8 iType);
    coreUintW DrawLine (const coreVector2 vPosition, const coreFloat fSize, const coreBool bHorizontal, const coreUint8 iType);
    void      DrawAll  (const coreUint8 iType);
    
    coreFloat CalcPercent();
    
    void Flash();

    // 
    coreBool TestCollision(const coreVector2 vPosition)const;

    // 
    inline coreBool IsActive()const {return (m_fVisibility != 0.0f);}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    static coreUintW __GetMapIndex(const coreFloat fValue);
    static coreFloat __GetMapValue(const coreUintW iIndex);
};


#endif // _P1_GUARD_TURF_H_