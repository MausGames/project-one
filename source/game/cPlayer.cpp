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
// constructor
cPlayer::cPlayer()noexcept
: m_pInput         (&g_aInput[0])
, m_iScoreMission  (0u)
, m_fChainCooldown (0.0f)
, m_fDarkAnimation (0.0f)
, m_fDarkTime      (0.0f)
, m_vNewPos        (coreVector2(0.0f,0.0f))
, m_vForce         (coreVector2(0.0f,0.0f))
{
    // load object resources
    this->DefineTexture(0u, "ship_player.png");
    this->DefineTexture(1u, "menu_background_black.png");
    this->DefineProgram("object_ship_program");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));
    this->SetTexSize    (coreVector2(1.2f,1.2f));

    // set initial status
    m_iStatus = PLAYER_STATUS_DEAD;
    this->SetMaxHealth(100);

    // load first weapons
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
    {
        m_apWeapon[i] = new cNoWeapon();
        m_apWeapon[i]->SetOwner(this);
    }

    // reset scoring stats
    this->ResetStats();

    // 
    m_pDarkProgram = Core::Manager::Resource->Get<coreProgram>("object_ship_darkness_program");

    // 
    m_Bubble.DefineModel  ("object_sphere.md3");
    m_Bubble.DefineTexture(0u, "effect_energy.png");
    m_Bubble.DefineProgram("effect_energy_bullet_spheric_program");
    m_Bubble.SetColor4    (coreVector4(0.5f,0.5f,0.5f,0.0f));
    m_Bubble.SetTexSize   (coreVector2(5.0f,5.0f));
    m_Bubble.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Exhaust.DefineModel  ("object_tube.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_direct_program");
    m_Exhaust.SetDirection (this->GetDirection());
    m_Exhaust.SetColor4    (coreVector4(COLOR_FIRE_BLUE, 0.7f));
    m_Exhaust.SetTexSize   (coreVector2(0.5f,0.25f));
    m_Exhaust.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
}


// ****************************************************************
// destructor
cPlayer::~cPlayer()
{
    // remove player from the game
    this->Kill(false);

    // delete weapon objects
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
        SAFE_DELETE(m_apWeapon[i])
}


// ****************************************************************
// configure the player
void cPlayer::Configure(const coreUintW iShipType, const coreVector3& vColor, const coreUintW iInputIndex)
{
    // select appearance type
    const coreChar* pcModelHigh;
    const coreChar* pcModelLow;
    switch(iShipType)
    {
    default: ASSERT(false)
    case PLAYER_SHIP_ATK: pcModelHigh = "ship_player_atk_high.md3"; pcModelLow = "ship_player_atk_low.md3"; break;
    case PLAYER_SHIP_DEF: pcModelHigh = "ship_player_def_high.md3"; pcModelLow = "ship_player_def_low.md3"; break;
    }

    // load models
    this->DefineModelHigh(pcModelHigh);
    this->DefineModelLow (pcModelLow);
    this->GetModel().GetHandle()->OnLoadOnce([&]()
    {
        // normalize collision size of different ship models
        this->SetCollisionModifier((coreVector3(1.0f,1.0f,1.0f) * PLAYER_COLLISION_SIZE) / this->GetModel()->GetBoundingRange());
    });

    // save color value
    this->SetBaseColor(vColor);

    // save input set
    WARN_IF(iInputIndex >= INPUT_SETS)
         m_pInput = &g_aInput[0];
    else m_pInput = &g_aInput[iInputIndex];
}


// ****************************************************************
// equip new main weapon
void cPlayer::EquipWeapon(const coreUintW iIndex, const coreInt32 iID)
{
    ASSERT(iIndex < PLAYER_WEAPONS)
    if(m_apWeapon[iIndex]) if(m_apWeapon[iIndex]->GetID() == iID) return;

    // delete possible old weapon
    SAFE_DELETE(m_apWeapon[iIndex])

    // create new weapon
    switch(iID)
    {
    default: ASSERT(false)
    case cNoWeapon   ::ID: m_apWeapon[iIndex] = new cNoWeapon   (); break;
    case cRayWeapon  ::ID: m_apWeapon[iIndex] = new cRayWeapon  (); break;
    case cPulseWeapon::ID: m_apWeapon[iIndex] = new cPulseWeapon(); break;
    case cWaveWeapon ::ID: m_apWeapon[iIndex] = new cWaveWeapon (); break;
    case cTeslaWeapon::ID: m_apWeapon[iIndex] = new cTeslaWeapon(); break;
    case cAntiWeapon ::ID: m_apWeapon[iIndex] = new cAntiWeapon (); break;
    }
    m_apWeapon[iIndex]->SetOwner(this);

#if defined(_CORE_DEBUG_)

    // same weapon should not be equipped twice
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
        for(coreUintW j = i+1u; j < PLAYER_WEAPONS; ++j)
            ASSERT(m_apWeapon[i]->GetID() != m_apWeapon[j]->GetID())

#endif
}


