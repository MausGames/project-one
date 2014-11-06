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

// NOTE: all global objects are on the foreground


// ****************************************************************
// shadow definitions
#define SHADOW_VIEW_ORIENTATION  coreVector3(0.0f, 0.624694824f, 0.780868530f)   // view orientation (not direction)
#define SHADOW_VIEW_DISTANCE     (90.0f)                                         // distance from origin to light source
#define SHADOW_CLIP_NEAR         (20.0f)                                         // near clipping plane
#define SHADOW_CLIP_FAR          (140.0f)                                        // far clipping plane
#define SHADOW_DETAIL_X          (0.014f)                                        // view size of the shadow map (higher = sharper, but more artifacts)
#define SHADOW_DETAIL_Y          (0.013f)                                        // same for Y
                                                  
#define SHADOW_HANDLES                   (5u)                 // number of handles for shader-programs with shadow maps
#define SHADOW_HANDLE_OUTDOOR            (0u)                 // outdoor-surface handle
#define SHADOW_HANDLE_OBJECT             (1u)                 // single object handle
#define SHADOW_HANDLE_OBJECT_INST        (2u)                 // instanced object handle
#define SHADOW_HANDLE_OBJECT_SIMPLE      (3u)                 // single simple object handle (simple = no normal mapping)
#define SHADOW_HANDLE_OBJECT_SIMPLE_INST (4u)                 // instanced simple object handle
#define SHADOW_SHADER_MATRIX             "u_m4ShadowMatrix"   // name of the shadow matrix uniform (transformation)


// ****************************************************************
// shadow map class
class cShadow final : coreResourceRelation
{
private:
    coreFrameBuffer m_iFrameBuffer;                        // shadow map frame buffer (depth only)
    coreByte m_iLevel;                                     // current configuration level

    std::vector<coreObject3D*>  m_apObject;                // shadow-casting objects (per frame buffer)
    std::vector<coreBatchList*> m_apList;                  // lists with shadow-casting objects

    static std::vector<coreObject3D*>  s_apGlobalObject;   // global shadow-casting objects (in all frame buffers)
    static std::vector<coreBatchList*> s_apGlobalList;     // global lists with shadow-casting objects

    static coreProgramPtr s_pProgramSingle;                // shader-program for single shadow-casting objects
    static coreProgramPtr s_pProgramInstanced;             // shader-program for instanced shadow-casting objects
    static coreProgramPtr s_apHandle[SHADOW_HANDLES];      // handles for shader-programs with shadow maps

    static coreMatrix4 s_amDrawShadowMatrix[2];            // matrices used for view transformation of depth values (0 = background, 1 = foreground)
    static coreMatrix4 s_mReadShadowMatrix;                // matrix with additional coordinate adjustment


public:
    cShadow()noexcept;
    ~cShadow();

    // update the shadow map
    void Update();

    // manage shadow-casting objects
    inline void BindObject  (coreObject3D* pObject) {m_apObject.push_back(pObject);}
    inline void UnbindObject(coreObject3D* pObject) {FOR_EACH(it, m_apObject) {if((*it) == pObject) {m_apObject.erase(it); return;}} ASSERT(false)}
    inline void ClearObjects()                      {m_apObject.clear();}

    // manage lists with shadow-casting objects
    inline void BindList  (coreBatchList* pList) {m_apList.push_back(pList);}
    inline void UnbindList(coreBatchList* pList) {FOR_EACH(it, m_apList) {if((*it) == pList) {m_apList.erase(it); return;}} ASSERT(false)}
    inline void ClearLists()                     {m_apList.clear();}

    // adjust to current configuration
    void Reconfigure();

    // access frame buffer object
    inline coreFrameBuffer* GetFrameBuffer() {return &m_iFrameBuffer;}

    // control the shadow map class
    static void GlobalInit();
    static void GlobalExit();
    static void GlobalUpdate();

    // manage global shadow-casting objects
    static inline void BindGlobalObject  (coreObject3D* pObject) {s_apGlobalObject.push_back(pObject);}
    static inline void UnbindGlobalObject(coreObject3D* pObject) {FOR_EACH(it, s_apGlobalObject) {if((*it) == pObject) {s_apGlobalObject.erase(it); return;}} ASSERT(false)}
    static inline void ClearGlobalObjects()                      {s_apGlobalObject.clear();}

    // manage global lists with shadow-casting objects
    static inline void BindGlobalList  (coreBatchList* pList) {s_apGlobalList.push_back(pList);}
    static inline void UnbindGlobalList(coreBatchList* pList) {FOR_EACH(it, s_apGlobalList) {if((*it) == pList) {s_apGlobalList.erase(it); return;}} ASSERT(false)}
    static inline void ClearGlobalLists()                     {s_apGlobalList.clear();}

    // recompile shader-programs with shadow maps
    static void Recompile();

    // enable shader-program and apply read shadow matrix
    static void EnableShadowRead(const coreByte& iProgramHandle);

    // render depth pass with foreground objects
    static void RenderForegroundDepth();


private:
    DISABLE_COPY(cShadow)

    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;

    // render shadow-casting objects
    static void __RenderSingle   (const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList, const std::vector<coreObject3D*>& apObject);
    static void __RenderInstanced(const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList);
};


#endif // _P1_GUARD_SHADOW_H_