///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
// constructor
cPlayer::cPlayer()noexcept
: m_apWeapon        {}
, m_pInput          (&g_TotalInput)
, m_vArea           (coreVector4(-FOREGROUND_AREA, FOREGROUND_AREA))
, m_vForce          (coreVector2(0.0f,0.0f))
, m_fSpeed          (1.0f)
, m_fRollTime       (0.0f)
, m_fFeelTime       (PLAYER_NO_FEEL)
, m_fIgnoreTime     (PLAYER_NO_IGNORE)
, m_iRollDir        (PLAYER_NO_ROLL)
, m_iFeelType       (0u)
, m_iIgnoreType     (0u)
, m_fInterrupt      (0.0f)
, m_fLightningTime  (0.0f)
, m_fLightningAngle (0.0f)
, m_fDesaturate     (0.0f)
, m_fAnimation      (0.0f)
, m_iLook           (0u)
{
    // load object resources
    this->DefineTexture(0u, "ship_player.png");
    this->DefineTexture(1u, "menu_background_black.png");

    // set object properties
    this->SetDirection        (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation      (coreVector3(0.0f,0.0f,1.0f));
    this->SetCollisionModifier(coreVector3(0.0f,0.0f,0.0f));
    this->SetTexSize          (coreVector2(1.2f,1.2f));

    // set initial status
    m_iStatus = PLAYER_STATUS_DEAD;

    // 
    this->SetMaxHealth(PLAYER_LIVES);

    // load first weapons
    for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
    {
        m_apWeapon[i] = new cNoWeapon();
        m_apWeapon[i]->SetOwner(this);
    }

    // 
    m_DataTable .SetOwner(this);
    m_ScoreTable.SetOwner(this);

    // 
    m_pNormalProgram = Core::Manager::Resource->Get<coreProgram>("object_ship_program");
    m_pDarkProgram   = Core::Manager::Resource->Get<coreProgram>("object_ship_darkness_program");
    this->ActivateDarkShading();

    // 
    m_Dot.DefineModel("object_dot.md3");
    m_Dot.SetSize    (coreVector3(1.0f,1.0f,1.0f) * PLAYER_COLLISION_MIN);

    // 
    m_Range.DefineModel  ("object_sphere.md3");
    m_Range.DefineTexture(0u, "default_white.png");
    m_Range.DefineProgram("effect_energy_flat_spheric_program");
    m_Range.SetSize      (coreVector3(1.0f,1.0f,1.0f) * 0.55f);
    m_Range.SetColor4    (coreVector4(COLOR_ENERGY_RED * 0.7f, 1.0f));

    // 
    m_Wind.DefineModel  ("object_sphere.md3");
    m_Wind.DefineTexture(0u, "effect_energy.png");
    m_Wind.DefineProgram("effect_energy_direct_program");
    m_Wind.SetColor4    (coreVector4(COLOR_ENERGY_BLUE * 1.6f, 0.0f));
    m_Wind.SetTexSize   (coreVector2(1.0f,5.0f));
    m_Wind.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    
    
    m_Wind2.DefineModel  ("object_sphere.md3");
    m_Wind2.DefineTexture(0u, "effect_energy.png");
    m_Wind2.DefineProgram("effect_energy_direct_program");
    m_Wind2.SetColor4    (coreVector4(COLOR_ENERGY_BLUE * 1.6f, 1.0f));
    m_Wind2.SetTexSize   (coreVector2(1.0f,5.0f));
    
    g_pGlow->BindObject(&m_Wind2);
    //m_Wind2.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Bubble.DefineModel  ("object_sphere.md3");
    m_Bubble.DefineTexture(0u, "effect_energy.png");
    m_Bubble.DefineProgram("effect_energy_flat_spheric_program");
    m_Bubble.SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.5f, 0.0f));
    m_Bubble.SetTexSize   (coreVector2(5.0f,5.0f));
    m_Bubble.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Shield.DefineModel  ("effect_shield.md3");
    m_Shield.DefineTexture(0u, "effect_particle_128.png");
    m_Shield.DefineProgram("effect_shield_program");
    m_Shield.SetSize      (coreVector3(4.7f,4.7f,4.7f));
    m_Shield.SetColor4    (coreVector4(COLOR_ENERGY_BLUE, 0.0f));
    m_Shield.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Exhaust.DefineModel  ("object_tube_open.md3");
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
    for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
        SAFE_DELETE(m_apWeapon[i])
        
        
    g_pGlow->UnbindObject(&m_Wind2);
}


