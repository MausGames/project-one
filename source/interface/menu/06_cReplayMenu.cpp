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
cReplayMenu::cReplayMenu()noexcept
: coreMenu (SURFACE_REPLAY_MAX, SURFACE_REPLAY_DEFAULT)
{
    // create menu objects
    m_Background.DefineTexture(0u, "menu_background_black.png");
    m_Background.DefineProgram("menu_border_program");
    m_Background.SetPosition  (coreVector2(0.0f,0.0f));
    m_Background.SetSize      (coreVector2(0.8f,0.7f));

    m_BackButton.Construct    (MENU_BUTTON, MENU_FONT_ICON_2, MENU_OUTLINE_SMALL);
    m_BackButton.DefineProgram("menu_border_program");
    m_BackButton.SetPosition  (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(0.5f,-0.5f) + coreVector2(0.0f,-0.02f));
    m_BackButton.SetSize      (coreVector2( 0.07f, 0.07f)); // * m_SaveButton.GetSize().y); 
    m_BackButton.SetAlignment (coreVector2(-1.0f, -1.0f));
    m_BackButton.GetCaption()->SetText(ICON_SHARE);

    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        m_aName[i].Construct   (MENU_FONT_DYNAMIC_1, MENU_OUTLINE_SMALL);
        m_aName[i].SetPosition (m_Background.GetPosition() + m_Background.GetSize()*coreVector2(-0.5f,0.5f) + coreVector2(0.04f, -0.05f - 0.05f*I_TO_F(i)));
        m_aName[i].SetAlignment(coreVector2(1.0f,0.0f));
        m_aName[i].SetColor3   (COLOR_MENU_WHITE);

        m_aTime[i].Construct   (MENU_FONT_STANDARD_1, MENU_OUTLINE_SMALL);
        m_aTime[i].SetPosition (coreVector2(-1.0f,1.0f) * m_aName[i].GetPosition());
        m_aTime[i].SetAlignment(coreVector2(-1.0f,0.0f));
        m_aTime[i].SetColor3   (COLOR_MENU_WHITE);

        m_aLine[i].DefineTexture(0u, "menu_detail_03.png");
        m_aLine[i].DefineTexture(1u, "menu_background_black.png");
        m_aLine[i].DefineProgram("menu_inner_program");
        m_aLine[i].SetPosition  (coreVector2(0.0f, m_aName[i].GetPosition().y));
        m_aLine[i].SetSize      (coreVector2(m_Background.GetSize().x, 0.05f));
        m_aLine[i].SetTexOffset (coreVector2(I_TO_F(i)*0.09f, 0.0f));
    }

    // bind menu objects
    for(coreUintW i = 0u; i < SURFACE_REPLAY_MAX; ++i)
    {
        this->BindObject(i, &m_Background);
        this->BindObject(i, &m_BackButton);
    }

    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) this->BindObject(SURFACE_REPLAY_DEFAULT, &m_aLine[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) this->BindObject(SURFACE_REPLAY_DEFAULT, &m_aName[i]);
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i) this->BindObject(SURFACE_REPLAY_DEFAULT, &m_aTime[i]);
}


// ****************************************************************
// move the replay menu
void cReplayMenu::Move()
{
    // move the menu
    this->coreMenu::Move();
    m_iStatus = MAX(m_iStatus - 100, 0);

    // 
    switch(this->GetCurSurface())
    {
    case SURFACE_REPLAY_DEFAULT:
        {
            // 
            for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
            {
                if(m_aName[i].IsClicked() || m_aTime[i].IsClicked() || m_aLine[i].IsClicked())
                {
                    if(i < m_aInfoList.size())
                    {
                        // 
                        g_pMenu->GetMsgBox()->ShowQuestion(PRINT("%s [%s] ?", Core::Language->GetString("QUESTION_LOAD"), m_aInfoList[i].oHeader.acName), [=, this](const coreInt32 iAnswer)
                        {
                            if(iAnswer == MSGBOX_ANSWER_YES)
                            {
                                if(g_pReplay->LoadFile(m_aInfoList[i].sPath.c_str(), false))
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
                        break;
                    }
                }
            }
        }
        break;

    default:
        ASSERT(false)
        break;
    }

    if(m_BackButton.IsClicked() || g_MenuInput.bCancel)
    {
        // 
        m_iStatus = 2;
    }

    // 
    cMenu::UpdateButton(&m_BackButton, m_BackButton.IsFocused());

    // 
    if(m_BackButton.IsFocused()) g_pMenu->GetTooltip()->ShowText(TOOLTIP_OBJECT(m_BackButton), TOOLTIP_ONELINER, Core::Language->GetString("BACK"));
}


// ****************************************************************
//
void cReplayMenu::LoadReplays()
{
    // 
    m_aInfoList.clear();
    cReplay::LoadInfoList(&m_aInfoList);

    // 
    for(coreUintW i = 0u, ie = MIN(m_aInfoList.size(), MENU_REPLAY_ENTRIES); i < ie; ++i)
    {
        const cReplay::sHeader& oHeader = m_aInfoList[i].oHeader;

        m_aName[i].SetText(oHeader.acName);
        m_aTime[i].SetText(coreData::TimeString(TIMEMAP_GM(oHeader.iEndTimestamp - oHeader.iStartTimestamp)));
    }

    // 
    for(coreUintW i = 0u; i < MENU_REPLAY_ENTRIES; ++i)
    {
        const coreObjectEnable eEnabled = (i < m_aInfoList.size()) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING;

        m_aName[i].SetEnabled(eEnabled);
        m_aTime[i].SetEnabled(eEnabled);
        m_aLine[i].SetEnabled(eEnabled);
    }
}