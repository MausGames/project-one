//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_INTERFACE_H_
#define _P1_GUARD_INTERFACE_H_

// TODO: re-position everything after resolution-change
// TODO: re-order rendering to reduce shader/texture changes


// ****************************************************************
// interface definitions
#define INTERFACE_VIEWS               (PLAYERS)   // number of player views
#define INTERFACE_BOSS_DELAY          (1.5f)      // 

#define INTERFACE_BANNER_SPEED        (4.0f)      // fade-in and fade-out speed
#define INTERFACE_BANNER_DURATION     (3.0f)      // display duration (with fading)
#define INTERFACE_BANNER_ANIMATION    (2.9f)      // animation duration

#define INTERFACE_BANNER_TYPE_BOSS    (false)     // boss banner type
#define INTERFACE_BANNER_TYPE_MISSION (true)      // mission banner type

#define INTERFACE_STORY_SPEED         (1.0f)      // 
#define INTERFACE_STORY_DURATION      (4.0f)      // 

#define INTERFACE_BONUS_SPEED         (4.0f)      // 
#define INTERFACE_BONUS_DURATION      (4.0f)      // 


// ****************************************************************
// interface class
class cInterface final
{
private:
    // player view structure
    struct sPlayerView final
    {
        coreObject2D aHealthBar  [2];   // player health bar   (0 = background, 1 = foreground)
        coreLabel    aHealthValue[2];   // player health value (0 = armor text, 1 = actual value)
        coreLabel    oScore;            // score label
        coreLabel    oCombo;            // combo label
        coreObject2D oChainBar;         // chain bar
        coreLabel    oChainValue;       // chain value

        void Construct(const coreUintW iIndex);
    };


private:
    sPlayerView m_aView[INTERFACE_VIEWS];   // player views
    coreUint8   m_iNumViews;                // number of constructed player views

    coreObject2D m_aBossHealthBar  [2];     // boss health bar   (0 = background, 1 = foreground)
    coreLabel    m_aBossHealthValue[2];     // boss health value (0 = percent,    1 = actual value)
    coreLabel    m_aBossTime       [2];     // current boss time (0 = seconds,    1 = deci-seconds)
    coreLabel    m_BossName;                // current boss name

    coreObject2D m_BannerBar;               // banner background
    coreLabel    m_aBannerText[4];          // banner labels
    coreFloat    m_fBannerStart;            // animation start time
    coreBool     m_bBannerType;             // animation type (boss, mission)

    coreLabel m_aStoryText[2];              // 
    coreFloat m_fStoryStart;                // 

    coreLabel m_aBonusText[2];              // 
    coreFloat m_fBonusStart;                // 

    coreBool  m_bVisible;                   // visibility status
    coreFloat m_fAlphaAll;                  // overall alpha value (except for banner)
    coreFloat m_fAlphaBoss;                 // boss alpha value


public:
    explicit cInterface(const coreUint8 iNumViews)noexcept;

    DISABLE_COPY(cInterface)

    // render and move the interface
    void Render();
    void Move();

    // control banner animation
    void ShowBoss   (const coreChar* pcMain, const coreChar* pcSub);
    void ShowBoss   (const cBoss*    pBoss);
    void ShowMission(const coreChar* pcMain, const coreChar* pcSub);
    void ShowMission(const cMission* pMission);
    coreBool IsBannerActive()const;

    // 
    void ShowStory(const coreChar* pcRow1, const coreChar* pcRow2 = "");
    coreBool IsStoryActive(const coreFloat fOffset)const;

    // 
    void ShowBonus(const coreChar* pcValue, const coreChar* pcTitle = "");
    coreBool IsBonusActive(const coreFloat fOffset)const;

    // 
    void UpdateLayout();

    // set object properties
    inline void SetVisible  (const coreBool  bVisible) {m_bVisible   = bVisible;}
    inline void SetAlphaAll (const coreFloat fAlpha)   {m_fAlphaAll  = fAlpha;}
    inline void SetAlphaBoss(const coreFloat fAlpha)   {m_fAlphaBoss = fAlpha;}
};


#endif // _P1_GUARD_INTERFACE_H_