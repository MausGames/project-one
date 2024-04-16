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
// TODO 1: MAIN: task-check, fragment, easy, hard idea, coop, regular score, extra score, medal goal, juiciness (move, rota, muzzle, effects), intro, outro, foreshadow, overdrive, sound, attack size/count/speed, enemy/boss size, object size, background rota/speed
// TODO 1: death: jaw opens like Pacmans death (make sure geometry does not collide), and sides will move away from each other (like a spring when backs touch) and will explode
// TODO 1: death: rotation wird langsamer, dann fetzen zusammen, bei einschlag starke drehung die langsamer wird, bei stillstand -> pacman death
// TODO 1: foreshadow: other boss, flies in the background
// TODO 1: player bullets shot into mouth should be on the same visual height
// TODO 1: hard: teleporter on the side
// TODO 1: 1 helfer kommt aus stampfer raus
// TODO 1: duplicate needs to light up when hitting the player
// TODO 1: beim auskotzen, spieler movement im magen und einflug von boss vielleicht separieren, damit environment transition logischer stattfinden kann (so wie beim verschlucken)
// TODO 1: lebenspunkte des inneren bosses müssen sichtbar werden (entweder zweiter bar, der separat gesteuert werden kann, oder ausblenden boss life 1, einblenden boss life 2 (mit hoch-animieren), oder shared life, anpassen der health-grenzen und anwenden auf geminga)
// TODO 1: dharuk sollte sich in boomerang-phase nach links und rechts bewegen
// TODO 1: make sure suck-in and spit-out animations are correctly handled for coop (e.g. spit-out position is currently identical)
// TODO 1: vielleicht sollte drehende welle bei corpse-phase nicht so breit sein, schaut vielleicht komisch aus, wenn so viele geschosse von stellen kommen wo keine leichen sind (aber rand vom mund muss attackiert werden, damit man nicht exploiten kann)
// TODO 1: improve handling of resource-loading between changing environments (stomach)
// TODO 1: flip bullets werden derzeit nicht verwendet
// TODO 1: transition aus erster phase heraus sollte knackiger sein, e.g. er bleibt kurz stehn (fade) und beschleunigt dann auf pacman (erst mund, dann move)
// TODO 1: sollten pearl patterns so sein, dass man leichter alle einsammeln kann (ende von einem in nähe des anfangs vom nächsten -> richtung vom zweiten invertieren)
// TODO 1: visuelle wind effekt in einsaug-phase (während der ganzen)
// TODO 1: vielleicht duplicate entfernen und stattdessen kleine bumeränge von nur 1 seite mit bewegendem dharuk haben (er sammelt sie wieder ein und schießt sie dann nach unten)
// TODO 1: vielleicht duplicate 90 grad versetzt fliegen lassen, oder 0.5f time fliegen lassen (abwechselnd)
// TODO 1: vor der ersten aufteilung sollte ein leichter ruck im boss sein (direkt nach dem charge), damit das plötzliche aufteilen der hälften hervorgehoben wird
// TODO 1: verwandlung des spielers fühlt sich komisch an, vielleicht komplett entfernen, und teleport 100% nur für den hard mode (auch rückverwandlung in ater entfernen)
// TODO 1: das finale des bosses braucht mehr kraft und geschwindigkeit (aber vielleicht reicht es schon aus wenn die death-animation kraft hat)
// TODO 1: 1 infinity-transition nach erster phase bevor pearl kommen (quasie eine pearl-transition ohne pearls) (von rechts nach links wenn andere bewegungen gleich bleiben, ansonsten links nach rechts)
// TODO 1: besseres auskotzen: theme: explosion von dharuk bringt geminga dazu dich wieder auszukotzen
// TODO 1: boomy sound-effect for closing jaws on start and after player-suck-in


// ****************************************************************
// counter identifier
#define CONNECTED_MOUTH (0u)
#define SMASH_COUNT     (1u)


// ****************************************************************
// vector identifier
#define OLD_MOUTH_ANGLE (0u)
#define SMASH_POSITION  (1u)
#define FOLLOW_VALUES   (2u)
#define BOUNCE_FORCE    (3u)
#define FINAL_ROTATION  (4u)


