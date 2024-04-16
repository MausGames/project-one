///////////////////////////////////////////////////////
//*-------------------------------------------------*//
//| Part of Project One (https://www.maus-games.at) |//
//*-------------------------------------------------*//
//| Copyright (c) 2010 Martin Mauersics             |//
//| Released under the zlib License                 |//
//*-------------------------------------------------*//
///////////////////////////////////////////////////////
#include "main.h"

// - yellow:  phase 3+A, phase 1+A
// - orange:  transition 2, transition 1, phase 2
// - red:     phase 3, phase 2
// - magenta: phase 1+A, phase 1+A, phase 2
// - violett: transition, phase, phase, phase, phase
// - blue:    phase 1, phase 3, phase 1
// - cyan:    phase 2, phase 3
// - green:   phase 2, transition 1, phase 2
// - white:   transition, transition, transition, transition, transition
// - intro:   phase

// boss should use absolutely all mechanics from the game (so far), a color for every mission and its mechanics, with new combinations and twists, this makes the fight extremely long (>12 min), but it is possible to scale by reducing the required number of colors the player has to beat
// TODO 1: fragmente fliegen am ende so zusammen wie die helfer, bilden das tafel-muster, muss aber erst letztes fragment einsammeln, man kann sich entscheiden zwischen letztem fragment oder P1 töten
// TODO 1: final endboss, ship jumps into z dimension, while side blocks move away, star fox (either looping or dive animation), rotation around z
// TODO 1: white flash on impact of white helper ?
// TODO 1: different phases should map to the general feeling and behavior of the same-colored bosses ?
// TODO 1: MAIN: task-check, fragment, easy, hard idea, coop, regular score, extra score, medal goal, juiciness (move, rota, muzzle, effects), intro, outro, foreshadow, overdrive, sound, attack size/count/speed, enemy/boss size, object size, background rota/speed
// TODO 1: hard mode: add another mechanic to each sub-stage
// TODO 1: alle objekte aller missionen müssen mit animation disabled werden, auch post-processing, auch spieler status und speed (eigene funktion ?)
// TODO 1: next mission needs to be requested as early as possible (when helper starts flying into center)
// TODO 1: make sure mechanics are properly separated with DEPTH_PUSH, and ordered based on priority
// TODO 1: rota angriff bei schilden und bei purple phase sind recht ähnlich (noch immer ?)
// TODO 1: how to handle death on boss ? maybe keep finished helpers disabled and skip intro !
// TODO 1: dying during green phase u.a. and restarting boss caused the time bonus to appear
// TODO 1: in easy fliegen die restlichen helfer nach außen weg bevor weiß kommt
// TODO 1: wenn man die reihenfolge gleich mit den level-farben macht passiert etwas
// TODO 1: remove turf from base classes
// TODO 1: helper impact wave verbessern, vielleicht gehen steine auch leicht nach unten vor und nach welle (in schwingung)
// TODO 1: hard: alle 8 colors, spieler muss in der letzten phase doch angreifen
// TODO 1: bullet vorkommen sollten möglichst gleichmäßig sein
// TODO 1: soll der boss selbst auch bunt werden ?

// yellow:
// spikes sollten so lang aktiv sein, dass es für den spieler wichtig ist seine position dem auf-und-ab der spikes anzupassen
// rotation der circle-ghosts ist gegen die rotation der moles
// spikes sollten nicht parallel zur zweiten ghost gruppe fliegen
// spikes sollten oben anfangen und langsam beschleunigen
// letting the last ghost spawn the boss adds another dimension to player-positioning based on the spikes
// TODO 1: zweite wizzrobe gruppe is zu leicht, wird zu leicht unabsichtlich getroffen, nachdem der vorherige gegner besiegt wurde
// TODO 1: (badge: activate alle stings)
// TODO 1: add small effekt when ghost enemy gets hit
// TODO 1: yellow sollte mit mole anfangen (nach pause) statt mit child-attack ?

// orange:
// placing way-blocks between orbs felt too clunky and not fluid enough
// placing player on orb-line and move orb-line with shooting felt forced and made attacking the boss very unsatisfying
// orb should spawn from a place where player is not shooting to, otherwise it will be hidden (so boss leaves on right, and orb spawns from left)
// way-block direction should generally be the same as their spawning-location, to make it more comprehensible
// in way-block phase, "falscher" block sollte nach richtigen (selbe seite) kommen und max 90 grad unterschied haben
// in way-block phase, 4er block kann nur mit ner pause davor kommen, is sonst too much
// in fang phase, animationen schauen linear und ohne gleichzeitige bewegung am besten aus, wie ein schiebe-puzzle (obwohl nicht das gleiche)
// bullets in letzter phase sollten nicht zu dicht sein, durch die reduzierung des spiel-felds wird es eh immer schwerer
// TODO 1: reduce speed if getting hit on both ways and fangs
// TODO 1: back-and-forth fang movement can be too confusing, especially with the high speed at the end (maybe clamp speed)

// red:
// second corpse pattern can start later, to give player time to understand first swap-teleportation
// using a cone-light to show generates was too easy, as player can just spin around all the time, so it made sense to switch to a short point-light
// boss sollte in letzter phase nicht auch noch selbst angreifen (auch wenns komisch aussieht), weil es einfach too much ist, fokus sollte auf gute seiten-patterns sein
// TODO 1: vielleicht sollte labyrinth mit anderer linie starten, die keine durchgängige horizontale pearl linie hat
// TODO 1: should it really move all the time or snap into 8x8 ?
// TODO 1: vielleicht sollten bei teleportation an beiden seiten kleine effekte entstehen, in mission explodieren die gegner ja auch
// TODO 1: gegner muss auf pearl impact reagieren (?)

// magenta:
// tried to split the screen again instead of rotating, and however I tried (different factors, movement, various axis), everything just sucks, the teleportation-aspect is just too unpredictable and watching two points is just too much
// bullets between phases need to be removed, because of gravity-mechanic creating inconsistent transitions (gravity phase starts with bullets not affected or being affected and overwhelming player, and ends with bullets not affected anymore)
// direction plates sollten nur blau sein, weil die rote platte sich mit dem magenta background und roten gravity waves zu sehr vermischt
// similar to Messier boss, meteors should not reflect bullets
// TODO 1: check if start-position "into" gravity-phase is safe, if player is directly in the way, maybe start-position should be opposit of current aim-direction (should be even safe for coop)
// TODO 1: first bullets in gravity phase have weird flip in center, maybe delay first shots
// TODO 1: badge: alle meteoriten abschießen/entfärben
// TODO 1: aiming sollte bei tod nicht mehr weiterlaufen

// purple
// all bullet types need to be present in some way
// phases: attack from everywhere, from top with slower sub, from right with faster sub, still linear attack, moving circular attack
// first wave (survival wave) should use the same movement pattern as the very first enemy group (though slight differences are allowed, but pattern should be identical)
// view-bullets should come from the side (side-scroller), to differentiate stronger from the wave-bullets (vert-scroller)
// quad bullet also need some very slow ones which stay long in the middle, to give additional obstacles when flying around boss (long enough)
// triangle bullets need to be dense enough to make it hard to evade them across half of the screen
// TODO 1: noch irgendwas (einmaliges) mit den flip bullets in purple phase machen (alle bullets müssen vorkommen!)
// TODO 1: transition weg von wave-bullet phase is fishy (vielleicht kommt er auch von links, und geht dann nach rechts)
// TODO 1: sollte boss in seiten-phase wirklich ganz am rand kleben, spieler sollte eh nicht durch-kommen, und wenn doch machen ihn die view-bullets fertig, und wnen nicht isser eh ein guter spieler und hat sichs verdient

// blue:
// geschoss-wände in erster phase fungieren als tore durch die der spieler im richtigen zeitpunkt hindurch muss
// schnee sollte nicht in finale phase mitgenommen werden, weil er unter dem violettem bullet-teppich sehr schwer zu erkennen ist, und ausweichmanöver (unerwartet) behindert
// bullet-teppich mit unterschiedlichen richtungen gleichzeitig (oben nach rechts, unten nach links) war extrem verwirrend zu navigieren, man hat löcher verpasst und konnte dann nicht mehr ausweichen
// TODO 1: morgenstern kette muss reißen und stein fliegt einfach raus aus bildschirm
// TODO 1: schauen ob blaues background pattern mit einem getauscht werden soll, weil sich der blaue background dreht
// TODO 1: der push-back in blauer phase braucht ne gescheite visuelle indikation (wind ? bei allen (mission, zeroth))
// TODO 1: schnee sollte interpoliert erzeugt werden
// TODO 1: push is noch sehr fishy, vielleicht statt voller rotation etwas mehr quantisieren, 8-richtungen ? bewegung des bosses gehört auch angepasst
// TODO 1: boss-angriff in snow-phase is etwas zu schwach/langweilig, und startet zu spät
// TODO 1: boss bewegung in morningstar phase anpassen, schneller? auch Y?
// TODO 1: vielleicht sollte blaue kugel schneller werden, die drehung des spielers braucht mehr impact
// TODO 1: mehr als 1 kugel in snow phase
// TODO 1: background wurde bei einem test nicht wieder nach vorne ausgerichtet
// TODO 1: bei speed-phase, spieler frei bewegen lasen (am rand entlang) und background passt sich an

// cyan:
// cyan residue kann nicht verwendet werden, weil es mit dem cyan background verschwimmt
// bomben sollten abwechselnd gerade und schräg liegen, auch auf selber höhe
// zeit zwischen sturmflug bei platten-phase gibt dem spieler zeit zu denken und sich zu bewegen, und der schnelle flug vom gegner fühlt sich intensiver an
// platten-phase sollte mit muster starten, weil es sonst zu leicht ist
// boss in platten-phase sollte nur entlang einer achse fliegen, um seine platten-interaktion berechenbarer zu machen
// the last phase feels like swimming in water
// die drehung des bosses und des pfeils sind wie eine uhr, um die richtung berechenbarer zu machen
// grow bullets sollten die mitte nicht berühren, der boss sollte immer angreifbar sein (auch wenn nur knapp)
// TODO 1: der grüne arrow mit den roten bullets is ein problem ? pfeil größer machen, form dominiert
// TODO 1: sollte schussfrequenz (zm. von residue) mit health erhöht werden?
// TODO 1: vielleicht doch auch horizontal fliegen (mittlere 2)

// green:
// geschwindigkeit vom violettem laser muss sehr langsam anfangen und erst spät schnell werden um diese phase in zwei sub-phasen mit weichem übergang zu teilen (fly free, get dragged)
// bei hoher laser geschwindigkeit muss der spieler durch bewegung entlang des lasers versuchen den geschossen auszuweichen
// both lasers having different speeds sounds interesting, but it negatively affects attacking the boss, making it very annoying
// moving bullets during transition feel much more dynamic than frozen bullets, especially combined with the movement-interaction
// boss should only charge from a single direction, otherwise player cannot plan his evasive maneuvers (which are hard in this phase), and can become very confused when boss suddenly appears from somewhere else than expected
// bullets in schild-phase sollten eher den spieler am movement hindern, weswegen sie langsam und dicht sind
// spieler kann an den schild-ecken in alle richtungen fliegen, aber das is in ordnung (weil logisch), besonders solange die geschosse normal blockiert werden
// started first with free moving shields, and 45 degree shields, but the fixed labyrinth felt much more interesting in combination with the jump mechanic
// 4x4 labyrinth was a bit too much, and the distance between player and boss for attacks was too short, even with a lower bullet density than now
// TODO 1: blend in shields, blend in lasers (if not already happening, but then improve)
// TODO 1: würden zusätzliche blaue pfeile (wie bei arrow, vielleicht anderer shader) die schild-phase verbessern ?

// white:
// boss should end with the final bullet-wave, upcoming events should not count to the time-bonus, and should not be disturbed with the time-bonus popup
// background should not be too bright, otherwise bullets become harder to see
// TODO 1: check if bullet-speed changes are correct for any framerate (FRAMERATE_MIN?)
// TODO 1: offset und größe der regenbeogen geschosse noch etwas angleichen
// TODO 1: einzelne bullet phasen allgemein polishen + dokumentieren
// TODO 1: spieler sollte nicht mehr schießen können (blitze), nur langsam werden, während regenbogen-phase (aber wieder möglich am ende)
// TODO 1: weißer helfer sieht noch etwas zu faad aus, vielleicht sollte er scheinen oder partikel erzeugen (alle helfer!)
// TODO 1: background etwas zu hell, man erkennt geschosse schlecht (vor allem gelb und magenta)

// intro:
// soll nur zum aufwärmen sein, nicht zu schwer und nicht zu lang
// boss sollte permanent schießen
// TODO 1: vielleicht sollte boss nicht weiß sein in dieser phase, grün mit gelben geschossen ? und dann komplett weiß beim intro-ende
// TODO 1: ist eine einzige phase ohne steigerung genug ? vielleicht mit was anderem alternieren 1-2-1-2


// ****************************************************************
// counter identifier
#define SELECTED_HELPER (0u)
#define CHARGE_COUNT    (1u)
#define JUMP_COUNT      (2u)
#define GHOST_ACTIVE    (3u)
#define RUSH_COUNT      (4u)
#define EVADE_COUNT     (5u)
#define TELEPORT_COUNT  (6u)
#define FANG_DATA       (7u)
#define PATH_COUNT      (8u)
#define SIDE_COUNT      (9u)
#define MOLE_COUNT      (10u)


// ****************************************************************
// vector identifier
#define HELPER_DATA  (0u)
#define LASER_ROTA   (1u)
#define CHARGE_SPEED (2u)
#define JUMP_TARGET  (3u)
#define SHOOT_TARGET (4u)
#define SPIKE_TIME   (5u)
#define RUTILUS_ROTA (6u)
#define CALOR_ROTA   (7u)
#define MOVE_VALUE   (8u)


// ****************************************************************
// constructor
cProjectOneBoss::cProjectOneBoss()noexcept
: m_vOldDir          (coreVector2(0.0f,-1.0f))
, m_fArrowValue      (0.0f)
, m_iHelperState     (0u)
, m_vLevelColor      (coreVector3(1.0f,1.0f,1.0f))
, m_fAnimation       (0.0f)
, m_fWaveValue       (1.0f)
, m_vColorFrom       (DARK_COLOR_DEFAULT)
, m_vColorTo         (DARK_COLOR_DEFAULT)
, m_fPatternValue    (0.0f)
, m_fPatternStrength (0.0f)
, m_iPatternType     (0u)
, m_fFinalLerp       (0.0f)
, m_bDead            (false)
, m_fFangTime        (0.0f)
, m_fPlateTime       (0.0f)
, m_iMeteorDir       (0u)
, m_fGameAngle       (0.0f)
, m_fPushForce       (0.0f)
, m_vPushDir         (coreVector2(1.0f,-1.0f).Normalized())
{
    // load object resources
    this->DefineModelHigh("ship_projectone_high.md3");
    this->DefineModelLow ("ship_projectone_low.md3");
    this->DefineVolume   ("ship_projectone_volume.md3");
    this->DefineTexture  (0u, "ship_player.png");

    // set object properties
    this->SetSize(coreVector3(1.0f,1.0f,1.0f) * 1.5f);
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * PROJECTONE_COLL_SCALE);

    // configure the boss
    this->Configure(1, 0u, COLOR_SHIP_GREY);
    this->AddStatus(ENEMY_STATUS_SECRET);

    // 
    constexpr coreVector3 avColor[] =
    {
        COLOR_ENERGY_YELLOW,
        COLOR_ENERGY_ORANGE,
        COLOR_ENERGY_RED,
        COLOR_ENERGY_MAGENTA,
        COLOR_ENERGY_PURPLE,
        COLOR_ENERGY_BLUE,
        COLOR_ENERGY_CYAN,
        COLOR_ENERGY_GREEN
    };
    for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
    {
        m_aShield[i].DefineModelHigh("object_sphere.md3");
        m_aShield[i].DefineModelLow ("object_sphere.md3");
        m_aShield[i].DefineTexture  (0u, "effect_energy.png");
        m_aShield[i].DefineProgram  ("effect_energy_blink_flat_spheric_program");
        m_aShield[i].SetTexSize     (coreVector2(1.0f,1.0f) * 4.0f);
        m_aShield[i].Configure      (1000, 0u, avColor[i]);
        m_aShield[i].AddStatus      (ENEMY_STATUS_ENERGY | ENEMY_STATUS_BOTTOM | ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_FLAT);
    }

    // 
    for(coreUintW i = 0u; i < PROJECTONE_CLONES; ++i)
    {
        m_aClone[i].DefineModelHigh     (this->GetModelHigh());
        m_aClone[i].DefineModelLow      (this->GetModelLow ());
        m_aClone[i].DefineTexture       (0u, this->GetTexture(0u));
        m_aClone[i].SetSize             (this->GetSize());
        m_aClone[i].SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 5.0f);
        m_aClone[i].Configure           (1, 0u, COLOR_SHIP_GREY);
        m_aClone[i].AddStatus           (ENEMY_STATUS_IMMORTAL | ENEMY_STATUS_GHOST | ENEMY_STATUS_WORTHLESS);
    }

    // 
    m_Range.DefineModel  ("object_penta_top.md3");
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
    m_Wind.SetDirection (coreVector3(0.0f,-1.0f,0.0f));
    m_Wind.SetAlpha     (0.0f);
    m_Wind.SetTexSize   (coreVector2(1.0f,5.0f));
    m_Wind.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Bubble.DefineModel  ("object_penta_top.md3");
    m_Bubble.DefineTexture(0u, "effect_energy.png");
    m_Bubble.DefineProgram("effect_energy_flat_spheric_program");
    m_Bubble.SetColor4    (coreVector4(COLOR_ENERGY_WHITE * 0.6f, 0.0f));
    m_Bubble.SetTexSize   (coreVector2(0.5f,0.5f));
    m_Bubble.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_Exhaust.DefineModel  ("object_tube_open.md3");
    m_Exhaust.DefineTexture(0u, "effect_energy.png");
    m_Exhaust.DefineProgram("effect_energy_direct_program");
    m_Exhaust.SetDirection (this->GetDirection());
    m_Exhaust.SetAlpha     (0.7f);
    m_Exhaust.SetTexSize   (coreVector2(0.5f,0.25f));
    m_Exhaust.SetEnabled   (CORE_OBJECT_ENABLE_NOTHING);

    // 
    m_HelperPath.Reserve(3u);
    m_HelperPath.AddNode(coreVector2(0.0f,0.0f), coreVector2( 0.0f, 1.0f));
    m_HelperPath.AddNode(coreVector2(0.5f,0.5f), coreVector2( 1.0f,-1.0f).Normalized());
    m_HelperPath.AddNode(coreVector2(0.0f,0.0f), coreVector2(-1.0f, 0.0f), 2.5f);
    m_HelperPath.Refine();

    // 
    m_aPurplePath[0].Reserve(2u);
    m_aPurplePath[0].AddNode(coreVector2(-0.4f, 1.3f), coreVector2(0.0f,-1.0f));
    m_aPurplePath[0].AddNode(coreVector2( 0.4f,-1.3f), coreVector2(0.0f,-1.0f));
    m_aPurplePath[0].Refine();

    // 
    m_aPurplePath[1].Reserve(2u);
    m_aPurplePath[1].AddNode(coreVector2(-1.3f,0.8f), coreVector2(1.0f,0.0f));
    m_aPurplePath[1].AddNode(coreVector2( 1.3f,0.0f), coreVector2(1.0f,0.0f));
    m_aPurplePath[1].Refine();
}


