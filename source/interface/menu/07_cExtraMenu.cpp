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
: coreMenu (SURFACE_EXTRA_MAX, SURFACE_EXTRA_STATS)
, m_Stats  {}
{
    constexpr coreFloat fSplit  = 1.0f/300.0f;   // 0.00333...
    constexpr coreFloat fSplit2 = 1.0f/60.0f;    // 0.01666...

    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.01f));
    m_Background.SetSize      (coreVector2(0.9f,0.75f));

    m_ProgressTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ProgressTab.DefineProgram("menu_border_program");
    m_ProgressTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.15f - fSplit + fSplit2, -0.0125f));
    m_ProgressTab.SetSize      (coreVector2(0.23f + fSplit + fSplit2*2.0f, 0.07f));
    m_ProgressTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_ProgressTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_ProgressTab.SetTexOffset (m_ProgressTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.15f - fSplit + fSplit2, 0.0125f));
    m_ProgressTab.GetCaption()->SetTextLanguage("EXTRA_PROGRESS");

    m_StatsTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StatsTab.DefineProgram("menu_border_program");
    m_StatsTab.SetPosition  (m_ProgressTab.GetPosition() * coreVector2(0.0f,1.0f));
    m_StatsTab.SetSize      (m_ProgressTab.GetSize());
    m_StatsTab.SetAlignment (m_ProgressTab.GetAlignment());
    m_StatsTab.SetTexSize   (m_ProgressTab.GetTexSize());
    m_StatsTab.SetTexOffset (m_ProgressTab.GetTexOffset() + coreVector2(m_StatsTab.GetPosition().x - m_ProgressTab.GetPosition().x, 0.0f));
    m_StatsTab.GetCaption()->SetTextLanguage("EXTRA_STATS");

    m_OtherTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_OtherTab.DefineProgram("menu_border_program");
    m_OtherTab.SetPosition  (m_ProgressTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_OtherTab.SetSize      (m_ProgressTab.GetSize());
    m_OtherTab.SetAlignment (m_ProgressTab.GetAlignment());
    m_OtherTab.SetTexSize   (m_ProgressTab.GetTexSize());
    m_OtherTab.SetTexOffset (m_ProgressTab.GetTexOffset() + coreVector2(m_OtherTab.GetPosition().x - m_ProgressTab.GetPosition().x, 0.0f));
    m_OtherTab.GetCaption()->SetTextLanguage("EXTRA_OTHER");
    
    
    m_StatsTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.235f,-0.0125f));
    m_StatsTab.SetSize      (coreVector2(0.41f,0.07f));
    m_StatsTab.SetTexOffset (m_StatsTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.235f,0.0125f));

    m_OtherTab.SetPosition  (m_StatsTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_OtherTab.SetSize      (m_StatsTab.GetSize());
    m_OtherTab.SetTexOffset (m_StatsTab.GetTexOffset() + coreVector2(m_OtherTab.GetPosition().x - m_StatsTab.GetPosition().x, 0.0));
    
    
    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f,0.07f));
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    m_ProgressArea.DefineTexture(0u, "menu_detail_04.png");
    m_ProgressArea.DefineTexture(1u, "menu_background_black.png");
    m_ProgressArea.DefineProgram("menu_inner_program");
    m_ProgressArea.SetPosition  (m_Background.GetPosition());
    m_ProgressArea.SetSize      (coreVector2(m_Background.GetSize().y, 0.2f));
    m_ProgressArea.SetDirection (coreVector2(1.0f,0.0f));

    for(coreUintW i = 0u; i < MENU_EXTRA_FRAGMENTS; ++i)
    {
        const coreVector2 vDir  = FRAGMENT_DIRECTION;
        const coreVector2 vPos  = MapToAxisInv(FRAGMENT_POSITION(i), vDir) * 0.35f;
        const coreVector2 vSize = g_aFragmentData[i].vSize * 0.35f * ((i == 8u) ? 1.1f : 1.0f);

        m_aFragment[i].DefineTexture(0u, PRINT("menu_fragment_%02zu.png", g_aFragmentData[i].iIndex));
        m_aFragment[i].DefineTexture(1u, "menu_background_black.png");
        m_aFragment[i].DefineProgram("menu_fragment_program");
        m_aFragment[i].SetPosition  (vPos + m_ProgressArea.GetPosition());
        m_aFragment[i].SetSize      (vSize);
        m_aFragment[i].SetDirection (vDir);
        m_aFragment[i].SetColor3    (g_aFragmentData[i].vColor);
    }

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

    m_PasswordHeader.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_PasswordHeader.SetPosition    (coreVector2(0.0f,0.3f));
    m_PasswordHeader.SetColor3      (COLOR_MENU_WHITE);
    m_PasswordHeader.SetTextLanguage("ENTER_PASSWORD");

    // 
    m_FilterType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_FilterType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_FilterDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);

    // 
    //m_Navigator.BindObject(&m_ProgressTab, &m_BackButton, &m_OtherTab,    &m_FilterMission, &m_StatsTab,    MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    //m_Navigator.BindObject(&m_StatsTab,    &m_BackButton, &m_ProgressTab, &m_FilterMission, &m_OtherTab,    MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    //m_Navigator.BindObject(&m_OtherTab,    &m_BackButton, &m_StatsTab,    &m_FilterMission, &m_ProgressTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_StatsTab,    &m_BackButton, &m_OtherTab, &m_FilterMission, &m_OtherTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_OtherTab,    &m_BackButton, &m_StatsTab, &m_FilterMission, &m_StatsTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);

    //m_Navigator.BindObject(&m_aFragment[8], &m_ProgressTab, NULL, &m_BackButton, NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_PROGRESS);
    //m_Navigator.BindObject(&m_aFragment[8], &m_StatsTab, NULL, &m_BackButton, NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_PROGRESS);

    m_Navigator.BindObject(&m_FilterMission,    &m_StatsTab,       NULL,                &m_FilterSegment, NULL,                MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_EXTRA_STATS);
    m_Navigator.BindObject(&m_FilterSegment,    &m_FilterMission,  NULL,                &m_FilterType,    NULL,                MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_EXTRA_STATS);
    m_Navigator.BindObject(&m_FilterType,       &m_FilterSegment,  &m_FilterDifficulty, &m_aStatsLine[0], &m_FilterDifficulty, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS, SURFACE_EXTRA_STATS);
    m_Navigator.BindObject(&m_FilterDifficulty, &m_FilterSegment,  &m_FilterType,       &m_aStatsLine[0], &m_FilterType,       MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS, SURFACE_EXTRA_STATS);

    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i)
    {
        m_Navigator.BindObject(&m_aStatsLine[i], (i == 0u)                    ? s_cast<coreObject2D*>(&m_FilterType) : &m_aStatsLine[(i + MENU_EXTRA_STATS - 1u) % MENU_EXTRA_STATS], NULL,
                                                 (i == MENU_EXTRA_STATS - 1u) ? s_cast<coreObject2D*>(&m_BackButton) : &m_aStatsLine[(i + 1u)                    % MENU_EXTRA_STATS], NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_STATS);
    }

    m_Navigator.BindObject(&m_Password, &m_OtherTab, NULL, &m_Credits,    NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_OTHER);
    m_Navigator.BindObject(&m_Credits,  &m_Password, NULL, &m_BackButton, NULL, MENU_TYPE_TAB_NODE, SURFACE_EXTRA_OTHER);

    m_Navigator.BindObject(&m_BackButton, &m_aStatsLine[MENU_EXTRA_STATS - 1u], NULL, &m_FilterMission, NULL, MENU_TYPE_DEFAULT);

    //m_Navigator.BindSurface(&m_ProgressTab, SURFACE_EXTRA_PROGRESS, &m_aFragment[8],                      NULL, &m_aFragment[8],  NULL);
    m_Navigator.BindSurface(&m_StatsTab,    SURFACE_EXTRA_STATS,    &m_aStatsLine[MENU_EXTRA_STATS - 1u], NULL, &m_FilterMission, NULL);
    m_Navigator.BindSurface(&m_OtherTab,    SURFACE_EXTRA_OTHER,    &m_Credits,                           NULL, &m_Password,      NULL);

    m_Navigator.BindScroll(&m_StatsBox);

    m_Navigator.AssignFirst(&m_FilterMission);
    m_Navigator.AssignBack (&m_BackButton);
    m_Navigator.AssignMenu (this);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_EXTRA_PASSWORD; ++i)
    {
        //if(i != SURFACE_EXTRA_PROGRESS) this->BindObject(i, &m_ProgressTab);
        if(i != SURFACE_EXTRA_STATS)    this->BindObject(i, &m_StatsTab);
        if(i != SURFACE_EXTRA_OTHER)    this->BindObject(i, &m_OtherTab);

        this->BindObject(i, &m_Background);

        //if(i == SURFACE_EXTRA_PROGRESS) this->BindObject(i, &m_ProgressTab);
        if(i == SURFACE_EXTRA_STATS)    this->BindObject(i, &m_StatsTab);
        if(i == SURFACE_EXTRA_OTHER)    this->BindObject(i, &m_OtherTab);

        this->BindObject(i, &m_BackButton);
    }

    this->BindObject(SURFACE_EXTRA_PROGRESS, &m_ProgressArea);
    for(coreUintW i = 0u; i < MENU_EXTRA_FRAGMENTS; ++i) this->BindObject(SURFACE_EXTRA_PROGRESS, &m_aFragment[i]);

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

    for(coreUintW i = 0u; i < SURFACE_EXTRA_PASSWORD; ++i) this->BindObject(i, &m_Navigator);

    this->BindObject(SURFACE_EXTRA_PASSWORD, g_pMenu->GetArcadeInput());
    this->BindObject(SURFACE_EXTRA_PASSWORD, &m_PasswordHeader);

    this->BindObject(SURFACE_EXTRA_CREDITS, g_pMenu->GetCreditRoll());
}


