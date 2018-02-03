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
#define SCOUT_RESURRECTIONS (0u)
#define IGNORE_BALL         (1u)
#define SUB_PHASE           (2u)
#define LASER_SHOT          (3u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cVausBoss::cVausBoss()noexcept
: m_iScoutOrder (0u)
{
    // load models
    this->DefineModelHigh("ship_boss_vaus_high.md3");
    this->DefineModelLow ("ship_boss_vaus_low.md3");

    // set object properties
    this->SetSize(coreVector3(2.5f,2.5f,2.5f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_YELLOW);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
    {
        m_aRay[i].DefineModel  ("object_tube.md3");
        m_aRay[i].DefineTexture(0u, "effect_energy.png");
        m_aRay[i].DefineProgram("effect_energy_invert_program");
        m_aRay[i].SetColor3    (i ? (COLOR_ENERGY_RED * 0.9f) : (COLOR_ENERGY_BLUE * 0.8f));
        m_aRay[i].SetTexSize   (TORUS_RAY_TEXSIZE);          
        m_aRay[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
    {
        m_aCompanion[i].DefineModelHigh("object_boss_vaus_companion_high.md3");
        m_aCompanion[i].DefineModelLow ("object_boss_vaus_companion_low.md3");
        m_aCompanion[i].SetSize        (coreVector3(2.2f,2.2f,2.2f));
        m_aCompanion[i].Configure      (1, COLOR_SHIP_YELLOW);
    }

    // TODO: companions must rotate sometimes (after bounce ?)
}


// ****************************************************************
// 
void cVausBoss::__ResurrectOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableBall  (0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized());
    pMission->EnablePaddle(0u, this);
}


// ****************************************************************
// 
void cVausBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    pMission->DisableBall(0u, bAnimated);

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        pMission->DisablePaddle(i, bAnimated);

    // 
    pMission->DeactivateBoss();
}


// ****************************************************************
// 
void cVausBoss::__RenderOwnAttack()
{
    if(m_aRay[1].IsEnabled(CORE_OBJECT_ENABLE_RENDER) ||
       m_aRay[2].IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        DEPTH_PUSH

        // 
        m_aRay[1].Render();
        m_aRay[2].Render();
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aRay[1]);
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aRay[2]);
    }

    if(m_aRay[0].IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        DEPTH_PUSH

        // 
        m_aRay[0].Render();
        g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aRay[0]);
    }
}


