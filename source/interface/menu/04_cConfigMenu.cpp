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
    m_SaveButton.GetCaption()->SetTextLanguage("SAVE");

    m_CancelButton.Construct    (MENU_BUTTON, MENU_FONT_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_CancelButton.DefineProgram("menu_border_program");
    m_CancelButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_CancelButton.SetSize      (m_SaveButton.GetSize());
    m_CancelButton.SetAlignment (coreVector2(-1.0f,-1.0f));
    m_CancelButton.GetCaption()->SetTextLanguage("CANCEL");


    coreUint8 iOffset = 0u;
    for(coreUintW i = 0u; i < MENU_CONFIG_MAX; ++i)
    {
        if(i == MENU_CONFIG_VIDEO || i == MENU_CONFIG_AUDIO || i == MENU_CONFIG_INPUT) iOffset = 0u;
        if(i == MENU_CONFIG_VIDEO_ANTIALIASING) ++iOffset;
        if(i == MENU_CONFIG_VIDEO_ASSETQUALITY) ++iOffset;
        if(i == MENU_CONFIG_AUDIO_MUSICVOLUME)  ++iOffset;
        if(i == MENU_CONFIG_AUDIO_AMBIENTSOUND) ++iOffset;

        m_aLabel[i].Construct   (MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
        m_aLabel[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.025f*I_TO_F(iOffset)));
        m_aLabel[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aLabel[i].SetColor3   (coreVector3(1.0f,1.0f,1.0f) * MENU_CONTRAST_WHITE);

        iOffset += 2u;
    }

    #define SET_TEXT(x) m_aLabel[MENU_CONFIG_ ## x].SetTextLanguage("CONFIG_" ## #x);
    {
        SET_TEXT(VIDEO_RESOLUTION)
        SET_TEXT(VIDEO_DISPLAYMODE)
        SET_TEXT(VIDEO_ANTIALIASING)
        SET_TEXT(VIDEO_TEXTUREFILTER)
        SET_TEXT(VIDEO_ASSETQUALITY)
        SET_TEXT(VIDEO_SHADOWQUALITY)
        SET_TEXT(AUDIO_OVERALLVOLUME)
        SET_TEXT(AUDIO_MUSICVOLUME)
        SET_TEXT(AUDIO_EFFECTVOLUME)
        SET_TEXT(AUDIO_AMBIENTSOUND)
        SET_TEXT(GAME_LANGUAGE)
    }
    #undef SET_TEXT


    for(coreUintW i = 0u; i < MENU_CONFIG_MAX; ++i)
    {
        m_aLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aLabel[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.1f, 0.0f));
    }

    m_Resolution.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_Resolution.SetPosition (m_aLabel[MENU_CONFIG_VIDEO_RESOLUTION].GetPosition() * coreVector2(-1.0f,1.0f));
    m_Resolution.SetSize     (coreVector2(0.25f,0.03f));
    m_Resolution.SetAlignment(coreVector2(-1.0f,0.0f));
    m_Resolution.GetCaption()->SetColor3(coreVector3(1.0f,1.0f,1.0f) * MENU_CONTRAST_WHITE);

    m_DisplayMode.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_DisplayMode.SetPosition (m_aLabel[MENU_CONFIG_VIDEO_DISPLAYMODE].GetPosition() * coreVector2(-1.0f,1.0f));
    m_DisplayMode.SetSize     (m_Resolution.GetSize());
    m_DisplayMode.SetAlignment(m_Resolution.GetAlignment());
    m_DisplayMode.GetCaption()->SetColor3(m_Resolution.GetCaption()->GetColor3());

    m_AntiAliasing.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_AntiAliasing.SetPosition (m_aLabel[MENU_CONFIG_VIDEO_ANTIALIASING].GetPosition() * coreVector2(-1.0f,1.0f));
    m_AntiAliasing.SetSize     (coreVector2(0.15f,0.03f));
    m_AntiAliasing.SetAlignment(m_Resolution.GetAlignment());
    m_AntiAliasing.GetCaption()->SetColor3(m_Resolution.GetCaption()->GetColor3());

    m_TextureFilter.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_TextureFilter.SetPosition (m_aLabel[MENU_CONFIG_VIDEO_TEXTUREFILTER].GetPosition() * coreVector2(-1.0f,1.0f));
    m_TextureFilter.SetSize     (m_AntiAliasing.GetSize());
    m_TextureFilter.SetAlignment(m_Resolution.GetAlignment());
    m_TextureFilter.GetCaption()->SetColor3(m_Resolution.GetCaption()->GetColor3());

    m_AssetQuality.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_AssetQuality.SetPosition (m_aLabel[MENU_CONFIG_VIDEO_ASSETQUALITY].GetPosition() * coreVector2(-1.0f,1.0f));
    m_AssetQuality.SetSize     (m_AntiAliasing.GetSize());
    m_AssetQuality.SetAlignment(m_Resolution.GetAlignment());
    m_AssetQuality.GetCaption()->SetColor3(m_Resolution.GetCaption()->GetColor3());

    m_ShadowQuality.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_ShadowQuality.SetPosition (m_aLabel[MENU_CONFIG_VIDEO_SHADOWQUALITY].GetPosition() * coreVector2(-1.0f,1.0f));
    m_ShadowQuality.SetSize     (m_AntiAliasing.GetSize());
    m_ShadowQuality.SetAlignment(m_Resolution.GetAlignment());
    m_ShadowQuality.GetCaption()->SetColor3(m_Resolution.GetCaption()->GetColor3());

    m_AmbientSound.Construct(MENU_SWITCHBOX, MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL);
    m_AmbientSound.SetPosition (m_aLabel[MENU_CONFIG_AUDIO_AMBIENTSOUND].GetPosition() * coreVector2(-1.0f,1.0f));
    m_AmbientSound.SetSize     (coreVector2(0.03f,0.03f));
    m_AmbientSound.SetAlignment(m_Resolution.GetAlignment());
    //m_AmbientSound.GetCaption()->SetColor3(m_Resolution.GetCaption()->GetColor3());

    m_Language.Construct   (MENU_SWITCHBOX, MENU_FONT_SMALL, MENU_OUTLINE_SMALL, 0u);
    m_Language.SetPosition (m_aLabel[MENU_CONFIG_GAME_LANGUAGE].GetPosition() * coreVector2(-1.0f,1.0f));
    m_Language.SetSize     (coreVector2(0.25f,0.03f));
    m_Language.SetAlignment(m_Resolution.GetAlignment());
    m_Language.GetCaption()->SetColor3(m_Resolution.GetCaption()->GetColor3());

    const std::vector<coreVector2>& avAvailable = Core::System->GetAvailable();
    FOR_EACH_REV(it, avAvailable)
    {
        m_Resolution.AddEntry(PRINT("%.0f x %.0f", it->x, it->y), avAvailable.size() - (it - avAvailable.rbegin()));
    }
    m_Resolution.AddEntryLanguage("DISPLAY_DESKTOP", 0xFFu);

    m_DisplayMode.AddEntryLanguage("DISPLAY_WINDOW",     0u);
    m_DisplayMode.AddEntryLanguage("DISPLAY_FULLSCREEN", 1u);

    m_AntiAliasing .AddEntryLanguage("VALUE_OFF", 0u);
    m_TextureFilter.AddEntryLanguage("VALUE_OFF", 0u);
    for(coreUintW i = 2u; i <=  8u; i <<= 1u) m_AntiAliasing .AddEntry(PRINT("x%u", i), i);
    for(coreUintW i = 2u; i <= 16u; i <<= 1u) m_TextureFilter.AddEntry(PRINT("x%u", i), i);

    m_AssetQuality .AddEntryLanguage("VALUE_LOW",  0u);
    m_AssetQuality .AddEntryLanguage("VALUE_HIGH", 1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_OFF",  0u);
    m_ShadowQuality.AddEntryLanguage("VALUE_LOW",  1u);
    m_ShadowQuality.AddEntryLanguage("VALUE_HIGH", 2u);

    // 
    const coreLookupStr<std::string>& asLanguageList = cMenu::GetLanguageList();

    // 
    FOR_EACH(it, asLanguageList.get_keylist())
        m_Language.AddEntry(it->c_str(), 0u);





    


    

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
        this->BindObject(i, &m_CancelButton);
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
    this->BindObject(SURFACE_CONFIG_AUDIO, &m_AmbientSound);
    this->BindObject(SURFACE_CONFIG_GAME,  &m_Language);
}


// ****************************************************************
// move the config menu
void cConfigMenu::Move()
{
    // move the menu
    coreMenu::Move();
    m_iStatus = 0;


         if(m_VideoTab.IsClicked()) this->ChangeSurface(SURFACE_CONFIG_VIDEO, 0.0f);
    else if(m_AudioTab.IsClicked()) this->ChangeSurface(SURFACE_CONFIG_AUDIO, 0.0f);
    else if(m_InputTab.IsClicked()) this->ChangeSurface(SURFACE_CONFIG_INPUT, 0.0f);
    else if(m_GameTab .IsClicked()) this->ChangeSurface(SURFACE_CONFIG_GAME,  0.0f);

    cMenu::UpdateButton(&m_VideoTab, (this->GetCurSurface() == SURFACE_CONFIG_VIDEO) || m_VideoTab.IsFocused());
    cMenu::UpdateButton(&m_AudioTab, (this->GetCurSurface() == SURFACE_CONFIG_AUDIO) || m_AudioTab.IsFocused());
    cMenu::UpdateButton(&m_InputTab, (this->GetCurSurface() == SURFACE_CONFIG_INPUT) || m_InputTab.IsFocused());
    cMenu::UpdateButton(&m_GameTab,  (this->GetCurSurface() == SURFACE_CONFIG_GAME ) || m_GameTab .IsFocused());


    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_CONFIG_VIDEO:
        {
            cMenu::UpdateSwitchBox(&m_Resolution);
            cMenu::UpdateSwitchBox(&m_DisplayMode);
            cMenu::UpdateSwitchBox(&m_AntiAliasing);
            cMenu::UpdateSwitchBox(&m_TextureFilter);
            cMenu::UpdateSwitchBox(&m_AssetQuality);
            cMenu::UpdateSwitchBox(&m_ShadowQuality);

                 if(m_AssetQuality.GetCurIndex() == 0u) m_AssetQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_AssetQuality.GetCurIndex() == 1u) m_AssetQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);

                 if(m_ShadowQuality.GetCurIndex() == 0u) m_ShadowQuality.GetCaption()->SetColor3(coreVector3(1.0f,1.0f,1.0f) * MENU_CONTRAST_WHITE);
            else if(m_ShadowQuality.GetCurIndex() == 1u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_YELLOW);
            else if(m_ShadowQuality.GetCurIndex() == 2u) m_ShadowQuality.GetCaption()->SetColor3(COLOR_MENU_GREEN);
        }
        break;

    case SURFACE_CONFIG_AUDIO:
        {
            cMenu::UpdateCheckBox(&m_AmbientSound);
        }
        break;

    case SURFACE_CONFIG_INPUT:
        {

        }
        break;

    case SURFACE_CONFIG_GAME:
        {
            cMenu::UpdateSwitchBox(&m_Language);
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    // 
    cMenu::UpdateButton(&m_SaveButton,   m_SaveButton  .IsFocused());
    cMenu::UpdateButton(&m_CancelButton, m_CancelButton.IsFocused());

    



    if(this->GetAlpha() >= 1.0f)
    {
        if(m_SaveButton.IsClicked())
        {
            // 
            SaveConfig();
        }
        else if(m_CancelButton.IsClicked())
        {
            // 
            LoadConfig();

            // 
            m_iStatus = 1;
        }
    }
}