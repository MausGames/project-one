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
    m_BackgroundCoop.SetCenter    (coreVector2(0.0f,-0.16f));

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
    m_aHeader[0].SetPosition(coreVector2(0.0f,0.39f));
    m_aHeader[0].SetColor3  (COLOR_MENU_WHITE);

    m_aHeader[1].Construct  (MENU_FONT_STANDARD_6, MENU_OUTLINE_SMALL);
    m_aHeader[1].SetPosition(coreVector2(0.0f, m_aHeader[0].GetPosition().y - 0.05f));
    m_aHeader[1].SetColor3  (COLOR_MENU_WHITE);

    m_MedalMission.DefineTexture(0u, "menu_medal.png");
    m_MedalMission.DefineProgram("default_2d_program");
    m_MedalMission.SetPosition  (coreVector2(0.0f,-0.37f));
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
        m_aName[i].SetPosition(coreVector2(0.0f, -0.013f - 0.1f*I_TO_F(i)));
        m_aName[i].SetColor3  (COLOR_MENU_WHITE);

        m_aValue[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aValue[i].SetPosition(coreVector2(0.0f, m_aName[i].GetPosition().y - 0.03f));
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
    m_TotalName.SetPosition    (coreVector2(0.0f,-0.242f));
    m_TotalName.SetColor3      (COLOR_MENU_WHITE);
    m_TotalName.SetTextLanguage("SUMMARY_TOTAL");

    m_TotalValue.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_TotalValue.SetPosition(coreVector2(0.0f, m_TotalName.GetPosition().y - 0.045f));
    m_TotalValue.SetColor3  (COLOR_MENU_WHITE);

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        const coreFloat fSide = j ? 1.0f : -1.0f;

        m_aTotalPart[j].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aTotalPart[j].SetPosition(m_TotalValue.GetPosition() + coreVector2(fSide * 0.36f, 0.0f));
        m_aTotalPart[j].SetColor3  (COLOR_MENU_WHITE);
    }

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
    }

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_MISSION_COOP, &m_aaPart[i][j]);
        this->BindObject(SURFACE_SUMMARY_MISSION_COOP, &m_aTotalPart[j]);
    }

    this->BindObject(SURFACE_SUMMARY_TITLE, &m_aTitle[0]);
    this->BindObject(SURFACE_SUMMARY_TITLE, &m_aTitle[1]);
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
                
                if((eOld != SUMMARY_OUTRO) && (m_eState == SUMMARY_OUTRO)) g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 1.0f);
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

                // 
                const auto nBlendMedalFunc = [&](cGuiObject* OUTPUT pMedal, const coreFloat fScale, const coreFloat fThreshold, const coreBool bMission)
                {
                    const coreFloat        fFadeIn     = CLAMP01((fBlendIn - fThreshold) * 10.0f);
                    const coreObjectEnable eEnabledOld = pMedal->GetEnabled();
                    const coreObjectEnable eEnabledNew = (fBlendIn >= fThreshold) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                    // 
                    pMedal->SetSize   (coreVector2(fScale, fScale) * LERP(1.5f, 1.0f, fFadeIn));
                    pMedal->SetAlpha  (MIN(fFadeIn, fBlendOut));
                    pMedal->SetEnabled(eEnabledNew);

                    // 
                    pMedal->Move();

                    // 
                    if(!eEnabledOld && eEnabledNew) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, bMission ? SPECIAL_SOUND_MEDAL(m_aiApplyMedal[0]) : SOUND_SUMMARY_MEDAL);
                };
                nBlendMedalFunc(&m_MedalMission, 0.1f, 5.5f, true);
                for(coreUintW i = 0u; i < MENU_SUMMARY_MEDALS; ++i) nBlendMedalFunc(&m_aMedalSegment[i], MISSION_SEGMENT_IS_BOSS(i) ? 0.085f : 0.07f, 0.8f + 0.1f * I_TO_F(i), false);

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

                // 
                const coreFloat fTickStep = 1.35f * RCP(ROUND(RCP(20.0f * TIME)) * TIME);
                if(F_TO_UI(m_fFinalSpinOld * fTickStep) < F_TO_UI(fFinalSpin * fTickStep)) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_SUMMARY_SCORE);
                m_fFinalSpinOld = fFinalSpin;

                // calculate visibility and animation value
                const coreFloat fVisibility = MAX(MIN(m_fIntroTimer, MENU_SUMMARY_BANNER_SPEED_REV - m_fOutroTimer), 0.0f) * MENU_SUMMARY_BANNER_SPEED;
                const coreFloat fAnimation  = LERPB(0.0f, 1.0f, MIN(m_fIntroTimer / MENU_SUMMARY_BANNER_ANIMATION, 1.0f)) * MENU_SUMMARY_BANNER_ANIMATION;

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
        }
        break;

    case SURFACE_SUMMARY_SEGMENT_SOLO:
    case SURFACE_SUMMARY_SEGMENT_COOP:
        {
            // 
            m_iStatus = 1;
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
}


// ****************************************************************
// 
void cSummaryMenu::ShowMission()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();

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
    coreUint32 iBonusSurvive = 0u;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint32 iDamageTaken   = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iDamageTaken;
        const coreUint32 iContinuesUsed = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iContinuesUsed;
        const coreUint32 iRepairsUsed   = pPlayer->GetDataTable()->GetCounterMission(iMissionIndex).iRepairsUsed;

        // 
        iBonusSurvive += cGame::CalcBonusSurvive(iDamageTaken, iContinuesUsed || iRepairsUsed);
    });

    // 
    const coreUint32 iModifier = g_pGame->IsCoop() ? GAME_PLAYERS : 1u;
    iBonusSurvive /= iModifier;

    // 
    const coreUint8 iMedalMission = (iMedalCount == MENU_SUMMARY_MEDALS) ? (iMedalTotal / MENU_SUMMARY_MEDALS) : MEDAL_NONE;
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
    this->ChangeSurface(g_pGame->IsCoop() ? SURFACE_SUMMARY_MISSION_COOP : SURFACE_SUMMARY_MISSION_SOLO, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowSegment()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();
    
    // TODO 1: implement, restart, exit, show some data NEW RECORD, bla

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
}