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
: coreMenu         (SURFACE_SCORE_MAX, SURFACE_SCORE_DEFAULT)
, m_iCurIndex      (0u)
, m_iPageOffset    (0u)
, m_iPageMax       (0u)
, m_bPageChanged   (false)
, m_iWorkUpload    (0u)
, m_iWorkDownload1 (0u)
, m_iWorkDownload2 (0u)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.045f));
    m_Background.SetSize      (coreVector2(0.9f,0.82f));

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f, 0.07f));
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

    m_FilterPure.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FilterPure.SetPosition(m_aFilterLine[2].GetPosition());
    m_FilterPure.SetSize    (m_FilterMission.GetSize());
    m_FilterPure.SetEndless (true);
    m_FilterPure.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FilterDifficulty.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FilterDifficulty.SetPosition(m_aFilterLine[3].GetPosition());
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
        m_aRank[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.145f - 0.05f*I_TO_F(i + MENU_SCORE_FILTERS)));
        m_aRank[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aRank[i].SetColor3   (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aRank[i].SetText     (coreData::ToChars(i + 1u));

        m_aName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition (coreVector2(1.0f,1.0f) * m_aRank[i].GetPosition() + coreVector2(0.08f,0.0f));
        m_aName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aTime[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aTime[i].SetPosition (coreVector2(0.0f,1.0f) * m_aRank[i].GetPosition());
        m_aTime[i].SetColor3   (COLOR_MENU_WHITE);

        m_aScore[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aScore[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aRank[i].GetPosition() + coreVector2(-0.105f,0.0f));
        m_aScore[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aScore[i].SetColor3   (COLOR_MENU_WHITE);

        m_aIcon[i].DefineTexture(0u, "menu_weapon.png");
        m_aIcon[i].DefineProgram("default_2d_program");
        m_aIcon[i].SetPosition  (coreVector2(-1.0f,1.0f) * m_aRank[i].GetPosition() + coreVector2(-0.04f,0.0f));
        m_aIcon[i].SetSize      (coreVector2( 1.0f,1.0f) * 0.05f);
        m_aIcon[i].SetAlignment (coreVector2(-1.0f,0.0f));
        m_aIcon[i].SetTexSize   (coreVector2(0.25f,0.5f));

        m_aMedal[i].DefineTexture(0u, "menu_medal.png");
        m_aMedal[i].DefineProgram("default_2d_program");
        m_aMedal[i].SetPosition  (coreVector2(-1.0f,1.0f) * m_aRank[i].GetPosition() + coreVector2(0.005f,0.0f));
        m_aMedal[i].SetSize      (coreVector2( 1.0f,1.0f) * 0.046f);
        m_aMedal[i].SetAlignment (coreVector2(-1.0f,0.0f));
        m_aMedal[i].SetTexSize   (coreVector2(0.25f,0.25f));

        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aRank[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i + MENU_SCORE_FILTERS)*0.09f, 0.0f));
        m_aLine[i].SetFocusable (true);
    }

    m_ScoreBox.SetPosition(m_Background.GetPosition() + coreVector2(0.0f,-0.11f));
    m_ScoreBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.4f));
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aLine [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aRank [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aName [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aTime [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aScore[i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aIcon [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aMedal[i]);

    m_PlayerRank.Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_PlayerRank.SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.145f - 0.625f));
    m_PlayerRank.SetAlignment(coreVector2(1.0f,0.0f));
    m_PlayerRank.SetColor3   (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);

    m_PlayerName.Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_PlayerName.SetPosition (coreVector2(1.0f,1.0f) * m_PlayerRank.GetPosition() + coreVector2(0.08f,0.0f));
    m_PlayerName.SetAlignment(coreVector2(1.0f,0.0f));
    m_PlayerName.SetColor3   (COLOR_MENU_WHITE);

    m_PlayerTime.Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_PlayerTime.SetPosition (coreVector2(0.0f,1.0f) * m_PlayerRank.GetPosition());
    m_PlayerTime.SetColor3   (COLOR_MENU_WHITE);

    m_PlayerScore.Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_PlayerScore.SetPosition (coreVector2(-1.0f,1.0f) * m_PlayerRank.GetPosition() + coreVector2(-0.105f,0.0f));
    m_PlayerScore.SetAlignment(coreVector2(-1.0f,0.0f));
    m_PlayerScore.SetColor3   (COLOR_MENU_WHITE);

    m_PlayerIcon.DefineTexture(0u, "menu_weapon.png");
    m_PlayerIcon.DefineProgram("default_2d_program");
    m_PlayerIcon.SetPosition  (coreVector2(-1.0f,1.0f) * m_PlayerRank.GetPosition() + coreVector2(-0.04f,0.0f));
    m_PlayerIcon.SetSize      (coreVector2( 1.0f,1.0f) * 0.05f);
    m_PlayerIcon.SetAlignment (coreVector2(-1.0f,0.0f));
    m_PlayerIcon.SetTexSize   (coreVector2(0.25f,0.5f));

    m_PlayerMedal.DefineTexture(0u, "menu_medal.png");
    m_PlayerMedal.DefineProgram("default_2d_program");
    m_PlayerMedal.SetPosition  (coreVector2(-1.0f,1.0f) * m_PlayerRank.GetPosition() + coreVector2(0.005f,0.0f));
    m_PlayerMedal.SetSize      (coreVector2( 1.0f,1.0f) * 0.046f);
    m_PlayerMedal.SetAlignment (coreVector2(-1.0f,0.0f));
    m_PlayerMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    m_PlayerLine.DefineTexture(0u, "menu_detail_04.png");
    m_PlayerLine.DefineTexture(1u, "menu_background_black.png");
    m_PlayerLine.DefineProgram("menu_inner_program");
    m_PlayerLine.SetPosition  (coreVector2(0.0f, m_PlayerRank.GetPosition().y));
    m_PlayerLine.SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
    m_PlayerLine.SetTexOffset (coreVector2(I_TO_F(MENU_SCORE_FILTERS + MENU_SCORE_ENTRIES)*0.09f, 0.0f));
    m_PlayerLine.SetFocusable (true);

    m_PageSelection.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_2, MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_PageSelection.SetPosition(m_FilterDifficulty.GetPosition() - coreVector2(0.0f,0.0725f));
    m_PageSelection.SetSize    (coreVector2(0.73f,0.065f));
    m_PageSelection.SetEndless (true);
    m_PageSelection.GetArrow(0u)->DefineProgram("menu_border_program");
    m_PageSelection.GetArrow(1u)->DefineProgram("menu_border_program");
    
    m_PageSelection.AddEntry("", 0u);
    m_PageSelection.AddEntry("", 1u);

    m_PageText.Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_PageText.SetPosition(m_PageSelection.GetPosition());
    m_PageText.SetColor3  (COLOR_MENU_WHITE);

    m_Loading.Construct  (MENU_FONT_ICON_4, MENU_OUTLINE_SMALL);
    m_Loading.SetPosition(m_ScoreBox.GetPosition());
    m_Loading.SetColor3  (COLOR_MENU_WHITE);
    m_Loading.SetText    (ICON_GEAR);
    m_Loading.SetRectify (false);

    m_LoadingPlayer.Construct  (MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_LoadingPlayer.SetPosition(m_PlayerLine.GetPosition());
    m_LoadingPlayer.SetColor3  (COLOR_MENU_WHITE);
    m_LoadingPlayer.SetText    (ICON_GEAR);
    m_LoadingPlayer.SetRectify (false);

    // 
    m_FilterPure      .AddEntryLanguage("FILTER_ANY",             254u);
    m_FilterPure      .AddEntryLanguage("FILTER_PURE",            255u);
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);

    // 
    m_Navigator.BindObject(&m_FilterMission,    &m_BackButton,       NULL, &m_FilterSegment,    NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_Navigator.BindObject(&m_FilterSegment,    &m_FilterMission,    NULL, &m_FilterPure,       NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_Navigator.BindObject(&m_FilterPure,       &m_FilterSegment,    NULL, &m_FilterDifficulty, NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_Navigator.BindObject(&m_FilterDifficulty, &m_FilterPure,       NULL, &m_PageSelection,    NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_Navigator.BindObject(&m_PageSelection,    &m_FilterDifficulty, NULL, &m_aLine[0],         NULL, MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);

    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
    {
        m_Navigator.BindObject(&m_aLine[i], (i == 0u)                      ? s_cast<coreObject2D*>(&m_PageSelection) : &m_aLine[(i + MENU_SCORE_ENTRIES - 1u) % MENU_SCORE_ENTRIES], NULL,
                                            (i == MENU_SCORE_ENTRIES - 1u) ? s_cast<coreObject2D*>(&m_PlayerLine)    : &m_aLine[(i + 1u)                      % MENU_SCORE_ENTRIES], NULL, MENU_TYPE_DEFAULT);
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
        this->BindObject(i, &m_BackButton);
    }

    for(coreUintW i = 0u; i < MENU_SCORE_FILTERS; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aFilterLine[i]);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerLine);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerRank);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerTime);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerName);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerScore);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerIcon);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PlayerMedal);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterIcon);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_ScoreBox);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PageSelection);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_PageText);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_Loading);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_LoadingPlayer);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterMission);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterSegment);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_FilterPure);
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
            if(m_FilterMission.GetUserSwitch() || m_FilterSegment.GetUserSwitch() || m_FilterPure.GetUserSwitch() || m_FilterDifficulty.GetUserSwitch())
            {
                m_iPageOffset = 0u;
                this->__UpdateScores();
            }

            // 
            cMenu::UpdateSwitchBox(&m_FilterMission);
            cMenu::UpdateSwitchBox(&m_FilterSegment);
            cMenu::UpdateSwitchBox(&m_FilterPure);
            cMenu::UpdateSwitchBox(&m_FilterDifficulty);

            // 
            for(coreUintW i = 0u; i < MENU_SCORE_FILTERS; ++i) cMenu::UpdateLine(&m_aFilterLine[i], true);
            for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) cMenu::UpdateLine(&m_aLine      [i], false);
            cMenu::UpdateLine(&m_PlayerLine, false);

            // 
            m_FilterMission.GetCaption()->SetColor3((m_FilterMission.GetCurValue() < ARRAY_SIZE(g_aMissionData)) ? g_aMissionData[m_FilterMission.GetCurValue()].vColor : COLOR_MENU_WHITE);

            // 
            m_FilterIcon.SetAlpha(m_FilterIcon.GetAlpha() * 0.6f);
            
            
            coreBool bNavEnabled = false;
            for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
            {
                if(m_aLine[i].IsFocused()) bNavEnabled = true;
            }
            if(m_PlayerLine.IsFocused()) bNavEnabled = true;
            
            const coreBool bLeft  = (bNavEnabled && !m_bPageChanged && (g_MenuInput.iMove == 2u));
            const coreBool bRight = (bNavEnabled && !m_bPageChanged && (g_MenuInput.iMove == 4u));
            
            if(!g_MenuInput.iMove) m_bPageChanged = false;
            
            // 
                 if((m_PageSelection.GetUserSwitch() < 0) || bLeft  || Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LEFT),  CORE_INPUT_PRESS)) {m_bPageChanged = true; if(--m_iPageOffset >= m_iPageMax) m_iPageOffset = m_iPageMax - 1u; this->__UpdateScores(false); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}
            else if((m_PageSelection.GetUserSwitch() > 0) || bRight || Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RIGHT), CORE_INPUT_PRESS)) {m_bPageChanged = true; if(++m_iPageOffset >= m_iPageMax) m_iPageOffset = 0u;              this->__UpdateScores(false); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}

            cMenu::UpdateButton(m_PageSelection.GetArrow(0u), &m_Navigator, m_PageSelection.GetArrow(0u)->IsFocused(), g_pMenu->GetButtonColor(), true, false);
            cMenu::UpdateButton(m_PageSelection.GetArrow(1u), &m_Navigator, m_PageSelection.GetArrow(1u)->IsFocused(), g_pMenu->GetButtonColor(), true, false);
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
    {
        // 
        m_iStatus = 1;
    }

    // 
    cMenu::UpdateButton(&m_BackButton, &m_Navigator, m_BackButton.IsFocused());

    // 
    //const coreBool bNewWorkUpload = !g_pSave->GetScoreQueue()->empty();
    const coreBool bNewWorkUpload2 = std::any_of(g_pSave->GetScoreQueue()->begin(), g_pSave->GetScoreQueue()->end(), [this](const cSave::sScorePack* pPack)
    {
        if(HAS_BIT(pPack->iType, 0u))
        {
            return false;
        }
        if(pPack->oData.iOptionDifficulty != m_FilterDifficulty.GetCurValue())
        {
            return false;
        }
        if((m_FilterPure.GetCurValue() == 255u) != HAS_BIT(pPack->iType, 7u))
        {
            return false;
        }
        if((pPack->iMissionIndex >= SAVE_MISSIONS) && (m_FilterMission.GetCurValue() >= SAVE_MISSIONS) &&
           (pPack->iSegmentIndex >= SAVE_SEGMENTS) && (m_FilterSegment.GetCurValue() >= SAVE_SEGMENTS))
        {
            return true;
        }
        if((pPack->iMissionIndex == m_FilterMission.GetCurValue()) &&
           (pPack->iSegmentIndex == m_FilterSegment.GetCurValue()))
        {
            return true;
        }
        return false;
    });
    if(m_iWorkUpload && !bNewWorkUpload2 && !m_iWorkDownload1)
    {
        this->__UpdateScores();
    }
    m_iWorkUpload = bNewWorkUpload2;

    // 
    const coreFloat   fRotation  = coreFloat(Core::System->GetTotalTime());
    const coreVector2 vDirection = coreVector2::Direction(fRotation * (0.5f*PI));

    m_Loading      .SetDirection(vDirection);
    m_LoadingPlayer.SetDirection(vDirection);

    m_Loading      .SetEnabled((m_iWorkDownload1)                  ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);
    m_LoadingPlayer.SetEnabled((m_iWorkUpload || m_iWorkDownload2) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);

    if(m_iWorkDownload1)
    {
        for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
        {
            m_aRank [i].SetAlpha(m_aRank [i].GetAlpha() * 0.5f);
            m_aName [i].SetAlpha(m_aName [i].GetAlpha() * 0.5f);
            m_aTime [i].SetAlpha(m_aTime [i].GetAlpha() * 0.5f);
            m_aScore[i].SetAlpha(m_aScore[i].GetAlpha() * 0.5f);
            m_aIcon [i].SetAlpha(m_aIcon [i].GetAlpha() * 0.5f);
            m_aMedal[i].SetAlpha(m_aMedal[i].GetAlpha() * 0.5f);
        }
    }

    if(m_iWorkUpload || m_iWorkDownload2)
    {
        m_PlayerRank .SetAlpha(m_PlayerRank .GetAlpha() * 0.5f);
        m_PlayerName .SetAlpha(m_PlayerName .GetAlpha() * 0.5f);
        m_PlayerTime .SetAlpha(m_PlayerTime .GetAlpha() * 0.0f);   // #
        m_PlayerScore.SetAlpha(m_PlayerScore.GetAlpha() * 0.5f);
        m_PlayerIcon .SetAlpha(m_PlayerIcon .GetAlpha() * 0.5f);
        m_PlayerMedal.SetAlpha(m_PlayerIcon .GetAlpha() * 0.5f);
    }
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

    m_FilterPure.SelectValue(g_CurConfig.Game.iPureMode ? 255u : 254u);
    
    this->__ResetScores();
    this->__UpdateScores();
    m_ScoreBox.SetCurOffset(0.0f);
    m_ScoreBox.SetMaxOffset(0.0f);
    m_aiCurFilter.clear();
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
        m_FilterSegment.AddEntry("-", 255u);
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
    if(!m_aiCurFilter.count(m_iCurIndex) || !m_FilterSegment.SelectValue(m_aiCurFilter.at(m_iCurIndex))) m_FilterSegment.SelectFirst();
}


