///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// kontinuierlicher laser erzeugt snow, oder laser und cube bewegung (na, was is mit schwingen), aber irgendwas das kontinuierlich nen pfad zeichnet
// TODO 1: handle cubes die am morgenstern hängen, aber jetzt zerstört werden müssen (weil ein neuer cube erzeugt wird)
// TODO 1: decent smoke on ice cubes
// TODO 1: schildkröte, zieht gliedmaßen ein, kann "umfallen"
// TODO 1: stone outside of boss should also be ice cube
// TODO 1: sollte sich mehr bewegen, so wie andere bosse
// nur extremitäten können angegriffen werden, ziehen sich zurück
// TODO 1: MAIN: fragment, easy, hard (decision), coop, 3 badges, boss health, medal goal, intro, outro, foreshadow
// TODO 1: stacheln sollten geschosse unterbinden, volumen anpassen
// evade phase geht einmal so herum, dann anders herum, der schwanz schießt den kopf durch
// snow-laser darf sich nicht zu schnell bewegen (test on 60 FPS), weil er keine dynamische funktion hat, und dann löcher hinterlässt
// TODO 1: ice-cube damage in coop will be halved
// TODO 1: panzer hat eine eis-schicht, die verschwindet sobald der morgenstern ihn fängt, dadurch kann man ihn endlich immer angreifen
// TODO 1: in coop: vor-letzter eiswürfel fängt spieler 1, letzter fängt spieler 2, kurz vor ende von herumzieh phase reißt spieler 2 ab und verliert morgenstern


// ****************************************************************
// counter identifier
#define ICE_INDEX   (0u)
#define EVADE_COUNT (1u)
#define STAR_HOLD   (2u)
#define DRAG_COUNT  (3u)


// ****************************************************************
// vector identifier
#define STAR_LENGTH (0u)


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
    this->Configure(10000, COLOR_SHIP_BLUE);
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
        m_aLimb[i].Configure      (1, COLOR_SHIP_BLUE);
        //m_aLimb[i].AddStatus      (ENEMY_STATUS_BOTTOM);
        m_aLimb[i].SetParent      (this);
    }

    // 
    m_Body.DefineModelHigh("ship_boss_zeroth_body_high.md3");
    m_Body.DefineModelLow ("ship_boss_zeroth_body_low.md3");
    m_Body.DefineVolume   ("ship_boss_zeroth_body_volume.md3");
    m_Body.SetSize        (this->GetSize());
    m_Body.Configure      (1, COLOR_SHIP_BLUE);
    m_Body.AddStatus      (/*ENEMY_STATUS_BOTTOM | */ENEMY_STATUS_INVINCIBLE);
    m_Body.SetParent      (this);
    
    // TODO 1: bottom wegen kette !

    // 
    m_Laser.DefineModel  ("object_tube_open.md3");
    m_Laser.DefineTexture(0u, "effect_energy.png");
    m_Laser.DefineProgram("effect_energy_direct_program");
    m_Laser.SetColor3    (COLOR_ENERGY_BLUE);
    m_Laser.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Laser2.DefineModel  ("object_tube_open.md3");
    m_Laser2.DefineTexture(0u, "effect_energy.png");
    m_Laser2.DefineProgram("effect_energy_program");
    m_Laser2.SetColor3    (COLOR_ENERGY_BLUE);
    m_Laser2.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    
    
    m_Laser2Wave.DefineModel  ("object_tube_open.md3");
    m_Laser2Wave.DefineTexture(0u, "effect_energy.png");
    m_Laser2Wave.DefineProgram("effect_energy_invert_program");
    m_Laser2Wave.SetColor3    (COLOR_ENERGY_WHITE * 0.8f);
    m_Laser2Wave.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    

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
void cZerothBoss::ResurrectIntro()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

    pMission->EnableStar(0u, this, coreVector2(0.0f,-8.0f));

    this->Resurrect();

    m_iPhase = 200u;
}


// ****************************************************************
// 
void cZerothBoss::__ResurrectOwn()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());

    if(!pMission->IsStarEnabled(0u))
    {
        // 
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pMission->EnableStar(i, pPlayer, coreVector2(0.0f,0.0f));
        });

        // 
        pMission->StartSwing(7.0f);
    }
}


