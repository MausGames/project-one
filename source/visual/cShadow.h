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

// TODO: check out shadow-volumes (would be overall superior in this use-case), in mockup: low-poly ship models have many artifacts (cuts), because optimized for outlining, but good performance and quality

// NOTE: shadow map binds local objects (per frame buffer) and global objects (on all frame buffers, for transitions)
// NOTE: all global objects are on the foreground


// ****************************************************************
// shadow definitions
#define SHADOW_VIEW_DISTANCE      (90.0f)              // distance from origin to light source
#define SHADOW_CLIP_NEAR          (20.0f)              // near clipping plane
#define SHADOW_CLIP_FAR           (140.0f)             // far clipping plane
#define SHADOW_DETAIL_X           (0.018f)             // view size of the shadow map (higher = sharper, but more artifacts)
#define SHADOW_DETAIL_Y           (0.016f)             // same for Y
#define SHADOW_HEIGHT_FACTOR      (2.5f)               // height correction factor (to reduce shadow length)

#define SHADOW_HANDLES            (3u)                 // number of handles for shader-programs with shadow maps
#define SHADOW_HANDLE_OUTDOOR     (0u)                 // outdoor-surface handle
#define SHADOW_HANDLE_OBJECT      (1u)                 // single object handle
#define SHADOW_HANDLE_OBJECT_INST (2u)                 // instanced object handle
#define SHADOW_SHADER_MATRIX      "u_m4ShadowMatrix"   // name of the shadow matrix uniform (transformation)


// ****************************************************************
// shadow map class
class cShadow final : public cBindContainer, public coreResourceRelation
{
private:
    coreFrameBuffer m_iFrameBuffer;                     // shadow map frame buffer (depth only)
    coreUint8 m_iLevel;                                 // current configuration level

    static cBindContainer s_GlobalContainer;            // 

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

    // access global bind container
    static inline cBindContainer* GetGlobalContainer() {return &s_GlobalContainer;}

    // recompile shader-programs with shadow maps
    static void Recompile();

    // 
    static void EnableShadowRead(const coreUintW& iProgramHandle);
    static inline void SendTransformSingle   (const coreMatrix4& mTransform) {cShadow::__SendTransform(s_pProgramSingle,    mTransform);}
    static inline void SendTransformInstanced(const coreMatrix4& mTransform) {cShadow::__SendTransform(s_pProgramInstanced, mTransform);}

    // 
    static inline const coreProgramPtr& GetProgramSingle   () {return s_pProgramSingle;}
    static inline const coreProgramPtr& GetProgramInstanced() {return s_pProgramInstanced;}


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;

    // 
    static void __SendTransform(const coreProgramPtr& pProgram, const coreMatrix4& mTransform);

    // render shadow-casting objects
    static void __RenderSingle   (const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList, const std::vector<coreObject3D*>& apObject);
    static void __RenderInstanced(const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList);
};


#endif // _P1_GUARD_SHADOW_H_