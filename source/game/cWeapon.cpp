///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

/*

Ray

20x
1x2=2
40dps

20x / 3 = 6.67x
5
33.33dps

Pulse

18x
2
36dps (muss niedriger als ray sein, 90% von ray)

18x / 3 = 6x
2x3 = 6
36dps

1.5x
2x12 + 3 = 27
40.5dps (kann höher sein, weil man eh nah ran muss, aber <= ray) (mitte etwas stärker um, damit man nur 2 statt 3 treffer braucht) (- muss leicht niedriger als sustained fire sein, sonst bringt sich sustained nix)

*/


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

void cWeapon::RenderAfter()
{
    // 
    this->__RenderAfterOwn();
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
void cWeapon::Prefetch()
{
    // 
    this->__PrefetchOwn();
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
: m_iBurst       (0u)
, m_fMuzzleTime  (0.0f)
, m_fMuzzleAnim  (0.0f)
, m_fMuzzleScale (0.0f)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(20.0f);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aMuzzle); ++i)
    {
        m_aMuzzle[i].DefineModel  ("object_tube.md3");
        m_aMuzzle[i].DefineTexture(0u, "effect_energy.png");
        m_aMuzzle[i].DefineProgram("effect_energy_flat_direct_program");
        m_aMuzzle[i].SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.8f, 0.0f));
        m_aMuzzle[i].SetTexSize   (coreVector2(0.5f,0.3f) * 0.3f);
        m_aMuzzle[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
        
        g_pGlow->BindObject(&m_aMuzzle[i]);
    }
}


// ****************************************************************
// 
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
  
    //if(HAS_BIT(m_pOwner->GetInput()->iStatus, 1u) && (m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) >= 0.0f))
    //{
    //    if(m_iBurst == 1u) m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 1.0f);
//
    //    m_iBurst = 0u;
    //}
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
    if(m_iBurst == 1u) m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 1.0f);
    
    // 
    m_iBurst = 0u;
}


// ****************************************************************
// shoot with the ray weapon
void cRayWeapon::__ShootOwn()
{
    m_fMuzzleTime = 1.0f;
    
    // 
    const coreVector2 vPos = m_pOwner->GetPosition ().xy();
    const coreVector2 vDir = m_pOwner->GetDirection().xy();
    const coreVector2 vTan = vDir.Rotated90();

    // 
    const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;
    
    cBulletManager* pManager = m_pOwner->HasStatus(PLAYER_STATUS_TOP) ? g_pGame->GetBulletManagerPlayerTop() : g_pGame->GetBulletManagerPlayer();
    
    
    const coreVector2 vOffset = vDir * (8.0f * BULLET_SPEED_FACTOR * m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) * RCP(m_CooldownTimer.GetSpeed() * m_pOwner->GetShootSpeed()));

    // 
    if(!m_iBurst)
    {
        m_iBurst = 1u;

        // (# bit less, as it basically merges upcoming damage) 
        this->_MakeWhite(pManager->AddBullet<cRayBullet>(5 * iSign, 8.0f, m_pOwner, vPos + vOffset, vDir))->ChangeScale(1.35f)->ChangeHeight(m_pOwner->GetPosition().z);

        // 
        m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 1.0f);

        // play bullet sound-effect
        g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 0.8f, SOUND_WEAPON_RAY);
    }
    else
    {
        m_iBurst = 2u;

        // 
        this->_MakeWhite(pManager->AddBullet<cRayBullet>(1 * iSign, 8.0f, m_pOwner, vPos + vOffset + vTan*1.55f, vDir))->ChangeScale(1.05f)->ChangeHeight(m_pOwner->GetPosition().z);
        this->_MakeWhite(pManager->AddBullet<cRayBullet>(1 * iSign, 8.0f, m_pOwner, vPos + vOffset - vTan*1.55f, vDir))->ChangeScale(1.05f)->ChangeHeight(m_pOwner->GetPosition().z);
        //this->_MakeWhite(pManager->AddBullet<cRayBullet>(2 * iSign, 6.0f, m_pOwner, vPos, vDir))->ChangeHeight(m_pOwner->GetPosition().z);

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
    const coreFloat fShootSpeed = m_pOwner->GetShootSpeed();
    
    m_fMuzzleAnim.Update(4.0f * fShootSpeed);//LERP(0.0f, 4.0f, m_fMuzzleTime));
    
    
    
    if(!m_fMuzzleTime) m_fMuzzleScale = 0.0f;
    else m_fMuzzleScale.UpdateMin(20.0f * fShootSpeed, 1.0f);
    
    m_fMuzzleTime.UpdateMax(-9.0f * fShootSpeed, 0.0f);
    
    const coreVector3 vPos = m_pOwner->GetPosition ();
    const coreVector3 vDir = m_pOwner->GetDirection();
    const coreVector3 vOri = m_pOwner->GetOrientation();
    const coreVector3 vTan = vDir.RotatedZ90();
    
    //const coreFloat fScale = MAX0(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL));
    
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aMuzzle); ++i)
    {
        m_aMuzzle[i].SetPosition(vPos + vDir * 3.0f * m_fMuzzleScale + vTan * (i ? -1.5f : 1.5f));
        m_aMuzzle[i].SetSize(coreVector3(0.5f, 1.2f * m_fMuzzleScale, 0.5f) * 2.3f * 1.7f);// * (0.9f + 0.1f * SIN(fTest * 6.0f*PI)));// * LERP(0.8f, 1.2f, m_fMuzzleTime));
        m_aMuzzle[i].SetDirection(-vDir);
        m_aMuzzle[i].SetAlpha(STEP(0.0f, 1.0f, m_fMuzzleTime) * STEP(0.7f, 1.0f, ABS(vOri.z)));
        m_aMuzzle[i].SetTexOffset(coreVector2(0.02f * (i ? 1.0f : -1.0f), 0.1f) * m_fMuzzleAnim);
        m_aMuzzle[i].SetEnabled(m_fMuzzleTime ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
        
        m_aMuzzle[i].Move();
    }
}


