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
cBridgeMenu::cBridgeMenu()noexcept
: coreMenu       (SURFACE_BRIDGE_MAX, SURFACE_BRIDGE_ENTER)
, m_fReturnTimer (0.0f)
, m_bReturnState (false)
, m_iTarget      (0u)
, m_bPaused      (false)
, m_bFade        (false)
, m_iUnlockIndex (0u)
, m_fUnlockTime  (0.0f)
, m_iUnlockState (0u)
{
    // create menu objects
    m_InputHeader.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_InputHeader.SetPosition    (coreVector2(0.0f,0.3f));
    m_InputHeader.SetColor3      (COLOR_MENU_WHITE);
    m_InputHeader.SetTextLanguage("ENTER_NAME");

    m_UnlockHeader.Construct      (MENU_FONT_DYNAMIC_5, MENU_OUTLINE_SMALL);
    m_UnlockHeader.SetPosition    (coreVector2(0.0f,0.25f));
    m_UnlockHeader.SetTextLanguage("UNLOCKED");

    m_UnlockName.Construct  (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_UnlockName.SetPosition(coreVector2(0.0f,-0.2f));

    m_UnlockDesc.Construct  (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_UnlockDesc.SetPosition(coreVector2(0.0f,-0.27f));
    m_UnlockDesc.SetColor3  (COLOR_MENU_INSIDE);

    m_UnlockIcon.DefineTexture(0u, "menu_unlocks.png");
    m_UnlockIcon.DefineProgram("menu_grey_program");
    m_UnlockIcon.SetSize      (coreVector2(0.15f,0.15f));

    m_UnlockBack.DefineProgram("menu_color_program");
    m_UnlockBack.SetSize      (m_UnlockIcon.GetSize() + coreVector2(0.01f,0.01f));
    m_UnlockBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.15f);

    // bind menu objects
    this->BindObject(SURFACE_BRIDGE_INPUT, &m_InputHeader);
    this->BindObject(SURFACE_BRIDGE_INPUT, g_pMenu->GetArcadeInput());

    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockHeader);
    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockName);
    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockDesc);
    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockBack);
    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockIcon);
}


