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
: coreMenu        (SURFACE_SUMMARY_MAX, SURFACE_SUMMARY_MISSION_SOLO)
, m_iFinalValue   (0u)
, m_aiFinalPart   {}
, m_aiApplyBonus  {}
, m_aiApplyMedal  {}
, m_iSelection    (0u)
, m_afSignalTime  {}
, m_iSignalActive (0u)
, m_fIntroTimer   (0.0f)
, m_fOutroTimer   (0.0f)
, m_fFinalSpinOld (0.0f)
, m_eState        (SUMMARY_INTRO)
{
    // create menu objects
    m_BackgroundMain.DefineTexture(0u, "menu_detail_04.png");
    m_BackgroundMain.DefineTexture(1u, "menu_background_black.png");
    m_BackgroundMain.DefineProgram("menu_animate_program");
    m_BackgroundMain.SetDirection (coreVector2(-1.0f,0.0f));

    m_BackgroundCoop.DefineTexture(0u, "menu_detail_04.png");
    m_BackgroundCoop.DefineTexture(1u, "menu_background_black.png");
    m_BackgroundCoop.DefineProgram("menu_animate_program");

    m_RestartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_RestartButton.DefineProgram("menu_border_program");
    m_RestartButton.SetPosition  (coreVector2(0.0f,-0.29f));
    m_RestartButton.SetSize      (coreVector2(0.4f,0.07f));
    m_RestartButton.GetCaption()->SetTextLanguage("AGAIN");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_RestartButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_RestartButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    m_aTitle[0].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aTitle[0].SetPosition(coreVector2(0.0f,0.0f));
    m_aTitle[0].SetAlignment(coreVector2(-1.0f,1.0f));
    m_aTitle[0].SetColor3  (COLOR_MENU_INSIDE);
    m_aTitle[0].SetText    ("EIGEN");

    m_aTitle[1].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_aTitle[1].SetPosition(coreVector2(0.0f,0.0f));
    m_aTitle[1].SetAlignment(coreVector2(1.0f,1.0f));
    m_aTitle[1].SetColor3  (COLOR_MENU_INSIDE * 0.6f);
    m_aTitle[1].SetText    ("GRAU");

    m_aHeader[0].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_aHeader[0].SetPosition(coreVector2(0.0f,0.42f));
    m_aHeader[0].SetColor3  (COLOR_MENU_WHITE);

    m_aHeader[1].Construct  (MENU_FONT_STANDARD_6, MENU_OUTLINE_SMALL);
    m_aHeader[1].SetPosition(coreVector2(0.0f, m_aHeader[0].GetPosition().y - 0.05f));
    m_aHeader[1].SetColor3  (COLOR_MENU_WHITE);

    m_MedalMission.DefineTexture(0u, "menu_medal.png");
    m_MedalMission.DefineProgram("default_2d_program");
    m_MedalMission.SetPosition  (coreVector2(0.0f,-0.385f));
    m_MedalMission.SetTexSize   (coreVector2(0.25f,0.25f));

    for(coreUintW i = 0u; i < MENU_SUMMARY_MEDALS; ++i)
    {
        const coreVector2 vOffset = MISSION_SEGMENT_IS_BOSS(i) ? coreVector2(0.0f,-0.16f) : coreVector2(((i < 3u) ? (I_TO_F(i) - 1.0f) : ((I_TO_F(i - 3u) - 0.5f))) * 0.075f, I_TO_F(i / 3u) * -0.075f);

        m_aMedalSegment[i].DefineTexture(0u, "menu_medal.png");
        m_aMedalSegment[i].DefineProgram("default_2d_program");
        m_aMedalSegment[i].SetPosition  (coreVector2(0.0f, 0.275f) + vOffset);
        m_aMedalSegment[i].SetTexSize   (coreVector2(0.25f,0.25f));
    }

    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i)
    {
        m_aName[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition(coreVector2(0.0f, 0.007f - 0.1f*I_TO_F(i)));
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
    m_TotalName.SetTextLanguage("SUMMARY_TOTAL");

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
    
    
    
    
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i)
    {
        m_aSegmentName[i].Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aSegmentName[i].SetPosition(coreVector2(0.0f, 0.27f - 0.125f*I_TO_F(i)));
        m_aSegmentName[i].SetColor3  (COLOR_MENU_WHITE);

        for(coreUintW j = 0u; j < MENU_SUMMARY_SIDES; ++j)
        {
            if(j) m_aaSegmentValue[i][j].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
             else m_aaSegmentValue[i][j].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
            //m_aaSegmentValue[i][j].SetPosition (coreVector2(m_aSegmentSide[j].GetPosition().x, m_aSegmentName[i].GetPosition().y - 0.035f));
            m_aaSegmentValue[i][j].SetPosition (coreVector2(0.0f, m_aSegmentName[i].GetPosition().y - 0.035f    +(j ? -0.032f : 0.0f)));
            //m_aaSegmentValue[i][j].SetAlignment(m_aSegmentSide[j].GetAlignment());
            m_aaSegmentValue[i][j].SetColor3   (j ? (COLOR_MENU_WHITE * 0.5f) : COLOR_MENU_WHITE);
        }

        for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
        {
            const coreFloat fSide = j ? 1.0f : -1.0f;

            m_aaSegmentPart[i][j].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
            m_aaSegmentPart[i][j].SetPosition(m_aaSegmentValue[i][0].GetPosition() + coreVector2(fSide * 0.36f, 0.0f));
            m_aaSegmentPart[i][j].SetColor3  (COLOR_MENU_WHITE);
        }
    }
    m_aSegmentName[0].SetTextLanguage("SUMMARY_SEGMENT_SCORE");
    m_aSegmentName[1].SetTextLanguage("SUMMARY_SEGMENT_TIME");
    m_aSegmentName[2].SetTextLanguage("SUMMARY_SEGMENT_COMBO");                           

    for(coreUintW j = 0u; j < MENU_SUMMARY_SIDES; ++j)
    {
        m_aSegmentShift[j].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aSegmentShift[j].SetPosition(m_aaSegmentValue[1][0].GetPosition() + coreVector2(0.36f, 0.0f));
        m_aSegmentShift[j].SetColor3  (COLOR_MENU_WHITE);
    }

    m_SegmentMedal.DefineTexture(0u, "menu_medal.png");
    m_SegmentMedal.DefineProgram("default_2d_program");
    m_SegmentMedal.SetPosition  (coreVector2(0.0f,-0.155f));
    m_SegmentMedal.SetTexSize   (coreVector2(0.25f,0.25f));
    
    

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aPerfect); ++i)
    {
        m_aPerfect[i].Construct      (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aPerfect[i].SetColor3      (COLOR_MENU_WHITE);
        m_aPerfect[i].SetTextLanguage("SUMMARY_PERFECT");
        m_aPerfect[i].SetRectify     (false);
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRecord); ++i)
    {
        m_aRecord[i].Construct      (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aRecord[i].SetColor3      (COLOR_MENU_WHITE);
        m_aRecord[i].SetTextLanguage("SUMMARY_RECORD");
        m_aRecord[i].SetRectify     (false);
    }
    m_aRecord[0].SetPosition(m_aaSegmentValue[0][1].GetPosition());
    m_aRecord[1].SetPosition(m_aaSegmentValue[1][1].GetPosition());
    m_aRecord[2].SetPosition(m_aaSegmentValue[2][1].GetPosition());
    m_aRecord[3].SetPosition(m_TotalBest           .GetPosition());
    

    // 
    m_Navigator.BindObject(NULL,             &m_RestartButton, NULL, &m_ExitButton, NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_RestartButton, NULL,             NULL, &m_ExitButton, NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ExitButton,    &m_RestartButton, NULL, NULL,          NULL, NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignMenu(this);

    // bind menu objects
    this->BindObject(SURFACE_SUMMARY_MISSION_COOP, &m_BackgroundCoop);

    for(coreUintW j = SURFACE_SUMMARY_MISSION_SOLO; j <= SURFACE_SUMMARY_MISSION_COOP; ++j)
    {
        this->BindObject(j, &m_BackgroundMain);
        this->BindObject(j, &m_MedalMission);
        for(coreUintW i = 0u; i < MENU_SUMMARY_MEDALS; ++i) this->BindObject(j, &m_aMedalSegment[i]);

        this->BindObject(j, &m_aHeader[0]);
        this->BindObject(j, &m_aHeader[1]);

        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(j, &m_aName[i]);
        this->BindObject(j, &m_TotalName);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(j, &m_aValue[i]);
        this->BindObject(j, &m_TotalValue);
        this->BindObject(j, &m_TotalBest);

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
        this->BindObject(j, &m_SegmentMedal);

        this->BindObject(j, &m_RestartButton);
        this->BindObject(j, &m_ExitButton);

        this->BindObject(j, &m_aHeader[0]);
        this->BindObject(j, &m_aHeader[1]);

        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(j, &m_aSegmentName  [i]);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(j, &m_aaSegmentValue[i][0]);
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(j, &m_aaSegmentValue[i][1]);
        //for(coreUintW i = 0u; i < MENU_SUMMARY_SIDES;           ++i) this->BindObject(j, &m_aSegmentShift [i]);
        
        this->BindObject(j, &m_aRecord[0]);
        this->BindObject(j, &m_aRecord[1]);
        this->BindObject(j, &m_aRecord[2]);

        this->BindObject(j, &m_Navigator);
    }

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) this->BindObject(SURFACE_SUMMARY_SEGMENT_COOP, &m_aaSegmentPart[i][j]);
    }

    this->BindObject(SURFACE_SUMMARY_TITLE, &m_aTitle[0]);
    this->BindObject(SURFACE_SUMMARY_TITLE, &m_aTitle[1]);

    STATIC_ASSERT(ARRAY_SIZE(m_aPerfect) + ARRAY_SIZE(m_aRecord) <= sizeof(m_iSignalActive)*8u)
}


