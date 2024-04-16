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

// TODO 1: menu control with keyboard+joystick, cursor gets invisible on these inputs
// TODO 1: short YES-no questions: Exit Game ? Return to Menu ?
// TODO 5: move mouse to buttons on joystick-input
// TODO 1: real-time sound-configuration (also quick left-right sound effect on 3d-sound option)
// TODO 1: score-menu names are restricted to characters from the text-board
// TODO 3: score-menu names must be sanitized
// TODO 3: unload fonts currently not used (e.g. from score-menu)
// TODO 1: options menu: ask if values should be discarded, ask if want to exit instead of saving
// TODO 3: options menu: 15 second on video change, yes, no
// TODO 3: rumble when changing rumble-option (could be annoying)
// TODO 5: display unattached joysticks and joystick names somehow
// TODO 3: highlight which joystick is which input set
// TODO 3: summary: add separate total-score for each player
// TODO 3: update texture filter and render quality in realtime (resource-manager reset ?)
// TODO 3: double initial languages by switching to two columns (on demand?)
// TODO 5: stages in GameMenu should be called segments
// TODO 2: upper white border in settings menu "moves" during transition on 1760x990, check all menus for moving during transition in common resolutions
// TODO 3: every object in menu needs outline: weapon icons
// TODO 3: show target FPS in config menu (speed x update rate)
// TODO 3: when switching resolution, and confirming by returning, the transition is broken
// TODO 3: when switching resolution, the next mouse click is not recognized (no press event is coming from SDL, only the release event)
// TODO 3: swapping controls should swap buttons visually
// TODO 3: context specific controller button icons on top of J0 J1 etc.
// TODO 1: show correct button icons in input config menu
// TODO 3: in config menu, switchboxes flicker when discarding config
// TODO 3: load replay number only on first entry, load headers async on demand, handle changes during runtime
// TODO 3: display gamepad/device name in options description
// TODO 4: locked config input buttons are set with dummy values before getting locked
// TODO 2: handle plugging in and out gamepad while in config menu, handle plugging out gamepad in general (when controller is currently selected)
// TODO 2: in emscripten müssen einige optionen geändert werden (disable: monitor, resolution, display mode | change: anti-aliasing [on/off])


// ****************************************************************
// menu definitions
#define MENU_LIGHT_ACTIVE             (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE               (0.65f)    // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE           (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK           (0.04f)    // black contrast value
#define MENU_CONTRAST_INSIDE          (0.94f)    // 
#define MENU_INSIDE_ALPHA             (0.95f)    // 

#define MENU_GAME_MISSIONS            (9u)
#define MENU_GAME_STAGES              (7u)//SEGMENTS)
#define MENU_GAME_PLAYERS             (PLAYERS)
#define MENU_GAME_OPTIONS             (3u)
#define MENU_GAME_EQUIPS              (2u)
#define MENU_SCORE_ENTRIES            (20u)
#define MENU_REPLAY_ENTRIES           (12u)
#define MENU_EXTRA_FILTERS            (2u)
#define MENU_EXTRA_STATS              (20u)
#define MENU_EXTRA_OTHERS             (3u)
#define MENU_CONFIG_INPUTS            (PLAYERS)
#define MENU_SUMMARY_MEDALS           (SEGMENTS)
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

    SURFACE_GAME_STANDARD = 0u,
    SURFACE_GAME_TRAINING,
    SURFACE_GAME_OPTIONS,
    SURFACE_GAME_ARMORY,
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

    SURFACE_SUMMARY_SOLO = 0u,
    SURFACE_SUMMARY_COOP,
    SURFACE_SUMMARY_BEGIN,
    SURFACE_SUMMARY_TITLE,
    SURFACE_SUMMARY_MAX,

    SURFACE_DEFEAT_CONTINUE = 0u,
    SURFACE_DEFEAT_GAMEOVER,
    SURFACE_DEFEAT_MAX,

    SURFACE_FINISH_DEFAULT = 0u,
    SURFACE_FINISH_MAX,

    SURFACE_BRIDGE_ENTER = 0u,
    SURFACE_BRIDGE_RETURN1,
    SURFACE_BRIDGE_RETURN2,
    SURFACE_BRIDGE_INPUT,
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
    ENTRY_VIDEO_ANTIALIASING,
    ENTRY_VIDEO_TEXTUREFILTER,
    ENTRY_VIDEO_RENDERQUALITY,
    ENTRY_VIDEO_SHADOWQUALITY,
    ENTRY_VIDEO_SHAKEEFFECTS,
    ENTRY_VIDEO_FLASHEFFECTS,
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
    cGuiButton m_StartButton;    // start button
    cGuiButton m_ScoreButton;    // score button
    cGuiButton m_ReplayButton;   // replay button
    cGuiButton m_ExtraButton;    // extra button
    cGuiButton m_ConfigButton;   // config button
    cGuiButton m_ExitButton;     // exit button

    cMenuNavigator m_Navigator;


