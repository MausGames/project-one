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
cDefeatMenu::cDefeatMenu()noexcept
: coreMenu      (SURFACE_DEFEAT_MAX, SURFACE_DEFEAT_CONTINUE)
, m_fCountdown  (0.0f)
, m_fBurst      (0.0f)
, m_fIntroTimer (0.0f)
, m_fOutroTimer (0.0f)
, m_eState      (DEFEAT_INTRO)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_detail_04.png");
    m_Background.DefineTexture(1u, "menu_background_black.png");
    m_Background.DefineProgram("menu_animate_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetCenter    (coreVector2(0.0f,0.1f));

    m_GameOverText.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_GameOverText.SetPosition    (coreVector2(0.0f,0.0f));
    m_GameOverText.SetCenter      (m_Background.GetCenter());
    m_GameOverText.SetTextLanguage("DEFEAT_GAMEOVER");

    m_ContinueText.Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_SMALL);
    m_ContinueText.SetPosition    (coreVector2(0.0f,-0.1f));
    m_ContinueText.SetTextLanguage("DEFEAT_CONTINUE");

    m_ContinueTimer.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_ContinueTimer.SetPosition(m_ContinueText.GetPosition() + coreVector2(0.0f,-0.05f));

    for(coreUintW i = 0u; i < MENU_DEFEAT_CONTINUES; ++i)
    {
        m_ContinueImage[i].DefineTexture(0u, g_pSpecialEffects->GetIconTexture(0u));
        m_ContinueImage[i].DefineProgram("default_2d_program");
        m_ContinueImage[i].SetSize      (coreVector2(1.0f,1.0f) * 0.058f);
        m_ContinueImage[i].SetTexSize   (ICON_TEXSIZE);
        m_ContinueImage[i].SetTexOffset (ICON_TEXOFFSET * I_TO_F(i + 1u));
    }

    // bind menu objects
    this->BindObject(SURFACE_DEFEAT_CONTINUE, &m_Background);
    this->BindObject(SURFACE_DEFEAT_CONTINUE, &m_GameOverText);
    this->BindObject(SURFACE_DEFEAT_CONTINUE, &m_ContinueText);
    this->BindObject(SURFACE_DEFEAT_CONTINUE, &m_ContinueTimer);

    for(coreUintW i = 0u; i < MENU_DEFEAT_CONTINUES; ++i) this->BindObject(SURFACE_DEFEAT_CONTINUE, &m_ContinueImage[i]);

    this->BindObject(SURFACE_DEFEAT_GAMEOVER, &m_Background);
    this->BindObject(SURFACE_DEFEAT_GAMEOVER, &m_GameOverText);
}


// ****************************************************************
// render the defeat menu
void cDefeatMenu::Render()
{
    // 
    cMenu::UpdateAnimateProgram(&m_Background);

    // 
    this->coreMenu::Render();
}


