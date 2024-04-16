///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cConfigMenu::cConfigMenu()noexcept
: coreMenu (SURFACE_CONFIG_MAX, SURFACE_CONFIG_VIDEO)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetSize      (coreVector2(0.8f,0.7f));

    m_VideoTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_VideoTab.DefineProgram("menu_border_program");
    m_VideoTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.115f,-0.0115f));
    m_VideoTab.SetSize      (coreVector2(0.17f,0.07f));
    m_VideoTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_VideoTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_VideoTab.SetTexOffset (m_VideoTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.115f,0.0115f));
    m_VideoTab.GetCaption()->SetTextLanguage("CONFIG_VIDEO");

    m_AudioTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_AudioTab.DefineProgram("menu_border_program");
    m_AudioTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(1.0f/3.0f,1.0f));
    m_AudioTab.SetSize      (m_VideoTab.GetSize());
    m_AudioTab.SetAlignment (m_VideoTab.GetAlignment());
    m_AudioTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_AudioTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_AudioTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0));
    m_AudioTab.GetCaption()->SetTextLanguage("CONFIG_AUDIO");

    m_InputTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_InputTab.DefineProgram("menu_border_program");
    m_InputTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(-1.0f/3.0f,1.0f));
    m_InputTab.SetSize      (m_VideoTab.GetSize());
    m_InputTab.SetAlignment (m_VideoTab.GetAlignment());
    m_InputTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_InputTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_InputTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0));
    m_InputTab.GetCaption()->SetTextLanguage("CONFIG_INPUT");

    m_GameTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_GameTab.DefineProgram("menu_border_program");
    m_GameTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_GameTab.SetSize      (m_VideoTab.GetSize());
    m_GameTab.SetAlignment (m_VideoTab.GetAlignment());
    m_GameTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_GameTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_GameTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0));
    m_GameTab.GetCaption()->SetTextLanguage("CONFIG_GAME");

    m_SaveButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_SaveButton.DefineProgram("menu_border_program");
    m_SaveButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_SaveButton.SetSize      (coreVector2(0.3f,0.07f));
    m_SaveButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_SaveButton.SetOverride  (-1);   // # used in LoadValues()
    m_SaveButton.GetCaption()->SetTextLanguage("SAVE");

    m_DiscardButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_DiscardButton.DefineProgram("menu_border_program");
    m_DiscardButton.SetPosition  (m_SaveButton.GetPosition() + coreVector2(m_SaveButton.GetSize().x + 0.02f, 0.0f));
    m_DiscardButton.SetSize      (m_SaveButton.GetSize());
    m_DiscardButton.SetAlignment (m_SaveButton.GetAlignment());
    m_DiscardButton.SetOverride  (-1);
    m_DiscardButton.GetCaption()->SetTextLanguage("DISCARD");

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_SaveButton.GetSize().y);
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    // create configuration entries
    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        if(i == ENTRY_VIDEO || i == ENTRY_AUDIO || i == ENTRY_INPUT) iOffset = 0u;
        if(i == ENTRY_VIDEO_ANTIALIASING)  ++iOffset;   // # new paragraph
        if(i == ENTRY_VIDEO_RENDERQUALITY) ++iOffset;
        if(i == ENTRY_AUDIO_MUSICVOLUME)   ++iOffset;
        if(i == ENTRY_AUDIO_AMBIENTSOUND)  ++iOffset;
        if(i == ENTRY_INPUT_MOVEUP)        ++iOffset;
        if(i == ENTRY_INPUT_ACTION1)       ++iOffset;
        if(i == ENTRY_GAME_GAMEROTATION)   ++iOffset;
        if(i == ENTRY_GAME_HUDROTATION)    ++iOffset;

        m_aLabel[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aLabel[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.025f*I_TO_F(iOffset)));
        m_aLabel[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aLabel[i].SetColor3   (COLOR_MENU_WHITE);

        iOffset += 2u;
    }

    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        m_aLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aLabel[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArrow); ++i)
    {
        m_aArrow[i].Construct  (MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aArrow[i].SetPosition(m_aLabel[ENTRY_INPUT_MOVEUP + i].GetPosition() + coreVector2(0.21f,0.0f));
        m_aArrow[i].SetColor3  (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    }
    m_aArrow[0].SetText(ICON_ARROW_UP);
    m_aArrow[1].SetText(ICON_ARROW_LEFT);
    m_aArrow[2].SetText(ICON_ARROW_DOWN);
    m_aArrow[3].SetText(ICON_ARROW_RIGHT);
    m_aArrow[6].SetText(ICON_UNDO_ALT);
    m_aArrow[7].SetText(ICON_REDO_ALT);

    #define __SET_OPTION(x,n,s)                                                  \
    {                                                                            \
        coreLabel& oLabel = m_aLabel[ENTRY_ ## n];                               \
        oLabel.SetTextLanguage("CONFIG_" #n);                                    \
                                                                                 \
        x.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL); \
        x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition());        \
        x.SetSize     (coreVector2(   (s),0.03f));                               \
        x.SetAlignment(coreVector2(-1.00f,0.00f));                               \
        x.GetCaption()->SetColor3     (COLOR_MENU_WHITE);                        \
        x.GetCaption()->ChangeLanguage(Core::Language);                          \
    }
    {
        __SET_OPTION(m_Monitor,       VIDEO_MONITOR,       0.26f)
        __SET_OPTION(m_Resolution,    VIDEO_RESOLUTION,    0.26f)
        __SET_OPTION(m_DisplayMode,   VIDEO_DISPLAYMODE,   0.26f)
        __SET_OPTION(m_AntiAliasing,  VIDEO_ANTIALIASING,  0.26f)
        __SET_OPTION(m_TextureFilter, VIDEO_TEXTUREFILTER, 0.26f)
        __SET_OPTION(m_RenderQuality, VIDEO_RENDERQUALITY, 0.26f)
        __SET_OPTION(m_ShadowQuality, VIDEO_SHADOWQUALITY, 0.26f)
        __SET_OPTION(m_OverallVolume, AUDIO_OVERALLVOLUME, 0.26f)
        __SET_OPTION(m_MusicVolume,   AUDIO_MUSICVOLUME,   0.26f)
        __SET_OPTION(m_EffectVolume,  AUDIO_EFFECTVOLUME,  0.26f)
        __SET_OPTION(m_AmbientSound,  AUDIO_AMBIENTSOUND,  0.26f)
        __SET_OPTION(m_Language,      GAME_LANGUAGE,       0.26f)
        __SET_OPTION(m_TextSize,      GAME_TEXTSIZE,       0.26f)
        __SET_OPTION(m_GameRotation,  GAME_GAMEROTATION,   0.26f)
        __SET_OPTION(m_GameScale,     GAME_GAMESCALE,      0.26f)
        __SET_OPTION(m_HudRotation,   GAME_HUDROTATION,    0.26f)
        __SET_OPTION(m_HudScale,      GAME_HUDSCALE,       0.26f)
        __SET_OPTION(m_HudType,       GAME_HUDTYPE,        0.26f)

        m_Monitor      .SetAutomatic(0.0f);   // # because of realtime-update
        m_ShadowQuality.SetAutomatic(0.0f);
        m_Language     .SetAutomatic(0.0f);
        m_TextSize     .SetAutomatic(0.0f);
        m_GameRotation .SetAutomatic(0.0f);
        m_GameScale    .SetAutomatic(0.0f);
        m_HudRotation  .SetAutomatic(0.0f);
        m_HudScale     .SetAutomatic(0.0f);
        m_HudType      .SetAutomatic(0.0f);
        m_AmbientSound .SetEndless(true);
        m_Language     .SetEndless(true);
        m_GameRotation .SetEndless(true);
        m_HudRotation  .SetEndless(true);
        m_HudType      .SetEndless(true);
    }
    #undef __SET_OPTION

    #define __SET_INPUT(x,n,s)                                                                  \
    {                                                                                           \
        coreLabel& oLabel = m_aLabel[ENTRY_ ## n];                                              \
        oLabel.SetTextLanguage("CONFIG_" #n);                                                   \
                                                                                                \
        m_aInput[i].x.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);    \
        m_aInput[i].x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition() - vOffset); \
        m_aInput[i].x.SetSize     (coreVector2(   (s),0.03f));                                  \
        m_aInput[i].x.SetAlignment(coreVector2(-1.00f,0.00f));                                  \
        m_aInput[i].x.GetCaption()->SetColor3     (COLOR_MENU_WHITE);                           \
        m_aInput[i].x.GetCaption()->ChangeLanguage(Core::Language);                             \
    }
    {
        for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
        {
            const coreVector2 vOffset = coreVector2(0.25f * I_TO_F(MENU_CONFIG_INPUTS - i - 1u), 0.0f);

            __SET_INPUT(oType,      INPUT_TYPE,      0.22f)
            __SET_INPUT(oRumble,    INPUT_RUMBLE,    0.22f)
            __SET_INPUT(oMoveUp,    INPUT_MOVEUP,    0.22f)
            __SET_INPUT(oMoveLeft,  INPUT_MOVELEFT,  0.22f)
            __SET_INPUT(oMoveDown,  INPUT_MOVEDOWN,  0.22f)
            __SET_INPUT(oMoveRight, INPUT_MOVERIGHT, 0.22f)
            __SET_INPUT(aAction[0], INPUT_ACTION1,   0.22f)
            __SET_INPUT(aAction[1], INPUT_ACTION2,   0.22f)
            __SET_INPUT(aAction[2], INPUT_ACTION3,   0.22f)
            __SET_INPUT(aAction[3], INPUT_ACTION4,   0.22f)
            __SET_INPUT(aAction[4], INPUT_ACTION5,   0.22f)

            m_aInput[i].oType  .SetAutomatic(0.0f);   // # because of realtime-update
            m_aInput[i].oType  .SetEndless(true);
            m_aInput[i].oRumble.SetEndless(true);
        }
    }
    #undef __SET_INPUT

    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        const coreButton& oLast = this->__RetrieveInputButton(i, INPUT_KEYS - 1u);

        m_aInput[i].oHeader.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aInput[i].oHeader.SetPosition(oLast.GetPosition() - coreVector2(oLast.GetSize().x * 0.5f, 0.05f));
        m_aInput[i].oHeader.SetColor3  (COLOR_MENU_WHITE);
    }

    m_SwapInput.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_SwapInput.SetPosition(LERP(m_aInput[0].oHeader.GetPosition(), m_aInput[1].oHeader.GetPosition(), 0.5f));
    m_SwapInput.SetSize    (coreVector2(0.03f,0.03f));
    m_SwapInput.GetCaption()->SetText("<>");
    STATIC_ASSERT(MENU_CONFIG_INPUTS == 2u)

    // fill configuration entries
    const std::vector<std::string>& asLanguageList = cMenu::GetLanguageList().get_keylist();

    for(coreUintW i = 0u, ie = asLanguageList.size(); i < ie; i++)
        m_Language.AddEntry(asLanguageList[i].c_str(), i);

    const coreUint8 iMaxSamples    = Core::Graphics->GetMaxSamples();
    const coreUint8 iMaxAnisotropy = Core::Graphics->GetMaxAnisotropy();

    m_DisplayMode  .AddEntryLanguage("DISPLAYMODE_WINDOW",     0u);
    m_DisplayMode  .AddEntryLanguage("DISPLAYMODE_BORDERLESS", 1u);
    m_DisplayMode  .AddEntryLanguage("DISPLAYMODE_FULLSCREEN", 2u);
    m_AntiAliasing .AddEntryLanguage("VALUE_OFF",              0u);
    m_TextureFilter.AddEntryLanguage("VALUE_OFF",              0u);
    for(coreUintW i = 2u, ie = iMaxSamples;    i <= ie; i <<= 1u) m_AntiAliasing .AddEntry(PRINT("%zux", i), i);
    for(coreUintW i = 2u, ie = iMaxAnisotropy; i <= ie; i <<= 1u) m_TextureFilter.AddEntry(PRINT("%zux", i), i);
    if(m_AntiAliasing.GetEntry(m_AntiAliasing.GetNumEntries() - 1u).tValue != iMaxSamples) m_AntiAliasing.AddEntry(PRINT("%ux", iMaxSamples), iMaxSamples);   // possible 6x
    m_RenderQuality.AddEntryLanguage("VALUE_LOW",              0u);
    m_RenderQuality.AddEntryLanguage("VALUE_HIGH",             1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_LOW",              1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_HIGH",             2u);
    for(coreUintW i = 0u; i <= 100u; i += 10u) m_OverallVolume.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 10u) m_MusicVolume  .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 10u) m_EffectVolume .AddEntry(PRINT("%zu%%", i), i);
    m_AmbientSound .AddEntryLanguage("VALUE_OFF",              0u);
    m_AmbientSound .AddEntryLanguage("VALUE_ON",               1u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oRumble.AddEntryLanguage("VALUE_OFF", 0u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oRumble.AddEntryLanguage("VALUE_ON", 10u);
    for(coreUintW i = 0u; i <= 2u; i += 1u) m_TextSize.AddEntry(PRINT("+%zu", i), i);
    m_GameRotation .AddEntryLanguage("VALUE_OFF",              0u);
    m_GameRotation .AddEntryLanguage("HUDROTATION_LEFT",       1u);
    m_GameRotation .AddEntryLanguage("HUDROTATION_UPSIDE",     2u);
    m_GameRotation .AddEntryLanguage("HUDROTATION_RIGHT",      3u);
    for(coreUintW i = 50u; i <= 100u; i += 1u) m_GameScale.AddEntry(PRINT("%zu%%", i), i);
    m_HudRotation  .AddEntryLanguage("VALUE_OFF",              0u);
    m_HudRotation  .AddEntryLanguage("HUDROTATION_LEFT",       1u);
    m_HudRotation  .AddEntryLanguage("HUDROTATION_UPSIDE",     2u);
    m_HudRotation  .AddEntryLanguage("HUDROTATION_RIGHT",      3u);
    for(coreUintW i = 50u; i <= 150u; i += 1u) m_HudScale.AddEntry(PRINT("%zu%%", i), i);
    m_HudType      .AddEntryLanguage("HUDTYPE_OUTSIDE",        0u);
    m_HudType      .AddEntryLanguage("HUDTYPE_INSIDE",         1u);



    m_MenuInput.BindShoulder(SURFACE_CONFIG_VIDEO, &m_VideoTab);
    m_MenuInput.BindShoulder(SURFACE_CONFIG_AUDIO, &m_AudioTab);
    m_MenuInput.BindShoulder(SURFACE_CONFIG_INPUT, &m_InputTab);
    m_MenuInput.BindShoulder(SURFACE_CONFIG_GAME,  &m_GameTab);

    m_MenuInput.BindObject(&m_SaveButton);

    m_MenuInput.BindMenu(this);


    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_CONFIG_MAX; ++i)
    {


        this->BindObject(i, &m_MenuInput);



        if(i != SURFACE_CONFIG_VIDEO) this->BindObject(i, &m_VideoTab);
        if(i != SURFACE_CONFIG_AUDIO) this->BindObject(i, &m_AudioTab);
        if(i != SURFACE_CONFIG_INPUT) this->BindObject(i, &m_InputTab);
        if(i != SURFACE_CONFIG_GAME)  this->BindObject(i, &m_GameTab);

        this->BindObject(i, &m_Background);

        if(i == SURFACE_CONFIG_VIDEO) this->BindObject(i, &m_VideoTab);
        if(i == SURFACE_CONFIG_AUDIO) this->BindObject(i, &m_AudioTab);
        if(i == SURFACE_CONFIG_INPUT) this->BindObject(i, &m_InputTab);
        if(i == SURFACE_CONFIG_GAME)  this->BindObject(i, &m_GameTab);

        this->BindObject(i, &m_SaveButton);
        this->BindObject(i, &m_DiscardButton);
        this->BindObject(i, &m_BackButton);
    }

    coreUintW iIndex;
    for(iIndex = 0u; iIndex < ENTRY_VIDEO; ++iIndex) this->BindObject(SURFACE_CONFIG_VIDEO, &m_aLine [iIndex]);
    for(;            iIndex < ENTRY_AUDIO; ++iIndex) this->BindObject(SURFACE_CONFIG_AUDIO, &m_aLine [iIndex]);
    for(;            iIndex < ENTRY_INPUT; ++iIndex) this->BindObject(SURFACE_CONFIG_INPUT, &m_aLine [iIndex]);
    for(;            iIndex < ENTRY_MAX;   ++iIndex) this->BindObject(SURFACE_CONFIG_GAME,  &m_aLine [iIndex]);
    for(iIndex = 0u; iIndex < ENTRY_VIDEO; ++iIndex) this->BindObject(SURFACE_CONFIG_VIDEO, &m_aLabel[iIndex]);
    for(;            iIndex < ENTRY_AUDIO; ++iIndex) this->BindObject(SURFACE_CONFIG_AUDIO, &m_aLabel[iIndex]);
    for(;            iIndex < ENTRY_INPUT; ++iIndex) this->BindObject(SURFACE_CONFIG_INPUT, &m_aLabel[iIndex]);
    for(;            iIndex < ENTRY_MAX;   ++iIndex) this->BindObject(SURFACE_CONFIG_GAME,  &m_aLabel[iIndex]);

    this->BindObject(SURFACE_CONFIG_VIDEO, &m_Monitor);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_Resolution);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_DisplayMode);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_AntiAliasing);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_TextureFilter);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_RenderQuality);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_ShadowQuality);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_OverallVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_MusicVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_EffectVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_AmbientSound);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_Language);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_TextSize);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_GameRotation);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_GameScale);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_HudRotation);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_HudScale);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_HudType);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aArrow); ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aArrow[i]);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;   ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oHeader);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;   ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oType);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;   ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oRumble);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;   ++i)
    {
        for(coreUintW j = 0u; j < INPUT_KEYS; ++j)
            this->BindObject(SURFACE_CONFIG_INPUT, &this->__RetrieveInputButton(i, j));
    }
    this->BindObject(SURFACE_CONFIG_INPUT, &m_SwapInput);
}


