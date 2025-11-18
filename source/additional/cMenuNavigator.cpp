///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector2   cMenuNavigator::s_vMouseMove    = coreVector2(0.0f,0.0f);
coreBool      cMenuNavigator::s_bKeyboard     = false;
coreBool      cMenuNavigator::s_bJoystick     = false;
coreUint8     cMenuNavigator::s_iJoystickType = SDL_GAMEPAD_TYPE_UNKNOWN;
coreObject2D* cMenuNavigator::s_pCurFocus     = NULL;


// ****************************************************************
// constructor
cMenuNavigator::cMenuNavigator()noexcept
: m_pCurObject      (NULL)
, m_iStore          (MENUNAVIGATOR_INVALID)
, m_iFirst          (MENUNAVIGATOR_INVALID)
, m_iBack           (MENUNAVIGATOR_INVALID)
, m_bPressed        (false)
, m_bGrabbed        (false)
, m_dPressTime      (0.0)
, m_fGrabTime       (1.0f)
, m_vGrabColor      (coreVector3(0.0f,0.0f,0.0f))
, m_iLock           (0xFFu)
, m_iLastPack       (8u)
, m_Automatic       (coreTimer(1.0f, 10.0f, 0u))
, m_vMouseOffset    (coreVector2(0.0f,0.0f))
, m_vCurPos         (HIDDEN_POS)
, m_vCurSize        (coreVector2(0.0f,0.0f))
, m_bShowIcon       (false)
, m_pMenu           (NULL)
, m_nShoulderLeft   (NULL)
, m_nShoulderRight  (NULL)
, m_bShoulder       (true)
, m_bIgnoreKeyboard (false)
, m_bActive         (true)
{
    // 
    this->DefineTexture(0u, g_pSpecialEffects->GetIconTexture(0u));
    this->DefineProgram("default_2d_program");
    this->SetSize      (coreVector2(0.07f,0.07f));
    this->SetTexSize   (ICON_TEXSIZE);

    m_aObject.emplace(NULL);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCursor); ++i)
    {
        m_aCursor[i].DefineTexture(0u, "menu_cursor.png");
        m_aCursor[i].DefineProgram("default_2d_program");
        m_aCursor[i].SetDirection (StepRotated90(i));
        m_aCursor[i].SetSize      (coreVector2(1.0f,1.0f) * 0.035f);
    }

    // 
    m_aPrompt[0].SetKey(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
    m_aPrompt[1].SetKey(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
    m_aPrompt[2].SetKey(CORE_INPUT_BUTTON_CANCEL);
}


// ****************************************************************
// 
void cMenuNavigator::Render()
{
    // 
    if(m_pCurObject)
    {
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCursor); ++i)
        {
            m_aCursor[i].SetAlpha(this->GetAlpha() * 0.65f);
            m_aCursor[i].Render();
        }

        // 
        if(m_bShowIcon) this->cGuiObject::Render();
    }

    if(s_bJoystick || DEFINED(_CORE_SWITCH_))
    {
        // 
        if((m_aTab.size() >= 2u) || (m_bShoulder && m_nShoulderLeft))
        {
            m_aPrompt[0].SetAlpha(this->GetAlpha());
            m_aPrompt[0].Render();
        }

        // 
        if((m_aTab.size() >= 2u) || (m_bShoulder && m_nShoulderRight))
        {
            m_aPrompt[1].SetAlpha(this->GetAlpha());
            m_aPrompt[1].Render();
        }

        // 
        if(m_iBack != MENUNAVIGATOR_INVALID)
        {
            m_aPrompt[2].SetAlpha(this->GetAlpha());
            m_aPrompt[2].Render();
        }
    }
}


