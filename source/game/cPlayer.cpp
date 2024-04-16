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
// constructor
cPlayer::cPlayer()noexcept
: m_apWeapon        {}
, m_pInput          (&g_TotalInput)
, m_vArea           (PLAYER_AREA_DEFAULT)
, m_vForce          (coreVector2(0.0f,0.0f))
, m_fScale          (1.0f)
, m_fTilt           (0.0f)
, m_fMoveSpeed      (1.0f)
, m_fShootSpeed     (1.0f)
, m_fAnimSpeed      (1.0f)
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
, m_iMaxShield      (0)
, m_iCurShield      (0)
, m_fAnimation      (0.0f)
, m_iLook           (0u)
, m_vMenuColor      (coreVector3(1.0f,1.0f,1.0f))
, m_vLedColor       (coreVector3(1.0f,1.0f,1.0f))
, m_vOldDir         (coreVector2(0.0f,1.0f))
, m_vSmoothOri      (coreVector2(0.0f,0.0f))
, m_fSmoothTilt     (0.0f)
, m_fRangeValue     (0.0f)
, m_fArrowValue     (0.0f)
, m_fCircleValue    (0.0f)

, m_fHitDelay (0.0f)
, m_fBoost    (0.0f)
{
    // load object resources
    this->DefineTexture(0u, "ship_player.png");
    this->DefineTexture(1u, "menu_background_black.png");

    // set object properties
    this->coreObject3D::SetDirection        (coreVector3(0.0f,1.0f,0.0f));
    this->SetOrientation      (coreVector3(0.0f,0.0f,1.0f));
    this->SetCollisionModifier(coreVector3(0.0f,0.0f,0.0f));
    this->SetTexSize          (coreVector2(1.2f,1.2f));

    // set initial status
    m_iStatus = PLAYER_STATUS_DEAD;

    // 
    this->SetMaxHealth(PLAYER_LIVES);
    this->SetBaseColor(COLOR_SHIP_BLACK);

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
    this->ActivateNormalShading();

    // 
    m_Dot.DefineModel("object_dot.md3");
    m_Dot.SetSize    (coreVector3(1.0f,1.0f,1.0f) * PLAYER_COLLISION_MIN);

    // 
    m_Range.DefineTexture(0u, "effect_energy.png");
    m_Range.DefineProgram("effect_energy_flat_invert_program");
    m_Range.SetAlpha     (0.0f);
    m_Range.SetTexSize   (coreVector2(0.1f,0.1f));
    m_Range.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Arrow.DefineModel  ("bullet_cone.md3");
    m_Arrow.DefineTexture(0u, "effect_energy.png");
    m_Arrow.DefineProgram("effect_energy_flat_invert_program");
    m_Arrow.SetAlpha     (0.0f);
    m_Arrow.SetTexSize   (coreVector2(4.0f,1.0f) * 0.2f);
    m_Arrow.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Wind.DefineModel  ("object_sphere.md3");
    m_Wind.DefineTexture(0u, "effect_energy.png");
    m_Wind.DefineProgram("effect_energy_flat_direct_program");
    m_Wind.SetAlpha     (0.0f);
    m_Wind.SetTexSize   (coreVector2(1.0f,5.0f));
    m_Wind.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Bubble.DefineTexture(0u, "effect_energy.png");
    m_Bubble.DefineProgram("effect_energy_flat_spheric_program");
    m_Bubble.SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.6f, 0.0f));
    m_Bubble.SetTexSize   (coreVector2(0.5f,0.5f));
    m_Bubble.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aShield); ++i)
    {
        m_aShield[i].DefineModel  ("effect_shield.md3");
        m_aShield[i].DefineTexture(0u, "effect_shield.png");
        m_aShield[i].DefineProgram("effect_shield_program");
        m_aShield[i].SetAlpha     (0.0f);
        m_aShield[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_Circle.DefineModel  ("object_sphere.md3");
    m_Circle.DefineTexture(0u, "effect_energy.png");
    m_Circle.DefineProgram("effect_energy_flat_spheric_program");
    m_Circle.SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 1.0f, 0.0f));
    m_Circle.SetTexSize   (coreVector2(1.0f,5.0f));
    m_Circle.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Exhaust.DefineModel  ("object_tube_open.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_direct_program");
    m_Exhaust.SetAlpha     (0.7f);
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
}


