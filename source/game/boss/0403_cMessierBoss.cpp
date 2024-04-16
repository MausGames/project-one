///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// time-bullets of different ticks cannot be near each others along fly-dir(!), because of depth-order switch when reverting
// bei time bullets, wenn sich erzeugungs-tick nicht mit zerstörungs-tick überlager müsst der ticker beim invertieren ausgeglichen werden (derzeit nicht der fall)
// meteors rotating against their movement-direction makes them more visible
// first time-bullet phase is easier at the bottom (normal phase) but harder at the top (revert phase)
// meteors in the first phase should not move too slow, as it is just too easy and boring
// in the first phase, player will try to stay near boss, this is ok and should be amplified
// screen rotation sollte gegen gravi-rotation sein, ansonsten wird es viel zu schnell
// in gravi-rotation, bullet-wände sollen den spieler zwingen gegen strömung zu fliegen und sich dadurch zu drehen
// meteoriten sollten keine geschosse reflektieren, weil das sonst zu verwirrend ist
// bullets während meteoriten-phase war zu viel, vor allem weil sie ja auch von gravitation beeinflusst werden
// platten müssen so überlagert werden, dass sie gut sichtbar und effektiv sind (kleine über große, schnelle über langsamere)
// platten sollte spieler nur so wegdrehen, dass er mit 1 aktion wieder richtung boss schauen kann (außer wenn spieler neben boss fliegt, aber das is speziell)
// time bubble hides stopping background movement
// TODO 1: intro: meteoriten kommen zusammen, kurze drehung und wartezeit, dann bumm, und messier bricht raus schon mit schild
// TODO 3: reversed bullets should disappear into boss with slight fade (oder mit partikel-effekt kaschieren ? bei creation und destruction)
// TODO 1: MAIN: task-check, fragment, easy, hard idea, coop, regular score, extra score, medal goal, juiciness (move, rota, muzzle, effects), intro, outro, foreshadow, overdrive, sound, attack size/count/speed, enemy/boss size, object size, background rota/speed
// TODO 1: mehr von Ikrauga endboss inspirieren lassen https://youtu.be/AVFdOhf5-P0?t=1247
// TODO 1: spieler sollte bei start von plate-phase weggestoßen werden (oder ähnliches, damit er nicht den ersten bullet-angriff in die fresse bekommt, spieler is zu dem zeitpunkt nah dran)
// TODO 1: improve bullet-directions for time-slowdown phase (sometimes they cluster, maybe first identify the reason/origin)
// TODO 1: boss hat am ende noch einmal geschossen (TODO in cBoss.h)
// TODO 1: add ~0.3s delay between time-bullet-stages
// TODO 1: vielleicht geschosse in slowdown phase, erst werfen und verlangsamen sobald sie die bubble berühren, statt gleichzeitig
// TODO 1: in first phase, maybe boss should be in the middle, and move up for plate phase
// TODO 1: bei tod, implosion, mit allen meteoriten vom background, hellem licht (eher nicht wegen epilepsie)
// TODO 1: vielleicht meteoriten die am anfang bei der explosion schon sichtbar sind anders anordnen, Y wert (index) könnte geshifted werden
// TODO 1: badge: 1 meteor kann zerstört werden (leicht andere farbe ?)
// TODO 1: kleinere meteoriten sollten mehr aushalten, und erste wave-phase sollte dann verkürzt werden
// TODO 1: vielleicht die time-bullet phasen etwas verkürzen (100 HP), nicht zu viel
// TODO 1: vielleicht in blauer time-bullet phase den sinus-ausschwung variieren (fix leicht-stark-leicht-stark)
// TODO 1: übergang beim time-shift: uhr-werk effekt (+ glocken-schlag sound (NWN)) ! (sollte mit gravitations-effekt zusammenpassen, könnte einfach ringe sein, 1 äußerer schneller, 1 innerer langsamer) sollte dann permanent aktiv sein und rückwärts-laufen und mit slowdown-bubble implodieren (.txt: zwei zeiger erscheinen vorwärts und beginnen sich rückwärts zu drehen, verschwinden am ende der phase)
// TODO 1: lebenspunkte bei time-phase begrenzen (geben keine punkte, sollte somit als soft-clamp in takedamage eingebaut werden), aber heal-status mit factor<=0.0f erst umschlagen, und auflösen mit der welle (effekt muss es anzeigen)
// TODO 1: (vielleicht) in platten phase eine zweite sub-phase mit wave-bullets einbauen (vielleicht auch rotierenden platten, am ende kommt eine ausgleich-platte) (würd eigentlich gut kommen mit den anderen "bullet-sub-phasen" der anderen phasen)
// TODO 1: musik invertieren und verlangsamen ?
// TODO 1: flip-bullets vielleicht mit 0.5f*GA oder 1.5f rotieren (statt 1.0f)
// TODO 1: am anfang fliegt boss mit wenn alle kleine meteoriten weg sind, rastet ein bei nächster phase zusammen mit pause (kann aber länger sein, life-milestones bleiben ja gleich)
// TODO 1: art der rotation ändert sich im laufe des kampfes, achtung wegen rota in time-phase, sollte schön sichtbar sein
// TODO 1: arrow-platten mechanik soll angriffe vom boss ablenken, vielleicht nicht durchgezogene linien
// TODO 1: zusätzliche geschosse in time-phase, aber nur bei part 1 und 2, nicht 3
// TODO 1: from mission code: reduce bullet-animation + dark-animation speed (abhängig von flyspeed ?) (auch bei boss, auch fürs rückwärts-fliegen)
// TODO 1: fix messier model
// TODO 1: fix messier background rotation, wird von area speed-calculation in bewegung gesetzt


