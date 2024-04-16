///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cPostProcessing::cPostProcessing()noexcept
: m_fSplitScreenValue (0.0f)
, m_bSplitScreen      (false)
, m_vDirectionConfig  (coreVector2(0.0f,1.0f))
, m_vDirectionGame    (coreVector2(0.0f,1.0f))
, m_afOffset          {}
, m_bOffsetActive     (false)
{
    // load post-processing shader-programs
    m_pProgramSimple    = Core::Manager::Resource->Get<coreProgram>("full_post_program");
    m_pProgramDistorted = Core::Manager::Resource->Get<coreProgram>("full_post_distorted_program");
    m_pProgramDebug     = Core::Manager::Resource->Get<coreProgram>("full_post_debug_program");
    this->Recompile();

    // create interiors
    this->UpdateLayout();

    // create wallpapers
    for(coreUintW i = 0u; i < POST_WALLS; ++i)
    {
        m_aWall[i].DefineProgram("menu_border_direct_program");
        m_aWall[i].DefineTexture(0u, "menu_background_black.png");
    }
    this->__UpdateWall();

    // create separator
    m_Separator.DefineProgram("default_2d_program");
    m_Separator.DefineTexture(0u, "default_white.png");
    m_Separator.SetColor4    (coreVector4(0.05f,0.05f,0.05f,0.0f));

    // 
    this->SetWallOpacity  (0.0f);
    this->SetSaturationAll(1.0f);
    this->SetValueAll     (1.0f);
    this->SetBorderAll    (0.0f);
}


// ****************************************************************
// render post-processing
void cPostProcessing::Render()
{
    // switch back to default frame buffer
    coreFrameBuffer::EndDraw();

    // select between debug, distorted or simple shader-program
                    if(g_bDebugOutput) this->DefineProgram(m_pProgramDebug);
    else if(g_pDistortion->IsActive()) this->DefineProgram(m_pProgramDistorted);
                                  else this->DefineProgram(m_pProgramSimple);

    // bind all required frame buffers
    for(coreUintW i = 0u; i < POST_INTERIORS; ++i)
    {
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_ENVIRONMENT, g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture);
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_FOREGROUND,  g_pForeground ->GetFrameBuffer()->GetColorTarget(0u).pTexture);
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_GLOW,        g_pGlow       ->GetFrameBuffer()->GetColorTarget(0u).pTexture);
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_DISTORTION,  g_pDistortion ->GetFrameBuffer()->GetColorTarget(0u).pTexture);
    }

    glDisable(GL_BLEND);
    {
        // render interiors (post-process)
        for(coreUintW i = 0u; i < POST_INTERIORS; ++i)
            m_aInterior[i].Render(this->GetProgram());

        // render wallpapers
        for(coreUintW i = m_bOffsetActive ? 0u : POST_WALLS_BASE; i < POST_WALLS; ++i)
            m_aWall[i].Render();
    }
    glEnable(GL_BLEND);

    // render separator
    if(m_fSplitScreenValue)
        m_Separator.Render();

    // 
    for(coreUintW i = 0u; i < POST_INTERIORS; ++i)
    {
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_ENVIRONMENT, NULL);
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_FOREGROUND,  NULL);
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_GLOW,        NULL);
        m_aInterior[i].DefineTexture(POST_TEXTURE_UNIT_DISTORTION,  NULL);
    }

    // invalidate all frame buffers (# not cGlow and cDistortion, because of incremental rendering and pause)
    if(!g_pMenu->IsPaused())
    {
        g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture->Invalidate(0u);
        g_pForeground ->GetFrameBuffer()->GetColorTarget(0u).pTexture->Invalidate(0u);
    }
}


// ****************************************************************
// move post-processing
void cPostProcessing::Move()
{
    // 
    if(m_bSplitScreen) m_fSplitScreenValue.UpdateMin( 3.0f, 1.0f);
                  else m_fSplitScreenValue.UpdateMax(-3.0f, 0.0f);

    // update interiors
    this->__UpdateInterior();

    // update separator
    this->__UpdateSeparator();

    if(m_bOffsetActive)
    {
        // update wallpapers
        this->__UpdateWall();

        // 
        if(std::all_of(m_afOffset, m_afOffset + POST_WALLS, [](const coreFloat x) {return coreMath::IsNear(x, 0.0f);}))
            m_bOffsetActive = false;
    }
}


// ****************************************************************
// recompile post-processing shader-programs
void cPostProcessing::Recompile()
{
#if !defined(CONFIG_FORCE)

    const coreChar* pcConfig1 =               g_CurConfig.Graphics.iGlow       ? SHADER_GLOW       : "";
    const coreChar* pcConfig2 = PRINT("%s%s", g_CurConfig.Graphics.iDistortion ? SHADER_DISTORTION : "", pcConfig1);
    const coreChar* pcConfig3 = PRINT("%s%s",                                    SHADER_DEBUG,           pcConfig2);

    // change configuration of post-processing shaders
    d_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post.frag")          ->GetRawResource())->SetCustomCode(pcConfig1);
    d_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post_distorted.frag")->GetRawResource())->SetCustomCode(pcConfig2);
    d_cast<coreShader*>(Core::Manager::Resource->Get<coreShader>("full_post_debug.frag")    ->GetRawResource())->SetCustomCode(pcConfig3);

    // recompile and relink
    m_pProgramSimple   .GetHandle()->Reload();
    m_pProgramDistorted.GetHandle()->Reload();
    m_pProgramDebug    .GetHandle()->Reload();

    // finish now
    if(Core::System->GetCurFrame())
    {
        coreSync::Finish();
        Core::Manager::Resource->UpdateResources();
    }

#endif
}


