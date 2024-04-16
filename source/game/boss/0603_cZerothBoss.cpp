///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// snow-laser darf sich nicht zu schnell bewegen (test on 60 FPS), weil er keine dynamische funktion hat, und dann löcher hinterlässt
// angriff während laser-phase soll den spieler vom boss fernhalten, damit er nicht ständig kurz umrunden kann
// wie auch bei den anderen bossen, darf der spieler im boss-intro keinen schaden erhalten, weswegen der damaging-state disabled ist
// in first phase, having multiple "attack all" phases did not work, as player could just fly back and forth between two limbs
// in first phase, CW around, CCW around, 042513, the tail should pop out the head
// in infinity phase, pattern sollte nicht zu dicht sein, weil man sonst nicht durch navigieren kann und dann den boss zu sehr auf abstand hält
// in infinity phase, da geschosse sich jetzt auch bewegen sollte global move langsamer sein, damit die maximal-geschwindigkeit nicht zu hoch ist, außerdem wegen bewegung können bullets in der luft stehen bleiben was ok ist
// wenn alle limbs draußen zum (regulären) angreifen sind, sollte auch unverwundbarkeit vom panzer aufgehoben werden
// in infinity phase, wechsel von 2-schuss auf 4-schuss fällt nicht auf wenn sich drehung nicht zusätzlich ändert
// in ice-cube phase sollte der spieler daran gehindert werden mit dem cube zum boss zu gelangen
// push-effekt in drag-phase sollte nicht zu stark sein, weil der spieler sonst wenig spielraum für bewegung hat
// am ende von drag-phase soll spieler zum boss gezogen werden (bremsung vom background), wodurch der übergang in die schwing-phase kräftiger wirkt
// am beginn und ende von drag-phase müssen alle geschosse zerstört werden, bei beschleunigung und bremsung fliegt der spieler sonst zu leicht in die geschosse rein
// den boss die eiswürfel erzeugen zu lassen (e.g. mit einem strahl) war zu verwirrend, weil es gleichzeitig mit dem fernhalten des spielers ist (geschoss-welle)
// bei erstem eis-würfel darf der boss keinen angriff machen, damit man sich darauf konzentrieren kann zu verstehen was zu tun ist
// boss sollte in schwing-phase nicht zu schnell schwingen, damit man ihn gut angreifen kann, dabei soll eine fixe pause zwischen den angriffen sein (wenn boss schräg ist) in der man zeit hat sich zu drehen
// schnee sollte erst nach bounce-transition verschwinden, als zusätzliche steigerung dieser phase
// transition in drag-phase kann nicht an jeder schnittstelle passieren, weil das plötzliche bremsen dann komisch wirkt (zb. nach der bogen-bewegung)
// spieler kann bei schwing-phase einfach stehen bleiben und alle rand-geschosse werden sauber zerstört, aber der zusätzliche boss-angriff bringt ihn dazu sich zu bewegen (geschwindigkeit der rand-geschosse sollte nicht weiter erhöht werden, weil es sonst zu schwer wird in kombination mit boss-angriff)
// TODO 1: decent smoke on ice cubes ?
// TODO 1: stone outside of boss should also be ice cube ?
// TODO 1: MAIN: fragment, easy, hard idea, coop, regular score, extra score, badges, medal goal, juiciness (move, rota, muzzle, effects), intro, outro, foreshadow, overdrive, sound, attack size/count/speed, enemy/boss size, object size, background rota/speed
// TODO 1: ice-cube damage in coop will be halved, but should not
// TODO 1: in coop: vor-letzter eiswürfel fängt spieler 1, letzter fängt spieler 2, bei bremsung vor ende von herumzieh phase reißt spieler 2 ab und verliert morgenstern
// TODO 1: leichtes zucken vom boss(?) wenn er vom stern ge-grabt wird (wenn das ok is und nur ein zufall weil spieler-speed auf 0 gesetzt wird, dann mit effekt kaschieren)
// TODO 1: in reflection algorithm: remove loop and create constant formula
// TODO 1: soll morgen-stern "eingezogen" werden, bis er wieder benötigt wird ?
// TODO 1: soll auch vorher (im boss) ein ice-cube kommen ?
// TODO 1: in finaler phase, flip-bullets und partikel sollten unter gegner sein (auch im level), aber cone-bullets drüber, kann man das im bullet-manager aufteilen ? oder eigener bullet-manager für boss (nur für cone) [[das ganze is aber ziemlich orsch, normale gegner können (derzeit) nicht TOP gemacht werden]]
// TODO 1: spear-bullets sind bisschen langweilig, vielleicht pattern verbessern (schräg ? spread ?)
// TODO 1: view-bullet spread attack könnte ur-plötzlich kommen, vielleicht irgendwie ankündigen ?
// TODO 1: laser wird plötzlich erzeugt, was ist wenn spieler direkt in schuss-bahn/drehrichtung steht, sollte gegen durchschnittlicher spieler-position erzeugt werden (zusätzliche kurze phase mit drehung), oder eher richtung ändern (is deterministischer)
// TODO 1: ist letzte bounce-phase zu ähnlich wie intro ?
// TODO 1: soll jetzt name und health-bar kaputt sein ? (oder allgemein UI, wenn dann mit sinnvollem block-shift effect) (health % sind kaputt (wert remapping), und/oder bar zeigt nix an)
// TODO 1: "eis laser erzeugt stacheln am rand (zum unterschied mit moving-wall-stacheln, sind sie nur am getroffenen bereich)" will ich das ? soll es wo anders vorkommen ?
// TODO 1: ... am ende wenn er stirbt reißt die kette, er fliegt aus bild, und am anderen ende (mittig) wieder rein, und knallt dann gegen wand und zerschellt (kann irgendwie rausfliegen, aber fliegt konsistent wieder rein) (oder mehrfach bounce hin und her!)
// TODO 1: grüner angriff wird erst schneller/stärker mit dne ersten 1-3 treffern
// TODO 1: check if morning star is properly handled when player dies (in all sub-situations)
// TODO 1: sollte sich vielleicht in der ersten phase leicht drehen, is eigentlich voll zach für die erste phase