// ****************************************************************
// move the extra menu
void cExtraMenu::Move()
{
    cArcadeInput* pArcadeInput = g_pMenu->GetArcadeInput();
    cCreditRoll*  pCreditRoll  = g_pMenu->GetCreditRoll ();

    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
         //if(m_ProgressTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_PROGRESS);
    /*else*/ if(m_StatsTab   .IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_STATS);
    else if(m_OtherTab   .IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_OTHER);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EXTRA_PROGRESS:
        {
            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 1;
            }

            const coreVector2 vTexOffset = coreVector2(0.0f, coreFloat(Core::System->GetTotalTime()) * 0.1f);
            for(coreUintW i = 0u; i < MENU_EXTRA_FRAGMENTS; ++i) m_aFragment[i].SetTexOffset(vTexOffset);
        }
        break;

    case SURFACE_EXTRA_STATS:
        {
            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 1;
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
                m_iStatus = 1;
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

            // 
            cMenu::UpdateButton(&m_Password, m_Password.IsFocused());
            cMenu::UpdateButton(&m_Credits,  m_Credits .IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS; ++i) cMenu::UpdateLine(&m_aOtherLine[i], true);
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
                    pArcadeInput->Clear();
                    
                    if(bSuccess) this->LoadMissions();

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
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_OTHER, 3.0f, 2u);
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    cMenu::UpdateTab(&m_ProgressTab, (this->GetCurSurface() == SURFACE_EXTRA_PROGRESS), m_ProgressTab.IsFocused());
    cMenu::UpdateTab(&m_StatsTab,    (this->GetCurSurface() == SURFACE_EXTRA_STATS),    m_StatsTab   .IsFocused());
    cMenu::UpdateTab(&m_OtherTab,    (this->GetCurSurface() >= SURFACE_EXTRA_OTHER),    m_OtherTab   .IsFocused());   // # all

    // 
    cMenu::UpdateButton(&m_BackButton, m_BackButton.IsFocused());
}


