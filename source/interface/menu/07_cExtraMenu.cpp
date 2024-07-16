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
cExtraMenu::cExtraMenu()noexcept
: coreMenu         (SURFACE_EXTRA_MAX, SURFACE_EXTRA_TROPHY)
, m_iCurIndex      (0u)
, m_Stats          {}
, m_iTrophyMission (0u)
, m_iTrophySegment (0u)
{
    constexpr coreFloat fSplit  = 1.0f/300.0f;   // 0.00333...
    constexpr coreFloat fSplit2 = 1.0f/60.0f;    // 0.01666...

    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.01f));
    m_Background.SetSize      (coreVector2(0.9f,0.75f));

    m_TrophyTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_TrophyTab.DefineProgram("menu_border_program");
    m_TrophyTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.15f - fSplit + fSplit2, -0.0125f));
    m_TrophyTab.SetSize      (coreVector2(0.23f + fSplit + fSplit2*2.0f, 0.07f));
    m_TrophyTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_TrophyTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_TrophyTab.SetTexOffset (m_TrophyTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.15f - fSplit + fSplit2, 0.0125f));
    m_TrophyTab.GetCaption()->SetTextLanguage("EXTRA_TROPHY");

    m_StatsTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StatsTab.DefineProgram("menu_border_program");
    m_StatsTab.SetPosition  (m_TrophyTab.GetPosition() * coreVector2(0.0f,1.0f));
    m_StatsTab.SetSize      (m_TrophyTab.GetSize());
    m_StatsTab.SetAlignment (m_TrophyTab.GetAlignment());
    m_StatsTab.SetTexSize   (m_TrophyTab.GetTexSize());
    m_StatsTab.SetTexOffset (m_TrophyTab.GetTexOffset() + coreVector2(m_StatsTab.GetPosition().x - m_TrophyTab.GetPosition().x, 0.0f));
    m_StatsTab.GetCaption()->SetTextLanguage("EXTRA_STATS");

    m_OtherTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_OtherTab.DefineProgram("menu_border_program");
    m_OtherTab.SetPosition  (m_TrophyTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_OtherTab.SetSize      (m_TrophyTab.GetSize());
    m_OtherTab.SetAlignment (m_TrophyTab.GetAlignment());
    m_OtherTab.SetTexSize   (m_TrophyTab.GetTexSize());
    m_OtherTab.SetTexOffset (m_TrophyTab.GetTexOffset() + coreVector2(m_OtherTab.GetPosition().x - m_TrophyTab.GetPosition().x, 0.0f));
    m_OtherTab.GetCaption()->SetTextLanguage("EXTRA_OTHER");

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f,0.07f));
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    m_SummaryName.Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_SummaryName.SetPosition    (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f,-0.05f));
    m_SummaryName.SetAlignment   (coreVector2(1.0f,0.0f));
    m_SummaryName.SetColor3      (COLOR_MENU_WHITE);
    m_SummaryName.SetTextLanguage("EXTRA_PROGRESS");

    m_SummaryValue.Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_SummaryValue.SetPosition (coreVector2(-1.0f,1.0f) * m_SummaryName.GetPosition() + coreVector2(-0.04f,0.0f));
    m_SummaryValue.SetAlignment(coreVector2(-1.0f,0.0f));
    m_SummaryValue.SetColor3   (COLOR_MENU_WHITE);

    m_SummaryLine.DefineTexture(0u, "menu_detail_04.png");
    m_SummaryLine.DefineTexture(1u, "menu_background_black.png");
    m_SummaryLine.DefineProgram("menu_inner_program");
    m_SummaryLine.SetPosition  (coreVector2(0.0f, m_SummaryName.GetPosition().y));
    m_SummaryLine.SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
    m_SummaryLine.SetTexOffset (coreVector2(-0.09f,0.0f));

    m_CueTrophy.Construct   (MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_CueTrophy.SetPosition (coreVector2(-1.0f,1.0f) * m_SummaryName.GetPosition());
    m_CueTrophy.SetAlignment(coreVector2(-1.0f,0.0f));
    m_CueTrophy.SetColor3   (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    m_CueTrophy.SetText     (ICON_TROPHY);

    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i)
    {
        const coreFloat fHeight = m_Background.GetPosition().y + m_Background.GetSize().y*0.5f + -0.175f - 0.15f*I_TO_F(i);

        m_aTrophyName[i].Construct   (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        //m_aTrophyName[i].SetPosition (coreVector2(m_Background.GetPosition().x - m_Background.GetSize().x*0.5f, fHeight) + coreVector2(0.145f,0.032f));
        m_aTrophyName[i].SetAlignment(coreVector2(1.0f,0.0f));

        for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaTrophyDesc[0]); ++j)
        {
            m_aaTrophyDesc[i][j].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
            //m_aaTrophyDesc[i][j].SetPosition (coreVector2(m_Background.GetPosition().x - m_Background.GetSize().x*0.5f, fHeight) + coreVector2(0.145f, -0.003f - 0.03f*I_TO_F(j)));
            m_aaTrophyDesc[i][j].SetAlignment(coreVector2(1.0f,0.0f));
            m_aaTrophyDesc[i][j].SetColor3   (COLOR_MENU_WHITE);
        }

        m_aTrophyCheck[i].Construct  (MENU_FONT_ICON_4, MENU_OUTLINE_SMALL);
        m_aTrophyCheck[i].SetPosition(coreVector2(m_Background.GetPosition().x + m_Background.GetSize().x*0.5f, fHeight) + coreVector2(-0.15f,0.0f));
        m_aTrophyCheck[i].SetText    (ICON_CHECK);

        m_aTrophyArrow[i].Construct  (MENU_SWITCHBOX, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
        m_aTrophyArrow[i].SetPosition(coreVector2(m_Background.GetPosition().x + m_Background.GetSize().x*0.5f, fHeight) + coreVector2(-0.07f,0.0f));
        m_aTrophyArrow[i].SetSize    (coreVector2(0.06f,0.03f));
        m_aTrophyArrow[i].GetCaption()->SetText(">");

        m_aTrophyTile[i].DefineProgram("menu_segment_program");
        m_aTrophyTile[i].SetPosition  (coreVector2(m_Background.GetPosition().x - m_Background.GetSize().x*0.5f, fHeight) + coreVector2(0.08f,0.0f));
        m_aTrophyTile[i].SetSize      (coreVector2(0.05f,0.05f) * 1.4f);

        m_aTrophyBack[i].DefineProgram("menu_color_program");
        m_aTrophyBack[i].SetPosition  (m_aTrophyTile[i].GetPosition());
        m_aTrophyBack[i].SetSize      (m_aTrophyTile[i].GetSize() + coreVector2(0.01f,0.01f));
        m_aTrophyBack[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.15f);
        m_aTrophyBack[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);

        m_aTrophyLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aTrophyLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aTrophyLine[i].DefineProgram("menu_inner_program");
        m_aTrophyLine[i].SetPosition  (coreVector2(0.0f, fHeight));
        m_aTrophyLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.15f));
        m_aTrophyLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }

    m_TrophyBox.SetPosition(m_Background.GetPosition() + coreVector2(0.0f,-0.0375f));
    m_TrophyBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.625f));
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) m_TrophyBox.BindObject(&m_aTrophyLine [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) m_TrophyBox.BindObject(&m_aTrophyBack [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) m_TrophyBox.BindObject(&m_aTrophyTile [i]);   // wave and boss
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) m_TrophyBox.BindObject(&m_aTrophyArrow[i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) m_TrophyBox.BindObject(&m_aTrophyCheck[i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) m_TrophyBox.BindObject(&m_aTrophyName [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaTrophyDesc[0]); ++j) m_TrophyBox.BindObject(&m_aaTrophyDesc[i][j]);

    for(coreUintW i = 0u; i < MENU_EXTRA_FILTERS; ++i)
    {
        m_aFilterLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aFilterLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aFilterLine[i].DefineProgram("menu_inner_program");
        m_aFilterLine[i].SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f, -0.05f - 0.05f*I_TO_F(i)));
        m_aFilterLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aFilterLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aFilterLine[i].SetFocusable (true);
    }

    m_FilterMission.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FilterMission.SetPosition(m_aFilterLine[0].GetPosition());
    m_FilterMission.SetSize    (coreVector2(0.47f,0.03f));
    m_FilterMission.SetEndless (true);
    m_FilterMission.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FilterSegment.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FilterSegment.SetPosition(m_aFilterLine[1].GetPosition());
    m_FilterSegment.SetSize    (m_FilterMission.GetSize());
    m_FilterSegment.SetEndless (true);
    m_FilterSegment.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FilterType.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FilterType.SetPosition(m_aFilterLine[2].GetPosition() + coreVector2(-0.125f,0.0f));
    m_FilterType.SetSize    (coreVector2(0.22f,0.03f));
    m_FilterType.SetEndless (true);
    m_FilterType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FilterDifficulty.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FilterDifficulty.SetPosition(m_aFilterLine[2].GetPosition() + coreVector2(0.125f,0.0f));
    m_FilterDifficulty.SetSize    (m_FilterType.GetSize());
    m_FilterDifficulty.SetEndless (true);
    m_FilterDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FilterIcon.DefineTexture(0u, "menu_helper.png");
    m_FilterIcon.DefineProgram("menu_helper_program");
    m_FilterIcon.SetPosition  (m_FilterMission.GetPosition());
    m_FilterIcon.SetSize      (coreVector2(0.1f,0.1f));
    m_FilterIcon.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_FilterIcon.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i)
    {
        m_aStatsName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aStatsName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.05f*I_TO_F(i + MENU_EXTRA_FILTERS)));
        m_aStatsName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aStatsName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aStatsValue[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aStatsValue[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aStatsName[i].GetPosition());
        m_aStatsValue[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aStatsValue[i].SetColor3   (COLOR_MENU_WHITE);

        m_aStatsLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aStatsLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aStatsLine[i].DefineProgram("menu_inner_program");
        m_aStatsLine[i].SetPosition  (coreVector2(0.0f, m_aStatsName[i].GetPosition().y));
        m_aStatsLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aStatsLine[i].SetTexOffset (coreVector2(I_TO_F(i + MENU_EXTRA_FILTERS)*0.09f, 0.0f));
        m_aStatsLine[i].SetFocusable (true);
    }

    m_StatsBox.SetPosition(m_Background.GetPosition() + coreVector2(0.0f,-0.0875f));
    m_StatsBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.525f));
    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i) m_StatsBox.BindObject(&m_aStatsLine [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i) m_StatsBox.BindObject(&m_aStatsName [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i) m_StatsBox.BindObject(&m_aStatsValue[i]);

    for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS; ++i)
    {
        m_aOtherName[i].Construct   (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aOtherName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.1f*I_TO_F(i)));
        m_aOtherName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aOtherName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aOtherLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aOtherLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aOtherLine[i].DefineProgram("menu_inner_program");
        m_aOtherLine[i].SetPosition  (coreVector2(0.0f, m_aOtherName[i].GetPosition().y));
        m_aOtherLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.1f));
        m_aOtherLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aOtherLine[i].SetFocusable (true);
    }
    m_aOtherName[0].SetTextLanguage("EXTRA_PASSWORD");
    m_aOtherName[1].SetTextLanguage("EXTRA_CREDITS");

    m_Password.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Password.SetPosition (coreVector2(-1.00f,1.00f) * m_aOtherName[0].GetPosition());
    m_Password.SetSize     (coreVector2( 0.26f,0.05f));
    m_Password.SetAlignment(coreVector2(-1.00f,0.00f));
    m_Password.GetCaption()->SetTextLanguage("ENTER");

    m_Credits.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Credits.SetPosition (coreVector2(-1.00f,1.00f) * m_aOtherName[1].GetPosition());
    m_Credits.SetSize     (m_Password.GetSize());
    m_Credits.SetAlignment(m_Password.GetAlignment());
    m_Credits.GetCaption()->SetTextLanguage("WATCH");

    m_Music.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Music.SetPosition (coreVector2(-1.00f,1.00f) * m_aOtherName[2].GetPosition());
    m_Music.SetSize     (m_Password.GetSize());
    m_Music.SetAlignment(m_Password.GetAlignment());

    m_PasswordHeader.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_PasswordHeader.SetPosition    (coreVector2(0.0f,0.35f));
    m_PasswordHeader.SetColor3      (COLOR_MENU_WHITE);
    m_PasswordHeader.SetTextLanguage("ENTER_PASSWORD");

    m_OtherNew.SetPosition(m_OtherTab.GetPosition() + m_OtherTab.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,0.06f));
    m_OtherNew.SetIndex   (NEW_EXTRA_OTHER);

    m_MusicBoxNew.SetPosition(m_Music.GetPosition() + coreVector2(0.045f,0.0f));
    m_MusicBoxNew.SetIndex   (NEW_EXTRA_MUSICBOX);

    // 
    m_FilterSegment   .AddEntry        ("",                       255u);   // dummy
    m_FilterType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_FilterType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_FilterType      .AddEntryLanguage("FILTER_ALL",             255u);
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    m_FilterDifficulty.AddEntryLanguage("FILTER_ALL",             255u);

    // 
    m_Navigator.BindObject(&m_TrophyTab, &m_BackButton, &m_OtherTab,  &m_FilterMission, &m_StatsTab,  MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_StatsTab,  &m_BackButton, &m_TrophyTab, &m_FilterMission, &m_OtherTab,  MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_OtherTab,  &m_BackButton, &m_StatsTab,  &m_FilterMission, &m_TrophyTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);

    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i)
    {
        m_Navigator.BindObject(&m_aTrophyLine[i], (i == 0u)                       ? s_cast<coreObject2D*>(&m_TrophyTab)  : &m_aTrophyLine[(i + MENU_EXTRA_TROPHIES - 1u) % MENU_EXTRA_TROPHIES], NULL,
                                                  (i == MENU_EXTRA_TROPHIES - 1u) ? s_cast<coreObject2D*>(&m_BackButton) : &m_aTrophyLine[(i + 1u)                       % MENU_EXTRA_TROPHIES], NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_TROPHY);
    }

    m_Navigator.BindObject(&m_FilterMission,    &m_StatsTab,       NULL,                &m_FilterSegment, NULL,                MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_EXTRA_STATS);
    m_Navigator.BindObject(&m_FilterSegment,    &m_FilterMission,  NULL,                &m_FilterType,    NULL,                MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_EXTRA_STATS);
    m_Navigator.BindObject(&m_FilterType,       &m_FilterSegment,  &m_FilterDifficulty, &m_aStatsLine[0], &m_FilterDifficulty, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS,                         SURFACE_EXTRA_STATS);
    m_Navigator.BindObject(&m_FilterDifficulty, &m_FilterSegment,  &m_FilterType,       &m_aStatsLine[0], &m_FilterType,       MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS,                         SURFACE_EXTRA_STATS);

    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i)
    {
        m_Navigator.BindObject(&m_aStatsLine[i], (i == 0u)                    ? s_cast<coreObject2D*>(&m_FilterType) : &m_aStatsLine[(i + MENU_EXTRA_STATS - 1u) % MENU_EXTRA_STATS], NULL,
                                                 (i == MENU_EXTRA_STATS - 1u) ? s_cast<coreObject2D*>(&m_BackButton) : &m_aStatsLine[(i + 1u)                    % MENU_EXTRA_STATS], NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_STATS);
    }

    m_Navigator.BindObject(&m_Password, &m_OtherTab, NULL, &m_Credits,    NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_OTHER);
    m_Navigator.BindObject(&m_Credits,  &m_Password, NULL, &m_Music,      NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_OTHER);
    m_Navigator.BindObject(&m_Music,    &m_Credits,  NULL, &m_BackButton, NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_OTHER);

    m_Navigator.BindObject(&m_BackButton, &m_aStatsLine[MENU_EXTRA_STATS - 1u], NULL, NULL, NULL, MENU_TYPE_DEFAULT);

    m_Navigator.BindSurface(&m_TrophyTab, SURFACE_EXTRA_TROPHY, &m_aTrophyLine[MENU_EXTRA_TROPHIES - 1u], NULL, &m_aTrophyLine[0], NULL);
    m_Navigator.BindSurface(&m_StatsTab,  SURFACE_EXTRA_STATS,  &m_aStatsLine [MENU_EXTRA_STATS    - 1u], NULL, &m_FilterMission,  NULL);
    m_Navigator.BindSurface(&m_OtherTab,  SURFACE_EXTRA_OTHER,  &m_Music,                                 NULL, &m_Password,       NULL);

    m_Navigator.BindScroll(&m_TrophyBox);
    m_Navigator.BindScroll(&m_StatsBox);

    m_Navigator.AssignFirst(&m_FilterMission);
    m_Navigator.AssignBack (&m_BackButton);
    m_Navigator.AssignMenu (this);

    const auto nTabFunc = [this](coreObject2D* pObject, const coreUint8 iPack)
    {
        if(iPack != 4u) return;
        switch(this->GetCurSurface())
        {
        default: UNREACHABLE
        case SURFACE_EXTRA_TROPHY: m_Navigator.RebindDown(pObject, &m_TrophyTab); break;
        case SURFACE_EXTRA_STATS:  m_Navigator.RebindDown(pObject, &m_StatsTab);  break;
        case SURFACE_EXTRA_OTHER:  m_Navigator.RebindDown(pObject, &m_OtherTab);  break;
        }
    };
    m_Navigator.BindDynamic(&m_BackButton, nTabFunc);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_EXTRA_PASSWORD; ++i)
    {
        if(i != SURFACE_EXTRA_TROPHY) this->BindObject(i, &m_TrophyTab);
        if(i != SURFACE_EXTRA_STATS)  this->BindObject(i, &m_StatsTab);
        if(i != SURFACE_EXTRA_OTHER)  this->BindObject(i, &m_OtherTab);

        this->BindObject(i, &m_Background);

        if(i == SURFACE_EXTRA_TROPHY) this->BindObject(i, &m_TrophyTab);
        if(i == SURFACE_EXTRA_STATS)  this->BindObject(i, &m_StatsTab);
        if(i == SURFACE_EXTRA_OTHER)  this->BindObject(i, &m_OtherTab);

        this->BindObject(i, &m_BackButton);

        this->BindObject(i, &m_OtherNew);
    }

    this->BindObject(SURFACE_EXTRA_TROPHY, &m_SummaryLine);
    this->BindObject(SURFACE_EXTRA_TROPHY, &m_SummaryName);
    this->BindObject(SURFACE_EXTRA_TROPHY, &m_SummaryValue);
    this->BindObject(SURFACE_EXTRA_TROPHY, &m_CueTrophy);
    this->BindObject(SURFACE_EXTRA_TROPHY, &m_TrophyBox);

    for(coreUintW i = 0u; i < MENU_EXTRA_FILTERS; ++i) this->BindObject(SURFACE_EXTRA_STATS, &m_aFilterLine[i]);
    this->BindObject(SURFACE_EXTRA_STATS, &m_FilterIcon);
    this->BindObject(SURFACE_EXTRA_STATS, &m_StatsBox);
    this->BindObject(SURFACE_EXTRA_STATS, &m_FilterMission);
    this->BindObject(SURFACE_EXTRA_STATS, &m_FilterSegment);
    this->BindObject(SURFACE_EXTRA_STATS, &m_FilterType);
    this->BindObject(SURFACE_EXTRA_STATS, &m_FilterDifficulty);

    for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS; ++i) this->BindObject(SURFACE_EXTRA_OTHER, &m_aOtherLine[i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS; ++i) this->BindObject(SURFACE_EXTRA_OTHER, &m_aOtherName[i]);

    this->BindObject(SURFACE_EXTRA_OTHER, &m_Password);
    this->BindObject(SURFACE_EXTRA_OTHER, &m_Credits);
    this->BindObject(SURFACE_EXTRA_OTHER, &m_Music);
    this->BindObject(SURFACE_EXTRA_OTHER, &m_MusicBoxNew);

    for(coreUintW i = 0u; i < SURFACE_EXTRA_PASSWORD; ++i) this->BindObject(i, &m_Navigator);

    this->BindObject(SURFACE_EXTRA_PASSWORD, g_pMenu->GetArcadeInput());
    this->BindObject(SURFACE_EXTRA_PASSWORD, &m_PasswordHeader);

    this->BindObject(SURFACE_EXTRA_CREDITS, g_pMenu->GetCreditRoll());

    this->BindObject(SURFACE_EXTRA_MUSIC, g_pMenu->GetMusicBox());
}


