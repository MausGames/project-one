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
coreBool cWeapon::Update(const coreUint8 iShootStatus, const coreFloat fShootSpeed)
{
    ASSERT(m_pOwner)
    STATIC_ASSERT(WEAPON_MODES <= sizeof(iShootStatus)*8u)

    // call individual update routine
    this->__UpdateOwn(iShootStatus, fShootSpeed);

    // check firing-modes and call trigger or release routine
    for(coreUintW i = 0u; i < WEAPON_MODES; ++i)
    {
        if(HAS_BIT(iShootStatus, i))
             {if(!HAS_BIT(m_iLastStatus, i)) this->__TriggerOwn(i);}
        else {if( HAS_BIT(m_iLastStatus, i)) this->__ReleaseOwn(i);}
    }
    m_iLastStatus = iShootStatus;

    // always update the primary cooldown timer
    if(m_CooldownTimer.Update(fShootSpeed))
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
void cWeapon::Move()
{
    // 
    this->__MoveOwn();
}


// ****************************************************************
// 
coreBool cWeapon::_IsOwnerDarkShading()const
{
    // 
    return true;//(g_pGame->GetCurMission()->GetID() != cIntroMission::ID);//m_pOwner->IsDarkShading();
}


// ****************************************************************
// 
coreBool cWeapon::_IsOwnerRainbow()const
{
    return m_pOwner->IsRainbow();
}


// ****************************************************************
// 
coreVector3 cWeapon::_GetEnergyColor()const
{
    return m_pOwner->GetEnergyColor();
}


// ****************************************************************
// 
void cWeapon::_TrackBullet()
{
    // 
    m_pOwner->GetDataTable()->EditCounterTotal  ()->iBulletsShot += 1u;
    m_pOwner->GetDataTable()->EditCounterMission()->iBulletsShot += 1u;
    m_pOwner->GetDataTable()->EditCounterSegment()->iBulletsShot += 1u;

    // 
    g_pSave->EditGlobalStats      ()->iBulletsShot += 1u;
    g_pSave->EditLocalStatsArcade ()->iBulletsShot += 1u;
    g_pSave->EditLocalStatsMission()->iBulletsShot += 1u;
    g_pSave->EditLocalStatsSegment()->iBulletsShot += 1u;
}


// ****************************************************************
// constructor
cRayWeapon::cRayWeapon()noexcept
: m_bBurst  (false)
, m_fMuzzleTime (0.0f)
, m_iMuzzleTick (0u)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(20.0f);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aMuzzle); ++i)
    {
        m_aMuzzle[i].DefineModel  ("object_tube_open.md3");
        m_aMuzzle[i].DefineTexture(0u, "effect_energy.png");
        m_aMuzzle[i].DefineProgram("effect_energy_flat_direct_program");
        m_aMuzzle[i].SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.8f, 0.0f));
        m_aMuzzle[i].SetTexSize   (coreVector2(0.5f,0.3f) * 0.3f);
        //m_aMuzzle[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
        
        g_pGlow->BindObject(&m_aMuzzle[i]);
    }
}


cRayWeapon::~cRayWeapon()
{
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aMuzzle); ++i)
        g_pGlow->UnbindObject(&m_aMuzzle[i]);
}


// ****************************************************************
// 
void cRayWeapon::__UpdateOwn(const coreUint8 iShootStatus, const coreFloat fShootSpeed)
{
    //if(iShootStatus) m_fVolume.UpdateMin(0.2f, 1.0f);
    //            else m_fVolume = 0.0f;
}


