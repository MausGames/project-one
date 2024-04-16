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
    m_ThankYouText.SetPosition    (coreVector2(0.0f,0.05f));
    m_ThankYouText.SetCenter      (m_Background.GetCenter());
    m_ThankYouText.SetColor3      (COLOR_MENU_WHITE);
    m_ThankYouText.SetTextLanguage(g_bDemoVersion ? "THANK_YOU_DEMO" : "THANK_YOU");

    m_TotalName.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_TotalName.SetPosition    (coreVector2(0.0f,-0.025f));
    m_TotalName.SetCenter      (m_Background.GetCenter());
    m_TotalName.SetColor3      (COLOR_MENU_WHITE);
    m_TotalName.SetTextLanguage("FINISH_TOTAL");

    m_TotalValue.Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_TotalValue.SetPosition(coreVector2(0.0f, m_TotalName.GetPosition().y - 0.03f));
    m_TotalValue.SetCenter  (m_Background.GetCenter());
    m_TotalValue.SetColor3  (COLOR_MENU_WHITE);

    for(coreUintW i = 0u; i < MENU_FINISH_PARTS; ++i)
    {
        const coreFloat fSide = i ? 1.0f : -1.0f;

        m_aTotalPart[i].Construct  (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aTotalPart[i].SetPosition(m_TotalValue.GetPosition() + coreVector2(fSide * 0.36f, 0.0f));
        m_aTotalPart[i].SetCenter  (m_Background.GetCenter());
        m_aTotalPart[i].SetColor3  (COLOR_MENU_WHITE);
    }

    m_SteamButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_SteamButton.DefineProgram("menu_border_program");
    m_SteamButton.SetPosition  (coreVector2(0.0f,-0.15f));
    m_SteamButton.SetSize      (coreVector2(0.4f,0.07f));
    m_SteamButton.GetCaption()->SetTextLanguage("TO_STEAM");

    m_ExitButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_ExitButton.DefineProgram("menu_border_program");
    m_ExitButton.SetPosition  (m_SteamButton.GetPosition() + coreVector2(0.0f,-0.09f));
    m_ExitButton.SetSize      (m_SteamButton.GetSize());
    m_ExitButton.GetCaption()->SetTextLanguage("EXIT_GAME");

    // 
    m_Navigator.BindObject(&m_SteamButton, &m_ExitButton,  NULL, &m_ExitButton,  NULL, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_ExitButton,  &m_SteamButton, NULL, &m_SteamButton, NULL, NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignFirst(&m_SteamButton);

    // bind menu objects
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_Background);
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_ThankYouText);
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_TotalName);
    this->BindObject(SURFACE_FINISH_DEFAULT, &m_TotalValue);

    for(coreUintW i = 0u; i < MENU_SUMMARY_PARTS; ++i) this->BindObject(SURFACE_FINISH_DEFAULT, &m_aTotalPart[i]);

    this->BindObject(SURFACE_FINISH_DEFAULT, &m_SteamButton);
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
                if(m_SteamButton.IsClicked())
                {
                    // 
                    SDL_OpenURL("https://store.steampowered.com/app/1624320/Eigengrau/");
                }
                else if(m_ExitButton.IsClicked())
                {
                    // 
                    if(m_eState == FINISH_WAIT) m_eState = FINISH_OUTRO;
                }
            }
            
            if((fOldTimer < 0.0f) && (m_fIntroTimer >= 0.0f))
            {
                g_MusicPlayer.SelectName("menu.ogg");
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
                m_Background.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_Background.GetDirection().yx());
                m_Background.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

                // animate background
                m_Background.SetSize     (coreVector2(fVisibility, 1.0f) * coreVector2(1.0f,0.3f));
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
                m_SteamButton.SetAlpha(fVisibility);
                m_ExitButton .SetAlpha(fVisibility);
                m_Navigator  .SetAlpha(fVisibility);
            }

            // 
            cMenu::UpdateButton(&m_SteamButton, m_SteamButton.IsFocused());
            cMenu::UpdateButton(&m_ExitButton,  m_ExitButton .IsFocused());
        }
        break;

    default:
        ASSERT(false)
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
    coreUint32 iSumScore = 0u;
    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint32 iScore = pPlayer->GetScoreTable()->GetScoreTotal();

        // 
        m_aTotalPart[i].SetText(g_pGame->IsMulti() ? coreData::ToChars(iScore) : "");

        // 
        iSumScore += iScore;
    });

    // 
    m_TotalValue.SetText(coreData::ToChars(iSumScore));

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