///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector2 cMenuNavigator::s_vMouseMove    = coreVector2(0.0f,0.0f);
coreBool    cMenuNavigator::s_bJoystick     = false;
coreUint8   cMenuNavigator::s_iJoystickType = SDL_CONTROLLER_TYPE_UNKNOWN;


// ****************************************************************
// constructor
cMenuNavigator::cMenuNavigator()noexcept
: m_pCurObject   (NULL)
, m_iStore       (MENUNAVIGATOR_INVALID)
, m_iFirst       (MENUNAVIGATOR_INVALID)
, m_iBack        (MENUNAVIGATOR_INVALID)
, m_bPressed     (false)
, m_bGrabbed     (false)
, m_fGrabTime    (1.0f)
, m_vMouseOffset (coreVector2(0.0f,0.0f))
, m_vCurPos      (HIDDEN_POS)
, m_vCurSize     (coreVector2(0.0f,0.0f))
, m_bShowIcon    (false)
, m_pMenu        (NULL)
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
        m_aCursor[i].SetDirection (StepRotated90(i).InvertedX());
        m_aCursor[i].SetSize      (coreVector2(1.0f,1.0f) * 0.035f);
    }

    // 
    m_aPrompt[0].SetKey(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    m_aPrompt[1].SetKey(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    m_aPrompt[2].SetKey(SDL_CONTROLLER_BUTTON_B);
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
            m_aCursor[i].SetAlpha(this->GetAlpha() * 0.6f);
            m_aCursor[i].Render();
        }
    }

    if(s_bJoystick)
    {
        // 
        if(m_aTab.size() >= 2u)
        {
            m_aPrompt[0].SetAlpha(this->GetAlpha());
            m_aPrompt[0].Render();
            m_aPrompt[1].SetAlpha(this->GetAlpha());
            m_aPrompt[1].Render();
        }

        // 
        if(m_iBack != MENUNAVIGATOR_INVALID)
        {
            m_aPrompt[2].SetAlpha(this->GetAlpha());
            m_aPrompt[2].Render();
        }

        // 
        if(m_bShowIcon) this->cGuiObject::Render();
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

        const coreVector2 vNewPos  = MapToAxis(vPosition, g_vHudDirection) * RCP(vResolution.Min());
        const coreVector2 vNewSize = m_pCurObject->GetSize();

        if(m_vCurPos == HIDDEN_POS)
        {
            m_vCurPos  = vNewPos;
            m_vCurSize = vNewSize;
        }
        else
        {
            const coreVector2 vDiff = vNewPos - m_vCurPos;
            if(!vDiff.IsNull())
            {
                m_vCurPos = m_vCurPos + vDiff.Normalized() * (10.0f * TIME * SmoothTowards(vDiff.Length(), 0.5f));
            }

            const coreVector2 vDiff2 = vNewSize - m_vCurSize;
            if(!vDiff2.IsNull())
            {
                m_vCurSize = m_vCurSize + vDiff2.Normalized() * (10.0f * TIME * SmoothTowards(vDiff2.Length(), 0.5f));
            }
        }

        const coreVector2 vOffset = coreVector2(m_vCurSize.x * -0.5f - 0.03f, 0.0f);

        this->SetPosition (m_vCurPos + vOffset);
        this->SetDirection(coreVector2::Direction(1.2f * coreFloat(Core::System->GetTotalTime()))); // TODO 1: should go faster when confirming something

        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCursor); ++i)
        {
            m_aCursor[i].SetPosition(m_vCurPos + (m_vCurSize * 0.5f + LERP(0.002f, 0.005f, (0.5f + 0.5f * SIN(coreFloat(Core::System->GetTotalTime()) * (2.0f*PI)))) * m_fGrabTime) * StepRotated90X(i) * SQRT2);
        }

        if(TIME)
        {
            const coreBool bInScroll = std::any_of(m_apScroll.begin(), m_apScroll.end(), [](const coreObject2D* A) {return A->IsFocused();});
            Core::Input->SetMousePosition(bInScroll ? MENUNAVIGATOR_IGNORE_MOUSE : ((vPosition + m_vMouseOffset) / vResolution));   // for focus
        }

        m_pCurObject->SetFocused(m_pCurObject->IsFocusable());

        m_iStore = this->__ToIndex(m_pCurObject);
    }
    else if(s_bJoystick)
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
        m_aCursor[i].SetColor3 (m_bGrabbed ? g_pMenu->GetHighlightColor() : coreVector3(1.0f,1.0f,1.0f));
        m_aCursor[i].SetEnabled(this->GetEnabled());
        m_aCursor[i].Move();
    }

    if(s_bJoystick)
    {
        // 
        m_aPrompt[0].SetBase(s_iJoystickType);
        m_aPrompt[1].SetBase(s_iJoystickType);
        m_aPrompt[2].SetBase(s_iJoystickType);

        // 
        coreBool abPress[MENUNAVIGATOR_PROMPTS] = {};
        for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
        {
            if(Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_LEFTSHOULDER,  CORE_INPUT_HOLD)) abPress[0] = true;
            if(Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, CORE_INPUT_HOLD)) abPress[1] = true;
            if(Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_B,             CORE_INPUT_HOLD)) abPress[2] = true;
        }

        // 
        if(m_aTab.size() >= 2u)
        {
            m_aPrompt[0].SetPosition(coreVector2(-0.43f,0.43f));
            m_aPrompt[0].SetSize    (coreVector2(0.1f,0.1f) * (abPress[0] ? 0.8f : 1.0f));
            m_aPrompt[0].Move();

            m_aPrompt[1].SetPosition(coreVector2(0.43f,0.43f));
            m_aPrompt[1].SetSize    (coreVector2(0.1f,0.1f) * (abPress[1] ? 0.8f : 1.0f));
            m_aPrompt[1].Move();
        }

        // 
        if(m_iBack != MENUNAVIGATOR_INVALID)
        {
            m_aPrompt[2].SetPosition(MapToAxis(GetTranslationArea(*this->__ToObject(m_iBack)), g_vHudDirection) + coreVector2(0.03f,-0.03f));
            m_aPrompt[2].SetSize    (coreVector2(0.07f,0.07f) * (abPress[2] ? 0.8f : 1.0f));
            m_aPrompt[2].Move();
        }
    }

    // 
    this->cGuiObject::Move();

    if(!this->GetAlpha()) Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, false);
}


