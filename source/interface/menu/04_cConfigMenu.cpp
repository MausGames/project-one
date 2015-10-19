//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Project One (http://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cConfigMenu::cConfigMenu()noexcept
: coreMenu (4u, SURFACE_CONFIG_VIDEO)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetSize      (coreVector2(0.8f,0.55f));

    m_VideoTab.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_VideoTab.DefineProgram("menu_border_program");
    m_VideoTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.115f,-0.012f));
    m_VideoTab.SetSize      (coreVector2(0.17f,0.07f));
    m_VideoTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_VideoTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_VideoTab.SetTexOffset (m_VideoTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(0.115f,0.012f));
    m_VideoTab.GetCaption()->SetTextLanguage("CONFIG_VIDEO");

    m_AudioTab.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_AudioTab.DefineProgram("menu_border_program");
    m_AudioTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(1.0f/3.0f,1.0f));
    m_AudioTab.SetSize      (m_VideoTab.GetSize());
    m_AudioTab.SetAlignment (m_VideoTab.GetAlignment());
    m_AudioTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_AudioTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_AudioTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0));
    m_AudioTab.GetCaption()->SetTextLanguage("CONFIG_AUDIO");

    m_InputTab.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_InputTab.DefineProgram("menu_border_program");
    m_InputTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(-1.0f/3.0f,1.0f));
    m_InputTab.SetSize      (m_VideoTab.GetSize());
    m_InputTab.SetAlignment (m_VideoTab.GetAlignment());
    m_InputTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_InputTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_InputTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0));
    m_InputTab.GetCaption()->SetTextLanguage("CONFIG_INPUT");

    m_GameTab.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_GameTab.DefineProgram("menu_border_program");
    m_GameTab.SetPosition  (m_VideoTab.GetPosition() * coreVector2(-1.0f,1.0f));
    m_GameTab.SetSize      (m_VideoTab.GetSize());
    m_GameTab.SetAlignment (m_VideoTab.GetAlignment());
    m_GameTab.SetTexSize   (m_VideoTab.GetTexSize());
    m_GameTab.SetTexOffset (m_VideoTab.GetTexOffset() + coreVector2(m_GameTab.GetPosition().x - m_VideoTab.GetPosition().x, 0.0));
    m_GameTab.GetCaption()->SetTextLanguage("CONFIG_GAME");

    m_SaveButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_SaveButton.DefineProgram("menu_border_program");
    m_SaveButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_SaveButton.SetSize      (coreVector2(0.3f,0.07f));
    m_SaveButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_SaveButton.SetOverride  (-1);   // # used in LoadValues()
    m_SaveButton.GetCaption()->SetTextLanguage("SAVE");

    m_DiscardButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_DiscardButton.DefineProgram("menu_border_program");
    m_DiscardButton.SetPosition  (m_SaveButton.GetPosition() + coreVector2(m_SaveButton.GetSize().x + 0.02f,0.0f));
    m_DiscardButton.SetSize      (m_SaveButton.GetSize());
    m_DiscardButton.SetAlignment (m_SaveButton.GetAlignment());
    m_DiscardButton.SetOverride  (-1);
    m_DiscardButton.GetCaption()->SetTextLanguage("DISCARD");

    m_BackButton.Construct    (MENU_BUTTON, MENU_ICON_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_SaveButton.GetSize().y);
    m_BackButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    // create configuration entries
    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_CONFIG_MAX; ++i)
    {
        if(i == MENU_CONFIG_VIDEO || i == MENU_CONFIG_AUDIO || i == MENU_CONFIG_INPUT) iOffset = 0u;
        if(i == MENU_CONFIG_VIDEO_ANTIALIASING) ++iOffset;   // # new paragraph
        if(i == MENU_CONFIG_VIDEO_ASSETQUALITY) ++iOffset;
        if(i == MENU_CONFIG_AUDIO_MUSICVOLUME)  ++iOffset;
        if(i == MENU_CONFIG_AUDIO_AMBIENTSOUND) ++iOffset;

        m_aLabel[i].Construct   (MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
        m_aLabel[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.025f*I_TO_F(iOffset)));
        m_aLabel[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aLabel[i].SetColor3   (COLOR_MENU_WHITE);

        iOffset += 2u;
    }

    for(coreUintW i = 0u; i < MENU_CONFIG_MAX; ++i)
    {
        m_aLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aLabel[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.1f, 0.0f));
    }

    #define __SET_SWITCHBOX(x,n,s)                                               \
    {                                                                            \
        coreLabel& oLabel = m_aLabel[MENU_CONFIG_ ## n];                         \
        oLabel.SetTextLanguage("CONFIG_" ## #n);                                 \
                                                                                 \
        x.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u); \
        x.SetPosition (coreVector2(-1.00f,1.00f) * oLabel.GetPosition());        \
        x.SetSize     (coreVector2(     s,0.03f));                               \
        x.SetAlignment(coreVector2(-1.00f,0.00f));                               \
        x.GetCaption()->SetColor3(COLOR_MENU_WHITE);                             \
    }
    {
        __SET_SWITCHBOX(m_Resolution,    VIDEO_RESOLUTION,    0.3f)
        __SET_SWITCHBOX(m_DisplayMode,   VIDEO_DISPLAYMODE,   0.3f)
        __SET_SWITCHBOX(m_AntiAliasing,  VIDEO_ANTIALIASING,  0.2f)
        __SET_SWITCHBOX(m_TextureFilter, VIDEO_TEXTUREFILTER, 0.2f)
        __SET_SWITCHBOX(m_AssetQuality,  VIDEO_ASSETQUALITY,  0.2f)
        __SET_SWITCHBOX(m_ShadowQuality, VIDEO_SHADOWQUALITY, 0.2f)
        __SET_SWITCHBOX(m_OverallVolume, AUDIO_OVERALLVOLUME, 0.2f)
        __SET_SWITCHBOX(m_MusicVolume,   AUDIO_MUSICVOLUME,   0.2f)
        __SET_SWITCHBOX(m_EffectVolume,  AUDIO_EFFECTVOLUME,  0.2f)
        __SET_SWITCHBOX(m_AmbientSound,  AUDIO_AMBIENTSOUND,  0.2f)
        __SET_SWITCHBOX(m_Language,      GAME_LANGUAGE,       0.3f)

        m_ShadowQuality.SetAutomatic(0.0f);
        m_Language     .SetAutomatic(0.0f);
    }
    #undef __SET_SWITCHBOX

    // fill configuration entries
    const std::vector<coreVector2>& avResolutionList = Core::System->GetAvailableRes();
    const std::vector<std::string>& asLanguageList   = cMenu::GetLanguageList().get_keylist();

    for(coreUintW i = avResolutionList.size(); i--; )
        m_Resolution.AddEntry(PRINT("%.0f x %.0f", avResolutionList[i].x, avResolutionList[i].y), i);

    for(coreUintW i = 0u, ie = asLanguageList.size(); i < ie; i++)
        m_Language.AddEntry(asLanguageList[i].c_str(), i);

    m_Resolution   .AddEntryLanguage("DISPLAY_DESKTOP", 0xFFu);
    m_DisplayMode  .AddEntryLanguage("DISPLAY_WINDOW",     0u);
    m_DisplayMode  .AddEntryLanguage("DISPLAY_FULLSCREEN", 1u);
    m_AntiAliasing .AddEntryLanguage("VALUE_OFF",  0u);
    m_TextureFilter.AddEntryLanguage("VALUE_OFF",  0u);
    for(coreUintW i = 2u; i <=  8u; i <<= 1u) m_AntiAliasing .AddEntry(PRINT("x%u", i), i);
    for(coreUintW i = 2u; i <= 16u; i <<= 1u) m_TextureFilter.AddEntry(PRINT("x%u", i), i);
    m_AssetQuality .AddEntryLanguage("VALUE_LOW",  0u);
    m_AssetQuality .AddEntryLanguage("VALUE_HIGH", 1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_OFF",  0u);
    m_ShadowQuality.AddEntryLanguage("VALUE_LOW",  1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_HIGH", 2u);
    for(coreUintW i = 0u; i <= 100u; i += 10u) m_OverallVolume.AddEntry(PRINT("%u%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 10u) m_MusicVolume  .AddEntry(PRINT("%u%%", i), i);
    for(coreUintW i = 0u; i <= 100u; i += 10u) m_EffectVolume .AddEntry(PRINT("%u%%", i), i);
    m_AmbientSound .AddEntryLanguage("VALUE_OFF",  0u);
    m_AmbientSound .AddEntryLanguage("VALUE_ON",   1u);

    // bind menu objects
    for(coreUintW i = 0u; i < this->GetNumSurfaces(); ++i)
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
    }

    coreUintW iIndex;
    for(iIndex = 0u; iIndex < MENU_CONFIG_VIDEO; ++iIndex) this->BindObject(SURFACE_CONFIG_VIDEO, &m_aLine [iIndex]);
    for(;            iIndex < MENU_CONFIG_AUDIO; ++iIndex) this->BindObject(SURFACE_CONFIG_AUDIO, &m_aLine [iIndex]);
    for(;            iIndex < MENU_CONFIG_INPUT; ++iIndex) this->BindObject(SURFACE_CONFIG_INPUT, &m_aLine [iIndex]);
    for(;            iIndex < MENU_CONFIG_MAX;   ++iIndex) this->BindObject(SURFACE_CONFIG_GAME,  &m_aLine [iIndex]);
    for(iIndex = 0u; iIndex < MENU_CONFIG_VIDEO; ++iIndex) this->BindObject(SURFACE_CONFIG_VIDEO, &m_aLabel[iIndex]);
    for(;            iIndex < MENU_CONFIG_AUDIO; ++iIndex) this->BindObject(SURFACE_CONFIG_AUDIO, &m_aLabel[iIndex]);
    for(;            iIndex < MENU_CONFIG_INPUT; ++iIndex) this->BindObject(SURFACE_CONFIG_INPUT, &m_aLabel[iIndex]);
    for(;            iIndex < MENU_CONFIG_MAX;   ++iIndex) this->BindObject(SURFACE_CONFIG_GAME,  &m_aLabel[iIndex]);

    this->BindObject(SURFACE_CONFIG_VIDEO, &m_Resolution);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_DisplayMode);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_AntiAliasing);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_TextureFilter);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_AssetQuality);
    this->BindObject(SURFACE_CONFIG_VIDEO, &m_ShadowQuality);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_OverallVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_MusicVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_EffectVolume);
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_AmbientSound);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_Language);
}


