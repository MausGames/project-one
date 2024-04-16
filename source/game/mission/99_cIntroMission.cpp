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
, m_vSkewerColor  (COLOR_SHIP_PURPLE)
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
    if(g_CurConfig.Game.iMirrorMode) glDisable(GL_CULL_FACE);

    // 
    for(coreUintW i = 0u; i < MISSION_PLAYERS; ++i)
    {
        for(coreUintW j = 0u; j < INTRO_MANUALS; ++j)
        {
            m_aaManual[i][j].Render();
        }
    }

    // 
    if(g_CurConfig.Game.iMirrorMode) glEnable(GL_CULL_FACE);
}


// ****************************************************************
// 
void cIntroMission::__MoveOwnAfter()
{
    // 
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        if((pPlayer->GetInput() != &g_TotalInput) && (P_TO_UI(pPlayer->GetInput() - g_aGameInput) >= INPUT_SETS)) return;   // # skip replay

        const coreUint8 iType = g_pGame->IsMulti() ? g_CurConfig.Input.aiType[i] : g_iTotalType;
        const auto&     oSet  = g_CurConfig.Input.aSet[iType];
        const coreUint8 iMode = g_CurConfig.Input.aiControlMode[i];

        // 
        cConfigMenu::PrintFigure(&m_aaManual[i][0],  iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveUp    : SDL_CONTROLLER_BUTTON_DPAD_UP);
        cConfigMenu::PrintFigure(&m_aaManual[i][1],  iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveLeft  : SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        cConfigMenu::PrintFigure(&m_aaManual[i][2],  iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveDown  : SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        cConfigMenu::PrintFigure(&m_aaManual[i][3],  iType, (iType < INPUT_SETS_KEYBOARD) ? oSet.iMoveRight : SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        cConfigMenu::PrintFigure(&m_aaManual[i][4],  iType, oSet.aiAction[0]);
        cConfigMenu::PrintFigure(&m_aaManual[i][5],  iType, oSet.aiAction[1]);
        cConfigMenu::PrintFigure(&m_aaManual[i][6],  iType, oSet.aiAction[2]);
        cConfigMenu::PrintFigure(&m_aaManual[i][7],  iType, oSet.aiAction[3]);
        cConfigMenu::PrintFigure(&m_aaManual[i][8],  iType, oSet.aiAction[4]);
        cConfigMenu::PrintFigure(&m_aaManual[i][9],  iType, oSet.aiAction[5]);
        cConfigMenu::PrintFigure(&m_aaManual[i][10], iType, oSet.aiAction[6]);

        // 
        const coreVector2 vGame  = g_pPostProcessing->GetDirection();
        const coreVector2 vHud   = g_vHudDirection;
        const coreVector2 vFinal = MapToAxisInv(vGame, vHud);
        ASSERT(vFinal.IsNormalized())
        
        const coreUintW iShift = PackDirection(vFinal)  / 2u;

        
        const coreVector2 vFlip  = vHud  .Processed(ABS) + vHud  .yx().Processed(ABS) * ((g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f);
        const coreVector2 vFlip2 = vFinal.Processed(ABS) + vFinal.yx().Processed(ABS) * ((g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f);
        const coreVector2 vFlip3 = coreVector2((g_CurConfig.Game.iMirrorMode == 1u) ? -1.0f : 1.0f, 1.0f);

        // 
        constexpr coreVector2 avOffset[] =
        {
            coreVector2( 0.0f, 1.0f),
            coreVector2(-1.0f, 0.0f),
            coreVector2( 0.0f,-1.0f),
            coreVector2( 1.0f, 0.0f),
            coreVector2( 1.0f, 0.0f),
            coreVector2(-1.0f, 0.5f),
            coreVector2( 1.0f, 0.5f),
            coreVector2( 0.0f, 1.0f),
            coreVector2(-1.0f, 0.0f),
            coreVector2( 0.0f,-1.0f),
            coreVector2( 1.0f, 0.0f),
        };

        // 
        const coreBool abBaseMove[] =
        {
            (pPlayer->GetInput()->vMove.y * vFlip3.y > 0.0f),
            (pPlayer->GetInput()->vMove.x * vFlip3.x < 0.0f),
            (pPlayer->GetInput()->vMove.y * vFlip3.y < 0.0f),
            (pPlayer->GetInput()->vMove.x * vFlip3.x > 0.0f)
        };
        const coreBool abPress[] =
        {
            abBaseMove[(0u + iShift) % 4u],
            abBaseMove[(1u + iShift) % 4u],
            abBaseMove[(2u + iShift) % 4u],
            abBaseMove[(3u + iShift) % 4u],
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_SHOOT(0u, 0u))),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_TURN_LEFT)),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_TURN_RIGHT)),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_SHOOT_UP)),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_SHOOT_LEFT)),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_SHOOT_DOWN)),
            (HAS_BIT(pPlayer->GetInput()->iActionHold, PLAYER_ACTION_SHOOT_RIGHT))
        };

        // 
        const coreUint8 aiModeBits[] =
        {
            0xFFu,
            0xFFu,
            0xFFu,
            0xFFu,
            BIT(0u),
            BIT(0u),
            BIT(0u),
            BIT(1u),
            BIT(1u),
            BIT(1u),
            BIT(1u)
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
                m_aaManual[i][j].SetPosition (MapToAxis(avOffset[j] * 0.1f, vGame) * vFlip);
                m_aaManual[i][j].SetSize     (coreVector2(0.1f,0.1f) * (abPress[j] ? 0.8f : 1.0f) * vFlip2);
                m_aaManual[i][j].SetDirection(MapToAxisInv(coreVector2(0.0f,1.0f), vGame));
                m_aaManual[i][j].SetCenter   (MapToAxis(g_pForeground->Project2D(pPlayer->GetPosition()), vHud));
                m_aaManual[i][j].SetAlpha    (BLENDH3(MIN1(fTime)));
                m_aaManual[i][j].SetEnabled  ((fTime && HAS_BIT(aiModeBits[j], iMode)) ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
                m_aaManual[i][j].Move();
            }
        }
    });
}


// ****************************************************************
// 
coreVector3 cIntroMission::__GetEnemyColor(const coreUintW iIndex)
{
    switch(iIndex % 8u)
    {
    default: ASSERT(false)
    case 0u: return COLOR_SHIP_YELLOW;
    case 1u: return COLOR_SHIP_ORANGE;
    case 2u: return COLOR_SHIP_RED;
    case 3u: return COLOR_SHIP_MAGENTA;
    case 4u: return COLOR_SHIP_PURPLE;
    case 5u: return COLOR_SHIP_BLUE;
    case 6u: return COLOR_SHIP_CYAN;
    case 7u: return COLOR_SHIP_GREEN;
    }
}