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


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cProjectOneBoss::cProjectOneBoss()noexcept
{
    // load object resources
    this->DefineModelHigh("ship_projectone.md3");
    this->DefineModelLow ("ship_projectone.md3");
    this->DefineTexture  (0u, "ship_player.png");

    // set object properties
    this->SetSize(coreVector3(1.2f,1.2f,1.2f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_GREY);
}


// ****************************************************************
// 
void cProjectOneBoss::__ResurrectOwn()
{

}


// ****************************************************************
// 
void cProjectOneBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveOwn()
{
    // ################################################################
    // 
     if(m_iPhase == 0u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.6f)
        {
            /*
            cEnemySquad* pSquad = g_pGame->GetCurMission()->GetEnemySquad(0u);

            pSquad->ForEachEnemy([](cEnemy* OUTPUT pEnemy, const coreUintW i)
            {
                pEnemy->Kill(true);
            });

            g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vDir = (pPlayer->GetPosition().xy() - this->GetPosition().xy()).Normalized();

                pPlayer->SetForce(vDir * 120.0f);
            });

            g_pSpecialEffects->MacroExplosionDarkBig(coreVector3(0.0f, FOREGROUND_AREA.y, 0.0f));
            */
            PHASE_CHANGE_INC
        });
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
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_TO(20u)
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
        if(CONTAINS_FLAG(g_pGame->GetStatus(), GAME_STATUS_PLAY))
        {
            PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(Core::Rand->Float(-90.0f, 90.0f) + 180.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.6f, this, vPos, vDir)/*->MakeWhite()*/->ChangeSize(1.2f);
            });

            const coreVector2 vPlayerPos = this->NearestPlayer()->GetPosition().xy();
            this->SetPosition(coreVector3(vPlayerPos.x, 0.8f * FOREGROUND_AREA.y, 0.0f));

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([&](cFlipBullet* OUTPUT pBullet)
            {
                if(pBullet->GetFlyTime() >= 2.0f) return;

                const coreVector2 vAim = (vPlayerPos - pBullet->GetPosition().xy()).Normalized();
                const coreVector2 vDir = (pBullet->GetFlyDir() + vAim * (2.0f * Core::System->GetTime())).Normalized();

                pBullet->SetSpeed (pBullet->GetSpeed() + 2.0f * BULLET_SPEED_FACTOR * Core::System->GetTime());   
                pBullet->SetFlyDir(vDir);
            });

            g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                if(pPlayer->IsFeeling()) pPlayer->Kill(false);
            });
        }
    }

    // startest mit rotation nach links, l‰dt strahl auf, schieﬂt strahl, sinusbewegung nach rechts bis ende
}