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
// counter identifier


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cAmemasuBoss::cAmemasuBoss()noexcept
{
    // load models
    this->DefineModelHigh("object_sphere.md3");
    this->DefineModelLow ("object_sphere.md3");

    // set object properties
    this->SetSize(coreVector3(10.0f,10.0f,10.0f));

    // configure the boss
    this->Configure(3000, COLOR_SHIP_CYAN);

    // 
    m_ChangePath.AddNode(coreVector2(0.0f, 0.0f),  coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f, 0.01f), coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f,-3.0f),  coreVector2(0.0f,-1.0f));
    m_ChangePath.Refine();

    // 
    m_apStomachTex[0] = Core::Manager::Resource->Get<coreTexture>("environment_mars_diff.png");
    m_apStomachTex[1] = Core::Manager::Resource->Get<coreTexture>("environment_rock_diff.png");
    m_apStomachTex[2] = Core::Manager::Resource->Get<coreTexture>("environment_dust_diff.png");
    m_apStomachTex[3] = Core::Manager::Resource->Get<coreTexture>("environment_earth_diff.png");
}


// ****************************************************************
// 
void cAmemasuBoss::__MoveOwn()
{
    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.0f), fTime);

            if(PHASE_TIME_POINT(0.85f))
            {
                g_pGame->GetMission()->SetCurBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 1u)
    {
        m_iPhase = 10u;
    }

    // ################################################################
    // 
    if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_BREAK_REV)
        {
            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_ROLL);
                    pPlayer->ChangeType(0);
                });
            }

            const coreVector2 vDir = coreVector2::Direction(fTime * (4.0f*PI));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vDiff = this->GetPosition().xy() - pPlayer->GetPosition().xy();
                const coreVector2 vMove = vDiff.Normalized() * (Core::System->GetTime() * fTime * 100.0f * SmoothTowards(vDiff.Length(), 5.0f));
                const coreVector2 vPos  = pPlayer->GetPosition().xy() + vMove;

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->SetPosition(coreVector3(0.0f, FOREGROUND_AREA.x * -2.0f, 0.0f));
                });

                g_pEnvironment->ChangeBackground(cStomachBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,-1.0f));
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 11u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_SMOOTH)
        {
            const coreFloat fHeightBoss = m_ChangePath.CalcPosition(CLAMP(fTime*1.4f, 0.0f, 1.0f) * m_ChangePath.GetTotalDistance()).y;
            this->SetPosition(coreVector3(0.0f, (0.0f + fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreFloat fSide = g_pGame->GetCoop() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS-1u))) : 0.0f;

                const coreFloat fHeightPlayer = m_ChangePath.CalcPosition(CLAMP((fTime*3.0f - 1.0f) / 2.0f, 0.0f, 1.0f) * m_ChangePath.GetTotalDistance()).y;
                pPlayer->SetPosition(coreVector3(fSide, (-3.75f - fHeightPlayer) * FOREGROUND_AREA.y, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                m_iPhase = 20u;     

                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);
                    pPlayer->ChangeType(TYPE_PLAYER);
                });
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_ROLL);
                    pPlayer->ChangeType(0);
                });

                g_pEnvironment->ChangeBackground(cSeaBackground::ID, ENVIRONMENT_MIX_WIPE, 1.0f, coreVector2(0.0f,1.0f));
            }

            const coreFloat fHeightBoss = m_ChangePath.CalcPosition(CLAMP((LERPS(0.0f, 1.0f, fTime)*3.0f - 1.0f) / 2.0f, 0.0f, 1.0f) * m_ChangePath.GetTotalDistance()).y;
            this->SetPosition(coreVector3(0.0f, (-3.0f - fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            const coreVector2 vDir = coreVector2::Direction(fTime * (10.0f*PI));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vMove = coreVector2(-pPlayer->GetPosition().x, 1.0f) * (Core::System->GetTime() * 90.0f);
                const coreVector2 vPos  = pPlayer->GetPosition().xy() + vMove;

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                ++m_iPhase;
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,0.0f), coreVector2(0.0f,-0.75f), fTime);

            const coreVector2 vDir = coreVector2::Direction(fTime * (9.0f*PI));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vPos = this->GetPosition().xy().InvertedY();

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                m_iPhase = 30u;

                g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_ALL);
                    pPlayer->ChangeType(TYPE_PLAYER);
                });
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 30u)
    {
    }

    // ################################################################
    // ################################################################
}


