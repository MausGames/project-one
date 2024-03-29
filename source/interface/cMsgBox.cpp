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
cMsgBox::cMsgBox()noexcept
: m_vCurMouse  (coreVector2(0.0f,0.0f))
, m_fFade      (0.0f)
, m_iMsgType   (0u)
, m_iInputType (0u)
{
    // 
    this->DefineTexture(0u, "menu_background_black.png");
    this->DefineProgram("menu_grey_program");
    this->SetColor4    (coreVector4(0.6f,0.6f,0.6f,0.0f));
    this->SetTexSize   (coreVector2(1.2f,1.2f));

    // 
    m_Box.DefineTexture(0u, "menu_background_black.png");
    m_Box.DefineProgram("menu_border_program");
    m_Box.SetPosition  (coreVector2(0.0f,0.0f));

    // 
    m_Msg.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_Msg.SetPosition(m_Box.GetPosition() + coreVector2(0.0f,0.045f));
    m_Msg.SetColor3  (COLOR_MENU_WHITE);

    // 
    m_Yes.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_Yes.DefineProgram("menu_border_program");
    m_Yes.SetSize      (coreVector2(0.07f,0.07f));
    m_Yes.GetCaption()->SetText(ICON_CHECK);

    // 
    m_No.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_No.DefineProgram("menu_border_program");
    m_No.SetSize      (m_Yes.GetSize());
    m_No.GetCaption()->SetText(ICON_TIMES);
}


// ****************************************************************
// render the message box
void cMsgBox::Render()
{
    if(!m_fFade) return;

    // 
    this->cGuiObject::Render();

    // 
    m_Box.Render();
    m_Msg.Render();
    m_Yes.Render();
    m_No .Render();
}


// ****************************************************************
// move the message box
void cMsgBox::Move()
{
    // 
    if(m_nCallback) m_fFade.UpdateMin( 10.0f, 1.0f);
               else m_fFade.UpdateMax(-10.0f, 0.0f);

    if(!m_fFade) return;

    // 
    if(Core::Input->GetMousePosition() != MSGBOX_IGNORE_MOUSE)
        m_vCurMouse = Core::Input->GetMousePosition();
    else Core::Input->SetMousePosition(m_vCurMouse);

    // only allow escape-button to cancel mapping
    if(m_iMsgType == MSGBOX_TYPE_MAPPING)
        g_MenuInput.bCancel = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS);

    // 
    this->SetSize     (Core::System->GetResolution() * RCP(Core::System->GetResolution().Min()));
    this->SetAlpha    (m_fFade * 0.5f);
    this->SetTexOffset(coreVector2(0.0f, FRACT(coreFloat(-0.04 * Core::System->GetTotalTime()))));
    this->cGuiObject::Move();

    // 
    m_Box.SetAlpha(m_fFade);
    m_Box.Move();

    // 
    m_Msg.SetAlpha(m_fFade);
    m_Msg.Move();

    // 
    if(m_Yes.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Yes.SetAlpha(m_fFade);
        m_Yes.Move();
        m_Yes.Interact();

        // 
        cMenu::UpdateButton(&m_Yes, m_Yes.IsFocused(), COLOR_MENU_BLUE);

        // 
        if(m_Yes.IsClicked() || g_MenuInput.bAccept)
            this->__ExecuteCallback(MSGBOX_ANSWER_YES, 0);
    }

    // 
    if(m_No.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_No.SetAlpha(m_fFade);
        m_No.Move();
        m_No.Interact();

        // 
        cMenu::UpdateButton(&m_No, m_No.IsFocused(), COLOR_MENU_RED);

        // 
        if(m_No.IsClicked() || g_MenuInput.bCancel)
            this->__ExecuteCallback(MSGBOX_ANSWER_NO, 0);
    }

    // 
    if(m_iMsgType == MSGBOX_TYPE_MAPPING)
    {
        if(m_iInputType < INPUT_SETS_KEYBOARD)   // # keyboard and mouse
        {
            // detect keyboard input
            if(Core::Input->GetLastKeyboard() != CORE_INPUT_INVALID_KEYBOARD)
                this->__ExecuteCallback(MSGBOX_ANSWER_KEY, coreInt16(Core::Input->GetLastKeyboard()));

            // detect mouse input
            if(Core::Input->GetLastMouse() != CORE_INPUT_INVALID_MOUSE)
                this->__ExecuteCallback(MSGBOX_ANSWER_KEY, -coreInt16(Core::Input->GetLastMouse()));
        }
        else   // # joystick/gamepad
        {
            const coreUintW iJoystickID = m_iInputType - INPUT_SETS_KEYBOARD;

            // detect joystick input
            if(Core::Input->GetLastJoystick(iJoystickID) != CORE_INPUT_INVALID_JOYSTICK)
                this->__ExecuteCallback(MSGBOX_ANSWER_KEY, coreInt16(Core::Input->GetLastJoystick(iJoystickID)));
        }
    }

    // 
    Core::Input->SetMousePosition(MSGBOX_IGNORE_MOUSE);
    Core::Input->ClearButtonAll();

    // 
    std::memset(&g_MenuInput, 0, sizeof(g_MenuInput));
}


// ****************************************************************
// 
void cMsgBox::__ExecuteCallback(const coreInt32 a, const coreInt32 b)
{
    if(!m_nCallback) return;

    // move callback to stack (to allow chaining)
    const auto nLocal = std::move(m_nCallback);
    m_nCallback = NULL;

    // execute callback
    nLocal(a, b);
}