// ****************************************************************
// 
void cRayWeapon::__PrefetchOwn()
{
    g_pGame->GetBulletManagerPlayer()->PrefetchBullet<cRayBullet>();
}


// ****************************************************************
// constructor
cPulseWeapon::cPulseWeapon()noexcept
: m_fCharge      (0.0f)
, m_iBurst       (0u)
, m_iShotType    (0u)
, m_fMuzzleTime  (0.0f)
, m_fMuzzleAnim  (0.0f)
, m_fMuzzleScale (0.0f)
, m_fWave        (0.0f)
, m_fAnimation   (0.0f)
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(18.0f);

    // 
    m_Charge.DefineModel  ("object_penta_top.md3");
    m_Charge.DefineTexture(0u, "effect_energy.png");
    m_Charge.DefineProgram("effect_energy_flat_spheric_program");
    m_Charge.SetTexSize   (coreVector2(1.0f,1.0f) * 0.15f);
    m_Charge.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_ChargeWave.DefineModel  ("object_penta_top.md3");
    m_ChargeWave.DefineTexture(0u, "effect_energy.png");
    m_ChargeWave.DefineProgram("effect_energy_flat_spheric_program");
    m_ChargeWave.SetColor3    (COLOR_ENERGY_WHITE * 0.4f);
    m_ChargeWave.SetTexSize   (coreVector2(1.0f,1.0f) * 0.15f);
    m_ChargeWave.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Muzzle.DefineModel  ("object_tube.md3");
    m_Muzzle.DefineTexture(0u, "effect_energy.png");
    m_Muzzle.DefineProgram("effect_energy_flat_direct_program");
    m_Muzzle.SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.8f, 0.0f));
    m_Muzzle.SetTexSize   (coreVector2(0.5f,0.3f) * 0.3f);
    m_Muzzle.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    g_pGlow->BindObject(&m_Charge);
    g_pGlow->BindObject(&m_ChargeWave);
    g_pGlow->BindObject(&m_Muzzle);
}


// ****************************************************************
// 
cPulseWeapon::~cPulseWeapon()
{
    // 
    g_pGlow->UnbindObject(&m_Charge);
    g_pGlow->UnbindObject(&m_ChargeWave);
    g_pGlow->UnbindObject(&m_Muzzle);
}


// ****************************************************************
// 
void cPulseWeapon::__UpdateOwn(const coreUint8 iShootStatus, const coreFloat fShootSpeed)
{

    const coreFloat fFactor = m_CooldownTimer.GetSpeed() / WEAPON_PULSE_CHARGE / 3.0f;
    
    const coreFloat fOld = m_fCharge;
    m_fCharge.UpdateMin(fFactor * m_pOwner->GetShootSpeed(), 1.0f);
    
    if(m_pOwner->HasStatus(PLAYER_STATUS_NO_INPUT_SHOOT)) m_fCharge = 0.0f;

    if((m_fCharge >= 1.0f) && (fOld < 1.0f))
    {
        m_fWave = 1.0f;
    }
}