// ****************************************************************
// configure the player
void cPlayer::Configure(const coreUintW iShipType, const coreVector3& vColor)
{
    // select appearance type
    coreHashString sModelHigh;
    coreHashString sModelLow;
    switch(iShipType)
    {
    default: ASSERT(false)
    case PLAYER_SHIP_ATK: sModelHigh = "ship_player_atk_high.md3"; sModelLow = "ship_player_atk_low.md3"; break;
    case PLAYER_SHIP_DEF: sModelHigh = "ship_player_def_high.md3"; sModelLow = "ship_player_def_low.md3"; break;
    case PLAYER_SHIP_P1:  sModelHigh = "ship_projectone.md3";      sModelLow = "ship_projectone.md3";     break;
    }

    // load models
    this->DefineModelHigh(sModelHigh);
    this->DefineModelLow (sModelLow);

    // set color
    this->SetBaseColor(vColor);

    // 
    SET_BITVALUE(m_iLook, 4u, 0u, iShipType)
    SET_BITVALUE(m_iLook, 8u, 4u, m_iBaseColor)
}


// ****************************************************************
// equip new main weapon
void cPlayer::EquipWeapon(const coreUintW iIndex, const coreInt32 iID)
{
    ASSERT(iIndex < PLAYER_EQUIP_WEAPONS)
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
    case cEnemyWeapon::ID: m_apWeapon[iIndex] = new cEnemyWeapon(); break;
    }
    m_apWeapon[iIndex]->SetOwner(this);

    // 
    SET_BITVALUE(m_iLook, 4u, 12u, iID)
    STATIC_ASSERT(PLAYER_EQUIP_WEAPONS == 1u)

#if defined(_CORE_DEBUG_)

    // same weapon should not be equipped twice
    for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
        for(coreUintW j = i+1u; j < PLAYER_EQUIP_WEAPONS; ++j)
            ASSERT((m_apWeapon[i]->GetID() != m_apWeapon[j]->GetID()) ||
                   (m_apWeapon[i]->GetID() == cNoWeapon::ID))

#endif
}


// ****************************************************************
// 
void cPlayer::EquipSupport(const coreUintW iIndex, const coreInt32 iID)
{
    ASSERT(iIndex < PLAYER_EQUIP_SUPPORTS)

    // 
    switch(iID)
    {
    default: ASSERT(false)
    case 0u:                        break;
    case 1u: this->__EquipShield(); break;
    }
}


// ****************************************************************
// render the player
void cPlayer::Render()
{
    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        
        glDisable(GL_DEPTH_TEST);
        m_Wind2.Render();
        glEnable(GL_DEPTH_TEST);
        
        // render the 3d-object
        this->coreObject3D::Render();
    }
}

void cPlayer::RenderBefore()
{
    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        // 
        m_Exhaust.Render();
    }
}

void cPlayer::RenderAfter()
{
    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        // 
        for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
            m_apWeapon[i]->Render();

        // 
        m_Bubble.Render();
        m_Shield.Render();
        m_Wind  .Render();

        // 
        //g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Range);
        //m_Range.Render();
    }
}


