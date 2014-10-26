//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

std::vector<coreObject3D*>  cShadow::s_apGlobalObject;           // = NULL;
std::vector<coreBatchList*> cShadow::s_apGlobalList;             // = NULL;
coreProgramPtr              cShadow::s_pProgramSingle               = NULL;
coreProgramPtr              cShadow::s_pProgramInstanced            = NULL;
coreProgramPtr              cShadow::s_apHandle[SHADOW_HANDLES]; // = NULL;
coreMatrix4                 cShadow::s_amDrawShadowMatrix[2];    // = coreMatrix4::Identity();
coreMatrix4                 cShadow::s_mReadShadowMatrix            = coreMatrix4::Identity();


// ****************************************************************
// constructor
cShadow::cShadow()
: m_iLevel (0xFF)
{
    // create shadow map
    this->Reconfigure();
}


// ****************************************************************
// destructor
cShadow::~cShadow()
{
    ASSERT(m_apObject.empty() && m_apList.empty())

    // remove all shadow objects and lists
    this->ClearObjects();
    this->ClearLists();
}


// ****************************************************************
// update the shadow map
void cShadow::Update()
{
    if(!m_iLevel) return;

#if defined(_CORE_DEBUG_)

    // check for duplicate objects and lists
    FOR_EACH(it, m_apObject)       FOR_EACH_SET(et, it+1, m_apObject)       ASSERT((*it) != (*et))
    FOR_EACH(it, m_apList)         FOR_EACH_SET(et, it+1, m_apList)         ASSERT((*it) != (*et))
    FOR_EACH(it, s_apGlobalObject) FOR_EACH_SET(et, it+1, s_apGlobalObject) ASSERT((*it) != (*et))
    FOR_EACH(it, s_apGlobalList)   FOR_EACH_SET(et, it+1, s_apGlobalList)   ASSERT((*it) != (*et))

#endif

    // fill the shadow map
    m_iFrameBuffer.StartDraw();
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
    {
        // render all single shadow-casting objects
        cShadow::__RenderSingle(s_amDrawShadowMatrix[0], m_apList,       m_apObject);
        cShadow::__RenderSingle(s_amDrawShadowMatrix[1], s_apGlobalList, s_apGlobalObject);

        // render all lists with shadow-casting objects
        cShadow::__RenderInstanced(s_amDrawShadowMatrix[0], m_apList);
        cShadow::__RenderInstanced(s_amDrawShadowMatrix[1], s_apGlobalList);
    }

    // enable shadow map
    m_iFrameBuffer.GetDepthTarget().pTexture->Enable(CORE_TEXTURE_SHADOW + 0);
}


// ****************************************************************
// adjust to current configuration
void cShadow::Reconfigure()
{
    if(m_iLevel == g_CurConfig.iShadow) return;
    m_iLevel = g_CurConfig.iShadow;

    // delete old shadow map
    m_iFrameBuffer.Delete();

    if(m_iLevel)
    {
        // create shadow map frame buffer
        m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_DEPTH, 0, CORE_TEXTURE_SPEC_DEPTH);
        m_iFrameBuffer.Create(g_vGameResolution * ((m_iLevel == 1) ? 1.0f : 1.6f), CORE_FRAMEBUFFER_CREATE_NORMAL);

        // enable depth value comparison
        m_iFrameBuffer.GetDepthTarget().pTexture->ShadowSampling(true);
    }
}


// ****************************************************************
// init the shadow map class
void cShadow::GlobalInit()
{
    // load shader-programs for shadow-casting objects
    s_pProgramSingle    = Core::Manager::Resource->Get<coreProgram>("effect_shadow_program");
    s_pProgramInstanced = Core::Manager::Resource->Get<coreProgram>("effect_shadow_inst_program");

    // load shader-programs with shadow maps
    s_apHandle[0] = Core::Manager::Resource->Get<coreProgram>("environment_outdoor_program");
    s_apHandle[1] = Core::Manager::Resource->Get<coreProgram>("object_shadow_program");
    s_apHandle[2] = Core::Manager::Resource->Get<coreProgram>("object_inst_shadow_program");   // instancing-property handled in Recompile()

    // adjust shader-programs
    cShadow::Recompile();
}


// ****************************************************************
// exit the shadow map class
void cShadow::GlobalExit()
{
    ASSERT(s_apGlobalObject.empty() && s_apGlobalList.empty())

    // remove all global shadow-casting objects and lists
    cShadow::ClearGlobalObjects();
    cShadow::ClearGlobalLists();

    // unload all shader-programs
    s_pProgramSingle    = NULL;
    s_pProgramInstanced = NULL;
    for(int i = 0; i < SHADOW_HANDLES; ++i)
        s_apHandle[i] = NULL;
}