// ****************************************************************
// constructor
cGemingaBoss::cGemingaBoss()noexcept
: m_fMouthAngle (0.0f)
{
    // load models
    this->DefineModelHigh(Core::Manager::Object->GetLowQuad());
    this->DefineModelLow (Core::Manager::Object->GetLowQuad());

    // set object properties
    this->SetSize       (coreVector3( 1.0f,1.0f,1.0f) * 3.0f);
    this->SetOrientation(coreVector3(-1.0f,0.0f,0.0f));

    // configure the boss
    this->Configure(8500, 0u, COLOR_SHIP_RED);
    this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

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
    m_aPackPath[0].AddNode(coreVector2(0.0f, 1.5f), coreVector2(0.0f,-1.0f));
    m_aPackPath[0].AddNode(coreVector2(0.0f,-1.5f), coreVector2(0.0f,-1.0f));
    m_aPackPath[0].Refine();

    m_aPackPath[1].Reserve(4u);
    m_aPackPath[1].AddNode (coreVector2(-0.6f, 1.5f), coreVector2(0.0f,-1.0f));
    m_aPackPath[1].AddNodes(coreVector2(-0.6f,-0.6f), coreVector2(0.0f,-1.0f), coreVector2(1.0f,0.0f));
    m_aPackPath[1].AddNode (coreVector2( 1.5f,-0.6f), coreVector2(1.0f, 0.0f));
    m_aPackPath[1].Refine();

    m_aPackPath[2].Reserve(6u);
    m_aPackPath[2].AddNode (coreVector2(-1.5f, 0.6f), coreVector2( 1.0f, 0.0f));
    m_aPackPath[2].AddNodes(coreVector2( 0.6f, 0.6f), coreVector2( 1.0f, 0.0f), coreVector2( 0.0f,-1.0f));
    m_aPackPath[2].AddNodes(coreVector2( 0.6f,-0.6f), coreVector2( 0.0f,-1.0f), coreVector2(-1.0f, 0.0f));
    m_aPackPath[2].AddNode (coreVector2(-1.5f,-0.6f), coreVector2(-1.0f, 0.0f));
    m_aPackPath[2].Refine();

    // 
    constexpr const coreChar* apcTex[] =
    {
        "effect_rain.png",
        "environment_blood_diff.png",
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
        "environment_tree_03_norm.png"
    };
    for(coreUintW i = 0u; i < ARRAY_SIZE(m_apStomachTex); ++i)
    {
        m_apStomachTex[i] = Core::Manager::Resource->Get<coreTexture>(apcTex[i]);
        STATIC_ASSERT(ARRAY_SIZE(m_apStomachTex) == ARRAY_SIZE(apcTex))
    }

    STATIC_ASSERT(offsetof(cGemingaBoss, m_InsideTop)    < offsetof(cGemingaBoss, m_Top))   // initialization order for collision detection
    STATIC_ASSERT(offsetof(cGemingaBoss, m_InsideBottom) < offsetof(cGemingaBoss, m_Bottom))
}


// ****************************************************************
// 
void cGemingaBoss::__ResurrectOwn()
{
    // 
    m_aiCounter[CONNECTED_MOUTH] = 1;

    // 
    this->_ResurrectBoss();
}


// ****************************************************************
// 
void cGemingaBoss::__KillOwn(const coreBool bAnimated)
{
    cMuscusMission* pMission = d_cast<cMuscusMission*>(g_pGame->GetCurMission());

    // 
    for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        pMission->DisablePearl(i, bAnimated);

    // 
    pMission->GetEnemySquad(0u)->ClearEnemies(bAnimated);
    pMission->GetEnemySquad(1u)->ClearEnemies(bAnimated);

    // 
    m_Dharuk.Kill(bAnimated);

    // 
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->RemoveStatus(PLAYER_STATUS_GHOST | PLAYER_STATUS_NO_INPUT_ALL);
    });

    // 
    if(g_pEnvironment->GetBackground()->GetID() == cStomachBackground::ID)
    {
        // TODO 1: wird am ende nicht eh alles verdunkelt ?
        //g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
    }
}


