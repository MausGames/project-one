//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MSGBOX_H_
#define _P1_GUARD_MSGBOX_H_

// TODO: transfer to Core Engine
// TODO: fix controller support


// ****************************************************************
// message box definitions
#define MSGBOX_IGNORE_MOUSE     (coreVector2(2.0f,2.0f))   // 

#define MSGBOX_TYPE_INFORMATION (1u)                       // 
#define MSGBOX_TYPE_QUESTION    (2u)                       // 

#define MSGBOX_STATUS_OK        (1)                        // 
#define MSGBOX_STATUS_YES       (1)                        // 
#define MSGBOX_STATUS_NO        (2)                        // 


// ****************************************************************
// message box class
class cMsgBox final : public coreObject2D
{
public:
    coreObject2D m_Box;                            // 
    coreLabel    m_Msg;                            // 
    coreButton   m_Yes;                            // 
    coreButton   m_No;                             // 

    std::function<void (coreInt32)> m_nCallback;   // 
    coreVector2 m_vCurMouse;                       // 

    coreFloat m_fFade;                             // 
    coreUint8 m_iType;                             // 


public:
    cMsgBox()noexcept;

    DISABLE_COPY(cMsgBox)

    // render and move the message box
    void Render()override;
    void Move  ()override;

    // 
    template <typename F> void ShowInformation(const coreChar* pcText, F&& nCallback);   // [](const coreUintW iStatus) -> void
    template <typename F> void ShowQuestion   (const coreChar* pcText, F&& nCallback);   // [](const coreUintW iStatus) -> void


private:
    // 
    template <typename F> void __ShowMessage(const coreChar* pcText, F&& nCallback);   // [](const coreUintW iStatus) -> void
};


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowInformation(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Yes.SetPosition(coreVector2(0.0f, m_No.GetPosition().y));

    // 
    m_iType = MSGBOX_TYPE_INFORMATION;
    this->__ShowMessage(pcText, nCallback);
}


// ****************************************************************
// 
template <typename F> void cMsgBox::ShowQuestion(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Yes.SetPosition(m_No.GetPosition().InvertedX());

    // 
    m_iType = MSGBOX_TYPE_QUESTION;
    this->__ShowMessage(pcText, nCallback);
}


// ****************************************************************
// 
template <typename F> void cMsgBox::__ShowMessage(const coreChar* pcText, F&& nCallback)
{
    // 
    m_Msg.SetText(pcText);

    // 
    ASSERT(!m_nCallback)
    m_nCallback = nCallback;
    m_vCurMouse = Core::Input->GetMousePosition();
}


#endif // _P1_GUARD_MSGBOX_H_