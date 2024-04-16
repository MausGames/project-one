///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MENU_H_
#define _P1_GUARD_MENU_H_

// TODO 1: short YES-no questions: Exit Game ? Return to Menu ?
// TODO 1: real-time sound-configuration (also quick left-right sound effect on 3d-sound option)
// TODO 1: score-menu names are restricted to characters from the text-board
// TODO 3: score-menu names must be sanitized
// TODO 3: unload fonts currently not used (e.g. from score-menu)
// TODO 3: options menu: highlight changed options
// TODO 3: options menu: 15 second on video change, yes, no
// TODO 3: rumble when changing rumble-option (could be annoying)
// TODO 5: display unattached joysticks and joystick names somehow
// TODO 3: highlight which joystick is which input set
// TODO 3: update texture filter and render quality in realtime (resource-manager reset ?)
// TODO 3: double initial languages by switching to two columns (on demand?)
// TODO 5: stages in GameMenu should be called segments
// TODO 3: every object in menu needs outline: weapon icons
// TODO 3: show target FPS in config menu (speed x update rate)
// TODO 3: when switching resolution, and confirming by returning, the transition is broken
// TODO 3: when switching resolution, the next mouse click is not recognized (no press event is coming from SDL, only the release event)
// TODO 3: swapping controls should swap buttons visually
// TODO 3: load replay number only on first entry, load headers async on demand, handle changes during runtime
// TODO 3: display gamepad/device name in options description
// TODO 4: locked config input buttons are set with dummy values before getting locked
// TODO 2: handle plugging in and out gamepad while in config menu, handle plugging out gamepad in general (when controller is currently selected)
// TODO 2: in emscripten müssen einige optionen geändert werden (disable: monitor, resolution, display mode | change/disable: anti-aliasing [on/off] | anisotropic ?)
// TODO 3: leichtes wackeln des texts wenn sich switch-box ändert
// TODO 3: welle, wenn button/switchbox gedrückt wird
// TODO 3: anderes: ganzes menü wackelt/shifted wenn man tab ändert, tabs verändern höhe, buttons ändern größe bei selection
// TODO 3: GetWinFocusLost to force pause should only take effect when pause is actually allowed, save and apply state
// TODO 1: re-position everything after resolution-change
// TODO 3: menu sound response is super-confused with gamepad-input, related to having fokus on multiple sub-menus (unnecessary button-fokus sound -> catch initial focusing), and being able to focus disabled buttons (active tab, save button)
// TODO 3: de-couple interface-class and display in options when game is not running (for HUD type option)
// TODO 3: bigger effect, when password is correct (maybe not text, but a fullscreen effect)
// TODO 3: wenn zwischen borderless und windowed gewechselt wird mit Desktop resolution, wird im size-change-event das 0:0 überschrieben und der save-button bleibt aktiv
// TODO 3: wenn im windowed mode die resolution zu groß ist, wird sie geclamped und der save-button bleibt aktiv


// ****************************************************************
// menu definitions
#define MENU_LIGHT_ACTIVE             (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE               (0.65f)    // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE           (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK           (0.04f)    // black contrast value
#define MENU_CONTRAST_INSIDE          (0.94f)    // 
#define MENU_INSIDE_ALPHA             (0.95f)    // 

#define MENU_GAME_PLAYERS             (PLAYERS)
#define MENU_GAME_MISSIONS            (11u)
#define MENU_GAME_SEGMENTS            (6u)
#define MENU_GAME_ARCADES             (3u)
#define MENU_GAME_OPTIONS             (3u)
#define MENU_GAME_ARMORIES            (6u)
#define MENU_GAME_DEMOS               (5u)
#define MENU_GAME_ARMORY_MEDALS       (6u + 1u)
#define MENU_GAME_ARMORY_BADGES       (WAVES * BADGES)
#define MENU_SCORE_FILTERS            (2u)
#define MENU_SCORE_ENTRIES            (20u)
#define MENU_REPLAY_ENTRIES           (12u)
#define MENU_EXTRA_FRAGMENTS          (FRAGMENTS)
#define MENU_EXTRA_FILTERS            (2u)
#define MENU_EXTRA_STATS              (27u)
#define MENU_EXTRA_OTHERS             (2u)
#define MENU_CONFIG_INPUTS            (PLAYERS)
#define MENU_SUMMARY_MEDALS           (6u)
#define MENU_SUMMARY_ENTRIES          (2u)
#define MENU_SUMMARY_ENTRIES          (2u)
#define MENU_SUMMARY_PARTS            (PLAYERS)
#define MENU_SUMMARY_BANNER_SPEED     (4.0f)
#define MENU_SUMMARY_BANNER_SPEED_REV (1.0f / MENU_SUMMARY_BANNER_SPEED)
#define MENU_SUMMARY_BANNER_ANIMATION (6.8f)
#define MENU_DEFEAT_DELAY_INTRO       (0.5f)
#define MENU_DEFEAT_DELAY_OUTRO       (1.0f)
#define MENU_DEFEAT_CONTINUES         (CONTINUES)
#define MENU_DEFEAT_BANNER_SPEED      (4.0f)
#define MENU_DEFEAT_BANNER_SPEED_REV  (1.0f / MENU_DEFEAT_BANNER_SPEED)
#define MENU_DEFEAT_BANNER_ANIMATION  (2.9f)
#define MENU_FINISH_DELAY_INTRO       (1.0f)
#define MENU_FINISH_PARTS             (PLAYERS)
#define MENU_FINISH_BANNER_SPEED      (4.0f)
#define MENU_FINISH_BANNER_SPEED_REV  (1.0f / MENU_FINISH_BANNER_SPEED)
#define MENU_FINISH_BANNER_ANIMATION  (2.9f)

