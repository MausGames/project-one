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
cScoreMenu::cScoreMenu()noexcept
: coreMenu    (SURFACE_SCORE_MAX, SURFACE_SCORE_DEFAULT)
, m_iCurIndex (0u)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.01f));
    m_Background.SetSize      (coreVector2(0.9f,0.75f));

    m_DefaultTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_DefaultTab.DefineProgram("menu_border_program");
    m_DefaultTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.0125f));
    m_DefaultTab.SetSize      (coreVector2(0.84f,0.07f));
    m_DefaultTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_DefaultTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_DefaultTab.SetTexOffset (m_DefaultTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(m_Background.GetSize().x * 0.5f, 0.0125f));
    m_DefaultTab.GetCaption()->SetTextLanguage("LEADERBOARDS");

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f, 0.07f)); // * m_SaveButton.GetSize().y); 
    m_BackButton.SetAlignment (coreVector2(-1.0f, -1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    for(coreUintW i = 0u; i < MENU_SCORE_FILTERS; ++i)
    {
        m_aFilterLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aFilterLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aFilterLine[i].DefineProgram("menu_inner_program");
        m_aFilterLine[i].SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f, -0.05f - 0.05f*I_TO_F(i)));
        m_aFilterLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aFilterLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
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

    m_FilterDifficulty.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FilterDifficulty.SetPosition(m_aFilterLine[2].GetPosition());
    m_FilterDifficulty.SetSize    (m_FilterMission.GetSize());
    m_FilterDifficulty.SetEndless (true);
    m_FilterDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FilterIcon.DefineTexture(0u, "menu_helper.png");
    m_FilterIcon.DefineProgram("menu_helper_program");
    m_FilterIcon.SetPosition  (m_FilterMission.GetPosition());
    m_FilterIcon.SetSize      (coreVector2(0.1f,0.1f));
    m_FilterIcon.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_FilterIcon.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
    {
        m_aRank[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aRank[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.05f*I_TO_F(i + MENU_SCORE_FILTERS)));
        m_aRank[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aRank[i].SetColor3   (COLOR_MENU_WHITE);
        m_aRank[i].SetText     (coreData::ToChars(i + 1u));

        m_aName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition (coreVector2(0.05f,0.0f) + m_aRank[i].GetPosition());
        m_aName[i].SetAlignment(coreVector2(1.0f, 0.0f));
        m_aName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aScore[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aScore[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aRank[i].GetPosition());
        m_aScore[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aScore[i].SetColor3   (COLOR_MENU_WHITE);

        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aRank[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i + MENU_SCORE_FILTERS)*0.09f, 0.0f));
    }

    m_ScoreBox.SetPosition (m_Background.GetPosition() + coreVector2(0.0f,-0.05f));
    m_ScoreBox.SetSize     (coreVector2(m_Background.GetSize().x, 0.45f));
    m_ScoreBox.SetMaxOffset(0.05f * I_TO_F(MENU_SCORE_ENTRIES) - m_ScoreBox.GetSize().y);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aLine [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aRank [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aName [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aScore[i]);

    m_PlayerRank.Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_PlayerRank.SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.625f));
    m_PlayerRank.SetAlignment(coreVector2(1.0f,0.0f));
    m_PlayerRank.SetColor3   (COLOR_MENU_WHITE);

    m_PlayerName.Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_PlayerName.SetPosition (coreVector2(0.05f,0.0f) + m_PlayerRank.GetPosition());
    m_PlayerName.SetAlignment(coreVector2(1.0f, 0.0f));
    m_PlayerName.SetColor3   (COLOR_MENU_WHITE);

    m_PlayerScore.Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_PlayerScore.SetPosition (coreVector2(-1.0f,1.0f) * m_PlayerRank.GetPosition());
    m_PlayerScore.SetAlignment(coreVector2(-1.0f,0.0f));
    m_PlayerScore.SetColor3   (COLOR_MENU_WHITE);

    m_PlayerLine.DefineTexture(0u, "menu_detail_04.png");
    m_PlayerLine.DefineTexture(1u, "menu_background_black.png");
    m_PlayerLine.DefineProgram("menu_inner_program");
    m_PlayerLine.SetPosition  (coreVector2(0.0f, m_PlayerRank.GetPosition().y));
    m_PlayerLine.SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
    m_PlayerLine.SetTexOffset (coreVector2(I_TO_F(MENU_SCORE_FILTERS + MENU_SCORE_ENTRIES)*0.09f, 0.0f));

    // 
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);

    // 
    m_Navigator.BindObject(&m_FilterMission,    &m_BackButton,    NULL, &m_FilterSegment,    NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_Navigator.BindObject(&m_FilterSegment,    &m_FilterMission, NULL, &m_FilterDifficulty, NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_Navigator.BindObject(&m_FilterDifficulty, &m_FilterSegment, NULL, &m_aLine[0],         NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);

    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
    {
        m_Navigator.BindObject(&m_aLine[i], (i == 0u)                      ? s_cast<coreObject2D*>(&m_FilterDifficulty) : &m_aLine[(i + MENU_SCORE_ENTRIES - 1u) % MENU_SCORE_ENTRIES], NULL,
                                            (i == MENU_SCORE_ENTRIES - 1u) ? s_cast<coreObject2D*>(&m_BackButton)       : &m_aLine[(i + 1u)                      % MENU_SCORE_ENTRIES], NULL, MENU_TYPE_DEFAULT);
    }

    m_Navigator.BindObject(&m_PlayerLine, &m_aLine[MENU_SCORE_ENTRIES - 1u], NULL, &m_BackButton,    NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_BackButton, &m_PlayerLine,                     NULL, &m_FilterMission, NULL, MENU_TYPE_DEFAULT);

    m_Navigator.BindScroll(&m_ScoreBox);

    m_Navigator.AssignFirst(&m_FilterMission);
    m_Navigator.AssignBack (&m_BackButton);
    m_Navigator.AssignMenu (this);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_SCORE_MAX; ++i)
    {
        this->BindObject(i, &m_Background);
        this->BindObject(i, &m_DefaultTab);
        this->BindObject(i, &m_BackButton);
    }

    for(coreUintW i = 0u; i < MENU_SCORE_FILTERS; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aFilterLine[i]);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerLine);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerRank);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerName);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerScore);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterIcon);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_ScoreBox);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aLine [i]);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aRank [i]);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aName [i]);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aScore[i]);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterMission);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterSegment);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterDifficulty);

    for(coreUintW i = 0u; i < SURFACE_SCORE_MAX; ++i) this->BindObject(i, &m_Navigator);
}


