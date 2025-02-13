///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MSGBOX_H_
#define _P1_GUARD_MSGBOX_H_

// TODO 1: transfer to Core Engine
// TODO 2: fix controller support (mouse gets warped because of input-lock)
// TODO 3: X character is off some pixels (font problem)
// TODO 3: do not react to input for a very short time, when opening a non-mapping msg-box
// TODO 1: cleanup m_aDownloadText


// ****************************************************************
// message box definitions
#define MSGBOX_IGNORE_MOUSE (coreVector2(2.0f,2.0f))   // 
#define MSGBOX_NO_CALLBACK  ([](){})                   // 

enum eMsgType : coreUint8
{
    MSGBOX_TYPE_INFORMATION = 0u,   // 
    MSGBOX_TYPE_QUESTION    = 1u,   // 
    MSGBOX_TYPE_MAPPING     = 2u,   // 
    MSGBOX_TYPE_DOWNLOAD    = 3u    // 
};

enum eMsgAnswer : coreUint8
{
    MSGBOX_ANSWER_YES = 0u,   // 
    MSGBOX_ANSWER_NO  = 1u,   // 
    MSGBOX_ANSWER_KEY = 2u    // 
};

STATIC_ASSERT(MSGBOX_IGNORE_MOUSE != MENUNAVIGATOR_IGNORE_MOUSE)


// ****************************************************************
// message box class
class cMsgBox final : public coreFullscreen
{
private:
    cGuiObject m_Box;                                         // 
    cGuiLabel  m_Msg;                                         // 
    cGuiButton m_Delete;                                      // 
    cGuiButton m_Yes;                                         // 
    cGuiButton m_No;                                          // 

    cGuiObject             m_aDownloadBar [3];                // (0 = background, 1 = foreground, 2 = empty) 
    cGuiLabel              m_aDownloadText[2];                // (0 = status, 1 = percent)
    corePlatformFileHandle m_iDownloadHandle;                 // 
    coreFloat              m_fDownloadValue;                  // 

    std::function<void(eMsgAnswer, coreInt16)> m_nCallback;   // 

    coreVector2 m_vCurMouse;                                  // 
    coreVector2 m_vBoxSize;                                   // 
    coreFlow    m_fFade;                                      // 

    eMsgType  m_eMsgType;                                     // 
    coreUint8 m_iInputType;                                   // 

    cMenuNavigator m_Navigator;                               // 


public:
    cMsgBox()noexcept;

    DISABLE_COPY(cMsgBox)

    // render and move the message box
    void Render()final;
    void Move  ()final;

    // 
    template <typename F> void ShowInformation(const coreChar* pcText,                             F&& nCallback);   // []() -> void
    template <typename F> void ShowQuestion   (const coreChar* pcText,                             F&& nCallback);   // [](const eMsgAnswer eAnswer) -> void
    template <typename F> void ShowMapping    (const coreChar* pcText, const coreUint8 iInputType, F&& nCallback);   // [](const eMsgAnswer eAnswer, const coreInt16 iKey) -> void

    // 
    void StartDownload(const corePlatformFileHandle iHandle);
    void EndDownload();

    // 
    inline coreBool IsVisible()const {return (m_nCallback != NULL);}

    // 
    inline const eMsgType& GetMsgType()const {return m_eMsgType;}


private:
    // 
    void __ExecuteCallback(const eMsgAnswer a, const coreInt16 b);

    // 
    template <typename F> void __ShowMessage(const coreChar* pcText, F&& nCallback);   // [](const eMsgAnswer a, const coreInt16 b) -> void

    // 
    static void __ClearInput();
};


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowInformation(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Delete.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_Yes   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_No    .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_eMsgType = MSGBOX_TYPE_INFORMATION;
    this->__ShowMessage(pcText, [nCallback = std::forward<F>(nCallback)](const eMsgAnswer a, const coreInt16 b) {nCallback();});
}


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowQuestion(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Delete.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_Yes   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_No    .SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_eMsgType = MSGBOX_TYPE_QUESTION;
    this->__ShowMessage(pcText, [nCallback = std::forward<F>(nCallback)](const eMsgAnswer a, const coreInt16 b) {nCallback(a);});
}


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowMapping(const coreChar* pcText, const coreUint8 iInputType, F&& nCallback)
{
    // 
    m_iInputType = iInputType;

    // 
    m_Delete.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_Yes   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_No    .SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_eMsgType = MSGBOX_TYPE_MAPPING;
    this->__ShowMessage(pcText, [nCallback = std::forward<F>(nCallback)](const eMsgAnswer a, const coreInt16 b) {nCallback(a, b);});
}


// ****************************************************************
// 
template <typename F> void cMsgBox::__ShowMessage(const coreChar* pcText, F&& nCallback)
{
    // 
    ASSERT(pcText)
    m_Msg.SetText(pcText);

    // 
    if(pcText[0])
    {
        m_vBoxSize = coreVector2(0.0f,0.0f);
        m_Msg.RetrieveDesiredSize([this](const coreVector2 vSize)
        {
            m_vBoxSize = coreVector2(MAX(vSize.x + 0.1f, 0.55f), 0.25f);
        });
    }

    // 
    if(m_No.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_Delete.SetPosition(m_Box.GetPosition() + coreVector2(-0.085f,-0.045f));
        m_Yes   .SetPosition(m_Box.GetPosition() + coreVector2(-0.085f,-0.045f));
        m_No    .SetPosition(m_Box.GetPosition() + coreVector2( 0.085f,-0.045f));

        m_Navigator.BindObject(NULL, NULL, &m_Yes, NULL, &m_No, MENU_TYPE_DEFAULT);
    }
    else
    {
        m_Delete.SetPosition(m_Box.GetPosition() + coreVector2(0.0f,-0.045f));
        m_Yes   .SetPosition(m_Box.GetPosition() + coreVector2(0.0f,-0.045f));
        m_No    .SetPosition(m_Box.GetPosition() + coreVector2(0.0f,-0.045f));

        m_Navigator.BindObject(NULL, &m_Yes, &m_Yes, &m_Yes, &m_Yes, MENU_TYPE_DEFAULT);
    }

    // 
    const coreObjectEnable eEnabled = (m_eMsgType == MSGBOX_TYPE_DOWNLOAD) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDownloadBar); ++i)
    {
        m_aDownloadBar[i].SetEnabled(eEnabled);
    }
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDownloadText); ++i)
    {
        m_aDownloadText[i].SetEnabled(eEnabled);
    }

    // 
    ASSERT(!m_nCallback)
    m_nCallback = std::forward<F>(nCallback);
    m_vCurMouse = Core::Input->GetMousePosition();

    // 
    m_Navigator.ResetFirst();

    // 
    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_MSGBOX_SHOW);
}


#endif // _P1_GUARD_MSGBOX_H_