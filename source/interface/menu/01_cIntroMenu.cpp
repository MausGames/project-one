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
cIntroMenu::cIntroMenu()noexcept
: coreMenu       (SURFACE_INTRO_MAX, SURFACE_INTRO_EMPTY)
, m_IntroTimer   (coreTimer(3.0f, 0.5f, 1u))
, m_iIntroStatus (0xFFu)
{
    // create menu objects
    m_WelcomeText.Construct      (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_WelcomeText.SetPosition    (coreVector2(0.0f,0.0f));
    m_WelcomeText.SetColor3      (COLOR_MENU_INSIDE);
    m_WelcomeText.SetTextLanguage("WELCOME");

    // bind menu objects
    this->BindObject(SURFACE_INTRO_WELCOME, &m_WelcomeText);
}


// ****************************************************************
// destructor
cIntroMenu::~cIntroMenu()
{
    // 
    FOR_EACH(it, m_apLanguageButton) SAFE_DELETE(*it)
    m_apLanguageButton.clear();
}


// ****************************************************************
// move the intro menu
void cIntroMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    if(this->GetCurSurface() == SURFACE_INTRO_LANGUAGE)
    {
        // 
        FOR_EACH(it, m_apLanguageButton)
        {
            if((*it)->IsClicked())
            {
                const coreChar* pcPath = m_apLanguageButton.get_key(it)->c_str();

                // 
                Core::Language->Load(pcPath);
                Core::Config->SetString(CORE_CONFIG_BASE_LANGUAGE, pcPath);
                Core::Config->Save();

                // 
                cMenu::UpdateLanguageFont();

                // 
                this->ChangeSurface(SURFACE_INTRO_EMPTY, 1.0f);
                break;
            }
        }

        // 
        FOR_EACH(it, m_apLanguageButton)
        {
            cMenu::UpdateButton((*it), &m_Navigator, (*it)->IsFocused());
        }
    }
    else if(this->GetOldSurface() == SURFACE_INTRO_LANGUAGE)
    {
        // 
        if(!this->GetTransition().GetStatus()) g_pMenu->ShiftSurface(this, SURFACE_INTRO_WELCOME, 1.0f, 0u);
    }
    else
    {
        // 
        m_IntroTimer.Update(1.0f);

        if(m_IntroTimer.GetStatus())
        {
            if(m_iIntroStatus == 1u)
            {
                // 
                if(Core::Input->GetAnyButton(CORE_INPUT_PRESS))
                {
                    // 
                    m_iIntroStatus = 2u;
                    //if(m_IntroTimer.GetValue(CORE_TIMER_GET_NORMAL) < 2.0f)
                    //{
                    //    m_IntroTimer.SetValue(2.0f);
                    //    this->ChangeSurface(SURFACE_INTRO_EMPTY, 0.0f);
                    //}
                }
            }
            else if(m_iIntroStatus == 2u)
            {
                // 
                const coreFloat fTime1 = m_IntroTimer.GetValue(CORE_TIMER_GET_NORMAL);
                if(((fTime1 >= 0.7f) && (fTime1 < 1.0f)) ||
                   ((fTime1 >= 1.7f) && (fTime1 < 2.0f)))
                {
                    // 
                    m_IntroTimer.SetValue(2.0f);
                }
            }

            // 
            const coreFloat fTime2 = m_IntroTimer.GetValue(CORE_TIMER_GET_NORMAL);
            if(fTime2 >= 2.0f)
            {
                // 
                m_iStatus = 1;

                // 
                g_pEnvironment->Activate();
                g_pPostProcessing->SetWallOpacity(CLAMP01(1.35f * (fTime2 - 2.0f)));
            }
            if(fTime2 >= 1.0f)
            {
                // 
                g_pMenu->ShiftSurface(this, SURFACE_INTRO_EMPTY, 0.75f, 0u);
            }
        }
    }
}


// ****************************************************************
// 
void cIntroMenu::StartIntro()
{
    // 
    m_IntroTimer.Play(CORE_TIMER_PLAY_RESET);
    m_iIntroStatus = 1u;

    // 
    const coreBool bSelectLanguage = (Core::Language->GetPath()[0] == '\0');
    if(bSelectLanguage)
    {
        // 
        const coreMap<coreString, coreString>& asLanguageList = cMenu::GetLanguageList();
        ASSERT(asLanguageList.size() <= 10u)

        // 
        const coreFloat fOffset = I_TO_F(asLanguageList.size()) * 0.5f - 0.5f;
        FOR_EACH(it, asLanguageList)
        {
            // 
            coreString sFont;
            if(!coreLanguage::FindString(it->c_str(), "FONT", &sFont)) sFont = MENU_FONT_DEFAULT;

            // create new language button
            cGuiButton* pButton = new cGuiButton();
            pButton->Construct    (MENU_BUTTON, coreData::StrFilename(sFont.c_str()), 20u, MENU_OUTLINE_SMALL);   // # always filter filename
            pButton->DefineProgram("menu_border_program");
            pButton->SetPosition  (coreVector2(0.0f,0.09f * (fOffset - I_TO_F(m_apLanguageButton.size()))));
            pButton->SetSize      (coreVector2(0.4f,0.07f));
            pButton->GetCaption()->SetText(asLanguageList.get_key(it)->c_str());

            // 
            this->BindObject(SURFACE_INTRO_LANGUAGE, pButton);

            // 
            m_apLanguageButton.emplace((*it), pButton);
        }

        // 
        for(coreUintW i = 0u, ie = m_apLanguageButton.size(); i < ie; ++i)
        {
            cGuiButton* pUp   = m_apLanguageButton.get_valuelist().at((i + ie - 1u) % ie);
            cGuiButton* pDown = m_apLanguageButton.get_valuelist().at((i + 1u)      % ie);

            m_Navigator.BindObject(m_apLanguageButton.get_valuelist().at(i), pUp, NULL, pDown, NULL, MENU_TYPE_DEFAULT);
        }
        m_Navigator.AssignFirst(m_apLanguageButton.front());
        m_Navigator.AssignMenu (this);
        m_Navigator.ShowIcon   (true);

        // 
        this->BindObject(SURFACE_INTRO_LANGUAGE, &m_Navigator);
    }

    // 
    g_pMenu->ShiftSurface(this, bSelectLanguage ? SURFACE_INTRO_LANGUAGE : SURFACE_INTRO_WELCOME, 0.75f, 0u);
}


// ****************************************************************
// 
void cIntroMenu::ActivateFirstPlay()
{
    // 
    m_iIntroStatus = 0u;

    // 
    m_WelcomeText.SetText("");
}