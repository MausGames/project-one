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
// TODO: move mouse to buttons on joystick-input
// TODO: real-time sound-configuration
// TODO: score-menu names and all name-inputs MUST support all languages (japanese (keifont), russian (default), arabic (default), ...)
// TODO: score-menu names must be sanitized
// TODO: unload fonts currently not used (e.g. from score-menu)
// TODO: options menu: ask if values should be discarded, ask if want to exit instead of saving
// TODO: options menu: 15 second on video change, yes, no
// TODO: rumble when changing rumble-option
// TODO: display unattached joysticks and joystick names somehow
// TODO: highlight which joystick is which input set
// TODO: summary_ add separate total-score for each player
// TODO: update texture filter and render quality in realtime
// TODO: double initial languages by switching to two columns (on demand?)
// TODO: stages in GameMenu should be called segments
// TODO: upper white border in settings menu "moves" during transition on 1760x990, check all menus for moving during transition in common resolutions
// TODO: every object in menu needs outline: weapon icons


// ****************************************************************
// menu definitions
#define MENU_LIGHT_ACTIVE             (1.0f)     // visible strength of active menu objects
#define MENU_LIGHT_IDLE               (0.667f)   // visible strength of idle menu objects
#define MENU_CONTRAST_WHITE           (0.8f)     // white contrast value (to reduce eye strain)
#define MENU_CONTRAST_BLACK           (0.04f)    // black contrast value

