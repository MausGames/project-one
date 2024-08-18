///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_MUSICBOX_H_
#define _P1_GUARD_MUSICBOX_H_

// TODO 3: volume control


// ****************************************************************
// music box definitions
#define MUSIC_TRACKS  (ARRAY_SIZE(g_aMusicTrack))   // 
#define MUSIC_ICONS   (3u)                          // 
#define MUSIC_DEFAULT (0u)                          // 

struct sMusicTrack final
{
    const coreChar* pcArtist;      // 
    const coreChar* pcTitle;       // 
    const coreChar* pcContext;     // 
    const coreChar* pcFile;        // 
    coreVector3     vColor;        // 
    coreInt32       iBackground;   // 
};

static constexpr sMusicTrack g_aMusicTrack[] =
{
    {"Tim Beek",       "Tech Background",                       "",             "menu",             cDarkBackground   ::Color, 0},
    {"Isao Yamaguchi", "Melty",                                 "INTRO",        "mission_00_intro", cCloudBackground  ::Color, cCloudBackground  ::ID},
    {"Isao Yamaguchi", "Lonesome Stranger",                     "VIRIDO",       "mission_01_intro", cGrassBackground  ::Color, cGrassBackground  ::ID},
    {"Isao Yamaguchi", "2NEONS II",                             "NEVO",         "mission_02_intro", cSeaBackground    ::Color, cSeaBackground    ::ID},
    {"Isao Yamaguchi", "Ready to kick your \u2217\u2217\u2217", "HARENA",       "mission_03_intro", cDesertBackground ::Color, cDesertBackground ::ID},
    {"Isao Yamaguchi", "Eyes on Darkness",                      "RUTILUS",      "mission_04_intro", cSpaceBackground  ::Color, cSpaceBackground  ::ID},
    {"Isao Yamaguchi", "The Reverse of Destruction",            "GELU",         "mission_05",       cVolcanoBackground::Color, cVolcanoBackground::ID},
    {"Isao Yamaguchi", "Algebraic Love",                        "CALOR",        "mission_06_intro", cSnowBackground   ::Color, cSnowBackground   ::ID},
    {"Isao Yamaguchi", "The Pale of World",                     "MUSCUS",       "mission_07_intro", cMossBackground   ::Color, cMossBackground   ::ID},
    {"Andrew Sitkov",  "Be Faster",                             "SHINAI",       "boss_00",          cCloudBackground  ::Color, cCloudBackground  ::ID},
    {"Andrew Sitkov",  "Time For Action",                       "TORUS",        "boss_01",          cGrassBackground  ::Color, cGrassBackground  ::ID},
    {"Andrew Sitkov",  "No Way Back",                           "LEVIATHAN",    "boss_02",          cSeaBackground    ::Color, cSeaBackground    ::ID},
    {"Andrew Sitkov",  "Can't Stop Me",                         "TIGER MK-III", "boss_03",          cDesertBackground ::Color, cDesertBackground ::ID},
    {"Andrew Sitkov",  "Unbreakable",                           "MESSIER 87",   "boss_04",          cSpaceBackground  ::Color, cSpaceBackground  ::ID},
    {"Andrew Sitkov",  "Let's Rock",                            "CHOL",         "boss_05",          cVolcanoBackground::Color, cVolcanoBackground::ID},
    {"Andrew Sitkov",  "Rage Machine",                          "ZEROTH",       "boss_06",          cSnowBackground   ::Color, cSnowBackground   ::ID},
    {"Andrew Sitkov",  "Heart of Warrior",                      "NAGUAL",       "boss_07",          cMossBackground   ::Color, cMossBackground   ::ID},
    {"Isao Yamaguchi", "Blue Garnet",                           "P1",           "boss_08_intro",    cDarkBackground   ::Color, cDarkBackground   ::ID},
    {"Isao Yamaguchi", "Red Sapphire",                          "EIGENGRAU",    "boss_99",          cDarkBackground   ::Color, cDarkBackground   ::ID},
    {"WOW Sound",      "A Waltz",                               "",             "ending_normal",    cDarkBackground   ::Color, 0},
    {"WOW Sound",      "Reminiscence",                          "",             "ending_secret",    cDarkBackground   ::Color, 0}
};


// ****************************************************************
// music box class
class cMusicBox final : public cGuiObject
{
private:
    cGuiObject m_Background;               // 

    cGuiLabel  m_Header;                   // 
    coreButton m_BackButton;               // 

    cGuiLabel m_BigTitle;                  // 
    cGuiLabel m_BigArtist;                 // 

    cGuiLabel  m_aTitle  [MUSIC_TRACKS];   // 
    cGuiLabel  m_aContext[MUSIC_TRACKS];   // 
    cGuiObject m_aLine   [MUSIC_TRACKS];   // 
    cGuiObject m_aFocus  [MUSIC_TRACKS];   // 

    cGuiLabel m_PlayIcon;                  // 
    cGuiLabel m_aMusicIcon[MUSIC_ICONS];   // 

    cScrollBox m_ScrollBox;                // 

    coreUint8 m_iSelection;                // 
    coreFlow  m_fDelay;                    // 

    cMenuNavigator m_Navigator;            // 


public:
    cMusicBox()noexcept;

    DISABLE_COPY(cMusicBox)

    // render and move the music box
    void Render()final;
    void Move  ()final;

    // 
    void ResetSelection();
    void ResetNavigator();

    // 
    inline coreBool GetFinished()const {return m_BackButton.IsClicked();}
};


#endif // _P1_GUARD_MUSICBOX_H_