// ****************************************************************
// 
void cMenuNavigator::Move()
{
    if(m_bGrabbed) m_fGrabTime.UpdateMax(-20.0f, 0.0f);
              else m_fGrabTime.UpdateMin( 20.0f, 1.0f);

    if(m_pCurObject)
    {
        const coreVector2 vPosition   = GetTranslation(*m_pCurObject);
        const coreVector2 vResolution = Core::System->GetResolution();

        coreVector2 vNewPos  = MapToAxisInv(vPosition, g_vHudDirection) / vResolution.Min();
        const coreVector2 vNewSize = m_pCurObject->GetSize() * (HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_BIG) ? 1.5f : 1.0f);

        const coreObject2D* pScroll = this->GetCurScroll();
        if(pScroll)
        {
            const coreVector2 vPos  = MapToAxisInv(GetTranslationArea(*pScroll), g_vHudDirection);
            const coreVector2 vSize = pScroll->GetSize();
            vNewPos.x = CLAMP(vNewPos.x, vPos.x - vSize.x * 0.5f + m_vCurSize.x * 0.5f, vPos.x + vSize.x * 0.5f - m_vCurSize.x * 0.5f);
            vNewPos.y = CLAMP(vNewPos.y, vPos.y - vSize.y * 0.5f + m_vCurSize.y * 0.5f, vPos.y + vSize.y * 0.5f - m_vCurSize.y * 0.5f);
        }

        if(m_vCurPos == HIDDEN_POS)
        {
            m_vCurPos  = vNewPos;
            m_vCurSize = vNewSize;
        }
        else
        {
            const coreVector2 vDiff = vNewPos - m_vCurPos;
            if(vDiff.LengthSq() < POW2(CORE_MATH_PRECISION))
            {
                m_vCurPos  = vNewPos;
                m_vCurSize = vNewSize;
            }
            else
            {
                const coreVector2 vOldPos = m_vCurPos;
                const coreFloat   fLen    = vDiff.Length();

                m_vCurPos  = m_vCurPos  + vDiff.Normalized() * (10.0f * TIME * SmoothTowards(fLen, 0.5f));
                m_vCurSize = m_vCurSize + (vNewSize - m_vCurSize) * ((vOldPos - m_vCurPos).Length() / fLen);
            }
        }

        const coreVector2 vOffset = coreVector2(m_vCurSize.x * -0.5f - 0.03f, 0.0f);

        this->SetPosition (m_vCurPos + vOffset);
        this->SetDirection(coreVector2::Direction(Core::System->GetTotalTimeFloat(20.0*PI_D) * -1.2f + BLENDBR(MAX0(coreFloat(m_dPressTime - Core::System->GetTotalTime()))) * (2.0f*PI) * 0.0f));

        // 
        const coreFloat fMove = LERP(0.002f, 0.005f, (0.5f + 0.5f * SIN(Core::System->GetTotalTimeFloat(1.0) * (2.0f*PI)))) * m_fGrabTime;
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCursor); ++i)
        {
            const coreVector2 vStep  = StepRotated90X(i) * SQRT2;
            m_aCursor[i].SetPosition(m_vCurPos + (m_vCurSize * 0.5f + fMove) * vStep);
        }

        if(TIME)
        {
            coreVector2 vNewTarget = (vPosition + m_vMouseOffset) / vResolution;
            const coreObject2D* pScroll2 = this->GetCurScroll();
            if(pScroll2)
            {
                const coreVector2 vPos  = GetTranslationArea(*pScroll2);
                const coreVector2 vSize = IsHorizontal(g_vHudDirection) ? pScroll2->GetSize().yx() : pScroll2->GetSize();
                vNewTarget.x = CLAMP(vNewTarget.x, vPos.x - vSize.x * 0.5f, vPos.x + vSize.x * 0.5f);
                vNewTarget.y = CLAMP(vNewTarget.y, vPos.y - vSize.y * 0.5f, vPos.y + vSize.y * 0.5f);
            }
            Core::Input->SetMousePosition(vNewTarget);   // for focus
        }

        m_pCurObject->SetFocused(m_pCurObject->IsFocusable());
        
        s_pCurFocus = m_pCurObject;

        m_iStore = this->__ToIndex(m_pCurObject);
    }
    else if(s_bKeyboard || s_bJoystick)
    {
        Core::Input->SetMousePosition(MENUNAVIGATOR_IGNORE_MOUSE);
    }

    if(!m_pCurObject)
    {
        m_vCurPos  = HIDDEN_POS;
        m_vCurSize = coreVector2(0.0f,0.0f);
        m_bGrabbed = false;
    }

    if(m_pCurObject)
    {
        this->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    }
    else
    {
        this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCursor); ++i)
    {
        m_aCursor[i].SetColor3 (m_bGrabbed ? (m_vGrabColor.IsNull() ? g_pMenu->GetHighlightColor() : m_vGrabColor) : coreVector3(1.0f,1.0f,1.0f));
        m_aCursor[i].SetEnabled(this->GetEnabled());
        m_aCursor[i].Move();
    }

    if(s_bJoystick || DEFINED(_CORE_SWITCH_))
    {
        // 
        m_aPrompt[0].SetBase(s_iJoystickType);
        m_aPrompt[1].SetBase(s_iJoystickType);
        m_aPrompt[2].SetBase(s_iJoystickType);

        // 
        coreBool abPress[MENUNAVIGATOR_PROMPTS] = {};
        if(Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,  CORE_INPUT_HOLD)) abPress[0] = true;
        if(Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, CORE_INPUT_HOLD)) abPress[1] = true;
        if(Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_LEFT_TRIGGER,    CORE_INPUT_HOLD)) abPress[0] = true;
        if(Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_RIGHT_TRIGGER,   CORE_INPUT_HOLD)) abPress[1] = true;
        if(Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_CANCEL,          CORE_INPUT_HOLD)) abPress[2] = true;

        // 
        if((m_aTab.size() >= 2u) || (m_bShoulder && m_nShoulderLeft))
        {
            m_aPrompt[0].SetPosition(coreVector2(-0.43f,0.43f));
            m_aPrompt[0].SetSize    (coreVector2(0.1f,0.1f) * (abPress[0] ? 0.8f : 1.0f));
            m_aPrompt[0].Move();
        }

        // 
        if((m_aTab.size() >= 2u) || (m_bShoulder && m_nShoulderRight))
        {
            m_aPrompt[1].SetPosition(coreVector2(0.43f,0.43f));
            m_aPrompt[1].SetSize    (coreVector2(0.1f,0.1f) * (abPress[1] ? 0.8f : 1.0f));
            m_aPrompt[1].Move();
        }

        // 
        if(m_iBack != MENUNAVIGATOR_INVALID)
        {
            m_aPrompt[2].SetPosition(MapToAxisInv(GetTranslationArea(*this->__ToObject(m_iBack)), g_vHudDirection) + coreVector2(0.03f,-0.03f));
            m_aPrompt[2].SetSize    (coreVector2(0.07f,0.07f) * (abPress[2] ? 0.8f : 1.0f));
            m_aPrompt[2].Move();
        }
    }

    // 
    this->cGuiObject::Move();

    if(!this->GetAlpha() && m_bPressed)    // when exiting the pause menu
    {
        Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, false);
        m_bPressed = false;
    }
}


