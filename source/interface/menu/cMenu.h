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

// TODO 3: unload fonts currently not used (e.g. from score-menu)
// TODO 3: options menu: 15 second on video change, yes, no
// TODO 5: display unattached joysticks and joystick names somehow
// TODO 3: display gamepad/device name in options description
// TODO 3: highlight which joystick is which input set
// TODO 3: update texture filter and render quality in realtime (resource-manager reset ?)
// TODO 3: swapping controls should swap buttons visually
// TODO 3: load replay number only on first entry, load headers async on demand, handle changes during runtime
// TODO 4: locked config input buttons are set with dummy values before getting locked
// TODO 2: [MF] handle plugging in and out gamepad while in config menu, handle plugging out gamepad in general (when controller is currently selected)
// TODO 3: add small text movement if switch-box changes
// TODO 1: click-wave when clicking on active button or tab (not for switch-box), what about stage tile ?
// TODO 3: whole menu wobbles/shifts when you change tab
// TODO 3: menu sound response is super-confused with gamepad-input, related to having focus on multiple sub-menus (unnecessary button-fokus sound -> catch initial focusing), and being able to focus disabled buttons (active tab, save button)
// TODO 3: de-couple interface-class and display in options when game is not running (for HUD type option)
// TODO 3: if you switch between borderless and windowed with Desktop resolution, the 0:0 is overwritten in the size change event and the save button remains active
// TODO 3: if the resolution is too big in windowed mode, it will be clamped and the save button remains active
// TODO 4: why do I have ChangeLanguage for switch-boxes, what does it do, should I add it to the engine instead?
// TODO 4: switch more menus to SetHighlightColor, which is more efficient
// TODO 2: this->ChangeSurface(XXX, 0.0f) wrap with timeless
// TODO 4: remove unnecessary tiles, medals, and others related to the missing Ater segments (and possible second bosses everywhere else)
// TODO 1: show mission summary for Ater before arcade summary on ?
// TODO 1: show player separation for arcade summary
// TODO 3: add auto-scroll for armory-segment-switch, for shoulder-buttons and arrow-keys
// TODO 2: latam language, text in summary menu is a little too close together
// TODO 1: optimize all menu render orderings, especially for shaders, especially in the game menu (also bind-object, in ALL menus and view-boxes)
// TODO 1: (remove mission page in armory)
// TODO 3: get rid of the additional interface object in config-menu
// TODO 3: when switching resolution (AA change, engine reset), the next mouse click is not recognized (no press event is coming from SDL, only the release event) (not always!)
// TODO 3: support for chinese names in leaderboard (s+t)
// TODO 3: hidden medals on arcade summary will blend in after 10000.0f
// TODO 3: make use of coreTranslate in menus (inherited, like in interface) to simplify language-string updates
// TODO 3: entering a menu with option-lines will always trigger a sound-effect (with gamepad), even though it's unnecessary, TIME+GetAlpha changes did not help

// NOTE: only short YES-NO questions: Exit Game ? Return to Menu ?
// NOTE: every object in menu needs outline: weapons, medals, icons


// ****************************************************************
// menu definitions
#define MENU_LIGHT_ACTIVE             (1.0f)                        // visible strength of active menu objects
#define MENU_LIGHT_IDLE               (0.65f)                       // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE           (0.8f)                        // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK           (0.04f)                       // black contrast value
#define MENU_CONTRAST_INSIDE          (0.92f)                       // 
#define MENU_INSIDE_ALPHA             (0.95f)                       // 
#define MENU_BUTTON_SHIFT             (coreVector2(0.0001f,0.0f))   // prevent rounding-issue and rectify-shift of caption on focus on 720p

#define MENU_RESOLUTION_DESKTOP       (0xEEu)
#define MENU_RESOLUTION_CUSTOM        (0xFFu)

#define MENU_LAYER_TEXOFFSET          (coreVector2(0.0f, FRACT(-0.04f * Core::System->GetTotalTimeFloat(100.0))))

#define MENU_GAME_PLAYERS             (PLAYERS)
#define MENU_GAME_MISSIONS            (11u)
#define MENU_GAME_SEGMENTS            (6u)
#define MENU_GAME_ARCADES             (1u)
#define MENU_GAME_ARMORIES            (6u)
#define MENU_GAME_FIRSTS              (4u)
#define MENU_GAME_DEMOS               (5u)
#define MENU_GAME_ARROWS              (MENU_GAME_SEGMENTS - 1u)
#define MENU_GAME_ARMORY_MEDALS       (9u + 1u)
#define MENU_GAME_ARMORY_BADGES       (BADGES - 4u)
#define MENU_GAME_ARMORY_BADGES_ALL   (MENU_GAME_ARMORY_BADGES * WAVES)
#define MENU_GAME_ARMORY_HELPERS      (8u)
#define MENU_GAME_ARMORY_ICONS        (9u)
#define MENU_GAME_ARMORY_ARROWS       (8u)
#define MENU_SCORE_FILTERS            (4u)
#define MENU_SCORE_ENTRIES            (20u)
#define MENU_REPLAY_ENTRIES           (20u)
#define MENU_REPLAY_PLAYERS           (PLAYERS)
#define MENU_REPLAY_MISSIONS          (MISSIONS - 3u)
#define MENU_REPLAY_DETAILS           (5u + MENU_REPLAY_DETAIL_DATAS + MENU_REPLAY_MISSIONS)
#define MENU_REPLAY_DETAIL_DATAS      (3u)
#define MENU_REPLAY_DETAIL_RUNS       (CONTINUES + 1u)
#define MENU_REPLAY_DETAIL_MEDALS     (MENU_GAME_ARMORY_MEDALS)
#define MENU_REPLAY_DETAIL_ICONS      (MENU_GAME_ARMORY_ICONS)
#define MENU_REPLAY_DETAIL_ARROWS     (MENU_GAME_ARMORY_ARROWS)
#define MENU_REPLAY_MISSION_OFFSET    (MENU_REPLAY_DETAILS - MENU_REPLAY_MISSIONS)
#define MENU_EXTRA_TROPHIES           (48u)
#define MENU_EXTRA_FILTERS            (3u)
#define MENU_EXTRA_STATS              (27u)
#define MENU_EXTRA_OTHERS             (3u)
#define MENU_CONFIG_INPUTS            (PLAYERS)
#define MENU_PAUSE_RESUME_POSITION    (coreVector2(0.0f,0.135f))
#define MENU_SUMMARY_ARCADES          (9u)
#define MENU_SUMMARY_RUNS             (CONTINUES + 1u)
#define MENU_SUMMARY_MEDALS           (6u)
#define MENU_SUMMARY_ENTRIES          (2u)
#define MENU_SUMMARY_PARTS            (PLAYERS)
#define MENU_SUMMARY_BANNER_SPEED     (4.0f)
#define MENU_SUMMARY_BANNER_SPEED_REV (1.0f / MENU_SUMMARY_BANNER_SPEED)
#define MENU_SUMMARY_BANNER_ANIMATION (6.8f)
#define MENU_SUMMARY_SKIP_MISSION     (5.5f)
#define MENU_SUMMARY_SKIP_SEGMENT     (2.2f)
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

STATIC_ASSERT(REPLAY_SLOTS <= MENU_REPLAY_ENTRIES)

