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
cConfigMenu::cConfigMenu()noexcept
: coreMenu           (SURFACE_CONFIG_MAX, SURFACE_CONFIG_VIDEO)
, m_apcDescKey       {}
, m_iCurMonitorIndex (Core::System->GetDisplayIndex())
, m_iJoystickNum     (0u)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.01f));
    m_Background.SetSize      (coreVector2(0.9f,0.75f));

    m_VideoTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_VideoTab.DefineProgram("menu_border_program");
    m_VideoTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.1275f,-0.0125f));
    m_VideoTab.SetSize      (coreVector2(0.195f,0.07f));
    m_VideoTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_VideoTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_VideoTab.SetTexOffset (m_VideoTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.1275f,0.0125f));
    m_VideoTab.GetCaption()->SetTextLanguage("CONFIG_VIDEO");

    m_AudioTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_AudioTab.DefineProgram("menu_border_program");
    m_AudioTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(1.0f/3.0f,1.0f));
    m_AudioTab.SetSize      (m_VideoTab.GetSize());
    m_AudioTab.SetAlignment (m_VideoTab.GetAlignment());
    m_AudioTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_AudioTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_AudioTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0f));
    m_AudioTab.GetCaption()->SetTextLanguage("CONFIG_AUDIO");

    m_InputTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_InputTab.DefineProgram("menu_border_program");
    m_InputTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(-1.0f/3.0f,1.0f));
    m_InputTab.SetSize      (m_VideoTab.GetSize());
    m_InputTab.SetAlignment (m_VideoTab.GetAlignment());
    m_InputTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_InputTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_InputTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0f));
    m_InputTab.GetCaption()->SetTextLanguage("CONFIG_INPUT");

    m_GameTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_GameTab.DefineProgram("menu_border_program");
    m_GameTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_GameTab.SetSize      (m_VideoTab.GetSize());
    m_GameTab.SetAlignment (m_VideoTab.GetAlignment());
    m_GameTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_GameTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_GameTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0f));
    m_GameTab.GetCaption()->SetTextLanguage("CONFIG_GAME");

    m_SaveButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_SaveButton.DefineProgram("menu_border_program");
    m_SaveButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f) + MENU_BUTTON_SHIFT);
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

    m_InputButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
    m_InputButton.DefineProgram("menu_border_program");
    m_InputButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_SaveButton.GetSize().y);
    m_InputButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_InputButton.GetCaption()->SetText(ICON_SWITCH);

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_SaveButton.GetSize().y);
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    m_InputButton.SetPosition(m_BackButton.GetPosition() - coreVector2(m_BackButton.GetSize().x + 0.02f, 0.0f));

    coreSet<coreUintW> aiSkip;
    #if defined(_CORE_EMSCRIPTEN_)
        aiSkip.insert(ENTRY_VIDEO_MONITOR);
        aiSkip.insert(ENTRY_VIDEO_RESOLUTION);
        aiSkip.insert(ENTRY_VIDEO_DISPLAYMODE);
        aiSkip.insert(ENTRY_VIDEO_VSYNC);
        aiSkip.insert(ENTRY_VIDEO_ANTIALIASING);
        aiSkip.insert(ENTRY_AUDIO_QUALITY);
        aiSkip.insert(ENTRY_AUDIO_MODE);
    #elif defined(_CORE_SWITCH_)
        aiSkip.insert(ENTRY_VIDEO_MONITOR);
        aiSkip.insert(ENTRY_VIDEO_RESOLUTION);
        aiSkip.insert(ENTRY_VIDEO_DISPLAYMODE);
        aiSkip.insert(ENTRY_VIDEO_VSYNC);
        aiSkip.insert(ENTRY_VIDEO_ANTIALIASING);
        aiSkip.insert(ENTRY_VIDEO_TEXTUREFILTER);
        aiSkip.insert(ENTRY_VIDEO_RENDERQUALITY);
        aiSkip.insert(ENTRY_VIDEO_SHADOWQUALITY);
        aiSkip.insert(ENTRY_VIDEO_WATERQUALITY);
        aiSkip.insert(ENTRY_AUDIO_QUALITY);
        aiSkip.insert(ENTRY_INPUT_TYPE);
        aiSkip.insert(ENTRY_GAME_UPDATEFREQ);
    #endif
    #if !defined(_CORE_DEBUG_)
        aiSkip.insert(ENTRY_GAME_VERSION);
    #endif
    if(!g_bLeaderboards && !DEFINED(_CORE_DEBUG_))
    {
        aiSkip.insert(ENTRY_GAME_LEADERBOARD);
    }

    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        if(aiSkip.count(i)) continue;

        if(i == ENTRY_VIDEO || i == ENTRY_AUDIO || i == ENTRY_INPUT) iOffset = 0u;
#if defined(_CORE_SWITCH_)
        if(i == ENTRY_INPUT_RUMBLE) iOffset = 0u;
#endif
        if(i == ENTRY_VIDEO_ANTIALIASING)    ++iOffset;   // # new paragraph
        if(i == ENTRY_VIDEO_RENDERQUALITY)   ++iOffset;
#if !defined(_CORE_SWITCH_)
        if(i == ENTRY_VIDEO_PARTICLEEFFECTS) ++iOffset;
#endif
        if(i == ENTRY_AUDIO_MUSICVOLUME)     ++iOffset;
        if(i == ENTRY_AUDIO_QUALITY)         ++iOffset;
#if defined(_CORE_SWITCH_)
        if(i == ENTRY_AUDIO_MODE)            ++iOffset;
#endif
        if(i == ENTRY_AUDIO_3DSOUND)         ++iOffset;
#if !defined(_CORE_SWITCH_)
        if(i == ENTRY_INPUT_RUMBLE)          ++iOffset;
#endif
        if(i == ENTRY_INPUT_MOVEUP)          ++iOffset;
        if(i == ENTRY_INPUT_ACTION1)         ++iOffset;
        if(i == ENTRY_INPUT_ACTION4)         ++iOffset;
        if(i == ENTRY_INPUT_ACTION8)         ++iOffset;
        if(i == ENTRY_GAME_GAMEDIRECTION)    ++iOffset;
        if(i == ENTRY_GAME_HUDDIRECTION)     ++iOffset;
        if(i == ENTRY_GAME_BACKROTATION)     ++iOffset;
#if defined(_CORE_SWITCH_)
        if(i == ENTRY_GAME_PUREMODE)         ++iOffset;
#else
        if(i == ENTRY_GAME_UPDATEFREQ)       ++iOffset;
