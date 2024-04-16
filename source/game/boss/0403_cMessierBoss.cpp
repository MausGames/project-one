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
// TODO 1: hard mode: in intro fliegt ein meteorit auf den bildschirm und crackt ihn
// TODO 1: MAIN: medal goal
// TODO 1: ACHIEVEMENT: name (), description (), destroy one big meteor / ###

// TODO 1: lebenspunkte bei time-phase begrenzen (geben keine punkte, sollte somit als soft-clamp in takedamage eingebaut werden), aber heal-status mit factor<=0.0f erst umschlagen, und auflösen mit der welle (effekt muss es anzeigen)
// TODO 1: art der rotation ändert sich im laufe des kampfes, achtung wegen rota in time-phase, sollte schön sichtbar sein

    // TODO 1: spieler sollte bei start von plate-phase weggestoßen werden (oder ähnliches, damit er nicht den ersten bullet-angriff in die fresse bekommt, spieler is zu dem zeitpunkt nah dran)
    // TODO 1: (vielleicht) in platten phase eine zweite sub-phase mit wave-bullets einbauen (vielleicht auch rotierenden platten, am ende kommt eine ausgleich-platte) (würd eigentlich gut kommen mit den anderen "bullet-sub-phasen" der anderen phasen)
    // TODO 1: arrow-platten mechanik soll angriffe vom boss ablenken, vielleicht nicht durchgezogene linien
    
    // TODO 1: finale phase sollt kleine meteoriten verwenden, die auch auf boss zurasen und explodieren um sie zu erneuern

// TODO 1: blaue geschosse hacken bei nem single-shot, weil der health-abhängig wert zu schnel lgeändert wird
// TODO 1: uhr-werk effekt verbessern, allgemeine optik, fade-out am ende
// TODO 1: improve bullet-directions for time-slowdown phase (sometimes they cluster, maybe first identify the reason/origin)
// TODO 1: eine cyan-bullet welle die nah beim boss war wurde normal weggeschleudert statt rotiert
// TODO 1: cover sollte sich beim schwarzen loch drehen ?


// ****************************************************************
// counter identifier


// ****************************************************************
// vector identifier
#define COVER_ROTATION (0u)
#define METEOR_DATA    (1u)
#define METEOR_MOVE    (2u)
#define PLATE_DATA     (3u)
#define HELPER_DATA    (4u)


// ****************************************************************
// constructor
cMessierBoss::cMessierBoss()noexcept
: m_fRingTime   (0.0f)
, m_fRingScreen (0.0f)
, m_fClockTime  (0.0f)
, m_bClockState (false)
, m_bHoleState  (false)
, m_fTimeFactor (1.0f)
, m_iTimeRevert (0u)
, m_bTimeMusic  (false)
, m_afShootTime {}
, m_aiShootTick {}
, m_iTick       (0u)
, m_fMeteorRota (0.0f)
, m_fBossRota   (0.0f)
{
    // load models
    this->DefineModelHigh("ship_boss_messier_core.md3");
    this->DefineModelLow ("ship_boss_messier_core.md3");

    // set object properties
    this->SetSize       (coreVector3(1.0f,1.0f,1.0f) * MESSIER_SCALE);
    this->SetOrientation(coreVector3(1.0f,1.0f,0.0f).Normalized());

    // configure the boss
    this->Configure(6300, 0u, COLOR_SHIP_MAGENTA);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_BOTTOM);

    // 
    for(coreUintW i = 0u; i < MESSIER_SHELLS; ++i)
    {
        m_aShell[i].DefineModelHigh(i ? "ship_boss_messier_inside_high.md3" : "ship_boss_messier_outside_high.md3");
        m_aShell[i].DefineModelLow (i ? "ship_boss_messier_inside_low.md3"  : "ship_boss_messier_outside_low.md3");
        m_aShell[i].DefineVolume   ("ship_boss_messier_volume.md3");
        m_aShell[i].SetSize        (this->GetSize() * 1.08f * (i ? 1.0f : 1.1f));
        m_aShell[i].Configure      (1, 0u, COLOR_SHIP_MAGENTA);
        m_aShell[i].AddStatus      (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_BOTTOM | ENEMY_STATUS_SECRET);
        m_aShell[i].SetParent      (this);
    }

    // 
    for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
    {
        m_aRing[i].DefineModel  ("object_ring.md3");
        m_aRing[i].DefineTexture(0u, "effect_energy.png");
        m_aRing[i].DefineProgram("effect_energy_spheric_program");
        m_aRing[i].SetColor3    (COLOR_ENERGY_MAGENTA * 0.7f);
        m_aRing[i].SetTexSize   (coreVector2(0.5f,1.0f) * 2.5f);
        m_aRing[i].SetTexOffset (coreVector2(I_TO_F(i) / I_TO_F(MESSIER_RINGS), 0.0f));
        m_aRing[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClock); ++i)
    {
        m_aClock[i].DefineModel  (i ? "object_arrow_long.md3" : "object_arrow.md3");
        m_aClock[i].DefineTexture(0u, "effect_energy.png");
        m_aClock[i].DefineProgram("effect_energy_invert_program");
        m_aClock[i].SetSize      (coreVector3(1.0f,1.0f,1.0f) * 3.0f);
        m_aClock[i].SetColor3    (COLOR_ENERGY_PURPLE * 0.9f);
        m_aClock[i].SetTexSize   (coreVector2(0.5f,0.2f) * 1.7f * (i ? 2.0f : 1.0f));
        m_aClock[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aHole); ++i)
    {
        m_aHole[i].DefineModel  ("object_sphere_center.md3");
        m_aHole[i].DefineTexture(0u, "effect_energy.png");
        m_aHole[i].DefineProgram(i ? "effect_energy_spheric_program" : "effect_energy_program");
        m_aHole[i].SetColor3    (COLOR_ENERGY_WHITE * (i ? 0.5f : 0.1f));
        m_aHole[i].SetTexSize   (coreVector2(2.0f,1.0f) * 0.3f);
        m_aHole[i].SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);
    }

    // 
    m_pBellSound = Core::Manager::Resource->Get<coreSound>("effect_bell.wav");
    m_pVoidSound = Core::Manager::Resource->Get<coreSound>("environment_space.wav");
}


