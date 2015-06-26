//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

coreProgramPtr cShadow::s_pProgramSingle               = NULL;
coreProgramPtr cShadow::s_pProgramInstanced            = NULL;
coreProgramPtr cShadow::s_apHandle[SHADOW_HANDLES]; // = NULL;
coreMatrix4    cShadow::s_amDrawShadowMatrix[2];    // = coreMatrix4::Identity();
coreMatrix4    cShadow::s_mReadShadowMatrix            = coreMatrix4::Identity();


// ****************************************************************
// constructor
cShadow::cShadow()noexcept
: m_iLevel (0xFFu)
{
    // create shadow map
    this->Reconfigure();
}


// ****************************************************************
// update the shadow map
void cShadow::Update()
{
    if(!m_iLevel) return;

    // fill the shadow map
    m_iFrameBuffer.StartDraw();
    m_iFrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_DEPTH);
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        {
            // render all lists with shadow-casting objects
            cShadow::__RenderInstanced(s_amDrawShadowMatrix[0], m_apList);
            cShadow::__RenderInstanced(s_amDrawShadowMatrix[1], s_apGlobalList);

            // render all single shadow-casting objects
            cShadow::__RenderSingle(s_amDrawShadowMatrix[0], m_apList,       m_apObject);
            cShadow::__RenderSingle(s_amDrawShadowMatrix[1], s_apGlobalList, s_apGlobalObject);
        }
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    // enable shadow map
    m_iFrameBuffer.GetDepthTarget().pTexture->Enable(CORE_TEXTURE_SHADOW + 0u);
}


// ****************************************************************
// adjust to current configuration
void cShadow::Reconfigure()
{
    if(m_iLevel == g_CurConfig.Graphics.iShadow) return;
    m_iLevel = g_CurConfig.Graphics.iShadow;

    // delete old shadow map
    m_iFrameBuffer.Delete();

    if(m_iLevel)
    {
        // create shadow map frame buffer
        m_iFrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH);
        m_iFrameBuffer.Create(g_vGameResolution * ((m_iLevel == 1u) ? 1.0f : 1.7f), CORE_FRAMEBUFFER_CREATE_NORMAL);

        // enable depth value comparison
        m_iFrameBuffer.GetDepthTarget().pTexture->ShadowSampling(true);

        // set polygon fill properties (to reduce projective aliasing, not in GlobalInit, because of easier engine-reset)
        glPolygonOffset(3.3f, 12.0f);
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
    s_apHandle[SHADOW_HANDLE_OUTDOOR]     = Core::Manager::Resource->Get<coreProgram>("environment_outdoor_program");
    s_apHandle[SHADOW_HANDLE_OBJECT]      = Core::Manager::Resource->Get<coreProgram>("object_ground_program");
    s_apHandle[SHADOW_HANDLE_OBJECT_INST] = Core::Manager::Resource->Get<coreProgram>("object_ground_inst_program");

    // adjust shader-programs
    cShadow::Recompile();
}


// ****************************************************************
// exit the shadow map class
void cShadow::GlobalExit()
{
    // remove all global objects and lists
    cGlobalBindContainer::_GlobalExit();

    // unload all shader-programs
    s_pProgramSingle    = NULL;
    s_pProgramInstanced = NULL;
    for(coreUintW i = 0u; i < SHADOW_HANDLES; ++i) s_apHandle[i] = NULL;
}