// ****************************************************************
// 
void cExtraMenu::LoadMissions()
{
    // 
    m_FilterMission.ClearEntries();

    // 
    m_FilterMission.AddEntryLanguage("GAME_GLOBAL", 255u);
    for(coreUintW i = 0u, ie = ARRAY_SIZE(g_aMissionData); i < ie; ++i)
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
}


// ****************************************************************
// 
void cExtraMenu::LoadSegments(const coreUintW iMissionIndex)
{
    // 
    const coreUintW iOldEntry = m_FilterSegment.GetNumEntries() ? m_FilterSegment.GetCurValue() : 0u;
    m_FilterSegment.ClearEntries();

    // 
    if(iMissionIndex < MISSIONS)
    {
        const coreUint8 iAdvance = g_pSave->GetHeader().oProgress.aiAdvance[iMissionIndex] * ((iMissionIndex < MISSION_BASE) ? 1u : 0u);
        const coreUint8 iMin     = (iMissionIndex == MISSION_ATER) ? 5u : 0u;
        const coreUint8 iMax     = (iMissionIndex == MISSION_ATER) ? 7u : 6u;

        m_FilterSegment.AddEntryLanguage("MISSION", 255u);
        for(coreUintW i = iMin, ie = MIN(iAdvance, iMax); i < ie; ++i)
        {
            m_FilterSegment.AddEntry(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, i)), i);
        }

        m_FilterSegment   .SetOverride(0);
        m_FilterType      .SetOverride(0);
        m_FilterDifficulty.SetOverride(0);

        m_FilterType      .Next(); m_FilterType      .Previous();
        m_FilterDifficulty.Next(); m_FilterDifficulty.Previous();
    }
    else
    {
        m_FilterSegment.AddEntryLanguage("GAME_ALL", 255u);

        m_FilterSegment   .SetOverride(-1);
        m_FilterType      .SetOverride(-1);
        m_FilterDifficulty.SetOverride(-1);

        m_FilterType      .GetCaption()->SetText("-");
        m_FilterDifficulty.GetCaption()->SetText("-");
    }

    // 
    if(!m_FilterSegment.SelectValue(iOldEntry)) m_FilterSegment.SelectFirst();
    
    // TODO 1: [MF] save stage similar to "-"
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
        nSetStatFunc("STAT_BOSSES_DONE",            "%'u",                     oStats.iBossesDone);
        nSetStatFunc("STAT_WAVES_DONE",             "%'u",                     oStats.iWavesDone);
        nSetStatFunc("STAT_ENEMIES_DONE",           "%'u",                     oStats.iEnemiesDone);
        nSetStatFunc("STAT_DAMAGE_GIVEN",           "%'llu",                   oStats.iDamageGiven);
        nSetStatFunc("STAT_DAMAGE_TAKEN",           "%'u",                     oStats.iDamageTaken);
        nSetStatFunc("STAT_CONTINUES_USED",         "%'u",                     oStats.iContinuesUsed);
        nSetStatFunc("STAT_REPAIRS_USED",           "%'u",                     oStats.iRepairsUsed);
        nSetStatFunc("STAT_SHIFT_GOOD_ADDED",       "%'u",                     oStats.iShiftGoodAdded);
        nSetStatFunc("STAT_SHIFT_BAD_ADDED",        "%'u",                     oStats.iShiftBadAdded);
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
    else
    {
        const coreBool bMission = (iSegmentValue == 255u);
        const coreBool bBoss    = MISSION_SEGMENT_IS_BOSS(iSegmentValue);

        const auto&     oStats      = bMission ? g_pSave->GetHeader().aaaaLocalStatsMission[iTypeValue][iModeValue][iDifficultyValue][iMissionValue] : g_pSave->GetHeader().aaaaaLocalStatsSegment[iTypeValue][iModeValue][iDifficultyValue][iMissionValue][iSegmentValue];
        const coreInt32 iBestShift  = coreInt32(oStats.iTimeBestShiftBad)  - coreInt32(oStats.iTimeBestShiftGood);
        const coreInt32 iWorstShift = coreInt32(oStats.iTimeWorstShiftBad) - coreInt32(oStats.iTimeWorstShiftGood);
        const coreFloat fBestTime   = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted)  - I_TO_F(iBestShift),  10.0f);
        const coreFloat fWorstTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeWorstShifted) - I_TO_F(iWorstShift), 10.0f);

        nSetStatFunc("STAT_SCORE_BEST",             "%'u",      oStats.iScoreBest);
        nSetStatFunc("STAT_SCORE_WORST",            "%'u",      oStats.iScoreWorst);
        nSetStatFunc("STAT_SCORE_TOTAL",            "%'llu",    oStats.iScoreTotal);
        nSetStatFunc("STAT_TIME_BEST_SHIFTED",      "%.1f %+d", fBestTime, iBestShift);
        nSetStatFunc("STAT_TIME_BEST_RAW",          "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBest),  10.0f));
        nSetStatFunc("STAT_TIME_WORST_SHIFTED",     "%.1f %+d", fWorstTime, iWorstShift);
        nSetStatFunc("STAT_TIME_WORST_RAW",         "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeWorst), 10.0f));
        nSetStatFunc("STAT_TIME_TOTAL",             "%.1f",     FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeTotal), 10.0f));
        if(!bMission)
        {
            if(bBoss) nSetStatFunc("STAT_MAX_CHAIN", "%'u",    oStats.iMaxSeries);
                 else nSetStatFunc("STAT_MAX_COMBO", "x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u);
        }
        nSetStatFunc("STAT_COUNT_START",            "%'u",      oStats.iCountStart);
        nSetStatFunc("STAT_COUNT_END",              "%'u",      oStats.iCountEnd);
        nSetStatFunc("STAT_DAMAGE_GIVEN",           "%'llu",    oStats.iDamageGiven);
        nSetStatFunc("STAT_DAMAGE_TAKEN",           "%'u",      oStats.iDamageTaken);
        nSetStatFunc("STAT_CONTINUES_USED",         "%'u",      oStats.iContinuesUsed);
        nSetStatFunc("STAT_REPAIRS_USED",           "%'u",      oStats.iRepairsUsed);
        nSetStatFunc("STAT_SHIFT_GOOD_ADDED",       "%'u",      oStats.iShiftGoodAdded);
        nSetStatFunc("STAT_SHIFT_BAD_ADDED",        "%'u",      oStats.iShiftBadAdded);
        nSetStatFunc("STAT_MOVES_MADE",             "%'llu",    oStats.iMovesMade);
        nSetStatFunc("STAT_TURNS_MADE",             "%'u",      oStats.iTurnsMade);
        nSetStatFunc("STAT_BULLETS_SHOT",           "%'llu",    oStats.iBulletsShot);
        nSetStatFunc("STAT_MEDALS_EARNED_BRONZE",   "%'u",      oStats.aiMedalsEarned[MEDAL_BRONZE]);
        nSetStatFunc("STAT_MEDALS_EARNED_SILVER",   "%'u",      oStats.aiMedalsEarned[MEDAL_SILVER]);
        nSetStatFunc("STAT_MEDALS_EARNED_GOLD",     "%'u",      oStats.aiMedalsEarned[MEDAL_GOLD]);
        nSetStatFunc("STAT_MEDALS_EARNED_PLATINUM", "%'u",      oStats.aiMedalsEarned[MEDAL_PLATINUM]);
        nSetStatFunc("STAT_MEDALS_EARNED_DARK",     "%'u",      oStats.aiMedalsEarned[MEDAL_DARK]);
        nSetStatFunc("STAT_MEDALS_EARNED",          "%'u",      oStats.aiMedalsEarned[MEDAL_BRONZE] + oStats.aiMedalsEarned[MEDAL_SILVER] + oStats.aiMedalsEarned[MEDAL_GOLD] + oStats.aiMedalsEarned[MEDAL_PLATINUM] + oStats.aiMedalsEarned[MEDAL_DARK]);
        if(bMission || !bBoss)
        {
            nSetStatFunc("STAT_BADGES_EARNED", "%'u", oStats.iBadgesEarned);
        }
        if(bMission || bBoss)
        {
            nSetStatFunc("STAT_HELPER_HIT", "%'u", oStats.iHelperHit);
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
{/*
    std::memset(&m_Stats, 0, sizeof(m_Stats));
    
    for(coreUintW i = 0u; i < SAVE_TYPES; ++i)
    {
        for(coreUintW j = 0u; j < SAVE_MODES; ++j)
        {
            for(coreUintW k = 0u; k < SAVE_DIFFICULTIES; ++k)
            {
                m_Stats.iScoreBest;
                m_Stats.iScoreWorst;
                m_Stats.iScoreTotal;
                m_Stats.iTimeBest;
                m_Stats.iTimeWorst;
                m_Stats.iTimeTotal;
                m_Stats.iCountStart;
                m_Stats.iCountEnd;
        
                m_Stats.iMaxSeries;
        
                m_Stats.iTimeBestShifted;
                m_Stats.iTimeBestShiftGood;
                m_Stats.iTimeBestShiftBad;
                m_Stats.iTimeWorstShifted;
                m_Stats.iTimeWorstShiftGood;
                m_Stats.iTimeWorstShiftBad;

                m_Stats.iDamageGiven;
                m_Stats.iDamageTaken;
                m_Stats.iContinuesUsed;
                m_Stats.iRepairsUsed;
                m_Stats.iShiftGoodAdded;
                m_Stats.iShiftBadAdded;
                m_Stats.iTurnsMade;
                m_Stats.iMovesMade;
                m_Stats.iBulletsShot;
                m_Stats.iBadgesEarned;
                m_Stats.iHelperHit;
                
                for(coreUintW l = 0u; l < SAVE_MEDALS; ++l)
                {
                    m_Stats.aiMedalsEarned[l];
                }
            }
        }
    }*/
    return m_Stats;
}