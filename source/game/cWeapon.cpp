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
coreBool cWeapon::Update(const coreUint8& iShootStatus)
{
    ASSERT(m_pOwner)
    STATIC_ASSERT(WEAPON_MODES <= sizeof(iShootStatus)*8u)

    // call individual update routine
    this->__UpdateOwn(iShootStatus);

    // check firing-modes and call trigger or release routine
    for(coreUintW i = 0u; i < WEAPON_MODES; ++i)
    {
        if(CONTAINS_BIT(iShootStatus, i))
             {if(!CONTAINS_BIT(m_iLastStatus, i)) this->__TriggerOwn(i);}
        else {if( CONTAINS_BIT(m_iLastStatus, i)) this->__ReleaseOwn(i);}
    }
    m_iLastStatus = iShootStatus;

    // always update the primary cooldown timer
    if(m_CooldownTimer.Update(1.0f))
    {
        if(CONTAINS_BIT(iShootStatus, 0u))
             m_CooldownTimer.Pause();   // preserve fractional part
        else m_CooldownTimer.Stop ();   // clamp to zero
    }

    if(CONTAINS_BIT(iShootStatus, 0u))
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
void cWeapon::RenderOverlay()
{
    // 
    this->__RenderOverlayOwn();
}


// ****************************************************************
// constructor
cRayWeapon::cRayWeapon()noexcept
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(14.0f);

    // load shooting sound-effect
    m_pShootSound  = Core::Manager::Resource->Get<coreSound>("bullet_ray.wav");
    m_pRocketSound = Core::Manager::Resource->Get<coreSound>("effect_launch.wav");
    m_pMineSound   = Core::Manager::Resource->Get<coreSound>("bullet_mine.wav");
}


// ****************************************************************
// 
void cRayWeapon::__TriggerOwn(const coreUint8& iMode)
{
    /*
    if(iMode == 0u)
    {
        //// 
        //const coreVector2 vPos = m_pOwner->GetPosition ().xy();
        //const coreVector2 vDir = m_pOwner->GetDirection().xy();
        //const coreVector2 vTan = vDir.Rotated90();
        //
        //
        //const coreFloat fAngle = vDir.Angle();
        //for(coreUintW i = 0u; i < 5u; ++i)
        //{
        //    const coreVector2 vDir2 = coreVector2::Direction(fAngle + 0.2f * I_TO_F(i - 2u));
        //    if(i != 2) g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 3.75f, m_pOwner, vPos, vDir2);
        //}
    }
    else if(iMode == 1u)
    {
        // 
        const coreVector2 vPos = m_pOwner->GetPosition().xy();

        //for(coreUintW i = 1u; i < 4u; ++i)
        //{
        //    // 
        //    const coreVector2 vDir = coreVector2::Direction(0.25f * PI * I_TO_F(i));
        //    g_pGame->GetBulletManagerPlayer()->AddBullet<cMineBullet>(12, 1.0f, m_pOwner, vPos,  vDir)->SetFlyDir( vDir);
        //    g_pGame->GetBulletManagerPlayer()->AddBullet<cMineBullet>(12, 1.0f, m_pOwner, vPos, -vDir)->SetFlyDir(-vDir);
        //}
        //
        //// 
        //m_pMineSound->PlayPosition(NULL, 1.0f, 1.0f, 0.0f, false, m_pOwner->GetPosition());

        m_pRocketSound->PlayPosition(NULL, 1.0f, 1.0f, 0.0f, false, m_pOwner->GetPosition());

        const coreVector2 vDir = m_pOwner->GetDirection().xy();
        g_pSpecialEffects->CreateSplashColor(m_pOwner->GetPosition(), SPECIAL_SPLASH_TINY, coreVector3(0.5f,0.5f,0.5f));

        static coreBool bSide = false;
        bSide = !bSide;

        const coreVector2 vTrueDir = bSide ? vDir.Rotated45() : -vDir.Rotated135();

        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(12, 1.0f, m_pOwner, vPos + vTrueDir*4.0f, vTrueDir);
    }
    */
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
    //g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 3.75f, m_pOwner, vPos + vDir*6.0f + vTan*1.1f, (vDir + vTan*0.025f*0.5f).Normalize())->MakeLighter(fAlpha);
    //g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 3.75f, m_pOwner, vPos + vDir*6.0f - vTan*1.1f, (vDir - vTan*0.025f*0.5f).Normalize())->MakeLighter(fAlpha);
    g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 5.0f, m_pOwner, vPos + vDir*6.0f + vTan*1.4f, vDir)->MakeLighter(fAlpha);
    g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2, 5.0f, m_pOwner, vPos + vDir*6.0f - vTan*1.4f, vDir)->MakeLighter(fAlpha);

    // 
    //if(CONTAINS_VALUE(m_pOwner->GetStatus(), PLAYER_STATUS_DARKNESS))
    //{
    //    g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1, 3.75f, m_pOwner, vPos + vDir*4.0f + vTan*3.3f, (vDir + vTan*0.050f*3.0f).Normalize())->MakeSmaller(0.85f)->MakeLighter(fAlpha);
    //    g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1, 3.75f, m_pOwner, vPos + vDir*4.0f - vTan*3.3f, (vDir - vTan*0.050f*3.0f).Normalize())->MakeSmaller(0.85f)->MakeLighter(fAlpha);
    //}

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 0.5f, 0.0f, false, m_pOwner->GetPosition());
}