public:
    cMainMenu()noexcept;

    DISABLE_COPY(cMainMenu)

    // move the main menu
    void Move()final;

    // 
    void ActivateFirstPlay();
    void DeactivateFirstPlay();
};


// ****************************************************************
// game menu class
class cGameMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                                      // 

    cGuiButton m_StandardTab;                                     // 
    cGuiButton m_TrainingTab;                                     // 

    cGuiButton m_StartButton;                                     // start button
    cGuiButton m_OptionButton;                                    // 
    cGuiButton m_BackButton;                                      // back button

    cGuiLabel m_OptionState;                                      // 

    cGuiLabel m_StandardHeader;                                   //                    
    cGuiLabel m_TrainingHeader;                                   //                    
    cGuiLabel m_ArmoryHeader;                                     //                    

    cWorldMap m_WorldMap;                                         // 

    cGuiObject m_StandardArea;                                    // 
    cGuiObject m_StandardMedal;                                   // 
    cGuiObject m_StandardFragment;                                // 
    cGuiLabel  m_aStandardName [2];                               // 
    cGuiLabel  m_aStandardScore[2];                               // 
    cGuiLabel  m_aStandardTime [2];                               // 

    cGuiLabel  m_aMissionName[MENU_GAME_MISSIONS];                // 
    cGuiObject m_aMissionLine[MENU_GAME_MISSIONS];                // 

    cGuiObject m_aaStage[MENU_GAME_MISSIONS][MENU_GAME_STAGES];   // 
    cGuiObject m_aStageCursor;                                    // 
    coreUint8  m_aiStageSelection[2];                             // 

    cGuiObject m_TrainingArea;                                    // 
    cGuiObject m_TrainingMedal;                                   // 
    cGuiObject m_TrainingFragment;                                // 
    cGuiObject m_aTrainingBadge[BADGES];                          // 
    cGuiLabel  m_aTrainingName [2];                               // 
    cGuiLabel  m_aTrainingScore[2];                               // 
    cGuiLabel  m_aTrainingTime [2];                               // 

    cGuiLabel  m_aOptionName[MENU_GAME_OPTIONS];                  // 
    cGuiObject m_aOptionLine[MENU_GAME_OPTIONS];                  // 

    cGuiSwitchBox m_Type;                                         // 
    cGuiSwitchBox m_Mode;                                         // 
    cGuiSwitchBox m_Difficulty;                                   // 

    cGuiLabel  m_aEquipName[MENU_GAME_EQUIPS];                    // 
    cGuiObject m_aEquipLine[MENU_GAME_EQUIPS];                    // 

    cGuiSwitchBox m_aWeapon     [MENU_GAME_PLAYERS];              // 
    cGuiSwitchBox m_aSupport    [MENU_GAME_PLAYERS];              // 
    cGuiObject    m_aWeaponIcon [MENU_GAME_PLAYERS];              // 
    cGuiObject    m_aSupportIcon[MENU_GAME_PLAYERS];              // 

    coreUint8 m_iCurPage;                                         // 
    coreUintW m_aiCurIndex[3];                                    // 

    cMenuNavigator m_Navigator;


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
    void RetrieveStartData(coreInt32* OUTPUT piMissionID, coreUint8* OUTPUT piTakeFrom, coreUint8* OUTPUT piTakeTo);//const;

    // 
    inline const coreUint8& GetSelectedType      ()const                       {return m_Type      .GetCurValue();}
    inline const coreUint8& GetSelectedMode      ()const                       {return m_Mode      .GetCurValue();}
    inline const coreUint8& GetSelectedDifficulty()const                       {return m_Difficulty.GetCurValue();}
    inline const coreUint8& GetSelectedWeapon    (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return m_aWeapon [iIndex].GetCurValue();}
    inline const coreUint8& GetSelectedSupport   (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return m_aSupport[iIndex].GetCurValue();}


