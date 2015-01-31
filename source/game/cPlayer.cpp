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
: m_pWeapon     (NULL)
, m_iInputIndex (0)
{
    // load object resources
    this->DefineProgram("object_ship_program");
    this->DefineTexture(0, "ship_player.png");

    // set object properties
    this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation(coreVector3(0.0f,0.0f,1.0f));

    // set initial status
    m_iStatus = PLAYER_STATUS_DEAD;

    // load first weapon
    m_pWeapon = new cNoWeapon();
    m_pWeapon->SetOwner(this);
}


// ****************************************************************
// destructor
cPlayer::~cPlayer()
{
    // remove player from the game
    this->Kill(false);

    // 
    SAFE_DELETE(m_pWeapon)
}


// ****************************************************************
// configure the player
void cPlayer::Configure(const coreByte& iAppearanceType, const coreVector3& vColor, const coreByte& iInputIndex)
{
    // select appearance type
    const char* pcModelHigh;
    const char* pcModelLow;
    switch(iAppearanceType)
    {
    default: ASSERT(false)
    case 0: pcModelHigh = "ship_player_off_high.md3"; pcModelLow = "ship_player_off_low.md3"; break;
    case 1: pcModelHigh = "ship_player_def_high.md3"; pcModelLow = "ship_player_def_low.md3"; break;
    }

    // load models
    this->DefineModel   (pcModelHigh);
    this->DefineModelLow(pcModelLow);

    // save color value (may also be overridden outside)
    this->SetColor3(vColor);

    // save input index
    m_iInputIndex = iInputIndex;
    WARN_IF(m_iInputIndex >= INPUT_SETS) m_iInputIndex = 0;
}


// ****************************************************************
// 
void cPlayer::EquipWeapon(const int& iID)
{
    if(m_pWeapon) if(m_pWeapon->GetID() == iID) return;

    // delete possible old weapon
    SAFE_DELETE(m_pWeapon)

    // create new weapon
    switch(iID)
    {
    default: ASSERT(false)
    case cNoWeapon   ::ID: m_pWeapon = new cNoWeapon   (); break;
    case cRayWeapon  ::ID: m_pWeapon = new cRayWeapon  (); break;
    case cPulseWeapon::ID: m_pWeapon = new cPulseWeapon(); break;
    case cWaveWeapon ::ID: m_pWeapon = new cWaveWeapon (); break;
    case cTeslaWeapon::ID: m_pWeapon = new cTeslaWeapon(); break;
    case cAntiWeapon ::ID: m_pWeapon = new cAntiWeapon (); break;
    }
    m_pWeapon->SetOwner(this);
}


// ****************************************************************
// render the player
void cPlayer::Render()
{
    if(m_iStatus & PLAYER_STATUS_DEAD) return;

    // render the 3d-object
    coreObject3D::Render();
}


// ****************************************************************
// move the player
void cPlayer::Move()
{
    if(m_iStatus & PLAYER_STATUS_DEAD) return;

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
    this->SetPosition   (this->GetPosition() + coreVector3(vOffset, 0.0f));
    this->SetOrientation(coreVector3(CLAMP(vDiff.x, -0.6f, 0.6f), 0.0f, 1.0f).Normalize());

    // move the 3d-object
    coreObject3D::Move();

    // 
    const bool bShoot = (g_aInput[m_iInputIndex].iButtonHold & BIT(0)) ? true : false;
    m_pWeapon->Update(bShoot);
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect(const coreVector2& vPosition)
{
    // resurrect player
    if(!(m_iStatus & PLAYER_STATUS_DEAD)) return;
    m_iStatus &= ~PLAYER_STATUS_DEAD;

    // reset player properties
    m_vNewPos = vPosition;
    this->SetPosition(coreVector3(vPosition,0.0f));

    // add player to global shadow and outline
    cShadow::BindGlobalObject(this);
    g_pOutlineFull->BindObject(this);

    // enable collision
    this->ChangeType(TYPE_PLAYER);
}


// ****************************************************************
// remove player from the game
void cPlayer::Kill(const bool& bAnimated)
{
    // kill player
    if(m_iStatus & PLAYER_STATUS_DEAD) return;
    m_iStatus |= PLAYER_STATUS_DEAD;

    // remove player from global shadow and outline
    cShadow::UnbindGlobalObject(this);
    g_pOutlineFull->UnbindObject(this);

    // disable collision
    this->ChangeType(0);
}