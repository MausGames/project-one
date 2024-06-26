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

// TODO 3: implement smooth snow creation (new enum), which automatically increases snow from 0 to 255, with correct frame-time (tick-loop adding 00...010...01... ?, or in draw-functions)


// ****************************************************************
// 
#define SNOW_SIZE (64u)   // 

enum eSnowType : coreUint8
{
    SNOW_TYPE_REMOVE = 0u,   // 
    SNOW_TYPE_ADD    = 1u,   // 
    SNOW_TYPE_INVERT = 2u    // 
};


// ****************************************************************
// 
class cSnow : public coreFullscreen, public coreResourceRelation
{
protected:
    coreTexturePtr m_pSnowMap;     // 
    coreUint8*     m_piSnowData;   // 

    coreFloat m_fVisibility;       // 
    coreFloat m_fDelay;            // 
    coreBool  m_bDirty;            // 


public:
    cSnow()noexcept;
    virtual ~cSnow()override;

    DISABLE_COPY(cSnow)

    // 
    virtual void Render()override;
    virtual void Move  ()override;

    // 
    void Enable ();
    void Disable(const coreFloat fDelay);

    // 
    coreUintW DrawPoint(const coreVector2 vPosition, const coreFloat fSize, const eSnowType eType);
    coreUintW DrawLine (const coreVector2 vPosition, const coreFloat fSize, const coreBool bHorizontal, const eSnowType eType);
    coreUintW DrawRay  (const coreVector2 vPosition, const coreVector2 vDirection, const eSnowType eType);
    void      DrawAll  (const eSnowType eType);

    // 
    coreBool TestCollision(const coreVector2 vPosition)const;
    coreBool TestCollision(const coreVector2 vPosition, const coreVector2 vDirection, coreFloat* OUTPUT pfDistance)const;

    // 
    coreBool AnyData()const;
    coreBool AllData()const;

    // 
    inline coreBool IsActive()const {return (m_fVisibility != 0.0f) && (m_fDelay == 0.0f);}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    static coreFloat __GetMapIndexFloat(const coreFloat fValue);
    static coreUintW __GetMapIndex     (const coreFloat fValue);
    static coreFloat __GetMapValue     (const coreUintW iIndex);

    // 
    static coreVector2 __SnapPosition(const coreVector2 vPosition);
};


#endif // _P1_GUARD_SNOW_H_