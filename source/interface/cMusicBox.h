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
// TODO 5: artist display


// ****************************************************************
// music box definitions
#define MUSIC_TRACKS  (ARRAY_SIZE(g_aMusicTrack))   // 
#define MUSIC_ICONS   (3u)                          // 
#define MUSIC_DEFAULT (0u)                          // 

struct sMusicTrack final
{
    const coreChar* pcArtist;    // 
    const coreChar* pcTitle;     // 
    const coreChar* pcContext;   // 
    const coreChar* pcFile;      // 
    coreVector3     vColor;      // 
};

static constexpr sMusicTrack g_aMusicTrack[] =
{
    {"Tim Beek",       "Tech Background",                       "",             "menu.ogg",             cDarkBackground   ::Color},
    {"Isao Yamaguchi", "Melty",                                 "INTRO",        "mission_00_intro.ogg", cCloudBackground  ::Color},
    {"Isao Yamaguchi", "Lonesome Stranger",                     "VIRIDO",       "mission_01_intro.ogg", cGrassBackground  ::Color},
    {"Isao Yamaguchi", "2NEONS II",                             "NEVO",         "mission_02_intro.ogg", cSeaBackground    ::Color},
    {"Isao Yamaguchi", "Ready to kick your \u2217\u2217\u2217", "HARENA",       "mission_03_intro.ogg", cDesertBackground ::Color},
    {"Isao Yamaguchi", "Eyes on Darkness",                      "RUTILUS",      "mission_04_intro.ogg", cSpaceBackground  ::Color},
    {"Isao Yamaguchi", "The Reverse of Destruction",            "GELU",         "mission_05.ogg",       cVolcanoBackground::Color},
    {"Isao Yamaguchi", "Algebraic Love",                        "CALOR",        "mission_06_intro.ogg", cSnowBackground   ::Color},
    {"Isao Yamaguchi", "The Pale of World",                     "MUSCUS",       "mission_07_intro.ogg", cMossBackground   ::Color},
    {"Andrew Sitkov",  "Be Faster",                             "SHINAI",       "boss_00.ogg",          cCloudBackground  ::Color},
    {"Andrew Sitkov",  "Time For Action",                       "TORUS",        "boss_01.ogg",          cGrassBackground  ::Color},
    {"Andrew Sitkov",  "No Way Back",                           "LEVIATHAN",    "boss_02.ogg",          cSeaBackground    ::Color},
    {"Andrew Sitkov",  "Can't Stop Me",                         "TIGER MK-III", "boss_03.ogg",          cDesertBackground ::Color},
    {"Andrew Sitkov",  "Unbreakable",                           "MESSIER 87",   "boss_04.ogg",          cSpaceBackground  ::Color},
    {"Andrew Sitkov",  "Let's Rock",                            "CHOL",         "boss_05.ogg",          cVolcanoBackground::Color},
    {"Andrew Sitkov",  "Rage Machine",                          "ZEROTH",       "boss_06.ogg",          cSnowBackground   ::Color},
    {"Andrew Sitkov",  "Heart of Warrior",                      "NAGUAL",       "boss_07.ogg",          cMossBackground   ::Color},
    {"Isao Yamaguchi", "Blue Garnet",                           "P1",           "boss_08_intro.ogg",    cDarkBackground   ::Color},
    {"Isao Yamaguchi", "Red Sapphire",                          "EIGENGRAU",    "boss_99.ogg",          cDarkBackground   ::Color},
    {"WOW Sound",      "A Waltz",                               "",             "ending_normal.ogg",    cDarkBackground   ::Color},
    {"WOW Sound",      "Reminiscence",                          "",             "ending_secret.ogg",    cDarkBackground   ::Color}
};


// ****************************************************************
// music box class
class cMusicBox final : public cGuiObject
{
private:
    cGuiObject m_Background;               // 

    cGuiLabel  m_Header;                   // 
    coreButton m_BackButton;               // 

    cGuiLabel  m_aTitle  [MUSIC_TRACKS];   // 
    cGuiLabel  m_aContext[MUSIC_TRACKS];   // 
    cGuiObject m_aLine   [MUSIC_TRACKS];   // 
    cGuiObject m_aFocus  [MUSIC_TRACKS];   // 

    cGuiLabel m_PlayIcon;                  // 
    cGuiLabel m_aMusicIcon[MUSIC_ICONS];   // 

    cScrollBox m_ScrollBox;                // 

    coreUint8 m_iSelection;                // 

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