// ****************************************************************
// counter identifier
#define ICE_INDEX   (0u)
#define EVADE_COUNT (1u)
#define STAR_HOLD   (2u)
#define DRAG_COUNT  (3u)
#define DRAG_SIDE   (4u)
#define CRASH_COUNT (5u)
#define SLAP_COUNT  (6u)


// ****************************************************************
// vector identifier
#define STAR_LENGTH   (0u)
#define FINAL_DIR     (1u)
#define LASER_MOVE    (2u)
#define INFINITY_ROTA (3u)


// ****************************************************************
// constructor
cZerothBoss::cZerothBoss()noexcept
: m_afLimbValue {}
, m_vLaserDir   (coreVector2(0.0f,0.0f))
, m_fAnimation  (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 3.0f);

    // configure the boss
    this->Configure(19000, 0u, COLOR_SHIP_BLUE);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
    {
        coreHashString sModelHigh;
        coreHashString sModelLow;
        coreHashString sVolume;
             if(i == ZEROTH_LIMB_HEAD) {sModelHigh = "ship_boss_zeroth_head_high.md3"; sModelLow = "ship_boss_zeroth_head_low.md3"; sVolume = "ship_boss_zeroth_head_volume.md3";}
        else if(i == ZEROTH_LIMB_TAIL) {sModelHigh = "ship_boss_zeroth_tail.md3";      sModelLow = "ship_boss_zeroth_tail.md3";     sVolume = "ship_boss_zeroth_tail_volume.md3";}
        else                           {sModelHigh = "ship_boss_zeroth_leg.md3";       sModelLow = "ship_boss_zeroth_leg.md3";      sVolume = "ship_boss_zeroth_leg_volume.md3";}

        m_aLimb[i].DefineModelHigh(sModelHigh);
        m_aLimb[i].DefineModelLow (sModelLow);
        m_aLimb[i].DefineVolume   (sVolume);
        m_aLimb[i].SetSize        (this->GetSize());
        m_aLimb[i].Configure      (1, 0u, COLOR_SHIP_BLUE);
        m_aLimb[i].SetParent      (this);
    }

    // 
    m_Body.DefineModelHigh("ship_boss_zeroth_body_high.md3");
    m_Body.DefineModelLow ("ship_boss_zeroth_body_low.md3");
    m_Body.DefineVolume   ("ship_boss_zeroth_body_volume.md3");
    m_Body.SetSize        (this->GetSize());
    m_Body.Configure      (1, 0u, COLOR_SHIP_BLUE);
    m_Body.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_SECRET);
    m_Body.SetParent      (this);

    // 
    m_Laser.DefineModel  ("object_tube_open.md3");
    m_Laser.DefineTexture(0u, "effect_energy.png");
    m_Laser.DefineProgram("effect_energy_program");
    m_Laser.SetColor3    (COLOR_ENERGY_BLUE * 0.8f);
    m_Laser.SetTexSize   (ZEROTH_LASER_TEXSIZE);
    m_Laser.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_LaserWave.DefineModel  ("object_tube_open.md3");
    m_LaserWave.DefineTexture(0u, "effect_energy.png");
    m_LaserWave.DefineProgram("effect_energy_direct_program");
    m_LaserWave.SetColor3    (COLOR_ENERGY_BLUE * 1.5f);
    m_LaserWave.SetTexSize   (ZEROTH_LASERWAVE_TEXSIZE);
    m_LaserWave.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
    {
        m_aIce[i].DefineModelHigh("object_cube_ice.md3");
        m_aIce[i].DefineModelLow ("object_cube_ice.md3");
        m_aIce[i].DefineTexture  (1u, "environment_water_norm.png");
        m_aIce[i].DefineProgram  ("object_ice_program");
        m_aIce[i].SetSize        (coreVector3(1.0f,1.0f,1.0f) * 6.0f);
        m_aIce[i].SetTexSize     (coreVector2(0.25f,0.25f));
        m_aIce[i].Configure      (50, 0u, coreVector3(1.0f,1.0f,1.0f));
        m_aIce[i].AddStatus      (ENEMY_STATUS_BOTTOM | ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_GHOST | ENEMY_STATUS_WORTHLESS | ENEMY_STATUS_SECRET);
    }

    STATIC_ASSERT(offsetof(cZerothBoss, m_aLimb) < offsetof(cZerothBoss, m_Body))   // initialization order for collision detection
}


// ****************************************************************
// 
void cZerothBoss::ResurrectIntro()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

    // 
    pMission->EnableStar(0u, this, coreVector2(0.0f,-8.0f));

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) this->__SetLimbValue(i, 1.0f);

    // 
    for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) m_aLimb[i].ChangeToBottom();
    m_Body.ChangeToBottom();

    // 
    m_iPhase = 200u;
    this->Resurrect();
}


// ****************************************************************
// 
void cZerothBoss::__ResurrectOwn()
{
    if(m_iPhase < 200u)
    {
        cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

        if(!pMission->IsStarEnabled(0u))
        {
            // 
            g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
            {
                pMission->EnableStar(i, pPlayer, coreVector2(0.0f,0.0f));
            });

            // 
            pMission->StartSwing(1.0f);
        }

        // 
        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) this->__SetLimbValue(i, 1.0f);

        // 
        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i) m_aLimb[i].ChangeToNormal();
        m_Body.ChangeToNormal();

        // 
        constexpr coreUint8 aiNewOrder[] = {cConeBullet::ID};
        g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));

        // 
        this->_ResurrectBoss();
    }
}


