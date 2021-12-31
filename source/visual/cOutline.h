///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTLINE_H_
#define _P1_GUARD_OUTLINE_H_

// TODO 3: check that bullet-styles are only used by objects with bullet-shaders
// TODO 3: bullets on top of each other have their outline multiplied
// TODO 1: improve outlining on top of each other (e.g. tiger body + weapon)


// ****************************************************************
// outline definitions
#define OUTLINE_STYLE_FULL          (0u)   // full outlining style
#define OUTLINE_STYLE_DIRECT        (1u)   // directional interpolated outlining style
#define OUTLINE_STYLE_FLAT_FULL     (2u)   // 
#define OUTLINE_STYLE_FLAT_DIRECT   (3u)   // 
#define OUTLINE_STYLE_BULLET_FULL   (4u)   // 
#define OUTLINE_STYLE_BULLET_DIRECT (5u)   // 
#define OUTLINE_STYLE_LIGHT         (6u)   // 
#define OUTLINE_STYLES              (7u)   // 

#define OUTLINE_THICKNESS (0.35f)


// ****************************************************************
// outline-style class
class cOutlineStyle final : public cBindContainerIn
{
private:
    coreProgramPtr s_pProgramSingle;      // shader-program for single outlined objects
    coreProgramPtr s_pProgramInstanced;   // shader-program for instanced outlined objects


public:
    cOutlineStyle() = default;

    DISABLE_COPY(cOutlineStyle)

    // 
    void Construct(const coreHashString& sProgramSingleName, const coreHashString& sProgramInstancedName);

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
    inline cOutlineStyle* GetStyle(const coreUintW iStyle) {ASSERT(iStyle < OUTLINE_STYLES) return &m_aOutlineStyle[iStyle];}
};


#endif // _P1_GUARD_OUTLINE_H_