// ****************************************************************
// 
void cProjectOneBoss::CalcColor(const coreUintW iIndex, coreVector3* OUTPUT pvEnergyColor, coreVector3* OUTPUT pvBlockColor, coreVector3* OUTPUT pvLevelColor)
{
    ASSERT(pvEnergyColor && pvBlockColor && pvLevelColor)

    switch(iIndex)
    {
    default: ASSERT(false)
    case 0u: (*pvEnergyColor) = COLOR_PLAYER_YELLOW;  (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_YELLOW,  1.0f); (*pvLevelColor) = COLOR_MENU_YELLOW;  break;
    case 1u: (*pvEnergyColor) = COLOR_PLAYER_ORANGE;  (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_ORANGE,  1.0f); (*pvLevelColor) = COLOR_MENU_ORANGE;  break;
    case 2u: (*pvEnergyColor) = COLOR_PLAYER_RED;     (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_RED,     0.9f); (*pvLevelColor) = COLOR_MENU_RED;     break;
    case 3u: (*pvEnergyColor) = COLOR_PLAYER_MAGENTA; (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_MAGENTA, 0.9f); (*pvLevelColor) = COLOR_MENU_MAGENTA; break;
    case 4u: (*pvEnergyColor) = COLOR_PLAYER_PURPLE;  (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_PURPLE,  1.0f); (*pvLevelColor) = COLOR_MENU_PURPLE;  break;
    case 5u: (*pvEnergyColor) = COLOR_PLAYER_BLUE;    (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_BLUE,    1.0f); (*pvLevelColor) = COLOR_MENU_BLUE;    break;
    case 6u: (*pvEnergyColor) = COLOR_PLAYER_CYAN;    (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_CYAN,    1.0f); (*pvLevelColor) = COLOR_MENU_CYAN;    break;
    case 7u: (*pvEnergyColor) = COLOR_PLAYER_GREEN;   (*pvBlockColor) = LERP(coreVector3(1.0f,1.0f,1.0f), COLOR_MENU_GREEN,   1.0f); (*pvLevelColor) = COLOR_MENU_GREEN;   break;
    case 8u: (*pvEnergyColor) = COLOR_PLAYER_WHITE;   (*pvBlockColor) = DARK_COLOR_DEFAULT;                                          (*pvLevelColor) = COLOR_MENU_WHITE;   break;
    case 9u: (*pvEnergyColor) = COLOR_PLAYER_WHITE;   (*pvBlockColor) = DARK_COLOR_DEFAULT;                                          (*pvLevelColor) = COLOR_MENU_WHITE;   break;
    }
}


// ****************************************************************
// 
void cProjectOneBoss::CalcColorLerp(const coreFloat fValue, coreVector3* OUTPUT pvEnergyColor, coreVector3* OUTPUT pvBlockColor, coreVector3* OUTPUT pvLevelColor)
{
    ASSERT(fValue >= 0.0f)

    const coreUintW iFrom = (F_TO_UI(fValue))      % 8u;
    const coreUintW iTo   = (F_TO_UI(fValue) + 1u) % 8u;

    coreVector3 vEnergyColor1, vBlockColor1, vLevelColor1;
    cProjectOneBoss::CalcColor(iFrom, &vEnergyColor1, &vBlockColor1, &vLevelColor1);

    coreVector3 vEnergyColor2, vBlockColor2, vLevelColor2;
    cProjectOneBoss::CalcColor(iTo, &vEnergyColor2, &vBlockColor2, &vLevelColor2);

    (*pvEnergyColor) = LERP(vEnergyColor1, vEnergyColor2, FRACT(fValue));
    (*pvBlockColor)  = LERP(vBlockColor1,  vBlockColor2,  FRACT(fValue));
    (*pvLevelColor)  = LERP(vLevelColor1,  vLevelColor2,  FRACT(fValue));
}


// ****************************************************************
// 
void cProjectOneBoss::__ResurrectOwn()
{
    // 
    this->__SwitchHealth(9u);
    this->__SwitchColor (9u, false);

    // 
    this->_ResurrectBoss();
}


// ****************************************************************
// 
void cProjectOneBoss::__KillOwn(const coreBool bAnimated)
{
    // 
    for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        m_aShield[i].Kill(bAnimated);

    // 
    for(coreUintW i = 0u; i < PROJECTONE_CLONES; ++i)
        m_aClone[i].Kill(bAnimated);

    // 
    this->__EndFeeling();

    // 
    this->__DisableRange();
    this->__DisableArrow();
    this->__DisableWind();
    this->__DisableBubble();
    this->__UpdateExhaust(0.0f);

    // 
    m_vOldDir     = coreVector2(0.0f,-1.0f);
    m_fArrowValue = 0.0f;

    // 
    this->__EndMission(bAnimated, false);
}


