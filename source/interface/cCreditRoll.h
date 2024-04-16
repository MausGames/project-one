///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#pragma once
#ifndef _P1_GUARD_CREDITROLL_H_
#define _P1_GUARD_CREDITROLL_H_

// TODO 3: add label pool


// ****************************************************************
// credit roll definitions
#define CREDIT_ENTRIES           (ARRAY_SIZE(g_aapcCreditEntry))           // 
#define CREDIT_ENTRIES_MUSIC     (ARRAY_SIZE(g_apcCreditEntryMusic))       // 
#define CREDIT_ENTRIES_SOUNDS    (ARRAY_SIZE(g_apcCreditEntrySounds))      // 
#define CREDIT_ENTRIES_TEXTURES  (ARRAY_SIZE(g_apcCreditEntryTextures))    // 
#define CREDIT_ENTRIES_MODELS    (ARRAY_SIZE(g_apcCreditEntryModels))      // 
#define CREDIT_ENTRIES_FONTS     (ARRAY_SIZE(g_apcCreditEntryFonts))       // 
#define CREDIT_ENTRIES_LIBRARIES (ARRAY_SIZE(g_apcCreditEntryLibraries))   // 
#define CREDIT_HEADERS           (6u)                                      // 
#define CREDIT_SPEED             (0.11f)                                   // 

