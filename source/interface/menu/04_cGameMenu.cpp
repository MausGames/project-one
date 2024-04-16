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
: coreMenu           (SURFACE_GAME_MAX, SURFACE_GAME_STANDARD)
, m_aiStageSelection {}
, m_iCurPage         (0u)
, m_aiCurIndex       {coreUintW(-1), coreUintW(-1), coreUintW(-1)}
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.01f));
    m_Background.SetSize      (coreVector2(0.9f,0.75f));

    m_StandardTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StandardTab.DefineProgram("menu_border_program");
    m_StandardTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.235f,-0.0125f));
    m_StandardTab.SetSize      (coreVector2(0.41f,0.07f));
    m_StandardTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_StandardTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_StandardTab.SetTexOffset (m_StandardTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.235f,0.0125f));
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
    m_StartButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButton.GetCaption()->SetTextLanguage("START");

    m_OptionButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_OptionButton.DefineProgram("menu_border_program");
    m_OptionButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_OptionButton.SetSize      (coreVector2(0.3f,0.07f));
    m_OptionButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_OptionButton.GetCaption()->SetTextLanguage("OPTIONS");

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);      
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    m_OptionState.Construct    (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_OptionState.SetPosition  (m_OptionButton.GetPosition() + coreVector2(m_OptionButton.GetSize().x + 0.02f, -0.035f * 0.5f));
    m_OptionState.SetSize      (m_OptionButton.GetSize());
    m_OptionState.SetAlignment (m_OptionButton.GetAlignment());
    m_OptionState.SetColor3    (COLOR_MENU_WHITE);
    m_OptionState.SetText("Solo | Original | Normal");                                   

    m_StandardHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StandardHeader.SetPosition    (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(-0.13f,-0.045f));
    m_StandardHeader.SetColor3      (COLOR_MENU_WHITE);
    m_StandardHeader.SetTextLanguage("GAME_HEADER_STANDARD");

    m_TrainingHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_TrainingHeader.SetPosition    (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.13f,-0.045f));
    m_TrainingHeader.SetColor3      (COLOR_MENU_WHITE);
    m_TrainingHeader.SetTextLanguage("GAME_HEADER_TRAINING");

    m_ArmoryHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArmoryHeader.SetPosition    (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_ArmoryHeader.SetColor3      (COLOR_MENU_WHITE);
    m_ArmoryHeader.SetTextLanguage("GAME_HEADER_ARMORY");

    m_WorldMap.SetPosition(m_Background.GetPosition() + coreVector2(-0.135f,-0.025f));
    m_WorldMap.SetSize    (m_Background.GetSize() + coreVector2(-0.32f,-0.105f));
    m_WorldMap.Arrange();

    m_StandardArea.DefineTexture(0u, "menu_detail_04.png");
    m_StandardArea.DefineTexture(1u, "menu_background_black.png");
    m_StandardArea.DefineProgram("menu_inner_program");
    m_StandardArea.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(-0.15f,0.0f));
    m_StandardArea.SetSize      (coreVector2(m_Background.GetSize().y, 0.25f));
    m_StandardArea.SetDirection (coreVector2(1.0f,0.0f));

    m_StandardMedal.DefineTexture(0u, "menu_medal.png");
    m_StandardMedal.DefineProgram("default_2d_program");
    m_StandardMedal.SetPosition  (m_StandardArea.GetPosition() + coreVector2(0.0f,-0.22f));
    m_StandardMedal.SetSize      (coreVector2(0.1f,0.1f));
    m_StandardMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    m_StandardFragment.DefineTexture(0u, "menu_star.png");
    m_StandardFragment.DefineProgram("default_2d_program");
    m_StandardFragment.SetPosition  (m_StandardArea.GetPosition() + coreVector2(0.0f,0.0f));
    m_StandardFragment.SetTexSize   (coreVector2(0.25f,0.25f));
    m_StandardFragment.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);                    

    m_aStandardName[0].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aStandardName[0].SetPosition(m_StandardArea.GetPosition() + coreVector2(0.0f,0.25f));

    m_aStandardName[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aStandardName[1].SetPosition(m_StandardArea.GetPosition() + coreVector2(0.0f,0.29f));
    m_aStandardName[1].SetColor3  (COLOR_MENU_WHITE);

    m_aStandardScore[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aStandardScore[0].SetPosition    (m_StandardArea.GetPosition() + coreVector2(0.0f,0.1f));
    m_aStandardScore[0].SetTextLanguage("GAME_SCORE");

    m_aStandardScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aStandardScore[1].SetPosition(m_StandardArea.GetPosition() + coreVector2(0.0f,0.066f));
    m_aStandardScore[1].SetColor3  (COLOR_MENU_WHITE);

    m_aStandardTime[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aStandardTime[0].SetPosition    (m_StandardArea.GetPosition() + coreVector2(0.0f,-0.026f));
    m_aStandardTime[0].SetTextLanguage("GAME_TIME");

    m_aStandardTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aStandardTime[1].SetPosition(m_StandardArea.GetPosition() + coreVector2(0.0f,-0.06f));
    m_aStandardTime[1].SetColor3  (COLOR_MENU_WHITE);

    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        m_aMissionName[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        //m_aMissionName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.3f, -0.1f - 0.025f*I_TO_F(iOffset)));
        //m_aMissionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aMissionName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,0.5f) + coreVector2(-0.04f, -0.1f - 0.025f*I_TO_F(iOffset)));
        m_aMissionName[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aMissionName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aMissionLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aMissionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aMissionLine[i].DefineProgram("menu_inner_program");
        m_aMissionLine[i].SetPosition  (coreVector2(0.0f, m_aMissionName[i].GetPosition().y));
        m_aMissionLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aMissionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));

        iOffset += (i == 6u) ? 3u : 2u;
    }
    m_aMissionName[0].SetText(cViridoMission ::Name);
    m_aMissionName[1].SetText(cNevoMission   ::Name);
    m_aMissionName[2].SetText(cHarenaMission ::Name);
    m_aMissionName[3].SetText(cRutilusMission::Name);
    m_aMissionName[4].SetText(cGeluMission   ::Name);
    m_aMissionName[5].SetText(cCalorMission  ::Name);
    m_aMissionName[6].SetText(cMuscusMission ::Name);
    m_aMissionName[7].SetText(cBonus1Mission ::Name);
    m_aMissionName[8].SetText(cBonus2Mission ::Name);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_STAGES; ++j)
        {
            const coreBool    bBoss = MISSION_SEGMENT_IS_BOSS(j);
            const coreVector2 vPos  = coreVector2((I_TO_F(j) - 3.5f) * 0.05f + (bBoss ? 0.02f : 0.0f), m_aMissionName[i].GetPosition().y);
            const coreVector2 vSize = bBoss ? coreVector2(0.1f,0.05f) : coreVector2(0.05f,0.05f);

            m_aaStage[i][j].DefineTexture   (0u, Core::Manager::Resource->Load<coreTexture>("test.png", CORE_RESOURCE_UPDATE_AUTO, "data/textures/test.png"));
            m_aaStage[i][j].DefineProgram   ("menu_grey_program");
            //m_aaStage[i][j].SetPosition     (vPos + coreVector2(0.225f,0.0f));
            m_aaStage[i][j].SetPosition     (vPos + coreVector2(0.05f,0.0f));
            m_aaStage[i][j].SetSize         (vSize * 0.8f);
            m_aaStage[i][j].SetFocusable    (true);
            m_aaStage[i][j].SetFocusModifier(coreVector2(1.25f,1.25f));
        }
    }

    m_aStageCursor.DefineProgram("menu_grey_program");
    m_aStageCursor.SetSize      (coreVector2(0.07f,0.07f));

    m_TrainingArea.DefineTexture(0u, "menu_detail_04.png");
    m_TrainingArea.DefineTexture(1u, "menu_background_black.png");
    m_TrainingArea.DefineProgram("menu_inner_program");
    m_TrainingArea.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.15f,0.0f));
    m_TrainingArea.SetSize      (coreVector2(m_Background.GetSize().y, 0.25f));
    m_TrainingArea.SetDirection (coreVector2(1.0f,0.0f));

    m_TrainingMedal.DefineTexture(0u, "menu_medal.png");
    m_TrainingMedal.DefineProgram("default_2d_program");
    m_TrainingMedal.SetPosition  (m_TrainingArea.GetPosition() + coreVector2(0.0f,-0.22f));
    m_TrainingMedal.SetSize      (coreVector2(0.1f,0.1f));
    m_TrainingMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    m_TrainingFragment.DefineTexture(0u, "menu_star.png");
    m_TrainingFragment.DefineProgram("default_2d_program");
    m_TrainingFragment.SetPosition  (m_TrainingArea.GetPosition() + coreVector2(0.0f,0.0f));
    m_TrainingFragment.SetTexSize   (coreVector2(0.25f,0.25f));
    m_TrainingFragment.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);                    

    for(coreUintW i = 0u; i < BADGES; ++i)
    {
        m_aTrainingBadge[i].DefineTexture(0u, "menu_star.png");
        m_aTrainingBadge[i].DefineProgram("default_2d_program");
        m_aTrainingBadge[i].SetPosition  (m_TrainingMedal.GetPosition() + coreVector2(I_TO_F(i - 1u) * 0.05f, (i == 1u) ? -0.08f : -0.06f));
        m_aTrainingBadge[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aTrainingBadge[i].SetTexSize   (coreVector2(0.5f, 1.0f));
        m_aTrainingBadge[i].SetTexOffset (coreVector2(i ? 0.5f : 0.0f, 0.0f));
    }

    m_aTrainingName[0].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aTrainingName[0].SetPosition(m_TrainingArea.GetPosition() + coreVector2(0.0f,0.25f));
    m_aTrainingName[0].SetColor3  (COLOR_MENU_GREEN);
    m_aTrainingName[0].SetText("MUSCUS");         

    m_aTrainingName[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aTrainingName[1].SetPosition(m_TrainingArea.GetPosition() + coreVector2(0.0f,0.29f));
    m_aTrainingName[1].SetColor3  (COLOR_MENU_WHITE);
    m_aTrainingName[1].SetText("Stage 1");         

    m_aTrainingScore[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aTrainingScore[0].SetPosition    (m_TrainingArea.GetPosition() + coreVector2(0.0f,0.1f));
    m_aTrainingScore[0].SetColor3      (COLOR_MENU_GREEN);
    m_aTrainingScore[0].SetTextLanguage("GAME_SCORE");

    m_aTrainingScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aTrainingScore[1].SetPosition(m_TrainingArea.GetPosition() + coreVector2(0.0f,0.066f));
    m_aTrainingScore[1].SetColor3  (COLOR_MENU_WHITE);
    m_aTrainingScore[1].SetText("123456");         

    m_aTrainingTime[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aTrainingTime[0].SetPosition    (m_TrainingArea.GetPosition() + coreVector2(0.0f,-0.026f));
    m_aTrainingTime[0].SetColor3      (COLOR_MENU_GREEN);
    m_aTrainingTime[0].SetTextLanguage("GAME_TIME");

    m_aTrainingTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aTrainingTime[1].SetPosition(m_TrainingArea.GetPosition() + coreVector2(0.0f,-0.06f));
    m_aTrainingTime[1].SetColor3  (COLOR_MENU_WHITE);
    m_aTrainingTime[1].SetText("45.6");         

    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i)
    {
        m_aOptionName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aOptionName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.1f - 0.05f*I_TO_F(i)));
        m_aOptionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aOptionName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aOptionLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aOptionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aOptionLine[i].DefineProgram("menu_inner_program");
        m_aOptionLine[i].SetPosition  (coreVector2(0.0f, m_aOptionName[i].GetPosition().y));
        m_aOptionLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aOptionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }
    m_aOptionName[0].SetTextLanguage("GAME_TYPE");
    m_aOptionName[1].SetTextLanguage("GAME_MODE");
    m_aOptionName[2].SetTextLanguage("GAME_DIFFICULTY");


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

    for(coreUintW i = 0u; i < MENU_GAME_EQUIPS; ++i)
    {
        m_aEquipName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aEquipName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.1f - 0.1f*I_TO_F(i)));
        m_aEquipName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aEquipName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aEquipLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aEquipLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aEquipLine[i].DefineProgram("menu_inner_program");
        m_aEquipLine[i].SetPosition  (coreVector2(0.0f, m_aEquipName[i].GetPosition().y));
        m_aEquipLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aEquipLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }
    m_aEquipName[0].SetTextLanguage("GAME_WEAPON");
    m_aEquipName[1].SetTextLanguage("GAME_SUPPORT");

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aWeapon[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aEquipName[0].GetPosition() - vOffset);
        m_aWeapon[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aWeapon[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aWeapon[i].SetEndless  (true);
        m_aWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aSupport[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aSupport[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aEquipName[1].GetPosition() - vOffset);
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

    // 
    m_Navigator.BindObject(&m_StandardTab, &m_BackButton,         &m_TrainingTab, m_WorldMap.GetPin(6u), &m_TrainingTab,        NULL, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_TrainingTab, &m_BackButton,         &m_StandardTab, m_WorldMap.GetPin(6u), &m_StandardTab,        NULL, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_StartButton, NULL,                  NULL,           m_WorldMap.GetPin(0u), m_WorldMap.GetPin(7u), NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_BackButton,  m_WorldMap.GetPin(2u), NULL,           m_WorldMap.GetPin(6u), NULL,                  NULL, MENU_TYPE_DEFAULT);

    m_Navigator.BindObject(m_WorldMap.GetPin(0u), m_WorldMap.GetPin(8u), NULL,                  &m_BackButton,         m_WorldMap.GetPin(1u), NULL,             MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(1u), m_WorldMap.GetPin(7u), m_WorldMap.GetPin(0u), &m_BackButton,         m_WorldMap.GetPin(2u), NULL,             MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(2u), m_WorldMap.GetPin(3u), m_WorldMap.GetPin(1u), &m_BackButton,         NULL,                  NULL,             MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(3u), m_WorldMap.GetPin(4u), NULL,                  m_WorldMap.GetPin(2u), NULL,                  NULL,             MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(4u), m_WorldMap.GetPin(5u), NULL,                  m_WorldMap.GetPin(3u), NULL,                  NULL,             MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(5u), NULL,                  m_WorldMap.GetPin(6u), m_WorldMap.GetPin(4u), NULL,                  NULL,             MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(6u), NULL,                  m_WorldMap.GetPin(7u), NULL,                  m_WorldMap.GetPin(5u), &m_aaStage[0][0], MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(7u), NULL,                  m_WorldMap.GetPin(8u), m_WorldMap.GetPin(1u), m_WorldMap.GetPin(6u), NULL,             MENU_TYPE_TAB_NODE);
    m_Navigator.BindObject(m_WorldMap.GetPin(8u), NULL,                  NULL,                  m_WorldMap.GetPin(0u), m_WorldMap.GetPin(7u), NULL,             MENU_TYPE_TAB_NODE);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_STAGES; ++j)
        {
            m_Navigator.BindObject(&m_aaStage[i][j], &m_aaStage[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS][j], &m_aaStage[i][(j + MENU_GAME_STAGES - 1u) % MENU_GAME_STAGES],
                                                     &m_aaStage[(i + 1u)                      % MENU_GAME_MISSIONS][j], &m_aaStage[i][(j + 1u)                    % MENU_GAME_STAGES], NULL, MENU_TYPE_TAB_NODE);
        }
    }

    m_Navigator.AssignSurface(&m_StandardTab, SURFACE_GAME_STANDARD);
    m_Navigator.AssignSurface(&m_TrainingTab, SURFACE_GAME_TRAINING);
    m_Navigator.AssignMenu(this);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_GAME_OPTIONS; ++i)
    {
        if(i != SURFACE_GAME_STANDARD) this->BindObject(i, &m_StandardTab);
        if(i != SURFACE_GAME_TRAINING) this->BindObject(i, &m_TrainingTab);

        this->BindObject(i, &m_Background);

        if(i == SURFACE_GAME_STANDARD) this->BindObject(i, &m_StandardTab);
        if(i == SURFACE_GAME_TRAINING) this->BindObject(i, &m_TrainingTab);

        this->BindObject(i, &m_OptionButton);
        this->BindObject(i, &m_BackButton);
        this->BindObject(i, &m_OptionState);
    }

    this->BindObject(SURFACE_GAME_STANDARD, &m_StandardArea);
    this->BindObject(SURFACE_GAME_STANDARD, &m_StandardMedal);
    this->BindObject(SURFACE_GAME_TRAINING, &m_StandardFragment);
    this->BindObject(SURFACE_GAME_STANDARD, &m_WorldMap);
    this->BindObject(SURFACE_GAME_STANDARD, &m_StandardHeader);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStandardName);  ++i) this->BindObject(SURFACE_GAME_STANDARD, &m_aStandardName [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStandardScore); ++i) this->BindObject(SURFACE_GAME_STANDARD, &m_aStandardScore[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aStandardTime);  ++i) this->BindObject(SURFACE_GAME_STANDARD, &m_aStandardTime [i]);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS;           ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aMissionLine  [i]);
    this->BindObject(SURFACE_GAME_TRAINING, &m_TrainingArea);
    this->BindObject(SURFACE_GAME_TRAINING, &m_TrainingMedal);
    this->BindObject(SURFACE_GAME_TRAINING, &m_TrainingFragment);
    for(coreUintW i = 0u; i < BADGES;                       ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aTrainingBadge[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS;           ++i) for(coreUintW j = 0u; j < MENU_GAME_STAGES; ++j) this->BindObject(SURFACE_GAME_TRAINING, &m_aaStage[i][j]);
    this->BindObject(SURFACE_GAME_TRAINING, &m_aStageCursor);
    this->BindObject(SURFACE_GAME_TRAINING, &m_TrainingHeader);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS;           ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aMissionName  [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrainingName);  ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aTrainingName [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrainingScore); ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aTrainingScore[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrainingTime);  ++i) this->BindObject(SURFACE_GAME_TRAINING, &m_aTrainingTime [i]);

    this->BindObject(SURFACE_GAME_OPTIONS, &m_Background);
    this->BindObject(SURFACE_GAME_OPTIONS, &m_BackButton);
    this->BindObject(SURFACE_GAME_OPTIONS, &m_ArmoryHeader);

    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_OPTIONS, &m_aOptionLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_OPTIONS, &m_aOptionName[i]);

    this->BindObject(SURFACE_GAME_OPTIONS, &m_Type);
    this->BindObject(SURFACE_GAME_OPTIONS, &m_Mode);
    this->BindObject(SURFACE_GAME_OPTIONS, &m_Difficulty);

    this->BindObject(SURFACE_GAME_ARMORY, &m_Background);
    this->BindObject(SURFACE_GAME_ARMORY, &m_StartButton);
    this->BindObject(SURFACE_GAME_ARMORY, &m_BackButton);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryHeader);

    for(coreUintW i = 0u; i < MENU_GAME_EQUIPS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aEquipLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_EQUIPS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aEquipName[i]);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeapon     [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupport    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aWeaponIcon [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aSupportIcon[i]);

    for(coreUintW i = 0u; i < SURFACE_GAME_MAX; ++i) this->BindObject(i, &m_Navigator);

    // 
    this->DeactivateFirstPlay();

    
    //m_TrainingTab.SetOverride(-1);   // TODO 1 
}