#define MENU_BUTTON             "menu_background_black.png", "menu_background_black.png"
#define MENU_SWITCHBOX          "default_black.webp", "default_black.webp"
#define MENU_FONT_STANDARD      "keifont.ttf"
#define MENU_FONT_DYNAMIC       "dynamic_font"
#define MENU_FONT_ICON          "fontawesome.ttf"
#define MENU_FONT_STANDARD_1    MENU_FONT_STANDARD, (20u)
#define MENU_FONT_STANDARD_2    MENU_FONT_STANDARD, (25u)
#define MENU_FONT_STANDARD_3    MENU_FONT_STANDARD, (35u)
#define MENU_FONT_STANDARD_4    MENU_FONT_STANDARD, (46u)
#define MENU_FONT_STANDARD_5    MENU_FONT_STANDARD, (79u)
#define MENU_FONT_STANDARD_6    MENU_FONT_STANDARD, (58u)
#define MENU_FONT_DYNAMIC_1     MENU_FONT_DYNAMIC,  (20u)
#define MENU_FONT_DYNAMIC_2     MENU_FONT_DYNAMIC,  (25u)
#define MENU_FONT_DYNAMIC_3     MENU_FONT_DYNAMIC,  (35u)
#define MENU_FONT_DYNAMIC_4     MENU_FONT_DYNAMIC,  (46u)
#define MENU_FONT_DYNAMIC_5     MENU_FONT_DYNAMIC,  (79u)
#define MENU_FONT_ICON_1        MENU_FONT_ICON,     (20u)
#define MENU_FONT_ICON_2        MENU_FONT_ICON,     (24u)
#define MENU_FONT_ICON_3        MENU_FONT_ICON,     (30u)
#define MENU_FONT_ICON_4        MENU_FONT_ICON,     (44u)
#define MENU_OUTLINE_SMALL      (3u)
#define MENU_OUTLINE_BIG        (4u)
#define MENU_SWITCHBOX_ZOOM     (3u)

enum eMenuUpdate : coreUint8
{
    MENU_UPDATE_DEFAULT  = 0x00u,
    MENU_UPDATE_NO_GROW  = 0x01u,
    MENU_UPDATE_NO_SOUND = 0x02u,
    MENU_UPDATE_STATIC   = 0x04u
};
ENABLE_BITWISE(eMenuUpdate)


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
    SURFACE_INTRO_IMPORT,
    SURFACE_INTRO_MAX,

    SURFACE_TITLE_LOGO = 0u,
    SURFACE_TITLE_FIRST,
    SURFACE_TITLE_MAX,

    SURFACE_MAIN_DEFAULT = 0u,
    SURFACE_MAIN_MAX,

    SURFACE_GAME_MAIN = 0u,
    SURFACE_GAME_ARMORY,
    SURFACE_GAME_FIRST,
    SURFACE_GAME_DEMO,
    SURFACE_GAME_MAX,

    SURFACE_SCORE_DEFAULT = 0u,
    SURFACE_SCORE_MAX,

    SURFACE_REPLAY_OVERVIEW = 0u,
    SURFACE_REPLAY_SLOTS,
    SURFACE_REPLAY_DETAILS,
    SURFACE_REPLAY_RENAME,
    SURFACE_REPLAY_MAX,

    SURFACE_EXTRA_TROPHY = 0u,
    SURFACE_EXTRA_STATS,
    SURFACE_EXTRA_OTHER,
    SURFACE_EXTRA_PASSWORD,
    SURFACE_EXTRA_CREDITS,
    SURFACE_EXTRA_MUSIC,
    SURFACE_EXTRA_MAX,

    SURFACE_CONFIG_VIDEO = 0u,
    SURFACE_CONFIG_AUDIO,
    SURFACE_CONFIG_INPUT,
    SURFACE_CONFIG_GAME,
    SURFACE_CONFIG_MAX,

    SURFACE_PAUSE_FULL = 0u,
    SURFACE_PAUSE_LIGHT,
    SURFACE_PAUSE_MAX,

    SURFACE_SUMMARY_ARCADE_1 = 0u,
    SURFACE_SUMMARY_ARCADE_2,
    SURFACE_SUMMARY_MISSION_SOLO,
    SURFACE_SUMMARY_MISSION_COOP,
    SURFACE_SUMMARY_SEGMENT_SOLO,
    SURFACE_SUMMARY_SEGMENT_COOP,
    SURFACE_SUMMARY_BEGINNING,
    SURFACE_SUMMARY_TITLE,
    SURFACE_SUMMARY_ENDING_NORMAL,
    SURFACE_SUMMARY_ENDING_SECRET,
    SURFACE_SUMMARY_MAX,

    SURFACE_DEFEAT_CONTINUE = 0u,
    SURFACE_DEFEAT_GAMEOVER,
    SURFACE_DEFEAT_REPLAY,
    SURFACE_DEFEAT_MAX,

    SURFACE_FINISH_DEFAULT = 0u,
    SURFACE_FINISH_MAX,

    SURFACE_BRIDGE_ENTER = 0u,
    SURFACE_BRIDGE_RETURN_1,
    SURFACE_BRIDGE_RETURN_2,
    SURFACE_BRIDGE_INPUT,
    SURFACE_BRIDGE_CONTINUE,
    SURFACE_BRIDGE_NEXT,
    SURFACE_BRIDGE_RESTART,
    SURFACE_BRIDGE_UNLOCK,
    SURFACE_BRIDGE_MAX
};


// ****************************************************************
// configuration entry indices
enum eEntry : coreUint8
{
    ENTRY_VIDEO_MONITOR = 0u,
    ENTRY_VIDEO_RESOLUTION,
    ENTRY_VIDEO_DISPLAYMODE,
    ENTRY_VIDEO_VSYNC,
    ENTRY_VIDEO_ANTIALIASING,
    ENTRY_VIDEO_TEXTUREFILTER,
    ENTRY_VIDEO_RENDERQUALITY,
    ENTRY_VIDEO_SHADOWQUALITY,
    ENTRY_VIDEO_WATERQUALITY,
    ENTRY_VIDEO_PARTICLEEFFECTS,
    ENTRY_VIDEO_SHAKEEFFECTS,
    ENTRY_VIDEO_FLASHEFFECTS,
    ENTRY_VIDEO_HITSTOPEFFECTS,
    ENTRY_VIDEO_CHROMAEFFECTS,
    ENTRY_VIDEO,

    ENTRY_AUDIO_GLOBALVOLUME = ENTRY_VIDEO,
    ENTRY_AUDIO_MUSICVOLUME,
    ENTRY_AUDIO_EFFECTVOLUME,
    ENTRY_AUDIO_AMBIENTVOLUME,
    ENTRY_AUDIO_MENUVOLUME,
    ENTRY_AUDIO_QUALITY,
    ENTRY_AUDIO_MODE,
    ENTRY_AUDIO_3DSOUND,
    ENTRY_AUDIO,

    ENTRY_INPUT_TYPE = ENTRY_AUDIO,
    ENTRY_INPUT_RUMBLE,
    ENTRY_INPUT_FIREMODE,
    ENTRY_INPUT_CONTROLMODE,
    ENTRY_INPUT_ACTION4,
    ENTRY_INPUT_ACTION5,
    ENTRY_INPUT_ACTION6,
    ENTRY_INPUT_ACTION7,
    ENTRY_INPUT_ACTION10,
    ENTRY_INPUT_ACTION1,
    ENTRY_INPUT_ACTION9,
    ENTRY_INPUT_ACTION2,
    ENTRY_INPUT_ACTION3,
    ENTRY_INPUT_ACTION8,
    ENTRY_INPUT_MOVEUP,
    ENTRY_INPUT_MOVELEFT,
    ENTRY_INPUT_MOVEDOWN,
    ENTRY_INPUT_MOVERIGHT,
    ENTRY_INPUT,