// ****************************************************************
// update the shadow map class
void cShadow::GlobalUpdate()
{
    if(!g_CurConfig.iShadow) return;

    // define orthographic projection
    constexpr_var float A =          SHADOW_CLIP_FAR + SHADOW_CLIP_NEAR;
    constexpr_var float I = -1.0f / (SHADOW_CLIP_FAR - SHADOW_CLIP_NEAR);
    constexpr_var coreMatrix4 mOrtho = coreMatrix4(SHADOW_DETAIL_X,            0.0f,   0.0f, 0.0f,
                                                              0.0f, SHADOW_DETAIL_Y,   0.0f, 0.0f,
                                                              0.0f,            0.0f, 2.0f*I, 0.0f,
                                                              0.0f,            0.0f,    A*I, 1.0f);

    // define additional coordinate adjustment
    constexpr_var coreMatrix4 mNorm = coreMatrix4(0.5f, 0.0f, 0.0f, 0.0f,
                                                  0.0f, 0.5f, 0.0f, 0.0f,
                                                  0.0f, 0.0f, 0.5f, 0.0f,
                                                  0.5f, 0.5f, 0.5f, 1.0f);

    // assemble additional movement matrix for foreground objects
    const coreVector3& P = g_pEnvironment->GetCameraPos();
    const coreVector2& D = g_pEnvironment->GetDirection();
    const coreMatrix4 mMove = coreMatrix4( D.y, -D.x, 0.0f, 0.0f,
                                           D.x,  D.y, 0.0f, 0.0f,
                                          0.0f, 0.0f, 1.0f, 0.0f,
                                           P.x,  P.y, 0.0f, 1.0f);

    // calculate high light direction (to reduce shadow length)
    const coreVector3 vHighLight = (g_pEnvironment->GetLightDir() * coreVector3(1.0f,1.0f,2.0f)).Normalize();

    // calculate full draw and read shadow matrices
    s_amDrawShadowMatrix[0] = coreMatrix4::Camera(vHighLight * -SHADOW_VIEW_DISTANCE + coreVector3(g_pEnvironment->GetCameraPos().xy(), WATER_HEIGHT) + coreVector3::Rand(SHADOW_JITTER), 
                                                  vHighLight, SHADOW_VIEW_ORIENTATION) * mOrtho;
    s_amDrawShadowMatrix[1] = mMove * s_amDrawShadowMatrix[0];
    s_mReadShadowMatrix     = s_amDrawShadowMatrix[0] * mNorm;
}


// ****************************************************************
// recompile shader-programs with shadow maps
void cShadow::Recompile()
{
    for(int i = 0; i < SHADOW_HANDLES; ++i)
    {
        const char* pcConfig = PRINT("%s %s", g_CurConfig.iShadow ? "#define _P1_SHADOW_ (1) \n"  : "",
                                              (i == 2)            ? CORE_SHADER_OPTION_INSTANCING : "");

        // change configuration of related shaders
        FOR_EACH(it, s_apHandle[i]->GetShader())
            (*it)->SetCustomCode(pcConfig);

        // recompile and relink
        s_apHandle[i].GetHandle()->Reload();
    }

    // finish now
    glFinish();
    Core::Manager::Resource->UpdateResources();
}


// ****************************************************************
// apply read shadow matrix
void cShadow::ApplyShadowMatrix(const coreProgramPtr& pProgram)
{
    if(!g_CurConfig.iShadow) return;

    // send read shadow matrix to shader-program
    pProgram->Enable();
    pProgram->SendUniform(SHADOW_SHADER_MATRIX, s_mReadShadowMatrix, false);
}


// ****************************************************************
// render depth pass with foreground objects
void cShadow::RenderForegroundDepth()
{
    // draw only into the currently active depth buffer
    glDrawBuffer(GL_NONE);
    {
        // render all global objects (foreground, instanced first)
        cShadow::__RenderInstanced(g_pForeground->GetViewProj(), s_apGlobalList);
        cShadow::__RenderSingle   (g_pForeground->GetViewProj(), s_apGlobalList, s_apGlobalObject);
    }
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}


// ****************************************************************
// reset with the resource manager
void cShadow::__Reset(const coreResourceReset& bInit)
{
    if(bInit) {m_iLevel = 0xFF; this->Reconfigure();}
         else m_iFrameBuffer.Delete();
}


// ****************************************************************
// render single shadow-casting objects
void cShadow::__RenderSingle(const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList, const std::vector<coreObject3D*>& apObject)
{
    if(apList.empty() && apObject.empty()) return;

    // send shadow matrix to single shader-program
    if(!s_pProgramSingle.IsUsable()) return;
    if(!s_pProgramSingle->Enable())  return;
    s_pProgramSingle->SendUniform(SHADOW_SHADER_MATRIX, mTransform, false);

    // render single objects
    FOR_EACH(it, apObject) (*it)->Render(s_pProgramSingle);
    FOR_EACH(it, apList)   {if(!(*it)->IsInstanced()) (*it)->Render(NULL, s_pProgramSingle);}
}


// ****************************************************************
// render lists with shadow-casting objects
void cShadow::__RenderInstanced(const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList)
{
    if(apList.empty()) return;

    // send shadow matrix to instanced shader-program
    if(!s_pProgramInstanced.IsUsable()) return;
    if(!s_pProgramInstanced->Enable())  return;
    s_pProgramInstanced->SendUniform(SHADOW_SHADER_MATRIX, mTransform, false);
   
    // render lists with objects
    FOR_EACH(it, apList) {if((*it)->IsInstanced()) (*it)->Render(s_pProgramInstanced, NULL);}
}