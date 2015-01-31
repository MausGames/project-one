//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_SHADOW_H_
#define _P1_GUARD_SHADOW_H_

// NOTE: shadow map binds local objects (per frame buffer) and global objects (on all frame buffers, for transitions)
// NOTE: all global objects are on the foreground


// ****************************************************************
// shadow definitions
#define SHADOW_VIEW_ORIENTATION   (coreVector3(0.0f, 0.624694824f, 0.780868530f))   // view orientation (not direction)
#define SHADOW_VIEW_DISTANCE      (90.0f)                                           // distance from origin to light source
#define SHADOW_CLIP_NEAR          (20.0f)                                           // near clipping plane
#define SHADOW_CLIP_FAR           (140.0f)                                          // far clipping plane
#define SHADOW_DETAIL_X           (0.014f)                                          // view size of the shadow map (higher = sharper, but more artifacts)
#define SHADOW_DETAIL_Y           (0.013f)                                          // same for Y

#define SHADOW_HANDLES            (3u)                 // number of handles for shader-programs with shadow maps
#define SHADOW_HANDLE_OUTDOOR     (0u)                 // outdoor-surface handle
#define SHADOW_HANDLE_OBJECT      (1u)                 // single object handle
#define SHADOW_HANDLE_OBJECT_INST (2u)                 // instanced object handle
#define SHADOW_SHADER_MATRIX      "u_m4ShadowMatrix"   // name of the shadow matrix uniform (transformation)


// ****************************************************************
// shadow map class
class cShadow final : public cBindContainer, public cGlobalBindContainer, public coreResourceRelation
{
private:
    coreFrameBuffer m_iFrameBuffer;                     // shadow map frame buffer (depth only)
    coreByte m_iLevel;                                  // current configuration level

    static coreProgramPtr s_pProgramSingle;             // shader-program for single shadow-casting objects
    static coreProgramPtr s_pProgramInstanced;          // shader-program for instanced shadow-casting objects
    static coreProgramPtr s_apHandle[SHADOW_HANDLES];   // handles for shader-programs with shadow maps

    static coreMatrix4 s_amDrawShadowMatrix[2];         // matrices used for view transformation of depth values (0 = background, 1 = foreground)
    static coreMatrix4 s_mReadShadowMatrix;             // matrix with additional coordinate adjustment


public:
    cShadow()noexcept;

    DISABLE_COPY(cShadow)

    // update the shadow map
    void Update();

    // adjust to current configuration
    void Reconfigure();

    // access frame buffer object
    inline coreFrameBuffer* GetFrameBuffer() {return &m_iFrameBuffer;}

    // control the shadow map class
    static void GlobalInit();
    static void GlobalExit();
    static void GlobalUpdate();

    // recompile shader-programs with shadow maps
    static void Recompile();

    // enable shader-program and apply read shadow matrix
    static void EnableShadowRead(const coreByte& iProgramHandle);

    // render depth pass with foreground objects
    static void RenderForegroundDepth();


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;

    // render shadow-casting objects
    static void __RenderSingle   (const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList, const std::vector<coreObject3D*>& apObject);
    static void __RenderInstanced(const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList);
};


#endif // _P1_GUARD_SHADOW_H_