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


// ****************************************************************
// shadow definitions
#define SHADOW_DISTANCE  (30.0f)                  // distance from origin to light source
#define SHADOW_DETAIL    (0.014f)                 // used area of the shadow map (higher = sharper, but more artifacts)
#define SHADOW_CLIP_NEAR (-32.0f)                 // near clipping plane
#define SHADOW_CLIP_FAR  (128.0f)                 // far clipping plane
                                                  
#define SHADOW_HANDLES   (3u)                     // number of handles for shader-programs with shadow maps
                          
#define SHADOW_SHADER_MATRIX "u_m4ShadowMatrix"   // name of the shadow matrix uniform


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

    static coreMatrix4 s_mDrawShadowMatrix;                // matrix used for view transformation of depth values
    static coreMatrix4 s_mReadShadowMatrix;                // matrix with additional coordinate adjustment


public:
    cShadow()noexcept;
    ~cShadow();

    // update the shadow map
    void Update();

    // manage shadow objects
    inline void BindObject  (coreObject3D* pObject) {m_apObject.push_back(pObject);}
    inline void UnbindObject(coreObject3D* pObject) {FOR_EACH(it, m_apObject) {if((*it) == pObject) {m_apObject.erase(it); return;}} ASSERT(false)}
    inline void ClearObjects()                      {m_apObject.clear();}

    // manage lists with shadow objects
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

    // manage global shadow objects
    static inline void BindGlobalObject  (coreObject3D* pObject) {s_apGlobalObject.push_back(pObject);}
    static inline void UnbindGlobalObject(coreObject3D* pObject) {FOR_EACH(it, s_apGlobalObject) {if((*it) == pObject) {s_apGlobalObject.erase(it); return;}} ASSERT(false)}
    static inline void ClearGlobalObjects()                      {s_apGlobalObject.clear();}

    // manage global lists with shadow objects
    static inline void BindGlobalList  (coreBatchList* pList) {s_apGlobalList.push_back(pList);}
    static inline void UnbindGlobalList(coreBatchList* pList) {FOR_EACH(it, s_apGlobalList) {if((*it) == pList) {s_apGlobalList.erase(it); return;}} ASSERT(false)}
    static inline void ClearGlobalLists()                     {s_apGlobalList.clear();}

    // recompile shader-programs with shadow maps
    static void Recompile();

    // apply shadow matrix
    static void ApplyShadowMatrix(const coreProgramPtr& pProgram);


private:
    DISABLE_COPY(cShadow)

    // reset with the resource manager
    void __Reset(const coreResourceReset& bInit)override;
};


#endif // _P1_GUARD_SHADOW_H_