// ****************************************************************
// counter identifier


// ****************************************************************
// vector identifier
#define COVER_ROTATION (0u)
#define METEOR_DATA    (1u)
#define METEOR_MOVE    (2u)
#define PLATE_DATA     (3u)


// ****************************************************************
// constructor
cMessierBoss::cMessierBoss()noexcept
: m_fRingTime   (0.0f)
, m_fRingScreen (0.0f)
, m_fTimeFactor (1.0f)
, m_iTimeRevert (0u)
, m_afShootTime {}
, m_aiShootTick {}
, m_iTick       (0u)
, m_fAnimation  (0.0f)
, m_fRotation   (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_messier_core_high.md3");
    this->DefineModelLow ("ship_boss_messier_core_low.md3");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f) * 9.0f);
    this->SetOrientation(coreVector3(1.0f,1.0f,0.0f).Normalized());

    // configure the boss
    this->Configure(6600, 0u, COLOR_SHIP_MAGENTA);

    // 
    for(coreUintW i = 0u; i < MESSIER_SHELLS; ++i)
    {
        m_aShell[i].DefineModelHigh(i ? "ship_boss_messier_inside_high.md3" : "ship_boss_messier_outside_high.md3");
        m_aShell[i].DefineModelLow (i ? "ship_boss_messier_inside_low.md3"  : "ship_boss_messier_outside_low.md3");
        m_aShell[i].SetSize        (this->GetSize() * 1.2f * (i ? 1.0f : 1.1f));
        m_aShell[i].Configure      (1, 0u, COLOR_SHIP_MAGENTA);
        m_aShell[i].AddStatus      (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_SECRET);
        m_aShell[i].SetParent      (this);
    }

    // 
    for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
    {
        // load object resources
        m_aRing[i].DefineModel  ("object_ring.md3");
        m_aRing[i].DefineTexture(0u, "effect_energy.png");
        m_aRing[i].DefineProgram("effect_energy_spheric_program");
        m_aRing[i].SetColor3    (COLOR_ENERGY_MAGENTA * 0.7f);
        m_aRing[i].SetTexSize   (coreVector2(0.5f,1.0f) * 2.5f);
        m_aRing[i].SetTexOffset (coreVector2(I_TO_F(i) / I_TO_F(MESSIER_RINGS), 0.0f));
        m_aRing[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    }
}


// ****************************************************************
// 
void cMessierBoss::__ResurrectOwn()
{
    // 
    constexpr coreUint8 aiNewOrder[] = {cFlipBullet::ID};
    g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));

    // 
    this->_ResurrectBoss();
}