// ****************************************************************
// 
void cZerothBoss::__KillOwn(const coreBool bAnimated)
{
    if(m_iPhase < 200u)
    {
        cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

        // 
        for(coreUintW i = 0u; i < CALOR_STARS; ++i)
            pMission->DisableStar(i, bAnimated);

        // 
        pMission->GetSnow()->Disable(bAnimated ? 1.0f : 0.0f);

        // 
        this->__DisableLaser(bAnimated);

        // 
        for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
            this->__DestroyCube(i, bAnimated);

        // 
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->SetMoveSpeed(1.0f);
        });

        // 
        g_pGame->GetBulletManagerEnemy()->ResetOrder();
    }

    // 
    m_iPhase = 0u;
}


// ****************************************************************
// 
void cZerothBoss::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_Laser.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Laser);

    DEPTH_PUSH

    glDepthMask(false);
    {
        // 
        m_LaserWave.Render();
    }
    glDepthMask(true);
}


// ****************************************************************
// 
void cZerothBoss::__RenderOwnOver()
{
    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
    {
        m_aIce[i].DefineTexture(0u, m_aIce[i].HasStatus(ENEMY_STATUS_DEAD) ? NULL : g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture);
    }
    // TODO 1: (look at cMenu::__Reset) needs to be properly unset before engine reset, otherwise shadow texture breaks
    // GetBackground()->GetResolvedTexture()
}