// ****************************************************************
// render the summary menu
void cSummaryMenu::Render()
{
    // 
    cMenu::UpdateAnimateProgram(&m_BackgroundMain);
    ASSERT(m_BackgroundMain.GetSize() == m_BackgroundCoop.GetSize())

    // 
    this->coreMenu::Render();
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
    case SURFACE_SUMMARY_MISSION_SOLO:
    case SURFACE_SUMMARY_MISSION_COOP:
        {
            // 
            constexpr coreFloat fSpinFrom = (2.5f + 0.8f * I_TO_F(MENU_SUMMARY_ENTRIES));
            constexpr coreFloat fSpinTo   = fSpinFrom + 1.5f;

            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= 1.0f /*MENU_SUMMARY_BANNER_SPEED_REV*/) && Core::Input->GetAnyButton(CORE_INPUT_PRESS))
            {
                const eSummaryState eOld = m_eState;
                
                // 
                     if(m_eState      >= SUMMARY_SKIPPED) m_eState = SUMMARY_OUTRO;     // leave summary
                else if(m_fIntroTimer >= fSpinTo)         m_eState = SUMMARY_OUTRO;
                else if(m_fIntroTimer <  fSpinTo)         m_eState = SUMMARY_SKIPPED;   // skip blend-in
                
                if((g_pGame->GetKind() == GAME_KIND_ALL) && (eOld != SUMMARY_OUTRO) && (m_eState == SUMMARY_OUTRO)) 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
            }

            // 
            if(m_eState == SUMMARY_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= MENU_SUMMARY_BANNER_SPEED_REV)
            {
                // 
                m_iStatus = 1;
            }

            // 
            if((m_eState == SUMMARY_OUTRO) && STATIC_ISVALID(g_pGame))
            {
                ASSERT((g_pGame->GetCurMission()->GetCurSegmentIndex() == MISSION_NO_SEGMENT))

                // 
                g_pGame->ForEachPlayerAll([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    if(m_aiApplyBonus[i]) {pPlayer->GetScoreTable()->AddScore(m_aiApplyBonus[i], false);  m_aiApplyBonus[i] = 0u;}
                    if(m_aiApplyMedal[i]) {pPlayer->GetDataTable ()->GiveMedalMission(m_aiApplyMedal[i]); m_aiApplyMedal[i] = 0u;}
                });
            }

            if(m_eState != SUMMARY_WAIT)
            {
                // 
                if((m_fIntroTimer >= MENU_SUMMARY_BANNER_ANIMATION) && (m_eState < SUMMARY_WAIT)) m_eState = SUMMARY_WAIT;
                STATIC_ASSERT(MENU_SUMMARY_BANNER_ANIMATION >= fSpinTo)

                // 
                const coreFloat fBlendIn  = m_eState ? fSpinTo : m_fIntroTimer.ToFloat();
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
                nBlendMedalFunc(&m_MedalMission, 0.12f, 5.5f, true);
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
                const coreFloat fTickStep = 1.35f * RCP(ROUND(RCP(20.0f * TIME)) * TIME);
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
            }
            
                
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
            if(m_RestartButton.IsClicked())
            {
                // 
                m_iSelection = 3u;
            }
            else if(m_ExitButton.IsClicked())
            {
                // 
                m_iSelection = 1u;
            }

            // 
            cMenu::UpdateButton(&m_RestartButton, m_RestartButton.IsFocused());
            cMenu::UpdateButton(&m_ExitButton,    m_ExitButton   .IsFocused());
            
            
            // 
            constexpr coreFloat fSpinFrom = (2.5f + 0.8f * I_TO_F(MENU_SUMMARY_ENTRIES_SEGMENT));
            constexpr coreFloat fSpinTo   = fSpinFrom + 1.5f;
            
            const coreBool bAnyButton = Core::Input->GetAnyButton(CORE_INPUT_PRESS);
            if(bAnyButton && (m_fIntroTimer < fSpinTo) && m_eState < SUMMARY_SKIPPED) m_eState = SUMMARY_SKIPPED;

            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= 1.0f /*MENU_SUMMARY_BANNER_SPEED_REV*/) && m_iSelection)
            {
                const eSummaryState eOld = m_eState;
                
                // 
                     if(m_eState      >= SUMMARY_SKIPPED) m_eState = SUMMARY_OUTRO;     // leave summary
                else if(m_fIntroTimer >= fSpinTo)         m_eState = SUMMARY_OUTRO;
                else if(m_fIntroTimer <  fSpinTo)         m_eState = SUMMARY_SKIPPED;   // skip blend-in
                
                if((m_iSelection == 3u) && (eOld != SUMMARY_OUTRO) && (m_eState == SUMMARY_OUTRO)) 
                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
            }

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
                const coreFloat fBlendIn  = m_eState ? fSpinTo : m_fIntroTimer.ToFloat();
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
                nBlendMedalFunc(&m_SegmentMedal, 0.13f, 3.4f, true);

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
                for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES_SEGMENT; ++i) nBlendLabelFunc(&m_aSegmentName[i], &m_aaSegmentValue[i][0], &m_aaSegmentValue[i][1], m_aaSegmentPart[i], 1.0f + 0.8f * I_TO_F(i));

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
                nSignalFunc(&m_aRecord[2], &m_aaSegmentValue[2][1], HAS_BIT(m_iSignalActive, 4u), SOUND_SUMMARY_RECORD);

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
                
                
                m_RestartButton.SetAlpha(fVisibility);
                m_ExitButton   .SetAlpha(fVisibility);
            }
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

                // 
                if(g_pSave->GetHeader().oProgress.bFirstPlay)   // TODO 1: replace
                {
                    if(InBetween(12.0f, fOldTime, m_fIntroTimer))
                    {
                        // 
                        this->ChangeSurface(SURFACE_SUMMARY_BEGINNING, 0.7f);

                        // 
                        g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
                    }
                    else if(InBetween(6.0f, fOldTime, m_fIntroTimer))
                    {
                        // 
                        g_pMenu->ShiftSurface(this, SURFACE_SUMMARY_TITLE, 0.5f, 0u);
                    }
                }

                if(m_fIntroTimer >= (g_pSave->GetHeader().oProgress.bFirstPlay ? 14.0f : 6.0f))   // TODO 1: replace
                {
                    // 
                    m_iStatus = 1;

                    // 
                    m_eState = SUMMARY_OUTRO;

                    // 
                    //g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

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
            m_fIntroTimer.Update(1.0f);

            // 
            g_pPostProcessing->SetValueAll(1.0f - STEPH3(3.0f, 5.0f, m_fIntroTimer));

            if(m_fIntroTimer >= 5.0f)
            {
                // 
                m_iStatus = 2;

                // 
                m_eState = SUMMARY_OUTRO;

                // 
                g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

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
                g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

                // 
                cMenu::ClearScreen();
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    const coreFloat fScale = 1.0f + 0.1f * SIN(coreFloat(Core::System->GetTotalTime()) * (2.0f*PI));
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRecord); ++i) m_aRecord[i].SetScale(fScale);
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
    m_aHeader[0].SetText(PRINT("%s %zu", Core::Language->GetString("MISSION"), g_pGame->GetCurMissionIndex()));
    m_aHeader[1].SetText(g_pGame->GetCurMission()->GetName());

    // 
    const coreUintW iMissionIndex = g_pGame->GetCurMissionIndex();

    // 
    coreUint32 iBonusMedal = 0u;
    coreUint16 iMedalTotal = 0u;
    coreUint8  iMedalCount = 0u;
    for(coreUintW i = 0u; i < MENU_SUMMARY_MEDALS; ++i)
    {
        const coreUint8 iMedalSegment = MAX(g_pGame->GetPlayer(0u)->GetDataTable()->GetMedalSegment(iMissionIndex, i), DEFINED(_CORE_DEBUG_) ? MEDAL_BRONZE : MEDAL_NONE);

        // 
        iBonusMedal += cGame::CalcBonusMedal(iMedalSegment);
        iMedalTotal += iMedalSegment;
        iMedalCount += iMedalSegment ? 1u : 0u;

        // 
        this->__SetMedalSegment(i, iMedalSegment);
    }

    // 
    coreUint32 iBonusSurvive   = 0u;
    coreBool   bPerfectSurvive = true;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint32 iDamageTaken   = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iDamageTaken;
        const coreUint32 iContinuesUsed = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iContinuesUsed;
        const coreUint32 iRepairsUsed   = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iRepairsUsed;

        // 
        iBonusSurvive += cGame::CalcBonusSurvive(iDamageTaken, iContinuesUsed || iRepairsUsed);

        // 
        if(iDamageTaken || iContinuesUsed || iRepairsUsed) bPerfectSurvive = false;
    });

    // 
    const coreUint32 iModifier = g_pGame->IsCoop() ? GAME_PLAYERS : 1u;
    iBonusSurvive /= iModifier;

    // 
    const coreUint8 iMedalMission = iMedalCount ? (iMedalTotal / iMedalCount) : MEDAL_NONE;
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
    const coreUint32 iScoreBest = g_pSave->GetHeader().aLocalStatsMission[iMissionIndex].iScoreBest;
    m_TotalBest.SetText(coreData::ToChars(MAX(m_iFinalValue, iScoreBest)));

    // 
    SET_BIT(m_iSignalActive, 0u, (iMedalMission == MEDAL_DARK))
    SET_BIT(m_iSignalActive, 1u, (bPerfectSurvive))
    SET_BIT(m_iSignalActive, 5u, (m_iFinalValue > iScoreBest))

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

    // 
    m_aHeader[0].SetText(PRINT("%s %s", Core::Language->GetString("MISSION"), cMenu::GetSegmentLetters(iMissionIndex, iSegmentIndex)));
    m_aHeader[1].SetText(g_pGame->GetCurMission()->GetName());

    // 
    coreUint32 iScoreFull     = 0u;
    coreUint32 iMaxSeriesFull = 0u;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const auto& oCounter = pPlayer->GetDataTable()->GetCounterSegment(iMissionIndex, iSegmentIndex);

        const coreUint32 iScore     = pPlayer->GetScoreTable()->GetScoreSegment    (iMissionIndex, iSegmentIndex);
        const coreInt32  iShift     = coreInt32(oCounter.iShiftBad) - coreInt32(oCounter.iShiftGood);
        const coreUint32 iMaxSeries = pPlayer->GetScoreTable()->GetMaxSeriesSegment(iMissionIndex, iSegmentIndex);

        // 
        m_aaSegmentPart[0][i].SetText(coreData::ToChars(iScore));
        m_aaSegmentPart[1][i].SetText(PRINT("%+d", iShift));
        m_aaSegmentPart[2][i].SetText(bBoss ? coreData::ToChars(iMaxSeries) : PRINT("x%u.%u", 1u + iMaxSeries / 10u, iMaxSeries % 10u));

        // 
        iScoreFull     += iScore;
        iMaxSeriesFull += iMaxSeries;
    });

    // 
    if(g_pGame->IsCoop()) iMaxSeriesFull /= GAME_PLAYERS;

    // 
    const coreInt32 iShift = g_pGame->GetTimeTable()->GetShiftSegment(iMissionIndex, iSegmentIndex);
    const coreFloat fTime  = g_pGame->GetTimeTable()->GetTimeSegment (iMissionIndex, iSegmentIndex);

    // 
    m_aaSegmentValue[0][0].SetText(coreData::ToChars(iScoreFull));
    m_aaSegmentValue[1][0].SetText(PRINT("%.1f %+d", fTime, iShift));
    m_aaSegmentValue[2][0].SetText(bBoss ? coreData::ToChars(iMaxSeriesFull) : PRINT("x%u.%u", 1u + iMaxSeriesFull / 10u, iMaxSeriesFull % 10u));

    // 
    const auto&     oStats     = g_pSave->GetHeader().aaLocalStatsSegment[iMissionIndex][iSegmentIndex];
    const coreInt32 iBestShift = coreInt32(oStats.iTimeBestShiftBad) - coreInt32(oStats.iTimeBestShiftGood);
    const coreFloat fBestTime  = FloorFactor(TABLE_TIME_TO_FLOAT(oStats.iTimeBestShifted) - I_TO_F(iBestShift), 10.0f);

    // 
    m_aaSegmentValue[0][1].SetText(coreData::ToChars(oStats.iScoreBest));
    m_aaSegmentValue[1][1].SetText(PRINT("%.1f %+d", fBestTime, iBestShift));
    m_aaSegmentValue[2][1].SetText(bBoss ? coreData::ToChars(oStats.iMaxSeries) : PRINT("x%u.%u", 1u + oStats.iMaxSeries / 10u, oStats.iMaxSeries % 10u));

    // 
    m_aSegmentName[2].SetTextLanguage(bBoss ? "SUMMARY_SEGMENT_CHAIN" : "SUMMARY_SEGMENT_COMBO");

    // 
    const coreUint8 iMedal = MAX(g_pGame->GetPlayer(0u)->GetDataTable()->GetMedalSegment(iMissionIndex, iSegmentIndex), DEFINED(_CORE_DEBUG_) ? MEDAL_BRONZE : MEDAL_NONE);
    cMenu::ApplyMedalTexture(&m_SegmentMedal, iMedal, MISSION_SEGMENT_IS_BOSS(iSegmentIndex) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);

    // 
    m_aiApplyMedal[0] = iMedal;

    // 
    const coreUint8 iRecordBroken = g_pGame->GetCurMission()->GetRecordBroken();
    SET_BIT(m_iSignalActive, 2u, HAS_BIT(iRecordBroken, 0u))
    SET_BIT(m_iSignalActive, 3u, HAS_BIT(iRecordBroken, 1u))
    SET_BIT(m_iSignalActive, 4u, HAS_BIT(iRecordBroken, 2u))

    // 
    this->ChangeSurface(g_pGame->IsCoop() ? SURFACE_SUMMARY_SEGMENT_COOP : SURFACE_SUMMARY_SEGMENT_SOLO, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowBeginning()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

    // 
    this->ChangeSurface(SURFACE_SUMMARY_BEGINNING, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowEndingNormal()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

    // 
    this->ChangeSurface(SURFACE_SUMMARY_ENDING_NORMAL, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowEndingSecret()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

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
}


// ****************************************************************
// 
void cSummaryMenu::__SetMedalMission(const coreUint8 iMedal)
{
    // 
    cMenu::ApplyMedalTexture(&m_MedalMission, iMedal, MEDAL_TYPE_MISSION);
}

void cSummaryMenu::__SetMedalSegment(const coreUintW iIndex, const coreUint8 iMedal)
{
    // 
    ASSERT(iIndex < MENU_SUMMARY_MEDALS)
    cMenu::ApplyMedalTexture(&m_aMedalSegment[iIndex], iMedal, MISSION_SEGMENT_IS_BOSS(iIndex) ? MEDAL_TYPE_BOSS : MEDAL_TYPE_WAVE);
}


// ****************************************************************
// 
void cSummaryMenu::__ResetState()
{
    // 
    m_iFinalValue = 0u;
    std::memset(m_aiFinalPart,  0, sizeof(m_aiFinalPart));
    std::memset(m_aiApplyBonus, 0, sizeof(m_aiApplyBonus));
    std::memset(m_aiApplyMedal, 0, sizeof(m_aiApplyMedal));
    m_fIntroTimer = 0.0f;
    m_fOutroTimer = 0.0f;
    m_eState      = SUMMARY_INTRO;

    // 
    m_BackgroundMain.SetSize(coreVector2(0.0f,0.0f));
    m_BackgroundMain.Move();
    m_BackgroundCoop.SetSize(coreVector2(0.0f,0.0f));
    m_BackgroundCoop.Move();

    // 
    this->SetAlpha(0.0f);
    
    
    m_iSelection = 0u;
    
    std::memset(m_afSignalTime, 0, sizeof(m_afSignalTime));
    m_iSignalActive = 0u;
}