// ****************************************************************
// 
void cRayWeapon::__TriggerOwn(const coreUint8 iMode)
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();

    // 
    const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;

    if(iMode == 0u)
    {
        /*
        // 
        if(m_CooldownTimer.GetStatus()) return;

        // 
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(4 * iSign, 6.0f, m_pOwner, vPos, vDir))->ChangeScale(1.3f);

        // 
        m_CooldownTimer.SetValue(-1.1f);
        m_CooldownTimer.Play(CORE_TIMER_PLAY_CURRENT);
        */
    }
    else if(iMode == 1u)
    {
        // 
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6 * iSign, 1.0f, m_pOwner, vPos,  vDir.Rotated45());
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6 * iSign, 1.0f, m_pOwner, vPos,  vDir.Rotated90());
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6 * iSign, 1.0f, m_pOwner, vPos, -vDir.Rotated90());
        g_pGame->GetBulletManagerPlayer()->AddBullet<cRocketBullet>(6 * iSign, 1.0f, m_pOwner, vPos, -vDir.Rotated135());

        // 
        //if(m_pRocketSound.IsUsable()) m_pRocketSound->PlayPosition(NULL, 1.0f, 1.0f, false, SOUND_EFFECT, m_pOwner->GetPosition());
        g_pSpecialEffects->CreateSplashColor(m_pOwner->GetPosition(), SPECIAL_SPLASH_TINY, coreVector3(0.5f,0.5f,0.5f));
    }
    else if(iMode == 2u)
    {
        // 
        for(coreUintW i = 1u; i < 4u; ++i)
        {
            const coreVector2 vDir2 = coreVector2::Direction(0.25f * PI * I_TO_F(i));

            g_pGame->GetBulletManagerPlayer()->AddBullet<cMineBullet>(6 * iSign, 1.0f, m_pOwner, vPos,  vDir2);
            g_pGame->GetBulletManagerPlayer()->AddBullet<cMineBullet>(6 * iSign, 1.0f, m_pOwner, vPos, -vDir2);
        }

        // 
        //if(m_pMineSound.IsUsable()) m_pMineSound->PlayPosition(NULL, 1.0f, 1.0f, false, SOUND_EFFECT, m_pOwner->GetPosition());
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
    m_fMuzzleTime = 1.0f;
    m_iMuzzleTick += 1u;
    
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = vDir.Rotated90();

    // 
    const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;

    // 
    if(!m_bBurst)
    {
        m_bBurst = true;

        // (# bit less, as it basically merges upcoming damage) 
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(5 * iSign, 8.0f, m_pOwner, vPos, vDir))->ChangeScale(1.2f)->ChangeHeight(m_pOwner->GetPosition().z);

        // 
        m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 2.0f);

        // play bullet sound-effect
        g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 0.8f, SOUND_WEAPON_RAY);
    }
    else
    {
        // 
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1 * iSign, 8.0f, m_pOwner, vPos + vTan*1.55f, vDir))->ChangeHeight(m_pOwner->GetPosition().z);
        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(1 * iSign, 8.0f, m_pOwner, vPos - vTan*1.55f, vDir))->ChangeHeight(m_pOwner->GetPosition().z);
        //this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cRayBullet>(2 * iSign, 6.0f, m_pOwner, vPos, vDir))->ChangeHeight(m_pOwner->GetPosition().z);

        // play bullet sound-effect
        g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_RAY);
    }
}


// ****************************************************************
// 
void cRayWeapon::__RenderOwn()
{
    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aMuzzle); ++i)
    {
        m_aMuzzle[i].Render();
    }
}


// ****************************************************************
// 
void cRayWeapon::__MoveOwn()
{
    fTest.Update(4.0f);//LERP(0.0f, 4.0f, m_fMuzzleTime));
    
    
    
    if(!m_fMuzzleTime) fScale = 0.0f;
    else fScale.UpdateMin(20.0f, 1.0f);
    
    m_fMuzzleTime.UpdateMax(-9.0f, 0.0f);
    
    const coreVector3 vPos = m_pOwner->GetPosition ();
    const coreVector3 vDir = m_pOwner->GetDirection();
    const coreVector3 vOri = m_pOwner->GetOrientation();
    const coreVector3 vTan = vDir.RotatedZ90();
    
    //const coreFloat fScale = MAX0(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL));
    
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aMuzzle); ++i)
    {
        m_aMuzzle[i].SetPosition(vPos + vDir * 3.0f * fScale + vTan * (i ? -1.5f : 1.5f));
        m_aMuzzle[i].SetSize(coreVector3(0.5f, 1.2f * fScale, 0.5f) * 2.3f * 1.7f);// * (0.9f + 0.1f * SIN(fTest * 6.0f*PI)));// * LERP(0.8f, 1.2f, m_fMuzzleTime));
        m_aMuzzle[i].SetDirection(-vDir);
        m_aMuzzle[i].SetAlpha(STEP(0.0f, 1.0f, m_fMuzzleTime) * STEP(0.7f, 1.0f, ABS(vOri.z)));
        m_aMuzzle[i].SetTexOffset(coreVector2(0.02f * (i ? 1.0f : -1.0f), 0.1f) * (fTest + 0.0f*I_TO_F(m_iMuzzleTick)));
        m_aMuzzle[i].SetEnabled(m_fMuzzleTime ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        
        m_aMuzzle[i].Move();
    }
}


// ****************************************************************
// constructor
cPulseWeapon::cPulseWeapon()noexcept
: m_fCharge (0.0f)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(6.0f);
}


// ****************************************************************
// 
void cPulseWeapon::__UpdateOwn(const coreUint8 iShootStatus, const coreFloat fShootSpeed)
{
    // 
    if(!m_CooldownTimer.GetStatus()) m_fCharge.UpdateMin(m_CooldownTimer.GetSpeed() * fShootSpeed, 2.0f);
}