// ****************************************************************
// 
void cPostProcessing::UpdateLayout()
{
    // 
    const coreVector2 vBase = coreVector2(g_CurConfig.Game.iMirrorMode ? -1.0f : 1.0f, 1.0f);
    this->SetSize(vBase * (I_TO_F(MIN(g_CurConfig.Game.iGameScale, 100u)) / 100.0f));

    // 
    switch(g_CurConfig.Game.iGameRotation)
    {
    default: m_vDirectionConfig = coreVector2( 0.0f, 1.0f); break;
    case 1u: m_vDirectionConfig = coreVector2(-1.0f, 0.0f); break;
    case 2u: m_vDirectionConfig = coreVector2( 0.0f,-1.0f); break;
    case 3u: m_vDirectionConfig = coreVector2( 1.0f, 0.0f); break;
    }

    // 
    this->__UpdateInterior();
}


// ****************************************************************
// set wallpaper opacity
void cPostProcessing::SetWallOpacity(const coreFloat fOpacity)
{
    // change color instead of transparency (blending is disabled)
    ASSERT((fOpacity >= 0.0f) && (fOpacity <= 1.0f))
    const coreVector3 vColor = coreVector3(1.0f,1.0f,1.0f) * fOpacity;

    // 
    for(coreUintW i = 0u; i < POST_WALLS; ++i)
        m_aWall[i].SetColor3(vColor);
}


// ****************************************************************
// reset with the resource manager
void cPostProcessing::__Reset(const coreResourceReset eInit)
{
    if(eInit) this->__UpdateWall();
}


// ****************************************************************
// update interiors
void cPostProcessing::__UpdateInterior()
{
    // 
    this->SetDirection(MapToAxis(m_vDirectionConfig, m_vDirectionGame));

    if(m_bSplitScreen)
    {
        // 
        for(coreUintW i = 1u; i < POST_INTERIORS; ++i)
        {
            m_aInterior[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }

        // 
        for(coreUintW i = 0u; i < POST_INTERIORS; ++i)
        {
            const coreVector2 vOffset = this->GetDirection().Rotated90().InvertedY() * (this->GetSize().x * (i ? 0.25f : -0.25f));

            m_aInterior[i].SetPosition (this->GetPosition () + vOffset);
            m_aInterior[i].SetSize     (this->GetSize     () * coreVector2(0.5f,1.0f));
            m_aInterior[i].SetDirection(this->GetDirection());
            m_aInterior[i].SetTexSize  (coreVector2(0.5f,1.0f));
            m_aInterior[i].SetTexOffset(coreVector2(i ? 0.5f : 0.0f, 0.0f));
            m_aInterior[i].Move();
        }
    }
    else
    {
        // 
        for(coreUintW i = 1u; i < POST_INTERIORS; ++i)
        {
            m_aInterior[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }

        // 
        m_aInterior[0].SetPosition (this->GetPosition ());
        m_aInterior[0].SetSize     (this->GetSize     ());
        m_aInterior[0].SetDirection(this->GetDirection());
        m_aInterior[0].SetTexSize  (coreVector2(1.0f,1.0f));
        m_aInterior[0].SetTexOffset(coreVector2(0.0f,0.0f));
        m_aInterior[0].Move();
    }
}


// ****************************************************************
// update wallpapers
void cPostProcessing::__UpdateWall()
{
    // place objects left-right or top-down depending on window aspect ratio
    const coreVector2 vResolution = Core::System->GetResolution();
    const coreVector2 vSize       = coreVector2(1.0f, ((vResolution - g_vGameResolution) / vResolution.yx()).Max() * 0.5f) + 0.1f;
    const coreVector2 vFlip       = IsHorizontal(vResolution) ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,1.0f);
    const coreUintW   iAdd        = IsHorizontal(vResolution) ? POST_WALLS_BASE        : 0u;

    // 
    for(coreUintW i = 0u; i < POST_WALLS; ++i)
    {
        const coreVector2 vTurn = ((i < 2u) ? vFlip.yx() : vFlip) * ((i % 2u) ? 1.0f : -1.0f);
        const coreFloat   fMove = ((i < 2u) ? vSize.y    :  0.1f) - m_afOffset[(i + iAdd) % POST_WALLS];

        m_aWall[i].SetPosition (vTurn *  fMove);
        m_aWall[i].SetSize     (vSize);
        m_aWall[i].SetDirection(vTurn *  coreVector2(-1.0f,1.0f));
        m_aWall[i].SetCenter   (vTurn *  0.5f);
        m_aWall[i].SetAlignment(vTurn * -1.0f);
        m_aWall[i].Move();
    }
}


// ****************************************************************
// update separator
void cPostProcessing::__UpdateSeparator()
{
    if(m_fSplitScreenValue)
    {
        // 
        const coreFloat fValue = (m_fSplitScreenValue == 1.0f) ? 1.0f : LERPB(0.0f, 1.0f, m_fSplitScreenValue);

        // 
        m_Separator.SetSize     (coreVector2(LERP(0.1f, 0.01f, fValue), 1.0f));
        m_Separator.SetDirection(this->GetDirection());
        m_Separator.SetAlpha    (LERP(0.0f, 0.8f, fValue));
        m_Separator.Move();
    }
}