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
cFinishMenu::cFinishMenu()noexcept
: coreMenu      (SURFACE_FINISH_MAX, SURFACE_FINISH_DEFAULT)
, m_fIntroTimer (0.0f)
, m_fOutroTimer (0.0f)
, m_eState      (FINISH_INTRO)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_detail_04.png");
    m_Background.DefineTexture(1u, "menu_background_black.png");
    m_Background.DefineProgram("menu_animate_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetCenter    (coreVector2(0.0f,0.1f));

    m_ThankYouText.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_ThankYouText.SetPosition    (coreVector2(0.0f,0.06f));
    m_ThankYouText.SetCenter      (m_Background.GetCenter());
    m_ThankYouText.SetColor3      (COLOR_MENU_WHITE);
    m_ThankYouText.SetTextLanguage(g_bDemoVersion ? "THANK_YOU_DEMO" : "THANK_YOU");

    m_TotalName.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_TotalName.SetPosition    (coreVector2(0.0f,-0.015f));
    m_TotalName.SetCenter      (m_Background.GetCenter());
    m_TotalName.SetColor3      (COLOR_MENU_WHITE);
    m_TotalName.SetTextLanguage("FINISH_TOTAL");

    m_TotalValue.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_TotalValue.SetPosition(coreVector2(0.0f, m_TotalName.GetPosition().y - 0.05f));
    m_TotalValue.SetCenter  (m_Background.GetCenter());
    m_TotalValue.SetColor3  (COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_FINISH_PARTS; ++i)
    {
        const coreFloat fSide = i ? 1.0f : -1.0f;

        m_aTotalPart[i].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aTotalPart[i].SetPosition(m_TotalValue.GetPosition() + coreVector2(fSide * 0.36f, 0.0f));
        m_aTotalPart[i].SetCenter  (m_Background.GetCenter());
        m_aTotalPart[i].SetColor3  (COLOR_MENU_WHITE);
    }

    m_StoreButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StoreButton.DefineProgram("menu_border_program");
    m_StoreButton.SetPosition  (coreVector2(0.0f,-0.15f));
    m_StoreButton.SetSize      (coreVector2(0.4f,0.07f));
    m_StoreButton.GetCaption()->SetTextLanguage(cMenu::GetStoreText());

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_StoreButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_StoreButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    // 
    m_Navigator.BindObject(NULL,           &m_StoreButton,  NULL, &m_StoreButton,  NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_StoreButton, &m_ExitButton,   NULL, &m_ExitButton,   NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ExitButton,  &m_StoreButton,  NULL, &m_StoreButton,  NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignMenu(this);
    m_Navigator.ShowIcon  (true);

    // bind menu objects
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_Background);
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_ThankYouText);
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_TotalName);
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_TotalValue);

    for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) this->BindObject(SURFACE_FINISH_DEFAULT, &m_aTotalPart[i]);

    this->BindObject(SURFACE_FINISH_DEFAULT, &m_StoreButton);
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_ExitButton);

    this->BindObject(SURFACE_FINISH_DEFAULT, &m_Navigator);
}


// ****************************************************************
// render the finish menu
void cFinishMenu::Render()
{
    // 
    cMenu::UpdateAnimateProgram(&m_Background);

    // 
    this->coreMenu::Render();
}