// ****************************************************************
// 
void cZerothBoss::__MoveOwn()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());
    cSnow*         pSnow    = pMission->GetSnow();
    coreObject3D*  pStar    = pMission->GetStar(0u);

    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 10.0f);

    if(this->ReachedDeath())
    {
        g_pSpecialEffects->MacroDestructionDark(&m_Body);
        this->Kill(false);
        this->_EndBoss();
    }

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.2f, LERP_BREAK)
        {
            if(PHASE_TIME_POINT(0.75f))
            {
                this->_StartBoss();
            }

            coreInt16 iCrash = 0;

            coreVector2 vPos = LERP(coreVector2(-0.1f,1.5f), coreVector2(-4.0f,-6.7f), fTime);

                                              while(ABS(vPos.x) > 1.0f) {iCrash += 1; vPos.x = SIGN(vPos.x) * 2.0f - vPos.x;}
            if(m_aiCounter[CRASH_COUNT] >= 1) while(ABS(vPos.y) > 1.0f) {iCrash += 1; vPos.y = SIGN(vPos.y) * 2.0f - vPos.y;}

            vPos *= FOREGROUND_AREA;

            if(m_aiCounter[CRASH_COUNT] < iCrash)
            {
                m_aiCounter[CRASH_COUNT] = iCrash;

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }

            this->SetPosition      (coreVector3(vPos, 0.0f));
            this->DefaultRotateLerp(1.0f*PI, 5.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(60u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        if(PHASE_MAINTIME_BEFORE(0.5f) || PHASE_MAINTIME_POINT(0.5f))
        {
            this->__SetLimbValue(ZEROTH_LIMB_TAIL, 1.0f - BLENDB(MIN1(m_fPhaseTime * 2.0f)));

            if(PHASE_MAINTIME_BEFORE(0.5f)) m_aLimb[ZEROTH_LIMB_TAIL].AddStatus(ENEMY_STATUS_GHOST);
        }

        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
        {
            if(m_aLimb[i].WasDamaged())
            {
                m_aiCounter[EVADE_COUNT] += 1;

                if(m_aiCounter[EVADE_COUNT] < 3)
                {
                    PHASE_RESET(0u)

                    this->StoreRotation();

                    m_aLimb[ZEROTH_LIMB_TAIL].AddStatus(ENEMY_STATUS_GHOST);
                    ASSERT(i == ZEROTH_LIMB_TAIL)
                }
                else if(m_aiCounter[EVADE_COUNT] < 6)
                {
                    this->__SetLimbValue(i, 1.0f);

                    coreUintW iNextLimb;
                    switch(m_aiCounter[EVADE_COUNT])
                    {
                    default: ASSERT(false)
                    case 3: iNextLimb = 0u; break;
                    case 4: iNextLimb = 4u; break;
                    case 5: iNextLimb = 2u; break;
                    }

                    this->__SetLimbValue(iNextLimb, 0.0f);
                }
                else if(m_aiCounter[EVADE_COUNT] < 7)
                {
                    for(coreUintW j = 0u; j < ZEROTH_LIMBS; ++j)
                    {
                        this->__SetLimbValue(j, (i == j) ? 1.0f : 0.0f);
                    }
                }
                else
                {
                    this->__SetLimbValue(i, 1.0f);

                    if(m_aiCounter[EVADE_COUNT] >= 11)
                    {
                        PHASE_CHANGE_INC
                    }
                }

                break;
            }
        }

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            if((m_aiCounter[EVADE_COUNT] >= 1) && (m_aiCounter[EVADE_COUNT] < 3))
            {
                coreFloat fNewAngle;
                switch(m_aiCounter[EVADE_COUNT])
                {
                default: ASSERT(false)
                case 1: fNewAngle =  (1.0f/3.0f)*PI; break;
                case 2: fNewAngle = (-1.0f/3.0f)*PI; break;
                }

                this->DefaultRotateLerp(AnglePos(m_vLastDirection.xy().Angle()), fNewAngle, fTime);

                if(PHASE_FINISHED)
                {
                    m_aLimb[ZEROTH_LIMB_TAIL].RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 2.0f, LERP_LINEAR)
        {
            for(coreUintW j = 6u; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) * ((2.0f/6.0f)*PI) + I_TO_F(iTick) * ((0.5f/6.0f)*PI));
                const coreVector2 vPos = this->GetPosition().xy() + vDir * ZEROTH_RADIUS;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5u, 1.0f, this, vPos, vDir)->ChangeSize(1.7f);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(61u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        if(PHASE_BEGINNING2)
        {
            pSnow->Enable();

            this->__EnableLaser(ZEROTH_LIMB_HEAD);
        }

        m_avVector[LASER_MOVE].x += 1.0f * TIME;

        const coreFloat fSpeed = 0.075f * LERP(1.0f, 0.5f, STEP(0.95f, 1.0f, this->GetCurHealthPct()));

        PHASE_CONTROL_TIMER(0u, fSpeed, LERP_LINEAR)
        {
            const coreVector2 vPos = coreVector2::Direction(m_avVector[LASER_MOVE].x * (-2.0f*PI) * 0.0375f) * LERPS(0.0f, 0.3f, CLAMP01((m_avVector[LASER_MOVE].x - 7.0f) * 0.1f));

            this->SetPosition      (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            this->DefaultRotateLerp(0.0f*PI, 4.0f*PI, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)
            }
        });

        PHASE_CONTROL_TIMER(1u, 0.2f, LERP_LINEAR)
        {
            const coreFloat fValue = BLENDH3(CLAMP01(0.5f + 0.7f * COS(fTime * (2.0f*PI))));

            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                this->__SetLimbValue(i, (i == ZEROTH_LIMB_HEAD) ? 1.0f : fValue);
            }

            if(m_aLimb[ZEROTH_LIMB_TAIL].HasStatus(ENEMY_STATUS_GHOST))
            {
                m_Body.AddStatus(ENEMY_STATUS_INVINCIBLE);
            }
            else
            {
                m_Body.RemoveStatus(ENEMY_STATUS_INVINCIBLE);
            }

            if(PHASE_TIME_POINT(0.45f))
            {
                for(coreUintW j = 12u; j--; )
                {
                    const coreVector2 vDir = MapToAxis(coreVector2::Direction(I_TO_F(j) * ((1.0f/6.0f)*PI)), this->GetDirection().xy());
                    const coreVector2 vPos = this->GetPosition().xy() + vDir * ZEROTH_RADIUS;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5u, 0.8f, this, vPos, vDir)->ChangeSize(1.7f);
                }
            }

            if(PHASE_FINISHED)
            {
                PHASE_RESET(1u)
            }
        });

        if((this->GetCurHealthPct() < 0.95f) && m_aLimb[ZEROTH_LIMB_TAIL].HasStatus(ENEMY_STATUS_HIDDEN))
        {
            PHASE_CHANGE_INC

            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                this->__SetLimbValue(i, 1.0f);
            }

            m_Body.AddStatus(ENEMY_STATUS_INVINCIBLE);

            this->__DisableLaser(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(62u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        const coreBool bExit = !m_avVector[FINAL_DIR].xy().IsNull();

        const coreFloat   fGlobalSpeed = 20.0f;
        const coreVector2 vGlobalDir   = bExit ? m_avVector[FINAL_DIR].xy() : g_pGame->GetPlayer(0u)->GetDirection().xy();
        const coreVector2 vGlobalMove  = vGlobalDir * (-fGlobalSpeed * TIME);

        if(PHASE_BEGINNING2)
        {
            this->SetPosition(coreVector3(vGlobalDir * FOREGROUND_AREA * 1.44f, 0.0f));

            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                this->__SetLimbValue(i, 0.0f);
                m_aLimb[i].ResetDamageForwarded();
            }

            pSnow->Disable(1.0f);

            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetMoveSpeed(1.0f);
            });
        }

        coreUintW iFinished = 0u;

        for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
        {
            if(m_aLimb[i].GetDamageForwarded() >= 30)
            {
                this->__SetLimbValue(i, 1.0f);
                iFinished += 1u;
            }
        }

        if(!bExit && (iFinished == ZEROTH_LIMBS))
        {
            m_avVector[FINAL_DIR].xy(vGlobalDir);
        }

        const coreBool bAdvance = (iFinished < 3u);

        m_avVector[INFINITY_ROTA].x += 1.0f * TIME * (bAdvance ? 1.0f : -1.0f);
        this->DefaultRotate(m_avVector[INFINITY_ROTA].x);

        PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
        {
            if((iTick % 8u) < 5u) return;

            for(coreUintW i = 0u; i < 4u; ++i)
            {
                if(bAdvance && (i % 2u)) continue;

                const coreVector2 vDir = MapToAxis(StepRotated90(i), this->GetDirection().xy());
                const coreVector2 vPos = this->GetPosition().xy() + vDir * ZEROTH_RADIUS;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.6f, this, vPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
        });

        const auto nMoveFunc = [&](coreObject3D* OUTPUT pObject)
        {
            coreVector2 vNewPos = pObject->GetPosition().xy() + vGlobalMove;

            if(bExit)
            {
                pObject->SetPosition(coreVector3(vNewPos, 0.0f));
                return !g_pForeground->IsVisiblePoint(vNewPos, 1.45f);
            }
            else
            {
                     if((vNewPos.x < -FOREGROUND_AREA.x * 1.45f) && (vGlobalMove.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.9f;
                else if((vNewPos.x >  FOREGROUND_AREA.x * 1.45f) && (vGlobalMove.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.9f;
                     if((vNewPos.y < -FOREGROUND_AREA.y * 1.45f) && (vGlobalMove.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.9f;
                else if((vNewPos.y >  FOREGROUND_AREA.y * 1.45f) && (vGlobalMove.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.9f;

                pObject->SetPosition(coreVector3(vNewPos, 0.0f));
                return false;
            }
        };

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
        {
            if(pBullet->GetFlyTime() >= 4.0f)
            {
                pBullet->Deactivate(true);
            }
            else if(nMoveFunc(pBullet))
            {
                pBullet->Deactivate(false);
            }
        });

        if(nMoveFunc(this))
        {
            PHASE_CHANGE_TO(40u)

            g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
                {
                    this->__SetLimbValue(i, 1.0f);
                }
            }

            this->DefaultMoveLerp  (coreVector2(0.0f,1.5f), coreVector2(0.0f,0.7f), fTime);
            this->DefaultRotateLerp(0.0f*PI,                3.0f*PI,                fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        m_avVector[INFINITY_ROTA].y -= 1.0f * TIME * MIN1(m_fPhaseTime) * (1.0f + 0.3f * I_TO_F(m_aiCounter[SLAP_COUNT]));
        this->DefaultRotate(m_avVector[INFINITY_ROTA].y);

        PHASE_CONTROL_TICKER(0u, 0u, 0.15f, LERP_LINEAR)
        {
            this->__CreateCube(coreVector2((iTick % 2u) ? -0.9f : 0.9f, -1.3f) * FOREGROUND_AREA, coreVector2(0.0f,1.0f));
        });

        if(PHASE_BEGINNING2)   // after control-macro
        {
            m_aTimer[0].SetValue(m_aTimer[0].GetValue(CORE_TIMER_GET_NORMAL) + 0.9f);
        }

        for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
        {
            if(this->WasDamaged())
            {
                m_aiCounter[SLAP_COUNT] += 1;

                if(m_aiCounter[SLAP_COUNT] >= 4)
                {
                    PHASE_CHANGE_TO(70u)

                    m_aiCounter[SLAP_COUNT] = 0;   // also prevent shooting

                    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

                    const coreUintW iIndex = m_aIce[i].LastAttacker() - g_pGame->GetPlayer(0u);

                    pMission->StopSwing();
                    pMission->SetStarLength(iIndex, CALOR_CHAIN_CONSTRAINT2);

                    ADD_BIT(m_aiCounter[STAR_HOLD], iIndex)

                    for(coreUintW j = 0u; j < ZEROTH_LIMBS; ++j)
                    {
                        this->__SetLimbValue(j, 0.0f);
                    }

                    m_Body.RemoveStatus(ENEMY_STATUS_INVINCIBLE);

                    for(coreUintW j = 0u; j < ZEROTH_ICES; ++j)
                    {
                        this->__DestroyCube(j, true);
                    }

                    const coreInt32 iNewMaxHealth = 3500;
                    const coreInt32 iNewCurHealth = F_TO_SI(I_TO_F(iNewMaxHealth) * this->GetCurHealthPct());

                    this->SetMaxHealth(iNewMaxHealth);
                    this->SetCurHealth(iNewCurHealth);
                }

                break;
            }
        }

        if(m_aiCounter[SLAP_COUNT])
        {
            const coreFloat fSpeed = I_TO_F(m_aiCounter[SLAP_COUNT]) * 0.8f;

            PHASE_CONTROL_TICKER(1u, 0u, fSpeed, LERP_LINEAR)
            {
                for(coreUintW j = 80u; j--; )
                {
                    if(((j + ((iTick * 2u) % 5u) * 2u) % 10u) < 5u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f));
                    const coreVector2 vPos = this->GetPosition().xy() + vDir * ZEROTH_RADIUS;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5u, 1.0f, this, vPos, vDir)->ChangeSize(1.3f);
                }
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreVector2 vDiff = this->GetPosition().xy() - g_pGame->GetPlayer(0u)->GetPosition().xy();

            const coreFloat fFrom = AnglePos(m_vLastDirection.xy().Angle());
            const coreFloat fTo   = vDiff.Angle();

            this->DefaultRotateLerp(fFrom, fFrom + AngleDiff(fTo, fFrom) + (4.0f*PI), BLENDB(MIN1(fTime)));

            const coreFloat fSpeed = LERPH3(ZEROTH_SPEED_FAST, ZEROTH_SPEED_SLOW, fTime) * (m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f);

            g_pEnvironment->SetTargetSpeedNow(fSpeed);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_avVector[STAR_LENGTH].x = vDiff.Length();

                pMission->CatchObject(0u, this);
                pMission->StartSwing(0.3f);
                pMission->SetStarLength(0u, m_avVector[STAR_LENGTH].x);

                this->SetCollisionModifier(m_Body.GetCollisionRange() * 0.7f);

                //for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
                //{
                //    m_aLimb[i].ChangeToTop();
                //}
                //m_Body.ChangeToTop();
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fLength = LERPS(m_avVector[STAR_LENGTH].x, 30.0f, fTime);

            pMission->SetStarLength(0u, fLength);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 52u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 60.0f, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);
            const coreVector2 vPos = vDir * FOREGROUND_AREA * -1.2f * SQRT2;

            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.25f, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
        });

        const coreFloat fSpeed = LERP(1.6f, CORE_MATH_PRECISION, STEP(0.0f, 0.25f, this->GetCurHealthPct()));

        PHASE_CONTROL_TICKER(1u, 0u, fSpeed, LERP_LINEAR)
        {
            const coreVector2 vTarger = this->NearestPlayerDual(iTick % 2u)->GetPosition().xy();

            for(coreUintW j = 6u; j--; )
            {
                const coreVector2 vBase = MapToAxis(coreVector2::Direction(I_TO_F(j) * ((2.0f/6.0f)*PI)), this->GetDirection().xy());

                const coreVector2 vPos = this->GetPosition().xy() + vBase * ZEROTH_RADIUS;
                const coreVector2 vDir = (vTarger - vPos).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5u, 0.8f, this, vPos, vDir)->ChangeSize(1.7f);
            }
        });

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([](cFlipBullet* OUTPUT pBullet)
        {
            if(pBullet->GetFlyTime() >= 5.0f) pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 60u) || (m_iPhase == 61u) || (m_iPhase == 62u))
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[CRASH_COUNT] = 0;
        }

        coreFloat   fSpeed;
        coreVector2 vTarget;
        coreUint8   iPhase;
        switch(m_iPhase)
        {
        default: ASSERT(false)
        case 60u: fSpeed = 0.25f; vTarget = coreVector2( 0.0f,-4.0f); iPhase = 10u; break;
        case 61u: fSpeed = 0.2f;  vTarget = coreVector2( 8.0f, 4.0f); iPhase = 20u; break;
        case 62u: fSpeed = 0.2f;  vTarget = coreVector2(-6.0f,-4.0f); iPhase = 30u; break;
        }

        PHASE_CONTROL_TIMER(0u, fSpeed, LERP_SMOOTH)
        {
            coreInt16 iCrash = 0;

            coreVector2 vPos = LERP(m_vLastPosition, vTarget, fTime);

            while(ABS(vPos.x) > 1.0f) {iCrash += 1; if((m_iPhase == 62u) && (iCrash >= 3)) {iCrash = 2; break;} vPos.x = SIGN(vPos.x) * 2.0f - vPos.x;}
            while(ABS(vPos.y) > 1.0f) {iCrash += 1;                                                             vPos.y = SIGN(vPos.y) * 2.0f - vPos.y;}

            vPos *= FOREGROUND_AREA;

            if(m_aiCounter[CRASH_COUNT] < iCrash)
            {
                m_aiCounter[CRASH_COUNT] = iCrash;

                for(coreUintW j = 20u; j--; )
                {
                    if((j % 5u) < 3u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 4.5f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos,  vDir)            ->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos,  vDir.Rotated90())->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos, -vDir)            ->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos, -vDir.Rotated90())->ChangeSize(1.3f);
                }

                g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }

            this->SetPosition      (coreVector3(vPos, 0.0f));
            this->DefaultRotateLerp(AnglePos(m_vLastDirection.xy().Angle()), 5.0f*PI, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(iPhase)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fFrom = AnglePos(m_vLastDirection.xy().Angle());
            const coreFloat fTo   = 0.0f*PI;

            this->DefaultMoveLerp  (m_vLastPosition, m_vLastPosition + coreVector2(0.0f,-0.3f), SIN(BLENDB(fTime) * (1.0f*PI)));
            this->DefaultRotateLerp(fFrom,           fFrom + AngleDiff(fTo, fFrom) + (4.0f*PI), BLENDB(fTime));

            const coreFloat fSpeed = LERPH3(ZEROTH_SPEED_SLOW, ZEROTH_SPEED_FAST, MIN1(fTime * 2.0f));
            g_pEnvironment->SetTargetSpeedNow(fSpeed);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 71u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            const coreFloat fSide = m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f;

            const coreVector2 vDir = coreVector2::Direction((I_TO_F(m_aiCounter[DRAG_COUNT]) + fTime) * (0.5f*PI)) * fSide;

            this->SetPosition (coreVector3(vDir * FOREGROUND_AREA * 0.7f, 0.0f));
            this->SetDirection(coreVector3(vDir, 0.0f));

            g_pEnvironment->SetTargetDirectionNow(vDir * SIGN(g_pEnvironment->GetSpeed()));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 72u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            const coreFloat fSide = m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f;

            const coreVector2 vEnvDir = g_pEnvironment->GetDirection() * SIGN(g_pEnvironment->GetSpeed());

            const coreFloat   fTime1 = BLENDH3(fTime);
            const coreFloat   fSin1  = SIN(fTime1 * (2.0f*PI)) * fSide;
            const coreVector2 vPos1  = (vEnvDir * 0.7f + vEnvDir.Rotated90() * fSin1 * 0.7f) * FOREGROUND_AREA;

            const coreFloat   fTime2 = BLENDH3(CLAMP01((fTime - 0.05f) / 0.95f));
            const coreFloat   fSin2  = SIN(fTime2 * (2.0f*PI)) * fSide;
            const coreVector2 vPos2  = (vEnvDir * 0.5f + vEnvDir.Rotated90() * fSin2 * 0.7f) * FOREGROUND_AREA;

            const coreVector2 vDir = (vPos1 - vPos2).Normalized();

            this->SetPosition (coreVector3(vPos1, 0.0f));
            this->SetDirection(coreVector3(vDir,  0.0f));

            if(PHASE_FINISHED)
            {
                if(this->GetCurHealthPct() < 0.5f)
                {
                    PHASE_CHANGE_TO(50u)

                    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);
                }
                else
                {
                    PHASE_CHANGE_TO(73u)

                    m_aiCounter[DRAG_COUNT] += 1;
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 73u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            const coreFloat fSide = (m_aiCounter[DRAG_SIDE] ? -1.0f : 1.0f);
            const coreFloat fLerp = LERP(1.0f, -1.0f, fTime);

            const coreVector2 vDir  = coreVector2::Direction(I_TO_F(m_aiCounter[DRAG_COUNT]) * (0.5f*PI)) * fSide;
            const coreVector2 vRota = coreVector2::Direction(LERP(0.0f*PI, 3.0f*PI, fTime));

            this->SetPosition (coreVector3(vDir * FOREGROUND_AREA * (0.7f * fLerp), 0.0f));
            this->SetDirection(coreVector3(MapToAxis(vDir, vRota), 0.0f));

            g_pEnvironment->SetTargetSpeedNow(ZEROTH_SPEED_FAST * fSide * fLerp);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(71u)
                PHASE_RESET(3u)

                m_aiCounter[DRAG_SIDE] = 1 - m_aiCounter[DRAG_SIDE];
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 200u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,1.5f), coreVector2(0.0f,0.7f), fTime);

            const coreVector2 vBase = this->GetPosition().xy() + this->GetDirection().xy() * -8.0f;
            const coreVector2 vPos  = vBase + coreVector2(0.0f,4.0f);

            pStar->SetPosition(coreVector3(vPos, 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 201u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            this->DefaultRotate(LERPBR(1.0f*PI, 5.0f*PI, fTime));

            const coreVector2 vBase = this->GetPosition().xy() + this->GetDirection().xy() * -8.0f;
            const coreVector2 vPos  = vBase - this->GetDirection().xy() * LERPBR(4.0f, 60.0f, STEP(0.2f, 1.0f, fTime));

            pStar->SetPosition(coreVector3(vPos, 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 202u)
    {
        static coreFloat fCurLength = 60.0f;

        cPlayer* pPlayer = this->NearestPlayerDual(0u);

        const coreVector2 vBase = this->GetPosition().xy() + this->GetDirection().xy() * -8.0f;
        const coreVector2 vDiff = pPlayer->GetPosition().xy() - vBase;

        coreBool bTurn = false;
        const coreFloat fSpeed = 4.0f;
        if(F_TO_SI(m_fPhaseTimeBefore * fSpeed / (2.0f*PI)) != F_TO_SI(m_fPhaseTime * fSpeed / (2.0f*PI)))
        {
            fCurLength = MAX(vDiff.Length(), 15.0f);
            bTurn = true;
        }

        const coreVector2 vPos = coreVector2::Direction(m_fPhaseTime * fSpeed) * fCurLength + vBase;
        pStar->SetPosition(coreVector3(vPos, 0.0f));

        if(pMission->GetCatchObject(0u) != pPlayer)
        {
            const coreVector2 vCatchDiff = pPlayer->GetPosition().xy() - pStar->GetPosition().xy();
            if(vCatchDiff.LengthSq() < POW2(4.0f))
            {
                pMission->CatchObject(0u, pPlayer);
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);
            }
        }
        else if(bTurn)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 203u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            this->DefaultRotate(LERPB(1.0f*PI, 2.0f*PI, fTime));

            const coreVector2 vPos = coreVector2(SIN(fTime * (1.0f*PI)) * -30.0f, LERP(FOREGROUND_AREA.y * 1.4f, FOREGROUND_AREA.y * -1.1f, POW2(POW3(fTime))));

            pStar->SetPosition(coreVector3(vPos, 0.0f));
            
            //this->SetDirection(coreVector3(-(vPos - this->GetPosition().xy()).Normalized(), 0.0f));

            if(PHASE_FINISHED)
            {
                cPlayer* pPlayer = d_cast<cPlayer*>(pMission->GetCatchObject(0u));

                pMission->DisableStar(0u, false);
                pMission->EnableStar (0u, pPlayer, coreVector2(0.0f,0.0f));

                pStar->SetPosition(coreVector3(0.0f,-1.1f,0.0f) * FOREGROUND_AREA3);

                pMission->UncatchObject(0u);

                pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
                pPlayer->ApplyForce  (coreVector2(0.0f,50.0f));
                pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);

                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 204u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(0.0f,1.5f), fTime);
            this->DefaultRotateLerp(2.0f*PI,         3.0f*PI,                fTime);

            if(PHASE_FINISHED)
            {
                this->Kill(false);
            }
        });
    }

    // ################################################################
    // ################################################################

    if(((m_iPhase >= 70u) && (m_iPhase < 80u)) || (m_iPhase == 50u))
    {
        const coreFloat   fPower = STEP(ZEROTH_SPEED_SLOW, ZEROTH_SPEED_FAST, ABS(g_pEnvironment->GetSpeed())) * SIGN(g_pEnvironment->GetSpeed()) * ((m_iPhase == 50u) ? -0.7f : 0.9f);
        const coreVector2 vPush  = g_pEnvironment->GetDirection() * (fPower * 60.0f * TIME);

        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->SetPosition(pPlayer->GetPosition() - coreVector3(vPush, 0.0f));
        });

        if((m_iPhase == 71u) || (m_iPhase == 72u))
        {
            PHASE_CONTROL_TICKER(3u, 0u, 10.0f, LERP_LINEAR)
            {
                if((iTick % 8u) < 4u) return;

                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = g_pEnvironment->GetDirection() * -SIGN(g_pEnvironment->GetSpeed());

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5u, 1.0f, this, vPos, vDir)->ChangeSize(1.8f);
            });
        }
    }

    if(m_Laser.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        const coreVector3 vBasePos = this->GetPosition();
        const coreVector3 vBaseDir = coreVector3(MapToAxis(m_vLaserDir, this->GetDirection().xy()), 0.0f);
        const coreFloat   fAlpha   = MIN1(m_Laser.GetAlpha() + 2.0f * TIME);
        const coreVector3 vScale   = coreVector3(fAlpha, 1.0f, fAlpha);

        // 
        m_Laser.SetPosition (vBasePos + vBaseDir * ZEROTH_LASER_SIZE.y);
        m_Laser.SetSize     (vScale * ZEROTH_LASER_SIZE);
        m_Laser.SetDirection(vBaseDir);
        m_Laser.SetAlpha    (fAlpha);
        m_Laser.SetTexOffset(coreVector2(-0.2f,-0.6f) * m_fAnimation);
        m_Laser.Move();

        // 
        m_LaserWave.SetPosition (vBasePos + vBaseDir * ZEROTH_LASERWAVE_SIZE.y);
        m_LaserWave.SetSize     (vScale * ZEROTH_LASERWAVE_SIZE);
        m_LaserWave.SetDirection(vBaseDir * -1.0f);
        m_LaserWave.SetAlpha    (fAlpha);
        m_LaserWave.SetTexOffset(coreVector2(0.3f,1.3f) * m_fAnimation);
        m_LaserWave.Move();

        const coreVector2 vRayPos = vBasePos.xy();
        const coreVector2 vRayDir = vBaseDir.xy();

        cPlayer::TestCollision(PLAYER_TEST_NORMAL, vRayPos, vRayDir, &m_Laser, [&](cPlayer* OUTPUT pPlayer, const coreFloat* pfHitDistance, const coreUint8 iHitCount, const coreBool bFirstHit)
        {
            if(!bFirstHit) return;
            if(pfHitDistance[0] <= 0.0f) return;

            const coreVector2 vIntersection = vRayPos + vRayDir * pfHitDistance[0];

            // 
            pPlayer->TakeDamage(5, ELEMENT_BLUE, vIntersection);

            // 
            g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vIntersection, 0.0f), COLOR_ENERGY_BLUE);
        });

        if(pSnow->IsActive())
        {
            const coreVector2 vSide = vRayDir.Rotated90();

            for(coreUintW i = 0u; i < 4u; ++i)
            {
                const coreVector2 vFinalPos = vRayPos + vSide * ((I_TO_F(i) - 1.5f) * 0.35f);

                pSnow->DrawRay(vFinalPos, vRayDir, SNOW_TYPE_ADD);
            }
        }

        PHASE_CONTROL_TICKER(4u, 0u, 60.0f, LERP_LINEAR)
        {
            const coreFloat   fLen    = g_pForeground->RayIntersection(vRayPos, vRayDir);
            const coreVector3 vTarget = coreVector3(vRayPos + vRayDir * fLen, 0.0f);

            g_pSpecialEffects->CreateSplashColor(vTarget, 70.0f, 2u, COLOR_ENERGY_BLUE);
            g_pSpecialEffects->CreateSplashFire (vTarget, 20.0f, 3u, COLOR_ENERGY_BLUE * 0.15f);
        });
    }

    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
    {
        cCustomEnemy& oIce = m_aIce[i];
        if(oIce.HasStatus(ENEMY_STATUS_DEAD)) continue;

        const coreFloat   fLerp = LERPB(1.0f, 0.0f, MIN1(oIce.GetLifeTime() * 0.5f));
        const coreVector2 vPos  = oIce.GetPosition ().xy() + coreVector2(0.0f,1.0f) * (40.0f * TIME * fLerp);
        const coreVector2 vDir  = oIce.GetDirection().xy() * coreMatrix3::Rotation(10.0f * TIME * fLerp).m12();

        // 
        oIce.SetPosition (coreVector3(vPos, 0.0f));
        oIce.SetDirection(coreVector3(vDir, 0.0f));
    }


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

    if(pSnow->IsActive())
    {
        g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
        {
            if(pSnow->DrawPoint(pBullet->GetPosition().xy() + 0.5f * pBullet->GetFlyMove(), 4.0f, SNOW_TYPE_REMOVE) +
               pSnow->DrawPoint(pBullet->GetPosition().xy(),                                4.0f, SNOW_TYPE_REMOVE) +
               pSnow->DrawPoint(pBullet->GetPosition().xy() - 0.5f * pBullet->GetFlyMove(), 3.0f, SNOW_TYPE_REMOVE))
            {
                pBullet->Deactivate(true);
            }
        });

        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->SetMoveSpeed((!pPlayer->IsRolling() && pSnow->IsActive() && pSnow->TestCollision(pPlayer->GetPosition().xy())) ? 0.2f : 1.0f);
        });
    }

    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        if(HAS_BIT(m_aiCounter[STAR_HOLD], i))
        {
            pMission->GetStar(i)->SetPosition(this->GetPosition());
        }
    }

    const coreFloat   fEnvSpeed = g_pEnvironment->GetSpeed();
    const coreVector2 vEnvDir   = g_pEnvironment->GetDirection() * SIGN(fEnvSpeed);

    g_pSpecialEffects->CreateGust(STEP(ZEROTH_SPEED_SLOW, ZEROTH_SPEED_FAST, ABS(fEnvSpeed)), vEnvDir.Angle());
}


