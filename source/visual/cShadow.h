///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHADOW_H_
#define _P1_GUARD_SHADOW_H_

// TODO 3: change projection to perfectly fit the screen
// TODO 3: check if reusing the center of the shadow-map is feasible
// TODO 5: poisson sampling
// TODO 3: make sure no textures are changed when rendering shadow or outline
// TODO 3: check if projected texture-sampling-offsets in shader are correct (and check if shadow of stings on stings in snow-background can be fixed)
// TODO 3: use conservative rasterization for low-resolution test area (NV_conservative_raster, INTEL_conservative_rasterization) (only on high setting ?) (very easy to enable, but needs testing)

// NOTE: shadow map binds local objects (per frame buffer) and global objects (on all frame buffers, for transitions)
// NOTE: all global objects are on the foreground


// ****************************************************************
// shadow definitions
#define SHADOW_VIEW_DISTANCE (90.0f)    // distance from origin to light source
#define SHADOW_CLIP_NEAR     (1.0f)     // near clipping plane
#define SHADOW_CLIP_FAR      (145.0f)   // far clipping plane
#define SHADOW_RANGE_X       (56.0f)    // view range of the shadow map (smaller = sharper, but more artifacts)
#define SHADOW_RANGE_Y       (62.0f)    // same for Y
#define SHADOW_HEIGHT_FACTOR (2.5f)     // height correction factor (to reduce shadow length)
#define SHADOW_TEST_FACTOR   (0.2f)     // 
#define SHADOW_RES_LOW       (1.0f)     // 
#define SHADOW_RES_HIGH      (1.5f)     // 

#define SHADOW_HANDLE_OUTDOOR            (0u)   // outdoor-surface handle
#define SHADOW_HANDLE_OUTDOOR_GLOW       (1u)   // 
#define SHADOW_HANDLE_OUTDOOR_LIGHT      (2u)   // 
#define SHADOW_HANDLE_OUTDOOR_LIGHT_GLOW (3u)   // 
#define SHADOW_HANDLE_OBJECT             (4u)   // single object handle
#define SHADOW_HANDLE_OBJECT_INST        (5u)   // instanced object handle
#define SHADOW_HANDLE_OBJECT_WAVE        (6u)   // 
#define SHADOW_HANDLE_OBJECT_WAVE_INST   (7u)   // 
#define SHADOW_HANDLES                   (8u)   // number of handles for shader-programs for shadow-receiving objects


// ****************************************************************
// shadow map class
class cShadow final : public cBindContainerIn, public coreResourceRelation
{
private:
    coreFrameBuffer m_FrameBuffer;                      // shadow map frame buffer
    coreUint8       m_iLevel;                           // current configuration level

    static cBindContainer s_GlobalContainer;            // global shadow-casting objects (in all frame buffers)

    static coreProgramPtr s_pProgramSingle;             // shader-program for single shadow-casting objects
    static coreProgramPtr s_pProgramInstanced;          // shader-program for instanced shadow-casting objects
    static coreProgramPtr s_apHandle[SHADOW_HANDLES];   // handles for shader-programs for shadow-receiving objects

    static coreMatrix4 s_amDrawShadowMatrix[2];         // matrices used for view transformation of depth values (0 = background, 1 = foreground)
    static coreMatrix4 s_mReadShadowMatrix;             // matrix with additional coordinate adjustment


public:
    cShadow()noexcept;

    DISABLE_COPY(cShadow)

    // update the shadow map
    void Update();

    // adjust to current configuration
    void Reconfigure();

    // access frame buffer
    inline coreFrameBuffer* GetFrameBuffer() {return &m_FrameBuffer;}

    // control the shadow map class
    static void GlobalInit();
    static void GlobalExit();
    static void GlobalUpdate();

    // recompile shader-programs for shadow-receiving objects
    static void Recompile();

    // render shadow-casting objects
    static void RenderSingle   (const coreMatrix4& mTransform, const coreList<coreBatchList*>& apList, const coreList<coreObject3D*>& apObject);
    static void RenderSingle   (const coreMatrix4& mTransform, const coreList<coreBatchList*>& apList);
    static void RenderInstanced(const coreMatrix4& mTransform, const coreList<coreBatchList*>& apList);

    // enable shader-program and apply read shadow matrix
    static void EnableShadowRead(const coreUintW iHandleIndex);

    // access global bind container
    static inline cBindContainer* GetGlobalContainer() {return &s_GlobalContainer;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // send transformation matrix to shader-program
    static void __SendTransform(const coreProgramPtr& pProgram, const coreMatrix4& mTransform);
};


#endif // _P1_GUARD_SHADOW_H_