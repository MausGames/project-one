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
#define CONTAINER_ATTACHED (0u)
#define DASH_SIDE          (1u)
#define SPIN_SIDE          (2u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cNautilusBoss::cNautilusBoss()noexcept
: m_fClawAngle (0.0f)
, m_fAnimation (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_nautilus_body_high.md3");
    this->DefineModelLow ("ship_boss_nautilus_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(2.0f,2.0f,2.0f));

    // configure the boss
    this->Configure(8000, COLOR_SHIP_PURPLE);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].DefineModelHigh("ship_boss_nautilus_claw_high.md3");
        m_aClaw[i].DefineModelLow ("ship_boss_nautilus_claw_low.md3");
        m_aClaw[i].SetSize        (coreVector3(2.0f,2.0f,2.0f));
        m_aClaw[i].Configure      (1, COLOR_SHIP_PURPLE);
        m_aClaw[i].SetParent      (this);
    }
}


// ****************************************************************
// 
void cNautilusBoss::__ResurrectOwn()
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableContainer     (coreVector2(1000.0f,1000.0f));
    pMission->SetContainerClamp   (false);
    pMission->SetContainerOverdraw(true);

    // 
    m_aiCounter[CONTAINER_ATTACHED] = 1;
}


// ****************************************************************
// 
void cNautilusBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cNautilusBoss::__MoveOwn()
{
    cNevoMission* pMission   = d_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

    // ################################################################
    // ################################################################

    if((m_iPhase < 10u) && Core::System->GetTime())
    {
        // 
        m_fAnimation.UpdateMod(1.0f, 2.0f*PI);

        // 
        const coreFloat fPrevX = 0.6f * SIN(m_fAnimation - Core::System->GetTime());
        const coreFloat fCurX  = 0.6f * SIN(m_fAnimation);

        // 
        this->SetPosition (coreVector3(fCurX * FOREGROUND_AREA.x, this->GetPosition().yz()));
        this->SetDirection(coreVector3(fCurX - fPrevX, Core::System->GetTime(), 0.0f).Normalized());
    }

    if(m_aiCounter[CONTAINER_ATTACHED])
    {
        // 
        pContainer->SetPosition (this->GetPosition () + NAUTILUS_ATTACH_DIST * this->GetDirection());
        pContainer->SetDirection(this->GetDirection());
    }

    // 
    const coreVector2& vImpact = pMission->GetContainerImpact();
    if(!vImpact.IsNull())
    {
        for(coreUintW i = 20u; i--; )
        {
            const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(i) * 9.0f));

            // 
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact,  vDir)->ChangeSize(1.3f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.7f, this, vImpact, -vDir)->ChangeSize(1.3f);
        }

        // 
        g_pSpecialEffects->CreateSplashColor(coreVector3(vImpact, 0.0f), SPECIAL_SPLASH_BIG, COLOR_ENERGY_BLUE);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
    }

    // 
    const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.11f * m_fClawAngle, this->GetOrientation()).m123();

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClaw); ++i)
    {
        m_aClaw[i].SetPosition   (this->GetPosition   ());
        m_aClaw[i].SetDirection  (this->GetDirection  () * (i ? mRota : mRota.Transposed()));
        m_aClaw[i].SetOrientation(this->GetOrientation() * (i ? -1.0f : 1.0f));
    }
}