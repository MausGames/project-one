///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SNOW_H_
#define _P1_GUARD_SNOW_H_


// ****************************************************************
// 
#define SNOW_SIZE (64u)   // 


// ****************************************************************
// 
class cSnow final : public coreFullscreen, public coreResourceRelation
{
private:
    coreTexturePtr m_pSnowMap;     // 
    coreUint8*     m_piSnowData;   // 

    coreFloat m_fVisibility;       // 
    coreFloat m_fDelay;            // 
    coreBool  m_bDirty;            // 


public:
    cSnow()noexcept;
    ~cSnow()final;

    DISABLE_COPY(cSnow)

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


#endif // _P1_GUARD_SNOW_H_