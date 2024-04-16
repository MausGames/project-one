///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// pillen und pacman aussehen passen sehr gut zusammen
// using teleporter was too complicated, either you had to attack boss and attack teleporter at the same time, which was just too much, or if teleporter used as "don't hit them", hitting them completely confuses the player
// man muss merken, dass die sichtbarkeit der bumeränge kürzer wird, thresholds entsprechend setzen
// erste phase soll einleiten, und auch zeigen, dass du dem boss keinen schaden zufügen kannst, während sein mund geschlossen ist
// geschosse in einsaug-phase sollten den mund so verdecken, dass es keinen safe-spot geben kann
// dharuk sollte sich nicht zu schnell bewegen, weil es schon schwer genug ist ihn gut zu treffen
// dharuk sollte in letzter phase nicht mehr unsichtbar werden, weil das duplicate eh schon zu sehr ablenkt
// seiten-geschosse bei dharuk sind einfach zu verarbeiten als gezielte geschosse oder rundum geschosse, es sollte aber nur 1 geschoss-loch vorhanden sein, weil das präzise lineare durch-fliegen vom feeling her guten kontrast zum freien positionieren zwischendurch hat (die seiten-geschosse passen auch gut zum spawning aus allen 4 richtungen)
// der erste abschnitt dreht sich um fressen, der zweite um dharuk, der dritte ums aufteilen
// orange geschosse sollten bis zum rand gehen, aber bei abwechselnden schießen wird das nur von einer der beiden hälften gemacht ohne es komplett zu überspannen
// stampf-phase geht nur auf einer ache, um die achse zu verändern müsste der boss in die mitte springen
// pearls sollten so erzeugt werden, dass ihre animation eine schöne welle erzeugt (in bewegungs-richtung vom boss)
// TODO 1: [MF] MAIN: hard idea, sound
// TODO 1: lebenspunkte des inneren bosses sollten sichtbar werden (entweder zweiter bar, der separat gesteuert werden kann, oder ausblenden boss life 1, einblenden boss life 2 (mit hoch-animieren), oder shared life, anpassen der health-grenzen und anwenden auf geminga)
// TODO 1: player bullets shot into mouth should be on the same visual height
// TODO 1: repair-enemy wird beim einsaugen und ausspucken über boss gezeichnet, boss kann aber nicht TOP gesetzt werden, wegen partikel-effekte, repair-enemy muss angepasst werden (render-order oder size)


// ****************************************************************
// counter identifier
#define CONNECTED_MOUTH (0u)
#define SMASH_COUNT     (1u)
#define STRIKE_COUNT    (2u)
#define BULLET_REFLECT  (3u)


// ****************************************************************
// vector identifier
#define OLD_MOUTH_ANGLE (0u)
#define SMASH_POSITION  (1u)
#define FOLLOW_VALUES   (2u)
#define BOUNCE_FORCE    (3u)
#define FINAL_ROTATION  (4u)
#define OLD_ANGLE       (5u)
#define SUCK_ANGLE      (6u)
#define ENV_ROTATION    (7u)
#define HELPER_DATA     (8u)


// ****************************************************************
// constructor
cGemingaBoss::cGemingaBoss()noexcept
: m_fMouthAngle (0.0f)
, m_SuckEffect  (g_pSpecialEffects->GetParticleColor())
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize       (coreVector3( 1.0f,1.0f,1.0f) * 3.0f);
    this->SetOrientation(coreVector3(-1.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(8600, 0u, COLOR_SHIP_RED);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

    // 
    m_Sphere.DefineModelHigh("object_sphere.md3");
    m_Sphere.DefineModelLow ("object_sphere.md3");
    m_Sphere.SetSize        (this->GetSize() * 4.0f);
    m_Sphere.Configure      (1, 0u, COLOR_SHIP_RED);
    m_Sphere.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_HIDDEN | ENEMY_STATUS_SECRET);
    m_Sphere.SetParent      (this);

    // 
    m_InsideTop.DefineModelHigh("ship_boss_amemasu_top_inside.md3");
    m_InsideTop.DefineModelLow ("ship_boss_amemasu_top_inside.md3");
    m_InsideTop.SetSize        (this->GetSize());
    m_InsideTop.Configure      (1, 0u, COLOR_SHIP_RED);
    m_InsideTop.AddStatus      (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_HIDDEN | ENEMY_STATUS_SECRET);
    m_InsideTop.SetParent      (this);

    // 
    m_InsideBottom.DefineModelHigh("ship_boss_amemasu_bottom_inside.md3");
    m_InsideBottom.DefineModelLow ("ship_boss_amemasu_bottom_inside.md3");
    m_InsideBottom.SetSize        (this->GetSize());
    m_InsideBottom.Configure      (1, 0u, COLOR_SHIP_RED);
    m_InsideBottom.AddStatus      (ENEMY_STATUS_DAMAGING | ENEMY_STATUS_HIDDEN | ENEMY_STATUS_SECRET);
    m_InsideBottom.SetParent      (this);

    // 
    m_Top.DefineModelHigh("ship_boss_amemasu_top_high.md3");
    m_Top.DefineModelLow ("ship_boss_amemasu_top_low.md3");
    m_Top.DefineVolume   ("ship_boss_amemasu_top_volume.md3");
    m_Top.SetSize        (this->GetSize());
    m_Top.Configure      (1, 0u, COLOR_SHIP_RED);
    m_Top.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_SECRET);
    m_Top.SetParent      (this);

    // 
    m_Bottom.DefineModelHigh("ship_boss_amemasu_bottom_high.md3");
    m_Bottom.DefineModelLow ("ship_boss_amemasu_bottom_low.md3");
    m_Bottom.DefineVolume   ("ship_boss_amemasu_bottom_volume.md3");
    m_Bottom.SetSize        (this->GetSize());
    m_Bottom.Configure      (1, 0u, COLOR_SHIP_RED);
    m_Bottom.AddStatus      (ENEMY_STATUS_INVINCIBLE | ENEMY_STATUS_SECRET);
    m_Bottom.SetParent      (this);

    // 
    m_ChangePath.Reserve(3u);
    m_ChangePath.AddNode(coreVector2(0.0f, 0.0f),  coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f, 0.01f), coreVector2(0.0f, 1.0f));
    m_ChangePath.AddNode(coreVector2(0.0f,-3.0f),  coreVector2(0.0f,-1.0f));
    m_ChangePath.Refine();

    // 
    m_aPackPath[0].Reserve(2u);
    m_aPackPath[0].AddNode(coreVector2(-1.5f,0.0f), coreVector2(1.0f,0.0f));
    m_aPackPath[0].AddNode(coreVector2( 1.5f,0.0f), coreVector2(1.0f,0.0f));
    m_aPackPath[0].Refine();

    m_aPackPath[1].Reserve(2u);
    m_aPackPath[1].AddNode(coreVector2(0.0f, 1.5f), coreVector2(0.0f,-1.0f));
    m_aPackPath[1].AddNode(coreVector2(0.0f,-1.5f), coreVector2(0.0f,-1.0f));
    m_aPackPath[1].Refine();

    m_aPackPath[2].Reserve(4u);
    m_aPackPath[2].AddNode (coreVector2( 1.5f,-0.6f), coreVector2(-1.0f,0.0f));
    m_aPackPath[2].AddNodes(coreVector2(-0.6f,-0.6f), coreVector2(-1.0f,0.0f), coreVector2(0.0f,1.0f));
    m_aPackPath[2].AddNode (coreVector2(-0.6f, 1.5f), coreVector2( 0.0f,1.0f));
    m_aPackPath[2].Refine();

    m_aPackPath[3].Reserve(6u);
    m_aPackPath[3].AddNode (coreVector2(-1.5f, 0.6f), coreVector2( 1.0f, 0.0f));
    m_aPackPath[3].AddNodes(coreVector2( 0.6f, 0.6f), coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
    m_aPackPath[3].AddNodes(coreVector2( 0.6f,-0.6f), coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
    m_aPackPath[3].AddNode (coreVector2(-1.5f,-0.6f), coreVector2(-1.0f, 0.0f));
    m_aPackPath[3].Refine();

    // 
    m_pVacuumSound = Core::Manager::Resource->Get<coreSound>("effect_vacuum.wav");

    // 
    constexpr const coreChar* apcName[] =
    {
        "effect_rain.png",
        "environment_blood_diff.png",
        "environment_clouds_grey.png",
        "environment_clouds_high.png",
        "environment_clouds_mid.png",
        "environment_grave_diff.png",
        "environment_grave_norm.png",
        "environment_moss_diff.png",
        "environment_rock_diff.png",
        "environment_tree_01_diff.png",
        "environment_tree_01_norm.png",
        "environment_tree_02_diff.png",
        "environment_tree_02_norm.png",
        "environment_tree_03_diff.png",
        "environment_tree_03_norm.png",

        "environment_tree_01.md3",
        "environment_tree_02.md3",
        "environment_tree_03.md3",
        "environment_grave.md3",

        "effect_weather_rain_moss_program",
        "environment_clouds_program",
        "environment_clouds_inst_program",
        "environment_rain_program",
        "object_ground_program",
        "object_ground_inst_program"
    };
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_apResCache); ++i)
    {
        m_apResCache[i] = Core::Manager::Resource->Get<coreResourceDummy>(apcName[i]);
        STATIC_ASSERT(ARRAY_SIZE(m_apResCache) == ARRAY_SIZE(apcName))
    }

    STATIC_ASSERT(offsetof(cGemingaBoss, m_InsideTop)    < offsetof(cGemingaBoss, m_Top))   // initialization order for collision detection
    STATIC_ASSERT(offsetof(cGemingaBoss, m_InsideBottom) < offsetof(cGemingaBoss, m_Bottom))
}