// ****************************************************************
// move the score menu
void cScoreMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_SCORE_DEFAULT:
        {
            // 
            if(m_FilterMission.GetUserSwitch())
                this->LoadSegments(m_FilterMission.GetCurValue());

            // 
            if(m_FilterMission.GetUserSwitch() || m_FilterSegment.GetUserSwitch() || m_FilterDifficulty.GetUserSwitch())
                this->__UpdateScores();

            // 
            cMenu::UpdateSwitchBox(&m_FilterMission);
            cMenu::UpdateSwitchBox(&m_FilterSegment);
            cMenu::UpdateSwitchBox(&m_FilterDifficulty);

            // 
            for(coreUintW i = 0u; i < MENU_SCORE_FILTERS; ++i) cMenu::UpdateLine(&m_aFilterLine[i], true);
            for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) cMenu::UpdateLine(&m_aLine      [i], false);

            // 
            m_FilterMission.GetCaption()->SetColor3((m_FilterMission.GetCurValue() < ARRAY_SIZE(g_aMissionData)) ? g_aMissionData[m_FilterMission.GetCurValue()].vColor : COLOR_MENU_WHITE);

            // 
            m_FilterIcon.SetAlpha(m_FilterIcon.GetAlpha() * 0.6f);
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
    cMenu::UpdateTab(&m_DefaultTab, true, true);

    // 
    cMenu::UpdateButton(&m_BackButton, &m_Navigator, m_BackButton.IsFocused());
}


