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
: m_fCurOffset (0.0f)
, m_fMaxOffset (0.0f)
, m_bDrag      (false)
, m_fDragValue (0.0f)
, m_fSpeed     (0.0f)
, m_bInside    (false)
, m_Automatic  (coreTimer(1.0f, 10.0f, 0u))
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArrow); ++i)
    {
        m_aArrow[i].Construct    (MENU_BUTTON, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aArrow[i].DefineProgram("menu_border_program");
        m_aArrow[i].SetSize      (coreVector2(1.0f,1.0f) * SCROLL_WIDTH);
        m_aArrow[i].SetDirection (coreVector2(i ? -1.0f : 1.0f, 0.0f));
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
    const coreBool bOverflow = (m_fMaxOffset > 0.0f);
    this->SetScissor(bOverflow);

    // 
    this->coreViewBox::Render();

    if(bOverflow)
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
    }
}


// ****************************************************************
// 
void cScrollBox::Move()
{
    ASSERT(this->GetSize().y > m_aArrow[0].GetSize().y + m_aArrow[1].GetSize().y + m_Cursor.GetSize().y)

    // 
    if(m_fMaxOffset > 0.0f)
    {
        // 
        m_aArrow[0].Interact();
        m_aArrow[1].Interact();
        m_Cursor   .Interact();

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArrow); ++i)
        {
            const coreFloat fSide = i         ? -0.5f : 0.5f;
            const coreFloat fFlip = m_bInside ? -0.5f : 0.5f;

            // 
            m_aArrow[i].SetPosition(this->GetPosition() + this->GetSize() * coreVector2(0.5f, fSide) + SCROLL_WIDTH * coreVector2(fFlip, -fSide));
            m_aArrow[i].Move();

            // 
            cMenu::UpdateButton(&m_aArrow[i], this, m_aArrow[i].IsFocused());
        }

        // 
        const coreFloat fHeight  = (SCROLL_WIDTH + m_Cursor.GetSize().y) * 0.5f;
        const coreFloat fPosUp   = m_aArrow[0].GetPosition().y - fHeight;
        const coreFloat fPosDown = m_aArrow[1].GetPosition().y + fHeight;

        // 
        const coreFloat fMousePos = MapToAxisInv(Core::Input->GetMousePosition() * (Core::System->GetResolution() / g_vGameResolution), g_vHudDirection).y;

        // 
        coreFloat fWheel = TIME ? Core::Input->GetMouseWheel() : 0.0f;

        // 
        const coreBool  bMouseWheel = (fWheel != 0.0f);
        const coreFloat fOldOffset  = m_fCurOffset;

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
        else if(!Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_HOLD))
        {
            m_bDrag      = false;
            m_fDragValue = 0.0f;
        }

        // 
        coreFloat fDrag = 0.0f;
        if(m_bDrag)
        {
            const coreFloat fNewDragValue = fMousePos - m_Cursor.GetPosition().y;
            fDrag += (fNewDragValue - m_fDragValue) / (fPosUp - fPosDown) * m_fMaxOffset;
        }

        // 
        m_fCurOffset = CLAMP(m_fCurOffset - fWheel*0.05f - fDrag, 0.0f, m_fMaxOffset);

        // 
        if(cMenuNavigator::IsUsingAny())
        {
            coreObject2D* pFocus = cMenuNavigator::GetCurFocus();
            if(this->ContainsObject(pFocus) && cMenuNavigator::IsValid(pFocus))
            {
                this->ScrollToObject(pFocus, false);
            }
        }

        // 
        const coreFloat fAbsDiff = ABS(m_fCurOffset - this->GetOffset().y);
        const coreFloat fBoost   = LERP(6.0f, 1.0f, STEP(0.0f, 0.002f, fAbsDiff));   // to snap into place

        // gradually increase speed (until the end)
        if(fAbsDiff < 0.001f) m_fSpeed = 0.0f;
        m_fSpeed.UpdateMin(10.0f, 1.0f);   // always update

        // jump directly to offset (on big difference)
        if(fAbsDiff > 0.7f) this->SetOffset(coreVector2(0.0f, m_fCurOffset));

        // 
        this->SetOffset(coreVector2(0.0f, fDrag ? m_fCurOffset : (this->GetOffset().y + (m_fCurOffset - this->GetOffset().y) * ((cMenuNavigator::IsUsingAny() ? 4.0f : 10.0f) * m_fSpeed * fBoost * TIME))));

        // 
        m_Cursor.SetPosition(coreVector2(m_aArrow[0].GetPosition().x, LERP(fPosUp, fPosDown, this->GetOffset().y / m_fMaxOffset)));
        m_Cursor.Move();

        // 
        cMenu::UpdateButton(&m_Cursor, this, m_Cursor.IsFocused(), MENU_UPDATE_NO_GROW);

        // 
        if(bMouseWheel && (fOldOffset != m_fCurOffset)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SCROLL);
    }
    else
    {
        // 
        m_fCurOffset = 0.0f;
        this->SetOffset(coreVector2(0.0f,0.0f));
    }

    // 
    this->coreViewBox::Move();
}


// ****************************************************************
// 
void cScrollBox::ScrollToObject(const coreObject2D* pObject, const coreBool bNow)
{
    // 
    const coreVector2 vDiff  = pObject->GetPosition() - this->GetPosition();
    const coreFloat   fShift = MAX0(ABS(vDiff.y) - MAX0(this->GetSize().y - SCROLL_FOCUS_SIZE) * 0.5f) * SIGN(vDiff.y);

    // 
    m_fCurOffset = CLAMP(this->GetOffset().y - fShift, 0.0f, m_fMaxOffset);

    // 
    this->SetFocused(true);

    // 
    if(bNow)
    {
        this->SetOffset(coreVector2(0.0f, m_fCurOffset));
        this->coreViewBox::Move();
    }
}