// ****************************************************************
// move the config menu
void cConfigMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
         if(m_VideoTab.IsClicked()) this->ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
    else if(m_AudioTab.IsClicked()) this->ChangeSurface(SURFACE_CONFIG_AUDIO, 0.0f);
    else if(m_InputTab.IsClicked()) this->ChangeSurface(SURFACE_CONFIG_INPUT, 0.0f);
    else if(m_GameTab .IsClicked()) this->ChangeSurface(SURFACE_CONFIG_GAME,  0.0f);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_CONFIG_VIDEO:
        {
            // 
            if(m_Monitor.IsClickedArrow())
                this->__LoadResolutions(m_Monitor.GetCurEntry().tValue);

            // 
            if(m_Resolution.IsClickedArrow())
            {
                if(m_Resolution.GetEntry(m_Resolution.GetNumEntries() - 1u).tValue == 0xFFu)
                    m_Resolution.DeleteEntry(m_Resolution.GetNumEntries() - 1u);
            }

            // 
            if(m_ShadowQuality.IsClickedArrow())
                this->__UpdateShadowQuality();

            // 
            cMenu::UpdateSwitchBox(&m_Monitor);
            cMenu::UpdateSwitchBox(&m_Resolution);
            cMenu::UpdateSwitchBox(&m_DisplayMode);
            cMenu::UpdateSwitchBox(&m_AntiAliasing);
            cMenu::UpdateSwitchBox(&m_TextureFilter);
            cMenu::UpdateSwitchBox(&m_RenderQuality);
            cMenu::UpdateSwitchBox(&m_ShadowQuality);

            // 
                 if(m_RenderQuality.GetCurEntry().tValue == 0u) m_RenderQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_RenderQuality.GetCurEntry().tValue == 1u) m_RenderQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
                 if(m_ShadowQuality.GetCurEntry().tValue == 1u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_ShadowQuality.GetCurEntry().tValue == 2u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
        }
        break;

    case SURFACE_CONFIG_AUDIO:
        {
            // 
            if(m_OverallVolume.IsClickedArrow(CORE_INPUT_LEFT, CORE_INPUT_HOLD))
                this->__UpdateOverallVolume();

            // 
            cMenu::UpdateSwitchBox(&m_OverallVolume);
            cMenu::UpdateSwitchBox(&m_MusicVolume);
            cMenu::UpdateSwitchBox(&m_EffectVolume);
            cMenu::UpdateSwitchBox(&m_AmbientSound);

            // 
            m_OverallVolume.GetCaption()->SetColor3(COLOR_HEALTH(I_TO_F(m_OverallVolume.GetCurEntry().tValue) * 0.01f));
            m_MusicVolume  .GetCaption()->SetColor3(COLOR_HEALTH(I_TO_F(m_MusicVolume  .GetCurEntry().tValue) * 0.01f));
            m_EffectVolume .GetCaption()->SetColor3(COLOR_HEALTH(I_TO_F(m_EffectVolume .GetCurEntry().tValue) * 0.01f));
        }
        break;

    case SURFACE_CONFIG_INPUT:
        {
            // 
            if(m_SwapInput.IsClicked())
            {
                // 
                std::swap(g_CurConfig.Input.aiType[0], g_CurConfig.Input.aiType[1]);
                for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oType.SelectValue(g_CurConfig.Input.aiType[i]);

                // 
                this->__LoadInputs();
            }
            STATIC_ASSERT(MENU_CONFIG_INPUTS == 2u)

            // 
            cMenu::UpdateButton(&m_SwapInput, m_SwapInput.IsFocused());

            for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
            {
                sPlayerInput& oInput = m_aInput[i];

                // 
                if(oInput.oType.IsClickedArrow())
                {
                    for(coreUintW j = 0u, je = oInput.oType.GetNumEntries(); j < je; ++j)
                    {
                        coreBool bSkip = false;

                        // skip joystick/gamepad input sets without available device
                        if((oInput.oType.GetCurEntry().tValue >= INPUT_SETS_KEYBOARD) && (oInput.oType.GetCurEntry().tValue - INPUT_SETS_KEYBOARD >= Core::Input->GetJoystickNum()))
                            bSkip = true;

                        // skip already assigned input sets
                        for(coreUintW k = 0u; k < MENU_CONFIG_INPUTS; ++k)
                        {
                            if((g_CurConfig.Input.aiType[k] == oInput.oType.GetCurEntry().tValue) && (k != i))
                            {
                                bSkip = true;
                                break;
                            }
                        }

                        // 
                        if(bSkip)
                        {
                                 if(oInput.oType.GetArrow(0u)->IsClicked()) oInput.oType.Previous();
                            else if(oInput.oType.GetArrow(1u)->IsClicked()) oInput.oType.Next();
                        }
                        else break;
                    }

                    // 
                    g_CurConfig.Input.aiType[i] = oInput.oType.GetCurEntry().tValue;
                    this->__LoadInputs();
                }

                // 
                if(oInput.oRumble.IsClickedArrow()) g_CurConfig.Input.aiRumble[i] = oInput.oRumble.GetCurEntry().tValue;

                // 
                cMenu::UpdateSwitchBox(&oInput.oType);
                cMenu::UpdateSwitchBox(&oInput.oRumble);

                for(coreUintW j = 0u; j < INPUT_KEYS; ++j)
                {
                    coreButton& oButton   = this->__RetrieveInputButton  (i, j);
                    coreInt16&  iCurValue = this->__RetrieveInputCurValue(i, j);

                    if(oButton.IsClicked())
                    {
                        const coreChar*  pcText = PRINT("%s [%s]", Core::Language->GetString("QUESTION_MAPPING"), m_aLabel[ENTRY_INPUT_MOVEUP + j].GetText());
                        const coreUint8& iType  = oInput.oType.GetCurEntry().tValue;   // # referenced in lambda

                        // 
                        g_pMenu->GetMsgBox()->ShowMapping(pcText, iType, [&](const coreInt32 iAnswer, const coreInt16 iKey)
                        {
                            if(iAnswer != MSGBOX_ANSWER_KEY) return;

                            // check if new key is already assigned somewhere else
                            for(coreUintW k = 0u; k < INPUT_SETS; ++k)
                            {
                                for(coreUintW m = 0u; m < INPUT_KEYS; ++m)
                                {
                                    coreInt16& iOtherCurValue = this->__RetrieveInputDirValue(k, m);

                                    // find same value and same input set type
                                    if((iOtherCurValue == iKey) && (((k < INPUT_SETS_KEYBOARD) && (iType < INPUT_SETS_KEYBOARD)) || (k == iType)) && (&iOtherCurValue != &iCurValue))
                                    {
                                        // replace with old key
                                        iOtherCurValue = iCurValue;

                                        // update related button if currently visible
                                        for(coreUintW n = 0u; n < MENU_CONFIG_INPUTS; ++n)
                                        {
                                            if(g_CurConfig.Input.aiType[n] == k)
                                            {
                                                coreButton& oOtherButton = this->__RetrieveInputButton(n, m);
                                                oOtherButton.GetCaption()->SetText(cConfigMenu::__PrintKey(k, iOtherCurValue));
                                                break;
                                            }
                                        }

                                        // leave all loops
                                        k = INPUT_SETS; m = INPUT_KEYS;
                                    }
                                }
                            }

                            // 
                            iCurValue = iKey;
                            oButton.GetCaption()->SetText(cConfigMenu::__PrintKey(iType, iCurValue));

                            // 
                            this->CheckValues();
                        });
                    }

                    // 
                    cMenu::UpdateButton(&oButton, oButton.IsFocused());
                    oButton.SetAlpha(oButton.GetAlpha() * (oButton.IsFocused() ? 1.0f : 0.75f));
                }
            }
        }
        break;

    case SURFACE_CONFIG_GAME:
        {
            // 
            if(m_Language.IsClickedArrow())
                this->__UpdateLanguage();

            // 
            if(m_TextSize.IsClickedArrow())
                {}

            // 
            if(m_GameRotation.IsClickedArrow() ||
               m_GameScale   .IsClickedArrow() ||
               m_HudRotation .IsClickedArrow() ||
               m_HudScale    .IsClickedArrow() ||
               m_HudType     .IsClickedArrow())
                this->__UpdateInterface();

            // 
            cMenu::UpdateSwitchBox(&m_Language);
            cMenu::UpdateSwitchBox(&m_TextSize);
            cMenu::UpdateSwitchBox(&m_GameRotation);
            cMenu::UpdateSwitchBox(&m_GameScale);
            cMenu::UpdateSwitchBox(&m_HudRotation);
            cMenu::UpdateSwitchBox(&m_HudScale);
            cMenu::UpdateSwitchBox(&m_HudType);
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(m_SaveButton.IsClicked())
    {
        // 
        this->SaveValues();
    }
    else if(m_DiscardButton.IsClicked())
    {
        // 
        this->LoadValues();
    }
    else if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
    {
        if(m_SaveButton.GetOverride() >= 0)
        {
            // 
            g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_SAVE"), [this](const coreInt32 iAnswer)
            {
                // 
                if(iAnswer == MSGBOX_ANSWER_YES)
                     this->SaveValues();
                else this->LoadValues();

                // 
                m_iStatus = 101;
            });
        }
        else
        {
            // 
            m_iStatus = 1;
        }
    }

    // (# after interaction, before visual update) 
    if(Core::Input->GetAnyButton(CORE_INPUT_HOLD) || Core::System->GetWinPosChanged() || Core::System->GetWinSizeChanged())
        this->CheckValues();

    // 
    cMenu::UpdateButton(&m_VideoTab, (this->GetCurSurface() == SURFACE_CONFIG_VIDEO) || m_VideoTab.IsFocused());
    cMenu::UpdateButton(&m_AudioTab, (this->GetCurSurface() == SURFACE_CONFIG_AUDIO) || m_AudioTab.IsFocused());
    cMenu::UpdateButton(&m_InputTab, (this->GetCurSurface() == SURFACE_CONFIG_INPUT) || m_InputTab.IsFocused());
    cMenu::UpdateButton(&m_GameTab,  (this->GetCurSurface() == SURFACE_CONFIG_GAME)  || m_GameTab .IsFocused());

    // 
    cMenu::UpdateButton(&m_SaveButton,    m_SaveButton   .IsFocused());
    cMenu::UpdateButton(&m_DiscardButton, m_DiscardButton.IsFocused());
    cMenu::UpdateButton(&m_BackButton,    m_BackButton   .IsFocused());

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}


