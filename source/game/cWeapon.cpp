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
// 
bool cWeapon::Update(const bool& bShootStatus)
{
    ASSERT(m_pOwner)

    // 
    m_CooldownTimer.Update(1.0f);

    // 
    if(bShootStatus) {if(!m_bLastStatus) this->__TriggerOwn();}
                else {if( m_bLastStatus) this->__ReleaseOwn();}
    m_bLastStatus = bShootStatus;

    if(bShootStatus)
    {
        // 
        if(!m_CooldownTimer.GetStatus())
        {
            // 
            m_CooldownTimer.Play(CORE_TIMER_PLAY_RESET);
            this->__ShootOwn();

            return true;
        }
    }

    return false;
}


// ****************************************************************
// constructor
cRayWeapon::cRayWeapon()noexcept
: m_bSpread (false)
{
    // 
    m_CooldownTimer.SetSpeed(6.0f);

    // 
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_ray.wav");
}


// ****************************************************************
// 
void cRayWeapon::__ReleaseOwn()
{
    // 
    m_bSpread = !m_bSpread;
}


// ****************************************************************
// 
void cRayWeapon::__ShootOwn()
{
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = vDir.yx();





    if(m_bSpread)
    {
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*1.1f, (vDir + vTan*0.5f*0.85f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*1.1f, (vDir - vTan*0.5f*0.85f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*2.2f, (vDir + vTan*0.5f*1.00f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*2.2f, (vDir - vTan*0.5f*1.00f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*3.3f, (vDir + vTan*0.5f*1.15f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*3.3f, (vDir - vTan*0.5f*1.15f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
    }
    else
    {
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*1.1f, (vDir + vTan*0.5f*0.05f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*1.1f, (vDir - vTan*0.5f*0.05f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*2.2f, (vDir + vTan*0.5f*0.10f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*2.2f, (vDir - vTan*0.5f*0.10f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*3.3f, (vDir + vTan*0.5f*0.15f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*3.3f, (vDir - vTan*0.5f*0.15f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
    }




    // 
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.5f, 0.0f, false, m_pOwner->GetPosition());
}