// ****************************************************************
// 
void cProjectOneBoss::__RenderOwnUnder()
{
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_HIDDEN))
    {
        DEPTH_PUSH

        // 
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Bubble);

        // 
        m_Bubble .Render();
        m_Exhaust.Render();
        m_Wind   .Render();
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__RenderOwnTop()
{
    if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_HIDDEN))
    {
        DEPTH_PUSH

        // 
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Range);
        g_pOutline->GetStyle(OUTLINE_STYLE_FLAT_FULL)->ApplyObject(&m_Arrow);

        // 
        m_Arrow.Render();   // # swapped
        m_Range.Render();
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveOwn()
{
    cDarkBackground* pBackground = d_cast<cDarkBackground*>(g_pEnvironment->GetBackground());

    // 
    this->_UpdateBoss();

    // 
    m_fAnimation.UpdateMod(1.0f, 20.0f);

    // ################################################################
    // 
    if(m_iPhase == 0u)
    {
        if(PHASE_BEGINNING2)
        {
            g_pEnvironment->SetTargetSpeed(1.0f, 0.2f);

            this->AddStatus(ENEMY_STATUS_GHOST);
        }

        for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        {
            cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);

            if(PHASE_BEGINNING2) pHelper->Resurrect(false);

            const coreFloat fTime = CLAMP01((m_fPhaseTime - I_TO_F(i) * 0.2f) * 0.7f);

            coreFloat fSide;
            switch(i % 4u)
            {
            default: ASSERT(false)
            case 0u: fSide = -0.35f; break;
            case 1u: fSide =  0.75f; break;
            case 2u: fSide = -0.75f; break;
            case 3u: fSide =  0.35f; break;
            }

            const coreVector2 vPos = coreVector2(fSide * 0.9f * LERPH3(1.0f, 0.8f, fTime), LERPH3(-1.2f, 1.3f, fTime)) * FOREGROUND_AREA;

            pHelper->SetPosition(coreVector3(vPos, 0.0f));

            if((i == PROJECTONE_SHIELDS - 1u) && (fTime >= 1.0f))
                PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 1u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
                g_pGame->GetHelper(ELEMENT_YELLOW + i)->Kill(false);

            this->RemoveStatus(ENEMY_STATUS_GHOST);
        }

        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,1.3f), coreVector2(0.0f,0.7f), fTime);

            if(PHASE_TIME_POINT(0.1f))
                this->_StartBoss();

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 2u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            this->__StartMission(9u);
        });
    }

    // ################################################################
    // return from color phase
    else if(m_iPhase == 10u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.8f)
        {
            PHASE_CHANGE_TO((coreMath::PopCount(m_iHelperState) == (g_pGame->IsEasy() ? 4u : 8u)) ? 40u : 32u)
        });
    }

    // ################################################################
    // return from white phase
    else if(m_iPhase == 11u)
    {
        if(PHASE_BEGINNING2)
        {
            m_bDead = true;

            m_fWaveValue = 1.1f;

            this->__EndFeeling();

            this->__DisableRange();
            this->__DisableWind ();

            g_pEnvironment->SetTargetSpeed(0.0f, 1.0f);
        }

        const coreVector2 vDir = coreVector2::Direction(m_fPhaseTime * 0.2f + (1.0f*PI));

        this->SetDirection(coreVector3(vDir, 0.0f));

        if(this->ReachedDeath())
        {
            this->Kill(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 30u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 31u)
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[SELECTED_HELPER] = -1;
            m_avVector [HELPER_DATA].z   = 1.0f*PI;
        }

        for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
        {
            const coreFloat fBlockTime = CLAMP01(m_fPhaseTime * 0.6f - I_TO_F(i % 7u) / 6.0f);

            const coreFloat fStartHeight = pBackground->GetStartHeight(i);
            const coreFloat fNewHeight   = fStartHeight * (1.0f + 1.0f * SIN(fBlockTime * (1.5f*PI)));

            const coreFloat   fLerp     = STEP(-DARK_DETAIL * 1.5f, DARK_DETAIL * 1.5f, fNewHeight);
            const coreVector3 vNewColor = LERP(0.1f, 1.0f, fLerp) * m_vColorTo;

            pBackground->SetBlockHeight(i, fNewHeight);
            pBackground->SetBlockColor (i, vNewColor);
        }

        const coreFloat fTime  = CLAMP01(m_fPhaseTime * 0.3f);
        const coreFloat fSpeed = 0.5f + 2.5f * SIN(fTime * (1.0f*PI));
        const coreFloat fSpawn = LERPB(FOREGROUND_AREA.x * 1.2f * SQRT2, 15.0f, fTime);
        const coreFloat fFade  = LERPH3(0.0f, 1.0f, CLAMP01(m_fPhaseTime - (1.0f / 0.3f - 1.0f)));

        const coreUintW iRotaCount = PROJECTONE_SHIELDS;
        coreUintW       iRotaNum   = 0u;

        m_avVector[HELPER_DATA].z += fSpeed * TIME;

        for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        {
            if(HAS_BIT(m_iHelperState, i)) continue;

            cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
            cCustomEnemy* pShield = &m_aShield[i];

            if(PHASE_MAINTIME_POINT(0.0f)) pHelper->Resurrect(false);
            if(PHASE_MAINTIME_POINT(1.0f / 0.3f - 1.0f)) pShield->Resurrect();

            const coreVector2 vDir = coreVector2::Direction((2.0f*PI) * I_TO_F(iRotaNum) * RCP(I_TO_F(iRotaCount)) + m_avVector[HELPER_DATA].z);
            const coreVector2 vPos = this->GetPosition().xy() + vDir * fSpawn;

            pHelper->SetPosition(coreVector3(vPos, 0.0f));

            pShield->SetSize (fFade * coreVector3(5.0f,5.0f,5.0f));
            pShield->SetAlpha(fFade);

            if(!pShield->HasStatus(ENEMY_STATUS_DEAD) && (pShield->GetLostHealth() >= 50))
            {
                m_aiCounter[SELECTED_HELPER] = i;
                m_avVector [HELPER_DATA].xy(vPos);
            }

            iRotaNum += 1u;
        }

        if(m_aiCounter[SELECTED_HELPER] >= 0)
        {
            PHASE_CHANGE_TO(33u)

            for(coreUintW j = 0u; j < PROJECTONE_SHIELDS; ++j)
                m_aShield[j].AddStatus(ENEMY_STATUS_GHOST);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 32u)
    {
        if(PHASE_BEGINNING2)
        {
            m_aiCounter[SELECTED_HELPER] = -1;
            m_avVector [HELPER_DATA].z   = 1.0f*PI;

            for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
                m_aShield[i].RemoveStatus(ENEMY_STATUS_GHOST);
        }

        this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.0f), BLENDS(MIN1(m_fPhaseTime * 0.8f)));
        this->SetDirection   (coreVector3(0.0f,-1.0f,0.0f));

        const coreFloat fSpawn = LERPB (0.0f, 15.0f, CLAMP01(m_fPhaseTime - 0.2f));
        const coreFloat fFade  = LERPH3(0.0f,  1.0f, CLAMP01(m_fPhaseTime - 0.0f));

        const coreUintW iRotaCount = PROJECTONE_SHIELDS - coreMath::PopCount(m_iHelperState);
        coreUintW       iRotaNum   = 0u;

        m_avVector[HELPER_DATA].z += 0.5f * TIME;

        for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        {
            if(HAS_BIT(m_iHelperState, i)) continue;

            cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
            cCustomEnemy* pShield = &m_aShield[i];

            if(PHASE_MAINTIME_POINT(0.2f)) pHelper->Resurrect(true);
            if(PHASE_MAINTIME_POINT(0.0f)) pShield->Resurrect();

            const coreVector2 vDir = coreVector2::Direction((2.0f*PI) * I_TO_F(iRotaNum) * RCP(I_TO_F(iRotaCount)) + m_avVector[HELPER_DATA].z);
            const coreVector2 vPos = this->GetPosition().xy() + vDir * fSpawn;

            pHelper->SetPosition(coreVector3(vPos, 0.0f));

            pShield->SetSize (fFade * coreVector3(5.0f,5.0f,5.0f));
            pShield->SetAlpha(fFade);

            if(!pShield->HasStatus(ENEMY_STATUS_DEAD) && (pShield->GetLostHealth() >= 50))
            {
                m_aiCounter[SELECTED_HELPER] = i;
                m_avVector [HELPER_DATA].xy(vPos);
            }

            iRotaNum += 1u;
        }

        if(m_aiCounter[SELECTED_HELPER] >= 0)
        {
            PHASE_CHANGE_INC

            for(coreUintW j = 0u; j < PROJECTONE_SHIELDS; ++j)
                m_aShield[j].AddStatus(ENEMY_STATUS_GHOST);
        }
    }

    // ################################################################
    // select helper
    else if(m_iPhase == 33u)
    {
        const coreFloat fFade = LERPB(1.0f, 0.0f, MIN(m_fPhaseTime * 3.0f, 1.0f));
        const coreFloat fLerp = MIN(m_fPhaseTime * 1.2f * RCP(m_HelperPath.GetTotalDistance()), 1.0f);

        const coreUintW iRotaCount = GAME_HELPERS - 1u - coreMath::PopCount(m_iHelperState);
        coreUintW       iRotaNum   = 0u;

        for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
        {
            if(HAS_BIT(m_iHelperState, i)) continue;

            cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
            cCustomEnemy* pShield = &m_aShield[i];

            if(i == coreUintW(m_aiCounter[SELECTED_HELPER]))
            {
                const coreVector2 vDir = coreVector2::Direction((2.0f*PI) * I_TO_F(iRotaNum) * RCP(I_TO_F(iRotaCount)) + m_avVector[HELPER_DATA].z);
                const coreVector2 vPos = MapToAxis(m_HelperPath.CalcPositionLerp(fLerp), vDir) * FOREGROUND_AREA + m_avVector[HELPER_DATA].xy() * (1.0f - fLerp);

                pHelper->SetPosition(coreVector3(vPos, 0.0f));

                pShield->SetSize ((1.5f - 0.5f * fFade) * coreVector3(5.0f,5.0f,5.0f));
                pShield->SetAlpha(fFade);
            }
            else
            {
                const coreVector2 vDir = pHelper->GetPosition().xy().Normalized();
                const coreVector2 vPos = pHelper->GetPosition().xy() + vDir * (10.0f * (3.0f + 6.0f * m_fPhaseTime) * TIME);

                pHelper->SetPosition(coreVector3(vPos, 0.0f));

                pShield->SetSize (fFade * coreVector3(5.0f,5.0f,5.0f));
                pShield->SetAlpha(fFade);
            }

            iRotaNum += 1u;
        }

        if(fLerp >= 1.0f)
        {
            const coreUintW iIndex = m_aiCounter[SELECTED_HELPER];

            ADD_BIT(m_iHelperState, iIndex)

            this->__SwitchHealth(iIndex);
            this->__SwitchColor (iIndex, true);
            this->__StartMission(iIndex);

            for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
            {
                cHelper*      pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);
                cCustomEnemy* pShield = &m_aShield[i];

                pHelper->Kill(false);
                pShield->Kill(false);
            }

            this->__EndFeeling();

            g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), m_aShield[iIndex].GetColor3());
        }
    }

    // ################################################################
    // receive white helper
    else if(m_iPhase == 40u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 41u)
    {
        cHelper* pHelper = g_pGame->GetHelper(ELEMENT_WHITE);

        if(PHASE_BEGINNING2)
        {
            pHelper->Resurrect(false);
        }

        constexpr coreFloat fDelay = 2.5f / 0.6f;

        const coreFloat fTime = POW(m_fPhaseTime, 1.2f);
        const coreFloat fOff  = COS(fTime * (0.6f*PI)) * (LERPH3(1.0f, 0.7f, MIN(fTime / fDelay, 1.0f)) + LERPH3(0.6f, 0.0f, MIN(fTime / 1.2f, 1.0f)));
        const coreFloat fOff2 = SIN(fTime * (0.6f*PI)) * (LERPH3(0.8f, 0.0f, MIN(fTime / fDelay, 1.0f)));

        const coreVector2 vDir = coreVector2::Direction(fTime * (-0.1f*PI) + (0.25f*PI));
        const coreVector2 vPos = (vDir * fOff + vDir.Rotated90() * fOff2) * FOREGROUND_AREA;

        pHelper->SetPosition(coreVector3(vPos, 0.0f));

        this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.0f), BLENDB(MIN1(m_fPhaseTime)));

        if(fTime >= fDelay)
        {
            this->__SwitchHealth(8u);
            this->__SwitchColor (8u, true);
            this->__StartMission(8u);

            pHelper->Kill(false);

            g_pSpecialEffects->MacroExplosionColorBig(this->GetPosition(), COLOR_ENERGY_WHITE);
        }
    }

    // ################################################################
    // 
    else if((m_iPhase >=  50u) && (m_iPhase <=  59u)) this->__MoveYellow ();
    else if((m_iPhase >=  60u) && (m_iPhase <=  69u)) this->__MoveOrange ();
    else if((m_iPhase >=  70u) && (m_iPhase <=  79u)) this->__MoveRed    ();
    else if((m_iPhase >=  80u) && (m_iPhase <=  89u)) this->__MoveMagenta();
    else if((m_iPhase >=  90u) && (m_iPhase <=  99u)) this->__MovePurple ();
    else if((m_iPhase >= 100u) && (m_iPhase <= 119u)) this->__MoveBlue   ();
    else if((m_iPhase >= 120u) && (m_iPhase <= 129u)) this->__MoveCyan   ();
    else if((m_iPhase >= 130u) && (m_iPhase <= 139u)) this->__MoveGreen  ();
    else if((m_iPhase >= 140u) && (m_iPhase <= 149u)) this->__MoveWhite  ();
    else if((m_iPhase >= 150u) && (m_iPhase <= 159u)) this->__MoveIntro  ();

    // ################################################################
    // ################################################################

    // 
    for(coreUintW i = 0u; i < PROJECTONE_SHIELDS; ++i)
    {
        const cHelper* pHelper = g_pGame->GetHelper(ELEMENT_YELLOW + i);

        m_aShield[i].SetPosition (pHelper->GetPosition());
        m_aShield[i].SetTexOffset(coreVector2(0.0f, 0.1f * m_fAnimation + I_TO_F(i) / I_TO_F(PROJECTONE_SHIELDS)));
    }

    if(!m_bDead)
    {
        if(!HAS_FLAG(m_iStatus, ENEMY_STATUS_HIDDEN))
        {
            if(!m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__EnableRange();
            if(!m_Wind .IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__EnableWind ();
        }
        else
        {
            if(m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableRange();
            if(m_Wind .IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableWind ();
        }

        if(m_Range.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            const coreVector2 vDir = coreVector2::Direction(m_fAnimation * (-1.6f*PI));

            // 
            m_Range.SetPosition (this->GetPosition());
            m_Range.SetSize     (coreVector3(1.0f,1.0f,1.0f) * PLAYER_RANGE_SIZE * this->GetSize().x);
            m_Range.SetDirection(coreVector3(vDir, 0.0f));
            m_Range.SetAlpha    (this->GetAlpha());
            m_Range.SetTexOffset(m_Range.GetTexOffset() - m_Range.GetDirection().xy() * (0.1f * TIME));
            m_Range.Move();
        }

        if(m_Arrow.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_fArrowValue.Update(-2.0f);

            // 
            if(m_fArrowValue <= 0.0f) this->__DisableArrow();

            // 
            m_Arrow.SetPosition (this->GetPosition () + this->GetDirection() * 6.2f * this->GetSize().x);
            m_Arrow.SetSize     (coreVector3(1.0f,1.0f,1.0f) * 1.3f * this->GetSize().x);
            m_Arrow.SetDirection(this->GetDirection());
            m_Arrow.SetAlpha    (LERPH3(0.0f, 1.0f, m_fArrowValue) * this->GetAlpha());
            m_Arrow.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.15f));
            m_Arrow.Move();
        }

        if(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_Wind.SetPosition (this->GetPosition());
            m_Wind.SetSize     (coreVector3(1.0f,1.08f,1.0f) * PLAYER_WIND_SIZE * this->GetSize().x);
            m_Wind.SetAlpha    (this->GetAlpha());
            m_Wind.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.3f));
            m_Wind.Move();
        }

        if(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_MOVE))
        {
            // 
            m_Bubble.SetAlpha(MIN(m_Bubble.GetAlpha() + 4.0f*TIME, 1.0f));

            // 
            const coreVector2 vDir = coreVector2::Direction(m_fAnimation * (1.6f*PI));

            // 
            m_Bubble.SetPosition (this->GetPosition());
            m_Bubble.SetSize     (coreVector3(1.0f,1.0f,1.0f) * PLAYER_BUBBLE_SIZE * this->GetSize().x * m_Bubble.GetAlpha());
            m_Bubble.SetDirection(coreVector3(vDir, 0.0f));
            m_Bubble.SetTexOffset(m_Bubble.GetTexOffset() - 0.2f * TIME * m_Bubble.GetDirection().xy());
            m_Bubble.Move();
        }

        // 
        if(!coreMath::IsNear(m_vOldDir.x, this->GetDirection().x) ||
           !coreMath::IsNear(m_vOldDir.y, this->GetDirection().y))
        {
            m_vOldDir = this->GetDirection().xy();
            if(g_pForeground->IsVisibleObject(this)) this->__ShowArrow();
        }
    }

    if(m_fWaveValue < 1.0f)
    {
        m_fWaveValue.UpdateMin(0.65f, 1.0f);

        for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
        {
            const coreVector2 vPos = pBackground->GetBlockPositionNorm(i);

            const coreFloat fEdge  = vPos.Length() / SQRT2;
            const coreFloat fStep  = STEP(fEdge, fEdge + 1.0f, m_fWaveValue * 2.0f) * 2.0f;
            const coreFloat fValue = BLENDS(fStep);

            const coreFloat   fLerp      = 0.5f + 0.5f * fValue;
            const coreFloat   fNewHeight = LERP(-1.0f, 1.0f, fLerp) * DARK_DETAIL * 1.5f;
            const coreVector3 vNewColor  = LERP( 0.1f, 1.0f, fLerp) * LERPH3(m_vColorFrom, m_vColorTo, MIN1(fStep));

            pBackground->SetBlockHeight(i, fNewHeight);
            pBackground->SetBlockColor (i, vNewColor);
        }
    }
    else if((m_iPatternType >= 1u) && (m_iPatternType <= 8u))
    {
        m_fPatternValue.Update(1.0f);

        if((m_iPhase >= 30u) && (m_iPhase <= 49u)) m_fPatternStrength.UpdateMax(-0.5f, 0.0f);
                                              else m_fPatternStrength.UpdateMin(0.25f, 1.0f);

        const coreFloat fStrength = BLENDS(m_fPatternStrength);

        for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
        {
            const coreVector2 vPos = pBackground->GetBlockPositionNorm(i);

            coreFloat fValue;
            switch(m_iPatternType)
            {
            default: ASSERT(false)
            case 1u: /* yellow  */ fValue = COS((1.5f*PI) * vPos.x - m_fPatternValue * 0.3f) * SIN((1.5f*PI) * vPos.y - m_fPatternValue * 0.5f);                 break;   // checkerboard (from bottom left)
            case 2u: /* orange  */ fValue = SIN(vPos.Angle() * 2.0f + m_fPatternValue * 1.1f);                                                                   break;   // rotation
            case 3u: /* red     */ fValue = COS((4.0f*PI) * vPos.x - m_fPatternValue * 0.7f) * SIN((3.5f*PI) * vPos.y - m_fPatternValue * 0.5f);                 break;   // random
            case 4u: /* magenta */ fValue = SIN((0.5f*PI) * vPos.y + m_fPatternValue * 1.0f) * (((i / 2u) % 2u) ? -1.0f : 1.0f);                                 break;   // alternating wave (from top)
            case 5u: /* purple  */ fValue = SIN((2.0f*PI) * vPos.Length() / SQRT2 - m_fPatternValue * 1.0f);                                                     break;   // center wave
            case 6u: /* blue    */ fValue = SIN((1.0f*PI) * (vPos.y * 1.5f + SIN((1.0f*PI) * vPos.x + m_fPatternValue * 1.0f) * 0.3f) - m_fPatternValue * 0.8f); break;   // wavy wave (from bottom right)
            case 7u: /* cyan    */ fValue = SIN((2.0f*PI) * (vPos.x + vPos.y) + m_fPatternValue * 1.5f);                                                         break;   // diagonal wave (from top right)
            case 8u: /* green   */ fValue = SIN((1.0f*PI) * vPos.x - m_fPatternValue * 1.0f);                                                                    break;   // linear wave (from left)
            }

            const coreFloat   fLerp      = 0.5f + 0.5f * fValue * fStrength;
            const coreFloat   fNewHeight = LERP(-1.0f, 1.0f, fLerp) * DARK_DETAIL * 1.5f;
            const coreVector3 vNewColor  = LERP( 0.1f, 1.0f, fLerp) * m_vColorTo;

            pBackground->SetBlockHeight(i, fNewHeight);
            pBackground->SetBlockColor (i, vNewColor);
        }
    }
    else if(m_iPatternType == 9u)   // white
    {
        if(m_bDead) m_fWaveValue.UpdateMin(0.65f, 3.0f);

        m_fPatternStrength.UpdateMin(0.25f, 1.0f);

        const coreFloat fStrength = BLENDS(m_fPatternStrength);
        const coreFloat fLight    = LERP(0.1f, 1.0f, 0.5f + 0.5f * fStrength);

        for(coreUintW i = 0u; i < DARK_BLOCKS; ++i)
        {
            const coreVector2 vPos = pBackground->GetBlockPositionNorm(i);

            const coreVector2 vDiff  = vPos - this->GetPosition().xy() / (FOREGROUND_AREA * 1.2f);
            const coreFloat   fEdge  = vDiff.Length() / SQRT2;
            const coreFloat   fStep  = STEP(fEdge, fEdge + 1.0f, (m_fWaveValue - 1.0f) * 2.0f) * 2.0f;
            const coreFloat   fValue = LERP(-1.0f + 2.0f * MAX0(1.0f - fEdge * 2.1f), BLENDS(fStep), MIN1(fStep));

            const coreFloat   fLerp      = 0.5f + 0.5f * fValue * fStrength;
            const coreFloat   fNewHeight = LERP(-1.0f, 1.0f, fLerp) * DARK_DETAIL * 1.5f;
            const coreVector3 vNewColor  = LERP(DARK_COLOR_DEFAULT, LERPH3(m_vColorTo, DARK_COLOR_DEFAULT * 0.5f, MIN1(fStep)), STEP(0.0f, 0.5f, fLerp)) * fLight;

            pBackground->SetBlockHeight(i, fNewHeight);
            pBackground->SetBlockColor (i, vNewColor);
        }
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveYellow()
{
    ASSERT((m_iPhase >= 50u) && (m_iPhase <= 59u))

    cAterMission*   pAter   = d_cast<cAterMission*>  (g_pGame->GetCurMission());
    cHarenaMission* pHarena = d_cast<cHarenaMission*>(pAter->GetInnerMission());

    // ################################################################
    // 
    if(m_iPhase == 50u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            for(coreUintW i = 0u; i < HARENA_SPIKE_DIMENSION; ++i)
                pHarena->EnableSpike(i, true);

            pHarena->ChangeInsanityP1();

            m_avVector[SHOOT_TARGET].xy(this->AimAtPlayerDual(0u).Normalized());

            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 51u)
    {
        PHASE_CONTROL_TICKER(1u, 10u, 10.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = m_avVector[SHOOT_TARGET].xy();

            pHarena->CreateExternChild(vPos, vDir, 2u);
        });

        if(this->GetCurHealthPct() <= 0.6f)
        {
            PHASE_CHANGE_TO(53u)
        }
        else
        {
            PHASE_CONTROL_TIMER(0u, 0.3f, LERP_LINEAR)
            {
                if(PHASE_FINISHED)
                {
                    PHASE_CHANGE_INC
                    PHASE_RESET(1u)
                }
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 52u)
    {
        if(PHASE_BEGINNING2)
        {
            this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
        }

        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_LINEAR)
        {
            const coreVector2 vTarget = coreVector2::Direction(I_TO_F((m_aiCounter[MOLE_COUNT] * 5) % 12) / 12.0f * (2.0f*PI)) * 0.8f;

            this->DefaultMoveLerp(m_vLastPosition, vTarget, fTime);

            const coreMatrix2 mRota    = coreMatrix3::Rotation(2.0f / 12.0f * (2.0f*PI)).m12();
            const coreMatrix2 mRotaRev = mRota.Transposed();

            const coreVector3 vOther1 = coreVector3(LERP(m_vLastPosition, vTarget * mRota,    fTime) * FOREGROUND_AREA, 0.0f);
            const coreVector3 vOther2 = coreVector3(LERP(m_vLastPosition, vTarget * mRotaRev, fTime) * FOREGROUND_AREA, 0.0f);

            PHASE_CONTROL_TICKER(2u, 0u, 40.0f, LERP_LINEAR)
            {
                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), 0.0f, 1u, COLOR_ENERGY_WHITE);
                g_pSpecialEffects->CreateSplashColor(vOther1,             0.0f, 1u, COLOR_ENERGY_WHITE);
                g_pSpecialEffects->CreateSplashColor(vOther2,             0.0f, 1u, COLOR_ENERGY_WHITE);
            });

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(51u)

                m_aiCounter[MOLE_COUNT] += 1;

                     if(m_aiCounter[MOLE_COUNT] % 4 == 0) this->SetPosition(vOther1);
                else if(m_aiCounter[MOLE_COUNT] % 4 == 2) this->SetPosition(vOther2);

                m_avVector[SHOOT_TARGET].xy(this->AimAtPlayerDual(m_aiCounter[MOLE_COUNT] % 2).Normalized());

                this->SetDirection(coreVector3(m_avVector[SHOOT_TARGET].xy(), 0.0f));

                this->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            }
        });
    }

    // ################################################################
    // 
    else if((m_iPhase == 53u) || (m_iPhase == 54u) || (m_iPhase == 55u))
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < PROJECTONE_CLONES; ++i)
            {
                if(m_aClone[i].HasStatus(ENEMY_STATUS_DEAD)) m_aClone[i].Resurrect();

                m_aClone[i].SetAlpha (0.0f);
                m_aClone[i].AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            }

            this->AddStatus(ENEMY_STATUS_GHOST);

            m_aiCounter[GHOST_ACTIVE] = BITLINE(PROJECTONE_CLONES);
        }

        if(this->HasStatus(ENEMY_STATUS_GHOST))
        {
            this->SetPosition(coreVector3(HIDDEN_POS, 0.0f));

            for(coreUintW i = 0u; i < PROJECTONE_CLONES; ++i)
            {
                if(!HAS_BIT(m_aiCounter[GHOST_ACTIVE], i)) continue;

                coreVector2 vPos;
                switch(m_iPhase - 53u)
                {
                default: ASSERT(false)
                case 0u: vPos = StepRotated90X(i % 4u) * (i ? 1.0f : 0.0f);                                                       break;
                case 1u: vPos = coreVector2(SIN(m_fPhaseTime * 1.0f + I_TO_F(i)) * 0.8f, (I_TO_F(i) - 2.0f) * 0.4f);              break;
                case 2u: vPos = coreVector2::Direction(-m_fPhaseTime + I_TO_F(i) / I_TO_F(PROJECTONE_CLONES) * (2.0f*PI)) * 0.8f; break;
                }

                const coreVector2 vDir = m_aClone[i].AimAtPlayerSide().Normalized();

                m_aClone[i].SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                m_aClone[i].SetDirection(coreVector3(vDir,                   0.0f));
            }

            if(PHASE_MAINTIME_AFTER(1.0f))
            {
                const coreFloat fAlpha = (g_CurConfig.Graphics.iFlash && (FRACT(m_fPhaseTime * 20.0f) < 0.5f)) ? 0.9f : 0.4f;

                for(coreUintW i = 0u; i < PROJECTONE_CLONES; ++i)
                {
                    if(!HAS_BIT(m_aiCounter[GHOST_ACTIVE], i)) continue;

                    m_aClone[i].SetAlpha    (fAlpha);
                    m_aClone[i].RemoveStatus(ENEMY_STATUS_HIDDEN);

                    if(PHASE_MAINTIME_AFTER(1.2f))
                    {
                        Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, &m_aClone[i], [&](const cBullet* pBullet, const coreObject3D* pClone, const coreVector3 vIntersection, const coreBool bFirstHit)
                        {
                            if(!HAS_BIT(m_aiCounter[GHOST_ACTIVE], i)) return;
                            REMOVE_BIT(m_aiCounter[GHOST_ACTIVE], i)

                            m_aClone[i].SetPosition(coreVector3(HIDDEN_POS, 0.0f));

                            if(coreMath::PopCount(m_aiCounter[GHOST_ACTIVE]) <= 1u)
                            {
                                const coreUintW iIndex = MIN(coreMath::BitScanFwd(m_aiCounter[GHOST_ACTIVE]), PROJECTONE_CLONES - 1u);   // defensive

                                this->SetPosition (m_aClone[iIndex].GetPosition ());
                                this->SetDirection(m_aClone[iIndex].GetDirection());
                                this->RemoveStatus(ENEMY_STATUS_GHOST);

                                m_aiCounter[GHOST_ACTIVE] = 0;
                            }
                        });
                    }

                    if(!m_aiCounter[GHOST_ACTIVE]) break;
                }
            }
        }
        else
        {
            this->SetDirection(coreVector3(this->AimAtPlayerSide().Normalized(), 0.0f));

            for(coreUintW i = 0u; i < PROJECTONE_CLONES; ++i)
            {
                m_aClone[i].SetPosition(coreVector3(HIDDEN_POS, 0.0f));
            }

            PHASE_CONTROL_TICKER(0u, 0u, 2.0f, LERP_LINEAR)
            {
                const coreVector2 vPos  = this->GetPosition ().xy();
                const coreFloat   fBase = this->GetDirection().xy().Angle();
                const coreUintW   iNum  = (iTick % 2u) ? 8u : 7u;

                for(coreUintW j = iNum; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - I_TO_F(iNum - 1u) * 0.5f) * 20.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.6f);
                }
            });
        }

        if(this->GetCurHealthPct() <= I_TO_F(55u - m_iPhase) * 0.2f)
        {
            PHASE_CHANGE_INC
            PHASE_RESET(0u)

            if(this->ReachedDeath())
            {
                for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
                    pHarena->DisableSpike(i, true);

                for(coreUintW i = 0u; i < PROJECTONE_CLONES; ++i)
                    m_aClone[i].Kill(false);

                this->__EndExplosion(true);
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 56u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);

            if(PHASE_FINISHED)
            {
                this->__EndMission(true, true);
            }
        });
    }

    // ################################################################
    // ################################################################

    m_avVector[SPIKE_TIME].x += 1.0f * TIME;

    const coreFloat fHeight = SIN(m_avVector[SPIKE_TIME].x) * FOREGROUND_AREA.y;

    for(coreUintW j = 0u; j < HARENA_SPIKE_DIMENSION; ++j)
    {
        coreObject3D* pBoard = pHarena->GetSpikeBoard(j);

        pBoard->SetPosition(coreVector3(pBoard->GetPosition().x, fHeight, 0.0f));

        const coreBool bIsActive = pHarena->GetSpikeLaunched(j);
        const coreBool bIsQuiet  = pHarena->GetSpikeQuiet   (j);

        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            const coreVector2 vDiff = pPlayer->GetPosition().xy() - pBoard->GetPosition().xy();

            if((ABS(vDiff.x) < pBoard->GetCollisionRange().x) &&
               (ABS(vDiff.y) < pBoard->GetCollisionRange().y))
            {
                if(bIsActive &&  pPlayer->IsNormal ()) pPlayer->TakeDamage(10, ELEMENT_NEUTRAL, pPlayer->GetPosition().xy());
                if(bIsQuiet  && !pPlayer->IsRolling()) pHarena->LaunchSpike(j, 10.0f);
            }
        });
    }

    pHarena->PlayInsanity();
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveOrange()
{
    ASSERT((m_iPhase >= 60u) && (m_iPhase <= 69u))

    cAterMission* pAter = d_cast<cAterMission*>(g_pGame->GetCurMission());
    cGeluMission* pGelu = d_cast<cGeluMission*>(pAter->GetInnerMission());

    coreVector2 vWallOffset = coreVector2(0.0f,0.0f);

    // ################################################################
    // 
    if(m_iPhase == 60u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 61u)
    {
        if(PHASE_BEGINNING2)
        {
            pGelu->EnableOrb(0u);
            pGelu->SetLineMode(0u);
        }

        const coreVector2 vFrom = coreVector2(1.3f,0.0f) * FOREGROUND_AREA;
        const coreVector2 vTo   = coreVector2(0.0f,0.0f) * FOREGROUND_AREA;

        pGelu->GetOrb(0u)->SetPosition(coreVector3(LERPB(-vFrom, vTo, CLAMP01(m_fPhaseTime - 1.0f)), 0.0f));

        this->SetPosition(coreVector3(LERPBR(vTo, vFrom, MIN1(m_fPhaseTime)), 0.0f));

        const coreFloat fSpeed = 1.0f + m_fPhaseTime * 0.06f;

        if(PHASE_MAINTIME_POINT(2.0f))
        {
            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->AddStatus(PLAYER_STATUS_NO_INPUT_MOVE);
            });

            const coreVector3 vPos = pGelu->GetOrb(0u)->GetPosition();

            g_pSpecialEffects->CreateSplashColor(vPos, SPECIAL_SPLASH_SMALL, COLOR_ENERGY_CYAN);
            g_pSpecialEffects->CreateBlastSphere(vPos, SPECIAL_BLAST_SMALL,  COLOR_ENERGY_CYAN);
        }

        if(PHASE_MAINTIME_AFTER(2.0f))
        {
            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vDiff2 = pGelu->GetOrb(0u)->GetPosition().xy() - pPlayer->GetPosition().xy();
                if(!vDiff2.IsNull())
                {
                    const coreVector2 vPos = pPlayer->GetPosition().xy() + vDiff2.Normalized() * (3.0f * pPlayer->CalcMoveSpeed() * TIME * SmoothTowards(vDiff2.Length(), 3.0f));
                    pPlayer->SetPosition(coreVector3(vPos, 0.0f));
                }
            });

            const coreUint8 aiDance[] = {0u, 1u, 2u, 3u, 1u, 3u, 0u, 0u, 2u, 1u, 3u, 0u, 3u, 2u, 1u, 0u, 2u, 0u, 2u, 1u, 3u, 1u, 1u, 3u, 0u, 2u, 1u, 4u, 3u};

            PHASE_CONTROL_TICKER(0u, ARRAY_SIZE(aiDance), 1.0f * fSpeed, LERP_LINEAR)
            {
                if(aiDance[iTick] == 4u) return;

                for(coreUintW i = 0u, ie = (iTick == 28u) ? 4u : 1u; i < ie; ++i)
                {
                    const coreVector2 vDir = StepRotated90(aiDance[iTick]);
                    const coreVector2 vPos = vDir * 1.3f * FOREGROUND_AREA;

                    pGelu->EnableWay((iTick + i) % GELU_WAYS, MapStepRotated90(vPos, i), ((iTick == 22u) ? -vDir.Rotated90() : (iTick == 7u) ? vDir.Rotated90() : vDir));
                }
            });
        }

        for(coreUintW i = 0u; i < GELU_WAYS; ++i)
        {
            coreObject3D* pWay = pGelu->GetWay(i);
            if(!pWay->IsEnabled(CORE_OBJECT_ENABLE_MOVE) || !pGelu->IsWayActive(i)) continue;

            const coreVector2 vNewPos = pWay->GetPosition().xy() - pWay->GetPosition().xy().Normalized() * (25.0f * TIME * fSpeed);

            if(InBetween(coreVector2(0.0f,0.0f), vNewPos - pWay->GetCollisionRange().xy(), vNewPos + pWay->GetCollisionRange().xy()))
            {
                if(pGelu->IsWayVisible(i))
                {
                    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
                    {
                        if(pPlayer->IsNormal()) pPlayer->TakeDamage(5, ELEMENT_MAGENTA, pPlayer->GetPosition().xy());
                    });
                }

                pGelu->DisableWay(i, true);
            }

            pWay->SetPosition(coreVector3(vNewPos, 0.0f));
        }

        if(PHASE_MAINTIME_AFTER(10.0f) && !m_aTimer[0].GetStatus() && !pGelu->IsWayActiveAny())
        {
            PHASE_CHANGE_INC

            pGelu->DisableOrb(0u, true);

            g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->RemoveStatus(PLAYER_STATUS_NO_INPUT_MOVE);
            });
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 62u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                pGelu->EnableFang(i);

            pGelu->SetCrushFree(true);
            pGelu->SetCrushLong(false);
        }

        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK_REV)
        {
            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            {
                coreObject3D* pFang = pGelu->GetFang(i);
                if(!pFang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreUintW X = i % GELU_FANGS_DIMENSION;
                const coreUintW Y = i / GELU_FANGS_DIMENSION;

                coreUintW iCompare;
                switch(Y)
                {
                default: ASSERT(false)
                case 0u: iCompare = 3u; break;
                case 1u: iCompare = 2u; break;
                case 2u: iCompare = 2u; break;
                case 3u: iCompare = 3u; break;
                case 4u: iCompare = 2u; break;
                }

                const coreFloat   fBlend  = ((X >= iCompare) ? 3.0f : -3.0f) * (1.0f - fTime) + (((Y == 2u) && (X >= iCompare)) ? 1.0f : 0.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F(X) - 2.0f + fBlend, I_TO_F(Y) - 2.0f) * GELU_FANG_STEP;

                pFang->SetPosition(coreVector3(vOffset * FOREGROUND_AREA, 0.0f));
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 63u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.8f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 64u)
    {
        constexpr coreUintW aiTarget[] = {12u, 13u, 18u, 17u, 12u, 13u,  8u,  7u,  6u, 11u, 12u, 17u, 16u, 11u, 12u, 17u, 12u,  7u, 12u, /**/0u};
        constexpr coreUintW aiSide  [] = {      0u,  1u,  2u,  3u,  0u,  3u,  2u,  0u,  1u,  0u,  3u,  2u,  1u,  0u,  3u,  3u,  3u,  3u, /**/0u, 0u};
        STATIC_ASSERT(ARRAY_SIZE(aiTarget) == ARRAY_SIZE(aiSide))

        const coreFloat fTimePrev = FRACT(m_fFangTime * 1.5f);

        m_fFangTime.Update(0.3f + m_fPhaseTime * 0.035f);

        const coreFloat fTime = FRACT(m_fFangTime * 1.5f);

        if(fTimePrev > fTime)
        {
            m_aiCounter[FANG_DATA] += 1;
        }

        const coreUintW iOldTarget = aiTarget[m_aiCounter[FANG_DATA]];
        const coreUintW iOldSide   = aiSide  [m_aiCounter[FANG_DATA]];
        const coreUintW iNewTarget = aiTarget[m_aiCounter[FANG_DATA] + 1u];
        const coreUintW iNewSide   = aiSide  [m_aiCounter[FANG_DATA]] ^ BIT(1u);

        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
        {
            coreObject3D* pFang = pGelu->GetFang(i);
            if(!pFang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

            const coreUintW X = i % GELU_FANGS_DIMENSION;
            const coreUintW Y = i / GELU_FANGS_DIMENSION;

            coreVector2 vOffset = coreVector2(I_TO_F(X) - 2.0f, I_TO_F(Y) - 2.0f) * GELU_FANG_STEP;

            const auto nShiftFunc = [&](const coreUintW iTarget, const coreUintW iSide, const coreFloat fValue)
            {
                if(HAS_BIT(iSide, 0u) ? (Y == iTarget / GELU_FANGS_DIMENSION) : (X == iTarget % GELU_FANGS_DIMENSION))
                {
                    const coreVector2 vShift = StepRotated90(iSide) * (((i == iTarget) || ((i < iTarget) == (HAS_BIT(iSide, 0u) != HAS_BIT(iSide, 1u)))) ? 1.0f : 0.0f);
                    vOffset += vShift * fValue * GELU_FANG_STEP;
                }
            };
            nShiftFunc(iOldTarget, iOldSide, 1.0f - STEP(0.5f, 1.0f, fTime));
            nShiftFunc(iNewTarget, iNewSide,        STEP(0.0f, 0.5f, fTime));

            pFang->SetPosition(coreVector3(vOffset * FOREGROUND_AREA, 0.0f));
        }

        if(m_aiCounter[FANG_DATA] == ARRAY_SIZE(aiTarget) - 2u)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 65u)
    {
        PHASE_CONTROL_TIMER(0u, 0.7f, LERP_BREAK_REV)
        {
            for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            {
                coreObject3D* pFang = pGelu->GetFang(i);
                if(!pFang->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

                const coreUintW X = i % GELU_FANGS_DIMENSION;
                const coreUintW Y = i / GELU_FANGS_DIMENSION;

                coreUintW iCompare;
                switch(X)
                {
                default: ASSERT(false)
                case 0u: iCompare = 3u; break;
                case 1u: iCompare = 2u; break;
                case 2u: iCompare = 2u; break;
                case 3u: iCompare = 3u; break;
                case 4u: iCompare = 2u; break;
                }

                const coreFloat   fBlend  = ((Y >= iCompare) ? 3.0f : -3.0f) * fTime + (((X == 2u) && (Y >= iCompare)) ? 1.0f : 0.0f);
                const coreVector2 vOffset = coreVector2(I_TO_F(X) - 2.0f, I_TO_F(Y) - 2.0f + fBlend) * GELU_FANG_STEP;

                pFang->SetPosition(coreVector3(vOffset * FOREGROUND_AREA, 0.0f));
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                for(coreUintW i = 0u; i < GELU_FANGS; ++i)
                    pGelu->DisableFang(i, false);

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 66u)
    {
        PHASE_CONTROL_TICKER(1u, 20u, 15.0f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < POST_WALLS; ++i)
            {
                const coreVector2 vPos = coreVector2(1.2f,1.2f);
                const coreVector2 vDir = coreVector2(1.0f,0.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(i + 1, 0.0f, this, vPos, vDir)->ChangeSize(1.5f)->AddStatus(BULLET_STATUS_GHOST);
            }

            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_TINY);
        });

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            const coreFloat fCurHealhtPct = this->GetCurHealthPct();

            vWallOffset.x = LERP(0.25f, 0.0f, STEP(0.5f, 1.0f, fCurHealhtPct)) * fTime;
            vWallOffset.y = LERP(0.25f, 0.0f, STEP(0.0f, 0.5f, fCurHealhtPct)) * fTime;

            this->DefaultMoveLerp(coreVector2(0.0f,1.3f), coreVector2(0.0f,0.7f), fTime);

            if(PHASE_FINISHED && !m_aTimer[1].GetStatus())
            {
                PHASE_CHANGE_INC
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 67u)
    {
        if(PHASE_BEGINNING2)
        {
            this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.5f);
        }

        pAter->SetCollEnemyBullet([COLL_THIS](const cEnemy* pEnemy, const cBullet* pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)   // on base mission
        {
            if(!bFirstHit) return;

            const coreFloat fPower = I_TO_F(pBullet->GetDamage()) * RCP(I_TO_F(g_pGame->GetNumPlayers()));

            m_fPushForce += fPower * 4.0f;
        });

        const coreFloat fCurHealhtPct = this->GetCurHealthPct();

        vWallOffset.x = LERP(0.25f, 0.0f, STEP(0.5f, 1.0f, fCurHealhtPct));
        vWallOffset.y = LERP(0.25f, 0.0f, STEP(0.0f, 0.5f, fCurHealhtPct));

        coreVector2 vNewPos = this->GetPosition().xy();

        if(m_fPushForce)
        {
            vNewPos      += m_vPushDir * m_fPushForce * TIME;
            m_fPushForce *= FrictionFactor(3.0f);
        }

        const auto nBounceFunc = [this](const coreVector2 vPos, const coreUintW iFlip)
        {
            for(coreUintW j = 40u; j--; )
            {
                if((j % 10u) < 7u) continue;

                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j + 5u * iFlip) * 4.5f));

                const coreFloat fSpeed = 0.5f + 0.05f * I_TO_F(j % 10u);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.3f);
            }

            g_pSpecialEffects->CreateSplashColor(coreVector3(vPos, 0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
        };

        const coreVector2 vBossArea = FOREGROUND_AREA * 1.05f - vWallOffset * FOREGROUND_AREA.x * 2.2f;

             if((vNewPos.x < -vBossArea.x) && (m_vPushDir.x < 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x + vBossArea.x); m_vPushDir.x =  ABS(m_vPushDir.x); nBounceFunc(vNewPos, 0u);}
        else if((vNewPos.x >  vBossArea.x) && (m_vPushDir.x > 0.0f)) {vNewPos.x -= 2.0f * (vNewPos.x - vBossArea.x); m_vPushDir.x = -ABS(m_vPushDir.x); nBounceFunc(vNewPos, 0u);}
             if((vNewPos.y < -vBossArea.y) && (m_vPushDir.y < 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y + vBossArea.y); m_vPushDir.y =  ABS(m_vPushDir.y); nBounceFunc(vNewPos, 1u);}
        else if((vNewPos.y >  vBossArea.y) && (m_vPushDir.y > 0.0f)) {vNewPos.y -= 2.0f * (vNewPos.y - vBossArea.y); m_vPushDir.y = -ABS(m_vPushDir.y); nBounceFunc(vNewPos, 1u);}

        vNewPos.x = CLAMP(vNewPos.x, -vBossArea.x, vBossArea.x);
        vNewPos.y = CLAMP(vNewPos.y, -vBossArea.y, vBossArea.y);

        this->SetPosition(coreVector3(vNewPos, 0.0f));

        this->SetDirection(coreVector3(coreVector2::Direction(this->GetDirection().xy().Angle() + m_fPushForce * 0.2f * TIME), 0.0f));

        if(this->ReachedDeath())
        {
            PHASE_CHANGE_INC

            this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * PROJECTONE_COLL_SCALE);

            g_pGame->GetBulletManagerEnemy()->ClearBulletsTyped<cQuadBullet>(true);

            this->__EndExplosion(false);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 68u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            vWallOffset = coreVector2(1.0f,1.0f) * LERP(0.25f, 0.0f, BLENDBR(fTime));

            this->__EndAnimation(BLENDB(fTime));

            if(PHASE_FINISHED)
            {
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

                this->__EndMission(true, true);
            }
        });
    }

    // ################################################################
    // ################################################################

    for(coreUintW i = 0u; i < POST_WALLS; ++i)
    {
        g_pPostProcessing->SetWallOffset(i, vWallOffset.arr(i / 2u));
    }

    const coreVector2 vPlayerAreaFrom = PLAYER_AREA_DEFAULT.xy() + vWallOffset * 2.2f * FOREGROUND_AREA;
    const coreVector2 vPlayerAreaTo   = PLAYER_AREA_DEFAULT.zw() - vWallOffset * 2.2f * FOREGROUND_AREA;

    g_pGame->ForEachPlayerAll([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->SetArea(coreVector4(vPlayerAreaFrom, vPlayerAreaTo));
    });

    coreUintW aiIndex[POST_WALLS] = {};
    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cConeBullet>([&](cConeBullet* OUTPUT pBullet)
    {
        const coreUintW iWall = pBullet->GetDamage() - 1;
        ASSERT(iWall < POST_WALLS)

        const coreFloat fOffset = 1.07f - 2.2f * vWallOffset.arr(iWall / 2u);
        const coreFloat fSide   = (I_TO_F(aiIndex[iWall]) - 9.5f) / 10.0f;

        coreVector2 vPos = coreVector2(fSide, fOffset) * FOREGROUND_AREA;
        coreVector2 vDir = coreVector2(0.0f, -1.0f);
        switch(iWall)
        {
        default: ASSERT(false)
        case 0u: vPos = -vPos.Rotated90(); vDir = -vDir.Rotated90(); break;
        case 1u: vPos =  vPos.Rotated90(); vDir =  vDir.Rotated90(); break;
        case 2u: vPos = -vPos;             vDir = -vDir;             break;
        case 3u: break;
        }

        pBullet->SetPosition(coreVector3(vPos, 0.0f));
        pBullet->SetFlyDir  (vDir);

        if(pBullet->GetFlyTime() == 0.0f) g_pSpecialEffects->CreateSplashColor(pBullet->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_ORANGE);

        ++aiIndex[iWall];
    });

    if((aiIndex[0] == 20u) && (this->GetCurHealth() > 0))
    {
        g_pGame->ForEachPlayer([](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            if(pPlayer->IsNormal())
            {
                const coreVector2 vPos  = pPlayer->GetPosition().xy();
                const coreVector4 vArea = pPlayer->GetArea();

                if((vPos.x <= vArea.x + CORE_MATH_PRECISION) ||
                   (vPos.x >= vArea.z - CORE_MATH_PRECISION) ||
                   (vPos.y <= vArea.y + CORE_MATH_PRECISION) ||
                   (vPos.y >= vArea.w - CORE_MATH_PRECISION))
                {
                    pPlayer->TakeDamage(5, ELEMENT_NEUTRAL, vPos);
                }
            }
        });
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveRed()
{
    ASSERT((m_iPhase >= 70u) && (m_iPhase <= 79u))

    cAterMission*   pAter   = d_cast<cAterMission*>  (g_pGame->GetCurMission());
    cMuscusMission* pMuscus = d_cast<cMuscusMission*>(pAter->GetInnerMission());

    cDarkBackground* pBackground = d_cast<cDarkBackground*>(g_pEnvironment->GetBackground());

    // 
    const cEnemySquad* pSquad2 = g_pGame->GetCurMission()->GetEnemySquad(1u);
    ASSERT(pSquad2->GetNumEnemies() == PROJECTONE_ENEMIES_LEGION)

    // ################################################################
    // 
    if(m_iPhase == 70u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 71u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
        {
            this->SetAlpha(1.0f - fTime);

            pBackground->GetHeadlight()->SetAlpha(fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->AddStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 72u)
    {
        constexpr coreUintW aiPlaceGenerate[] = {3u, 7u, 9u, 13u, 17u, 18u, 19u, 20u, 21u, 22u, 27u, 32u, 33u, 35u, 37u, 39u, 45u, 49u, 50u, 51u, 53u, 54u, 64u, 65u, 67u, 69u, 70u, 71u};
        constexpr coreUintW aiPlacePearl   [] = {0u, 1u, 2u, 4u, 5u, 6u, 8u, 10u, 11u, 12u, 14u, 15u, 16u, 23u, 24u, 25u, 26u, 28u, 29u, 30u, 31u, 34u, 36u, 38u, 40u, 41u, 42u, 43u, 44u, 46u, 47u, 48u, 52u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u, 66u, 68u};
        STATIC_ASSERT(ARRAY_SIZE(aiPlaceGenerate) + ARRAY_SIZE(aiPlacePearl) == 8u * 9u)

        if(PHASE_BEGINNING2)
        {
            const auto nInitFunc = [](coreObject3D* OUTPUT pObject, const coreUintW iPlace)
            {
                const coreUintW X = iPlace % 8u;
                const coreUintW Y = iPlace / 8u;

                const coreVector2 vPos = (coreVector2(I_TO_F(X), I_TO_F(Y)) - 3.5f) * 0.275f + coreVector2(0.0f,2.2f);

                pObject->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            };

            for(coreUintW i = 0u; i < ARRAY_SIZE(aiPlaceGenerate); ++i)
            {
                pMuscus->EnableGenerate(i);
                nInitFunc(pMuscus->GetGenerate(i), aiPlaceGenerate[i]);
            }

            for(coreUintW i = 0u; i < ARRAY_SIZE(aiPlacePearl); ++i)
            {
                pMuscus->EnablePearl(i);
                nInitFunc(pMuscus->GetPearl(i), aiPlacePearl[i]);
            }
        }

        const coreVector2 vMove = coreVector2(0.0f,-14.0f) * TIME;

        const auto nMoveFunc = [&](coreObject3D* OUTPUT pObject)
        {
            coreVector2 vNewPos = pObject->GetPosition().xy() + vMove;

            if(vNewPos.y < -FOREGROUND_AREA.y * 4.5f * 0.275f) vNewPos.y += FOREGROUND_AREA.y * 9.0f * 0.275f;

            pObject->SetPosition(coreVector3(vNewPos, 0.0f));
        };

        for(coreUintW i = 0u; i < ARRAY_SIZE(aiPlaceGenerate); ++i)
        {
            nMoveFunc(pMuscus->GetGenerate(i));
        }

        for(coreUintW i = 0u; i < ARRAY_SIZE(aiPlacePearl); ++i)
        {
            if(pMuscus->IsPearlActive(i))
            {
                nMoveFunc(pMuscus->GetPearl(i));
            }
        }

        if(!pMuscus->IsPearlValidAny())
        {
            PHASE_CHANGE_INC

            for(coreUintW i = 0u; i < ARRAY_SIZE(aiPlaceGenerate); ++i)
            {
                pMuscus->DisableGenerate(i, false);
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 73u)
    {
        if(PHASE_BEGINNING2)
        {
            pBackground->FlashLightning();
            pBackground->GetHeadlight()->SetAlpha(0.0f);

            g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);

            this->SetAlpha(1.0f);
            this->RemoveStatus(ENEMY_STATUS_GHOST | ENEMY_STATUS_HIDDEN);

            for(coreUintW i = 0u; i < PROJECTONE_ENEMIES_LEGION; ++i)
            {
                cEnemy* pEnemy = pSquad2->GetEnemy(i);

                pEnemy->Resurrect();

                pEnemy->SetMaxHealth(10);
                pEnemy->SetCurHealth(1);
                pEnemy->RefreshColor();

                const coreVector2 vBase = StepRotated90(i);
                const coreVector2 vPos  = vBase * 1.05f + vBase.Rotated90() * 1.05f;
                const coreVector2 vDir  = vBase.Rotated45();

                pEnemy->SetPosition (coreVector3(vPos * FOREGROUND_AREA, 0.0f));
                pEnemy->SetDirection(coreVector3(vDir,                   0.0f));
            }
        }

        const coreUint8 iTeleport = GET_BITVALUE(m_aiCounter[TELEPORT_COUNT], 8u, 0u);

        this->SetDirection(coreVector3(this->AimAtPlayerDual(iTeleport % 2u).Normalized(), 0.0f));

        if(iTeleport == 0u)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 14.0f, LERP_LINEAR)
            {
                if((iTick % 10u) < 5u) return;

                const coreVector2 vBase = coreVector2::Direction((0.5f + 0.5f * COS(I_TO_F(iTick) * (0.02f*PI))) * (0.5f*PI) + (0.5f*PI));

                for(coreUintW i = 0u; i < PROJECTONE_ENEMIES_LEGION; ++i)
                {
                    const cEnemy* pEnemy = pSquad2->GetEnemy(i);

                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = MapStepRotated90(vBase, i);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.8f, this, vPos, vDir)->ChangeSize(1.3f);
                }
            });
        }
        else if(iTeleport == 1u)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 0.35f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < PROJECTONE_ENEMIES_LEGION; ++i)
                {
                    const cEnemy* pEnemy = pSquad2->GetEnemy(i);

                    for(coreUintW j = 30u; j--; )
                    {
                        if(((j % 6u) < 3u) == ((iTick % 2u) != 0u)) continue;

                        const coreVector2 vBase = coreVector2::Direction((I_TO_F(j) / 29.0f) * (0.5f*PI) + (0.5f*PI));

                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = MapStepRotated90(vBase, i);

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.4f, this, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
            });

            if(!HAS_BIT(m_aiCounter[TELEPORT_COUNT], 8u))
            {
                ADD_BIT(m_aiCounter[TELEPORT_COUNT], 8u)
                m_aTimer[0].SetValue(m_aTimer[0].GetValue(CORE_TIMER_GET_NORMAL) + 0.5f);
            }
        }
        else if(iTeleport == 2u)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 16.0f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < PROJECTONE_ENEMIES_LEGION; ++i)
                {
                    if(((iTick % 30u) < 15u) == ((i % 2u) != 0u)) continue;

                    const cEnemy* pEnemy = pSquad2->GetEnemy(i);

                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = pEnemy->AimAtPlayerDual(i % 2u).Normalized();

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 1.0f, this, vPos, vDir)->ChangeSize(1.3f);
                }
            });
        }
        else if(iTeleport == 3u)
        {
            PHASE_CONTROL_TICKER(0u, 0u, 0.35f, LERP_LINEAR)
            {
                for(coreUintW i = 0u; i < PROJECTONE_ENEMIES_LEGION; ++i)
                {
                    const cEnemy* pEnemy = pSquad2->GetEnemy(i);

                    for(coreUintW j = 35u; j--; )
                    {
                        if(((j % 10u) < 5u) == (((iTick + i) % 2u) != 0u)) continue;

                        const coreVector2 vBase = LERP(coreVector2(0.0f,-1.0f), coreVector2(-1.0f,0.0f), I_TO_F(j) / 34.0f);

                        const coreVector2 vPos = pEnemy->GetPosition().xy();
                        const coreVector2 vDir = MapStepRotated90(vBase.Normalized(), i);

                        const coreFloat fSpeed = 0.6f * vBase.Length();

                        g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.3f);
                    }
                }
            });

            if(!HAS_BIT(m_aiCounter[TELEPORT_COUNT], 9u))
            {
                ADD_BIT(m_aiCounter[TELEPORT_COUNT], 9u)
                m_aTimer[0].SetValue(m_aTimer[0].GetValue(CORE_TIMER_GET_NORMAL) + 0.9f);
            }
        }
        else
        {
            const coreFloat fSpeed = 0.3f + 0.11f * I_TO_F(iTeleport - 4u);

            PHASE_CONTROL_TICKER(0u, 0u, 50.0f * fSpeed, LERP_LINEAR)
            {
                const coreVector2 vBase = coreVector2::Direction(I_TO_F(iTick) * GA);

                for(coreUintW i = 0u; i < PROJECTONE_ENEMIES_LEGION; ++i)
                {
                    const cEnemy* pEnemy = pSquad2->GetEnemy(i);

                    const coreVector2 vPos = pEnemy->GetPosition().xy();
                    const coreVector2 vDir = MapStepRotated90(vBase, i);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos,  vDir)->ChangeSize(1.3f);
                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos, -vDir)->ChangeSize(1.3f);
                }
            });
        }

        const coreBool bFinished = this->ReachedDeath();

        if(this->ReachedHealth(90 * 6 + 850 * 4) ||
           this->ReachedHealth(90 * 6 + 850 * 3) ||
           this->ReachedHealth(90 * 6 + 850 * 2) ||
           this->ReachedHealth(90 * 6 + 850)     ||
           this->ReachedHealth(90 * 6)           ||
           this->ReachedHealth(90 * 5)           ||
           this->ReachedHealth(90 * 4)           ||
           this->ReachedHealth(90 * 3)           ||
           this->ReachedHealth(90 * 2)           ||
           this->ReachedHealth(90)               ||
           bFinished)
        {
            const auto nTeleportFunc = [](cEnemy* OUTPUT pEnemy)
            {
                cPlayer* pPlayer = pEnemy->LastAttacker();

                const coreVector3 vOldPos = pPlayer->GetPosition();
                const coreVector3 vNewPos = pEnemy ->GetPosition();

                pPlayer->SetPosition(vNewPos);
                pEnemy ->SetPosition(vOldPos);

                g_pGame->GetBulletManagerPlayer()->ForEachBullet([&](cBullet* OUTPUT pBullet)
                {
                    if(pBullet->GetOwner() == pPlayer) pBullet->Deactivate(true);
                });

                const coreVector3 vPos  = vOldPos;
                const coreVector3 vDiff = vNewPos - vPos;
                const coreUintW   iNum  = MAX(F_TO_UI(vDiff.Length() / 1.9f), 2u);

                for(coreUintW j = iNum; j--; ) g_pSpecialEffects->CreateSplashColor(vPos + vDiff * (I_TO_F(j) * RCP(I_TO_F(iNum - 1u))), 10.0f, 1u, pPlayer->GetEnergyColor());
            };

            nTeleportFunc(this);

            g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

            m_aiCounter[TELEPORT_COUNT] += 1;
        }

        if(bFinished)
        {
            PHASE_CHANGE_INC

            pSquad2->ClearEnemies(true);

            this->__EndExplosion(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 74u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);

            if(PHASE_FINISHED)
            {
                this->__EndMission(true, true);
            }
        });
    }

    // ################################################################
    // ################################################################

    g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        for(coreUintW j = 0u; j < MUSCUS_PEARLS; ++j)
        {
            const coreObject3D* pPearl = pMuscus->GetPearl(j);
            if(!pMuscus->IsPearlActive(j)) continue;

            const coreVector2 vDiff = pPlayer->GetPosition().xy() - pPearl->GetPosition().xy();
            if(vDiff.LengthSq() < POW2(4.5f))
            {
                pMuscus->StrikeAttack(j, pPlayer, this);
                g_pSpecialEffects->CreateSplashColor(pPearl->GetPosition(), 5.0f, 3u, COLOR_ENERGY_WHITE);
            }
        }
    });

    const coreUint64 iStrikeState = pMuscus->GetStrikeState();
    if(iStrikeState)
    {
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
        {
            if(HAS_BIT(iStrikeState, i))
            {
                const coreUint8 iDamage = 50u / GAME_PLAYERS * g_pGame->GetNumPlayers();

                this->TakeDamage(iDamage, ELEMENT_NEUTRAL, this->GetPosition().xy(), pMuscus->GetStrikePlayer(i));
            }
        }
    }

    for(coreUintW j = 0u; j < MUSCUS_GENERATES; ++j)
    {
        const coreObject3D* pGenerate = pMuscus->GetGenerate(j);
        if(!pGenerate->IsEnabled(CORE_OBJECT_ENABLE_MOVE)) continue;

        g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW i)
        {
            const coreVector2 vDiff = pGenerate->GetPosition().xy() - pPlayer->GetPosition().xy();
            if(vDiff.LengthSq() < POW2(20.0f))
            {
                pMuscus->ShowGenerate(j, 0.1f);
            }
        });
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveMagenta()
{
    ASSERT((m_iPhase >= 80u) && (m_iPhase <= 89u))

    cAterMission*    pAter    = d_cast<cAterMission*>   (g_pGame->GetCurMission());
    cRutilusMission* pRutilus = d_cast<cRutilusMission*>(pAter->GetInnerMission());

    // 
    const cEnemySquad* pSquad1 = g_pGame->GetCurMission()->GetEnemySquad(0u);
    ASSERT(pSquad1->GetNumEnemies() == PROJECTONE_ENEMIES_METEOR)

    if(m_iPhase >= 82u)
    {
        m_avVector[RUTILUS_ROTA].x += 1.0f * TIME;

        const coreVector2 vDir = coreVector2::Direction(m_avVector[RUTILUS_ROTA].x * 0.5f);
        const coreFloat   fLen = LERPS(0.0f, 0.9f, MIN1(m_avVector[RUTILUS_ROTA].x * 0.2f));

        this->SetPosition(coreVector3(vDir * fLen * FOREGROUND_AREA, 0.0f));
    }

    // ################################################################
    // 
    if(m_iPhase == 80u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            pRutilus->EnablePlate(3u, 0.0f, 0.0f, 0.0f, 0.4f);
            pRutilus->EnablePlate(1u, 0.0f, 0.0f, 0.0f, 0.4f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 81u)
    {
        if(PHASE_BEGINNING2)
        {
            pRutilus->EnableArea();
        }

        this->SetPosition(coreVector3(0.0f,0.0f,0.0f));

        const coreFloat fScale = LERP(2.0f, 0.5f, STEP(0.0f, 1500.0f, I_TO_F(this->GetLostHealth())));

        pRutilus->SetAreaPosition(this->GetPosition().xy());
        pRutilus->SetAreaScale   (fScale);

        PHASE_CONTROL_TICKER(0u, 0u, 6.0f, LERP_LINEAR)
        {
            const coreVector2 vPos  = this->GetPosition().xy();
            const coreFloat   fBase = this->AimAtPlayerDual(0u).Normalized().Angle();

            for(coreUintW j = 5u; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 3.0f) + fBase);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 1.4f, this, vPos, vDir)->ChangeSize(1.4f);
            }
        });

        if(this->GetLostHealth() >= 1500)
        {
            PHASE_CHANGE_INC

            g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

            pRutilus->DisableArea(true);
            g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_PURPLE);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 82u)
    {
        if(PHASE_BEGINNING2)
        {
            pRutilus->EnableWave(0u);
            pRutilus->SetWavePosition(coreVector2(0.0f,0.0f));
            pRutilus->SetWavePull(true);
        }

        PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = this->AimAtPlayerDual(1u).Normalized();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.6f)->AddStatus(BULLET_STATUS_IMMORTAL);
        });

        if(this->GetLostHealth() >= 2500)
        {
            PHASE_CHANGE_INC

            g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

            pRutilus->DisableWave(true);
            g_pSpecialEffects->CreateSplashColor(coreVector3(0.0f,0.0f,0.0f), SPECIAL_SPLASH_TINY, COLOR_ENERGY_RED);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 83u)
    {
        if(PHASE_BEGINNING2)
        {
            m_iMeteorDir = 0b01010101u;

            for(coreUintW i = 0u; i < PROJECTONE_ENEMIES_METEOR; ++i)
            {
                const coreVector2 vPos = coreVector2(FmodRange(I_TO_F(i) * 0.7f, -1.0f, 1.0f) + 2.3f * (HAS_BIT(m_iMeteorDir, i) ? 1.0f : -1.0f), (I_TO_F(i) - 3.5f) * 0.25f * 1.05f);

                pSquad1->GetEnemy(i)->Resurrect();
                pSquad1->GetEnemy(i)->SetPosition(coreVector3(vPos * FOREGROUND_AREA, 0.0f));
            }
        }

        const coreFloat fRotaSpeed = LERP(0.3f, 0.5f, STEP(2500.0f, 3300.0f, I_TO_F(this->GetLostHealth()))) * MIN1(m_fPhaseTime * 0.1f);

        const coreFloat fOldGameAngle = m_fGameAngle;
        m_fGameAngle += fRotaSpeed * TIME;

        const coreVector2 vOldAxis = coreVector2::Direction(fOldGameAngle);
        const coreVector2 vNewAxis = coreVector2::Direction(m_fGameAngle);

        g_pPostProcessing->SetDirectionGame(vNewAxis);

        this->ToAxis(vNewAxis);

        g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
        {
            pPlayer->SetPosition   (MapToAxis(MapToAxisInv(pPlayer->GetPosition   (), vOldAxis), vNewAxis));
            pPlayer->SetDirection  (MapToAxis(MapToAxisInv(pPlayer->GetDirection  (), vOldAxis), vNewAxis));
            pPlayer->SetOrientation(MapToAxis(MapToAxisInv(pPlayer->GetOrientation(), vOldAxis), vNewAxis));
        });

        PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
        {
            if((iTick % 30u) < 20u) return;

            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->AimAtPlayerDual(0u).Normalized();

            g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.5f);
        });

        if(this->ReachedDeath())
        {
            PHASE_CHANGE_INC

            g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetDirection(coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
            });

            this->__EndExplosion(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 84u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);
        });

        PHASE_CONTROL_TICKER(1u, PROJECTONE_ENEMIES_METEOR, 6.0f, LERP_LINEAR)
        {
            pSquad1->GetEnemy(iTick)->Kill(true);
        });

        const coreVector2 vNewAxis = coreVector2::Direction(AngleLerp(m_fGameAngle, 0.0f, BLENDS(MIN1(m_fPhaseTime * 0.5f))));

        g_pPostProcessing->SetDirectionGame(vNewAxis);

        if(PHASE_MAINTIME_AFTER(2.0f))
        {
            this->__EndMission(true, true);
        }
    }

    // ################################################################
    // ################################################################

    this->SetDirection(coreVector3(this->AimAtPlayerDual((m_iPhase == 82u) ? 1u : 0u).Normalized(), 0.0f));   // after, to handle screen rotation

    if(pRutilus->IsPlateEnabled(3u) || pRutilus->IsPlateEnabled(1u))
    {
        const coreFloat fOldPlateTime = m_fPlateTime;
        m_fPlateTime.Update(0.5f);

        const coreVector2 vPlateDir    = coreVector2::Direction(m_fPlateTime);
        const coreFloat   fPlateSide   = SIN(m_fPlateTime) * 0.3f;
        const coreFloat   fPlateOffset = m_fPlateTime * -0.5f;

        pRutilus->SetPlateDirection(3u, vPlateDir.InvertedX());
        pRutilus->SetPlateDirection(1u, vPlateDir);

        pRutilus->SetPlateSide     (3u, fPlateSide * -1.0f);
        pRutilus->SetPlateSide     (1u, fPlateSide);

        pRutilus->SetPlateOffsetNow(3u, FmodRange(MIN1(fPlateOffset + 2.0f), -1.0f, 1.0f));
        pRutilus->SetPlateOffsetNow(1u, FmodRange(MIN1(fPlateOffset + 1.0f), -1.0f, 1.0f));

        if(m_iPhase >= 83u)
        {
            if(F_TO_UI(fOldPlateTime * 0.25f + 0.5f) != F_TO_UI(m_fPlateTime * 0.25f + 0.5f)) pRutilus->DisablePlate(3u, false);
            if(F_TO_UI(fOldPlateTime * 0.25f)        != F_TO_UI(m_fPlateTime * 0.25f))        pRutilus->DisablePlate(1u, false);
        }
    }

    STAGE_FOREACH_ENEMY_ALL(pSquad1, pEnemy, i)
    {
        const coreVector2 vMove = coreVector2(HAS_BIT(m_iMeteorDir, i) ? -1.0f : 1.0f, 0.0f) * 0.2f;

        coreVector2 vCurPos = pEnemy->GetPosition().xy() + vMove * (50.0f * TIME);

             if((vCurPos.x < -FOREGROUND_AREA.x * 1.0f) && (vMove.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.0f); REMOVE_BIT(m_iMeteorDir, i)}
        else if((vCurPos.x >  FOREGROUND_AREA.x * 1.0f) && (vMove.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.0f); ADD_BIT   (m_iMeteorDir, i)}

        pEnemy->SetPosition  (coreVector3(vCurPos, 0.0f));
        pEnemy->DefaultRotate(-m_fAnimation * (0.4f*PI) + I_TO_F(i));
    });

    STATIC_ASSERT(PROJECTONE_ENEMIES_METEOR <= sizeof(m_iMeteorDir)*8u)
}


