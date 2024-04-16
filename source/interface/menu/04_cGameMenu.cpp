//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cGameMenu::cGameMenu()noexcept
: coreMenu(1u, SURFACE_GAME_DEFAULT)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetSize      (coreVector2(0.8f,0.35f));

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aLabel); ++i)
    {
        m_aLabel[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aLabel[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.05f*I_TO_F(i)));
        m_aLabel[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aLabel[i].SetColor3   (COLOR_MENU_WHITE);

        m_aLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aLabel[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }
    m_aLabel[0].SetTextLanguage("GAME_MODE");
    m_aLabel[1].SetTextLanguage("GAME_DIFFICULTY");
    m_aLabel[2].SetTextLanguage("GAME_PLAYERS");

    m_Mode.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Mode.SetPosition (coreVector2(-1.0f,1.0f) * m_aLabel[0].GetPosition());
    m_Mode.SetSize     (coreVector2(0.26f,0.03f));
    m_Mode.SetAlignment(coreVector2(-1.0f,0.0f));
    m_Mode.SetEndless  (true);
    m_Mode.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_Difficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Difficulty.SetPosition (coreVector2(-1.0f,1.0f) * m_aLabel[1].GetPosition());
    m_Difficulty.SetSize     (m_Mode.GetSize());
    m_Difficulty.SetAlignment(m_Mode.GetAlignment());
    m_Difficulty.SetEndless  (true);
    m_Difficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_Players.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Players.SetPosition (coreVector2(-1.0f,1.0f) * m_aLabel[2].GetPosition());
    m_Players.SetSize     (m_Mode.GetSize());
    m_Players.SetAlignment(m_Mode.GetAlignment());
    m_Players.SetEndless  (true);
    m_Players.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    // fill setup entries
    m_Mode      .AddEntryLanguage("GAME_MODE_STANDARD",      0u);
    m_Mode      .AddEntryLanguage("GAME_MODE_TRAINING",      1u);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",    0u);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_ORIGNAL", 1u);
    m_Players   .AddEntryLanguage("GAME_PLAYERS_SOLO",       1u);
    m_Players   .AddEntryLanguage("GAME_PLAYERS_COOP",       2u);

    // bind menu objects
    this->BindObject(SURFACE_GAME_DEFAULT, &m_Background);
    this->BindObject(SURFACE_GAME_DEFAULT, &m_StartButton);
    this->BindObject(SURFACE_GAME_DEFAULT, &m_BackButton);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aLine);  ++i) this->BindObject(SURFACE_GAME_DEFAULT, &m_aLine [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aLabel); ++i) this->BindObject(SURFACE_GAME_DEFAULT, &m_aLabel[i]);

    this->BindObject(SURFACE_GAME_DEFAULT, &m_Mode);
    this->BindObject(SURFACE_GAME_DEFAULT, &m_Difficulty);
    this->BindObject(SURFACE_GAME_DEFAULT, &m_Players);
}


// ****************************************************************
// move the game menu
void cGameMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_GAME_DEFAULT:
        {

        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(this->GetAlpha() >= 1.0f)
    {
        if(m_StartButton.IsClicked())
        {
            // 
            m_iStatus = 1;
        }
        else if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
        {
            // 
            m_iStatus = 2;
        }
    }

    // 
    cMenu::UpdateButton(&m_StartButton, m_StartButton.IsFocused());
    cMenu::UpdateButton(&m_BackButton,  m_BackButton .IsFocused());

    // 
    cMenu::UpdateSwitchBox(&m_Mode);
    cMenu::UpdateSwitchBox(&m_Difficulty);
    cMenu::UpdateSwitchBox(&m_Players);

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}