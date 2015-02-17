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
: coreMenu       (3, SURFACE_INTRO_EMPTY)
, m_IntroTimer   (coreTimer(2.0f, 0.5f, 1))
, m_iIntroStatus (0)
{
    // 
    m_MausLogo.DefineProgram("default_2d_program");
    m_MausLogo.DefineTexture(0, "maus_logo.png");
    m_MausLogo.SetPosition  (coreVector2(0.0f,0.0f));
    m_MausLogo.SetSize      (coreVector2(2.0f,1.0f) * 0.2f);

    // 
    this->BindObject(SURFACE_INTRO_LOGO, &m_MausLogo);

    // 
    std::vector<std::string> asFile;
    coreData::ScanFolder("data/languages", "*.lng", &asFile);

    // 
    const float fOffset = I_TO_F(asFile.size()) * 0.5f - 0.5f;
    FOR_EACH(it, asFile)
    {
        // 
        coreButton* pButton = new coreButton(MENU_BUTTON, MENU_FONT_BIG, 0);
        pButton->DefineProgram("menu_border_program");
        pButton->SetPosition  (coreVector2(0.0f, 0.15f * (fOffset - I_TO_F(m_apLanguageButton.size()))));
        pButton->SetSize      (coreVector2(3.5f, 0.8f) * 0.14f);
        pButton->GetCaption()->SetText(coreLanguage(it->c_str()).GetString("LANGUAGE"));

        // 
        this->BindObject(SURFACE_INTRO_LANGUAGE, pButton);

        // 
        m_apLanguageButton.push_back(pButton);
        m_asLanguagePath  .push_back(it->c_str());
    }

    // 
    m_IntroTimer.Play(CORE_TIMER_PLAY_RESET);
    this->ChangeSurface(SURFACE_INTRO_LOGO, 1.0f);
}


// ****************************************************************
// destructor
cIntroMenu::~cIntroMenu()
{
    // 
    FOR_EACH(it, m_apLanguageButton) SAFE_DELETE(*it)

    // 
    m_apLanguageButton.clear();
    m_asLanguagePath  .clear();
}


// ****************************************************************
// move the intro menu
void cIntroMenu::Move()
{
    // move the menu
    coreMenu::Move();

    if(this->GetCurSurface() == SURFACE_INTRO_LANGUAGE)
    {
        // 
        FOR_EACH(it, m_apLanguageButton)
            cMenu::UpdateButton(*it);
    }
    else
    {
        // 
        if(m_IntroTimer.Update(1.0f))
            m_iStatus = 1;



        // TODO ### 
        const float fValue = m_IntroTimer.GetValue(CORE_TIMER_GET_NORMAL);
        if((fValue >= 1.0f) && (m_iIntroStatus == 0))
        {
            m_iIntroStatus = 1;
            this->ChangeSurface(SURFACE_INTRO_EMPTY, 1.0f);

            g_pPostProcessing->SetSideOpacity(1.0f);
        }

        if(m_iIntroStatus == 0)
        {
            g_pPostProcessing->SetSideOpacity(this->GetTransition().GetValue(CORE_TIMER_GET_NORMAL));
        }


    }
}