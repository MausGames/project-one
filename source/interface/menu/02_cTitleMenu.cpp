///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
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
    m_GameLogo.SetPosition  (coreVector2(0.0f,0.1f));
    m_GameLogo.SetSize      (coreVector2(1.0f,0.25f) * 0.9f);
    m_GameLogo.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 1.05f);

    m_PromptText.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_PromptText.SetTextLanguage("PROMPT");

    m_aVersionText[0].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_aVersionText[0].SetPosition (coreVector2(0.0f, 0.03f));
    m_aVersionText[0].SetCenter   (coreVector2(0.0f,-0.5f) * g_vMenuCenter);
    m_aVersionText[0].SetAlignment(coreVector2(0.0f, 1.0f));
    m_aVersionText[0].SetText     ("(c) 2010-2020 Martin Mauersics (@MausGames)");

    m_aVersionText[1].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_aVersionText[1].SetPosition (m_aVersionText[0].GetPosition() + coreVector2(0.0f,-0.02f));
    m_aVersionText[1].SetCenter   (m_aVersionText[0].GetCenter());
    m_aVersionText[1].SetAlignment(m_aVersionText[0].GetAlignment());
    m_aVersionText[1].SetText     (PRINT("Project One - v0.1.0 third edition - %s %.5s", __DATE__, __TIME__));

    // bind menu objects
    this->BindObject(SURFACE_TITLE_LOGO,  &m_GameLogo);
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
        {

        }
        break;

    case SURFACE_TITLE_FIRST:
        {

        }
        break;

    default:
        ASSERT(false)
        break;
    }

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
    }

    // 
    m_PromptText.SetScale (LERPB(1.0f, 1.1f, MAX(m_fPromptExpand, 0.0f)));
    m_PromptText.SetColor3(coreVector3(1.0f,1.0f,1.0f) * LERP(MENU_LIGHT_IDLE, MENU_LIGHT_ACTIVE, 0.5f + 0.5f * SIN(10.0f * m_fPromptAnimation)));
    m_PromptText.SetAlpha (m_PromptText.GetAlpha() * MIN(m_fPromptAnimation + 1.0f, 1.0f) * LERPB(1.0f, 0.0f, MAX(m_fPromptExpand, 0.0f)));
}


// ****************************************************************
// 
void cTitleMenu::ActivateFirstPlay()
{
    // 
    this->ChangeSurface(SURFACE_TITLE_FIRST, 0.0f);

    // 
    m_PromptText.SetPosition(coreVector2(0.0f,0.0f));
}


// ****************************************************************
// 
void cTitleMenu::DeactivateFirstPlay()
{
    // 
    this->ChangeSurface(SURFACE_TITLE_LOGO, 0.0f);

    // 
    m_PromptText.SetPosition(coreVector2(0.0f,-0.1f));
}