// ****************************************************************
// configure the player
void cPlayer::Configure(const coreUintW iShipType)
{
    // select appearance type
    coreHashString sModelHigh;
    coreHashString sModelLow;
    coreHashString sGeometry;
    coreVector3    vEnergy;
    switch(iShipType)
    {
    default: ASSERT(false)
    case PLAYER_SHIP_ATK: sModelHigh = "ship_player_atk_high.md3"; sModelLow = "ship_player_atk_low.md3"; sGeometry = "object_cube_top.md3";  vEnergy = COLOR_PLAYER_BLUE;   m_vMenuColor = COLOR_MENU_BLUE;   m_vLedColor = COLOR_LED_BLUE;   break;
    case PLAYER_SHIP_DEF: sModelHigh = "ship_player_def_high.md3"; sModelLow = "ship_player_def_low.md3"; sGeometry = "object_tetra_top.md3"; vEnergy = COLOR_PLAYER_YELLOW; m_vMenuColor = COLOR_MENU_YELLOW; m_vLedColor = COLOR_LED_YELLOW; break;
    case PLAYER_SHIP_P1:  sModelHigh = "ship_projectone_high.md3"; sModelLow = "ship_projectone_low.md3"; sGeometry = "object_penta_top.md3"; vEnergy = COLOR_PLAYER_GREEN;  m_vMenuColor = COLOR_MENU_GREEN;  m_vLedColor = COLOR_LED_GREEN;  break;
    }

    // load models
    this->DefineModelHigh(sModelHigh);
    this->DefineModelLow (sModelLow);
    this->DefineVolume   (sModelHigh);   // prevent clusters

    // 
    m_Range .DefineModel(sGeometry);
    m_Bubble.DefineModel(sGeometry);

    // 
    m_Range     .SetColor3(vEnergy);
    m_Arrow     .SetColor3(vEnergy * (0.9f/1.1f));
    m_Wind      .SetColor3(vEnergy * (1.6f/1.1f));
    m_aShield[0].SetColor3(vEnergy * (1.0f/1.1f));
    m_aShield[1].SetColor3(vEnergy * (1.0f/1.1f));
    m_Exhaust   .SetColor3(vEnergy);

    // 
    SET_BITVALUE(m_iLook, 4u, 0u, iShipType)
    SET_BITVALUE(m_iLook, 8u, 4u, m_iBaseColor)
}


// ****************************************************************
// 
void cPlayer::EquipShield(const coreInt32 iShield)
{
    ASSERT( HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    ASSERT(!HAS_FLAG(m_iStatus, PLAYER_STATUS_SHIELDED))

    if(iShield)
    {
        // 
        ADD_FLAG(m_iStatus, PLAYER_STATUS_SHIELDED)
        m_iMaxShield = (iShield == SHIELD_MAX) ? SHIELD_INVINCIBLE : iShield;
    }
}


// ****************************************************************
// equip new main weapon
void cPlayer::EquipWeapon(const coreUintW iIndex, const coreInt32 iID)
{
    ASSERT(iIndex < PLAYER_EQUIP_WEAPONS)
    if(m_apWeapon[iIndex] && (m_apWeapon[iIndex]->GetID() == iID)) return;

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
    case 0u: break;
    }
}


// ****************************************************************
// render the player
void cPlayer::Render()
{
    if(HAS_FLAG(m_iStatus, PLAYER_STATUS_TOP)) return;

    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        // 
        cLodObject::RenderHighObject(this);
    }
}

void cPlayer::RenderBefore()
{
    if(HAS_FLAG(m_iStatus, PLAYER_STATUS_TOP)) return;

    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        // 
        m_Circle.Render();

        // 
        for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
            m_apWeapon[i]->Render();
        // 
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Bubble);

        // 
        m_Bubble    .Render();
        m_aShield[1].Render();
        m_Wind      .Render();
        m_Exhaust   .Render();
    }
}

void cPlayer::RenderMiddle()
{
    if(HAS_FLAG(m_iStatus, PLAYER_STATUS_TOP)) return;

    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        // 
        m_aShield[0].Render();
    }
}

void cPlayer::RenderAfter()
{
    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD))
    {
        if(HAS_FLAG(m_iStatus, PLAYER_STATUS_TOP))
        {
            this->RemoveStatus(PLAYER_STATUS_TOP);
            {
                this->RenderBefore();

                glEnable(GL_DEPTH_TEST);
                {
                    this->Render();
                    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(this);   // TODO 1: wird doppelt gezeichnet
                }
                glDisable(GL_DEPTH_TEST);

                this->RenderMiddle();
            }
            this->AddStatus(PLAYER_STATUS_TOP);
        }
        
        
        // 
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Range);
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Arrow);

        // 
        m_Arrow.Render();   // # swapped
        m_Range.Render();
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
            if(HAS_BIT(m_pInput->iActionPress, PLAYER_ACTION_TURN_LEFT))
                vNewDir = -vNewDir.Rotated90();
            if(HAS_BIT(m_pInput->iActionPress, PLAYER_ACTION_TURN_RIGHT))
                vNewDir =  vNewDir.Rotated90();

            // 
#if defined(_CORE_DEBUG_)
            //if(vNewDir != this->GetDirection().xy()) g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_PLAYER_TURN);
