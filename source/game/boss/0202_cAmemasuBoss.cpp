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
#define CONNECTED_MOUTH (0u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cAmemasuBoss::cAmemasuBoss()noexcept
: m_fMouthAngle (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize       (coreVector3(0.0f,0.0f,0.0f));
    this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(3000, COLOR_SHIP_CYAN);

    // 
    m_Top.DefineModelHigh("ship_boss_amemasu_top_high.md3");
    m_Top.DefineModelLow ("ship_boss_amemasu_top_low.md3");
    m_Top.SetSize        (coreVector3(3.0f,3.0f,3.0f));
    m_Top.Configure      (1, COLOR_SHIP_CYAN);
    m_Top.SetParent      (this);

    // 
    m_Bottom.DefineModelHigh("ship_boss_amemasu_bottom_high.md3");
    m_Bottom.DefineModelLow ("ship_boss_amemasu_bottom_low.md3");
    m_Bottom.SetSize        (coreVector3(3.0f,3.0f,3.0f));
    m_Bottom.Configure      (1, COLOR_SHIP_CYAN);
    m_Bottom.SetParent      (this);

    // 
    m_ChangePath.AddNode(coreVector2(0.0f, 0.0f),  coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f, 0.01f), coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f,-3.0f),  coreVector2(0.0f,-1.0f));
    m_ChangePath.Refine();

    // 
    m_apStomachTex[0] = Core::Manager::Resource->Get<coreTexture>("environment_mars_diff.png");
    m_apStomachTex[1] = Core::Manager::Resource->Get<coreTexture>("environment_rock_diff.png");
    m_apStomachTex[2] = Core::Manager::Resource->Get<coreTexture>("environment_dust_diff.png");
    m_apStomachTex[3] = Core::Manager::Resource->Get<coreTexture>("environment_earth_diff.png");
}


// ****************************************************************
// 
void cAmemasuBoss::__ResurrectOwn()
{
    cNevoMission* pMission = d_cast<cNevoMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableContainer     (coreVector2(0.0f,0.0f));
    pMission->SetContainerClamp   (false);
    pMission->SetContainerOverdraw(false);

    // 
    m_aiCounter[CONNECTED_MOUTH] = 1;
}


// ****************************************************************
// 
void cAmemasuBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cAmemasuBoss::__MoveOwn()
{
    cNevoMission* pMission   = d_cast<cNevoMission*>(g_pGame->GetCurMission());
    coreObject3D* pContainer = pMission->GetContainer();

    // ################################################################
    // ################################################################






    if(m_aiCounter[CONNECTED_MOUTH])
    {
        // 
        const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.2f*PI * m_fMouthAngle, coreVector3::Cross(this->GetDirection(), this->GetOrientation()).Normalized()).m123();
        const coreVector3 vDir1 = this->GetDirection() * mRota;
        const coreVector3 vDir2 = this->GetDirection() * mRota.Transposed();
        const coreVector3 vPos1 = this->GetPosition () + (vDir1 - this->GetDirection()) * (m_Top   .GetSize().x * 3.5f);
        const coreVector3 vPos2 = this->GetPosition () + (vDir2 - this->GetDirection()) * (m_Bottom.GetSize().x * 3.5f);

        // 
        m_Top.SetPosition   (vPos1);
        m_Top.SetDirection  (vDir1);
        m_Top.SetOrientation(this->GetOrientation());

        // 
        m_Bottom.SetPosition   (vPos2);
        m_Bottom.SetDirection  (vDir2);
        m_Bottom.SetOrientation(this->GetOrientation());
    }
}