// ****************************************************************
// move the game menu
void cGameMenu::Move()
{
    // 
    m_Navigator.Update();

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
                m_iCurPage = 0u;

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

            this->__SelectStandard(m_WorldMap.GetSelectionIndex());
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
            
            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                for(coreUintW j = 0u; j < MENU_GAME_STAGES; ++j)
                {
                    if(m_aaStage[i][j].IsClicked())
                    {
                        // 
                        m_iCurPage = 1u;

                        // 
                        g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, true, false);
                        break;
                    }

                    if(m_aaStage[i][j].IsFocused())
                    {
                        m_aiStageSelection[0] = i;
                        m_aiStageSelection[1] = j;
                        break;
                    }
                }
            }
            
            this->__SelectTraining(m_aiStageSelection[0] + 1u + ((m_aiStageSelection[0] >= 7u) ? 1u : 0u), m_aiStageSelection[1]);
            
            const cGuiObject& oStage = m_aaStage[m_aiStageSelection[0]][m_aiStageSelection[1]];
            
            m_aStageCursor.DefineTexture(0u, oStage.GetTexture(0u));
            m_aStageCursor.SetPosition(oStage.GetPosition());
            m_aStageCursor.SetSize    (oStage.GetSize    () * 1.25f + coreVector2(0.02f,0.02f));
            m_aStageCursor.Move();
            
                 if(m_aiStageSelection[0] == 0u) m_aStageCursor.SetColor3(cGrassBackground  ::Color);
            else if(m_aiStageSelection[0] == 1u) m_aStageCursor.SetColor3(cSeaBackground    ::Color);
            else if(m_aiStageSelection[0] == 2u) m_aStageCursor.SetColor3(cDesertBackground ::Color);
            else if(m_aiStageSelection[0] == 3u) m_aStageCursor.SetColor3(cSpaceBackground  ::Color);
            else if(m_aiStageSelection[0] == 4u) m_aStageCursor.SetColor3(cVolcanoBackground::Color);
            else if(m_aiStageSelection[0] == 5u) m_aStageCursor.SetColor3(cSnowBackground   ::Color);
            else if(m_aiStageSelection[0] == 6u) m_aStageCursor.SetColor3(cMossBackground   ::Color);
            else                                 m_aStageCursor.SetColor3(cDarkBackground   ::Color);
            
            m_aMissionName[0].SetColor3((m_aiStageSelection[0] == 0u) ? cGrassBackground  ::Color : COLOR_MENU_WHITE);
            m_aMissionName[1].SetColor3((m_aiStageSelection[0] == 1u) ? cSeaBackground    ::Color : COLOR_MENU_WHITE);
            m_aMissionName[2].SetColor3((m_aiStageSelection[0] == 2u) ? cDesertBackground ::Color : COLOR_MENU_WHITE);
            m_aMissionName[3].SetColor3((m_aiStageSelection[0] == 3u) ? cSpaceBackground  ::Color : COLOR_MENU_WHITE);
            m_aMissionName[4].SetColor3((m_aiStageSelection[0] == 4u) ? cVolcanoBackground::Color : COLOR_MENU_WHITE);
            m_aMissionName[5].SetColor3((m_aiStageSelection[0] == 5u) ? cSnowBackground   ::Color : COLOR_MENU_WHITE);
            m_aMissionName[6].SetColor3((m_aiStageSelection[0] == 6u) ? cMossBackground   ::Color : COLOR_MENU_WHITE);
            m_aMissionName[7].SetColor3((m_aiStageSelection[0] == 7u) ? cDarkBackground   ::Color : COLOR_MENU_WHITE);
            m_aMissionName[8].SetColor3((m_aiStageSelection[0] == 8u) ? cDarkBackground   ::Color : COLOR_MENU_WHITE);
            
            m_aMissionLine[0].SetColor3((m_aiStageSelection[0] == 0u) ? cGrassBackground  ::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[1].SetColor3((m_aiStageSelection[0] == 1u) ? cSeaBackground    ::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[2].SetColor3((m_aiStageSelection[0] == 2u) ? cDesertBackground ::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[3].SetColor3((m_aiStageSelection[0] == 3u) ? cSpaceBackground  ::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[4].SetColor3((m_aiStageSelection[0] == 4u) ? cVolcanoBackground::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[5].SetColor3((m_aiStageSelection[0] == 5u) ? cSnowBackground   ::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[6].SetColor3((m_aiStageSelection[0] == 6u) ? cMossBackground   ::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[7].SetColor3((m_aiStageSelection[0] == 7u) ? cDarkBackground   ::Color : coreVector3(1.0f,1.0f,1.0f));
            m_aMissionLine[8].SetColor3((m_aiStageSelection[0] == 8u) ? cDarkBackground   ::Color : coreVector3(1.0f,1.0f,1.0f));
            
            
            /*
            m_aMissionName[0].SetColor3(cDesertBackground ::Color);
            m_aMissionName[1].SetColor3(cVolcanoBackground::Color);
            m_aMissionName[2].SetColor3(cMossBackground   ::Color);
            m_aMissionName[3].SetColor3(cSpaceBackground  ::Color);
            m_aMissionName[4].SetColor3(cCloudBackground  ::Color);
            m_aMissionName[5].SetColor3(cSnowBackground   ::Color);
            m_aMissionName[6].SetColor3(cSeaBackground    ::Color);
            m_aMissionName[7].SetColor3(cGrassBackground  ::Color);
            m_aMissionName[8].SetColor3(cDarkBackground   ::Color);
            
            m_aMissionLine[0].SetColor3(cDesertBackground ::Color);
            m_aMissionLine[1].SetColor3(cVolcanoBackground::Color);
            m_aMissionLine[2].SetColor3(cMossBackground   ::Color);
            m_aMissionLine[3].SetColor3(cSpaceBackground  ::Color);
            m_aMissionLine[4].SetColor3(cCloudBackground  ::Color);
            m_aMissionLine[5].SetColor3(cSnowBackground   ::Color);
            m_aMissionLine[6].SetColor3(cSeaBackground    ::Color);
            m_aMissionLine[7].SetColor3(cGrassBackground  ::Color);
            m_aMissionLine[8].SetColor3(cDarkBackground   ::Color);
            */
        }
        break;

    case SURFACE_GAME_OPTIONS:
        {
            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                g_pMenu->ShiftSurface(this, this->GetOldSurface(), 3.0f, false, true);
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
            else if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
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


            const coreBool bMulti = (m_Type.GetCurValue() != GAME_TYPE_SOLO);
            m_aWeapon [1].SetOverride(bMulti ? 0 : -1);
            m_aSupport[1].SetOverride(bMulti ? 0 : -1);

            if(false)
            {
                const coreVector2 vOffset = coreVector2(bMulti ? 0.25f : 0.125f, 0.0f);

                m_aWeapon     [0].SetPosition(coreVector2(-1.00f,1.00f) * m_aOptionName[1].GetPosition() - vOffset);
                m_aSupport    [0].SetPosition(coreVector2(-1.00f,1.00f) * m_aOptionName[2].GetPosition() - vOffset);
                m_aWeaponIcon [0].SetPosition(m_aWeapon [0].GetPosition() + m_aWeapon [0].GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.0f,0.05f));
                m_aSupportIcon[0].SetPosition(m_aSupport[0].GetPosition() + m_aSupport[0].GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.0f,0.05f));
            }

            {
                m_aWeapon     [1].SetEnabled(bMulti ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aSupport    [1].SetEnabled(bMulti ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aWeaponIcon [1].SetEnabled(bMulti ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aSupportIcon[1].SetEnabled(bMulti ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            }

        }
        break;

    default:
        ASSERT(false)
        break;
    }
    
    if(m_OptionButton.IsClicked())
    {
        g_pMenu->ShiftSurface(this, SURFACE_GAME_OPTIONS, 3.0f, true, false);
    }

    // 
    cMenu::UpdateButton(&m_StandardTab, (this->GetCurSurface() == SURFACE_GAME_STANDARD) || m_StandardTab.IsFocused());
    cMenu::UpdateButton(&m_TrainingTab, (this->GetCurSurface() == SURFACE_GAME_TRAINING) || m_TrainingTab.IsFocused());

    // 
    cMenu::UpdateButton(&m_StartButton,  m_StartButton .IsFocused());
    cMenu::UpdateButton(&m_OptionButton, m_OptionButton.IsFocused());
    cMenu::UpdateButton(&m_BackButton,   m_BackButton  .IsFocused());

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_BackButton), TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
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
        m_WorldMap.EnablePin(i, bEnable, bEnable || (g_aMissionData[i].iID == cAterMission::ID));
    }

    // 
    m_WorldMap.SelectPin(g_pSave->GetHeader().oOptions.iStandard);
    m_aiStageSelection[0] = g_pSave->GetHeader().oOptions.iTraining / MENU_GAME_STAGES;
    m_aiStageSelection[1] = g_pSave->GetHeader().oOptions.iTraining % MENU_GAME_STAGES;

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
    g_pSave->EditOptions()->iTraining = m_aiStageSelection[0] * MENU_GAME_STAGES + m_aiStageSelection[1];

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


// ****************************************************************
// 
void cGameMenu::RetrieveStartData(coreInt32* OUTPUT piMissionID, coreUint8* OUTPUT piTakeFrom, coreUint8* OUTPUT piTakeTo)//const
{
    
    // TODO 1: intro liefert m_aiCurIndex[0]==-1
    if(m_aiCurIndex[0] == coreUintW(-1)) m_aiCurIndex[0] = 0u;
    if(m_aiCurIndex[1] == coreUintW(-1)) m_aiCurIndex[1] = 0u;
    if(m_aiCurIndex[2] == coreUintW(-1)) m_aiCurIndex[2] = 0u;
    
    if(m_iCurPage == 0u)
    {
        // 
        (*piMissionID) = g_aMissionData[m_aiCurIndex[0]].iID;
        (*piTakeFrom)  = 0u;
        (*piTakeTo)    = TAKE_MISSION;
    }
    else if(m_iCurPage == 1u)
    {
        // 
        (*piMissionID) = g_aMissionData[m_aiCurIndex[1]].iID;
        (*piTakeFrom)  = m_aiCurIndex[2];
        (*piTakeTo)    = m_aiCurIndex[2];
    }
    else ASSERT(false)
}


// ****************************************************************
// 
void cGameMenu::__SelectStandard(const coreUintW iMissionIndex)
{
    ASSERT(iMissionIndex < MISSIONS)

    // 
    if(m_aiCurIndex[0] == iMissionIndex) return;
    m_aiCurIndex[0] = iMissionIndex;

    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];

    // 
    m_aStandardName [0].SetColor3(oData.vColor);
    m_aStandardScore[0].SetColor3(oData.vColor);
    m_aStandardTime [0].SetColor3(oData.vColor);

    // 
    m_aStandardName [0].SetText(oData.pcName);
    m_aStandardName [1].SetText(PRINT("%s %zu", Core::Language->GetString("MISSION"), iMissionIndex));
    m_aStandardScore[1].SetText(coreData::ToChars(Core::Rand->Uint(999999u)));   // TODO 1: read real data
    m_aStandardTime [1].SetText(PRINT("%.1f", Core::Rand->Float(99.9f)));   // TODO 1: read real data

    // 
    cMenu::ApplyMedalTexture(&m_StandardMedal, Core::Rand->Uint(MEDAL_BRONZE, MEDAL_DARK), MEDAL_TYPE_MISSION);   // TODO 1: read real data
}


// ****************************************************************
// 
void cGameMenu::__SelectTraining(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT((iMissionIndex < MISSIONS) && (iSegmentIndex < SEGMENTS))

    // 
    if((m_aiCurIndex[1] == iMissionIndex) && (m_aiCurIndex[2] == iSegmentIndex)) return;
    m_aiCurIndex[1] = iMissionIndex;
    m_aiCurIndex[2] = iSegmentIndex;

    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];
    const coreBool      bBoss = MISSION_SEGMENT_IS_BOSS(iSegmentIndex);

    // 
    const coreChar* pcMissionLetter;
    switch(iMissionIndex)
    {
    case  9u: pcMissionLetter = "X1";                             break;
    case 10u: pcMissionLetter = "X2";                             break;
    default:  pcMissionLetter = coreData::ToChars(iMissionIndex); break;
    }

    // 
    const coreChar* pcSegmentLetter = bBoss ? "B" : coreData::ToChars(iSegmentIndex + 1u);

    // 
    m_aTrainingName [0].SetColor3(oData.vColor);
    m_aTrainingScore[0].SetColor3(oData.vColor);
    m_aTrainingTime [0].SetColor3(oData.vColor);

    // 
    m_aTrainingName [0].SetText(oData.pcName);
    m_aTrainingName [1].SetText(PRINT("%s %s-%s", Core::Language->GetString("MISSION"), pcMissionLetter, pcSegmentLetter));
    m_aTrainingScore[1].SetText(coreData::ToChars(Core::Rand->Uint(999999u)));   // TODO 1: read real data
    m_aTrainingTime [1].SetText(PRINT("%.1f", Core::Rand->Float(99.9f)));   // TODO 1: read real data

    // 
    cMenu::ApplyMedalTexture(&m_TrainingMedal, Core::Rand->Uint(MEDAL_BRONZE, MEDAL_DARK), bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);   // TODO 1: read real data
    // TODO 1: badges
}