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
cReplayMenu::cReplayMenu()noexcept
: coreMenu    (SURFACE_REPLAY_MAX, SURFACE_REPLAY_OVERVIEW)
, m_iSelected (0u)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.01f));
    m_Background.SetSize      (coreVector2(0.9f,0.75f));

    m_DefaultTab.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_DefaultTab.DefineProgram("menu_border_program");
    m_DefaultTab.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.0125f));
    m_DefaultTab.SetSize      (coreVector2(0.84f,0.07f));
    m_DefaultTab.SetAlignment (coreVector2(0.0f, 1.0f));
    m_DefaultTab.SetTexSize   (coreVector2(1.0f,-1.0f));
    m_DefaultTab.SetTexOffset (m_DefaultTab.GetSize()*coreVector2(-0.5f,-1.0f) + coreVector2(m_Background.GetSize().x * 0.5f, 0.0125f));
    m_DefaultTab.GetCaption()->SetTextLanguage("REPLAYS");

    m_StartButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_StartButton.DefineProgram("menu_border_program");
    m_StartButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_StartButton.SetSize      (coreVector2(0.3f,0.07f));
    m_StartButton.SetAlignment (coreVector2(1.0f,-1.0f));
    m_StartButton.GetCaption()->SetTextLanguage("START");

    m_DeleteButton.Construct    (MENU_BUTTON, MENU_FONT_DYNAMIC_2, MENU_OUTLINE_SMALL);
    m_DeleteButton.DefineProgram("menu_border_program");
    m_DeleteButton.SetPosition  (m_StartButton.GetPosition() + coreVector2(m_StartButton.GetSize().x + 0.02f, 0.0f));
    m_DeleteButton.SetSize      (m_StartButton.GetSize());
    m_DeleteButton.SetAlignment (m_StartButton.GetAlignment());
    m_DeleteButton.GetCaption()->SetTextLanguage("DELETE");

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 1.0f, 1.0f) * m_StartButton.GetSize().y);
    m_BackButton.SetAlignment (coreVector2(-1.0f, -1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    m_Page.Construct  (MENU_SWITCHBOX, MENU_FONT_DYNAMIC_1, MENU_FONT_ICON_1, MENU_OUTLINE_SMALL);
    m_Page.SetPosition(m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.0f,0.5f) + coreVector2(0.0f,-0.05f));
    m_Page.SetSize    (coreVector2(0.47f,0.03f));
    m_Page.SetEndless (true);
    m_Page.GetCaption()->SetColor3(COLOR_MENU_WHITE);

    m_PageLine.DefineTexture(0u, "menu_detail_04.png");
    m_PageLine.DefineTexture(1u, "menu_background_black.png");
    m_PageLine.DefineProgram("menu_inner_program");
    m_PageLine.SetPosition  (coreVector2(0.0f, m_Page.GetPosition().y));
    m_PageLine.SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
    m_PageLine.SetFocusable (true);

    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        m_aName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.075f - 0.05f*I_TO_F(i + 1u)));
        m_aName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aTime[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aTime[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aName[i].GetPosition());
        m_aTime[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aTime[i].SetColor3   (COLOR_MENU_WHITE);

        m_aLine[i].DefineTexture(0u, "menu_detail_04.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aName[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i + 1u)*0.09f, 0.0f));
        m_aLine[i].SetFocusable (true);
    }

    // 
    m_Navigator.BindObject(&m_BackButton, NULL, NULL, NULL, NULL, MENU_TYPE_DEFAULT);

    m_Navigator.AssignFirst(&m_BackButton);
    m_Navigator.AssignBack (&m_BackButton);
    m_Navigator.AssignMenu (this);

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_REPLAY_MAX; ++i)
    {
        this->BindObject(i, &m_Background);
        if(i == SURFACE_REPLAY_OVERVIEW) this->BindObject(i, &m_DefaultTab);
        this->BindObject(i, &m_BackButton);
        this->BindObject(i, &m_Navigator);
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_aLine[i]);
    this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_PageLine);
    this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_Page);
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_aName[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) this->BindObject(SURFACE_REPLAY_OVERVIEW, &m_aTime[i]);

    this->BindObject(SURFACE_REPLAY_DETAILS, &m_StartButton);
    this->BindObject(SURFACE_REPLAY_DETAILS, &m_DeleteButton);
}


