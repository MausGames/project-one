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

// TODO: language and main menu control with keyboard+joystick, cursor gets invisible on these inputs, in game menu only cursor
// TODO: short YES-no questions: Exit Game ? Return to Menu ?


// ****************************************************************
// menu definitions
#define MENU_BUTTON        "menu_background_black.png", "menu_background_black.png"
#define MENU_FONT_SMALL    "ethnocentric.ttf", 13u
#define MENU_FONT_MEDIUM_2 "ethnocentric.ttf", 20u
#define MENU_FONT_MEDIUM_3 "ethnocentric.ttf", 30u
#define MENU_FONT_BIG_4    "ethnocentric.ttf", 40u
#define MENU_FONT_BIG_7    "ethnocentric.ttf", 70u

#define MENU_LIGHT_ACTIVE   (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE     (0.667f)   // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK (0.04f)    // black contrast value


// ****************************************************************
// menu surface numbers
#define SURFACE_EMPTY          (0u)
#define SURFACE_INTRO          (1u)
#define SURFACE_MAIN           (2u)

#define SURFACE_INTRO_EMPTY    (0u)
#define SURFACE_INTRO_LOGO     (1u)
#define SURFACE_INTRO_LANGUAGE (2u)

#define SURFACE_MAIN_DEFAULT   (0u)
#define SURFACE_MAIN_CREDITS   (1u)


// ****************************************************************
// intro menu class
class cIntroMenu final : public coreMenu
{
private:
    coreObject2D m_MausLogo;                         // Maus logo

    coreTimer m_IntroTimer;                          // intro animation timer
    coreUint8 m_iIntroStatus;                        // current intro animation status

    coreLookupStr<coreButton*> m_apLanguageButton;   // list with buttons for valid language files
    coreUint8 m_iCurLanguage;                        // current selected language button


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
    coreObject2D m_GameLogo;       // game logo
    coreObject2D m_Background;     // background object (credits surface)

    coreButton m_StartButton;      // start button
    coreButton m_CreditsButton;    // credits button
    coreButton m_ExitButton;       // exit button
    coreButton m_BackButton;       // back button (credits surface)

    coreLabel m_aVersionInfo[2];   // hard-coded version info string

    coreUint8 m_iCurButton;        // current selected menu button


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
    cIntroMenu* m_pIntroMenu;   // intro menu object (dynamically unloaded)
    cMainMenu*  m_pMainMenu;    // main menu object (dynamically unloaded)

    cMsgBox  m_MsgBox;          // message box overlay
    cTooltip m_Tooltip;         // tooltip overlay


public:
    cMenu()noexcept;
    ~cMenu();

    DISABLE_COPY(cMenu)

    // move the menu
    void Move()override;

    // display special menu overlays
    void ShowMsgBox () {}
    void ShowTooltip() {}

    // default menu routines
    static void        UpdateButton(coreButton* OUTPUT pButton, const coreBool& bFocused);
    static coreVector3 HealthColor (const coreFloat& fValue);
};


#endif // _P1_GUARD_MENU_H_