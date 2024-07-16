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
, m_iSegmentLast       (0u)
, m_fSegmentTime       (0.0f)
, m_iArmoryStartIndex  (0u)
, m_iCurPage           (0u)
, m_aiCurIndex         {coreUintW(-1), coreUintW(-1), coreUintW(-1), 0u}
, m_bFromTrophy        (false)
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

    m_BackgroundArmory.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundArmory.DefineProgram("menu_border_program");
    m_BackgroundArmory.SetPosition  (coreVector2(0.0f,0.045f));
    m_BackgroundArmory.SetSize      (coreVector2(0.9f,0.82f));

    m_BackgroundFirst.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundFirst.DefineProgram("menu_border_program");
    m_BackgroundFirst.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundFirst.SetSize      (coreVector2(0.9f,0.405f));

    m_BackgroundDemo.DefineTexture(0u, "menu_background_black.png");
    m_BackgroundDemo.DefineProgram("menu_border_program");
    m_BackgroundDemo.SetPosition  (coreVector2(0.0f,0.01f));
    m_BackgroundDemo.SetSize      (coreVector2(0.9f,0.48f));

    m_StartButtonArmory.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButtonArmory.DefineProgram("menu_border_program");
    m_StartButtonArmory.SetPosition  (m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f) + MENU_BUTTON_SHIFT);
    m_StartButtonArmory.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButtonArmory.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButtonArmory.GetCaption()->SetTextLanguage("START");

    m_StartButtonFirst.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButtonFirst.DefineProgram("menu_border_program");
    m_StartButtonFirst.SetPosition  (m_BackgroundFirst.GetPosition() + m_BackgroundFirst.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f) + MENU_BUTTON_SHIFT);
    m_StartButtonFirst.SetSize      (m_StartButtonArmory.GetSize());
    m_StartButtonFirst.SetAlignment (m_StartButtonArmory.GetAlignment());
    m_StartButtonFirst.GetCaption()->SetTextLanguage("START");

    m_StartButtonDemo.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButtonDemo.DefineProgram("menu_border_program");
    m_StartButtonDemo.SetPosition  (m_BackgroundDemo.GetPosition() + m_BackgroundDemo.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f) + MENU_BUTTON_SHIFT);
    m_StartButtonDemo.SetSize      (m_StartButtonArmory.GetSize());
    m_StartButtonDemo.SetAlignment (m_StartButtonArmory.GetAlignment());
    m_StartButtonDemo.GetCaption()->SetTextLanguage("START");

    m_InputButtonArmory.Construct    (MENU_BUTTON, MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_InputButtonArmory.DefineProgram("menu_border_program");
    m_InputButtonArmory.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButtonArmory.GetSize().y);
    m_InputButtonArmory.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_InputButtonArmory.GetCaption()->SetText(ICON_SWITCH);

    m_InputButtonFirst.Construct    (MENU_BUTTON, MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_InputButtonFirst.DefineProgram("menu_border_program");
    m_InputButtonFirst.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButtonFirst.GetSize().y);
    m_InputButtonFirst.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_InputButtonFirst.GetCaption()->SetText(ICON_SWITCH);

    m_InputButtonDemo.Construct    (MENU_BUTTON, MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_InputButtonDemo.DefineProgram("menu_border_program");
    m_InputButtonDemo.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButtonDemo.GetSize().y);
    m_InputButtonDemo.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_InputButtonDemo.GetCaption()->SetText(ICON_SWITCH);

    m_BackButtonMain.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButtonMain.DefineProgram("menu_border_program");
    m_BackButtonMain.SetPosition  (m_BackgroundMain.GetPosition() + m_BackgroundMain.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButtonMain.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButtonArmory.GetSize().y);
    m_BackButtonMain.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButtonMain.GetCaption()->SetText(ICON_SHARE);

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

    m_InputButtonArmory.SetPosition(m_BackButtonArmory.GetPosition() - coreVector2(m_BackButtonArmory.GetSize().x + 0.02f, 0.0f));
    m_InputButtonFirst .SetPosition(m_BackButtonFirst .GetPosition() - coreVector2(m_BackButtonFirst .GetSize().x + 0.02f, 0.0f));
    m_InputButtonDemo  .SetPosition(m_BackButtonDemo  .GetPosition() - coreVector2(m_BackButtonDemo  .GetSize().x + 0.02f, 0.0f));

    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        const coreFloat fHeight = m_BackgroundMain.GetPosition().y + m_BackgroundMain.GetSize().y*0.5f + -0.35f - 0.025f*I_TO_F(iOffset);

        m_aSegmentName[i].Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aSegmentName[i].SetPosition (coreVector2(m_BackgroundMain.GetPosition().x - m_BackgroundMain.GetSize().x*0.5f, fHeight) + coreVector2(0.03f,0.0995f));
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
        m_aSegmentIcon[i].SetColor3    (g_aMissionData[i].vColor * 0.8f);
        m_aSegmentIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aSegmentIcon[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aSegmentIconBack[i].DefineTexture(0u, "menu_helper.png");
        m_aSegmentIconBack[i].DefineProgram("menu_helper_program");
        m_aSegmentIconBack[i].SetPosition  (m_aSegmentIcon[i].GetPosition ());
        m_aSegmentIconBack[i].SetSize      (m_aSegmentIcon[i].GetSize     ());
        m_aSegmentIconBack[i].SetColor3    (m_aSegmentIcon[i].GetColor3   ());
        m_aSegmentIconBack[i].SetTexSize   (m_aSegmentIcon[i].GetTexSize  ());
        m_aSegmentIconBack[i].SetTexOffset (m_aSegmentIcon[i].GetTexOffset());

        m_aSegmentLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aSegmentLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aSegmentLine[i].DefineProgram("menu_inner_program");
        m_aSegmentLine[i].SetPosition  (coreVector2(0.0f, fHeight));
        m_aSegmentLine[i].SetSize      (coreVector2(m_BackgroundMain.GetSize().x, 0.15f));
        m_aSegmentLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));

        iOffset += (i == 8u) ? 11u : 10u;
    }

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            const coreBool    bBoss = MISSION_SEGMENT_IS_BOSS(j);
            const coreVector2 vPos  = coreVector2((I_TO_F(j) - 3.5f) * 0.1f, m_aSegmentLine[i].GetPosition().y);
            const coreVector2 vSize = bBoss ? coreVector2(0.1f,0.05f) : coreVector2(0.05f,0.05f);

            m_aaSegmentTile[i][j].DefineProgram   ("menu_segment_program");
            m_aaSegmentTile[i][j].SetPosition     (vPos + (bBoss ? coreVector2(0.015f,0.0f) : coreVector2(-0.02f,0.0f)) + coreVector2(0.145f,0.0f));
            m_aaSegmentTile[i][j].SetSize         (vSize * 1.4f);
            m_aaSegmentTile[i][j].SetColor3       (COLOR_MENU_WHITE);
            m_aaSegmentTile[i][j].SetFocusable    (true);
            m_aaSegmentTile[i][j].SetFocusModifier(coreVector2(1.25f,1.25f));

            m_aaSegmentBack[i][j].DefineProgram("menu_color_program");
            m_aaSegmentBack[i][j].SetPosition  (m_aaSegmentTile[i][j].GetPosition());
            m_aaSegmentBack[i][j].SetSize      (m_aaSegmentTile[i][j].GetSize() + coreVector2(0.01f,0.01f));
            m_aaSegmentBack[i][j].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.15f);
            m_aaSegmentBack[i][j].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);
        }

        for(coreUintW j = 0u; j < MENU_GAME_ARROWS; ++j)
        {
            m_aaSegmentArrow[i][j].DefineTexture(0u, "menu_arrow.png");
            m_aaSegmentArrow[i][j].DefineProgram("menu_single_program");
            m_aaSegmentArrow[i][j].SetPosition  (m_aaSegmentTile[i][j].GetPosition() + coreVector2(0.05f,0.0f));
            m_aaSegmentArrow[i][j].SetSize      (coreVector2(0.021f,0.021f));
            m_aaSegmentArrow[i][j].SetColor3    (COLOR_MENU_WHITE);
        }
    }

    m_aSegmentCursor[0].DefineProgram("menu_color_program");
    m_aSegmentCursor[0].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);   // less    
    m_aSegmentCursor[1].DefineProgram("menu_segment_program");
    m_aSegmentCursor[2].DefineTexture(0u, "effect_headlight_point.png");
    m_aSegmentCursor[2].DefineProgram("menu_single_program");
    m_aSegmentCursor[2].SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_SegmentMedal.DefineTexture(0u, "menu_medal.png");
    m_SegmentMedal.DefineProgram("default_2d_program");
    m_SegmentMedal.SetSize      (coreVector2(1.0f,1.0f) * 0.07f * 1.4f);
    m_SegmentMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        m_aSegmentBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aSegmentBadge[i].DefineProgram("default_2d_program");
        m_aSegmentBadge[i].SetSize      (coreVector2(0.05f,0.05f) * 1.5f);
        m_aSegmentBadge[i].SetTexSize   (coreVector2(0.5f,1.0f));

        m_aSegmentBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aSegmentBadgeWave[i].DefineProgram("menu_single_program");
        m_aSegmentBadgeWave[i].SetSize      (m_aSegmentBadge[i].GetSize() * 0.7f);
    }

    m_SegmentBox.SetPosition(m_BackgroundMain.GetPosition() + coreVector2(0.0f,0.0f));
    m_SegmentBox.SetSize    (coreVector2(m_BackgroundMain.GetSize().x, 0.77f));
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentLine    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentIconBack[i]);        
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentIcon    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentName    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aSegmentNumber  [i]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS;          ++j) m_SegmentBox.BindObject(&m_aaSegmentBack[i][j]);
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS - 1u;     ++j) m_SegmentBox.BindObject(&m_aaSegmentTile[i][j]);   // waves
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) m_SegmentBox.BindObject(&m_aaSegmentTile[i][MENU_GAME_SEGMENTS - 1u]);                                         // bosses
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i) for(coreUintW j = 0u; j < MENU_GAME_ARROWS;            ++j) m_SegmentBox.BindObject(&m_aaSegmentArrow[i][j]);
    m_SegmentBox.BindObject(&m_aSegmentCursor[2]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) m_SegmentBox.BindObject(&m_aSegmentBadgeWave[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i) m_SegmentBox.BindObject(&m_aSegmentBadge    [i]);
    m_SegmentBox.BindObject(&m_SegmentMedal);
    m_SegmentBox.BindObject(&m_aSegmentCursor[0]);
    m_SegmentBox.BindObject(&m_aSegmentCursor[1]);

    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i)
    {
        const coreFloat fHeight = m_BackgroundMain.GetPosition().y + m_BackgroundMain.GetSize().y*0.5f + -0.125f;

        m_aArcadeName[i].Construct   (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
        m_aArcadeName[i].SetPosition (coreVector2(m_BackgroundMain.GetPosition().x - m_BackgroundMain.GetSize().x*0.5f, fHeight) + coreVector2(0.03f,0.0745f));
        m_aArcadeName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aArcadeName[i].SetColor3   (COLOR_MENU_WHITE);

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
    m_aArcadeName[0].SetTextLanguage("GAME_ARCADE");

    m_ArcadeComplete.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArcadeComplete.SetPosition (coreVector2( 0.00f,1.00f) * m_aArcadeLine[0].GetPosition());
    m_ArcadeComplete.SetSize     (coreVector2( 0.35f,0.05f));
    m_ArcadeComplete.GetCaption()->SetTextLanguage("GAME_ARCADE_COMPLETE");

    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i) m_SegmentBox.BindObject(&m_aArcadeLine[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARCADES; ++i) m_SegmentBox.BindObject(&m_aArcadeName[i]);
    m_SegmentBox.BindObject(&m_ArcadeComplete);

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
    m_aArmoryName[2].SetTextLanguage("GAME_GAMESPEED");
    m_aArmoryName[3].SetTextLanguage("GAME_DIFFICULTY");
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
        m_aArmoryMedal[i].SetPosition  (i ? coreVector2(0.0f,0.0f) : (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.01f)));
        m_aArmoryMedal[i].SetSize      (coreVector2(1.0f,1.0f) * (i ? 0.0f : 0.115f));
        m_aArmoryMedal[i].SetTexSize   (coreVector2(0.25f,0.25f));

        m_aArmoryMedalBest[i].DefineTexture(0u, "menu_medal.png");
        m_aArmoryMedalBest[i].DefineProgram("default_2d_program");
        m_aArmoryMedalBest[i].SetPosition  (i ? coreVector2(0.0f,0.0f) : (m_aArmoryMedal[0].GetPosition() + coreVector2(0.0f,-0.05f)));
        m_aArmoryMedalBest[i].SetSize      (coreVector2(1.0f,1.0f) * (i ? 0.0f : 0.06f));
        m_aArmoryMedalBest[i].SetTexSize   (coreVector2(0.25f,0.25f));
    }

    m_ArmoryMedalBack.DefineTexture(0u, "menu_detail_02.png");
    m_ArmoryMedalBack.DefineTexture(1u, "menu_background_black.png");
    m_ArmoryMedalBack.DefineProgram("menu_free_program");
    m_ArmoryMedalBack.SetPosition  (m_ArmoryArea2.GetPosition() + coreVector2(0.0f,-0.014f));
    m_ArmoryMedalBack.SetSize      (coreVector2(7.0f + 5.0f, 0.5f) * 0.07f);
    m_ArmoryMedalBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        m_aArmoryMedalBack2[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aArmoryMedalBack2[i].DefineProgram("menu_single_program");
        m_aArmoryMedalBack2[i].SetPosition  (m_aArmoryMedal[i].GetPosition());
        m_aArmoryMedalBack2[i].SetSize      (m_aArmoryMedal[i].GetSize() + coreVector2(0.05f,0.05f));
        m_aArmoryMedalBack2[i].SetColor3    (coreVector3(0.0f,0.0f,0.0f));
    }

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
        //m_aArmoryBadgeBack[i].SetPosition  (m_ArmoryArea2.GetPosition() + coreVector2(((i == 1u) ? 0.025f : 0.0f) - 0.39f, 0.0f + I_TO_F(i - 1u) * -0.045f));
        m_aArmoryBadgeBack[i].SetPosition  (m_ArmoryArea2.GetPosition() + coreVector2(0.01f - 0.39f, 0.0f + (I_TO_F(i) - 0.5f) * -0.06f));
        m_aArmoryBadgeBack[i].SetSize      (coreVector2(7.0f + 2.0f + 0.6f, 0.5f) * 0.07f);
        m_aArmoryBadgeBack[i].SetAlignment (coreVector2(1.0f,0.0f));
        m_aArmoryBadgeBack[i].SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);
        m_aArmoryBadgeBack[i].SetTexSize   (coreVector2(0.5f,1.0f));
        m_aArmoryBadgeBack[i].SetTexOffset (coreVector2(0.5f,0.0f));

        m_aArmoryBadgeDesc[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryBadgeDesc[i].SetPosition (m_aArmoryBadgeBack[i].GetPosition() + coreVector2(0.05f,0.0f));
        m_aArmoryBadgeDesc[i].SetAlignment(m_aArmoryBadgeBack[i].GetAlignment());
        m_aArmoryBadgeDesc[i].SetColor3   (COLOR_MENU_WHITE);

        for(coreUintW j = 0u; j < ARRAY_SIZE(m_aArmoryBadgeDesc2[0]); ++j)
        {
            m_aArmoryBadgeDesc2[i][j].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
            m_aArmoryBadgeDesc2[i][j].SetPosition (m_aArmoryBadgeBack[i].GetPosition() + coreVector2(0.05f, j ? -0.015f : 0.015f));
            m_aArmoryBadgeDesc2[i][j].SetAlignment(m_aArmoryBadgeBack[i].GetAlignment());
            m_aArmoryBadgeDesc2[i][j].SetColor3   (COLOR_MENU_WHITE);
        }
    }

    m_aArmoryHelperBack.DefineTexture(0u, "menu_detail_03.png");
    m_aArmoryHelperBack.DefineTexture(1u, "menu_background_black.png");
    m_aArmoryHelperBack.DefineProgram("menu_free_program");
    m_aArmoryHelperBack.SetPosition  (m_ArmoryArea2.GetPosition() + coreVector2(0.025f - 0.39f, 0.0f));
    m_aArmoryHelperBack.SetSize      (coreVector2(7.0f + 2.0f + 0.6f, 0.5f) * 0.07f);
    m_aArmoryHelperBack.SetAlignment (coreVector2(1.0f,0.0f));
    m_aArmoryHelperBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.5f);
    m_aArmoryHelperBack.SetTexSize   (coreVector2(0.5f,1.0f));
    m_aArmoryHelperBack.SetTexOffset (coreVector2(0.5f,0.0f));

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS; ++i)
    {
        m_aArmoryHelper[i].DefineTexture(0u, "menu_helper.png");
        m_aArmoryHelper[i].DefineProgram("menu_helper_program");
        m_aArmoryHelper[i].SetPosition  (m_aArmoryHelperBack.GetPosition() + coreVector2(0.095f + I_TO_F(i) * 0.06f, 0.0f));
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
        //m_aArmoryIcon[i].SetSize      (coreVector2(1.0f,1.0f) * 0.065f);
        //m_aArmoryIcon[i].SetDirection (coreVector2(1.0f,1.0f).Normalized());
        m_aArmoryIcon[i].SetColor3    (g_aMissionData[i].vColor * 1.0f);
        m_aArmoryIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aArmoryIcon[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aArmoryIconBack[i].DefineTexture(0u, "menu_helper.png");
        m_aArmoryIconBack[i].DefineProgram("menu_helper_program");
        //m_aArmoryIconBack[i].SetSize      (m_aArmoryIcon[i].GetSize     () * 1.2f);
        m_aArmoryIconBack[i].SetColor3    (m_aArmoryIcon[i].GetColor3   ());
        m_aArmoryIconBack[i].SetTexSize   (m_aArmoryIcon[i].GetTexSize  ());
        m_aArmoryIconBack[i].SetTexOffset (m_aArmoryIcon[i].GetTexOffset());
    }

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ARROWS; ++i)
    {
        m_aArmoryArrow[i].DefineTexture(0u, "menu_arrow.png");
        m_aArmoryArrow[i].DefineProgram("menu_single_program");
        m_aArmoryArrow[i].SetSize      (coreVector2(0.021f,0.021f));
        m_aArmoryArrow[i].SetColor3    (COLOR_MENU_WHITE);
    }

    m_aArmoryTitle[0].Construct   (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_aArmoryTitle[0].SetAlignment(coreVector2(0.0f,0.0f));

    m_aArmoryTitle[1].Construct   (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_aArmoryTitle[1].SetAlignment(m_aArmoryTitle[0].GetAlignment());
    m_aArmoryTitle[1].SetColor3   (COLOR_MENU_WHITE);

    m_aArmoryScore[0].Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_aArmoryScore[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(-0.235f,0.018f));
    m_aArmoryScore[0].SetColor3      (COLOR_MENU_WHITE);
    m_aArmoryScore[0].SetTextLanguage("GAME_SCORE");

    m_aArmoryScore[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryScore[1].SetPosition(m_aArmoryScore[0].GetPosition() + coreVector2(0.0f,-0.036f));
    m_aArmoryScore[1].SetColor3  (COLOR_MENU_WHITE);

    m_aArmoryTime[0].Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_aArmoryTime[0].SetPosition    (m_ArmoryArea.GetPosition() + coreVector2(0.235f,0.018f));
    m_aArmoryTime[0].SetColor3      (COLOR_MENU_WHITE);
    m_aArmoryTime[0].SetTextLanguage("GAME_TIME");

    m_aArmoryTime[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArmoryTime[1].SetPosition(m_aArmoryTime[0].GetPosition() + coreVector2(0.0f,-0.036f));
    m_aArmoryTime[1].SetColor3  (COLOR_MENU_WHITE);

    m_ArmoryIconBig.DefineTexture(0u, "menu_helper.png");
    m_ArmoryIconBig.DefineProgram("menu_helper_program");
    //m_ArmoryIconBig.SetPosition  (m_aArmoryTitle[0].GetPosition() + coreVector2(0.0f,0.018f));
    m_ArmoryIconBig.SetSize      (coreVector2(0.2f,0.2f));
    m_ArmoryIconBig.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_ArmoryIconBig.SetTexSize   (coreVector2(0.25f,0.25f));

    m_ArmoryTrophy.Construct(MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_ArmoryTrophy.SetColor3(COLOR_MENU_WHITE);
    m_ArmoryTrophy.SetText  (ICON_TROPHY);

    m_ArmoryTile.DefineProgram("menu_segment_program");

    m_ArmoryBack.DefineProgram("menu_color_program");
    m_ArmoryBack.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 0.1f);

    m_ArmorySelection.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_2, MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_ArmorySelection.SetPosition(m_BackgroundArmory.GetPosition() + m_BackgroundArmory.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.08f));
    m_ArmorySelection.SetSize    (coreVector2(0.73f,0.065f));
    m_ArmorySelection.SetEndless (true);
    m_ArmorySelection.GetArrow(0u)->DefineProgram("menu_border_program");
    m_ArmorySelection.GetArrow(1u)->DefineProgram("menu_border_program");
    m_ArmorySelection.AddEntry("", 0u);
    m_ArmorySelection.AddEntry("", 1u);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryStartArrow); ++i)
    {
        m_aArmoryStartArrow[i].DefineTexture(0u, "menu_arrow.png");
        m_aArmoryStartArrow[i].DefineProgram("menu_single_program");
        m_aArmoryStartArrow[i].SetSize      (coreVector2( 1.0f,1.0f) * (i ? 0.021f : 0.034f));
        m_aArmoryStartArrow[i].SetDirection (coreVector2(-1.0f,0.0f));
        m_aArmoryStartArrow[i].SetColor3    (i ? COLOR_MENU_WHITE : coreVector3( 0.1f,0.1f,0.1f));
    }

    m_ArmoryStartLabel.Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryStartLabel.SetTextLanguage("BEGINNING");
    m_ArmoryStartLabel.SetRectifyY    (false);

    m_ArmoryStartBack.DefineTexture(0u, "effect_headlight_point.png");
    m_ArmoryStartBack.DefineProgram("menu_single_program");
    m_ArmoryStartBack.SetSize      (coreVector2(0.1f,0.05f));
    m_ArmoryStartBack.SetColor3    (coreVector3(0.0f,0.0f,0.0f));

    m_ArmoryType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_ArmoryType.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[0].GetPosition());
    m_ArmoryType.SetSize     (coreVector2( 0.47f,0.03f));
    m_ArmoryType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_ArmoryType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryMode.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_ArmoryMode.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[1].GetPosition());
    m_ArmoryMode.SetSize     (m_ArmoryType.GetSize());
    m_ArmoryMode.SetAlignment(m_ArmoryType.GetAlignment());
    m_ArmoryMode.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmoryDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_ArmoryDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[3].GetPosition());
    m_ArmoryDifficulty.SetSize     (m_ArmoryType.GetSize());
    m_ArmoryDifficulty.SetAlignment(m_ArmoryType.GetAlignment());
    m_ArmoryDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_ArmorySpeed.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_ArmorySpeed.SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[2].GetPosition());
    m_ArmorySpeed.SetSize     (m_ArmoryType.GetSize());
    m_ArmorySpeed.SetAlignment(m_ArmoryType.GetAlignment());
    m_ArmorySpeed.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aArmoryShield[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aArmoryShield[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[4].GetPosition() - vOffset);
        m_aArmoryShield[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aArmoryShield[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aArmoryShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aArmoryWeapon[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aArmoryWeapon[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aArmoryName[5].GetPosition() - vOffset);
        m_aArmoryWeapon[i].SetSize     (m_aArmoryShield[i].GetSize());
        m_aArmoryWeapon[i].SetAlignment(m_aArmoryShield[i].GetAlignment());
        m_aArmoryWeapon[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aArmoryPlayer[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aArmoryPlayer[i].SetPosition(m_aArmoryWeapon[i].GetPosition() + coreVector2(m_aArmoryWeapon[i].GetSize().x * -0.5f, -0.05f));
        m_aArmoryPlayer[i].SetColor3  (COLOR_MENU_WHITE);

        m_aArmoryWeaponIcon[i].DefineTexture(0u, "menu_weapon.png");
        m_aArmoryWeaponIcon[i].DefineProgram("default_2d_program");
        m_aArmoryWeaponIcon[i].SetPosition  (m_aArmoryWeapon[i].GetPosition() + coreVector2(m_aArmoryWeapon[i].GetSize().x * -0.5f, 0.0f));
        m_aArmoryWeaponIcon[i].SetSize      (coreVector2(1.0f, 1.0f) * 0.085f);
        m_aArmoryWeaponIcon[i].SetTexSize   (coreVector2(0.25f,0.5f));
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryRaise); ++i)
    {
        m_aArmoryRaise[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aArmoryRaise[i].SetAlignment(coreVector2((i == 2u) ? 1.0f : -1.0f, 0.0f));
        m_aArmoryRaise[i].SetColor3   (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    }
    m_aArmoryRaise[0].SetPosition(m_ArmorySpeed      .GetPosition() + coreVector2(-0.48f, 0.0f));
    m_aArmoryRaise[1].SetPosition(m_aArmoryShield[1] .GetPosition() + coreVector2(-0.48f, 0.0f));
    m_aArmoryRaise[2].SetPosition(m_StartButtonArmory.GetPosition() + m_StartButtonArmory.GetSize()*coreVector2(1.0f,-0.5f) + coreVector2(0.025f,-0.016f));

    m_ArmoryRaiseText.Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_ArmoryRaiseText.SetPosition    (m_StartButtonArmory.GetPosition() + m_StartButtonArmory.GetSize()*coreVector2(1.0f,-0.5f) + coreVector2(0.025f,0.016f));
    m_ArmoryRaiseText.SetAlignment   (coreVector2(1.0f,0.0f));
    m_ArmoryRaiseText.SetColor3      (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    m_ArmoryRaiseText.SetTextLanguage("SCORE_RAISE");

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryCueLock); ++i)
    {
        m_aArmoryCueLock[i].Construct(MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aArmoryCueLock[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aArmoryCueLock[i].SetText  (ICON_LOCK);
    }
    m_aArmoryCueLock[0].SetPosition(m_aArmoryName[2].GetPosition() + coreVector2(0.312f,0.0f));
    m_aArmoryCueLock[1].SetPosition(m_aArmoryName[4].GetPosition() + coreVector2(0.312f,0.0f));

    m_FirstHeader.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_FirstHeader.SetPosition    (m_BackgroundFirst.GetPosition() + m_BackgroundFirst.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.06f));
    m_FirstHeader.SetColor3      (COLOR_MENU_WHITE);
    m_FirstHeader.SetTextLanguage("GAME_HEADER_DEMO");

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

        iOffset += (i == 2u) ? 3u : 2u;
    }
    m_aFirstName[0].SetTextLanguage("GAME_TYPE");
    m_aFirstName[1].SetTextLanguage("GAME_GAMESPEED");
    m_aFirstName[2].SetTextLanguage("GAME_DIFFICULTY");
    m_aFirstName[3].SetTextLanguage("GAME_SHIELD");

    m_FirstType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FirstType.SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[0].GetPosition());
    m_FirstType.SetSize     (coreVector2( 0.47f,0.03f));
    m_FirstType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_FirstType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FirstDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FirstDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[2].GetPosition());
    m_FirstDifficulty.SetSize     (m_FirstType.GetSize());
    m_FirstDifficulty.SetAlignment(m_FirstType.GetAlignment());
    m_FirstDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_FirstSpeed.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_FirstSpeed.SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[1].GetPosition());
    m_FirstSpeed.SetSize     (m_FirstType.GetSize());
    m_FirstSpeed.SetAlignment(m_FirstType.GetAlignment());
    m_FirstSpeed.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_GAME_PLAYERS - i - 1u), 0.0f);

        m_aFirstShield[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aFirstShield[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aFirstName[3].GetPosition() - vOffset);
        m_aFirstShield[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aFirstShield[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aFirstShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aFirstPlayer[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aFirstPlayer[i].SetPosition(m_aFirstShield[i].GetPosition() - coreVector2(m_aFirstShield[i].GetSize().x * 0.5f, 0.05f));
        m_aFirstPlayer[i].SetColor3  (COLOR_MENU_WHITE);
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFirstRaise); ++i)
    {
        m_aFirstRaise[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aFirstRaise[i].SetAlignment(coreVector2((i == 2u) ? 1.0f : -1.0f, 0.0f));
        m_aFirstRaise[i].SetColor3   (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    }
    m_aFirstRaise[0].SetPosition(m_FirstSpeed      .GetPosition() + coreVector2(-0.48f, 0.0f));
    m_aFirstRaise[1].SetPosition(m_aFirstShield[1] .GetPosition() + coreVector2(-0.48f, 0.0f));
    m_aFirstRaise[2].SetPosition(m_StartButtonFirst.GetPosition() + m_StartButtonFirst.GetSize()*coreVector2(1.0f,-0.5f) + coreVector2(0.025f,-0.016f));

    m_FirstRaiseText.Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_FirstRaiseText.SetPosition    (m_StartButtonFirst.GetPosition() + m_StartButtonFirst.GetSize()*coreVector2(1.0f,-0.5f) + coreVector2(0.025f,0.016f));
    m_FirstRaiseText.SetAlignment   (coreVector2(1.0f,0.0f));
    m_FirstRaiseText.SetColor3      (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    m_FirstRaiseText.SetTextLanguage("SCORE_RAISE");

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFirstCueLock); ++i)
    {
        m_aFirstCueLock[i].Construct(MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aFirstCueLock[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aFirstCueLock[i].SetText  (ICON_LOCK);
    }
    m_aFirstCueLock[0].SetPosition(m_aFirstName[1].GetPosition() + coreVector2(0.312f,0.0f));
    m_aFirstCueLock[1].SetPosition(m_aFirstName[3].GetPosition() + coreVector2(0.312f,0.0f));

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

        iOffset += ((i == 2u) || (i == 3u)) ? 3u : 2u;
    }
    m_aDemoName[0].SetTextLanguage("GAME_TYPE");
    m_aDemoName[1].SetTextLanguage("GAME_GAMESPEED");
    m_aDemoName[2].SetTextLanguage("GAME_DIFFICULTY");
    m_aDemoName[3].SetTextLanguage("GAME_STAGE");
    m_aDemoName[4].SetTextLanguage("GAME_SHIELD");

    m_DemoType.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_DemoType.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[0].GetPosition());
    m_DemoType.SetSize     (coreVector2( 0.47f,0.03f));
    m_DemoType.SetAlignment(coreVector2(-1.00f,0.00f));
    m_DemoType.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoDifficulty.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_DemoDifficulty.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[2].GetPosition());
    m_DemoDifficulty.SetSize     (m_DemoType.GetSize());
    m_DemoDifficulty.SetAlignment(m_DemoType.GetAlignment());
    m_DemoDifficulty.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoSpeed.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_DemoSpeed.SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[1].GetPosition());
    m_DemoSpeed.SetSize     (m_DemoType.GetSize());
    m_DemoSpeed.SetAlignment(m_DemoType.GetAlignment());
    m_DemoSpeed.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_DemoStage.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
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

        m_aDemoShield[i].Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aDemoShield[i].SetPosition (coreVector2(-1.00f,1.00f) * m_aDemoName[4].GetPosition() - vOffset);
        m_aDemoShield[i].SetSize     (coreVector2( 0.22f,0.03f));
        m_aDemoShield[i].SetAlignment(coreVector2(-1.00f,0.00f));
        m_aDemoShield[i].GetCaption()->SetColor3(COLOR_MENU_WHITE);

        m_aDemoPlayer[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aDemoPlayer[i].SetPosition(m_aDemoShield[i].GetPosition() - coreVector2(m_aDemoShield[i].GetSize().x * 0.5f, 0.05f));
        m_aDemoPlayer[i].SetColor3  (COLOR_MENU_WHITE);
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRaise); ++i)
    {
        m_aDemoRaise[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aDemoRaise[i].SetAlignment(coreVector2((i == 2u) ? 1.0f : -1.0f, 0.0f));
        m_aDemoRaise[i].SetColor3   (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    }
    m_aDemoRaise[0].SetPosition(m_DemoSpeed      .GetPosition() + coreVector2(-0.48f, 0.0f));
    m_aDemoRaise[1].SetPosition(m_aDemoShield[1] .GetPosition() + coreVector2(-0.48f, 0.0f));
    m_aDemoRaise[2].SetPosition(m_StartButtonDemo.GetPosition() + m_StartButtonDemo.GetSize()*coreVector2(1.0f,-0.5f) + coreVector2(0.025f,-0.016f));

    m_DemoRaiseText.Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_DemoRaiseText.SetPosition    (m_StartButtonDemo.GetPosition() + m_StartButtonDemo.GetSize()*coreVector2(1.0f,-0.5f) + coreVector2(0.025f,0.016f));
    m_DemoRaiseText.SetAlignment   (coreVector2(1.0f,0.0f));
    m_DemoRaiseText.SetColor3      (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    m_DemoRaiseText.SetTextLanguage("SCORE_RAISE");

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoCueLock); ++i)
    {
        m_aDemoCueLock[i].Construct(MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aDemoCueLock[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aDemoCueLock[i].SetText  (ICON_LOCK);
    }
    m_aDemoCueLock[0].SetPosition(m_aDemoName[1].GetPosition() + coreVector2(0.312f,0.0f));
    m_aDemoCueLock[1].SetPosition(m_aDemoName[4].GetPosition() + coreVector2(0.312f,0.0f));

    m_SpeedNew.SetPosition(m_ArmorySpeed.GetPosition() + coreVector2(0.045f,0.0f));
    m_SpeedNew.SetIndex   (NEW_ARMORY_GAMESPEED);

    m_ShieldNew.SetPosition(m_aArmoryShield[1].GetPosition() + coreVector2(0.045f,0.0f));
    m_ShieldNew.SetIndex   (NEW_ARMORY_SHIELD);

    m_WeaponNew.SetPosition(m_aArmoryWeapon[1].GetPosition() + coreVector2(0.045f,0.0f));
    m_WeaponNew.SetIndex   (NEW_ARMORY_WEAPON);

    // fill option entries       
    m_ArmoryMode     .AddEntryLanguage("GAME_MODE_STANDARD",     GAME_MODE_STANDARD);
    m_ArmoryMode     .AddEntryLanguage("GAME_MODE_PACIFIST",     GAME_MODE_PACIFIST);
    m_ArmoryMode     .AddEntryLanguage("GAME_MODE_MASOCHIST",    GAME_MODE_MASOCHIST);
    m_FirstType      .AddEntryLanguage("GAME_TYPE_SOLO",         GAME_TYPE_SOLO);
    m_FirstType      .AddEntryLanguage("GAME_TYPE_COOP",         GAME_TYPE_COOP);
    m_FirstDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",   GAME_DIFFICULTY_EASY);
    m_FirstDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL", GAME_DIFFICULTY_NORMAL);
    for(coreUintW i = 50u; i <= 100u; i += 5u) m_FirstSpeed.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aFirstShield[i].AddEntryLanguage("VALUE_NO", 0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) for(coreUintW j = 5u; j <= SHIELD_DEFAULT; j += 5u) m_aFirstShield[i].AddEntry(PRINT("%zu", j), j);
    m_DemoType      .AddEntryLanguage("GAME_TYPE_SOLO",          GAME_TYPE_SOLO);
    m_DemoType      .AddEntryLanguage("GAME_TYPE_COOP",          GAME_TYPE_COOP);
    m_DemoDifficulty.AddEntryLanguage("GAME_DIFFICULTY_EASY",    GAME_DIFFICULTY_EASY);
    m_DemoDifficulty.AddEntryLanguage("GAME_DIFFICULTY_NORMAL",  GAME_DIFFICULTY_NORMAL);
    for(coreUintW i = 50u; i <= 100u; i += 5u) m_DemoSpeed.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) m_aDemoShield[i].AddEntryLanguage("VALUE_NO", 0u);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) for(coreUintW j = 5u; j <= SHIELD_DEFAULT; j += 5u) m_aDemoShield[i].AddEntry(PRINT("%zu", j), j);

    // 
    for(coreUintW i = 0u; i < FRAGMENTS; ++i)
    {
        m_apFragment[i] = Core::Manager::Resource->Get<coreTexture>(PRINT("menu_fragment_%02zu.png", i + 1u));
    }
    m_apSegment[0] = Core::Manager::Resource->Get<coreTexture>("menu_segment_boss.png");
    m_apSegment[1] = Core::Manager::Resource->Get<coreTexture>("menu_segment_wave.png");

    // 
    m_NavigatorMain.BindObject(&m_BackButtonMain, &m_aaSegmentTile[MENU_GAME_MISSIONS - 1u][MENU_GAME_SEGMENTS - 1u], NULL, &m_ArcadeComplete, NULL, MENU_TYPE_DEFAULT);

    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
#if defined(_CORE_DEBUG_)
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            m_NavigatorMain.BindObject(&m_aaSegmentTile[i][j], (i == 0u)                      ? s_cast<coreObject2D*>(&m_ArcadeComplete) :                                                                  &m_aaSegmentTile[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS][j], (j == 0u)                      ? &m_aSegmentIconBack[i] : &m_aaSegmentTile[i][(j + MENU_GAME_SEGMENTS - 1u) % MENU_GAME_SEGMENTS],
                                                               (i == MENU_GAME_MISSIONS - 1u) ? s_cast<coreObject2D*>(&m_BackButtonMain) : (i == 7u) ? &m_aaSegmentTile[(i + 1u) % MENU_GAME_MISSIONS][5] : &m_aaSegmentTile[(i + 1u)                      % MENU_GAME_MISSIONS][j], (j == MENU_GAME_SEGMENTS - 1u) ? &m_aSegmentIconBack[i] : &m_aaSegmentTile[i][(j + 1u)                      % MENU_GAME_SEGMENTS], j ? &m_aaSegmentTile[i][(j + MENU_GAME_SEGMENTS - 1u) % MENU_GAME_SEGMENTS] : NULL, MENU_TYPE_BIG);
        }
        m_NavigatorMain.BindObject(&m_aSegmentIconBack[i], (i == 0u)                      ? s_cast<coreObject2D*>(&m_ArcadeComplete) : &m_aSegmentIconBack[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS], &m_aaSegmentTile[i][MENU_GAME_SEGMENTS - 1u],
                                                           (i == MENU_GAME_MISSIONS - 1u) ? s_cast<coreObject2D*>(&m_BackButtonMain) : &m_aSegmentIconBack[(i + 1u)                      % MENU_GAME_MISSIONS], &m_aaSegmentTile[i][0], MENU_TYPE_DEFAULT);
