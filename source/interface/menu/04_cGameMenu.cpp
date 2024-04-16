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
: coreMenu             (SURFACE_GAME_MAX, SURFACE_GAME_OPTION)
, m_fMissionTime       (0.0f)
, m_aiSegmentSelection {}
, m_iCurPage           (0u)
, m_aiCurIndex         {coreUintW(-1), coreUintW(-1), coreUintW(-1)}
, m_aiCurIndex2        {coreUintW(-1), coreUintW(-1), coreUintW(-1)}
{
    constexpr coreFloat fSplit  = 1.0f/300.0f;   // 0.00333...
    constexpr coreFloat fSplit2 = 1.0f/60.0f;    // 0.01666...

    // create menu objects
    m_BackgroundMain.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundMain.DefineProgram("menu_border_program");
    m_BackgroundMain.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundMain.SetSize      (coreVector2(0.9f,0.75f));

    m_BackgroundOption.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundOption.DefineProgram("menu_border_program");
    m_BackgroundOption.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundOption.SetSize      (coreVector2(0.9f,0.25f));

    m_BackgroundArmory.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundArmory.DefineProgram("menu_border_program");
    m_BackgroundArmory.SetPosition  (coreVector2(0.0f,0.045f));
    m_BackgroundArmory.SetSize      (coreVector2(0.9f,0.82f));

    m_BackgroundDemo.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundDemo.DefineProgram("menu_border_program");
    m_BackgroundDemo.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundDemo.SetSize      (coreVector2(0.9f,0.48f));

    m_MissionTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_MissionTab.DefineProgram("menu_border_program");
    m_MissionTab.SetPosition  (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.15f - fSplit + fSplit2, -0.0125f));
    m_MissionTab.SetSize      (coreVector2(0.23f + fSplit + fSplit2*2.0f, 0.07f));
    m_MissionTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_MissionTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_MissionTab.SetTexOffset (m_MissionTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.15f - fSplit + fSplit2, 0.0125f));
    m_MissionTab.GetCaption()->SetTextLanguage("GAME_TAB_MISSION");

    m_SegmentTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_SegmentTab.DefineProgram("menu_border_program");
    m_SegmentTab.SetPosition  (m_MissionTab.GetPosition() * coreVector2(0.0f,1.0f));
    m_SegmentTab.SetSize      (m_MissionTab.GetSize());
    m_SegmentTab.SetAlignment (m_MissionTab.GetAlignment());
    m_SegmentTab.SetTexSize   (m_MissionTab.GetTexSize());
    m_SegmentTab.SetTexOffset (m_MissionTab.GetTexOffset() + coreVector2(m_SegmentTab.GetPosition().x - m_MissionTab.GetPosition().x, 0.0f));
    m_SegmentTab.GetCaption()->SetTextLanguage("GAME_TAB_SEGMENT");

    m_ArcadeTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArcadeTab.DefineProgram("menu_border_program");
    m_ArcadeTab.SetPosition  (m_MissionTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_ArcadeTab.SetSize      (m_MissionTab.GetSize());
    m_ArcadeTab.SetAlignment (m_MissionTab.GetAlignment());
    m_ArcadeTab.SetTexSize   (m_MissionTab.GetTexSize());
    m_ArcadeTab.SetTexOffset (m_MissionTab.GetTexOffset() + coreVector2(m_ArcadeTab.GetPosition().x - m_MissionTab.GetPosition().x, 0.0f));
    m_ArcadeTab.GetCaption()->SetTextLanguage("GAME_TAB_ARCADE");

    m_StartButtonArmory.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButtonArmory.DefineProgram("menu_border_program");
    m_StartButtonArmory.SetPosition  (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButtonArmory.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButtonArmory.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButtonArmory.GetCaption()->SetTextLanguage("START");

    m_StartButtonDemo.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButtonDemo.DefineProgram("menu_border_program");
    m_StartButtonDemo.SetPosition  (m_BackgroundDemo.GetPosition() + m_BackgroundDemo.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButtonDemo.SetSize      (m_StartButtonArmory.GetSize());
    m_StartButtonDemo.SetAlignment (m_StartButtonArmory.GetAlignment());
    m_StartButtonDemo.GetCaption()->SetTextLanguage("START");

    m_ContinueButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ContinueButton.DefineProgram("menu_border_program");
    m_ContinueButton.SetPosition  (m_BackgroundOption.GetPosition() + m_BackgroundOption.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_ContinueButton.SetSize      (m_StartButtonArmory.GetSize());
    m_ContinueButton.SetAlignment (m_StartButtonArmory.GetAlignment());
    m_ContinueButton.GetCaption()->SetTextLanguage("CONTINUE");

    m_BackButtonMain.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonMain.DefineProgram("menu_border_program");
    m_BackButtonMain.SetPosition  (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButtonMain.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButtonArmory.GetSize().y);
    m_BackButtonMain.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButtonMain.GetCaption()->SetText(ICON_SHARE);

    m_BackButtonOption.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonOption.DefineProgram("menu_border_program");
    m_BackButtonOption.SetPosition  (m_BackgroundOption.GetPosition() + m_BackgroundOption.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButtonOption.SetSize      (m_BackButtonMain.GetSize());
    m_BackButtonOption.SetAlignment (m_BackButtonMain.GetAlignment());
    m_BackButtonOption.GetCaption()->SetText(ICON_SHARE);

    m_BackButtonArmory.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonArmory.DefineProgram("menu_border_program");
    m_BackButtonArmory.SetPosition  (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButtonArmory.SetSize      (m_BackButtonMain.GetSize());
    m_BackButtonArmory.SetAlignment (m_BackButtonMain.GetAlignment());
    m_BackButtonArmory.GetCaption()->SetText(ICON_SHARE);

    m_BackButtonDemo.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonDemo.DefineProgram("menu_border_program");
    m_BackButtonDemo.SetPosition  (m_BackgroundDemo.GetPosition() + m_BackgroundDemo.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButtonDemo.SetSize      (m_BackButtonMain.GetSize());
    m_BackButtonDemo.SetAlignment (m_BackButtonMain.GetAlignment());
    m_BackButtonDemo.GetCaption()->SetText(ICON_SHARE);

    m_OptionState.Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_OptionState.SetPosition (m_BackButtonMain.GetPosition()  * coreVector2(-1.0f,1.0f));
    m_OptionState.SetAlignment(m_BackButtonMain.GetAlignment() * coreVector2(-1.0f,1.0f));
    m_OptionState.SetColor3   (COLOR_MENU_WHITE);

    m_MissionHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_MissionHeader.SetPosition    (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(-0.13f,-0.045f));
    m_MissionHeader.SetColor3      (COLOR_MENU_WHITE);
    m_MissionHeader.SetTextLanguage("GAME_HEADER_MISSION");

    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        const coreFloat fHeight = m_BackgroundMain.GetPosition().y + m_BackgroundMain.GetSize().y*0.5f + -0.1375f - 0.025f*I_TO_F(iOffset);

        m_aMissionName[i].Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aMissionName[i].SetPosition (coreVector2(m_BackgroundMain.GetPosition().x - m_BackgroundMain.GetSize().x*0.5f, fHeight) + coreVector2(0.07f,-0.013f));
        m_aMissionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aMissionName[i].SetColor3   (COLOR_MENU_WHITE);                              
        m_aMissionName[i].SetText     (g_aMissionData[i].pcName);

        m_aMissionTitle[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aMissionTitle[i].SetPosition (m_aMissionName[i].GetPosition() + coreVector2(0.0f,0.034f));
        m_aMissionTitle[i].SetAlignment(m_aMissionName[i].GetAlignment());
        m_aMissionTitle[i].SetColor3   (COLOR_MENU_WHITE);
        m_aMissionTitle[i].SetText     (PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetMissionLetters(i)));                           

        m_aMissionScore[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aMissionScore[i].SetPosition(coreVector2(-0.11f, fHeight));
        m_aMissionScore[i].SetColor3  (COLOR_MENU_WHITE);

        m_aMissionTime[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aMissionTime[i].SetPosition(coreVector2(0.04f, fHeight));
        m_aMissionTime[i].SetColor3  (COLOR_MENU_WHITE);

        m_aMissionFragment[i].DefineTexture(0u, "menu_fragment.png");
        m_aMissionFragment[i].DefineProgram("default_2d_program");
        m_aMissionFragment[i].SetPosition  (coreVector2(0.3f, fHeight));
        m_aMissionFragment[i].SetSize      (coreVector2(0.04f,0.04f));

        m_aMissionBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aMissionBadge[i].DefineProgram("default_2d_program");
        m_aMissionBadge[i].SetPosition  (coreVector2(0.19f, fHeight));
        m_aMissionBadge[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aMissionBadge[i].SetTexSize   (coreVector2(0.5f,1.0f));

        m_aMissionBadgeNum[i].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aMissionBadgeNum[i].SetPosition (m_aMissionBadge[i].GetPosition() + coreVector2(0.025f,0.0f));
        m_aMissionBadgeNum[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aMissionBadgeNum[i].SetColor3   (COLOR_MENU_WHITE);

        m_aMissionIcon[i].DefineTexture(0u, "menu_helper_small_08.png");
        m_aMissionIcon[i].DefineProgram("default_2d_program");
        m_aMissionIcon[i].SetPosition  (coreVector2(0.01f + m_aMissionName[i].GetPosition().x, fHeight));
        m_aMissionIcon[i].SetSize      (coreVector2(0.1f,0.1f));
        m_aMissionIcon[i].SetColor3    (g_aMissionData[i].vColor * 0.8f);
        m_aMissionIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aMissionIcon[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aMissionMedal[i].DefineTexture(0u, "menu_medal.png");
        m_aMissionMedal[i].DefineProgram("default_2d_program");
        m_aMissionMedal[i].SetPosition  (coreVector2(-0.01f - m_aMissionName[i].GetPosition().x, fHeight));
        m_aMissionMedal[i].SetSize      (coreVector2(0.08f,0.08f));
        m_aMissionMedal[i].SetTexSize   (coreVector2(0.25f,0.25f));

        m_aMissionLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aMissionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aMissionLine[i].DefineProgram("menu_inner_program");
        m_aMissionLine[i].SetPosition  (coreVector2(0.0f, fHeight));
        m_aMissionLine[i].SetSize      (coreVector2(m_BackgroundMain.GetSize().x, 0.125f));
        m_aMissionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aMissionLine[i].SetFocusable (true);

        iOffset += (i == 8u) ? 6u : 5u;
    }

    m_MissionBox.SetPosition (m_BackgroundMain.GetPosition() + coreVector2(0.0f,-0.025f));
    m_MissionBox.SetSize     (coreVector2(m_BackgroundMain.GetSize().x, 0.65f));
    m_MissionBox.SetMaxOffset(0.125f * I_TO_F(MENU_GAME_MISSIONS) + 0.025f - m_MissionBox.GetSize().y);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionLine    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionIcon    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionMedal   [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionFragment[i]);
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionBadge   [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionName    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionTitle   [i]);
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionScore   [i]);
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionTime    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aMissionBadgeNum[i]);

    m_SegmentHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_SegmentHeader.SetPosition    (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(-0.13f,-0.045f));
    m_SegmentHeader.SetColor3      (COLOR_MENU_WHITE);
    m_SegmentHeader.SetTextLanguage("GAME_HEADER_SEGMENT");

    iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        m_aSegmentName[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aSegmentName[i].SetPosition (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.055f, -0.1f - 0.025f*I_TO_F(iOffset)));
        m_aSegmentName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aSegmentName[i].SetColor3   (COLOR_MENU_WHITE);                                
        m_aSegmentName[i].SetText     (g_aMissionData[i].pcName);

        m_aSegmentNumber[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aSegmentNumber[i].SetPosition (m_aSegmentName[i].GetPosition() + coreVector2(0.165f,0.0f));
        m_aSegmentNumber[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aSegmentNumber[i].SetColor3   (COLOR_MENU_WHITE);
        m_aSegmentNumber[i].SetText     (cMenu::GetMissionLetters(i));

        m_aSegmentIconSmall[i].DefineTexture(0u, "menu_helper_small_12.png");
        m_aSegmentIconSmall[i].DefineProgram("default_2d_program");
        m_aSegmentIconSmall[i].SetPosition  (m_aSegmentName[i].GetPosition() + coreVector2(0.0f,0.0f));
        m_aSegmentIconSmall[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aSegmentIconSmall[i].SetDirection (coreVector2(1.0f,1.0f).Normalized());
        m_aSegmentIconSmall[i].SetColor3    (g_aMissionData[i].vColor * 0.8f);
        m_aSegmentIconSmall[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aSegmentIconSmall[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aSegmentLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aSegmentLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aSegmentLine[i].DefineProgram("menu_inner_program");
        m_aSegmentLine[i].SetPosition  (coreVector2(0.0f, m_aSegmentName[i].GetPosition().y));
        m_aSegmentLine[i].SetSize      (coreVector2(m_BackgroundMain.GetSize().x, 0.05f));
        m_aSegmentLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));

        iOffset += (i == 8u) ? 3u : 2u;
    }

#if !defined(_CORE_IDE_)

    coreUintW iCountBoss = 0u;
    coreUintW iCountWave = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            const coreBool    bBoss      = MISSION_SEGMENT_IS_BOSS(j);
            const coreVector2 vPos       = coreVector2((I_TO_F(j) - 3.5f) * 0.055f + (bBoss ? 0.03f : 0.0f), m_aSegmentName[i].GetPosition().y);
            const coreVector2 vSize      = bBoss ? coreVector2(0.1f,0.05f)                                   : coreVector2(0.05f,0.05f);
            const coreVector2 vStep      = bBoss ? coreVector2((1.0f/37.0f), (1.0f/21.0f) * (581.0f/512.0f)) : coreVector2((1.0f/41.0f), (1.0f/41.0f));
            const coreVector2 vTexSize   = bBoss ? coreVector2(8.0f,4.0f)                                    : coreVector2(4.0f,4.0f);
            const coreVector2 vTexOffset = bBoss ? coreVector2(I_TO_F(iCountBoss % 4u) * 9.0f + 1.0f, I_TO_F(iCountBoss / 4u) * 5.0f + 1.0f) :
                                                   coreVector2(I_TO_F(iCountWave % 8u) * 5.0f + 1.0f, I_TO_F(iCountWave / 8u) * 5.0f + 1.0f);

            m_aaSegmentTile[i][j].DefineTexture   (0u, bBoss ? "menu_segment_boss.png" : "menu_segment_wave.png");
            m_aaSegmentTile[i][j].DefineProgram   ("menu_grey_program");
            m_aaSegmentTile[i][j].SetPosition     (vPos + coreVector2(-0.011f,0.0f));
            m_aaSegmentTile[i][j].SetSize         (vSize * 0.9f);
            m_aaSegmentTile[i][j].SetColor3       (COLOR_MENU_WHITE);
            m_aaSegmentTile[i][j].SetTexSize      (vTexSize   * vStep);
            m_aaSegmentTile[i][j].SetTexOffset    (vTexOffset * vStep);
            m_aaSegmentTile[i][j].SetFocusable    (true);
            m_aaSegmentTile[i][j].SetFocusModifier(coreVector2(1.25f,1.25f));

            if(bBoss) iCountBoss += 1u;
                 else iCountWave += 1u;
        }
    }

#endif
    
    
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            const coreBool    bBoss      = MISSION_SEGMENT_IS_BOSS(j);
            const coreVector2 vPos       = coreVector2((I_TO_F(j) - 3.5f) * 0.055f + (bBoss ? 0.03f : 0.0f), m_aMissionLine[i].GetPosition().y);

            m_aaSegmentTile[i][j].SetPosition     (vPos + coreVector2(0.09f,0.055f * 0.5f) + (bBoss ? coreVector2(/*-0.1075f*/-0.055f * 3.5f - 0.0025f,-0.055f) : coreVector2(0.0f,0.0f)));
        }
    }
    
    
    
        for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS - 1u; ++j) m_MissionBox.BindObject(&m_aaSegmentTile[i][j]);   // waves
        for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_MissionBox.BindObject(&m_aaSegmentTile[i][MENU_GAME_SEGMENTS - 1u]);                                     // bosses

    m_aSegmentCursor[0].DefineProgram("menu_color_program");
    m_aSegmentCursor[0].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);
    m_aSegmentCursor[1].DefineProgram("menu_grey_program");

    m_SegmentArea.DefineTexture(0u, "menu_detail_04.png");
    m_SegmentArea.DefineTexture(1u, "menu_background_black.png");
    m_SegmentArea.DefineProgram("menu_inner_program");
    m_SegmentArea.SetPosition  (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(-0.15f,0.0f));
    m_SegmentArea.SetSize      (coreVector2(m_BackgroundMain.GetSize().y, 0.25f));
    m_SegmentArea.SetDirection (coreVector2(1.0f,0.0f));

    m_SegmentMedal.DefineTexture(0u, "menu_medal.png");
    m_SegmentMedal.DefineProgram("default_2d_program");
    m_SegmentMedal.SetPosition  (m_SegmentArea.GetPosition() + coreVector2(0.0f,-0.22f));
    m_SegmentMedal.SetSize      (coreVector2(0.1f,0.1f));
    m_SegmentMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    m_SegmentFragment.DefineTexture(0u, "menu_badge.png");
    m_SegmentFragment.DefineProgram("default_2d_program");
    m_SegmentFragment.SetPosition  (m_SegmentArea.GetPosition() + coreVector2(0.0f,0.0f));
    m_SegmentFragment.SetTexSize   (coreVector2(0.25f,0.25f));
    m_SegmentFragment.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);                    

    for(coreUintW i = 0u; i < BADGES; ++i)
    {
        m_aSegmentBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aSegmentBadge[i].DefineProgram("default_2d_program");
        m_aSegmentBadge[i].SetPosition  (m_SegmentMedal.GetPosition() + coreVector2(I_TO_F(i - 1u) * 0.05f, (i == 1u) ? -0.08f : -0.06f));
        m_aSegmentBadge[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aSegmentBadge[i].SetTexSize   (coreVector2(0.5f, 1.0f));

        m_aSegmentBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aSegmentBadgeWave[i].DefineProgram("menu_single_program");
        m_aSegmentBadgeWave[i].SetPosition  (m_aSegmentBadge[i].GetPosition());
        m_aSegmentBadgeWave[i].SetSize      (m_aSegmentBadge[i].GetSize() * 0.7f);
    }

    m_aSegmentTitle[0].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aSegmentTitle[0].SetPosition(m_SegmentArea.GetPosition() + coreVector2(0.0f,0.25f));
    m_aSegmentTitle[0].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentTitle[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentTitle[1].SetPosition(m_aSegmentTitle[0].GetPosition() + coreVector2(0.0f,0.04f));
    m_aSegmentTitle[1].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentScore[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aSegmentScore[0].SetPosition    (m_SegmentArea.GetPosition() + coreVector2(0.0f,0.12f));
    m_aSegmentScore[0].SetColor3      (COLOR_MENU_WHITE);
    m_aSegmentScore[0].SetTextLanguage("GAME_SCORE");

    m_aSegmentScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentScore[1].SetPosition(m_aSegmentScore[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aSegmentScore[1].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentTime[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aSegmentTime[0].SetPosition    (m_SegmentArea.GetPosition() + coreVector2(0.0f,0.02f));
    m_aSegmentTime[0].SetColor3      (COLOR_MENU_WHITE);
    m_aSegmentTime[0].SetTextLanguage("GAME_TIME");

    m_aSegmentTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentTime[1].SetPosition(m_aSegmentTime[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aSegmentTime[1].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentMaxSeries[0].Construct  (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aSegmentMaxSeries[0].SetPosition(m_SegmentArea.GetPosition() + coreVector2(0.0f,-0.08f));
    m_aSegmentMaxSeries[0].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentMaxSeries[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentMaxSeries[1].SetPosition(m_aSegmentMaxSeries[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aSegmentMaxSeries[1].SetColor3  (COLOR_MENU_WHITE);

    m_SegmentIcon.DefineTexture(0u, "menu_helper_big_06.png");
    m_SegmentIcon.DefineProgram("default_2d_program");
    m_SegmentIcon.SetPosition  (m_aSegmentTitle[0].GetPosition() + coreVector2(0.0f,0.0f));
    m_SegmentIcon.SetSize      (coreVector2(0.14f,0.14f));
    m_SegmentIcon.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_SegmentIcon.SetTexSize   (coreVector2(0.25f,0.25f));

    m_ArcadeHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArcadeHeader.SetPosition    (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_ArcadeHeader.SetColor3      (COLOR_MENU_WHITE);
    m_ArcadeHeader.SetTextLanguage("GAME_HEADER_ARCADE");

    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i)
    {
        m_aArcadeName[i].Construct   (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aArcadeName[i].SetPosition (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(-0.5f,0.0f) + coreVector2(0.04f, -0.15f*I_TO_F(i - 1u)));
        m_aArcadeName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aArcadeName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aArcadeLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aArcadeLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aArcadeLine[i].DefineProgram("menu_inner_program");
        m_aArcadeLine[i].SetPosition  (coreVector2(0.0f, m_aArcadeName[i].GetPosition().y));
        m_aArcadeLine[i].SetSize      (coreVector2(m_BackgroundMain.GetSize().x, 0.1f));
        m_aArcadeLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aArcadeLine[i].SetFocusable (true);
    }
    m_aArcadeName[0].SetTextLanguage("GAME_ARCADE_COMPLETE");
    m_aArcadeName[1].SetTextLanguage("UNKNOWN");
    m_aArcadeName[2].SetTextLanguage("UNKNOWN");

    m_ArcadeComplete.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArcadeComplete.SetPosition (coreVector2(-1.00f,1.00f) * m_aArcadeName[0].GetPosition());
    m_ArcadeComplete.SetSize     (coreVector2( 0.26f,0.05f));
    m_ArcadeComplete.SetAlignment(coreVector2(-1.00f,0.00f));
    m_ArcadeComplete.GetCaption()->SetTextLanguage("CONTINUE");

    m_OptionHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_OptionHeader.SetPosition    (m_BackgroundOption.GetPosition() + m_BackgroundOption.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_OptionHeader.SetColor3      (COLOR_MENU_WHITE);
    m_OptionHeader.SetTextLanguage("GAME_HEADER_OPTION");

    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i)
    {
        m_aOptionName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aOptionName[i].SetPosition (m_BackgroundOption.GetPosition() + m_BackgroundOption.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.1f - 0.05f*I_TO_F(i)));
        m_aOptionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aOptionName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aOptionLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aOptionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aOptionLine[i].DefineProgram("menu_inner_program");
        m_aOptionLine[i].SetPosition  (coreVector2(0.0f, m_aOptionName[i].GetPosition().y));
        m_aOptionLine[i].SetSize      (coreVector2(m_BackgroundOption.GetSize().x, 0.05f));
        m_aOptionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aOptionLine[i].SetFocusable (true);
    }
    m_aOptionName[0].SetTextLanguage("GAME_TYPE");
    m_aOptionName[1].SetTextLanguage("GAME_MODE");
    m_aOptionName[2].SetTextLanguage("GAME_DIFFICULTY");

    m_OptionType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_OptionType.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[0].GetPosition());
    m_OptionType.SetSize     (coreVector2( 0.47f,0.03f));
    m_OptionType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_OptionType.SetEndless  (true);
    m_OptionType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_OptionMode.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_OptionMode.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[1].GetPosition());
    m_OptionMode.SetSize     (m_OptionType.GetSize());
    m_OptionMode.SetAlignment(m_OptionType.GetAlignment());
    m_OptionMode.SetEndless  (true);
    m_OptionMode.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_OptionDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_OptionDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[2].GetPosition());
    m_OptionDifficulty.SetSize     (m_OptionType.GetSize());
    m_OptionDifficulty.SetAlignment(m_OptionType.GetAlignment());
    m_OptionDifficulty.SetEndless  (true);
    m_OptionDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArmoryHeader.SetPosition    (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_ArmoryHeader.SetColor3      (COLOR_MENU_WHITE);
    m_ArmoryHeader.SetTextLanguage("GAME_HEADER_ARMORY");

    iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES; ++i)
    {
        m_aArmoryName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryName[i].SetPosition (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.44f - 0.025f*I_TO_F(iOffset)));
        m_aArmoryName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aArmoryName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aArmoryLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aArmoryLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aArmoryLine[i].DefineProgram("menu_inner_program");
        m_aArmoryLine[i].SetPosition  (coreVector2(0.0f, m_aArmoryName[i].GetPosition().y));
        m_aArmoryLine[i].SetSize      (coreVector2(m_BackgroundArmory.GetSize().x, 0.05f));
        m_aArmoryLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aArmoryLine[i].SetFocusable (true);

        iOffset += (i == 2u) ? 3u : 2u;
    }
    m_aArmoryName[0].SetTextLanguage("GAME_TYPE");
    m_aArmoryName[1].SetTextLanguage("GAME_MODE");
    m_aArmoryName[2].SetTextLanguage("GAME_DIFFICULTY");
    m_aArmoryName[3].SetTextLanguage("CONFIG_GAME_GAMESPEED");
    m_aArmoryName[4].SetTextLanguage("GAME_SHIELD");
    m_aArmoryName[5].SetTextLanguage("GAME_WEAPON");

    m_ArmoryArea.DefineTexture(0u, "menu_detail_04.png");
    m_ArmoryArea.DefineTexture(1u, "menu_background_black.png");
    m_ArmoryArea.DefineProgram("menu_inner_program");
    m_ArmoryArea.SetPosition  (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.2305f));
    m_ArmoryArea.SetSize      (coreVector2(m_BackgroundArmory.GetSize().x, 0.32f));

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        m_aArmoryMedal[i].DefineTexture(0u, "menu_medal.png");
        m_aArmoryMedal[i].DefineProgram("default_2d_program");
        m_aArmoryMedal[i].SetPosition  (m_ArmoryArea.GetPosition() + (i ? coreVector2(-0.35f + I_TO_F(i - 1u) * 0.14f, -0.09f) : coreVector2(0.175f,0.074f)));
        m_aArmoryMedal[i].SetSize      (coreVector2(1.0f,1.0f) * (i ? ((i == 6u) ? 0.09f : 0.08f) : 0.11f));
        m_aArmoryMedal[i].SetTexSize   (coreVector2(0.25f,0.25f));

        //m_aArmoryMedalBack[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aArmoryMedalBack[i].DefineTexture(0u, Core::Manager::Resource->Load<coreTexture>("menu_detail_test2.png", CORE_RESOURCE_UPDATE_AUTO, "data/textures/menu_detail_test2.png"));
        m_aArmoryMedalBack[i].DefineTexture(0u, "menu_detail_01.png");
        m_aArmoryMedalBack[i].DefineTexture(0u, "menu_background_black.png");
        m_aArmoryMedalBack[i].DefineProgram("menu_border_program");
        //m_aArmoryMedalBack[i].DefineProgram("menu_free_program");
        //m_aArmoryMedalBack[i].DefineProgram("default_2d_program");
        m_aArmoryMedalBack[i].SetPosition  (m_aArmoryMedal[i].GetPosition());
       // m_aArmoryMedalBack[i].SetSize      (coreVector2(3.5f,0.45f) * 0.07f);
        //m_aArmoryMedalBack[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.05f);
        m_aArmoryMedalBack[i].SetColor3    (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aArmoryMedalBack[i].SetSize      (m_aArmoryMedal[i].GetSize() * 1.5f* 0.0f);
        //m_aArmoryMedalBack[i].SetSize      (coreVector2(3.5f,0.45f) * 0.07f);
        m_aArmoryMedalBack[i].SetDirection(coreVector2(1.0f,0.0f));
    }

    m_ArmoryFragment.DefineTexture(0u, "menu_fragment.png");
    m_ArmoryFragment.DefineProgram("default_2d_program");
    m_ArmoryFragment.SetSize      (coreVector2(0.05f,0.05f));
    m_ArmoryFragment.SetDirection (coreVector2(1.0f,1.0f).Normalized());

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        m_aArmoryBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aArmoryBadge[i].DefineProgram("default_2d_program");
        m_aArmoryBadge[i].SetTexSize   (coreVector2(0.5f,1.0f));

        m_aArmoryBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aArmoryBadgeWave[i].DefineProgram("menu_single_program");
        m_aArmoryBadgeWave[i].SetPosition  (m_aArmoryBadge[i].GetPosition());
        m_aArmoryBadgeWave[i].SetSize      (m_aArmoryBadge[i].GetSize() * 0.7f);
    }

    for(coreUintW i = 0u; i < BADGES; ++i)
    {
        m_aArmoryBadgeBack1[i].DefineTexture(0u, "menu_detail_03.png");
        m_aArmoryBadgeBack1[i].DefineTexture(1u, "menu_background_black.png");
        m_aArmoryBadgeBack1[i].DefineProgram("menu_free_program");
        m_aArmoryBadgeBack1[i].SetPosition  (m_ArmoryArea.GetPosition() + coreVector2(((i == 1u) ? -0.32f : -0.34f) - 0.05f, -0.014f + I_TO_F(i) * -0.05f));
        m_aArmoryBadgeBack1[i].SetSize      (coreVector2(7.0f + 2.0f, 0.5f) * 0.07f);
        m_aArmoryBadgeBack1[i].SetAlignment (coreVector2(1.0f,0.0f));
        m_aArmoryBadgeBack1[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);
        m_aArmoryBadgeBack1[i].SetTexSize   (coreVector2(0.5f,1.0f));
        m_aArmoryBadgeBack1[i].SetTexOffset (coreVector2(0.5f,0.0f));

        m_aArmoryBadgeBack2[i].DefineTexture(0u, "menu_detail_02.png");
        m_aArmoryBadgeBack2[i].DefineTexture(1u, "menu_background_black.png");
        m_aArmoryBadgeBack2[i].DefineProgram("menu_free_program");
        m_aArmoryBadgeBack2[i].SetPosition  (m_ArmoryArea.GetPosition() + coreVector2(0.0f, 0.01f + I_TO_F(i) * -0.05f));
        m_aArmoryBadgeBack2[i].SetSize      (coreVector2(7.0f + 5.0f, 0.5f) * 0.07f);
        m_aArmoryBadgeBack2[i].SetColor3    (m_aArmoryBadgeBack1[i].GetColor3());

        m_aArmoryBadgeDesc[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryBadgeDesc[i].SetPosition (m_aArmoryBadgeBack1[i].GetPosition() + coreVector2(0.05f,0.0f));
        m_aArmoryBadgeDesc[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aArmoryBadgeDesc[i].SetColor3   (COLOR_MENU_WHITE);
    }

    for(coreUintW i = 0u; i < MENU_GAME_SEGMENTS; ++i)
    {
        m_aArmoryBadgeNum[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aArmoryBadgeNum[i].SetPosition(m_aArmoryMedal[i + 1u].GetPosition() + coreVector2(0.0f, 0.055f * ((i % 2u) ? 1.0f : 1.0f)));
        m_aArmoryBadgeNum[i].SetColor3  (COLOR_MENU_WHITE);
        //m_aArmoryBadgeNum[i].SetText    (MISSION_SEGMENT_IS_BOSS(i) ? "B" : coreData::ToChars(i + 1u));
    }

    m_aArmoryTitle[0].Construct   (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(-0.36f,0.059f));
    m_aArmoryTitle[0].SetAlignment(coreVector2(1.0f,0.0f));
    m_aArmoryTitle[0].SetColor3   (COLOR_MENU_WHITE);

    m_aArmoryTitle[1].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryTitle[1].SetPosition (m_aArmoryTitle[0].GetPosition() + coreVector2(0.002f,0.044f));
    m_aArmoryTitle[1].SetAlignment(m_aArmoryTitle[0].GetAlignment());
    m_aArmoryTitle[1].SetColor3   (COLOR_MENU_WHITE);

    m_aArmoryScore[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    //m_aArmoryScore[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(-0.05f,0.101f));
    m_aArmoryScore[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(0.33f,0.11f));
    m_aArmoryScore[0].SetColor3      (COLOR_MENU_WHITE);
    m_aArmoryScore[0].SetTextLanguage("GAME_SCORE");

    m_aArmoryScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryScore[1].SetPosition(m_aArmoryScore[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryScore[1].SetColor3  (COLOR_MENU_WHITE);

    m_aArmoryTime[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    //m_aArmoryTime[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(0.15f,0.101f));
    m_aArmoryTime[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(0.33f,0.017f));
    m_aArmoryTime[0].SetColor3      (COLOR_MENU_WHITE);
    m_aArmoryTime[0].SetTextLanguage("GAME_TIME");

    m_aArmoryTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryTime[1].SetPosition(m_aArmoryTime[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryTime[1].SetColor3  (COLOR_MENU_WHITE);

    m_aArmoryMaxSeries[0].Construct  (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    //m_aArmoryMaxSeries[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(0.35f,0.101f));
    m_aArmoryMaxSeries[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(0.33f,-0.076f));
    m_aArmoryMaxSeries[0].SetColor3  (COLOR_MENU_WHITE);

    m_aArmoryMaxSeries[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryMaxSeries[1].SetPosition(m_aArmoryMaxSeries[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryMaxSeries[1].SetColor3  (COLOR_MENU_WHITE);

    m_ArmoryIcon.DefineTexture(0u, "menu_helper_big_06.png");
    m_ArmoryIcon.DefineProgram("default_2d_program");
    m_ArmoryIcon.SetPosition  (m_aArmoryTitle[0].GetPosition() + coreVector2(0.005f,0.018f));
    m_ArmoryIcon.SetSize      (coreVector2(0.115f,0.115f));
    m_ArmoryIcon.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_ArmoryIcon.SetTexSize   (coreVector2(0.25f,0.25f));

    m_ArmoryType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryType.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[0].GetPosition());
    m_ArmoryType.SetSize     (coreVector2( 0.47f,0.03f));
    m_ArmoryType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_ArmoryType.SetEndless  (true);
    m_ArmoryType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryMode.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryMode.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[1].GetPosition());
    m_ArmoryMode.SetSize     (m_ArmoryType.GetSize());
    m_ArmoryMode.SetAlignment(m_ArmoryType.GetAlignment());
    m_ArmoryMode.SetEndless  (true);
    m_ArmoryMode.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[2].GetPosition());
    m_ArmoryDifficulty.SetSize     (m_ArmoryType.GetSize());
    m_ArmoryDifficulty.SetAlignment(m_ArmoryType.GetAlignment());
    m_ArmoryDifficulty.SetEndless  (true);
    m_ArmoryDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmorySpeed.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmorySpeed.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[3].GetPosition());
    m_ArmorySpeed.SetSize     (m_ArmoryType.GetSize());
    m_ArmorySpeed.SetAlignment(m_ArmoryType.GetAlignment());
    m_ArmorySpeed.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aArmoryShield[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryShield[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[4].GetPosition() - vOffset);
        m_aArmoryShield[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aArmoryShield[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aArmoryShield[i].SetEndless  (true);
        m_aArmoryShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aArmoryWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryWeapon[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[5].GetPosition() - vOffset);
        m_aArmoryWeapon[i].SetSize     (m_aArmoryShield[i].GetSize());
        m_aArmoryWeapon[i].SetAlignment(m_aArmoryShield[i].GetAlignment());
        m_aArmoryWeapon[i].SetEndless  (true);
        m_aArmoryWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aArmoryPlayer[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aArmoryPlayer[i].SetPosition(m_aArmoryWeapon[i].GetPosition() - coreVector2(m_aArmoryWeapon[i].GetSize().x * 0.5f, 0.05f));
        m_aArmoryPlayer[i].SetColor3  (COLOR_MENU_WHITE);
    }

    m_DemoHeader.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_DemoHeader.SetPosition    (m_BackgroundDemo.GetPosition() + m_BackgroundDemo.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.06f));
    m_DemoHeader.SetColor3      (COLOR_MENU_WHITE);
    m_DemoHeader.SetTextLanguage("GAME_HEADER_DEMO");

    iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_DEMOS; ++i)
    {
        m_aDemoName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aDemoName[i].SetPosition (m_BackgroundDemo.GetPosition() + m_BackgroundDemo.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.13f - 0.025f*I_TO_F(iOffset)));
        m_aDemoName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aDemoName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aDemoLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aDemoLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aDemoLine[i].DefineProgram("menu_inner_program");
        m_aDemoLine[i].SetPosition  (coreVector2(0.0f, m_aDemoName[i].GetPosition().y));
        m_aDemoLine[i].SetSize      (coreVector2(m_BackgroundDemo.GetSize().x, 0.05f));
        m_aDemoLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aDemoLine[i].SetFocusable (true);

        iOffset += ((i == 0u) || (i == 3u)) ? 3u : 2u;
    }
    m_aDemoName[0].SetTextLanguage("GAME_TYPE");
    m_aDemoName[1].SetTextLanguage("GAME_DIFFICULTY");
    m_aDemoName[2].SetTextLanguage("CONFIG_GAME_GAMESPEED");
    m_aDemoName[3].SetTextLanguage("GAME_STAGE");
    m_aDemoName[4].SetTextLanguage("GAME_SHIELD");

    m_DemoType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_DemoType.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[0].GetPosition());
    m_DemoType.SetSize     (coreVector2( 0.47f,0.03f));
    m_DemoType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_DemoType.SetEndless  (true);
    m_DemoType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_DemoDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[1].GetPosition());
    m_DemoDifficulty.SetSize     (m_DemoType.GetSize());
    m_DemoDifficulty.SetAlignment(m_DemoType.GetAlignment());
    m_DemoDifficulty.SetEndless  (true);
    m_DemoDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoSpeed.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_DemoSpeed.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[2].GetPosition());
    m_DemoSpeed.SetSize     (m_DemoType.GetSize());
    m_DemoSpeed.SetAlignment(m_DemoType.GetAlignment());
    m_DemoSpeed.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoStage.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_DemoStage.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[3].GetPosition());
    m_DemoStage.SetSize     (m_DemoType.GetSize());
    m_DemoStage.SetAlignment(m_DemoType.GetAlignment());
    m_DemoStage.SetEndless  (true);
    m_DemoStage.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aDemoShield[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aDemoShield[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[4].GetPosition() - vOffset);
        m_aDemoShield[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aDemoShield[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aDemoShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aDemoPlayer[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aDemoPlayer[i].SetPosition(m_aDemoShield[i].GetPosition() - coreVector2(m_aDemoShield[i].GetSize().x * 0.5f, 0.05f));
        m_aDemoPlayer[i].SetColor3  (COLOR_MENU_WHITE);
    }

    // fill option entries       
    m_OptionType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_OptionType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_OptionType      .AddEntryLanguage("GAME_TYPE_DUEL",         GAME_TYPE_DUEL);
    m_OptionMode      .AddEntryLanguage("GAME_MODE_STANDARD",     GAME_MODE_STANDARD);
    m_OptionMode      .AddEntryLanguage("GAME_MODE_PACIFIST",     GAME_MODE_PACIFIST);
    m_OptionMode      .AddEntryLanguage("GAME_MODE_MASOCHIST",    GAME_MODE_MASOCHIST);
    m_OptionDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_OptionDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    m_OptionDifficulty.AddEntryLanguage("GAME_DIFFICULTY_HARD",   GAME_DIFFICULTY_HARD);
    m_ArmoryType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_ArmoryType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_ArmoryType      .AddEntryLanguage("GAME_TYPE_DUEL",         GAME_TYPE_DUEL);
    m_ArmoryMode      .AddEntryLanguage("GAME_MODE_STANDARD",     GAME_MODE_STANDARD);
    m_ArmoryMode      .AddEntryLanguage("GAME_MODE_PACIFIST",     GAME_MODE_PACIFIST);
    m_ArmoryMode      .AddEntryLanguage("GAME_MODE_MASOCHIST",    GAME_MODE_MASOCHIST);
    m_ArmoryDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_ArmoryDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    m_ArmoryDifficulty.AddEntryLanguage("GAME_DIFFICULTY_HARD",   GAME_DIFFICULTY_HARD);
    for(coreUintW i = 50u; i <= 200u; i += 5u) m_ArmorySpeed.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aArmoryShield[i].AddEntryLanguage("VALUE_NO",                           0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) for(coreUintW j = 10u; j <= 100u; j += 10u) m_aArmoryShield[i].AddEntry(PRINT("%zu", j), j);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aArmoryShield[i].AddEntry        (coreData::ToChars(SHIELD_INVINCIBLE), SHIELD_MAX);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aArmoryWeapon[i].AddEntryLanguage("GAME_WEAPON_RAY",                    1u);
    m_DemoType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_DemoType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_DemoDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_DemoDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    for(coreUintW i = 50u; i <= 200u; i += 5u) m_DemoSpeed.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aDemoShield[i].AddEntryLanguage("VALUE_NO",                           0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) for(coreUintW j = 10u; j <= 100u; j += 10u) m_aDemoShield[i].AddEntry(PRINT("%zu", j), j);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aDemoShield[i].AddEntry        (coreData::ToChars(SHIELD_INVINCIBLE), SHIELD_MAX);

    // 
    m_NavigatorMain.BindObject(&m_MissionTab,     &m_BackButtonMain, &m_ArcadeTab,  NULL, &m_SegmentTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_NavigatorMain.BindObject(&m_SegmentTab,     &m_BackButtonMain, &m_MissionTab, NULL, &m_ArcadeTab,  MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_NavigatorMain.BindObject(&m_ArcadeTab,      &m_BackButtonMain, &m_SegmentTab, NULL, &m_MissionTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_NavigatorMain.BindObject(&m_BackButtonMain, NULL,              NULL,          NULL, NULL,          MENU_TYPE_DEFAULT);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        m_NavigatorMain.BindObject(&m_aMissionLine[i], &m_aMissionLine[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS], NULL,
                                                       &m_aMissionLine[(i + 1u)                      % MENU_GAME_MISSIONS], NULL, MENU_TYPE_TAB_NODE, SURFACE_GAME_MISSION);
    }

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            m_NavigatorMain.BindObject(&m_aaSegmentTile[i][j], &m_aaSegmentTile[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS][j], &m_aaSegmentTile[i][(j + MENU_GAME_SEGMENTS - 1u) % MENU_GAME_SEGMENTS],
                                                               &m_aaSegmentTile[(i + 1u)                      % MENU_GAME_MISSIONS][j], &m_aaSegmentTile[i][(j + 1u)                      % MENU_GAME_SEGMENTS], MENU_TYPE_TAB_NODE, SURFACE_GAME_SEGMENT);
        }
    }

    m_NavigatorMain.BindSurface(&m_MissionTab, SURFACE_GAME_MISSION, &m_aMissionLine [MENU_GAME_MISSIONS - 1u],                          NULL, &m_aMissionLine[0],     NULL);
    m_NavigatorMain.BindSurface(&m_SegmentTab, SURFACE_GAME_SEGMENT, &m_aaSegmentTile[MENU_GAME_MISSIONS - 1u][MENU_GAME_SEGMENTS - 1u], NULL, &m_aaSegmentTile[0][0], NULL);
    m_NavigatorMain.BindSurface(&m_ArcadeTab,  SURFACE_GAME_ARCADE,  NULL,                                                               NULL, NULL,                   NULL);

    m_NavigatorMain.BindScroll(&m_MissionBox);

    m_NavigatorMain.AssignFirst(&m_aMissionLine[0]);
    m_NavigatorMain.AssignBack (&m_BackButtonMain);
    m_NavigatorMain.AssignMenu (this);

    m_NavigatorOption.BindObject(&m_OptionType,       &m_ContinueButton,   NULL,                &m_OptionMode,       NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorOption.BindObject(&m_OptionMode,       &m_OptionType,       NULL,                &m_OptionDifficulty, NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorOption.BindObject(&m_OptionDifficulty, &m_OptionMode,       NULL,                &m_ContinueButton,   NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorOption.BindObject(&m_ContinueButton,   &m_OptionDifficulty, &m_BackButtonOption, &m_OptionType,       &m_BackButtonOption, MENU_TYPE_DEFAULT);
    m_NavigatorOption.BindObject(&m_BackButtonOption, &m_OptionDifficulty, &m_ContinueButton,   &m_OptionType,       &m_ContinueButton,   MENU_TYPE_DEFAULT);

    m_NavigatorOption.AssignFirst(&m_ContinueButton);
    m_NavigatorOption.AssignBack (&m_BackButtonOption);
    m_NavigatorOption.AssignMenu (this);

    m_NavigatorArmory.BindObject(&m_ArmoryType,        &m_StartButtonArmory, NULL,                 &m_ArmoryMode,        NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmoryMode,        &m_ArmoryType,        NULL,                 &m_ArmoryDifficulty,  NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmoryDifficulty,  &m_ArmoryMode,        NULL,                 &m_ArmorySpeed,       NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmorySpeed,       &m_ArmoryDifficulty,  NULL,                 &m_aArmoryShield[0],  NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_aArmoryShield[0],  &m_ArmorySpeed,       &m_aArmoryShield[1],  &m_aArmoryWeapon[0],  &m_aArmoryShield[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_aArmoryShield[1],  &m_ArmorySpeed,       &m_aArmoryShield[0],  &m_aArmoryWeapon[1],  &m_aArmoryShield[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_aArmoryWeapon[0],  &m_aArmoryShield[0],  &m_aArmoryWeapon[1],  &m_StartButtonArmory, &m_aArmoryWeapon[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_aArmoryWeapon[1],  &m_aArmoryShield[1],  &m_aArmoryWeapon[0],  &m_StartButtonArmory, &m_aArmoryWeapon[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_StartButtonArmory, &m_aArmoryWeapon[0],  &m_BackButtonArmory,  &m_ArmoryType,        &m_BackButtonArmory,  MENU_TYPE_DEFAULT);
    m_NavigatorArmory.BindObject(&m_BackButtonArmory,  &m_aArmoryWeapon[0],  &m_StartButtonArmory, &m_ArmoryType,        &m_StartButtonArmory, MENU_TYPE_DEFAULT);

    m_NavigatorArmory.AssignFirst(&m_StartButtonArmory);
    m_NavigatorArmory.AssignBack (&m_BackButtonArmory);
    m_NavigatorArmory.AssignMenu (this);

    m_NavigatorDemo.BindObject(&m_DemoType,        &m_StartButtonDemo, NULL,               &m_DemoDifficulty,  NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_DemoDifficulty,  &m_DemoType,        NULL,               &m_DemoSpeed,       NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_DemoSpeed,       &m_DemoDifficulty,  NULL,               &m_DemoStage,       NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_DemoStage,       &m_DemoSpeed,       NULL,               &m_aDemoShield[0],  NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_aDemoShield[0],  &m_DemoStage,       &m_aDemoShield[1],  &m_StartButtonDemo, &m_aDemoShield[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorDemo.BindObject(&m_aDemoShield[1],  &m_DemoStage,       &m_aDemoShield[0],  &m_StartButtonDemo, &m_aDemoShield[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorDemo.BindObject(&m_StartButtonDemo, &m_aDemoShield[0],  &m_BackButtonDemo,  &m_DemoType,        &m_BackButtonDemo,  MENU_TYPE_DEFAULT);
    m_NavigatorDemo.BindObject(&m_BackButtonDemo,  &m_aDemoShield[0],  &m_StartButtonDemo, &m_DemoType,        &m_StartButtonDemo, MENU_TYPE_DEFAULT);

    m_NavigatorDemo.AssignFirst(&m_DemoType);
    m_NavigatorDemo.AssignBack (&m_BackButtonDemo);
    m_NavigatorDemo.AssignMenu (this);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_GAME_OPTION; ++i)
    {
        if(i != SURFACE_GAME_MISSION) this->BindObject(i, &m_MissionTab);
        if(i != SURFACE_GAME_SEGMENT) this->BindObject(i, &m_SegmentTab);
        //if(i != SURFACE_GAME_ARCADE)  this->BindObject(i, &m_ArcadeTab);

        this->BindObject(i, &m_BackgroundMain);

        if(i == SURFACE_GAME_MISSION) this->BindObject(i, &m_MissionTab);
        if(i == SURFACE_GAME_SEGMENT) this->BindObject(i, &m_SegmentTab);
        //if(i == SURFACE_GAME_ARCADE)  this->BindObject(i, &m_ArcadeTab);

        this->BindObject(i, &m_BackButtonMain);
        this->BindObject(i, &m_OptionState);
    }

    this->BindObject(SURFACE_GAME_MISSION, &m_MissionBox);
    this->BindObject(SURFACE_GAME_MISSION, &m_MissionHeader);

            this->BindObject(SURFACE_GAME_MISSION, &m_SegmentArea);
            this->BindObject(SURFACE_GAME_MISSION, &m_SegmentIcon);
            this->BindObject(SURFACE_GAME_MISSION, &m_SegmentMedal);
            this->BindObject(SURFACE_GAME_MISSION, &m_SegmentFragment);
            for(coreUintW i = 0u; i < BADGES;             ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aSegmentBadge[i]);
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentTitle);     ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aSegmentTitle    [i]);
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentScore);     ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aSegmentScore    [i]);
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentTime);      ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aSegmentTime     [i]);
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentMaxSeries); ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aSegmentMaxSeries[i]);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentLine[i]);
    this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentArea);
    this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentIcon);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS - 1u; ++j) this->BindObject(SURFACE_GAME_SEGMENT, &m_aaSegmentTile[i][j]);   // waves
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aaSegmentTile[i][MENU_GAME_SEGMENTS - 1u]);                                     // bosses
    this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentCursor[0]);
    this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentCursor[1]);
    this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentMedal);
    this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentFragment);
    for(coreUintW i = 0u; i < BADGES;             ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentBadgeWave[i]);
    for(coreUintW i = 0u; i < BADGES;             ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentBadge    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentIconSmall[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentName     [i]);
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentNumber   [i]);
    this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentHeader);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentTitle);     ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentTitle    [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentScore);     ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentScore    [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentTime);      ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentTime     [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentMaxSeries); ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentMaxSeries[i]);

    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i) this->BindObject(SURFACE_GAME_ARCADE, &m_aArcadeLine[i]);
    this->BindObject(SURFACE_GAME_ARCADE, &m_ArcadeHeader);
    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i) this->BindObject(SURFACE_GAME_ARCADE, &m_aArcadeName[i]);
    this->BindObject(SURFACE_GAME_ARCADE, &m_ArcadeComplete);

    for(coreUintW i = 0u; i < SURFACE_GAME_OPTION; ++i) this->BindObject(i, &m_NavigatorMain);

    this->BindObject(SURFACE_GAME_OPTION, &m_BackgroundOption);
    this->BindObject(SURFACE_GAME_OPTION, &m_ContinueButton);
    this->BindObject(SURFACE_GAME_OPTION, &m_BackButtonOption);
    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_OPTION, &m_aOptionLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) this->BindObject(SURFACE_GAME_OPTION, &m_aOptionName[i]);
    this->BindObject(SURFACE_GAME_OPTION, &m_OptionHeader);
    this->BindObject(SURFACE_GAME_OPTION, &m_OptionType);
    this->BindObject(SURFACE_GAME_OPTION, &m_OptionMode);
    this->BindObject(SURFACE_GAME_OPTION, &m_OptionDifficulty);
    this->BindObject(SURFACE_GAME_OPTION, &m_NavigatorOption);

    this->BindObject(SURFACE_GAME_ARMORY, &m_BackgroundArmory);
    this->BindObject(SURFACE_GAME_ARMORY, &m_StartButtonArmory);
    this->BindObject(SURFACE_GAME_ARMORY, &m_BackButtonArmory);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryArea);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryLine[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryIcon);
    for(coreUintW i = 0u; i < BADGES;                  ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeBack1[i]);
    for(coreUintW i = 0u; i < BADGES;                  ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeBack2[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeWave [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadge     [i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryFragment);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMedalBack[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMedal    [i]);
    for(coreUintW i = 0u; i < BADGES;                  ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeDesc[i]);
    for(coreUintW i = 0u; i < MENU_GAME_SEGMENTS;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeNum [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryName     [i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryHeader);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTitle);     ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryTitle    [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryScore);     ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryScore    [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTime);      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryTime     [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryMaxSeries); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMaxSeries[i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS;              ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryPlayer   [i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryType);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryMode);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryDifficulty);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmorySpeed);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryShield[i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryWeapon[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_NavigatorArmory);

    this->BindObject(SURFACE_GAME_DEMO, &m_BackgroundDemo);
    this->BindObject(SURFACE_GAME_DEMO, &m_StartButtonDemo);
    this->BindObject(SURFACE_GAME_DEMO, &m_BackButtonDemo);
    for(coreUintW i = 0u; i < MENU_GAME_DEMOS; ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_DEMOS; ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoName[i]);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoHeader);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoPlayer[i]);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoType);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoDifficulty);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoSpeed);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoStage);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoShield[i]);
    this->BindObject(SURFACE_GAME_DEMO, &m_NavigatorDemo);

    // 
    this->DeactivateFirstPlay();
}


