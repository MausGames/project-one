///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

coreVector2 cMenuNavigator::s_vMouseMove = coreVector2(0.0f,0.0f);
coreBool    cMenuNavigator::s_bJoystick  = false;


// ****************************************************************
// constructor
cMenuNavigator::cMenuNavigator()noexcept
: m_pCurObject   (NULL)
, m_iStore       (0u)
, m_iFirst       (0u)
, m_bPressed     (false)
, m_vMouseOffset (coreVector2(0.0f,0.0f))
, m_pMenu        (NULL)
{
    // 
    this->DefineTexture(0u, "ship_icon_01.png");
    this->DefineProgram("default_2d_program");
    this->SetSize      (coreVector2(0.06f,0.06f));

    m_aObject.emplace(NULL);
}


// ****************************************************************
// 
void cMenuNavigator::Move()
{
    if(m_pCurObject)// && TIME)// && !bIsShiftingAway)
    {
        const coreVector2 vPosition   = GetTranslation(*m_pCurObject);
        const coreVector2 vOffset     = coreVector2(m_pCurObject->GetSize().x * -0.5f - 0.025f, 0.0f);
        const coreVector2 vResolution = Core::System->GetResolution();

        this->SetPosition (vPosition * RCP(vResolution.Min()) + vOffset);
        this->SetDirection(coreVector2::Direction(1.2f * coreFloat(Core::System->GetTotalTime()))); // TODO 1: should go faster when confirming something

        if(TIME) Core::Input->SetMousePosition((vPosition + m_vMouseOffset) / vResolution);

        m_pCurObject->SetFocused(m_pCurObject->IsFocusable());

        m_iStore = this->__ToIndex(m_pCurObject);
    }

    if(m_pCurObject)
    {
        this->SetEnabled(CORE_OBJECT_ENABLE_ALL);
    }
    else
    {
        this->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    this->coreObject2D::Move();
    
    if(!this->GetAlpha()) 
            Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, false);
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
        if(!m_bPressed && !bNewPressed) Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, true);
        bNewPressed = true;
    };

    if(m_aiLock.size() != Core::Input->GetJoystickNum())
    {
        m_aiLock    .resize(Core::Input->GetJoystickNum(), 0xFFu);
        m_aiLastPack.resize(Core::Input->GetJoystickNum(), 8u);
    }

    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        const coreVector2 vRelative = Core::Input->GetJoystickRelativeL(i);

        const coreVector2 vMove = vRelative.IsNull() ? vRelative : AlongCross(vRelative);
        const coreUint8   iPack = PackDirection(vMove);

        if(vRelative.IsNull() && !Core::Input->GetCountJoystick(i, CORE_INPUT_HOLD))
            REMOVE_BIT(m_aiLock[i], 0u)

        if(!HAS_BIT(m_aiLock[i], 0u))
        {
            if(Core::Input->GetJoystickButton(i, 0u, CORE_INPUT_PRESS)) nPressFunc();

            if(!m_aTab.empty())
            {
                const coreBool bShoulderLeft  = Core::Input->GetJoystickButton(i,  9u, CORE_INPUT_PRESS);
                const coreBool bShoulderRight = Core::Input->GetJoystickButton(i, 10u, CORE_INPUT_PRESS);

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

                    m_pMenu->ChangeSurface(m_aTab.at(pCurTab).iSurface, 0.0f);

                    if(HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_TAB_NODE))
                    {
                        m_pCurObject = this->__ToObject(m_aObject.at(pCurTab).iMoveDown);

                        for(coreUintW j = 0u, je = m_aObject.size(); (j < je) && m_pCurObject && !m_pCurObject->GetAlpha(); ++j)
                        {
                            const coreUint8 iFallback = m_aObject.at(m_pCurObject).iMoveFallback;
                            m_pCurObject = this->__ToObject(iFallback ? iFallback : m_aObject.at(m_pCurObject).iMoveDown);
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
                coreSwitchBoxU8* pSwitchBox = d_cast<coreSwitchBoxU8*>(m_pCurObject);

                //     if(Core::Input->GetJoystickButton(i, 1u, CORE_INPUT_HOLD)) {m_vMouseOffset = GetTranslation(*pSwitchBox->GetArrow(0u)) - GetTranslation(*pSwitchBox); nPressFunc();}
                //else 
                    if(Core::Input->GetJoystickButton(i, 0u, CORE_INPUT_HOLD)) {m_vMouseOffset = GetTranslation(*pSwitchBox->GetArrow(1u)) - GetTranslation(*pSwitchBox); nPressFunc();}
            }

            if(HAS_FLAG(oEntry.eType, MENU_TYPE_SWITCH_MOVE))
            {
                coreSwitchBoxU8* pSwitchBox = d_cast<coreSwitchBoxU8*>(m_pCurObject);

                     if(iPack == 2u) {m_vMouseOffset = GetTranslation(*pSwitchBox->GetArrow(0u)) - GetTranslation(*pSwitchBox); nPressFunc();}
                else if(iPack == 6u) {m_vMouseOffset = GetTranslation(*pSwitchBox->GetArrow(1u)) - GetTranslation(*pSwitchBox); nPressFunc();}
            }

            if((iPack != m_aiLastPack[i]) && (iPack != 8u))
            {
                if(!HAS_BIT(m_aiLock[i], 1u))
                {
                    ADD_BIT(m_aiLock[i], 1u)

                    coreObject2D* pNewObject = NULL;

                    switch(iPack)
                    {
                    default: ASSERT(false)
                    case 0u: pNewObject = this->__ToObject(oEntry.iMoveUp);    break;
                    case 2u: pNewObject = this->__ToObject(oEntry.iMoveLeft);  break;
                    case 4u: pNewObject = this->__ToObject(oEntry.iMoveDown);  break;
                    case 6u: pNewObject = this->__ToObject(oEntry.iMoveRight); break;
                    }

                    for(coreUintW j = 0u, je = m_aObject.size(); (j < je) && pNewObject && !pNewObject->GetAlpha(); ++j)
                    {
                        const coreUint8 iFallback = m_aObject.at(pNewObject).iMoveFallback;
                        if(iFallback)
                        {
                            pNewObject = this->__ToObject(iFallback);
                        }
                        else
                        {
                            switch(iPack)
                            {
                            default: ASSERT(false)
                            case 0u: pNewObject = this->__ToObject(m_aObject.at(pNewObject).iMoveUp);    break;
                            case 2u: pNewObject = this->__ToObject(m_aObject.at(pNewObject).iMoveLeft);  break;
                            case 4u: pNewObject = this->__ToObject(m_aObject.at(pNewObject).iMoveDown);  break;
                            case 6u: pNewObject = this->__ToObject(m_aObject.at(pNewObject).iMoveRight); break;
                            }
                        }
                    }

                    if(!m_pCurObject && !pNewObject)
                    {
                        FOR_EACH(it, m_aObject.get_keylist())
                        {
                            if((*it) && (*it)->GetAlpha())
                            {
                                pNewObject = (*it);
                                break;
                            }
                        }
                    }

                    if(pNewObject && pNewObject->GetAlpha())
                    {
                        m_pCurObject = pNewObject;

                        if(HAS_FLAG(m_aObject.at(m_pCurObject).eType, MENU_TYPE_AUTO_CLICK))
                        {
                            nPressFunc();
                        }
                    }
                }
            }
            else
            {
                REMOVE_BIT(m_aiLock[i], 1u)
            }

            m_aiLastPack[i] = iPack;
        }
    }

    if(s_bJoystick && !m_pCurObject && (m_iStore || m_iFirst))
    {
        m_pCurObject = this->__ToObject(m_iStore ? m_iStore : m_iFirst);
        //for(coreUintW i = 0u, ie = m_aObject.size(); (i < ie) && m_pCurObject && !m_pCurObject->GetAlpha(); ++i)
        //{
        //    const coreUint8 iFallback = m_aObject.at(m_pCurObject).iMoveFallback;
        //    if(!iFallback) break;
        //    m_pCurObject = this->__ToObject(iFallback);
        //}
    }

    if(!s_bJoystick && !m_iFirst)
    {
        m_iStore = 0u;
    }

    if(m_pMenu && !m_pMenu->GetAlpha())
    {
    //    this->ResetFirst();
        std::memset(m_aiLock.data(), 0xFF, m_aiLock.size() * sizeof(coreUint8));
    }

    if(!s_bJoystick) std::memset(m_aiLock.data(), 0xFF, m_aiLock.size() * sizeof(coreUint8));

    if(m_pCurObject && m_pCurObject->GetAlpha())
    {
        const coreVector2 vPosition   = GetTranslation(*m_pCurObject);
        const coreVector2 vResolution = Core::System->GetResolution();

        Core::Input->SetMousePosition((vPosition + m_vMouseOffset) / vResolution);
    }
    
    
    if(m_bPressed)
    {
        if(!bNewPressed) 
            Core::Input->SetMouseButtonNow(CORE_INPUT_LEFT, false);
    }
    m_bPressed = bNewPressed;
}