    ENTRY_GAME_LANGUAGE = ENTRY_INPUT,
    ENTRY_GAME_GAMEDIRECTION,
    ENTRY_GAME_MIRRORMODE,
    ENTRY_GAME_HUDDIRECTION,
    ENTRY_GAME_HUDTYPE,
    ENTRY_GAME_COMBATTEXT,
    ENTRY_GAME_BACKROTATION,
    ENTRY_GAME_BACKSPEED,
    ENTRY_GAME_UPDATEFREQ,
    ENTRY_GAME_PUREMODE,
    ENTRY_GAME_LEADERBOARD,
    ENTRY_GAME_VERSION,
    ENTRY_MAX
};


// ****************************************************************
// icon codes (UTF-8)
#define __ICON(x)            (r_cast<const coreChar*>(x))
#define ICON_SWITCH          (__ICON(u8"\u0030"))
#define ICON_MUSIC           (__ICON(u8"\uF001"))
#define ICON_CHECK           (__ICON(u8"\uF00C"))
#define ICON_TIMES           (__ICON(u8"\uF00D"))
#define ICON_GEAR            (__ICON(u8"\uF013"))
#define ICON_LOCK            (__ICON(u8"\uF023"))
#define ICON_PAUSE           (__ICON(u8"\uF04C"))
#define ICON_FORWARD         (__ICON(u8"\uF04E"))
#define ICON_ARROW_UP        (__ICON(u8"\uF062"))
#define ICON_SHARE           (__ICON(u8"\uF064"))
#define ICON_TROPHY          (__ICON(u8"\uF091"))
#define ICON_BOLT            (__ICON(u8"\uF0E7"))
#define ICON_FLOPPY_DISK     (__ICON(u8"\uF0C7"))
#define ICON_UNDO_ALT        (__ICON(u8"\uF2EA"))
#define ICON_TRASH_CAN       (__ICON(u8"\uF2ED"))
#define ICON_REDO_ALT        (__ICON(u8"\uF2F9"))
#define ICON_PEN             (__ICON(u8"\uF304"))
#define ICON_BURN            (__ICON(u8"\uF46A"))
#define ICON_FILE_ARROW_DOWN (__ICON(u8"\uF56D"))
#define ICON_FIRE_ALT        (__ICON(u8"\uF7E4"))


// ****************************************************************
// intro menu class
class cIntroMenu final : public coreMenu
{
private:
    cGuiLabel m_WelcomeText;                               // 

    coreTimer m_IntroTimer;                                // intro animation 
    coreUint8 m_iIntroStatus;                              // 

    coreFlow m_fDelayTimer;                                // 

    coreMap<coreString, cGuiButton*> m_apLanguageButton;   // list with buttons for valid language files
    cScrollBox m_LanguageBox;                              // 

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
    cGuiObject m_GameLogoKana;      // 
    cGuiObject m_Shadow;            // 

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

    cNewIndicator m_StartNew;
    cNewIndicator m_ScoreNew;
    cNewIndicator m_ReplayNew;
    cNewIndicator m_ExtraNew;
    cNewIndicator m_ConfigNew;

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
    cGuiObject m_BackgroundArmory;                                // 
    cGuiObject m_BackgroundFirst;                                 // 
    cGuiObject m_BackgroundDemo;                                  // 

    cGuiButton m_StartButtonArmory;                               // start button   
    cGuiButton m_StartButtonFirst;                                // start button   
    cGuiButton m_StartButtonDemo;                                 // start button   
    cGuiButton m_InputButtonArmory;                               // input button   
    cGuiButton m_InputButtonFirst;                                // input button   
    cGuiButton m_InputButtonDemo;                                 // input button   
    cGuiButton m_BackButtonMain;                                  // back button   
    cGuiButton m_BackButtonArmory;                                // back button   
    cGuiButton m_BackButtonFirst;                                 // back button   
    cGuiButton m_BackButtonDemo;                                  // back button   

    cGuiLabel  m_aSegmentName  [MENU_GAME_MISSIONS];              // 
    cGuiLabel  m_aSegmentNumber[MENU_GAME_MISSIONS];              // 
    cGuiObject m_aSegmentIcon  [MENU_GAME_MISSIONS];              // 
    cGuiObject m_aSegmentIconBack  [MENU_GAME_MISSIONS];              // 
    cGuiObject m_aSegmentLine  [MENU_GAME_MISSIONS];              // 

    cGuiObject m_aaSegmentTile[MENU_GAME_MISSIONS][MENU_GAME_SEGMENTS];   // 
    cGuiObject m_aaSegmentBack[MENU_GAME_MISSIONS][MENU_GAME_SEGMENTS];   // 
    cGuiObject m_aaSegmentArrow[MENU_GAME_MISSIONS][MENU_GAME_ARROWS];                // 
    cGuiObject m_aSegmentCursor[3];                               // 
    cGuiObject m_SegmentMedal;                                    // 
    cGuiObject m_aSegmentBadge    [MENU_GAME_ARMORY_BADGES];      // 
    cGuiObject m_aSegmentBadgeWave[MENU_GAME_ARMORY_BADGES];      // 
    coreUint8  m_aiSegmentSelection[2];                           // 
    coreUint8  m_iSegmentLast;                                    // 
    coreFlow   m_fSegmentTime;                                    // 
    cScrollBox m_SegmentBox;                                      // 

    cGuiLabel  m_aArcadeName[MENU_GAME_ARCADES];                  // 
    cGuiObject m_aArcadeBack[MENU_GAME_ARCADES];                  // 
    cGuiObject m_aArcadeLine[MENU_GAME_ARCADES];                  // 

    cGuiButton m_ArcadeComplete;                                  // 

    cGuiLabel  m_ArmoryHeader;                                    // 
    cGuiLabel  m_aArmoryName[MENU_GAME_ARMORIES];                 // 
    cGuiObject m_aArmoryLine[MENU_GAME_ARMORIES];                 // 

    cGuiObject m_ArmoryArea;                                      // 
    cGuiObject m_ArmoryArea2;                                     // 
    cGuiObject m_aArmoryMedal    [MENU_GAME_ARMORY_MEDALS];       // 
    cGuiObject m_aArmoryMedalBest[MENU_GAME_ARMORY_MEDALS];       // 
    cGuiObject m_ArmoryMedalBack;                                 // 
    cGuiObject m_aArmoryMedalBack2[MENU_GAME_ARMORY_MEDALS];      // 
    cGuiObject m_ArmoryFragment;                                  // 
    cGuiObject m_ArmoryFragmentBack;                              // 
    cGuiObject m_aArmoryBadge    [MENU_GAME_ARMORY_BADGES_ALL];   // 
    cGuiObject m_aArmoryBadgeWave[MENU_GAME_ARMORY_BADGES_ALL];   // 
    cGuiObject m_aArmoryBadgeBack[MENU_GAME_ARMORY_BADGES];       // 
    cGuiLabel  m_aArmoryBadgeDesc[MENU_GAME_ARMORY_BADGES];       // 
    cGuiLabel  m_aArmoryBadgeDesc2[MENU_GAME_ARMORY_BADGES][2];       // 
    cGuiObject m_aArmoryHelperBack;                               // 
    cGuiObject m_aArmoryHelper   [MENU_GAME_ARMORY_HELPERS];      // 
    cGuiObject m_aArmoryHelperWave[MENU_GAME_ARMORY_HELPERS];      //    
    cGuiObject m_aArmoryIcon     [MENU_GAME_ARMORY_ICONS];        // 
    cGuiObject m_aArmoryIconBack [MENU_GAME_ARMORY_ICONS];        // 
    cGuiObject m_aArmoryArrow    [MENU_GAME_ARMORY_ARROWS];       // 
    cGuiLabel  m_aArmoryTitle    [2];                             // 
    cGuiLabel  m_aArmoryScore    [2];                             // 
    cGuiLabel  m_aArmoryTime     [2];                             // 
    cGuiObject m_ArmoryTile;                                      // 
    cGuiObject m_ArmoryBack;                                      // 
    cGuiObject m_ArmoryIconBig;                                   // 
    cGuiObject m_ArmoryLogo;                                      // 
    cGuiLabel  m_ArmoryTrophy;                                    // 