// ****************************************************************
// 
void cMessierBoss::__ResurrectOwn()
{
    cRutilusMission* pMission = d_cast<cRutilusMission*>(g_pGame->GetCurMission());

    // 
    pMission->SetAreaUpdate(false);

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
    this->__DisableClock(bAnimated);

    // 
    this->__DisableHole(bAnimated);

    // 
    if(m_pVoidSound->EnableRef(this))
    {
        m_pVoidSound->Stop();
    }

    // 
    g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
    });
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->RemoveStatus(PLAYER_STATUS_HEALER);
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
        DEPTH_PUSH_SHIP   // TODO 1: eigene RenderOwnShip kategorie, die vor den ship-outlines zeichnet, damit diese korrekt ge-merged werden   -> ball und ringe

        // 
        const coreUintW iStart = MESSIER_RINGS - F_TO_UI(FRACT(m_fRingTime) * MESSIER_RINGS);

        // 
        for(coreUintW i = 0u; i < MESSIER_RINGS; ++i)
            m_aRing[(iStart + i) % MESSIER_RINGS].Render();
    }
}


// ****************************************************************
// 
void cMessierBoss::__RenderOwnTop()
{
    DEPTH_PUSH

    // 
    m_aClock[1].Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aClock[1]);

    DEPTH_PUSH

    // 
    m_aClock[0].Render();
    g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_aClock[0]);

    glDisable(GL_DEPTH_TEST);
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aHole); ++i)
            m_aHole[i].Render();
    }
    glEnable(GL_DEPTH_TEST);
}


