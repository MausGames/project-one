///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// update the weapon
coreBool cWeapon::Update(const coreUint8 iShootStatus)
{
    ASSERT(m_pOwner)
    STATIC_ASSERT(WEAPON_MODES <= sizeof(iShootStatus)*8u)

    // call individual update routine
    this->__UpdateOwn(iShootStatus);

    // check firing-modes and call trigger or release routine
    for(coreUintW i = 0u; i < WEAPON_MODES; ++i)
    {
        if(HAS_BIT(iShootStatus, i))
             {if(!HAS_BIT(m_iLastStatus, i)) this->__TriggerOwn(i);}
        else {if( HAS_BIT(m_iLastStatus, i)) this->__ReleaseOwn(i);}
    }
    m_iLastStatus = iShootStatus;

    // always update the primary cooldown timer
    if(m_CooldownTimer.Update(1.0f))
    {
        if(HAS_BIT(iShootStatus, 0u))
             m_CooldownTimer.Pause();   // preserve fractional part
        else m_CooldownTimer.Stop ();   // clamp to zero
    }

    if(HAS_BIT(iShootStatus, 0u))
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
// 
void cWeapon::Render()
{
    // 
    this->__RenderOwn();
}


// ****************************************************************
// 
coreBool cWeapon::_IsOwnerDarkShading()const
{
    // 
    return true;//m_pOwner->IsDarkShading();
}


// ****************************************************************
// constructor
cRayWeapon::cRayWeapon()noexcept
: m_bBurst (false)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(20.0f);

    // load shooting sound-effects
    m_pBulletSound = Core::Manager::Resource->Get<coreSound>("bullet_ray.wav");
    m_pRocketSound = Core::Manager::Resource->Get<coreSound>("effect_launch.wav");
    m_pMineSound   = Core::Manager::Resource->Get<coreSound>("bullet_mine.wav");
}


// ****************************************************************
// 
void cRayWeapon::__TriggerOwn(const coreUint8 iMode)
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();

    if(iMode == 0u)
    {
        /*
        // 
        if(m_CooldownTimer.GetStatus()) return;

        // 
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(4, 6.0f, m_pOwner, vPos, vDir))->ChangeScale(1.3f);

        // 
        m_CooldownTimer.SetValue(-1.1f);
        m_CooldownTimer.Play(CORE_TIMER_PLAY_CURRENT);
        */
    }
    else if(iMode == 1u)
    {
        // 
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6, 1.0f, m_pOwner, vPos,  vDir.Rotated45());
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6, 1.0f, m_pOwner, vPos,  vDir.Rotated90());
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6, 1.0f, m_pOwner, vPos, -vDir.Rotated90());
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6, 1.0f, m_pOwner, vPos, -vDir.Rotated135());

        // 
        if(m_pRocketSound.IsUsable()) m_pRocketSound->PlayPosition(NULL, 1.0f, 1.0f, false, SOUND_EFFECT, m_pOwner->GetPosition());
        g_pSpecialEffects->CreateSplashColor(m_pOwner->GetPosition(), SPECIAL_SPLASH_TINY, coreVector3(0.5f,0.5f,0.5f));
    }
    else if(iMode == 2u)
    {
        // 
        for(coreUintW i = 1u; i < 4u; ++i)
        {
            const coreVector2 vDir2 = coreVector2::Direction(0.25f * PI * I_TO_F(i));

            g_pGame->GetBulletManagerPlayer()->AddBullet<cMineBullet>(6, 1.0f, m_pOwner, vPos,  vDir2);
            g_pGame->GetBulletManagerPlayer()->AddBullet<cMineBullet>(6, 1.0f, m_pOwner, vPos, -vDir2);
        }

        // 
        if(m_pMineSound.IsUsable()) m_pMineSound->PlayPosition(NULL, 1.0f, 1.0f, false, SOUND_EFFECT, m_pOwner->GetPosition());
    }
}


// ****************************************************************
// 
void cRayWeapon::__ReleaseOwn(const coreUint8 iMode)
{
    // 
    m_bBurst = false;
}