    cGuiSwitchBox m_ArmorySelection;                              // 

    cGuiObject m_aArmoryStartArrow[2];                            // 
    cGuiLabel  m_ArmoryStartLabel;                                // 
    cGuiObject m_ArmoryStartBack;                                 // 
    coreUint8  m_iArmoryStartIndex;                               // 

    cGuiSwitchBox m_ArmoryType;                                   // 
    cGuiSwitchBox m_ArmoryMode;                                   // 
    cGuiSwitchBox m_ArmoryDifficulty;                             // 
    cGuiSwitchBox m_ArmorySpeed;                                  // 
    cGuiSwitchBox m_aArmoryShield    [MENU_GAME_PLAYERS];         // 
    cGuiSwitchBox m_aArmoryWeapon    [MENU_GAME_PLAYERS];         // 
    cGuiObject    m_aArmoryWeaponIcon[MENU_GAME_PLAYERS];         // 
    cGuiLabel     m_aArmoryPlayer    [MENU_GAME_PLAYERS];         // 
    cGuiLabel     m_aArmoryRaise     [3];                         // 
    cGuiLabel     m_ArmoryRaiseText;                              // 
    cGuiLabel     m_aArmoryCueLock[2];                            // 

    cGuiLabel  m_FirstHeader;                                     // 
    cGuiLabel  m_aFirstName[MENU_GAME_FIRSTS];                    // 
    cGuiObject m_aFirstLine[MENU_GAME_FIRSTS];                    // 

    cGuiSwitchBox m_FirstType;                                    // 
    cGuiSwitchBox m_FirstDifficulty;                              // 
    cGuiSwitchBox m_FirstSpeed;                                   // 
    cGuiSwitchBox m_aFirstShield[MENU_GAME_PLAYERS];              // 
    cGuiLabel     m_aFirstPlayer[MENU_GAME_PLAYERS];              // 
    cGuiLabel     m_aFirstRaise [3];                              // 
    cGuiLabel     m_FirstRaiseText;                               // 
    cGuiLabel     m_aFirstCueLock[2];                             // 

    cGuiLabel  m_DemoHeader;                                      // 
    cGuiLabel  m_aDemoName[MENU_GAME_DEMOS];                      // 
    cGuiObject m_aDemoLine[MENU_GAME_DEMOS];                      // 

    cGuiSwitchBox m_DemoType;                                     // 
    cGuiSwitchBox m_DemoDifficulty;                               // 
    cGuiSwitchBox m_DemoSpeed;                                    // 
    cGuiSwitchBox m_DemoStage;                                    // 
    cGuiObject    m_DemoStageIcon;                                // 
    cGuiSwitchBox m_aDemoShield[MENU_GAME_PLAYERS];               // 
    cGuiLabel     m_aDemoPlayer[MENU_GAME_PLAYERS];               // 
    cGuiLabel     m_aDemoRaise [3];                               // 
    cGuiLabel     m_DemoRaiseText;                                // 
    cGuiLabel     m_aDemoCueLock[2];                              // 

    coreUint8 m_iCurPage;                                         // 
    coreUintW m_aiCurIndex[4];                                    // 
    coreBool  m_bFromTrophy;                                      // 

    coreUint8 m_iBaseType;                                        // 
    coreUint8 m_iBaseMode;                                        // 
    coreUint8 m_iBaseDifficulty;                                  // 

    coreBool m_bFirstPlay;

    coreTexturePtr m_apFragment[FRAGMENTS];
    coreTexturePtr m_apSegment [2];

    cNewIndicator m_SpeedNew;
    cNewIndicator m_ShieldNew;
    cNewIndicator m_WeaponNew;

    cMenuNavigator m_NavigatorMain;
    cMenuNavigator m_NavigatorArmory;
    cMenuNavigator m_NavigatorFirst;
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
    void RetrievePageData (coreUintW* OUTPUT piMissionIndex, coreUintW* OUTPUT piSegmentIndex)const;

    // 
    void SelectPrevious();
    void SelectNext    ();
    void SelectTrophy  (const coreUintW iMissionIndex, const coreUintW iSegmentIndex);

    // 
    inline const coreUint8& GetSelectedType      ()const                       {return m_ArmoryType      .GetCurValue();}
    inline const coreUint8& GetSelectedMode      ()const                       {return m_ArmoryMode      .GetCurValue();}
    inline const coreUint8& GetSelectedDifficulty()const                       {return m_ArmoryDifficulty.GetCurValue();}
    inline const coreUint8& GetSelectedShield    (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return m_aArmoryShield[iIndex].GetCurValue();}
    inline const coreUint8& GetSelectedWeapon    (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return m_aArmoryWeapon[iIndex].GetCurValue();}
    inline       coreUint8  GetSelectedSupport   (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return 0u;}

    // 
    inline void ResetNavigator() {/*m_NavigatorMain.ResetFirst();*/ m_NavigatorArmory.ResetFirst(); m_NavigatorFirst.ResetFirst(); m_NavigatorDemo.ResetFirst();}


private:
    // 
    void __LoadUnlocks();

    // 
    void __RefreshBase();
    void __RefreshCursor(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);

    // 
    void __SelectMission(const coreUintW iMissionIndex);
    void __SelectSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);

    // 
    void __PrepareArcade ();
    void __PrepareMission(const coreUintW iMissionIndex);
    void __PrepareSegment(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);

    // 
    coreBool __SetupInput(const coreBool bAlways);
};


