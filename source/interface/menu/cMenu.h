///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MENU_H_
#define _P1_GUARD_MENU_H_

// TODO: language and main menu control with keyboard+joystick, cursor gets invisible on these inputs, in game menu only cursor
// TODO: short YES-no questions: Exit Game ? Return to Menu ?
// TODO: menue-riss bei start oder seitlicher fade-out, ausserdem bei start im title-menue animation und explosion des logos oder riss in mitte
// TODO: move mouse to buttons on joystick-input
// TODO: real-time sound-configuration
// TODO: score-menu names and all name-inputs MUST support all languages (japanese (keifont), russian (default), arabic (default), ...)
// TODO: score-menu names must be sanitized
// TODO: unload fonts currently not used (e.g. from score-menu)
// TODO: options menu: ask if values should be discarded, ask if want to exit instead of saving
// TODO: rumble when changing rumble-option
// TODO: display unattached joysticks and joystick names somehow
// TODO: highlight which joystick is which input set
// TODO: fix all g_vMenuCenter usages when changing aspect ratio


// ****************************************************************
// menu definitions
#define MENU_LIGHT_ACTIVE    (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE      (0.667f)   // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE  (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK  (0.04f)    // black contrast value

#define MENU_CONFIG_INPUTS   (PLAYERS)
#define MENU_SCORE_ENTRIES   (10u)
#define MENU_REPLAY_ENTRIES  (5u)
#define MENU_SUMMARY_ENTRIES (3u)
#define MENU_SUMMARY_PARTS   (PLAYERS)

#define MENU_BUTTON          "menu_background_black.png", "menu_background_black.png"
#define MENU_SWITCHBOX       "default_black.png", "default_black.png"
#define MENU_FONT_DYNAMIC_1  "dynamic_font",     (13u)
#define MENU_FONT_DYNAMIC_2  "dynamic_font",     (20u)
#define MENU_FONT_DYNAMIC_3  "dynamic_font",     (30u)
#define MENU_FONT_DYNAMIC_4  "dynamic_font",     (40u)
#define MENU_FONT_DYNAMIC_5  "dynamic_font",     (70u)
#define MENU_FONT_STANDARD_1 "ethnocentric.ttf", (13u)
#define MENU_FONT_STANDARD_2 "ethnocentric.ttf", (20u)
#define MENU_FONT_STANDARD_3 "ethnocentric.ttf", (30u)
#define MENU_FONT_STANDARD_4 "ethnocentric.ttf", (40u)
#define MENU_FONT_STANDARD_5 "ethnocentric.ttf", (70u)
#define MENU_FONT_ICON_1     "fontawesome.otf",  (20u)
#define MENU_FONT_ICON_2     "fontawesome.otf",  (24u)
#define MENU_FONT_ICON_3     "fontawesome.otf",  (40u)
#define MENU_OUTLINE_SMALL   (1u)
#define MENU_OUTLINE_BIG     (4u)


// ****************************************************************
// menu surface numbers
#define SURFACE_EMPTY           (0u)
#define SURFACE_INTRO           (1u)
#define SURFACE_TITLE           (2u)
#define SURFACE_MAIN            (3u)
#define SURFACE_GAME            (4u)
#define SURFACE_SCORE           (5u)
#define SURFACE_REPLAY          (6u)
#define SURFACE_CONFIG          (7u)
#define SURFACE_EXTRA           (8u)
#define SURFACE_PAUSE           (9u)
#define SURFACE_SUMMARY         (10u)

#define SURFACE_INTRO_EMPTY     (0u)
#define SURFACE_INTRO_LOGO      (1u)
#define SURFACE_INTRO_LANGUAGE  (2u)

#define SURFACE_TITLE_DEFAULT   (0u)

#define SURFACE_MAIN_DEFAULT    (0u)

#define SURFACE_GAME_DEFAULT    (0u)

#define SURFACE_SCORE_DEFAULT   (0u)

#define SURFACE_REPLAY_DEFAULT  (0u)

#define SURFACE_CONFIG_VIDEO    (0u)
#define SURFACE_CONFIG_AUDIO    (1u)
#define SURFACE_CONFIG_INPUT    (2u)
#define SURFACE_CONFIG_GAME     (3u)

#define SURFACE_EXTRA_DEFAULT   (0u)

#define SURFACE_PAUSE_DEFAULT   (0u)

#define SURFACE_SUMMARY_DEFAULT (0u)


// ****************************************************************
// configuration entry indices
#define ENTRY_VIDEO_MONITOR       (0u)
#define ENTRY_VIDEO_RESOLUTION    (1u)
#define ENTRY_VIDEO_DISPLAYMODE   (2u)
#define ENTRY_VIDEO_ANTIALIASING  (3u)
#define ENTRY_VIDEO_TEXTUREFILTER (4u)
#define ENTRY_VIDEO_ASSETQUALITY  (5u)
#define ENTRY_VIDEO_SHADOWQUALITY (6u)
#define ENTRY_VIDEO               (7u)