#endif

            // set new direction
            this->coreObject3D::SetDirection(coreVector3(vNewDir, 0.0f));
        }

        if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_ROLL))
        {
            // 
            //if(HAS_BIT(m_pInput->iActionPress, PLAYER_ACTION_TURN_ROLL))
            //    if(m_fRollTime <= 0.0f) this->StartRolling(m_pInput->vMove);
        }

        if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_MOVE))
        {
            // move the ship
            vNewPos += (m_pInput->vMove * this->CalcMoveSpeed() + m_vForce) * TIME;

            // 
            const coreVector2 vDiff = (vNewPos - this->GetPosition().xy()) * RCP(MAX(TIME * FRAMERATE_MIN, CORE_MATH_PRECISION));
            const coreVector2 vTarget = vDiff - m_vSmoothOri;
            if(!vTarget.IsNull()) m_vSmoothOri = m_vSmoothOri + vTarget.Normalized() * (30.0f * TIME * SmoothTowards(vTarget.Length(), 1.0f));
            vNewOri = coreVector3(CLAMP(m_vSmoothOri.x, -0.8f, 0.8f), CLAMP(m_vSmoothOri.y, -0.8f, 0.8f), 1.0f).NormalizedUnsafe();

            // restrict movement to the foreground area
                 if(vNewPos.x < m_vArea.x) {vNewPos.x = m_vArea.x; m_vForce.x =  ABS(m_vForce.x);}
            else if(vNewPos.x > m_vArea.z) {vNewPos.x = m_vArea.z; m_vForce.x = -ABS(m_vForce.x);}
                 if(vNewPos.y < m_vArea.y) {vNewPos.y = m_vArea.y; m_vForce.y =  ABS(m_vForce.y);}
            else if(vNewPos.y > m_vArea.w) {vNewPos.y = m_vArea.w; m_vForce.y = -ABS(m_vForce.y);}
        }
        else m_vSmoothOri = coreVector2(0.0f,0.0f);

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
                const coreFloat fAngle = LERPB(0.0f, 8.0f*PI, m_fRollTime);
                const coreFloat fSide  = -SIGN(coreVector2::Dot(-this->GetDirection().xy().Rotated90(), UnpackDirection(m_iRollDir)));
                vNewOri *= coreMatrix4::RotationAxis(fAngle * fSide, this->GetDirection()).m123();
            }

            // set new position and orientation
            this->coreObject3D::SetPosition(coreVector3(vNewPos, this->GetPosition().z));
            this->SetOrientation(vNewOri);
        }
        
        
        this->SetSize(coreVector3(1.0f,1.0f,1.0f) * PLAYER_SIZE_FACTOR);

        // normalize collision size
        if(this->GetVolume().IsUsable())
        {
            const coreFloat fRadius = MAX(this->GetMove().Length(), PLAYER_COLLISION_MIN);
            this->SetCollisionModifier((coreVector3(1.0f,1.0f,1.0f) * fRadius) / this->GetVolume()->GetBoundingRange());

            ASSERT(!this->GetVolume()->GetNumClusters())
        }

        // 
        m_fAnimation.UpdateMod(1.0f * m_fAnimSpeed, 20.0f);
        if(this->IsDarkShading()) this->SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.25f));

        // 
        const coreMatrix3 mTiltMat = coreMatrix4::RotationX(m_fTilt).m123();
        const coreVector3 vOldDir  = this->GetDirection  ();
        const coreVector3 vOldOri  = this->GetOrientation();
        
        if(m_fTilt)
        {
            const coreFloat fTarget = this->GetMove().y * 0.5f - m_fSmoothTilt;
            m_fSmoothTilt = m_fSmoothTilt + SIGN(fTarget) * (30.0f * TIME * SmoothTowards(ABS(fTarget), 1.0f));
        }
        else m_fSmoothTilt = 0.0f;

        // 
        this->coreObject3D::SetDirection((vOldDir * mTiltMat + coreVector3(0.0f, m_fSmoothTilt, 0.0f)).Normalized());
        this->SetOrientation((vOldOri * mTiltMat + coreVector3(0.0f, ABS(m_fSmoothTilt), 0.0f)).Normalized());//(vOldOri * mTiltMat);

        // move the 3d-object
        this->coreObject3D::Move();

        // 
        this->coreObject3D::SetDirection(vOldDir);
        this->SetOrientation(vOldOri);

        // update all weapons (shooting and stuff)
        for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
        {
            const coreUint8 iShoot = (/*!this->IsRolling() && */!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_SHOOT) && !m_fInterrupt) ? ((m_pInput->iActionHold & (BITLINE(WEAPON_MODES) << (i*WEAPON_MODES))) >> (i*WEAPON_MODES)) : 0u;
            m_apWeapon[i]->Update(iShoot, m_fShootSpeed);
        }

        // 
        if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_NO_INPUT_ALL) || HAS_FLAG(m_iStatus, PLAYER_STATUS_KEEP_RANGE))
        {
            if(!m_fRangeValue) this->EnableRange();
            m_fRangeValue.UpdateMin(3.0f, 1.0f);
        }
        else
        {
            m_fRangeValue.UpdateMax(-3.0f, 0.0f);
            if(!m_fRangeValue) this->DisableRange();
        }

        // 
        if(!coreMath::IsNear(m_vOldDir.x, this->GetDirection().x) ||
           !coreMath::IsNear(m_vOldDir.y, this->GetDirection().y))
        {
            m_vOldDir = this->GetDirection().xy();
            this->ShowArrow(0u);
        }

        // 
        m_Dot.SetPosition(this->GetPosition());
        m_Dot.Move();

        if(m_Range.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            const coreVector2 vDir   = coreVector2::Direction(m_fAnimation * (-1.6f*PI));
            const coreFloat   fScale = LERPS(0.0f, 1.0f, m_fRangeValue);

            // 
            m_Range.SetPosition (this->GetPosition());
            m_Range.SetSize     (coreVector3(1.0f,1.0f,1.0f) * PLAYER_RANGE_SIZE * PLAYER_SIZE_FACTOR * fScale);
            m_Range.SetDirection(coreVector3(vDir, 0.0f));
            m_Range.SetAlpha    (STEP(0.0f, 0.15f, fScale));
            m_Range.SetTexOffset(m_Range.GetTexOffset() - m_Range.GetDirection().xy() * (0.1f * TIME * m_fAnimSpeed));
            m_Range.Move();
        }

        if(m_Arrow.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_fArrowValue.Update(-2.0f * m_fAnimSpeed);

            // 
            if(m_fArrowValue <= 0.0f) this->DisableArrow();

            // 
            m_Arrow.SetPosition (this->GetPosition () + this->GetDirection() * 6.2f * PLAYER_SIZE_FACTOR);
            m_Arrow.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.3f * PLAYER_SIZE_FACTOR);
            m_Arrow.SetDirection(this->GetDirection());
            m_Arrow.SetAlpha    (LERPH3(0.0f, 1.0f, m_fArrowValue));
            m_Arrow.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.15f));
            m_Arrow.Move();
        }

        if(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_Wind.SetAlpha((m_Wind.GetAlpha() < 0.0f) ? 0.5f : COS((0.5f*PI) * m_fRollTime*0.0f));

            // 
            m_Wind.SetPosition (this->GetPosition());
            m_Wind.SetSize     (coreVector3(1.0f,1.08f,1.0f) * PLAYER_WIND_SIZE * PLAYER_SIZE_FACTOR * LERP(1.0f, 1.5f, POW3(m_fRollTime*0.0f)));
            m_Wind.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.3f));
            m_Wind.Move();
        }

        if(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_fFeelTime.Update(-1.0f * m_fAnimSpeed);

            // 
            if(m_fFeelTime > 0.0f) m_Bubble.SetAlpha(MIN(m_Bubble.GetAlpha() + 4.0f*TIME * m_fAnimSpeed, 1.0f));
                              else m_Bubble.SetAlpha(MAX(m_Bubble.GetAlpha() - 4.0f*TIME * m_fAnimSpeed, 0.0f));

            // 
            if(!m_Bubble.GetAlpha()) this->EndFeeling();

            // 
            const coreVector2 vDir = coreVector2::Direction(m_fAnimation * (1.6f*PI));

            // 
            m_Bubble.SetPosition (this->GetPosition());
            m_Bubble.SetSize     (coreVector3(1.0f,1.0f,1.0f) * PLAYER_BUBBLE_SIZE * PLAYER_SIZE_FACTOR * m_Bubble.GetAlpha());
            m_Bubble.SetDirection(coreVector3(vDir, 0.0f));
            m_Bubble.SetTexOffset(m_Bubble.GetTexOffset() - m_Bubble.GetDirection().xy() * (0.2f * TIME * m_fAnimSpeed));
            m_Bubble.Move();
        }

        if(m_aShield[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_fIgnoreTime.Update(-1.0f / PLAYER_IGNORE_TIME * m_fAnimSpeed);

            // 
            if(m_fIgnoreTime <= 0.0f) this->EndIgnoring();

            // 
            const coreVector2 vDir    = coreVector2::Direction(m_fAnimation * (1.0f*PI));
            const coreFloat   fBounce = m_iIgnoreType ? LERPBR(2.0f, 0.5f, m_fIgnoreTime) : LERPH3(0.0f, 0.5f, m_fIgnoreTime);

            // 
            m_aShield[0].SetPosition   (this->GetPosition());
            m_aShield[0].SetSize       (coreVector3(4.7f,4.7f,4.7f) * PLAYER_SIZE_FACTOR);
            m_aShield[0].SetOrientation(coreVector3(vDir.x, 0.0f, vDir.y));
            m_aShield[0].SetAlpha      (LERPH3(0.0f, 1.0f, MIN(m_fIgnoreTime * 1.4f, 1.0f)));
            m_aShield[0].SetTexOffset  (coreVector2(fBounce, 0.0f));
            m_aShield[0].Move();

            // 
            m_aShield[1].SetPosition   (m_aShield[0].GetPosition   ());
            m_aShield[1].SetSize       (coreVector3(4.7f,4.7f,4.7f) * -PLAYER_SIZE_FACTOR);
            m_aShield[1].SetOrientation(m_aShield[0].GetOrientation() * -1.0f);
            m_aShield[1].SetAlpha      (m_aShield[0].GetAlpha      ());
            m_aShield[1].SetTexOffset  (m_aShield[0].GetTexOffset  ());
            m_aShield[1].Move();
        }

        if(m_Circle.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_fCircleValue.Update(-2.0f * m_fAnimSpeed);

            // 
            if(m_fCircleValue <= 0.0f) this->DisableCircle();

            // 
            m_Circle.SetPosition (this->GetPosition());
            m_Circle.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.5f * PLAYER_SIZE_FACTOR * LERPBR(5.0f, 1.0f, m_fCircleValue));
            m_Circle.SetAlpha    (LERPBR(0.0f, 1.0f, m_fCircleValue));
            m_Circle.SetTexOffset(coreVector2(0.0f, m_fAnimation * -0.2f));
            m_Circle.Move();
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
        
        
        if(this->IsProjectOne())
        {
            coreVector3 vEnergyColor, vBlockColor, vLevelColor;
            cProjectOneBoss::CalcColorLerp(FMOD(m_fAnimation / 20.0f * 24.0f, 8.0f), &vEnergyColor, &vBlockColor, &vLevelColor);
            
            m_Range     .SetColor3(vEnergyColor);
            m_Arrow     .SetColor3(vEnergyColor * (0.9f/1.1f));
            m_Wind      .SetColor3(vEnergyColor * (1.6f/1.1f));
            m_aShield[0].SetColor3(vEnergyColor * (1.0f/1.1f));
            m_aShield[1].SetColor3(vEnergyColor * (1.0f/1.1f));
            m_Exhaust   .SetColor3(vEnergyColor);
            
            m_Bubble   .SetColor3(vEnergyColor);
        }
        
        // 
        REMOVE_FLAG(m_iStatus, PLAYER_STATUS_REPAIRED)
    }

    // 
    for(coreUintW i = 0u; i < PLAYER_EQUIP_WEAPONS; ++i)
        m_apWeapon[i]->Move();

    // 
    m_DataTable .Update();
    m_ScoreTable.Update();

    // 
    this->__UpdateCollisions();

    // 
    this->_UpdateAlwaysAfter();
    
    if(m_fHitDelay)
    {
        m_fHitDelay.UpdateMax(-1.0f, 0.0f, 0);
        if(!m_fHitDelay)
        {

        }
    }
}


