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
: m_pInput         (&g_TotalInput)
, m_vForce         (coreVector2(0.0f,0.0f))
, m_fRollTime      (0.0f)
, m_iRollDir       (PLAYER_NO_ROLL)
, m_fChainCooldown (0.0f)
, m_fAnimation     (0.0f)
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
    m_Wind.DefineModel  ("object_sphere.md3");
    m_Wind.DefineTexture(0u, "effect_energy.png");
    m_Wind.DefineProgram("effect_energy_direct_program");
    m_Wind.SetColor4    (coreVector4(COLOR_ENERGY_BLUE * 1.6f, 0.0f));
    m_Wind.SetTexSize   (coreVector2(1.0f,5.0f));
    m_Wind.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

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
void cPlayer::Configure(const coreUintW iShipType, const coreVector3& vColor)
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
    this->GetModel().OnUsableOnce([this]()
    {
        // normalize collision size of different ship models
        this->SetCollisionModifier((coreVector3(1.0f,1.0f,1.0f) * PLAYER_COLLISION_SIZE) / this->GetModel()->GetBoundingRange());
    });

    // save color value
    this->SetBaseColor(vColor);
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
            ASSERT((m_apWeapon[i]->GetID() != m_apWeapon[j]->GetID()) ||
                   (m_apWeapon[i]->GetID() == cNoWeapon::ID))

#endif
}


// ****************************************************************
// render the player
void cPlayer::Render()
{
    if(!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
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
        this->coreObject3D::Render();

        // 
        for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
            m_apWeapon[i]->Render();

        // 
        if(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_RENDER))
        {
            g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(this);
            m_Wind.Render();
        }
    }
}