enum eCreditType : coreUint8
{
    CREDIT_TYPE_MENU   = 0u,   // 
    CREDIT_TYPE_NORMAL = 1u,   // 
    CREDIT_TYPE_SECRET = 2u    // 
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_aapcCreditEntry[][3] =
{
    {"Benjamin Schaden",                          "$THANKS",                         NULL},
    {"Chris Soukup",                              "$THANKS",                         NULL},
    {"Christoph Lang",                            "$TESTER",                         NULL},
    {"Claire Deiller (Loc3 Ltd.)",                "$LOCA - Français",                "french.lng"},
    {"Clément Sonntag",                           "$LOCA",                           NULL},
    {"Damian Hiltebrand",                         "$THANKS",                         NULL},
    {"Daniel Rubio (Pinknoise)",                  "$LOCA - Español (Latinoamérica)", "latam.lng"},
    {"David Portisch",                            "$TESTER",                         NULL},
    {"Eric Emanuel (Loc3 Ltd.)",                  "$LOCA - Français",                "french.lng"},
    {"Etel Baglietto (Pinknoise)",                "$LOCA - Español (Latinoamérica)", "latam.lng"},
    {"Henry Buckley (DICO Co., Ltd.)",            "$LOCA - 日本語",                   "japanese.lng"},
    {"Iris Kuppen (Local Heroes Worldwide B.V.)", "$LOCA",                           NULL},
    {"Jingwen Wu",                                "$LOCA - 简体中文",                 "schinese.lng"},
    {"Jochen Leopold",                            "$TESTER",                         NULL},
    {"Johnny Marques (Locsmiths)",                "$LOCA - Português (Brasil)",      "brazilian.lng"},
    {"Lara Cecilia Garau (Pinknoise)",            "$LOCA - Español (Latinoamérica)", "latam.lng"},
    {"Lukas Meindl",                              "$TESTER",                         NULL},
    {"Mariano Saab (Pinknoise)",                  "$LOCA - Español (Latinoamérica)", "latam.lng"},
    {"Martin Filipp",                             "$THANKS",                         NULL},
    {"Martin Mauersics",                          "$DESIGNER, $PROGRAMMER",          NULL},
    {"Michael Hartinger",                         "$THANKS",                         NULL},
    {"Michaela Orfandl",                          "$THANKS",                         NULL},
    {"Miho Miyazaki (DICO Co., Ltd.)",            "$LOCA - 日本語",                   "japanese.lng"},
    {"Murilo Martins (Locsmiths)",                "$LOCA - Português (Brasil)",      "brazilian.lng"},
    {"Neil Doppelmayr",                           "$THANKS",                         NULL},
    {"Nikmik aka Mirko",                          "$THANKS",                         NULL},
    {"Oliver Bisztyga",                           "$THANKS",                         NULL},
    {"Rainer Petermann",                          "$THANKS",                         NULL},
    {"Ricardo Nakaoka (Locsmiths)",               "$LOCA - Português (Brasil)",      "brazilian.lng"},
    {"Robert Glashüttner",                        "$THANKS",                         NULL},
    {"Stefan Karner",                             "$TESTER",                         NULL},
    {"Tetsugen Ryo",                              "$LOCA - 繁體中文",                 "tchinese.lng"},
    {"Tova Bele",                                 "$ADVISER, $TESTER",               NULL},
    {"Vanesa Potenzoni (Pinknoise)",              "$LOCA - Español (Latinoamérica)", "latam.lng"},
    {"Ziya Sarper Ekim (DICO Co., Ltd.)",         "$LOCA - 日本語",                   "japanese.lng"},
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryMusic[] =
{
    "Andrew Sitkov",
    "Isao Yamaguchi",
    "Tim Beek",
    "WOW Sound"
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntrySounds[] =
{
    "0ktober",
    "Albert Fernández",
    "Arto Koivisto",
    "Audible Dread",
    "Audionautics",
    "Casonika",
    "cedarstudios",
    "divaldo11",
    "Eelke",
    "frantramp",
    "FreqMan",
    "FuseHive",
    "Garuda1982",
    "J.BoB",
    "Joel Steudler",
    "LadyCailin",
    "lavik89",
    "monosfera",
    "Porphyr",
    "RHumphries",
    "Rich McBride",
    "Sergenious",
    "sirplus",
    "smcameron",
    "Stormwave Audio",
    "themfish",
    "xdrav"
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryTextures[] =
{
    "Nicolae Berbece",
    "Patrick Hoesly",
    "Rob Tuytel",
    "s58y",
#if !defined(_CORE_SWITCH_)
    "textures.com"
#endif
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryModels[] =
{
    "AAAnimators",
    "Creative Trio",
    "Jerome Angeles",
    "resprog",
    "Ruslan Malovsky",
    "Sebastian Irmer",
    "ZugZug"
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryFonts[] =
{
    "Do-Font",
    "Fonticons",
    "Google"
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryLibraries[] =
{
    "angle, glew, imgui, libc++, OpenAL_soft",
    "SDL2, SDL2_image, SDL2_ttf",
    "stb_dxt, stb_sprintf, stb_vorbis",
    "xxhash, zstd"
};


// ****************************************************************
// credit roll class
class cCreditRoll final : public cGuiObject
{
private:
    coreViewBox m_ViewBox;                                   // 

    cGuiObject m_GameLogo;                                   // 
    cGuiObject m_GameLogoDemo;
    cGuiObject m_GameLogoKana;                               // 

    cGuiLabel m_aName       [CREDIT_ENTRIES];                // 
    cGuiLabel m_aDescription[CREDIT_ENTRIES];                // 

    cGuiLabel m_aSupportText[2];                             // 

    cGuiLabel m_aOtherHeader   [CREDIT_HEADERS];             // 
    cGuiLabel m_aOtherMusic    [CREDIT_ENTRIES_MUSIC];       // 
    cGuiLabel m_aOtherSounds   [CREDIT_ENTRIES_SOUNDS];      // 
    cGuiLabel m_aOtherTextures [CREDIT_ENTRIES_TEXTURES];    // 
    cGuiLabel m_aOtherModels   [CREDIT_ENTRIES_MODELS];      // 
    cGuiLabel m_aOtherFonts    [CREDIT_ENTRIES_FONTS];       // 
    cGuiLabel m_aOtherLibraries[CREDIT_ENTRIES_LIBRARIES];   // 

    cGuiLabel m_ThankYouText;                                // 
    cGuiLabel m_EndText;                                     // 

    coreFlow  m_fOffset;                                     // 
    coreFloat m_fMaxOffset;                                  // 

    eCreditType m_eType;                                     // 
    coreBool    m_bFinished;                                 // 


public:
    cCreditRoll()noexcept;

    DISABLE_COPY(cCreditRoll)

    // render and move the credit roll
    void Render()final;
    void Move  ()final;

    // 
    void Start(const eCreditType eType);
    void End();

    // 
    inline const eCreditType& GetType    ()const {return m_eType;}
    inline const coreBool&    GetFinished()const {return m_bFinished;}
};


#endif // _P1_GUARD_CREDITROLL_H_