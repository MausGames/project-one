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
cGameMenu::cGameMenu()noexcept
: coreMenu(3u, SURFACE_GAME_STANDARD)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetSize      (coreVector2(0.8f,0.55f));



    m_ArmoryBackground.DefineTexture(0u, "menu_background_black.png");
    m_ArmoryBackground.DefineProgram("menu_border_program");
    m_ArmoryBackground.SetPosition  (coreVector2(0.0f,0.0f));
    m_ArmoryBackground.SetSize      (coreVector2(0.8f,0.45f));


    m_StandardTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StandardTab.DefineProgram("menu_border_program");
    m_StandardTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.21f,-0.0115f));
    m_StandardTab.SetSize      (coreVector2(0.36f,0.07f));
    m_StandardTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_StandardTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_StandardTab.SetTexOffset (m_StandardTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.21f,0.0115f));
    m_StandardTab.GetCaption()->SetTextLanguage("GAME_MODE_STANDARD");

    m_TrainingTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_TrainingTab.DefineProgram("menu_border_program");
    m_TrainingTab.SetPosition  (m_StandardTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_TrainingTab.SetSize      (m_StandardTab.GetSize());
    m_TrainingTab.SetAlignment (m_StandardTab.GetAlignment());
    m_TrainingTab.SetTexSize   (m_StandardTab.GetTexSize());
    m_TrainingTab.SetTexOffset (m_StandardTab.GetTexOffset() + coreVector2(m_TrainingTab.GetPosition().x - m_StandardTab.GetPosition().x, 0.0));
    m_TrainingTab.GetCaption()->SetTextLanguage("GAME_MODE_TRAINING");

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (m_ArmoryBackground.GetPosition() + m_ArmoryBackground.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButton.GetCaption()->SetTextLanguage("START");

    m_CancelButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_CancelButton.DefineProgram("menu_border_program");
    m_CancelButton.SetPosition  (m_ArmoryBackground.GetPosition() + m_ArmoryBackground.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_CancelButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_CancelButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_CancelButton.GetCaption()->SetText(ICON_SHARE);

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    

    m_MissionHeader.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_MissionHeader.SetPosition(m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_MissionHeader.SetColor3  (COLOR_MENU_WHITE);
    m_MissionHeader.SetText    ("Select Stage");
    
    m_ArmoryHeader.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArmoryHeader.SetPosition(m_ArmoryBackground.GetPosition() + m_ArmoryBackground.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_ArmoryHeader.SetColor3  (COLOR_MENU_WHITE);
    m_ArmoryHeader.SetText    ("Select Equipment");
    


    m_WorldMap.SetPosition(m_Background.GetPosition() - coreVector2(0.0f,0.03f));
    m_WorldMap.SetSize    (m_Background.GetSize() - coreVector2(0.035f,0.095f));
    m_WorldMap.Arrange();
    
    m_StageArea.DefineTexture(0u, "default_white.png");
    m_StageArea.DefineProgram("default_2d_program");
    m_StageArea.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.25f,0.0f) + coreVector2(-0.55f * 0.015f, -0.0275f));
    m_StageArea.SetSize      (m_Background.GetSize() * coreVector2(0.5f,1.0f) - coreVector2(0.035f * 0.5f,0.09f));
    m_StageArea.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.7f);   
    



    // 
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        m_aMissionName[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aMissionName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.1f - 0.05f*I_TO_F(i)));
        m_aMissionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aMissionName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aMissionLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aMissionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aMissionLine[i].DefineProgram("menu_inner_program");
        m_aMissionLine[i].SetPosition  (coreVector2(0.0f, m_aMissionName[i].GetPosition().y));
        m_aMissionLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aMissionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }

      
    m_aMissionName[0].SetText(cIntroMission  ::Name);
    m_aMissionName[1].SetText(cViridoMission ::Name);
    m_aMissionName[2].SetText(cNevoMission   ::Name);
    m_aMissionName[3].SetText(cHarenaMission ::Name);
    m_aMissionName[4].SetText(cRutilusMission::Name);
    m_aMissionName[5].SetText(cGeluMission   ::Name);
    m_aMissionName[6].SetText(cCalorMission  ::Name);
    m_aMissionName[7].SetText(cMuscusMission ::Name);
    m_aMissionName[8].SetText(cAterMission   ::Name);


    for(coreUintW i = 0u; i < MENU_GAME_STAGES; ++i)
    {
        const coreBool bBoss = ((i + 1u) % 6u) ? false : true;
            
        //m_aStage[i].DefineTexture(0u, Core::Manager::Resource->Load<coreTexture>("skill_passive.png", CORE_RESOURCE_UPDATE_AUTO, "data/textures/skill_passive.png"));//"default_white.png");
        //m_aStage[i].DefineProgram("default_2d_program");
        m_aStage[i].Construct    (MENU_BUTTON, bBoss ? "ethnocentric.ttf" : "keifont.ttf", bBoss ? 20u : 30u, MENU_OUTLINE_SMALL);    
        m_aStage[i].DefineProgram("menu_border_program");
        m_aStage[i].SetPosition  (m_StageArea.GetPosition() + (bBoss ? coreVector2(0.0f, -0.07f * (I_TO_F(i / 6u)*2.0f - 1.5f)) : coreVector2(0.07f * (I_TO_F(i % 6u) - 2.0f), -0.07f * (I_TO_F(i / 6u)*2.0f - 2.5f))));
        m_aStage[i].SetSize      ((bBoss ? coreVector2(0.35f,0.07f) : coreVector2(0.07f,0.07f)));
        m_aStage[i].SetAlignment (coreVector2(0.0f,0.0f));   
        //m_aStage[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.7f);   
    }

    m_aStage[0] .GetCaption()->SetText(u8"回");
    m_aStage[1] .GetCaption()->SetText(u8"躱");
    m_aStage[2] .GetCaption()->SetText(u8"列");
    m_aStage[3] .GetCaption()->SetText(u8"蜂");
    m_aStage[4] .GetCaption()->SetText(u8"億");

    
    m_aStage[5] .GetCaption()->SetText(cDharukBoss::Name);
    m_aStage[11].GetCaption()->SetText(cTorusBoss ::Name);
    m_aStage[17].GetCaption()->SetText(cVausBoss  ::Name);


    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aOptionName); ++i)
    {
        m_aOptionName[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aOptionName[i].SetPosition (m_ArmoryBackground.GetPosition() + m_ArmoryBackground.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.1f - 0.05f*I_TO_F(i ? (i*2u + 1u) : i)));
        m_aOptionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aOptionName[i].SetColor3   (COLOR_MENU_WHITE);
    }

    m_aOptionName[0].SetTextLanguage("GAME_PLAYERS");
    m_aOptionName[1].SetText("Weapon");
    m_aOptionName[2].SetText("Support");

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aOptionLine); ++i)
    {
        m_aOptionLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aOptionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aOptionLine[i].DefineProgram("menu_inner_program");
        m_aOptionLine[i].SetPosition  (coreVector2(0.0f, m_aOptionName[i].GetPosition().y));
        m_aOptionLine[i].SetSize      (coreVector2(m_ArmoryBackground.GetSize().x, 0.05f));
        m_aOptionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }


    m_Players.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Players.SetPosition (coreVector2(-1.0f,1.0f) * m_aOptionName[0].GetPosition());
    m_Players.SetSize     (coreVector2(0.47f,0.03f));
    m_Players.SetAlignment(coreVector2(-1.0f,0.0f));
    m_Players.SetEndless  (true);
    m_Players.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    
    m_Players   .AddEntryLanguage("GAME_PLAYERS_SOLO",      1u);
    m_Players   .AddEntryLanguage("GAME_PLAYERS_COOP",      2u);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aWeapon[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aOptionName[1].GetPosition() - vOffset);
        m_aWeapon[i].SetSize     (coreVector2(0.22f,0.03f));
        m_aWeapon[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aWeapon[i].SetEndless  (true);
        m_aWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);  

        m_aSupport[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aSupport[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aOptionName[2].GetPosition() - vOffset);
        m_aSupport[i].SetSize     (m_aWeapon[i].GetSize());
        m_aSupport[i].SetAlignment(m_aWeapon[i].GetAlignment());
        m_aSupport[i].SetEndless  (true);
        m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);  


        m_aWeaponIcon[i].DefineTexture(0u, "menu_weapon.png");
        m_aWeaponIcon[i].DefineProgram("default_2d_program");
        m_aWeaponIcon[i].SetPosition (m_aWeapon[i].GetPosition() - m_aWeapon[i].GetSize()*coreVector2(0.5f,0.0f) + coreVector2(0.0f,-0.05f));
        m_aWeaponIcon[i].SetSize     (coreVector2(0.06f,0.06f));
        m_aWeaponIcon[i].SetTexSize  (coreVector2(0.25f,0.5f));


        m_aSupportIcon[i].DefineTexture(0u, "menu_weapon.png");
        m_aSupportIcon[i].DefineProgram("default_2d_program");
        m_aSupportIcon[i].SetPosition (m_aSupport[i].GetPosition() - m_aSupport[i].GetSize()*coreVector2(0.5f,0.0f) + coreVector2(0.0f,-0.05f));
        m_aSupportIcon[i].SetSize     (coreVector2(0.06f,0.06f));
        m_aSupportIcon[i].SetTexSize  (coreVector2(0.25f,0.5f));



        // 
        m_aWeapon [i].AddEntry("Nothing", 0u);
        m_aWeapon [i].AddEntry("Laser",   1u);
        m_aSupport[i].AddEntry("Nothing", 0u);
        m_aSupport[i].AddEntry("Shield",  1u);
    }


    /*
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
    m_Mode      .AddEntryLanguage("GAME_MODE_STANDARD",     0u);
    m_Mode      .AddEntryLanguage("GAME_MODE_TRAINING",     1u);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   0u);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", 1u);
    m_Players   .AddEntryLanguage("GAME_PLAYERS_SOLO",      1u);
    m_Players   .AddEntryLanguage("GAME_PLAYERS_COOP",      2u);
    */

    // bind menu objects
    for(coreUintW i = 0u, ie = 2u/*this->GetNumSurfaces()*/; i < ie; ++i)
    {
        if(i != SURFACE_GAME_STANDARD) this->BindObject(i, &m_StandardTab);
        if(i != SURFACE_GAME_TRAINING) this->BindObject(i, &m_TrainingTab);
         
        
        if(i == SURFACE_GAME_STANDARD) this->BindObject(i, &m_StandardTab);  
        if(i == SURFACE_GAME_TRAINING) this->BindObject(i, &m_TrainingTab); 

        this->BindObject(i, &m_Background);

        //if(i == SURFACE_GAME_STANDARD) this->BindObject(i, &m_StandardTab);
        //if(i == SURFACE_GAME_TRAINING) this->BindObject(i, &m_TrainingTab);

        //this->BindObject(i, &m_StartButton);
        this->BindObject(i, &m_BackButton);
    }

    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryBackground);
    this->BindObject(SURFACE_GAME_ARMORY, &m_StartButton);
    this->BindObject(SURFACE_GAME_ARMORY, &m_CancelButton);

    
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aOptionLine); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aOptionLine[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aOptionName); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aOptionName[i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeaponIcon [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupportIcon[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_Players);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeapon    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupport   [i]);

    /*
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aLine);  ++i) this->BindObject(SURFACE_GAME_DEFAULT, &m_aLine [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aLabel); ++i) this->BindObject(SURFACE_GAME_DEFAULT, &m_aLabel[i]);

    this->BindObject(SURFACE_GAME_DEFAULT, &m_Mode);
    this->BindObject(SURFACE_GAME_DEFAULT, &m_Difficulty);
    this->BindObject(SURFACE_GAME_DEFAULT, &m_Players);
    */

    this->BindObject(SURFACE_GAME_STANDARD, &m_MissionHeader); // TODO: order  
    this->BindObject(SURFACE_GAME_TRAINING, &m_MissionHeader); // TODO: order  
    this->BindObject(SURFACE_GAME_ARMORY,   &m_ArmoryHeader); // TODO: order  
    
    
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_STANDARD, &m_aLine[i]);
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_STANDARD, &m_aName[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aMissionLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aMissionName[i]);

    
    this->BindObject(SURFACE_GAME_TRAINING, &m_StageArea); // TODO: order  

    //for(coreUintW i = 1u; i < MENU_GAME_MISSIONS-1u; ++i) for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaParticle[0]); ++j) this->BindObject(SURFACE_GAME_STANDARD, &m_aaParticle[i][j]); // TODO: order  
    for(coreUintW i = 0u; i < MENU_GAME_STAGES; ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aStage[i]); // TODO: order  
    
    
    this->BindObject(SURFACE_GAME_STANDARD, &m_WorldMap); // TODO: order  
    
}