// ****************************************************************
// 
void cMessierBoss::__KillOwn(const coreBool bAnimated)
{
    cRutilusMission* pMission = d_cast<cRutilusMission*>(g_pGame->GetCurMission());

    // 
    for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
        pMission->DisablePlate(i, bAnimated);

    // 
    pMission->DisableArea(bAnimated);

    // 
    pMission->DisableWave(bAnimated);

    // 
    pMission->GetEnemySquad(0u)->ClearEnemies(bAnimated);
    pMission->GetEnemySquad(1u)->ClearEnemies(bAnimated);

    // 
    this->__DisableRings(bAnimated);

    // 
    g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
    });

    // 
    g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,1.0f));

    // 
    g_pGame->GetBulletManagerEnemy()->ResetOrder();
}


// ****************************************************************
// 
void cMessierBoss::__RenderOwnOver()
{
    if(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        DEPTH_PUSH_SHIP   // TODO 1: eigene RenderOwnShip kategorie, die vor den ship-outlines zeichnet, damit diese korrekt ge-merged werden

        // 
        const coreUintW iStart = MESSIER_RINGS - F_TO_UI(FRACT(m_fRingTime) * MESSIER_RINGS);

        // 
        for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
            m_aRing[(iStart + i) % MESSIER_RINGS].Render();
    }
}


