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
: coreMenu             (SURFACE_GAME_MAX, SURFACE_GAME_FIRST)
, m_aiSegmentSelection {}
, m_fSegmentTime       (0.0f)
, m_iCurPage           (0u)
, m_aiCurIndex         {coreUintW(-1), coreUintW(-1), coreUintW(-1), 0u}
, m_iBaseType          (0u)
, m_iBaseMode          (0u)
, m_iBaseDifficulty    (0u)
, m_bFirstPlay         (false)
{
    // create menu objects
    m_BackgroundMain.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundMain.DefineProgram("menu_border_program");
    m_BackgroundMain.SetPosition  (coreVector2(0.0f,0.045f));
    m_BackgroundMain.SetSize      (coreVector2(0.9f,0.82f));

    m_BackgroundOption.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundOption.DefineProgram("menu_border_program");
    m_BackgroundOption.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundOption.SetSize      (coreVector2(0.9f,0.25f));

    m_BackgroundArmory.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundArmory.DefineProgram("menu_border_program");
    m_BackgroundArmory.SetPosition  (coreVector2(0.0f,0.045f));
    m_BackgroundArmory.SetSize      (coreVector2(0.9f,0.82f));

    m_BackgroundFirst.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundFirst.DefineProgram("menu_border_program");
    m_BackgroundFirst.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundFirst.SetSize      (coreVector2(0.9f,0.43f));

    m_BackgroundDemo.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundDemo.DefineProgram("menu_border_program");
    m_BackgroundDemo.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundDemo.SetSize      (coreVector2(0.9f,0.48f));

    m_MissionTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_MissionTab.DefineProgram("menu_border_program");
    m_MissionTab.SetPosition  (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.235f,-0.0125f));
    m_MissionTab.SetSize      (coreVector2(0.41f,0.07f));
    m_MissionTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_MissionTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_MissionTab.SetTexOffset (m_MissionTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.235f,0.0125f));
    m_MissionTab.GetCaption()->SetTextLanguage("GAME_TAB_MISSION");

    m_SegmentTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_SegmentTab.DefineProgram("menu_border_program");
    m_SegmentTab.SetPosition  (m_MissionTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_SegmentTab.SetSize      (m_MissionTab.GetSize());
    m_SegmentTab.SetAlignment (m_MissionTab.GetAlignment());
    m_SegmentTab.SetTexSize   (m_MissionTab.GetTexSize());
    m_SegmentTab.SetTexOffset (m_MissionTab.GetTexOffset() + coreVector2(m_SegmentTab.GetPosition().x - m_MissionTab.GetPosition().x, 0.0));
    m_SegmentTab.GetCaption()->SetTextLanguage("GAME_TAB_SEGMENT");

    m_StartButtonArmory.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButtonArmory.DefineProgram("menu_border_program");
    m_StartButtonArmory.SetPosition  (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButtonArmory.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButtonArmory.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButtonArmory.GetCaption()->SetTextLanguage("START");

    m_StartButtonFirst.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButtonFirst.DefineProgram("menu_border_program");
    m_StartButtonFirst.SetPosition  (m_BackgroundFirst.GetPosition() + m_BackgroundFirst.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButtonFirst.SetSize      (m_StartButtonArmory.GetSize());
    m_StartButtonFirst.SetAlignment (m_StartButtonArmory.GetAlignment());
    m_StartButtonFirst.GetCaption()->SetTextLanguage("START");

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

    m_BackButtonFirst.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonFirst.DefineProgram("menu_border_program");
    m_BackButtonFirst.SetPosition  (m_BackgroundFirst.GetPosition() + m_BackgroundFirst.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButtonFirst.SetSize      (m_BackButtonMain.GetSize());
    m_BackButtonFirst.SetAlignment (m_BackButtonMain.GetAlignment());
    m_BackButtonFirst.GetCaption()->SetText(ICON_SHARE);

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

    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        m_aMissionName[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aMissionName[i].SetPosition (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.055f, -0.05f - 0.025f*I_TO_F(iOffset)));
        m_aMissionName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aMissionName[i].SetColor3   (g_aMissionData[i].vColor);
        m_aMissionName[i].SetText     (g_aMissionData[i].pcName);

        m_aMissionScore[i].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aMissionScore[i].SetPosition(coreVector2(-0.11f, m_aMissionName[i].GetPosition().y));
        m_aMissionScore[i].SetColor3  (COLOR_MENU_WHITE);

        m_aMissionTime[i].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aMissionTime[i].SetPosition(coreVector2(0.11f, m_aMissionName[i].GetPosition().y));
        m_aMissionTime[i].SetColor3  (COLOR_MENU_WHITE);

        m_aMissionIcon[i].DefineTexture(0u, "menu_helper.png");
        m_aMissionIcon[i].DefineProgram("menu_helper_program");
        m_aMissionIcon[i].SetPosition  (m_aMissionName[i].GetPosition() + coreVector2(0.0f,0.0f));
        m_aMissionIcon[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aMissionIcon[i].SetDirection (coreVector2(1.0f,1.0f).Normalized());
        m_aMissionIcon[i].SetColor3    (g_aMissionData[i].vColor * 0.8f);
        m_aMissionIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aMissionIcon[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aMissionMedal[i].DefineTexture(0u, "menu_medal.png");
        m_aMissionMedal[i].DefineProgram("default_2d_program");
        m_aMissionMedal[i].SetPosition  (coreVector2(0.3f, m_aMissionName[i].GetPosition().y));
        m_aMissionMedal[i].SetSize      (coreVector2(0.05f,0.05f));
        m_aMissionMedal[i].SetTexSize   (coreVector2(0.25f,0.25f));

        m_aMissionLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aMissionLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aMissionLine[i].DefineProgram("menu_inner_program");
        m_aMissionLine[i].SetPosition  (coreVector2(0.0f, m_aMissionName[i].GetPosition().y));
        m_aMissionLine[i].SetSize      (coreVector2(m_BackgroundMain.GetSize().x, 0.05f));
        m_aMissionLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aMissionLine[i].SetFocusable (true);

        m_aMissionButton[i].Construct  (MENU_SWITCHBOX, MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aMissionButton[i].SetPosition(coreVector2(0.38f, m_aMissionName[i].GetPosition().y));
        m_aMissionButton[i].SetSize    (coreVector2(0.06f,0.03f));
        m_aMissionButton[i].GetCaption()->SetText(">");

        iOffset += (i == 8u) ? 3u : 2u;
    }

    iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        const coreFloat fHeight = m_BackgroundMain.GetPosition().y + m_BackgroundMain.GetSize().y*0.5f + -0.35f - 0.025f*I_TO_F(iOffset);

        m_aSegmentName[i].Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aSegmentName[i].SetPosition (coreVector2(m_BackgroundMain.GetPosition().x - m_BackgroundMain.GetSize().x*0.5f, fHeight) + coreVector2(0.03f+0.04f*0.0f,0.0995f));
        m_aSegmentName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aSegmentName[i].SetColor3   (g_aMissionData[i].vColor);
        m_aSegmentName[i].SetText     (g_aMissionData[i].pcName);

        m_aSegmentNumber[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aSegmentNumber[i].SetPosition (m_aSegmentName[i].GetPosition() + coreVector2(0.003f,0.034f));
        m_aSegmentNumber[i].SetAlignment(m_aSegmentName[i].GetAlignment());
        m_aSegmentNumber[i].SetColor3   (COLOR_MENU_WHITE);

        m_aSegmentIcon[i].DefineTexture(0u, "menu_helper.png");
        m_aSegmentIcon[i].DefineProgram("menu_helper_program");
        m_aSegmentIcon[i].SetPosition  (m_aSegmentName[i].GetPosition()*coreVector2(1.0f,1.0f) + coreVector2(0.005f,0.015f));                     
        m_aSegmentIcon[i].SetPosition  (coreVector2(m_BackgroundMain.GetPosition().x - m_BackgroundMain.GetSize().x*0.5f, fHeight) + coreVector2(0.09f,0.0f));
        m_aSegmentIcon[i].SetSize      (coreVector2(0.1f,0.1f));
        //m_aSegmentIcon[i].SetDirection (coreVector2(1.0f,1.0f).Normalized()); 
        m_aSegmentIcon[i].SetColor3    (g_aMissionData[i].vColor * 0.8f);
        m_aSegmentIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aSegmentIcon[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aSegmentIconBack[i].DefineTexture(0u, "menu_helper.png");
        m_aSegmentIconBack[i].DefineProgram("menu_helper_program");
        m_aSegmentIconBack[i].SetPosition  (m_aSegmentIcon[i].GetPosition ());
        m_aSegmentIconBack[i].SetSize      (m_aSegmentIcon[i].GetSize     ());
        //m_aSegmentIconBack[i].SetDirection (m_aSegmentIcon[i].GetDirection());
        m_aSegmentIconBack[i].SetColor3    (m_aSegmentIcon[i].GetColor3   ());
        m_aSegmentIconBack[i].SetTexSize   (m_aSegmentIcon[i].GetTexSize  ());
        m_aSegmentIconBack[i].SetTexOffset (m_aSegmentIcon[i].GetTexOffset());
        m_aSegmentIconBack[i].SetFocusable (true);

        m_aSegmentLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aSegmentLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aSegmentLine[i].DefineProgram("menu_inner_program");
        m_aSegmentLine[i].SetPosition  (coreVector2(0.0f, fHeight));
        m_aSegmentLine[i].SetSize      (coreVector2(m_BackgroundMain.GetSize().x, 0.15f));
        m_aSegmentLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        //m_aSegmentLine[i].SetFocusable (true);

        iOffset += (i == 8u) ? 11u : 10u;
    }

#if !defined(_CORE_IDE_)

    coreUintW iCountBoss = 0u;
    coreUintW iCountWave = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            const coreBool    bBoss      = MISSION_SEGMENT_IS_BOSS(j);
            const coreVector2 vPos       = coreVector2((I_TO_F(j) - 3.5f) * 0.1f, m_aSegmentLine[i].GetPosition().y + 0.055f*0.0f);
            const coreVector2 vSize      = bBoss ? coreVector2(0.1f,0.05f)                                   : coreVector2(0.05f,0.05f);
            const coreVector2 vStep      = bBoss ? coreVector2((1.0f/37.0f), (1.0f/21.0f) * (581.0f/512.0f)) : coreVector2((1.0f/41.0f), (1.0f/41.0f));
            const coreVector2 vTexSize   = bBoss ? coreVector2(8.0f,4.0f)                                    : coreVector2(4.0f,4.0f);
            const coreVector2 vTexOffset = bBoss ? coreVector2(I_TO_F(iCountBoss % 4u) * 9.0f + 1.0f, I_TO_F(iCountBoss / 4u) * 5.0f + 1.0f) :
                                                   coreVector2(I_TO_F(iCountWave % 8u) * 5.0f + 1.0f, I_TO_F(iCountWave / 8u) * 5.0f + 1.0f);

            m_aaSegmentTile[i][j].DefineTexture   (0u, bBoss ? "menu_segment_boss.png" : "menu_segment_wave.png");
            m_aaSegmentTile[i][j].DefineProgram   ("menu_grey_program");
            m_aaSegmentTile[i][j].SetPosition     (vPos + (bBoss ? coreVector2(0.015f,0.0f) : coreVector2(-0.02f,0.0f)) + coreVector2(0.145f,0.0f));
            m_aaSegmentTile[i][j].SetSize         (vSize * 1.4f);
            m_aaSegmentTile[i][j].SetColor3       (COLOR_MENU_WHITE);
            m_aaSegmentTile[i][j].SetTexSize      (vTexSize   * vStep);
            m_aaSegmentTile[i][j].SetTexOffset    (vTexOffset * vStep);
            m_aaSegmentTile[i][j].SetFocusable    (true);
            m_aaSegmentTile[i][j].SetFocusModifier(coreVector2(1.25f,1.25f));

            m_aaSegmentBack[i][j].DefineProgram("menu_color_program");
            m_aaSegmentBack[i][j].SetPosition  (m_aaSegmentTile[i][j].GetPosition());
            m_aaSegmentBack[i][j].SetSize      (m_aaSegmentTile[i][j].GetSize() + coreVector2(0.01f,0.01f));
            m_aaSegmentBack[i][j].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.15f);
            m_aaSegmentBack[i][j].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);
            
            
            m_aaSegmentMedal[i][j].DefineTexture(0u, "menu_medal.png");
            m_aaSegmentMedal[i][j].DefineProgram("default_2d_program");
            m_aaSegmentMedal[i][j].SetPosition  (m_aaSegmentTile[i][j].GetPosition() + m_aaSegmentTile[i][j].GetSize()*coreVector2(0.0f,-0.5f) + coreVector2(0.0f,-0.055f));
            m_aaSegmentMedal[i][j].SetSize      (coreVector2(0.06f,0.06f));
            m_aaSegmentMedal[i][j].SetTexSize   (coreVector2(0.25f,0.25f));
            
            for(coreUintW k = 0u; k < MENU_GAME_ARMORY_BADGES; ++k)
            {
                m_aaSegmentBadge[i][j][k].DefineTexture(0u, "menu_badge.png");
                m_aaSegmentBadge[i][j][k].DefineProgram("default_2d_program");
                m_aaSegmentBadge[i][j][k].SetPosition  (m_aaSegmentMedal[i][j].GetPosition() + coreVector2(I_TO_F(k - 1u) * 0.05f, (k == 1u) ? -0.085f : -0.065f) * 0.6f);
                m_aaSegmentBadge[i][j][k].SetSize      (coreVector2(0.04f,0.04f));
                m_aaSegmentBadge[i][j][k].SetTexSize   (coreVector2(0.5f, 1.0f));
            }

            if(bBoss) iCountBoss += 1u;
                 else iCountWave += 1u;
        }
    }

#endif

    m_aSegmentCursor[0].DefineProgram("menu_color_program");
    m_aSegmentCursor[0].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);   // less    
    m_aSegmentCursor[1].DefineProgram("menu_grey_program");

    m_SegmentBox.SetPosition(m_BackgroundMain.GetPosition() + coreVector2(0.0f,0.0f));
    m_SegmentBox.SetSize    (coreVector2(m_BackgroundMain.GetSize().x, 0.77f));
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentIconBack[i]);        
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentIcon[i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS;      ++j) m_SegmentBox.BindObject(&m_aaSegmentBack[i][j]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS - 1u; ++j) m_SegmentBox.BindObject(&m_aaSegmentTile[i][j]);   // waves
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aaSegmentTile[i][MENU_GAME_SEGMENTS - 1u]);                                     // bosses
    m_SegmentBox.BindObject(&m_aSegmentCursor[0]);
    m_SegmentBox.BindObject(&m_aSegmentCursor[1]);
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS;      ++j) m_SegmentBox.BindObject(&m_aaSegmentMedal[i][j]);
    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS;      ++j) for(coreUintW k = 0u; k < MENU_GAME_ARMORY_BADGES; ++k) m_SegmentBox.BindObject(&m_aaSegmentBadge[i][j][k]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentName  [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentNumber[i]);

    m_SegmentArea.DefineTexture(0u, "menu_detail_04.png");
    m_SegmentArea.DefineTexture(1u, "menu_background_black.png");
    m_SegmentArea.DefineProgram("menu_inner_program");
    m_SegmentArea.SetPosition  (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(0.5f,0.0f) + coreVector2(-10.15f,0.0f));                                
    m_SegmentArea.SetSize      (coreVector2(m_BackgroundMain.GetSize().y, 0.25f));
    m_SegmentArea.SetDirection (coreVector2(1.0f,0.0f));

    m_SegmentMedal.DefineTexture(0u, "menu_medal.png");
    m_SegmentMedal.DefineProgram("default_2d_program");
    m_SegmentMedal.SetPosition  (m_SegmentArea.GetPosition() + coreVector2(0.0f,-0.215f));
    m_SegmentMedal.SetSize      (coreVector2(0.11f,0.11f));
    m_SegmentMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    m_SegmentFragment.DefineTexture(1u, "menu_background_black.png");
    m_SegmentFragment.DefineProgram("menu_fragment_program");
    m_SegmentFragment.SetPosition  (m_SegmentMedal.GetPosition() + coreVector2(0.0f,-0.07f));
    m_SegmentFragment.SetDirection (coreVector2(1.0f,1.0f).Normalized());

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        m_aSegmentBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aSegmentBadge[i].DefineProgram("default_2d_program");
        m_aSegmentBadge[i].SetPosition  (m_SegmentMedal.GetPosition() + coreVector2(I_TO_F(i - 1u) * 0.05f, (i == 1u) ? -0.085f : -0.065f));
        m_aSegmentBadge[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aSegmentBadge[i].SetTexSize   (coreVector2(0.5f, 1.0f));

        m_aSegmentBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aSegmentBadgeWave[i].DefineProgram("menu_single_program");
        m_aSegmentBadgeWave[i].SetPosition  (m_aSegmentBadge[i].GetPosition());
        m_aSegmentBadgeWave[i].SetSize      (m_aSegmentBadge[i].GetSize() * 0.7f);
    }

    m_aSegmentTitle[0].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aSegmentTitle[0].SetPosition(m_SegmentArea.GetPosition() + coreVector2(0.0f,0.233f));
    m_aSegmentTitle[0].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentTitle[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentTitle[1].SetPosition(m_aSegmentTitle[0].GetPosition() + coreVector2(0.0f,0.04f));
    m_aSegmentTitle[1].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentScore[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aSegmentScore[0].SetPosition    (m_SegmentArea.GetPosition() + coreVector2(0.0f,0.118f));
    m_aSegmentScore[0].SetColor3      (COLOR_MENU_WHITE);
    m_aSegmentScore[0].SetTextLanguage("GAME_SCORE");

    m_aSegmentScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentScore[1].SetPosition(m_aSegmentScore[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aSegmentScore[1].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentTime[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aSegmentTime[0].SetPosition    (m_SegmentArea.GetPosition() + coreVector2(0.0f,0.018f));
    m_aSegmentTime[0].SetColor3      (COLOR_MENU_WHITE);
    m_aSegmentTime[0].SetTextLanguage("GAME_TIME");

    m_aSegmentTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentTime[1].SetPosition(m_aSegmentTime[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aSegmentTime[1].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentMaxSeries[0].Construct  (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aSegmentMaxSeries[0].SetPosition(m_SegmentArea.GetPosition() + coreVector2(0.0f,-0.082f));
    m_aSegmentMaxSeries[0].SetColor3  (COLOR_MENU_WHITE);

    m_aSegmentMaxSeries[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aSegmentMaxSeries[1].SetPosition(m_aSegmentMaxSeries[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aSegmentMaxSeries[1].SetColor3  (COLOR_MENU_WHITE);

    m_SegmentIconBig.DefineTexture(0u, "menu_helper.png");
    m_SegmentIconBig.DefineProgram("menu_helper_program");
    m_SegmentIconBig.SetPosition  (m_aSegmentTitle[0].GetPosition() + coreVector2(0.0f,0.017f));
    m_SegmentIconBig.SetSize      (coreVector2(0.16f,0.16f));
    m_SegmentIconBig.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_SegmentIconBig.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i)
    {
        const coreFloat fHeight = m_BackgroundMain.GetPosition().y + m_BackgroundMain.GetSize().y*0.5f + -0.125f;

        m_aArcadeName[i].Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aArcadeName[i].SetPosition (coreVector2(m_BackgroundMain.GetPosition().x - m_BackgroundMain.GetSize().x*0.5f, fHeight) + coreVector2(0.03f,0.0745f));
        m_aArcadeName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aArcadeName[i].SetColor3   (COLOR_MENU_WHITE);
        
        //m_aArcadeName[i].Construct   (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        //m_aArcadeName[i].SetPosition (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.1f*I_TO_F(i)));
        //m_aArcadeName[i].SetAlignment(coreVector2(1.0f,0.0f));
        //m_aArcadeName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aArcadeBack[i].DefineTexture(0u, "menu_detail_02.png");
        m_aArcadeBack[i].DefineTexture(1u, "menu_background_black.png");
        m_aArcadeBack[i].DefineProgram("menu_free_program");
        m_aArcadeBack[i].SetPosition  (coreVector2(0.0f, m_aArcadeName[i].GetPosition().y));
        m_aArcadeBack[i].SetSize      (coreVector2(7.0f + 5.0f, 0.5f) * 0.07f);
        m_aArcadeBack[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);

        m_aArcadeLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aArcadeLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aArcadeLine[i].DefineProgram("menu_inner_program");
        m_aArcadeLine[i].SetPosition  (coreVector2(0.0f, fHeight));//m_aArcadeName[i].GetPosition().y));
        m_aArcadeLine[i].SetSize      (coreVector2(m_BackgroundMain.GetSize().x, 0.1f));
        m_aArcadeLine[i].SetTexOffset (coreVector2(I_TO_F(i - 1u)*0.09f, 0.0f));
        m_aArcadeLine[i].SetFocusable (true);
    }
    //m_aArcadeName[0].SetTextLanguage("GAME_ARCADE_COMPLETE")
    m_aArcadeName[0].SetText("ARCADE");

    m_ArcadeComplete.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArcadeComplete.SetPosition (coreVector2( 0.00f,1.00f) * m_aArcadeLine[0].GetPosition());
    m_ArcadeComplete.SetSize     (coreVector2( 0.35f,0.05f));
    //m_ArcadeComplete.SetAlignment(coreVector2(-1.00f,0.00f));
    //m_ArcadeComplete.GetCaption()->SetTextLanguage("CONTINUE")
    m_ArcadeComplete.GetCaption()->SetTextLanguage("GAME_ARCADE_COMPLETE");
    
    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i) m_SegmentBox.BindObject(&m_aArcadeLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i) m_SegmentBox.BindObject(&m_aArcadeName[i]);
    m_SegmentBox.BindObject(&m_ArcadeComplete);

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
    m_OptionType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_OptionMode.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_OptionMode.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[1].GetPosition());
    m_OptionMode.SetSize     (m_OptionType.GetSize());
    m_OptionMode.SetAlignment(m_OptionType.GetAlignment());
    m_OptionMode.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_OptionDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_OptionDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aOptionName[2].GetPosition());
    m_OptionDifficulty.SetSize     (m_OptionType.GetSize());
    m_OptionDifficulty.SetAlignment(m_OptionType.GetAlignment());
    m_OptionDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryHeader.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArmoryHeader.SetPosition    (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.045f));
    m_ArmoryHeader.SetColor3      (COLOR_MENU_WHITE);
    m_ArmoryHeader.SetTextLanguage("GAME_HEADER_ARMORY");

    iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES; ++i)
    {
        m_aArmoryName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryName[i].SetPosition (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.495f - 0.025f*I_TO_F(iOffset)));
        m_aArmoryName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aArmoryName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aArmoryLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aArmoryLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aArmoryLine[i].DefineProgram("menu_inner_program");
        m_aArmoryLine[i].SetPosition  (coreVector2(0.0f, m_aArmoryName[i].GetPosition().y));
        m_aArmoryLine[i].SetSize      (coreVector2(m_BackgroundArmory.GetSize().x, 0.05f));
        m_aArmoryLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aArmoryLine[i].SetFocusable (true);

        iOffset += (i == 1u) ? 0u : (i == 3u) ? 3u : 2u;
    }
    m_aArmoryName[0].SetTextLanguage("GAME_TYPE");
    m_aArmoryName[1].SetTextLanguage("GAME_MODE");
    m_aArmoryName[2].SetTextLanguage("GAME_DIFFICULTY");
    m_aArmoryName[3].SetTextLanguage("CONFIG_GAME_GAMESPEED");
    m_aArmoryName[4].SetTextLanguage("GAME_SHIELD");
    
    m_aArmoryName[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aArmoryLine[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

    m_ArmoryArea.DefineTexture(0u, "menu_detail_04.png");
    m_ArmoryArea.DefineTexture(1u, "menu_background_black.png");
    m_ArmoryArea.DefineProgram("menu_inner_program");
    m_ArmoryArea.SetPosition  (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.205f));
    m_ArmoryArea.SetSize      (coreVector2(m_BackgroundArmory.GetSize().x, 0.12f));
    m_ArmoryArea.SetTexOffset (coreVector2(I_TO_F(-2)*0.09f, 0.0f));

    m_ArmoryArea2.DefineTexture(0u, "menu_detail_04.png");
    m_ArmoryArea2.DefineTexture(1u, "menu_background_black.png");
    m_ArmoryArea2.DefineProgram("menu_inner_program");
    m_ArmoryArea2.SetPosition  (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.355f));
    m_ArmoryArea2.SetSize      (coreVector2(m_BackgroundArmory.GetSize().x, 0.18f));
    m_ArmoryArea2.SetTexOffset (coreVector2(I_TO_F(-1)*0.09f, 0.0f));

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        m_aArmoryMedal[i].DefineTexture(0u, "menu_medal.png");
        m_aArmoryMedal[i].DefineProgram("default_2d_program");
        m_aArmoryMedal[i].SetPosition  (i ? coreVector2(0.0f,0.0f) : (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.0f)));
        m_aArmoryMedal[i].SetSize      (coreVector2(1.0f,1.0f) * (i ? 0.0f : 0.1f));
        m_aArmoryMedal[i].SetTexSize   (coreVector2(0.25f,0.25f));
    }

    m_ArmoryMedalBack.DefineTexture(0u, "menu_detail_02.png");
    m_ArmoryMedalBack.DefineTexture(1u, "menu_background_black.png");
    m_ArmoryMedalBack.DefineProgram("menu_free_program");
    m_ArmoryMedalBack.SetPosition  (m_ArmoryArea2.GetPosition() + coreVector2(0.0f,-0.022f));
    m_ArmoryMedalBack.SetSize      (coreVector2(7.0f + 5.0f, 0.5f) * 0.07f);
    m_ArmoryMedalBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);

    m_ArmoryFragment.DefineTexture(1u, "menu_background_black.png");
    m_ArmoryFragment.DefineProgram("menu_fragment_program");
    m_ArmoryFragment.SetDirection (coreVector2(1.0f,1.0f).Normalized());

    m_ArmoryFragmentBack.DefineTexture(0u, "menu_detail_05.png");
    m_ArmoryFragmentBack.DefineTexture(1u, "menu_background_black.png");
    m_ArmoryFragmentBack.DefineProgram("menu_free_program");
    m_ArmoryFragmentBack.SetSize      (coreVector2(1.0f,1.0f) * 0.1f);
    m_ArmoryFragmentBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
    {
        m_aArmoryBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aArmoryBadge[i].DefineProgram("default_2d_program");
        m_aArmoryBadge[i].SetTexSize   (coreVector2(0.5f,1.0f));

        m_aArmoryBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aArmoryBadgeWave[i].DefineProgram("menu_single_program");
    }

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        m_aArmoryBadgeBack[i].DefineTexture(0u, "menu_detail_03.png");
        m_aArmoryBadgeBack[i].DefineTexture(1u, "menu_background_black.png");
        m_aArmoryBadgeBack[i].DefineProgram("menu_free_program");
        m_aArmoryBadgeBack[i].SetPosition  (m_ArmoryArea2.GetPosition() + coreVector2(((i == 1u) ? 0.025f : 0.0f) - 0.39f, 0.0f + I_TO_F(i - 1u) * -0.045f));
        m_aArmoryBadgeBack[i].SetSize      (coreVector2(7.0f + 2.0f + 0.6f, 0.5f) * 0.07f);
        m_aArmoryBadgeBack[i].SetAlignment (coreVector2(1.0f,0.0f));
        m_aArmoryBadgeBack[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);
        m_aArmoryBadgeBack[i].SetTexSize   (coreVector2(0.5f,1.0f));
        m_aArmoryBadgeBack[i].SetTexOffset (coreVector2(0.5f,0.0f));

        m_aArmoryBadgeDesc[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryBadgeDesc[i].SetPosition (m_aArmoryBadgeBack[i].GetPosition() + coreVector2(0.05f,0.0f));
        m_aArmoryBadgeDesc[i].SetAlignment(m_aArmoryBadgeBack[i].GetAlignment());
        m_aArmoryBadgeDesc[i].SetColor3   (COLOR_MENU_WHITE);
    }

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS; ++i)
    {
        m_aArmoryHelper[i].DefineTexture(0u, "menu_helper.png");
        m_aArmoryHelper[i].DefineProgram("menu_helper_program");
        m_aArmoryHelper[i].SetPosition  (m_aArmoryBadgeDesc[1].GetPosition() + coreVector2(0.045f + I_TO_F(i) * 0.06f, 0.0f));
        m_aArmoryHelper[i].SetSize      (coreVector2(1.0f,1.0f) * 0.045f);
        m_aArmoryHelper[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aArmoryHelper[i].SetTexOffset (coreVector2(0.25f,0.25f) * coreVector2(I_TO_F(i % 4u), I_TO_F(i / 4u)));

        m_aArmoryHelperWave[i].DefineTexture(0u, m_aArmoryHelper[i].GetTexture(0u));
        m_aArmoryHelperWave[i].DefineProgram(m_aArmoryHelper[i].GetProgram  ());
        m_aArmoryHelperWave[i].SetPosition  (m_aArmoryHelper[i].GetPosition ());
        m_aArmoryHelperWave[i].SetSize      (m_aArmoryHelper[i].GetSize     () * 1.2f);
        m_aArmoryHelperWave[i].SetTexSize   (m_aArmoryHelper[i].GetTexSize  ());
        m_aArmoryHelperWave[i].SetTexOffset (m_aArmoryHelper[i].GetTexOffset());
    }

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS; ++i)
    {
        m_aArmoryIcon[i].DefineTexture(0u, "menu_helper.png");
        m_aArmoryIcon[i].DefineProgram("menu_helper_program");
        m_aArmoryIcon[i].SetSize      (coreVector2(1.0f,1.0f) * 0.07f);
        m_aArmoryIcon[i].SetDirection (coreVector2(1.0f,1.0f).Normalized());
        m_aArmoryIcon[i].SetColor3    (g_aMissionData[i].vColor * 0.8f);
        m_aArmoryIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aArmoryIcon[i].SetTexOffset (g_aMissionData[i].vIcon);
    }

    m_aArmoryTitle[0].Construct   (MENU_FONT_STANDARD_4, MENU_OUTLINE_SMALL);
    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.105f));
    m_aArmoryTitle[0].SetAlignment(coreVector2(0.0f,0.0f));

    m_aArmoryTitle[1].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryTitle[1].SetPosition (m_aArmoryTitle[0].GetPosition() + coreVector2(0.0f,0.044f));
    m_aArmoryTitle[1].SetAlignment(m_aArmoryTitle[0].GetAlignment());
    m_aArmoryTitle[1].SetColor3   (COLOR_MENU_WHITE);

    m_aArmoryScore[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aArmoryScore[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(-0.235f,0.017f));
    m_aArmoryScore[0].SetColor3      (COLOR_MENU_WHITE);
    m_aArmoryScore[0].SetTextLanguage("GAME_SCORE");

    m_aArmoryScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryScore[1].SetPosition(m_aArmoryScore[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryScore[1].SetColor3  (COLOR_MENU_WHITE);

    m_aArmoryTime[0].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aArmoryTime[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(0.235f,0.017f));
    m_aArmoryTime[0].SetColor3      (COLOR_MENU_WHITE);
    m_aArmoryTime[0].SetTextLanguage("GAME_TIME");

    m_aArmoryTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryTime[1].SetPosition(m_aArmoryTime[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryTime[1].SetColor3  (COLOR_MENU_WHITE);

    m_aArmoryMaxSeries[0].Construct  (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_aArmoryMaxSeries[0].SetPosition(m_ArmoryArea.GetPosition() + coreVector2(0.33f,-0.077f));
    m_aArmoryMaxSeries[0].SetColor3  (COLOR_MENU_WHITE);

    m_aArmoryMaxSeries[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryMaxSeries[1].SetPosition(m_aArmoryMaxSeries[0].GetPosition() + coreVector2(0.0f,-0.034f));
    m_aArmoryMaxSeries[1].SetColor3  (COLOR_MENU_WHITE);

    m_ArmoryIconBig.DefineTexture(0u, "menu_helper.png");
    m_ArmoryIconBig.DefineProgram("menu_helper_program");
    m_ArmoryIconBig.SetPosition  (m_aArmoryTitle[0].GetPosition() + coreVector2(0.0f,0.018f));
    m_ArmoryIconBig.SetSize      (coreVector2(0.2f,0.2f));
    m_ArmoryIconBig.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_ArmoryIconBig.SetTexSize   (coreVector2(0.25f,0.25f));

    m_ArmoryLogo.DefineTexture(0u, "game_logo_free.png");
    m_ArmoryLogo.DefineProgram("default_2d_program");
    m_ArmoryLogo.SetPosition  (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.123f));
    m_ArmoryLogo.SetSize      (coreVector2(1.0f,0.25f) * 0.5f);

    m_ArmoryTile.DefineProgram("menu_grey_program");

    m_ArmoryBack.DefineProgram("menu_color_program");
    m_ArmoryBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);

    m_ArmorySelection.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);   // TODO 1: appearance still weird, especially animations
    m_ArmorySelection.SetPosition(m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.08f));
    m_ArmorySelection.SetSize    (coreVector2(0.73f,0.065f));
    m_ArmorySelection.SetEndless (true);
    m_ArmorySelection.GetArrow(0u)->DefineProgram("menu_border_program");
    m_ArmorySelection.GetArrow(1u)->DefineProgram("menu_border_program");
    
    m_ArmorySelection.AddEntry("", 0u);
    m_ArmorySelection.AddEntry("", 1u);

    m_ArmoryType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryType.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[0].GetPosition());
    m_ArmoryType.SetSize     (coreVector2( 0.47f,0.03f));
    m_ArmoryType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_ArmoryType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryMode.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryMode.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[1].GetPosition());
    m_ArmoryMode.SetSize     (m_ArmoryType.GetSize());
    m_ArmoryMode.SetAlignment(m_ArmoryType.GetAlignment());
    m_ArmoryMode.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[2].GetPosition());
    m_ArmoryDifficulty.SetSize     (m_ArmoryType.GetSize());
    m_ArmoryDifficulty.SetAlignment(m_ArmoryType.GetAlignment());
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
        m_aArmoryShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aArmoryWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryWeapon[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[5].GetPosition() - vOffset);
        m_aArmoryWeapon[i].SetSize     (m_aArmoryShield[i].GetSize());
        m_aArmoryWeapon[i].SetAlignment(m_aArmoryShield[i].GetAlignment());
        m_aArmoryWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aArmoryPlayer[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aArmoryPlayer[i].SetPosition(m_aArmoryWeapon[i].GetPosition() - coreVector2(m_aArmoryWeapon[i].GetSize().x * 0.5f, 0.05f));
        m_aArmoryPlayer[i].SetColor3  (COLOR_MENU_WHITE);
    }

    m_FirstHeader.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_FirstHeader.SetPosition    (m_BackgroundFirst.GetPosition() + m_BackgroundFirst.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.06f));
    m_FirstHeader.SetColor3      (COLOR_MENU_WHITE);
    m_FirstHeader.SetTextLanguage("GAME_HEADER_FIRST");

    iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_FIRSTS; ++i)
    {
        m_aFirstName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aFirstName[i].SetPosition (m_BackgroundFirst.GetPosition() + m_BackgroundFirst.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.13f - 0.025f*I_TO_F(iOffset)));
        m_aFirstName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aFirstName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aFirstLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aFirstLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aFirstLine[i].DefineProgram("menu_inner_program");
        m_aFirstLine[i].SetPosition  (coreVector2(0.0f, m_aFirstName[i].GetPosition().y));
        m_aFirstLine[i].SetSize      (coreVector2(m_BackgroundFirst.GetSize().x, 0.05f));
        m_aFirstLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aFirstLine[i].SetFocusable (true);

        iOffset += ((i == 0u) || (i == 2u)) ? 3u : 2u;
    }
    m_aFirstName[0].SetTextLanguage("GAME_TYPE");
    m_aFirstName[1].SetTextLanguage("GAME_DIFFICULTY");
    m_aFirstName[2].SetTextLanguage("CONFIG_GAME_GAMESPEED");
    m_aFirstName[3].SetTextLanguage("GAME_SHIELD");

    m_FirstType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_FirstType.SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[0].GetPosition());
    m_FirstType.SetSize     (coreVector2( 0.47f,0.03f));
    m_FirstType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_FirstType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FirstDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_FirstDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[1].GetPosition());
    m_FirstDifficulty.SetSize     (m_FirstType.GetSize());
    m_FirstDifficulty.SetAlignment(m_FirstType.GetAlignment());
    m_FirstDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FirstSpeed.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_FirstSpeed.SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[2].GetPosition());
    m_FirstSpeed.SetSize     (m_FirstType.GetSize());
    m_FirstSpeed.SetAlignment(m_FirstType.GetAlignment());
    m_FirstSpeed.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aFirstShield[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aFirstShield[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[3].GetPosition() - vOffset);
        m_aFirstShield[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aFirstShield[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aFirstShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aFirstPlayer[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aFirstPlayer[i].SetPosition(m_aFirstShield[i].GetPosition() - coreVector2(m_aFirstShield[i].GetSize().x * 0.5f, 0.05f));
        m_aFirstPlayer[i].SetColor3  (COLOR_MENU_WHITE);
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
    m_DemoType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_DemoDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[1].GetPosition());
    m_DemoDifficulty.SetSize     (m_DemoType.GetSize());
    m_DemoDifficulty.SetAlignment(m_DemoType.GetAlignment());
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
    m_DemoStage.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoStageIcon.DefineTexture(0u, "menu_helper.png");
    m_DemoStageIcon.DefineProgram("menu_helper_program");
    m_DemoStageIcon.SetPosition  (m_DemoStage.GetPosition() + m_DemoStage.GetSize() * coreVector2(-0.5f,0.0f));
    m_DemoStageIcon.SetSize      (coreVector2(0.1f,0.1f));
    m_DemoStageIcon.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_DemoStageIcon.SetTexSize   (coreVector2(0.25f,0.25f));

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

    m_SpeedNew.SetPosition(m_ArmorySpeed.GetPosition() + coreVector2(0.045f,0.0f));
    m_SpeedNew.SetIndex   (NEW_ARMORY_GAMESPEED);

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
    m_ArmoryMode      .AddEntryLanguage("GAME_MODE_STANDARD",     GAME_MODE_STANDARD);
    m_ArmoryMode      .AddEntryLanguage("GAME_MODE_PACIFIST",     GAME_MODE_PACIFIST);
    m_ArmoryMode      .AddEntryLanguage("GAME_MODE_MASOCHIST",    GAME_MODE_MASOCHIST);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aArmoryShield[i].AddEntryLanguage("VALUE_NO",                           0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) for(coreUintW j = 10u; j <= 100u; j += 10u) m_aArmoryShield[i].AddEntry(PRINT("%zu", j), j);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aArmoryShield[i].AddEntry        (coreData::ToChars(SHIELD_INVINCIBLE), SHIELD_MAX);
    m_FirstType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_FirstType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_FirstDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_FirstDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    for(coreUintW i = 50u; i <= 100u; i += 5u) m_FirstSpeed.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aFirstShield[i].AddEntryLanguage("VALUE_NO",                           0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) for(coreUintW j = 10u; j <= 100u; j += 10u) m_aFirstShield[i].AddEntry(PRINT("%zu", j), j);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aFirstShield[i].AddEntry        (coreData::ToChars(SHIELD_INVINCIBLE), SHIELD_MAX);
    m_DemoType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_DemoType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_DemoDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_DemoDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    for(coreUintW i = 50u; i <= 100u; i += 5u) m_DemoSpeed.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aDemoShield[i].AddEntryLanguage("VALUE_NO",                           0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) for(coreUintW j = 10u; j <= 100u; j += 10u) m_aDemoShield[i].AddEntry(PRINT("%zu", j), j);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aDemoShield[i].AddEntry        (coreData::ToChars(SHIELD_INVINCIBLE), SHIELD_MAX);

    // 
    //m_NavigatorMain.BindObject(&m_MissionTab,     &m_BackButtonMain, &m_SegmentTab, &m_ArcadeComplete,      &m_SegmentTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    //m_NavigatorMain.BindObject(&m_SegmentTab,     &m_BackButtonMain, &m_MissionTab, &m_aaSegmentTile[0][0], &m_MissionTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_NavigatorMain.BindObject(&m_BackButtonMain, &m_aaSegmentTile[MENU_GAME_MISSIONS - 1u][MENU_GAME_SEGMENTS - 1u], NULL,          &m_ArcadeComplete,    NULL,          MENU_TYPE_DEFAULT);

    //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    //{
    //    m_NavigatorMain.BindObject(&m_aMissionLine[i], (i == 0u)                      ? s_cast<coreObject2D*>(&m_MissionTab)     : &m_aMissionLine[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS], NULL,
    //                                                   (i == MENU_GAME_MISSIONS - 1u) ? s_cast<coreObject2D*>(&m_ArcadeComplete) : &m_aMissionLine[(i + 1u)                      % MENU_GAME_MISSIONS], NULL, MENU_TYPE_TAB_NODE, SURFACE_GAME_MISSION);
    //}
    //m_NavigatorMain.BindObject(&m_ArcadeComplete, &m_aMissionLine[MENU_GAME_MISSIONS - 1u], NULL, &m_BackButtonMain, NULL, MENU_TYPE_TAB_NODE, SURFACE_GAME_MISSION);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            m_NavigatorMain.BindObject(&m_aaSegmentTile[i][j], (i == 0u)                      ? s_cast<coreObject2D*>(&m_ArcadeComplete) :                                                                  &m_aaSegmentTile[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS][j], (j == 0u)                      ? &m_aSegmentIcon[i] : &m_aaSegmentTile[i][(j + MENU_GAME_SEGMENTS - 1u) % MENU_GAME_SEGMENTS],
                                                               (i == MENU_GAME_MISSIONS - 1u) ? s_cast<coreObject2D*>(&m_BackButtonMain) : (i == 7u) ? &m_aaSegmentTile[(i + 1u) % MENU_GAME_MISSIONS][5] : &m_aaSegmentTile[(i + 1u)                      % MENU_GAME_MISSIONS][j], (j == MENU_GAME_SEGMENTS - 1u) ? &m_aSegmentIcon[i] : &m_aaSegmentTile[i][(j + 1u)                      % MENU_GAME_SEGMENTS], MENU_TYPE_BIG);
        }
        m_NavigatorMain.BindObject(&m_aSegmentIcon[i], (i == 0u)                      ? s_cast<coreObject2D*>(&m_ArcadeComplete) : &m_aSegmentIcon[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS], &m_aaSegmentTile[i][MENU_GAME_SEGMENTS - 1u],
                                                       (i == MENU_GAME_MISSIONS - 1u) ? s_cast<coreObject2D*>(&m_BackButtonMain) : &m_aSegmentIcon[(i + 1u)                      % MENU_GAME_MISSIONS], &m_aaSegmentTile[i][0], MENU_TYPE_DEFAULT);
    }
    m_NavigatorMain.BindObject(&m_ArcadeComplete, &m_BackButtonMain, NULL, &m_aaSegmentTile[0][0], NULL, MENU_TYPE_DEFAULT);

    //m_NavigatorMain.BindSurface(&m_MissionTab, SURFACE_GAME_MISSION, &m_ArcadeComplete,                                                  NULL, &m_aMissionLine [0],    NULL);
    //m_NavigatorMain.BindSurface(&m_SegmentTab, SURFACE_GAME_SEGMENT, &m_aaSegmentTile[MENU_GAME_MISSIONS - 1u][MENU_GAME_SEGMENTS - 1u], NULL, &m_aaSegmentTile[0][0], NULL);

    m_NavigatorMain.BindScroll(&m_SegmentBox);

    m_NavigatorMain.AssignFirst(&m_ArcadeComplete);
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

    m_NavigatorArmory.BindObject(&m_ArmorySelection,   &m_StartButtonArmory, NULL,                 &m_ArmoryType,        NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmoryType,        &m_ArmorySelection,   NULL,                 &m_ArmoryMode,        NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
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

    m_NavigatorArmory.UseShoulderLeft ([this]() {this->SelectPrevious(); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});
    m_NavigatorArmory.UseShoulderRight([this]() {this->SelectNext    (); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});

    m_NavigatorFirst.BindObject(&m_FirstType,        &m_StartButtonFirst, NULL,                &m_FirstDifficulty,  NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorFirst.BindObject(&m_FirstDifficulty,  &m_FirstType,        NULL,                &m_FirstSpeed,       NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorFirst.BindObject(&m_FirstSpeed,       &m_FirstDifficulty,  NULL,                &m_aFirstShield[0],  NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorFirst.BindObject(&m_aFirstShield[0],  &m_FirstSpeed,       &m_aFirstShield[1],  &m_StartButtonFirst, &m_aFirstShield[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorFirst.BindObject(&m_aFirstShield[1],  &m_FirstSpeed,       &m_aFirstShield[0],  &m_StartButtonFirst, &m_aFirstShield[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorFirst.BindObject(&m_StartButtonFirst, &m_aFirstShield[0],  &m_BackButtonFirst,  &m_FirstType,        &m_BackButtonFirst,  MENU_TYPE_DEFAULT);
    m_NavigatorFirst.BindObject(&m_BackButtonFirst,  &m_aFirstShield[0],  &m_StartButtonFirst, &m_FirstType,        &m_StartButtonFirst, MENU_TYPE_DEFAULT);

    m_NavigatorFirst.AssignFirst(&m_FirstType);
    m_NavigatorFirst.AssignBack (&m_BackButtonFirst);
    m_NavigatorFirst.AssignMenu (this);

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
        //if(i != SURFACE_GAME_MISSION) this->BindObject(i, &m_MissionTab);
        //if(i != SURFACE_GAME_SEGMENT) this->BindObject(i, &m_SegmentTab);

        this->BindObject(i, &m_BackgroundMain);

        //if(i == SURFACE_GAME_MISSION) this->BindObject(i, &m_MissionTab);
        //if(i == SURFACE_GAME_SEGMENT) this->BindObject(i, &m_SegmentTab);

        this->BindObject(i, &m_BackButtonMain);
        //this->BindObject(i, &m_OptionState);
    }

    //for(coreUintW i = 0u; i < MENU_GAME_ARCADES;  ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aArcadeLine   [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aMissionLine  [i]);
    //for(coreUintW i = 0u; i < MENU_GAME_ARCADES;  ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aArcadeBack   [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aMissionMedal [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aMissionIcon  [i]);
    //for(coreUintW i = 0u; i < MENU_GAME_ARCADES;  ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aArcadeName   [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aMissionName  [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aMissionScore [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aMissionTime  [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) this->BindObject(SURFACE_GAME_MISSION, &m_aMissionButton[i]);
    //this->BindObject(SURFACE_GAME_MISSION, &m_ArcadeComplete);

    this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentBox);
    //this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentArea);
    //this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentFragment);
    //this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentMedal);
    //for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentBadgeWave[i]);
    //for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentBadge    [i]);
    //this->BindObject(SURFACE_GAME_SEGMENT, &m_SegmentIconBig);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentTitle);     ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentTitle    [i]);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentScore);     ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentScore    [i]);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentTime);      ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentTime     [i]);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentMaxSeries); ++i) this->BindObject(SURFACE_GAME_SEGMENT, &m_aSegmentMaxSeries[i]);

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
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryArea2);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryIconBig);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeBack[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryMedalBack);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryFragmentBack);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryFragment);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryBack);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryTile);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryHelperWave[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryHelper    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryIcon      [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeWave [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadge     [i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryLogo);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMedal    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeDesc[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryName     [i]);
    //this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryHeader);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTitle);     ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryTitle    [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryScore);     ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryScore    [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTime);      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryTime     [i]);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryMaxSeries); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMaxSeries[i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS;              ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryPlayer   [i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmorySelection);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryType);
    //this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryMode);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryDifficulty);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmorySpeed);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryShield[i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryWeapon[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_SpeedNew);
    this->BindObject(SURFACE_GAME_ARMORY, &m_NavigatorArmory);

    this->BindObject(SURFACE_GAME_FIRST, &m_BackgroundFirst);
    this->BindObject(SURFACE_GAME_FIRST, &m_StartButtonFirst);
    this->BindObject(SURFACE_GAME_FIRST, &m_BackButtonFirst);
    for(coreUintW i = 0u; i < MENU_GAME_FIRSTS; ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_FIRSTS; ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstName[i]);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstHeader);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstPlayer[i]);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstType);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstDifficulty);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstSpeed);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstShield[i]);
    this->BindObject(SURFACE_GAME_FIRST, &m_NavigatorFirst);

    this->BindObject(SURFACE_GAME_DEMO, &m_BackgroundDemo);
    this->BindObject(SURFACE_GAME_DEMO, &m_StartButtonDemo);
    this->BindObject(SURFACE_GAME_DEMO, &m_BackButtonDemo);
    for(coreUintW i = 0u; i < MENU_GAME_DEMOS; ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoLine[i]);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoStageIcon);
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
    case SURFACE_GAME_OPTION:  m_NavigatorOption.Update(); break;
    case SURFACE_GAME_ARMORY:  m_NavigatorArmory.Update(); break;
    case SURFACE_GAME_FIRST:   m_NavigatorFirst .Update(); break;
    case SURFACE_GAME_DEMO:    m_NavigatorDemo  .Update(); break;
    }

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
         if(m_MissionTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_GAME_MISSION);
    else if(m_SegmentTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_GAME_SEGMENT);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_GAME_MISSION:
        {
            if(m_ArcadeComplete.IsClicked())
            {
                // 
                m_iCurPage = 2u;

                // 
                this->__PrepareArcade();

                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u, true, false);
            }
            else if(m_BackButtonMain.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                //g_pMenu->ShiftSurface(this, SURFACE_GAME_OPTION, 3.0f, 2u, false, true);

                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }

            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                if(!m_aMissionLine[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreBool bFocused = m_aMissionButton[i].IsFocused() || m_aMissionLine[i].IsFocused();
                const coreBool bClicked = m_aMissionButton[i].IsClicked() || m_aMissionLine[i].IsClicked();

                if(TIME && bFocused)
                {
                    // 
                    this->__SelectMission(i);
                }

                if(bClicked)
                {
                    // 
                    m_iCurPage = 0u;

                    // 
                    this->__PrepareMission(i);

                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u, true, false);
                }

                if(m_aMissionLine[i].IsClicked()) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);

                // 
                cMenu::UpdateButton(&m_aMissionButton[i], bFocused, g_aMissionData[i].vColor2);

                // 
                m_aMissionLine[i].SetFocused(bFocused || ((i < MISSION_BASE) && m_ArcadeComplete.IsFocused()));
                cMenu::UpdateLine(&m_aMissionLine[i], false, g_aMissionData[i].vColor2);

                // 
                m_aMissionIcon[i].SetAlpha(m_aMissionIcon[i].GetAlpha() * 0.6f);
            }

            // 
            cMenu::UpdateButton(&m_ArcadeComplete, m_ArcadeComplete.IsFocused());
        }
        break;

    case SURFACE_GAME_SEGMENT:
        {
            if(m_ArcadeComplete.IsClicked())
            {
                // 
                m_iCurPage = 2u;

                // 
                this->__PrepareArcade();

                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u, true, false);
            }
            else if(m_BackButtonMain.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                //g_pMenu->ShiftSurface(this, SURFACE_GAME_OPTION, 3.0f, 2u, false, true);

                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }

            if(TIME)
            {
            //    m_aiSegmentSelection[0] = MENU_GAME_MISSIONS;
            //    m_aiSegmentSelection[1] = 0u;
            }
            if(m_ArcadeComplete.IsFocused())
            {
                m_aiSegmentSelection[0] = MENU_GAME_MISSIONS;
                m_aiSegmentSelection[1] = 0u;
            }
            
            

            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                m_aSegmentIcon[i].SetSize     (coreVector2(0.1f,0.1f));
                m_aSegmentIcon[i].SetDirection(coreVector2(1.0f,1.0f).Normalized());
                m_aSegmentIcon[i].Move();

                m_aSegmentIconBack[i].SetSize     (m_aSegmentIcon[i].GetSize());
                m_aSegmentIconBack[i].SetDirection(coreVector2(0.0f,1.0f));
                m_aSegmentIconBack[i].Move();
            }

            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
                {
                    const coreBool    bBoss = MISSION_SEGMENT_IS_BOSS(j);
                    const coreVector2 vSize = bBoss ? coreVector2(0.1f,0.05f) : coreVector2(0.05f,0.05f);

                    m_aaSegmentTile[i][j].SetSize  (vSize * 1.4f);
                    m_aaSegmentTile[i][j].SetColor3(COLOR_MENU_WHITE);
                    m_aaSegmentTile[i][j].Move();

                    m_aaSegmentBack[i][j].SetSize(m_aaSegmentTile[i][j].GetSize() + coreVector2(0.01f,0.01f));
                    m_aaSegmentBack[i][j].Move();
                }
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
                        this->__PrepareSegment(i, j);

                        // 
                        g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u, true, false);

                        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                    }

                    if(TIME && m_aaSegmentTile[i][j].IsFocused())
                    {
                        if((m_aiSegmentSelection[0] != i) || (m_aiSegmentSelection[1] != j)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);

                        m_aiSegmentSelection[0] = i;
                        m_aiSegmentSelection[1] = j;

                        i = MENU_GAME_MISSIONS;
                        j = MENU_GAME_SEGMENTS;
                    }
                }
            }
            
            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                if(m_aSegmentIconBack[i].IsClicked())
                {
                    // 
                    m_iCurPage = 0u;

                    // 
                    this->__SelectMission(i);
                    this->__PrepareMission(i);

                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u, true, false);

                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                }

                if(TIME && m_aSegmentIconBack[i].IsFocused())
                {
                    if((m_aiSegmentSelection[0] != i) || (m_aiSegmentSelection[1] != MENU_GAME_SEGMENTS))
                    {
                        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);

                        m_fSegmentTime = 0.0f;
                    }

                    m_aiSegmentSelection[0] = i;
                    m_aiSegmentSelection[1] = MENU_GAME_SEGMENTS;

                    break;
                }
            }

                
            //for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            //    m_aSegmentIcon[i].SetAlpha(m_aSegmentIcon[i].GetAlpha() * 0.6f);
            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
                m_aSegmentIconBack[i].SetAlpha(m_aSegmentIconBack[i].GetAlpha() * 0.5f);


            if(m_aiSegmentSelection[0] < MENU_GAME_MISSIONS)
            {
                if(m_aiSegmentSelection[1] < MENU_GAME_SEGMENTS)
                {
                    // 
                    this->__SelectSegment(m_aiSegmentSelection[0], m_aiSegmentSelection[1]);

                    const cGuiObject& oTile = m_aaSegmentTile[m_aiSegmentSelection[0]][m_aiSegmentSelection[1]];

                    m_aSegmentCursor[1].DefineTexture(0u, oTile.GetTexture(0u));
                    m_aSegmentCursor[1].SetPosition  (oTile.GetPosition ());
                    m_aSegmentCursor[1].SetSize      (oTile.GetSize     () * 1.45f);
                    m_aSegmentCursor[1].SetTexSize   (oTile.GetTexSize  ());
                    m_aSegmentCursor[1].SetTexOffset (oTile.GetTexOffset());
                    m_aSegmentCursor[1].Move();

                    m_aSegmentCursor[0].SetPosition(m_aSegmentCursor[1].GetPosition());
                    m_aSegmentCursor[0].SetSize    (m_aSegmentCursor[1].GetSize    () + coreVector2(0.01f,0.01f));
                    m_aSegmentCursor[0].Move();

                    m_aSegmentCursor[1].SetColor3(g_aMissionData[m_aiSegmentSelection[0]].vColor);

                }
                else
                {
                    m_aSegmentCursor[1].SetPosition(HIDDEN_POS);
                    m_aSegmentCursor[1].Move();

                    m_aSegmentCursor[0].SetPosition(m_aSegmentCursor[1].GetPosition());
                    m_aSegmentCursor[0].Move();
                    
                    m_fSegmentTime.Update(1.0f);

                    m_aSegmentIcon    [m_aiSegmentSelection[0]].SetDirection(coreVector2::Direction(m_fSegmentTime *  (0.2f*PI) - (0.25f*PI)));
                    m_aSegmentIconBack[m_aiSegmentSelection[0]].SetDirection(coreVector2::Direction(m_fSegmentTime * (-0.1f*PI)));

                    m_aSegmentIcon[m_aiSegmentSelection[0]].SetSize(coreVector2(0.1f,0.1f) * 1.2f);
                    m_aSegmentIcon[m_aiSegmentSelection[0]].Move();

                    m_aSegmentIconBack[m_aiSegmentSelection[0]].SetSize(m_aSegmentIcon[m_aiSegmentSelection[0]].GetSize     ());
                    m_aSegmentIconBack[m_aiSegmentSelection[0]].Move();

                    for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
                    {
                        const coreBool    bBoss = MISSION_SEGMENT_IS_BOSS(j);
                        const coreVector2 vSize = bBoss ? coreVector2(0.1f,0.05f) : coreVector2(0.05f,0.05f);

                        m_aaSegmentTile[m_aiSegmentSelection[0]][j].SetSize  (vSize * 1.4f * 1.1f);
                        m_aaSegmentTile[m_aiSegmentSelection[0]][j].SetColor3(g_aMissionData[m_aiSegmentSelection[0]].vColor);
                        m_aaSegmentTile[m_aiSegmentSelection[0]][j].Move();

                        m_aaSegmentBack[m_aiSegmentSelection[0]][j].SetSize(m_aaSegmentTile[m_aiSegmentSelection[0]][j].GetSize() + coreVector2(0.01f,0.01f));
                        m_aaSegmentBack[m_aiSegmentSelection[0]][j].Move();
                    }
                }
            }
            else
            {
                m_aSegmentCursor[1].SetPosition(HIDDEN_POS);
                m_aSegmentCursor[1].Move();

                m_aSegmentCursor[0].SetPosition(m_aSegmentCursor[1].GetPosition());
                m_aSegmentCursor[0].Move();
            }


            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                //cMenu::UpdateLine(&m_aSegmentLine[i], true, g_aMissionData[i].vColor2);
                m_aSegmentLine[i].SetColor3((m_aiSegmentSelection[0] == i) ? g_aMissionData[i].vColor2 : coreVector3(1.0f,1.0f,1.0f));
            }

            m_SegmentIconBig.SetAlpha(m_SegmentIconBig.GetAlpha() * 0.6f);

            m_SegmentFragment.SetTexOffset(coreVector2(0.0f, coreFloat(Core::System->GetTotalTime()) * 0.1f));

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
                this->__RefreshBase();

                // 
                //g_pMenu->ShiftSurface(this, g_pSave->GetHeader().oOptions.iNavigation ? SURFACE_GAME_MISSION : SURFACE_GAME_SEGMENT, 3.0f, 1u, false, true);
            }
            else if(m_BackButtonOption.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }

            // 
            if(m_OptionType      .GetUserSwitch()) m_ArmoryType      .SelectValue(m_OptionType      .GetCurValue());
            if(m_OptionMode      .GetUserSwitch()) m_ArmoryMode      .SelectValue(m_OptionMode      .GetCurValue());
            if(m_OptionDifficulty.GetUserSwitch()) m_ArmoryDifficulty.SelectValue(m_OptionDifficulty.GetCurValue());

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
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_READY"), [this](const coreInt32 iAnswer)
                {
                    if(iAnswer == MSGBOX_ANSWER_YES)
                    {
                        // 
                        this->SaveValues();

                        // 
                        m_iStatus = 101;

                        // 
                        m_SpeedNew.Ressolve();
                    }
                });
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
            if(m_ArmoryType.GetUserSwitch() || m_ArmoryMode.GetUserSwitch() || m_ArmoryDifficulty.GetUserSwitch())
            {
                // 
                this->__RefreshBase();

                // 
                switch(m_iCurPage)
                {
                default: ASSERT(false)
                case 0u: this->__PrepareMission(m_aiCurIndex[0]);                  break;
                case 1u: this->__PrepareSegment(m_aiCurIndex[1], m_aiCurIndex[2]); break;
                case 2u: this->__PrepareArcade();                                  break;
                }
            }

            // 
            if(m_ArmoryType      .GetUserSwitch()) m_OptionType      .SelectValue(m_ArmoryType      .GetCurValue());
            if(m_ArmoryMode      .GetUserSwitch()) m_OptionMode      .SelectValue(m_ArmoryMode      .GetCurValue());
            if(m_ArmoryDifficulty.GetUserSwitch()) m_OptionDifficulty.SelectValue(m_ArmoryDifficulty.GetCurValue());

            // 
                 if(m_ArmorySelection.GetUserSwitch() < 0) this->SelectPrevious();
            else if(m_ArmorySelection.GetUserSwitch() > 0) this->SelectNext();

            // 
            cMenu::UpdateSwitchBox(&m_ArmorySelection);
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
            m_aArmoryWeapon[1].SetOverride(bMulti ? m_aArmoryWeapon[0].GetOverride() : -1);
            m_aArmoryPlayer[1].SetColor3  (COLOR_MENU_WHITE * (bMulti ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));

            m_ArmoryIconBig.SetAlpha(m_ArmoryIconBig.GetAlpha() * 0.6f);

            m_ArmoryFragment.SetTexOffset(coreVector2(0.0f, coreFloat(Core::System->GetTotalTime()) * 0.1f));

            //for(coreUintW i = 0u; i < MENU_GAME_ARMORY_WAVES; ++i)
            //{
            //    m_aArmoryWave[i].SetAlpha(m_aArmoryWave[i].GetAlpha() * 0.6f);
            //}
            
            
            const coreFloat fRotation = coreFloat(Core::System->GetTotalTime());

            for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
            {
                if(m_aArmoryBadgeWave[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    m_aArmoryBadge[i].SetDirection(coreVector2::Direction(fRotation + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_BADGES))));
                }
                else
                {
                    m_aArmoryBadge[i].SetDirection(coreVector2(0.0f,1.0f));
                }
                m_aArmoryBadge[i].Move();
            }

            for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS; ++i)
            {
                if(m_aArmoryHelperWave[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    m_aArmoryHelper    [i].SetDirection(coreVector2::Direction(fRotation         + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_HELPERS))));
                    m_aArmoryHelperWave[i].SetDirection(coreVector2::Direction(fRotation * -0.5f + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_HELPERS))));
                }
                else
                {
                    m_aArmoryHelper    [i].SetDirection(coreVector2(0.0f,1.0f));
                    m_aArmoryHelperWave[i].SetDirection(coreVector2(0.0f,1.0f));
                }
                m_aArmoryHelper    [i].Move();
                m_aArmoryHelperWave[i].Move();

                m_aArmoryHelperWave[i].SetAlpha(m_aArmoryHelperWave[i].GetAlpha() * 0.5f);
            }
        }
        break;

    case SURFACE_GAME_FIRST:
        {
            if(m_StartButtonFirst.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_READY"), [this](const coreInt32 iAnswer)
                {
                    if(iAnswer == MSGBOX_ANSWER_YES)
                        m_iStatus = 101;
                });

                // 
                m_iCurPage      = 2u;
                m_aiCurIndex[0] = 0u;
            }
            else if(m_BackButtonFirst.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                this->SaveValues();

                // 
                m_iStatus = 2;
            }

            // 
            if(m_FirstType      .GetUserSwitch()) m_ArmoryType      .SelectValue(m_FirstType      .GetCurValue());
            if(m_FirstDifficulty.GetUserSwitch()) m_ArmoryDifficulty.SelectValue(m_FirstDifficulty.GetCurValue());
            if(m_FirstSpeed     .GetUserSwitch()) m_ArmorySpeed     .SelectValue(m_FirstSpeed     .GetCurValue());
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) if(m_aFirstShield[i].GetUserSwitch()) m_aArmoryShield[i].SelectValue(m_aFirstShield[i].GetCurValue());

            // 
            cMenu::UpdateSwitchBox(&m_FirstType);
            cMenu::UpdateSwitchBox(&m_FirstDifficulty);
            cMenu::UpdateSwitchBox(&m_FirstSpeed);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aFirstShield[i]);

            // 
            cMenu::UpdateButton(&m_StartButtonFirst, m_StartButtonFirst.IsFocused());
            cMenu::UpdateButton(&m_BackButtonFirst,  m_BackButtonFirst .IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_GAME_FIRSTS; ++i) cMenu::UpdateLine(&m_aFirstLine[i], true);

            // 
            m_aFirstShield[0].GetCaption()->SetColor3(m_aFirstShield[0].GetCurValue() ? COLOR_MENU_BLUE   : COLOR_MENU_WHITE);
            m_aFirstShield[1].GetCaption()->SetColor3(m_aFirstShield[1].GetCurValue() ? COLOR_MENU_YELLOW : COLOR_MENU_WHITE);

            const coreBool bMulti = (m_FirstType.GetCurValue() != GAME_TYPE_SOLO);
            m_aFirstShield[1].SetOverride(bMulti ? 0 : -1);
            m_aFirstPlayer[1].SetColor3  (COLOR_MENU_WHITE * (bMulti ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
        }
        break;

    case SURFACE_GAME_DEMO:
        {
            if(m_StartButtonDemo.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_READY"), [this](const coreInt32 iAnswer)
                {
                    if(iAnswer == MSGBOX_ANSWER_YES)
                        m_iStatus = 101;
                });

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
            if(m_DemoType      .GetUserSwitch()) m_ArmoryType      .SelectValue(m_DemoType      .GetCurValue());
            if(m_DemoDifficulty.GetUserSwitch()) m_ArmoryDifficulty.SelectValue(m_DemoDifficulty.GetCurValue());
            if(m_DemoSpeed     .GetUserSwitch()) m_ArmorySpeed     .SelectValue(m_DemoSpeed     .GetCurValue());
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) if(m_aDemoShield[i].GetUserSwitch()) m_aArmoryShield[i].SelectValue(m_aDemoShield[i].GetCurValue());

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
            m_aDemoPlayer[1].SetColor3  (COLOR_MENU_WHITE * (bMulti ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));

            const coreBool bUnlocked = (m_DemoStage.GetNumEntries() > 1u);
            m_DemoStage   .SetOverride(bUnlocked ? 0 : -1);
            m_aDemoName[3].SetColor3  (COLOR_MENU_WHITE * (bUnlocked ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));

            // 
            m_DemoStageIcon.SetColor3   (g_aMissionData[m_DemoStage.GetCurValue()].vColor * 0.8f);
            m_DemoStageIcon.SetTexOffset(g_aMissionData[m_DemoStage.GetCurValue()].vIcon);
            m_DemoStageIcon.SetEnabled  (bUnlocked ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // 
            m_DemoStageIcon.SetAlpha(m_DemoStageIcon.GetAlpha() * 0.6f);
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if((this->GetCurSurface() == SURFACE_GAME_MISSION) || (this->GetCurSurface() == SURFACE_GAME_SEGMENT))
    {
        // 
        cMenu::UpdateTab(&m_MissionTab, (this->GetCurSurface() == SURFACE_GAME_MISSION), m_MissionTab.IsFocused());
        cMenu::UpdateTab(&m_SegmentTab, (this->GetCurSurface() == SURFACE_GAME_SEGMENT), m_SegmentTab.IsFocused());
        
        
        //     if(this->GetCurSurface() == SURFACE_GAME_MISSION) g_pSave->EditOptions()->iNavigation = 1u;
        //else if(this->GetCurSurface() == SURFACE_GAME_SEGMENT) g_pSave->EditOptions()->iNavigation = 0u;
    }

    // 
    cMenu::UpdateButton(&m_BackButtonMain, m_BackButtonMain.IsFocused());
    
    m_BackgroundMain.SetTexOffset(coreVector2(0.0f, m_SegmentBox.GetOffset().y));                                   
}


// ****************************************************************
// 
void cGameMenu::ActivateFirstPlay()
{
    m_bFirstPlay = true;
}


// ****************************************************************
// 
void cGameMenu::DeactivateFirstPlay()
{
    m_bFirstPlay = false;
}


// ****************************************************************
// 
void cGameMenu::LoadValues()
{
    // 
    LoadConfig();

    // 
    this->__LoadUnlocks();

    // 
    m_ArmoryType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
    m_ArmoryMode      .SelectValue(g_pSave->GetHeader().oOptions.iMode);
    m_ArmoryDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
    m_ArmorySpeed     .SelectValue(g_CurConfig.Game.iGameSpeed);

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        m_aArmoryShield[i].SelectValue(g_pSave->GetHeader().oOptions.aiShield [i]);
        m_aArmoryWeapon[i].SelectValue(g_pSave->GetHeader().oOptions.aaiWeapon[i][0]);
        m_aArmoryPlayer[i].SetText    (PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
    }

    // 
    m_OptionType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
    m_OptionMode      .SelectValue(g_pSave->GetHeader().oOptions.iMode);
    m_OptionDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);

    if(m_bFirstPlay)
    {
        // 
        m_FirstType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
        m_FirstDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
        m_FirstSpeed     .SelectValue(g_CurConfig.Game.iGameSpeed);

        // 
        for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
        {
            m_aFirstShield[i].SelectValue(g_pSave->GetHeader().oOptions.aiShield[i]);
            m_aFirstPlayer[i].SetText    (PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
        }
    }

    // 
    //m_aiSegmentSelection[0] = GET_BITVALUE(g_pSave->GetHeader().oOptions.iNavigation, 5u, 0u);
    //m_aiSegmentSelection[1] = GET_BITVALUE(g_pSave->GetHeader().oOptions.iNavigation, 5u, 5u);

    if(g_bDemoVersion)
    {
        // 
        m_DemoStage.ClearEntries();

        // 
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
        m_DemoStage     .SelectValue(g_pSave->GetHeader().oOptions.iNavigation);

        // 
        for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
        {
            m_aDemoShield[i].SelectValue(g_pSave->GetHeader().oOptions.aiShield[i]);
            m_aDemoPlayer[i].SetText    (PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
        }
    }

    // 
    this->__RefreshBase();
}


// ****************************************************************
// 
void cGameMenu::SaveValues()
{
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
    //SET_BITVALUE(g_pSave->EditOptions()->iNavigation, 5u, 0u, m_aiSegmentSelection[0])
    //SET_BITVALUE(g_pSave->EditOptions()->iNavigation, 5u, 5u, m_aiSegmentSelection[1])

    // 
    if(g_bDemoVersion)
    {
        g_pSave->EditOptions()->iNavigation = m_DemoStage.GetCurValue();
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
        (*piMissionID) = cIntroMission::ID;
        (*piTakeFrom)  = 0u;
        (*piTakeTo)    = TAKE_MISSION;
        (*piKind)      = GAME_KIND_ALL;
    }
    else ASSERT(false)
}


// ****************************************************************
// 
void cGameMenu::SelectPrevious()
{
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    if(m_iCurPage == 0u)
    {
        for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
        {
            if(--m_aiCurIndex[0] >= MENU_GAME_MISSIONS)
            {
                m_aiCurIndex[0] = MENU_GAME_MISSIONS - 1u;
            }

            if(oProgress.aiAdvance[m_aiCurIndex[0]] && (m_aiCurIndex[0] < MISSION_BASE)) break;
        }

        this->__SelectMission(m_aiCurIndex[0]);
        if(this->GetAlpha()) this->__PrepareMission(m_aiCurIndex[0]);

        m_aiSegmentSelection[0] = m_aiCurIndex[0];
        m_aiSegmentSelection[1] = MENU_GAME_SEGMENTS;
        m_NavigatorMain.ForceCurrent(&m_aSegmentIcon[m_aiCurIndex[0]]);
        m_SegmentBox.ScrollToObject(&m_aSegmentIcon[m_aiCurIndex[0]], true);
    }
    else if(m_iCurPage == 1u)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            if((--m_aiCurIndex[2]) - ((m_aiCurIndex[1] == MISSION_ATER) ? 5u : 0u) >= MENU_GAME_SEGMENTS)
            {
                for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
                {
                    if(--m_aiCurIndex[1] >= MENU_GAME_MISSIONS)
                    {
                        m_aiCurIndex[1] = MENU_GAME_MISSIONS - 1u;
                    }

                    m_aiCurIndex[2] = MENU_GAME_SEGMENTS - 1u;

                    if(oProgress.aiAdvance[m_aiCurIndex[1]] && (m_aiCurIndex[1] < MISSION_BASE)) break;
                }
            }

            if(oProgress.aiAdvance[m_aiCurIndex[1]] > m_aiCurIndex[2]) break;
        }

        this->__SelectSegment(m_aiCurIndex[1], m_aiCurIndex[2]);
        if(this->GetAlpha()) this->__PrepareSegment(m_aiCurIndex[1], m_aiCurIndex[2]);

        m_aiSegmentSelection[0] = m_aiCurIndex[1];
        m_aiSegmentSelection[1] = m_aiCurIndex[2];
        m_NavigatorMain.ForceCurrent(&m_aaSegmentTile[m_aiCurIndex[1]][m_aiCurIndex[2]]);
        m_SegmentBox.ScrollToObject(&m_aaSegmentTile[m_aiCurIndex[1]][m_aiCurIndex[2]], true);
    }
    else if(m_iCurPage == 2u)
    {
        // nothing
    }
    else ASSERT(false)
}


