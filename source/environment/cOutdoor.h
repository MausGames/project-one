///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_OUTDOOR_H_
#define _P1_GUARD_OUTDOOR_H_

// TODO 5: check out jcgt_Duff2017Basis to inline tangent calculations into shader (less bandwidth, but more ALU + reg ?)
// TODO 5: increase normal map sharpness, if quality improves (raise depth-value in generator until it throws an assertion)
// TODO 4: move m_pOutdoor->SetEnabled(CORE_OBJECT_ENABLE_NOTHING); into constructor/factoryfunc
// TODO 3: texture-coordinates are just stretched across XY, but they should be denser on Z changes
// TODO 3: implement a way to invalidate the token when the normal-map lambda is finished
// TODO 3: make sure setting the normal-map to the real texture is atomic


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

#define OUTDOOR_SCALE_FACTOR   (SCALE_FACTOR)                                             // frame buffer resolution factor

#define OUTDOOR_SHADER_ATTRIBUTE_HEIGHT       "a_v1Height"
#define OUTDOOR_SHADER_ATTRIBUTE_POSITION     "a_v2Position"
#define OUTDOOR_SHADER_ATTRIBUTE_HEIGHT_NUM   (CORE_SHADER_ATTRIBUTE_USER_NUM + 0u)
#define OUTDOOR_SHADER_ATTRIBUTE_POSITION_NUM (CORE_SHADER_ATTRIBUTE_USER_NUM + 1u)

STATIC_ASSERT((OUTDOOR_WIDTH == OUTDOOR_VIEW) && !coreMath::IsAligned(OUTDOOR_WIDTH, 2u))

enum eOutdoorAlgorithm : coreUint8
{
    OUTDOOR_ALGORITHM_NOTHING = 0u,
    OUTDOOR_ALGORITHM_SNOW,
    OUTDOOR_ALGORITHM_GRASS,
    OUTDOOR_ALGORITHM_DESERT,
    OUTDOOR_ALGORITHM_UNUSED_1,
    OUTDOOR_ALGORITHM_MOSS,
    OUTDOOR_ALGORITHM_SEA,
    OUTDOOR_ALGORITHM_VOLCANO,
    OUTDOOR_ALGORITHM_UNUSED_2,
    OUTDOOR_ALGORITHM_UNUSED_3,
    OUTDOOR_ALGORITHM_STOMACH
};


// ****************************************************************
// outdoor-surface class
class cOutdoor final : public coreObject3D, public coreResourceRelation
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
    coreFloat  m_fMaxHeight;                         // 

    coreUint32 m_iVertexOffset;                      // current vertex offset
    coreUint32 m_iIndexOffset;                       // current index offset
    coreFloat  m_fFlyOffset;                         // current fly offset

    coreUint8  m_iHandleIndex;                       // 
    coreUint8  m_iAlgorithm;                         // geometry algorithm ID
    coreFloat  m_fGrade;                             // randomness grade
    coreUint64 m_iSeed;                              // 

    const coreChar* m_pcTop;                         // 
    const coreChar* m_pcBottom;                      // 

    cShadow         m_ShadowMap;                     // shadow map object
    coreFrameBuffer m_LightMap;                      // 

    coreTexturePtr m_pNormalMap;                     // normal map object
    coreProgramPtr m_pDefaultProgram;                // 
    coreProgramPtr m_pLightProgram;                  // 

    coreFloat  m_afLerpMul  [2];                     // (old, new) 
    coreFloat  m_afLerpAdd  [2];                     // (old, new) 
    coreUint16 m_aiLerpRange[2];                     // (current, target) 
    coreFloat  m_afLerpData [7];                     // (from mul, from add, to mul, to add, mid mul, mid add, mid pos) 

    coreSync   m_Sync;                               // sync object for asynchronous texture loading
    coreUint32 m_iToken;                             // 


