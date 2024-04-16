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
: coreMenu (SURFACE_EXTRA_MAX, SURFACE_EXTRA_PROGRESS)
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

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f, 0.07f)); // * m_SaveButton.GetSize().y); 
    m_BackButton.SetAlignment (coreVector2(-1.0f, -1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    m_TrophyHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_TrophyHeader.SetPosition    (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.21f,-0.045f));
    m_TrophyHeader.SetColor3      (COLOR_MENU_WHITE);
    m_TrophyHeader.SetTextLanguage("UNKNOWN");   // TODO 1: "EXTRA_TROPHIES"

    m_ProgressArea.DefineTexture(0u, "menu_detail_04.png");
    m_ProgressArea.DefineTexture(1u, "menu_background_black.png");
    m_ProgressArea.DefineProgram("menu_inner_program");
    m_ProgressArea.SetPosition  (coreVector2(m_TrophyHeader.GetPosition().x, m_Background.GetPosition().y));
    m_ProgressArea.SetSize      (coreVector2(m_Background.GetSize().y, 0.43f));
    m_ProgressArea.SetDirection (coreVector2(1.0f,0.0f));

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFragment); ++i)
    {
        m_aFragment[i].DefineTexture(0u, Core::Manager::Resource->Load<coreTexture>("fragments.png", CORE_RESOURCE_UPDATE_AUTO, "data/textures/fragments.png", CORE_TEXTURE_LOAD_NO_COMPRESS));
        m_aFragment[i].DefineProgram("menu_grey_program");
        m_aFragment[i].SetPosition  (m_Background.GetPosition() + coreVector2(-0.22f,0.0f));
        m_aFragment[i].SetSize      (coreVector2(0.25f,0.25f));
        m_aFragment[i].SetDirection (coreVector2(1.0f,1.0f).Normalized());
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrophy); ++i)
    {
        const coreVector2 vPos  = coreVector2(I_TO_F(i % 4u) - 1.5f, -I_TO_F(i / 4u) + 2.0f) * 0.095f + coreVector2(0.0f,0.07f);

        m_aTrophy[i].DefineTexture(0u, "environment_clouds_grey.png");
        m_aTrophy[i].DefineProgram("menu_grey_program");
        m_aTrophy[i].SetPosition  (m_ProgressArea.GetPosition() + vPos);
        m_aTrophy[i].SetSize      (coreVector2(0.07f,0.07f));
    }

    m_TrophyName.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_TrophyName.SetPosition(m_ProgressArea.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,-0.5f) + coreVector2(0.0f,0.155f));
    m_TrophyName.SetColor3  (COLOR_MENU_WHITE);
    m_TrophyName.SetText("One Color Clear");               

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrophyDescription); ++i)
    {
        m_aTrophyDescription[i].Construct  (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aTrophyDescription[i].SetPosition(m_ProgressArea.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,-0.5f) + coreVector2(0.0f, 0.105f - 0.03f*I_TO_F(i)));
        m_aTrophyDescription[i].SetColor3  (COLOR_MENU_WHITE);
        m_aTrophyDescription[i].SetText("Lorem ipsum dolor sit amet");               
    }
    m_aTrophyDescription[0].SetText("Hold a combo multiplier of");               
    m_aTrophyDescription[1].SetText("x10 for at least 20 seconds.");               
    m_aTrophyDescription[2].SetText("Lorem ipsum dolor sit amet");               

    for(coreUintW i = 0u; i < MENU_EXTRA_FILTERS; ++i)
    {
        m_aFilterLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aFilterLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aFilterLine[i].DefineProgram("menu_inner_program");
        m_aFilterLine[i].SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f, -0.05f - 0.05f*I_TO_F(i)));
        m_aFilterLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aFilterLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }

    m_Mission.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Mission.SetPosition(m_aFilterLine[0].GetPosition());
    m_Mission.SetSize    (coreVector2(0.47f,0.03f));
    m_Mission.SetEndless (true);
    m_Mission.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_Segment.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Segment.SetPosition(m_aFilterLine[1].GetPosition());
    m_Segment.SetSize    (m_Mission.GetSize());
    m_Segment.SetEndless (true);
    m_Segment.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i)
    {
        m_aStatsName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aStatsName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.05f*I_TO_F(i + MENU_EXTRA_FILTERS)));
        m_aStatsName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aStatsName[i].SetColor3   (COLOR_MENU_WHITE);
        m_aStatsName[i].SetText(PRINT("TEST %zu", i));

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
    }
    m_aStatsName[0].SetTextLanguage("STAT_TIME_PLAYED");
    m_aStatsName[1].SetTextLanguage("STAT_SCORE_GAINED");
    m_aStatsName[2].SetTextLanguage("STAT_MISSIONS_DONE");
    m_aStatsName[3].SetTextLanguage("STAT_BOSSES_DONE");
    m_aStatsName[4].SetTextLanguage("STAT_WAVES_DONE");
    

    m_StatsBox.SetPosition (m_Background.GetPosition() + coreVector2(0.0f,-0.05f));
    m_StatsBox.SetSize     (coreVector2(m_Background.GetSize().x, 0.55f));
    m_StatsBox.SetMaxOffset(0.05f * I_TO_F(MENU_EXTRA_STATS) - m_StatsBox.GetSize().y);
    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i) m_StatsBox.BindObject(&m_aStatsLine [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i) m_StatsBox.BindObject(&m_aStatsName [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_STATS; ++i) m_StatsBox.BindObject(&m_aStatsValue[i]);
    

    for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS; ++i)
    {
        m_aOtherName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
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
    m_aOtherName[2].SetTextLanguage("EXTRA_CREDITS2");

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

    m_Credits2.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Credits2.SetPosition (coreVector2(-1.00f,1.00f) * m_aOtherName[2].GetPosition());
    m_Credits2.SetSize     (m_Password.GetSize());
    m_Credits2.SetAlignment(m_Password.GetAlignment());
    m_Credits2.GetCaption()->SetTextLanguage("WATCH");

    // 
    m_Navigator.BindObject(&m_BackButton, NULL, NULL, NULL, NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.AssignFirst(&m_BackButton);
    m_Navigator.AssignMenu(this);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_EXTRA_PASSWORD; ++i)
    {
        if(i != SURFACE_EXTRA_PROGRESS) this->BindObject(i, &m_ProgressTab);
        if(i != SURFACE_EXTRA_STATS)    this->BindObject(i, &m_StatsTab);
        if(i != SURFACE_EXTRA_OTHER)    this->BindObject(i, &m_OtherTab);

        this->BindObject(i, &m_Background);

        if(i == SURFACE_EXTRA_PROGRESS) this->BindObject(i, &m_ProgressTab);
        if(i == SURFACE_EXTRA_STATS)    this->BindObject(i, &m_StatsTab);
        if(i == SURFACE_EXTRA_OTHER)    this->BindObject(i, &m_OtherTab);

        this->BindObject(i, &m_BackButton);
        this->BindObject(i, &m_Navigator);
    }

    this->BindObject(SURFACE_EXTRA_PROGRESS, &m_ProgressArea);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFragment);          ++i) this->BindObject(SURFACE_EXTRA_PROGRESS, &m_aFragment[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrophy);            ++i) this->BindObject(SURFACE_EXTRA_PROGRESS, &m_aTrophy  [i]);
    this->BindObject(SURFACE_EXTRA_PROGRESS, &m_TrophyHeader);
    this->BindObject(SURFACE_EXTRA_PROGRESS, &m_TrophyName);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrophyDescription); ++i) this->BindObject(SURFACE_EXTRA_PROGRESS, &m_aTrophyDescription[i]);

    for(coreUintW i = 0u; i < MENU_EXTRA_FILTERS; ++i) this->BindObject(SURFACE_EXTRA_STATS, &m_aFilterLine[i]);
    this->BindObject(SURFACE_EXTRA_STATS, &m_StatsBox);
    //for(coreUintW i = 0u; i < MENU_EXTRA_STATS;   ++i) this->BindObject(SURFACE_EXTRA_STATS, &m_aStatsLine [i]);
    //for(coreUintW i = 0u; i < MENU_EXTRA_STATS;   ++i) this->BindObject(SURFACE_EXTRA_STATS, &m_aStatsName [i]);
    //for(coreUintW i = 0u; i < MENU_EXTRA_STATS;   ++i) this->BindObject(SURFACE_EXTRA_STATS, &m_aStatsValue[i]);
    this->BindObject(SURFACE_EXTRA_STATS, &m_Mission);
    this->BindObject(SURFACE_EXTRA_STATS, &m_Segment);

    for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS;  ++i) this->BindObject(SURFACE_EXTRA_OTHER, &m_aOtherLine [i]);
    for(coreUintW i = 0u; i < MENU_EXTRA_OTHERS;  ++i) this->BindObject(SURFACE_EXTRA_OTHER, &m_aOtherName [i]);

    this->BindObject(SURFACE_EXTRA_OTHER, &m_Password);
    this->BindObject(SURFACE_EXTRA_OTHER, &m_Credits);
    this->BindObject(SURFACE_EXTRA_OTHER, &m_Credits2);

    this->BindObject(SURFACE_EXTRA_PASSWORD, g_pMenu->GetArcadeInput());
    this->BindObject(SURFACE_EXTRA_CREDITS,  g_pMenu->GetCreditRoll ());
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
         if(m_ProgressTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_PROGRESS);
    else if(m_StatsTab   .IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_STATS);
    else if(m_OtherTab   .IsClicked()) cMenu::ChangeTab(this, SURFACE_EXTRA_OTHER);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_EXTRA_PROGRESS:
        {

        }
        break;

    case SURFACE_EXTRA_STATS:
        {
            // 
            if(m_Mission.GetUserSwitch())
                this->LoadSegments(m_Mission.GetCurIndex());

            const coreUint64 iTimePlayed = g_pSave->GetHeader().oGlobalStats.iTimePlayed / 1000u;

            #define PRINT_EXT(...)   (coreData::Print(__VA_ARGS__))
            
            // 
            m_aStatsValue[0].SetText(PRINT_EXT("%llud %lluh %llum %llus", (iTimePlayed / 86400u), (iTimePlayed / 3600u) % 24u, (iTimePlayed / 60u) % 60u, iTimePlayed % 60u));
            m_aStatsValue[1].SetText(PRINT_EXT("%'llu", g_pSave->GetHeader().oGlobalStats.iScoreGained));
            m_aStatsValue[2].SetText(PRINT_EXT("%'u",  g_pSave->GetHeader().oGlobalStats.iMissionsDone));
            m_aStatsValue[3].SetText(PRINT_EXT("%'u",  g_pSave->GetHeader().oGlobalStats.iBossesDone));
            m_aStatsValue[4].SetText(PRINT_EXT("%'u",  g_pSave->GetHeader().oGlobalStats.iWavesDone));

            // 
            cMenu::UpdateSwitchBox(&m_Mission);
            cMenu::UpdateSwitchBox(&m_Segment);
        }
        break;

    case SURFACE_EXTRA_OTHER:
        {
            // 
            if(m_Password.IsClicked())
            {
                pArcadeInput->Start("", 16u);
                
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_PASSWORD, 3.0f, 1u);
            }

            // 
            if(m_Credits.IsClicked())
            {
                pCreditRoll->Start();
                
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_CREDITS, 3.0f, 1u);
            }

            // 
            cMenu::UpdateButton(&m_Password, m_Password.IsFocused());
            cMenu::UpdateButton(&m_Credits,  m_Credits .IsFocused());
            cMenu::UpdateButton(&m_Credits2, m_Credits2.IsFocused());
        }
        break;

    case SURFACE_EXTRA_PASSWORD:
        {
            if(pArcadeInput->GetFinished())
            {
                g_pMenu->ShiftSurface(this, SURFACE_EXTRA_OTHER, 3.0f, 2u);
            }
        }
        break;

    case SURFACE_EXTRA_CREDITS:
        {
            
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(m_BackButton.IsClicked() || g_MenuInput.bCancel)   // TODO 1: can cancel out of password and credits
    {
        // 
        m_iStatus = 1;
    }

    // 
    cMenu::UpdateTab(&m_ProgressTab, (this->GetCurSurface() == SURFACE_EXTRA_PROGRESS), m_ProgressTab.IsFocused());
    cMenu::UpdateTab(&m_StatsTab,    (this->GetCurSurface() == SURFACE_EXTRA_STATS),    m_StatsTab   .IsFocused());
    cMenu::UpdateTab(&m_OtherTab,    (this->GetCurSurface() == SURFACE_EXTRA_OTHER),    m_OtherTab   .IsFocused());

    // 
    cMenu::UpdateButton(&m_BackButton, m_BackButton.IsFocused());

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_BackButton), TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}


