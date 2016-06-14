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


// ****************************************************************
// constructor
cVausBoss::cVausBoss()noexcept
: m_iScoutOrder (0u)
{
    // load models
    this->DefineModelHigh("ship_boss_vaus_high.md3");
    this->DefineModelLow ("ship_boss_vaus_low.md3");

    // 
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
    // gegner die ihren schaden weiterleiten ?
    // forwarddamageto vergleicht cur mit max, leitet diff weiter und regeneiert ? combat text
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
void cVausBoss::__RenderOwnAttack()
{
    //
    //m_aCompanion[0].Render();
    //m_aCompanion[1].Render();

    //
    //g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aCompanion[0]);
    //g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aCompanion[1]);
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
        if(m_aiCounter[SCOUT_RESURRECTIONS] >= (VAUS_SCOUTS / 2))
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
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        if(pSquad->IsFinished())
        {
            pMission->MakeSticky();
            if(pMission->GetStickyState())
            {
                m_avVector[0].xy(this->GetPosition().xy() / FOREGROUND_AREA);
                m_iPhase = 10u; 
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_SMOOTH)
        {
            // 
            this->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(0.0f,-0.95f), fTime);
            //this->DefaultRotateLerp(0.0f*PI,                 3.0f*PI,                  fTime);

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_PAUSE(0u, 4.0f)
        {
            ++m_iPhase;

            // 
            m_aCompanion[0].Resurrect(coreVector2(-1.8f * FOREGROUND_AREA.x, 0.0f), coreVector2( 1.0f,0.0f));
            m_aCompanion[1].Resurrect(coreVector2( 1.8f * FOREGROUND_AREA.x, 0.0f), coreVector2(-1.0f,0.0f));

            pMission->EnablePaddle(1u, &m_aCompanion[0]);
            pMission->EnablePaddle(2u, &m_aCompanion[1]);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.5f, LERP_SMOOTH)
        {
            // 
            //const coreFloat fCur = this->GetPosition().x / FOREGROUND_AREA.x;
            this->DefaultMoveLerp  (coreVector2(0.0f, -0.95f), coreVector2(0.0f, 0.95f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                   3.0f*PI,                  fTime);



            m_aCompanion[0].DefaultMoveLerp(coreVector2(-1.8f,0.0f), coreVector2(-0.95f,0.0f), fTime);
            m_aCompanion[1].DefaultMoveLerp(coreVector2( 1.8f,0.0f), coreVector2( 0.95f,0.0f), fTime);


            // 
            if(PHASE_TIME_POINT(0.35f))
            {
                g_pGame->GetMission()->SetCurBoss(this);
                g_pGame->GetInterface()->ShowBoss(this);
            }

            // 
            if(PHASE_FINISHED)
            {
                ++m_iPhase;

                //coreObject3D* P1 = pMission->GetPaddle(0u);
                //coreObject3D* P2 = pMission->GetPaddle(1u);
                //coreObject3D* P3 = pMission->GetPaddle(2u);
                //
                //coreFloat x = P3->GetPosition().x - P2->GetPosition().x - P2->GetCollisionRange().y*2.0f - pBall->GetCollisionRadius()*2.0f;
                //coreFloat y = P1->GetPosition().y + FOREGROUND_AREA.y - P1->GetCollisionRange().y  - pBall->GetCollisionRadius() * 0.5f;
             //   pMission->GetPaddle(0u)->GetPosition().x + FOREGROUND_AREA

                coreVector2 v1 = coreVector2(-1.0f,-1.2099035f).Normalize();
                //coreVector2 v2 = coreVector2(-x,-y).Normalize();
                //coreVector2 v2 = coreVector2(-1.61f,-1.0f).Normalize();
                coreVector2 v2 = coreVector2(-2.45f,-1.0f).Normalize();
                coreVector2 v3 = coreVector2(-1.0f,-3.63f).Normalize();

                // 
                pMission->UnmakeSticky(v2);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 13u)
    {
        const coreFloat fDiff  = pBall->GetPosition().y - m_aCompanion[0].GetPosition().y;
        const coreFloat fSpeed = fDiff * 20.0f * Core::System->GetTime();


        // 
        //this->SetPosition(coreVector3(this->GetPosition().x + fSpeed, this->GetPosition().yz()));




        m_aCompanion[0].SetPosition(coreVector3(m_aCompanion[0].GetPosition().x, m_aCompanion[0].GetPosition().y + fSpeed, m_aCompanion[0].GetPosition().z));   
        m_aCompanion[1].SetPosition(coreVector3(m_aCompanion[1].GetPosition().x, m_aCompanion[1].GetPosition().y + fSpeed, m_aCompanion[1].GetPosition().z));   
    
        static coreFloat prev = 0.0f;

        if(SIGN(prev) != SIGN(pBall->GetDirection().y))
        {
     //       pBall->SetPosition(coreVector3(-pBall->GetPosition().x, pBall->GetPosition().yz()));
    //        pBall->SetPosition(coreVector3(0.0f, pBall->GetPosition().yz()));
        }
        prev = SIGN(pBall->GetDirection().y);

       //if(pMission->GetBounceState(0u))
       //    pBall->SetPosition(coreVector3(0.0f, pBall->GetPosition().yz()));
    }



        PHASE_CONTROL_TICKER(2u, 0u, 1.0f)
        {
            for(coreUintW i = 0u; i < pSquad->GetNumEnemies(); ++i)
            {
                cEnemy* pScout = pSquad->GetEnemy(i);//pMission->GetScout(i);
                if(CONTAINS_FLAG(pScout->GetStatus(), ENEMY_STATUS_DEAD)) continue;

                cPlayer* pPlayer = g_pGame->FindPlayer(pScout->GetPosition().xy());

                const coreVector2 vDir = (pPlayer->GetPosition().xy() - pScout->GetPosition().xy()).Normalize();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, pScout->GetPosition().xy(), vDir)->MakeOrange();
            }

        });



        for(coreUintW i = 0u; i < pSquad->GetNumEnemies(); ++i)
        {
            cEnemy* pScout = pSquad->GetEnemy(i);//pMission->GetScout(i);

            const coreUintW A = (i/8u + (CONTAINS_BIT(m_iScoutOrder, i%8u) ? 1u : 0u)) & 0x01u;

            const coreVector2 vGridPos = coreVector2(-0.7f + 0.2f*I_TO_F(i%8u), 0.47f + 0.2f*I_TO_F(A));

            if(CONTAINS_FLAG(pScout->GetStatus(), ENEMY_STATUS_DEAD))
            {
                if(!A) TOGGLE_BIT(m_iScoutOrder, i % 8u)

                if(m_aiCounter[SCOUT_RESURRECTIONS] < VAUS_SCOUTS)
                {
                    pScout->Resurrect(vGridPos * FOREGROUND_AREA + coreVector2(0.0f, 3.0f*FOREGROUND_AREA.y), coreVector2(0.0f,-1.0f));
                    ++m_aiCounter[SCOUT_RESURRECTIONS];
                }
            }

            pScout->DefaultMoveSmooth(vGridPos, 3.0f, 30.0f);
        }

        ASSERT(pSquad->GetNumEnemies()/2u <= sizeof(m_iScoutOrder)*8u)


    const coreFloat fDiff  = pBall->GetPosition().x - this->GetPosition().x;
    const coreFloat fSpeed = fDiff * 20.0f * Core::System->GetTime();// * RCP(MAX((ABS(vDiff.y) - 5.0f) / 4.0f, 1.0f));


    // 
    if(m_iPhase != 12u) this->SetPosition(coreVector3(this->GetPosition().x + fSpeed, this->GetPosition().yz()));


//    this->SetPosition(coreVector3(this->GetPosition().x, -0.95f * FOREGROUND_AREA.y, this->GetPosition().z));

    if(m_iPhase == 0u)
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        const coreVector3& vCurPos = pPlayer->GetPosition();
        pPlayer->SetPosition(coreVector3(vCurPos.x, MAX(vCurPos.y, this->GetPosition().y + 9.5f), vCurPos.z));
        pPlayer->SetNewPos  (pPlayer->GetPosition().xy());
    });


    //Core::Manager::Object->TestCollision(TYPE_PLAYER, this, [&](cPlayer* OUTPUT pPlayer, const coreBool bFirstHit)
    //{
    //    const coreVector2 vDiff = pPlayer->GetPosition().xy() - this->GetPosition().xy();
    //    const coreVector2 vSize = pPlayer->GetCollisionRange().xy() + this->GetCollisionRange().xy();
    //
    //    const coreVector2 vMove = coreVector2(MAX(vSize.x - ABS(vDiff.x), 0.0f) * SIGN(vDiff.x),
    //                                          MAX(vSize.y - ABS(vDiff.y), 0.0f) * SIGN(vDiff.y));
    //    //pPlayer->SetForce(vForce * 50.0f);
    //
    //    pPlayer->SetPosition(coreVector3(pPlayer->GetPosition().xy() + vMove, 0.0f));
    //});
    

    //pWarrior->DefaultMoveSmooth(coreVector2(this->GetPosition().x / FOREGROUND_AREA.x, 0.93f), 2.0f, 20.0f);
    //pWarrior->SetPosition(coreVector3(this->GetPosition().x, pWarrior->GetPosition().yz()));


    for(coreUintW j = 0u; j < 3u; ++j)
    {
        if(pMission->GetBounceState(j))
        {
            coreObject3D* pPaddle = pMission->GetPaddle(j);

            const coreVector2 vPos = pPaddle->GetPosition().xy();

            const coreFloat fAngle = pPaddle->GetDirection().xy().Angle();
            for(coreUintW i = 0u; i < 7u; ++i)
            {
                const coreVector2 vDir = coreVector2::Direction(fAngle + ((j || true) ? 0.1f : 0.25f) * I_TO_F(i - 3u));

                auto* pBullet1 = g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*5.0f, vDir);
                if(j) pBullet1->MakeRed();
                else  pBullet1->MakeBlue();

                auto* pBullet2 = g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*2.5f, vDir);
                if(j) pBullet2->MakeRed();
                else  pBullet2->MakeBlue();
            }
        }
    }

    g_pEnvironment->SetTargetSide(this->GetPosition().xy() * MIN(m_fLifeTime*0.1f, 1.0f) * 0.5f);

    // fängt ball, geht runter, bild dreht sich, kommt oben rauf und macht seinen ersten erngieangriff
}