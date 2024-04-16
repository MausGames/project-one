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

// TODO 1: realtime language update (other locations as well ?), boss-titles and wave-names
// TODO 3: MENU_INSIDE_ALPHA should only be used with inside-hud
// TODO 4: merge wave time and boss time if possible


// ****************************************************************
// interface definitions
#define INTERFACE_VIEWS                   (PLAYERS)        // number of player views
#define INTERFACE_LIVES                   (LIVES)          // 
#define INTERFACE_FRAGMENTS               (FRAGMENTS)      // 
#define INTERFACE_BADGES                  (BADGES  - 3u)   // 
#define INTERFACE_HELPERS                 (HELPERS - 1u)   // 
#define INTERFACE_BOSS_DELAY              (1.5f)           // 
#define INTERFACE_DIALOGS                 (2u)             // 
#define INTERFACE_INVALID_START           (-3600.0f)       // 

#define INTERFACE_BANNER_SPEED            (4.0f)           // fade-in and fade-out speed
#define INTERFACE_BANNER_SPEED_OPEN       (1.0f)           // 
#define INTERFACE_BANNER_ANIMATION        (2.9f)           // animation duration
#define INTERFACE_BANNER_DURATION_MISSION (3.7f)           // 
#define INTERFACE_BANNER_DURATION_BOSS    (3.7f)           // display duration (with fading)  
#define INTERFACE_BANNER_DURATION_SCORE   (5.0f)           // 
#define INTERFACE_BANNER_DURATION_ALERT   (3.5f)           // 

#define INTERFACE_BANNER_TYPE_MISSION     (0u)             // mission banner type
#define INTERFACE_BANNER_TYPE_BOSS        (1u)             // boss banner type
#define INTERFACE_BANNER_TYPE_WAVE        (2u)             // 
#define INTERFACE_BANNER_TYPE_SCORE       (3u)             // 
#define INTERFACE_BANNER_TYPE_ALERT       (4u)             // 

#define INTERFACE_STORY_SPEED             (1.0f)           // 
#define INTERFACE_STORY_DURATION          (4.0f)           // 

#define INTERFACE_FRAGMENT_SPEED          (1.5f)           // 
#define INTERFACE_FRAGMENT_DURATION       (5.0f)           // 
#define INTERFACE_FRAGMENT_SCALE          (0.4f)           // 
#define INTERFACE_FRAGMENT_TYPE_SHOW      (0xFFu)          // 
#define INTERFACE_FRAGMENT_TYPE_HIDE      (0xFEu)          // 


// ****************************************************************
// interface class
class cInterface final : public coreTranslate
{
private:
    // player view structure
    struct sPlayerView final
    {
        cGuiObject aLife[INTERFACE_LIVES];   // player lives
        cGuiObject aShieldBar[3];            // player shield bar (0 = background, 1 = foreground, 2 = empty)
        cGuiLabel  oShieldValue;             // player shield value
        coreFlow   fShieldBump;              // 
        coreInt32  iShieldOld;               // 
        cGuiLabel  oScore;                   // 
        cGuiObject oCooldownBar;             // cooldown bar
        cGuiLabel  oComboValue;              // combo label
        cGuiLabel  oChainValue;              // chain value
        cGuiLabel  oImmune;                  // 
        coreFlow   fImmuneTime;              // 
        coreFlow   fSpin;                    // 
        coreFlow   fSpin2;                   // 

        void Construct(const coreUintW iIndex);
    };


private:
    sPlayerView m_aView[INTERFACE_VIEWS];           // player views
    coreUint8   m_iNumViews;                        // number of constructed player views

    cGuiObject m_aBossHealthBar[3];                 // boss health bar (0 = background, 1 = foreground, 2 = empty)
    cGuiLabel  m_BossHealthValue;                   // boss health value
    cGuiLabel  m_aBossTime[3];                      // boss time (0 = seconds, 1 = deci-seconds, 2 = shift)
    coreFlow   m_fBossSpin;                         // 

    cGuiLabel m_aWaveTime[3];                       // wave time (0 = seconds, 1 = deci-seconds, 2 = shift)

    cGuiLabel  m_SegmentName;                       // segment name
    cGuiLabel  m_SegmentBest;                       // 
    coreString m_sSegmentString;                    // 

    cGuiObject m_aTurfBar[3];                       // 
    cGuiLabel  m_TurfValue;                         // 