#define MENU_GAME_MISSIONS            (9u)
#define MENU_GAME_STAGES              (SEGMENTS)
#define MENU_GAME_PLAYERS             (PLAYERS)
#define MENU_GAME_OPTIONS             (3u)
#define MENU_SCORE_ENTRIES            (10u)
#define MENU_REPLAY_ENTRIES           (5u)
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
#define MENU_FONT_STANDARD_1    MENU_FONT_DEFAULT, (16u)
#define MENU_FONT_STANDARD_2    MENU_FONT_DEFAULT, (22u)
#define MENU_FONT_STANDARD_3    MENU_FONT_DEFAULT, (32u)
#define MENU_FONT_STANDARD_4    MENU_FONT_DEFAULT, (42u)
#define MENU_FONT_STANDARD_5    MENU_FONT_DEFAULT, (72u)
#define MENU_FONT_DYNAMIC_1     "dynamic_font",    (16u)
#define MENU_FONT_DYNAMIC_2     "dynamic_font",    (22u)
#define MENU_FONT_DYNAMIC_3     "dynamic_font",    (32u)
#define MENU_FONT_DYNAMIC_4     "dynamic_font",    (42u)
#define MENU_FONT_DYNAMIC_5     "dynamic_font",    (72u)
#define MENU_FONT_ICON_1        "fontawesome.otf", (20u)
#define MENU_FONT_ICON_2        "fontawesome.otf", (24u)
#define MENU_FONT_ICON_3        "fontawesome.otf", (40u)
#define MENU_OUTLINE_SMALL      (2u)
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
    SURFACE_MAX,

    SURFACE_INTRO_EMPTY = 0u,
    SURFACE_INTRO_WELCOME,
    SURFACE_INTRO_LANGUAGE,
    SURFACE_INTRO_MAX,

    SURFACE_TITLE_LOGO = 0u,
    SURFACE_TITLE_FIRST,
    SURFACE_TITLE_RETURN,
    SURFACE_TITLE_MAX,

    SURFACE_MAIN_DEFAULT = 0u,
    SURFACE_MAIN_MAX,

    SURFACE_GAME_STANDARD = 0u,
    SURFACE_GAME_TRAINING,
    SURFACE_GAME_ARMORY,
    SURFACE_GAME_MAX,

    SURFACE_SCORE_DEFAULT = 0u,
    SURFACE_SCORE_MAX,

    SURFACE_REPLAY_DEFAULT = 0u,
    SURFACE_REPLAY_MAX,

    SURFACE_EXTRA_DEFAULT = 0u,
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
    SURFACE_FINISH_MAX
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
    ENTRY_VIDEO,

    ENTRY_AUDIO_GLOBALVOLUME = ENTRY_VIDEO,
    ENTRY_AUDIO_MUSICVOLUME,
    ENTRY_AUDIO_EFFECTVOLUME,
    ENTRY_AUDIO_AMBIENTVOLUME,
    ENTRY_AUDIO,

    ENTRY_INPUT_TYPE = ENTRY_AUDIO,
    ENTRY_INPUT_RUMBLE,
    ENTRY_INPUT_FIREMODE = ENTRY_INPUT_RUMBLE,
    ENTRY_INPUT_MOVEUP,
    ENTRY_INPUT_MOVELEFT,
    ENTRY_INPUT_MOVEDOWN,
    ENTRY_INPUT_MOVERIGHT,
    ENTRY_INPUT_ACTION1,
    ENTRY_INPUT_ACTION2,
    ENTRY_INPUT_ACTION3,
    ENTRY_INPUT_ACTION4,
    ENTRY_INPUT_ACTION5,
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
    coreLabel m_WelcomeText;                                   // 

    coreTimer m_IntroTimer;                                    // intro animation 
    coreUint8 m_iIntroStatus;                                  // 

    coreLookup<std::string, coreButton*> m_apLanguageButton;   // list with buttons for valid language files


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
    coreObject2D m_GameLogo;        // game logo

    coreLabel m_PromptText;         // 
    coreFlow  m_fPromptAnimation;   // 
    coreFlow  m_fPromptExpand;      // 

    coreLabel m_aVersionText[2];    // hard-coded version info strings 

    coreFlow m_fReturnTimer;        // 


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
    coreButton m_StartButton;    // start button
    coreButton m_ScoreButton;    // score button
    coreButton m_ReplayButton;   // replay button
    coreButton m_ExtraButton;    // extra button
    coreButton m_ConfigButton;   // config button
    coreButton m_ExitButton;     // exit button

    cMenuInput m_MenuInput;  


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
    coreObject2D m_DirectoryBackground;                  // 
    coreObject2D m_ArmoryBackground;                     // 

    coreButton m_StandardTab;                            // 
    coreButton m_TrainingTab;                            // 

    coreButton m_StartButton;                            // start button
    coreButton m_CancelButton;                           // cancel button
    coreButton m_BackButton;                             // back button

    coreLabel m_DirectoryHeader;                         // 
    coreLabel m_ArmoryHeader;                            // 

    cWorldMap m_WorldMap;                                // 

    coreLabel    m_aMissionName[MENU_GAME_MISSIONS];     // 
    coreObject2D m_aMissionLine[MENU_GAME_MISSIONS];     // 

    coreObject2D m_StageArea;                            // 
    coreButton   m_aStage[MENU_GAME_STAGES];             // 

    coreLabel    m_aOptionName[MENU_GAME_OPTIONS];       // 
    coreObject2D m_aOptionLine[MENU_GAME_OPTIONS];       // 

    coreSwitchBoxU8 m_Players;                           // 
    coreSwitchBoxU8 m_aWeapon     [MENU_GAME_PLAYERS];   // 
    coreSwitchBoxU8 m_aSupport    [MENU_GAME_PLAYERS];   // 
    coreObject2D    m_aWeaponIcon [MENU_GAME_PLAYERS];   // 
    coreObject2D    m_aSupportIcon[MENU_GAME_PLAYERS];   // 


    //cPlayer* m_apShip[2];
    sGameInput m_aShipInput[2];

    cMenuInput m_MenuInput;  