#define MENU_BUTTON             "menu_background_black.png", "menu_background_black.png"
#define MENU_SWITCHBOX          "default_black.png", "default_black.png"
#define MENU_FONT_DEFAULT       "keifont.ttf"
#define MENU_FONT_STANDARD_1    MENU_FONT_DEFAULT, (20u)
#define MENU_FONT_STANDARD_2    MENU_FONT_DEFAULT, (25u)
#define MENU_FONT_STANDARD_3    MENU_FONT_DEFAULT, (35u)
#define MENU_FONT_STANDARD_4    MENU_FONT_DEFAULT, (46u)
#define MENU_FONT_STANDARD_5    MENU_FONT_DEFAULT, (79u)
#define MENU_FONT_STANDARD_6    MENU_FONT_DEFAULT, (58u)
#define MENU_FONT_DYNAMIC_1     "dynamic_font",    (20u)
#define MENU_FONT_DYNAMIC_2     "dynamic_font",    (25u)
#define MENU_FONT_DYNAMIC_3     "dynamic_font",    (35u)
#define MENU_FONT_DYNAMIC_4     "dynamic_font",    (46u)
#define MENU_FONT_DYNAMIC_5     "dynamic_font",    (79u)
#define MENU_FONT_ICON_1        "fontawesome.ttf", (20u)
#define MENU_FONT_ICON_2        "fontawesome.ttf", (24u)
#define MENU_FONT_ICON_3        "fontawesome.ttf", (40u)
#define MENU_OUTLINE_SMALL      (3u)
#define MENU_OUTLINE_BIG        (4u)


// ****************************************************************
// menu surface indices
enum eSurface : coreUint8
{
    SURFACE_EMPTY = 0u,
    SURFACE_INTRO,
    SURFACE_TITLE,
    SURFACE_MAIN,
    SURFACE_GAME,
    SURFACE_SCORE,
    SURFACE_REPLAY,
    SURFACE_EXTRA,
    SURFACE_CONFIG,
    SURFACE_PAUSE,
    SURFACE_SUMMARY,
    SURFACE_DEFEAT,
    SURFACE_FINISH,
    SURFACE_BRIDGE,
    SURFACE_CREDITS,
    SURFACE_MAX,

    SURFACE_INTRO_EMPTY = 0u,
    SURFACE_INTRO_WELCOME,
    SURFACE_INTRO_LANGUAGE,
    SURFACE_INTRO_MAX,

    SURFACE_TITLE_LOGO = 0u,
    SURFACE_TITLE_FIRST,
    SURFACE_TITLE_MAX,

    SURFACE_MAIN_DEFAULT = 0u,
    SURFACE_MAIN_MAX,

    SURFACE_GAME_MISSION = 0u,
    SURFACE_GAME_SEGMENT,
    SURFACE_GAME_ARCADE,
    SURFACE_GAME_OPTION,
    SURFACE_GAME_ARMORY,
    SURFACE_GAME_DEMO,
    SURFACE_GAME_MAX,

    SURFACE_SCORE_DEFAULT = 0u,
    SURFACE_SCORE_MAX,

    SURFACE_REPLAY_OVERVIEW = 0u,
    SURFACE_REPLAY_DETAILS,
    SURFACE_REPLAY_MAX,

    SURFACE_EXTRA_PROGRESS = 0u,
    SURFACE_EXTRA_STATS,
    SURFACE_EXTRA_OTHER,
    SURFACE_EXTRA_PASSWORD,
    SURFACE_EXTRA_CREDITS,
    SURFACE_EXTRA_MAX,

    SURFACE_CONFIG_VIDEO = 0u,
    SURFACE_CONFIG_AUDIO,
    SURFACE_CONFIG_INPUT,
    SURFACE_CONFIG_GAME,
    SURFACE_CONFIG_MAX,

    SURFACE_PAUSE_DEFAULT = 0u,
    SURFACE_PAUSE_MAX,

    SURFACE_SUMMARY_MISSION_SOLO = 0u,
    SURFACE_SUMMARY_MISSION_COOP,
    SURFACE_SUMMARY_SEGMENT_SOLO,
    SURFACE_SUMMARY_SEGMENT_COOP,
    SURFACE_SUMMARY_BEGINNING,
    SURFACE_SUMMARY_ENDING_NORMAL,
    SURFACE_SUMMARY_ENDING_SECRET,
    SURFACE_SUMMARY_TITLE,
    SURFACE_SUMMARY_MAX,

    SURFACE_DEFEAT_CONTINUE = 0u,
    SURFACE_DEFEAT_GAMEOVER,
    SURFACE_DEFEAT_MAX,

    SURFACE_FINISH_DEFAULT = 0u,
    SURFACE_FINISH_MAX,

    SURFACE_BRIDGE_ENTER = 0u,
    SURFACE_BRIDGE_RETURN_1,
    SURFACE_BRIDGE_RETURN_2,
    SURFACE_BRIDGE_INPUT,
    SURFACE_BRIDGE_UNLOCK,
    SURFACE_BRIDGE_CONTINUE,
    SURFACE_BRIDGE_RESTART,
    SURFACE_BRIDGE_MAX
};


// ****************************************************************
// configuration entry indices
enum eEntry : coreUint8
{
    ENTRY_VIDEO_MONITOR = 0u,
    ENTRY_VIDEO_RESOLUTION,
    ENTRY_VIDEO_DISPLAYMODE,
    ENTRY_VIDEO_ANTIALIASING,
    ENTRY_VIDEO_TEXTUREFILTER,
    ENTRY_VIDEO_RENDERQUALITY,
    ENTRY_VIDEO_SHADOWQUALITY,
    ENTRY_VIDEO_SHAKEEFFECTS,
    ENTRY_VIDEO_FLASHEFFECTS,
    ENTRY_VIDEO_HITSTOPEFFECTS,
    ENTRY_VIDEO,

    ENTRY_AUDIO_GLOBALVOLUME = ENTRY_VIDEO,
    ENTRY_AUDIO_MUSICVOLUME,
    ENTRY_AUDIO_EFFECTVOLUME,
    ENTRY_AUDIO_AMBIENTVOLUME,
    ENTRY_AUDIO_3DSOUND,
    ENTRY_AUDIO,