// ****************************************************************
// 
void cPulseWeapon::__ReleaseOwn(const coreUint8 iMode)
{
    if(m_iBurst == 1u) m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 1.0f);
    
    // 
    m_iBurst = 0u;
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
    
    cBulletManager* pManager = m_pOwner->HasStatus(PLAYER_STATUS_TOP) ? g_pGame->GetBulletManagerPlayerTop() : g_pGame->GetBulletManagerPlayer();
    
    const coreVector2 vOffset = vDir * (6.0f * BULLET_SPEED_FACTOR * m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) * RCP(m_CooldownTimer.GetSpeed() * m_pOwner->GetShootSpeed()));


    
    // 
    if(!m_iBurst)
    {
        m_iBurst = 1u;

        // 
        m_CooldownTimer.SetValue(m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) - 1.0f);

        
        constexpr coreUintW aiAngle[] = {0u, 12u, 1u, 11u, 2u, 10u, 3u, 9u, 4u, 8u, 5u, 7u};

        coreUintW iCount = 0u;
             if(m_fCharge >= 1.0f)                       iCount = 12u;
        else if(m_fCharge >= 1.0f / WEAPON_PULSE_CHARGE) iCount = 2u;
        
        for(coreUintW i = 12u - iCount; i < 12u; ++i)
        {
            const coreVector2 vPart = MapToAxis(coreVector2::Direction((I_TO_F(aiAngle[i]) - 6.0f) * (0.03f*PI)), vDir);
            const coreFloat fSpeed = 6.0f * (1.0f - 0.05f * ABS(I_TO_F(aiAngle[i]) - 6.0f));
            
            this->_MakeWhite(pManager->AddBullet<cPulseBullet>(2 * iSign, fSpeed, m_pOwner, vPos + vOffset, vPart))->ChangeScale(1.6f)->ChangeHeight(m_pOwner->GetPosition().z);
        }

        //  
        this->_MakeWhite(pManager->AddBullet<cPulseBullet>(((iCount == 12u) ? 3 : 2) * iSign, 6.0f, m_pOwner, vPos + vOffset, vDir))->ChangeScale(1.8f)->ChangeHeight(m_pOwner->GetPosition().z);
        
        m_iShotType = 0u;
    }
    else
    {
        m_iBurst = 2u;

        if(++m_iShotType >= 4u) m_iShotType = 0u;

        const coreVector2 vTan = vDir.Rotated90();
        const coreVector2 vNewDir = (vDir + vTan * ((m_iShotType < 3u) ? (1.0f * I_TO_F(m_iShotType - 1u)) : 0.0f) * 0.05f).Normalized();

        this->_MakeWhite(pManager->AddBullet<cPulseBullet>(2 * iSign, 6.0f, m_pOwner, vPos + vOffset, vNewDir))->ChangeScale(1.8f)->ChangeHeight(m_pOwner->GetPosition().z);
    }

    // 
    m_fCharge = 0.0f;

    // 
    m_fMuzzleTime = 1.0f;

    // play bullet sound-effect
    g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_RAY);
}


// ****************************************************************
// 
void cPulseWeapon::__RenderOwn()
{
    // 
    m_Muzzle.Render();
}


// ****************************************************************
// 
void cPulseWeapon::__RenderAfterOwn()
{
    // 
    m_Charge.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Charge);

    // 
    m_ChargeWave.Render();
}