public:
    cGameMenu()noexcept;
    ~cGameMenu()final;

    DISABLE_COPY(cGameMenu)

        void Render()final;

    // move the game menu
    void Move()final;

    // 
    void ActivateFirstPlay();
    void DeactivateFirstPlay();

    // 
    void LoadValues();
    void SaveValues();

    // 
    inline const coreUint8& GetMissionIndex   ()const                       {return m_WorldMap.GetSelectionIndex();}
    inline const coreUint8& GetSelectedPlayers()const                       {return m_Players.GetCurEntry().tValue;}
    inline const coreUint8& GetSelectedWeapon (const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return m_aWeapon [iIndex].GetCurEntry().tValue;}
    inline const coreUint8& GetSelectedSupport(const coreUintW iIndex)const {ASSERT(iIndex < MENU_GAME_PLAYERS) return m_aSupport[iIndex].GetCurEntry().tValue;}
};


// ****************************************************************
// score menu class
class cScoreMenu final : public coreMenu
{
private:
    coreObject2D m_Background;                   // 

    coreButton m_BackButton;                     // back button

    coreLabel    m_aRank [MENU_SCORE_ENTRIES];   // 
    coreLabel    m_aName [MENU_SCORE_ENTRIES];   // 
    coreLabel    m_aScore[MENU_SCORE_ENTRIES];   // 
    coreObject2D m_aLine [MENU_SCORE_ENTRIES];   // 


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

    coreLabel    m_aName[MENU_REPLAY_ENTRIES];   // 
    coreLabel    m_aTime[MENU_REPLAY_ENTRIES];   // 
    coreObject2D m_aLine[MENU_REPLAY_ENTRIES];   // 

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
        coreSwitchBoxU8 oFireMode;
        coreButton      oMoveUp;
        coreButton      oMoveLeft;
        coreButton      oMoveDown;
        coreButton      oMoveRight;
        coreButton      aAction[INPUT_KEYS_ACTION];
    };


private:
    coreObject2D m_Background;              // 

    coreButton m_VideoTab;                  // 
    coreButton m_AudioTab;                  // 
    coreButton m_InputTab;                  // 
    coreButton m_GameTab;                   // 

    coreButton m_SaveButton;                // save button
    coreButton m_DiscardButton;             // discard button
    coreButton m_BackButton;                // back button

    coreLabel    m_aLabel   [ENTRY_MAX];    // 
    coreObject2D m_aLine    [ENTRY_MAX];    // 
    coreLabel    m_aCueInput[INPUT_KEYS];   // 
    coreLabel    m_aCueRota [2];            // 

    coreSwitchBoxU8 m_Monitor;
    coreSwitchBoxU8 m_Resolution;
    coreSwitchBoxU8 m_DisplayMode;
    coreSwitchBoxU8 m_AntiAliasing;
    coreSwitchBoxU8 m_TextureFilter;
    coreSwitchBoxU8 m_RenderQuality;
    coreSwitchBoxU8 m_ShadowQuality;
    coreSwitchBoxU8 m_GlobalVolume;
    coreSwitchBoxU8 m_MusicVolume;
    coreSwitchBoxU8 m_EffectVolume;
    coreSwitchBoxU8 m_AmbientVolume;
    coreSwitchBoxU8 m_Language;
    coreSwitchBoxU8 m_TextSize;
    coreSwitchBoxU8 m_GameRotation;
    coreSwitchBoxU8 m_GameScale;
    coreSwitchBoxU8 m_GameSpeed;
    coreSwitchBoxU8 m_HudRotation;
    coreSwitchBoxU8 m_HudScale;
    coreSwitchBoxU8 m_HudType;
    coreSwitchBoxU8 m_UpdateFreq;
    coreSwitchBoxU8 m_Version;
    coreSwitchBoxU8 m_MirrorMode;

    sPlayerInput m_aInput[MENU_CONFIG_INPUTS];
    coreButton   m_SwapInput;

    coreUintW                          m_iCurMonitorIndex;
    coreLookup<coreUintW, std::string> m_asCurResolution;
    
    
