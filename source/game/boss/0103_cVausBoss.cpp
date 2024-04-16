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
// counter identifier
#define IGNORE_BALL    (0u)
#define SUB_PHASE      (1u)
#define SUB_PHASE_INIT (2u)
#define CURRENT_SIDE   (3u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cVausBoss::cVausBoss()noexcept
//: m_iScoutOrder (0u)
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
        m_aCompanion[i].DefineModelHigh("object_boss_vaus_companion_high.md3");
        m_aCompanion[i].DefineModelLow ("object_boss_vaus_companion_low.md3");
        m_aCompanion[i].SetSize        (coreVector3(2.2f,2.2f,2.2f));
        m_aCompanion[i].Configure      (1, COLOR_SHIP_YELLOW);
    }
}


// ****************************************************************
// 
void cVausBoss::__ResurrectOwn()
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableBall  (0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalized());
    pMission->EnablePaddle(0u, this);

    // 
    m_aiCounter[SUB_PHASE_INIT] = -1;
}


// ****************************************************************
// 
void cVausBoss::__KillOwn(const coreBool bAnimated)
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());

    // 
    pMission->DisableBall(0u, bAnimated);

    // 
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i)
        pMission->DisablePaddle(i, bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cVausBoss::__MoveOwn()
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());
    coreObject3D*   pBall    = pMission->GetBall(0u);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        //if(m_aiCounter[SCOUT_RESURRECTIONS] >= (VAUS_SCOUTS_TOTAL / 2))
        {
            PHASE_CONTROL_TIMER(0u, 0.25f, LERP_BREAK)
            {
                const coreFloat fCur = this->GetPosition().x / FOREGROUND_AREA.x;
                this->DefaultMoveLerp(coreVector2(fCur, 2.0f), coreVector2(fCur, 0.95f), fTime);

                if(PHASE_FINISHED)
                    PHASE_CHANGE_INC
            });
        }

        g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            const coreVector3 vPos = pPlayer->GetPosition();
            pPlayer->SetPosition(coreVector3(vPos.x, MIN(vPos.y, this->GetPosition().y - 9.5f), vPos.z));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        if(pMission->GetStickyState())
        {
            PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
            {
                if(PHASE_BEGINNING)
                {
                    this->StorePosition();
                    this->StoreRotation();
                    m_aiCounter[IGNORE_BALL] = 1;
                }

                coreVector2 vNewPos;
                //switch(m_aiCounter[CURRENT_SIDE] & 0x03u)
                //{
                //default: ASSERT(false)
                //case 0u: vNewPos = coreVector2( m_vLastPosition.y, 0.0f); break;
                //case 1u: vNewPos = coreVector2(0.0f, -m_vLastPosition.x); break;
                //case 2u: vNewPos = coreVector2( m_vLastPosition.y, 0.0f); break;
                //case 3u: vNewPos = coreVector2(0.0f, -m_vLastPosition.x); break;
                //}
                vNewPos = coreVector2(0.0f, -m_vLastPosition.y);

                const coreFloat fNewAngle = m_fLastDirAngle + 3.0f*PI;// + 1.5f*PI;

                this->DefaultMoveLerp  (m_vLastPosition, vNewPos,   fTime);
                this->DefaultRotateLerp(m_fLastDirAngle, fNewAngle, fTime);

                if(PHASE_FINISHED)
                {
                    PHASE_RESET(0u)

                    m_aiCounter[IGNORE_BALL]   = 0;
                    m_aiCounter[SUB_PHASE]    += 1;
                    m_aiCounter[CURRENT_SIDE] += 1;

                    const coreVector2 vNewBallDir = (this->GetDirection().xy() + this->GetDirection().xy().Rotated90() * 0.5f).Normalized();

                    pMission->UnmakeSticky(vNewBallDir);

                    g_pSpecialEffects->CreateBlowColor(pBall->GetPosition(), coreVector3(vNewBallDir, 0.0f), SPECIAL_BLOW_SMALL, COLOR_ENERGY_GREEN);
                }
            });

            
        }

        if(this->GetCurHealthPct() <= 0.1f)//<= 0.8f)
        {
            pMission->MakeSticky();

            if(pMission->GetStickyState())
                PHASE_CHANGE_INC
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

            const coreFloat fDrop   = (fSpeed / FRAMERATE_MIN) * RCP(LERPB(0.0f, 1.0f, (fSpeed / FRAMERATE_MIN))) * 0.5f / fSpeed; // TODO: wtf, get rid of time value if necessary  
            const coreFloat fNewPos = m_avVector[0].x + pBall->GetDirection().x * VIRIDO_BALL_SPEED * fDrop;

            this->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(fNewPos, -0.95f), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_PAUSE(0u, 4.0f)
        {
            PHASE_CHANGE_INC

            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
            {
                const coreFloat fSide = i ? 1.0f : -1.0f;

                m_aCompanion[i].Resurrect();
                m_aCompanion[i].SetPosition (coreVector3( fSide * 1.8f * FOREGROUND_AREA.x, 0.0f, 0.0f));
                m_aCompanion[i].SetDirection(coreVector3(-fSide,                            0.0f, 0.0f));
            }

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
            this->DefaultMoveLerp  (m_avVector[0].xy(), coreVector2(0.0f,0.95f), fTime);
            this->DefaultRotateLerp(0.0f*PI,            3.0f*PI,                 fTime);

            m_aCompanion[0].DefaultMoveLerp(coreVector2(-1.8f,-0.6f), coreVector2(-0.95f,-0.6f), fTime);
            m_aCompanion[1].DefaultMoveLerp(coreVector2( 1.8f,-0.6f), coreVector2( 0.95f,-0.6f), fTime);

            if(PHASE_TIME_POINT(0.35f))
                this->_StartBoss();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
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
                PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // ################################################################

    // 
    if(m_iPhase <= 1u) this->__UpdateBreakout();

    // 
    if(!m_aiCounter[IGNORE_BALL])
    {
        coreVector2 vNewPos = this->GetPosition().xy();
        if(IsHorizontal(this->GetDirection().xy())) vNewPos.y = pBall->GetPosition().y;
                                               else vNewPos.x = pBall->GetPosition().x;

        this->SetPosition(coreVector3(vNewPos, this->GetPosition().z));

        //m_aCompanion[0].SetPosition(coreVector3(m_aCompanion[0].GetPosition().x, pBall->GetPosition().y, m_aCompanion[0].GetPosition().z));
        //m_aCompanion[1].SetPosition(coreVector3(m_aCompanion[1].GetPosition().x, pBall->GetPosition().y, m_aCompanion[1].GetPosition().z));
    }



    // 
    if(CONTAINS_BIT(pMission->GetBounceState(), 0u)
       )//||
       //CONTAINS_BIT(pMission->GetBounceState(), 7u))
    {
        coreObject3D* pPaddle = pMission->GetPaddle(0);

        const coreVector2 vPos   = pPaddle->GetPosition ().xy();
        const coreFloat   fAngle = pPaddle->GetDirection().xy().Angle();



        const coreVector2 vRealDir = pPaddle->GetDirection().xy();
        constexpr coreFloat fOff = 30.0f;


        for(coreUintW i = 0u; i < 7u; ++i)
        {
            const coreVector2 vDir = coreVector2::Direction(fAngle + 0.1f * I_TO_F(i - 3u));

            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*(3.0f+fOff) - vRealDir*fOff, vRealDir)->ChangeSize(1.1f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*(0.5f+fOff) - vRealDir*fOff, vRealDir)->ChangeSize(1.1f);
        }
    }


    if(pMission->GetRealState())
    {
        // 
        PHASE_CONTROL_TICKER(3u, 0u, 12.0f, LERP_LINEAR)
        {
            if((iTick % 12u) < 4u)
            {
                const coreVector2 vPos = pBall->GetPosition().xy();
                const coreVector2 vDir = coreVector2(1.0f,0.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos,  vDir)->ChangeSize(1.2f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos, -vDir)->ChangeSize(1.2f);
            }
        });

        //this->DefaultMoveSmooth(coreVector2(0.0f, 0.95f - 0.95f * (0.8f - this->GetCurHealthPct()) / 0.8f), 100.0f, 10.0f);
    }

}