// ****************************************************************
// 
void cMenuNavigator::Update()
{
    if(!TIME) return;

    if(!s_bJoystick)
    {
        m_pCurObject = NULL;
    }

    m_vMouseOffset = coreVector2(0.0f,0.0f);

    coreBool bNewPressed = false;
    const auto nPressFunc = [&]()
    {
        if(!g_pMenu->IsShifting())
            if(!m_bPressed && !bNewPressed) Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, true);
        bNewPressed = true;
    };

    if(m_aiLock.size() != Core::Input->GetJoystickNum())
    {
        m_aiLock    .resize(Core::Input->GetJoystickNum(), 0xFFu);
        m_aiLastPack.resize(Core::Input->GetJoystickNum(), 8u);
        m_aAutomatic.resize(Core::Input->GetJoystickNum(), coreTimer(1.0f, 10.0f, 0u));
    }

    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        const coreVector2 vRelative = Core::Input->GetJoystickRelativeL(i);

        const coreVector2 vMove = vRelative.IsNull() ? coreVector2(0.0f,0.0f) : AlongCross(vRelative);
        const coreUint8   iPack = PackDirection(vMove);

        if(vRelative.IsNull() && !Core::Input->GetCountJoystick(i, CORE_INPUT_HOLD))
        {
            REMOVE_BIT(m_aiLock[i], 0u)
        }

        if(s_bJoystick && !m_pCurObject && (m_iFirst == MENUNAVIGATOR_INVALID))
        {
            REMOVE_BIT(m_aiLock[i], 0u)
        }

        if(!HAS_BIT(m_aiLock[i], 0u))
        {
            if(Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_A, CORE_INPUT_PRESS)) nPressFunc();

            if(!m_aTab.empty())
            {
                const coreBool bShoulderLeft  = Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_LEFTSHOULDER,  CORE_INPUT_PRESS);
                const coreBool bShoulderRight = Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, CORE_INPUT_PRESS);

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

                    if(bShoulderLeft) {if(--iCurTab >= m_aTab.size()) iCurTab = m_aTab.size() - 1u;}
                                 else {if(++iCurTab >= m_aTab.size()) iCurTab = 0u;}

                    coreObject2D* pCurTab = m_aTab.get_keylist()[iCurTab];

                    cMenu::ChangeTab(m_pMenu, m_aTab.at(pCurTab).iSurface);

                    if(HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_TAB_NODE))
                    {
                        m_pCurObject = this->__ToObject(m_aTab.at(pCurTab).iFallDown);

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

            if(HAS_FLAG(oEntry.eType, MENU_TYPE_SWITCH_PRESS))
            {
                const coreBool bOldGrabbed = m_bGrabbed;

                if(Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_A, CORE_INPUT_PRESS))
                {
                    m_bGrabbed = !m_bGrabbed;
                }
                if(Core::Input->GetJoystickButton(i, SDL_CONTROLLER_BUTTON_B, CORE_INPUT_PRESS) && m_bGrabbed)
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

            if((iPack != m_aiLastPack[i]) || (iPack == 8u) || m_bGrabbed)
            {
                m_aAutomatic[i].Pause();
                m_aAutomatic[i].SetValue(CORE_SWITCHBOX_DELAY);
            }

            if((iPack != 8u) && !m_bGrabbed && (m_aAutomatic[i].Update(1.0f) || !m_aAutomatic[i].GetStatus()))
            {
                coreObject2D* pNewObject = NULL;

                // TODO 1: erlaub schräge eingabe hier, für arcade input

                switch(iPack)
                {
                default: ASSERT(false)
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
                            default: ASSERT(false)
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
                    switch(iPack)
                    {
                    default: ASSERT(false)
                    case 0u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveUp);    break;
                    case 2u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveLeft);  break;
                    case 4u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveDown);  break;
                    case 6u: A = this->__ToObject(m_aObject.at(pNewObject).iMoveRight); break;
                    }
                    pNewObject = A;
                }

                //if(!m_pCurObject && !pNewObject)
                //{
                //    FOR_EACH(it, m_aObject.get_keylist())
                //    {
                //        if((*it) && (*it)->GetAlpha())
                //        {
                //            pNewObject = (*it);
                //            break;
                //        }
                //    }
                //}

                if(pNewObject && cMenuNavigator::IsValid(pNewObject))
                {
                    m_pCurObject = pNewObject;

                    if(HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_AUTO_CLICK))
                    {
                        nPressFunc();
                    }
                }
                
                m_aAutomatic[i].Play(CORE_TIMER_PLAY_CURRENT);
            }

            m_aiLastPack[i] = iPack;
        }
    }

    if(s_bJoystick && !m_pCurObject && ((m_iStore != MENUNAVIGATOR_INVALID) || (m_iFirst != MENUNAVIGATOR_INVALID)))
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

    if(!s_bJoystick && (m_iFirst == MENUNAVIGATOR_INVALID))
    {
        m_iStore = MENUNAVIGATOR_INVALID;
    }

    if(m_pMenu && !m_pMenu->GetAlpha())
    {
        std::memset(m_aiLock.data(), 0xFF, m_aiLock.size() * sizeof(coreUint8));
    }

    if(!s_bJoystick) std::memset(m_aiLock.data(), 0xFF, m_aiLock.size() * sizeof(coreUint8));

    if(m_pCurObject && cMenuNavigator::IsValid(m_pCurObject))
    {
        const coreVector2 vPosition   = GetTranslation(*m_pCurObject);
        const coreVector2 vResolution = Core::System->GetResolution();

        const coreBool bInScroll = std::any_of(m_apScroll.begin(), m_apScroll.end(), [](const coreObject2D* A) {return A->IsFocused();});
        Core::Input->SetMousePosition(bInScroll ? MENUNAVIGATOR_IGNORE_MOUSE : (vPosition + m_vMouseOffset) / vResolution);   // for focus and click
    }
    else if(s_bJoystick)
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
void cMenuNavigator::BindObject(coreObject2D* pObject, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight, const eMenuType eType, const coreUint8 iSurface)
{
    // 
    if(!m_aObject.count(pUp))    m_aObject.emplace(pUp);
    if(!m_aObject.count(pLeft))  m_aObject.emplace(pLeft);
    if(!m_aObject.count(pDown))  m_aObject.emplace(pDown);
    if(!m_aObject.count(pRight)) m_aObject.emplace(pRight);

    // 
    sMenuEntry oEntry;
    oEntry.iMoveUp       = this->__ToIndex(pUp);
    oEntry.iMoveLeft     = this->__ToIndex(pLeft);
    oEntry.iMoveDown     = this->__ToIndex(pDown);
    oEntry.iMoveRight    = this->__ToIndex(pRight);
    oEntry.eType         = eType;
    oEntry.iSurface      = iSurface;

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
void cMenuNavigator::GlobalInit()
{
    if(Core::Input->GetJoystickNum())
    {
        s_bJoystick     = true;
        s_iJoystickType = Core::Input->GetJoystickGamepadType(0);
    }
}


// ****************************************************************
// 
void cMenuNavigator::GlobalUpdate()
{
    if(s_bJoystick)
    {
        // 
        s_vMouseMove += Core::Input->GetMouseRelative().xy() * Core::System->GetResolution();
        if(s_vMouseMove.LengthSq() > POW2(10.0f))
        {
            s_bJoystick = false;
        }

        // 
        if(Core::Input->GetLastMouse() != CORE_INPUT_INVALID_MOUSE)
        {
            s_bJoystick = false;
            Core::Input->SetMousePosition(MENUNAVIGATOR_IGNORE_MOUSE);
        }
    }

    // 
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        if(!Core::Input->GetJoystickRelativeL(i).IsNull() || Core::Input->GetCountJoystick(i, CORE_INPUT_HOLD))
        {
            s_vMouseMove    = coreVector2(0.0f,0.0f);
            s_bJoystick     = true;
            s_iJoystickType = Core::Input->GetJoystickGamepadType(i);
            break;
        }
    }

    // 
    Core::Input->ShowCursor(!s_bJoystick && (g_pMenu->GetCurSurface() != SURFACE_EMPTY)   &&
                                            (g_pMenu->GetCurSurface() != SURFACE_SUMMARY) &&   // TODO 1: not for segment summary
                                            (g_pMenu->GetCurSurface() != SURFACE_DEFEAT)  &&
                                            (g_pMenu->GetCurSurface() != SURFACE_BRIDGE));
}