//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTDOOR_H_
#define _P1_GUARD_OUTDOOR_H_


// ****************************************************************
// outdoor definitions
#define OUTDOOR_VIEW           (31u)                                             // visible rows
#define OUTDOOR_DETAIL         (6.0f)                                            // size of a block

#define OUTDOOR_WIDTH          (31u)                                             // vertices per row
#define OUTDOOR_HEIGHT         (160u)                                            // vertices per column
#define OUTDOOR_HEIGHT_FULL    (OUTDOOR_HEIGHT + OUTDOOR_VIEW)                   // with double vertices

#define OUTDOOR_BLOCKS_X       (OUTDOOR_WIDTH-1u)                                // blocks per row
#define OUTDOOR_BLOCKS_Y       (OUTDOOR_HEIGHT_FULL-1u)                          // blocks per column
#define OUTDOOR_BLOCKS         (OUTDOOR_BLOCKS_X * OUTDOOR_BLOCKS_Y)             // number of all blocks

#define OUTDOOR_PER_INDICES    (6u)                                              // indices per block
#define OUTDOOR_TOTAL_VERTICES (OUTDOOR_WIDTH * OUTDOOR_HEIGHT_FULL)             // total number of vertices
#define OUTDOOR_TOTAL_INDICES  (OUTDOOR_PER_INDICES * OUTDOOR_BLOCKS)            // total number of indices

#define OUTDOOR_RANGE (OUTDOOR_WIDTH * (OUTDOOR_VIEW+1u))                        // vertices used in a draw call
#define OUTDOOR_COUNT (OUTDOOR_BLOCKS_X * OUTDOOR_VIEW * OUTDOOR_PER_INDICES)    // indices to draw

STATIC_ASSERT((OUTDOOR_WIDTH == OUTDOOR_VIEW) && (OUTDOOR_WIDTH % 2))


// ****************************************************************
// outdoor-surface class
class cOutdoor final : public coreObject3D
{
private:
    // vertex structure
    struct sVertex
    {
        coreVector3 vPosition;   // vertex position
        coreVector3 vNormal;     // normal vector
        coreVector4 vTangent;    // additional tangent vector
    };

    // compressed vertex structure
    struct sVertexPacked
    {
        coreFloat  fPosition;   // vertex position           (only height)
        coreUint32 iNormal;     // normal vector             (Snorm210/Snorm4x8)
        coreUint32 iTangent;    // additional tangent vector (Snorm210/Snorm4x8)
    };


private:
    coreFloat m_afHeight[OUTDOOR_TOTAL_VERTICES];   // height data for height calculations

    coreUint32 m_iVertexOffset;                     // current vertex offset
    coreUint32 m_iIndexOffset;                      // current index offset
    coreFloat  m_fFlyOffset;                        // current fly offset

    coreUint8 m_iHandleIndex;                       // 
    coreUint8 m_iAlgorithm;                         // geometry algorithm ID
    coreFloat m_fGrade;                             // randomness grade

    cShadow m_ShadowMap;                            // shadow map object
    coreTexturePtr m_pNormalMap;                    // normal map object


public:
    cOutdoor(const coreChar* pcTextureTop, const coreChar* pcTextureBottom, const coreUint8 iAlgorithm, const coreFloat fGrade)noexcept;
    ~cOutdoor();

    DISABLE_COPY(cOutdoor)

    // render the outdoor-surface
    void Render(const coreProgramPtr& pProgram)override;
    void Render()override;

    // load outdoor resources
    void LoadGeometry(const coreUint8 iAlgorithm, const coreFloat fGrade);
    void LoadTextures(const coreChar* pcTextureTop, const coreChar* pcTextureBottom);
    void LoadProgram (const coreBool bGlow);

    // retrieve height value
    coreFloat   RetrieveHeight   (const coreVector2& vPosition);
    coreVector3 RetrieveIntersect(const coreVector3& vRayPosition, const coreVector3& vRayDirection);

    // access shadow map object
    inline cShadow* GetShadowMap() {return &m_ShadowMap;}

    // set object properties
    void SetFlyOffset(const coreFloat fFlyOffset);

    // get object properties
    inline const coreUint32& GetVertexOffset()const {return m_iVertexOffset;}
    inline const coreUint32& GetIndexOffset ()const {return m_iIndexOffset;}
    inline const coreFloat&  GetFlyOffset   ()const {return m_fFlyOffset;}
};


#endif // _P1_GUARD_OUTDOOR_H_