// ****************************************************************
// move the config menu
void cConfigMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);   // # to allow message box modifications

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
            cMenu::UpdateSwitchBox(&m_Resolution);
            cMenu::UpdateSwitchBox(&m_DisplayMode);
            cMenu::UpdateSwitchBox(&m_AntiAliasing);
            cMenu::UpdateSwitchBox(&m_TextureFilter);
            cMenu::UpdateSwitchBox(&m_AssetQuality);
            cMenu::UpdateSwitchBox(&m_ShadowQuality);

            // 
                 if(m_AssetQuality .GetCurIndex() == 0u) m_AssetQuality .GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_AssetQuality .GetCurIndex() == 1u) m_AssetQuality .GetCaption()->SetColor3(COLOR_MENU_GREEN);
                 if(m_ShadowQuality.GetCurIndex() == 0u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_WHITE);
            else if(m_ShadowQuality.GetCurIndex() == 1u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_ShadowQuality.GetCurIndex() == 2u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);

            // 
            if(m_ShadowQuality.IsClickedArrow())
                this->__UpdateShadowQuality();
        }
        break;

    case SURFACE_CONFIG_AUDIO:
        {
            // 
            cMenu::UpdateSwitchBox(&m_OverallVolume);
            cMenu::UpdateSwitchBox(&m_MusicVolume);
            cMenu::UpdateSwitchBox(&m_EffectVolume);
            cMenu::UpdateSwitchBox(&m_AmbientSound);

            // 
            m_OverallVolume.GetCaption()->SetColor3(cMenu::HealthColor(I_TO_F(m_OverallVolume.GetCurEntry().second) * 0.01f));
            m_MusicVolume  .GetCaption()->SetColor3(cMenu::HealthColor(I_TO_F(m_MusicVolume  .GetCurEntry().second) * 0.01f));
            m_EffectVolume .GetCaption()->SetColor3(cMenu::HealthColor(I_TO_F(m_EffectVolume .GetCurEntry().second) * 0.01f));

            // 
            if(m_OverallVolume.IsClickedArrow(CORE_INPUT_LEFT, CORE_INPUT_HOLD))
                this->__UpdateOverallVolume();
        }
        break;

    case SURFACE_CONFIG_INPUT:
        {

        }
        break;

    case SURFACE_CONFIG_GAME:
        {
            // 
            cMenu::UpdateSwitchBox(&m_Language);

            // 
            if(m_Language.IsClickedArrow())
                this->__UpdateLanguage();
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(this->GetAlpha() >= 1.0f)
    {
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
        else if(m_BackButton.IsClicked())
        {
            if(m_SaveButton.GetOverride() >= 0)
            {
                // 
                g_pMenu->GetMsgBox()->ShowQuestion(Core::Language->GetString("QUESTION_SAVE"), [&](const coreInt32& iStatus)
                {
                    // 
                    if(iStatus == MSGBOX_STATUS_YES)
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
    }

    // 
    if(Core::Input->GetMouseButton(CORE_INPUT_LEFT, CORE_INPUT_HOLD))
        this->CheckValues();

    // 
    cMenu::UpdateButton(&m_VideoTab, (this->GetCurSurface() == SURFACE_CONFIG_VIDEO) || m_VideoTab.IsFocused());
    cMenu::UpdateButton(&m_AudioTab, (this->GetCurSurface() == SURFACE_CONFIG_AUDIO) || m_AudioTab.IsFocused());
    cMenu::UpdateButton(&m_InputTab, (this->GetCurSurface() == SURFACE_CONFIG_INPUT) || m_InputTab.IsFocused());
    cMenu::UpdateButton(&m_GameTab,  (this->GetCurSurface() == SURFACE_CONFIG_GAME ) || m_GameTab .IsFocused());

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
    const coreUint8&  iCurValue      = m_Resolution.GetCurEntry().second;
    const coreVector2 vCurResolution = (iCurValue == 0xFFu) ? coreVector2(0.0f,0.0f) : Core::System->GetAvailableRes()[iCurValue];

    // 
    const coreBool bReset = (vCurResolution != coreVector2(I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH)), I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT)))) ||
                            (m_DisplayMode  .GetCurEntry().second != Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))                      ||
                            (m_AntiAliasing .GetCurEntry().second != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING))                  ||
                            (m_TextureFilter.GetCurEntry().second != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREFILTER))                 ||
                            (m_AssetQuality .GetCurEntry().second != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY))                       ||
                            (m_ShadowQuality.GetCurEntry().second != g_OldConfig.Graphics.iShadow)                                             ||
                            (m_OverallVolume.GetCurEntry().second != F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME) * 100.0f)) ||
                            (m_MusicVolume  .GetCurEntry().second != F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)  * 100.0f)) ||
                            (m_EffectVolume .GetCurEntry().second != F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME)  * 100.0f)) ||
                            (m_AmbientSound .GetCurEntry().second != g_OldConfig.Audio.iAmbient)                                               ||
                            (std::strcmp(Core::Language->GetPath(), Core::Config->GetString(CORE_CONFIG_SYSTEM_LANGUAGE)));

    // 
    m_SaveButton   .SetOverride(bReset ? 0 : -1);
    m_DiscardButton.SetOverride(bReset ? 0 : -1);
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

    // 
    if(!m_Resolution.SelectText(PRINT("%d x %d", Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH), Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT))))
        m_Resolution.SelectValue(0xFFu);

    // 
    m_DisplayMode  .SelectValue(Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN));
    m_AntiAliasing .SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING));
    m_TextureFilter.SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREFILTER));
    m_AssetQuality .SelectValue(Core::Config->GetInt(CORE_CONFIG_GRAPHICS_QUALITY));
    m_ShadowQuality.SelectValue(g_CurConfig.Graphics.iShadow);

    // 
    m_OverallVolume.SelectValue(F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME) * 100.0f));
    m_MusicVolume  .SelectValue(F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME)  * 100.0f));
    m_EffectVolume .SelectValue(F_TO_UI(Core::Config->GetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME)  * 100.0f));
    m_AmbientSound .SelectValue(g_CurConfig.Audio.iAmbient);

    // 
    const coreLookupStr<std::string>& asLanguageList = cMenu::GetLanguageList();
    m_Language.SelectIndex(std::find(asLanguageList.begin(), asLanguageList.end(), Core::Config->GetString(CORE_CONFIG_SYSTEM_LANGUAGE)) - asLanguageList.begin());

    // 
    if(m_SaveButton.GetOverride() >= 0)
    {
        if(iShadowQualityIndex != m_ShadowQuality.GetCurIndex()) this->__UpdateShadowQuality();
        if(iOverallVolumeIndex != m_OverallVolume.GetCurIndex()) this->__UpdateOverallVolume();
        if(iLanguageIndex      != m_Language     .GetCurIndex()) this->__UpdateLanguage();
    }

    // 
    m_SaveButton   .SetOverride(-1);
    m_DiscardButton.SetOverride(-1);
}