    ENTRY_INPUT_TYPE = ENTRY_AUDIO,
    ENTRY_INPUT_RUMBLE,
    ENTRY_INPUT_FIREMODE,
    ENTRY_INPUT_MOVEUP,
    ENTRY_INPUT_MOVELEFT,
    ENTRY_INPUT_MOVEDOWN,
    ENTRY_INPUT_MOVERIGHT,
    ENTRY_INPUT_ACTION1,
    ENTRY_INPUT_ACTION2,
    ENTRY_INPUT_ACTION3,
    ENTRY_INPUT_ACTION4,
    ENTRY_INPUT,

    ENTRY_GAME_LANGUAGE = ENTRY_INPUT,
    ENTRY_GAME_TEXTSIZE,
    ENTRY_GAME_GAMEROTATION,
    ENTRY_GAME_GAMESCALE,
    ENTRY_GAME_GAMESPEED,
    ENTRY_GAME_HUDROTATION,
    ENTRY_GAME_HUDSCALE,
    ENTRY_GAME_HUDTYPE,
    ENTRY_GAME_UPDATEFREQ,
    ENTRY_GAME_VERSION,
    ENTRY_GAME_MIRRORMODE,
    ENTRY_MAX
};


// ****************************************************************
// icon codes (UTF-8)
#define __ICON(x)         (r_cast<const coreChar*>(x))
#define ICON_CHECK        (__ICON(u8"\uF00C"))
#define ICON_TIMES        (__ICON(u8"\uF00D"))
#define ICON_POWER_OFF    (__ICON(u8"\uF011"))
#define ICON_REFRESH      (__ICON(u8"\uF021"))
#define ICON_ARROW_LEFT   (__ICON(u8"\uF060"))
#define ICON_ARROW_RIGHT  (__ICON(u8"\uF061"))
#define ICON_ARROW_UP     (__ICON(u8"\uF062"))
#define ICON_ARROW_DOWN   (__ICON(u8"\uF063"))
#define ICON_SHARE        (__ICON(u8"\uF064"))
#define ICON_COGS         (__ICON(u8"\uF085"))
#define ICON_CARET_DOWN   (__ICON(u8"\uF0D7"))
#define ICON_CARET_UP     (__ICON(u8"\uF0D8"))
#define ICON_CARET_LEFT   (__ICON(u8"\uF0D9"))
#define ICON_CARET_RIGHT  (__ICON(u8"\uF0DA"))
#define ICON_PAUSE_CIRCLE (__ICON(u8"\uF28B"))
#define ICON_UNDO_ALT     (__ICON(u8"\uF2EA"))
#define ICON_REFRESH_ALT  (__ICON(u8"\uF2F1"))
#define ICON_REDO_ALT     (__ICON(u8"\uF2F9"))
#define ICON_SHIELD_ALT   (__ICON(u8"\uF3ED"))
#define ICON_BURN         (__ICON(u8"\uF46A"))
#define ICON_FEATHER_ALT  (__ICON(u8"\uF56B"))


// ****************************************************************
// intro menu class
class cIntroMenu final : public coreMenu
{
private:
    cGuiLabel m_WelcomeText;                               // 

    coreTimer m_IntroTimer;                                // intro animation 
    coreUint8 m_iIntroStatus;                              // 

    coreMap<coreString, cGuiButton*> m_apLanguageButton;   // list with buttons for valid language files

    cMenuNavigator m_Navigator;


public:
    cIntroMenu()noexcept;
    ~cIntroMenu()final;

    DISABLE_COPY(cIntroMenu)

    // move the intro menu
    void Move()final;

    // 
    void StartIntro();

    // 
    void ActivateFirstPlay();
};


// ****************************************************************
// title menu class
class cTitleMenu final : public coreMenu
{
private:
    cGuiObject m_GameLogo;          // game logo
    cGuiObject m_GameLogoDemo;      // 

    cGuiLabel m_PromptText;         // 
    coreFlow  m_fPromptAnimation;   // 
    coreFlow  m_fPromptExpand;      // 

    cGuiLabel m_aVersionText[2];    // hard-coded version info strings 


public:
    cTitleMenu()noexcept;

    DISABLE_COPY(cTitleMenu)

    // move the title menu
    void Move()final;

    // 
    void ActivateFirstPlay();
    void DeactivateFirstPlay();
};


// ****************************************************************
// main menu class
class cMainMenu final : public coreMenu
{
private:
    cGuiButton m_StartButton;     // start button
    cGuiButton m_ScoreButton;     // score button
    cGuiButton m_ReplayButton;    // replay button
    cGuiButton m_ExtraButton;     // extra button
    cGuiButton m_ConfigButton;    // config button
    cGuiButton m_SteamButton;     // steam button
    cGuiButton m_CreditsButton;   // credits button
    cGuiButton m_ExitButton;      // exit button

    cMenuNavigator m_Navigator;


public:
    cMainMenu()noexcept;

    DISABLE_COPY(cMainMenu)

    // move the main menu
    void Move()final;

    // 
    void ActivateFirstPlay();
    void DeactivateFirstPlay();

    // 
    void ActivateDemoVersion();

    // 
    inline void ResetNavigator() {m_Navigator.ResetFirst();}
};


// ****************************************************************
// game menu class
class cGameMenu final : public coreMenu
{
private:
    cGuiObject m_BackgroundMain;                                  // 
    cGuiObject m_BackgroundOption;                                // 
    cGuiObject m_BackgroundArmory;                                // 
    cGuiObject m_BackgroundDemo;                                  // 

    cGuiButton m_MissionTab;                                      // 
    cGuiButton m_SegmentTab;                                      // 
    cGuiButton m_ArcadeTab;                                       // 

    cGuiButton m_StartButtonArmory;                               // start button   
    cGuiButton m_StartButtonDemo;                                 // start button   
    cGuiButton m_ContinueButton;                                  // 
    cGuiButton m_BackButtonMain;                                  // back button   
    cGuiButton m_BackButtonOption;                                // back button   
    cGuiButton m_BackButtonArmory;                                // back button   
    cGuiButton m_BackButtonDemo;                                  // back button   

