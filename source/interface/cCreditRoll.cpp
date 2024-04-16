///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cCreditRoll::cCreditRoll()noexcept
: m_fOffset    (0.0f)
, m_fMaxOffset (0.0f)
, m_bFinished  (false)
{
    constexpr coreFloat fWait = 1.1f;

    // 
    m_ViewBox.SetPosition(coreVector2(0.0f,0.0f));
    m_ViewBox.SetSize    (coreVector2(1.0f,1.0f));

    // 
    m_GameLogo.DefineTexture(0u, "game_logo.png");
    m_GameLogo.DefineProgram("default_2d_program");
    m_GameLogo.SetPosition  (coreVector2(0.0f,-0.7f));
    m_GameLogo.SetSize      (coreVector2(1.0f,0.25f) * 0.7f);
    m_GameLogo.SetColor3    (coreVector3(1.0f,1.0f,1.0f) * 1.05f);
    m_GameLogo.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    m_ViewBox.BindObject(&m_GameLogo);

    // 
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i)
    {
        const coreFloat fHeight = I_TO_F(i) * -0.15f - fWait + 0.05f;

        m_aName[i].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aName[i].SetColor3  (COLOR_MENU_INSIDE);
        m_aName[i].SetAlpha   (MENU_INSIDE_ALPHA);
        m_aName[i].SetEnabled (CORE_OBJECT_ENABLE_NOTHING);
        m_aName[i].SetText    (g_aapcCreditEntry[i][0]);

        m_ViewBox.BindObject(&m_aName[i]);
    }

    // 
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i)
    {
        const coreFloat fHeight = I_TO_F(i) * -0.15f - fWait;

        m_aDescription[i].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        m_aDescription[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aDescription[i].SetAlpha   (MENU_INSIDE_ALPHA);
        m_aDescription[i].SetEnabled (CORE_OBJECT_ENABLE_NOTHING);
        m_aDescription[i].SetText    (g_aapcCreditEntry[i][1]);

        m_ViewBox.BindObject(&m_aDescription[i]);
    }

    coreUintW iOffset = CREDIT_ENTRIES * 3u;

    // 
    const auto nInitFunc = [&](cGuiLabel* OUTPUT pHeader, cGuiLabel* OUTPUT pEntry, const coreHashString& sKey, const coreChar* const* ppcText, const coreUintW iNum)
    {
        iOffset += 3u;
        const coreFloat fHeight = I_TO_F(iOffset) * -0.05f - fWait;

        pHeader->Construct      (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
        pHeader->SetPosition    (coreVector2(0.0f,fHeight));
        pHeader->SetAlpha       (MENU_INSIDE_ALPHA);
        pHeader->SetEnabled     (CORE_OBJECT_ENABLE_NOTHING);
        pHeader->SetTextLanguage(sKey);

        m_ViewBox.BindObject(pHeader);

        // 
        for(coreUintW i = 0u; i < iNum; ++i)
        {
            iOffset += 1u;
            const coreFloat fHeight2 = I_TO_F(iOffset) * -0.05f - fWait;

            pEntry[i].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
            pEntry[i].SetPosition(coreVector2(0.0f, fHeight2));
            pEntry[i].SetAlpha   (MENU_INSIDE_ALPHA);
            pEntry[i].SetEnabled (CORE_OBJECT_ENABLE_NOTHING);
            pEntry[i].SetText    (ppcText[i]);

            m_ViewBox.BindObject(&pEntry[i]);
        }
    };
    nInitFunc(&m_aOtherHeader[0], m_aOtherMusic,     "CREDITS_MUSIC",     g_apcCreditEntryMusic,     CREDIT_ENTRIES_MUSIC);
    nInitFunc(&m_aOtherHeader[1], m_aOtherSounds,    "CREDITS_SOUNDS",    g_apcCreditEntrySounds,    CREDIT_ENTRIES_SOUNDS);
    nInitFunc(&m_aOtherHeader[2], m_aOtherTextures,  "CREDITS_TEXTURES",  g_apcCreditEntryTextures,  CREDIT_ENTRIES_TEXTURES);
    nInitFunc(&m_aOtherHeader[3], m_aOtherModels,    "CREDITS_MODELS",    g_apcCreditEntryModels,    CREDIT_ENTRIES_MODELS);
    nInitFunc(&m_aOtherHeader[4], m_aOtherFonts,     "CREDITS_FONTS",     g_apcCreditEntryFonts,     CREDIT_ENTRIES_FONTS);
    nInitFunc(&m_aOtherHeader[5], m_aOtherLibraries, "CREDITS_LIBRARIES", g_apcCreditEntryLibraries, CREDIT_ENTRIES_LIBRARIES);
    //nInitFunc(&m_aOtherHeader[6], m_aOtherThanks,    "CREDITS_THANKS",    g_apcCreditEntryThanks,    CREDIT_ENTRIES_THANKS);

    // 
    m_fMaxOffset = I_TO_F(iOffset) * 0.05f + fWait + 0.6f;
}


// ****************************************************************
// render the credit roll
void cCreditRoll::Render()
{
    // 
    m_ViewBox.Render();
}


// ****************************************************************
// move the credit roll
void cCreditRoll::Move()
{
    // 
    const coreVector3 vColor = g_pMenu->GetHighlightColor();
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i) m_aDescription[i].SetColor3(vColor);
    for(coreUintW i = 0u; i < CREDIT_HEADERS; ++i) m_aOtherHeader[i].SetColor3(vColor);

    // 
    const coreFloat fSpeed = (Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_HOLD) || HAS_BIT(g_TotalInput.iActionHold, PLAYER_ACTION_SHOOT(0u, 0u))) ? 5.0f : 1.0f;
    m_fOffset.Update(0.1f * fSpeed);

    // 
    m_bFinished = (m_fOffset >= m_fMaxOffset);
    if(m_bFinished) m_fOffset = 0.0f;

    // 
    m_ViewBox.SetOffset(coreVector2(0.0f, m_fOffset));
    m_ViewBox.Move();

    // 
    const auto nCullFunc = [](coreObject2D* OUTPUT pObject, const coreFloat fRange)
    {
        pObject->SetEnabled((ABS(pObject->GetPosition().y) < fRange) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    };
    const auto nCullListFunc = [&](cGuiLabel* OUTPUT pLabel, const coreFloat fRange, const coreUintW iNum)
    {
        for(coreUintW i = 0u; i < iNum; ++i) nCullFunc(&pLabel[i], fRange);
    };
    nCullFunc    (&m_GameLogo,       1.3f);
    nCullListFunc(m_aName,           1.1f, CREDIT_ENTRIES);
    nCullListFunc(m_aDescription,    1.1f, CREDIT_ENTRIES);
    nCullListFunc(m_aOtherHeader,    1.1f, CREDIT_HEADERS);
    nCullListFunc(m_aOtherMusic,     1.1f, CREDIT_ENTRIES_MUSIC);
    nCullListFunc(m_aOtherSounds,    1.1f, CREDIT_ENTRIES_SOUNDS);
    nCullListFunc(m_aOtherTextures,  1.1f, CREDIT_ENTRIES_TEXTURES);
    nCullListFunc(m_aOtherModels,    1.1f, CREDIT_ENTRIES_MODELS);
    nCullListFunc(m_aOtherFonts,     1.1f, CREDIT_ENTRIES_FONTS);
    nCullListFunc(m_aOtherLibraries, 1.1f, CREDIT_ENTRIES_LIBRARIES);
    //nCullListFunc(m_aOtherThanks,    1.1f, CREDIT_ENTRIES_THANKS);
}


