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

// TODO 5: rotate entries and make them rotatable
// TODO 3: add label pool

// https://old.reddit.com/r/shmups/comments/10dd8qd/i_would_like_to_show_you_the_game_ive_been/


// ****************************************************************
// credit roll definitions
#define CREDIT_ENTRIES           (ARRAY_SIZE(g_aapcCreditEntry))           // 
#define CREDIT_ENTRIES_MUSIC     (ARRAY_SIZE(g_apcCreditEntryMusic))       // 
#define CREDIT_ENTRIES_SOUNDS    (ARRAY_SIZE(g_apcCreditEntrySounds))      // 
#define CREDIT_ENTRIES_TEXTURES  (ARRAY_SIZE(g_apcCreditEntryTextures))    // 
#define CREDIT_ENTRIES_MODELS    (ARRAY_SIZE(g_apcCreditEntryModels))      // 
#define CREDIT_ENTRIES_FONTS     (ARRAY_SIZE(g_apcCreditEntryFonts))       // 
#define CREDIT_ENTRIES_LIBRARIES (ARRAY_SIZE(g_apcCreditEntryLibraries))   // 
#define CREDIT_ENTRIES_THANKS    (ARRAY_SIZE(g_apcCreditEntryThanks))      // 
#define CREDIT_HEADERS           (6u)//(7u)                                      // 

enum eCreditType : coreUint8
{
    CREDIT_TYPE_MENU   = 0u,   // 
    CREDIT_TYPE_NORMAL = 1u,   // 
    CREDIT_TYPE_SECRET = 2u    // 
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_aapcCreditEntry[][2] =
{
    {"Christoph Lang",   "Tester"},
    {"David Portisch",   "Tester"},
    {"Jochen Leopold",   "Tester"},
    {"Lukas Meindl",     "Tester"},
    {"Martin Mauersics", "Game Designer, Programmer"},
    {"Stefan Karner",    "Tester"},
    {"Tova Sarah Bele",  "Adviser, Tester"}
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryMusic[] =
{
    "Andrew Sitkov",
    "Isao Yamaguchi",
    "Tim Beek"
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntrySounds[] =
{
    "Audible Dread",
    "FuseHive",
    "J.BoB",
    "Joel Steudler",
    "Rich McBride",
    "Stormwave Audio"
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryTextures[] =
{
    "Nicolae Berbece",
    "Patrick Hoesly",
    "Rob Tuytel",
    "s58y",
    "textures.com"
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
    "forsyth_too",
    "glew",
    "imgui",
    "OpenAL_soft",
    "SDL2",
    "SDL2_image",
    "SDL2_ttf",
    "stb_dxt",
    "stb_sprintf",
    "stb_vorbis",
    "steamworks",
    "xxhash",
    "zstd"
};

// 
alignas(ALIGNMENT_CACHE) static constexpr const coreChar* g_apcCreditEntryThanks[] =
{
        ""
    //"AgitatedBrilliant",   // r/shmups: comment
    //"cy13erpunk",          // r/shmups: feedback, comment
    //"FaceTimePolice",      // r/shmups: feedback, comment
    //"JewsEatFruit",        // r/shmups: comment, marking
    //"leanderish",          // r/shmups: feedback, comment
    //"Sentmoraap",          // r/shmups: comment
    //"TheCaptainGhost"      // r/shmups: feedback, comment
};


// ****************************************************************
// credit roll class
class cCreditRoll final : public cGuiObject
{
private:
    coreViewBox m_ViewBox;                                   // 

    cGuiObject m_GameLogo;                                   // 

    cGuiLabel m_aName       [CREDIT_ENTRIES];                // 
    cGuiLabel m_aDescription[CREDIT_ENTRIES];                // 

    cGuiLabel m_aOtherHeader   [CREDIT_HEADERS];             // 
    cGuiLabel m_aOtherMusic    [CREDIT_ENTRIES_MUSIC];       // 
    cGuiLabel m_aOtherSounds   [CREDIT_ENTRIES_SOUNDS];      // 
    cGuiLabel m_aOtherTextures [CREDIT_ENTRIES_TEXTURES];    // 
    cGuiLabel m_aOtherModels   [CREDIT_ENTRIES_MODELS];      // 
    cGuiLabel m_aOtherFonts    [CREDIT_ENTRIES_FONTS];       // 
    cGuiLabel m_aOtherLibraries[CREDIT_ENTRIES_LIBRARIES];   // 
    cGuiLabel m_aOtherThanks   [CREDIT_ENTRIES_THANKS];      // 

    cGuiLabel m_ThankYouText;                                // 

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

    // 
    inline const eCreditType& GetType    ()const {return m_eType;}
    inline const coreBool&    GetFinished()const {return m_bFinished;}
};


#endif // _P1_GUARD_CREDITROLL_H_