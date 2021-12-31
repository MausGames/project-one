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
cScrollBox::cScrollBox()noexcept
: m_fMaxOffset (0.0f)
, m_bDrag      (false)
, m_fDragValue (0.0f)
, m_Automatic  (coreTimer(1.0f, 10.0f, 0u))
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArrow); ++i)
    {
        m_aArrow[i].Construct    (MENU_BUTTON, MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aArrow[i].DefineProgram("menu_border_program");
        m_aArrow[i].SetSize      (coreVector2(1.0f,1.0f) * SCROLL_WIDTH);
        m_aArrow[i].SetDirection (coreVector2(i ? 1.0f : -1.0f, 0.0f));
        m_aArrow[i].GetCaption()->SetText("<");
    }

    // 
    m_Cursor.Construct    (MENU_BUTTON);
    m_Cursor.DefineProgram("menu_border_program");
    m_Cursor.SetSize      (coreVector2(1.0f,3.0f) * SCROLL_WIDTH);

    // 
    this->SetStyle(CORE_OBJECT2D_STYLE_VIEWDIR | CORE_OBJECT2D_STYLE_ALTCENTER);
}


// ****************************************************************
// 
void cScrollBox::Render()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArrow); ++i)
    {
        m_aArrow[i].SetAlpha(this->GetAlpha());
        m_aArrow[i].Render();
    }

    // 
    m_Cursor.SetAlpha(this->GetAlpha());
    m_Cursor.Render();

    // 
    this->coreViewBox::Render();
}


// ****************************************************************
// 
void cScrollBox::Move()
{
    ASSERT(this->GetSize().y > m_aArrow[0].GetSize().y + m_aArrow[1].GetSize().y + m_Cursor.GetSize().y)

    // 
    m_aArrow[0].Interact();
    m_aArrow[1].Interact();
    m_Cursor   .Interact();

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArrow); ++i)
    {
        const coreFloat fSide = i ? -0.5f : 0.5f;

        // 
        m_aArrow[i].SetPosition(this->GetPosition() + this->GetSize() * coreVector2(0.5f, fSide) + SCROLL_WIDTH * coreVector2(0.5f, -fSide));
        m_aArrow[i].Move();

        // 
        cMenu::UpdateButton(&m_aArrow[i], m_aArrow[i].IsFocused());
    }

    // 
    const coreFloat fHeight  = (SCROLL_WIDTH + m_Cursor.GetSize().y) * 0.5f;
    const coreFloat fPosUp   = m_aArrow[0].GetPosition().y - fHeight;
    const coreFloat fPosDown = m_aArrow[1].GetPosition().y + fHeight;

    // 
    const coreFloat fMousePos = MapToAxis(Core::Input->GetMousePosition() * (Core::System->GetResolution() / g_vGameResolution), g_vHudDirection).y;

    // 
    coreFloat fWheel = Core::Input->GetMouseWheel();

    // 
    const coreInputType eType  = m_Automatic.GetStatus() ? CORE_INPUT_HOLD : CORE_INPUT_PRESS;
    const coreBool      bLeft  = m_aArrow[0].IsClicked(CORE_INPUT_LEFT, eType);
    const coreBool      bRight = m_aArrow[1].IsClicked(CORE_INPUT_LEFT, eType);

    if(bLeft || bRight)
    {
        // 
        if(m_Automatic.Update(1.0f) || !m_Automatic.GetStatus())
        {
            // 
            fWheel += bLeft ? 1.0f : -1.0f;

            // 
            m_Automatic.Play(CORE_TIMER_PLAY_CURRENT);
        }
    }
    else
    {
        // 
        m_Automatic.Pause();
        m_Automatic.SetValue(SCROLL_DELAY);
    }

    // 
    if(m_Cursor.IsClicked())
    {
        m_bDrag      = true;
        m_fDragValue = fMousePos - m_Cursor.GetPosition().y;
    }
    else if(Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_RELEASE))
    {
        m_bDrag      = false;
        m_fDragValue = 0.0f;
    }

    // 
    coreFloat fDrag = 0.0f;
    if(m_bDrag)
    {
        const coreFloat fNewDragValue = fMousePos - m_Cursor.GetPosition().y;
        fDrag += (fNewDragValue - m_fDragValue) * RCP(fPosUp - fPosDown) * m_fMaxOffset;
    }

    // 
    this->SetOffset(coreVector2(0.0f, CLAMP(this->GetOffset().y - fWheel*0.05f - fDrag, 0.0f, m_fMaxOffset)));

    // 
    m_Cursor.SetPosition(coreVector2(m_aArrow[0].GetPosition().x, LERP(fPosUp, fPosDown, this->GetOffset().y * RCP(m_fMaxOffset))));
    m_Cursor.Move();

    // 
    cMenu::UpdateButton(&m_Cursor, m_Cursor.IsFocused());

    // 
    this->coreViewBox::Move();
}