// ****************************************************************
// 
void cMenuNavigator::Update()
{
    if(!TIME) return;

    if(!m_bActive) return;

    if(!s_bKeyboard && !s_bJoystick)
    {
        m_pCurObject = NULL;
    }

    m_vMouseOffset = coreVector2(0.0f,0.0f);

    coreBool bNewPressed = false;
    const auto nPressFunc = [&]()
    {
        if(!g_pMenu->IsShifting() || (g_pMenu->GetShifting().GetValue(CORE_TIMER_GET_NORMAL) > 0.7f))
        {
            if(!m_bPressed && !bNewPressed) Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, true);
        }
        bNewPressed = true;
    };

    coreVector2 vRelative      = Core::Input->GetJoystickStickL(CORE_INPUT_JOYSTICK_ANY);
    coreUint8   iCount         = Core::Input->GetCountJoystick (CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_HOLD);
    coreBool    bButtonA       = Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_ACCEPT,          CORE_INPUT_PRESS);
    coreBool    bButtonB       = Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_CANCEL,          CORE_INPUT_PRESS);
    coreBool    bShoulderLeft  = Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,  CORE_INPUT_PRESS) || Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_LEFT_TRIGGER,  CORE_INPUT_PRESS);
    coreBool    bShoulderRight = Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, CORE_INPUT_PRESS) || Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_RIGHT_TRIGGER, CORE_INPUT_PRESS);

    if(s_bKeyboard && !m_bIgnoreKeyboard)
    {
        if(vRelative.IsNull())
        {
                 if(g_MenuInput.iMove == 1u) vRelative.y =  1.0f;
            else if(g_MenuInput.iMove == 2u) vRelative.x = -1.0f;
            else if(g_MenuInput.iMove == 3u) vRelative.y = -1.0f;
            else if(g_MenuInput.iMove == 4u) vRelative.x =  1.0f;
        }
        iCount   = MAX(iCount, Core::Input->GetCountKeyboard(CORE_INPUT_HOLD));
        bButtonA = bButtonA || g_MenuInput.bAccept;
    }

    {
        const coreVector2 vMove = AlongCross(vRelative);
        const coreUint8   iPack = PackDirection(vMove);

        if(vRelative.IsNull() && !iCount)
        {
            REMOVE_BIT(m_iLock, 0u)
        }

        if((s_bKeyboard || s_bJoystick) && !m_pCurObject && (m_iFirst == MENUNAVIGATOR_INVALID))
        {
            REMOVE_BIT(m_iLock, 0u)
        }

        if(!HAS_BIT(m_iLock, 0u))
        {
            if(bButtonA) nPressFunc();

            if(m_bShoulder && m_nShoulderLeft  && bShoulderLeft)  m_nShoulderLeft ();
            if(m_bShoulder && m_nShoulderRight && bShoulderRight) m_nShoulderRight();

            if(bShoulderLeft || bShoulderRight) m_bGrabbed = false;

            if(!m_aTab.empty())
            {
                if(bShoulderLeft || bShoulderRight)
                {
                    ASSERT(m_pMenu)

                    coreUint8 iCurTab = 0u;
                    for(coreUintW j = 0u, je = m_aTab.size(); j < je; ++j)
                    {
                        if(m_aTab.get_valuelist()[j].iSurface == m_pMenu->GetCurSurface())
                        {
                            iCurTab = j;
                            break;
                        }
                    }

                    coreObject2D* pOldTab = m_aTab.get_keylist()[iCurTab];

                    if(bShoulderLeft) {if(--iCurTab >= m_aTab.size()) iCurTab = m_aTab.size() - 1u;}
                                 else {if(++iCurTab >= m_aTab.size()) iCurTab = 0u;}

                    coreObject2D* pCurTab = m_aTab.get_keylist()[iCurTab];

                    const sMenuTab& oTab = m_aTab.at(pCurTab);
                    cMenu::ChangeTab(m_pMenu, oTab.iSurface);

                    if(HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_TAB_NODE))
                    {
                        if(m_pCurObject) m_aTab.at(pOldTab).iLastEntry = this->__ToIndex(m_pCurObject);
                        m_pCurObject = this->__ToObject((oTab.iLastEntry != MENUNAVIGATOR_INVALID) ? oTab.iLastEntry : oTab.iFallDown);

                        for(coreUintW j = 0u, je = m_aObject.size(); (j < je) && m_pCurObject && !cMenuNavigator::IsValid(m_pCurObject); ++j)
                        {
                            const coreUint8 iFallback = m_aObject.at(m_pCurObject).iMoveDown;
                            if(!iFallback) break;

                            m_pCurObject = this->__ToObject(iFallback);
                        }
                    }
                    else if(HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_TAB_ROOT))
                    {
                        m_pCurObject = pCurTab;
                    }
                }
            }

            const sMenuEntry& oEntry = m_aObject.at(m_pCurObject);

            if(m_pCurObject && cMenuNavigator::IsValid(m_pCurObject))   // TODO 1: for everything in the loop ?
            {
                if(HAS_FLAG(oEntry.eType, MENU_TYPE_SWITCH_PRESS))
                {
                    coreSwitchBoxU8* pSwitchBox = d_cast<coreSwitchBoxU8*>(m_pCurObject);

                    const coreBool bOldGrabbed = m_bGrabbed;

                    if(bButtonA && (pSwitchBox->GetOverride() >= 0))
                    {
                        m_bGrabbed = !m_bGrabbed;
                    }
                    if(bButtonB && m_bGrabbed)
                    {
                        g_MenuInput.bCancel = false;
                        m_bGrabbed = false;
                    }

                    if(bOldGrabbed != m_bGrabbed) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                }

                if(HAS_FLAG(oEntry.eType, MENU_TYPE_SWITCH_MOVE) || (HAS_FLAG(oEntry.eType, MENU_TYPE_SWITCH_PRESS) && m_bGrabbed))
                {
                    coreSwitchBoxU8* pSwitchBox = d_cast<coreSwitchBoxU8*>(m_pCurObject);

                         if(iPack == 2u) {m_vMouseOffset = GetTranslation(*pSwitchBox->GetArrow(0u)) - GetTranslation(*pSwitchBox); nPressFunc();}
                    else if(iPack == 6u) {m_vMouseOffset = GetTranslation(*pSwitchBox->GetArrow(1u)) - GetTranslation(*pSwitchBox); nPressFunc();}
                }
            }

            if((iPack != m_iLastPack) || (iPack == 8u) || m_bGrabbed)
            {
                m_Automatic.Pause();
                m_Automatic.SetValue(CORE_SWITCHBOX_DELAY);
            }

            const coreBool bAuto = m_Automatic.Update(1.0f);

            if((iPack != 8u) && !m_bGrabbed && (bAuto || !m_Automatic.GetStatus()))
            {
                coreObject2D* pNewObject = NULL;

                // TODO 1: allow diagonal input here, for arcade input

                if(m_anDynamic.count(m_pCurObject)) m_anDynamic.at(m_pCurObject)(m_pCurObject, iPack);
                switch(iPack)
                {
                default: UNREACHABLE
                case 0u: pNewObject = this->__ToObject(oEntry.iMoveUp);    break;
                case 2u: pNewObject = this->__ToObject(oEntry.iMoveLeft);  break;
                case 4u: pNewObject = this->__ToObject(oEntry.iMoveDown);  break;
                case 6u: pNewObject = this->__ToObject(oEntry.iMoveRight); break;
                }

                if(m_pMenu && HAS_FLAG(m_aObject.at(pNewObject).eType, MENU_TYPE_TAB_NODE) && (m_aObject.at(pNewObject).iSurface != m_pMenu->GetCurSurface()))
                {
                    for(coreUintW j = 0u, je = m_aTab.size(); j < je; ++j)
                    {
                        if(m_aTab.get_valuelist()[j].iSurface == m_pMenu->GetCurSurface())
                        {
                            switch(iPack)
                            {
                            default: UNREACHABLE
                            case 0u: pNewObject = this->__ToObject(m_aTab.get_valuelist()[j].iFallUp);    break;
                            case 2u: pNewObject = this->__ToObject(m_aTab.get_valuelist()[j].iFallLeft);  break;
                            case 4u: pNewObject = this->__ToObject(m_aTab.get_valuelist()[j].iFallDown);  break;
                            case 6u: pNewObject = this->__ToObject(m_aTab.get_valuelist()[j].iFallRight); break;
                            }
                            break;
                        }
                    }
                }

                for(coreUintW j = 0u, je = m_aObject.size(); (j < je) && pNewObject && !cMenuNavigator::IsValid(pNewObject); ++j)
                {
                    coreObject2D* A;

                    A = this->__ToObject(m_aObject.at(pNewObject).iFallback);
                    if(A && (A != m_pCurObject))
                    {
                        for(coreUintW k = 0u, ke = m_aObject.size(); (k < ke) && A && !cMenuNavigator::IsValid(A); ++k)
                        {
                            A = this->__ToObject(m_aObject.at(A).iFallback);
                        }
                    }
                    
                    if(!A || A == m_pCurObject)
                    {
                        if(m_anDynamic.count(pNewObject)) m_anDynamic.at(pNewObject)(pNewObject, iPack);
                        switch(iPack)
                        {
                        default: UNREACHABLE
                        case 0u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveUp);    break;
                        case 2u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveLeft);  break;
                        case 4u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveDown);  break;
                        case 6u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveRight); break;
                        }
                    }
                    pNewObject = A;
                }

                // prevent automatic exit in scrollbox
                if(bAuto)
                {
                    FOR_EACH(it, m_apScroll)
                    {
                        const cScrollBox* pScroll = d_cast<cScrollBox*>(*it);
                        if(pScroll->IsFocused() && cMenuNavigator::IsValid(pScroll))
                        {
                            if(pScroll->ContainsObject(m_pCurObject) && !pScroll->ContainsObject(pNewObject))
                            {
                                pNewObject = NULL;
                                break;
                            }
                        }
                    }
                }

                if(pNewObject && cMenuNavigator::IsValid(pNewObject))
                {
                    m_pCurObject = pNewObject;

                    if(HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_AUTO_CLICK))
                    {
                        nPressFunc();
                    }
                }
                
                m_Automatic.Play(CORE_TIMER_PLAY_CURRENT);
            }

            m_iLastPack = iPack;
        }
    }

    if((s_bKeyboard || s_bJoystick) && !m_pCurObject && ((m_iStore != MENUNAVIGATOR_INVALID) || (m_iFirst != MENUNAVIGATOR_INVALID)))
    {
        m_pCurObject = this->__ToObject((m_iStore != MENUNAVIGATOR_INVALID) ? m_iStore : m_iFirst);

        if(m_pMenu && HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_TAB_NODE) && (m_aObject.at(m_pCurObject).iSurface != m_pMenu->GetCurSurface()))
        {
            for(coreUintW j = 0u, je = m_aTab.size(); j < je; ++j)
            {
                if(m_aTab.get_valuelist()[j].iSurface == m_pMenu->GetCurSurface())
                {
                    m_pCurObject = this->__ToObject(m_aTab.get_valuelist()[j].iFallDown);
                    break;
                }
            }
        }
    }

    if(!s_bKeyboard && !s_bJoystick && (m_iFirst == MENUNAVIGATOR_INVALID))
    {
        m_iStore = MENUNAVIGATOR_INVALID;
    }

    if(m_pMenu && !m_pMenu->GetAlpha())
    {
        m_iLock = 0xFF;
    }

    if(!s_bKeyboard && !s_bJoystick) m_iLock = 0xFF;

    if(m_pCurObject && cMenuNavigator::IsValid(m_pCurObject))
    {
        const coreVector2 vPosition   = GetTranslation(*m_pCurObject);
        const coreVector2 vResolution = Core::System->GetResolution();

        coreVector2 vNewTarget = (vPosition + m_vMouseOffset) / vResolution;
        const coreObject2D* pScroll = this->GetCurScroll();
        if(pScroll)
        {
            const coreVector2 vPos  = GetTranslationArea(*pScroll);
            const coreVector2 vSize = IsHorizontal(g_vHudDirection) ? pScroll->GetSize().yx() : pScroll->GetSize();
            vNewTarget.x = CLAMP(vNewTarget.x, vPos.x - vSize.x * 0.5f, vPos.x + vSize.x * 0.5f);
            vNewTarget.y = CLAMP(vNewTarget.y, vPos.y - vSize.y * 0.5f, vPos.y + vSize.y * 0.5f);
        }
        Core::Input->SetMousePosition(vNewTarget);   // for focus and click

        if((m_pCurObject->GetAlpha() >= 1.0f) && bNewPressed) m_dPressTime = Core::System->GetTotalTime() + 1.0;
    }
    else if(s_bKeyboard || s_bJoystick)
    {
        Core::Input->SetMousePosition(MENUNAVIGATOR_IGNORE_MOUSE);
    }
    
    if(m_bPressed && !bNewPressed)
    {
        Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, false);
    }
    m_bPressed = bNewPressed;
}