#else
        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            m_NavigatorMain.BindObject(&m_aaSegmentTile[i][j], (i == 0u)                      ? s_cast<coreObject2D*>(&m_ArcadeComplete) :                                                                  &m_aaSegmentTile[(i + MENU_GAME_MISSIONS - 1u) % MENU_GAME_MISSIONS][j], (j == 0u)                      ? &m_aaSegmentTile[i][MENU_GAME_SEGMENTS - 1u] : &m_aaSegmentTile[i][(j + MENU_GAME_SEGMENTS - 1u) % MENU_GAME_SEGMENTS],
                                                               (i == MENU_GAME_MISSIONS - 1u) ? s_cast<coreObject2D*>(&m_BackButtonMain) : (i == 7u) ? &m_aaSegmentTile[(i + 1u) % MENU_GAME_MISSIONS][5] : &m_aaSegmentTile[(i + 1u)                      % MENU_GAME_MISSIONS][j], (j == MENU_GAME_SEGMENTS - 1u) ? &m_aaSegmentTile[i][0]                       : &m_aaSegmentTile[i][(j + 1u)                      % MENU_GAME_SEGMENTS], j ? &m_aaSegmentTile[i][(j + MENU_GAME_SEGMENTS - 1u) % MENU_GAME_SEGMENTS] : NULL, MENU_TYPE_BIG);
        }
