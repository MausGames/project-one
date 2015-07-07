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
cMsgBox::cMsgBox()noexcept
: m_vCurMouse   (coreVector2(0.0f,0.0f))
, m_iCurTooltip ((Core::Rand->Int(MSGBOX_TOOLTIPS - 1u) << 4u) | Core::Rand->Int(MSGBOX_TOOLTIPS - 1u))
, m_iType       (0u)
, m_fFade       (0.0f)
{
    // 
    this->DefineProgram("default_2d_program");
    this->DefineTexture(0u, "menu_background_black.png");
    this->SetSize      (coreVector2(1.0f,1.0f));
    this->SetColor4    (coreVector4(0.6f,0.6f,0.6f,0.0f));
    this->SetTexSize   (coreVector2(1.2f,1.2f));
    coreObject2D::Move();

    // 
    m_Box.DefineProgram("menu_border_program");
    m_Box.DefineTexture(0u, "menu_background_black.png");
    m_Box.SetPosition  (coreVector2(0.0f, 0.0f));
    m_Box.SetSize      (coreVector2(0.45f,0.25f));

    // 
    m_Msg.Construct  (MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_Msg.SetPosition(m_Box.GetPosition() + coreVector2(0.0f,0.045f));

    // 
    m_Yes.Construct    (MENU_BUTTON, MENU_ICON_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_Yes.DefineProgram("menu_border_program");
    m_Yes.SetPosition  (m_Box.GetPosition() + coreVector2(-0.1f,-0.05f));
    m_Yes.SetSize      (coreVector2(0.07f,0.07f));
    m_Yes.GetCaption()->SetText(ICON_CHECK);

    // 
    m_No.Construct    (MENU_BUTTON, MENU_ICON_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_No.DefineProgram("menu_border_program");
    m_No.SetPosition  (m_Yes.GetPosition().InvertedX());
    m_No.SetSize      (m_Yes.GetSize());
    m_No.GetCaption()->SetText(ICON_TIMES);
}


// ****************************************************************
// render the message box
void cMsgBox::Render()
{
    if(!m_fFade) return;

    // 
    coreObject2D::Render();

    // 
    m_Box.Render();
    m_Msg.Render();

    if(m_iType == MSGBOX_TYPE_INFORMATION)
    {
        // 
        m_Yes.Render();
    }
    else if(m_iType == MSGBOX_TYPE_QUESTION)
    {
        // 
        m_Yes.Render();
        m_No .Render();
    }
    else ASSERT(false)
}


// ****************************************************************
// move the message box
void cMsgBox::Move()
{
    // 
    if(m_nCallback) m_fFade = MIN(m_fFade + 10.0f * Core::System->GetTime(), 1.0f);
               else m_fFade = MAX(m_fFade - 10.0f * Core::System->GetTime(), 0.0f);

    if(!m_fFade) return;

    // 
    if(Core::Input->GetMousePosition() == MSGBOX_IGNORE_MOUSE)
        Core::Input->SetMousePosition(m_vCurMouse);
    else m_vCurMouse = Core::Input->GetMousePosition();

    // 
    this->SetAlpha    (m_fFade * 0.5f);
    this->SetTexOffset(coreVector2(0.0f, -0.04f * coreFloat(Core::System->GetTotalTime())));

    // 
    m_Box.SetAlpha(m_fFade);
    m_Msg.SetAlpha(m_fFade);
    m_Yes.SetAlpha(m_fFade);
    m_No .SetAlpha(m_fFade);

    // 
    m_Box.Move();
    m_Msg.Move();
    m_Yes.Move();
    m_No .Move();

    if(m_iType == MSGBOX_TYPE_INFORMATION)
    {
        // 
        m_Yes.Interact();

        // 
        cMenu::UpdateButton(&m_Yes, m_Yes.IsFocused(), COLOR_MENU_GREEN);

        // 
        if(m_Yes.IsFocused()) {g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString(PRINT("OK_%u", 1u + (m_iCurTooltip & 0x0Fu))));}
        if(m_Yes.IsClicked()) {m_nCallback(MSGBOX_STATUS_OK); m_nCallback = NULL;}
    }
    else if(m_iType == MSGBOX_TYPE_QUESTION)
    {
        const coreBool& bEsc = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS);

        // 
        m_Yes.Interact();
        m_No .Interact();

        // 
        cMenu::UpdateButton(&m_Yes, m_Yes.IsFocused(), COLOR_MENU_GREEN);
        cMenu::UpdateButton(&m_No,  m_No .IsFocused(), COLOR_MENU_RED);

        // 
             if(m_Yes.IsFocused())         {g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString(PRINT("YES_%u", 1u + ((m_iCurTooltip & 0xF0u) >> 4u))));}
        else if(m_No .IsFocused())         {g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString(PRINT("NO_%u",  1u +  (m_iCurTooltip & 0x0Fu))));}
             if(m_Yes.IsClicked())         {m_nCallback(MSGBOX_STATUS_YES); m_nCallback = NULL;}
        else if(m_No .IsClicked() || bEsc) {m_nCallback(MSGBOX_STATUS_NO);  m_nCallback = NULL;}
    }
    else ASSERT(false)

    // 
    Core::Input->SetMousePosition(MSGBOX_IGNORE_MOUSE);
    Core::Input->ClearMouseButton(CORE_INPUT_LEFT);
}