#endif

        m_aLabel[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aLabel[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.025f*I_TO_F(iOffset)));
        m_aLabel[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aLabel[i].SetColor3   (COLOR_MENU_WHITE);

        iOffset += 2u;
    }

    iOffset = 0u;
    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        if(aiSkip.count(i)) continue;

        if(i == ENTRY_AUDIO_GLOBALVOLUME) iOffset += 1u;
        if(i == ENTRY_GAME_LANGUAGE)      iOffset += 2u;

        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aLabel[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(iOffset)*0.09f, 0.0f));
        m_aLine[i].SetFocusable (true);

        iOffset += 1u;
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueInput); ++i)
    {
        m_aCueInput[i].Construct  (MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        //m_aCueInput[i].SetPosition(m_aLabel[ENTRY_INPUT_ACTION4 + i].GetPosition() + coreVector2(0.23f,0.0f));
        m_aCueInput[i].SetColor3  (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    }
    m_aCueInput[0] .SetDirection(coreVector2( 0.0f,-1.0f));
    m_aCueInput[1] .SetDirection(coreVector2(-1.0f, 0.0f));
    m_aCueInput[3] .SetDirection(coreVector2( 1.0f, 0.0f));
    m_aCueInput[11].SetDirection(coreVector2( 1.0f, 0.0f));
    m_aCueInput[12].SetDirection(coreVector2( 0.0f,-1.0f));
    m_aCueInput[13].SetDirection(coreVector2(-1.0f, 0.0f));
    m_aCueInput[0] .SetText     (ICON_BURN);
    m_aCueInput[1] .SetText     (ICON_BURN);
    m_aCueInput[2] .SetText     (ICON_BURN);
    m_aCueInput[3] .SetText     (ICON_BURN);
    m_aCueInput[4] .SetText     (ICON_FORWARD);
    m_aCueInput[5] .SetText     (ICON_FIRE_ALT);
    m_aCueInput[6] .SetText     (ICON_BOLT);
    m_aCueInput[7] .SetText     (ICON_UNDO_ALT);
    m_aCueInput[8] .SetText     (ICON_REDO_ALT);
    m_aCueInput[9] .SetText     (ICON_PAUSE);
    m_aCueInput[10].SetText     (ICON_ARROW_UP);
    m_aCueInput[11].SetText     (ICON_ARROW_UP);
    m_aCueInput[12].SetText     (ICON_ARROW_UP);
    m_aCueInput[13].SetText     (ICON_ARROW_UP);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueRota); ++i)
    {
        m_aCueRota[i].Construct(MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aCueRota[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aCueRota[i].SetText  (ICON_ARROW_UP);
    }
    m_aCueRota[0].SetPosition(m_aLabel[ENTRY_GAME_GAMEDIRECTION].GetPosition() + coreVector2(0.472f,0.0f));
    m_aCueRota[1].SetPosition(m_aLabel[ENTRY_GAME_HUDDIRECTION] .GetPosition() + coreVector2(0.472f,0.0f));

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueLock); ++i)
    {
        m_aCueLock[i].Construct(MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aCueLock[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aCueLock[i].SetText  (ICON_LOCK);
    }
    m_aCueLock[0].SetPosition(m_aLabel[ENTRY_GAME_BACKROTATION].GetPosition() + coreVector2(0.472f,0.0f));
    m_aCueLock[1].SetPosition(m_aLabel[ENTRY_GAME_BACKSPEED]   .GetPosition() + coreVector2(0.472f,0.0f));
#if defined(_CORE_SWITCH_)
    m_aCueLock[2].SetPosition(HIDDEN_POS);
#else
    m_aCueLock[2].SetPosition(m_aLabel[ENTRY_GAME_UPDATEFREQ]  .GetPosition() + coreVector2(0.472f,0.0f));
#endif
    m_aCueLock[3].SetPosition(m_aLabel[ENTRY_GAME_PUREMODE]    .GetPosition() + coreVector2(0.472f,0.0f));
    m_aCueLock[4].SetPosition(HIDDEN_POS);   // ENTRY_GAME_VERSION

    m_Description.Construct  (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    m_Description.SetPosition(m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,-0.5f) + coreVector2(0.0f,0.045f));
    m_Description.SetColor3  (COLOR_MENU_WHITE);

    #define __SET_OPTION(x,n,s)                                                  \
    {                                                                            \
        cGuiLabel& oLabel = m_aLabel[ENTRY_ ## n];                               \
        oLabel.SetTextLanguage("CONFIG_" #n);                                    \
                                                                                 \
        x.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL); \
        x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition());        \
        x.SetSize     (coreVector2((s),   0.03f));                               \
        x.SetAlignment(coreVector2(-1.00f,0.00f));                               \
        x.GetCaption()->SetColor3     (COLOR_MENU_WHITE);                        \
        x.GetCaption()->ChangeLanguage(Core::Language);                          \
                                                                                 \
        m_apcDescKey[ENTRY_ ## n] = "CONFIG_" #n "_DESC";                        \
                                                                                 \
        if(aiSkip.count(ENTRY_ ## n)) x.SetStatus(1);                            \
    }
    {
        __SET_OPTION(m_Monitor,         VIDEO_MONITOR,         0.31f)
        __SET_OPTION(m_Resolution,      VIDEO_RESOLUTION,      0.31f)
        __SET_OPTION(m_DisplayMode,     VIDEO_DISPLAYMODE,     0.31f)
        __SET_OPTION(m_Vsync,           VIDEO_VSYNC,           0.31f)
        __SET_OPTION(m_AntiAliasing,    VIDEO_ANTIALIASING,    0.31f)
        __SET_OPTION(m_TextureFilter,   VIDEO_TEXTUREFILTER,   0.31f)
        __SET_OPTION(m_RenderQuality,   VIDEO_RENDERQUALITY,   0.31f)
        __SET_OPTION(m_ShadowQuality,   VIDEO_SHADOWQUALITY,   0.31f)
        __SET_OPTION(m_WaterQuality,    VIDEO_WATERQUALITY,    0.31f)
        __SET_OPTION(m_ParticleEffects, VIDEO_PARTICLEEFFECTS, 0.31f)
        __SET_OPTION(m_ShakeEffects,    VIDEO_SHAKEEFFECTS,    0.31f)
        __SET_OPTION(m_FlashEffects,    VIDEO_FLASHEFFECTS,    0.31f)
        __SET_OPTION(m_HitStopEffects,  VIDEO_HITSTOPEFFECTS,  0.31f)
        __SET_OPTION(m_ChromaEffects,   VIDEO_CHROMAEFFECTS,   0.31f)
        __SET_OPTION(m_GlobalVolume,    AUDIO_GLOBALVOLUME,    0.31f)
        __SET_OPTION(m_MusicVolume,     AUDIO_MUSICVOLUME,     0.31f)
        __SET_OPTION(m_EffectVolume,    AUDIO_EFFECTVOLUME,    0.31f)
        __SET_OPTION(m_AmbientVolume,   AUDIO_AMBIENTVOLUME,   0.31f)
        __SET_OPTION(m_MenuVolume,      AUDIO_MENUVOLUME,      0.31f)
        __SET_OPTION(m_AudioQuality,    AUDIO_QUALITY,         0.31f)
        __SET_OPTION(m_AudioMode,       AUDIO_MODE,            0.31f)
        __SET_OPTION(m_3DSound,         AUDIO_3DSOUND,         0.31f)
        __SET_OPTION(m_Language,        GAME_LANGUAGE,         0.39f)   // #
        __SET_OPTION(m_GameDirection,   GAME_GAMEDIRECTION,    0.31f)
        __SET_OPTION(m_MirrorMode,      GAME_MIRRORMODE,       0.31f)
        __SET_OPTION(m_HudDirection,    GAME_HUDDIRECTION,     0.31f)
        __SET_OPTION(m_HudType,         GAME_HUDTYPE,          0.31f)
        __SET_OPTION(m_CombatText,      GAME_COMBATTEXT,       0.31f)
        __SET_OPTION(m_BackRotation,    GAME_BACKROTATION,     0.31f)
        __SET_OPTION(m_BackSpeed,       GAME_BACKSPEED,        0.31f)
        __SET_OPTION(m_UpdateFreq,      GAME_UPDATEFREQ,       0.31f)
        __SET_OPTION(m_PureMode,        GAME_PUREMODE,         0.31f)
        __SET_OPTION(m_Leaderboard,     GAME_LEADERBOARD,      0.31f)
        __SET_OPTION(m_Version,         GAME_VERSION,          0.31f)
 
        m_Language     .SetEndless(true);
        m_GameDirection.SetEndless(true);
        m_HudDirection .SetEndless(true);

        m_Navigator.BindObject(&m_Monitor,         &m_VideoTab,        NULL, &m_Resolution,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_Resolution,      &m_Monitor,         NULL, &m_DisplayMode,     NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_DisplayMode,     &m_Resolution,      NULL, &m_Vsync,           NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_Vsync,           &m_DisplayMode,     NULL, &m_AntiAliasing,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_AntiAliasing,    &m_Vsync,           NULL, &m_TextureFilter,   NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_TextureFilter,   &m_AntiAliasing,    NULL, &m_RenderQuality,   NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_RenderQuality,   &m_TextureFilter,   NULL, &m_ShadowQuality,   NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_ShadowQuality,   &m_RenderQuality,   NULL, &m_WaterQuality,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_WaterQuality,    &m_ShadowQuality,   NULL, &m_ParticleEffects, NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_ParticleEffects, &m_WaterQuality,    NULL, &m_ShakeEffects,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_ShakeEffects,    &m_ParticleEffects, NULL, &m_FlashEffects,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_FlashEffects,    &m_ShakeEffects,    NULL, &m_HitStopEffects,  NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_HitStopEffects,  &m_FlashEffects,    NULL, &m_ChromaEffects,   NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_ChromaEffects,   &m_HitStopEffects,  NULL, &m_SaveButton,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_VIDEO);
        m_Navigator.BindObject(&m_GlobalVolume,    &m_AudioTab,        NULL, &m_MusicVolume,     NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_MusicVolume,     &m_GlobalVolume,    NULL, &m_EffectVolume,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_EffectVolume,    &m_MusicVolume,     NULL, &m_AmbientVolume,   NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_AmbientVolume,   &m_EffectVolume,    NULL, &m_MenuVolume,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_MenuVolume,      &m_AmbientVolume,   NULL, &m_AudioQuality,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_AudioQuality,    &m_MenuVolume,      NULL, &m_AudioMode,       NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_AudioMode,       &m_AudioQuality,    NULL, &m_3DSound,         NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_3DSound,         &m_AudioMode,       NULL, &m_SaveButton,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_AUDIO);
        m_Navigator.BindObject(&m_Language,        &m_GameTab,         NULL, &m_GameDirection,   NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_GameDirection,   &m_Language,        NULL, &m_MirrorMode,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_MirrorMode,      &m_GameDirection,   NULL, &m_HudDirection,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_HudDirection,    &m_MirrorMode,      NULL, &m_HudType,         NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_HudType,         &m_HudDirection,    NULL, &m_CombatText,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_CombatText,      &m_HudType,         NULL, &m_BackRotation,    NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_BackRotation,    &m_CombatText,      NULL, &m_BackSpeed,       NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_BackSpeed,       &m_BackRotation,    NULL, &m_UpdateFreq,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_UpdateFreq,      &m_BackSpeed,       NULL, &m_PureMode,        NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_PureMode,        &m_UpdateFreq,      NULL, &m_Leaderboard,     NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_Leaderboard,     &m_PureMode,        NULL, &m_Version,         NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
        m_Navigator.BindObject(&m_Version,         &m_Leaderboard,     NULL, &m_SaveButton,      NULL, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE, SURFACE_CONFIG_GAME);
    }
    #undef __SET_OPTION

    #define __SET_INPUT_SWITCH(x,n)                                                             \
    {                                                                                           \
        cGuiLabel& oLabel = m_aLabel[ENTRY_ ## n];                                              \
        oLabel.SetTextLanguage("CONFIG_" #n);                                                   \
                                                                                                \
        m_aInput[i].x.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);    \
        m_aInput[i].x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition() - vOffset); \
        m_aInput[i].x.SetSize     (coreVector2( 0.26f,0.03f));                                  \
        m_aInput[i].x.SetAlignment(coreVector2(-1.00f,0.00f));                                  \
        m_aInput[i].x.GetCaption()->SetColor3     (COLOR_MENU_WHITE);                           \
        m_aInput[i].x.GetCaption()->ChangeLanguage(Core::Language);                             \
                                                                                                \
        m_apcDescKey[ENTRY_ ## n] = "CONFIG_" #n "_DESC";                                       \
                                                                                                \
        if(aiSkip.count(ENTRY_ ## n)) m_aInput[i].x.SetStatus(1);                               \
    }
    #define __SET_INPUT_BUTTON(x,n,f)                                                           \
    {                                                                                           \
        cGuiLabel& oLabel = m_aLabel[ENTRY_ ## n];                                              \
        oLabel.SetTextLanguage("CONFIG_" #n);                                                   \
                                                                                                \
        m_aInput[i].x.Construct   (MENU_SWITCHBOX);                                             \
        m_aInput[i].x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition() - vOffset + coreVector2(-0.05f,0.0f)); \
        m_aInput[i].x.SetSize     (coreVector2( 0.16f,0.03f));                                  \
        m_aInput[i].x.SetAlignment(coreVector2(-1.00f,0.00f));                                  \
                                                                                                \
        m_aInput[i].f.SetPosition(coreVector2(-1.00f,1.00f) * oLabel.GetPosition() - vOffset2); \
        m_aInput[i].f.SetSize    (coreVector2( 0.06f,0.06f));                                   \
                                                                                                \
        m_apcDescKey[ENTRY_ ## n] = "CONFIG_" #n "_DESC";                                       \
                                                                                                \
        if(aiSkip.count(ENTRY_ ## n)) m_aInput[i].x.SetStatus(1);                               \
        if(aiSkip.count(ENTRY_ ## n)) m_aInput[i].f.SetStatus(1);                               \
    }
    {
        for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
        {
            const coreVector2 vOffset  = coreVector2(0.29f * I_TO_F(MENU_CONFIG_INPUTS - i - 1u),                0.0f);
            const coreVector2 vOffset2 = coreVector2(0.29f * I_TO_F(MENU_CONFIG_INPUTS - i - 1u) + 0.26f * 0.5f, 0.0f);

            __SET_INPUT_SWITCH(oType,        INPUT_TYPE)
            __SET_INPUT_SWITCH(oRumble,      INPUT_RUMBLE)
            __SET_INPUT_SWITCH(oFireMode,    INPUT_FIREMODE)
            __SET_INPUT_SWITCH(oControlMode, INPUT_CONTROLMODE)
            __SET_INPUT_BUTTON(oMoveUp,      INPUT_MOVEUP,    oFigureMoveUp)
            __SET_INPUT_BUTTON(oMoveLeft,    INPUT_MOVELEFT,  oFigureMoveLeft)
            __SET_INPUT_BUTTON(oMoveDown,    INPUT_MOVEDOWN,  oFigureMoveDown)
            __SET_INPUT_BUTTON(oMoveRight,   INPUT_MOVERIGHT, oFigureMoveRight)
            __SET_INPUT_BUTTON(aAction[0],   INPUT_ACTION1,   aFigureAction[0])
            __SET_INPUT_BUTTON(aAction[1],   INPUT_ACTION2,   aFigureAction[1])
            __SET_INPUT_BUTTON(aAction[2],   INPUT_ACTION3,   aFigureAction[2])
            __SET_INPUT_BUTTON(aAction[3],   INPUT_ACTION4,   aFigureAction[3])
            __SET_INPUT_BUTTON(aAction[4],   INPUT_ACTION5,   aFigureAction[4])
            __SET_INPUT_BUTTON(aAction[5],   INPUT_ACTION6,   aFigureAction[5])
            __SET_INPUT_BUTTON(aAction[6],   INPUT_ACTION7,   aFigureAction[6])
            __SET_INPUT_BUTTON(aAction[7],   INPUT_ACTION8,   aFigureAction[7])
            __SET_INPUT_BUTTON(aAction[8],   INPUT_ACTION9,   aFigureAction[8])
            __SET_INPUT_BUTTON(aAction[9],   INPUT_ACTION10,  aFigureAction[9])

            m_aInput[i].oFigureStickL.SetPosition(LERP(m_aInput[i].oMoveLeft.GetPosition(), m_aInput[i].oMoveDown.GetPosition(), 0.5f));
            m_aInput[i].oFigureStickL.SetSize    (coreVector2(0.08f,0.08f));

            m_aInput[i].oFigureStickR.SetPosition(LERP(m_aInput[i].aAction[4].GetPosition(), m_aInput[i].aAction[5].GetPosition(), 0.5f));
            m_aInput[i].oFigureStickR.SetSize    (coreVector2(0.08f,0.08f));

            m_aInput[i].oType.SetEndless(true);

            m_Navigator.BindObject(&m_aInput[i].oType,        &m_InputTab,               &m_aInput[1u - i].oType,        &m_aInput[i].oRumble,      &m_aInput[1u - i].oType,        MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS, SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].oRumble,      &m_aInput[i].oType,        &m_aInput[1u - i].oRumble,      &m_aInput[i].oFireMode,    &m_aInput[1u - i].oRumble,      MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS, SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].oFireMode,    &m_aInput[i].oRumble,      &m_aInput[1u - i].oFireMode,    &m_aInput[i].oControlMode, &m_aInput[1u - i].oFireMode,    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS, SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].oControlMode, &m_aInput[i].oFireMode,    &m_aInput[1u - i].oControlMode, &m_aInput[i].aAction[3],   &m_aInput[1u - i].oControlMode, MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS, SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[3],   &m_aInput[i].oControlMode, &m_aInput[1u - i].aAction[3],   &m_aInput[i].aAction[4],   &m_aInput[1u - i].aAction[3],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[4],   &m_aInput[i].aAction[3],   &m_aInput[1u - i].aAction[4],   &m_aInput[i].aAction[5],   &m_aInput[1u - i].aAction[4],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[5],   &m_aInput[i].aAction[4],   &m_aInput[1u - i].aAction[5],   &m_aInput[i].aAction[6],   &m_aInput[1u - i].aAction[5],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[6],   &m_aInput[i].aAction[5],   &m_aInput[1u - i].aAction[6],   &m_aInput[i].aAction[9],   &m_aInput[1u - i].aAction[6],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[9],   &m_aInput[i].aAction[6],   &m_aInput[1u - i].aAction[9],   &m_aInput[i].aAction[0],   &m_aInput[1u - i].aAction[9],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[0],   &m_aInput[i].aAction[9],   &m_aInput[1u - i].aAction[0],   &m_aInput[i].aAction[8],   &m_aInput[1u - i].aAction[0],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[8],   &m_aInput[i].aAction[0],   &m_aInput[1u - i].aAction[8],   &m_aInput[i].aAction[1],   &m_aInput[1u - i].aAction[8],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[1],   &m_aInput[i].aAction[8],   &m_aInput[1u - i].aAction[1],   &m_aInput[i].aAction[2],   &m_aInput[1u - i].aAction[1],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[2],   &m_aInput[i].aAction[1],   &m_aInput[1u - i].aAction[2],   &m_aInput[i].aAction[7],   &m_aInput[1u - i].aAction[2],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].aAction[7],   &m_aInput[i].aAction[2],   &m_aInput[1u - i].aAction[7],   &m_aInput[i].oMoveUp,      &m_aInput[1u - i].aAction[7],   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].oMoveUp,      &m_aInput[i].aAction[7],   &m_aInput[1u - i].oMoveUp,      &m_aInput[i].oMoveLeft,    &m_aInput[1u - i].oMoveUp,      MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].oMoveLeft,    &m_aInput[i].oMoveUp,      &m_aInput[1u - i].oMoveLeft,    &m_aInput[i].oMoveDown,    &m_aInput[1u - i].oMoveLeft,    MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].oMoveDown,    &m_aInput[i].oMoveLeft,    &m_aInput[1u - i].oMoveDown,    &m_aInput[i].oMoveRight,   &m_aInput[1u - i].oMoveDown,    MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);
            m_Navigator.BindObject(&m_aInput[i].oMoveRight,   &m_aInput[i].oMoveDown,    &m_aInput[1u - i].oMoveRight,   &m_SwapInput,              &m_aInput[1u - i].oMoveRight,   MENU_TYPE_TAB_NODE,                          SURFACE_CONFIG_INPUT);

            STATIC_ASSERT(MENU_CONFIG_INPUTS == 2u)
        }
    }
    #undef __SET_INPUT_SWITCH
    #undef __SET_INPUT_BUTTON

    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        m_aInput[i].oHeader.Construct  (MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_aInput[i].oHeader.SetPosition(coreVector2(m_aInput[i].oMoveUp.GetPosition().x - m_aInput[i].oMoveUp.GetSize().x * 0.5f, m_Background.GetPosition().y - 0.325f));
        m_aInput[i].oHeader.SetColor3  (COLOR_MENU_WHITE);
    }
    
    // TODO 1: order
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueInput); ++i)
    {
        m_aCueInput[i].SetPosition(coreVector2(LERP(m_aInput[0].oHeader.GetPosition().x, m_aInput[1].oHeader.GetPosition().x, 0.5f), m_aLabel[ENTRY_INPUT_ACTION4 + i].GetPosition().y));
    }
    
    
    
    m_SwapInput.Construct  (MENU_SWITCHBOX, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_SwapInput.SetPosition(LERP(m_aInput[0].oHeader.GetPosition(), m_aInput[1].oHeader.GetPosition(), 0.5f));
    m_SwapInput.SetSize    (coreVector2(0.06f,0.03f));
    m_SwapInput.GetCaption()->SetText("< >");
    STATIC_ASSERT(MENU_CONFIG_INPUTS == 2u)

    m_VideoBox.SetPosition(m_Background.GetPosition() + coreVector2(0.0f,0.025f));
    m_VideoBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.65f));
#if !defined(_CORE_EMSCRIPTEN_) && !defined(_CORE_SWITCH_)
    m_VideoBox.SetMaxOffset(0.05f * 15.5f - m_VideoBox.GetSize().y);
#endif
    for(coreUintW i = 0u; i < ENTRY_VIDEO; ++i) {if(!aiSkip.count(i)) m_VideoBox.BindObject(&m_aLine [i]);}
    for(coreUintW i = 0u; i < ENTRY_VIDEO; ++i) {if(!aiSkip.count(i)) m_VideoBox.BindObject(&m_aLabel[i]);}
    if(!m_Monitor        .GetStatus()) m_VideoBox.BindObject(&m_Monitor);
    if(!m_Resolution     .GetStatus()) m_VideoBox.BindObject(&m_Resolution);
    if(!m_DisplayMode    .GetStatus()) m_VideoBox.BindObject(&m_DisplayMode);
    if(!m_Vsync          .GetStatus()) m_VideoBox.BindObject(&m_Vsync);
    if(!m_AntiAliasing   .GetStatus()) m_VideoBox.BindObject(&m_AntiAliasing);
    if(!m_TextureFilter  .GetStatus()) m_VideoBox.BindObject(&m_TextureFilter);
    if(!m_RenderQuality  .GetStatus()) m_VideoBox.BindObject(&m_RenderQuality);
    if(!m_ShadowQuality  .GetStatus()) m_VideoBox.BindObject(&m_ShadowQuality);
    if(!m_WaterQuality   .GetStatus()) m_VideoBox.BindObject(&m_WaterQuality);
    if(!m_ParticleEffects.GetStatus()) m_VideoBox.BindObject(&m_ParticleEffects);
    if(!m_ShakeEffects   .GetStatus()) m_VideoBox.BindObject(&m_ShakeEffects);
    if(!m_FlashEffects   .GetStatus()) m_VideoBox.BindObject(&m_FlashEffects);
    if(!m_HitStopEffects .GetStatus()) m_VideoBox.BindObject(&m_HitStopEffects);
    if(!m_ChromaEffects  .GetStatus()) m_VideoBox.BindObject(&m_ChromaEffects);

    m_InputBox.SetPosition(m_Background.GetPosition() + coreVector2(0.0f,0.025f));
    m_InputBox.SetSize    (coreVector2(m_Background.GetSize().x, 0.65f));
#if defined(_CORE_SWITCH_)
    m_InputBox.SetMaxOffset(0.05f * 19.0f - m_InputBox.GetSize().y);
#else
    m_InputBox.SetMaxOffset(0.05f * 20.5f - m_InputBox.GetSize().y);
#endif
    for(coreUintW i = ENTRY_AUDIO; i < ENTRY_INPUT; ++i) {if(!aiSkip.count(i)) m_InputBox.BindObject(&m_aLine [i]);}
    for(coreUintW i = ENTRY_AUDIO; i < ENTRY_INPUT; ++i) {if(!aiSkip.count(i)) m_InputBox.BindObject(&m_aLabel[i]);}
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueInput); ++i) m_InputBox.BindObject(&m_aCueInput[i]);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) if(!m_aInput[i].oType       .GetStatus()) m_InputBox.BindObject(&m_aInput[i].oType);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) if(!m_aInput[i].oRumble     .GetStatus()) m_InputBox.BindObject(&m_aInput[i].oRumble);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) if(!m_aInput[i].oFireMode   .GetStatus()) m_InputBox.BindObject(&m_aInput[i].oFireMode);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) if(!m_aInput[i].oControlMode.GetStatus()) m_InputBox.BindObject(&m_aInput[i].oControlMode);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) for(coreUintW j = 0u; j < INPUT_KEYS; ++j) if(!this->__RetrieveInputButton(i, j).GetStatus()) m_InputBox.BindObject(&this->__RetrieveInputButton(i, j));
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) for(coreUintW j = 0u; j < INPUT_KEYS; ++j) if(!this->__RetrieveInputFigure(i, j).GetStatus()) m_InputBox.BindObject(&this->__RetrieveInputFigure(i, j));
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) m_InputBox.BindObject(&m_aInput[i].oFigureStickL);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) m_InputBox.BindObject(&m_aInput[i].oFigureStickR);

    m_Navigator.BindObject(&m_SwapInput, &m_aInput[0].oMoveRight, NULL, &m_SaveButton, NULL, MENU_TYPE_TAB_NODE, SURFACE_CONFIG_INPUT);

    m_GameNew.SetPosition(m_GameTab.GetPosition() + m_GameTab.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,0.06f));
    m_GameNew.SetIndex   (NEW_CONFIG_GAME);

    m_MirrorModeNew.SetPosition(m_MirrorMode.GetPosition() + coreVector2(0.045f,0.0f));
    m_MirrorModeNew.SetIndex   (NEW_CONFIG_MIRRORMODE);

    // fill configuration entries
    const coreList<coreString>& asLanguageList = cMenu::GetLanguageList().get_keylist();

    for(coreUintW i = 0u, ie = asLanguageList.size(); i < ie; i++)
        m_Language.AddEntry(asLanguageList[i].c_str(), i);

    const coreUint8 iMaxSamples    = Core::Graphics->GetMaxSamples();
    const coreUint8 iMaxAnisotropy = Core::Graphics->GetMaxAnisotropy();

    m_DisplayMode    .AddEntryLanguage("DISPLAYMODE_WINDOW",     CORE_SYSTEM_MODE_WINDOWED);
    m_DisplayMode    .AddEntryLanguage("DISPLAYMODE_BORDERLESS", CORE_SYSTEM_MODE_BORDERLESS);
    m_DisplayMode    .AddEntryLanguage("DISPLAYMODE_FULLSCREEN", CORE_SYSTEM_MODE_FULLSCREEN);
    m_Vsync          .AddEntryLanguage("VALUE_OFF",              0u);
    m_Vsync          .AddEntryLanguage("VALUE_AUTO",             1u);
    m_AntiAliasing   .AddEntryLanguage("VALUE_OFF",              0u);
         if(CORE_GL_SUPPORT(INTEL_framebuffer_CMAA))               for(coreUintW i = 2u, ie = iMaxSamples; i <= ie; i <<= 1u) m_AntiAliasing.AddEntry(PRINT("%zux (%s)",   i, (i == 1u)                       ? "CMAA" : "MSAA"), i);
    else if(CORE_GL_SUPPORT(AMD_framebuffer_multisample_advanced)) for(coreUintW i = 2u, ie = iMaxSamples; i <= ie; i <<= 1u) m_AntiAliasing.AddEntry(PRINT("%zux (%s)",   i, (i == 2u || i == 4u || i == 8u) ? "EQAA" : "MSAA"), i);
    else if(CORE_GL_SUPPORT(NV_framebuffer_multisample_coverage))  for(coreUintW i = 2u, ie = iMaxSamples; i <= ie; i <<= 1u) m_AntiAliasing.AddEntry(PRINT("%zux (%s)",   i, (i == 4u || i == 8u)            ? "CSAA" : "MSAA"), i);
    else                                                           for(coreUintW i = 2u, ie = iMaxSamples; i <= ie; i <<= 1u) m_AntiAliasing.AddEntry(PRINT("%zux (MSAA)", i),                                                    i);
    if(m_AntiAliasing.GetValue(m_AntiAliasing.GetNumEntries() - 1u) != iMaxSamples) m_AntiAliasing.AddEntry(PRINT("%ux (MSAA)", iMaxSamples), iMaxSamples);   // possible 6x
    m_TextureFilter  .AddEntryLanguage("VALUE_OFF",              0u);
    for(coreUintW i = 2u, ie = iMaxAnisotropy; i <= ie; i <<= 1u) m_TextureFilter.AddEntry(PRINT("%zux", i), i);
    m_RenderQuality  .AddEntryLanguage("VALUE_LOW",              0u);
    m_RenderQuality  .AddEntryLanguage("VALUE_HIGH",             1u);
    if(cShadow::IsSupported())
    {
        m_ShadowQuality.AddEntryLanguage("VALUE_LOW",  1u);
        m_ShadowQuality.AddEntryLanguage("VALUE_HIGH", 2u);
    }
    else
    {
        m_ShadowQuality.AddEntryLanguage("VALUE_OFF", 1u);
    }
    m_WaterQuality   .AddEntryLanguage("VALUE_LOW",              0u);
    m_WaterQuality   .AddEntryLanguage("VALUE_HIGH",             1u);
    for(coreUintW i = 0u; i <= 200u; i += 5u) m_ParticleEffects.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 200u; i += 5u) m_ShakeEffects   .AddEntry(PRINT("%zu%%", i), i);
    m_FlashEffects   .AddEntryLanguage("VALUE_OFF",              0u);
    m_FlashEffects   .AddEntryLanguage("VALUE_ON",               1u);
    m_HitStopEffects .AddEntryLanguage("VALUE_OFF",              0u);
    m_HitStopEffects .AddEntryLanguage("VALUE_ON",               1u);
    m_ChromaEffects  .AddEntryLanguage("VALUE_OFF",              0u);
    m_ChromaEffects  .AddEntryLanguage("VALUE_ON",               1u);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_GlobalVolume   .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_MusicVolume    .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_EffectVolume   .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_AmbientVolume  .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_MenuVolume     .AddEntry(PRINT("%zu%%", i), i);
    m_AudioQuality   .AddEntryLanguage("VALUE_LOW",              0u);
    m_AudioQuality   .AddEntryLanguage("VALUE_HIGH",             1u);
    m_AudioMode      .AddEntryLanguage("VALUE_AUTO",             CORE_AUDIO_MODE_AUTO);
    m_AudioMode      .AddEntryLanguage("AUDIOMODE_MONO",         CORE_AUDIO_MODE_MONO);
    m_AudioMode      .AddEntryLanguage("AUDIOMODE_SPEAKERS",     CORE_AUDIO_MODE_SPEAKERS);
    m_AudioMode      .AddEntryLanguage("AUDIOMODE_HEADPHONES",   CORE_AUDIO_MODE_HEADPHONES);
    m_3DSound        .AddEntryLanguage("VALUE_OFF",              0u);
    m_3DSound        .AddEntryLanguage("VALUE_ON",               1u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oRumble     .AddEntryLanguage("VALUE_OFF",             0u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oRumble     .AddEntryLanguage("VALUE_ON",              10u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oFireMode   .AddEntryLanguage("FIREMODE_NORMAL",       0u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oFireMode   .AddEntryLanguage("FIREMODE_INVERT",       1u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oFireMode   .AddEntryLanguage("FIREMODE_TOGGLE",       2u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oControlMode.AddEntryLanguage("CONTROLMODE_TWINSTICK", 1u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oControlMode.AddEntryLanguage("CONTROLMODE_ORIGINAL",  0u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oControlMode.AddEntryLanguage("CONTROLMODE_MIXED",     2u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oControlMode.AddEntryLanguage("CONTROLMODE_SINGLE",    3u);
    m_GameDirection  .AddEntryLanguage("VALUE_OFF",              0u);
    m_GameDirection  .AddEntryLanguage("HUDDIRECTION_RIGHT",     1u);
    m_GameDirection  .AddEntryLanguage("HUDDIRECTION_UPSIDE",    2u);
    m_GameDirection  .AddEntryLanguage("HUDDIRECTION_LEFT",      3u);
    m_HudDirection   .AddEntryLanguage("VALUE_OFF",              0u);
    m_HudDirection   .AddEntryLanguage("HUDDIRECTION_RIGHT",     1u);
    m_HudDirection   .AddEntryLanguage("HUDDIRECTION_UPSIDE",    2u);
    m_HudDirection   .AddEntryLanguage("HUDDIRECTION_LEFT",      3u);
    m_HudType        .AddEntryLanguage("HUDTYPE_OUTSIDE",        0u);
    m_HudType        .AddEntryLanguage("HUDTYPE_BORDER",         2u);
    m_HudType        .AddEntryLanguage("HUDTYPE_INSIDE",         1u);
    m_CombatText     .AddEntryLanguage("VALUE_OFF",              0u);
    m_CombatText     .AddEntryLanguage("VALUE_ON",               0xFFu);
    m_BackRotation   .AddEntryLanguage("VALUE_OFF",              0u);
    m_BackRotation   .AddEntryLanguage("VALUE_ON",               1u);
    for(coreUintW i = 50u; i <= 200u; i += 5u) m_BackSpeed.AddEntry(PRINT("%zu%%", i), i);
    m_PureMode       .AddEntryLanguage("VALUE_OFF",              0u);
    m_PureMode       .AddEntryLanguage("VALUE_ON",               1u);
    m_Leaderboard    .AddEntryLanguage("VALUE_OFF",              0u);
    m_Leaderboard    .AddEntryLanguage("VALUE_ON",               1u);
    m_Version        .AddEntry        ("1.0.0",                  1u);
    m_Version        .AddEntryLanguage("VERSION_LATEST",         0u);   // TODO 1: "Latest (1.2.0)"

    // 
    m_Navigator.BindObject(&m_VideoTab, &m_SaveButton, &m_GameTab,  &m_Monitor, &m_AudioTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_AudioTab, &m_SaveButton, &m_VideoTab, &m_Monitor, &m_InputTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_InputTab, &m_SaveButton, &m_AudioTab, &m_Monitor, &m_GameTab,  MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_GameTab,  &m_SaveButton, &m_InputTab, &m_Monitor, &m_VideoTab, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);

    m_Navigator.BindObject(&m_SaveButton,    &m_Version, &m_BackButton,   NULL, &m_DiscardButton, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_DiscardButton, &m_Version, &m_SaveButton,   NULL, &m_InputButton,   MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_InputButton,   &m_Version, &m_DiscardButton,NULL, &m_BackButton,    MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_BackButton,    &m_Version, &m_InputButton,  NULL, &m_SaveButton,    MENU_TYPE_DEFAULT);

    m_Navigator.BindSurface(&m_VideoTab, SURFACE_CONFIG_VIDEO, &m_ChromaEffects, NULL, &m_Monitor,         NULL);
    m_Navigator.BindSurface(&m_AudioTab, SURFACE_CONFIG_AUDIO, &m_3DSound,       NULL, &m_GlobalVolume,    NULL);
    m_Navigator.BindSurface(&m_InputTab, SURFACE_CONFIG_INPUT, &m_SwapInput,     NULL, DEFINED(_CORE_SWITCH_) ? &m_aInput[0].oRumble : &m_aInput[0].oType, NULL);
    m_Navigator.BindSurface(&m_GameTab,  SURFACE_CONFIG_GAME,  &m_Version,       NULL, &m_Language,        NULL);

    m_Navigator.BindScroll(&m_VideoBox);
    m_Navigator.BindScroll(&m_InputBox);

    m_Navigator.AssignFirst(DEFINED(_CORE_EMSCRIPTEN_) ? &m_TextureFilter : (DEFINED(_CORE_SWITCH_) ? &m_ParticleEffects : &m_Monitor));
    m_Navigator.AssignBack (&m_BackButton);
    m_Navigator.AssignMenu (this);

    const auto nTabFunc = [this](coreObject2D* pObject, const coreUint8 iPack)
    {
        if(iPack != 4u) return;
        switch(this->GetCurSurface())
        {
        default: UNREACHABLE
        case SURFACE_CONFIG_VIDEO: m_Navigator.RebindDown(pObject, &m_VideoTab); break;
        case SURFACE_CONFIG_AUDIO: m_Navigator.RebindDown(pObject, &m_AudioTab); break;
        case SURFACE_CONFIG_INPUT: m_Navigator.RebindDown(pObject, &m_InputTab); break;
        case SURFACE_CONFIG_GAME:  m_Navigator.RebindDown(pObject, &m_GameTab);  break;
        }
    };
    m_Navigator.BindDynamic(&m_SaveButton,    nTabFunc);
    m_Navigator.BindDynamic(&m_DiscardButton, nTabFunc);
    m_Navigator.BindDynamic(&m_InputButton,   nTabFunc);
    m_Navigator.BindDynamic(&m_BackButton,    nTabFunc);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_CONFIG_MAX; ++i)
    {
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

        #if defined(_CORE_SWITCH_) || defined(_CORE_DEBUG_)
            this->BindObject(i, &m_InputButton);
        #endif

        this->BindObject(i, &m_GameNew);
        this->BindObject(i, &m_Description);
    }

    coreUintW iIndex;
    for(iIndex = 0u; iIndex < ENTRY_VIDEO; ++iIndex) {}//{if(!aiSkip.count(iIndex)) this->BindObject(SURFACE_CONFIG_VIDEO, &m_aLine [iIndex]);}
    for(;            iIndex < ENTRY_AUDIO; ++iIndex) {if(!aiSkip.count(iIndex)) this->BindObject(SURFACE_CONFIG_AUDIO, &m_aLine [iIndex]);}
    for(;            iIndex < ENTRY_INPUT; ++iIndex) {}//{if(!aiSkip.count(iIndex) && (iIndex < ENTRY_INPUT_MOVEUP)) this->BindObject(SURFACE_CONFIG_INPUT, &m_aLine [iIndex]);}
    for(;            iIndex < ENTRY_MAX;   ++iIndex) {if(!aiSkip.count(iIndex)) this->BindObject(SURFACE_CONFIG_GAME,  &m_aLine [iIndex]);}
    for(iIndex = 0u; iIndex < ENTRY_VIDEO; ++iIndex) {}//{if(!aiSkip.count(iIndex)) this->BindObject(SURFACE_CONFIG_VIDEO, &m_aLabel[iIndex]);}
    for(;            iIndex < ENTRY_AUDIO; ++iIndex) {if(!aiSkip.count(iIndex)) this->BindObject(SURFACE_CONFIG_AUDIO, &m_aLabel[iIndex]);}
    for(;            iIndex < ENTRY_INPUT; ++iIndex) {}//{if(!aiSkip.count(iIndex) && (iIndex < ENTRY_INPUT_MOVEUP)) this->BindObject(SURFACE_CONFIG_INPUT, &m_aLabel[iIndex]);}
    for(;            iIndex < ENTRY_MAX;   ++iIndex) {if(!aiSkip.count(iIndex)) this->BindObject(SURFACE_CONFIG_GAME,  &m_aLabel[iIndex]);}

    if(!m_GlobalVolume   .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_GlobalVolume);
    if(!m_MusicVolume    .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_MusicVolume);
    if(!m_EffectVolume   .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_EffectVolume);
    if(!m_AmbientVolume  .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_AmbientVolume);
    if(!m_MenuVolume     .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_MenuVolume);
    if(!m_AudioQuality   .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_AudioQuality);
    if(!m_AudioMode      .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_AudioMode);
    if(!m_3DSound        .GetStatus()) this->BindObject(SURFACE_CONFIG_AUDIO, &m_3DSound);
    if(!m_Language       .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_Language);
    if(!m_GameDirection  .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_GameDirection);
    if(!m_MirrorMode     .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_MirrorMode);
    if(!m_HudDirection   .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_HudDirection);
    if(!m_HudType        .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_HudType);
    if(!m_CombatText     .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_CombatText);
    if(!m_BackRotation   .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_BackRotation);
    if(!m_BackSpeed      .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_BackSpeed);
    if(!m_UpdateFreq     .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_UpdateFreq);
    if(!m_PureMode       .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_PureMode);
    if(!m_Leaderboard    .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_Leaderboard);
    if(!m_Version        .GetStatus()) this->BindObject(SURFACE_CONFIG_GAME,  &m_Version);

    this->BindObject(SURFACE_CONFIG_VIDEO, &m_VideoBox);

    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oHeader);
    this->BindObject(SURFACE_CONFIG_INPUT, &m_InputBox);
#if !defined(_CORE_SWITCH_)
    this->BindObject(SURFACE_CONFIG_INPUT, &m_SwapInput);
#endif

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueRota); ++i) this->BindObject(SURFACE_CONFIG_GAME, &m_aCueRota[i]);
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueLock); ++i) this->BindObject(SURFACE_CONFIG_GAME, &m_aCueLock[i]);
    this->BindObject(SURFACE_CONFIG_GAME, &m_MirrorModeNew);

    for(coreUintW i = 0u; i < SURFACE_CONFIG_MAX; ++i) this->BindObject(i, &m_Navigator);
}


// ****************************************************************
// move the config menu
void cConfigMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
         if(m_VideoTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_CONFIG_VIDEO);
    else if(m_AudioTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_CONFIG_AUDIO);
    else if(m_InputTab.IsClicked()) cMenu::ChangeTab(this, SURFACE_CONFIG_INPUT);
    else if(m_GameTab .IsClicked()) cMenu::ChangeTab(this, SURFACE_CONFIG_GAME);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_CONFIG_VIDEO:
        {
            // 
            if(m_Monitor.GetUserSwitch())
            {
                this->__LoadResolutions(m_Monitor.GetCurValue());
                this->__LoadFrequencies(m_Monitor.GetCurValue());
            }

            // 
            if(m_RenderQuality.GetUserSwitch())
                this->__UpdateRenderQuality();

            // 
            if(m_ShadowQuality.GetUserSwitch())
                this->__UpdateShadowQuality();

            // 
            if(m_WaterQuality.GetUserSwitch())
                this->__UpdateWaterQuality();

            // 
            cMenu::UpdateSwitchBox(&m_Monitor);
            cMenu::UpdateSwitchBox(&m_Resolution);
            cMenu::UpdateSwitchBox(&m_DisplayMode);
            cMenu::UpdateSwitchBox(&m_Vsync);
            cMenu::UpdateSwitchBox(&m_AntiAliasing);
            cMenu::UpdateSwitchBox(&m_TextureFilter);
            cMenu::UpdateSwitchBox(&m_RenderQuality);
            cMenu::UpdateSwitchBox(&m_ShadowQuality);
            cMenu::UpdateSwitchBox(&m_WaterQuality);
            cMenu::UpdateSwitchBox(&m_ParticleEffects);
            cMenu::UpdateSwitchBox(&m_ShakeEffects);
            cMenu::UpdateSwitchBox(&m_FlashEffects);
            cMenu::UpdateSwitchBox(&m_HitStopEffects);
            cMenu::UpdateSwitchBox(&m_ChromaEffects);

            // 
                 if(m_RenderQuality.GetCurValue() == 0u) m_RenderQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_RenderQuality.GetCurValue() == 1u) m_RenderQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
                 if(m_ShadowQuality.GetCurValue() == 1u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_ShadowQuality.GetCurValue() == 2u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
                 if(m_WaterQuality .GetCurValue() == 0u) m_WaterQuality .GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_WaterQuality .GetCurValue() == 1u) m_WaterQuality .GetCaption()->SetColor3(COLOR_MENU_GREEN);
        }
        break;

    case SURFACE_CONFIG_AUDIO:
        {
            // 
            if(m_GlobalVolume .GetUserSwitch() ||
               m_MusicVolume  .GetUserSwitch() ||
               m_EffectVolume .GetUserSwitch() ||
               m_AmbientVolume.GetUserSwitch() ||
               m_MenuVolume   .GetUserSwitch())
                this->__UpdateVolume();

            // 
            if(m_EffectVolume.GetUserSwitch()) g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 1.0f, 1.0f, SOUND_BULLET_HIT);

            // 
            cMenu::UpdateSwitchBox(&m_GlobalVolume);
            cMenu::UpdateSwitchBox(&m_MusicVolume);
            cMenu::UpdateSwitchBox(&m_EffectVolume, false);
            cMenu::UpdateSwitchBox(&m_AmbientVolume);
            cMenu::UpdateSwitchBox(&m_MenuVolume);
            cMenu::UpdateSwitchBox(&m_AudioQuality);
            cMenu::UpdateSwitchBox(&m_AudioMode);
            cMenu::UpdateSwitchBox(&m_3DSound);

            // 
            const auto nColorFunc = [](const coreUint8 iValue) {return TernaryLerp(COLOR_MENU_RED, COLOR_MENU_YELLOW, COLOR_MENU_GREEN, cConfigMenu::__VolumeToFloat(iValue));};
            m_GlobalVolume .GetCaption()->SetColor3(nColorFunc(m_GlobalVolume .GetCurValue()));
            m_MusicVolume  .GetCaption()->SetColor3(nColorFunc(m_MusicVolume  .GetCurValue()));
            m_EffectVolume .GetCaption()->SetColor3(nColorFunc(m_EffectVolume .GetCurValue()));
            m_AmbientVolume.GetCaption()->SetColor3(nColorFunc(m_AmbientVolume.GetCurValue()));
            m_MenuVolume   .GetCaption()->SetColor3(nColorFunc(m_MenuVolume   .GetCurValue()));

            // 
                 if(m_AudioQuality.GetCurValue() == 0u) m_AudioQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_AudioQuality.GetCurValue() == 1u) m_AudioQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
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
                this->__RefreshManual();
            }
            STATIC_ASSERT(MENU_CONFIG_INPUTS == 2u)

            // 
            cMenu::UpdateButton(&m_SwapInput, &m_Navigator, m_SwapInput.IsFocused());

            for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
            {
                sPlayerInput& oInput = m_aInput[i];

                // 
                if(oInput.oType.GetUserSwitch())
                {
                    for(coreUintW j = 0u, je = oInput.oType.GetNumEntries(); j < je; ++j)
                    {
                        coreBool bSkip = false;

                        // skip joystick/gamepad input sets without available device
                        if((oInput.oType.GetCurValue() >= INPUT_SETS_KEYBOARD) && (oInput.oType.GetCurValue() - INPUT_SETS_KEYBOARD >= Core::Input->GetJoystickNum()))
                        {
                            bSkip = true;
                        }

                        // skip already assigned input sets
                        for(coreUintW k = 0u; k < MENU_CONFIG_INPUTS; ++k)
                        {
                            if((g_CurConfig.Input.aiType[k] == oInput.oType.GetCurValue()) && (k != i))
                            {
                                bSkip = true;
                                break;
                            }
                        }

                        // 
                        if(bSkip)
                        {
                                 if(oInput.oType.GetUserSwitch() < 0) oInput.oType.Previous();
                            else if(oInput.oType.GetUserSwitch() > 0) oInput.oType.Next();
                        }
                        else break;
                    }

                    // 
                    g_CurConfig.Input.aiType[i] = oInput.oType.GetCurValue();
                    this->__LoadInputs();
                    this->__RefreshManual();
                }

                // 
                if(oInput.oType.IsFocused() && (oInput.oType.GetCurValue() >= INPUT_SETS_KEYBOARD))
                {
                    // TODO 1: show gamepad name somehow
                    // Core::Input->GetJoystickName(oInput.oType.GetCurValue() - INPUT_SETS_KEYBOARD)
                }

                // 
                if(oInput.oRumble     .GetUserSwitch()) g_CurConfig.Input.aiRumble     [i] = oInput.oRumble     .GetCurValue();
                if(oInput.oFireMode   .GetUserSwitch()) g_CurConfig.Input.aiFireMode   [i] = oInput.oFireMode   .GetCurValue();
                if(oInput.oControlMode.GetUserSwitch()) g_CurConfig.Input.aiControlMode[i] = oInput.oControlMode.GetCurValue();

                // 
                if(oInput.oControlMode.GetUserSwitch())
                {
                    this->__LoadInputs();
                    this->__RefreshManual();
                }

                // 
                cMenu::UpdateSwitchBox(&oInput.oType);
                cMenu::UpdateSwitchBox(&oInput.oRumble);
                cMenu::UpdateSwitchBox(&oInput.oFireMode);
                cMenu::UpdateSwitchBox(&oInput.oControlMode);

                for(coreUintW j = 0u; j < INPUT_KEYS; ++j)
                {
                    cGuiButton& oButton   = this->__RetrieveInputButton  (i, j);
                    cFigure&    oFigure   = this->__RetrieveInputFigure  (i, j);
                    coreInt16&  iCurValue = this->__RetrieveInputCurValue(i, j);

                    if(oButton.IsClicked())
                    {
                        constexpr coreUintW aiText[] =
                        {
                            ENTRY_INPUT_MOVEUP,
                            ENTRY_INPUT_MOVELEFT,
                            ENTRY_INPUT_MOVEDOWN,
                            ENTRY_INPUT_MOVERIGHT,
                            ENTRY_INPUT_ACTION1,
                            ENTRY_INPUT_ACTION2,
                            ENTRY_INPUT_ACTION3,
                            ENTRY_INPUT_ACTION4,
                            ENTRY_INPUT_ACTION5,
                            ENTRY_INPUT_ACTION6,
                            ENTRY_INPUT_ACTION7,
                            ENTRY_INPUT_ACTION8,
                            ENTRY_INPUT_ACTION9,
                            ENTRY_INPUT_ACTION10
                        };
                        STATIC_ASSERT(ARRAY_SIZE(aiText) == INPUT_KEYS)

                        const coreChar*  pcText = PRINT("%s [%s]", Core::Language->GetString(DEFINED(_CORE_SWITCH_) ? "MAPPING_SWITCH" : "MAPPING"), m_aLabel[aiText[j]].GetText());
                        const coreUint8& iType  = oInput.oType.GetCurValue();   // # referenced in lambda

                        // 
                        g_pMenu->GetMsgBox()->ShowMapping(pcText, iType, [&, j](const coreInt32 iAnswer, const coreInt16 iKey)
                        {
                            if(iAnswer != MSGBOX_ANSWER_KEY) return;
                            
                            if(iKey != INPUT_KEY_INVALID)
                            {
                            coreBool bReplace = false;

                            // check if new key is already assigned somewhere else
                            for(coreUintW k = 0u; k < INPUT_SETS; ++k)
                            {
                                for(coreUintW m = 0u; m < INPUT_KEYS; ++m)
                                {
                                    coreInt16& iOtherCurValue = this->__RetrieveInputDirValue(k, m);
                                    if(&iOtherCurValue == &iCurValue) continue;

                                    // ignore locked buttons
                                    const coreBool bKeyboard = (k < INPUT_SETS_KEYBOARD);
                                    if(!bKeyboard && (m ==  0u)) continue;
                                    if(!bKeyboard && (m ==  1u)) continue;
                                    if(!bKeyboard && (m ==  2u)) continue;
                                    if(!bKeyboard && (m ==  3u)) continue;
                                    if( bKeyboard && (m == 11u)) continue;

                                    // find same value and same input set type
                                    if(((k < INPUT_SETS_KEYBOARD) && (iType < INPUT_SETS_KEYBOARD)) || (k == iType))
                                    {
                                        // update related button if currently visible
                                        const auto nUpdateFunc = [&]()
                                        {
                                            for(coreUintW n = 0u; n < MENU_CONFIG_INPUTS; ++n)
                                            {
                                                if(g_CurConfig.Input.aiType[n] == k)
                                                {
                                                    cFigure& oOtherFigure = this->__RetrieveInputFigure(n, m);
                                                    cConfigMenu::PrintFigure(&oOtherFigure, k, iOtherCurValue);
                                                    break;
                                                }
                                            }
                                        };

                                        if(iOtherCurValue == iKey)
                                        {
                                            // 
                                            if(k == iType)
                                            {
                                            if((((j >= 4u) && (j <= 6u)) || (j == 12u)) && (((m >= 7u) && (m <= 10u)) || (m == 13u))) continue;
                                            if((((m >= 4u) && (m <= 6u)) || (m == 12u)) && (((j >= 7u) && (j <= 10u)) || (j == 13u))) continue;
                                            }

                                            // replace with old key
                                            iOtherCurValue = iCurValue;
                                            nUpdateFunc();

                                            if(k == iType)
                                            {
                                            if((((j >= 4u) && (j <=  6u)) || (j == 12u)) && (((m >= 4u) && (m <=  6u)) || (m == 12u))) continue;
                                            if((((m >= 7u) && (m <= 10u)) || (m == 13u)) && (((j >= 7u) && (j <= 10u)) || (j == 13u))) continue;
                                            }
                                            bReplace = true;
                                        }
                                        //else if(iOtherCurValue == iCurValue)
                                        //{
                                        //    // replace with new key
                                        //    iOtherCurValue = iKey;
                                        //    nUpdateFunc();
                                        //}
                                    }
                                }
                            }
                            
                            if(bReplace)
                            {
                            // check if new key is already assigned somewhere else
                            for(coreUintW k = 0u; k < INPUT_SETS; ++k)
                            {
                                for(coreUintW m = 0u; m < INPUT_KEYS; ++m)
                                {
                                    coreInt16& iOtherCurValue = this->__RetrieveInputDirValue(k, m);
                                    if(&iOtherCurValue == &iCurValue) continue;

                                    // ignore locked buttons
                                    const coreBool bKeyboard = (k < INPUT_SETS_KEYBOARD);
                                    if(!bKeyboard && (m ==  0u)) continue;
                                    if(!bKeyboard && (m ==  1u)) continue;
                                    if(!bKeyboard && (m ==  2u)) continue;
                                    if(!bKeyboard && (m ==  3u)) continue;
                                    if( bKeyboard && (m == 11u)) continue;

                                    // find same value and same input set type
                                    if(k == iType)
                                    {
                                        // update related button if currently visible
                                        const auto nUpdateFunc = [&]()
                                        {
                                            for(coreUintW n = 0u; n < MENU_CONFIG_INPUTS; ++n)
                                            {
                                                if(g_CurConfig.Input.aiType[n] == k)
                                                {
                                                    cFigure& oOtherFigure = this->__RetrieveInputFigure(n, m);
                                                    cConfigMenu::PrintFigure(&oOtherFigure, k, iOtherCurValue);
                                                    break;
                                                }
                                            }
                                        };

                                        if(iOtherCurValue == iCurValue)
                                        {
                                            if(!((((j >= 4u) && (j <= 6u)) || (j == 12u)) && (((m >= 7u) && (m <= 10u)) || (m == 13u))) && !((((m >= 4u) && (m <= 6u)) || (m == 12u)) && (((j >= 7u) && (j <= 10u)) || (j == 13u)))) continue;

                                            // replace with new key
                                            iOtherCurValue = iKey;
                                            nUpdateFunc();
                                        }
                                    }
                                }
                            }
                            }
                            }

                            // 
                            iCurValue = iKey;
                            cConfigMenu::PrintFigure(&oFigure, iType, iCurValue);

                            // 
                            this->CheckValues();
                        });
                    }

                    // 
                    cMenu::UpdateButton(&oButton, &m_Navigator, oButton.IsFocused());
                    oButton.SetAlpha(oButton.GetAlpha() * (oButton.IsFocused() ? 1.0f : 0.75f));
                    oFigure.SetColor3(oButton.IsFocused() ? g_pMenu->GetButtonColor() : coreVector3(1.0f,1.0f,1.0f));
                }
            }
        }
        break;

    case SURFACE_CONFIG_GAME:
        {
            // 
            if(m_Language.GetUserSwitch())
                this->__UpdateLanguage();

            // 
            if(m_BackSpeed.GetUserSwitch())
                this->__UpdateBackSpeed();

            // 
            if(m_BackRotation.GetUserSwitch() ||
               m_BackSpeed   .GetUserSwitch() ||
               m_UpdateFreq  .GetUserSwitch())
                this->__UpdatePureModeBase();

            // 
            if(m_PureMode.GetUserSwitch())
                this->__UpdatePureMode();

            // 
            if(m_GameDirection.GetUserSwitch() ||
               m_MirrorMode   .GetUserSwitch() ||
               m_HudDirection .GetUserSwitch() ||
               m_HudType      .GetUserSwitch())
                this->__UpdateInterface();

            // 
            m_aCueRota[0].SetDirection(MapToAxisInv(g_pPostProcessing->GetDirection(), g_vHudDirection));

            // 
            m_BackRotation.SetOverride((STATIC_ISVALID(g_pGame) || g_CurConfig.Game.iPureMode) ? -1 : 0);
            m_BackSpeed   .SetOverride((STATIC_ISVALID(g_pGame) || g_CurConfig.Game.iPureMode) ? -1 : 0);
            m_UpdateFreq  .SetOverride((STATIC_ISVALID(g_pGame) || g_CurConfig.Game.iPureMode) ? -1 : 0);
            m_PureMode    .SetOverride((STATIC_ISVALID(g_pGame))                               ? -1 : 0);
            m_Version     .SetOverride((STATIC_ISVALID(g_pGame))                               ? -1 : 0);

            // 
            m_aCueLock[0].SetEnabled((STATIC_ISVALID(g_pGame) || g_CurConfig.Game.iPureMode) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aCueLock[1].SetEnabled((STATIC_ISVALID(g_pGame) || g_CurConfig.Game.iPureMode) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aCueLock[2].SetEnabled((STATIC_ISVALID(g_pGame) || g_CurConfig.Game.iPureMode) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aCueLock[3].SetEnabled((STATIC_ISVALID(g_pGame))                               ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
            m_aCueLock[4].SetEnabled((STATIC_ISVALID(g_pGame))                               ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

            // 
            cMenu::UpdateSwitchBox(&m_Language);
            cMenu::UpdateSwitchBox(&m_GameDirection);
            cMenu::UpdateSwitchBox(&m_MirrorMode);
            cMenu::UpdateSwitchBox(&m_HudDirection);
            cMenu::UpdateSwitchBox(&m_HudType);
            cMenu::UpdateSwitchBox(&m_CombatText);
            cMenu::UpdateSwitchBox(&m_BackRotation);
            cMenu::UpdateSwitchBox(&m_BackSpeed);
            cMenu::UpdateSwitchBox(&m_UpdateFreq);
            cMenu::UpdateSwitchBox(&m_PureMode);
            cMenu::UpdateSwitchBox(&m_Leaderboard);
            cMenu::UpdateSwitchBox(&m_Version);

            // 
            m_GameNew      .Release();
            m_MirrorModeNew.Release();
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    if(m_SaveButton.IsClicked())
    {
        // 
        this->SaveValues();
    }
    else if(m_DiscardButton.IsClicked() || Core::System->GetTerminated())
    {
        // 
        this->LoadValues();
    }
    else if(m_InputButton.IsClicked())
    {
        // 
        this->__SetupInput();
    }
    else if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
    {
        if(m_SaveButton.GetOverride() >= 0)
        {
            // 
            g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_CONFIG_SAVE"), [this](const coreInt32 iAnswer)
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

        // 
        m_GameNew      .Resolve();
        m_MirrorModeNew.Resolve();
    }

    // (# after interaction, before visual update) 
    if(Core::Input->GetAnyButton(CORE_INPUT_HOLD) || Core::System->GetWinPosChanged() || Core::System->GetWinSizeChanged())
        this->CheckValues();

    // 
    cMenu::UpdateTab(&m_VideoTab, (this->GetCurSurface() == SURFACE_CONFIG_VIDEO), m_VideoTab.IsFocused());
    cMenu::UpdateTab(&m_AudioTab, (this->GetCurSurface() == SURFACE_CONFIG_AUDIO), m_AudioTab.IsFocused());
    cMenu::UpdateTab(&m_InputTab, (this->GetCurSurface() == SURFACE_CONFIG_INPUT), m_InputTab.IsFocused());
    cMenu::UpdateTab(&m_GameTab,  (this->GetCurSurface() == SURFACE_CONFIG_GAME),  m_GameTab .IsFocused());

    // 
    cMenu::UpdateButton(&m_SaveButton,    &m_Navigator, m_SaveButton   .IsFocused());
    cMenu::UpdateButton(&m_DiscardButton, &m_Navigator, m_DiscardButton.IsFocused());
    cMenu::UpdateButton(&m_InputButton,   &m_Navigator, m_InputButton  .IsFocused());
    cMenu::UpdateButton(&m_BackButton,    &m_Navigator, m_BackButton   .IsFocused());

    
    m_Description.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_Description.SetColor3(g_pMenu->GetHighlightColor());

    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        const coreBool bInteract = m_VideoBox.ContainsObject(&m_aLine[i]) ? m_VideoBox.IsFocused() :
                                   m_InputBox.ContainsObject(&m_aLine[i]) ? m_InputBox.IsFocused() : true;

        cMenu::UpdateLine(&m_aLine[i], bInteract);

        if(m_aLine[i].IsFocused() && TIME)
        {
            m_Description.SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_Description.SetTextLanguage(m_apcDescKey[i]);

            if((i == ENTRY_GAME_MIRRORMODE) && (m_MirrorMode.GetOverride() < 0))
                m_Description.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    }


    if(m_iJoystickNum != Core::Input->GetJoystickNum())
    {
        if(m_SaveButton.GetOverride() < 0)
        {
            //this->LoadValues();
        }

    #if defined(_CORE_SWITCH_)
        this->__LoadInputs();
        this->__RefreshManual();
    #endif
    }
}


// ****************************************************************
// 
void cConfigMenu::CheckValues()
{
    const coreUint8   iCurMonitor    = m_Monitor   .GetCurValue();
    const coreUint8   iCurValue      = m_Resolution.GetCurValue();
    const coreVector2 vCurResolution = (iCurValue == 0xFFu) ? Core::System->GetResolution() : ((iCurValue == 0xEEu) ? coreVector2(0.0f,0.0f) : Core::System->GetDisplayData(iCurMonitor).avAvailableRes[iCurValue]);

    // 
    const coreBool bSave = (vCurResolution != coreVector2(I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH)), I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT)))) ||
                           (m_Monitor        .GetCurValue() !=  Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY))                                       ||
                           (m_DisplayMode    .GetCurValue() !=  Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))                                    ||
                           (m_Vsync          .GetCurValue() !=  Core::Config->GetInt(CORE_CONFIG_SYSTEM_VSYNC))                                         ||
                           (m_AntiAliasing   .GetCurValue() !=  Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING))                                ||
                           (m_TextureFilter  .GetCurValue() !=  Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY))                           ||
                           (m_RenderQuality  .GetCurValue() !=  g_OldConfig.Graphics.iRender)                                                           ||
                           (m_ShadowQuality  .GetCurValue() !=  g_OldConfig.Graphics.iShadow)                                                           ||
                           (m_WaterQuality   .GetCurValue() !=  g_OldConfig.Graphics.iReflection)                                                       ||
                           (m_ParticleEffects.GetCurValue() !=  g_OldConfig.Graphics.iParticle)                                                         ||
                           (m_ShakeEffects   .GetCurValue() !=  g_OldConfig.Graphics.iShake)                                                            ||
                           (m_FlashEffects   .GetCurValue() !=  g_OldConfig.Graphics.iFlash)                                                            ||
                           (m_HitStopEffects .GetCurValue() !=  g_OldConfig.Graphics.iHitStop)                                                          ||
                           (m_ChromaEffects  .GetCurValue() !=  g_OldConfig.Graphics.iChroma)                                                           ||
                           (m_GlobalVolume   .GetCurValue() !=  cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME)))   ||
                           (m_MusicVolume    .GetCurValue() !=  cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)))    ||
                           (m_EffectVolume   .GetCurValue() !=  cConfigMenu::__VolumeToUint8(g_OldConfig.Audio.fEffectVolume))                          ||
                           (m_AmbientVolume  .GetCurValue() !=  cConfigMenu::__VolumeToUint8(g_OldConfig.Audio.fAmbientVolume))                         ||
                           (m_MenuVolume     .GetCurValue() !=  cConfigMenu::__VolumeToUint8(g_OldConfig.Audio.fMenuVolume))                            ||
                           (m_AudioQuality   .GetCurValue() !=  g_OldConfig.Audio.iQuality)                                                             ||
                           (m_AudioMode      .GetCurValue() !=  Core::Config->GetInt(CORE_CONFIG_AUDIO_MODE))                                           ||
                           (m_3DSound        .GetCurValue() !=  g_OldConfig.Audio.i3DSound)                                                             ||
                           (std::strcmp(Core::Language->GetPath(), Core::Config->GetString(CORE_CONFIG_BASE_LANGUAGE)))                                 ||
                           (m_GameDirection  .GetCurValue() !=  g_OldConfig.Game.iGameDirection)                                                        ||
                           (m_MirrorMode     .GetCurValue() !=  g_OldConfig.Game.iMirrorMode)                                                           ||
                           (m_HudDirection   .GetCurValue() !=  g_OldConfig.Game.iHudDirection)                                                         ||
                           (m_HudType        .GetCurValue() !=  g_OldConfig.Game.iHudType)                                                              ||
                           (m_CombatText     .GetCurValue() !=  g_OldConfig.Game.iCombatText)                                                           ||
                           (m_BackRotation   .GetCurValue() != (g_OldConfig.Game.iPureMode ? SCORE_PURE_BACKROTATION : g_OldConfig.Game.iBackRotation)) ||
                           (m_BackSpeed      .GetCurValue() != (g_OldConfig.Game.iPureMode ? SCORE_PURE_BACKSPEED    : g_OldConfig.Game.iBackSpeed))    ||
                           (m_UpdateFreq     .GetCurValue() != (g_OldConfig.Game.iPureMode ? SCORE_PURE_UPDATEFREQ   : g_OldConfig.Game.iUpdateFreq))   ||
                           (m_PureMode       .GetCurValue() !=  g_OldConfig.Game.iPureMode)                                                             ||
                           (m_Leaderboard    .GetCurValue() !=  g_OldConfig.Game.iLeaderboard)                                                          ||
                           (m_Version        .GetCurValue() !=  g_OldConfig.Game.iVersion)                                                              ||
                           (std::memcmp(&g_CurConfig.Input, &g_OldConfig.Input, sizeof(sConfig::Input)));

    // 
    m_SaveButton   .SetOverride(bSave ? 0 : -1);
    m_DiscardButton.SetOverride(bSave ? 0 : -1);
}


