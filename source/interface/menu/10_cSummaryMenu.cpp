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
cSummaryMenu::cSummaryMenu()noexcept
: coreMenu           (SURFACE_SUMMARY_MAX, SURFACE_SUMMARY_MISSION_SOLO)
, m_iFinalValue      (0u)
, m_aiFinalPart      {}
, m_aiApplyBonus     {}
, m_aiApplyMedal     {}
, m_fOtherTime       (0.0f)
, m_iOtherShift      (0)
, m_iOtherNumMission (0u)
, m_iOtherNumMedal   (0u)
, m_iOtherStart      (0xFFu)
, m_iSelection       (0u)
, m_afSignalTime     {}
, m_iSignalActive    (0u)
, m_fIntroTimer      (0.0f)
, m_fOutroTimer      (0.0f)
, m_fTimeShift       (0.0f)
, m_fFinalSpinOld    (0.0f)
, m_eState           (SUMMARY_INTRO)
, m_bHasReplay       (false)
{
    // create menu objects
    m_BackgroundMain.DefineTexture(0u, "menu_detail_04.png");
    m_BackgroundMain.DefineTexture(1u, "menu_background_black.png");
    m_BackgroundMain.DefineProgram("menu_animate_program");
    m_BackgroundMain.SetDirection (coreVector2(-1.0f,0.0f));

    m_BackgroundCoop.DefineTexture(0u, "menu_detail_04.png");
    m_BackgroundCoop.DefineTexture(1u, "menu_background_black.png");
    m_BackgroundCoop.DefineProgram("menu_animate_program");

    m_NextButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_NextButton.DefineProgram("menu_border_program");
    m_NextButton.SetPosition  (coreVector2(0.0f,-0.22f));
    m_NextButton.SetSize      (coreVector2(0.4f,0.07f));
    m_NextButton.GetCaption()->SetTextLanguage("NEXT");

    m_AgainButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_AgainButton.DefineProgram("menu_border_program");
    m_AgainButton.SetPosition  (m_NextButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_AgainButton.SetSize      (m_NextButton.GetSize());
    m_AgainButton.GetCaption()->SetTextLanguage("AGAIN");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_AgainButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_NextButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    m_aTitle[0].Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aTitle[0].SetPosition (coreVector2(0.0f,0.0f));
    m_aTitle[0].SetAlignment(coreVector2(-1.0f,0.0f));
    m_aTitle[0].SetColor3   (COLOR_MENU_INSIDE);
    m_aTitle[0].SetText     ("EIGEN");

    m_aTitle[1].Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aTitle[1].SetPosition (coreVector2(0.0f,0.0f));
    m_aTitle[1].SetAlignment(coreVector2(1.0f,0.0f));
    m_aTitle[1].SetColor3   (COLOR_MENU_INSIDE * 0.6f);
    m_aTitle[1].SetText     ("GRAU");
    
    
    
    
    m_ArcadeLayer.DefineTexture(0u, "menu_background_black.png");
    m_ArcadeLayer.DefineProgram("menu_grey_vignette_program");
    m_ArcadeLayer.SetColor3    (coreVector3(0.6f,0.6f,0.6f));
    m_ArcadeLayer.SetStyle     (CORE_OBJECT2D_STYLE_VIEWDIR);

    m_ArcadeHeader.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_ArcadeHeader.SetPosition    (coreVector2(0.0f,0.43f));
    m_ArcadeHeader.SetColor3      (COLOR_MENU_WHITE);
    m_ArcadeHeader.SetTextLanguage("SUMMARY_ARCADE");

    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i)
    {
        const coreFloat fHeight = 0.26f - 0.06f*I_TO_F(i);

        m_aArcadeName[i].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aArcadeName[i].SetPosition (coreVector2(-0.45f, fHeight));
        m_aArcadeName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aArcadeName[i].SetColor3   (g_aMissionData[i].vColor);
        m_aArcadeName[i].SetText     (g_aMissionData[i].pcName);

        for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS; ++j)
        {
            m_aaArcadeScoreRun[i][j].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
            m_aaArcadeScoreRun[i][j].SetPosition(coreVector2(-0.18f + I_TO_F(j) * 0.18f, fHeight));
        }

        m_aArcadeScore[i].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aArcadeScore[i].SetPosition(coreVector2(-0.21f, fHeight));
        m_aArcadeScore[i].SetColor3  (COLOR_MENU_WHITE);

        m_aArcadeTime[i].Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aArcadeTime[i].SetPosition(coreVector2(-0.04f, fHeight));
        m_aArcadeTime[i].SetColor3  (COLOR_MENU_WHITE);

        m_aArcadeIcon[i].DefineTexture(0u, "menu_helper.png");
        m_aArcadeIcon[i].DefineProgram("menu_helper_program");
        m_aArcadeIcon[i].SetPosition  (m_aArcadeName[i].GetPosition() + coreVector2(0.0f,0.0f));
        m_aArcadeIcon[i].SetSize      (coreVector2(0.06f,0.06f));
        m_aArcadeIcon[i].SetColor3    (g_aMissionData[i].vColor * 0.7f);
        m_aArcadeIcon[i].SetTexSize   (coreVector2(0.25f,0.25f));
        m_aArcadeIcon[i].SetTexOffset (g_aMissionData[i].vIcon);

        m_aArcadeIconBack[i].DefineTexture(0u, "menu_helper.png");
        m_aArcadeIconBack[i].DefineProgram("menu_helper_program");
        m_aArcadeIconBack[i].SetPosition  (m_aArcadeIcon[i].GetPosition ());
        m_aArcadeIconBack[i].SetSize      (m_aArcadeIcon[i].GetSize     () * 1.2f);
        m_aArcadeIconBack[i].SetColor3    (m_aArcadeIcon[i].GetColor3   ());
        m_aArcadeIconBack[i].SetTexSize   (m_aArcadeIcon[i].GetTexSize  ());
        m_aArcadeIconBack[i].SetTexOffset (m_aArcadeIcon[i].GetTexOffset());

        m_aArcadeMedalMission[i].DefineTexture(0u, "menu_medal.png");
        m_aArcadeMedalMission[i].DefineProgram("default_2d_program");
        m_aArcadeMedalMission[i].SetPosition  (coreVector2(0.45f, fHeight));
        m_aArcadeMedalMission[i].SetTexSize   (coreVector2(0.25f,0.25f));

        for(coreUintW j = 0u; j < MENU_SUMMARY_MEDALS; ++j)
        {
            m_aaArcadeMedalSegment[i][j].DefineTexture(0u, "menu_medal.png");
            m_aaArcadeMedalSegment[i][j].DefineProgram("default_2d_program");
            m_aaArcadeMedalSegment[i][j].SetPosition  (coreVector2(0.1f + 0.05f * I_TO_F(j), fHeight));
            m_aaArcadeMedalSegment[i][j].SetTexSize   (coreVector2(0.25f,0.25f));
        }

        m_aArcadeLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aArcadeLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aArcadeLine[i].DefineProgram("menu_inner_program");
        m_aArcadeLine[i].SetPosition  (coreVector2(0.0f, fHeight));
        m_aArcadeLine[i].SetSize      (coreVector2(1.1f, 0.06f));
        m_aArcadeLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }

    for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS; ++j)
    {
        m_aArcadeSum[j].Construct(MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    }

    m_aArcadeTotalName[0].Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aArcadeTotalName[0].SetPosition    (coreVector2(-0.275f,-0.355f));
    m_aArcadeTotalName[0].SetColor3      (COLOR_MENU_WHITE);
    m_aArcadeTotalName[0].SetTextLanguage("SUMMARY_SEGMENT_SCORE");

    m_aArcadeTotalName[1].Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_aArcadeTotalName[1].SetPosition    (coreVector2(0.06f,-0.355f));
    m_aArcadeTotalName[1].SetColor3      (COLOR_MENU_WHITE);
    m_aArcadeTotalName[1].SetTextLanguage("SUMMARY_SEGMENT_TIME");

    m_aArcadeTotalBest[0].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArcadeTotalBest[0].SetPosition(m_aArcadeTotalName[0].GetPosition() + coreVector2(0.0f,-0.09f));
    m_aArcadeTotalBest[0].SetColor3  (COLOR_MENU_WHITE * 0.5f);

    m_aArcadeTotalBest[1].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aArcadeTotalBest[1].SetPosition(m_aArcadeTotalName[1].GetPosition() + coreVector2(0.0f,-0.09f));
    m_aArcadeTotalBest[1].SetColor3  (COLOR_MENU_WHITE * 0.5f);

    m_ArcadeTotalScore.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_ArcadeTotalScore.SetPosition(m_aArcadeTotalName[0].GetPosition() + coreVector2(0.0f,-0.05f));
    m_ArcadeTotalScore.SetColor3  (COLOR_MENU_WHITE);

    m_ArcadeTotalTime.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_ArcadeTotalTime.SetPosition(m_aArcadeTotalName[1].GetPosition() + coreVector2(0.0f,-0.05f));
    m_ArcadeTotalTime.SetColor3  (COLOR_MENU_WHITE);

    m_ArcadeOptions.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ArcadeOptions.SetPosition(m_ArcadeHeader.GetPosition() + coreVector2(0.0f,-0.05f));
    m_ArcadeOptions.SetColor3  (COLOR_MENU_WHITE);

    m_ArcadeTotalMedal.DefineTexture(0u, "menu_medal.png");
    m_ArcadeTotalMedal.DefineProgram("default_2d_program");
    m_ArcadeTotalMedal.SetPosition  (coreVector2(0.34f,-0.394f));
    m_ArcadeTotalMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_SUMMARY_RUNS; ++i)
    {
        m_aContinueImage[i].DefineTexture(0u, g_pSpecialEffects->GetIconTexture(0u));
        m_aContinueImage[i].DefineProgram("default_2d_program");
        m_aContinueImage[i].SetPosition  (m_aaArcadeScoreRun[0][i].GetPosition() + coreVector2(0.0f,0.065f));
        m_aContinueImage[i].SetSize      (coreVector2(1.0f,1.0f) * 0.058f);
        m_aContinueImage[i].SetTexSize   (ICON_TEXSIZE);
        m_aContinueImage[i].SetTexOffset (ICON_TEXOFFSET * I_TO_F(i + 1u));
    }
    

    m_aHeader[0].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_aHeader[0].SetPosition(coreVector2(0.0f,0.4f));
    m_aHeader[0].SetColor3  (COLOR_MENU_WHITE);

    m_aHeader[1].Construct  (MENU_FONT_STANDARD_6, MENU_OUTLINE_SMALL);
    m_aHeader[1].SetPosition(coreVector2(0.0f, m_aHeader[0].GetPosition().y - 0.05f));
    m_aHeader[1].SetColor3  (COLOR_MENU_WHITE);

    m_Icon.DefineTexture(0u, "menu_helper.png");
    m_Icon.DefineProgram("menu_helper_program");
    m_Icon.SetPosition  (coreVector2(0.0f, m_aHeader[1].GetPosition().y + 0.02f));
    m_Icon.SetDirection (coreVector2(1.0f,1.0f).Normalized());
    m_Icon.SetTexSize   (coreVector2(0.25f,0.25f));

    m_MedalMission.DefineTexture(0u, "menu_medal.png");
    m_MedalMission.DefineProgram("default_2d_program");
    m_MedalMission.SetPosition  (coreVector2(0.0f,-0.4f));
    m_MedalMission.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_SUMMARY_MEDALS; ++i)
    {
        const coreVector2 vOffset = MISSION_SEGMENT_IS_BOSS(i) ? coreVector2(0.0f,-0.16f) : coreVector2(((i < 3u) ? (I_TO_F(i) - 1.0f) : ((I_TO_F(i - 3u) - 0.5f))) * 0.075f, I_TO_F(i / 3u) * -0.075f);

        m_aMedalSegment[i].DefineTexture(0u, "menu_medal.png");
        m_aMedalSegment[i].DefineProgram("default_2d_program");
        m_aMedalSegment[i].SetPosition  (coreVector2(0.0f, 0.255f) + vOffset);
        m_aMedalSegment[i].SetTexSize   (coreVector2(0.25f,0.25f));
    }

    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i)
    {
        m_aName[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition(coreVector2(0.0f, -0.003f - 0.1f*I_TO_F(i)));
        m_aName[i].SetColor3  (COLOR_MENU_WHITE);

        m_aValue[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aValue[i].SetPosition(coreVector2(0.0f, m_aName[i].GetPosition().y - 0.035f));
        m_aValue[i].SetColor3  (COLOR_MENU_WHITE);

        for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
        {
            const coreFloat fSide = j ? 1.0f : -1.0f;

            m_aaPart[i][j].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
            m_aaPart[i][j].SetPosition(m_aValue[i].GetPosition() + coreVector2(fSide * 0.36f, 0.0f));
            m_aaPart[i][j].SetColor3  (COLOR_MENU_WHITE);
        }
    }
    m_aName[0].SetTextLanguage("BONUS_MEDAL");
    m_aName[1].SetTextLanguage("BONUS_SURVIVE");

    m_TotalName.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_TotalName.SetPosition    (coreVector2(0.0f,-0.217f));
    m_TotalName.SetColor3      (COLOR_MENU_WHITE);
    m_TotalName.SetTextLanguage("SUMMARY_MISSION_TOTAL");

    m_TotalValue.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_TotalValue.SetPosition(coreVector2(0.0f, m_TotalName.GetPosition().y - 0.05f));
    m_TotalValue.SetColor3  (COLOR_MENU_WHITE);

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        const coreFloat fSide = j ? 1.0f : -1.0f;

        m_aTotalPart[j].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aTotalPart[j].SetPosition(m_TotalValue.GetPosition() + coreVector2(fSide * 0.36f, 0.0f));
        m_aTotalPart[j].SetColor3  (COLOR_MENU_WHITE);
    }

    m_TotalBest.Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_TotalBest.SetPosition(coreVector2(0.0f, m_TotalName.GetPosition().y - 0.09f));
    m_TotalBest.SetColor3  (COLOR_MENU_WHITE * 0.5f);

    m_Raise.Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    m_Raise.SetPosition(coreVector2(0.0f,-0.465f));
    m_Raise.SetColor3  (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    
    
    
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i)
    {
        m_aSegmentName[i].Construct  (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
        m_aSegmentName[i].SetPosition(coreVector2(0.0f, 0.245f - 0.16f*I_TO_F(i)));
        m_aSegmentName[i].SetColor3  (COLOR_MENU_WHITE);

        for(coreUintW j = 0u; j < MENU_SUMMARY_SIDES; ++j)
        {
            if(j) m_aaSegmentValue[i][j].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
             else m_aaSegmentValue[i][j].Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
            //m_aaSegmentValue[i][j].SetPosition (coreVector2(m_aSegmentSide[j].GetPosition().x, m_aSegmentName[i].GetPosition().y - 0.035f));
            m_aaSegmentValue[i][j].SetPosition (coreVector2(0.0f, m_aSegmentName[i].GetPosition().y - 0.05f    +(j ? -0.04f : 0.0f)));
            //m_aaSegmentValue[i][j].SetAlignment(m_aSegmentSide[j].GetAlignment());
            m_aaSegmentValue[i][j].SetColor3   (j ? (COLOR_MENU_WHITE * 0.5f) : COLOR_MENU_WHITE);
        }

        for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
        {
            const coreFloat fSide = j ? 1.0f : -1.0f;

            m_aaSegmentPart[i][j].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
            m_aaSegmentPart[i][j].SetPosition(m_aaSegmentValue[i][0].GetPosition() + coreVector2(fSide * 0.36f, 0.0f));
            m_aaSegmentPart[i][j].SetColor3  (COLOR_MENU_WHITE);
        }
    }
    m_aSegmentName[0].SetTextLanguage("SUMMARY_SEGMENT_SCORE");
    m_aSegmentName[1].SetTextLanguage("SUMMARY_SEGMENT_TIME");

    for(coreUintW j = 0u; j < MENU_SUMMARY_SIDES; ++j)
    {
        m_aSegmentShift[j].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aSegmentShift[j].SetPosition(m_aaSegmentValue[1][0].GetPosition() + coreVector2(0.36f,0.0f));
        m_aSegmentShift[j].SetColor3  (COLOR_MENU_WHITE);
    }

    m_SegmentMedal.DefineTexture(0u, "menu_medal.png");
    m_SegmentMedal.DefineProgram("default_2d_program");
    m_SegmentMedal.SetPosition  (coreVector2(0.0f,-0.1f));
    m_SegmentMedal.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_SUMMARY_BADGES; ++i)
    {
        m_aSegmentBadge[i].DefineTexture(0u, "menu_badge.png");
        m_aSegmentBadge[i].DefineProgram("default_2d_program");
        m_aSegmentBadge[i].SetTexSize   (coreVector2(0.5f,1.0f));

        m_aSegmentBadgeWave[i].DefineTexture(0u, "effect_headlight_point.png");
        m_aSegmentBadgeWave[i].DefineProgram("menu_single_program");
    }

    m_SegmentRaise.Construct  (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
    //m_SegmentRaise.SetPosition(m_SegmentMedal.GetPosition() + coreVector2(-0.115f,0.0f));
    m_SegmentRaise.SetPosition(coreVector2(0.0f,-0.465f));
    m_SegmentRaise.SetColor3  (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    
    
    

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aPerfect); ++i)
    {
        m_aPerfect[i].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aPerfect[i].SetColor3      (COLOR_MENU_WHITE);
        m_aPerfect[i].SetTextLanguage("SUMMARY_PERFECT");
        m_aPerfect[i].SetRectify     (false);
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRecord); ++i)
    {
        m_aRecord[i].Construct      (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aRecord[i].SetColor3      (COLOR_MENU_WHITE);
        m_aRecord[i].SetTextLanguage("SUMMARY_RECORD");
        m_aRecord[i].SetRectify     (false);
    }
    m_aRecord[0].SetPosition(m_aaSegmentValue[0][1].GetPosition());
    m_aRecord[1].SetPosition(m_aaSegmentValue[1][1].GetPosition());
    m_aRecord[3].SetPosition(m_TotalBest           .GetPosition());
    m_aRecord[4].SetPosition(m_aArcadeTotalBest[0] .GetPosition());
    m_aRecord[5].SetPosition(m_aArcadeTotalBest[1] .GetPosition());
    

    // 
    m_Navigator.BindObject(NULL,           &m_NextButton,  NULL, &m_NextButton,  NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_NextButton,  &m_ExitButton,  NULL, &m_AgainButton, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_AgainButton, &m_NextButton,  NULL, &m_ExitButton,  NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ExitButton,  &m_AgainButton, NULL, &m_NextButton,  NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignMenu(this);
    m_Navigator.ShowIcon  (true);

    // bind menu objects
    for(coreUintW k = SURFACE_SUMMARY_ARCADE_1; k <= SURFACE_SUMMARY_ARCADE_2; ++k)
    {
        this->BindObject(k, &m_ArcadeLayer);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) this->BindObject(k, &m_aArcadeLine    [i]);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) this->BindObject(k, &m_aArcadeIconBack[i]);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) this->BindObject(k, &m_aArcadeIcon    [i]);
        this->BindObject(k, &m_ArcadeTotalMedal);
    }

    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) this->BindObject(SURFACE_SUMMARY_ARCADE_1, &m_aArcadeMedalMission[i]);
    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) for(coreUintW j = 0u; j < MENU_SUMMARY_MEDALS; ++j) this->BindObject(SURFACE_SUMMARY_ARCADE_1, &m_aaArcadeMedalSegment[i][j]);
    for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS;    ++j) this->BindObject(SURFACE_SUMMARY_ARCADE_2, &m_aContinueImage[j]);
    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS;   ++j) this->BindObject(SURFACE_SUMMARY_ARCADE_2, &m_aaArcadeScoreRun    [i][j]);
    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) this->BindObject(SURFACE_SUMMARY_ARCADE_1, &m_aArcadeScore[i]);
    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) this->BindObject(SURFACE_SUMMARY_ARCADE_1, &m_aArcadeTime [i]);
    for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS;    ++j) this->BindObject(SURFACE_SUMMARY_ARCADE_2, &m_aArcadeSum  [j]);

    for(coreUintW k = SURFACE_SUMMARY_ARCADE_1; k <= SURFACE_SUMMARY_ARCADE_2; ++k)
    {
        this->BindObject(k, &m_ArcadeHeader);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i) this->BindObject(k, &m_aArcadeName[i]);
        this->BindObject(k, &m_aArcadeTotalName[0]);
        this->BindObject(k, &m_aArcadeTotalName[1]);
        this->BindObject(k, &m_aArcadeTotalBest[0]);
        this->BindObject(k, &m_aArcadeTotalBest[1]);
        this->BindObject(k, &m_ArcadeTotalScore);
        this->BindObject(k, &m_ArcadeTotalTime);
        this->BindObject(k, &m_ArcadeOptions);
        this->BindObject(k, &m_aRecord[4]);
        this->BindObject(k, &m_aRecord[5]);
    }

    this->BindObject(SURFACE_SUMMARY_MISSION_COOP, &m_BackgroundCoop);

    for(coreUintW j = SURFACE_SUMMARY_MISSION_SOLO; j <= SURFACE_SUMMARY_MISSION_COOP; ++j)
    {
        this->BindObject(j, &m_BackgroundMain);
        this->BindObject(j, &m_Icon);
        this->BindObject(j, &m_MedalMission);
        for(coreUintW i = 0u; i < MENU_SUMMARY_MEDALS; ++i) this->BindObject(j, &m_aMedalSegment[i]);

        this->BindObject(j, &m_aHeader[0]);
        this->BindObject(j, &m_aHeader[1]);

        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(j, &m_aName[i]);
        this->BindObject(j, &m_TotalName);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(j, &m_aValue[i]);
        this->BindObject(j, &m_TotalValue);
        this->BindObject(j, &m_TotalBest);
        //this->BindObject(j, &m_Raise);

        this->BindObject(j, &m_aPerfect[0]);
        this->BindObject(j, &m_aPerfect[1]);
        this->BindObject(j, &m_aRecord [3]);
    }

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_MISSION_COOP, &m_aaPart[i][j]);
        this->BindObject(SURFACE_SUMMARY_MISSION_COOP, &m_aTotalPart[j]);
    }

    this->BindObject(SURFACE_SUMMARY_SEGMENT_COOP, &m_BackgroundCoop);

    for(coreUintW j = SURFACE_SUMMARY_SEGMENT_SOLO; j <= SURFACE_SUMMARY_SEGMENT_COOP; ++j)
    {
        this->BindObject(j, &m_BackgroundMain);
        this->BindObject(j, &m_Icon);
        this->BindObject(j, &m_SegmentMedal);

        for(coreUintW i = 0u; i < MENU_SUMMARY_BADGES; ++i) this->BindObject(j, &m_aSegmentBadgeWave[i]);
        for(coreUintW i = 0u; i < MENU_SUMMARY_BADGES; ++i) this->BindObject(j, &m_aSegmentBadge    [i]);

        this->BindObject(j, &m_NextButton);
        this->BindObject(j, &m_AgainButton);
        this->BindObject(j, &m_ExitButton);

        this->BindObject(j, &m_aHeader[0]);
        this->BindObject(j, &m_aHeader[1]);

        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(j, &m_aSegmentName  [i]);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(j, &m_aaSegmentValue[i][0]);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(j, &m_aaSegmentValue[i][1]);
        //for(coreUintW i = 0u; i < MENU_SUMMARY_SIDES;           ++i) this->BindObject(j, &m_aSegmentShift [i]);
        //this->BindObject(j, &m_SegmentRaise);
        
        this->BindObject(j, &m_aRecord[0]);
        this->BindObject(j, &m_aRecord[1]);
        //this->BindObject(j, &m_aRecord[2]);

        this->BindObject(j, &m_Navigator);
    }

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(SURFACE_SUMMARY_SEGMENT_COOP, &m_aaSegmentPart[i][j]);
    }

    this->BindObject(SURFACE_SUMMARY_TITLE, &m_aTitle[0]);
    this->BindObject(SURFACE_SUMMARY_TITLE, &m_aTitle[1]);

    STATIC_ASSERT(ARRAY_SIZE(m_aPerfect) + ARRAY_SIZE(m_aRecord) <= BITSOF(m_iSignalActive))
}


