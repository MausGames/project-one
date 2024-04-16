///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

cBindContainer cShadow::s_GlobalContainer          = {};
coreProgramPtr cShadow::s_pProgramSingle           = NULL;
coreProgramPtr cShadow::s_pProgramInstanced        = NULL;
coreProgramPtr cShadow::s_apHandle[SHADOW_HANDLES] = {};
coreMatrix4    cShadow::s_amDrawShadowMatrix[2]    = {coreMatrix4::Identity(), coreMatrix4::Identity()};
coreMatrix4    cShadow::s_mReadShadowMatrix        =  coreMatrix4::Identity();


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
    m_FrameBuffer.StartDraw();
    m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR | CORE_FRAMEBUFFER_TARGET_DEPTH);
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        {
            glColorMask(false, false, false, false);
            {
                // render all shadow-casting objects (from the background)
                cShadow::RenderInstanced(s_amDrawShadowMatrix[0], this->GetListSet());
                cShadow::RenderSingle   (s_amDrawShadowMatrix[0], this->GetListSet(), this->GetObjectSet());
            }
            glColorMask(true, true, true, true);

            if(!s_GlobalContainer.IsEmpty())
            {
                glDisable(GL_DEPTH_TEST);
                {
                    // render all shadow-casting objects (from the foreground)
                    cShadow::RenderSingle   (s_amDrawShadowMatrix[1], s_GlobalContainer.GetListSet(), s_GlobalContainer.GetObjectSet());
                    cShadow::RenderInstanced(s_amDrawShadowMatrix[1], s_GlobalContainer.GetListSet());

                    // reduce current view frustum
                    Core::Graphics->SetView(m_FrameBuffer.GetResolution() * SHADOW_TEST_FACTOR, m_FrameBuffer.GetFOV(), m_FrameBuffer.GetNearClip(), m_FrameBuffer.GetFarClip());

                    // 
                    Core::Graphics->StartConservativeRaster();
                    {
                        // render low-resolution test area (for improved shading performance)
                        cShadow::RenderInstanced(s_amDrawShadowMatrix[1], s_GlobalContainer.GetListSet());
                        cShadow::RenderSingle   (s_amDrawShadowMatrix[1], s_GlobalContainer.GetListSet(), s_GlobalContainer.GetObjectSet());
                    }
                    Core::Graphics->EndConservativeRaster();
                }
                glEnable(GL_DEPTH_TEST);
            }
        }
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    // enable shadow map
    m_FrameBuffer.GetColorTarget(0u).pTexture->Enable(3u);
    m_FrameBuffer.GetDepthTarget()  .pTexture->Enable(CORE_TEXTURE_SHADOW + 0u);
}


// ****************************************************************
// adjust to current configuration
void cShadow::Reconfigure()
{
    if(m_iLevel == g_CurConfig.Graphics.iShadow) return;
    m_iLevel = g_CurConfig.Graphics.iShadow;

    // 
    if(!cShadow::IsSupported()) m_iLevel = 0u;

    // delete old shadow map
    m_FrameBuffer.Delete();

    if(m_iLevel)
    {
        const coreTextureSpec oSpec = CORE_GL_SUPPORT(ARB_texture_rg) ? CORE_TEXTURE_SPEC_R8 : CORE_TEXTURE_SPEC_RGB8;
        const coreVector2     vRes  = g_vGameResolution * ((m_iLevel == 1u) ? SHADOW_RES_LOW : SHADOW_RES_HIGH) * ENVIRONMENT_SCALE_FACTOR;

        // create shadow map frame buffer
        m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, oSpec);
        m_FrameBuffer.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_DEPTH, 0u, CORE_TEXTURE_SPEC_DEPTH16);
        m_FrameBuffer.Create(vRes, CORE_FRAMEBUFFER_CREATE_NORMAL);

        // enable depth value comparison
        m_FrameBuffer.GetDepthTarget().pTexture->EnableShadowSampling();

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

    // load shader-programs for shadow-receiving objects
    s_apHandle[SHADOW_HANDLE_OUTDOOR]            = Core::Manager::Resource->Get<coreProgram>("environment_outdoor_program");
    s_apHandle[SHADOW_HANDLE_OUTDOOR_GLOW]       = Core::Manager::Resource->Get<coreProgram>("environment_outdoor_glow_program");
    s_apHandle[SHADOW_HANDLE_OUTDOOR_LIGHT]      = Core::Manager::Resource->Get<coreProgram>("environment_outdoor_light_program");
    s_apHandle[SHADOW_HANDLE_OUTDOOR_LIGHT_GLOW] = Core::Manager::Resource->Get<coreProgram>("environment_outdoor_light_glow_program");
    s_apHandle[SHADOW_HANDLE_OBJECT]             = Core::Manager::Resource->Get<coreProgram>("object_ground_program");
    s_apHandle[SHADOW_HANDLE_OBJECT_INST]        = Core::Manager::Resource->Get<coreProgram>("object_ground_inst_program");
    s_apHandle[SHADOW_HANDLE_OBJECT_WAVE]        = Core::Manager::Resource->Get<coreProgram>("object_wave_program");
    s_apHandle[SHADOW_HANDLE_OBJECT_WAVE_INST]   = Core::Manager::Resource->Get<coreProgram>("object_wave_inst_program");

    // adjust shader-programs
    cShadow::Recompile();
}