    cGuiLabel m_OptionState;                                      // 

    cGuiLabel  m_MissionHeader;                                   // 
    cGuiLabel  m_aMissionName    [MENU_GAME_MISSIONS];            // 
    cGuiLabel  m_aMissionTitle   [MENU_GAME_MISSIONS];            // 
    cGuiLabel  m_aMissionScore   [MENU_GAME_MISSIONS];            // 
    cGuiLabel  m_aMissionTime    [MENU_GAME_MISSIONS];            // 
    cGuiObject m_aMissionFragment[MENU_GAME_MISSIONS];            // 
    cGuiObject m_aMissionBadge   [MENU_GAME_MISSIONS];            // 
    cGuiLabel  m_aMissionBadgeNum[MENU_GAME_MISSIONS];            // 
    cGuiObject m_aMissionIcon    [MENU_GAME_MISSIONS];            // 
    cGuiObject m_aMissionMedal   [MENU_GAME_MISSIONS];            // 
    cGuiObject m_aMissionLine    [MENU_GAME_MISSIONS];            // 
    cScrollBox m_MissionBox;                                      // 
    coreFlow   m_fMissionTime;                                    // 

    cGuiLabel  m_SegmentHeader;                                   // 
    cGuiLabel  m_aSegmentName     [MENU_GAME_MISSIONS];           // 
    cGuiLabel  m_aSegmentNumber    [MENU_GAME_MISSIONS];           // 
    cGuiObject m_aSegmentIconSmall[MENU_GAME_MISSIONS];           // 
    cGuiObject m_aSegmentLine     [MENU_GAME_MISSIONS];           // 

    cGuiObject m_aaSegmentTile[MENU_GAME_MISSIONS][MENU_GAME_SEGMENTS];   // 
    cGuiObject m_aSegmentCursor    [2];                           // 
    coreUint8  m_aiSegmentSelection[2];                           // 

    cGuiObject m_SegmentArea;                                     // 
    cGuiObject m_SegmentMedal;                                    // 
    cGuiObject m_SegmentFragment;                                 // 
    cGuiObject m_aSegmentBadge    [BADGES];                       // 
    cGuiObject m_aSegmentBadgeWave[BADGES];                       // 
    cGuiLabel  m_aSegmentTitle    [2];                            // 
    cGuiLabel  m_aSegmentScore    [2];                            // 
    cGuiLabel  m_aSegmentTime     [2];                            // 
    cGuiLabel  m_aSegmentMaxSeries[2];                            // 
    cGuiObject m_SegmentIcon;                                     // 

    cGuiLabel  m_ArcadeHeader;                                    // 
    cGuiLabel  m_aArcadeName[MENU_GAME_ARCADES];                  // 
    cGuiObject m_aArcadeLine[MENU_GAME_ARCADES];                  // 

    cGuiButton m_ArcadeComplete;                                  // 

    cGuiLabel  m_OptionHeader;                                    // 
    cGuiLabel  m_aOptionName[MENU_GAME_OPTIONS];                  // 
    cGuiObject m_aOptionLine[MENU_GAME_OPTIONS];                  // 

    cGuiSwitchBox m_OptionType;                                   // 
    cGuiSwitchBox m_OptionMode;                                   // 
    cGuiSwitchBox m_OptionDifficulty;                             // 

    cGuiLabel  m_ArmoryHeader;                                    // 
    cGuiLabel  m_aArmoryName[MENU_GAME_ARMORIES];                 // 
    cGuiObject m_aArmoryLine[MENU_GAME_ARMORIES];                 // 

    cGuiObject m_ArmoryArea;                                      // 
    cGuiObject m_aArmoryMedal    [MENU_GAME_ARMORY_MEDALS];       // 
    cGuiObject m_aArmoryMedalBack[MENU_GAME_ARMORY_MEDALS];       // 
    cGuiObject m_ArmoryFragment;                                  // 
    cGuiObject m_aArmoryBadge     [MENU_GAME_ARMORY_BADGES];      // 
    cGuiObject m_aArmoryBadgeWave [MENU_GAME_ARMORY_BADGES];      // 
    cGuiObject m_aArmoryBadgeBack1[BADGES];                       // 
    cGuiObject m_aArmoryBadgeBack2[BADGES];                       // 
    cGuiLabel  m_aArmoryBadgeDesc [BADGES];                       // 
    cGuiLabel  m_aArmoryBadgeNum  [MENU_GAME_SEGMENTS];           // 
    cGuiLabel  m_aArmoryTitle     [2];                            // 
    cGuiLabel  m_aArmoryScore     [2];                            // 
    cGuiLabel  m_aArmoryTime      [2];                            // 
    cGuiLabel  m_aArmoryMaxSeries [2];                            // 
    cGuiObject m_ArmoryIcon;                                      // 

    cGuiSwitchBox m_ArmoryType;                                   // 
    cGuiSwitchBox m_ArmoryMode;                                   // 
    cGuiSwitchBox m_ArmoryDifficulty;                             // 
    cGuiSwitchBox m_ArmorySpeed;                                  // 
    cGuiSwitchBox m_aArmoryShield[MENU_GAME_PLAYERS];             // 
    cGuiSwitchBox m_aArmoryWeapon[MENU_GAME_PLAYERS];             // 
    cGuiLabel     m_aArmoryPlayer[MENU_GAME_PLAYERS];             // 

    cGuiLabel  m_DemoHeader;                                      // 
    cGuiLabel  m_aDemoName[MENU_GAME_DEMOS];                      // 
    cGuiObject m_aDemoLine[MENU_GAME_DEMOS];                      // 

    cGuiSwitchBox m_DemoType;                                     // 
    cGuiSwitchBox m_DemoDifficulty;                               // 
    cGuiSwitchBox m_DemoSpeed;                                    // 
    cGuiSwitchBox m_DemoStage;                                    // 
    cGuiSwitchBox m_aDemoShield[MENU_GAME_PLAYERS];               // 
    cGuiLabel     m_aDemoPlayer[MENU_GAME_PLAYERS];               // 