// ****************************************************************
// 
void cConfigMenu::CheckValues()
{
    const coreUint8    iCurMonitor    = m_Monitor   .GetCurEntry().tValue;
    const coreUint8    iCurValue      = m_Resolution.GetCurEntry().tValue;
    const coreVector2& vCurResolution = (iCurValue == 0xFFu) ? Core::System->GetResolution() : Core::System->GetDisplayData(iCurMonitor).avAvailableRes[iCurValue];

    // 
    const coreBool bSave = (vCurResolution != coreVector2(I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH)), I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT)))) ||
                           (m_Monitor      .GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY))                         ||
                           (m_DisplayMode  .GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))                      ||
                           (m_AntiAliasing .GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING))                  ||
                           (m_TextureFilter.GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY))             ||
                           (m_RenderQuality.GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY))                       ||
                           (m_ShadowQuality.GetCurEntry().tValue != g_OldConfig.Graphics.iShadow)                                             ||
                           (m_OverallVolume.GetCurEntry().tValue != F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME) * 100.0f)) ||
                           (m_MusicVolume  .GetCurEntry().tValue != F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)  * 100.0f)) ||
                           (m_EffectVolume .GetCurEntry().tValue != F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME)  * 100.0f)) ||
                           (m_AmbientSound .GetCurEntry().tValue != g_OldConfig.Audio.iAmbient)                                               ||
                           (std::strcmp(Core::Language->GetPath(), Core::Config->GetString(CORE_CONFIG_BASE_LANGUAGE)))                       ||
                           (m_TextSize     .GetCurEntry().tValue != g_OldConfig.Game.iTextSize)                                               ||
                           (m_GameRotation .GetCurEntry().tValue != g_OldConfig.Game.iGameRotation)                                           ||
                           (m_GameScale    .GetCurEntry().tValue != g_OldConfig.Game.iGameScale)                                              ||
                           (m_HudRotation  .GetCurEntry().tValue != g_OldConfig.Game.iHudRotation)                                            ||
                           (m_HudScale     .GetCurEntry().tValue != g_OldConfig.Game.iHudScale)                                               ||
                           (m_HudType      .GetCurEntry().tValue != g_OldConfig.Game.iHudType)                                                ||
                           (std::memcmp(&g_CurConfig.Input, &g_OldConfig.Input, sizeof(sConfig::Input)));

    // 
    m_SaveButton   .SetOverride(bSave ? 0 : -1);
    m_DiscardButton.SetOverride(bSave ? 0 : -1);
}