private:
    // 
    void __SelectStandard(const coreUintW iMissionIndex);
    void __SelectTraining(const coreUintW iMissionIndex, const coreUintW iSegmentIndex);
};


// ****************************************************************
// score menu class
class cScoreMenu final : public coreMenu
{
private:
    cGuiObject m_Background;                        // 

    cGuiButton m_DefaultTab;                        // 

    cGuiButton m_BackButton;                        // back button

    cGuiObject m_aFilterLine[MENU_EXTRA_FILTERS];   // 

    cGuiSwitchBox m_Mission;                        // 
    cGuiSwitchBox m_Segment;                        // 

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

    cGuiLabel m_TrophyHeader;

    cGuiObject m_ProgressArea;                      // 
    cGuiObject m_aFragment[8];
    cGuiObject m_aTrophy[20];

    cGuiLabel m_TrophyName;
    cGuiLabel m_aTrophyDescription[3];

    cGuiObject m_aFilterLine[MENU_EXTRA_FILTERS];   // 

    cGuiSwitchBox m_Mission;                        // 
    cGuiSwitchBox m_Segment;                        // 

    cGuiLabel  m_aStatsName [MENU_EXTRA_STATS];     // 
    cGuiLabel  m_aStatsValue[MENU_EXTRA_STATS];     // 
    cGuiObject m_aStatsLine [MENU_EXTRA_STATS];     // 
    cScrollBox         m_StatsBox;

    cGuiLabel  m_aOtherName[MENU_EXTRA_OTHERS];     // 
    cGuiObject m_aOtherLine[MENU_EXTRA_OTHERS];     // 

    cGuiButton m_Password;                          // 
    cGuiButton m_Credits;                           // 
    cGuiButton m_Credits2;                          // 

    cMenuNavigator m_Navigator;


public:
    cExtraMenu()noexcept;

    DISABLE_COPY(cExtraMenu)

    // move the extra menu
    void Move()final;

    // 
    void LoadMissions();
    void LoadSegments(const coreUintW iIndex);
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
    };


private:
    cGuiObject m_Background;              // 

    cGuiButton m_VideoTab;                // 
    cGuiButton m_AudioTab;                // 
    cGuiButton m_InputTab;                // 
    cGuiButton m_GameTab;                 // 

    cGuiButton m_SaveButton;              // save button
    cGuiButton m_DiscardButton;           // discard button
    cGuiButton m_BackButton;              // back button

    cGuiLabel  m_aLabel   [ENTRY_MAX];    // 
    cGuiObject m_aLine    [ENTRY_MAX];    // 
    cGuiLabel  m_aCueInput[INPUT_KEYS];   // 
    cGuiLabel  m_aCueRota [2];            // 

    cGuiLabel       m_aDescription[2];         // 
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
    void __LoadInputs();

    // 
    inline cGuiButton& __RetrieveInputButton  (const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < MENU_CONFIG_INPUTS) && (iKeyIndex < INPUT_KEYS)) return *((&m_aInput[iPlayerIndex].oMoveUp)                                         + iKeyIndex);}
    inline coreInt16&  __RetrieveInputCurValue(const coreUintW iPlayerIndex, const coreUintW iKeyIndex) {ASSERT((iPlayerIndex < INPUT_TYPES)        && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[g_CurConfig.Input.aiType[iPlayerIndex]].iMoveUp) + iKeyIndex);}
    inline coreInt16&  __RetrieveInputDirValue(const coreUintW iType,        const coreUintW iKeyIndex) {ASSERT((iType        < INPUT_SETS)         && (iKeyIndex < INPUT_KEYS)) return *((&g_CurConfig.Input.aSet[iType].iMoveUp)                                  + iKeyIndex);}

    // 
    static inline coreFloat __VolumeToFloat(const coreUint8 iVolume) {return I_TO_F(iVolume) * 0.01f;}
    static inline coreUint8 __VolumeToUint8(const coreFloat fVolume) {return F_TO_UI(ROUND(CLAMP(fVolume, 0.0f, 1.0f) * 20.0f) * 5.0f);}

    // convert input key to readable string
    static const coreChar* __PrintKey(const coreUint8 iType, const coreInt16 iKey);
};