// ****************************************************************
// 
void cProjectOneBoss::__MovePurple()
{
    ASSERT((m_iPhase >= 90u) && (m_iPhase <= 99u))

    // ################################################################
    // 
    if(m_iPhase == 90u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            constexpr coreUint8 aiNewOrder[] = {cConeBullet::ID, cOrbBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 91u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,-1.3f), fTime);
            this->DefaultRotateLerp(1.0f*PI,       3.0f*PI,                 fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 92u)
    {
        PHASE_CONTROL_TIMER(0u, 0.35f, LERP_LINEAR)
        {
            const coreUintW i = m_aiCounter[PATH_COUNT];

            const coreSpline2* pPath = ((i % 4u) < 2u) ? &m_aPurplePath[0] : &m_aPurplePath[1];

            const coreVector2 vFactor = coreVector2(((i % 2u) < 1u) ? -1.0f : 1.0f, (i < 4u) ? 1.0f : -1.0f);
            const coreVector2 vOffset = coreVector2((i < 4u) ? 0.0f : -0.2f, 0.0f);

            this->DefaultMovePath  (pPath, vFactor, vOffset * vFactor, fTime * pPath->GetTotalDistance());
            this->DefaultRotateLerp(0.0f*PI, 6.0f*PI, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_RESET(0u)
                PHASE_AGAIN

                m_aiCounter[PATH_COUNT] += 1;

                if(m_aiCounter[PATH_COUNT] == 6u)
                {
                    PHASE_CHANGE_INC
                }
            }
        });

        PHASE_CONTROL_TICKER(1u, 0u, 20.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();
            const coreVector2 vTan = vDir.Rotated90() * 1.5f;

            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.3f, this, vPos + vTan, vDir)->ChangeSize(1.5f);
            g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>(5, 1.3f, this, vPos - vTan, vDir)->ChangeSize(1.5f);
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 93u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(0.0f,1.4f), coreVector2(0.0f,0.5f), fTime);
        });

        m_avVector[MOVE_VALUE].x += 0.1f * TIME;
        m_avVector[MOVE_VALUE].y += 1.0f * TIME;

        const coreFloat fShift = SIN(m_avVector[MOVE_VALUE].y) * 0.5f * BLENDH3(CLAMP01(m_avVector[MOVE_VALUE].x - 0.5f)) * FOREGROUND_AREA.x;

        this->SetPosition (coreVector3(fShift, this->GetPosition().yz()));
        this->SetDirection(coreVector3(this->AimAtPlayerDual(0u).Normalized(), 0.0f));

        if(PHASE_MAINTIME_AFTER(1.0f))
        {
            PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * DEG_TO_RAD(27.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos,  vDir)->ChangeSize(1.7f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>(5, 1.1f, this, vPos, -vDir)->ChangeSize(1.7f);
            });

            PHASE_CONTROL_TICKER(2u, 0u, 1.0f, LERP_LINEAR)
            {
                const coreVector2 vPos  = this->GetPosition ().xy();
                const coreFloat   fBase = this->GetDirection().xy().Angle();

                for(coreUintW j = 5u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD((I_TO_F(j) - 2.0f) * 4.0f) + fBase);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.8f, this, vPos, vDir)->ChangeSize(1.6f);
                }
            });
        }

        if(this->GetLostHealth() >= 1500)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 94u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            const coreFloat fAngle = m_vLastDirection.xy().Angle();

            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(1.4f, m_vLastPosition.y), fTime);
            this->DefaultRotateLerp(fAngle,          fAngle + (4.0f*PI),                   fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 95u)
    {
        const coreInt32 iLostHealth = this->GetLostHealth();

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->DefaultMoveLerp(coreVector2(1.4f,0.0f), coreVector2(0.97f,0.0f), fTime);
        });

        const cPlayer* pPlayer = this->NearestPlayerDual(1u);

        this->SetPosition (coreVector3(this->GetPosition().x, pPlayer->GetPosition().y, this->GetPosition().z));
        this->SetDirection(coreVector3(-1.0f,0.0f,0.0f));

        PHASE_CONTROL_TICKER(1u, 0u, 0.95f, LERP_LINEAR)
        {
            for(coreUintW i = 0u; i < 40u; ++i)
            {
                if(((i + 1u * ((iTick * 2u) % 5u)) % 10u) < 5u) continue;

                const coreVector2 vDir = coreVector2(-1.0f,0.0f);
                const coreVector2 vPos = (vDir * -1.2f + vDir.Rotated90() * ((I_TO_F(i) - 19.5f - 0.5f) * 0.055f)) * FOREGROUND_AREA;

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 1.1f, this, vPos, vDir)->ChangeSize(1.5f);
            }
        });

        if(PHASE_MAINTIME_AFTER(1.0f))
        {
            const coreFloat fSpeed = LERP(2.0f, 3.0f, STEP(1500.0f, 2500.0f, I_TO_F(iLostHealth)));

            PHASE_CONTROL_TICKER(2u, 0u, fSpeed, LERP_LINEAR)
            {
                const coreFloat   fShift = (iTick % 4u) ? (I_TO_F(iTick % 4u) - 2.0f) : 0.0f;
                const coreVector2 vDir   = coreVector2(-1.0f, fShift * 0.14f).Normalized();

                for(coreUintW j = 10u; j--; )
                {
                    const coreVector2 vPos = this->GetPosition().xy() + coreVector2::Direction(I_TO_F(j) / 10.0f * (2.0f*PI)) * 5.0f;

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 1.4f, this, vPos, vDir)->ChangeSize(1.7f);
                }
            });
        }

        if(iLostHealth >= 2500)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 96u)
    {
        if(PHASE_MAINTIME_BEFORE(2.0f) || PHASE_MAINTIME_POINT(2.0f))
        {
            PHASE_CONTROL_TIMER(0u, 0.5f, LERP_SMOOTH)
            {
                if(PHASE_BEGINNING)
                {
                    m_avVector[MOVE_VALUE].z = m_vLastDirection.xy().Angle();
                }

                m_avVector[MOVE_VALUE].z = SmoothAimAngle(m_avVector[MOVE_VALUE].z, this->AimAtPlayerDual(0u).Angle(), fTime * 10.0f);

                this->DefaultMoveLerp  (m_vLastPosition,          coreVector2(0.0f,0.0f),               fTime);
                this->DefaultRotateLerp(m_avVector[MOVE_VALUE].z, m_avVector[MOVE_VALUE].z + (4.0f*PI), fTime);
            });
        }
        else
        {
            this->SetDirection(coreVector3(this->AimAtPlayerDual(0u).Normalized(), 0.0f));

            PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition ().xy();
                const coreVector2 vDir = this->GetDirection().xy();

                for(coreUintW j = 0u; j < 19u; ++j)
                {
                    const coreFloat fSpeed = 0.1f + 0.1f * I_TO_F(j);

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, fSpeed, this, vPos, vDir)->ChangeSize(1.5f);
                }
            });

            if(this->GetLostHealth() >= 3200)
            {
                PHASE_CHANGE_INC

                this->SetDirection(coreVector3(AlongCrossNormal(this->GetDirection().xy()), 0.0f));
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 97u)
    {
        coreVector2 vCurPos = this->GetPosition ().xy();
        coreVector2 vCurDir = this->GetDirection().xy();

        const coreVector2 vDiff = this->AimAtPlayerDual(1u);

        if(coreVector2::Dot(vDiff, vCurDir) <= 0.0f) vCurDir = vCurDir.Rotated90() * SIGN(coreVector2::Dot(vDiff, vCurDir.Rotated90()));

             if((vCurPos.x < -FOREGROUND_AREA.x * 1.1f) && (vCurDir.x < 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x + FOREGROUND_AREA.x * 1.1f); vCurDir.x =  ABS(vCurDir.x);}
        else if((vCurPos.x >  FOREGROUND_AREA.x * 1.1f) && (vCurDir.x > 0.0f)) {vCurPos.x -= 2.0f * (vCurPos.x - FOREGROUND_AREA.x * 1.1f); vCurDir.x = -ABS(vCurDir.x);}
             if((vCurPos.y < -FOREGROUND_AREA.y * 1.1f) && (vCurDir.y < 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y + FOREGROUND_AREA.y * 1.1f); vCurDir.y =  ABS(vCurDir.y);}
        else if((vCurPos.y >  FOREGROUND_AREA.y * 1.1f) && (vCurDir.y > 0.0f)) {vCurPos.y -= 2.0f * (vCurPos.y - FOREGROUND_AREA.y * 1.1f); vCurDir.y = -ABS(vCurDir.y);}

        this->SetPosition(coreVector3(vCurPos, 0.0f));
        this->DefaultMoveForward(vCurDir, 20.0f * MIN1(m_fPhaseTime * 0.8f));

        PHASE_CONTROL_TICKER(0u, 0u, 1.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();

            for(coreUintW j = 18u; j--; )
            {
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) / 18.0f * (1.0f*PI));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.2f, this, vPos,  vDir)->ChangeSize(1.3f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.2f, this, vPos, -vDir)->ChangeSize(1.3f);
            }
        });

        if(this->ReachedDeath())
        {
            PHASE_CHANGE_INC

            this->__EndExplosion(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 98u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);

            if(PHASE_FINISHED)
            {
                this->__EndMission(true, true);
            }
        });
    }

    // ################################################################
    // ################################################################
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveBlue()
{
    ASSERT((m_iPhase >= 100u) && (m_iPhase <= 119u))

    cAterMission*  pAter  = d_cast<cAterMission*> (g_pGame->GetCurMission());
    cCalorMission* pCalor = d_cast<cCalorMission*>(pAter->GetInnerMission());

    cSnow* pSnow = pCalor->GetSnow();

    coreFloat fDistance = 0.0f;

    const coreFloat   fGlobalSpeed = 40.0f;
    const coreVector2 vGlobalDir   = g_pGame->GetPlayer(0u)->GetDirection().xy();
    const coreVector2 vGlobalMove  = vGlobalDir * (-fGlobalSpeed * TIME);

    // ################################################################
    // 
    if(m_iPhase == 100u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 101u)
    {
        PHASE_CONTROL_TICKER(0u, 0u, 10.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = coreVector2(0.0f, 1.2f) * FOREGROUND_AREA;
            const coreVector2 vDir = coreVector2(0.0f,-1.0f);

                 if(((iTick)      % 12u) >= 7u) g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.8f, this, vPos,             vDir)            ->ChangeSize(1.3f);
            else if(((iTick + 6u) % 12u) >= 7u) g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.8f, this, vPos.Rotated90(), vDir.Rotated90())->ChangeSize(1.3f);
        });

        fDistance = BLENDS(MIN1(m_fPhaseTime * 0.3f)) * 0.6f;

        if(this->GetLostHealth() >= 700)
        {
            PHASE_CHANGE_INC
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 102u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            fDistance = LERP(0.6f, 1.3f * SQRT2, fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pCalor->EnableSnow();

                const coreVector2 vPos = vGlobalDir * FOREGROUND_AREA * 1.25f;
                const coreVector2 vDir = coreVector2(0.0f,1.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>(5, 0.0f, this, vPos, vDir)->ChangeSize(3.0f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 103u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.4f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 104u)
    {
        if(PHASE_BEGINNING2)
        {
            coreVector2 vNewPos;
            switch(m_aiCounter[EVADE_COUNT])
            {
            default: ASSERT(false)
            case 0: vNewPos = coreVector2(-1.3f, 0.5f); break;
            case 1: vNewPos = coreVector2( 0.5f,-1.3f); break;
            case 2: vNewPos = coreVector2(-0.5f, 1.3f); break;
            case 3: vNewPos = coreVector2( 1.3f,-0.5f); break;
            case 4: vNewPos = coreVector2( 0.5f, 1.3f); break;
            case 5: vNewPos = coreVector2(-1.3f,-0.5f); break;
            case 6: vNewPos = coreVector2( 1.3f, 0.5f); break;
            case 7: vNewPos = coreVector2(-0.5f,-1.3f); break;
            }

            this->StorePosition(vNewPos * FOREGROUND_AREA);

            this->SetDirection(coreVector3(AlongCrossNormal(-vNewPos), 0.0f));
        }

        PHASE_CONTROL_TIMER(0u, 4.0f, LERP_BREAK)
        {
            const coreVector2 vFrom = m_vLastPosition;
            const coreVector2 vTo   = IsHorizontal(vFrom) ? coreVector2(0.9f * SIGN(vFrom.x), vFrom.y) : coreVector2(vFrom.x, 0.9f * SIGN(vFrom.y));

            this->DefaultMoveLerp(vFrom, vTo, fTime);
        });

        if(!m_aTimer[0].GetStatus())
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
            {
                coreUintW iType;
                switch(m_aiCounter[EVADE_COUNT])
                {
                default: ASSERT(false)
                case 0: iType = 1; break;
                case 1: iType = 1; break;
                case 2: iType = 1; break;
                case 3: iType = 2; break;
                case 4: iType = 2; break;
                case 5: iType = 2; break;
                case 6: iType = 4; break;
                case 7: iType = 4; break;
                }

                const coreVector2 vPos = this->GetPosition().xy();

                for(coreUintW j = 40u; j--; )
                {
                    if(((j / iType) + (iTick & 0x01u)) & 0x01u) continue;

                    const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 9.0f + 4.5f));

                    g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.8f, this, vPos, vDir)->ChangeSize(1.6f);
                }
            });

            if(this->WasDamaged())
            {
                PHASE_CHANGE_INC

                m_aiCounter[EVADE_COUNT] += 1;
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 105u)
    {
        PHASE_CONTROL_TIMER(0u, 4.0f, LERP_BREAK_REV)
        {
            const coreVector2 vFrom = m_vLastPosition;
            const coreVector2 vTo   = IsHorizontal(vFrom) ? coreVector2(1.3f * SIGN(vFrom.x), vFrom.y) : coreVector2(vFrom.x, 1.3f * SIGN(vFrom.y));

            this->DefaultMoveLerp(vFrom, vTo, fTime);

            if(PHASE_FINISHED)
            {
                if(m_aiCounter[EVADE_COUNT] >= 8)
                {
                    PHASE_CHANGE_INC
                }
                else
                {
                    PHASE_CHANGE_TO(104u)
                }
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 106u)
    {
        PHASE_CONTROL_TIMER(0u, 0.8f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp  (coreVector2(1.3f,0.0f), coreVector2(-1.09f,0.0f), fTime);
            this->DefaultRotateLerp(1.0f*PI,                5.5f*PI,                  fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
                {
                    pBullet->Deactivate(false);

                    ASSERT(pSnow->IsActive())
                    pSnow->DrawPoint(pBullet->GetPosition().xy(), 20.0f, SNOW_TYPE_REMOVE);

                    pCalor->DisableSnow(true);

                    g_pSpecialEffects->MacroExplosionColorBig(pBullet->GetPosition(), COLOR_ENERGY_BLUE);
                });

                pCalor->EnableStar(0u, this, coreVector2(0.0f,0.0f));
                pCalor->GetStar(0u)->SetPosition(coreVector3(-1.1f,0.0f,0.0f) * FOREGROUND_AREA3);

                m_avVector[CALOR_ROTA].x = 0.5f*PI;

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 107u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 108u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(-0.4f,0.0f), fTime);
            this->DefaultRotateLerp(5.5f*PI,         3.5f*PI,                 fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pCalor->GetBoulder()->SetPosition(coreVector3(-1.25f,0.0f,0.0f) * FOREGROUND_AREA3);
                pCalor->GetBoulder()->Resurrect();

                pCalor->GetBoulder()->AddStatus   (ENEMY_STATUS_DAMAGING);
                pCalor->GetBoulder()->RemoveStatus(ENEMY_STATUS_GHOST);

                pCalor->CatchObject(0u, pCalor->GetBoulder());

                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 109u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.8f, m_vLastPosition.y), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 110u)
    {
        this->SetPosition(coreVector3(COS(m_fPhaseTime * 0.3f) * 0.8f * FOREGROUND_AREA.x, 0.0f, 0.0f));

        PHASE_CONTROL_TICKER(0u, 0u, 2.0f, LERP_LINEAR)
        {
            for(coreUintW j = 30u; j--; )
            {
                const coreVector2 vPos = coreVector2((j % 2u) ? 1.7f : 1.72f, (I_TO_F(j) - 14.5f + ((iTick % 2u) ? -0.25f : 0.25f)) * 0.08f) * FOREGROUND_AREA;
                const coreVector2 vDir = coreVector2(-1.0f,0.0f);

                g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>(5, 0.5f, this, vPos, vDir)->ChangeSize(1.5f)->AddStatus(BULLET_STATUS_IMMORTAL);
            }
        });

        if(this->ReachedDeath())
        {
            PHASE_CHANGE_INC

            pCalor->StartSwing(3.0f / CALOR_SWING_SPEED);

            for(coreUintW i = 0u; i < CALOR_STARS; ++i)
                pCalor->DisableStar(i, true);

            pCalor->GetBoulder()->RemoveStatus(ENEMY_STATUS_DAMAGING);
            pCalor->GetBoulder()->AddStatus   (ENEMY_STATUS_GHOST);

            this->__EndExplosion(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 111u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);

            if(PHASE_FINISHED)
            {
                pCalor->StopSwing();

                for(coreUintW i = 0u; i < CALOR_STARS; ++i)
                    pCalor->UncatchObject(i);

                pCalor->GetBoulder()->Kill(false);

                this->__EndMission(true, true);
            }
        });
    }

    // ################################################################
    // ################################################################

    if(m_iPhase >= 101u)
    {
        if(m_iPhase == 101u) m_avVector[CALOR_ROTA].z = MIN(m_avVector[CALOR_ROTA].z + 0.2f * TIME,  1.0f);
                        else m_avVector[CALOR_ROTA].z = MAX(m_avVector[CALOR_ROTA].z - 0.3f * TIME, -3.0f);

        if(m_avVector[CALOR_ROTA].z > 0.0f)
        {
            m_avVector[CALOR_ROTA].w += -0.4f * TIME * m_avVector[CALOR_ROTA].z;   // prev: -0.8f

            g_pEnvironment->SetTargetDirectionNow(coreVector2::Direction(m_avVector[CALOR_ROTA].w));

            const coreVector2 vEnvDir = g_pEnvironment->GetDirection();
            const coreVector2 vPush   = vEnvDir * (-60.0f * TIME * m_avVector[CALOR_ROTA].z);   // prev -40.0f

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                pPlayer->SetPosition(pPlayer->GetPosition() + coreVector3(vPush, 0.0f));
            });

            if(m_iPhase <= 102u)
            {
                this->SetPosition (coreVector3(vEnvDir * fDistance, 0.0f) * FOREGROUND_AREA3);
                this->SetDirection(coreVector3(vEnvDir * -1.0f,     0.0f));
            }

            g_pSpecialEffects->CreateGust(m_avVector[CALOR_ROTA].z, vEnvDir.Angle());
        }
        else
        {
            const coreVector2 vDir = coreVector2::Direction(AngleLerp(m_avVector[CALOR_ROTA].w, 0.0f, BLENDS(STEP(-3.0f, 0.0f, m_avVector[CALOR_ROTA].z))));

            g_pEnvironment->SetTargetDirectionNow(vDir);
        }
    }

    if(pSnow->IsActive())
    {
        g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cOrbBullet>([&](cOrbBullet* OUTPUT pBullet)
        {
            coreVector2 vNewPos = pBullet->GetPosition().xy() + vGlobalMove;

                 if((vNewPos.x < -FOREGROUND_AREA.x * 1.25f) && (vGlobalMove.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.5f;
            else if((vNewPos.x >  FOREGROUND_AREA.x * 1.25f) && (vGlobalMove.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.5f;
                 if((vNewPos.y < -FOREGROUND_AREA.y * 1.25f) && (vGlobalMove.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.5f;
            else if((vNewPos.y >  FOREGROUND_AREA.y * 1.25f) && (vGlobalMove.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.5f;

            pBullet->SetPosition(coreVector3(vNewPos, 0.0f));

            pSnow->DrawPoint(vNewPos, 7.0f, SNOW_TYPE_ADD);
        });
    }

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cFlipBullet>([](cFlipBullet* OUTPUT pBullet)
    {
        if(pBullet->GetFlyTime() >= 5.0f) pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
    });

    if(!pCalor->GetBoulder()->HasStatus(ENEMY_STATUS_DEAD) && (this->GetCurHealth() > 0))
    {
        m_avVector[CALOR_ROTA].y  = MIN1(m_avVector[CALOR_ROTA].y + 0.2f * TIME);
        m_avVector[CALOR_ROTA].x += 3.0f * TIME * m_avVector[CALOR_ROTA].y;

        const coreFloat   fLen = 0.7f * FOREGROUND_AREA.x;
        const coreVector2 vDir = coreVector2::Direction(m_avVector[CALOR_ROTA].x);
        const coreVector2 vPos = this->GetPosition().xy() + vDir * fLen;

        pCalor->GetStar(0u)->SetPosition(coreVector3(vPos, 0.0f));

        this->SetDirection(coreVector3(-vDir, 0.0f));
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveCyan()
{
    ASSERT((m_iPhase >= 120u) && (m_iPhase <= 129u))

    cAterMission* pAter = d_cast<cAterMission*>(g_pGame->GetCurMission());
    cNevoMission* pNevo = d_cast<cNevoMission*>(pAter->GetInnerMission());

    // ################################################################
    // 
    if(m_iPhase == 120u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC

            constexpr coreUint8 aiNewOrder[] = {cTriangleBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 121u)
    {
        PHASE_CONTROL_TIMER(0u, 1.3f, LERP_BREAK_REV)
        {
            this->DefaultMoveLerp  (m_vLastPosition, coreVector2(1.3f, m_vLastPosition.y), fTime);
            this->DefaultRotateLerp(1.0f*PI,         3.0f*PI,                              fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 122u)
    {
        if(PHASE_BEGINNING2)
        {
            for(coreUintW i = 0u; i < 16u; ++i)
                pNevo->EnableTile(i, 4u);

            this->AddStatus(ENEMY_STATUS_GHOST);
        }

        coreBool bComplete1 = true;

        static coreBool bComplete2 = false;
        static coreUint32 iTileState = 0u;
        static coreUint32 aiRemember[GAME_PLAYERS + 1u] = {};

        STATIC_ASSERT(LEVIATHAN_TILES <= sizeof(iTileState)   *8u)
        STATIC_ASSERT(LEVIATHAN_TILES <= sizeof(aiRemember[0])*8u)
        STATIC_ASSERT(LEVIATHAN_TILES <= NEVO_TILES)

        if(PHASE_BEGINNING2)
        {
            bComplete2 = false;
            iTileState = 0b1010'0101'1010'0101u;
            std::memset(aiRemember, 0, sizeof(aiRemember));
        }

        if(PHASE_MAINTIME_AFTER(4.0f))
        {
            PHASE_CONTROL_TIMER(4u, 0.25f, LERP_LINEAR)
            {
                constexpr coreUintW aiLane[] = {1u, 3u, 0u, 2u};

                const coreFloat   fOffset = (I_TO_F(aiLane[m_aiCounter[RUSH_COUNT] % 4]) - 1.5f) * 0.55f;
                const coreFloat   fSide   = ((m_aiCounter[RUSH_COUNT] / 2) % 2) ? -1.0f : 1.0f;
                const coreVector2 vFrom   = coreVector2(fOffset, fSide *  1.3f);
                const coreVector2 vTo     = coreVector2(fOffset, fSide * -1.3f);

                const coreFloat fRealTime = STEP(0.0f, 0.3f, fTime);

                this->SetDirection(coreVector3(coreVector2(0.0f, -fSide), 0.0f));

                this->DefaultMoveLerp     (vFrom,   vTo,     fRealTime);
                this->DefaultOrientateLerp(0.0f*PI, 4.0f*PI, fRealTime);

                if(!this->WasTeleporting())
                {
                    const auto nCreateBomb = [&](const coreUintW iIndex, const coreBool bDiagonal)
                    {
                        coreObject3D* pBomb = pNevo->GetBomb(iIndex);

                        pNevo->EnableBomb(iIndex, true);

                        pBomb->SetPosition (coreVector3(s_vPositionPoint,                   0.0f));
                        pBomb->SetDirection(coreVector3(StepRotated45(bDiagonal ? 1u : 0u), 0.0f));
                    };

                         if(PHASE_POSITION_POINT(this,  1.5f * 0.55f * FOREGROUND_AREA.y * fSide, y)) nCreateBomb(m_aiCounter[RUSH_COUNT] % 2,     (ABS(fOffset) < 0.5f));
                    else if(PHASE_POSITION_POINT(this,  0.5f * 0.55f * FOREGROUND_AREA.y * fSide, y)) nCreateBomb(m_aiCounter[RUSH_COUNT] % 2 + 2, (ABS(fOffset) > 0.5f));
                    else if(PHASE_POSITION_POINT(this, -0.5f * 0.55f * FOREGROUND_AREA.y * fSide, y)) nCreateBomb(m_aiCounter[RUSH_COUNT] % 2 + 4, (ABS(fOffset) < 0.5f));
                    else if(PHASE_POSITION_POINT(this, -1.5f * 0.55f * FOREGROUND_AREA.y * fSide, y)) nCreateBomb(m_aiCounter[RUSH_COUNT] % 2 + 6, (ABS(fOffset) > 0.5f));
                }

                if(PHASE_FINISHED)
                {
                    if(bComplete2)
                    {
                        PHASE_CHANGE_INC
                    }
                    else
                    {
                        PHASE_RESET(4u)

                        m_aiCounter[RUSH_COUNT] += 1;
                    }
                }
            });
        }

        for(coreUintW i = 0u; i < NEVO_TILES; ++i)
        {
            const coreObject3D* pTile = pNevo->GetTile(i);
            if(!pNevo->IsTileEnabled(i)) continue;

            if(!bComplete2)
            {
                const auto nTriggerFunc = [&](const coreObject3D* pObject, const coreUintW j)
                {
                    const coreVector2 vDiff = MapToAxisInv(pObject->GetPosition().xy() - pTile->GetPosition().xy(), pTile->GetDirection().xy());

                    if((ABS(vDiff.x) < pTile->GetCollisionRange().x) &&
                       (ABS(vDiff.y) < pTile->GetCollisionRange().y))
                    {
                        if(!HAS_BIT(aiRemember[j], i)) TOGGLE_BIT(iTileState, i)
                        ADD_BIT(aiRemember[j], i)
                    }
                    else
                    {
                        REMOVE_BIT(aiRemember[j], i)
                    }
                };

                g_pGame->ForEachPlayer([&](const cPlayer* pPlayer, const coreUintW j)
                {
                    if(pPlayer->IsRolling()) return;
                    nTriggerFunc(pPlayer, j);
                });

                nTriggerFunc(this, GAME_PLAYERS);

                if(HAS_BIT(iTileState, i))
                {
                    pNevo->SetTileStyle(i, 1u);
                }
                else
                {
                    pNevo->SetTileStyle(i, 0u);
                    bComplete1 = false;
                }
            }
        }

        if(!bComplete2)
        {
            bComplete2 = bComplete1;
            if(bComplete2)
            {
                for(coreUintW i = 0u; i < LEVIATHAN_TILES; ++i)
                {
                    pNevo->SetTileStyle(i, 2u);
                    pNevo->DisableTile(i, true);
                }
            }
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 123u)
    {
        PHASE_CONTROL_TIMER(0u, 0.25f * (1.0f/0.3f) * (2.6f/2.3f), LERP_LINEAR)
        {
            this->SetDirection(coreVector3(1.0f,0.0f,0.0f));

            this->DefaultMoveLerp     (coreVector2(-1.3f,0.0f), coreVector2(1.0f,0.0f), fTime);
            this->DefaultOrientateLerp(0.0f*PI,                 4.0f*PI,                fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                pNevo->EnableArrow(0u, this, coreVector2(0.0f,-1.0f));

                this->RemoveStatus(ENEMY_STATUS_GHOST);

                g_pSpecialEffects->CreateSplashColor(this->GetPosition(), SPECIAL_SPLASH_TINY, COLOR_ENERGY_CYAN);
                g_pSpecialEffects->ShakeScreen(SPECIAL_SHAKE_SMALL);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 124u)
    {
        constexpr coreFloat fArrowSpeed = 0.25f;

        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.0f), fTime);
        });

        this->DefaultRotate(m_fPhaseTime * (-2.0f*PI) * fArrowSpeed + STEPB(0.0f, 3.0f, m_fPhaseTime) * (-6.0f*PI));

        PHASE_CONTROL_TICKER(1u, 0u, 4.5f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition().xy();
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);

            g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(5, 0.4f, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
        });

        PHASE_CONTROL_TICKER(2u, 0u, 0.4f, LERP_LINEAR)
        {
            const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);
            const coreVector2 vPos = (vDir + vDir.Rotated90() * 0.2f) * -1.2f * FOREGROUND_AREA * SQRT2;

            g_pGame->GetBulletManagerEnemy()->AddBullet<cGrowBullet>(5, 0.2f, this, vPos, vDir)->ChangeSize(1.0f)->AddStatus(BULLET_STATUS_IMMORTAL);
        });

        PHASE_CONTROL_TICKER(3u, 0u, fArrowSpeed, LERP_LINEAR)
        {
            pNevo->DisableArrow(0u, false);
            pNevo->EnableArrow (0u, this, StepRotated90((iTick + 3u) % 4u).InvertedX());
        });

        pAter->SetCollEnemyBullet([COLL_VAL(pNevo)](const cEnemy* pEnemy, cBullet* OUTPUT pBullet, const coreVector3 vIntersection, const coreBool bFirstHit)   // on base mission
        {
            if(!bFirstHit) return;

            const coreVector2 vSide = AlongCrossNormal(pBullet->GetFlyDir());
            const coreUint8   iPack = PackDirection(vSide);

            if(iPack != pNevo->GetArrowDir(0u))
            {
                pBullet->Deactivate(true);
                pBullet->AddStatus(BULLET_STATUS_GHOST);
            }
        });

        if(this->ReachedDeath())
        {
            PHASE_CHANGE_INC

            for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
                pNevo->DisableArrow(i, true);

            pAter->ResetCollEnemyBullet();

            this->__EndExplosion(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 125u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);

            if(PHASE_FINISHED)
            {
                this->__EndMission(true, true);
            }
        });
    }

    // ################################################################
    // ################################################################

    for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
    {
        const cLodObject* pBomb = pNevo->GetBomb(i);

        if(pNevo->GetBombGone(i))
        {
            const coreVector2 vPos = pBomb->GetPosition ().xy();
            const coreVector2 vDir = pBomb->GetDirection().xy();

            g_pGame->ForEachPlayer([&](cPlayer* OUTPUT pPlayer, const coreUintW i)
            {
                const coreVector2 vDiff = MapToAxisInv(pPlayer->GetPosition().xy() - vPos, vDir);

                if((ABS(vDiff.x) < 3.3f) || (ABS(vDiff.y) < 3.3f))
                {
                    if(pPlayer->IsNormal()) pPlayer->TakeDamage(10, ELEMENT_ORANGE, pPlayer->GetPosition().xy());
                }
            });
        }
    }

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cGrowBullet>([](cGrowBullet* OUTPUT pBullet)
    {
        if(pBullet->GetFlyTime() >= 10.0f) pBullet->RemoveStatus(BULLET_STATUS_IMMORTAL);
    });

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cTriangleBullet>([&](cTriangleBullet* OUTPUT pBullet)
    {
        const coreVector2 vFlyDir = pBullet->GetFlyDir();

        coreVector2 vNewPos = pBullet->GetPosition().xy();

             if((vNewPos.x < -FOREGROUND_AREA.x * 1.25f) && (vFlyDir.x < 0.0f)) vNewPos.x += FOREGROUND_AREA.x * 2.5f;
        else if((vNewPos.x >  FOREGROUND_AREA.x * 1.25f) && (vFlyDir.x > 0.0f)) vNewPos.x -= FOREGROUND_AREA.x * 2.5f;
             if((vNewPos.y < -FOREGROUND_AREA.y * 1.25f) && (vFlyDir.y < 0.0f)) vNewPos.y += FOREGROUND_AREA.y * 2.5f;
        else if((vNewPos.y >  FOREGROUND_AREA.y * 1.25f) && (vFlyDir.y > 0.0f)) vNewPos.y -= FOREGROUND_AREA.y * 2.5f;

        pBullet->SetPosition(coreVector3(vNewPos, 0.0f));
    });

    Core::Manager::Object->TestCollision(TYPE_BULLET_PLAYER, TYPE_BULLET_ENEMY, [](const cBullet* pBulletPlayer, cBullet* OUTPUT pBulletEnemy, const coreVector3 vIntersection, const coreBool bFirstHit)
    {
        if(pBulletEnemy->GetID() != cTriangleBullet::ID) return;

        pBulletEnemy->Deactivate(true, vIntersection.xy(), pBulletPlayer->GetFlyDir());

        g_pSpecialEffects->PlaySound(vIntersection, 1.0f, 1.0f, SOUND_PLACEHOLDER);
    });
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveGreen()
{
    ASSERT((m_iPhase >= 130u) && (m_iPhase <= 139u))

    cAterMission*   pAter   = d_cast<cAterMission*>  (g_pGame->GetCurMission());
    cViridoMission* pVirido = d_cast<cViridoMission*>(pAter->GetInnerMission());

    // ################################################################
    // 
    if(m_iPhase == 130u)
    {
        PHASE_CONTROL_PAUSE(0u, 1.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 131u)
    {
        if(PHASE_BEGINNING2)
        {
            pVirido->EnableLaser(0u, this);
            pVirido->EnableLaser(1u, this);

            pVirido->SetLaserCross(false);

            m_avVector[LASER_ROTA].x = 0.5f*PI;
            m_avVector[LASER_ROTA].y = 0.5f*PI;

            for(coreUintW j = 100u; j--; ) g_pSpecialEffects->CreateSplashColor(this->GetPosition() + coreVector3(1.0f,0.0f,0.0f) * (2.0f * (I_TO_F(j) - 49.5f)), 10.0f, 1u, COLOR_ENERGY_PURPLE);
        }

        const coreFloat fAccel = MIN1(m_fPhaseTime * 0.3f) * 4.0f;
        const coreFloat fSpeed = LERP(0.5f, 1.6f, STEP(300.0f, 1300.0f, I_TO_F(this->GetLostHealth())));

        m_avVector[LASER_ROTA].x += TIME *  0.2f * fAccel * fSpeed;
        m_avVector[LASER_ROTA].y += TIME * -0.2f * fAccel * fSpeed;

        pVirido->GetLaser(0u)->SetPosition(this->GetPosition());
        pVirido->GetLaser(0u)->SetDirection(coreVector3(coreVector2::Direction(m_avVector[LASER_ROTA].x), 0.0f));

        pVirido->GetLaser(1u)->SetPosition(this->GetPosition());
        pVirido->GetLaser(1u)->SetDirection(coreVector3(coreVector2::Direction(m_avVector[LASER_ROTA].y), 0.0f));

        PHASE_CONTROL_TICKER(0u, 0u, 0.7f * fSpeed, LERP_LINEAR)
        {
            for(coreUintW j = 8u; j--; )
            {
                if((j % 2u) == (iTick % 2u)) continue;

                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(DEG_TO_RAD(I_TO_F(j) * 45.0f));

                g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>(5, 0.4f, this, vPos, vDir)->ChangeSize(1.7f);
            }
        });

        if(this->GetLostHealth() >= 1600)
        {
            PHASE_CHANGE_INC

            pVirido->DisableLaser(0u, true);
            pVirido->DisableLaser(1u, true);

            pVirido->SetLaserCross(true);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 132u)
    {
        PHASE_CONTROL_PAUSE(0u, 2.0f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 133u)
    {
        if(PHASE_BEGINNING2)
        {
            this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * 1.0f);
            this->AddStatus(ENEMY_STATUS_DAMAGING);
        }

        if(!m_aiCounter[CHARGE_COUNT])
        {
            m_avVector[CHARGE_SPEED].x += 80.0f * TIME;
        }

        const coreVector2 vCurDir = this->GetDirection().xy();
        const coreVector2 vCurPos = this->GetPosition ().xy() + vCurDir * ((20.0f + m_avVector[CHARGE_SPEED].x) * TIME);

        this->SetPosition(coreVector3(vCurPos, 0.0f));

        if(g_pForeground->IsVisiblePoint(this->GetPosition().xy()))
        {
            PHASE_CONTROL_TICKER(0u, 0u, 30.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = this->AimAtPlayerDual(1u).Normalized();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>(5, 0.5f, this, vPos, vDir)->ChangeSize(1.3f)->AddStatus(BULLET_STATUS_IMMORTAL);
            });
        }

        if(((vCurPos.y < -FOREGROUND_AREA.y * 1.3f) && (vCurDir.y < 0.0f)) ||
           ((vCurPos.y >  FOREGROUND_AREA.y * 1.3f) && (vCurDir.y > 0.0f)))
        {
            m_aiCounter[CHARGE_COUNT] += 1;

            m_avVector[CHARGE_SPEED].x = 80.0f + 2.0f * I_TO_F(m_aiCounter[CHARGE_COUNT]);

            const coreFloat   fSide   = this->NearestPlayerDual(0u)->GetPosition().x;
            const coreVector2 vNewPos = coreVector2(fSide, 1.3f * FOREGROUND_AREA.y);

            this->SetPosition (coreVector3(vNewPos,   0.0f));
            this->SetDirection(coreVector3(0.0f,-1.0f,0.0f));
        }

        if(m_aiCounter[CHARGE_COUNT] >= 12)
        {
            PHASE_CHANGE_INC

            this->SetPosition(coreVector3(HIDDEN_POS, 0.0f));

            this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * PROJECTONE_COLL_SCALE);
            this->RemoveStatus(ENEMY_STATUS_DAMAGING);
        }
    }

    // ################################################################
    // 
    else if(m_iPhase == 134u)
    {
        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 135u)
    {
        PHASE_CONTROL_TIMER(0u, 0.6f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                for(coreUintW i = 0u; i < 12u; ++i)
                {
                    pVirido->EnableBarrier((i % 6u) + ((i >= 6u) ? VIRIDO_BARRIERS_FIRST : 0u), VIRIDO_BARRIER_FREE, coreVector2(0.0f,1.0f), 2.05f);
                }
            }

            for(coreUintW i = 0u; i < 12u; ++i)
            {
                coreObject3D* pBarrier = pVirido->GetBarrier((i % 6u) + ((i >= 6u) ? VIRIDO_BARRIERS_FIRST : 0u));

                coreVector2 vPos = coreVector2(I_TO_F(i % 2u) - 0.5f, I_TO_F((i / 2u) % 3u) - 1.0f) * 0.75f;
                if(i >= 6u) vPos = vPos.Rotated90();

                coreVector2 vDir = coreVector2(((i % 2u) == ((i / 2u) % 2u)) ? -1.0f : 1.0f, 0.0f);
                if(i >= 6u) vDir = vDir.Rotated90();

                const coreFloat fLerp = 1.0f - BLENDB(CLAMP01((fTime - I_TO_F(i / 6u) * 0.4f) / 0.6f));

                pBarrier->SetPosition (coreVector3(vPos * FOREGROUND_AREA - vDir * (2.7f - fLerp * 70.0f), 0.0f));
                pBarrier->SetDirection(coreVector3(vDir, 0.0f));
            }

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->StorePosition(coreVector2(0.0f,1.3f) * FOREGROUND_AREA);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 136u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            if(PHASE_BEGINNING)
            {
                this->ChangeToTop();

                pVirido->EnableShadow(0u, this, m_avVector[JUMP_TARGET].xy(), false);
            }

            const coreFloat   fSin    = SIN(fTime * (1.0f*PI));
            const coreVector2 vPos    = LERP(m_vLastPosition * FOREGROUND_AREA, m_avVector[JUMP_TARGET].xy(), fTime) * (1.0f - 0.45f * fSin);
            const coreFloat   fHeight = 50.0f * fSin;

            this->SetPosition (coreVector3(vPos, fHeight));
            this->SetDirection(coreVector3(coreVector2::Direction(fTime * (-4.0f*PI)), 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                this->ChangeToNormal();

                pVirido->DisableShadow(0u, true);
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 137u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            this->SetDirection(coreVector3(coreVector2::Direction(fTime * (6.0f*PI)), 0.0f));

            PHASE_CONTROL_TICKER(1u, 0u, 24.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition ().xy();
                const coreVector2 vDir = this->GetDirection().xy();

                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.5f, this, vPos,  vDir)->ChangeSize(1.5f);
                g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>(5, 0.5f, this, vPos, -vDir)->ChangeSize(1.5f);
            });

            if(PHASE_FINISHED)
            {
                m_avVector[JUMP_TARGET].xy(StepRotated45((m_aiCounter[JUMP_COUNT] * 3u) % 8u).Processed(SIGNUM) * 0.75f * FOREGROUND_AREA);

                m_aiCounter[JUMP_COUNT] += 1;

                PHASE_CHANGE_TO(136u)
                PHASE_RESET(1u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 138u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);

            if(PHASE_FINISHED)
            {
                this->__EndMission(true, true);
            }
        });
    }

    // ################################################################
    // ################################################################

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(138u)

        this->__EndExplosion(true);
    }

    const coreVector2 vMove = this->NearestPlayerDual(1u)->GetMove() * -0.5f;

    g_pGame->GetBulletManagerEnemy()->ForEachBulletTyped<cQuadBullet>([&](cQuadBullet* OUTPUT pBullet)
    {
        const coreVector2 vNewPos = pBullet->GetPosition().xy() + vMove;

        pBullet->SetPosition(coreVector3(vNewPos, 0.0f));
    });

    g_pGame->GetBulletManagerPlayer()->ForEachBullet([](cBullet* OUTPUT pBullet)
    {
        if(pBullet->GetFlyTime() >= 1.0f) pBullet->Deactivate(true);
    });
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveWhite()
{
    ASSERT((m_iPhase >= 140u) && (m_iPhase <= 149u))

    coreVector3 vEnergyColor, vBlockColor, vLevelColor;
    cProjectOneBoss::CalcColorLerp(m_fLifeTime, &vEnergyColor, &vBlockColor, &vLevelColor);

    this->__SetEnergyColor(vEnergyColor);

    m_Bubble.SetColor3(vEnergyColor);

    m_vColorTo    = vBlockColor;
    m_vLevelColor = vLevelColor;

    // ################################################################
    // 
    if(m_iPhase == 140u)
    {
        if(PHASE_BEGINNING2)
        {
            g_pEnvironment->SetTargetSpeed(2.0f, 0.2f);

            constexpr coreUint8 aiNewOrder[] = {cSpearBullet::ID, cConeBullet::ID, cTriangleBullet::ID, cViewBullet::ID, cFlipBullet::ID, cOrbBullet::ID, cQuadBullet::ID, cWaveBullet::ID};
            g_pGame->GetBulletManagerEnemy()->OverrideOrder(aiNewOrder, ARRAY_SIZE(aiNewOrder));
        }

        PHASE_CONTROL_PAUSE(0u, 0.5f)
        {
            PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    if(m_iPhase == 141u)
    {
        PHASE_CONTROL_TIMER(0u, 0.3f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,0.7f), fTime);

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 142u)
    {
        const coreFloat fValue = STEP(0.0f, 60.0f, m_fPhaseTime);

        this->SetCurHealthPct(0.05f + 0.95f * (1.0f - fValue));

        // flower
        if(fValue < 0.2f)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 1.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();

                for(coreUintW j = 72u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) / 72.0f * (2.0f*PI) - I_TO_F(iTick * 3u) * GA);

                    const coreFloat fSpeed = 1.0f + 0.5f * SIN(I_TO_F(j) / 72.0f * (6.0f*PI));

                    this->__AddRainbowBullet((j + (iTick * 3u)) % 8u, 1, fSpeed, vPos, vDir);
                }
            });
        }

        // whirl
        else if(fValue < 0.4f)
        {
            m_fFinalLerp.Update(LERP(1.0f, 3.0f, STEP(0.2f, 0.4f, fValue)));

            PHASE_CONTROL_TICKER(1u, 0u, 10.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();

                for(coreUintW j = 8u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) / 8.0f * (2.0f*PI) + m_fFinalLerp);

                    this->__AddRainbowBullet((j + iTick) % 8u, 2, 1.0f, vPos, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }

        // lines
        else if(fValue < 0.6f)
        {
            m_fFinalLerp.Update(LERP(0.0f, -1.5f, STEP(0.4f, 0.6f, fValue)));

            PHASE_CONTROL_TICKER(1u, 0u, 15.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();

                for(coreUintW j = 12u; j--; )
                {
                    if((j % 2u) == ((iTick / 6u) % 2u)) continue;

                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) / 12.0f * (2.0f*PI) - m_fFinalLerp);

                    this->__AddRainbowBullet((j + iTick) % 8u, 3, 1.5f, vPos, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }

        // random back and forth
        else if(fValue < 0.829f)
        {
            PHASE_CONTROL_TICKER(1u, 0u, 30.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();
                const coreVector2 vDir = coreVector2::Direction(I_TO_F(iTick) * GA);

                this->__AddRainbowBullet((iTick * 5u) % 8u, 4, 1.5f, vPos, vDir)->AddStatus(BULLET_STATUS_IMMORTAL);
            });
        }

        // final wave
        else
        {
            PHASE_CONTROL_TICKER(1u, 1u, 1.0f/5.0f, LERP_LINEAR)
            {
                const coreVector2 vPos = this->GetPosition().xy();

                for(coreUintW j = 40u; j--; )
                {
                    const coreVector2 vDir = coreVector2::Direction(I_TO_F(j) / 160.0f * (2.0f*PI));

                    this->__AddRainbowBullet(j % 8u, 5, 2.0f, vPos,  vDir)            ->AddStatus(BULLET_STATUS_IMMORTAL);
                    this->__AddRainbowBullet(j % 8u, 5, 2.0f, vPos, -vDir)            ->AddStatus(BULLET_STATUS_IMMORTAL);
                    this->__AddRainbowBullet(j % 8u, 5, 2.0f, vPos,  vDir.Rotated90())->AddStatus(BULLET_STATUS_IMMORTAL);
                    this->__AddRainbowBullet(j % 8u, 5, 2.0f, vPos, -vDir.Rotated90())->AddStatus(BULLET_STATUS_IMMORTAL);
                }
            });
        }

        const coreMatrix2 mRota1 = coreMatrix3::Rotation(TIME * 0.3f).m12();
        const coreMatrix2 mRota2 = coreMatrix3::Rotation(TIME * LERP(0.0f, -1.5f, STEP(0.4f, 0.6f, fValue))).m12();

        g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
        {
            const coreInt32 iDamage = pBullet->GetDamage();
            if((iDamage != 2) && (iDamage != 3)) return;

            const coreMatrix2 mRota = (iDamage == 2) ? mRota1 : mRota2;
            const coreVector2 vDiff = pBullet->GetPosition().xy() - pBullet->GetOwner()->GetPosition().xy();
            const coreVector2 vPos  = pBullet->GetOwner()->GetPosition().xy() + vDiff * mRota;
            const coreVector2 vDir  = pBullet->GetFlyDir() * mRota;

            pBullet->SetPosition(coreVector3(vPos, 0.0f));
            pBullet->SetFlyDir  (vDir);

            if(vDiff.LengthSq() >= (FOREGROUND_AREA * 2.0f).LengthSq()) pBullet->Deactivate(false);
        });

        PHASE_CONTROL_TICKER(2u, 0u, FRAMERATE_MIN, LERP_LINEAR)
        {
            g_pGame->GetBulletManagerEnemy()->ForEachBullet([&](cBullet* OUTPUT pBullet)
            {
                const coreInt32 iDamage = pBullet->GetDamage();
                if((iDamage != 4) && (iDamage != 5)) return;

                const coreVector2 vDiff  = pBullet->GetPosition().xy() - pBullet->GetOwner()->GetPosition().xy();
                const coreFloat   fSpeed = pBullet->GetSpeed() - ((iDamage == 4) ? 0.4f : 0.75f) * (1.0f / FRAMERATE_MIN);

                pBullet->SetSpeed(fSpeed);

                if((pBullet->GetFlyTime() >= 3.0f) && (vDiff.LengthSq() <= 2.0f)) pBullet->Deactivate(false);
            });
        });

        if(fValue >= 1.0f)
        {
            PHASE_CHANGE_TO(11u)

            this->__SwitchColor(9u, false);

            g_pSpecialEffects->MacroExplosionDarkBig(this->GetPosition());

            g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

            this->_EndBoss();
        }
    }

    // ################################################################
    // ################################################################
}


