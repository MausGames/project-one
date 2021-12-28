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
cCreditRoll::cCreditRoll()noexcept
: m_fOffset (0.0f)
{
    // 
    m_ViewBox.SetPosition(coreVector2(0.0f,0.0f));
    m_ViewBox.SetSize    (coreVector2(1.0f,1.0f));

    // 
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i)
    {
        const coreFloat fHeight = I_TO_F(i) * -0.15f - 0.65f;

        m_aName[i].Construct  (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aName[i].SetColor3  (COLOR_MENU_INSIDE);
        m_aName[i].SetAlpha   (MENU_INSIDE_ALPHA);
        m_aName[i].SetText    (g_aapcCreditEntry[i][0]);

        m_ViewBox.BindObject(&m_aName[i]);
    }

    // 
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i)
    {
        const coreFloat fHeight = I_TO_F(i) * -0.15f - 0.7f;

        m_aDescription[i].Construct  (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
        m_aDescription[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aDescription[i].SetColor3  (COLOR_MENU_BLUE);
        m_aDescription[i].SetAlpha   (MENU_INSIDE_ALPHA);
        m_aDescription[i].SetText    (g_aapcCreditEntry[i][1]);

        m_ViewBox.BindObject(&m_aDescription[i]);
    }
}


// ****************************************************************
// render the credit roll
void cCreditRoll::Render()
{
    // 
    m_ViewBox.Render();
}


// ****************************************************************
// move the credit roll
void cCreditRoll::Move()
{
    // 
    m_fOffset.Update(0.1f);

    // 
    m_ViewBox.SetOffset(coreVector2(0.0f, m_fOffset));
    m_ViewBox.Move();
}


// ****************************************************************
// 
void cCreditRoll::Start()
{
    // 
    m_fOffset = 0.0f;

    // 
    m_ViewBox.SetOffset(coreVector2(0.0f,0.0f));
    m_ViewBox.Move();
}