// ****************************************************************
// move the player
void cPlayer::Move()
{
    // 
    this->_UpdateAlwaysBefore();

    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        coreVector2 vNewPos = this->GetPosition().xy();
        coreVector3 vNewOri = coreVector3(0.0f,0.0f,1.0f);

        if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_TURN))
        {
            coreVector2 vNewDir = this->GetDirection().xy();

            // 
            if(HAS_BIT(m_pInput->iActionPress, PLAYER_EQUIP_WEAPONS * WEAPON_MODES))
                vNewDir = -vNewDir.Rotated90();
            if(HAS_BIT(m_pInput->iActionPress, PLAYER_EQUIP_WEAPONS * WEAPON_MODES + 1u))
                vNewDir =  vNewDir.Rotated90();

            // set new direction
            this->SetDirection(coreVector3(vNewDir, 0.0f));
        }

        if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_ROLL))
        {
            // 
            //if(HAS_BIT(m_pInput->iActionPress, PLAYER_EQUIP_WEAPONS * WEAPON_MODES))
            //    if(m_fRollTime <= 0.0f) this->StartRolling(m_pInput->vMove);
        }

        if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_MOVE))
        {
            // move the ship
            vNewPos += (m_pInput->vMove * this->CalcMoveSpeed() + m_vForce) * TIME;

            // restrict movement to the foreground area
                 if(vNewPos.x < m_vArea.x) {vNewPos.x = m_vArea.x; m_vForce.x =  ABS(m_vForce.x);}
            else if(vNewPos.x > m_vArea.z) {vNewPos.x = m_vArea.z; m_vForce.x = -ABS(m_vForce.x);}
                 if(vNewPos.y < m_vArea.y) {vNewPos.y = m_vArea.y; m_vForce.y =  ABS(m_vForce.y);}
            else if(vNewPos.y > m_vArea.w) {vNewPos.y = m_vArea.w; m_vForce.y = -ABS(m_vForce.y);}

            // 
            const coreVector2 vDiff = (vNewPos - this->GetPosition().xy()) * RCP(TIME * FRAMERATE_MIN + CORE_MATH_PRECISION);
            vNewOri = coreVector3(CLAMP(vDiff.x, -0.6f, 0.6f), CLAMP(vDiff.y, -0.6f, 0.6f), 1.0f).NormalizedUnsafe();
        }

        // 
        if(m_fRollTime >= 1.0f) this->EndRolling();

        // 
        m_fRollTime.Update(this->IsRolling() ? PLAYER_ROLL_SPEED : -PLAYER_ROLL_COOLDOWN);
        m_fRollTime = CLAMP(m_fRollTime, 0.0f, 1.0f);

        // 
        m_vForce *= FrictionFactor(8.0f);

        if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_ALL))
        {
            // 
            if(this->IsRolling())
            {
                const coreFloat fAngle = LERPB(0.0f, 4.0f*PI, m_fRollTime);
                const coreFloat fSide  = -SIGN(coreVector2::Dot(-this->GetDirection().xy().Rotated90(), UnpackDirection(m_iRollDir)));
                vNewOri *= coreMatrix4::RotationAxis(fAngle * fSide, this->GetDirection()).m123();
            }

            // set new position and orientation
            this->SetPosition   (coreVector3(vNewPos, 0.0f));
            this->SetOrientation(vNewOri);
        }

        // normalize collision size
        if(this->GetModel().IsUsable())
        {
            const coreFloat fRadius = MAX(this->GetMove().Length(), PLAYER_COLLISION_MIN);
            this->SetCollisionModifier((coreVector3(1.0f,1.0f,1.0f) * fRadius) / this->GetModel()->GetBoundingRange());
        }

        // 
        m_fAnimation.UpdateMod(1.0f, 20.0f);
        this->SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.25f));

        // move the 3d-object
        this->coreObject3D::Move();

        // update all weapons (shooting and stuff)
        for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
        {
            const coreUint8 iShoot = (!this->IsRolling() && !HAS_FLAG(m_iStatus, PLAYER_STATUS_PACIFIST) && !HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_SHOOT) && !m_fInterrupt) ? ((m_pInput->iActionHold & (BITLINE(WEAPON_MODES) << (i*WEAPON_MODES))) >> (i*WEAPON_MODES)) : 0u;
            m_apWeapon[i]->Update(iShoot);
        }

        // 
        m_Dot.SetPosition(this->GetPosition());
        m_Dot.Move();

        // 
        m_Range.SetPosition(this->GetPosition());
        m_Range.Move();

        if(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_Wind.SetAlpha((m_Wind.GetAlpha() < 0.0f) ? 0.5f : COS((0.5f*PI) * m_fRollTime));

            // 
            m_Wind.SetPosition (this->GetPosition());
            m_Wind.SetSize     (coreVector3(1.0f,1.08f,1.0f) * PLAYER_WIND_SIZE * LERP(1.0f, 1.5f, POW3(m_fRollTime)));
            m_Wind.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.4f));
            m_Wind.Move();
        }

        // 
        m_Wind2.SetPosition (this->GetPosition());
        m_Wind2.SetSize     (coreVector3(1.0f,1.08f,1.0f) * PLAYER_WIND_SIZE * 1.0f);
        m_Wind2.SetDirection(this->GetDirection() * -1.0f);
        m_Wind2.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.3f));
        m_Wind2.Move();

        if(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_fFeelTime.Update(-1.0f);

            // 
            if(m_fFeelTime > 0.0f) m_Bubble.SetAlpha(MIN(m_Bubble.GetAlpha() + 4.0f*TIME, 0.8f));
                              else m_Bubble.SetAlpha(MAX(m_Bubble.GetAlpha() - 4.0f*TIME, 0.0f));

            // 
            if(!m_Bubble.GetAlpha()) this->EndFeeling();

            // 
            m_Bubble.SetPosition (this->GetPosition());
            m_Bubble.SetSize     (coreVector3(1.0f,1.0f,1.0f) * PLAYER_BUBBLE_SIZE * m_Bubble.GetAlpha());
            m_Bubble.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.1f));
            m_Bubble.Move();
        }

        if(m_Shield.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_fIgnoreTime.Update(-2.0f);

            // 
            if(m_fIgnoreTime <= 0.0f) this->EndIgnoring();

            // 
            const coreVector2 vDir       = coreVector2::Direction(m_fAnimation * (0.3f*PI));
            const coreFloat   fBounce    = 0.25f + 0.2f * m_fIgnoreTime;
            const coreFloat   fExplosion = m_iIgnoreType ? ((1.0f - POW3(m_fIgnoreTime)) * 2.0f) : 0.0f;

            // 
            m_Shield.SetPosition   (this->GetPosition());
            m_Shield.SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
            m_Shield.SetTexOffset  (coreVector2(fBounce + fExplosion, 0.0f));
            m_Shield.SetAlpha      (MIN(POW2(m_fIgnoreTime) * 1.4f, 1.0f));
            m_Shield.Move();
        }

        // 
        m_fInterrupt.UpdateMax(-1.0f, 0.0f);
        if(m_fInterrupt)
        {
            // 
            m_fLightningTime.Update(10.0f);
            if(m_fLightningTime >= 1.0f)
            {
                m_fLightningTime -= 1.0f;

                // 
                m_fLightningAngle = FMOD(m_fLightningAngle + DEG_TO_RAD(145.0f), DEG_TO_RAD(360.0f));
                const coreVector2 vDir = coreVector2::Direction(m_fLightningAngle);

                // 
                g_pSpecialEffects->CreateLightning(this, vDir, 7.5f, SPECIAL_LIGHTNING_SMALL, coreVector3(1.0f,1.0f,1.0f), coreVector2(1.0f,1.0f), 0.0f);
            }
        }

        // 
        m_fDesaturate.UpdateMax(-1.0f, 0.0f);
    }

    // 
    m_DataTable .Update();
    m_ScoreTable.Update();

    // 
    this->__UpdateCollisions();

    // 
    this->_UpdateAlwaysAfter();
}