// ****************************************************************
// 
void cProjectOneBoss::__MoveIntro()
{
    ASSERT((m_iPhase >= 150u) && (m_iPhase <= 159u))

    // ################################################################
    // 
    if(m_iPhase == 150u)
    {
        PHASE_CONTROL_TIMER(0u, 2.0f, LERP_SMOOTH)
        {
            this->DefaultMoveLerp(m_vLastPosition, coreVector2(0.0f,1.0f), fTime);

            if(PHASE_TIME_POINT(0.5f))
            {
                this->SetDirection(coreVector3(-1.0f,0.0f,0.0f));
            }

            if(PHASE_FINISHED)
                PHASE_CHANGE_INC
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 151u)
    {
        PHASE_CONTROL_TIMER(0u, 0.5f, LERP_LINEAR)
        {
            const coreVector2 vBase = StepRotated90(m_aiCounter[SIDE_COUNT] % 4);
            const coreVector2 vDir  = vBase.Rotated90() * (((m_aiCounter[SIDE_COUNT] / 3) % 2) ? 1.0f : -1.0f);

            this->DefaultMoveLerp(vBase * 1.0f, vBase * -1.0f, fTime);
            this->SetDirection   (coreVector3(MapToAxis(vDir, coreVector2::Direction(fTime * (1.0f*PI))), 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_INC

                m_aiCounter[SIDE_COUNT] += 1;
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 152u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_LINEAR)
        {
            const coreInt32 iCountPrev = m_aiCounter[SIDE_COUNT] - 1;

            const coreVector2 vBase1 = StepRotated90(iCountPrev              % 4);
            const coreVector2 vBase2 = StepRotated90(m_aiCounter[SIDE_COUNT] % 4);
            const coreVector2 vDir1  = vBase1.Rotated90() * (((iCountPrev              / 3) % 2) ? 1.0f : -1.0f);
            const coreVector2 vDir2  = vBase2.Rotated90() * (((m_aiCounter[SIDE_COUNT] / 3) % 2) ? 1.0f : -1.0f);

            this->DefaultMoveLerp(vBase1 * -1.0f, vBase2 * 1.0f, fTime);
            this->SetDirection   (coreVector3((fTime < 0.5f) ? -vDir1 : vDir2, 0.0f));

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(151u)
            }
        });
    }

    // ################################################################
    // 
    else if(m_iPhase == 153u)
    {
        PHASE_CONTROL_TIMER(0u, 1.0f, LERP_BREAK)
        {
            this->__EndAnimation(fTime);

            if(PHASE_FINISHED)
            {
                PHASE_CHANGE_TO(30u)
            }
        });
    }

    // ################################################################
    // ################################################################

    if((m_iPhase > 150u) && (this->GetCurHealth() > 0))
    {
        PHASE_CONTROL_TICKER(1u, 0u, 20.0f, LERP_LINEAR)
        {
            const coreVector2 vPos = this->GetPosition ().xy();
            const coreVector2 vDir = this->GetDirection().xy();
            const coreVector2 vTan = vDir.Rotated90() * 1.5f;

            g_pGame->GetBulletManagerEnemy()->AddBullet<cRayBullet>(1, 8.0f, this, vPos + vTan, vDir)->MakeWhite();
            g_pGame->GetBulletManagerEnemy()->AddBullet<cRayBullet>(1, 8.0f, this, vPos - vTan, vDir)->MakeWhite();
        });
    }

    if(this->ReachedDeath())
    {
        PHASE_CHANGE_TO(153u)

        this->__EndExplosion(true);
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__ShowArrow()
{
    // 
    if(m_fArrowValue <= 0.0f) this->__EnableArrow();
    m_fArrowValue = 1.0f;
}


// ****************************************************************
// 
void cProjectOneBoss::__StartFeeling()
{
    // 
    this->AddStatus(ENEMY_STATUS_GHOST);

    // 
    this->__EnableBubble();
}


// ****************************************************************
// 
void cProjectOneBoss::__EndFeeling()
{
    // 
    this->RemoveStatus(ENEMY_STATUS_GHOST);

    // 
    this->__DisableBubble();
}


// ****************************************************************
// 
void cProjectOneBoss::__EnableRange()
{
    WARN_IF(m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableRange();

    // 
    m_Range.SetAlpha(0.0f);

    // 
    m_Range.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Range);
}


// ****************************************************************
// 
void cProjectOneBoss::__DisableRange()
{
    if(!m_Range.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Range.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Range);
}


// ****************************************************************
// 
void cProjectOneBoss::__EnableArrow()
{
    WARN_IF(m_Arrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableArrow();

    // 
    m_Arrow.SetAlpha(0.0f);

    // 
    m_Arrow.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Arrow);
}


// ****************************************************************
// 
void cProjectOneBoss::__DisableArrow()
{
    if(!m_Arrow.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Arrow.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Arrow);
}


// ****************************************************************
// 
void cProjectOneBoss::__EnableWind()
{
    WARN_IF(m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableWind();

    // 
    m_Wind.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Wind);
}


// ****************************************************************
// 
void cProjectOneBoss::__DisableWind()
{
    if(!m_Wind.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Wind.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Wind);
}


// ****************************************************************
// 
void cProjectOneBoss::__EnableBubble()
{
    WARN_IF(m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_ALL)) this->__DisableBubble();

    // 
    m_Bubble.SetAlpha(0.0f);

    // 
    m_Bubble.SetEnabled(CORE_OBJECT_ENABLE_ALL);
    g_pGlow->BindObject(&m_Bubble);
}


