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
#define CONNECTED_MOUTH (0u)


// ****************************************************************
// vector identifier
#define OLD_MOUTH_ANGLE (0u)
#define STOMP_POSITION  (1u)


// ****************************************************************
// constructor
cAmemasuBoss::cAmemasuBoss()noexcept
: m_fMouthAngle (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize       (coreVector3(0.0f,0.0f,0.0f));
    this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(3000, COLOR_SHIP_CYAN);

    // 
    m_Top.DefineModelHigh("ship_boss_amemasu_top_high.md3");
    m_Top.DefineModelLow ("ship_boss_amemasu_top_low.md3");
    m_Top.SetSize        (coreVector3(3.0f,3.0f,3.0f));
    m_Top.Configure      (1, COLOR_SHIP_CYAN);
    m_Top.SetParent      (this);

    // 
    m_Bottom.DefineModelHigh("ship_boss_amemasu_bottom_high.md3");
    m_Bottom.DefineModelLow ("ship_boss_amemasu_bottom_low.md3");
    m_Bottom.SetSize        (coreVector3(3.0f,3.0f,3.0f));
    m_Bottom.Configure      (1, COLOR_SHIP_CYAN);
    m_Bottom.SetParent      (this);

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

    // ENERGIE ATEM
}


// ****************************************************************
// 
void cAmemasuBoss::__ResurrectOwn()
{
    // 
    s_cast<cNevoMission*>(g_pGame->GetCurMission())->EnableContainer(coreVector2(0.0f,0.0f));

    // 
    m_aiCounter[CONNECTED_MOUTH] = 1;
}


// ****************************************************************
// 
void cAmemasuBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cAmemasuBoss::__MoveOwn()
{
    cNevoMission* pMission   = s_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(coreVector2(pContainer->GetPosition().x / FOREGROUND_AREA.x, 2.0f), pContainer->GetPosition().xy() / FOREGROUND_AREA, fTime);

            if(PHASE_TIME_AFTER(0.5f)) m_fMouthAngle = SIN(PI * (fTime * 2.0f - 1.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pMission->DisableContainer(false);
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.75f), fTime);

            if(PHASE_TIME_AFTER(0.5f)) m_fMouthAngle = SIN(0.1f*PI * (fTime * 2.0f - 1.0f));

            if(PHASE_TIME_POINT(0.85f))
                this->_StartBoss();

            if(PHASE_FINISHED && !g_pGame->GetInterface()->IsBannerActive())
                PHASE_CHANGE_TO(10u)   
        });
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
                PHASE_CHANGE_INC

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
                PHASE_CHANGE_TO(20u)     

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

                m_fMouthAngle = 0.0f;

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
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_BREAK)
        {
            this->DefaultMoveLerp     (coreVector2(0.0f,0.0f), coreVector2(0.0f,-0.75f), fTime);
            this->DefaultRotateLerp   (1.0f*PI,                2.0f*PI,                  fTime);
            this->DefaultOrientateLerp(1.5f*PI,                2.5f*PI,                  fTime);

            m_fMouthAngle = SIN(0.1f*PI * fTime);

            const coreVector2 vDir = coreVector2::Direction(fTime * (9.0f*PI));

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vPos = this->GetPosition().xy().InvertedY();

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(40u)

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
        PHASE_CONTROL_TIMER(0u, 2.0f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) m_avVector[OLD_MOUTH_ANGLE].x = m_fMouthAngle;

            m_fMouthAngle = LERP(m_avVector[OLD_MOUTH_ANGLE].x, 0.0f, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[CONNECTED_MOUTH] = 0;
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 31u || m_iPhase == 33u || m_iPhase == 35u || m_iPhase == 37u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) m_avVector[STOMP_POSITION].y = this->NearestPlayer()->GetPosition().y / FOREGROUND_AREA.y;

            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.85f, SIN(PI * fTime)), LERPS(m_vLastPosition.y, m_avVector[STOMP_POSITION].y, fTime)) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 32u || m_iPhase == 34u || m_iPhase == 36u || m_iPhase == 38u)
    {
        PHASE_CONTROL_PAUSE(0u, 4.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 39u)
    {
        PHASE_CONTROL_TIMER(0u, 0.9f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING) m_aiCounter[CONNECTED_MOUTH] = 1;

            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(0.0f,0.75f), fTime);
            this->DefaultOrientateLerp(1.5f*PI,         3.5f*PI,                 fTime);

            m_fMouthAngle = LERP(0.0f, SIN(0.1f*PI), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 40u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING) m_avVector[OLD_MOUTH_ANGLE].x = m_fMouthAngle;

            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(0.0f,0.75f), fTime);
            this->DefaultOrientateLerp(0.5f*PI,         2.5f*PI,                 fTime);

            m_fMouthAngle = LERP(m_avVector[OLD_MOUTH_ANGLE].x, 0.0f, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[CONNECTED_MOUTH] = 0;
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.85f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 42u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            const coreVector2 vPos = coreVector2(m_vLastPosition.x, LERP(m_vLastPosition.y, -m_vLastPosition.y, fTime)) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 43u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.0f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(50u)   

                m_aiCounter[CONNECTED_MOUTH] = 1;
            }
        });
    }

    // ################################################################
    // ################################################################

    if(m_aiCounter[CONNECTED_MOUTH])
    {
        // 
        const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.2f*PI * m_fMouthAngle, coreVector3::Cross(this->GetDirection(), this->GetOrientation()).Normalized()).m123();
        const coreVector3 vDir1 = this->GetDirection() * mRota;
        const coreVector3 vDir2 = this->GetDirection() * mRota.Transposed();
        const coreVector3 vPos1 = this->GetPosition () + (vDir1 - this->GetDirection()) * (m_Top   .GetSize().x * 3.5f);
        const coreVector3 vPos2 = this->GetPosition () + (vDir2 - this->GetDirection()) * (m_Bottom.GetSize().x * 3.5f);

        // 
        m_Top.SetPosition   (vPos1);
        m_Top.SetDirection  (vDir1);
        m_Top.SetOrientation(this->GetOrientation());

        // 
        m_Bottom.SetPosition   (vPos2);
        m_Bottom.SetDirection  (vDir2);
        m_Bottom.SetOrientation(this->GetOrientation());
    }
}


// spieler werden verschluckt, schiff-objekte werden versteckt
// boss wird nach unten aus dem bildschirm interpoliert
// dabei wird background geändert (transition typ ausprobieren)
// spieler fliegen von unten rein in neuen background (andere dinge von oben)
// umgekehrt beim ausspucken
// sobald boss nicht mehr sichtbar ist spieler einfliegen um übergang kurz zu halten
// schießt dann torpedos von unten nach oben