// ****************************************************************
// score menu class
class cScoreMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                        // 

    cGuiLabel m_Header;                             // 

    cGuiButton m_BackButton;                        // back button

    cGuiObject m_aFilterLine[MENU_SCORE_FILTERS];   // 
    cGuiObject m_FilterIcon;                        // 

    cGuiSwitchBox m_FilterMission;                  // 
    cGuiSwitchBox m_FilterSegment;                  // 
    cGuiSwitchBox m_FilterPure;                     // 
    cGuiSwitchBox m_FilterDifficulty;               // 

    cGuiLabel  m_aRank [MENU_SCORE_ENTRIES];        // 
    cGuiLabel  m_aName [MENU_SCORE_ENTRIES];        // 
    cGuiLabel  m_aTime [MENU_SCORE_ENTRIES];        // 
    cGuiLabel  m_aScore[MENU_SCORE_ENTRIES];        // 
    cGuiLabel  m_aFile [MENU_SCORE_ENTRIES];        // 
    cGuiObject m_aIcon [MENU_SCORE_ENTRIES];        // 
    cGuiObject m_aMedal[MENU_SCORE_ENTRIES];        // 
    cGuiObject m_aLine [MENU_SCORE_ENTRIES];        // 
    cScrollBox m_ScoreBox;                          // 

    cGuiLabel  m_PlayerRank;                        // 
    cGuiLabel  m_PlayerName;                        // 
    cGuiLabel  m_PlayerTime;                        // 
    cGuiLabel  m_PlayerScore;                       // 
    cGuiObject m_PlayerIcon;                        // 
    cGuiObject m_PlayerMedal;                       // 
    cGuiObject m_PlayerLine;                        // 

    cGuiSwitchBox m_PageSelection;                  // 
    cGuiLabel     m_PageText;                       // 

    cGuiLabel m_Loading;                            // 
    cGuiLabel m_LoadingPlayer;                      // 
    cGuiLabel m_Nothing;                            // 

    coreMap<coreUint8, coreUint8> m_aiCurFilter;
    coreUint8 m_iCurIndex;

    coreUint32 m_iPageOffset;                       // 
    coreUint32 m_iPageMax;                          // 
    coreBool   m_bPageChanged;                      // 
    coreUint8  m_iWorkUpload;                       // 
    coreUint8  m_iWorkDownload1;                    // 
    coreUint8  m_iWorkDownload2;                    // 
    coreUint8  m_iCurRequest;                       // 

    corePlatformFileHandle m_aiFileHandle[MENU_SCORE_ENTRIES];   // 

    coreBool m_bLimitRequest;                       // 
    coreFlow m_fLimitCooldown;                      // 

    cMenuNavigator m_Navigator;


public:
    cScoreMenu()noexcept;

    DISABLE_COPY(cScoreMenu)

    // move the score menu
    void Move()final;

    // 
    void LoadMissions();
    void LoadSegments(const coreUintW iMissionIndex);

    // 
    inline void ResetNavigator() {m_Navigator.ResetFirst();}


private:
    // 
    void __UpdateScores(const coreBool bPlayer);
    void __ResetScores();

    // 
    static void __SetWeaponIcon     (cGuiObject* OUTPUT pObject, const coreUint8 iWeapon);
    static void __SetMedalIcon      (cGuiObject* OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType);
    static void __SetMedalIconArcade(cGuiObject* OUTPUT pObject, const coreUint8 (&aaiMedal)[SCORE_MISSIONS / 2u][SCORE_SEGMENTS]);
};


// ****************************************************************
// replay menu class
class cReplayMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                    // 

    cGuiLabel m_Header;                         // 

    cGuiButton m_StartButton;                   // 
    cGuiButton m_DeleteButton;                  // 
    cGuiButton m_RenameButton;                  // 
    cGuiButton m_SaveButton;                    // 
    cGuiButton m_BackButtonOverview;            // 
    cGuiButton m_BackButtonDetails;             // 

    cGuiSwitchBox m_PageSelection;              // 
    cGuiLabel     m_PageText;                   // 

    cGuiLabel  m_aName [MENU_REPLAY_ENTRIES];   // 
    cGuiLabel  m_aTime [MENU_REPLAY_ENTRIES];   // 
    cGuiLabel  m_aScore[MENU_REPLAY_ENTRIES];   // 
    cGuiObject m_aLine [MENU_REPLAY_ENTRIES];   // 
    cScrollBox m_ReplayBox;                     // 

    cGuiLabel m_Nothing;                        // 

    cGuiSwitchBox m_DetailSelection;                              // 

    cGuiLabel  m_aDetailName[MENU_REPLAY_DETAILS];                // 
    cGuiObject m_aDetailLine[MENU_REPLAY_DETAILS];                // 

    cGuiObject m_DetailArea;                                      // 
    cGuiObject m_DetailArea2;                                     // 
    cGuiObject m_aDetailMedal    [MENU_REPLAY_DETAIL_MEDALS];     // 
    cGuiObject m_aDetailMedalBack[MENU_REPLAY_DETAIL_MEDALS];     // 
    cGuiObject m_aDetailIcon     [MENU_REPLAY_DETAIL_ICONS];      // 
    cGuiObject m_aDetailIconBack [MENU_REPLAY_DETAIL_ICONS];      // 
    cGuiObject m_aDetailArrow    [MENU_REPLAY_DETAIL_ARROWS];     // 
    cGuiLabel  m_DetailTitle;                                     // 
    cGuiLabel  m_DetailTitleSmall;                                // 
    cGuiLabel  m_DetailTitleSmall2;                               // 
    cGuiLabel  m_aDetailScore[2];                                 // 
    cGuiLabel  m_aDetailTime [2];                                 // 

    cGuiSwitchBox m_DetailType;                                   // 
    cGuiSwitchBox m_DetailDifficulty;                             // 
    cGuiSwitchBox m_DetailSpeed;                                  // 
    cGuiSwitchBox m_aDetailShield    [MENU_REPLAY_PLAYERS];       // 
    cGuiSwitchBox m_aDetailWeapon    [MENU_REPLAY_PLAYERS];       // 
    cGuiObject    m_aDetailWeaponIcon[MENU_REPLAY_PLAYERS];       // 
    cGuiLabel     m_aDetailData[MENU_REPLAY_DETAIL_DATAS];        // 
    cGuiLabel     m_aaDetailRun[MENU_REPLAY_MISSIONS][MENU_REPLAY_DETAIL_RUNS];

    cGuiObject m_aDetailStartArrow[2];                            // 
    cGuiLabel  m_DetailStartLabel;                                // 
    cGuiObject m_DetailStartBack;                                 // 
    coreUint8  m_iDetailStartIndex;                               // 

    cScrollBox m_DetailBox;                                       // 

    cGuiLabel m_RenameHeader;                  // 

    coreList<cReplay::sInfo> m_aInfoList;      // 
    coreUintW m_iSelected;                     // 
    coreUintW m_iOverwrite;                    // 
    coreBool  m_bDelete;                       // 
    coreBool  m_bRename;                       // 
    coreBool  m_bSave;                         // 
    coreBool  m_bReturn;                       // 
    coreBool  m_bDownloaded;                   // 

    coreUint32 m_iPageOffset;                  // 
    coreUint32 m_iPageMax;                     // 
    coreBool   m_bPageChanged;                 // 

    cMenuNavigator m_NavigatorOverview;
    cMenuNavigator m_NavigatorDetails;


public:
    cReplayMenu()noexcept;

    DISABLE_COPY(cReplayMenu)

    // move the replay menu
    void Move()final;

    // 
    void LoadOverview();
    void LoadDetails(const coreUintW iIndex);
    void LoadDownload();

    // 
    void SelectPrevious();
    void SelectNext    ();

    // 
    inline void ResetNavigator() {m_NavigatorOverview.AssignFirst(m_aLine[0].IsEnabled(CORE_OBJECT_ENABLE_ALL) ? s_cast<coreObject2D*>(&m_aLine[0]) : &m_PageSelection); m_NavigatorOverview.ResetFirst(); m_NavigatorDetails.ResetFirst();}

    // 
    static void HandleSaveError(const std::function<void(coreInt32)> nCallback);


private:
    // 
    void __UpdateFiles();
    void __UpdateList(const coreBool bReset);
    void __UpdateSelection();

    // 
    void __UpdateDownloaded(const coreBool bDownloaded);

    // 
    void __MarkSaved();

    // 
    static coreUint32 __GetTotalScore(const cReplay::sHeader& oHeader);

    // 
    static const coreChar* __PrintWithName(const coreHashString sLanguageKey, const coreChar* pcName);
};