// ****************************************************************
// 
void cGameMenu::SelectNext()
{
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    if(m_iCurPage == 0u)
    {
        for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
        {
            if(++m_aiCurIndex[0] >= MENU_GAME_MISSIONS)
            {
                m_aiCurIndex[0] = 0u;
            }

            if(oProgress.aiAdvance[m_aiCurIndex[0]] && (m_aiCurIndex[0] < MISSION_BASE)) break;
        }

        this->__SelectMission(m_aiCurIndex[0]);
        if(this->GetAlpha()) this->__PrepareMission(m_aiCurIndex[0]);

        m_aiSegmentSelection[0] = m_aiCurIndex[0];
        m_aiSegmentSelection[1] = MENU_GAME_SEGMENTS;
        m_NavigatorMain.ForceCurrent(&m_aSegmentIcon[m_aiCurIndex[0]]);
        m_SegmentBox.ScrollToObject(&m_aSegmentIcon[m_aiCurIndex[0]], true);
    }
    else if(m_iCurPage == 1u)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            if(++m_aiCurIndex[2] >= MENU_GAME_SEGMENTS)
            {
                for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
                {
                    if(++m_aiCurIndex[1] >= MENU_GAME_MISSIONS)
                    {
                        m_aiCurIndex[1] = 0u;
                    }

                    m_aiCurIndex[2] = (m_aiCurIndex[1] == MISSION_ATER) ? 5u : 0u;

                    if(oProgress.aiAdvance[m_aiCurIndex[1]] && (m_aiCurIndex[1] < MISSION_BASE)) break;
                }
            }

            if(oProgress.aiAdvance[m_aiCurIndex[1]] > m_aiCurIndex[2]) break;
        }

        this->__SelectSegment(m_aiCurIndex[1], m_aiCurIndex[2]);
        if(this->GetAlpha()) this->__PrepareSegment(m_aiCurIndex[1], m_aiCurIndex[2]);

        m_aiSegmentSelection[0] = m_aiCurIndex[1];
        m_aiSegmentSelection[1] = m_aiCurIndex[2];
        m_NavigatorMain.ForceCurrent(&m_aaSegmentTile[m_aiCurIndex[1]][m_aiCurIndex[2]]);
        m_SegmentBox.ScrollToObject(&m_aaSegmentTile[m_aiCurIndex[1]][m_aiCurIndex[2]], true);
    }
    else if(m_iCurPage == 2u)
    {
        // nothing
    }
    else ASSERT(false)
}