// ****************************************************************
// update the shadow map class
void cShadow::GlobalUpdate()
{
    if(!g_CurConfig.Graphics.iShadow) return;

    // define orthographic projection
    constexpr_var coreFloat A =          SHADOW_CLIP_FAR + SHADOW_CLIP_NEAR;
    constexpr_var coreFloat I = -1.0f / (SHADOW_CLIP_FAR - SHADOW_CLIP_NEAR);
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
    const coreMatrix4 mMove = coreMatrix4( D.y,  D.x, 0.0f, 0.0f,
                                          -D.x,  D.y, 0.0f, 0.0f,
                                          0.0f, 0.0f, 1.0f, 0.0f,
                                           P.x,  P.y, 0.0f, 1.0f);

    // increase light direction height (to reduce shadow length)
    const coreVector3 vHighLight = (g_pEnvironment->GetLightDir() * coreVector3(1.0f, 1.0f, SHADOW_HEIGHT_FACTOR)).Normalize();

    // calculate full draw and read shadow matrices
    s_amDrawShadowMatrix[0] = coreMatrix4::Camera(vHighLight * -SHADOW_VIEW_DISTANCE + coreVector3(g_pEnvironment->GetCameraPos().xy(), WATER_HEIGHT),
                                                  vHighLight, coreVector3(g_pEnvironment->GetDirection().InvertedX(), 0.0f)) * mOrtho;
    s_amDrawShadowMatrix[1] = mMove * s_amDrawShadowMatrix[0];
    s_mReadShadowMatrix     = s_amDrawShadowMatrix[0] * mNorm;
}


// ****************************************************************
// recompile shader-programs with shadow maps
void cShadow::Recompile()
{
    for(coreUintW i = 0u; i < SHADOW_HANDLES; ++i)
    {
        const coreChar* pcConfig = PRINT("%s %s", g_CurConfig.Graphics.iShadow     ? SHADER_SHADOW                 : "",
                                                  (i == SHADOW_HANDLE_OBJECT_INST) ? CORE_SHADER_OPTION_INSTANCING : "");

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
// enable shader-program and apply read shadow matrix
void cShadow::EnableShadowRead(const coreUintW& iProgramHandle)
{
    if(!g_CurConfig.Graphics.iShadow) return;

    // send read shadow matrix to shader-program
    ASSERT(iProgramHandle < ARRAY_SIZE(s_apHandle))
    cShadow::__SendTransform(s_apHandle[iProgramHandle], s_mReadShadowMatrix);
}


// ****************************************************************
// reset with the resource manager
void cShadow::__Reset(const coreResourceReset& bInit)
{
    if(bInit) {m_iLevel = 0xFFu; this->Reconfigure();}
         else m_iFrameBuffer.Delete();
}


// ****************************************************************
// 
void cShadow::__SendTransform(const coreProgramPtr& pProgram, const coreMatrix4& mTransform)
{
    // send transformation matrix to shader-program
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;
    pProgram->SendUniform(SHADOW_SHADER_MATRIX, mTransform, false);
}


// ****************************************************************
// render single shadow-casting objects
void cShadow::__RenderSingle(const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList, const std::vector<coreObject3D*>& apObject)
{
    // only enable and update without instancing
    if(!apObject.empty() || std::any_of(apList.begin(), apList.end(), [](const coreBatchList* pList) {return !pList->IsInstanced();}))
    {
        // send draw shadow matrix to single shader-program
        cShadow::__SendTransform(s_pProgramSingle, mTransform);

        // render single objects
        FOR_EACH(it, apObject) (*it)->Render(s_pProgramSingle);
        FOR_EACH(it, apList)   {if(!(*it)->IsInstanced()) (*it)->Render(NULL, s_pProgramSingle);}
    }
}


// ****************************************************************
// render lists with shadow-casting objects
void cShadow::__RenderInstanced(const coreMatrix4& mTransform, const std::vector<coreBatchList*>& apList)
{
    // only enable and update on instancing
    if(std::any_of(apList.begin(), apList.end(), [](const coreBatchList* pList) {return pList->IsInstanced();}))
    {
        // send draw shadow matrix to instanced shader-program
        cShadow::__SendTransform(s_pProgramInstanced, mTransform);

        // render lists with objects
        FOR_EACH(it, apList) {if((*it)->IsInstanced()) (*it)->Render(s_pProgramInstanced, NULL);}
    }
}