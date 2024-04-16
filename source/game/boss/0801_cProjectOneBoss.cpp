///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 1: white flash on impact of white helper
// different phases should map to the general feeling and behavior of the same-colored bosses
// background sind weiß-schwarz-graue blöcke, und farbe ändert sich je nach phase (breitet sich wie ne welle aus)
// TODO 1: final endboss, ship jumps into z dimension, while side blocks move away, star fox (either looping or dive animation), rotation around z
// TODO 1: MAIN: fragment, easy, hard (decision), coop, 3 badges, boss health, medal goal, intro, outro, foreshadow

// cDarkBackground:
// - jede boss phase hat eine andere animation
// - welle von links nach rechts
// - welle aus mitte
// - schachmuster (auf und ab animation)
// - rotation
// - auf und ab linien, größere blöcke, diagonale
// - boss macht bomben-angriffe die eine lokale welle erzeugen
// - hügel bewegt sich mit etwas vom boss/spieler mit
// - blöcke reist es weg von mitte(?) beginnend nach außen bei transition zu eigengrau
// - TODO 1: color vignetting (per block, or in shader), maybe for all environments -> TO algorithm
// - const coreFloat Z = (((X + (Y + iOffset)) % 2u) ? -5.0f : 5.0f) * SIN(Core::System->GetTotalTime());
// - m_aBlockRaw[i].SetColor3(coreVector3(1.0f,1.0f,1.0f) * (0.5f + 0.5f * ABS(vPosition.Normalized().z)));
// - colors on blocks may need *1.5 like on ship-shader
// - return (iIndex + F_TO_UI(m_fFlyOffset / DARK_DETAIL)) % DARK_BLOCKS_Y; GetBlockShift
// - color interpolation der blocks, abhängig von ihrer höhe, entweder zwischen zwei farbtönen oder farbton+grau


// ****************************************************************
// counter identifier
#define SELECTED_HELPER (0u)


// ****************************************************************
// vector identifier
#define HELPER_DATA (0u)


