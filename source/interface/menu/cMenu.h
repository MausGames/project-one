//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MENU_H_
#define _P1_GUARD_MENU_H_


// ****************************************************************
// menu definitions
#define MENU_BUTTON        "menu_background_black.png", "menu_background_black.png"
#define MENU_FONT_SMALL    "ethnocentric.ttf", 13
#define MENU_FONT_MEDIUM   "ethnocentric.ttf", 20
#define MENU_FONT_BIG      "ethnocentric.ttf", 30

#define MENU_LIGHT_ACTIVE  (1.0f)
#define MENU_LIGHT_IDLE    (0.667f)
#define MENU_TEXT_CONTRAST (0.8f)


// ****************************************************************
// menu surface numbers
#define SURFACE_INTRO          (0)
#define SURFACE_MAIN           (1)

#define SURFACE_INTRO_EMPTY    (0)
#define SURFACE_INTRO_LOGO     (1)
#define SURFACE_INTRO_LANGUAGE (2)

#define SURFACE_MAIN_DEFAULT   (0)
#define SURFACE_MAIN_CREDITS   (1)


// ****************************************************************
// intro menu class
class cIntroMenu final : public coreMenu
{
private:
    coreObject2D m_MausLogo;                       // 

    coreTimer m_IntroTimer;                        // 
    coreByte  m_iIntroStatus;                      // 

    std::vector<coreButton*> m_apLanguageButton;   // 
    std::vector<std::string> m_asLanguagePath;     // 


public:
    cIntroMenu()noexcept;
    ~cIntroMenu();

    DISABLE_COPY(cIntroMenu)

    // move the intro menu
    void Move()override;
};


// ****************************************************************
// main menu class
class cMainMenu final : public coreMenu
{
private:
    coreObject2D m_GameLogo;      // 

    coreButton m_StartButton;     // 
    coreButton m_CreditsButton;   // 
    coreButton m_ExitButton;      // 

    coreLabel m_VersionInfo;      // 


public:
    cMainMenu()noexcept;

    DISABLE_COPY(cMainMenu)

    // move the main menu
    void Move()override;
};


// ****************************************************************
// master menu class
class cMenu final : public coreMenu
{
private:
    cIntroMenu m_IntroMenu;   // 
    cMainMenu  m_MainMenu;    // 

    cMsgBox  m_MsgBox;        // 
    cTooltip m_Tooltip;       // 


public:
    cMenu()noexcept;

    DISABLE_COPY(cMenu)

    // move the menu
    void Move();

    // 
    static void UpdateButton(coreButton* OUTPUT pButton);
};


#endif // _P1_GUARD_MENU_H_