// ****************************************************************
// 
void cScoreMenu::LoadMissions()
{
    // 
    m_FilterMission.ClearEntries();

    // 
    m_FilterMission.AddEntryLanguage("GAME_ARCADE_COMPLETE", 254u);
    for(coreUintW i = 0u, ie = ARRAY_SIZE(g_aMissionData); i < ie; ++i)
    {
        const coreUint8 iAdvance = g_pSave->GetHeader().oProgress.aiAdvance[i] * ((i < MISSION_BASE) ? 1u : 0u);

        if(iAdvance) m_FilterMission.AddEntry(g_aMissionData[i].pcName, i);
    }

    // 
    m_FilterMission.SelectIndex(0u);

    // 
    m_FilterDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
    this->LoadSegments(m_FilterMission.GetCurValue());
    this->__UpdateScores();
}


// ****************************************************************
// 
void cScoreMenu::LoadSegments(const coreUintW iMissionIndex)
{
    // 
    m_aiCurFilter[m_iCurIndex] = m_FilterSegment.GetNumEntries() ? m_FilterSegment.GetCurValue() : 0u;

    // 
    if(iMissionIndex == 254u)
    {
        m_FilterSegment.ClearEntries();

        m_FilterSegment.AddEntryLanguage("FILTER_COMPLETE", 255u);

        m_FilterSegment.SetOverride(-1);

        m_FilterIcon.SetEnabled(CORE_OBJECT_ENABLE_MOVE);

        m_iCurIndex = 0u;
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

        m_FilterSegment.SetOverride(0);

        m_FilterIcon.SetColor3   (g_aMissionData[iMissionIndex].vColor * 0.8f);
        m_FilterIcon.SetTexOffset(g_aMissionData[iMissionIndex].vIcon);
        m_FilterIcon.SetEnabled  (CORE_OBJECT_ENABLE_ALL);

        m_iCurIndex = 1u;
    }

    // 
    if(m_iCurIndex && (!m_aiCurFilter.count(m_iCurIndex) || !m_FilterSegment.SelectValue(m_aiCurFilter.at(m_iCurIndex)))) m_FilterSegment.SelectFirst();
}


// ****************************************************************
// 
void cScoreMenu::__UpdateScores()
{
    const coreUint32 iStart = 0;

    const coreUint8 iMissionValue    = m_FilterMission.GetCurValue();
    const coreUint8 iSegmentValue    = m_FilterSegment.GetCurValue();
    //const coreUint8 iTypeValue       = 0u;
    //const coreUint8 iModeValue       = 0u;
    const coreUint8 iDifficultyValue = m_FilterDifficulty.GetCurValue();

    const coreChar* pcDifficulty  = iDifficultyValue ? "normal" : "easy";
    const coreChar* pcLeaderboard = (iMissionValue < SAVE_MISSIONS) ? PRINT("stage_score_solo_%s_%02u_%02u", pcDifficulty, iMissionValue, iSegmentValue + 1u) : PRINT("arcade_score_solo_%s", pcDifficulty);

    Core::Platform->DownloadLeaderboard(pcLeaderboard, TYPE_GLOBAL, iStart + 1, iStart + MENU_SCORE_ENTRIES, [this](const coreScore* pScore, const coreUint32 iNum, const coreUint32 iTotal)
    {
        // TODO 1: check rank with start and discard if wrong
        
        for(coreUintW i = 0u, ie = MIN(iNum, MENU_SCORE_ENTRIES); i < ie; ++i)
        {
            m_aRank [i].SetText(coreData::ToChars(pScore[i].iRank));
            m_aName [i].SetText(pScore[i].pcName);
            m_aScore[i].SetText(coreData::ToChars(pScore[i].iValue));

            m_aRank [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aName [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aScore[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aLine [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }
        for(coreUintW i = iNum, ie = MENU_SCORE_ENTRIES; i < ie; ++i)
        {
            m_aRank [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aName [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aScore[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aLine [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    });

    // TODO 1: not on page change
    Core::Platform->DownloadLeaderboard(pcLeaderboard, TYPE_USER, 0, 0, [this](const coreScore* pScore, const coreUint32 iNum, const coreUint32 iTotal)
    {
        if(iNum)
        {
            m_PlayerRank .SetText(coreData::ToChars(pScore->iRank));
            m_PlayerName .SetText(pScore->pcName);
            m_PlayerScore.SetText(coreData::ToChars(pScore->iValue));

            m_PlayerRank .SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_PlayerName .SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_PlayerScore.SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }
        else
        {
            m_PlayerRank .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_PlayerName .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_PlayerScore.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    });
}