// ****************************************************************
// reduce current health
coreInt32 cPlayer::TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2& vImpact)
{
    if(iDamage > 0)
    {
        // 
        m_ScoreTable.CancelCombo();

        // 
        const coreInt32 iTaken = this->_TakeDamage(1, iElement, vImpact);

        if(m_iCurHealth)
        {
            // 
            if(!this->IsDarkShading()) this->RefreshColor();

            if(HAS_FLAG(m_iStatus, PLAYER_STATUS_SHIELDED))
            {
                // 
                this->SetDesaturate(PLAYER_DESATURATE);
                this->StartIgnoring((m_iCurHealth == 1) ? 1u : 0u);
            }
            else
            {
                // 
                this->SetDesaturate(PLAYER_DESATURATE);
                this->StartFeeling (PLAYER_FEEL_TIME, 0u);
            }

            // 
            //g_pSpecialEffects->FreezeScreen(12.0f / FRAMERATE_MIN);

            // 
            m_fInterrupt = 0.0f;

            // 
            m_DataTable.EditCounterTotal  ()->iDamageTaken += iTaken;
            m_DataTable.EditCounterMission()->iDamageTaken += iTaken;
            m_DataTable.EditCounterSegment()->iDamageTaken += iTaken;

            // 
            g_pSave->EditGlobalStats      ()->iDamageTaken += iTaken;
            g_pSave->EditLocalStatsMission()->iDamageTaken += iTaken;
            g_pSave->EditLocalStatsSegment()->iDamageTaken += iTaken;
        }
        else
        {
            // 
            this->Kill(true);
        }

        return iTaken;
    }

    return 0;
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect()
{
    // resurrect player
    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DEAD)

    // add ship to global shadow and outline
    cShadow::GetGlobalContainer()->BindObject(this);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);

    // enable collision
    this->ChangeType(TYPE_PLAYER);

    // add ship to the game
    this->_Resurrect();
}


