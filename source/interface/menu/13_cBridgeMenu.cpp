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
{
    // create menu objects
    m_InputHeader.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_InputHeader.SetPosition    (coreVector2(0.0f,0.3f));
    m_InputHeader.SetColor3      (COLOR_MENU_WHITE);
    m_InputHeader.SetTextLanguage("ENTER_NAME");

    m_UnlockHeader.Construct      (MENU_FONT_DYNAMIC_5, MENU_OUTLINE_SMALL);
    m_UnlockHeader.SetPosition    (coreVector2(0.0f,0.3f));
    m_UnlockHeader.SetColor3      (COLOR_MENU_GREEN);                           
    m_UnlockHeader.SetText("UNLOCKED");                           

    m_UnlockName.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_UnlockName.SetPosition    (coreVector2(0.0f,-0.2f));
    m_UnlockName.SetColor3      (COLOR_MENU_GREEN);                           
    m_UnlockName.SetText("MIRROR MODE");                           

    m_UnlockDescription.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_UnlockDescription.SetPosition    (coreVector2(0.0f,-0.27f));
    m_UnlockDescription.SetColor3      (COLOR_MENU_WHITE);
    m_UnlockDescription.SetText("Invert the game horizontally");                           

    m_UnlockIcon.DefineTexture(0u, "environment_clouds_grey.png");
    m_UnlockIcon.DefineProgram("menu_grey_program");
    m_UnlockIcon.SetPosition  (coreVector2(0.0f,0.0f));
    m_UnlockIcon.SetSize      (coreVector2(0.15f,0.15f));

    // bind menu objects
    this->BindObject(SURFACE_BRIDGE_INPUT, &m_InputHeader);
    this->BindObject(SURFACE_BRIDGE_INPUT, g_pMenu->GetArcadeInput());

    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockHeader);
    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockName);
    this->BindObject(SURFACE_BRIDGE_UNLOCK, &m_UnlockDescription);
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

    case SURFACE_BRIDGE_RETURN1:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                // 
                g_pPostProcessing->SetValueAll(CLAMP(1.0f - m_fReturnTimer, 0.0f, 1.0f));

                if(m_fReturnTimer >= 1.5f)
                {
                    pArcadeInput->Start("Martin", 16u);

                    g_pMenu->ShiftSurface(this, SURFACE_BRIDGE_INPUT, 1.0f);
                }
            }
        }
        break;

    case SURFACE_BRIDGE_RETURN2:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                // 
                //g_pPostProcessing->SetValueAll(CLAMP(1.0f - m_fReturnTimer, 0.0f, 1.0f));

                if(m_fReturnTimer >= 1.5f)
                {
                    // 
                    m_iStatus = 2;

                    // 
                    m_bReturnState = false;

                    // 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID,           ENVIRONMENT_MIX_FADE,    0.0f);
                    g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_CURTAIN, 0.75f, coreVector2(1.0f,0.0f));

                    // 
                    g_pPostProcessing->SetSaturationAll(1.0f);
                    g_pPostProcessing->SetValueAll     (1.0f);

                    // prevent flickering (# render function)
                    g_pGlow->Clear();
                    g_pSpecialEffects->ClearAll();
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
                            m_iStatus = 103;
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
                        m_iStatus = 103;
                    });
                }
            }
        }
        break;

    case SURFACE_BRIDGE_UNLOCK:
        {

        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(m_iStatus == 3)
    {
        m_fReturnTimer = 0.0f;
        g_pMenu->ShiftSurface(this, SURFACE_BRIDGE_RETURN2, 1.0f);
    }
}


// ****************************************************************
// 
void cBridgeMenu::EnterGame()
{
    ASSERT(!STATIC_ISVALID(g_pGame))

    // 
    m_fReturnTimer = 0.0f;
    m_bReturnState = false;
    m_iTarget      = 0u;
    m_bPaused      = false;

    if(!g_pSave->GetHeader().oProgress.bFirstPlay)   // TODO 1: replace
    {
        // 
        g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
    }

    // 
    this->SetAlpha(0.0f);
    this->ChangeSurface(SURFACE_BRIDGE_ENTER, 0.0f);
}


// ****************************************************************
// 
void cBridgeMenu::ReturnMenu(const coreUint8 iTarget, const coreBool bPaused)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_fReturnTimer = 0.0f;
    m_bReturnState = true;
    m_iTarget      = iTarget;
    m_bPaused      = bPaused;

    // 
    g_pGame->GetInterface()->SetVisible(false);   // TODO 1: interface does not move during pause, also handle combat text blending   
    g_pGame->GetInterface()->Reset();

    // 
    this->SetAlpha(0.0f);
    this->ChangeSurface(SURFACE_BRIDGE_RETURN1, 0.0f);
}


// ****************************************************************
// 
void cBridgeMenu::ShowUnlock()
{

}


// ****************************************************************
// 
coreBool cBridgeMenu::RequiresUnlock()const
{
    return false;
}