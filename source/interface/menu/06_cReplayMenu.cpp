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
cReplayMenu::cReplayMenu()noexcept
: coreMenu            (SURFACE_REPLAY_MAX, SURFACE_REPLAY_OVERVIEW)
, m_iDetailStartIndex (0u)
, m_iSelected         (0u)
, m_iOverwrite        (0u)
, m_bDelete           (false)
, m_bRename           (false)
, m_bSave             (false)
, m_bReturn           (false)
, m_bDownloaded       (false)
, m_iPageOffset       (0u)
, m_iPageMax          (0u)
, m_bPageChanged      (false)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.045f));
    m_Background.SetSize      (coreVector2(0.9f,0.82f));

    m_Header.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_Header.SetPosition    (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_Header.SetColor3      (COLOR_MENU_WHITE);
    m_Header.SetTextLanguage("REPLAYS");

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f) + MENU_BUTTON_SHIFT);
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButton.GetCaption()->SetTextLanguage("START");

    m_DeleteButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_DeleteButton.DefineProgram("menu_border_program");
    m_DeleteButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_DeleteButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_DeleteButton.GetCaption()->SetText(ICON_TRASH_CAN);

    m_RenameButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_RenameButton.DefineProgram("menu_border_program");
    m_RenameButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_RenameButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_RenameButton.GetCaption()->SetText(ICON_PEN);

    m_SaveButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_SaveButton.DefineProgram("menu_border_program");
    m_SaveButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_SaveButton.SetAlignment (coreVector2(-1.0f,-1.0f));

    m_BackButtonOverview.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonOverview.DefineProgram("menu_border_program");
    m_BackButtonOverview.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButtonOverview.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_BackButtonOverview.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButtonOverview.GetCaption()->SetText(ICON_SHARE);

    m_BackButtonDetails.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonDetails.DefineProgram("menu_border_program");
    m_BackButtonDetails.SetPosition  (m_BackButtonOverview.GetPosition ());
    m_BackButtonDetails.SetSize      (m_BackButtonOverview.GetSize     ());
    m_BackButtonDetails.SetAlignment (m_BackButtonOverview.GetAlignment());
    m_BackButtonDetails.GetCaption()->SetText(ICON_SHARE);

    m_RenameButton.SetPosition(m_BackButtonDetails.GetPosition() - coreVector2(m_BackButtonDetails.GetSize().x + 0.02f, 0.0f));
    m_DeleteButton.SetPosition(m_RenameButton     .GetPosition() - coreVector2(m_RenameButton     .GetSize().x + 0.02f, 0.0f));
    m_SaveButton  .SetPosition(m_RenameButton     .GetPosition());

    m_PageSelection.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_2, MENU_FONT_ICON_3 + MENU_SWITCHBOX_ZOOM, MENU_OUTLINE_SMALL);
    m_PageSelection.SetPosition(m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.1f));
    m_PageSelection.SetSize    (coreVector2(0.73f,0.065f));
    m_PageSelection.SetEndless (true);
    m_PageSelection.GetArrow(0u)->DefineProgram("menu_border_program");
    m_PageSelection.GetArrow(1u)->DefineProgram("menu_border_program");
    m_PageSelection.AddEntry("", 0u);
    m_PageSelection.AddEntry("", 1u);

    m_PageText.Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_PageText.SetPosition(m_PageSelection.GetPosition());
    m_PageText.SetColor3  (COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        const coreFloat fHeight = m_Background.GetPosition().y + m_Background.GetSize().y*0.5f - (REPLAY_SLOTSYSTEM ? 0.1f : 0.17f) - 0.05f*I_TO_F(i);

        m_aName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition (coreVector2(m_Background.GetPosition().x - m_Background.GetSize().x*0.5f, fHeight) + coreVector2(0.04f,0.0f));
        m_aName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aTime[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aTime[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aName[i].GetPosition());
        m_aTime[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aTime[i].SetColor3   (COLOR_MENU_WHITE);

        m_aScore[i].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aScore[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aScore[i].SetColor3  (COLOR_MENU_WHITE);

        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, fHeight));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i + 1u)*0.09f, 0.0f));
        m_aLine[i].SetFocusable (true);
    }

    if(REPLAY_SLOTSYSTEM)
    {
        m_ReplayBox.SetPosition(m_Background.GetPosition() + coreVector2(0.0f,-0.025f));
        m_ReplayBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.72f));
    }
    else
    {
        m_ReplayBox.SetPosition(m_Background.GetPosition() + coreVector2(0.0f,-0.06f));
        m_ReplayBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.65f));
    }
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) m_ReplayBox.BindObject(&m_aLine [i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) m_ReplayBox.BindObject(&m_aName [i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) m_ReplayBox.BindObject(&m_aTime [i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) m_ReplayBox.BindObject(&m_aScore[i]);

    m_Nothing.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_Nothing.SetPosition    (m_ReplayBox.GetPosition());
    m_Nothing.SetColor3      (COLOR_MENU_WHITE);
    m_Nothing.SetTextLanguage("GAME_WEAPON_NOTHING");

    m_DetailSelection.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_2, MENU_FONT_ICON_3 + MENU_SWITCHBOX_ZOOM, MENU_OUTLINE_SMALL);
    m_DetailSelection.SetPosition(m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.08f));
    m_DetailSelection.SetSize    (coreVector2(0.73f,0.065f));
    m_DetailSelection.SetEndless (true);
    m_DetailSelection.GetArrow(0u)->DefineProgram("menu_border_program");
    m_DetailSelection.GetArrow(1u)->DefineProgram("menu_border_program");
    m_DetailSelection.AddEntry("", 0u);
    m_DetailSelection.AddEntry("", 1u);

    coreUintW iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAILS; ++i)
    {
        m_aDetailName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aDetailName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.495f - 0.025f*I_TO_F(iOffset)));
        m_aDetailName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aDetailName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aDetailLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aDetailLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aDetailLine[i].DefineProgram("menu_inner_program");
        m_aDetailLine[i].SetPosition  (coreVector2(0.0f, m_aDetailName[i].GetPosition().y));
        m_aDetailLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aDetailLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aDetailLine[i].SetFocusable (true);

        iOffset += (i == 2u || i == 4u || i == 7u) ? 3u : 2u;

#if defined(_CORE_SWITCH_)
        if(i == 6u) iOffset -= 2u;