// ****************************************************************
// 
void cProjectOneBoss::__DisableBubble()
{
    if(!m_Bubble.IsEnabled(CORE_OBJECT_ENABLE_ALL)) return;

    // 
    m_Bubble.SetEnabled(CORE_OBJECT_ENABLE_NOTHING);
    g_pGlow->UnbindObject(&m_Bubble);
}


// ****************************************************************
// 
void cProjectOneBoss::__UpdateExhaust(const coreFloat fStrength)
{
    // 
    const coreFloat fLen  = fStrength * 40.0f;
    const coreFloat fSize = 1.0f - fStrength * 0.25f;

    // 
         if( fStrength && !m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) g_pGlow->BindObject  (&m_Exhaust);
    else if(!fStrength &&  m_Exhaust.IsEnabled(CORE_OBJECT_ENABLE_ALL)) g_pGlow->UnbindObject(&m_Exhaust);

    // 
    m_Exhaust.SetSize     (coreVector3(fSize, fLen, fSize) * 0.6f * this->GetSize().x);
    m_Exhaust.SetTexOffset(coreVector2(0.0f, m_fAnimation * 0.75f));
    m_Exhaust.SetPosition (this->GetPosition () - this->GetDirection() * (m_Exhaust.GetSize().y + 4.0f * this->GetSize().x));
    m_Exhaust.SetDirection(this->GetDirection());
    m_Exhaust.SetEnabled  (fStrength ? CORE_OBJECT_ENABLE_ALL : CORE_OBJECT_ENABLE_NOTHING);
    m_Exhaust.Move();
}