// ****************************************************************
// 
void cMessierBoss::__MoveOwn()
{
    cRutilusMission* pMission = d_cast<cRutilusMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    if(this->ReachedDeath())
    {
        this->Kill(true);   
        this->_EndBoss();
    }

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);
    const cEnemySquad* pSquad2 = g_pGame->GetCurMission()->GetEnemySquad(1u);
    ASSERT(pSquad1->GetNumEnemies() == MESSIER_ENEMIES_SMALL)
    ASSERT(pSquad2->GetNumEnemies() == MESSIER_ENEMIES_BIG)

    // 
    cSpaceBackground* pBackground = d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground());
    pBackground->SetMeteorSpeed(m_fTimeFactor);
    
    if(m_iPhase < 60u) g_pEnvironment->SetTargetSpeedNow(4.0f * m_fTimeFactor);

    // 
    m_fAnimation.UpdateMod(1.0f * m_fTimeFactor, 2.0f*PI);
    m_fRotation .UpdateMod(1.0f * m_fTimeFactor, 2.0f*PI);

    // 
    const coreVector2 vBossDir = coreVector2::Direction(m_fRotation);
    this->SetDirection(coreVector3(coreVector2(1.0f,-1.0f).Normalized() * vBossDir.x, vBossDir.y).Normalized());

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            this->SetPosition(coreVector3(0.0f,0.6f,0.0f) * FOREGROUND_AREA3);
            this->SetSize    (coreVector3(1.0f,1.0f,1.0f) * 10.0f * BLENDB(MIN1(fTime * 3.0f)));

            if(PHASE_BEGINNING)
            {
                this->_StartBoss();

                for(coreUintW i = 0u; i < MESSIER_ENEMIES_BIG; ++i)
                {
                    const coreVector2 vPos = coreVector2(FmodRange(I_TO_F(i) * -1.0f, MESSIER_METEOR_RANGE * -3.0f, MESSIER_METEOR_RANGE * -1.0f), (I_TO_F(i) - 3.5f) * (2.0f / I_TO_F(MESSIER_ENEMIES_BIG)) * MESSIER_METEOR_RANGE);

                    pSquad2->GetEnemy(i)->Resurrect();
                    pSquad2->GetEnemy(i)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }

                for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
                {
                    pSquad1->GetEnemy(i)->Resurrect();
                }

                g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_MAGENTA);
            }

            for(coreUintW i = 0u; i < MESSIER_ENEMIES_BIG; ++i)
            {
                if((i != 2u) && (i != 4u) && (i != 6u) && (i != 7u)) continue;

                const coreVector2 vTarget = coreVector2(FmodRange(I_TO_F(i) * -1.0f, -MESSIER_METEOR_RANGE, MESSIER_METEOR_RANGE), (I_TO_F(i) - 3.5f) * 0.25f * MESSIER_METEOR_RANGE);
                const coreVector2 vPos    = LERP(this->GetPosition().xy() / FOREGROUND_AREA, vTarget, BLENDB(fTime));

                pSquad2->GetEnemy(i)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }

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
            PHASE_CHANGE_TO(30u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        if(PHASE_BEGINNING2)
        {
            pMission->EnableWave(1u);

            pMission->SetWavePosition (coreVector2(1.1f,0.0f) * FOREGROUND_AREA);
            pMission->SetWaveDirection(coreVector2(0.0f,1.0f));
            pMission->SetWavePull     (true);
            pMission->SetWaveDelayed  (true);
        }

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            m_avVector[METEOR_MOVE].xy(LERP(coreVector2(0.0f,0.0f), coreVector2(1.0f,0.0f), fTime));
            m_avVector[METEOR_MOVE].zw(m_avVector[METEOR_MOVE].xy());
        });

        if(this->GetCurHealth() < 5900)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        if(PHASE_BEGINNING2)
        {
            pMission->SetWavePosition(coreVector2(-1.1f,0.0f) * FOREGROUND_AREA);
        }

        m_avVector[METEOR_DATA].x = SIN(m_fPhaseTime) * 0.5f;

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            m_avVector[METEOR_MOVE].xy(LERP(coreVector2(1.0f,0.0f), coreVector2(-1.0f, m_avVector[METEOR_DATA].x), fTime));
            m_avVector[METEOR_MOVE].zw(m_avVector[METEOR_MOVE].xy());
        });

        if(this->GetCurHealth() < 5200)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        if(PHASE_BEGINNING2)
        {
            pMission->SetWavePosition (coreVector2(0.0f,1.1f) * FOREGROUND_AREA);
            pMission->SetWaveDirection(coreVector2(1.0f,0.0f));
        }

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            m_avVector[METEOR_MOVE].xy(LERP(coreVector2(-1.0f, m_avVector[METEOR_DATA].x), coreVector2( 0.4f, 1.0f), fTime));
            m_avVector[METEOR_MOVE].zw(LERP(coreVector2(-1.0f, m_avVector[METEOR_DATA].x), coreVector2(-0.2f, 1.0f), fTime));
        });

        if(this->GetCurHealth() < 4500)
        {
            PHASE_CHANGE_INC

            pMission->DisableWave(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(40u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        if(PHASE_BEGINNING2)
        {
            pMission->EnablePlate(0u, 0.0f, 0.0f, -0.25f, -0.25f);
            pMission->EnablePlate(1u, 0.0f, 0.0f, -0.25f, -0.25f);
            pMission->EnablePlate(3u, 0.0f, 0.0f, -0.25f, -0.25f);

            pMission->SetPlateRotated(0u, false);
            pMission->SetPlateRotated(1u, false);
            pMission->SetPlateRotated(3u, false);

            pMission->SetPlateDirection(0u, coreVector2( 1.0f,0.0f));
            pMission->SetPlateDirection(1u, coreVector2(-1.0f,0.0f));
            pMission->SetPlateDirection(3u, coreVector2(-1.0f,0.0f));
        }

        PHASE_CONTROL_TICKER(1u, 0u, 1.5f, LERP_LINEAR)
        {
            const coreVector2 vPos  = this->GetPosition().xy();
            const coreFloat   fBase = this->AimAtPlayerDual((iTick / 2u) % 2u).Angle();

            for(coreUintW j = 7u; j--; )
            {
                const coreVector2 vDir   = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 3.0f) * 12.0f) + fBase);
                const coreFloat   fSpeed = 1.4f - ABS(I_TO_F(j - 3u)) * 0.1f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.6f);
            }
        });

        if(this->GetCurHealth() < 3500)
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.5f)
        {
            PHASE_CHANGE_TO(50u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        const coreInt32 iCurHealth = this->GetCurHealth();

        if(iCurHealth > 2800)
        {
            if(this->__PhaseTicker(0u, 1.5f))
            {
                const coreVector2 vPos = this->GetPosition().xy();

                for(coreUintW j = 80u; j--; )
                {
                    if((j % 8u) < 4u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 2.25f + I_TO_F((m_iTick * 3u) % 8u) * 2.25f));

                    this->__AddBullet(2u, 1.0f, vPos,  vDir);
                    this->__AddBullet(2u, 1.0f, vPos, -vDir);
                }
            }
        }
        else if(iCurHealth > 2100)
        {
            if(this->__PhaseTicker(1u, 20.0f))
            {
                const coreVector2 vPos  = this->GetPosition().xy();
                const coreFloat   fBase = SIN(I_TO_F(m_iTick) * 0.2f) * 21.0f;

                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f + 18.0f + fBase));

                    this->__AddBullet(1u, 1.0f, vPos,  vDir);
                    this->__AddBullet(1u, 1.0f, vPos, -vDir);
                }
            }
        }
        else
        {
            if(this->__PhaseTicker(2u, MESSIER_SHOOT_RATE))
            {
                const coreVector2 vPos  = this->GetPosition().xy();
                const coreFloat   fBase = I_TO_F(m_iTick) * 3.0f;

                for(coreUintW j = 72u; j--; )
                {
                    if((j % 12u) < 6u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 2.5f * ((m_iTick % 2u) ? -1.0f : 1.0f) + fBase));

                    this->__AddBullet(0u, 1.0f + I_TO_F(j % 6u) * 0.07f, vPos,  vDir);
                    this->__AddBullet(0u, 1.0f + I_TO_F(j % 6u) * 0.07f, vPos, -vDir);
                }
            }

            if(iCurHealth <= 1400)
            {
                if(!m_iTimeRevert) g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_HEALER);
                });

                m_iTimeRevert = 1u;
            }
        }

        if(m_iTimeRevert && (iCurHealth >= 3500))
        {
            PHASE_CHANGE_TO(60u)
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        if(PHASE_BEGINNING2)
        {
            pMission->EnableArea();
            pMission->SetAreaPosition(this->GetPosition().xy());

            m_iTimeRevert = 2u;
        }

        if(PHASE_MAINTIME_POINT(1.0f))
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([](cBullet* OUTPUT pBullet)
            {
                if(g_pForeground->IsVisiblePoint(pBullet->GetPosition().xy(), 1.2f))
                {
                    pBullet->SetFlyDir(coreVector2::Direction(pBullet->GetFlyDir().Angle() * 9.5f));

                    const coreFloat fSpeed = I_TO_F(pBullet->GetDamage()) * MESSIER_SHOOT_STEP;
                    pBullet->SetSpeed(fSpeed);
                }
                else
                {
                    pBullet->Deactivate(false);
                }
            });

            m_iTimeRevert = 3u;

            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->RemoveStatus(PLAYER_STATUS_HEALER);   // TODO 1: also on kill
            });
        }

        const coreFloat A = 1.3f;
        pMission->SetAreaScale(ParaLerp(1.0f, 0.8f, 4.2f * A, BLENDH3(MIN(m_fPhaseTime * 0.85f / A, 1.0f))));

        if(PHASE_MAINTIME_POINT(10.0f))
        {
            PHASE_CHANGE_INC

            pMission->DisableArea(true);

            g_pEnvironment->SetTargetSpeedNow(0.0f);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(70u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 70u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.0f), fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->__EnableRings();

                for(coreUintW i = 0u; i < 4u; ++i)
                {
                    pSquad2->GetEnemy(i)->ChangeToNormal();
                    pSquad2->GetEnemy(i)->Resurrect();
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 71u)
    {
        const coreInt32 iCurHealth = this->GetCurHealth();

        if(iCurHealth > 1400)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 0.3f, LERP_LINEAR)
            {
                const coreVector2 vPos  = this->GetPosition().xy();
                const coreFloat   fBase = I_TO_F(iTick) * (0.05f*PI);

                for(coreUintW j = 48u; j--; )
                {
                    const coreVector2 vDir   = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 180.0f / 48.0f) + fBase);
                    const coreFloat   fSpeed = ((j / 12u) % 2u) ? 0.6f : 0.35f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.1f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.1f);
                }
            });
        }
        else
        {
            PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(m_fLifeTime * (0.08f*PI));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, this, vPos,  vDir)            ->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, this, vPos, -vDir)            ->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, this, vPos,  vDir.Rotated90())->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.0f, this, vPos, -vDir.Rotated90())->ChangeSize(1.5f);
            });

            const coreFloat fSpeed = LERP(5.0f, CORE_MATH_PRECISION, I_TO_F(iCurHealth) / 1400.0f);

            PHASE_CONTROL_TICKER(2u, 0u, fSpeed, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.9f, this, vPos, vDir)->ChangeSize(1.7f);
            });
        }
    }

    // ################################################################
    // ################################################################

    if(m_iPhase >= 40u)
    {
        m_avVector[PLATE_DATA].x += 1.0f * TIME;

        constexpr coreFloat fOffset = 0.625f * 1.5f;

        const coreFloat fHeight1 = m_avVector[PLATE_DATA].x * -0.6f + fOffset;
        const coreFloat fHeight2 = m_avVector[PLATE_DATA].x * -0.3f + fOffset;
        const coreFloat fHeight3 = m_avVector[PLATE_DATA].x * -0.3f + fOffset * 2.0f;

        const coreFloat fReal1 = (fHeight1 > 0.0f) ? fHeight1 : FmodRange(fHeight1, -fOffset, fOffset);
        const coreFloat fReal2 = (fHeight2 > 0.0f) ? fHeight2 : FmodRange(fHeight2, -fOffset, fOffset);
        const coreFloat fReal3 = (fHeight3 > 0.0f) ? fHeight3 : FmodRange(fHeight3, -fOffset, fOffset);

        pMission->SetPlateOffsetNow(0u, -fReal1);
        pMission->SetPlateOffsetNow(1u, fReal2);
        pMission->SetPlateOffsetNow(3u, fReal3);

        if(m_iPhase >= 41u)
        {
            if(!coreMath::IsNear(fReal1, 0.0f, 0.625f)) pMission->DisablePlate(0u, false);
            if(!coreMath::IsNear(fReal2, 0.0f, 0.625f)) pMission->DisablePlate(1u, false);
            if(!coreMath::IsNear(fReal3, 0.0f, 0.625f)) pMission->DisablePlate(3u, false);
        }
    }

    if(m_iTimeRevert == 1u)
    {
        PHASE_CONTROL_TIMER(4u, 0.3f, LERP_LINEAR)
        {
            m_fTimeFactor = LERPH3(1.0f, -1.0f, CLAMP(fTime, 0.0f, 1.0f));
        });
    }
    else if((m_iTimeRevert == 2u) || (m_iTimeRevert == 3u))
    {
        PHASE_CONTROL_TIMER(4u, 0.6f, LERP_LINEAR)
        {
            m_fTimeFactor = LERPH3(-1.0f, 1.0f, CLAMP(fTime, 0.0f, 1.0f));
        });
    }
    else
    {
        m_fTimeFactor = 1.0f;
    }

    // 
    m_aShell[0].SetPosition   (this->GetPosition   ());
    m_aShell[0].SetDirection  (this->GetDirection  ());   
    m_aShell[0].SetOrientation(this->GetOrientation());   

    // 
    m_aShell[1].SetPosition   (this->GetPosition   ());
    m_aShell[1].SetDirection  (this->GetDirection  ().InvertedX());   
    m_aShell[1].SetOrientation(this->GetOrientation().InvertedX());   

    // 
    if(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_fRingTime.Update(0.4f);

        for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
        {
            // 
            const coreFloat   fTime   = FRACT(MAX(m_fRingTime + (I_TO_F(i) / I_TO_F(MESSIER_RINGS)), 0.0f));
            const coreFloat   fOffset = (I_TO_F(i) / I_TO_F(MESSIER_RINGS)) * 4.0f*PI + 1.1f*PI;
            const coreVector2 vDir    = coreVector2::Direction(0.5f*PI * m_fRingTime + fOffset);
            const coreVector2 vOri    = coreVector2::Direction(1.0f*PI * m_fRingTime + fOffset);

            // 
            m_aRing[i].SetPosition   (this->GetPosition());
            m_aRing[i].SetSize       (coreVector3(1.0f,1.0f,1.0f) * LERP(10.0f, 30.0f, fTime));
            m_aRing[i].SetDirection  (coreVector3(vDir, 0.0f));
            m_aRing[i].SetOrientation(OriRoundDir(vOri, vDir));
            m_aRing[i].SetAlpha      (LERPH3(0.0f, 1.0f, MIN(1.0f - fTime, 4.0f * fTime, 1.0f)));
            m_aRing[i].Move();
        }

        // 
        const coreVector2 vCameraPos = g_pEnvironment->GetCameraPos().xy();
        const coreFloat   fRotaTime  = -TIME * CLAMP((m_fRingTime + 1.0f) * 0.5f, 0.0f, 1.0f);

        // 
        const auto nRotaFunc = [&](const coreVector2 vPosition)
        {
            return coreMatrix3::Rotation(fRotaTime * MAX(1.0f - vPosition.LengthSq() / POW2(80.0f), 0.01f)).m12();
        };

        // 
        coreBatchList* pList = pBackground->GetMeteorList();
        for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
        {
            coreObject3D* pMeteor = (*pList->List())[i];
            if(!pMeteor->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

            // 
            const coreVector2 vRelPos = pMeteor->GetPosition().xy() - vCameraPos;
            const coreMatrix2 vRota   = nRotaFunc(vRelPos);
            const coreVector2 vNewPos = vRelPos * vRota + vCameraPos;
            const coreVector2 vNewDir = pMeteor->GetDirection  ().xy() * vRota;
            const coreVector2 vNewOri = pMeteor->GetOrientation().xy() * vRota;

            // 
            pMeteor->SetPosition   (coreVector3(vNewPos, pMeteor->GetPosition   ().z));
            pMeteor->SetDirection  (coreVector3(vNewDir, pMeteor->GetDirection  ().z).Normalized());
            pMeteor->SetOrientation(coreVector3(vNewOri, pMeteor->GetOrientation().z).Normalized());
        }

        // 
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            // 
            const coreMatrix2 vRota   = nRotaFunc(pPlayer->GetPosition().xy());
            const coreVector2 vNewPos = pPlayer->GetPosition ().xy() * vRota;
            const coreVector2 vNewDir = pPlayer->GetDirection().xy() * vRota;

            // 
            pPlayer->SetPosition (coreVector3(vNewPos, 0.0f));
            pPlayer->SetDirection(coreVector3(vNewDir, 0.0f));
        });

        // 
        if(this->GetCurHealth() <= 1400) m_fRingScreen.Update(0.4f);

        // 
        m_avVector[COVER_ROTATION].x += TIME * 0.5f * STEPH3(0.0f, 10.0f, m_fRingTime);
        pBackground->SetCoverDir(coreVector2::Direction(m_avVector[COVER_ROTATION].x));

        // 
        m_avVector[COVER_ROTATION].y += TIME * 1.0f * STEPH3(0.0f, 10.0f, m_fRingScreen);
        g_pPostProcessing->SetDirectionGame(coreVector2::Direction(m_avVector[COVER_ROTATION].y));
    }

    // 
    if(m_iTimeRevert < 3u)
    {
        g_pGame->GetBulletManagerEnemy()->ForEachBullet([this](cBullet* OUTPUT pBullet)
        {
            if(!pBullet->HasStatus(BULLET_STATUS_IMMORTAL)) return;

            // 
            const coreFloat fSpeed = I_TO_F(pBullet->GetDamage()) * MESSIER_SHOOT_STEP;
            pBullet->SetSpeed(m_fTimeFactor * fSpeed);

            // 
            const coreVector2 vPos  = pBullet->GetPosition().xy();
            const coreVector2 vDiff = this->GetPosition().xy() - vPos;

            // 
            if(coreVector2::Dot(vDiff, pBullet->GetFlyDir()) > 0.0f)
            {
                pBullet->Deactivate(false);
            }

            // 
            if(pBullet->GetFlyTime() >= 0.5f)
            {
                if(vDiff.LengthSq() >= POW2(fSpeed * MESSIER_SHOOT_RADIUS))
                {
                    pBullet->Deactivate(false);
                }
            }
        });
    }

    //if((m_iPhase >= 1u) && (m_iPhase < 40u) && pSquad1->IsFinished())
    //{
    //    for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
    //    {
    //        //pSquad1->GetEnemy(i)->Resurrect();
    //    }
    //}

    // 
    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
    {
        STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

        const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * 0.25f*PI - m_fAnimation);
        const coreFloat   fLen = LERPB(0.0f, 18.0f, MIN1(fLifeTime));

        pEnemy->SetPosition (this->GetPosition() + coreVector3(vDir * fLen, 0.0f));
        pEnemy->SetDirection(coreVector3(coreVector2::Direction((m_fAnimation + I_TO_F(i))), 0.0f));
    });

    const auto nHideFunc = [this](cEnemy* OUTPUT pEnemy)
    {
        if(m_iPhase >= 33u) pEnemy->Kill(false);
    };

    STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
    {
        if(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * (2.0f/4.0f)*PI + fLifeTime * ((i % 2u) ? 0.5f : 1.25f));
            const coreFloat   fLen = LERPB(70.0f, 0.0f, CLAMP01((fLifeTime - 5.0f) * 0.1f)) + ((i % 2u) ? 42.0f : 22.0f);

            pEnemy->SetPosition(this->GetPosition() + coreVector3(vDir * fLen, 0.0f));
        }
        else
        {
            const coreVector2 vMove = ((i != 3u) && (i != 4u) && (i != 6u) && (i != 7u)) ? m_avVector[METEOR_MOVE].xy() : m_avVector[METEOR_MOVE].zw();

            coreVector2 vCurPos = pEnemy->GetPosition().xy() + vMove * (50.0f * TIME);

                 if((vCurPos.x < -FOREGROUND_AREA.x * MESSIER_METEOR_RANGE) && (vMove.x < 0.0f)) {vCurPos.x += FOREGROUND_AREA.x * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}
            else if((vCurPos.x >  FOREGROUND_AREA.x * MESSIER_METEOR_RANGE) && (vMove.x > 0.0f)) {vCurPos.x -= FOREGROUND_AREA.x * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}
                 if((vCurPos.y < -FOREGROUND_AREA.y * MESSIER_METEOR_RANGE) && (vMove.y < 0.0f)) {vCurPos.y += FOREGROUND_AREA.y * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}
            else if((vCurPos.y >  FOREGROUND_AREA.y * MESSIER_METEOR_RANGE) && (vMove.y > 0.0f)) {vCurPos.y -= FOREGROUND_AREA.y * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
        }

        pEnemy->SetDirection(coreVector3(coreVector2::Direction((-m_fAnimation + I_TO_F(i))), 0.0f));
    });
}