// ****************************************************************
// 
void cCreditRoll::Start()
{
    // 
    const coreBool bHorizontal = IsHorizontal(g_vHudDirection);

    // 
    const auto nSetRectifyFunc = [&](cGuiLabel* OUTPUT pLabel, const coreUintW iNum)
    {
        for(coreUintW i = 0u; i < iNum; ++i)
        {
            pLabel[i].SetRectifyX(!bHorizontal);
            pLabel[i].SetRectifyY( bHorizontal);
        }
    };
    nSetRectifyFunc(m_aName,           CREDIT_ENTRIES);
    nSetRectifyFunc(m_aDescription,    CREDIT_ENTRIES);
    nSetRectifyFunc(m_aOtherHeader,    CREDIT_HEADERS);
    nSetRectifyFunc(m_aOtherMusic,     CREDIT_ENTRIES_MUSIC);
    nSetRectifyFunc(m_aOtherSounds,    CREDIT_ENTRIES_SOUNDS);
    nSetRectifyFunc(m_aOtherTextures,  CREDIT_ENTRIES_TEXTURES);
    nSetRectifyFunc(m_aOtherModels,    CREDIT_ENTRIES_MODELS);
    nSetRectifyFunc(m_aOtherFonts,     CREDIT_ENTRIES_FONTS);
    nSetRectifyFunc(m_aOtherLibraries, CREDIT_ENTRIES_LIBRARIES);
    //nSetRectifyFunc(m_aOtherThanks,    CREDIT_ENTRIES_THANKS);

    // 
    m_fOffset   = 0.0f;
    m_bFinished = false;

    // 
    m_ViewBox.SetOffset(coreVector2(0.0f,0.0f));
    m_ViewBox.Move();
}