// ****************************************************************
// constructor
cPulseWeapon::cPulseWeapon()noexcept
: m_fCharge (0.0f)
, m_iCombo  (0u)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(3.0f);

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_pulse.wav");
}


// ****************************************************************
// 
void cPulseWeapon::__UpdateOwn(const coreUint8& iShootStatus)
{
    // 
    if(!CONTAINS_BIT(iShootStatus, 0u)) m_fCharge = MIN(m_fCharge + m_CooldownTimer.GetSpeed() * Core::System->GetTime(), 3.0f);
}


// ****************************************************************
// shoot with the pulse weapon
void cPulseWeapon::__ShootOwn()
{
    const coreFloat fAlpha = g_CurConfig.Game.iTransparent ? 0.5f : 1.0f;

    // 
    m_CooldownTimer.SetSpeed(CONTAINS_VALUE(m_pOwner->GetStatus(), PLAYER_STATUS_DARKNESS) ? 4.5f : 3.0f);

    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = vDir.Rotated90();

    // 
    switch(F_TO_UI(m_fCharge))
    {
    case 3:
        //break;

    case 2:
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*4.0f + vTan*2.6f, (vDir + vTan*0.025f*0.8f).Normalize())->MakeSmaller(1.15f)->MakeLighter(fAlpha);
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.6f, m_pOwner, vPos + vDir*6.0f,              vDir)                                ->MakeSmaller(1.35f)->MakeLighter(fAlpha);
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*4.0f - vTan*2.6f, (vDir - vTan*0.025f*0.8f).Normalize())->MakeSmaller(1.15f)->MakeLighter(fAlpha);
        break;

    case 1:
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*6.0f + vTan*1.1f/*1.3f*/, (vDir + vTan*0.025f*2.0f/*0.5f*/).Normalize())->MakeSmaller(1.25f)->MakeLighter(fAlpha);
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 1.5f, m_pOwner, vPos + vDir*6.0f - vTan*1.1f/*1.3f*/, (vDir - vTan*0.025f*2.0f/*0.5f*/).Normalize())->MakeSmaller(1.25f)->MakeLighter(fAlpha);
        break;

    default:
        g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>(12, 0.5f, m_pOwner, vPos + vDir*6.0f, vDir)->MakeLighter(fAlpha);
        break;
    }

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
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_wave.wav");


    // 
    m_Arrow.Construct(MENU_ICON_MEDIUM_2, MENU_OUTLINE_SMALL, 0u);
    m_Arrow.SetColor4(coreVector4(COLOR_ENERGY_GREEN, 0.8f));
    m_Arrow.SetText  (ICON_CARET_UP);
}


// ****************************************************************
// 
void cWaveWeapon::__UpdateOwn(const coreUint8& iShootStatus)
{
    if(CONTAINS_BIT(iShootStatus, 1u)) 
    {
        static coreUint8 iTest = 0u;
        if(++iTest >= 6u)
        {
            iTest = 0u;

            m_bSide = true;
            this->__ShootOwn();
            m_bSide = false;
        }
    }
}


