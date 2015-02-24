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
#define INTERFACE_VIEWS (PLAYERS)   // 


// ****************************************************************
// interface class
class cInterface final
{
private:
    // player view structure
    struct sPlayerView
    {
        coreObject2D m_aHealthBar[2];   // (0 = background, 1 = foreground) 
        coreLabel    m_HealthValue;     // 
        coreLabel    m_ScoreMission;    // 
        coreLabel    m_ScoreBoss;       // 
        coreLabel    m_Combo;           // 
        coreLabel    m_Chain;           // 

        sPlayerView()noexcept;
    };


private:
    sPlayerView* m_apView[INTERFACE_VIEWS];   // 

    coreObject2D m_aBossHealthBar[2];         // (0 = background, 1 = foreground) 
    coreLabel    m_BossHealthValue;           // 
    coreLabel    m_BossName;                  // 

    coreObject2D m_BannerBar;                 // 
    coreLabel    m_BannerMain;                // 
    coreLabel    m_aBannerSub[3];             // 

    coreLabel m_aTimeMission[2];              // (0 = seconds, 1 = milliseconds) 
    coreLabel m_aTimeBoss   [2];              // (0 = seconds, 1 = milliseconds) 


public:
    cInterface(const coreByte& iNumViews)noexcept;
    ~cInterface();

    DISABLE_COPY(cInterface)

    // render and move the interface
    void Render();
    void Move();

    // funktion zum anzeigen des missions-banners
    // funktion zum anzeigen eines boss-banners, dedicated zum übersteuern
};


#endif // _P1_GUARD_INTERFACE_H_