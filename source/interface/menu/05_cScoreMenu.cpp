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
cScoreMenu::cScoreMenu()noexcept
: coreMenu (SURFACE_SCORE_MAX, SURFACE_SCORE_DEFAULT)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetSize      (coreVector2(0.8f,0.7f));

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f, 0.07f)); // * m_SaveButton.GetSize().y); 
    m_BackButton.SetAlignment (coreVector2(-1.0f, -1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
    {
        m_aRank[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aRank[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.05f*I_TO_F(i)));
        m_aRank[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aRank[i].SetColor3   (COLOR_MENU_WHITE);
        m_aRank[i].SetText     (coreData::ToChars(i + 1u));

        m_aName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition (coreVector2(0.05f,0.0f) + m_aRank[i].GetPosition());
        m_aName[i].SetAlignment(coreVector2(1.0f, 0.0f));
        m_aName[i].SetColor3   (COLOR_MENU_WHITE);
        m_aName[i].SetText     ("Horst");

        m_aScore[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aScore[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aRank[i].GetPosition());
        m_aScore[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aScore[i].SetColor3   (COLOR_MENU_WHITE);
        m_aScore[i].SetText     ("0000000");

        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aRank[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_SCORE_MAX; ++i)
    {
        this->BindObject(i, &m_Background);
        this->BindObject(i, &m_BackButton);
    }

    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aLine [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aRank [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aName [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aScore[i]);
}


// ****************************************************************
// move the score menu
void cScoreMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_SCORE_DEFAULT:
        {

        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
    {
        // 
        m_iStatus = 1;
    }

    // 
    cMenu::UpdateButton(&m_BackButton, m_BackButton.IsFocused());

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_BackButton), TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}