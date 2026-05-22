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
cPauseMenu::cPauseMenu()noexcept
: coreMenu (SURFACE_PAUSE_MAX, SURFACE_PAUSE_FULL)
{
    // create menu objects
    m_ResumeButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ResumeButton.DefineProgram("menu_border_program");
    m_ResumeButton.SetPosition  (MENU_PAUSE_RESUME_POSITION);
    m_ResumeButton.SetSize      (coreVector2(0.4f,0.07f));
    m_ResumeButton.GetCaption()->SetTextLanguage("RESUME");

    m_RestartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_RestartButton.DefineProgram("menu_border_program");
    m_RestartButton.SetPosition  (m_ResumeButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_RestartButton.SetSize      (m_ResumeButton.GetSize());
    m_RestartButton.GetCaption()->SetTextLanguage("RESTART");

    m_ConfigButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ConfigButton.DefineProgram("menu_border_program");
    m_ConfigButton.SetPosition  (m_RestartButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ConfigButton.SetSize      (m_ResumeButton.GetSize());
    m_ConfigButton.GetCaption()->SetTextLanguage("SETTINGS");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_ConfigButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_ResumeButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i)
    {
        m_aBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aBadge[i].DefineProgram("default_2d_program");
        m_aBadge[i].SetSize      (coreVector2(0.063f,0.063f) * 1.1f);
        m_aBadge[i].SetTexSize   (coreVector2(0.5f,1.0f));

        m_aBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aBadgeWave[i].DefineProgram("menu_single_program");
        m_aBadgeWave[i].SetSize      (m_aBadge[i].GetSize() * 0.7f);

        m_aBadgeBack[i].DefineTexture(0u, "menu_detail_03.png");
        m_aBadgeBack[i].DefineTexture(1u, "menu_background_black.png");
        m_aBadgeBack[i].DefineProgram("menu_free_program");
        m_aBadgeBack[i].SetSize      (coreVector2(7.0f + 2.0f + 2.5f, 0.7f) * 0.07f);
        m_aBadgeBack[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);

        for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaBadgeDesc[0]); ++j)
        {
            m_aaBadgeDesc[i][j].Construct(MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
            m_aaBadgeDesc[i][j].SetColor3(COLOR_MENU_WHITE);
        }
    }

    m_Trophy.DefineTexture(0u, "menu_trophy.png");
    m_Trophy.DefineProgram("default_2d_program");
    m_Trophy.SetSize      (coreVector2(0.063f,0.063f) * 1.1f);

    m_TrophyWave.DefineTexture(0u, "effect_headlight_point.png");
    m_TrophyWave.DefineProgram("menu_single_program");
    m_TrophyWave.SetSize      (m_Trophy.GetSize() * 0.7f);

    m_TrophyBack.DefineTexture(0u, "menu_detail_03.png");
    m_TrophyBack.DefineTexture(1u, "menu_background_black.png");
    m_TrophyBack.DefineProgram("menu_free_program");
    m_TrophyBack.SetSize      (coreVector2(7.0f + 2.0f + 2.0f, 0.7f) * 0.07f);
    m_TrophyBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);

    m_TrophyMark.Construct  (MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_TrophyMark.SetRectifyX(false);

    for(coreUintW j = 0u; j < ARRAY_SIZE(m_aTrophyDesc); ++j)
    {
        m_aTrophyDesc[j].Construct(MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aTrophyDesc[j].SetColor3(COLOR_MENU_WHITE);
    }

    // 
    m_Navigator.BindObject(&m_ResumeButton,  &m_ExitButton,    NULL, &m_RestartButton, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_RestartButton, &m_ResumeButton,  NULL, &m_ConfigButton,  NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ConfigButton,  &m_RestartButton, NULL, &m_ExitButton,    NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ExitButton,    &m_ConfigButton,  NULL, &m_ResumeButton,  NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignFirst(&m_ResumeButton);
    m_Navigator.AssignMenu (this);
    m_Navigator.ShowIcon   (true);

    // bind menu objects
    this->BindObject(SURFACE_PAUSE_FULL, &m_ResumeButton);
    this->BindObject(SURFACE_PAUSE_FULL, &m_RestartButton);
    this->BindObject(SURFACE_PAUSE_FULL, &m_ConfigButton);
    this->BindObject(SURFACE_PAUSE_FULL, &m_ExitButton);
    this->BindObject(SURFACE_PAUSE_FULL, &m_Navigator);

    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i) this->BindObject(SURFACE_PAUSE_FULL, &m_aBadgeBack[i]);
    this->BindObject(SURFACE_PAUSE_FULL, &m_TrophyBack);
    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i) this->BindObject(SURFACE_PAUSE_FULL, &m_aBadgeWave[i]);
    this->BindObject(SURFACE_PAUSE_FULL, &m_TrophyWave);
    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i) this->BindObject(SURFACE_PAUSE_FULL, &m_aBadge[i]);
    this->BindObject(SURFACE_PAUSE_FULL, &m_Trophy);
    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i) for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaBadgeDesc[0]); ++j) this->BindObject(SURFACE_PAUSE_FULL, &m_aaBadgeDesc[i][j]);
    this->BindObject(SURFACE_PAUSE_FULL, &m_TrophyMark);
    for(coreUintW j = 0u; j < ARRAY_SIZE(m_aTrophyDesc); ++j) this->BindObject(SURFACE_PAUSE_FULL, &m_aTrophyDesc[j]);

    this->BindObject(SURFACE_PAUSE_LIGHT, &m_ResumeButton);
    this->BindObject(SURFACE_PAUSE_LIGHT, &m_Navigator);
}