// ****************************************************************
// extra menu class
class cExtraMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                          // 

    cGuiButton m_TrophyTab;                           // 
    cGuiButton m_StatsTab;                            // 
    cGuiButton m_OtherTab;                            // 

    cGuiButton m_BackButton;                          // back button

    cGuiLabel  m_SummaryName;                         // 
    cGuiLabel  m_SummaryValue;                        // 
    cGuiObject m_SummaryLine;                         // 

    cGuiLabel  m_CueTrophy;                           // 

    cGuiLabel  m_aTrophyName [MENU_EXTRA_TROPHIES];   // 
    cGuiLabel  m_aaTrophyDesc[MENU_EXTRA_TROPHIES][2];   // 
    cGuiLabel  m_aTrophyCheck[MENU_EXTRA_TROPHIES];   // 
    cGuiButton m_aTrophyArrow[MENU_EXTRA_TROPHIES];   // 
    cGuiObject m_aTrophyTile [MENU_EXTRA_TROPHIES];   // 
    cGuiObject m_aTrophyBack [MENU_EXTRA_TROPHIES];   // 
    cGuiObject m_aTrophyLine [MENU_EXTRA_TROPHIES];   // 
    cScrollBox m_TrophyBox;                           // 

    cGuiObject m_aFilterLine[MENU_EXTRA_FILTERS];     // 
    cGuiObject m_FilterIcon;                          // 

    cGuiSwitchBox m_FilterMission;                    // 
    cGuiSwitchBox m_FilterSegment;                    // 
    cGuiSwitchBox m_FilterType;                       // 
    cGuiSwitchBox m_FilterDifficulty;                 // 

    cGuiLabel  m_aStatsName [MENU_EXTRA_STATS];       // 
    cGuiLabel  m_aStatsValue[MENU_EXTRA_STATS];       // 
    cGuiObject m_aStatsLine [MENU_EXTRA_STATS];       // 
    cScrollBox m_StatsBox;                            // 

    cGuiLabel  m_aOtherName[MENU_EXTRA_OTHERS];       // 
    cGuiObject m_aOtherLine[MENU_EXTRA_OTHERS];       // 

    cGuiButton m_Password;                            // 
    cGuiButton m_Credits;                             // 
    cGuiButton m_Music;                               // 

    cGuiLabel m_PasswordHeader;                       // 

    coreMap<coreUint8, coreUint8> m_aiCurFilter;
    coreUint8 m_iCurIndex;

    cSave::sLocalStats m_Stats;

    coreUintW m_iTrophyMission;
    coreUintW m_iTrophySegment;

    cNewIndicator m_OtherNew;
    cNewIndicator m_MusicBoxNew;

    cMenuNavigator m_Navigator;


public:
    cExtraMenu()noexcept;

    DISABLE_COPY(cExtraMenu)

    // move the extra menu
    void Move()final;

    // 
    void LoadTrophies();
    void LoadMissions();
    void LoadSegments(const coreUintW iMissionIndex);

    // 
    void SetSelectedTrophy(const coreUintW   iMissionIndex,  const coreUintW   iSegmentIndex);
    void GetSelectedTrophy(coreUintW* OUTPUT piMissionIndex, coreUintW* OUTPUT piSegmentIndex)const;

    // 
    inline void ResetNavigator() {m_Navigator.ResetFirst();}


private:
    // 
    void __UpdateStats();

    // 
    const cSave::sLocalStats& __PrepareStats(const coreUint8 iTypeValue, const coreUint8 iModeValue, const coreUint8 iDifficultyValue, const coreUint8 iMissionValue, const coreUint8 iSegmentValue);
};


// ****************************************************************
// config menu class
class cConfigMenu final : public coreMenu
{
private:
    // 
    struct sPlayerInput final
    {
        cGuiLabel     oHeader;                            // 
        cGuiSwitchBox oType;                              // 
        cGuiSwitchBox oRumble;                            // 
        cGuiSwitchBox oFireMode;                          // 
        cGuiSwitchBox oControlMode;                       // 
        cGuiButton    oMoveUp;                            // 
        cGuiButton    oMoveLeft;                          // 
        cGuiButton    oMoveDown;                          // 
        cGuiButton    oMoveRight;                         // 
        cGuiButton    aAction[INPUT_KEYS_ACTION];         // 
        cFigure       oFigureMoveUp;                      // 
        cFigure       oFigureMoveLeft;                    // 
        cFigure       oFigureMoveDown;                    // 
        cFigure       oFigureMoveRight;                   // 
        cFigure       aFigureAction[INPUT_KEYS_ACTION];   // 
        cFigure       oFigureStickL;                      // 
        cFigure       oFigureStickR;                      // 
    };


private:
    cGuiObject m_Background;                             // 

    cGuiButton m_VideoTab;                               // 
    cGuiButton m_AudioTab;                               // 
    cGuiButton m_InputTab;                               // 
    cGuiButton m_GameTab;                                // 

    cGuiButton m_SaveButton;                             // save button
    cGuiButton m_DiscardButton;                          // discard button
    cGuiButton m_InputButton;                            // input button
    cGuiButton m_BackButton;                             // back button

    cGuiLabel  m_aLabel[ENTRY_MAX];                      // 
    cGuiObject m_aLine [ENTRY_MAX];                      // 

    cGuiLabel       m_Description;                       // 
    const coreChar* m_apcDescKey[ENTRY_MAX];             // 

    cGuiSwitchBox m_Monitor;                             // 
    cGuiSwitchBox m_Resolution;                          // 
    cGuiSwitchBox m_DisplayMode;                         // 
    cGuiSwitchBox m_Vsync;                               // 
    cGuiSwitchBox m_AntiAliasing;                        // 
    cGuiSwitchBox m_TextureFilter;                       // 
    cGuiSwitchBox m_RenderQuality;                       // 
    cGuiSwitchBox m_ShadowQuality;                       // 
    cGuiSwitchBox m_WaterQuality;                        // 
    cGuiSwitchBox m_ParticleEffects;                     // 
    cGuiSwitchBox m_ShakeEffects;                        // 
    cGuiSwitchBox m_FlashEffects;                        // 
    cGuiSwitchBox m_HitStopEffects;                      // 
    cGuiSwitchBox m_ChromaEffects;                       // 
    cGuiSwitchBox m_GlobalVolume;                        // 
    cGuiSwitchBox m_MusicVolume;                         // 
    cGuiSwitchBox m_EffectVolume;                        // 
    cGuiSwitchBox m_AmbientVolume;                       // 
    cGuiSwitchBox m_MenuVolume;                          // 
    cGuiSwitchBox m_AudioQuality;                        // 
    cGuiSwitchBox m_AudioMode;                           // 
    cGuiSwitchBox m_3DSound;                             // 
    cGuiSwitchBox m_Language;                            // 
    cGuiSwitchBox m_GameDirection;                       // 
    cGuiSwitchBox m_MirrorMode;                          // 
    cGuiSwitchBox m_HudDirection;                        // 
    cGuiSwitchBox m_HudType;                             // 
    cGuiSwitchBox m_CombatText;                          // 
    cGuiSwitchBox m_BackRotation;                        // 
    cGuiSwitchBox m_BackSpeed;                           // 
    cGuiSwitchBox m_UpdateFreq;                          // 
    cGuiSwitchBox m_PureMode;                            // 
    cGuiSwitchBox m_Leaderboard;                         // 
    cGuiSwitchBox m_Version;                             // 

    sPlayerInput m_aInput[MENU_CONFIG_INPUTS];           // 
    cGuiButton   m_SwapInput;                            //

    cGuiLabel m_aCueInput[INPUT_KEYS];                   // 
    cGuiLabel m_aCueRota [2];                            // 
    cGuiLabel m_aCueLock [5];                            // 