// ****************************************************************
// move the game menu
void cGameMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
         if(m_StandardTab.IsClicked()) this->ChangeSurface(SURFACE_GAME_STANDARD, 0.0f);
    else if(m_TrainingTab.IsClicked()) this->ChangeSurface(SURFACE_GAME_TRAINING, 0.0f);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_GAME_STANDARD:
        {
            if(m_WorldMap.GetSelectionState())
            {
                g_pMenu->AnimateSurface(this, SURFACE_GAME_ARMORY, 3.0f);
            }

            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 2;
            }
        }
        break;

    case SURFACE_GAME_TRAINING:
        {
            if(this->GetAlpha() >= 1.0f)
            {
                if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
                {
                    // 
                    m_iStatus = 2;
                }
            }
        }
        break;

    case SURFACE_GAME_ARMORY:
        {
            if(this->GetAlpha() >= 1.0f)
            {
                if(m_StartButton.IsClicked())
                {
                    // 
                    m_iStatus = 1;
                }
                else if(m_CancelButton.IsClicked() || g_MenuInput.bCancel)
                {
                    // 
                    g_pMenu->AnimateSurface(this, this->GetOldSurface(), 3.0f);
                }
            }

            cMenu::UpdateSwitchBox(&m_Players);

            // 
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                cMenu::UpdateSwitchBox(&m_aWeapon [i]);
                cMenu::UpdateSwitchBox(&m_aSupport[i]);


                     if(m_aWeapon [i].GetCurEntry().tValue == 0u) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_WHITE);
                else if(m_aWeapon [i].GetCurEntry().tValue == 1u) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_YELLOW);
                     if(m_aSupport[i].GetCurEntry().tValue == 0u) m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);
                else if(m_aSupport[i].GetCurEntry().tValue == 1u) m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_BLUE);

                     if(m_aWeapon [i].GetCurEntry().tValue == 0u) m_aWeaponIcon[i].SetTexOffset(coreVector2(0.25f,0.5f));
                else if(m_aWeapon [i].GetCurEntry().tValue == 1u) m_aWeaponIcon[i].SetTexOffset(coreVector2(0.0f, 0.0f));

                m_aWeaponIcon [i].SetColor3(m_aWeapon [i].GetCaption()->GetColor3());
                m_aSupportIcon[i].SetColor3(m_aSupport[i].GetCaption()->GetColor3());
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    cMenu::UpdateButton(&m_StandardTab, (this->GetCurSurface() == SURFACE_GAME_STANDARD) || m_StandardTab.IsFocused());
    cMenu::UpdateButton(&m_TrainingTab, (this->GetCurSurface() == SURFACE_GAME_TRAINING) || m_TrainingTab.IsFocused());

    // 
    cMenu::UpdateButton(&m_StartButton,  m_StartButton .IsFocused());
    cMenu::UpdateButton(&m_CancelButton, m_CancelButton.IsFocused());
    cMenu::UpdateButton(&m_BackButton,   m_BackButton  .IsFocused());

    /*
    // 
    cMenu::UpdateSwitchBox(&m_Mode);
    cMenu::UpdateSwitchBox(&m_Difficulty);
    cMenu::UpdateSwitchBox(&m_Players);
    */

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("BACK"));



    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        m_aMissionLine[i].SetAlpha(m_aMissionLine[i].GetAlpha() * ((m_aMissionName[i].IsFocused() || m_aMissionLine[i].IsFocused()) ? 2.0f : 0.9f));
    }

    for(coreUintW i = 0u; i < MENU_GAME_STAGES; ++i)
    {
        cMenu::UpdateButton(&m_aStage[i], m_aStage[i].IsFocused());
    }
}