// ****************************************************************
// 
void cGemingaBoss::ResurrectIntro()
{
    // 
    m_aiCounter[CONNECTED_MOUTH] = 1;

    // 
    m_fMouthAngle = SIN(0.05f*PI);

    // 
    this->SetPosition(coreVector3(0.0f,0.0f,0.0f));

    // 
    m_InsideTop   .AddStatus(ENEMY_STATUS_GHOST);
    m_InsideBottom.AddStatus(ENEMY_STATUS_GHOST);
    m_Top         .AddStatus(ENEMY_STATUS_GHOST);
    m_Bottom      .AddStatus(ENEMY_STATUS_GHOST);

    // 
    m_bForeshadow = true;

    // 
    m_iPhase = 200u;
    this->Resurrect();
}


// ****************************************************************
// 
void cGemingaBoss::__ResurrectOwn()
{
    if(m_iPhase < 200u)
    {
        // 
        m_aiCounter[CONNECTED_MOUTH] = 1;

        // 
        m_fMouthAngle = 0.0f;

        // 
        this->SetPosition(coreVector3(HIDDEN_POS, 0.0f));

        // 
        cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());
        pBackground->SetEnableLightning(false);

        // 
        this->_ResurrectBoss();
    }
}


// ****************************************************************
// 
void cGemingaBoss::__KillOwn(const coreBool bAnimated)
{
    if(m_iPhase < 200u)
    {
        cMuscusMission* pMission = d_cast<cMuscusMission*>(g_pGame->GetCurMission());

        // 
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
            pMission->DisablePearl(i, bAnimated);

        // 
        pMission->ResetCollEnemyBullet();

        // 
        pMission->GetEnemySquad(0u)->ClearEnemies(bAnimated);
        pMission->GetEnemySquad(1u)->ClearEnemies(bAnimated);

        // 
        m_Dharuk.Kill(bAnimated);

        // 
        if(m_pVacuumSound->EnableRef(this)) m_pVacuumSound->Stop();

        // 
        g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->RemoveStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_ALL);
        });
    }

    // 
    m_InsideTop   .RemoveStatus(ENEMY_STATUS_GHOST);
    m_InsideBottom.RemoveStatus(ENEMY_STATUS_GHOST);
    m_Top         .RemoveStatus(ENEMY_STATUS_GHOST);
    m_Bottom      .RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    m_iPhase = 0u;
}