// ****************************************************************
// remove player from the game
void cPlayer::Kill(const coreBool bAnimated)
{
    // kill player
    if(HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;
    ADD_FLAG(m_iStatus, PLAYER_STATUS_DEAD)

    // reset weapon shoot status
    for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
        m_apWeapon[i]->Update(0u);

    // 
    this->EndRolling();
    this->EndFeeling();
    this->EndIgnoring();

    // 
    this->DisableWind();
    this->DisableBubble();
    this->DisableShield();
    this->UpdateExhaust(0.0f);

    // 
    m_fInterrupt      = 0.0f;
    m_fLightningTime  = 0.0f;
    m_fLightningAngle = 0.0f;

    // 
    if(bAnimated && this->IsEnabled(CORE_OBJECT_ENABLE_RENDER))
        g_pSpecialEffects->MacroExplosionPhysicalDarkBig(this->GetPosition());

    // remove ship from global shadow and outline
    cShadow::GetGlobalContainer()->UnbindObject(this);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->UnbindObject(this);

    // disable collision
    this->ChangeType(0);

    // remove ship from the game
    this->_Kill(bAnimated);
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
    this->EnableWind(vDirection);
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
    this->DisableWind();
}


// ****************************************************************
// 
void cPlayer::StartFeeling(const coreFloat fTime, const coreUint8 iType)
{
    WARN_IF(this->IsFeeling()) return;

    // 
    m_fFeelTime = fTime;
    m_iFeelType = iType;

    // 
    this->EnableBubble();

    // 
         if(iType == 0u) g_pSpecialEffects->MacroExplosionPhysicalDarkBig  (this->GetPosition());
    else if(iType == 1u) g_pSpecialEffects->MacroExplosionPhysicalDarkSmall(this->GetPosition());
}


// ****************************************************************
// 
void cPlayer::EndFeeling()
{
    if(!this->IsFeeling()) return;

    // 
    m_fFeelTime = PLAYER_NO_FEEL;
    m_iFeelType = 0u;

    // 
    this->DisableBubble();
}


// ****************************************************************
// 
void cPlayer::StartIgnoring(const coreUint8 iType)
{
    WARN_IF(this->IsIgnoring()) return;

    // 
    m_fIgnoreTime = 1.0f;
    m_iIgnoreType = iType;

    // 
    this->EnableShield();

    // 
    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

    // 
    if(iType == 1u)
    {
        this->SetDesaturate(PLAYER_DESATURATE);
        this->StartFeeling (PLAYER_FEEL_TIME_SHIELD, 2u);
    }
}


// ****************************************************************
// 
void cPlayer::EndIgnoring()
{
    if(!this->IsIgnoring()) return;

    // 
    m_fIgnoreTime = PLAYER_NO_IGNORE;
    m_iIgnoreType = 0u;

    // 
    this->DisableShield();
}


// ****************************************************************
// 
void cPlayer::TurnIntoEnemy()
{
    ASSERT(!this->IsEnemyLook())
    
    
    this->DefineModelHigh("ship_enemy_warrior_high.md3");
    this->DefineModelLow ("ship_enemy_warrior_low.md3");
    this->DefineTexture  (0u, "ship_enemy.png");
    
    this->SetBaseColor(COLOR_SHIP_MAGENTA);           
    SAFE_DELETE(m_apWeapon[0])
    m_apWeapon[0] = new cEnemyWeapon();
    m_apWeapon[0]->SetOwner(this);
    //this->EquipWeapon(0u, cEnemyWeapon::ID);           // <- bad overwrite
    this->ActivateNormalShading();           
    
}


// ****************************************************************
// 
void cPlayer::TurnIntoPlayer()
{
    ASSERT(this->IsEnemyLook())

    this->DefineTexture  (0u, "ship_player.png");

    this->Configure(GET_BITVALUE(m_iLook, 4u, 0u), coreVector4::UnpackUnorm4x8(GET_BITVALUE(m_iLook, 8u, 4u)).xyz());
    this->EquipWeapon(0u, GET_BITVALUE(m_iLook, 4u, 12u));
    this->ActivateDarkShading();
}


// ****************************************************************
// 
void cPlayer::EnableWind(const coreVector2& vDirection)
{
    WARN_IF(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    const coreFloat fSide = -SIGN(coreVector2::Dot(-this->GetDirection().xy().Rotated90(), vDirection));

    // 
    m_Wind.SetDirection(this->GetDirection() * fSide);
    m_Wind.SetAlpha    (-1.0f);

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
    m_Bubble.SetAlpha(0.0f);

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
void cPlayer::EnableShield()
{
    WARN_IF(m_Shield.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Shield.SetAlpha(0.0f);

    // 
    m_Shield.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Shield);
}


// ****************************************************************
// 
void cPlayer::DisableShield()
{
    if(!m_Shield.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Shield.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Shield);
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
    m_Exhaust.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.75f));
    m_Exhaust.SetPosition (this->GetPosition () - this->GetDirection() * (m_Exhaust.GetSize().y + 4.0f));
    m_Exhaust.SetDirection(this->GetDirection());
    m_Exhaust.SetEnabled  (fStrength ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Exhaust.Move();
}


// ****************************************************************
// 
coreBool cPlayer::TestCollisionPrecise(const coreObject3D* pObject, coreVector3* OUTPUT pvIntersection, coreBool* OUTPUT pbFirstHit)
{
    ASSERT(pObject && pvIntersection && pbFirstHit)

    // 
    if(Core::Manager::Object->TestCollision(&m_Dot, pObject, pvIntersection))
    {
        // 
        (*pvIntersection) = this->GetPosition();
        (*pbFirstHit)     = this->__NewCollision(pObject);
        return true;
    }

    // 
    const coreVector2 vMove = this->GetMove();
    if(vMove.IsNull())
    {
        // 
        const coreVector3 vRayPos = this->GetPosition();
        const coreVector3 vRayDir = coreVector3(0.0f,0.0f,1.0f);

        // 
        coreFloat   fHitDistance = 0.0f;
        coreVector3 vHitNormal   = coreVector3(0.0f,0.0f,0.0f);
        coreUint8   iHitCount    = 1u;
        if(Core::Manager::Object->TestCollision(pObject, vRayPos, vRayDir, &fHitDistance, &vHitNormal, &iHitCount) && (iHitCount & 0x01u))
        {
            // 
            (*pvIntersection) = this->GetPosition();
            (*pbFirstHit)     = this->__NewCollision(pObject);
            return true;
        }
    }
    else
    {
        // 
        const coreVector3 vRayPos = this->GetPosition();
        const coreVector3 vRayDir = coreVector3(-vMove.NormalizedUnsafe(), 0.0f);

        // 
        coreFloat   fHitDistance = 0.0f;
        coreVector3 vHitNormal   = coreVector3(0.0f,0.0f,0.0f);
        coreUint8   iHitCount    = 1u;
        if(Core::Manager::Object->TestCollision(pObject, vRayPos, vRayDir, &fHitDistance, &vHitNormal, &iHitCount) && ((iHitCount & 0x01u) || (POW2(fHitDistance) < vMove.LengthSq())))
        {
            // 
            (*pvIntersection) = vRayPos + vRayDir * MIN(fHitDistance, vMove.Length());
            (*pbFirstHit)     = this->__NewCollision(pObject);
            return true;
        }
    }
    return false;
}


// ****************************************************************
// 
coreVector2 cPlayer::CalcMove()const
{
    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_MOVE))
    {
        // move the ship
        coreVector2 vNewPos = this->GetPosition().xy() + (m_pInput->vMove * this->CalcMoveSpeed() + m_vForce) * TIME;

        // restrict movement to the foreground area
        vNewPos.x = CLAMP(vNewPos.x, m_vArea.x, m_vArea.z);
        vNewPos.y = CLAMP(vNewPos.y, m_vArea.y, m_vArea.w);

        return vNewPos - this->GetPosition().xy();
    }

    return coreVector2(0.0f,0.0f);
}


