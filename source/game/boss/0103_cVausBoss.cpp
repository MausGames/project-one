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
    if(CONTAINS_BIT(pMission->GetBounceState(), 0u))
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


// ****************************************************************
// 
void cVausBoss::__UpdateBreakout()
{
}