#endif
    }
    m_aDetailName[0].SetTextLanguage("GAME_TYPE");
    m_aDetailName[1].SetTextLanguage("GAME_GAMESPEED");
    m_aDetailName[2].SetTextLanguage("GAME_DIFFICULTY");
    m_aDetailName[4].SetTextLanguage("GAME_SHIELD");
    m_aDetailName[5].SetTextLanguage("CONFIG_GAME_BACKROTATION");
    m_aDetailName[6].SetTextLanguage("CONFIG_GAME_BACKSPEED");
    m_aDetailName[7].SetTextLanguage("CONFIG_GAME_UPDATEFREQ");

    m_DetailArea.DefineTexture(0u, "menu_detail_04.png");
    m_DetailArea.DefineTexture(1u, "menu_background_black.png");
    m_DetailArea.DefineProgram("menu_inner_program");
    m_DetailArea.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.205f));
    m_DetailArea.SetSize      (coreVector2(m_Background.GetSize().x, 0.12f));
    m_DetailArea.SetTexOffset (coreVector2(I_TO_F(-2)*0.09f, 0.0f));

    m_DetailArea2.DefineTexture(0u, "menu_detail_04.png");
    m_DetailArea2.DefineTexture(1u, "menu_background_black.png");
    m_DetailArea2.DefineProgram("menu_inner_program");
    m_DetailArea2.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.355f));
    m_DetailArea2.SetSize      (coreVector2(m_Background.GetSize().x, 0.18f));
    m_DetailArea2.SetTexOffset (coreVector2(I_TO_F(-1)*0.09f, 0.0f));

    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_MEDALS; ++i)
    {
        m_aDetailMedal[i].DefineTexture(0u, "menu_medal.png");
        m_aDetailMedal[i].DefineProgram("default_2d_program");
        m_aDetailMedal[i].SetPosition  (i ? (m_DetailArea2.GetPosition() + coreVector2((I_TO_F(i - 1u) - 4.0f) * 0.09f, -0.023f)) : m_DetailArea.GetPosition());
        m_aDetailMedal[i].SetSize      (coreVector2(1.0f,1.0f) * (i ? 0.08f : 0.115f));
        m_aDetailMedal[i].SetTexSize   (coreVector2(0.25f,0.25f));

        m_aDetailMedalBack[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aDetailMedalBack[i].DefineProgram("menu_single_program");
        m_aDetailMedalBack[i].SetPosition  (m_aDetailMedal[i].GetPosition());
        m_aDetailMedalBack[i].SetSize      (m_aDetailMedal[i].GetSize() + coreVector2(0.05f,0.05f));
        m_aDetailMedalBack[i].SetColor3    (coreVector3(0.0f,0.0f,0.0f));
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_ICONS; ++i)
    {
        m_aDetailIcon[i].DefineTexture(0u, "menu_helper.png");
        m_aDetailIcon[i].DefineProgram("menu_helper_program");
        m_aDetailIcon[i].SetPosition  (m_aDetailMedal[i + 1u].GetPosition() + coreVector2(0.0f,0.04f));
        m_aDetailIcon[i].SetSize      (coreVector2(1.0f,1.0f) * 0.065f);
        m_aDetailIcon[i].SetColor3    (g_aMissionData[i].vColor * 1.0f);
        m_aDetailIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aDetailIcon[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aDetailIconBack[i].DefineTexture(0u, "menu_helper.png");
        m_aDetailIconBack[i].DefineProgram("menu_helper_program");
        m_aDetailIconBack[i].SetPosition  (m_aDetailIcon[i].GetPosition ());
        m_aDetailIconBack[i].SetSize      (m_aDetailIcon[i].GetSize     () * 1.2f);
        m_aDetailIconBack[i].SetColor3    (m_aDetailIcon[i].GetColor3   ());
        m_aDetailIconBack[i].SetTexSize   (m_aDetailIcon[i].GetTexSize  ());
        m_aDetailIconBack[i].SetTexOffset (m_aDetailIcon[i].GetTexOffset());

        STATIC_ASSERT(MENU_REPLAY_DETAIL_ICONS + 1u == MENU_REPLAY_DETAIL_MEDALS)
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_ARROWS; ++i)
    {
        m_aDetailArrow[i].DefineTexture(0u, "menu_arrow.png");
        m_aDetailArrow[i].DefineProgram("menu_single_program");
        m_aDetailArrow[i].SetPosition  (LERP(m_aDetailMedal[i + 1u].GetPosition(), m_aDetailMedal[i + 2u].GetPosition(), 0.5f) + coreVector2(0.001f,0.0f));
        m_aDetailArrow[i].SetSize      (coreVector2(0.021f,0.021f));
        m_aDetailArrow[i].SetColor3    (COLOR_MENU_WHITE);

        STATIC_ASSERT(MENU_REPLAY_DETAIL_ARROWS + 2u <= MENU_REPLAY_DETAIL_MEDALS)
    }

    m_DetailTitle.Construct   (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_DetailTitle.SetPosition (m_DetailArea.GetPosition() + coreVector2(0.0f,0.123f));
    m_DetailTitle.SetAlignment(coreVector2(0.0f,0.0f));
    m_DetailTitle.SetColor3   (COLOR_MENU_WHITE);

    m_DetailTitleSmall.Construct   (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_DetailTitleSmall.SetPosition (m_DetailTitle.GetPosition ());
    m_DetailTitleSmall.SetAlignment(m_DetailTitle.GetAlignment());
    m_DetailTitleSmall.SetColor3   (m_DetailTitle.GetColor3   ());

    m_DetailTitleSmall2.Construct   (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_DetailTitleSmall2.SetPosition (m_DetailTitle.GetPosition ());
    m_DetailTitleSmall2.SetAlignment(m_DetailTitle.GetAlignment());
    m_DetailTitleSmall2.SetColor3   (m_DetailTitle.GetColor3   ());

    m_aDetailScore[0].Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_aDetailScore[0].SetPosition    (m_DetailArea.GetPosition() + coreVector2(-0.235f,0.018f));
    m_aDetailScore[0].SetColor3      (g_aMissionData[MISSION_ATER].vColor);
    m_aDetailScore[0].SetTextLanguage("STAT_SCORE_TOTAL");

    m_aDetailScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aDetailScore[1].SetPosition(m_aDetailScore[0].GetPosition() + coreVector2(0.0f,-0.036f));
    m_aDetailScore[1].SetColor3  (COLOR_MENU_WHITE);

    m_aDetailTime[0].Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_aDetailTime[0].SetPosition    (m_DetailArea.GetPosition() + coreVector2(0.235f,0.018f));
    m_aDetailTime[0].SetColor3      (g_aMissionData[MISSION_ATER].vColor);
    m_aDetailTime[0].SetTextLanguage("STAT_TIME_TOTAL");

    m_aDetailTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aDetailTime[1].SetPosition(m_aDetailTime[0].GetPosition() + coreVector2(0.0f,-0.036f));
    m_aDetailTime[1].SetColor3  (COLOR_MENU_WHITE);

    m_DetailType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1 + MENU_SWITCHBOX_ZOOM, MENU_OUTLINE_SMALL);
    m_DetailType.SetPosition (coreVector2(-1.00f,1.00f) * m_aDetailName[0].GetPosition());
    m_DetailType.SetSize     (coreVector2( 0.47f,0.03f));
    m_DetailType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_DetailType.SetOverride (-1);
    m_DetailType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DetailDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1 + MENU_SWITCHBOX_ZOOM, MENU_OUTLINE_SMALL);
    m_DetailDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aDetailName[2].GetPosition());
    m_DetailDifficulty.SetSize     (m_DetailType.GetSize());
    m_DetailDifficulty.SetAlignment(m_DetailType.GetAlignment());
    m_DetailDifficulty.SetOverride (-1);
    m_DetailDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DetailSpeed.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1 + MENU_SWITCHBOX_ZOOM, MENU_OUTLINE_SMALL);
    m_DetailSpeed.SetPosition (coreVector2(-1.00f,1.00f) * m_aDetailName[1].GetPosition());
    m_DetailSpeed.SetSize     (m_DetailType.GetSize());
    m_DetailSpeed.SetAlignment(m_DetailType.GetAlignment());
    m_DetailSpeed.SetOverride (-1);
    m_DetailSpeed.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aDetailShield[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1 + MENU_SWITCHBOX_ZOOM, MENU_OUTLINE_SMALL);
        m_aDetailShield[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aDetailName[4].GetPosition() - vOffset);
        m_aDetailShield[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aDetailShield[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aDetailShield[i].SetOverride (-1);
        m_aDetailShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aDetailWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1 + MENU_SWITCHBOX_ZOOM, MENU_OUTLINE_SMALL);
        m_aDetailWeapon[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aDetailName[3].GetPosition() - vOffset);
        m_aDetailWeapon[i].SetSize     (m_aDetailShield[i].GetSize());
        m_aDetailWeapon[i].SetAlignment(m_aDetailShield[i].GetAlignment());
        m_aDetailWeapon[i].SetOverride (-1);
        m_aDetailWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aDetailWeaponIcon[i].DefineTexture(0u, "menu_weapon.png");
        m_aDetailWeaponIcon[i].DefineProgram("default_2d_program");
        m_aDetailWeaponIcon[i].SetPosition  (m_aDetailWeapon[i].GetPosition() + coreVector2(m_aDetailWeapon[i].GetSize().x * -0.5f, 0.025f));
        m_aDetailWeaponIcon[i].SetSize      (coreVector2(1.0f, 1.0f) * 0.085f);
        m_aDetailWeaponIcon[i].SetTexSize   (coreVector2(0.25f,0.5f));
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_DATAS; ++i)
    {
        m_aDetailData[i].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aDetailData[i].SetPosition(coreVector2(-0.165f + 3.0f * 0.17f, m_aDetailName[5u + i].GetPosition().y));
        m_aDetailData[i].SetColor3  (COLOR_MENU_WHITE);
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_REPLAY_DETAIL_RUNS; ++j)
        {
            m_aaDetailRun[i][j].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
            m_aaDetailRun[i][j].SetPosition(coreVector2(-0.165f + I_TO_F(j) * 0.17f, m_aDetailName[i + MENU_REPLAY_MISSION_OFFSET].GetPosition().y));   // # aligned with options
        }
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDetailStartArrow); ++i)
    {
        m_aDetailStartArrow[i].DefineTexture(0u, "menu_arrow.png");
        m_aDetailStartArrow[i].DefineProgram("menu_single_program");
        m_aDetailStartArrow[i].SetSize      (coreVector2( 1.0f,1.0f) * (i ? 0.021f : 0.034f));
        m_aDetailStartArrow[i].SetDirection (coreVector2(-1.0f,0.0f));
        m_aDetailStartArrow[i].SetColor3    (i ? COLOR_MENU_WHITE : coreVector3( 0.1f,0.1f,0.1f));
    }

    m_DetailStartLabel.Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_DetailStartLabel.SetTextLanguage("BEGINNING");
    m_DetailStartLabel.SetRectifyY    (false);

    m_DetailStartBack.DefineTexture(0u, "effect_headlight_point.png");
    m_DetailStartBack.DefineProgram("menu_single_program");
    m_DetailStartBack.SetSize      (coreVector2(0.1f,0.05f));
    m_DetailStartBack.SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_DetailBox.SetPosition(m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.6325f));
    m_DetailBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.325f));
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAILS;      ++i) m_DetailBox.BindObject(&m_aDetailLine[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAILS;      ++i) m_DetailBox.BindObject(&m_aDetailName[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_DATAS; ++i) m_DetailBox.BindObject(&m_aDetailData[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_MISSIONS;     ++i) for(coreUintW j = 0u; j < MENU_REPLAY_DETAIL_RUNS; ++j) m_DetailBox.BindObject(&m_aaDetailRun[i][j]);
    m_DetailBox.BindObject(&m_DetailType);
    m_DetailBox.BindObject(&m_DetailDifficulty);
    m_DetailBox.BindObject(&m_DetailSpeed);
    for(coreUintW i = 0u; i < MENU_REPLAY_PLAYERS; ++i) m_DetailBox.BindObject(&m_aDetailWeaponIcon[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_PLAYERS; ++i) m_DetailBox.BindObject(&m_aDetailShield    [i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_PLAYERS; ++i) m_DetailBox.BindObject(&m_aDetailWeapon    [i]);

    m_RenameHeader.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_RenameHeader.SetPosition    (coreVector2(0.0f,0.35f));
    m_RenameHeader.SetColor3      (COLOR_MENU_WHITE);
    m_RenameHeader.SetTextLanguage("ENTER_RENAME");

    // 
    m_DetailType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_DetailType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_DetailType      .AddEntryLanguage("GAME_TYPE_DUEL",         GAME_TYPE_DUEL);
    m_DetailDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_DetailDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    m_DetailDifficulty.AddEntryLanguage("GAME_DIFFICULTY_HARD",   GAME_DIFFICULTY_HARD);
    for(coreUintW i = 50u, ie = 200u; i <= ie; i += 5u) m_DetailSpeed.AddEntry(PRINT("%zu%%", i), i);

    for(coreUintW i = 0u; i < MENU_REPLAY_PLAYERS; ++i)
    {
        m_aDetailShield[i].AddEntryLanguage("VALUE_NO", 0u);
        for(coreUintW j = 5u, je = 100u; j <= je; j += 5u) m_aDetailShield[i].AddEntry(PRINT("%zu", j), j);
        m_aDetailShield[i].AddEntry(coreData::ToChars(SHIELD_INVINCIBLE), SHIELD_MAX);
        m_aDetailShield[i].AddEntry("-",                                  0xFFu);

        m_aDetailWeapon[i].AddEntryLanguage("GAME_WEAPON_RAY",   1u);
        m_aDetailWeapon[i].AddEntryLanguage("GAME_WEAPON_PULSE", 2u);
        m_aDetailWeapon[i].AddEntryLanguage("GAME_WEAPON_WAVE",  3u);
        m_aDetailWeapon[i].AddEntryLanguage("GAME_WEAPON_TESLA", 4u);
        m_aDetailWeapon[i].AddEntryLanguage("GAME_WEAPON_ANTI",  5u);
        m_aDetailWeapon[i].AddEntryLanguage("UNKNOWN",           0xFFu);
        m_aDetailWeapon[i].AddEntry        ("-",                 0xFFu);
    }

    // 
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        m_NavigatorOverview.BindObject(&m_aLine[i], (i == 0u)                       ? s_cast<coreObject2D*>(&m_PageSelection)      : &m_aLine[i - 1u], NULL,
                                                    (i == MENU_REPLAY_ENTRIES - 1u) ? s_cast<coreObject2D*>(&m_BackButtonOverview) : &m_aLine[i + 1u], NULL, MENU_TYPE_DEFAULT);
    }

    m_NavigatorOverview.BindObject(&m_PageSelection,      &m_BackButtonOverview,              NULL, &m_aLine[0],      NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorOverview.BindObject(&m_BackButtonOverview, &m_aLine[MENU_REPLAY_ENTRIES - 1u], NULL, &m_PageSelection, NULL, MENU_TYPE_DEFAULT);

    m_NavigatorOverview.BindScroll(&m_ReplayBox);

    m_NavigatorOverview.AssignFirst(&m_aLine[0]);   // TODO 1: changed in ResetNavigator            
    m_NavigatorOverview.AssignBack (&m_BackButtonOverview);
    m_NavigatorOverview.AssignMenu (this);

    if(!REPLAY_SLOTSYSTEM)
    {
        m_NavigatorOverview.UseShoulderLeft ([this]() {if(m_iPageMax > 1) {if(--m_iPageOffset >= m_iPageMax) m_iPageOffset = m_iPageMax - 1u; this->__UpdateList(true);} g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});
        m_NavigatorOverview.UseShoulderRight([this]() {if(m_iPageMax > 1) {if(++m_iPageOffset >= m_iPageMax) m_iPageOffset = 0u;              this->__UpdateList(true);} g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});

        m_NavigatorOverview.SetShoulder(true);
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_DETAILS; ++i)
    {
        m_NavigatorDetails.BindObject(&m_aDetailLine[i], (i == 0u)                       ? s_cast<coreObject2D*>(&m_DetailSelection) : &m_aDetailLine[i - 1u], NULL,
                                                         (i == MENU_REPLAY_DETAILS - 1u) ? s_cast<coreObject2D*>(&m_StartButton)     : &m_aDetailLine[i + 1u], NULL, MENU_TYPE_DEFAULT);
    }

    m_NavigatorDetails.BindObject(&m_DetailSelection,   &m_StartButton,                           NULL,                 &m_aDetailLine[0],  NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDetails.BindObject(&m_StartButton,       &m_aDetailLine[MENU_REPLAY_DETAILS - 1u], &m_BackButtonDetails, &m_DetailSelection, &m_DeleteButton,      MENU_TYPE_DEFAULT);
    m_NavigatorDetails.BindObject(&m_DeleteButton,      &m_aDetailLine[MENU_REPLAY_DETAILS - 1u], &m_StartButton,       &m_DetailSelection, &m_RenameButton,      MENU_TYPE_DEFAULT);
    m_NavigatorDetails.BindObject(&m_RenameButton,      &m_aDetailLine[MENU_REPLAY_DETAILS - 1u], &m_DeleteButton,      &m_DetailSelection, &m_SaveButton,        MENU_TYPE_DEFAULT);
    m_NavigatorDetails.BindObject(&m_SaveButton,        &m_aDetailLine[MENU_REPLAY_DETAILS - 1u], &m_RenameButton,      &m_DetailSelection, &m_BackButtonDetails, MENU_TYPE_DEFAULT);
    m_NavigatorDetails.BindObject(&m_BackButtonDetails, &m_aDetailLine[MENU_REPLAY_DETAILS - 1u], &m_SaveButton,        &m_DetailSelection, &m_StartButton,       MENU_TYPE_DEFAULT);

    m_NavigatorDetails.BindScroll(&m_DetailBox);

    m_NavigatorDetails.AssignFirst(&m_StartButton);
    m_NavigatorDetails.AssignBack (&m_BackButtonDetails);
    m_NavigatorDetails.AssignMenu (this);

    m_NavigatorDetails.UseShoulderLeft ([this]() {this->SelectPrevious(); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});
    m_NavigatorDetails.UseShoulderRight([this]() {this->SelectNext    (); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_REPLAY_RENAME; ++i)
    {
        this->BindObject(i, &m_Background);
    }

    this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_Header);
    this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_BackButtonOverview);
    if(!REPLAY_SLOTSYSTEM)
    {
        this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_PageSelection);
        this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_PageText);
    }
    this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_Nothing);
    this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_ReplayBox);
    this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_NavigatorOverview);

    this->BindObject(SURFACE_REPLAY_SLOTS, &m_Header);
    this->BindObject(SURFACE_REPLAY_SLOTS, &m_BackButtonOverview);
    this->BindObject(SURFACE_REPLAY_SLOTS, &m_ReplayBox);
    this->BindObject(SURFACE_REPLAY_SLOTS, &m_NavigatorOverview);

    this->BindObject(SURFACE_REPLAY_DETAILS, &m_StartButton);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DeleteButton);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_RenameButton);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_SaveButton);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_BackButtonDetails);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailArea);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailArea2);
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_MEDALS;  ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailMedalBack[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_ARROWS;  ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailArrow    [i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_ICONS;   ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailIconBack [i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_ICONS;   ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailIcon     [i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_MEDALS;  ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailMedal    [i]);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailStartBack);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDetailStartArrow); ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailStartArrow[i]);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailSelection);   // # before title
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailTitle);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailTitleSmall);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailTitleSmall2);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailStartLabel);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDetailScore); ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailScore[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDetailTime);  ++i) this->BindObject(SURFACE_REPLAY_DETAILS, &m_aDetailTime [i]);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DetailBox);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_NavigatorDetails);

    this->BindObject(SURFACE_REPLAY_RENAME, g_pMenu->GetArcadeInput());
    this->BindObject(SURFACE_REPLAY_RENAME, &m_RenameHeader);
}


