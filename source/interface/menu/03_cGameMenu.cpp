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
: coreMenu (2u, SURFACE_GAME_MISSION)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.05f));
    m_Background.SetSize      (coreVector2(0.8f,0.8f));


    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTest); ++i) 
    {
        m_aTest[i].DefineTexture(0u, "menu_detail_03.png");
        m_aTest[i].DefineTexture(1u, "menu_background_black.png");
        m_aTest[i].DefineProgram("menu_inner_program");
        m_aTest[i].SetPosition  (coreVector2(0.0f, 0.3f - I_TO_F(i)*0.16f*0.75f));
        m_aTest[i].SetSize      (coreVector2(4.0f,0.8f * 0.65f) * 0.2f);
        //m_aTest[i].SetTexSize   (coreVector2(1.0f,0.5f));
         

        m_aTest2[i].DefineTexture(0u, "menu_weapons.png");
        m_aTest2[i].DefineProgram("default_2d_program");
        m_aTest2[i].SetPosition  (coreVector2(-0.33f, m_aTest[i].GetPosition().y));
        m_aTest2[i].SetSize      (coreVector2(1.0f,1.0f) * 0.065f);
        m_aTest2[i].SetTexSize   (coreVector2(0.25f,0.5f));


        m_aTest3[i].DefineTexture(0u, "menu_weapons.png");
        m_aTest3[i].DefineProgram("default_2d_program");
        m_aTest3[i].SetPosition  (coreVector2(-0.33f, m_aTest[i].GetPosition().y));
        m_aTest3[i].SetSize      (coreVector2(1.0f,1.0f) * 0.065f + coreVector2(0.01f,0.01f));
        m_aTest3[i].SetColor3    (coreVector3(0.0f,0.0f,0.0f));
        m_aTest3[i].SetTexSize   (coreVector2(0.25f,0.5f));
        m_aTest3[i].SetTexOffset (coreVector2(0.75f,0.5f));

        m_aTest4[i].Construct   (MENU_FONT_BIG_4, MENU_OUTLINE_BIG, 0u);
        m_aTest4[i].SetPosition (coreVector2(0.36f, m_aTest[i].GetPosition().y));
        m_aTest4[i].SetAlignment(coreVector2(-1.0f,0.0f));
    }
    m_aTest2[0].SetColor3   (COLOR_MENU_YELLOW);
    m_aTest2[1].SetColor3   (COLOR_MENU_PURPLE);
    m_aTest2[2].SetColor3   (COLOR_MENU_GREEN);
    m_aTest2[3].SetColor3   (COLOR_MENU_BLUE);
    m_aTest2[4].SetColor3   (COLOR_MENU_RED);
    m_aTest2[0].SetTexOffset(coreVector2(0.0f, 0.0f));
    m_aTest2[1].SetTexOffset(coreVector2(0.25f,0.0f));
    m_aTest2[2].SetTexOffset(coreVector2(0.5f, 0.0f));
    m_aTest2[3].SetTexOffset(coreVector2(0.75f,0.0f));
    m_aTest2[4].SetTexOffset(coreVector2(0.0f, 0.5f));

    m_aTest4[0].SetColor3   (COLOR_MENU_YELLOW);
    m_aTest4[1].SetColor3   (COLOR_MENU_PURPLE);
    m_aTest4[2].SetColor3   (COLOR_MENU_GREEN);
    m_aTest4[3].SetColor3   (COLOR_MENU_BLUE);
    m_aTest4[4].SetColor3   (COLOR_MENU_RED);
    m_aTest4[0].SetText("Ray");
    m_aTest4[1].SetText("Pulse");
    m_aTest4[2].SetText("Wave");
    m_aTest4[3].SetText("Tesla");
    m_aTest4[4].SetText("Antimatter");
    



    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (coreVector2(0.0f,-0.28f));
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f) * 1.1f);
    m_StartButton.GetCaption()->SetTextLanguage("START_GAME");

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_ConfigButton.SetSize      (coreVector2(0.3f,0.07f));
    m_ConfigButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_ExtraButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_ExtraButton.DefineProgram("menu_border_program");
    m_ExtraButton.SetPosition  (m_ConfigButton.GetPosition() + coreVector2(m_ConfigButton.GetSize().x + 0.02f,0.0f));
    m_ExtraButton.SetSize      (m_ConfigButton.GetSize());
    m_ExtraButton.SetAlignment (m_ConfigButton.GetAlignment());
    m_ExtraButton.GetCaption()->SetTextLanguage("EXTRAS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_ICON_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_ExitButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_ConfigButton.GetSize().y);
    m_ExitButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_ExitButton.GetCaption()->SetText(ICON_POWER_OFF);

    // bind menu objects
    this->BindObject(SURFACE_GAME_MISSION, &m_Background);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTest);  ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aTest[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTest3); ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aTest3[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTest2); ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aTest2[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTest4); ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aTest4[i]);
    this->BindObject(SURFACE_GAME_MISSION, &m_StartButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ConfigButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ExtraButton);
    this->BindObject(SURFACE_GAME_MISSION, &m_ExitButton);
}


// ****************************************************************
// move the game menu
void cGameMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;


    m_aTest4[0].SetAlpha(0.5f);
    m_aTest4[1].SetAlpha(0.5f);
    m_aTest4[2].SetAlpha(0.5f);
    m_aTest4[3].SetAlpha(0.5f);
    m_aTest4[4].SetAlpha(0.5f);


    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_GAME_MISSION:
        {
            // 
            cMenu::UpdateButton(&m_StartButton,  m_StartButton .IsFocused());
            cMenu::UpdateButton(&m_ConfigButton, m_ConfigButton.IsFocused());
            cMenu::UpdateButton(&m_ExtraButton,  m_ExtraButton .IsFocused());
            cMenu::UpdateButton(&m_ExitButton,   m_ExitButton  .IsFocused(), COLOR_MENU_RED);

            // 
            if(m_ExitButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("EXIT_GAME"));

            if(this->GetAlpha() >= 1.0f)
            {
                if(m_StartButton.IsClicked())
                {
                    // 
                    m_iStatus = 1;
                }
                else if(m_ConfigButton.IsClicked())
                {
                    // 
                    m_iStatus = 2;
                }
                else if(m_ExtraButton.IsClicked())
                {
                    // 
                    m_iStatus = 3;
                }
                else if(m_ExitButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_EXIT_GAME"), [](const coreInt32& iStatus)
                    {
                        if(iStatus == MSGBOX_STATUS_YES)
                            Core::System->Quit();
                    });
                }
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }
}