// ****************************************************************
// 
void cConfigMenu::LoadValues()
{
    const coreUintW iShadowQualityIndex = m_ShadowQuality.GetCurIndex();
    const coreUintW iOverallVolumeIndex = m_OverallVolume.GetCurIndex();
    const coreUintW iLanguageIndex      = m_Language     .GetCurIndex();

    // 
    LoadConfig();
    this->__LoadMonitors();
    this->__LoadResolutions(Core::System->GetDisplayIndex());
    this->__LoadInputs();

    // 
    if(!m_Resolution.SelectText(PRINT("%d x %d", Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH), Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT))))
    {
        m_Resolution.AddEntry("Other", 0xFFu);
        m_Resolution.SelectLast();
    }

    // 
    m_Monitor      .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY));
    m_DisplayMode  .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN));
    m_AntiAliasing .SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING));
    m_TextureFilter.SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY));
    m_RenderQuality.SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY));
    m_ShadowQuality.SelectValue(g_CurConfig.Graphics.iShadow);

    // 
    m_OverallVolume.SelectValue(F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME) * 100.0f));
    m_MusicVolume  .SelectValue(F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)  * 100.0f));
    m_EffectVolume .SelectValue(F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME)  * 100.0f));
    m_AmbientSound .SelectValue(g_CurConfig.Audio.iAmbient);

    // 
    const std::vector<std::string>& asLanguageList = cMenu::GetLanguageList().get_valuelist();
    m_Language    .SelectIndex(std::find(asLanguageList.begin(), asLanguageList.end(), Core::Config->GetString(CORE_CONFIG_BASE_LANGUAGE)) - asLanguageList.begin());
    m_TextSize    .SelectValue(g_CurConfig.Game.iTextSize);
    m_GameRotation.SelectValue(g_CurConfig.Game.iGameRotation);
    m_GameScale   .SelectValue(g_CurConfig.Game.iGameScale);
    m_HudRotation .SelectValue(g_CurConfig.Game.iHudRotation);
    m_HudScale    .SelectValue(g_CurConfig.Game.iHudScale);
    m_HudType     .SelectValue(g_CurConfig.Game.iHudType);

    // 
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        m_aInput[i].oType  .SelectValue(g_CurConfig.Input.aiType  [i]);
        m_aInput[i].oRumble.SelectValue(g_CurConfig.Input.aiRumble[i]);
    }

    // 
    if(m_SaveButton.GetOverride() >= 0)
    {
        if(iShadowQualityIndex != m_ShadowQuality.GetCurIndex()) this->__UpdateShadowQuality();
        if(iOverallVolumeIndex != m_OverallVolume.GetCurIndex()) this->__UpdateOverallVolume();
        if(iLanguageIndex      != m_Language     .GetCurIndex()) this->__UpdateLanguage();
        // TODO m_TextSize
        this->__UpdateInterface();
    }

    // 
    m_SaveButton   .SetOverride(-1);
    m_DiscardButton.SetOverride(-1);
}