// ****************************************************************
// 
void cMenuNavigator::BindObject(coreObject2D* pObject, coreObject2D* pUp, coreObject2D* pLeft, coreObject2D* pDown, coreObject2D* pRight, coreObject2D* pFallback, const eMenuType eType)
{
    // 
    if(!m_aObject.count(pUp))       m_aObject.emplace(pUp);
    if(!m_aObject.count(pLeft))     m_aObject.emplace(pLeft);
    if(!m_aObject.count(pDown))     m_aObject.emplace(pDown);
    if(!m_aObject.count(pRight))    m_aObject.emplace(pRight);
    if(!m_aObject.count(pFallback)) m_aObject.emplace(pFallback);

    // 
    sMenuEntry oEntry;
    oEntry.iMoveUp       = this->__ToIndex(pUp);
    oEntry.iMoveLeft     = this->__ToIndex(pLeft);
    oEntry.iMoveDown     = this->__ToIndex(pDown);
    oEntry.iMoveRight    = this->__ToIndex(pRight);
    oEntry.iMoveFallback = this->__ToIndex(pFallback);
    oEntry.eType         = eType;

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
void cMenuNavigator::GlobalUpdate()
{
    // 
    s_vMouseMove += Core::Input->GetMouseRelative().xy() * Core::System->GetResolution();
    if(s_vMouseMove.LengthSq() > POW2(10.0f)) s_bJoystick = false;

    // 
    for(coreUintW i = 0u, ie = Core::Input->GetJoystickNum(); i < ie; ++i)
    {
        if(!Core::Input->GetJoystickRelativeL(i).IsNull() || Core::Input->GetCountJoystick(i, CORE_INPUT_HOLD))
        {
            s_vMouseMove = coreVector2(0.0f,0.0f);
            s_bJoystick  = true;
        }
    }
    
    

    Core::Input->ShowCursor(!s_bJoystick && (g_pMenu->GetCurSurface() != SURFACE_EMPTY)   &&
                                            (g_pMenu->GetCurSurface() != SURFACE_SUMMARY) &&
                                            (g_pMenu->GetCurSurface() != SURFACE_DEFEAT)  &&
                                            (g_pMenu->GetCurSurface() != SURFACE_FINISH)  &&
                                            (g_pMenu->GetCurSurface() != SURFACE_BRIDGE));
}