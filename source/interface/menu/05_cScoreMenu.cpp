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
: coreMenu (SURFACE_SCORE_MAX, SURFACE_SCORE_DEFAULT)
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

    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i)
    {
        m_aRank[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aRank[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.05f*I_TO_F(i + MENU_EXTRA_FILTERS)));
        m_aRank[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aRank[i].SetColor3   (COLOR_MENU_WHITE);
        m_aRank[i].SetText     (coreData::ToChars(i + 1u));

        m_aName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition (coreVector2(0.05f,0.0f) + m_aRank[i].GetPosition());
        m_aName[i].SetAlignment(coreVector2(1.0f, 0.0f));
        m_aName[i].SetColor3   (COLOR_MENU_WHITE);
        m_aName[i].SetText     ("Horst");

        m_aScore[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aScore[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aRank[i].GetPosition());
        m_aScore[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aScore[i].SetColor3   (COLOR_MENU_WHITE);
        m_aScore[i].SetText     ("0000000");

        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aRank[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i + MENU_EXTRA_FILTERS)*0.09f, 0.0f));
    }

    m_ScoreBox.SetPosition (m_Background.GetPosition() + coreVector2(0.0f,-0.05f));
    m_ScoreBox.SetSize     (coreVector2(m_Background.GetSize().x, 0.55f));
    m_ScoreBox.SetMaxOffset(0.05f * I_TO_F(MENU_SCORE_ENTRIES) - m_ScoreBox.GetSize().y);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aLine [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aRank [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aName [i]);
    for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) m_ScoreBox.BindObject(&m_aScore[i]);

    // 
    m_Navigator.BindObject(&m_BackButton, NULL, NULL, NULL, NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.AssignFirst(&m_BackButton);
    m_Navigator.AssignMenu(this);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_SCORE_MAX; ++i)
    {
        this->BindObject(i, &m_Background);
        this->BindObject(i, &m_DefaultTab);
        this->BindObject(i, &m_BackButton);
        this->BindObject(i, &m_Navigator);
    }

    for(coreUintW i = 0u; i < MENU_EXTRA_FILTERS; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aFilterLine[i]);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_ScoreBox);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aLine [i]);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aRank [i]);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aName [i]);
    //for(coreUintW i = 0u; i < MENU_SCORE_ENTRIES; ++i) this->BindObject(SURFACE_SCORE_DEFAULT, &m_aScore[i]);

    this->BindObject(SURFACE_SCORE_DEFAULT, &m_Mission);
    this->BindObject(SURFACE_SCORE_DEFAULT, &m_Segment);
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
            if(m_Mission.GetUserSwitch())
                this->LoadSegments(m_Mission.GetCurIndex());

            // 
            cMenu::UpdateSwitchBox(&m_Mission);
            cMenu::UpdateSwitchBox(&m_Segment);
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
    cMenu::UpdateButton(&m_DefaultTab, true);

    // 
    cMenu::UpdateButton(&m_BackButton, m_BackButton.IsFocused());

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_BackButton), TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}


// ****************************************************************
// 
void cScoreMenu::LoadMissions()
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
void cScoreMenu::LoadSegments(const coreUintW iIndex)
{
    // 
    const coreUintW iOldEntry = m_Segment.GetCurIndex();
    m_Segment.ClearEntries();
    
    const coreUintW iMissionIndex = iIndex - 1u;

    // 
    const coreChar* pcMissionLetter;
    switch(iMissionIndex)
    {
    case  9u: pcMissionLetter = "X1";                             break;
    case 10u: pcMissionLetter = "X2";                             break;
    default:  pcMissionLetter = coreData::ToChars(iMissionIndex); break;
    }


    // 
    if(iIndex)
    {
        m_Segment.AddEntry("Adventure", 0u);   // TODO 1: language
        for(coreUintW i = 0u; i < 7u; ++i)
        {
            
            // 
            const coreBool      bBoss = MISSION_SEGMENT_IS_BOSS(i);
            const coreChar* pcSegmentLetter = bBoss ? "B" : coreData::ToChars(i + 1u);
        
        
            m_Segment.AddEntry(PRINT("%s-%s", pcMissionLetter, pcSegmentLetter), 0u);
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