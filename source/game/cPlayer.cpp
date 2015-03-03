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
: m_iInputIndex    (0)
, m_iScoreMission  (0)
, m_fChainCooldown (0.0f)
{
    // load object resources
    this->DefineProgram("object_ship_program");
    this->DefineTexture(0, "ship_player.png");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // set initial status
    m_iStatus = PLAYER_STATUS_DEAD;
    m_iMaxHealth = m_iCurHealth = 100;

    // load first weapons
    for(int i = 0; i < PLAYER_WEAPONS; ++i)
    {
        m_apWeapon[i] = new cNoWeapon();
        m_apWeapon[i]->SetOwner(this);
    }

    // reset scoring stats
    this->ResetStats();
}


// ****************************************************************
// destructor
cPlayer::~cPlayer()
{
    // remove player from the game
    this->Kill(false);

    // delete weapon objects
    for(int i = 0; i < PLAYER_WEAPONS; ++i)
        SAFE_DELETE(m_apWeapon[i])
}


// ****************************************************************
// configure the player
void cPlayer::Configure(const coreByte& iShipType, const coreVector3& vColor, const coreByte& iInputIndex)
{
    // select appearance type
    const char* pcModelHigh;
    const char* pcModelLow;
    switch(iShipType)
    {
    default: ASSERT(false)
    case PLAYER_SHIP_OFF: pcModelHigh = "ship_player_off_high.md3"; pcModelLow = "ship_player_off_low.md3"; break;
    case PLAYER_SHIP_DEF: pcModelHigh = "ship_player_def_high.md3"; pcModelLow = "ship_player_def_low.md3"; break;
    }

    // load models
    this->DefineModel   (pcModelHigh);
    this->DefineModelLow(pcModelLow);

    // save color value
    this->SetBaseColor(vColor);

    // save input index
    m_iInputIndex = iInputIndex;
    WARN_IF(m_iInputIndex >= INPUT_SETS) m_iInputIndex = 0;
}


// ****************************************************************
// equip new main weapon
void cPlayer::EquipWeapon(const coreByte& iIndex, const int& iID)
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
    for(int i = 0; i < PLAYER_WEAPONS; ++i)
        for(int j = i+1; j < PLAYER_WEAPONS; ++j)
            ASSERT(m_apWeapon[i]->GetID() != m_apWeapon[j]->GetID())

#endif
}


// ****************************************************************
// render the player
void cPlayer::Render()
{
    if(CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;

    // render the 3d-object
    coreObject3D::Render();
}


// ****************************************************************
// move the player
void cPlayer::Move()
{
    if(CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;

    if(!CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_NO_INPUT_MOVE))
    {
        // move the ship
        m_vNewPos += g_aInput[m_iInputIndex].vMove * Core::System->GetTime() * 50.0f;

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

    // update the weapons (shooting and stuff)
    for(int i = 0; i < PLAYER_WEAPONS; ++i)
    {
        const bool bShoot = CONTAINS_BIT(g_aInput[m_iInputIndex].iButtonHold, i) ? !CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_NO_INPUT_WEAPON) : false;
        m_apWeapon[i]->Update(bShoot);
    }
}


// ****************************************************************
// reduce current health
void cPlayer::TakeDamage(const int& iDamage)
{
    // 
    m_iCurHealth -= iDamage;

    // 
    if(m_iCurHealth <= 0)
    {
        m_iCurHealth = 0;
    }

    // 
    this->SetColor3(LERP(coreVector3(0.5f,0.5f,0.5f), this->GetBaseColor(), (I_TO_F(m_iCurHealth) / I_TO_F(m_iMaxHealth))));
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect(const coreVector2& vPosition)
{
    // resurrect player
    if(!CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;
    REMOVE_VALUE(m_iStatus, PLAYER_STATUS_DEAD)

    // add ship to the game
    cShip::_Resurrect(vPosition, TYPE_PLAYER);
}


// ****************************************************************
// remove player from the game
void cPlayer::Kill(const bool& bAnimated)
{
    // kill player
    if(CONTAINS_VALUE(m_iStatus, PLAYER_STATUS_DEAD)) return;
    ADD_VALUE(m_iStatus, PLAYER_STATUS_DEAD)

    // reset weapon shoot status
    for(int i = 0; i < PLAYER_WEAPONS; ++i) m_apWeapon[i]->Update(false);

    // remove ship from the game
    cShip::_Kill(bAnimated);
}


// ****************************************************************
// 
void cPlayer::AddScore(const coreUint& iValue, const bool& bModified)
{

}


// ****************************************************************
// 
void cPlayer::AddCombo(const coreUint& iValue)
{

}

void cPlayer::AddCombo(const float& fModifier)
{

}


// ****************************************************************
// 
void cPlayer::AddChain(const coreUint& iValue)
{

}


// ****************************************************************
// reset scoring stats
void cPlayer::ResetStats()
{
    // 
    m_iScoreMission = 0;
    std::memset(m_aiScoreBoss, 0, sizeof(m_aiScoreBoss));

    // 
    m_iComboValue[1] = m_iComboValue[0] = 0;
    m_iChainValue[1] = m_iChainValue[0] = 0;
    m_fChainCooldown = 0.0f;
}