// ****************************************************************
// 
void cZerothBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    this->__DisableLaser(bAnimated);
    this->__DisableLaser2(bAnimated);

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
        m_aIce[i].Kill(bAnimated);

    // 
    if(m_iPhase < 200u) this->_EndBoss(bAnimated);
}


// ****************************************************************
// 
void cZerothBoss::__RenderOwnUnder()
{
    DEPTH_PUSH

    // 
    m_Laser.Render();

    glDepthMask(false);
    m_Laser2Wave.Render();
    glDepthMask(true);

    m_Laser2.Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FULL)->ApplyObject(&m_Laser2);
}


// ****************************************************************
// 
void cZerothBoss::__RenderOwnOver()
{
    DEPTH_PUSH

    // 
    for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
        m_aIce[i].DefineTexture(0u, m_aIce[i].HasStatus(ENEMY_STATUS_DEAD) ? NULL : g_pEnvironment->GetFrameBuffer()->GetColorTarget(0u).pTexture);   // TODO 1: (look at cMenu::__Reset) needs to be properly unset before engine reset, otherwise shadow texture breaks
    // GetBackground()->GetResolvedTexture()
}


// ****************************************************************
// 
void cZerothBoss::__MoveOwn()
{
    cCalorMission* pMission = d_cast<cCalorMission*>(g_pGame->GetCurMission());
    coreObject3D*  pStar    = pMission->GetStar(0u);

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
        g_pGame->GetInterface()->ChangeBannerText(PRINT("%02XZE%02XRO%02XTH%02X", coreUint8(oRand.Raw()), coreUint8(oRand.Raw()), coreUint8(oRand.Raw()), coreUint8(oRand.Raw())), NULL);
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
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 10u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < ZEROTH_LIMBS; ++i)
            {
                this->__SetLimbValue(i, (i == ZEROTH_LIMB_TAIL) ? 0.0f : 1.0f);
            }
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

                    m_aLimb[i].AddStatus(ENEMY_STATUS_GHOST);
                    ASSERT(i == ZEROTH_LIMB_TAIL)
                }
                else if(m_aiCounter[EVADE_COUNT] < 6)
                {
                    this->__SetLimbValue(i, 1.0f);

                    coreUintW iNextLimb;
                    switch(m_aiCounter[EVADE_COUNT])
                    {
                    case 3: iNextLimb = 0u; break;
                    case 4: iNextLimb = 4u; break;
                    case 5: iNextLimb = 2u; break;
                    }

                    this->__SetLimbValue(iNextLimb, 0.0f);
                }
                else
                {
                    for(coreUintW j = 0u; j < ZEROTH_LIMBS; ++j)
                    {
                        this->__SetLimbValue(j, (i == j) ? 1.0f : 0.0f);
                    }
                }

                break;
            }
        }

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            if(m_aiCounter[EVADE_COUNT] < 3)
            {
                coreFloat fNewAngle;
                switch(m_aiCounter[EVADE_COUNT])
                {
                default: fNewAngle =  (3.0f/3.0f)*PI; break;
                case 1:  fNewAngle =  (1.0f/3.0f)*PI; break;
                case 2:  fNewAngle = (-1.0f/3.0f)*PI; break;
                }

                const coreFloat fOldAngle = FmodRange(m_vLastDirection.xy().Angle(), 0.0f*PI, 2.0f*PI);

                this->DefaultRotateLerp(fOldAngle, fNewAngle, fTime);

                if(PHASE_FINISHED)
                {
                    m_aLimb[ZEROTH_LIMB_TAIL].RemoveStatus(ENEMY_STATUS_GHOST);
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 20u)
    {
        if(PHASE_BEGINNING2)
        {
            pMission->GetSnow()->Enable();
        }

        PHASE_CONTROL_TIMER(0u, 0.075f, LERP_LINEAR)
        {
            this->DefaultRotateLerp(0.0f*PI, 2.0f*PI, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
        {
            if(iTick % 2u)
            {
                this->__DisableLaser2(true);
            }
            else
            {
                this->__EnableLaser2(ZEROTH_LIMB_HEAD);
            }
        });

        if(m_Laser2.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            pMission->GetSnow()->DrawRay(this->GetPosition().xy(), m_Laser2.GetDirection().xy(), SNOW_TYPE_ADD);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        // TODO 1: 2 slow, 4 fast (invert rotation ?)

        const coreFloat   fGlobalSpeed = 30.0f;
        const coreVector2 vGlobalDir   = g_pGame->GetPlayer(0u)->GetDirection().xy();
        const coreVector2 vGlobalMove  = vGlobalDir * (-fGlobalSpeed * TIME);

        const auto nMoveFunc = [&](coreObject3D* OUTPUT pObject)
        {
            coreVector2 vNewPos = pObject->GetPosition().xy() + vGlobalMove;

                 if((vNewPos.x < -FOREGROUND_AREA.x * 1.45f) && (vGlobalMove.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.9f;
            else if((vNewPos.x >  FOREGROUND_AREA.x * 1.45f) && (vGlobalMove.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.9f;
                 if((vNewPos.y < -FOREGROUND_AREA.y * 1.45f) && (vGlobalMove.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.9f;
            else if((vNewPos.y >  FOREGROUND_AREA.y * 1.45f) && (vGlobalMove.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.9f;

            pObject->SetPosition(coreVector3(vNewPos, 0.0f));
        };

        nMoveFunc(this);

        this->DefaultRotate(m_fPhaseTime);

        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
        {
            if(pBullet->GetFlyTime() >= 4.0f) pBullet->Deactivate(true);
            nMoveFunc(pBullet);
        });

        PHASE_CONTROL_TICKER(1u, 0u, 5.0f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < 2u; ++i)
            {
                const coreVector2 vDir = MapToAxis(coreVector2(0.0f, i ? -1.0f : 1.0f), this->GetDirection().xy());
                const coreVector2 vPos = this->GetPosition().xy() + vDir * 8.0f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5u, 0.5f, this, vPos, vDir)->ChangeSize(1.9f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        this->SetDirection(coreVector3(this->AimAtPlayerDual(0u).Normalized(), 0.0f));

        PHASE_CONTROL_TICKER(1u, 0u, 0.2f, LERP_LINEAR)
        {
            this->__EnableLaser(this->GetPosition().xy(), this->GetDirection().xy());
            this->__DisableLaser(true);
        });

        for(coreUintW i = 0u; i < ZEROTH_ICES; ++i)
        {
            if(m_aIce[i].ReachedDeath())
            {
                PHASE_CHANGE_INC

                const cPlayer*  pPlayer = m_aIce[i].LastAttacker();
                const coreUintW iIndex  = pPlayer - g_pGame->GetPlayer(0u);

                pMission->StopSwing();
                
                pMission->SetStarLength(iIndex, CALOR_CHAIN_CONSTRAINT2);

                ADD_BIT(m_aiCounter[STAR_HOLD], iIndex)

                m_Body.RemoveStatus(ENEMY_STATUS_INVINCIBLE);

#if 0
                const cPlayer*  pPlayer = m_aIce[i].LastAttacker();
                const coreUintW iIndex  = pPlayer - g_pGame->GetPlayer(0u);

                pMission->CatchObject(iIndex, this);

                pMission->GetStar(iIndex)->SetPosition(this->GetPosition());
                pMission->StartSwing();

                const coreFloat fLength = (this->GetPosition().xy() - pPlayer->GetPosition().xy()).Length();

                m_avVector[STAR_LENGTH].x = fLength;
                pMission->SetStarLength(iIndex, fLength);
#endif
                break;
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction((I_TO_F(m_aiCounter[DRAG_COUNT]) + BLENDS(fTime)) * (0.5f*PI));

            this->SetPosition (coreVector3(vDir * 0.7f * FOREGROUND_AREA, 0.0f));
            this->SetDirection(coreVector3(vDir, 0.0f));

            g_pEnvironment->SetTargetDirectionNow(vDir);
            g_pEnvironment->SetTargetSpeedNow(16.0f);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
        
            const coreVector2 vPush = g_pEnvironment->GetDirection() * (LERPB(0.5f, -1.0f, 1.0f) * 60.0f * TIME);

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetPosition(pPlayer->GetPosition() + coreVector3(vPush, 0.0f));
            });
            
            

        PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
        {
            if((iTick % 8u) < 4u) return;
            
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = -g_pEnvironment->GetDirection();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5u, 1.0f, this, vPos, vDir)->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 42u)
    {
        PHASE_CONTROL_TIMER(0u, 0.15f, LERP_LINEAR)
        {
            const coreFloat fSide = (m_aiCounter[DRAG_COUNT] % 2) ? -1.0f : 1.0f;
            
            const coreFloat   fSin = SIN(BLENDH3(fTime) * (2.0f*PI)) * fSide;
            const coreVector2 vDir = g_pEnvironment->GetDirection();
            const coreVector2 vPos = (vDir * 0.7f + vDir.Rotated90() * fSin * 0.7f) * FOREGROUND_AREA;
            
            const coreFloat   fSin2 = SIN(BLENDH3(CLAMP01((fTime - 0.05f) / 0.95f)) * (2.0f*PI)) * fSide;
            const coreVector2 vPos2 = (vDir * 0.5f + vDir.Rotated90() * fSin2 * 0.7f) * FOREGROUND_AREA;

            this->SetPosition (coreVector3(vPos, 0.0f));
            this->SetDirection(coreVector3((vPos - vPos2).Normalized(), 0.0f));

            if(PHASE_FINISHED)
            {
                if(this->GetCurHealthPct() < 0.97f)
                {
                    PHASE_CHANGE_TO(50u)
                }
                else
                {
                    PHASE_CHANGE_TO(41u)

                    m_aiCounter[DRAG_COUNT] += 1;
                }
            }
        });
        
            const coreVector2 vPush = g_pEnvironment->GetDirection() * (LERPB(0.5f, -1.0f, 1.0f) * 60.0f * TIME);

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetPosition(pPlayer->GetPosition() + coreVector3(vPush, 0.0f));
            });

        PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
        {
            if((iTick % 8u) < 4u) return;
            
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = -g_pEnvironment->GetDirection();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5u, 1.0f, this, vPos, vDir)->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreVector2 vDiff = this->GetPosition().xy() - g_pGame->GetPlayer(0u)->GetPosition().xy();
            
            const coreFloat fFrom = m_vLastDirection.xy().Angle();
            const coreFloat fTo   = vDiff.Angle();
            
            this->DefaultRotateLerp(fFrom, fFrom + AngleDiff(fTo, fFrom) + (2.0f*PI), BLENDB(MIN1(fTime*1.0f)));
            
            const coreFloat fSpeed = LERP(16.0f, 8.0f, fTime);
            g_pEnvironment->SetTargetSpeedNow(fSpeed);
            
            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pMission->CatchObject(0u, this);
                pMission->StartSwing(2.0f);

                const coreFloat fLength = vDiff.Length();

                m_avVector[STAR_LENGTH].x = fLength;
                pMission->SetStarLength(0u, fLength);
            }
        });
        
            const coreVector2 vPush = g_pEnvironment->GetDirection() * (LERP(0.0f, -1.0f, STEP(8.0f, 16.0f, g_pEnvironment->GetSpeed())) * 60.0f * TIME);

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetPosition(pPlayer->GetPosition() + coreVector3(vPush, 0.0f));
            });
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fLength = LERPH3(m_avVector[STAR_LENGTH].x, 30.0f, fTime);

            pMission->SetStarLength(0u, fLength);
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
    
    
    if(m_Laser.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        m_Laser.SetAlpha(MAX(m_Laser.GetAlpha() - 2.0f * TIME, 0.0f));
        
        if(!m_Laser.GetAlpha()) this->__DisableLaser(false);
        
        m_Laser.SetTexOffset(coreVector2(0.1f,-0.5f) * m_fAnimation);
        m_Laser.Move();
    }

    if(m_Laser2.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        const coreVector2 vDir = MapToAxis(m_vLaserDir, this->GetDirection().xy());
        
        m_Laser2.SetPosition (coreVector3(this->GetPosition().xy() + vDir * 45.0f, 0.0f));
        m_Laser2.SetSize     (coreVector3(1.7f, 45.0f, 1.7f));
        m_Laser2.SetDirection(coreVector3(vDir,                             0.0f));
        
        m_Laser2.SetTexOffset(coreVector2(0.1f,-0.5f) * m_fAnimation);
        m_Laser2.Move();
        
        
        
        m_Laser2Wave.SetPosition (m_Laser2.GetPosition ());
        m_Laser2Wave.SetSize     (m_Laser2.GetSize     () * coreVector3(1.4f,1.0,1.4f));
        m_Laser2Wave.SetDirection(m_Laser2.GetDirection());
        
        m_Laser2Wave.SetTexOffset(coreVector2(0.1f,-0.5f) * -m_fAnimation);
        m_Laser2Wave.Move();
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


    cSnow* pSnow = pMission->GetSnow();

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
            pPlayer->SetMoveSpeed((!pPlayer->IsRolling() && pSnow->IsActive() && pSnow->TestCollision(pPlayer->GetPosition().xy())) ? 0.2f : 1.0f);   // TODO 1: set back when disabling snow, cannot be outside because of morningstar
        });
    }
    
    
    
    for(coreUintW i = 0u; i < CALOR_STARS; ++i)
    {
        if(HAS_BIT(m_aiCounter[STAR_HOLD], i))
        {
            pMission->GetStar(i)->SetPosition(this->GetPosition());
        }
    }

                
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
void cZerothBoss::__EnableLaser2(const coreUintW iLimb)
{
    WARN_IF(m_Laser2.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;
    
    const coreVector2 vDirection = coreVector2::Direction(I_TO_F(iLimb) * (0.4f*PI));
    
        const coreVector2 vDir = MapToAxis(vDirection, this->GetDirection().xy());

    // 
    const coreFloat fLen = g_pForeground->RayIntersection(this->GetPosition().xy(), vDir);

    // 
    m_Laser2.SetPosition (coreVector3(this->GetPosition().xy() + vDir * (fLen * 0.5f), 0.0f));
    m_Laser2.SetSize     (coreVector3(2.0f, fLen * 0.5f, 2.0f));
    m_Laser2.SetDirection(coreVector3(vDir,                             0.0f));
    m_Laser2.SetAlpha    (1.0f);

    // 
    m_Laser2.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Laser2);
    
    m_Laser2Wave.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Laser2Wave);
    
    
    ASSERT(vDirection.IsNormalized())
    m_vLaserDir = vDirection;

    const coreVector3 vTarget = coreVector3(this->GetPosition().xy() + vDir * fLen, 0.0f);
    
    g_pSpecialEffects->CreateSplashColor(vTarget, SPECIAL_SPLASH_BIG, COLOR_ENERGY_WHITE);


    //this->__CreateCube(vTarget.xy(), -vDirection);
}


// ****************************************************************
// 
void cZerothBoss::__DisableLaser2(const coreBool bAnimated)
{
    if(!m_Laser2.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    //if(!bAnimated)
    {
        // 
        m_Laser2.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_Laser2);
        
        m_Laser2Wave.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_Laser2Wave);
        
    }

    if(bAnimated)
    {
        // 
        const coreVector3 vPos = m_Laser2.GetPosition();
        const coreVector3 vDir = m_Laser2.GetDirection();

        // 
        for(coreUintW j = 50u; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDir * (2.0f * (I_TO_F(j) - 24.5f)), 10.0f, 1u, COLOR_ENERGY_WHITE);
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
    m_afLimbValue[iIndex] = CLAMP01(fValue) * -5.0f;

    // 
    if(fValue >= 0.8f) oLimb.AddStatus   (ENEMY_STATUS_GHOST);
                  else oLimb.RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    if(fValue >= 1.0f) oLimb.AddStatus   (ENEMY_STATUS_HIDDEN);
                  else oLimb.RemoveStatus(ENEMY_STATUS_HIDDEN);
}