// ****************************************************************
// 
void cVausBoss::__MoveOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetCurMission());
    coreObject3D*   pBall    = pMission->GetBall(0u);
    cEnemySquad*    pSquad   = pMission->GetEnemySquad(0u);

    constexpr coreFloat fLimit1 = 0.8f;
    constexpr coreFloat fLimit2 = 0.1f;

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        if(m_aiCounter[SCOUT_RESURRECTIONS] >= (VAUS_SCOUTS_TOTAL / 2))
        {
            PHASE_CONTROL_TIMER(0u, 0.25f, LERP_BREAK)
            {
                const coreFloat fCur = this->GetPosition().x / FOREGROUND_AREA.x;
                this->DefaultMoveLerp(coreVector2(fCur, -2.0f), coreVector2(fCur, -0.95f), fTime);

                if(PHASE_FINISHED)
                    ++m_iPhase;
            });
        }

        g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->SetPosition(coreVector3(pPlayer->GetPosition().x, MAX(pPlayer->GetPosition().y, this->GetPosition().y + 9.5f), pPlayer->GetPosition().z));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        if(pSquad->IsFinished())
        {
            pMission->MakeSticky();

            if(pMission->GetStickyState())
                ++m_iPhase;
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        constexpr coreFloat fSpeed = 0.85f;
        PHASE_CONTROL_TIMER(0u, fSpeed, LERP_BREAK)
        {
            if(m_aiCounter[IGNORE_BALL] == 0)
            {
                m_aiCounter[IGNORE_BALL] = 1;

                m_avVector[0].xy(this->GetPosition().xy() / FOREGROUND_AREA);
                m_avVector[0].z = pBall->GetDirection().x;
            }

            if((m_avVector[0].z * pBall->GetDirection().x) <= 0.0f)
            {
                m_avVector[0].xy(2.0f * (this->GetPosition().xy() / FOREGROUND_AREA) - m_avVector[0].xy());
                m_avVector[0].z = pBall->GetDirection().x;
            }

            const coreFloat fDrop   = (FRAMERATE_TIME * fSpeed) * RCP(LERPB(0.0f, 1.0f, (FRAMERATE_TIME * fSpeed))) * 0.5f / fSpeed;
            const coreFloat fNewPos = m_avVector[0].x + pBall->GetDirection().x * VIRIDO_BALL_SPEED * fDrop;

            this->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(fNewPos, -0.95f), fTime);

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                m_avVector[0].xy(this->GetPosition().xy() / FOREGROUND_AREA);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_PAUSE(0u, 4.0f)
        {
            ++m_iPhase;

            m_aCompanion[0].Resurrect(coreVector2(-1.8f * FOREGROUND_AREA.x, 0.0f), coreVector2( 1.0f,0.0f));
            m_aCompanion[1].Resurrect(coreVector2( 1.8f * FOREGROUND_AREA.x, 0.0f), coreVector2(-1.0f,0.0f));

            pMission->EnablePaddle(1u, &m_aCompanion[0]);
            pMission->EnablePaddle(2u, &m_aCompanion[1]);
        });
    }

    // spin on ball-hit    

    // ################################################################
    // 
    else if(m_iPhase == 4u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp  (m_avVector[0].xy(), coreVector2(0.0f,0.95f), fTime);
            this->DefaultRotateLerp(0.0f*PI,            3.0f*PI,                 fTime);

            m_aCompanion[0].DefaultMoveLerp(coreVector2(-1.8f,-0.6f), coreVector2(-0.95f,-0.6f), fTime);
            m_aCompanion[1].DefaultMoveLerp(coreVector2( 1.8f,-0.6f), coreVector2( 0.95f,-0.6f), fTime);

            if(PHASE_TIME_POINT(0.35f))
            {
                g_pGame->GetCurMission()->ActivateBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 5u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.0f, LERP_SMOOTH)
        {
            const coreFloat fBallPosY = pBall->GetPosition().y / FOREGROUND_AREA.y;

            m_aCompanion[0].DefaultMoveLerp  (coreVector2(-0.95f,-0.6f), coreVector2(-0.95f, fBallPosY), fTime);
            m_aCompanion[1].DefaultMoveLerp  (coreVector2( 0.95f,-0.6f), coreVector2( 0.95f, fBallPosY), fTime);
            m_aCompanion[0].DefaultRotateLerp(-0.5f*PI,                   1.5f*PI,                       fTime);
            m_aCompanion[1].DefaultRotateLerp( 0.5f*PI,                  -1.5f*PI,                       fTime);

            if(PHASE_FINISHED)
                m_iPhase = 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            // TODO: charge 

            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            ++m_iPhase;

            m_aiCounter[IGNORE_BALL] = 0;

            coreVector2 vBallDir;
            switch(m_aiCounter[SUB_PHASE] % 3)
            {
            default: ASSERT(false)
            case 2: vBallDir = coreVector2(-1.0f, -3.543f).Normalized(); break;
            case 1: vBallDir = coreVector2(-1.0f, -1.18f) .Normalized(); break;
            case 0: vBallDir = coreVector2(-2.54f,-1.0f)  .Normalized(); break;
            }

            if(m_aiCounter[SUB_PHASE] % 2) vBallDir = vBallDir.InvertedX();

            pMission->UnmakeSticky(vBallDir);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        coreFloat fSpeed;
        switch(m_aiCounter[SUB_PHASE] % 3)
        {
        default: ASSERT(false)
        case 2: fSpeed = 1.0f/7.0f; break;
        case 1: fSpeed = 1.0f/6.0f; break;
        case 0: fSpeed = 1.0f/6.0f; break;
        }

        PHASE_CONTROL_PAUSE(0u, fSpeed)
        {
            pMission->MakeSticky();
        });

        if(pMission->GetStickyState())
        {
            ++m_iPhase;

            if((this->GetCurHealthPct() < fLimit1) && (m_iPhase < 30u))
            {
                m_iPhase = 40u;
            }
            else
            {

                m_iPhase = ((m_aiCounter[SUB_PHASE] % 3) == 2) ? 20u : 11u;
                if(++m_aiCounter[SUB_PHASE] >= 6) m_aiCounter[SUB_PHASE] = 0;   
            }

            this->SetPosition(coreVector3(coreVector2(0.0f,0.95f) * FOREGROUND_AREA, 0.0f)); 
            m_aiCounter[IGNORE_BALL] = 1; 

            //m_aiCounter[IGNORE_BALL] = 1;
            //
            //this->SetPosition(coreVector3(coreVector2(0.0f,0.95f) * FOREGROUND_AREA, 0.0f));
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        const coreVector2 vNewPlayerPos = g_pGame->FindPlayer(this->GetPosition().xy())->GetPosition().xy() / FOREGROUND_AREA;

        m_avVector[0].xy(this          ->GetPosition().xy() / FOREGROUND_AREA);
        m_avVector[1].xy(m_aCompanion[0].GetPosition().xy() / FOREGROUND_AREA);
        m_avVector[2].xy(m_aCompanion[1].GetPosition().xy() / FOREGROUND_AREA);
        m_avVector[3].xy(vNewPlayerPos - m_avVector[4].xy());

        m_avVector[3].y = ((m_aiCounter[LASER_SHOT] & 0x01) ? -0.2f : 0.2f) * SIGN(m_avVector[3].y); // MIN(ABS(m_avVector[3].y), 0.3f) * SIGN(m_avVector[3].y);
        m_avVector[3].xy(m_avVector[3].xy() + vNewPlayerPos);

        m_avVector[4].xy(vNewPlayerPos);

        

        m_iPhase = (m_aiCounter[LASER_SHOT] == VAUS_SHOTS) ? 23u : 22u;
        ++m_aiCounter[LASER_SHOT];

        m_aRay[0].SetEnabled((m_iPhase == 23u) ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
        m_aRay[1].SetEnabled((m_iPhase == 23u) ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);
        m_aRay[2].SetEnabled((m_iPhase == 23u) ? CORE_OBJECT_ENABLE_NOTHING : CORE_OBJECT_ENABLE_ALL);

        if(m_aiCounter[LASER_SHOT] != 1)
        {
            constexpr coreUintW iDensity = 35u;
            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
            {
                const coreVector3& vPos = m_aRay[i].GetPosition();
                const coreVector3& vDir = m_aRay[i].GetDirection();
                const coreFloat    fLen = m_aRay[i].GetSize().y / I_TO_F(iDensity-1u) * 2.0f;
                for(coreUintW j = iDensity; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (fLen*I_TO_F(j-(iDensity/2u))), 5.0f, 1u, i ? COLOR_ENERGY_RED : COLOR_ENERGY_BLUE);
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 22)
    {
        const coreFloat fSpeed = 1.0f / (((m_aiCounter[LASER_SHOT] == 1) ? 2.0f : 1.0f) * (1.5f - 0.8f*I_TO_F(m_aiCounter[LASER_SHOT]) / I_TO_F(VAUS_SHOTS)));
        PHASE_CONTROL_TIMER(0u, fSpeed, LERP_BREAK)
        {
            //const coreUintW iSide = m_aiCounter[LASER_SHOT] & 0x01;
            coreUintW iSide = (ABS(m_avVector[1].y - m_avVector[3].y) <
                               ABS(m_avVector[2].y - m_avVector[3].y)) ? 0u : 1u;
            if(m_aiCounter[LASER_SHOT] & 0x01) iSide = 1u - iSide;

            //if(m_aiCounter[LASER_SHOT])
            //{
            //    cEnemy* pEnemy = &m_aCompanion[m_aiCounter[LASER_SHOT]-1u];
            //    pEnemy->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(m_avVector[0].x, m_avVector[3].y), fTime);
            m_aCompanion[0].DefaultMoveLerp(m_avVector[1].xy(), coreVector2(m_avVector[1].x, m_avVector[3 + iSide].y), fTime);
            m_aCompanion[1].DefaultMoveLerp(m_avVector[2].xy(), coreVector2(m_avVector[2].x, m_avVector[4 - iSide].y), fTime);
            //}
            //else
            //{
                this->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(m_avVector[4].x, m_avVector[0].y), fTime);
            //}

            m_aRay[0].SetAlpha(1.0f - fTime);
            m_aRay[1].SetAlpha(1.0f - fTime);
            m_aRay[2].SetAlpha(1.0f - fTime);

            if(PHASE_FINISHED)
            {
                m_iPhase = 21u;
                PHASE_RESET(0u)

                m_aRay[0].SetAlpha(1.0f);
                m_aRay[1].SetAlpha(1.0f);
                m_aRay[2].SetAlpha(1.0f);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 23)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.0f, LERP_BREAK)
        {
            const coreFloat fBallPosY = pBall->GetPosition().y / FOREGROUND_AREA.y;

            this->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(0.0f, m_avVector[0].y), fTime);

            m_aCompanion[0].DefaultMoveLerp(m_avVector[1].xy(), coreVector2(m_avVector[1].x, fBallPosY), fTime);
            m_aCompanion[1].DefaultMoveLerp(m_avVector[2].xy(), coreVector2(m_avVector[2].x, fBallPosY), fTime);

            if(PHASE_FINISHED)
            {
                m_iPhase = 11u;

                m_aiCounter[LASER_SHOT] = 0;

                m_avVector[4] = coreVector3(0.0f,0.0f,0.0f);
                // TODO: phase switch   
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        if(pMission->GetStickyState())
        {
            ++m_iPhase;
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        if(pMission->GetStickyState())
        {
            ++m_iPhase;

            pMission->UnmakeSticky(coreVector2(1.0f,-1.0f).Normalized());
        }
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
        constexpr coreFloat fDiff  = 0.2f;
        constexpr coreFloat fFirst = 0.8f;
        constexpr coreFloat fMax   = 0.6f;
        PHASE_CONTROL_TIMER(0u, 1.0f/2.0f, LERP_LINEAR)
        {
            const coreFloat fBallPosY = pBall->GetPosition().y / FOREGROUND_AREA.y;

            const coreFloat fTime1 = LERPB(0.0f, 1.4f, MIN(fTime / fFirst, 1.0f) * fMax);
            const coreFloat fTime2 = LERPB(0.0f, 1.4f, MAX((fTime - fDiff) / (1.0f - fDiff), 0.0f) * fMax);

            m_aCompanion[0].DefaultMoveLerp  (coreVector2(-0.95f, fBallPosY), coreVector2(-0.5f,0.2f), fTime1);
            m_aCompanion[1].DefaultMoveLerp  (coreVector2( 0.95f, fBallPosY), coreVector2( 0.5f,0.2f), fTime2);
            m_aCompanion[0].DefaultRotateLerp(-0.5f*PI,                       -1.2f*PI,                fTime1);
            m_aCompanion[1].DefaultRotateLerp( 0.5f*PI,                        5.1f*PI,                fTime2);

            if(PHASE_TIME_POINT(0.0f))
            {
                g_pSpecialEffects->MacroExplosionPhysicalSmall(m_aCompanion[0].GetPosition());
            }
            if(PHASE_TIME_POINT(fDiff))
            {
                g_pSpecialEffects->MacroExplosionPhysicalSmall(m_aCompanion[1].GetPosition());
            }

            if(PHASE_TIME_POINT(fFirst))
            {
                m_aCompanion[0].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                g_pSpecialEffects->MacroExplosionPhysicalBig(m_aCompanion[0].GetPosition());

                pMission->EnablePaddle(1u, g_pGame->GetPlayer(0u)); 
            }

            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                m_aCompanion[1].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
                g_pSpecialEffects->MacroExplosionPhysicalBig(m_aCompanion[1].GetPosition());

                if(g_pGame->GetCoop()) pMission->EnablePaddle (2u, g_pGame->GetPlayer(1u)); 
                                  else pMission->DisablePaddle(2u, true);

                pMission->MakeReal(1u);
                pMission->MakeReal(2u);

                m_aCompanion[0].Kill(true);
                m_aCompanion[1].Kill(true);

                m_aiCounter[IGNORE_BALL] = 0;
                pMission->UnmakeSticky(coreVector2(1.0f,-1.0f).Normalized());
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        if(this->GetCurHealthPct() <= 0.6f)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 1.0f/3.0f)
            {
                const coreUintW iIndex = iTick & 0x03u;
                if(iIndex & 0x01u) pSquad->GetEnemy(iIndex)->Resurrect(coreVector2( 2.0f,0.0f) * FOREGROUND_AREA, coreVector2(-1.0f,0.0f));
                              else pSquad->GetEnemy(iIndex)->Resurrect(coreVector2(-2.0f,0.2f) * FOREGROUND_AREA, coreVector2( 1.0f,0.0f));
            });

            pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
            {
                const coreVector2 vTarget = (i & 0x01u) ? coreVector2(-2.0f,0.0f) : coreVector2(2.0f,0.2f);
                if(pEnemy->DefaultMoveSmooth(vTarget, 30.0f, 10.0f))
                    pEnemy->Kill(false);
            });
        }
    }

    // ################################################################
    // ################################################################



    //// ################################################################
    //// 
    //if(m_iPhase == (21 + VAUS_SHOTS))
    //{
    //    PHASE_CONTROL_TIMER(3u, 1.0f/2.0f, LERP_SMOOTH)
    //    {
    //        // 
    //        const coreBool bSkip = this->__ExecuteCompanionAttack(3u, fTime);
    //
    //        // 
    //        //if(PHASE_FINISHED || bSkip)
    //        //    ++m_iPhase;
    //    });
    //}
    //
    //// ################################################################
    //// 
    //else if(m_iPhase == (22 + VAUS_SHOTS))
    //{
    //    PHASE_CONTROL_TIMER(3u, 1.0f/2.0f, LERP_SMOOTH)
    //    {
    //        // 
    //        const coreBool bSkip = this->__ExecuteCompanionAttack(0u, fTime);
    //
    //        // 
    //        //if(PHASE_FINISHED || bSkip)
    //        //    ++m_iPhase;
    //    });
    //}
    //
    //// ################################################################
    //// 
    //else if(m_iPhase == (23 + VAUS_SHOTS))
    //{
    //    PHASE_CONTROL_TIMER(3u, 1.0f/2.0f, LERP_SMOOTH)
    //    {
    //        // 
    //        const coreBool bSkip = this->__ExecuteCompanionAttack(1u, fTime);
    //
    //        // 
    //        //if(PHASE_FINISHED || bSkip)
    //        //    ++m_iPhase;
    //    });
    //}
    //
    //// ################################################################
    //// 
    //else if(m_iPhase == (24 + VAUS_SHOTS))
    //{
    //    PHASE_CONTROL_TIMER(3u, 1.0f/2.0f, LERP_SMOOTH)
    //    {
    //        // 
    //        const coreBool bSkip = this->__ExecuteCompanionAttack(2u, fTime);
    //
    //        // 
    //        //if(PHASE_FINISHED || bSkip)
    //        //{
    //        //    m_iPhase = 10u;
    //        //    PHASE_RESET(1u)
    //        //
    //        //    // 
    //        //    //if(++m_aiCounter[SUB_PHASE] >= 6) m_aiCounter[SUB_PHASE] = 0;
    //        //}
    //    });
    //}
       




    // boss geht weiter runter mit lebenspunkte bis zur grenze bei der finale phase beginnt


    //PHASE_CONTROL_TICKER(1u, 0u, 2.0f)         
    //{
    //    //if(iTick == 0u) i = 1 - m_aiCounter[SUB_PHASE] % 2u;
    //    //i = 1u - i;
    //    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
    //    {
    //        const coreVector2 vPos = m_aCompanion[i].GetPosition ().xy();
    //        const coreVector2 vDir = m_aCompanion[i].GetDirection().xy();
    //
    //        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, vPos, vDir)->MakeRed();
    //    }
    //
    //    //const coreVector2 vPos = pBall->GetPosition ().xy();
    //    //const coreVector2 vDir = -pBall->GetDirection().xy();//(g_pGame->FindPlayer(vPos)->GetPosition().xy() - vPos).Normalize();
    //    //
    //    //g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, vPos, vDir)->MakeGreen();
    //});





    if(m_iPhase < 10u)
    {
        // 
        PHASE_CONTROL_TICKER(2u, 0u, 1.2f)
        {
            pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->MakeOrange();
            });
        });

        // 
        pSquad->ForEachEnemyAll([this](cEnemy* OUTPUT pEnemy, const coreUintW i) // TODO don't calculate if all dead (m_iPhase < 10u) 
        {
            const coreUintW x = i % VAUS_SCOUTS_X;
            const coreUintW y = i / VAUS_SCOUTS_X;

            const coreBool    bFront   = (y + (CONTAINS_BIT(m_iScoutOrder, x) ? 1u : 0u)) & 0x01u;
            const coreVector2 vGridPos = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + (bFront ? 0.2f : 0.0f));

            if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))
            {
                if(!bFront) TOGGLE_BIT(m_iScoutOrder, x)

                if(m_aiCounter[SCOUT_RESURRECTIONS] < VAUS_SCOUTS_TOTAL)
                {
                    ++m_aiCounter[SCOUT_RESURRECTIONS];
                    pEnemy->Resurrect((vGridPos + coreVector2(0.0f,3.0f)) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
                }
            }

            pEnemy->DefaultMoveSmooth(vGridPos, 30.0f, 10.0f);

            STATIC_ASSERT(VAUS_SCOUTS_X <= sizeof(m_iScoutOrder)*8u)
        });
    }

    // 
    if(!m_aiCounter[IGNORE_BALL])
    {
        this->SetPosition(coreVector3(pBall->GetPosition().x, this->GetPosition().yz()));
        m_aCompanion[0].SetPosition(coreVector3(m_aCompanion[0].GetPosition().x, pBall->GetPosition().y, m_aCompanion[0].GetPosition().z));
        m_aCompanion[1].SetPosition(coreVector3(m_aCompanion[1].GetPosition().x, pBall->GetPosition().y, m_aCompanion[1].GetPosition().z));
    }

    // 
    if(g_pGame->GetCurMission()->GetCurBoss() == this)
    {
        if(pMission->GetBounceState())
        {
            for(coreUintW j = 0u; j < VIRIDO_PADDLES; ++j)
            {
                if(CONTAINS_BIT(pMission->GetRealState(), j)) continue;
                coreObject3D* pPaddle = pMission->GetPaddle(j);

                const coreVector2 vPos   = pPaddle->GetPosition ().xy();
                const coreFloat   fAngle = pPaddle->GetDirection().xy().Angle();

                const auto nMakeColor = j ? &cOrbBullet::MakeRed : &cOrbBullet::MakeBlue;


                const coreVector2 vRealDir = pPaddle->GetDirection().xy(); 
                const coreFloat fOff = 30.0f;


                for(coreUintW i = 0u; i < (j ? 5u : 7u); ++i)
                {
                    const coreVector2 vDir = coreVector2::Direction(fAngle + 0.1f * I_TO_F(i - (j ? 2u : 3u)));

                    (g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*(3.0f+fOff) - vRealDir*fOff, vRealDir)->*nMakeColor)();
                    (g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*(0.5f+fOff) - vRealDir*fOff, vRealDir)->*nMakeColor)();
                }
            }
        }
    }


    if(pMission->GetRealState())
    {
        // 
        PHASE_CONTROL_TICKER(3u, 0u, 12.0f)
        {
            if((iTick % 12u) < 4u)
            {
                const coreVector2 vPos = pBall->GetPosition().xy();
                const coreVector2 vDir = coreVector2(1.0f,0.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos,  vDir)->MakeOrange();
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos, -vDir)->MakeOrange();
            }
        });

        this->DefaultMoveSmooth(coreVector2(0.0f, 0.95f - 0.95f * (fLimit1 - this->GetCurHealthPct()) * RCP(fLimit1)), 100.0f, 10.0f);
    }


    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aRay); ++i)
    {
        coreObject3D& oCurRay = m_aRay[i];

        if(oCurRay.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            const coreFloat fSize = 50.0f;

            const coreObject3D* pPaddle = pMission->GetPaddle(i);

            oCurRay.SetPosition (coreVector3(pPaddle->GetPosition().xy() + pPaddle->GetDirection().xy() * (fSize + 3.0f), 0.0f));
            oCurRay.SetSize     (coreVector3(1.2f, fSize, 1.2f));
            oCurRay.SetDirection(coreVector3(pPaddle->GetDirection().xy(), 0.0f));
            //m_Ray.SetAlpha();
            oCurRay.SetTexOffset(coreVector2(0.4f,0.3f) * -coreFloat(Core::System->GetTotalTime()));   // TODO: fract   
            oCurRay.Move();
        }
    }

    // 
    //g_pEnvironment->SetTargetSide(this->GetPosition().xy() * MIN(m_fLifeTime*0.1f, 1.0f) * 0.5f);
    //g_pEnvironment->SetTargetDirection((this->GetPosition().xy() * coreVector2(MIN(m_fLifeTime*0.1f, 1.0f) * 0.5f, 1.0f)).Normalize());

    //g_pPostProcessing->SetValue(0.8f);
}


