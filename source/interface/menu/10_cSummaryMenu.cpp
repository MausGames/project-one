///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cSummaryMenu::cSummaryMenu()noexcept
: coreMenu      (2u, SURFACE_SUMMARY_RECAP)
, m_iFinalScore (0u)
, m_fIntroTimer (0.0f)
, m_fOutroTimer (0.0f)
, m_iState      (SUMMARY_INTRO)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_detail_03.png");
    m_Background.DefineTexture(1u, "menu_background_black.png");
    m_Background.DefineProgram("menu_animate_program");
    m_Background.SetPosition  (coreVector2( 0.0f,0.0f));
    m_Background.SetDirection (coreVector2(-1.0f,0.0f));

    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i)
    {
        m_aTitle[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aTitle[i].SetPosition(coreVector2(0.0f, 0.25f - 0.1f*I_TO_F(i)));
        m_aTitle[i].SetColor3  (COLOR_MENU_BLUE);

        m_aValue[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aValue[i].SetPosition(coreVector2(0.0f, m_aTitle[i].GetPosition().y - 0.03f));
        m_aValue[i].SetColor3  (COLOR_MENU_WHITE);

        for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
        {
            const coreFloat fSide = j ? 1.0f : -1.0f;

            m_aaPart[i][j].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
            m_aaPart[i][j].SetPosition (m_aValue[i].GetPosition() + coreVector2(fSide * 0.3f, 0.0f));
            m_aaPart[i][j].SetAlignment(coreVector2(fSide, 0.0f));
            m_aaPart[i][j].SetColor3   (COLOR_MENU_WHITE);
        }
    }

    m_TotalTitle.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_TotalTitle.SetPosition    (coreVector2(0.0f,-0.15f));
    m_TotalTitle.SetColor3      (COLOR_MENU_BLUE);
    m_TotalTitle.SetTextLanguage("SUMMARY_TOTAL");

    m_TotalValue.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_TotalValue.SetPosition(coreVector2(0.0f, m_TotalTitle.GetPosition().y - 0.04f));
    m_TotalValue.SetColor3  (COLOR_MENU_WHITE);

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        const coreFloat fSide = j ? 1.0f : -1.0f;

        m_aTotalPart[j].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aTotalPart[j].SetPosition (m_TotalValue.GetPosition() + coreVector2(fSide * 0.3f, 0.0f));
        m_aTotalPart[j].SetAlignment(coreVector2(fSide, 0.0f));
        m_aTotalPart[j].SetColor3   (COLOR_MENU_WHITE);
    }

    m_BonusValue.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_BonusValue.SetPosition(coreVector2(0.0f,0.0f));
    m_BonusValue.SetColor3  (COLOR_MENU_WHITE);

    // bind menu objects
    this->BindObject(SURFACE_SUMMARY_RECAP, &m_Background);

    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_RECAP, &m_aTitle[i]);
    this->BindObject(SURFACE_SUMMARY_RECAP, &m_TotalTitle);
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_RECAP, &m_aValue[i]);
    this->BindObject(SURFACE_SUMMARY_RECAP, &m_TotalValue);

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_RECAP, &m_aaPart[i][j]);
        this->BindObject(SURFACE_SUMMARY_RECAP, &m_aTotalPart[j]);
    }

    this->BindObject(SURFACE_SUMMARY_BEGIN, &m_BonusValue);
}


