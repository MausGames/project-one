///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTDOOR_H_
#define _P1_GUARD_OUTDOOR_H_

// TODO: definitions for algorithms (background names ?)
// TODO: add F16C SSE to RetrieveBackHeight
// TODO: check out jcgt_Duff2017Basis to inline tangent calculations into shader (less bandwidth, but more ALU + reg ?)
// TODO: outdoor parameters are reset on engine-reset !!!    
// TODO: fix file getting unloaded while others are reading (in reource-manager), maybe make copies of the file
// TODO: get max height of outdoor model and use for first step in RetrieveIntersect


// ****************************************************************
// outdoor definitions
#define OUTDOOR_VIEW           (31u)                                                      // visible rows
#define OUTDOOR_DETAIL         (6.0f)                                                     // size of a block

#define OUTDOOR_WIDTH          (31u)                                                      // vertices per row
#define OUTDOOR_HEIGHT         (160u)                                                     // vertices per column
#define OUTDOOR_HEIGHT_FULL    (OUTDOOR_HEIGHT + OUTDOOR_VIEW)                            // with double vertices

#define OUTDOOR_BLOCKS_X       (OUTDOOR_WIDTH - 1u)                                       // blocks per row
#define OUTDOOR_BLOCKS_Y       (OUTDOOR_HEIGHT_FULL - 1u)                                 // blocks per column
#define OUTDOOR_BLOCKS         (OUTDOOR_BLOCKS_X * OUTDOOR_BLOCKS_Y)                      // number of all blocks

#define OUTDOOR_PER_INDICES    (6u)                                                       // indices per block
#define OUTDOOR_TOTAL_VERTICES (OUTDOOR_WIDTH * OUTDOOR_HEIGHT_FULL)                      // total number of vertices
#define OUTDOOR_TOTAL_INDICES  (OUTDOOR_PER_INDICES * OUTDOOR_BLOCKS)                     // total number of indices

#define OUTDOOR_RANGE          (OUTDOOR_WIDTH * (OUTDOOR_VIEW + 1u))                      // vertices used in a draw call
#define OUTDOOR_COUNT          (OUTDOOR_BLOCKS_X * OUTDOOR_VIEW * OUTDOOR_PER_INDICES)    // indices to draw

#define OUTDOOR_SCALE_FACTOR   (0.5f)                                                     // frame buffer resolution factor

STATIC_ASSERT((OUTDOOR_WIDTH == OUTDOOR_VIEW) && (OUTDOOR_WIDTH % 2u))


// ****************************************************************
// outdoor-surface class
class cOutdoor final : public coreObject3D
{
private:
    // vertex structure
    struct sVertex final
    {
        coreVector3 vPosition;   // vertex position
        coreVector3 vNormal;     // normal vector
        coreVector4 vTangent;    // additional tangent vector
    };

    // compressed vertex structure
    #pragma pack(push, 4)
    struct sVertexPacked final
    {
        coreFloat  fHeight;    // vertex height
        coreUint32 iNormal;    // normal vector             (Snorm210/Snorm4x8)
        coreUint32 iTangent;   // additional tangent vector (Snorm210/Snorm4x8)
    };
    STATIC_ASSERT(sizeof(sVertexPacked) == 12u)
    #pragma pack(pop)


private:
    coreUint16 m_aiHeight[OUTDOOR_TOTAL_VERTICES];   // height data for height calculations (half-float)

    coreUint32 m_iVertexOffset;                      // current vertex offset
    coreUint32 m_iIndexOffset;                       // current index offset
    coreFloat  m_fFlyOffset;                         // current fly offset

    coreUint8 m_iHandleIndex;                        // 
    coreUint8 m_iAlgorithm;                          // geometry algorithm ID
    coreFloat m_fGrade;                              // randomness grade

    cShadow         m_ShadowMap;                     // shadow map object
    coreFrameBuffer m_LightMap;                      // 