// ****************************************************************
// 
void cConfigMenu::SaveValues()
{
    const coreUint8    iCurMonitor    = m_Monitor   .GetCurEntry().tValue;
    const coreUint8    iCurValue      = m_Resolution.GetCurEntry().tValue;
    const coreVector2& vCurResolution = (iCurValue == 0xFFu) ? Core::System->GetResolution() : Core::System->GetDisplayData(iCurMonitor).avAvailableRes[iCurValue];

    // 
    const coreBool bReset = (vCurResolution != coreVector2(I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH)), I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT)))) ||
                            (m_Monitor      .GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY))        ||
                            (m_DisplayMode  .GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))     ||
                            (m_AntiAliasing .GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING)) ||
                            (m_TextureFilter.GetCurEntry().tValue != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY));

    // 
    Core::Config->SetInt (CORE_CONFIG_SYSTEM_WIDTH,               F_TO_SI(vCurResolution.x));
    Core::Config->SetInt (CORE_CONFIG_SYSTEM_HEIGHT,              F_TO_SI(vCurResolution.y));
    Core::Config->SetInt (CORE_CONFIG_SYSTEM_DISPLAY,             m_Monitor      .GetCurEntry().tValue);
    Core::Config->SetInt (CORE_CONFIG_SYSTEM_FULLSCREEN,          m_DisplayMode  .GetCurEntry().tValue);
    Core::Config->SetInt (CORE_CONFIG_GRAPHICS_ANTIALIASING,      m_AntiAliasing .GetCurEntry().tValue);
    Core::Config->SetInt (CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY, m_TextureFilter.GetCurEntry().tValue);
    Core::Config->SetInt (CORE_CONFIG_GRAPHICS_QUALITY,           m_RenderQuality.GetCurEntry().tValue);
    Core::Config->SetBool(CORE_CONFIG_GRAPHICS_TEXTURETRILINEAR,  m_RenderQuality.GetCurEntry().tValue ? true : false);
    g_CurConfig.Graphics.iShadow = m_ShadowQuality.GetCurEntry().tValue;

    // 
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME, I_TO_F(m_OverallVolume.GetCurEntry().tValue) * 0.01f);
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME,  I_TO_F(m_MusicVolume  .GetCurEntry().tValue) * 0.01f);
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME,  I_TO_F(m_EffectVolume .GetCurEntry().tValue) * 0.01f);
    g_CurConfig.Audio.iAmbient = m_AmbientSound.GetCurEntry().tValue;

    // 
    Core::Config->SetString(CORE_CONFIG_BASE_LANGUAGE, Core::Language->GetPath());
    g_CurConfig.Game.iGameRotation = m_GameRotation.GetCurEntry().tValue;
    g_CurConfig.Game.iGameScale    = m_GameScale   .GetCurEntry().tValue;
    g_CurConfig.Game.iHudRotation  = m_HudRotation .GetCurEntry().tValue;
    g_CurConfig.Game.iHudScale     = m_HudScale    .GetCurEntry().tValue;
    g_CurConfig.Game.iHudType      = m_HudType     .GetCurEntry().tValue;

    // 
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        g_CurConfig.Input.aiType  [i] = m_aInput[i].oType  .GetCurEntry().tValue;
        g_CurConfig.Input.aiRumble[i] = m_aInput[i].oRumble.GetCurEntry().tValue;
    }

    // 
    SaveConfig();
    this->CheckValues();

    if(bReset)
    {
        // 
        InitResolution(vCurResolution);
        Core::Reset();
        InitFramerate();

        // 
        this->__LoadMonitors();
        this->__LoadResolutions(Core::System->GetDisplayIndex());
        this->__LoadInputs();
    }
}


