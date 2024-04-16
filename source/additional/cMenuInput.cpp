///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreBool cMenuInput::s_bJoystick = false;


// ****************************************************************
// constructor
cMenuInput::cMenuInput()noexcept
: m_pCurObject (NULL)
, m_pCurTab (NULL)
, m_bMoved (false)
, m_pMenu (NULL)
{

    this->DefineTexture(0u, Core::Manager::Resource->Load<coreTexture>("default_cursor.png", CORE_RESOURCE_UPDATE_AUTO, "data/textures/default_cursor.png"));
    this->DefineProgram("default_2d_program");
    this->SetSize      (coreVector2(0.04f,0.04f));
    this->SetDirection (coreVector2(-1.0f,-1.0f).Normalized());


}


// ****************************************************************
// 
void cMenuInput::Move()
{

    if(Core::Input->GetMouseRelative().x || Core::Input->GetMouseRelative().y)
        s_bJoystick = false;

    if(!s_bJoystick)
    {
        m_pCurObject = NULL;
        m_pCurTab = NULL;
    }


    Core::Input->ShowCursor(!s_bJoystick);

    //coreBool bReset = true;
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        //Core::Input->ForwardHatToStick(i);

        if(Core::Input->GetJoystickButton(i, 0u, CORE_INPUT_PRESS))   Core::Input->SetMouseButton(CORE_INPUT_LEFT,  true);
        if(Core::Input->GetJoystickButton(i, 0u, CORE_INPUT_RELEASE)) Core::Input->SetMouseButton(CORE_INPUT_LEFT,  false);
        if(Core::Input->GetJoystickButton(i, 1u, CORE_INPUT_PRESS))   Core::Input->SetMouseButton(CORE_INPUT_RIGHT, true);
        if(Core::Input->GetJoystickButton(i, 1u, CORE_INPUT_RELEASE)) Core::Input->SetMouseButton(CORE_INPUT_RIGHT, false);

        
        if(Core::Input->GetJoystickButton(i, 10u, CORE_INPUT_PRESS))   Core::Input->SetMouseButton(CORE_INPUT_LEFT,  true);
        if(Core::Input->GetJoystickButton(i, 10u, CORE_INPUT_RELEASE)) Core::Input->SetMouseButton(CORE_INPUT_LEFT,  false);


        if(s_bJoystick && m_pMenu && Core::Input->GetJoystickButton(i, 10u, CORE_INPUT_PRESS))
        {
            //coreObject2D* pCurTarget = NULL;

            const coreUint8 iCurSurface = m_pMenu->GetCurSurface();

            FOR_EACH(it, m_aiShoulder.get_keylist())
            {
                if((*it) == iCurSurface)
                {
                    coreObject2D* pTab = m_aiShoulder.at((it - m_aiShoulder.get_keylist().begin() + 1u) % m_aiShoulder.size());
                    m_pCurTab = pTab;
                    m_pCurObject = NULL;

                    break;
                }
            }
        }


        // TODO: make this file time-independent 


        const coreVector2 vRelative = Core::Input->GetJoystickRelativeL(i);

        if(!vRelative.IsNull())
        {
            if(!m_bMoved)
            {
                m_bMoved = true;
                s_bJoystick = true;

                const coreVector2 vMove = AlongCross(vRelative);

                if(m_pCurObject == NULL)
                {
                    ASSERT(!m_apObject.empty())
                    m_pCurObject = m_apObject.front();
                    m_pCurTab = NULL;
                }
                else
                {

                    coreObject2D* pCurTarget = NULL;
                    coreFloat     fCurDot    = FLT_MAX;

                    FOR_EACH(it, m_apObject)
                    {
                        if((*it) == m_pCurObject) continue;

                        const coreVector2 vScreenPosition1 = coreVector2((*it)       ->GetTransform()._31,  (*it)       ->GetTransform()._32);
                        const coreVector2 vScreenPosition2 = coreVector2(m_pCurObject->GetTransform()._31,  m_pCurObject->GetTransform()._32);

                        const coreVector2 vDiff  = vScreenPosition1 - vScreenPosition2;
                        const coreFloat   fDot   = coreVector2::Dot(vMove.Normalized(), vDiff.Normalized());
                        const coreFloat   fValue = vDiff.LengthSq();//(1.1f - POW3(fDot)) * vDiff.LengthSq();

                        if(fDot > 0.01f && fValue < fCurDot)
                        {
                            pCurTarget = (*it);
                            fCurDot    = fValue;
                        }
                    }

                    if(pCurTarget) m_pCurObject = pCurTarget;
                }
            }
        }
        else m_bMoved = false;
    }

    if(m_pCurObject)
    {
        FOR_EACH(it, m_apObject)
            (*it)->SetFocused((*it) == m_pCurObject);


       // const coreVector2 vScreenPosition2 = coreVector2(m_pCurObject->GetTransform()._31,  m_pCurObject->GetTransform()._32);

        this->SetPosition(m_pCurObject->GetPosition() + m_pCurObject->GetSize()*(m_pCurObject->GetAlignment() * 0.5f - coreVector2(0.5f,0.0f))*coreVector2(1.0f,1.0f) + coreVector2(-0.05f,0.0f));
        this->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    }
    else
    {
        this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    if(m_pCurTab)
    {
        m_pCurTab->SetFocused(true);
    }

    this->coreObject2D::Move();
}