    coreUint8 m_iCurPage;                                         // 
    coreUintW m_aiCurIndex [3];                                   // 
    coreUintW m_aiCurIndex2[3];                                   // 

    cMenuNavigator m_NavigatorMain;
    cMenuNavigator m_NavigatorOption;
    cMenuNavigator m_NavigatorArmory;
    cMenuNavigator m_NavigatorDemo;


public:
    cGameMenu()noexcept;

    DISABLE_COPY(cGameMenu)

    // move the game menu
    void Move()final;

    // 
    void ActivateFirstPlay();
    void DeactivateFirstPlay();

    // 
    void LoadValues();
    void SaveValues();

    // 
    void RetrieveStartData(coreInt32* OUTPUT piMissionID, coreUint8* OUTPUT piTakeFrom, coreUint8* OUTPUT piTakeTo, coreUint8* OUTPUT piKind)const;

    // 
    inline const coreUint8& GetSelectedType      ()const                       {return m_ArmoryType      .GetCurValue();}
    inline const coreUint8& GetSelectedMode      ()const                       {return m_ArmoryMode      .GetCurValue();}
    inline const coreUint8& GetSelectedDifficulty()const                       {return m_ArmoryDifficulty.GetCurValue();}
    inline const coreUint8& GetSelectedShield    (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return m_aArmoryShield[iIndex].GetCurValue();}
    inline       coreUint8  GetSelectedWeapon    (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return 1u;}
    inline       coreUint8  GetSelectedSupport   (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return 0u;}

    // 
    inline void ResetNavigator() {m_NavigatorMain.ResetFirst(); m_NavigatorOption.ResetFirst(); m_NavigatorArmory.ResetFirst(); m_NavigatorDemo.ResetFirst();}


private:
    // 
    void __SelectMission(const coreUintW iMissionIndex);
    void __SelectSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);

    // 
    void __PrepareMission(const coreUintW iMissionIndex);
    void __PrepareSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
};


// ****************************************************************
// score menu class
class cScoreMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                        // 

    cGuiButton m_DefaultTab;                        // 

    cGuiButton m_BackButton;                        // back button

    cGuiObject m_aFilterLine[MENU_SCORE_FILTERS];   // 

    cGuiSwitchBox m_FilterMission;                  // 
    cGuiSwitchBox m_FilterSegment;                  // 

    cGuiLabel  m_aRank [MENU_SCORE_ENTRIES];        // 
    cGuiLabel  m_aName [MENU_SCORE_ENTRIES];        // 
    cGuiLabel  m_aScore[MENU_SCORE_ENTRIES];        // 
    cGuiObject m_aLine [MENU_SCORE_ENTRIES];        // 
    cScrollBox         m_ScoreBox;

    cMenuNavigator m_Navigator;


public:
    cScoreMenu()noexcept;

    DISABLE_COPY(cScoreMenu)

    // move the score menu
    void Move()final;

    // 
    void LoadMissions();
    void LoadSegments(const coreUintW iIndex);
};


// ****************************************************************
// replay menu class
class cReplayMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                   // 

    cGuiButton m_DefaultTab;                   // 

    cGuiButton m_StartButton;                  // 
    cGuiButton m_DeleteButton;                 // 
    cGuiButton m_RenameButton;                 // 
    cGuiButton m_BackButton;                   // back button

    cGuiSwitchBox m_Page;                      // 
    cGuiObject    m_PageLine;                  // 

    cGuiLabel  m_aName[MENU_REPLAY_ENTRIES];   // 
    cGuiLabel  m_aTime[MENU_REPLAY_ENTRIES];   // 
    cGuiObject m_aLine[MENU_REPLAY_ENTRIES];   // 

    coreList<cReplay::sInfo> m_aInfoList;      // 
    coreUintW m_iSelected;                     // 

    cMenuNavigator m_Navigator;


public:
    cReplayMenu()noexcept;

    DISABLE_COPY(cReplayMenu)

    // move the replay menu
    void Move()final;

    // 
    void LoadOverview();
    void LoadDetails(const coreUintW iIndex);
};


// ****************************************************************
// extra menu class
class cExtraMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                        // 

    cGuiButton m_ProgressTab;                       // 
    cGuiButton m_StatsTab;                          // 
    cGuiButton m_OtherTab;                          // 

    cGuiButton m_BackButton;                        // back button

    cGuiObject m_ProgressArea;                      // 
    cGuiObject m_aFragment[MENU_EXTRA_FRAGMENTS];   // 

    cGuiObject m_aFilterLine[MENU_EXTRA_FILTERS];   // 

    cGuiSwitchBox m_FilterMission;                  // 
    cGuiSwitchBox m_FilterSegment;                  // 

    cGuiLabel  m_aStatsName [MENU_EXTRA_STATS];     // 
    cGuiLabel  m_aStatsValue[MENU_EXTRA_STATS];     // 
    cGuiObject m_aStatsLine [MENU_EXTRA_STATS];     // 
    cScrollBox m_StatsBox;                          // 

    cGuiLabel  m_aOtherName[MENU_EXTRA_OTHERS];     // 
    cGuiObject m_aOtherLine[MENU_EXTRA_OTHERS];     // 

    cGuiButton m_Password;                          // 
    cGuiButton m_Credits;                           // 

    cGuiLabel m_PasswordHeader;                     // 

    cMenuNavigator m_Navigator;


public:
    cExtraMenu()noexcept;

    DISABLE_COPY(cExtraMenu)

    // move the extra menu
    void Move()final;

    // 
    void LoadMissions();
    void LoadSegments(const coreUintW iIndex);


private:
    // 
    void __UpdateStats();
};


