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
cTitleMenu::cTitleMenu()noexcept
: coreMenu           (SURFACE_TITLE_MAX, SURFACE_TITLE_LOGO)
, m_fPromptAnimation (-1.0f)
, m_fPromptExpand    (-1.0f)
{
    // create menu objects
    m_GameLogo.DefineTexture(0u, "game_logo.png");
    m_GameLogo.DefineProgram("default_2d_program");
    m_GameLogo.SetPosition  (coreVector2(0.0f,0.14f));
    m_GameLogo.SetSize      (coreVector2(1.0f,0.25f) * 0.8f);

    m_GameLogoDemo.DefineTexture(0u, "game_logo_demo.png");
    m_GameLogoDemo.DefineProgram("default_2d_program");
    m_GameLogoDemo.SetPosition  (m_GameLogo.GetPosition() + coreVector2(0.0f,-0.1f));
    m_GameLogoDemo.SetSize      (coreVector2(1.0f,0.25f) * 0.3f);

    m_GameLogoKana.DefineTexture(0u, "game_logo_kana.png");
    m_GameLogoKana.DefineProgram("default_2d_program");
    m_GameLogoKana.SetPosition  (m_GameLogo.GetPosition() + coreVector2(0.0f,-0.1f));
    m_GameLogoKana.SetSize      (coreVector2(2.0f,0.25f) * 0.3f);

    m_Shadow.DefineTexture(0u, "effect_headlight_point.png");
    m_Shadow.DefineProgram("menu_single_program");
    m_Shadow.SetPosition  (m_GameLogo.GetPosition());
    m_Shadow.SetSize      (m_GameLogo.GetSize() * coreVector2(1.35f,1.2f));
    m_Shadow.SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_PromptText.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_BIG);
    m_PromptText.SetTextLanguage(DEFINED(_CORE_SWITCH_) ? "PROMPT_SWITCH" : "PROMPT");

    m_aVersionText[0].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_BIG);
    m_aVersionText[0].SetPosition (coreVector2(0.0f, 0.045f));
    m_aVersionText[0].SetCenter   (coreVector2(0.0f,-0.5f));
    m_aVersionText[0].SetAlignment(coreVector2(0.0f, 1.0f));
    m_aVersionText[0].SetColor3   (COLOR_MENU_INSIDE);
    m_aVersionText[0].SetText     (DEFINED(_CORE_SWITCH_) ? "(c) 2010, 2023 Martin Mauersics" : "(c) 2010, 2023 Martin Mauersics (www.maus-games.at)");

    m_aVersionText[1].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_BIG);
    m_aVersionText[1].SetPosition (m_aVersionText[0].GetPosition() + coreVector2(0.0f,-0.03f));
    m_aVersionText[1].SetCenter   (m_aVersionText[0].GetCenter());
    m_aVersionText[1].SetAlignment(m_aVersionText[0].GetAlignment());
    m_aVersionText[1].SetColor3   (COLOR_MENU_INSIDE);
    m_aVersionText[1].SetText     (PRINT("Project One%s%s - v%s third edition - %s %.5s", g_bDemoVersion ? " Demo" : "", DEFINED(_CORE_SWITCH_) ? "" : PRINT(" (%s)", Core::Platform->GetIdentifier()), CoreApp::Settings::Version, __DATE__, __TIME__));

    // bind menu objects
    this->BindObject(SURFACE_TITLE_LOGO,  &m_Shadow);
    this->BindObject(SURFACE_TITLE_LOGO,  &m_GameLogo);
    if(g_bDemoVersion) this->BindObject(SURFACE_TITLE_LOGO, &m_GameLogoDemo);
    else               this->BindObject(SURFACE_TITLE_LOGO, &m_GameLogoKana);
    this->BindObject(SURFACE_TITLE_LOGO,  &m_PromptText);
    this->BindObject(SURFACE_TITLE_LOGO,  &m_aVersionText[0]);
    this->BindObject(SURFACE_TITLE_LOGO,  &m_aVersionText[1]);
    this->BindObject(SURFACE_TITLE_FIRST, &m_PromptText);

    // 
    this->DeactivateFirstPlay();
}


// ****************************************************************
// move the title menu
void cTitleMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_TITLE_LOGO:
    case SURFACE_TITLE_FIRST:
        {
            if(!g_pMenu->IsShifting())
            {
                // 
                m_fPromptAnimation.UpdateMod(1.0f, 2.0f*PI);
            }

            if(m_fPromptExpand >= 0.0f)
            {
                // 
                m_fPromptExpand.Update(5.0f);
                if(m_fPromptExpand >= 1.0f)
                {
                    // 
                    m_iStatus = 1;

                    // 
                    m_fPromptAnimation = -1.0f;
                    m_fPromptExpand    = -1.0f;
                }
            }
            else if((m_fPromptAnimation >= -0.5f) && Core::Input->GetAnyButton(CORE_INPUT_PRESS))
            {
                // 
                m_fPromptExpand = 0.0f;

                // 
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_START);
            }

            // 
            m_PromptText.SetScale (coreVector2(1.0f,1.0f) * LERPB(1.0f, 1.1f, MAX0(m_fPromptExpand)));
            m_PromptText.SetColor3(COLOR_MENU_INSIDE * LERP(MENU_LIGHT_IDLE, MENU_LIGHT_ACTIVE, 0.5f + 0.5f * SIN(10.0f * m_fPromptAnimation)));
            m_PromptText.SetAlpha (m_PromptText.GetAlpha() * MIN1(m_fPromptAnimation + 1.0f) * LERPB(1.0f, 0.0f, MAX0(m_fPromptExpand)));
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    // 
    m_Shadow.SetAlpha(m_Shadow.GetAlpha() * 0.4f);
}


// ****************************************************************
// 
void cTitleMenu::ActivateFirstPlay()
{
    // 
    this->ChangeSurface(SURFACE_TITLE_FIRST, 0.0f);

    // 
    m_PromptText.SetPosition(coreVector2(0.0f,0.0f));
    m_fPromptAnimation = -4.0f;
}


// ****************************************************************
// 
void cTitleMenu::DeactivateFirstPlay()
{
    // 
    this->ChangeSurface(SURFACE_TITLE_LOGO, 0.0f);

    // 
    m_PromptText.SetPosition(coreVector2(0.0f,-0.1f));
    m_fPromptAnimation = -1.0f;
}