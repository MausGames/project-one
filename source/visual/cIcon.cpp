///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cIcon::cIcon()noexcept
: m_Range      (ICON_VARIATIONS)
, m_iIndex     (0u)
, m_fAnimation (0.0f)
, m_fStorage   (-1.0f)
, m_bDirty     (true)
{
    // 
    m_FrameBuffer.AttachTargetBuffer(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_FrameBuffer.Create(g_vGameResolution * ICON_SCALE_VECTOR, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED);

    // 
    m_ResolvedTexture.AttachTargetTexture(CORE_FRAMEBUFFER_TARGET_COLOR, 0u, CORE_TEXTURE_SPEC_RGBA8);
    m_ResolvedTexture.Create(g_vGameResolution * ICON_SCALE_VECTOR, CORE_FRAMEBUFFER_CREATE_NORMAL);

    // 
    m_Range.DefineProgram("effect_energy_flat_invert_inst_program");
    {
        for(coreUintW i = 0u; i < ICON_VARIATIONS; ++i)
        {
            // 
            coreObject3D* pRange = &m_aRangeRaw[i];
            pRange->DefineTexture(0u, "effect_energy.png");
            pRange->DefineProgram("effect_energy_flat_invert_program");

            // 
            pRange->SetPosition (coreVector3(1.0f,0.0f,0.0f) * 91.5f * (I_TO_F(i) - I_TO_F(ICON_VARIATIONS - 1u) * 0.5f));
            pRange->SetDirection(coreVector3(0.0f,1.0f,0.0f));
            pRange->SetTexSize  (coreVector2(0.1f,0.1f));

            // 
            m_Range.BindObject(pRange);
        }
    }

    // 
    m_pDefaultModel = Core::Manager::Resource->Get<coreModel>("object_cube_top.md3");
}


// ****************************************************************
// 
void cIcon::Update()
{
    coreFloat fScale = 1.0f;
    if(STATIC_ISVALID(g_pGame))
    {
        const cPlayer* pPlayer = g_pGame->GetPlayer(m_iIndex);
        
        if(m_aRangeRaw[0].GetModel().GetHandle() != pPlayer->GetRangeModel().GetHandle()) m_bDirty = true;

        // 
        for(coreUintW i = 0u; i < ICON_VARIATIONS; ++i)
        {
            m_aRangeRaw[i].DefineModel(pPlayer->GetRangeModel ());
            m_aRangeRaw[i].SetColor3  (pPlayer->GetEnergyColor().Processed(MIN, 1.0f));
        }

        fScale = pPlayer->GetRangeScale();
    }
    else
    {
        if(m_aRangeRaw[0].GetModel().GetHandle() != m_pDefaultModel.GetHandle()) m_bDirty = true;
        
        // 
        for(coreUintW i = 0u; i < ICON_VARIATIONS; ++i)
        {
            m_aRangeRaw[i].DefineModel(m_pDefaultModel);
            m_aRangeRaw[i].SetColor3  (COLOR_PLAYER_BLUE.Processed(MIN, 1.0f));
        }
    }

    // 
    if(!m_aRangeRaw[0].GetModel().IsUsable()) return;

    if(g_pMenu->IsPaused() && (m_fStorage < 0.0f)) m_fStorage = m_fAnimation;
    if(!g_pMenu->IsPaused() && (m_fStorage >= 0.0f)) {m_fAnimation = m_fStorage; m_fStorage = -1.0f;}
    
    // 
    m_fAnimation.UpdateMod(0.1f, 1.0f);
    
    if(g_CurConfig.Graphics.iRender == 0u)
    {
        if(Core::Manager::Resource->IsLoading()) return;
        if(!m_bDirty) return;
        m_bDirty = false;
    }
    

    // 
    m_FrameBuffer.StartDraw();
    m_FrameBuffer.Clear(CORE_FRAMEBUFFER_TARGET_COLOR);
    {
        glDisable(GL_DEPTH_TEST);
        {
            // 
            for(coreUintW i = 0u; i < ICON_VARIATIONS; ++i)
            {
                m_aRangeRaw[i].SetSize(coreVector3(1.0f,1.0f,1.0f) * 27.0f * fScale);
            }
            m_Range.MoveNormal();

            // 
            g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyList(&m_Range);

            // 
            for(coreUintW i = 0u; i < ICON_VARIATIONS; ++i)
            {
                m_aRangeRaw[i].SetSize     (coreVector3(1.0f,1.0f,1.0f) * 22.0f * fScale);
                m_aRangeRaw[i].SetTexOffset(coreVector2(0.0f,1.0f) * (((!i || !g_pMenu->IsPaused()) ? m_fAnimation.ToFloat() : m_fStorage) + (I_TO_F(i) / I_TO_F(ICON_VARIATIONS))));
            }
            m_Range.MoveNormal();

            // 
            m_Range.Render();
        }
        glEnable(GL_DEPTH_TEST);
    }

    // 
    m_FrameBuffer.Blit      (CORE_FRAMEBUFFER_TARGET_COLOR, &m_ResolvedTexture);
    m_FrameBuffer.Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
}


// ****************************************************************
// reset with the resource manager
void cIcon::__Reset(const coreResourceReset eInit)
{
    if(eInit) {m_FrameBuffer.Create(g_vGameResolution * ICON_SCALE_VECTOR, CORE_FRAMEBUFFER_CREATE_MULTISAMPLED); m_ResolvedTexture.Create(g_vGameResolution * ICON_SCALE_VECTOR, CORE_FRAMEBUFFER_CREATE_NORMAL); m_bDirty = true;}
         else {m_FrameBuffer.Delete();                                                                            m_ResolvedTexture.Delete();}
}