// ****************************************************************
// 
void cWaveWeapon::__TriggerOwn(const coreUint8& iMode)
{
    // 
    //if(iMode == 1u) m_bSide = !m_bSide;
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
    m_pShootSound->PlayPosition(NULL, 1.0f, 1.0f, 0.0f, false, m_pOwner->GetPosition());
}


// ****************************************************************
// 
void cWaveWeapon::__RenderOverlayOwn()
{
    const coreVector2 vDir = m_bSide ? coreVector2(1.0f,0.0f) : coreVector2(0.0f,1.0f);

    m_Arrow.SetCenter(g_pForeground->Project(m_pOwner->GetPosition()) * g_vMenuCenter);

    m_Arrow.SetPosition (vDir * 0.055f);
    m_Arrow.SetDirection(vDir.InvertedX());
    m_Arrow.Move();
    m_Arrow.Render();

    m_Arrow.SetPosition (-vDir * 0.055f);
    m_Arrow.SetDirection(-vDir.InvertedX());
    m_Arrow.Move();
    m_Arrow.Render();
}


// ****************************************************************
// constructor
cTeslaWeapon::cTeslaWeapon()noexcept
: m_iShotType   (0)
, m_iStrikeType (0)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(4.0f); // 6.0f

    // load shooting sound-effect
    m_pShootSound = Core::Manager::Resource->Get<coreSound>("bullet_tesla.wav");
}