// ****************************************************************
// 
void cMenuNavigator::BindObject(coreObject2D* pObject, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight, coreObject2D* pFallback, const eMenuType eType, const coreUint8 iSurface)
{
    // 
    if(!m_aObject.count(pUp))       m_aObject.emplace(pUp);
    if(!m_aObject.count(pLeft))     m_aObject.emplace(pLeft);
    if(!m_aObject.count(pDown))     m_aObject.emplace(pDown);
    if(!m_aObject.count(pRight))    m_aObject.emplace(pRight);
    if(!m_aObject.count(pFallback)) m_aObject.emplace(pFallback);
    ASSERT(m_aObject.size() <= 0xFFu)

    // 
    sMenuEntry oEntry;
    oEntry.iMoveUp    = this->__ToIndex(pUp);
    oEntry.iMoveLeft  = this->__ToIndex(pLeft);
    oEntry.iMoveDown  = this->__ToIndex(pDown);
    oEntry.iMoveRight = this->__ToIndex(pRight);
    oEntry.iFallback  = this->__ToIndex(pFallback);
    oEntry.eType      = eType;
    oEntry.iSurface   = iSurface;

    // 
    m_aObject[pObject] = oEntry;

    // 
    if(HAS_FLAG(eType, MENU_TYPE_TAB_ROOT))
    {
        ASSERT(!m_aTab.count(pObject))
        m_aTab.emplace(pObject);
    }
    
    if(pObject) pObject->SetAlpha(0.0f);
}