// ****************************************************************
// config menu class
class cConfigMenu final : public coreMenu
{
private:
    // 
    struct sPlayerInput final
    {
        cGuiLabel     oHeader;
        cGuiSwitchBox oType;
        cGuiSwitchBox oRumble;
        cGuiSwitchBox oFireMode;
        cGuiButton    oMoveUp;
        cGuiButton    oMoveLeft;
        cGuiButton    oMoveDown;
        cGuiButton    oMoveRight;
        cGuiButton    aAction[INPUT_KEYS_ACTION];
        cFigure       oFigureMoveUp;
        cFigure       oFigureMoveLeft;
        cFigure       oFigureMoveDown;
        cFigure       oFigureMoveRight;
        cFigure       aFigureAction[INPUT_KEYS_ACTION];
    };


private:
    cGuiObject m_Background;                   // 

    cGuiButton m_VideoTab;                     // 
    cGuiButton m_AudioTab;                     // 
    cGuiButton m_InputTab;                     // 
    cGuiButton m_GameTab;                      // 

    cGuiButton m_SaveButton;                   // save button
    cGuiButton m_DiscardButton;                // discard button
    cGuiButton m_BackButton;                   // back button

    cGuiLabel  m_aLabel   [ENTRY_MAX];         // 
    cGuiObject m_aLine    [ENTRY_MAX];         // 
    cGuiLabel  m_aCueInput[INPUT_KEYS];        // 
    cGuiLabel  m_aCueRota [2];                 // 

    cGuiLabel       m_Description;             // 
    const coreChar* m_apcDescKey[ENTRY_MAX];   // 

    cGuiSwitchBox m_Monitor;
    cGuiSwitchBox m_Resolution;
    cGuiSwitchBox m_DisplayMode;
    cGuiSwitchBox m_AntiAliasing;
    cGuiSwitchBox m_TextureFilter;
    cGuiSwitchBox m_RenderQuality;
    cGuiSwitchBox m_ShadowQuality;
    cGuiSwitchBox m_ShakeEffects;
    cGuiSwitchBox m_FlashEffects;
    cGuiSwitchBox m_HitStopEffects;
    cGuiSwitchBox m_GlobalVolume;
    cGuiSwitchBox m_MusicVolume;
    cGuiSwitchBox m_EffectVolume;
    cGuiSwitchBox m_AmbientVolume;
    cGuiSwitchBox m_3DSound;
    cGuiSwitchBox m_Language;
    cGuiSwitchBox m_TextSize;
    cGuiSwitchBox m_GameRotation;
    cGuiSwitchBox m_GameScale;
    cGuiSwitchBox m_GameSpeed;
    cGuiSwitchBox m_HudRotation;
    cGuiSwitchBox m_HudScale;
    cGuiSwitchBox m_HudType;
    cGuiSwitchBox m_UpdateFreq;
    cGuiSwitchBox m_Version;
    cGuiSwitchBox m_MirrorMode;

    sPlayerInput m_aInput[MENU_CONFIG_INPUTS];
    cGuiButton   m_SwapInput;

    coreUintW                      m_iCurMonitorIndex;
    coreMap<coreUintW, coreString> m_asCurResolution;

    cMenuNavigator m_Navigator;


public:
    cConfigMenu()noexcept;

    DISABLE_COPY(cConfigMenu)

    // move the config menu
    void Move()final;

    // 
    void CheckValues();
    void LoadValues();
    void SaveValues();

    // 
    inline void ResetNavigator() {m_Navigator.ResetFirst();}

    // convert input key to readable string                 
    static void PrintFigure(cFigure* OUTPUT pFigure, const coreUint8 iType, const coreInt16 iKey);


private:
    // 
    void __UpdateRenderQuality();
    void __UpdateShadowQuality();
    void __UpdateVolume();
    void __UpdateLanguage();
    void __UpdateInterface();

    // 
    void __LoadMonitors();
    void __LoadResolutions(const coreUintW iMonitorIndex);
    void __LoadFrequencies(const coreUintW iMonitorIndex);
    void __LoadInputs();

    // 
    inline cGuiButton& __RetrieveInputButton  (const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < MENU_CONFIG_INPUTS) && (iKeyIndex < INPUT_KEYS)) return *((&m_aInput[iPlayerIndex].oMoveUp)                                         + iKeyIndex);}
    inline cFigure&    __RetrieveInputFigure  (const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < MENU_CONFIG_INPUTS) && (iKeyIndex < INPUT_KEYS)) return *((&m_aInput[iPlayerIndex].oFigureMoveUp)                                     + iKeyIndex);}
    inline coreInt16&  __RetrieveInputCurValue(const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < INPUT_TYPES)        && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[g_CurConfig.Input.aiType[iPlayerIndex]].iMoveUp) + iKeyIndex);}
    inline coreInt16&  __RetrieveInputDirValue(const coreUintW iType,        const coreUintW iKeyIndex) {ASSERT((iType        < INPUT_SETS)         && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[iType].iMoveUp)                                  + iKeyIndex);}

    // 
    static inline coreFloat __VolumeToFloat(const coreUint8 iVolume) {return I_TO_F(iVolume) * 0.01f;}
    static inline coreUint8 __VolumeToUint8(const coreFloat fVolume) {return F_TO_UI(ROUND(CLAMP(fVolume, 0.0f, 1.0f) * 20.0f) * 5.0f);}
};


// ****************************************************************
// pause menu class
class cPauseMenu final : public coreMenu
{
private:
    cGuiButton m_ResumeButton;    // resume button
    cGuiButton m_RestartButton;   // restart button
    cGuiButton m_ConfigButton;    // config button
    cGuiButton m_ExitButton;      // exit button

    cMenuNavigator m_Navigator;


public:
    cPauseMenu()noexcept;

    DISABLE_COPY(cPauseMenu)

    // move the pause menu
    void Move()final;

    // 
    void ActivateFirstPlay();
    void DeactivateFirstPlay();

    // 
    void ResetNavigator();
};


// ****************************************************************
// summary menu class
class cSummaryMenu final : public coreMenu
{
private:
    // 
    enum eSummaryState : coreUint8
    {
        SUMMARY_INTRO   = 0u,
        SUMMARY_SKIPPED = 1u,
        SUMMARY_WAIT    = 2u,
        SUMMARY_OUTRO   = 3u
    };


private:
    cGuiObject m_BackgroundMain;                                    // 
    cGuiObject m_BackgroundCoop;                                    // 