// ****************************************************************
// move the extra menu
void cExtraMenu::Move()
{
    cArcadeInput* pArcadeInput = g_pMenu->GetArcadeInput();
    cCreditRoll*  pCreditRoll  = g_pMenu->GetCreditRoll ();
    cMusicBox*    pMusicBox    = g_pMenu->GetMusicBox   ();

    // 
    if(this->GetCurSurface() < SURFACE_EXTRA_PASSWORD) m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
         if(m_TrophyTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_TROPHY);
    else if(m_StatsTab .IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_STATS);
    else if(m_OtherTab .IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_OTHER);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EXTRA_TROPHY:
        {
            for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i)
            {
                if(m_aTrophyLine[i].IsClicked() || m_aTrophyArrow[i].IsClicked())
                {
                    // 
                    m_iStatus = 1;

                    // 
                    m_iTrophyMission = i / 6u;
                    m_iTrophySegment = i % 6u;

                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);

                    break;
                }
            }

            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 2;
            }

            // 
            for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) cMenu::UpdateLine(&m_aTrophyLine[i], false, false, coreVector3(1.0f,1.0f,1.0f), false);//cMenuNavigator::IsUsingAny());

            // 
            for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i) cMenu::UpdateButton(&m_aTrophyArrow[i], this, m_aTrophyLine[i].IsFocused() || m_aTrophyArrow[i].IsFocused(), m_aTrophyTile[i].GetColor3(), true, false);
        }
        break;

    case SURFACE_EXTRA_STATS:
        {
            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 2;
            }

            // 
            if(m_FilterMission.GetUserSwitch())
                this->LoadSegments(m_FilterMission.GetCurValue());

            // 
            if(m_FilterMission.GetUserSwitch() || m_FilterSegment.GetUserSwitch() || m_FilterType.GetUserSwitch() || m_FilterDifficulty.GetUserSwitch())
                this->__UpdateStats();

            // 
            cMenu::UpdateSwitchBox(&m_FilterMission);
            cMenu::UpdateSwitchBox(&m_FilterSegment);
            cMenu::UpdateSwitchBox(&m_FilterType);
            cMenu::UpdateSwitchBox(&m_FilterDifficulty);

            // 
            for(coreUintW i = 0u; i < MENU_EXTRA_FILTERS; ++i) cMenu::UpdateLine(&m_aFilterLine[i], true);
            for(coreUintW i = 0u; i < MENU_EXTRA_STATS;   ++i) cMenu::UpdateLine(&m_aStatsLine [i], false);

            // 
            m_FilterMission.GetCaption()->SetColor3((m_FilterMission.GetCurValue() < ARRAY_SIZE(g_aMissionData)) ? g_aMissionData[m_FilterMission.GetCurValue()].vColor : COLOR_MENU_WHITE);

            // 
            m_FilterIcon.SetAlpha(m_FilterIcon.GetAlpha() * 0.6f);
        }
        break;

    case SURFACE_EXTRA_OTHER:
        {
            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 2;
            }
            else if(m_Password.IsClicked())
            {
                // 
                pArcadeInput->Start("", 16u);

                // 
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_PASSWORD, 3.0f, 1u);
            }
            else if(m_Credits.IsClicked())
            {
                // 
                pCreditRoll->Start(CREDIT_TYPE_MENU);

                // 
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_CREDITS, 3.0f, 1u);
            }
            else if(m_Music.IsClicked())
            {
                // 
                pMusicBox->ResetNavigator();

                // 
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_MUSIC, 3.0f, 1u);

                // 
                m_OtherNew   .Resolve();
                m_MusicBoxNew.Resolve();
            }

            // 
            cMenu::UpdateButton(&m_Password, &m_Navigator, m_Password.IsFocused());
            cMenu::UpdateButton(&m_Credits,  &m_Navigator, m_Credits .IsFocused());
            cMenu::UpdateButton(&m_Music,    &m_Navigator, m_Music   .IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS; ++i) cMenu::UpdateLine(&m_aOtherLine[i], true, true, g_pMenu->GetButtonColor(), !cMenuNavigator::IsUsingAny());
        }
        break;

    case SURFACE_EXTRA_PASSWORD:
        {
            if(pArcadeInput->GetFinished())
            {
                // 
                const coreChar* pcText = pArcadeInput->GetText();
                if(pcText[0])
                {
                    // 
                    const coreBool bSuccess = ApplyPassword(pcText);

                    // 
                         if(!std::strcmp(pcText, "PASSWORD")) pArcadeInput->OverrideText("NICE-TRY");
                    else if(!std::strcmp(pcText, "PASSWORT")) pArcadeInput->OverrideText("SO-NICHT");
                    else
                    {
                        // 
                        pArcadeInput->Clear();
                    }

                    if(bSuccess)
                    {
                        // 
                        pArcadeInput->Flash();
                        this->LoadMissions();
                    }

                    // 
                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, bSuccess ? SOUND_MENU_MSGBOX_YES : SOUND_MENU_MSGBOX_NO);
                }
                else
                {
                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_EXTRA_OTHER, 3.0f, 2u);
                }
            }
        }
        break;

    case SURFACE_EXTRA_CREDITS:
        {
            if(pCreditRoll->GetFinished() || g_MenuInput.bCancel)
            {
                // 
                pCreditRoll->End();

                // 
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_OTHER, 3.0f, 2u);
            }
        }
        break;

    case SURFACE_EXTRA_MUSIC:
        {
            if(pMusicBox->GetFinished() || g_MenuInput.bCancel)
            {
                // 
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_OTHER, 3.0f, 2u);
            }
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    // 
    cMenu::UpdateTab(&m_TrophyTab, (this->GetCurSurface() == SURFACE_EXTRA_TROPHY), m_TrophyTab.IsFocused());
    cMenu::UpdateTab(&m_StatsTab,  (this->GetCurSurface() == SURFACE_EXTRA_STATS),  m_StatsTab .IsFocused());
    cMenu::UpdateTab(&m_OtherTab,  (this->GetCurSurface() >= SURFACE_EXTRA_OTHER),  m_OtherTab .IsFocused());   // # all

    // 
    cMenu::UpdateButton(&m_BackButton, &m_Navigator, m_BackButton.IsFocused());
}