    cScrollBox m_VideoBox;                               // 
    cScrollBox m_InputBox;                               // 

    coreUintW                      m_iCurMonitorIndex;   // 
    coreMap<coreUintW, coreString> m_asCurResolution;    // 

    coreUintW m_iJoystickNum;                            // 

    cNewIndicator m_GameNew;                             // 
    cNewIndicator m_MirrorModeNew;                       // 

    cMenuNavigator m_Navigator;                          // 


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
    void ResetNavigator();

    // convert input key to readable string                 
    static void PrintFigure(cFigure* OUTPUT pFigure, const coreUint8 iType, const coreInt16 iKey);


private:
    // 
    void __UpdateRenderQuality();
    void __UpdateShadowQuality();
    void __UpdateWaterQuality();
    void __UpdateVolume();
    void __UpdateLanguage();
    void __UpdateBackSpeed();
    void __UpdatePureModeBase();
    void __UpdatePureMode();
    void __UpdateInterface();

    // 
    void __LoadMonitors();
    void __LoadResolutions(const coreUintW iMonitorIndex);
    void __LoadFrequencies(const coreUintW iMonitorIndex);
    void __LoadInputs();
    void __LoadUnlocks();

    // 
    void __RefreshManual();
    void __RefreshTabs();

    // 
    coreBool __SetupInput();

    // 
    inline cGuiButton& __RetrieveInputButton     (const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < MENU_CONFIG_INPUTS) && (iKeyIndex < INPUT_KEYS)) return *((&m_aInput[iPlayerIndex].oMoveUp)                                         + iKeyIndex);}
    inline cFigure&    __RetrieveInputFigure     (const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < MENU_CONFIG_INPUTS) && (iKeyIndex < INPUT_KEYS)) return *((&m_aInput[iPlayerIndex].oFigureMoveUp)                                   + iKeyIndex);}
    inline coreInt16&  __RetrieveInputCurValue   (const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < INPUT_TYPES)        && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[g_CurConfig.Input.aiType[iPlayerIndex]].iMoveUp) + iKeyIndex);}
    inline coreInt16&  __RetrieveInputDirCurValue(const coreUintW iType,        const coreUintW iKeyIndex) {ASSERT((iType        < INPUT_SETS)         && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[iType].iMoveUp)                                  + iKeyIndex);}
    inline coreInt16&  __RetrieveInputDirOldValue(const coreUintW iType,        const coreUintW iKeyIndex) {ASSERT((iType        < INPUT_SETS)         && (iKeyIndex < INPUT_KEYS)) return *((&g_OldConfig.Input.aSet[iType].iMoveUp)                                  + iKeyIndex);}

    // 
    static inline coreFloat __VolumeToFloat(const coreUint8 iVolume) {return I_TO_F(iVolume) * 0.01f;}
    static inline coreUint8 __VolumeToUint8(const coreFloat fVolume) {return F_TO_UI(ROUND(CLAMP01(fVolume) * 20.0f) * 5.0f);}
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

    cGuiButton m_NextButton;                                        // next button
    cGuiButton m_AgainButton;                                       // again button
    cGuiButton m_ExitButton;                                        // exit button

    cGuiLabel m_aTitle[2];                                          // 

    coreFullscreen m_ArcadeLayer;                                   // 
    cGuiLabel      m_ArcadeHeader;                                  // 

    cGuiLabel  m_aArcadeName         [MENU_SUMMARY_ARCADES];              // 
    cGuiLabel  m_aaArcadeScoreRun    [MENU_SUMMARY_ARCADES][MENU_SUMMARY_RUNS];   // 
    cGuiLabel  m_aArcadeScore        [MENU_SUMMARY_ARCADES];   // 
    cGuiLabel  m_aArcadeTime         [MENU_SUMMARY_ARCADES];              // 
    cGuiObject m_aArcadeIcon         [MENU_SUMMARY_ARCADES];              // 
    cGuiObject m_aArcadeIconBack     [MENU_SUMMARY_ARCADES];              // 
    cGuiObject m_aArcadeMedalMission [MENU_SUMMARY_ARCADES];
    cGuiObject m_aaArcadeMedalSegment[MENU_SUMMARY_ARCADES][MENU_SUMMARY_MEDALS];
    cGuiObject m_aArcadeLine         [MENU_SUMMARY_ARCADES];              // 

    cGuiLabel  m_aArcadeSum[MENU_SUMMARY_RUNS];                     // 
    cGuiLabel  m_aArcadeTotalName[2];
    cGuiLabel  m_aArcadeTotalBest[2];
    cGuiLabel  m_ArcadeTotalScore;
    cGuiLabel  m_ArcadeTotalTime;
    cGuiLabel  m_ArcadeOptions;
    cGuiObject m_ArcadeTotalMedal;

    cGuiObject m_aContinueImage[MENU_SUMMARY_RUNS];                 // 

    cGuiLabel  m_aHeader[2];                                        // 
    cGuiObject m_Icon;                                              // 

    cGuiObject m_MedalMission;                                      // 
    cGuiObject m_aMedalSegment[MENU_SUMMARY_MEDALS];                // 

    cGuiLabel m_aName [MENU_SUMMARY_ENTRIES];                       // 
    cGuiLabel m_aValue[MENU_SUMMARY_ENTRIES];                       // 
    cGuiLabel m_aaPart[MENU_SUMMARY_ENTRIES][MENU_SUMMARY_PARTS];   // 

    cGuiLabel m_TotalName;                                          // 
    cGuiLabel m_TotalValue;                                         // 
    cGuiLabel m_aTotalPart[MENU_SUMMARY_PARTS];                     // 
    cGuiLabel m_TotalBest;                                          // 

    cGuiLabel m_Raise;

    coreUint32 m_iFinalValue;                                       // (just for display) 
    coreUint32 m_aiFinalPart [MENU_SUMMARY_PARTS];                  // (just for display) 
    coreUint32 m_aiApplyBonus[MENU_SUMMARY_PARTS];                  // 
    coreUint8  m_aiApplyMedal[MENU_SUMMARY_PARTS];                  // 

    coreFloat m_fOtherTime;
    coreInt32 m_iOtherShift;
    coreUint8 m_iOtherNumMission;
    coreUint8 m_iOtherNumMedal;
    coreUint8 m_iOtherStart;


#define MENU_SUMMARY_ENTRIES_SEGMENT (2u)
#define MENU_SUMMARY_SIDES (2u)
#define MENU_SUMMARY_BADGES (BADGES - 4u)

    cGuiLabel m_aSegmentName [MENU_SUMMARY_ENTRIES_SEGMENT];                       // 
    cGuiLabel m_aaSegmentValue[MENU_SUMMARY_ENTRIES_SEGMENT][MENU_SUMMARY_SIDES];                       // 
    cGuiLabel m_aaSegmentPart[MENU_SUMMARY_ENTRIES_SEGMENT][MENU_SUMMARY_PARTS];   // 
    
    cGuiLabel m_aSegmentShift[MENU_SUMMARY_SIDES];                       // 
    
    cGuiObject m_SegmentMedal;
    
    cGuiObject m_aSegmentBadge    [MENU_SUMMARY_BADGES];
    cGuiObject m_aSegmentBadgeWave[MENU_SUMMARY_BADGES];
    
    cGuiLabel m_SegmentRaise;
    
    coreUint8 m_iSelection;
    
    
    cGuiLabel m_aPerfect[2];
    cGuiLabel m_aRecord[6];   // TODO 1: one too much (from max series)
    coreFlow  m_afSignalTime[2];
    coreUint8 m_iSignalActive;
    

    coreFlow  m_fIntroTimer;                                        // 
    coreFlow  m_fOutroTimer;                                        // 
    coreFloat m_fTimeShift;                                         // 

    coreFloat m_fFinalSpinOld;                                      // 

    eSummaryState m_eState;                                         // 
    coreBool m_bHasReplay;

    cMenuNavigator m_Navigator;