// ****************************************************************
// reduce current health
coreInt32 cPlayer::TakeDamage(const coreInt32 iDamage, const coreUint8 iElement, const coreVector2 vImpact)
{
    ASSERT(STATIC_ISVALID(g_pGame))

    if(iDamage)
    {
        constexpr coreInt32 iReplacement = 1;

        // 
        m_ScoreTable.CancelCooldown();

        // 
        const coreUint16 iShift = g_pGame->IsMulti() ? 2u : 3u;
        g_pGame->GetTimeTable()->AddShiftBad(iShift);

        m_DataTable.EditCounterTotal  ()->iShiftBad += iShift;
        m_DataTable.EditCounterMission()->iShiftBad += iShift;
        m_DataTable.EditCounterSegment()->iShiftBad += iShift;

        // 
        g_pGame->GetCombatText()->DrawShift(iShift, this->GetPosition());

        // 
        const coreInt32 iShieldDamage = CLAMP(iReplacement, m_iCurShield - m_iMaxShield, m_iCurShield);
        m_iCurShield -= iShieldDamage;

        // 
        const coreInt32 iTaken = this->_TakeDamage(iReplacement - iShieldDamage, iElement, vImpact) + iShieldDamage;

        if(m_iCurHealth)
        {
            if(iShieldDamage)
            {
                // 
                this->StartIgnoring(m_iCurShield ? 0u : 1u);

                // 
                if(g_CurConfig.Graphics.iHitStop) g_pSpecialEffects->FreezeScreen(0.1f);
            }
            else
            {
                // 
                this->SetDesaturate(PLAYER_DESATURATE);
                this->StartFeeling (PLAYER_FEEL_TIME, 0u);

                // 
                if(g_CurConfig.Graphics.iHitStop) g_pSpecialEffects->FreezeScreen(0.2f);
            }

            // 
            m_fInterrupt = 0.0f;
        }
        else
        {
            // 
            this->Kill(true);

            // 
            if(g_CurConfig.Graphics.iHitStop) g_pSpecialEffects->FreezeScreen(1.5f);
        }

        // 
        if(HAS_FLAG(m_iStatus, PLAYER_STATUS_INVINCIBLE))
        {
            m_iCurHealth = m_iMaxHealth;
            m_iCurShield = m_iMaxShield;
            
            g_pGame->GetInterface()->PingImmune(g_pGame->GetPlayerIndex(this));
        }

        // 
        m_DataTable.EditCounterTotal  ()->iDamageTaken += iTaken;
        m_DataTable.EditCounterMission()->iDamageTaken += iTaken;
        m_DataTable.EditCounterSegment()->iDamageTaken += iTaken;

        // 
        g_pSave->EditGlobalStats      ()->iDamageTaken += iTaken;
        g_pSave->EditLocalStatsMission()->iDamageTaken += iTaken;
        g_pSave->EditLocalStatsSegment()->iDamageTaken += iTaken;

        return iTaken;
    }

    return 0;
}