// ****************************************************************
// 
void cConfigMenu::SaveValues()
{
    const coreUint8&  iCurValue      = m_Resolution.GetCurEntry().second;
    const coreVector2 vCurResolution = (iCurValue == 0xFFu) ? coreVector2(0.0f,0.0f) : Core::System->GetAvailableRes()[iCurValue];

    // 
    const coreBool bReset = (vCurResolution != coreVector2(I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_WIDTH)), I_TO_F(Core::Config->GetInt(CORE_CONFIG_SYSTEM_HEIGHT)))) ||
                            (m_DisplayMode  .GetCurEntry().second != Core::Config->GetInt(CORE_CONFIG_SYSTEM_FULLSCREEN))     ||
                            (m_AntiAliasing .GetCurEntry().second != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING)) ||
                            (m_TextureFilter.GetCurEntry().second != Core::Config->GetInt(CORE_CONFIG_GRAPHICS_TEXTUREFILTER));

    // 
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_WIDTH,           F_TO_SI(vCurResolution.x));
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_HEIGHT,          F_TO_SI(vCurResolution.y));
    Core::Config->SetInt(CORE_CONFIG_SYSTEM_FULLSCREEN,      m_DisplayMode  .GetCurEntry().second);
    Core::Config->SetInt(CORE_CONFIG_GRAPHICS_ANTIALIASING,  m_AntiAliasing .GetCurEntry().second);
    Core::Config->SetInt(CORE_CONFIG_GRAPHICS_TEXTUREFILTER, m_TextureFilter.GetCurEntry().second);
    Core::Config->SetInt(CORE_CONFIG_GRAPHICS_QUALITY,       m_AssetQuality .GetCurEntry().second);
    g_CurConfig.Graphics.iShadow = m_ShadowQuality.GetCurEntry().second;

    // 
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_GLOBALVOLUME, I_TO_F(m_OverallVolume.GetCurEntry().second) * 0.01f);
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_MUSICVOLUME,  I_TO_F(m_MusicVolume  .GetCurEntry().second) * 0.01f);
    Core::Config->SetFloat(CORE_CONFIG_AUDIO_SOUNDVOLUME,  I_TO_F(m_EffectVolume .GetCurEntry().second) * 0.01f);
    g_CurConfig.Audio.iAmbient = m_AmbientSound.GetCurEntry().second;

    // 
    Core::Config->SetString(CORE_CONFIG_SYSTEM_LANGUAGE, Core::Language->GetPath());

    // 
    SaveConfig();
    this->CheckValues();

    // 
    if(bReset)
    {
        InitResolution((iCurValue == 0xFFu) ? Core::System->GetDesktopRes() : vCurResolution);
        Core::Reset();
        InitFramerate();
    }
}


// ****************************************************************
// 
void cConfigMenu::__UpdateShadowQuality()
{
    // 
    g_CurConfig.Graphics.iShadow = m_ShadowQuality.GetCurEntry().second;

    // 
    g_pEnvironment->GetBackground()->GetOutdoor()->GetShadowMap()->Reconfigure();
    cShadow::Recompile();
}


// ****************************************************************
// 
void cConfigMenu::__UpdateOverallVolume()
{
    // 
    Core::Audio->SetVolume(I_TO_F(m_OverallVolume.GetCurEntry().second) * 0.01f);
}


// ****************************************************************
// 
void cConfigMenu::__UpdateLanguage()
{
    // 
    const coreLookupStr<std::string>& asLanguageList = cMenu::GetLanguageList();
    Core::Language->Load(asLanguageList.at(m_Language.GetCurEntry().first->c_str()).c_str());

    // 
    g_pMenu->GetTooltip()->Invalidate();
}