// ****************************************************************
// 
void cExtraMenu::LoadMissions()
{
    // 
    const coreUintW iOldEntry = m_Mission.GetCurIndex();
    m_Mission.ClearEntries();

    // 
    m_Mission.AddEntry("Global", 0u);   // TODO 1: language
    for(coreUintW i = 0u, ie = ARRAY_SIZE(g_aMissionData); i < ie; ++i) m_Mission.AddEntry(g_aMissionData[i].pcName, 0u);

    // 
    m_Mission.SelectIndex((iOldEntry < m_Mission.GetNumEntries()) ? iOldEntry : 0u);
    
    
    this->LoadSegments(m_Mission.GetCurIndex());
}


// ****************************************************************
// 
void cExtraMenu::LoadSegments(const coreUintW iIndex)
{
    // 
    const coreUintW iOldEntry = m_Segment.GetCurIndex();
    m_Segment.ClearEntries();
    
    const coreUintW iMissionIndex = iIndex - 1u;

    // 
    if(iIndex)
    {
        m_Segment.AddEntry("Adventure", 0u);   // TODO 1: language
        for(coreUintW i = 0u; i < 7u; ++i)
        {
            m_Segment.AddEntry(cMenu::GetSegmentLetters(iMissionIndex, i), 0u);
        }
        m_Segment.SetOverride(0);
    }
    else
    {
        for(coreUintW i = 0u; i < 8u; ++i) m_Segment.AddEntry("All", 0u);
        m_Segment.SetOverride(-1);
    }

    // 
    m_Segment.SelectIndex((iOldEntry < m_Segment.GetNumEntries()) ? iOldEntry : 0u);
}