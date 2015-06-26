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
#define MENU_FONT_SMALL    "ethnocentric.ttf", (13u), true
#define MENU_FONT_MEDIUM_2 "ethnocentric.ttf", (20u), true
#define MENU_FONT_MEDIUM_3 "ethnocentric.ttf", (30u), true
#define MENU_FONT_BIG_4    "ethnocentric.ttf", (40u), true
#define MENU_FONT_BIG_7    "ethnocentric.ttf", (70u), true
#define MENU_ICON_MEDIUM_2 "fontawesome.ttf",  (24u), true

#define MENU_LIGHT_ACTIVE   (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE     (0.667f)   // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK (0.04f)    // black contrast value


// ****************************************************************
// menu surface numbers
#define SURFACE_EMPTY          (0u)
#define SURFACE_INTRO          (1u)
#define SURFACE_MAIN           (2u)
#define SURFACE_GAME           (3u)
#define SURFACE_CONFIG         (4u)

#define SURFACE_INTRO_EMPTY    (0u)
#define SURFACE_INTRO_LOGO     (1u)
#define SURFACE_INTRO_LANGUAGE (2u)

#define SURFACE_MAIN_DEFAULT   (0u)

#define SURFACE_GAME_MISSION   (0u)
#define SURFACE_GAME_TROPHY    (1u)

#define SURFACE_CONFIG_VIDEO   (0u)
#define SURFACE_CONFIG_AUDIO   (1u)
#define SURFACE_CONFIG_INPUT   (2u)
#define SURFACE_CONFIG_MISC    (3u)


// ****************************************************************
// 
#define ICON_POWER_OFF "\xEF\x80\x91"
#define ICON_CHECK     "\xEF\x80\x8C"
#define ICON_TIMES     "\xEF\x80\x8D"


// ****************************************************************
// intro menu class
class cIntroMenu final : public coreMenu
{
private:
    coreObject2D m_MausLogo;                         // Maus logo

    coreTimer m_IntroTimer;                          // intro animation timer
    coreUint8 m_iIntroStatus;                        // current intro animation status

    coreLookupStr<coreButton*> m_apLanguageButton;   // list with buttons for valid language files
    coreUint8 m_iSelected;                           // current selected language button


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

    coreButton m_StartButton;      // start button
    coreButton m_ExitButton;       // exit button

    coreLabel m_aVersionInfo[2];   // hard-coded version info strings

    coreUint8 m_iSelected;         // current selected menu button


public:
    cMainMenu()noexcept;

    DISABLE_COPY(cMainMenu)

    // move the main menu
    void Move()override;
};


// ****************************************************************
// game menu class
class cGameMenu final : public coreMenu
{
private:
    coreObject2D m_Background;   // 

    coreButton m_StartButton;    // start button

    coreButton m_ConfigButton;   // config button
    coreButton m_ExtraButton;    // extra button
    coreButton m_ExitButton;     // exit button


public:
    cGameMenu()noexcept;

    DISABLE_COPY(cGameMenu)

    // move the game menu
    void Move()override;
};


// ****************************************************************
// config menu class
class cConfigMenu final : public coreMenu
{
private:
    coreObject2D m_Background;   // 

    coreButton m_SaveButton;     // save button
    coreButton m_CancelButton;   // cancel button


public:
    cConfigMenu()noexcept;

    DISABLE_COPY(cConfigMenu)

    // move the config menu
    void Move()override;
};


// ****************************************************************
// master menu class
class cMenu final : public coreMenu
{
private:
    cIntroMenu* m_pIntroMenu;   // intro menu object (dynamically unloaded)
    cMainMenu*  m_pMainMenu;    // main menu object (dynamically unloaded)

    cGameMenu   m_GameMenu;     // game menu object
    cConfigMenu m_ConfigMenu;   // config menu object

    cMsgBox  m_MsgBox;          // message box overlay
    cTooltip m_Tooltip;         // tooltip overlay


public:
    cMenu()noexcept;
    ~cMenu();

    DISABLE_COPY(cMenu)

    // render and move the menu
    void Render()override;
    void Move  ()override;

    // access special menu overlays
    inline cMsgBox*  GetMsgBox () {return &m_MsgBox;}
    inline cTooltip* GetTooltip() {return &m_Tooltip;}

    // default menu routines
    static void UpdateButton(coreButton* OUTPUT pButton, const coreBool& bFocused, const coreVector3 vFocusColor = coreVector3(1.0f,1.0f,1.0f));
    static coreVector3 FUNC_CONST HealthColor(const coreFloat& fValue);
    static coreVector3 FUNC_CONST ChainColor (const coreFloat& fValue);
};


#endif // _P1_GUARD_MENU_H_