// ****************************************************************
// 
void cPlayer::HealHealth(const coreUint8 iHealth)
{
    // 
    m_iCurHealth = MIN(m_iCurHealth + iHealth, m_iMaxHealth);
}


// ****************************************************************
// 
void cPlayer::HealShield(const coreUint8 iShield)
{
    // 
    m_iCurShield = MIN(m_iCurShield + iShield, m_iMaxShield);
}


// ****************************************************************
// add player to the game
void cPlayer::Resurrect()
{
    // resurrect player
    if(!HAS_FLAG(m_iStatus, PLAYER_STATUS_DEAD)) return;
    REMOVE_FLAG(m_iStatus, PLAYER_STATUS_DEAD)

    // 
    m_iCurShield = m_iMaxShield;

    // add ship to global shadow and outline
    cShadow::GetGlobalContainer()->BindObject(this);
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->BindObject(this);

    // enable collision
    this->ChangeType(TYPE_PLAYER);

    // add ship to the game
    this->_Resurrect();
    
    this->EnableWind(this->GetDirection().xy());
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
        m_apWeapon[i]->Update(0u, 1.0f);

    // 
    this->EndRolling();
    this->EndFeeling();
    this->EndIgnoring();

    // 
    this->DisableRange();
    this->DisableArrow();
    this->DisableWind();
    this->DisableBubble();
    this->DisableShield();
    this->DisableCircle();
    this->UpdateExhaust(0.0f);

    // 
    m_vArea  = PLAYER_AREA_DEFAULT;
    m_vForce = coreVector2(0.0f,0.0f);

    // 
    m_fMoveSpeed  = 1.0f;
    m_fShootSpeed = 1.0f;
    m_fAnimSpeed  = 1.0f;

    // 
    m_fInterrupt      = 0.0f;
    m_fLightningTime  = 0.0f;
    m_fLightningAngle = 0.0f;

    // 
    m_vOldDir      = coreVector2(0.0f,1.0f);
    m_vSmoothOri   = coreVector2(0.0f,0.0f);
    m_fSmoothTilt  = 0.0f;
    m_fRangeValue  = 0.0f;
    m_fArrowValue  = 0.0f;
    m_fCircleValue = 0.0f;

    // 
    if(bAnimated && this->IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        g_pSpecialEffects->MacroExplosionPhysicalDarkBig(this->GetPosition());
        g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_PLAYER_EXPLOSION);
    }

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
void cPlayer::ShowArrow(const coreUint8 iType)
{
    // 
    if(m_fArrowValue <= 0.0f) this->EnableArrow();
    m_fArrowValue = 1.0f;
}