#endif
    }
    m_NavigatorMain.BindObject(&m_ArcadeComplete, &m_BackButtonMain, NULL, NULL, NULL, MENU_TYPE_DEFAULT);

    m_NavigatorMain.BindScroll(&m_SegmentBox);

    m_NavigatorMain.AssignFirst(&m_ArcadeComplete);
    m_NavigatorMain.AssignBack (&m_BackButtonMain);
    m_NavigatorMain.AssignMenu (this);

    m_NavigatorMain.BindDynamic(&m_ArcadeComplete, [this](coreObject2D* pObject, const coreUint8 iPack) {if(iPack == 4u) m_NavigatorMain.RebindDown(pObject, &m_aaSegmentTile[0][m_iSegmentLast]);});

    m_NavigatorArmory.BindObject(&m_ArmorySelection,   &m_StartButtonArmory, NULL,                 NULL,                 NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmoryType,        NULL,                 NULL,                 &m_ArmoryMode,        NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmoryMode,        &m_ArmoryType,        NULL,                 &m_ArmorySpeed,       NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmorySpeed,       &m_ArmoryMode,        NULL,                 &m_ArmoryDifficulty,  NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_ArmoryDifficulty,  &m_ArmorySpeed,       NULL,                 &m_aArmoryShield[0],  NULL,                 MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorArmory.BindObject(&m_aArmoryShield[0],  &m_ArmoryDifficulty,  &m_aArmoryShield[1],  &m_aArmoryWeapon[0],  &m_aArmoryShield[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_aArmoryShield[1],  &m_ArmoryDifficulty,  &m_aArmoryShield[0],  &m_aArmoryWeapon[1],  &m_aArmoryShield[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_aArmoryWeapon[0],  &m_aArmoryShield[0],  &m_aArmoryWeapon[1],  &m_StartButtonArmory, &m_aArmoryWeapon[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_aArmoryWeapon[1],  &m_aArmoryShield[1],  &m_aArmoryWeapon[0],  &m_StartButtonArmory, &m_aArmoryWeapon[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorArmory.BindObject(&m_StartButtonArmory, &m_aArmoryWeapon[0],  &m_BackButtonArmory,  &m_ArmorySelection,   &m_InputButtonArmory, MENU_TYPE_DEFAULT);
    m_NavigatorArmory.BindObject(&m_InputButtonArmory, &m_aArmoryWeapon[0],  &m_StartButtonArmory, &m_ArmorySelection,   &m_BackButtonArmory,  MENU_TYPE_DEFAULT);
    m_NavigatorArmory.BindObject(&m_BackButtonArmory,  &m_aArmoryWeapon[0],  &m_InputButtonArmory, &m_ArmorySelection,   &m_StartButtonArmory, MENU_TYPE_DEFAULT);

    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        m_NavigatorArmory.BindObject(&m_aArmoryMedal[i], &m_ArmorySelection, (i == 1u)                           ? &m_aArmoryMedal[MENU_GAME_ARMORY_MEDALS - 1u] : &m_aArmoryMedal[i - 1u],
                                                         &m_ArmoryType,      (i == MENU_GAME_ARMORY_MEDALS - 1u) ? &m_aArmoryMedal[1u]                           : &m_aArmoryMedal[i + 1u], MENU_TYPE_DEFAULT);
    }

    m_NavigatorArmory.AssignFirst(&m_StartButtonArmory);
    m_NavigatorArmory.AssignBack (&m_BackButtonArmory);
    m_NavigatorArmory.AssignMenu (this);

    m_NavigatorArmory.UseShoulderLeft ([this]() {this->SelectPrevious(); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});
    m_NavigatorArmory.UseShoulderRight([this]() {this->SelectNext    (); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);});

    m_NavigatorArmory.BindDynamic(&m_ArmoryType,      [this](coreObject2D* pObject, const coreUint8 iPack) {if(iPack == 0u) m_NavigatorArmory.RebindUp  (pObject, &m_aArmoryMedal[m_iArmoryStartIndex + 1u]);});
    m_NavigatorArmory.BindDynamic(&m_ArmorySelection, [this](coreObject2D* pObject, const coreUint8 iPack) {if(iPack == 4u) m_NavigatorArmory.RebindDown(pObject, &m_aArmoryMedal[m_iArmoryStartIndex + 1u]);});

    m_NavigatorFirst.BindObject(&m_FirstType,        &m_StartButtonFirst, NULL,                &m_FirstSpeed,       NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorFirst.BindObject(&m_FirstSpeed,       &m_FirstType,        NULL,                &m_FirstDifficulty,  NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorFirst.BindObject(&m_FirstDifficulty,  &m_FirstSpeed,       NULL,                &m_aFirstShield[0],  NULL,                MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorFirst.BindObject(&m_aFirstShield[0],  &m_FirstDifficulty,  &m_aFirstShield[1],  &m_StartButtonFirst, &m_aFirstShield[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorFirst.BindObject(&m_aFirstShield[1],  &m_FirstDifficulty,  &m_aFirstShield[0],  &m_StartButtonFirst, &m_aFirstShield[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorFirst.BindObject(&m_StartButtonFirst, &m_aFirstShield[0],  &m_BackButtonFirst,  &m_FirstType,        &m_InputButtonFirst, MENU_TYPE_DEFAULT);
    m_NavigatorFirst.BindObject(&m_InputButtonFirst, &m_aFirstShield[0],  &m_StartButtonFirst, &m_FirstType,        &m_BackButtonFirst,  MENU_TYPE_DEFAULT);
    m_NavigatorFirst.BindObject(&m_BackButtonFirst,  &m_aFirstShield[0],  &m_InputButtonFirst, &m_FirstType,        &m_StartButtonFirst, MENU_TYPE_DEFAULT);

    m_NavigatorFirst.AssignFirst(&m_FirstType);
    m_NavigatorFirst.AssignBack (&m_BackButtonFirst);
    m_NavigatorFirst.AssignMenu (this);

    m_NavigatorDemo.BindObject(&m_DemoType,        &m_StartButtonDemo, NULL,               &m_DemoSpeed,       NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_DemoSpeed,       &m_DemoType,        NULL,               &m_DemoDifficulty,  NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_DemoDifficulty,  &m_DemoSpeed,       NULL,               &m_DemoStage,       NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_DemoStage,       &m_DemoDifficulty,  NULL,               &m_aDemoShield[0],  NULL,               MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    m_NavigatorDemo.BindObject(&m_aDemoShield[0],  &m_DemoStage,       &m_aDemoShield[1],  &m_StartButtonDemo, &m_aDemoShield[1],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorDemo.BindObject(&m_aDemoShield[1],  &m_DemoStage,       &m_aDemoShield[0],  &m_StartButtonDemo, &m_aDemoShield[0],  MENU_TYPE_SWITCH_PRESS);
    m_NavigatorDemo.BindObject(&m_StartButtonDemo, &m_aDemoShield[0],  &m_BackButtonDemo,  &m_DemoType,        &m_InputButtonDemo, MENU_TYPE_DEFAULT);
    m_NavigatorDemo.BindObject(&m_InputButtonDemo, &m_aDemoShield[0],  &m_StartButtonDemo, &m_DemoType,        &m_BackButtonDemo,  MENU_TYPE_DEFAULT);
    m_NavigatorDemo.BindObject(&m_BackButtonDemo,  &m_aDemoShield[0],  &m_InputButtonDemo, &m_DemoType,        &m_StartButtonDemo, MENU_TYPE_DEFAULT);

    m_NavigatorDemo.AssignFirst(&m_DemoType);
    m_NavigatorDemo.AssignBack (&m_BackButtonDemo);
    m_NavigatorDemo.AssignMenu (this);

    // bind menu objects
    this->BindObject(SURFACE_GAME_MAIN, &m_BackgroundMain);
    this->BindObject(SURFACE_GAME_MAIN, &m_BackButtonMain);
    this->BindObject(SURFACE_GAME_MAIN, &m_SegmentBox);
    this->BindObject(SURFACE_GAME_MAIN, &m_NavigatorMain);

    this->BindObject(SURFACE_GAME_ARMORY, &m_BackgroundArmory);
    this->BindObject(SURFACE_GAME_ARMORY, &m_StartButtonArmory);
#if defined(_CORE_SWITCH_) || defined(_CORE_DEBUG_)
    this->BindObject(SURFACE_GAME_ARMORY, &m_InputButtonArmory);
#endif
    this->BindObject(SURFACE_GAME_ARMORY, &m_BackButtonArmory);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryArea);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryArea2);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMedalBack2[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryIconBig);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES;           ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryLine     [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryCueLock); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryCueLock  [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeBack[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryHelperBack);
    //this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryMedalBack);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryFragmentBack);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryFragment);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryBack);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryTile);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryHelperWave[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryHelper    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryIconBack  [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS;      ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryIcon      [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeWave [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadge     [i]);
#if defined(_CORE_DEBUG_)
    //for(coreUintW i = 0u; i < MENU_GAME_PLAYERS;           ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryWeaponIcon[i]);
#endif
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ARROWS;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryArrow    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMedal    [i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_MEDALS;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryMedalBest[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryStartBack);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryStartArrow); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryStartArrow[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;    ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeDesc[i]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;    ++i) for(coreUintW j = 0u; j < ARRAY_SIZE(m_aArmoryBadgeDesc2[0]); ++j) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryBadgeDesc2[i][j]);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORIES;         ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryName     [i]);
#if 0
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryRaise); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryRaise    [i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryRaiseText);
#endif
    //this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryHeader);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryStartLabel);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryTrophy);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTitle); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryTitle [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryScore); ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryScore [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTime);  ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryTime  [i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS;          ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryPlayer[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmorySelection);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryType);
    //this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryMode);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmoryDifficulty);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ArmorySpeed);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryShield[i]);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_ARMORY, &m_aArmoryWeapon[i]);
    this->BindObject(SURFACE_GAME_ARMORY, &m_SpeedNew);
    this->BindObject(SURFACE_GAME_ARMORY, &m_ShieldNew);
    this->BindObject(SURFACE_GAME_ARMORY, &m_WeaponNew);
    this->BindObject(SURFACE_GAME_ARMORY, &m_NavigatorArmory);

    this->BindObject(SURFACE_GAME_FIRST, &m_BackgroundFirst);
    this->BindObject(SURFACE_GAME_FIRST, &m_StartButtonFirst);
#if defined(_CORE_SWITCH_) || defined(_CORE_DEBUG_)
    this->BindObject(SURFACE_GAME_FIRST, &m_InputButtonFirst);
#endif
    this->BindObject(SURFACE_GAME_FIRST, &m_BackButtonFirst);
    for(coreUintW i = 0u; i < MENU_GAME_FIRSTS;            ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstLine   [i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFirstCueLock); ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstCueLock[i]);
    for(coreUintW i = 0u; i < MENU_GAME_FIRSTS;            ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstName   [i]);
#if 0
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aFirstRaise); ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstRaise[i]);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstRaiseText);
#endif
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstHeader);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstPlayer[i]);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstType);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstDifficulty);
    this->BindObject(SURFACE_GAME_FIRST, &m_FirstSpeed);
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) this->BindObject(SURFACE_GAME_FIRST, &m_aFirstShield[i]);
    this->BindObject(SURFACE_GAME_FIRST, &m_NavigatorFirst);

    this->BindObject(SURFACE_GAME_DEMO, &m_BackgroundDemo);
    this->BindObject(SURFACE_GAME_DEMO, &m_StartButtonDemo);
#if defined(_CORE_SWITCH_) || defined(_CORE_DEBUG_)
    this->BindObject(SURFACE_GAME_DEMO, &m_InputButtonDemo);
#endif
    this->BindObject(SURFACE_GAME_DEMO, &m_BackButtonDemo);
    for(coreUintW i = 0u; i < MENU_GAME_DEMOS; ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoLine[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoCueLock); ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoCueLock[i]);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoStageIcon);
    for(coreUintW i = 0u; i < MENU_GAME_DEMOS;          ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoName [i]);