// ****************************************************************
// 
void cMessierBoss::__MoveOwn()
{
    cRutilusMission* pMission = d_cast<cRutilusMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);
    const cEnemySquad* pSquad2 = g_pGame->GetCurMission()->GetEnemySquad(1u);
    ASSERT(pSquad1->GetNumEnemies() == MESSIER_ENEMIES_SMALL)
    ASSERT(pSquad2->GetNumEnemies() == MESSIER_ENEMIES_BIG)

    
    const coreFloat fTotalSpeed = pMission->GetAreaSpeed() * m_fTimeFactor;

    // 
    cSpaceBackground* pBackground = d_cast<cSpaceBackground*>(g_pEnvironment->GetBackground());
    pBackground->SetMeteorSpeed(fTotalSpeed);

    
    if(m_iPhase < 60u) g_pEnvironment->SetTargetSpeedNow(4.0f * fTotalSpeed);
    
    g_MusicPlayer.SetPitch(MAX(ABS(fTotalSpeed), 0.5f));
    g_pGame->SetMusicVolume(STEP(0.0f, 0.5f, ABS(fTotalSpeed)));
    
    if(!m_bTimeMusic && (fTotalSpeed < 0.0f))
    {
        m_bTimeMusic = true;

        g_MusicPlayer.Lock();
        {
            const coreDouble fFactor = g_MusicPlayer.GetCurMusic()->TellFactor();

            g_MusicPlayer.SelectName(this->GetMusicNameRev());
            g_MusicPlayer.GetCurMusic()->SeekFactor(1.0 - fFactor);
        }
        g_MusicPlayer.Unlock();
    }
    else if(m_bTimeMusic && (fTotalSpeed >= 0.0f))
    {
        m_bTimeMusic = false;

        g_MusicPlayer.Lock();
        {
            const coreDouble fFactor = g_MusicPlayer.GetCurMusic()->TellFactor();

            g_MusicPlayer.SelectName(this->GetMusicName());
            g_MusicPlayer.GetCurMusic()->SeekFactor(1.0 - fFactor);
        }
        g_MusicPlayer.Unlock();
    }

    const coreBool bIntro = (m_iPhase < 2u);

    if(!bIntro)
    {
        // 
        m_fMeteorRota.UpdateMod(1.0f * fTotalSpeed, 2.0f*PI);
        m_fBossRota  .UpdateMod(1.0f * fTotalSpeed, 2.0f*PI);
    }

    // 
    const coreVector2 vBossDir = coreVector2::Direction(m_fBossRota);
    this->SetDirection(coreVector3(coreVector2(1.0f,-1.0f).Normalized() * vBossDir.x, vBossDir.y).Normalized());
    
    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,-1.3f), coreVector2(0.0f,0.6f), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.35f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
                {
                    pSquad1->GetEnemy(i)->Resurrect();
                    pSquad1->GetEnemy(i)->AddStatus(ENEMY_STATUS_GHOST);
                }
            }

            const coreFloat   fAngle = fTime * (2.0f*PI);
            const coreVector2 vRota  = coreVector2::Direction(fAngle);

            for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
            {
                const coreFloat fLerpOld = CLAMP01((fTimeBefore - I_TO_F(i) / I_TO_F(MESSIER_ENEMIES_SMALL)) * I_TO_F(MESSIER_ENEMIES_SMALL));
                const coreFloat fLerp    = CLAMP01((fTime       - I_TO_F(i) / I_TO_F(MESSIER_ENEMIES_SMALL)) * I_TO_F(MESSIER_ENEMIES_SMALL));

                const coreFloat   fLen = LERPBR(2.5f, 0.1f + ((i < MESSIER_ENEMIES_SMALL - 1u) ? (I_TO_F(i) * 0.015f) : 0.0f), fLerp);
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * GA);
                const coreVector2 vPos = this->GetPosition().xy() + MapToAxis(vDir, vRota) * fLen * FOREGROUND_AREA;

                pSquad1->GetEnemy(i)->SetPosition  (coreVector3(vPos, 0.0f));
                pSquad1->GetEnemy(i)->DefaultRotate(fAngle + I_TO_F(i));

                if((fLerp >= 1.0f) && (fLerpOld < 1.0f))
                {
                    g_pSpecialEffects->MacroExplosionColorSmall(coreVector3(vPos, 0.0f), COLOR_ENERGY_MAGENTA);
                    g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
                }
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
        {
            //this->SetPosition(coreVector3(0.0f,0.6f,0.0f) * FOREGROUND_AREA3);
            this->SetSize    (coreVector3(1.0f,1.0f,1.0f) * MESSIER_SCALE * BLENDB(MIN1(fTime * 3.0f)));

            if(PHASE_BEGINNING)
            {
                this->_StartBoss();

                for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
                {
                    pSquad1->GetEnemy(i)->RemoveStatus(ENEMY_STATUS_GHOST);
                }
                this->RemoveStatus(ENEMY_STATUS_GHOST);

                for(coreUintW i = 0u; i < MESSIER_ENEMIES_BIG; ++i)
                {
                    const coreVector2 vPos = coreVector2(FmodRange(I_TO_F(i) * -1.0f, MESSIER_METEOR_RANGE * -3.0f, MESSIER_METEOR_RANGE * -1.0f), (I_TO_F(i) - 3.5f) * (2.0f / I_TO_F(MESSIER_ENEMIES_BIG)) * MESSIER_METEOR_RANGE);

                    pSquad2->GetEnemy(i)->Resurrect();
                    pSquad2->GetEnemy(i)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }

                g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_MAGENTA);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_09);
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
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(30u)
            //PHASE_CHANGE_TO(70u)
            
           // g_pEnvironment->SetTargetSpeedNow(0.0f);
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

        if(this->GetCurHealth() < 5700)
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

        if(this->GetCurHealth() < 5100)
        {
            PHASE_CHANGE_INC

            for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
            {
                pSquad1->GetEnemy(i)->Kill(true);
            }

            this->ChangeToNormal();
            for(coreUintW i = 0u; i < MESSIER_SHELLS; ++i)
            {
                m_aShell[i].ChangeToNormal();
            }
            for(coreUintW i = 0u; i < MESSIER_ENEMIES_BIG; ++i)
            {
                pSquad2->GetEnemy(i)->ChangeToTop();
            }
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

        if(PHASE_MAINTIME_POINT(3.0f))
        {
            this->_ResurrectHelper(ELEMENT_PURPLE, false);
        }

        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            m_avVector[METEOR_MOVE].xy(LERP(coreVector2(-1.0f, m_avVector[METEOR_DATA].x), coreVector2( 0.4f, 1.0f), fTime));
            m_avVector[METEOR_MOVE].zw(LERP(coreVector2(-1.0f, m_avVector[METEOR_DATA].x), coreVector2(-0.2f, 1.0f), fTime));
        });

        if(this->GetCurHealth() < 4400)
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
            //pMission->EnablePlate(3u, 0.0f, 0.0f, -0.25f, -0.25f);

            pMission->SetPlateRotated(0u, true);
            pMission->SetPlateRotated(1u, true);
            //pMission->SetPlateRotated(3u, false);

            pMission->SetPlateDirection(0u, coreVector2( 1.0f,0.0f));
            pMission->SetPlateDirection(1u, coreVector2(-1.0f,0.0f));
            //pMission->SetPlateDirection(3u, coreVector2(-1.0f,0.0f));

            for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
            {
           //     pSquad1->GetEnemy(i)->Resurrect();
            }

            m_avVector[METEOR_DATA].y = 0.0f;

            g_pSpecialEffects->MacroExplosionColorSmall(this->GetPosition(), COLOR_ENERGY_MAGENTA);
        }

        if(PHASE_MAINTIME_POINT(5.0f))
        {
            this->_ResurrectHelper(ELEMENT_YELLOW, false);
        }

        PHASE_CONTROL_TICKER(1u, 0u, g_pGame->IsEasy() ? 1.0f : 1.5f, LERP_LINEAR)
        {
            if(iTick == 0u)
            {
                g_pGame->ForEachPlayer([this](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    const coreVector2 vDiff = pPlayer->GetOldPos() - this->GetPosition().xy();
                    pPlayer->ApplyForce(vDiff.Normalized() * 100.0f);
                });
                return;
            }
            
            
            const coreVector2 vPos  = this->GetPosition().xy();
            const coreFloat   fBase = this->AimAtPlayerDual((iTick / 2u) % 2u).Angle();

            for(coreUintW j = 7u; j--; )
            {
                const coreVector2 vDir   = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 3.0f) * (g_pGame->IsEasy() ? 24.0f : 12.0f)) + fBase);
                const coreFloat   fSpeed = 1.4f - ABS(I_TO_F(j - 3u)) * 0.1f;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.7f);
            }
        });

        if(this->GetCurHealth() < 3500)
        {
            PHASE_CHANGE_INC

            for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
            {
                pSquad1->GetEnemy(i)->Kill(true);
            }
        }
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

        if(iCurHealth > 2900 + 25)
        {
            if(this->__PhaseTicker(0u, 1.5f))
            {
                const coreVector2 vPos  = this->GetPosition().xy();
                const coreUintW   iStep = F_TO_UI(LERP(5.0f - CORE_MATH_PRECISION, 2.0f + CORE_MATH_PRECISION, STEP(2900.0f, 3500.0f - 100.0f, I_TO_F(iCurHealth))));

                for(coreUintW j = 80u; j--; )
                {
                    if((j % BIT(iStep)) < BIT(iStep - 1u)) continue;
                    if(g_pGame->IsEasy() && ((j % 16u) < 8u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 2.25f + I_TO_F((m_iTick * 3u) % 8u) * 2.25f));

                    this->__AddBullet(2u, 1.0f, vPos,  vDir);
                    this->__AddBullet(2u, 1.0f, vPos, -vDir);
                }
            }
        }
        else if((iCurHealth <= 2900 - 25) && (iCurHealth > 2300 + 25))
        {
            if(this->__PhaseTicker(1u, 20.0f))
            {
                const coreVector2 vPos  = this->GetPosition().xy();
                const coreFloat   fBase = SIN(I_TO_F(m_iTick) * 0.2f) * (g_pGame->IsEasy() ? 17.0f : 24.0f) * SIN(STEP(2300.0f, 2900.0f, I_TO_F(iCurHealth)) * (1.0f*PI));

                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 36.0f + 18.0f + fBase));

                    this->__AddBullet(1u, 1.0f, vPos,  vDir);
                    this->__AddBullet(1u, 1.0f, vPos, -vDir);
                }
            }

            if(!m_iTimeRevert && (iCurHealth <= 2600))
            {
                this->_ResurrectHelper(ELEMENT_BLUE, false);
            }
        }
        else if(iCurHealth <= 2300 - 25)
        {
            if(this->__PhaseTicker(2u, MESSIER_SHOOT_RATE))
            {
                const coreVector2 vPos  = this->GetPosition().xy();
                const coreFloat   fBase = I_TO_F(m_iTick) * 3.0f;

                for(coreUintW j = 72u; j--; )
                {
                    if((j % 12u) < (g_pGame->IsEasy() ? 8u : 6u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 2.5f * ((m_iTick % 2u) ? -1.0f : 1.0f) + fBase));

                    this->__AddBullet(0u, 1.0f + I_TO_F(j % 6u) * 0.07f, vPos,  vDir);
                    this->__AddBullet(0u, 1.0f + I_TO_F(j % 6u) * 0.07f, vPos, -vDir);
                }
            }

            if(m_iTimeRevert && (iCurHealth >= 2000))
            {
                this->_ResurrectHelper(ELEMENT_ORANGE, false);
            }

            if(iCurHealth <= 1700)
            {
                if(!m_iTimeRevert)
                {
                    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                    {
                        pPlayer->AddStatus(PLAYER_STATUS_HEALER);
                    });

                    this->__EnableClock();
                }

                m_iTimeRevert = 1u;
            }
        }

        if(m_iTimeRevert && (iCurHealth >= 3500))
        {
            PHASE_CHANGE_TO(60u)

            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->RemoveStatus(PLAYER_STATUS_HEALER);
            });
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

            this->__DisableClock(true);

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
                    pBullet->SetSpeed    (fSpeed);
                    pBullet->SetAnimSpeed(1.0f);
                }
                else
                {
                    pBullet->Deactivate(false);
                }

                pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
            });

            m_iTimeRevert = 3u;

            if(this->_ResurrectHelper(ELEMENT_CYAN, true))
            {
                g_pGame->GetHelper(ELEMENT_CYAN)->SetPosition(coreVector3(-0.4f,0.0f,0.0f) * FOREGROUND_AREA3);
            }
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
                    if(g_pGame->IsEasy() && (((j + 6u) % 12u) < 6u)) continue;

                    const coreVector2 vDir   = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 180.0f / 48.0f) + fBase);
                    const coreFloat   fSpeed = ((j / 12u) % 2u) ? 0.6f : 0.35f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.1f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.1f);
                }
            });

            if(iCurHealth <= 2000)
            {
                this->_ResurrectHelper(ELEMENT_RED, false);
            }
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

            if(!g_pGame->IsEasy())
            {
                const coreFloat fSpeed = LERP(10.0f, CORE_MATH_PRECISION, I_TO_F(iCurHealth) / 1400.0f);

                PHASE_CONTROL_TICKER(2u, 0u, fSpeed, LERP_LINEAR)
                {
                    const coreVector2 vPos = this->GetPosition().xy();
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.9f, this, vPos, vDir)->ChangeSize(1.7f);
                });
            }

            if(iCurHealth <= 1000)
            {
                this->_ResurrectHelper(ELEMENT_MAGENTA, true);
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        if(PHASE_BEGINNING2)
        {
            this->AddStatus(ENEMY_STATUS_GHOST);
            for(coreUintW i = 0u; i < MESSIER_SHELLS; ++i)
            {
                m_aShell[i].AddStatus(ENEMY_STATUS_GHOST);
            }

            this->_EndBoss();

            for(coreUintW i = 0u; i < MESSIER_ENEMIES_SMALL; ++i)
            {
                pSquad1->GetEnemy(i)->Kill(true);
            }

            for(coreUintW i = 0u; i < MESSIER_ENEMIES_BIG; ++i)
            {
                pSquad2->GetEnemy(i)->Kill(true);
            }

            this->__DisableRings(true);

            g_pEnvironment->SetTargetSpeedNow(0.0f);

            g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
            });

            g_pSpecialEffects->MacroExplosionPhysicalDarkBig(this->GetPosition());
            g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
        }

        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            m_avVector[COVER_ROTATION].x += TIME * 0.5f * (1.0f - BLENDH3(fTime));
            pBackground->SetCoverDir(coreVector2::Direction(m_avVector[COVER_ROTATION].x));

            m_avVector[COVER_ROTATION].y += TIME * 1.0f * (1.0f - BLENDH3(fTime));
            g_pPostProcessing->SetDirectionGame(coreVector2::Direction(m_avVector[COVER_ROTATION].y));
        });

        PHASE_CONTROL_PAUSE(1u, 0.18f)
        {
            PHASE_CHANGE_INC

            m_avVector[COVER_ROTATION].y = FMOD(m_avVector[COVER_ROTATION].y, 2.0f*PI);
            m_avVector[COVER_ROTATION].z = (m_avVector[COVER_ROTATION].y > 1.0f*PI) ? (2.0f*PI) : (0.0f*PI);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        PHASE_CONTROL_TIMER(0u, 0.28f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->__EnableHole();

                m_pVoidSound->PlayRelative(this, 4.0f, 0.5f, true, SOUND_EFFECT);
            }

            g_pPostProcessing->SetDirectionGame(coreVector2::Direction(LERPBR(m_avVector[COVER_ROTATION].y, m_avVector[COVER_ROTATION].z, fTime)));

            pBackground->SetCoverScale(1.0f - STEPBR(0.1f, 1.0f, fTime));

            const coreFloat fScale = 1.0f - BLENDBR(fTime);

            this->SetSize(coreVector3(1.0f,1.0f,1.0f) * MESSIER_SCALE * fScale);
            for(coreUintW i = 0u; i < MESSIER_SHELLS; ++i)
            {
                m_aShell[i].SetSize(this->GetSize() * 1.08f * (i ? 1.0f : 1.1f));
            }

            m_aHole[0].SetSize(coreVector3(1.0f,1.0f,1.0f) * LERPB(0.0f, 7.0f, fTime));

            if(m_pVoidSound->EnableRef(this))
            {
                m_pVoidSound->SetPitch(LERPB(0.5f, 2.0f, fTime));
            }

            if(PHASE_FINISHED)
            {
                if(this->HasAllHelpers())
                {
                    this->_CreateFragment(4u);
                }

                this->__DisableHole(true);

                if(m_pVoidSound->EnableRef(this))
                {
                    m_pVoidSound->Stop();
                }

                g_pSpecialEffects->CreateExplosion (this->GetPosition());
                g_pSpecialEffects->CreateSplashDark(this->GetPosition(), 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->SlowScreen(4.0f);

                PHASE_CHANGE_INC
            }
        });

        const coreVector2 vCameraPos = g_pEnvironment->GetCameraPos().xy();
        const coreFloat   fMove      = 100.0f * MIN1(m_fPhaseTime * 0.25f) * TIME;

        coreBatchList* pList = pBackground->GetMeteorList();
        for(coreUintW i = 0u, ie = pList->List()->size(); i < ie; ++i)
        {
            coreObject3D* pMeteor = (*pList->List())[i];
            if(!pMeteor->IsEnabled(CORE_OBJECT_ENABLE_ALL)) continue;

            const coreVector2 vDiff = pMeteor->GetPosition().xy() - vCameraPos;
            if(vDiff.IsNull()) continue;

            const coreVector2 vPos  = pMeteor->GetPosition().xy() - vDiff.Normalized() * (fMove * SmoothTowards(vDiff.Length(), 0.5f));
            const coreVector3 vSize = pMeteor->GetSize() * FrictionFactor((1.0f - STEP(1.0f, 50.0f, vDiff.Length())) * 0.4f);

            pMeteor->SetPosition(coreVector3(vPos, pMeteor->GetPosition().z));
            pMeteor->SetSize    (vSize);
        }

    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        if(PHASE_MAINTIME_POINT(0.2f))
        {
            this->Kill(false);

            g_pEnvironment->ChangeBackground(cSpaceBackground::ID, ENVIRONMENT_MIX_FADE, 0.0f);
            g_pEnvironment->SetTargetSpeedNow(ENVIRONMENT_DEFAULT_SPEED);
        }
    }

    // ################################################################
    // ################################################################

    if(m_iPhase >= 40u)
    {
        m_avVector[PLATE_DATA].x += 0.5f * TIME    * (2.0f - STEP(3500.0f, 4400.0f, I_TO_F(this->GetCurHealth())));

        constexpr coreFloat fOffset = 0.625f * 1.5f;

        const coreFloat fHeight1 = m_avVector[PLATE_DATA].x * -0.6f + fOffset;
        const coreFloat fHeight2 = m_avVector[PLATE_DATA].x * -0.3f + fOffset;
        const coreFloat fHeight3 = m_avVector[PLATE_DATA].x * -0.3f + fOffset * 2.0f;

        const coreFloat fReal1 = (fHeight1 > 0.0f) ? fHeight1 : FmodRange(fHeight1, -fOffset, fOffset);
        const coreFloat fReal2 = (fHeight2 > 0.0f) ? fHeight2 : FmodRange(fHeight2, -fOffset, fOffset);
        const coreFloat fReal3 = (fHeight3 > 0.0f) ? fHeight3 : FmodRange(fHeight3, -fOffset, fOffset);

        //pMission->SetPlateOffsetNow(0u, -fReal1);
        pMission->SetPlateOffsetNow(1u, fReal3);
        //pMission->SetPlateOffsetNow(3u, fReal3);
        pMission->SetPlateOffsetNow(0u, -fReal2);

        if(m_iPhase >= 41u)
        {
            //if(!coreMath::IsNear(fReal1, 0.0f, 0.625f)) pMission->DisablePlate(0u, false);
            if(!coreMath::IsNear(fReal3, 0.0f, 0.625f)) pMission->DisablePlate(1u, false);
            //if(!coreMath::IsNear(fReal3, 0.0f, 0.625f)) pMission->DisablePlate(3u, false);
            if(!coreMath::IsNear(fReal2, 0.0f, 0.625f)) pMission->DisablePlate(0u, false);
        }
    }

    if(m_iTimeRevert == 1u)
    {
        PHASE_CONTROL_TIMER(4u, 0.3f, LERP_LINEAR)
        {
            m_fTimeFactor = LERPH3(1.0f, -1.0f, CLAMP(fTime, 0.0f, 1.0f));
        });
    }
    else if(m_iTimeRevert == 2u)
    {
        PHASE_CONTROL_TIMER(4u, 1.0f, LERP_LINEAR)
        {
            m_fTimeFactor = LERPH3(-1.0f, 0.0f, CLAMP(fTime, 0.0f, 1.0f));
        });
    }
    else
    {
        m_fTimeFactor = 1.0f;
    }
    
    if(!bIntro)
    {
        // 
        m_aShell[0].SetPosition   (this->GetPosition   ());
        m_aShell[0].SetDirection  (this->GetDirection  ());   
        m_aShell[0].SetOrientation(this->GetOrientation());   

        // 
        m_aShell[1].SetPosition   (this->GetPosition   ());
        m_aShell[1].SetDirection  (this->GetDirection  ().InvertedX());   
        m_aShell[1].SetOrientation(this->GetOrientation().InvertedX());
    }

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
    if(m_aClock[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        // 
        m_fClockTime.Update(-0.3f * m_fTimeFactor);

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClock); ++i)
        {
            // 
            if(m_bClockState) m_aClock[i].SetAlpha(MIN1(m_aClock[i].GetAlpha() + 5.0f * TIME));
                         else m_aClock[i].SetAlpha(MAX0(m_aClock[i].GetAlpha() - 5.0f * TIME));

            // 
            if(!m_aClock[i].GetAlpha()) this->__DisableClock(false);

            // 
            const coreVector2 vDir    = coreVector2::Direction(m_fClockTime * (i ? 1.5f : 1.0f) * (2.0f*PI));
            const coreFloat   fOffset = I_TO_F(i) * (1.0f/2.0f);

            // 
            m_aClock[i].SetPosition (coreVector3(this->GetPosition().xy() + vDir * (5.0f + m_aClock[i].GetCollisionRange().y), 0.0f));
            m_aClock[i].SetDirection(coreVector3(vDir, 0.0f));
            m_aClock[i].SetTexOffset(coreVector2(FRACT(0.8f * m_fClockTime + fOffset), 0.0f));
            m_aClock[i].Move();
        }
    }

    // 
    if(m_aHole[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
    {
        if(!m_bHoleState)
        {
            const coreFloat fAlpha = MAX0(m_aHole[0].GetAlpha() - 20.0f * TIME);
            const coreFloat fScale = 7.0f * LERPB(1.8f, 1.0f, fAlpha);

            m_aHole[0].SetSize (coreVector3(1.0f,1.0f,1.0f) * fScale);
            m_aHole[0].SetAlpha(fAlpha);

            if(!fAlpha) this->__DisableHole(false);
        }

        m_aHole[1].SetSize (m_aHole[0].GetSize () * 0.8f);
        m_aHole[1].SetAlpha(m_aHole[0].GetAlpha());

        m_aHole[0].SetTexOffset(coreVector2(0.0f, -FRACT(this->GetLifeTime() * 0.3f)));
        m_aHole[1].SetTexOffset(coreVector2(0.0f,  FRACT(this->GetLifeTime() * 0.3f)));

        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aHole); ++i)
        {
            m_aHole[i].SetPosition(this->GetPosition());
            m_aHole[i].Move();
        }
    }

    // 
    if(m_iTimeRevert < 3u)
    {
        g_pGame->GetBulletManagerEnemy()->ForEachBullet([this](cBullet* OUTPUT pBullet)
        {
            if(!pBullet->HasStatus(BULLET_STATUS_IMMORTAL)) return;

            // 
            const coreFloat fSpeed = I_TO_F(pBullet->GetDamage()) * MESSIER_SHOOT_STEP;
            pBullet->SetSpeed    (m_fTimeFactor * fSpeed);
            pBullet->SetAnimSpeed(m_fTimeFactor);

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

    if(!bIntro)
    {
        m_avVector[METEOR_DATA].y = MIN1(m_avVector[METEOR_DATA].y + 1.0f * TIME);

        // 
        STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * 0.25f*PI - m_fMeteorRota);
            const coreFloat   fLen = LERPB(0.0f, 20.0f, m_avVector[METEOR_DATA].y);

            pEnemy->SetPosition  (this->GetPosition() + coreVector3(vDir * fLen, 0.0f));
            pEnemy->DefaultRotate(m_fMeteorRota + I_TO_F(i));

            if(pEnemy->ReachedDeath())
            {
                if(!g_pGame->IsEasy())
                {
                    for(coreUintW j = 3u; j--; )
                    {
                        const coreVector2 vPos  = pEnemy->GetPosition().xy();
                        const coreVector2 vDir2 = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 60.0f));

                        // 
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f * 0.8f, this, vPos,  vDir2)->ChangeSize(1.7f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f * 0.8f, this, vPos,  vDir2)->ChangeSize(1.7f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f * 0.8f, this, vPos,  vDir2)->ChangeSize(1.7f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f * 0.8f, this, vPos, -vDir2)->ChangeSize(1.7f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f * 0.8f, this, vPos, -vDir2)->ChangeSize(1.7f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f * 0.8f, this, vPos, -vDir2)->ChangeSize(1.7f);
                    }
                }

                if(pSquad1->GetNumEnemiesAlive() == 4u)
                {
                    if(this->_ResurrectHelper(ELEMENT_GREEN, true))
                    {
                        g_pGame->GetHelper(ELEMENT_GREEN)->SetPosition(this->GetPosition());
                        m_avVector[HELPER_DATA].xy((pEnemy->GetPosition().xy() - this->GetPosition().xy()).Normalized());
                    }
                }
            }
        });
    }

    const auto nHideFunc = [this](cEnemy* OUTPUT pEnemy)
    {
        if(m_iPhase >= 33u) pEnemy->Kill(false);
    };

    const coreFloat fMeteorSpeed = (g_pGame->IsEasy() ? 30.0f : 50.0f) * TIME;

    STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
    {
        if(m_aRing[0].IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

            //const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * (2.0f/4.0f)*PI + fLifeTime * ((i % 2u) ? 0.5f : 1.25f));
            //const coreFloat   fLen = LERPB(70.0f, 0.0f, CLAMP01((fLifeTime - 5.0f) * 0.1f)) + ((i % 2u) ? 42.0f : 22.0f);
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(i) * (2.0f/4.0f)*PI + fLifeTime * ((i % 2u) ? 0.5f : 1.25f));
            const coreFloat   fLen = LERPB(70.0f, 0.0f, CLAMP01((fLifeTime - 5.0f) * 0.1f)) + ((i % 2u) ? 142.0f : LERP(42.0f, 22.0f, 0.5f + 0.5f * SIN(fLifeTime)));

            pEnemy->SetPosition(this->GetPosition() + coreVector3(vDir * fLen, 0.0f));
        }
        else
        {
            const coreVector2 vMove = ((i != 3u) && (i != 4u) && (i != 6u) && (i != 7u)) ? m_avVector[METEOR_MOVE].xy() : m_avVector[METEOR_MOVE].zw();

            coreVector2 vCurPos = pEnemy->GetPosition().xy() + vMove * fMeteorSpeed;

                 if((vCurPos.x < -FOREGROUND_AREA.x * MESSIER_METEOR_RANGE) && (vMove.x < 0.0f)) {vCurPos.x += FOREGROUND_AREA.x * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}
            else if((vCurPos.x >  FOREGROUND_AREA.x * MESSIER_METEOR_RANGE) && (vMove.x > 0.0f)) {vCurPos.x -= FOREGROUND_AREA.x * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}
                 if((vCurPos.y < -FOREGROUND_AREA.y * MESSIER_METEOR_RANGE) && (vMove.y < 0.0f)) {vCurPos.y += FOREGROUND_AREA.y * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}
            else if((vCurPos.y >  FOREGROUND_AREA.y * MESSIER_METEOR_RANGE) && (vMove.y > 0.0f)) {vCurPos.y -= FOREGROUND_AREA.y * MESSIER_METEOR_RANGE * 2.0f; nHideFunc(pEnemy);}

            pEnemy->SetPosition(coreVector3(vCurPos, 0.0f));
        }

        pEnemy->DefaultRotate(-m_fMeteorRota + I_TO_F(i));
    });

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(80u)
    }
    
    
    // 
    cHelper* pYellowHelper = g_pGame->GetHelper(ELEMENT_YELLOW);
    if(!pYellowHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pYellowHelper->GetLifeTime() * 0.5f;

        pYellowHelper->SetPosition(coreVector3(-1.0f, LERP(1.3f, -1.3f, fTime), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_YELLOW, false);
    }

    // 
    cHelper* pOrangeHelper = g_pGame->GetHelper(ELEMENT_ORANGE);
    if(!pOrangeHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pOrangeHelper->GetLifeTime() * 0.3f;

        pOrangeHelper->SetPosition(coreVector3(LERPBR(coreVector2(1.3f,-1.0f) * FOREGROUND_AREA, this->GetPosition().xy(), fTime), 0.0f));

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_ORANGE, true);
    }

    // 
    cHelper* pRedHelper = g_pGame->GetHelper(ELEMENT_RED);
    if(!pRedHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat   fTime = pRedHelper->GetLifeTime() * 0.2f;
        const coreFloat   fLen  = LERP(1.3f, 1.0f, SIN(fTime * (1.0f*PI))) * FOREGROUND_AREA.x;
        const coreVector2 vDir  = coreVector2::Direction(fTime * (2.0f*PI) + (0.0f*PI));
        const coreVector2 vPos  = vDir * fLen;

        pRedHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_RED, false);
    }

    // 
    cHelper* pMagentaHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);
    if(!pMagentaHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        pMagentaHelper->SetPosition(coreVector3(-1.0f,1.0f,0.0f) * FOREGROUND_AREA3);

        if(pMagentaHelper->GetLifeTime() >= 12.0f)
        {
            this->_KillHelper(ELEMENT_MAGENTA, true);
        }
    }

    // 
    cHelper* pPurpleHelper = g_pGame->GetHelper(ELEMENT_PURPLE);
    if(!pPurpleHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pPurpleHelper->GetLifeTime() * 0.2f;

        pPurpleHelper->SetPosition(coreVector3(LerpSmoothRev(-1.3f, 1.3f, fTime), -0.8f, 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_PURPLE, false);
    }

    // 
    cHelper* pBlueHelper = g_pGame->GetHelper(ELEMENT_BLUE);
    if(!pBlueHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pBlueHelper->GetLifeTime() * 0.4f;

        pBlueHelper->SetPosition(coreVector3(0.0f, -1.3f + 0.4f * SIN(fTime * (1.0f*PI)), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_BLUE, false);
    }

    // 
    cHelper* pCyanHelper = g_pGame->GetHelper(ELEMENT_CYAN);
    if(!pCyanHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        if(pCyanHelper->GetLifeTime() >= 5.0f)
        {
            this->_KillHelper(ELEMENT_CYAN, true);
        }
    }

    // 
    cHelper* pGreenHelper = g_pGame->GetHelper(ELEMENT_GREEN);
    if(!pGreenHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vPos = pGreenHelper->GetPosition().xy() + m_avVector[HELPER_DATA].xy() * (10.0f * TIME);

        pGreenHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pGreenHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_GREEN, false);
        }
    }
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
    g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_10);
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
void cMessierBoss::__EnableClock()
{
    WARN_IF(m_aClock[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableClock(false);

    // 
    m_bClockState = true;

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClock); ++i)
    {
        m_aClock[i].SetAlpha  (0.0f);
        m_aClock[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(&m_aClock[i]);
    }

    // 
    g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_PURPLE);
    g_pDistortion->CreateWave(this->GetPosition(), 40.0f, 0.3f);

    // 
    g_pSpecialEffects->ExternPlayPosition(m_pBellSound, NULL, 1.0f, 1.0f, false, SOUND_EFFECT, this->GetPosition());
}


