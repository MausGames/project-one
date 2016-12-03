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
// TODO: score-menu names and all name-inputs MUST support all languages (japanese (keifont), russian (default), arabic (default), ...)
// TODO: score-menu names must be sanitized
// TODO: unload fonts currently not used (e.g. from score-menu)


// ****************************************************************
// menu definitions
#define MENU_LIGHT_ACTIVE      (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE        (0.667f)   // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE    (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK    (0.04f)    // black contrast value

#define MENU_SCORE_ENTRIES     (10u)
#define MENU_SCORE_NAME_LENGTH (32u)
#define MENU_REPLAY_ENTRIES    (5u)
#define MENU_REPLAY_NAME_LENTH (REPLAY_NAME_LENTH)

#define MENU_BUTTON            "menu_background_black.png", "menu_background_black.png"
#define MENU_SWITCHBOX         "default_black.png", "default_black.png"
#define MENU_FONT_DYNAMIC_1    "dynamic_font",     (13u)
#define MENU_FONT_DYNAMIC_2    "dynamic_font",     (20u)
#define MENU_FONT_DYNAMIC_3    "dynamic_font",     (30u)
#define MENU_FONT_DYNAMIC_4    "dynamic_font",     (40u)
#define MENU_FONT_DYNAMIC_5    "dynamic_font",     (70u)
#define MENU_FONT_STANDARD_1   "ethnocentric.ttf", (13u)
#define MENU_FONT_STANDARD_2   "ethnocentric.ttf", (20u)
#define MENU_FONT_STANDARD_3   "ethnocentric.ttf", (30u)
#define MENU_FONT_STANDARD_4   "ethnocentric.ttf", (40u)
#define MENU_FONT_STANDARD_5   "ethnocentric.ttf", (70u)
#define MENU_FONT_ICON_1       "fontawesome.ttf",  (24u)
#define MENU_OUTLINE_SMALL     (1u)
#define MENU_OUTLINE_BIG       (4u)


// ****************************************************************
// menu surface numbers
#define SURFACE_EMPTY          (0u)
#define SURFACE_INTRO          (1u)
#define SURFACE_MAIN           (2u)
#define SURFACE_GAME           (3u)
#define SURFACE_CONFIG         (4u)
#define SURFACE_EXTRA          (5u)
#define SURFACE_PAUSE          (6u)
#define SURFACE_SCORE          (7u)
#define SURFACE_REPLAY         (8u)

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

#define SURFACE_PAUSE_DEFAULT  (0u)

#define SURFACE_SCORE_DEFAULT  (0u)

#define SURFACE_REPLAY_DEFAULT (0u)


// ****************************************************************
// configuration entry indices
#define ENTRY_VIDEO_DISPLAY       (0u)
#define ENTRY_VIDEO_RESOLUTION    (1u)
#define ENTRY_VIDEO_DISPLAYMODE   (2u)
#define ENTRY_VIDEO_ANTIALIASING  (3u)
#define ENTRY_VIDEO_TEXTUREFILTER (4u)
#define ENTRY_VIDEO_ASSETQUALITY  (5u)
#define ENTRY_VIDEO_SHADOWQUALITY (6u)
#define ENTRY_VIDEO               (7u)

#define ENTRY_AUDIO_OVERALLVOLUME (0u + ENTRY_VIDEO)
#define ENTRY_AUDIO_MUSICVOLUME   (1u + ENTRY_VIDEO)
#define ENTRY_AUDIO_EFFECTVOLUME  (2u + ENTRY_VIDEO)
#define ENTRY_AUDIO_AMBIENTSOUND  (3u + ENTRY_VIDEO)
#define ENTRY_AUDIO               (4u + ENTRY_VIDEO)

#define ENTRY_INPUT               (0u + ENTRY_AUDIO)

#define ENTRY_GAME_LANGUAGE       (0u + ENTRY_INPUT)
#define ENTRY_MAX                 (1u + ENTRY_INPUT)


// ****************************************************************
// icon codes (UTF-8)
#define ICON_CHECK     "\xEF\x80\x8C"
#define ICON_TIMES     "\xEF\x80\x8D"
#define ICON_POWER_OFF "\xEF\x80\x91"
#define ICON_REFRESH   "\xEF\x80\xA1"
#define ICON_SHARE     "\xEF\x81\xA4"
#define ICON_COGS      "\xEF\x82\x85"
#define ICON_CARET_UP  "\xEF\x83\x98"


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
    ~cIntroMenu()final;

    DISABLE_COPY(cIntroMenu)

    // move the intro menu
    void Move()final;
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
    void Move()final;
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
    coreButton m_ScoreButton;    // score button
    coreButton m_ReplayButton;   // replay button
    coreButton m_ExitButton;     // exit button