#if 0
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDemoRaise); ++i) this->BindObject(SURFACE_GAME_DEMO, &m_aDemoRaise[i]);
    this->BindObject(SURFACE_GAME_DEMO, &m_DemoRaiseText);
#endif
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
    default: UNREACHABLE
    case SURFACE_GAME_MAIN:   m_NavigatorMain  .Update(); break;
    case SURFACE_GAME_ARMORY: m_NavigatorArmory.Update(); break;
    case SURFACE_GAME_FIRST:  m_NavigatorFirst .Update(); break;
    case SURFACE_GAME_DEMO:   m_NavigatorDemo  .Update(); break;
    }

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_GAME_MAIN:
        {
            if(m_ArcadeComplete.IsClicked())
            {
                // 
                m_iCurPage      = 2u;
                m_aiCurIndex[0] = m_iArmoryStartIndex;

                // 
                this->__PrepareArcade();

                // 
                g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u);

                // 
                m_NavigatorArmory.ResetFirst();
            }
            else if(m_BackButtonMain.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                //g_pMenu->ShiftSurface(this, SURFACE_GAME_OPTION, 3.0f, 2u);

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
                m_aSegmentIcon[i].SetSize     (coreVector2(0.1f,0.1f) * 1.1f);
                m_aSegmentIcon[i].SetDirection(coreVector2(1.0f,1.0f).Normalized());
                //m_aSegmentIcon[i].Move();

                m_aSegmentIconBack[i].SetSize     (m_aSegmentIcon[i].GetSize() * 1.2f);
                m_aSegmentIconBack[i].SetDirection(coreVector2(0.0f,1.0f));
                //m_aSegmentIconBack[i].Move();
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
                        this->__SelectSegment(i, j);
                        this->__PrepareSegment(i, j);

                        // 
                        g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u);

                        // 
                        m_NavigatorArmory.ResetFirst();

                        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                    }

                    if(TIME && m_aaSegmentTile[i][j].IsFocused())
                    {
                        if((m_aiSegmentSelection[0] != i) || (m_aiSegmentSelection[1] != j)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);
                        
                        if(m_aiSegmentSelection[0] != i) m_fSegmentTime = 0.0f;

                        m_aiSegmentSelection[0] = i;
                        m_aiSegmentSelection[1] = j;
                        m_iSegmentLast = j;

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
                    g_pMenu->ShiftSurface(this, SURFACE_GAME_ARMORY, 3.0f, 1u);

                    // 
                    m_NavigatorArmory.ResetFirst();

                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                }

                if(TIME && m_aSegmentIconBack[i].IsFocused())
                {
                    if((m_aiSegmentSelection[0] != i) || (m_aiSegmentSelection[1] != MENU_GAME_SEGMENTS))
                    {
                        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);

                        if(m_aiSegmentSelection[0] != i) m_fSegmentTime = 0.0f;
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

            if((m_aiSegmentSelection[0] < MENU_GAME_MISSIONS) && (m_aiSegmentSelection[1] < MENU_GAME_SEGMENTS))
            {
                this->__RefreshCursor(m_aiSegmentSelection[0], m_aiSegmentSelection[1]);
                
                m_fSegmentTime.Update(1.0f);

                m_aSegmentIcon    [m_aiSegmentSelection[0]].SetDirection(coreVector2::Direction(m_fSegmentTime *  (0.2f*PI) - (0.25f*PI)));
                m_aSegmentIconBack[m_aiSegmentSelection[0]].SetDirection(coreVector2::Direction(m_fSegmentTime * (-0.1f*PI)));
            }
            else
            {
                for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSegmentCursor); ++i)
                {
                    m_aSegmentCursor[i].SetPosition(HIDDEN_POS);
                    m_aSegmentCursor[i].Move();
                }

                m_SegmentMedal.SetPosition(HIDDEN_POS);
                m_SegmentMedal.Move();

                for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
                {
                    m_aSegmentBadge[i].SetPosition(HIDDEN_POS);
                    m_aSegmentBadge[i].Move();

                    m_aSegmentBadgeWave[i].SetPosition(HIDDEN_POS);
                    m_aSegmentBadgeWave[i].Move();
                }

                if(m_aiSegmentSelection[0] < MENU_GAME_MISSIONS)
                {
                    m_fSegmentTime.Update(1.0f);

                    m_aSegmentIcon    [m_aiSegmentSelection[0]].SetDirection(coreVector2::Direction(m_fSegmentTime *  (0.2f*PI) - (0.25f*PI)));
                    m_aSegmentIconBack[m_aiSegmentSelection[0]].SetDirection(coreVector2::Direction(m_fSegmentTime * (-0.1f*PI)));

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
            


            const coreFloat fRotation = Core::System->GetTotalTimeFloat(2.0f*PI);

            for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
            {
                if(m_aSegmentBadgeWave[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    m_aSegmentBadge[i].SetDirection(coreVector2::Direction(fRotation + (0.8f*PI) * (I_TO_F(i) / I_TO_F(3u))));
                }
                else
                {
                    m_aSegmentBadge[i].SetDirection(coreVector2(0.0f,1.0f));
                }
                m_aSegmentBadge[i].Move();
            }
            
            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                m_aSegmentIcon    [i].Move();
                m_aSegmentIconBack[i].Move();
            }


            for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
            {
                //cMenu::UpdateLine(&m_aSegmentLine[i], true, g_aMissionData[i].vColor2);
                m_aSegmentLine[i].SetColor3((m_aiSegmentSelection[0] == i) ? g_aMissionData[i].vColor2 : coreVector3(1.0f,1.0f,1.0f));
            }

            // 
            cMenu::UpdateButton(&m_ArcadeComplete, &m_NavigatorMain, m_ArcadeComplete.IsFocused());
        }
        break;

    case SURFACE_GAME_ARMORY:
        {
            if(m_StartButtonArmory.IsClicked())
            {
                // 
                this->__SetupInput(false);

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
                        m_SpeedNew .Resolve();
                        m_ShieldNew.Resolve();
                        m_WeaponNew.Resolve();

                        // 
                        m_NavigatorArmory.SetActive(false);

                        // 
                        m_bFromTrophy = false;
                    }
                });
            }
            else if(m_InputButtonArmory.IsClicked())
            {
                // 
                this->__SetupInput(true);
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
                else if(m_bFromTrophy)
                {
                    // 
                    m_iStatus = 3;
                }
                else
                {
                    // 
                    g_pMenu->ShiftSurface(this, this->GetOldSurface(), 3.0f, 2u);
                }

                // 
                m_bFromTrophy = false;
            }

            // 
            if(m_ArmoryType.GetUserSwitch() || m_ArmoryMode.GetUserSwitch() || m_ArmoryDifficulty.GetUserSwitch())
            {
                // 
                this->__RefreshBase();

                // 
                switch(m_iCurPage)
                {
                default: UNREACHABLE
                case 0u: this->__PrepareMission(m_aiCurIndex[0]);                  break;
                case 1u: this->__PrepareSegment(m_aiCurIndex[1], m_aiCurIndex[2]); break;
                case 2u: this->__PrepareArcade();                                  break;
                }
            }

            // 
                 if(m_ArmorySelection.GetUserSwitch() < 0) {this->SelectPrevious(); g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}
            else if(m_ArmorySelection.GetUserSwitch() > 0) {this->SelectNext();     g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SWITCH_ENABLED);}


            // 
            coreVector3 vMenuColor;
            switch(m_iCurPage)
            {
            default: UNREACHABLE
            case 0u: vMenuColor = g_aMissionData[m_aiCurIndex[0]].vColor2; break;
            case 1u: vMenuColor = g_aMissionData[m_aiCurIndex[1]].vColor2; break;
            case 2u: vMenuColor = g_aMissionData[MISSION_ATER]   .vColor2; break;
            }
            for(coreUintW i = 0u; i < MENU_GAME_ARMORIES; ++i) cMenu::UpdateLine(&m_aArmoryLine[i], true, true, vMenuColor);
            
            

            // 
            cMenu::UpdateButton(&m_StartButtonArmory, &m_NavigatorArmory, m_StartButtonArmory.IsFocused(), vMenuColor);
            cMenu::UpdateButton(&m_InputButtonArmory, &m_NavigatorArmory, m_InputButtonArmory.IsFocused(), vMenuColor);
            cMenu::UpdateButton(&m_BackButtonArmory,  &m_NavigatorArmory, m_BackButtonArmory .IsFocused(), vMenuColor);
            
            cMenu::UpdateButton(m_ArmorySelection.GetArrow(0u), &m_NavigatorArmory, m_ArmorySelection.GetArrow(0u)->IsFocused(), vMenuColor, true, false);
            cMenu::UpdateButton(m_ArmorySelection.GetArrow(1u), &m_NavigatorArmory, m_ArmorySelection.GetArrow(1u)->IsFocused(), vMenuColor, true, false);

            m_NavigatorArmory.SetGrabColor(vMenuColor);
            
            // 
            m_aArmoryShield[0].GetCaption()->SetColor3(m_aArmoryShield[0].GetCurValue() ? COLOR_MENU_BLUE   : COLOR_MENU_RED);
            m_aArmoryShield[1].GetCaption()->SetColor3(m_aArmoryShield[1].GetCurValue() ? COLOR_MENU_YELLOW : COLOR_MENU_RED);


            const coreBool bMulti = (m_ArmoryType.GetCurValue() != GAME_TYPE_SOLO);
            m_aArmoryShield[1].SetOverride(bMulti ? m_aArmoryShield[0].GetOverride() : -1);
            m_aArmoryWeapon[1].SetOverride(bMulti ? m_aArmoryWeapon[0].GetOverride() : -1);
            m_aArmoryPlayer[1].SetColor3  (COLOR_MENU_WHITE * (bMulti ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));

            m_ArmoryIconBig.SetAlpha(m_ArmoryIconBig.GetAlpha() * 0.6f);

            m_ArmoryFragment.SetTexOffset(coreVector2(0.0f, Core::System->GetTotalTimeFloat(10.0) * 0.1f));

            

            // 
            m_ArmorySpeed     .SetOverride(g_CurConfig.Game.iPureMode ? -1 : 0);
            m_aArmoryShield[0].SetOverride(g_CurConfig.Game.iPureMode ? -1 : 0);
            m_aArmoryShield[1].SetOverride(g_CurConfig.Game.iPureMode ? -1 : m_aArmoryShield[1].GetOverride());

            // 
            m_aArmoryCueLock[0].SetEnabled(g_CurConfig.Game.iPureMode ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aArmoryCueLock[1].SetEnabled(g_CurConfig.Game.iPureMode ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // 
            //cMenu::UpdateSwitchBox(&m_ArmorySelection);
            cMenu::UpdateSwitchBox(&m_ArmoryType);
            cMenu::UpdateSwitchBox(&m_ArmoryMode);
            cMenu::UpdateSwitchBox(&m_ArmoryDifficulty);
            cMenu::UpdateSwitchBox(&m_ArmorySpeed);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aArmoryShield[i]);
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i) cMenu::UpdateSwitchBox(&m_aArmoryWeapon[i]);
            
                        
            for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
            {
                coreVector3 vColor     = COLOR_MENU_WHITE;
                coreVector2 vTexOffset = coreVector2(0.5f,0.0f);
                if(!m_aArmoryWeapon[0].GetOverride())
                {
                    switch(m_aArmoryWeapon[i].GetCurValue())
                    {
                    default: UNREACHABLE
                    case 1u: vColor = COLOR_MENU_YELLOW; vTexOffset = coreVector2(0.0f, 0.0f); break;
                    case 2u: vColor = COLOR_MENU_PURPLE; vTexOffset = coreVector2(0.25f,0.0f); break;
                    case 3u: vColor = COLOR_MENU_GREEN;  vTexOffset = coreVector2(0.5f, 0.0f); break;
                    case 4u: vColor = COLOR_MENU_BLUE;   vTexOffset = coreVector2(0.75f,0.0f); break;
                    case 5u: vColor = COLOR_MENU_RED;    vTexOffset = coreVector2(0.0f, 0.5f); break;
                    }
                }

                m_aArmoryWeapon[i].GetCaption()->SetColor3(vColor);

                m_aArmoryWeaponIcon[i].SetColor3   (vColor);
                m_aArmoryWeaponIcon[i].SetTexOffset(vTexOffset);

                //m_aArmoryWeaponIcon[i].SetAlpha(m_aArmoryWeaponIcon[i].GetAlpha() * 0.6f * m_aArmoryWeapon[i].GetAlpha());
                m_aArmoryWeaponIcon[i].SetAlpha(m_aArmoryWeaponIcon[i].GetAlpha() * 0.8f * m_aArmoryWeapon[i].GetAlpha());
            }
            
            //const coreUint8 iRaiseSpeed   = cGame::CalcRaiseSpeed (m_ArmorySpeed     .GetCurValue());
            //const coreUint8 iRaiseShield1 = cGame::CalcRaiseShield(m_aArmoryShield[0].GetCurValue());
            //const coreUint8 iRaiseShield2 = cGame::CalcRaiseShield(m_aArmoryShield[1].GetCurValue());
            //const coreUint8 iRaiseShield  = bMulti ? ((iRaiseShield1 + iRaiseShield2) / 2u) : iRaiseShield1;
            //const coreUint8 iRaiseFull    = iRaiseSpeed + iRaiseShield;
