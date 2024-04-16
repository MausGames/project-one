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
cIntroMission::cIntroMission()noexcept
: m_aafManualTime {}
, m_bFirstPlay    (g_pSave->GetHeader().oProgress.bFirstPlay)
{
    // 
    m_apBoss[0] = &m_Intro;

    // 
    for(coreUintW i = 0u; i < MISSION_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < INTRO_MANUALS; ++j)
        {
            m_aaManual[i][j].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        }
    }
}


// ****************************************************************
// 
void cIntroMission::__RenderOwnTop()
{
    // 
    for(coreUintW i = 0u; i < MISSION_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < INTRO_MANUALS; ++j)
        {
            m_aaManual[i][j].Render();
        }
    }
}


// ****************************************************************
// 
void cIntroMission::__MoveOwnAfter()
{
    // 
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreUint8 iType = g_CurConfig.Input.aiType[i];
        const auto&     oSet  = g_CurConfig.Input.aSet[iType];

        // 
        cConfigMenu::PrintFigure(&m_aaManual[i][0], iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveUp    : SDL_CONTROLLER_BUTTON_DPAD_UP);
        cConfigMenu::PrintFigure(&m_aaManual[i][1], iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveLeft  : SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        cConfigMenu::PrintFigure(&m_aaManual[i][2], iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveDown  : SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        cConfigMenu::PrintFigure(&m_aaManual[i][3], iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveRight : SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        cConfigMenu::PrintFigure(&m_aaManual[i][4], iType, oSet.aiAction[0]);
        cConfigMenu::PrintFigure(&m_aaManual[i][5], iType, oSet.aiAction[1]);
        cConfigMenu::PrintFigure(&m_aaManual[i][6], iType, oSet.aiAction[2]);

        // 
        constexpr coreVector2 avOffset[] =
        {
            coreVector2( 0.0f, 1.0f),
            coreVector2(-1.0f, 0.0f),
            coreVector2( 0.0f,-1.0f),
            coreVector2( 1.0f, 0.0f),
            coreVector2( 1.0f, 0.0f),
            coreVector2(-1.0f, 0.5f),
            coreVector2( 1.0f, 0.5f)
        };

        // 
        const coreBool abPress[] =
        {
            (pPlayer->GetInput()->vMove.y > 0.0f),
            (pPlayer->GetInput()->vMove.x < 0.0f),
            (pPlayer->GetInput()->vMove.y < 0.0f),
            (pPlayer->GetInput()->vMove.x > 0.0f),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_SHOOT(0u, 0u))),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_TURN_LEFT)),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_TURN_RIGHT))
        };

        // 
        for(coreUintW j = 0u; j < INTRO_MANUALS; ++j)
        {
            coreFlow& fTime = m_aafManualTime[i][j];
            if(fTime)
            {
                // 
                fTime.UpdateMax(-1.0f, 0.0f);

                // 
                m_aaManual[i][j].SetPosition(avOffset[j] * 0.1f);
                m_aaManual[i][j].SetSize    (coreVector2(0.1f,0.1f) * (abPress[j] ? 0.8f : 1.0f));
                m_aaManual[i][j].SetCenter  (g_pForeground->Project2D(pPlayer->GetPosition()));
                m_aaManual[i][j].SetAlpha   (BLENDH3(MIN1(fTime)));
                m_aaManual[i][j].SetEnabled (fTime ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aaManual[i][j].Move();
            }
        }
    });
}