// ****************************************************************
// move the bridge menu
void cBridgeMenu::Move()
{
    cArcadeInput* pArcadeInput = g_pMenu->GetArcadeInput();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_BRIDGE_ENTER:
        {
            // 
            m_iStatus = 1;
        }
        break;

    case SURFACE_BRIDGE_RETURN_1:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            // 
            g_pPostProcessing->SetValueAll(BLENDH3(CLAMP01(1.0f - m_fReturnTimer)));

            if(m_fReturnTimer >= 1.5f)
            {
                pArcadeInput->Start(g_pSave->GetHeader().oOptions.acName, 16u);

                g_pMenu->ShiftSurface(this, SURFACE_BRIDGE_INPUT, 1.0f, 0u);
            }
        }
        break;

    case SURFACE_BRIDGE_RETURN_2:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                if(m_bFade)
                {
                    // 
                    g_pPostProcessing->SetValueAll(BLENDH3(CLAMP01(1.0f - m_fReturnTimer)));

                    if(g_pMenu->IsPaused())
                    {
                        g_pGame->MoveOverlay();      
                        ASSERT(STATIC_ISVALID(g_pGame))      
                        ASSERT(g_pMenu->IsPaused())      
                    }
                }

                if(m_fReturnTimer >= 1.5f)
                {
                    if(this->HasUnlocks())
                    {
                        // 
                        this->ShowUnlocks();
                    }
                    else
                    {
                        // 
                        m_iStatus = 2;

                        // 
                        m_bReturnState = false;

                        if(m_bFade)
                        {
                            // 
                            g_pEnvironment->ChangeBackground(cNoBackground::ID,           ENVIRONMENT_MIX_FADE,    0.0f);
                            g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_CURTAIN, 0.75f, coreVector2(1.0f,0.0f));

                            // 
                            cMenu::ClearScreen();
                        }
                    }
                }
            }
        }
        break;

    case SURFACE_BRIDGE_INPUT:
        {
            if(pArcadeInput->GetFinished())
            {
                if(!pArcadeInput->GetText()[0])
                {
                    g_pMenu->GetMsgBox()->ShowQuestion("SKIP NAME ?", [&](const coreInt32 iAnswer)     
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES)
                        {
                            m_iStatus = 110;
                        }
                    });
                }
                else
                {
                    g_pMenu->GetMsgBox()->ShowQuestion("SAVE REPLAY ?", [&](const coreInt32 iAnswer)     
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES)
                        {
                        }
                        m_iStatus = 110;
                    });
                }
            }
        }
        break;

    case SURFACE_BRIDGE_UNLOCK:
        {
            if(m_iUnlockState == 0u)
            {
                const coreFloat fOldTime = m_fUnlockTime;
                m_fUnlockTime.UpdateMin(1.5f, 2.0f);

                if((m_fUnlockTime >= 2.0f) && Core::Input->GetAnyButton(CORE_INPUT_PRESS))
                {
                    m_iUnlockState = 1u;
                }

                if(InBetween(1.1f, fOldTime, m_fUnlockTime)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_UNLOCK);
            }
            else if(m_iUnlockState == 1u)
            {
                m_fUnlockTime.Update(2.0f);

                if(m_fUnlockTime >= 4.0f)
                {
                    // 
                    this->ApplyUnlocks();

                    if(this->HasUnlocks())
                    {
                        // 
                        this->ShowUnlocks(1.0f);
                    }
                    else
                    {
                        if(STATIC_ISVALID(g_pGame))
                        {
                            // 
                            this->ChangeSurface(SURFACE_BRIDGE_RETURN_2, 0.0f);
                        }
                        else
                        {
                            // 
                            m_iStatus = 6;
                        }
                    }
                }
            }

            const coreFloat fHeight = LERPB(-0.04f, 0.01f, STEP(1.0f, 2.0f, m_fUnlockTime));
            const coreFloat fAlpha  = STEPH3(1.0f, 2.0f, m_fUnlockTime) - STEPH3(2.0f, 3.0f, m_fUnlockTime);

            m_UnlockIcon.SetPosition(coreVector2(0.0f, fHeight));
            m_UnlockBack.SetPosition(coreVector2(0.0f, fHeight));

            m_UnlockHeader.SetAlpha(m_UnlockHeader.GetAlpha() * fAlpha);
            m_UnlockName  .SetAlpha(m_UnlockName  .GetAlpha() * fAlpha);
            m_UnlockDesc  .SetAlpha(m_UnlockDesc  .GetAlpha() * fAlpha);
            m_UnlockIcon  .SetAlpha(m_UnlockIcon  .GetAlpha() * fAlpha);
            m_UnlockBack  .SetAlpha(m_UnlockIcon  .GetAlpha() * fAlpha);
        }
        break;

    case SURFACE_BRIDGE_CONTINUE:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                // 
                g_pPostProcessing->SetValueAll(BLENDH3(CLAMP01(1.0f - m_fReturnTimer)));

                if(m_fReturnTimer >= 1.5f)
                {
                    // 
                    m_iStatus = 3;

                    // 
                    m_bReturnState = false;

                    // 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

                    // 
                    cMenu::ClearScreen();
                }
            }
        }
        break;

    case SURFACE_BRIDGE_NEXT:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                // 
                g_pPostProcessing->SetValueAll(BLENDH3(CLAMP01(1.0f - m_fReturnTimer)));

                if(m_fReturnTimer >= 1.5f)
                {
                    // 
                    m_iStatus = 4;

                    // 
                    m_bReturnState = false;

                    // 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

                    // 
                    cMenu::ClearScreen();
                }
            }
        }
        break;

    case SURFACE_BRIDGE_RESTART:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                // 
                g_pPostProcessing->SetValueAll(BLENDH3(CLAMP01(1.0f - m_fReturnTimer)));

                if(g_pMenu->IsPaused())   // TODO 1: mit m_bPaused ersetzen ?
                {
                    g_pGame->MoveOverlay();      
                    ASSERT(STATIC_ISVALID(g_pGame))      
                    ASSERT(g_pMenu->IsPaused())      
                }

                if(m_fReturnTimer >= 1.5f)
                {
                    // 
                    m_iStatus = 5;

                    // 
                    m_bReturnState = false;

                    // 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

                    // 
                    cMenu::ClearScreen();
                }
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(m_iStatus == 10)
    {
        m_fReturnTimer = 0.0f;
        g_pMenu->ShiftSurface(this, SURFACE_BRIDGE_RETURN_2, 1.0f, 0u);
    }
}


// ****************************************************************
// 
void cBridgeMenu::EnterGame(const coreBool bFade)
{
    ASSERT(!STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();
    m_bFade = bFade;

    // 
    if(bFade) g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);

    // 
    this->ChangeSurface(SURFACE_BRIDGE_ENTER, 0.0f);
}


