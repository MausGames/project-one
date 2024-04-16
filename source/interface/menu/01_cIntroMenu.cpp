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
cIntroMenu::cIntroMenu()noexcept
: coreMenu      (3u, SURFACE_INTRO_EMPTY)
, m_IntroTimer  (coreTimer(2.5f, 0.5f, 1u))
, m_fIntroAlpha (1.0f)
, m_iSelected   (0u)
{
    // create menu objects
    m_MausLogo.DefineTexture(0u, "maus_logo.png");
    m_MausLogo.DefineProgram("default_2d_program");
    m_MausLogo.SetPosition  (coreVector2(0.0f,0.0f));
    m_MausLogo.SetSize      (coreVector2(2.0f,1.0f) * 0.2f);

    // bind menu objects
    this->BindObject(SURFACE_INTRO_LOGO, &m_MausLogo);

    // 
    const coreBool bSelectLanguage = Core::Language->GetPath()[0] ? false : true;
    if(bSelectLanguage)
    {
        // 
        const coreLookup<std::string, std::string>& asLanguageList = cMenu::GetLanguageList();

        // 
        const coreFloat fOffset = I_TO_F(asLanguageList.size()) * 0.5f - 0.5f;
        FOR_EACH(it, asLanguageList)
        {
            // 
            std::string sFont;
            if(!coreLanguage::FindString(it->c_str(), "FONT", &sFont)) sFont = "ethnocentric.ttf";

            // create new language button
            coreButton* pButton = new coreButton(MENU_BUTTON, coreData::StrFilename(sFont.c_str()), 30u, MENU_OUTLINE_SMALL);   // # filter filename again
            pButton->DefineProgram("menu_border_program");
            pButton->SetPosition  (coreVector2(0.0f, 0.15f * (fOffset - I_TO_F(m_apLanguageButton.size()))));
            pButton->SetSize      (coreVector2(0.3f, 0.07f) * 1.5f);
            pButton->GetCaption()->SetText(asLanguageList.get_key(it)->c_str());

            // 
            this->BindObject(SURFACE_INTRO_LANGUAGE, pButton);

            // 
            m_apLanguageButton.emplace((*it), pButton);
        }
    }

    // 
    m_IntroTimer.Play(CORE_TIMER_PLAY_RESET);
    this->ChangeSurface(bSelectLanguage ? SURFACE_INTRO_LANGUAGE : SURFACE_INTRO_LOGO, 1.0f);
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
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    if(this->GetCurSurface() == SURFACE_INTRO_LANGUAGE)
    {
        // 
        FOR_EACH(it, m_apLanguageButton) if((*it)->IsFocused())     m_iSelected =  coreUint8(it - m_apLanguageButton.begin());
        FOR_EACH(it, m_apLanguageButton) cMenu::UpdateButton((*it), m_iSelected == coreUint8(it - m_apLanguageButton.begin()));

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
    }
    else if(this->GetOldSurface() == SURFACE_INTRO_LANGUAGE)
    {
        // 
        if(!this->GetTransition().GetStatus()) this->ChangeSurface(SURFACE_INTRO_LOGO, 1.0f);
    }
    else
    {
        // 
        m_MausLogo.SetAlpha(m_MausLogo.GetAlpha() * m_fIntroAlpha);

        if(m_IntroTimer.Update(1.0f))
        {
            // 
            m_iStatus = 1;
        }

        if(Core::Input->GetAnyButton(CORE_INPUT_PRESS))
        {
            // 
            const coreFloat fTime1 = m_IntroTimer.GetValue(CORE_TIMER_GET_NORMAL);
            if(fTime1 < 1.0f)
            {
                // 
                m_fIntroAlpha = m_MausLogo.GetAlpha();
            }
            if(fTime1 < 1.75f)
            {
                // 
                m_IntroTimer.SetValue(1.75f);
            }
        }

        // 
        const coreFloat fTime2 = m_IntroTimer.GetValue(CORE_TIMER_GET_NORMAL);
        if(fTime2 >= 1.0f)
        {
            // 
            this->ChangeSurface(SURFACE_INTRO_EMPTY, 1.0f);
        }
        if(fTime2 >= 1.75f)
        {
            // 
            g_pEnvironment->Activate();
            g_pPostProcessing->SetWallOpacity(CLAMP(1.35f * (fTime2 - 1.75f), 0.0f, 1.0f));
        }
    }
}