// ****************************************************************
// 
void cProjectOneBoss::__SetEnergyColor(const coreVector3 vColor)
{
    m_Range  .SetColor3(vColor);
    m_Arrow  .SetColor3(vColor * (0.9f/1.1f));
    m_Wind   .SetColor3(vColor * (1.6f/1.1f));
    m_Exhaust.SetColor3(vColor);
}


// ****************************************************************
// 
void cProjectOneBoss::__SwitchHealth(const coreUintW iIndex)
{
    // 
    coreInt32 iHealth;
    switch(iIndex)
    {
    default: ASSERT(false)
    case 0u: iHealth = 2000;                    break;   // yellow
    case 1u: iHealth = 1800;                    break;   // orange
    case 2u: iHealth = 2200 + 90 * 6 + 850 * 4; break;   // red
    case 3u: iHealth = 3300;                    break;   // magenta
    case 4u: iHealth = 4400;                    break;   // purple
    case 5u: iHealth = 2400;                    break;   // blue
    case 6u: iHealth = 1800;                    break;   // cyan
    case 7u: iHealth = 3100;                    break;   // green
    case 8u: iHealth = 10000;                   break;   // white
    case 9u: iHealth = 600;                     break;   // intro
    }

    // 
    this->SetMaxHealth(iHealth);
    this->SetCurHealth(iHealth);

    // 
    this->RefreshColor(1.0f);
}


