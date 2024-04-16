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
cMainMenu::cMainMenu()noexcept
: coreMenu (SURFACE_MAIN_MAX, SURFACE_MAIN_DEFAULT)
{
    // create menu objects
    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetSize      (coreVector2(0.4f,0.07f));
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_ScoreButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ScoreButton.DefineProgram("menu_border_program");
    m_ScoreButton.SetSize      (m_StartButton.GetSize());

    m_ReplayButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ReplayButton.DefineProgram("menu_border_program");
    m_ReplayButton.SetSize      (m_StartButton.GetSize());

    m_ExtraButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExtraButton.DefineProgram("menu_border_program");
    m_ExtraButton.SetSize      (m_StartButton.GetSize());

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetSize      (m_StartButton.GetSize());
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_SteamButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_SteamButton.DefineProgram("menu_border_program");
    m_SteamButton.SetSize      (m_StartButton.GetSize());
    m_SteamButton.GetCaption()->SetTextLanguage(cMenu::GetStoreText());

    m_CreditsButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_CreditsButton.DefineProgram("menu_border_program");
    m_CreditsButton.SetSize      (m_StartButton.GetSize());
    m_CreditsButton.GetCaption()->SetTextLanguage("CREDITS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetSize      (m_StartButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    // 
    if(g_bDemoVersion)
    {
        m_Navigator.BindObject(&m_StartButton,   &m_ExitButton,    NULL, &m_ConfigButton,  NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ConfigButton,  &m_StartButton,   NULL, &m_SteamButton,   NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_SteamButton,   &m_ConfigButton,  NULL, &m_CreditsButton, NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_CreditsButton, &m_SteamButton,   NULL, &m_ExitButton,    NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ExitButton,    &m_CreditsButton, NULL, &m_StartButton,   NULL, MENU_TYPE_DEFAULT);
    }
    else if(DEFINED(_CORE_DEBUG_))
    {
        m_Navigator.BindObject(&m_StartButton,  &m_ExitButton,   NULL, &m_ScoreButton,  NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ScoreButton,  &m_StartButton,  NULL, &m_ReplayButton, NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ReplayButton, &m_ScoreButton,  NULL, &m_ExtraButton,  NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ExtraButton,  &m_ReplayButton, NULL, &m_ConfigButton, NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ConfigButton, &m_ExtraButton,  NULL, &m_ExitButton,   NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ExitButton,   &m_ConfigButton, NULL, &m_StartButton,  NULL, MENU_TYPE_DEFAULT);
    }
    else if(g_bLeaderboards)
    {
        m_Navigator.BindObject(&m_StartButton,  &m_ExitButton,   NULL, &m_ScoreButton,  NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ScoreButton,  &m_StartButton,  NULL, &m_ExtraButton,  NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ExtraButton,  &m_ScoreButton,  NULL, &m_ConfigButton, NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ConfigButton, &m_ExtraButton,  NULL, &m_ExitButton,   NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ExitButton,   &m_ConfigButton, NULL, &m_StartButton,  NULL, MENU_TYPE_DEFAULT);
    }
    else
    {
        m_Navigator.BindObject(&m_StartButton,  &m_ExitButton,   NULL, &m_ExtraButton,  NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ExtraButton,  &m_StartButton,  NULL, &m_ConfigButton, NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ConfigButton, &m_ExtraButton,  NULL, &m_ExitButton,   NULL, MENU_TYPE_DEFAULT);
        m_Navigator.BindObject(&m_ExitButton,   &m_ConfigButton, NULL, &m_StartButton,  NULL, MENU_TYPE_DEFAULT);
    }

    m_Navigator.AssignFirst(&m_StartButton);
    m_Navigator.ShowIcon   (true);

    // bind menu objects
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_StartButton);

    if(g_bDemoVersion)
    {
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ConfigButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_SteamButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_CreditsButton);
    }
    else if(DEFINED(_CORE_DEBUG_))
    {
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ScoreButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ReplayButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExtraButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ConfigButton);
    }
    else if(g_bLeaderboards)
    {
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ScoreButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExtraButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ConfigButton);
    }
    else
    {
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExtraButton);
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ConfigButton);
    }

    #if !defined(_CORE_EMSCRIPTEN_) && !defined(_CORE_SWITCH_)
        this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExitButton);
    #endif

    this->BindObject(SURFACE_MAIN_DEFAULT, &m_StartNew);
    if(g_bLeaderboards) this->BindObject(SURFACE_MAIN_DEFAULT, &m_ScoreNew);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ExtraNew);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_ConfigNew);
    this->BindObject(SURFACE_MAIN_DEFAULT, &m_Navigator);

    // 
    coreSet<coreObject2D*>* pObjectSet = this->GetObjectSet(SURFACE_MAIN_DEFAULT);
    for(coreUintW i = 0u, ie = pObjectSet->size() - (g_bLeaderboards ? 5u : 4u); i < ie; ++i)
    {
        (*pObjectSet)[i]->SetPosition(coreVector2(0.0f, 0.09f * (I_TO_F(ie - 1u) * 0.5f - I_TO_F(i))));
    }

    m_StartNew.SetPosition(m_StartButton.GetPosition() + m_StartButton.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(0.07f,0.0f));
    m_StartNew.SetIndex   (NEW_MAIN_START);

    m_ScoreNew.SetPosition(m_ScoreButton.GetPosition() + m_ScoreButton.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(0.07f,0.0f));
    m_ScoreNew.SetIndex   (NEW_MAIN_SCORE);

    m_ExtraNew.SetPosition(m_ExtraButton.GetPosition() + m_ExtraButton.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(0.07f,0.0f));
    m_ExtraNew.SetIndex   (NEW_MAIN_EXTRA);

    m_ConfigNew.SetPosition(m_ConfigButton.GetPosition() + m_ConfigButton.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(0.07f,0.0f));
    m_ConfigNew.SetIndex   (NEW_MAIN_CONFIG);

    // 
    this->DeactivateFirstPlay();
}


