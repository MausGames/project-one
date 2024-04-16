///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// crashing enemies should not smoke, and should be subtle, otherwise they are too annoying
// adding the tank is just too much, so some of the attacks from enemies are therefore removed, and focus is on the enemy mechanics in combinations with the unique tank attacks
// it is very harder to find a perfect safe-spot, because of the tank-movement
// explosion when changing weapons is hiding fade-in of new weapon
// position change of the tank into move direction also changes the way tank-aiming interacts with the player (e.g. on side the attacks mostly hit the other side, in center it is evenly distributed, turning by 90 degree also changes the side by 90 degree)
// after a weapon change, the new weapon needs to attack 1-2 times before enemies appear
// the haubitze and the wizzrobe enemies work well together with their different timings (especially when enabled groups are completely separate and on the border, player has to switch or time perfectly for a multi-kill)
// rotating side-movement with laser-weapon changes the primary attack direction (which is very apparent for this weapon)
// using the laser-weapon with the mole enemies, it is very important the laser color is distinct from the mole color (the laser had the same color before, and it was so confusing)
// tower enemies should be very simple, as they can damage the player besides the tank weapon for the first time, so complex setups can overwhelm the player (especially moving enemies were too much)
// deactivated spike-plates should indicate the next enemies to spawn
// the child enemies need to be killed fully to cause damage to the boss, which adds a bit more depth tho this sub-phase, especially when also evading the wave-attack
// alle angriffe, außer die raketen, erscheinen an der einschlags-stelle, ein teppich von der anderen seite zum ziel war zu un-balanced (zu nah am einschlag war zu leicht, zu fern zu schwer), geschosse entlang des schusses waren viel zu schwer, und ein wellen-angriff direkt vom tank aus hat die mitte komplett unbrauchbar gemacht (und hat zu sehr eingeschränkt)
// intro gegner sollten weit auseinander stehen, damit man die abstürze gut sieht, und später kommen, damit der spieler erst versucht den tank anzugreifen (und dabei die haubitze sieht, mit der aiming mechanik die bei allen waffen gleich ist)
// abstürzende gegner sollten explodieren vor absturz, was besonders wichtig ist um die gegner über spike-plates zu kaschieren
// ACHIEVEMENT: get hit by every tank-weapon exactly once
// TODO 1: hard-mode: wind + minen
// TODO 1: hard mode: wind force in environment-richtung (damit es von tiger position beeinflusst wird) (stark genug, dass man beim schießen nicht dagegen ankommt)
// TODO 1: Symbol I V X L C D M Value 1 5 10 50 100 500 1000    -> anbringen mit eigenem kleinen quad als sub-weapon pro waffe
// TODO 1: sollen jetzt noch zahlen und/oder farben für jede waffe hinzugefügt werden ? ein eigenes quad, das über die waffe schwebt (kann durch explosion versteckt werden)
// TODO 5: (mines need to be enemies to allow blinking, combo/chain)
// TODO 5: (in die stacheln schießen erzeugt effekt (knusprig))
// TODO 5: (make sure to disable wind on boss-death (hard if necessary))
// TODO 5: (sting calculations only for enabled stings)


// ****************************************************************
// counter identifier
#define WEAPON_SHOT      (0u)
#define BEAM_SHOT        (1u)
#define MANUAL_TRANSFORM (2u)


// ****************************************************************
// vector identifier
#define TRAIL_HIT     (0u)   // # uses 0u - 1u
#define OVERDRIVE_HIT (2u)
#define RECOIL_TIME   (3u)
#define POS_OFFSET    (4u)
#define JUMP_DATA     (5u)
#define HELPER_DATA   (6u)
#define FINAL_ROTA    (7u)
#define VEIL_VALUE    (8u)