// ****************************************************************
// 
void cExtraMenu::LoadTrophies()
{
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    coreUintW iMax = 0u;
    for(coreUintW i = 0u; i < 8u; ++i)
    {
        if(oProgress.aiAdvance[i]) iMax = i * 6u + oProgress.aiAdvance[i];
    }

    coreUintW iAchieved = 0u;

    coreUintW iCountBoss = 0u;
    coreUintW iCountWave = 0u;
    for(coreUintW i = 0u; i < MENU_EXTRA_TROPHIES; ++i)
    {
        if(i < iMax)
        {
            m_aTrophyName [i]   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aaTrophyDesc[i][0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aaTrophyDesc[i][1].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aTrophyArrow[i]   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aTrophyTile [i]   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aTrophyBack [i]   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aTrophyLine [i]   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aTrophyLine [i]   .SetFocusable(true);

            const coreUintW iMissionIndex = i / 6u;
            const coreUintW iSegmentIndex = i % 6u;
    
            const coreBool bAchieved = HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 3u);
            if(bAchieved) iAchieved += 1u;
            
            m_aTrophyName[i].SetColor3(bAchieved ? g_aMissionData[iMissionIndex].vColor : (COLOR_MENU_WHITE * MENU_LIGHT_IDLE));
            m_aTrophyName[i].SetText(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    
            m_aTrophyCheck[i].SetEnabled(bAchieved ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aTrophyCheck[i].SetColor3(bAchieved ? g_aMissionData[iMissionIndex].vColor : COLOR_MENU_WHITE);
    
            m_aTrophyTile[i].SetColor3(bAchieved ? g_aMissionData[iMissionIndex].vColor  : COLOR_MENU_WHITE);
            m_aTrophyLine[i].SetColor3(bAchieved ? g_aMissionData[iMissionIndex].vColor2 : COLOR_MENU_WHITE);
            
            
            const coreFloat fShift = (i == 17u) ? 0.5f : (i == 23u) ? 3.2f : (i == 41u) ? 1.0f : 2.0f;

            const coreBool    bBoss      = ((i % 6u) == 5u);
            const coreVector2 vStep      = bBoss ? coreVector2((1.0f/37.0f), (1.0f/21.0f) * (581.0f/512.0f)) : coreVector2((1.0f/41.0f), (1.0f/41.0f));
            const coreVector2 vTexSize   = bBoss ? coreVector2(4.0f,4.0f)                                    : coreVector2(4.0f,4.0f);
            const coreVector2 vTexOffset = bBoss ? coreVector2(I_TO_F(iCountBoss % 4u) * 9.0f + 1.0f + fShift, I_TO_F(iCountBoss / 4u) * 5.0f + 1.0f) :
                                                   coreVector2(I_TO_F(iCountWave % 8u) * 5.0f + 1.0f,          I_TO_F(iCountWave / 8u) * 5.0f + 1.0f);
    
            m_aTrophyTile[i].DefineTexture(0u, bBoss ? "menu_segment_boss.png" : "menu_segment_wave.png");
            m_aTrophyTile[i].SetTexSize   (vTexSize   * vStep);
            m_aTrophyTile[i].SetTexOffset (vTexOffset * vStep);
            
            if((i == iMax - 1u) && !bAchieved)
            {
                const coreVector2 vStep2      = coreVector2((1.0f/41.0f), (1.0f/41.0f));
                const coreVector2 vTexOffset2 = coreVector2(I_TO_F(62u % 8u) * 5.0f + 1.0f, I_TO_F(62u / 8u) * 5.0f + 1.0f);

                m_aTrophyTile[i].DefineTexture(0u, "menu_segment_wave.png");
                m_aTrophyTile[i].SetTexSize   (vTexSize    * vStep2);
                m_aTrophyTile[i].SetTexOffset (vTexOffset2 * vStep2);

                m_aaTrophyDesc[i][0].SetTextLanguage("UNKNOWN");
                m_aaTrophyDesc[i][1].SetText("");
            }
            else
            {
                constexpr coreUint8 aiSwitch[] = {0u};

                const coreBool bHasSwitch = DEFINED(_CORE_SWITCH_) && std::memchr(aiSwitch, i, ARRAY_SIZE(aiSwitch));
                ASSERT(i <= 0xFFu)

                const coreChar* pcString = Core::Language->GetString(PRINT("ACHIEVEMENT_STAGE_%02d_%02zu_DESC%s", g_aMissionData[i / 6u].iID, (i % 6u) + 1u, bHasSwitch ? "_SWITCH" : ""));
                const coreChar* pcBreak  = std::strchr(pcString, '#');

                if(pcBreak)
                {
                    m_aaTrophyDesc[i][0].SetText(pcString, pcBreak - pcString);
                    m_aaTrophyDesc[i][1].SetText(pcBreak + 1u);
                }
                else
                {
                    m_aaTrophyDesc[i][0].SetText(pcString);
                    m_aaTrophyDesc[i][1].SetText("");
                }
            }
            
            const coreFloat fHeight = m_Background.GetPosition().y + m_Background.GetSize().y*0.5f + -0.175f - 0.15f*I_TO_F(i) + (m_aaTrophyDesc[i][1].GetText()[0] ? 0.0f : -0.015f);

            m_aTrophyName[i].SetPosition(coreVector2(m_Background.GetPosition().x - m_Background.GetSize().x*0.5f, fHeight) + coreVector2(0.145f,0.032f) + m_TrophyBox.GetOffset());
            for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaTrophyDesc[0]); ++j)
            {
                m_aaTrophyDesc[i][j].SetPosition(coreVector2(m_Background.GetPosition().x - m_Background.GetSize().x*0.5f, fHeight) + coreVector2(0.145f, -0.003f - 0.03f*I_TO_F(j)) + m_TrophyBox.GetOffset());
            }

            if(bBoss) iCountBoss += 1u;
                 else iCountWave += 1u;
        }
        else
        {
            m_aTrophyName [i]   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aaTrophyDesc[i][0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aaTrophyDesc[i][1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aTrophyCheck[i]   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aTrophyArrow[i]   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aTrophyTile [i]   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aTrophyBack [i]   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aTrophyLine [i]   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aTrophyLine [i]   .SetFocusable(false);
        }
    }

    m_SummaryValue.SetText(PRINT("%zu/%s", iAchieved, (iMax <= MENU_EXTRA_TROPHIES) ? "???" : "48"));
    STATIC_ASSERT(MENU_EXTRA_TROPHIES == 48u)

    m_TrophyBox.SetCurOffset(0.0f);
    m_TrophyBox.SetMaxOffset(0.15f * I_TO_F(MIN(iMax, MENU_EXTRA_TROPHIES)) - m_TrophyBox.GetSize().y);
}


// ****************************************************************
// 
void cExtraMenu::LoadMissions()
{
    // 
    m_FilterMission.ClearEntries();

    // 
    m_FilterMission.AddEntryLanguage("FILTER_GLOBAL",        255u);
    m_FilterMission.AddEntryLanguage("GAME_ARCADE_COMPLETE", 254u);
    for(coreUintW i = 0u; i < ARRAY_SIZE(g_aMissionData); ++i)
    {
        const coreUint8 iAdvance = g_pSave->GetHeader().oProgress.aiAdvance[i] * ((i < MISSION_BASE) ? 1u : 0u);

        if(iAdvance) m_FilterMission.AddEntry(g_aMissionData[i].pcName, i);
    }

    // 
    m_FilterMission.SelectIndex(0u);

    // 
    m_FilterType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
    m_FilterDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
    this->LoadSegments(m_FilterMission.GetCurValue());
    this->__UpdateStats();
    m_StatsBox.SetCurOffset(0.0f);
    m_aiCurFilter.clear();
    
    
    
    const coreBool bMusicBox = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_MUSICBOX);
    
    m_aOtherName[2].SetTextLanguage(bMusicBox ? "EXTRA_MUSIC" : "UNKNOWN");
    m_Music.GetCaption()->SetTextLanguage(bMusicBox ? "LISTEN" : "UNKNOWN");
    m_Music.SetOverride(bMusicBox ? 0 : -1);
}


// ****************************************************************
// 
void cExtraMenu::LoadSegments(const coreUintW iMissionIndex)
{
    // 
    m_aiCurFilter[m_iCurIndex] = m_FilterSegment.GetNumEntries() ? m_FilterSegment.GetCurValue() : 0u;

    // 
    if(iMissionIndex == 255u)
    {
        m_FilterSegment   .SetOverride(-1);
        m_FilterType      .SetOverride(-1);
        m_FilterDifficulty.SetOverride(-1);

        m_FilterSegment   .GetCaption()->SetText("-");
        m_FilterType      .GetCaption()->SetText("-");
        m_FilterDifficulty.GetCaption()->SetText("-");

        m_iCurIndex = 0u;
    }
    else if(iMissionIndex == 254u)
    {
        m_FilterSegment.ClearEntries();

        m_FilterSegment.AddEntryLanguage("FILTER_COMPLETE", 255u);
        for(coreUintW i = 0u; i < ARRAY_SIZE(g_aMissionData); ++i)
        {
            const coreUint8 iAdvance = g_pSave->GetHeader().oProgress.aiAdvance[i] * ((i < MISSION_BASE) ? 1u : 0u);

            if(iAdvance) m_FilterSegment.AddEntry(g_aMissionData[i].pcName, i);
        }

        m_FilterSegment   .SetOverride(0);
        m_FilterType      .SetOverride(0);
        m_FilterDifficulty.SetOverride(0);

        m_FilterType      .Next(); m_FilterType      .Previous();
        m_FilterDifficulty.Next(); m_FilterDifficulty.Previous();

        m_iCurIndex = 1u;
    }
    else
    {
        ASSERT(iMissionIndex < MISSIONS)

        const coreUint8 iAdvance = g_pSave->GetHeader().oProgress.aiAdvance[iMissionIndex] * ((iMissionIndex < MISSION_BASE) ? 1u : 0u);
        const coreUint8 iMin     = (iMissionIndex == MISSION_ATER) ? 5u : 0u;
        const coreUint8 iMax     = (iMissionIndex == MISSION_ATER) ? 7u : 6u;

        m_FilterSegment.ClearEntries();

        for(coreUintW i = iMin, ie = MIN(iAdvance, iMax); i < ie; ++i)
        {
            m_FilterSegment.AddEntry(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, i)), i);
        }

        m_FilterSegment   .SetOverride(0);
        m_FilterType      .SetOverride(0);
        m_FilterDifficulty.SetOverride(0);

        m_FilterType      .Next(); m_FilterType      .Previous();
        m_FilterDifficulty.Next(); m_FilterDifficulty.Previous();

        m_iCurIndex = 2u;
    }

    // 
    if(m_iCurIndex && (!m_aiCurFilter.count(m_iCurIndex) || !m_FilterSegment.SelectValue(m_aiCurFilter.at(m_iCurIndex)))) m_FilterSegment.SelectFirst();
}