// ****************************************************************
// move the pause menu
void cPauseMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_PAUSE_FULL:
        {
            if(m_ResumeButton.IsClicked() || g_MenuInput.bPause)
            {
                // 
                m_iStatus = 1;
            }
            else if(m_RestartButton.IsClicked())
            {
                // 
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_RESTART"), [this](const eMsgAnswer eAnswer)
                {
                    if(eAnswer == MSGBOX_ANSWER_YES)
                        m_iStatus = 102;
                });
            }
            else if(m_ConfigButton.IsClicked())
            {
                // 
                m_iStatus = 3;
            }
            else if(m_ExitButton.IsClicked())
            {
                // 
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_EXIT"), [this](const eMsgAnswer eAnswer)
                {
                    if(eAnswer == MSGBOX_ANSWER_YES)
                        m_iStatus = 104;
                });
            }

            // 
            m_ResumeButton.SetPosition(MENU_PAUSE_RESUME_POSITION);

            // 
            cMenu::UpdateButton(&m_ResumeButton,  &m_Navigator, m_ResumeButton .IsFocused());
            cMenu::UpdateButton(&m_RestartButton, &m_Navigator, m_RestartButton.IsFocused());
            cMenu::UpdateButton(&m_ConfigButton,  &m_Navigator, m_ConfigButton .IsFocused());
            cMenu::UpdateButton(&m_ExitButton,    &m_Navigator, m_ExitButton   .IsFocused());
            
            
            const coreFloat fRotation1 = Core::System->GetTotalTimeFloat(4.0*PI_D);

            for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i)
            {
                if(m_aBadgeWave[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    m_aBadge[i].SetDirection(coreVector2::Direction(fRotation1 * -1.0f - (0.8f*PI) * (I_TO_F(i) / I_TO_F(3u))));
                }
                else
                {
                    m_aBadge[i].SetDirection(coreVector2(0.0f,1.0f));
                }
                m_aBadge[i].Move();
            }
            
            if(!m_aaBadgeDesc[1][0].GetText()[0]) m_aBadgeBack[1].SetAlpha(m_aBadgeBack[1].GetAlpha() * 0.5f);
            
            
            
        }
        break;

    case SURFACE_PAUSE_LIGHT:
        {
            if(m_ResumeButton.IsClicked() || g_MenuInput.bPause)
            {
                // 
                m_iStatus = 1;
            }

            // 
            m_ResumeButton.SetPosition(coreVector2(0.0f,0.0f));

            // 
            cMenu::UpdateButton(&m_ResumeButton, &m_Navigator, m_ResumeButton.IsFocused());
        }
        break;

    default:
        UNREACHABLE
        break;
    }
}