public:
    cGameMenu()noexcept;

    DISABLE_COPY(cGameMenu)

    // move the game menu
    void Move()final;
};


// ****************************************************************
// config menu class
class cConfigMenu final : public coreMenu
{
private:
    coreObject2D m_Background;          // 

    coreButton m_VideoTab;              // 
    coreButton m_AudioTab;              // 
    coreButton m_InputTab;              // 
    coreButton m_GameTab;               // 

    coreButton m_SaveButton;            // save button
    coreButton m_DiscardButton;         // discard button
    coreButton m_BackButton;            // back button

    coreLabel    m_aLabel[ENTRY_MAX];   // 
    coreObject2D m_aLine [ENTRY_MAX];   // 

    coreSwitchBoxU8 m_Display;
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
    void Move()final;

    // 
    void CheckValues();
    void LoadValues();
    void SaveValues();


private:
    // 
    void __UpdateShadowQuality();
    void __UpdateOverallVolume();
    void __UpdateLanguage();

    // 
    void __LoadDisplays();
    void __LoadResolutions(const coreUintW iDisplayIndex);
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
    void Move()final;
};


// ****************************************************************
// pause menu class
class cPauseMenu final : public coreMenu
{
private:
    coreButton m_ResumeButton;    // resume button
    coreButton m_ConfigButton;    // config button
    coreButton m_RestartButton;   // restart button
    coreButton m_ExitButton;      // exit button

    coreUint8 m_iSelected;        // current selected menu button


public:
    cPauseMenu()noexcept;

    DISABLE_COPY(cPauseMenu)

    // move the pause menu
    void Move()final;
};


// ****************************************************************
// score menu class
class cScoreMenu final : public coreMenu
{
private:
    coreObject2D m_Background;                   // 

    coreButton m_BackButton;                     // back button

    coreObject2D m_aLine [MENU_SCORE_ENTRIES];   // 
    coreLabel    m_aRank [MENU_SCORE_ENTRIES];   // 
    coreLabel    m_aName [MENU_SCORE_ENTRIES];   // 
    coreLabel    m_aScore[MENU_SCORE_ENTRIES];   // 


public:
    cScoreMenu()noexcept;

    DISABLE_COPY(cScoreMenu)

    // move the score menu
    void Move()final;
};


// ****************************************************************
// replay menu class
class cReplayMenu final : public coreMenu
{
private:
    coreObject2D m_Background;                   // 

    coreButton m_BackButton;                     // back button

    coreObject2D m_aLine[MENU_REPLAY_ENTRIES];   // 
    coreLabel    m_aName[MENU_REPLAY_ENTRIES];   // 
    coreLabel    m_aTime[MENU_REPLAY_ENTRIES];   // 

    std::vector<cReplay::uInfo> m_aInfoList;     // 


public:
    cReplayMenu()noexcept;

    DISABLE_COPY(cReplayMenu)

    // move the replay menu
    void Move()final;

    // 
    void LoadReplays();
};


// ****************************************************************
// master menu class
class cMenu final : public coreMenu
{
private:
    cIntroMenu* m_pIntroMenu;     // intro menu object (dynamically unloaded)
    cMainMenu*  m_pMainMenu;      // main menu object (dynamically unloaded)

    cGameMenu   m_GameMenu;       // game menu object
    cConfigMenu m_ConfigMenu;     // config menu object
    cExtraMenu  m_ExtraMenu;      // extra menu object
    cPauseMenu  m_PauseMenu;      // pause menu object
    cScoreMenu  m_ScoreMenu;      // score menu object
    cReplayMenu m_ReplayMenu;     // replay menu object

    cMsgBox  m_MsgBox;            // message box overlay
    cTooltip m_Tooltip;           // tooltip overlay

    coreObject2D m_PauseLayer;    // 
    coreUint32   m_iPauseFrame;   // 


public:
    cMenu()noexcept;
    ~cMenu()final;

    DISABLE_COPY(cMenu)

    // render and move the menu
    void Render()final;
    void Move  ()final;

    // access special menu overlays
    inline cMsgBox*  GetMsgBox () {return &m_MsgBox;}
    inline cTooltip* GetTooltip() {return &m_Tooltip;}

    // 
    coreBool IsPaused()const;
    coreBool IsPausedWithStep();
    inline void InvokePauseStep() {m_iPauseFrame = Core::System->GetCurFrame();}

    // 
    static void UpdateLanguageFont();
    static const coreLookup<std::string, std::string>& GetLanguageList();

    // menu helper routines
    static void UpdateButton(coreButton* OUTPUT pButton, const coreBool bFocused, const coreVector3& vFocusColor);
    static void UpdateButton(coreButton* OUTPUT pButton, const coreBool bFocused);
    static void UpdateSwitchBox(coreSwitchBoxU8* OUTPUT pSwitchBox);
};


#endif // _P1_GUARD_MENU_H_