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
, m_eType      (CREDIT_TYPE_MENU)
, m_bFinished  (true)
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
    m_GameLogo.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    m_ViewBox.BindObject(&m_GameLogo);

    // 
    m_GameLogoDemo.DefineTexture(0u, "game_logo_demo.png");
    m_GameLogoDemo.DefineProgram("default_2d_program");
    m_GameLogoDemo.SetPosition  (m_GameLogo.GetPosition() + coreVector2(0.0f,-0.1f) * (0.7f/0.8f));
    m_GameLogoDemo.SetSize      (coreVector2(1.0f,0.25f) * 0.3f * (0.7f/0.8f));
    m_GameLogoDemo.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_GameLogoKana.DefineTexture(0u, "game_logo_kana.png");
    m_GameLogoKana.DefineProgram("default_2d_program");
    m_GameLogoKana.SetPosition  (m_GameLogo.GetPosition() + coreVector2(0.0f,-0.1f) * (0.7f/0.8f));
    m_GameLogoKana.SetSize      (coreVector2(2.0f,0.25f) * 0.3f * (0.7f/0.8f));
    m_GameLogoKana.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    m_ViewBox.BindObject(g_bDemoVersion ? &m_GameLogoDemo : &m_GameLogoKana);

    // 
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i)
    {
        const coreFloat fHeight = I_TO_F(i) * -0.15f - fWait + 0.05f;

        m_aName[i].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_BIG);
        m_aName[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aName[i].SetColor3  (COLOR_MENU_INSIDE);
        m_aName[i].SetAlpha   (MENU_INSIDE_ALPHA);
        m_aName[i].SetEnabled (CORE_OBJECT_ENABLE_NOTHING);
        m_aName[i].SetText    (g_aapcCreditEntry[i][0]);

        m_ViewBox.BindObject(&m_aName[i]);
    }

    // 
    const coreMap<coreString, coreString>& asLanguageList = cMenu::GetLanguageList();

    // 
    coreMapStr<coreString> asFont;
    asFont.reserve(asLanguageList.size());

    // 
    FOR_EACH(it, asLanguageList)
    {
        // 
        coreString sFont;
        if(!coreLanguage::FindString(it->c_str(), "FONT", &sFont)) sFont = MENU_FONT_STANDARD;

        asFont.emplace_bs(coreData::StrFilename(it->c_str()), coreData::StrFilename(sFont.c_str()));
    }

    // 
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i)
    {
        const coreFloat fHeight = I_TO_F(i) * -0.15f - fWait;

        const coreChar* pcFont = asFont.count_bs(g_aapcCreditEntry[i][2]) ? asFont.at_bs(g_aapcCreditEntry[i][2]).c_str() : NULL;

        m_aDescription[i].Construct  (pcFont ? pcFont : MENU_FONT_DYNAMIC_3, MENU_OUTLINE_BIG);
        m_aDescription[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aDescription[i].SetAlpha   (MENU_INSIDE_ALPHA);
        m_aDescription[i].SetEnabled (CORE_OBJECT_ENABLE_NOTHING);

        m_ViewBox.BindObject(&m_aDescription[i]);
    }

    coreUintW iOffset = CREDIT_ENTRIES * 3u;

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aSupportText); ++i)
    {
        iOffset += 1u;
        const coreFloat fHeight = I_TO_F(iOffset) * -0.05f - fWait;

        m_aSupportText[i].Construct  (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_BIG);
        m_aSupportText[i].SetPosition(coreVector2(0.0f, fHeight));
        m_aSupportText[i].SetColor3  (COLOR_MENU_INSIDE);
        m_aSupportText[i].SetAlpha   (MENU_INSIDE_ALPHA);
        m_aSupportText[i].SetEnabled (CORE_OBJECT_ENABLE_NOTHING);

        m_ViewBox.BindObject(&m_aSupportText[i]);
    }

    // 
    iOffset += 1u;
    const auto nInitFunc = [&](cGuiLabel* OUTPUT pHeader, cGuiLabel* OUTPUT pEntry, const coreHashString& sKey, const coreChar* const* ppcText, const coreUintW iNum)
    {
        iOffset += 3u;
        const coreFloat fHeight = I_TO_F(iOffset) * -0.05f - fWait;

        pHeader->Construct      (MENU_FONT_DYNAMIC_3, MENU_OUTLINE_BIG);
        pHeader->SetPosition    (coreVector2(0.0f, fHeight));
        pHeader->SetAlpha       (MENU_INSIDE_ALPHA);
        pHeader->SetEnabled     (CORE_OBJECT_ENABLE_NOTHING);
        pHeader->SetTextLanguage(sKey);

        m_ViewBox.BindObject(pHeader);

        // 
        for(coreUintW i = 0u; i < iNum; ++i)
        {
            iOffset += 1u;
            const coreFloat fHeight2 = I_TO_F(iOffset) * -0.05f - fWait;

            pEntry[i].Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_BIG);
            pEntry[i].SetPosition(coreVector2(0.0f, fHeight2));
            pEntry[i].SetColor3  (COLOR_MENU_INSIDE);
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

    // 
    m_ThankYouText.Construct      (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_BIG);
    m_ThankYouText.SetPosition    (coreVector2(0.0f,0.0f));
    m_ThankYouText.SetColor3      (COLOR_MENU_INSIDE);
    m_ThankYouText.SetAlpha       (0.0f);
    m_ThankYouText.SetEnabled     (CORE_OBJECT_ENABLE_NOTHING);
    m_ThankYouText.SetTextLanguage("THANK_YOU");

    // 
    m_EndText.Construct  (MENU_FONT_STANDARD_3, MENU_OUTLINE_BIG);
    m_EndText.SetPosition(coreVector2(0.0f,0.0f));
    m_EndText.SetColor3  (COLOR_MENU_INSIDE);
    m_EndText.SetAlpha   (0.0f);
    m_EndText.SetEnabled (CORE_OBJECT_ENABLE_NOTHING);
    m_EndText.SetText    ("ENDE");

    // 
    m_fMaxOffset = I_TO_F(iOffset) * 0.05f + fWait + 0.6f;
}