// ****************************************************************
// 
void cPulseWeapon::__MoveOwn()
{
    const coreFloat fShootSpeed = m_pOwner->GetShootSpeed();
    const coreFloat fAnimSpeed  = m_pOwner->GetAnimSpeed();
    
    // 
    m_fWave     .UpdateMax(-3.0f * fAnimSpeed,  0.0f);
    m_fAnimation.UpdateMod( 1.0f * fAnimSpeed, 20.0f);
    

    const coreVector3 vPos = m_pOwner->GetPosition ();
    const coreVector3 vDir = m_pOwner->GetDirection();
    
    constexpr coreVector3 vBaseSize = coreVector3(1.0f,1.0f,1.0f) * 2.5f;
    
    const coreFloat fScale = STEP(1.0f / WEAPON_PULSE_CHARGE, 1.0f, m_fCharge);
    
    // 
    m_Charge.SetPosition (vPos + vDir * 4.0f);
    m_Charge.SetSize     (vBaseSize * fScale);
    m_Charge.SetDirection(coreVector3(coreVector2::Direction(m_fAnimation * PI), 0.0f));
    m_Charge.SetColor3   (COLOR_ENERGY_WHITE * ((m_fCharge < 1.0f) ? 0.2f : 0.45f));
    m_Charge.SetTexOffset(m_Charge.GetTexOffset() - m_Charge.GetDirection().xy() * (0.15f * TIME * fAnimSpeed));
    m_Charge.SetEnabled  (fScale ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Charge.Move();

    // 
    m_ChargeWave.SetPosition (m_Charge.GetPosition());
    m_ChargeWave.SetSize     (vBaseSize * LERPBR(3.0f, 1.0f, m_fWave));
    m_ChargeWave.SetDirection(m_Charge.GetDirection());
    m_ChargeWave.SetAlpha    (BLENDBR(m_fWave));
    m_ChargeWave.SetTexOffset(m_Charge.GetTexOffset());
    m_ChargeWave.SetEnabled  (m_fWave ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_ChargeWave.Move();
    
    
    
    m_fMuzzleAnim.Update(4.0f * fShootSpeed);

    if(!m_fMuzzleTime) m_fMuzzleScale = 0.0f;
    else m_fMuzzleScale.UpdateMin(20.0f * fShootSpeed, 1.0f);

    m_fMuzzleTime.UpdateMax(-5.0f * fShootSpeed, 0.0f);

    const coreVector3 vOri = m_pOwner->GetOrientation();


    m_Muzzle.SetPosition (vPos + vDir * 3.5f * m_fMuzzleScale);
    m_Muzzle.SetSize     (coreVector3(0.5f, 1.2f * m_fMuzzleScale, 0.5f) * 2.3f * 2.0f);
    m_Muzzle.SetDirection(-vDir);
    m_Muzzle.SetAlpha    (STEP(0.0f, 1.0f, m_fMuzzleTime) * STEP(0.7f, 1.0f, ABS(vOri.z)));
    m_Muzzle.SetTexOffset(coreVector2(0.02f,0.1f) * m_fMuzzleAnim);
    m_Muzzle.SetEnabled  (m_fMuzzleTime ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Muzzle.Move();
}


// ****************************************************************
// 
void cPulseWeapon::__PrefetchOwn()
{
    g_pGame->GetBulletManagerPlayer()->PrefetchBullet<cPulseBullet>();
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
    const coreVector2 vTan = vDir.Rotated90();

    // 
    const coreInt32 iSign = m_pOwner->HasStatus(PLAYER_STATUS_HEALER) ? -1 : 1;
    
    cBulletManager* pManager = m_pOwner->HasStatus(PLAYER_STATUS_TOP) ? g_pGame->GetBulletManagerPlayerTop() : g_pGame->GetBulletManagerPlayer();
    
    const coreVector2 vOffset = vTan * (3.5f * BULLET_SPEED_FACTOR * m_CooldownTimer.GetValue(CORE_TIMER_GET_NORMAL) * RCP(m_CooldownTimer.GetSpeed() * m_pOwner->GetShootSpeed()));

    // 
    this->_MakeWhite(pManager->AddBullet<cSurgeBullet>(2 * iSign, 3.5f, m_pOwner, vPos + vOffset,  vTan))->ChangeScale(1.8f)->ChangeHeight(m_pOwner->GetPosition().z);
    this->_MakeWhite(pManager->AddBullet<cSurgeBullet>(2 * iSign, 3.5f, m_pOwner, vPos - vOffset, -vTan))->ChangeScale(1.8f)->ChangeHeight(m_pOwner->GetPosition().z);

    // play bullet sound-effect
    g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_RAY);
}


// ****************************************************************
// 
void cWaveWeapon::__PrefetchOwn()
{
    g_pGame->GetBulletManagerPlayer()->PrefetchBullet<cSurgeBullet>();
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
// 
void cTeslaWeapon::__PrefetchOwn()
{
    g_pGame->GetBulletManagerPlayer()->PrefetchBullet<cTeslaBullet>();
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
    g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_RAY);
}


// ****************************************************************
// 
void cEnemyWeapon::__PrefetchOwn()
{
    g_pGame->GetBulletManagerPlayer()->PrefetchBullet<cViewBullet>();
}


// ****************************************************************
// constructor
cFinalWeapon::cFinalWeapon()noexcept
{
    // set base fire-rate
    m_CooldownTimer.SetSpeed(20.0f);
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


    this->_MakeWhite(pManager->AddBullet<cFinalBullet>(2, 16.0f, m_pOwner, vPos, vDir))->ChangeScale(1.5f)->SetTiltProperties(m_pOwner->GetPosition(), vRealDir)->ChangeHeight(m_pOwner->GetPosition().z);

    // play bullet sound-effect
    g_pSpecialEffects->PlaySound(m_pOwner->GetPosition(), 1.0f, 1.0f, SOUND_WEAPON_RAY);
}


// ****************************************************************
// 
void cFinalWeapon::__PrefetchOwn()
{
    g_pGame->GetBulletManagerPlayer()->PrefetchBullet<cFinalBullet>();
}