// ****************************************************************
// 
void cScoreMenu::__UpdateScores(const coreBool bPlayer)
{
    if(!g_bLeaderboards) return;

    const coreUint32 iStart = m_iPageOffset * MENU_SCORE_ENTRIES;

    const coreUint8 iMissionValue    = m_FilterMission   .GetCurValue();
    const coreUint8 iSegmentValue    = m_FilterSegment   .GetCurValue();
    const coreUint8 iPureValue       = m_FilterPure      .GetCurValue();
    const coreUint8 iDifficultyValue = m_FilterDifficulty.GetCurValue();

    const coreBool bMission = (iMissionValue < SAVE_MISSIONS);
    const coreBool bSegment = (iSegmentValue < SAVE_SEGMENTS);

    const coreChar* pcDifficulty  = (iDifficultyValue)   ? "normal" : "easy";
    const coreChar* pcPure        = (iPureValue == 255u) ? "_pure"  : "";
    const coreChar* pcLeaderboard = bSegment ? PRINT("stage_score_solo_%s_%02u_%02u%s", pcDifficulty, iMissionValue, iSegmentValue + 1u, pcPure) : PRINT("arcade_score_solo_%s%s", pcDifficulty, pcPure);

    m_iWorkDownload1 += 1u;
    Core::Platform->DownloadLeaderboard(pcLeaderboard, TYPE_GLOBAL, iStart + 1u, iStart + MENU_SCORE_ENTRIES, [=, this](const coreScore* pScore, const coreUint32 iNum, const coreUint32 iTotal)
    {
        m_iWorkDownload1 -= 1u;

        const coreUint32 iRealNum = MIN(iNum, MENU_SCORE_ENTRIES);

        for(coreUintW i = 0u, ie = iRealNum; i < ie; ++i)
        {
            const sScoreData* pData = GetScoreData(&pScore[i]);

            coreInt32 iShift = 0;
            coreFloat fTime  = 0.0f;
            for(coreUintW j = 0u, je = (bSegment || bMission) ? 1u : SCORE_MISSIONS; j < je; ++j)
            {
                iShift += coreInt32(pData->aiShiftBadMission[j]) - coreInt32(pData->aiShiftGoodMission[j]);
                fTime  += FloorFactor(TABLE_TIME_TO_FLOAT(pData->aiTimeMission[j]), 10.0f);
            }

            m_aRank [i].SetText(PRINT("%u.", pScore[i].iRank));
            m_aName [i].SetText(pScore[i].pcName);
            m_aTime [i].SetText(PRINT("%.1f %+d", fTime, iShift));
            m_aScore[i].SetText(PRINT("%'u", pScore[i].iValue));

            cScoreMenu::__SetWeaponIcon(&m_aIcon[i], pData->aaiOptionWeapon[0][0]);

                 if(bSegment) cScoreMenu::__SetMedalIcon      (&m_aMedal[i], pData->aaiMedalSegment[0][0], MISSION_SEGMENT_IS_BOSS(iSegmentValue) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);
            else if(bMission) cScoreMenu::__SetMedalIcon      (&m_aMedal[i], pData->aaiMedalSegment[0][0], MEDAL_TYPE_MISSION);
            else              cScoreMenu::__SetMedalIconArcade(&m_aMedal[i], pData->aaiMedalSegment);

            m_aRank [i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);
            m_aName [i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);
            m_aScore[i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);
            m_aIcon [i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);
            m_aMedal[i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);
            m_aLine [i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);
            m_aLine [i].SetFocusable(true);
        }
        for(coreUintW i = iNum, ie = MENU_SCORE_ENTRIES; i < ie; ++i)
        {
            m_aRank [i].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
            m_aName [i].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
            m_aScore[i].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
            m_aIcon [i].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
            m_aMedal[i].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
            m_aLine [i].SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);
            m_aLine [i].SetFocusable(false);
        }

        m_iPageMax = coreMath::CeilAlign(iTotal, MENU_SCORE_ENTRIES) / MENU_SCORE_ENTRIES;

        m_PageText.SetText(PRINT("%u-%u / %u", iStart + MIN(iRealNum, 1u), iStart + iRealNum, iTotal));

        m_ScoreBox.SetCurOffset(0.0f);
        m_ScoreBox.SetMaxOffset(0.05f * I_TO_F(iRealNum) - m_ScoreBox.GetSize().y - CORE_MATH_PRECISION);
        if(m_Navigator.GetCurScroll() == &m_ScoreBox)
        {
            m_Navigator.OverrideCurrent(&m_aLine[0]);
        }
    });

    if(bPlayer)
    {
        m_iWorkDownload2 += 1u;
        Core::Platform->DownloadLeaderboard(pcLeaderboard, TYPE_USER, 0, 0, [=, this](const coreScore* pScore, const coreUint32 iNum, const coreUint32 iTotal)
        {
            m_iWorkDownload2 -= 1u;

            if(iNum)
            {
                const sScoreData* pData = GetScoreData(pScore);

                coreInt32 iShift = 0;
                coreFloat fTime  = 0.0f;
                for(coreUintW j = 0u, je = (bSegment || bMission) ? 1u : SCORE_MISSIONS; j < je; ++j)
                {
                    iShift += coreInt32(pData->aiShiftBadMission[j]) - coreInt32(pData->aiShiftGoodMission[j]);
                    fTime  += FloorFactor(TABLE_TIME_TO_FLOAT(pData->aiTimeMission[j]), 10.0f);
                }

                m_PlayerRank .SetText(PRINT("%u.", pScore->iRank));
                m_PlayerName .SetText(pScore->pcName);
                m_PlayerTime .SetText(PRINT("%.1f %+d", fTime, iShift));
                m_PlayerScore.SetText(PRINT("%'u", pScore->iValue));

                cScoreMenu::__SetWeaponIcon(&m_PlayerIcon, pData->aaiOptionWeapon[0][0]);

                     if(bSegment) cScoreMenu::__SetMedalIcon      (&m_PlayerMedal, pData->aaiMedalSegment[0][0], MISSION_SEGMENT_IS_BOSS(iSegmentValue) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);
                else if(bMission) cScoreMenu::__SetMedalIcon      (&m_PlayerMedal, pData->aaiMedalSegment[0][0], MEDAL_TYPE_MISSION);
                else              cScoreMenu::__SetMedalIconArcade(&m_PlayerMedal, pData->aaiMedalSegment);

                m_PlayerRank .SetEnabled(CORE_OBJECT_ENABLE_ALL);
                m_PlayerName .SetEnabled(CORE_OBJECT_ENABLE_ALL);
                m_PlayerTime .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                m_PlayerIcon .SetEnabled(CORE_OBJECT_ENABLE_ALL);
                m_PlayerMedal.SetEnabled(CORE_OBJECT_ENABLE_ALL);
                m_PlayerScore.SetEnabled(CORE_OBJECT_ENABLE_ALL);
            }
            else
            {
                m_PlayerTime.SetText("-");

                m_PlayerRank .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                m_PlayerName .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                m_PlayerTime .SetEnabled(CORE_OBJECT_ENABLE_ALL);
                m_PlayerIcon .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                m_PlayerMedal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                m_PlayerScore.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            }
        });
    }
}