// ****************************************************************
// 
void cMessierBoss::__DisableClock(const coreBool bAnimated)
{
    if(!m_aClock[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_bClockState = false;

    if(!bAnimated)
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClock); ++i)
        {
            m_aClock[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            g_pGlow->UnbindObject(&m_aClock[i]);
        }
    }
}


// ****************************************************************
// 
void cMessierBoss::__EnableHole()
{
    WARN_IF(m_aHole[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableHole(false);

    // 
    m_bHoleState = true;

    // 
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_aClock); ++i)
    {
        m_aHole[i].SetSize   (coreVector3(0.0f,0.0f,0.0f));
        m_aHole[i].SetAlpha  (1.0f);
        m_aHole[i].SetEnabled(CORE_OBJECT_ENABLE_ALL);
        g_pGlow->BindObject(&m_aHole[i]);
    }
}


// ****************************************************************
// 
void cMessierBoss::__DisableHole(const coreBool bAnimated)
{
    if(!m_aHole[0].IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_bHoleState = false;

    if(!bAnimated)
    {
        // 
        for(coreUintW i = 0u; i < ARRAY_SIZE(m_aHole); ++i)
        {
            m_aHole[i].SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
            g_pGlow->UnbindObject(&m_aHole[i]);
        }
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

    //if(!this->__PhaseDelay(iIndex, ((m_iTimeRevert == 1u) && (iIndex == 1u)) ? 0.7f : 0.3f)) return false;

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