public:
    cSummaryMenu()noexcept;

    DISABLE_COPY(cSummaryMenu)

    // render and move the summary menu
    void Render()final;
    void Move  ()final;

    // 
    void ShowArcade();
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
    void __ApplyDeferred();

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
    cGuiObject m_Background;                              // 

    cGuiLabel  m_GameOverText;                            // 
    cGuiLabel  m_ContinueText;                            // 
    cGuiLabel  m_ContinueTimer;                           // 
    cGuiObject m_aContinueImage[MENU_DEFEAT_CONTINUES];   // 

    coreFlow m_fCountdown;                                // 
    coreFlow m_fBurst;                                    // 

    coreFlow m_fIntroTimer;                               // 
    coreFlow m_fOutroTimer;                               // 

    eDefeatState m_eState;                                // 


public:
    cDefeatMenu()noexcept;

    DISABLE_COPY(cDefeatMenu)

    // render and move the defeat menu
    void Render()final;
    void Move  ()final;

    // 
    void ShowContinue();
    void ShowGameOver();
    void ShowReplay();


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
    cGuiLabel m_InputHeader;       // 

    cGuiLabel  m_UnlockHeader;     // 
    cGuiLabel  m_UnlockName;       // 
    cGuiLabel  m_aUnlockDesc[2];   // 
    cGuiObject m_UnlockIcon;       // 
    cGuiObject m_UnlockBack;       // 

    coreFlow m_fReturnTimer;       // 
    coreBool m_bReturnState;       // 

    coreUint8 m_iTarget;           // 
    coreBool  m_bPaused;           // 
    coreBool  m_bFade;             // 

    coreUintW m_iUnlockIndex;      // 
    coreFlow  m_fUnlockTime;       // 
    coreUint8 m_iUnlockState;      // 


public:
    cBridgeMenu()noexcept;

    DISABLE_COPY(cBridgeMenu)

    // move the bridge menu
    void Move()final;

    // 
    void EnterGame  (const coreBool bFade);
    void ReturnMenu (const coreUint8 iTarget, const coreBool bPaused, const coreBool bFade);
    void UseContinue();
    void UseNext    ();
    void UseRestart (const coreBool bPaused);

    // 
    void     ShowUnlocks(const coreFloat fTimeOffset = 0.0f);
    coreBool HasUnlocks();
    void     ApplyUnlocks();

    // 
    void SetHighlightColor(const coreVector3 vColor);

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
    cMusicBox    m_MusicBox;             // 

    coreFullscreen m_PauseLayer;         // 
    coreUint32     m_iPauseFrame;        // 

    cGuiLabel m_NoticeSave;              // 
    coreFlow  m_fNoticeSaveTime;         // 
    coreBool  m_bNoticeSavePrevent;      // 

    coreFrameBuffer m_aFrameBuffer[3];   // 
    coreObject2D    m_MixObject;         // 
    
    coreUint8 iForceA;
    coreUint8 iForceB;

    coreTimer m_TransitionTime;          // 
    coreUint8 m_iTransitionState;        // 
    coreMenu* m_pTransitionMenu;         // 
    
    coreFlow m_fVolume;
    
    
    cInterface m_Interface;
    coreFlow   m_fInterfaceAlpha;

    static coreVector3 s_vHighlightColor;       // 
    static coreVector3 s_vButtonColor;       // 
    
    static coreMap<const void*, coreButton*> s_apCurButton;
    static coreMap<const void*, coreButton*> s_apNewButton;
    static cGuiButton* s_pCurTab;
    static cGuiButton* s_pNewTab;
    static cGuiObject* s_pCurLine;
    static cGuiObject* s_pNewLine;
    
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
    inline cMusicBox*    GetMusicBox   () {return &m_MusicBox;}

    // 
    coreBool IsPaused()const;
    coreBool IsPausedWithStep();
    inline void InvokePauseStep() {m_iPauseFrame = Core::System->GetCurFrame();}

    // 
    coreBool NeedsCursor()const;

    // 
    void ShiftSurface(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface, const coreFloat fSpeed, const coreUint8 iSound);
    inline coreBool IsShifting()const {return m_TransitionTime.GetStatus();}
    inline const coreTimer& GetShifting()const {return m_TransitionTime;}

    // 
    void SetHighlightColor(const coreVector3 vColor);
    void SetButtonColor(const coreVector3 vColor);
   
    
    inline const coreVector3& GetHighlightColor()const {return s_vHighlightColor;}
    inline const coreVector3& GetButtonColor()const {return s_vButtonColor;}
    
    inline cInterface* GetInterface() {return &m_Interface;}
    
    inline void PreventSaveText() {m_bNoticeSavePrevent = true;}
    
    // 
    coreFloat GetVolume()const;

    // 
    static void UpdateLanguageFont();
    static const coreMap<coreString, coreString>& GetLanguageList();
    
    static void ClearScreen();
    
    static const coreChar* GetMissionLetters(const coreUintW iMissionIndex);
    static const coreChar* GetSegmentLetters(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
    
    static const coreChar* GetStoreText();
    static void            OpenStoreLink();

    // menu helper routines
    static void UpdateButton        (coreButton*    OUTPUT pButton, const void* pMenu, const coreBool bFocused, const coreVector3 vFocusColor,   const eMenuUpdate eUpdate = MENU_UPDATE_DEFAULT);
    static void UpdateButton        (coreButton*    OUTPUT pButton, const void* pMenu, const coreBool bFocused,                                  const eMenuUpdate eUpdate = MENU_UPDATE_DEFAULT);
    static void UpdateTab           (cGuiButton*    OUTPUT pTab, const coreBool bLocked, const coreBool bFocused, const coreVector3 vFocusColor, const eMenuUpdate eUpdate = MENU_UPDATE_DEFAULT);
    static void UpdateTab           (cGuiButton*    OUTPUT pTab, const coreBool bLocked, const coreBool bFocused,                                const eMenuUpdate eUpdate = MENU_UPDATE_DEFAULT);
    static void UpdateSwitchBox     (cGuiSwitchBox* OUTPUT pSwitchBox,                                                                           const eMenuUpdate eUpdate = MENU_UPDATE_DEFAULT);
    static void UpdateLine          (cGuiObject*    OUTPUT pLine, const coreBool bInteract, const coreVector3 vFocusColor,                       const eMenuUpdate eUpdate = MENU_UPDATE_DEFAULT);
    static void UpdateLine          (cGuiObject*    OUTPUT pLine, const coreBool bInteract,                                                      const eMenuUpdate eUpdate = MENU_UPDATE_DEFAULT);
    static void UpdateAnimateProgram(cGuiObject*    OUTPUT pObject);
    static void ApplyMedalTexture   (cGuiObject*    OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType, const coreBool bHide);
    static void ChangeTab           (coreMenu*      OUTPUT pMenu, const coreUint8 iNewSurface);
    
    static void ClearButtonTime(cGuiButton* OUTPUT pButton);


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __StartGame();
    void __EndGame();
};


#endif // _P1_GUARD_MENU_H_