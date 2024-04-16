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

    case SURFACE_BRIDGE_RETURN_1:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            // 
            g_pPostProcessing->SetValueAll(BLENDH3(CLAMP01(1.0f - m_fReturnTimer)));

            if(m_fReturnTimer >= 1.5f)
            {
                pArcadeInput->Start("Martin", 16u);

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

    case SURFACE_BRIDGE_RESTART:
        {
            // 
            m_fReturnTimer.Update(1.0f);

            if(m_bReturnState)
            {
                // 
                g_pPostProcessing->SetValueAll(BLENDH3(CLAMP01(1.0f - m_fReturnTimer)));

                if(g_pMenu->IsPaused())
                {
                    g_pGame->MoveOverlay();      
                    ASSERT(STATIC_ISVALID(g_pGame))      
                    ASSERT(g_pMenu->IsPaused())      
                }

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
    if(g_MusicPlayer.GetCurMusic() != g_MusicPlayer.GetMusicName("menu.ogg"))   // TODO 1: condition for finale in demo
        g_pGame->FadeMusic(1.0f);

    // 
    this->ChangeSurface(SURFACE_BRIDGE_RETURN_2, 0.0f);//SURFACE_BRIDGE_RETURN_1, 0.0f);   // TODO 1: skip arcade input
}


// ****************************************************************
// 
void cBridgeMenu::ShowUnlock()
{

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
coreBool cBridgeMenu::RequiresUnlock()const
{
    return false;
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