#define BALL_FLYPAST(e,f,v)                            \
    ((e)->GetPosition(). v < (f)->GetPosition(). v) ^  \
    ((e)->GetPosition(). v < vTestOldPos.        v) || \
    ((e)->GetOldPos  (). v < (f)->GetPosition(). v) ^  \
    ((e)->GetOldPos  (). v < vTestOldPos.        v) || \
    ((f)->GetPosition(). v < (e)->GetPosition(). v) ^  \
    ((f)->GetPosition(). v < (e)->GetOldPos  (). v) || \
    (vTestOldPos.        v < (e)->GetPosition(). v) ^  \
    (vTestOldPos.        v < (e)->GetOldPos  (). v)

static coreVector2 vTestOldPos = coreVector2(0.0f,0.0f);
// ****************************************************************
// 
void cVausBoss::__UpdateBreakout()
{
#define VAUS_SCOUTS_X (8u)
#define VAUS_SCOUTS_Y (2u)
#if 0
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());
    coreObject3D*   pBall    = pMission->GetBall(0u);
    cEnemySquad*    pSquad   = pMission->GetEnemySquad(0u);

    // 
    const coreBool bInit = (m_aiCounter[SUB_PHASE_INIT] != m_aiCounter[SUB_PHASE]);
    m_aiCounter[SUB_PHASE_INIT] = m_aiCounter[SUB_PHASE];

    if(m_aiCounter[SUB_PHASE] == 2)
    {
        // 
        const auto nEnemyFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            if(i >= VAUS_SCOUTS_X * VAUS_SCOUTS_Y) return;

            const coreUintW x = i % VAUS_SCOUTS_X;
            const coreUintW y = i / VAUS_SCOUTS_X;

            const coreVector2 vPos = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + 0.2f * I_TO_F(y));

            if(bInit)
            {
                pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                pEnemy->Resurrect((vPos + coreVector2(0.0f, 1.0f + 0.12f * I_TO_F(x))) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));

                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));
            }

            pEnemy->DefaultMoveSmooth(vPos, 40.0f, 10.0f);
        };

        // 
        if(bInit) pSquad->ForEachEnemyAll(nEnemyFunc);
             else pSquad->ForEachEnemy   (nEnemyFunc);

        // 
        PHASE_CONTROL_TICKER(2u, 0u, 1.2f, LERP_LINEAR)
        {
            pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();
                const coreVector2 vDir = pEnemy->AimAtPlayer().Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.4f, pEnemy, vPos, vDir)->ChangeSize(1.2f);
            });
        });
    }
    else if(m_aiCounter[SUB_PHASE] == 1)
    {
        // 
        const auto nEnemyFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            const coreUintW   iRow = i / 8u;
            const coreVector2 vDir = coreVector2((iRow & 0x01u) ? -1.0f : 1.0f, 0.0f)/*.Rotated90()*/;

            if(bInit)
            {
                //const coreUintW iRow = i / 6u;
                ////const coreUintW iCol = i % 6u;

                coreVector2 vPos = coreVector2(-1.2f + 2.4f * (0.5f/6.0f) * I_TO_F(i) - (0.6f * 8.0f), 0.67f - 0.2f * I_TO_F(2u - iRow));
                //const coreVector2 vDir = coreVector2((iRow & 0x01u) ? -1.0f : 1.0f, 0.0f);
                vPos = vPos.InvertedY();


                switch(iRow)
                {
                default: ASSERT(false)
                case 0: pEnemy->SetBaseColor(COLOR_SHIP_GREEN);  break;
                case 1: pEnemy->SetBaseColor(COLOR_SHIP_YELLOW); break;
                case 2: pEnemy->SetBaseColor(COLOR_SHIP_RED);    break;
                }

                pEnemy->Resurrect((((iRow & 0x01u) ? vPos.InvertedX() : vPos)/*.Rotated90()*//* - vDir * 2.4f*/) * FOREGROUND_AREA, vDir);
                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));
            }

            pEnemy->DefaultMoveForward(vDir/*pEnemy->GetDirection().xy()*/, 30.0f);

            if(pEnemy->GetPosition().x * vDir/*pEnemy->GetDirection()*/.x > FOREGROUND_AREA.x * 1.2f)
                pEnemy->SetPosition(pEnemy->GetPosition() - coreVector3(vDir/*pEnemy->GetDirection().xy()*/ * (FOREGROUND_AREA.x * 2.4f), 0.0f));
            if(pEnemy->GetPosition().y * vDir/*pEnemy->GetDirection()*/.y > FOREGROUND_AREA.y * 1.2f)
                pEnemy->SetPosition(pEnemy->GetPosition() - coreVector3(vDir/*pEnemy->GetDirection().xy()*/ * (FOREGROUND_AREA.y * 2.4f), 0.0f));
        };

        // 
        if(bInit) pSquad->ForEachEnemyAll(nEnemyFunc);
             else pSquad->ForEachEnemy   (nEnemyFunc);

        // 
        if(!pSquad->IsFinished())
        {
            const coreFloat fHeight = -0.6f + 1.6f * pSquad->GetNumEnemiesAlivePct();
            PHASE_CONTROL_TICKER(2u, 0u, 5.0f, LERP_LINEAR)
            {
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.0f, this, coreVector2(-1.1f, fHeight)        * FOREGROUND_AREA, coreVector2(1.0f,0.0f))->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.0f, this, coreVector2(-1.1f, fHeight - 0.1f) * FOREGROUND_AREA, coreVector2(1.0f,0.0f))->ChangeSize(1.3f);
            });
        }

        // 
        pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            if(i >= 12u)
            {
                if(!pEnemy->WasTeleporting() &&
                   (PHASE_POSITION_POINT(pEnemy, -0.6f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -1.0f * FOREGROUND_AREA.x, x)))
                {
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(0.0f,-1.0f))->ChangeSize(1.2f);
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(1.0f, 0.0f))->ChangeSize(1.2f);
                }
            }
            else if(i >= 6u)
            {
                if(!pEnemy->WasTeleporting() &&
                   (PHASE_POSITION_POINT(pEnemy,  0.2f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.2f * FOREGROUND_AREA.x, x)))
                {
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(0.0f,-1.0f))->ChangeSize(1.2f);
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(1.0f, 0.0f))->ChangeSize(1.2f);
                }
            }
            else
            {
                if(!pEnemy->WasTeleporting() &&
                   (PHASE_POSITION_POINT(pEnemy,  1.0f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy,  0.6f * FOREGROUND_AREA.x, x)))
                {
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(0.0f,-1.0f))->ChangeSize(1.2f);
                    //g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(1.0f, 0.0f))->ChangeSize(1.2f);
                }
            }
            /*
            if(i >= 12u)
            {
                if(!pEnemy->WasTeleporting() &&
                   (PHASE_POSITION_POINT(pEnemy,  1.0f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy,  0.6f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy,  0.2f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.2f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.6f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -1.0f * FOREGROUND_AREA.x, x)))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(0.0f,-1.0f))->ChangeSize(1.2f);
                }
            }
            else if(i >= 6u)
            {
                //if(!pEnemy->WasTeleporting() &&
                //   (PHASE_POSITION_POINT(pEnemy,  0.8f * FOREGROUND_AREA.x, x) ||
                //    PHASE_POSITION_POINT(pEnemy,  0.4f * FOREGROUND_AREA.x, x) ||
                //    PHASE_POSITION_POINT(pEnemy,  0.0f * FOREGROUND_AREA.x, x) ||
                //    PHASE_POSITION_POINT(pEnemy, -0.4f * FOREGROUND_AREA.x, x) ||
                //    PHASE_POSITION_POINT(pEnemy, -0.8f * FOREGROUND_AREA.x, x)))
                if(!pEnemy->WasTeleporting() &&
                   (PHASE_POSITION_POINT(pEnemy,  0.866f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy,  0.466f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy,  0.066f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.333f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.733f * FOREGROUND_AREA.x, x)))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(0.0f,-1.0f))->ChangeSize(1.2f);
                }
            }
            else
            {
                if(!pEnemy->WasTeleporting() &&
                   (PHASE_POSITION_POINT(pEnemy,  0.733f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy,  0.333f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.066f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.466f * FOREGROUND_AREA.x, x) ||
                    PHASE_POSITION_POINT(pEnemy, -0.866f * FOREGROUND_AREA.x, x)))
                {
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.5f, pEnemy, s_vPositionPoint, coreVector2(0.0f,-1.0f))->ChangeSize(1.2f);
                }
            }
            */
        });
    }
    else if(m_aiCounter[SUB_PHASE] == 0)
    {
        //#define TEST 0x08u
        #define TEST 0x01u
#define TEST2 (i >= 8u)
//#define TEST2 true

        // 
        const auto nEnemyFunc2 = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            if(i >= 16u) return;

            const coreUintW iRow = i % 8u;
            //const coreUintW iCol = i % 6u;

            const coreVector2 vPos = coreVector2(-1.0f + ((i >= 8u)/*(iRow & TEST)*/ ? 0.0f : 0.0f), -1.0f + 0.24f * I_TO_F(iRow));
            const coreVector2 vDir = coreVector2(TEST2/*(iRow & TEST)*/ ? -1.0f : 1.0f, 0.0f);

            if(bInit)
            {
                pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                pEnemy->Resurrect(((TEST2/*(iRow & TEST)*/ ? vPos.InvertedX() : vPos) - vDir * ((((iRow & TEST) == TEST)/* != TEST2*/) ? 0.5f : 1.5f)) * FOREGROUND_AREA, vDir);
            }

            pEnemy->DefaultMoveSmooth(TEST2 ? vPos.InvertedX() : vPos, 40.0f, 10.0f);
        };
        const auto nEnemyFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            if(i >= 16u) return;

            const coreUintW   iRow = i / 8u;
            const coreVector2 vDir = coreVector2((iRow & 0x01u) ? -1.0f : 1.0f, 0.0f).Rotated90();

            if(bInit)
            {
                //const coreUintW iRow = i / 6u;
                ////const coreUintW iCol = i % 6u;

                coreVector2 vPos = coreVector2(-1.2f + 2.4f * (0.5f/6.0f) * I_TO_F(i + (((i % 8u) >= 4u) ? 2u : 0u)) - (0.6f * 8.0f), iRow ? -1.0f : 1.0f);
                ////const coreVector2 vDir = coreVector2((iRow & 0x01u) ? -1.0f : 1.0f, 0.0f);
                vPos = vPos.InvertedY();

                pEnemy->SetBaseColor(COLOR_SHIP_BLUE);

                pEnemy->Resurrect((((iRow & 0x01u) ? vPos.InvertedX() : vPos).Rotated90()/* - vDir * 2.4f*/) * FOREGROUND_AREA, vDir);
                d_cast<cCinderEnemy*>(pEnemy)->SetAngle(I_TO_F(i));
            }

            pEnemy->DefaultMoveForward(vDir/*pEnemy->GetDirection().xy()*/, 30.0f);

            if(pEnemy->GetPosition().x * vDir/*pEnemy->GetDirection()*/.x > FOREGROUND_AREA.x * 1.2f)
                pEnemy->SetPosition(pEnemy->GetPosition() - coreVector3(vDir/*pEnemy->GetDirection().xy()*/ * (FOREGROUND_AREA.x * 2.4f), 0.0f));
            if(pEnemy->GetPosition().y * vDir/*pEnemy->GetDirection()*/.y > FOREGROUND_AREA.y * 1.2f)
                pEnemy->SetPosition(pEnemy->GetPosition() - coreVector3(vDir/*pEnemy->GetDirection().xy()*/ * (FOREGROUND_AREA.y * 2.4f), 0.0f));
        };

        // 
        if(bInit) pSquad->ForEachEnemyAll(nEnemyFunc);
             else pSquad->ForEachEnemy   (nEnemyFunc);

             UNUSED const cPlayer* pPlayer = g_pGame->GetPlayer(0u);

        // 
             static coreUintW iLastEnemy = 255u;
             coreUintW iNewLastEnemy = 255u;
   //         pSquad->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
   //         {
   //             //if(SIGN(pEnemy->GetPosition().x) != SIGN(pBall->GetPosition().x)) return;
   //             if(iLastEnemy == (i % 8u)) return;
   //
   //             //if(BALL_FLYPAST(pEnemy, pBall, y))
   //             if(!pEnemy->WasTeleporting() &&
   //                PHASE_FLYPAST(pEnemy, pPlayer, y))
   //             {
   //                 const coreVector2 vPos = coreVector2(pEnemy->GetPosition().x, pPlayer->GetPosition().y);
   //                 const coreVector2 vDir = coreVector2(-SIGN(pEnemy->GetPosition().x), 0.0f);
   //
   //                 g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
   //
   //                 iNewLastEnemy = i % 8u;
   //             }
   //         });

            PHASE_CONTROL_TICKER(2u, 0u, 1.0f, LERP_LINEAR)
            {
                pSquad->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
                {
                    if((iTick & 0x01u) == (i / 8u)) return;

                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = coreVector2(-SIGN(pEnemy->GetPosition().x), 0.0f);
                    const coreVector2 vTan = vDir.Rotated90() * FOREGROUND_AREA;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos + vTan * 0.05f, vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, pEnemy, vPos - vTan * 0.05f, vDir)->ChangeSize(1.3f);
                });
            });

            if(iNewLastEnemy != 255u) iLastEnemy = iNewLastEnemy;
        //#define TEST 0x02u
        //// 
        //const auto nEnemyFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        //{
        //    if(i >= 16u) return;
        //
        //    const coreUintW iRow = i % 8u;
        //    //const coreUintW iCol = i % 6u;
        //
        //    const coreVector2 vPos = coreVector2(-1.0f, -1.0f + 0.24f * I_TO_F(iRow));
        //    const coreVector2 vDir = coreVector2((iRow & TEST) ? -1.0f : 1.0f, 0.0f);
        //
        //    if(bInit)
        //    {
        //        pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
        //        pEnemy->Resurrect((((iRow & TEST) ? vPos.InvertedX() : vPos) - vDir * ((((iRow & TEST) == TEST) != (i >= 8u)) ? 0.5f : 1.0f)) * FOREGROUND_AREA, vDir);
        //    }
        //
        //    pEnemy->DefaultMoveSmooth((i >= 8u) ? vPos.InvertedX() : vPos, 40.0f, 10.0f);
        //};
        //
        //// 
        //if(bInit) pSquad->ForEachEnemyAll(nEnemyFunc);
        //     else pSquad->ForEachEnemy   (nEnemyFunc);
        //
        //// 
        //PHASE_CONTROL_TICKER(2u, 0u, 0.9f, LERP_LINEAR)
        //{
        //    pSquad->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        //    {
        //        const coreUintW iRow = i % 8u;
        //        const coreBool bRevert = (((iRow & TEST) == TEST) != (i >= 8u));
        //
        //        if(((iTick & 0x01u) == 0x01u) == bRevert) return;
        //
        //        const coreVector2 vBase = (pEnemy->GetDirection().xy() + coreVector2(0.0f,0.5f)).Normalized();
        //
        //        const coreVector2 vPos = pEnemy->GetPosition().xy();
        //        const coreVector2 vDir = bRevert? -vBase : vBase;
        //
        //        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, pEnemy, vPos, vDir)->ChangeSize(1.3f);
        //    });
        //});
    }
    else if(m_aiCounter[SUB_PHASE] == 0)
    {
        // 
        const auto nEnemyFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            if(i >= VAUS_SCOUTS_X * VAUS_SCOUTS_Y) return;


            const coreUintW t = i % 9u;
            const coreUintW b = i / 9u;
            const coreUintW x = t % 3u;
            const coreUintW y = t / 3u;

            const coreVector2 vPos = coreVector2(-0.2f + 0.2f * I_TO_F(x) + (b ? -0.5f : 0.5f), -0.4f + 0.2f * I_TO_F(y));

            if(bInit)
            {
                pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                pEnemy->Resurrect((vPos + coreVector2(0.0f, 1.0f + 0.12f * I_TO_F(x))) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
            }

            pEnemy->DefaultMoveSmooth(vPos, 40.0f, 10.0f);
        };

        // 
        if(bInit) pSquad->ForEachEnemyAll(nEnemyFunc);
             else pSquad->ForEachEnemy   (nEnemyFunc);

        // 
        PHASE_CONTROL_TICKER(2u, 0u, 10.0f, LERP_LINEAR)
        {
            //const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * DEG_TO_RAD(3.0f));

            pSquad->ForEachEnemy([&](cEnemy* OUTPUT pEnemy, const coreUintW i)
            {
                const coreVector2 vPos = pEnemy->GetPosition().xy();

                const coreVector2 vDir = (pBall->GetPosition().xy() - vPos).Normalized();

                const coreUintW t = i % 9u;
                const coreUintW b = i / 9u;
                if(t == 4)
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.4f, pEnemy, vPos, b ? vDir : vDir);
            });
        });
    }
    else if(m_aiCounter[SUB_PHASE] == 0)
    {
        // 
        const auto nEnemyFunc = [&](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            if(i >= VAUS_SCOUTS_X * VAUS_SCOUTS_Y) return;

            const coreUintW x = i % VAUS_SCOUTS_X;
            const coreUintW y = i / VAUS_SCOUTS_X;

            const coreVector2 vPos = coreVector2(-0.7f + 0.2f * I_TO_F(x), 0.47f + 0.2f * I_TO_F(y));

            if(bInit)
            {
                pEnemy->SetBaseColor(COLOR_SHIP_BLUE);
                pEnemy->Resurrect((vPos + coreVector2(0.0f, 1.0f + 0.12f * I_TO_F(x))) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
            }

            pEnemy->DefaultMoveSmooth(vPos, 40.0f, 10.0f);
        };

        // 
        if(bInit) pSquad->ForEachEnemyAll(nEnemyFunc);
             else pSquad->ForEachEnemy   (nEnemyFunc);

        // 
        pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
        {
            const coreInt32 iHealthDiff = pEnemy->GetPreHealth() - pEnemy->GetCurHealth();
            if(iHealthDiff < 1) return;

            for(coreUintW j = 5u; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f));

                // 
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(),  vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(),  vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, pEnemy, pEnemy->GetPosition().xy(), -vDir);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, pEnemy, pEnemy->GetPosition().xy(), -vDir);
            }
        });
    }

    // 
    if(pSquad->IsFinished()) pMission->MakeSticky();


    vTestOldPos = pBall->GetPosition().xy();