// ****************************************************************
// move the replay menu
void cReplayMenu::Move()
{
    // 
    m_Navigator.Update();

    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_REPLAY_OVERVIEW:
        {
            if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                m_iStatus = 2;
            }

            // 
            for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
            {
                if(m_aName[i].IsClicked() || m_aTime[i].IsClicked() || m_aLine[i].IsClicked())
                {
                    if(i < m_aInfoList.size())
                    {
                        // 
                        g_pMenu->ShiftSurface(this, SURFACE_REPLAY_DETAILS, 3.0f, 1u, false, false);

                        // 
                        this->LoadDetails(i);
                        break;
                    }
                }
            }

            // 
            cMenu::UpdateSwitchBox(&m_Page);
        }
        break;

    case SURFACE_REPLAY_DETAILS:
        {
            if(m_StartButton.IsClicked())
            {
                ASSERT(m_iSelected < m_aInfoList.size())
                const cReplay::sInfo& oInfo = m_aInfoList[m_iSelected];

                // 
                g_pMenu->GetMsgBox()->ShowQuestion(PRINT("%s [%s] ?", Core::Language->GetString("QUESTION_LOAD"), oInfo.oHeader.acName), [=, this](const coreInt32 iAnswer)
                {
                    if(iAnswer == MSGBOX_ANSWER_YES)
                    {
                        if(g_pReplay->LoadFile(oInfo.sPath.c_str(), false))
                        {
                            // 
                            m_iStatus = 101;
                        }
                        else
                        {
                            // 
                            g_pMenu->GetMsgBox()->ShowInformation(Core::Language->GetString("INFORMATION_REPLAY_CORRUPT"), MSGBOX_NO_CALLBACK);
                        }
                    }
                });
            }
            else if(m_DeleteButton.IsClicked())
            {
            }
            else if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
            {
                // 
                g_pMenu->ShiftSurface(this, SURFACE_REPLAY_OVERVIEW, 3.0f, 2u, false, false);
            }

            // 
            cMenu::UpdateButton(&m_StartButton,  &m_Navigator, m_StartButton .IsFocused());
            cMenu::UpdateButton(&m_DeleteButton, &m_Navigator, m_DeleteButton.IsFocused());
        }
        break;

    default:
        UNREACHABLE
        break;
    }

    // 
    cMenu::UpdateTab(&m_DefaultTab, true, true);

    // 
    cMenu::UpdateButton(&m_BackButton, &m_Navigator, m_BackButton.IsFocused());

    
    
    m_PageLine.Interact();
    m_PageLine.SetColor3(m_PageLine.IsFocused() ? g_pMenu->GetHighlightColor() : coreVector3(1.0f,1.0f,1.0f));
    
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        m_aLine[i].Interact();
        m_aLine[i].SetColor3(m_aLine[i].IsFocused() ? g_pMenu->GetHighlightColor() : coreVector3(1.0f,1.0f,1.0f));
    }
}


// ****************************************************************
//
void cReplayMenu::LoadOverview()
{
    // 
    m_aInfoList.clear();
    cReplay::LoadInfoList(&m_aInfoList);

    // 
    for(coreUintW i = 0u, ie = MIN(m_aInfoList.size(), MENU_REPLAY_ENTRIES); i < ie; ++i)
    {
        const cReplay::sHeader& oHeader = m_aInfoList[i].oHeader;

        m_aName[i].SetText(oHeader.acName);
        m_aTime[i].SetText(coreData::DateTimePrint("%Y-%m-%d %H:%M", TIMEMAP_GM(oHeader.iStartTimestamp)));
    }

    // 
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        const coreObjectEnable eEnabled = (i < m_aInfoList.size()) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

        m_aName[i].SetEnabled(eEnabled);
        m_aTime[i].SetEnabled(eEnabled);
        m_aLine[i].SetEnabled(eEnabled);
    }

    // 
    m_Page.ClearEntries();
    for(coreUintW i = 0u; i < 5u; ++i) m_Page.AddEntry(PRINT("%zu / %u", i + 1u, 5u), 0u);
}


// ****************************************************************
//
void cReplayMenu::LoadDetails(const coreUintW iIndex)
{
    ASSERT(iIndex < m_aInfoList.size())
    
    m_iSelected = iIndex;
}