// ****************************************************************
// render the credit roll
void cCreditRoll::Render()
{
    // 
    m_ViewBox.Render();

    // 
    m_ThankYouText.Render();
    m_EndText     .Render();
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
    const coreFloat fSpeed = (Core::Input->GetKeyboardButton(CORE_INPUT_KEY(RETURN),                            CORE_INPUT_HOLD) ||
                              Core::Input->GetKeyboardButton(CORE_INPUT_KEY(SPACE),                             CORE_INPUT_HOLD) ||
                              Core::Input->GetMouseButton   (CORE_INPUT_LEFT,                                   CORE_INPUT_HOLD) ||
                              Core::Input->GetJoystickButton(CORE_INPUT_JOYSTICK_ANY, CORE_INPUT_BUTTON_ACCEPT, CORE_INPUT_HOLD) ||
                              (!g_TotalInput.vMove.IsNull() && SameDirection90(g_TotalInput.vMove, coreVector2(0.0f,-1.0f)))) ? ((m_fOffset < m_fMaxOffset) ? 6.0f : 2.0f) : 1.0f;
    m_fOffset.Update(CREDIT_SPEED * fSpeed);

    // 
    m_bFinished = (m_fOffset >= m_fMaxOffset) && (m_eType == CREDIT_TYPE_MENU);
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
    nCullFunc    (&m_GameLogoDemo,   1.3f);
    nCullFunc    (&m_GameLogoKana,   1.3f);
    nCullListFunc(m_aName,           1.1f, CREDIT_ENTRIES);
    nCullListFunc(m_aDescription,    1.1f, CREDIT_ENTRIES);
    nCullListFunc(m_aSupportText,    1.1f, ARRAY_SIZE(m_aSupportText));
    nCullListFunc(m_aOtherHeader,    1.1f, CREDIT_HEADERS);
    nCullListFunc(m_aOtherMusic,     1.1f, CREDIT_ENTRIES_MUSIC);
    nCullListFunc(m_aOtherSounds,    1.1f, CREDIT_ENTRIES_SOUNDS);
    nCullListFunc(m_aOtherTextures,  1.1f, CREDIT_ENTRIES_TEXTURES);
    nCullListFunc(m_aOtherModels,    1.1f, CREDIT_ENTRIES_MODELS);
    nCullListFunc(m_aOtherFonts,     1.1f, CREDIT_ENTRIES_FONTS);
    nCullListFunc(m_aOtherLibraries, 1.1f, CREDIT_ENTRIES_LIBRARIES);

    
    if(m_eType != CREDIT_TYPE_MENU)
    {
        ASSERT(STATIC_ISVALID(g_pGame))

        const coreFloat fFactor1 = m_fOffset * RCP(m_fMaxOffset);
        const coreFloat fFactor2 = m_fOffset - m_fMaxOffset;

        if(fFactor2 >= 0.0f)
        {
            const coreFloat fAlpha = STEPH3(0.0f, 0.15f, fFactor2 - 0.0f) - STEPH3(0.0f, 0.15f, fFactor2 - 0.5f);

            m_ThankYouText.SetAlpha  (fAlpha * MENU_INSIDE_ALPHA);
            m_ThankYouText.SetEnabled(fAlpha ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        }

        if(m_eType == CREDIT_TYPE_NORMAL)
        {
            constexpr coreInt32 aiBackground[] =
            {
                cNoBackground     ::ID,
                cCloudBackground  ::ID,
                cGrassBackground  ::ID,
                cSeaBackground    ::ID,
                cDesertBackground ::ID,
                cSpaceBackground  ::ID,
                cVolcanoBackground::ID,
                cSnowBackground   ::ID,
                cMossBackground   ::ID,
                cDarkBackground   ::ID
            };

            const coreUintW iNewIndex = MIN(F_TO_UI(fFactor1 * I_TO_F(ARRAY_SIZE(aiBackground))), ARRAY_SIZE(aiBackground) - 1u);
            const coreInt32 iNewID    = aiBackground[iNewIndex];

            if(iNewID != g_pEnvironment->GetBackground()->GetID())
            {
                g_pEnvironment->ChangeBackground(iNewID, iNewIndex ? ENVIRONMENT_MIX_WIPE : ENVIRONMENT_MIX_FADE, 100.0f, coreVector2(-1.0f,0.0f));
                g_pEnvironment->SetTargetSpeedNow(1.0f);
            }
            g_pEnvironment->UpdateTransitionSpeed(0.5f * fSpeed);

            g_pPostProcessing->SetSaturationAll(0.0f);

            if(fFactor2 >= 0.0f)
            {
                const coreFloat fAlpha = STEPH3(0.0f, 0.15f, fFactor2 - 0.0f) - STEPH3(0.0f, 0.15f, fFactor2 - 0.5f);

                g_pGame->GetInterface()->ShowFragment(INTERFACE_FRAGMENT_TYPE_SHOW);
                g_pGame->GetInterface()->SetAlphaFragment(fAlpha);
            }

            if(fFactor2 >= 0.75f)
            {
                g_pGame->GetInterface()->ShowFragment(INTERFACE_FRAGMENT_TYPE_HIDE);

                m_bFinished = true;
            }
        }
        else if(m_eType == CREDIT_TYPE_SECRET)
        {
            if(fFactor2 >= 0.75f)
            {
                const coreFloat fAlpha = STEPH3(0.0f, 0.15f, fFactor2 - 0.75f) - STEPH3(0.0f, 0.15f, fFactor2 - 1.75f);

                m_EndText.SetAlpha  (fAlpha * MENU_INSIDE_ALPHA);
                m_EndText.SetEnabled(fAlpha ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            }

            if(fFactor2 >= 2.0f)
            {
                m_bFinished = true;
            }
        }
    }
}


// ****************************************************************
// 
void cCreditRoll::Start(const eCreditType eType)
{
    // 
    const auto nSetRectifyFunc = [&](cGuiLabel* OUTPUT pLabel)
    {
        pLabel->SetRectifyY(false);
    };
    const auto nSetRectifyListFunc = [&](cGuiLabel* OUTPUT pLabel, const coreUintW iNum)
    {
        for(coreUintW i = 0u; i < iNum; ++i) nSetRectifyFunc(&pLabel[i]);
    };
    nSetRectifyListFunc(m_aName,           CREDIT_ENTRIES);
    nSetRectifyListFunc(m_aDescription,    CREDIT_ENTRIES);
    nSetRectifyListFunc(m_aSupportText,    ARRAY_SIZE(m_aSupportText));
    nSetRectifyListFunc(m_aOtherHeader,    CREDIT_HEADERS);
    nSetRectifyListFunc(m_aOtherMusic,     CREDIT_ENTRIES_MUSIC);
    nSetRectifyListFunc(m_aOtherSounds,    CREDIT_ENTRIES_SOUNDS);
    nSetRectifyListFunc(m_aOtherTextures,  CREDIT_ENTRIES_TEXTURES);
    nSetRectifyListFunc(m_aOtherModels,    CREDIT_ENTRIES_MODELS);
    nSetRectifyListFunc(m_aOtherFonts,     CREDIT_ENTRIES_FONTS);
    nSetRectifyListFunc(m_aOtherLibraries, CREDIT_ENTRIES_LIBRARIES);

    // 
    const coreChar* const aacMap[][2] =
    {
        {"$DESIGNER",   Core::Language->GetString("CREDITS_ROLE_DESIGNER")},
        {"$PROGRAMMER", Core::Language->GetString("CREDITS_ROLE_PROGRAMMER")},
        {"$TESTER",     Core::Language->GetString("CREDITS_ROLE_TESTER")},
        {"$ADVISER",    Core::Language->GetString("CREDITS_ROLE_ADVISER")},
        {"$LOCA",       Core::Language->GetString("CREDITS_ROLE_LOCA")},
        {"$THANKS",     Core::Language->GetString("CREDITS_ROLE_THANKS")}
    };

    // 
    coreWorkString sWork;
    for(coreUintW i = 0u; i < CREDIT_ENTRIES; ++i)
    {
        sWork.assign(g_aapcCreditEntry[i][1]);

        for(coreUintW j = 0u; j < ARRAY_SIZE(aacMap); ++j)
        {
            sWork.replace(aacMap[j][0], aacMap[j][1]);
        }

        m_aDescription[i].SetText(sWork.c_str());
    }

    // 
    const coreChar* pcString = Core::Language->GetString("THANK_YOU_SUPPORT");
    const coreChar* pcBreak  = std::strchr(pcString, '#');

    if(pcBreak)
    {
        m_aSupportText[0].SetText(pcString, pcBreak - pcString);
        m_aSupportText[1].SetText(pcBreak + 1u);
    }
    else
    {
        m_aSupportText[0].SetText(pcString);
        m_aSupportText[1].SetText("");
    }

    // 
    m_fOffset   = 0.0f;
    m_eType     = eType;
    m_bFinished = false;

    // 
    m_ViewBox.SetOffset(coreVector2(0.0f,0.0f));
    m_ViewBox.Move();
}


// ****************************************************************
// 
void cCreditRoll::End()
{
}