// ****************************************************************
// 
void cBridgeMenu::ReturnMenu(const coreUint8 iTarget, const coreBool bPaused, const coreBool bFade)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();
    m_iTarget = iTarget;
    m_bPaused = bPaused;
    m_bFade   = bFade;

    // 
    g_pGame->GetInterface ()->Reset();
    g_pGame->GetInterface ()->SetVisible(false);
    g_pGame->GetCombatText()->SetVisible(false);

    // 
    if(g_MusicPlayer.GetCurMusic() != g_MusicPlayer.GetMusicName("menu.ogg"))   // TODO 1: [also] condition for finale in demo
        g_pGame->FadeMusic(1.0f);

    // 
    this->ChangeSurface(SURFACE_BRIDGE_RETURN_2, 0.0f);//SURFACE_BRIDGE_RETURN_1, 0.0f);   // TODO 1: skip arcade input
}


// ****************************************************************
// 
void cBridgeMenu::UseContinue()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

    // 
    this->ChangeSurface(SURFACE_BRIDGE_CONTINUE, 0.0f);
}


// ****************************************************************
// 
void cBridgeMenu::UseNext()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

    // 
    g_pGame->GetInterface ()->Reset();
    g_pGame->GetInterface ()->SetVisible(false);
    g_pGame->GetCombatText()->SetVisible(false);

    // 
    this->ChangeSurface(SURFACE_BRIDGE_NEXT, 0.0f);
}


// ****************************************************************
// 
void cBridgeMenu::UseRestart(const coreBool bPaused)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();
    m_bPaused = bPaused;

    // 
    g_pGame->GetInterface ()->Reset();
    g_pGame->GetInterface ()->SetVisible(false);
    g_pGame->GetCombatText()->SetVisible(false);

    // 
    this->ChangeSurface(SURFACE_BRIDGE_RESTART, 0.0f);
}


// ****************************************************************
// 
void cBridgeMenu::ShowUnlocks(const coreFloat fTimeOffset)
{
    // 
    this->SetAlpha(0.0f);

    // 
    m_UnlockName.SetTextLanguage(PRINT("UNLOCK_%02zu_NAME", m_iUnlockIndex + 1u));
    m_UnlockDesc.SetTextLanguage(PRINT("UNLOCK_%02zu_DESC", m_iUnlockIndex + 1u));

    // 
    const coreVector2 vStep      = coreVector2((1.0f/21.0f), (1.0f/21.0f));
    const coreVector2 vTexSize   = coreVector2(4.0f,4.0f);
    const coreVector2 vTexOffset = coreVector2(I_TO_F(m_iUnlockIndex % 4u) * 5.0f + 1.0f, I_TO_F(m_iUnlockIndex / 4u) * 5.0f + 1.0f);

    // 
    m_UnlockIcon.SetTexSize  (vTexSize   * vStep);
    m_UnlockIcon.SetTexOffset(vTexOffset * vStep);

    // 
    m_fUnlockTime  = fTimeOffset;
    m_iUnlockState = 0u;

    // 
    this->ChangeSurface(SURFACE_BRIDGE_UNLOCK, 0.0f);
}


// ****************************************************************
// 
coreBool cBridgeMenu::HasUnlocks()
{
    const auto nApplyUnlockFunc = [&](const coreUintW iIndex)
    {
        if(HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiUnlock, iIndex))
        {
            return false;
        }
        else
        {
            m_iUnlockIndex = iIndex;
            return true;
        }
    };

         if(nApplyUnlockFunc(UNLOCK_MIRRORMORE)  && ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[4][1];})) return true;
    else if(nApplyUnlockFunc(UNLOCK_GAMESPEEDUP) && ALL_MEDAL_SEGMENT(aaiMedal) {return aaiMedal[6][3];})) return true;

    return false;
}


// ****************************************************************
// 
void cBridgeMenu::ApplyUnlocks()
{
    // 
    ADD_BIT_EX(g_pSave->EditProgress()->aiUnlock, m_iUnlockIndex)

    // 
    switch(m_iUnlockIndex)
    {
    case UNLOCK_MIRRORMORE:
        ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_MAIN_CONFIG)
        ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_CONFIG_GAME)
        ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_CONFIG_MIRRORMODE)
        break;

    case UNLOCK_GAMESPEEDUP:
        ADD_BIT_EX(g_pSave->EditProgress()->aiNew, NEW_ARMORY_GAMESPEED)
        break;

    default:
        break;
    }
}


// ****************************************************************
// 
void cBridgeMenu::SetHighlightColor(const coreVector3 vColor)
{
    // 
    m_UnlockHeader.SetColor3(vColor);
    m_UnlockName  .SetColor3(vColor);
}


// ****************************************************************
// 
void cBridgeMenu::__ResetState()
{
    // 
    m_fReturnTimer = 0.0f;
    m_bReturnState = true;
    m_iTarget      = 0u;
    m_bPaused      = false;
    m_bFade        = true;

    // 
    this->SetAlpha(0.0f);
}