// ****************************************************************
// 
void cConfigMenu::LoadValues()
{
    const coreUintW iRenderQualityIndex = m_RenderQuality.GetCurIndex();
    const coreUintW iShadowQualityIndex = m_ShadowQuality.GetCurIndex();
    const coreUintW iWaterQualityIndex  = m_WaterQuality .GetCurIndex();
    const coreUintW iLanguageIndex      = m_Language     .GetCurIndex();

    // 
    LoadConfig();
    this->__LoadMonitors();
    this->__LoadResolutions(Core::System->GetDisplayIndex());
    this->__LoadFrequencies(Core::System->GetDisplayIndex());
    this->__LoadInputs();
    this->__LoadUnlocks();

    // 
    if((Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH) <= 0) && (Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT) <= 0))
    {
        m_Resolution.SelectValue(0xEEu);
    }
    else if(!m_Resolution.SelectText(PRINT("%d x %d", Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH), Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT))))
    {
        m_Resolution.SelectValue(0xFFu);
    }

    // 
    m_Monitor        .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY));
    m_DisplayMode    .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN));
    m_Vsync          .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_VSYNC));
    m_AntiAliasing   .SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING));
    m_TextureFilter  .SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY));
    m_RenderQuality  .SelectValue(g_CurConfig.Graphics.iRender);
    m_ShadowQuality  .SelectValue(g_CurConfig.Graphics.iShadow);
    m_WaterQuality   .SelectValue(g_CurConfig.Graphics.iReflection);
    m_ParticleEffects.SelectValue(g_CurConfig.Graphics.iParticle);
    m_ShakeEffects   .SelectValue(g_CurConfig.Graphics.iShake);
    m_FlashEffects   .SelectValue(g_CurConfig.Graphics.iFlash);
    m_HitStopEffects .SelectValue(g_CurConfig.Graphics.iHitStop);
    m_ChromaEffects  .SelectValue(g_CurConfig.Graphics.iChroma);

    // 
    m_GlobalVolume .SelectValue(cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME)));
    m_MusicVolume  .SelectValue(cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)));
    m_EffectVolume .SelectValue(cConfigMenu::__VolumeToUint8(g_CurConfig.Audio.fEffectVolume));
    m_AmbientVolume.SelectValue(cConfigMenu::__VolumeToUint8(g_CurConfig.Audio.fAmbientVolume));
    m_MenuVolume   .SelectValue(cConfigMenu::__VolumeToUint8(g_CurConfig.Audio.fMenuVolume));
    m_AudioQuality .SelectValue(g_CurConfig.Audio.iQuality);
    m_AudioMode    .SelectValue(Core::Config->GetInt(CORE_CONFIG_AUDIO_MODE));
    m_3DSound      .SelectValue(g_CurConfig.Audio.i3DSound);

    // 
    const coreList<coreString>& asLanguageList = cMenu::GetLanguageList().get_valuelist();
    m_Language     .SelectIndex(asLanguageList.first_index(Core::Config->GetString(CORE_CONFIG_BASE_LANGUAGE)));
    m_GameDirection.SelectValue(g_CurConfig.Game.iGameDirection);
    m_MirrorMode   .SelectValue(g_CurConfig.Game.iMirrorMode);
    m_HudDirection .SelectValue(g_CurConfig.Game.iHudDirection);
    m_HudType      .SelectValue(g_CurConfig.Game.iHudType);
    m_CombatText   .SelectValue(g_CurConfig.Game.iCombatText);
    m_BackRotation .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_BACKROTATION : g_CurConfig.Game.iBackRotation);
    m_BackSpeed    .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_BACKSPEED    : g_CurConfig.Game.iBackSpeed);
    m_UpdateFreq   .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_UPDATEFREQ   : g_CurConfig.Game.iUpdateFreq);
    m_PureMode     .SelectValue(g_CurConfig.Game.iPureMode);
    m_Leaderboard  .SelectValue(g_CurConfig.Game.iLeaderboard);
    m_Version      .SelectValue(g_CurConfig.Game.iVersion);

    // 
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        m_aInput[i].oType       .SelectValue(g_CurConfig.Input.aiType       [i]);
        m_aInput[i].oRumble     .SelectValue(g_CurConfig.Input.aiRumble     [i]);
        m_aInput[i].oFireMode   .SelectValue(g_CurConfig.Input.aiFireMode   [i]);
        m_aInput[i].oControlMode.SelectValue(g_CurConfig.Input.aiControlMode[i]);
    }

    // 
    if(m_SaveButton.GetOverride() >= 0)
    {
        if(iRenderQualityIndex != m_RenderQuality.GetCurIndex()) this->__UpdateRenderQuality();
        if(iShadowQualityIndex != m_ShadowQuality.GetCurIndex()) this->__UpdateShadowQuality();
        if(iWaterQualityIndex  != m_WaterQuality .GetCurIndex()) this->__UpdateWaterQuality();
        if(iLanguageIndex      != m_Language     .GetCurIndex()) this->__UpdateLanguage();
        this->__UpdateVolume();
        this->__UpdateBackSpeed();
        this->__UpdateInterface();
    }

    // 
    this->__UpdatePureMode();

    // 
    m_SaveButton   .SetOverride(-1);
    m_DiscardButton.SetOverride(-1);
}