#define ENTRY_AUDIO_OVERALLVOLUME (0u  + ENTRY_VIDEO)
#define ENTRY_AUDIO_MUSICVOLUME   (1u  + ENTRY_VIDEO)
#define ENTRY_AUDIO_EFFECTVOLUME  (2u  + ENTRY_VIDEO)
#define ENTRY_AUDIO_AMBIENTSOUND  (3u  + ENTRY_VIDEO)
#define ENTRY_AUDIO               (4u  + ENTRY_VIDEO)

#define ENTRY_INPUT_TYPE          (0u  + ENTRY_AUDIO)
#define ENTRY_INPUT_RUMBLE        (1u  + ENTRY_AUDIO)
#define ENTRY_INPUT_MOVEUP        (2u  + ENTRY_AUDIO)
#define ENTRY_INPUT_MOVELEFT      (3u  + ENTRY_AUDIO)
#define ENTRY_INPUT_MOVEDOWN      (4u  + ENTRY_AUDIO)
#define ENTRY_INPUT_MOVERIGHT     (5u  + ENTRY_AUDIO)
#define ENTRY_INPUT_ACTION1       (6u  + ENTRY_AUDIO)
#define ENTRY_INPUT_ACTION2       (7u  + ENTRY_AUDIO)
#define ENTRY_INPUT_ACTION3       (8u  + ENTRY_AUDIO)
#define ENTRY_INPUT_ACTION4       (9u  + ENTRY_AUDIO)
#define ENTRY_INPUT_ACTION5       (10u + ENTRY_AUDIO)
#define ENTRY_INPUT               (11u + ENTRY_AUDIO)

#define ENTRY_GAME_LANGUAGE       (0u  + ENTRY_INPUT)
#define ENTRY_MAX                 (1u  + ENTRY_INPUT)


// ****************************************************************
// icon codes (UTF-8)
#define ICON_CHECK        u8"\uF00C"
#define ICON_TIMES        u8"\uF00D"
#define ICON_POWER_OFF    u8"\uF011"
#define ICON_REFRESH      u8"\uF021"
#define ICON_ARROW_LEFT   u8"\uF060"
#define ICON_ARROW_RIGHT  u8"\uF061"
#define ICON_ARROW_UP     u8"\uF062"
#define ICON_ARROW_DOWN   u8"\uF063"
#define ICON_SHARE        u8"\uF064"
#define ICON_COGS         u8"\uF085"
#define ICON_CARET_DOWN   u8"\uF0D7"
#define ICON_CARET_UP     u8"\uF0D8"
#define ICON_CARET_LEFT   u8"\uF0D9"
#define ICON_CARET_RIGHT  u8"\uF0DA"
#define ICON_PAUSE_CIRCLE u8"\uF28B"
#define ICON_UNDO_ALT     u8"\uF2EA"
#define ICON_REFRESH_ALT  u8"\uF2F1"
#define ICON_REDO_ALT     u8"\uF2F9"
#define ICON_SHIELD_ALT   u8"\uF3ED"
#define ICON_BURN         u8"\uF46A"
#define ICON_FEATHER_ALT  u8"\uF56B"


// ****************************************************************
// intro menu class
class cIntroMenu final : public coreMenu
{
private:
    coreObject2D m_MausLogo;                                   // Maus logo

    coreTimer m_IntroTimer;                                    // intro animation 
    coreFloat m_fIntroAlpha;                                   // 

    coreLookup<std::string, coreButton*> m_apLanguageButton;   // list with buttons for valid language files
    coreUint8 m_iSelected;                                     // current selected language button


public:
    cIntroMenu()noexcept;
    ~cIntroMenu()final;

    DISABLE_COPY(cIntroMenu)

    // move the intro menu
    void Move()final;
};


// ****************************************************************
// title menu class
class cTitleMenu final : public coreMenu
{
private:
    coreObject2D m_GameLogo;       // game logo

    coreLabel m_StartMessage;      // 
    coreLabel m_aVersionInfo[2];   // hard-coded version info strings

    coreParticleEffect m_Effect;   // 


public:
    cTitleMenu()noexcept;
    ~cTitleMenu()final;

    DISABLE_COPY(cTitleMenu)

    // move the title menu
    void Move()final;
};


// ****************************************************************
// main menu class
class cMainMenu final : public coreMenu
{
private:
    coreButton m_StartButton;    // start button
    coreButton m_ScoreButton;    // score button
    coreButton m_ReplayButton;   // replay button
    coreButton m_ConfigButton;   // config button
    coreButton m_ExtraButton;    // extra button
    coreButton m_ExitButton;     // exit button


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
    coreObject2D m_Background;      // 

    coreButton m_StartButton;       // start button
    coreButton m_BackButton;        // back button

    coreLabel    m_aLabel[3];       // 
    coreObject2D m_aLine [3];       // 