// ****************************************************************
// move the game menu
void cGameMenu::Move()
{
    // 
    switch(this->GetCurSurface())
    {
    default: ASSERT(false)
    case SURFACE_GAME_MISSION: m_NavigatorMain  .Update(); break;
    case SURFACE_GAME_SEGMENT: m_NavigatorMain  .Update(); break;
    case SURFACE_GAME_ARCADE:  m_NavigatorMain  .Update(); break;
    case SURFACE_GAME_OPTION:  m_NavigatorOption.Update(); break;
    case SURFACE_GAME_ARMORY:  m_NavigatorArmory.Update(); break;
    case SURFACE_GAME_DEMO:    m_NavigatorDemo  .Update(); break;
    }

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
         if(m_MissionTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_GAME_MISSION);
    else if(m_SegmentTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_GAME_SEGMENT);
    else if(m_ArcadeTab .IsClicked()) cMenu::ChangeTab(this, SURFACE_GAME_ARCADE);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_GAME_MISSION:
        {
            if(m_BackButtonMain.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_OPTION, 3.0f, 2u, false, true);
            }
            
            m_fMissionTime.UpdateMod(1.0f, 2.0f*PI);

            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                if(!m_aMissionLine[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                if(m_aMissionLine[i].IsFocused())
                {
                    // 
                    this->__SelectMission(i);
                }

                if(m_aMissionLine[i].IsClicked())
                {
                    // 
                    m_iCurPage = 0u;

                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u, true, false);

                    // 
                    this->__PrepareMission(i);
                }

                cMenu::UpdateLine(&m_aMissionLine[i], false, g_aMissionData[i].vColor);

                m_aMissionName[i].SetColor3(g_aMissionData[i].vColor);

                m_aMissionIcon[i].SetAlpha    (m_aMissionIcon[i].GetAlpha() * 0.6f);
                m_aMissionIcon[i].SetDirection(coreVector2::Direction((m_aMissionLine[i].IsFocused() ? m_fMissionTime.ToFloat() : 0.0f) - (0.25f*PI)));
            }
        }
        break;

    case SURFACE_GAME_SEGMENT:
        {
            if(m_BackButtonMain.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_OPTION, 3.0f, 2u, false, true);
            }
            
            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
                {
                    if(m_aaSegmentTile[i][j].IsClicked())
                    {
                        // 
                        m_iCurPage = 1u;

                        // 
                        g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u, true, false);

                        // 
                        this->__PrepareSegment(i, j);
                    }

                    if(m_aaSegmentTile[i][j].IsFocused())
                    {
                        m_aiSegmentSelection[0] = i;
                        m_aiSegmentSelection[1] = j;

                        i = MENU_GAME_MISSIONS;
                        j = MENU_GAME_SEGMENTS;
                    }
                }
            }

            // 
            this->__SelectSegment(m_aiSegmentSelection[0], m_aiSegmentSelection[1]);
            
            const cGuiObject& oTile = m_aaSegmentTile[m_aiSegmentSelection[0]][m_aiSegmentSelection[1]];
            
            m_aSegmentCursor[0].SetPosition(oTile.GetPosition ());
            m_aSegmentCursor[0].SetSize    (oTile.GetSize     () * 1.75f + coreVector2(0.01f,0.01f));
            m_aSegmentCursor[0].Move();
            
            m_aSegmentCursor[1].DefineTexture(0u, oTile.GetTexture(0u));
            m_aSegmentCursor[1].SetPosition  (oTile.GetPosition ());
            m_aSegmentCursor[1].SetSize      (oTile.GetSize     () * 1.75f);
            m_aSegmentCursor[1].SetTexSize   (oTile.GetTexSize  ());
            m_aSegmentCursor[1].SetTexOffset (oTile.GetTexOffset());
            m_aSegmentCursor[1].Move();
            
            
            m_SegmentIcon.SetAlpha(m_SegmentIcon.GetAlpha() * 0.6f);
            
            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
                m_aSegmentIconSmall[i].SetAlpha(m_aSegmentIconSmall[i].GetAlpha() * 0.6f);


            const sMissionData& oData = g_aMissionData[m_aiSegmentSelection[0]];

            m_aSegmentCursor[1].SetColor3(oData.vColor);

            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                //m_aSegmentName[i].SetColor3((m_aiSegmentSelection[0] == i) ? g_aMissionData[i].vColor : COLOR_MENU_WHITE);
                m_aSegmentName[i].SetColor3(g_aMissionData[i].vColor);
                m_aSegmentLine[i].SetColor3((m_aiSegmentSelection[0] == i) ? g_aMissionData[i].vColor : coreVector3(1.0f,1.0f,1.0f));
            }
        }
        break;

    case SURFACE_GAME_ARCADE:
        {
            if(m_BackButtonMain.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_OPTION, 3.0f, 2u, false, true);
            }

            // 
            cMenu::UpdateButton(&m_ArcadeComplete, m_ArcadeComplete.IsFocused());
        }
        break;

    case SURFACE_GAME_OPTION:
        {
            if(m_ContinueButton.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_MISSION, 3.0f, 1u, false, true);
            }
            else if(m_BackButtonOption.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }

            // 
            cMenu::UpdateSwitchBox(&m_OptionType);
            cMenu::UpdateSwitchBox(&m_OptionMode);
            cMenu::UpdateSwitchBox(&m_OptionDifficulty);

            // 
            cMenu::UpdateButton(&m_ContinueButton,   m_ContinueButton  .IsFocused());
            cMenu::UpdateButton(&m_BackButtonOption, m_BackButtonOption.IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_GAME_OPTIONS; ++i) cMenu::UpdateLine(&m_aOptionLine[i], true);
        }
        break;

    case SURFACE_GAME_ARMORY:
        {
            if(m_StartButtonArmory.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 1;
            }
            else if(m_BackButtonArmory.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                if(g_pSave->GetHeader().oProgress.bFirstPlay)
                {
                    // 
                    m_iStatus = 2;
                }
                else
                {
                    // 
                    g_pMenu->ShiftSurface(this, this->GetOldSurface(), 3.0f, 2u, false, true);
                }
            }

            // 
            cMenu::UpdateSwitchBox(&m_ArmoryType);
            cMenu::UpdateSwitchBox(&m_ArmoryMode);
            cMenu::UpdateSwitchBox(&m_ArmoryDifficulty);
            cMenu::UpdateSwitchBox(&m_ArmorySpeed);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aArmoryShield[i]);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aArmoryWeapon[i]);

            // 
            cMenu::UpdateButton(&m_StartButtonArmory, m_StartButtonArmory.IsFocused());
            cMenu::UpdateButton(&m_BackButtonArmory,  m_BackButtonArmory .IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_GAME_ARMORIES; ++i) cMenu::UpdateLine(&m_aArmoryLine[i], true);

            // 
            m_aArmoryShield[0].GetCaption()->SetColor3(m_aArmoryShield[0].GetCurValue() ? COLOR_MENU_BLUE   : COLOR_MENU_WHITE);
            m_aArmoryShield[1].GetCaption()->SetColor3(m_aArmoryShield[1].GetCurValue() ? COLOR_MENU_YELLOW : COLOR_MENU_WHITE);

            const coreBool bMulti = (m_ArmoryType.GetCurValue() != GAME_TYPE_SOLO);
            m_aArmoryShield[1].SetOverride(bMulti ? 0 : -1);
            m_aArmoryWeapon[1].SetOverride(bMulti ? 0 : -1);
            m_aArmoryPlayer[1].SetColor3(COLOR_MENU_WHITE * (bMulti ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
            
            
            
            m_ArmoryIcon.SetAlpha(m_ArmoryIcon.GetAlpha() * 0.6f);
        }
        break;

    case SURFACE_GAME_DEMO:
        {
            if(m_StartButtonDemo.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 1;

                // 
                m_iCurPage      = 2u;
                m_aiCurIndex[0] = m_DemoStage.GetCurValue();
            }
            else if(m_BackButtonDemo.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }

            // 
            cMenu::UpdateSwitchBox(&m_DemoType);
            cMenu::UpdateSwitchBox(&m_DemoDifficulty);
            cMenu::UpdateSwitchBox(&m_DemoSpeed);
            cMenu::UpdateSwitchBox(&m_DemoStage);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aDemoShield[i]);

            // 
            cMenu::UpdateButton(&m_StartButtonDemo, m_StartButtonDemo.IsFocused());
            cMenu::UpdateButton(&m_BackButtonDemo,  m_BackButtonDemo .IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_GAME_DEMOS; ++i) cMenu::UpdateLine(&m_aDemoLine[i], true);

            // 
            m_aDemoShield[0].GetCaption()->SetColor3(m_aDemoShield[0].GetCurValue() ? COLOR_MENU_BLUE   : COLOR_MENU_WHITE);
            m_aDemoShield[1].GetCaption()->SetColor3(m_aDemoShield[1].GetCurValue() ? COLOR_MENU_YELLOW : COLOR_MENU_WHITE);

            // 
                 if(m_DemoStage.GetCurValue() == 0u) m_DemoStage.GetCaption()->SetColor3(g_aMissionData[0].vColor);
            else if(m_DemoStage.GetCurValue() == 1u) m_DemoStage.GetCaption()->SetColor3(g_aMissionData[1].vColor);
            else if(m_DemoStage.GetCurValue() == 2u) m_DemoStage.GetCaption()->SetColor3(g_aMissionData[2].vColor);
            else                                     m_DemoStage.GetCaption()->SetColor3(COLOR_MENU_WHITE);

            const coreBool bMulti = (m_DemoType.GetCurValue() != GAME_TYPE_SOLO);
            m_aDemoShield[1].SetOverride(bMulti ? 0 : -1);
            m_aDemoPlayer[1].SetColor3(COLOR_MENU_WHITE * (bMulti ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));

            const coreBool bUnlocked = (m_DemoStage.GetNumEntries() > 1u);
            m_DemoStage.SetOverride(bUnlocked ? 0 : -1);
            m_aDemoName[3].SetColor3(COLOR_MENU_WHITE * (bUnlocked ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if((this->GetCurSurface() == SURFACE_GAME_MISSION) || (this->GetCurSurface() == SURFACE_GAME_SEGMENT) || (this->GetCurSurface() == SURFACE_GAME_ARCADE))
    {
        // 
        cMenu::UpdateTab(&m_MissionTab, (this->GetCurSurface() == SURFACE_GAME_MISSION), m_MissionTab.IsFocused());
        cMenu::UpdateTab(&m_SegmentTab, (this->GetCurSurface() == SURFACE_GAME_SEGMENT), m_SegmentTab.IsFocused());
        cMenu::UpdateTab(&m_ArcadeTab,  (this->GetCurSurface() == SURFACE_GAME_ARCADE),  m_ArcadeTab .IsFocused());
    }

    // 
    cMenu::UpdateButton(&m_BackButtonMain, m_BackButtonMain.IsFocused());
}


// ****************************************************************
// 
void cGameMenu::ActivateFirstPlay()
{
}


// ****************************************************************
// 
void cGameMenu::DeactivateFirstPlay()
{
}


// ****************************************************************
// 
void cGameMenu::LoadValues()
{
    // 
    LoadConfig();

    // 
    coreUintW iMaxMission = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        const coreUint8 iAdvance = g_pSave->GetHeader().oProgress.aiAdvance[i] * ((i < 9u) ? 1u : 0u);

        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            const coreBool bEnabled = (iAdvance >= j + 1u) && ((i != 8u) || MISSION_SEGMENT_IS_BOSS(j));

            m_aaSegmentTile[i][j].SetEnabled  (bEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aaSegmentTile[i][j].SetFocusable(bEnabled);
        }

        if(iAdvance) iMaxMission = i;

        m_aMissionName [i].SetEnabled(iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aMissionTitle[i].SetEnabled(iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aMissionIcon [i].SetEnabled(iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aMissionLine [i].SetEnabled(iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aMissionLine [i].SetFocusable(iAdvance);

        m_aSegmentName     [i].SetEnabled(iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aSegmentIconSmall[i].SetEnabled(iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aSegmentLine     [i].SetEnabled(iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        cMenu::ApplyMedalTexture(&m_aMissionMedal[i], MAX(g_pSave->GetHeader().oProgress.aiMedalMission[i], DEFINED(_CORE_DEBUG_) ? MEDAL_BRONZE : MEDAL_NONE), MEDAL_TYPE_MISSION);

        const auto&     oStats     = g_pSave->GetHeader().aLocalStatsMission[i];
        const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
        const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

        m_aMissionScore   [i].SetText(coreData::ToChars(oStats.iScoreBest));
        m_aMissionTime    [i].SetText(PRINT("%.1f %+d", fBestTime, iBestShift));
        m_aMissionBadgeNum[i].SetText(PRINT("x %u", coreMath::PopCount(g_pSave->GetHeader().oProgress.aiBadge[i])));
    }

    m_MissionBox.SetMaxOffset(0.125f * I_TO_F(iMaxMission + 1u) - m_MissionBox.GetSize().y);

    // 
    m_aiSegmentSelection[0] = g_pSave->GetHeader().oOptions.iSegment / MENU_GAME_SEGMENTS;
    m_aiSegmentSelection[1] = g_pSave->GetHeader().oOptions.iSegment % MENU_GAME_SEGMENTS;

    // 
    m_ArmoryType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
    m_ArmoryMode      .SelectValue(g_pSave->GetHeader().oOptions.iMode);
    m_ArmoryDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
    m_ArmorySpeed     .SelectValue(g_CurConfig.Game.iGameSpeed);

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        m_aArmoryShield[i].SelectValue(g_pSave->EditOptions()->aiShield [i]);
        m_aArmoryWeapon[i].SelectValue(g_pSave->EditOptions()->aaiWeapon[i][0]);
    
        m_aArmoryPlayer[i].SetText(PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
    }

    if(g_bDemoVersion)
    {
        m_DemoStage.ClearEntries();
        m_DemoStage.AddEntry(PRINT("%s 0", Core::Language->GetString("MISSION")), 0u);
        if(!g_pSave->GetHeader().oProgress.bFirstPlay)
        {
            m_DemoStage.AddEntry(PRINT("%s 1", Core::Language->GetString("MISSION")), 1u);
            m_DemoStage.AddEntry(PRINT("%s 2", Core::Language->GetString("MISSION")), 2u);
        }

        // 
        m_DemoType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
        m_DemoDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
        m_DemoSpeed     .SelectValue(g_CurConfig.Game.iGameSpeed);
        m_DemoStage     .SelectValue(g_pSave->GetHeader().oOptions.iMission);

        // 
        for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
        {
            m_aDemoShield[i].SelectValue(g_pSave->GetHeader().oOptions.aiShield[i]);
    
            m_aDemoPlayer[i].SetText(PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
        }
    }
}


// ****************************************************************
// 
void cGameMenu::SaveValues()
{
    // 
    g_pSave->EditOptions()->iMission = 0u;
    g_pSave->EditOptions()->iSegment = m_aiSegmentSelection[0] * MENU_GAME_SEGMENTS + m_aiSegmentSelection[1];

    // 
    g_pSave->EditOptions()->iType       = m_ArmoryType      .GetCurValue();
    g_pSave->EditOptions()->iMode       = m_ArmoryMode      .GetCurValue();
    g_pSave->EditOptions()->iDifficulty = m_ArmoryDifficulty.GetCurValue();
    g_CurConfig.Game.iGameSpeed         = m_ArmorySpeed     .GetCurValue();

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        g_pSave->EditOptions()->aiShield [i]    = m_aArmoryShield[i].GetCurValue();
        g_pSave->EditOptions()->aaiWeapon[i][0] = m_aArmoryWeapon[i].GetCurValue();
    }

    // 
    m_OptionState.SetText(PRINT("%s / %s", m_OptionType.GetCurText(), m_OptionDifficulty.GetCurText()));
    
    
    if(g_bDemoVersion)
    {
        // 
        g_pSave->EditOptions()->iType       = m_DemoType      .GetCurValue();
        g_pSave->EditOptions()->iDifficulty = m_DemoDifficulty.GetCurValue();
        g_CurConfig.Game.iGameSpeed         = m_DemoSpeed     .GetCurValue();
        g_pSave->EditOptions()->iMission    = m_DemoStage     .GetCurValue();
        for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) g_pSave->EditOptions()->aiShield[i] = m_aDemoShield[i].GetCurValue();

        // 
        m_ArmoryType      .SelectValue(m_DemoType      .GetCurValue());
        m_ArmoryMode      .SelectValue(0u);
        m_ArmoryDifficulty.SelectValue(m_DemoDifficulty.GetCurValue());
        m_ArmorySpeed     .SelectValue(m_DemoStage     .GetCurValue());
        for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aArmoryShield[i].SelectValue(m_aDemoShield[i].GetCurValue());
    }

    // 
    InitFramerate();

    // 
    SaveConfig();
}


// ****************************************************************
// 
void cGameMenu::RetrieveStartData(coreInt32* OUTPUT piMissionID, coreUint8* OUTPUT piTakeFrom, coreUint8* OUTPUT piTakeTo, coreUint8* OUTPUT piKind)const
{
    if(g_pSave->GetHeader().oProgress.bFirstPlay)
    {
        // 
        (*piMissionID) = cIntroMission::ID;
        (*piTakeFrom)  = 0u;
        (*piTakeTo)    = TAKE_MISSION;
        (*piKind)      = GAME_KIND_ALL;
    }
    else if(m_iCurPage == 0u)
    {
        // 
        (*piMissionID) = g_aMissionData[m_aiCurIndex[0]].iID;
        (*piTakeFrom)  = 0u;
        (*piTakeTo)    = TAKE_MISSION;
        (*piKind)      = GAME_KIND_MISSION;
    }
    else if(m_iCurPage == 1u)
    {
        // 
        (*piMissionID) = g_aMissionData[m_aiCurIndex[1]].iID;
        (*piTakeFrom)  = m_aiCurIndex[2];
        (*piTakeTo)    = m_aiCurIndex[2];
        (*piKind)      = GAME_KIND_SEGMENT;
    }
    else if(m_iCurPage == 2u)
    {
        // 
        (*piMissionID) = g_aMissionData[m_aiCurIndex[0]].iID;
        (*piTakeFrom)  = 0u;
        (*piTakeTo)    = TAKE_MISSION;
        (*piKind)      = GAME_KIND_ALL;
    }
    else ASSERT(false)
}


// ****************************************************************
// 
void cGameMenu::__SelectMission(const coreUintW iMissionIndex)
{
    ASSERT(iMissionIndex < MISSIONS)

    // 
    if(m_aiCurIndex[0] == iMissionIndex) return;
    m_aiCurIndex[0] = iMissionIndex;
    
    m_fMissionTime = 0.0f;

    // 
    //const sMissionData& oData = g_aMissionData[iMissionIndex];

    // 
    //const auto&     oStats     = g_pSave->GetHeader().aLocalStatsMission[iMissionIndex];
    //const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    //const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    //m_aStandardName [0].SetColor3(oData.vColor);
    //m_aStandardScore[0].SetColor3(oData.vColor);
    //m_aStandardTime [0].SetColor3(oData.vColor);

    // 
    //m_aStandardName [0].SetText(oData.pcName);
    //m_aStandardName [1].SetText(PRINT("%s %zu", Core::Language->GetString("MISSION"), iMissionIndex));
    //m_aStandardScore[1].SetText(coreData::ToChars(oStats.iScoreBest));
    //m_aStandardTime [1].SetText(PRINT("%.1f %+d", fBestTime, iBestShift));

    // 
    //cMenu::ApplyMedalTexture(&m_StandardMedal, g_pSave->GetHeader().oProgress.aiMedalMission[iMissionIndex], MEDAL_TYPE_MISSION);
}


// ****************************************************************
// 
void cGameMenu::__SelectSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
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
    const auto&     oStats     = g_pSave->GetHeader().aaLocalStatsSegment[iMissionIndex][iSegmentIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    m_aSegmentTitle    [0].SetColor3(oData.vColor);
    m_aSegmentScore    [0].SetColor3(oData.vColor);
    m_aSegmentTime     [0].SetColor3(oData.vColor);
    m_aSegmentMaxSeries[0].SetColor3(oData.vColor);

    // 
    m_aSegmentTitle    [0].SetText(bBoss ? oData.pcBoss : oData.pcName);
    m_aSegmentTitle    [1].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aSegmentScore    [1].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)                                                                                    : "-");
    m_aSegmentTime     [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift)                                                                                : "-");
    m_aSegmentMaxSeries[1].SetText(oStats.iMaxSeries ? (bBoss ? coreData::ToChars(oStats.iMaxSeries) : PRINT("x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u)) : "-");

    // 
    m_aSegmentMaxSeries[0].SetTextLanguage(bBoss ? "GAME_CHAIN" : "GAME_COMBO");

    // 
    cMenu::ApplyMedalTexture(&m_SegmentMedal, g_pSave->GetHeader().oProgress.aaiMedalSegment[iMissionIndex][iSegmentIndex], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);

    // 
    for(coreUintW i = 0u; i < BADGES; ++i)
    {
        const coreBool bState = HAS_BIT(g_pSave->GetHeader().oProgress.aiBadge[iMissionIndex], i + BADGES * iSegmentIndex);

        m_aSegmentBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
        m_aSegmentBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));

        m_aSegmentBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_SegmentIcon.SetColor3   (oData.vColor * 0.8f);
    m_SegmentIcon.SetTexOffset(oData.vIcon);
}


// ****************************************************************
// 
void cGameMenu::__PrepareMission(const coreUintW iMissionIndex)
{
    ASSERT(iMissionIndex < MISSIONS)

    // 
    if((m_aiCurIndex2[1] == iMissionIndex) && (m_aiCurIndex2[2] == coreUintW(-1))) return;
    m_aiCurIndex2[1] = iMissionIndex;
    m_aiCurIndex2[2] = coreUintW(-1);
    
    
    
    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];

    // 
    const auto&     oStats     = g_pSave->GetHeader().aLocalStatsMission[iMissionIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    m_aArmoryTitle    [0].SetColor3(oData.vColor);
    m_aArmoryScore    [0].SetColor3(oData.vColor);
    m_aArmoryTime     [0].SetColor3(oData.vColor);
    m_aArmoryMaxSeries[0].SetColor3(oData.vColor);

    // 
    m_aArmoryTitle    [0].SetText(oData.pcName);
    m_aArmoryTitle    [1].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetMissionLetters(iMissionIndex)));
    m_aArmoryScore    [1].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)                                   : "-");
    m_aArmoryTime     [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift)                               : "-");
    m_aArmoryMaxSeries[1].SetText(oStats.iMaxSeries ? PRINT("x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u) : "-");

    // 
    //m_aArmoryMaxSeries[0].SetTextLanguage("GAME_COMBO");

    // 
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], g_pSave->GetHeader().oProgress.aiMedalMission[iMissionIndex], MEDAL_TYPE_MISSION);
    
    for(coreUintW i = 1; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        const coreBool bBoss = MISSION_SEGMENT_IS_BOSS(i - 1u);

        cMenu::ApplyMedalTexture(&m_aArmoryMedal[i], g_pSave->GetHeader().oProgress.aaiMedalSegment[iMissionIndex][i - 1u], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);
    }

    // 
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        const coreBool bState = HAS_BIT(g_pSave->GetHeader().oProgress.aiBadge[iMissionIndex], i);

        m_aArmoryBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
        m_aArmoryBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));

        m_aArmoryBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
    

    // 
    m_ArmoryIcon.SetColor3   (oData.vColor * 0.8f);
    m_ArmoryIcon.SetTexOffset(oData.vIcon);
    
    
    // 
    for(coreUintW i = 1;      i < MENU_GAME_ARMORY_MEDALS;        ++i) m_aArmoryMedal     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = BADGES; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadge     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //for(coreUintW i = BADGES; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeWave [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;     i < BADGES;                         ++i) m_aArmoryBadgeBack1[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;     i < BADGES;                         ++i) m_aArmoryBadgeBack2[i].SetEnabled((i == 2u) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;     i < BADGES;                         ++i) m_aArmoryBadgeDesc [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;     i < MENU_GAME_SEGMENTS;             ++i) m_aArmoryBadgeNum  [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;     i < ARRAY_SIZE(m_aArmoryMaxSeries); ++i) m_aArmoryMaxSeries [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    //for(coreUintW i = 0u;     i < ARRAY_SIZE(m_aArmoryScore); ++i) m_aArmoryScore [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    //for(coreUintW i = 0u;     i < ARRAY_SIZE(m_aArmoryTime); ++i) m_aArmoryTime [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    
    //m_aArmoryMedal[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(0.175f,0.074f));
        
    //m_aArmoryScore[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(0.33f,0.12f));
    //m_aArmoryScore[1].SetPosition(m_aArmoryScore[0].GetPosition() + coreVector2(0.0f,-0.034f));
    
    //m_aArmoryTime[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(0.33f,0.047f));
    //m_aArmoryTime[1].SetPosition(m_aArmoryTime[0].GetPosition() + coreVector2(0.0f,-0.034f));
    

    m_aArmoryMedal[0].SetPosition(coreVector2(m_aArmoryMedal[MENU_GAME_ARMORY_MEDALS - 1u].GetPosition().x, m_ArmoryArea.GetPosition().y + 0.06f));
    
        m_aArmoryMedalBack[0].SetPosition  (m_aArmoryMedal[0].GetPosition());
        //m_aArmoryMedalBack[0].SetSize      (m_aArmoryMedal[0].GetSize());
    
    m_aArmoryScore[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(-0.03f,0.08f));
    m_aArmoryScore[1].SetPosition(m_aArmoryScore[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryTime[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(0.17f,0.08f));
    m_aArmoryTime[1].SetPosition(m_aArmoryTime[0].GetPosition() + coreVector2(0.0f,-0.034f));
    

    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(-0.28f,0.059f));
    m_aArmoryTitle[0].SetAlignment(coreVector2(0.0f,0.0f));

    m_aArmoryTitle[1].SetPosition (m_aArmoryTitle[0].GetPosition() + coreVector2(0.00f,0.044f));
    m_aArmoryTitle[1].SetAlignment(m_aArmoryTitle[0].GetAlignment());
    
    m_ArmoryIcon.SetPosition  (m_aArmoryTitle[0].GetPosition() + coreVector2(0.00f,0.0f));
    m_ArmoryIcon.SetSize      (coreVector2(0.15f,0.15f));
    
    
    
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        m_aArmoryBadge[i].SetPosition(m_aArmoryBadgeDesc[i % BADGES].GetPosition() + coreVector2(-0.0f + I_TO_F(i / BADGES) * 0.11f, 0.0f));
        m_aArmoryBadge[i].SetPosition(m_aArmoryMedal[(i / BADGES) + 1u].GetPosition() + coreVector2(I_TO_F((i % BADGES) - 1u) * 0.05f, (((i % BADGES) == 1u) ? -0.08f : -0.06f) * (((i / BADGES) % 2u) ? -1.0f : -1.0f)) * 0.8f);
        m_aArmoryBadge[i].SetSize    (coreVector2(0.05f,0.05f));
        
        m_aArmoryBadgeWave[i].SetPosition(m_aArmoryBadge[i].GetPosition());
        m_aArmoryBadgeWave[i].SetSize    (m_aArmoryBadge[i].GetSize() * 0.7f);
    }
    
    
    m_ArmoryFragment.SetPosition(m_aArmoryMedal[6].GetPosition() + coreVector2(0.0f,0.05f));
    //m_ArmoryFragment.SetPosition(m_aArmoryMaxSeries[1].GetPosition());
    m_ArmoryFragment.SetEnabled(CORE_OBJECT_ENABLE_ALL);
}