// ****************************************************************
// 
void cConfigMenu::SaveValues()
{
    const coreUint8   iCurMonitor    = m_Monitor   .GetCurValue();
    const coreUint8   iCurValue      = m_Resolution.GetCurValue();
    const coreVector2 vCurResolution = (iCurValue == 0xFFu) ? Core::System->GetResolution() : ((iCurValue == 0xEEu) ? coreVector2(0.0f,0.0f) : Core::System->GetDisplayData(iCurMonitor).avAvailableRes[iCurValue]);

    // 
    const coreBool bReset   = (m_AntiAliasing .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING));
    const coreBool bReshape = (vCurResolution != coreVector2(I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH)), I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT)))) ||
                              (m_Monitor      .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY))             ||
                              (m_DisplayMode  .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))          ||
                              (m_RenderQuality.GetCurValue() != g_CurConfig.Graphics.iRender);
    const coreBool bManager = (m_TextureFilter.GetCurValue() != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY)) ||
                              (m_RenderQuality.GetCurValue() != g_CurConfig.Graphics.iRender);
    const coreBool bAudio   = (m_AudioQuality .GetCurValue() != g_CurConfig.Audio.iQuality)                                   ||
                              (m_AudioMode    .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_AUDIO_MODE));

    // 
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_WIDTH,               F_TO_SI(vCurResolution.x));
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_HEIGHT,              F_TO_SI(vCurResolution.y));
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_DISPLAY,             m_Monitor      .GetCurValue());
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_FULLSCREEN,          m_DisplayMode  .GetCurValue());
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_VSYNC,               m_Vsync        .GetCurValue());
    Core::Config->SetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING,      m_AntiAliasing .GetCurValue());
    Core::Config->SetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY, m_TextureFilter.GetCurValue());
    g_CurConfig.Graphics.iRender     = m_RenderQuality  .GetCurValue();
    g_CurConfig.Graphics.iShadow     = m_ShadowQuality  .GetCurValue();
    g_CurConfig.Graphics.iReflection = m_WaterQuality   .GetCurValue();
    g_CurConfig.Graphics.iParticle   = m_ParticleEffects.GetCurValue();
    g_CurConfig.Graphics.iShake      = m_ShakeEffects   .GetCurValue();
    g_CurConfig.Graphics.iFlash      = m_FlashEffects   .GetCurValue();
    g_CurConfig.Graphics.iHitStop    = m_HitStopEffects .GetCurValue();
    g_CurConfig.Graphics.iChroma     = m_ChromaEffects  .GetCurValue();

    // 
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME, cConfigMenu::__VolumeToFloat(m_GlobalVolume.GetCurValue()));
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME,  cConfigMenu::__VolumeToFloat(m_MusicVolume .GetCurValue()));
    g_CurConfig.Audio.fEffectVolume  = cConfigMenu::__VolumeToFloat(m_EffectVolume .GetCurValue());
    g_CurConfig.Audio.fAmbientVolume = cConfigMenu::__VolumeToFloat(m_AmbientVolume.GetCurValue());
    g_CurConfig.Audio.fMenuVolume    = cConfigMenu::__VolumeToFloat(m_MenuVolume   .GetCurValue());
    g_CurConfig.Audio.iQuality       = m_AudioQuality.GetCurValue();
    Core::Config->SetInt(CORE_CONFIG_AUDIO_MODE, m_AudioMode.GetCurValue());
    g_CurConfig.Audio.i3DSound       = m_3DSound     .GetCurValue();

    // 
    Core::Config->SetString(CORE_CONFIG_BASE_LANGUAGE, Core::Language->GetPath());
    g_CurConfig.Game.iGameDirection = m_GameDirection.GetCurValue();
    g_CurConfig.Game.iMirrorMode    = m_MirrorMode   .GetCurValue();
    g_CurConfig.Game.iHudDirection  = m_HudDirection .GetCurValue();
    g_CurConfig.Game.iHudType       = m_HudType      .GetCurValue();
    g_CurConfig.Game.iCombatText    = m_CombatText   .GetCurValue();
    g_CurConfig.Game.iBackRotation  = m_PureMode     .GetCurValue() ? g_CurConfig.Game.iBackRotation : m_BackRotation.GetCurValue();
    g_CurConfig.Game.iBackSpeed     = m_PureMode     .GetCurValue() ? g_CurConfig.Game.iBackSpeed    : m_BackSpeed   .GetCurValue();
    g_CurConfig.Game.iUpdateFreq    = m_PureMode     .GetCurValue() ? g_CurConfig.Game.iUpdateFreq   : m_UpdateFreq  .GetCurValue();
    g_CurConfig.Game.iPureMode      = m_PureMode     .GetCurValue();
    g_CurConfig.Game.iLeaderboard   = m_Leaderboard  .GetCurValue();
    g_CurConfig.Game.iVersion       = m_Version      .GetCurValue();

    // 
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        g_CurConfig.Input.aiType       [i] = m_aInput[i].oType       .GetCurValue();
        g_CurConfig.Input.aiRumble     [i] = m_aInput[i].oRumble     .GetCurValue();
        g_CurConfig.Input.aiFireMode   [i] = m_aInput[i].oFireMode   .GetCurValue();
        g_CurConfig.Input.aiControlMode[i] = m_aInput[i].oControlMode.GetCurValue();
    }

    // 
    SaveConfig();
    this->CheckValues();

    if(bReset || bReshape || bManager)
    {
        Core::System->SetWindowAll(iCurMonitor, vCurResolution, coreSystemMode(m_DisplayMode.GetCurValue()));

        InitResolution(Core::System->GetResolution());

        if(bReset)
        {
            Core::Reset();
        }
        else
        {
            if(bReshape)
            {
                Core::Reshape();
            }

            if(bManager)
            {
                Core::Manager::Resource->Reset();
            }
        }

        // 
        this->__LoadMonitors();
        this->__LoadResolutions(Core::System->GetDisplayIndex());
        this->__LoadFrequencies(Core::System->GetDisplayIndex());
        this->__LoadInputs();

        // 
        g_pMenu->InvokePauseStep();
        
        if(STATIC_ISVALID(g_pGame))   // TODO 1: this is called during Move()
        {   // TODO 1: does not work fully, as resources are not loaded yet
            // refresh frame-buffers        
            g_pGlow->Update();
            Timeless([]()
            {
                g_pDistortion->Update();
            });
            coreFrameBuffer::EndDraw();
        }
    }

    if(bAudio)
    {
        // 
        Core::Audio->Reconfigure();
    }

    // 
    InitDirection();
    InitFramerate();
}


