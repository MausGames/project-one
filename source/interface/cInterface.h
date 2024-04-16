///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_INTERFACE_H_
#define _P1_GUARD_INTERFACE_H_

// TODO 1: re-position everything after resolution-change (also menu)
// TODO 3: realtime language update (other locations as well ?), boss-titles and wave-names
// TODO 3: MENU_INSIDE_ALPHA should only be used with inside-hud
// TODO 3: effect on UI when recovering life or shield (and on player, and combat text)
// TODO 1: aktuelles level anzeigen mit wave 01-03, 03-BOSS (e.g. oben rechts im single-player)
// TODO 1: show best time and best score in-game (mission, segment), and when a new record was made! (score also in mission summary)
// TODO 1: health icons should be different, when the player has color (+ transformed to enemy)
// TODO 4: merge wave time and boss time if possible
// TODO 1: wave-name FÜNF is getting shifted down
// TODO 1: add option to display outside interface near line, to handle ultrawide screens
// TODO 3: UI is not correctly updated when changing window size (manually) during pause


// ****************************************************************
// interface definitions
#define INTERFACE_VIEWS                   (PLAYERS)                         // number of player views
#define INTERFACE_LIVES                   (LIVES)                           // 
#define INTERFACE_BADGES                  (BADGES)                          // 
#define INTERFACE_BOSS_DELAY              (1.5f)                            // 
#define INTERFACE_DIALOGS                 (2u)

#define INTERFACE_BANNER_SPEED            (4.0f)                            // fade-in and fade-out speed
//#define INTERFACE_BANNER_SPEED_REV        (1.0f / INTERFACE_BANNER_SPEED)   // 
#define INTERFACE_BANNER_SPEED_MISSION    (1.0f)                            // 
#define INTERFACE_BANNER_ANIMATION        (2.9f)                            // animation duration
#define INTERFACE_BANNER_DURATION_MISSION (3.7f)                            // 
#define INTERFACE_BANNER_DURATION_BOSS    (3.0f)                            // display duration (with fading)  
#define INTERFACE_BANNER_DURATION_SCORE   (5.0f)                            // 
#define INTERFACE_BANNER_DURATION_ALERT   (3.5f)                            // 

#define INTERFACE_BANNER_TYPE_MISSION     (0u)                              // mission banner type
#define INTERFACE_BANNER_TYPE_BOSS        (1u)                              // boss banner type
#define INTERFACE_BANNER_TYPE_WAVE        (2u)                              // 
#define INTERFACE_BANNER_TYPE_SCORE       (3u)                              // 
#define INTERFACE_BANNER_TYPE_ALERT       (4u)                              // 

#define INTERFACE_STORY_SPEED             (1.0f)                            // 
#define INTERFACE_STORY_DURATION          (4.0f)                            // 


// ****************************************************************
// interface class
class cInterface final
{
private:
    // player view structure
    struct sPlayerView final
    {
        cGuiObject aLife[INTERFACE_LIVES];   // player lives
        cGuiObject aShieldBar[3];            // player shield bar (0 = background, 1 = foreground, 2 = empty)
        cGuiLabel  oShieldValue;             // player shield value
        cGuiLabel  oScoreTotal;              // total score
        cGuiLabel  oScoreMission;            // mission score
        cGuiObject oCooldownBar;             // cooldown bar
        cGuiLabel  oComboValue;              // combo label
        cGuiLabel  oChainValue;              // chain value
        coreFlow   fSpin;                    // 
        coreFlow   fSpin2;                   // 

        void Construct(const coreUintW iIndex);
    };


private:
    sPlayerView m_aView[INTERFACE_VIEWS];    // player views
    coreUint8   m_iNumViews;                 // number of constructed player views

    cGuiObject m_aBossHealthBar[3];          // boss health bar (0 = background, 1 = foreground, 2 = empty)
    cGuiLabel  m_BossHealthValue;            // boss health value
    cGuiLabel  m_aBossTime[3];               // boss time (0 = seconds, 1 = deci-seconds, 2 = shift)
    coreFlow   m_fBossSpin;                  // 

    cGuiLabel m_WaveName;                    // wave name
    cGuiLabel m_aWaveTime[3];                // wave time (0 = seconds, 1 = deci-seconds, 2 = shift)

    cGuiObject m_aTurfBar[3];                // 
    cGuiLabel  m_TurfValue;                  // 

    cGuiObject m_GoalMedal;                  // 
    cGuiLabel  m_GoalTime;                   // 

    cGuiObject m_aBadge[INTERFACE_BADGES];   // 

    cGuiObject m_BannerBar;                  // banner background
    cGuiObject m_BannerShadow;               // 
    cGuiLabel  m_aBannerText[4];             // banner labels
    coreFloat  m_fBannerStart;               // animation start time
    coreFloat  m_fBannerDuration;            // 
    coreFloat  m_fBannerSpeed;               // 
    coreUint8  m_iBannerType;                // animation type (boss, mission, wave, score, alert)

    cGuiLabel m_aStoryText[2];               // 
    coreFloat m_fStoryStart;                 // 

    cGuiLabel m_aDialogText[INTERFACE_DIALOGS];              // 

    cGuiObject m_Medal;                      // 

    coreFlow m_fAnimation;                   // 

    coreBool m_bVisible;                     // visibility status
    coreFlow m_fAlphaAll;                    // overall alpha value (except for banner)
    coreFlow m_fAlphaBoss;                   // boss alpha value
    coreFlow m_fAlphaTurf;                   // 
    
    coreFlow m_fAlphaValid;
    
    coreSoundPtr m_pAlertSound;
    coreFloat m_fAlertStart;


public:
    explicit cInterface(const coreUint8 iNumViews)noexcept;
    ~cInterface();

    DISABLE_COPY(cInterface)

    // render and move the interface
    void Render();
    void Move();

    // control banner animation
    void ShowMission(const coreChar*  pcMain, const coreChar* pcSub);
    void ShowMission(const cMission*  pMission);
    void ShowBoss   (const coreChar*  pcMain, const coreChar* pcSub);
    void ShowBoss   (const cBoss*     pBoss, const coreBool bSilent = false);
    void ShowWave   (const coreChar*  pcName);
    void ShowScore  (const coreChar*  pcMain, const coreChar* pcSub, const coreUint8 iMedal, const coreUint8 iMedalType);
    void ShowScore  (const coreUint32 iScore,                        const coreUint8 iMedal, const coreUint8 iMedalType);
    void ShowAlert  ();
    coreBool IsBannerActive()const;

    // 
    void ShowStory(const coreChar* pcRow1, const coreChar* pcRow2 = "");
    coreBool IsStoryActive()const;

    // 
    inline cGuiLabel* GetDialogText(const coreUintW iIndex) {ASSERT(iIndex < INTERFACE_DIALOGS) return &m_aDialogText[iIndex];}

    // 
    void ChangeBannerText(const coreChar* pcMain, const coreChar* pcSub);

    // 
    void UpdateLayout();
    void UpdateEnabled();

    // 
    inline void Reset() {m_fBannerStart = -FLT_MAX; m_fStoryStart = -FLT_MAX; m_fAlertStart = -FLT_MAX;}

    // set object properties
    inline void SetVisible  (const coreBool  bVisible) {m_bVisible   = bVisible;}
    inline void SetAlphaAll (const coreFloat fAlpha)   {m_fAlphaAll  = fAlpha;}
    inline void SetAlphaBoss(const coreFloat fAlpha)   {m_fAlphaBoss = fAlpha;}


private:
    // 
    void __PrepareBanner();
};


#endif // _P1_GUARD_INTERFACE_H_