// ****************************************************************
// move the defeat menu
void cDefeatMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_DEFEAT_CONTINUE:
        {
            
            for(coreUintW i = 0u; i < MENU_DEFEAT_CONTINUES; ++i)
                m_ContinueImage[i].SetDirection(coreVector2::Direction(coreFloat(Core::System->GetTotalTime()) + (0.8f*PI) * (I_TO_F(i) / I_TO_F(MENU_DEFEAT_CONTINUES))));
            
            // 
            m_fIntroTimer.Update(1.0f);
            if(m_fIntroTimer >= MENU_DEFEAT_BANNER_SPEED_REV)
            {
                if(HAS_BIT(g_TotalInput.iActionPress, 0u))
                {
                    // 
                    if(m_eState < DEFEAT_BURST)
                    {
                        m_eState = DEFEAT_BURST;
                        g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_CONTINUE_ACCEPT);
                    }
                }
                else if(Core::Input->GetAnyButton(CORE_INPUT_PRESS))
                {
                    // 
                    m_fCountdown -= FRACT(m_fCountdown) - CORE_MATH_PRECISION;
                }
            }

            // 
            if(m_eState == DEFEAT_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= MENU_DEFEAT_BANNER_SPEED_REV)
            {
                // 
                m_iStatus = 1;
            }

            if(m_eState < DEFEAT_BURST)
            {
                const coreFloat fOldCountdown = m_fCountdown;

                // 
                m_fCountdown.Update(-0.5f);
                if(m_fCountdown <= 0.0f)
                {
                    // 
                    this->ChangeSurface(SURFACE_DEFEAT_GAMEOVER, 1.0f);
                }

                // 
                m_ContinueTimer.SetText(PRINT("%.0f", MAX(FLOOR(m_fCountdown.ToFloat()), 0.0f)));

                // 
                if((fOldCountdown >= 0.0f) && (fOldCountdown < 11.0f) && (F_TO_SI(fOldCountdown) != F_TO_SI(m_fCountdown)))
                    g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_CONTINUE_TICK);
            }
            else
            {
                // 
                m_fBurst.Update(0.7f);
                if(m_fBurst >= 1.0f) m_eState = DEFEAT_OUTRO;

                // 
                const coreUint8 iContinues = STATIC_ISVALID(g_pGame) ? g_pGame->GetContinues() : 0u;
                if(iContinues)
                {
                    // 
                    static const coreSpline2 s_Spline = []()
                    {
                        coreSpline2 oSpline(3u);

                        oSpline.AddNode(coreVector2(-0.05f, 0.0f),  coreVector2(0.0f,-1.0f));
                        oSpline.AddNode(coreVector2( 0.0f, -0.05f), coreVector2(1.0f, 0.0f));
                        oSpline.AddNode(coreVector2( 0.05f, 0.8f),  coreVector2(0.0f, 1.0f));
                        oSpline.Refine();

                        return oSpline;
                    }();

                    // 
                    cGuiObject&       oTarget = m_ContinueImage[iContinues - 1u];
                    const cGuiObject& oSource = m_ContinueImage[iContinues % MENU_DEFEAT_CONTINUES];
                    oTarget.SetPosition(coreVector2(oTarget.GetPosition().x, oSource.GetPosition().y + s_Spline.CalcPositionLerp(LERPBR(0.0f, 1.0f, MIN(m_fBurst, 1.0f))).y));

                    STATIC_ASSERT(MENU_DEFEAT_CONTINUES > 1u)
                }
            }
        }
        break;

    case SURFACE_DEFEAT_GAMEOVER:
        {
            // 
            m_fIntroTimer.Update(1.0f);
            if((m_fIntroTimer >= MENU_DEFEAT_BANNER_SPEED_REV) && !this->GetTransition().GetStatus() && Core::Input->GetAnyButton(CORE_INPUT_PRESS))
            {
                // 
                m_eState = DEFEAT_OUTRO;
                if(STATIC_ISVALID(g_pGame))
                {
                    g_pGame->GetInterface ()->SetVisible(false);
                    g_pGame->GetCombatText()->SetVisible(false);
                }
            }

            // 
            if(m_eState == DEFEAT_OUTRO) m_fOutroTimer.Update(1.0f);
            if(m_fOutroTimer >= MENU_DEFEAT_BANNER_SPEED_REV + MENU_DEFEAT_DELAY_OUTRO)
            {
                // 
                m_iStatus = 2;
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(m_eState != DEFEAT_WAIT)
    {
        // 
        if((m_fIntroTimer >= MENU_DEFEAT_BANNER_ANIMATION) && (m_eState < DEFEAT_WAIT)) m_eState = DEFEAT_WAIT;

        // calculate visibility and animation value
        const coreFloat fVisibility = MAX0(MIN(m_fIntroTimer, MENU_DEFEAT_BANNER_SPEED_REV - m_fOutroTimer)) * MENU_DEFEAT_BANNER_SPEED;
        const coreFloat fAnimation  = BLENDB(MIN1(m_fIntroTimer / MENU_DEFEAT_BANNER_ANIMATION)) * MENU_DEFEAT_BANNER_ANIMATION;

        // slash background across screen (# direction can be swapped, also alpha value is used as texture coordinate correction)
        const coreBool bLeftRight = m_fOutroTimer ? true : false;
        m_Background.SetPosition ((bLeftRight ?        0.5f : -0.5f) * (1.0f-fVisibility) * m_Background.GetDirection().yx());
        m_Background.SetAlpha    ( bLeftRight ? fVisibility :  1.0f);

        // animate background
        m_Background.SetSize     (coreVector2(fVisibility, 1.0f) * coreVector2(1.0f,0.21f));
        m_Background.SetTexSize  (coreVector2(fVisibility, 1.0f));
        m_Background.SetTexOffset(coreVector2(1.0f,1.0f) * (fAnimation * 0.05f));

        // move background manually (to remove some delay)
        m_Background.Move();

        // 
        m_GameOverText .SetAlpha(fVisibility);
        m_ContinueText .SetAlpha(fVisibility);
        m_ContinueTimer.SetAlpha(fVisibility);
        for(coreUintW i = 0u; i < MENU_DEFEAT_CONTINUES; ++i) m_ContinueImage[i].SetAlpha(fVisibility);
    }

    // 
    g_pPostProcessing->SetSaturationAll(MIN(m_fOutroTimer * MENU_DEFEAT_BANNER_SPEED, 1.0f));
}


// ****************************************************************
// 
void cDefeatMenu::ShowContinue()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();
    m_fCountdown = 11.0f;

    // 
    const coreUint8 iContinues = g_pGame->GetContinues();
    ASSERT(iContinues)

    // 
    m_ContinueTimer.SetText("");

    // 
    for(coreUintW i = 0u; i < MENU_DEFEAT_CONTINUES; ++i)
    {
        m_ContinueImage[i].SetPosition(m_ContinueText.GetPosition() + coreVector2((I_TO_F(i) - 0.5f * I_TO_F(iContinues-1u)) * 0.055f, -0.11f));
        m_ContinueImage[i].SetEnabled ((iContinues > i) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    this->ChangeSurface(SURFACE_DEFEAT_CONTINUE, 0.0f);
}


// ****************************************************************
// 
void cDefeatMenu::ShowGameOver()
{
    ASSERT(STATIC_ISVALID(g_pGame))

    // 
    this->__ResetState();
    m_fIntroTimer = -MENU_DEFEAT_DELAY_INTRO;

    // 
    this->ChangeSurface(SURFACE_DEFEAT_GAMEOVER, 0.0f);
}


// ****************************************************************
// 
void cDefeatMenu::__ResetState()
{
    // 
    m_fCountdown  = 0.0f;
    m_fBurst      = 0.0f;
    m_fIntroTimer = 0.0f;
    m_fOutroTimer = 0.0f;
    m_eState      = DEFEAT_INTRO;

    // 
    m_Background.SetSize(coreVector2(0.0f,0.0f));
    m_Background.Move();

    // 
    this->SetAlpha(0.0f);
}