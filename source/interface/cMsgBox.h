///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MSGBOX_H_
#define _P1_GUARD_MSGBOX_H_

// TODO: transfer to Core Engine
// TODO: fix controller support (mouse gets warped because of input-lock)
// TODO: X character is off some pixels (font problem)


// ****************************************************************
// message box definitions
#define MSGBOX_IGNORE_MOUSE     (coreVector2(2.0f,2.0f))   // 

#define MSGBOX_TYPE_INFORMATION (1u)                       // 
#define MSGBOX_TYPE_QUESTION    (2u)                       // 
#define MSGBOX_TYPE_MAPPING     (3u)                       // 

#define MSGBOX_ANSWER_YES       (1)                        // 
#define MSGBOX_ANSWER_NO        (2)                        // 
#define MSGBOX_ANSWER_KEY       (3)                        // 


// ****************************************************************
// message box class
class cMsgBox final : public coreObject2D
{
public:
    coreObject2D m_Box;                                      // 
    coreLabel    m_Msg;                                      // 
    coreButton   m_Yes;                                      // 
    coreButton   m_No;                                       // 

    std::function<void(coreInt32, coreInt32)> m_nCallback;   // 
    coreVector2 m_vCurMouse;                                 // 

    coreFlow  m_fFade;                                       // 
    coreUint8 m_iMsgType;                                    // 
    coreUint8 m_iInputType;                                  // 


public:
    cMsgBox()noexcept;

    DISABLE_COPY(cMsgBox)

    // render and move the message box
    void Render()final;
    void Move  ()final;

    // 
    template <typename F> void ShowInformation(const coreChar* pcText,                             F&& nCallback);   // [](const corInt32 iAnswer) -> void
    template <typename F> void ShowQuestion   (const coreChar* pcText,                             F&& nCallback);   // [](const corInt32 iAnswer) -> void
    template <typename F> void ShowMapping    (const coreChar* pcText, const coreUint8 iInputType, F&& nCallback);   // [](const corInt32 iAnswer, const coreInt16 iKey) -> void


private:
    // 
    void __ExecuteCallback(const coreInt32 a, const coreInt32 b);

    // 
    template <typename F> void __ShowMessage(const coreChar* pcText, F&& nCallback);   // [](const corInt32 a, const corInt32 b) -> void
};


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowInformation(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Yes.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_No .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_iMsgType = MSGBOX_TYPE_INFORMATION;
    this->__ShowMessage(pcText, [=](const coreInt32 a, const coreInt32 b) {nCallback(a);});
}


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowQuestion(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Yes.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_No .SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_iMsgType = MSGBOX_TYPE_QUESTION;
    this->__ShowMessage(pcText, [=](const coreInt32 a, const coreInt32 b) {nCallback(a);});
}


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowMapping(const coreChar* pcText, const coreUint8 iInputType, F&& nCallback)
{
    // 
    m_iInputType = iInputType;

    // 
    m_Yes.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_No .SetEnabled(CORE_OBJECT_ENABLE_ALL);

    // 
    m_iMsgType = MSGBOX_TYPE_MAPPING;
    this->__ShowMessage(pcText, [=](const coreInt32 a, const coreInt32 b) {nCallback(a, b);});
}


// ****************************************************************
// 
template <typename F> void cMsgBox::__ShowMessage(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Msg.SetText(pcText);

    // 
    m_Box.SetSize(coreVector2(0.0f,0.0f));
    m_Msg.RetrieveDesiredSize([this](const coreVector2& vSize)
    {
        m_Box.SetSize(coreVector2(MAX(vSize.x + 0.1f, 0.55f), 0.25f));
    });

    // 
    if(m_Yes.IsEnabled(CORE_OBJECT_ENABLE_MOVE) && m_No.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_Yes.SetPosition(m_Box.GetPosition() + coreVector2(-0.085f,-0.05f));
        m_No .SetPosition(m_Box.GetPosition() + coreVector2( 0.085f,-0.05f));
    }
    else
    {
        m_Yes.SetPosition(m_Box.GetPosition() + coreVector2(0.0f,-0.05f));
        m_No .SetPosition(m_Box.GetPosition() + coreVector2(0.0f,-0.05f));
    }

    // 
    ASSERT(!m_nCallback)
    m_nCallback = nCallback;
    m_vCurMouse = Core::Input->GetMousePosition();
}


#endif // _P1_GUARD_MSGBOX_H_