// ****************************************************************
// constructor
cTigerBoss::cTigerBoss()noexcept
: m_vBeamPos       (coreVector2(0.0f,0.0f))
, m_fBeamTime      (0.0f)
, m_Sting          (TIGER_STINGS)
, m_afStingTime    {}
, m_avPushDir      {}
, m_fPushPower     (0.0f)
, m_bPushState     (false)
, m_iWeaponType    (0u)
, m_iWeaponTypeOld (0u)
, m_fWeaponChange  (0.0f)
, m_aiBulletHit    {}
, m_iDecalState    (0u)
, m_fAnimation     (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_tiger_body_high.md3");
    this->DefineModelLow ("ship_boss_tiger_body_low.md3");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.5f);

    // configure the boss
    this->Configure(TIGER_DAMAGE * 117, 0u, COLOR_SHIP_YELLOW);
    this->AddStatus(ENEMY_STATUS_BOTTOM | ENEMY_STATUS_SECRET);

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrack); ++i)
    {
        // 
        m_aTrack[i].DefineModelHigh(i ? "ship_boss_tiger_track_right.md3" : "ship_boss_tiger_track_left.md3");
        m_aTrack[i].DefineModelLow (i ? "ship_boss_tiger_track_right.md3" : "ship_boss_tiger_track_left.md3");
        m_aTrack[i].DefineTexture  (0u, "effect_track.png");
        m_aTrack[i].DefineProgram  ("effect_track_program");
        m_aTrack[i].SetSize        (this->GetSize());
        m_aTrack[i].SetTexSize     (coreVector2(1.0f,2.0f));
        m_aTrack[i].Configure      (1, 0u, coreVector3(1.0f,1.0f,1.0f));
        m_aTrack[i].AddStatus      (ENEMY_STATUS_BOTTOM | ENEMY_STATUS_GHOST);
        m_aTrack[i].SetParent      (this);
    }

    for(coreUintW i = 0u; i < TIGER_SUBS; ++i)
    {
        // 
        m_aWeapon[i].Configure(1, 0u, COLOR_SHIP_YELLOW);
        m_aWeapon[i].AddStatus(ENEMY_STATUS_BOTTOM | ENEMY_STATUS_GHOST);
        m_aWeapon[i].SetParent(this);
    }

    for(coreUintW i = 0u; i < TIGER_SUBS; ++i)
    {
        // 
        m_aWeaponOld[i].Configure(1, 0u, COLOR_SHIP_YELLOW);
        m_aWeaponOld[i].AddStatus(ENEMY_STATUS_BOTTOM | ENEMY_STATUS_GHOST);
        m_aWeaponOld[i].SetParent(this);
    }

    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aBeam); ++i)
    {
        // 
        m_aBeam[i].DefineModel  ("object_tube.md3");
        m_aBeam[i].DefineTexture(0u, "effect_energy.png");
        m_aBeam[i].DefineProgram("effect_energy_direct_program");
        m_aBeam[i].SetSize      (i ? coreVector3(0.0f,0.0f,0.0f) : coreVector3(0.0f,100.0f,0.0f));
        m_aBeam[i].SetColor3    (i ? (LERP(COLOR_ENERGY_WHITE, COLOR_ENERGY_MAGENTA, 0.5f)) : (COLOR_ENERGY_MAGENTA * 1.0f));
        m_aBeam[i].SetTexSize   (i ? coreVector2(3.0f,0.2f) : coreVector2(4.0f,1.0f));
        m_aBeam[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    for(coreUintW i = 0u; i < TIGER_AIMS; ++i)
    {
        // 
        m_aAim[i].DefineModel  (Core::Manager::Object->GetLowQuad());
        m_aAim[i].DefineTexture(0u, "effect_aim.png");
        m_aAim[i].DefineProgram("effect_decal_program");
        m_aAim[i].SetSize      (coreVector3(1.0f,1.0f,1.0f) * 12.0f);
        m_aAim[i].SetTexSize   (coreVector2(0.5f,1.0f));
        m_aAim[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_AimRing.DefineModel  (Core::Manager::Object->GetLowQuad());
    m_AimRing.DefineTexture(0u, "effect_aim.png");
    m_AimRing.DefineProgram("effect_decal_program");
    m_AimRing.SetTexSize   (coreVector2(0.5f,1.0f));
    m_AimRing.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Sting.DefineProgram("object_ship_glow_inst_program");
    {
        for(coreUintW i = 0u; i < TIGER_STINGS; ++i)
        {
            // load object resources
            coreObject3D* pSting = &m_aStingRaw[i];
            pSting->DefineModel  ("object_sting.md3");
            pSting->DefineTexture(0u, "ship_enemy.png");
            pSting->DefineProgram("object_ship_glow_program");

            // set object properties
            pSting->SetSize     (coreVector3(1.0f,1.0f,7.0f));
            pSting->SetDirection(coreVector3(0.0f,0.0f,1.0f));
            pSting->SetColor3   (coreVector3(1.0f,1.0f,1.0f) * Core::Rand->Float(0.4f, 0.6f));
            pSting->SetEnabled  (CORE_OBJECT_ENABLE_NOTHING);

            // add object to the list
            m_Sting.BindObject(pSting);
        }
    }

    // 
    for(coreUintW i = 0u; i < TIGER_SIDES; ++i)
    {
        m_afStingTime[i] = -1.0f;
    }

    // 
    constexpr coreUintW aiSubs[] = {2u, 2u, 2u, 2u, 2u};
    STATIC_ASSERT((ARRAY_SIZE(aiSubs) == TIGER_WEAPONS) && std::all_of(aiSubs, aiSubs + ARRAY_SIZE(aiSubs), [](const coreUintW A) {return (A > 0u) && (A <= TIGER_SUBS);}))

    // 
    for(coreUintW i = 0u; i < TIGER_WEAPONS; ++i)
    {
        for(coreUintW j = 0u, je = aiSubs[i]; j < je; ++j)
        {
            m_aapModelHigh[i][j] = Core::Manager::Resource->Get<coreModel>(PRINT("ship_boss_tiger_weapon_%02zu_%02zu_high.md3", i + 1u, j + 1u));
            m_aapModelLow [i][j] = Core::Manager::Resource->Get<coreModel>(PRINT("ship_boss_tiger_weapon_%02zu_%02zu_low.md3",  i + 1u, j + 1u));
        }
    }
    m_aapModelHigh[1][2] = m_aapModelHigh[1][1];
    m_aapModelLow [1][2] = m_aapModelLow [1][1];
    m_aapModelHigh[4][4] = m_aapModelHigh[4][3] = m_aapModelHigh[4][2] = m_aapModelHigh[4][1];
    m_aapModelLow [4][4] = m_aapModelLow [4][3] = m_aapModelLow [4][2] = m_aapModelLow [4][1];

    // 
    m_pTankSound = Core::Manager::Resource->Get<coreSound>("effect_tank.wav");

    // 
    this->__SwitchWeapon(0u);
}


// ****************************************************************
// destructor
cTigerBoss::~cTigerBoss()
{
    // 
    this->Kill(false);

    // 
    this->__DisableBeam(false);
    for(coreUintW i = 0u; i < TIGER_AIMS;  ++i) this->__DisableAim   (i, false);
    for(coreUintW i = 0u; i < TIGER_SIDES; ++i) this->__DisableStings(i, false);
}


// ****************************************************************
// 
void cTigerBoss::__ResurrectOwn()
{
    // 
    g_pGame->GetCrashManager()->SetImpactCallback([this](void* pData)
    {
        WARN_IF(!pData) return;

        const coreUint8 iPlayer = GET_BITVALUE(P_TO_UI(pData), 8u, 8u);
        const coreUint8 iDamage = GET_BITVALUE(P_TO_UI(pData), 8u, 0u);

        this->TakeDamage(iDamage, ELEMENT_NEUTRAL, HIDDEN_POS, g_pGame->GetPlayer(iPlayer), true);

        if((m_iPhase == 50u) && (this->GetCurHealth() <= TIGER_DAMAGE * 21)) this->AddStatus(ENEMY_STATUS_INVINCIBLE);
    });

    // 
    this->_ResurrectBoss();
}


// ****************************************************************
// 
void cTigerBoss::__KillOwn(const coreBool bAnimated)
{
    cHarenaMission* pMission = d_cast<cHarenaMission*>(g_pGame->GetCurMission());

    // 
    pMission->ResetCollPlayerBullet();

    // 
    this->__DisableBeam(bAnimated);
    for(coreUintW i = 0u; i < TIGER_AIMS;  ++i) this->__DisableAim   (i, bAnimated);
    for(coreUintW i = 0u; i < TIGER_SIDES; ++i) this->__DisableStings(i, bAnimated);

    // 
    if(m_pTankSound->EnableRef(this)) m_pTankSound->Stop();

    // 
    g_pGame->GetCrashManager()->SetImpactCallback(NULL);
}


// ****************************************************************
// 
void cTigerBoss::__RenderOwnBottom()
{
    if(m_aBeam[0].IsEnabled(CORE_OBJECT_ENABLE_RENDER))
    {
        DEPTH_PUSH

        // 
        //m_aBeam[0].Render();
        //g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aBeam[0]);
    }
}


// ****************************************************************
// 
void cTigerBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    //glDepthFunc(GL_ALWAYS);
    //{
    //    // 
    //    m_Sting.Render();
    //}
    //glDepthFunc(GL_LEQUAL);

    glDisable(GL_DEPTH_TEST);
    {
        // 
        m_AimRing.Render();

        // 
        for(coreUintW i = 0u; i < TIGER_AIMS; ++i)
            m_aAim[i].Render();
    }
    glEnable(GL_DEPTH_TEST);

    // 
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyList(&m_Sting);

    // 
    
   //         glCullFace(GL_FRONT);
        m_aBeam[0].Render();
        
  //          glCullFace(GL_BACK);
    m_aBeam[1].Render();
    //g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_aBeam[1]);
}


// ****************************************************************
// 
void cTigerBoss::__MoveOwn()
{
    cHarenaMission* pMission = d_cast<cHarenaMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        m_avVector[POS_OFFSET].y = -70.0f;

        const coreFloat fCurFlyOffset = g_pEnvironment->GetFlyOffset();
        const coreFloat fOldFlyOffset = fCurFlyOffset - TIME * g_pEnvironment->GetSpeed();

        if(F_TO_SI(fCurFlyOffset * (0.075f*PI) / (2.0f*PI)) != F_TO_SI(fOldFlyOffset * (0.075f*PI) / (2.0f*PI)))
        {
            g_pEnvironment->OverrideFlyOffset(RoundFactor(fCurFlyOffset, (0.075f*PI) / (2.0f*PI)));

            m_aWeapon[0].SetDirection(coreVector3(coreVector2(-0.5f,1.0f).Normalized(), 0.0f));
            this->__ShootWeapon();

            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TICKER(0u, 4u, 3.5f, LERP_LINEAR)
        {
            coreVector2 vDir;
            switch(iTick)
            {
            default: UNREACHABLE
            case 0u: vDir = coreVector2( 0.2f,1.0f); break;
            case 1u: vDir = coreVector2( 0.5f,1.0f); break;
            case 2u: vDir = coreVector2(-0.2f,1.0f); break;
            case 3u: vDir = coreVector2(-0.8f,1.0f); break;
            }

            m_aWeapon[0].SetDirection(coreVector3(vDir.Normalized(), 0.0f));
            this->__ShootWeapon();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                g_pSpecialEffects->ExternPlayPosition(m_pTankSound, this, 0.0f, 1.0f, true, SOUND_EFFECT, this->GetPosition());
            }

            if(m_pTankSound->EnableRef(this))
            {
                m_pTankSound->SetVolume(ParaLerp(0.0f, 1.0f, 0.2f, fTime));
            }

            m_avVector[POS_OFFSET].y = ParaLerp(-70.0f, -10.0f, -40.0f, fTime);

            if(PHASE_TIME_POINT(0.7f))
                this->_StartBoss();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(1u);

            PHASE_CHANGE_TO(20u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        if(pMission->GetStageSub() == 12u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(-40.0f, 0.0f, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 22u)
    {
        if(pMission->GetStageSub() == 13u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 23u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(0.0f, 40.0f, fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__SwitchWeapon(2u);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 24u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(2u);

            PHASE_CHANGE_TO(30u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        if(pMission->GetStageSub() == 2u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_SMOOTH)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(fTime * (-1.5f*PI)));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        if(pMission->GetStageSub() == 7u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(40.0f, 0.0f, fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__SwitchWeapon(3u);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 34u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(3u);

            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        if(pMission->GetStageSub() == 4u)
        {
            if(this->_ResurrectHelper(ELEMENT_YELLOW, false))
            {
                g_pGame->GetHelper(ELEMENT_YELLOW)->SetPosition(coreVector3(0.95f,1.2f,0.0f) * FOREGROUND_AREA3);
                m_avVector[HELPER_DATA].z = -1.0f;
            }
        }

        if(pMission->GetStageSub() == 7u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TIMER(0u, 0.1f, LERP_SMOOTH)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(fTime * (-1.5f*PI) + (-1.5f*PI)));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 42u)
    {
        if(pMission->GetStageSub() == 17u)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 43u)
    {
        //PHASE_CONTROL_PAUSE(0u, 0.5f)
        //{
        //    this->__SwitchWeapon(1u);
//
        //    PHASE_CHANGE_INC
        //});

        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(0.0f, -40.0f, fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__SwitchWeapon(1u);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 44u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(4u);

            PHASE_CHANGE_TO(50u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        if(this->GetCurHealth() <= TIGER_DAMAGE * 21)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        //PHASE_CONTROL_PAUSE(0u, 0.4f)   // longer
        //{
        //    this->__SwitchWeapon(4u);
//
        //    PHASE_CHANGE_INC
        //});

        PHASE_CONTROL_TIMER(0u, 0.25f, LERP_SMOOTH)
        {
            m_avVector[POS_OFFSET].y = LERP(-40.0f, 0.0f, fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                this->__SwitchWeapon(4u);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 52u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.3f)
        {
            pMission->ChangeInsanity(5u);

            this->RemoveStatus(ENEMY_STATUS_INVINCIBLE);

            PHASE_CHANGE_TO(60u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        constexpr coreUintW iHelperSpike = 35u;

        const coreObject3D* pBoard = pMission->GetSpikeBoard(iHelperSpike);

        if(pBoard->IsEnabled(CORE_OBJECT_ENABLE_MOVE) && pMission->GetSpikeLaunched(iHelperSpike))
        {
            if(this->_ResurrectHelper(ELEMENT_CYAN, true))
            {
                g_pGame->GetHelper(ELEMENT_CYAN)->SetPosition(pBoard->GetPosition());
            }
        }

        if(this->GetCurHealth() <= TIGER_DAMAGE * 5)
            PHASE_CHANGE_TO(80u)
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        if(PHASE_BEGINNING2)
        {
            this->AddStatus(ENEMY_STATUS_GHOST_PLAYER);

            this->_EndBoss();

            m_avVector[POS_OFFSET].y = 0.0f;

            for(coreUintW i = 0u; i < TIGER_SUBS; ++i)
            {
                m_aWeapon[i].Kill(false);
            }
            m_iWeaponType = 5u;

            g_pSpecialEffects->MacroExplosionPhysicalDarkBig(this->GetPosition());
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
        }

        PHASE_CONTROL_TICKER(4u, 0u, 60.0f, LERP_LINEAR)
        {
            g_pSpecialEffects->CreateBlowSmoke(this->GetPosition(), coreVector3(-g_pEnvironment->GetDirection(), 0.0f), 100.0f, 3u, coreVector3(1.0f,1.0f,1.0f));
        });

        PHASE_CONTROL_PAUSE(0u, 0.18f)
        {
            m_aiCounter[MANUAL_TRANSFORM] = 1;

            m_avVector[JUMP_DATA].x = this->GetPosition().z;

            if(m_pTankSound->EnableRef(this)) m_pTankSound->Stop();

            g_pSpecialEffects->MacroExplosionPhysicalDarkBig(this->GetPosition());
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
            g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);

            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 71u)
    {
        PHASE_CONTROL_TIMER(0u, 0.35f, LERP_LINEAR)
        {
            const coreVector2 vSide = g_pEnvironment->GetDirection().Rotated90();

            const coreMatrix3 mRota1 = coreMatrix4::RotationAxis(TIME * -12.0f, coreVector3(vSide, 0.5f).Normalized()).m123();
            const coreMatrix3 mRota2 = coreMatrix4::RotationAxis(TIME *   6.0f, coreVector3(vSide, 0.5f).Normalized()).m123();

            const coreFloat fHeight = m_avVector[JUMP_DATA].x + 70.0f * SIN(fTime * PI);

            this->SetPosition   (coreVector3(this->GetPosition().xy(), fHeight));
            this->SetDirection  ((this->GetDirection  () * mRota1).Normalized());
            this->SetOrientation((this->GetOrientation() * mRota1).Normalized());

            if(fHeight >= 0.0f) this->ChangeToTop();
                           else this->ChangeToBottom();

            for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrack); ++i)
            {
                m_aTrack[i].SetPosition   (this->GetPosition() + coreVector3(vSide * (fTime * 150.0f * (i ? 1.0f : -1.0f)), 0.0f));
                m_aTrack[i].SetDirection  ((m_aTrack[i].GetDirection  () * mRota2).Normalized());
                m_aTrack[i].SetOrientation((m_aTrack[i].GetOrientation() * mRota2).Normalized());
            }

            if(PHASE_FINISHED)
            {
                this->Kill(true);

                if(this->HasAllHelpers())
                {
                    this->_CreateFragment(3u);
                }

                g_pSpecialEffects->CreateExplosion (this->GetPosition());
                g_pSpecialEffects->CreateSplashDark(this->GetPosition(), 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.2f, 0.6f, SOUND_EFFECT_SHAKE_02);
                g_pSpecialEffects->SlowScreen(4.0f);

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "effect_soot.png");
                pObject->DefineProgram("effect_decal_single_program");

                // set object properties
                pObject->SetSize     (coreVector3(20.0f,20.0f,1.0f));
                pObject->SetDirection(coreVector3(0.0f,1.0f,0.0f));
                pObject->SetColor3   (coreVector3(0.0f,0.0f,0.0f));
                
                const coreVector3 vDecalPos = coreVector3(this->GetPosition().xy(), g_pEnvironment->RetrieveSafeHeight(this->GetPosition().xy()));

                // 
                g_pEnvironment->GetBackground()->AddDecal(pObject, vDecalPos, 1u, "effect_decal_single_inst_program", LIST_KEY);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        PHASE_CONTROL_TICKER(0u, 6u, 2.0f, LERP_LINEAR)
        {
            if(iTick < 5u)
            {
                this->__EnableAim(iTick);

                coreVector2 vPos;
                switch(iTick)
                {
                default: UNREACHABLE
                case 0u: vPos = coreVector2(-1.5f, 1.0f); break;
                case 1u: vPos = coreVector2( 1.5f,-0.5f); break;
                case 2u: vPos = coreVector2(-1.0f,-1.0f); break;
                case 3u: vPos = coreVector2( 1.0f, 0.5f); break;
                case 4u: vPos = coreVector2( 0.0f, 0.0f); break;
                }

                m_aAim[iTick].SetPosition(coreVector3(vPos * FOREGROUND_AREA * 0.5f, 0.0f));

                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_EFFECT_BEEP);
            }

            if(PHASE_FINISHED)
            {
                this->__DisableAim(0u, true);
                this->__EnableBeam(m_aAim[0].GetPosition().xy());
                this->__CauseBeamDamage(NULL);

                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        PHASE_CONTROL_TICKER(0u, 4u, 1.0f, LERP_LINEAR)
        {
            const coreUintW iIndex = iTick + 1u;

            this->__DisableBeam(false);

            this->__DisableAim(iIndex, true);
            this->__EnableBeam(m_aAim[iIndex].GetPosition().xy());
            this->__CauseBeamDamage(NULL);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 83u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 84u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->__EnableAim(0u);
            }

            constexpr coreFloat fLock = 0.75f - CORE_MATH_PRECISION;   // at a beep

            cPlayer* pTarget = this->NearestPlayerDual(m_aiCounter[BEAM_SHOT] % 2);

            if(PHASE_TIME_BEFORE(fLock))
            {
                m_vBeamPos = pTarget->GetPosition().xy();

                m_aAim[0].SetPosition(coreVector3(m_vBeamPos, 0.0f));
            }

            m_AimRing.SetSize  (m_aAim[0].GetSize() * LERPBR(3.0f, 1.3f, STEP(0.0f, fLock, fTime)));
            m_AimRing.SetColor3(PHASE_TIME_BEFORE(fLock) ? coreVector3(1.0f,1.0f,1.0f) : COLOR_MENU_MAGENTA);
            m_AimRing.SetAlpha (STEPH3(0.0f, fLock, fTime));

            PHASE_CONTROL_TICKER(1u, 3u, 4.0f * 0.5f, LERP_LINEAR)
            {
                g_pSpecialEffects->PlaySound(coreVector3(m_vBeamPos, 0.0f), 1.0f, 1.1f, SOUND_EFFECT_BEEP);
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
                PHASE_RESET(1u)

                this->__DisableAim(0u, true);
                this->__EnableBeam(m_vBeamPos);
                this->__CauseBeamDamage(pTarget);

                m_aiCounter[BEAM_SHOT] += 1;

                if(m_aiCounter[BEAM_SHOT] == 3u)
                {
                    if(this->_ResurrectHelper(ELEMENT_MAGENTA, true))
                    {
                        g_pGame->GetHelper(ELEMENT_MAGENTA)->SetPosition(coreVector3(m_vBeamPos, 0.0f));
                    }
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 85u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(84u)
        });
    }

    // ################################################################
    // ################################################################

    if(m_iPhase >= 60u)
    {
        m_avVector[VEIL_VALUE].x = MAX0(m_avVector[VEIL_VALUE].x - 0.1f * TIME);
    }
    else if(m_iPhase >= 31u)
    {
        m_avVector[VEIL_VALUE].x = MIN1(m_avVector[VEIL_VALUE].x + 0.1f * TIME);
    }

    cDesertBackground* pBackground = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground());
    pBackground->SetVeilAlpha(BLENDS(m_avVector[VEIL_VALUE].x));

    if(m_iPhase >= 60u)
    {
        m_avVector[FINAL_ROTA].x += 1.0f * TIME;

        const coreFloat fRotaTime = BLENDBR(MIN1(m_avVector[FINAL_ROTA].x * 0.5f)) * 2.0f + MAX0(m_avVector[FINAL_ROTA].x - 2.0f);
        g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(fRotaTime * (-0.1f*PI) + (-3.0f*PI)));
    }

    // 
    const coreVector2 vEnvDirection = g_pEnvironment->GetDirection();
    const coreFloat   fEnvSpeed     = g_pEnvironment->GetSpeed();
    const coreFloat   fEnvFlyOffset = g_pEnvironment->GetFlyOffset() * OUTDOOR_DETAIL;
    ASSERT(fEnvSpeed > 0.0f)

    if(!m_aiCounter[MANUAL_TRANSFORM])
    {
        // 
        const coreFloat   fFullOffset = fEnvFlyOffset + m_avVector[POS_OFFSET].y;
        const coreVector2 vGroundPos  = coreVector2(SIN(fFullOffset / OUTDOOR_DETAIL * (0.075f*PI)) * -12.5f, fFullOffset);

        // 
        const coreFloat   fPrevFullOffset = fFullOffset - fEnvSpeed;   // approximate
        const coreVector2 vPrevGroundPos  = coreVector2(SIN(fPrevFullOffset / OUTDOOR_DETAIL * (0.075f*PI)) * -12.5f, fPrevFullOffset);

        // 
        const coreVector2 vBodyPos    = MapToAxis(coreVector2(vGroundPos.x, m_avVector[POS_OFFSET].y), vEnvDirection);
        const coreFloat   fBodyHeight = LERP(this->GetPosition().z, g_pEnvironment->RetrieveSafeHeight(vBodyPos) + 2.0f, m_fLifeTimeBefore ? TIME : 1.0f);
        const coreVector2 vBodyDir    = MapToAxis(vGroundPos - vPrevGroundPos, vEnvDirection).Normalized(this->GetDirection().xy());

        // 
        this->SetPosition (coreVector3(vBodyPos, fBodyHeight));
        this->SetDirection(coreVector3(vBodyDir, 0.0f));

        // 
        m_fAnimation.Update(fEnvSpeed * -0.25f * 0.5f);

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aTrack); ++i)
        {
            // 
            m_aTrack[i].SetPosition   (this->GetPosition   ());
            m_aTrack[i].SetDirection  (this->GetDirection  ());
            m_aTrack[i].SetOrientation(this->GetOrientation());
            m_aTrack[i].SetColor3     (coreVector3(1.0f,1.0f,1.0f));   // override brightness
            m_aTrack[i].SetTexOffset  (coreVector2(0.0f, FRACT(m_fAnimation)));
        }

        // 
        m_aWeapon[0].SetPosition   (this->GetPosition   ());
        m_aWeapon[0].SetOrientation(this->GetOrientation());

        if(!m_aiCounter[WEAPON_SHOT]) m_aWeapon[0].SetDirection(this->GetDirection());          

        // 
        const coreVector2 vTrailPos1 = vBodyPos - vBodyDir * 8.0f + vBodyDir.Rotated90() * 6.8f;
        const coreVector2 vTrailPos2 = vBodyPos - vBodyDir * 8.0f - vBodyDir.Rotated90() * 6.8f;
        this->__CreateTrail(0u, coreVector3(vTrailPos1, g_pEnvironment->RetrieveSafeHeight(vTrailPos1) - 0.0f));
        this->__CreateTrail(1u, coreVector3(vTrailPos2, g_pEnvironment->RetrieveSafeHeight(vTrailPos2) - 0.0f));
    }

    if(m_iPhase >= 3u)
    {
        if(m_iWeaponType < 4u)
        {
            coreFloat fShootSpeed;
            switch(m_iWeaponType)
            {
            default: UNREACHABLE
            case 0u: fShootSpeed = 0.5f; break;
            case 1u: fShootSpeed = 2.0f; break;
            case 2u: fShootSpeed = 1.0f; break;
            case 3u: fShootSpeed = 0.3f; break;
            }

            PHASE_CONTROL_TIMER(3u, fShootSpeed, LERP_LINEAR)
            {
                if(PHASE_BEGINNING)
                {
                    m_aiCounter[WEAPON_SHOT] += 1;
                }

                const coreVector2 vTarget = this->NearestPlayerDual(((m_aiCounter[WEAPON_SHOT] - 1) / ((m_iWeaponType == 1u) ? 4 : 2)) % 2)->GetPosition().xy();
                const coreVector2 vAim    = (vTarget - g_pForeground->Project3D(this->GetPosition())).Normalized();
                const coreVector2 vNewDir = SmoothAim(m_aWeapon[0].GetDirection().xy(), vAim, 5.0f);

                m_aWeapon[0].SetDirection(coreVector3(vNewDir, 0.0f));

                if(PHASE_FINISHED)
                {
                    this->__ShootWeapon();
                    PHASE_RESET(3u)
                }
            });
        }
        else if(m_iWeaponType == 4u)
        {
            PHASE_CONTROL_TIMER(3u, ((m_iPhase >= 83u) && (m_iPhase < 90u)) ? 2.0f : 1.0f, LERP_LINEAR)
            {
                if(PHASE_BEGINNING)
                {
                    m_aiCounter[WEAPON_SHOT] += 1;
                }

                const coreVector2 vNewDir = coreVector2::Direction(m_fLifeTime * 2.0f);

                m_aWeapon[0].SetDirection(coreVector3(vNewDir, 0.0f));

                if(PHASE_FINISHED)
                {
                    if(m_aiCounter[WEAPON_SHOT] % 4 == 2)
                    {
                        const cPlayer* pPlayer = this->NearestPlayerDual((m_aiCounter[WEAPON_SHOT] / 4) % 2);

                        const coreVector2 vAim  = (pPlayer->GetPosition().xy() - g_pForeground->Project3D(this->GetPosition())).Normalized();
                        const coreVector2 vBase = g_pForeground->Project3D(m_aWeapon[0].GetPosition());

                        for(coreUintW i = 4u; i--; )
                        {
                            if(g_pGame->IsEasy() && ((i == 0u) || (i == 3u))) continue;

                            const coreVector2 vDir = MapStepRotated90X(vAim, i);
                            const coreVector2 vPos = vBase + vDir * 3.0f;

                            g_pGame->GetBulletManagerEnemy()->AddBullet<cRocketBullet>(5, 1.0f, this, vPos, vDir)->SetTarget(pPlayer)->ChangeSize(1.5f);
                        }

                        g_pSpecialEffects->PlaySound(coreVector3(vBase, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_12);

                        m_avVector[RECOIL_TIME].x = 1.0f;
                    }

                    PHASE_RESET(3u)
                }
            });
        }
        else if(m_iWeaponType == 5u)
        {
            // no weapon
        }
        else UNREACHABLE
    }
    

    if(m_aBeam[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        if(m_vBeamPos.IsNull()) m_vBeamPos = coreVector2(0.0f, CORE_MATH_PRECISION);
        
       // const coreFloat   fTempHeight = g_pEnvironment->RetrieveSafeHeight(m_vBeamPos);
        const coreVector2 vRealPos = m_vBeamPos;// * 1.2f;

        const coreFloat   fBeamHeight = g_pEnvironment->RetrieveSafeHeight(vRealPos);
        //const coreVector2 vBeamCenter = m_vBeamPos.Normalized() * -10.0f;// * (1.0f - 
        const coreVector2 vBeamCenter = vRealPos + vRealPos.Normalized() * 11.0f;// * (1.0f - STEP(0.0f, 50.0f, m_vBeamPos.Length()));
        const coreVector3 vBeamDir    = (coreVector3(vRealPos, fBeamHeight) - coreVector3(vBeamCenter, 0.0f)).Normalized();
        //const coreVector3 vBeamOri    = coreVector3(vBeamCenter.Normalized().x, coreVector3::Cross(vBeamDir, coreVector3(vBeamDir.xy().Normalized().Rotated90(), 0.0f)).yz()).Normalized();
        const coreVector3 vBeamOri    = coreVector3(vBeamCenter.Normalized(), 0.0f);
        
        
        

        // 
        m_fBeamTime.Update(1.5f);
        const coreFloat fTime = m_fBeamTime;

        // 
        if(fTime >= 1.0f) this->__DisableBeam(false);
        

        // 
        const coreFloat fLerp  = 1.0f - POW(MAX0(fTime - 0.0f), 9.0f);
        const coreFloat fBlend = CLAMP01(fTime * 20.0f);
        const coreFloat fSize  = LERP(1.0f, 4.0f, fLerp) * fBlend * 1.5f;
        const coreFloat fAlpha = LERP(0.0f, 1.0f, fLerp) * fBlend;
        const coreFloat fTex   = (m_fAnimation * -1.0f);
        
            // 
            const coreVector3 vPos = coreVector3(vRealPos, fBeamHeight) - vBeamDir * m_aBeam[0].GetSize().y;

            // 
            m_aBeam[0].SetPosition   (vPos);
            m_aBeam[0].SetDirection  (vBeamDir);
            m_aBeam[0].SetOrientation(vBeamOri);
            m_aBeam[0].SetSize       (coreVector3(fSize, m_aBeam[0].GetSize().y, fSize));
            m_aBeam[0].SetAlpha      (fAlpha);
            m_aBeam[0].SetTexOffset  (coreVector2(1.0f,1.0f) * -fTex);
            m_aBeam[0].Move();
            
            
            // 
            const coreFloat fLen = (20.0f - vRealPos.Processed(ABS).Max() / 50.0f * 15.0f) * 1.0f;
            const coreVector3 vPos2 = coreVector3(vRealPos, fBeamHeight) - vBeamDir * fLen * 1.0f;

            // 
            m_aBeam[1].SetPosition   (vPos2);
            m_aBeam[1].SetDirection  (vBeamDir);
            m_aBeam[1].SetOrientation(vBeamOri);
            m_aBeam[1].SetSize       (coreVector3(fSize * 1.1f, fLen, fSize * 1.3f));
            m_aBeam[1].SetAlpha      (fAlpha);
            m_aBeam[1].SetTexOffset  (coreVector2(0.5f,0.5f) * fTex);
            m_aBeam[1].Move();
        

        this->__CreateOverdrive(coreVector3(vRealPos, fBeamHeight));
    }

    const coreVector2 vAimDir = coreVector2::Direction((1.5f*PI) * m_fAnimation);

    for(coreUintW i = 0u; i < TIGER_AIMS; ++i)
    {
        coreObject3D& oAim = m_aAim[i];
        if(!oAim.IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        oAim.SetDirection(coreVector3(vAimDir, 0.0f));
        oAim.SetAlpha    (MIN1(oAim.GetAlpha() + 5.0f * TIME));
        oAim.Move();
    }

    if(m_AimRing.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_AimRing.SetPosition (m_aAim[0].GetPosition ());
        m_AimRing.SetDirection(m_aAim[0].GetDirection().InvertedX());
        m_AimRing.Move();
    }

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([](cTriangleBullet* OUTPUT pBullet)
    {
        const coreInt32 iLimit = g_pGame->IsEasy() ? 3 : 2;

        if(pBullet->GetDamage() > iLimit)
        {
            coreVector2 vCurPos = pBullet->GetPosition().xy();
            coreVector2 vCurDir = pBullet->GetFlyDir();
            coreInt32   iDamage = pBullet->GetDamage();

                 if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.1f); vCurDir.x =  ABS(vCurDir.x); iDamage -= 1;}
            else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.1f); vCurDir.x = -ABS(vCurDir.x); iDamage -= 1;}
            else if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.1f); vCurDir.y =  ABS(vCurDir.y); iDamage -= 1;}   // # else
            else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.1f); vCurDir.y = -ABS(vCurDir.y); iDamage -= 1;}
            ASSERT(iDamage >= iLimit)

            pBullet->SetPosition(coreVector3(vCurPos, 0.0f));
            pBullet->SetFlyDir  (vCurDir);
            pBullet->SetDamage  (iDamage);
        }
        else pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
    });

    const coreFloat fVelocity = (g_pGame->IsEasy() ? 3.0f : 2.0f) * TIME;

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([&](cConeBullet* OUTPUT pBullet)
    {
        const coreFloat fNewSpeed = pBullet->GetSpeed() - fVelocity;

        pBullet->SetSpeed(fNewSpeed);
        if(fNewSpeed <= 0.0f) pBullet->Deactivate(true);
    });

    //const coreMatrix2 mBulletRota = coreMatrix3::Rotation(0.7f * TIME).m12();

    //g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([&](cFlipBullet* OUTPUT pBullet)
    //{
    //    //pBullet->SetPosition(coreVector3(pBullet->GetPosition().xy() * mBulletRota, 0.0f));
    //    pBullet->SetFlyDir  (pBullet->GetFlyDir() * mBulletRota);
    //});



    if(m_fWeaponChange < 3.0f)
    {
        m_fWeaponChange.Update(1.0f);

        m_aWeapon[0].SetSize(this->GetSize() * BLENDB(MIN1(m_fWeaponChange)) * 1.3f * (((m_iWeaponType == 1u) || (m_iWeaponType == 2u) || (m_iWeaponType == 4u)) ? 1.05f : 1.0f));

        const coreVector3 vPos  = m_aWeaponOld[0].GetPosition() + coreVector3(vEnvDirection * (fEnvSpeed * TIME * -10.0f), 0.0f);
        const coreMatrix3 mRota = coreMatrix4::RotationZ(-15.0f * TIME).m123();

        m_aWeaponOld[0].SetPosition   (vPos);
        m_aWeaponOld[0].SetDirection  ((m_aWeaponOld[0].GetDirection  () * mRota).Normalized());
        m_aWeaponOld[0].SetOrientation((m_aWeaponOld[0].GetOrientation() * mRota).Normalized());

        m_aWeaponOld[0].RemoveStatus(ENEMY_STATUS_HIDDEN);
    }
    else
    {
        m_aWeaponOld[0].AddStatus(ENEMY_STATUS_HIDDEN);
    }


    // 
    coreFloat afPrevStingTime[TIGER_SIDES];
    std::memcpy(afPrevStingTime, m_afStingTime, sizeof(m_afStingTime));

    for(coreUintW i = 0u; i < TIGER_SIDES; ++i)
    {
        if(m_afStingTime[i] >= 0.0f) m_afStingTime[i].UpdateMin(20.0f, 30.0f);
    }

    // 
    coreRand oStingRand(20u);
    for(coreUintW i = 0u; i < TIGER_STINGS; ++i)
    {
        const coreVector2 vSide  = (i < TIGER_STINGS_SIDE * 2u) ? coreVector2((i < TIGER_STINGS_SIDE * 1u) ? -1.0f : 1.0f, 0.0f) : coreVector2(0.0f, (i < TIGER_STINGS_SIDE * 3u) ? -1.0f : 1.0f);
        const coreFloat   fShift = I_TO_F(i % TIGER_STINGS_SIDE) - I_TO_F(TIGER_STINGS_SIDE - 1u) * 0.5f;

        const coreUintW iIndex      = i / TIGER_STINGS_SIDE;
        const coreFloat fPrevExtend = CLAMP01((afPrevStingTime[iIndex] - ABS(fShift) - 1.0f) * 0.3f);
        const coreFloat fExtend     = CLAMP01((m_afStingTime  [iIndex] - ABS(fShift) - 1.0f) * 0.3f);

        const coreVector2 vBase = (vSide.Rotated90() * (fShift * 0.062f) + vSide) * FOREGROUND_AREA * 1.15f;
        const coreVector2 vDir  = (-vBase.Normalized() + 0.5f * coreVector2(oStingRand.Float(-1.0f, 1.0f), oStingRand.Float(-1.0f, 1.0f)).Normalized()).Normalized();
        const coreVector2 vPos  = vBase + vDir * LERP(-12.0f, 0.0f, fExtend);

        m_aStingRaw[i].SetPosition   (coreVector3(vPos, 0.0f));
        m_aStingRaw[i].SetOrientation(coreVector3(vDir, 0.0f));
        m_aStingRaw[i].SetEnabled    (fExtend ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);

        if((fPrevExtend < 0.3f) && (fExtend >= 0.3f))
        {
            g_pSpecialEffects->CreateSplashColor(coreVector3(vBase + vDir * 2.0f, 0.0f), 5.0f, 3u, COLOR_ENERGY_WHITE * 0.8f);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
        }
    }

    // 
    m_Sting.MoveNormal();

    // 
    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        if(pPlayer->IsNormal())
        {
            const coreVector2 vPos = pPlayer->GetPosition().xy();

            if(((vPos.x <= -FOREGROUND_AREA.x + CORE_MATH_PRECISION) && (m_afStingTime[0] >= 30.0f)) ||
               ((vPos.x >=  FOREGROUND_AREA.x - CORE_MATH_PRECISION) && (m_afStingTime[1] >= 30.0f)) ||
               ((vPos.y <= -FOREGROUND_AREA.y + CORE_MATH_PRECISION) && (m_afStingTime[2] >= 30.0f)) ||
               ((vPos.y >=  FOREGROUND_AREA.y - CORE_MATH_PRECISION) && (m_afStingTime[3] >= 30.0f)))
            {
                pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vPos);
            }
        }
    });




    // 
    if(m_bPushState) m_fPushPower.UpdateMin( 1.0f, 1.0f);
                else m_fPushPower.UpdateMax(-1.0f, 0.0f);

    // 
    if(m_fPushPower)
    {
        // 
        const coreFloat fPower = BLENDH3(m_fPushPower);

        // 
        const coreVector2 vRealPushDir = MapToAxis(m_avPushDir[1], vEnvDirection);

        // 
        m_avPushDir[0] = LERP(m_avPushDir[0], m_avPushDir[1], TIME * 3.0f);
        d_cast<cDesertBackground*>(g_pEnvironment->GetBackground())->SetSandMove(m_avPushDir[0] * (1.0f + 3.0f * fPower));

        // 
        const coreVector2 vPlayerForce = vRealPushDir * (200.0f * fPower);
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->ApplyForceTimed(vPlayerForce);
        });

        // 
        const coreVector2 vBulletForce = vRealPushDir * (TIME * 16.0f * fPower);
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cMineBullet>([&](cMineBullet* OUTPUT pBullet)
        {
            const coreVector2 vPos = pBullet->GetPosition().xy() + vBulletForce;

            if(((vPos.x < -FOREGROUND_AREA.x * 1.2f) && (vBulletForce.x < 0.0f)) ||
               ((vPos.x >  FOREGROUND_AREA.x * 1.2f) && (vBulletForce.x > 0.0f)) ||
               ((vPos.y < -FOREGROUND_AREA.y * 1.2f) && (vBulletForce.y < 0.0f)) ||
               ((vPos.y >  FOREGROUND_AREA.y * 1.2f) && (vBulletForce.y > 0.0f)))
            {
                pBullet->Deactivate(false);
            }

            pBullet->SetPosition(coreVector3(vPos, 0.0f));
        });
    }


    for(coreUintW i = 1u; i < TIGER_SUBS; ++i)
    {
        m_aWeapon[i].SetPosition   (m_aWeapon[0].GetPosition   ());
        m_aWeapon[i].SetSize       (m_aWeapon[0].GetSize       ());
        m_aWeapon[i].SetDirection  (m_aWeapon[0].GetDirection  ());
        m_aWeapon[i].SetOrientation(m_aWeapon[0].GetOrientation());

        m_aWeaponOld[i].SetPosition   (m_aWeaponOld[0].GetPosition   ());
        m_aWeaponOld[i].SetSize       (m_aWeaponOld[0].GetSize       ());
        m_aWeaponOld[i].SetDirection  (m_aWeaponOld[0].GetDirection  ());
        m_aWeaponOld[i].SetOrientation(m_aWeaponOld[0].GetOrientation());

        if(!m_aWeapon[i].GetModel() || m_aWeapon[0].HasStatus(ENEMY_STATUS_HIDDEN))
             m_aWeapon[i].AddStatus   (ENEMY_STATUS_HIDDEN);
        else m_aWeapon[i].RemoveStatus(ENEMY_STATUS_HIDDEN);

        if(!m_aWeaponOld[i].GetModel() || m_aWeaponOld[0].HasStatus(ENEMY_STATUS_HIDDEN))
             m_aWeaponOld[i].AddStatus   (ENEMY_STATUS_HIDDEN);
        else m_aWeaponOld[i].RemoveStatus(ENEMY_STATUS_HIDDEN);
    }
    
    if(m_iWeaponType == 1u)
    {
        const coreVector3 vOffset = m_aWeapon[0].GetDirection().RotatedZ90() * (0.95f * m_aWeapon[0].GetSize().x);

        m_aWeapon[1].SetPosition(m_aWeapon[0].GetPosition() + vOffset);
        m_aWeapon[2].SetPosition(m_aWeapon[0].GetPosition() - vOffset);
    }
    if(m_iWeaponTypeOld == 1u)
    {
        const coreVector3 vOffset = m_aWeaponOld[0].GetDirection().RotatedZ90() * (0.95f * m_aWeaponOld[0].GetSize().x);

        m_aWeaponOld[1].SetPosition(m_aWeaponOld[0].GetPosition() + vOffset);
        m_aWeaponOld[2].SetPosition(m_aWeaponOld[0].GetPosition() - vOffset);
    }

    if(m_iWeaponType == 4u)
    {
        const coreVector3 vDir = m_aWeapon[0].GetDirection();

        for(coreUintW i = 1u; i < TIGER_SUBS; ++i)
        {
            m_aWeapon[i].SetDirection(MapToAxis(vDir, StepRotated90X(i % 4u)));
        }
    }
    if(m_iWeaponTypeOld == 4u)
    {
        const coreVector3 vDir = m_aWeaponOld[0].GetDirection();

        for(coreUintW i = 1u; i < TIGER_SUBS; ++i)
        {
            m_aWeaponOld[i].SetDirection(MapToAxis(vDir, StepRotated90X(i % 4u)));
        }
    }

    m_avVector[RECOIL_TIME].xy((m_avVector[RECOIL_TIME].xy() - 1.0f * TIME).Processed(MAX, 0.0f));

    if(m_iWeaponType == 0u)
    {
        m_aWeapon[1].SetPosition(m_aWeapon[1].GetPosition() - m_aWeapon[1].GetDirection() * (2.8f * m_aWeapon[1].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x)));
    }
    else if(m_iWeaponType == 1u)
    {
        m_aWeapon[1].SetPosition(m_aWeapon[1].GetPosition() - m_aWeapon[1].GetDirection() * (2.5f * m_aWeapon[1].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x)));
        m_aWeapon[2].SetPosition(m_aWeapon[2].GetPosition() - m_aWeapon[2].GetDirection() * (2.5f * m_aWeapon[2].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].y)));
    }
    else if(m_iWeaponType == 2u)
    {
        m_aWeapon[1].SetPosition(m_aWeapon[1].GetPosition() - m_aWeapon[1].GetDirection() * ((-0.5f + 1.0f * (1.0f - STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x))) * m_aWeapon[1].GetSize().x));
    }
    else if(m_iWeaponType == 3u)
    {
        m_aWeapon[1].SetPosition(m_aWeapon[1].GetPosition() - m_aWeapon[1].GetDirection() * ((-0.7f + 1.6f * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x)) * m_aWeapon[1].GetSize().x));
    }
    else if(m_iWeaponType == 4u)
    {
        for(coreUintW i = 1u; i < TIGER_SUBS; ++i)
        {
            m_aWeapon[i].SetPosition(m_aWeapon[i].GetPosition() - m_aWeapon[i].GetDirection() * (0.8f * m_aWeapon[i].GetSize().x * STEP(0.5f, 1.0f, m_avVector[RECOIL_TIME].x)));
        }
    }

    // 
    if(m_pTankSound->EnableRef(this))
    {
        g_pSpecialEffects->ExternSetSource(m_pTankSound, this->GetPosition());
    }

    // 
    pMission->PlayInsanity();

    // 
    pMission->SetCollPlayerBullet([COLL_THIS](const cPlayer* pPlayer, const cBullet* pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(!bFirstHit) return;
        if(!pPlayer->IsNormal()) return;

        switch(pBullet->GetID())
        {
        case cConeBullet    ::ID: m_aiBulletHit[0] += 1u; break;
        case cTriangleBullet::ID: m_aiBulletHit[1] += 1u; break;
        case cFlipBullet    ::ID: m_aiBulletHit[2] += 1u; break;
        case cWaveBullet    ::ID: m_aiBulletHit[3] += 1u; break;
        case cRocketBullet  ::ID: m_aiBulletHit[4] += 1u; break;
        }

        if(std::all_of(m_aiBulletHit, m_aiBulletHit + TIGER_WEAPONS, [](const coreUint16 A) {return (A == 1u);}))
        {
            g_pGame->GetCurMission()->GiveBadge(3u, BADGE_ACHIEVEMENT, pPlayer->GetPosition());
        }
        else if(std::any_of(m_aiBulletHit, m_aiBulletHit + TIGER_WEAPONS, [](const coreUint16 A) {return (A > 1u);}))
        {
            g_pGame->GetCurMission()->FailTrophy();
        }
    });
    
    

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(70u)
    }
    
    
    // 
    cHelper* pYellowHelper = g_pGame->GetHelper(ELEMENT_YELLOW);
    if(!pYellowHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        coreVector2 vNewPos = pYellowHelper->GetPosition().xy() + coreVector2(0.0f, 60.0f * TIME * m_avVector[HELPER_DATA].z);

             if((vNewPos.y < -FOREGROUND_AREA.y * 1.1f) && (m_avVector[HELPER_DATA].z < 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.1f); m_avVector[HELPER_DATA].z = 1.0f;}
        else if((vNewPos.y >  FOREGROUND_AREA.y * 1.1f) && (m_avVector[HELPER_DATA].z > 0.0f)) {this->_KillHelper(ELEMENT_YELLOW, false);}

        pYellowHelper->SetPosition(coreVector3(vNewPos, 0.0f));
    }

    // 
    cHelper* pOrangeHelper = g_pGame->GetHelper(ELEMENT_ORANGE);
    if(!pOrangeHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        if(pOrangeHelper->GetLifeTime() >= (1.0f/0.75f)) this->_KillHelper(ELEMENT_ORANGE, false);
    }

    // 
    cHelper* pRedHelper = g_pGame->GetHelper(ELEMENT_RED);
    if(!pRedHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pRedHelper->GetPosition().xy() + m_avVector[HELPER_DATA].xy() * (1.4f * BULLET_SPEED_FACTOR * TIME);

        pRedHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pRedHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_RED, false);
        }
    }

    // 
    cHelper* pMagentaHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);
    if(!pMagentaHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pMagentaHelper->GetPosition().xy() + pMagentaHelper->GetPosition().xy().Normalized() * ((pMagentaHelper->GetLifeTime() * 20.0f + 10.0f) * TIME);

        pMagentaHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pMagentaHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_MAGENTA, false);
        }
    }

    // 
    cHelper* pPurpleHelper = g_pGame->GetHelper(ELEMENT_PURPLE);
    if(!pPurpleHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pPurpleHelper->GetPosition().xy() + AlongCrossNormal(pPurpleHelper->GetPosition().xy()) * (MAX0(pPurpleHelper->GetLifeTime() - 1.0f) * 1.5f * TIME);

        pPurpleHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pPurpleHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_PURPLE, true);
        }
    }

    // 
    cHelper* pBlueHelper = g_pGame->GetHelper(ELEMENT_BLUE);
    if(!pBlueHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        // handled in mission code
    }

    // 
    cHelper* pCyanHelper = g_pGame->GetHelper(ELEMENT_CYAN);
    if(!pCyanHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        if(pCyanHelper->GetLifeTime() >= 2.0f) this->_KillHelper(ELEMENT_CYAN, true);
    }

    // 
    cHelper* pGreenHelper = g_pGame->GetHelper(ELEMENT_GREEN);
    if(!pGreenHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pGreenHelper->GetPosition().xy() + coreVector2(1.0f,0.0f) * (50.0f * TIME);

        pGreenHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pGreenHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_GREEN, true);
        }
    }
}


