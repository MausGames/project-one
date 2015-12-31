//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTLINE_H_
#define _P1_GUARD_OUTLINE_H_


// ****************************************************************
// outline definitions
#define OUTLINE_STYLE_FULL   (0u)   // full outlining style
#define OUTLINE_STYLE_DIRECT (1u)   // directional interpolated outlining style
#define OUTLINE_STYLES       (2u)   // 


// ****************************************************************
// outline-style class
class cOutlineStyle final : public cBindContainer
{
private:
    coreProgramPtr s_pProgramSingle;      // shader-program for single outlined objects
    coreProgramPtr s_pProgramInstanced;   // shader-program for instanced outlined objects


public:
    cOutlineStyle() = default;

    DISABLE_COPY(cOutlineStyle)

    // 
    void Construct(const coreChar* pcProgramSingleName, const coreChar* pcProgramInstancedName);

    // apply deferred outline-style
    void Apply();

    // apply immediate outline-style
    void ApplyObject(coreObject3D*  pObject)const;
    void ApplyList  (coreBatchList* pList)const;
};


// ****************************************************************
// outline-layer class
class cOutline final
{
private:
    cOutlineStyle m_aOutlineStyle[OUTLINE_STYLES];   // 


public:
    cOutline()noexcept;

    DISABLE_COPY(cOutline)

    // apply deferred outline-layer
    void Apply();

    // 
    void ClearObjects();
    void ClearLists();

    // 
    inline cOutlineStyle* GetStyle(const coreUintW& iStyle) {ASSERT(iStyle < OUTLINE_STYLES) return &m_aOutlineStyle[iStyle];}
};


#endif // _P1_GUARD_OUTLINE_H_