    coreSwitchBoxU8 m_Mode;         // 
    coreSwitchBoxU8 m_Difficulty;   // 
    coreSwitchBoxU8 m_Players;      // 


public:
    cGameMenu()noexcept;

    DISABLE_COPY(cGameMenu)

    // move the game menu
    void Move()final;

    // 
    inline const coreUint8& GetSelectedMode      ()const {return m_Mode      .GetCurEntry().tValue;}
    inline const coreUint8& GetSelectedDifficulty()const {return m_Difficulty.GetCurEntry().tValue;}
    inline const coreUint8& GetSelectedPlayers   ()const {return m_Players   .GetCurEntry().tValue;}
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

    std::vector<cReplay::sInfo> m_aInfoList;     // 


public:
    cReplayMenu()noexcept;

    DISABLE_COPY(cReplayMenu)

    // move the replay menu
    void Move()final;

    // 
    void LoadReplays();
};


// ****************************************************************
// config menu class
class cConfigMenu final : public coreMenu
{
private:
    // 
    struct sPlayerInput final
    {
        coreLabel       oHeader;
        coreSwitchBoxU8 oType;
        coreSwitchBoxU8 oRumble;
        coreButton      oMoveUp;
        coreButton      oMoveLeft;
        coreButton      oMoveDown;
        coreButton      oMoveRight;
        coreButton      aAction[INPUT_KEYS_ACTION];
    };


private:
    coreObject2D m_Background;           // 

    coreButton m_VideoTab;               // 
    coreButton m_AudioTab;               // 
    coreButton m_InputTab;               // 
    coreButton m_GameTab;                // 

    coreButton m_SaveButton;             // save button
    coreButton m_DiscardButton;          // discard button
    coreButton m_BackButton;             // back button

    coreLabel    m_aLabel[ENTRY_MAX];    // 
    coreObject2D m_aLine [ENTRY_MAX];    // 
    coreLabel    m_aArrow[INPUT_KEYS];   // 

    coreSwitchBoxU8 m_Monitor;
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
    sPlayerInput    m_aInput[MENU_CONFIG_INPUTS];


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
    void __LoadMonitors();
    void __LoadResolutions(const coreUintW iMonitorIndex);
    void __LoadInputs();

    // 
    inline coreButton& __RetrieveInputButton  (const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < MENU_CONFIG_INPUTS) && (iKeyIndex < INPUT_KEYS)) return *((&m_aInput[iPlayerIndex].oMoveUp)                                         + iKeyIndex);}
    inline coreInt16&  __RetrieveInputCurValue(const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < INPUT_TYPES)        && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[g_CurConfig.Input.aiType[iPlayerIndex]].iMoveUp) + iKeyIndex);}
    inline coreInt16&  __RetrieveInputDirValue(const coreUintW iType,        const coreUintW iKeyIndex) {ASSERT((iType        < INPUT_SETS)         && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[iType].iMoveUp)                                  + iKeyIndex);}

    // convert input key to readable string
    static const coreChar* __PrintKey(const coreUint8 iType, const coreInt16 iKey);
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
// summary menu class
class cSummaryMenu final : public coreMenu
{
private:
    coreObject2D m_Background;                                      // 

    coreLabel m_aTitle[MENU_SUMMARY_ENTRIES];                       // 
    coreLabel m_aValue[MENU_SUMMARY_ENTRIES];                       // 
    coreLabel m_aaPart[MENU_SUMMARY_ENTRIES][MENU_SUMMARY_PARTS];   // 

    coreLabel m_TotalTitle;                                         // 
    coreLabel m_TotalValue;                                         // 
    coreLabel m_aTotalPart[MENU_SUMMARY_PARTS];                     // 

    coreUint32 m_iFinalScore;                                       // (just for display) 
    coreFlow   m_fIntroTimer;                                       // 
    coreFlow   m_fOutroTimer;                                       // 
    coreUint8  m_iSkipped;                                          // 


public:
    cSummaryMenu()noexcept;

    DISABLE_COPY(cSummaryMenu)

    // render and move the summary menu
    void Render()final;
    void Move  ()final;

    // 
    void ShowSummary();
};


// ****************************************************************
// master menu class
class cMenu final : public coreMenu
{
private:
    cIntroMenu* m_pIntroMenu;     // intro menu object (dynamically unloaded)
    cTitleMenu* m_pTitleMenu;     // title menu object (dynamically unloaded)

    cMainMenu    m_MainMenu;      // main menu object
    cGameMenu    m_GameMenu;      // game menu object
    cScoreMenu   m_ScoreMenu;     // score menu object
    cReplayMenu  m_ReplayMenu;    // replay menu object
    cConfigMenu  m_ConfigMenu;    // config menu object
    cExtraMenu   m_ExtraMenu;     // extra menu object
    cPauseMenu   m_PauseMenu;     // pause menu object
    cSummaryMenu m_SummaryMenu;   // summary menu object

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
    static void UpdateAnimateProgram(coreObject2D* OUTPUT pObject);
};


#endif // _P1_GUARD_MENU_H_