// ****************************************************************
// 
void cExtraMenu::SetSelectedTrophy(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    m_iTrophyMission = MIN(iMissionIndex, MISSION_ATER - 1u);
    m_iTrophySegment = MIN(iSegmentIndex, 6u);
    
    cGuiObject& oLine = m_aTrophyLine[m_iTrophyMission * 6u + m_iTrophySegment];

    // 
    m_Navigator.ForceCurrent(&oLine);
    m_TrophyBox.ScrollToObject(&oLine, true);
}


// ****************************************************************
// 
void cExtraMenu::GetSelectedTrophy(coreUintW* OUTPUT piMissionIndex, coreUintW* OUTPUT piSegmentIndex)const
{
    ASSERT(piMissionIndex && piSegmentIndex)

    // 
    (*piMissionIndex) = m_iTrophyMission;
    (*piSegmentIndex) = m_iTrophySegment;
}


// ****************************************************************
// 
void cExtraMenu::__UpdateStats()
{
    coreUintW iIndex = 0u;

    const auto nSetStatFunc = [&] <typename... A> (const coreHashString& sTextKey, const coreChar* pcFormat, A&&... vArgs)
    {
        ASSERT(iIndex < MENU_EXTRA_STATS)

        m_aStatsName [iIndex].SetTextLanguage(sTextKey);
        m_aStatsValue[iIndex].SetText(PRINT(pcFormat, std::forward<A>(vArgs)...));

        iIndex += 1u;
    };

    const coreUint8 iMissionValue    = m_FilterMission   .GetCurValue();
    const coreUint8 iSegmentValue    = m_FilterSegment   .GetCurValue();
    const coreUint8 iTypeValue       = m_FilterType      .GetCurValue();
    const coreUint8 iModeValue       = 0u;
    const coreUint8 iDifficultyValue = m_FilterDifficulty.GetCurValue();

    if(iMissionValue == 255u)
    {
        const auto&      oStats      = g_pSave->GetHeader().oGlobalStats;
        const coreUint64 iTimePlayed = oStats.iTimePlayed / 1000u;

        nSetStatFunc("STAT_TIME_PLAYED",            "%llud %lluh %llum %llus", (iTimePlayed / 86400u), (iTimePlayed / 3600u) % 24u, (iTimePlayed / 60u) % 60u, iTimePlayed % 60u);
        nSetStatFunc("STAT_SCORE_GAINED",           "%'llu",                   oStats.iScoreGained);
        nSetStatFunc("STAT_MISSIONS_DONE",          "%'u",                     oStats.iMissionsDone);
        nSetStatFunc("STAT_SEGMENTS_DONE",          "%'u",                     oStats.iBossesDone + oStats.iWavesDone);
        nSetStatFunc("STAT_BOSSES_DONE",            "%'u",                     oStats.iBossesDone);
        nSetStatFunc("STAT_ENEMIES_DONE",           "%'u",                     oStats.iEnemiesDone);
        nSetStatFunc("STAT_DAMAGE_GIVEN",           "%'llu",                   oStats.iDamageGiven);
        nSetStatFunc("STAT_DAMAGE_TAKEN",           "%'u",                     oStats.iDamageTaken);
        nSetStatFunc("STAT_CONTINUES_USED",         "%'u",                     oStats.iContinuesUsed);
        nSetStatFunc("STAT_REPAIRS_USED",           "%'u",                     oStats.iRepairsUsed);
        nSetStatFunc("STAT_MOVES_MADE",             "%'llu",                   oStats.iMovesMade);
        nSetStatFunc("STAT_TURNS_MADE",             "%'u",                     oStats.iTurnsMade);
        nSetStatFunc("STAT_BULLETS_SHOT",           "%'llu",                   oStats.iBulletsShot);
        nSetStatFunc("STAT_MEDALS_EARNED_BRONZE",   "%'u",                     oStats.aiMedalsEarned[MEDAL_BRONZE]);
        nSetStatFunc("STAT_MEDALS_EARNED_SILVER",   "%'u",                     oStats.aiMedalsEarned[MEDAL_SILVER]);
        nSetStatFunc("STAT_MEDALS_EARNED_GOLD",     "%'u",                     oStats.aiMedalsEarned[MEDAL_GOLD]);
        nSetStatFunc("STAT_MEDALS_EARNED_PLATINUM", "%'u",                     oStats.aiMedalsEarned[MEDAL_PLATINUM]);
        nSetStatFunc("STAT_MEDALS_EARNED_DARK",     "%'u",                     oStats.aiMedalsEarned[MEDAL_DARK]);
        nSetStatFunc("STAT_MEDALS_EARNED",          "%'u",                     oStats.aiMedalsEarned[MEDAL_BRONZE] + oStats.aiMedalsEarned[MEDAL_SILVER] + oStats.aiMedalsEarned[MEDAL_GOLD] + oStats.aiMedalsEarned[MEDAL_PLATINUM] + oStats.aiMedalsEarned[MEDAL_DARK]);
        nSetStatFunc("STAT_BADGES_EARNED",          "%'u",                     oStats.iBadgesEarned);
        nSetStatFunc("STAT_HELPER_HIT",             "%'u",                     oStats.iHelperHit);

        m_FilterIcon.SetEnabled(CORE_OBJECT_ENABLE_MOVE);
    }
    else if(iMissionValue == 254u)
    {
        const auto&     oStats      = this->__PrepareStats(iTypeValue, iModeValue, iDifficultyValue, iSegmentValue, 255u);
        const coreInt32 iBestShift  = coreInt32(oStats.iTimeBestShiftBad)  - coreInt32(oStats.iTimeBestShiftGood);
        const coreInt32 iWorstShift = coreInt32(oStats.iTimeWorstShiftBad) - coreInt32(oStats.iTimeWorstShiftGood);
        const coreFloat fBestTime   = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted)  - I_TO_F(iBestShift),  GAME_GOAL_FACTOR);
        const coreFloat fWorstTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeWorstShifted) - I_TO_F(iWorstShift), GAME_GOAL_FACTOR);

        nSetStatFunc("STAT_COUNT_START",            "%'u",      oStats.iCountStart);
        nSetStatFunc("STAT_COUNT_END",              "%'u",      oStats.iCountEnd);
        nSetStatFunc("STAT_SCORE_BEST",             "%'u",      oStats.iScoreBest);
        nSetStatFunc("STAT_SCORE_WORST",            "%'u",      oStats.iScoreWorst);
        nSetStatFunc("STAT_SCORE_TOTAL",            "%'llu",    oStats.iScoreTotal);
        nSetStatFunc("STAT_TIME_BEST_SHIFTED",      "%.1f %+d", fBestTime, iBestShift);
        nSetStatFunc("STAT_TIME_BEST_RAW",          "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBest),  GAME_GOAL_FACTOR));
        nSetStatFunc("STAT_TIME_WORST_SHIFTED",     "%.1f %+d", fWorstTime, iWorstShift);
        nSetStatFunc("STAT_TIME_WORST_RAW",         "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeWorst), GAME_GOAL_FACTOR));
        nSetStatFunc("STAT_TIME_TOTAL",             "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeTotal), GAME_GOAL_FACTOR));
        nSetStatFunc("STAT_DAMAGE_GIVEN",           "%'llu",    oStats.iDamageGiven);
        nSetStatFunc("STAT_DAMAGE_TAKEN",           "%'u",      oStats.iDamageTaken);
        nSetStatFunc("STAT_CONTINUES_USED",         "%'u",      oStats.iContinuesUsed);
        nSetStatFunc("STAT_REPAIRS_USED",           "%'u",      oStats.iRepairsUsed);
        nSetStatFunc("STAT_MOVES_MADE",             "%'llu",    oStats.iMovesMade);
        nSetStatFunc("STAT_TURNS_MADE",             "%'u",      oStats.iTurnsMade);
        nSetStatFunc("STAT_BULLETS_SHOT",           "%'llu",    oStats.iBulletsShot);
        nSetStatFunc("STAT_MEDALS_EARNED_BRONZE",   "%'u",      oStats.aiMedalsEarned[MEDAL_BRONZE]);
        nSetStatFunc("STAT_MEDALS_EARNED_SILVER",   "%'u",      oStats.aiMedalsEarned[MEDAL_SILVER]);
        nSetStatFunc("STAT_MEDALS_EARNED_GOLD",     "%'u",      oStats.aiMedalsEarned[MEDAL_GOLD]);
        nSetStatFunc("STAT_MEDALS_EARNED_PLATINUM", "%'u",      oStats.aiMedalsEarned[MEDAL_PLATINUM]);
        nSetStatFunc("STAT_MEDALS_EARNED_DARK",     "%'u",      oStats.aiMedalsEarned[MEDAL_DARK]);
        nSetStatFunc("STAT_MEDALS_EARNED",          "%'u",      oStats.aiMedalsEarned[MEDAL_BRONZE] + oStats.aiMedalsEarned[MEDAL_SILVER] + oStats.aiMedalsEarned[MEDAL_GOLD] + oStats.aiMedalsEarned[MEDAL_PLATINUM] + oStats.aiMedalsEarned[MEDAL_DARK]);
        nSetStatFunc("STAT_BADGES_EARNED",          "%'u",      oStats.iBadgesEarned);
        nSetStatFunc("STAT_HELPER_HIT",             "%'u",      oStats.iHelperHit);

        m_FilterIcon.SetEnabled(CORE_OBJECT_ENABLE_MOVE);
    }
    else
    {
        const coreBool bBoss = MISSION_SEGMENT_IS_BOSS(iSegmentValue);

        const auto&     oStats      = this->__PrepareStats(iTypeValue, iModeValue, iDifficultyValue, iMissionValue, iSegmentValue);
        const coreInt32 iBestShift  = coreInt32(oStats.iTimeBestShiftBad)  - coreInt32(oStats.iTimeBestShiftGood);
        const coreInt32 iWorstShift = coreInt32(oStats.iTimeWorstShiftBad) - coreInt32(oStats.iTimeWorstShiftGood);
        const coreFloat fBestTime   = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted)  - I_TO_F(iBestShift),  GAME_GOAL_FACTOR);
        const coreFloat fWorstTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeWorstShifted) - I_TO_F(iWorstShift), GAME_GOAL_FACTOR);

        nSetStatFunc("STAT_COUNT_START",            "%'u",      oStats.iCountStart);
        nSetStatFunc("STAT_COUNT_END",              "%'u",      oStats.iCountEnd);
        nSetStatFunc("STAT_SCORE_BEST",             "%'u",      oStats.iScoreBest);
        nSetStatFunc("STAT_SCORE_WORST",            "%'u",      oStats.iScoreWorst);
        nSetStatFunc("STAT_SCORE_TOTAL",            "%'llu",    oStats.iScoreTotal);
        nSetStatFunc("STAT_TIME_BEST_SHIFTED",      "%.1f %+d", fBestTime, iBestShift);
        nSetStatFunc("STAT_TIME_BEST_RAW",          "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBest),  GAME_GOAL_FACTOR));
        nSetStatFunc("STAT_TIME_WORST_SHIFTED",     "%.1f %+d", fWorstTime, iWorstShift);
        nSetStatFunc("STAT_TIME_WORST_RAW",         "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeWorst), GAME_GOAL_FACTOR));
        nSetStatFunc("STAT_TIME_TOTAL",             "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeTotal), GAME_GOAL_FACTOR));
        if(bBoss)
        {
            nSetStatFunc("STAT_MAX_CHAIN", "%'u", oStats.iMaxSeries);
        }
        else
        {
            nSetStatFunc("STAT_MAX_COMBO", "x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u);
        }
        nSetStatFunc("STAT_DAMAGE_GIVEN",           "%'llu",    oStats.iDamageGiven);
        nSetStatFunc("STAT_DAMAGE_TAKEN",           "%'u",      oStats.iDamageTaken);
        nSetStatFunc("STAT_CONTINUES_USED",         "%'u",      oStats.iContinuesUsed);
        nSetStatFunc("STAT_REPAIRS_USED",           "%'u",      oStats.iRepairsUsed);
        nSetStatFunc("STAT_MOVES_MADE",             "%'llu",    oStats.iMovesMade);
        nSetStatFunc("STAT_TURNS_MADE",             "%'u",      oStats.iTurnsMade);
        nSetStatFunc("STAT_BULLETS_SHOT",           "%'llu",    oStats.iBulletsShot);
        nSetStatFunc("STAT_MEDALS_EARNED_BRONZE",   "%'u",      oStats.aiMedalsEarned[MEDAL_BRONZE]);
        nSetStatFunc("STAT_MEDALS_EARNED_SILVER",   "%'u",      oStats.aiMedalsEarned[MEDAL_SILVER]);
        nSetStatFunc("STAT_MEDALS_EARNED_GOLD",     "%'u",      oStats.aiMedalsEarned[MEDAL_GOLD]);
        nSetStatFunc("STAT_MEDALS_EARNED_PLATINUM", "%'u",      oStats.aiMedalsEarned[MEDAL_PLATINUM]);
        nSetStatFunc("STAT_MEDALS_EARNED_DARK",     "%'u",      oStats.aiMedalsEarned[MEDAL_DARK]);
        nSetStatFunc("STAT_MEDALS_EARNED",          "%'u",      oStats.aiMedalsEarned[MEDAL_BRONZE] + oStats.aiMedalsEarned[MEDAL_SILVER] + oStats.aiMedalsEarned[MEDAL_GOLD] + oStats.aiMedalsEarned[MEDAL_PLATINUM] + oStats.aiMedalsEarned[MEDAL_DARK]);
        if(bBoss)
        {
            nSetStatFunc("STAT_HELPER_HIT", "%'u", oStats.iHelperHit);
        }
        else
        {
            nSetStatFunc("STAT_BADGES_EARNED", "%'u", oStats.iBadgesEarned);
        }

        m_FilterIcon.SetColor3   (g_aMissionData[iMissionValue].vColor * 0.8f);
        m_FilterIcon.SetTexOffset(g_aMissionData[iMissionValue].vIcon);
        m_FilterIcon.SetEnabled  (CORE_OBJECT_ENABLE_ALL);
    }

    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i)
    {
        const coreBool         bVisible = (i < iIndex);
        const coreObjectEnable eEnabled = bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

        m_aStatsName [i].SetEnabled  (eEnabled);
        m_aStatsValue[i].SetEnabled  (eEnabled);
        m_aStatsLine [i].SetEnabled  (eEnabled);
        m_aStatsLine [i].SetFocusable(bVisible);
    }

    m_StatsBox.SetMaxOffset(0.05f * I_TO_F(iIndex) - m_StatsBox.GetSize().y);
    m_StatsBox.Move();
}


// ****************************************************************
// 
const cSave::sLocalStats& cExtraMenu::__PrepareStats(const coreUint8 iTypeValue, const coreUint8 iModeValue, const coreUint8 iDifficultyValue, const coreUint8 iMissionValue, const coreUint8 iSegmentValue)
{
    // 
    std::memset(&m_Stats, 0, sizeof(m_Stats));

    // 
    for(coreUintW i = 0u; i < SAVE_TYPES; ++i)
    {
        if((iTypeValue != i) && (iTypeValue < SAVE_TYPES)) continue;

        for(coreUintW j = 0u; j < SAVE_MODES; ++j)
        {
            if((iModeValue != j) && (iModeValue < SAVE_MODES)) continue;

            for(coreUintW k = 0u; k < SAVE_DIFFICULTIES; ++k)
            {
                if((iDifficultyValue != k) && (iDifficultyValue < SAVE_DIFFICULTIES)) continue;

                const cSave::sLocalStats* A;
                     if(iMissionValue >= SAVE_MISSIONS) A = &g_pSave->GetHeader().aaaLocalStatsArcade   [i][j][k];
                else if(iSegmentValue >= SAVE_SEGMENTS) A = &g_pSave->GetHeader().aaaaLocalStatsMission [i][j][k][iMissionValue];
                else                                    A = &g_pSave->GetHeader().aaaaaLocalStatsSegment[i][j][k][iMissionValue][iSegmentValue];

                m_Stats.iScoreBest  = MAX(m_Stats.iScoreBest,       A->iScoreBest);
                m_Stats.iScoreWorst = MIN(m_Stats.iScoreWorst - 1u, A->iScoreWorst - 1u) + 1u;
                m_Stats.iTimeBest   = MIN(m_Stats.iTimeBest   - 1u, A->iTimeBest   - 1u) + 1u;
                m_Stats.iTimeWorst  = MAX(m_Stats.iTimeWorst,       A->iTimeWorst);
                m_Stats.iScoreTotal = m_Stats.iScoreTotal + A->iScoreTotal;
                m_Stats.iTimeTotal  = m_Stats.iTimeTotal  + A->iTimeTotal;
                m_Stats.iCountStart = m_Stats.iCountStart + A->iCountStart;
                m_Stats.iCountEnd   = m_Stats.iCountEnd   + A->iCountEnd;

                m_Stats.iMaxSeries  = MAX(m_Stats.iMaxSeries, A->iMaxSeries);

                if(A->iTimeBestShifted - 1u < m_Stats.iTimeBestShifted - 1u)
                {
                    m_Stats.iTimeBestShifted   = A->iTimeBestShifted;
                    m_Stats.iTimeBestShiftGood = A->iTimeBestShiftGood;
                    m_Stats.iTimeBestShiftBad  = A->iTimeBestShiftBad;
                }

                if(A->iTimeWorstShifted > m_Stats.iTimeWorstShifted)
                {
                    m_Stats.iTimeWorstShifted   = A->iTimeWorstShifted;
                    m_Stats.iTimeWorstShiftGood = A->iTimeWorstShiftGood;
                    m_Stats.iTimeWorstShiftBad  = A->iTimeWorstShiftBad;
                }

                m_Stats.iDamageGiven    = m_Stats.iDamageGiven    + A->iDamageGiven;
                m_Stats.iDamageTaken    = m_Stats.iDamageTaken    + A->iDamageTaken;
                m_Stats.iContinuesUsed  = m_Stats.iContinuesUsed  + A->iContinuesUsed;
                m_Stats.iRepairsUsed    = m_Stats.iRepairsUsed    + A->iRepairsUsed;
                m_Stats.iShiftGoodAdded = m_Stats.iShiftGoodAdded + A->iShiftGoodAdded;
                m_Stats.iShiftBadAdded  = m_Stats.iShiftBadAdded  + A->iShiftBadAdded;
                m_Stats.iTurnsMade      = m_Stats.iTurnsMade      + A->iTurnsMade;
                m_Stats.iMovesMade      = m_Stats.iMovesMade      + A->iMovesMade;
                m_Stats.iBulletsShot    = m_Stats.iBulletsShot    + A->iBulletsShot;
                m_Stats.iBadgesEarned   = m_Stats.iBadgesEarned   + A->iBadgesEarned;
                m_Stats.iHelperHit      = m_Stats.iHelperHit      + A->iHelperHit;

                for(coreUintW l = 0u; l < SAVE_MEDALS; ++l)
                {
                    m_Stats.aiMedalsEarned[l] = m_Stats.aiMedalsEarned[l] + A->aiMedalsEarned[l];
                }
            }
        }
    }

    return m_Stats;
}