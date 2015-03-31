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
cIntroMenu::cIntroMenu()noexcept
: coreMenu       (3u, SURFACE_INTRO_EMPTY)
, m_IntroTimer   (coreTimer(1.5f, 0.5f, 1u))
, m_iIntroStatus (0u)
, m_iCurLanguage (0u)
{
    // create menu objects
    m_MausLogo.DefineProgram("default_2d_program");
    m_MausLogo.DefineTexture(0u, "maus_logo.png");
    m_MausLogo.SetPosition  (coreVector2(0.0f,0.0f));
    m_MausLogo.SetSize      (coreVector2(2.0f,1.0f) * 0.2f);

    // bind menu objects
    this->BindObject(SURFACE_INTRO_LOGO, &m_MausLogo);

    const coreBool bSelectLanguage = Core::Language->GetNumStrings() ? false : true;
    if(bSelectLanguage)
    {
        // 
        std::vector<std::string> asFile;
        coreData::ScanFolder("data/languages", "*.lng", &asFile);
        if(asFile.empty()) Core::Log->Error("No language files found (data/languages/*.lng)");

        // 
        const coreFloat fOffset = I_TO_F(asFile.size()) * 0.5f - 0.5f;
        FOR_EACH(it, asFile)
        {
            // create new language button
            coreButton* pButton = new coreButton(MENU_BUTTON, MENU_FONT_MEDIUM_3, 0u);
            pButton->DefineProgram("menu_border_program");
            pButton->SetPosition  (coreVector2(0.0f, 0.15f * (fOffset - I_TO_F(m_apLanguageButton.size()))));
            pButton->SetSize      (coreVector2(0.3f, 0.07f) * 1.5f);
            pButton->GetCaption()->SetText(coreLanguage(it->c_str()).GetString("LANGUAGE"));

            // 
            this->BindObject(SURFACE_INTRO_LANGUAGE, pButton);

            // 
            m_apLanguageButton[it->c_str()] = pButton;
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
    coreMenu::Move();
    m_iStatus = 0;

    if(this->GetCurSurface() == SURFACE_INTRO_LANGUAGE)
    {
        // 
        FOR_EACH(it, m_apLanguageButton) if((*it)->IsFocused())     m_iCurLanguage =  coreUint8(it - m_apLanguageButton.begin());
        FOR_EACH(it, m_apLanguageButton) cMenu::UpdateButton((*it), m_iCurLanguage == coreUint8(it - m_apLanguageButton.begin()));

        // 
        FOR_EACH(it, m_apLanguageButton)
        {
            if((*it)->IsClicked())
            {
                const coreChar* pcPath = m_apLanguageButton.get_key(it)->c_str();

                // 
                Core::Language->Load(pcPath);
                Core::Config->SetString(CORE_CONFIG_SYSTEM_LANGUAGE, pcPath);

                // 
                this->ChangeSurface(SURFACE_INTRO_EMPTY, 1.0f);
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
        if(m_IntroTimer.Update(1.0f) || Core::Input->GetKeyboardChar() || Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_PRESS))
        {
            // 
            m_iStatus = 1;

            // 
            g_pEnvironment->Activate();
        }

        // 
        if(m_iIntroStatus == 0u)
        {
            if(m_IntroTimer.GetValue(CORE_TIMER_GET_NORMAL) >= 1.0f)
            {
                // 
                m_iIntroStatus = 1u;
                this->ChangeSurface(SURFACE_INTRO_EMPTY, 1.0f);

                // 
                g_pEnvironment->Activate();
                g_pPostProcessing->SetSideOpacity(1.0f);
            }
            else
            {
                // 
                g_pPostProcessing->SetSideOpacity(this->GetTransition().GetValue(CORE_TIMER_GET_NORMAL));
            }
        }
    }
}