// ****************************************************************
// exit the shadow map class
void cShadow::GlobalExit()
{
    // unload all shader-programs
    s_pProgramSingle    = NULL;
    s_pProgramInstanced = NULL;
    for(coreUintW i = 0u; i < SHADOW_HANDLES; ++i) s_apHandle[i] = NULL;
}


// ****************************************************************
// update the shadow map class
void cShadow::GlobalUpdate()
{
    if(!g_CurConfig.Graphics.iShadow || !cShadow::IsSupported()) return;

    // create orthographic projection
    constexpr coreMatrix4 mOrtho = coreMatrix4::Ortho(-SHADOW_RANGE_X,   SHADOW_RANGE_X,
                                                      -SHADOW_RANGE_Y,   SHADOW_RANGE_Y,
                                                       SHADOW_CLIP_NEAR, SHADOW_CLIP_FAR);

    // create movement compensation for foreground objects
    const coreVector3 P = g_pEnvironment->GetCameraPos();
    const coreVector2 D = g_pEnvironment->GetDirection();
    const coreMatrix4 mMove = coreMatrix4( D.y,  D.x, 0.0f, 0.0f,
                                          -D.x,  D.y, 0.0f, 0.0f,
                                          0.0f, 0.0f, 1.0f, 0.0f,
                                           P.x,  P.y, 0.0f, 1.0f);

    // create texture coordinate adjustment
    constexpr coreMatrix4 mNorm = coreMatrix4(0.5f, 0.0f, 0.0f, 0.0f,
                                              0.0f, 0.5f, 0.0f, 0.0f,
                                              0.0f, 0.0f, 0.5f, 0.0f,
                                              0.5f, 0.5f, 0.5f, 1.0f);

    // increase light direction height (to reduce shadow length)
    const coreVector3 vHighLight = (g_pEnvironment->GetLightDir() * coreVector3(1.0f, 1.0f, SHADOW_HEIGHT_FACTOR)).Normalized();

    // clamp camera movement (to reduce flickering)
    const coreFloat   fClampFactor = g_vGameResolution.x * ((g_CurConfig.Graphics.iShadow == 1u) ? SHADOW_RES_LOW : SHADOW_RES_HIGH) * ENVIRONMENT_SCALE_FACTOR * 0.007274395f;
    const coreVector2 vClampPos    = (g_pEnvironment->GetCameraPos().xy() * fClampFactor).Processed(FLOOR) * RCP(fClampFactor);

    // assemble camera matrix (viewed from light source)
    const coreMatrix4 mCamera = coreMatrix4::Camera(vHighLight * -SHADOW_VIEW_DISTANCE + coreVector3(vClampPos, WATER_HEIGHT),
                                                    vHighLight, coreVector3(g_pEnvironment->GetDirection().InvertedX(), 0.0f));

    // calculate full draw and read shadow matrices
    s_amDrawShadowMatrix[0] = mCamera * mOrtho;
    s_amDrawShadowMatrix[1] = mMove * s_amDrawShadowMatrix[0];
    s_mReadShadowMatrix     = s_amDrawShadowMatrix[0] * mNorm;
}