    cGuiObject m_GoalMedal;                         // 
    cGuiLabel  m_GoalTime;                          // 
    coreFlow   m_fGoalBump;                         // 
    coreUint8  m_iGoalOld;                          // 

    cGuiObject m_aHelper     [INTERFACE_HELPERS];   // 
    cGuiObject m_aHelperWave [INTERFACE_HELPERS];   // 
    coreFlow   m_afHelperBump[INTERFACE_HELPERS];   // 
    coreUint8  m_iHelperState;                      // 

    cGuiObject m_aBadge     [INTERFACE_BADGES];     // 
    cGuiObject m_aBadgeWave [INTERFACE_BADGES];     // 
    coreFlow   m_afBadgeBump[INTERFACE_BADGES];     // 
    coreUint8  m_iBadgeState;                       // 

    cGuiObject m_BannerBar;                         // banner background
    cGuiObject m_BannerShadow;                      // 
    cGuiObject m_aBannerIcon[2];                    // 
    cGuiLabel  m_aBannerText[4];                    // banner labels
    coreFloat  m_fBannerStart;                      // animation start time
    coreFloat  m_fBannerDuration;                   // 
    coreFloat  m_fBannerSpeed;                      // 
    coreUint8  m_iBannerType;                       // animation type (boss, mission, wave, score, alert)

    cGuiLabel m_aStoryText[2];                      // 
    coreFloat m_fStoryStart;                        // 

    cGuiLabel m_aDialogText[INTERFACE_DIALOGS];     // 

    cGuiObject m_aFragment[INTERFACE_FRAGMENTS];    // 
    cGuiObject m_aFragmentTable[2];                 // 
    cGuiObject m_FragmentShadow;                    // 
    coreFloat  m_fFragmentStart;                    // 
    coreUint8  m_iFragmentNew;                      // 
    coreUint8  m_iFragmentState;                    // 

    cGuiObject m_Medal;                             // 

    coreSoundPtr m_pAlertSound;                     // 
    coreFloat    m_fAlertStart;                     // 

    coreFlow m_fAnimation;                          // 
    coreFlow m_fRotation;                           // 
    coreFlow m_fShake;                              // 

    coreBool m_bVisible;                            // visibility status
    coreFlow m_fAlphaAll;                           // overall alpha value (except for banner)
    coreFlow m_fAlphaBoss;                          // boss alpha value
    coreFlow m_fAlphaWave;                          // 
    coreFlow m_fAlphaSegment;                       // 
    coreFlow m_fAlphaTurf;                          // 
    coreFlow m_fAlphaGoal;                          // 
    coreFlow m_fAlphaBadge;                         // 


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
    void ShowFragment(const coreUint8 iNewIndex, const coreUint32 iOldBits);
    void ShowFragment(const coreUint8 iNewIndex);
    coreBool IsFragmentActive()const;

    // 
    void UpdateLayout();
    void UpdateSpacing();
    void UpdateEnabled();
    void MoveTimeless();

    // 
    inline void PingImmune(const coreUintW iIndex) {ASSERT(iIndex < INTERFACE_VIEWS) m_aView[iIndex].fImmuneTime = 1.0f;}

    // 
    inline void Reset() {m_fBannerStart = INTERFACE_INVALID_START; m_fStoryStart = INTERFACE_INVALID_START; m_fFragmentStart = INTERFACE_INVALID_START; m_fAlertStart = INTERFACE_INVALID_START;}

    // set object properties
    inline void SetVisible  (const coreBool  bVisible) {m_bVisible   = bVisible;}
    inline void SetAlphaAll (const coreFloat fAlpha)   {m_fAlphaAll  = fAlpha;}
    inline void SetAlphaBoss(const coreFloat fAlpha)   {m_fAlphaBoss = fAlpha;}
    inline void SetAlphaWave(const coreFloat fAlpha)   {m_fAlphaWave = fAlpha;}

    // 
    inline cGuiLabel* GetDialogText(const coreUintW iIndex) {ASSERT(iIndex < INTERFACE_DIALOGS) return &m_aDialogText[iIndex];}


private:
    // update object after modification
    void __Update()final;

    // 
    void __PrepareBanner();

    // 
    static inline coreBool __IsFlipped() {return IsHorizontal(MapToAxis(Core::System->GetResolution(), g_vHudDirection));}
};


#endif // _P1_GUARD_INTERFACE_H_