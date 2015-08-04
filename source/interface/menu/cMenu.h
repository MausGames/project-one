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
// TODO: menue-riss bei start oder seitlicher fade-out, ausserdem bei start im main-menue animation und explosion des logos oder riss in mitte
// TODO: move mouse to buttons on joystick-input
// TODO: real-time sound-configuration


// ****************************************************************
// menu definitions
#define MENU_BUTTON        "menu_background_black.png", "menu_background_black.png"
#define MENU_SWITCHBOX     "default_black.png", "default_black.png"
#define MENU_FONT_SMALL    "ethnocentric.ttf", (13u)
#define MENU_FONT_MEDIUM_2 "ethnocentric.ttf", (20u)
#define MENU_FONT_MEDIUM_3 "ethnocentric.ttf", (30u)
#define MENU_FONT_BIG_4    "ethnocentric.ttf", (40u)
#define MENU_FONT_BIG_7    "ethnocentric.ttf", (70u)
#define MENU_ICON_MEDIUM_2 "fontawesome.ttf",  (24u)
#define MENU_OUTLINE_SMALL (1u)
#define MENU_OUTLINE_BIG   (4u)

#define MENU_LIGHT_ACTIVE   (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE     (0.667f)   // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK (0.04f)    // black contrast value

#define MENU_CONFIG_VIDEO_RESOLUTION    (0u)
#define MENU_CONFIG_VIDEO_DISPLAYMODE   (1u)
#define MENU_CONFIG_VIDEO_ANTIALIASING  (2u)
#define MENU_CONFIG_VIDEO_TEXTUREFILTER (3u)
#define MENU_CONFIG_VIDEO_ASSETQUALITY  (4u)
#define MENU_CONFIG_VIDEO_SHADOWQUALITY (5u)
#define MENU_CONFIG_VIDEO               (6u)
#define MENU_CONFIG_AUDIO_OVERALLVOLUME (0u + MENU_CONFIG_VIDEO)
#define MENU_CONFIG_AUDIO_MUSICVOLUME   (1u + MENU_CONFIG_VIDEO)
#define MENU_CONFIG_AUDIO_EFFECTVOLUME  (2u + MENU_CONFIG_VIDEO)
#define MENU_CONFIG_AUDIO_AMBIENTSOUND  (3u + MENU_CONFIG_VIDEO)
#define MENU_CONFIG_AUDIO               (4u + MENU_CONFIG_VIDEO)
#define MENU_CONFIG_INPUT               (0u +  MENU_CONFIG_AUDIO)
#define MENU_CONFIG_GAME_LANGUAGE       (0u +   MENU_CONFIG_INPUT)
#define MENU_CONFIG_MAX                 (1u +   MENU_CONFIG_INPUT)


// ****************************************************************
// menu surface numbers
#define SURFACE_EMPTY          (0u)
#define SURFACE_INTRO          (1u)
#define SURFACE_MAIN           (2u)
#define SURFACE_GAME           (3u)
#define SURFACE_CONFIG         (4u)
#define SURFACE_EXTRA          (5u)

#define SURFACE_INTRO_EMPTY    (0u)
#define SURFACE_INTRO_LOGO     (1u)
#define SURFACE_INTRO_LANGUAGE (2u)

#define SURFACE_MAIN_DEFAULT   (0u)

#define SURFACE_GAME_MISSION   (0u)
#define SURFACE_GAME_TROPHY    (1u)

#define SURFACE_CONFIG_VIDEO   (0u)
#define SURFACE_CONFIG_AUDIO   (1u)
#define SURFACE_CONFIG_INPUT   (2u)
#define SURFACE_CONFIG_GAME    (3u)

#define SURFACE_EXTRA_CREDITS  (0u)


// ****************************************************************
// icon codes (UTF-8)
#define ICON_POWER_OFF  "\xEF\x80\x91"
#define ICON_CHECK      "\xEF\x80\x8C"
#define ICON_TIMES      "\xEF\x80\x8D"
#define ICON_CARET_UP   "\xEF\x83\x98"
#define ICON_CHEVRON_UP "\xEF\x81\xB7"
#define ICON_SHARE      "\xEF\x81\xA4"


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


    coreObject2D m_aTest[5];      // 
    coreObject2D m_aTest2[5];     // 
    coreObject2D m_aTest3[5];     // 
    coreLabel    m_aTest4[5];     // 
    coreLabel m_aDemo[4];

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
    coreObject2D m_Background;                // 

    coreButton m_VideoTab;                    // 
    coreButton m_AudioTab;                    // 
    coreButton m_InputTab;                    // 
    coreButton m_GameTab;                     // 

    coreButton m_SaveButton;                  // save button
    coreButton m_DiscardButton;               // discard button
    coreButton m_BackButton;                  // back button

    coreLabel    m_aLabel[MENU_CONFIG_MAX];   // 
    coreObject2D m_aLine [MENU_CONFIG_MAX];   // 

    coreSwitchBoxU8 m_Resolution;
    coreSwitchBoxU8 m_DisplayMode;
    coreSwitchBoxU8 m_AntiAliasing;
    coreSwitchBoxU8 m_TextureFilter;
    coreSwitchBoxU8 m_AssetQuality;
    coreSwitchBoxU8 m_ShadowQuality;

    coreSwitchBoxU8 m_OverallVolume;
    coreSwitchBoxU8 m_MusicVolume;
    coreSwitchBoxU8 m_EffectVolume;
    coreSwitchBoxU8 m_AmbientSound;

    coreSwitchBoxU8 m_Language;


public:
    cConfigMenu()noexcept;

    DISABLE_COPY(cConfigMenu)

    // move the config menu
    void Move()override;

    // 
    void CheckValues();
    void LoadValues ();
    void SaveValues ();


private:
    // 
    void __UpdateShadowQuality();
    void __UpdateOverallVolume();
    void __UpdateLanguage     ();
};


// ****************************************************************
// extra menu class
class cExtraMenu final : public coreMenu
{
private:
    coreObject2D m_Background;   // 

    coreButton m_BackButton;     // back button


public:
    cExtraMenu()noexcept;

    DISABLE_COPY(cExtraMenu)

    // move the extra menu
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
    cExtraMenu  m_ExtraMenu;    // extra menu object

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
    static const coreLookupStr<std::string>& GetLanguageList();
    static void UpdateButton(coreButton* OUTPUT pButton, const coreBool& bFocused, const coreVector3& vFocusColor);
    static void UpdateButton(coreButton* OUTPUT pButton, const coreBool& bFocused);
    static void UpdateSwitchBox(coreSwitchBoxU8* OUTPUT pSwitchBox);
    static coreVector3 FUNC_CONST HealthColor(const coreFloat& fValue);
    static coreVector3 FUNC_CONST ChainColor (const coreFloat& fValue);
};


#endif // _P1_GUARD_MENU_H_