// ****************************************************************
// recompile shader-programs for shadow-receiving objects
void cShadow::Recompile()
{
    for(coreUintW i = 0u; i < SHADOW_HANDLES; ++i)
    {
        coreProgramPtr& pHandle  = s_apHandle[i];
        const coreChar* pcConfig = PRINT("%s%s%s%s%s%s", PRINT(SHADER_SHADOW(%u), cShadow::IsSupported() ? g_CurConfig.Graphics.iShadow : 0u), CORE_SHADER_OPTION_VIEWDIR,
                                                         (i == SHADOW_HANDLE_OBJECT_INST   || i == SHADOW_HANDLE_OBJECT_WAVE_INST)   ? CORE_SHADER_OPTION_INSTANCING : "",
                                                         (i == SHADOW_HANDLE_OBJECT_WAVE   || i == SHADOW_HANDLE_OBJECT_WAVE_INST)   ? SHADER_WAVE                   : "",
                                                         (i == SHADOW_HANDLE_OUTDOOR_GLOW  || i == SHADOW_HANDLE_OUTDOOR_LIGHT_GLOW) ? SHADER_GLOW                   : "",
                                                         (i == SHADOW_HANDLE_OUTDOOR_LIGHT || i == SHADOW_HANDLE_OUTDOOR_LIGHT_GLOW) ? SHADER_LIGHT                  : "");

        // change configuration of related shaders
        for(coreUintW j = 0u, je = pHandle->GetNumShaders(); j < je; ++j)
        {
            pHandle->GetShaderHandle(j)->UpdateStart();
            pHandle->GetShader(j)->SetCustomCode(pcConfig);
            pHandle->GetShaderHandle(j)->UpdateEnd();
        }

        // recompile and relink
        pHandle.GetHandle()->Reload();
    }

    // finish now
    if(Core::System->GetCurFrame())
    {
        coreSync::Finish();
        Core::Manager::Resource->UpdateResources();
    }
}


// ****************************************************************
// render single shadow-casting objects
void cShadow::RenderSingle(const coreMatrix4& mTransform, const coreList<coreBatchList*>& apList, const coreList<coreObject3D*>& apObject)
{
    // only enable and update without instancing
    if(!apObject.empty() || std::any_of(apList.begin(), apList.end(), [](const coreBatchList* pList) {return !pList->IsInstanced();}))
    {
        // send draw shadow matrix to single shader-program
        cShadow::__SendTransform(s_pProgramSingle, mTransform);

        // render single objects
        FOR_EACH(it, apObject) (*it)->Render(s_pProgramSingle, false);
        FOR_EACH(it, apList)   {if(!(*it)->IsInstanced()) (*it)->Render(NULL, s_pProgramSingle, false);}
    }
}

void cShadow::RenderSingle(const coreMatrix4& mTransform, const coreList<coreBatchList*>& apList)
{
    // 
    const coreList<coreObject3D*> apEmpty;
    cShadow::RenderSingle(mTransform, apList, apEmpty);
}


// ****************************************************************
// render lists with shadow-casting objects
void cShadow::RenderInstanced(const coreMatrix4& mTransform, const coreList<coreBatchList*>& apList)
{
    // only enable and update on instancing
    if(std::any_of(apList.begin(), apList.end(), [](const coreBatchList* pList) {return pList->IsInstanced();}))
    {
        // send draw shadow matrix to instanced shader-program
        cShadow::__SendTransform(s_pProgramInstanced, mTransform);

        // render lists with objects
        FOR_EACH(it, apList) {if((*it)->IsInstanced()) (*it)->Render(s_pProgramInstanced, NULL, false);}
    }
}


// ****************************************************************
// enable shader-program and apply read shadow matrix
void cShadow::EnableShadowRead(const coreUintW iHandleIndex)
{
    if(!g_CurConfig.Graphics.iShadow || !cShadow::IsSupported()) return;

    ASSERT(iHandleIndex < ARRAY_SIZE(s_apHandle))
    cShadow::__SendTransform(s_apHandle[iHandleIndex], s_mReadShadowMatrix);
}


// ****************************************************************
// reset with the resource manager
void cShadow::__Reset(const coreResourceReset eInit)
{
    if(eInit) {m_iLevel = 0xFFu; this->Reconfigure();}
         else {m_FrameBuffer.Delete();}
}


// ****************************************************************
// send transformation matrix to shader-program
void cShadow::__SendTransform(const coreProgramPtr& pProgram, const coreMatrix4& mTransform)
{
    if(!pProgram.IsUsable()) return;
    if(!pProgram->Enable())  return;
    pProgram->SendUniform("u_m4ShadowMatrix", mTransform, false);
}