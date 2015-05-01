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
coreBool cWeapon::Update(const coreBool& bShootStatus)
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
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(12.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_ray.wav");
}


// ****************************************************************
// shoot with the ray weapon
void cRayWeapon::__ShootOwn()
{
    const coreFloat fAlpha = g_CurConfig.Game.iTransparent ? 0.5f : 1.0f;

    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = vDir.Rotated90();

    // 
    g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 3.75f, m_pOwner, vPos + vDir*6.0f + vTan*1.1f, (vDir + vTan*0.025f*0.5f).Normalize())->MakeLighter(fAlpha);
    g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 3.75f, m_pOwner, vPos + vDir*6.0f - vTan*1.1f, (vDir - vTan*0.025f*0.5f).Normalize())->MakeLighter(fAlpha);

    // 
    if(CONTAINS_VALUE(m_pOwner->GetStatus(), PLAYER_STATUS_DARKNESS))
    {
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1, 3.75f, m_pOwner, vPos + vDir*4.0f + vTan*3.3f, (vDir + vTan*0.050f*3.0f).Normalize())->MakeSmaller(0.85f)->MakeLighter(fAlpha*0.5f);
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1, 3.75f, m_pOwner, vPos + vDir*4.0f - vTan*3.3f, (vDir - vTan*0.050f*3.0f).Normalize())->MakeSmaller(0.85f)->MakeLighter(fAlpha*0.5f);
    }

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.5f, 0.0f, false, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cPulseWeapon::cPulseWeapon()noexcept
: m_iCharge (0u)
, m_bSide   (false)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(4.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_pulse.wav");
}


// ****************************************************************
// shoot with the pulse weapon
void cPulseWeapon::__ShootOwn()
{
    const coreFloat fAlpha = g_CurConfig.Game.iTransparent ? 0.5f : 1.0f;

    // 
    const coreBool bDark = CONTAINS_VALUE(m_pOwner->GetStatus(), PLAYER_STATUS_DARKNESS);
    m_CooldownTimer.SetSpeed(bDark ? 6.0f : 4.0f);

    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = coreVector2(bDark ? (((m_bSide = !m_bSide) == true) ? 1.0f : -1.0f) : 0.0f, 0.0f);

    // 
    g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(12, 2.0f, m_pOwner, vPos + vDir*6.0f + vTan*1.5f, (vDir + vTan*0.025f*0.5f).Normalize())->MakePurple()->MakeSmaller(1.3f)->MakeLighter(fAlpha);

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.7f, 0.0f, false, m_pOwner->GetPosition());
}