// ****************************************************************
// move the replay menu
void cReplayMenu::Move()
{
    cArcadeInput* pArcadeInput = g_pMenu->GetArcadeInput();

    // 
    switch(this->GetCurSurface())
    {
    default: UNREACHABLE
    case SURFACE_REPLAY_OVERVIEW: m_NavigatorOverview.Update(); break;
    case SURFACE_REPLAY_SLOTS:    m_NavigatorOverview.Update(); break;
    case SURFACE_REPLAY_DETAILS:  m_NavigatorDetails .Update(); break;
    case SURFACE_REPLAY_RENAME:                                 break;
    }

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_REPLAY_OVERVIEW:
        {
            if(m_BackButtonOverview.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 2;
            }

            // 
            for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
            {
                const coreUintW iIndex = i + m_iPageOffset * MENU_REPLAY_ENTRIES;
                const coreBool  bValid = (iIndex < m_aInfoList.size()) && (m_aInfoList[iIndex].oHeader.iMagic != 0u);

                if(bValid && m_aLine[i].IsClicked())
                {
                    // 
                    this->LoadDetails(iIndex);

                    // 
                    this->__UpdateDownloaded(false);

                    // 
                    m_DetailBox.SetCurOffset(0.0f);
                    m_NavigatorDetails.ResetFirst();

                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_REPLAY_DETAILS, 3.0f, 1u);

                    // 
                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                }

                m_aName[i].SetColor3(COLOR_MENU_WHITE * (bValid ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
                m_aTime[i].SetColor3(COLOR_MENU_WHITE * (bValid ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
            }

            if(!REPLAY_SLOTSYSTEM)
            {
                if(m_iPageMax > 1u)
                {
                    const coreBool bLeft  = (!m_bPageChanged && (g_MenuInput.iMove == 2u));
                    const coreBool bRight = (!m_bPageChanged && (g_MenuInput.iMove == 4u));

                    if(!g_MenuInput.iMove && TIME) m_bPageChanged = false;

                         if((m_PageSelection.GetUserSwitch() < 0) || bLeft)  {m_bPageChanged = true; if(--m_iPageOffset >= m_iPageMax) m_iPageOffset = m_iPageMax - 1u; this->__UpdateList(true); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}
                    else if((m_PageSelection.GetUserSwitch() > 0) || bRight) {m_bPageChanged = true; if(++m_iPageOffset >= m_iPageMax) m_iPageOffset = 0u;              this->__UpdateList(true); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}
                }

                // 
                cMenu::UpdateButton(m_PageSelection.GetArrow(0u), &m_NavigatorOverview, m_PageSelection.GetArrow(0u)->IsFocused(), g_pMenu->GetButtonColor(), MENU_UPDATE_NO_SOUND);
                cMenu::UpdateButton(m_PageSelection.GetArrow(1u), &m_NavigatorOverview, m_PageSelection.GetArrow(1u)->IsFocused(), g_pMenu->GetButtonColor(), MENU_UPDATE_NO_SOUND);
            }

            // 
            cMenu::UpdateButton(&m_BackButtonOverview, &m_NavigatorOverview, m_BackButtonOverview.IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) cMenu::UpdateLine(&m_aLine[i], false);

            // 
            m_Nothing.SetEnabled(m_aLine[0].IsFocusable() ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
        }
        break;

    case SURFACE_REPLAY_SLOTS:
        {
            const auto nReturnFunc = [this](const coreInt32 iStatus, const coreBool bSuccess)
            {
                if(m_bDownloaded)
                {
                    m_bReturn = true;
                    if(bSuccess) this->__MarkSaved();
                }
                else
                {
                    m_iStatus = iStatus;
                }
            };

            if(m_bReturn)
            {
                m_bReturn = false;
                g_pMenu->ShiftSurface(this, SURFACE_REPLAY_DETAILS, 3.0f, 2u);
            }

            if(m_BackButtonOverview.IsClicked() || g_MenuInput.bCancel)
            {
                if(m_bDownloaded)
                {
                    // 
                    nReturnFunc(3, false);
                }
                else
                {
                    g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_REPLAY_SAVE"), [=](const coreInt32 iAnswer)
                    {
                        if(iAnswer != MSGBOX_ANSWER_YES)
                        {
                            // 
                            nReturnFunc(103, false);
                        }
                    });
                }
            }

            cReplayMenu::HandleSaveError([=, this](const coreUint8 iAnswer)
            {
                if(iAnswer == MSGBOX_ANSWER_YES)
                {
                    if(!g_pReplay->SaveFile(m_iOverwrite)) return;

                    // 
                    nReturnFunc(103, true);
                }

                g_pReplay->ResetStatus();
            });

            // 
            for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
            {
                ASSERT(m_iPageOffset == 0u)

                if((i < m_aInfoList.size()) && m_aLine[i].IsClicked())
                {
                    const cReplay::sInfo& oInfo = m_aInfoList[i];

                    // 
                    m_iOverwrite = i;

                    if(oInfo.oHeader.iMagic)
                    {
                        // 
                        g_pMenu->GetMsgBox()->ShowQuestion(cReplayMenu::__PrintWithName("QUESTION_REPLAY_OVERWRITE", oInfo.oHeader.acName), [=, this](const coreInt32 iAnswer1)
                        {
                            if(iAnswer1 == MSGBOX_ANSWER_YES)
                            {
                                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_SURE"), [=, this](const coreInt32 iAnswer2)
                                {
                                    if(iAnswer2 == MSGBOX_ANSWER_YES)
                                    {
                                        // 
                                        if(!m_bDownloaded)
                                        {
                                            g_pReplay->SetNameDefault(m_iOverwrite);
                                        }

                                        // 
                                        if(g_pReplay->SaveFile(m_iOverwrite))
                                        {
                                            // 
                                            nReturnFunc(103, true);

                                            // 
                                            this->__UpdateFiles();
                                            this->__UpdateList(false);
                                        }
                                    }
                                });
                            }
                        });
                    }
                    else
                    {
                        // 
                        if(!m_bDownloaded)
                        {
                            g_pReplay->SetNameDefault(m_iOverwrite);
                        }

                        // 
                        if(g_pReplay->SaveFile(m_iOverwrite))
                        {
                            // 
                            nReturnFunc(3, true);

                            // 
                            this->__UpdateFiles();
                            this->__UpdateList(false);

                            // 
                            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_MSGBOX_YES);
                        }
                    }

                    // 
                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                }

                m_aName[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_ACTIVE);
                m_aTime[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_ACTIVE);
            }

            // 
            cMenu::UpdateButton(&m_BackButtonOverview, &m_NavigatorOverview, m_BackButtonOverview.IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) cMenu::UpdateLine(&m_aLine[i], false);
        }
        break;

    case SURFACE_REPLAY_DETAILS:
        {
            if(TIME)
            {
                ASSERT(m_iSelected < m_aInfoList.size())
                const cReplay::sInfo& oInfo = m_aInfoList[m_iSelected];

                
                cReplayMenu::HandleSaveError([=, this](const coreUint8 iAnswer)
                {
                    if(iAnswer == MSGBOX_ANSWER_YES)
                    {
                        if(!g_pReplay->SaveFile(0u)) return;
                        this->__MarkSaved();
                    }

                    g_pReplay->ResetStatus();
                });
                

                if(m_StartButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_REPLAY_LOAD"), [=, this](const coreInt32 iAnswer)
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES)
                        {
                            if(m_bDownloaded || (g_pReplay->LoadFile(oInfo.sPath.c_str(), false) && g_pReplay->HasData()))
                            {
                                // 
                                m_iStatus = 101;
                            }
                            else
                            {
                                // 
                                g_pMenu->GetMsgBox()->ShowInformation(Core::Language->GetString("INFORMATION_REPLAY_CORRUPT"), MSGBOX_NO_CALLBACK);
                            }
                        }
                    });
                }
                else if(m_DeleteButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(cReplayMenu::__PrintWithName("QUESTION_REPLAY_DELETE", oInfo.oHeader.acName), [this](const coreInt32 iAnswer1)
                    {
                        if(iAnswer1 == MSGBOX_ANSWER_YES)
                        {
                            g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_SURE"), [this](const coreInt32 iAnswer2)
                            {
                                if(iAnswer2 == MSGBOX_ANSWER_YES) m_bDelete = true;
                            });
                        }
                    });
                }
                else if(m_RenameButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(cReplayMenu::__PrintWithName("QUESTION_REPLAY_RENAME", oInfo.oHeader.acName), [this](const coreInt32 iAnswer)
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES) m_bRename = true;
                    });
                }
                else if(m_SaveButton.IsClicked())
                {
                    // 
                    g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_REPLAY_SAVE"), [this](const coreInt32 iAnswer)
                    {
                        if(iAnswer == MSGBOX_ANSWER_YES)
                        {
                            if(REPLAY_SLOTSYSTEM)
                            {
                                m_bSave = true;
                            }
                            else
                            {
                                if(g_pReplay->SaveFile(0u))
                                {
                                    this->__MarkSaved();
                                }
                            }
                        }
                    });
                }
                else if(m_BackButtonDetails.IsClicked() || g_MenuInput.bCancel)
                {
                    if(m_bDownloaded)
                    {
                        // 
                        m_iStatus = 2u;
                    }
                    else
                    {
                        // 
                        this->__UpdateSelection();

                        // 
                        g_pMenu->ShiftSurface(this, SURFACE_REPLAY_OVERVIEW, 3.0f, 2u);
                    }
                }

                if(m_bDelete)
                {
                    m_bDelete = false;   // deferred to remove flickering

                    // 
                    coreData::FileDelete(oInfo.sPath.c_str());

                    // 
                    this->__UpdateFiles();
                    if(m_iPageOffset && (m_iPageOffset >= m_iPageMax)) m_iPageOffset -= 1u;
                    this->__UpdateList(true);

                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_REPLAY_OVERVIEW, 3.0f, 2u);
                }
                else if(m_bRename)
                {
                    m_bRename = false;

                    // 
                    pArcadeInput->Start("", REPLAY_NAME_INPUT);
                    pArcadeInput->OverrideText(oInfo.oHeader.acName);

                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_REPLAY_RENAME, 3.0f, 1u);
                }
                else if(m_bSave)
                {
                    m_bSave = false;

                    // 
                    this->LoadOverview();

                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_REPLAY_SLOTS, 3.0f, 1u);
                }
            }

            if(!m_bDownloaded)
            {
                // 
                     if(m_DetailSelection.GetUserSwitch() < 0) {this->SelectPrevious(); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}
                else if(m_DetailSelection.GetUserSwitch() > 0) {this->SelectNext();     g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}
            }

            // 
            cMenu::UpdateButton(&m_StartButton,       &m_NavigatorDetails, m_StartButton      .IsFocused());
            cMenu::UpdateButton(&m_DeleteButton,      &m_NavigatorDetails, m_DeleteButton     .IsFocused());
            cMenu::UpdateButton(&m_RenameButton,      &m_NavigatorDetails, m_RenameButton     .IsFocused());
            cMenu::UpdateButton(&m_SaveButton,        &m_NavigatorDetails, m_SaveButton       .IsFocused());
            cMenu::UpdateButton(&m_BackButtonDetails, &m_NavigatorDetails, m_BackButtonDetails.IsFocused());

            // 
            cMenu::UpdateSwitchBox(&m_DetailType,       MENU_UPDATE_STATIC);
            cMenu::UpdateSwitchBox(&m_DetailDifficulty, MENU_UPDATE_STATIC);
            cMenu::UpdateSwitchBox(&m_DetailSpeed,      MENU_UPDATE_STATIC);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aDetailShield[i], MENU_UPDATE_STATIC);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aDetailWeapon[i], MENU_UPDATE_STATIC);

            // 
            for(coreUintW i = 0u; i < MENU_REPLAY_DETAILS; ++i) cMenu::UpdateLine(&m_aDetailLine[i], false);
            

            
            //m_DetailSelection.GetArrow(0u)->SetOverride((m_aInfoList.size() > 1u) ? 0 : -1);
            //m_DetailSelection.GetArrow(1u)->SetOverride((m_aInfoList.size() > 1u) ? 0 : -1);
            cMenu::UpdateButton(m_DetailSelection.GetArrow(0u), &m_NavigatorDetails, m_DetailSelection.GetArrow(0u)->IsFocused(), g_pMenu->GetButtonColor(), MENU_UPDATE_NO_SOUND);
            cMenu::UpdateButton(m_DetailSelection.GetArrow(1u), &m_NavigatorDetails, m_DetailSelection.GetArrow(1u)->IsFocused(), g_pMenu->GetButtonColor(), MENU_UPDATE_NO_SOUND);
            

            const coreFloat fRotation = Core::System->GetTotalTimeFloat(20.0);

            for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS; ++i)
            {
                if(m_aDetailIcon[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    m_aDetailIcon    [i].SetDirection(coreVector2::Direction(fRotation *  (0.2f*PI) + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_GAME_ARMORY_ICONS))));
                    m_aDetailIconBack[i].SetDirection(coreVector2::Direction(fRotation * (-0.1f*PI) + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_GAME_ARMORY_ICONS))));
                }
                else
                {
                    m_aDetailIcon    [i].SetDirection(coreVector2(0.0f,1.0f));
                    m_aDetailIconBack[i].SetDirection(coreVector2(0.0f,1.0f));
                }

                m_aDetailIcon    [i].Move();
                m_aDetailIconBack[i].Move();

                m_aDetailIconBack[i].SetAlpha(m_aDetailIconBack[i].GetAlpha() * 0.5f);
            }

            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                const coreBool bValid = ((m_DetailType.GetCurValue() != GAME_TYPE_SOLO) || !i);

                m_aDetailShield[i].GetCaption()->SetColor3(bValid ? (m_aDetailShield[i].GetCurValue() ? (i ? COLOR_MENU_YELLOW : COLOR_MENU_BLUE) : COLOR_MENU_RED) : COLOR_MENU_WHITE);

                coreVector3 vColor     = COLOR_MENU_WHITE;
                coreVector2 vTexOffset = coreVector2(0.75f,0.5f);
                if(bValid)
                {
                    switch(m_aDetailWeapon[i].GetCurValue())
                    {
                    default:    UNREACHABLE
                    case 1u:    vColor = COLOR_MENU_YELLOW; vTexOffset = coreVector2(0.0f, 0.0f); break;
                    case 2u:    vColor = COLOR_MENU_PURPLE; vTexOffset = coreVector2(0.25f,0.0f); break;
                    case 3u:    vColor = COLOR_MENU_GREEN;  vTexOffset = coreVector2(0.5f, 0.0f); break;
                    case 4u:    vColor = COLOR_MENU_BLUE;   vTexOffset = coreVector2(0.75f,0.0f); break;
                    case 5u:    vColor = COLOR_MENU_RED;    vTexOffset = coreVector2(0.0f, 0.5f); break;
                    case 0xFFu: vColor = COLOR_MENU_WHITE;  vTexOffset = coreVector2(0.75f,0.5f); break;
                    }
                }

                m_aDetailWeapon[i].GetCaption()->SetColor3(vColor);

                m_aDetailWeaponIcon[i].SetColor3   (vColor);
                m_aDetailWeaponIcon[i].SetAlpha    (m_aDetailWeapon[i].GetAlpha());
                m_aDetailWeaponIcon[i].SetTexOffset(vTexOffset);
            }
            
            
            
            
            for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_ARROWS; ++i)
            {
                m_aDetailArrow[i].SetAlpha(m_aDetailArrow[i].GetAlpha() * ((i < m_iDetailStartIndex) ? 0.3f : 1.0f));
            }
            
            const coreFloat   fHeight     = SIN(Core::System->GetTotalTimeFloat(2.0*PI_D) * 10.0f) * 0.002f;
            const coreVector2 vTargetPos  = m_aDetailMedal[m_iDetailStartIndex + 1u].GetPosition() + coreVector2(0.0f,0.075f + fHeight);
            const coreVector2 vTargetPos2 = m_aDetailMedal[m_iDetailStartIndex + 1u].GetPosition() + coreVector2(0.0f,0.075f);
            m_aDetailStartArrow[0].SetPosition(vTargetPos  + coreVector2(0.0f,-0.0015f));
            m_aDetailStartArrow[1].SetPosition(vTargetPos);
            m_DetailStartLabel    .SetPosition(vTargetPos  + coreVector2(0.0f,0.025f));
            m_DetailStartBack     .SetPosition(vTargetPos2 + coreVector2(0.0f,0.025f));

            m_DetailStartBack .SetAlpha (m_DetailStartBack.GetAlpha() * 0.7f);
            m_DetailStartLabel.SetColor3(g_aMissionData[m_iDetailStartIndex].vColor);
        }
        break;

    case SURFACE_REPLAY_RENAME:
        {
            if(pArcadeInput->GetFinished())
            {
                ASSERT(m_iSelected < m_aInfoList.size())
                const cReplay::sInfo& oInfo = m_aInfoList[m_iSelected];

                // 
                const coreChar* pcText = pArcadeInput->GetText();
                if(pcText[0] && std::strcmp(pcText, oInfo.oHeader.acName))
                {
                    // 
                    cReplay::RenameReplay(oInfo.sPath.c_str(), pcText);

                    // 
                    this->__UpdateFiles();
                    this->__UpdateList(false);
                    this->LoadDetails(m_iSelected);
                }

                // 
                g_pMenu->ShiftSurface(this, SURFACE_REPLAY_DETAILS, 3.0f, 2u);
            }
        }
        break;

    default:
        UNREACHABLE
        break;
    }
}


