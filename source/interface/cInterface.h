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


// ****************************************************************
// interface definitions
#define INTERFACE_VIEWS             (PLAYERS)   // 

#define INTERFACE_MISSION_START     (-1.0f)     // 
#define INTERFACE_MISSION_DURATION   (3.0f)     // 
#define INTERFACE_MISSION_SPEED      (4.0f)     // 
#define INTERFACE_MISSION_ANIMATION  (2.9f)     // 
#define INTERFACE_MISSION_SWAP       (2.0f)     // 


// ****************************************************************
// interface class
class cInterface final
{
private:
    // player view structure
    struct sPlayerView
    {
        coreObject2D m_aHealthBar  [2];   // (0 = background, 1 = foreground) 
        coreLabel    m_aHealthValue[2];   // (0 = ##UNUSED##, 1 = actual value) # 
        coreLabel    m_ScoreMission;      // 
        coreLabel    m_ScoreBoss;         // ##UNUSED## 
        coreLabel    m_Combo;             // 
        coreLabel    m_Chain;             // 

        sPlayerView()noexcept;
    };


private:
    sPlayerView* m_apView[INTERFACE_VIEWS];   // 

    coreObject2D m_aBossHealthBar  [2];       // (0 = background, 1 = foreground) 
    coreLabel    m_aBossHealthValue[2];       // (0 = percent, 1 = actual value) 
    coreLabel    m_BossName;                  // 

    coreObject2D m_BannerBar;                 // 
    coreLabel    m_BannerMain;                // 
    coreLabel    m_aBannerSub[3];             // 

    coreLabel m_aTimeMission[2];              // (0 = seconds, 1 = milliseconds) ##UNUSED## 
    coreLabel m_aTimeBoss   [2];              // 

    float m_fAlphaAll;                        // 
    float m_fAlphaBoss;                       // 
    bool  m_bVisible;                         // 


public:
    cInterface(const coreByte& iNumViews)noexcept;
    ~cInterface();

    DISABLE_COPY(cInterface)

    // render and move the interface
    void Render();
    void Move();

    // 
    void FillMission(const char* pcMain, const char* pcSub);
    void FillBoss   (const char* pcMain, const char* pcSub);

    // 
    inline void SetAlphaAll (const float& fAlpha)   {m_fAlphaAll  = fAlpha;}
    inline void SetAlphaBoss(const float& fAlpha)   {m_fAlphaBoss = fAlpha;}
    inline void SetVisible  (const bool&  bVisible) {m_bVisible   = bVisible;}
};


#endif // _P1_GUARD_INTERFACE_H_