    cGuiButton m_RestartButton;                                     // restart button
    cGuiButton m_ExitButton;                                        // exit button

    cGuiLabel m_aTitle[2];                                          // 

    cGuiLabel m_aHeader[2];                                         // 

    cGuiObject m_MedalMission;                                      // 
    cGuiObject m_aMedalSegment[MENU_SUMMARY_MEDALS];                // 

    cGuiLabel m_aName [MENU_SUMMARY_ENTRIES];                       // 
    cGuiLabel m_aValue[MENU_SUMMARY_ENTRIES];                       // 
    cGuiLabel m_aaPart[MENU_SUMMARY_ENTRIES][MENU_SUMMARY_PARTS];   // 

    cGuiLabel m_TotalName;                                          // 
    cGuiLabel m_TotalValue;                                         // 
    cGuiLabel m_aTotalPart[MENU_SUMMARY_PARTS];                     // 
    cGuiLabel m_TotalBest;                                          // 

    coreUint32 m_iFinalValue;                                       // (just for display) 
    coreUint32 m_aiFinalPart [MENU_SUMMARY_PARTS];                  // (just for display) 
    coreUint32 m_aiApplyBonus[MENU_SUMMARY_PARTS];                  // 
    coreUint8  m_aiApplyMedal[MENU_SUMMARY_PARTS];                  // 


#define MENU_SUMMARY_ENTRIES_SEGMENT (3u)
#define MENU_SUMMARY_SIDES (2u)

    cGuiLabel m_aSegmentName [MENU_SUMMARY_ENTRIES_SEGMENT];                       // 
    cGuiLabel m_aaSegmentValue[MENU_SUMMARY_ENTRIES_SEGMENT][MENU_SUMMARY_SIDES];                       // 
    cGuiLabel m_aaSegmentPart[MENU_SUMMARY_ENTRIES_SEGMENT][MENU_SUMMARY_PARTS];   // 
    
    cGuiLabel m_aSegmentShift[MENU_SUMMARY_SIDES];                       // 
    
    cGuiObject m_SegmentMedal;
    
    coreUint8 m_iSelection;
    
    
    cGuiLabel m_aPerfect[2];
    cGuiLabel m_aRecord[4];
    coreFlow  m_afSignalTime[2];
    coreUint8 m_iSignalActive;
    

    coreFlow m_fIntroTimer;                                         // 
    coreFlow m_fOutroTimer;                                         // 

    coreFloat m_fFinalSpinOld;                                      // 

    eSummaryState m_eState;                                         // 

    cMenuNavigator m_Navigator;


public:
    cSummaryMenu()noexcept;

    DISABLE_COPY(cSummaryMenu)

    // render and move the summary menu
    void Render()final;
    void Move  ()final;

    // 
    void ShowMission();
    void ShowSegment();
    void ShowBeginning();
    void ShowEndingNormal();
    void ShowEndingSecret();

    // 
    void SetHighlightColor(const coreVector3 vColor);


private:
    // 
    void __SetMedalMission(const coreUint8 iMedal);
    void __SetMedalSegment(const coreUintW iIndex, const coreUint8 iMedal);

    // 
    void __ResetState();
};


// ****************************************************************
// defeat menu class
class cDefeatMenu final : public coreMenu
{
private:
    // 
    enum eDefeatState : coreUint8
    {
        DEFEAT_INTRO = 0u,
        DEFEAT_WAIT  = 1u,
        DEFEAT_BURST = 2u,
        DEFEAT_OUTRO = 3u
    };


private:
    cGuiObject m_Background;                             // 

    cGuiLabel  m_GameOverText;                           // 
    cGuiLabel  m_ContinueText;                           // 
    cGuiLabel  m_ContinueTimer;                          // 
    cGuiObject m_ContinueImage[MENU_DEFEAT_CONTINUES];   // 

    coreFlow m_fCountdown;                               // 
    coreFlow m_fBurst;                                   // 

    coreFlow m_fIntroTimer;                              // 
    coreFlow m_fOutroTimer;                              // 

    eDefeatState m_eState;                               // 


public:
    cDefeatMenu()noexcept;

    DISABLE_COPY(cDefeatMenu)

    // render and move the defeat menu
    void Render()final;
    void Move  ()final;

    // 
    void ShowContinue();
    void ShowGameOver();


private:
    // 
    void __ResetState();
};


// ****************************************************************
// finish menu class
class cFinishMenu final : public coreMenu
{
private:
    // 
    enum eFinishState : coreUint8
    {
        FINISH_INTRO = 0u,
        FINISH_WAIT  = 1u,
        FINISH_OUTRO = 2u
    };


private:
    cGuiObject m_Background;                     // 

    cGuiLabel m_ThankYouText;                    // 

    cGuiLabel m_TotalName;                       // 
    cGuiLabel m_TotalValue;                      // 
    cGuiLabel m_aTotalPart[MENU_FINISH_PARTS];   // 

    cGuiButton m_SteamButton;                    // steam button
    cGuiButton m_ExitButton;                     // exit button

    coreFlow m_fIntroTimer;                      // 
    coreFlow m_fOutroTimer;                      // 

    eFinishState m_eState;                       // 

    cMenuNavigator m_Navigator;


public:
    cFinishMenu()noexcept;

    DISABLE_COPY(cFinishMenu)

    // render and move the finish menu
    void Render()final;
    void Move  ()final;

    // 
    void ShowThankYou();

    // 
    void SetHighlightColor(const coreVector3 vColor);
    // TODO 1: this menu is handling score and replay saving


private:
    // 
    void __ResetState();
};


// ****************************************************************
// bridge menu class
class cBridgeMenu final : public coreMenu
{
private:
    cGuiLabel m_InputHeader;          // 

    cGuiLabel  m_UnlockHeader;        // 
    cGuiLabel  m_UnlockName;          // 
    cGuiLabel  m_UnlockDescription;   // 
    cGuiObject m_UnlockIcon;          // 

    coreFlow m_fReturnTimer;          // 
    coreBool m_bReturnState;          // 

