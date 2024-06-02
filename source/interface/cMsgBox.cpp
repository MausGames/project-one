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
: m_iDownloadHandle (0u)
, m_fDownloadValue  (0.0f)
, m_vCurMouse       (coreVector2(0.0f,0.0f))
, m_vBoxSize        (coreVector2(0.0f,0.0f))
, m_fFade           (0.0f)
, m_iMsgType        (0u)
, m_iInputType      (0u)
{
    // 
    this->DefineTexture(0u, "menu_background_black.png");
    this->DefineProgram("menu_grey_vignette_program");
    this->SetColor4    (coreVector4(0.6f,0.6f,0.6f,0.0f));
    this->SetTexSize   (coreVector2(1.2f,1.2f));
    this->SetStyle     (CORE_OBJECT2D_STYLE_VIEWDIR);

    // 
    m_Box.DefineTexture(0u, "menu_background_black.png");
    m_Box.DefineProgram("menu_border_program");
    m_Box.SetPosition  (coreVector2(0.0f,0.0f));

    // 
    m_Msg.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_Msg.SetPosition(m_Box.GetPosition() + coreVector2(0.0f,0.045f));
    m_Msg.SetColor3  (COLOR_MENU_WHITE);

    // 
    m_Delete.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_Delete.DefineProgram("menu_border_program");
    m_Delete.SetSize      (coreVector2(0.07f,0.07f));
    m_Delete.GetCaption()->SetText(ICON_TRASH_CAN);

    // 
    m_Yes.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_Yes.DefineProgram("menu_border_program");
    m_Yes.SetSize      (m_Delete.GetSize());
    m_Yes.GetCaption()->SetText(ICON_CHECK);

    // 
    m_No.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_No.DefineProgram("menu_border_program");
    m_No.SetSize      (m_Delete.GetSize());
    m_No.GetCaption()->SetText(ICON_TIMES);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDownloadBar); ++i)
    {
        m_aDownloadBar[i].DefineTexture(0u, "menu_detail_02.png");
        m_aDownloadBar[i].DefineProgram("default_2d_program");
        m_aDownloadBar[i].SetPosition  (coreVector2(0.0f,-0.005f));
        m_aDownloadBar[i].SetSize      (coreVector2(7.0f,0.45f) * 0.07f);
    }
    m_aDownloadBar[0].SetColor3(coreVector3(0.0f,0.0f,0.0f));
    m_aDownloadBar[1].SetSize  (m_aDownloadBar[0].GetSize() - coreVector2(0.01f,0.01f));
    m_aDownloadBar[2].SetSize  (m_aDownloadBar[1].GetSize());

    // 
    m_aDownloadText[0].Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_aDownloadText[0].SetPosition    (m_Box.GetPosition() + coreVector2(0.0f,0.032f));
    m_aDownloadText[0].SetColor3      (COLOR_MENU_WHITE);
    //m_aDownloadText[0].SetTextLanguage("DOWNLOADING");

    // 
    m_aDownloadText[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aDownloadText[1].SetPosition(m_aDownloadBar[0].GetPosition() + coreVector2(0.0f,0.01f));
    m_aDownloadText[1].SetColor3  (COLOR_MENU_WHITE);

    // 
    m_Navigator.BindObject(&m_Delete, NULL, NULL,      NULL, &m_No, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_Yes,    NULL, &m_Delete, NULL, &m_No, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_No,     NULL, &m_Yes,    NULL, NULL,  MENU_TYPE_DEFAULT);
}


// ****************************************************************
// render the message box
void cMsgBox::Render()
{
    if(!m_fFade) return;

    // 
    this->coreFullscreen::Render();

    // 
    m_Box.Render();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDownloadBar); ++i)
    {
        m_aDownloadBar[i].Render();
    }

    // 
    m_Msg   .Render();
    m_Delete.Render();
    m_Yes   .Render();
    m_No    .Render();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDownloadText); ++i)
    {
        m_aDownloadText[i].Render();
    }

    // 
    m_Navigator.Render();
}