cMenuInput m_MenuInput;  


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
    void __UpdateVolume();
    void __UpdateLanguage();
    void __UpdateInterface();

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
// pause menu class
class cPauseMenu final : public coreMenu
{
private:
    coreButton m_ResumeButton;   // resume button
    coreButton m_ConfigButton;   // config button
    coreButton m_ExitButton;     // exit button


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
    coreObject2D m_BackgroundMain;                                  // 
    coreObject2D m_BackgroundCoop;                                  // 

    coreLabel m_Title;                                              // 

    coreLabel m_aHeader[2];                                         // 

    coreObject2D m_MedalMission;                                    // 
    coreObject2D m_aMedalSegment[MENU_SUMMARY_MEDALS];              // 

    coreLabel m_aName [MENU_SUMMARY_ENTRIES];                       // 
    coreLabel m_aValue[MENU_SUMMARY_ENTRIES];                       // 
    coreLabel m_aaPart[MENU_SUMMARY_ENTRIES][MENU_SUMMARY_PARTS];   // 

    coreLabel m_TotalName;                                          // 
    coreLabel m_TotalValue;                                         // 
    coreLabel m_aTotalPart[MENU_SUMMARY_PARTS];                     // 

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
    void ShowNormal();
    void ShowBegin();


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
    coreObject2D m_Background;                             // 

    coreLabel    m_GameOverText;                           // 
    coreLabel    m_ContinueText;                           // 
    coreLabel    m_ContinueTimer;                          // 
    coreObject2D m_ContinueImage[MENU_DEFEAT_CONTINUES];   // 

    coreFlow m_fCountdown;                                 // 
    coreFlow m_fBurst;                                     // 

    coreFlow m_fIntroTimer;                                // 
    coreFlow m_fOutroTimer;                                // 

    eDefeatState m_eState;                                 // 


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
    coreObject2D m_Background;                   // 

    coreLabel m_ThankYouText;                    // 

    coreLabel m_TotalName;                       // 
    coreLabel m_TotalValue;                      // 
    coreLabel m_aTotalPart[MENU_FINISH_PARTS];   // 

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
    
    // this menu is handling score and replay saving
};


// ****************************************************************
// master menu class
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

    cMsgBox  m_MsgBox;                   // message box overlay
    cTooltip m_Tooltip;                  // tooltip overlay

    coreObject2D m_PauseLayer;           // 
    coreUint32   m_iPauseFrame;          // 

    coreFrameBuffer m_aFrameBuffer[3];   // 
    coreObject2D    m_MixObject;         // 
    
    coreUint8 iForceA;
    coreUint8 iForceB;

    coreTimer m_TransitionTime;          // 
    coreUint8 m_iTransitionState;        // 
    coreMenu* m_pTransitionMenu;         // 


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
    void ShiftSurface(coreMenu* OUTPUT pMenu, const coreUint8 iNewSurface, const coreFloat fSpeed, const coreBool bUpdateFrom = false, const coreBool bUpdateTo = false);
    inline coreBool IsShifting()const {return m_TransitionTime.GetStatus();}

    // 
    static void UpdateLanguageFont();
    static const coreLookup<std::string, std::string>& GetLanguageList();

    // menu helper routines
    static void UpdateButton        (coreButton*      OUTPUT pButton, const coreBool bFocused, const coreVector3& vFocusColor = COLOR_MENU_WHITE);
    static void UpdateSwitchBox     (coreSwitchBoxU8* OUTPUT pSwitchBox);
    static void UpdateAnimateProgram(coreObject2D*    OUTPUT pObject);
    static void ApplyMedalTexture   (coreObject2D*    OUTPUT pObject, const coreUint8 iMedal, const coreUint8 iMedalType);


private:
    // reset with the resource manager
    void __Reset(const coreResourceReset eInit)final;

    // 
    void __StartGame();
    void __EndGame();
};


#endif // _P1_GUARD_MENU_H_