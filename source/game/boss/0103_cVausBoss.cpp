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
}


// ****************************************************************
// 
void cVausBoss::__ResurrectOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetMission());
    cWarriorEnemy*  pWarrior = NULL;//pMission->GetWarrior();

    // 
    pMission->EnableBall  (0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalize());
    pMission->EnablePaddle(0u, this);
    pMission->EnablePaddle(1u, pWarrior);

    // 
    pWarrior->Resurrect(coreVector2(0.0f,2.0f) * FOREGROUND_AREA, coreVector2(0.0f,-1.0f));
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
    cWarriorEnemy*  pWarrior = NULL;//pMission->GetWarrior();

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        if(m_aiCounter[SCOUT_RESURRECTIONS] >= 60)
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
        PHASE_CONTROL_TICKER(0u, 0u, 2.0f)
        {
            m_iPhase = 10u;
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 2.0f)
        {
            m_iPhase = 10u;

        });
    }



    
    //if(pWarrior->GetCurHealthPct() < 0.9f)
    //{
    //
    //    PHASE_CONTROL_TICKER(2u, 0u, 1.0f)
    //    {
    //        for(coreUintW i = 0u; i < VIRIDO_SCOUTS; ++i)
    //        {
    //            cScoutEnemy* pScout = pMission->GetScout(i);
    //            if(CONTAINS_VALUE(pScout->GetStatus(), ENEMY_STATUS_DEAD)) continue;
    //
    //            cPlayer* pPlayer = g_pGame->FindPlayer(pScout->GetPosition().xy());
    //
    //            const coreVector2 vDir = (pPlayer->GetPosition().xy() - pScout->GetPosition().xy()).Normalize();
    //
    //            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, pScout->GetPosition().xy(), vDir)->MakeYellow();
    //        }
    //
    //    });
    //
    //
    //
    //    for(coreUintW i = 0u; i < VIRIDO_SCOUTS; ++i)
    //    {
    //        cScoutEnemy* pScout = pMission->GetScout(i);
    //
    //        const coreUintW A = (i/8u + (CONTAINS_BIT(m_iScoutOrder, i%8u) ? 1u : 0u)) & 0x01u;
    //
    //        const coreVector2 vGridPos = coreVector2(-0.7f + 0.2f*I_TO_F(i%8u), 0.47f + 0.2f*I_TO_F(A));
    //
    //        if(CONTAINS_VALUE(pScout->GetStatus(), ENEMY_STATUS_DEAD))
    //        {
    //            if(!A) m_iScoutOrder ^= BIT(i%8u);
    //
    //            if(m_aiCounter[SCOUT_RESURRECTIONS] < 80)//40)
    //            {
    //                pScout->Resurrect(vGridPos * FOREGROUND_AREA + coreVector2(0.0f, 3.0f*FOREGROUND_AREA.y), coreVector2(0.0f,-1.0f));
    //                ++m_aiCounter[SCOUT_RESURRECTIONS];
    //            }
    //        }
    //
    //        pScout->DefaultMoveSmooth(vGridPos, 3.0f, 30.0f);
    //    }
    //
    //    STATIC_ASSERT(VIRIDO_SCOUTS/2u <= sizeof(m_iScoutOrder)*8u)
    //
    //}

    const coreFloat fDiff  = pBall->GetPosition().x - this->GetPosition().x;
    const coreFloat fSpeed = fDiff * 20.0f * Core::System->GetTime();// * RCP(MAX((ABS(vDiff.y) - 5.0f) / 4.0f, 1.0f));


    // 
    this->SetPosition(coreVector3(this->GetPosition().x + fSpeed, this->GetPosition().yz()));



    //g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    //{
    //    const coreVector3& vCurPos = pPlayer->GetPosition();
    //    pPlayer->SetPosition(coreVector3(vCurPos.x, MAX(vCurPos.y, this->GetPosition().y + 9.5f), vCurPos.z));
    //    pPlayer->SetNewPos  (pPlayer->GetPosition().xy());
    //});


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
    

    pWarrior->DefaultMoveSmooth(coreVector2(this->GetPosition().x / FOREGROUND_AREA.x, 0.93f), 2.0f, 20.0f);
    pWarrior->SetPosition(coreVector3(this->GetPosition().x, pWarrior->GetPosition().yz()));


    for(coreUintW j = 0u; j < 2u; ++j)
    {
        if(pMission->GetBounceState(j))
        {
            coreObject3D* pPaddle = pMission->GetPaddle(j);

            const coreVector2 vPos = pPaddle->GetPosition().xy();

            const coreFloat fAngle = pPaddle->GetDirection().xy().Angle();
            for(coreUintW i = 0u; i < 7u; ++i)
            {
                const coreVector2 vDir = coreVector2::Direction(fAngle + 0.25f * I_TO_F(i - 3u));
                auto* pBullet1 = g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*5.0f, vDir)->MakeBlue();
                auto* pBullet2 = g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos + vDir*2.5f, vDir)->MakeBlue();

                if(j)
                {
                    pBullet1->MakeRed();
                    pBullet2->MakeRed();
                }
            }
        }
    }

    g_pEnvironment->SetTargetSide(this->GetPosition().xy() * MIN(m_fLifeTime*0.1f, 1.0f) * 0.5f);

    // fängt ball, geht runter, bild dreht sich, kommt oben rauf und macht seinen ersten erngieangriff
}