// ****************************************************************
// render the summary menu
void cSummaryMenu::Render()
{
    if((this->GetCurSurface() == SURFACE_SUMMARY_ARCADE_1) ||
       (this->GetCurSurface() == SURFACE_SUMMARY_ARCADE_2))
    {
        // 
        const coreVector2 vCorner = coreVector2(0.5f,0.5f) * (g_vGameResolution * Core::Graphics->GetViewResolution().zw());
        Core::Graphics->StartScissorTest(-vCorner, vCorner);
        {
            // 
            this->coreMenu::Render();
        }
        Core::Graphics->EndScissorTest();
    }
    else
    {
        // 
        cMenu::UpdateAnimateProgram(&m_BackgroundMain);
        ASSERT(m_BackgroundMain.GetSize() == m_BackgroundCoop.GetSize())

        // 
        this->coreMenu::Render();
    }
}


// ****************************************************************
// move the summary menu
void cSummaryMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_SUMMARY_ARCADE_1:
    case SURFACE_SUMMARY_ARCADE_2:
        {
            constexpr coreFloat fSpinFrom = 1.0f;
            
            cReplayMenu::HandleSaveError([this](const coreUint8 iAnswer)
            {
                if(iAnswer == MSGBOX_ANSWER_YES)
                {
                    if(!g_pReplay->SaveFile(0u)) return;
                }

                g_pReplay->ResetStatus();
                m_eState = SUMMARY_OUTRO;
            });
            
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= fSpinFrom + 0.1f * I_TO_F(m_iOtherNumMission) + 0.05f * I_TO_F(m_iOtherNumMedal) + 1.45f + 1.0f) && (m_eState != SUMMARY_OUTRO) && (m_eState != SUMMARY_WAIT) && g_MenuInput.bAny)
            {
                if(!this->GetTransition().GetStatus())
                {
                    if(this->GetCurSurface() == SURFACE_SUMMARY_ARCADE_1)
                    {
                        this->ChangeSurface(SURFACE_SUMMARY_ARCADE_2, 3.0f);
                        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SUB_OUT);
                    }
                    else
                    {
                        if(g_MenuInput.bCancel)
                        {
                            this->ChangeSurface(SURFACE_SUMMARY_ARCADE_1, 3.0f);
                            g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_SUB_IN);
                        }
                        else
                        {
                            if(m_bHasReplay)
                            {
                                m_eState = SUMMARY_WAIT;
                                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_REPLAY_SAVE"), [this](const eMsgAnswer eAnswer)
                                {
                                    if(eAnswer == MSGBOX_ANSWER_YES)
                                    {
                                        if(REPLAY_SLOTSYSTEM)
                                        {
                                            m_iStatus = 106;
                                            return;
                                        }
                                        else
                                        {
                                            g_pReplay->SetNameDefault(0u);
                                            if(!g_pReplay->SaveFile(0u)) return;
                                        }
                                    }

                                    m_eState = SUMMARY_OUTRO;
                                });
                            }
                            else
                            {
                                m_eState = SUMMARY_OUTRO;
                                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_MENU_MSGBOX_YES);
                            }
                        }
                    }
                }
            }
            
            
            if(m_eState == SUMMARY_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= 1.0f)
            {
                // 
                m_iStatus = 5;
            }
            
            const coreFloat fBlendIn  = m_fIntroTimer;
            const coreFloat fBlendOut = 1.0f - m_fOutroTimer;
            
            const auto nBlendMedalFunc = [&](cGuiObject* OUTPUT pMedal, const coreFloat fScale, const coreFloat fThreshold, const eSoundEffect eSoundIndex = SOUND_NONE)
            {
                const coreFloat fFadeIn     = CLAMP01((fBlendIn - fThreshold) * 10.0f);
                const coreInt32 iStatusdOld = pMedal->GetStatus();
                const coreInt32 iStatusdNew = (fBlendIn >= fThreshold) ? 1 : 0;

                // 
                pMedal->SetSize (coreVector2(fScale, fScale) * LERP(1.5f, 1.0f, fFadeIn));
                pMedal->SetAlpha(pMedal->GetAlpha() * MIN(fFadeIn, fBlendOut));
                pMedal->SetStatus(iStatusdNew);

                // 
                pMedal->Move();

                // 
                if(pMedal->GetEnabled() && !iStatusdOld && iStatusdNew && (eSoundIndex != SOUND_NONE)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, eSoundIndex);
            };
            
            const auto nAlphaFunc = [&](coreObject2D* OUTPUT pObject, const coreFloat fTime, const eSoundEffect eSoundIndex = SOUND_NONE)
            {
                const coreFloat        fFade       = fTime * fBlendOut;
                const coreObjectEnable eEnabledOld = pObject->GetEnabled();
                const coreObjectEnable eEnabledNew = fFade ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                pObject->SetAlpha  (pObject->GetAlpha() * fFade);
                pObject->SetEnabled(eEnabledNew);

                // 
                if(!eEnabledOld && eEnabledNew && (eSoundIndex != SOUND_NONE)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, eSoundIndex);
            };
            
            nAlphaFunc(&m_ArcadeHeader,  CLAMP01(m_fIntroTimer));
            nAlphaFunc(&m_ArcadeOptions, CLAMP01(m_fIntroTimer));
            
            
            const coreFloat fNum = I_TO_F(m_iOtherNumMission);
            
            for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i)
            {
                const coreFloat fDelay = fSpinFrom + 0.1f * I_TO_F(i);
                const coreFloat fTime  = (i < m_iOtherNumMission) ? CLAMP01((m_fIntroTimer - fDelay) * 10.0f) : 0.0f;

                nAlphaFunc(&m_aArcadeName    [i], fTime);
                for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS; ++j) nAlphaFunc(&m_aaArcadeScoreRun[i][j], fTime);
                nAlphaFunc(&m_aArcadeScore   [i], fTime);
                nAlphaFunc(&m_aArcadeTime    [i], fTime);
                nAlphaFunc(&m_aArcadeIcon    [i], fTime);
                nAlphaFunc(&m_aArcadeIconBack[i], fTime * 0.5f);
                nAlphaFunc(&m_aArcadeLine    [i], fTime, SOUND_MENU_MSGBOX_SHOW);

                const coreUintW iAterOffset  = (i == MISSION_ATER) ? 5u : 0u;   // ignore segment medals
                const coreUintW iAterOffset2 = (i == MISSION_ATER) ? 1u : 0u;   // ignore mission medal

                const coreUintW iFirst = i * (MENU_SUMMARY_MEDALS + 1u);

                const coreUint8 iIndex     = iFirst + MENU_SUMMARY_MEDALS - iAterOffset;
                const coreFloat fThreshold = ((iIndex < m_iOtherStart + m_iOtherNumMedal) && (iIndex >= m_iOtherStart)) ? (fSpinFrom + 0.1f * fNum + 0.05f * I_TO_F(MENU_SUMMARY_MEDALS + iFirst - m_iOtherStart - iAterOffset)) : 10000.0f;
                nBlendMedalFunc(&m_aArcadeMedalMission[i], 0.07f, fThreshold, SOUND_SUMMARY_MEDAL);

                for(coreUintW j = 0u; j < MENU_SUMMARY_MEDALS; ++j)
                {
                    const coreUint8 iIndex2     = iFirst + j + iAterOffset2;
                    const coreFloat fThreshold2 = ((iIndex2 < m_iOtherStart + m_iOtherNumMedal) && (iIndex2 >= m_iOtherStart)) ? (fSpinFrom + 0.1f * fNum + 0.05f * I_TO_F(j + iFirst - m_iOtherStart)) : 10000.0f;
                    nBlendMedalFunc(&m_aaArcadeMedalSegment[i][j], 0.05f, fThreshold2, SOUND_SUMMARY_MEDAL);
                }
            }
            
            const coreFloat fDelay     = fSpinFrom + 0.1f * fNum + 0.05f * I_TO_F(m_iOtherNumMedal);
            const coreFloat fFinalSpin = CLAMP01((fBlendIn - fDelay) * (1.0f/1.35f));
            m_ArcadeTotalScore.SetText(PRINT("%.0f",       I_TO_F(m_iFinalValue) * fFinalSpin));
            m_ArcadeTotalTime .SetText(PRINT("%.1f %+.0f", m_fOtherTime * fFinalSpin, I_TO_F(m_iOtherShift) * fFinalSpin));
            
            
            const coreFloat fTickStep = 1.35f * (TIME ? RCP(ROUND(RCP(20.0f * TIME)) * TIME) : 20.0f);
            if(F_TO_UI(m_fFinalSpinOld * fTickStep) < F_TO_UI(fFinalSpin * fTickStep)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_SUMMARY_SCORE);
            m_fFinalSpinOld = fFinalSpin;
            

            for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS; ++j) nAlphaFunc(&m_aArcadeSum[j], fFinalSpin ? 1.0f : 0.0f);
            
            nAlphaFunc(&m_aArcadeTotalName[0], fFinalSpin ? 1.0f : 0.0f, SOUND_SUMMARY_TEXT);
            nAlphaFunc(&m_aArcadeTotalName[1], fFinalSpin ? 1.0f : 0.0f);
            nAlphaFunc(&m_ArcadeTotalScore,    fFinalSpin ? 1.0f : 0.0f);
            nAlphaFunc(&m_ArcadeTotalTime,     fFinalSpin ? 1.0f : 0.0f);

            nAlphaFunc(&m_ArcadeLayer, CLAMP01(m_fIntroTimer) * 0.5f);
            
            
            m_aArcadeTotalBest[0].SetEnabled((m_ArcadeTotalScore.IsEnabled(CORE_OBJECT_ENABLE_ALL) && (fFinalSpin >= 1.0f)) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aArcadeTotalBest[0].SetAlpha  (m_ArcadeTotalScore.GetAlpha());
            m_aArcadeTotalBest[1].SetEnabled((m_ArcadeTotalTime.IsEnabled(CORE_OBJECT_ENABLE_ALL) && (fFinalSpin >= 1.0f)) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aArcadeTotalBest[1].SetAlpha  (m_ArcadeTotalTime.GetAlpha());
            
            
            const auto nSignalFunc = [&](cGuiLabel* OUTPUT pSignal, const coreObject2D* pParent, const coreBool bCondition)
            {
                const coreObjectEnable eEnabledOld = pSignal->GetEnabled();
                const coreObjectEnable eEnabledNew = (pParent->IsEnabled(CORE_OBJECT_ENABLE_ALL) && bCondition) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                // 
                pSignal->SetEnabled(eEnabledNew);
                pSignal->SetAlpha  (pParent->GetAlpha());

                // 
                if(!eEnabledOld && eEnabledNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_SUMMARY_RECORD);
            };
            nSignalFunc(&m_aRecord[4], &m_aArcadeTotalBest[0], HAS_BIT(m_iSignalActive, 6u));
            nSignalFunc(&m_aRecord[5], &m_aArcadeTotalBest[1], HAS_BIT(m_iSignalActive, 7u));

            // 
            const coreFloat fThreshold3 = fDelay + 1.45f;
            nBlendMedalFunc(&m_ArcadeTotalMedal, 0.13f, fThreshold3, SPECIAL_SOUND_MEDAL(m_aiApplyMedal[0]));

            // 
            m_ArcadeLayer.SetSize     (Core::System->GetCanonSize());
            m_ArcadeLayer.SetTexSize  (Core::System->GetCanonSize() * 1.2f);
            m_ArcadeLayer.SetTexOffset(MENU_LAYER_TEXOFFSET);
            
            
            
            const coreFloat fRotation1 = Core::System->GetTotalTimeFloat(20.0);
            const coreFloat fRotation2 = Core::System->GetTotalTimeFloat(2.0*PI_D);

            for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i)
            {
                m_aArcadeIcon    [i].SetDirection(coreVector2::Direction(fRotation1 *  (0.2f*PI) + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_SUMMARY_ARCADES))));
                m_aArcadeIcon    [i].Move();
                m_aArcadeIconBack[i].SetDirection(coreVector2::Direction(fRotation1 * (-0.1f*PI) + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_SUMMARY_ARCADES))));
                m_aArcadeIconBack[i].Move();
            }
            
    
            for(coreUintW i = 0u; i < MENU_SUMMARY_RUNS; ++i)
            {
                m_aContinueImage[i].SetDirection(coreVector2::Direction(fRotation2 + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_SUMMARY_RUNS))));
                m_aContinueImage[i].SetAlpha    (m_aContinueImage[i].GetAlpha() * fBlendOut);
            }
        }
        break;

    case SURFACE_SUMMARY_MISSION_SOLO:
    case SURFACE_SUMMARY_MISSION_COOP:
        {
            const eSummaryState eOld = m_eState;
                
            // 
            constexpr coreFloat fSpinFrom = (2.5f + 0.8f * I_TO_F(MENU_SUMMARY_ENTRIES));
            constexpr coreFloat fSpinTo   = fSpinFrom + 1.5f;

            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= 1.0f /*MENU_SUMMARY_BANNER_SPEED_REV*/) && g_MenuInput.bAny)
            {
                // 
                     if(m_eState      >= SUMMARY_SKIPPED) m_eState = SUMMARY_OUTRO;     // leave summary
                else if(m_fIntroTimer >= fSpinTo)         m_eState = SUMMARY_OUTRO;
                else if(m_fIntroTimer <  fSpinTo)         m_eState = SUMMARY_SKIPPED;   // skip blend-in
                
                if((g_pGame->GetKind() == GAME_KIND_ALL) && (eOld != SUMMARY_OUTRO) && (m_eState == SUMMARY_OUTRO)) 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
            }
            
            
            if((eOld != SUMMARY_SKIPPED) && (m_eState == SUMMARY_SKIPPED)) m_fTimeShift = MAX0(MENU_SUMMARY_SKIP_MISSION - m_fIntroTimer);

            // 
            if(m_eState == SUMMARY_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= MENU_SUMMARY_BANNER_SPEED_REV)
            {
                // 
                m_iStatus = 1;
            }

            // 
            if(m_eState == SUMMARY_OUTRO)
            {
                this->__ApplyDeferred();
            }

            if(m_eState != SUMMARY_WAIT)
            {
                // 
                if((m_fIntroTimer >= MENU_SUMMARY_BANNER_ANIMATION) && (m_eState < SUMMARY_WAIT)) m_eState = SUMMARY_WAIT;
                STATIC_ASSERT(MENU_SUMMARY_BANNER_ANIMATION >= fSpinTo)

                // 
                const coreFloat fBlendIn  = m_fIntroTimer + m_fTimeShift;
                const coreFloat fBlendOut = 1.0f - m_fOutroTimer * MENU_SUMMARY_BANNER_SPEED;

                // (# enable-state should not be changed) 
                const auto nBlendMedalFunc = [&](cGuiObject* OUTPUT pMedal, const coreFloat fScale, const coreFloat fThreshold, const coreBool bMission)
                {
                    const coreFloat fFadeIn     = CLAMP01((fBlendIn - fThreshold) * 10.0f);
                    const coreInt32 iStatusdOld = pMedal->GetStatus();
                    const coreInt32 iStatusdNew = (fBlendIn >= fThreshold) ? 1 : 0;

                    // 
                    pMedal->SetSize  (coreVector2(fScale, fScale) * LERP(1.5f, 1.0f, fFadeIn));
                    pMedal->SetAlpha (MIN(fFadeIn, fBlendOut));
                    pMedal->SetStatus(iStatusdNew);

                    // 
                    pMedal->Move();

                    // 
                    if(pMedal->GetEnabled() && !iStatusdOld && iStatusdNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, bMission ? SPECIAL_SOUND_MEDAL(m_aiApplyMedal[0]) : SOUND_SUMMARY_MEDAL);
                };
                nBlendMedalFunc(&m_MedalMission, 0.13f, MENU_SUMMARY_SKIP_MISSION, true);
                for(coreUintW i = 0u; i < MENU_SUMMARY_MEDALS; ++i) nBlendMedalFunc(&m_aMedalSegment[i], MISSION_SEGMENT_IS_BOSS(i) ? 0.09f : 0.07f, 0.8f + 0.1f * I_TO_F(i), false);

                // 
                const auto nBlendLabelFunc = [&](cGuiLabel* OUTPUT pName, cGuiLabel* OUTPUT pValue, cGuiLabel* OUTPUT pPart, const coreFloat fThreshold)
                {
                    const coreObjectEnable eEnabledOld = pName->GetEnabled();
                    const coreObjectEnable eEnabledNew = (fBlendIn >= fThreshold) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                    // blend-in
                    pName ->SetEnabled(eEnabledNew);
                    pValue->SetEnabled(eEnabledNew);
                    for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) pPart[i].SetEnabled(eEnabledNew);

                    // blend-out
                    pName ->SetAlpha(fBlendOut);
                    pValue->SetAlpha(fBlendOut);
                    for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) pPart[i].SetAlpha(fBlendOut);

                    // 
                    if(!eEnabledOld && eEnabledNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_SUMMARY_TEXT);
                };
                for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) nBlendLabelFunc(&m_aName[i], &m_aValue[i], m_aaPart[i], 2.5f + 0.8f * I_TO_F(i));
                nBlendLabelFunc(&m_TotalName, &m_TotalValue, m_aTotalPart, fSpinFrom);

                // spin-up final score value
                const coreFloat fFinalSpin = CLAMP01((fBlendIn - fSpinFrom) * (1.0f/1.35f));
                m_TotalValue.SetText(PRINT("%.0f", I_TO_F(m_iFinalValue) * fFinalSpin));
                for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) m_aTotalPart[i].SetText(PRINT("%.0f", I_TO_F(m_aiFinalPart[i]) * fFinalSpin));
                
                m_TotalBest.SetEnabled((m_TotalValue.IsEnabled(CORE_OBJECT_ENABLE_ALL) && (fFinalSpin >= 1.0f)) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_TotalBest.SetAlpha  (m_TotalValue.GetAlpha());

                // 
                const coreFloat fTickStep = 1.35f * (TIME ? RCP(ROUND(RCP(20.0f * TIME)) * TIME) : 20.0f);
                if(F_TO_UI(m_fFinalSpinOld * fTickStep) < F_TO_UI(fFinalSpin * fTickStep)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_SUMMARY_SCORE);
                m_fFinalSpinOld = fFinalSpin;

                // 
                const auto nSignalFunc = [&](cGuiLabel* OUTPUT pSignal, const coreObject2D* pParent, const coreBool bCondition, const eSoundEffect eSoundIndex)
                {
                    const coreObjectEnable eEnabledOld = pSignal->GetEnabled();
                    const coreObjectEnable eEnabledNew = (pParent->IsEnabled(CORE_OBJECT_ENABLE_ALL) && bCondition) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                    // 
                    pSignal->SetEnabled(eEnabledNew);
                    pSignal->SetAlpha  (pParent->GetAlpha());

                    // 
                    if(!eEnabledOld && eEnabledNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, eSoundIndex);
                };
                nSignalFunc(&m_aPerfect[0], &m_aValue[0], HAS_BIT(m_iSignalActive, 0u), SOUND_SUMMARY_PERFECT);
                nSignalFunc(&m_aPerfect[1], &m_aValue[1], HAS_BIT(m_iSignalActive, 1u), SOUND_SUMMARY_PERFECT);
                nSignalFunc(&m_aRecord [3], &m_TotalBest, HAS_BIT(m_iSignalActive, 5u), SOUND_SUMMARY_RECORD);

                // calculate visibility and animation value
                const coreFloat fVisibility = MAX0(MIN(m_fIntroTimer, MENU_SUMMARY_BANNER_SPEED_REV - m_fOutroTimer)) * MENU_SUMMARY_BANNER_SPEED;
                const coreFloat fAnimation  = BLENDB(MIN1(m_fIntroTimer / MENU_SUMMARY_BANNER_ANIMATION)) * MENU_SUMMARY_BANNER_ANIMATION;

                // slash background across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
                const coreBool bLeftRight = m_fOutroTimer ? true : false;
                m_BackgroundMain.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_BackgroundMain.GetDirection().yx());
                m_BackgroundMain.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

                // animate background
                m_BackgroundMain.SetSize     (coreVector2(fVisibility, 1.0f) * coreVector2(1.0f,0.45f));
                m_BackgroundMain.SetTexSize  (coreVector2(fVisibility, 1.0f));
                m_BackgroundMain.SetTexOffset(coreVector2(1.0f,1.0f) * (fAnimation * 0.05f));

                // duplicate background for multiplayer
                m_BackgroundCoop.SetPosition (m_BackgroundMain.GetPosition ().Rotated90() * -1.0f);
                m_BackgroundCoop.SetAlpha    (m_BackgroundMain.GetAlpha    ());
                m_BackgroundCoop.SetSize     (m_BackgroundMain.GetSize     ());
                m_BackgroundCoop.SetTexSize  (m_BackgroundMain.GetTexSize  ());
                m_BackgroundCoop.SetTexOffset(m_BackgroundMain.GetTexOffset());

                // move background manually (to remove some delay)
                m_BackgroundMain.Move();
                m_BackgroundCoop.Move();

                // 
                m_aHeader[0].SetAlpha(fVisibility);
                m_aHeader[1].SetAlpha(fVisibility);
                m_Icon      .SetAlpha(fVisibility);
                
                
                m_Icon.SetSize(coreVector2(0.3f,0.3f) * LERPB(0.85f, 1.0f, fVisibility));
                m_Icon.Move();
            }
            
            
            m_Icon.SetAlpha(m_Icon.GetAlpha() * 0.6f);
            
                
                for(coreUintW i = 0u; i < ARRAY_SIZE(m_aPerfect); ++i)
                {
                    if(m_aPerfect[i].IsEnabled(CORE_OBJECT_ENABLE_ALL))
                    {
                        m_afSignalTime[i].UpdateMin(0.7f, 1.0f);
                        const coreFloat fTime = BLENDB(m_afSignalTime[i]);

                        m_aPerfect[i].SetPosition(m_aValue[i].GetPosition() + coreVector2(0.0f, fTime * 0.05f));
                        m_aPerfect[i].SetAlpha(1.0f - fTime);
                    }
                }
        }
        break;

    case SURFACE_SUMMARY_SEGMENT_SOLO:
    case SURFACE_SUMMARY_SEGMENT_COOP:
        {
            if(m_NextButton.IsClicked())
            {
                // 
                m_iSelection = 3u;
            }
            else if(m_AgainButton.IsClicked())
            {
                // 
                m_iSelection = 4u;
            }
            else if(m_ExitButton.IsClicked())
            {
                // 
                m_iSelection = 1u;
                 
                g_pGame->FadeMusic(0.7f);
            }
            
            
            const eSummaryState eOld = m_eState;
            
            // 
            constexpr coreFloat fSpinFrom = (2.5f + 0.8f * I_TO_F(MENU_SUMMARY_ENTRIES_SEGMENT));
            constexpr coreFloat fSpinTo   = fSpinFrom + 1.5f;
            
            const coreBool bAnyButton = g_MenuInput.bAny || m_Navigator.GetCurObject();
            if(bAnyButton && (m_fIntroTimer < fSpinTo) && m_eState < SUMMARY_SKIPPED) m_eState = SUMMARY_SKIPPED;

            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= 1.0f /*MENU_SUMMARY_BANNER_SPEED_REV*/) && m_iSelection)
            {
                // 
                     if(m_eState      >= SUMMARY_SKIPPED) m_eState = SUMMARY_OUTRO;     // leave summary
                else if(m_fIntroTimer >= fSpinTo)         m_eState = SUMMARY_OUTRO;
                else if(m_fIntroTimer <  fSpinTo)         m_eState = SUMMARY_SKIPPED;   // skip blend-in
                
                //if((eOld != SUMMARY_OUTRO) && (m_eState == SUMMARY_OUTRO)) 
                //    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
            }
            
            if((eOld != SUMMARY_SKIPPED) && (m_eState == SUMMARY_SKIPPED)) m_fTimeShift = MAX0(MENU_SUMMARY_SKIP_SEGMENT - m_fIntroTimer);

            // 
            if(m_fIntroTimer >= 1.0f) m_Navigator.SetActive(true);

            // 
            if(m_eState == SUMMARY_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= MENU_SUMMARY_BANNER_SPEED_REV)
            {
                // 
                m_iStatus = m_iSelection;
            }

            if(m_eState != SUMMARY_WAIT)
            {
                // 
                if((m_fIntroTimer >= MENU_SUMMARY_BANNER_ANIMATION) && (m_eState < SUMMARY_WAIT)) m_eState = SUMMARY_WAIT;
                STATIC_ASSERT(MENU_SUMMARY_BANNER_ANIMATION >= fSpinTo)

                // 
                const coreFloat fBlendIn  = m_fIntroTimer + m_fTimeShift;
                const coreFloat fBlendOut = 1.0f - m_fOutroTimer * MENU_SUMMARY_BANNER_SPEED;

                // (# enable-state should not be changed) 
                const auto nBlendMedalFunc = [&](cGuiObject* OUTPUT pMedal, const coreFloat fScale, const coreFloat fThreshold, const coreBool bMission)
                {
                    const coreFloat fFadeIn     = CLAMP01((fBlendIn - fThreshold) * 10.0f);
                    const coreInt32 iStatusdOld = pMedal->GetStatus();
                    const coreInt32 iStatusdNew = (fBlendIn >= fThreshold) ? 1 : 0;

                    // 
                    pMedal->SetSize  (coreVector2(fScale, fScale) * LERP(1.5f, 1.0f, fFadeIn));
                    pMedal->SetAlpha (MIN(fFadeIn, fBlendOut));
                    pMedal->SetStatus(iStatusdNew);

                    // 
                    pMedal->Move();

                    // 
                    if(pMedal->GetEnabled() && !iStatusdOld && iStatusdNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, bMission ? SPECIAL_SOUND_MEDAL(m_aiApplyMedal[0]) : SOUND_SUMMARY_MEDAL);
                };
                nBlendMedalFunc(&m_SegmentMedal, 0.13f, MENU_SUMMARY_SKIP_SEGMENT, true);

                // 
                const auto nBlendBadgeFunc = [&](cGuiObject* OUTPUT pBadge, cGuiObject* OUTPUT pWave, const coreFloat fThreshold)
                {
                    const coreFloat fFadeIn = CLAMP01((fBlendIn - fThreshold) * 10.0f);

                    // 
                    pBadge->SetSize (coreVector2(0.07f,0.07f) * LERP(1.5f, 1.0f, fFadeIn));
                    pBadge->SetAlpha(MIN(fFadeIn, fBlendOut));

                    // 
                    pBadge->Move();

                    pWave->SetSize (pBadge->GetSize () * 0.7f);
                    pWave->SetAlpha(pBadge->GetAlpha());
        
                    pWave->Move();
                    
                };
                for(coreUintW i = 0u; i < MENU_SUMMARY_BADGES; ++i) nBlendBadgeFunc(&m_aSegmentBadge[i], &m_aSegmentBadgeWave[i], 2.3f + 0.1f * I_TO_F(i));

                // 
                const auto nBlendLabelFunc = [&](cGuiLabel* OUTPUT pName, cGuiLabel* OUTPUT pValue1, cGuiLabel* OUTPUT pValue2, cGuiLabel* OUTPUT pPart, const coreFloat fThreshold)
                {
                    const coreObjectEnable eEnabledOld = pName->GetEnabled();
                    const coreObjectEnable eEnabledNew = (fBlendIn >= fThreshold) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                    // blend-in
                    pName  ->SetEnabled(eEnabledNew);
                    pValue1->SetEnabled(eEnabledNew);
                    pValue2->SetEnabled(eEnabledNew);
                    for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) pPart[i].SetEnabled(eEnabledNew);

                    // blend-out
                    pName  ->SetAlpha(fBlendOut);
                    pValue1->SetAlpha(fBlendOut);
                    pValue2->SetAlpha(fBlendOut);
                    for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) pPart[i].SetAlpha(fBlendOut);

                    // 
                    if(!eEnabledOld && eEnabledNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_SUMMARY_TEXT);
                };
                for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) nBlendLabelFunc(&m_aSegmentName[i], &m_aaSegmentValue[i][0], &m_aaSegmentValue[i][1], m_aaSegmentPart[i], 1.0f + 0.6f * I_TO_F(i));

                // 
                const auto nSignalFunc = [&](cGuiLabel* OUTPUT pSignal, const coreObject2D* pParent, const coreBool bCondition, const eSoundEffect eSoundIndex)
                {
                    const coreObjectEnable eEnabledOld = pSignal->GetEnabled();
                    const coreObjectEnable eEnabledNew = (pParent->IsEnabled(CORE_OBJECT_ENABLE_ALL) && bCondition) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                    // 
                    pSignal->SetEnabled(eEnabledNew);
                    pSignal->SetAlpha  (pParent->GetAlpha());

                    // 
                    if(!eEnabledOld && eEnabledNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, eSoundIndex);
                };
                nSignalFunc(&m_aRecord[0], &m_aaSegmentValue[0][1], HAS_BIT(m_iSignalActive, 2u), SOUND_SUMMARY_RECORD);
                nSignalFunc(&m_aRecord[1], &m_aaSegmentValue[1][1], HAS_BIT(m_iSignalActive, 3u), SOUND_SUMMARY_RECORD);

                // calculate visibility and animation value
                const coreFloat fVisibility = MAX0(MIN(m_fIntroTimer, MENU_SUMMARY_BANNER_SPEED_REV - m_fOutroTimer)) * MENU_SUMMARY_BANNER_SPEED;
                const coreFloat fAnimation  = BLENDB(MIN1(m_fIntroTimer / MENU_SUMMARY_BANNER_ANIMATION)) * MENU_SUMMARY_BANNER_ANIMATION;

                // slash background across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
                const coreBool bLeftRight = m_fOutroTimer ? true : false;
                m_BackgroundMain.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_BackgroundMain.GetDirection().yx());
                m_BackgroundMain.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

                // animate background
                m_BackgroundMain.SetSize     (coreVector2(fVisibility, 1.0f) * coreVector2(1.0f,0.45f));
                m_BackgroundMain.SetTexSize  (coreVector2(fVisibility, 1.0f));
                m_BackgroundMain.SetTexOffset(coreVector2(1.0f,1.0f) * (fAnimation * 0.05f));

                // duplicate background for multiplayer
                m_BackgroundCoop.SetPosition (m_BackgroundMain.GetPosition ().Rotated90() * -1.0f);
                m_BackgroundCoop.SetAlpha    (m_BackgroundMain.GetAlpha    ());
                m_BackgroundCoop.SetSize     (m_BackgroundMain.GetSize     ());
                m_BackgroundCoop.SetTexSize  (m_BackgroundMain.GetTexSize  ());
                m_BackgroundCoop.SetTexOffset(m_BackgroundMain.GetTexOffset());

                // move background manually (to remove some delay)
                m_BackgroundMain.Move();
                m_BackgroundCoop.Move();

                // 
                m_aHeader[0].SetAlpha(fVisibility);
                m_aHeader[1].SetAlpha(fVisibility);
                m_Icon      .SetAlpha(fVisibility);
                
                
                m_NextButton .SetAlpha(fVisibility);
                m_AgainButton.SetAlpha(fVisibility);
                m_ExitButton .SetAlpha(fVisibility);
                m_Navigator  .SetAlpha(fVisibility);
                
                
                m_Icon.SetSize(coreVector2(0.3f,0.3f) * LERPB(0.85f, 1.0f, fVisibility));
                m_Icon.Move();
            }

            // 
            cMenu::UpdateButton(&m_NextButton,  &m_Navigator, m_NextButton .IsFocused());   // after SetAlpha
            cMenu::UpdateButton(&m_AgainButton, &m_Navigator, m_AgainButton.IsFocused());
            cMenu::UpdateButton(&m_ExitButton,  &m_Navigator, m_ExitButton .IsFocused());
            
            const coreFloat fRotation = Core::System->GetTotalTimeFloat(2.0*PI_D);

            for(coreUintW i = 0u; i < MENU_SUMMARY_BADGES; ++i)
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
            
            
            m_Icon.SetAlpha(m_Icon.GetAlpha() * 0.6f);
        }
        break;

    case SURFACE_SUMMARY_BEGINNING:
    case SURFACE_SUMMARY_TITLE:
        {
            // 
            const coreFloat fOldTime = m_fIntroTimer;

            // 
            m_fIntroTimer.Update(1.0f);

            if(m_eState == SUMMARY_INTRO)
            {
                // 
                g_pPostProcessing->SetValueAll(1.0f - STEPH3(3.0f, 5.0f, m_fIntroTimer));

                if(InBetween(6.0f, fOldTime, m_fIntroTimer))
                {
                    // 
                    g_pMenu->ShiftSurface(this, SURFACE_SUMMARY_TITLE, 0.5f, 0u);
                }
                else if(InBetween(12.0f, fOldTime, m_fIntroTimer))
                {
                    // 
                    this->ChangeSurface(SURFACE_SUMMARY_BEGINNING, 0.7f);

                    // 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);   // to blend out environment sound
                }

                if(m_fIntroTimer >= 14.0f)
                {
                    // 
                    m_iStatus = 1;

                    // 
                    m_eState = SUMMARY_OUTRO;

                    // 
                    this->__ApplyDeferred();

                    // 
                    cMenu::ClearScreen();
                }
            }

            m_aTitle[0].RetrieveDesiredSize([this](const coreVector2 vSize1)
            {
                m_aTitle[1].RetrieveDesiredSize([&](const coreVector2 vSize2)
                {
                    const coreFloat fDiff = vSize1.x - vSize2.x;

                    m_aTitle[0].SetPosition(coreVector2(fDiff * 0.5f + 0.002f, m_aTitle[0].GetPosition().y));
                    m_aTitle[1].SetPosition(coreVector2(fDiff * 0.5f - 0.002f, m_aTitle[1].GetPosition().y));
                });
            });
        }
        break;

    case SURFACE_SUMMARY_ENDING_NORMAL:
        {
            // 
            const coreFloat fOldTime = m_fIntroTimer;

            // 
            m_fIntroTimer.Update(1.0f);

            // 
            const coreFloat fBlend = 1.0f - STEPH3(7.0f, 9.0f, m_fIntroTimer);
            g_pPostProcessing->SetValueAll   (fBlend);
            g_pPostProcessing->SetSoundVolume(fBlend);
            
            if(InBetween(3.0f, fOldTime, m_fIntroTimer))
            {
                g_pGame->GetInterface()->ShowStory(Core::Language->GetString("ENDING_NORMAL"), "", 9.0f, 0.5f);
            }

            if(m_fIntroTimer >= 13.0f)
            {
                // 
                m_iStatus = 2;

                // 
                m_eState = SUMMARY_OUTRO;

                // 
                this->__ApplyDeferred();

                // 
                g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);
                g_pPostProcessing->SetSoundVolume(1.0f);

                // 
                cMenu::ClearScreen();
            }
        }
        break;

    case SURFACE_SUMMARY_ENDING_SECRET:
        {
            // 
            m_fIntroTimer.Update(1.0f);

            if(m_fIntroTimer >= 1.0f)
            {
                // 
                m_iStatus = 2;

                // 
                m_eState = SUMMARY_OUTRO;

                // 
                this->__ApplyDeferred();

                // 
                g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

                // 
                cMenu::ClearScreen();
            }
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    // 
    const coreFloat fScale = 1.0f + 0.1f * SIN(Core::System->GetTotalTimeFloat(1.0) * (2.0f*PI));
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRecord); ++i) m_aRecord[i].SetScale(coreVector2(1.0f,1.0f) * fScale);
}