// ****************************************************************
// move the message box
void cMsgBox::Move()
{
    if(!m_fFade && !m_nCallback) return;

    // 
    if(m_nCallback) m_fFade.UpdateMin( 10.0f, 1.0f);
               else m_fFade.UpdateMax(-10.0f, 0.0f);

    // (before mouse-position override) 
    m_Navigator.SetAlpha(m_fFade);
    m_Navigator.Update();
    m_Navigator.Move();

    // 
    if(Core::Input->GetMousePosition() != MSGBOX_IGNORE_MOUSE)
    {
        m_vCurMouse = Core::Input->GetMousePosition();
    }
    else
    {
        Core::Input->SetMousePosition(m_vCurMouse);
    }

    // only allow escape-button to cancel mapping
    if(m_iMsgType == MSGBOX_TYPE_MAPPING)
    {
        g_MenuInput.bCancel = Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS);
    }

    // 
    this->SetSize     (coreVector2(1.0f,1.0f) * MaxAspectRatio(Core::System->GetResolution()));
    this->SetAlpha    (m_fFade * 0.5f);
    this->SetTexOffset(coreVector2(0.0f, MENU_LAYER_TEXOFFSET));
    this->coreFullscreen::Move();

    // 
    m_Box.SetSize     (m_vBoxSize * LERPB(0.8f, 1.0f, m_fFade));
    m_Box.SetTexOffset((m_Box.GetSize() - m_vBoxSize) * -0.5f);
    m_Box.SetAlpha    (m_fFade);
    m_Box.Move();

    // 
    m_Msg.SetAlpha(m_fFade);
    m_Msg.Move();

    // 
    if(m_Delete.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Delete.SetAlpha(m_fFade);
        m_Delete.Move();
        m_Delete.Interact();

        // 
        cMenu::UpdateButton(&m_Delete, this, m_Delete.IsFocused(), COLOR_MENU_YELLOW);

        // 
        if(m_Delete.IsClicked())
            this->__ExecuteCallback(MSGBOX_ANSWER_KEY, INPUT_KEY_INVALID);
    }

    // 
    if(m_Yes.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_Yes.SetAlpha(m_fFade);
        m_Yes.Move();
        m_Yes.Interact();

        // 
        cMenu::UpdateButton(&m_Yes, this, m_Yes.IsFocused(), COLOR_MENU_BLUE);

        // 
        if(m_Yes.IsClicked())
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
        cMenu::UpdateButton(&m_No, this, m_No.IsFocused(), COLOR_MENU_RED);

        // 
        if(m_No.IsClicked() || ((m_iMsgType == MSGBOX_TYPE_MAPPING) && Core::Input->GetKeyboardButton(CORE_INPUT_KEY(ESCAPE), CORE_INPUT_PRESS)))
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
    if(m_iMsgType == MSGBOX_TYPE_DOWNLOAD)
    {
        // 
        coreUint32 iCurrent, iTotal;
        if(m_iDownloadHandle && Core::Platform->ProgressFile(m_iDownloadHandle, &iCurrent, &iTotal))
        {
            m_fDownloadValue = coreFloat(coreDouble(iCurrent) / coreDouble(MAX(iTotal, 1u)));
        }

        // 
        m_aDownloadText[1].SetText(PRINT("%.0f%%", m_fDownloadValue * 100.0f));

        // 
        const coreFloat fBase  = m_fDownloadValue;
        const coreFloat fWidth = m_aDownloadBar[0].GetSize().x - 0.01f;
        m_aDownloadBar[1].SetPosition(coreVector2(fWidth * (fBase-1.0f) * 0.5f, m_aDownloadBar[1].GetPosition().y));
        m_aDownloadBar[1].SetSize    (coreVector2(fWidth *  fBase,              m_aDownloadBar[1].GetSize    ().y));
        m_aDownloadBar[1].SetTexSize (coreVector2(fBase, 1.0f));

        const coreFloat fRev = 1.0f - fBase;
        m_aDownloadBar[2].SetPosition (coreVector2(fWidth * fBase * 0.5f, m_aDownloadBar[2].GetPosition().y));
        m_aDownloadBar[2].SetSize     (coreVector2(fWidth * fRev,         m_aDownloadBar[2].GetSize    ().y));
        m_aDownloadBar[2].SetTexSize  (coreVector2(fRev,  1.0f));
        m_aDownloadBar[2].SetTexOffset(coreVector2(fBase, 1.0f));

        const coreVector3 vColor = g_pMenu->GetHighlightColor();
        m_aDownloadBar[1].SetColor3(vColor * 1.0f);
        m_aDownloadBar[2].SetColor3(vColor * 0.3f);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDownloadBar); ++i)
    {
        m_aDownloadBar[i].SetAlpha(m_fFade);
        m_aDownloadBar[i].Move();
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDownloadText); ++i)
    {
        m_aDownloadText[i].SetAlpha(m_fFade);
        m_aDownloadText[i].Move();
    }

    // 
    if(!cMenuNavigator::IsUsingJoystick() || m_nCallback || (m_iMsgType == MSGBOX_TYPE_MAPPING))
    {
        // 
        cMsgBox::__ClearInput();
    }

    // 
    std::memset(&g_MenuInput, 0, sizeof(g_MenuInput));
}


// ****************************************************************
// 
void cMsgBox::StartDownload(const corePlatformFileHandle iHandle)
{
    // 
    m_Delete.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_Yes   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_No    .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_iDownloadHandle = iHandle;
    m_fDownloadValue  = 0.0f;

    // 
    m_iMsgType = MSGBOX_TYPE_DOWNLOAD;
    this->__ShowMessage("", [](const coreInt32 a, const coreInt32 b) {});
}


// ****************************************************************
// 
void cMsgBox::EndDownload()
{
    ASSERT(m_iMsgType == MSGBOX_TYPE_DOWNLOAD)

    // 
    m_iDownloadHandle = 0u;
    m_fDownloadValue  = 1.0f;

    // 
    m_nCallback = NULL;
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

    // 
    cMsgBox::__ClearInput();

    // 
    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, (a == MSGBOX_ANSWER_NO) ? SOUND_MENU_MSGBOX_NO : SOUND_MENU_MSGBOX_YES);
}


// ****************************************************************
// 
void cMsgBox::__ClearInput()
{
    Core::Input->SetMousePosition(MSGBOX_IGNORE_MOUSE);
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        Core::Input->SetJoystickAxis(i, 0u, 0.0f);
        Core::Input->SetJoystickAxis(i, 1u, 0.0f);
    }
    Core::Input->ClearButtonAll();
}