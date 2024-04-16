///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Released under the zlib License                 |//
//| More information available in the readme file   |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// kontinuierlicher laser erzeugt snow, oder laser und cube bewegung (na, was is mit schwingen), aber irgendwas das kontinuierlich nen pfad zeichnet
// TODO 1: handle cubes die am morgenstern hängen, aber jetzt zerstört werden müssen (weil ein neuer cube erzeugt wird)
// TODO 1: decent smoke on ice cubes
// TODO 1: schildkröte, zieht gliedmaßen ein, kann "umfallen"
// TODO 1: stone outside of boss should also be ice cube
// nur extremitäten können angegriffen werden, ziehen sich zurück


// ****************************************************************
// counter identifier
#define ICE_INDEX (0u)


// ****************************************************************
// vector identifier


// ****************************************************************
// constructor
cZerothBoss::cZerothBoss()noexcept
: m_afLimbValue {}
, m_fAnimation  (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.0f);

    // configure the boss
    this->Configure(10000, COLOR_SHIP_MAGENTA);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
    {
        coreHashString sModelHigh;
        coreHashString sModelLow;
        coreHashString sVolume;
             if(i == 0u) {sModelHigh = "ship_boss_zeroth_head_high.md3"; sModelLow = "ship_boss_zeroth_head_low.md3"; sVolume = "ship_boss_zeroth_head_volume.md3";}
        else if(i == 3u) {sModelHigh = "ship_boss_zeroth_tail.md3";      sModelLow = "ship_boss_zeroth_tail.md3";     sVolume = "ship_boss_zeroth_tail_volume.md3";}
        else             {sModelHigh = "ship_boss_zeroth_leg.md3";       sModelLow = "ship_boss_zeroth_leg.md3";      sVolume = "ship_boss_zeroth_leg_volume.md3";}

        m_aLimb[i].DefineModelHigh(sModelHigh);
        m_aLimb[i].DefineModelLow (sModelLow);
        m_aLimb[i].DefineVolume   (sVolume);
        m_aLimb[i].SetSize        (this->GetSize());
        m_aLimb[i].Configure      (1, COLOR_SHIP_MAGENTA);
        m_aLimb[i].SetParent      (this);
    }

    // 
    m_Body.DefineModelHigh("ship_boss_zeroth_body_high.md3");
    m_Body.DefineModelLow ("ship_boss_zeroth_body_low.md3");
    m_Body.DefineVolume   ("ship_boss_zeroth_body_volume.md3");
    m_Body.SetSize        (this->GetSize());
    m_Body.Configure      (1, COLOR_SHIP_MAGENTA);
    m_Body.AddStatus      (ENEMY_STATUS_INVINCIBLE);
    m_Body.SetParent      (this);

    // 
    m_Laser.DefineModel  ("object_tube_open.md3");
    m_Laser.DefineTexture(0u, "effect_energy.png");
    m_Laser.DefineProgram("effect_energy_direct_program");
    m_Laser.SetColor3    (COLOR_ENERGY_BLUE);
    m_Laser.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
    {
        m_aIce[i].DefineModelHigh("object_cube_ice.md3");
        m_aIce[i].DefineModelLow ("object_cube_ice.md3");
        m_aIce[i].DefineTexture  (1u, "environment_water_norm.png");
        m_aIce[i].DefineProgram  ("object_ice_program");
        m_aIce[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 6.0f);
        m_aIce[i].SetTexSize     (coreVector2(0.25f,0.25f));
        m_aIce[i].Configure      (50, coreVector3(1.0f,1.0f,1.0f));
        m_aIce[i].AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_GHOST | ENEMY_STATUS_WORTHLESS);
    }

    STATIC_ASSERT(offsetof(cZerothBoss, m_aLimb) < offsetof(cZerothBoss, m_Body))
}


// ****************************************************************
// 
void cZerothBoss::__ResurrectOwn()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());
    //pMission->EnableStar(0u, this);
    
    
    pMission->EnableStar(0u, g_pGame->GetPlayer(0u));

    if(g_pGame->GetCoop())
    {
        pMission->EnableStar(1u, g_pGame->GetPlayer(1u));
    }

    pMission->StartSwing();
}


// ****************************************************************
// 
void cZerothBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->__DisableLaser(bAnimated);

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
        m_aIce[i].Kill(bAnimated);

    // 
    this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cZerothBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    m_Laser.Render();

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
        m_aIce[i].DefineTexture(0u, m_aIce[i].HasStatus(ENEMY_STATUS_DEAD) ? NULL : g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture);   // TODO 1: (look at cMenu::__Reset) needs to be properly unset before engine reset, otherwise shadow texture breaks
    // GetBackground()->GetResolvedTexture()
}