// ****************************************************************
// render the player
void cPlayer::Render()
{
    if(CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_Bubble .Render();
        m_Exhaust.Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    this->_EnableBlink();

    // render the 3d-object
    coreObject3D::Render();

    // 
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
        m_apWeapon[i]->Render();
}


// ****************************************************************
// move the player
void cPlayer::Move()
{
    // 
    this->_UpdateAlways();
    if(CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;

    //const coreUint8 iShoot = (m_pInput->iButtonHold
    //                          || Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LSHIFT), CORE_INPUT_HOLD)
    //                          || Core::Input->GetKeyboardButton(CORE_INPUT_KEY(SPACE), CORE_INPUT_HOLD)) ? 1 : 0;

    if(!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_MOVE))
    {

        if(m_pInput->iButtonPress & 0x02u)
        {
            // this->EquipWeapon(0u, (m_apWeapon[0]->GetID() % cTeslaWeapon::ID) + 1u);

            //g_pDistortion    ->CreateWave       (this->GetPosition(), DISTORTION_WAVE_SMALL);
            //g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_SMALL, m_apWeapon[0]->GetColorEnergy());
            //g_pSpecialEffects->CreateBlast      (this->GetPosition(), SPECIAL_BLAST_SMALL, m_apWeapon[0]->GetColorEnergy());

            //if(m_pInput->iButtonHold & 0x01u) this->EquipWeapon(0u, cRayWeapon::ID);
            //else if(m_pInput->iButtonHold & 0x02u) this->EquipWeapon(0u, cPulseWeapon::ID);
            //else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(LSHIFT), CORE_INPUT_HOLD)) this->EquipWeapon(0u, cWaveWeapon::ID);
            //else if(Core::Input->GetKeyboardButton(CORE_INPUT_KEY(SPACE), CORE_INPUT_HOLD))  this->EquipWeapon(0u, cTeslaWeapon::ID);
        }

        // move the ship
        const coreFloat fSpeed = (!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_SHOOT) && CONTAINS_BIT(m_pInput->iButtonHold, 0u)) ? 20.0f : 50.0f;
        m_vNewPos += m_pInput->vMove * (Core::System->GetTime() * fSpeed);

        // 
        if(!m_vForce.IsNull())
        {
            m_vNewPos += m_vForce * Core::System->GetTime();
            m_vForce  *= 1.0f - 3.0f * Core::System->GetTime();
        }

        // restrict movement to the foreground area
             if(m_vNewPos.x < -FOREGROUND_AREA.x) m_vNewPos.x = -FOREGROUND_AREA.x;
        else if(m_vNewPos.x >  FOREGROUND_AREA.x) m_vNewPos.x =  FOREGROUND_AREA.x;
             if(m_vNewPos.y < -FOREGROUND_AREA.y) m_vNewPos.y = -FOREGROUND_AREA.y;
        else if(m_vNewPos.y >  FOREGROUND_AREA.y) m_vNewPos.y =  FOREGROUND_AREA.y;

        // calculate smooth position-offset
        const coreVector2 vDiff   = m_vNewPos - this->GetPosition().xy();
        const coreVector2 vOffset = vDiff * (Core::System->GetTime() * 40.0f);


        static coreFlow time = 0.0f;
        static coreBool status = false;
        static coreFloat side = 1.0f;
        if(m_pInput->iButtonPress & 0x02u)
        {
            if(!status)
            {
                status = true;
                side = (m_pInput->vMove.x <= 0.0) ? -1.0f : 1.0f;
                this->ChangeType(0);
            }
        }
        if(status)
        {
            time.Update(2.0f);
            if(time >= 1.0f)
            {
                time = 0.0f;
                status = false;
                this->ChangeType(TYPE_PLAYER);
            }
        }

        // geschosse beim austoßen erst vergrößern
        // unterschiedliche waffen am start erlauben
        // 


        const coreMatrix2 mRota = coreMatrix3::Rotation(LERPS(0.0f, 2.0f*PI, time) * side).m12();
        const coreVector2 vOri  = coreVector2(CLAMP(vDiff.x, -0.6f, 0.6f), 1.0f).Normalize() * mRota;


        // set new position and orientation
        
        //this->SetPosition   (coreVector3(vOffset + this->GetPosition().xy(), 0.0f));
        this->SetPosition   (coreVector3(m_vNewPos, 0.0f));
        //this->SetOrientation(coreVector3(CLAMP(vDiff.x, -0.6f, 0.6f), 0.0f, 1.0f).Normalize());
        this->SetOrientation(coreVector3(vOri.x, 0.0f, vOri.y));
    }

    // 
    this->_UpdateBlink();

    // move the 3d-object
    coreObject3D::Move();

    // 
    if(m_fChainCooldown)
    {
        if(!(m_fChainCooldown = MAX(m_fChainCooldown - Core::System->GetTime(), 0.0f)))
            this->TransferChain();
    }

    // update all weapons (shooting and stuff)
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
    {
        const coreUint8 iShoot = (!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_SHOOT)) ? ((m_pInput->iButtonHold & (BITLINE(WEAPON_MODES) << (i*WEAPON_MODES))) >> (i*WEAPON_MODES)) : 0u;
        m_apWeapon[i]->Update(iShoot);
    }

    // 
    //if(CONTAINS_BIT(m_pInput->iButtonPress, PLAYER_WEAPONS * WEAPON_MODES))
     //   this->TransformDark(CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS) ? PLAYER_DARK_OFF : PLAYER_DARK_ON);

    if(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_ALL))
    {
        // 
        m_fDarkAnimation.Update(-1.0f);
        this->SetTexOffset(coreVector2(0.0f, m_fDarkAnimation * 0.25f));

        // 
        if(CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS))
             m_Bubble.SetAlpha(MIN(m_Bubble.GetAlpha() + 4.0f*Core::System->GetTime(), 0.8f));
        else m_Bubble.SetAlpha(MAX(m_Bubble.GetAlpha() - 4.0f*Core::System->GetTime(), 0.0f));

        // 
        if(!m_Bubble.GetAlpha()) this->DisableBubble();

        // 
        m_Bubble.SetPosition (this->GetPosition());
        m_Bubble.SetSize     (coreVector3(1.25f,1.25f,1.25f) * m_Bubble.GetAlpha() * PLAYER_DARK_BUBBLE_SIZE       / 1.25f*1.15f);
        m_Bubble.SetTexOffset(coreVector2(0.0f, m_fDarkAnimation * 0.1f));
        m_Bubble.Move();
    }

    // 
    //if(m_fDarkTime)
    //{
    //    coreBool bGrace = false;
    //
    //    // 
    //    Core::Manager::Object->TestCollision(TYPE_BULLET_ENEMY, &m_Bubble, [&bGrace](const cBullet* pBullet, const coreBool bFirstHit)
    //    {
    //        bGrace = true;
    //    });
    //
    //    // 
    //    m_fDarkTime = CLAMP(m_fDarkTime + (bGrace ? 0.025f : -0.05f) * Core::System->GetTime(), 0.0f, 1.0f);
    //    if(!m_fDarkTime) this->TransformDark(PLAYER_DARK_RESET);
    //}
}