// ****************************************************************
// 
void cReplayMenu::LoadOverview()
{
    // 
    m_iPageOffset = 0u;
    m_iPageMax    = 0u;

    // 
    this->__UpdateFiles();
    this->__UpdateList(true);
}


// ****************************************************************
// 
void cReplayMenu::LoadDetails(const coreUintW iIndex)
{
    ASSERT(iIndex < m_aInfoList.size())

    // 
    m_iSelected = iIndex;
    const cReplay::sInfo& oInfo = m_aInfoList[iIndex];

    // 
    const coreUintW iMissionIndexStart = oInfo.oHeader.iMissionIndexStart;
    const coreUintW iMissionIndexEnd   = oInfo.oHeader.iMissionIndexEnd;
    const coreUintW iNumPlayers        = oInfo.oHeader.iNumPlayers;
    const coreUintW iNumRuns           = oInfo.oHeader.iNumRuns;

    // 
    const coreUint32 iTotalScore = cReplayMenu::__GetTotalScore(oInfo.oHeader);

    // 
    coreInt32 iShift = 0;
    coreFloat fTime  = 0.0f;
    for(coreUintW j = 0u; j < REPLAY_MISSIONS; ++j)
    {
        iShift += coreInt32(oInfo.oHeader.aiShiftBadMission[j]) - coreInt32(oInfo.oHeader.aiShiftGoodMission[j]);
        fTime  += FloorFactor(TABLE_TIME_TO_FLOAT(oInfo.oHeader.aiTimeMission[j]), GAME_GOAL_FACTOR);
    }

    // 
    m_DetailTitle      .SetText(oInfo.oHeader.acName);
    m_DetailTitleSmall .SetText(oInfo.oHeader.acName);
    m_DetailTitleSmall2.SetText(oInfo.oHeader.acName);
    m_aDetailScore[1]  .SetText(PRINT("%'u",      iTotalScore));
    m_aDetailTime [1]  .SetText(PRINT("%.1f %+d", fTime, iShift));

    m_DetailTitle.RetrieveDesiredSize([this](const coreVector2 vSize)
    {
        m_DetailTitleSmall.RetrieveDesiredSize([=, this](const coreVector2 vSize2)
        {
            const coreBool bBig  = (vSize .x > m_DetailSelection.GetSize().x - m_DetailSelection.GetSize().y * 2.0f);
            const coreBool bBig2 = (vSize2.x > m_DetailSelection.GetSize().x - m_DetailSelection.GetSize().y * 2.0f);

            m_DetailTitle      .SetEnabled(bBig  ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
            m_DetailTitleSmall .SetEnabled(bBig2 ? CORE_OBJECT_ENABLE_NOTHING : bBig ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_DetailTitleSmall2.SetEnabled(bBig2 ? CORE_OBJECT_ENABLE_ALL     : CORE_OBJECT_ENABLE_NOTHING);
        });
    });

    // 
    cMenu::ApplyMedalTexture(&m_aDetailMedal[0], oInfo.oHeader.iMedalArcade, MEDAL_TYPE_ARCADE, false);

    for(coreUintW i = 1u; i < MENU_REPLAY_DETAIL_MEDALS; ++i)
    {
        const coreUint8 iMedal = oInfo.oHeader.aiMedalMission[i - 1u];

        // 
        cMenu::ApplyMedalTexture(&m_aDetailMedal[i], iMedal, MEDAL_TYPE_MISSION, (i - 1u > iMissionIndexEnd));
        m_aDetailMedalBack[i].SetEnabled(m_aDetailMedal[i].GetEnabled());

        // 
        if((i >= iMissionIndexStart + 1u) && (i <= iMissionIndexEnd + 1u))
        {
            m_aDetailMedal    [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aDetailMedalBack[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }

        // 
        m_aDetailIconBack[i - 1u].SetEnabled(iMedal ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aDetailIcon    [i - 1u].SetEnabled(iMedal ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        STATIC_ASSERT(MENU_REPLAY_DETAIL_MEDALS - 1u == MENU_REPLAY_DETAIL_ICONS)
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_DETAIL_ARROWS; ++i)
    {
        const coreBool bEnabled = (m_aDetailMedal[i + 1u].IsEnabled(CORE_OBJECT_ENABLE_ALL) && m_aDetailMedal[i + 2u].IsEnabled(CORE_OBJECT_ENABLE_ALL));

        // 
        m_aDetailArrow[i].SetEnabled(bEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        STATIC_ASSERT(MENU_REPLAY_DETAIL_ARROWS + 2u <= MENU_REPLAY_DETAIL_MEDALS)
    }

    // 
    m_DetailType      .SelectValue(oInfo.oHeader.iOptionType);
    m_DetailDifficulty.SelectValue(oInfo.oHeader.iOptionDifficulty);
    m_DetailSpeed     .SelectValue(oInfo.oHeader.iConfigGameSpeed);
    for(coreUintW i = 0u; i < MENU_REPLAY_PLAYERS; ++i)
    {
        m_aDetailShield[i].SelectValue(oInfo.oHeader.aiOptionShield [i]);
        m_aDetailWeapon[i].SelectValue(oInfo.oHeader.aaiOptionWeapon[i][0]);
    }

    const coreBool bWeaponPulse = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_PULSE);
    const coreBool bWeaponWave  = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_WAVE);
    const coreBool bWeaponTesla = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_TESLA);
    const coreBool bWeaponAnti  = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_ANTI);
    const coreBool bAnyWeapons  = bWeaponPulse || bWeaponWave || bWeaponTesla || bWeaponAnti;
    
    m_aDetailName[3].SetColor3      (COLOR_MENU_WHITE * (bAnyWeapons ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
    m_aDetailName[3].SetTextLanguage(bAnyWeapons ? "GAME_WEAPON" : "UNKNOWN");
    if(!bAnyWeapons) for(coreUintW i = 0u; i < MENU_REPLAY_PLAYERS; ++i) m_aDetailWeapon[i].SelectValue(0xFFu);

    if(!oInfo.oHeader.iOptionType)   // after overriding weapon text
    {
        m_aDetailShield[1].SelectLast();
        m_aDetailWeapon[1].SelectLast();
    }

    // 
    m_aDetailData[0].SetTextLanguage(oInfo.oHeader.iConfigBackRotation ? "VALUE_ON" : "VALUE_OFF");
    m_aDetailData[1].SetText        (PRINT("%u%%",  oInfo.oHeader.iConfigBackSpeed));
    m_aDetailData[2].SetText        (PRINT("%u Hz", oInfo.oHeader.iConfigUpdateFreq));

    // 
    coreUint32 aiScoreFull[MENU_REPLAY_DETAIL_RUNS] = {};
    for(coreUintW i = iMissionIndexStart; i <= iMissionIndexEnd; ++i)
    {
        // 
        coreUint32 aiRunScore[MENU_REPLAY_DETAIL_RUNS] = {};
        coreUint8  aiRunIndex[MENU_REPLAY_DETAIL_RUNS] = {}; std::memset(aiRunIndex, 0xFFu, sizeof(aiRunIndex));
        for(coreUintW j = 0u; j < iNumPlayers; ++j)
        {
            // 
            for(coreUintW k = 0u; k < iNumRuns; ++k)
            {
                aiRunScore[MENU_REPLAY_DETAIL_RUNS - 1u - iNumRuns + k] += oInfo.oHeader.aaaiRunMission    [j][k][i];
                aiRunIndex[MENU_REPLAY_DETAIL_RUNS - 1u - iNumRuns + k]  = oInfo.oHeader.aaiRunMissionIndex[j][k];
            }

            // 
            aiRunScore[MENU_REPLAY_DETAIL_RUNS - 1u] += oInfo.oHeader.aaiScoreMission[j][i];
            aiRunIndex[MENU_REPLAY_DETAIL_RUNS - 1u]  = iMissionIndexEnd;
        }

        // 
        for(coreUintW j = 0u; j < MENU_REPLAY_DETAIL_RUNS; ++j)
        {
            // 
            m_aaDetailRun[i - iMissionIndexStart][j].SetText((aiRunScore[j] || (aiRunIndex[j] == i)) ? coreData::ToChars(aiRunScore[j]) : "");

            // 
            aiScoreFull[j] += aiRunScore[j];
        }
    }

    constexpr coreUintW iOffset = MENU_REPLAY_MISSION_OFFSET;

    const coreUint32 iMaxValue = (*std::max_element(aiScoreFull, aiScoreFull + MENU_REPLAY_DETAIL_RUNS));

    for(coreUintW i = 0u; i < MENU_REPLAY_MISSIONS; ++i)
    {
        const coreUintW        iMissionIndex = MIN(iMissionIndexStart + i, iMissionIndexEnd);
        const coreBool         bVisible      = (i <= iMissionIndexEnd - iMissionIndexStart);
        const coreObjectEnable eEnabled      = bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

        m_aDetailName[i + iOffset].SetColor3   (g_aMissionData[iMissionIndex].vColor);
        m_aDetailName[i + iOffset].SetText     (g_aMissionData[iMissionIndex].pcName);
        m_aDetailName[i + iOffset].SetEnabled  (eEnabled);
        m_aDetailLine[i + iOffset].SetEnabled  (eEnabled);
        m_aDetailLine[i + iOffset].SetFocusable(bVisible);

        for(coreUintW j = 0u; j < MENU_REPLAY_DETAIL_RUNS; ++j)
        {
            const coreBool bBest = (aiScoreFull[j] == iMaxValue);

            m_aaDetailRun[i][j].SetColor3 (COLOR_MENU_WHITE * (bBest ? 1.0f : MENU_LIGHT_IDLE));
            m_aaDetailRun[i][j].SetEnabled(eEnabled);
        }
    }

#if defined(_CORE_SWITCH_)
    m_aDetailName[7].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
    m_aDetailLine[7].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
    m_aDetailLine[7].SetFocusable(false);
    m_aDetailData[2].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
#endif

    // 
    m_iDetailStartIndex = MIN(iMissionIndexStart, MENU_REPLAY_DETAIL_MEDALS - 2u);

    // 
    m_DetailBox.SetMaxOffset(0.05f * (2.5f + I_TO_F(iOffset - (DEFINED(_CORE_SWITCH_) ? 1u : 0u)) + I_TO_F(iMissionIndexEnd - iMissionIndexStart)) - m_DetailBox.GetSize().y);
}


// ****************************************************************
// 
void cReplayMenu::LoadDownload()
{
    ASSERT(g_pReplay->HasData())

    // 
    cReplay::sInfo oInfo;
    oInfo.sPath   = g_pReplay->GetHeader().acName;
    oInfo.iSlot   = 0xFFu;
    oInfo.oHeader = g_pReplay->GetHeader();

    // 
    m_aInfoList.clear();
    m_aInfoList.push_back(std::move(oInfo));

    // 
    m_iSelected = 0u;
    this->LoadDetails(0u);

    // 
    this->__UpdateDownloaded(true);

    // 
    m_DetailBox.SetCurOffset(0.0f);
    m_NavigatorDetails.ResetFirst();
}


// ****************************************************************
// 
void cReplayMenu::SelectPrevious()
{
    // 
    do
    {
        if(--m_iSelected >= m_aInfoList.size()) m_iSelected = m_aInfoList.size() - 1u;
    }
    while(!m_aInfoList[m_iSelected].oHeader.iMagic);

    // 
    this->LoadDetails(m_iSelected);
}


// ****************************************************************
// 
void cReplayMenu::SelectNext()
{
    // 
    do
    {
        if(++m_iSelected >= m_aInfoList.size()) m_iSelected = 0u;
    }
    while(!m_aInfoList[m_iSelected].oHeader.iMagic);

    // 
    this->LoadDetails(m_iSelected);
}


// ****************************************************************
// 
void cReplayMenu::HandleSaveError(const std::function<void(coreInt32)> nCallback)   // TODO 1: optimize
{
    // 
    const eSaveStatus eStatus = g_pReplay->GetStatus();
    if((eStatus != SAVE_STATUS_OK) && !g_pMenu->GetMsgBox()->IsVisible())
    {
        g_pMenu->GetMsgBox()->ShowQuestion(PRINT("%s (%s)", Core::Language->GetString("QUESTION_ERROR_SAVE"), Core::Language->GetString(PRINT("ERROR_SAVE_%02u", eStatus))), [=](const coreInt32 iAnswer)
        {
            nCallback(iAnswer);
        });
    }
}


// ****************************************************************
// 
void cReplayMenu::__UpdateFiles()
{
    // 
    m_aInfoList.clear();
    cReplay::LoadInfoList(&m_aInfoList);

    // 
    m_iPageMax = coreMath::CeilAlign(m_aInfoList.size(), MENU_REPLAY_ENTRIES) / MENU_REPLAY_ENTRIES;
}


// ****************************************************************
//
void cReplayMenu::__UpdateList(const coreBool bReset)
{
    
    const coreUint32 iStart   = m_iPageOffset * MENU_REPLAY_ENTRIES;
    const coreUint32 iRealNum = MIN(m_aInfoList.size() - iStart, MENU_SCORE_ENTRIES);
    
    
    // 
    for(coreUintW i = 0u, ie = iRealNum; i < ie; ++i)
    {
        const cReplay::sHeader& oHeader = m_aInfoList[i + iStart].oHeader;

        const coreUintW iLen = std::strlen(oHeader.acName);
        
        m_aName [i].SetText((iLen <= REPLAY_NAME_INPUT) ? oHeader.acName : PRINT("%.*s~", REPLAY_NAME_INPUT, oHeader.acName));
        m_aTime [i].SetText(oHeader.iMagic ? coreData::DateTimePrint("%Y-%m-%d %H:%M", TIMEMAP_LOCAL(oHeader.iViewTimestamp)) : "-");
        m_aScore[i].SetText(oHeader.iMagic ? PRINT("%'u", cReplayMenu::__GetTotalScore(oHeader))                              : "");
    }

    // 
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        const coreBool         bVisible = (i + iStart < m_aInfoList.size());
        const coreObjectEnable eEnabled = bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

        m_aName [i].SetEnabled  (eEnabled);
        m_aTime [i].SetEnabled  (eEnabled);
        m_aScore[i].SetEnabled  (eEnabled);
        m_aLine [i].SetEnabled  (eEnabled);
        m_aLine [i].SetFocusable(bVisible);
    }

    if(bReset)
    {
        // 
        m_ReplayBox.SetCurOffset(0.0f);

        // 
        if(!m_aLine[0].IsEnabled(CORE_OBJECT_ENABLE_ALL))
        {
            m_NavigatorOverview.OverrideCurrent(&m_PageSelection);
        }
        else if(m_NavigatorOverview.GetCurScroll() == &m_ReplayBox)
        {
            m_NavigatorOverview.OverrideCurrent(&m_aLine[0]);
        }
        else if(this->GetCurSurface() != SURFACE_REPLAY_OVERVIEW)
        {
            m_NavigatorOverview.ForceCurrent(&m_aLine[0]);
        }
    }

    // 
    m_ReplayBox.SetMaxOffset(0.05f * I_TO_F(iRealNum) - m_ReplayBox.GetSize().y - CORE_MATH_PRECISION);
    m_ReplayBox.Move();

    // 
    m_PageText.SetText(m_aInfoList.empty() ? "-" : PRINT("%u-%u / %zu", iStart + MIN(iRealNum, 1u), iStart + iRealNum, m_aInfoList.size()));
}



// ****************************************************************
// 
void cReplayMenu::__UpdateSelection()
{
    const coreUint32 iNewPageOffset = coreMath::FloorAlign(m_iSelected, MENU_REPLAY_ENTRIES) / MENU_REPLAY_ENTRIES;

    // 
    if(m_iPageOffset != iNewPageOffset)
    {
        m_iPageOffset = iNewPageOffset;
        this->__UpdateList(false);
    }

    // 
    m_ReplayBox.ScrollToObject(&m_aLine[m_iSelected % MENU_REPLAY_ENTRIES], true);

    // 
    m_NavigatorOverview.ForceCurrent(&m_aLine[m_iSelected % MENU_REPLAY_ENTRIES]);
}


// ****************************************************************
// 
void cReplayMenu::__UpdateDownloaded(const coreBool bDownloaded)
{
    // 
    m_bDownloaded = bDownloaded;

    // 
    if(bDownloaded)
    {
        m_DeleteButton   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_RenameButton   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_SaveButton     .SetEnabled(CORE_OBJECT_ENABLE_ALL);
        m_DetailSelection.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

        m_DeleteButton.SetFocusable(false);
        m_RenameButton.SetFocusable(false);
        m_SaveButton  .SetFocusable(true);

        m_NavigatorDetails.SetShoulder(false);
    }
    else
    {
        m_DeleteButton   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
        m_RenameButton   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
        m_SaveButton     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_DetailSelection.SetEnabled(CORE_OBJECT_ENABLE_ALL);

        m_DeleteButton.SetFocusable(true);
        m_RenameButton.SetFocusable(true);
        m_SaveButton  .SetFocusable(false);

        m_NavigatorDetails.SetShoulder(true);
    }

    // 
    m_SaveButton.SetOverride(0);
    m_SaveButton.GetCaption()->SetText(ICON_FLOPPY_DISK);
}


// ****************************************************************
// 
void cReplayMenu::__MarkSaved()
{
    // 
    m_SaveButton.SetOverride(-1);
    m_SaveButton.GetCaption()->SetText(ICON_CHECK);
}


// ****************************************************************
// 
coreUint32 cReplayMenu::__GetTotalScore(const cReplay::sHeader& oHeader)
{
    // 
    coreUint32 aiScore[REPLAY_RUNS + 1u] = {};
    for(coreUintW j = 0u; j < REPLAY_PLAYERS; ++j)
    {
        for(coreUintW i = 0u; i < REPLAY_RUNS + 1u; ++i)
        {
            const coreUint32 iScore = (i < TABLE_RUNS) ? oHeader.aaiRunTotal[j][i] : oHeader.aiScoreTotal[j];

            aiScore[i] += iScore;
        }
    }

    // 
    const coreUintW iMaxIndex = std::max_element(aiScore, aiScore + REPLAY_RUNS + 1u) - aiScore;
    return aiScore[iMaxIndex];
}


// ****************************************************************
// 
const coreChar* cReplayMenu::__PrintWithName(const coreHashString sLanguageKey, const coreChar* pcName)
{
    static coreString s_sString;

    // 
    s_sString = Core::Language->GetString(sLanguageKey);
    s_sString.replace("{NAME}", pcName);

    return s_sString.c_str();
}