// ****************************************************************
// 
void cZerothBoss::__MoveOwn()
{
    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);
    
    
    
    //const coreVector2 vNewDir = this->AimAtPlayerDual(0u).Normalized();
    //this->SetDirection(coreVector3(vNewDir, 0.0f));
    

    if(g_pGame->GetInterface()->IsBannerActive())
    {
        //g_pGame->GetInterface()->ChangeBossName(PRINT("%02XZE%02XRO%02XTH%02X", coreUint8(P_TO_UI(this) >> 24u), coreUint8(P_TO_UI(this) >> 16u), coreUint8(P_TO_UI(this) >> 8u), coreUint8(P_TO_UI(this))));

        coreRand oRand(F_TO_UI(m_fLifeTime * 10.0f));oRand.Raw();
        g_pGame->GetInterface()->ChangeBannerText(PRINT("%02XZE%02XRO%02XTH%02X", coreUint8(oRand.Raw() >> 8u), coreUint8(oRand.Raw() >> 8u), coreUint8(oRand.Raw() >> 8u), coreUint8(oRand.Raw() >> 8u)), NULL);
    }

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->_StartBoss();
            }

            this->SetPosition(coreVector3(0.0f,0.6f,0.0f) * FOREGROUND_AREA3);

            if(PHASE_TIME_AFTER(0.85f))
                PHASE_CHANGE_TO(10u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 0.2f, LERP_LINEAR)
        {
            //this->__EnableLaser(this->GetPosition().xy(), this->GetDirection().xy());
            //this->__DisableLaser(true);
        });
    }

    // ################################################################
    // ################################################################
    
    
    if(m_Laser.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_Laser.SetAlpha(MAX(m_Laser.GetAlpha() - 2.0f * TIME, 0.0f));
        
        if(!m_Laser.GetAlpha()) this->__DisableLaser(false);
        
        m_Laser.SetTexOffset(coreVector2(0.1f,-0.5f) * m_fAnimation);
        m_Laser.Move();
    }


    
    this->__SetLimbValue(0u, 0.5f + 0.5f * SIN(m_fLifeTime));
    this->__SetLimbValue(1u, 0.5f + 0.5f * SIN(m_fLifeTime));
    
    
    // 
    m_Body.SetPosition   (this->GetPosition   ());
    m_Body.SetDirection  (this->GetDirection  ());
    m_Body.SetOrientation(this->GetOrientation());

    // 
    const coreFloat fRotaBase = this->GetDirection().xy().Angle();

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
    {
        const coreVector2 vDir = coreVector2::Direction(fRotaBase - I_TO_F(i) * (2.0f/6.0f * PI));

        m_aLimb[i].SetPosition   (this->GetPosition   () + coreVector3(vDir * m_afLimbValue[i], 0.0f));
        m_aLimb[i].SetDirection  (this->GetDirection  ());
        m_aLimb[i].SetOrientation(this->GetOrientation());
    }

    // 
    m_aLimb[1].SetDirection(coreVector3(m_aLimb[1].GetDirection().xy().Rotated120() * -1.0f, 0.0f));
    m_aLimb[4].SetDirection(coreVector3(m_aLimb[4].GetDirection().xy().Rotated120(),         0.0f));
    m_aLimb[5].SetDirection(coreVector3(m_aLimb[5].GetDirection().xy()              * -1.0f, 0.0f));
}


// ****************************************************************
// 
void cZerothBoss::__EnableLaser(const coreVector2 vPosition, const coreVector2 vDirection)
{
    WARN_IF(m_Laser.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    const coreFloat fLen = g_pForeground->RayIntersection(vPosition, vDirection);

    // 
    m_Laser.SetPosition (coreVector3(vPosition + vDirection * (fLen * 0.5f), 0.0f));
    m_Laser.SetSize     (coreVector3(2.0f, fLen * 0.5f, 2.0f));
    m_Laser.SetDirection(coreVector3(vDirection,                             0.0f));
    m_Laser.SetAlpha    (1.0f);

    // 
    m_Laser.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Laser);
    
    
    

    const coreVector3 vTarget = coreVector3(vPosition + vDirection * fLen, 0.0f);
    
    g_pSpecialEffects->CreateSplashColor(vTarget, SPECIAL_SPLASH_BIG, COLOR_ENERGY_WHITE);


    this->__CreateCube(vTarget.xy(), -vDirection);
}


// ****************************************************************
// 
void cZerothBoss::__DisableLaser(const coreBool bAnimated)
{
    if(!m_Laser.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    if(!bAnimated)
    {
        // 
        m_Laser.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_Laser);
    }
}


// ****************************************************************
// 
void cZerothBoss::__CreateCube(const coreVector2 vPosition, const coreVector2 vDirection)
{
    // 
    if(!m_aIce[m_aiCounter[ICE_INDEX]].HasStatus(ENEMY_STATUS_DEAD))
        m_aiCounter[ICE_INDEX] = 1 - m_aiCounter[ICE_INDEX];

    // 
    cCustomEnemy& oIce = m_aIce[m_aiCounter[ICE_INDEX]];

    // 
    oIce.Kill(true);
    oIce.Resurrect();

    // 
    oIce.SetPosition (coreVector3(vPosition,  0.0f));
    oIce.SetDirection(coreVector3(vDirection, 0.0f));
}


// ****************************************************************
// 
void cZerothBoss::__SetLimbValue(const coreUintW iIndex, const coreFloat fValue)
{
    ASSERT(iIndex < ZEROTH_LIMBS)
    cCustomEnemy& oLimb = m_aLimb[iIndex];

    // 
    m_afLimbValue[iIndex] = CLAMP(fValue, 0.0f, 1.0f) * -5.0f;

    // 
    if(fValue >= 0.8f) oLimb.AddStatus   (ENEMY_STATUS_GHOST);
                  else oLimb.RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    if(fValue >= 1.0f) oLimb.AddStatus   (ENEMY_STATUS_HIDDEN);
                  else oLimb.RemoveStatus(ENEMY_STATUS_HIDDEN);
}