//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cSummaryMenu::cSummaryMenu()noexcept
: coreMenu      (1u, SURFACE_SUMMARY_DEFAULT)
, m_iFinalScore (0u)
, m_fIntroTimer (0.0f)
, m_fOutroTimer (0.0f)
, m_iSkipped    (0u)
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

    // bind menu objects
    this->BindObject(SURFACE_SUMMARY_DEFAULT, &m_Background);

    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_DEFAULT, &m_aTitle[i]);
    this->BindObject(SURFACE_SUMMARY_DEFAULT, &m_TotalTitle);
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_DEFAULT, &m_aValue[i]);
    this->BindObject(SURFACE_SUMMARY_DEFAULT, &m_TotalValue);

    for(coreUintW j = 0u; j < MENU_SUMMARY_PARTS; ++j)
    {
        for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) this->BindObject(SURFACE_SUMMARY_DEFAULT, &m_aaPart[i][j]);
        this->BindObject(SURFACE_SUMMARY_DEFAULT, &m_aTotalPart[j]);
    }
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
    case SURFACE_SUMMARY_DEFAULT:
        {
            // 
            constexpr coreFloat fFull     = (1.0f / INTERFACE_BANNER_SPEED);
            constexpr coreFloat fSpinFrom = (0.5f + 0.4f * I_TO_F(MENU_SUMMARY_ENTRIES));
            constexpr coreFloat fSpinTo   = fSpinFrom + 1.0f;

            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= fFull) && (Core::Input->GetAnyButton(CORE_INPUT_PRESS)))
            {
                // 
                     if(m_iSkipped    >= 1u)      m_iSkipped = 3u;   // leave summary
                else if(m_fIntroTimer >= fSpinTo) m_iSkipped = 3u;
                else if(m_fIntroTimer <  fSpinTo) m_iSkipped = 1u;   // skip blend-in
            }

            // 
            if(m_iSkipped == 3u) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= fFull)
            {
                // 
                m_iStatus = 1;
            }

            if(m_iSkipped != 2u)
            {
                // 
                if((m_fIntroTimer >= fSpinTo) && (m_iSkipped < 2u)) m_iSkipped = 2u;

                // 
                const coreFloat fBlendIn  = m_iSkipped ? fSpinTo : m_fIntroTimer;
                const coreFloat fBlendOut = 1.0f - m_fOutroTimer * INTERFACE_BANNER_SPEED;

                // 
                auto nBlendFunc = [&](coreLabel* OUTPUT pTitle, coreLabel* OUTPUT pValue, const coreFloat fThreshold)
                {
                    const coreObjectEnable iEnable = (fBlendIn >= fThreshold) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

                    // blend-in
                    pTitle->SetEnabled(iEnable);
                    pValue->SetEnabled(iEnable);

                    // blend-out
                    pTitle->SetAlpha(fBlendOut);
                    pValue->SetAlpha(fBlendOut);
                };
                for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i) nBlendFunc(&m_aTitle[i], &m_aValue[i], 0.5f + 0.4f * I_TO_F(i));
                nBlendFunc(&m_TotalTitle, &m_TotalValue, fSpinFrom);

                // spin-up final score value
                m_TotalValue.SetText(PRINT("%07.0f", I_TO_F(m_iFinalScore) * CLAMP(fBlendIn - fSpinFrom, 0.0f, 1.0f)));

                // calculate visibility and animation value
                const coreFloat fVisibility = MIN(m_fIntroTimer, fFull - m_fOutroTimer) * INTERFACE_BANNER_SPEED;
                const coreFloat fAnimation  = LERPB(0.0f, INTERFACE_BANNER_ANIMATION, MIN(m_fIntroTimer / INTERFACE_BANNER_ANIMATION, 1.0f));

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

    default:
        ASSERT(false)
        break;
    }
}


// ****************************************************************
// 
void cSummaryMenu::ShowSummary()
{
    ASSERT(g_pGame)
    const cMission* pMission = g_pGame->GetCurMission();

    // 
    m_iFinalScore = 0u;
    m_fIntroTimer = 0.0f;
    m_fOutroTimer = 0.0f;
    m_iSkipped    = 0u;

    // 
    m_Background.SetSize(coreVector2(0.0f,0.0f));

    // 
    for(coreUintW i = 0u; i < MENU_SUMMARY_ENTRIES; ++i)
        m_aTitle[i].SetText(pMission->GetBoss(i)->GetName());

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
    this->ChangeSurface(SURFACE_SUMMARY_DEFAULT, 0.0f);
}