// ****************************************************************
// 
void cMessierBoss::__EnableRings()
{
    WARN_IF(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableRings(false);

    // 
    m_fRingTime = -1.0f;

    // 
    for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
    {
        m_aRing[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(&m_aRing[i]);
    }

    // 
    g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_MAGENTA);
}


// ****************************************************************
// 
void cMessierBoss::__DisableRings(const coreBool bAnimated)
{
    if(!m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
    {
        m_aRing[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
        g_pGlow->UnbindObject(&m_aRing[i]);
    }
}


// ****************************************************************
// 
void cMessierBoss::__AddBullet(const coreUintW iType, const coreFloat fSpeed, const coreVector2 vPosition, const coreVector2 vDirection)
{
    // 
    const coreInt32 iDamage = F_TO_SI(fSpeed / MESSIER_SHOOT_STEP);
    ASSERT(fSpeed && coreMath::IsNear(I_TO_F(iDamage) * MESSIER_SHOOT_STEP, fSpeed))

    // 
    const coreVector2 vPos = (m_fTimeFactor > 0.0f) ? vPosition : (vPosition + vDirection * (fSpeed * MESSIER_SHOOT_RADIUS));
    const coreVector2 vDir = vDirection;

    // 
    cBullet* pBullet;
    switch(iType)
    {
    default: ASSERT(false)
    case 0u: pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(iDamage, fSpeed, this, vPos, vDir)->ChangeSize(1.7f); break;
    case 1u: pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet> (iDamage, fSpeed, this, vPos, vDir)->ChangeSize(1.7f); break;
    case 2u: pBullet = g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(iDamage, fSpeed, this, vPos, vDir)->ChangeSize(1.3f); break;
    }

    // 
    pBullet->AddStatus(BULLET_STATUS_IMMORTAL);
}


// ****************************************************************
// 
coreBool cMessierBoss::__PhaseTicker(const coreUintW iIndex, const coreFloat fRate)
{
    ASSERT(iIndex < MESSIER_SHOOT_TIMES)
    coreFlow&   fShootTime = m_afShootTime[iIndex];
    coreUint32& iShootTick = m_aiShootTick[iIndex];

    fShootTime.Update(fRate * m_fTimeFactor);

    if(!InBetween(fShootTime, 0.0f, 1.0f))
    {
        fShootTime = FmodRange(fShootTime, 0.0f, 1.0f);

        if(m_fTimeFactor > 0.0f)
        {
            iShootTick += 1u;
            m_iTick     = iShootTick;
        }
        else
        {
            iShootTick -= 1u;
            m_iTick     = iShootTick + 1u - F_TO_UI(FLOOR(fRate * (MESSIER_SHOOT_RADIUS / BULLET_SPEED_FACTOR)));
        }

        return true;
    }

    STATIC_ASSERT((sizeof(iShootTick) == 4u) && (sizeof(m_iTick) == 4u))

    return false;
}