    coreTexturePtr m_pNormalMap;                     // normal map object
    coreProgramPtr m_pDefaultProgram;                // 
    coreProgramPtr m_pLightProgram;                  // 

    coreFloat  m_afLerpMul  [2];                     // (old, new) 
    coreFloat  m_afLerpAdd  [2];                     // (old, new) 
    coreUint16 m_aiLerpRange[2];                     // (current, target) 
    coreFloat  m_afLerpData [7];                     // (from mul, from add, to mul, to add, mid mul, mid add, mid pos) 

    coreSync m_Sync;                                 // sync object for asynchronous texture loading


public:
    cOutdoor(const coreChar* pcTextureTop, const coreChar* pcTextureBottom, const coreUint8 iAlgorithm, const coreFloat fGrade)noexcept;
    ~cOutdoor()final;

    DISABLE_COPY(cOutdoor)

    // render the outdoor-surface
    void Render()final;
    void RenderLight();
    void RenderDepth();

    // load outdoor resources
    void LoadGeometry(const coreUint8 iAlgorithm, const coreFloat fGrade);
    void LoadTextures(const coreChar* pcTextureTop, const coreChar* pcTextureBottom);
    void LoadProgram (const coreBool bGlow);

    // retrieve geometric data
    FUNC_PURE coreFloat   RetrieveHeight    (const coreVector2& vPosition)const;
    FUNC_PURE coreFloat   RetrieveBackHeight(const coreVector2& vPosition)const;
    FUNC_PURE coreVector3 RetrieveNormal    (const coreVector2& vPosition)const;
    FUNC_PURE coreVector3 RetrieveBackNormal(const coreVector2& vPosition)const;
    FUNC_PURE coreVector3 RetrieveIntersect (const coreVector3& vRayPosition, const coreVector3& vRayDirection)const;

    // 
    void                   LerpHeight    (const coreFloat fMul, const coreFloat fAdd, const coreUint16 iRange = 0u);
    void                   LerpHeightNow (const coreFloat fMul, const coreFloat fAdd);
    FUNC_LOCAL coreVector2 CalcLerpVector(const coreFloat fPositionY)const;
    inline coreBool IsLerping()const {return (m_aiLerpRange[0] != m_aiLerpRange[1]);}

    // access shadow and light map object
    inline cShadow*         GetShadowMap() {return &m_ShadowMap;}
    inline coreFrameBuffer* GetLightMap () {return &m_LightMap;}
    void UpdateLightMap();

    // set object properties
    void SetFlyOffset(const coreFloat fFlyOffset);

    // get object properties
    inline const coreUint32& GetVertexOffset()const {return m_iVertexOffset;}
    inline const coreUint32& GetIndexOffset ()const {return m_iIndexOffset;}
    inline const coreFloat&  GetFlyOffset   ()const {return m_fFlyOffset;}


private:
    // 
    template <typename F> void __Render(const coreProgramPtr& pProgram, F&& nFunction);   // [](void) -> void
};


// ****************************************************************
// 
template <typename F> void cOutdoor::__Render(const coreProgramPtr& pProgram, F&& nFunction)
{
    if(!this->IsEnabled(CORE_OBJECT_ENABLE_RENDER)) return;

    // check for model status
    if(!m_pModel.IsUsable()) return;

    // enable the shader-program
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // update all object uniforms
    pProgram->SendUniform("u_v4LerpData1", r_cast<coreVector4&>(m_afLerpData[0]));
    pProgram->SendUniform("u_v3LerpData2", r_cast<coreVector3&>(m_afLerpData[4]));

    // 
    nFunction();

    // draw the model
    m_pModel->Enable();
    glDrawRangeElements(m_pModel->GetPrimitiveType(), m_iVertexOffset, m_iVertexOffset + OUTDOOR_RANGE, OUTDOOR_COUNT, m_pModel->GetIndexType(), I_TO_P(m_iIndexOffset));
}


#endif // _P1_GUARD_OUTDOOR_H_