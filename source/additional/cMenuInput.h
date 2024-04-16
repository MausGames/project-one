///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD__H_   
#define _P1_GUARD__H_   


// ****************************************************************
// 
class cTest final
{
private:


    static coreBool s_bEnabled;   // 


public:
    cBindContainerIn() = default;
    inline ~cBindContainerIn();

    ENABLE_COPY(cBindContainerIn)


};



        /*

        FOR_EACH_REV(it, m_papObject[m_iCurSurface])
        {
            if((*it) == m_pCurObject) continue;

            //if(!Core::Input->GetMouseRelative().IsNull())
                (*it)->Interact();

            if((*it)->IsFocused())
            {
                m_pCurObject = (*it);
                break;
            }
        }
        
        coreBool bReset = true;
        for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
        {
            Core::Input->ForwardHatToStick(i);

            if(Core::Input->GetJoystickButton(i, 0u, CORE_INPUT_PRESS))   Core::Input->SetMouseButton(CORE_INPUT_LEFT,  true);
            if(Core::Input->GetJoystickButton(i, 0u, CORE_INPUT_RELEASE)) Core::Input->SetMouseButton(CORE_INPUT_LEFT,  false);
            if(Core::Input->GetJoystickButton(i, 1u, CORE_INPUT_PRESS))   Core::Input->SetMouseButton(CORE_INPUT_RIGHT, true);
            if(Core::Input->GetJoystickButton(i, 1u, CORE_INPUT_RELEASE)) Core::Input->SetMouseButton(CORE_INPUT_RIGHT, false);

            const coreVector2& vRelative = Core::Input->GetJoystickRelative(i);

            if(!vRelative.IsNull())
            {
                bReset = false;
                if(!s_bMoved)
                {
                    if(m_pCurObject == NULL)
                    {
                        FOR_EACH(it, m_papObject[m_iCurSurface])
                        {
                            if((*it)->IsFocusable())
                            {
                                m_pCurObject = (*it);
                                break;
                            }
                        }

                        s_bMoved = true;
                        break;
                    }

                    coreObject2D* pCurTarget = NULL;
                    coreFloat     fCurDot    = FLT_MAX;

                    FOR_EACH(it, m_papObject[m_iCurSurface])
                    {
                        if((*it) == m_pCurObject) continue;
                        if(!(*it)->IsFocusable()) continue;

                        const coreVector2 vScreenPosition1 = coreVector2((*it)       ->GetTransform()._31,  (*it)       ->GetTransform()._32);
                        const coreVector2 vScreenPosition2 = coreVector2(m_pCurObject->GetTransform()._31,  m_pCurObject->GetTransform()._32);

                        const coreVector2 vDiff  = vScreenPosition1 - vScreenPosition2;
                        const coreFloat   fDot   = coreVector2::Dot(vRelative.Normalized(), vDiff.Normalized());
                        const coreFloat   fValue = (1.1f - POW3(fDot)) * vDiff.LengthSq();

                        if(fDot > 0.01f && fValue < fCurDot)
                        {
                            pCurTarget = (*it);
                            fCurDot    = fValue;
                        }
                    }

                    if(pCurTarget) m_pCurObject = pCurTarget;

                    s_bMoved = true;
                }

                break;
            }
        }
        if(bReset) s_bMoved = false;

        FOR_EACH(it, m_papObject[m_iCurSurface])
            (*it)->SetFocused((*it) == m_pCurObject);



        // move current surface
        FOR_EACH_REV(it, m_papObject[m_iCurSurface])
        {
            coreObject2D* pObject = (*it);

            // find current object with input focus
            //if(!m_pCurObject)
            //{
            //    // interact and check status
            //    pObject->Interact();
            //    if(pObject->IsFocused()) m_pCurObject = pObject;
            //}
            //else pObject->SetFocus(false);

            // update object
            pObject->SetAlpha(this->GetAlpha());
            pObject->Move();
        }


        */


#endif // _P1_GUARD__H_   