// ****************************************************************
// 
void cPauseMenu::LoadDescriptions()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    //
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    const coreUintW iSegmentIndex = g_pGame->GetCurMission()->GetCurSegmentIndex();

    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];
    const coreBool      bBoss = MISSION_SEGMENT_IS_BOSS(iSegmentIndex);

    // 
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    const coreBool         bValid         = (iSegmentIndex < SEGMENTS) && (oProgress.aiAdvance[iMissionIndex] > iSegmentIndex + 1u) && (iMissionIndex < MISSION_ATER);
    const coreObjectEnable eEnabledBadge  = (bValid && !bBoss)          ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;
    const coreObjectEnable eEnabledTrophy = (bValid && !g_bDemoVersion) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i)
    {
        m_aBadge    [i].SetEnabled(eEnabledBadge);
        m_aBadgeWave[i].SetEnabled(eEnabledBadge);
        m_aBadgeBack[i].SetEnabled(eEnabledBadge);
        for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaBadgeDesc[0]); ++j) m_aaBadgeDesc[i][j].SetEnabled(eEnabledBadge);
    }

    m_Trophy    .SetEnabled(eEnabledTrophy);
    m_TrophyWave.SetEnabled(eEnabledTrophy);
    m_TrophyBack.SetEnabled(eEnabledTrophy);
    m_TrophyMark.SetEnabled(eEnabledTrophy);
    for(coreUintW j = 0u; j < ARRAY_SIZE(m_aTrophyDesc); ++j) m_aTrophyDesc[j].SetEnabled(eEnabledTrophy);

    if(!bValid) return;

    // 
    const coreBool abState[] =
    {
        HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 0u) || g_pGame->GetPlayer(0u)->GetDataTable()->GetBadge(0u, iMissionIndex, iSegmentIndex),
        HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 1u) || g_pGame->GetPlayer(0u)->GetDataTable()->GetBadge(1u, iMissionIndex, iSegmentIndex),
        HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 2u) || g_pGame->GetPlayer(0u)->GetDataTable()->GetBadge(2u, iMissionIndex, iSegmentIndex),
        (oProgress.aiAdvance[iMissionIndex] > iSegmentIndex + 1u)   // # redundant
    };
    STATIC_ASSERT(ARRAY_SIZE(abState) >= MENU_GAME_ARMORY_BADGES);

    // 
    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i)
    {
        const coreBool bState   = abState[i];
        const coreBool bVisible = (iMissionIndex == MISSION_INTRO) ? ((i != 0u) || abState[0]) : (((i == 0u) && (abState[0] || abState[3])) || ((i == 1u) && (abState[0] || abState[1])) || ((i == 2u) && ((abState[0] && abState[1]) || abState[2])));

        m_aBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
        m_aBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));

        m_aBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        const coreChar* pcString = Core::Language->GetString(bVisible ? PRINT("BADGE_%02d_%02zu_%02zu_DESC", oData.iID, iSegmentIndex + 1u, i + 1u) : "UNKNOWN");
        const coreChar* pcBreak  = std::strchr(pcString, '#');

        if(pcBreak)
        {
            m_aaBadgeDesc[i][0].SetText(pcString, pcBreak - pcString);
            m_aaBadgeDesc[i][1].SetText(pcBreak + 1u);
        }
        else
        {
            m_aaBadgeDesc[i][0].SetText(pcString);
            m_aaBadgeDesc[i][1].SetText("");
        }
    }

    const coreBool bTrophyReceived = HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 3u);
    const coreBool bTrophyFailed   = g_pGame->GetCurMission()->GetTrophyFailed();

    m_TrophyWave.SetEnabled(bTrophyReceived ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

    m_Trophy.SetColor3(coreVector3(1.0f,1.0f,1.0f) * (bTrophyReceived ? 1.0f : 0.5f) * 0.95f);

    const coreBool bHasSwitch = DEFINED(_CORE_SWITCH_) && (iMissionIndex == 0u) && (iSegmentIndex == 0u);

    const coreChar* pcString = Core::Language->GetString(PRINT("ACHIEVEMENT_STAGE_%02d_%02zu_DESC%s", oData.iID, iSegmentIndex + 1u, bHasSwitch ? "_SWITCH" : ""));
    const coreChar* pcBreak  = std::strchr(pcString, '#');

    if(pcBreak)
    {
        m_aTrophyDesc[0].SetText(pcString, pcBreak - pcString);
        m_aTrophyDesc[1].SetText(pcBreak + 1u);
    }
    else
    {
        m_aTrophyDesc[0].SetText(pcString);
        m_aTrophyDesc[1].SetText("");
    }

    if(bTrophyReceived)
    {
        m_TrophyMark.SetColor3(COLOR_MENU_BLUE);
        m_TrophyMark.SetText  (ICON_CHECK);
    }
    else if(bTrophyFailed)
    {
        m_TrophyMark.SetColor3(COLOR_MENU_RED);
        m_TrophyMark.SetText  (ICON_TIMES);
    }
    m_TrophyMark.SetEnabled((bTrophyReceived || bTrophyFailed) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

    ASSERT(m_aaBadgeDesc[0][0].GetFont().IsUsable())

    constexpr coreFloat fWidth = 0.6f;

    for(coreUintW i = 0u; i < MENU_PAUSE_BADGES; ++i)
    {
        m_aBadge    [i].SetPosition(coreVector2(fWidth * -0.5f, -0.25f + I_TO_F(i) * -0.08f));
        m_aBadgeWave[i].SetPosition(m_aBadge[i].GetPosition());
        m_aBadgeBack[i].SetPosition(coreVector2(0.0f, m_aBadge[i].GetPosition().y));
        for(coreUintW j = 0u; j < ARRAY_SIZE(m_aaBadgeDesc[0]); ++j)
        {
            m_aaBadgeDesc[i][j].SetPosition(m_aBadge[i].GetPosition() + coreVector2(0.05f, m_aaBadgeDesc[i][1].GetText()[0] ? (j ? -0.015f : 0.015f) : 0.0f));
            m_aaBadgeDesc[i][j].SetPosition(coreVector2(0.0f, m_aaBadgeDesc[i][j].GetPosition().y));
        }
    }
    m_Trophy    .SetPosition(coreVector2(fWidth * -0.5f, -0.25f + I_TO_F(2u) * -0.08f));
    m_TrophyWave.SetPosition(m_Trophy.GetPosition());
    m_TrophyBack.SetPosition(coreVector2(0.0f, m_Trophy.GetPosition().y));
    m_TrophyMark.SetPosition(m_Trophy.GetPosition());
    for(coreUintW j = 0u; j < ARRAY_SIZE(m_aTrophyDesc); ++j)
    {
        m_aTrophyDesc[j].SetPosition(m_Trophy.GetPosition() + coreVector2(0.05f, m_aTrophyDesc[1].GetText()[0] ? (j ? -0.015f : 0.015f) : 0.0f));
        m_aTrophyDesc[j].SetPosition(coreVector2(0.0f, m_aTrophyDesc[j].GetPosition().y));
    }
}


// ****************************************************************
// 
void cPauseMenu::ActivateFirstPlay()
{
    // 
    m_RestartButton.SetOverride(-1);
}


// ****************************************************************
// 
void cPauseMenu::DeactivateFirstPlay()
{
    // 
    m_RestartButton.SetOverride(0);
}


// ****************************************************************
// 
void cPauseMenu::ResetNavigator()
{
    // 
    m_Navigator.ResetFirst();

    // 
    cMenu::ClearButtonTime(&m_ResumeButton);
    cMenu::ClearButtonTime(&m_RestartButton);
    cMenu::ClearButtonTime(&m_ConfigButton);
    cMenu::ClearButtonTime(&m_ExitButton);
}