// ****************************************************************
// move the player
void cPlayer::Move()
{
    // 
    this->_UpdateAlwaysBefore();

    if(!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        if(!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_ROLL))
        {
            // 
            if(CONTAINS_BIT(m_pInput->iActionPress, PLAYER_WEAPONS * WEAPON_MODES))
                if(m_fRollTime <= 0.0f) this->StartRolling(m_pInput->vMove);
        }

        // 
        if(m_fRollTime >= 1.0f) this->EndRolling();

        // 
        m_fRollTime.Update(this->IsRolling() ? PLAYER_ROLL_SPEED : -PLAYER_ROLL_COOLDOWN);
        m_fRollTime = CLAMP(m_fRollTime, 0.0f, 1.0f);

        if(!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_MOVE))
        {
            // 
            coreVector2 vNewPos = this->GetPosition().xy();
            if(this->IsRolling())
            {
                // roll the ship
                const coreFloat fSpeed = 50.0f + 80.0f * SIN(PI * m_fRollTime);
                vNewPos += UnpackDirection(m_iRollDir) * (Core::System->GetTime() * fSpeed);
            }
            else
            {
                // move the ship
                const coreFloat fSpeed = (!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_SHOOT) && CONTAINS_BIT(m_pInput->iActionHold, 0u)) ? 20.0f : 50.0f;
                vNewPos += m_pInput->vMove * (Core::System->GetTime() * fSpeed);
            }

            // apply external forces
            if(!m_vForce.IsNull())
            {
                vNewPos  += m_vForce * Core::System->GetTime();
                m_vForce *= 1.0f - 3.0f * Core::System->GetTime();
            }

            // restrict movement to the foreground area
                 if(vNewPos.x < -FOREGROUND_AREA.x) {vNewPos.x = -FOREGROUND_AREA.x; m_vForce.x =  ABS(m_vForce.x);}
            else if(vNewPos.x >  FOREGROUND_AREA.x) {vNewPos.x =  FOREGROUND_AREA.x; m_vForce.x = -ABS(m_vForce.x);}
                 if(vNewPos.y < -FOREGROUND_AREA.y) {vNewPos.y = -FOREGROUND_AREA.y; m_vForce.y =  ABS(m_vForce.y);}
            else if(vNewPos.y >  FOREGROUND_AREA.y) {vNewPos.y =  FOREGROUND_AREA.y; m_vForce.y = -ABS(m_vForce.y);}

            // 
            const coreVector2 vDiff = vNewPos - this->GetPosition().xy();
            coreVector2 vOri = coreVector2(CLAMP(vDiff.x, -0.6f, 0.6f), 1.0f).Normalized();

            // 
            if(this->IsRolling())
            {
                const coreFloat fAngle = LERPS(0.0f, 2.0f*PI, m_fRollTime);
                vOri *= coreMatrix3::Rotation(fAngle * ((m_iRollDir & 0x04u) ? 1.0f : -1.0f)).m12();
            }

            // set new position and orientation
            this->SetPosition   (coreVector3(vNewPos, 0.0f));
            this->SetOrientation(coreVector3(vOri.x, 0.0f, vOri.y));
        }

        // move the 3d-object
        this->coreObject3D::Move();

        // update all weapons (shooting and stuff)
        for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
        {
            const coreUint8 iShoot = (!this->IsRolling() && !CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_SHOOT)) ? ((m_pInput->iActionHold & (BITLINE(WEAPON_MODES) << (i*WEAPON_MODES))) >> (i*WEAPON_MODES)) : 0u;
            m_apWeapon[i]->Update(iShoot);
        }

        // 
        if(m_fChainCooldown)
        {
            if(!(m_fChainCooldown = MAX(m_fChainCooldown - Core::System->GetTime(), 0.0f)))
                this->TransferChain();
        }

        // 
        m_fAnimation.UpdateMod(1.0f, 20.0f);

        if(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_Wind.SetAlpha((m_Wind.GetAlpha() < 0.0f) ? 0.5f : COS((0.5f*PI) * m_fRollTime));

            // 
            m_Wind.SetPosition (this->GetPosition());
            m_Wind.SetSize     (coreVector3(1.0f,1.08f,1.0f) * PLAYER_WIND_SIZE);
            m_Wind.SetTexOffset(coreVector2(0.0f, m_fAnimation * ((m_iRollDir & 0x04u) ? 0.4f : -0.4f)));
            m_Wind.Move();
        }

        if(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            this->SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.25f));

            // 
            if(CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS))
                 m_Bubble.SetAlpha(MIN(m_Bubble.GetAlpha() + 4.0f*Core::System->GetTime(), 0.8f));
            else m_Bubble.SetAlpha(MAX(m_Bubble.GetAlpha() - 4.0f*Core::System->GetTime(), 0.0f));

            // 
            if(!m_Bubble.GetAlpha()) this->DisableBubble();

            // 
            m_Bubble.SetPosition (this->GetPosition());
            m_Bubble.SetSize     (coreVector3(1.0f,1.0f,1.0f) * PLAYER_BUBBLE_SIZE * m_Bubble.GetAlpha());
            m_Bubble.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.1f));
            m_Bubble.Move();
        }
    }

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// reduce current health
coreBool cPlayer::TakeDamage(const coreInt32 iDamage, const coreUint8 iElement)
{
    if(iDamage > 0)
    {
        // 
        //g_pGame->GetCombatText()->AddDamage(iDamage, this->GetPosition());   

        // 
        this->TransferChain();
        this->ReduceCombo();
    }

    // 
    if(this->_TakeDamage(iDamage, iElement))
    {
        this->Kill(true);
        return true;
    }

    return false;
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect(const coreVector2& vPosition)
{
    // resurrect player
    if(!CONTAINS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DEAD)

    // 
    this->SetPosition(coreVector3(vPosition, 0.0f));

    // add ship to the game
    this->_Resurrect(true, vPosition, coreVector2(0.0f,1.0f), TYPE_PLAYER);
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
    this->EndRolling();
    this->TransformDark(PLAYER_DARK_RESET);

    // 
    this->DisableWind();
    this->DisableBubble();
    this->UpdateExhaust(0.0f);

    // 
    if(bAnimated) g_pSpecialEffects->MacroExplosionPhysicalBig(this->GetPosition());

    // remove ship from the game
    this->_Kill(true, bAnimated);
}


// ****************************************************************
// 
void cPlayer::AddScore(const coreUint32 iValue, const coreBool bModified)
{
    const coreUint32 iFinalValue = bModified ? (iValue * this->GetCurCombo()) : iValue;

    // 
    m_iScoreGame    += iFinalValue;
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
    m_iScoreGame    = 0u;
    m_iScoreMission = 0u;
    for(coreUintW i = 0u; i < MISSION_BOSSES; ++i) m_aiScoreBoss[i] = 0u;

    // 
    m_iComboValue[1] = m_iComboValue[0] = 0u;
    m_iChainValue[1] = m_iChainValue[0] = 0u;
    m_fChainCooldown = 0.0f;
}


// ****************************************************************
// 
void cPlayer::StartRolling(const coreVector2& vDirection)
{
    WARN_IF(this->IsRolling()) return;

    // 
    m_fRollTime = 0.0f;
    m_iRollDir  = PackDirection(vDirection);

    // 
    this->ChangeType(TYPE_PLAYER_ROLL);
    this->EnableWind();
}


// ****************************************************************
// 
void cPlayer::EndRolling()
{
    if(!this->IsRolling()) return;

    // 
    m_fRollTime = 1.0f;
    m_iRollDir  = PLAYER_NO_ROLL;

    // 
    this->ChangeType(TYPE_PLAYER);
    this->DisableWind();
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
        g_pSpecialEffects->CreateBlast     (this->GetPosition(), SPECIAL_BLAST_SMALL, COLOR_ENERGY_WHITE);

        // 
        this->EnableBubble();
    }
    else if(iStatus == PLAYER_DARK_OFF)
    {
        REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS)

        // 
        g_pDistortion    ->CreateWave      (this->GetPosition(), DISTORTION_WAVE_BIG);
        g_pSpecialEffects->CreateSplashDark(this->GetPosition(), SPECIAL_SPLASH_BIG);
        g_pSpecialEffects->CreateBlast     (this->GetPosition(), SPECIAL_BLAST_BIG, COLOR_ENERGY_WHITE);
    }
    else REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DARKNESS)

    // 
    std::swap(m_pDarkProgram, m_pProgram);
}


// ****************************************************************
// 
void cPlayer::EnableWind()
{
    WARN_IF(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Wind.SetAlpha(-1.0f);

    // 
    m_Wind.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Wind);
}


// ****************************************************************
// 
void cPlayer::DisableWind()
{
    if(!m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Wind.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Wind);
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