public:
    cOutdoor()noexcept;
    cOutdoor(const coreChar* pcTextureTop, const coreChar* pcTextureBottom, const coreUint8 iAlgorithm, const coreFloat fGrade, const coreBool bLight, const coreUint64 iSeed = CORE_RAND_SEED)noexcept;
    ~cOutdoor()final;

    DISABLE_COPY(cOutdoor)

    // render the outdoor-surface
    void Render()final;
    void RenderLight();
    void RenderDepth();

    // load outdoor resources
    void LoadGeometry(const coreUint8 iAlgorithm, const coreFloat fGrade, const coreUint64 iSeed);
    void LoadTextures(const coreChar* pcTextureTop, const coreChar* pcTextureBottom);
    void LoadProgram (const coreBool bGlow);

    // retrieve geometric data
    FUNC_PURE coreFloat   RetrieveHeight    (const coreVector2 vPosition)const;
    FUNC_PURE coreFloat   RetrieveBackHeight(const coreVector2 vPosition)const;
    FUNC_PURE coreVector3 RetrieveNormal    (const coreVector2 vPosition)const;
    FUNC_PURE coreVector3 RetrieveBackNormal(const coreVector2 vPosition)const;
    FUNC_PURE coreVector3 RetrieveIntersect (const coreVector3 vRayPosition, const coreVector3 vRayDirection)const;

    // 
    void                   LerpHeight    (const coreFloat fMul, const coreFloat fAdd, const coreUint16 iRange = 100u);
    void                   LerpHeightNow (const coreFloat fMul, const coreFloat fAdd);
    FUNC_LOCAL coreVector2 CalcLerpVector(const coreFloat fPositionY)const;
    inline coreBool  IsLerping()const {return (m_aiLerpRange[0] != m_aiLerpRange[1]);}
    inline coreFloat GetLerp  ()const {return (m_aiLerpRange[1] ? (I_TO_F(m_aiLerpRange[0]) * RCP(I_TO_F(m_aiLerpRange[1]))) : 1.0f);}

    // access shadow and light map object
    inline cShadow*         GetShadowMap() {return &m_ShadowMap;}
    inline coreFrameBuffer* GetLightMap () {return &m_LightMap;}
    void UpdateLightMap();
    void InvalidateLightMap();

    // set object properties
    void SetFlyOffset(const coreFloat fFlyOffset);
    void SetTransform(const coreFloat fFlyOffset, const coreFloat fSideOffset, const coreVector2 vDirection);

    // get object properties
    inline const coreUint32& GetVertexOffset()const {return m_iVertexOffset;}
    inline const coreUint32& GetIndexOffset ()const {return m_iIndexOffset;}
    inline const coreFloat&  GetFlyOffset   ()const {return m_fFlyOffset;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // reshape with the resource manager
    void __Reshape()final;

    // 
    template <typename F> void __Render(const coreProgramPtr& pProgram, F&& nFunction);   // [](void) -> void
};


// ****************************************************************
// 
template <typename F> void cOutdoor::__Render(const coreProgramPtr& pProgram, F&& nFunction)
{
    if(!this->IsEnabled(CORE_OBJECT_ENABLE_RENDER)) return;

    // check for model status
    ASSERT(m_pModel)
    if(!m_pModel.IsUsable()) return;

    // enable the shader-program
    ASSERT(pProgram)
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;

    // update all object uniforms
    pProgram->SendUniform("u_v4LerpData1", r_cast<coreVector4&>(m_afLerpData[0]));
    pProgram->SendUniform("u_v3LerpData2", r_cast<coreVector3&>(m_afLerpData[4]));

    // 
    nFunction();

    // draw the model
    m_pModel->Enable();
    glDrawRangeElements(m_pModel->GetPrimitiveType(), m_iVertexOffset, m_iVertexOffset + OUTDOOR_RANGE - 1u, OUTDOOR_COUNT, m_pModel->GetIndexType(), I_TO_P(m_iIndexOffset));
}


#endif // _P1_GUARD_OUTDOOR_H_