// ****************************************************************
// 
void cGameMenu::__LoadUnlocks()
{
    const coreBool bDuelType       = false;
    const coreBool bHardDifficulty = false;
    const coreBool bGameSpeedUp    = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_GAMESPEEDUP);
    const coreBool bAnyWeapons     = false;

    // 
    m_ArmoryType.ClearEntries();
    m_ArmoryType.AddEntryLanguage("GAME_TYPE_SOLO", GAME_TYPE_SOLO);
    m_ArmoryType.AddEntryLanguage("GAME_TYPE_COOP", GAME_TYPE_COOP);
    if(bDuelType)
    {
        m_ArmoryType.AddEntryLanguage("GAME_TYPE_DUEL", GAME_TYPE_DUEL);
    }

    // 
    m_ArmoryDifficulty.ClearEntries();
    m_ArmoryDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_ArmoryDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    if(bHardDifficulty)
    {
        m_ArmoryDifficulty.AddEntryLanguage("GAME_DIFFICULTY_HARD", GAME_DIFFICULTY_HARD);
    }

    // 
    m_ArmorySpeed.ClearEntries();
    for(coreUintW i = 50u, ie = bGameSpeedUp ? 200u : 100u; i <= ie; i += 5u) m_ArmorySpeed.AddEntry(PRINT("%zu%%", i), i);

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        // 
        m_aArmoryWeapon[i].ClearEntries();
        if(bAnyWeapons)
        {
            m_aArmoryWeapon[i].AddEntryLanguage("GAME_WEAPON_RAY", 1u);
        }
        else
        {
            m_aArmoryWeapon[i].AddEntryLanguage("UNKNOWN", 1u);
        }

        // 
        m_aArmoryWeapon[i].SetOverride(bAnyWeapons ? 0 : -1);
    }

    // 
    m_aArmoryName[5].SetColor3      (COLOR_MENU_WHITE * (bAnyWeapons ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
    m_aArmoryName[5].SetTextLanguage(bAnyWeapons ? "GAME_WEAPON" : "UNKNOWN");
}


// ****************************************************************
// 
void cGameMenu::__RefreshBase()
{
    // 
    m_iBaseType       = m_ArmoryType      .GetCurValue();
    m_iBaseMode       = m_ArmoryMode      .GetCurValue();
    m_iBaseDifficulty = m_ArmoryDifficulty.GetCurValue();

    // 
    m_aiCurIndex[3] = 1u;

    // 
    m_OptionState.SetText(PRINT("%s / %s", m_ArmoryType.GetCurText(), m_ArmoryDifficulty.GetCurText()));

    // 
    coreUintW iMaxMission = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        const coreUint8        iAdvance = g_pSave->GetHeader().oProgress.aiAdvance[i] * ((i < MISSION_BASE) ? 1u : 0u);
        const coreObjectEnable eEnabled = iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            const coreBool         bVisible  = (iAdvance >= j + 1u) && ((i != 8u) || MISSION_SEGMENT_IS_BOSS(j));
            const coreObjectEnable eEnabled2 = bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

            m_aaSegmentTile[i][j].SetEnabled  (eEnabled2);
            m_aaSegmentTile[i][j].SetFocusable(bVisible);
            m_aaSegmentBack[i][j].SetEnabled  (eEnabled2);
            
            
            //cMenu::ApplyMedalTexture(&m_aaSegmentMedal[i][j], g_pSave->GetHeader().oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][i][j], MISSION_SEGMENT_IS_BOSS(j) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);
            //for(coreUintW k = 0u; k < MENU_GAME_ARMORY_BADGES; ++k)
            //{
            //    const coreBool bState = HAS_BIT(g_pSave->GetHeader().oProgress.aiBadge[i], k + BADGES * j);
    //
            //    m_aaSegmentBadge[i][j][k].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
            //    m_aaSegmentBadge[i][j][k].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
            //    m_aaSegmentBadge[i][j][k].SetEnabled  (CORE_OBJECT_ENABLE_ALL);
            //}
        }

        m_aMissionName  [i].SetEnabled  (eEnabled);
        m_aMissionScore [i].SetEnabled  (eEnabled);
        m_aMissionTime  [i].SetEnabled  (eEnabled);
        m_aMissionIcon  [i].SetEnabled  (eEnabled);
        m_aMissionLine  [i].SetEnabled  (eEnabled);
        m_aMissionLine  [i].SetFocusable(iAdvance);
        m_aMissionButton[i].SetEnabled  (eEnabled);
        m_aMissionButton[i].SetFocusable(iAdvance);

        m_aSegmentName  [i].SetEnabled(eEnabled);
        m_aSegmentNumber[i].SetEnabled(eEnabled);
        m_aSegmentIcon  [i].SetEnabled(eEnabled);
        m_aSegmentIconBack[i].SetEnabled(eEnabled);
        m_aSegmentLine  [i].SetEnabled(eEnabled);

        m_aSegmentNumber[i].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetMissionLetters(i)));

        //const auto&     oStats     = g_pSave->GetHeader().aaaaLocalStatsMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][i];
        //const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
        //const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);