// ****************************************************************
// shoot with the ray weapon
void cRayWeapon::__ShootOwn()
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = vDir.Rotated90();

    // 
    if(!m_bBurst)
    {
        m_bBurst = true;

        // 
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(6, 8.0f, m_pOwner, vPos, vDir))->ChangeScale(1.2f)->ChangeTilt(m_pOwner->GetTilt());
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cSpearBullet>   (4, 0.0f, m_pOwner, vPos + coreVector2(0.0f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cConeBullet>    (4, 0.0f, m_pOwner, vPos + coreVector2(0.1f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cTriangleBullet>(4, 0.0f, m_pOwner, vPos + coreVector2(0.2f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cViewBullet>    (4, 0.0f, m_pOwner, vPos + coreVector2(0.3f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cFlipBullet>    (4, 0.0f, m_pOwner, vPos + coreVector2(0.4f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cOrbBullet>     (4, 0.0f, m_pOwner, vPos + coreVector2(0.5f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cQuadBullet>    (4, 0.0f, m_pOwner, vPos + coreVector2(0.6f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);
        //g_pGame->GetBulletManagerPlayer()->AddBullet<cWaveBullet>    (4, 0.0f, m_pOwner, vPos + coreVector2(0.7f,0.0f)*FOREGROUND_AREA, vDir)->ChangeSize(1.5f);

        // 
        m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 2.0f);
    }
    else
    {
        // 
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1, 8.0f, m_pOwner, vPos + vTan*1.5f, vDir))->ChangeTilt(m_pOwner->GetTilt());
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1, 8.0f, m_pOwner, vPos - vTan*1.5f, vDir))->ChangeTilt(m_pOwner->GetTilt());
        //this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 6.0f, m_pOwner, vPos, vDir))->ChangeTilt(m_pOwner->GetTilt());
    }

    // play bullet sound-effect
    if(m_pBulletSound.IsUsable()) m_pBulletSound->PlayPosition(NULL, 1.0f, 0.5f, false, SOUND_EFFECT, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cPulseWeapon::cPulseWeapon()noexcept
: m_fCharge (0.0f)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(6.0f);

    // load shooting sound-effects
    m_pBulletSound = Core::Manager::Resource->Get<coreSound>("bullet_pulse.wav");
}


// ****************************************************************
// 
void cPulseWeapon::__UpdateOwn(const coreUint8 iShootStatus)
{
    // 
    if(!m_CooldownTimer.GetStatus()) m_fCharge.UpdateMin(m_CooldownTimer.GetSpeed(), 2.0f);
}


// ****************************************************************
// shoot with the pulse weapon
void cPulseWeapon::__ShootOwn()
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();

    // 
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(5 + 10 * F_TO_SI(m_fCharge), 3.0f, m_pOwner, vPos, vDir))->ChangeScale(1.2f + 0.4f * FLOOR(m_fCharge))->AddStatus(BULLET_STATUS_PENETRATE);

    // 
    m_fCharge = 0.0f;

    // play bullet sound-effect
    if(m_pBulletSound.IsUsable()) m_pBulletSound->PlayPosition(NULL, 1.0f, 0.7f, false, SOUND_EFFECT, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cWaveWeapon::cWaveWeapon()noexcept
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(10.0f);

    // load shooting sound-effects
    m_pBulletSound = Core::Manager::Resource->Get<coreSound>("bullet_wave.wav");
}


// ****************************************************************
// shoot with the wave weapon
void cWaveWeapon::__ShootOwn()
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();

    // 
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cSurgeBullet>(2, 3.5f, m_pOwner, vPos,  vDir))->ChangeScale(1.8f);
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cSurgeBullet>(2, 3.5f, m_pOwner, vPos, -vDir))->ChangeScale(1.8f);

    // play bullet sound-effect
    if(m_pBulletSound.IsUsable()) m_pBulletSound->PlayPosition(NULL, 1.0f, 1.0f, false, SOUND_EFFECT, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cTeslaWeapon::cTeslaWeapon()noexcept
: m_iShotType     (0)
, m_iStrikeType   (0)
, m_fStrikeOffset (0.0f)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(4.0f);

    // load shooting sound-effects
    m_pBulletSound = Core::Manager::Resource->Get<coreSound>("bullet_tesla.wav");
}


// ****************************************************************
// 
void cTeslaWeapon::__TriggerOwn(const coreUint8 iMode)
{
    if(iMode != 1u) return;

    const coreObject3D* apStrikeEmitter[WEAPON_TESLA_TARGETS];
    cEnemy*             apStrikeTarget [WEAPON_TESLA_TARGETS];
    coreUintW           iNum = 0u;

    coreList<const coreObject3D*> apNode;

    // 
    apNode.reserve(16u);
    g_pGame->GetBulletManagerPlayer()->ForEachBulletTyped<cTeslaBullet>([&](const cTeslaBullet* pBullet)
    {
        if(pBullet->GetOwner() == m_pOwner)
            apNode.push_back(pBullet);
    });

    // 
    apNode.push_back(m_pOwner);
    std::reverse(apNode.begin(), apNode.end());

    // 
    FOR_EACH(it, apNode)
    {
        const coreObject3D* pObject = (*it);

        // 
        g_pGame->GetEnemyManager()->ForEachEnemy([&](cEnemy* OUTPUT pEnemy)
        {
            // 
            if(iNum == WEAPON_TESLA_TARGETS)
                return;

            // 
            for(coreUintW i = 0u; i < iNum; ++i)
            {
                if(pEnemy == apStrikeTarget[i])
                    return;
            }

            // 
            if((pEnemy->GetPosition().xy() - pObject->GetPosition().xy()).LengthSq() < POW2(40.0f))
            {
                apStrikeEmitter[iNum] = pObject;
                apStrikeTarget [iNum] = pEnemy;
                ++iNum;
            }
        });

        // 
        if(iNum == WEAPON_TESLA_TARGETS)
            break;
    }

    // 
    if(iNum)
    {
        const coreObject3D* pLastEmitter = apStrikeEmitter[iNum-1u];

        // 
        if(++m_iStrikeType >= 4u) m_iStrikeType = 0u;
        const coreVector2 vTexSizeFactor = coreVector2((m_iStrikeType & 0x02u) ? -1.0f : 1.0f,
                                                       (m_iStrikeType & 0x01u) ? -1.0f : 1.0f);

        // 
        if(pLastEmitter != m_pOwner)
        {
            // 
            for(coreUintW i = 0u, ie = (apNode.size() - 1u); i < ie; ++i)
            {
                const coreObject3D* pEmitter = apNode[i];
                const coreObject3D* pTarget  = apNode[i+1u];

                // 
                const coreVector2 vPosFrom = pEmitter->GetPosition().xy();
                const coreVector2 vPosTo   = pTarget ->GetPosition().xy();
                m_fStrikeOffset = g_pSpecialEffects->CreateLightning(vPosFrom, vPosTo, SPECIAL_LIGHTNING_BIG, coreVector3(1.0f,1.0f,1.0f), vTexSizeFactor, m_fStrikeOffset);

                // 
                if(pLastEmitter == pTarget) break;
            }
        }

        // 
        for(coreUintW i = 0u; i < iNum; ++i)
        {
            const coreObject3D* pEmitter = apStrikeEmitter[i];
            cEnemy*             pTarget  = apStrikeTarget [i];

            // 
            const coreVector2 vPosFrom = pEmitter->GetPosition().xy();
            const coreVector2 vPosTo   = pTarget ->GetPosition().xy();
            m_fStrikeOffset = g_pSpecialEffects->CreateLightning(vPosFrom, vPosTo, SPECIAL_LIGHTNING_BIG, coreVector3(1.0f,1.0f,1.0f), vTexSizeFactor, m_fStrikeOffset);

            // 
            pTarget->TakeDamage(6, ELEMENT_BLUE, vPosTo, m_pOwner);
            g_pSpecialEffects->MacroExplosionColorSmall(pTarget->GetPosition(), COLOR_ENERGY_BLUE);
        }
    }

    // 
    g_pSpecialEffects->CreateSplashColor(m_pOwner->GetPosition(), 5.0f, 3u, COLOR_ENERGY_BLUE);
}


// ****************************************************************
// shoot with the tesla weapon
void cTeslaWeapon::__ShootOwn()
{
    // 
    if(++m_iShotType >= 4) m_iShotType = 0;

    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = coreVector2((m_iShotType < 3) ? (0.04f * I_TO_F(m_iShotType-1)) : 0.0f, 0.0f);

    // 
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cTeslaBullet>(6, 3.0f, m_pOwner, vPos, (vDir + vTan).Normalized()));

    // play bullet sound-effect
    if(m_pBulletSound.IsUsable()) m_pBulletSound->PlayPosition(NULL, 1.0f, 1.0f, false, SOUND_EFFECT, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cEnemyWeapon::cEnemyWeapon()noexcept
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(10.0f);
}


// ****************************************************************
// shoot with the enemy weapon
void cEnemyWeapon::__ShootOwn()
{
    // 
    const coreVector2 vPos  = m_pOwner->GetPosition().xy();
    const coreFloat   fBase = m_pOwner->GetDirection().xy().Angle();

    // 
    for(coreUintW j = 5u; j--; )
    {
        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cViewBullet>(1, 2.4f, m_pOwner, vPos, vDir))->ChangeSize(1.5f);
    }
}