// ****************************************************************
// 
void cPlayer::ShowCircle()
{
    // 
    if(m_fCircleValue <= 0.0f) this->EnableCircle();
    m_fCircleValue = 1.0f;
}


// ****************************************************************
// 
void cPlayer::StartRolling(const coreVector2 vDirection)
{
    WARN_IF(this->IsRolling()) return;

    // 
    m_fRollTime = 0.0f;
    m_iRollDir  = PackDirection(vDirection);

    // 
    //this->EnableWind(vDirection);
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
    //this->DisableWind();
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

    // 
    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_PLAYER_FEEL);
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
    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, iType ? SOUND_SHIELD_DESTROY : SOUND_SHIELD_HIT);
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
    
    
    this->DefineModelHigh("ship_enemy_miner_high.md3");
    this->DefineModelLow ("ship_enemy_miner_low.md3");
    this->DefineTexture  (0u, "ship_enemy.png");
    
    this->SetBaseColor(COLOR_SHIP_MAGENTA);           
    this->RefreshColor(1.0f);        
    SAFE_DELETE(m_apWeapon[0])
    m_apWeapon[0] = new cEnemyWeapon();
    m_apWeapon[0]->SetOwner(this);
    //this->EquipWeapon(0u, cEnemyWeapon::ID);           // <- bad overwrite
    //this->ActivateNormalShading();           

    this->SetScale(1.1f);
}