//
        //m_aMissionScore[i].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)     : "-");
        //m_aMissionTime [i].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift) : "-");
//
        //cMenu::ApplyMedalTexture(&m_aMissionMedal[i], g_pSave->GetHeader().oProgress.aaaaiMedalMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][i], MEDAL_TYPE_MISSION, false);

        if(iAdvance) iMaxMission = i;
    }

    // 
    m_SegmentBox.SetMaxOffset(0.25f * I_TO_F(iMaxMission + 1u) - m_SegmentBox.GetSize().y + 0.15f);
}


// ****************************************************************
// 
void cGameMenu::__SelectMission(const coreUintW iMissionIndex)
{
    ASSERT(iMissionIndex < MISSIONS)

    // 
    if((m_aiCurIndex[0] == iMissionIndex) && !m_aiCurIndex[3]) return;
    m_aiCurIndex[0] = iMissionIndex;
    m_aiCurIndex[3] = 0u;
}


// ****************************************************************
// 
void cGameMenu::__SelectSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT((iMissionIndex < MISSIONS) && (iSegmentIndex < SEGMENTS))

    // 
    if((m_aiCurIndex[1] == iMissionIndex) && (m_aiCurIndex[2] == iSegmentIndex) && !m_aiCurIndex[3]) return;
    m_aiCurIndex[1] = iMissionIndex;
    m_aiCurIndex[2] = iSegmentIndex;
    m_aiCurIndex[3] = 0u;