// ****************************************************************
// 
void cSummaryMenu::ShowArcade()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

    // 
    const cDataTable* pTable         = g_pGame->GetPlayer(0u)->GetDataTable();
    const coreUint16  iContinuesUsed = pTable->GetCounterTotal().iContinuesUsed;
    ASSERT(iContinuesUsed < MENU_SUMMARY_RUNS)

    // 
    coreUint32 aiScoreFull[MENU_SUMMARY_RUNS] = {};
    coreUint16 iShiftGood  = 0u;
    coreUint16 iShiftBad   = 0u;
    coreUint16 iMedalTotal = 0u;
    coreUint8  iMedalCount = 0u;
    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i)
    {
        // 
        coreUint32 aiRunScore[MENU_SUMMARY_RUNS] = {};
        coreUint8  aiRunIndex[MENU_SUMMARY_RUNS] = {}; std::memset(aiRunIndex, 0xFFu, sizeof(aiRunIndex));
        g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW j)
        {
            // 
            for(coreUintW k = 0u; k < iContinuesUsed; ++k)
            {
                aiRunScore[MENU_SUMMARY_RUNS - 1u - iContinuesUsed + k] += pPlayer->GetScoreTable()->GetRunMission(k, i);
                aiRunIndex[MENU_SUMMARY_RUNS - 1u - iContinuesUsed + k]  = pPlayer->GetScoreTable()->GetRunMissionIndex(k);
            }

            // 
            aiRunScore[MENU_SUMMARY_RUNS - 1u] += pPlayer->GetScoreTable()->GetScoreMission(i);
            aiRunIndex[MENU_SUMMARY_RUNS - 1u]  = g_pGame->GetCurMissionIndex();
        });

        // 
        const coreInt32 iShift = g_pGame->GetTimeTable()->GetShiftMission(i);
        const coreFloat fTime  = FloorFactor(g_pGame->GetTimeTable()->GetTimeMission(i), GAME_GOAL_FACTOR);

        // 
        const coreBool bValid = (fTime || (i == g_pGame->GetCurMissionIndex()));

        // 
        for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS; ++j)
        {
            // 
            m_aaArcadeScoreRun[i][j].SetText((aiRunScore[j] || (aiRunIndex[j] == i)) ? coreData::ToChars(aiRunScore[j]) : "");

            // 
            aiScoreFull[j] += aiRunScore[j];
        }

        // 
        m_aArcadeTime[i].SetText(bValid ? PRINT("%.1f %+d", fTime, iShift) : "");

        // 
        m_fOtherTime  += fTime;
        m_iOtherShift += iShift;

        // 
        const coreUint8 iMedalMission = pTable->GetMedalMission(i);

        // 
        cMenu::ApplyMedalTexture(&m_aArcadeMedalMission[i], iMedalMission, MEDAL_TYPE_MISSION, true);
        if(iMedalMission) m_iOtherNumMedal += 1u;

        // 
        const coreBool bAter = (i == MISSION_ATER);

        for(coreUintW j = 0u, je = bAter ? 2u : MENU_SUMMARY_MEDALS; j < je; ++j)
        {
            // 
            const coreUintW iIndex        = j + (bAter ? 5u : 0u);
            const coreUint8 iMedalSegment = pTable->GetMedalSegment(i, iIndex);

            // 
            cMenu::ApplyMedalTexture(&m_aaArcadeMedalSegment[i][j], iMedalSegment, MISSION_SEGMENT_IS_BOSS(iIndex) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, true);
            if(iMedalSegment) m_iOtherNumMedal += 1u;

            // 
            if((m_iOtherStart == 0xFFu) && iMedalSegment) m_iOtherStart = iMedalCount + i;

            // 
            iMedalTotal += MAX(iMedalSegment, MEDAL_BRONZE);
            iMedalCount += 1u;
        }

        // 
        iShiftGood += g_pGame->GetTimeTable()->GetShiftGoodMission(i);
        iShiftBad  += g_pGame->GetTimeTable()->GetShiftBadMission (i);

        // 
        if(bValid) m_iOtherNumMission = MAX(m_iOtherNumMission, i + 1u);
    }

    // 
    m_iFinalValue = (*std::max_element(aiScoreFull, aiScoreFull + MENU_SUMMARY_RUNS));

    // 
    for(coreUintW i = 0u; i < MENU_SUMMARY_ARCADES; ++i)
    {
        for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS; ++j)
        {
            const coreBool bBest = (aiScoreFull[j] == m_iFinalValue);

            m_aaArcadeScoreRun[i][j].SetColor3(COLOR_MENU_WHITE * (bBest ? 1.0f : MENU_LIGHT_IDLE));
            if(bBest) m_aArcadeScore[i].SetText(m_aaArcadeScoreRun[i][j].GetText());
        }
    }

    // 
    for(coreUintW j = 0u; j < MENU_SUMMARY_RUNS; ++j)
    {
        ASSERT(m_iOtherNumMission)
        
        const coreVector2 vBase    = m_aaArcadeScoreRun[m_iOtherNumMission - 1u][j].GetPosition();
        const coreBool    bVisible = (j >= MENU_SUMMARY_RUNS - 1u - iContinuesUsed);

        m_aArcadeSum[j].SetPosition(vBase + coreVector2(0.0f,-0.065f));
        m_aArcadeSum[j].SetColor3  (COLOR_MENU_WHITE * ((aiScoreFull[j] == m_iFinalValue) ? 1.0f : MENU_LIGHT_IDLE));
        m_aArcadeSum[j].SetText    (bVisible ? coreData::ToChars(aiScoreFull[j]) : "");
        
        m_aContinueImage[j].SetEnabled(bVisible ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);
    }

    // 
    const coreUint8 iMedal = iMedalCount ? MIN<coreUint8>((iMedalTotal + MEDAL_MARGIN_ARCADE) / iMedalCount, MEDAL_MAX - 1u) : MEDAL_NONE;   // TODO 1: get rid of template parameter

    // 
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->GetDataTable()->GiveMedalArade(iMedal);
    });

    // 
    cMenu::ApplyMedalTexture(&m_ArcadeTotalMedal, iMedal, MEDAL_TYPE_ARCADE, false);
    m_aiApplyMedal[0] = iMedal;

    // 
    const coreBool bComplete     = (pTable->GetMedalSegment(MISSION_INTRO, 0u) != MEDAL_NONE) && (pTable->GetMedalSegment(MISSION_ATER, 6u) != MEDAL_NONE);
    const coreBool bNearComplete = (pTable->GetMedalSegment(MISSION_INTRO, 0u) != MEDAL_NONE) && (pTable->GetMedalSegment(MISSION_ATER, 5u) != MEDAL_NONE);

    // 
    const auto&      oStats     = g_pSave->GetHeader().aaaLocalStatsArcade[g_pGame->GetType()][g_pGame->GetMode()][g_pGame->GetDifficulty()];
    const coreUint32 iBestScore = oStats.iScoreBest;
    const coreInt32  iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat  fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), GAME_GOAL_FACTOR);

    // 
    m_aArcadeTotalBest[0].SetText(coreData::ToChars((g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK) ? MAX(m_iFinalValue, iBestScore) : iBestScore));
    m_aArcadeTotalBest[1].SetText(bComplete ? PRINT("%.1f %+d", fBestTime, iBestShift) : "");

    // 
    const coreUint8* piShield       = g_pGame->GetOptions().aiShield;
    const coreBool   bOneColorClear = bNearComplete && !iContinuesUsed && std::all_of(piShield, piShield + g_pGame->GetNumPlayers(), [](const coreUint8 A) {return !A;});

    // 
    constexpr const coreChar* apcNameType      [] = {"GAME_TYPE_SOLO",       "GAME_TYPE_COOP",         ""};
    constexpr const coreChar* apcNameDifficulty[] = {"GAME_DIFFICULTY_EASY", "GAME_DIFFICULTY_NORMAL", "GAME_DIFFICULTY_HARD"};

