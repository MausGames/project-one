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
// constructor
cVausBoss::cVausBoss()noexcept
: m_iScoutOrder (0u)
{
    // load models
    this->DefineModel   ("ship_boss_vaus_high.md3");
    this->DefineModelLow("ship_boss_vaus_low.md3");

    // 
    this->SetSize(coreVector3(2.5f,2.5f,2.5f));

    // configure the boss
    this->Configure(10000, coreVector3(51.0f/360.0f, 100.0f/100.0f, 85.0f/100.0f).HSVtoRGB());
}


// ****************************************************************
// destructor
cVausBoss::~cVausBoss()
{
    // 
    s_cast<cViridoMission*>(g_pGame->GetMission())->DisablePaddle(0u, false);
}


// ****************************************************************
// 
void cVausBoss::__ResurrectOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetMission());

    // 
    pMission->EnableBall  (0u, coreVector2(0.0f,0.0f), coreVector2(-0.5f,1.0f).Normalize());
    pMission->EnablePaddle(0u);
}


// ****************************************************************
// 
void cVausBoss::__KillOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetMission());

    // 
    pMission->DisablePaddle(0u, true);
}


// ****************************************************************
// 
void cVausBoss::__MoveOwn()
{
    cViridoMission* pMission = s_cast<cViridoMission*>(g_pGame->GetMission());
    coreObject3D*   pBall    = pMission->GetBall(0u);

  

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_BREAK)
        {
            // 
            this->DefaultMoveLerp(coreVector2(0.0f,-2.0f), coreVector2(this->GetPosition().x / FOREGROUND_AREA.x, -0.95f), fTime);

            // 
            if(PHASE_FINISHED)
                ++m_iPhase;
        });
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


    PHASE_CONTROL_TICKER(2u, 0u, 1.0f)
    {
        for(coreUintW i = 0u; i < VIRIDO_SCOUTS; ++i)
        {
            cScoutEnemy* pScout = pMission->GetScout(i);

            cPlayer* pPlayer = g_pGame->FindPlayer(pScout->GetPosition().xy());

            const coreVector2 vDir = (pPlayer->GetPosition().xy() - pScout->GetPosition().xy()).Normalize();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, pScout->GetPosition().xy(), vDir)->MakeYellow();
        }

    });


    for(coreUintW i = 0u; i < VIRIDO_SCOUTS; ++i)
    {
        cScoutEnemy* pScout = pMission->GetScout(i);

        const coreVector2 vGridPos = coreVector2(-0.7f + 0.2f*I_TO_F(i%8u), 0.5f + 0.2f*I_TO_F((i/8u + (CONTAINS_BIT(m_iScoutOrder, i%8u) ? 1u : 0u)) & 0x01u)) * FOREGROUND_AREA;

        if(CONTAINS_VALUE(pScout->GetStatus(), ENEMY_STATUS_DEAD))
        {
            m_iScoutOrder ^= BIT(i%8u);
            pScout->Resurrect(vGridPos + coreVector2(0.0f, 2.0f*FOREGROUND_AREA.y), coreVector2(0.0f,-1.0f));
        }

        pScout->DefaultMoveSmooth(vGridPos, 4.0f, 40.0f);
    }

    STATIC_ASSERT(VIRIDO_SCOUTS/2u <= sizeof(m_iScoutOrder)*8u)


    const coreVector2 vDiff  = pBall->GetPosition().xy() - this->GetPosition().xy();
    const coreFloat   fSpeed = vDiff.x * RCP(MAX((ABS(vDiff.y) - 5.0f) / 5.0f, 1.0f));

    // 
    this->SetPosition(coreVector3(this->GetPosition().x + fSpeed, this->GetPosition().yz()));



    if(pMission->GetPaddleBounce(0u))
    {
        const coreVector2 vPos = this->GetPosition().xy();

        const coreFloat fAngle = this->GetDirection().xy().Angle();
        for(coreUintW i = 0u; i < 7u; ++i)
        {
            //if(3u <= i && i <= 3u) continue;

            const coreVector2 vDir = coreVector2::Direction(fAngle + 0.25f * I_TO_F(i - 3u));
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.6f - 0.2f * ABS(I_TO_F(i - 3u)), this, vPos + vDir*5.0f, vDir);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.6f - 0.2f * ABS(I_TO_F(i - 3u)), this, vPos + vDir*2.5f, vDir);
        }

        //g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.0f, this, this->GetPosition().xy(), this->GetDirection().xy());
    }


}