/*
    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];
    const coreBool      bBoss = MISSION_SEGMENT_IS_BOSS(iSegmentIndex);

    // 
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaaaaLocalStatsSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][iSegmentIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    m_aSegmentTitle    [0].SetColor3(oData.vColor);
    m_aSegmentScore    [0].SetColor3(oData.vColor);
    m_aSegmentTime     [0].SetColor3(oData.vColor);
    m_aSegmentMaxSeries[0].SetColor3(oData.vColor);

    // 
    m_aSegmentTitle    [0].SetText(bBoss ? oData.pcBoss : oData.pcName);
    m_aSegmentTitle    [1].SetText(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aSegmentScore    [1].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)                                                                                    : "-");
    m_aSegmentTime     [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift)                                                                                : "-");
    m_aSegmentMaxSeries[1].SetText(oStats.iMaxSeries ? (bBoss ? coreData::ToChars(oStats.iMaxSeries) : PRINT("x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u)) : "-");

    // 
    m_aSegmentMaxSeries[0].SetTextLanguage(bBoss ? "GAME_CHAIN" : "GAME_COMBO");

    // 
    cMenu::ApplyMedalTexture(&m_SegmentMedal, oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][iSegmentIndex], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);

    if(bBoss)
    {
        // 
        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
        {
            m_aSegmentBadge    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aSegmentBadgeWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }

        // 
        m_SegmentFragment.DefineTexture(0u, g_aFragmentData[iMissionIndex].iIndex ? PRINT("menu_fragment_%02zu.png", g_aFragmentData[iMissionIndex].iIndex) : NULL);
        m_SegmentFragment.SetSize      (g_aFragmentData[iMissionIndex].vSize.LowRatio() * 0.1f);
        m_SegmentFragment.SetColor3    (g_aFragmentData[iMissionIndex].vColor);
        m_SegmentFragment.SetEnabled   (HAS_BIT(oProgress.aiFragment[iMissionIndex], 0u) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
    else
    {
        // 
        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
        {
            const coreBool bState = HAS_BIT(oProgress.aiBadge[iMissionIndex], i + BADGES * iSegmentIndex);

            m_aSegmentBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
            m_aSegmentBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
            m_aSegmentBadge[i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);

            m_aSegmentBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        }

        // 
        m_SegmentFragment.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_SegmentIconBig.SetColor3   (oData.vColor * 0.8f);
    m_SegmentIconBig.SetTexOffset(oData.vIcon);
    */
}