// ****************************************************************
// move the main menu
void cMainMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_MAIN_DEFAULT:
        {
            if(m_StartButton.IsClicked())
            {
                // 
                m_iStatus = 1;

                // 
                m_StartNew.Resolve();
            }
            else if(m_ScoreButton.IsClicked())
            {
                // 
                m_iStatus = 2;

                // 
                m_ScoreNew.Resolve();
            }
            else if(m_ReplayButton.IsClicked())
            {
                // 
                m_iStatus = 3;
            }
            else if(m_ExtraButton.IsClicked())
            {
                // 
                m_iStatus = 4;

                // 
                m_ExtraNew.Resolve();
            }
            else if(m_ConfigButton.IsClicked())
            {
                // 
                m_iStatus = 5;

                // 
                m_ConfigNew.Resolve();
            }
            else if(m_SteamButton.IsClicked())
            {
                // 
                SDL_OpenURL(cMenu::GetStoreLink());
            }
            else if(m_CreditsButton.IsClicked())
            {
                // 
                m_iStatus = 6;
            }
            else if(m_ExitButton.IsClicked())
            {
                // 
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_EXIT_GAME"), [](const coreInt32 iAnswer)
                {
                    if(iAnswer == MSGBOX_ANSWER_YES)
                        Core::System->Quit();
                });
            }

            // 
            cMenu::UpdateButton(&m_StartButton,   &m_Navigator, m_StartButton  .IsFocused());
            cMenu::UpdateButton(&m_ScoreButton,   &m_Navigator, m_ScoreButton  .IsFocused());
            cMenu::UpdateButton(&m_ReplayButton,  &m_Navigator, m_ReplayButton .IsFocused());
            cMenu::UpdateButton(&m_ExtraButton,   &m_Navigator, m_ExtraButton  .IsFocused());
            cMenu::UpdateButton(&m_ConfigButton,  &m_Navigator, m_ConfigButton .IsFocused());
            cMenu::UpdateButton(&m_SteamButton,   &m_Navigator, m_SteamButton  .IsFocused());
            cMenu::UpdateButton(&m_CreditsButton, &m_Navigator, m_CreditsButton.IsFocused());
            cMenu::UpdateButton(&m_ExitButton,    &m_Navigator, m_ExitButton   .IsFocused());
        }
        break;

    default:
        ASSERT(false)
        break;
    }
}


// ****************************************************************
// 
void cMainMenu::ActivateFirstPlay()
{
    // 
    m_ScoreButton .SetOverride(-1);
    m_ReplayButton.SetOverride(-1);
    m_ExtraButton .SetOverride(-1);

    // 
    m_ScoreButton .GetCaption()->SetTextLanguage("UNKNOWN");
    m_ReplayButton.GetCaption()->SetTextLanguage("UNKNOWN");
    m_ExtraButton .GetCaption()->SetTextLanguage("UNKNOWN");
}


// ****************************************************************
// 
void cMainMenu::DeactivateFirstPlay()
{
    // 
    m_ScoreButton .SetOverride(0);
    m_ReplayButton.SetOverride(0);
    m_ExtraButton .SetOverride(0);

    // 
    m_ScoreButton .GetCaption()->SetTextLanguage("LEADERBOARDS");
    m_ReplayButton.GetCaption()->SetTextLanguage("REPLAYS");
    m_ExtraButton .GetCaption()->SetTextLanguage("EXTRAS");
}


// ****************************************************************
// 
void cMainMenu::ActivateDemoVersion()
{
    // 
    this->ActivateFirstPlay();
}