// ****************************************************************
// move the finish menu
void cFinishMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_FINISH_DEFAULT:
        {
            const coreFloat fOldTimer = m_fIntroTimer;
            
            
            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= MENU_FINISH_BANNER_SPEED_REV))// && Core::Input->GetAnyButton(CORE_INPUT_PRESS))
            {
                if(m_StoreButton.IsClicked())
                {
                    // 
                    cMenu::OpenStoreLink();
                }
                else if(m_ExitButton.IsClicked())
                {
                    // 
                    if(m_eState == FINISH_WAIT) m_eState = FINISH_OUTRO;   // TODO 1: verhindert, dass man exit drueckt waerend banner noch animiert
                }
            }
            
            // 
            if(m_fIntroTimer >= MENU_FINISH_BANNER_SPEED_REV) m_Navigator.SetActive(true);
            
            if((fOldTimer < 0.0f) && (m_fIntroTimer >= 0.0f))
            {
                g_MusicPlayer.SelectName("menu");
                g_MusicPlayer.Play();
            }

            // 
            if(m_eState == FINISH_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= MENU_FINISH_BANNER_SPEED_REV)
            {
                // 
                m_iStatus = 1;
            }

            if(m_eState != FINISH_WAIT)
            {
                // 
                if((m_fIntroTimer >= MENU_FINISH_BANNER_ANIMATION) && (m_eState < FINISH_WAIT)) m_eState = FINISH_WAIT;

                // calculate visibility and animation value
                const coreFloat fVisibility = MAX0(MIN(m_fIntroTimer, MENU_FINISH_BANNER_SPEED_REV - m_fOutroTimer)) * MENU_FINISH_BANNER_SPEED;
                const coreFloat fAnimation  = BLENDB(MIN1(m_fIntroTimer / MENU_FINISH_BANNER_ANIMATION)) * MENU_FINISH_BANNER_ANIMATION;

                // slash background across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
                const coreBool bLeftRight = m_fOutroTimer ? true : false;
                m_Background.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_Background.GetDirection().Rotated90());
                m_Background.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

                // animate background
                m_Background.SetSize     (coreVector2(fVisibility, 1.0f) * coreVector2(1.0f,0.32f));
                m_Background.SetTexSize  (coreVector2(fVisibility, 1.0f));
                m_Background.SetTexOffset(coreVector2(1.0f,1.0f) * (fAnimation * 0.05f));

                // move background manually (to remove some delay)
                m_Background.Move();

                // 
                m_ThankYouText.SetAlpha(fVisibility);
                m_TotalName   .SetAlpha(fVisibility);
                m_TotalValue  .SetAlpha(fVisibility);
                for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) m_aTotalPart[i].SetAlpha(fVisibility);

                // 
                m_StoreButton.SetAlpha(fVisibility);
                m_ExitButton .SetAlpha(fVisibility);
                m_Navigator  .SetAlpha(fVisibility);
            }

            // 
            cMenu::UpdateButton(&m_StoreButton, &m_Navigator, m_StoreButton.IsFocused());
            cMenu::UpdateButton(&m_ExitButton,  &m_Navigator, m_ExitButton .IsFocused());
        }
        break;

    default:
        UNREACHABLE
        break;
    }
}


// ****************************************************************
// 
void cFinishMenu::ShowThankYou()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();
    m_fIntroTimer = -MENU_FINISH_DELAY_INTRO - 1.5f;

    // 
    coreUint32 aiScoreFull [TABLE_RUNS + 1u]                    = {};
    coreUint32 aaiScorePart[TABLE_RUNS + 1u][MENU_FINISH_PARTS] = {};
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        for(coreUintW j = 0u; j < TABLE_RUNS + 1u; ++j)
        {
            const coreUint32 iScore = (j < TABLE_RUNS) ? pPlayer->GetScoreTable()->GetRunTotal(j) : pPlayer->GetScoreTable()->GetScoreTotal();

            aiScoreFull [j]    += iScore;
            aaiScorePart[j][i] += iScore;
        }
    });

    // 
    const coreUintW iMaxIndex = std::max_element(aiScoreFull, aiScoreFull + TABLE_RUNS + 1u) - aiScoreFull;

    // 
    m_TotalValue.SetText(coreData::ToChars(aiScoreFull[iMaxIndex]));

    // 
    for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i)
    {
        m_aTotalPart[i].SetText(g_pGame->IsMulti() ? coreData::ToChars(aaiScorePart[iMaxIndex][i]) : "");
    }

    // 
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->GetDataTable()->GiveMedalArade(MEDAL_BRONZE);
    });

    // 
    g_pSave->EditLocalStatsArcade()->iScoreBest   = MAX(g_pSave->EditLocalStatsArcade()->iScoreBest,       aiScoreFull[iMaxIndex]);
    g_pSave->EditLocalStatsArcade()->iScoreWorst  = MIN(g_pSave->EditLocalStatsArcade()->iScoreWorst - 1u, aiScoreFull[iMaxIndex] - 1u) + 1u;
    g_pSave->EditLocalStatsArcade()->iScoreTotal += aiScoreFull[iMaxIndex];

    // 
    g_pSave->SaveFile();

    // 
    m_Navigator.ResetFirst();
    m_Navigator.SetActive(false);

    // 
    this->ChangeSurface(SURFACE_FINISH_DEFAULT, 0.0f);
}


// ****************************************************************
// 
void cFinishMenu::SetHighlightColor(const coreVector3 vColor)
{
    // 
    m_TotalName.SetColor3(vColor);
}


// ****************************************************************
// 
void cFinishMenu::__ResetState()
{
    // 
    m_fIntroTimer = 0.0f;
    m_fOutroTimer = 0.0f;
    m_eState      = FINISH_INTRO;

    // 
    m_Background.SetSize(coreVector2(0.0f,0.0f));
    m_Background.Move();

    // 
    this->SetAlpha(0.0f);
}