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
cNewIndicator::cNewIndicator()noexcept
: m_fFade   (0.0f)
, m_bLocked (false)
, m_iIndex  (0xFFu)
{
    // 
    this->DefineTexture(0u, "effect_headlight_point.png");
    this->DefineProgram("menu_single_program");
    this->SetSize      (coreVector2(1.0f,1.0f) * 0.08f);
    this->SetColor3    (coreVector3(1.0f,1.0f,1.0f));

    // 
    m_Text.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_Text.SetColor3      (COLOR_MENU_INSIDE);
    m_Text.SetTextLanguage("NEW");
    m_Text.SetRectify     (false);
}


// ****************************************************************
// 
void cNewIndicator::Render()
{
    if(this->IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        // 
        const coreFloat fAlpha = this->GetAlpha() * BLENDH3(m_fFade);

        // 
        this->SetAlpha(fAlpha * 0.3f);
        this->cGuiObject::Render();

        // 
        m_Text.SetAlpha(fAlpha);
        m_Text.Render();
    }
}


// ****************************************************************
// 
void cNewIndicator::Move()
{
    // 
    if(HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiNew, m_iIndex))
    {
        m_fFade = 1.0f;
    }
    else if(!this->GetAlpha() || (m_fFade < 1.0f))
    {
        m_fFade.UpdateMax(-2.0f, 0.0f, 0u);   // # faster in transition
    }

    // 
    this->SetEnabled(m_fFade ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

    if(this->IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        const coreFloat fScale = 1.0f + 0.1f * SIN(Core::System->GetTotalTimeFloat(1.0) * (2.0f*PI));

        // 
        m_Text.SetPosition (this->GetPosition () + 0.5f * this->GetSize() * this->GetAlignment());
        m_Text.SetDirection(this->GetDirection());
        m_Text.SetCenter   (this->GetCenter   ());
        m_Text.SetScale    (coreVector2(1.0f,1.0f) * fScale);
        m_Text.Move();

        // 
        this->cGuiObject::Move();
    }
}


// ****************************************************************
// 
void cNewIndicator::Resolve()
{
    if(!m_bLocked) REMOVE_BIT_EX(g_pSave->EditProgress()->aiNew, m_iIndex)
}


// ****************************************************************
// 
void cNewIndicator::Acquire()
{
    m_fFade   = HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiNew, m_iIndex) ? 1.0f : 0.0f;
    m_bLocked = true;
}


// ****************************************************************
// 
void cNewIndicator::Release()
{
    m_bLocked = false;
}