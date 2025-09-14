///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// TODO 5: Torus creates the ball at the beginning (with charge and recoil-spin effect), can be shot, but does nothing (?)


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
    if(m_bSkipped) return;

    // load models
    this->DefineModelHigh("ship_boss_vaus_high.md3");
    this->DefineModelLow ("ship_boss_vaus_low.md3");
    this->DefineVolume   ("ship_boss_vaus_volume.md3");

    // set object properties
    this->SetSize(coreVector3(2.5f,2.5f,2.5f));

    // configure the boss
    this->Configure(10000, COLOR_SHIP_YELLOW);

    // 
    PHASE_HEALTH_GOAL({0})

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aCompanion); ++i)
    {
        m_aCompanion[i].DefineModelHigh("ship_boss_vaus_companion_high.md3");
        m_aCompanion[i].DefineModelLow ("ship_boss_vaus_companion_low.md3");
        m_aCompanion[i].DefineVolume   ("ship_boss_vaus_companion_volume.md3");
        m_aCompanion[i].SetSize        (coreVector3(2.2f,2.2f,2.2f));
        m_aCompanion[i].Configure      (1, COLOR_SHIP_YELLOW);
    }
}


// ****************************************************************
// destructor
cVausBoss::~cVausBoss()
{
    if(m_bSkipped) return;

    // 
    this->Kill(false);
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
    for(coreUintW i = 0u; i < VIRIDO_PADDLES; ++i) pMission->DisablePaddle(i, bAnimated);
}


// ****************************************************************
// 
void cVausBoss::__MoveOwn()
{
    cViridoMission* pMission = d_cast<cViridoMission*>(g_pGame->GetCurMission());
    coreObject3D*   pBall    = pMission->GetBall(0u);

    // 
    this->_UpdateBoss();

    if(this->GetCurHealthPct() < 0.9f)
    {
        this->Kill(true);                
        this->_EndBoss();
    }

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        pMission->MakeSticky();
        if(!pMission->GetStickyState()) {pBall->SetPosition(coreVector3(0.0f,-0.8f,0.0f) * FOREGROUND_AREA3);pBall->SetDirection(coreVector3(0.0f,-1.0f,0.0f));}
        
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            const coreFloat fCur = this->GetPosition().x / FOREGROUND_AREA.x;
            this->DefaultMoveLerp(coreVector2(fCur, -1.2f), coreVector2(fCur, -1.0f), fTime);
            
            this->SetDirection(coreVector3(0.0f,1.0f,0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        
       // pBall->SetPosition(coreVector3(0.0f,-0.8f,0.0f) * FOREGROUND_AREA3);
        
        if(pMission->GetStickyState())
        {
            PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
            {
                if(PHASE_BEGINNING)
                {
                    this->StorePosition();
                    this->StoreRotation();
                    m_aiCounter[IGNORE_BALL] = 1;
                }

                if(fTime < 0.46f)
                {
                    this->DefaultMoveLerp(m_vLastPosition, m_vLastPosition + coreVector2(0.0f,0.3f), SIN(PI * BLENDBR(fTime / 0.46f)));
                }
                else
                {
                    const coreVector2 vNewPos   = coreVector2(0.0f, -m_vLastPosition.y);
                    const coreFloat   fOldAngle = m_vLastDirection.xy().Angle();
                    const coreFloat   fNewAngle = fOldAngle + 1.0f*PI;// + 1.5f*PI;
    
                    this->DefaultMoveLerp  (m_vLastPosition, vNewPos,   (fTime - 0.46f) / 0.54f);
                    this->DefaultRotateLerp(fOldAngle,       fNewAngle, (fTime - 0.46f) / 0.54f);
                }

                if(PHASE_FINISHED)
                {
                    PHASE_CHANGE_INC
                    //PHASE_RESET(0u)

                    m_aiCounter[IGNORE_BALL]   = 0;
                    m_aiCounter[SUB_PHASE]    += 1;
                    m_aiCounter[CURRENT_SIDE] += 1;

                    //const coreVector2 vNewBallDir = (this->GetDirection().xy() + this->GetDirection().xy().Rotated90() * 0.5f).Normalized();

                    //pMission->UnmakeSticky(vNewBallDir);
                    //g_pSpecialEffects->CreateBlowColor(pBall->GetPosition(), coreVector3(vNewBallDir, 0.0f), SPECIAL_BLOW_SMALL, COLOR_ENERGY_GREEN);
                    
                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                    
                    this->_StartBoss();
                }
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_TIMER(0u, 0.85f, LERP_BREAK)
        {
            
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
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

            const coreFloat fDrop   = (fSpeed / FRAMERATE_MIN) / (BLENDB(fSpeed / FRAMERATE_MIN)) * 0.5f / fSpeed; // TODO 1: wtf, get rid of time value if necessary  
            const coreFloat fNewPos = m_avVector[0].x + pBall->GetDirection().x * VIRIDO_BALL_SPEED * fDrop;

            this->DefaultMoveLerp(m_avVector[0].xy(), coreVector2(fNewPos, -0.95f), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 4u)
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
    else if(m_iPhase == 5u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f/2.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp  (m_avVector[0].xy(), coreVector2(0.0f,0.95f), fTime);
            this->DefaultRotateLerp(0.0f*PI,            3.0f*PI,                 fTime);

            m_aCompanion[0].DefaultMoveLerp(coreVector2(-1.8f,-0.6f), coreVector2(-0.95f,-0.6f), fTime);
            m_aCompanion[1].DefaultMoveLerp(coreVector2( 1.8f,-0.6f), coreVector2( 0.95f,-0.6f), fTime);

            //if(PHASE_TIME_POINT(0.35f))
            //    this->_StartBoss();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 6u)
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
    if(HAS_BIT(pMission->GetBounceState(), 0u))
    {
       //||
       //HAS_BIT(pMission->GetBounceState(), 7u))
        const coreObject3D* pPaddle = pMission->GetPaddle(0);

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