// ****************************************************************
// 
void cGemingaBoss::__MoveOwn()
{
    cMuscusMission* pMission = d_cast<cMuscusMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    // 
    const cEnemySquad* pSquad1 = (m_iPhase == 200u) ? NULL : g_pGame->GetCurMission()->GetEnemySquad(0u);
    const cEnemySquad* pSquad2 = (m_iPhase == 200u) ? NULL : g_pGame->GetCurMission()->GetEnemySquad(1u);
    ASSERT(!pSquad1 || (pSquad1->GetNumEnemies() == GEMINGA_ENEMIES_TELEPORT))
    ASSERT(!pSquad2 || (pSquad2->GetNumEnemies() == GEMINGA_ENEMIES_LEGION))

    coreFloat fSuck = 0.0f;

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, (1.0f/6.85f), LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                m_Dharuk.Resurrect();
                m_Dharuk.AddStatus(ENEMY_STATUS_WORTHLESS);

                m_InsideTop   .AddStatus(ENEMY_STATUS_GHOST);
                m_InsideBottom.AddStatus(ENEMY_STATUS_GHOST);
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 1u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fLerp = BLENDBR(fTime);

            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.0f), fLerp);

            m_fMouthAngle = SIN(1.0f*PI * STEP(0.5f, 1.0f, fLerp));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pGame->GetInterface()->Reset();
                g_pGame->GetInterface()->SetBossChange(true);

                m_Dharuk.Kill(false);
                m_Dharuk.RemoveStatus(ENEMY_STATUS_WORTHLESS);

                m_InsideTop   .RemoveStatus(ENEMY_STATUS_GHOST);
                m_InsideBottom.RemoveStatus(ENEMY_STATUS_GHOST);

                g_pSpecialEffects->MacroEruptionColorBig(this->GetPosition(), coreVector2(0.0f,-1.0f), COLOR_ENERGY_RED);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 2u)
    {
        PHASE_CONTROL_TIMER(0u, 2.0f, LERP_LINEAR)
        {
            const coreFloat fCancel = 1.0f - fTime;

            g_MusicPlayer.SetPitch(MAX(fCancel, 0.5f));
            g_pGame->SetMusicVolume(STEP(0.0f, 0.5f, fCancel));

            if(PHASE_FINISHED)
                g_MusicPlayer.Pause();
        });

        PHASE_CONTROL_PAUSE(1u, 0.4f)
        {
            PHASE_CHANGE_INC

            g_MusicPlayer.Lock();
            {
                g_MusicPlayer.GetCurMusic()->SeekTime(6.2f);   // guitar: 11.0f
            }
            g_MusicPlayer.Unlock();

            g_MusicPlayer.Play();
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreFloat fLerp = BLENDS(fTime);

            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.75f), fLerp);
            this->DefaultOrientateLerp(0.5f*PI,    2.5f*PI,                 fLerp);

            m_fMouthAngle = SIN(0.1f*PI * MAX0((fLerp - 0.5f) * 2.0f));

            const coreFloat fResume = STEPH3(0.0f, 0.25f, fTime);

            g_MusicPlayer.SetPitch(MAX(fResume, 0.5f));
            g_pGame->SetMusicVolume(STEP(0.0f, 0.5f, fResume));

            if(PHASE_TIME_POINT(0.2f))
            {
                this->_StartBoss();
                g_pGame->GetInterface()->SetBossChange(false);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 4u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.4f)
        {
            PHASE_CHANGE_TO(50u)

            cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());
            pBackground->SetEnableLightning(true);
        });
    }

    // ################################################################
    // transition to inner phase
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK_REV)
        {
            static coreSpline2 aMove  [GAME_PLAYERS];
            static coreFloat   afAngle[GAME_PLAYERS];

            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_TURN);

                    aMove[i].ClearNodes();
                    aMove[i].Reserve(2u);
                    aMove[i].AddNode(pPlayer->GetPosition().xy(), coreVector2(0.0f, 1.0f));
                    aMove[i].AddNode(this   ->GetPosition().xy(), coreVector2(0.0f,-4.0f), 8.0f);
                    aMove[i].Refine();

                    afAngle[i] = pPlayer->GetDirection().xy().Angle();
                });
            }

            m_fMouthAngle = LERPBR(1.0f, 0.0f, STEP(0.85f, 1.0f, fTime));

            const coreFloat fMoveTime = MIN1(fTime / 0.9f);

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vPos = aMove[i].CalcPositionLerp(fMoveTime);
                const coreVector2 vDir = coreVector2::Direction(afAngle[i] + fTime * (4.0f*PI));

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->SetPosition (coreVector3(HIDDEN_POS, 0.0f));
                    pPlayer->SetDirection(coreVector3(0.0f, 1.0f, 0.0f));
                    pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_ALL);
                });

                g_pSpecialEffects->MacroEruptionColorBig(this->GetPosition(), coreVector2(0.0f,1.0f), COLOR_ENERGY_RED);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);

                if(m_pVacuumSound->EnableRef(this))
                {
                    m_pVacuumSound->Stop();
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 11u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            m_InsideTop   .AddStatus(ENEMY_STATUS_GHOST);
            m_InsideBottom.AddStatus(ENEMY_STATUS_GHOST);
            m_Top         .AddStatus(ENEMY_STATUS_GHOST);
            m_Bottom      .AddStatus(ENEMY_STATUS_GHOST);

            g_pEnvironment->ChangeBackground(cStomachBackground::ID, ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(MIN1(BLENDS(fTime) * 1.4f)).y;
            this->SetPosition(coreVector3(0.0f, (m_vLastPosition.y + fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreFloat fLerp = BLENDB(fTime);//BLENDB(CLAMP01((i ? 0.0f : 0.08f) + fTime));
                const coreFloat fSide = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;

                const coreFloat fHeightPlayer = m_ChangePath.CalcPositionLerp(CLAMP01((fLerp * 3.0f - 1.0f) / 2.0f)).y;
                pPlayer->SetPosition(coreVector3(fSide, (-3.75f - fHeightPlayer) * FOREGROUND_AREA.y, 0.0f));

                STATIC_ASSERT(GAME_PLAYERS == 2u)
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(60u)

                g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_ALL);
                });
            }
        });
    }

    // ################################################################
    // transition back to outer phase
    else if(m_iPhase == 20u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_LINEAR)
        {
            static coreFloat afAngle[GAME_PLAYERS];

            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->AddStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_MOVE | PLAYER_STATUS_NO_INPUT_TURN);

                    afAngle[i] = pPlayer->GetDirection().xy().Angle();
                });

                m_fMouthAngle = 0.0f;

                m_InsideTop   .RemoveStatus(ENEMY_STATUS_GHOST);
                m_InsideBottom.RemoveStatus(ENEMY_STATUS_GHOST);
                m_Top         .RemoveStatus(ENEMY_STATUS_GHOST);
                m_Bottom      .RemoveStatus(ENEMY_STATUS_GHOST);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(SPECIAL_RELATIVE, 0.5f, 1.5f, SOUND_EFFECT_SHAKE);

                cStomachBackground* pBackground = d_cast<cStomachBackground*>(g_pEnvironment->GetBackground());
                pBackground->GetHeadlight()->BlendOut();
            }

            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(CLAMP01((BLENDS(fTime) * 3.0f - 1.0f) / 2.0f)).y;
            this->SetPosition(coreVector3(0.0f, (-3.0f - fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
            this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

            if(PHASE_TIME_POINT(0.5f))
            {
                g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
                g_pEnvironment->SetTargetSideNow(coreVector2(0.0f,0.0f));
            }
            

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vMove = coreVector2(0.0f,1.0f) * (TIME * 110.0f);
                const coreVector2 vPos  = pPlayer->GetPosition().xy() + vMove;
                const coreVector2 vDir  = coreVector2::Direction(afAngle[i] + fTime * (14.0f*PI));

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));

                if(vPos.y > FOREGROUND_AREA.y * 1.3f) pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_ALL);
            });

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });

        PHASE_CONTROL_TICKER(1u, 18u, 10.0f, LERP_LINEAR)
        {
            const coreFloat fSide = (iTick % 2u) ? 1.0f : -1.0f;

            const coreVector2 vPos = coreVector2(fSide *  1.2f, Core::Rand->Float(-0.9f, 0.9f)) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2(fSide * -1.0f, 0.0f);

            g_pSpecialEffects->MacroEruptionColorBig(coreVector3(vPos, 0.0f), vDir, COLOR_ENERGY_RED);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 21u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_SHOOT);
                });

                g_pSpecialEffects->MacroEruptionColorBig(this->GetPosition(), coreVector2(0.0f,1.0f), COLOR_ENERGY_RED);
                g_pSpecialEffects->CreateSplashSmoke(this->GetPosition(), 40.0f, 40u, coreVector3(1.0f,1.0f,1.0f));
                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.2f, 1.0f, SOUND_ENEMY_EXPLOSION_02);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }

            this->DefaultMoveLerp(coreVector2(0.0f,0.0f), coreVector2(0.0f,-0.75f), fTime);

            m_fMouthAngle = LERPH3(0.5f - 0.5f * COS((2.0f*PI) * STEP(0.0f, 0.5f, fTime)), SIN(0.1f*PI), fTime);

            const coreVector2 vDir = coreVector2::Direction(fTime * (7.0f*PI));

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreFloat   fSide = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;
                const coreVector2 vPos  = coreVector2(fSide * fTime, -this->GetPosition().y);

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));

                STATIC_ASSERT(GAME_PLAYERS == 2u)
            });

            PHASE_CONTROL_TICKER(1u, 0u, 60.0f * (1.0f - fTime + CORE_MATH_PRECISION), LERP_LINEAR)
            {
                g_pSpecialEffects->CreateBlowSmoke(this->GetPosition(), coreVector3(0.0f,1.0f,0.0f), 50.0f, 3u, coreVector3(1.0f,1.0f,1.0f));
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(40u)

                g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
                {
                    pPlayer->RemoveStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_ALL);
                });
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_TIMER(0u, 2.0f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING) m_avVector[OLD_MOUTH_ANGLE].x = m_fMouthAngle;

            m_fMouthAngle = LERP(m_avVector[OLD_MOUTH_ANGLE].x, 0.0f, fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        PHASE_CONTROL_TIMER(0u, g_pGame->IsEasy() ? 0.5f : 0.7f, LERP_LINEAR)
        {
            const coreBool bExit = (this->GetCurHealth() <= 2100);

            if(PHASE_BEGINNING)
            {
                m_avVector[SMASH_POSITION].xy(bExit ? coreVector2(0.0f,0.0f) : this->NearestPlayerDual(m_aiCounter[SMASH_COUNT] % 2)->GetPosition().xy() / FOREGROUND_AREA);

                if((m_aiCounter[SMASH_COUNT] >= 3) && this->_ResurrectHelper(ELEMENT_GREEN, true))
                {
                    g_pGame->GetHelper(ELEMENT_GREEN)->SetPosition(this->GetPosition());
                }
            }

            const coreVector2 vFrom = m_vLastPosition;
            const coreVector2 vTo   = coreVector2(-0.85f, m_avVector[SMASH_POSITION].y);
            const coreVector2 vPos  = coreVector2(LERP(vFrom.x, vTo.x, SIN(PI * fTime)), LERP(vFrom.y, vTo.y, BLENDS(fTime))) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            const coreMatrix3 mRota = coreMatrix4::RotationAxis((1.0f*PI) * fTime + ((m_aiCounter[SMASH_COUNT] % 2) ? (1.0f*PI) : (0.0f*PI)), coreVector3(1.0f,0.0f,0.0f)).m123();

            m_Top   .SetDirection(coreVector3(0.0f,1.0f,0.0f) * mRota);
            m_Bottom.SetDirection(coreVector3(0.0f,1.0f,0.0f) * mRota.Transposed());

            if(PHASE_FINISHED)
            {
                if(bExit)
                {
                    if(m_avVector[SMASH_POSITION].z)
                    {
                        PHASE_CHANGE_TO(33u)
                    }
                    else
                    {
                        PHASE_CHANGE_INC
                        //PHASE_RESET(0u)
                        //PHASE_AGAIN

                        //this->StorePosition();

                        m_avVector[SMASH_POSITION].z = 1.0f;
                    }
                }
                else
                {
                    PHASE_CHANGE_INC
                }

                const coreVector2 vPos2 = this->GetPosition().xy();

                for(coreUintW i = 0u; i < 13u; ++i)
                {
                    if((i % 2u) == (m_aiCounter[SMASH_COUNT] % 2u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) - 6.0f) * (g_pGame->IsEasy() ? 12.0f : 10.0f)));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos2,  vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos2, -vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, vPos2,  vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.0f, this, vPos2, -vDir)->ChangeSize(1.8f);
                }

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);

                m_aiCounter[SMASH_COUNT] += 1;

                this->_KillHelper(ELEMENT_GREEN, true);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_TO(31u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 33u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 34u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            const coreVector2 vBase = coreVector2::Direction(fTime * (1.5f*PI));
            const coreVector2 vDir  = (m_aiCounter[SMASH_COUNT] % 2) ? -vBase : vBase;

            m_Top   .SetDirection(coreVector3(vDir, 0.0f));
            m_Bottom.SetDirection(coreVector3(vDir, 0.0f));

            m_Top   .SetOrientation(coreVector3(-vBase.Rotated90(), 0.0f));
            m_Bottom.SetOrientation(coreVector3(-vBase.Rotated90(), 0.0f));

            g_pEnvironment->SetTargetDirectionNow(vBase);

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(45u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 40u)
    {
        PHASE_CONTROL_TIMER(0u, 0.75f, LERP_SMOOTH)
        {
            if(PHASE_BEGINNING) m_avVector[OLD_MOUTH_ANGLE].x = m_fMouthAngle;

            this->DefaultMoveLerp     (m_vLastPosition, coreVector2(0.0f,0.8f), fTime);
            this->DefaultOrientateLerp(0.5f*PI,         3.5f*PI,                fTime);

            m_fMouthAngle = LERP(m_avVector[OLD_MOUTH_ANGLE].x, 0.0f, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[CONNECTED_MOUTH] = 0;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_LINEAR)
        {
            if(PHASE_TIME_POINT(0.2f))
            {
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);

                for(coreUintW j = 0u; j < 18u; ++j)
                {
                    const coreVector2 vPos = this->GetPosition().xy() + coreVector2(0.0f, (I_TO_F(j) - 8.5f) * 1.45f);

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 25.0f, 2u, COLOR_ENERGY_WHITE);
                }
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 42u) || (m_iPhase == 45u))
    {
        const coreBool bSecond = (m_iPhase == 45u);

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
            }

            const coreVector2 vTo     = bSecond ? coreVector2(m_vLastPosition.x, 0.85f) : coreVector2(-0.85f, m_vLastPosition.y);
            const coreVector2 vPos    = LERP(m_vLastPosition, vTo, fTime) * FOREGROUND_AREA;
            const coreVector2 vBottom = bSecond ? -vPos : vPos.InvertedX();

            this   ->SetPosition(coreVector3(vPos,    0.0f));
            m_Top   .SetPosition(coreVector3(vPos,    0.0f));
            m_Bottom.SetPosition(coreVector3(vBottom, 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 43u) || (m_iPhase == 46u))
    {
        const coreBool bSecond = (m_iPhase == 46u);

        PHASE_CONTROL_TIMER(0u, bSecond ? 0.15f : 0.3f, LERP_SMOOTH)
        {
            const coreVector2 vBase   = bSecond ? coreVector2(0.0f,1.0f) : coreVector2(1.0f,0.0f);
            const coreFloat   fHeight = bSecond ? (SIN(fTime * (2.0f*PI)) * 0.8f) : LERP(m_vLastPosition.y, -m_vLastPosition.y, fTime);
            const coreVector2 vPos    = (vBase * m_vLastPosition + vBase.yx() * fHeight) * FOREGROUND_AREA;
            const coreVector2 vBottom = bSecond ? -vPos : vPos.InvertedX();

            this   ->SetPosition(coreVector3(vPos,    0.0f));
            m_Top   .SetPosition(coreVector3(vPos,    0.0f));
            m_Bottom.SetPosition(coreVector3(vBottom, 0.0f));

            for(coreUintW i = 0u; i < 7u; ++i)
            {
                if(bSecond ? PHASE_POSITION_POINT(this, 0.76f * (1.0f - (2.0f/6.0f) * I_TO_F(i)) * FOREGROUND_AREA.x, x) :
                             PHASE_POSITION_POINT(this, 0.76f * (1.0f - (2.0f/6.0f) * I_TO_F(i)) * FOREGROUND_AREA.y, y))
                {
                    if(i % 2u) s_vPositionPoint = bSecond ? -s_vPositionPoint : s_vPositionPoint.InvertedX();

                    const coreVector2 vDir = vBase * -s_vPositionPoint.Processed(SIGN);

                    for(coreUintW j = 0u; j < 9u; ++j)
                    {
                        if(g_pGame->IsEasy() && ((j < 2u) || (j >= 7u))) continue;

                        const coreVector2 vPos2 = s_vPositionPoint + vBase * ((j % 2u) ? 1.0f : -1.0f) + vBase.yx() * ((I_TO_F(j) - 4.0f) * 2.9f);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, this, vPos2, vDir)->ChangeSize(1.6f);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos2, 0.0f), 5.0f, 2u, COLOR_ENERGY_ORANGE);
                    }

                    g_pSpecialEffects->PlaySound(coreVector3(s_vPositionPoint, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
                }
            }

            if(bSecond && PHASE_TIME_POINT(0.5f))
            {
                this->_ResurrectHelper(ELEMENT_ORANGE, false);
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 44u) || (m_iPhase == 47u))
    {
        const coreBool bSecond = (m_iPhase == 47u);

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            const coreVector2 vTo     = bSecond ? coreVector2(m_vLastPosition.x, 0.0f) : coreVector2(0.0f, m_vLastPosition.y);
            const coreVector2 vPos    = LERP(m_vLastPosition, vTo, fTime) * FOREGROUND_AREA;
            const coreVector2 vBottom = bSecond ? -vPos : vPos.InvertedX();

            this   ->SetPosition(coreVector3(vPos,    0.0f));
            m_Top   .SetPosition(coreVector3(vPos,    0.0f));
            m_Bottom.SetPosition(coreVector3(vBottom, 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO((m_iPhase == 47u) ? 100u : 30u)

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 50u)
    {
        PHASE_CONTROL_TIMER(0u, 3.0f, LERP_HERMITE3)
        {
            m_fMouthAngle = SIN(LERP(0.1f*PI, 0.0f*PI, fTime));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        static coreVector2 vPrev;
        static coreVector2 vCur;
        static coreFlow    fTurn;

        if(PHASE_BEGINNING2)
        {
            vPrev   = this->GetDirection().xy();   
            vCur    = this->GetDirection().xy();   
            fTurn   = 1.0f;   
        }

        const coreInt32 iLostHealth = this->GetLostHealth();

        const coreBool bExit = (m_avVector[FOLLOW_VALUES].w != 0.0f);

        if(bExit)
        {
            if(m_avVector[FOLLOW_VALUES].z < 1.0f)
            {
                m_avVector[FOLLOW_VALUES].z += 1.0f * TIME;
                m_avVector[FOLLOW_VALUES].x = -1.0f;
            }
            else
            {
                m_avVector[FOLLOW_VALUES].x = 3.0f;
            }
        }

        const coreFloat fMoveSpeed = 1.0f + m_avVector[FOLLOW_VALUES].x;

        this->DefaultMoveForward(this->GetDirection().xy(), 15.0f * fMoveSpeed);

        this->__DefaultOrientation();

        if(bExit)
        {
            if(!g_pForeground->IsVisiblePoint(this->GetPosition().xy(), 1.5f))
            {
                PHASE_CHANGE_TO(70u)
            }
        }

        const coreFloat fOldSnapValue = m_avVector[FOLLOW_VALUES].y;
        m_avVector[FOLLOW_VALUES].y += 1.5f * fMoveSpeed * TIME;

        m_fMouthAngle = 0.5f - 0.5f * COS(1.0f*PI * m_avVector[FOLLOW_VALUES].y);

        if(!bExit)
        {
            if(InBetweenExt(0.5f, FMOD(fOldSnapValue, 2.0f), FMOD(m_avVector[FOLLOW_VALUES].y, 2.0f)) == 1)
            {
                const coreVector2 vPos  = this->GetPosition().xy();
                const coreFloat   fBase = vCur.Angle();

                for(coreUintW j = 0u; j < 6u; ++j)
                {
                    if(g_pGame->IsEasy() && ((j == 2u) || (j == 3u))) continue;

                    const coreVector2 vDir   = coreVector2::Direction(fBase + DEG_TO_RAD((I_TO_F(j) - 2.5f) * 9.0f));
                    const coreFloat   fSpeed = 1.8f - 0.3f * ABS(I_TO_F(j) - 2.5f);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, fSpeed,        this, vPos, vDir)->ChangeSize(1.7f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, fSpeed - 0.1f, this, vPos, vDir)->ChangeSize(1.7f);
                }

                g_pSpecialEffects->CreateBlowColor(coreVector3(vPos, 0.0f), coreVector3(vCur, 0.0f), SPECIAL_BLOW_TINY, COLOR_ENERGY_PURPLE);
                g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
            }

            if(FMOD(fOldSnapValue, 2.0f) > FMOD(m_avVector[FOLLOW_VALUES].y, 2.0f))
            {
                if(iLostHealth >= 1200)
                {
                    m_avVector[FOLLOW_VALUES].w = 1.0f;

                    g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                    g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
                }
            }

            const coreVector2 vAim = this->AimAtPlayerDual((iLostHealth < 600) ? 0u : 1u).Normalized();

            if(coreVector2::Dot(vCur, vAim) <= 0.4f)
            {
                vPrev = vCur;
                vCur  = AlongCrossNormal(vAim);
                fTurn = 0.0f;
            }

            fTurn.UpdateMin(3.0f * fMoveSpeed, 1.0f);

            this->SetDirection(coreVector3(LERPS(vPrev, vCur, fTurn).Normalized(), 0.0f));

            PHASE_CONTROL_TICKER(0u, 0u, 0.2f, LERP_LINEAR)
            {
                cEnemy* pEnemy = pSquad1->GetEnemy(iTick % GEMINGA_ENEMIES_TELEPORT);
                if(pEnemy->HasStatus(ENEMY_STATUS_DEAD))
                {
                    const coreVector3 vPos = this->GetPosition() - this->GetDirection() * 7.0f;

                    pEnemy->Resurrect();
                    pEnemy->SetPosition(vPos);

                    g_pSpecialEffects->MacroExplosionColorSmall(vPos, COLOR_ENERGY_BLUE);
                    g_pSpecialEffects->PlaySound(vPos, 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_04);
                }
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 60u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            m_Dharuk.Resurrect();
            m_Dharuk.ChangePhase(100u);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        coreVector2 vTarget = coreVector2(0.0f,0.0f);
        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            vTarget += pPlayer->GetPosition().xy();
        });

        g_pEnvironment->SetTargetSide(vTarget * (0.1f * BLENDH3(MIN1(m_fPhaseTime * 0.5f)) * RCP(I_TO_F(g_pGame->GetNumPlayers()))), 10.0f);

        if(m_Dharuk.GetHelperEvent())
        {
            this->_ResurrectHelper(ELEMENT_MAGENTA, false);
        }

        if(m_Dharuk.HasStatus(ENEMY_STATUS_DEAD))
        {
            PHASE_CHANGE_TO(20u)
        }
    }

    // ################################################################
    // 
    else if((m_iPhase == 70u) || (m_iPhase == 71u) || (m_iPhase == 72u) || (m_iPhase == 73u))
    {
        const coreUintW iPackIndex = m_iPhase - 70u;
        ASSERT(iPackIndex < 4u)

        if(PHASE_BEGINNING2)
        {
            if(iPackIndex == 0u)
            {
            }
            else if(iPackIndex == 1u)
            {
                constexpr coreUintW iStart = 0u;

                for(coreUintW i = 0u; i < 7u; ++i)
                {
                    const coreVector2 vPos = coreVector2(0.0f, 3.0f - I_TO_F(i)) * 0.3f;

                    pMission->EnablePearl(i + iStart);
                    pMission->GetPearl   (i + iStart)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }
            }
            else if(iPackIndex == 2u)
            {
                constexpr coreUintW iStart = 7u;

                for(coreUintW i = 0u; i < 11u; ++i)
                {
                    const coreVector2 vPos = ((i < 6u) ? coreVector2(3.0f - I_TO_F(i), -2.0f) : coreVector2(-2.0f, I_TO_F(i - 6u) - 1.0f)) * 0.3f;

                    pMission->EnablePearl(i + iStart);
                    pMission->GetPearl   (i + iStart)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }
            }
            else if(iPackIndex == 3u)
            {
                constexpr coreUintW iStart = 20u;

                for(coreUintW i = 0u; i < 15u; ++i)
                {
                    const coreVector2 vPos = ((i < 6u) ? coreVector2(I_TO_F(i) - 3.0f, 2.0f) : ((i < 9u) ? coreVector2(2.0f, 1.0f - I_TO_F(i - 6u)) : coreVector2(2.0f - I_TO_F(i - 9u), -2.0f))) * 0.3f;

                    pMission->EnablePearl(i + iStart);
                    pMission->GetPearl   (i + iStart)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }

                if(this->_ResurrectHelper(ELEMENT_CYAN, true))
                {
                    g_pGame->GetHelper(ELEMENT_CYAN)->SetPosition(coreVector3(3.0f,2.0f,0.0f) * 0.3f * FOREGROUND_AREA3);
                }
            }
        }

        const coreSpline2* pPath = &m_aPackPath[iPackIndex];
        const coreFloat    fTime = MAX0(m_fPhaseTime - 0.8f) * 1.4f;

        this->DefaultMovePath(pPath, coreVector2(1.0f,1.0f), coreVector2(0.0f,0.0f), fTime);

        this->__DefaultOrientation();

        m_fMouthAngle = 0.5f + 0.5f * SIN((4.0f*PI) * fTime);

        if(fTime >= pPath->GetTotalDistance())
        {
            if(iPackIndex == 3u)
            {
                PHASE_CHANGE_TO(90u)
            }
            else
            {
                PHASE_CHANGE_INC
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 80u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_BREAK)
        {
            this->SetDirection(coreVector3(0.0f,1.0f,0.0f));

            this->DefaultMoveLerp     (coreVector2(0.0f,-1.5f), coreVector2(0.0f,-0.5f), fTime);
            this->DefaultOrientateLerp(1.5f*PI,                 3.5f*PI,                 fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        if(PHASE_BEGINNING2)
        {
            this->SetPosition (coreVector3(0.0f,-0.5f,0.0f) * FOREGROUND_AREA3);
            this->SetDirection(coreVector3(0.0f, 1.0f,0.0f));

            for(coreUintW i = 0u; i < GEMINGA_ENEMIES_LEGION; ++i)
            {
                cEnemy* pEnemy = pSquad2->GetEnemy(i);

                pEnemy->Resurrect();

                pEnemy->SetMaxHealth(10);
                pEnemy->SetCurHealth(1);
                pEnemy->RefreshColor();

                pEnemy->DefaultRotate(I_TO_F(i) * GA);
            }
        }

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            m_fMouthAngle = 0.5f - 0.5f * COS(2.0f*PI * fTime);
        });

        STAGE_FOREACH_ENEMY(pSquad2, pEnemy, i)
        {
            STAGE_LIFETIME(pEnemy, 2.0f, 0.0f)

            if(STAGE_TAKEOFF)
            {
                pEnemy->SetPosition(this->GetPosition());
                pEnemy->AddStatus(ENEMY_STATUS_GHOST);
            }

            const coreVector2 vMove = coreVector2(I_TO_F(i) - 4.5f, 7.5f).Normalized();

            if(pEnemy->GetPosition().y < FOREGROUND_AREA.y * 1.05f)
            {
                coreVector2       vNewPos = pEnemy->GetPosition().xy() + vMove * (100.0f * TIME);
                const coreVector2 vNewDir = coreVector2::Direction(pEnemy->GetDirection().xy().Angle() + (10.0f * TIME));

                if((vNewPos.y > FOREGROUND_AREA.y * 1.05f) && (vMove.y > 0.0f)) vNewPos *= FOREGROUND_AREA.y * 1.05f * RCP(vNewPos.y);

                pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
                pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));
            }
        });

        if(PHASE_MAINTIME_AFTER(2.0f))
            PHASE_CHANGE_INC
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        if(PHASE_BEGINNING2)
        {
            m_pVacuumSound->PlayRelative(this, 0.0f, 0.5f, true, SOUND_EFFECT);
        }

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            m_fMouthAngle = fTime;
        });

        const coreVector2 vPoint = this->GetPosition().xy() + coreVector2(0.0f,-7.0f);

        const coreInt32 iLostHealth = this->GetLostHealth();

        fSuck = STEP(3700.0f, 5500.0f, I_TO_F(iLostHealth));

        if(m_pVacuumSound->EnableRef(this))
        {
            m_pVacuumSound->SetVolume(LERP(0.0f, 1.3f, fSuck));
            m_pVacuumSound->SetPitch (LERP(0.5f, 0.8f, fSuck));
        }

        if(iLostHealth < 3700)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.0f * (g_pGame->IsEasy() ? 0.7f : 1.0f), LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < 24u; ++i)
                {
                    if(((i + 2u * (iTick % 3u)) % 8u) < 4u) continue;

                    const coreVector2 vTarget = coreVector2((I_TO_F(i) - 11.5f) / 11.5f * 1.1f, 1.0f);
                    const coreVector2 vRange  = vTarget - vPoint / (FOREGROUND_AREA * 1.2f);

                    const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vRange.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vRange.Length() * 0.6f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else if(iLostHealth < 4300)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 3.0f, LERP_LINEAR)
            {
                for(coreUintW i = 0u, ie = g_pGame->IsEasy() ? 4u : 5u; i < ie; ++i)
                {
                    const coreFloat   fBase   = 1.2f * SQRT2 - (vPoint.y / (FOREGROUND_AREA.y * 1.2f));
                    const coreVector2 vTarget = coreVector2(0.0f, fBase + 0.06f * I_TO_F(i));

                    const coreVector2 vPos   = vPoint + vTarget * FOREGROUND_AREA;
                    const coreVector2 vDir   = coreVector2(0.0f,-1.0f);
                    const coreFloat   fSpeed = fBase * 0.6f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>((iTick % 2u) ? 7 : 6, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else if(iLostHealth < 4900)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 10.0f * (g_pGame->IsEasy() ? 0.5f : 1.0f), LERP_LINEAR)
            {
                if(iTick < 7u) return;

                for(coreUintW i = 0u; i < 1u; ++i)
                {
                    const coreVector2 vTarget = coreVector2((I_TO_F((iTick * 7u) % 24u) - 11.5f) / 11.5f * -1.1f, 1.0f);
                    const coreVector2 vRange  = vTarget - vPoint / (FOREGROUND_AREA * 1.2f);

                    const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vRange.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vRange.Length() * 0.6f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else if(iLostHealth < 5500)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.5f * (g_pGame->IsEasy() ? 0.5f : 1.0f), LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < 24u; ++i)
                {
                    if(((i % 12u) < 6u) == ((iTick % 2u) != 0u)) continue;

                    const coreVector2 vTarget = coreVector2((I_TO_F(i) - 11.5f) / 11.5f * 1.1f, 1.0f);
                    const coreVector2 vRange  = vTarget - vPoint / (FOREGROUND_AREA * 1.2f);

                    const coreVector2 vPos   = vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vRange.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vRange.Length() * (0.4f + 0.05f * ((iTick % 2u) ? (I_TO_F(5u - (i % 6u))) : I_TO_F(i % 6u)));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else
        {
            PHASE_CHANGE_INC
        }

        if(iLostHealth >= 5200)
        {
            this->_ResurrectHelper(ELEMENT_RED, false);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 83u)
    {
        const coreVector2 vPoint = this->GetPosition().xy() + coreVector2(0.0f,-7.0f);

        static coreVector4 avStart[GEMINGA_ENEMIES_LEGION];

        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < GEMINGA_ENEMIES_LEGION; ++i)
            {
                const cEnemy* pEnemy = pSquad2->GetEnemy(i);

                avStart[i] = coreVector4(pEnemy->GetPosition().xy(), pEnemy->GetSize().x, pEnemy->GetDirection().xy().Angle());
            }
        }

        fSuck = 1.0f;

        for(coreUintW i = 0u; i < GEMINGA_ENEMIES_LEGION; ++i)
        {
            cEnemy* pEnemy = pSquad2->GetEnemy(i);

            const coreFloat fTime = BLENDBR(CLAMP01((m_fPhaseTime - I_TO_F((i * 3u) % GEMINGA_ENEMIES_LEGION) * 0.2f) * 1.0f));

            const coreVector2 vNewPos  = LERP(avStart[i].xy(), vPoint, fTime);
            const coreVector3 vNewSize = coreVector3(1.0f,1.0f,1.0f) * (avStart[i].z * (1.0f - STEPH3(0.7f, 1.0f, fTime)));
            const coreVector2 vNewDir  = coreVector2::Direction(avStart[i].w + (4.0f*PI) * fTime);

            pEnemy->SetPosition (coreVector3(vNewPos, 0.0f));
            pEnemy->SetSize     (vNewSize);
            pEnemy->SetDirection(coreVector3(vNewDir, 0.0f));

            if(fTime >= 1.0f) pEnemy->Kill(false);
        }

        if(pSquad2->IsFinished())
        {
            PHASE_CHANGE_TO(10u)
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 90u)
    {
        coreVector2 vForce = m_avVector[BOUNCE_FORCE].xy();

        if(PHASE_BEGINNING2)
        {
            this->SetPosition(coreVector3(-1.4f,0.9f,0.0f) * FOREGROUND_AREA3);

            m_fMouthAngle = 0.0f;

            constexpr coreUintW iStart = 0u;

            for(coreUintW i = 0u; i < 20u; ++i)
            {
                pMission->EnablePearl(i + iStart);
            }

            vForce = coreVector2(g_pGame->IsEasy() ? 0.5f : 1.1f, 0.0f);

            m_Top   .AddStatus(ENEMY_STATUS_DAMAGING);
            m_Bottom.AddStatus(ENEMY_STATUS_DAMAGING);
        }

        for(coreUintW i = 0u; i < 20u; ++i)
        {
            coreObject3D* pPearl = pMission->GetPearl(i);
            if(!pMission->IsPearlActive(i)) continue;

            const coreFloat fTime = MAX0(m_fPhaseTime * (0.4f*PI) - I_TO_F(i) * 0.1f - I_TO_F(i / 2u) * 0.5f);
            const coreFloat fLerp = LERPH3(1.2f, 1.0f, CLAMP01(fTime));

            const coreFloat fOffset = COS(fTime + I_TO_F((i / 2u) % 2u) * (1.0f*PI)) * fLerp;
            const coreFloat fHeight = (I_TO_F(i) - 9.5f) * 0.095f;

            const coreVector2 vPos = coreVector2(fOffset, fHeight) * FOREGROUND_AREA;

            pPearl->SetPosition(coreVector3(vPos, 0.0f));
        }

        vForce.y -= 1.0f * TIME;

        coreVector2 vNewPos = this->GetPosition().xy() + vForce * FOREGROUND_AREA * (1.5f * TIME);

             if((vNewPos.x < -FOREGROUND_AREA.x * 1.0f) && (vForce.x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x * 1.0f); vForce.x =  ABS(vForce.x); g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY); g_pSpecialEffects->PlaySound(coreVector3(vNewPos, 0.0f), 0.5f, 1.5f, SOUND_EFFECT_SHAKE);}
        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.0f) && (vForce.x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x * 1.0f); vForce.x = -ABS(vForce.x); g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY); g_pSpecialEffects->PlaySound(coreVector3(vNewPos, 0.0f), 0.5f, 1.5f, SOUND_EFFECT_SHAKE);}

        if(!pMission->IsPearlValidAny())
        {
            if(vNewPos.y < -FOREGROUND_AREA.y * 1.5f)
            {
                PHASE_CHANGE_INC

                m_Top   .RemoveStatus(ENEMY_STATUS_DAMAGING);
                m_Bottom.RemoveStatus(ENEMY_STATUS_DAMAGING);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(coreVector3(vNewPos, 0.0f), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
            }
        }
        else if((vNewPos.y < -FOREGROUND_AREA.y * 1.0f) && (vForce.y < 0.0f))
        {
            vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.0f); vForce.y = ABS(vForce.y);

            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->PlaySound(coreVector3(vNewPos, 0.0f), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
        }

        this->SetPosition  (coreVector3(vNewPos, 0.0f));
        this->DefaultRotate(m_fPhaseTime * 5.0f);

        this->__DefaultOrientation();

        m_avVector[BOUNCE_FORCE].xy(vForce);

        if(this->GetPosition().x >= 0.0f) m_avVector[BOUNCE_FORCE].z = 1.0f;

        if(m_avVector[BOUNCE_FORCE].z)
        {
            m_avVector[BOUNCE_FORCE].w = MIN1(m_avVector[BOUNCE_FORCE].w + 0.3f * TIME);
            g_pEnvironment->SetTargetDirectionNow(coreVector2(this->GetPosition().x * BLENDBR(m_avVector[BOUNCE_FORCE].w), FOREGROUND_AREA.y * 3.0f).Normalized());
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 91u)
    {
        PHASE_CONTROL_TIMER(0u, 2.0f, LERP_BREAK)
        {
            if(PHASE_BEGINNING)
            {
                m_avVector[BOUNCE_FORCE].w = g_pEnvironment->GetDirection().Angle();
            }

            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(m_avVector[BOUNCE_FORCE].w, 0.0f*PI, fTime)));

            if(PHASE_FINISHED)
                PHASE_CHANGE_TO(80u)
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 100u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC

            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 101u)
    {
        const coreInt32 iCurHealth = this->GetCurHealth();

        m_avVector[FINAL_ROTATION].x += 0.05f * TIME * LERP(0.5f, 2.0f, 1.0f - STEP(0.0f, 1700.0f, I_TO_F(iCurHealth)));

        PHASE_CONTROL_TIMER(0u, 0.05f, LERP_LINEAR)
        {
            const coreVector2 vBase = coreVector2::Direction((4.0f*PI) * m_avVector[FINAL_ROTATION].x);
            const coreFloat   fLen  = (STEPB(0.0f, 0.2f, fTime) - STEPBR(0.8f, 1.0f, fTime)) * FOREGROUND_AREA.x * 0.8f;
            const coreVector2 vPos  = vBase * fLen;

            this   ->SetPosition(coreVector3(vPos,         0.0f));
            m_Top   .SetPosition(coreVector3(vPos,         0.0f));
            m_Bottom.SetPosition(coreVector3(vPos * -1.0f, 0.0f));

            m_Top   .SetOrientation(coreVector3(vBase, 0.0f));
            m_Bottom.SetOrientation(coreVector3(vBase, 0.0f));

            const coreMatrix3 mRota = coreMatrix4::RotationAxis((8.0f*PI) * fTime + (0.0f*PI), coreVector3(vBase, 0.0f)).m123();
            const coreVector2 vDir  = (m_aiCounter[SMASH_COUNT] % 2) ? -vBase : vBase;

            m_Top   .SetDirection(coreVector3(vDir.Rotated90(), 0.0f) * mRota);
            m_Bottom.SetDirection(coreVector3(vDir.Rotated90(), 0.0f) * mRota.Transposed());

            g_pEnvironment->SetTargetDirectionNow(vBase.Rotated90());

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)
                PHASE_AGAIN

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
        {
            if(g_pGame->IsEasy() && ((iTick % 20u) < 10u)) return;

            const coreBool      bOther  = ((iTick % 80u) < 40u);
            const coreObject3D* pSource = bOther ? &m_Bottom : &m_Top;
            const coreFloat     fBase   = (pSource->GetOrientation().xy() * (bOther ? 1.0f : -1.0f)).Angle();

            const coreVector2 vPos = pSource->GetPosition().xy();
            const coreVector2 vDir = coreVector2::Direction(fBase + (0.3f*PI) * ((I_TO_F((iTick * 2u) % 5u) - 2.0f) / 2.0f));

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.7f);

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), 10.0f, 2u, COLOR_ENERGY_YELLOW);
            g_pSpecialEffects->PlaySound(coreVector3(vPos, 0.0f), 1.0f, 1.0f, SOUND_WEAPON_ENEMY);
        });

        if(iCurHealth <= 700)
        {
            this->_ResurrectHelper(ELEMENT_YELLOW, false);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 110u)
    {
        if(PHASE_BEGINNING2)
        {
            m_InsideTop   .AddStatus(ENEMY_STATUS_GHOST);
            m_InsideBottom.AddStatus(ENEMY_STATUS_GHOST);
            m_Top         .AddStatus(ENEMY_STATUS_GHOST);
            m_Bottom      .AddStatus(ENEMY_STATUS_GHOST);

            this->_EndBoss();

            cMossBackground* pBackground = d_cast<cMossBackground*>(g_pEnvironment->GetBackground());
            pBackground->SetEnableLightning(false);

            g_pSpecialEffects->MacroExplosionPhysicalDarkBig(m_Top   .GetPosition());
            g_pSpecialEffects->MacroExplosionPhysicalDarkBig(m_Bottom.GetPosition());
            g_pSpecialEffects->PlaySound(m_Top   .GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);
            g_pSpecialEffects->PlaySound(m_Bottom.GetPosition(), 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_08);

            m_avVector[ENV_ROTATION].x = g_pEnvironment->GetDirection().Angle();
        }

        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(LERP(m_avVector[ENV_ROTATION].x, 0.0f*PI, fTime)));
        });

        PHASE_CONTROL_TIMER(1u, 0.22f, LERP_LINEAR)
        {
            m_Top   .SetPosition(m_Top   .GetPosition() + m_Top   .GetOrientation() * (110.0f * TIME));
            m_Bottom.SetPosition(m_Bottom.GetPosition() - m_Bottom.GetOrientation() * (110.0f * TIME));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 111u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                m_aiCounter[CONNECTED_MOUTH] = 1;
            }

            this->DefaultMoveLerp(coreVector2(0.0f,-2.0f), coreVector2(0.0f,0.6f), fTime);
            this->SetDirection   (coreVector3(0.0f,1.0f,0.0f));

            m_fMouthAngle = 0.5f + 0.5f * COS(fTime * (14.0f*PI));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
                g_pSpecialEffects->PlaySound(this->GetPosition(), 0.6f, 1.3f, SOUND_EFFECT_SHAKE);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 112u)
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[CONNECTED_MOUTH] = 0;
        }

        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            m_avVector[OLD_ANGLE].x = m_Top.GetDirection().xy().Angle();
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 113u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction(LERP(m_avVector[OLD_ANGLE].x, 0.9f*PI, fTime));
            const coreVector2 vPos = this->GetPosition().xy() + (vDir - coreVector2(0.0f,1.0f)) * (m_Top.GetSize().x * 3.5f) + coreVector2(fTime * -7.0f, 0.0f);

            m_Top.SetPosition   (coreVector3(vPos, 0.0f));
            m_Top.SetDirection  (coreVector3(vDir, 0.0f));
            m_Top.SetOrientation(coreVector3(vDir.Rotated90() * -1.0f, 0.0f));

            m_Bottom.SetPosition   (m_Top.GetPosition   ().InvertedX());
            m_Bottom.SetDirection  (m_Top.GetDirection  ().InvertedX());
            m_Bottom.SetOrientation(coreVector3(vDir.InvertedX().Rotated90() * -1.0f, 0.0f));

            if(PHASE_FINISHED)
            {
                m_Top   .Kill(true);
                m_Bottom.Kill(true);

                this->Kill(false);

                const coreVector3 vCenter = this->GetPosition() - coreVector3(0.0f,1.0f,0.0f) * (m_Top.GetSize().x * 7.0f);

                if(this->HasAllHelpers())
                {
                    this->_CreateFragment(7u, vCenter.xy());
                }

                g_pSpecialEffects->CreateExplosion (vCenter);
                g_pSpecialEffects->CreateSplashDark(vCenter, 200.0f, 400u, true);
                g_pSpecialEffects->PlaySound       (vCenter, 1.0f, 1.0f, SOUND_ENEMY_EXPLOSION_11);
                g_pSpecialEffects->SlowScreen(4.0f);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 200u)
    {
    }

    // ################################################################
    // ################################################################

    if(m_aiCounter[CONNECTED_MOUTH])
    {
        const coreFloat fFinalAngle = m_fMouthAngle * (1.0f + 0.08f * SIN(m_fLifeTime * 3.0f));

        // 
        const coreMatrix3 mRota = coreMatrix4::RotationAxis(0.2f*PI * fFinalAngle, coreVector3::Cross(this->GetDirection(), this->GetOrientation()).Normalized()).m123();
        const coreVector3 vDir1 = this->GetDirection() * mRota;
        const coreVector3 vDir2 = this->GetDirection() * mRota.Transposed();
        const coreVector3 vPos1 = this->GetPosition () + (vDir1 - this->GetDirection()) * (m_Top   .GetSize().x * 3.5f);
        const coreVector3 vPos2 = this->GetPosition () + (vDir2 - this->GetDirection()) * (m_Bottom.GetSize().x * 3.5f);

        // 
        m_Top.SetPosition   (vPos1);
        m_Top.SetDirection  (vDir1);
        m_Top.SetOrientation(this->GetOrientation());
        m_Top.SetAlpha      (this->GetAlpha());

        // 
        m_Bottom.SetPosition   (vPos2);
        m_Bottom.SetDirection  (vDir2);
        m_Bottom.SetOrientation(this->GetOrientation());
        m_Bottom.SetAlpha      (this->GetAlpha());
    }

    // 
    m_Sphere.SetPosition   (this->GetPosition   ());
    m_Sphere.SetDirection  (this->GetDirection  ());
    m_Sphere.SetOrientation(this->GetOrientation());

    // 
    m_InsideTop.SetPosition   (m_Top.GetPosition   ());
    m_InsideTop.SetDirection  (m_Top.GetDirection  ());
    m_InsideTop.SetOrientation(m_Top.GetOrientation());

    // 
    m_InsideBottom.SetPosition   (m_Bottom.GetPosition   ());
    m_InsideBottom.SetDirection  (m_Bottom.GetDirection  ());
    m_InsideBottom.SetOrientation(m_Bottom.GetOrientation());

    // 
    const coreBool bClosed = coreMath::IsNear((m_Top.GetPosition() - m_Bottom.GetPosition()).LengthSq(), 0.0f);
    if(bClosed)
    {
        m_InsideTop   .AddStatus(ENEMY_STATUS_INVINCIBLE);
        m_InsideBottom.AddStatus(ENEMY_STATUS_INVINCIBLE);
    }
    else
    {
        m_InsideTop   .RemoveStatus(ENEMY_STATUS_INVINCIBLE);
        m_InsideBottom.RemoveStatus(ENEMY_STATUS_INVINCIBLE);
    }
    if(bClosed && !m_InsideTop.HasStatus(ENEMY_STATUS_GHOST))
    {
        m_Sphere.RemoveStatus(ENEMY_STATUS_GHOST);
    }
    else
    {
        m_Sphere.AddStatus(ENEMY_STATUS_GHOST);
    }

    if(m_iPhase < 200u)
    {
    if(this->GetLostHealth() >= 3700) m_avVector[SUCK_ANGLE].x += (g_pGame->IsEasy() ? 0.7f : 1.0f) * TIME;

    const coreVector2 vBulletDir    = -coreVector2::Direction(SIN(m_avVector[SUCK_ANGLE].x) * (0.2f*PI));
    const coreVector2 vBulletDirRev = vBulletDir.InvertedX();

    const coreVector2 vPoint = this->GetPosition().xy() + coreVector2(0.0f,-7.0f);

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
    {
        const coreVector2 vDiff = vPoint - pBullet->GetPosition().xy();

        switch(pBullet->GetDamage())
        {
        case 5:
            break;

        case 6:
            {
                const coreVector2 vNewDiff = vBulletDir * vDiff.Length();

                pBullet->SetPosition(coreVector3(vPoint - vNewDiff, 0.0f));
                pBullet->SetFlyDir  (vNewDiff.Normalized());
            }
            break;

        case 7:
            {
                const coreVector2 vNewDiff = vBulletDirRev * vDiff.Length();

                pBullet->SetPosition(coreVector3(vPoint - vNewDiff, 0.0f));
                pBullet->SetFlyDir  (vNewDiff.Normalized());
            }
            break;

        default:
            ASSERT(false)
            break;
        }

        if((vDiff.LengthSq() < POW2(2.0f)) || (pBullet->GetFlyTime() >= 10.0f))
            pBullet->Deactivate(true);
    });

    pMission->SetCollEnemyBullet([COLL_THIS](cEnemy* OUTPUT pEnemy, cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if((pEnemy != &m_InsideTop) && (pEnemy != &m_InsideBottom)) return;

        if(pBullet->HasStatus(BULLET_STATUS_REFLECTED))
        {
            pBullet->Deactivate(true);
            pBullet->AddStatus(BULLET_STATUS_GHOST);
        }
        else
        {
            const coreVector3 vTan = coreVector3::Cross(pEnemy->GetDirection(), pEnemy->GetOrientation()).Normalized();
            const coreVector3 vOri = coreVector3::Cross(pEnemy->GetDirection(), vTan);
            const coreFloat   fDot = coreVector3::Dot(coreVector3(pBullet->GetFlyDir(), 0.0f), vOri * ((pEnemy == &m_InsideTop) ? -1.0f : 1.0f));

            if(fDot < CORE_MATH_PRECISION)
            {
                pBullet->Deactivate(true);
                pBullet->AddStatus(BULLET_STATUS_GHOST);
            }
        }
    });

    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        for(coreUintW j = 0u; j < MUSCUS_PEARLS; ++j)
        {
            const coreObject3D* pPearl = pMission->GetPearl(j);
            if(!pMission->IsPearlActive(j)) continue;

            const coreVector2 vDiff = pPlayer->GetPosition().xy() - pPearl->GetPosition().xy();
            if(vDiff.LengthSq() < POW2(4.5f))
            {
                pPlayer->GetScoreTable()->RefreshCooldown();

                pMission->StrikeAttack(j, pPlayer, this);
                g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_WHITE);
                g_pSpecialEffects->PlaySound(pPearl->GetPosition(), 1.0f, pMission->RetrievePearlPitch(), SOUND_EFFECT_PEARL);

                if(!m_aiCounter[BULLET_REFLECT])
                {
                    pMission->GiveBadge(3u, BADGE_ACHIEVEMENT, pPearl->GetPosition());
                }

                if(m_iPhase == 90u)
                {
                    m_aiCounter[STRIKE_COUNT] += 1;
                    if(m_aiCounter[STRIKE_COUNT] >= 10)
                    {
                        this->_ResurrectHelper(ELEMENT_PURPLE, false);
                    }
                }
            }
        }
    });

    if((m_iPhase >= 70u) && (m_iPhase < 80u))
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        {
            const coreObject3D* pPearl = pMission->GetPearl(i);
            if(!pMission->IsPearlActive(i)) continue;

            const coreVector2 vDiff = this->GetPosition().xy() - pPearl->GetPosition().xy();
            if(vDiff.LengthSq() < POW2(4.5f))
            {
                pMission->DisablePearl(i, true);
                g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_YELLOW);
                g_pSpecialEffects->PlaySound(pPearl->GetPosition(), 1.2f, 1.0f, SOUND_EFFECT_DUST);

                if(!g_pGame->IsEasy())
                {
                    const coreVector2 vPos = pPearl->GetPosition().xy();

                    for(coreUintW j = 4u; j--; )
                    {
                        const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 45.0f + 22.5f));

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos,  vDir)->ChangeSize(1.5f);
                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos, -vDir)->ChangeSize(1.5f);
                    }

                    g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
                }
            }
        }
    }

    const coreUint64 iStrikeState = pMission->GetStrikeState();
    if(iStrikeState)
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        {
            if(HAS_BIT(iStrikeState, i))
            {
                this->TakeDamage(50, ELEMENT_NEUTRAL, this->GetPosition().xy(), pMission->GetStrikePlayer(i), true);

                g_pSpecialEffects->PlaySound(this->GetPosition(), 1.0f, 1.2f, SOUND_EFFECT_DUST);
            }
        }
    }

    if(!m_aiCounter[BULLET_REFLECT])
    {
        g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
        {
            if(pBullet->HasStatus(BULLET_STATUS_REFLECTED))
                m_aiCounter[BULLET_REFLECT] = 1;
        });
    }

    if(fSuck)
    {
        m_SuckEffect.CreateParticle(1u, fSuck * 30.0f, [&](coreParticle* OUTPUT pParticle)
        {
            const coreVector2 vPos = coreVector2(Core::Rand->Float(-1.5f, 1.5f), 1.3f) * FOREGROUND_AREA;

            pParticle->SetPositionAbs(coreVector3(vPos, 0.0f),               this->GetPosition() + coreVector3(0.0f,-7.0f,0.0f));
            pParticle->SetScaleAbs   (3.5f,                                  1.0f);
            pParticle->SetAngleRel   (Core::Rand->Float(-PI, PI),            PI);
            pParticle->SetColor4Abs  (coreVector4(COLOR_ENERGY_WHITE, 1.0f), coreVector4(COLOR_ENERGY_WHITE, 0.0f));
            pParticle->SetSpeed      (1.3f * Core::Rand->Float(0.7f, 1.3f));
        });
    }

    STAGE_FOREACH_ENEMY(pSquad1, pEnemy, i)
    {
        STAGE_LIFETIME(pEnemy, 1.0f, 0.0f)

        pEnemy->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.2f * BLENDB(MIN1(fLifeTime)));

        if(fLifeTime < 1.0f)
        {
            pEnemy->AddStatus(ENEMY_STATUS_GHOST_BULLET);
        }
        else
        {
            pEnemy->RemoveStatus(ENEMY_STATUS_GHOST_BULLET);
        }

        if(InBetween(1.0f, fLifeTimeBefore, fLifeTime))
        {
            const coreVector2 vPos = pEnemy->GetPosition().xy();

            const coreFloat fSpeed = g_pGame->IsEasy() ? 0.25f : 0.4f;

            for(coreUintW j = 30u; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) / 30.0f * 7.0f * (0.5f*PI));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, pEnemy, vPos,  vDir)            ->ChangeSize(2.0f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, pEnemy, vPos, -vDir)            ->ChangeSize(2.0f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, pEnemy, vPos,  vDir.Rotated90())->ChangeSize(2.0f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, pEnemy, vPos, -vDir.Rotated90())->ChangeSize(2.0f);
            }

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_BLUE);

            if((i == 2u) && this->_ResurrectHelper(ELEMENT_BLUE, true))
            {
                g_pGame->GetHelper(ELEMENT_BLUE)->SetPosition(pEnemy->GetPosition());
                m_avVector[HELPER_DATA].xy(-AlongCrossXNormal(pEnemy->GetPosition().xy()));
            }
        }

        if(pEnemy->ReachedDeath())
        {
            cPlayer* pPlayer = pEnemy->LastAttacker();
            if(!pPlayer->IsEnemyLook())
            {
                const coreVector3 vOldPos = pPlayer->GetPosition();
                const coreVector3 vNewPos = pEnemy ->GetPosition();

                pPlayer->SetPosition(vNewPos);

                g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    if(pBullet->GetOwner() == pPlayer) pBullet->Deactivate(true);
                });

                const coreVector3 vPos  = vOldPos;
                const coreVector3 vDiff = vNewPos - vPos;
                const coreUintW   iNum  = MAX(F_TO_UI(vDiff.Length() / 1.9f), 2u);

                for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, pPlayer->GetEnergyColor());
            }

            g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
            {
                if(pBullet->GetOwner() == pEnemy) pBullet->Deactivate(true);
            });
        }
    });

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(110u)
    }
    
    // 
    cHelper* pYellowHelper = g_pGame->GetHelper(ELEMENT_YELLOW);
    if(!pYellowHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat   fTime = pYellowHelper->GetLifeTime() * 0.3f;
        const coreFloat   fLen  = LERP(1.3f, 1.0f, SIN(fTime * (1.0f*PI))) * FOREGROUND_AREA.x;
        const coreVector2 vDir  = coreVector2::Direction(fTime * (-2.0f*PI) + (1.0f*PI));
        const coreVector2 vPos  = vDir * fLen;

        pYellowHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_YELLOW, false);
    }

    // 
    cHelper* pOrangeHelper = g_pGame->GetHelper(ELEMENT_ORANGE);
    if(!pOrangeHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pOrangeHelper->GetLifeTime() * 0.5f;

        pOrangeHelper->SetPosition(coreVector3(0.0f, LERP(-1.3f, 1.3f, fTime), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_ORANGE, false);
    }

    // 
    cHelper* pRedHelper = g_pGame->GetHelper(ELEMENT_RED);
    if(!pRedHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pRedHelper->GetLifeTime() * 0.3f;

        pRedHelper->SetPosition(coreVector3(LERPS(-1.3f, 1.3f, fTime), -0.98f, 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_RED, false);
    }

    // 
    cHelper* pMagentaHelper = g_pGame->GetHelper(ELEMENT_MAGENTA);
    if(!pMagentaHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        pMagentaHelper->SetPosition(-m_Dharuk.GetPosition());

        if((pMagentaHelper->GetLifeTime() > 1.0f) && !g_pForeground->IsVisiblePoint(pMagentaHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_MAGENTA, false);
        }
    }

    // 
    cHelper* pPurpleHelper = g_pGame->GetHelper(ELEMENT_PURPLE);
    if(!pPurpleHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreFloat fTime = pPurpleHelper->GetLifeTime() * 0.4f;

        pPurpleHelper->SetPosition(coreVector3(0.0f, -1.3f + 0.5f * SIN(fTime * (1.0f*PI)), 0.0f) * FOREGROUND_AREA3);

        if(fTime >= 1.0f) this->_KillHelper(ELEMENT_PURPLE, false);
    }

    // 
    cHelper* pBlueHelper = g_pGame->GetHelper(ELEMENT_BLUE);
    if(!pBlueHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        const coreVector2 vDir = m_avVector[HELPER_DATA].xy();
        const coreVector2 vPos = pBlueHelper->GetPosition().xy() + vDir * (0.4f * BULLET_SPEED_FACTOR * TIME);

        pBlueHelper->SetPosition(coreVector3(vPos, 0.0f));

        if(!g_pForeground->IsVisiblePoint(pBlueHelper->GetPosition().xy(), 1.3f))
        {
            this->_KillHelper(ELEMENT_BLUE, false);
        }
    }

    // 
    cHelper* pCyanHelper = g_pGame->GetHelper(ELEMENT_CYAN);
    if(!pCyanHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        if(pCyanHelper->GetLifeTime() >= 3.0f)
        {
            this->_KillHelper(ELEMENT_CYAN, true);
        }
    }

    // 
    cHelper* pGreenHelper = g_pGame->GetHelper(ELEMENT_GREEN);
    if(!pGreenHelper->HasStatus(HELPER_STATUS_DEAD))
    {
        // handled in phase code
    }
    }
}