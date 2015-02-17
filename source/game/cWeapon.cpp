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
// update the weapon
bool cWeapon::Update(const bool& bShootStatus)
{
    ASSERT(m_pOwner)

    // always update the cooldown timer
    m_CooldownTimer.Update(1.0f);

    // call trigger or release event
    if(bShootStatus) {if(!m_bLastStatus) this->__TriggerOwn();}
                else {if( m_bLastStatus) this->__ReleaseOwn();}
    m_bLastStatus = bShootStatus;

    if(bShootStatus)
    {
        // check current cooldown status
        if(!m_CooldownTimer.GetStatus())
        {
            // start cooldown and shoot
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
    // set base fire-rate
    m_CooldownTimer.SetSpeed(6.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_ray.wav");
}


// ****************************************************************
// release the ray weapon
void cRayWeapon::__ReleaseOwn()
{
    // toggle bullet spread
    m_bSpread = !m_bSpread;
}


// ****************************************************************
// shoot with the ray weapon
void cRayWeapon::__ShootOwn()
{
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vPos = m_pOwner->GetPosition ().xy() + vDir*3.0f;
    const coreVector2 vTan = vDir.yx();

    if(m_bSpread)
    {
        // shoot wide
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*1.1f, (vDir + vTan*0.425f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*1.1f, (vDir - vTan*0.425f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*2.2f, (vDir + vTan*0.500f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*2.2f, (vDir - vTan*0.500f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*3.3f, (vDir + vTan*0.575f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*3.3f, (vDir - vTan*0.575f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
    }
    else
    {
        // shoot concentrated
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*1.1f, (vDir + vTan*0.025f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*1.1f, (vDir - vTan*0.025f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f));
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*2.2f, (vDir + vTan*0.050f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*2.2f, (vDir - vTan*0.050f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos + vTan*3.3f, (vDir + vTan*0.075f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, m_pOwner, TYPE_BULLET_PLAYER, vPos - vTan*3.3f, (vDir - vTan*0.075f).Normalize())->SetSize(coreVector3(2.5f,2.5f,2.5f)*0.8f);
    }

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.5f, 0.0f, false, m_pOwner->GetPosition());
}