// ****************************************************************
// constructor
cProjectOneBoss::cProjectOneBoss()noexcept
: m_iHelperState (0u)
, m_vLevelColor  (coreVector3(0.0f,0.0f,0.0f))
, m_fAnimation   (0.0f)
{
    // load object resources
    this->DefineModelHigh("ship_projectone.md3");
    this->DefineModelLow ("ship_projectone.md3");
    this->DefineTexture  (0u, "ship_player.png");

    // set object properties
    this->SetSize(coreVector3(1.2f,1.2f,1.2f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_GREY);

    // 
    constexpr coreVector3 avColor[] =
    {
        COLOR_ENERGY_YELLOW,
        COLOR_ENERGY_ORANGE,
        COLOR_ENERGY_RED,
        COLOR_ENERGY_MAGENTA,
        COLOR_ENERGY_PURPLE,
        COLOR_ENERGY_BLUE,
        COLOR_ENERGY_CYAN,
        COLOR_ENERGY_GREEN
    };
    for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
    {
        m_aShield[i].DefineModelHigh("object_sphere.md3");
        m_aShield[i].DefineModelLow ("object_sphere.md3");
        m_aShield[i].DefineTexture  (0u, "effect_energy.png");
        m_aShield[i].DefineProgram  ("effect_energy_blink_spheric_program");   // TODO 1: vielleicht lieber effect_energy_blink_flat_spheric_program ?
        m_aShield[i].SetTexSize     (coreVector2(1.0f,1.0f) * 4.0f);
        m_aShield[i].Configure      (1000, avColor[i]);
        m_aShield[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_BOTTOM | ENEMY_STATUS_IMMORTAL);
    }

    // 
    m_HelperPath.Reserve(3u);
    m_HelperPath.AddNode(coreVector2(0.0f,0.0f), coreVector2( 0.0f, 1.0f));
    m_HelperPath.AddNode(coreVector2(0.5f,0.5f), coreVector2( 1.0f,-1.0f).Normalized());
    m_HelperPath.AddNode(coreVector2(0.0f,0.0f), coreVector2(-1.0f, 0.0f), 2.5f);
    m_HelperPath.Refine();
}


// ****************************************************************
// 
void cProjectOneBoss::__ResurrectOwn()
{
    // 
    m_vLevelColor = coreVector3(1.0f,1.0f,1.0f);
}


// ****************************************************************
// 
void cProjectOneBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    //this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        {
            cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);

            if(PHASE_BEGINNING2) pHelper->Resurrect();

            const coreFloat fTime = CLAMP((m_fPhaseTime - I_TO_F(i) * 0.2f) * 0.7f, 0.0f, 1.0f);

            coreFloat fSide;
            switch(i % 4u)
            {
            default: ASSERT(false)
            case 0u: fSide = -0.35f; break;
            case 1u: fSide =  0.75f; break;
            case 2u: fSide = -0.75f; break;
            case 3u: fSide =  0.35f; break;
            }

            const coreVector2 vPos = coreVector2(fSide * 0.9f * LERPH3(1.0f, 0.8f, fTime), LERPH3(-1.2f, 1.3f, fTime)) * FOREGROUND_AREA;

            pHelper->SetPosition(coreVector3(vPos, 0.0f));

            if((i == PROJECTONE_SHIELDS - 1u) && (fTime >= 1.0f))
                PHASE_CHANGE_INC
        }

        //PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        //{
            /*
            cEnemySquad* pSquad = g_pGame->GetCurMission()->GetEnemySquad(0u);

            pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
            {
                pEnemy->Kill(true);
            });

            g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vDir = (pPlayer->GetPosition().xy() - this->GetPosition().xy()).Normalized();

                pPlayer->ApplyForce(vDir * 120.0f);
            });

            g_pSpecialEffects->MacroExplosionDarkBig(coreVector3(0.0f, FOREGROUND_AREA.y, 0.0f));
            */
        //    PHASE_CHANGE_INC
        //});
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,1.2f), coreVector2(0.0f,0.8f), fTime);

            //if(PHASE_TIME_POINT(0.85f))
            //    this->_StartBoss();

            if(PHASE_FINISHED)// && !g_pGame->GetInterface()->IsBannerActive())
                //PHASE_CHANGE_INC
                PHASE_CHANGE_TO(42u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp     (coreVector2(0.0f,0.8f), coreVector2(-0.8f,0.8f), fTime);
            this->DefaultOrientateLerp(0.0f*PI,                4.0f*PI,                 fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(coreVector2(-0.8f,0.8f), coreVector2(0.8f,0.8f), fTime);

            PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition ().xy();
                const coreVector2 vDir = this->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.6f, this, vPos, vDir)->ChangeSize(1.4f);
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
                PHASE_RESET(1u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp     (coreVector2(0.8f,0.8f), coreVector2(0.0f,0.8f), fTime);
            this->DefaultOrientateLerp(0.0f*PI,                4.0f*PI,                fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
        {
            PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(Core::Rand->Float(-90.0f, 90.0f) + 180.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.6f, this, vPos,  vDir)->MakeWhite()->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.6f, this, vPos, -vDir)->MakeWhite()->ChangeSize(1.3f);
            });

            const coreVector2 vPlayerPos = this->NearestPlayerDual(0u)->GetPosition().xy();
            //this->SetPosition(coreVector3(vPlayerPos.x, 0.8f * FOREGROUND_AREA.y, 0.0f));

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([&](cFlipBullet* OUTPUT pBullet)
            {
                if(pBullet->GetFlyTime() >= 2.0f) return;

                const coreVector2 vAim = (vPlayerPos - pBullet->GetPosition().xy()).Normalized();
                const coreVector2 vDir = SmoothAim(pBullet->GetFlyDir(), vAim, 2.0f);

                pBullet->SetSpeed (pBullet->GetSpeed() + 2.0f * TIME);   
                pBullet->SetFlyDir(vDir);
            });

            g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                if(pPlayer->IsFeeling() || pPlayer->IsIgnoring()) pPlayer->Kill(false);
            });
        }

        if(HAS_FLAG(g_pGame->GetStatus(), GAME_STATUS_OUTRO))
            PHASE_CHANGE_TO(30u)
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.9f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(m_vLastPosition.x, -1.2f), fTime);
            this->DefaultOrientateLerp(0.0f*PI,         2.0f*PI,                               fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[SELECTED_HELPER] = -1;
            m_avVector [HELPER_DATA].z   = 1.0f*PI;

            for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
                m_aShield[i].RemoveStatus(ENEMY_STATUS_GHOST);

            this->AddStatus(ENEMY_STATUS_GHOST);
        }

        this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.0f), LERPB(0.0f, 1.0f, MIN(m_fPhaseTime, 1.0f)));

        const coreFloat fSpawn = LERPB (0.0f, 15.0f, MIN(m_fPhaseTime - 0.5f, 1.0f));
        const coreFloat fFade  = LERPH3(0.0f,  1.0f, MIN(m_fPhaseTime - 0.3f, 1.0f));

        const coreUintW iRotaCount = PROJECTONE_SHIELDS - coreMath::PopCount(m_iHelperState);
        coreUintW       iRotaNum   = 0u;

        m_avVector[HELPER_DATA].z += 0.3f * TIME;

        for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        {
            if(HAS_BIT(m_iHelperState, i)) continue;

            cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
            cCustomEnemy* pShield = &m_aShield[i];

            if(PHASE_MAINTIME_POINT(0.5f)) pHelper->Resurrect();
            if(PHASE_MAINTIME_POINT(0.3f)) pShield->Resurrect();

            const coreVector2 vDir = coreVector2::Direction((2.0f*PI) * I_TO_F(iRotaNum) * RCP(I_TO_F(iRotaCount)) + m_avVector[HELPER_DATA].z);
            const coreVector2 vPos = this->GetPosition().xy() + vDir * fSpawn;

            pHelper->SetPosition(coreVector3(vPos, 0.0f));

            pShield->SetSize (fFade * coreVector3(5.0f,5.0f,5.0f));
            pShield->SetAlpha(fFade);

            if(!pShield->HasStatus(ENEMY_STATUS_DEAD) && (pShield->GetLostHealth() >= 50))
            {
                m_aiCounter[SELECTED_HELPER] = i;
                m_avVector [HELPER_DATA].xy(vPos);
            }

            iRotaNum += 1u;
        }

        if(m_aiCounter[SELECTED_HELPER] >= 0)
        {
            PHASE_CHANGE_INC

            for(coreUintW j = 0u; j < PROJECTONE_SHIELDS; ++j)
                m_aShield[j].AddStatus(ENEMY_STATUS_GHOST);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        const coreFloat fFade = LERPB(1.0f, 0.0f, MIN(m_fPhaseTime * 3.0f, 1.0f));
        const coreFloat fLerp = MIN(m_fPhaseTime * 1.2f * RCP(m_HelperPath.GetTotalDistance()), 1.0f);

        const coreUintW iRotaCount = GAME_HELPERS - 1u - coreMath::PopCount(m_iHelperState);
        coreUintW       iRotaNum   = 0u;

        for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        {
            if(HAS_BIT(m_iHelperState, i)) continue;

            cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
            cCustomEnemy* pShield = &m_aShield[i];

            if(i == coreUintW(m_aiCounter[SELECTED_HELPER]))
            {
                const coreVector2 vDir = coreVector2::Direction((2.0f*PI) * I_TO_F(iRotaNum) * RCP(I_TO_F(iRotaCount)) + m_avVector[HELPER_DATA].z);
                const coreVector2 vPos = MapToAxis(m_HelperPath.CalcPositionLerp(fLerp), vDir) * FOREGROUND_AREA + m_avVector[HELPER_DATA].xy() * (1.0f - fLerp);

                pHelper->SetPosition(coreVector3(vPos, 0.0f));

                pShield->SetSize ((1.5f - 0.5f * fFade) * coreVector3(5.0f,5.0f,5.0f));
                pShield->SetAlpha(fFade);
            }
            else
            {
                const coreVector2 vDir = pHelper->GetPosition().xy().Normalized();
                const coreVector2 vPos = pHelper->GetPosition().xy() + vDir * (10.0f * (3.0f + 6.0f * m_fPhaseTime) * TIME);

                pHelper->SetPosition(coreVector3(vPos, 0.0f));

                pShield->SetSize (fFade * coreVector3(5.0f,5.0f,5.0f));
                pShield->SetAlpha(fFade);
            }

            iRotaNum += 1u;
        }

        if(fLerp >= 1.0f)
        {
            const coreUintW iIndex = m_aiCounter[SELECTED_HELPER];

            PHASE_CHANGE_TO(50u + iIndex * 10u)

            ADD_BIT(m_iHelperState, iIndex)

            switch(iIndex)
            {
            default: ASSERT(false)
            case 1u: m_vLevelColor = COLOR_MENU_YELLOW;  break;
            case 2u: m_vLevelColor = COLOR_MENU_ORANGE;  break;
            case 3u: m_vLevelColor = COLOR_MENU_RED;     break;
            case 4u: m_vLevelColor = COLOR_MENU_MAGENTA; break;
            case 5u: m_vLevelColor = COLOR_MENU_PURPLE;  break;
            case 6u: m_vLevelColor = COLOR_MENU_BLUE;    break;
            case 7u: m_vLevelColor = COLOR_MENU_CYAN;    break;
            case 8u: m_vLevelColor = COLOR_MENU_GREEN;   break;
            }

            for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
            {
                cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
                cCustomEnemy* pShield = &m_aShield[i];

                pHelper->Kill(false);
                pShield->Kill(false);
            }

            this->RemoveStatus(ENEMY_STATUS_GHOST);

            g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), m_aShield[iIndex].GetColor3());
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 42u)
    {
        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_WHITE);

        if(PHASE_BEGINNING2)
        {
            pHelper->Resurrect();
        }

        constexpr coreFloat fDelay = 2.5f / 0.6f;

        const coreFloat fTime = POW(m_fPhaseTime, 1.2f);
        const coreFloat fOff  = COS(fTime * (0.6f*PI)) * (LERPH3(1.0f, 0.7f, MIN(fTime / fDelay, 1.0f)) + LERPH3(0.6f, 0.0f, MIN(fTime / 1.2f, 1.0f)));
        const coreFloat fOff2 = SIN(fTime * (0.6f*PI)) * (LERPH3(0.8f, 0.0f, MIN(fTime / fDelay, 1.0f)));

        const coreVector2 vDir = coreVector2::Direction(fTime * (-0.1f*PI) + (0.25f*PI));
        const coreVector2 vPos = (vDir * fOff + vDir.Rotated90() * fOff2) * FOREGROUND_AREA;

        pHelper->SetPosition(coreVector3(vPos, 0.0f));

        this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.0f), LERPB(0.0f, 1.0f, MIN(m_fPhaseTime, 1.0f)));

        if(fTime >= fDelay)
        {
            PHASE_CHANGE_TO(130u)

            m_vLevelColor = coreVector3(1.0f,1.0f,1.0f);

            pHelper->Kill(false);

            g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_WHITE);
        }
    }

    // ################################################################
    // yellow phase
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // orange phase
    else if(m_iPhase == 60u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // red phase
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // magenta phase
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // purple phase
    else if(m_iPhase == 90u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // blue phase
    else if(m_iPhase == 100u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // cyan phase
    else if(m_iPhase == 110u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // green phase
    else if(m_iPhase == 120u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // white phase
    else if(m_iPhase == 130u)
    {

    }

    // ################################################################
    // ################################################################


    // 
    for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
    {
        const cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);

        m_aShield[i].SetPosition (pHelper->GetPosition());
        m_aShield[i].SetTexOffset(coreVector2(0.0f, 0.1f * m_fAnimation + I_TO_F(i) / I_TO_F(PROJECTONE_SHIELDS)));
    }
}