    coreUint8 m_iTarget;              // 
    coreBool  m_bPaused;              // 
    coreBool  m_bFade;                // 


public:
    cBridgeMenu()noexcept;

    DISABLE_COPY(cBridgeMenu)

    // move the bridge menu
    void Move()final;

    // 
    void EnterGame  (const coreBool bFade);
    void ReturnMenu (const coreUint8 iTarget, const coreBool bPaused, const coreBool bFade);
    void ShowUnlock ();
    void UseContinue();
    void UseRestart (const coreBool bPaused);

    // 
    coreBool RequiresUnlock()const;

    // 
    inline coreUint8 GetTarget()const {return m_iTarget;}
    inline coreBool  GetPaused()const {return m_bPaused;}


private:
    // 
    void __ResetState();
};


// ****************************************************************
// primary menu class
class cMenu final : public coreMenu, public coreResourceRelation
{
private:
    // 
    struct sButtonData final
    {
        coreVector2 vPosition;    // 
        coreVector2 vSize;        // 
        coreVector2 vTexOffset;   // 
        coreFlow    fTime;        // 
        coreBool    bGrow;        // 
    };


private:
    cIntroMenu   m_IntroMenu;            // intro menu object
    cTitleMenu   m_TitleMenu;            // title menu object
    cMainMenu    m_MainMenu;             // main menu object
    cGameMenu    m_GameMenu;             // game menu object
    cScoreMenu   m_ScoreMenu;            // score menu object
    cReplayMenu  m_ReplayMenu;           // replay menu object
    cExtraMenu   m_ExtraMenu;            // extra menu object
    cConfigMenu  m_ConfigMenu;           // config menu object
    cPauseMenu   m_PauseMenu;            // pause menu object
    cSummaryMenu m_SummaryMenu;          // summary menu object
    cDefeatMenu  m_DefeatMenu;           // defeat menu object
    cFinishMenu  m_FinishMenu;           // finish menu object
    cBridgeMenu  m_BridgeMenu;           // bridge menu object

    cMsgBox      m_MsgBox;               // message box overlay
    cTooltip     m_Tooltip;              // tooltip overlay
    cArcadeInput m_ArcadeInput;          // 
    cCreditRoll  m_CreditRoll;           // 

    coreFullscreen m_PauseLayer;         // 
    coreUint32     m_iPauseFrame;        // 

    coreFrameBuffer m_aFrameBuffer[3];   // 
    coreObject2D    m_MixObject;         // 
    
    coreUint8 iForceA;
    coreUint8 iForceB;

    coreTimer m_TransitionTime;          // 
    coreUint8 m_iTransitionState;        // 
    coreMenu* m_pTransitionMenu;         // 

    static coreVector3 m_vHighlightColor;       // 
    static coreVector3 m_vButtonColor;       // 
    
    static cGuiButton* m_apCurButton[2];
    static cGuiButton* m_apNewButton[2];
    static cGuiButton* m_pCurTab;
    static cGuiButton* m_pNewTab;
    static cGuiObject* m_pCurLine;
    static cGuiObject* m_pNewLine;
    
    static coreMap<const void*, sButtonData> s_aButtonData;


public:
    cMenu()noexcept;
    ~cMenu()final;

    DISABLE_COPY(cMenu)

    // render and move the menu
    void Render()final;
    void Move  ()final;

    // access special menu overlays
    inline cMsgBox*      GetMsgBox     () {return &m_MsgBox;}
    inline cTooltip*     GetTooltip    () {return &m_Tooltip;}
    inline cArcadeInput* GetArcadeInput() {return &m_ArcadeInput;}
    inline cCreditRoll*  GetCreditRoll () {return &m_CreditRoll;}

    // 
    coreBool IsPaused()const;
    coreBool IsPausedWithStep();
    inline void InvokePauseStep() {m_iPauseFrame = Core::System->GetCurFrame();}

    // 
    void ShiftSurface(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface, const coreFloat fSpeed, const coreUint8 iSound, const coreBool bUpdateFrom = false, const coreBool bUpdateTo = false);
    inline coreBool IsShifting()const {return m_TransitionTime.GetStatus();}

    // 
    void SetHighlightColor(const coreVector3 vColor);
    void SetButtonColor(const coreVector3 vColor);
   
    
    inline const coreVector3& GetHighlightColor()const {return m_vHighlightColor;}
    inline const coreVector3& GetButtonColor()const {return m_vButtonColor;}

    // 
    static void UpdateLanguageFont();
    static const coreMap<coreString, coreString>& GetLanguageList();
    
    static void ChangeTab(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface);
    
    static void ClearScreen();
    
    static const coreChar* GetMissionLetters(const coreUintW iMissionIndex);
    static const coreChar* GetSegmentLetters(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);

    // menu helper routines
    static void UpdateButton        (cGuiButton*    OUTPUT pButton, const coreBool bFocused, const coreVector3 vFocusColor, const coreBool bGrow = true);
    static void UpdateButton        (cGuiButton*    OUTPUT pButton, const coreBool bFocused, const coreBool bGrow = true);
    static void UpdateTab           (cGuiButton*    OUTPUT pTab, const coreBool bLocked, const coreBool bFocused, const coreVector3 vFocusColor);
    static void UpdateTab           (cGuiButton*    OUTPUT pTab, const coreBool bLocked, const coreBool bFocused);
    static void UpdateSwitchBox     (cGuiSwitchBox* OUTPUT pSwitchBox);
    static void UpdateLine          (cGuiObject*    OUTPUT pLine, const coreBool bInteract, const coreVector3 vFocusColor);
    static void UpdateLine          (cGuiObject*    OUTPUT pLine, const coreBool bInteract);
    static void UpdateAnimateProgram(cGuiObject*    OUTPUT pObject);
    static void ApplyMedalTexture   (cGuiObject*    OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType);
    
    static void ClearButtonTime(const cGuiButton* pButton);


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __StartGame();
    void __EndGame();
};


#endif // _P1_GUARD_MENU_H_