// ****************************************************************
// 
void cGameMenu::__PrepareSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT((iMissionIndex < MISSIONS) && (iSegmentIndex < SEGMENTS))

    // 
    if((m_aiCurIndex2[1] == iMissionIndex) && (m_aiCurIndex2[2] == iSegmentIndex)) return;
    m_aiCurIndex2[1] = iMissionIndex;
    m_aiCurIndex2[2] = iSegmentIndex;

    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];
    const coreBool      bBoss = MISSION_SEGMENT_IS_BOSS(iSegmentIndex);

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaLocalStatsSegment[iMissionIndex][iSegmentIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    m_aArmoryTitle    [0].SetColor3(oData.vColor);
    m_aArmoryScore    [0].SetColor3(oData.vColor);
    m_aArmoryTime     [0].SetColor3(oData.vColor);
    m_aArmoryMaxSeries[0].SetColor3(oData.vColor);

    // 
    m_aArmoryTitle    [0].SetText(bBoss ? oData.pcBoss : oData.pcName);
    m_aArmoryTitle    [1].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aArmoryScore    [1].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)                                                                                    : "-");
    m_aArmoryTime     [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift)                                                                                : "-");
    m_aArmoryMaxSeries[1].SetText(oStats.iMaxSeries ? (bBoss ? coreData::ToChars(oStats.iMaxSeries) : PRINT("x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u)) : "-");

    // 
    m_aArmoryMaxSeries[0].SetTextLanguage(bBoss ? "GAME_CHAIN" : "GAME_COMBO");

    // 
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], g_pSave->GetHeader().oProgress.aaiMedalSegment[iMissionIndex][iSegmentIndex], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);

    // 
    for(coreUintW i = 0u; i < BADGES; ++i)
    {
        const coreBool bState = HAS_BIT(g_pSave->GetHeader().oProgress.aiBadge[iMissionIndex], i + BADGES * iSegmentIndex);

        m_aArmoryBadge    [i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
        m_aArmoryBadge    [i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
        m_aArmoryBadgeDesc[i].SetTextLanguage("UNKNOWN");

        m_aArmoryBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
    m_aArmoryBadgeDesc[0].SetText("Destroy all broken shields");
    m_aArmoryBadgeDesc[1].SetText("Lorem ipsum dolor sit amet");
    m_aArmoryBadgeDesc[2].SetText("Consectetur adipiscing elit");
    // erstes is immer sichtbar, zweites wird sichtbar wenn erstes erlangt wurde, drittes wird sichtbar wenn BEIDE anderen erlangt wurden

    // 
    for(coreUintW i = 1;      i < MENU_GAME_ARMORY_MEDALS;        ++i) m_aArmoryMedal     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = BADGES; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadge     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = BADGES; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeWave [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;     i < BADGES;                         ++i) m_aArmoryBadgeBack1[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;     i < BADGES;                         ++i) m_aArmoryBadgeBack2[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;     i < BADGES;                         ++i) m_aArmoryBadgeDesc [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;     i < MENU_GAME_SEGMENTS;             ++i) m_aArmoryBadgeNum  [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;     i < ARRAY_SIZE(m_aArmoryMaxSeries); ++i) m_aArmoryMaxSeries [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    

    for(coreUintW i = 0u; i < BADGES; ++i)
    {
        m_aArmoryBadge[i].SetPosition(m_aArmoryBadgeDesc[i].GetPosition() + coreVector2(-0.05f,0.0f));
        m_aArmoryBadge[i].SetSize    (coreVector2(0.06f,0.06f));
        
        m_aArmoryBadgeWave[i].SetPosition(m_aArmoryBadge[i].GetPosition());
        m_aArmoryBadgeWave[i].SetSize    (m_aArmoryBadge[i].GetSize() * 0.7f);
    }
    
    

    // 
    m_ArmoryIcon.SetColor3   (oData.vColor * 0.8f);
    m_ArmoryIcon.SetTexOffset(oData.vIcon);
    
    
    
    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(-0.36f,0.059f));
    m_aArmoryTitle[0].SetAlignment(coreVector2(1.0f,0.0f));
    m_aArmoryTitle[1].SetPosition (m_aArmoryTitle[0].GetPosition() + coreVector2(0.002f,0.044f));
    m_aArmoryTitle[1].SetAlignment(m_aArmoryTitle[0].GetAlignment());
    m_aArmoryScore[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(0.33f,0.11f));
    m_aArmoryScore[1].SetPosition(m_aArmoryScore[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryTime[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(0.33f,0.017f));
    m_aArmoryTime[1].SetPosition(m_aArmoryTime[0].GetPosition() + coreVector2(0.0f,-0.034f));
    

    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(-0.28f,0.059f));
    m_aArmoryTitle[0].SetAlignment(coreVector2(0.0f,0.0f));

    m_aArmoryTitle[1].SetPosition (m_aArmoryTitle[0].GetPosition() + coreVector2(0.00f,0.044f));
    m_aArmoryTitle[1].SetAlignment(m_aArmoryTitle[0].GetAlignment());
    
    m_ArmoryIcon.SetPosition  (m_aArmoryTitle[0].GetPosition() + coreVector2(0.00f,0.0f));
    m_ArmoryIcon.SetSize      (coreVector2(0.17f,0.17f));
    
    
    //m_ArmoryIcon.SetPosition  (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.0f));
    //m_ArmoryIcon.SetSize      (coreVector2(0.27f,0.27f)    * 0.0f);
    
    
    m_aArmoryMedal[0].SetPosition  (m_ArmoryArea.GetPosition() + coreVector2(0.155f,0.074f));
    for(coreUintW i = 1u;     i < MENU_GAME_ARMORY_MEDALS;             ++i) m_aArmoryMedalBack  [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
        m_aArmoryMedalBack[0].SetPosition  (m_aArmoryMedal[0].GetPosition());
        m_aArmoryMedalBack[0].SetSize      (m_aArmoryMedal[0].GetSize() * 1.05f);
    
    //m_ArmoryIcon.SetPosition  (m_aArmoryMedal[0].GetPosition() + coreVector2(0.00f,0.0f));

    
    m_ArmoryFragment.SetEnabled(bBoss ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
}