// ****************************************************************
// 
void cPlayer::TurnIntoPlayer()
{
    ASSERT(this->IsEnemyLook())

    this->DefineTexture  (0u, "ship_player.png");

    this->Configure(GET_BITVALUE(m_iLook, 4u, 0u));//, coreVector4::UnpackUnorm4x8(GET_BITVALUE(m_iLook, 8u, 4u)).xyz());
    this->EquipWeapon(0u, GET_BITVALUE(m_iLook, 4u, 12u));
    //this->ActivateDarkShading();
    // TODO 1: color
    
    
    this->SetBaseColor(COLOR_SHIP_BLACK);
    this->RefreshColor(1.0f);        
    
    
    this->SetScale(1.0f);
}


// ****************************************************************
// 
void cPlayer::EnableRange()
{
    WARN_IF(m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableRange();

    // 
    m_Range.SetAlpha(0.0f);

    // 
    m_Range.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Range);
}


// ****************************************************************
// 
void cPlayer::DisableRange()
{
    if(!m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Range.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Range);
}


// ****************************************************************
// 
void cPlayer::EnableArrow()
{
    WARN_IF(m_Arrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableArrow();

    // 
    m_Arrow.SetAlpha(0.0f);

    // 
    m_Arrow.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Arrow);
}


// ****************************************************************
// 
void cPlayer::DisableArrow()
{
    if(!m_Arrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Arrow.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Arrow);
}