// ****************************************************************
// pause menu class
class cPauseMenu final : public coreMenu
{
private:
    cGuiButton m_ResumeButton;   // resume button
    cGuiButton m_ConfigButton;   // config button
    cGuiButton m_ExitButton;     // exit button

    cMenuNavigator m_Navigator;


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

    coreUint32 m_iFinalValue;                                       // (just for display) 
    coreUint32 m_aiFinalPart [MENU_SUMMARY_PARTS];                  // (just for display) 
    coreUint32 m_aiApplyBonus[MENU_SUMMARY_PARTS];                  // 
    coreUint8  m_aiApplyMedal[MENU_SUMMARY_PARTS];                  // 

    coreFlow m_fIntroTimer;                                         // 
    coreFlow m_fOutroTimer;                                         // 

    eSummaryState m_eState;                                         // 


public:
    cSummaryMenu()noexcept;

    DISABLE_COPY(cSummaryMenu)

    // render and move the summary menu
    void Render()final;
    void Move  ()final;

    // 
    void ShowMission();
    void ShowSegment();
    void ShowBegin();

    // 
    void SetHighlightColor(const coreVector3 vColor);


private:
    // 
    void __SetMedalMission(const coreUint8 iMedal);
    void __SetMedalSegment(const coreUintW iIndex, const coreUint8 iMedal);
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

    coreFlow m_fIntroTimer;                      // 
    coreFlow m_fOutroTimer;                      // 

    eFinishState m_eState;                       // 


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
    // this menu is handling score and replay saving
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


public:
    cBridgeMenu()noexcept;

    DISABLE_COPY(cBridgeMenu)

    // move the bridge menu
    void Move()final;

    // 
    void EnterGame();
    void ReturnMenu(const coreUint8 iTarget, const coreBool bPaused);
    void ShowUnlock();

    // 
    coreBool RequiresUnlock()const;

    // 
    inline coreUint8 GetTarget()const {return m_iTarget;}
    inline coreBool  GetPaused()const {return m_bPaused;}
};


// ****************************************************************
// primary menu class
class cMenu final : public coreMenu, public coreResourceRelation
{
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

    cGuiObject m_PauseLayer;             // 
    coreUint32 m_iPauseFrame;            // 

    coreFrameBuffer m_aFrameBuffer[3];   // 
    coreObject2D    m_MixObject;         // 
    
    coreUint8 iForceA;
    coreUint8 iForceB;

    coreTimer m_TransitionTime;          // 
    coreUint8 m_iTransitionState;        // 
    coreMenu* m_pTransitionMenu;         // 

    static coreVector3 m_vHighlightColor;       // 


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
    void ShiftSurface(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface, const coreFloat fSpeed, const coreBool bUpdateFrom = false, const coreBool bUpdateTo = false);
    inline coreBool IsShifting()const {return m_TransitionTime.GetStatus();}

    // 
    void SetHighlightColor(const coreVector3 vColor);
    
    inline const coreVector3& GetHighlightColor()const {return m_vHighlightColor;}

    // 
    static void UpdateLanguageFont();
    static const coreMap<coreString, coreString>& GetLanguageList();

    // menu helper routines
    static void UpdateButton        (cGuiButton*    OUTPUT pButton, const coreBool bFocused, const coreVector3 vFocusColor);
    static void UpdateButton        (cGuiButton*    OUTPUT pButton, const coreBool bFocused);
    static void UpdateSwitchBox     (cGuiSwitchBox* OUTPUT pSwitchBox);
    static void UpdateAnimateProgram(cGuiObject*    OUTPUT pObject);
    static void ApplyMedalTexture   (cGuiObject*    OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType);


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __StartGame();
    void __EndGame();
};


#endif // _P1_GUARD_MENU_H_