// ****************************************************************
// 
void cGameMenu::__PrepareMission(const coreUintW iMissionIndex)
{
    ASSERT(iMissionIndex < MISSIONS)

    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];

    // 
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaaaLocalStatsMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex];
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
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], oProgress.aaaaiMedalMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex], MEDAL_TYPE_MISSION, false);

    // 
    for(coreUintW i = 1u; i < 7u; ++i)
    {
        const coreBool bBoss = MISSION_SEGMENT_IS_BOSS(i - 1u);

        cMenu::ApplyMedalTexture(&m_aArmoryMedal[i], oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][i - 1u], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);
    }

    // 
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
    {
        const coreBool bState = HAS_BIT(oProgress.aiBadge[iMissionIndex], (i % MENU_GAME_ARMORY_BADGES) + BADGES * (i / MENU_GAME_ARMORY_BADGES));

        m_aArmoryBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
        m_aArmoryBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
        m_aArmoryBadge[i].SetEnabled  (CORE_OBJECT_ENABLE_ALL);

        m_aArmoryBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_ArmoryFragment.DefineTexture(0u, g_aFragmentData[iMissionIndex].iIndex ? PRINT("menu_fragment_%02zu.png", g_aFragmentData[iMissionIndex].iIndex) : NULL);
    m_ArmoryFragment.SetSize      (g_aFragmentData[iMissionIndex].vSize * 0.2f);
    m_ArmoryFragment.SetColor3    (g_aFragmentData[iMissionIndex].vColor);
    m_ArmoryFragment.SetEnabled   (HAS_BIT(oProgress.aiFragment[iMissionIndex], 0u) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_ArmoryIconBig.SetColor3   (oData.vColor * 0.8f);
    m_ArmoryIconBig.SetTexOffset(oData.vIcon);
    m_ArmoryIconBig.SetEnabled  (CORE_OBJECT_ENABLE_ALL);

    // 
    m_ArmoryMedalBack   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryFragmentBack.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 7u; i < MENU_GAME_ARMORY_MEDALS;        ++i) m_aArmoryMedal     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeBack [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeDesc [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;       ++i) m_aArmoryHelper    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;       ++i) m_aArmoryHelperWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS;         ++i) m_aArmoryIcon      [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTitle);     ++i) m_aArmoryTitle     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryScore);     ++i) m_aArmoryScore     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTime);      ++i) m_aArmoryTime      [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryMaxSeries); ++i) m_aArmoryMaxSeries [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    
    for(coreUintW i = 1u; i < 7u; ++i)
    {
        m_aArmoryMedal[i].SetPosition(coreVector2(m_ArmoryArea2.GetPosition().x + (I_TO_F(i - 1u) - 2.5f) * 0.14f, m_ArmoryMedalBack.GetPosition().y));
        m_aArmoryMedal[i].SetSize    (coreVector2(1.0f,1.0f) * ((i == 6u) ? 0.09f : 0.08f));
        m_aArmoryMedal[i].Move();
    }
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
    {
        m_aArmoryBadge[i].SetPosition(m_aArmoryMedal[(i / MENU_GAME_ARMORY_BADGES) + 1u].GetPosition() + coreVector2(I_TO_F((i % MENU_GAME_ARMORY_BADGES) - 1u) * 0.05f, ((i % MENU_GAME_ARMORY_BADGES) == 1u) ? 0.08f : 0.06f) * 0.8f);
        m_aArmoryBadge[i].SetSize    (coreVector2(0.05f,0.05f));
        m_aArmoryBadge[i].Move();

        m_aArmoryBadgeWave[i].SetPosition(m_aArmoryBadge[i].GetPosition());
        m_aArmoryBadgeWave[i].SetSize    (m_aArmoryBadge[i].GetSize() * 0.7f);
        m_aArmoryBadgeWave[i].Move();
    }
    
    if(iMissionIndex == MISSION_INTRO)
    {
        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
        {
            if((i != 3u) && (i != 9u) && (i != 12u))
            {
                m_aArmoryBadge    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                m_aArmoryBadgeWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            }
        }
    }
    else if(iMissionIndex == MISSION_ATER)
    {
        cMenu::ApplyMedalTexture(&m_aArmoryMedal[4], oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][6], MEDAL_TYPE_BOSS, true);

        const coreBool bEigengrau = m_aArmoryMedal[4].IsEnabled(CORE_OBJECT_ENABLE_ALL);

        for(coreUintW i = 1u; i < 5u; ++i)
        {
            m_aArmoryMedal[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
        for(coreUintW i = 5u; i < 7u; ++i)
        {
            m_aArmoryMedal[i].SetPosition(coreVector2(m_ArmoryArea2.GetPosition().x + (I_TO_F(i - 5u) - 0.5f) * (bEigengrau ? -0.14f : 0.0f), m_ArmoryMedalBack.GetPosition().y));   // inverted
            m_aArmoryMedal[i].SetSize    (coreVector2(1.0f,1.0f) * 0.09f);
            m_aArmoryMedal[i].Move();
        }
        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
        {
            m_aArmoryBadge    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aArmoryBadgeWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    }

    m_ArmoryFragment.SetPosition(m_aArmoryMedal[6].GetPosition() + coreVector2(0.0f,0.05f) + g_aFragmentData[iMissionIndex].vShift);
    m_ArmoryFragment.Move();

    m_ArmoryFragmentBack.SetPosition(m_aArmoryMedal[6].GetPosition() + coreVector2(0.0f,0.05f));
    m_ArmoryFragmentBack.Move();
    
    m_ArmorySelection.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryTile.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    m_ArmoryLogo.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    m_NavigatorArmory.SetShoulder(true);
}


// ****************************************************************
// 
void cGameMenu::__PrepareSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT((iMissionIndex < MISSIONS) && (iSegmentIndex < SEGMENTS))

    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];
    const coreBool      bBoss = MISSION_SEGMENT_IS_BOSS(iSegmentIndex);

    // 
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaaaaLocalStatsSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][iSegmentIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    m_aArmoryTitle    [0].SetColor3(oData.vColor);
    m_aArmoryScore    [0].SetColor3(oData.vColor);
    m_aArmoryTime     [0].SetColor3(oData.vColor);
    m_aArmoryMaxSeries[0].SetColor3(oData.vColor);

    // 
    m_aArmoryTitle    [0].SetText(bBoss ? oData.pcBoss : oData.pcName);
    m_aArmoryTitle    [1].SetText(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aArmoryScore    [1].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)                                                                                    : "-");
    m_aArmoryTime     [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift)                                                                                : "-");
    m_aArmoryMaxSeries[1].SetText(oStats.iMaxSeries ? (bBoss ? coreData::ToChars(oStats.iMaxSeries) : PRINT("x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u)) : "-");

    // 
    m_aArmoryMaxSeries[0].SetTextLanguage(bBoss ? "GAME_CHAIN" : "GAME_COMBO");

    // 
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][iSegmentIndex], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);

    
    
    if(bBoss)
    {

        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
        {
            m_aArmoryBadgeWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }

        // 
        m_ArmoryFragment.DefineTexture(0u, g_aFragmentData[iMissionIndex].iIndex ? PRINT("menu_fragment_%02zu.png", g_aFragmentData[iMissionIndex].iIndex) : NULL);
        m_ArmoryFragment.SetSize      (g_aFragmentData[iMissionIndex].vSize * 0.2f);
        m_ArmoryFragment.SetColor3    (g_aFragmentData[iMissionIndex].vColor);
        m_ArmoryFragment.SetEnabled   (HAS_BIT(oProgress.aiFragment[iMissionIndex], 0u) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
    else
    {
        // 
        const coreBool abState[] =
        {
            HAS_BIT(oProgress.aiBadge[iMissionIndex], 0u + BADGES * iSegmentIndex),
            HAS_BIT(oProgress.aiBadge[iMissionIndex], 1u + BADGES * iSegmentIndex),
            HAS_BIT(oProgress.aiBadge[iMissionIndex], 2u + BADGES * iSegmentIndex),
            (oProgress.aiAdvance[iMissionIndex] > iSegmentIndex + 1u)
        };
        STATIC_ASSERT(ARRAY_SIZE(abState) >= MENU_GAME_ARMORY_BADGES);

        // 
        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
        {
            const coreBool bState   = abState[i];
            const coreBool bVisible = ((i == 0u) && (abState[0] || abState[3])) || ((i == 1u) && (abState[0] || abState[1])) || ((i == 2u) && ((abState[0] && abState[1]) || abState[2])) || (iMissionIndex == MISSION_INTRO);

            m_aArmoryBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
            m_aArmoryBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));

            m_aArmoryBadgeWave[i].SetEnabled     (bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aArmoryBadgeDesc[i].SetTextLanguage(bVisible ? PRINT("BADGE_DESC_%02d_%02zu_%02zu", oData.iID, iSegmentIndex + 1u, i + 1u) : "UNKNOWN");
        }

        // 
        m_ArmoryFragment.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_ArmoryIconBig.SetColor3   (oData.vColor * 0.8f);
    m_ArmoryIconBig.SetTexOffset(oData.vIcon);
    m_ArmoryIconBig.SetEnabled  (CORE_OBJECT_ENABLE_ALL);

    // 
    m_ArmoryMedalBack   .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryFragmentBack.SetEnabled(bBoss ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 1u;                      i < MENU_GAME_ARMORY_MEDALS;        ++i) m_aArmoryMedal     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_BADGES_ALL;    ++i) m_aArmoryBadge     [i].SetEnabled((i < MENU_GAME_ARMORY_BADGES && !bBoss) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = MENU_GAME_ARMORY_BADGES; i < MENU_GAME_ARMORY_BADGES_ALL;    ++i) m_aArmoryBadgeWave [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeBack [i].SetEnabled((i == 1u || !bBoss) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeDesc [i].SetEnabled(bBoss ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_HELPERS;       ++i) m_aArmoryHelper    [i].SetEnabled(bBoss ? CORE_OBJECT_ENABLE_ALL     : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_ICONS;         ++i) m_aArmoryIcon      [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < ARRAY_SIZE(m_aArmoryTitle);     ++i) m_aArmoryTitle     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < ARRAY_SIZE(m_aArmoryScore);     ++i) m_aArmoryScore     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < ARRAY_SIZE(m_aArmoryTime);      ++i) m_aArmoryTime      [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < ARRAY_SIZE(m_aArmoryMaxSeries); ++i) m_aArmoryMaxSeries [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS; ++i)
    {
        coreUintW iFragment;
        switch(i)
        {
        default: ASSERT(false)
        case 0u: iFragment = 3u; break;
        case 1u: iFragment = 5u; break;
        case 2u: iFragment = 7u; break;
        case 3u: iFragment = 4u; break;
        case 4u: iFragment = 0u; break;
        case 5u: iFragment = 6u; break;
        case 6u: iFragment = 2u; break;
        case 7u: iFragment = 1u; break;
        }

        const coreBool bState = HAS_BIT(g_pSave->GetHeader().oProgress.aiHelper[iMissionIndex], i) || ((iMissionIndex == MISSION_ATER) && g_pSave->GetHeader().oProgress.aiFragment[iFragment]);

        if(bBoss && bState)
        {
            coreVector3 vColor;
            switch(i)
            {
            default: ASSERT(false)
            case 0u: vColor = COLOR_MENU_YELLOW;  break;
            case 1u: vColor = COLOR_MENU_ORANGE;  break;
            case 2u: vColor = COLOR_MENU_RED;     break;
            case 3u: vColor = COLOR_MENU_MAGENTA; break;
            case 4u: vColor = COLOR_MENU_PURPLE;  break;
            case 5u: vColor = COLOR_MENU_BLUE;    break;
            case 6u: vColor = COLOR_MENU_CYAN;    break;
            case 7u: vColor = COLOR_MENU_GREEN;   break;
            }

            m_aArmoryHelper[i].SetColor3(vColor);

            m_aArmoryHelperWave[i].SetColor3(vColor);
            m_aArmoryHelperWave[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        }
        else
        {
            m_aArmoryHelper[i].SetColor3(coreVector3(1.0f,1.0f,1.0f) * 0.5f);

            m_aArmoryHelperWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    }
    

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        m_aArmoryBadge[i].SetPosition(m_aArmoryBadgeBack[i].GetPosition());
        m_aArmoryBadge[i].SetSize    (coreVector2(0.06f,0.06f));
        m_aArmoryBadge[i].Move();

        m_aArmoryBadgeWave[i].SetPosition(m_aArmoryBadge[i].GetPosition());
        m_aArmoryBadgeWave[i].SetSize    (m_aArmoryBadge[i].GetSize() * 0.7f);
        m_aArmoryBadgeWave[i].Move();
    }

    m_ArmoryFragment.SetPosition(m_aArmoryBadge[1].GetPosition() + g_aFragmentData[iMissionIndex].vShift);
    m_ArmoryFragment.Move();

    m_ArmoryFragmentBack.SetPosition(m_aArmoryBadge[1].GetPosition());
    m_ArmoryFragmentBack.Move();

    
    
    m_ArmorySelection.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryTile.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryBack.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    m_ArmoryLogo.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    const coreVector2 vSize = (bBoss ? coreVector2(0.1f,0.05f) : coreVector2(0.05f,0.05f)) * 1.8f;

    m_ArmoryTile.DefineTexture   (0u, bBoss ? "menu_segment_boss.png" : "menu_segment_wave.png");
    m_ArmoryTile.SetPosition     (m_ArmoryArea2.GetPosition() + coreVector2(0.395f - vSize.x * 0.5f,0.0f));
    m_ArmoryTile.SetSize         (vSize);
    m_ArmoryTile.SetColor3       (oData.vColor);
    m_ArmoryTile.SetTexSize      (m_aaSegmentTile[iMissionIndex][iSegmentIndex].GetTexSize());
    m_ArmoryTile.SetTexOffset    (m_aaSegmentTile[iMissionIndex][iSegmentIndex].GetTexOffset());
    m_ArmoryTile.Move();

    m_ArmoryBack.SetPosition  (m_ArmoryTile.GetPosition());
    m_ArmoryBack.SetSize      (m_ArmoryTile.GetSize() + coreVector2(0.01f,0.01f));
    m_ArmoryBack.Move();
    
    
    m_NavigatorArmory.SetShoulder(true);
}


// ****************************************************************
// 
void cGameMenu::__PrepareArcade()
{
    // 
    const sMissionData& oData = g_aMissionData[MISSION_ATER];

    // 
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaaLocalStatsArcade[m_iBaseType][m_iBaseMode][m_iBaseDifficulty];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    m_aArmoryScore[0].SetColor3(oData.vColor);
    m_aArmoryTime [0].SetColor3(oData.vColor);

    // 
    m_aArmoryScore[1].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)     : "-");
    m_aArmoryTime [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift) : "-");
    
    if(oStats.iScoreBest && !fBestTime)
    {
        m_aArmoryTime[1].SetTextLanguage("UNKNOWN");
    }

    // 
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], oProgress.aaaiMedalArcade[m_iBaseType][m_iBaseMode][m_iBaseDifficulty], MEDAL_TYPE_ARCADE, false);
    
    
    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        cMenu::ApplyMedalTexture(&m_aArmoryMedal[i], oProgress.aaaaiMedalMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][i - 1u], MEDAL_TYPE_MISSION, false);
    }
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS; ++i)
    {
        m_aArmoryIcon[i].SetEnabled(oProgress.aaaaiMedalMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][i] ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
    
    
    
    // 
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL;    ++i) m_aArmoryBadge     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL;    ++i) m_aArmoryBadgeWave [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeBack [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;        ++i) m_aArmoryBadgeDesc [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;       ++i) m_aArmoryHelper    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;       ++i) m_aArmoryHelperWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTitle);     ++i) m_aArmoryTitle     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryScore);     ++i) m_aArmoryScore     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTime);      ++i) m_aArmoryTime      [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryMaxSeries); ++i) m_aArmoryMaxSeries [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    m_ArmoryMedalBack   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryFragment    .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryFragmentBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryIconBig     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    m_ArmorySelection.SetEnabled(CORE_OBJECT_ENABLE_MOVE);   // #
    m_ArmoryTile.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    m_ArmoryLogo.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    
    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        m_aArmoryMedal[i].SetPosition(coreVector2(m_ArmoryArea2.GetPosition().x + (I_TO_F(i - 1u) - 4.0f) * 0.088f, m_ArmoryMedalBack.GetPosition().y));
        m_aArmoryMedal[i].SetSize      (coreVector2(1.0f,1.0f) * 0.08f);
        m_aArmoryMedal[i].Move();
    }
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS; ++i)
    {
        m_aArmoryIcon[i].SetPosition(m_aArmoryMedal[i + 1u].GetPosition() + coreVector2(0.0f,0.04f));
        m_aArmoryIcon[i].Move();
        
        STATIC_ASSERT(MENU_GAME_ARMORY_ICONS + 1u == MENU_GAME_ARMORY_MEDALS)
    }
    
    m_NavigatorArmory.SetShoulder(false);
}