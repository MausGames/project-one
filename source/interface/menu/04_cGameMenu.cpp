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
: coreMenu(SURFACE_GAME_MAX, SURFACE_GAME_STANDARD)
{
    // create menu objects
    m_DirectoryBackground.DefineTexture(0u, "menu_background_black.png");
    m_DirectoryBackground.DefineProgram("menu_border_program");
    m_DirectoryBackground.SetPosition  (coreVector2(0.0f,0.0f));
    m_DirectoryBackground.SetSize      (coreVector2(0.8f,0.7f));

    m_ArmoryBackground.DefineTexture(0u, "menu_background_black.png");
    m_ArmoryBackground.DefineProgram("menu_border_program");
    m_ArmoryBackground.SetPosition  (coreVector2(0.0f,0.0f));
    m_ArmoryBackground.SetSize      (coreVector2(0.8f,0.4f));

    m_StandardTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StandardTab.DefineProgram("menu_border_program");
    m_StandardTab.SetPosition  (m_DirectoryBackground.GetPosition() + m_DirectoryBackground.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.21f,-0.0115f));
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
    m_BackButton.SetPosition  (m_DirectoryBackground.GetPosition() + m_DirectoryBackground.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    m_DirectoryHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_DirectoryHeader.SetPosition    (m_DirectoryBackground.GetPosition() + m_DirectoryBackground.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_DirectoryHeader.SetColor3      (COLOR_MENU_WHITE);
    m_DirectoryHeader.SetTextLanguage("GAME_HEADER_DIRECTORY");

    m_ArmoryHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArmoryHeader.SetPosition    (m_ArmoryBackground.GetPosition() + m_ArmoryBackground.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_ArmoryHeader.SetColor3      (COLOR_MENU_WHITE);
    m_ArmoryHeader.SetTextLanguage("GAME_HEADER_ARMORY");

    m_WorldMap.SetPosition(m_DirectoryBackground.GetPosition() + coreVector2(0.0f,-0.03f));
    m_WorldMap.SetSize    (m_DirectoryBackground.GetSize() + coreVector2(-0.035f,-0.095f));
    m_WorldMap.Arrange();

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        m_aMissionName[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aMissionName[i].SetPosition (m_DirectoryBackground.GetPosition() + m_DirectoryBackground.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.1f - 0.05f*I_TO_F(i)));
        m_aMissionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aMissionName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aMissionLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aMissionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aMissionLine[i].DefineProgram("menu_inner_program");
        m_aMissionLine[i].SetPosition  (coreVector2(0.0f, m_aMissionName[i].GetPosition().y));
        m_aMissionLine[i].SetSize      (coreVector2(m_DirectoryBackground.GetSize().x, 0.05f));
        m_aMissionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }
    m_aMissionName[0].SetText(cIntroMission  ::Name); //m_aMissionName[0].SetColor3(cCloudBackground  ::Color);
    m_aMissionName[1].SetText(cViridoMission ::Name); //m_aMissionName[1].SetColor3(cGrassBackground  ::Color);
    m_aMissionName[2].SetText(cNevoMission   ::Name); //m_aMissionName[2].SetColor3(cSeaBackground    ::Color);
    m_aMissionName[3].SetText(cHarenaMission ::Name); //m_aMissionName[3].SetColor3(cDesertBackground ::Color);
    m_aMissionName[4].SetText(cRutilusMission::Name); //m_aMissionName[4].SetColor3(cSpaceBackground  ::Color);
    m_aMissionName[5].SetText(cGeluMission   ::Name); //m_aMissionName[5].SetColor3(cVolcanoBackground::Color);
    m_aMissionName[6].SetText(cCalorMission  ::Name); //m_aMissionName[6].SetColor3(cSnowBackground   ::Color);
    m_aMissionName[7].SetText(cMuscusMission ::Name); //m_aMissionName[7].SetColor3(cMossBackground   ::Color);
    m_aMissionName[8].SetText(cAterMission   ::Name); //m_aMissionName[8].SetColor3(cDarkBackground   ::Color);   vielleicht nur bei selection in der zeile

    m_StageArea.DefineTexture(0u, "default_white.png");
    m_StageArea.DefineProgram("default_2d_program");
    m_StageArea.SetPosition  (m_DirectoryBackground.GetPosition() + m_DirectoryBackground.GetSize()*coreVector2(0.25f,0.0f) + coreVector2(-0.035f * 0.25f, -0.0275f));
    m_StageArea.SetSize      (m_DirectoryBackground.GetSize()*coreVector2(0.5f,1.0f) + coreVector2(-0.035f * 0.5f, -0.09f));
    m_StageArea.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.7f);


    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_STAGES; ++j)
        {
            const coreBool    bBoss = (j == MENU_GAME_STAGES-1u);//MISSION_SEGMENT_IS_BOSS(j);
            const coreBool    bSpecial = (i == 0u || i == 8u);
            const coreVector2 vPos  = coreVector2(bSpecial ? 0.0f : ((I_TO_F(j) - 3.5f) * 0.05f + (bBoss ? 0.025f : 0.0f)), m_aMissionName[i].GetPosition().y);//bBoss ? coreVector2(0.0f, -0.07f * (I_TO_F(j / 6u) * 2.0f - 1.5f)) : coreVector2(0.07f * (I_TO_F(j % 6u) - 2.0f), -0.07f * (I_TO_F(j / 6u) * 2.0f - 2.5f));
            const coreVector2 vSize = bSpecial ? coreVector2(0.4f,0.05f) : (bBoss ? coreVector2(0.1f,0.05f)                                   : coreVector2(0.05f,0.05f));

            m_aaStage[i][j].Construct    (MENU_BUTTON, MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
            m_aaStage[i][j].DefineProgram("menu_border_program");
            m_aaStage[i][j].SetPosition  (vPos);// + m_StageArea.GetPosition());
            m_aaStage[i][j].SetSize      (vSize);
        }
    }
    
    m_aStageCursor.DefineTexture(0u, "default_black.png");
    m_aStageCursor.DefineProgram("menu_border_program");
    m_aStageCursor.SetPosition  (HIDDEN_POS);// + m_StageArea.GetPosition());
    m_aStageCursor.SetSize      (coreVector2(0.07f,0.07f));
    /*
    for(coreUintW i = 0u; i < MENU_GAME_STAGES; ++i)
    {
        const coreBool    bBoss = MISSION_SEGMENT_IS_BOSS(i);
        const coreVector2 vPos  = bBoss ? coreVector2(0.0f, -0.07f * (I_TO_F(i / 6u) * 2.0f - 1.5f)) : coreVector2(0.07f * (I_TO_F(i % 6u) - 2.0f), -0.07f * (I_TO_F(i / 6u) * 2.0f - 2.5f));
        const coreVector2 vSize = bBoss ? coreVector2(0.35f,0.07f)                                   : coreVector2(0.07f,0.07f);

        m_aStage[i].Construct    (MENU_BUTTON, MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aStage[i].DefineProgram("menu_border_program");
        m_aStage[i].SetPosition  (vPos + m_StageArea.GetPosition());
        m_aStage[i].SetSize      (vSize);
    }
    */

    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i)
    {
        m_aOptionName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aOptionName[i].SetPosition (m_ArmoryBackground.GetPosition() + m_ArmoryBackground.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.1f - 0.05f*I_TO_F(iOffset)));
        m_aOptionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aOptionName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aOptionLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aOptionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aOptionLine[i].DefineProgram("menu_inner_program");
        m_aOptionLine[i].SetPosition  (coreVector2(0.0f, m_aOptionName[i].GetPosition().y));
        m_aOptionLine[i].SetSize      (coreVector2(m_ArmoryBackground.GetSize().x, 0.05f));
        m_aOptionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));

        iOffset += (i >= 1u) ? 2u : 1u;
    }
    m_aOptionName[0].SetTextLanguage("GAME_PLAYERS");
    m_aOptionName[1].SetTextLanguage("GAME_DIFFICULTY");
    m_aOptionName[2].SetTextLanguage("GAME_WEAPON");
    m_aOptionName[3].SetTextLanguage("GAME_SUPPORT");

    m_Players.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Players.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[0].GetPosition());
    //m_Players.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[0].GetPosition() + coreVector2(-0.125f,0.0f));
    m_Players.SetSize     (coreVector2( 0.47f,0.03f));
    //m_Players.SetSize     (coreVector2( 0.22f,0.03f));
    m_Players.SetAlignment(coreVector2(-1.00f,0.00f));
    m_Players.SetEndless  (true);
    m_Players.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_Difficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Difficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[1].GetPosition());
    m_Difficulty.SetSize     (m_Players.GetSize());
    m_Difficulty.SetAlignment(m_Players.GetAlignment());
    m_Difficulty.SetEndless  (true);
    m_Difficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aWeapon[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[2].GetPosition() - vOffset);
        m_aWeapon[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aWeapon[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aWeapon[i].SetEndless  (true);
        m_aWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aSupport[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aSupport[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[3].GetPosition() - vOffset);
        m_aSupport[i].SetSize     (m_aWeapon[i].GetSize());
        m_aSupport[i].SetAlignment(m_aWeapon[i].GetAlignment());
        m_aSupport[i].SetEndless  (true);
        m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aWeaponIcon[i].DefineTexture(0u, "menu_weapon.png");
        m_aWeaponIcon[i].DefineProgram("default_2d_program");
        m_aWeaponIcon[i].SetPosition  (m_aWeapon[i].GetPosition() + m_aWeapon[i].GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.0f,0.05f));
        m_aWeaponIcon[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aWeaponIcon[i].SetTexSize   (coreVector2(0.25f,0.5f));

        m_aSupportIcon[i].DefineTexture(0u, "menu_weapon.png");
        m_aSupportIcon[i].DefineProgram("default_2d_program");
        m_aSupportIcon[i].SetPosition  (m_aSupport[i].GetPosition() + m_aSupport[i].GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.0f,0.05f));
        m_aSupportIcon[i].SetSize      (m_aWeaponIcon[i].GetSize());
        m_aSupportIcon[i].SetTexSize   (m_aWeaponIcon[i].GetTexSize());
    }


    this->BindObject(SURFACE_GAME_ARMORY, &m_MenuInput);


    // fill option entries
    m_Players   .AddEntryLanguage("GAME_PLAYERS_SOLO",      1u);
    m_Players   .AddEntryLanguage("GAME_PLAYERS_COOP",      2u);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   0u);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", 1u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aWeapon [i].AddEntryLanguage("GAME_WEAPON_NOTHING",  cNoWeapon   ::ID);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aWeapon [i].AddEntryLanguage("GAME_WEAPON_RAY",      cRayWeapon  ::ID);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aWeapon [i].AddEntryLanguage("GAME_WEAPON_PULSE",    cPulseWeapon::ID);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aWeapon [i].AddEntryLanguage("GAME_WEAPON_WAVE",     cWaveWeapon ::ID);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aWeapon [i].AddEntryLanguage("GAME_WEAPON_TESLA",    cTeslaWeapon::ID);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aWeapon [i].AddEntryLanguage("GAME_WEAPON_ANTI",     cAntiWeapon ::ID);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aSupport[i].AddEntryLanguage("GAME_SUPPORT_NOTHING", 0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aSupport[i].AddEntryLanguage("GAME_SUPPORT_SHIELD",  1u);

    // bind menu objects
    for(coreUintW i = 0u; i < 2u; ++i)
    {
        if(i != SURFACE_GAME_STANDARD) this->BindObject(i, &m_StandardTab);
        if(i != SURFACE_GAME_TRAINING) this->BindObject(i, &m_TrainingTab);

        this->BindObject(i, &m_DirectoryBackground);

        if(i == SURFACE_GAME_STANDARD) this->BindObject(i, &m_StandardTab);
        if(i == SURFACE_GAME_TRAINING) this->BindObject(i, &m_TrainingTab);

        this->BindObject(i, &m_BackButton);
        this->BindObject(i, &m_DirectoryHeader);
    }

    this->BindObject(SURFACE_GAME_STANDARD, &m_WorldMap);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aMissionLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aMissionName[i]);

    //this->BindObject(SURFACE_GAME_TRAINING, &m_StageArea);

    //for(coreUintW i = 0u; i < MENU_GAME_STAGES; ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aStage[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_STAGES; ++j) this->BindObject(SURFACE_GAME_TRAINING, &m_aaStage[i][j]);
    this->BindObject(SURFACE_GAME_TRAINING, &m_aStageCursor);

    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryBackground);
    this->BindObject(SURFACE_GAME_ARMORY, &m_StartButton);
    this->BindObject(SURFACE_GAME_ARMORY, &m_CancelButton);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryHeader);

    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aOptionLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aOptionName[i]);

    this->BindObject(SURFACE_GAME_ARMORY, &m_Players);
    this->BindObject(SURFACE_GAME_ARMORY, &m_Difficulty);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeapon     [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupport    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeaponIcon [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupportIcon[i]);

    // 
    this->DeactivateFirstPlay();


    
    //m_TrainingTab.SetOverride(-1);   // TODO 1 
    
    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) m_MenuInput.BindObject(&m_aOptionLine[i]);
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
                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, true, false);
            }
            else if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }
        }
        break;

    case SURFACE_GAME_TRAINING:
        {
            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }
            
            m_aStageCursor.SetPosition(HIDDEN_POS);
            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                for(coreUintW j = 0u; j < MENU_GAME_STAGES; ++j)
                {
                    if(m_aaStage[i][j].IsFocused())
                    {
                        m_aStageCursor.SetPosition(m_aaStage[i][j].GetPosition());
                        m_aStageCursor.SetSize    (m_aaStage[i][j].GetSize    () + coreVector2(0.02f,0.02f));
                        break;
                    }
                    //m_aaStage[i][j].SetSize(m_aaStage[i][j].IsFocused() ? coreVector2(0.07f,0.07f) : coreVector2(0.05f,0.05f));
                    //m_aaStage[i][j].Move();           
                }
            }
            m_aStageCursor.Move();
            
        }
        break;

    case SURFACE_GAME_ARMORY:
        {
            if(m_StartButton.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 1;
            }
            else if(m_CancelButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                if(m_aWeapon[0].GetOverride() >= 0)
                {
                    // 
                    g_pMenu->ShiftSurface(this, this->GetOldSurface(), 3.0f, false, true);
                }
                else
                {
                    // 
                    m_iStatus = 2;
                }
            }

            // 
            cMenu::UpdateSwitchBox(&m_Players);
            cMenu::UpdateSwitchBox(&m_Difficulty);

            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                // 
                cMenu::UpdateSwitchBox(&m_aWeapon [i]);
                cMenu::UpdateSwitchBox(&m_aSupport[i]);

                // 
                     if(m_aWeapon [i].GetCurEntry().tValue == cNoWeapon   ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_WHITE);
                else if(m_aWeapon [i].GetCurEntry().tValue == cRayWeapon  ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_YELLOW);
                else if(m_aWeapon [i].GetCurEntry().tValue == cPulseWeapon::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_PURPLE);
                else if(m_aWeapon [i].GetCurEntry().tValue == cWaveWeapon ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_GREEN);
                else if(m_aWeapon [i].GetCurEntry().tValue == cTeslaWeapon::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_BLUE);
                else if(m_aWeapon [i].GetCurEntry().tValue == cAntiWeapon ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_RED);
                     if(m_aSupport[i].GetCurEntry().tValue == 0u)               m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);
                else if(m_aSupport[i].GetCurEntry().tValue == 1u)               m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_BLUE);

                // 
                     if(m_aWeapon [i].GetCurEntry().tValue == cNoWeapon   ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.25f,0.5f));
                else if(m_aWeapon [i].GetCurEntry().tValue == cRayWeapon  ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.0f, 0.0f));
                else if(m_aWeapon [i].GetCurEntry().tValue == cPulseWeapon::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.25f,0.0f));
                else if(m_aWeapon [i].GetCurEntry().tValue == cWaveWeapon ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.5f, 0.0f));
                else if(m_aWeapon [i].GetCurEntry().tValue == cTeslaWeapon::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.75f,0.0f));
                else if(m_aWeapon [i].GetCurEntry().tValue == cAntiWeapon ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.0f, 0.5f));
                     if(m_aSupport[i].GetCurEntry().tValue == 0u)               m_aSupportIcon[i].SetTexOffset(coreVector2(0.25f,0.5f));
                else if(m_aSupport[i].GetCurEntry().tValue == 1u)               m_aSupportIcon[i].SetTexOffset(coreVector2(0.5f, 0.5f));

                // 
                m_aWeaponIcon [i].SetColor3(m_aWeapon [i].GetCaption()->GetColor3());
                m_aSupportIcon[i].SetColor3(m_aSupport[i].GetCaption()->GetColor3());
            }


            const coreBool bCoop = (m_Players.GetCurEntry().tValue > 1);
            m_aWeapon [1].SetOverride(bCoop ? 0 : -1);
            m_aSupport[1].SetOverride(bCoop ? 0 : -1);

            if(false)
            {
                const coreVector2 vOffset = coreVector2(bCoop ? 0.25f : 0.125f, 0.0f);

                m_aWeapon     [0].SetPosition(coreVector2(-1.00f,1.00f) * m_aOptionName[1].GetPosition() - vOffset);
                m_aSupport    [0].SetPosition(coreVector2(-1.00f,1.00f) * m_aOptionName[2].GetPosition() - vOffset);
                m_aWeaponIcon [0].SetPosition(m_aWeapon [0].GetPosition() + m_aWeapon [0].GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.0f,0.05f));
                m_aSupportIcon[0].SetPosition(m_aSupport[0].GetPosition() + m_aSupport[0].GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.0f,0.05f));
            }

            {
                m_aWeapon     [1].SetEnabled(bCoop ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aSupport    [1].SetEnabled(bCoop ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aWeaponIcon [1].SetEnabled(bCoop ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aSupportIcon[1].SetEnabled(bCoop ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
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

    // 
    if(m_CancelButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_CancelButton), TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
    if(m_BackButton  .IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_BackButton),   TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}


// ****************************************************************
// 
void cGameMenu::ActivateFirstPlay()
{
    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        m_aWeapon [i].SetOverride(-1);
        m_aSupport[i].SetOverride(-1);
    }
}


// ****************************************************************
// 
void cGameMenu::DeactivateFirstPlay()
{
    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        m_aWeapon [i].SetOverride(0);
        m_aSupport[i].SetOverride(0);
    }
}


// ****************************************************************
// 
void cGameMenu::LoadValues()
{
    // 
    for(coreUintW i = 0u; i < WORLDMAP_PINS; ++i)
    {
        const coreBool bEnable = (g_pSave->GetHeader().oProgress.aiAdvance[i] != 0u);
        m_WorldMap.EnablePin(i, bEnable, bEnable || (i == WORLDMAP_PINS - 1u));
    }

    // 
    m_WorldMap.SelectPin(g_pSave->GetHeader().oOptions.iStandard);

    // 
    m_Players.SelectValue(g_pSave->GetHeader().oOptions.iPlayers);
    m_Players.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        m_aWeapon [i].SelectValue(g_pSave->GetHeader().oOptions.aaiWeapon [i][0]);
        m_aSupport[i].SelectValue(g_pSave->GetHeader().oOptions.aaiSupport[i][0]);
    }
}


// ****************************************************************
// 
void cGameMenu::SaveValues()
{
    // 
    g_pSave->EditOptions()->iStandard = m_WorldMap.GetSelectionIndex();

    // 
    g_pSave->EditOptions()->iPlayers    = m_Players   .GetCurEntry().tValue;
    g_pSave->EditOptions()->iDifficulty = m_Difficulty.GetCurEntry().tValue;

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        g_pSave->EditOptions()->aaiWeapon [i][0] = m_aWeapon [i].GetCurEntry().tValue;
        g_pSave->EditOptions()->aaiSupport[i][0] = m_aSupport[i].GetCurEntry().tValue;
    }
}