// ****************************************************************
// 
void cTigerBoss::__EnableBeam(const coreVector2 vPosition)
{
    // 
    WARN_IF(m_aBeam[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableBeam(false);

    
    const coreVector2 vRealPos = vPosition * 1.2f;
    
    // 
    m_vBeamPos  = vRealPos;
    m_fBeamTime = 0.0f;

    // 
    m_avVector[OVERDRIVE_HIT].xyz(coreVector3(0.0f,0.0f,0.0f));

    // 
    const auto nInitFunc = [&](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    nInitFunc(&m_aBeam[0]);
    nInitFunc(&m_aBeam[1]);
    
    const coreVector3 vImpact = coreVector3(vRealPos, g_pEnvironment->RetrieveSafeHeight(vRealPos));

    g_pSpecialEffects->MacroExplosionColorBig(vImpact, COLOR_ENERGY_MAGENTA);
    g_pSpecialEffects->PlaySound(vImpact, 0.8f, 1.0f, SOUND_EFFECT_FIRE);
    g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_BIG, 250u);
    
    
    
    g_pSpecialEffects->CreateSplashFire(vImpact, 50.0f, 200u, COLOR_ENERGY_MAGENTA);
    g_pSpecialEffects->CreateSplashColor(vImpact, 100.0f, 50u, COLOR_ENERGY_MAGENTA);
}


// ****************************************************************
// 
void cTigerBoss::__DisableBeam(const coreBool bAnimated)
{
    // 
    if(!m_aBeam[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    if(!bAnimated)
    {
        // 
        const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
        {
            pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            g_pGlow->UnbindObject(pObject);
        };
        nExitFunc(&m_aBeam[0]);
        nExitFunc(&m_aBeam[1]);
    }
}


// ****************************************************************
// 
void cTigerBoss::__EnableAim(const coreUintW iIndex)
{
    ASSERT(iIndex < TIGER_AIMS)
    coreObject3D& oAim = m_aAim[iIndex];

    // 
    WARN_IF(oAim.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableAim(iIndex, false);

    // 
    oAim.SetAlpha  (0.0f);
    oAim.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    
    if(iIndex == 0u)
    {
        m_AimRing.SetAlpha  (0.0f);
        m_AimRing.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    }
}


// ****************************************************************
// 
void cTigerBoss::__DisableAim(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TIGER_AIMS)
    coreObject3D& oAim = m_aAim[iIndex];

    // 
    if(!oAim.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    oAim.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    
    if(iIndex == 0u)
    {
        m_AimRing.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cTigerBoss::__EnableStings(const coreUintW iIndex)
{
    ASSERT(iIndex < TIGER_SIDES)

    //   3
    // 0   1
    //   2

    // 
    ASSERT(m_afStingTime[iIndex] < 0.0f)
    m_afStingTime[iIndex] = 0.0f;
}


// ****************************************************************
// 
void cTigerBoss::__DisableStings(const coreUintW iIndex, const coreBool bAnimated)
{
    ASSERT(iIndex < TIGER_SIDES)

    // 
    const coreUintW iFrom = TIGER_STINGS_SIDE * (iIndex);
    const coreUintW iTo   = TIGER_STINGS_SIDE * (iIndex + 1u);
    ASSERT(iTo <= TIGER_STINGS)

    // 
    for(coreUintW i = iFrom; i < iTo; ++i)
    {
        m_aStingRaw[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_afStingTime[iIndex] = -1.0f;

    // TODO 1: implement animation 
}


// ****************************************************************
// 
void cTigerBoss::__EnableWind(const coreVector2 vDir)
{
    ASSERT(vDir.IsNormalized())

    // 
    m_avPushDir[0] = d_cast<cDesertBackground*>(g_pEnvironment->GetBackground())->GetSandMove().Normalized();
    m_avPushDir[1] = vDir;
    m_bPushState   = true;
}


// ****************************************************************
// 
void cTigerBoss::__DisableWind()
{
    // 
    m_bPushState = false;
}


// ****************************************************************
// 
void cTigerBoss::__SwitchWeapon(const coreUintW iType)
{
    ASSERT(iType < TIGER_WEAPONS)

    // 0: orange
    // 1: green
    // 2: red
    // 3: purple
    // 4: rocket
    // 5: nothing

    for(coreUintW i = 0u; i < TIGER_SUBS; ++i)
    {
        // 
        m_aWeaponOld[i].DefineModelHigh(m_aWeapon[i].GetModelHigh  ());
        m_aWeaponOld[i].DefineModelLow (m_aWeapon[i].GetModelLow   ());
        m_aWeaponOld[i].SetPosition    (m_aWeapon[i].GetPosition   ());
        m_aWeaponOld[i].SetSize        (m_aWeapon[i].GetSize       ());
        m_aWeaponOld[i].SetDirection   (m_aWeapon[i].GetDirection  ());
        m_aWeaponOld[i].SetOrientation (m_aWeapon[i].GetOrientation());

        // 
        m_aWeapon[i].DefineModelHigh(m_aapModelHigh[iType][i]);
        m_aWeapon[i].DefineModelLow (m_aapModelLow [iType][i]);
    }

    // 
    m_iWeaponTypeOld = m_iWeaponType;
    m_iWeaponType    = iType;
    m_fWeaponChange  = 0.0f;

    // 
    if(!this->HasStatus(ENEMY_STATUS_DEAD))
    {
        g_pSpecialEffects->MacroExplosionPhysicalColorSmall(this->GetPosition(), COLOR_FIRE_ORANGE);
        g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_03);
    }

    // 
    if(((m_iWeaponTypeOld == 0u) && !m_aiBulletHit[0]) ||
       ((m_iWeaponTypeOld == 1u) && !m_aiBulletHit[3]) ||
       ((m_iWeaponTypeOld == 2u) && !m_aiBulletHit[1]) ||
       ((m_iWeaponTypeOld == 3u) && !m_aiBulletHit[2]) ||
       ((m_iWeaponTypeOld == 4u) && !m_aiBulletHit[4]))
    {
        g_pGame->GetCurMission()->FailTrophy();
    }

    // 
    PHASE_RESET(3u)
    m_aiCounter[WEAPON_SHOT] = 0;
}


// ****************************************************************
// 
void cTigerBoss::__ShootWeapon()
{
    if((m_iWeaponType != 1u) || ((m_aiCounter[WEAPON_SHOT] - 1) % 4 >= 2))
    {
        const coreVector2 vWeaponPos = g_pForeground->Project3D(m_aWeapon[0].GetPosition());
        const coreVector2 vWeaponDir = m_aWeapon[0].GetDirection().xy();
        const coreVector2 vHit       = vWeaponPos + vWeaponDir * g_pForeground->RayIntersection(vWeaponPos, vWeaponDir);

        if(m_iWeaponType == 0u)
        {
            const coreVector2 vBase = -AlongCrossNormal(vHit).Rotated90();

            for(coreUintW i = 37u; i--; )
            {
                const coreVector2 vDir = MapToAxis(coreVector2::Direction(I_TO_F(i) * (2.0f*PI) / 72.0f), vBase);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 2.0f, this, vHit, vDir)->ChangeSize(1.6f);
            }

            g_pSpecialEffects->PlaySound(coreVector3(vHit, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_01);

            m_avVector[RECOIL_TIME].x = 1.0f;
        }
        else if(m_iWeaponType == 1u)
        {
            const coreVector2 vDir = -AlongCrossNormal(vHit);

            for(coreUintW i = g_pGame->IsEasy() ? 4u : 6u; i--; )
            {
                const coreVector2 vPos1 = vHit + vDir.Rotated90() * (I_TO_F(i) * 2.2f);
                const coreVector2 vPos2 = vHit - vDir.Rotated90() * (I_TO_F(i) * 2.2f);

                      g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f - I_TO_F(i) * 0.035f, this, vPos1, vDir)->ChangeSize(1.5f);
                if(i) g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f - I_TO_F(i) * 0.035f, this, vPos2, vDir)->ChangeSize(1.5f);
            }

            g_pSpecialEffects->PlaySound(coreVector3(vHit, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_07);

            m_avVector[RECOIL_TIME].arr(m_aiCounter[WEAPON_SHOT] % 2) = 1.0f;
        }
        else if(m_iWeaponType == 2u)
        {
            const coreVector2 vDir = IsHorizontal(vHit) ? vWeaponDir.InvertedX() : vWeaponDir.InvertedY();

            for(coreUintW i = g_pGame->IsEasy() ? 6u : 8u; i--; )
            {
                const coreVector2 vPos = vHit - vDir * (I_TO_F(i) * 3.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 1.4f, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }

            g_pSpecialEffects->PlaySound(coreVector3(vHit, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_05);

            m_avVector[RECOIL_TIME].x = 1.0f;

            if((m_iPhase >= 33u) && this->_ResurrectHelper(ELEMENT_RED, true))
            {
                g_pGame->GetHelper(ELEMENT_RED)->SetPosition(coreVector3(vHit, 0.0f));
                m_avVector[HELPER_DATA].xy(vDir);
            }
        }
        else if(m_iWeaponType == 3u)
        {
            for(coreUintW i = 75u; i--; )
            {
                if((i % 15u) < (g_pGame->IsEasy() ? 6u : 3u)) continue;

                const coreVector2 vDir   = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) - 3.0f) * 2.4f));
                const coreFloat   fSpeed = LERP(0.7f, 1.0f, I_TO_F(i % 15u) * 0.065f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, fSpeed, this, vHit,  vDir)->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, fSpeed, this, vHit, -vDir)->ChangeSize(1.5f);
            }

            g_pSpecialEffects->PlaySound(coreVector3(vHit, 0.0f), 1.2f, 0.9f, SOUND_ENEMY_EXPLOSION_09);

            m_avVector[RECOIL_TIME].x = 1.0f;

            if((m_iPhase >= 41u) && this->_ResurrectHelper(ELEMENT_PURPLE, true))
            {
                g_pGame->GetHelper(ELEMENT_PURPLE)->SetPosition(coreVector3(vHit, 0.0f));
            }
        }
        else UNREACHABLE

        const coreVector3 vStart = m_aWeapon[0].GetPosition() + m_aWeapon[0].GetDirection() * 9.0f;
        const coreVector3 vDiff  = coreVector3(vHit, 0.0f) - vStart;
        const coreUintW   iNum   = MAX(F_TO_UI(vDiff.Length() / 1.7f), 2u);

        for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vStart + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, COLOR_ENERGY_WHITE * 0.8f);

        g_pSpecialEffects->CreateSplashColor(coreVector3(vHit, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_WHITE * 0.8f);
        g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
        g_pSpecialEffects->RumblePlayer(NULL, SPECIAL_RUMBLE_SMALL, 250u);
    }
}


// ****************************************************************
// 
void cTigerBoss::__CauseBeamDamage(cPlayer* OUTPUT pTarget)
{
    const coreVector2 vDiff = this->GetPosition().xy() - m_vBeamPos;
    if(vDiff.LengthSq() < POW2(15.0f))
    {
        this->TakeDamage(TIGER_DAMAGE, ELEMENT_NEUTRAL, HIDDEN_POS, pTarget, true);

        g_pGame->GetCombatText()->DrawText(Core::Language->GetString("TEXT_HIT"), this->GetPosition(), COLOR_MENU_MAGENTA);
        g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_09);
    }

    g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        if(pPlayer->IsNormal())
        {
            const coreVector2 vDiff = pPlayer->GetPosition().xy() - m_vBeamPos;
            if(vDiff.LengthSq() < POW2(7.0f))
            {
                pPlayer->TakeDamage(5, ELEMENT_MAGENTA, m_vBeamPos);
            }
        }
    });
}


// ****************************************************************
// 
void cTigerBoss::__CreateTrail(const coreUintW iIndex, const coreVector3 vIntersect)
{
    ASSERT(iIndex < LEVIATHAN_RAYS)

    // always track intersection without rotation (to account for boss rotating with background)
    const coreVector2 vEnvDirection    = g_pEnvironment->GetDirection();
    const coreVector3 vRelIntersection = MapToAxisInv(vIntersect, vEnvDirection);

    // 
    constexpr coreFloat fMin = 1.7f;
    coreVector3 vOldHit = m_avVector[TRAIL_HIT + iIndex].xyz();

    // 
    if(vOldHit.IsNull()) vOldHit = vRelIntersection;
    else
    {
        while(true)
        {
            // 
            const coreVector3 vDiff = vRelIntersection - vOldHit;
            const coreFloat   fLen  = vDiff.Length();

            // 
            if(fLen < fMin) break;

            // 
            const coreVector3 vNewHit      = LERP(vOldHit, vRelIntersection, fMin*RCP(fLen));
            const coreVector2 vOldOnScreen = g_pForeground->Project2D(vOldHit);
            const coreVector2 vNewOnScreen = g_pForeground->Project2D(vNewHit);

            // 
            if(((ABS(vOldOnScreen.x) < 0.55f) && (ABS(vOldOnScreen.y) < 0.55f)) ||
               ((ABS(vNewOnScreen.x) < 0.55f) && (ABS(vNewOnScreen.y) < 0.55f)))
            {
                // 
                const coreVector3 vDecalPos  = (vOldHit + vNewHit) * 0.5f;
                const coreVector2 vDecalSize = coreVector2(3.6f, fMin*0.7f);
                const coreVector2 vDecalDir  = vDiff.xy().Normalized();

                // add rotation back to final position
                const coreVector3 vAbsPos = MapToAxis(vDecalPos, vEnvDirection);

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "default_white.png");
                pObject->DefineProgram("effect_decal_single_program");   // TODO 1: eigenen decal_color shader (similar to menu_color, ohne textur)

                // set object properties
                pObject->SetSize     (coreVector3(vDecalSize, 1.0f));
                pObject->SetDirection(coreVector3(vDecalDir,  0.0f));
                pObject->SetColor4   (coreVector4(0.0f,0.0f,0.0f,0.5f));

                // add object to background
                g_pEnvironment->GetBackground()->AddDecal(pObject, vAbsPos, 256u, "effect_decal_single_inst_program", LIST_KEY);
            }

            // 
            vOldHit = vNewHit;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * TIME * OUTDOOR_DETAIL;

    // 
    m_avVector[TRAIL_HIT + iIndex].xyz(vOldHit);
}


// ****************************************************************
// 
void cTigerBoss::__CreateOverdrive(const coreVector3 vIntersect)
{
    // 
    constexpr coreFloat fMin = 2.5f;
    constexpr coreFloat fMax = 5.0f;
    coreVector3 vOldHit = m_avVector[OVERDRIVE_HIT].xyz();

    // 
    if(vOldHit.IsNull()) vOldHit = vIntersect;
    else
    {
        while(true)
        {
            // 
            const coreVector3 vDiff = vIntersect - vOldHit;
            const coreFloat   fLen  = vDiff.Length();

            // 
            if(fLen < fMin) break;

            // 
            const coreVector3 vNewHit      = (fLen > fMax) ? LERP(vOldHit, vIntersect, fMax*RCP(fLen)) : vIntersect;
            const coreVector2 vOldOnScreen = g_pForeground->Project2D(vOldHit);
            const coreVector2 vNewOnScreen = g_pForeground->Project2D(vNewHit);

            // 
            if(((ABS(vOldOnScreen.x) < 0.55f) && (ABS(vOldOnScreen.y) < 0.55f)) ||
               ((ABS(vNewOnScreen.x) < 0.55f) && (ABS(vNewOnScreen.y) < 0.55f)))
            {
                // 
                if(HAS_BIT(m_iDecalState, 0u)) TOGGLE_BIT(m_iDecalState, 1u)
                TOGGLE_BIT(m_iDecalState, 0u)

                // 
                const coreBool    bRotated   = HAS_BIT(m_iDecalState, 0u);
                const coreBool    bFlipped   = HAS_BIT(m_iDecalState, 1u);
                const coreVector3 vDecalPos  = (vOldHit + vNewHit) * 0.5f;
                const coreVector2 vDecalSize = coreVector2(Core::Rand->Float(15.0f, 19.5f), MIN(fLen, fMax)*1.8f*3.0f);
                const coreVector2 vDecalDir  = vDiff.xy().Normalized();

                // load object resources
                coreObject3D* pObject = MANAGED_NEW(coreObject3D);
                pObject->DefineModel  (Core::Manager::Object->GetLowQuad());
                pObject->DefineTexture(0u, "effect_soot.png");
                pObject->DefineProgram("effect_decal_single_program");

                // set object properties
                pObject->SetSize     (coreVector3((bRotated ? vDecalSize.yx()       : vDecalSize),                            1.0f));
                pObject->SetDirection(coreVector3((bRotated ? vDecalDir.Rotated90() : vDecalDir) * (bFlipped ? -1.0f : 1.0f), 0.0f));
                pObject->SetColor3   (coreVector3(0.0f,0.0f,0.0f));

                // add object to background or windscreen
                g_pEnvironment->GetBackground()->AddDecal(pObject, vDecalPos, 128u, "effect_decal_single_inst_program", LIST_KEY);
                
                const coreVector3 vZero    = coreVector3(g_pForeground->Project3D(vNewHit), 0.0f);
                const coreVector3 vTowards = vZero.Normalized() * -1.0f;
                
                
                for(coreUintW i = 0u; i < 8u; ++i)
                {
                    const coreVector3 vDir = MapToAxisInv(vTowards, coreVector2::Direction((I_TO_F(i) - 3.5f) * (0.125f*PI))) * m_aBeam[0].GetSize().x * 1.0f;

                    g_pSpecialEffects->CreateSplashFire (vZero + vDir, 5.0f*2.0f, 1u, COLOR_ENERGY_MAGENTA);
                }
                
                
                // 
                for(coreUintW i = 0u; i < 8u; ++i)
                {
                    const coreVector3 vDir  = coreVector3(coreVector2::Direction(I_TO_F(i) * (0.125f*PI)), 0.0f);
                    const coreVector3 vDir2 = vDir * (m_aBeam[0].GetSize().x * 1.0f);

                    // 
                    g_pSpecialEffects->CreateSplashFire (vNewHit + vDir2,      5.0f*4.0f, 1u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->CreateSplashFire (vNewHit - vDir2,      5.0f*4.0f, 1u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->CreateSplashFire (vNewHit + vDir2*1.5f, 5.0f*4.0f, 1u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->CreateSplashFire (vNewHit - vDir2*1.5f, 5.0f*4.0f, 1u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->CreateSplashFire (vNewHit + vDir2*2.0f, 5.0f*4.0f, 1u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->CreateSplashFire (vNewHit - vDir2*2.0f, 5.0f*4.0f, 1u, COLOR_ENERGY_MAGENTA);

                    g_pSpecialEffects->CreateBlowColor(vNewHit + vDir2,  vDir, 25.0f*3.0f, 1u, COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->CreateBlowColor(vNewHit - vDir2, -vDir, 25.0f*3.0f, 1u, COLOR_ENERGY_MAGENTA);
                }
            }

            // 
            g_pSpecialEffects->ShakeScreen(0.3f);

            // 
            vOldHit = vNewHit;
        }
    }

    // 
    vOldHit.y -= g_pEnvironment->GetSpeed() * TIME * OUTDOOR_DETAIL;

    // 
    m_avVector[OVERDRIVE_HIT].xyz(vOldHit);
}