// ****************************************************************
// 
void cConfigMenu::__UpdateShadowQuality()
{
    // 
    g_CurConfig.Graphics.iShadow = m_ShadowQuality.GetCurEntry().tValue;

    // 
    cOutdoor* pOutdoor = g_pEnvironment->GetBackground()->GetOutdoor();
    if(pOutdoor) pOutdoor->GetShadowMap()->Reconfigure();
    cShadow::Recompile();

    // 
    g_pMenu->InvokePauseStep();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateOverallVolume()
{
    // 
    Core::Audio->SetVolume(I_TO_F(m_OverallVolume.GetCurEntry().tValue) * 0.01f);
}


// ****************************************************************
// 
void cConfigMenu::__UpdateLanguage()
{
    // 
    const std::vector<std::string>& asLanguageList = cMenu::GetLanguageList().get_valuelist();
    Core::Language->Load(asLanguageList[m_Language.GetCurIndex()].c_str());

    // 
    cMenu::UpdateLanguageFont();

    // invalidate unbound language-strings
    g_pMenu->GetTooltip()->Invalidate();
    this->__LoadMonitors();
    this->__LoadInputs();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateInterface()
{
    // 
    g_CurConfig.Game.iGameRotation = m_GameRotation.GetCurEntry().tValue;
    g_CurConfig.Game.iGameScale    = m_GameScale   .GetCurEntry().tValue;
    g_CurConfig.Game.iHudRotation  = m_HudRotation .GetCurEntry().tValue;
    g_CurConfig.Game.iHudScale     = m_HudScale    .GetCurEntry().tValue;
    g_CurConfig.Game.iHudType      = m_HudType     .GetCurEntry().tValue;

    // 
    g_pPostProcessing->UpdateLayout();

    // 
    if(STATIC_ISVALID(g_pGame)) g_pGame->GetInterface()->UpdateLayout();
}


// ****************************************************************
// 
void cConfigMenu::__LoadMonitors()
{
    // 
    const coreUintW iOldEntry = m_Monitor.GetCurIndex();
    m_Monitor.ClearEntries();

    // 
    m_Monitor.AddEntryLanguage("MONITOR_PRIMARY", 0u);
    if(Core::System->GetDisplayCount() == 2u)
    {
        m_Monitor.AddEntryLanguage("MONITOR_SECONDARY", 1u);
    }
    else
    {
        for(coreUintW i = 1u, ie = Core::System->GetDisplayCount(); i < ie; ++i)
            m_Monitor.AddEntry(PRINT("%s %zu", Core::Language->GetString("MONITOR_SECONDARY"), i), i);
    }

    // 
    m_Monitor.SelectIndex((iOldEntry < m_Monitor.GetNumEntries()) ? iOldEntry : 0u);
}


// ****************************************************************
// 
void cConfigMenu::__LoadResolutions(const coreUintW iMonitorIndex)
{
    // 
    const std::string sOldEntry = m_Resolution.GetNumEntries() ? (*m_Resolution.GetCurEntry().psText) : "";
    m_Resolution.ClearEntries();

    // 
    const coreSet<coreVector2>& avResolutionList = Core::System->GetDisplayData(iMonitorIndex).avAvailableRes;
    for(coreUintW i = avResolutionList.size(); i--; ) m_Resolution.AddEntry(PRINT("%.0f x %.0f", avResolutionList[i].x, avResolutionList[i].y), i);

    // 
    if(!m_Resolution.SelectText(sOldEntry.c_str())) m_Resolution.SelectLast();
}


// ****************************************************************
// 
void cConfigMenu::__LoadInputs()
{
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        sPlayerInput& oInput = m_aInput[i];

        // 
        const coreUintW iOldEntry = oInput.oType.GetCurIndex();
        oInput.oType.ClearEntries();

        // 
        oInput.oHeader.SetText(PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
        for(coreUintW j = 0u; j < INPUT_SETS_KEYBOARD; ++j) oInput.oType.AddEntry(PRINT("%s %zu", Core::Language->GetString("INPUT_KEYBOARD"), j + 1u), j + 0u);
        for(coreUintW j = 0u; j < INPUT_SETS_JOYSTICK; ++j) oInput.oType.AddEntry(PRINT("%s %zu", Core::Language->GetString("INPUT_JOYSTICK"), j + 1u), j + INPUT_SETS_KEYBOARD);

        // 
        oInput.oType.SelectIndex((iOldEntry < oInput.oType.GetNumEntries()) ? iOldEntry : 0u);

        // 
        for(coreUintW j = 0u; j < INPUT_KEYS; ++j)
        {
            coreButton& oButton   = this->__RetrieveInputButton  (i, j);
            coreInt16&  iCurValue = this->__RetrieveInputCurValue(i, j);

            oButton.GetCaption()->SetText(cConfigMenu::__PrintKey(g_CurConfig.Input.aiType[i], iCurValue));
        }

        // 
        const auto nLockFunc = [](const coreBool bLock, coreButton* OUTPUT pButton, const coreChar* pcText)
        {
            pButton->SetOverride(bLock ? -1 : 0);
            if(bLock) pButton->GetCaption()->SetText(pcText);
        };

        // 
        const coreBool bKeyboard = (g_CurConfig.Input.aiType[i] < INPUT_SETS_KEYBOARD);
        nLockFunc(!bKeyboard, &oInput.oMoveUp,                         SDL_GetKeyName(CORE_INPUT_CHAR(UP)));
        nLockFunc(!bKeyboard, &oInput.oMoveLeft,                       SDL_GetKeyName(CORE_INPUT_CHAR(LEFT)));
        nLockFunc(!bKeyboard, &oInput.oMoveDown,                       SDL_GetKeyName(CORE_INPUT_CHAR(DOWN)));
        nLockFunc(!bKeyboard, &oInput.oMoveRight,                      SDL_GetKeyName(CORE_INPUT_CHAR(RIGHT)));
        nLockFunc( bKeyboard, &oInput.aAction[INPUT_KEYS_ACTION - 1u], SDL_GetKeyName(CORE_INPUT_CHAR(ESCAPE)));

        // 
        oInput.oRumble.SetOverride(bKeyboard ? -1 : 0);
    }
}


// ****************************************************************
// convert input key to readable string
const coreChar* cConfigMenu::__PrintKey(const coreUint8 iType, const coreInt16 iKey)
{
    if(iType < INPUT_SETS_KEYBOARD)   // # keyboard and mouse
    {
        return (iKey <= 0) ? PRINT("M %d", -iKey) : SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(iKey)));
    }
    else   // # joystick/gamepad
    {
        return PRINT("J %d", iKey);
    }
}