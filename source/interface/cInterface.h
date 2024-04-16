///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_INTERFACE_H_
#define _P1_GUARD_INTERFACE_H_

// TODO: re-position everything after resolution-change (also menu)
// TODO: realtime language update (other locations as well ?), boss-titles and wave-names


// ****************************************************************
// interface definitions
#define INTERFACE_VIEWS                   (PLAYERS)                         // number of player views
#define INTERFACE_LIVES                   (LIVES)                           // 
#define INTERFACE_BOSS_DELAY              (1.5f)                            // 

#define INTERFACE_BANNER_SPEED            (4.0f)                            // fade-in and fade-out speed
#define INTERFACE_BANNER_SPEED_REV        (1.0f / INTERFACE_BANNER_SPEED)   // 
#define INTERFACE_BANNER_ANIMATION        (2.9f)                            // animation duration
#define INTERFACE_BANNER_DURATION_MISSION (3.0f)                            // 
#define INTERFACE_BANNER_DURATION_BOSS    (3.0f)                            // display duration (with fading)  
#define INTERFACE_BANNER_DURATION_SCORE   (2.5f)                            // 

#define INTERFACE_BANNER_TYPE_MISSION     (0u)                              // mission banner type
#define INTERFACE_BANNER_TYPE_BOSS        (1u)                              // boss banner type
#define INTERFACE_BANNER_TYPE_WAVE        (2u)                              // 
#define INTERFACE_BANNER_TYPE_SCORE       (3u)                              // 

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
        cGuiObject aShieldBar[2];            // player shield bar (0 = background, 1 = foreground)
        cGuiLabel  oShieldValue;             // player shield value
        cGuiLabel  oScoreTotal;              // total score
        cGuiLabel  oScoreMission;            // mission score
        cGuiObject oComboBar;                // combo bar
        cGuiLabel  oComboValue;              // combo label
        cGuiLabel  oChainValue;              // chain value
        coreFlow   fSpin;                    // 

        void Construct(const coreUintW iIndex);
    };


private:
    sPlayerView m_aView[INTERFACE_VIEWS];   // player views
    coreUint8   m_iNumViews;                // number of constructed player views

    cGuiObject m_aBossHealthBar[2];         // boss health bar (0 = background, 1 = foreground)
    cGuiLabel  m_BossHealthValue;           // boss health value
    cGuiLabel  m_aBossTime[2];              // boss time (0 = seconds, 1 = deci-seconds)
    coreFlow   m_fBossSpin;                 // 

    cGuiLabel m_WaveName;                   // wave name
    cGuiLabel m_aWaveTime[2];               // wave time (0 = seconds, 1 = deci-seconds)

    cGuiObject m_BannerBar;                 // banner background
    cGuiLabel  m_aBannerText[4];            // banner labels
    coreFloat  m_fBannerStart;              // animation start time
    coreFloat  m_fBannerDuration;           // 
    coreUint8  m_iBannerType;               // animation type (boss, mission, score)

    cGuiLabel m_aStoryText[2];              // 
    coreFloat m_fStoryStart;                // 

    cGuiObject m_Medal;                     // 

    coreFlow m_fAnimation;                  // 

    coreBool m_bVisible;                    // visibility status
    coreFlow m_fAlphaAll;                   // overall alpha value (except for banner)
    coreFlow m_fAlphaBoss;                  // boss alpha value


public:
    explicit cInterface(const coreUint8 iNumViews)noexcept;

    DISABLE_COPY(cInterface)

    // render and move the interface
    void Render();
    void Move();

    // control banner animation
    void ShowMission(const coreChar*  pcMain, const coreChar* pcSub);
    void ShowMission(const cMission*  pMission);
    void ShowBoss   (const coreChar*  pcMain, const coreChar* pcSub);
    void ShowBoss   (const cBoss*     pBoss);
    void ShowWave   (const coreChar*  pcName);
    void ShowScore  (const coreChar*  pcMain, const coreChar* pcSub, const coreUint8 iMedal, const coreUint8 iMedalType);
    void ShowScore  (const coreUint32 iScore,                        const coreUint8 iMedal, const coreUint8 iMedalType);
    void CancelBanner();
    coreBool IsBannerActive()const;

    // 
    void ShowStory(const coreChar* pcRow1, const coreChar* pcRow2 = "");
    coreBool IsStoryActive()const;

    // 
    void UpdateLayout();
    void UpdateEnabled();

    // 
    inline void Reset() {m_fBannerStart = -FLT_MAX; m_fStoryStart = -FLT_MAX;}

    // set object properties
    inline void SetVisible  (const coreBool  bVisible) {m_bVisible   = bVisible;}
    inline void SetAlphaAll (const coreFloat fAlpha)   {m_fAlphaAll  = fAlpha;}
    inline void SetAlphaBoss(const coreFloat fAlpha)   {m_fAlphaBoss = fAlpha;}


private:
    // 
    void __PrepareBanner();
};


#endif // _P1_GUARD_INTERFACE_H_