#if 1
    const coreChar* pcOption = PRINT("%s / %s%s", Core::Language->GetString(apcNameType[g_pGame->GetType()]), Core::Language->GetString(apcNameDifficulty[g_pGame->GetDifficulty()]), bOneColorClear ? " / 1CC" : "");
#else
    const coreUint8 iRaise   = g_pGame->GetRaise();
    const coreChar* pcOption = PRINT("%s / %s / +%u%%%s", Core::Language->GetString(apcNameType[g_pGame->GetType()]), Core::Language->GetString(apcNameDifficulty[g_pGame->GetDifficulty()]), iRaise, bOneColorClear ? " / 1CC" : "");
#endif

    // 
    m_ArcadeOptions.SetText(pcOption);

    // 
    SET_BIT(m_iSignalActive, 6u, (m_iFinalValue > iBestScore) && (g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK))
    SET_BIT(m_iSignalActive, 7u, (false))

    // 
    g_pSave->EditLocalStatsArcade()->iScoreBest   = MAX(g_pSave->EditLocalStatsArcade()->iScoreBest,       m_iFinalValue);
    g_pSave->EditLocalStatsArcade()->iScoreWorst  = MIN(g_pSave->EditLocalStatsArcade()->iScoreWorst - 1u, m_iFinalValue - 1u) + 1u;
    g_pSave->EditLocalStatsArcade()->iScoreTotal += m_iFinalValue;

    if(bComplete)
    {
        // 
        const coreUint32 iTimeUint = TABLE_TIME_TO_UINT(m_fOtherTime);
        g_pSave->EditLocalStatsArcade()->iTimeBest   = MIN(g_pSave->EditLocalStatsArcade()->iTimeBest - 1u, iTimeUint - 1u) + 1u;
        g_pSave->EditLocalStatsArcade()->iTimeWorst  = MAX(g_pSave->EditLocalStatsArcade()->iTimeWorst,     iTimeUint);
        g_pSave->EditLocalStatsArcade()->iTimeTotal += iTimeUint;
        g_pSave->EditLocalStatsArcade()->iCountEnd  += 1u;

        // 
        const coreUint32 iTimeShiftedUint = TABLE_TIME_TO_UINT(m_fOtherTime + I_TO_F(iShiftBad) - I_TO_F(iShiftGood));
        if(iTimeShiftedUint - 1u < g_pSave->EditLocalStatsArcade()->iTimeBestShifted - 1u)
        {
            if(g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK) m_aArcadeTotalBest[1].SetText(PRINT("%.1f %+d", m_fOtherTime, m_iOtherShift));

            SET_BIT(m_iSignalActive, 7u, (true) && (g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK))

            g_pSave->EditLocalStatsArcade()->iTimeBestShifted   = iTimeShiftedUint;
            g_pSave->EditLocalStatsArcade()->iTimeBestShiftGood = iShiftGood;
            g_pSave->EditLocalStatsArcade()->iTimeBestShiftBad  = iShiftBad;
        }
        if(iTimeShiftedUint > g_pSave->EditLocalStatsArcade()->iTimeWorstShifted)
        {
            g_pSave->EditLocalStatsArcade()->iTimeWorstShifted   = iTimeShiftedUint;
            g_pSave->EditLocalStatsArcade()->iTimeWorstShiftGood = iShiftGood;
            g_pSave->EditLocalStatsArcade()->iTimeWorstShiftBad  = iShiftBad;
        }
    }

    if(bNearComplete)
    {
        // 
        if(bOneColorClear)
        {
            ADD_BIT_EX(g_pSave->EditProgress()->aiTrophy, TROPHY_ONECOLORCLEAR)
        }

        // 
        if(GetCurGameSpeed() >= 200u)
        {
            ADD_BIT(g_pSave->EditLocalStatsArcade()->iFeat, FEAT_TWOHUNDRED)
        }
    }

    if(DEFINED(_CORE_DEBUG_) || m_iOtherNumMedal)
    {
        coreByte*  pReplayData = NULL;
        coreUint32 iReplaySize = 0u;

        // 
        if((g_pReplay->GetMode() == REPLAY_MODE_RECORDING) && g_pReplay->HasData())
        {
            g_pReplay->EndRecording();
            g_pReplay->SaveData(&pReplayData, &iReplaySize);

            m_bHasReplay = true;
        }

        // 
        UploadLeaderboardsArcade(m_iFinalValue, bComplete ? (TABLE_TIME_TO_UINT(m_fOtherTime + I_TO_F(iShiftBad) - I_TO_F(iShiftGood))) : 0u, &pReplayData, iReplaySize);
        SAFE_DELETE_ARRAY(pReplayData)
    }

    // 
    g_pSave->SaveFile();

    // 
    this->ChangeSurface(SURFACE_SUMMARY_ARCADE_1, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowMission()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

    // 
    m_BackgroundCoop.SetCenter(coreVector2(0.0f,-0.14f));

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    const coreBool  bAter         = (iMissionIndex == MISSION_ATER);

    // 
    m_aHeader[0].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetMissionLetters(iMissionIndex)));
    m_aHeader[1].SetText(g_pGame->GetCurMission()->GetName());

    // 
    m_Icon.SetColor3   (g_aMissionData[iMissionIndex].vColor * 0.8f);
    m_Icon.SetTexOffset(g_aMissionData[iMissionIndex].vIcon);

    // 
    coreUint32 iBonusMedal = 0u;
    coreUint16 iMedalTotal = 0u;
    coreUint8  iMedalCount = 0u;
    for(coreUintW i = 0u, ie = bAter ? 2u : MENU_SUMMARY_MEDALS; i < ie; ++i)
    {
        const coreUintW iIndex        = i + (bAter ? 5u : 0u);
        const coreUint8 iMedalSegment = g_pGame->GetPlayer(0u)->GetDataTable()->GetMedalSegment(iMissionIndex, iIndex);

        // 
        iBonusMedal += cGame::CalcBonusMedal(iMedalSegment);
        iMedalTotal += iMedalSegment;
        iMedalCount += 1u;

        // 
        this->__SetMedalSegment(i, iMedalSegment);
    }

    // 
    coreUint32 iBonusSurvive   = 0u;
    coreBool   bPerfectSurvive = true;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint32 iDamageTaken   = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iDamageTaken;
        const coreUint16 iContinuesUsed = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iContinuesUsed;
        const coreUint16 iRepairsUsed   = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iRepairsUsed;

        // 
        iBonusSurvive += cGame::CalcBonusSurvive(iDamageTaken, iContinuesUsed || iRepairsUsed);

        // 
        if(iDamageTaken || iContinuesUsed || iRepairsUsed) bPerfectSurvive = false;
    });

    // 
    iBonusMedal   = g_pGame->RaiseValue(iBonusMedal);
    iBonusSurvive = g_pGame->RaiseValue(iBonusSurvive);

    // 
    const coreUint32 iModifier = g_pGame->IsCoop() ? GAME_PLAYERS : 1u;
    iBonusSurvive /= iModifier;

    // 
    const coreUint8 iMedalMission = iMedalCount ? MIN<coreUint8>((iMedalTotal + (bAter ? MEDAL_MARGIN_MISSION_ATER : MEDAL_MARGIN_MISSION)) / iMedalCount, MEDAL_MAX - 1u) : MEDAL_NONE;   // TODO 1: get rid of template parameter
    this->__SetMedalMission(iMedalMission);

    // 
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint32 iBonus = iBonusMedal + iBonusSurvive;
        const coreUint32 iScore = pPlayer->GetScoreTable()->GetScoreMission(iMissionIndex);

        // 
        m_iFinalValue    += iBonus + iScore;
        m_aiFinalPart [i] = iBonus + iScore;
        m_aiApplyBonus[i] = iBonus;
        m_aiApplyMedal[i] = iMedalMission;

        // 
        m_aaPart[0][i].SetText(coreData::ToChars(iBonusMedal));
        m_aaPart[1][i].SetText(coreData::ToChars(iBonusSurvive));
    });

    // 
    m_aValue[0].SetText(coreData::ToChars(iBonusMedal   * iModifier));
    m_aValue[1].SetText(coreData::ToChars(iBonusSurvive * iModifier));

    // 
    const coreUint32 iBestScore = g_pSave->GetHeader().aaaaLocalStatsMission[g_pGame->GetType()][g_pGame->GetMode()][g_pGame->GetDifficulty()][iMissionIndex].iScoreBest;
    m_TotalBest.SetText(coreData::ToChars((g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK) ? MAX(m_iFinalValue, iBestScore) : iBestScore));

    // 
    const coreUint8 iRaise = g_pGame->GetRaise();
    m_Raise.SetText(PRINT("+%u%%", iRaise));

    // 
    SET_BIT(m_iSignalActive, 0u, (iMedalTotal == iMedalCount * MEDAL_DARK))
    SET_BIT(m_iSignalActive, 1u, (bPerfectSurvive))
    SET_BIT(m_iSignalActive, 5u, (m_iFinalValue > iBestScore) && (g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK))

    // 
    this->ChangeSurface(g_pGame->IsCoop() ? SURFACE_SUMMARY_MISSION_COOP : SURFACE_SUMMARY_MISSION_SOLO, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowSegment()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

    // 
    m_BackgroundCoop.SetCenter(coreVector2(0.0f,0.12f));

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();
    const coreUintW iSegmentIndex = g_pGame->GetCurMission()->GetTakeFrom();
    const coreBool  bBoss         = MISSION_SEGMENT_IS_BOSS(iSegmentIndex);
    const coreBool  bIntro        = (iMissionIndex == MISSION_INTRO);

    // 
    m_aHeader[0].SetText(PRINT("%s %s", Core::Language->GetString("SEGMENT"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aHeader[1].SetText(g_pGame->GetCurMission()->GetName());

    // 
    m_Icon.SetColor3   (((iMissionIndex == MISSION_ATER) ? g_pEnvironment->GetBackground()->GetColor() : g_aMissionData[iMissionIndex].vColor) * 0.8f);
    m_Icon.SetTexOffset(g_aMissionData[iMissionIndex].vIcon);

    // 
    coreUint32 iScoreFull = 0u;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const auto& oCounter = pPlayer->GetDataTable()->GetCounterSegment(iMissionIndex, iSegmentIndex);

        const coreUint32 iScore = pPlayer->GetScoreTable()->GetScoreSegment    (iMissionIndex, iSegmentIndex);
        const coreInt32  iShift = coreInt32(oCounter.iShiftBadAdded);   // # only bad

        // 
        m_aaSegmentPart[0][i].SetText(coreData::ToChars(iScore));
        m_aaSegmentPart[1][i].SetText(PRINT("%+d", iShift));

        // 
        iScoreFull += iScore;
    });

    // 
    const coreInt32 iShift = g_pGame->GetTimeTable()->GetShiftSegment(iMissionIndex, iSegmentIndex);
    const coreFloat fTime  = g_pGame->GetTimeTable()->GetTimeSegment (iMissionIndex, iSegmentIndex);

    // 
    m_aaSegmentValue[0][0].SetText(coreData::ToChars(iScoreFull));
    m_aaSegmentValue[1][0].SetText(PRINT("%.1f %+d", fTime, iShift));

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaaaaLocalStatsSegment[g_pGame->GetType()][g_pGame->GetMode()][g_pGame->GetDifficulty()][iMissionIndex][iSegmentIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), GAME_GOAL_FACTOR);

    // 
    m_aaSegmentValue[0][1].SetText(coreData::ToChars(oStats.iScoreBest));
    m_aaSegmentValue[1][1].SetText(PRINT("%.1f %+d", fBestTime, iBestShift));

    // 
    const coreUint8 iMedal = g_pGame->GetPlayer(0u)->GetDataTable()->GetMedalSegment(iMissionIndex, iSegmentIndex);
    cMenu::ApplyMedalTexture(&m_SegmentMedal, iMedal, bBoss ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, true);

    // 
    m_aiApplyMedal[0] = iMedal;

    // 
    for(coreUintW i = 0u; i < MENU_SUMMARY_BADGES; ++i)
    {
        const coreBool bState = g_pGame->GetPlayer(0u)->GetDataTable()->GetBadge(i, iMissionIndex, iSegmentIndex);

        // 
        m_aSegmentBadge[i].SetPosition (m_SegmentMedal.GetPosition() + coreVector2(0.11f, ((I_TO_F(i) - 0.5f) * -0.06f) * (bIntro ? 0.0f : 1.0f)));
        m_aSegmentBadge[i].SetTexOffset(coreVector2(bState ? 0.0f : 0.5f, 0.0f));
        m_aSegmentBadge[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * (bState ? 1.0f : 0.5f));
        m_aSegmentBadge[i].SetEnabled  (((!bIntro || (i == 0u)) && !bBoss) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        // 
        m_aSegmentBadgeWave[i].SetPosition(m_aSegmentBadge[i].GetPosition());
        m_aSegmentBadgeWave[i].SetEnabled (bState ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    const coreUint8 iRaise = g_pGame->GetRaise();
    m_SegmentRaise.SetText(PRINT("+%u%%", iRaise));

    // 
    const coreUint8 iRecordBroken = (g_pReplay->GetMode() != REPLAY_MODE_PLAYBACK) ? g_pGame->GetCurMission()->GetRecordBroken() : 0u;
    SET_BIT(m_iSignalActive, 2u, HAS_BIT(iRecordBroken, 0u))
    SET_BIT(m_iSignalActive, 3u, HAS_BIT(iRecordBroken, 1u))

    // 
    const coreBool bNoNext = (iMissionIndex >= MISSION_ATER) && bBoss;
    m_NextButton.SetOverride(bNoNext ? -1 : 0);

    // 
    m_Navigator.ResetFirst();
    m_Navigator.SetActive(false);

    // 
    this->ChangeSurface(g_pGame->IsCoop() ? SURFACE_SUMMARY_SEGMENT_COOP : SURFACE_SUMMARY_SEGMENT_SOLO, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowBeginning()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // (hidden bonus and medal) 
    this->ShowMission();
    ASSERT(g_pGame->GetKind() != GAME_KIND_SEGMENT)

    // 
    this->ChangeSurface(SURFACE_SUMMARY_BEGINNING, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowEndingNormal()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    if(g_pGame->GetKind() == GAME_KIND_SEGMENT)
    {
        // 
        this->__ResetState();
    }
    else
    {
        // (hidden bonus and medal) 
        this->ShowMission();
    }

    // 
    this->ChangeSurface(SURFACE_SUMMARY_ENDING_NORMAL, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowEndingSecret()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    if(g_pGame->GetKind() == GAME_KIND_SEGMENT)
    {
        // 
        this->__ResetState();
    }
    else
    {
        // (hidden bonus and medal) 
        this->ShowMission();
    }

    // 
    this->ChangeSurface(SURFACE_SUMMARY_ENDING_SECRET, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::SetHighlightColor(const coreVector3 vColor)
{
    // 
    m_aHeader[1].SetColor3(vColor);

    // 
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) m_aName[i].SetColor3(vColor);
    m_TotalName.SetColor3(vColor);

    // 
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) m_aSegmentName[i].SetColor3(vColor);
    
    
    
    m_aArcadeTotalName[0].SetColor3(vColor);
    m_aArcadeTotalName[1].SetColor3(vColor);
    m_ArcadeOptions      .SetColor3(vColor);
}


// ****************************************************************
// 
void cSummaryMenu::__SetMedalMission(const coreUint8 iMedal)
{
    // 
    cMenu::ApplyMedalTexture(&m_MedalMission, iMedal, MEDAL_TYPE_MISSION, true);
}

void cSummaryMenu::__SetMedalSegment(const coreUintW iIndex, const coreUint8 iMedal)
{
    // 
    ASSERT(iIndex < MENU_SUMMARY_MEDALS)
    cMenu::ApplyMedalTexture(&m_aMedalSegment[iIndex], iMedal, MISSION_SEGMENT_IS_BOSS(iIndex) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE, true);
}


// ****************************************************************
// 
void cSummaryMenu::__ApplyDeferred()
{
    if(STATIC_ISVALID(g_pGame))
    {
        ASSERT((g_pGame->GetCurMission()->GetCurSegmentIndex() == MISSION_NO_SEGMENT))

        // 
        g_pGame->ForEachPlayerAll([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            if(m_aiApplyBonus[i]) {pPlayer->GetScoreTable()->AddScore(m_aiApplyBonus[i], false);  m_aiApplyBonus[i] = 0u;}
            if(m_aiApplyMedal[i]) {pPlayer->GetDataTable ()->GiveMedalMission(m_aiApplyMedal[i]); m_aiApplyMedal[i] = 0u;}
        });
    }
}


// ****************************************************************
// 
void cSummaryMenu::__ResetState()
{
    // 
    m_iFinalValue      = 0u;
    std::memset(m_aiFinalPart,  0, sizeof(m_aiFinalPart));
    std::memset(m_aiApplyBonus, 0, sizeof(m_aiApplyBonus));
    std::memset(m_aiApplyMedal, 0, sizeof(m_aiApplyMedal));
    m_fOtherTime       = 0.0f;
    m_iOtherShift      = 0;
    m_iOtherNumMission = 0u;
    m_iOtherNumMedal   = 0u;
    m_iOtherStart      = 0xFFu;
    m_fIntroTimer      = 0.0f;
    m_fOutroTimer      = 0.0f;
    m_fTimeShift       = 0.0f;
    m_fFinalSpinOld    = 0.0f;
    m_eState           = SUMMARY_INTRO;
    m_bHasReplay       = false;

    // 
    m_BackgroundMain.SetSize(coreVector2(0.0f,0.0f));
    m_BackgroundMain.Move();
    m_BackgroundCoop.SetSize(coreVector2(0.0f,0.0f));
    m_BackgroundCoop.Move();

    // 
    m_Icon.SetSize(coreVector2(0.0f,0.0f));
    m_Icon.Move();

    // 
    this->SetAlpha(0.0f);
    
    
    m_iSelection = 0u;
    
    std::memset(m_afSignalTime, 0, sizeof(m_afSignalTime));
    m_iSignalActive = 0u;
}