//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"

// ****************************************************************
// constructor
cPostProcessing::cPostProcessing()noexcept
: m_fFPSValue (0.0f)
{
    const coreVector2& vResolution = Core::System->GetResolution();

    // 
    this->DefineProgram("full_post_program");
    this->SetSize     (coreVector2(1.0f, 1.0f));
    this->SetTexSize  (coreVector2(1.0f,-1.0f));
    this->SetTexOffset(coreVector2(0.0f, 1.0f));
    this->Move();

    // 
    const coreVector2 vFlip = (vResolution.AspectRatio() < 1.0f) ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
    for(int i = 0; i < 2; ++i)
    {
        const float fSide = (i ? 1.0f : -1.0f);

        // 
        m_aSideArea[i].DefineProgram("default_2d_program");
        m_aSideArea[i].DefineTexture(0, "menu_background_black.png");

        m_aSideArea[i].SetSize     ((vResolution - g_vGameResolution) / vResolution.yx() * 0.5f + vFlip.yx());
        m_aSideArea[i].SetCenter   (vFlip * (fSide *  0.5f));
        m_aSideArea[i].SetAlignment(vFlip * (fSide * -1.0f));
        m_aSideArea[i].SetTexSize  (m_aSideArea[i].GetSize() * 3.0f);
        m_aSideArea[i].Move();
        
        // 
        m_aSideLine[i].DefineProgram("default_2d_program");
        m_aSideLine[i].DefineTexture(0, "default_white.png");

        m_aSideLine[i].SetPosition (vFlip * (fSide * (-m_aSideArea[i].GetSize() + 0.011f)));
        m_aSideLine[i].SetSize     (vFlip * 0.006f + vFlip.yx());
        m_aSideLine[i].SetCenter   (m_aSideArea[i].GetCenter());
        m_aSideLine[i].SetAlignment(m_aSideArea[i].GetAlignment());
        m_aSideLine[i].SetColor3   (coreVector3(0.8f,0.8f,0.8f));
        m_aSideLine[i].Move();
    }

    // 
    m_FPS.Construct("ethnocentric.ttf", 24, 8);
    m_FPS.SetPosition (coreVector2(0.008f, 0.0f));
    m_FPS.SetCenter   (coreVector2( -0.5f, 0.5f));
    m_FPS.SetAlignment(coreVector2(  1.0f,-1.0f));
}


// ****************************************************************
// destructor
cPostProcessing::~cPostProcessing()
{
}


// ****************************************************************
// apply post-processing
void cPostProcessing::Apply()
{
    // 
    coreFrameBuffer::EndDraw();

    // 
    this->DefineTexture(0, g_pEnvironment->GetFrameBuffer()->GetColorTarget(0).pTexture);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    {
        // 
        this->Render();

        // 
        for(int i = 0; i < 2; ++i) m_aSideArea[i].Render();
        for(int i = 0; i < 2; ++i) m_aSideLine[i].Render();

        // 
        if(!Core::Input->GetKeyboardButton(CORE_INPUT_KEY(PRINTSCREEN), CORE_INPUT_PRESS))
        {
            if(Core::System->GetTime()) m_fFPSValue = m_fFPSValue * 0.95f + RCP(Core::System->GetTime()) * 0.05f;
            m_FPS.SetText(PRINT("%.1f", m_fFPSValue));
            m_FPS.Move();
            m_FPS.Render();
        }
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // 
    g_pEnvironment->GetFrameBuffer()->Invalidate(CORE_FRAMEBUFFER_TARGET_COLOR);
    this->DefineTexture(0, NULL);
}