// ****************************************************************
// 
coreFloat cPlayer::CalcMoveSpeed()const
{
    static coreFloat fBoost = 0.0f;
    if(HAS_BIT(m_pInput->iActionPress,   0u) ||   // to make movement during quickshots easier
       HAS_BIT(m_pInput->iActionRelease, 0u))     // to make emergency evasion maneuvers easier
        fBoost = 1.0f;
    else fBoost = MAX(fBoost - 10.0f * TIME, 0.0f);

    // 
    const coreFloat fModifier = this->IsRolling() ? (50.0f + LERPB(25.0f, 0.0f, m_fRollTime)) : (HAS_BIT(m_pInput->iActionHold, 0u) ? LERPH3(20.0f, 40.0f, fBoost) : LERPH3(50.0f, 70.0f, fBoost));
    return m_fSpeed * fModifier;
}


// ****************************************************************
// 
void cPlayer::__EquipShield()
{
    ASSERT( HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    ASSERT(!HAS_FLAG(m_iStatus, PLAYER_STATUS_SHIELDED))

    // 
    ADD_FLAG(m_iStatus, PLAYER_STATUS_SHIELDED)
    this->SetMaxHealth(PLAYER_SHIELD);
}


// ****************************************************************
// 
coreBool cPlayer::__NewCollision(const coreObject3D* pObject)
{
    // find existing collision
    if(m_aCollision.count(pObject))
    {
        // update frame number
        m_aCollision.at(pObject) = Core::System->GetCurFrame();
        return false;
    }

    // add collision to list
    m_aCollision.emplace(pObject, Core::System->GetCurFrame());
    return true;
}


// ****************************************************************
// 
void cPlayer::__UpdateCollisions()
{
    // loop through all collisions
    const coreUint32 iCurFrame = Core::System->GetCurFrame() - 1u;
    FOR_EACH_DYN(it, m_aCollision)
    {
        // check for old entries and remove them
        if((*it) == iCurFrame) DYN_KEEP  (it)
                          else DYN_REMOVE(it, m_aCollision)
    }
}