// ****************************************************************
// 
void cPlayer::EnableWind(const coreVector2 vDirection)
{
    WARN_IF(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableWind();

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
    WARN_IF(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableBubble();

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
    WARN_IF(m_aShield[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableShield();

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aShield); ++i)
    {
        // 
        m_aShield[i].SetAlpha(0.0f);

        // 
        m_aShield[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(&m_aShield[i]);
    }
}


// ****************************************************************
// 
void cPlayer::DisableShield()
{
    if(!m_aShield[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aShield); ++i)
    {
        // 
        m_aShield[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_aShield[i]);
    }
}


// ****************************************************************
// 
void cPlayer::EnableCircle()
{
    WARN_IF(m_Circle.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->DisableCircle();

    // 
    m_Circle.SetAlpha(0.0f);

    // 
    m_Circle.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Circle);
}


// ****************************************************************
// 
void cPlayer::DisableCircle()
{
    if(!m_Circle.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Circle.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Circle);
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
    m_Exhaust.SetSize     (coreVector3(fSize, fLen, fSize) * 0.6f * PLAYER_SIZE_FACTOR);
    m_Exhaust.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.75f));
    m_Exhaust.SetPosition (this->GetPosition () - this->GetDirection() * (m_Exhaust.GetSize().y + 4.0f * PLAYER_SIZE_FACTOR));
    m_Exhaust.SetDirection(this->GetDirection());
    m_Exhaust.SetEnabled  (fStrength ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Exhaust.Move();
}


// ****************************************************************
// 
coreBool cPlayer::TestCollisionPrecise(const coreObject3D* pObject, coreVector3* OUTPUT pvIntersection, coreBool* OUTPUT pbFirstHit)
{
    ASSERT(pObject && pvIntersection && pbFirstHit)

#if 0   // # disable volume collision

    // 
    if(Core::Manager::Object->TestCollision(&m_Dot, pObject, pvIntersection))
    {
        // 
        (*pvIntersection) = this->GetPosition();
        (*pbFirstHit)     = this->__NewCollision(pObject);
        return true;
    }

#endif

    // 
    const coreVector2 vMove = this->GetMove();
    if(vMove.IsNull())
    {
        // 
        const coreVector3 vRayPos = this->GetPosition();
        const coreVector3 vRayDir = coreVector3(0.0f,0.0f,1.0f);

        // 
        coreFloat fHitDistance = 0.0f;
        coreUint8 iHitCount    = 1u;
        if(Core::Manager::Object->TestCollision(pObject, vRayPos, vRayDir, &fHitDistance, &iHitCount) && (iHitCount & 0x01u))
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
        coreFloat fHitDistance = 0.0f;
        coreUint8 iHitCount    = 1u;
        if(Core::Manager::Object->TestCollision(pObject, vRayPos, vRayDir, &fHitDistance, &iHitCount) && ((iHitCount & 0x01u) || (POW2(fHitDistance) < vMove.LengthSq())))
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
coreBool cPlayer::TestCollisionPrecise(const coreVector2 vRayPos, const coreVector2 vRayDir, const coreObject3D* pRef, coreFloat* OUTPUT pfHitDistance, coreUint8* OUTPUT piHitCount, coreBool* OUTPUT pbFirstHit)
{
    ASSERT(vRayDir.IsNormalized() && pRef && pfHitDistance && piHitCount && pbFirstHit)

    // 
    const sRayData oOldRay = this->__NewRayData(vRayPos, vRayDir, pRef);

    // 
    const coreVector2 vDiff    = this->GetPosition().xy() - vRayPos;
    const coreVector2 vDiffOld = this->GetOldPos()        - oOldRay.vPosition;

    // 
    const coreFloat fDot    = coreVector2::Dot(vDiff,    vRayDir           .Rotated90());
    const coreFloat fDotOld = coreVector2::Dot(vDiffOld, oOldRay.vDirection.Rotated90());

    // 
    if((SIGN(fDot) != SIGN(fDotOld)) && (ABS(fDot) < 5.0f) && (ABS(fDotOld) < 5.0f))   // to handle teleportation
    {
        // 
        (*pfHitDistance) = coreVector2::Dot(vDiff, vRayDir);
        (*piHitCount)    = 1u;
        (*pbFirstHit)    = this->__NewCollision(pRef);
        return true;
    }

    return false;
}


// ****************************************************************
// 
coreVector2 cPlayer::CalcMove()
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
coreFloat cPlayer::CalcMoveSpeed()
{
    
    //if(HAS_BIT(m_pInput->iActionPress,   0u) ||   // to make movement during quickshots easier
    //   HAS_BIT(m_pInput->iActionRelease, 0u))     // to make emergency evasion maneuvers easier
    //    s_fBoost = 1.0f;
    //else s_fBoost = MAX(s_fBoost - 10.0f * TIME, 0.0f);
         if(HAS_BIT(m_pInput->iActionPress,   0u)) m_fBoost = 1.0f;
    else if(HAS_BIT(m_pInput->iActionRelease, 0u)) m_fBoost = 0.0f;
    else m_fBoost = MAX(m_fBoost - 10.0f * TIME, 0.0f);

    // 
    //const coreFloat fModifier = this->IsRolling() ? (50.0f + LERPB(25.0f, 0.0f, m_fRollTime)) : (HAS_BIT(m_pInput->iActionHold, 0u) ? LERPH3(20.0f, 40.0f, s_fBoost) : LERPH3(50.0f, 70.0f, s_fBoost));
    //const coreFloat fModifier = HAS_BIT(m_pInput->iActionHold, 0u) ? LERPH3(20.0f, 40.0f, m_fBoost) : LERPH3(50.0f, 70.0f, m_fBoost);
    const coreFloat fModifier = HAS_BIT(m_pInput->iActionHold, 0u) ? ((m_fBoost) ? 50.0f : 20.0f) : 50.0f;
    return m_fMoveSpeed * fModifier;
}


// ****************************************************************
// 
coreBool cPlayer::__NewCollision(const coreObject3D* pObject)
{
    // find existing collision
    if(m_aiCollision.count(pObject))
    {
        // update frame number
        m_aiCollision.at(pObject) = g_pGame->GetTimeTable()->GetTimeMonoRaw();//Core::System->GetCurFrame();
        return false;
    }

    // add collision to list
    m_aiCollision.emplace(pObject, g_pGame->GetTimeTable()->GetTimeMonoRaw());//Core::System->GetCurFrame());
    return true;
}


// ****************************************************************
// 
cPlayer::sRayData cPlayer::__NewRayData(const coreVector2 vRayPos, const coreVector2 vRayDir, const coreObject3D* pRef)
{
    // 
    sRayData oData;
    oData.iFrame     = g_pGame->GetTimeTable()->GetTimeMonoRaw();//Core::System->GetCurFrame();
    oData.vPosition  = vRayPos;
    oData.vDirection = vRayDir;

    // 
    if(m_aRayData.count(pRef))
    {
        const sRayData oOldData = m_aRayData.at(pRef);

        // 
        m_aRayData.at(pRef) = oData;
        return oOldData;
    }

    // 
    m_aRayData.emplace(pRef, oData);
    return oData;
}


// ****************************************************************
// 
void cPlayer::__UpdateCollisions()
{
    const coreUint32 iCurFrame = g_pGame->GetTimeTable()->GetTimeMonoRaw() - 1u;//Core::System->GetCurFrame() - 1u;

    // loop through all collisions
    FOR_EACH_DYN(it, m_aiCollision)
    {
        // check for old entries and remove them
        if((*it) == iCurFrame) DYN_KEEP  (it)
                          else DYN_REMOVE(it, m_aiCollision)
    }

    // 
    FOR_EACH_DYN(it, m_aRayData)
    {
        // check for old entries and remove them
        if(it->iFrame == iCurFrame) DYN_KEEP  (it)
                               else DYN_REMOVE(it, m_aRayData)
    }
}


// ****************************************************************
// 
coreBool cPlayer::__CheckPlayerTest(const ePlayerTest eTest)const
{
    if(this->IsRolling ()) return HAS_FLAG(eTest, PLAYER_TEST_ROLL);
    if(this->IsFeeling ()) return HAS_FLAG(eTest, PLAYER_TEST_FEEL);
    if(this->IsIgnoring()) return HAS_FLAG(eTest, PLAYER_TEST_IGNORE);
                           return HAS_FLAG(eTest, PLAYER_TEST_NORMAL);
}