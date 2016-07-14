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
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
    {
        m_aCompanion[i].DefineModelHigh("ship_boss_vaus_high.md3");
        m_aCompanion[i].DefineModelLow ("ship_boss_vaus_low.md3");
        m_aCompanion[i].SetSize        (coreVector3(2.0f,2.0f,2.0f));

        m_aCompanion[i].Configure(1, COLOR_SHIP_YELLOW);

        m_aCompanion[i].SetPosition(coreVector3(0.0f,0.0f,0.0f));
    }
}


// ****************************************************************
// 
void cVausBoss::__ResurrectOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetMission());

    // 
    pMission->EnableBall  (0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalize());
    pMission->EnablePaddle(0u, this);
}


// ****************************************************************
// 
void cVausBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetMission());

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        pMission->DisablePaddle(i, bAnimated);
}


// ****************************************************************
// 
void cVausBoss::__MoveOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetMission());
    coreObject3D*   pBall    = pMission->GetBall(0u);
    cEnemySquad*    pSquad   = pMission->GetEnemySquad(0u);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        if(m_aiCounter[SCOUT_RESURRECTIONS] >= (VAUS_SCOUTS_TOTAL / 2))
        {
            PHASE_CONTROL_TIMER(0u, 0.25f, LERP_BREAK)
            {
                // 
                const coreFloat fCur = this->GetPosition().x / FOREGROUND_AREA.x;
                this->DefaultMoveLerp(coreVector2(fCur, -2.0f), coreVector2(fCur, -0.95f), fTime);

                // 
                if(PHASE_FINISHED)
                    ++m_iPhase;
            });
        }

        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
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
            // 
            pMission->MakeSticky();

            // 
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
            // 
            if(m_aiCounter[IGNORE_BALL] == 0)
            {
                m_aiCounter[IGNORE_BALL] = 1;

                m_avVector[0].xy(this->GetPosition().xy() / FOREGROUND_AREA);
                m_avVector[0].z = pBall->GetDirection().x;
            }

            // 
            if((m_avVector[0].z * pBall->GetDirection().x) <= 0.0f)
            {
                m_avVector[0].xy(2.0f * (this->GetPosition().xy() / FOREGROUND_AREA) - m_avVector[0].xy());
                m_avVector[0].z = pBall->GetDirection().x;
            }

            // 
            const coreFloat fDrop   = (FRAMERATE_TIME * fSpeed) * RCP(LERPB(0.0f, 1.0f, (FRAMERATE_TIME * fSpeed))) * 0.5f / fSpeed;
            const coreFloat fNewPos = m_avVector[0].x + pBall->GetDirection().x * VIRIDO_BALL_SPEED * fDrop;

            // 
            this->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(fNewPos, -0.95f), fTime);

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                // 
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

            // 
            m_aCompanion[0].Resurrect(coreVector2(-1.8f * FOREGROUND_AREA.x, 0.0f), coreVector2( 1.0f,0.0f));
            m_aCompanion[1].Resurrect(coreVector2( 1.8f * FOREGROUND_AREA.x, 0.0f), coreVector2(-1.0f,0.0f));

            // 
            pMission->EnablePaddle(1u, &m_aCompanion[0]);
            pMission->EnablePaddle(2u, &m_aCompanion[1]);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 4u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.5f, LERP_SMOOTH)
        {
            // 
            this->DefaultMoveLerp  (m_avVector[0].xy(), coreVector2(0.0f,0.95f), fTime);
            this->DefaultRotateLerp(0.0f*PI,            3.0f*PI,                 fTime);

            // 
            m_aCompanion[0].DefaultMoveLerp(coreVector2(-1.8f,-0.6f), coreVector2(-0.95f,-0.6f), fTime);
            m_aCompanion[1].DefaultMoveLerp(coreVector2( 1.8f,-0.6f), coreVector2( 0.95f,-0.6f), fTime);

            // 
            if(PHASE_TIME_POINT(0.35f))
            {
                g_pGame->GetMission()->SetCurBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            // 
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
            // 
            m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f,-0.6f), coreVector2(-0.95f, pBall->GetPosition().y / FOREGROUND_AREA.y), fTime);
            m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f,-0.6f), coreVector2( 0.95f, pBall->GetPosition().y / FOREGROUND_AREA.y), fTime);

            // 
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

            // 
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

            // 
            m_aiCounter[IGNORE_BALL] = 0;

            // 
            coreVector2 vBallDir;
            switch(m_aiCounter[SUB_PHASE] % 3)
            {
            default: ASSERT(false)
            case 2: vBallDir = coreVector2(-1.0f, -3.675f).Normalize(); break;
            case 1: vBallDir = coreVector2(-1.0f, -1.212f).Normalize(); break;
            case 0: vBallDir = coreVector2(-2.45f,-1.0f)  .Normalize(); break;
            }

            // 
            if(m_aiCounter[SUB_PHASE] % 2) vBallDir.InvertX();

            // 
            pMission->UnmakeSticky(vBallDir);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        // 
        coreFloat fSpeed;
        switch(m_aiCounter[SUB_PHASE] % 3)
        {
        default: ASSERT(false)
        case 2: fSpeed = 1.0f/7.0f; break;
        case 1: fSpeed = 1.0f/6.0f; 
            {
                const coreFloat y = (pBall->GetPosition().x <= 0.0f) ? 1.211f : 1.212f;
                const coreVector2 vDir = pBall->GetDirection().xy().Sign() * coreVector2(1.0f,y).Normalize();
                pBall->SetDirection(coreVector3(vDir, 0.0f));
            }

            break;
        case 0: fSpeed = 1.0f/6.0f; break;
        }

        // 
        PHASE_CONTROL_PAUSE(0u, fSpeed)
        {
            pMission->MakeSticky();
        });

        // 
        if(pMission->GetStickyState())
        {
            ++m_iPhase;


            m_iPhase = 11u;   
            if(++m_aiCounter[SUB_PHASE] >= 6) m_aiCounter[SUB_PHASE] = 0;   
            this->SetPosition(coreVector3(coreVector2(0.0f,0.95f) * FOREGROUND_AREA, 0.0f)); 
            m_aiCounter[IGNORE_BALL] = 1; 


            //// 
            //m_aiCounter[IGNORE_BALL] = 1;
            //
            //// 
            //this->SetPosition(coreVector3(coreVector2(0.0f,0.95f) * FOREGROUND_AREA, 0.0f));
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 13u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.0f, LERP_SMOOTH)
        {
            // 
            const coreBool bSkip = this->__ExecuteCompanionAttack(0u, fTime);

            // 
            if(PHASE_FINISHED || bSkip)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 14u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.0f, LERP_SMOOTH)
        {
            // 
            const coreBool bSkip = this->__ExecuteCompanionAttack(1u, fTime);

            // 
            if(PHASE_FINISHED || bSkip)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 15u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.0f, LERP_SMOOTH)
        {
            // 
            const coreBool bSkip = this->__ExecuteCompanionAttack(2u, fTime);

            // 
            if(PHASE_FINISHED || bSkip)
            {
                m_iPhase = 10u;
                PHASE_RESET(1u)

                // 
                if(++m_aiCounter[SUB_PHASE] >= 6) m_aiCounter[SUB_PHASE] = 0;
            }
        });
    }



    PHASE_CONTROL_TICKER(1u, 0u, 2.0f)         
    {
        //static coreUintW i = 0u;
        //if(iTick == 0u) i = 1 - m_aiCounter[SUB_PHASE] % 2u;
        //i = 1u - i;
        //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
        //{
        //    const coreVector2 vPos = m_aCompanion[i].GetPosition ().xy();
        //    const coreVector2 vDir = m_aCompanion[i].GetDirection().xy();
        //
        //    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, vPos, vDir)->MakeRed();
        //}

        //const coreVector2 vPos = pBall->GetPosition ().xy();
        //const coreVector2 vDir = -pBall->GetDirection().xy();//(g_pGame->FindPlayer(vPos)->GetPosition().xy() - vPos).Normalize();
        //
        //g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.4f, this, vPos, vDir)->MakeGreen();
    });





    // 
    PHASE_CONTROL_TICKER(2u, 0u, 1.2f)
    {
        pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            const coreVector2 vPos = pEnemy->GetPosition().xy();
            const coreVector2 vDir = pEnemy->AimAtPlayer().Normalize();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, pEnemy, vPos, vDir)->MakeOrange();
        });
    });

    // 
    pSquad->ForEachEnemyAll([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
    {
        const coreUintW x = i % VAUS_SCOUTS_X;
        const coreUintW y = i / VAUS_SCOUTS_X;

        const coreBool    bFront   = (y + (CONTAINS_BIT(m_iScoutOrder, x) ? 1u : 0u)) & 0x01u;
        const coreVector2 vGridPos = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + (bFront ? 0.2f : 0.0f));

        if(CONTAINS_FLAG(pEnemy->GetStatus(), ENEMY_STATUS_DEAD))
        {
            if(!bFront) TOGGLE_BIT(m_iScoutOrder, x)
            STATIC_ASSERT(VAUS_SCOUTS_X <= sizeof(m_iScoutOrder)*8u)

            if(m_aiCounter[SCOUT_RESURRECTIONS] < VAUS_SCOUTS_TOTAL)
            {
                ++m_aiCounter[SCOUT_RESURRECTIONS];
                pEnemy->Resurrect((vGridPos + coreVector2(0.0f,3.0f)) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
            }
        }

        pEnemy->DefaultMoveSmooth(vGridPos, 3.0f, 30.0f);
    });

    // 
    if(!m_aiCounter[IGNORE_BALL])
    {
        this->SetPosition(coreVector3(pBall->GetPosition().x, this->GetPosition().yz()));
        m_aCompanion[0].SetPosition(coreVector3(m_aCompanion[0].GetPosition().x, pBall->GetPosition().y, m_aCompanion[0].GetPosition().z));
        m_aCompanion[1].SetPosition(coreVector3(m_aCompanion[1].GetPosition().x, pBall->GetPosition().y, m_aCompanion[1].GetPosition().z));
    }

    // 
    if(g_pGame->GetMission()->GetCurBoss() == this)
    {
        if(pMission->GetBounceState())
        {
            for(coreUintW j = 0u; j < VIRIDO_PADDLES; ++j)
            {
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

    cViridoMission* pMission    = s_cast<cViridoMission*>(g_pGame->GetMission());
    coreObject3D*   pBall       = pMission->GetBall(0u);
    const coreFloat fBallHeight = pBall->GetPosition().y / FOREGROUND_AREA.y;

    // 
    coreFloat fFrom1, fFrom2, fTo1, fTo2;
    switch(m_aiCounter[SUB_PHASE] % 3)
    {
    default:// ASSERT(false)
    case 0: fFrom1 = fBallHeight; fFrom2 = -1.0f; fTo1 = -1.0f; fTo2 = fBallHeight; break;
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
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fBallHeight), coreVector2(-0.95f, fFrom1), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fBallHeight), coreVector2( 0.95f, fFrom2), fTime);
        break;

    case 1u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fFrom1), coreVector2(-0.95f, fTo1), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fFrom2), coreVector2( 0.95f, fTo2), fTime);
        break;

    case 2u:
        m_aCompanion[0].DefaultMoveLerp(coreVector2(-0.95f, fTo1), coreVector2(-0.95f, fBallHeight), fTime);
        m_aCompanion[1].DefaultMoveLerp(coreVector2( 0.95f, fTo2), coreVector2( 0.95f, fBallHeight), fTime);
        break;
    }

    PHASE_CONTROL_TICKER(1u, 0u, 10.0f)
    {
        static coreUintW i = 0u;
        if(iTick == 0u) i = 1 - m_aiCounter[SUB_PHASE] % 2u;
        i = 1u - i;
        //for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
        {
            const coreVector2 vPos = m_aCompanion[i].GetPosition ().xy();
            const coreVector2 vDir = m_aCompanion[i].GetDirection().xy();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos, vDir)->MakeOrange();
        }
    });

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