// ****************************************************************
// 
void cProjectOneBoss::__SwitchColor(const coreUintW iIndex, const coreBool bWave)
{
    coreVector3 vEnergyColor, vBlockColor, vLevelColor;
    cProjectOneBoss::CalcColor(iIndex, &vEnergyColor, &vBlockColor, &vLevelColor);

    // 
    this->__SetEnergyColor(vEnergyColor);

    // 
    m_vLevelColor = vLevelColor;

    if(bWave)
    {
        m_fWaveValue = 0.1f;

        m_vColorFrom = m_vColorTo;
        m_vColorTo   = vBlockColor;

        m_fPatternValue = 0.0f;
        m_iPatternType  = iIndex + 1u;
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__StartMission(const coreUintW iIndex)
{
    cAterMission* pAter = d_cast<cAterMission*>(g_pGame->GetCurMission());

    // 
    switch(iIndex)
    {
    default: ASSERT(false)
    case 0u: pAter->RequestInnerMission(cHarenaMission ::ID); PHASE_CHANGE_TO( 50u) break;   // yellow
    case 1u: pAter->RequestInnerMission(cGeluMission   ::ID); PHASE_CHANGE_TO( 60u) break;   // orange
    case 2u: pAter->RequestInnerMission(cMuscusMission ::ID); PHASE_CHANGE_TO( 70u) break;   // red
    case 3u: pAter->RequestInnerMission(cRutilusMission::ID); PHASE_CHANGE_TO( 80u) break;   // magenta
    case 4u: pAter->RequestInnerMission(cNoMission     ::ID); PHASE_CHANGE_TO( 90u) break;   // purple
    case 5u: pAter->RequestInnerMission(cCalorMission  ::ID); PHASE_CHANGE_TO(100u) break;   // blue
    case 6u: pAter->RequestInnerMission(cNevoMission   ::ID); PHASE_CHANGE_TO(120u) break;   // cyan
    case 7u: pAter->RequestInnerMission(cViridoMission ::ID); PHASE_CHANGE_TO(130u) break;   // green
    case 8u: pAter->RequestInnerMission(cNoMission     ::ID); PHASE_CHANGE_TO(140u) break;   // white
    case 9u: pAter->RequestInnerMission(cNoMission     ::ID); PHASE_CHANGE_TO(150u) break;   // intro
    }
}


// ****************************************************************
// 
void cProjectOneBoss::__EndMission(const coreBool bAnimated, const coreBool bReturn)
{
    cDarkBackground* pBackground = d_cast<cDarkBackground*>(g_pEnvironment->GetBackground());

    cAterMission* pAter  = d_cast<cAterMission*>(g_pGame->GetCurMission());
    cMission*     pInner = pAter->GetInnerMission();

    // 
    const coreInt32 iID = pInner->GetID();

    // 
    if(iID == cViridoMission::ID)
    {
        cViridoMission* pVirido = d_cast<cViridoMission*>(pInner);

        // 
        for(coreUintW i = 0u; i < VIRIDO_BARRIERS; ++i)
            pVirido->DisableBarrier(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < VIRIDO_LASERS; ++i)
            pVirido->DisableLaser(i, bAnimated);
    }
    else if(iID == cNevoMission::ID)
    {
        cNevoMission* pNevo = d_cast<cNevoMission*>(pInner);

        // 
        for(coreUintW i = 0u; i < NEVO_BOMBS; ++i)
            pNevo->DisableBomb(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < NEVO_BLASTS; ++i)
            pNevo->DisableBlast(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < NEVO_TILES; ++i)
            pNevo->DisableTile(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < NEVO_ARROWS; ++i)
            pNevo->DisableArrow(i, bAnimated);

    }
    else if(iID == cHarenaMission::ID)
    {
        cHarenaMission* pHarena = d_cast<cHarenaMission*>(pInner);

        // 
        for(coreUintW i = 0u; i < HARENA_SPIKES; ++i)
            pHarena->DisableSpike(i, bAnimated);
    }
    else if(iID == cRutilusMission::ID)
    {
        cRutilusMission* pRutilus = d_cast<cRutilusMission*>(pInner);

        // 
        for(coreUintW i = 0u; i < RUTILUS_PLATES; ++i)
            pRutilus->DisablePlate(i, bAnimated);

        // 
        pRutilus->DisableArea(bAnimated);

        // 
        pRutilus->DisableWave(bAnimated);
    }
    else if(iID == cGeluMission::ID)
    {
        cGeluMission* pGelu = d_cast<cGeluMission*>(pInner);

        // 
        for(coreUintW i = 0u; i < GELU_FANGS; ++i)
            pGelu->DisableFang(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < GELU_WAYS; ++i)
            pGelu->DisableWay(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < GELU_ORBS; ++i)
            pGelu->DisableOrb(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < GELU_LINES; ++i)
            pGelu->DisableLine(i, bAnimated);
    }
    else if(iID == cCalorMission::ID)
    {
        cCalorMission* pCalor = d_cast<cCalorMission*>(pInner);

        // 
        for(coreUintW i = 0u; i < CALOR_STARS; ++i)
            pCalor->UncatchObject(i);

        // 
        for(coreUintW i = 0u; i < CALOR_STARS; ++i)
            pCalor->DisableStar(i, bAnimated);

        // 
        pCalor->DisableSnow(bAnimated);

        // 
        pCalor->GetBoulder()->Kill(bAnimated);
    }
    else if(iID == cMuscusMission::ID)
    {
        cMuscusMission* pMuscus = d_cast<cMuscusMission*>(pInner);

        // 
        for(coreUintW i = 0u; i < MUSCUS_GENERATES; ++i)
            pMuscus->DisableGenerate(i, bAnimated);

        // 
        for(coreUintW i = 0u; i < MUSCUS_PEARLS; ++i)
            pMuscus->DisablePearl(i, bAnimated);
    }
    else if(iID == cNoMission::ID)
    {
        // nothing
    }
    else ASSERT(false)

    // 
    this->SetCollisionModifier(coreVector3(1.0f,1.0f,1.0f) * PROJECTONE_COLL_SCALE);

    // 
    pAter->ResetCollEnemyBullet();   // on base mission

    // 
    pAter->GetEnemySquad(0u)->ClearEnemies(bAnimated);
    pAter->GetEnemySquad(1u)->ClearEnemies(bAnimated);

    // 
    pBackground->GetHeadlight()->SetAlpha(0.0f);

    // 
    for(coreUintW i = 0u; i < POST_WALLS; ++i)
        g_pPostProcessing->SetWallOffset(i, 0.0f);

    // 
    g_pPostProcessing->SetDirectionGame(coreVector2(0.0f,1.0f));

    // 
    g_pGame->ForEachPlayerAll([](cPlayer* OUTPUT pPlayer, const coreUintW i)
    {
        pPlayer->SetDirection (coreVector3(AlongCrossNormal(pPlayer->GetDirection().xy()), 0.0f));
        pPlayer->SetArea      (PLAYER_AREA_DEFAULT);
        pPlayer->SetMoveSpeed (1.0f);
        pPlayer->SetShootSpeed(1.0f);
        pPlayer->SetAnimSpeed (1.0f);
        pPlayer->RemoveStatus (PLAYER_STATUS_NO_INPUT_MOVE);
    });

    // 
    g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

    // 
    g_pGame->GetBulletManagerEnemy()->ResetOrder();

    // 
    if(bReturn) PHASE_CHANGE_TO(10u)
}


// ****************************************************************
// 
void cProjectOneBoss::__EndExplosion(const coreBool bClear)
{
    // 
    this->__StartFeeling();

    // 
    if(bClear) g_pGame->GetBulletManagerEnemy()->ClearBullets(true);

    // 
    g_pSpecialEffects->MacroExplosionDarkBig (this->GetPosition());
}


// ****************************************************************
// 
void cProjectOneBoss::__EndAnimation(const coreFloat fTime)
{
    const coreFloat fFrom = m_vLastDirection.xy().Angle();
    const coreFloat fTo   = fFrom + AngleDiff(1.0f*PI, fFrom) - (4.0f*PI);

    this->DefaultMoveLerp  (m_vLastPosition, coreVector2(0.0f,0.0f), fTime);
    this->DefaultRotateLerp(fFrom,           fTo,                    fTime);
}


// ****************************************************************
// 
cBullet* cProjectOneBoss::__AddRainbowBullet(const coreUintW iType, const coreInt32 iDamage, const coreFloat fSpeed, const coreVector2 vPos, const coreVector2 vDir)
{
    switch(iType)
    {
    default: ASSERT(false)
    case 0u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cSpearBullet>   (iDamage, fSpeed, this, vPos - 0.7f * vDir, vDir)->ChangeSize(1.5f);
    case 1u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cConeBullet>    (iDamage, fSpeed, this, vPos,               vDir)->ChangeSize(1.6f);
    case 2u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cTriangleBullet>(iDamage, fSpeed, this, vPos,               vDir)->ChangeSize(1.3f);
    case 3u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cViewBullet>    (iDamage, fSpeed, this, vPos - 2.0f * vDir, vDir)->ChangeSize(1.5f);
    case 4u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cFlipBullet>    (iDamage, fSpeed, this, vPos,               vDir)->ChangeSize(1.5f);
    case 5u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cOrbBullet>     (iDamage, fSpeed, this, vPos,               vDir)->ChangeSize(2.1f);
    case 6u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cQuadBullet>    (iDamage, fSpeed, this, vPos,               vDir)->ChangeSize(1.4f);
    case 7u: return g_pGame->GetBulletManagerEnemy()->AddBullet<cWaveBullet>    (iDamage, fSpeed, this, vPos,               vDir)->ChangeSize(1.5f);
    }
}