// ****************************************************************
// reduce current health
void cPlayer::TakeDamage(const coreInt32 iDamage, const coreUint8 iElement)
{
    // 
    g_pGame->GetCombatText()->AddDamage(iDamage, this->GetPosition());

    // 
    this->TransferChain();
    this->ReduceCombo();

    // 
    if(this->_TakeDamage(iDamage, iElement))
        this->Kill(true);
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect(const coreVector2& vPosition)
{
    // resurrect player
    if(!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DEAD)

    // 
    m_vNewPos = vPosition;

    // add ship to the game
    cShip::_Resurrect(true, vPosition, coreVector2(0.0f,1.0f), TYPE_PLAYER);
}


// ****************************************************************
// remove player from the game
void cPlayer::Kill(const coreBool bAnimated)
{
    // kill player
    if(CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;
    ADD_FLAG(m_iStatus, PLAYER_STATUS_DEAD)

    // reset weapon shoot status
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
        m_apWeapon[i]->Update(0u);

    // 
    this->TransformDark(PLAYER_DARK_RESET);

    // 
    this->DisableBubble();
    this->UpdateExhaust(0.0f);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalBig(this->GetPosition());

    // remove ship from the game
    cShip::_Kill(true, bAnimated);
}


// ****************************************************************
// 
void cPlayer::AddScore(const coreUint32 iValue, const coreBool bModified)
{
    const coreUint32 iFinalValue = bModified ? (iValue * this->GetCurCombo()) : iValue;

    // 
    m_iScoreMission += iFinalValue;

    // 
    if(g_pGame->GetMission()->GetCurBoss())
        m_aiScoreBoss[g_pGame->GetMission()->GetCurBossIndex()] += iFinalValue;
}


// ****************************************************************
// 
void cPlayer::AddCombo(const coreUint32 iValue)
{
    const coreUint32 iOld = F_TO_UI(this->GetCurCombo());
    {
        // 
        m_iComboValue[0] += iValue;
        m_iComboValue[1]  = MAX(m_iComboValue[0], m_iComboValue[1]);
    }
    const coreUint32 iNew = F_TO_UI(this->GetCurCombo());

    // 
    if(iOld != iNew) g_pGame->GetCombatText()->AddCombo(iNew, this->GetPosition());
}

void cPlayer::AddCombo(const coreFloat fModifier)
{
    // TODO # 
}


// ****************************************************************
// 
void cPlayer::AddChain(const coreUint32 iValue)
{
    // 
    m_iChainValue[0] += iValue;
    m_iChainValue[1]  = MAX(m_iChainValue[0], m_iChainValue[1]);

    // 
    m_fChainCooldown = 1.0f;
}


// ****************************************************************
// 
void cPlayer::ReduceCombo()
{
    // TODO # 
}


// ****************************************************************
// 
void cPlayer::TransferChain()
{
    if(m_iChainValue[0])
    {
        // 
        g_pGame->GetCombatText()->AddChain(m_iChainValue[0], this->GetPosition());

        // 
        this->AddScore(m_iChainValue[0], true);
        m_iChainValue[0] = 0u;

        // 
        m_fChainCooldown = 0.0f;
    }
}


// ****************************************************************
// reset scoring stats
void cPlayer::ResetStats()
{
    // 
    m_iScoreMission = 0u;
    std::memset(m_aiScoreBoss, 0, sizeof(m_aiScoreBoss));

    // 
    m_iComboValue[1] = m_iComboValue[0] = 0u;
    m_iChainValue[1] = m_iChainValue[0] = 0u;
    m_fChainCooldown = 0.0f;
}


// ****************************************************************
// 
void cPlayer::TransformDark(const coreUint8 iStatus)
{
    // 
    const coreBool bDark = CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS);
    if(bDark == (!iStatus)) return;

    if(iStatus == PLAYER_DARK_ON)
    {
        ADD_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS)

        // 
        g_pDistortion    ->CreateWave      (this->GetPosition(), DISTORTION_WAVE_SMALL);
        g_pSpecialEffects->CreateSplashDark(this->GetPosition(), SPECIAL_SPLASH_SMALL);
        g_pSpecialEffects->CreateBlast     (this->GetPosition(), SPECIAL_BLAST_SMALL, coreVector3(1.0f,1.0f,1.0f));

        // 
        m_fDarkAnimation = 0.0f;
        m_fDarkTime      = 1.0f;

        // 
        this->EnableBubble();
    }
    else if(iStatus == PLAYER_DARK_OFF)
    {
        REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS)

        // 
        g_pDistortion    ->CreateWave      (this->GetPosition(), DISTORTION_WAVE_BIG);
        g_pSpecialEffects->CreateSplashDark(this->GetPosition(), SPECIAL_SPLASH_BIG);
        g_pSpecialEffects->CreateBlast     (this->GetPosition(), SPECIAL_BLAST_BIG, coreVector3(1.0f,1.0f,1.0f));

        // 
        m_fDarkTime = 0.0f;

        // 
        g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
        {
            // 
            pBullet->Deactivate(false);

            // 
            const coreVector2 vDir = (pBullet->GetPosition().xy() - this->GetPosition().xy()).Normalize();
            g_pSpecialEffects->CreateBlowColor(pBullet->GetPosition(), coreVector3(vDir, 0.0f), 60.0f, 2u, pBullet->GetColor3());
        });
    }
    else REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS)

    // 
    std::swap(m_pDarkProgram, m_pProgram);
}


// ****************************************************************
// 
void cPlayer::EnableBubble()
{
    WARN_IF(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Bubble.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Bubble);
}


// ****************************************************************
// 
void cPlayer::DisableBubble()
{
    if(!m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Bubble.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Bubble);
}


// ****************************************************************
// 
void cPlayer::UpdateExhaust(const coreFloat fStrength)
{
    // 
    const coreFloat fLen  = fStrength * 40.0f;
    const coreFloat fSize = 1.0f - fStrength * 0.25f;

    // 
         if( fStrength && !m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) g_pGlow->BindObject  (&m_Exhaust);
    else if(!fStrength &&  m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) g_pGlow->UnbindObject(&m_Exhaust);

    // 
    m_Exhaust.SetSize     (coreVector3(fSize, fLen, fSize) * 0.6f);
    m_Exhaust.SetTexOffset(coreVector2(0.0f, coreFloat(Core::System->GetTotalTime()) * 0.75f));
    m_Exhaust.SetPosition (coreVector3(0.0f, -(m_Exhaust.GetSize().y + 4.0f), 0.0f) + this->GetPosition());
    m_Exhaust.SetEnabled  (fStrength ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Exhaust.Move();
}