#endif
}


/*
3-5 level, wie in arkanoid, wenn man alle schaft kommt fragment
dann kommen helfer von der seite schnell drehend und spannen laser nach mitte
helfer greifen in dieser phase unten an
|     OOO     |
|             |
|-------------|
|O...         |
|      ^  ...O|
>   #
># # #
> #
>1#####     >
>           >2#####
>           >    #####2
>    #####1 >
*/

/*

+--vvvvvvvv--+ 12
|            |
|  ########  |
|  ########  |
|            |
|            |
|            |
+------------+

+-----------+ 11
|    ^^^    |
>### ### ###|
>### ### ###| fliegen mit abstand ein
>### ### ###|
|vvv     vvv|
|           |
+-----------+

+------------+ 12
|############> ~~?
|            |
|############<
|            |
|############>
|            |
+------------+

+------------+ 12
|            |
|#          #|
| #        # |
|#          #|
| #        # |
|#          #|
+------------+

+------------+ 12
|            |
|            |
|            |
|            |
|  ##  ##  ##|
|##  ##  ##  |
+^^^^^^^^^^^^+

+------------+ 12
|            |
|            |
|            |
|            |
|            |
|            |
+------------+

+------------+ 12
|            |
|            |
|            |
|            |
|            |
|            |
+------------+

*/


/*
in dritter phase spielt vaus unten mit companion oben, ball hat orange seiten-angriffe X0X0X0 -> 0X0X0X
sobald spieler paddle hat und ihn zum ersten mal erwischt ist seiten-angriff permanent
*/


/*
laser von allen 3 bosse nausgehend
bei phasenwechsel seitenbosse sterben, dropen schilde zum einsammeln (markiert)
boss bringt seinen laser zur mitte
schießt ball mit aller kraft nach unten, muss man ausweichen, ball größer schneller
spieler kann schild nehmen und auch ball so schießen (aber nicht aktiv)
laser in mitte (oder seite) blockiert unbeschadet spieler oder paddle


*/