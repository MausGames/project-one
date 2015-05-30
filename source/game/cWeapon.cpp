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
    if(m_CooldownTimer.Update(1.0f))
    {
        if(bShootStatus) m_CooldownTimer.Pause();
                    else m_CooldownTimer.Stop();
    }

    // 
    this->__UpdateOwn(bShootStatus);

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
            m_CooldownTimer.Play(CORE_TIMER_PLAY_CURRENT);
            this->__ShootOwn();

            return true;
        }
    }

    return false;
}


// ****************************************************************
// constructor
cRayWeapon::cRayWeapon()noexcept
: m_bSide (false)
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
: m_fCharge (0.0f)
, m_bSide   (false)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(3.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_pulse.wav");
}


// ****************************************************************
// 
void cPulseWeapon::__UpdateOwn(const coreBool& bShootStatus)
{
    // 
    if(!bShootStatus) m_fCharge = MIN(m_fCharge + m_CooldownTimer.GetSpeed() * Core::System->GetTime(), 3.0f);
}


// ****************************************************************
// shoot with the pulse weapon
void cPulseWeapon::__ShootOwn()
{
    const coreFloat fAlpha = g_CurConfig.Game.iTransparent ? 0.5f : 1.0f;

    // 
    const coreBool bDark = CONTAINS_VALUE(m_pOwner->GetStatus(), PLAYER_STATUS_DARKNESS);
    m_CooldownTimer.SetSpeed(bDark ? 6.0f : 3.0f);

    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = vDir.Rotated90(); // coreVector2(bDark ? (((m_bSide = !m_bSide) == true) ? 1.0f : -1.0f) : 0.0f, 0.0f);

    // 
    switch(F_TO_UI(m_fCharge))
    {
    case 3:
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*4.0f + vTan*2.6f, (vDir + vTan*0.025f*0.8f).Normalize())->MakeSmaller(1.15f)->MakeLighter(fAlpha);
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.6f, m_pOwner, vPos + vDir*6.0f,              vDir)                                ->MakeSmaller(1.35f)->MakeLighter(fAlpha);
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*4.0f - vTan*2.6f, (vDir - vTan*0.025f*0.8f).Normalize())->MakeSmaller(1.15f)->MakeLighter(fAlpha);
        break;

    case 2:
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*6.0f + vTan*1.3f, (vDir + vTan*0.025f*0.5f).Normalize())->MakeSmaller(1.25f)->MakeLighter(fAlpha);
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*6.0f - vTan*1.3f, (vDir - vTan*0.025f*0.5f).Normalize())->MakeSmaller(1.25f)->MakeLighter(fAlpha);
        break;

    case 1:
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*6.0f, vDir)->MakeSmaller(1.25f)->MakeLighter(fAlpha);
        break;

    default:
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*6.0f, vDir)->MakeLighter(fAlpha);
        break;
    }
    // g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 2.0f, m_pOwner, vPos + vDir*6.0f + vTan*1.5f, (vDir + vTan*0.025f*0.5f).Normalize())->MakeLighter(fAlpha);

    // 
    m_fCharge = 0.0f;

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.7f, 0.0f, false, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cWaveWeapon::cWaveWeapon()noexcept
: m_bSide (false)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(10.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_ray.wav");
}


// ****************************************************************
// 
void cWaveWeapon::__ReleaseOwn()
{
    // 
    m_bSide = !m_bSide;
}


// ****************************************************************
// shoot with the wave weapon
void cWaveWeapon::__ShootOwn()
{
    const coreFloat fAlpha = g_CurConfig.Game.iTransparent ? 0.5f : 1.0f;

    // 
    const coreVector2 vPos = m_pOwner->GetPosition().xy();
    const coreVector2 vDir = m_bSide ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,1.0f);

    // 
    g_pGame->GetBulletManagerPlayer()->AddBullet<cWaveBullet>(4, 3.5f, m_pOwner, vPos + vDir*6.0f,  vDir)->MakeSmaller(1.8f)->MakeLighter(fAlpha);
    g_pGame->GetBulletManagerPlayer()->AddBullet<cWaveBullet>(4, 3.5f, m_pOwner, vPos - vDir*6.0f, -vDir)->MakeSmaller(1.8f)->MakeLighter(fAlpha);

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.5f, 0.0f, false, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cTeslaWeapon::cTeslaWeapon()noexcept
: m_iSide (0)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(6.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_tesla.wav");
}


// ****************************************************************
// shoot with the tesla weapon
void cTeslaWeapon::__ShootOwn()
{
    const coreFloat fAlpha = g_CurConfig.Game.iTransparent ? 0.5f : 1.0f;

    // 
    if(++m_iSide > 3) m_iSide = 0;

    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = coreVector2((m_iSide < 3) ? (0.04f * I_TO_F(m_iSide-1)) : 0.0f, 0.0f);

    // 
    g_pGame->GetBulletManagerPlayer()->AddBullet<cTeslaBullet>(2, 3.0f, m_pOwner, vPos + vDir*6.0f, (vDir + vTan).Normalize())->MakeLighter(fAlpha);

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 1.0f, 0.0f, false, m_pOwner->GetPosition());
}