//
            //m_aArmoryRaise[0].SetText(PRINT("+%u%%", iRaiseSpeed));
            //m_aArmoryRaise[1].SetText(PRINT("+%u%%", iRaiseShield));
            //m_aArmoryRaise[2].SetText(PRINT("+%u%%", iRaiseFull));
//
            //STATIC_ASSERT(GAME_PLAYERS == 2u)

            //for(coreUintW i = 0u; i < MENU_GAME_ARMORY_WAVES; ++i)
            //{
            //    m_aArmoryWave[i].SetAlpha(m_aArmoryWave[i].GetAlpha() * 0.6f);
            //}
            
            for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i)
            {
                const coreFloat fScale = m_aArmoryMedal[i].IsFocused() ? 1.2f : 1.0f;

                m_aArmoryMedal[i].SetSize(coreVector2(1.0f,1.0f) * 0.08f * fScale);
                m_aArmoryMedal[i].Move();
                
                m_aArmoryMedalBest[i].SetPosition(m_aArmoryMedal[i].GetPosition() + coreVector2(0.0f,-0.05f) * (0.08f/0.115f) * fScale);
                m_aArmoryMedalBest[i].SetSize(coreVector2(1.0f,1.0f) * (0.06f * (0.08f/0.115f)) * fScale);
                m_aArmoryMedalBest[i].Move();

                
                m_aArmoryIcon    [i - 1u].SetPosition(m_aArmoryMedal[i].GetPosition() + coreVector2(0.0f,0.04f) * fScale);
                m_aArmoryIconBack[i - 1u].SetPosition(m_aArmoryIcon[i - 1u].GetPosition());

                m_aArmoryIcon    [i - 1u].SetSize(coreVector2(1.0f,1.0f) * 0.065f * fScale);
                m_aArmoryIconBack[i - 1u].SetSize(m_aArmoryIcon[i - 1u].GetSize() * 1.2f);
                

                if(m_aArmoryMedal[i].IsClicked())
                {
                    m_iArmoryStartIndex = i - 1u;
                    m_aiCurIndex[0] = m_iArmoryStartIndex;

                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_BUTTON_PRESS);
                }

                if(TIME && m_aArmoryMedal[i].IsFocusedEnter())
                {
                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_CHANGE_BUTTON);
                }
            }
            
            for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ARROWS; ++i)
            {
                m_aArmoryArrow[i].SetAlpha(m_aArmoryArrow[i].GetAlpha() * ((i < m_iArmoryStartIndex) ? 0.3f : 1.0f));   // TODO 1: handle mission menu
            }
            
            
            const coreFloat   fHeight     = SIN(Core::System->GetTotalTimeFloat(2.0f*PI) * 10.0f) * 0.002f;
            const coreVector2 vTargetPos  = m_aArmoryMedal[m_iArmoryStartIndex + 1u].GetPosition() + coreVector2(0.0f,0.075f + fHeight);
            const coreVector2 vTargetPos2 = m_aArmoryMedal[m_iArmoryStartIndex + 1u].GetPosition() + coreVector2(0.0f,0.075f);
            m_aArmoryStartArrow[0].SetPosition(vTargetPos  + coreVector2(0.0f,-0.0015f));
            m_aArmoryStartArrow[1].SetPosition(vTargetPos);
            m_ArmoryStartLabel    .SetPosition(vTargetPos  + coreVector2(0.0f,0.025f));
            m_ArmoryStartBack     .SetPosition(vTargetPos2 + coreVector2(0.0f,0.025f));
            
            m_ArmoryStartBack .SetAlpha (m_ArmoryStartBack.GetAlpha() * 0.7f);
            m_ArmoryStartLabel.SetColor3(g_aMissionData[m_iArmoryStartIndex].vColor);
            
            const coreFloat fRotation1 = Core::System->GetTotalTimeFloat(4.0f*PI);
            const coreFloat fRotation2 = Core::System->GetTotalTimeFloat(20.0);

            for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
            {
                if(m_aArmoryBadgeWave[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    m_aArmoryBadge[i].SetDirection(coreVector2::Direction(fRotation1 + (0.8f*PI) * (I_TO_F(i) / I_TO_F(3u))));
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
                    m_aArmoryHelper    [i].SetDirection(coreVector2::Direction(fRotation1         + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_HELPERS))));
                    m_aArmoryHelperWave[i].SetDirection(coreVector2::Direction(fRotation1 * -0.5f + (0.8f*PI) * (I_TO_F(i) / I_TO_F(INTERFACE_HELPERS))));
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

            for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS; ++i)
            {
                if(m_aArmoryIcon[i].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
                {
                    m_aArmoryIcon    [i].SetDirection(coreVector2::Direction(fRotation2 *  (0.2f*PI) + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_GAME_ARMORY_ICONS))));
                    m_aArmoryIconBack[i].SetDirection(coreVector2::Direction(fRotation2 * (-0.1f*PI) + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_GAME_ARMORY_ICONS))));
                }
                else
                {
                    m_aArmoryIcon    [i].SetDirection(coreVector2(0.0f,1.0f));
                    m_aArmoryIconBack[i].SetDirection(coreVector2(0.0f,1.0f));
                }
                m_aArmoryIcon    [i].Move();
                m_aArmoryIconBack[i].Move();
                
                m_aArmoryIconBack[i].SetAlpha(m_aArmoryIconBack[i].GetAlpha() * 0.5f);
            }

            // 
            if(!m_aArmoryBadgeDesc[1].GetText()[0] && !m_aArmoryBadgeDesc2[1][0].GetText()[0]) m_aArmoryBadgeBack[1].SetAlpha(m_aArmoryBadgeBack[1].GetAlpha() * 0.5f);
        }
        break;

    case SURFACE_GAME_FIRST:
        {
            if(m_StartButtonFirst.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                this->__SetupInput(false);

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
            else if(m_InputButtonFirst.IsClicked())
            {
                // 
                this->__SetupInput(true);
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
            cMenu::UpdateButton(&m_StartButtonFirst, &m_NavigatorFirst, m_StartButtonFirst.IsFocused());
            cMenu::UpdateButton(&m_InputButtonFirst, &m_NavigatorFirst, m_InputButtonFirst.IsFocused());
            cMenu::UpdateButton(&m_BackButtonFirst,  &m_NavigatorFirst, m_BackButtonFirst .IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_GAME_FIRSTS; ++i) cMenu::UpdateLine(&m_aFirstLine[i], true);

            // 
            m_aFirstShield[0].GetCaption()->SetColor3(m_aFirstShield[0].GetCurValue() ? COLOR_MENU_BLUE   : COLOR_MENU_RED);
            m_aFirstShield[1].GetCaption()->SetColor3(m_aFirstShield[1].GetCurValue() ? COLOR_MENU_YELLOW : COLOR_MENU_RED);

            const coreBool bMulti = (m_FirstType.GetCurValue() != GAME_TYPE_SOLO);
            m_aFirstShield[1].SetOverride(bMulti ? 0 : -1);
            m_aFirstPlayer[1].SetColor3  (COLOR_MENU_WHITE * (bMulti ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));

            // 
            m_FirstSpeed     .SetOverride(g_CurConfig.Game.iPureMode ? -1 : 0);
            m_aFirstShield[0].SetOverride(g_CurConfig.Game.iPureMode ? -1 : 0);
            m_aFirstShield[1].SetOverride(g_CurConfig.Game.iPureMode ? -1 : m_aFirstShield[1].GetOverride());

            // 
            m_aFirstCueLock[0].SetEnabled(g_CurConfig.Game.iPureMode ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aFirstCueLock[1].SetEnabled(g_CurConfig.Game.iPureMode ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            
            //const coreUint8 iRaiseSpeed   = cGame::CalcRaiseSpeed (m_FirstSpeed     .GetCurValue());
            //const coreUint8 iRaiseShield1 = cGame::CalcRaiseShield(m_aFirstShield[0].GetCurValue());
            //const coreUint8 iRaiseShield2 = cGame::CalcRaiseShield(m_aFirstShield[1].GetCurValue());
            //const coreUint8 iRaiseShield  = bMulti ? ((iRaiseShield1 + iRaiseShield2) / 2u) : iRaiseShield1;
            //const coreUint8 iRaiseFull    = iRaiseSpeed + iRaiseShield;
//
            //m_aFirstRaise[0].SetText(PRINT("+%u%%", iRaiseSpeed));
            //m_aFirstRaise[1].SetText(PRINT("+%u%%", iRaiseShield));
            //m_aFirstRaise[2].SetText(PRINT("+%u%%", iRaiseFull));
//
            //STATIC_ASSERT(GAME_PLAYERS == 2u)
        }
        break;

    case SURFACE_GAME_DEMO:
        {
            if(m_StartButtonDemo.IsClicked())
            {
                // 
                this->SaveValues();

                // 
                this->__SetupInput(false);

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
            else if(m_InputButtonDemo.IsClicked())
            {
                // 
                this->__SetupInput(true);
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
            cMenu::UpdateButton(&m_StartButtonDemo, &m_NavigatorDemo, m_StartButtonDemo.IsFocused());
            cMenu::UpdateButton(&m_InputButtonDemo, &m_NavigatorDemo, m_InputButtonDemo.IsFocused());
            cMenu::UpdateButton(&m_BackButtonDemo,  &m_NavigatorDemo, m_BackButtonDemo .IsFocused());

            // 
            for(coreUintW i = 0u; i < MENU_GAME_DEMOS; ++i) cMenu::UpdateLine(&m_aDemoLine[i], true);

            // 
            m_aDemoShield[0].GetCaption()->SetColor3(m_aDemoShield[0].GetCurValue() ? COLOR_MENU_BLUE   : COLOR_MENU_RED);
            m_aDemoShield[1].GetCaption()->SetColor3(m_aDemoShield[1].GetCurValue() ? COLOR_MENU_YELLOW : COLOR_MENU_RED);

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
            m_DemoSpeed     .SetOverride(g_CurConfig.Game.iPureMode ? -1 : 0);
            m_aDemoShield[0].SetOverride(g_CurConfig.Game.iPureMode ? -1 : 0);
            m_aDemoShield[1].SetOverride(g_CurConfig.Game.iPureMode ? -1 : m_aDemoShield[1].GetOverride());

            // 
            m_aDemoCueLock[0].SetEnabled(g_CurConfig.Game.iPureMode ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aDemoCueLock[1].SetEnabled(g_CurConfig.Game.iPureMode ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // 
            m_DemoStageIcon.SetColor3   (g_aMissionData[m_DemoStage.GetCurValue()].vColor * 0.8f);
            m_DemoStageIcon.SetTexOffset(g_aMissionData[m_DemoStage.GetCurValue()].vIcon);
            m_DemoStageIcon.SetEnabled  (bUnlocked ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // 
            m_DemoStageIcon.SetAlpha(m_DemoStageIcon.GetAlpha() * 0.6f);
            
            //const coreUint8 iRaiseSpeed   = cGame::CalcRaiseSpeed (m_DemoSpeed     .GetCurValue());
            //const coreUint8 iRaiseShield1 = cGame::CalcRaiseShield(m_aDemoShield[0].GetCurValue());
            //const coreUint8 iRaiseShield2 = cGame::CalcRaiseShield(m_aDemoShield[1].GetCurValue());
            //const coreUint8 iRaiseShield  = bMulti ? ((iRaiseShield1 + iRaiseShield2) / 2u) : iRaiseShield1;
            //const coreUint8 iRaiseFull    = iRaiseSpeed + iRaiseShield;
//
            //m_aDemoRaise[0].SetText(PRINT("+%u%%", iRaiseSpeed));
            //m_aDemoRaise[1].SetText(PRINT("+%u%%", iRaiseShield));
            //m_aDemoRaise[2].SetText(PRINT("+%u%%", iRaiseFull));
//
            //STATIC_ASSERT(GAME_PLAYERS == 2u)
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    // 
    cMenu::UpdateButton(&m_BackButtonMain, &m_NavigatorMain, m_BackButtonMain.IsFocused());
    
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
    m_ArmorySpeed     .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_GAMESPEED : g_CurConfig.Game.iGameSpeed);

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        m_aArmoryShield[i].SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_SHIELD : g_pSave->GetHeader().oOptions.aiShield[i]);
        m_aArmoryWeapon[i].SelectValue(g_pSave->GetHeader().oOptions.aaiWeapon[i][0]);
        m_aArmoryPlayer[i].SetText    (PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
    }

    if(m_bFirstPlay)
    {
        // 
        m_FirstType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
        m_FirstDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
        m_FirstSpeed     .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_GAMESPEED : g_CurConfig.Game.iGameSpeed);

        // 
        for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
        {
            m_aFirstShield[i].SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_SHIELD : g_pSave->GetHeader().oOptions.aiShield[i]);
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
        m_DemoStage.AddEntry(g_aMissionData[0].pcName, 0u);
        if(!g_pSave->GetHeader().oProgress.bFirstPlay)
        {
            m_DemoStage.AddEntry(g_aMissionData[1].pcName, 1u);
            m_DemoStage.AddEntry(g_aMissionData[2].pcName, 2u);
        }

        // 
        m_DemoType      .SelectValue(g_pSave->GetHeader().oOptions.iType);
        m_DemoDifficulty.SelectValue(g_pSave->GetHeader().oOptions.iDifficulty);
        m_DemoSpeed     .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_GAMESPEED : g_CurConfig.Game.iGameSpeed);
        m_DemoStage     .SelectValue(g_pSave->GetHeader().oOptions.iNavigation);

        // 
        for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
        {
            m_aDemoShield[i].SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_SHIELD : g_pSave->GetHeader().oOptions.aiShield[i]);
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
    g_CurConfig.Game.iGameSpeed         = g_CurConfig.Game.iPureMode ? g_CurConfig.Game.iGameSpeed : m_ArmorySpeed.GetCurValue();

    // 
    for(coreUintW i = 0u; i < MENU_GAME_PLAYERS; ++i)
    {
        g_pSave->EditOptions()->aiShield [i]    = g_CurConfig.Game.iPureMode ? g_pSave->EditOptions()->aiShield[i] : m_aArmoryShield[i].GetCurValue();
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
    else UNREACHABLE
}


// ****************************************************************
// 
void cGameMenu::RetrievePageData(coreUintW* OUTPUT piMissionIndex, coreUintW* OUTPUT piSegmentIndex)const
{
    ASSERT(piMissionIndex && piSegmentIndex)

    // 
    (*piMissionIndex) = m_aiCurIndex[1];
    (*piSegmentIndex) = m_aiCurIndex[2];
    ASSERT(m_iCurPage == 1u)
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
        m_NavigatorMain.ForceCurrent(&m_aSegmentIconBack[m_aiCurIndex[0]]);
        m_SegmentBox.ScrollToObject(&m_aSegmentIconBack[m_aiCurIndex[0]], true);
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
    else UNREACHABLE
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
        m_NavigatorMain.ForceCurrent(&m_aSegmentIconBack[m_aiCurIndex[0]]);
        m_SegmentBox.ScrollToObject(&m_aSegmentIconBack[m_aiCurIndex[0]], true);
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
    else UNREACHABLE
}


// ****************************************************************
// 
void cGameMenu::SelectTrophy(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    m_iCurPage = 1u;

    // 
    this->__SelectSegment (iMissionIndex, iSegmentIndex);
    this->__PrepareSegment(iMissionIndex, iSegmentIndex);

    m_aiSegmentSelection[0] = m_aiCurIndex[1];
    m_aiSegmentSelection[1] = m_aiCurIndex[2];
    m_NavigatorMain.ForceCurrent(&m_aaSegmentTile[m_aiCurIndex[1]][m_aiCurIndex[2]]);
    m_SegmentBox.ScrollToObject(&m_aaSegmentTile[m_aiCurIndex[1]][m_aiCurIndex[2]], true);
    
    m_bFromTrophy = true;
}


// ****************************************************************
// 
void cGameMenu::__LoadUnlocks()
{
    const coreBool bDuelType       = false;
    const coreBool bHardDifficulty = DEFINED(_CORE_DEBUG_);
    const coreBool bGameSpeedUp    = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_GAMESPEEDUP);
    const coreBool bPowerShield    = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_POWERSHIELD);
    const coreBool bWeaponPulse    = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_PULSE);
    const coreBool bWeaponWave     = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_WAVE);
    const coreBool bWeaponTesla    = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_TESLA);
    const coreBool bWeaponAnti     = HAS_BIT_EX(g_pSave->EditProgress()->aiUnlock, UNLOCK_WEAPON_ANTI);
    const coreBool bAnyWeapons     = bWeaponPulse || bWeaponWave || bWeaponTesla || bWeaponAnti;

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
        m_aArmoryShield[i].ClearEntries();
        m_aArmoryShield[i].AddEntryLanguage("VALUE_NO", 0u);
        for(coreUintW j = 5u, je = bPowerShield ? 100u : SHIELD_DEFAULT; j <= je; j += 5u) m_aArmoryShield[i].AddEntry(PRINT("%zu", j), j);
        if(bPowerShield)
        {
            m_aArmoryShield[i].AddEntry(coreData::ToChars(SHIELD_INVINCIBLE), SHIELD_MAX);
        }

        // 
        m_aArmoryWeapon[i].ClearEntries();
        if(bAnyWeapons)
        {
                             m_aArmoryWeapon[i].AddEntryLanguage("GAME_WEAPON_RAY",   1u);
            if(bWeaponPulse) m_aArmoryWeapon[i].AddEntryLanguage("GAME_WEAPON_PULSE", 2u);
            if(bWeaponWave)  m_aArmoryWeapon[i].AddEntryLanguage("GAME_WEAPON_WAVE",  3u);
            if(bWeaponTesla) m_aArmoryWeapon[i].AddEntryLanguage("GAME_WEAPON_TESLA", 4u);
            if(bWeaponAnti)  m_aArmoryWeapon[i].AddEntryLanguage("GAME_WEAPON_ANTI",  5u);
        }
        else
        {
            m_aArmoryWeapon[i].AddEntryLanguage("UNKNOWN", 1u);
        }

        // 
        m_aArmoryWeapon    [i].SetOverride(bAnyWeapons ? 0 : -1);
        m_aArmoryWeaponIcon[i].SetEnabled (bAnyWeapons ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
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
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    // 
    coreUintW iMaxMission = 0u;
    coreUintW iHiddenTile = 0u;   // first few tiles are never hidden
    coreUintW iCountBoss  = 0u;
    coreUintW iCountWave  = 0u;
    for(coreUintW i = 0u; i < MENU_GAME_MISSIONS; ++i)
    {
        const coreUint8        iAdvance = oProgress.aiAdvance[i] * ((i < MISSION_BASE) ? 1u : 0u);
        const coreObjectEnable eEnabled = iAdvance ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

        for(coreUintW j = 0u; j < MENU_GAME_SEGMENTS; ++j)
        {
            const coreBool         bVisible  = (iAdvance >= j + 1u) && ((i != MISSION_ATER) || MISSION_SEGMENT_IS_BOSS(j));
            const coreObjectEnable eEnabled2 = bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

            m_aaSegmentTile[i][j].SetEnabled  (eEnabled2);
            m_aaSegmentTile[i][j].SetFocusable(bVisible);
            m_aaSegmentBack[i][j].SetEnabled  (eEnabled2);
            
            const coreBool    bBoss      = MISSION_SEGMENT_IS_BOSS(j);
            const coreVector2 vStep      = bBoss ? coreVector2((1.0f/37.0f), (1.0f/21.0f) * (581.0f/512.0f)) : coreVector2((1.0f/41.0f), (1.0f/41.0f));
            const coreVector2 vTexSize   = bBoss ? coreVector2(8.0f,4.0f)                                    : coreVector2(4.0f,4.0f);
            const coreVector2 vTexOffset = bBoss ? coreVector2(I_TO_F(iCountBoss % 4u) * 9.0f + 1.0f, I_TO_F(iCountBoss / 4u) * 5.0f + 1.0f) :
                                                   coreVector2(I_TO_F(iCountWave % 8u) * 5.0f + 1.0f, I_TO_F(iCountWave / 8u) * 5.0f + 1.0f);

            m_aaSegmentTile[i][j].DefineTexture(0u, bBoss ? "menu_segment_boss.png" : "menu_segment_wave.png");
            m_aaSegmentTile[i][j].SetTexSize   (vTexSize   * vStep);
            m_aaSegmentTile[i][j].SetTexOffset (vTexOffset * vStep);

            if(!iHiddenTile && (iAdvance <= j + 1u))
            {
                iHiddenTile = i * MENU_GAME_SEGMENTS + j;
            }

            if(bBoss) iCountBoss += 1u;
                 else iCountWave += 1u;
        }

        for(coreUintW j = 0u; j < MENU_GAME_ARROWS; ++j)
        {
            const coreBool bVisible = m_aaSegmentTile[i][j].IsEnabled(CORE_OBJECT_ENABLE_ALL) && m_aaSegmentTile[i][j + 1u].IsEnabled(CORE_OBJECT_ENABLE_ALL);

            m_aaSegmentArrow[i][j].SetEnabled(bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            STATIC_ASSERT(MENU_GAME_ARROWS < MENU_GAME_SEGMENTS)
        }

        m_aSegmentName    [i].SetEnabled  (eEnabled);
        m_aSegmentNumber  [i].SetEnabled  (eEnabled);
        m_aSegmentIcon    [i].SetEnabled  (eEnabled);
        m_aSegmentIconBack[i].SetEnabled  (eEnabled);
        m_aSegmentIconBack[i].SetFocusable(DEFINED(_CORE_DEBUG_) ? iAdvance : false);
        m_aSegmentLine    [i].SetEnabled  (eEnabled);

        m_aSegmentNumber[i].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetMissionLetters(i)));

        if(iAdvance) iMaxMission = i;
    }

    if(iHiddenTile)
    {
        const coreBool bBoss = MISSION_SEGMENT_IS_BOSS(iHiddenTile % MENU_GAME_SEGMENTS);
        cGuiObject& oHidden = m_aaSegmentTile[iHiddenTile / MENU_GAME_SEGMENTS][iHiddenTile % MENU_GAME_SEGMENTS];

        const coreVector2 vStep      = coreVector2((1.0f/41.0f), (1.0f/41.0f));
        const coreVector2 vTexSize   = coreVector2(bBoss ? 8.0f : 4.0f,4.0f);
        const coreVector2 vTexOffset = coreVector2(I_TO_F(62u % 8u) * 5.0f + 1.0f + (bBoss ? -2.0f : 0.0f), I_TO_F(62u / 8u) * 5.0f + 1.0f);

        oHidden.DefineTexture(0u, "menu_segment_wave.png");
        oHidden.SetTexSize   (vTexSize   * vStep);
        oHidden.SetTexOffset (vTexOffset * vStep);
    }

    // 
    m_SegmentBox.SetMaxOffset(0.25f * I_TO_F(iMaxMission + 1u) - m_SegmentBox.GetSize().y + 0.15f);
}


// ****************************************************************
// 
void cGameMenu::__RefreshCursor(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    // 
    const cGuiObject& oTile = m_aaSegmentTile[iMissionIndex][iSegmentIndex];

    m_aSegmentCursor[1].DefineTexture(0u, oTile.GetTexture(0u));
    m_aSegmentCursor[1].SetPosition  (oTile.GetPosition ());
    m_aSegmentCursor[1].SetSize      (oTile.GetSize     () * 1.54f);
    m_aSegmentCursor[1].SetTexSize   (oTile.GetTexSize  ());
    m_aSegmentCursor[1].SetTexOffset (oTile.GetTexOffset());
    m_aSegmentCursor[1].Move();

    m_aSegmentCursor[0].SetPosition(m_aSegmentCursor[1].GetPosition());
    m_aSegmentCursor[0].SetSize    (m_aSegmentCursor[1].GetSize    () + coreVector2(0.01f,0.01f));
    m_aSegmentCursor[0].Move();

    m_aSegmentCursor[2].SetPosition(m_aSegmentCursor[1].GetPosition());
    m_aSegmentCursor[2].SetSize    (m_aSegmentCursor[1].GetSize    () + coreVector2(0.1f,0.25f));
    m_aSegmentCursor[2].Move();

    m_aSegmentCursor[1].SetColor3(g_aMissionData[iMissionIndex].vColor);
    

    coreUint8 iMaxMedal = MEDAL_NONE;
    ALL_MEDAL_SEGMENT(aaiMedal) {iMaxMedal = MAX(iMaxMedal, aaiMedal[iMissionIndex][iSegmentIndex]); return false;});

    cMenu::ApplyMedalTexture(&m_SegmentMedal, iMaxMedal, MISSION_SEGMENT_IS_BOSS(iSegmentIndex) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);
    
    m_SegmentMedal.SetPosition(oTile.GetPosition() + coreVector2(0.0f, 0.065f*1.54f + 0.01f));
    m_SegmentMedal.Move();
    
    
    if(MISSION_SEGMENT_IS_BOSS(iSegmentIndex))
    {
        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
        {
            m_aSegmentBadge[i].SetPosition(HIDDEN_POS);
            m_aSegmentBadge[i].Move();

            m_aSegmentBadgeWave[i].SetPosition(HIDDEN_POS);
            m_aSegmentBadgeWave[i].Move();
        }
    }
    else
    {
        const auto& oProgress = g_pSave->GetHeader().oProgress;

        const coreBool bIntro = (iMissionIndex == MISSION_INTRO);

        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
        {
            const coreBool bState = HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], i);

            m_aSegmentBadge[i].SetPosition (oTile.GetPosition() + coreVector2(bIntro ? 0.0f : ((i ? 1.0f : -1.0f) * 0.021f * 1.54f), -0.065f*1.54f));
            m_aSegmentBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
            m_aSegmentBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));

            m_aSegmentBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            m_aSegmentBadge[i].Move();

            m_aSegmentBadgeWave[i].SetPosition(m_aSegmentBadge[i].GetPosition());
            m_aSegmentBadgeWave[i].Move();
        }
        
        m_aSegmentBadge[1].SetEnabled(bIntro ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
    }
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
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), GAME_GOAL_FACTOR);

    // 
    m_aSegmentTitle[0].SetColor3(oData.vColor);
    m_aSegmentScore[0].SetColor3(oData.vColor);
    m_aSegmentTime [0].SetColor3(oData.vColor);

    // 
    m_aSegmentTitle[0].SetText(bBoss ? oData.pcBoss : oData.pcName);
    m_aSegmentTitle[1].SetText(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aSegmentScore[1].SetText(oStats.iScoreBest ? coreData::ToChars(oStats.iScoreBest)     : "-");
    m_aSegmentTime [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift) : "-");

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
            const coreBool bState = HAS_BIT(oProgress.aiBadge[iMissionIndex][iSegmentIndex], i);

            m_aSegmentBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
            m_aSegmentBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
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
void cGameMenu::__PrepareArcade()
{
    // 
    const sMissionData& oData = g_aMissionData[MISSION_ATER];

    // 
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaaLocalStatsArcade[m_iBaseType][m_iBaseMode][m_iBaseDifficulty];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), GAME_GOAL_FACTOR);

    // 
    m_aArmoryScore[0].SetColor3(oData.vColor);
    m_aArmoryTime [0].SetColor3(oData.vColor);

    // 
    m_aArmoryScore[1].SetText(oStats.iScoreBest ? PRINT("%'u",      oStats.iScoreBest)     : "-");
    m_aArmoryTime [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift) : "-");
    
    if(oStats.iScoreBest && !fBestTime)
    {
        m_aArmoryTime[1].SetTextLanguage("UNKNOWN");
    }

    // 
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], oProgress.aaaiMedalArcade[m_iBaseType][m_iBaseMode][m_iBaseDifficulty], MEDAL_TYPE_ARCADE, false);
    
    coreUint8 iMaxMedal = MEDAL_NONE;
    ALL_MEDAL_ARCADE(iMedal) {iMaxMedal = MAX(iMaxMedal, iMedal); return false;});

    cMenu::ApplyMedalTexture(&m_aArmoryMedalBest[0], iMaxMedal, MEDAL_TYPE_ARCADE, false);

    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        cMenu::ApplyMedalTexture(&m_aArmoryMedal[i], oProgress.aaaaiMedalMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][i - 1u], MEDAL_TYPE_MISSION, false);

        iMaxMedal = MEDAL_NONE;
        ALL_MEDAL_MISSION(aiMedal) {iMaxMedal = MAX(iMaxMedal, aiMedal[i - 1u]); return false;});

        cMenu::ApplyMedalTexture(&m_aArmoryMedalBest[i], iMaxMedal, MEDAL_TYPE_MISSION, false);

        const coreBool bProgress = iMaxMedal; // (oProgress.aiAdvance[i - 1u] >= 7u);
        
        if(i + 1u < MENU_GAME_ARMORY_MEDALS)
        {
            m_aArmoryMedal[i + 1u].SetFocusable(bProgress || HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiState, STATE_FULL_ACCESS));
        }
        
        m_aArmoryIcon    [i - 1u].SetEnabled(bProgress ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aArmoryIconBack[i - 1u].SetEnabled(bProgress ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
    m_aArmoryMedal[1].SetFocusable(true);
    
    

    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        const coreBool bEnabled = m_aArmoryMedal[i].IsFocusable();

        m_aArmoryMedal     [i].SetEnabled(bEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aArmoryMedalBest [i].SetEnabled(bEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        m_aArmoryMedalBack2[i].SetEnabled(bEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        
        if(i >= 2u) m_aArmoryArrow[i - 2u].SetEnabled(bEnabled ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS; ++i)
    {
        const coreBool bEnabled = m_aArmoryMedal[i + 1u].IsFocusable();//oProgress.aaaaiMedalMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][i];

        if(!bEnabled) m_aArmoryIcon    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        if(!bEnabled) m_aArmoryIconBack[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
    
    
    
    // 
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i) m_aArmoryBadge     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i) m_aArmoryBadgeWave [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;     ++i) m_aArmoryBadgeBack [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;     ++i) m_aArmoryBadgeDesc [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;     ++i) for(coreUintW j = 0u; j < ARRAY_SIZE(m_aArmoryBadgeDesc2[0]); ++j) m_aArmoryBadgeDesc2[i][j].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;    ++i) m_aArmoryHelper    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;    ++i) m_aArmoryHelperWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 1u; i < ARRAY_SIZE(m_aArmoryTitle);  ++i) m_aArmoryTitle     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryScore); ++i) m_aArmoryScore     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTime);  ++i) m_aArmoryTime      [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    m_ArmoryMedalBack   .SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryFragment    .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryFragmentBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aArmoryHelperBack .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryIconBig     .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    m_aArmoryStartArrow[0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aArmoryStartArrow[1].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryStartLabel.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryStartBack.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    m_aArmoryTitle[0].SetColor3(COLOR_MENU_WHITE);
    m_aArmoryTitle[0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_aArmoryTitle[0].SetTextLanguage("GAME_ARCADE_COMPLETE");

    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.123f));
    
    
    //for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i) m_aArmoryMedalBack2[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    //for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ARROWS; ++i) m_aArmoryArrow[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    m_ArmorySelection.SetEnabled(CORE_OBJECT_ENABLE_MOVE);   // #
    m_ArmoryTile.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    m_ArmoryTrophy.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    
    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS; ++i)
    {
        m_aArmoryMedal[i].SetPosition(coreVector2(m_ArmoryArea2.GetPosition().x + (I_TO_F(i - 1u) - 4.0f) * 0.09f, m_ArmoryMedalBack.GetPosition().y));
        m_aArmoryMedal[i].SetSize    (coreVector2(1.0f,1.0f) * 0.08f);      
        m_aArmoryMedal[i].Move();
        
        //m_aArmoryMedalBest[i].SetPosition(m_aArmoryMedal[i].GetPosition() + coreVector2(0.0f,-0.05f) * (0.08f/0.115f));
        //m_aArmoryMedalBest[i].Move();
        
        m_aArmoryMedalBack2[i].SetPosition(m_aArmoryMedal[i].GetPosition());
        m_aArmoryMedalBack2[i].SetSize    (m_aArmoryMedal[i].GetSize() + coreVector2(0.05f,0.05f));
        m_aArmoryMedalBack2[i].Move();
    }
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS; ++i)
    {
        //m_aArmoryIcon[i].SetPosition(m_aArmoryMedal[i + 1u].GetPosition() + coreVector2(0.0f,0.04f));
        //m_aArmoryIcon[i].Move();
        //
        //m_aArmoryIconBack[i].SetPosition(m_aArmoryIcon[i].GetPosition());
        //m_aArmoryIconBack[i].Move();
        
        STATIC_ASSERT(MENU_GAME_ARMORY_ICONS + 1u == MENU_GAME_ARMORY_MEDALS)
    }

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ARROWS; ++i)
    {
        m_aArmoryArrow[i].SetPosition(LERP(m_aArmoryMedal[i + 1u].GetPosition(), m_aArmoryMedal[i + 2u].GetPosition(), 0.5f) + coreVector2(0.001f,0.0f));
        m_aArmoryArrow[i].Move();
        
        STATIC_ASSERT(MENU_GAME_ARMORY_ARROWS + 2u <= MENU_GAME_ARMORY_MEDALS)
    }
    
    m_NavigatorArmory.SetShoulder(false);
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
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), GAME_GOAL_FACTOR);

    // 
    m_aArmoryTitle[0].SetColor3(oData.vColor);
    m_aArmoryScore[0].SetColor3(oData.vColor);
    m_aArmoryTime [0].SetColor3(oData.vColor);

    // 
    m_aArmoryTitle[0].SetText(oData.pcName);
    m_aArmoryTitle[1].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetMissionLetters(iMissionIndex)));
    m_aArmoryScore[1].SetText(oStats.iScoreBest ? PRINT("%'u",      oStats.iScoreBest)     : "-");
    m_aArmoryTime [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift) : "-");

    // 
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], oProgress.aaaaiMedalMission[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex], MEDAL_TYPE_MISSION, false);
    
    coreUint8 iMaxMedal = MEDAL_NONE;
    ALL_MEDAL_MISSION(aiMedal) {iMaxMedal = MAX(iMaxMedal, aiMedal[iMissionIndex]); return false;});

    cMenu::ApplyMedalTexture(&m_aArmoryMedalBest[0], iMaxMedal, MEDAL_TYPE_MISSION, false);

    // 
    for(coreUintW i = 1u; i < 7u; ++i)
    {
        const coreBool bBoss = MISSION_SEGMENT_IS_BOSS(i - 1u);

        cMenu::ApplyMedalTexture(&m_aArmoryMedal[i], oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][i - 1u], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);

        iMaxMedal = MEDAL_NONE;
        ALL_MEDAL_SEGMENT(aaiMedal) {iMaxMedal = MAX(iMaxMedal, aaiMedal[iMissionIndex][i - 1u]); return false;});

        cMenu::ApplyMedalTexture(&m_aArmoryMedalBest[i], iMaxMedal, bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);
    }

    // 
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
    {
        const coreBool bState = HAS_BIT(oProgress.aaiBadge[iMissionIndex][i / MENU_GAME_ARMORY_BADGES], (i % MENU_GAME_ARMORY_BADGES));

        m_aArmoryBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
        m_aArmoryBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
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
    m_aArmoryHelperBack .SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 7u; i < MENU_GAME_ARMORY_MEDALS;    ++i) m_aArmoryMedal     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS;    ++i) m_aArmoryMedal     [i].SetFocusable(false);
    for(coreUintW i = 7u; i < MENU_GAME_ARMORY_MEDALS;    ++i) m_aArmoryMedalBest [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 1u; i < MENU_GAME_ARMORY_MEDALS;    ++i) m_aArmoryMedalBack2[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;    ++i) m_aArmoryBadgeBack [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;    ++i) m_aArmoryBadgeDesc [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES;    ++i) for(coreUintW j = 0u; j < ARRAY_SIZE(m_aArmoryBadgeDesc2[0]); ++j) m_aArmoryBadgeDesc2[i][j].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;   ++i) m_aArmoryHelper    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS;   ++i) m_aArmoryHelperWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS;     ++i) m_aArmoryIcon      [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ICONS;     ++i) m_aArmoryIconBack  [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_ARROWS;    ++i) m_aArmoryArrow     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTitle); ++i) m_aArmoryTitle     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryScore); ++i) m_aArmoryScore     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArmoryTime);  ++i) m_aArmoryTime      [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    m_aArmoryStartArrow[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aArmoryStartArrow[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryStartLabel.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryStartBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    for(coreUintW i = 1u; i < 7u; ++i) m_aArmoryMedalBack2[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u; i < 5u; ++i) m_aArmoryArrow[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    for(coreUintW i = 1u; i < 7u; ++i)
    {
        m_aArmoryMedal[i].SetPosition(coreVector2(m_ArmoryArea2.GetPosition().x + (I_TO_F(i - 1u) - 2.5f) * 0.14f, m_ArmoryMedalBack.GetPosition().y));
        m_aArmoryMedal[i].SetSize    (coreVector2(1.0f,1.0f) * ((i == 6u) ? 0.09f : 0.08f));
        m_aArmoryMedal[i].Move();
    }
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES_ALL; ++i)
    {
        m_aArmoryBadge[i].SetPosition(m_aArmoryMedal[(i / MENU_GAME_ARMORY_BADGES) + 1u].GetPosition() + coreVector2((I_TO_F(i % MENU_GAME_ARMORY_BADGES) - 0.5f) * ((iMissionIndex == MISSION_INTRO) ? 0.0f : 0.06f), 0.07f) * 0.8f);
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
            if(i % MENU_GAME_ARMORY_BADGES)
            {
                m_aArmoryBadge    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                m_aArmoryBadgeWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            }
        }
    }
    else if(iMissionIndex == MISSION_ATER)
    {
        cMenu::ApplyMedalTexture(&m_aArmoryMedal[5], oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][6], MEDAL_TYPE_BOSS, true);

        iMaxMedal = MEDAL_NONE;
        ALL_MEDAL_SEGMENT(aaiMedal) {iMaxMedal = MAX(iMaxMedal, aaiMedal[iMissionIndex][6]); return false;});

        cMenu::ApplyMedalTexture(&m_aArmoryMedalBest[5], iMaxMedal, MEDAL_TYPE_BOSS, true);

        const coreBool bEigengrau = m_aArmoryMedal[5].IsEnabled(CORE_OBJECT_ENABLE_ALL);

        for(coreUintW i = 1u; i < 5u; ++i)
        {
            m_aArmoryMedal[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            m_aArmoryMedalBest[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }

        for(coreUintW i = bEigengrau ? 1u : 0u; i < 5u; ++i) m_aArmoryMedalBack2[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        for(coreUintW i = 1u, ie = bEigengrau ? 5u : 6u; i < ie; ++i)  m_aArmoryArrow[i - 1u].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

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
    
    
    

    for(coreUintW i = 0u; i < 5u; ++i)
    {
        m_aArmoryArrow[i].SetPosition(LERP(m_aArmoryMedal[i + 1u].GetPosition(), m_aArmoryMedal[i + 2u].GetPosition(), 0.5f) + coreVector2(0.001f,0.0f));
        m_aArmoryArrow[i].Move();
    }
    
    
    for(coreUintW i = 1u; i < 7u; ++i)
    {
        m_aArmoryMedalBest[i].SetPosition(m_aArmoryMedal[i].GetPosition() + coreVector2(0.0f,-0.05f) * (0.08f/0.115f));
        m_aArmoryMedalBest[i].Move();
        
        m_aArmoryMedalBack2[i].SetPosition(m_aArmoryMedal[i].GetPosition());
        m_aArmoryMedalBack2[i].SetSize    (m_aArmoryMedal[i].GetSize() + coreVector2(0.05f,0.05f));
        m_aArmoryMedalBack2[i].Move();
    }
    
    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.105f));
    m_aArmoryTitle[1].SetPosition (m_aArmoryTitle[0].GetPosition() + coreVector2(0.0f,0.044f));
    m_ArmoryIconBig.SetPosition  (m_aArmoryTitle[0].GetPosition() + coreVector2(0.0f,0.018f));

    m_ArmoryFragment.SetPosition(m_aArmoryMedal[6].GetPosition() + coreVector2(0.0f,0.05f) + g_aFragmentData[iMissionIndex].vShift);
    m_ArmoryFragment.Move();

    m_ArmoryFragmentBack.SetPosition(m_aArmoryMedal[6].GetPosition() + coreVector2(0.0f,0.05f));
    m_ArmoryFragmentBack.Move();
    
    m_ArmorySelection.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryTile.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    m_ArmoryTrophy.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    m_NavigatorArmory.SetShoulder(true);
}


// ****************************************************************
// 
void cGameMenu::__PrepareSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex)
{
    ASSERT((iMissionIndex < MISSIONS) && (iSegmentIndex < SEGMENTS))
    
    /*
    constexpr const coreChar* aapcName[][5] =
    {
        {
            "NULL",
            "EINS",
            "ZWEI",
            "DREI",
            "VIER",
        },
        {
            "ZEHN",
            "ELF",
            "ZWÖLF",
            "DREIZEHN",
            "VIERZEHN",
        },
        {
            "ZWNAZIG",
            "EINUNDZWANZIG",
            "ZWEIUNDZWANZIG",
            "DREIUNDZWANZIG",
            "VIERUNDZWANZIG",
        },
        {
            "DREISSIG",
            "EINUNDDREISSIG",
            "ZWEIUNDDREISSIG",
            "DREIUNDDREISSIG",
            "VIERUNDDREISSIG",
        },
        {
            "VIERZIG",
            "EINUNDVIERZIG",
            "ZWEIUNDVIERZIG",
            "DREIUNDVIERZIG",
            "VIERUNDVIERZIG",
        },
        {
            "FÜNFZIG",
            "EINUNDFÜNFZIG",
            "ZWEIUNDFÜNFZIG",
            "DREIUNDFÜNFZIG",
            "VIERUNDFÜNFZIG",
        },
        {
            "SECHZIG",
            "EINUNDSECHZIG",
            "ZWEIUNDSECHZIG",
            "DREIUNDSECHZIG",
            "VIERUNDSECHZIG",
        },
        {
            "SIEBZIG",
            "EINUNDSIEBZIG",
            "ZWEIUNDSIEBZIG",
            "DREIUNDSIEBZIG",
            "VIERUNDSIEBZIG",
        }
    };
    */
    

    // 
    const sMissionData& oData = g_aMissionData[iMissionIndex];
    const coreBool      bBoss = MISSION_SEGMENT_IS_BOSS(iSegmentIndex);

    // 
    const auto& oProgress = g_pSave->GetHeader().oProgress;

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaaaaLocalStatsSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][iSegmentIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), GAME_GOAL_FACTOR);

    // 
    m_aArmoryTitle[0].SetColor3(oData.vColor);
    m_aArmoryScore[0].SetColor3(oData.vColor);
    m_aArmoryTime [0].SetColor3(oData.vColor);

    // 
    m_aArmoryTitle[0].SetText(bBoss ? oData.pcBoss : oData.pcName);
    m_aArmoryTitle[1].SetText(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aArmoryScore[1].SetText(oStats.iScoreBest ? PRINT("%'u",      oStats.iScoreBest)     : "-");
    m_aArmoryTime [1].SetText(fBestTime         ? PRINT("%.1f %+d", fBestTime, iBestShift) : "-");

    // 
    cMenu::ApplyMedalTexture(&m_aArmoryMedal[0], oProgress.aaaaaiMedalSegment[m_iBaseType][m_iBaseMode][m_iBaseDifficulty][iMissionIndex][iSegmentIndex], bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);

    coreUint8 iMaxMedal = MEDAL_NONE;
    ALL_MEDAL_SEGMENT(aaiMedal) {iMaxMedal = MAX(iMaxMedal, aaiMedal[iMissionIndex][iSegmentIndex]); return false;});

    cMenu::ApplyMedalTexture(&m_aArmoryMedalBest[0], iMaxMedal, bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, false);
    

    if(bBoss && (g_pSave->GetHeader().oProgress.aiAdvance[iMissionIndex] <= iSegmentIndex + 1u))
        m_aArmoryTitle[0].SetTextLanguage("UNKNOWN");
    
    
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
            HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 0u),
            HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 1u),
            HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 2u),
            (oProgress.aiAdvance[iMissionIndex] > iSegmentIndex + 1u)
        };
        STATIC_ASSERT(ARRAY_SIZE(abState) >= MENU_GAME_ARMORY_BADGES);

        // 
        for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
        {
            const coreBool bState   = abState[i];
            const coreBool bVisible = (iMissionIndex == MISSION_INTRO) ? ((i != 0u) || abState[0]) : (((i == 0u) && (abState[0] || abState[3])) || ((i == 1u) && (abState[0] || abState[1])) || ((i == 2u) && ((abState[0] && abState[1]) || abState[2])));

            m_aArmoryBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
            m_aArmoryBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));

            m_aArmoryBadgeWave[i].SetEnabled(bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            const coreChar* pcString = Core::Language->GetString(bVisible ? PRINT("BADGE_%02d_%02zu_%02zu_DESC", oData.iID, iSegmentIndex + 1u, i + 1u) : "UNKNOWN");
            const coreChar* pcBreak  = std::strchr(pcString, '#');

            if(pcBreak)
            {
                m_aArmoryBadgeDesc [i]   .SetText("");
                m_aArmoryBadgeDesc2[i][0].SetText(pcString, pcBreak - pcString);
                m_aArmoryBadgeDesc2[i][1].SetText(pcBreak + 1u);
            }
            else
            {
                m_aArmoryBadgeDesc [i]   .SetText(pcString);
                m_aArmoryBadgeDesc2[i][0].SetText("");
                m_aArmoryBadgeDesc2[i][1].SetText("");
            }
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
    m_aArmoryHelperBack .SetEnabled(bBoss ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 1u;                      i < MENU_GAME_ARMORY_MEDALS;     ++i) m_aArmoryMedal    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 1u;                      i < MENU_GAME_ARMORY_MEDALS;     ++i) m_aArmoryMedal    [i].SetFocusable(false);
    for(coreUintW i = 1u;                      i < MENU_GAME_ARMORY_MEDALS;     ++i) m_aArmoryMedalBest[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 1u;                      i < MENU_GAME_ARMORY_MEDALS;     ++i) m_aArmoryMedalBack2[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_BADGES_ALL; ++i) m_aArmoryBadge    [i].SetEnabled((i < MENU_GAME_ARMORY_BADGES && !bBoss) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = MENU_GAME_ARMORY_BADGES; i < MENU_GAME_ARMORY_BADGES_ALL; ++i) m_aArmoryBadgeWave[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_BADGES;     ++i) m_aArmoryBadgeBack[i].SetEnabled(bBoss ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_BADGES;     ++i) m_aArmoryBadgeDesc[i].SetEnabled(bBoss ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_BADGES;     ++i) for(coreUintW j = 0u; j < ARRAY_SIZE(m_aArmoryBadgeDesc2[0]); ++j) m_aArmoryBadgeDesc2[i][j].SetEnabled(bBoss ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_HELPERS;    ++i) m_aArmoryHelper   [i].SetEnabled(bBoss ? CORE_OBJECT_ENABLE_ALL     : CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_ICONS;      ++i) m_aArmoryIcon     [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_ICONS;      ++i) m_aArmoryIconBack [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < MENU_GAME_ARMORY_ARROWS;     ++i) m_aArmoryArrow    [i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    for(coreUintW i = 0u;                      i < ARRAY_SIZE(m_aArmoryTitle);  ++i) m_aArmoryTitle    [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < ARRAY_SIZE(m_aArmoryScore);  ++i) m_aArmoryScore    [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    for(coreUintW i = 0u;                      i < ARRAY_SIZE(m_aArmoryTime);   ++i) m_aArmoryTime     [i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    m_aArmoryStartArrow[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_aArmoryStartArrow[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryStartLabel.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_ArmoryStartBack.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    
    

    // 
    if(!m_aArmoryBadgeDesc[1].GetText()[0] && !m_aArmoryBadgeDesc2[1][0].GetText()[0])
    {
        //m_aArmoryBadge    [1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        //m_aArmoryBadgeBack[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

        //m_aArmoryBadgeBack[0].SetPosition(m_ArmoryArea2.GetPosition() + coreVector2(0.01f - 0.39f, 0.0f));
    }
    else
    {
        //m_aArmoryBadgeBack[0].SetPosition(m_ArmoryArea2.GetPosition() + coreVector2(0.01f - 0.39f, 0.0f + (I_TO_F(0) - 0.5f) * -0.06f));
    }

    //m_aArmoryBadgeDesc[0].SetPosition(m_aArmoryBadgeBack[0].GetPosition() + coreVector2(0.05f,0.0f));
    //for(coreUintW j = 0u; j < ARRAY_SIZE(m_aArmoryBadgeDesc2[0]); ++j)
    //{
    //    m_aArmoryBadgeDesc2[0][j].SetPosition(m_aArmoryBadgeBack[0].GetPosition() + coreVector2(0.05f, j ? -0.015f : 0.015f));
    //}
    //
    //m_aArmoryBadgeBack[0].Move();
    //m_aArmoryBadgeDesc[0].Move();
    //for(coreUintW j = 0u; j < ARRAY_SIZE(m_aArmoryBadgeDesc2[0]); ++j)
    //{
    //    m_aArmoryBadgeDesc2[0][j].Move();
    //}
    
    
    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_HELPERS; ++i)
    {
        constexpr coreUintW aiOrder[] = {3u, 5u, 7u, 4u, 0u, 6u, 2u, 1u};
        STATIC_ASSERT(ARRAY_SIZE(aiOrder) == MENU_GAME_ARMORY_HELPERS)

        const coreBool bState = HAS_BIT(g_pSave->GetHeader().oProgress.aiHelper[iMissionIndex], i) || ((iMissionIndex == MISSION_ATER) && g_pSave->GetHeader().oProgress.aiFragment[aiOrder[i]]);

        if(bBoss && bState)
        {
            coreVector3 vColor;
            switch(i)
            {
            default: UNREACHABLE
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
    
    
    m_aArmoryTitle[0].SetPosition (m_ArmoryArea.GetPosition() + coreVector2(0.0f,0.105f));
    m_aArmoryTitle[1].SetPosition (m_aArmoryTitle[0].GetPosition() + coreVector2(0.0f,0.044f));
    m_ArmoryIconBig.SetPosition  (m_aArmoryTitle[0].GetPosition() + coreVector2(0.0f,0.018f));
    

    for(coreUintW i = 0u; i < MENU_GAME_ARMORY_BADGES; ++i)
    {
        m_aArmoryBadge[i].SetPosition(m_aArmoryBadgeBack[i].GetPosition());
        m_aArmoryBadge[i].SetSize    (coreVector2(0.063f,0.063f));
        m_aArmoryBadge[i].Move();

        m_aArmoryBadgeWave[i].SetPosition(m_aArmoryBadge[i].GetPosition());
        m_aArmoryBadgeWave[i].SetSize    (m_aArmoryBadge[i].GetSize() * 0.7f);
        m_aArmoryBadgeWave[i].Move();
    }

    m_ArmoryFragment.SetPosition(m_aArmoryHelperBack.GetPosition() + g_aFragmentData[iMissionIndex].vShift);
    m_ArmoryFragment.Move();

    m_ArmoryFragmentBack.SetPosition(m_aArmoryHelperBack.GetPosition());
    m_ArmoryFragmentBack.Move();

    
    
    m_ArmorySelection.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryTile.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    m_ArmoryBack.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    
    const coreBool bAchieved = HAS_BIT(oProgress.aaiBadge[iMissionIndex][iSegmentIndex], 3u);
    m_ArmoryTrophy.SetEnabled(bAchieved ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    
    
    const coreVector2 vSize = (bBoss ? coreVector2(0.1f,0.05f) : coreVector2(0.05f,0.05f)) * 1.8f;

    m_ArmoryTile.DefineTexture   (0u, m_aaSegmentTile[iMissionIndex][iSegmentIndex].GetTexture(0u));
    m_ArmoryTile.SetPosition     (m_ArmoryArea2.GetPosition() + coreVector2(0.395f - vSize.x * 0.5f,0.0f));
    m_ArmoryTile.SetSize         (vSize);
    m_ArmoryTile.SetColor3       (oData.vColor);
    m_ArmoryTile.SetTexSize      (m_aaSegmentTile[iMissionIndex][iSegmentIndex].GetTexSize());
    m_ArmoryTile.SetTexOffset    (m_aaSegmentTile[iMissionIndex][iSegmentIndex].GetTexOffset());
    m_ArmoryTile.Move();

    m_ArmoryBack.SetPosition  (m_ArmoryTile.GetPosition());
    m_ArmoryBack.SetSize      (m_ArmoryTile.GetSize() + coreVector2(0.01f,0.01f));
    m_ArmoryBack.Move();
    
    
    m_ArmoryTrophy.SetPosition(m_ArmoryTile.GetPosition() + m_ArmoryTile.GetSize()*coreVector2(0.5f,-0.5f));
    m_ArmoryTrophy.Move();
    
    
    m_NavigatorArmory.SetShoulder(true);
}


// ****************************************************************
// 
coreBool cGameMenu::__SetupInput(const coreBool bAlways)
{
    return false;
}