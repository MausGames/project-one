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
: m_iInputIndex    (0u)
, m_iScoreMission  (0u)
, m_fChainCooldown (0.0f)
, m_vNewPos        (coreVector2(0.0f,0.0f))
{
    // load object resources
    this->DefineProgram("object_ship_program");
    this->DefineTexture(0u, "ship_player.png");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // set initial status
    m_iStatus = PLAYER_STATUS_DEAD;
    m_iMaxHealth = m_iCurHealth = 100;

    // load first weapons
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
    {
        m_apWeapon[i] = new cNoWeapon();
        m_apWeapon[i]->SetOwner(this);
    }

    // reset scoring stats
    this->ResetStats();

    // 
    m_Exhaust.DefineProgram("effect_energy_direct_program");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineModel  ("object_tube.md3");
    m_Exhaust.SetDirection (this->GetDirection());
    m_Exhaust.SetColor4    (coreVector4(0.306f,0.527f,1.0f,0.7f));
    m_Exhaust.SetTexSize   (coreVector2(0.5f,  0.25f));
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

    // 
    this->SetExhaust(0.0f);
}


// ****************************************************************
// configure the player
void cPlayer::Configure(const coreUintW& iShipType, const coreVector3& vColor, const coreUintW& iInputIndex)
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
    this->DefineModel   (pcModelHigh);
    this->DefineModelLow(pcModelLow);
    this->GetModel().GetHandle()->OnLoadOnce([&]()
    {
        // normalize collision size of different ship models
        this->SetCollisionModifier((coreVector3(1.0f,1.0f,1.0f) * PLAYER_COLLISION_SIZE) / this->GetModel()->GetBoundingRange());
    });

    // save color value
    this->SetBaseColor(vColor);

    // save input index
    m_iInputIndex = iInputIndex;
    WARN_IF(m_iInputIndex >= INPUT_SETS) m_iInputIndex = 0u;
}


// ****************************************************************
// equip new main weapon
void cPlayer::EquipWeapon(const coreUintW& iIndex, const coreInt32& iID)
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
    if(CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;

    // 
    this->_UpdateBlink();

    // render the 3d-object
    coreObject3D::Render();

    // 
    m_Exhaust.Render();
}


// ****************************************************************
// move the player
void cPlayer::Move()
{
    if(CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;

    if(!CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_NO_INPUT_MOVE))
    {
        // move the ship
        const coreFloat fSpeed = CONTAINS_BIT(g_aInput[m_iInputIndex].iButtonHold, 0u) ? 20.0f : 50.0f;
        m_vNewPos += g_aInput[m_iInputIndex].vMove * Core::System->GetTime() * fSpeed;

        // restrict movement to the foreground area
             if(m_vNewPos.x < -FOREGROUND_AREA.x) m_vNewPos.x = -FOREGROUND_AREA.x;
        else if(m_vNewPos.x >  FOREGROUND_AREA.x) m_vNewPos.x =  FOREGROUND_AREA.x;
             if(m_vNewPos.y < -FOREGROUND_AREA.y) m_vNewPos.y = -FOREGROUND_AREA.y;
        else if(m_vNewPos.y >  FOREGROUND_AREA.y) m_vNewPos.y =  FOREGROUND_AREA.y;

        // calculate smooth position-offset
        const coreVector2 vDiff   = m_vNewPos - this->GetPosition().xy();
        const coreVector2 vOffset = vDiff * Core::System->GetTime() * 40.0f;

        // set new position and orientation
        this->SetPosition   (coreVector3(vOffset + this->GetPosition().xy(), 0.0f));
        this->SetOrientation(coreVector3(CLAMP(vDiff.x, -0.6f, 0.6f), 0.0f, 1.0f).Normalize());
    }

    // move the 3d-object
    coreObject3D::Move();

    // 
    if(m_fChainCooldown)
    {
        if(!(m_fChainCooldown = MAX(m_fChainCooldown - Core::System->GetTime(), 0.0f)))
            this->TransferChain();
    }

    // update the weapons (shooting and stuff)
    const coreBool bArmed = CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_NO_INPUT_WEAPON) ? false : true;
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i)
    {
        const coreBool bShoot  = CONTAINS_BIT(g_aInput[m_iInputIndex].iButtonHold,  i*2u)      ? bArmed : false;
        const coreBool bChange = CONTAINS_BIT(g_aInput[m_iInputIndex].iButtonPress, i*2u + 1u) ? bArmed : false;
        m_apWeapon[i]->Update(bShoot, bChange);
    }
}


// ****************************************************************
// reduce current health
void cPlayer::TakeDamage(const coreInt32& iDamage)
{
    // 
    g_pGame->GetCombatText()->AddDamage(iDamage, this->GetPosition());

    // 
    this->TransferChain();
    this->ReduceCombo();

    // 
    if(this->_TakeDamage(iDamage))
    {

    }
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect(const coreVector2& vPosition)
{
    // resurrect player
    if(!CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;
    REMOVE_VALUE(m_iStatus, PLAYER_STATUS_DEAD)

    // 
    m_vNewPos = vPosition;

    // add ship to the game
    cShip::_Resurrect(vPosition, coreVector2(0.0f,1.0f), TYPE_PLAYER);
}


// ****************************************************************
// remove player from the game
void cPlayer::Kill(const coreBool& bAnimated)
{
    // kill player
    if(CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;
    ADD_VALUE(m_iStatus, PLAYER_STATUS_DEAD)

    // reset weapon shoot status
    for(coreUintW i = 0u; i < PLAYER_WEAPONS; ++i) m_apWeapon[i]->Update(false, false);

    // remove ship from the game
    cShip::_Kill(bAnimated);
}


// ****************************************************************
// 
void cPlayer::AddScore(const coreUint32& iValue, const coreBool& bModified)
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
void cPlayer::AddCombo(const coreUint32& iValue)
{
    const coreUint32 iOld = F_TO_UI(this->GetCurCombo());

    // 
    m_iComboValue[0] += iValue;
    m_iComboValue[1]  = MAX(m_iComboValue[0], m_iComboValue[1]);

    const coreUint32 iNew = F_TO_UI(this->GetCurCombo());

    // 
    if(iOld != iNew) g_pGame->GetCombatText()->AddCombo(iNew, this->GetPosition());
}

void cPlayer::AddCombo(const coreFloat& fModifier)
{

}


// ****************************************************************
// 
void cPlayer::AddChain(const coreUint32& iValue)
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
void cPlayer::SetExhaust(const coreFloat& fStrength)
{
    // 
    const coreFloat fLen  = fStrength * 40.0f;
    const coreFloat fSize = 1.0f -  fStrength * 0.25f;

    // 
         if( fStrength && !m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) g_pGlow->BindObject  (&m_Exhaust);
    else if(!fStrength &&  m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) g_pGlow->UnbindObject(&m_Exhaust);

    // 
    m_Exhaust.SetSize     (coreVector3(fSize, fLen, fSize) * 0.6f);
    m_Exhaust.SetTexOffset(coreVector2(0.0f, coreFloat(Core::System->GetTotalTime()) * 0.75f));
    m_Exhaust.SetPosition (this->GetPosition() + coreVector3(0.0f, -(4.0f + m_Exhaust.GetSize().y), 0.0f));
    m_Exhaust.SetEnabled  (fStrength ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Exhaust.Move();
}