void cMenuNavigator::BindObject(coreObject2D* pObject, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight, const eMenuType eType, const coreUint8 iSurface)
{
    this->BindObject(pObject, pUp, pLeft, pDown, pRight, NULL, eType, iSurface);
}


// ****************************************************************
// 
void cMenuNavigator::BindSurface(coreObject2D* pTab, const coreUint8 iSurface, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight)
{
    ASSERT(m_aTab.count(pTab))

    sMenuTab oTab;
    oTab.iSurface   = iSurface;
    oTab.iFallUp    = this->__ToIndex(pUp);
    oTab.iFallLeft  = this->__ToIndex(pLeft);
    oTab.iFallDown  = this->__ToIndex(pDown);
    oTab.iFallRight = this->__ToIndex(pRight);
    oTab.iLastEntry = MENUNAVIGATOR_INVALID;

    m_aTab.at(pTab) = oTab;
}


// ****************************************************************
// 
void cMenuNavigator::BindScroll(coreObject2D* pScroll)
{
    m_apScroll.insert(pScroll);
}


// ****************************************************************
// 
const coreObject2D* cMenuNavigator::GetCurScroll()const
{
    FOR_EACH(it, m_apScroll)
    {
        const cScrollBox* pScroll = d_cast<cScrollBox*>(*it);
        if(pScroll->IsFocused() && cMenuNavigator::IsValid(pScroll) && pScroll->ContainsObject(m_pCurObject))
        {
            return pScroll;
        }
    }
    return NULL;
}