// ****************************************************************
// 
coreBool cVausBoss::__ExecuteCompanionAttack(const coreUintW iType, const coreFloat fTime)
{
    //if((iType == 1u) && ((m_aiCounter[SUB_PHASE] % 3) == 1))
    //    return true;

    cViridoMission* pMission  = s_cast<cViridoMission*>(g_pGame->GetCurMission());
    coreObject3D*   pBall     = pMission->GetBall(0u);
    const coreFloat fBallPosY = pBall->GetPosition().y / FOREGROUND_AREA.y;

    // 
    coreFloat fFrom1, fFrom2, fTo1, fTo2;
    switch(m_aiCounter[SUB_PHASE] % 3)
    {
    default:// ASSERT(false)
    case 0: fFrom1 = fBallPosY; fFrom2 = -1.0f; fTo1 = -1.0f; fTo2 = fBallPosY; break;
   // case 1: fFrom1 = 0.9f; fFrom2 = -0.9f; fTo1 = 0.9f; fTo2 = -0.9f; break;
   // case 2: fFrom1 = 0.9f; fFrom2 = -0.9f; fTo1 = 0.9f; fTo2 = -0.9f; break;
    }

    // 
    if(m_aiCounter[SUB_PHASE] % 2u)
    {
        std::swap(fFrom1, fFrom2);
        std::swap(fTo1,   fTo2);
    }

    // 
    switch(iType)
    {
    default: ASSERT(false)
    case 0u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fBallPosY), coreVector2(-0.95f, fFrom1), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fBallPosY), coreVector2( 0.95f, fFrom2), fTime);
        break;

    case 1u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fFrom1), coreVector2(-0.95f, fTo1), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fFrom2), coreVector2( 0.95f, fTo2), fTime);
        break;

    case 2u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fTo1), coreVector2(-0.95f, fBallPosY), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fTo2), coreVector2( 0.95f, fBallPosY), fTime);
        break;

    case 3u:
        break;
    }

   // PHASE_CONTROL_TICKER(1u, 0u, 10.0f)
   // {
   //     static coreUintW i = 0u;  
   //     if(iTick == 0u) i = 1 - m_aiCounter[SUB_PHASE] % 2u;
   //     i = 1u - i;
   //     //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
   //     {
   //         const coreVector2 vPos = m_aCompanion[i].GetPosition ().xy();
   //         const coreVector2 vDir = m_aCompanion[i].GetDirection().xy();
   //
   //         g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos, vDir)->MakeOrange();
   //     }
   // });

    return false;
}


/*
laser von allen 3 bosse nausgehend
bei phasenwechsel seitenbosse sterben, dropen schilde zum einsammeln (markiert)
boss bringt seinen laser zur mitte
schieﬂt ball mit aller kraft nach unten, muss man ausweichen, ball grˆﬂer schneller
spieler kann schild nehmen und auch ball so schieﬂen (aber nicht aktiv)
laser in mitte (oder seite) blockiert unbeschadet spieler oder paddle


*/