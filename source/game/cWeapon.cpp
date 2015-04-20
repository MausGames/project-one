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
coreBool cWeapon::Update(const coreBool& bShootStatus, const coreBool& bChangeStatus)
{
    ASSERT(m_pOwner)

    // always update the cooldown timer
    m_CooldownTimer.Update(1.0f);

    // call trigger or release event
    if(bShootStatus) {if(!m_bLastStatus) this->__TriggerOwn();}
                else {if( m_bLastStatus) this->__ReleaseOwn();}
    m_bLastStatus = bShootStatus;

    // call change event
    if(bChangeStatus) this->__ChangeOwn();

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
: m_iSpread (0u)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(12.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_ray.wav");
}


// ****************************************************************
// change the ray weapon
void cRayWeapon::__ChangeOwn()
{
    // toggle bullet spread
    if(++m_iSpread >= 4u) m_iSpread = 0u;
}


// ****************************************************************
// shoot with the ray weapon
void cRayWeapon::__ShootOwn()
{
    // 
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vTan = vDir.Rotated90();

    switch(m_iSpread)
    {

    // front or back
    case 0u:
    case 1u:
    {
        const coreVector2 vNewDir = m_iSpread ? -vDir : vDir;
        const coreVector2 vNewPos = vPos + vNewDir*6.0f;

        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(1, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vNewPos + vTan*1.1f, (vNewDir + vTan*0.025f*0.50f).Normalize());
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(1, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vNewPos - vTan*1.1f, (vNewDir - vTan*0.025f*0.50f).Normalize());
        //g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.96f, m_pOwner, TYPE_BULLET_PLAYER, vNewPos + vTan*2.2f - vDir*4.4f, (vNewDir + vTan*0.050f*0.75f).Normalize())->MakeSmaller(0.8f);
        //g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.96f, m_pOwner, TYPE_BULLET_PLAYER, vNewPos - vTan*2.2f - vDir*4.4f, (vNewDir - vTan*0.050f*0.75f).Normalize())->MakeSmaller(0.8f);
        //g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, 3.75f * 0.8f, m_pOwner, TYPE_BULLET_PLAYER, vNewPos + vTan*3.3f, (vNewDir + vTan*0.075f).Normalize())->MakeSmaller(0.8f);
        //g_pGame->GetBulletManager()->AddBullet<cRayBullet>(2, 3.75f * 0.8f, m_pOwner, TYPE_BULLET_PLAYER, vNewPos - vTan*3.3f, (vNewDir - vTan*0.075f).Normalize())->MakeSmaller(0.8f);
        break;
    }

    // sides
    case 2u:
    {
        const coreVector2 vUpPos   = vPos + vTan*6.0f;
        const coreVector2 vDownPos = vPos - vTan*6.0f;

        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vUpPos   + vDir*1.1f,  (vTan + vDir*0.025f).Normalize());
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vUpPos   - vDir*1.1f,  (vTan - vDir*0.025f).Normalize());
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vUpPos   + vDir*2.2f,  (vTan + vDir*0.050f).Normalize())->MakeSmaller(0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vUpPos   - vDir*2.2f,  (vTan - vDir*0.050f).Normalize())->MakeSmaller(0.9f);

        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vDownPos + vDir*1.1f, (-vTan + vDir*0.025f).Normalize());
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vDownPos - vDir*1.1f, (-vTan - vDir*0.025f).Normalize());
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vDownPos + vDir*2.2f, (-vTan + vDir*0.050f).Normalize())->MakeSmaller(0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vDownPos - vDir*2.2f, (-vTan - vDir*0.050f).Normalize())->MakeSmaller(0.9f);
        break;
    }

    // diagonal
    case 3u:
    {
        const coreVector2 vCorDir = vDir   .Rotated45();
        const coreVector2 vCorTan = vCorDir.Rotated90();

        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vPos + vCorDir*6.0f,                  vCorDir);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos + vCorDir*6.0f + vCorTan*1.1f,  (vCorDir + vCorTan*0.035f).Normalize())->MakeSmaller(0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos + vCorDir*6.0f - vCorTan*1.1f,  (vCorDir - vCorTan*0.035f).Normalize())->MakeSmaller(0.9f);

        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vPos - vCorDir*6.0f,                 -vCorDir);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos - vCorDir*6.0f + vCorTan*1.1f, (-vCorDir + vCorTan*0.035f).Normalize())->MakeSmaller(0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos - vCorDir*6.0f - vCorTan*1.1f, (-vCorDir - vCorTan*0.035f).Normalize())->MakeSmaller(0.9f);

        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vPos + vCorTan*6.0f,                  vCorTan);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos + vCorTan*6.0f + vCorDir*1.1f,  (vCorTan + vCorDir*0.035f).Normalize())->MakeSmaller(0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos + vCorTan*6.0f - vCorDir*1.1f,  (vCorTan - vCorDir*0.035f).Normalize())->MakeSmaller(0.9f);

        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(6, 3.75f,        m_pOwner, TYPE_BULLET_PLAYER, vPos - vCorTan*6.0f,                 -vCorTan);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos - vCorTan*6.0f + vCorDir*1.1f, (-vCorTan + vCorDir*0.035f).Normalize())->MakeSmaller(0.9f);
        g_pGame->GetBulletManager()->AddBullet<cRayBullet>(4, 3.75f * 0.9f, m_pOwner, TYPE_BULLET_PLAYER, vPos - vCorTan*6.0f - vCorDir*1.1f, (-vCorTan - vCorDir*0.035f).Normalize())->MakeSmaller(0.9f);
        break;
    }

    default:
        ASSERT(false)
        break;
    }

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.5f, 0.0f, false, m_pOwner->GetPosition());
}