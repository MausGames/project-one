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

    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        if(i == ENTRY_VIDEO || i == ENTRY_AUDIO || i == ENTRY_INPUT) iOffset = 0u;
        if(i == ENTRY_VIDEO_ANTIALIASING)  ++iOffset;   // # new paragraph
        if(i == ENTRY_VIDEO_RENDERQUALITY) ++iOffset;
        if(i == ENTRY_VIDEO_SHAKEEFFECTS)  ++iOffset;
        if(i == ENTRY_AUDIO_MUSICVOLUME)   ++iOffset;
        if(i == ENTRY_AUDIO_3DSOUND)       ++iOffset;
        if(i == ENTRY_INPUT_MOVEUP)        ++iOffset;
        if(i == ENTRY_INPUT_ACTION1)       ++iOffset;
        if(i == ENTRY_GAME_GAMEROTATION)   ++iOffset;
        if(i == ENTRY_GAME_HUDROTATION)    ++iOffset;
        if(i == ENTRY_GAME_UPDATEFREQ)     ++iOffset;
        if(i == ENTRY_GAME_MIRRORMODE)     ++iOffset;

        m_aLabel[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aLabel[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.025f*I_TO_F(iOffset)));
        m_aLabel[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aLabel[i].SetColor3   (COLOR_MENU_WHITE);

        iOffset += 2u;
    }

    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aLabel[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aLine[i].SetFocusable (true);
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueInput); ++i)
    {
        m_aCueInput[i].Construct  (MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aCueInput[i].SetPosition(m_aLabel[ENTRY_INPUT_MOVEUP + i].GetPosition() + coreVector2(0.23f,0.0f));
        m_aCueInput[i].SetColor3  (COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
    }
    m_aCueInput[1].SetDirection(coreVector2( 1.0f, 0.0f));
    m_aCueInput[2].SetDirection(coreVector2( 0.0f,-1.0f));
    m_aCueInput[3].SetDirection(coreVector2(-1.0f, 0.0f));
    m_aCueInput[0].SetText     (ICON_ARROW_UP);
    m_aCueInput[1].SetText     (ICON_ARROW_UP);
    m_aCueInput[2].SetText     (ICON_ARROW_UP);
    m_aCueInput[3].SetText     (ICON_ARROW_UP);
    m_aCueInput[5].SetText     (ICON_UNDO_ALT);
    m_aCueInput[6].SetText     (ICON_REDO_ALT);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueRota); ++i)
    {
        m_aCueRota[i].Construct(MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
        m_aCueRota[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
        m_aCueRota[i].SetText  (ICON_ARROW_UP);
    }
    m_aCueRota[0].SetPosition(m_aLabel[ENTRY_GAME_GAMEROTATION].GetPosition() + coreVector2(0.522f,0.0f));
    m_aCueRota[1].SetPosition(m_aLabel[ENTRY_GAME_HUDROTATION] .GetPosition() + coreVector2(0.522f,0.0f));

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDescription); ++i)
    {
        m_aDescription[i].Construct  (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aDescription[i].SetPosition(m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,-0.5f) + coreVector2(0.0f, 0.075f - 0.03f*I_TO_F(i)));
        m_aDescription[i].SetColor3  (COLOR_MENU_WHITE);
    }

    #define __SET_OPTION(x,n,s)                                                  \
    {                                                                            \
        cGuiLabel& oLabel = m_aLabel[ENTRY_ ## n];                               \
        oLabel.SetTextLanguage("CONFIG_" #n);                                    \
                                                                                 \
        x.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL); \
        x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition());        \
        x.SetSize     (coreVector2(   (s),0.03f));                               \
        x.SetAlignment(coreVector2(-1.00f,0.00f));                               \
        x.GetCaption()->SetColor3     (COLOR_MENU_WHITE);                        \
        x.GetCaption()->ChangeLanguage(Core::Language);                          \
                                                                                 \
        m_apcDescKey[ENTRY_ ## n] = "CONFIG_" #n "_DESC";                        \
    }
    {
        __SET_OPTION(m_Monitor,       VIDEO_MONITOR,       0.26f)
        __SET_OPTION(m_Resolution,    VIDEO_RESOLUTION,    0.26f)
        __SET_OPTION(m_DisplayMode,   VIDEO_DISPLAYMODE,   0.26f)
        __SET_OPTION(m_AntiAliasing,  VIDEO_ANTIALIASING,  0.26f)
        __SET_OPTION(m_TextureFilter, VIDEO_TEXTUREFILTER, 0.26f)
        __SET_OPTION(m_RenderQuality, VIDEO_RENDERQUALITY, 0.26f)
        __SET_OPTION(m_ShadowQuality, VIDEO_SHADOWQUALITY, 0.26f)
        __SET_OPTION(m_ShakeEffects,  VIDEO_SHAKEEFFECTS,  0.26f)
        __SET_OPTION(m_FlashEffects,  VIDEO_FLASHEFFECTS,  0.26f)
        __SET_OPTION(m_GlobalVolume,  AUDIO_GLOBALVOLUME,  0.26f)
        __SET_OPTION(m_MusicVolume,   AUDIO_MUSICVOLUME,   0.26f)
        __SET_OPTION(m_EffectVolume,  AUDIO_EFFECTVOLUME,  0.26f)
        __SET_OPTION(m_AmbientVolume, AUDIO_AMBIENTVOLUME, 0.26f)
        __SET_OPTION(m_3DSound,       AUDIO_3DSOUND,       0.26f)
        __SET_OPTION(m_Language,      GAME_LANGUAGE,       0.26f)
        __SET_OPTION(m_TextSize,      GAME_TEXTSIZE,       0.26f)
        __SET_OPTION(m_GameRotation,  GAME_GAMEROTATION,   0.26f)
        __SET_OPTION(m_GameScale,     GAME_GAMESCALE,      0.26f)
        __SET_OPTION(m_GameSpeed,     GAME_GAMESPEED,      0.26f)
        __SET_OPTION(m_HudRotation,   GAME_HUDROTATION,    0.26f)
        __SET_OPTION(m_HudScale,      GAME_HUDSCALE,       0.26f)
        __SET_OPTION(m_HudType,       GAME_HUDTYPE,        0.26f)
        __SET_OPTION(m_UpdateFreq,    GAME_UPDATEFREQ,     0.26f)
        __SET_OPTION(m_Version,       GAME_VERSION,        0.26f)
        __SET_OPTION(m_MirrorMode,    GAME_MIRRORMODE,     0.26f)

        m_Language    .SetEndless(true);
        m_GameRotation.SetEndless(true);
        m_HudRotation .SetEndless(true);
        m_HudType     .SetEndless(true);
        m_MirrorMode  .SetEndless(true);

        m_Navigator.BindObject(&m_Monitor,       &m_VideoTab,      NULL, &m_Resolution,     NULL, &m_GlobalVolume,         MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_Resolution,    &m_Monitor,       NULL, &m_DisplayMode,    NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_DisplayMode,   &m_Resolution,    NULL, &m_AntiAliasing,   NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_AntiAliasing,  &m_DisplayMode,   NULL, &m_TextureFilter,  NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_TextureFilter, &m_AntiAliasing,  NULL, &m_RenderQuality,  NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_RenderQuality, &m_TextureFilter, NULL, &m_ShadowQuality,  NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_ShadowQuality, &m_RenderQuality, NULL, &m_ShakeEffects,   NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_ShakeEffects,  &m_ShadowQuality, NULL, &m_FlashEffects,   NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_FlashEffects,  &m_ShakeEffects,  NULL, &m_SaveButton,     NULL, &m_3DSound,              MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_GlobalVolume,  &m_AudioTab,      NULL, &m_MusicVolume,    NULL, &m_aInput[0].oType,      MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_MusicVolume,   &m_GlobalVolume,  NULL, &m_EffectVolume,   NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_EffectVolume,  &m_MusicVolume,   NULL, &m_AmbientVolume,  NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_AmbientVolume, &m_EffectVolume,  NULL, &m_3DSound,        NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_3DSound,       &m_AmbientVolume, NULL, &m_SaveButton,     NULL, &m_aInput[0].aAction[3], MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_Language,      &m_GameTab,       NULL, &m_TextSize,       NULL, &m_Monitor,              MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_TextSize,      &m_Language,      NULL, &m_GameRotation,   NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_GameRotation,  &m_TextSize,      NULL, &m_GameScale,      NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_GameScale,     &m_GameRotation,  NULL, &m_GameSpeed,      NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_GameSpeed,     &m_GameScale,     NULL, &m_HudRotation,    NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_HudRotation,   &m_GameSpeed,     NULL, &m_HudScale,       NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_HudScale,      &m_HudRotation,   NULL, &m_HudType,        NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_HudType,       &m_HudScale,      NULL, &m_UpdateFreq,     NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_UpdateFreq,    &m_HudType,       NULL, &m_Version,        NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_Version,       &m_UpdateFreq,    NULL, &m_MirrorMode,     NULL, NULL,                    MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
        m_Navigator.BindObject(&m_MirrorMode,    &m_Version,       NULL, &m_SaveButton,     NULL, &m_FlashEffects,         MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS | MENU_TYPE_SWITCH_MOVE);
    }
    #undef __SET_OPTION

    #define __SET_INPUT(x,n,s)                                                                  \
    {                                                                                           \
        cGuiLabel& oLabel = m_aLabel[ENTRY_ ## n];                                              \
        oLabel.SetTextLanguage("CONFIG_" #n);                                                   \
                                                                                                \
        m_aInput[i].x.Construct   (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);    \
        m_aInput[i].x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition() - vOffset); \
        m_aInput[i].x.SetSize     (coreVector2(   (s),0.03f));                                  \
        m_aInput[i].x.SetAlignment(coreVector2(-1.00f,0.00f));                                  \
        m_aInput[i].x.GetCaption()->SetColor3     (COLOR_MENU_WHITE);                           \
        m_aInput[i].x.GetCaption()->ChangeLanguage(Core::Language);                             \
                                                                                                \
        m_apcDescKey[ENTRY_ ## n] = "CONFIG_" #n "_DESC";                                       \
    }
    {
        for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
        {
            const coreVector2 vOffset = coreVector2(0.29f * I_TO_F(MENU_CONFIG_INPUTS - i - 1u), 0.0f);

            __SET_INPUT(oType,      INPUT_TYPE,      0.26f)
            __SET_INPUT(oRumble,    INPUT_RUMBLE,    0.26f)
            __SET_INPUT(oFireMode,  INPUT_FIREMODE,  0.26f)
            __SET_INPUT(oMoveUp,    INPUT_MOVEUP,    0.26f)
            __SET_INPUT(oMoveLeft,  INPUT_MOVELEFT,  0.26f)
            __SET_INPUT(oMoveDown,  INPUT_MOVEDOWN,  0.26f)
            __SET_INPUT(oMoveRight, INPUT_MOVERIGHT, 0.26f)
            __SET_INPUT(aAction[0], INPUT_ACTION1,   0.26f)
            __SET_INPUT(aAction[1], INPUT_ACTION2,   0.26f)
            __SET_INPUT(aAction[2], INPUT_ACTION3,   0.26f)
            __SET_INPUT(aAction[3], INPUT_ACTION4,   0.26f)

            m_aInput[i].oType    .SetEndless(true);
            m_aInput[i].oRumble  .SetEndless(true);
            m_aInput[i].oFireMode.SetEndless(true);

            m_Navigator.BindObject(&m_aInput[i].oType,      &m_InputTab,             &m_aInput[1u - i].oType,      &m_aInput[i].oRumble,    &m_aInput[1u - i].oType,      &m_Language,   MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS);
            m_Navigator.BindObject(&m_aInput[i].oRumble,    &m_aInput[i].oType,      &m_aInput[1u - i].oRumble,    &m_aInput[i].oFireMode,  &m_aInput[1u - i].oRumble,    NULL,          MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS);
            m_Navigator.BindObject(&m_aInput[i].oFireMode,  &m_aInput[i].oRumble,    &m_aInput[1u - i].oFireMode,  &m_aInput[i].oMoveUp,    &m_aInput[1u - i].oFireMode,  NULL,          MENU_TYPE_TAB_NODE | MENU_TYPE_SWITCH_PRESS);
            m_Navigator.BindObject(&m_aInput[i].oMoveUp,    &m_aInput[i].oFireMode,  &m_aInput[1u - i].oMoveUp,    &m_aInput[i].oMoveLeft,  &m_aInput[1u - i].oMoveUp,    NULL,          MENU_TYPE_TAB_NODE);
            m_Navigator.BindObject(&m_aInput[i].oMoveLeft,  &m_aInput[i].oMoveUp,    &m_aInput[1u - i].oMoveLeft,  &m_aInput[i].oMoveDown,  &m_aInput[1u - i].oMoveLeft,  NULL,          MENU_TYPE_TAB_NODE);
            m_Navigator.BindObject(&m_aInput[i].oMoveDown,  &m_aInput[i].oMoveLeft,  &m_aInput[1u - i].oMoveDown,  &m_aInput[i].oMoveRight, &m_aInput[1u - i].oMoveDown,  NULL,          MENU_TYPE_TAB_NODE);
            m_Navigator.BindObject(&m_aInput[i].oMoveRight, &m_aInput[i].oMoveDown,  &m_aInput[1u - i].oMoveRight, &m_aInput[i].aAction[0], &m_aInput[1u - i].oMoveRight, NULL,          MENU_TYPE_TAB_NODE);
            m_Navigator.BindObject(&m_aInput[i].aAction[0], &m_aInput[i].oMoveRight, &m_aInput[1u - i].aAction[0], &m_aInput[i].aAction[1], &m_aInput[1u - i].aAction[0], NULL,          MENU_TYPE_TAB_NODE);
            m_Navigator.BindObject(&m_aInput[i].aAction[1], &m_aInput[i].aAction[0], &m_aInput[1u - i].aAction[1], &m_aInput[i].aAction[2], &m_aInput[1u - i].aAction[1], NULL,          MENU_TYPE_TAB_NODE);
            m_Navigator.BindObject(&m_aInput[i].aAction[2], &m_aInput[i].aAction[1], &m_aInput[1u - i].aAction[2], &m_aInput[i].aAction[3], &m_aInput[1u - i].aAction[2], NULL,          MENU_TYPE_TAB_NODE);
            m_Navigator.BindObject(&m_aInput[i].aAction[3], &m_aInput[i].aAction[2], &m_aInput[1u - i].aAction[3], &m_SaveButton,           &m_aInput[1u - i].aAction[3], &m_MirrorMode, MENU_TYPE_TAB_NODE);

            STATIC_ASSERT(MENU_CONFIG_INPUTS == 2u)
        }
    }
    #undef __SET_INPUT

    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        const cGuiButton& oLast = this->__RetrieveInputButton(i, INPUT_KEYS - 1u);

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
    const coreList<coreString>& asLanguageList = cMenu::GetLanguageList().get_keylist();

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
    if(m_AntiAliasing.GetValue(m_AntiAliasing.GetNumEntries() - 1u) != iMaxSamples) m_AntiAliasing.AddEntry(PRINT("%ux", iMaxSamples), iMaxSamples);   // possible 6x
    m_RenderQuality.AddEntryLanguage("VALUE_LOW",              0u);
    m_RenderQuality.AddEntryLanguage("VALUE_HIGH",             1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_LOW",              1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_HIGH",             2u);
    for(coreUintW i = 0u; i <= 200u; i += 5u) m_ShakeEffects .AddEntry(PRINT("%zu%%", i), i);
    m_FlashEffects .AddEntryLanguage("VALUE_OFF",              0u);
    m_FlashEffects .AddEntryLanguage("VALUE_ON",               1u);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_GlobalVolume .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_MusicVolume  .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_EffectVolume .AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 5u) m_AmbientVolume.AddEntry(PRINT("%zu%%", i), i);
    m_3DSound      .AddEntryLanguage("VALUE_OFF",              0u);
    m_3DSound      .AddEntryLanguage("VALUE_ON",               1u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oRumble  .AddEntryLanguage("VALUE_OFF",       0u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oRumble  .AddEntryLanguage("VALUE_ON",        10u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oFireMode.AddEntryLanguage("FIREMODE_NORMAL", 0u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oFireMode.AddEntryLanguage("FIREMODE_INVERT", 1u);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i) m_aInput[i].oFireMode.AddEntryLanguage("FIREMODE_TOGGLE", 2u);
    for(coreUintW i = 0u; i <= 2u; i += 1u) m_TextSize.AddEntry(PRINT("+%zu", i), i);
    m_GameRotation .AddEntryLanguage("VALUE_OFF",              0u);
    m_GameRotation .AddEntryLanguage("HUDROTATION_RIGHT",      1u);
    m_GameRotation .AddEntryLanguage("HUDROTATION_UPSIDE",     2u);
    m_GameRotation .AddEntryLanguage("HUDROTATION_LEFT",       3u);
    for(coreUintW i = 50u; i <= 100u; i += 1u) m_GameScale.AddEntry(PRINT("%zu%%", i), i);
    for(coreUintW i = 50u; i <= 200u; i += 5u) m_GameSpeed.AddEntry(PRINT("%zu%%", i), i);
    m_HudRotation  .AddEntryLanguage("VALUE_OFF",              0u);
    m_HudRotation  .AddEntryLanguage("HUDROTATION_RIGHT",      1u);
    m_HudRotation  .AddEntryLanguage("HUDROTATION_UPSIDE",     2u);
    m_HudRotation  .AddEntryLanguage("HUDROTATION_LEFT",       3u);
    for(coreUintW i = 50u; i <= 150u; i += 1u) m_HudScale.AddEntry(PRINT("%zu%%", i), i);
    m_HudType      .AddEntryLanguage("HUDTYPE_OUTSIDE",        0u);
    m_HudType      .AddEntryLanguage("HUDTYPE_INSIDE",         1u);
    m_HudType      .AddEntryLanguage("HUDTYPE_BORDER",         2u);
    m_UpdateFreq   .AddEntryLanguage("VALUE_AUTO",             0u);   // TODO 1: "Auto (60 Hz)"
    m_UpdateFreq   .AddEntry        ("60 Hz",                  60u);
    m_UpdateFreq   .AddEntry        ("75 Hz",                  75u);
    m_UpdateFreq   .AddEntry        ("100 Hz",                 100u);
    m_UpdateFreq   .AddEntry        ("120 Hz",                 120u);
    m_UpdateFreq   .AddEntry        ("144 Hz",                 144u);
    m_UpdateFreq   .AddEntry        ("165 Hz",                 165u);
    m_UpdateFreq   .AddEntry        ("240 Hz",                 240u);
    //m_UpdateFreq   .AddEntry        ("360 Hz",                 360u); check again which hz are actually useful
    //m_UpdateFreq   .AddEntry        ("480 Hz",                 480u); brand new https://arstechnica.com/gadgets/2022/05/480-hz-desktop-laptop-displays-teased-by-pc-panel-pusher-auo/
    m_Version      .AddEntry        ("1.0.0",                  1u);
    m_Version      .AddEntryLanguage("VERSION_LATEST",         0u);   // TODO 1: "Latest (1.2.0)"
    m_MirrorMode   .AddEntryLanguage("VALUE_OFF",              0u);
    m_MirrorMode   .AddEntryLanguage("VALUE_LOW",              1u);
    m_MirrorMode   .AddEntryLanguage("VALUE_HIGH",             2u);

    // 
    m_Navigator.BindObject(&m_VideoTab, &m_SaveButton, &m_GameTab,  &m_Monitor, &m_AudioTab, NULL, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_AudioTab, &m_SaveButton, &m_VideoTab, &m_Monitor, &m_InputTab, NULL, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_InputTab, &m_SaveButton, &m_AudioTab, &m_Monitor, &m_GameTab,  NULL, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);
    m_Navigator.BindObject(&m_GameTab,  &m_SaveButton, &m_InputTab, &m_Monitor, &m_VideoTab, NULL, MENU_TYPE_TAB_ROOT | MENU_TYPE_AUTO_CLICK);

    m_Navigator.BindObject(&m_SaveButton,    &m_MirrorMode, &m_BackButton,    &m_Monitor, &m_DiscardButton, NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_DiscardButton, &m_MirrorMode, &m_SaveButton,    &m_Monitor, &m_BackButton,    NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindObject(&m_BackButton,    &m_MirrorMode, &m_DiscardButton, &m_Monitor, &m_SaveButton,    NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignSurface(&m_VideoTab, SURFACE_CONFIG_VIDEO);
    m_Navigator.AssignSurface(&m_AudioTab, SURFACE_CONFIG_AUDIO);
    m_Navigator.AssignSurface(&m_InputTab, SURFACE_CONFIG_INPUT);
    m_Navigator.AssignSurface(&m_GameTab,  SURFACE_CONFIG_GAME);

    m_Navigator.AssignFirst(&m_Monitor);
    m_Navigator.AssignMenu(this);

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

        for(coreUintW j = 0u; j < ARRAY_SIZE(m_aDescription); ++j) this->BindObject(i, &m_aDescription[j]);
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
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_ShakeEffects);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_FlashEffects);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_GlobalVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_MusicVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_EffectVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_AmbientVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_3DSound);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_Language);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_TextSize);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_GameRotation);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_GameScale);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_GameSpeed);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_HudRotation);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_HudScale);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_HudType);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_UpdateFreq);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_Version);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_MirrorMode);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueInput); ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aCueInput[i]);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oHeader);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oType);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oRumble);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) this->BindObject(SURFACE_CONFIG_INPUT, &m_aInput[i].oFireMode);
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS;      ++i) for(coreUintW j = 0u; j < INPUT_KEYS; ++j) this->BindObject(SURFACE_CONFIG_INPUT, &this->__RetrieveInputButton(i, j));
    this->BindObject(SURFACE_CONFIG_INPUT, &m_SwapInput);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCueRota); ++i) this->BindObject(SURFACE_CONFIG_GAME, &m_aCueRota[i]);

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
            if(m_Monitor.GetUserSwitch())
                this->__LoadResolutions(m_Monitor.GetCurValue());

            // 
            if(m_RenderQuality.GetUserSwitch())
                this->__UpdateRenderQuality();

            // 
            if(m_ShadowQuality.GetUserSwitch())
                this->__UpdateShadowQuality();

            // 
            cMenu::UpdateSwitchBox(&m_Monitor);
            cMenu::UpdateSwitchBox(&m_Resolution);
            cMenu::UpdateSwitchBox(&m_DisplayMode);
            cMenu::UpdateSwitchBox(&m_AntiAliasing);
            cMenu::UpdateSwitchBox(&m_TextureFilter);
            cMenu::UpdateSwitchBox(&m_RenderQuality);
            cMenu::UpdateSwitchBox(&m_ShadowQuality);
            cMenu::UpdateSwitchBox(&m_ShakeEffects);
            cMenu::UpdateSwitchBox(&m_FlashEffects);

            // 
                 if(m_RenderQuality.GetCurValue() == 0u) m_RenderQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_RenderQuality.GetCurValue() == 1u) m_RenderQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
                 if(m_ShadowQuality.GetCurValue() == 1u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_ShadowQuality.GetCurValue() == 2u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
        }
        break;

    case SURFACE_CONFIG_AUDIO:
        {
            // 
            if(m_GlobalVolume .GetUserSwitch() ||
               m_MusicVolume  .GetUserSwitch() ||
               m_EffectVolume .GetUserSwitch() ||
               m_AmbientVolume.GetUserSwitch())
                this->__UpdateVolume();

            // 
            if(m_EffectVolume.GetUserSwitch())
                {/* play effect */}

            // 
            cMenu::UpdateSwitchBox(&m_GlobalVolume);
            cMenu::UpdateSwitchBox(&m_MusicVolume);
            cMenu::UpdateSwitchBox(&m_EffectVolume);
            cMenu::UpdateSwitchBox(&m_AmbientVolume);
            cMenu::UpdateSwitchBox(&m_3DSound);

            // 
            m_GlobalVolume .GetCaption()->SetColor3(COLOR_HEALTH(I_TO_F(m_GlobalVolume .GetCurValue()) * 0.01f));
            m_MusicVolume  .GetCaption()->SetColor3(COLOR_HEALTH(I_TO_F(m_MusicVolume  .GetCurValue()) * 0.01f));
            m_EffectVolume .GetCaption()->SetColor3(COLOR_HEALTH(I_TO_F(m_EffectVolume .GetCurValue()) * 0.01f));
            m_AmbientVolume.GetCaption()->SetColor3(COLOR_HEALTH(I_TO_F(m_AmbientVolume.GetCurValue()) * 0.01f));
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
                if(oInput.oType.GetUserSwitch())
                {
                    for(coreUintW j = 0u, je = oInput.oType.GetNumEntries(); j < je; ++j)
                    {
                        coreBool bSkip = false;

                        // skip joystick/gamepad input sets without available device
                        if((oInput.oType.GetCurValue() >= INPUT_SETS_KEYBOARD) && (oInput.oType.GetCurValue() - INPUT_SETS_KEYBOARD >= Core::Input->GetJoystickNum()))
                            bSkip = true;

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
                }

                // 
                if(oInput.oRumble  .GetUserSwitch()) g_CurConfig.Input.aiRumble  [i] = oInput.oRumble  .GetCurValue();
                if(oInput.oFireMode.GetUserSwitch()) g_CurConfig.Input.aiFireMode[i] = oInput.oFireMode.GetCurValue();

                // 
                cMenu::UpdateSwitchBox(&oInput.oType);
                cMenu::UpdateSwitchBox(&oInput.oRumble);
                cMenu::UpdateSwitchBox(&oInput.oFireMode);

                for(coreUintW j = 0u; j < INPUT_KEYS; ++j)
                {
                    cGuiButton& oButton   = this->__RetrieveInputButton  (i, j);
                    coreInt16&  iCurValue = this->__RetrieveInputCurValue(i, j);

                    if(oButton.IsClicked())
                    {
                        const coreChar*  pcText = PRINT("%s [%s]", Core::Language->GetString("MAPPING"), m_aLabel[ENTRY_INPUT_MOVEUP + j].GetText());
                        const coreUint8& iType  = oInput.oType.GetCurValue();   // # referenced in lambda

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
                                                cGuiButton& oOtherButton = this->__RetrieveInputButton(n, m);
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
            if(m_Language.GetUserSwitch())
                this->__UpdateLanguage();

            // 
            if(m_TextSize.GetUserSwitch())
                {}

            // 
            if(m_GameRotation.GetUserSwitch() ||
               m_GameScale   .GetUserSwitch() ||
               m_HudRotation .GetUserSwitch() ||
               m_HudScale    .GetUserSwitch() ||
               m_HudType     .GetUserSwitch() ||
               m_MirrorMode  .GetUserSwitch())
                this->__UpdateInterface();

            // 
            m_aCueRota[0].SetDirection(MapToAxisInv(g_pPostProcessing->GetDirection(), g_vHudDirection));

            // 
            m_TextSize  .SetOverride(-1);   // TODO 1: enable 
            m_HudScale  .SetOverride(-1);   // TODO 1: enable 
            m_UpdateFreq.SetOverride(STATIC_ISVALID(g_pGame) ? -1 : 0);
            m_Version   .SetOverride(STATIC_ISVALID(g_pGame) ? -1 : 0);

            // 
            cMenu::UpdateSwitchBox(&m_Language);
            cMenu::UpdateSwitchBox(&m_TextSize);
            cMenu::UpdateSwitchBox(&m_GameRotation);
            cMenu::UpdateSwitchBox(&m_GameScale);
            cMenu::UpdateSwitchBox(&m_GameSpeed);
            cMenu::UpdateSwitchBox(&m_HudRotation);
            cMenu::UpdateSwitchBox(&m_HudScale);
            cMenu::UpdateSwitchBox(&m_HudType);
            cMenu::UpdateSwitchBox(&m_UpdateFreq);
            cMenu::UpdateSwitchBox(&m_Version);
            cMenu::UpdateSwitchBox(&m_MirrorMode);
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
    else if(m_DiscardButton.IsClicked() || Core::System->GetTerminated())
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
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_BackButton), TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
    
    
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aDescription); ++i)
    {
        m_aDescription[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
    
    for(coreUintW i = 0u; i < ENTRY_MAX; ++i)
    {
        if(!m_aLine[i].GetAlpha()) continue;

        m_aLine[i].Interact();
        m_aLine[i].SetColor3(m_aLine[i].IsFocused() ? g_pMenu->GetHighlightColor() : coreVector3(1.0f,1.0f,1.0f));

        if(m_aLine[i].IsFocused())
        {
            m_aDescription[0].SetEnabled(CORE_OBJECT_ENABLE_ALL);
            m_aDescription[0].SetTextLanguage(m_apcDescKey[i]);
        }
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
                           (m_Monitor      .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY))                                     ||
                           (m_DisplayMode  .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))                                  ||
                           (m_AntiAliasing .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING))                              ||
                           (m_TextureFilter.GetCurValue() != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY))                         ||
                           (m_RenderQuality.GetCurValue() != g_OldConfig.Graphics.iRender)                                                         ||
                           (m_ShadowQuality.GetCurValue() != g_OldConfig.Graphics.iShadow)                                                         ||
                           (m_ShakeEffects .GetCurValue() != g_OldConfig.Graphics.iShake)                                                          ||
                           (m_FlashEffects .GetCurValue() != g_OldConfig.Graphics.iFlash)                                                          ||
                           (m_GlobalVolume .GetCurValue() != cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME))) ||
                           (m_MusicVolume  .GetCurValue() != cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)))  ||
                           (m_EffectVolume .GetCurValue() != cConfigMenu::__VolumeToUint8(g_OldConfig.Audio.fEffectVolume))                        ||
                           (m_AmbientVolume.GetCurValue() != cConfigMenu::__VolumeToUint8(g_OldConfig.Audio.fAmbientVolume))                       ||
                           (m_3DSound      .GetCurValue() != g_OldConfig.Audio.i3DSound)                                                           ||
                           (std::strcmp(Core::Language->GetPath(), Core::Config->GetString(CORE_CONFIG_BASE_LANGUAGE)))                            ||
                           (m_TextSize     .GetCurValue() != g_OldConfig.Game.iTextSize)                                                           ||
                           (m_GameRotation .GetCurValue() != g_OldConfig.Game.iGameRotation)                                                       ||
                           (m_GameScale    .GetCurValue() != g_OldConfig.Game.iGameScale)                                                          ||
                           (m_GameSpeed    .GetCurValue() != g_OldConfig.Game.iGameSpeed)                                                          ||
                           (m_HudRotation  .GetCurValue() != g_OldConfig.Game.iHudRotation)                                                        ||
                           (m_HudScale     .GetCurValue() != g_OldConfig.Game.iHudScale)                                                           ||
                           (m_HudType      .GetCurValue() != g_OldConfig.Game.iHudType)                                                            ||
                           (m_UpdateFreq   .GetCurValue() != g_OldConfig.Game.iUpdateFreq)                                                         ||
                           (m_Version      .GetCurValue() != g_OldConfig.Game.iVersion)                                                            ||
                           (m_MirrorMode   .GetCurValue() != g_OldConfig.Game.iMirrorMode)                                                         ||
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
    const coreUintW iLanguageIndex      = m_Language     .GetCurIndex();

    // 
    LoadConfig();
    this->__LoadMonitors();
    this->__LoadResolutions(Core::System->GetDisplayIndex());
    this->__LoadInputs();

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
    m_Monitor      .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY));
    m_DisplayMode  .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN));
    m_AntiAliasing .SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING));
    m_TextureFilter.SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY));
    m_RenderQuality.SelectValue(g_CurConfig.Graphics.iRender);
    m_ShadowQuality.SelectValue(g_CurConfig.Graphics.iShadow);
    m_ShakeEffects .SelectValue(g_CurConfig.Graphics.iShake);
    m_FlashEffects .SelectValue(g_CurConfig.Graphics.iFlash);

    // 
    m_GlobalVolume .SelectValue(cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME)));
    m_MusicVolume  .SelectValue(cConfigMenu::__VolumeToUint8(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)));
    m_EffectVolume .SelectValue(cConfigMenu::__VolumeToUint8(g_CurConfig.Audio.fEffectVolume));
    m_AmbientVolume.SelectValue(cConfigMenu::__VolumeToUint8(g_CurConfig.Audio.fAmbientVolume));
    m_3DSound      .SelectValue(g_CurConfig.Audio.i3DSound);

    // 
    const coreList<coreString>& asLanguageList = cMenu::GetLanguageList().get_valuelist();
    m_Language    .SelectIndex(asLanguageList.first_index(Core::Config->GetString(CORE_CONFIG_BASE_LANGUAGE)));
    m_TextSize    .SelectValue(g_CurConfig.Game.iTextSize);
    m_GameRotation.SelectValue(g_CurConfig.Game.iGameRotation);
    m_GameScale   .SelectValue(g_CurConfig.Game.iGameScale);
    m_GameSpeed   .SelectValue(g_CurConfig.Game.iGameSpeed);
    m_HudRotation .SelectValue(g_CurConfig.Game.iHudRotation);
    m_HudScale    .SelectValue(g_CurConfig.Game.iHudScale);
    m_HudType     .SelectValue(g_CurConfig.Game.iHudType);
    m_UpdateFreq  .SelectValue(g_CurConfig.Game.iUpdateFreq);
    m_Version     .SelectValue(g_CurConfig.Game.iVersion);
    m_MirrorMode  .SelectValue(g_CurConfig.Game.iMirrorMode);

    // 
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        m_aInput[i].oType    .SelectValue(g_CurConfig.Input.aiType    [i]);
        m_aInput[i].oRumble  .SelectValue(g_CurConfig.Input.aiRumble  [i]);
        m_aInput[i].oFireMode.SelectValue(g_CurConfig.Input.aiFireMode[i]);
    }

    // 
    if(m_SaveButton.GetOverride() >= 0)
    {
        if(iRenderQualityIndex != m_RenderQuality.GetCurIndex()) this->__UpdateRenderQuality();
        if(iShadowQualityIndex != m_ShadowQuality.GetCurIndex()) this->__UpdateShadowQuality();
        if(iLanguageIndex      != m_Language     .GetCurIndex()) this->__UpdateLanguage();
        this->__UpdateVolume();
        this->__UpdateInterface();

        // TODO 1 m_TextSize 
    }

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
    const coreBool bReset = (vCurResolution != coreVector2(I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH)), I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT)))) ||
                            (m_Monitor      .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_SYSTEM_DISPLAY))             ||
                            (m_DisplayMode  .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))          ||
                            (m_AntiAliasing .GetCurValue() != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING))      ||
                            (m_TextureFilter.GetCurValue() != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY)) ||
                            (m_RenderQuality.GetCurValue() != g_CurConfig.Graphics.iRender);

    // 
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_WIDTH,               F_TO_SI(vCurResolution.x));
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_HEIGHT,              F_TO_SI(vCurResolution.y));
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_DISPLAY,             m_Monitor      .GetCurValue());
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_FULLSCREEN,          m_DisplayMode  .GetCurValue());
    Core::Config->SetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING,      m_AntiAliasing .GetCurValue());
    Core::Config->SetInt(CORE_CONFIG_GRAPHICS_TEXTUREANISOTROPY, m_TextureFilter.GetCurValue());
    g_CurConfig.Graphics.iRender = m_RenderQuality.GetCurValue();
    g_CurConfig.Graphics.iShadow = m_ShadowQuality.GetCurValue();
    g_CurConfig.Graphics.iShake  = m_ShakeEffects .GetCurValue();
    g_CurConfig.Graphics.iFlash  = m_FlashEffects .GetCurValue();

    // 
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME, cConfigMenu::__VolumeToFloat(m_GlobalVolume.GetCurValue()));
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME,  cConfigMenu::__VolumeToFloat(m_MusicVolume .GetCurValue()));
    g_CurConfig.Audio.fEffectVolume  = cConfigMenu::__VolumeToFloat(m_EffectVolume .GetCurValue());
    g_CurConfig.Audio.fAmbientVolume = cConfigMenu::__VolumeToFloat(m_AmbientVolume.GetCurValue());
    g_CurConfig.Audio.i3DSound       = m_3DSound.GetCurValue();

    // 
    Core::Config->SetString(CORE_CONFIG_BASE_LANGUAGE, Core::Language->GetPath());
    g_CurConfig.Game.iGameRotation = m_GameRotation.GetCurValue();
    g_CurConfig.Game.iGameScale    = m_GameScale   .GetCurValue();
    g_CurConfig.Game.iGameSpeed    = m_GameSpeed   .GetCurValue();
    g_CurConfig.Game.iHudRotation  = m_HudRotation .GetCurValue();
    g_CurConfig.Game.iHudScale     = m_HudScale    .GetCurValue();
    g_CurConfig.Game.iHudType      = m_HudType     .GetCurValue();
    g_CurConfig.Game.iUpdateFreq   = m_UpdateFreq  .GetCurValue();
    g_CurConfig.Game.iVersion      = m_Version     .GetCurValue();
    g_CurConfig.Game.iMirrorMode   = m_MirrorMode  .GetCurValue();

    // 
    for(coreUintW i = 0u; i < MENU_CONFIG_INPUTS; ++i)
    {
        g_CurConfig.Input.aiType    [i] = m_aInput[i].oType    .GetCurValue();
        g_CurConfig.Input.aiRumble  [i] = m_aInput[i].oRumble  .GetCurValue();
        g_CurConfig.Input.aiFireMode[i] = m_aInput[i].oFireMode.GetCurValue();
    }

    // 
    SaveConfig();
    this->CheckValues();

    if(bReset)
    {
        const coreVector2 vSafeResolution = vCurResolution.IsNull() ? Core::System->GetDisplayData(iCurMonitor).vDesktopRes : vCurResolution;

        Core::System->SetWindowResolution(vSafeResolution);
        
        // 
        InitResolution(Core::System->GetResolution());//vSafeResolution);
        Core::Reset();

        // 
        this->__LoadMonitors();
        this->__LoadResolutions(Core::System->GetDisplayIndex());
        this->__LoadInputs();
    }

    // 
    InitDirection();
    InitFramerate();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateRenderQuality()
{
    // 
    g_CurConfig.Graphics.iRender = m_RenderQuality.GetCurValue();
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
void cConfigMenu::__UpdateVolume()
{
    // 
    Core::Audio->SetGlobalVolume(cConfigMenu::__VolumeToFloat(m_GlobalVolume.GetCurValue()));
    Core::Audio->SetMusicVolume (cConfigMenu::__VolumeToFloat(m_MusicVolume .GetCurValue()));
    g_CurConfig.Audio.fEffectVolume  = cConfigMenu::__VolumeToFloat(m_EffectVolume .GetCurValue());
    g_CurConfig.Audio.fAmbientVolume = cConfigMenu::__VolumeToFloat(m_AmbientVolume.GetCurValue());

    // 
    Core::Audio->SetTypeVolume(g_CurConfig.Audio.fEffectVolume,  SOUND_EFFECT);
    Core::Audio->SetTypeVolume(g_CurConfig.Audio.fAmbientVolume, SOUND_AMBIENT);
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
    g_pMenu->GetTooltip()->Invalidate();
    this->__LoadMonitors();
    this->__LoadInputs();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateInterface()
{
    // 
    g_CurConfig.Game.iGameRotation = m_GameRotation.GetCurValue();
    g_CurConfig.Game.iGameScale    = m_GameScale   .GetCurValue();
    g_CurConfig.Game.iHudRotation  = m_HudRotation .GetCurValue();
    g_CurConfig.Game.iHudScale     = m_HudScale    .GetCurValue();
    g_CurConfig.Game.iHudType      = m_HudType     .GetCurValue();
    g_CurConfig.Game.iMirrorMode   = m_MirrorMode  .GetCurValue();

    // 
    InitDirection();

    // 
    g_pPostProcessing->UpdateLayout();
    if(STATIC_ISVALID(g_pGame))
    {
        g_pGame->GetInterface ()->UpdateLayout();
        g_pGame->GetCombatText()->UpdateLayout();
    }

    // 
    this->coreMenu::Move();
    // TODO 1: iterate over all objects and move them instead, so buttons don't flicker, due to alpha update    
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
            cGuiButton& oButton   = this->__RetrieveInputButton  (i, j);
            coreInt16&  iCurValue = this->__RetrieveInputCurValue(i, j);

            oButton.GetCaption()->SetText(cConfigMenu::__PrintKey(g_CurConfig.Input.aiType[i], iCurValue));
        }

        // 
        const auto nLockFunc = [](const coreBool bLock, cGuiButton* OUTPUT pButton, const coreChar* pcText)
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
    // TODO 1: use as translation key, what about keyboard ?
    // TODO 1: test with old controller
    
    if(iType < INPUT_SETS_KEYBOARD)   // # keyboard and mouse
    {
        if(-iKey == CORE_INPUT_LEFT)   return "M LEFT";
        if(-iKey == CORE_INPUT_MIDDLE) return "M MIDDLE";
        if(-iKey == CORE_INPUT_RIGHT)  return "M RIGHT";
        return (iKey <= 0) ? PRINT("M %d", -iKey) : SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(iKey)));
    }
    else   // # joystick/gamepad
    {
        const coreChar* pcText = SDL_GameControllerGetStringForButton(SDL_GameControllerButton(iKey));
        return pcText ? coreData::StrUpper(pcText) : PRINT("J %d", iKey);
    }
}