// ****************************************************************
// 
void cTeslaWeapon::__TriggerOwn(const coreUint8& iMode)
{
    if(iMode != 1u) return;





    std::vector<coreObject3D*> pList;
    pList.reserve(16u);

    g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
    {
        if((pBullet->GetID() == cTeslaBullet::ID) && (pBullet->GetOwner() == m_pOwner))
            pList.push_back(pBullet);
    });

    //pList.push_back(m_pOwner);
    std::reverse(pList.begin(), pList.end());



    
    coreObject3D* aapStrikePair[3][2] = {NULL};   // # {target (enemy), emitter (player/bullet)}
    const cBullet*      pLastBullet         =  NULL;

    coreUint8 iNum = 0u;


    static coreFloat fCurOffset = 0.0f;  


    // 
    if(++m_iStrikeType > 3u) m_iStrikeType = 0u;


    // 
    auto nBulletLightningFunc = [&]()
    {
        

        const coreVector2 vTexOffset = coreVector2((m_iStrikeType & 0x02u) ? -1.0f : 1.0f, (m_iStrikeType & 0x01u) ? -1.0f : 1.0f);

        const coreObject3D* pCurAnchor = m_pOwner;//pList.front();

        FOR_EACH(it, pList)
        {
            const coreObject3D* pObject = (*it);

            const coreVector2 vPosFrom = pCurAnchor->GetPosition().xy();
            const coreVector2 vPosTo   = pObject   ->GetPosition().xy();

            fCurOffset = g_pSpecialEffects->CreateLightning(vPosFrom, vPosTo, SPECIAL_LIGHTNING_BIG, coreVector3(1.0f,1.0f,1.0f), vTexOffset, fCurOffset);

            if(pObject == pLastBullet) break;

            pCurAnchor = pObject;
        }
    };


    auto nEnemyLightningFunc = [&](coreObject3D* OUTPUT pTarget, const coreObject3D* pEmitter)
    {
        // 


        const coreVector2 vTexOffset = coreVector2((m_iStrikeType & 0x02u) ? -1.0f : 1.0f, (m_iStrikeType & 0x01u) ? -1.0f : 1.0f);

        const coreVector2 vPosFrom = pEmitter->GetPosition().xy();
        const coreVector2 vPosTo   = pTarget ->GetPosition().xy();

        fCurOffset = g_pSpecialEffects->CreateLightning(vPosFrom, vPosTo, SPECIAL_LIGHTNING_BIG, coreVector3(1.0f,1.0f,1.0f), vTexOffset, fCurOffset);

        //if(++m_iStrikeType > 3u) m_iStrikeType = 0u;
        g_pSpecialEffects->MacroExplosionColorSmall(pTarget->GetPosition(), COLOR_ENERGY_BLUE);
        //g_pDistortion->CreateWave       (pTarget->GetPosition(), DISTORTION_WAVE_TINY);
        //g_pSpecialEffects->CreateSplashColor(pTarget->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_BLUE);
        //g_pSpecialEffects         ->ShakeScreen      (SPECIAL_SHAKE_SMALL);

        
        s_cast<cEnemy*>(pTarget)->TakeDamage(12, m_pOwner);
    };


    g_pGame->GetEnemyManager()->ForEachEnemy([&](cEnemy* OUTPUT pEnemy)
    {
        if(iNum == ARRAY_SIZE(aapStrikePair))
            return;

        if((pEnemy->GetPosition().xy() - m_pOwner->GetPosition().xy()).LengthSq() < 1600.0f)
        {

            aapStrikePair[iNum][0] = pEnemy;
            aapStrikePair[iNum][1] = m_pOwner;
            ++iNum;
        }
    });


    FOR_EACH(it, pList)
    {
        coreObject3D* pObject = (*it);

        g_pGame->GetEnemyManager()->ForEachEnemy([&](cEnemy* OUTPUT pEnemy)
        {
            if(iNum == ARRAY_SIZE(aapStrikePair))
                return;

            for(coreUintW i = 0u; i < ARRAY_SIZE(aapStrikePair); ++i)
            {
                if(pEnemy == aapStrikePair[i][0])
                    return;
            }

            if((pEnemy->GetPosition().xy() - pObject->GetPosition().xy()).LengthSq() < 1600.0f)
            {

                aapStrikePair[iNum][0] = pEnemy;
                aapStrikePair[iNum][1] = pObject;
                ++iNum;

                pLastBullet = s_cast<cBullet*>(pObject);
            }
        });
    }


    if(pLastBullet) nBulletLightningFunc();
    for(coreUintW i = 0u; i < iNum; ++i)
        nEnemyLightningFunc(aapStrikePair[i][0], aapStrikePair[i][1]);

    //if(m_iLastStatus & 0x01u)  // TODO # 1 frame behind  
        g_pSpecialEffects->CreateSplashColor(m_pOwner->GetPosition(), 5.0f, 3u, COLOR_ENERGY_BLUE);
    //else
    //{
    //    g_pSpecialEffects->MacroExplosionColorSmall(m_pOwner->GetPosition(), COLOR_ENERGY_BLUE);
    //
    //    const coreVector2 vTexOffset = coreVector2((m_iStrikeType & 0x02u) ? -1.0f : 1.0f, (m_iStrikeType & 0x01u) ? -1.0f : 1.0f);
    //
    //    const coreVector2 vPos = m_pOwner->GetPosition().xy();
    //    const coreVector2 vDir = coreVector2::Rand();
    //
    //    fCurOffset = g_pSpecialEffects->CreateLightning(vPos, vPos + 100.0f * vDir, SPECIAL_LIGHTNING_BIG, coreVector3(1.0f,1.0f,1.0f), vTexOffset, fCurOffset);
    //    fCurOffset = g_pSpecialEffects->CreateLightning(vPos, vPos - 100.0f * vDir, SPECIAL_LIGHTNING_BIG, coreVector3(1.0f,1.0f,1.0f), vTexOffset, fCurOffset);
    //}
}


// ****************************************************************
// shoot with the tesla weapon
void cTeslaWeapon::__ShootOwn()
{
    const coreFloat fAlpha = g_CurConfig.Game.iTransparent ? 0.5f : 1.0f;

    // 
    if(++m_iShotType > 3) m_iShotType = 0;

    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = coreVector2((m_iShotType < 3) ? (0.04f * I_TO_F(m_iShotType-1)) : 0.0f, 0.0f);

    // 
    g_pGame->GetBulletManagerPlayer()->AddBullet<cTeslaBullet>(12, 3.0f, m_pOwner, vPos + vDir*6.0f, (vDir + vTan).Normalize())->MakeLighter(fAlpha);

    // play shooting sound-effect
    m_pShootSound->PlayPosition(NULL, 1.0f, 1.0f, 0.0f, false, m_pOwner->GetPosition());
}