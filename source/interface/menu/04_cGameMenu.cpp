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
cGameMenu::cGameMenu()noexcept
: coreMenu (SURFACE_GAME_MAX, SURFACE_GAME_STANDARD)
{
    // create menu objects
    m_DirectoryBackground.DefineTexture(0u, "menu_background_black.png");
    m_DirectoryBackground.DefineProgram("menu_border_program");
    m_DirectoryBackground.SetPosition  (coreVector2(0.0f,0.0f));
    m_DirectoryBackground.SetSize      (coreVector2(0.8f,0.7f));

    m_ArmoryBackground.DefineTexture(0u, "menu_background_black.png");
    m_ArmoryBackground.DefineProgram("menu_border_program");
    m_ArmoryBackground.SetPosition  (coreVector2(0.0f,0.0f));
    m_ArmoryBackground.SetSize      (coreVector2(0.8f,0.45f));

    m_StandardTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StandardTab.DefineProgram("menu_border_program");
    m_StandardTab.SetPosition  (m_DirectoryBackground.GetPosition() + m_DirectoryBackground.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.21f,-0.0115f));
    m_StandardTab.SetSize      (coreVector2(0.36f,0.07f));
    m_StandardTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_StandardTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_StandardTab.SetTexOffset (m_StandardTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.21f,0.0115f));
    m_StandardTab.GetCaption()->SetTextLanguage("GAME_TAB_STANDARD");

    m_TrainingTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_TrainingTab.DefineProgram("menu_border_program");
    m_TrainingTab.SetPosition  (m_StandardTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_TrainingTab.SetSize      (m_StandardTab.GetSize());
    m_TrainingTab.SetAlignment (m_StandardTab.GetAlignment());
    m_TrainingTab.SetTexSize   (m_StandardTab.GetTexSize());
    m_TrainingTab.SetTexOffset (m_StandardTab.GetTexOffset() + coreVector2(m_TrainingTab.GetPosition().x - m_StandardTab.GetPosition().x, 0.0));
    m_TrainingTab.GetCaption()->SetTextLanguage("GAME_TAB_TRAINING");

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
    m_aMissionName[0].SetText(cIntroMission  ::Name);
    m_aMissionName[1].SetText(cViridoMission ::Name);
    m_aMissionName[2].SetText(cNevoMission   ::Name);
    m_aMissionName[3].SetText(cHarenaMission ::Name);
    m_aMissionName[4].SetText(cRutilusMission::Name);
    m_aMissionName[5].SetText(cGeluMission   ::Name);
    m_aMissionName[6].SetText(cCalorMission  ::Name);
    m_aMissionName[7].SetText(cMuscusMission ::Name);
    m_aMissionName[8].SetText(cAterMission   ::Name);

    m_StageArea.DefineTexture(0u, "default_white.png");
    m_StageArea.DefineProgram("default_2d_program");
    m_StageArea.SetPosition  (m_DirectoryBackground.GetPosition() + m_DirectoryBackground.GetSize()*coreVector2(0.25f,0.0f) + coreVector2(-0.035f * 0.25f, -0.0275f));
    m_StageArea.SetSize      (m_DirectoryBackground.GetSize()*coreVector2(0.5f,1.0f) + coreVector2(-0.035f * 0.5f, -0.09f));
    m_StageArea.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.7f);

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

        iOffset += (i >= 2u) ? 2u : 1u;
    }
    m_aOptionName[0].SetTextLanguage("GAME_TYPE");
    m_aOptionName[1].SetTextLanguage("GAME_MODE");
    m_aOptionName[2].SetTextLanguage("GAME_DIFFICULTY");
    m_aOptionName[3].SetTextLanguage("GAME_WEAPON");
    m_aOptionName[4].SetTextLanguage("GAME_SUPPORT");

    m_Type.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Type.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[0].GetPosition());
    m_Type.SetSize     (coreVector2( 0.47f,0.03f));
    m_Type.SetAlignment(coreVector2(-1.00f,0.00f));
    m_Type.SetEndless  (true);
    m_Type.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_Mode.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Mode.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[1].GetPosition());
    m_Mode.SetSize     (m_Type.GetSize());
    m_Mode.SetAlignment(m_Type.GetAlignment());
    m_Mode.SetEndless  (true);
    m_Mode.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_Difficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Difficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[2].GetPosition());
    m_Difficulty.SetSize     (m_Type.GetSize());
    m_Difficulty.SetAlignment(m_Type.GetAlignment());
    m_Difficulty.SetEndless  (true);
    m_Difficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aWeapon[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[3].GetPosition() - vOffset);
        m_aWeapon[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aWeapon[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aWeapon[i].SetEndless  (true);
        m_aWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aSupport[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aSupport[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[4].GetPosition() - vOffset);
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

    // fill option entries
    m_Type      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_Type      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_Type      .AddEntryLanguage("GAME_TYPE_DUEL",         GAME_TYPE_DUEL);
    m_Mode      .AddEntryLanguage("GAME_MODE_STANDARD",     GAME_MODE_STANDARD);
    m_Mode      .AddEntryLanguage("GAME_MODE_PACIFIST",     GAME_MODE_PACIFIST);
    m_Mode      .AddEntryLanguage("GAME_MODE_MASOCHIST",    GAME_MODE_MASOCHIST);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    m_Difficulty.AddEntryLanguage("GAME_DIFFICULTY_HARD",   GAME_DIFFICULTY_HARD);
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

    this->BindObject(SURFACE_GAME_TRAINING, &m_StageArea);

    for(coreUintW i = 0u; i < MENU_GAME_STAGES; ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aStage[i]);

    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryBackground);
    this->BindObject(SURFACE_GAME_ARMORY, &m_StartButton);
    this->BindObject(SURFACE_GAME_ARMORY, &m_CancelButton);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryHeader);

    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aOptionLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aOptionName[i]);

    this->BindObject(SURFACE_GAME_ARMORY, &m_Type);
    this->BindObject(SURFACE_GAME_ARMORY, &m_Mode);
    this->BindObject(SURFACE_GAME_ARMORY, &m_Difficulty);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeapon     [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupport    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeaponIcon [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupportIcon[i]);

    // 
    this->DeactivateFirstPlay();
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
                g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f);
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
                    g_pMenu->ShiftSurface(this, this->GetOldSurface(), 3.0f);
                }
                else
                {
                    // 
                    m_iStatus = 2;
                }
            }

            // 
            cMenu::UpdateSwitchBox(&m_Type);
            cMenu::UpdateSwitchBox(&m_Mode);
            cMenu::UpdateSwitchBox(&m_Difficulty);

            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                // 
                cMenu::UpdateSwitchBox(&m_aWeapon [i]);
                cMenu::UpdateSwitchBox(&m_aSupport[i]);

                // 
                     if(m_aWeapon [i].GetCurValue() == cNoWeapon   ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_WHITE);
                else if(m_aWeapon [i].GetCurValue() == cRayWeapon  ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_YELLOW);
                else if(m_aWeapon [i].GetCurValue() == cPulseWeapon::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_PURPLE);
                else if(m_aWeapon [i].GetCurValue() == cWaveWeapon ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_GREEN);
                else if(m_aWeapon [i].GetCurValue() == cTeslaWeapon::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_BLUE);
                else if(m_aWeapon [i].GetCurValue() == cAntiWeapon ::ID) m_aWeapon [i].GetCaption()->SetColor3(COLOR_MENU_RED);
                     if(m_aSupport[i].GetCurValue() == 0u)               m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);
                else if(m_aSupport[i].GetCurValue() == 1u)               m_aSupport[i].GetCaption()->SetColor3(COLOR_MENU_BLUE);

                // 
                     if(m_aWeapon [i].GetCurValue() == cNoWeapon   ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.25f,0.5f));
                else if(m_aWeapon [i].GetCurValue() == cRayWeapon  ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.0f, 0.0f));
                else if(m_aWeapon [i].GetCurValue() == cPulseWeapon::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.25f,0.0f));
                else if(m_aWeapon [i].GetCurValue() == cWaveWeapon ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.5f, 0.0f));
                else if(m_aWeapon [i].GetCurValue() == cTeslaWeapon::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.75f,0.0f));
                else if(m_aWeapon [i].GetCurValue() == cAntiWeapon ::ID) m_aWeaponIcon [i].SetTexOffset(coreVector2(0.0f, 0.5f));
                     if(m_aSupport[i].GetCurValue() == 0u)               m_aSupportIcon[i].SetTexOffset(coreVector2(0.25f,0.5f));
                else if(m_aSupport[i].GetCurValue() == 1u)               m_aSupportIcon[i].SetTexOffset(coreVector2(0.5f, 0.5f));

                // 
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
    m_Type      .SelectValue(g_pSave->GetHeader().oOptions.iType);
    m_Mode      .SelectValue(g_pSave->GetHeader().oOptions.iMode);
    m_Difficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);

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
    g_pSave->EditOptions()->iType       = m_Type      .GetCurValue();
    g_pSave->EditOptions()->iMode       = m_Mode      .GetCurValue();
    g_pSave->EditOptions()->iDifficulty = m_Difficulty.GetCurValue();

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        g_pSave->EditOptions()->aaiWeapon [i][0] = m_aWeapon [i].GetCurValue();
        g_pSave->EditOptions()->aaiSupport[i][0] = m_aSupport[i].GetCurValue();
    }
}