///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_ICON_H_
#define _P1_GUARD_ICON_H_

// TODO 1: optimize this, it uses quite a lot of CPU (OpenGL), even in release (e.g. separat list for outline and/or no batch for normal rendering) (eigener super-dicker outline shader)


// ****************************************************************
// 
#define ICON_VARIATIONS   (6u)      // 
#define ICON_SCALE_FACTOR (0.07f)   // 

#define ICON_TEXSIZE      (coreVector2(1.0f / I_TO_F(ICON_VARIATIONS), 1.0f))
#define ICON_TEXOFFSET    (coreVector2(1.0f / I_TO_F(ICON_VARIATIONS), 0.0f))
#define ICON_SCALE_VECTOR (coreVector2(I_TO_F(ICON_VARIATIONS), 1.0f) * ICON_SCALE_FACTOR)


// ****************************************************************
// 
class cIcon final: public coreResourceRelation
{
private:
    coreFrameBuffer m_FrameBuffer;                // 
    coreFrameBuffer m_ResolvedTexture;            // 

    coreBatchList m_Range;                        // 
    coreObject3D  m_aRangeRaw[ICON_VARIATIONS];   // 

    coreModelPtr m_pDefaultModel;                 // 

    coreUintW m_iIndex;                           // 
    coreFlow  m_fAnimation;                       // 
    coreFloat m_fStorage;                         // 


public:
    cIcon()noexcept;

    DISABLE_COPY(cIcon)

    // 
    void Update();

    // 
    void SetIndex(const coreUintW iIndex) {m_iIndex = iIndex;}

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_ResolvedTexture;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;
};


#endif // _P1_GUARD_ICON_H_