// ****************************************************************
// 
void cMenuNavigator::GlobalInit()
{
    if(Core::Input->GetJoystickNum())
    {
        s_bJoystick     = true;
        s_iJoystickType = Core::Input->GetJoystickGamepadType(0u);
    }
}


// ****************************************************************
// 
void cMenuNavigator::GlobalUpdate()
{
    if(s_bKeyboard || s_bJoystick)
    {
        // 
        s_vMouseMove += Core::Input->GetMouseRelative().xy() * Core::System->GetResolution();
        if(s_vMouseMove.LengthSq() > POW2(50.0f))
        {
            s_bKeyboard = false;
            s_bJoystick = false;
        }

        // 
        if(Core::Input->GetLastMouse() != CORE_INPUT_INVALID_MOUSE)
        {
            s_bKeyboard = false;
            s_bJoystick = false;
            Core::Input->SetMousePosition(MENUNAVIGATOR_IGNORE_MOUSE);
        }
    }

    // 
    const coreBool bAllowButtons = (!g_pMenu->GetMsgBox()->IsVisible() || (g_pMenu->GetMsgBox()->GetMsgType() != MSGBOX_TYPE_MAPPING)) && !Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_HOLD) && !Core::Input->GetMouseButton(CORE_INPUT_RIGHT, CORE_INPUT_HOLD);

    // 
    if(g_MenuInput.iMove && (g_pMenu->GetCurSurface() != SURFACE_EMPTY) && bAllowButtons)   // # before joystick handling
    {
        s_vMouseMove = coreVector2(0.0f,0.0f);
        s_bKeyboard  = true;
        s_bJoystick  = false;
    }

    // 
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        if(!Core::Input->GetJoystickStickL(i).IsNull() || !Core::Input->GetJoystickStickR(i).IsNull() || (Core::Input->GetCountJoystick(i, CORE_INPUT_PRESS) && bAllowButtons))
        {
            s_vMouseMove    = coreVector2(0.0f,0.0f);
            s_bKeyboard     = false;
            s_bJoystick     = true;
            s_iJoystickType = Core::Input->GetJoystickGamepadType(i);
            break;
        }
    }

    // 
    Core::Input->ShowCursor(!s_bKeyboard && !s_bJoystick && g_pMenu->NeedsCursor());
}