// ****************************************************************
// shoot with the pulse weapon
void cPulseWeapon::__ShootOwn()
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();

    // 
    const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;

    // 
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cPulseBullet>((5 + 10 * F_TO_SI(m_fCharge)) * iSign, 3.0f, m_pOwner, vPos, vDir))->ChangeScale(1.2f + 0.4f * FLOOR(m_fCharge))->AddStatus(BULLET_STATUS_PENETRATE);

    // 
    m_fCharge = 0.0f;

    // play bullet sound-effect
    //g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_PULSE);
}


// ****************************************************************
// constructor
cWaveWeapon::cWaveWeapon()noexcept
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(10.0f);
}


// ****************************************************************
// shoot with the wave weapon
void cWaveWeapon::__ShootOwn()
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();

    // 
    const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;

    // 
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cSurgeBullet>(2 * iSign, 3.5f, m_pOwner, vPos,  vDir))->ChangeScale(1.8f);
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cSurgeBullet>(2 * iSign, 3.5f, m_pOwner, vPos, -vDir))->ChangeScale(1.8f);

    // play bullet sound-effect
    //g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_WAVE);
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
            const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;

            // 
            pTarget->TakeDamage(6 * iSign, ELEMENT_BLUE, vPosTo, m_pOwner, false);
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
    const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;

    // 
    this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cTeslaBullet>(6 * iSign, 3.0f, m_pOwner, vPos, (vDir + vTan).Normalized()));

    // play bullet sound-effect
    //g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_TESLA);
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
    const coreVector2 vPos  = m_pOwner->GetPosition ().xy();
    const coreFloat   fBase = m_pOwner->GetDirection().xy().Angle();

    // 
    for(coreUintW j = 5u; j--; )
    {
        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 5.0f) + fBase);

        this->_MakeWhite(g_pGame->GetBulletManagerPlayer()->AddBullet<cViewBullet>(1, 4.0f, m_pOwner, vPos, vDir))->ChangeSize(1.5f);
    }

    // play bullet sound-effect
    g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
}


// ****************************************************************
// constructor
cFinalWeapon::cFinalWeapon()noexcept
: m_bBurst (false)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(20.0f);
}


// ****************************************************************
// 
void cFinalWeapon::__ReleaseOwn(const coreUint8 iMode)
{
    // 
    m_bBurst = false;
}


// ****************************************************************
// shoot with the final weapon
void cFinalWeapon::__ShootOwn()
{
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    //const coreVector2 vTan = vDir.Rotated90();

    // 
    cBulletManager* pManager = ((vDir.y <= 0.0f) && m_pOwner->GetTilt()) ? g_pGame->GetBulletManagerPlayerTop() : g_pGame->GetBulletManagerPlayer();
    
    const coreMatrix3 mTiltMat  = coreMatrix4::RotationX(m_pOwner->GetTilt()).m123();
    const coreVector3 vRealDir  = coreVector3(vDir, 0.0f) * mTiltMat;
    //const coreVector3 vRealTan  = coreVector3(vTan, 0.0f) * mTiltMat;
/*
    // 
    if(!m_bBurst)
    {
        m_bBurst = true;

        // (# bit less, as it basically merges upcoming damage) 
        this->_MakeWhite(pManager->AddBullet<cFinalBullet>(5, 8.0f, m_pOwner, vPos, vDir))->ChangeScale(1.2f)->SetTiltProperties(m_pOwner->GetPosition(), vRealDir)->ChangeHeight(m_pOwner->GetPosition().z);

        // 
        m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 2.0f);

        // play bullet sound-effect
        g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 0.8f, SOUND_WEAPON_RAY);
    }
    else*/
    {
        // 
        //this->_MakeWhite(pManager->AddBullet<cFinalBullet>(1, 8.0f, m_pOwner, vPos + vTan*1.55f, vDir))->SetTiltProperties(m_pOwner->GetPosition() + vRealTan*3.0f, vRealDir)->ChangeHeight(m_pOwner->GetPosition().z);
        //this->_MakeWhite(pManager->AddBullet<cFinalBullet>(1, 8.0f, m_pOwner, vPos - vTan*1.55f, vDir))->SetTiltProperties(m_pOwner->GetPosition() - vRealTan*3.0f, vRealDir)->ChangeHeight(m_pOwner->GetPosition().z);
        
        this->_MakeWhite(pManager->AddBullet<cFinalBullet>(2, 16.0f, m_pOwner, vPos, vDir))->ChangeScale(1.5f)->SetTiltProperties(m_pOwner->GetPosition(), vRealDir)->ChangeHeight(m_pOwner->GetPosition().z);

        // play bullet sound-effect
        g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_RAY);
    }
}