// ****************************************************************
// 
void cConfigMenu::ResetNavigator()
{
    // 
    m_Navigator.ResetFirst();

    // 
    m_VideoBox.SetCurOffset(0.0f);
    m_InputBox.SetCurOffset(0.0f);

    // 
    m_GameNew      .Acquire();
    m_MirrorModeNew.Acquire();

    // 
    this->__RefreshTabs();
}


// ****************************************************************
// convert input key to readable string                   
void cConfigMenu::PrintFigure(cFigure* OUTPUT pFigure, const coreUint8 iType, const coreInt16 iKey)
{
    pFigure->SetBaseAsType(iType);
    pFigure->SetKey       (iKey);
}


// ****************************************************************
// 
void cConfigMenu::__UpdateRenderQuality()
{
    // 
    //g_CurConfig.Graphics.iRender = m_RenderQuality.GetCurValue();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateShadowQuality()
{
    // 
    g_CurConfig.Graphics.iShadow = m_ShadowQuality.GetCurValue();

    // 
    cOutdoor* pOutdoor = g_pEnvironment->GetBackground()->GetOutdoor();
    if(pOutdoor) pOutdoor->GetShadowMap()->Reconfigure();
    cShadow::Recompile();

    // 
    g_pMenu->InvokePauseStep();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateWaterQuality()
{
    // 
    g_CurConfig.Graphics.iReflection = m_WaterQuality.GetCurValue();

    // 
    g_pMenu->InvokePauseStep();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateVolume()
{
    // 
    Core::Audio->SetGlobalVolume(cConfigMenu::__VolumeToFloat(m_GlobalVolume.GetCurValue()));
    Core::Audio->SetMusicVolume (cConfigMenu::__VolumeToFloat(m_MusicVolume .GetCurValue()));
    g_CurConfig.Audio.fEffectVolume  = cConfigMenu::__VolumeToFloat(m_EffectVolume .GetCurValue());
    g_CurConfig.Audio.fAmbientVolume = cConfigMenu::__VolumeToFloat(m_AmbientVolume.GetCurValue());
    g_CurConfig.Audio.fMenuVolume    = cConfigMenu::__VolumeToFloat(m_MenuVolume   .GetCurValue());

    // 
    Core::Audio->SetTypeVolume(g_CurConfig.Audio.fEffectVolume,  SOUND_EFFECT);
    Core::Audio->SetTypeVolume(g_CurConfig.Audio.fAmbientVolume, SOUND_AMBIENT);
    Core::Audio->SetTypeVolume(g_CurConfig.Audio.fMenuVolume,    SOUND_MENU);
}


// ****************************************************************
// 
void cConfigMenu::__UpdateLanguage()
{
    // 
    const coreList<coreString>& asLanguageList = cMenu::GetLanguageList().get_valuelist();
    Core::Language->Load(asLanguageList[m_Language.GetCurIndex()].c_str());

    // 
    cMenu::UpdateLanguageFont();

    // invalidate unbound language-strings
    g_pMenu->GetTooltip()->Invalidate();    // TODO 1: not needed anymore (?)
    this->__LoadMonitors();
    this->__LoadFrequencies(Core::System->GetDisplayIndex());
    this->__LoadInputs();

    // 
    this->__RefreshTabs();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateBackSpeed()
{
    // 
    g_CurConfig.Game.iBackSpeed = m_BackSpeed.GetCurValue();

    // 
    if(!STATIC_ISVALID(g_pGame)) g_pEnvironment->SetTargetSpeedNow(ENVIRONMENT_DEFAULT_SPEED);
}


// ****************************************************************
// 
void cConfigMenu::__UpdatePureModeBase()
{
    // 
    g_CurConfig.Game.iBackRotation = m_BackRotation.GetCurValue();
    g_CurConfig.Game.iBackSpeed    = m_BackSpeed   .GetCurValue();
    g_CurConfig.Game.iUpdateFreq   = m_UpdateFreq  .GetCurValue();
}


// ****************************************************************
// 
void cConfigMenu::__UpdatePureMode()
{
    // 
    g_CurConfig.Game.iPureMode = m_PureMode.GetCurValue();

    // 
    m_BackRotation.SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_BACKROTATION : g_CurConfig.Game.iBackRotation);
    m_BackSpeed   .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_BACKSPEED    : g_CurConfig.Game.iBackSpeed);
    m_UpdateFreq  .SelectValue(g_CurConfig.Game.iPureMode ? SCORE_PURE_UPDATEFREQ   : g_CurConfig.Game.iUpdateFreq);

    // 
    if(!STATIC_ISVALID(g_pGame)) g_pEnvironment->SetTargetSpeedNow(ENVIRONMENT_DEFAULT_SPEED);
}


// ****************************************************************
// 
void cConfigMenu::__UpdateInterface()
{
    // 
    g_CurConfig.Game.iGameDirection = m_GameDirection.GetCurValue();
    g_CurConfig.Game.iMirrorMode    = m_MirrorMode   .GetCurValue();
    g_CurConfig.Game.iHudDirection  = m_HudDirection .GetCurValue();
    g_CurConfig.Game.iHudType       = m_HudType      .GetCurValue();

    // 
    InitDirection();

    // 
    g_pPostProcessing->UpdateLayout();
    if(STATIC_ISVALID(g_pGame))
    {
        g_pGame->GetInterface ()->UpdateLayout();
        g_pGame->GetInterface ()->UpdateSpacing();
        g_pGame->GetInterface ()->MoveTimeless();
        g_pGame->GetCombatText()->UpdateLayout();

        this->__RefreshManual();
    }
    
    g_pMenu->GetInterface()->UpdateLayout();
    g_pMenu->GetInterface()->UpdateSpacing();
    g_pMenu->GetInterface()->MoveTimeless();
        

    // 
    Core::Manager::Object->RefreshSprites();
}


// ****************************************************************
// 
void cConfigMenu::__LoadMonitors()
{
    // 
    const coreUintW iOldEntry = m_Monitor.GetNumEntries() ? m_Monitor.GetCurValue() : 0u;
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
    if(!m_Monitor.SelectValue(iOldEntry)) m_Monitor.SelectFirst();
}


// ****************************************************************
// 
void cConfigMenu::__LoadResolutions(const coreUintW iMonitorIndex)
{
    // 
    m_asCurResolution[m_iCurMonitorIndex] = m_Resolution.GetNumEntries() ? m_Resolution.GetCurText() : "";
    m_Resolution.ClearEntries();

    // 
    const coreSet<coreVector2>& avResolutionList = Core::System->GetDisplayData(iMonitorIndex).avAvailableRes;
    for(coreUintW i = avResolutionList.size(); i--; ) m_Resolution.AddEntry(PRINT("%.0f x %.0f", avResolutionList[i].x, avResolutionList[i].y), i);

    // 
    m_Resolution.AddEntryLanguage("RESOLUTION_DESKTOP", 0xEEu);
    if((iMonitorIndex == Core::System->GetDisplayIndex()) && !avResolutionList.count(Core::System->GetResolution()))
    {
        m_Resolution.AddEntryLanguage("RESOLUTION_CUSTOM", 0xFFu);
    }

    // 
    if(!m_Resolution.SelectText(m_asCurResolution[iMonitorIndex].c_str())) m_Resolution.SelectLast();

    // 
    m_iCurMonitorIndex = iMonitorIndex;
}


// ****************************************************************
// 
void cConfigMenu::__LoadFrequencies(const coreUintW iMonitorIndex)
{
    const coreUint8 aiDefault[] = {60u, 75u, 100u, 120u, 144u, 165u, 240u};   // TODO 1: constexpr

    // 
    const coreUintW iOldEntry = m_UpdateFreq.GetNumEntries() ? m_UpdateFreq.GetCurValue() : 0u;
    m_UpdateFreq.ClearEntries();

    // 
    SDL_DisplayMode oMode = {};
    SDL_GetCurrentDisplayMode(iMonitorIndex, &oMode);

    // 
    const coreUint32 iRefreshRate = (oMode.refresh_rate >= F_TO_SI(FRAMERATE_MIN)) ? oMode.refresh_rate : SCORE_PURE_UPDATEFREQ;

    // 
    m_UpdateFreq.AddEntry(PRINT("%s (%u Hz)", Core::Language->GetString("VALUE_AUTO"), iRefreshRate), 0u);
    for(coreUintW i = 0u; i < ARRAY_SIZE(aiDefault); ++i) m_UpdateFreq.AddEntry(PRINT("%u Hz", aiDefault[i]), aiDefault[i]);

    // 
    if(!m_UpdateFreq.SelectValue(iOldEntry)) m_UpdateFreq.SelectFirst();
}


// ****************************************************************
// 
void cConfigMenu::__LoadInputs()
{
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        sPlayerInput& oInput = m_aInput[i];

        // 
        const coreUintW iOldEntry = oInput.oType.GetNumEntries() ? oInput.oType.GetCurValue() : 0u;
        oInput.oType.ClearEntries();

        // 
        oInput.oHeader.SetText(PRINT("%s %zu", Core::Language->GetString("PLAYER"), i + 1u));
        for(coreUintW j = 0u; j < INPUT_SETS_KEYBOARD; ++j) oInput.oType.AddEntry(PRINT("%s %zu", Core::Language->GetString("INPUT_KEYBOARD"), j + 1u), j + 0u);
        for(coreUintW j = 0u; j < INPUT_SETS_JOYSTICK; ++j) oInput.oType.AddEntry(PRINT("%s %zu", Core::Language->GetString("INPUT_JOYSTICK"), j + 1u), j + INPUT_SETS_KEYBOARD);

        // 
        if(!oInput.oType.SelectValue(iOldEntry)) oInput.oType.SelectFirst();

        // 
        for(coreUintW j = 0u; j < INPUT_KEYS; ++j)
        {
            cFigure&   oFigure   = this->__RetrieveInputFigure  (i, j);
            coreInt16& iCurValue = this->__RetrieveInputCurValue(i, j);

            cConfigMenu::PrintFigure(&oFigure, g_CurConfig.Input.aiType[i], iCurValue);
        }

        #if defined(_CORE_SWITCH_)
            const coreBool bHideSecond = (i && (Core::Input->GetJoystickNum() < 2u));
        #endif

        // 
        const auto nLockFunc = [&](const coreBool bLock, cGuiButton* OUTPUT pButton, cFigure* OUTPUT pFigure, const coreInt16 iKey, const coreBool bHide)
        {
            pButton->SetOverride(bLock ? -1 : 0);
                 if(bHide) pFigure->SetEnabled(bLock ? CORE_OBJECT_ENABLE_MOVE : CORE_OBJECT_ENABLE_ALL);
            else if(bLock) cConfigMenu::PrintFigure(pFigure, g_CurConfig.Input.aiType[i], iKey);

        #if defined(_CORE_SWITCH_)
            if(bHideSecond) pButton->SetOverride(-1);
            if(bHideSecond || !bHide) pFigure->SetEnabled(bHideSecond ? CORE_OBJECT_ENABLE_MOVE : CORE_OBJECT_ENABLE_ALL);
        #endif
        };

        // 
        const coreBool bKeyboard = (g_CurConfig.Input.aiType[i] < INPUT_SETS_KEYBOARD);
        nLockFunc(!bKeyboard, &oInput.oMoveUp,    &oInput.oFigureMoveUp,    SDL_CONTROLLER_BUTTON_DPAD_UP,    false);
        nLockFunc(!bKeyboard, &oInput.oMoveLeft,  &oInput.oFigureMoveLeft,  SDL_CONTROLLER_BUTTON_DPAD_LEFT,  false);
        nLockFunc(!bKeyboard, &oInput.oMoveDown,  &oInput.oFigureMoveDown,  SDL_CONTROLLER_BUTTON_DPAD_DOWN,  false);
        nLockFunc(!bKeyboard, &oInput.oMoveRight, &oInput.oFigureMoveRight, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, false);
        nLockFunc( bKeyboard, &oInput.aAction[7], &oInput.aFigureAction[7], SDL_SCANCODE_ESCAPE,              false);

        // 
        const coreBool bOriginal  = (g_CurConfig.Input.aiControlMode[i] == 0u);
        const coreBool bTwinstick = (g_CurConfig.Input.aiControlMode[i] == 1u);
        const coreBool bMixed     = (g_CurConfig.Input.aiControlMode[i] == 2u);
        const coreBool bSingle    = (g_CurConfig.Input.aiControlMode[i] == 3u);
        nLockFunc(!bOriginal  && !bMixed && !bSingle, &oInput.aAction[0], &oInput.aFigureAction[0], 0u, true);
        nLockFunc(!bOriginal  && !bMixed,             &oInput.aAction[1], &oInput.aFigureAction[1], 0u, true);
        nLockFunc(!bOriginal  && !bMixed,             &oInput.aAction[2], &oInput.aFigureAction[2], 0u, true);
        nLockFunc(!bTwinstick && !bMixed,             &oInput.aAction[3], &oInput.aFigureAction[3], 0u, true);
        nLockFunc(!bTwinstick && !bMixed,             &oInput.aAction[4], &oInput.aFigureAction[4], 0u, true);
        nLockFunc(!bTwinstick && !bMixed,             &oInput.aAction[5], &oInput.aFigureAction[5], 0u, true);
        nLockFunc(!bTwinstick && !bMixed,             &oInput.aAction[6], &oInput.aFigureAction[6], 0u, true);
        nLockFunc(!bOriginal  && !bMixed && !bSingle, &oInput.aAction[8], &oInput.aFigureAction[8], 0u, true);
        nLockFunc(!bTwinstick && !bMixed,             &oInput.aAction[9], &oInput.aFigureAction[9], 0u, true);

        // 
        const coreBool bStickyL = (!bKeyboard);
        const coreBool bStickyR = (!bKeyboard && (bTwinstick || bMixed));
        oInput.oFigureStickL.SetEnabled(bStickyL ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);
        oInput.oFigureStickR.SetEnabled(bStickyR ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_MOVE);
        if(bStickyL) cConfigMenu::PrintFigure(&oInput.oFigureStickL, g_CurConfig.Input.aiType[i], FIGURE_KEY_LEFTSTICK);
        if(bStickyR) cConfigMenu::PrintFigure(&oInput.oFigureStickR, g_CurConfig.Input.aiType[i], FIGURE_KEY_RIGHTSTICK);

        #if defined(_CORE_SWITCH_)
            if(bHideSecond) oInput.oFigureStickL.SetEnabled(CORE_OBJECT_ENABLE_MOVE);
            if(bHideSecond) oInput.oFigureStickR.SetEnabled(CORE_OBJECT_ENABLE_MOVE);
            oInput.oHeader.SetColor3(COLOR_MENU_WHITE * (bHideSecond ? MENU_LIGHT_IDLE : MENU_LIGHT_ACTIVE));
        #endif

        // 
        //oInput.oRumble.SetOverride((bKeyboard || !Core::Input->GetJoystickHasRumble(g_CurConfig.Input.aiType[i] - INPUT_SETS_KEYBOARD)) ? -1 : 0);
    }

    // 
    m_iJoystickNum = Core::Input->GetJoystickNum();
}


// ****************************************************************
// 
void cConfigMenu::__LoadUnlocks()
{
    const coreBool bMirrorMode = HAS_BIT_EX(g_pSave->GetHeader().oProgress.aiUnlock, UNLOCK_MIRRORMODE);

    // 
    m_MirrorMode.ClearEntries();
    if(bMirrorMode)
    {
        m_MirrorMode.AddEntryLanguage("VALUE_OFF", 0u);
        m_MirrorMode.AddEntryLanguage("VALUE_ON",  1u);
        //m_MirrorMode.AddEntryLanguage("VALUE_LOW",  1u);
        //m_MirrorMode.AddEntryLanguage("VALUE_HIGH", 2u);
    }
    else
    {
        m_MirrorMode.AddEntryLanguage("UNKNOWN", 0u);
    }

    // 
    m_MirrorMode.SetOverride(bMirrorMode ? 0 : -1);

    // 
    m_aLabel[ENTRY_GAME_MIRRORMODE].SetColor3      (COLOR_MENU_WHITE * (bMirrorMode ? MENU_LIGHT_ACTIVE : MENU_LIGHT_IDLE));
    m_aLabel[ENTRY_GAME_MIRRORMODE].SetTextLanguage(bMirrorMode ? "CONFIG_GAME_MIRRORMODE" : "UNKNOWN");
}


// ****************************************************************
// 
void cConfigMenu::__RefreshManual()
{
    if(STATIC_ISVALID(g_pGame) && (g_pGame->GetCurMission()->GetID() == cIntroMission::ID))
    {
        Timeless([]()
        {
            g_pGame->GetCurMission()->MoveAfter();
        });
        g_pMenu->InvokePauseStep();
    }
}


// ****************************************************************
// 
void cConfigMenu::__RefreshTabs()
{
    if(Core::Language->GetString("OVERFLOW_CONFIG_TABS")[0] != '0')
    {
        m_VideoTab.Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_AudioTab.Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_InputTab.Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_GameTab .Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
    }
    else
    {
        m_VideoTab.Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_AudioTab.Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_InputTab.Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
        m_GameTab .Construct(MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    }

    // 
    m_VideoTab.DefineProgram("menu_border_program");
    m_AudioTab.DefineProgram("menu_border_program");
    m_InputTab.DefineProgram("menu_border_program");
    m_GameTab .DefineProgram("menu_border_program");
}


// ****************************************************************
// 
coreBool cConfigMenu::__SetupInput()
{
    return false;
}