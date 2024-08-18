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
cMusicBox::cMusicBox()noexcept
: m_iSelection (MUSIC_DEFAULT)
, m_fDelay     (0.0f)
{
    // 
    this->SetAlpha(0.0f);   // due to late initialization

    // 
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_grey_vignette_program");
    m_Background.SetSize      (coreVector2(1.0f,1.0f));
    m_Background.SetTexSize   (coreVector2(1.2f,1.2f));

    // 
    m_Header.Construct  (MENU_FONT_DYNAMIC_4, MENU_OUTLINE_SMALL);
    m_Header.SetPosition(coreVector2(0.0f, 0.415f));
    m_Header.SetColor3  (COLOR_MENU_WHITE);

    // 
    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (coreVector2(0.415f,-0.42f));
    m_BackButton.SetSize      (coreVector2(0.07f,  0.07f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);
    
#if defined(_CORE_DEBUG_)
    // 
    m_BigTitle.Construct   (MENU_FONT_STANDARD_3, MENU_OUTLINE_SMALL);
    m_BigTitle.SetPosition (coreVector2(-0.45f,-0.402f));
    m_BigTitle.SetAlignment(coreVector2(1.0f,0.0f));

    // 
    m_BigArtist.Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
    m_BigArtist.SetPosition (m_BigTitle.GetPosition () + coreVector2(0.002f,-0.042f));
    m_BigArtist.SetAlignment(m_BigTitle.GetAlignment());
    m_BigArtist.SetColor3   (COLOR_MENU_WHITE);
#endif

    for(coreUintW i = 0u; i < MUSIC_TRACKS; ++i)
    {
        const coreFloat fHeight = 0.315f - 0.06f*I_TO_F(i);

        // 
        m_aTitle[i].Construct   (MENU_FONT_STANDARD_2, MENU_OUTLINE_SMALL);
        m_aTitle[i].SetPosition (coreVector2(-0.45f, fHeight));
        m_aTitle[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aTitle[i].SetText     (g_aMusicTrack[i].pcTitle);

        // 
        m_aContext[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aContext[i].SetPosition (coreVector2(0.02f, fHeight));
        m_aContext[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aContext[i].SetText     (g_aMusicTrack[i].pcContext);

        // 
        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, fHeight));
        m_aLine[i].SetSize      (coreVector2(1.1f, 0.06f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
        m_aLine[i].SetFocusable (true);

        // 
        m_aFocus[i].DefineTexture(0u, "default_black.webp");   // invisible
        m_aFocus[i].DefineProgram("menu_single_program");
        m_aFocus[i].SetPosition  (coreVector2(-0.025f, fHeight));
        m_aFocus[i].SetSize      (coreVector2(0.88f,0.06f));
    }

    // 
    m_PlayIcon.Construct (MENU_FONT_ICON_4, MENU_OUTLINE_SMALL);
    m_PlayIcon.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    m_PlayIcon.SetText   (">");

    for(coreUintW i = 0u; i < MUSIC_ICONS; ++i)
    {
        // 
        m_aMusicIcon[i].Construct  (MENU_FONT_ICON_3, MENU_OUTLINE_SMALL);
        m_aMusicIcon[i].SetEnabled (CORE_OBJECT_ENABLE_NOTHING);
        m_aMusicIcon[i].SetText    (ICON_MUSIC);
        m_aMusicIcon[i].SetRectifyY(false);
    }

    // 
    m_ScrollBox.SetPosition (coreVector2(0.0f,0.0f));
    m_ScrollBox.SetSize     (coreVector2(1.0f,0.69f));
    m_ScrollBox.SetAlpha    (0.0f);   // due to late initialization
    m_ScrollBox.SetMaxOffset(0.06f * I_TO_F(MUSIC_TRACKS) - m_ScrollBox.GetSize().y);
    m_ScrollBox.SetInside   (true);
    for(coreUintW i = 0u; i < MUSIC_TRACKS; ++i) m_ScrollBox.BindObject(&m_aFocus    [i]);
    for(coreUintW i = 0u; i < MUSIC_TRACKS; ++i) m_ScrollBox.BindObject(&m_aLine     [i]);
    for(coreUintW i = 0u; i < MUSIC_TRACKS; ++i) m_ScrollBox.BindObject(&m_aTitle    [i]);
    for(coreUintW i = 0u; i < MUSIC_TRACKS; ++i) m_ScrollBox.BindObject(&m_aContext  [i]);
    m_ScrollBox.BindObject(&m_PlayIcon);
    for(coreUintW i = 0u; i < MUSIC_ICONS;  ++i) m_ScrollBox.BindObject(&m_aMusicIcon[i]);

    // 
    for(coreUintW i = 0u; i < MUSIC_TRACKS; ++i)
    {
        m_Navigator.BindObject(&m_aFocus[i], (i == 0u)                ? s_cast<coreObject2D*>(&m_BackButton) : &m_aFocus[i - 1u], NULL,
                                             (i == MUSIC_TRACKS - 1u) ? s_cast<coreObject2D*>(&m_BackButton) : &m_aFocus[i + 1u], NULL, MENU_TYPE_DEFAULT);
    }
    m_Navigator.BindObject(&m_BackButton, &m_aFocus[MUSIC_TRACKS - 1u], NULL, &m_aFocus[0], NULL, MENU_TYPE_DEFAULT);
    m_Navigator.BindScroll(&m_ScrollBox);

    // 
    m_Navigator.AssignFirst(&m_aFocus[0]);
    m_Navigator.AssignBack (&m_BackButton);
}


// ****************************************************************
// render the music box
void cMusicBox::Render()
{
    // 
    m_Background.SetAlpha(this->GetAlpha() * 0.5f);
    m_Background.Render();

    // 
    m_BackButton.SetAlpha(this->GetAlpha());   // # before
    m_BackButton.Render();

    // 
    m_Header.SetAlpha(this->GetAlpha());
    m_Header.Render();
    
#if defined(_CORE_DEBUG_)
    // 
    m_BigTitle.SetAlpha(this->GetAlpha());
    m_BigTitle.Render();

    // 
    m_BigArtist.SetAlpha(this->GetAlpha());
    m_BigArtist.Render();
#endif

    // 
    m_ScrollBox.Render();

    // 
    m_Navigator.Render();
}


// ****************************************************************
// move the music box
void cMusicBox::Move()
{
    // 
    m_Navigator.Update();

    // 
    m_ScrollBox.Interact();

    if(TIME)
    {
        // 
        m_PlayIcon.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);

        for(coreUintW i = 0u; i < MUSIC_TRACKS; ++i)
        {
            // 
            if(m_ScrollBox.IsBarFocused()) m_aLine[i].SetFocused(false);
            cMenu::UpdateLine(&m_aLine[i], false, g_aMusicTrack[i].vColor);

            if(m_aLine[i].IsClicked())
            {
                // 
                m_iSelection = i;
                m_fDelay     = 0.0f;

                // 
                g_MusicPlayer.SelectName(g_aMusicTrack[i].pcFile);
                //g_MusicPlayer.GetCurMusic()->Rewind();
            }

            if(m_aLine[i].IsFocused())
            {
                if(i != m_iSelection)
                {
                    // 
                    m_PlayIcon.SetPosition(coreVector2(0.38f, m_aLine[i].GetPosition().y));
                    m_PlayIcon.SetColor3  (g_aMusicTrack[i].vColor);
                    m_PlayIcon.SetEnabled (CORE_OBJECT_ENABLE_ALL);
                }

                // 
                m_aTitle  [i].SetColor3(g_aMusicTrack[i].vColor);
                m_aContext[i].SetColor3(g_aMusicTrack[i].vColor * MENU_LIGHT_IDLE);

#if defined(_CORE_DEBUG_)
                // 
                m_BigTitle .SetColor3(g_aMusicTrack[i].vColor);
                m_BigTitle .SetText  (g_aMusicTrack[i].pcTitle);
                m_BigArtist.SetText  (g_aMusicTrack[i].pcArtist);
#endif
            }
            else
            {
                // 
                m_aTitle  [i].SetColor3(COLOR_MENU_WHITE);
                m_aContext[i].SetColor3(COLOR_MENU_WHITE * MENU_LIGHT_IDLE);
            }
        }
    }

    if(m_iSelection < MUSIC_TRACKS)
    {
        for(coreUintW i = 0u; i < MUSIC_ICONS; ++i)
        {
            const coreFloat fHeight = 0.006f * SIN(Core::System->GetTotalTimeFloat(2.0f*PI) * 5.0f + I_TO_F(i));

            // 
            m_aMusicIcon[i].SetPosition(coreVector2(0.375f - 0.06f * I_TO_F(i), m_aLine[m_iSelection].GetPosition().y + fHeight));
            m_aMusicIcon[i].SetColor3  (g_aMusicTrack[m_iSelection].vColor);
            m_aMusicIcon[i].SetEnabled (CORE_OBJECT_ENABLE_ALL);
        }

#if defined(_CORE_DEBUG_)
        // 
        m_fDelay.Update(1.0f);
        if(m_fDelay >= 2.0f)
        {
            const coreInt32 iBackground = g_aMusicTrack[m_iSelection].iBackground;
            if(iBackground && (g_pEnvironment->GetBackground()->GetID() != iBackground))
            {
                g_pEnvironment->ChangeBackground(iBackground, ENVIRONMENT_MIX_FADE, 0.5f);
            }
        }
#endif
    }

    // 
    m_BackButton.Interact();
    cMenu::UpdateButton(&m_BackButton, &m_Navigator, m_BackButton.IsFocused());

    // 
    m_Background.SetTexOffset(coreVector2(0.0f, MENU_LAYER_TEXOFFSET));
    m_Background.Move();

    // 
    m_Header    .Move();
    m_BackButton.Move();
    
#if defined(_CORE_DEBUG_)
    // 
    m_BigTitle .Move();
    m_BigArtist.Move();
#endif

    // 
    m_ScrollBox.SetAlpha(this->GetAlpha());
    m_ScrollBox.Move();

    // 
    m_Navigator.SetAlpha(this->GetAlpha());
    m_Navigator.Move();
}


// ****************************************************************
// 
void cMusicBox::ResetSelection()
{
    // 
    m_iSelection = MUSIC_DEFAULT;

    // 
    for(coreUintW i = 0u; i < MUSIC_ICONS; ++i)
    {
        m_aMusicIcon[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMusicBox::ResetNavigator()
{
    if(m_iSelection < MUSIC_TRACKS)
    {
        // 
        m_ScrollBox.ScrollToObject(&m_aFocus[m_iSelection], true);

        // 
        m_Navigator.ForceCurrent(&m_aFocus[m_iSelection]);
    }
    else
    {
        // 
        m_Navigator.ResetFirst();

        // 
        m_ScrollBox.SetCurOffset(0.0f);
    }

    // 
    m_Header.SetText(coreData::StrToUpperUTF8(Core::Language->GetString("EXTRA_MUSIC")));
}