// ****************************************************************
// 
void cZerothBoss::__EnableLaser(const coreUintW iLimb)
{
    WARN_IF(m_Laser.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    ASSERT(iLimb < ZEROTH_LIMBS)
    m_vLaserDir = coreVector2::Direction(I_TO_F(iLimb) * (0.4f*PI));

    // 
    const auto nInitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetAlpha  (0.0f);
        pObject->SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(pObject);
    };
    nInitFunc(&m_Laser);
    nInitFunc(&m_LaserWave);

    // 
    const coreVector2 vDir = MapToAxis(m_vLaserDir, this->GetDirection().xy());
    const coreFloat   fLen = g_pForeground->RayIntersection(this->GetPosition().xy(), vDir);

    // 
    g_pSpecialEffects->CreateSplashColor(coreVector3(this->GetPosition().xy() + vDir * fLen, 0.0f), SPECIAL_SPLASH_BIG, COLOR_ENERGY_BLUE);
    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
}


// ****************************************************************
// 
void cZerothBoss::__DisableLaser(const coreBool bAnimated)
{
    if(!m_Laser.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    const auto nExitFunc = [](coreObject3D* OUTPUT pObject)
    {
        pObject->SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(pObject);
    };
    nExitFunc(&m_Laser);
    nExitFunc(&m_LaserWave);

    if(bAnimated)
    {
        // 
        const coreVector3 vPos = m_Laser.GetPosition();
        const coreVector3 vDir = m_Laser.GetDirection();

        // 
        for(coreUintW j = 100u; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (1.0f * (I_TO_F(j) - 49.5f) + ZEROTH_RADIUS), 10.0f, 1u, COLOR_ENERGY_BLUE);
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
    this->__DestroyCube(m_aiCounter[ICE_INDEX], true);
    oIce.Resurrect();

    // 
    oIce.SetPosition (coreVector3(vPosition,  0.0f));
    oIce.SetDirection(coreVector3(vDirection, 0.0f));
}


// ****************************************************************
// 
void cZerothBoss::__DestroyCube(const coreUintW iIndex, const coreBool bAnimated)
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

    // 
    ASSERT(iIndex < ZEROTH_ICES)
    cCustomEnemy& oIce = m_aIce[iIndex];

    // 
    if(oIce.HasStatus(ENEMY_STATUS_DEAD)) return;

    // 
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        if(pMission->GetCatchObject(i) == &oIce)
        {
            pMission->UncatchObject(iIndex);
            break;
        }
    }

    // 
    oIce.Kill(bAnimated);
}


// ****************************************************************
// 
void cZerothBoss::__SetLimbValue(const coreUintW iIndex, const coreFloat fValue)
{
    ASSERT(iIndex < ZEROTH_LIMBS)
    cCustomEnemy& oLimb = m_aLimb[iIndex];

    // 
    m_afLimbValue[iIndex] = CLAMP01(fValue) * -5.0f;

    // 
    if(fValue >= 0.8f) oLimb.AddStatus   (ENEMY_STATUS_GHOST);
                  else oLimb.RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    if(fValue >= 1.0f) oLimb.AddStatus   (ENEMY_STATUS_HIDDEN);
                  else oLimb.RemoveStatus(ENEMY_STATUS_HIDDEN);
}