// ****************************************************************
// 
void cScoreMenu::__ResetScores()
{
    // 
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
    {
        m_aRank [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aName [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aTime [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aScore[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aIcon [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aMedal[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        m_aLine [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_PlayerRank .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_PlayerName .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_PlayerIcon .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_PlayerMedal.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_PlayerScore.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_PageText.SetText("-");

    // 
    m_iPageOffset = 0u;
    m_iPageMax    = 0u;
}


// ****************************************************************
// 
void cScoreMenu::__SetWeaponIcon(cGuiObject* OUTPUT pObject, const coreUint8 iWeapon)
{
    ASSERT(pObject)

    coreVector3 vColor;
    coreVector2 vTexOffset;

    switch(iWeapon)
    {
    default: UNREACHABLE
    case 1u: vColor = COLOR_MENU_YELLOW; vTexOffset = coreVector2(0.0f, 0.0f); break;
    case 2u: vColor = COLOR_MENU_PURPLE; vTexOffset = coreVector2(0.25f,0.0f); break;
    case 3u: vColor = COLOR_MENU_GREEN;  vTexOffset = coreVector2(0.5f, 0.0f); break;
    }

    pObject->SetColor3   (vColor);
    pObject->SetTexOffset(vTexOffset);
}


// ****************************************************************
// 
void cScoreMenu::__SetMedalIcon(cGuiObject* OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType)
{
    // 
    cMenu::ApplyMedalTexture(pObject, iMedal, iMedalType, true);
}


// ****************************************************************
// 
void cScoreMenu::__SetMedalIconArcade(cGuiObject* OUTPUT pObject, const coreUint8 (&aaiMedal)[SCORE_MISSIONS / 2u][SCORE_SEGMENTS])
{
    coreUint16 iMedalTotal = 0u;
    coreUint8  iMedalCount = 0u;

    for(coreUintW j = 0u; j < MISSION_BASE; ++j)
    {
        // 
        const coreBool bAter = (j == MISSION_ATER);

        for(coreUintW i = 0u, ie = bAter ? 2u : 6u; i < ie; ++i)
        {
            // 
            const coreUintW iIndex = i + (bAter ? 5u : 0u);
            const coreUint8 iValue = GET_BITVALUE(aaiMedal[j / 2u][iIndex], 4u, (j % 2u) * 4u);

            // 
            iMedalTotal += MAX(iValue, MEDAL_BRONZE);
            iMedalCount += 1u;
        }
    }

    // 
    const coreUint8 iMedal = iMedalCount ? (iMedalTotal / iMedalCount) : MEDAL_NONE;

    // 
    cMenu::ApplyMedalTexture(pObject, iMedal, MEDAL_TYPE_ARCADE, true);
}