// ****************************************************************
// 
void cGemingaBoss::__MoveOwn()
{
    cMuscusMission* pMission = d_cast<cMuscusMission*>(g_pGame->GetCurMission());

    // 
    this->_UpdateBoss();

    if(this->ReachedDeath())
    {
        g_pSpecialEffects->MacroDestructionDark(&m_Top);
        g_pSpecialEffects->MacroDestructionDark(&m_Bottom);
        this->Kill(false);   
        this->_EndBoss();
    }

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);
    const cEnemySquad* pSquad2 = g_pGame->GetCurMission()->GetEnemySquad(1u);
    ASSERT(pSquad1->GetNumEnemies() == GEMINGA_ENEMIES_TELEPORT)
    ASSERT(pSquad2->GetNumEnemies() == GEMINGA_ENEMIES_LEGION)

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        PHASE_CONTROL_TIMER(0u, (1.0f/6.85f), LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                m_Dharuk.Resurrect();
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
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,2.0f), coreVector2(0.0f,0.0f), fTime);

            m_fMouthAngle = SIN(1.0f*PI * MAX0((fTime - 0.5f) * 2.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_Dharuk.Kill(false);

                g_MusicPlayer.Pause();
            }
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC

            g_MusicPlayer.Play();
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 3u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.75f), fTime);
            this->DefaultOrientateLerp(0.5f*PI,    2.5f*PI,                 fTime);

            m_fMouthAngle = SIN(0.1f*PI * MAX0((fTime - 0.5f) * 2.0f));

            if(PHASE_TIME_POINT(0.2f))
                this->_StartBoss();

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
                    aMove[i].AddNode(this   ->GetPosition().xy(), coreVector2(0.0f,-1.0f), 8.0f);
                    aMove[i].Refine();

                    afAngle[i] = pPlayer->GetDirection().xy().Angle();
                });
            }

            m_fMouthAngle = LERPBR(1.0f, 0.0f, MAX0(fTime - 0.85f) / 0.15f);

            const coreFloat fMoveTime = MIN(fTime / 0.9f, 1.0f);

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
                    pPlayer->AddStatus   (PLAYER_STATUS_NO_INPUT_SHOOT);
                });
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
            g_pEnvironment->SetTargetSpeed(2.0f, 1.0f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 12u)
    {
        PHASE_CONTROL_TIMER(0u, 0.4f, LERP_SMOOTH)
        {
            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(MIN1(fTime * 1.4f)).y;
            this->SetPosition(coreVector3(0.0f, (m_vLastPosition.y + fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreFloat fSide = g_pGame->IsMulti() ? (20.0f * (I_TO_F(i) - 0.5f * I_TO_F(GAME_PLAYERS - 1u))) : 0.0f;

                const coreFloat fHeightPlayer = m_ChangePath.CalcPositionLerp(CLAMP01((fTime * 3.0f - 1.0f) / 2.0f)).y;
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
            }

            const coreFloat fHeightBoss = m_ChangePath.CalcPositionLerp(CLAMP01((BLENDS(fTime) * 3.0f - 1.0f) / 2.0f)).y;
            this->SetPosition(coreVector3(0.0f, (-3.0f - fHeightBoss) * FOREGROUND_AREA.y, 0.0f));

            this->SetDirection  (coreVector3(0.0f,1.0f,0.0f));
            this->SetOrientation(coreVector3(1.0f,0.0f,0.0f));

            if(PHASE_TIME_POINT(0.5f))
            {
                g_pEnvironment->ChangeBackground(g_pEnvironment->GetLastID(), ENVIRONMENT_MIX_CURTAIN, 1.0f, coreVector2(1.0f,0.0f));
            }

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vMove = coreVector2(-pPlayer->GetPosition().x / FOREGROUND_AREA.x, -1.0f) * (TIME * 90.0f);
                const coreVector2 vPos  = pPlayer->GetPosition().xy() + vMove;
                const coreVector2 vDir  = coreVector2::Direction(afAngle[i] + fTime * (12.0f*PI));

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));

                if(vPos.y > FOREGROUND_AREA.y * 1.3f) pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_SHOOT);
            });

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
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
                    pPlayer->TurnIntoEnemy();
                });
            }

            this->DefaultMoveLerp(coreVector2(0.0f,0.0f), coreVector2(0.0f,-0.75f), fTime);

            m_fMouthAngle = LERPH3(0.5f - 0.5f * COS((2.0f*PI) * STEP(0.0f, 0.5f, fTime)), SIN(0.1f*PI), fTime);

            const coreVector2 vDir = coreVector2::Direction(fTime * (7.0f*PI));

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vPos = this->GetPosition().xy().InvertedY();

                pPlayer->SetPosition (coreVector3(vPos, 0.0f));
                pPlayer->SetDirection(coreVector3(vDir, 0.0f));
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
        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_LINEAR)
        {
            const coreBool bExit = (this->GetLostHealth() >= 6400);

            if(PHASE_BEGINNING) m_avVector[SMASH_POSITION].xy(bExit ? coreVector2(0.0f,0.0f) : this->NearestPlayerDual(m_aiCounter[SMASH_COUNT] % 2)->GetPosition().xy() / FOREGROUND_AREA);

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
                        PHASE_RESET(0u)
                        PHASE_AGAIN

                        this->StorePosition();

                        m_avVector[SMASH_POSITION].z = 1.0f;
                    }
                }
                else
                {
                    PHASE_CHANGE_INC
                }

                const coreVector2 vPos2 = this->GetPosition().xy();

                for(coreUintW i = 0u; i < 9u; ++i)
                {
                    if((i % 2u) == (m_aiCounter[SMASH_COUNT] % 2u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(i) - 4.0f) * 13.0f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos2,  vDir)->ChangeSize(1.8f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.2f, this, vPos2, -vDir)->ChangeSize(1.8f);
                }

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_GREEN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                m_aiCounter[SMASH_COUNT] += 1;
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
            PHASE_CHANGE_TO(44u)
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
    else if((m_iPhase == 41u) || (m_iPhase == 44u))
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, -0.85f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 42u) || (m_iPhase == 45u))
    {
        const coreBool bSecond = (m_iPhase == 45u);

        PHASE_CONTROL_TIMER(0u, bSecond ? 0.15f : 0.3f, LERP_SMOOTH)
        {
            const coreFloat   fHeight = bSecond ? (SIN(fTime * (2.0f*PI)) * 0.8f) : LERP(m_vLastPosition.y, -m_vLastPosition.y, fTime);
            const coreVector2 vPos    = coreVector2(m_vLastPosition.x, fHeight) * FOREGROUND_AREA;
            const coreVector2 vBottom = bSecond ? -vPos : vPos.InvertedX();

            this   ->SetPosition(coreVector3(vPos,    0.0f));
            m_Top   .SetPosition(coreVector3(vPos,    0.0f));
            m_Bottom.SetPosition(coreVector3(vBottom, 0.0f));

            for(coreUintW i = 0u; i < 7u; ++i)
            {
                if(PHASE_POSITION_POINT(this, 0.76f * (1.0f - (2.0f/6.0f) * I_TO_F(i)) * FOREGROUND_AREA.y, y))
                {
                    if(i % 2u) s_vPositionPoint = bSecond ? -s_vPositionPoint : s_vPositionPoint.InvertedX();

                    const coreVector2 vDir = coreVector2(-SIGN(s_vPositionPoint.x), 0.0f);

                    for(coreUintW j = 0u; j < 9u; ++j)
                    {
                        const coreVector2 vPos2 = s_vPositionPoint + coreVector2((j % 2u) ? 1.0f : -1.0f, (I_TO_F(j) - 4.0f) * 2.9f);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.3f, this, vPos2, vDir)->ChangeSize(1.5f);

                        g_pSpecialEffects->CreateSplashColor(coreVector3(vPos2, 0.0f), 5.0f, 1u, COLOR_ENERGY_ORANGE);
                    }
                }
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 43u) || (m_iPhase == 46u))
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            const coreVector2 vPos = coreVector2(LERP(m_vLastPosition.x, 0.0f, fTime), m_vLastPosition.y) * FOREGROUND_AREA;

            this   ->SetPosition(coreVector3(vPos,             0.0f));
            m_Top   .SetPosition(coreVector3(vPos,             0.0f));
            m_Bottom.SetPosition(coreVector3(vPos.InvertedX(), 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO((m_iPhase == 46u) ? 100u : 30u)
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
        static coreUintW   iFollow;
        static coreVector2 vPrev;
        static coreVector2 vCur;
        static coreFlow    fTurn;

        if(PHASE_BEGINNING2)
        {
            iFollow = 0u;   
            vPrev   = this->GetDirection().xy();   
            vCur    = this->GetDirection().xy();   
            fTurn   = 1.0f;   
        }

        const coreBool bExit = (this->GetLostHealth() >= 1000);

        if(bExit)
        {
            if(!m_avVector[FOLLOW_VALUES].x) m_avVector[FOLLOW_VALUES].x = 0.5f;
            m_avVector[FOLLOW_VALUES].x += 0.5f * TIME;
        }

        const coreFloat fMoveSpeed = 1.0f + m_avVector[FOLLOW_VALUES].x;

        this->DefaultMoveForward(this->GetDirection().xy(), 15.0f * fMoveSpeed);

        if(bExit)
        {
            if(!g_pForeground->IsVisiblePoint(this->GetPosition().xy(), 1.5f))
            {
                PHASE_CHANGE_TO(70u)
            }
        }

        const coreFloat fOldSnapValue = m_avVector[FOLLOW_VALUES].y;
        m_avVector[FOLLOW_VALUES].y += 1.5f * fMoveSpeed * TIME;

        if(InBetweenExt(0.5f, FMOD(fOldSnapValue, 2.0f), FMOD(m_avVector[FOLLOW_VALUES].y, 2.0f)) == 1)
        {
            const coreVector2 vPos  = this->GetPosition().xy();
            const coreFloat   fBase = vCur.Angle();

            for(coreUintW j = 0u; j < 6u; ++j)
            {
                const coreVector2 vDir   = coreVector2::Direction(fBase + DEG_TO_RAD((I_TO_F(j) - 2.5f) * 8.0f));
                const coreFloat   fSpeed = 1.8f - 0.3f * ABS(I_TO_F(j) - 2.5f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(2.0f);
            }
        }

        m_fMouthAngle = 0.5f - 0.5f * COS(1.0f*PI * m_avVector[FOLLOW_VALUES].y);

        if(!bExit)
        {
            const coreVector2 vAim = this->AimAtPlayerDual(iFollow).Normalized();

            if(coreVector2::Dot(vCur, vAim) <= 0.4f)
            {
                vPrev = vCur;
                vCur  = AlongCrossNormal(vAim);
                fTurn = 0.0f;
            }
        }

        fTurn.UpdateMin(3.0f * fMoveSpeed, 1.0f);

        this->SetDirection(coreVector3(LERPS(vPrev, vCur, fTurn).Normalized(), 0.0f));

        this->__DefaultOrientation();
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
        if(m_Dharuk.HasStatus(ENEMY_STATUS_DEAD))
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 62u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_TO(20u)

            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_BIG);
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 70u) || (m_iPhase == 71u) || (m_iPhase == 72u))
    {
        const coreUintW iPackIndex = m_iPhase - 70u;
        ASSERT(iPackIndex < 3u)

        if(PHASE_BEGINNING2)
        {
            if(iPackIndex == 0u)
            {
                constexpr coreUintW iStart = 0u;

                for(coreUintW i = 0u; i < 7u; ++i)
                {
                    const coreVector2 vPos = coreVector2(0.0f, 3.0f - I_TO_F(i)) * 0.3f;

                    pMission->EnablePearl(i + iStart);
                    pMission->GetPearl   (i + iStart)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }
            }
            else if(iPackIndex == 1u)
            {
                constexpr coreUintW iStart = 7u;

                for(coreUintW i = 0u; i < 11u; ++i)
                {
                    const coreVector2 vPos = ((i < 6u) ? coreVector2(-2.0f, 3.0f - I_TO_F(i)) : coreVector2(I_TO_F(i - 6u) - 1.0f, -2.0f)) * 0.3f;

                    pMission->EnablePearl(i + iStart);
                    pMission->GetPearl   (i + iStart)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                }
            }
            else if(iPackIndex == 2u)
            {
                constexpr coreUintW iStart = 20u;

                for(coreUintW i = 0u; i < 15u; ++i)
                {
                    const coreVector2 vPos = ((i < 6u) ? coreVector2(I_TO_F(i) - 3.0f, 2.0f) : ((i < 9u) ? coreVector2(2.0f, 1.0f - I_TO_F(i - 6u)) : coreVector2(2.0f - I_TO_F(i - 9u), -2.0f))) * 0.3f;

                    pMission->EnablePearl(i + iStart);
                    pMission->GetPearl   (i + iStart)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
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
            if(iPackIndex == 2u)
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
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            m_fMouthAngle = fTime;
        });

        const coreVector2 vPoint = this->GetPosition().xy() + coreVector2(0.0f,-7.0f);

        const coreInt32 iLostHealth = this->GetLostHealth();

        if(iLostHealth < 3500)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
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
        else if(iLostHealth < 4100)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
            {
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
        else if(iLostHealth < 4700)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.5f, LERP_LINEAR)
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
        else if(iLostHealth < 5300)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < 36u; ++i)
                {
                    if(((i % 6u) < 3u) == (((iTick / 2u) % 2u) != 0u)) continue;

                    const coreFloat   fLen    = (coreVector2(1.2f,1.2f) - (vPoint / (FOREGROUND_AREA * 1.2f))).Length();
                    const coreVector2 vTarget = coreVector2::Direction((I_TO_F(i) - 17.5f) * 0.05f + ((iTick % 2u) ? -0.5f : 0.5f)) * fLen;

                    const coreVector2 vPos   = vPoint + vTarget * FOREGROUND_AREA * 1.2f;
                    const coreVector2 vDir   = vTarget.Normalized() * -1.0f;
                    const coreFloat   fSpeed = vTarget.Length() * 0.6f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>((iTick % 2u) ? 7 : 6, fSpeed, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }
        else
        {
            PHASE_CHANGE_INC
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

            vForce = coreVector2(1.1f,0.0f);

            m_Top   .AddStatus(ENEMY_STATUS_DAMAGING);
            m_Bottom.AddStatus(ENEMY_STATUS_DAMAGING);
        }

        for(coreUintW i = 0u; i < 20u; ++i)
        {
            coreObject3D* pPearl = pMission->GetPearl(i);
            if(!pMission->IsPearlActive(i)) continue;

            const coreFloat fTime = MAX0(m_fPhaseTime * (0.4f*PI) - I_TO_F(i) * 0.1f);
            const coreFloat fLerp = LERPH3(1.2f, 1.0f, CLAMP01(fTime));

            const coreFloat fOffset = COS(fTime + I_TO_F((i / 2u) % 2u) * (1.0f*PI)) * fLerp;
            const coreFloat fHeight = (I_TO_F(i) - 9.5f) * 0.105f;

            const coreVector2 vPos = coreVector2(fOffset, fHeight) * FOREGROUND_AREA;

            pPearl->SetPosition(coreVector3(vPos, 0.0f));
        }

        vForce.y -= 1.0f * TIME;

        coreVector2 vNewPos = this->GetPosition().xy() + vForce * FOREGROUND_AREA * (1.5f * TIME);

             if((vNewPos.x < -FOREGROUND_AREA.x * 1.0f) && (vForce.x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + FOREGROUND_AREA.x * 1.0f); vForce.x =  ABS(vForce.x);}
        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.0f) && (vForce.x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - FOREGROUND_AREA.x * 1.0f); vForce.x = -ABS(vForce.x);}

        if(!pMission->IsPearlValidAny())
        {
            if(vNewPos.y < -FOREGROUND_AREA.y * 1.5f)
            {
                PHASE_CHANGE_INC

                m_Top   .RemoveStatus(ENEMY_STATUS_DAMAGING);
                m_Bottom.RemoveStatus(ENEMY_STATUS_DAMAGING);
            }
        }
        else if((vNewPos.y < -FOREGROUND_AREA.y * 1.0f) && (vForce.y < 0.0f))
        {
            vNewPos.y -= 2.0f * (vNewPos.y + FOREGROUND_AREA.y * 1.0f); vForce.y = ABS(vForce.y);
        }

        this->SetPosition  (coreVector3(vNewPos, 0.0f));
        this->DefaultRotate(m_fPhaseTime * 5.0f);

        this->__DefaultOrientation();

        m_avVector[BOUNCE_FORCE].xy(vForce);
    }

    // ################################################################
    // 
    else if(m_iPhase == 91u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
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
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 101u)
    {
        m_avVector[FINAL_ROTATION].x += 0.05f * TIME * LERP(0.5f, 2.0f, 1.0f - STEP(0.0f, 0.2f, this->GetCurHealthPct()));

        PHASE_CONTROL_TIMER(0u, 0.05f, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction((4.0f*PI) * m_avVector[FINAL_ROTATION].x + (0.5f*PI));
            const coreFloat   fLen = (STEPB(0.0f, 0.2f, fTime) - STEPBR(0.8f, 1.0f, fTime)) * FOREGROUND_AREA.x * 0.8f;
            const coreVector2 vPos = vDir * fLen;

            this   ->SetPosition(coreVector3(vPos,         0.0f));
            m_Top   .SetPosition(coreVector3(vPos,         0.0f));
            m_Bottom.SetPosition(coreVector3(vPos * -1.0f, 0.0f));

            m_Top   .SetOrientation(coreVector3(vDir, 0.0f));
            m_Bottom.SetOrientation(coreVector3(vDir, 0.0f));

            const coreMatrix3 mRota = coreMatrix4::RotationAxis((8.0f*PI) * fTime + ((m_aiCounter[SMASH_COUNT] % 2) ? (1.0f*PI) : (0.0f*PI)), coreVector3(vDir, 0.0f)).m123();

            m_Top   .SetDirection(coreVector3(vDir.Rotated90(), 0.0f) * mRota);
            m_Bottom.SetDirection(coreVector3(vDir.Rotated90(), 0.0f) * mRota.Transposed());

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)
                PHASE_AGAIN
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
        {
            const coreBool      bOther  = ((iTick % 80u) < 40u);
            const coreObject3D* pSource = bOther ? &m_Bottom : &m_Top;
            const coreFloat     fBase   = (pSource->GetOrientation().xy() * (bOther ? 1.0f : -1.0f)).Angle();

            const coreVector2 vPos = pSource->GetPosition().xy();
            const coreVector2 vDir = coreVector2::Direction(fBase + (0.3f*PI) * ((I_TO_F((iTick * 2u) % 5u) - 2.0f) / 2.0f));

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.7f);
        });
    }

    // ################################################################
    // ################################################################

    const coreMatrix2 mBulletRota    = coreMatrix3::Rotation(0.2f * TIME).m12();
    const coreMatrix2 mBulletRotaRev = mBulletRota.Transposed();

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
                const coreVector2 vNewDiff = vDiff * mBulletRota;

                pBullet->SetPosition(coreVector3(vPoint - vNewDiff, 0.0f));
                pBullet->SetFlyDir  (vNewDiff.Normalized());
            }
            break;

        case 7:
            {
                const coreVector2 vNewDiff = vDiff * mBulletRotaRev;

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

        // 
        m_Bottom.SetPosition   (vPos2);
        m_Bottom.SetDirection  (vDir2);
        m_Bottom.SetOrientation(this->GetOrientation());
    }

    // 
    m_InsideTop.SetPosition   (m_Top.GetPosition   ());
    m_InsideTop.SetDirection  (m_Top.GetDirection  ());
    m_InsideTop.SetOrientation(m_Top.GetOrientation());

    // 
    m_InsideBottom.SetPosition   (m_Bottom.GetPosition   ());
    m_InsideBottom.SetDirection  (m_Bottom.GetDirection  ());
    m_InsideBottom.SetOrientation(m_Bottom.GetOrientation());

    // 
    if(coreMath::IsNear((m_Top.GetPosition() - m_Bottom.GetPosition()).LengthSq(), 0.0f))
    {
        m_InsideTop   .AddStatus(ENEMY_STATUS_INVINCIBLE);
        m_InsideBottom.AddStatus(ENEMY_STATUS_INVINCIBLE);
    }
    else
    {
        m_InsideTop   .RemoveStatus(ENEMY_STATUS_INVINCIBLE);
        m_InsideBottom.RemoveStatus(ENEMY_STATUS_INVINCIBLE);
    }



    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        for(coreUintW j = 0u; j < MUSCUS_PEARLS; ++j)
        {
            const coreObject3D* pPearl = pMission->GetPearl(j);
            if(!pMission->IsPearlActive(j)) continue;

            const coreVector2 vDiff = pPlayer->GetPosition().xy() - pPearl->GetPosition().xy();
            if(vDiff.LengthSq() < POW2(4.5f))
            {
                pMission->StrikeAttack(j, pPlayer, this);
                g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_WHITE);
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

                const coreVector2 vPos = pPearl->GetPosition().xy();

                for(coreUintW j = 4u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 45.0f + 22.5f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos,  vDir)->ChangeSize(1.4f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.7f, this, vPos, -vDir)->ChangeSize(1.4f);
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
                const coreUint8 iDamage = 50u / GAME_PLAYERS * g_pGame->GetNumPlayers();

                this->TakeDamage(iDamage, ELEMENT_NEUTRAL, this->GetPosition().xy(), pMission->GetStrikePlayer(i));
            }
        }
    }
}