// ****************************************************************
// render the summary menu
void cSummaryMenu::Render()
{
    // 
    cMenu::UpdateAnimateProgram(&m_Background);

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
    case SURFACE_SUMMARY_RECAP:
        {
            // 
            constexpr coreFloat fSpinFrom = (0.5f + 0.4f * I_TO_F(MENU_SUMMARY_ENTRIES));
            constexpr coreFloat fSpinTo   = fSpinFrom + 1.0f;

            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= INTERFACE_BANNER_SPEED_REV) && Core::Input->GetAnyButton(CORE_INPUT_PRESS))
            {
                // 
                     if(m_iState      >= SUMMARY_SKIPPED) m_iState = SUMMARY_OUTRO;     // leave summary
                else if(m_fIntroTimer >= fSpinTo)         m_iState = SUMMARY_OUTRO;
                else if(m_fIntroTimer <  fSpinTo)         m_iState = SUMMARY_SKIPPED;   // skip blend-in
            }

            // 
            if(m_iState == SUMMARY_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= INTERFACE_BANNER_SPEED_REV)
            {
                // 
                m_iStatus = 1;
            }

            if(m_iState != SUMMARY_WAIT)
            {
                // 
                if((m_fIntroTimer >= INTERFACE_BANNER_ANIMATION) && (m_iState < SUMMARY_WAIT)) m_iState = SUMMARY_WAIT;
                STATIC_ASSERT(INTERFACE_BANNER_ANIMATION >= fSpinTo)

                // 
                const coreFloat fBlendIn  = m_iState ? fSpinTo : m_fIntroTimer;
                const coreFloat fBlendOut = 1.0f - m_fOutroTimer * INTERFACE_BANNER_SPEED;

                // 
                const auto nBlendFunc = [&](coreLabel* OUTPUT pTitle, coreLabel* OUTPUT pValue, const coreFloat fThreshold)
                {
                    const coreObjectEnable eEnabled = (fBlendIn >= fThreshold) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                    // blend-in
                    pTitle->SetEnabled(eEnabled);
                    pValue->SetEnabled(eEnabled);

                    // blend-out
                    pTitle->SetAlpha(fBlendOut);
                    pValue->SetAlpha(fBlendOut);
                };
                for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) nBlendFunc(&m_aTitle[i], &m_aValue[i], 0.5f + 0.4f * I_TO_F(i));
                nBlendFunc(&m_TotalTitle, &m_TotalValue, fSpinFrom);

                // spin-up final score value
                m_TotalValue.SetText(PRINT("%07.0f", I_TO_F(m_iFinalScore) * CLAMP(fBlendIn - fSpinFrom, 0.0f, 1.0f)));

                // calculate visibility and animation value
                const coreFloat fVisibility = MIN(m_fIntroTimer, INTERFACE_BANNER_SPEED_REV - m_fOutroTimer) * INTERFACE_BANNER_SPEED;
                const coreFloat fAnimation  = LERPB(0.0f, 1.0f, MIN(m_fIntroTimer / INTERFACE_BANNER_ANIMATION, 1.0f)) * INTERFACE_BANNER_ANIMATION;

                // slash background across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
                const coreBool bLeftRight = m_fOutroTimer ? true : false;
                m_Background.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_Background.GetDirection().yx());
                m_Background.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

                // animate background
                m_Background.SetSize     (coreVector2(fVisibility, 1.0f) * (coreVector2(1.0f,0.45f)));
                m_Background.SetTexSize  (coreVector2(fVisibility, 1.0f));
                m_Background.SetTexOffset(coreVector2(1.0f,1.0f) * (fAnimation * 0.05f));

                // move background manually (to remove some delay)
                m_Background.Move();
            }
        }
        break;

    case SURFACE_SUMMARY_BEGIN:
        {
            // 
            m_fIntroTimer.Update(1.0f);


            

            // 
            m_BonusValue.SetAlpha  (m_BonusValue.GetAlpha() * CLAMP(4.0f - m_fIntroTimer, 0.0f, 1.0f));
            m_BonusValue.SetEnabled((m_fIntroTimer >= 2.0f) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // 
            m_BonusValue.SetText(PRINT("%07.0f", I_TO_F(m_iFinalScore) * CLAMP(m_fIntroTimer - 2.0f, 0.0f, 1.0f)));


            if(m_iState == SUMMARY_INTRO)
            {
                g_pPostProcessing->SetValue(CLAMP(2.0f - m_fIntroTimer, 0.0f, 1.0f));

                if(m_fIntroTimer >= 4.0f)
                {
                    // 
                    m_iStatus = 1;

                    m_iState = SUMMARY_OUTRO;

                    
                g_pPostProcessing->SetSaturation(1.0f);
                g_pPostProcessing->SetValue     (1.0f);


                    g_pEnvironment->ChangeBackground(cNoBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);

                    // prevent flickering  
                    g_pGlow->Clear();
                }
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
void cSummaryMenu::ShowRecap()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_iFinalScore = 0u;
    m_fIntroTimer = 0.0f;
    m_fOutroTimer = 0.0f;
    m_iState      = SUMMARY_INTRO;

    // 
    m_Background.SetSize(coreVector2(0.0f,0.0f));

    // 
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i)
        m_aTitle[i].SetText(g_pGame->GetCurMission()->GetBoss(i)->GetName());

    if(g_pGame->GetCoop())
    {
        // 
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i)
        {
            coreUint32 iSum = 0u;

            // 
            for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
            {
                const coreUint32 iPart = g_pGame->GetPlayer(j)->GetScoreTable()->GetScoreBoss(g_pGame->GetCurMissionIndex(), i);

                // 
                iSum += iPart;
                m_aaPart[i][j].SetText(PRINT("%07u", iPart));
            }

            // 
            m_iFinalScore += iSum;
            m_aValue[i].SetText(PRINT("%07u", iSum));
        }
    }
    else
    {
        // 
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i)
        {
            const coreUint32 iSum = g_pGame->GetPlayer(0u)->GetScoreTable()->GetScoreBoss(g_pGame->GetCurMissionIndex(), i);

            // 
            m_iFinalScore += iSum;
            m_aValue[i].SetText(PRINT("%07u", iSum));
        }
    }

    // 
    this->ChangeSurface(SURFACE_SUMMARY_RECAP, 0.0f);
}


// ****************************************************************
// 
void cSummaryMenu::ShowBegin()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    m_iFinalScore = 0u;
    m_fIntroTimer = 0.0f;
    m_fOutroTimer = 0.0f;
    m_iState      = SUMMARY_INTRO;

    // 
    g_pGame->ForEachPlayerAll([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        m_iFinalScore += pPlayer->GetScoreTable()->GetScoreMission(g_pGame->GetCurMissionIndex());
    });

    // 
    this->ChangeSurface(SURFACE_SUMMARY_BEGIN, 0.0f);
}