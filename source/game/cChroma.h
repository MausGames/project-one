///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CHROMA_H_
#define _P1_GUARD_CHROMA_H_


// ****************************************************************
// 
#define CHROMA_SCALE_TINY  (1.0f)   // 
#define CHROMA_SCALE_SMALL (2.0f)   // 
#define CHROMA_SCALE_BIG   (3.0f)   // 


// ****************************************************************
// 
class cChromaManager final
{
private:
    cBulletManager m_BulletManager;   // 


public:
    cChromaManager()noexcept;

    DISABLE_COPY(cChromaManager)

    // 
    void Render();
    void Move();

    // 
    void AddChroma(const coreVector2 vPosition, const coreVector2 vDirection, const coreFloat fScale, const coreVector3 vColor);
    void ClearChromas(const coreBool bAnimated);
};


#endif // _P1_GUARD_CHROMA_H_